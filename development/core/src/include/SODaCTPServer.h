#ifndef _SODACTPSERVER_H_
#define _SODACTPSERVER_H_

#ifdef SOFEATURE_TUNNEL

#include "SODaCServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCTPServerImpl                                                                |
//-----------------------------------------------------------------------------

class SODaCServer;

class SODAC_EXPORT SODaCTPServerImpl : virtual public SODaCServerICommunication
{
	friend class SODaCServer;
public:
	SODaCTPServerImpl();

	virtual HRESULT getStatus(OUT SODaCServerStatus& status);
	virtual HRESULT queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propertyList);
	virtual HRESULT getPropertiesData(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propteryList);

	virtual HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, OUT SOCmnVariant* pValues,
						 OUT WORD* pQualities = NULL, OUT SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	virtual HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues,
						  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	virtual WORD getSupportedOPCVersion(void);
	virtual BOOL forceOPCVersion(WORD opcVersion);

	virtual void connectionFailureImpl(void);

	void setIPPort(WORD port)
	{
		m_ipPort = port;
	}
	WORD getIPPort(void)
	{
		return m_ipPort;
	}

	virtual SOCltTPConnection* getTPConnection(void)
	{
		m_tpCon.addRef();
		return m_tpCon;
	}

	virtual BOOL logon(IN LPCTSTR userName, IN LPCTSTR password);
	virtual BOOL logoff(void);

protected:
	SOCmnPointer<SOCltTPConnection> m_tpCon;
	WORD m_ipPort;

	virtual ~SODaCTPServerImpl();

	// do the state transitions
	virtual HRESULT doDisconnectImpl(BOOL connectionMonitorStatus);
	virtual HRESULT doStateTransitionImpl(void);

	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);
	virtual SODaCNameSpaceBrowser* getNameSpaceBrowserImpl(void);
}; // SODaCTPServerImpl




//-----------------------------------------------------------------------------
// SODaCTPServerDefault                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPServerDefault :
	public SODaCServer,
	virtual public SODaCTPServerImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCTPServerDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif
