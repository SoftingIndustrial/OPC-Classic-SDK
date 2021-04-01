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
//  Filename    : SODmCObjectView.cpp                                         |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : classes of the top left splitter window (object view)       |
//                and all toolkit objects                                     |
//                - SODmCObjectView: object view class                        |
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
#include "SODmCObjectView.h"
#include "SODmCPropFrm.h"
#include "SODmCMainFrm.h"
#include "SODaCServer.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCServerAE.h"
#include "SODmCServerDA.h"
#include "SODmCSubscription.h"
#include "SODmCEvent.h"
#include ".\sodmcobjectview.h"


//-----------------------------------------------------------------------------
// Global SODmCObjectView access function                                     |
//-----------------------------------------------------------------------------

static SODmCObjectView* s_objectView = NULL;

//-----------------------------------------------------------------------------
// getObjectView
//
// - get object view object
//
// returns:
//		object view object
//
SODmCObjectView* getObjectView()
{
	return s_objectView;
} // getObjectView




//-----------------------------------------------------------------------------
// SODmCObjectView                                                            |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(SODmCObjectView, CTreeView)

BEGIN_MESSAGE_MAP(SODmCObjectView, CTreeView)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSC)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDI)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBLE)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnELE)
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_COMMAND(ID_DELETEOBJECT, onDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETEOBJECT, onUpdDelete)
	ON_COMMAND(ID_EDIT_ADDGROUP, onAddGroup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDGROUP, onUpdAddGroup)
	ON_COMMAND(ID_EDIT_ADDSUBSCRIPTION, onAddSubscription)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSUBSCRIPTION, onUpdAddSubscription)
	ON_COMMAND(ID_EDIT_ADDITEM, onAddItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDITEM, onUpdAddItem)
	ON_COMMAND(ID_EDIT_PROPERTIES, onProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, onUpdProperties)
	ON_COMMAND(ID_START_FLAT, onStartObjFlat)
	ON_COMMAND(ID_START_DEEP, onStartObjDeep)
	ON_COMMAND(ID_CONNECT_FLAT, onConnectObjFlat)
	ON_COMMAND(ID_CONNECT_DEEP, onConnectObjDeep)
	ON_COMMAND(ID_STOP, onStopObj)
	ON_UPDATE_COMMAND_UI(ID_START_FLAT, onUpdStartObjFlat)
	ON_UPDATE_COMMAND_UI(ID_START_DEEP, onUpdStartObjDeep)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_FLAT, onUpdConnectObjFlat)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_DEEP, onUpdConnectObjDeep)
	ON_UPDATE_COMMAND_UI(ID_STOP, onUpdStopObj)
	ON_COMMAND(ID_GROUP_REFRESH, onRefreshGroup)
	ON_UPDATE_COMMAND_UI(ID_GROUP_REFRESH, onUpdRefreshGroup)
	ON_COMMAND(ID_ITEM_READ, onReadItem)
	ON_UPDATE_COMMAND_UI(ID_ITEM_READ, onUpdReadItem)
	ON_COMMAND(ID_WRITEFROMFILE, onWriteFromFile)
	ON_UPDATE_COMMAND_UI(ID_WRITEFROMFILE, onUpdWriteFromFile)
	ON_COMMAND(ID_READTOFILE, onReadToFile)
	ON_UPDATE_COMMAND_UI(ID_READTOFILE, onUpdReadToFile)
	ON_UPDATE_COMMAND_UI(ID_LOGTOFILE, OnUpdLogtofile)
	ON_COMMAND(ID_LOGTOFILE, OnLogtofile)
END_MESSAGE_MAP()

SODmCObjectView::SODmCObjectView()
{
	s_objectView = this;
	m_images.Create(IDB_OBJECT_IMAGE, 16, 0, RGB(255, 0, 255));
	m_firstInit = TRUE;
	m_hDA = 0;
	m_hAE = 0;
	m_workingList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
}


//-----------------------------------------------------------------------------
// OnDestroy
//
// - clear item list if list view is destroyed
//
afx_msg void SODmCObjectView::OnDestroy(void)
{
	KillTimer(m_timer);
	GetTreeCtrl().DeleteAllItems();
	s_objectView = NULL;
	m_workingList.destroy();
} // OnDestroy


//-----------------------------------------------------------------------------
// OnInitialUpdate
//
// - set tree control image list
//
void SODmCObjectView::OnInitialUpdate(void)
{
	CTreeView::OnInitialUpdate();

	if (m_firstInit)
	{
		m_firstInit = FALSE;
		GetTreeCtrl().SetImageList(&m_images, TVSIL_NORMAL);
		m_timer = SetTimer(1, 130, 0);
	}
} // OnInitialUpdate


//-----------------------------------------------------------------------------
// PreCreateWindow
//
// - initialize tree control
//
// returns:
//      result of base class
//
BOOL SODmCObjectView::PreCreateWindow(
	IN CREATESTRUCT& cs)    // creation data
{
	cs.style &= ~(TVS_DISABLEDRAGDROP);
	cs.style |= (TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS);
	return CTreeView::PreCreateWindow(cs);
} // PreCreateWindow


//-----------------------------------------------------------------------------
// getTreeHandleDA
//
// - get the tree handle of the root of the DA object tree
//
// returns:
//      tree handle
//
HTREEITEM SODmCObjectView::getTreeHandleDA(void)
{
	if (m_hDA == 0)
	{
		m_hDA = SODmCTreeInsert(&GetTreeCtrl(), TVI_ROOT, _T("Data Access"), TRUE, NULL, SODMCOBJECTVIEW_IMG_SPEC_DA, SODMCOBJECTVIEW_IMG_SPEC_DA, TVI_LAST);
	}

	return m_hDA;
} // getTreeHandleDA


//-----------------------------------------------------------------------------
// deleteDATree
//
// - delete the root of the DA object tree
//
void SODmCObjectView::deleteDATree(void)
{
	if (m_hDA != 0)
	{
		GetTreeCtrl().DeleteItem(m_hDA);
	}

	m_hDA = 0;
} // deleteDATree


//-----------------------------------------------------------------------------
// getTreeHandleAE
//
// - get the tree handle of the root of the DA object tree
//
// returns:
//      tree handle
//
HTREEITEM SODmCObjectView::getTreeHandleAE(void)
{
	if (m_hAE == 0)
	{
		m_hAE = SODmCTreeInsert(&GetTreeCtrl(), TVI_ROOT, _T("Alarms and Events"), TRUE, NULL, SODMCOBJECTVIEW_IMG_SPEC_AE, SODMCOBJECTVIEW_IMG_SPEC_AE, TVI_LAST);
	}

	return m_hAE;
} // getTreeHandleAE


//-----------------------------------------------------------------------------
// deleteAETree
//
// - delete the root of the DA object tree
//
void SODmCObjectView::deleteAETree(void)
{
	if (m_hAE != 0)
	{
		GetTreeCtrl().DeleteItem(m_hAE);
	}

	m_hAE = 0;
} // deleteAETree


void SODmCObjectView::OnTimer(UINT nIDEvent)
{
	static int curImg = 0;
	SOCmnList<SOCltElement> workingList(m_workingList);
	SOCmnListPosition pos;
	SOCltElement* pEl;
	HTREEITEM hTree;
	int baseImg;
	BYTE objState, targetObjectState;
	pos = workingList.getStartPosition();

	while (pos)
	{
		pEl = workingList.getNext(pos);
		objState = pEl->getObjectState();
		targetObjectState = pEl->getTargetObjectState();

		if (objState != targetObjectState)
		{
			if (((objState == SOCltElement::disconnected) && (targetObjectState == SOCltElement::started)) ||
				((objState == SOCltElement::started) && (targetObjectState == SOCltElement::disconnected)))
			{
				baseImg = SODMCOBJECTVIEW_IMG_WORK_R_G;
			}
			else if (((objState == SOCltElement::disconnected) && (targetObjectState == SOCltElement::connected)) ||
					 ((objState == SOCltElement::connected) && (targetObjectState == SOCltElement::disconnected)))
			{
				baseImg = SODMCOBJECTVIEW_IMG_WORK_R_Y;
			}
			else
			{
				baseImg = SODMCOBJECTVIEW_IMG_WORK_Y_G;
			}

			switch (pEl->getObjectType())
			{
#ifdef SODMC_DA

			case SOCMNOBJECT_TYPE_SERVER_DA:
			{
				SODmCCmnServerDA* pCmnSrv = NULL;
#ifdef SODMC_DCOM

				if (((SODaCServer*)pEl)->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)pEl;
				}

#endif
#ifdef SODMC_SOAP

				if (((SODaCServer*)pEl)->getProtocol() == SOCLT_PROTOCOL_SOAP)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)pEl;
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (((SODaCServer*)pEl)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					pCmnSrv = (SODmCCmnServerDA*)(SODmCTPServerDA*)pEl;
				}

#endif
				hTree = pCmnSrv->getTreeHandle();
			}
			break;

			case SOCMNOBJECT_TYPE_GROUP:
#ifdef SODMC_DCOM
				if (((SODaCGroup*)pEl)->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					hTree = ((SODmCComGroup*)pEl)->getTreeHandle();
				}

#endif
#ifdef SODMC_SOAP

				if (((SODaCGroup*)pEl)->getProtocol() == SOCLT_PROTOCOL_SOAP)
				{
					hTree = ((SODmCXmlGroup*)pEl)->getTreeHandle();
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (((SODaCGroup*)pEl)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					hTree = ((SODmCTPGroup*)pEl)->getTreeHandle();
				}

#endif
				break;
#endif
#ifdef SODMC_AE

			case SOCMNOBJECT_TYPE_SERVER_AE:
			{
				SODmCCmnServerAE* pCmnSrv = NULL;
#ifdef SODMC_DCOM

				if (((SOAeCServer*)pEl)->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)pEl;
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (((SOAeCServer*)pEl)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)pEl;
				}

#endif
				hTree = pCmnSrv->getTreeHandle();
			}
			break;

			case SOCMNOBJECT_TYPE_SUBSCRIPTION:
#ifdef SODMC_DCOM
				if (((SOAeCSubscription*)pEl)->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					hTree = ((SODmCComSubscription*)pEl)->getTreeHandle();
				}

#endif
#ifdef SOFEATURE_TUNNEL

				if (((SOAeCSubscription*)pEl)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					hTree = ((SODmCTPSubscription*)pEl)->getTreeHandle();
				}

#endif
				break;
#endif
			}

			GetTreeCtrl().SetItemImage(hTree, baseImg + curImg, baseImg + curImg);
		}
	}

	curImg++;

	if (curImg > 3)
	{
		curImg = 0;
	}
}

