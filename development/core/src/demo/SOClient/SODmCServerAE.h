#ifndef _SODMCSERVERAE_H_
#define _SODMCSERVERAE_H_

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// SODmCServerAE                                                              |
//-----------------------------------------------------------------------------

class SODmCCmnServerAE
{
public:
	SODmCCmnServerAE();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	void setDescription(LPCTSTR description);
	void setProgID(LPCTSTR progid);
	LPCTSTR getDescription(void) const;
	LPCTSTR getProgID(void) const;

	void setStringFilter(LPCTSTR filter)
	{
		m_stringFilter = filter;
	}
	LPCTSTR getStringFilter(void) const
	{
		return m_stringFilter;
	}

protected:
	HTREEITEM m_treeHandle;
	SOCmnString m_label;
	SOCmnString m_description;
	SOCmnString m_progId;
	SOCmnString m_stringFilter;
}; // SODmCCmnServerAE

inline HTREEITEM SODmCCmnServerAE::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCCmnServerAE::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

inline void SODmCCmnServerAE::setDescription(LPCTSTR description)
{
	m_description = description;
}

inline void SODmCCmnServerAE::setProgID(LPCTSTR progid)
{
	m_progId = progid;
}

inline LPCTSTR SODmCCmnServerAE::getDescription(void) const
{
	return m_description;
}

inline LPCTSTR SODmCCmnServerAE::getProgID(void) const
{
	return m_progId;
}


#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCComServerAE                                                           |
//-----------------------------------------------------------------------------

class SODmCComServerAE :
	public SOAeCServer,
	virtual public SOCltComServerImpl,
	virtual public SOAeCComServerImpl,
public SODmCCmnServerAE
{
public:
	SODmCComServerAE()  {}

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	virtual BOOL setParentBranch(IN SOCmnElement* parent);
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);

	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
}; // SODmCComServerAE

#endif // SODMC_DCOM


#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPServerAE                                                           |
//-----------------------------------------------------------------------------

class SODmCTPServerAE :
	public SOAeCServer,
	virtual public SOAeCTPServerImpl,
public SODmCCmnServerAE
{
public:
	SODmCTPServerAE() {}

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	virtual BOOL setParentBranch(IN SOCmnElement* parent);
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCTPServerDA

#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// SODmCPPServerAE                                                            |
//-----------------------------------------------------------------------------

class SODmCPPServerAE  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPServerAE)
public:
	SODmCPPServerAE(IN SODmCPSheet* sheet, IN UINT resId);
	~SODmCPPServerAE();

	void setObject(IN SOAeCServer* srv);  // set server object

protected:
	SOAeCServer* m_obj;                     // server object

	BOOL connect(void);                     // connect server to OPC server

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerAE

inline void SODmCPPServerAE::setObject(IN SOAeCServer* srv)
{
	m_obj = srv;
}




//-----------------------------------------------------------------------------
// SODmCPPServerAEAttributes                                                  |
//-----------------------------------------------------------------------------

class SODmCPPServerAEAttributes : public SODmCPPServerAE
{
	DECLARE_DYNAMIC(SODmCPPServerAEAttributes)
public:
	SODmCPPServerAEAttributes(IN SODmCPSheet* sheet);
	~SODmCPPServerAEAttributes();

	void onSetObjectState(IN BYTE state);   // changed object state

protected:
	virtual BOOL OnInitDialog(void);                    // initialize page
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// PPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	// changed locale id in combo box
	afx_msg void onClickLocaleID();
	// changed client name in edit field
	afx_msg void onClickClientName();

	void enableLocaleID(IN BOOL enable);
	void enableClientName(IN BOOL enable);

	// get the available locale id from the server
	void queryLocaleIDs(void);
	// get the available filters from the server
	void queryAvailableFilters(void);

	// current attributes
	CString m_URL;
	CString m_localeID;
	BOOL m_validLocaleID;
	CString m_clientName;
	BOOL m_validClientName;
	CLSID m_clsid;

	// saved attributes (for reset)
	CString m_oURL;
	CString m_oLocaleID;
	BOOL m_oValidLocaleID;
	CString m_oClientName;
	BOOL m_oValidClientName;

	// controls
	SODmCPEdit m_edURL;
	SODmCPComboBox m_cbLocaleID;
	SODmCPButton m_buValidLocaleID;
	SODmCPEdit m_edClientName;
	SODmCPButton m_buValidClientName;
	SODmCPListCtrl m_liAvailableFilters;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerAEAttributes




//-----------------------------------------------------------------------------
// SODmCPPServerAEStatus                                                      |
//-----------------------------------------------------------------------------

class SODmCPPServerAEStatus : public SODmCPPServerAE
{
	DECLARE_DYNAMIC(SODmCPPServerAEStatus)
public:
	SODmCPPServerAEStatus(IN SODmCPSheet* sheet);
	~SODmCPPServerAEStatus();

	virtual void onOK(void);    // specific onOK handler

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// PPage virtuals
	virtual BOOL loadAttributes(void);
	virtual void enableControls(IN BOOL enable);
	virtual void clearAttributes(void);

	// current attributes
	CString m_startTime;
	CString m_currentTime;
	CString m_updateTime;
	CString m_version;
	CString m_vendorInfo;
	CString m_serverState;

	// controls
	SODmCPEdit m_edStartTime;
	SODmCPEdit m_edCurrentTime;
	SODmCPEdit m_edUpdateTime;
	SODmCPEdit m_edVersion;
	SODmCPEdit m_edVendorInfo;
	SODmCPEdit m_edServerState;

	DECLARE_MESSAGE_MAP()
}; // SODmCPPServerAEStatus




//-----------------------------------------------------------------------------
// SODmCPPServerAEBrowse                                                      |
//-----------------------------------------------------------------------------

class SODmCPPServerAEBrowse : public SODmCPPServerAE
{
	DECLARE_DYNAMIC(SODmCPPServerAEBrowse)
public:
	SODmCPPServerAEBrowse(IN SODmCPSheet* sheet);
	~SODmCPPServerAEBrowse();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// SODmCPPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	// current attributes
	CString m_stringFilter;

	// saved attributes (for reset)
	CString m_oStringFilter;

	// controls
	SODmCPEdit m_edStringFilter;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerAEBrowse



//-----------------------------------------------------------------------------
// SODmCPPServerAESecurity                                                    |
//-----------------------------------------------------------------------------

class SODmCPPServerAESecurity : public SODmCPPServerAE
{
	DECLARE_DYNAMIC(SODmCPPServerAESecurity)
public:
	SODmCPPServerAESecurity(IN SODmCPSheet* sheet);
	~SODmCPPServerAESecurity();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// PPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	// current attributes
	CString m_userName;
	CString m_password;
	BOOL m_storeSecInfo;

	// saved attributes (for reset)
	CString m_oUserName;
	CString m_oPassword;

	// controls
	SODmCPEdit m_edUserName;
	SODmCPEdit m_edPassword;
	SODmCPButton m_buStoreSecInfo;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerAESecurity


#endif

#endif
