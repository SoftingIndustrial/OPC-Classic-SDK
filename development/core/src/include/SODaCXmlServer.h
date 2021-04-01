#ifndef _SODACXMLSERVER_H_
#define _SODACXMLSERVER_H_

#ifdef SOFEATURE_SOAP

#include "SODaCServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCXmlServerImpl                                                                |
//-----------------------------------------------------------------------------

class SODaCServer;
class SODaCXmlRequestOptions;

class SODAC_EXPORT SODaCXmlServerImpl : virtual public SODaCServerICommunication
{
	friend class SODaCServer;
public:
	SODaCXmlServerImpl();

	virtual HRESULT getStatus(OUT SODaCServerStatus& status);
	virtual HRESULT queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propertyList);
	virtual HRESULT getPropertiesData(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propteryList);

	virtual HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, OUT SOCmnVariant* pValues,
						 OUT WORD* pQualities = NULL, OUT SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	virtual HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues,
						  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	virtual WORD getSupportedOPCVersion(void);
	virtual BOOL forceOPCVersion(WORD opcVersion);

	virtual BOOL setTimeout(float timeoutPercent, DWORD timeoutMin, DWORD timeoutMax);
	virtual float getTimeoutPercent(void);
	virtual DWORD getTimeoutMin(void);
	virtual DWORD getTimeoutMax(void);

	virtual void setIPPort(WORD port)
	{
		m_httpCon->setPort(port);
	}
	virtual WORD getIPPort(void)
	{
		return m_httpCon->getPort();
	}
	virtual void setHTTPPath(LPCTSTR path)
	{
		m_httpCon->setDefaultURL(path);
	}
	virtual LPCTSTR getHTTPPath(void)
	{
		return m_httpCon->getDefaultURL();
	}
	virtual SOCltHTTPConnection* getHTTPConnection(void)
	{
		m_httpCon.addRef();
		return m_httpCon;
	}

	virtual void connectionFailureImpl(void);

	static DWORD getServerStateFromString(IN LPCTSTR stateSZ);

	virtual SODaCXmlRequestOptions* fillRequestOptions(SODaCXmlRequestOptions* pReqOptions);

	virtual void setTimeDifference(LONGLONG timeDiffSend, LONGLONG timeDiffReply);
	virtual LONGLONG getTimeDifference(void);

	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

protected:
	SOCmnPointer<SOCltHTTPConnection> m_httpCon;
	LONGLONG m_timeDifference;
	float m_timeoutPercent;
	DWORD m_timeoutMin;
	DWORD m_timeoutMax;

	virtual ~SODaCXmlServerImpl();

	// do the state transitions
	virtual HRESULT doStateTransitionImpl(void);
	virtual SODaCNameSpaceBrowser* getNameSpaceBrowserImpl(void);

	virtual HRESULT doDisconnectImpl(BOOL connectionMonitorStatus);
}; // SODaCXmlServerImpl




//-----------------------------------------------------------------------------
// SODaCXmlServerDefault                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlServerDefault :
	public SODaCServer,
	virtual public SODaCXmlServerImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCXmlServerDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif
