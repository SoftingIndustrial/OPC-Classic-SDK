#ifndef _SOAECCOMBROWSE_H_
#define _SOAECCOMBROWSE_H_

#pragma pack(push,4)


//-----------------------------------------------------------------------------
// SOAeCComAreaBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCComAreaBrowserImpl
	: virtual public SOAeCAreaBrowserICommunication
{
public:
	SOAeCComAreaBrowserImpl(SOAeCServer* pServer);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pParent, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPathImpl(IN SOCltBrowseObject* pParent);

	virtual HRESULT queryInterface(IN REFIID iid, OUT void** interf);

protected:
	virtual ~SOAeCComAreaBrowserImpl();

	CComPtr<IOPCEventAreaBrowser> m_eventAreaBrowser;
	CComPtr<IOPCEventServer> m_eventServer;

};


class SODAC_EXPORT SOAeCComAreaBrowser :
	public SOAeCAreaBrowser,
	virtual public SOAeCComAreaBrowserImpl
{
public:
	SOAeCComAreaBrowser(SOAeCServer* pServer);
};



//-----------------------------------------------------------------------------
// SOAeCComEventBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCComEventBrowserImpl
	: virtual public SOAeCEventBrowserICommunication
{
public:
	SOAeCComEventBrowserImpl(SOAeCServer* pServer);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pParent, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

	virtual HRESULT queryInterface(IN REFIID iid, OUT void** interf);

protected:
	virtual ~SOAeCComEventBrowserImpl();

	CComPtr<IOPCEventServer> m_eventServer;
};


class SODAC_EXPORT SOAeCComEventBrowser :
	public SOAeCEventBrowser,
	virtual public SOAeCComEventBrowserImpl
{
public:
	SOAeCComEventBrowser(SOAeCServer* pServer);
};


#pragma pack(pop)

#endif // _SOAECBROWSE_H_