void SODmCObjectView::addToWorkingList(IN SOCltElement* element)
{
	SOCmnList<SOCltElement> workingList(m_workingList);
	workingList.add(element);
}

void SODmCObjectView::removeFromWorkingList(IN SOCltElement* element)
{
	SOCmnList<SOCltElement> workingList(m_workingList);
	workingList.removeObject(element);
}

//-----------------------------------------------------------------------------
// OnRButtonDown
//
// - show popup menu on right mouse button down
//
void SODmCObjectView::OnRButtonDown(
	IN UINT nFlags,     // flags
	IN CPoint point)    // mouse point
{
	CMenu bar;
	GetParentFrame()->ActivateFrame();
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		switch (obj->getObjectType())
		{
		case SOCMNOBJECT_TYPE_SERVER_DA:
			bar.LoadMenu(IDR_OBJECT_SERVERDA_POPUP);
			break;

		case SOCMNOBJECT_TYPE_SERVER_AE:
			bar.LoadMenu(IDR_OBJECT_SERVERAE_POPUP);
			break;

		case SOCMNOBJECT_TYPE_GROUP:
			bar.LoadMenu(IDR_OBJECT_GROUP_POPUP);
			break;

		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
			bar.LoadMenu(IDR_OBJECT_SUBSCRIPTION_POPUP);
			break;

		case SOCMNOBJECT_TYPE_ITEM:
			bar.LoadMenu(IDR_OBJECT_ITEM_POPUP);
			break;
		}
	}

	if (bar.m_hMenu != 0)
	{
		CMenu& popup = *bar.GetSubMenu(0);  // get sub menu
		// view menu
		ClientToScreen(&point);
		popup.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
		CTreeView::OnRButtonDown(nFlags, point);
	}
} // OnRButtonDown


//-----------------------------------------------------------------------------
// OnSC
//
// - selected object changed
//
void SODmCObjectView::OnSC(NMHDR* pNMHDR, LRESULT* pResult)
{
	SODmCMainView* mainview = ::getMainView();

	if (pResult)
	{
		*pResult = 0;
	}

	switch (mainview->getSelectedTab())
	{
#ifdef SODMC_DA

	case SODMCMAINVIEW_TAB_TDA:
	{
		SOCmnPointer<SODaCServer> srv = getSelectedServerDA();
		mainview->changeSelectedServerDA(srv, FALSE);
	}
	break;

	case SODMCMAINVIEW_TAB_LDA:
	{
		SOCmnPointer<SODmCItem> item = getSelectedItem();
		mainview->clearSelection();
		mainview->changeSelectedItem(item);
	}
	break;
#endif
#ifdef SODMC_AE

	case SODMCMAINVIEW_TAB_TAE:
	{
		SOCmnPointer<SOAeCSubscription> subs = getSelectedSubscription();
		mainview->changeSelectedSubscription(subs, FALSE);
	}
	break;
#endif
	}
} // OnSC


