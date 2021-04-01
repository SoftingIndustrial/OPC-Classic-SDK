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
//  Filename    : SODmCPropFrm.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Property Frame, Sheet and Page base classes                 |
//                - SODmCPropertyFrame: Property frame class                  |
//                - SODmCPSheet: Property sheet of the demo client            |
//                - SODmCPTabCtrl: Tab control of the property sheet          |
//                - SODmCPPage: Base class for all property pages             |
//                - SODmCPEdit: Base class for all edit fields                |
//                - SODmCPButton: Base class for all buttons                  |
//                - SODmCPComboBox: Base class for all combo boxes            |
//                - SODmCPListCtrl: Base class for all list controls          |
//                - SODmCPTreeCtrl: Base class for all tree controls          |
//                - SODmCPPAbout: About property page                         |
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
#include "SODmCPropFrm.h"
#include "SODmCServerDA.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCServerAE.h"
#include "SODmCSubscription.h"
#include "SODmCEvent.h"
#include "SODmCNameSpaceDA.h"

//-----------------------------------------------------------------------------
// SODmCPropertyFrame                                                         |
//-----------------------------------------------------------------------------

SODmCPropertyFrame* getPropertyFrame(void)
{
	SODmCMainFrame* pMainFrameWnd = (SODmCMainFrame *)AfxGetMainWnd();
	if ( pMainFrameWnd ==  NULL)
	{
		return NULL;
	}

	return pMainFrameWnd->getPropertyFrame();
}

IMPLEMENT_DYNCREATE(SODmCPropertyFrame, CMiniFrameWnd)

BEGIN_MESSAGE_MAP(SODmCPropertyFrame, CMiniFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_PROPERTY_OK, onOK)
	ON_COMMAND(ID_PROPERTY_APPLY, onApply)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_APPLY, onUpdApply)
	ON_COMMAND(ID_PROPERTY_RESET, onReset)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_RESET, onUpdReset)
END_MESSAGE_MAP()


SODmCPropertyFrame::SODmCPropertyFrame()
{
	m_pPropSheet = NULL;
	m_enableApply = FALSE;
	m_enableReset = FALSE;
}

int SODmCPropertyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (getComCtlVersion() >= COMCTL32_VERSION_IE4)
	{
		if (!m_wndToolBar.CreateEx(this) ||
			!m_wndToolBar.LoadToolBar(IDR_PROPERTYFRAME))
		{
			return -1;    // fail to create
		}

		if (!m_wndReBar.Create(this) ||
			!m_wndReBar.AddBar(&m_wndToolBar))
		{
			return -1;    // fail to create
		}
	}
	else
	{
		EnableDocking(CBRS_ALIGN_ANY);

		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_PROPERTYFRAME))
		{
			return -1;    // fail to create
		}

		m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
		DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);
	}

	m_pPropSheet = new SODmCPSheet(this);

	if (!m_pPropSheet->Create(this, WS_CHILD | WS_VISIBLE, 0))
	{
		delete m_pPropSheet;
		m_pPropSheet = NULL;
		return -1;
	}

	// Resize the mini frame so that it fits around the child property sheet.
	CRect rectClient, rectWindow;
	m_pPropSheet->GetWindowRect(rectClient);
	rectWindow = rectClient;
	// CMiniFrameWnd::CalcWindowRect adds the extra width and height
	// needed from the mini frame.
	CalcWindowRect(rectWindow);
	SetWindowPos(NULL, rectWindow.left, rectWindow.top,
				 rectWindow.Width(), rectWindow.Height() + 43,
				 SWP_NOZORDER | SWP_NOACTIVATE);
	m_pPropSheet->SetWindowPos(NULL, 0, 43,
							   rectClient.Width(), rectClient.Height(),
							   SWP_NOZORDER | SWP_NOACTIVATE);
	m_pPropSheet->onCreate();
	return 0;
}

SODmCPropertyFrame::~SODmCPropertyFrame()
{
	if (m_pPropSheet)
	{
		delete m_pPropSheet;
	}
}

void SODmCPropertyFrame::OnClose()
{
	SODmCPPage* pPP = (SODmCPPage*)m_pPropSheet->GetActivePage();
	pPP->onReset();
	ShowWindow(SW_HIDE);
}


//-----------------------------------------------------------------------------
// enable
//
// - enable button
//
void SODmCPropertyFrame::enable(
	IN UINT button, // button id
	IN BOOL enable) // enable or disable
{
	switch (button)
	{
	case ID_PROPERTY_APPLY:
		m_enableApply = enable;
		break;

	case ID_PROPERTY_RESET:
		m_enableReset = enable;
		break;
	}
} // enable

