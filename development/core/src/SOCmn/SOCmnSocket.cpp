#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnString.h>

#ifdef SOFEATURE_CMN_SOCKET

//-----------------------------------------------------------------------------
// SOCmnSocket                                                                |
//-----------------------------------------------------------------------------

SOCmnSocket::SOCmnSocket(void)
{
	m_fdClients.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD
					   | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED
					   | SOCMNLIST_TYPE_BLOCKED);
	m_fdRequest.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO
					   | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED
					   | SOCMNLIST_TYPE_BLOCKED);
	m_sendtoRcvThread = INVALID_SOCKET;
	m_controlRcvThread = INVALID_SOCKET;
	m_currentElem = NULL;
	m_checkTimeOuts = FALSE;
}

SOCKET SOCmnSocket::createSocket(IN int type, IN int af)
{
	return socket(af, type, 0);
}

int SOCmnSocket::closeSocket(SOCKET s)
{
#ifdef SOOS_WINDOWS
	return ::closesocket(s);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::close(s);
#endif
}

int SOCmnSocket::ioctlNonblocked(SOCKET s)
{
#ifdef SOOS_WINDOWS
	u_long on = 1;
	return ioctlsocket(s, FIONBIO, &on);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	int on = 1;
	return ioctl(s, FIONBIO, &on);
	// this works too...
	// fcntl(s, F_SETFL, O_NONBLOCK);
#endif
}

int SOCmnSocket::ioctlNodelay(SOCKET s)
{
	int optval = 1;
	return setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
}

#ifdef SOOS_WINDOWS
int SOCmnSocket::createSocketPair(SOCKET* s_connect, SOCKET* s_accept)
{
	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	SOCKET s_listen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s_listen == INVALID_SOCKET)
	{
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -1;
	}

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = 0;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (bind(s_listen, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
	{
		closeSocket(s_listen);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -2;
	}

	if (listen(s_listen, 1) == SOCKET_ERROR)
	{
		closeSocket(s_listen);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -3;
	}

	socklen_t namelen = sizeof(listen_addr);

	if (getsockname(s_listen, (struct sockaddr*)&listen_addr, &namelen) == SOCKET_ERROR)
	{
		closeSocket(s_listen);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -4;
	}

	*s_connect = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (*s_connect == INVALID_SOCKET)
	{
		closeSocket(s_listen);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -5;
	}

	if (connect(*s_connect, (struct sockaddr*)&listen_addr, namelen) == SOCKET_ERROR)
	{
		closeSocket(s_listen);
		closeSocket(*s_connect);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -6;
	}

	*s_accept = accept(s_listen, NULL, 0);

	if (*s_accept == INVALID_SOCKET)
	{
		closeSocket(s_listen);
		closeSocket(*s_connect);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -7;
	}

	if (closeSocket(s_listen) == SOCKET_ERROR)
	{
		closeSocket(*s_connect);
		closeSocket(*s_accept);
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return -8;
	}

	return 0;
}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
int SOCmnSocket::createSocketPair(SOCKET* s_connect, SOCKET* s_accept)
{
	int filedes[2];
	int retval = socketpair(AF_UNIX, SOCK_STREAM, 0, filedes);

	if (retval < 0)
	{
		*s_connect = INVALID_SOCKET;
		*s_accept  = INVALID_SOCKET;
		return errno;
	}

	*s_connect = filedes[1];
	*s_accept  = filedes[0];
	return 0;
}
#endif // SOOS_LINUX

u_long SOCmnSocket::resolveInetAddr(LPCTSTR host_name)
{
	USES_CONVERSION;
	//  remove zeros from the IP
	LPTSTR regularIp = _tcsdup(host_name);

	if (regularIp == NULL)
	{
		return INADDR_NONE;
	}

	BOOL isIP = TRUE;
	BOOL removeZero = TRUE;
	int nDots = 0;
	int index = 0;

	while (TRUE == isIP && regularIp[index] != 0)
	{
		if (regularIp[index] == '.')
		{
			if (++nDots > 3)
			{
				isIP = FALSE;
			}

			removeZero = TRUE;
		}
		else
		{
			if (!_istdigit(regularIp[index]))
			{
				isIP = FALSE;
			}
			else
			{
				if (removeZero)
				{
					if (regularIp[index] == _T('0') && regularIp[index + 1] != _T('.'))
					{
						_tcscpy(&regularIp[index], &regularIp[index + 1]);
						index--;
					}
					else
					{
						removeZero = FALSE;
					}
				}
			}
		}

		index++;
	}

	u_long retval = INADDR_NONE;

	if (isIP)
	{
		retval = inet_addr(T2CA(regularIp));
	}

	free(regularIp);

	if (retval == INADDR_NONE)
	{
		struct hostent* host;
#ifdef SOOS_LINUX
#ifdef SODEVENV_CYGWIN
		host = gethostbyname(T2CA(host_name));
#else
		int result_h_errno;
		struct hostent result_hostent;
		char result_buf[1024];
		gethostbyname_r(host_name, &result_hostent,
						result_buf, sizeof(result_buf),
						&host, &result_h_errno);
#endif
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		host = gethostbyname(T2CA(host_name));
#endif

		if (host != NULL
			&& host->h_addrtype == AF_INET
			&& host->h_length == sizeof(u_long)
			&& host->h_addr_list[0] != NULL)
		{
			memcpy(&retval, host->h_addr_list[0], sizeof(u_long));
		}
	}

	return retval;
}

DWORD _stdcall SOCmnSocket::rcvThreadProc(void* context)
{
	SOCmnSocket* pT = (SOCmnSocket*)context;
	return pT->rcvThread();
}

void SOCmnSocket::rcvData(
	IN SOCmnObject* /* pSocket */,
	IN LONG /* size */,
	IN BYTE* /* pData */)
{
}

void SOCmnSocket::timeOut(IN SOCmnObject* pSocket)
{
	if (!pSocket)
	{
		return;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCKET hSocket = pSocketData->m_socket;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("[%lu] timeout"), (DWORD)hSocket));
	// default action: removeClient
	removeClient(pSocket);
}

