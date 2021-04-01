#ifndef _DLGABOUT_H_
#define _DLGABOUT_H_

#include <afxdhtml.h>
#include "resource.h"

// CDlgAbout dialog

class CDlgAbout : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CDlgAbout)

public:
	CDlgAbout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAbout();
// Overrides
	HRESULT OnButtonOK(IHTMLElement* pElement);

// Dialog Data
	enum { IDD = IDD_ABOUT, IDH = IDR_HTML_DLGABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	CString m_version;
	CString m_build;
	CString m_softing;
	CString m_website;
	CString m_updatesRef;

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};

#endif