void SODmCPropertyFrame::onUpdApply(
	OUT CCmdUI* pCmdUI)     // command UI
{
	pCmdUI->Enable(m_enableApply);
} // OnUpdApply

void SODmCPropertyFrame::onUpdReset(
	OUT CCmdUI* pCmdUI)     // command UI
{
	pCmdUI->Enable(m_enableReset);
} // OnUpdApply

//-----------------------------------------------------------------------------
// removedObject
//
// - notifiy property sheet of object removal
//
void SODmCPropertyFrame::removedObject(IN SOCmnObject* obj) // object
{
	m_pPropSheet->removedObject(obj);
} // removedObject


//-----------------------------------------------------------------------------
// onEnter
//
// - Handler for Enter key -> same as OK button
//
void SODmCPropertyFrame::onEnter(void)
{
	if (m_enableApply)
	{
		onApply();
	}
} // onEnter


//-----------------------------------------------------------------------------
// onEscape
//
// - Handler for Escape key -> same as Reset button
//
void SODmCPropertyFrame::onEscape(void)
{
	if (m_enableReset)
	{
		onReset();
	}
} // onEscape


//-----------------------------------------------------------------------------
// onOK
//
// - Handler for OK button
//
void SODmCPropertyFrame::onOK(void)
{
	SODmCPPage* pPP = (SODmCPPage*)m_pPropSheet->GetActivePage();
	pPP->onOK();
	ShowWindow(SW_HIDE);
} // onOK


//-----------------------------------------------------------------------------
// onApply
//
// - Handler for Apply button
//
void SODmCPropertyFrame::onApply(void)
{
	SODmCPPage* pPP = (SODmCPPage*)m_pPropSheet->GetActivePage();
	pPP->onOK();
} // onApply


//-----------------------------------------------------------------------------
// onReset
//
// - Handler for Reset button
//
void SODmCPropertyFrame::onReset(void)
{
	SODmCPPage* pPP = (SODmCPPage*)m_pPropSheet->GetActivePage();
	pPP->onReset();
} // onReset




//-----------------------------------------------------------------------------
// SODmCPSheet                                                                |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPSheet, CPropertySheet)
	ON_MESSAGE(OWNMSG_HIT_ENTER, onEnter)
	ON_MESSAGE(OWNMSG_HIT_ESCAPE, onEscape)
END_MESSAGE_MAP()


SODmCPSheet::SODmCPSheet(SODmCPropertyFrame* frame)
	: CPropertySheet(_T("SOClient Properties"), (CWnd*)frame, 0)
{
	m_frame = frame;
	// create property pages
#ifdef SODMC_DA
	m_ppServerAttributes = new SODmCPPServerDAAttributes(this);
	m_ppServerStatus = new SODmCPPServerDAStatus(this);
	m_ppServerBrowse = new SODmCPPServerDABrowse(this);
	m_ppServerSecurity = new SODmCPPServerDASecurity(this);
	m_ppGroup = new SODmCPPGroup(this);
	m_ppItemAttributes = new SODmCPPItemAttributes(this);
	m_ppNode = new SODmCPPProperties(this);
	m_ppNodeWrite = new SODmCPPTagWrite(this);
	m_ppTag = new SODmCPPProperties(this);
	m_ppTagWrite = new SODmCPPTagWrite(this);
	m_ppProperty = new SODmCPPProperty(this);
#endif
#ifdef SODMC_AE
	m_ppServerAEAttributes = new SODmCPPServerAEAttributes(this);
	m_ppServerAEStatus = new SODmCPPServerAEStatus(this);
	m_ppServerAEBrowse = new SODmCPPServerAEBrowse(this);
	m_ppServerAESecurity = new SODmCPPServerAESecurity(this);
	m_ppSubscriptionAttributes = new SODmCPPSubscriptionAttributes(this);
#endif
	m_ppAbout = new SODmCPPAbout(this);
	showAbout();
#ifdef SODMC_DA
	m_lastServerPage = m_ppServerAttributes;
	m_lastGroupPage = m_ppGroup;
	m_lastItemPage = m_ppItemAttributes;
	m_lastNodePage = m_ppNode;
	m_lastTagPage = m_ppTag;
	m_lastPropertyPage = m_ppProperty;
#endif
#ifdef SODMC_AE
	m_lastServerAEPage = m_ppServerAEAttributes;
	m_lastSubscriptionPage = m_ppSubscriptionAttributes;
#endif
}

