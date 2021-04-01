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
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvTPDiscovery.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : TP server related classes                                   |
//                - SOSrvTPDiscovery: TP server object                                 |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_TUNNEL
#ifdef SOFEATURE_TP_DISCOVERY

#include "SOSrv.h"
#include "SOCmnTP.h"
#include "SOSrvTPDiscovery.h"

SOSrvTPDiscovery::SOSrvTPDiscovery(SOCmnObject* parent) : SOCmnObject()
{
	setObjectState(SOCMNSOCKET_STATE_INIT);
	SOCMN_ALLOCATE_OBJECT_C(m_udpCommunicationHandlerSnd, SOCmnUDPSocket(), (SOCmnUDPSocket*));
	SOCMN_ALLOCATE_OBJECT_C(m_udpCommunicationHandlerRcv, SOCmnUDPSocket(), (SOCmnUDPSocket*));
	SOCMN_ALLOCATE_OBJECT_C(m_listenThread, SOCmnThread(), (SOCmnThread*));
	SOCMN_ALLOCATE_OBJECT_C(m_marshaler, SOCmnTP_Marshaler(0), (SOCmnTP_Marshaler*));
	SOCMN_ALLOCATE_OBJECT_C(m_unMarshaler, SOCmnTP_UnMarshaler(0), (SOCmnTP_UnMarshaler*));
	m_listenSendEvent = m_listenThread->createEvent();
	m_listenReceiveEvent = m_listenThread->createEvent();

	if (m_deviceList.create(SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_SYNCED))
	{
		_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "), (_T("Device list created.")));
	}
	else
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "), (_T("Device list could not be created.")));
	}//end if..else

	m_isPendingData = TRUE;
	m_discoveryDeviceType = DISCOVERY_DEVICE_TYPE_ALL;
	m_discoveryRequestType = TP_DISCOVERY_REQUEST_TYPE_BROADCAST;
	m_ipAddress = _T("255.255.255.255");
} // end ctor

SOSrvTPDiscovery::~SOSrvTPDiscovery(void)
{
	if (m_listenThread != NULL)
	{
		delete m_listenThread;
		m_listenThread = NULL;
	} // end if
} // end dtor


BOOL SOSrvTPDiscovery::DeviceIdentity::setServerName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_serverName = name;
	return TRUE;
} // end setServerName



BOOL SOSrvTPDiscovery::DeviceIdentity::setVendorInfo(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_vendorInfo = name;
	return TRUE;
} // end setRunningEventName



BOOL SOSrvTPDiscovery::DeviceIdentity::setHostName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_hostName = name;
	return TRUE;
} // end setHostName


BOOL SOSrvTPDiscovery::DeviceIdentity::setUrl(
	const SOCmnString& url)
{
	if (url.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_url = url;
	return TRUE;
} // end setUrl


BOOL SOSrvTPDiscovery::initCommunication(
	const SOCmnString& address,
	USHORT port,
	USHORT localPort,
	DWORD timeout)
{
	USES_CONVERSION;

	if (address.isEmpty() != TRUE)
	{
		m_udpCommunicationHandlerSnd->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)address)));
	} // end if

	m_udpCommunicationHandlerSnd->setBroadCastPort(port);
	m_udpCommunicationHandlerSnd->setLocalPort(localPort);
	m_udpCommunicationHandlerSnd->setBroadcastReplyTimeOut(timeout);
	m_udpCommunicationHandlerSnd->init(FALSE);

	if (address.isEmpty() != TRUE)
	{
		m_udpCommunicationHandlerRcv->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)address)));
	} // end if

	m_udpCommunicationHandlerRcv->setBroadCastPort(port + 1);
	m_udpCommunicationHandlerRcv->setLocalPort(localPort);
	m_udpCommunicationHandlerRcv->setBroadcastReplyTimeOut(timeout);
	m_udpCommunicationHandlerRcv->init(TRUE);
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
			(_T("The communication to devices was establish.")));
	return TRUE;
} // end initCommunication


BOOL SOSrvTPDiscovery::init(
	const DiscoveryDeviceType& discoveryDeviceType,
	const DiscoveryRequestType& discoveryRequestType,
	const SOCmnString& ipAddress)
{
	m_discoveryDeviceType = discoveryDeviceType;
	m_discoveryRequestType = discoveryRequestType;

	if (m_discoveryRequestType == TP_DISCOVERY_REQUEST_TYPE_UNICAST)
	{
		m_ipAddress = ipAddress;
	} // end if

	if (m_ipAddress != _T("255.255.255.255"))
	{
		USES_CONVERSION;
		m_udpCommunicationHandlerRcv->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)ipAddress)));
		m_udpCommunicationHandlerSnd->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)ipAddress)));
	}

	m_listenThread->start(listenThreadProc, this);
	setObjectState(SOCMNSOCKET_STATE_LISTEN);
	return TRUE;
} // end startSend


