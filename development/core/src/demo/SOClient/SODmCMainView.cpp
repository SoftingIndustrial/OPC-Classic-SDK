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
//  Filename    : SODmCMainView.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : classes of top right splitter window (main view)            |
//                - SODmCMainView: main view class                            |
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
#include "SODmCMainView.h"
#include "SODmCMainFrm.h"
#include "SODmCPropFrm.h"
#include "SODmCServerDA.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCServerAE.h"
#include "SODmCSubscription.h"
#include "SODmCEvent.h"
#include "SODmCMultiWriteDlg.h"
#include ".\sodmcmainview.h"

static const unsigned char UTF_LEAD_0 = 0xefU;
static const unsigned char UTF_LEAD_1 = 0xbbU;
static const unsigned char UTF_LEAD_2 = 0xbfU;
//-----------------------------------------------------------------------------
// Callbacks                                                                  |
//-----------------------------------------------------------------------------

int compareName(SOCltBrowseObject* obj1, SOCltBrowseObject* obj2)
{
	SOCmnString name1(obj1->getName());
	SOCmnString name2(obj2->getName());
	return _tcscmp(name1, name2);
}
int __stdcall compareNames(const void* obj1, const void* obj2)
{
	return compareName((SOCltBrowseObject*) obj1, (SOCltBrowseObject*) obj2);
}

#ifdef SODMC_DA
int comparePropName(SODaCProperty* obj1, SODaCProperty* obj2)
{
	SOCmnString name1(obj1->getName());
	SOCmnString name2(obj2->getName());
	return _tcscmp(name1, name2);
}
int __stdcall comparePropNames(const void* obj1, const void* obj2)
{
	return compareName((SODaCProperty*) obj1, (SODaCProperty*) obj2);
}
#endif

//-----------------------------------------------------------------------------
// Global SODmCMainView access function                                       |
//-----------------------------------------------------------------------------

static SODmCMainView* s_mainView = NULL;

//-----------------------------------------------------------------------------
// getMainView
//
// - get main view object
//
// returns:
//      main view object
//
SODmCMainView* getMainView(void)
{
	return s_mainView;
} // getMainView




//-----------------------------------------------------------------------------
// SODmCMainView                                                              |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(SODmCMainView, CFormView)

BEGIN_MESSAGE_MAP(SODmCMainView, CFormView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, OnTabSC)
#ifdef SODMC_DCOM
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_MAIN_TREE_SRV, OnTreeSrvIE)
	ON_NOTIFY(TVN_DELETEITEM, IDC_MAIN_TREE_SRV, OnTreeSrvDI)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MAIN_TREE_SRV, OnTreeSrvCL)
	ON_NOTIFY(NM_DBLCLK, IDC_MAIN_TREE_SRV, OnTreeSrvDC)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_MAIN_TREE_SRV, OnTreeSrvBLE)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_MAIN_TREE_SRV, OnTreeSrvELE)
#endif // SODMC_DCOM
#ifdef SODMC_DA
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_MAIN_TREE_DA, OnTreeDaIE)
	ON_NOTIFY(TVN_DELETEITEM, IDC_MAIN_TREE_DA, OnTreeDaDI)
	ON_NOTIFY(NM_DBLCLK, IDC_MAIN_TREE_DA, OnTreeDaDC)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAIN_LIST_DA, OnListDaIC)
#endif
#ifdef SODMC_AE
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_MAIN_TREE_AE, OnTreeAeIE)
	ON_NOTIFY(TVN_DELETEITEM, IDC_MAIN_TREE_AE, OnTreeAeDI)
	ON_NOTIFY(LVN_DELETEITEM, IDC_MAIN_LIST_AE, OnListAeDI)
	ON_NOTIFY(LVN_DELETEITEM, IDC_MAIN_LIST_AEC, OnListAecDI)
	ON_NOTIFY(NM_DBLCLK, IDC_MAIN_LIST_AEC, OnListAecDC)
#endif
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_TAB, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_TREE_SRV, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_TREE_DA, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_LIST_DA, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_LIST_AE, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_LIST_AEC, OnRBD)
	ON_NOTIFY(NM_RCLICK, IDC_MAIN_LIST_ERR, OnRBD)
	ON_BN_CLICKED(IDC_BUTTON_ADDSERVER, onButtonAddServer)
	ON_COMMAND(ID_VIEW_REFRESH, onRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, onUpdRefresh)
	ON_COMMAND(ID_DELETEOBJECT, onDeleteObject)
	ON_UPDATE_COMMAND_UI(ID_DELETEOBJECT, onUpdDeleteObject)
	ON_COMMAND(ID_EDIT_ADDITEM, onAddItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDITEM, onUpdAddItem)
	ON_COMMAND(ID_EDIT_ADDSERVER, onAddServer)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSERVER, onUpdAddServer)
	ON_COMMAND(ID_ADDTAGSOFNODE, onAddTagsOfNode)
	ON_UPDATE_COMMAND_UI(ID_ADDTAGSOFNODE, onUpdAddTagsOfNode)
	ON_COMMAND(ID_BROWSEALLTAGSOFNODE2FILE, onBrowseAllTagsOfNode2File)
	ON_UPDATE_COMMAND_UI(ID_BROWSEALLTAGSOFNODE2FILE, onUpdBrowseAllTagsOfNode2File)

	ON_COMMAND(ID_ADDNEWGROUPWITHALLTAGS, onAddNewGroupWithAllTags)
	ON_UPDATE_COMMAND_UI(ID_ADDNEWGROUPWITHALLTAGS, onUpdAddNewGroupWithAllTags)

	ON_COMMAND(ID_WRITEFROMFILE, onWriteFromFile)
	ON_UPDATE_COMMAND_UI(ID_WRITEFROMFILE, onUpdWriteFromFile)
	ON_COMMAND(ID_READTOFILE, onReadToFile)
	ON_UPDATE_COMMAND_UI(ID_READTOFILE, onUpdReadToFile)
	ON_COMMAND(ID_EDIT_PROPERTIES, onProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, onUpdProperties)
	ON_COMMAND(ID_START_FLAT, onStartObjFlat)
	ON_COMMAND(ID_CONNECT_FLAT, onConnectObjFlat)
	ON_COMMAND(ID_STOP, onStopObj)
	ON_UPDATE_COMMAND_UI(ID_START_FLAT, onUpdStartObjFlat)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_FLAT, onUpdConnectObjFlat)
	ON_UPDATE_COMMAND_UI(ID_STOP, onUpdStopObj)
	ON_COMMAND(ID_ITEM_READ, onReadItem)
	ON_UPDATE_COMMAND_UI(ID_ITEM_READ, onUpdReadItem)
	ON_COMMAND(ID_ITEM_WRITE, onWriteItem)
	ON_UPDATE_COMMAND_UI(ID_ITEM_WRITE, onUpdWriteItem)
	ON_COMMAND(ID_EVENT_ACK, onAcknowledge)
	ON_UPDATE_COMMAND_UI(ID_EVENT_ACK, onUpdAcknowledge)
	ON_COMMAND(ID_ERROR_DELETEALL, onErrDeleteAll)
	ON_UPDATE_COMMAND_UI(ID_ERROR_DELETEALL, onUpdErrDeleteAll)
	ON_MESSAGE(OWNMSG_HIT_ENTER, onEnter)
	ON_UPDATE_COMMAND_UI(ID_LOGTOFILE, OnUpdLogToFile)
	ON_COMMAND(ID_LOGTOFILE, OnLogToFile)
END_MESSAGE_MAP()

SODmCMainView::SODmCMainView()
	: CFormView(IDD_MAINVIEW_FORM)
{
	s_mainView = this;
	m_images.Create(IDB_MAIN_IMAGE, 16, 0, RGB(255, 0, 255));
	m_firstInit = TRUE;
	// set the CLSID of the serial INAT server
	// {3DA28331-68CB-11D2-9C65-0021A0020009}
	m_serialINATServerClsId.Data1 = 0x3DA28331;
	m_serialINATServerClsId.Data2 = 0x68CB;
	m_serialINATServerClsId.Data3 = 0x11D2;
	m_serialINATServerClsId.Data4[0] = 0x9C;
	m_serialINATServerClsId.Data4[1] = 0x65;
	m_serialINATServerClsId.Data4[2] = 0x00;
	m_serialINATServerClsId.Data4[3] = 0x21;
	m_serialINATServerClsId.Data4[4] = 0xA0;
	m_serialINATServerClsId.Data4[5] = 0x02;
	m_serialINATServerClsId.Data4[6] = 0x00;
	m_serialINATServerClsId.Data4[7] = 0x09;
	// set the CLSID of the Ethernet INAT server
	// {3DA28330-68CB-11D2-9C65-0021A0020009}
	m_ethernetINATServerClsId.Data1 = 0x3DA28330;
	m_ethernetINATServerClsId.Data2 = 0x68CB;
	m_ethernetINATServerClsId.Data3 = 0x11D2;
	m_ethernetINATServerClsId.Data4[0] = 0x9C;
	m_ethernetINATServerClsId.Data4[1] = 0x65;
	m_ethernetINATServerClsId.Data4[2] = 0x00;
	m_ethernetINATServerClsId.Data4[3] = 0x21;
	m_ethernetINATServerClsId.Data4[4] = 0xA0;
	m_ethernetINATServerClsId.Data4[5] = 0x02;
	m_ethernetINATServerClsId.Data4[6] = 0x00;
	m_ethernetINATServerClsId.Data4[7] = 0x09;
}

SODmCMainView::~SODmCMainView()
{
#ifdef SODMC_DA
	m_treeDAServer.release();
#endif
#ifdef SODMC_AE
	m_treeAESubscription.release();
#endif
}

//-----------------------------------------------------------------------------
// OnInitialUpdate
//
// - initialize the view
//
void SODmCMainView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if (m_firstInit)
	{
		m_firstInit = FALSE;
		// tab control
		m_tab.InsertItem(SODMCMAINVIEW_TAB_SRV, _T("OPC Servers"), SODMCMAINVIEW_TAB_SRV);
#ifdef SODMC_DA
		m_tab.InsertItem(SODMCMAINVIEW_TAB_TDA, _T("DA Browse"), SODMCMAINVIEW_TAB_TDA);
		m_tab.InsertItem(SODMCMAINVIEW_TAB_LDA, _T("DA Items"), SODMCMAINVIEW_TAB_LDA);
#endif
#ifdef SODMC_AE
		m_tab.InsertItem(SODMCMAINVIEW_TAB_LAE, _T("AE Browse"), SODMCMAINVIEW_TAB_TAE);
		m_tab.InsertItem(SODMCMAINVIEW_TAB_LAE, _T("AE Events"), SODMCMAINVIEW_TAB_LAE);
		m_tab.InsertItem(SODMCMAINVIEW_TAB_LAEC, _T("AE Conditions"), SODMCMAINVIEW_TAB_LAEC);
#endif
		m_tab.InsertItem(SODMCMAINVIEW_TAB_ERR, _T("Errors"), SODMCMAINVIEW_TAB_ERR);
		// image lists
		m_treeSrv.SetImageList(&m_images, TVSIL_NORMAL);
		m_treeDA.SetImageList(&m_images, TVSIL_NORMAL);
		m_treeAE.SetImageList(&m_images, TVSIL_NORMAL);
		m_listDA.SetImageList(&m_images, LVSIL_SMALL);
		m_listDA.ModifyStyle(LVS_SINGLESEL, 0);
		m_listAE.SetImageList(&m_images, LVSIL_SMALL);
		m_listAEC.SetImageList(&m_images, LVSIL_SMALL);
		m_listErr.SetImageList(&m_images, LVSIL_SMALL);
#ifdef SODMC_DCOM
		// installed  OPC servers
		SOCltServerBrowser* sb;
		HTREEITEM local, manual, manualMachine;
		sb = new SODmCServerBrowser();
		sb->setNodeName(_T(""));
		local = SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Local"), TRUE, (void*)(SOCltServerBrowser*)sb, SODMCMAINVIEW_IMG_MACHINE, SODMCMAINVIEW_IMG_MACHINE, TVI_LAST);
#ifdef SODMC_DA
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V1"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA10A, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V2"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA20, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V3"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA30, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#ifdef SODMC_AE
		SODmCTreeInsert(&m_treeSrv, local, _T("Alarms and Events"), TRUE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
		SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Remote"), TRUE, NULL, SODMCMAINVIEW_IMG_NETWORK, SODMCMAINVIEW_IMG_NETWORK, TVI_LAST);
		manual = SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Manual"), TRUE, NULL, SODMCMAINVIEW_IMG_MANUAL, SODMCMAINVIEW_IMG_MANUAL, TVI_LAST);
		sb = new SODmCServerBrowser();
		sb->setNodeName(_T(""));
		manualMachine = SODmCTreeInsert(&m_treeSrv, manual, _T("<Computer name or address>"), TRUE, (void*)(SOCltServerBrowser*)sb, SODMCMAINVIEW_IMG_MACHINE_MANUAL, SODMCMAINVIEW_IMG_MACHINE_MANUAL, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("<CLSID of the OPC Data Access Server>"), FALSE, (void*)(SOCLT_BROWSE_SERVER_DA20 | SOCLT_BROWSE_SERVER_DA10A), SODMCMAINVIEW_IMG_CLSID_MANUAL, SODMCMAINVIEW_IMG_CLSID_MANUAL, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("<CLSID of the OPC Alarms and Events Server>"), FALSE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_CLSID_MANUAL, SODMCMAINVIEW_IMG_CLSID_MANUAL, TVI_LAST);
#ifdef SODMC_DA
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V1"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA10A, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V2"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA20, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V3"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA30, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#ifdef SODMC_AE
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Alarms and Events"), TRUE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#endif // SODMC_DCOM
#ifdef SODMC_DA
		// DA item list
		m_listDA.InsertColumn(0, _T("Item"), LVCFMT_LEFT, 135);
		m_listDA.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 80);
		m_listDA.InsertColumn(2, _T("Quality"), LVCFMT_LEFT, 80);
		m_listDA.InsertColumn(3, _T("TimeStamp"), LVCFMT_LEFT, 80);
		m_listDA.InsertColumn(4, _T("Result"), LVCFMT_LEFT, 80);
		m_listDA.InsertColumn(5, _T("Server"), LVCFMT_LEFT, 75);
		m_listDA.InsertColumn(6, _T("Group"), LVCFMT_LEFT, 75);
#endif
#ifdef SODMC_AE
		// AE event list
		m_listAE.InsertColumn(0, _T("Source"), LVCFMT_LEFT, 135);
		m_listAE.InsertColumn(1, _T("Severity"), LVCFMT_LEFT, 55);
		m_listAE.InsertColumn(2, _T("Message"), LVCFMT_LEFT, 80);
		m_listAE.InsertColumn(3, _T("Time"), LVCFMT_LEFT, 80);
		m_listAE.InsertColumn(4, _T("Actor ID"), LVCFMT_LEFT, 80);
		m_listAE.InsertColumn(5, _T("Server"), LVCFMT_LEFT, 75);
		m_listAE.InsertColumn(6, _T("Subscription"), LVCFMT_LEFT, 75);
		// AE condition list
		m_listAEC.InsertColumn(0, _T("Source"), LVCFMT_LEFT, 135);
		m_listAEC.InsertColumn(1, _T("Condition"), LVCFMT_LEFT, 80);
		m_listAEC.InsertColumn(2, _T("Severity"), LVCFMT_LEFT, 55);
		m_listAEC.InsertColumn(3, _T("Message"), LVCFMT_LEFT, 80);
		m_listAEC.InsertColumn(4, _T("Time"), LVCFMT_LEFT, 80);
		m_listAEC.InsertColumn(5, _T("Actor ID"), LVCFMT_LEFT, 80);
		m_listAEC.InsertColumn(6, _T("Subcondition"), LVCFMT_LEFT, 80);
		m_listAEC.InsertColumn(7, _T("Server"), LVCFMT_LEFT, 75);
		m_listAEC.InsertColumn(8, _T("Subscription"), LVCFMT_LEFT, 75);
#endif
		// error list
		m_listErr.InsertColumn(0, _T("Object"), LVCFMT_LEFT, 130);
		m_listErr.InsertColumn(1, _T("Time"), LVCFMT_LEFT, 80);
		m_listErr.InsertColumn(2, _T("Errortext"), LVCFMT_LEFT, 180);
		m_listErr.InsertColumn(3, _T("Result"), LVCFMT_LEFT, 80);
		m_listErr.InsertColumn(4, _T("Resultext"), LVCFMT_LEFT, 180);
		m_listErr.InsertColumn(5, _T("Error"), LVCFMT_LEFT, 80);
		// hide all
		m_listDA.ShowWindow(SW_HIDE);
		m_listAE.ShowWindow(SW_HIDE);
		m_listErr.ShowWindow(SW_HIDE);
		m_treeSrv.ShowWindow(SW_HIDE);
		m_lbURLSrv.ShowWindow(SW_HIDE);
		m_edURLSrv.ShowWindow(SW_HIDE);
		m_buAddSrv.ShowWindow(SW_HIDE);
		m_treeDA.ShowWindow(SW_HIDE);
		m_treeAE.ShowWindow(SW_HIDE);
		// select servers page
		m_tab.SetCurSel(0);
		OnTabSC(NULL, NULL);
	}
} // OnInitialUpdate