//-----------------------------------------------------------------------------
// OnDI
//
// - tree item deleted
//
void SODmCObjectView::OnDI(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
	{
		*pResult = 0;
	}

	NM_TREEVIEW* nmTV = (NM_TREEVIEW*)pNMHDR;
	SOCltElement* obj = (SOCltElement*)GetTreeCtrl().GetItemData(nmTV->itemOld.hItem);

	if (obj)
	{
		switch (obj->getObjectType())
		{
#ifdef SODMC_DA

		case SOCMNOBJECT_TYPE_SERVER_DA:
		{
			SODmCCmnServerDA* pCmnSrv = NULL;
#ifdef SODMC_DCOM

			if (((SODaCServer*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)obj;
			}

#endif
#ifdef SODMC_SOAP

			if (((SODaCServer*)obj)->getProtocol() == SOCLT_PROTOCOL_SOAP)
			{
				pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)obj;
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SODaCServer*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				pCmnSrv = (SODmCCmnServerDA*)(SODmCTPServerDA*)obj;
			}

#endif
			pCmnSrv->setTreeHandle(0);
		}
		break;

		case SOCMNOBJECT_TYPE_ITEM:
			((SODmCItem*)obj)->setTreeHandle(0);
			break;

		case SOCMNOBJECT_TYPE_GROUP:
#ifdef SODMC_DCOM
			if (((SODaCGroup*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				((SODmCComGroup*)obj)->setTreeHandle(0);
			}

#endif
#ifdef SODMC_SOAP

			if (((SODaCGroup*)obj)->getProtocol() == SOCLT_PROTOCOL_SOAP)
			{
				((SODmCXmlGroup*)obj)->setTreeHandle(0);
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SODaCGroup*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				((SODmCTPGroup*)obj)->setTreeHandle(0);
			}

#endif
			break;
#endif
#ifdef SODMC_AE

		case SOCMNOBJECT_TYPE_SERVER_AE:
		{
			SODmCCmnServerAE* pCmnSrv = NULL;
#ifdef SODMC_DCOM

			if (((SOAeCServer*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)obj;
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SOAeCServer*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)obj;
			}

#endif
			pCmnSrv->setTreeHandle(0);
		}
		break;

		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
#ifdef SODMC_DCOM
			if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				((SODmCComSubscription*)obj)->setTreeHandle(0);
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				((SODmCTPSubscription*)obj)->setTreeHandle(0);
			}

#endif
			break;
#endif
		}
	}
} // OnDI


//-----------------------------------------------------------------------------
// OnBLE
//
// - begin editing tree label
//
void SODmCObjectView::OnBLE(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVDISPINFO* nmDI = (NMTVDISPINFO*)pNMHDR;

	if (GetTreeCtrl().GetItemData(nmDI->item.hItem))
	{
		*pResult = TRUE;
	}
	else
	{
		*pResult = FALSE;
	}
} // OnBLE


//-----------------------------------------------------------------------------
// OnELE
//
// - end editing tree label
//
void SODmCObjectView::OnELE(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVDISPINFO* nmDI = (NMTVDISPINFO*)pNMHDR;
	SOCltElement* obj = (SOCltElement*)GetTreeCtrl().GetItemData(nmDI->item.hItem);
	*pResult = FALSE;

	if ((nmDI->item.pszText) && (obj))
	{
		switch (obj->getObjectType())
		{
#ifdef SODMC_DA

		case SOCMNOBJECT_TYPE_SERVER_DA:
			((SODaCServer*)obj)->setServerName(nmDI->item.pszText);
			*pResult = TRUE;
			break;

		case SOCMNOBJECT_TYPE_ITEM:
		{
			SODmCItem* item = ((SODmCItem*)obj);
			BYTE targetState = item->getTargetObjectState();
			*pResult = item->setItemID(nmDI->item.pszText);

			if ((item->isConnected()) || (targetState >= SOCltElement::connected))
			{
				item->disconnect();

				if (targetState == SOCltElement::connected)
				{
					item->connect();
				}
				else
				{
					item->start();
				}
			}
		}
		break;

		case SOCMNOBJECT_TYPE_GROUP:
		{
			SODaCGroup* group = ((SODaCGroup*)obj);
			BYTE targetState = group->getTargetObjectState();

			if (SUCCEEDED(group->setName(nmDI->item.pszText)))
			{
				*pResult = TRUE;

				if ((group->isConnected()) || (targetState >= SOCltElement::connected))
				{
					if (group->getConnectionType() == SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME)
					{
						group->disconnect();

						if (targetState == SOCltElement::connected)
						{
							group->connect(TRUE);
						}
						else
						{
							group->start(TRUE);
						}
					}
					else
					{
						group->update(FALSE, SODAC_GRPPAR_NAME);
					}
				}
			}
		}
		break;
#endif
#ifdef SODMC_AE

		case SOCMNOBJECT_TYPE_SERVER_AE:
			((SOAeCServer*)obj)->setServerName(nmDI->item.pszText);
			*pResult = TRUE;
			break;

		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
#ifdef SODMC_DCOM
			if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				((SODmCComSubscription*)obj)->setLabel(nmDI->item.pszText);
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SOAeCSubscription*)obj)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				((SODmCTPSubscription*)obj)->setLabel(nmDI->item.pszText);
			}

#endif
			*pResult = TRUE;
			break;
#endif
		}

		getApp()->setDirtyFlag();
	}
} // OnELE


//-----------------------------------------------------------------------------
// getSelectedObject
//
// - get the selected toolkit object
//
// returns:
//      Selected object
//		NULL - No object selected
//
SOCltElement* SODmCObjectView::getSelectedObject(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCltElement* obj = NULL;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			obj->addRef();
		}
	}

	return obj;
} // getSelectedObject


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// getSelectedServerDA
//
// - get the selected server DA object
//
// returns:
//      Selected server DA
//		NULL - No server DA selected
//
SODaCServer* SODmCObjectView::getSelectedServerDA(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCmnElement* obj;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			switch (obj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_SERVER_DA:
				obj->addRef();
				return (SODaCServer*)obj;
				break;

			case SOCMNOBJECT_TYPE_GROUP:
				return (SODaCServer*)obj->getParentBranch();
				break;

			case SOCMNOBJECT_TYPE_ITEM:
			{
				SOCmnPointer<SOCmnElement> group;
				group = obj->getParentBranch();
				return (SODaCServer*)group->getParentBranch();
			}
			break;
			}
		}
	}

	return NULL;
} // getSelectedServerDA
#endif


