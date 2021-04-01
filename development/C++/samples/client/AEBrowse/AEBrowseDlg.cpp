#include "stdafx.h"

#include "AEBrowse.h"
#include "AEBrowseDlg.h"

#include "ClientEnums.h"
#include "ClientApplication.h"
#include "ClientCommon.h"

#include "ClientAddressSpaceElementBrowseOptions.h"
#include "ClientAddressSpaceElement.h"
#include "ClientAeCategory.h"
#include "ClientAeAttribute.h"
#include ".\aebrowsedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SoftingOPCToolboxClient;

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseDlg dialog

CAEBrowseDlg::CAEBrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAEBrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAEBrowseDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_session = NULL;
}

void CAEBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAEBrowseDlg)
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAEBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CAEBrowseDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnItemexpandingTree)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE1, OnDeleteitemTree)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseDlg message handlers

CAEBrowseDlg::~CAEBrowseDlg()
{
	if (m_session != NULL)
	{
		if (m_session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			m_session->disconnect(NULL);
		}

		m_application->removeAeSession(m_session);
		delete m_session;
	}

	m_application->terminate();
	releaseApplication();
}

BOOL CAEBrowseDlg::OnInitDialog()
{
	long result = S_OK;
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon
	VERIFY(m_imgList.Create(IDB_BITMAP_TREE, 16, 0, RGB(255, 0, 255)));
	m_tree.SetImageList(&m_imgList, TVSIL_NORMAL);
	m_tab.InsertItem(0, _T("Area Space"));
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

		if (m_session->getValid())
		{
			m_session->connect(TRUE, FALSE, NULL);
		}

		browseAreaSpace();
	} //end if

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAEBrowseDlg::OnPaint()
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
HCURSOR CAEBrowseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAEBrowseDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pTV = (NMTREEVIEW*)pNMHDR;

	// Check if this is the first time a node gets expanded.
	if (TVE_EXPAND != (pTV->action & TVE_EXPAND))
	{
		return;
	}

	if (TVIS_EXPANDEDONCE & (pTV->itemNew.state))
	{
		return;
	}

	//browse the server's area space
	*pResult = 0;
	HTREEITEM hItem = pTV->itemNew.hItem;
	AddressSpaceElement* addressSpaceElement = (AddressSpaceElement*)pTV->itemNew.lParam;
	AddressSpaceElementBrowseOptions* someBrowseOptions = new AddressSpaceElementBrowseOptions();
	std::vector<AddressSpaceElement*>addressSpaceElements;
	std::vector<tstring> conditions;
	size_t count, i, j;
	TV_INSERTSTRUCT tvis;
	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	//browse all the branches of the current node
	someBrowseOptions->setElementTypeFilter(EnumAddressSpaceElementType_BRANCH);

	//synchronously browse the current node's branches
	if (SUCCEEDED(m_session->browse(
					  addressSpaceElement,
					  someBrowseOptions,
					  addressSpaceElements,
					  NULL)))
	{
		count = addressSpaceElements.size();

		for (i = 0; i < count; i++)
		{
			tstring name = addressSpaceElements[i]->getName();
			tvis.item.pszText = (LPTSTR)(LPCTSTR) name.c_str() ;
			tvis.item.lParam = (LPARAM)addressSpaceElements[i];
			tvis.item.cChildren = 1;
			tvis.item.iImage = 1;
			tvis.item.iSelectedImage = 1;
			HTREEITEM newItem = m_tree.InsertItem(&tvis);
			//query existing conditions
			tstring sourcePath = addressSpaceElements[i]->getQualifiedName();

			// synchronously get source conditions
			if (SUCCEEDED(m_session->queryAeSourceConditions(
							  sourcePath,
							  conditions,
							  NULL)))
			{
				for (j = 0; j < conditions.size(); j++)
				{
					TV_INSERTSTRUCT conditionTvis;
					conditionTvis.hParent = newItem;
					conditionTvis.hInsertAfter = TVI_LAST;
					conditionTvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					conditionTvis.item.pszText = (LPTSTR)(LPCTSTR) conditions[j].c_str();
					conditionTvis.item.lParam = NULL;
					conditionTvis.item.cChildren = 0;
					conditionTvis.item.iImage = 2;
					conditionTvis.item.iSelectedImage = 2;
					m_tree.InsertItem(&conditionTvis);
				}   //  end for
			}   //  end if
		}   //  end for
	}   //  end if

	//browse all the leaves of the current node
	someBrowseOptions->setElementTypeFilter(EnumAddressSpaceElementType_LEAF);

	if (SUCCEEDED(m_session->browse(
					  addressSpaceElement,
					  someBrowseOptions,
					  addressSpaceElements,
					  NULL)))
	{
		count = addressSpaceElements.size();

		for (i = 0; i < count; i++)
		{
			tstring name = addressSpaceElements[i]->getName();
			tvis.item.pszText = (LPTSTR)(LPCTSTR) name.c_str() ;
			tvis.item.lParam = (LPARAM)addressSpaceElements[i];
			tvis.item.cChildren = 1;
			tvis.item.iImage = 2;
			tvis.item.iSelectedImage = 2;
			HTREEITEM newItem = m_tree.InsertItem(&tvis);
			//query existing conditions
			tstring sourcePath = addressSpaceElements[i]->getQualifiedName();

			// synchronously get source conditions
			if (SUCCEEDED(m_session->queryAeSourceConditions(
							  sourcePath,
							  conditions,
							  NULL)))
			{
				if (conditions.size() == 0)
				{
					TVITEM item;
					item.hItem = newItem;
					item.mask = TVIF_CHILDREN;
					item.cChildren = 0;
					m_tree.SetItem(&item);
				}   //  end if

				for (j = 0; j < conditions.size(); j++)
				{
					TV_INSERTSTRUCT conditionTvis;
					conditionTvis.hParent = newItem;
					conditionTvis.hInsertAfter = TVI_LAST;
					conditionTvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					conditionTvis.item.pszText = (LPTSTR)(LPCTSTR) conditions[j].c_str();
					conditionTvis.item.lParam = NULL;
					conditionTvis.item.cChildren = 0;
					conditionTvis.item.iImage = 2;
					conditionTvis.item.iSelectedImage = 2;
					m_tree.InsertItem(&conditionTvis);
				}   //  end for
			}   //  end if
		} //end for

		addressSpaceElements.clear();
	}   //  end if

	delete someBrowseOptions;
}

void CAEBrowseDlg::OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pTV = (NM_TREEVIEW*)pNMHDR;
	delete(AddressSpaceElement*)pTV->itemOld.lParam;
	*pResult = 0;
}

void CAEBrowseDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_tree.DeleteAllItems();
	browseAreaSpace();
	*pResult = 0;
}



void CAEBrowseDlg::browseAreaSpace()
{
	m_tree.DeleteAllItems();
	// Insert the root item.
	// The children will be inserted 'on demand' when a node
	// gets expanded
	tstring rootNodeText = m_session->getUrl();
	AddressSpaceElement* rootNode = new AddressSpaceElement(
		EnumAddressSpaceElementType_BRANCH,
		rootNodeText, _T(""), 0);
	TVINSERTSTRUCT tvis;
	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = TVI_FIRST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.pszText = (LPTSTR)(LPCTSTR) rootNodeText.c_str();
	tvis.item.lParam = (LPARAM)rootNode;
	tvis.item.cChildren = 1;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	HTREEITEM hRootNode = m_tree.InsertItem(&tvis);
}

