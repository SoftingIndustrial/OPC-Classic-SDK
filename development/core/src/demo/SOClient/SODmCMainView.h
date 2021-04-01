#ifndef _SODMCMAINVIEW_H_
#define _SODMCMAINVIEW_H_

#include "SODmCObjectView.h"
#include "SODmCPropFrm.h"

//-----------------------------------------------------------------------------
// SODmCMainView                                                              |
//-----------------------------------------------------------------------------

// tab index
#if defined(SODMC_DA) && defined(SODMC_AE)
#define SODMCMAINVIEW_TAB_SRV    0
#define SODMCMAINVIEW_TAB_TDA    1
#define SODMCMAINVIEW_TAB_LDA    2
#define SODMCMAINVIEW_TAB_TAE    3
#define SODMCMAINVIEW_TAB_LAE    4
#define SODMCMAINVIEW_TAB_LAEC 5
#define SODMCMAINVIEW_TAB_ERR  6
#else
#ifdef SODMC_DA
#define SODMCMAINVIEW_TAB_SRV    0
#define SODMCMAINVIEW_TAB_TDA    1
#define SODMCMAINVIEW_TAB_LDA    2
#define SODMCMAINVIEW_TAB_ERR  3
#define SODMCMAINVIEW_TAB_TAE    -1
#define SODMCMAINVIEW_TAB_LAE    -2
#define SODMCMAINVIEW_TAB_LAEC -3
#endif
#ifdef SODMC_AE
#define SODMCMAINVIEW_TAB_SRV    0
#define SODMCMAINVIEW_TAB_TAE    1
#define SODMCMAINVIEW_TAB_LAE    2
#define SODMCMAINVIEW_TAB_LAEC 3
#define SODMCMAINVIEW_TAB_ERR  4
#define SODMCMAINVIEW_TAB_TDA    -1
#define SODMCMAINVIEW_TAB_LDA    -2
#endif
#endif

// image list index
#define SODMCMAINVIEW_IMG_NETWORK             1
#define SODMCMAINVIEW_IMG_MACHINE             0
#define SODMCMAINVIEW_IMG_DOMAIN              2
#define SODMCMAINVIEW_IMG_REMOTE_MACHINE      3
#define SODMCMAINVIEW_IMG_COM_CATEGORY        4
#define SODMCMAINVIEW_IMG_OPCSERVER           5
#define SODMCMAINVIEW_IMG_OPCSERVER_PROPS     6
#define SODMCMAINVIEW_IMG_SRV_DA              7
#define SODMCMAINVIEW_IMG_SRV_AE              7
#define SODMCMAINVIEW_IMG_NODE                8
#define SODMCMAINVIEW_IMG_TAG                 9
#define SODMCMAINVIEW_IMG_PROPERTY           10
#define SODMCMAINVIEW_IMG_SIMPLEEVENT        22
#define SODMCMAINVIEW_IMG_TRACKINGEVENT      11
#define SODMCMAINVIEW_IMG_ERROR              12
#define SODMCMAINVIEW_IMG_OBJ_DEFAULT        13
#define SODMCMAINVIEW_IMG_OBJ_R_R            13
#define SODMCMAINVIEW_IMG_OBJ_Y_Y            14
#define SODMCMAINVIEW_IMG_OBJ_G_G            15
#define SODMCMAINVIEW_IMG_OBJ_R_G            16
#define SODMCMAINVIEW_IMG_OBJ_R_Y            17
#define SODMCMAINVIEW_IMG_OBJ_Y_G            18
#define SODMCMAINVIEW_IMG_OBJ_Y_R            19
#define SODMCMAINVIEW_IMG_OBJ_G_Y            20
#define SODMCMAINVIEW_IMG_OBJ_G_R            21
#define SODMCMAINVIEW_IMG_CONDITIONEVENT     23
#define SODMCMAINVIEW_IMG_AREA                8
#define SODMCMAINVIEW_IMG_SOURCE              9
#define SODMCMAINVIEW_IMG_CONDITION          27
#define SODMCMAINVIEW_IMG_CATEGORY            8
#define SODMCMAINVIEW_IMG_ATTRIBUTE           9
#define SODMCMAINVIEW_IMG_CONDITION_CLASS    27
#define SODMCMAINVIEW_IMG_SUBCONDITION_CLASS 28
#define SODMCMAINVIEW_IMG_MANUAL             31
#define SODMCMAINVIEW_IMG_MACHINE_MANUAL     32
#define SODMCMAINVIEW_IMG_CLSID_MANUAL       33
#define SODMCMAINVIEW_IMG_NODE_ITEM          34

