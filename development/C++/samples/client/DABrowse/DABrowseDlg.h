#include "ClientApplication.h"
#include "MyDaSession.h"

using namespace SoftingOPCToolboxClient;

#if !defined(AFX_DABROWSEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_)
#define AFX_DABROWSEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDABrowseDlg dialog

class CDABrowseDlg : public CDialog
{
// Construction
public:
	CDABrowseDlg(CWnd* pParent = NULL); // standard constructor
	~CDABrowseDlg();
// Dialog Data
	//{{AFX_DATA(CDABrowseDlg)
	enum { IDD = IDD_DABROWSE_DIALOG };
	CTreeCtrl   m_tree;
	CStatic		m_label;
	bool		m_ready;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDABrowseDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_imgList;

	// Generated message map functions
	//{{AFX_MSG(CDABrowseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void browseNamespace();
	BOOL isExpandable(DaAddressSpaceElement& element);
	Application* m_application;
	MyDaSession* m_session;

public:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DABROWSEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_)
