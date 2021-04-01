#include "stdafx.h"

#include "OpcEnumerate.h"
#include "OpcEnumerateDlg.h"

#include "ClientEnums.h"
#include "ClientApplication.h"
#include "ClientCommon.h"

#include ".\opcenumeratedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SoftingOPCToolboxClient;

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateDlg dialog

COpcEnumerateDlg::COpcEnumerateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpcEnumerateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_serverBrowser = new ServerBrowser(_T("localhost"));
}
COpcEnumerateDlg::~COpcEnumerateDlg()
{
	delete m_serverBrowser;

	for (m_browserDataIterator = m_browserData.begin(); m_browserDataIterator != m_browserData.end(); m_browserDataIterator++)
	{
		delete(*m_browserDataIterator);
	} //end for

	m_application->terminate();
	releaseApplication();
}

void COpcEnumerateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpcEnumerateDlg)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, urlEditBox);
}

BEGIN_MESSAGE_MAP(COpcEnumerateDlg, CDialog)
	//{{AFX_MSG_MAP(COpcEnumerateDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnItemexpandingTree)
	//}}AFX_MSG_MAP

//	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateDlg message handlers

BOOL COpcEnumerateDlg::OnInitDialog()
{
	long result = S_OK;
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon
	VERIFY(m_imgList.Create(IDB_BITMAP_TREE, 16, 0, RGB(255, 0, 255)));
	m_tree.SetImageList(&m_imgList, TVSIL_NORMAL);
	// Initialize the application
	m_application = getApplication();
	m_application->setVersionOtb(447);
	m_application->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Client feature
	// result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Client Feature
	// result = getApplication()->activate(EnumFeature_XMLDA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the COM-AE Client Feature
	// result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
	}

	//  END TODO - binary license activation

	if (SUCCEEDED(m_application->initialize()))
	{
		CreateMainTree();
	}   //  end if

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COpcEnumerateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
void COpcEnumerateDlg::CreateMainTree()
{
	m_tree.DeleteAllItems();
	// Create the main structure
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = NULL;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsert.item.pszText = (LPTSTR)_T("Local");
	tvInsert.item.lParam = (LPARAM) _T("Local");
	HTREEITEM hRootNode = m_tree.InsertItem(&tvInsert);
	TV_INSERTSTRUCT tvDAV1;
	tvDAV1.hParent = hRootNode;
	tvDAV1.hInsertAfter = TVI_LAST;
	tvDAV1.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvDAV1.item.pszText = (LPTSTR)_T("Data Access V1");
	tvDAV1.item.lParam = (LPARAM)(LPCSTR) _T("Data Access V1");
	tvDAV1.item.cChildren = 1;
	tvDAV1.item.iImage = 1;
	tvDAV1.item.iSelectedImage = 1;
	m_tree.InsertItem(&tvDAV1);
	TV_INSERTSTRUCT tvDAV2;
	tvDAV2.hParent = hRootNode;
	tvDAV2.hInsertAfter = TVI_LAST;
	tvDAV2.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvDAV2.item.pszText = (LPTSTR)_T("Data Access V2");
	tvDAV2.item.lParam  = (LPARAM)(LPCSTR)_T("Data Access V2");
	tvDAV2.item.cChildren = 1;
	tvDAV2.item.iImage = 1;
	tvDAV2.item.iSelectedImage = 1;
	m_tree.InsertItem(&tvDAV2);
	TV_INSERTSTRUCT tvDAV3;
	tvDAV3.hParent = hRootNode;
	tvDAV3.hInsertAfter = TVI_LAST;
	tvDAV3.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvDAV3.item.pszText = (LPTSTR)_T("Data Access V3");
	tvDAV3.item.lParam = (LPARAM)(LPCSTR)_T("Data Access V3");
	tvDAV3.item.cChildren = 1;
	tvDAV3.item.iImage = 1;
	tvDAV3.item.iSelectedImage = 1;
	m_tree.InsertItem(&tvDAV3);
	TV_INSERTSTRUCT tvDAE;
	tvDAE.hParent = hRootNode;
	tvDAE.hInsertAfter = TVI_LAST;
	tvDAE.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvDAE.item.pszText = (LPTSTR)_T("Alarms and Events");
	tvDAE.item.lParam = (LPARAM)(LPCSTR)_T("Alarms and Events");
	tvDAE.item.cChildren = 1;
	tvDAE.item.iImage = 1;
	tvDAE.item.iSelectedImage = 1;
	m_tree.InsertItem(&tvDAE);
}

