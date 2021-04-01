#ifndef _SOAECBROWSE_H_
#define _SOAECBROWSE_H_

#pragma pack(push, 4)



class SOAeCAreaBrowser;

class SOAEC_EXPORT SOAeCAreaBrowserICommunication
{
	friend class SOAeCAreaBrowser;
public:
	SOAeCAreaBrowserICommunication(void);
	virtual ~SOAeCAreaBrowserICommunication(void);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pParent, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPathImpl(IN SOCltBrowseObject* pParent);

protected:
	virtual SOAeCAreaBrowser* getObjectSOAeCAreaBrowser(void);
};

class SOAeCServer;

//-----------------------------------------------------------------------------
// SOAeCAreaBrowser                                                           |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCAreaBrowser :
	public SOCltBrowseObject,
	virtual public SOCmnElementListLeafBranch,
	virtual public SOAeCAreaBrowserICommunication
{
	friend class SOAeCServer;   // creates the instance
public:
	SOAeCAreaBrowser(SOAeCServer* pServer);

	virtual SOCmnString getDisplayName();
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

	SOAeCServer* getServer(void)
	{
		m_server.addRef();
		return m_server;
	};
	virtual SOCltBrowseObject* getBrowser(void)
	{
		addRef();
		return this;
	};

	LPCTSTR getElementNameFilter(void) const
	{
		return m_filterCriteria;
	}
	BOOL setFilter(LPCTSTR elementNameFilter)
	{
		m_filterCriteria = elementNameFilter;
		return TRUE;
	}

	// tree hierarchy level delimiter and not allowed chars
	void setTreeDelimiter(IN TCHAR create, IN LPCTSTR accept,
						  IN LPCTSTR bad)
	{
		m_delimNScreate = create;
		m_delimNSaccept = accept;
		m_badNSchars = bad;
	}
	virtual TCHAR getTreeDelimiter(OUT LPCTSTR* accept, OUT LPCTSTR* bad);

protected:
	SOCmnPointer<SOAeCServer> m_server;
	SOCmnString m_filterCriteria;

	// tree delimiters
	TCHAR m_delimNScreate;              // creation tree delimiter
	SOCmnString m_delimNSaccept;    // accepted tree delimiter
	SOCmnString m_badNSchars;           // bad tree characters

	virtual ~SOAeCAreaBrowser();

	virtual SOAeCAreaBrowser* getObjectSOAeCAreaBrowser(void);
};



//-----------------------------------------------------------------------------
// SOAeCArea                                                                  |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCArea :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCArea(SOAeCAreaBrowser* parent, LPCWSTR wszName);
	SOAeCArea(SOAeCArea* parent, LPCWSTR wszName);
	SOAeCArea(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCArea();

// Overrides
	// SOCMNOBJECT_TYPE_AREA or SOCMNOBJECT_TYPE_SOURCE
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPath();

	SOAeCAreaBrowser* getAreaSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getAreaSpaceBrowser();
	};

	SOCmnStringList* getPathList(void)
	{
		return &m_path;
	}

protected:
	SOCmnPointer<SOAeCAreaBrowser> m_browser;
	SOCmnStringList m_path;
};




//-----------------------------------------------------------------------------
// SOAeCSource                                                                |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCSource :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:

	SOAeCSource(SOAeCAreaBrowser* parent, LPCWSTR wszName);
	SOAeCSource(SOAeCArea* parent, LPCWSTR wszName);
	SOAeCSource(IN SOCltBrowseObject* parent, LPCTSTR name);

	virtual ~SOAeCSource();

// Overrides
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPath();

	SOAeCAreaBrowser* getAreaSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getAreaSpaceBrowser();
	};

	SOCmnStringList* getPathList(void)
	{
		return &m_path;
	}

	void setPath(LPCTSTR pt)
	{
		m_fullPath = pt;
	}

protected:
	SOCmnPointer<SOAeCAreaBrowser> m_browser;
	SOCmnStringList m_path;
	SOCmnString m_fullPath;
};




