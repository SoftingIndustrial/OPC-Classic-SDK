#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_TUNNEL
#ifdef SOFEATURE_TP_DISCOVERY

#include "SOClt.h"
#include "SOCmnTP.h"
#include "SOCltTPDiscovery.h"


SOCltTPDiscovery::SOCltTPDiscovery(
	SOCmnObject* parent /*=NULL*/) : SOCmnObject()
{
	setObjectState(SOCMNSOCKET_STATE_INIT);
	SOCMN_ALLOCATE_OBJECT_C(m_udpCommunicationSender, SOCmnUDPSocket(), (SOCmnUDPSocket*));
	SOCMN_ALLOCATE_OBJECT_C(m_udpCommunicationReceiver, SOCmnUDPSocket(), (SOCmnUDPSocket*));
	SOCMN_ALLOCATE_OBJECT_C(m_listenThread, SOCmnThread(), (SOCmnThread*));
	SOCMN_ALLOCATE_OBJECT_C(m_marshaler, SOCmnTP_Marshaler(0), (SOCmnTP_Marshaler*));
	SOCMN_ALLOCATE_OBJECT_C(m_unMarshaler, SOCmnTP_UnMarshaler(0), (SOCmnTP_UnMarshaler*));
	m_listenEvent = m_listenThread->createEvent();
	m_unicastEvent = m_listenThread->createEvent();
	m_broadcastEvent = m_listenThread->createEvent();
	m_deviceIdentityList.create(SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_SYNCED);
} // end ctor



SOCltTPDiscovery::~SOCltTPDiscovery(
	void)
{
	if (m_listenThread != NULL)
	{
		delete m_listenThread;
		m_listenThread = NULL;
	} // end if
} // end ctor



BOOL SOCltTPDiscovery::DeviceIdentity::setDemoEventName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_demoEventName = name;
	return TRUE;
} // end setDemoEventName



BOOL SOCltTPDiscovery::DeviceIdentity::setRunningEventName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_runningEventName = name;
	return TRUE;
} // end setRunningEventName



BOOL SOCltTPDiscovery::DeviceIdentity::setServerName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_serverName = name;
	return TRUE;
} // end setServerName



BOOL SOCltTPDiscovery::DeviceIdentity::setVendorInfo(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_vendorInfo = name;
	return TRUE;
} // end setRunningEventName



BOOL SOCltTPDiscovery::DeviceIdentity::setHostName(
	const SOCmnString& name)
{
	if (name.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_hostName = name;
	return TRUE;
} // end setHostName



BOOL SOCltTPDiscovery::DeviceIdentity::setStatus(
	USHORT aStatus)
{
	if (aStatus != TP_DISCOVERY_RESPONSE_DEVICE_STATUS_DEMO &&
		aStatus != TP_DISCOVERY_RESPONSE_DEVICE_STATUS_RUNNING)
	{
		m_status = TP_DISCOVERY_RESPONSE_DEVICE_STATUS_UNKNOWN;
	} // end if

	m_status = aStatus;
	return TRUE;
} // end if



BOOL SOCltTPDiscovery::DeviceIdentity::setUrl(
	const SOCmnString& url)
{
	if (url.isEmpty() == TRUE)
	{
		return FALSE;
	} // end if

	m_url = url;
	m_urlWithIp = url;
	return TRUE;
} // end setUrl



BOOL SOCltTPDiscovery::DeviceIdentity::setIpOnUrl(
	const SOCmnString& anIp)
{
	SOCmnString ipForLocalhost(_T("/"));
	ipForLocalhost += anIp;
	ipForLocalhost += _T(":");
	m_urlWithIp = getUrl();

	if (m_urlWithIp.find(_T("/localhost:")) > 0)
	{
		m_urlWithIp.replace(_T("/localhost:"), ipForLocalhost);
	} // end while

	return TRUE;
} // end setIpOnUrl



BOOL SOCltTPDiscovery::initCommunication(
	const SOCmnString& address,
	USHORT port,
	USHORT localPort,
	DWORD timeout)
{
	if (port < 0 || port >= USHRT_MAX)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("initCommunication: invalid tunnel discovery port number")));
		return FALSE;
	} // end if

	if (localPort < 0 || localPort >= USHRT_MAX)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("initCommunication: invalid local port number")));
		return FALSE;
	} // end if

	if (timeout <= 0)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("initCommunication: invalid tunnel discovery timeout")));
		return FALSE;
	} // end if

	if (address.isEmpty() != TRUE)
	{
		USES_CONVERSION;
		m_udpCommunicationSender->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)address)));
	} // end if

	// send on userDefined port + 1 !!! - needed if running on same machine
	USHORT sendPort = port + 1;
	m_udpCommunicationSender->setBroadCastPort(sendPort);
	m_udpCommunicationSender->setLocalPort(localPort);
	m_udpCommunicationSender->setBroadcastReplyTimeOut(timeout);
	m_udpCommunicationReceiver->setBroadCastPort(port);
	m_udpCommunicationReceiver->setLocalPort(localPort);
	m_udpCommunicationReceiver->setBroadcastReplyTimeOut(timeout);

	if (m_udpCommunicationSender->init(FALSE) &&
		m_udpCommunicationReceiver->init(TRUE))
	{
		m_listenThread->start(listenThreadProc, this);
		_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("initCommunication: ok on - %s:%d with timeout %d"), address, port, timeout));
		return TRUE;
	} // end if

	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("initCommunication: not ok on - %s:%d with timeout %d"), address, port, timeout));
	return FALSE;
} // end initCommunication