#ifdef SODMC_AE
//-----------------------------------------------------------------------------
// getSelectedServerAE
//
// - get the selected server AE object
//
// returns:
//      Selected server AE
//		NULL - No server AE selected
//
SOAeCServer* SODmCObjectView::getSelectedServerAE(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCmnElement* obj;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			switch (obj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_SERVER_AE:
				obj->addRef();
				return (SOAeCServer*)obj;
				break;

			case SOCMNOBJECT_TYPE_SUBSCRIPTION:
				return (SOAeCServer*)obj->getParentBranch();
				break;
			}
		}
	}

	return NULL;
} // getSelectedServerDA
#endif


#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// getSelectedGroup
//
// - get the selected group
//
// returns:
//      Selected group
//		NULL - No group selected
//
SODaCGroup* SODmCObjectView::getSelectedGroup(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCmnElement* obj;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			switch (obj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_SERVER_DA:
			{
				SOCmnList<SOCmnElement> branchList(obj->getBranchList());

				if (branchList.getCount() == 1)
				{
					SODaCGroup* grp = (SODaCGroup*)branchList.getHead();
					grp->addRef();
					return grp;
				}
			}
			break;

			case SOCMNOBJECT_TYPE_GROUP:
				obj->addRef();
				return (SODaCGroup*)obj;
				break;

			case SOCMNOBJECT_TYPE_ITEM:
				return (SODaCGroup*)obj->getParentBranch();
				break;
			}
		}
	}

	return NULL;
} // getSelectedGroup
#endif


#ifdef SODMC_AE
//-----------------------------------------------------------------------------
// getSelectedSubscription
//
// - get the selected subscription
//
// returns:
//      Selected group
//		NULL - No group selected
//
SOAeCSubscription* SODmCObjectView::getSelectedSubscription(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCmnElement* obj;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			switch (obj->getObjectType())
			{
			case SOCMNOBJECT_TYPE_SERVER_AE:
			{
				SOCmnList<SOCmnElement> leafList(obj->getLeafList());

				if (leafList.getCount() == 1)
				{
					SOAeCSubscription* sub = (SOAeCSubscription*)leafList.getHead();
					sub->addRef();
					return sub;
				}
			}
			break;

			case SOCMNOBJECT_TYPE_SUBSCRIPTION:
				obj->addRef();
				return (SOAeCSubscription*)obj;
				break;
			}
		}
	}

	return NULL;
} // getSelectedSubscription
#endif

#ifdef SODMC_DA
//-----------------------------------------------------------------------------
// getSelectedItem
//
// - get the selected DA item
//
// returns:
//      Selected DA item
//		NULL - No DA item selected
//
SODmCItem* SODmCObjectView::getSelectedItem(void)
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	SOCmnElement* obj;

	if (hItem != NULL)
	{
		// item selected the object pointer is the item data
		obj = (SOCltElement*)GetTreeCtrl().GetItemData(hItem);

		if (obj)
		{
			if (obj->is(SOCMNOBJECT_TYPE_ITEM))
			{
				obj->addRef();
				return (SODmCItem*)obj;
			}
		}
	}

	return NULL;
} // getSelectedItem


//-----------------------------------------------------------------------------
// changeSelectedItem
//
// - called to change the selected item
//
void SODmCObjectView::changeSelectedItem(IN SODmCItem* item)    // new selected item
{
	if (item)
	{
		GetTreeCtrl().Select(item->getTreeHandle(), TVGN_CARET);
	}
} // changeSelectedItem


//-----------------------------------------------------------------------------
// changeSelectedServerDA
//
// - called to change the selected item
//
void SODmCObjectView::changeSelectedServerDA(IN SODaCServer* server)    // new selected server
{
	if (server)
	{
		SODmCCmnServerDA* pCmnSrv = NULL;
#ifdef SODMC_DCOM

		if (((SODaCServer*)server)->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)server;
		}

#endif
#ifdef SODMC_SOAP

		if (((SODaCServer*)server)->getProtocol() == SOCLT_PROTOCOL_SOAP)
		{
			pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)server;
		}

#endif
#ifdef SOFEATURE_TUNNEL

		if (((SODaCServer*)server)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			pCmnSrv = (SODmCCmnServerDA*)(SODmCTPServerDA*)server;
		}

#endif
		GetTreeCtrl().Select(pCmnSrv->getTreeHandle(), TVGN_CARET);
	}
} // changeSelectedServerDA
#endif


#ifdef SODMC_AE
//-----------------------------------------------------------------------------
// changeSelectedServerAE
//
// - called to change the selected item
//
void SODmCObjectView::changeSelectedServerAE(IN SOAeCServer* server)    // new selected server
{
	if (server)
	{
		SODmCCmnServerAE* pCmnSrv = NULL;

		if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)(SOAeCServer*)server;
		}

#ifdef SOFEATURE_TUNNEL
		else
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)(SOAeCServer*)server;
		}

#endif
		GetTreeCtrl().Select(pCmnSrv->getTreeHandle(), TVGN_CARET);
	}
} // changeSelectedServerAE
#endif