//-----------------------------------------------------------------------------
// SOAeCCondition                                                             |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCCondition :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCCondition(SOAeCSource* parent, LPCWSTR wszName);
	SOAeCCondition(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCCondition();

// Overrides
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	SOAeCAreaBrowser* getAreaSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getAreaSpaceBrowser();
	};

protected:
	SOCmnPointer<SOAeCAreaBrowser> m_browser;
};



class SOAeCEventBrowser;

class SOAEC_EXPORT SOAeCEventBrowserICommunication
{
	friend class SOAeCEventBrowser;
public:
	SOAeCEventBrowserICommunication(void);
	virtual ~SOAeCEventBrowserICommunication(void);

	virtual BOOL expandImpl(IN SOCltBrowseObject* pParent, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

protected:
	virtual SOAeCEventBrowser* getObjectSOAeCEventBrowser(void);
};

//-----------------------------------------------------------------------------
// SOAeCEventBrowser                                                          |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCEventBrowser :
	public SOCltBrowseObject,
	virtual public SOAeCEventBrowserICommunication
{

public:
	SOAeCEventBrowser(SOAeCServer* pServer);

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getDisplayName();

	SOAeCServer* getServer(void)
	{
		m_server.addRef();
		return m_server;
	};
	virtual SOCltBrowseObject* getBrowser(void)
	{
		addRef();
		return this;
	};

	BOOL setFilter(DWORD filterCategories)
	{
		m_filterCategories = filterCategories;
		return TRUE;
	}
	DWORD getFilter(void)
	{
		return m_filterCategories;
	}

	friend class SOAeCServer;   // creates the instance

protected:
	virtual ~SOAeCEventBrowser();

	SOCmnPointer<SOAeCServer> m_server;
	DWORD m_filterCategories;   // any combination of OPC_*_EVENT flags

	virtual SOAeCEventBrowser* getObjectSOAeCEventBrowser(void);
};




//-----------------------------------------------------------------------------
// SOAeCCategory                                                              |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCCategory :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCCategory(SOAeCEventBrowser* parent, LPCWSTR szName);
	SOAeCCategory(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCCategory();

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

	DWORD getCategoryID(void)
	{
		return m_id;
	}
	DWORD getEventType(void)
	{
		return m_eventType;
	}

	DWORD m_id;                                 // Category ID
	DWORD m_eventType;

	SOAeCEventBrowser* getEventSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getEventSpaceBrowser();
	};

protected:
	SOCmnPointer<SOAeCEventBrowser> m_browser;
};




//-----------------------------------------------------------------------------
// SOAeCAttribute                                                             |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCAttribute :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCAttribute(SOAeCCategory* parent, LPCWSTR wszName);
	SOAeCAttribute(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCAttribute();

	virtual BOOL expand(IN DWORD Attribute, OUT SOCmnList<SOCltBrowseObject> &objList);

	SOAeCEventBrowser* getEventSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getEventSpaceBrowser();
	};

	DWORD getAttributeID(void)
	{
		return m_id;
	}
	VARTYPE getDatatype(void)
	{
		return m_type;
	}

	DWORD m_id;                                 // Attribute ID
	VARTYPE m_type;                         // Attribute type

protected:
	SOCmnPointer<SOAeCEventBrowser> m_browser;
};




//-----------------------------------------------------------------------------
// SOAeCConditionClass                                                        |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCConditionClass :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCConditionClass(SOAeCCategory* parent, LPCWSTR wszName);
	SOAeCConditionClass(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCConditionClass();

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

	SOAeCEventBrowser* getEventSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getEventSpaceBrowser();
	};

protected:
	SOCmnPointer<SOAeCEventBrowser> m_browser;
};




//-----------------------------------------------------------------------------
// SOAeCSubConditionClass                                                     |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCSubConditionClass :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:
	SOAeCSubConditionClass(SOAeCConditionClass* parent, LPCWSTR szName);
	SOAeCSubConditionClass(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual ~SOAeCSubConditionClass();

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);

	SOAeCEventBrowser* getEventSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getEventSpaceBrowser();
	};

protected:
	SOCmnPointer<SOAeCEventBrowser> m_browser;
};


#pragma pack(pop)

#endif // _SOAECBROWSE_H_
