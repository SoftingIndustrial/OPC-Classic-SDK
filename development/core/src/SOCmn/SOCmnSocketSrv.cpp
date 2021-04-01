#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnString.h>

#ifdef SOFEATURE_CMN_SOCKET

#ifdef SOFEATURE_CMN_SOCKET_SRV

//-----------------------------------------------------------------------------
// SOCmnSocketSrv                                                             |
//-----------------------------------------------------------------------------

SOCmnSocketSrv::SOCmnSocketSrv(IN WORD port)
{
	setObjectState(SOCMNSOCKET_STATE_INIT);
	m_socket = INVALID_SOCKET;
	m_port = port;
}

SOCmnSocketSrv::~SOCmnSocketSrv(void)
{
	stop();
	close();
}

//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//              trace id string
//
LPCTSTR SOCmnSocketSrv::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[SOS|%8.8lX| %u]"), (DWORD)this->getOpcHandle(), m_port);
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


LONG SOCmnSocketSrv::create(void)
{
	if (getObjectState() >= SOCMNSOCKET_STATE_CREATED)
	{
		return 0;
	}

	LONG ret = 0;
	m_socket = createSocket();

	if (m_socket != INVALID_SOCKET)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_family      = AF_INET;
		addr.sin_port        = htons(m_port);
#ifdef SOOS_LINUX
#ifndef SODEVENV_CYGWIN
		// SO_REUSEADDR is implemented differently:
		// linux allows the server to be restarted immediately. but no more than one process at the same time!
		// windows allows more than one process listening on the same socket at the same time!
		int opt_val = 1;
		setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
				   (const char*)&opt_val, sizeof(opt_val));
#endif
#endif

		if (0 != (ret = bind(m_socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in))))
		{
			close();
		}
		else
		{
			setObjectState(SOCMNSOCKET_STATE_CREATED);
		}
	}
	else
	{
		ret = getLastError();
	}

	return ret;
}

LONG SOCmnSocketSrv::create(IN WORD port)
{
	if (getObjectState() >= SOCMNSOCKET_STATE_CREATED)
	{
		return (m_port == port) ? 0 : 1;
	}

	m_port = port;
	return create();
}

