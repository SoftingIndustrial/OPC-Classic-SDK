#ifndef _SODMCMAINFRM_H_
#define _SODMCMAINFRM_H_

#include "SODmCPropFrm.h"

//-----------------------------------------------------------------------------
// SODmCMainFrame                                                             |
//-----------------------------------------------------------------------------

class SODmCPropertyFrame;

class SODmCMainFrame : public CFrameWnd
{

public:
	SODmCMainFrame();
	virtual ~SODmCMainFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// property frame
	SODmCPropertyFrame* getPropertyFrame(void)
	{
		return m_pPropFrame;
	}
	void showPropertyFrame(void);

	void updateDAWriteBar(void);

protected:
	CStatusBar m_wndStatusBar;         // status bar
	CToolBar m_wndToolBar;             // main tool bar
	CReBar m_wndReBar;                 // re bar
	CSplitterWnd m_wndSplitter;        // splitter window for the object view
	SODmCPropertyFrame* m_pPropFrame;  // property frame
	BOOL m_propFrameMinimized;

	// DA write dialog bar
	CDialogBar m_wndDAWriteBar;        // DA write dialog bar
	SODmCPButton m_writeDAItemButton;  // DA write button
	SODmCPEdit m_writeDAItemValue;     // DA write value

	SODmCPropertyFrame* createPropertyFrame(void);

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	LRESULT onCallbackNofiy(WPARAM, LPARAM);

	afx_msg LONG onEnter(IN UINT wParam, IN LONG lParam);

	afx_msg void onStart(void);
	afx_msg void onConnect(void);
	afx_msg void onStop(void);

	afx_msg void onWriteDAItem(void);
	afx_msg void onUpdWriteDAItem(OUT CCmdUI* pCmdUI);

	afx_msg void onViewDAWriteBar(void);
	afx_msg void onUpdViewDAWriteBar(OUT CCmdUI* pCmdUI);
	afx_msg void onViewPropertyView(void);
	afx_msg void onUpdViewPropertyView(OUT CCmdUI* pCmdUI);

	afx_msg void onEditOptions(void);

	afx_msg void onAbout(void);
	afx_msg void onUpdAbout(OUT CCmdUI* pCmdUI);

	DECLARE_DYNAMIC(SODmCMainFrame)
	DECLARE_MESSAGE_MAP()
public:
}; // SODmCMainFrame

inline SODmCMainFrame* getMainFrame(void)
{
	return (SODmCMainFrame*)AfxGetMainWnd();
}



//-----------------------------------------------------------------------------
// SODmCDlgOptions                                                            |
//-----------------------------------------------------------------------------

class SODmCDlgOptions : public CDialog
{
public:
	SODmCDlgOptions(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DLG_OPTIONS };

protected:
	virtual BOOL OnInitDialog(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void onTimeOnly();
	afx_msg void onTimeDate();
	afx_msg void onValuePure();
	afx_msg void onValueVT();
	afx_msg void onValueDec();
	afx_msg void onValueHex();
	afx_msg void onCmOn();
	afx_msg void onCmOff();
	afx_msg void onIoErrorsOn();
	afx_msg void onIoErrorsOff();
	afx_msg void onTrcOn();
	afx_msg void onTrcOff();
	afx_msg void onAlphaItmOn();
	afx_msg void onAlphaItmOff();
	afx_msg void onAlphaAEOn();
	afx_msg void onAlphaAEOff();

	DECLARE_MESSAGE_MAP()
public:
}; // SODmCDlgOptions

#endif
