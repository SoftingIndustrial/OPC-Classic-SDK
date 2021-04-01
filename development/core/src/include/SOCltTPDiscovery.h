#ifndef _SOCLTTPDISCOVERY_H_
#define _SOCLTTPDISCOVERY_H_

#ifdef SOFEATURE_TUNNEL
#ifdef SOFEATURE_TP_DISCOVERY

#include "SOCmnUDPSocket.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


class SOCLT_EXPORT SOCltTPDiscovery : public SOCmnObject
{
	// request message header
	typedef struct _T_TP_REQ_HEADER
	{
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

		unsigned char    Url[255];
		unsigned char    DemoEventName[255];
		unsigned char    RunningEventName[255];
		unsigned char    ServerName[255];
		unsigned char    VendorInfo[255];
		unsigned char    HostName[65];
		unsigned short   MajorVersion;
		unsigned short   MinorVersion;
		unsigned short   PatchVersion;
		unsigned long    BuildNumber;
	} T_TP_DEVICE_RES;
public:
	// message header constants
	static const unsigned short TP_DISCOVERY_REQUEST_HEADER_LENGTH = 0xA000;
	static const unsigned short TP_DISCOVERY_RESPONSE_TYPE_BROADCAST = 0x0000;
	static const unsigned short TP_DISCOVERY_RESPONSE_TYPE_UNICAST = 0x0100;
	static const unsigned short TP_DISCOVERY_VERSION_NUMBER = 0x1000;
	static const unsigned short TP_DISCOVERY_SERVICE = 0x0001;

	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_ALL = 0x0000;
	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_OPC_TUNNEL = 0x4000;
	static const unsigned short TP_DISCOVERY_DEVICE_TYPE_OPC_XMLDA = 0x8000;


	// message response constants
	static const unsigned short TP_DISCOVERY_RESPONSE_HEADER_LENGTH = 0x5B00;
	static const unsigned short TP_DISCOVERY_RESPONSE_VERSION_NUMBER = 0x1000;
	static const unsigned short TP_DISCOVERY_RESPONSE_SERVICE = 0x0101;

	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_STATUS_RUNNING = 0x0000;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_STATUS_DEMO = 0x0001;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_STATUS_UNKNOWN = 0x0002;

	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_TYPE_ALL = 0x0000;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_TYPE_TUNNEL = 0x8000;
	static const unsigned short TP_DISCOVERY_RESPONSE_DEVICE_TYPE_XMLDA = 0x9000;

	static const unsigned short TP_OCT_TUNNEL_DISCOVERY_PORT = 53842;
	static const unsigned short TP_OCT_TUNNEL_DISCOVERY_TIMEOUT = 3000;

	// device identity
	class DeviceIdentity : public SOCmnObject
	{
	public:
		DeviceIdentity(SOCmnObject* parent = NULL) : SOCmnObject()
		{
			m_status = TP_DISCOVERY_RESPONSE_DEVICE_STATUS_UNKNOWN;
		};
		virtual ~DeviceIdentity(void) {};

		BOOL setDemoEventName(const SOCmnString& name);
		const SOCmnString& getDemoEventName(void)
		{
			return m_demoEventName;
		};

		BOOL setRunningEventName(const SOCmnString& name);
		const SOCmnString& getRunningEventName(void)
		{
			return m_runningEventName;
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

		BOOL setStatus(USHORT aStatus);
		USHORT getStatus(void)
		{
			return m_status;
		};

		BOOL setUrl(const SOCmnString& anUrl);
		const SOCmnString& getUrl(void)
		{
			return m_url;
		};

		BOOL setIpOnUrl(const SOCmnString& anIp);
		const SOCmnString& getUrlWithIp(void)
		{
			return m_urlWithIp;
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
		USHORT m_status;
		SOCmnString m_url;
		SOCmnString m_demoEventName;
		SOCmnString m_runningEventName;
		SOCmnString m_serverName;
		SOCmnString m_vendorInfo;
		SOCmnString m_hostName;
		USHORT m_majorVersion;
		USHORT m_minorVersion;
		USHORT m_patchVersion;
		ULONG m_buildNumber;
		SOCmnString m_urlWithIp;
	};

public:
	SOCltTPDiscovery(SOCmnObject* parent = NULL);
	virtual ~SOCltTPDiscovery(void);

	BOOL initCommunication(const SOCmnString& address, USHORT port, USHORT localPort, DWORD timeout);
	BOOL addIdentity(const SOCmnString& url,
					 const SOCmnString& demoEvent,
					 const SOCmnString& runningEvent,
					 const SOCmnString& serverName,
					 const SOCmnString& vendorInfo,
					 USHORT majorVersion,
					 USHORT minorVersion,
					 USHORT patchVersion,
					 ULONG buildNumber);
	BOOL removeIdentity(const SOCmnString& url);
	BOOL updateIdenty(const SOCmnString& url, USHORT status);

	static USHORT getDefaultTunnelDiscoveryPort(void)
	{
		return TP_OCT_TUNNEL_DISCOVERY_PORT;
	};
	static USHORT getDefaultTunnelDiscoveryTimeout(void)
	{
		return TP_OCT_TUNNEL_DISCOVERY_TIMEOUT;
	};

	virtual BOOL start(void);
	virtual BOOL stop(void);

protected:
	static DWORD _stdcall listenThreadProc(void* context);

	SOCmnThread* getListenThread(void)
	{
		return m_listenThread;
	}
	DWORD getListenEvent(void)
	{
		return m_listenEvent;
	};
	DWORD getUnicastEvent(void)
	{
		return m_unicastEvent;
	};
	DWORD getBroadcastEvent(void)
	{
		return m_broadcastEvent;
	};

	SOCmnUDPSocket* getUdpCommunicationSender(void)
	{
		m_udpCommunicationSender.addRef();
		return m_udpCommunicationSender;
	};
	SOCmnUDPSocket* getUdpCommunicationReceiver(void)
	{
		m_udpCommunicationReceiver.addRef();
		return m_udpCommunicationReceiver;
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
	BOOL marshalResponse(DeviceIdentity* pDevice, UCHAR** message, ULONG& size, USHORT deviceType);

	BOOL setIpAddressOnDevices(const SOCmnString& anIpAddress);

	virtual BOOL handleRequest(UCHAR* pRequest, ULONG pDataSize);

	virtual BOOL sendUnicastResponse(USHORT deviceType);
	virtual BOOL sendBroadcastResponse(USHORT deviceType);
	virtual BOOL sendResponse(USHORT deviceType, bool sendBroadcast);

protected:
	SOCmnThread* m_listenThread;
	DWORD m_listenEvent;
	DWORD m_broadcastEvent;
	DWORD m_unicastEvent;

private:
	SOCmnPointer<SOCmnUDPSocket> m_udpCommunicationSender;
	SOCmnPointer<SOCmnUDPSocket> m_udpCommunicationReceiver;
	SOCmnPointer<SOCmnTP_Marshaler> m_marshaler;
	SOCmnPointer<SOCmnTP_UnMarshaler> m_unMarshaler;
	SOCmnList<DeviceIdentity> m_deviceIdentityList;
}; // end SOCltTPDiscovery

#pragma pack(pop)

#endif // SOFEATURE_TP_DISCOVERY
#endif // SOFEATURE_TUNNEL
#endif // _SOCLTTPDISCOVERY_H_