//-----------------------------------------------------------------------------
// OnDestroy
//
// - clear item list if list view is destroyed
//
afx_msg void SODmCMainView::OnDestroy(void)
{
	m_treeSrv.DeleteAllItems();
	s_mainView = NULL;
} // OnDestroy


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - exchange form data
//
void SODmCMainView::DoDataExchange(IN CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_tab);
	DDX_Control(pDX, IDC_MAIN_LIST_DA, m_listDA);
	DDX_Control(pDX, IDC_MAIN_LIST_AE, m_listAE);
	DDX_Control(pDX, IDC_MAIN_LIST_AEC, m_listAEC);
	DDX_Control(pDX, IDC_MAIN_TREE_SRV, m_treeSrv);
	DDX_Control(pDX, IDC_MAIN_TREE_DA, m_treeDA);
	DDX_Control(pDX, IDC_MAIN_TREE_AE, m_treeAE);
	DDX_Control(pDX, IDC_MAIN_LIST_ERR, m_listErr);
	DDX_Control(pDX, IDC_LABEL_URL, m_lbURLSrv);
	DDX_Control(pDX, IDC_EDIT_URL, m_edURLSrv);
	DDX_Control(pDX, IDC_BUTTON_ADDSERVER, m_buAddSrv);
	DDX_Text(pDX, IDC_EDIT_URL, m_szURLSrv);
} // DoDataExchange


//-----------------------------------------------------------------------------
// OnSize
//
// - adapt all control sizes
//
void SODmCMainView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (m_tab.m_hWnd)
	{
		m_tab.MoveWindow(0, 0, cx, cy);
	}

	if (m_listDA.m_hWnd)
	{
		m_listDA.MoveWindow(3, 3, cx - 6, cy - 28);
	}

	if (m_listAE.m_hWnd)
	{
		m_listAE.MoveWindow(3, 3, cx - 6, cy - 28);
	}

	if (m_listAEC.m_hWnd)
	{
		m_listAEC.MoveWindow(3, 3, cx - 6, cy - 28);
	}

	if (m_listErr.m_hWnd)
	{
		m_listErr.MoveWindow(3, 3, cx - 6, cy - 28);
	}

	if (m_treeSrv.m_hWnd)
	{
		m_treeSrv.MoveWindow(3, 3, cx - 6, cy - 70);
		m_lbURLSrv.MoveWindow(3, cy - 52, 30, 20);
		m_edURLSrv.MoveWindow(35, cy - 55, cx - 130, 20);
		m_buAddSrv.MoveWindow(cx - 80, cy - 55, 70, 20);
	}

	if (m_treeDA.m_hWnd)
	{
		m_treeDA.MoveWindow(3, 3, cx - 6, cy - 28);
	}

	if (m_treeAE.m_hWnd)
	{
		m_treeAE.MoveWindow(3, 3, cx - 6, cy - 28);
	}
} // OnSize


//-----------------------------------------------------------------------------
// OnTabSC
//
// - selected tab changed
//
void SODmCMainView::OnTabSC(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_listDA.ShowWindow(SW_HIDE);
	m_listAE.ShowWindow(SW_HIDE);
	m_listAEC.ShowWindow(SW_HIDE);
	m_listErr.ShowWindow(SW_HIDE);
	m_treeSrv.ShowWindow(SW_HIDE);
	m_lbURLSrv.ShowWindow(SW_HIDE);
	m_edURLSrv.ShowWindow(SW_HIDE);
	m_buAddSrv.ShowWindow(SW_HIDE);
	m_treeDA.ShowWindow(SW_HIDE);
	m_treeAE.ShowWindow(SW_HIDE);

	switch (m_tab.GetCurSel())
	{
	case SODMCMAINVIEW_TAB_SRV: // Installed OPC Servers
#ifdef SODMC_DCOM
		m_treeSrv.ShowWindow(SW_SHOW);
#endif // SODMC_DCOM
		m_lbURLSrv.ShowWindow(SW_SHOW);
		m_edURLSrv.ShowWindow(SW_SHOW);
		m_buAddSrv.ShowWindow(SW_SHOW);
		break;
#ifdef SODMC_DA

	case SODMCMAINVIEW_TAB_TDA: // DA Browse
	{
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SODaCServer> curSelSrv = objview->getSelectedServerDA();;

		if ((SODaCServer*)m_treeDAServer != (SODaCServer*)curSelSrv)
		{
			m_treeDA.DeleteAllItems();
			m_treeDAServer = curSelSrv;

			if (m_treeDAServer.isNotNull())
			{
				SODmCCmnServerDA* pCmnSrv = NULL;
#ifdef SODMC_DCOM

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
#ifdef SODMC_SOAP

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_SOAP)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCTPServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
				SODaCNameSpaceBrowser* browser = m_treeDAServer->getNameSpaceBrowser();
				SOCmnString stringFilter, vendorFilter;
				stringFilter = pCmnSrv->getStringFilter();
				vendorFilter = pCmnSrv->getVendorFilter();

				if ((pCmnSrv->getDatatypeFilter() != VT_EMPTY) || (pCmnSrv->getAccessRightsFilter() != 0) || ((!stringFilter.isEmpty()) && (m_treeDAServer->getSupportedOPCVersion() < SODAC_VERSION_OPC_DAV30)))
				{
					browser->setFilter(stringFilter, pCmnSrv->getAccessRightsFilter(), pCmnSrv->getDatatypeFilter());
				}

				if (((!stringFilter.isEmpty()) && (m_treeDAServer->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)) || (!vendorFilter.isEmpty()))
				{
					browser->setFilter(stringFilter, vendorFilter);
				}

				SODmCTreeInsert(&m_treeDA, TVI_ROOT, objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()), TRUE, (void*)(SODaCNameSpaceBrowser*)browser, SODMCMAINVIEW_IMG_SRV_DA, SODMCMAINVIEW_IMG_SRV_DA, TVI_FIRST);
			}
		}

		if (m_treeDAServer.isNotNull())
		{
			m_treeDA.ShowWindow(SW_SHOW);
		}
	}
	break;

	case SODMCMAINVIEW_TAB_LDA: // DA Items
	{
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SODmCItem> curSelItem = objview->getSelectedItem();;
		changeSelectedItem(curSelItem);
		m_listDA.ShowWindow(SW_SHOW);
	}
	break;
#endif
#ifdef SODMC_AE

	case SODMCMAINVIEW_TAB_TAE: // AE Browse
	{
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SOAeCSubscription> curSelSub = objview->getSelectedSubscription();;

		if ((SOAeCSubscription*)m_treeAESubscription != (SOAeCSubscription*)curSelSub)
		{
			m_treeAE.DeleteAllItems();
			m_treeAESubscription = curSelSub;

			if (m_treeAESubscription.isNotNull())
			{
				SOCmnPointer<SOAeCServer> treeAEServer = (SOAeCServer*)m_treeAESubscription->getParentBranch();
				SODmCCmnServerAE* pCmnSrv = NULL;
#ifdef SODMC_DCOM

				if (treeAEServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)(SOAeCServer*)treeAEServer;
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (treeAEServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)(SOAeCServer*)treeAEServer;
				}

#endif
				SOAeCEventBrowser* eBrowser = treeAEServer->getEventBrowser();
				SOAeCAreaBrowser* aBrowser = treeAEServer->getAreaBrowser();
				aBrowser->setFilter(pCmnSrv->getStringFilter());
				SOCmnString brName;
				brName.format(_T("%s - Event space"), objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()));
				SODmCTreeInsert(&m_treeAE, TVI_ROOT, brName, TRUE, (void*)(SOAeCEventBrowser*)eBrowser, SODMCMAINVIEW_IMG_SRV_AE, SODMCMAINVIEW_IMG_SRV_AE, TVI_FIRST);
				brName.format(_T("%s - Area space"), objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()));
				SODmCTreeInsert(&m_treeAE, TVI_ROOT, brName, TRUE, (void*)(SOAeCAreaBrowser*)aBrowser, SODMCMAINVIEW_IMG_SRV_AE, SODMCMAINVIEW_IMG_SRV_AE, TVI_FIRST);
			}
		}

		if (m_treeAESubscription.isNotNull())
		{
			m_treeAE.ShowWindow(SW_SHOW);
		}
	}
	break;

	case SODMCMAINVIEW_TAB_LAE: // AE Events
		m_listAE.ShowWindow(SW_SHOW);
		break;

	case SODMCMAINVIEW_TAB_LAEC: // AE Conditions
		m_listAEC.ShowWindow(SW_SHOW);
		break;
#endif

	case SODMCMAINVIEW_TAB_ERR: // Errors
		m_listErr.ShowWindow(SW_SHOW);
		break;
	}

	if (pResult)
	{
		*pResult = 0;
	}
} // OnTabSC


//-----------------------------------------------------------------------------
// OnRBD
//
// - popup menu handler
//
void SODmCMainView::OnRBD(
	IN NMHDR* pNMHDR,       // parameter
	OUT LRESULT* pResult)   // result
{
	CMenu bar;
	GetParentFrame()->ActivateFrame();
	CPoint point;
	GetCursorPos(&point);

	switch (m_tab.GetCurSel())
	{
#ifdef SODMC_DCOM

	case SODMCMAINVIEW_TAB_SRV: // Installed OPC Servers
	{
		HTREEITEM hItem = m_treeSrv.GetSelectedItem();;

		if (hItem)
		{
			int img, selImg;
			m_treeSrv.GetItemImage(hItem, img, selImg);

			if ((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS))
			{
				bar.LoadMenu(IDR_OBJECT_OPCSERVER_POPUP);
			}
		}
	}
	break;
#endif // SODMC_DCOM
#ifdef SODMC_DA

	case SODMCMAINVIEW_TAB_TDA: // DA Browse
	{
		// DA Browse
		HTREEITEM hItem = m_treeDA.GetSelectedItem();

		if (hItem)
		{
			SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);

			if (bobj)
			{
				switch (bobj->getObjectType())
				{
				case SOCMNOBJECT_TYPE_NAMESPACE_BROWSER:
					bar.LoadMenu(IDR_OBJECT_NAMESPACEBROWSER_POPUP);
					break;

				case SOCMNOBJECT_TYPE_NODE:
					bar.LoadMenu(IDR_OBJECT_NODE_POPUP);
					break;

				case SOCMNOBJECT_TYPE_TAG:
					bar.LoadMenu(IDR_OBJECT_TAG_POPUP);
					break;

				case SOCMNOBJECT_TYPE_PROPERTY:
					bar.LoadMenu(IDR_OBJECT_PROPERTY_POPUP);
					break;
				}
			}
		}
	}
	break;

	case SODMCMAINVIEW_TAB_LDA: // DA Items
	{
		SOCmnPointer<SODmCItem> item = getSelectedItem();

		if (item.isNotNull())
		{
			bar.LoadMenu(IDR_OBJECT_ITEM_POPUP);
		}
	}
	break;
#endif
#ifdef SODMC_AE

	case SODMCMAINVIEW_TAB_LAE: // AE Events
		break;

	case SODMCMAINVIEW_TAB_LAEC: // AE Conditions
	{
		SOCmnPointer<SODmCEvent> cond = getSelectedCondition();

		if (cond.isNotNull())
		{
			bar.LoadMenu(IDR_OBJECT_EVENTCOND_POPUP);
		}
	}
	break;
#endif

	case SODMCMAINVIEW_TAB_ERR: // Errors
	{
		bar.LoadMenu(IDR_ERRORLST_POPUP);
	}
	break;
	}

	if (bar.m_hMenu != 0)
	{
		CMenu& popup = *bar.GetSubMenu(0);  // get sub menu
		// view menu
		popup.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
} // OnRBD