void COpcEnumerateDlg::DeleteChildItems(HTREEITEM hItem)
{
	// Delete all items of the tree starting at hItem
	if (m_tree.ItemHasChildren(hItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_tree.GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			hNextItem = m_tree.GetNextItem(hChildItem, TVGN_NEXT);
			DeleteChildItems(hChildItem);
			m_tree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
}

void COpcEnumerateDlg::BrowseServers(EnumOPCSpecification specification , HTREEITEM hItem)
{
	// clear child items
	DeleteChildItems(hItem);
	std::vector<ServerBrowserData*> serverData;
	ExecutionOptions executionOptions;
	executionOptions.setExecutionContext(m_serverBrowser->getUserData());
	long result = m_serverBrowser->browse(
					  specification,
					  EnumServerBrowserData_ALL,
					  serverData,
					  NULL);

	if (SUCCEEDED(result))
	{
		size_t count = serverData.size();

		for (unsigned long i = 0; i < count; i++)
		{
			TV_INSERTSTRUCT tvDescription;
			tvDescription.hParent = hItem;
			tvDescription.hInsertAfter = TVI_LAST;
			tvDescription.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvDescription.item.pszText = (LPTSTR)(LPCTSTR) serverData[i]->getDescription().c_str();
			tvDescription.item.lParam = (LPARAM)(LPCTSTR) serverData[i]->getUrl().c_str();
			tvDescription.item.cChildren = 1;
			tvDescription.item.iImage = 2;
			tvDescription.item.iSelectedImage = 2;
			HTREEITEM hDescriptionNode = m_tree.InsertItem(&tvDescription);
			TV_INSERTSTRUCT tvClsId;
			tvClsId.hParent = hDescriptionNode;
			tvClsId.hInsertAfter = TVI_LAST;
			tvClsId.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvClsId.item.pszText = (LPTSTR)(LPCTSTR) serverData[i]->getClsId().c_str();
			tvClsId.item.lParam = (LPARAM)(LPCTSTR) serverData[i]->getUrl().c_str();
			tvClsId.item.cChildren = 0;
			tvClsId.item.iImage = 3;
			tvClsId.item.iSelectedImage = 3;
			m_tree.InsertItem(&tvClsId);
			TV_INSERTSTRUCT tvProgId;
			tvProgId.hParent = hDescriptionNode;
			tvProgId.hInsertAfter = TVI_LAST;
			tvProgId.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvProgId.item.pszText = (LPTSTR)(LPCTSTR) serverData[i]->getProgId().c_str();
			tvProgId.item.lParam = (LPARAM)(LPCTSTR)serverData[i]->getUrl().c_str();
			tvProgId.item.cChildren = 0;
			tvProgId.item.iImage = 3;
			tvProgId.item.iSelectedImage = 3;
			m_tree.InsertItem(&tvProgId);
			TV_INSERTSTRUCT tvProgIdVersionIndependent;
			tvProgIdVersionIndependent.hParent = hDescriptionNode;
			tvProgIdVersionIndependent.hInsertAfter = TVI_LAST;
			tvProgIdVersionIndependent.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvProgIdVersionIndependent.item.pszText = (LPTSTR)(LPCTSTR) serverData[i]->getProgIdVersionIndependent().c_str();
			tvProgIdVersionIndependent.item.lParam = (LPARAM)(LPCTSTR)serverData[i]->getUrl().c_str();
			tvProgIdVersionIndependent.item.cChildren = 0;
			tvProgIdVersionIndependent.item.iImage = 3;
			tvProgIdVersionIndependent.item.iSelectedImage = 3;
			m_tree.InsertItem(&tvProgIdVersionIndependent);
			m_browserData.push_back(serverData[i]);
		} //end for
	} //end if
}//end BrowseServers

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COpcEnumerateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void COpcEnumerateDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	this->urlEditBox.SetWindowText(_T(""));
	NMTREEVIEW* pTV = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pTV->itemNew.hItem;
	bool writeUrl = true;
	tstring  tvName = (LPCTSTR)pTV->itemNew.lParam;

	if (tvName == _T("Data Access V1"))
	{
		BrowseServers(EnumOPCSpecification_DA10, hItem);
		writeUrl = false;
	} //end if

	if (tvName == _T("Data Access V2"))
	{
		BrowseServers(EnumOPCSpecification_DA20, hItem);
		writeUrl = false;
	} //end if

	if (tvName == _T("Data Access V3"))
	{
		BrowseServers(EnumOPCSpecification_DA30, hItem);
		writeUrl = false;
	} //end if

	if (tvName == _T("Alarms and Events"))
	{
		BrowseServers(EnumOPCSpecification_AE10, hItem);
		writeUrl = false;
	} //end if

	writeUrl = writeUrl && (tvName != _T("Local"));

	if (writeUrl == true)
	{
		LPCTSTR url = (LPCTSTR)(pTV->itemNew.lParam);
		this->urlEditBox.SetWindowText(url);
	} //end if
}
