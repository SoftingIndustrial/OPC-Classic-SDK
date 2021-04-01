#ifndef _SODACBROWSE_H_
#define _SODACBROWSE_H_

#pragma pack(push, 4)


class SODaCNameSpaceBrowser;

class SODAC_EXPORT SODaCNameSpaceBrowserICommunication
{
	friend class SODaCNameSpaceBrowser;
public:
	SODaCNameSpaceBrowserICommunication(void);
	virtual ~SODaCNameSpaceBrowserICommunication(void);

	virtual BOOL expand20(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual BOOL expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR itemPath, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual LPCTSTR retrieveItemID(SOCltBrowseObject* pObj);

	virtual void initUse(void);
	virtual void use20(void);
	virtual void use30(void);
	virtual BOOL support20(void);
	virtual BOOL support30(void);

protected:
	virtual SODaCNameSpaceBrowser* getObjectSODaCNameSpaceBrowser(void);
};


//-----------------------------------------------------------------------------
// SODaCNameSpaceBrowser                                                      |
//-----------------------------------------------------------------------------

#define SODACNAMESPACEBROWSER_STATE_NO_BROWSE_INTERFACE              0x80
#define SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE 0x01
#define SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES 0x02
#define SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE                   0x04


class SODAC_EXPORT SODaCNameSpaceBrowser
	: public SOCltBrowseObject,
	  virtual public SODaCNameSpaceBrowserICommunication,
	  virtual public SOCmnElementListLeafBranch
{
	friend class SODaCServer;   // creates the instance

public:
	SODaCNameSpaceBrowser(SODaCServer* pServer);

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getDisplayName();

	SODaCServer* getServer(void)
	{
		m_server.addRef();
		return (SODaCServer*)m_server;
	};

	BOOL setFilter(LPCTSTR elementNameFilter, DWORD accessRightsFilter, VARTYPE datatypeFilter);
	BOOL setFilter(LPCTSTR elementNameFilter, LPCTSTR vendorFilter);
	void resetFilter(void);

	void setMaxElements(DWORD maxEl)
	{
		m_maxElements = maxEl;
	}
	DWORD getMaxElements(void)
	{
		return m_maxElements;
	}
	void setRetrievePropertyValues(BOOL doRetrieve)
	{
		m_retrievePropValues = (BYTE)doRetrieve;
	}
	BOOL getRetrievePropertyValues(void)
	{
		return m_retrievePropValues;
	}
	void setReturnProperties(BOOL retProps)
	{
		m_returnProps = (BYTE)retProps;
	}
	BOOL getReturnProperties(void)
	{
		return m_returnProps;
	}
	void setRetrieveItemID(BOOL doRetrieve)
	{
		m_retrieveItemID = (BYTE)doRetrieve;
	}
	BOOL getRetrieveItemID(void)
	{
		return m_retrieveItemID;
	}

	LPCTSTR getVendorFilter(void) const
	{
		return m_vendorFilter;
	}
	LPCTSTR getElementNameFilter(void) const
	{
		return m_elementNameFilter;
	}
	LPCTSTR getContinuationPoint() const
	{
		return m_continuationPoint;
	}
	void setContinuationPoint(LPCTSTR contPoint)
	{
		m_continuationPoint = contPoint;
	}
	VARTYPE getDatatypeFilter(void) const
	{
		return m_datatypeFilter;
	}
	DWORD getAccessRightsFilter(void) const
	{
		return m_accessRightsFilter;
	}
	DWORD getMaxElements(void) const
	{
		return m_maxElements;
	}

	void setForceBrowseUp(BOOL forceBrowseUp)
	{
		m_forceBrowseUp = (BYTE)forceBrowseUp;
	}
	BOOL getForceBrowseUp(void)
	{
		return m_forceBrowseUp;
	}

	virtual SOCltBrowseObject* getBrowser(void)
	{
		addRef();
		return this;
	};

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
	SOCmnPointer<SODaCServer> m_server;
	SOCmnString m_vendorFilter;
	SOCmnString m_elementNameFilter;
	SOCmnString m_continuationPoint;
	VARTYPE m_datatypeFilter;                   // VT_* type
	DWORD m_accessRightsFilter;                 // any combination of OPC_READABLE and OPC_WRITEABLE
	DWORD m_maxElements;
	BYTE m_retrievePropValues;
	BYTE m_returnProps;
	BYTE m_retrieveItemID;
	// tree delimiters
	TCHAR m_delimNScreate;              // creation tree delimiter
	SOCmnString m_delimNSaccept;    // accepted tree delimiter
	SOCmnString m_badNSchars;           // bad tree characters
	BYTE m_forceBrowseUp;
	virtual ~SODaCNameSpaceBrowser();

	virtual SODaCNameSpaceBrowser* getObjectSODaCNameSpaceBrowser(void);
};




//-----------------------------------------------------------------------------
// SODaCNode                                                                  |
//-----------------------------------------------------------------------------

class SODaCProperty;

class SODAC_EXPORT SODaCNode :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
	friend class SODaCNameSpaceBrowser;
public:
	SODaCNode(IN SODaCNameSpaceBrowser* parent, LPCTSTR wszName);
	SODaCNode(IN SODaCNode* parent, LPCTSTR wszName);
	SODaCNode(IN SOCltBrowseObject* parent, LPCTSTR wszName);

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPath();
	virtual SOCmnString getItemPath(void)
	{
		return m_itemPath;
	}
	void setItemPath(IN LPCTSTR itemPath)
	{
		m_itemPath = itemPath;
	}

