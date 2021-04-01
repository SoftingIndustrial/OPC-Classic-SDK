#ifndef _SOCLTSERVER_H_
#define _SOCLTSERVER_H_

#include "SOCltElement.h"
#include "SOCltThread.h"

#pragma pack(push, 4)


class SOCltServer;

class SOCLT_EXPORT SOCltServerICommunication
{
	friend class SOCltServer;
public:
	SOCltServerICommunication(void);
	virtual ~SOCltServerICommunication(void);

	virtual void initClt(void);
	virtual void terminate(void);

#ifdef SOFEATURE_DCOM
	// DCOM specific methods
	virtual HRESULT getErrorString(IN HRESULT error, OUT SOCmnString* string);
	virtual BOOL setClassID(IN REFCLSID clsid);
	virtual CLSID getClassID(void) const;
	virtual BOOL setClsctx(IN DWORD clsctx);
	virtual DWORD getClsctx(void) const;
#endif

protected:
	virtual SOCltServer* getObjectCltServer(void);
};

// server update parameters

#define SOCLT_SRVPAR_CLIENT_NAME          0x00000001
#define SOCLT_SRVPAR_LCID                 0x00000002
#define SOCLT_SRVPAR_ALL                  0xFFFFFFFF

//-----------------------------------------------------------------------------
// SOCltServer                                                                |
//-----------------------------------------------------------------------------

class SOCltShutdown;
#ifdef SOFEATURE_DCOM
class SOCltComServerImpl;
#endif

class SOCLT_EXPORT SOCltServer :
	public SOCltElement,
	virtual public SOCltServerICommunication
{
#ifdef SOFEATURE_DCOM
	friend class SOCltComServerImpl;
#endif

public:
	SOCltServer(IN OPTIONAL DWORD objType = 0);

	// the server requests to shutdown the connection
	virtual void shutdown(IN LPCTSTR reason);

	// name of a remote maschine
	virtual BOOL setNodeName(IN LPCTSTR nodeName);
	virtual SOCmnString getNodeName(void) const;

	// Flags for valid parameter
	virtual DWORD getValidParameters(void) const;
	virtual void resetValidParameters(void);

	// server name
	virtual BOOL setServerName(IN LPCTSTR serverName);
	virtual SOCmnString getServerName(void) const;

	// local Id
	virtual BOOL setLCID(IN DWORD lcid);
	virtual DWORD getLCID(void) const;

	// request timeout
	virtual BOOL setRequestTimeout(IN DWORD requestTimeout);
	virtual DWORD getRequestTimeout(void) const;

	// client name
	virtual BOOL setClientName(IN LPCTSTR serverName);
	virtual SOCmnString getClientName(void) const;

	// perform initial read
	virtual BOOL setPerformInitialRead(IN BYTE performInitialRead);
	virtual BYTE getPerformInitialRead(void) const;

	// URL
	virtual BOOL setURL(IN LPCTSTR url)
	{
		m_url = url;
		return TRUE;
	}
	virtual LPCTSTR getURL(void) const
	{
		return m_url;
	}

	// overrides
	virtual BOOL setParentBranch(IN SOCmnElement* parent);
	virtual SOCltWorkerThread* getWorkerThread(void);

	// connection monitor
	virtual BOOL addConnectionMonitor(DWORD checkCycle, DWORD connect1Attempts, DWORD connect1Cycle, DWORD connect2Cycle);
	virtual BOOL removeConnectionMonitor(void);

	// security
	virtual BOOL setUserName(IN LPCTSTR userName);
	virtual SOCmnString getUserName(void) const;
	virtual BOOL setPassword(IN LPCTSTR password);
	virtual SOCmnString getPassword(void) const;

	virtual void setProtocol(BYTE p)
	{
		m_protocol = p;
	}
	virtual BYTE getProtocol(void) const
	{
		return m_protocol;
	}

#ifdef SOFEATURE_SOAP
	virtual BOOL convertLCIDtoLocaleID(LCID lcid, SOCmnString& localeId);
	virtual BOOL convertLocaleIDtoLCID(LPCTSTR localeId, LCID& lcid);
#endif // SOFEATURE_SOAP

protected:
	SOCmnString m_url;              // the servers URL
	BYTE m_protocol;                // protocol of the server (SOCLT_PROTOCOL_xxx)

	SOCmnString m_nodeName;         // computer name the OPC Server object should run on
	SOCmnString m_clientName;       // client name in the OPC Server
	SOCmnString m_serverName;       // logical server name of the proxy
	LCID m_lcid;                    // default locale id
	DWORD m_requestTimeout;         // request timeout
	DWORD m_validParameter;         // valid parameters
	SOCmnString m_userName;         // user name for logon to the OPC Server
	SOCmnString m_password;         // password for logon to the OPC Server

	SOCltWorkerThread m_workThread;
	DWORD m_cookieConnectMonitor;
	BYTE m_performInitialRead;

protected:
	virtual ~SOCltServer(void);

	// callbacks
	virtual void onSetLCID(void);
	virtual void onSetClientName(void);
	virtual void onSetRequestTimeout(void);
	virtual void onSetPerformInitialRead(void);
	virtual void onSetServerName(void);

	// shutdown callback
	virtual void onShutdown(void);

	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCltServer* getObjectCltServer(void);
}; // SOCltServer


inline BOOL SOCltServer::setNodeName(IN LPCTSTR nodeName)
{
	m_nodeName = nodeName;
	return TRUE;
}

inline SOCmnString SOCltServer::getNodeName(void) const
{
	return m_nodeName;
}

inline DWORD SOCltServer::getLCID(void) const
{
	return m_lcid;
}

inline DWORD SOCltServer::getRequestTimeout(void) const
{
	return m_requestTimeout;
}

inline SOCmnString SOCltServer::getClientName(void) const
{
	return m_clientName;
}

inline BYTE SOCltServer::getPerformInitialRead(void) const
{
	return m_performInitialRead;
}

inline SOCmnString SOCltServer::getServerName(void) const
{
	return m_serverName;
}

inline DWORD SOCltServer::getValidParameters(void) const
{
	return m_validParameter;
}

inline void SOCltServer::resetValidParameters(void)
{
	m_validParameter = 0;
}

inline SOCmnString SOCltServer::getUserName(void) const
{
	return m_userName;
}

inline SOCmnString SOCltServer::getPassword(void) const
{
	return m_password;
}

#pragma pack(pop)
#endif