SODmCPSheet::~SODmCPSheet(void)
{
	m_obj.release();
#ifdef SODMC_DA
	m_srv.release();
#endif
#ifdef SODMC_AE
	m_srvAE.release();
#endif

	// delete property pages
	if (m_ppAbout)
	{
		delete m_ppAbout;
	}

#ifdef SODMC_DA

	if (m_ppServerAttributes)
	{
		delete m_ppServerAttributes;
	}

	if (m_ppServerStatus)
	{
		delete m_ppServerStatus;
	}

	if (m_ppServerBrowse)
	{
		delete m_ppServerBrowse;
	}

	if (m_ppServerSecurity)
	{
		delete m_ppServerSecurity;
	}

	if (m_ppGroup)
	{
		delete m_ppGroup;
	}

	if (m_ppItemAttributes)
	{
		delete m_ppItemAttributes;
	}

	if (m_ppNode)
	{
		delete m_ppNode;
	}

	if (m_ppNodeWrite)
	{
		delete m_ppNodeWrite;
	}

	if (m_ppTag)
	{
		delete m_ppTag;
	}

	if (m_ppTagWrite)
	{
		delete m_ppTagWrite;
	}

	if (m_ppProperty)
	{
		delete m_ppProperty;
	}

#endif
#ifdef SODMC_AE

	if (m_ppServerAEAttributes)
	{
		delete m_ppServerAEAttributes;
	}

	if (m_ppServerAEStatus)
	{
		delete m_ppServerAEStatus;
	}

	if (m_ppServerAEBrowse)
	{
		delete m_ppServerAEBrowse;
	}

	if (m_ppServerAESecurity)
	{
		delete m_ppServerAESecurity;
	}

	if (m_ppSubscriptionAttributes)
	{
		delete m_ppSubscriptionAttributes;
	}

#endif
}


//-----------------------------------------------------------------------------
// onCreate
//
// - subclass tab control
//
void SODmCPSheet::onCreate(void)
{
	HWND hwnd = (HWND)::SendMessage(m_hWnd, PSM_GETTABCONTROL, 0, 0);

	if (IsWindow(hwnd))
	{
		m_tab.SubclassWindow(hwnd);
	}
} // onCreate


//-----------------------------------------------------------------------------
// insertPage
//
// - insert property page into sheet
//
void SODmCPSheet::insertPage(IN SODmCPPage* pPropPage)  // property page
{
	m_listPages.AddTail(pPropPage);
	AddPage(pPropPage);
} // insertPage


//-----------------------------------------------------------------------------
// removeCurrentPages
//
// - remove all current property pages from the sheet
//
void SODmCPSheet::removeCurrentPages(void)
{
	SODmCPPage* pPP;
	POSITION pos;       // index in the list
	pos = m_listPages.GetTailPosition();

	while (pos != NULL)
	{
		pPP = (SODmCPPage*)m_listPages.GetPrev(pos);
		RemovePage(pPP);
	}

	m_listPages.RemoveAll();
} // removeCurrentPages


//-----------------------------------------------------------------------------
// saveLastPage
//
// - save the actual property page as last shown page for the object class
//
void SODmCPSheet::saveLastPage(void)
{
	if ((SOCmnObject*)m_obj)
	{
		switch (m_obj->getObjectType())
		{
#ifdef SODMC_DA

		case SOCMNOBJECT_TYPE_SERVER_DA:
			m_lastServerPage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_GROUP:
			m_lastGroupPage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_ITEM:
			m_lastItemPage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_NODE:
			m_lastNodePage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_TAG:
			m_lastTagPage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_PROPERTY:
			m_lastPropertyPage = GetActivePage();
			break;
#endif
#ifdef SODMC_AE

		case SOCMNOBJECT_TYPE_SERVER_AE:
			m_lastServerAEPage = GetActivePage();
			break;

		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
			m_lastSubscriptionPage = GetActivePage();
			break;
#endif
		}
	}
} // saveLastPage


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// showServerDAAttributes
//
// - show the property pages for a server object
//
void SODmCPSheet::showServerDAAttributes(IN SODaCServer* srv)   // server object
{
	if (m_obj == (SOCmnObject*)srv)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)srv;
	m_obj.addRef();
	m_srv = srv;
	m_srv.addRef();
	removeCurrentPages();
	m_ppServerAttributes->setObject(srv);
	m_ppServerBrowse->setObject(srv);
	m_ppServerStatus->setObject(srv);
	m_ppServerSecurity->setObject(srv);
	insertPage(m_ppServerAttributes);
	insertPage(m_ppServerStatus);
	insertPage(m_ppServerSecurity);
	insertPage(m_ppServerBrowse);
	SetActivePage(m_ppServerAttributes);
	SetActivePage(m_lastServerPage);
} // showServerDAAttributes