	SODaCNameSpaceBrowser* getNameSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getNameSpaceBrowser();
	};

	virtual SOCmnStringList* getPathList(void)
	{
		return &m_path;
	}

	virtual void setItemID(IN LPCTSTR itemID, IN LPCTSTR /* parentItemID */)
	{
		m_itemID = itemID;
	}
	virtual SOCmnString getItemID(IN OPTIONAL BOOL retrive = TRUE)
	{
		if (!retrive)
		{
			return m_itemID;
		}
		else
		{
			return getPath();
		}
	}
	void setFlags(IN DWORD flags)
	{
		m_flags = flags;
	}
	DWORD getFlags(void)
	{
		return m_flags;
	}
	BOOL hasChildren(void)
	{
		return ((m_flags & OPC_BROWSE_HASCHILDREN) == OPC_BROWSE_HASCHILDREN);
	}
	BOOL isItem(void)
	{
		return ((m_flags & OPC_BROWSE_ISITEM) == OPC_BROWSE_ISITEM);
	}

	SODaCServer* getServer(void)
	{
		if (m_browser.isNotNull())
		{
			return m_browser->getServer();
		}
		else
		{
			return NULL;
		}
	};

	SOCmnList<SODaCProperty> *getPropertyList(void);

protected:
	virtual ~SODaCNode();
	SOCmnPointer<SODaCNameSpaceBrowser> m_browser;
	SOCmnList<SODaCProperty> m_propList;
	// 2.0
	SOCmnStringList m_path;
	// 3.0
	SOCmnString m_itemID;
	DWORD m_flags;
	// XML-DA
	SOCmnString m_itemPath;
};




