#ifndef _SODMCPROPFRM_H_
#define _SODMCPROPFRM_H_


// key messages
#define OWNMSG_HIT_ENTER    (WM_USER + 450)
#define OWNMSG_HIT_ESCAPE   (WM_USER + 451)


//-----------------------------------------------------------------------------
// SODmCPTabCtrl                                                              |
//-----------------------------------------------------------------------------

class SODmCPTabCtrl : public CTabCtrl
{
public:
	SODmCPTabCtrl();

protected:
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPTabCtrl




//-----------------------------------------------------------------------------
// SODmCPSheet                                                                |
//-----------------------------------------------------------------------------

class SODmCPPage;
class SODmCPPServerDAAttributes;
class SODmCPPServerDAStatus;
class SODmCPPServerDABrowse;
class SODmCPPServerDASecurity;
class SODmCPPGroup;
class SODmCPPItemAttributes;
class SODmCPPAbout;
class SODmCPPServerAEAttributes;
class SODmCPPServerAEStatus;
class SODmCPPServerAESecurity;
class SODmCPPServerAEBrowse;
class SODmCPPSubscriptionAttributes;

class SODmCItem;
class SODmCPPProperties;
class SODmCPPProperty;
class SODmCPropertyFrame;
class SODmCPPTagWrite;
class SODaCNode;
class SODaCTag;
class SODaCProperty;

class SODmCPSheet : public CPropertySheet
{
	friend SODmCPPage;

public:
	SODmCPSheet(SODmCPropertyFrame* frame);
	~SODmCPSheet(void);

	// sheet initialisation
	void onCreate();

	// insert property page into sheet
	void insertPage(IN SODmCPPage* pPropPage);
	// remove the current pages from the sheet
	void removeCurrentPages(void);

	// show the right property pages
	void showAbout(void);
#ifdef SODMC_DA
	void showServerDAAttributes(IN SODaCServer* srv);
	void showGroupAttributes(IN SODaCGroup* grp);
	void showItemAttributes(IN SODmCItem* item);
	void showNodeAttributes(IN SODaCNode* node);
	void showTagAttributes(IN SODaCTag* tag);
	void showPropertyAttributes(IN SODaCProperty* prop);
#endif
#ifdef SODMC_AE
	void showServerAEAttributes(IN SOAeCServer* srv);
	void showSubscriptionAttributes(IN SOAeCSubscription* srv);
#endif

	// object state changes
	void setObjectState(IN SOCmnObject* obj, IN BYTE state);
	void setTargetObjectState(IN SOCmnObject* obj, IN BYTE state);
	void setName(IN SOCmnObject* obj, IN LPCTSTR name);
#ifdef SODMC_DA
	void setItemID(IN SODaCItem* item, IN LPCTSTR itemID);
#endif

	// object removed
	void removedObject(IN GenericObject* obj);

protected:
	SODmCPropertyFrame* m_frame;                      // property frame
	// property pages
	SODmCPPAbout* m_ppAbout;                                        // about property page
#ifdef SODMC_DA
	SODmCPPServerDAAttributes* m_ppServerAttributes;  // server attributes property page
	SODmCPPServerDAStatus* m_ppServerStatus;                // server status property page
	SODmCPPServerDABrowse* m_ppServerBrowse;                // browse server property page
	SODmCPPServerDASecurity* m_ppServerSecurity;        // security server property page
	SODmCPPGroup* m_ppGroup;                                      // group attributes property page
	SODmCPPItemAttributes* m_ppItemAttributes;            // item attributes property page
	SODmCPPProperties* m_ppNode;                      // node attributes property page
	SODmCPPTagWrite* m_ppNodeWrite;                   // tag value property page
	SODmCPPProperties* m_ppTag;                       // tag attributes property page
	SODmCPPTagWrite* m_ppTagWrite;                    // tag value property page
	SODmCPPProperty* m_ppProperty;                    // property attributes property page
#endif
#ifdef SODMC_AE
	SODmCPPServerAEAttributes* m_ppServerAEAttributes;// server attributes property page
	SODmCPPServerAEStatus* m_ppServerAEStatus;            // server status property page
	SODmCPPServerAEBrowse* m_ppServerAEBrowse;            // browse server property page
	SODmCPPServerAESecurity* m_ppServerAESecurity;    // security server property page
	SODmCPPSubscriptionAttributes* m_ppSubscriptionAttributes; // subscription attributes property page
#endif

#ifdef SODMC_DA
	CPropertyPage* m_lastServerPage;            // last shown server page
	CPropertyPage* m_lastGroupPage;             // last shown group page
	CPropertyPage* m_lastItemPage;              // last shown item page
	CPropertyPage* m_lastNodePage;              // last shown node page
	CPropertyPage* m_lastTagPage;                 // last shown tag page
	CPropertyPage* m_lastPropertyPage;    // last shown property page
#endif
#ifdef SODMC_AE
	CPropertyPage* m_lastServerAEPage;    // last shown server AE page
	CPropertyPage* m_lastSubscriptionPage;// last shown subscription AE page
#endif

	SODmCPTabCtrl m_tab;                                // tab control of property sheet
	CObList m_listPages;                              // list of the current property pages

	SOCmnPointer<SOCmnObject> m_obj;        // current object
#ifdef SODMC_DA
	SOCmnPointer<SODaCServer> m_srv;      // current server DA
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCServer> m_srvAE;  // current server AE
#endif

