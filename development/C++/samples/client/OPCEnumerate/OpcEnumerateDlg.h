#include "ClientApplication.h"
#include "ClientServerBrowser.h"
#include "afxwin.h"

using namespace SoftingOPCToolboxClient;

#if !defined(AFX_OPCENUMERATEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_)
#define AFX_OPCENUMERATEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateDlg dialog

class COpcEnumerateDlg : public CDialog
{
// Construction
public:
	COpcEnumerateDlg(CWnd* pParent = NULL); // standard constructor
	~COpcEnumerateDlg();
// Dialog Data
	//{{AFX_DATA(COpcEnumerateDlg)
	enum { IDD = IDD_OPCENUMERATE_DIALOG };
	CTreeCtrl   m_tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpcEnumerateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_imgList;
	Application* m_application;
	ServerBrowser* m_serverBrowser;
	//a list that contains all the pointers to ServerBrowserData objects created when browsing
	std::vector<ServerBrowserData*> m_browserData;
	std::vector<ServerBrowserData*>::iterator m_browserDataIterator;
	// Generated message map functions
	//{{AFX_MSG(COpcEnumerateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void BrowseServers(EnumOPCSpecification specification , HTREEITEM parent);

	void CreateMainTree();

	// delete all children found under the given item
	void DeleteChildItems(HTREEITEM hItem);

	//SOCmnPointer<SODaCServer> m_server;
public:

	//afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	// urlEditBox
	CEdit urlEditBox;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPCENUMERATEDLG_H__66C7D189_11DE_11D4_9201_009027A26C86__INCLUDED_)
