#ifndef _SODmCComGroup_H_
#define _SODmCComGroup_H_

#include "SODmCPropFrm.h"

#ifdef SODMC_DA

#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCComGroup                                                                 |
//-----------------------------------------------------------------------------

class SODmCComGroup :
	public SODaCGroup,
	virtual public SODaCComGroupImpl
{
public:
	SODmCComGroup();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	HTREEITEM m_treeHandle;

	virtual void onSetParentBranch();
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onSetName(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCComGroup

inline HTREEITEM SODmCComGroup::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCComGroup::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

#endif SODMC_DCOM


#ifdef SODMC_SOAP

//-----------------------------------------------------------------------------
// SODmCXmlGroup                                                                 |
//-----------------------------------------------------------------------------

class SODmCXmlGroup :
	public SODaCGroup,
	virtual public SODaCXmlGroupImpl
{
public:
	SODmCXmlGroup();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	HTREEITEM m_treeHandle;

	virtual void onSetParentBranch();
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onSetName(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCXmlGroup

inline HTREEITEM SODmCXmlGroup::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCXmlGroup::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

#endif // SODMC_SOAP

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPGroup                                                                 |
//-----------------------------------------------------------------------------

class SODmCTPGroup :
	public SODaCGroup,
	virtual public SODaCTPGroupImpl
{
public:
	SODmCTPGroup();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	HTREEITEM m_treeHandle;

	virtual void onSetParentBranch();
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onSetName(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCTPGroup

inline HTREEITEM SODmCTPGroup::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCTPGroup::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// SODmCPPGroup                                                               |
//-----------------------------------------------------------------------------

class SODmCPPGroup : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPGroup)
public:
	SODmCPPGroup(SODmCPSheet* sheet);
	~SODmCPPGroup();

	void setObject(SODaCGroup* grp);            // set group object

	void onSetObjectState(BYTE state);  // changed object state
	void onSetName(LPCTSTR name)
	{
		m_groupName = name;
		UpdateData(FALSE);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// check button handlers
	afx_msg void onClickUpdateRate();
	afx_msg void onClickTimeBias();
	afx_msg void onClickDeadband();
	afx_msg void onClickLocaleID();
	afx_msg void onClickKeepAliveTime();

	// combo box change handler
	afx_msg void onChangeConnectionType(void);

	// SODmCPPage virtuals
	virtual void enableControls(IN BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	// optional attributes edit enabling
	void enableUpdateRate(IN BOOL enable);
	void enableTimeBias(IN BOOL enable);
	void enableDeadband(IN BOOL enable);
	void enableLocaleID(IN BOOL enable);
	void enableKeepAliveTime(IN BOOL enable);

	void queryLocaleIDs(IN BYTE state);

	// current attributes
	CString m_groupName;
	CString m_conType;
	CString m_dataRet;
	DWORD m_updateRate;
	LONG m_timeBias;
	FLOAT m_deadband;
	DWORD m_keepAliveTime;
	CString m_sLocaleID;
	CString m_localeID;
	BOOL m_validUpdateRate;
	BOOL m_validTimeBias;
	BOOL m_validDeadband;
	BOOL m_validLocaleID;
	BOOL m_validKeepAliveTime;
	BOOL m_public;
	CString m_serverHandle;
	CString m_clientHandle;

	// saved attributes (for reset)
	CString m_oGroupName;
	CString m_oConType;
	CString m_oDataRet;
	DWORD m_oUpdateRate;
	LONG m_oTimeBias;
	FLOAT m_oDeadband;
	DWORD m_oKeepAliveTime;
	CString m_oLocaleID;
	BOOL m_oValidUpdateRate;
	BOOL m_oValidTimeBias;
	BOOL m_oValidDeadband;
	BOOL m_oValidLocaleID;
	BOOL m_oValidKeepAliveTime;
	BOOL m_oPublic;

	// controls
	SODmCPEdit m_edName;
	SODmCPEdit m_edUpdateRate;
	SODmCPEdit m_edTimeBias;
	SODmCPEdit m_edDeadband;
	SODmCPEdit m_edKeepAliveTime;
	SODmCPComboBox m_cbLocaleID;
	SODmCPComboBox m_cbConType;
	SODmCPComboBox m_cbDataRet;
	SODmCPButton m_buValidUpdateRate;
	SODmCPButton m_buValidTimeBias;
	SODmCPButton m_buValidDeadband;
	SODmCPButton m_buValidLocaleID;
	SODmCPButton m_buValidKeepAliveTime;
	SODmCPButton m_buPublic;
	SODmCPEdit m_edClientHandle;
	SODmCPEdit m_edServerHandle;

	SODaCGroup* m_obj;  // group object
	BOOL m_preventLoadAttributes;

	DECLARE_MESSAGE_MAP()
}; //SODmCPPGroup


inline void SODmCPPGroup::setObject(IN SODaCGroup* grp)
{
	m_obj = grp;
}

#endif

#endif
