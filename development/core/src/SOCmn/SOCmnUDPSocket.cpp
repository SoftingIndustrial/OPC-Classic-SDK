//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOCmn                              |
//                                                                            |
//  Filename    : SOCmnUDPSocket.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : UDP socket communication related classes                    |
//																			  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOFEATURE_TP_DISCOVERY
#include <SOCmnString.h>

#include "SOCmnUDPSocket.h"

/**************************************************************
              SOCmnUDPSocket::WSAHandler
 **************************************************************/

SOCmnUDPSocket::WSAHandler* SOCmnUDPSocket::WSAHandler::m_pInstance = 0;

SOCmnUDPSocket::WSAHandler* SOCmnUDPSocket::WSAHandler::instance(
	void)
{
	if (m_pInstance)
	{
		return m_pInstance;
	}
	else
	{
		m_pInstance = new WSAHandler();
		return m_pInstance;
	} // end if .. else
} // end instance



SOCmnUDPSocket::WSAHandler::WSAHandler(
	void) : m_initialized(false)
{
} // end ctor



SOCmnUDPSocket::WSAHandler::~WSAHandler(
	void)
{
	delete m_pInstance;
} // end dtor



void SOCmnUDPSocket::WSAHandler::initialize(
	void)
{
#ifdef SOOS_WINDOWS
	// Initiate use of the WinSock DLL.
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	// WSA startup not needed
#endif // SOOS_LINUX
	m_initialized = true;
} // end initializeWSA



BOOL SOCmnUDPSocket::WSAHandler::isInitialized(
	void)
{
	return m_initialized;
} // end isWSAInitialized



void SOCmnUDPSocket::WSAHandler::finalize(
	void)
{
#ifdef SOOS_WINDOWS
	// Cleanup.
	cleanUp();
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	// WSA cleanup not needed
#endif // SOOS_LINUX
	m_initialized = false;
} // end finalizeWSA



void SOCmnUDPSocket::WSAHandler::cleanUp(
	void)
{
#ifdef SOOS_WINDOWS
	// Cleanup.
	WSACleanup();
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	// WSA cleanup not needed
#endif // SOOS_LINUX
} // end finalizeWSA



LONG SOCmnUDPSocket::WSAHandler::getLastError(
	void)
{
#ifdef SOOS_WINDOWS
	m_lastError = WSAGetLastError();
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_lastError = (LONG)errno;
#endif // SOOS_LINUX
	return m_lastError;
} // end getWSALastError



LONG SOCmnUDPSocket::WSAHandler::ioctl(
	SOCKET s,
	DWORD dwIoControlCode,
	LPVOID lpvInBuffer,
	DWORD cbInBuffer,
	LPVOID lpvOutBuffer,
	DWORD cbOutBuffer,
	LPDWORD lpcbBytesReturned,
	LPWSAOVERLAPPED lpOverlapped /*= NULL*/,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine /*= NULL*/)
{
#ifdef SOOS_WINDOWS
	return WSAIoctl(s, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer,
					cbOutBuffer, lpcbBytesReturned, lpOverlapped, lpCompletionRoutine);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
#endif // SOOS_LINUX
} // end ioctl



/**************************************************************
                    SOCmnUDPSocket
 **************************************************************/

//object can be more generic/configurabe if the local,device ports and address can be configured
SOCmnUDPSocket::SOCmnUDPSocket(
	SOCmnObject* parent) : SOCmnObject(),
	m_broadcastReplyTimeout(DEFAULT_BROADCAST_TIMEOUT),
	m_localPort(0),
	m_broadcastPort(0)
{
	m_socket = INVALID_SOCKET;
	m_numberOfInterfaces = 0;
	m_senderAddress = NULL;
	SOCMN_ALLOCATE_OBJECT_C(m_timeoutHandlerThread, SOCmnThread(), (SOCmnThread*));
	m_waitForTimeoutEvent = m_timeoutHandlerThread->createEvent();
	m_waitForSendResponse = m_timeoutHandlerThread->createEvent();
} // end ctor