BOOL SOCmnSocket::removeClient(IN SOCmnObject* pSocket, IN LONG reason)
{
	_ASSERTION(pSocket != NULL, "bad socket handle");

	if (!pSocket)
	{
		return FALSE;
	}

	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnPointer<SOCmnSocketData> elem = fdc.findKey(pSocket->getOpcHandle());

	if (!fdc.removeKey(pSocket->getOpcHandle()))
	{
		return FALSE;
	}

	if (m_currentElem != elem)
	{
		m_currentElem = NULL;
	}

	fdc.autoUnlock();
	SOCKET hSocket = elem->m_socket;
	elem->m_shutdown = true;
	elem->m_clientHandle = NULL;  // remove possible cyclic reference

	if (reason == SOCMNSOCKET_REQ_ABORTIVE_CLOSE)
	{
		struct linger opt; // linger: on, but 0 seconds timeout => abortive close
		opt.l_linger = 0;
		opt.l_onoff = 1;
		setsockopt(hSocket, SOL_SOCKET, SO_LINGER, (const char*)&opt, sizeof(opt));
	}
	else if (reason == SOCMNSOCKET_REQ_GRACEFUL_CLOSE)
	{
		shutdown(hSocket, SHUT_RDWR);
	}

	if (m_rcvThread.isOwnThread())
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] immediate closesocket"), (DWORD)hSocket));
		closeSocket(hSocket);
	}
	else
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] pending closesocket"), (DWORD)hSocket));
		SOCmnList<SOCmnSocketData> request_list(&m_fdRequest);
		request_list.add(elem);
		request_list.autoUnlock();
		// now wake my worker thread...
		char dummy = 0;
		send(m_sendtoRcvThread, &dummy, sizeof(dummy), MSG_NOSIGNAL);
	}

	return TRUE;
}

