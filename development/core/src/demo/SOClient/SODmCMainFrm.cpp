//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : SODmCMainFrm.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : main window classes                                         |
//                - SODmCMainFrame: MFC main frame class                      |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "SOClient.h"
#include "SODmCMainFrm.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"
#include "SODmCPropFrm.h"
#include "SODmCItem.h"
#if _MSC_VER > 1200
#include "DlgAbout.h"
#include ".\sodmcmainfrm.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-----------------------------------------------------------------------------
// SODmCMainFrame                                                             |
//-----------------------------------------------------------------------------

// toolkit notitfy message
static UINT s_messageCallbackNotify = ::RegisterWindowMessage(_T("DISPATCHCALLBACKS"));

IMPLEMENT_DYNAMIC(SODmCMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(SODmCMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_SESSION_START, onStart)
	ON_COMMAND(ID_SESSION_STOP, onStop)
	ON_COMMAND(ID_SESSION_CONNECT, onConnect)
	ON_REGISTERED_MESSAGE(SOCltEntry::s_messageCallbackNotify, onCallbackNofiy)
	ON_COMMAND(IDC_DAITEMWRITE, onWriteDAItem)
	ON_UPDATE_COMMAND_UI(IDC_DAITEMWRITE, onUpdWriteDAItem)
	ON_COMMAND(ID_VIEW_DAWRITE_BAR, onViewDAWriteBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DAWRITE_BAR, onUpdViewDAWriteBar)
	ON_MESSAGE(OWNMSG_HIT_ENTER, onEnter)
	ON_COMMAND(ID_EDIT_OPTIONS, onEditOptions)
	ON_COMMAND(ID_HELP_ABOUT, onAbout)
	ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT, onUpdAbout)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,      // status line indicator
};

SODmCMainFrame::SODmCMainFrame()
{
	m_pPropFrame = NULL;
	m_propFrameMinimized = FALSE;
//>>>SJ 04.09.2008 help added
	TCHAR szBasepath[_MAX_PATH + sizeof(TCHAR)];
	LPTSTR  pszBase;
	CString sBaseName, sHelpFile;

	if (::GetModuleFileName(AfxGetInstanceHandle(), szBasepath, _MAX_PATH) <= 0)
	{
		return; // insufficient buffer
	}

	pszBase = _tcsrchr(szBasepath, _T('\\'));

	if (pszBase != NULL)
	{
		*pszBase = _T('\0');
	}

	sBaseName = szBasepath;
	sHelpFile = sBaseName  + _T("\\SOClientG.chm");
	// get help file name from the registry
	HKEY keyInstallation = NULL;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Client"),
									  NULL, KEY_READ, &keyInstallation))
	{
		DWORD sizeBuffer = _MAX_PATH;
		szBasepath[0] = _T('\0');
		RegQueryValueEx(keyInstallation, _T("InstDir"), NULL, NULL, (LPBYTE)szBasepath, &sizeBuffer);

		if ((sizeBuffer != 0) && (_tcslen(szBasepath) != 0))
		{
			pszBase = _tcsrchr(szBasepath, _T('\\'));

			if (pszBase != NULL)
			{
				*pszBase = _T('\0');
			}

			sBaseName = szBasepath;
		}

		TCHAR sBuffer[_MAX_PATH + sizeof(TCHAR)];
		sizeBuffer = _MAX_PATH;
		sBuffer[0] = _T('\0');
		RegQueryValueEx(keyInstallation, _T("Language"), NULL, NULL, (LPBYTE)sBuffer, &sizeBuffer);

		if ((sizeBuffer != 0) && (_tcslen(sBuffer) != 0))
		{
			if (_tcsnicmp(sBuffer, _T("de"), 2) == 0)
			{
				sizeBuffer = _MAX_PATH;
				TCHAR sLanguage[_MAX_PATH + sizeof(TCHAR)];
				sLanguage[0] = _T('\0');
				RegQueryValueEx(keyInstallation, _T("Help_de"), NULL, NULL, (LPBYTE)sLanguage, &sizeBuffer);

				if ((sizeBuffer != 0) && (_tcslen(sLanguage) != 0))
				{
					sHelpFile = sBaseName  + _T('\\') + sLanguage;
				}
			}
			else
			{
				sizeBuffer = _MAX_PATH;
				TCHAR sLanguage[_MAX_PATH + sizeof(TCHAR)];
				sLanguage[0] = _T('\0');
				RegQueryValueEx(keyInstallation, _T("Help_en"), NULL, NULL, (LPBYTE)sLanguage, &sizeBuffer);

				if ((sizeBuffer != 0) && (_tcslen(sLanguage) != 0))
				{
					sHelpFile = sBaseName  + _T('\\') + sLanguage;
				}
			}
		}

		RegCloseKey(keyInstallation);
	}

	//else
	//{
	//  int ret = ::GetModuleFileName(AfxGetInstanceHandle(), szBasepath, _MAX_PATH);
	//  if (ret <= 0)
	//  {
	//      return; // insufficient buffer
	//  }
	//  pszBase = _tcsrchr(szBasepath, _T('\\'));
	//  if (pszBase != NULL)
	//  {
	//      *pszBase = _T('\0');
	//  }
	//  sBaseName = szBasepath;
	//  sHelpFile = sBaseName  + _T("\\SOClientG.chm");
	//}

	// set help file path
	if (AfxGetApp()->m_pszHelpFilePath != NULL)
	{
		free((void*) AfxGetApp()->m_pszHelpFilePath);
	}

	AfxGetApp()->m_pszHelpFilePath = _tcsdup(sHelpFile);
