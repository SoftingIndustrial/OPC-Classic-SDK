#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnString.h>

#ifdef SOFEATURE_CMN_SOCKET

#ifdef SOFEATURE_CMN_SOCKET_CLIENT

//-----------------------------------------------------------------------------
// SOCmnSocketSrv                                                             |
//-----------------------------------------------------------------------------

SOCmnSocketClient::SOCmnSocketClient(void)
{
	setObjectState(SOCMNSOCKET_STATE_INIT);
}

SOCmnSocketClient::~SOCmnSocketClient(void)
{
	stop();
}

//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//              trace id string
//
LPCTSTR SOCmnSocketClient::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[SOC|%8.8lX|]"), this->getOpcHandle());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId

BOOL SOCmnSocketClient::start(void)
{
	BOOL ret = FALSE;

	if (m_rcvThread.isRunning())
	{
		return TRUE;
	}

	if (createSocketPair(&m_sendtoRcvThread,
						 &m_controlRcvThread) == 0)
	{
		setObjectState(SOCMNSOCKET_STATE_CREATED);
		ret = m_rcvThread.start(rcvThreadProc, (void*)this);
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

BOOL SOCmnSocketClient::stop(void)
{
	if (getObjectState() != SOCMNSOCKET_STATE_CREATED)
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
	setObjectState(SOCMNSOCKET_STATE_INIT);
	return ret;
}

BOOL SOCmnSocketClient::connect(IN SOCmnObject* pClientHandle, IN LPCTSTR host_name, IN WORD port)
{
	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	host_addr.sin_addr.s_addr = resolveInetAddr(host_name); // <- this _may_ block...

	if (host_addr.sin_addr.s_addr == INADDR_NONE)
	{
		return FALSE;
	}

	return connect(pClientHandle, &host_addr);
}

BOOL SOCmnSocketClient::connect(IN SOCmnObject* pClientHandle, IN struct sockaddr_in* pSockAddr)
{
	if (getObjectState() != SOCMNSOCKET_STATE_CREATED)
	{
		return FALSE;
	}

	SOCKET hNewSocket = createSocket();

	if (hNewSocket == SOCKET_ERROR)
	{
		return FALSE;
	}

	if (hNewSocket == INVALID_SOCKET)
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[SOC] create socket res(%d) INVALID_SOCKET(%d)"), hNewSocket, getLastError()));
		return FALSE;
	}

	if (m_rcvThread.isOwnThread())
	{
		SOCmnPointer<SOCmnObject> pNewClient = pClientHandle;
		pNewClient.addRef();
		return addClient(hNewSocket, pSockAddr, &pNewClient);
	}
	else
	{
		SOCmnSocketData* pSocketData;
		SOCMN_ALLOCATE_OBJECT_C(pSocketData, SOCmnSocketData(hNewSocket), (SOCmnSocketData*));

		if (pSocketData == NULL)
		{
			closeSocket(hNewSocket);
			return FALSE;
		}

		pSocketData->m_cbData = sizeof(struct sockaddr_in);
		pSocketData->m_lpData = (BYTE*) malloc(sizeof(struct sockaddr_in));

		if (pSocketData->m_lpData == NULL)
		{
			pSocketData->release();
			closeSocket(hNewSocket);
			return FALSE;
		}

		*(struct sockaddr_in*)pSocketData->m_lpData = *pSockAddr;
		pSocketData->m_clientHandle = pClientHandle;
		pSocketData->m_clientHandle.addRef();
		SOCmnList<SOCmnSocketData> request_list(&m_fdRequest);
		request_list.add(pSocketData);
		request_list.autoUnlock();
		pSocketData->release();
		// now wake my worker thread...
		char dummy = 0;
		int sndErr = send(m_sendtoRcvThread, &dummy, sizeof(dummy), MSG_NOSIGNAL);
		if (sndErr == SOCKET_ERROR)
		{
			// see what`s the error
			sndErr = WSAGetLastError();
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("socket send error [%lu]"), sndErr));
			return FALSE;
		}
		return TRUE;
	}
}


#define SOCMNSOCKETCLIENT_RCV_BUFFER_SIZE 16384

DWORD SOCmnSocketClient::rcvThread(void)
{
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnListPosition pos;
	SOCmnSocketData* pElem;
	int sockCnt;
	DWORD dwTimeStart, dwTimeDiff, dwMaxWait;
	fd_set readfds, writefds, errorfds;
	BYTE rcvBuffer[SOCMNSOCKETCLIENT_RCV_BUFFER_SIZE];
	LONG bytesRead;
	LONG bytesSent;
	SOCKET rcvSocket;
	SOCKET maxSocket;
	_ASSERTION(m_rcvThread.isOwnThread(), "wrong thread?");

	for (;;)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&errorfds);
		FD_SET(m_controlRcvThread, &readfds);
		maxSocket = m_controlRcvThread;
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

			if (pElem->m_connecting)
			{
				FD_SET(rcvSocket, &writefds); // connect succeeds
				FD_SET(rcvSocket, &errorfds); // connect fails
			}
			else // connection established
			{
				FD_SET(rcvSocket, &readfds);

				if (pElem->m_cbData != 0)
				{
					FD_SET(rcvSocket, &writefds);
				}
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
			sockCnt = select((int)(maxSocket + 1), &readfds, &writefds, &errorfds, &tv);
		}
		else
		{
			sockCnt = select((int)(maxSocket + 1), &readfds, &writefds, &errorfds, NULL);
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

				if (pElem->m_connecting)
				{
					if (FD_ISSET(rcvSocket, &errorfds))
					{
						int err = 0;
						socklen_t opt_len = sizeof(err);
#ifdef SOOS_WINDOWS
						Sleep(10); /* workaround for a bug in W2K: getsockopt burns CPU cycles */
#endif
						getsockopt(rcvSocket, SOL_SOCKET, SO_ERROR,
								   (char*)&err, &opt_len);
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET,
							   (_T("[%lu] connect error [%u]"),
								(DWORD)rcvSocket, err));
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
					else if (FD_ISSET(rcvSocket, &writefds))
					{
						_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET,
							   (_T("[%lu] connect succeeds"),
								(DWORD)rcvSocket));
						pElem->m_connecting = false;
						pElem->m_lastActive = GetTickCount();
						pElem->addRef();
						m_currentElem = pElem;
						fdc.autoUnlock(); /* remove possible deadlock */
						connectedClient(pElem);
						pElem->release();
						fdc.autoLock();

						if (m_currentElem == NULL)
						{
							break;    /* break loop because pos may be invalid now */
						}

						m_currentElem = NULL;
					}

					continue; /* while(pos) */
				}

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
							_ASSERTION(pElem->m_cbData == sizeof(struct sockaddr_in), "wrong connection data");
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] delayed addClient"),
																		   (DWORD)pElem->m_socket));
							DWORD currentTimeOut = pElem->m_timeOut;
							addClient(pElem->m_socket, (struct sockaddr_in*)pElem->m_lpData, &pElem->m_clientHandle);
							setTimeOut(pElem->m_clientHandle, currentTimeOut);
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] socket timeout is %lu"), (DWORD)pElem->m_socket, currentTimeOut));
						}

						pElem->release();
					}

					continue;
				}

				//_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("control socket shutdown")));

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

BOOL SOCmnSocketClient::addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** ppSocket)
{
	USES_CONVERSION;
	_ASSERTION(m_rcvThread.isOwnThread(), "wrong thread?");
	_ASSERTION(ppSocket != NULL, "bad socket handle");

	if (!ppSocket)
	{
		return FALSE;
	}

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("[%lu] connect addr %s port %u"),
												   (DWORD)hSocket, A2T(inet_ntoa(pSockAddr->sin_addr)), ntohs(pSockAddr->sin_port)));
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	ioctlNonblocked(hSocket);
	ioctlNodelay(hSocket);
#ifdef SOOS_WINDOWS

	if (fdc.getCount() >= FD_SETSIZE - 1)
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

	int ret = ::connect(hSocket, (struct sockaddr*)pSockAddr, sizeof(struct sockaddr_in));

	if (ret == SOCKET_ERROR)
	{
		LONG err = getLastError();

		if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS)
		{
			// network error
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("connect error [%lu]"), err));
			closeSocket(hSocket);
			return FALSE;
		}
	}

	if (ret == 0) // this was really quick...
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET,
			   (_T("[%lu] connect succeeds (immediately?!)"),
				(DWORD)hSocket));
	}

	SOCmnSocketData* pSocketData;
	SOCMN_ALLOCATE_OBJECT_C(pSocketData, SOCmnSocketData(hSocket), (SOCmnSocketData*));

	if (pSocketData == NULL)
	{
		closeSocket(hSocket);
		return FALSE;
	}

	fdc.add(pSocketData, pSocketData->getOpcHandle());
	pSocketData->m_connecting = true;
	pSocketData->m_clientHandle = *ppSocket;
	*ppSocket = pSocketData;
	m_currentElem = NULL;
	return TRUE;
}

void SOCmnSocketClient::connectedClient(IN SOCmnObject* /* pSocket */)
{
}

#endif // SOFEATURE_CMN_SOCKET_CLIENT

#endif // SOFEATURE_CMN_SOCKET