//-----------------------------------------------------------------------------
// SODaCTag                                                                   |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTag :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameString
{
public:

	SODaCTag(IN SODaCNameSpaceBrowser* parent, LPCTSTR szName);
	SODaCTag(IN SODaCNode* parent, LPCTSTR szName);
	SODaCTag(IN SOCltBrowseObject* parent, LPCTSTR wszName);

	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPath();
	virtual SOCmnString getItemPath(void)
	{
		return m_itemPath;
	}
	void setItemPath(IN LPCTSTR itemPath)
	{
		m_itemPath = itemPath;
	}

	SODaCNameSpaceBrowser* getNameSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getNameSpaceBrowser();
	};

	virtual SOCmnStringList* getPathList(void)
	{
		return &m_path;
	}

	virtual void setItemID(IN LPCTSTR itemID, IN LPCTSTR /* parentItemID */)
	{
		m_itemID = itemID;
	}
	virtual SOCmnString getItemID(IN OPTIONAL BOOL retrive = TRUE)
	{
		if (!retrive)
		{
			return m_itemID;
		}
		else
		{
			return getPath();
		}
	}
	void setFlags(IN DWORD flags)
	{
		m_flags = flags;
	}
	DWORD getFlags(void)
	{
		return m_flags;
	}

	SODaCServer* getServer(void)
	{
		if (m_browser.isNotNull())
		{
			return m_browser->getServer();
		}
		else
		{
			return NULL;
		}
	};

	SOCmnList<SODaCProperty> *getPropertyList(void);

protected:
	virtual ~SODaCTag();
	SOCmnPointer<SODaCNameSpaceBrowser> m_browser;
	SOCmnList<SODaCProperty> m_propList;
	// 2.0
	SOCmnStringList m_path;
	// 3.0
	SOCmnString m_itemID;
	DWORD m_flags;
	// XML-DA
	SOCmnString m_itemPath;
};




//-----------------------------------------------------------------------------
// SODaCProperty                                                              |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCProperty :
	public SOCltBrowseObject,
	virtual public SOCmnElementNameIntString
{
public:
	SODaCProperty(IN SODaCNode* parent, LPCTSTR szName, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath);
	SODaCProperty(IN SODaCTag* parent, LPCTSTR szName, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath);
	SODaCProperty(IN SOCltBrowseObject* parent, LPCTSTR szName, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath);

// Overrides
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual SOCmnString getPath();

	SODaCServer* getServer(void)
	{
		if (m_browser.isNotNull())
		{
			return m_browser->getServer();
		}
		else
		{
			return NULL;
		}
	};

	SODaCNameSpaceBrowser* getNameSpaceBrowser(void)
	{
		m_browser.addRef();
		return m_browser;
	}
	virtual SOCltBrowseObject* getBrowser(void)
	{
		return getNameSpaceBrowser();
	};

	virtual void setItemID(IN LPCTSTR itemID, IN LPCTSTR /* parentItemID */)
	{
		m_itemID = itemID;
	}
	virtual SOCmnString getItemID(IN OPTIONAL BOOL retrive = TRUE)
	{
		if (!retrive)
		{
			return m_itemID;
		}
		else
		{
			return getPath();
		}
	}
	void setValue(IN LPCVARIANT value)
	{
		m_value = value;
	}
	void getValue(IN LPVARIANT value)
	{
		m_value.copyTo(value);
	}

	SOCmnString getParentItemID(void)
	{
		return m_parentID;
	}
	SOCmnString getParentItemPath(void)
	{
		return m_parentPath;
	}
	DWORD getPropertyID(void)
	{
		return getNameInt();
	}
	VARTYPE getDatatype(void)
	{
		return m_dataType;
	}

	virtual void setPropertyName(IN LPCTSTR pN)
	{
		setElementName(pN);
	}
	virtual SOCmnString getPropertyName(void)
	{
		return getName();
	}

	static LPCTSTR getStandardPropertyNameByID(IN DWORD propID, OUT SOCmnString& propName);
	static void convertPropertyValueFromSOAPtoDCOM(IN DWORD propID, IN OUT SOCmnVariant& value);

protected:
	SOCmnPointer<SODaCNameSpaceBrowser> m_browser;
	SOCmnString m_parentID;
	VARTYPE m_dataType;
	// 3.0
	SOCmnString m_itemID;
	SOCmnVariant m_value;
	// XML
	SOCmnString m_parentPath;

	virtual ~SODaCProperty();
};


#pragma pack(pop)

#endif // _SODACBROWSE_H_