//<<<SJ 04.09.2008 help added
}

SODmCMainFrame::~SODmCMainFrame()
{
}


//-----------------------------------------------------------------------------
// OnClose
//
// - close the property frame
//
void SODmCMainFrame::OnClose()
{
	if (IDCANCEL == getApp()->doSaveChanges())
	{
		return;
	}

	if (m_pPropFrame)
	{
		m_pPropFrame->DestroyWindow();
		m_pPropFrame = NULL;
	}

	CFrameWnd::OnClose();
} // OnClose


//-----------------------------------------------------------------------------
// OnCreate
//
// - initialize the main frame
//
// returns:
//      0  - all ok
//      -1 - error
//
int SODmCMainFrame::OnCreate(
	IN LPCREATESTRUCT lpCreateStruct)   // create struct
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (getComCtlVersion() >= COMCTL32_VERSION_IE4)
	{
		if (!m_wndToolBar.CreateEx(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			return -1;    // fail to create
		}

		if (!m_wndDAWriteBar.Create(this, IDR_DAWRITE,
									CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
		{
			return -1;    // fail to create
		}

		m_wndDAWriteBar.EnableToolTips();

		if (!m_wndReBar.Create(this) ||
			!m_wndReBar.AddBar(&m_wndToolBar) ||
			!m_wndReBar.AddBar(&m_wndDAWriteBar))
		{
			return -1;    // fail to create
		}
	}
	else
	{
		EnableDocking(CBRS_ALIGN_ANY);

		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			return -1;    // fail to create
		}

		m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
		DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

		if (!m_wndDAWriteBar.Create(this, IDR_DAWRITE,
									CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
		{
			return -1;    // fail to create
		}

		m_wndDAWriteBar.EnableDocking(CBRS_ALIGN_TOP);
		m_wndDAWriteBar.EnableToolTips();
		DockControlBar(&m_wndDAWriteBar, AFX_IDW_DOCKBAR_TOP);
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
									  sizeof(indicators) / sizeof(UINT)))
	{
		return -1;    // fail to create
	}

	m_wndDAWriteBar.ShowWindow(SW_HIDE);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
							 CBRS_TOOLTIPS | CBRS_FLYBY);
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);   // Set big icon
	SetIcon(hIcon, FALSE);  // Set small icon
	// DA write dialog bar
	HWND hWndCtrl;
	m_wndDAWriteBar.GetDlgItem(IDC_DAITEMWRITE, &hWndCtrl);
	m_writeDAItemButton.SubclassWindow(hWndCtrl);
	m_wndDAWriteBar.GetDlgItem(IDC_DAITEMVALUE, &hWndCtrl);
	m_writeDAItemValue.SubclassWindow(hWndCtrl);
	m_writeDAItemValue.setNotificationWnd(this);
	onViewDAWriteBar();
	return 0;
} // OnCreate


//-----------------------------------------------------------------------------
// PreCreateWindow
//
// - called during window creation
//
// returns:
//      return of base class
//
BOOL SODmCMainFrame::PreCreateWindow(IN CREATESTRUCT& cs)
{
	cs.cx = 800;
	cs.cy = 600;

	if (!CFrameWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
} // PreCreateWindow


//-----------------------------------------------------------------------------
// OnCreateClient
//
// - create splitter windows
//
// returns:
//      return of base class
//
BOOL SODmCMainFrame::OnCreateClient(IN LPCREATESTRUCT lpcs, IN CCreateContext* pContext)
{
	// calculate sizes
	CSize szLeft(205, 500);
	CSize szRight(585, 500);

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		return FALSE;
	}

	m_wndSplitter.SetColumnInfo(0, 205, 10);
	m_wndSplitter.SetColumnInfo(1, 585, 10);

	// add the first splitter pane
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(SODmCObjectView), szLeft, pContext))
	{
		return FALSE;
	}

	// add the second splitter pane
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(SODmCMainView), szRight, pContext))
	{
		return FALSE;
	}

	// activate the object view
	SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));
	SODmCPropertyFrame* pPropFrame = createPropertyFrame();
	pPropFrame->ShowWindow(SW_HIDE);
	return CFrameWnd::OnCreateClient(lpcs, pContext);
} // OnCreateClient


//-----------------------------------------------------------------------------
// OnActivate
//
// - window activation/deactivation
//
afx_msg void SODmCMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if (m_pPropFrame)
	{
		if ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE))
		{
			// bring property frame below window if visible
			if ((m_pPropFrame->IsWindowVisible()) || (m_propFrameMinimized))
			{
				m_propFrameMinimized = FALSE;
				m_pPropFrame->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			}
		}
		else if ((nState == WA_INACTIVE) && (bMinimized))
		{
			// bring property frame below window if visible
			if (m_pPropFrame->IsWindowVisible())
			{
				m_pPropFrame->ShowWindow(SW_HIDE);
				m_propFrameMinimized = TRUE;
			}
		}
	}
} // OnActivate


//-----------------------------------------------------------------------------
// createPropertyFrame
//
// - create property frame window
//
// returns:
//      created property frame window
//
SODmCPropertyFrame* SODmCMainFrame::createPropertyFrame()
{
	if (m_pPropFrame == NULL)
	{
		m_pPropFrame = new SODmCPropertyFrame;
		CRect rect(0, 0, 0, 0);

		if (!m_pPropFrame->Create(NULL, _T("Softing OPC Classic Demo Client - Properties"),
								  WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, NULL))
		{
			delete m_pPropFrame;
			m_pPropFrame = NULL;
		}
		else
		{
			m_pPropFrame->CenterWindow();
		}
	}

	return m_pPropFrame;
} // createPropertyFrame


//-----------------------------------------------------------------------------
// showPropertyFrame
//
// - show the property frame window
//
void SODmCMainFrame::showPropertyFrame()
{
	if (m_pPropFrame)
	{
		m_pPropFrame->ShowWindow(SW_SHOW);
		m_pPropFrame->BringWindowToTop();
	}
} // showPropertyFrame


//-----------------------------------------------------------------------------
// onStart
//
// - Handler for Session/Start menu and toolbar button
//
void SODmCMainFrame::onStart(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
//	entryDA->start(TRUE, SOCLT_EXEC_ASYNC);
	BOOL ret = entryDA->start(TRUE);

	if (ret == FALSE)
	{
		ret = TRUE;
	}

#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->start(TRUE, SOCLT_EXEC_ASYNC);
#endif
} // onStart