//-----------------------------------------------------------------------------
// showGroupAttributes
//
// - show the property pages for a server object
//
void SODmCPSheet::showGroupAttributes(IN SODaCGroup* grp)   // group object
{
	if (m_obj == (SOCmnObject*)grp)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)grp;
	m_obj.addRef();
	m_srv = (SODaCServer*)grp->getParentBranch();
	removeCurrentPages();
	m_ppGroup->setObject(grp);
	insertPage(m_ppGroup);
	SetActivePage(m_ppGroup);
	SetActivePage(m_lastGroupPage);
} // showGroupAttributes


//-----------------------------------------------------------------------------
// showItemAttributes
//
// - show the property pages for an item object
//
void SODmCPSheet::showItemAttributes(IN SODmCItem* item)    // item object
{
	if (m_obj == (SOCmnObject*)item)
	{
		return;
	}

	saveLastPage();
	SOCmnPointer<SODaCGroup> grp;
	m_obj = (SOCmnObject*)item;
	m_obj.addRef();
	grp = (SODaCGroup*)item->getParentBranch();
	m_srv = (SODaCServer*)grp->getParentBranch();
	removeCurrentPages();
	m_ppItemAttributes->setObject(item);
	insertPage(m_ppItemAttributes);
	SetActivePage(m_ppItemAttributes);
	SetActivePage(m_lastItemPage);
} // showItemAttributes


//-----------------------------------------------------------------------------
// showNodeAttributes
//
// - show the property pages for a node object
//
void SODmCPSheet::showNodeAttributes(IN SODaCNode* node)    // node object
{
	if (m_obj == (SOCmnObject*)node)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)node;
	m_obj.addRef();
	m_srv = (SODaCServer*)(SOCltServer*)(node->getServer());
	removeCurrentPages();
	m_ppNode->setObject(node);
	m_ppNodeWrite->setObject(node);
	insertPage(m_ppNode);
	insertPage(m_ppNodeWrite);
	SetActivePage(m_ppNode);
	SetActivePage(m_ppNodeWrite);
	SetActivePage(m_lastNodePage);
} // showNodeAttributes


//-----------------------------------------------------------------------------
// showTagAttributes
//
// - show the property pages for a tag object
//
void SODmCPSheet::showTagAttributes(IN SODaCTag* tag)   // tag object
{
	if (m_obj == (SOCmnObject*)tag)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)tag;
	m_obj.addRef();
	m_srv = (SODaCServer*)(SOCltServer*)(tag->getServer());
	removeCurrentPages();
	m_ppTag->setObject(tag);
	m_ppTagWrite->setObject(tag);
	insertPage(m_ppTag);
	insertPage(m_ppTagWrite);
	SetActivePage(m_ppTag);
	SetActivePage(m_ppTagWrite);
	SetActivePage(m_lastTagPage);
} // showTagAttributes


//-----------------------------------------------------------------------------
// showPropertyAttributes
//
// - show the property pages for a tag object
//
void SODmCPSheet::showPropertyAttributes(IN SODaCProperty* prop)    // property object
{
	if (m_obj == (SOCmnObject*)prop)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)prop;
	m_obj.addRef();
	m_srv = (SODaCServer*)(SOCltServer*)(prop->getServer());
	removeCurrentPages();
	m_ppProperty->setObject(prop);
	insertPage(m_ppProperty);
	SetActivePage(m_ppProperty);
	SetActivePage(m_lastPropertyPage);
} // showPropertyAttributes

#endif
#ifdef SODMC_AE
//-----------------------------------------------------------------------------
// showServerAEAttributes
//
// - show the property pages for a server object
//
void SODmCPSheet::showServerAEAttributes(IN SOAeCServer* srv)   // server object
{
	if (m_obj == (SOCmnObject*)srv)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)srv;
	m_obj.addRef();
	m_srvAE = srv;
	m_srvAE.addRef();
	removeCurrentPages();
	m_ppServerAEAttributes->setObject(srv);
	m_ppServerAEBrowse->setObject(srv);
	m_ppServerAEStatus->setObject(srv);
	m_ppServerAESecurity->setObject(srv);
	insertPage(m_ppServerAEAttributes);
	insertPage(m_ppServerAEStatus);
	insertPage(m_ppServerAESecurity);
	insertPage(m_ppServerAEBrowse);
	SetActivePage(m_ppServerAEAttributes);
	SetActivePage(m_lastServerAEPage);
} // showServerDAAttributes


