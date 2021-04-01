#ifndef _SOAESCOMAREABROWSER_H_
#define _SOAESCOMAREABROWSER_H_

#ifdef SOFEATURE_DCOM

#include <opc_ae.h>
#include "SOSrvComObject.h"

//-----------------------------------------------------------------------------
// SOAeSComAreaBrowser                                                        |
//-----------------------------------------------------------------------------

class SOAeSAreaBrowser;
class SOAeSComServer;
class SOAeSServer;

class SOAeSComAreaBrowser :
	public IOPCEventAreaBrowser,
	public CComObjectRoot,
	public SOSrvComBase
{
	friend SOAeSServer;
public:
	SOAeSComAreaBrowser();
	~SOAeSComAreaBrowser();

	BEGIN_COM_MAP(SOAeSComAreaBrowser)
	COM_INTERFACE_ENTRY(IOPCEventAreaBrowser)
	END_COM_MAP()

	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComAreaBrowser)

	// IOPCEventAreaBrowser interface
	STDMETHODIMP ChangeBrowsePosition(IN OPCAEBROWSEDIRECTION browseDirection,
									  IN LPCWSTR string);

	STDMETHODIMP BrowseOPCAreas(IN OPCAEBROWSETYPE browseFilterType,
								IN LPCWSTR filterCriteria, OUT LPENUMSTRING* enumString);

	STDMETHODIMP GetQualifiedAreaName(IN LPCWSTR areaName,
									  OUT LPWSTR* qualifiedAreaName);

	STDMETHODIMP GetQualifiedSourceName(IN LPCWSTR sourceName,
										OUT LPWSTR* qualifiedSourceName);

	// initialize area browser object
	void init(IN SOAeSComServer* server, IN SOAeSAreaBrowser* areaBrowser);

	// object trace id
	LPCTSTR getObjTraceId(void);

protected:
	SOCmnString m_objectTraceId;    // object trace id string
	SOAeSAreaBrowser* m_me;         // area browser object
	SOAeSComServer* m_server;       // server object

	// query interface
	virtual HRESULT queryInterface(REFIID iid, void** object);

	// connection from area browser object
	virtual void lockMe(void);
	virtual void unlockMe(void);
	virtual void disuniteMe(void);

	void releaseComServer(void);
}; // SOAeSComAreaBrowser

#endif // SOFEATURE_DCOM

#endif