//-----------------------------------------------------------------------------
// onConnect
//
// - Handler for Session/Connect menu and toolbar button
//
void SODmCMainFrame::onConnect(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	entryDA->connect(TRUE, SOCLT_EXEC_ASYNC);
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->connect(TRUE, SOCLT_EXEC_ASYNC);
#endif
} // onConnect


//-----------------------------------------------------------------------------
// onStop
//
// - Handler for Session/Stop menu and toolbar button
//
void SODmCMainFrame::onStop(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	entryDA->disconnect(SOCLT_EXEC_ASYNC);
	entryDA->connect(FALSE, SOCLT_EXEC_ASYNC);
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->disconnect(SOCLT_EXEC_ASYNC);
	entryAE->connect(FALSE, SOCLT_EXEC_ASYNC);
#endif
} // onStop


//-----------------------------------------------------------------------------
// onEnter
//
// - Handler for Hit Enter message
//
afx_msg LONG SODmCMainFrame::onEnter(IN UINT wParam, IN LONG lParam)
{
#ifdef SODMC_DA

	if (IDC_DAITEMVALUE == wParam)
	{
		onWriteDAItem();
	}

#endif
	return 0;
} // onEnter


//-----------------------------------------------------------------------------
// onViewDAWriteBar
//
// - menu handler of menu view/DA Write Bar
//
void SODmCMainFrame::onViewDAWriteBar(void)
{
#ifdef SODMC_DA
	// toggle visible state
	m_wndDAWriteBar.ShowWindow((m_wndDAWriteBar.GetStyle() & WS_VISIBLE) == 0);
	RecalcLayout();
#endif
} // onViewDAWriteBar


//-----------------------------------------------------------------------------
// onUpdViewDAWriteBar
//
// - update menu handler of menu view/DA Write Bar
//
void SODmCMainFrame::onUpdViewDAWriteBar(
	OUT CCmdUI* pCmdUI)     // command UI
{
#ifdef SODMC_DA
	pCmdUI->SetCheck(m_wndDAWriteBar.GetStyle() & WS_VISIBLE);
#else
	pCmdUI->Enable(FALSE);
#endif
} // onUpdViewDAWriteBar


//-----------------------------------------------------------------------------
// updateDAWriteBar
//
// -  update DA Write Bar
//
void SODmCMainFrame::updateDAWriteBar(void)
{
#ifdef SODMC_DA
	// toggle visible state
	m_wndDAWriteBar.ShowWindow((m_wndDAWriteBar.GetStyle() & WS_VISIBLE) != 0);
	RecalcLayout();
#endif
} // onViewDAWriteBar


//-----------------------------------------------------------------------------
// onWriteDAItem
//
// - handler of OK button of DA Write Bar
//
void SODmCMainFrame::onWriteDAItem()
{
#ifdef SODMC_DA
	SODmCObjectView* objview = getObjectView();

	if (objview)
	{
		// get the item to write to
		SOCmnPointer<SODmCItem> item = objview->getSelectedItem();

		if (item.isNotNull())
		{
			CString cString;

			if (m_wndDAWriteBar.GetDlgItemText(IDC_DAITEMVALUE, cString))
			{
				item->writeStringValue(cString, VT_EMPTY);
			}
		}
	}

#endif
} // onWriteDAItem