LONG SOCmnSocket::sndData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData)
{
	_ASSERTION(pSocket != NULL, "bad socket handle");

	if (!pSocket)
	{
		return -1;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCKET hSocket = pSocketData->m_socket;
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] snd %lu bytes"), (DWORD)hSocket, size));
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnPointer<SOCmnSocketData> elem;
	LONG res;
	elem = fdc.findKey(pSocketData->getOpcHandle());

	if (!elem || elem->m_shutdown)
	{
		return -1;
	}

	if (elem->m_cbData != 0 || elem->m_connecting)
	{
		BYTE* pnew = (BYTE*)realloc(elem->m_lpData, elem->m_cbData + size);

		if (pnew == NULL)
		{
			return -1;
		}

		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] queue %lu bytes"), (DWORD)hSocket, size));
		elem->m_lpData = pnew;
		memcpy(elem->m_lpData + elem->m_cbData, pData, size);
		elem->m_cbData += size;
		return 0;
	}

	res = send(hSocket, (char*)pData, size, MSG_NOSIGNAL);

	if (res == SOCKET_ERROR)
	{
		LONG err = getLastError();

		if (err != WSAEWOULDBLOCK)
		{
			return res;    /* hard error */
		}

		res = 0;
	}

	_TRACEASCIISTRING(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET_DATA, res, (CHAR*)pData);

	if (res < size)
	{
		BYTE* pnew = (BYTE*)realloc(elem->m_lpData, size - res);

		if (pnew == NULL)
		{
			return -1;
		}

		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] queue %lu bytes"), (DWORD)hSocket, size - res));
		elem->m_lpData = pnew;
		memcpy(elem->m_lpData, pData + res, size - res);
		elem->m_cbData = size - res;
		fdc.autoUnlock();
		elem.release();

		if (!m_rcvThread.isOwnThread())
		{
			// now wake my worker thread...
			char dummy = 0;
			send(m_sendtoRcvThread, &dummy, sizeof(dummy), MSG_NOSIGNAL);
		}
	}

	return res;
}

BOOL SOCmnSocket::shutdownClient(IN SOCmnObject* pSocket)
{
	_ASSERTION(pSocket != NULL, "bad socket handle");

	if (!pSocket)
	{
		return FALSE;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCKET hSocket = pSocketData->m_socket;
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] shutdown"), (DWORD)hSocket));
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnPointer<SOCmnSocketData> elem;
	elem = fdc.findKey(pSocketData->getOpcHandle());

	if (!elem || elem->m_shutdown)
	{
		return FALSE;
	}

	if (elem->m_cbData != 0)
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("[%lu] shutdown pending"), (DWORD)hSocket));
		elem->m_shutdown = true;
		return TRUE;
	}

	fdc.autoUnlock();
	return removeClient(pSocket);
}

SOCmnObject* SOCmnSocket::getClientHandle(IN SOCmnObject* pSocket)
{
	if (!pSocket)
	{
		return NULL;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	fdc.autoLock();
	pSocketData->m_clientHandle.addRef();
	return pSocketData->m_clientHandle;
}

void SOCmnSocket::setClientHandle(IN SOCmnObject* pSocket, IN SOCmnObject* pClient)
{
	if (!pSocket)
	{
		return;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	fdc.autoLock();
	pSocketData->m_clientHandle = pClient;
	pSocketData->m_clientHandle.addRef();
}

BOOL SOCmnSocket::setTimeOut(IN SOCmnObject* pSocket, IN DWORD dwTimeOut)
{
	if (!pSocket)
	{
		return FALSE;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnPointer<SOCmnSocketData> elem;
	elem = fdc.findKey(pSocketData->getOpcHandle());

	if (!elem)
	{
		return FALSE;
	}

	if (dwTimeOut < elem->m_timeOut)
	{
		m_checkTimeOuts = TRUE;
	}

	elem->m_timeOut = dwTimeOut;
	fdc.autoUnlock();

	if (!m_rcvThread.isOwnThread())
	{
		// now wake my worker thread...
		char dummy = 0;
		send(m_sendtoRcvThread, &dummy, sizeof(dummy), MSG_NOSIGNAL);
	}

	return TRUE;
}

BOOL SOCmnSocket::resetIdleTimer(IN SOCmnObject* pSocket)
{
	if (!pSocket)
	{
		return FALSE;
	}

	SOCmnSocketData* pSocketData = (SOCmnSocketData*)pSocket;
	SOCmnList<SOCmnSocketData> fdc(&m_fdClients);
	SOCmnPointer<SOCmnSocketData> elem;
	elem = fdc.findKey(pSocketData->getOpcHandle());

	if (!elem)
	{
		return FALSE;
	}

	elem->m_lastActive = GetTickCount();
	return TRUE;
}

#endif // SOFEATURE_CMN_SOCKET
