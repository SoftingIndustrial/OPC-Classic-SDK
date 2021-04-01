#include "stdafx.h"
#include "AEBrowse.h"
#include "AEBrowseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseApp

BEGIN_MESSAGE_MAP(CAEBrowseApp, CWinApp)
	//{{AFX_MSG_MAP(CAEBrowseApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseApp construction

CAEBrowseApp::CAEBrowseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAEBrowseApp object

CAEBrowseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseApp initialization

BOOL CAEBrowseApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	CAEBrowseDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
