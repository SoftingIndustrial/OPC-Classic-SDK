#ifndef _SOCMNSOCKET_H_
#define _SOCMNSOCKET_H_

#pragma pack(push, 4)


#ifdef SOFEATURE_CMN_SOCKET

#ifdef SOOS_LINUX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef int            SOCKET;

#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1

#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAEINPROGRESS EINPROGRESS
#define WSAEALREADY EALREADY
#define WSAENOTSOCK ENOTSOCK
#define WSAEDESTADDRREQ EDESTADDRREQ
#define WSAEMSGSIZE EMSGSIZE
#define WSAEPROTOTYPE EPROTOTYPE
#define WSAENOPROTOOPT ENOPROTOOPT
#define WSAEPROTONOSUPPORT EPROTONOSUPPORT
#define WSAESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define WSAEOPNOTSUPP EOPNOTSUPP
#define WSAEPFNOSUPPORT EPFNOSUPPORT
#define WSAEAFNOSUPPORT EAFNOSUPPORT
#define WSAEADDRINUSE EADDRINUSE
#define WSAEADDRNOTAVAIL EADDRNOTAVAIL
#define WSAENETDOWN ENETDOWN
#define WSAENETUNREACH ENETUNREACH
#define WSAENETRESET ENETRESET
#define WSAECONNABORTED ECONNABORTED
#define WSAECONNRESET ECONNRESET
#define WSAENOBUFS ENOBUFS
#define WSAEISCONN EISCONN
#define WSAENOTCONN ENOTCONN
#define WSAESHUTDOWN ESHUTDOWN
#define WSAETOOMANYREFS ETOOMANYREFS
#define WSAETIMEDOUT ETIMEDOUT
#define WSAECONNREFUSED ECONNREFUSED
#define WSAELOOP ELOOP
#define WSAENAMETOOLONG ENAMETOOLONG
#define WSAEHOSTDOWN EHOSTDOWN
#define WSAEHOSTUNREACH EHOSTUNREACH
#define WSAENOTEMPTY ENOTEMPTY
#define WSAEPROCLIM EPROCLIM
#define WSAEUSERS EUSERS
#define WSAEDQUOT EDQUOT
#define WSAESTALE ESTALE
#define WSAEREMOTE EREMOTE
#define WSAEINTR EINTR
#endif // SOOS_LINUX

#ifdef SOOS_WINDOWS
#ifndef _WINSOCKAPI_
#ifndef SOOS_WINDOWS_CE
#include <winsock2.h>
#else
#if _WIN32_WCE > 300
#include <winsock2.h>
#else
#include <winsock.h>
#endif
#endif
#endif

// some linux compatibility macros
#define socklen_t int
#define MSG_NOSIGNAL 0
#define SHUT_RD      SD_RECEIVE
#ifndef SOOS_WINDOWS_CE
#define SHUT_WR      SD_SEND
#define SHUT_RDWR    SD_BOTH
#else
#if _WIN32_WCE > 300
#define SHUT_WR      SD_SEND
#define SHUT_RDWR    SD_BOTH
#else
#define SHUT_WR      1
#define SHUT_RDWR    2
#endif
#endif
#endif // SOOS_WINDOWS

#include "SOCmnThread.h"

class SOCMN_EXPORT SOCmnSocketData : public SOCmnObject
{
	// helper class to queue data to be sent
	// its a mess that this class is used outside of SOCmnSocketXXX.cpp
public:
	SOCmnSocketData(SOCKET socket)
		: m_socket(socket), m_cbData(0), m_lpData(NULL),
		  m_connecting(false), m_shutdown(false),
		  m_timeOut(INFINITE), m_lastActive(GetTickCount()) {}

	SOCmnPointer<SOCmnObject> m_clientHandle;
	const SOCKET m_socket;
	DWORD m_cbData;
	BYTE* m_lpData;
	bool m_connecting;
	bool m_shutdown;
	DWORD m_timeOut;
	DWORD m_lastActive;

protected:
	~SOCmnSocketData()
	{
		free(m_lpData);
	}
};

//-----------------------------------------------------------------------------
// SOCmnSocket                                                                |
//-----------------------------------------------------------------------------

#define SOCMNSOCKET_STATE_INIT     0x00
#define SOCMNSOCKET_STATE_CREATED  0x01
#define SOCMNSOCKET_STATE_LISTEN   0x02

const LONG SOCMNSOCKET_REQ_GRACEFUL_CLOSE = -1;
const LONG SOCMNSOCKET_REQ_ABORTIVE_CLOSE = -2;