LONG SOCmnSocketSrv::close(void)
{
	LONG ret = 0;

	if (m_socket != INVALID_SOCKET)
	{
		closeSocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	setObjectState(SOCMNSOCKET_STATE_INIT);
	return ret;
}

LONG SOCmnSocketSrv::listen(IN OPTIONAL int queueLen)
{
	if (getObjectState() != SOCMNSOCKET_STATE_CREATED)
	{
		return 0;
	}

	LONG ret = ::listen(m_socket, queueLen);

	if (ret == 0)
	{
		setObjectState(SOCMNSOCKET_STATE_LISTEN);
	}

	return ret;
}

BOOL SOCmnSocketSrv::start(void)
{
	BOOL ret = FALSE;

	if (m_rcvThread.isRunning())
	{
		return TRUE;
	}

	if (create() == 0)
	{
		if (listen() == 0)
		{
			if (createSocketPair(&m_sendtoRcvThread,
								 &m_controlRcvThread) == 0)
			{
				ret = m_rcvThread.start(rcvThreadProc, (void*)this);
			}
		}
	}

	if (!ret)
	{
		stop();
	}
	else
	{
		m_rcvThread.setPriority(THREAD_PRIORITY_HIGHEST);
	}

	return ret;
}

BOOL SOCmnSocketSrv::stop(void)
{
	if (getObjectState() < SOCMNSOCKET_STATE_LISTEN)
	{
		return TRUE;
	}

	BOOL ret;
	shutdown(m_sendtoRcvThread, SHUT_WR);
	ret = m_rcvThread.stop();
	closeSocket(m_sendtoRcvThread);
	closeSocket(m_controlRcvThread);
	m_sendtoRcvThread = INVALID_SOCKET;
	m_controlRcvThread = INVALID_SOCKET;
	close();
	return ret;
}


#define SOCMNSOCKETSRV_RCV_BUFFER_SIZE 16384

DWORD SOCmnSocketSrv::rcvThread(void)
{
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnListPosition pos;
	SOCmnSocketData* pElem;
	int sockCnt;
	DWORD dwTimeStart, dwTimeDiff, dwMaxWait;
	fd_set readfds, writefds;
	BYTE rcvBuffer[SOCMNSOCKETSRV_RCV_BUFFER_SIZE];
	LONG bytesRead;
	LONG bytesSent;
	SOCKET rcvSocket;
	SOCKET maxSocket;
	SOCKET hNewClient;
	struct sockaddr_in sockAddr;
	socklen_t sockAddrLen;
	_ASSERTION(m_rcvThread.isOwnThread(), "wrong thread?");

	for (;;)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(m_controlRcvThread, &readfds);
		FD_SET(m_socket, &readfds);
		maxSocket = __max(m_controlRcvThread, m_socket);
		pos = fdc.getStartPosition();
		dwTimeStart = GetTickCount();
		dwMaxWait = INFINITE;

		if (m_checkTimeOuts)
		{
			m_checkTimeOuts = FALSE;

			while (pos)
			{
				pElem = fdc.getNext(pos);

				if (pElem->m_timeOut != INFINITE)
				{
					dwTimeDiff = dwTimeStart - pElem->m_lastActive;

					if (dwTimeDiff < pElem->m_timeOut)
					{
						dwTimeDiff = pElem->m_timeOut - dwTimeDiff;

						if (dwTimeDiff < dwMaxWait)
						{
							dwMaxWait = dwTimeDiff;
						}
					}
					else
					{
						pElem->addRef();
						m_currentElem = pElem;
						fdc.autoUnlock(); /* remove possible deadlock */
						timeOut(pElem);
						fdc.autoLock();

						if (m_currentElem == NULL || m_checkTimeOuts)
						{
							m_currentElem = NULL;
							pElem->release();
							pos = fdc.getStartPosition();
							dwTimeStart = GetTickCount();
							dwMaxWait = INFINITE;
							m_checkTimeOuts = FALSE;
							continue; /* re-calculate all timeouts now */
						}

						m_currentElem = NULL;
						dwTimeDiff = GetTickCount() - dwTimeStart;
						dwTimeStart += dwTimeDiff;

						if (dwMaxWait != INFINITE)
							if (dwTimeDiff > dwMaxWait)
							{
								dwMaxWait = 0;
							}
							else
							{
								dwMaxWait -= dwTimeDiff;
							}

						if (pElem->m_timeOut != INFINITE && fdc.findKey(pElem->getOpcHandle()) == pElem)
						{
							/* pElem still active: re-calculate timeout... */
							dwTimeDiff = dwTimeStart - pElem->m_lastActive;

							if (dwTimeDiff < pElem->m_timeOut)
							{
								dwTimeDiff = pElem->m_timeOut - dwTimeDiff;
							}
							else
							{
								dwTimeDiff = 0;
							}

							if (dwTimeDiff < dwMaxWait)
							{
								dwMaxWait = dwTimeDiff;
							}

							pElem->release();
						}

						pElem->release();
					}
				}
			}

			if (dwMaxWait != INFINITE)
			{
				m_checkTimeOuts = TRUE;
			}
		}

		pos = fdc.getStartPosition();

		while (pos)
		{
			pElem = fdc.getNext(pos);
			rcvSocket = pElem->m_socket;
			maxSocket = __max(maxSocket, rcvSocket);
			FD_SET(rcvSocket, &readfds);

			if (pElem->m_cbData != 0)
			{
				FD_SET(rcvSocket, &writefds);
			}
		}

		fdc.autoUnlock();

		if (dwMaxWait != INFINITE)
		{
			struct timeval tv;
			dwTimeDiff = GetTickCount() - dwTimeStart;

			if (dwTimeDiff > dwMaxWait)
			{
				dwMaxWait = 0;
			}
			else
			{
				dwMaxWait -= dwTimeDiff;
			}

			tv.tv_sec = dwMaxWait / 1000;
			tv.tv_usec = 1000 * (dwMaxWait % 1000);
			sockCnt = select((int)(maxSocket + 1), &readfds, &writefds, NULL, &tv);
		}
		else
		{
			sockCnt = select((int)(maxSocket + 1), &readfds, &writefds, NULL, NULL);
		}

		switch (sockCnt)
		{
		case 0:
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("timeout")));
			break;

		case SOCKET_ERROR:
		{
			LONG err = getLastError();

			if (err == WSAEINTR)
			{
				continue;    // interrupted blocking call
			}

			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("select error [%lu]"), err));
			Sleep(10);
		}
		break;

		default:
		{
			pos = fdc.getStartPosition();

			while (pos)
			{
				pElem = fdc.getNext(pos);
				rcvSocket = pElem->m_socket;

				if (pElem->m_cbData != 0 && FD_ISSET(rcvSocket, &writefds))
				{
					bytesSent = send(rcvSocket, (char*)pElem->m_lpData,
									 pElem->m_cbData, MSG_NOSIGNAL);

					if ((bytesSent != SOCKET_ERROR) && (bytesSent != 0))
					{
						_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET,
							   (_T("[%lu] snd %lu queued bytes"),
								(DWORD)rcvSocket, bytesSent));
						_TRACEASCIISTRING(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET_DATA, bytesSent, (CHAR*)pElem->m_lpData);
						pElem->m_lastActive = GetTickCount();
						pElem->m_cbData -= bytesSent;
						memmove(pElem->m_lpData, pElem->m_lpData + bytesSent, pElem->m_cbData);
						pElem->m_lpData = (BYTE*)realloc(pElem->m_lpData, pElem->m_cbData);

						if (pElem->m_shutdown && pElem->m_cbData == 0)
						{
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] delayed shutdown"), (DWORD)rcvSocket));
							pElem->addRef();
							m_currentElem = pElem;
							fdc.autoUnlock(); /* remove possible deadlock */
							removeClient(pElem);
							pElem->release();
							fdc.autoLock();

							if (m_currentElem == NULL)
							{
								break;    /* break loop because pos may be invalid now */
							}

							m_currentElem = NULL;
						}
					}
				}

				if (!FD_ISSET(rcvSocket, &readfds))
				{
					continue;    /* while(pos) */
				}

				bytesRead = recv(rcvSocket, (char*)rcvBuffer,
								 sizeof(rcvBuffer), MSG_NOSIGNAL);

				if ((bytesRead != SOCKET_ERROR) && (bytesRead != 0))
				{
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET,
						   (_T("[%lu] rcv %lu bytes"),
							(DWORD)rcvSocket, bytesRead));
					_TRACEASCIISTRING(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET_DATA, bytesRead, (CHAR*)rcvBuffer);
					pElem->m_lastActive = GetTickCount();
					pElem->addRef();
					m_currentElem = pElem;
					fdc.autoUnlock(); /* remove possible deadlock */
					rcvData(pElem, bytesRead, rcvBuffer);
					pElem->release();
					fdc.autoLock();

					if (m_currentElem == NULL)
					{
						break;    /* break loop because pos may be invalid now */
					}

					m_currentElem = NULL;
				}
				else
				{
					LONG err = 0;

					if (bytesRead == SOCKET_ERROR)
					{
						err = getLastError();

						if (err == WSAEINTR || err == WSAEWOULDBLOCK)
						{
							continue;    /* while(pos) */
						}

						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET,
							   (_T("[%lu] rcv error [%lu]"),
								(DWORD)rcvSocket, err));
					}
					else
					{
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET,
							(_T("[%lu] disconnection notified by WinSockAPI (0 bytes received)"),
							(DWORD)rcvSocket));
					}

					pElem->addRef();
					m_currentElem = pElem;
					fdc.autoUnlock(); /* remove possible deadlock */
					removeClient(pElem, err);
					pElem->release();
					fdc.autoLock();

					if (m_currentElem == NULL)
					{
						break;    /* break loop because pos may be invalid now */
					}

					m_currentElem = NULL;
				}
			} /* while(pos) */

			fdc.autoUnlock();

			if (FD_ISSET(m_socket, &readfds))
			{
				sockAddrLen = sizeof(struct sockaddr_in);
				hNewClient = accept(m_socket, (struct sockaddr*)&sockAddr, &sockAddrLen);

				if (INVALID_SOCKET != hNewClient)
				{
					// new client connection
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("accept socket:%lu"), (DWORD)hNewClient));
					SOCmnPointer<SOCmnObject> pNewClient;
					addClient(hNewClient, &sockAddr, &pNewClient);
				}
				else
				{
					LONG err = getLastError();
					_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("accept error [%lu]"), err));
				}
			}

			if (FD_ISSET(m_controlRcvThread, &readfds))
			{
				if (recv(m_controlRcvThread, (char*)rcvBuffer,
						 sizeof(rcvBuffer), MSG_NOSIGNAL) > 0)
				{
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("control socket wakeup")));
					SOCmnList<SOCmnSocketData> request_list(&m_fdRequest);

					while ((pElem = request_list.pop()) != NULL)
					{
						request_list.autoUnlock();

						if (pElem->m_shutdown)
						{
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] delayed closesocket"),
																		   (DWORD)pElem->m_socket));
							closeSocket(pElem->m_socket);
						}
						else
						{
							_ASSERTION(FALSE, "illegal request");
						}

						pElem->release();
					}

					continue;
				}

				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("control socket shutdown")));

				while ((pElem = fdc.getHead()) != NULL)
				{
					pElem->addRef();
					fdc.autoUnlock();
					removeClient(pElem, SOCMNSOCKET_REQ_ABORTIVE_CLOSE);
					pElem->release();
				}

				return 0; // END Thread
			}
		}
		break;
		} /* switch */
	} /* for(;;) */

