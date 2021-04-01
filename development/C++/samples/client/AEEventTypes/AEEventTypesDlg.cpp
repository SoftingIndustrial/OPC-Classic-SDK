#include "stdafx.h"

#include "AEEventTypes.h"
#include "AEEventTypesDlg.h"

#include "ClientEnums.h"
#include "ClientCommon.h"
#include "ClientApplication.h"

#include "ClientAddressSpaceElementBrowseOptions.h"
#include "ClientAddressSpaceElement.h"
#include "AE\ClientAeCategory.h"
#include "AE\ClientAeAttribute.h"
#include ".\AEEventTypesdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SoftingOPCToolboxClient;

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesDlg dialog

CAEEventTypesDlg::CAEEventTypesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAEEventTypesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAEEventTypesDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAEEventTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAEEventTypesDlg)
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT2, m_availableFiltersEditBox);
}

BEGIN_MESSAGE_MAP(CAEEventTypesDlg, CDialog)
	//{{AFX_MSG_MAP(CAEEventTypesDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesDlg message handlers

CAEEventTypesDlg::~CAEEventTypesDlg()
{
	if (m_session->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		m_session->disconnect(NULL);
	}

	m_application->removeAeSession(m_session);
	m_application->terminate();
	releaseApplication();
	delete m_session;
}
BOOL CAEEventTypesDlg::OnInitDialog()
{
	long result = S_OK;
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon
	VERIFY(m_imgList.Create(IDB_BITMAP_TREE, 16, 0, RGB(255, 0, 255)));
	m_tree.SetImageList(&m_imgList, TVSIL_NORMAL);
	m_tab.InsertItem(0, _T("Event Space"));
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

	//  activate the COM-AE Client Feature
	//result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
	}

	//  END TODO - binary license activation

	//Initialize the application
	if (SUCCEEDED(m_application->initialize()))
	{
		m_session = new AeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
		m_session->connect(TRUE, FALSE, NULL);
		BrowseEventSpace();
	} // end if

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAEEventTypesDlg::OnPaint()
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAEEventTypesDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CAEEventTypesDlg::BrowseEventSpace()
{
	m_tree.DeleteAllItems();
	m_availableFiltersEditBox.SetWindowText(_T(""));
	// Insert the root item.
	// The chlidren will be inserted 'on demand' when a node
	// gets expanded
	unsigned long i, j;
	std::vector<AeCategory*> categories;
	std::vector<AeCategory*>::iterator categoriesIterator;
	std::vector<AeAttribute*> attributes;
	std::vector<AeAttribute*>::iterator attributesIterator;
	std::vector<tstring> conditionNames;
	std::vector<tstring> subConditionNames;
	tstring rootNodeText = m_session->getUrl();
	TVINSERTSTRUCT tvisSession;
	tvisSession.hParent = TVI_ROOT;
	tvisSession.hInsertAfter = TVI_FIRST;
	tvisSession.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvisSession.item.pszText = (LPTSTR)(LPCTSTR) rootNodeText.c_str();
	tvisSession.item.cChildren = 1;
	tvisSession.item.iImage = 0;
	tvisSession.item.iSelectedImage = 0;
	HTREEITEM hRootNode = m_tree.InsertItem(&tvisSession);
	m_session->queryAeCategories(
		categories,
		NULL);

	for (i = 0; i < categories.size(); i++)
	{
		TVINSERTSTRUCT tvisCategory;
		tvisCategory.hParent = hRootNode;
		tvisCategory.hInsertAfter = TVI_FIRST;
		tvisCategory.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvisCategory.item.pszText = (LPTSTR)(LPCTSTR) categories[i]->getDescription().c_str();
		tvisCategory.item.cChildren = 1;
		tvisCategory.item.iImage = 1;
		tvisCategory.item.iSelectedImage = 1;
		HTREEITEM categoryNode = m_tree.InsertItem(&tvisCategory);
		categories[i]->queryAeAttributes(
			attributes,
			NULL);

		for (j = 0; j < attributes.size(); j++)
		{
			TVINSERTSTRUCT tvisAttribute;
			tvisAttribute.hParent = categoryNode;
			tvisAttribute.hInsertAfter = TVI_FIRST;
			tvisAttribute.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvisAttribute.item.pszText = (LPTSTR)(LPCTSTR) attributes[j]->getDescription().c_str();
			tvisAttribute.item.cChildren = 0;
			tvisAttribute.item.iImage = 2;
			tvisAttribute.item.iSelectedImage = 2;
			HTREEITEM attributeNode = m_tree.InsertItem(&tvisAttribute);
		} //end for

		for (attributesIterator = attributes.begin(); attributesIterator != attributes.end(); attributesIterator++)
		{
			delete *attributesIterator;
		} //end for

		conditionNames.clear();
		categories[i]->queryAeConditionNames(
			conditionNames,
			NULL);

		for (j = 0; j < conditionNames.size(); j++)
		{
			TVINSERTSTRUCT tvisConditionName;
			tvisConditionName.hParent = categoryNode;
			tvisConditionName.hInsertAfter = TVI_FIRST;
			tvisConditionName.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvisConditionName.item.pszText = (LPTSTR)(LPCTSTR) conditionNames[j].c_str();
			tvisConditionName.item.cChildren = 0;
			tvisConditionName.item.iImage = 3;
			tvisConditionName.item.iSelectedImage = 3;
			HTREEITEM attributeNode = m_tree.InsertItem(&tvisConditionName);
		} //end for
	} //end for

	for (categoriesIterator = categories.begin(); categoriesIterator != categories.end(); categoriesIterator++)
	{
		delete *categoriesIterator;
	} //end for

	EnumFilterBy availableFilters;
	m_session->queryAvailableAeFilters(
		availableFilters,
		NULL);
	tstring filters;

	if ((availableFilters & EnumFilterBy_AREA) == EnumFilterBy_AREA)
	{
		filters += _T("Filter by Area \n");
	} //end if

	if ((availableFilters & EnumFilterBy_CATEGORY) == EnumFilterBy_CATEGORY)
	{
		filters += _T("Filter by Category \n");
	} //end if

	if ((availableFilters & EnumFilterBy_EVENT) == EnumFilterBy_EVENT)
	{
		filters += _T("Filter by Event \n");
	} //end if

	if ((availableFilters & EnumFilterBy_SEVERITY) == EnumFilterBy_SEVERITY)
	{
		filters += _T("Filter by Severity \n");
	} //end if

	if ((availableFilters & EnumFilterBy_SOURCE) == EnumFilterBy_SOURCE)
	{
		filters += _T("Filter by Source \n");
	} //end if

	m_availableFiltersEditBox.SetWindowText(filters.c_str());
} //end BrowseEventSpace