SOCmnUDPSocket::~SOCmnUDPSocket(
	void)
{
	if (WSAHandler::instance()->isInitialized())
	{
		WSAHandler::instance()->finalize();
	} // end if

	if (m_senderAddress != NULL)
	{
		free(m_senderAddress);
		m_senderAddress = NULL;
	} // end if

	if (m_timeoutHandlerThread != NULL)
	{
		delete m_timeoutHandlerThread;
		m_timeoutHandlerThread = NULL;
	} // end if

	close(m_socket);
} // end dtor



BOOL SOCmnUDPSocket::init(
	BOOL bindToPort)
{
	if (!WSAHandler::instance()->isInitialized())
	{
		WSAHandler::instance()->initialize();
	} // end if

	if (getAvailableInterfaces() != TRUE)
	{
		WSAHandler::instance()->finalize();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::init - Could not get list of available network interfaces !")));
		return FALSE;
	} // end if

	m_senderAddress = (sockaddr_in*)malloc(sizeof(sockaddr_in));
	memset(m_senderAddress, 0, sizeof(sockaddr_in));
	m_socket = create();

	if (m_socket == INVALID_SOCKET)
	{
		close(m_socket);
		WSAHandler::instance()->finalize();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::init - Socket creation failed: %d"), WSAHandler::instance()->getLastError()));
		return FALSE;
	} // end if

	// Create the address we are receiving on.
	sockaddr_in bcaddr;
	memset((char*)&bcaddr, 0, sizeof(bcaddr));
	bcaddr.sin_family = AF_INET;
	bcaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bcaddr.sin_port = htons(m_broadcastPort);
	u_long noBlock = 1;
	ioctlsocket(m_socket, FIONBIO, &noBlock);

	if (bindToPort == TRUE)
	{
		INT bindResult = bind(m_socket, (sockaddr*)&bcaddr, sizeof(bcaddr));

		if (bindResult == SOCKET_ERROR)
		{
			close(m_socket);
			WSAHandler::instance()->finalize();
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::init - Error occurred while binding socket: %d"), WSAHandler::instance()->getLastError()));
			return FALSE;
		} // end if
	} // end if

	// set socket option
	BOOL bOptVal = TRUE;
	int  bOptLen = sizeof(BOOL);
	INT setOptionResult = setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&bOptVal, bOptLen);

	if (setOptionResult == SOCKET_ERROR)
	{
		close(m_socket);
		WSAHandler::instance()->finalize();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::init - Error occurred while setting socket options: %d"), WSAHandler::instance()->getLastError()));
		return FALSE;
	} // end if

	// start the internal handler thread
	m_timeoutHandlerThread->start(handleTimeoutThreadProc, this);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::init - Succesfully initialised the socket")));
	return TRUE;
} // end init


BOOL SOCmnUDPSocket::getAvailableInterfaces(
	void)
{
	// create a test socket
	SOCKET s = create();

	if (s == INVALID_SOCKET)
	{
		WSAHandler::instance()->finalize();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::getAvailableInterfaces - Socket creation failed while aquiring network interfaces: %d"), WSAHandler::instance()->getLastError()));
		return FALSE;
	} // end if

	// get all interfaces on socket
	DWORD bytesReturned = 0;

	if (WSAHandler::instance()->ioctl(s, SIO_GET_INTERFACE_LIST, NULL, 0,
									  &m_pInterFaceInfo, sizeof(m_pInterFaceInfo),
									  &bytesReturned) == SOCKET_ERROR)
	{
		close(s);
		WSAHandler::instance()->finalize();
		LONG error = WSAHandler::instance()->getLastError();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::getAvailableInterfaces - Socket error aquiring network interfaces: %d"), error));
		return FALSE;
	} // end if

	close(s);
	m_numberOfInterfaces = (bytesReturned / sizeof(INTERFACE_INFO));
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::getAvailableInterfaces - Number of available network interfaces: %d"), m_numberOfInterfaces));
	return TRUE;
} // end getAvailableInterfaces