BOOL SOCltTPDiscovery::addIdentity(
	const SOCmnString& url,
	const SOCmnString& demoEvent,
	const SOCmnString& runningEvent,
	const SOCmnString& serverName,
	const SOCmnString& vendorInfo,
	USHORT majorVersion,
	USHORT minorVersion,
	USHORT patchVersion,
	ULONG buildNumber)
{
	SOCmnPointer<DeviceIdentity> pDeviceIdentity;

	if (m_deviceIdentityList.findKey(url, pDeviceIdentity) == TRUE)
	{
		// device already added
		_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("addIdentity: identity %s [0x%X] already in list"), url, pDeviceIdentity));
		return TRUE;
	} // end if

	SOCMN_ALLOCATE_OBJECT_C(pDeviceIdentity, DeviceIdentity(), (DeviceIdentity*));
	SOCmnString hostName;
	m_udpCommunicationSender->getHostName(hostName);
	pDeviceIdentity->setUrl(url);
	pDeviceIdentity->setDemoEventName(demoEvent);
	pDeviceIdentity->setRunningEventName(runningEvent);
	pDeviceIdentity->setServerName(serverName);
	pDeviceIdentity->setVendorInfo(vendorInfo);
	pDeviceIdentity->setHostName(hostName);
	pDeviceIdentity->setMajorVersion(majorVersion);
	pDeviceIdentity->setMinorVersion(minorVersion);
	pDeviceIdentity->setPatchVersion(patchVersion);
	pDeviceIdentity->setBuildNumber(buildNumber);
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("addIdentity: identity %s [0x%X] added to list"), url, pDeviceIdentity));
	return m_deviceIdentityList.add(pDeviceIdentity, url);
} // end setIdentity



BOOL SOCltTPDiscovery::removeIdentity(
	const SOCmnString& url)
{
	SOCmnPointer<DeviceIdentity> pDevice;

	if (url.isEmpty() == TRUE || m_deviceIdentityList.findKey(url, pDevice) == FALSE)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("removeIdentity: failed on argument check %s, %d"), url, pDevice));
		return FALSE;
	} // end if

	if (pDevice.isNotNull() == FALSE)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("removeIdentity: identity %s not in list"), url));
		return FALSE;
	} // end if

	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("removeIdentity: identity %s removed from list"), url));
	return m_deviceIdentityList.removeKey(url);
} // end removeIdentity