//-----------------------------------------------------------------------------
// onRefresh
//
// - menu handler of menu view/refresh
//
afx_msg void SODmCMainView::onRefresh(void)
{
	switch (m_tab.GetCurSel())
	{
#ifdef SODMC_DCOM

	case SODMCMAINVIEW_TAB_SRV: // Installed OPC Servers
	{
		m_treeSrv.DeleteAllItems();
		// installed  OPC servers
		SOCltServerBrowser* sb;
		HTREEITEM local, manual, manualMachine;
		sb = new SODmCServerBrowser();
		sb->setNodeName(_T(""));
		local = SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Local"), TRUE, (void*)(SOCltServerBrowser*)sb, SODMCMAINVIEW_IMG_MACHINE, SODMCMAINVIEW_IMG_MACHINE, TVI_LAST);
#ifdef SODMC_DA
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V1"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA10A, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V2"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA20, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, local, _T("Data Access V3"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA30, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#ifdef SODMC_AE
		SODmCTreeInsert(&m_treeSrv, local, _T("Alarms and Events"), TRUE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
		SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Remote"), TRUE, NULL, SODMCMAINVIEW_IMG_NETWORK, SODMCMAINVIEW_IMG_NETWORK, TVI_LAST);
		manual = SODmCTreeInsert(&m_treeSrv, TVI_ROOT, _T("Manual"), TRUE, NULL, SODMCMAINVIEW_IMG_MANUAL, SODMCMAINVIEW_IMG_MANUAL, TVI_LAST);
		sb = new SODmCServerBrowser();
		sb->setNodeName(_T(""));
		manualMachine = SODmCTreeInsert(&m_treeSrv, manual, _T("<Computer name or address>"), TRUE, (void*)(SOCltServerBrowser*)sb, SODMCMAINVIEW_IMG_MACHINE_MANUAL, SODMCMAINVIEW_IMG_MACHINE_MANUAL, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("<CLSID of the OPC Data Access Server>"), FALSE, (void*)(SOCLT_BROWSE_SERVER_DA20 | SOCLT_BROWSE_SERVER_DA10A), SODMCMAINVIEW_IMG_CLSID_MANUAL, SODMCMAINVIEW_IMG_CLSID_MANUAL, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("<CLSID of the OPC Alarms and Events Server>"), FALSE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_CLSID_MANUAL, SODMCMAINVIEW_IMG_CLSID_MANUAL, TVI_LAST);
#ifdef SODMC_DA
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V1"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA10A, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V2"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA20, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Data Access V3"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA30, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#ifdef SODMC_AE
		SODmCTreeInsert(&m_treeSrv, manualMachine, _T("Alarms and Events"), TRUE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
	}
	break;
#endif // SODMC_DCOM
#ifdef SODMC_DA

	case SODMCMAINVIEW_TAB_TDA: // DA Browse
		changeSelectedServerDA(m_treeDAServer, TRUE);
		break;

	case SODMCMAINVIEW_TAB_LDA: // DA Items
		updateItemProperties();
		break;
#endif
#ifdef SODMC_AE

	case SODMCMAINVIEW_TAB_TAE: // AE Browse
		changeSelectedSubscription(m_treeAESubscription, TRUE);
		break;
#endif
	}
} // onRefresh


//-----------------------------------------------------------------------------
// onUpdRefresh
//
// - update menu handler of menu view/refresh
//
afx_msg void SODmCMainView::onUpdRefresh(OUT CCmdUI* pCmdUI)
{
	if ((m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA) ||
		(m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TAE) ||
		(m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA) ||
		(m_tab.GetCurSel() == SODMCMAINVIEW_TAB_SRV))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdRefresh


//-----------------------------------------------------------------------------
// onDeleteObject
//
// - menu handler of menu edit/delete object
//
afx_msg void SODmCMainView::onDeleteObject(void)
{
#ifdef SODMC_DA
	// remove the selected item
	//SOCmnPointer<SODmCItem> item(getSelectedItem());
	int iCnt = m_listDA.GetSelectedCount();
	int iSelCnt = iCnt;
	int* DeletionIndices = new int[iCnt];
	POSITION pos = m_listDA.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_listDA.GetNextSelectedItem(pos);
		iSelCnt--;
		DeletionIndices[iSelCnt] = nItem;
	}

	for (iSelCnt = 0; iSelCnt < iCnt; iSelCnt++)
	{
		int nItem = DeletionIndices[iSelCnt];
		SODaCItem* item = (SODaCItem*)m_listDA.GetItemData(nItem);

		if (item != NULL)
		{
			SOCmnPointer<SOCmnElement> parent = item->getParentBranch();
			item->disconnect();
			parent->removeLeaf(item);
		}
	}

	getApp()->setDirtyFlag();
	delete DeletionIndices;
#endif
} // onDeleteObject


//-----------------------------------------------------------------------------
// onUpdDeleteObject
//
// - update menu handler of menu edit/delete object
//
afx_msg void SODmCMainView::onUpdDeleteObject(OUT CCmdUI* pCmdUI)
{
	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
#ifdef SODMC_DA
		SOCmnPointer<SODmCItem> item(getSelectedItem());
		pCmdUI->Enable(item.isNotNull());
#endif
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdDeleteObject


//-----------------------------------------------------------------------------
// onAddServer
//
// - menu handler of menu edit/add server
//
afx_msg void SODmCMainView::onAddServer(void)
{
#ifdef SODMC_DCOM

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_SRV) // Installed OPC Servers
	{
		OnTreeSrvDC(NULL, NULL);
	}

#endif // SODMC_DCOM
} // onAddServer


//-----------------------------------------------------------------------------
// onUpdAddServer
//
// - update menu handler of menu edit/add server
//
afx_msg void SODmCMainView::onUpdAddServer(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DCOM

	if (m_tab.GetCurSel() ==  SODMCMAINVIEW_TAB_SRV) // Installed OPC Servers
	{
		HTREEITEM hItem;
		int img, selImg;
		hItem = m_treeSrv.GetSelectedItem();
		m_treeSrv.GetItemImage(hItem, img, selImg);
		pCmdUI->Enable((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS));
		return;
	}

#endif // SODMC_DCOM
	pCmdUI->Enable(FALSE);
} // onUpdAddServer


//-----------------------------------------------------------------------------
// onAddItem
//
// - menu handler of menu edit/add item
//
afx_msg void SODmCMainView::onAddItem(void)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA) // DA Browse
	{
		OnTreeDaDC(NULL, NULL);
	}

#endif
} // onAddItem


//-----------------------------------------------------------------------------
// onUpdAddItem
//
// - update menu handler of menu edit/add item
//
afx_msg void SODmCMainView::onUpdAddItem(OUT CCmdUI* pCmdUI)
{
	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA) // DA Browse
	{
#ifdef SODMC_DA
		HTREEITEM hItem;
		int img, selImg;
		hItem = m_treeDA.GetSelectedItem();
		m_treeDA.GetItemImage(hItem, img, selImg);
		pCmdUI->Enable((img == SODMCMAINVIEW_IMG_TAG) || (img == SODMCMAINVIEW_IMG_PROPERTY));
		return;
#endif
	}

	pCmdUI->Enable(FALSE);
} // onUpdAddItem


//-----------------------------------------------------------------------------
// onAddTagsOfNode
//
// - menu handler of menu edit/add all tags of node
//
afx_msg void SODmCMainView::onAddTagsOfNode(void)
{
#ifdef SODMC_DA
	SODmCObjectView* objview = ::getObjectView();
	SOCmnPointer<SODaCGroup> curSelGrp = objview->getSelectedGroup();

	if (curSelGrp.isNotNull())
	{
		HTREEITEM hItem = m_treeDA.GetSelectedItem();
		SOCmnString path;
		// add DA item to the selected DA group
		SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);

		if (bobj)
		{
			if ((bobj->getObjectType() == SOCMNOBJECT_TYPE_NODE) ||
				(bobj->getObjectType() == SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))
			{
				OPCNAMESPACETYPE nsType = OPC_NS_HIERARCHIAL;
#ifdef SODMC_DCOM
				SOCmnPointer<SODaCNameSpaceBrowser> broot = (SODaCNameSpaceBrowser*)bobj->getBrowser();
				SOCmnPointer<SODaCServer> server = broot->getServer();

				if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
					HRESULT res;
					res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

					if (SUCCEEDED(res))
					{
						SOCmnPointer<SODaCServer> server = broot->getServer();
						server->beforeInterfaceCall();
						res = browseSAS->QueryOrganization(&nsType);
						server->afterInterfaceCall();
					}
				}

#endif
				addTagsOfNode(bobj, (SODaCGroup*)curSelGrp, nsType);
				curSelGrp->start(TRUE, SOCLT_EXEC_ASYNC);
				getApp()->setDirtyFlag();
			}
		}
	}

#endif
} // onAddTagsOfNode


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// addTagsOfNode
//
// - method to add all tags of a node to the selected group
//
void SODmCMainView::addTagsOfNode(
	SOCltBrowseObject* node,
	SODaCGroup* group,
	OPCNAMESPACETYPE nsType)
{
	SOCmnPointer<SOCltBrowseObject> broot = node->getBrowser();
	SOCmnList<SOCltBrowseObject> objList;
	SOCmnListPosition pos;
	SOCltBrowseObject* bobj;
	SOCmnString path;

	// browse all objects of the next level
	if (broot->getObjectState() == SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE)
	{
		node->expand(SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM, objList);
	}
	else
	{
		if (nsType == OPC_NS_HIERARCHIAL)
		{
			node->expand(SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_TAG, objList);
		}
		else
		{
			node->expand(SOCMNOBJECT_TYPE_TAG, objList);
		}
	}

	pos = objList.getStartPosition();

	while (pos)
	{
		bobj = objList.getNext(pos);

		switch (bobj->getObjectType())
		{
		case SOCMNOBJECT_TYPE_NODE:
			if (((SODaCNode*)bobj)->isItem())
			{
				path = bobj->getPath();

				if (!path.isEmpty())
				{
					SOCmnPointer<SODmCItem> item = (SODmCItem*)group->addItem(path, VT_EMPTY, bobj->getItemPath());
				}
			}

			addTagsOfNode(bobj, group, nsType);
			break;

		case SOCMNOBJECT_TYPE_TAG:
			path = bobj->getPath();

			if (!path.isEmpty())
			{
				SOCmnPointer<SODmCItem> item = (SODmCItem*)group->addItem(path, VT_EMPTY, bobj->getItemPath());
			}

			break;
		}
	}
} // addTagsOfNode
#endif


//-----------------------------------------------------------------------------
// onUpdAddTagsOfNode
//
// - update menu handler of menu edit/add all tags
//
afx_msg void SODmCMainView::onUpdAddTagsOfNode(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	HTREEITEM hItem;
	int img, selImg;

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA)
	{
		// DA Browse
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SODaCGroup> curSelGrp = objview->getSelectedGroup();

		if (curSelGrp.isNotNull())
		{
			hItem = m_treeDA.GetSelectedItem();
			m_treeDA.GetItemImage(hItem, img, selImg);
			pCmdUI->Enable((img == SODMCMAINVIEW_IMG_NODE_ITEM) || (img == SODMCMAINVIEW_IMG_NODE) || (SODMCMAINVIEW_IMG_SRV_DA));
			return;
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdAddObject


//-----------------------------------------------------------------------------
// onBrowseAllTagsOfNode2File
//
// - menu handler of menu edit/add all tags of node
//
afx_msg void SODmCMainView::onBrowseAllTagsOfNode2File(void)
{
#ifdef SODMC_DA
	SODmCObjectView* objview = ::getObjectView();
	SOCmnPointer<SODaCGroup> curSelGrp = objview->getSelectedGroup();

	if (curSelGrp.isNotNull())
	{
		HTREEITEM hItem = m_treeDA.GetSelectedItem();
		SOCmnString path;
		// add DA item to the selected DA group
		SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);

		if (bobj)
		{
			if ((bobj->getObjectType() == SOCMNOBJECT_TYPE_NODE) ||
				(bobj->getObjectType() == SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))
			{
				OPCNAMESPACETYPE nsType = OPC_NS_HIERARCHIAL;
#ifdef SODMC_DCOM
				SOCmnPointer<SODaCNameSpaceBrowser> broot = (SODaCNameSpaceBrowser*)bobj->getBrowser();
				SOCmnPointer<SODaCServer> server = broot->getServer();

				if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
					HRESULT res;
					res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

					if (SUCCEEDED(res))
					{
						SOCmnPointer<SODaCServer> server = broot->getServer();
						server->beforeInterfaceCall();
						res = browseSAS->QueryOrganization(&nsType);
						server->afterInterfaceCall();
					}
				}

#endif
				// show save file dialog
				CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
								_T("All Files (*.*)|*.*||"));
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
				dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

				if (IDOK != dlg.DoModal())
				{
					return;    // file dialog is canceled
				}

				// get file name
				SOCmnString sFileName;
				sFileName = dlg.GetPathName();
				CWaitCursor waitCursor;
				CFile file;

				if (!file.Open(sFileName, CFile::modeCreate | CFile::modeWrite))
				{
					SOCmnString errDescr;
					errDescr.format(_T("Can't open file %s to write itemIDs to file"), sFileName);
					MessageBox(errDescr, _T("Error"), MB_OK | MB_ICONERROR);
					return; // file open error
				}

				browseAllTagsOfNode2File(bobj, &file, nsType);
				file.Close();
			}
		}
	}

#endif
}// onBrowseAllTagsOfNode2File


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// browseAllTagsOfNode2File
//
// - method to add all tags of a node to the selected group
//
void SODmCMainView::browseAllTagsOfNode2File(
	SOCltBrowseObject* node,
	CFile* file,
	OPCNAMESPACETYPE nsType)
{
	if (!file)
	{
		return;
	}

	SOCmnPointer<SOCltBrowseObject> broot = node->getBrowser();
	SOCmnList<SOCltBrowseObject> objList;
	SOCmnListPosition pos;
	SOCltBrowseObject* bobj;
	SOCmnString path;

	// browse all objects of the next level
	if (broot->getObjectState() == SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE)
	{
		node->expand(SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM, objList);
	}
	else
	{
		if (nsType == OPC_NS_HIERARCHIAL)
		{
			node->expand(SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_TAG, objList);
		}
		else
		{
			node->expand(SOCMNOBJECT_TYPE_TAG, objList);
		}
	}

	USES_CONVERSION;
	pos = objList.getStartPosition();

	while (pos)
	{
		bobj = objList.getNext(pos);

		switch (bobj->getObjectType())
		{
		case SOCMNOBJECT_TYPE_NODE:
			if (((SODaCNode*)bobj)->isItem())
			{
				path = bobj->getPath();

				if (!path.isEmpty())
				{
					USES_CONVERSION;
					SOCmnString szItemID((path + _T("\r\n")));
					file->Write(T2A(szItemID), strlen(T2A(szItemID)));
				}
			}

			browseAllTagsOfNode2File(bobj, file, nsType);
			break;

		case SOCMNOBJECT_TYPE_TAG:
			path = bobj->getPath();

			if (!path.isEmpty())
			{
				SOCmnString szItemID((path + _T("\r\n")));
				file->Write(T2A(szItemID), strlen(T2A(szItemID)));
			}

			break;
		}
	}
} // browseAllTagsOfNode2File
#endif


//-----------------------------------------------------------------------------
// onUpdBrowseAllTagsOfNode2File
//
// - update menu handler of menu edit/add all tags
//
afx_msg void SODmCMainView::onUpdBrowseAllTagsOfNode2File(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	HTREEITEM hItem;
	int img, selImg;

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA)
	{
		// DA Browse
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SODaCGroup> curSelGrp = objview->getSelectedGroup();

		if (curSelGrp.isNotNull())
		{
			hItem = m_treeDA.GetSelectedItem();
			m_treeDA.GetItemImage(hItem, img, selImg);
			pCmdUI->Enable((img == SODMCMAINVIEW_IMG_NODE_ITEM) || (img == SODMCMAINVIEW_IMG_NODE) || (SODMCMAINVIEW_IMG_SRV_DA));
			return;
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdAddObject

//onAddNewGroupWithAllTags)
afx_msg void SODmCMainView::onAddNewGroupWithAllTags(void)
{
#ifdef SODMC_DA
	HTREEITEM hItem = m_treeDA.GetSelectedItem();
	SOCmnString path;
	// add DA item to the selected DA group
	SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);

	if (bobj)
	{
		if ((bobj->getObjectType() == SOCMNOBJECT_TYPE_NODE) ||
			(bobj->getObjectType() == SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))
		{
			OPCNAMESPACETYPE nsType = OPC_NS_HIERARCHIAL;
			SOCmnPointer<SODaCNameSpaceBrowser> broot = (SODaCNameSpaceBrowser*)bobj->getBrowser();
			SOCmnPointer<SODaCServer> server = broot->getServer();
#ifdef SODMC_DCOM

			if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
				HRESULT res;
				res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

				if (SUCCEEDED(res))
				{
					SOCmnPointer<SODaCServer> server = broot->getServer();
					server->beforeInterfaceCall();
					res = browseSAS->QueryOrganization(&nsType);
					server->afterInterfaceCall();
				}
			}

#endif

			if (server.isNotNull())
			{
				SOCmnPointer<SODaCGroup> pGroup = server->addGroup(bobj->getName());

				if (!pGroup)
				{
					SOCmnString nameExt;
					nameExt.Format(_T("%04d"), GetTickCount() / 1000);
					pGroup = server->addGroup(SOCmnString(bobj->getName()) + nameExt);
				}

				if (pGroup.isNotNull())
				{
					addTagsOfNode(bobj, (SODaCGroup*)pGroup, nsType);
					pGroup->start(TRUE, SOCLT_EXEC_ASYNC);
					getApp()->setDirtyFlag();
				}
			}
		}
	}

#endif
}

//-----------------------------------------------------------------------------
// onUpdAddNewGroupWithAllTags
//
// - update menu handler of menu edit/add all tags to new group
//
afx_msg void SODmCMainView::onUpdAddNewGroupWithAllTags(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	HTREEITEM hItem;
	int img, selImg;

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA)
	{
		// DA Browse
		hItem = m_treeDA.GetSelectedItem();
		m_treeDA.GetItemImage(hItem, img, selImg);
		pCmdUI->Enable((img == SODMCMAINVIEW_IMG_NODE_ITEM) || (img == SODMCMAINVIEW_IMG_NODE) || (SODMCMAINVIEW_IMG_SRV_DA));
		return;
	}

#endif
	pCmdUI->Enable(FALSE);
}


//-----------------------------------------------------------------------------
// onWriteFromFile
//
// - menu handler of menu edit/write from file
//
afx_msg void SODmCMainView::onWriteFromFile(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());
	writeFromFile(item);
#endif
} // onWriteFromFile


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// writeFromFile
//
// - write item from file
//
void SODmCMainView::writeFromFile(IN SODmCItem* item)
{
	if (!item)
	{
		return;    // no item selected
	}

	if (!(item->getAccessRights() & OPC_WRITEABLE) ||
		(!(item->getNativeDatatype() & VT_ARRAY) && !(item->getNativeDatatype() & VT_BSTR)))
	{
		return;    // write is not allowed on the selected item
	}

	if ((item->getNativeDatatype() & VT_ARRAY) &&
		(!(item->getNativeDatatype() & VT_UI1) || !(item->getNativeDatatype() & VT_I1)))
	{
		return;    // write is not allowed on the selected item
	}

	// show file open dialog
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"));
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
	dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

	if (IDOK != dlg.DoModal())
	{
		return;    // file open dialog canceled
	}

	// get file name
	SOCmnString sFileName;
	sFileName = dlg.GetPathName();
	// open choosen file to read the data
	CWaitCursor waitCursor;
	CStdioFile  file;

	if (!file.Open(sFileName, CFile::modeRead | CFile::typeBinary))
	{
		SOCmnString errDescr;
		errDescr.format(_T("Can't open file %s to write item value from file"), sFileName);
		insertError(item, HRESULT_FROM_WIN32(GetLastError()), SODMC_ERR_OPEN_FILE, errDescr);
		return; // file open error
	}

	int size = (int)file.GetLength();

	if (size)
	{
		BYTE* stg = (BYTE*)malloc(size + 1);

		if (stg == NULL)
		{
			// out of memory
			item->writeFailed(E_OUTOFMEMORY);
			file.Close();
			return;
		}

		file.Read((void*)stg, size);
		const unsigned char* pu = reinterpret_cast<const unsigned char*>(stg);
		bool isUTF8 = false;
		wchar_t *wideString = NULL;
		int res = 0;
		if ((size >= 3) && (*(pu+0) == UTF_LEAD_0
            && *(pu+1) == UTF_LEAD_1
            && *(pu+2) == UTF_LEAD_2))
		{
			isUTF8 = true;
			pu += 3;
			res = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)stg, size, NULL, 0);
			wideString = (wchar_t*)malloc(sizeof(wchar_t) * res + 1);
			memset(wideString, 0, res + 1);
			res = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)stg + 3, size - 3, wideString, res);
			if(res != 0)
			{
				wideString[res] = 0;
			}
			else
			{
				SOCmnString errDescr;
				errDescr.format(_T("Can't convert to WideChar file %s to write from file: 0x%08x"), sFileName, HRESULT_FROM_WIN32(GetLastError()));
				MessageBox(errDescr, _T("Error"), MB_OK | MB_ICONERROR);
				// TODO - cleanup
				free(wideString);
				free(stg);
				file.Close();
				return; 
			}
		}
		else
		{
			// TODO
			ASSERT(FALSE);
		}
		
		SOCmnVariant value;

		if (item->getNativeDatatype() & VT_ARRAY)
		{
			// array
			SAFEARRAYBOUND dim;
			BYTE* pData;
			dim.lLbound = 0;
			dim.cElements = size;

			if ((item->getNativeDatatype() & VT_UI1) == VT_UI1)
			{
				value.vt = VT_UI1 | VT_ARRAY;
			}
			else // if (item->getNativeDatatype() & VT_I1)
			{
				value.vt = VT_I1 | VT_ARRAY;
			}

			value.parray = SOCmnVariant::safeArrayCreate(VT_UI1, 1, &dim);
			SafeArrayAccessData(value.parray, (void**)&pData);
			memcpy(pData, stg, size);
			SafeArrayUnaccessData(value.parray);
		}
		else
		{
			// if (item->getNativeDatatype() & VT_BSTR) => string
			USES_CONVERSION;
			stg[size] = '\0';
			value.vt = VT_BSTR;
			if(isUTF8 == true)
			{
				value.bstrVal = ::SysAllocString(T2W((LPTSTR)wideString));
			}
			else
			{
				value.bstrVal = ::SysAllocString(T2W((LPTSTR)stg));
			}
		}

		// write the item data
		item->setWriteValue(value);
		item->write();
		if(wideString != NULL)
		{
			free(wideString);
		}
		free(stg);
	}

	file.Close();
} // writeFromFile
#endif


//-----------------------------------------------------------------------------
// onUpdWriteFromFile
//
// - update menu handler of menu edit/write from file
//
afx_msg void SODmCMainView::onUpdWriteFromFile(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull() && m_listDA.GetSelectedCount() == 1)
		{
			if ((item->getAccessRights() & OPC_WRITEABLE)
				&& ((item->getNativeDatatype() & VT_ARRAY) || (item->getNativeDatatype() & VT_BSTR)))
			{
				if ((item->getNativeDatatype() & VT_ARRAY)
					&& (!(item->getNativeDatatype() & VT_UI1) || !(item->getNativeDatatype() & VT_I1)))
				{
					pCmdUI->Enable(FALSE);
					return; // write is not allowed on the selected array
				}

				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}

#endif
	pCmdUI->Enable(FALSE);
	return;
} // onUpdWriteFromFile


//-----------------------------------------------------------------------------
// onReadToFile
//
// - menu handler of menu edit/read to file
//
afx_msg void SODmCMainView::onReadToFile(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());
	readToFile(item);
#endif
} // onReadToFile


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// readToFile
//
// - read item to file
//
void SODmCMainView::readToFile(IN SODmCItem* item)
{
	if (!item)
	{
		return;    // no item selected
	}

	if (!(item->getAccessRights() & OPC_READABLE) ||
		(!(item->getNativeDatatype() & VT_ARRAY) && !(item->getNativeDatatype() & VT_BSTR)))
	{
		return;    // read is not allowed on the selected item
	}

	if ((item->getNativeDatatype() & VT_ARRAY) &&
		(!(item->getNativeDatatype() & VT_UI1) || !(item->getNativeDatatype() & VT_I1)))
	{
		return;    // read is not allowed on the selected item
	}

	// read item data
	HRESULT res;
	SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)item->getParentBranch();

	if (!(SODaCGroup*) grp)
	{
		return;
	}

	SOCmnList<SODaCItem> itemList;
	itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	itemList.add(item);
	// call the read method
	res = grp->syncRead(&itemList, SODAC_MAXAGE_DEVICE);
	itemList.destroy();

	if (res != S_OK)
	{
		return;    // read error
	}

	// show save file dialog
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
					_T("All Files (*.*)|*.*||"));
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
	dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

	if (IDOK != dlg.DoModal())
	{
		return;    // file dialog is canceled
	}

	// get file name
	SOCmnString sFileName;
	sFileName = dlg.GetPathName();
	CWaitCursor waitCursor;
	CStdioFile  file;

	if (!file.Open(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		SOCmnString errDescr;
		errDescr.format(_T("Can't open file %s to read item value to file"), sFileName);
		insertError(item, HRESULT_FROM_WIN32(GetLastError()), SODMC_ERR_OPEN_FILE, errDescr);
		return; // file open error
	}

	SOCmnVariant value;

	if (item->getNativeDatatype() & VT_ARRAY)
	{
		// array
		if ((item->getNativeDatatype() & VT_UI1) == VT_UI1)
		{
			value.vt = VT_UI1 | VT_ARRAY;
		}
		else // if (item->getNativeDatatype() & VT_I1)
		{
			value.vt = VT_I1 | VT_ARRAY;
		}

		// read item data to a variant
		if (item->getReadValue(value))
		{
			LONG lBoundV, uBoundV;
			SafeArrayGetLBound(value.parray, 1, &lBoundV);
			SafeArrayGetUBound(value.parray, 1, &uBoundV);
			LONG size = (uBoundV - lBoundV + 1);
			BYTE* stg = (BYTE*)malloc(size + 1);

			if (stg == NULL)
			{
				// out of memory
				item->readFailed(E_OUTOFMEMORY, OPC_QUALITY_BAD);
				file.Close();
				return;
			}

			BYTE* pData;
			SafeArrayAccessData(value.parray, (void**)&pData);
			memcpy(stg, pData, size);
			SafeArrayUnaccessData(value.parray);

			if (size) // copy object data to the file
			{
				file.Write(stg, size);
			}

			free(stg);
		}
	}
	else // BSTR
	{
		// if (item->getNativeDatatype() & VT_BSTR) => string
		USES_CONVERSION;
		value.vt = VT_BSTR;

		// read item data to a variant
		if (item->getReadValue(value))
		{
			UINT size = SysStringLen(value.bstrVal);

			if (size) // copy object data to the file
			{
				file.Write(W2A(value.bstrVal), size);
			}
		}
	}

	file.Close();
} // readToFile
#endif


//-----------------------------------------------------------------------------
// onUpdReadToFile
//
// - update menu handler of menu edit/read to file
//
afx_msg void SODmCMainView::onUpdReadToFile(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull() && m_listDA.GetSelectedCount() == 1)
		{
			if ((item->getAccessRights() & OPC_READABLE)
				&& ((item->getNativeDatatype() & VT_ARRAY) || (item->getNativeDatatype() & VT_BSTR)))
			{
				if ((item->getNativeDatatype() & VT_ARRAY)
					&& (!(item->getNativeDatatype() & VT_UI1) || !(item->getNativeDatatype() & VT_I1)))
				{
					pCmdUI->Enable(FALSE);
					return; // write is not allowed on the selected array
				}

				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}

#endif
	pCmdUI->Enable(FALSE);
	return;
} // onUpdReadToFile


//-----------------------------------------------------------------------------
// onProperties
//
// - menu handler of menu edit/properties object
//
afx_msg void SODmCMainView::onProperties(void)
{
	SODmCPSheet* sheet = getPropertyFrame()->getPropertySheet();

	switch (m_tab.GetCurSel())
	{
#ifdef SODMC_DA

	case SODMCMAINVIEW_TAB_TDA:
	{
		// DA Browse
		HTREEITEM hItem = m_treeDA.GetSelectedItem();
		SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);

		if (bobj)
		{
			switch (bobj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_NAMESPACE_BROWSER:
			{
				SOCmnPointer<SODaCServer> server = (SODaCServer*)((SODaCNameSpaceBrowser*)bobj)->getServer();
				sheet->showServerDAAttributes(server);
			}
			break;

			case SOCMNOBJECT_TYPE_NODE:
				sheet->showNodeAttributes((SODaCNode*)bobj);
				break;

			case SOCMNOBJECT_TYPE_TAG:
				sheet->showTagAttributes((SODaCTag*)bobj);
				break;

			case SOCMNOBJECT_TYPE_PROPERTY:
				sheet->showPropertyAttributes((SODaCProperty*)bobj);
				break;

			default:
				sheet->showAbout();
				break;
			}
		}
		else
		{
			sheet->showAbout();
		}
	}
	break;

	case SODMCMAINVIEW_TAB_LDA:
	{
		// DA Items
		SOCmnPointer<SODmCItem> item = getSelectedItem();

		if (item.isNotNull())
		{
			sheet->showItemAttributes((SODmCItem*)item);
		}
		else
		{
			sheet->showAbout();
		}
	}
	break;
#endif
#ifdef SODMC_AE

	case SODMCMAINVIEW_TAB_TAE:
	{
		// AE Browse
		HTREEITEM hItem = m_treeAE.GetSelectedItem();
		SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeAE.GetItemData(hItem);

		if (bobj)
		{
			switch (bobj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_AREA_BROWSER:
			{
				SOCmnPointer<SOAeCServer> server = (SOAeCServer*)((SOAeCAreaBrowser*)bobj)->getServer();
				sheet->showServerAEAttributes(server);
			}
			break;

			default:
				sheet->showAbout();
				break;
			}
		}
		else
		{
			sheet->showAbout();
		}
	}
	break;
#endif

	default:
		sheet->showAbout();
		break;
	}

	getMainFrame()->showPropertyFrame();
} // onProperties


//-----------------------------------------------------------------------------
// onUpdProperties
//
// - update menu handler of menu edit/properties object
//
afx_msg void SODmCMainView::onUpdProperties(OUT CCmdUI* pCmdUI)
{
	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TDA)
	{
		HTREEITEM hItem = m_treeDA.GetSelectedItem();

		if (hItem)
		{
			SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);
			pCmdUI->Enable(bobj->is(SOCMNOBJECT_TYPE_NODE) || bobj->is(SOCMNOBJECT_TYPE_TAG) || bobj->is(SOCMNOBJECT_TYPE_PROPERTY) || bobj->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER));
			return;
		}
	}
	else if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
#ifdef SODMC_DA
		SOCmnPointer<SODmCItem> item = getSelectedItem();

		if (m_listDA.GetSelectedCount() == 1)
		{
			pCmdUI->Enable(item.isNotNull());
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}

#endif
		return;
	}
	else if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_TAE)
	{
		HTREEITEM hItem = m_treeDA.GetSelectedItem();

		if (hItem)
		{
			SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);
			pCmdUI->Enable(bobj->is(SOCMNOBJECT_TYPE_AREA_BROWSER));
			return;
		}
	}

	pCmdUI->Enable(FALSE);
} // onUpdProperties


//-----------------------------------------------------------------------------
// onStartObjFlat
//
// - menu handler State/Start/Flat
//
afx_msg void SODmCMainView::onStartObjFlat(void)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		performGroupedStateTransition(SOCltElement::started);
	}

#endif
} // onStartObjFlat


//-----------------------------------------------------------------------------
// onUpdStartObjFlat
//
// - update menu handler of menu  State/Start/Flat
//
afx_msg void SODmCMainView::onUpdStartObjFlat(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull())
		{
			pCmdUI->Enable((m_listDA.GetSelectedCount() > 1) || (item->getObjectState() != SOCltElement::started));
			return;
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdStartObjFlat


//-----------------------------------------------------------------------------
// onConnectObjFlat
//
// - menu handler State/Connect/Flat
//
afx_msg void SODmCMainView::onConnectObjFlat(void)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		performGroupedStateTransition(SOCltElement::connected);
	}

#endif
} // onConnectObjFlat


//-----------------------------------------------------------------------------
// onUpdConnectObjFlat
//
// - update menu handler of menu  State/Connect/Flat
//
afx_msg void SODmCMainView::onUpdConnectObjFlat(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull())
		{
			pCmdUI->Enable((m_listDA.GetSelectedCount() > 1) || (item->getObjectState() != SOCltElement::connected));
			return;
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdConnectObjFlat


//-----------------------------------------------------------------------------
// onStopObj
//
// - menu handler State/Stop
//
afx_msg void SODmCMainView::onStopObj(void)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		performGroupedStateTransition(SOCltElement::disconnected);
	}

#endif
} // onStopObj


//-----------------------------------------------------------------------------
// onUpdStopObj
//
// - update menu handler of menu  State/Stop
//
afx_msg void SODmCMainView::onUpdStopObj(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull())
		{
			pCmdUI->Enable((m_listDA.GetSelectedCount() > 1) || (item->getObjectState() != SOCltElement::disconnected));
			return;
		}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdStopObj


//-----------------------------------------------------------------------------
// onReadItem
//
// - menu handler Read
//
afx_msg void SODmCMainView::onReadItem(void)
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		readGrouped();
	}

#endif
} // onReadItem



//-----------------------------------------------------------------------------
// onUpdReadItem
//
// - update menu handler of menu Read
//
afx_msg void SODmCMainView::onUpdReadItem(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	/*if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
	    SOCmnPointer<SODmCItem> item(getSelectedItem());
	    if(item.isNotNull())
	        pCmdUI->Enable(item->isConnected());
	    else
	        pCmdUI->Enable(FALSE);
	}*/
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
} // onUpdReadItem

//-----------------------------------------------------------------------------
// onWriteItem
//
// - menu handler Write
//
afx_msg void SODmCMainView::onWriteItem(void)
{
#ifdef SODMC_DA
	/*  if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	    {
	        SOCmnPointer<SODmCItem> item(getSelectedItem());
	        if(item.isNotNull())
	            item->read(SODAC_MAXAGE_DEVICE);
	    }
	*/
	SODmCMultiWriteDlg dlgMultiWrite;
	dlgMultiWrite.DoModal();
#endif
} // onReadItem

//-----------------------------------------------------------------------------
// onUpdWriteItem
//
// - update menu handler of menu Write
//
afx_msg void SODmCMainView::onUpdWriteItem(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	/*if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
	    SOCmnPointer<SODmCItem> item(getSelectedItem());
	    if(item.isNotNull())
	        pCmdUI->Enable(item->isConnected());
	    else
	        pCmdUI->Enable(FALSE);
	}*/
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
} // onUpdReadItem


void SODmCMainView::OnLogToFile()
{
#ifdef SODMC_DA

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull())
		{
			// show save file dialog
			CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
							_T("All Files (*.*)|*.*||"));
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
			dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

			if (IDOK != dlg.DoModal())
			{
				item->setLogFile(SOCmnString());
			}
			else
			{
				// get file name
				item->setLogFile(SOCmnString(dlg.GetPathName()));
			}

			item->setLogToFile(!item->getLogFile().isEmpty());
		}
	}

#endif // SODMC_DA
}

//-----------------------------------------------------------------------------
// OnUpdLogToFile
//
// - update menu handler of menu LogToFile
//
void SODmCMainView::OnUpdLogToFile(CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	pCmdUI->Enable(TRUE);

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LDA)
	{
		SOCmnPointer<SODmCItem> item(getSelectedItem());

		if (item.isNotNull())
		{
			pCmdUI->SetCheck(item->getLogToFile());
		}
		else
		{
			pCmdUI->SetCheck(FALSE);
		}
	}

#else
	pCmdUI->Enable(FALSE);
#endif // SODMC_DA
}


#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// OPC Servers Tab

//-----------------------------------------------------------------------------
// OnTreeSrvIE
//
// - item expanding message
//
void SODmCMainView::OnTreeSrvIE(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmTV = (NMTREEVIEW*)pNMHDR;

	if (pResult)
	{
		*pResult = 0;
	}

	if (TVE_EXPAND != (nmTV->action & TVE_EXPAND))
	{
		return;    // check if to expand
	}

	if (TVIS_EXPANDEDONCE & (nmTV->itemNew.state))
	{
		return;
	}

	if (nmTV->itemNew.iImage == SODMCMAINVIEW_IMG_COM_CATEGORY)
	{
		// COM category item
		// browse the COM category
		HTREEITEM hItem = nmTV->itemNew.hItem;
		HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);

		if (hParent == NULL)
		{
			return;
		}

		SOCltServerBrowser* sb = (SOCltServerBrowser*)m_treeSrv.GetItemData(hParent);

		if (sb == NULL)
		{
			return;
		}

		CWaitCursor wait;
		DWORD whatServers = m_treeSrv.GetItemData(hItem);
		SOCmnList<CLSID> clsidList;
		clsidList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_VOID);

		// browse for the installed servers
		if (sb->browseServer(whatServers, clsidList))
		{
			SOCmnListPosition pos;
			CLSID* clsid, *cpyClsid = NULL;
			SOCmnString classID;
			SOCmnString descr;
			SOCmnString progID;
			SOCmnString viProgID;
			HTREEITEM server;
			pos = clsidList.getStartPosition();

			while (pos)
			{
				clsid = clsidList.getNext(pos);
				sb->getClassDetail(*clsid, descr, progID, viProgID);
				classID.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), clsid->Data1, clsid->Data2, clsid->Data3, clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3], clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);
				cpyClsid = (CLSID*)malloc(sizeof(CLSID));
				memcpy(cpyClsid, clsid, sizeof(CLSID));
				// add server and attributes to the tree
				server = SODmCTreeInsert(&m_treeSrv, hItem, descr, TRUE, cpyClsid, SODMCMAINVIEW_IMG_OPCSERVER, SODMCMAINVIEW_IMG_OPCSERVER, TVI_SORT);
				SODmCTreeInsert(&m_treeSrv, server, classID, FALSE, NULL, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, TVI_LAST);
				SODmCTreeInsert(&m_treeSrv, server, progID, FALSE, NULL, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, TVI_LAST);

				if (!viProgID.isEmpty())
				{
					SODmCTreeInsert(&m_treeSrv, server, viProgID, FALSE, NULL, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, SODMCMAINVIEW_IMG_OPCSERVER_PROPS, TVI_LAST);
				}
			}
		}

		if (clsidList.isEmpty())
		{
			// no servers of this category installed
			TVITEM  item;
			item.hItem = hItem;
			item.mask = TVIF_CHILDREN | TVIF_HANDLE;
			item.cChildren = FALSE;
			m_treeSrv.SetItem(&item);
		}
	}
	else if (nmTV->itemNew.iImage == SODMCMAINVIEW_IMG_NETWORK)
	{
		// network item
		// insert the domains
		CWaitCursor wc;
		insertNetwork(NULL, nmTV->itemNew.hItem);
	}
	else if (nmTV->itemNew.iImage == SODMCMAINVIEW_IMG_DOMAIN)
	{
		// network item
		// insert the servers of the domain
		CWaitCursor wc;
		LPNETRESOURCE pnrDomain = (LPNETRESOURCE)nmTV->itemNew.lParam;
		insertNetwork(pnrDomain, nmTV->itemNew.hItem);
	}
} // OnTreeSrvIE


//-----------------------------------------------------------------------------
// insertNetwork
//
// - browse the network and add the nodes to the tree
//
void SODmCMainView::insertNetwork(LPNETRESOURCE pnr, HTREEITEM hItem)
{
	DWORD dwResult, dwResultEnum;
	HANDLE hEnum;
	DWORD i;

	if (NO_ERROR != WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_ANY, 0, pnr, &hEnum))
	{
		return;
	}

	do
	{
		DWORD cbBuffer = 16384;      // 16K is a good size
		DWORD cEntries = 0xFFFFFFFF; // enumerate all possible entries
		// allocate memory for NETRESOURCE structures.
		LPNETRESOURCE pnrLocal = (LPNETRESOURCE) malloc(cbBuffer);
		dwResultEnum = WNetEnumResource(hEnum, &cEntries, pnrLocal, &cbBuffer);

		if (dwResultEnum == NO_ERROR)
		{
			SOCmnString displayName;

			for (i = 0; i < cEntries; i++)
			{
				// check for domain or server name:
				HTREEITEM hContainer = hItem;

				if (pnrLocal[i].dwDisplayType == RESOURCEDISPLAYTYPE_SERVER)
				{
					// have a server!
					HTREEITEM hServer;
					SOCltServerBrowser* sb = new SODmCServerBrowser();
					sb->setNodeName(pnrLocal[i].lpRemoteName);
					displayName = (pnrLocal[i].lpRemoteName) + 2;
					displayName.makeLower(1);
					hServer = SODmCTreeInsert(&m_treeSrv, hItem, displayName, TRUE, (void*)(SOCltServerBrowser*)sb, SODMCMAINVIEW_IMG_REMOTE_MACHINE, SODMCMAINVIEW_IMG_REMOTE_MACHINE, TVI_SORT);
#ifdef SODMC_DA
					SODmCTreeInsert(&m_treeSrv, hServer, _T("Data Access V1"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA10A, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
					SODmCTreeInsert(&m_treeSrv, hServer, _T("Data Access V2"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA20, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
					SODmCTreeInsert(&m_treeSrv, hServer, _T("Data Access V3"), TRUE, (void*)SOCLT_BROWSE_SERVER_DA30, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
#ifdef SODMC_AE
					SODmCTreeInsert(&m_treeSrv, hServer, _T("Alarms and Events"), TRUE, (void*)SOCLT_BROWSE_SERVER_AE10, SODMCMAINVIEW_IMG_COM_CATEGORY, SODMCMAINVIEW_IMG_COM_CATEGORY, TVI_LAST);
#endif
					continue;
				}
				else if (pnrLocal[i].dwDisplayType == RESOURCEDISPLAYTYPE_DOMAIN)
				{
					// have a domain!
					displayName = (pnrLocal[i].lpRemoteName);
					displayName.makeLower(1);
					LPNETRESOURCE pnrDomain = (LPNETRESOURCE)malloc(sizeof(NETRESOURCE));
					*pnrDomain = pnrLocal[i];

					if (pnrLocal[i].lpLocalName)
					{
						pnrDomain->lpLocalName = _tcsdup(pnrLocal[i].lpLocalName);
					}

					if (pnrLocal[i].lpRemoteName)
					{
						pnrDomain->lpRemoteName = _tcsdup(pnrLocal[i].lpRemoteName);
					}

					if (pnrLocal[i].lpComment)
					{
						pnrDomain->lpComment = _tcsdup(pnrLocal[i].lpComment);
					}

					if (pnrLocal[i].lpProvider)
					{
						pnrDomain->lpProvider = _tcsdup(pnrLocal[i].lpProvider);
					}

					hContainer = SODmCTreeInsert(&m_treeSrv, hItem, displayName, TRUE, (void*)pnrDomain, SODMCMAINVIEW_IMG_DOMAIN, SODMCMAINVIEW_IMG_DOMAIN, TVI_SORT);
					continue;
				}

				// any other container (i.e network, ...)
				if ((pnrLocal[i].dwUsage & RESOURCEUSAGE_CONTAINER) != 0)
				{
					insertNetwork(&pnrLocal[i], hContainer);
				}
			}

			free((void*)pnrLocal);
		}
		else
		{
			free((void*)pnrLocal);

			if (dwResultEnum != ERROR_NO_MORE_ITEMS)
			{
				return;
			}

			break;
		}
	}
	while (dwResultEnum != ERROR_NO_MORE_ITEMS);

	dwResult = WNetCloseEnum(hEnum);
} // insertNetwork


//-----------------------------------------------------------------------------
// OnTreeSrvDI
//
// - item delete message
//
void SODmCMainView::OnTreeSrvDI(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 0;
	}

	NM_TREEVIEW* nmTV = (NM_TREEVIEW*)pNMHDR;
	int img, selImg;
	m_treeSrv.GetItemImage(nmTV->itemOld.hItem, img, selImg);

	// free stored parameters
	if ((img == SODMCMAINVIEW_IMG_MACHINE) || (img == SODMCMAINVIEW_IMG_REMOTE_MACHINE) ||
		(img == SODMCMAINVIEW_IMG_MACHINE_MANUAL))
	{
		SOCltServerBrowser* sb = (SOCltServerBrowser*)nmTV->itemOld.lParam;

		if (sb)
		{
			sb->release();
		}

		return;
	}

	if (img == SODMCMAINVIEW_IMG_OPCSERVER)
	{
		free((void*)nmTV->itemOld.lParam);
		return;
	}

	if (img == SODMCMAINVIEW_IMG_DOMAIN)
	{
		LPNETRESOURCE pnrDomain = (LPNETRESOURCE)nmTV->itemOld.lParam;

		if (pnrDomain->lpLocalName)
		{
			free(pnrDomain->lpLocalName);
		}

		if (pnrDomain->lpRemoteName)
		{
			free(pnrDomain->lpRemoteName);
		}

		if (pnrDomain->lpComment)
		{
			free(pnrDomain->lpComment);
		}

		if (pnrDomain->lpProvider)
		{
			free(pnrDomain->lpProvider);
		}

		free((void*)nmTV->itemOld.lParam);
		return;
	}
} // OnTreeSrvDI


//-----------------------------------------------------------------------------
// OnTreeSrvDC
//
// - double click message
//
void SODmCMainView::OnTreeSrvDC(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 1;
	}

	HTREEITEM hItem = m_treeSrv.GetSelectedItem();
	int img, selImg;
	m_treeSrv.GetItemImage(hItem, img, selImg);

	if ((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS) || (img == SODMCMAINVIEW_IMG_CLSID_MANUAL))
	{
		// double click on OPC server
		// add the server to the toolkit
		SODmCObjectView* objview = ::getObjectView();
		CLSID* clsid, locClsid;
		SOCmnString serverName;
		DWORD whatServer;
		SOCmnString nodeName;
		SOCmnString descr;
		SOCmnString progid;

		if ((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS))
		{
			SOCltServerBrowser* sb;

			// get the server data
			if (img == SODMCMAINVIEW_IMG_OPCSERVER)
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				HTREEITEM hParentParent = m_treeSrv.GetParentItem(hParent);
				sb = (SOCltServerBrowser*)m_treeSrv.GetItemData(hParentParent);
				clsid = (CLSID*)m_treeSrv.GetItemData(hItem);
				whatServer = m_treeSrv.GetItemData(hParent);
				serverName = m_treeSrv.GetItemText(hItem);
			}
			else
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				HTREEITEM hParentParent = m_treeSrv.GetParentItem(hParent);
				HTREEITEM hParentParentParent = m_treeSrv.GetParentItem(hParentParent);
				sb = (SOCltServerBrowser*)m_treeSrv.GetItemData(hParentParentParent);
				clsid = (CLSID*)m_treeSrv.GetItemData(hParent);
				whatServer = m_treeSrv.GetItemData(hParentParent);
				serverName = m_treeSrv.GetItemText(hParent);
			}

			nodeName = sb->getNodeName();

			if (!nodeName.isEmpty())
			{
				serverName = serverName + _T(" (") + nodeName + _T(")");
			}

			sb->getServerDescription(*clsid, descr);
			sb->getServerProgId(*clsid, progid);
		}
		else
		{
			USES_CONVERSION;
			SOCmnString itemText = m_treeSrv.GetItemText(hItem);

			if (NOERROR == ::CLSIDFromString(T2W(itemText), &locClsid))
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				serverName = m_treeSrv.GetItemText(hParent);
				nodeName = serverName;
				clsid = &locClsid;
				whatServer = m_treeSrv.GetItemData(hItem);
			}
			else
			{
				return;
			}
		}

		if (whatServer == SOCLT_BROWSE_SERVER_AE10)
		{
			// AE
#ifdef SODMC_AE
			// add server and subscription
			SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
			SOCmnPointer<SODmCComServerAE> server = (SODmCComServerAE*)entry->addServer(m_szURLSrv);
			server->setDescription(descr);
			server->setProgID(progid);
			server->setClientName(_T("Softing OPC Client"));
			SOCmnPointer<SOAeCSubscription> subs = server->addSubscription();
			entry->start(FALSE);
			server->start(TRUE, SOCLT_EXEC_ASYNC);  // activate
			objview->changeSelectedServerAE((SOAeCServer*)server);
#endif
		}
		else
		{
			// DA
#ifdef SODMC_DA
			// add server and default group
			SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
			SOCmnPointer<SODmCComServerDA> server = (SODmCComServerDA*)entry->addServer(m_szURLSrv);
			server->setDescription(descr);
			server->setProgID(progid);
			server->setClientName(_T("Softing OPC Client"));
			SOCmnPointer<SODaCGroup> group = server->addGroup(_T("group"), 1000);
			entry->start(FALSE);
			server->start(TRUE, SOCLT_EXEC_ASYNC);  // activate
			objview->changeSelectedServerDA((SODaCServer*)server);
#endif
		}

		getApp()->setDirtyFlag();
	}
} // OnTreeSrvDC