//-----------------------------------------------------------------------------
// showSubscriptionAttributes
//
// - show the property pages for a subscription object
//
void SODmCPSheet::showSubscriptionAttributes(IN SOAeCSubscription* sus) // subscription object
{
	if (m_obj == (SOCmnObject*)sus)
	{
		return;
	}

	saveLastPage();
	m_obj = (SOCmnObject*)sus;
	m_obj.addRef();
	m_srvAE = (SOAeCServer*)sus->getParentBranch();
	removeCurrentPages();
	m_ppSubscriptionAttributes->setObject(sus);
	insertPage(m_ppSubscriptionAttributes);
	SetActivePage(m_ppSubscriptionAttributes);
	SetActivePage(m_lastSubscriptionPage);
} // showSubscriptionAttributes
#endif

//-----------------------------------------------------------------------------
// showAbout
//
// - show the about property page
// - no object selected
//
void SODmCPSheet::showAbout(void)
{
	saveLastPage();
	m_obj.release();
#ifdef SODMC_DA
	m_srv.release();
#endif
#ifdef SODMC_AE
	m_srvAE.release();
#endif
	removeCurrentPages();
	insertPage(m_ppAbout);
	SetActivePage(m_ppAbout);
} // showAbout


//-----------------------------------------------------------------------------
// setObjectState
//
// - notify the property pages of the state change of an object
//
void SODmCPSheet::setObjectState(
	IN SOCmnObject* obj,    // object
	IN BYTE state)            // new state
{
	if (obj == (SOCmnObject*)m_obj)
	{
#ifdef SODMC_DA

		if (m_obj->is(SOCMNOBJECT_TYPE_SERVER_DA))
		{
			if (::IsWindow(m_ppServerAttributes->m_hWnd))
			{
				m_ppServerAttributes->onSetObjectState(state);
			}
		}

		if (m_obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			if (::IsWindow(m_ppItemAttributes->m_hWnd))
			{
				m_ppItemAttributes->onSetObjectState(state);
			}
		}

		if (m_obj->is(SOCMNOBJECT_TYPE_GROUP))
		{
			if (::IsWindow(m_ppGroup->m_hWnd))
			{
				m_ppGroup->onSetObjectState(state);
			}
		}

#endif
#ifdef SODMC_AE

		if (m_obj->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			if (::IsWindow(m_ppServerAEAttributes->m_hWnd))
			{
				m_ppServerAEAttributes->onSetObjectState(state);
			}
		}

#endif
	}

#ifdef SODMC_DA

	if (obj == (SOCmnObject*)m_srv)
	{
		if (state == SOCltElement::disconnected)
			if ((::IsWindow(m_ppTag->m_hWnd)) || (::IsWindow(m_ppNode->m_hWnd)) || (::IsWindow(m_ppProperty->m_hWnd)))
			{
				showAbout();
			}
	}

#endif
} // setObjectState


//-----------------------------------------------------------------------------
// setTargetObjectState
//
// - notify the property pages of the target state change of an object
//
void SODmCPSheet::setTargetObjectState(
	IN SOCmnObject* obj,    // object
	IN BYTE state)            // new target state
{
} // setTargetObjectState


//-----------------------------------------------------------------------------
// setName
//
// - object name changed
//
void SODmCPSheet::setName(IN SOCmnObject* obj, IN LPCTSTR name)
{
	if (obj == (SOCmnObject*)m_obj)
	{
#ifdef SODMC_DA

		if (m_obj->is(SOCMNOBJECT_TYPE_GROUP))
		{
			if (::IsWindow(m_ppGroup->m_hWnd))
			{
				m_ppGroup->onSetName(name);
			}
		}

#endif
	}
} // setName


#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// setItemID
//
// - item id changed
//
void SODmCPSheet::setItemID(IN SODaCItem* item, IN LPCTSTR itemID)
{
	if ((SOCmnObject*)item == (SOCmnObject*)m_obj)
	{
		if (m_obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			if (::IsWindow(m_ppItemAttributes->m_hWnd))
			{
				m_ppItemAttributes->onSetItemID(itemID);
			}
		}
	}
} // setItemID

#endif