class SODmCItem;
class SODmCEvent;

class SODmCMainView : public CFormView
{
public:
	virtual ~SODmCMainView();

	int getSelectedTab(void) const;

	CListCtrl* getListDA(void) const;
	CListCtrl* getListAE(void) const;
	CListCtrl* getListAEC(void) const;
	CListCtrl* getListErr(void) const;

#ifdef SODMC_DA
	// DA browse tree
	void changeSelectedServerDA(IN SODaCServer* server, IN BOOL refresh);

	// DA item list
	void changeSelectedItem(IN SODmCItem* item);
	void clearSelection();
	SODmCItem* getSelectedItem(void) ;
	void updateListDAIndex(IN int startIdx);
	void updateItemProperties(void);
	void readGrouped();
	void performGroupedStateTransition(BYTE state);
#endif
#ifdef SODMC_AE
	// AE browse tree
	void changeSelectedSubscription(IN SOAeCSubscription* subs, IN BOOL refresh);

	// AE condition list
	SODmCEvent* getSelectedCondition(void);
#endif

	// error list
	void insertError(SOCltElement* obj, DWORD result, DWORD errorCode, LPCTSTR errotText);

#ifdef SODMC_DA
	void writeFromFile(IN SODmCItem* item);
	void readToFile(IN SODmCItem* item);
#endif

protected:
	BOOL m_firstInit;
	CTabCtrl m_tab;
	CListCtrl m_listDA;
	CListCtrl m_listAE;
	CListCtrl m_listAEC;
	CTreeCtrl m_treeSrv;
	CStatic m_lbURLSrv;
	SODmCPEdit m_edURLSrv;
	CString m_szURLSrv;
	CButton m_buAddSrv;
	CTreeCtrl m_treeDA;
	CTreeCtrl m_treeAE;
	CListCtrl m_listErr;
	CImageList m_images;    // image list

	CLSID m_serialINATServerClsId;
	CLSID m_ethernetINATServerClsId;
#ifdef SODMC_DA
	SOCmnPointer<SODaCServer> m_treeDAServer;
	SOCmnPointer<SODmCItem> m_listDAItem;
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCSubscription> m_treeAESubscription;
#endif

	SODmCMainView();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();                     // called first time after construct
	afx_msg void OnSize(UINT nType, int cx, int cy);    // called if size changes
	afx_msg void OnDestroy(void);