BOOL SOSrvTPDiscovery::scanForDevices(
	void)
{
	if (m_isPendingData == FALSE)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "), (_T("Scaning for devices could not be started.")));
		m_isPendingData = TRUE;
		return FALSE;
	}
	else
	{
		m_listenThread->getEvents()->signal(m_listenSendEvent);
	}//end if..else;

	return TRUE;
}

BOOL SOSrvTPDiscovery::stop(
	void)
{
	m_udpCommunicationHandlerRcv->close();
	m_udpCommunicationHandlerSnd->close();
	m_listenThread->stop();
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
			(_T("Scaning for devices stopped.")));
	return TRUE;
} // end stopSend


BOOL SOSrvTPDiscovery::marshalRequest(
	UCHAR** message,
	ULONG& size)
{
	T_TP_REQ_HEADER request;
	request.Length = TP_DISCOVERY_REQUEST_HEADER_LENGTH;
	request.ResponseType = (unsigned short)m_discoveryRequestType;
	request.Version = TP_DISCOVERY_VERSION_NUMBER;
	request.Service = TP_DISCOVERY_SERVICE;
	request.DeviceType = (unsigned short)m_discoveryDeviceType;;
	m_marshaler->BeginMarshal(sizeof(T_TP_REQ_HEADER));
	getMarshaler()->MarshalString(m_ipAddress);
	getMarshaler()->MarshalShort(request.Length);
	getMarshaler()->MarshalShort(request.ResponseType);
	getMarshaler()->MarshalShort(request.Version);
	getMarshaler()->MarshalShort(request.Service);
	getMarshaler()->MarshalShort(request.DeviceType);

	if (getMarshaler()->GetMarshalDataLen() <= 0 ||
		getMarshaler()->GetMarshalData() == 0)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "), (_T("The request message could not be marshaled.")));
		return FALSE;
	} // end if

	// else allocate and copy the message
	size = getMarshaler()->GetMarshalDataLen();
	*message = (UCHAR*)malloc(size);
	memcpy(*message, getMarshaler()->GetMarshalData(), size);
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
			(_T("The request message contains: IP = %s, Length = %d, ResponseType = %d, Version = %d, Service = %d, DeviceType = %d"),
			 request.Ip, request.Length, request.ResponseType, request.Version, request.Service, request.DeviceType));
	return TRUE;
} // end marshalRequest


DWORD _stdcall SOSrvTPDiscovery::listenThreadProc(
	void* pContext)
{
	if (pContext == NULL)
	{
		return S_FALSE;
	} // end if

	SOSrvTPDiscovery* pT = reinterpret_cast<SOSrvTPDiscovery*>(pContext);
	pT->addRef();
	SOCmnThread* pMe = pT->getListenThread();
	DWORD waitR;
	BOOL end = FALSE;
	DWORD dwWait = 500;
	UCHAR* pSentMessage = NULL;
	ULONG sizeSentMessage = 0;
	SOCmnPointer<SOCmnUDPSocket> pCommunicationHandlerSnd = pT->getCommunicationHandleSnd();
	SOCmnPointer<SOCmnUDPSocket> pCommunicationHandlerRcv = pT->getCommunicationHandleRcv();

	while (!end)
	{
		waitR = pMe->waitForEvents(dwWait);

		if (waitR == pMe->getStopEventId())
		{
			end = TRUE;
		}
		else if (waitR == pT->getListenSendEvent())
		{
			pMe->getEvents()->reset(pT->getListenSendEvent());
			bool sentOk = false;

			if (pT->m_ipAddress == _T("255.255.255.255"))
			{
				USES_CONVERSION;
				UINT n = pCommunicationHandlerSnd->getNoOfIfaces();

				for (UINT i = 0; i < n; ++i)
				{
					if (pCommunicationHandlerSnd->getIfaceAddress(i) != 0x0100007F) // != 127.0.0.1
					{
						IN_ADDR inetBcastAddr = {0};
						inetBcastAddr.S_un.S_addr = pCommunicationHandlerSnd->getIfaceBcastAddr(i);
						pCommunicationHandlerSnd->setBroadcastAddress(inetBcastAddr.S_un.S_addr);
						pT->m_ipAddress.format(_T("%s"), A2W(inet_ntoa(inetBcastAddr)));
						pT->marshalRequest(&pSentMessage, sizeSentMessage);

						if (pCommunicationHandlerSnd->sendData((char*)pSentMessage, sizeSentMessage) == S_OK)
						{
							sentOk = true;
						}
					}
				}
			}
			else
			{
				pT->marshalRequest(&pSentMessage, sizeSentMessage);

				if (pCommunicationHandlerSnd->sendData((char*)pSentMessage, sizeSentMessage) == S_OK)
				{
					sentOk = true;
				}
			}

			if (sentOk)
			{
				pMe->getEvents()->signal(pT->getListenReceiveEvent());
				pMe->getEvents()->reset(pT->getListenSendEvent());
				_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
						(_T("The request message was sent.")));
			}
			else
			{
				pMe->getEvents()->signal(pT->getListenSendEvent());
				_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
						(_T("The request message was not sent, it will be resend")));
			}

			delete pSentMessage;
			pSentMessage = NULL;
		}//end if..else
		else if (waitR == pT->getListenReceiveEvent())
		{
			_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
					(_T("I is waiting for response.")));
			UCHAR* pReceivedMessage = NULL;
			ULONG sizeReceivedMessage = 0;
			int counter = 0;

			while (TRUE)
			{
				if (pCommunicationHandlerRcv->hasPendingData() == TRUE)
				{
					if (pCommunicationHandlerRcv->receiveData((char**)&pReceivedMessage, (ULONG)sizeReceivedMessage) == S_OK)
					{
						pT->handleResponse(pReceivedMessage, sizeReceivedMessage);
						counter++;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			if (counter == 0)
			{
				_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
						(_T("No device had response to the request.")));
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
						(_T("%d devices had response to the request."), counter));
			}//end if..else

			pMe->getEvents()->reset(pT->getListenReceiveEvent());
		}//end if..else
	} // end while

	pT->m_isPendingData = FALSE;
	pT->release();
	return S_OK;
} // end listenThreadProc