//-----------------------------------------------------------------------------
// setObjectStateImage
//
// - set the correct object state image
//
void SODmCObjectView::setObjectStateImage(
	IN HTREEITEM treeHandle,
	IN int listIdx,
	IN BYTE objState,
	IN BYTE targetObjState)
{
	int image;

	switch (objState)
	{
	case SOCltElement::disconnected:
	{
		switch (targetObjState)
		{
		case SOCltElement::disconnected:
			image = SODMCOBJECTVIEW_IMG_OBJ_R_R;
			break;

		case SOCltElement::connected:
			image = SODMCOBJECTVIEW_IMG_OBJ_R_Y;
			break;

		case SOCltElement::started:
			image = SODMCOBJECTVIEW_IMG_OBJ_R_G;
			break;
		}
	}
	break;

	case SOCltElement::connected:
	{
		switch (targetObjState)
		{
		case SOCltElement::disconnected:
			image = SODMCOBJECTVIEW_IMG_OBJ_Y_R;
			break;

		case SOCltElement::connected:
			image = SODMCOBJECTVIEW_IMG_OBJ_Y_Y;
			break;

		case SOCltElement::started:
			image = SODMCOBJECTVIEW_IMG_OBJ_Y_G;
			break;
		}
	}
	break;

	case SOCltElement::started:
	{
		switch (targetObjState)
		{
		case SOCltElement::disconnected:
			image = SODMCOBJECTVIEW_IMG_OBJ_G_R;
			break;

		case SOCltElement::connected:
			image = SODMCOBJECTVIEW_IMG_OBJ_G_Y;
			break;

		case SOCltElement::started:
			image = SODMCOBJECTVIEW_IMG_OBJ_G_G;
			break;
		}
	}
	break;
	}

	if (treeHandle != 0)
	{
		SODmCObjectView* objview = getObjectView();
		objview->GetTreeCtrl().SetItemImage(treeHandle, image, image);
	}

	if (listIdx != -1)
	{
		SODmCMainView* mainview = getMainView();
		image += SODMCMAINVIEW_IMG_OBJ_DEFAULT - SODMCOBJECTVIEW_IMG_OBJ_DEFAULT;
		LV_ITEM lv;
		lv.mask = LVIF_IMAGE;
		lv.iItem = listIdx;
		lv.iSubItem = 0;
		lv.iImage = image;
		mainview->getListDA()->SetItem(&lv);
	}
} // setObjectStateImage


//-----------------------------------------------------------------------------
// OnAddGroup
//
// - menu handler edit/add group
//
void SODmCObjectView::onAddGroup()
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		if (obj->is(SOCMNOBJECT_TYPE_SERVER_DA))
		{
			SOCmnPointer<SODaCGroup> group = (SODaCGroup*)((SODaCServer*)(SOCltElement*)obj)->addGroup(NULL, 1000);
			group->start(TRUE, SOCLT_EXEC_ASYNC);   // activate
			getApp()->setDirtyFlag();
			SODmCPSheet* sheet = getPropertyFrame()->getPropertySheet();
			sheet->showGroupAttributes(group);
			getMainFrame()->showPropertyFrame();
		}
	}

#endif
} // OnAddGroup


//-----------------------------------------------------------------------------
// onUpdAddGroup
//
// - Enable menu if server or DA item is selected
//
void SODmCObjectView::onUpdAddGroup(OUT CCmdUI* pCmdUI)     // command UI
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->is(SOCMNOBJECT_TYPE_SERVER_DA));
	}
	else
#endif
		pCmdUI->Enable(FALSE);
} // onUpdAddGroup


//-----------------------------------------------------------------------------
// OnAddSubscription
//
// - menu handler edit/add subscription
//
void SODmCObjectView::onAddSubscription()
{
#ifdef SODMC_AE
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		if (obj->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			SOCmnPointer<SOAeCSubscription> sub = (SOAeCSubscription*)((SOAeCServer*)(SOCltElement*)obj)->addSubscription();
			sub->start(TRUE, SOCLT_EXEC_ASYNC); // activate
			getApp()->setDirtyFlag();
			SODmCPSheet* sheet = getPropertyFrame()->getPropertySheet();
			sheet->showSubscriptionAttributes(sub);
			getMainFrame()->showPropertyFrame();
		}
	}

#endif
} // OnAddSubscription


//-----------------------------------------------------------------------------
// onUpdAddSubscription
//
// - Enable menu if server or DA item is selected
//
void SODmCObjectView::onUpdAddSubscription(OUT CCmdUI* pCmdUI)      // command UI
{
#ifdef SODMC_AE
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->is(SOCMNOBJECT_TYPE_SERVER_AE));
	}
	else
#endif
		pCmdUI->Enable(FALSE);
} // onUpdAddSubscription


//-----------------------------------------------------------------------------
// OnAddItem
//
// - menu handler edit/add item
//
void SODmCObjectView::onAddItem()
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		if (obj->is(SOCMNOBJECT_TYPE_GROUP))
		{
			SOCmnPointer<SODmCItem> item = (SODmCItem*)((SODaCGroup*)(SOCltElement*)obj)->addItem(_T("Item"));
			item->start(FALSE, SOCLT_EXEC_ASYNC);   // activate
			getApp()->setDirtyFlag();
			SODmCPSheet* sheet = getPropertyFrame()->getPropertySheet();
			sheet->showItemAttributes(item);
			getMainFrame()->showPropertyFrame();
		}
	}