//-----------------------------------------------------------------------------
// OnTreeSrvCL
//
// - double click message
//
void SODmCMainView::OnTreeSrvCL(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 0;
	}

	m_szURLSrv.Empty();
	HTREEITEM hItem = m_treeSrv.GetSelectedItem();
	int img, selImg;
	m_treeSrv.GetItemImage(hItem, img, selImg);

	if ((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS) || (img == SODMCMAINVIEW_IMG_CLSID_MANUAL))
	{
		// click on OPC server
		// update the URL
		SODmCObjectView* objview = ::getObjectView();
		CLSID* clsid, locClsid;
		DWORD whatServer;
		SOCmnString nodeName;
		SOCmnString progid;
		SOCmnString classID;
		SOCmnString protocol;

		if ((img == SODMCMAINVIEW_IMG_OPCSERVER) || (img == SODMCMAINVIEW_IMG_OPCSERVER_PROPS))
		{
			SOCltServerBrowser* sb;

			// get the server data
			if (img == SODMCMAINVIEW_IMG_OPCSERVER)
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				HTREEITEM hParentParent = m_treeSrv.GetParentItem(hParent);
				sb = (SOCltServerBrowser*)m_treeSrv.GetItemData(hParentParent);
				clsid = (CLSID*)m_treeSrv.GetItemData(hItem);
				whatServer = m_treeSrv.GetItemData(hParent);
			}
			else
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				HTREEITEM hParentParent = m_treeSrv.GetParentItem(hParent);
				HTREEITEM hParentParentParent = m_treeSrv.GetParentItem(hParentParent);
				sb = (SOCltServerBrowser*)m_treeSrv.GetItemData(hParentParentParent);
				clsid = (CLSID*)m_treeSrv.GetItemData(hParent);
				whatServer = m_treeSrv.GetItemData(hParentParent);
			}

			nodeName = sb->getNodeName();
			sb->getServerProgId(*clsid, progid);
		}
		else
		{
			USES_CONVERSION;
			SOCmnString itemText = m_treeSrv.GetItemText(hItem);

			if (NOERROR == ::CLSIDFromString(T2W(itemText), &locClsid))
			{
				HTREEITEM hParent = m_treeSrv.GetParentItem(hItem);
				nodeName = m_treeSrv.GetItemText(hParent);
				clsid = &locClsid;
				whatServer = m_treeSrv.GetItemData(hItem);
			}
			else
			{
				return;
			}
		}

		if (whatServer == SOCLT_BROWSE_SERVER_AE10)
		{
			// AE
#ifdef SODMC_AE
			protocol = _T("opcae");
			classID.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), clsid->Data1, clsid->Data2, clsid->Data3, clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3], clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);

			if (nodeName .findOneOf(_T("\\")) >= 0)
			{
				nodeName = nodeName.Right(nodeName.GetLength() - 2);    // cut '\\' at the beginning if present
			}

			m_szURLSrv.Format(_T("%s://%s/%s/%s"), protocol, nodeName, progid, classID);
#endif
		}
		else
		{
			// DA
#ifdef SODMC_DA
			protocol = _T("opcda");
			classID.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), clsid->Data1, clsid->Data2, clsid->Data3, clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3], clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);

			if (nodeName .findOneOf(_T("\\")) >= 0)
			{
				nodeName = nodeName.Right(nodeName.GetLength() - 2);    // cut '\\' at the beginning if present
			}

			m_szURLSrv.Format(_T("%s://%s/%s/%s"), protocol, nodeName, progid, classID);
#endif
		}
	}

	UpdateData(FALSE);
} // OnTreeSrvCL


//-----------------------------------------------------------------------------
// OnBLE
//
// - begin editing tree label
//
void SODmCMainView::OnTreeSrvBLE(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!pResult)
	{
		return;
	}

	*pResult = TRUE;
	NMTVDISPINFO* nmDI = (NMTVDISPINFO*)pNMHDR;
	int img, selImg;
	m_treeSrv.GetItemImage(nmDI->item.hItem, img, selImg);

	if ((img == SODMCMAINVIEW_IMG_MACHINE_MANUAL) || (img == SODMCMAINVIEW_IMG_CLSID_MANUAL))
	{
		*pResult = FALSE;
	}
} // OnBLE


//-----------------------------------------------------------------------------
// OnELE
//
// - end editing tree label
//
void SODmCMainView::OnTreeSrvELE(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!pResult)
	{
		return;
	}

	*pResult = FALSE;
	NMTVDISPINFO* nmDI = (NMTVDISPINFO*)pNMHDR;
	int img, selImg;
	m_treeSrv.GetItemImage(nmDI->item.hItem, img, selImg);

	if (img == SODMCMAINVIEW_IMG_MACHINE_MANUAL)
	{
		SOCltServerBrowser* sb = (SOCltServerBrowser*)nmDI->item.lParam;
		sb->setNodeName(nmDI->item.pszText);

		if (m_treeSrv.ItemHasChildren(nmDI->item.hItem))
		{
			HTREEITEM hItem = m_treeSrv.GetChildItem(nmDI->item.hItem);

			while (hItem != NULL)
			{
				m_treeSrv.GetItemImage(hItem, img, selImg);

				if (img == SODMCMAINVIEW_IMG_COM_CATEGORY)
				{
					TVITEM  item;
					m_treeSrv.Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
					item.hItem = hItem;
					item.mask = TVIF_CHILDREN | TVIF_HANDLE;
					item.cChildren = TRUE;
					m_treeSrv.SetItem(&item);
				}

				hItem = m_treeSrv.GetNextSiblingItem(hItem);
			}
		}

		*pResult = TRUE;
	}

	if (img == SODMCMAINVIEW_IMG_CLSID_MANUAL)
	{
		CLSID clsid;
		USES_CONVERSION;

		if (NOERROR == ::CLSIDFromString(T2W(nmDI->item.pszText), &clsid))
		{
			*pResult = TRUE;
		}
	}
}

#endif // SODMC_DCOM


void SODmCMainView::onButtonAddServer(void)
{
	UpdateData(TRUE);
	SODmCObjectView* objview = ::getObjectView();
#ifdef SODMC_DA

	if ((_tcsncmp(m_szURLSrv, _T("opcae"), 5) != 0) && (_tcsncmp(m_szURLSrv, _T("tpae"), 4) != 0))
	{
		// add server and default group
		SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
		SOCmnPointer<SODaCServer> server = entry->addServer(m_szURLSrv);

		if (server.isNotNull())
		{
			server->setClientName(_T("Softing OPC Client"));
			SOCmnPointer<SODaCGroup> group = server->addGroup(_T("group"), 1000);
			entry->start(FALSE);
			server->start(TRUE, SOCLT_EXEC_ASYNC);  // activate
			objview->changeSelectedServerDA((SODaCServer*)server);
			getApp()->setDirtyFlag();
		}
	}

#endif
#ifdef SODMC_AE

	if ((_tcsncmp(m_szURLSrv, _T("opcae"), 5) == 0) || (_tcsncmp(m_szURLSrv, _T("tpae"), 4) == 0))
	{
		// add server and subscription
		SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
		SOCmnPointer<SOAeCServer> server = (SOAeCServer*)entry->addServer(m_szURLSrv);

		if (server.isNotNull())
		{
			server->setClientName(_T("Softing OPC Client"));
			SOCmnPointer<SOAeCSubscription> subs = server->addSubscription();
			entry->start(FALSE);
			server->start(TRUE, SOCLT_EXEC_ASYNC);  // activate
			objview->changeSelectedServerAE((SOAeCServer*)(SOAeCServer*)server);
			getApp()->setDirtyFlag();
		}
	}

#endif
}


#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// DA Browse Tab

//-----------------------------------------------------------------------------
// OnTreeDaIE
//
// - item expanding message
//
void SODmCMainView::OnTreeDaIE(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmTV = (NMTREEVIEW*)pNMHDR;

	if (pResult)
	{
		*pResult = 0;
	}

	if (TVE_EXPAND != (nmTV->action & TVE_EXPAND))
	{
		return;    // check if to expand
	}

	if (TVIS_EXPANDEDONCE & (nmTV->itemNew.state))
	{
		return;
	}

	HTREEITEM hItem = nmTV->itemNew.hItem;
	HTREEITEM hNewItem;
	SOCltBrowseObject* bobj = (SOCltBrowseObject*)nmTV->itemNew.lParam;
	SOCmnPointer<SODaCNameSpaceBrowser> broot = (SODaCNameSpaceBrowser*)bobj->getBrowser();
	DWORD cCnt = 0;
	SOCmnList<SOCltBrowseObject> objList;
	DWORD objType;
	SOCmnListPosition pos;
	SOCltBrowseObject* pObj;
	int img;
	SOCmnList<SOCltBrowseObject> propList;
	SOCmnListPosition posProp;
	SODaCProperty* pProp;
	DWORD pCnt;
	
	OPCNAMESPACETYPE nsType = OPC_NS_HIERARCHIAL;

	if (broot->getObjectState() != SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE)
	{
		// DA 1.0 and DA 2.0
		DWORD i;

		for (i = 0; i < 3; i++)
		{
#ifdef SODMC_DCOM

			if (i == 0)
			{
				// query organisation
				SOCmnPointer<SODaCServer> server = broot->getServer();

				if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
					HRESULT res;
					res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

					if (SUCCEEDED(res))
					{
						SOCmnPointer<SODaCServer> server = broot->getServer();

						if ((server->getClassID() == m_serialINATServerClsId) || (server->getClassID() == m_ethernetINATServerClsId))
						{
							broot->setForceBrowseUp(TRUE);
						}
						else
						{
							broot->setForceBrowseUp(FALSE);
						}

						
						server->beforeInterfaceCall();
						res = browseSAS->QueryOrganization(&nsType);
						server->afterInterfaceCall();
						if ((FAILED(res)))
						{
							SOCmnString et;
							DWORD errorcode = res;
							server->getErrorDescription(errorcode, et, 0x0800);
							insertError((SOCltElement*)server, res, errorcode, et);
							continue; // do not ask for nodes in flat namespace
						}
					}
				}
			}

#endif

			switch (i)
			{
			case 0:
				objType = SOCMNOBJECT_TYPE_NODE;
				img = SODMCMAINVIEW_IMG_NODE;
				break;

			case 1:
				objType = SOCMNOBJECT_TYPE_TAG;
				img = SODMCMAINVIEW_IMG_TAG;
				break;

			case 2:
				objType = SOCMNOBJECT_TYPE_PROPERTY;
				img = SODMCMAINVIEW_IMG_PROPERTY;
				break;
			}
			

			// browse all objects of the next level
			BOOL result = FALSE;
			if(nsType == OPC_NS_FLAT)
			{
				if(bobj->getObjectType() == SOCMNOBJECT_TYPE_NAMESPACE_BROWSER && i == 1)
				{
					result = ((SODaCComNameSpaceBrowser*)bobj)->expandFlat(objList);
				}
				if (bobj->getObjectType() == SOCMNOBJECT_TYPE_TAG && i == 2)
				{
					result = bobj->expand(objType, objList);
				}
			}
			else
			{
				result = bobj->expand(objType, objList);
			}
			
			if (result)
			{
				if (!objList.isEmpty())
				{
					if (::getApp()->getOptions()->alphaItemOn)
					{
						objList.sort(compareNames);
					}

					pos = objList.getStartPosition();

					while (pos)
					{
						pObj = objList.getNext(pos);
					
						if (i == 2)
						{
							// property
							pProp = (SODaCProperty*)pObj;

							if (pProp->getPropertyID() <= 6)
							{
								continue;    // don't display the default tag properties; no items could be created for them
							}
						}

						pObj->addRef();
						hNewItem = SODmCTreeInsert(&m_treeDA, hItem, pObj->getDisplayName(), pObj->isExpandable(), (void*)pObj, img, img, TVI_LAST);
						cCnt++;
						
						/*
						if (i == 1)
						{
							
							// tag
							// expand tags immediately
							pCnt = 0;

							if (pObj->expand(SOCMNOBJECT_TYPE_PROPERTY, propList))
							{
								if (::getApp()->getOptions()->alphaItemOn)
								{
									propList.sort(comparePropNames);
								}

								posProp = propList.getStartPosition();

								while (posProp)
								{
									pProp = (SODaCProperty*)propList.getNext(posProp);

									if (pProp->getPropertyID() > 6)
									{
										pProp->addRef();
										SODmCTreeInsert(&m_treeDA, hNewItem, pProp->getDisplayName(), pProp->isExpandable(), (void*)pProp, SODMCMAINVIEW_IMG_PROPERTY, SODMCMAINVIEW_IMG_PROPERTY, TVI_LAST);
										pCnt++;
									}
								}

								propList.destroy();
							}

							if (pCnt == 0)
							{
								// no children
								TVITEM  item;
								item.hItem = hNewItem;
								item.mask = TVIF_CHILDREN | TVIF_HANDLE;
								item.cChildren = FALSE;
								m_treeDA.SetItem(&item);
							}
							
							m_treeDA.SetItemState(hNewItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
							
						}
						*/
					}

					objList.destroy();
				}
			}
		}
	} // DA 1.0 and DA 2.0
	else
	{
		// DA 3.0
		DWORD i;
		SOCmnString eF, vF;
		// browse all objects of the next level
		broot->setReturnProperties(TRUE);
		eF = broot->getElementNameFilter();
		vF = broot->getVendorFilter();

		for (i = 0; i < 2; i++)
		{
			if (i == 0)
			{
				objType = SOCMNOBJECT_TYPE_NODE;
				broot->setFilter(_T(""), _T(""));
			}
			else
			{
				objType = SOCMNOBJECT_TYPE_ITEM;
				broot->setFilter(eF, vF);
			}

			if (bobj->expand(objType, objList))
			{
				if (::getApp()->getOptions()->alphaItemOn)
				{
					objList.sort(compareNames);
				}

				pos = objList.getStartPosition();

				while (pos)
				{
					pObj = objList.getNext(pos);

					switch (pObj->getObjectType())
					{
					case SOCMNOBJECT_TYPE_NODE:
						if (!((SODaCNode*)pObj)->isItem())
						{
							img = SODMCMAINVIEW_IMG_NODE;
						}
						else
						{
							img = SODMCMAINVIEW_IMG_NODE_ITEM;
						}

						break;

					case SOCMNOBJECT_TYPE_TAG:
						propList = (SOCmnList<SOCltBrowseObject> *)((SODaCTag*)pObj)->getPropertyList();
						img = SODMCMAINVIEW_IMG_TAG;
						break;
					}

					if (((i == 0) && (pObj->is(SOCMNOBJECT_TYPE_NODE))) ||
						((i == 1) && (pObj->is(SOCMNOBJECT_TYPE_TAG))))
					{
						pObj->addRef();
						hNewItem = SODmCTreeInsert(&m_treeDA, hItem, pObj->getDisplayName(), pObj->isExpandable(), (void*)pObj, img, img, TVI_LAST);
						cCnt++;

						if ((propList.isInit()) && (pObj->is(SOCMNOBJECT_TYPE_TAG)))
						{
							// properties of tags become expanded immediately
							if (::getApp()->getOptions()->alphaItemOn)
							{
								propList.sort(comparePropNames);
							}

							pCnt = 0;
							posProp = propList.getStartPosition();

							while (posProp)
							{
								pProp = (SODaCProperty*)propList.getNext(posProp);

								if (pProp->getPropertyID() <= 99)
								{
									continue;    // don't display the default tag properties; no items could be created for them
								}

								pProp->addRef();
								SODmCTreeInsert(&m_treeDA, hNewItem, pProp->getDisplayName(), pProp->isExpandable(), (void*)pProp, SODMCMAINVIEW_IMG_PROPERTY, SODMCMAINVIEW_IMG_PROPERTY, TVI_LAST);
								pCnt++;
							}

							propList.detach();

							if (pCnt == 0)
							{
								// no children
								TVITEM  item;
								item.hItem = hNewItem;
								item.mask = TVIF_CHILDREN | TVIF_HANDLE;
								item.cChildren = FALSE;
								m_treeDA.SetItem(&item);
							}

							m_treeDA.SetItemState(hNewItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
						}
					}
				}
			}

			objList.destroy();
		}

		if (bobj->is(SOCMNOBJECT_TYPE_NODE))
		{
			propList = (SOCmnList<SOCltBrowseObject> *)((SODaCNode*)bobj)->getPropertyList();

			if (propList.isInit())
			{
				// add the properties of the node object (retrieved in previous browse step)
				if (::getApp()->getOptions()->alphaItemOn)
				{
					propList.sort(comparePropNames);
				}

				posProp = propList.getStartPosition();

				while (posProp)
				{
					pProp = (SODaCProperty*)propList.getNext(posProp);

					if (pProp->getPropertyID() <= 99)
					{
						continue;    // don't display the default tag properties; no items could be created for them
					}

					pProp->addRef();
					SODmCTreeInsert(&m_treeDA, hItem, pProp->getDisplayName(), pProp->isExpandable(), (void*)pProp, SODMCMAINVIEW_IMG_PROPERTY, SODMCMAINVIEW_IMG_PROPERTY, TVI_LAST);
					cCnt++;
				}

				propList.detach();
			}
		}
	} // DA 3.0

	if (cCnt == 0)
	{
		// no children
		TVITEM  item;
		item.hItem = hItem;
		item.mask = TVIF_CHILDREN | TVIF_HANDLE;
		item.cChildren = FALSE;
		m_treeDA.SetItem(&item);
	}
} // OnTreeDaIE