	// main message handlers
	void OnTabSC(NMHDR* pNMHDR, LRESULT* pResult);
	void OnRBD(NMHDR* pNMHDR, LRESULT* pResult);

#ifdef SODMC_DCOM
	// servers tree message handler
	void OnTreeSrvIE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeSrvDI(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeSrvDC(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeSrvBLE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeSrvELE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeSrvCL(NMHDR* pNMHDR, LRESULT* pResult);
#endif // SODMC_DCOM

	// DA browse tree message handler
	void OnTreeDaIE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeDaDI(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeDaDC(NMHDR* pNMHDR, LRESULT* pResult);

	// DA item list message handler
	void OnListDaIC(NMHDR* pNMHDR, LRESULT* pResult);

	// AE browse tree message handler
	void OnTreeAeIE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeAeDI(NMHDR* pNMHDR, LRESULT* pResult);

	// AE event list message handler
	void OnListAeDI(NMHDR* pNMHDR, LRESULT* pResult);

	// AE condition list message handler
	void OnListAecDI(NMHDR* pNMHDR, LRESULT* pResult);
	void OnListAecDC(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void onButtonAddServer(void);

	// menu handlers
	afx_msg void onDeleteObject(void);
	afx_msg void onUpdDeleteObject(OUT CCmdUI* pCmdUI);
	afx_msg void onAddItem(void);
	afx_msg void onUpdAddItem(OUT CCmdUI* pCmdUI);
	afx_msg void onAddServer(void);
	afx_msg void onUpdAddServer(OUT CCmdUI* pCmdUI);
	afx_msg void onAddTagsOfNode(void);
	afx_msg void onUpdAddTagsOfNode(OUT CCmdUI* pCmdUI);
	afx_msg void onBrowseAllTagsOfNode2File(void);
	afx_msg void onUpdBrowseAllTagsOfNode2File(OUT CCmdUI* pCmdUI);
	afx_msg void onAddNewGroupWithAllTags(void);
	afx_msg void onUpdAddNewGroupWithAllTags(OUT CCmdUI* pCmdUI);

	afx_msg void onProperties(void);
	afx_msg void onUpdProperties(OUT CCmdUI* pCmdUI);

	afx_msg void onRefresh(void);
	afx_msg void onUpdRefresh(OUT CCmdUI* pCmdUI);

	afx_msg void onReadItem(void);
	afx_msg void onUpdReadItem(OUT CCmdUI* pCmdUI);

	afx_msg void onWriteItem(void);
	afx_msg void onUpdWriteItem(OUT CCmdUI* pCmdUI);

	afx_msg void OnLogToFile();
	afx_msg void OnUpdLogToFile(CCmdUI* pCmdUI);

	afx_msg void onAcknowledge(void);
	afx_msg void onUpdAcknowledge(OUT CCmdUI* pCmdUI);

	afx_msg void onWriteFromFile(void);
	afx_msg void onUpdWriteFromFile(OUT CCmdUI* pCmdUI);
	afx_msg void onReadToFile(void);
	afx_msg void onUpdReadToFile(OUT CCmdUI* pCmdUI);

	afx_msg void onStartObjFlat(void);
	afx_msg void onConnectObjFlat(void);
	afx_msg void onStopObj(void);
	afx_msg void onUpdStartObjFlat(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdConnectObjFlat(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdStopObj(OUT CCmdUI* pCmdUI);

	afx_msg void onErrDeleteAll(void);
	afx_msg void onUpdErrDeleteAll(OUT CCmdUI* pCmdUI);

	afx_msg LONG onEnter(IN UINT wParam, IN LONG lParam);

	void insertNetwork(LPNETRESOURCE pnr, HTREEITEM hItem);
#ifdef SODMC_DA
	void addTagsOfNode(SOCltBrowseObject* node, SODaCGroup* group, OPCNAMESPACETYPE nsType);
	void browseAllTagsOfNode2File(SOCltBrowseObject* node, CFile* file, OPCNAMESPACETYPE nsType);
#endif
	void getResultText(IN SOCltServer* srv, IN HRESULT result, OUT SOCmnString& string);

	DECLARE_DYNCREATE(SODmCMainView)
	DECLARE_MESSAGE_MAP()
public:

	void selectDAItemTab(void);
}; // SODmCMainView

inline CListCtrl* SODmCMainView::getListDA(void) const
{
	return (CListCtrl*)&m_listDA;
}

inline CListCtrl* SODmCMainView::getListAE(void) const
{
	return (CListCtrl*)&m_listAE;
}

inline CListCtrl* SODmCMainView::getListAEC(void) const
{
	return (CListCtrl*)&m_listAEC;
}

inline CListCtrl* SODmCMainView::getListErr(void) const
{
	return (CListCtrl*)&m_listErr;
}

inline int SODmCMainView::getSelectedTab(void) const
{
	return m_tab.GetCurSel();
}

inline void SODmCMainView::selectDAItemTab(void)
{
	m_tab.SetCurSel(2);
	OnTabSC(NULL, NULL);
}

//-----------------------------------------------------------------------------
// Global SODmCMainView access function                                       |
//-----------------------------------------------------------------------------

SODmCMainView* getMainView();

#endif
