#include "stdafx.h"
#include "AEEventTypes.h"
#include "AEEventTypesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesApp

BEGIN_MESSAGE_MAP(CAEEventTypesApp, CWinApp)
	//{{AFX_MSG_MAP(CAEEventTypesApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesApp construction

CAEEventTypesApp::CAEEventTypesApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAEEventTypesApp object

CAEEventTypesApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesApp initialization

BOOL CAEEventTypesApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	CAEEventTypesDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