class SOCMN_EXPORT SOCmnSocket : public SOCmnObject
{
public:
	SOCmnSocket(void);

	static SOCKET createSocket(int type = SOCK_STREAM, int af = AF_INET);
	static int closeSocket(SOCKET s);
	static int ioctlNonblocked(SOCKET s);
	static int ioctlNodelay(SOCKET s);
	static int createSocketPair(SOCKET* s_connect, SOCKET* s_accept);
	static u_long resolveInetAddr(LPCTSTR host_name);

	static LONG getLastError(void)
	{
#ifdef SOOS_WINDOWS
		return WSAGetLastError();
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		return (LONG)errno;
#endif
	}

public:
	virtual BOOL start(void) = 0;
	virtual BOOL stop(void) = 0;

protected:
	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** ppSocket) = 0;
	virtual void rcvData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData);
	virtual void timeOut(IN SOCmnObject* pSocket);

public:
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason = SOCMNSOCKET_REQ_GRACEFUL_CLOSE);
	virtual LONG sndData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData);
	virtual BOOL shutdownClient(IN SOCmnObject* pSocket);
	virtual SOCmnObject* getClientHandle(IN SOCmnObject* pSocket);
	virtual void setClientHandle(IN SOCmnObject* pSocket, IN SOCmnObject* pClient);
	virtual BOOL setTimeOut(IN SOCmnObject* pSocket, IN DWORD dwTimeOut);
	virtual BOOL resetIdleTimer(IN SOCmnObject* pSocket);

protected:
	/* these are simple socket pairs:
	 * to wake the XXXThread
	 * write a byte to m_sendtoXXXThread
	 * to terminate the XXXThread
	 * shutdown(m_sendtoXXXThread, SD_WR)
	 */
	SOCKET m_sendtoRcvThread;
	SOCKET m_controlRcvThread;
	SOCmnThread m_rcvThread;

	SOCmnList<SOCmnSocketData> m_fdClients;
	SOCmnList<SOCmnSocketData> m_fdRequest;
	volatile SOCmnSocketData* m_currentElem;
	volatile BOOL m_checkTimeOuts;

	virtual DWORD rcvThread(void) = 0;

	static DWORD _stdcall rcvThreadProc(void* context);
}; // SOCmnSocket


#ifdef SOFEATURE_CMN_SOCKET_SRV

//-----------------------------------------------------------------------------
// SOCmnSocketSrv                                                             |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnSocketSrv : public SOCmnSocket
{
public:
	SOCmnSocketSrv(IN WORD port = 80);

	virtual LPCTSTR getObjTraceId(void);

	virtual LONG create(void);
	LONG create(IN WORD port);
	virtual LONG close(void);
	virtual LONG listen(IN OPTIONAL int queueLen = SOMAXCONN);

	virtual BOOL start(void);
	virtual BOOL stop(void);

	void setPort(WORD port)
	{
		_ASSERTION(getObjectState() < SOCMNSOCKET_STATE_CREATED, "cannot change port now");
		setObjTraceId(NULL);
		m_port = port;
	}
	WORD getPort(void)
	{
		return m_port;
	}

protected:
	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** ppSocket);

protected:
	SOCKET m_socket;
	WORD m_port;

	virtual DWORD rcvThread(void);

	virtual ~SOCmnSocketSrv();
}; // SOCmnSocketSrv

#endif // SOFEATURE_CMN_SOCKET_SRV

#ifdef SOFEATURE_CMN_SOCKET_CLIENT

//-----------------------------------------------------------------------------
// SOCmnSocketClient                                                          |
//-----------------------------------------------------------------------------
// client socket helper thread
class SOCMN_EXPORT SOCmnSocketClient : public SOCmnSocket
{
public:
	SOCmnSocketClient();

	virtual LPCTSTR getObjTraceId(void);

	virtual BOOL connect(IN SOCmnObject* pClientHandle, IN LPCTSTR host_name, IN WORD port = 80);
	virtual BOOL connect(IN SOCmnObject* pClientHandle, IN struct sockaddr_in* pSockAddr);
	virtual BOOL start(void);
	virtual BOOL stop(void);

protected:
	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN OUT SOCmnObject** ppSocket);
	virtual void connectedClient(IN SOCmnObject* pSocket);

protected:
	virtual DWORD rcvThread(void);

	virtual ~SOCmnSocketClient();
}; // SOCmnSocketClient
#endif // SOFEATURE_CMN_SOCKET_CLIENT
#endif // SOFEATURE_CMN_SOCKET

#pragma pack(pop)
#endif // _SOCMNSOCKET_H_