//-----------------------------------------------------------------------------
// OnTreeDaDI
//
// - item delete message
//
void SODmCMainView::OnTreeDaDI(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 0;
	}

	NM_TREEVIEW* nmTV = (NM_TREEVIEW*)pNMHDR;
	// free server browser
	SOCltBrowseObject* bobj = (SOCltBrowseObject*)nmTV->itemOld.lParam;

	if (bobj)
	{
		bobj->release();
	}
} // OnTreeDaDI


//-----------------------------------------------------------------------------
// OnTreeDaDC
//
// - double click message
//
void SODmCMainView::OnTreeDaDC(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 1;
	}

	HTREEITEM hItem = m_treeDA.GetSelectedItem();
	int img, selImg;
	m_treeDA.GetItemImage(hItem, img, selImg);

	if ((img == SODMCMAINVIEW_IMG_NODE_ITEM) || (img == SODMCMAINVIEW_IMG_NODE) || (img == SODMCMAINVIEW_IMG_TAG) || (img == SODMCMAINVIEW_IMG_PROPERTY))
	{
		// clicked on tag or property
		SODmCObjectView* objview = ::getObjectView();
		SOCmnPointer<SODaCGroup> curSelGrp = objview->getSelectedGroup();

		if (curSelGrp.isNotNull())
		{
			SOCmnString path;
			// add DA item to the selected DA group
			SOCltBrowseObject* bobj = (SOCltBrowseObject*)m_treeDA.GetItemData(hItem);
			path = bobj->getPath();

			if (!path.isEmpty())
			{
				SOCmnPointer<SODmCItem> item = (SODmCItem*)curSelGrp->addItem(path, VT_EMPTY, bobj->getItemPath());
				item->start(SOCLT_EXEC_ASYNC);
				getApp()->setDirtyFlag();
			}
		}
	}
} // OnTreeDaDC


//-----------------------------------------------------------------------------
// changeSelectedServerDA
//
// - the selected DA server in the object view has changed
//
void SODmCMainView::changeSelectedServerDA(
	IN SODaCServer* server, // new selected server
	IN BOOL refresh)
{
	if (((SODaCServer*)m_treeDAServer != server) || (refresh))
	{
		// realy changed
		// clear browsing tree
		m_treeDA.DeleteAllItems();
		m_treeDAServer = server;
		m_treeDAServer.addRef();

		if (m_treeDAServer.isNotNull())
		{
			if (m_treeDAServer->getObjectState() != SOCltElement::disconnected)
			{
				SODmCCmnServerDA* pCmnSrv = NULL;
#ifdef SODMC_DCOM

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
#ifdef SODMC_SOAP

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_SOAP)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (m_treeDAServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)(SODaCServer*)m_treeDAServer;
				}

#endif
				SODmCObjectView* objview = ::getObjectView();
				SODaCNameSpaceBrowser* browser = m_treeDAServer->getNameSpaceBrowser();
				SOCmnString stringFilter, vendorFilter;
				stringFilter = pCmnSrv->getStringFilter();
				vendorFilter = pCmnSrv->getVendorFilter();

				if ((pCmnSrv->getDatatypeFilter() != VT_EMPTY) || (pCmnSrv->getAccessRightsFilter() != 0) || ((!stringFilter.isEmpty()) && (m_treeDAServer->getSupportedOPCVersion() < SODAC_VERSION_OPC_DAV30)))
				{
					browser->setFilter(stringFilter, pCmnSrv->getAccessRightsFilter(), pCmnSrv->getDatatypeFilter());
				}
				else if (((!stringFilter.isEmpty()) && (m_treeDAServer->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)) || (!vendorFilter.isEmpty()))
				{
					browser->setFilter(stringFilter, vendorFilter);
				}

				SODmCTreeInsert(&m_treeDA, TVI_ROOT, objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()), TRUE, (void*)(SODaCNameSpaceBrowser*)browser, SODMCMAINVIEW_IMG_SRV_DA, SODMCMAINVIEW_IMG_SRV_DA, TVI_FIRST);
			}
			else
			{
				m_treeDAServer.release();
			}
		}

		if (m_tab.GetCurSel() ==  SODMCMAINVIEW_TAB_TDA)
		{
			if (m_treeDAServer.isNotNull())
			{
				m_treeDA.ShowWindow(SW_SHOW);
			}
			else
			{
				m_treeDA.ShowWindow(SW_HIDE);
			}
		}
	}
} // changeSelectedServerDA




//-----------------------------------------------------------------------------
// DA Items Tab

//-----------------------------------------------------------------------------
// OnListDaIC
//
// - item (selection) changed message
//
void SODmCMainView::OnListDaIC(
	IN NMHDR* pNMHDR,       // parameter
	OUT LRESULT* pResult)   // result
{
	NM_LISTVIEW* nmListView = (NM_LISTVIEW*)pNMHDR;

	if (((nmListView->uChanged & LVIF_STATE) == LVIF_STATE) &&
		((nmListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED))
	{
		if (m_listDA.GetSelectedCount() == 1)
		{
			SOCmnPointer<SODmCItem> item = getSelectedItem();
			SODmCObjectView* objview = ::getObjectView();
			objview->changeSelectedItem(item);
		}
	}
} // OnListDaIC

//-----------------------------------------------------------------------------
// clearSelection
//
// - clears the selection
//
void SODmCMainView::clearSelection()
{
	int iCnt = m_listDA.GetItemCount();

	for (int iIdx = 0; iIdx < iCnt; iIdx++)
	{
		UINT uiState = m_listDA.GetItemState(iIdx, LVIS_SELECTED);
		m_listDA.SetItemState(iIdx, 0, LVIS_SELECTED);
	}
}
//-----------------------------------------------------------------------------
// changeSelectedItem
//
// - change the selected DA item
//
void SODmCMainView::changeSelectedItem(
	IN SODmCItem* item) // new selected DA item
{
	if ((SODmCItem*)m_listDAItem != item)
	{
		// selection has changed
		m_listDAItem = item;
		m_listDAItem.addRef();

		if (m_listDAItem.isNotNull())
		{
			m_listDA.SetItemState(m_listDAItem->getListIndex(), LVIS_SELECTED, LVIS_SELECTED);
		}
	}
} // changeSelectedItem


//-----------------------------------------------------------------------------
// updateListIndex
//
// - update the GUI Items list index
//
void SODmCMainView::updateListDAIndex(
	IN int startIdx)    // start index for update
{
	SODmCItem* item;
	int i;

	for (i = startIdx; i < m_listDA.GetItemCount(); i++)
	{
		item = (SODmCItem*)(SOCltElement*)m_listDA.GetItemData(i);
		item->setListIndex(i);
	}
} // updateListIndex


//-----------------------------------------------------------------------------
// getSelectedItem
//
// - get the selected item in the DA Items list
//
// returns:
//      selected item
//      NULL - no item selected
//
SODmCItem* SODmCMainView::getSelectedItem(void)
{
	int index = -1;
	UINT state;

	while ((index = m_listDA.GetNextItem(index, LVNI_ALL)) > -1)
	{
		state = m_listDA.GetItemState(index, LVIS_SELECTED);

		if (state & LVIS_SELECTED)
		{
			// item is selected
			SODmCItem* item = (SODmCItem*)(SOCltElement*)m_listDA.GetItemData(index);

			if (item)
			{
				item->addRef();
			}

			return item;
		}
	}

	return NULL;
} // getSelectedItem


void SODmCMainView::updateItemProperties(void)
{
	SOCmnPointer<SODaCEntry> entryDA = getSODaCEntry();
	SOCmnList<SODaCServer> serverList((SOCmnList<SODaCServer>*)entryDA->getBranchList());
	SOCmnListPosition posS;
	SODaCServer* pServer;
	SOCmnList<SODaCGroup> groupList;
	SOCmnListPosition posG;
	SODaCGroup* pGroup;
#ifdef SODMC_DCOM
	SODmCComGroup* pComObj;
#endif
	posS = serverList.getStartPosition();

	while (posS)
	{
		pServer = serverList.getNext(posS);
		groupList = (SOCmnList<SODaCGroup> *)pServer->getBranchList();
		posG = groupList.getStartPosition();

		while (posG)
		{
			pGroup = groupList.getNext(posG);
			pGroup->updateItems(TRUE, (SOCmnList<SODaCItem>*)pGroup->getLeafList());
#ifdef SODMC_DCOM

			if (pGroup->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				pComObj = (SODmCComGroup*)pGroup;
				pComObj->validateItems((SOCmnList<SODaCItem>*)pGroup->getLeafList());
			}

#endif // SODMC_DCOM
		}
	}

	getMainFrame()->updateDAWriteBar();
}


#endif

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// AE Browse Tab

//-----------------------------------------------------------------------------
// OnTreeAeIE
//
// - item expanding message
//
void SODmCMainView::OnTreeAeIE(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmTV = (NMTREEVIEW*)pNMHDR;

	if (pResult)
	{
		*pResult = 0;
	}

	if (TVE_EXPAND != (nmTV->action & TVE_EXPAND))
	{
		return;    // check if to expand
	}

	if (TVIS_EXPANDEDONCE & (nmTV->itemNew.state))
	{
		return;
	}

	HTREEITEM hItem = nmTV->itemNew.hItem;
	HTREEITEM hNewItem;
	SOCltBrowseObject* bobj = (SOCltBrowseObject*)nmTV->itemNew.lParam;
	SOCmnList<SOCltBrowseObject> objList;
	SOCmnList<SOCltBrowseObject> subObjList;
	BOOL end = FALSE;
	BOOL ok;
	SOCmnListPosition pos, posSubObj;
	SOCltBrowseObject* pObj, *pSubObj;
	int img, subImg;
	DWORD cCnt = 0;
	DWORD pCnt;
	DWORD loopCnt = 0;

	while (!end)
	{
		switch (bobj->getObjectType())
		{
		case SOCMNOBJECT_TYPE_EVENT_BROWSER:
			ok = bobj->expand(SOCMNOBJECT_TYPE_CATEGORY, objList);
			img = SODMCMAINVIEW_IMG_CATEGORY;
			end = TRUE;
			break;

		case SOCMNOBJECT_TYPE_CATEGORY:
			if (loopCnt == 0)
			{
				ok = bobj->expand(SOCMNOBJECT_TYPE_CONDITION_CLASS, objList);
				img = SODMCMAINVIEW_IMG_CONDITION_CLASS;
				subImg = SODMCMAINVIEW_IMG_SUBCONDITION_CLASS;
			}
			else
			{
				ok = bobj->expand(SOCMNOBJECT_TYPE_ATTRIBUTE, objList);
				img = SODMCMAINVIEW_IMG_ATTRIBUTE;
				end = TRUE;
			}

			break;

		case SOCMNOBJECT_TYPE_AREA_BROWSER:
		case SOCMNOBJECT_TYPE_AREA:
			if (loopCnt == 0)
			{
				ok = bobj->expand(SOCMNOBJECT_TYPE_AREA, objList);
				img = SODMCMAINVIEW_IMG_AREA;
			}
			else
			{
				ok = bobj->expand(SOCMNOBJECT_TYPE_SOURCE, objList);
				end = TRUE;
				img = SODMCMAINVIEW_IMG_SOURCE;
				subImg = SODMCMAINVIEW_IMG_CONDITION;
			}

			break;

		case SOCMNOBJECT_TYPE_SOURCE:
			ok = bobj->expand(SOCMNOBJECT_TYPE_CONDITION, objList);
			img = SODMCMAINVIEW_IMG_CONDITION;
			end = TRUE;
			break;

		case SOCMNOBJECT_TYPE_CONDITION_CLASS:
			ok = bobj->expand(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS, objList);
			img = SODMCMAINVIEW_IMG_SUBCONDITION_CLASS;
			end = TRUE;
			break;

		default:
			ok = FALSE;
			end = TRUE;
			break;
		}

		loopCnt++;

		// browse all objects of the next level
		if (ok)
		{
			if (!objList.isEmpty())
			{
				if (::getApp()->getOptions()->alphaAEOn)
				{
					objList.sort(compareNames);
				}

				pos = objList.getStartPosition();

				while (pos)
				{
					pObj = objList.getNext(pos);
					pObj->addRef();
					hNewItem = SODmCTreeInsert(&m_treeAE, hItem, pObj->getDisplayName(), pObj->isExpandable(), (void*)pObj, img, img, TVI_LAST);
					cCnt++;

					if ((pObj->is(SOCMNOBJECT_TYPE_SOURCE)) || (pObj->is(SOCMNOBJECT_TYPE_CONDITION_CLASS)))
					{
						// expand immediately
						pCnt = 0;

						if (pObj->expand(SOCMNOBJECT_TYPE_ALL, subObjList))
						{
							if (::getApp()->getOptions()->alphaAEOn)
							{
								subObjList.sort(compareNames);
							}

							if ((pObj->is(SOCMNOBJECT_TYPE_SOURCE)) ||
								(subObjList.getCount() > 1))
							{
								posSubObj = subObjList.getStartPosition();

								while (posSubObj)
								{
									pSubObj = subObjList.getNext(posSubObj);
									pSubObj->addRef();
									SODmCTreeInsert(&m_treeAE, hNewItem, pSubObj->getDisplayName(), pSubObj->isExpandable(), (void*)pSubObj, subImg, subImg, TVI_LAST);
									pCnt++;
								}
							}

							subObjList.destroy();
						}

						if (pCnt == 0)
						{
							// no children
							TVITEM  item;
							item.hItem = hNewItem;
							item.mask = TVIF_CHILDREN | TVIF_HANDLE;
							item.cChildren = FALSE;
							m_treeAE.SetItem(&item);
						}

						m_treeAE.SetItemState(hNewItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);
					}
				}

				objList.destroy();
			}
		}
	}

	if (cCnt == 0)
	{
		// no children
		TVITEM  item;
		item.hItem = hItem;
		item.mask = TVIF_CHILDREN | TVIF_HANDLE;
		item.cChildren = FALSE;
		m_treeAE.SetItem(&item);
	}
} // OnTreeAeIE


//-----------------------------------------------------------------------------
// OnTreeAeDI
//
// - item delete message
//
void SODmCMainView::OnTreeAeDI(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 0;
	}

	NM_TREEVIEW* nmTV = (NM_TREEVIEW*)pNMHDR;
	// free server browser
	SOCltBrowseObject* bobj = (SOCltBrowseObject*)nmTV->itemOld.lParam;

	if (bobj)
	{
		bobj->release();
	}
} // OnTreeAeDI