LONG SOCmnUDPSocket::sendData(
	const char* message,
	unsigned long messageSize)
{
	int result;
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_broadcastPort);
	serverAddr.sin_addr.s_addr = m_broadcastAddress;
	result = sendto(m_socket,
					message,
					messageSize,
					0,
					reinterpret_cast<sockaddr*>(&serverAddr),
					sizeof(serverAddr));

	if (result == SOCKET_ERROR)
	{
		USES_CONVERSION;
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::sendData - Error #%d occurred while sending message of size %d to address %s"), WSAHandler::instance()->getLastError(), messageSize, A2T(inet_ntoa(serverAddr.sin_addr))));
		return S_FALSE;
	}

	m_timeoutHandlerThread->getEvents()->signal(m_waitForTimeoutEvent);
	return S_OK;
} // end sendData



LONG SOCmnUDPSocket::receiveData(
	char** message,
	unsigned long& messageSize)
{
	sockaddr_in senderAddr = { 0 };
	//Receive a message into the buffer.
	CHAR replyBuffer[MAXIMUM_BUFFER_SIZE];
	INT alen = sizeof(struct sockaddr);
	INT size = recvfrom(m_socket, replyBuffer, MAXIMUM_BUFFER_SIZE, 0, (sockaddr*)&senderAddr, &alen);

	if (size <= 0)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::receiveData - Error occurred while receiving the messages: %d"), WSAHandler::instance()->getLastError()));
		return S_FALSE;
	} // end if

	USES_CONVERSION;
	memcpy(m_senderAddress, &senderAddr, sizeof(sockaddr_in));
	m_senderIPAddress.format(_T("%s"), A2CT(inet_ntoa((in_addr)senderAddr.sin_addr)));
	messageSize = (unsigned long)size;
	*message = (char*)malloc((messageSize) * sizeof(char));
	memset(*message, 0, messageSize * sizeof(char));
	memcpy(*message, replyBuffer, messageSize);
	// sets the interface where the broadcast was received
	setBroadcastReceiverInterface();
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::receiveData - Succesfully received message of size: %d"), messageSize));
	return S_OK;
} // end receiveData



BOOL SOCmnUDPSocket::hasPendingData(
	void)
{
	fd_set read_descriptor;
	int nfds;
	struct timeval Timeout;
	int result;
	FD_ZERO(&read_descriptor);
	FD_SET(m_socket, &read_descriptor);
	nfds = m_socket + 1;
	Timeout.tv_sec  = 0;
	Timeout.tv_usec = 10; // microseconds
	result = select(0, &read_descriptor, NULL, NULL, 0);

	if (result > 0)
	{
		if (FD_ISSET(m_socket, &read_descriptor) == 0)
		{
			m_timeoutHandlerThread->getEvents()->signal(m_waitForTimeoutEvent);
			setIOControlNonBlocking(m_socket);
			FD_CLR(m_socket, &read_descriptor);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::hasPendingData - Timeout on waiting for pending data: %d"), WSAHandler::instance()->getLastError()));
			return FALSE;
		} // end if

		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnUDPSocket::hasPendingData - Prepared for sending messages!")));
		setIOControlNonBlocking(m_socket);
		m_timeoutHandlerThread->getEvents()->signal(m_waitForSendResponse);
		FD_CLR(m_socket, &read_descriptor);
		return TRUE;
	} // end if

	return FALSE;
} // end hasPendingData



LONG SOCmnUDPSocket::getLastError(
	void)
{
	return WSAHandler::instance()->getLastError();
} // end getLastError



