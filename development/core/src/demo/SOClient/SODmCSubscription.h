#ifndef _SODMCSUBSCRIPTION_H_
#define _SODMCSUBSCRIPTION_H_

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// SODmCComSubscription                                                          |
//-----------------------------------------------------------------------------

class SODmCComSubscription :
	public SOAeCSubscription,
	virtual public SOAeCComSubscriptionImpl
{
public:
	SODmCComSubscription();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	LPCTSTR getLabel(void)
	{
		return m_label;
	}
	void setLabel(LPCTSTR label);

	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	HTREEITEM m_treeHandle;
	SOCmnString m_label;

	virtual void onSetParentBranch();
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCComSubscription

inline HTREEITEM SODmCComSubscription::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCComSubscription::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}


#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPSubscription                                                                 |
//-----------------------------------------------------------------------------

class SODmCTPSubscription :
	public SOAeCSubscription,
	virtual public SOAeCTPSubscriptionImpl
{
public:
	SODmCTPSubscription();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	LPCTSTR getLabel(void)
	{
		return m_label;
	}
	void setLabel(LPCTSTR label);

	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

protected:
	HTREEITEM m_treeHandle;
	SOCmnString m_label;

	virtual void onSetParentBranch();
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCTPSubscription

inline HTREEITEM SODmCTPSubscription::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCTPSubscription::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

#endif // SOFEATURE_TUNNEL



//-----------------------------------------------------------------------------
// SODmCPPSubscription                                                        |
//-----------------------------------------------------------------------------

class SODmCPPSubscription  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPSubscription)
public:
	SODmCPPSubscription(IN SODmCPSheet* sheet, IN UINT resId);
	~SODmCPPSubscription();

	void setObject(IN SOAeCSubscription* sus);  // set subscription object

protected:
	SOAeCSubscription* m_obj;   // subscription object

	DECLARE_MESSAGE_MAP()
}; //SODmCPPSubscription

inline void SODmCPPSubscription::setObject(IN SOAeCSubscription* sus)
{
	m_obj = sus;
}




//-----------------------------------------------------------------------------
// SODmCPPSubscriptionAttributes                                              |
//-----------------------------------------------------------------------------

class SODmCPPSubscriptionAttributes : public SODmCPPSubscription
{
	DECLARE_DYNAMIC(SODmCPPSubscriptionAttributes)
public:
	SODmCPPSubscriptionAttributes(IN SODmCPSheet* sheet);
	~SODmCPPSubscriptionAttributes();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// SODmCPPage virtuals
	virtual void enableControls(IN BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	// current attributes
	DWORD m_bufferTime;
	DWORD m_maxSize;
	CString m_clientHandle;

	// saved attributes (for reset)
	DWORD m_oBufferTime;
	DWORD m_oMaxSize;

	// controls
	SODmCPEdit m_edBufferTime;
	SODmCPEdit m_edMaxSize;
	SODmCPEdit m_edClientHandle;

	DECLARE_MESSAGE_MAP()
}; // SODmCPPSubscriptionAttributes

#endif

#endif