#endif
} // OnAddItem


//-----------------------------------------------------------------------------
// onUpdAddItem
//
// - Enable menu if server or DA item is selected
//
void SODmCObjectView::onUpdAddItem(OUT CCmdUI* pCmdUI)      // command UI
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->is(SOCMNOBJECT_TYPE_GROUP));
	}
	else
#endif
		pCmdUI->Enable(FALSE);
} // onUpdAddItem


//-----------------------------------------------------------------------------
// OnDeleteObject
//
// - menu handler edit/delete object
//
void SODmCObjectView::onDelete()
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		SOCmnPointer<SOCmnElement> parent = obj->getParentBranch();

		switch (obj->getObjectType())
		{
		case SOCMNOBJECT_TYPE_SERVER_DA:
		case SOCMNOBJECT_TYPE_SERVER_AE:
		case SOCMNOBJECT_TYPE_GROUP:
			obj->disconnect();
			obj->removeAllAdditionalElements();
			obj->removeAllLeaves();
			obj->removeAllBranches();
			parent->removeBranch(obj);
			break;

		case SOCMNOBJECT_TYPE_ITEM:
		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
			obj->disconnect();
			parent->removeLeaf(obj);
			break;
		}

		obj->removeAllAdditionalElements();
		obj->removeAllLeaves();
		obj->removeAllBranches();
		getApp()->setDirtyFlag();
	}
} // OnDeleteObject


//-----------------------------------------------------------------------------
// onUpdSelected
//
// - Enable menu if server or DA item is selected
//
void SODmCObjectView::onUpdDelete(OUT CCmdUI* pCmdUI)       // command UI
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdSelected


//-----------------------------------------------------------------------------
// onProperties
//
// - menu handler of menu edit/properties object
//
afx_msg void SODmCObjectView::onProperties(void)
{
	// get the selected item
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		SODmCPSheet* sheet = getPropertyFrame()->getPropertySheet();

		switch (obj->getObjectType())
		{
#ifdef SODMC_DA

		case SOCMNOBJECT_TYPE_SERVER_DA:
			sheet->showServerDAAttributes((SODaCServer*)(SOCltElement*)obj);
			break;

		case SOCMNOBJECT_TYPE_GROUP:
			sheet->showGroupAttributes((SODaCGroup*)(SOCltElement*)obj);
			break;

		case SOCMNOBJECT_TYPE_ITEM:
			sheet->showItemAttributes((SODmCItem*)(SOCltElement*)obj);
			break;
#endif
#ifdef SODMC_AE

		case SOCMNOBJECT_TYPE_SERVER_AE:
			sheet->showServerAEAttributes((SOAeCServer*)(SOCltElement*)obj);
			break;

		case SOCMNOBJECT_TYPE_SUBSCRIPTION:
			sheet->showSubscriptionAttributes((SOAeCSubscription*)(SOCltElement*)obj);
			break;
#endif

		default:
			sheet->showAbout();
			break;
		}

		getMainFrame()->showPropertyFrame();
	}
} // onProperties


//-----------------------------------------------------------------------------
// onUpdProperties
//
// - update menu handler of menu edit/properties object
//
afx_msg void SODmCObjectView::onUpdProperties(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->is(SOCMNOBJECT_TYPE_SERVER_DA) || obj->is(SOCMNOBJECT_TYPE_GROUP) || obj->is(SOCMNOBJECT_TYPE_ITEM) || obj->is(SOCMNOBJECT_TYPE_SERVER_AE) || obj->is(SOCMNOBJECT_TYPE_SUBSCRIPTION));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdProperties


//-----------------------------------------------------------------------------
// onStartObjFlat
//
// - menu handler State/Start/Flat
//
afx_msg void SODmCObjectView::onStartObjFlat(void)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		obj->start(FALSE, SOCLT_EXEC_ASYNC);
	}
} // onStartObjFlat


//-----------------------------------------------------------------------------
// onUpdStartObjFlat
//
// - update menu handler of menu  State/Start/Flat
//
afx_msg void SODmCObjectView::onUpdStartObjFlat(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->getObjectState() != SOCltElement::started);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdStartObjFlat


//-----------------------------------------------------------------------------
// onStartObjDeep
//
// - menu handler State/Start/Deep
//
afx_msg void SODmCObjectView::onStartObjDeep(void)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		obj->start(TRUE, SOCLT_EXEC_ASYNC);
	}
} // onStartObjDeep


//-----------------------------------------------------------------------------
// onUpdStartObjDeep
//
// - update menu handler of menu  State/Start/Deep
//
afx_msg void SODmCObjectView::onUpdStartObjDeep(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdStartObjDeep


//-----------------------------------------------------------------------------
// onConnectObjFlat
//
// - menu handler State/Connect/Flat
//
afx_msg void SODmCObjectView::onConnectObjFlat(void)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		obj->connect(FALSE, SOCLT_EXEC_ASYNC);
	}
} // onConnectObjFlat


