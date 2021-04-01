#ifndef _SODMCOBJECTVIEW_H_
#define _SODMCOBJECTVIEW_H_

#include <afxcview.h>

//-----------------------------------------------------------------------------
// SODmCObjectView                                                            |
//-----------------------------------------------------------------------------

// offsets in image list
#define SODMCOBJECTVIEW_IMG_SPEC_DA 0
#define SODMCOBJECTVIEW_IMG_SPEC_AE 1
#define SODMCOBJECTVIEW_IMG_OBJ_DEFAULT 2
#define SODMCOBJECTVIEW_IMG_OBJ_R_R 2
#define SODMCOBJECTVIEW_IMG_OBJ_Y_Y 3
#define SODMCOBJECTVIEW_IMG_OBJ_G_G 4
#define SODMCOBJECTVIEW_IMG_OBJ_R_G 5
#define SODMCOBJECTVIEW_IMG_OBJ_R_Y 6
#define SODMCOBJECTVIEW_IMG_OBJ_Y_G 7
#define SODMCOBJECTVIEW_IMG_OBJ_Y_R 8
#define SODMCOBJECTVIEW_IMG_OBJ_G_Y 9
#define SODMCOBJECTVIEW_IMG_OBJ_G_R 10
#define SODMCOBJECTVIEW_IMG_WORK_R_G    11
#define SODMCOBJECTVIEW_IMG_WORK_R_Y    15
#define SODMCOBJECTVIEW_IMG_WORK_Y_G    19

class SODmCItem;

class SODmCObjectView : public CTreeView
{
public:
	// view initialization
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(IN CREATESTRUCT& cs);

	HTREEITEM getTreeHandleDA(void);
	HTREEITEM getTreeHandleAE(void);
	void deleteDATree(void);
	void deleteAETree(void);

#ifdef SODMC_DA
	SODaCServer* getSelectedServerDA(void);
	SODaCGroup* getSelectedGroup(void);
	SODmCItem* getSelectedItem(void);
#endif
#ifdef SODMC_AE
	SOAeCServer* getSelectedServerAE(void);
	SOAeCSubscription* getSelectedSubscription(void);
#endif
	SOCltElement* getSelectedObject(void);

#ifdef SODMC_DA
	void changeSelectedItem(IN SODmCItem* item);
	void changeSelectedServerDA(IN SODaCServer* server);
#endif
#ifdef SODMC_AE
	void changeSelectedServerAE(IN SOAeCServer* server);
#endif

	static void setObjectStateImage(IN HTREEITEM treeHandle,
									IN int listIdx, IN BYTE objState,   IN BYTE targetObjState);

	void addToWorkingList(IN SOCltElement* element);
	void removeFromWorkingList(IN SOCltElement* element);

protected:
	BOOL m_firstInit;
	CImageList m_images;                // image list
	UINT m_timer;
	HTREEITEM m_hDA;
	HTREEITEM m_hAE;
	SOCmnList<SOCltElement> m_workingList;

	afx_msg void OnRButtonDown(IN UINT nFlags, IN CPoint point);    // popup menu
	afx_msg void OnDestroy(void);

	afx_msg void OnTimer(UINT nIDEvent);

	void OnSC(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDI(NMHDR* pNMHDR, LRESULT* pResult);
	void OnBLE(NMHDR* pNMHDR, LRESULT* pResult);
	void OnELE(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void onDelete(void);
	afx_msg void onUpdDelete(OUT CCmdUI* pCmdUI);

	afx_msg void onProperties(void);
	afx_msg void onUpdProperties(OUT CCmdUI* pCmdUI);

	afx_msg void onAddGroup(void);
	afx_msg void onUpdAddGroup(OUT CCmdUI* pCmdUI);
	afx_msg void onAddSubscription(void);
	afx_msg void onUpdAddSubscription(OUT CCmdUI* pCmdUI);
	afx_msg void onAddItem(void);
	afx_msg void onUpdAddItem(OUT CCmdUI* pCmdUI);

	afx_msg void onRefreshGroup(void);
	afx_msg void onUpdRefreshGroup(OUT CCmdUI* pCmdUI);
	afx_msg void onReadItem(void);
	afx_msg void onUpdReadItem(OUT CCmdUI* pCmdUI);

	afx_msg void onWriteFromFile(void);
	afx_msg void onUpdWriteFromFile(OUT CCmdUI* pCmdUI);
	afx_msg void onReadToFile(void);
	afx_msg void onUpdReadToFile(OUT CCmdUI* pCmdUI);

	afx_msg void onStartObjFlat(void);
	afx_msg void onStartObjDeep(void);
	afx_msg void onConnectObjFlat(void);
	afx_msg void onConnectObjDeep(void);
	afx_msg void onStopObj(void);
	afx_msg void onUpdStartObjFlat(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdStartObjDeep(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdConnectObjFlat(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdConnectObjDeep(OUT CCmdUI* pCmdUI);
	afx_msg void onUpdStopObj(OUT CCmdUI* pCmdUI);

	SODmCObjectView();        // create from serialization only
	DECLARE_DYNCREATE(SODmCObjectView)

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdLogtofile(CCmdUI* pCmdUI);
	afx_msg void OnLogtofile();
}; // SODmCObjectView


//-----------------------------------------------------------------------------
// Global SODmCObjectView access function                                     |
//-----------------------------------------------------------------------------

SODmCObjectView* getObjectView();

#endif
