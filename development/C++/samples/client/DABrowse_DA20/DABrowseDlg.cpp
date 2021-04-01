#include "stdafx.h"

#include "DABrowse.h"
#include "DABrowseDlg.h"

#include "ClientEnums.h"
#include "ClientCommon.h"
#include "ClientApplication.h"
#include "ClientAddressSpaceElementBrowseOptions.h"
#include "ClientDaAddressSpaceElement.h"
#include "ClientDaGetPropertiesOptions.h"
#include "ClientDaProperty.h"
#include ".\dabrowsedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SoftingOPCToolboxClient;

/////////////////////////////////////////////////////////////////////////////
// CDABrowseDlg dialog

CDABrowseDlg::CDABrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDABrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDABrowseDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_session = NULL;
}

void CDABrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDABrowseDlg)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDABrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CDABrowseDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE1, OnDeleteitemTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnItemexpandingTree)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDABrowseDlg message handlers
CDABrowseDlg::~CDABrowseDlg()
{
	if (m_session != NULL)
	{
		if (m_session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			m_session->disconnect(NULL);
		}

		m_application->removeDaSession(m_session);
		delete m_session;
	}

	m_application->terminate();
	releaseApplication();
}


BOOL CDABrowseDlg::OnInitDialog()
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
	//result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	if (SUCCEEDED(m_application->initialize()))
	{
		m_session = new DaSession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
		m_session->connect(TRUE, FALSE, NULL);
		browseRoot();
	} //end if

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDABrowseDlg::OnPaint()
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

// checks whether a given DaAddressSpaceElement has children (leaves or branches) or
// vendor defined properties
BOOL CDABrowseDlg::isExpandable(DaAddressSpaceElement& element, BOOL forNodes)
{
	BOOL returnedResult = FALSE;

	if (forNodes)
	{
		DaAddressSpaceElementBrowseOptions someBrowseOptions;
		std::vector<DaAddressSpaceElement*>addressSpaceElements;
		std::vector<DaAddressSpaceElement*>::iterator addressSpaceelementsIterator;
		//  now browse for all elements
		someBrowseOptions.setElementTypeFilter(EnumAddressSpaceElementType_ALL);
		//  for some special DA2.0 servers the following browse selection must be done
		//  Rockwell OPC toolkit based servers
		//someBrowseOptions.setForceBrowseUp(true);
		// gets all the element's children
		element.browse(
			&someBrowseOptions,
			addressSpaceElements,
			NULL);

		// if the element has children set the returned result to TRUE
		if (addressSpaceElements.size() != 0)
		{
			returnedResult = TRUE;
		}

		for (addressSpaceelementsIterator = addressSpaceElements.begin(); addressSpaceelementsIterator != addressSpaceElements.end(); addressSpaceelementsIterator++)
		{
			delete(*addressSpaceelementsIterator);
		}

		// it is not necessary to go further because we already found that the element is expandable
		if (returnedResult)
		{
			return TRUE;
		}
	}   //  end if

	std::vector<DaProperty*>::iterator propertiesIterator;
	std::vector<DaProperty*>allProperties;
	DaGetPropertiesOptions propertiesOptions;
	propertiesOptions.setWhatPropertyData(EnumPropertyData_ALL);
	long result = element.getDaProperties(
					  &propertiesOptions,
					  allProperties,
					  NULL);
	size_t count = allProperties.size();
	size_t i = 0;

	while ((i < count) && (!returnedResult))
	{
		// only the vendor defined properties must be added as children of the current element
		// so only if the element has such properties it gets expandable
		if (allProperties[i]->getId() >= 100)
		{
			returnedResult = TRUE;
		}

		i++;
	}

	for (propertiesIterator = allProperties.begin(); propertiesIterator != allProperties.end(); propertiesIterator++)
	{
		delete(*propertiesIterator);
	}

	return returnedResult;
}

void CDABrowseDlg::browseRoot()
{
	m_tree.DeleteAllItems();
	tstring rootNodeText = m_session->getUrl();
	DaAddressSpaceElement* rootNode = new DaAddressSpaceElement(
		EnumAddressSpaceElementType_BRANCH,
		rootNodeText, _T(""), 0, _T(""), m_session);
	// Insert the root item.
	// The chlidren will be inserted 'on demand' when a node
	// gets expanded
	TVINSERTSTRUCT root;
	root.hParent = TVI_ROOT;
	root.hInsertAfter = TVI_FIRST;
	root.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	root.item.pszText = (LPTSTR)rootNodeText.c_str();
	root.item.lParam = (LPARAM)rootNode;
	root.item.cChildren = 1;
	root.item.iImage = 0;
	root.item.iSelectedImage = 0;
	HTREEITEM hRootNode = m_tree.InsertItem(&root);
	TV_INSERTSTRUCT tvis;
	tvis.hParent = hRootNode;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	// get the root's properties
	std::vector<DaProperty*>::iterator propertiesIterator;
	std::vector<DaProperty*>allProperties;
	DaGetPropertiesOptions propertiesOptions;
	propertiesOptions.setWhatPropertyData(EnumPropertyData_ALL);
	long result = rootNode->getDaProperties(
					  &propertiesOptions,
					  allProperties,
					  NULL);
	size_t count = allProperties.size();
	TV_INSERTSTRUCT property;
	property.hParent = hRootNode;
	property.hInsertAfter = TVI_LAST;
	property.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	for (unsigned long i = 0; i < count; i++)
	{
		if (allProperties[i]->getId() >= 100)
		{
			tstring name = allProperties[i]->getName();
			property.item.pszText = (LPTSTR)name.c_str() ;
			property.item.lParam = NULL;
			property.item.cChildren = 0;
			property.item.iImage = 3;
			property.item.iSelectedImage = 3;
			m_tree.InsertItem(&property);
		}
	}

	for (propertiesIterator = allProperties.begin(); propertiesIterator != allProperties.end(); propertiesIterator++)
	{
		delete(*propertiesIterator);
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDABrowseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDABrowseDlg::OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pTV = (NM_TREEVIEW*)pNMHDR;

	if (pTV->itemOld.lParam != NULL)
	{
		DaAddressSpaceElement* element = (DaAddressSpaceElement*)pTV->itemOld.lParam;
		delete element;
		pTV->itemOld.lParam = NULL;
	}

	*pResult = 0;
}

void CDABrowseDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pTV = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pTV->itemNew.hItem;
	DaAddressSpaceElement* addressSpaceElement = (DaAddressSpaceElement*)pTV->itemNew.lParam;
	DaAddressSpaceElementBrowseOptions someBrowseOptions;
	std::vector<DaAddressSpaceElement*>addressSpaceElements;
	std::vector<DaProperty*>::iterator propertiesIterator;
	unsigned long branchCount, leafCount, propertyCount, i;
	int cChildren = 0;

	// Check if this is the first time a node gets expanded.
	if (TVE_EXPAND != (pTV->action & TVE_EXPAND))
	{
		return;
	}

	if (TVIS_EXPANDEDONCE & (pTV->itemNew.state))
	{
		return;
	}

	TV_INSERTSTRUCT tvis;
	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	//  browse all the branches of the current node
	someBrowseOptions.setElementTypeFilter(EnumAddressSpaceElementType_BRANCH);
	//  for some special DA2.0 servers the following browse selection must be done
	//  Rockwell OPC toolkit based servers
	//someBrowseOptions.setForceBrowseUp(true);
	addressSpaceElement->browse(
		&someBrowseOptions,
		addressSpaceElements,
		NULL);
	branchCount = addressSpaceElements.size();

	for (i = 0; i < branchCount; i++)
	{
		tstring displayName = addressSpaceElements[i]->getName();
		displayName += _T(" - ");
		displayName += addressSpaceElements[i]->getItemId();
		tvis.item.pszText = (LPTSTR)displayName.c_str() ;
		tvis.item.lParam = (LPARAM)addressSpaceElements[i];
		tvis.item.cChildren = isExpandable(*addressSpaceElements[i], TRUE);
		tvis.item.iImage = 1;
		tvis.item.iSelectedImage = 1;
		m_tree.InsertItem(&tvis);
	} //end for

	addressSpaceElements.clear();
	//browse all the leaves of the current node
	someBrowseOptions.setElementTypeFilter(EnumAddressSpaceElementType_LEAF);
	//  for some special DA2.0 servers the following browse selection must be done
	//  Rockwell OPC toolkit based servers
	//someBrowseOptions.setForceBrowseUp(true);
	addressSpaceElement->browse(
		&someBrowseOptions,
		addressSpaceElements,
		NULL);
	leafCount = addressSpaceElements.size();

	for (i = 0; i < leafCount; i++)
	{
		tstring displayName = addressSpaceElements[i]->getName();
		displayName += _T(" - ");
		displayName += addressSpaceElements[i]->getItemId();
		tvis.item.pszText = (LPTSTR)displayName.c_str() ;
		tvis.item.lParam = (LPARAM)addressSpaceElements[i];
		tvis.item.cChildren = isExpandable(*addressSpaceElements[i], FALSE);
		tvis.item.iImage = 2;
		tvis.item.iSelectedImage = 2;
		m_tree.InsertItem(&tvis);
	} //end for

	addressSpaceElements.clear();
	//get all the properties of the current element
	std::vector<DaProperty*>allProperties;
	DaGetPropertiesOptions propertiesOptions;
	propertiesOptions.setWhatPropertyData(EnumPropertyData_ALL);
	long result = addressSpaceElement->getDaProperties(
					  &propertiesOptions,
					  allProperties,
					  NULL);
	propertyCount = allProperties.size();
	TV_INSERTSTRUCT property;
	property.hParent = hItem;
	property.hInsertAfter = TVI_LAST;
	property.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	for (i = 0; i < propertyCount; i++)
	{
		if (allProperties[i]->getId() >= 100)
		{
			tstring description = allProperties[i]->getDescription();
			property.item.pszText = (LPTSTR)description.c_str() ;
			property.item.lParam =  NULL;
			property.item.cChildren = 0;
			property.item.iImage = 3;
			property.item.iSelectedImage = 3;
			m_tree.InsertItem(&property);
		}
	}

	for (propertiesIterator = allProperties.begin(); propertiesIterator != allProperties.end(); propertiesIterator++)
	{
		delete(*propertiesIterator);
	}
}




