#include "stdafx.h"
#include "DABrowse.h"
#include "DABrowseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDABrowseApp

BEGIN_MESSAGE_MAP(CDABrowseApp, CWinApp)
	//{{AFX_MSG_MAP(CDABrowseApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDABrowseApp construction

CDABrowseApp::CDABrowseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDABrowseApp object

CDABrowseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDABrowseApp initialization

BOOL CDABrowseApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	CDABrowseDlg  dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
