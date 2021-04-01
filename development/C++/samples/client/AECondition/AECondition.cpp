#include "stdafx.h"
#include "AECondition.h"
#include "AEConditionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAEConditionApp

BEGIN_MESSAGE_MAP(CAEConditionApp, CWinApp)
	//{{AFX_MSG_MAP(CAEConditionApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEConditionApp construction

CAEConditionApp::CAEConditionApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAEConditionApp object

CAEConditionApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAEConditionApp initialization

BOOL CAEConditionApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	CAEConditionDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
