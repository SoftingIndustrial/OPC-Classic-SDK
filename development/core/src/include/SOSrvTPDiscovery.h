//-----------------------------------------------------------------------------
#ifndef _SOSRVTPDISCOVERY_H_
#define _SOSRVTPDISCOVERY_H_

#ifdef SOFEATURE_TP_DISCOVERY

#include "SOCmnUDPSocket.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

class SOSrvTPDiscovery;

//	callback declaration for search expiration function
typedef BOOL (CALLBACK* SEARCHEXPIRED)(SOSrvTPDiscovery* /* SOSrvTPDiscovery*/, void* /* pContest*/);

class SOSRV_EXPORT SOSrvTPDiscovery : public SOCmnObject
{
	// message header constants
	static const unsigned short TP_DISCOVERY_REQUEST_HEADER_LENGTH = 0xA000;
	/*
	static const unsigned short TP_DISCOVERY_RESPONSE_TYPE_BROADCAST = 0x0000;
	static const unsigned short TP_DISCOVERY_RESPONSE_TYPE_UNICAST = 0x0100;
	*/
	static const unsigned short TP_DISCOVERY_VERSION_NUMBER = 0x1000;
	static const unsigned short TP_DISCOVERY_SERVICE = 0x0001;
	/*
	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_ALL = 0x0000;
	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_OPC_TUNNEL = 0x4000;
	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_OPC_XMLDA = 0x8000;
	*/
	// message response constants
	static const unsigned short TP_DISCOVERY_RESPONSE_HEADER_LENGTH = 0x5B00;
	static const unsigned short TP_DISCOVERY_RESPONSE_VERSION_NUMBER = 0x1000;
	static const unsigned short TP_DISCOVERY_RESPONSE_SERVICE = 0x0101;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_STATUS = 0x0000;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_TYPE = 0x8000;

	static const unsigned short IP_ADDRESS_MAX_DIMENSION = 4;
	static const unsigned short HOST_NAME_MAX_DIMENSION = 65;
	static const unsigned short INFO_MAX_DIMESION = 255;

	// request message header
	typedef struct _T_TP_REQ_HEADER
	{
		unsigned char    Ip[HOST_NAME_MAX_DIMENSION];
		unsigned short   Length;
		unsigned short   ResponseType;
		unsigned short   Version;
		unsigned short   Service;
		unsigned short   DeviceType;
	} T_TP_REQ_HEADER;

	// response message header
	typedef struct _T_TP_RES_HEADER
	{
		unsigned short   Length;
		unsigned short   Version;
		unsigned short   Service;
		unsigned short   Status;
		unsigned short   DeviceType;
	} T_TP_RES_HEADER;

	// response message
	typedef struct _T_TP_DEVICE_RES
	{
		T_TP_RES_HEADER Header;
		unsigned char    Url[INFO_MAX_DIMESION];
		unsigned char    ServerName[INFO_MAX_DIMESION];
		unsigned char    VendorInfo[INFO_MAX_DIMESION];
		unsigned char    HostName[HOST_NAME_MAX_DIMENSION];
		unsigned short   MajorVersion;
		unsigned short   MinorVersion;
		unsigned short   PatchVersion;
		unsigned long    BuildNumber;

	} T_TP_DEVICE_RES;

public:
	enum DiscoveryRequestType
	{
		TP_DISCOVERY_REQUEST_TYPE_BROADCAST = 0x000,
		TP_DISCOVERY_REQUEST_TYPE_UNICAST = 0x0100
	}; // end DiscoveryRequestType
	enum DiscoveryDeviceType
	{
		DISCOVERY_DEVICE_TYPE_ALL = 0x0000,
		DISCOVERY_DEVICE_TYPE_OPC_TUNNEL = 0x4000,
		DISCOVERY_DEVICE_TYPE_OPC_XMLDA = 0x8000
	}; // end DicoveryDeviceType

public:
	class DeviceIdentity : public SOCmnObject
	{
	public:
		DeviceIdentity(SOCmnObject* parent = NULL) : SOCmnObject() {};
		~DeviceIdentity(void) {};

		BOOL setUrl(const SOCmnString& anUrl);
		const SOCmnString& getUrl(void)
		{
			return m_url;
		};

		BOOL setServerName(const SOCmnString& aName);
		const SOCmnString& getServerName(void)
		{
			return m_serverName;
		};

		BOOL setVendorInfo(const SOCmnString& aName);
		const SOCmnString& getVendorInfo(void)
		{
			return m_vendorInfo;
		};

