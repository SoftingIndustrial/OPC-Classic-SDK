#include "ClientApplication.h"
#include "afxwin.h"

using namespace SoftingOPCToolboxClient;

#if !defined(AFX_AEEVENTTYPESDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_)
#define AFX_AEEVENTTYPESDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesDlg dialog

class CAEEventTypesDlg : public CDialog
{
// Construction
public:
	CAEEventTypesDlg(CWnd* pParent = NULL); // standard constructor
	~CAEEventTypesDlg();
// Dialog Data
	//{{AFX_DATA(CAEEventTypesDlg)
	enum { IDD = IDD_AEEVENTTYPES_DIALOG };
	CTabCtrl    m_tab;
	CTreeCtrl   m_tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEEventTypesDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_imgList;

	// Generated message map functions
	//{{AFX_MSG(CAEEventTypesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void BrowseEventSpace();

	Application* m_application;
	AeSession* m_session;

public:
	CEdit m_availableFiltersEditBox;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AEEVENTTYPESDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_)