BOOL SOCmnUDPSocket::getHostName(
	SOCmnString& hostName)
{
	const ULONG nameSize = 255;
	char nameBuffer[nameSize];
	INT ret = gethostname(nameBuffer, nameSize);

	if (ret == 0)
	{
		USES_CONVERSION;
		hostName.format(_T("%s"), A2CT(nameBuffer));
		_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, _T("SOCmnUDPSocket::getHostName"), (_T("Host name set to: %s"), hostName));
		return TRUE;
	} // end if

	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, _T("SOCmnUDPSocket::getHostName"), (_T("Could not get the host name !")));
	return FALSE;
} // end getHostName



SOCKET SOCmnUDPSocket::create(
	LONG type /*=SOCK_DGRAM*/,
	LONG af /*=AF_INET*/,
	LONG protocol /*=IPPROTO_UDP*/)
{
	return socket(af, type, protocol);
} // end create



LONG SOCmnUDPSocket::close(
	SOCKET s)
{
#ifdef SOOS_WINDOWS
	return ::closesocket(s);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::close(s);
#endif
} // end close


LONG SOCmnUDPSocket::close(
	void)
{
	m_timeoutHandlerThread->getEvents()->signal(m_timeoutHandlerThread->getStopEventId());
#ifdef SOOS_WINDOWS
	return ::closesocket(m_socket);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::close(m_socket);
#endif
}


LONG SOCmnUDPSocket::setIOControlNonBlocking(
	SOCKET s,
	ULONG on /*= TRUE*/)
{
#ifdef SOOS_WINDOWS
	return ioctlsocket(s, FIONBIO, &on);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ioctl(s, FIONBIO, &on);
#endif
} // end setIOControlBlocked



ULONG SOCmnUDPSocket::getIOControlInputBufferLength(
	SOCKET s,
	ULONG* buffer)
{
#ifdef SOOS_WINDOWS
	return ioctlsocket(s, FIONREAD, buffer);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ioctl(s, FIONREAD, buffer);
#endif
} // end getIOControlInputBufferLength



ULONG SOCmnUDPSocket::resolveInetAddr(
	LPCTSTR hostName)
{
	USES_CONVERSION;
	//  remove zeros from the IP
	LPTSTR regularIp = _tcsdup(hostName);

	if (regularIp == NULL)
	{
		return INADDR_NONE;
	} // end if

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
			} // end if

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
		host = gethostbyname(T2CA(hostName));
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
		host = gethostbyname(T2CA(hostName));
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
} // end resolveInetAddr



DWORD SOCmnUDPSocket::getNumberOfInterfaces(void)
{
	return m_numberOfInterfaces;
} // end getNumberOfInterfaces



BOOL SOCmnUDPSocket::getInterface(
	INTERFACE_INFO** info,
	DWORD count)
{
	if (count < 0 ||
		count >= m_numberOfInterfaces)
	{
		return FALSE;
	} // end if

	*info = (INTERFACE_INFO*)malloc(sizeof(INTERFACE_INFO));
	memcpy(*info, &m_pInterFaceInfo[count], sizeof(INTERFACE_INFO));
	return TRUE;
} // end if


const SOCmnString& SOCmnUDPSocket::getBroadcastReceiverInterface(
	void)
{
	return m_broadcastReceiverAddress;
} // end getBroadcastReceiverInterface



BOOL SOCmnUDPSocket::setBroadcastReceiverInterface(
	void)
{
	// mask based check to see if interfaces are in the same subnet
	ULONG senderIP = m_senderAddress->sin_addr.s_addr;

	for (DWORD i = 0; i < m_numberOfInterfaces; i++)
	{
		// build the mask
		ULONG mask = m_pInterFaceInfo[i].iiNetmask.AddressIn.sin_addr.s_addr;
		// localIP
		ULONG localIP = m_pInterFaceInfo[i].iiAddress.AddressIn.sin_addr.s_addr;

		if ((senderIP & mask) == (localIP & mask))
		{
			USES_CONVERSION;
			SOCmnString anAddress;
			anAddress.format(_T("%s"), A2CT(inet_ntoa(m_pInterFaceInfo[i].iiAddress.AddressIn.sin_addr)));
			m_broadcastReceiverAddress = anAddress;
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("Broadcast receiver interface identified as: %s"), m_broadcastReceiverAddress));
			return TRUE;
		} // end if
	} // end for

	_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Could not identify broadcast receiver interface !")));
	return FALSE;
} // end setBroadcastReceiverInterface