// unreachable code
//    return 0;
}

BOOL SOCmnSocketSrv::addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** ppSocket)
{
	pSockAddr = pSockAddr;
	_ASSERTION(m_rcvThread.isOwnThread(), "wrong thread?");
	_ASSERTION(ppSocket != NULL, "bad socket handle");

	if (!ppSocket)
	{
		return FALSE;
	}

	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	ioctlNonblocked(hSocket);
	ioctlNodelay(hSocket);
#ifdef SOOS_WINDOWS

	if (fdc.getCount() >= FD_SETSIZE - 2)
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		if (hSocket >= FD_SETSIZE)
#endif
		{
			// too many sockets
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("[%lu] too many sockets"), (DWORD)hSocket));
			closeSocket(hSocket);
			return FALSE;
		}

	SOCmnSocketData* pSocketData;
	SOCMN_ALLOCATE_OBJECT_C(pSocketData, SOCmnSocketData(hSocket), (SOCmnSocketData*));

	if (pSocketData == NULL)
	{
		closeSocket(hSocket);
		return FALSE;
	}

	fdc.add(pSocketData, pSocketData->getOpcHandle());
	pSocketData->m_clientHandle = *ppSocket;
	*ppSocket = pSocketData;
	m_currentElem = NULL;
	return TRUE;
}

#endif // SOFEATURE_CMN_SOCKET_SRV

#endif // SOFEATURE_CMN_SOCKET
