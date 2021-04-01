#ifndef _SOCMNUDPSOCKET_H_
#define _SOCMNUDPSOCKET_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


#ifdef SOFEATURE_TP_DISCOVERY

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
#include <ws2ipdef.h>
#ifndef SOOS_WINDOWS_CE
#pragma comment(lib, "ws2_32.lib")
#else
#if _WIN32_WCE > 300
#pragma comment(lib, "ws2.lib")
#else
#pragma comment(lib, "winsock.lib")
#endif
#endif //SOOS_WINDOWS_CE
#endif //SOOS_WINDOWS

#ifdef SOOS_WINDOWS
#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif
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
#endif // SOOS_WINDOWS

#include <SOCmnObject.h>


class SOCmnUDPSocket : public SOCmnObject
{
	static const DWORD MAX_NUMBER_OF_INTERFACES = 32;
	static const DWORD MAXIMUM_BUFFER_SIZE = 2048;
	static const DWORD DEFAULT_BROADCAST_TIMEOUT = 100;
public:
	class WSAHandler
	{
		WSAHandler();

	public:
		static SOCmnUDPSocket::WSAHandler* instance();
		~WSAHandler();

		void initialize(void);
		BOOL isInitialized(void);
		void finalize(void);
		void cleanUp(void);
		LONG getLastError(void);

		LONG ioctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer,
				   DWORD cbOutBuffer, LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped = NULL,
				   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine = NULL);


	private:
		static WSAHandler* m_pInstance;
		LONG m_lastError;
		BOOL m_initialized;
	};

public:
	SOCmnUDPSocket(SOCmnObject* parent = 0);
	~SOCmnUDPSocket();

	virtual LONG sendData(const char* message, unsigned long messageSize);
	virtual LONG receiveData(char** message, unsigned long& messageSize);
	virtual BOOL init(BOOL bindToPort);
	virtual BOOL hasPendingData(void);
	virtual LONG close(void);

	LONG getLastError(void);

	static BOOL getHostName(SOCmnString& hostName);
	static SOCKET create(LONG type = SOCK_DGRAM, LONG af = AF_INET, LONG protocol = IPPROTO_UDP);
	static LONG close(SOCKET s);
	static LONG setIOControlNonBlocking(SOCKET s, ULONG on = TRUE);
	static ULONG getIOControlInputBufferLength(SOCKET s, ULONG* buffer);
	static ULONG resolveInetAddr(LPCTSTR hostName);

	void setBroadcastReplyTimeOut(const int timeout)
	{
		m_broadcastReplyTimeout = timeout;
	}
	void setLocalPort(const unsigned short port)
	{
		m_localPort = port;
	}
	void setBroadCastPort(const unsigned short port)
	{
		m_broadcastPort = port;
	}
	void setBroadcastAddress(ULONG bcastAddr);

	LONG getBroadcastReplyTimeOut(void)
	{
		return m_broadcastReplyTimeout;
	}
	USHORT getLocalPort(void)
	{
		return m_localPort;
	}
	USHORT getBroadcastPort(void)
	{
		return m_broadcastPort;
	}

	// will return sender info for last received message or empty string
	const SOCmnString& getSenderAddress(void)
	{
		return m_senderIPAddress;
	};
	const SOCmnString& getBroadcastReceiverInterface(void);
	BOOL setBroadcastReceiverInterface();

	// number of interfaces on the system
	DWORD getNumberOfInterfaces(void);
	BOOL getInterface(INTERFACE_INFO** info, DWORD count);

	virtual BOOL handleTimeoutExpired(void);
	virtual BOOL handleSendResponse(void);

	DWORD getNoOfIfaces() const;
	// getIfaceAddress not used so far
	ULONG getIfaceAddress(UINT ifaceNo) const;
	// getIfaceNetMask not used so far
	ULONG getIfaceNetMask(UINT ifaceNo) const;
	ULONG getIfaceBcastAddr(UINT ifaceNo) const;

protected:
	SOCKET* getInternalSocket(void)
	{
		return &m_socket;
	};
	BOOL getAvailableInterfaces(void);

	SOCmnThread* getTimeoutHandlerThread(void)
	{
		return m_timeoutHandlerThread;
	};
	DWORD getWaitForTimeoutEvent(void)
	{
		return m_waitForTimeoutEvent;
	}
	DWORD getWaitForSendResponse(void)
	{
		return m_waitForSendResponse;
	}

	static DWORD _stdcall handleTimeoutThreadProc(void* context);

protected:
	SOCKET m_socket;

private:
	INTERFACE_INFO m_pInterFaceInfo[MAX_NUMBER_OF_INTERFACES];

	SOCmnThread* m_timeoutHandlerThread;
	DWORD m_waitForTimeoutEvent;
	DWORD m_waitForSendResponse;

	ULONG m_broadcastAddress;
	SOCmnString m_senderIPAddress;
	SOCmnString m_broadcastReceiverAddress;
	SOCmnString m_hostName;

	sockaddr_in* m_senderAddress;

	ULONG m_broadcastReplyTimeout;
	USHORT m_localPort;
	USHORT m_broadcastPort;
	DWORD m_numberOfInterfaces;
};

#endif  //  SOFEATURE_TP_DISCOVERY

#pragma pack(pop)
#endif // _SOCMNUDPSOCKET_H_