//-----------------------------------------------------------------------------
// onUpdWriteDAItem
//
// - Enable menu if object is selected
//
void SODmCMainFrame::onUpdWriteDAItem(
	OUT CCmdUI* pCmdUI)     // command UI
{
#ifdef SODMC_DA
	SODmCObjectView* objview = getObjectView();

	if (objview)
	{
		SOCmnPointer<SODmCItem> item = objview->getSelectedItem();

		if (item.isNotNull())
		{
			if (item->getAccessRights() & OPC_WRITEABLE)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdWriteDAItem


//-----------------------------------------------------------------------------
// onCallbackNofiy
//
// - notifiy message of the toolkit
//
LRESULT SODmCMainFrame::onCallbackNofiy(WPARAM, LPARAM)
{
	// dispatch all callbacks
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	entryDA->dispatchCallbacks();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->dispatchCallbacks();
#endif
	return 0;
} // onCallbackNofiy


//-----------------------------------------------------------------------------
// onEditOptions
//
// - Handler for Edit/Options menu
//
void SODmCMainFrame::onEditOptions()
{
	SODmCDlgOptions dlg;        // options dialog
	dlg.DoModal();
} // onEditOptions

afx_msg void SODmCMainFrame::onAbout(void)
{
#if _MSC_VER > 1200
	CDlgAbout dlgAbout;
	dlgAbout.DoModal();
#endif
}

afx_msg void SODmCMainFrame::onUpdAbout(OUT CCmdUI* pCmdUI)
{
#if _MSC_VER > 1200
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
}


//-----------------------------------------------------------------------------
// SODmCDlgOptions                                                               |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCDlgOptions, CDialog)
	ON_BN_CLICKED(IDC_OPT_TIME_ONLY, onTimeOnly)
	ON_BN_CLICKED(IDC_OPT_TIME_DATE, onTimeDate)
	ON_BN_CLICKED(IDC_OPT_VALUE_PURE, onValuePure)
	ON_BN_CLICKED(IDC_OPT_VALUE_VT, onValueVT)
	ON_BN_CLICKED(IDC_OPT_VALUE_DEC, onValueDec)
	ON_BN_CLICKED(IDC_OPT_VALUE_HEX, onValueHex)
	ON_BN_CLICKED(IDC_OPT_CM_ON, onCmOn)
	ON_BN_CLICKED(IDC_OPT_CM_OFF, onCmOff)
	ON_BN_CLICKED(IDC_OPT_IO_ERRORS_ON, onIoErrorsOn)
	ON_BN_CLICKED(IDC_OPT_IO_ERRORS_OFF, onIoErrorsOff)
	ON_BN_CLICKED(IDC_OPT_TRC_ON, onTrcOn)
	ON_BN_CLICKED(IDC_OPT_TRC_OFF, onTrcOff)
	ON_BN_CLICKED(IDC_OPT_ALPHAITEM_ON, onAlphaItmOn)
	ON_BN_CLICKED(IDC_OPT_ALPHAITEM_OFF, onAlphaItmOff)
	ON_BN_CLICKED(IDC_OPT_ALPHAAE_ON, onAlphaAEOn)
	ON_BN_CLICKED(IDC_OPT_ALPHAAE_OFF, onAlphaAEOff)
END_MESSAGE_MAP()

SODmCDlgOptions::SODmCDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(SODmCDlgOptions::IDD, pParent)
{
}

BOOL SODmCDlgOptions::OnInitDialog(void)
{
	BOOL ret = CDialog::OnInitDialog();
	SODmCApp* app = getApp();
	CButton* itemNames = NULL;
	CButton* time = NULL;
	CButton* date = NULL;
	CButton* errors = NULL;
	CButton* value = NULL;

	if (app->getOptions()->timeOnly)
	{
		time = (CButton*)GetDlgItem(IDC_OPT_TIME_ONLY);
	}
	else
	{
		time = (CButton*)GetDlgItem(IDC_OPT_TIME_DATE);
	}

	if (time)
	{
		time->SetCheck(TRUE);
	}

	if (app->getOptions()->valuePure)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_VALUE_PURE);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_VALUE_VT);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->valueDec)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_VALUE_DEC);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_VALUE_HEX);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->cmOn)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_CM_ON);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_CM_OFF);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->viewIOErrors)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_IO_ERRORS_ON);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_IO_ERRORS_OFF);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->trcOn)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_TRC_ON);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_TRC_OFF);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->alphaItemOn)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_ALPHAITEM_ON);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_ALPHAITEM_OFF);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	if (app->getOptions()->alphaAEOn)
	{
		value = (CButton*)GetDlgItem(IDC_OPT_ALPHAAE_ON);
	}
	else
	{
		value = (CButton*)GetDlgItem(IDC_OPT_ALPHAAE_OFF);
	}

	if (value)
	{
		value->SetCheck(TRUE);
	}

	return ret;
}

void SODmCDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void SODmCDlgOptions::onTimeOnly()
{
	SODmCApp* app = getApp();
	app->getOptions()->timeOnly = TRUE;
}