	void saveLastPage(void);                // save the last shown page

	// key handlers
	afx_msg LONG onEnter(IN UINT wParam, IN LONG lParam);
	afx_msg LONG onEscape(IN UINT wParam, IN LONG lParam);

	DECLARE_MESSAGE_MAP()
}; // SODmCPSheet




//-----------------------------------------------------------------------------
// SODmCPPage                                                                 |
//-----------------------------------------------------------------------------

class SODmCPPage : public CPropertyPage
{
	DECLARE_DYNAMIC(SODmCPPage)
public:
	SODmCPPage(SODmCPSheet* sheet, UINT resId);

	// button handlers
	virtual void onOK(void);
	virtual void onReset(void);

	// enable button
	void enableButton(IN UINT button, IN BOOL enable);

protected:
	SODmCPSheet* m_sheet;   // property sheet

#ifdef SODMC_DA
	SODaCServer* getServerDA(IN GenericElement* obj);     // get server object
	SODaCServer* connectServerDA(IN GenericElement* obj);   // connect server object
#endif

	virtual BOOL OnSetActive(void);     // handler for page activation
	virtual BOOL OnKillActive(void);    // handler for page deactivation

	// enable controls
	virtual void enableControls(IN BOOL enable);
	// load attributes of page
	virtual BOOL loadAttributes(void);
	// store attributes of page
	virtual BOOL storeAttributes(void);
	// clear the attribute values shown in the mask
	virtual void clearAttributes(void);
	// save the current attributes
	virtual void saveCurrentAttributes(void);
	// reset the current attributes
	virtual void resetCurrentAttributes(void);

	// key handlers
	afx_msg LONG onEnter(IN UINT wParam, IN LONG lParam);
	afx_msg LONG onEscape(IN UINT wParam, IN LONG lParam);

	DECLARE_MESSAGE_MAP()
}; // SODmCPPage




//-----------------------------------------------------------------------------
// SODmCPEdit                                                                  |
//-----------------------------------------------------------------------------

class SODmCPEdit : public CEdit
{
public:
	SODmCPEdit();

	BYTE m_lastControl;     // is last control in property page
	BYTE m_firstControl;    // is first control in property page

	void setNotificationWnd(CWnd* pWnd)
	{
		m_notificationWnd = pWnd;
	}

protected:
	CWnd* m_notificationWnd;

	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPEdit




//-----------------------------------------------------------------------------
// SODmCPButton                                                               |
//-----------------------------------------------------------------------------

class SODmCPButton : public CButton
{
public:
	SODmCPButton();

	BYTE m_lastControl;     // is last control in property page
	BYTE m_firstControl;    // is first control in property page

protected:
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPButton




//-----------------------------------------------------------------------------
// SODmCPComboBox                                                             |
//-----------------------------------------------------------------------------

class SODmCPComboBox : public CComboBox
{
public:
	SODmCPComboBox();

	BYTE m_lastControl;     // is last control in property page
	BYTE m_firstControl;    // is first control in property page

protected:
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPComboBox




//-----------------------------------------------------------------------------
// SODmCPListCtrl                                                             |
//-----------------------------------------------------------------------------

class SODmCPListCtrl : public CListCtrl
{
public:
	SODmCPListCtrl();

	BYTE m_lastControl;     // is last control in property page
	BYTE m_firstControl;    // is first control in property page

protected:
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPListCtrl




//-----------------------------------------------------------------------------
// SODmCPTreeCtrl                                                             |
//-----------------------------------------------------------------------------

class SODmCPTreeCtrl : public CTreeCtrl
{
public:
	SODmCPTreeCtrl();

	BYTE m_lastControl;     // is last control in property page
	BYTE m_firstControl;    // is first control in property page

protected:
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
}; // SODmCPTreeCtrl




//-----------------------------------------------------------------------------
// SODmCPropertyFrame                                                         |
//-----------------------------------------------------------------------------

class SODmCPropertyFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(SODmCPropertyFrame)
public:
	SODmCPropertyFrame();
	virtual ~SODmCPropertyFrame();

	SODmCPSheet* getPropertySheet()
	{
		return m_pPropSheet;
	}

	// enable button of property view
	void enable(UINT button, BOOL enable);

	// called after removed object
	void removedObject(SOCmnObject* obj);

	// message hanlder for messages from the property sheet
	void onEnter(void);
	void onEscape(void);

protected:
	SODmCPSheet* m_pPropSheet;  // the property sheet
	CToolBar m_wndToolBar;  // toolbar of the property frame
	CReBar m_wndReBar;
	BOOL m_enableApply;               // enable OK button
	BOOL m_enableReset;             // enable Reset button

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();

	// button click handler
	afx_msg void onOK(void);
	afx_msg void onApply(void);
	afx_msg void onReset(void);
	afx_msg void onUpdApply(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdReset(OUT CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
}; // SODmCPropertyFrame

extern SODmCPropertyFrame* getPropertyFrame(void);



//-----------------------------------------------------------------------------
// SODmCPPAbout                                                               |
//-----------------------------------------------------------------------------

class SODmCPPAbout : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPAbout)
public:
	SODmCPPAbout(SODmCPSheet* sheet);

protected:
	virtual BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()
}; //SODmCPPAbout

#endif