BOOL SOCltTPDiscovery::updateIdenty(
	const SOCmnString& url,
	USHORT status)
{
	SOCmnPointer<DeviceIdentity> pDevice;

	if (url.isEmpty() == TRUE || m_deviceIdentityList.findKey(url, pDevice) == FALSE)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("updateIdentity: failed on argument check %s, %d"), url, pDevice));
		return FALSE;
	} // end if

	if (pDevice.isNotNull() == FALSE)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("updateIdentity: identity %s not in list"), url));
		return FALSE;
	} // end if

	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("removeIdentity: identity %s updated in list"), url));
	return pDevice->setStatus(status);
} // end updateIdentity



BOOL SOCltTPDiscovery::start(
	void)
{
	m_listenThread->getEvents()->signal(m_listenEvent);
	setObjectState(SOCMNSOCKET_STATE_LISTEN);
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("start: tunnel discovery client service started succesfully")));
	return TRUE;
} // end ctor



BOOL SOCltTPDiscovery::stop(
	void)
{
	m_udpCommunicationSender->close();
	m_udpCommunicationReceiver->close();
	m_listenThread->stop();
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("stop: tunnel discovery client service stopped succesfully")));
	return TRUE;
} // end ctor



BOOL SOCltTPDiscovery::handleRequest(
	UCHAR* pMessage,
	ULONG size)
{
	if (pMessage == NULL || size == 0)
	{
		m_listenThread->getEvents()->signal(m_listenEvent);
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("handleRequest: failed on argument checking %d %d"), pMessage, size));
		return FALSE;
	} // end if

	// unmarshal the message
	m_unMarshaler->SetMarshalData(pMessage, size);
	SOCmnString requestIp;
	m_unMarshaler->UnMarshalString(&requestIp);
	T_TP_REQ_HEADER request;
	m_unMarshaler->UnMarshalShort(&request.Length);
	m_unMarshaler->UnMarshalShort(&request.ResponseType);
	m_unMarshaler->UnMarshalShort(&request.Version);
	m_unMarshaler->UnMarshalShort(&request.Service);
	m_unMarshaler->UnMarshalShort(&request.DeviceType);

	// check if really is discovery message
	if ((request.DeviceType != TP_DISCOVERY_DEVICE_TYPE_ALL &&
		 request.DeviceType != TP_DISCOVERY_DEVICE_TYPE_OPC_TUNNEL &&
		 request.DeviceType != TP_DISCOVERY_DEVICE_TYPE_OPC_XMLDA) ||
		request.Service != TP_DISCOVERY_SERVICE)
	{
		m_listenThread->getEvents()->signal(m_listenEvent);
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("handleRequest: not a discovery message %d %d"), pMessage, size));
		return FALSE;
	} // end if

	// if multi-interface setup determine IP address
	setIpAddressOnDevices(m_udpCommunicationReceiver->getBroadcastReceiverInterface());
	m_listenThread->getEvents()->reset(m_listenEvent);
	// always answer only to asker, do not broadcast response
	requestIp = m_udpCommunicationReceiver->getSenderAddress();

	if (requestIp)
	{
		sendUnicastResponse(request.DeviceType);
	}
	else
	{
		// just wait for another request
		m_listenThread->getEvents()->signal(m_listenEvent);
	} // end if .. else

	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("handleRequest: succesfully handled request %d %d"), pMessage, size));
	return TRUE;
} // end if



BOOL SOCltTPDiscovery::sendUnicastResponse(
	USHORT deviceType)
{
	USES_CONVERSION;
	// no broadcast - just set the sender's address
	SOCmnString sender = m_udpCommunicationReceiver->getSenderAddress();
	m_udpCommunicationSender->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)sender)));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("sendUnicastResponse: sending unicast response")));
	return sendResponse(deviceType, false);
} // end sendUnicastResponse



BOOL SOCltTPDiscovery::sendBroadcastResponse(
	USHORT deviceType)
{
	USES_CONVERSION;
	// default it back to broadcast address
	SOCmnString sender(_T("255.255.255.255"));
	m_udpCommunicationSender->setBroadcastAddress(inet_addr(T2CA((LPTSTR)sender)));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("sendUnicastResponse: sending broadcast response")));
	return sendResponse(deviceType, true);
} // end sendBroadcastResponse



