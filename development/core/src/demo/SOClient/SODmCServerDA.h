#ifndef _SODMCSERVERDA_H_
#define _SODMCSERVERDA_H_

#include "SODmCPropFrm.h"

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// SODmCCmnServerDA                                                              |
//-----------------------------------------------------------------------------

class SODmCCmnServerDA
{
public:
	SODmCCmnServerDA();

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
	void setAccessRightsFilter(DWORD filter)
	{
		m_accessRightsFilter = filter;
	}
	void setDatatypeFilter(VARTYPE filter)
	{
		m_datatypeFilter = filter;
	}
	void setVendorFilter(LPCTSTR filter)
	{
		m_vendorFilter = filter;
	}
	LPCTSTR getStringFilter(void) const
	{
		return m_stringFilter;
	}
	DWORD getAccessRightsFilter(void) const
	{
		return m_accessRightsFilter;
	}
	VARTYPE getDatatypeFilter(void) const
	{
		return m_datatypeFilter;
	}
	LPCTSTR getVendorFilter(void) const
	{
		return m_vendorFilter;
	}

protected:
	HTREEITEM m_treeHandle;
	SOCmnString m_description;
	SOCmnString m_progId;
	SOCmnString m_stringFilter;
	DWORD m_accessRightsFilter;
	VARTYPE m_datatypeFilter;
	SOCmnString m_label;
	SOCmnString m_vendorFilter;
}; // SODmCCmnServerDA

inline HTREEITEM SODmCCmnServerDA::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCCmnServerDA::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

inline void SODmCCmnServerDA::setDescription(LPCTSTR description)
{
	m_description = description;
}

inline void SODmCCmnServerDA::setProgID(LPCTSTR progid)
{
	m_progId = progid;
}

inline LPCTSTR SODmCCmnServerDA::getDescription(void) const
{
	return m_description;
}

inline LPCTSTR SODmCCmnServerDA::getProgID(void) const
{
	return m_progId;
}


#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCComServerDA                                                           |
//-----------------------------------------------------------------------------

class SODmCComServerDA :
	public SODaCServer,
	virtual public SOCltComServerImpl,
	virtual public SODaCComServerImpl,
public SODmCCmnServerDA
{
public:
	SODmCComServerDA();

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
}; // SODmCComServerDA

#endif // SODMC_DCOM

#ifdef SODMC_SOAP

//-----------------------------------------------------------------------------
// SODmCXmlServerDA                                                           |
//-----------------------------------------------------------------------------

class SODmCXmlServerDA :
	public SODaCServer,
	virtual public SODaCXmlServerImpl,
public SODmCCmnServerDA
{
public:
	SODmCXmlServerDA();

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	virtual BOOL setParentBranch(IN SOCmnElement* parent);
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCXmlServerDA

#endif SODMC_SOAP

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPServerDA                                                           |
//-----------------------------------------------------------------------------

class SODmCTPServerDA :
	public SODaCServer,
	virtual public SODaCTPServerImpl,
public SODmCCmnServerDA
{
public:
	SODmCTPServerDA();

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
// SODmCPPServerDA                                                            |
//-----------------------------------------------------------------------------

class SODmCPPServerDA  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPServerDA)
public:
	SODmCPPServerDA(IN SODmCPSheet* sheet, IN UINT resId);
	~SODmCPPServerDA();

	void setObject(IN SODaCServer* srv);  // set server object

protected:
	SODaCServer* m_obj;                 // server object

	BOOL connect(void);                     // connect server to OPC server

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerDA

inline void SODmCPPServerDA::setObject(IN SODaCServer* srv)
{
	m_obj = srv;
}




//-----------------------------------------------------------------------------
// SODmCPPServerDAAttributes                                                  |
//-----------------------------------------------------------------------------

class SODmCPPServerDAAttributes : public SODmCPPServerDA
{
	DECLARE_DYNAMIC(SODmCPPServerDAAttributes)
public:
	SODmCPPServerDAAttributes(IN SODmCPSheet* sheet);
	~SODmCPPServerDAAttributes();

	void onSetObjectState(IN BYTE state);   // changed object state

protected:
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
	void queryLocaleIDs(IN BYTE state);

	// current attributes
	CString m_URL;
	CString m_localeID;
	BOOL m_validLocaleID;
	CString m_clientName;
	BOOL m_validClientName;
	CString m_opcVersion;
	CString m_useOpcVersion;

	// saved attributes (for reset)
	CString m_oURL;
	CString m_oLocaleID;
	BOOL m_oValidLocaleID;
	CString m_oClientName;
	BOOL m_oValidClientName;
	CString m_oOpcVersion;
	CString m_oUseOpcVersion;

	// controls
	SODmCPEdit m_edURL;
	SODmCPComboBox m_cbLocaleID;
	SODmCPButton m_buValidLocaleID;
	SODmCPEdit m_edClientName;
	SODmCPButton m_buValidClientName;
	SODmCPEdit m_edOpcVersion;
	SODmCPComboBox m_cbUseOpcVersion;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerDAAttributes




//-----------------------------------------------------------------------------
// SODmCPPServerDAStatus                                                      |
//-----------------------------------------------------------------------------

class SODmCPPServerDAStatus : public SODmCPPServerDA
{
	DECLARE_DYNAMIC(SODmCPPServerDAStatus)
public:
	SODmCPPServerDAStatus(IN SODmCPSheet* sheet);
	~SODmCPPServerDAStatus();

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
	CString m_statusInfo;

	// controls
	SODmCPEdit m_edStartTime;
	SODmCPEdit m_edCurrentTime;
	SODmCPEdit m_edUpdateTime;
	SODmCPEdit m_edVersion;
	SODmCPEdit m_edVendorInfo;
	SODmCPEdit m_edStatusInfo;
	SODmCPEdit m_edServerState;

	DECLARE_MESSAGE_MAP()
}; // SODmCPPServerDAStatus




//-----------------------------------------------------------------------------
// SODmCPPServerDABrowse                                                      |
//-----------------------------------------------------------------------------

class SODmCPPServerDABrowse : public SODmCPPServerDA
{
	DECLARE_DYNAMIC(SODmCPPServerDABrowse)
public:
	SODmCPPServerDABrowse(IN SODmCPSheet* sheet);
	~SODmCPPServerDABrowse();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// SODmCPPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	afx_msg void onClickDA2();
	afx_msg void onClickDA3();

	// current attributes
	CString m_stringFilter;
	CString m_datatype;
	CString m_accessRights;
	CString m_vendorFilter;

	// saved attributes (for reset)
	CString m_oStringFilter;
	CString m_oDatatype;
	CString m_oAccessRights;
	CString m_oVendorFilter;

	// controls
	SODmCPEdit m_edStringFilter;
	SODmCPEdit m_edVendorFilter;
	SODmCPEdit m_cbAccessRights;
	SODmCPComboBox m_cbDatatype;
	SODmCPButton m_buDA2Filter;
	SODmCPButton m_buDA3Filter;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPServerDABrowse


//-----------------------------------------------------------------------------
// SODmCPPServerDASecurity                                                    |
//-----------------------------------------------------------------------------

class SODmCPPServerDASecurity : public SODmCPPServerDA
{
	DECLARE_DYNAMIC(SODmCPPServerDASecurity)
public:
	SODmCPPServerDASecurity(IN SODmCPSheet* sheet);
	~SODmCPPServerDASecurity();

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
}; //SODmCPPServerDASecurity

#endif

#endif