//-----------------------------------------------------------------------------
// onUpdConnectObjFlat
//
// - update menu handler of menu  State/Connect/Flat
//
afx_msg void SODmCObjectView::onUpdConnectObjFlat(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->getObjectState() != SOCltElement::connected);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdConnectObjFlat


//-----------------------------------------------------------------------------
// onConnectObjDeep
//
// - menu handler State/Connect/Deep
//
afx_msg void SODmCObjectView::onConnectObjDeep(void)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		obj->connect(TRUE, SOCLT_EXEC_ASYNC);
	}
} // onConnectObjDeep


//-----------------------------------------------------------------------------
// onUpdConnectObjDeep
//
// - update menu handler of menu  State/Connect/Deep
//
afx_msg void SODmCObjectView::onUpdConnectObjDeep(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdConnectObjDeep


//-----------------------------------------------------------------------------
// onStopObj
//
// - menu handler State/Stop
//
afx_msg void SODmCObjectView::onStopObj(void)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		obj->disconnect(SOCLT_EXEC_ASYNC);
	}
} // onStopObj


//-----------------------------------------------------------------------------
// onUpdStopObj
//
// - update menu handler of menu  State/Stop
//
afx_msg void SODmCObjectView::onUpdStopObj(OUT CCmdUI* pCmdUI)
{
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable(obj->getObjectState() != SOCltElement::disconnected);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // onUpdStopObj


//-----------------------------------------------------------------------------
// onRefreshGroup
//
// - menu handler Refresh
//
afx_msg void SODmCObjectView::onRefreshGroup(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		((SODaCGroup*)(SOCmnElement*)obj)->asyncRefresh(SODAC_MAXAGE_DEVICE);
	}

#endif
} // onRefreshGroup


//-----------------------------------------------------------------------------
// onUpdRefreshGroup
//
// - update menu handler of menu Refresh
//
afx_msg void SODmCObjectView::onUpdRefreshGroup(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
		pCmdUI->Enable((obj->is(SOCMNOBJECT_TYPE_GROUP)) && (obj->isStarted()) &&
					   (((SODaCGroup*)(SOCmnElement*)obj)->getDataRetrieval() == SODaCGroup::drAsyncAdvise));
	else
#endif
		pCmdUI->Enable(FALSE);
} // onUpdRefreshGroup


//-----------------------------------------------------------------------------
// onReadItem
//
// - menu handler Read
//
afx_msg void SODmCObjectView::onReadItem(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		((SODmCItem*)(SOCmnElement*)obj)->read(SODAC_MAXAGE_DEVICE);
	}

#endif
} // onReadItem


//-----------------------------------------------------------------------------
// onUpdReadItem
//
// - update menu handler of menu Read
//
afx_msg void SODmCObjectView::onUpdReadItem(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	SOCmnPointer<SOCltElement> obj(getSelectedObject());

	if (obj.isNotNull())
	{
		pCmdUI->Enable((obj->is(SOCMNOBJECT_TYPE_ITEM)) && (obj->isConnected()));
	}
	else
#endif
		pCmdUI->Enable(FALSE);
} // onUpdReadItem


//-----------------------------------------------------------------------------
// onWriteFromFile
//
// - menu handler of menu edit/write from file
//
afx_msg void SODmCObjectView::onWriteFromFile(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());
	getMainView()->writeFromFile(item);
#endif
} // onWriteFromFile


//-----------------------------------------------------------------------------
// onUpdWriteFromFile
//
// - update menu handler of menu edit/write from file
//
afx_msg void SODmCObjectView::onUpdWriteFromFile(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());

	if (item.isNotNull())
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

#endif
	pCmdUI->Enable(FALSE);
} // onUpdWriteFromFile


//-----------------------------------------------------------------------------
// onReadToFile
//
// - menu handler of menu edit/read to file
//
afx_msg void SODmCObjectView::onReadToFile(void)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());
	getMainView()->readToFile(item);
#endif
} // onReadToFile


//-----------------------------------------------------------------------------
// onUpdReadToFile
//
// - update menu handler of menu edit/read to file
//
afx_msg void SODmCObjectView::onUpdReadToFile(OUT CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	SOCmnPointer<SODmCItem> item(getSelectedItem());

	if (item.isNotNull())
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

#endif
	pCmdUI->Enable(FALSE);
} // onUpdReadToFile


void SODmCObjectView::OnLogtofile()
{
#ifdef SODMC_DA
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

#endif // SODMC_DA
}

void SODmCObjectView::OnUpdLogtofile(CCmdUI* pCmdUI)
{
#ifdef SODMC_DA
	pCmdUI->Enable(TRUE);
	SOCmnPointer<SODmCItem> item(getSelectedItem());

	if (item.isNotNull())
	{
		pCmdUI->SetCheck(item->getLogToFile());
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}

#else
	pCmdUI->Enable(FALSE);
#endif // SODMC_DA
}