BOOL SOSrvTPDiscovery::handleResponse(
	UCHAR* pMessage,
	ULONG size)
{
	if (pMessage == NULL || size == 0)
	{
		return FALSE;
	} // end if

	// unmarshal the message
	m_unMarshaler->SetMarshalData(pMessage, size);
	T_TP_DEVICE_RES response;
	m_unMarshaler->UnMarshalShort(&response.Header.Length);

	if (response.Header.Length == TP_DISCOVERY_RESPONSE_HEADER_LENGTH)
	{
		m_unMarshaler->UnMarshalShort(&response.Header.Version);
		m_unMarshaler->UnMarshalShort(&response.Header.Service);
		m_unMarshaler->UnMarshalShort(&response.Header.Status);
		m_unMarshaler->UnMarshalShort(&response.Header.DeviceType);
		SOCmnString url;
		m_unMarshaler->UnMarshalString(&url);
		SOCmnString serverName;
		m_unMarshaler->UnMarshalString(&serverName);
		SOCmnString vendorInfo;
		m_unMarshaler->UnMarshalString(&vendorInfo);
		m_unMarshaler->UnMarshalShort(&response.MajorVersion);
		m_unMarshaler->UnMarshalShort(&response.MinorVersion);
		m_unMarshaler->UnMarshalShort(&response.PatchVersion);
		m_unMarshaler->UnMarshalLong(&response.BuildNumber);
		SOCmnString hostName;
		m_unMarshaler->UnMarshalString(&hostName);

		if ((response.Header.DeviceType != TP_DISCOVERY_RESPONSE_DEVICE_TYPE) &&
			(response.Header.Service != TP_DISCOVERY_RESPONSE_SERVICE))
		{
			return FALSE;
		} // end if

		_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_SRV, _T("Tunnel Discovery: "),
				(_T("Device with IP address: %s had response. It was added to the device list"), hostName));
		BOOL exists = FALSE;
		SOCmnListPosition pos = m_deviceList.getStartPosition();

		while (pos)
		{
			DeviceIdentity* devIdent = m_deviceList.getNext(pos);

			// current configurations should be visible under remote node with all of the machine`s ip addresses
			// therefore no exclusion is done by host and configuration name
			if (devIdent->getUrl() == url)
			{
				exists = TRUE;
			}
		} // end while

		if (exists == FALSE)
		{
			DeviceIdentity* pDeviceIdentity = NULL;
			SOCMN_ALLOCATE_OBJECT_C(pDeviceIdentity, DeviceIdentity(), (DeviceIdentity*));
			pDeviceIdentity->setUrl(&url);
			pDeviceIdentity->setServerName(&serverName);
			pDeviceIdentity->setVendorInfo(&vendorInfo);
			pDeviceIdentity->setHostName(&hostName);
			pDeviceIdentity->setMajorVersion(response.MajorVersion);
			pDeviceIdentity->setMinorVersion(response.MinorVersion);
			pDeviceIdentity->setPatchVersion(response.PatchVersion);
			pDeviceIdentity->setBuildNumber(response.BuildNumber);
			m_deviceList.add(pDeviceIdentity);
		} // end if
	}//end if

	return TRUE;
} // end handleResponse


SOCmnList<SOSrvTPDiscovery::DeviceIdentity>& SOSrvTPDiscovery::getDeviceList(
	void)
{
	return m_deviceList;
}//end getDeviceList

#endif  // SOFEATURE_TP_DISCOVERY
#endif  // SOFEATURE_TUNNEL
#endif  // SOSRV
