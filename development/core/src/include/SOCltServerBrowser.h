#ifndef _SOCLTSERVERBROWSER_H_
#define _SOCLTSERVERBROWSER_H_

#ifdef SOFEATURE_DCOM

#include "SOCltElement.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCltServerBrowser                                                         |
//-----------------------------------------------------------------------------

// server browsing whatServers
#define SOCLT_BROWSE_SERVER_REG_DA10A   0x01
#define SOCLT_BROWSE_SERVER_CAT_DA10A   0x02
#define SOCLT_BROWSE_SERVER_CAT_DA20    0x04
#define SOCLT_BROWSE_SERVER_CAT_AE10    0x08
#define SOCLT_BROWSE_SERVER_CAT_DA30    0x10

#define SOCLT_BROWSE_SERVER_CAT_ALL ( SOCLT_BROWSE_SERVER_CAT_DA10A | SOCLT_BROWSE_SERVER_CAT_DA20 | SOCLT_BROWSE_SERVER_CAT_AE10 | SOCLT_BROWSE_SERVER_CAT_DA30)
#define SOCLT_BROWSE_SERVER_DA10A       ( SOCLT_BROWSE_SERVER_REG_DA10A | SOCLT_BROWSE_SERVER_CAT_DA10A )
#define SOCLT_BROWSE_SERVER_DA20        ( SOCLT_BROWSE_SERVER_CAT_DA20 )
#define SOCLT_BROWSE_SERVER_DA30        ( SOCLT_BROWSE_SERVER_CAT_DA30 )
#define SOCLT_BROWSE_SERVER_AE10        ( SOCLT_BROWSE_SERVER_CAT_AE10 )
#define SOCLT_BROWSE_SERVER_ALL       ( SOCLT_BROWSE_SERVER_CAT_ALL | SOCLT_BROWSE_SERVER_REG_DA10A )

class SOCLT_EXPORT SOCltServerBrowser :
	public SOCmnObject,
	virtual public SOCltElementErrorStore
{
public:
	SOCltServerBrowser(void);

	void setNodeName(LPCTSTR nodeName); // set node name of server browser object
	SOCmnString getNodeName(void);          // get node name of server brower object

	// server browsing
	BOOL browseServer(IN DWORD whatServers, OUT SOCmnList<CLSID> &classIdArray, OUT SOCmnDWORDList* pWhatServersList = NULL);

	// details of server
	BOOL getServerDescription(IN REFCLSID clsid, OUT SOCmnString& descr);
	BOOL getServerProgId(IN REFCLSID clsid, OUT SOCmnString& progId);
	BOOL getClassDetail(IN REFCLSID clsid, OUT SOCmnString& descr, OUT SOCmnString& progId,
						OUT SOCmnString& vProgId);

	BOOL getCLSIDFromProgID(IN LPCTSTR progID, OUT CLSID* pClsid);

protected:

	virtual ~SOCltServerBrowser(void);

	CComPtr<IOPCServerList> m_serverBrowser;   // IOPCServerList interface of the browse node
	CComPtr<IOPCServerList2> m_serverBrowser2; // IOPCServerList2 interface of the browse node
	SOCmnString m_serverBrowserNode;                     // node name of server brower object

	BOOL createServerBrowser();             // creates server browser object
	void releaseServerBrowser(void);    // release server browser object

	BOOL includesCLSID(IN REFCLSID clsid, IN SOCmnList<CLSID> *classIdList, IN DWORD opcSpec, IN SOCmnDWORDList* whatServersList);

	virtual SOCmnObject* getObjectCltElementIError(void);
}; // SOCltServerBrowser

inline void SOCltServerBrowser::releaseServerBrowser(void)
{
	m_serverBrowser.Release();
}

inline SOCmnString SOCltServerBrowser::getNodeName(void)
{
	return m_serverBrowserNode;
}

inline void SOCltServerBrowser::setNodeName(LPCTSTR nodeName)
{
	m_serverBrowserNode = nodeName;
	releaseServerBrowser();
}


#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif
