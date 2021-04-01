#include "ClientApplication.h"
#include "afxwin.h"

using namespace SoftingOPCToolboxClient;

#if !defined(AFX_AEBROWSEDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_)
#define AFX_AEBROWSEDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseDlg dialog

class CAEBrowseDlg : public CDialog
{
// Construction
public:
	CAEBrowseDlg(CWnd* pParent = NULL); // standard constructor
	virtual ~CAEBrowseDlg();
// Dialog Data
	//{{AFX_DATA(CAEBrowseDlg)
	enum { IDD = IDD_AEBROWSE_DIALOG };
	CTabCtrl    m_tab;
	CTreeCtrl   m_tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEBrowseDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_imgList;

	// Generated message map functions
	//{{AFX_MSG(CAEBrowseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void browseAreaSpace();
	Application* m_application;
	AeSession* m_session;

public:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AEBROWSEDLG_H__A492A329_1113_11D4_9200_009027A26C86__INCLUDED_)