void SODmCDlgOptions::onTimeDate()
{
	SODmCApp* app = getApp();
	app->getOptions()->timeOnly = FALSE;
}

void SODmCDlgOptions::onValuePure()
{
	SODmCApp* app = getApp();
	app->getOptions()->valuePure = TRUE;
}

void SODmCDlgOptions::onValueVT()
{
	SODmCApp* app = getApp();
	app->getOptions()->valuePure = FALSE;
}

void SODmCDlgOptions::onValueDec()
{
	SODmCApp* app = getApp();
	app->getOptions()->valueDec = TRUE;
}

void SODmCDlgOptions::onValueHex()
{
	SODmCApp* app = getApp();
	app->getOptions()->valueDec = FALSE;
}

void SODmCDlgOptions::onCmOn()
{
	SODmCApp* app = getApp();
	app->getOptions()->cmOn = TRUE;
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
#endif
	SOCmnList<SOCltServer> serverList;
	SOCmnListPosition pos;
	SOCltServer* pServer;
#ifdef SODMC_ALL

	for (DWORD i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			serverList = (SOCmnList<SOCltServer> *)entryDA->getBranchList();
		}
		else
		{
			serverList = (SOCmnList<SOCltServer> *)entryAE->getBranchList();
		}

#else
#ifdef SODMC_DA
	{
		serverList = (SOCmnList<SOCltServer> *)entryDA->getBranchList();
#else
	{
		serverList = (SOCmnList<SOCltServer> *)entryAE->getBranchList();
#endif
#endif
		pos = serverList.getStartPosition();

		while (pos)
		{
			pServer = serverList.getNext(pos);
			pServer->addConnectionMonitor(5000, 10, 10000, 300000);
		}
	}
}

void SODmCDlgOptions::onCmOff()
{
	SODmCApp* app = getApp();
	app->getOptions()->cmOn = FALSE;
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
#endif
	SOCmnList<SOCltServer> serverList;
	SOCmnListPosition pos;
	SOCltServer* pServer;
#ifdef SODMC_ALL

	for (DWORD i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			serverList = (SOCmnList<SOCltServer> *)entryDA->getBranchList();
		}
		else
		{
			serverList = (SOCmnList<SOCltServer> *)entryAE->getBranchList();
		}

#else
#ifdef SODMC_DA
	{
		serverList = (SOCmnList<SOCltServer> *)entryDA->getBranchList();
#else
	{
		serverList = (SOCmnList<SOCltServer> *)entryAE->getBranchList();
#endif
#endif
		pos = serverList.getStartPosition();

		while (pos)
		{
			pServer = serverList.getNext(pos);
			pServer->removeConnectionMonitor();
		}
	}
}

void SODmCDlgOptions::onIoErrorsOn()
{
	SODmCApp* app = getApp();
	app->getOptions()->viewIOErrors = TRUE;
}

void SODmCDlgOptions::onIoErrorsOff()
{
	SODmCApp* app = getApp();
	app->getOptions()->viewIOErrors = FALSE;
}

void SODmCDlgOptions::onTrcOn()
{
	SODmCApp* app = getApp();
	app->getOptions()->trcOn = TRUE;
	app->enableTrace(app->getOptions()->trcOn);
}


void SODmCDlgOptions::onTrcOff()
{
	SODmCApp* app = getApp();
	app->getOptions()->trcOn = FALSE;
	app->enableTrace(app->getOptions()->trcOn);
}

void SODmCDlgOptions::onAlphaItmOn()
{
	SODmCApp* app = getApp();
	app->getOptions()->alphaItemOn = TRUE;
}


void SODmCDlgOptions::onAlphaItmOff()
{
	SODmCApp* app = getApp();
	app->getOptions()->alphaItemOn = FALSE;
}

void SODmCDlgOptions::onAlphaAEOn()
{
	SODmCApp* app = getApp();
	app->getOptions()->alphaAEOn = TRUE;
}


void SODmCDlgOptions::onAlphaAEOff()
{
	SODmCApp* app = getApp();
	app->getOptions()->alphaAEOn = FALSE;
}