//-----------------------------------------------------------------------------
// removedObject
//
// - show about page if the current object was removed
//
void SODmCPSheet::removedObject(IN SOCmnObject* obj)    // object
{
	if (obj == (SOCmnObject*)m_obj)
	{
		showAbout();
	}
} // removedObject


//-----------------------------------------------------------------------------
// onEnter
//
// - Handler for Hit Enter message
//
afx_msg LONG SODmCPSheet::onEnter(IN UINT wParam, IN LONG lParam)
{
	m_frame->onEnter();
	return 0;
} // onEnter


//-----------------------------------------------------------------------------
// onEscape
//
// - Handler for Hit Escape message
//
afx_msg LONG SODmCPSheet::onEscape(IN UINT wParam, IN LONG lParam)
{
	m_frame->onEscape();
	return 0;
} // onEscape




//-----------------------------------------------------------------------------
// SODmCPTabCtrl                                                              |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPTabCtrl, CTabCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPTabCtrl::SODmCPTabCtrl()
{
}


//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPTabCtrl::OnGetDlgCode(void)
{
	return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPTabCtrl::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (nChar == 13)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		::SetFocus(::GetNextDlgTabItem(::GetParent(m_hWnd), m_hWnd, GetKeyState(VK_SHIFT) & 0xFF00));
	}
	else
	{
		CTabCtrl::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
} // OnChar



//-----------------------------------------------------------------------------
// SODmCPPage                                                                 |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPage, CPropertyPage)

BEGIN_MESSAGE_MAP(SODmCPPage, CPropertyPage)
	ON_MESSAGE(OWNMSG_HIT_ENTER, onEnter)
	ON_MESSAGE(OWNMSG_HIT_ESCAPE, onEscape)
END_MESSAGE_MAP()

SODmCPPage::SODmCPPage(
	IN SODmCPSheet* sheet,  // property sheet
	UINT resId)         // resource id of the property page form
	: CPropertyPage(resId)
{
	m_sheet = sheet;
}


//-----------------------------------------------------------------------------
// onOK
//
// - Default handler for OK
// - store attributes and save the current attributes
//
void SODmCPPage::onOK(void)
{
	storeAttributes();
	saveCurrentAttributes();
} // onOK


//-----------------------------------------------------------------------------
// onReset
//
// - Default handler for Reset
// - reset the current attributes
//
void SODmCPPage::onReset(void)
{
	resetCurrentAttributes();
} // onReset


//-----------------------------------------------------------------------------
// OnSetActive
//
// - Default handler for page activation
//
// returns:
//		return of base class
//
BOOL SODmCPPage::OnSetActive()
{
	BOOL ret = CPropertyPage::OnSetActive();
	clearAttributes();

	if (loadAttributes())
	{
		saveCurrentAttributes();
	}
	else
	{
		UpdateData(FALSE);
	}

	enableControls(TRUE);
	return ret;
} // OnSetActive


//-----------------------------------------------------------------------------
// OnKillActive
//
// - Default handler for page deactivation
//
// returns:
//		return of base class
//
BOOL SODmCPPage::OnKillActive()
{
	BOOL ret = CPropertyPage::OnKillActive();
//	clearAttributes();
//	enableControls(FALSE);
	return ret;
} // OnKillActive


//-----------------------------------------------------------------------------
// enableControls
//
// - Virtual method for control enabling
//
void SODmCPPage::enableControls(IN BOOL enable) // enable or disable
{} // enableControls


//-----------------------------------------------------------------------------
// loadAttributes
//
// - Virtual method for loading of all attributes
//
// returns:
//		TRUE
//
BOOL SODmCPPage::loadAttributes(void)
{
	return TRUE;    // loadAttributes
}


//-----------------------------------------------------------------------------
// storeAttributes
//
// - Virtual method for storing of all attributes
//
// returns:
//		TRUE
//
BOOL SODmCPPage::storeAttributes(void)
{
	return TRUE;    // storeAttributes
}


//-----------------------------------------------------------------------------
// clearAttributes
//
// - Virtual method for loading of all attributes
//
// returns:
//		TRUE
//
void SODmCPPage::clearAttributes(void)
{} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - Virtual method for saving the current attributes
//
void SODmCPPage::saveCurrentAttributes(void)
{} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - Virtual method for reseting the current attributes
//
void SODmCPPage::resetCurrentAttributes(void)
{} // resetCurrentAttributes


