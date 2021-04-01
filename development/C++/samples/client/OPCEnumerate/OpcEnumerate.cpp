#include "stdafx.h"
#include "OpcEnumerate.h"
#include "OpcEnumerateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateApp

BEGIN_MESSAGE_MAP(COpcEnumerateApp, CWinApp)
	//{{AFX_MSG_MAP(COpcEnumerateApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateApp construction

COpcEnumerateApp::COpcEnumerateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COpcEnumerateApp object

COpcEnumerateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateApp initialization

BOOL COpcEnumerateApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	COpcEnumerateDlg  dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