		BOOL setHostName(const SOCmnString& name);
		const SOCmnString& getHostName(void)
		{
			return m_hostName;
		};

		void setMajorVersion(USHORT version)
		{
			m_majorVersion = version;
		};
		USHORT getMajorVersion(void)
		{
			return m_majorVersion;
		};

		void setMinorVersion(USHORT version)
		{
			m_minorVersion = version;
		};
		USHORT getMinorVersion(void)
		{
			return m_minorVersion;
		};

		void setPatchVersion(USHORT version)
		{
			m_patchVersion = version;
		};
		USHORT getPatchVersion(void)
		{
			return m_patchVersion;
		};

		void setBuildNumber(ULONG buildNumber)
		{
			m_buildNumber = buildNumber;
		};
		ULONG getBuildNumber(void)
		{
			return m_buildNumber;
		};

		BOOL isTunnel(void)
		{
			if (m_url.find(_T("tpda:")) >= 0 || m_url.find(_T("tpae:")) >= 0)
			{
				return TRUE;
			}

			return FALSE;
		};
		BOOL isXmlDa(void)
		{
			if (m_url.find(_T("http:")) >= 0)
			{
				return TRUE;
			}

			return FALSE;
		};

	private:
		SOCmnString m_url;
		SOCmnString m_serverName;
		SOCmnString m_vendorInfo;
		SOCmnString m_hostName;
		USHORT m_majorVersion;
		USHORT m_minorVersion;
		USHORT m_patchVersion;
		ULONG m_buildNumber;


	};

public:
	SOSrvTPDiscovery(SOCmnObject* parent = NULL);
	~SOSrvTPDiscovery(void);

	BOOL initCommunication(const SOCmnString& address, USHORT port, USHORT localPort, DWORD timeout);

	virtual BOOL init(const DiscoveryDeviceType& discoveryDeviceType = DISCOVERY_DEVICE_TYPE_ALL,
					  const DiscoveryRequestType& discoveryRequestType = TP_DISCOVERY_REQUEST_TYPE_BROADCAST,
					  const SOCmnString& ipAddress = _T("255.255.255.255"));
	virtual BOOL scanForDevices();
	virtual BOOL stop(void);

	SOCmnList<DeviceIdentity>& getDeviceList(void);

protected:

	static DWORD _stdcall listenThreadProc(void* context);

	SOCmnThread* getListenThread(void)
	{
		return m_listenThread;
	}
	DWORD getListenSendEvent(void)
	{
		return m_listenSendEvent;
	};
	DWORD getListenReceiveEvent(void)
	{
		return m_listenReceiveEvent;
	};

	SOCmnUDPSocket* getCommunicationHandleSnd(void)
	{
		m_udpCommunicationHandlerSnd.addRef();
		return m_udpCommunicationHandlerSnd;
	};
	SOCmnUDPSocket* getCommunicationHandleRcv(void)
	{
		m_udpCommunicationHandlerRcv.addRef();
		return m_udpCommunicationHandlerRcv;
	};

	SOCmnTP_Marshaler* getMarshaler(void)
	{
		m_marshaler.addRef();
		return m_marshaler;
	};
	SOCmnTP_UnMarshaler* getUnMarshaler(void)
	{
		m_unMarshaler.addRef();
		return m_unMarshaler;
	};
	BOOL marshalRequest(UCHAR** message, ULONG& size);
	BOOL handleResponse(UCHAR* pResponse, ULONG pDataSize);

protected:
	SOCmnThread* m_listenThread;
	DWORD m_listenSendEvent;
	DWORD m_listenReceiveEvent;
	BOOL m_isPendingData;

private:
	SOCmnPointer<SOCmnUDPSocket> m_udpCommunicationHandlerSnd;
	SOCmnPointer<SOCmnUDPSocket> m_udpCommunicationHandlerRcv;
	SOCmnPointer<SOCmnTP_Marshaler> m_marshaler;
	SOCmnPointer<SOCmnTP_UnMarshaler> m_unMarshaler;
	SOCmnList<DeviceIdentity> m_deviceList;

	DiscoveryDeviceType m_discoveryDeviceType;
	DiscoveryRequestType m_discoveryRequestType;
	SOCmnString m_ipAddress;

}; //end SOSrvTPDiscovery

#pragma pack(pop)

#endif // SOFEATURE_TP_DISCOVERY

#endif // _SOSRVTPDISCOVERY_H_