void SODmCPPage::enableButton(UINT button, BOOL enable)
{
	m_sheet->m_frame->enable(button, enable);
}

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// getServer
//
// - Get the server object of an object
// returns:
//		server object
//    NULL - no server object
//
SODaCServer* SODmCPPage::getServerDA(IN SOCmnElement* obj)  // object
{
	SODaCServer* srv = NULL;

	if (obj)
	{
		SOCMN_TRY
		{
			switch (obj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_SERVER_DA:
			{
				srv = (SODaCServer*)obj;
				srv->addRef();
			}
			break;

			case SOCMNOBJECT_TYPE_GROUP:
			{
				srv = (SODaCServer*)obj->getParentBranch();
			}
			break;

			case SOCMNOBJECT_TYPE_ITEM:
			{
				SOCmnPointer<SODaCGroup> group;
				group = (SODaCGroup*)obj->getParentBranch();

				if ((SODaCGroup*)group)
				{
					srv = (SODaCServer*)group->getParentBranch();
				}
			}
			break;

			case SOCMNOBJECT_TYPE_NODE:
				srv = (SODaCServer*)(SOCltServer*)(((SODaCNode*)obj)->getServer());
				break;

			case SOCMNOBJECT_TYPE_TAG:
				srv = (SODaCServer*)(SOCltServer*)(((SODaCTag*)obj)->getServer());
				break;

			case SOCMNOBJECT_TYPE_PROPERTY:
				srv = (SODaCServer*)(SOCltServer*)(((SODaCProperty*)obj)->getServer());
				break;
			}
		}
		SOCMN_CATCH(...)
		{}
	}

	return srv;
} // getServer


//-----------------------------------------------------------------------------
// connectServer
//
// - Connect the server object of an object
// returns:
//		server object
//    NULL - no server object or error connect to OPC server
//
SODaCServer* SODmCPPage::connectServerDA(IN GenericElement* obj)    // object
{
	SODaCServer* srv;
	srv = getServerDA(obj);

	if (srv)
	{
		// connect server
		if (!srv->isConnected())
		{
			if (srv->getTargetObjectState() == SODaCServer::disconnected)
			{
				if (FAILED(srv->connect(FALSE)))
				{
					srv->release();
					srv = NULL;
				}
			}
			else
			{
				srv->release();
				srv = NULL;
			}
		}
	}

	return srv;
} // connectServer

#endif

//-----------------------------------------------------------------------------
// onEnter
//
// - Handler for Hit Enter message
//
afx_msg LONG SODmCPPage::onEnter(IN UINT wParam, IN LONG lParam)
{
	m_sheet->m_frame->onEnter();
	return 0;
} // onEnter


//-----------------------------------------------------------------------------
// onEscape
//
// - Handler for Hit Escape message
//
afx_msg LONG SODmCPPage::onEscape(IN UINT wParam, IN LONG lParam)
{
	m_sheet->m_frame->onEscape();
	return 0;
} // onEscape




//-----------------------------------------------------------------------------
// SODmCPEdit                                                                 |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPEdit::SODmCPEdit()
{
	m_lastControl = FALSE;
	m_firstControl = FALSE;
	m_notificationWnd = NULL;
}

//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPEdit::OnGetDlgCode(void)
{
	return DLGC_HASSETSEL | DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPEdit::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (!m_notificationWnd)
	{
		m_notificationWnd = GetParent();
	}

	if (nChar == 13)
	{
		m_notificationWnd->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		m_notificationWnd->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		SHORT shiftPressed = GetKeyState(VK_SHIFT) & 0xFF00;
		HWND pWnd = ::GetParent(m_hWnd);

		if (((shiftPressed) && (m_firstControl)) || ((!shiftPressed) && (m_lastControl)))
		{
			::SetFocus(::FindWindowEx(::GetParent(pWnd), NULL, _T("SysTabControl32"), NULL));
		}
		else
		{
			::SetFocus(::GetNextDlgTabItem(pWnd, m_hWnd, shiftPressed));
		}
	}
	else
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
} // OnChar




//-----------------------------------------------------------------------------
// SODmCPButton                                                               |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPButton, CButton)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPButton::SODmCPButton()
{
	m_lastControl = FALSE;
	m_firstControl = FALSE;
}

//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPButton::OnGetDlgCode(void)
{
	return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPButton::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (nChar == 13)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		SHORT shiftPressed = GetKeyState(VK_SHIFT) & 0xFF00;
		HWND pWnd = ::GetParent(m_hWnd);

		if (((shiftPressed) && (m_firstControl)) || ((!shiftPressed) && (m_lastControl)))
		{
			::SetFocus(::FindWindowEx(::GetParent(pWnd), NULL, _T("SysTabControl32"), NULL));
		}
		else
		{
			::SetFocus(::GetNextDlgTabItem(pWnd, m_hWnd, shiftPressed));
		}
	}
	else
	{
		CButton::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
} // OnChar




