#ifndef _SOAESAREABROWSER_H_
#define _SOAESAREABROWSER_H_

#pragma pack(push,4)


//-----------------------------------------------------------------------------
// SOAeSAreaBrowser                                                           |
//-----------------------------------------------------------------------------

class SOAeSComAreaBrowser;
class SOAeSServer;

#ifdef SOFEATURE_TUNNEL
class SOAeSTPAreaBrowser;
#endif


class SOAES_EXPORT SOAeSAreaBrowser :
	public SOCmnElement,
	public SOSrvBrowser
{
	friend SOAeSComAreaBrowser;
	friend SOAeSServer;
	friend SOAeSEntry;

#ifdef SOFEATURE_TUNNEL
	friend SOAeSTPAreaBrowser;
#endif

public:
	SOAeSAreaBrowser(void);

	// COM object handling
	void* getMe(void);
	ULONG releaseMe(IN void* me);

#ifdef SOFEATURE_DCOM
	// check interface access
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#endif

protected:
	void* m_me;         // COM area browser object
	SOCmnSync m_meSync; // synchronisation of COM area browser object setting

	virtual ~SOAeSAreaBrowser(void);

	// get root of browse object space
	virtual SOCmnElement* getBrowserRoot(void);

	// string based overwirtables
	virtual BOOL isLeafName(IN LPCTSTR fullName);
	virtual BOOL isBranchName(IN LPCTSTR fullName);
	virtual HRESULT getChildNames(IN enum browserFilterType browseType,
								  IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
								  IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList);
	virtual BOOL browseOnlyStringBased(void);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// tracing object
	virtual SOCmnObject* getObjectSrvBrowser(void);

	// query interface
	virtual HRESULT queryInterface(REFIID iid, void** object);

#ifdef SOFEATURE_COMIF_HOOKS
	// interface function callbacks
	virtual HRESULT onBChangeBrowsePosition(IN OPCAEBROWSEDIRECTION browseDirection, IN LPCWSTR string);
	virtual HRESULT onBBrowseOPCAreas(IN OPCAEBROWSETYPE browseFilterType, IN LPCWSTR filterCriteria, OUT LPENUMSTRING* enumString);
	virtual HRESULT onBGetQualifiedAreaName(IN LPCWSTR areaName, OUT LPWSTR* qualifiedAreaName);
	virtual HRESULT onBGetQualifiedSourceName(IN LPCWSTR sourceName, OUT LPWSTR* qualifiedSourceName);
	virtual void onEChangeBrowsePosition(IN HRESULT res, IN OPCAEBROWSEDIRECTION browseDirection, IN LPCWSTR string);
	virtual void onEBrowseOPCAreas(IN HRESULT res, IN OPCAEBROWSETYPE browseFilterType, IN LPCWSTR filterCriteria, IN LPENUMSTRING* enumString);
	virtual void onEGetQualifiedAreaName(IN HRESULT res, IN LPCWSTR areaName, IN LPWSTR* qualifiedAreaName);
	virtual void onEGetQualifiedSourceName(IN HRESULT res, IN LPCWSTR sourceName, IN LPWSTR* qualifiedSourceName);
#endif // SOFEATURE_COMIF_HOOKS
}; // SOAeSAreaBrowser


#pragma pack(pop)
#endif