DWORD _stdcall SOCmnUDPSocket::handleTimeoutThreadProc(
	void* pContext)
{
	if (pContext == NULL)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, _T("SOCmnUDPSocket::handleTimeoutThreadProc"), (_T("NULL context passed in sockets timeout handle thread")));
		return S_FALSE;
	} // end if

	SOCmnUDPSocket* pSocket = reinterpret_cast<SOCmnUDPSocket*>(pContext);
	pSocket->addRef();
	SOCmnThread* pMe = pSocket->getTimeoutHandlerThread();
	DWORD waitR;
	BOOL end = FALSE;
	DWORD dwWait = 1000;

	while (!end)
	{
		waitR = pMe->waitForEvents(dwWait);

		if (waitR == pMe->getStopEventId())
		{
			end = TRUE;
		}
		else if (waitR == pSocket->getWaitForTimeoutEvent())
		{
			_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, _T("SOCmnUDPSocket::handleTimeoutThreadProc"), (_T("Timeout event received on sockets timeout handle thread")));
			pSocket->handleTimeoutExpired();
		}
		else if (waitR == pSocket->getWaitForSendResponse())
		{
			_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, _T("SOCmnUDPSocket::handleTimeoutThreadProc"), (_T("Send response event received on sockets timeout handle thread")));
			pSocket->handleSendResponse();
		} // end if .. else
	} // end while

	pSocket->release();
	return TRUE;
} // end handleTimeoutThreadProc



// reimplement this in derived classes if you want special timeout handling
BOOL SOCmnUDPSocket::handleTimeoutExpired(
	void)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("Broadcast timeout expired while waiting for messages!")));
	m_timeoutHandlerThread->getEvents()->reset(m_waitForTimeoutEvent);
	return TRUE;
} // end handleTimeoutExpired



// reimplement this in derived classes if you want to send responses to messages
// (for example: call sendData)
BOOL SOCmnUDPSocket::handleSendResponse(
	void)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("Send response to messages !")));
	sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);
	accept(m_socket, (sockaddr*)&sinRemote, &nAddrSize);
	m_timeoutHandlerThread->getEvents()->reset(m_waitForSendResponse);
	return TRUE;
} // end handleSendResponse

DWORD SOCmnUDPSocket::getNoOfIfaces() const
{
	return m_numberOfInterfaces;
}

ULONG SOCmnUDPSocket::getIfaceAddress(UINT ifaceNo) const
{
	if (ifaceNo >= m_numberOfInterfaces)
	{
		return 0L;
	}

	return m_pInterFaceInfo[ifaceNo].iiAddress.AddressIn.sin_addr.s_addr;
}

ULONG SOCmnUDPSocket::getIfaceNetMask(UINT ifaceNo) const
{
	if (ifaceNo >= m_numberOfInterfaces)
	{
		return 0L;
	}

	return m_pInterFaceInfo[ifaceNo].iiNetmask.AddressIn.sin_addr.s_addr;
}

ULONG SOCmnUDPSocket::getIfaceBcastAddr(UINT ifaceNo) const
{
	if (ifaceNo >= m_numberOfInterfaces)
	{
		return 0L;
	}

	unsigned long bcast = m_pInterFaceInfo[ifaceNo].iiAddress.AddressIn.sin_addr.s_addr;
	bcast |= ~(m_pInterFaceInfo[ifaceNo].iiNetmask.AddressIn.sin_addr.s_addr);
	return bcast;
}

void SOCmnUDPSocket::setBroadcastAddress(ULONG bcastAddr)
{
	m_broadcastAddress = bcastAddr;
}


#endif // SOFEATURE_CMN_UDP_SOCKET