//-----------------------------------------------------------------------------
// SODmCPComboBox                                                             |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPComboBox, CComboBox)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPComboBox::SODmCPComboBox()
{
	m_lastControl = FALSE;
	m_firstControl = FALSE;
}

//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPComboBox::OnGetDlgCode(void)
{
	return DLGC_HASSETSEL | DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPComboBox::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (nChar == 13)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		SHORT shiftPressed = GetKeyState(VK_SHIFT) & 0xFF00;
		HWND pWnd = ::GetParent(m_hWnd);

		if (((shiftPressed) && (m_firstControl)) || ((!shiftPressed) && (m_lastControl)))
		{
			::SetFocus(::FindWindowEx(::GetParent(pWnd), NULL, _T("SysTabControl32"), NULL));
		}
		else
		{
			::SetFocus(::GetNextDlgTabItem(pWnd, m_hWnd, shiftPressed));
		}
	}
	else
	{
		CComboBox::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
} // OnChar




//-----------------------------------------------------------------------------
// SODmCPListCtrl                                                             |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPListCtrl, CListCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPListCtrl::SODmCPListCtrl()
{
	m_lastControl = FALSE;
	m_firstControl = FALSE;
}

//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPListCtrl::OnGetDlgCode(void)
{
	return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPListCtrl::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (nChar == 13)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		SHORT shiftPressed = GetKeyState(VK_SHIFT) & 0xFF00;
		HWND pWnd = ::GetParent(m_hWnd);

		if (((shiftPressed) && (m_firstControl)) || ((!shiftPressed) && (m_lastControl)))
		{
			::SetFocus(::FindWindowEx(::GetParent(pWnd), NULL, _T("SysTabControl32"), NULL));
		}
		else
		{
			::SetFocus(::GetNextDlgTabItem(pWnd, m_hWnd, shiftPressed));
		}
	}
	else
	{
		CListCtrl::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
}




//-----------------------------------------------------------------------------
// SODmCPTreeCtrl                                                             |
//-----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(SODmCPTreeCtrl, CTreeCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

SODmCPTreeCtrl::SODmCPTreeCtrl()
{
	m_lastControl = FALSE;
	m_firstControl = FALSE;
}

//-----------------------------------------------------------------------------
// OnGetDlgCode
//
// - Set the keys you want to be notified of
//
afx_msg UINT SODmCPTreeCtrl::OnGetDlgCode(void)
{
	return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;
} // OnGetDlgCode


//-----------------------------------------------------------------------------
// OnChar
//
// - Handler for WM_CHAR message
// - Check for Enter and Escape
//
void SODmCPTreeCtrl::OnChar(
	IN UINT nChar,      // key id
	IN UINT nRepCnt,    // repeat count
	IN UINT nFlags)     // flags
{
	if (nChar == 13)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ENTER, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 27)
	{
		GetParent()->SendMessage(OWNMSG_HIT_ESCAPE, GetDlgCtrlID(), (LPARAM)m_hWnd);
	}
	else if (nChar == 9)
	{
		SHORT shiftPressed = GetKeyState(VK_SHIFT) & 0xFF00;
		HWND pWnd = ::GetParent(m_hWnd);

		if (((shiftPressed) && (m_firstControl)) || ((!shiftPressed) && (m_lastControl)))
		{
			::SetFocus(::FindWindowEx(::GetParent(pWnd), NULL, _T("SysTabControl32"), NULL));
		}
		else
		{
			::SetFocus(::GetNextDlgTabItem(pWnd, m_hWnd, shiftPressed));
		}
	}
	else
	{
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);    // permitted
	}
}




//-----------------------------------------------------------------------------
// SODmCPPAbout                                                               |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPAbout, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPAbout, SODmCPPage)
	ON_WM_PAINT()
END_MESSAGE_MAP()

SODmCPPAbout::SODmCPPAbout(
	IN SODmCPSheet* sheet)
	: SODmCPPage(sheet, IDD_PP_ABOUT)
{
}


//-----------------------------------------------------------------------------
// OnSetActive
//
// - Disable all buttons
//
BOOL SODmCPPAbout::OnSetActive()
{
	BOOL ret = CPropertyPage::OnSetActive();
	enableButton(ID_PROPERTY_APPLY, FALSE);
	enableButton(ID_PROPERTY_RESET, FALSE);
	return ret;
} // OnSetActive


