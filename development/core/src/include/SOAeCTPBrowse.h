#ifndef _SOAECTPBROWSE_H_
#define _SOAECTPBROWSE_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOAeCTPAreaBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPAreaBrowserImpl
	: virtual public SOAeCAreaBrowserICommunication
{
public:
	SOAeCTPAreaBrowserImpl(SOAeCServer* pServer);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPathImpl(IN SOCltBrowseObject* pParent);

protected:
	DWORD m_serverHandle;
	SOCmnPointer<SOAeCServer> m_server;

	virtual ~SOAeCTPAreaBrowserImpl();
};


class SODAC_EXPORT SOAeCTPAreaBrowser :
	public SOAeCAreaBrowser,
	virtual public SOAeCTPAreaBrowserImpl
{
public:
	SOAeCTPAreaBrowser(SOAeCServer* pServer);
};


//-----------------------------------------------------------------------------
// SOAeCTPEventBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPEventBrowserImpl
	: virtual public SOAeCEventBrowserICommunication
{
public:
	SOAeCTPEventBrowserImpl(SOAeCServer* pServer);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

protected:
	virtual ~SOAeCTPEventBrowserImpl();
};


class SODAC_EXPORT SOAeCTPEventBrowser :
	public SOAeCEventBrowser,
	virtual public SOAeCTPEventBrowserImpl
{
public:
	SOAeCTPEventBrowser(SOAeCServer* pServer);
};

#pragma pack(pop)

#endif //   SOFEATURE_TUNNEL

#endif // _SOAECBROWSE_H_