//-----------------------------------------------------------------------------
// changeSelectedSubscription
//
// - the selected AE subscription in the object view has changed
//
void SODmCMainView::changeSelectedSubscription(
	IN SOAeCSubscription* subs, // new selected subscription
	IN BOOL refresh)
{
	if (((SOAeCSubscription*)m_treeAESubscription != subs) || (refresh))
	{
		// realy changed
		// clear browsing tree
		m_treeAE.DeleteAllItems();
		m_treeAESubscription = subs;
		m_treeAESubscription.addRef();

		if (m_treeAESubscription.isNotNull())
		{
			if (m_treeAESubscription->getObjectState() != SOCltElement::disconnected)
			{
				SOCmnPointer<SOAeCServer> treeAEServer = (SOAeCServer*)m_treeAESubscription->getParentBranch();
				SODmCCmnServerAE* pCmnSrv = NULL;

				if (treeAEServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)(SOAeCServer*)treeAEServer;
				}

#ifdef SOFEATURE_TUNNEL
				else
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)(SOAeCServer*)treeAEServer;
				}

#endif
				SODmCObjectView* objview = getObjectView();
				SOAeCEventBrowser* eBrowser = treeAEServer->getEventBrowser();
				SOAeCAreaBrowser* aBrowser = treeAEServer->getAreaBrowser();
				aBrowser->setFilter(pCmnSrv->getStringFilter());
				SOCmnString brName;
				brName.format(_T("%s - Event space"), objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()));
				SODmCTreeInsert(&m_treeAE, TVI_ROOT, brName, TRUE, (void*)(SOAeCEventBrowser*)eBrowser, SODMCMAINVIEW_IMG_SRV_AE, SODMCMAINVIEW_IMG_SRV_AE, TVI_FIRST);
				brName.format(_T("%s - Area space"), objview->GetTreeCtrl().GetItemText(pCmnSrv->getTreeHandle()));
				SODmCTreeInsert(&m_treeAE, TVI_ROOT, brName, TRUE, (void*)(SOAeCAreaBrowser*)aBrowser, SODMCMAINVIEW_IMG_SRV_AE, SODMCMAINVIEW_IMG_SRV_AE, TVI_FIRST);
			}
			else
			{
				m_treeAESubscription.release();
			}
		}

		if (m_tab.GetCurSel() ==  SODMCMAINVIEW_TAB_TAE)
		{
			if (m_treeAESubscription.isNotNull())
			{
				m_treeAE.ShowWindow(SW_SHOW);
			}
			else
			{
				m_treeAE.ShowWindow(SW_HIDE);
			}
		}
	}
} // changeSelectedSubscription



//-----------------------------------------------------------------------------
// AE Events Tab

//-----------------------------------------------------------------------------
// OnListAeDI
//
// - item delete message
//
void SODmCMainView::OnListAeDI(
	IN NMHDR* pNMHDR,       // parameter
	OUT LRESULT* pResult)   // result
{
	NM_LISTVIEW* nmListView = (NM_LISTVIEW*)pNMHDR;
	SODmCEvent* event = (SODmCEvent*)m_listAE.GetItemData(nmListView->iItem);

	if (event)
	{
		event->release();
	}
} // OnListAeDI




//-----------------------------------------------------------------------------
// AE Conditions Tab

//-----------------------------------------------------------------------------
// OnListAecDI
//
// - item delete message
//
void SODmCMainView::OnListAecDI(
	IN NMHDR* pNMHDR,       // parameter
	OUT LRESULT* pResult)   // result
{
	NM_LISTVIEW* nmListView = (NM_LISTVIEW*)pNMHDR;
	SODmCEvent* event = (SODmCEvent*)m_listAEC.GetItemData(nmListView->iItem);

	if (event)
	{
		event->release();
	}
} // OnListAeDI


//-----------------------------------------------------------------------------
// getSelectedCondition
//
// - get the selected item in the DA Items list
//
// returns:
//      selected item
//      NULL - no item selected
//
SODmCEvent* SODmCMainView::getSelectedCondition(void)
{
	int index = -1;
	UINT state;

	while ((index = m_listAEC.GetNextItem(index, LVNI_ALL)) > -1)
	{
		state = m_listAEC.GetItemState(index, LVIS_SELECTED);

		if (state & LVIS_SELECTED)
		{
			// item is selected
			SODmCEvent* event = (SODmCEvent*)(SOCltElement*)m_listAEC.GetItemData(index);

			if (event)
			{
				event->addRef();
			}

			return event;
		}
	}

	return NULL;
} // getSelectedCondition


//-----------------------------------------------------------------------------
// OnListAecDC
//
// - double click message
//
void SODmCMainView::OnListAecDC(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 1;
	}

	SOCmnPointer<SODmCEvent> event = getSelectedCondition();

	if (event.isNotNull())
	{
		SOCmnPointer<SOAeCServer> server = (SOAeCServer*)event->m_subs->getParentBranch();
		SOCmnList<SOAeCEvent> eventList;
		eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT);
		eventList.add(event);
		server->ackConditions(&eventList, NULL, NULL, SOCLT_EXEC_ASYNC);
	}
} // OnListAecDC

#endif

//-----------------------------------------------------------------------------
// onAcknowledge
//
// - menu handler for acknowledge conditions
//
afx_msg void SODmCMainView::onAcknowledge(void)
{
#ifdef SODMC_AE

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LAEC)
	{
		SOCmnPointer<SOAeCServer> listServer;
		SOCmnPointer<SOAeCServer> server;
		SOCmnList<SOAeCEvent> eventList;
		int index = -1;
		UINT state;
		eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT);

		while ((index = m_listAEC.GetNextItem(index, LVNI_ALL)) > -1)
		{
			state = m_listAEC.GetItemState(index, LVIS_SELECTED);

			if (state & LVIS_SELECTED)
			{
				// item is selected
				SODmCEvent* event = (SODmCEvent*)(SOCltElement*)m_listAEC.GetItemData(index);

				if (event)
				{
					server = (SOAeCServer*)event->m_subs->getParentBranch();

					if ((SOAeCServer*)server != (SOAeCServer*)listServer)
					{
						if ((listServer.isNotNull()) && (!eventList.isEmpty()))
						{
							listServer->ackConditions(&eventList, NULL, NULL, SOCLT_EXEC_ASYNC);
						}

						eventList.removeAll();
						listServer = server;
					}

					eventList.add(event);
				}
			}
		}

		if ((listServer.isNotNull()) && (!eventList.isEmpty()))
		{
			listServer->ackConditions(&eventList, NULL, NULL, SOCLT_EXEC_ASYNC);
		}
	}

#endif
} // onAcknowledge


//-----------------------------------------------------------------------------
// onUpdAcknowledge
//
// - menu update handler for acknowledge conditions
//
afx_msg void SODmCMainView::onUpdAcknowledge(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_AE

	if (m_tab.GetCurSel() == SODMCMAINVIEW_TAB_LAEC)
	{
		SOCmnPointer<SODmCEvent> cond(getSelectedCondition());

		if (cond.isNotNull())
			if (cond->m_subs.isNotNull())
			{
				pCmdUI->Enable(cond->m_subs->isConnected());
				return;
			}
	}

#endif
	pCmdUI->Enable(FALSE);
} // onUpdAcknowledge




//-----------------------------------------------------------------------------
// Errors Tab

//-----------------------------------------------------------------------------
// insertError
//
// - insert error to the error list
//
void SODmCMainView::insertError(
	IN SOCltElement* obj,   // object
	IN DWORD result,        // result
	IN DWORD errorCode,     // toolkit error code
	IN LPCTSTR errorText)   // toolkit error text
{
	// filter usual error codes
	if ((errorCode == SODAC_ERR_SRV_BRS_ACCESS_PATHS) ||
		(errorCode == SODAC_ERR_GRP_ITM_GET_DEADBAND))
	{
		return;
	}

	// filter another usual error code - improvement of speed, should not display EU properties errors
	if (errorCode == SODAC_ERR_SRV_GET_PROPERTIES)
	{
		return;
	}

	SOCmnPointer<SOCltServer> srv;
	SOCmnString objPath;
	SOCmnString conv;
	SOCmnString et(errorText);
	FILETIME now;

	switch (obj->getObjectType())
	{
	case SOCMNOBJECT_TYPE_SERVER_DA:
	case SOCMNOBJECT_TYPE_SERVER_AE:
		objPath = ((SOCltServer*)obj)->getURL();
		srv = (SOCltServer*)obj;
		srv.addRef();
		break;
#ifdef SODMC_DA

	case SOCMNOBJECT_TYPE_GROUP:
	{
		srv = (SOCltServer*)obj->getParentBranch();

		if (srv.isNotNull())
		{
			objPath.format(_T("%s°%s"), srv->getURL(), obj->getName());
		}
		else
		{
			objPath.format(_T("°%s"), obj->getName());
		}
	}
	break;

	case SOCMNOBJECT_TYPE_ITEM:
	{
		SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)obj->getParentBranch();

		if (grp.isNotNull())
		{
			srv = (SOCltServer*)grp->getParentBranch();

			if (srv.isNotNull())
			{
				objPath.format(_T("%s°%s°%s"), srv->getURL(), grp->getName(), ((SODaCItem*)obj)->getItemID());
			}
			else
			{
				objPath.format(_T("°%s°%s"), grp->getName(), ((SODaCItem*)obj)->getItemID());
			}
		}
		else
		{
			objPath.format(_T("°°%s"), ((SODaCItem*)obj)->getItemID());
		}
	}
	break;
#endif
#ifdef SODMC_AE

	case SOCMNOBJECT_TYPE_SUBSCRIPTION:
	{
		SOCmnString label;
#ifdef SODMC_DCOM

		if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			label = ((SODmCComSubscription*)obj)->getLabel();
		}

#endif
#ifdef SOFEATURE_TUNNEL

		if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			label = ((SODmCTPSubscription*)obj)->getLabel();
		}

#endif
		srv = (SOCltServer*)obj->getParentBranch();

		if (srv.isNotNull())
		{
			objPath.format(_T("%s°%s"), srv->getURL(), label);
		}
		else
		{
			objPath.format(_T("°%s"), label);
		}
	}
	break;
#endif
#ifdef SODMC_DCOM

	case SOCMNOBJECT_TYPE_OTHER:    // server browser
	{
		SOCmnString nodeName = ((SOCltServerBrowser*)obj)->getNodeName();

		if (nodeName.isEmpty())
		{
			objPath = _T("Server Browser");
		}
		else
		{
			objPath.format(_T("Server Browser on %s"), nodeName);
		}
	}

#endif // SODMC_DCOM
	}

	et.replace(_T("\r\n"), _T(" "));
	int idx = SODmCListPush(&m_listErr, objPath, NULL, SODMCMAINVIEW_IMG_ERROR);
	CoFileTimeNow(&now);
	FILETIME2STRING(&conv, &now);
	m_listErr.SetItemText(idx, 1, conv);
	m_listErr.SetItemText(idx, 2, et);
	conv.format(_T("0x%8.8X"), result);
	m_listErr.SetItemText(idx, 3, conv);
	getResultText(srv, result, conv);
	m_listErr.SetItemText(idx, 4, conv);
	conv.format(_T("0x%8.8X"), errorCode);
	m_listErr.SetItemText(idx, 5, conv);
	DWORD itmCnt = m_listErr.GetItemCount();

	if (itmCnt > 70)
	{
		DWORD i;

		for (i = itmCnt; i > 70; i--)
		{
			m_listErr.DeleteItem(i);
		}
	}
} // insertError

static BOOL g_callingServer = FALSE;

//-----------------------------------------------------------------------------
// getResultText
//
// - get error string for an error code
//
void SODmCMainView::getResultText(
	IN SOCltServer* srv,            // server
	IN HRESULT result,              // error code
	OUT SOCmnString& string)    // error string
{	
#ifdef SODMC_DCOM
	
	g_callingServer = FALSE;

	if ((srv) && (!g_callingServer))
	{
		if (srv->isConnected())
		{
			switch (result)
			{
			case 0x800706be:
			case 0x800706ba:
			case 0x800706bf:
				break;
			default:

				g_callingServer = TRUE;

				if (SUCCEEDED(srv->getErrorString(result, &string)))
				{
					g_callingServer = FALSE;
					return;
				}

				g_callingServer = FALSE;
				break;
			}
		}
	}

#endif // SODMC_DCOM
	// search for system errorcodes
	LPVOID msgBuf = NULL;
	DWORD formatResult = 0;
	formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
								 NULL, result, LANGIDFROMLCID(MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT)), (LPTSTR) &msgBuf, 0, NULL);

	if (formatResult != 0)
	{
		string = (LPTSTR)msgBuf;
		string.replace(_T("\r\n"), _T(""));
		LocalFree(msgBuf);
	}
	else
	{
		string.empty();
	}
} // getErrorString


//-----------------------------------------------------------------------------
// onErrDeleteAll
//
// - delete all entries of the error list
//
afx_msg void SODmCMainView::onErrDeleteAll(void)
{
	m_listErr.DeleteAllItems();
}

//-----------------------------------------------------------------------------
// onUpdErrDeleteAll
//
// - menu update handler for error list
//
afx_msg void SODmCMainView::onUpdErrDeleteAll(OUT CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_listErr.GetItemCount() > 0);
}

//-----------------------------------------------------------------------------
// onEnter
//
// - Handler for Hit Enter message
//
afx_msg LONG SODmCMainView::onEnter(IN UINT wParam, IN LONG lParam)
{
	if (IDC_EDIT_URL == wParam)
	{
		onButtonAddServer();
	}

	return 0;
} // onEnter


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// readGrouped
//
// - reads all selected items group by group
//
void SODmCMainView::readGrouped()
{
	SOCmnList<SODmCItemList>lstGroupList;
	lstGroupList.create(SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_MAP);
	POSITION pos = m_listDA.GetFirstSelectedItemPosition();

	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_listDA.GetNextSelectedItem(pos);
			SODaCItem* pItem = (SODaCItem*)m_listDA.GetItemData(nItem);

			if ((pItem->getAccessRights() & OPC_READABLE) && (pItem->isConnected()))
			{
				SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)pItem->getParentBranch();
				SODmCItemList* pList = (SODmCItemList*) lstGroupList.findKey(grp->getOpcHandle());

				if (pList != NULL)
				{
					pList->m_itemList.add(pItem);
				}
				else
				{
					pList = new SODmCItemList();

					if (pList != NULL)
					{
						pList->m_itemList.add(pItem);
						pList->m_group = grp;
					}

					lstGroupList.add(pList, grp->getOpcHandle());
				}
			}
		}
	}

	SOCmnListPosition listpos = lstGroupList.getStartPosition();

	while (listpos) // loop for each group
	{
		SODmCItemList* pList = lstGroupList.GetNext(listpos);
		SODaCGroup* pGrp = pList->m_group;

		if (pGrp->getDataRetrieval() == SODaCGroup::drAsyncAdvise)
		{
			pGrp->asyncRead(&pList->m_itemList);
		}
		else //synched
		{
			pGrp->syncRead(&pList->m_itemList);
		}
	}

	lstGroupList.destroy();
}


//-----------------------------------------------------------------------------
// performGroupedStateTransition
//
// - sets connection state for each selected item over all groups
//
void SODmCMainView::performGroupedStateTransition(BYTE state)
{
	SOCmnList<SODmCItemList>lstGroupList;
	lstGroupList.create(SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_MAP);
	POSITION pos = m_listDA.GetFirstSelectedItemPosition();

	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_listDA.GetNextSelectedItem(pos);
			SODaCItem* pItem = (SODaCItem*)m_listDA.GetItemData(nItem);

			if (pItem != NULL)
			{
				pItem->setTargetObjectState(state);
				SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)pItem->getParentBranch();
				SODmCItemList* pList = (SODmCItemList*) lstGroupList.findKey(grp->getOpcHandle());

				if (pList != NULL)
				{
					pList->m_itemList.add(pItem);
				}
				else
				{
					pList = new SODmCItemList();

					if (pList != NULL)
					{
						pList->m_itemList.add(pItem);
						pList->m_group = grp;
					}

					lstGroupList.add(pList, grp->getOpcHandle());
				}
			}
		}
	}

	SOCmnListPosition listpos = lstGroupList.getStartPosition();

	while (listpos) // loop for each group
	{
		SODmCItemList* pList = lstGroupList.GetNext(listpos);
		SODaCGroup* pGrp = pList->m_group;
		pGrp->performStateTransition(TRUE, FALSE, NULL);
	}

	lstGroupList.destroy();
}

#endif SODMC_DA