BOOL SOCltTPDiscovery::sendResponse(
	USHORT deviceType,
	bool sendBroadcast)
{
	UCHAR* message = NULL;
	ULONG size = 0;
	SOCmnListPosition pos = m_deviceIdentityList.getStartPosition();

	while (pos)
	{
		DeviceIdentity* pDevice = m_deviceIdentityList.getNext(pos);

		if (pDevice != NULL)
		{
			message = NULL;
			size = 0;

			// send only response if device is matching request type
			if ((deviceType == TP_DISCOVERY_DEVICE_TYPE_ALL) || // all
				(deviceType == TP_DISCOVERY_DEVICE_TYPE_OPC_TUNNEL && pDevice->isTunnel() == TRUE) || // tunnel
				(deviceType == TP_DISCOVERY_DEVICE_TYPE_OPC_XMLDA && pDevice->isXmlDa() == TRUE))
			{
				if (marshalResponse(pDevice, &message, size, deviceType) == TRUE)
				{
					if (sendBroadcast)
					{
						for (UINT i = 0L, n = m_udpCommunicationSender->getNoOfIfaces(); i < n; ++i)
						{
							m_udpCommunicationSender->setBroadcastAddress(m_udpCommunicationSender->getIfaceBcastAddr(i));
							m_udpCommunicationSender->sendData((char*)message, size);
							_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("sendResponse: sent response %s of size %d"), message, size));
						}
					}
					else
					{
						USES_CONVERSION;
						m_udpCommunicationSender->setBroadcastAddress(inet_addr(T2CA((LPCTSTR)m_udpCommunicationReceiver->getSenderAddress())));
						m_udpCommunicationSender->sendData((char*)message, size);
						_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("sendResponse: sent response %s of size %d"), message, size));
					}
				} // end if
			} // end if
		} // end if
	} // end while

	m_listenThread->getEvents()->signal(m_listenEvent);
	return TRUE;
} // end sendResponse



BOOL SOCltTPDiscovery::setIpAddressOnDevices(
	const SOCmnString& anIpAddress)
{
	if (m_deviceIdentityList.isEmpty() == TRUE)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("setIpAddressOnDevices: no devices in list")));
		return FALSE;
	} // end if

	SOCmnString ipForLocalhost(_T("/"));
	ipForLocalhost += anIpAddress;
	ipForLocalhost += _T(":");
	SOCmnListPosition pos = m_deviceIdentityList.getStartPosition();

	while (pos)
	{
		DeviceIdentity* pDevice = m_deviceIdentityList.getNext(pos);

		if (pDevice != NULL)
		{
			pDevice->setIpOnUrl(anIpAddress);
		} // end if
	} // end while

	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("setIpAddressOnDevices: IP address %s set on all devices"), anIpAddress));
	return TRUE;
} // end setIpAddressOnDevices



BOOL SOCltTPDiscovery::marshalResponse(
	DeviceIdentity* pDevice,
	UCHAR** message,
	ULONG& size,
	USHORT deviceType)
{
	// check response type
	if (deviceType == TP_DISCOVERY_DEVICE_TYPE_OPC_TUNNEL &&
		!pDevice->isTunnel())
	{
		// no tunnel IP is set
		_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("marshalResponse: tunnel response requested but no tunnel ip is defined")));
		return FALSE;
	} // end if

	if (deviceType == TP_DISCOVERY_DEVICE_TYPE_OPC_XMLDA &&
		!pDevice->isXmlDa())
	{
		// no http port or XMLDA url is set
		_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("marshalResponse: xmlda response requested but no xmlda url is defined")));
		return FALSE;
	} // end if

	T_TP_DEVICE_RES response;
	response.Header.Length = TP_DISCOVERY_RESPONSE_HEADER_LENGTH;
	response.Header.Version = TP_DISCOVERY_RESPONSE_VERSION_NUMBER;
	response.Header.Service = TP_DISCOVERY_RESPONSE_SERVICE;
	response.Header.DeviceType = deviceType;
	response.Header.Status = pDevice->getStatus();
	memcpy(response.Url, pDevice->getUrlWithIp(), sizeof(response.Url));
	memcpy(response.ServerName, pDevice->getServerName(), sizeof(response.ServerName));
	memcpy(response.VendorInfo, pDevice->getVendorInfo(), sizeof(response.VendorInfo));
	memcpy(response.HostName, pDevice->getHostName(), sizeof(response.HostName));
	response.MajorVersion = pDevice->getMajorVersion();
	response.MinorVersion = pDevice->getMinorVersion();
	response.PatchVersion = pDevice->getPatchVersion();
	response.BuildNumber = pDevice->getBuildNumber();
	m_marshaler->BeginMarshal(sizeof(T_TP_DEVICE_RES));
	getMarshaler()->MarshalShort(response.Header.Length);
	getMarshaler()->MarshalShort(response.Header.Version);
	getMarshaler()->MarshalShort(response.Header.Service);
	getMarshaler()->MarshalShort(response.Header.Status);
	getMarshaler()->MarshalShort(response.Header.DeviceType);
	getMarshaler()->MarshalString(pDevice->getUrlWithIp());
	getMarshaler()->MarshalString(pDevice->getServerName());
	getMarshaler()->MarshalString(pDevice->getVendorInfo());
	getMarshaler()->MarshalShort(response.MajorVersion);
	getMarshaler()->MarshalShort(response.MinorVersion);
	getMarshaler()->MarshalShort(response.PatchVersion);
	getMarshaler()->MarshalLong(response.BuildNumber);
	getMarshaler()->MarshalString(pDevice->getHostName());

	if (getMarshaler()->GetMarshalDataLen() <= 0 ||
		getMarshaler()->GetMarshalData() == 0)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("marshalResponse: invalid marshaled data for: %s"), pDevice->getUrl()));
		return FALSE;
	} // end if

	// else allocate and copy the message
	*message = (UCHAR*)malloc(getMarshaler()->GetMarshalDataLen());
	memcpy(*message, getMarshaler()->GetMarshalData(), getMarshaler()->GetMarshalDataLen());
	size = getMarshaler()->GetMarshalDataLen();
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("marshalResponse: succesfully marshaled response for: %s"), pDevice->getUrl()));
	return TRUE;
} // end marshalResponse



DWORD _stdcall SOCltTPDiscovery::listenThreadProc(
	void* pContext)
{
	if (pContext == NULL)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_DISCOVERY_CLT, _T("SOCltTPDiscovery"), (_T("listenThreadProc: invalid context")));
		return S_FALSE;
	} // end if

	SOCltTPDiscovery* pT = reinterpret_cast<SOCltTPDiscovery*>(pContext);
	pT->addRef();
	SOCmnThread* pMe = pT->getListenThread();
	DWORD waitR;
	BOOL end = FALSE;
	SOCmnUDPSocket* pCommunicationHandler = pT->getUdpCommunicationReceiver();
	DWORD dwWait = pCommunicationHandler->getBroadcastReplyTimeOut();

	while (!end)
	{
		waitR = pMe->waitForEvents(dwWait);

		if (waitR == pMe->getStopEventId())
		{
			end = TRUE;
		}
		else if (waitR == pT->getListenEvent())
		{
			UCHAR* pMessage = NULL;
			ULONG size = 0;

			while (TRUE)
			{
				if (pCommunicationHandler->hasPendingData() == TRUE)
				{
					if (pCommunicationHandler->receiveData((char**)&pMessage, (ULONG)size) == S_OK)
					{
						pT->handleRequest(pMessage, size);
					}
					else
					{
						//error = SOCmnUDPSocket::WSAHandler::instance()->getLastError();
						pMe->getEvents()->signal(pT->getListenEvent());
						break;
					} // end if .. else
				}
				else
				{
					//error = SOCmnUDPSocket::WSAHandler::instance()->getLastError();
					pMe->getEvents()->signal(pT->getListenEvent());
					break;
				} // end if .. else
			} // end while
		} // end else .. if
	} // end while

	pT->release();
	return S_OK;
} // end listenThreadProc

#endif // SOFEATURE_TP_DISCOVERY
#endif // SOFEATURE_TUNNEL
#endif // SOCLT
