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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCTPGroup.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Group object related classes                                |
//                - SODaCTPGroupImpl: proxy for OPC group object              |
//                - SODaCTPGroupImplDefault: default group class for creator  |
//                - SODaCTaskSyncRead: synchron read task                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_TUNNEL

#include <opcda.h>
#include "SODaCServer.h"
#include "SODaCTPGroup.h"
#include "SODaCTPCall.h"
#include "SODaCItem.h"
#include "SODaCEntry.h"


//-----------------------------------------------------------------------------
// SODaCTPGroupImpl                                                          |
//-----------------------------------------------------------------------------

SODaCTPGroupImpl::SODaCTPGroupImpl(void)
{
	m_serverHandle = 0;
}

void SODaCTPGroupImpl::init(SODaCServer* /* pServer */)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	pDaCGroup->m_dataRetrieval = SODaCGroup::drAsyncAdvise;
}

SODaCTPGroupImpl::~SODaCTPGroupImpl(void)
{
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//		returns of unadvise
//
HRESULT SODaCTPGroupImpl::doDisconnectImpl(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	HRESULT res = S_OK;
	//-- remove group --
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->removeGroup(pDaCGroup, m_serverHandle);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Server::RemoveGroup [0x%X] "), (unsigned int)res));
	pDaCGroup->setLastError(res, SODAC_ERR_REMOVE_GROUP);
	pDaCGroup->setObjectState(SOCltElement::disconnected);
	return res;
}

//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the group if the target state differs from the
//   actual state of the group

HRESULT SODaCTPGroupImpl::doStateTransitionImpl(BOOL deep, IN BYTE stateDeep)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	BYTE targetState = pDaCGroup->getTargetObjectState();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	HRESULT res = S_OK;

	// check server object
	if (!server)
	{
		pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
		return E_FAIL;
	}

	if (SOCltElement::disconnected == targetState)
	{
		pDaCGroup->setItemsTargetState(targetState);
		doItemsStateTransition();
	}

	if (pDaCGroup->getObjectState() != targetState)
	{
		pDaCGroup->setStateTransitionActive(TRUE);
		SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
		SOCmnPointer<SODaCTPCall> tpCall;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));

		switch (pDaCGroup->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			//-- connect or start group --
			pDaCGroup->setConnectionType(SODaCGroup::ctAddGroup);
			pDaCGroup->setPublic(FALSE);

			if (!server->isConnected())
			{
				pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
				pDaCGroup->setStateTransitionActive(FALSE);
				return E_FAIL;
			}

			pDaCGroup->beforeInterfaceCall();
			res = tpCall->addGroup(pDaCGroup, m_serverHandle);
			pDaCGroup->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Server::AddGroup [0x%X] "), (unsigned int)res));
			pDaCGroup->setLastError(res, SODAC_ERR_ADD_GROUP);

			if (SUCCEEDED(res))
			{
				pDaCGroup->setObjectState(targetState); // set object state to target state
			}
		}
		break;

		default:
			if (targetState == SOCltElement::disconnected)
			{
				res = pDaCGroup->doDisconnect();
			}
			else
			{
				//-- start or connect group --
				pDaCGroup->setObjectState(targetState);
			}

			break;
		}

		pDaCGroup->setStateTransitionActive(FALSE);
	}

	if ((SUCCEEDED(res)) && (deep) && (targetState != SOCltElement::disconnected))
	{
		if (stateDeep != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE)
		{
			pDaCGroup->setItemsTargetState(stateDeep);
		}

		res = doItemsStateTransition();
	}

	return res;
} // doStateTransition


//-----------------------------------------------------------------------------
// doItemsStateTransition
//
// - try to transit the item states to the target item state
//
// returns:
//		TRUE  - transit all items to target state
//		FALSE - error during state change
//
HRESULT SODaCTPGroupImpl::doItemsStateTransition(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnList<SOCmnElement> list(pDaCGroup->getLeafList());
	SOCmnListPosition pos;
	SODaCItem* item;
	DWORD itemTargetState;
	DWORD itemState;
	SOCmnList<SODaCItem> addItemsList;
	SOCmnList<SODaCItem> removeItemsList;
	SOCmnList<SODaCItem> setActiveList;
	SOCmnList<SODaCItem> setInactiveList;
	SOCmnDWORDList addItemsTargetStateList;
	HRESULT res = S_OK;
	DWORD i;
	addItemsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);
	removeItemsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);
	setActiveList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);
	setInactiveList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);
	addItemsTargetStateList.create();
	pos = list.getStartPosition();

	while (pos)
	{
		item = (SODaCItem*)list.getNext(pos);
		itemTargetState = item->getTargetObjectState();
		itemState = item->getObjectState();

		if (itemTargetState != itemState)
		{
			// item has the same target state and hasn't reached it.
			item->setStateTransitionActive(TRUE);

			switch (itemState)
			{
			case SODaCItem::disconnected:
			{
				// must call AddItems to connect or start
				addItemsTargetStateList.add(itemTargetState);
				addItemsList.add(item);
			}
			break;

			case SODaCItem::connected:
			{
				if (itemTargetState == SODaCItem::disconnected)
				{
					removeItemsList.add(item);
				}
				else
				{
					setActiveList.add(item);
					item->setStateTransitionActive(FALSE);
					_TRACEO(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("SetActiveState for items is not supported from the Tunnel %s"), item->getItemID()));
				}
			}
			break;

			case SODaCItem::started:
			{
				if (itemTargetState == SODaCItem::disconnected)
				{
					removeItemsList.add(item);
				}
				else
				{
					setInactiveList.add(item);
					item->setStateTransitionActive(FALSE);
					_TRACEO(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("SetActiveState for items is not supported from the Tunnel %s"), item->getItemID()));
				}
			}
			break;
			}
		}
	}

	list.autoUnlock();

	if (!addItemsList.isEmpty())
	{
		SOCmnPointer<SODaCTPCall> tpCall;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
		pDaCGroup->beforeInterfaceCall();
		res = tpCall->addItems(pDaCGroup, getServerHandle(), &addItemsList, &addItemsTargetStateList);
		pDaCGroup->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::AddItems %lu [0x%X]"), addItemsList.getCount(), (unsigned int)res));
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_ADD_ITEMS);
		pos = addItemsList.getStartPosition();
		i = 0;

		while (pos)
		{
			item = (SODaCItem*)addItemsList.getNext(pos);
			item->setStateTransitionActive(FALSE);
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  aI  [%lu] %s (0x%8.8X) [%d]"), i, item->getItemID(), (DWORD)item->getOpcHandle(), !item->isConnected()));
			i++;
		}
	}

	if (!removeItemsList.isEmpty())
	{
		SOCmnPointer<SODaCTPCall> tpCall;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
		pDaCGroup->beforeInterfaceCall();
		res = tpCall->removeItems(pDaCGroup, getServerHandle(), &removeItemsList);
		pDaCGroup->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::RemoveItems %lu [0x%X]"), removeItemsList.getCount(), (unsigned int)res));
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_REMOVE_ITEMS);
		pos = removeItemsList.getStartPosition();
		i = 0;

		while (pos)
		{
			item = (SODaCItem*)removeItemsList.getNext(pos);
			item->setStateTransitionActive(FALSE);
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  rI  [%lu] %s (0x%8.8X) [%d]"), i, item->getItemID(), (DWORD)item->getOpcHandle(), item->isConnected()));
			i++;
		}
	}

	addItemsList.destroy();
	removeItemsList.destroy();
	setActiveList.destroy();
	setInactiveList.destroy();
	return res;
}

//-----------------------------------------------------------------------------
// update
//
// - update the group parameter
//
// returns:
//		S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCTPGroupImpl::doUpdateImpl(
	IN BOOL fromServer,                // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)  // update what parameters
{
	HRESULT res = S_OK;

	if (!fromServer)
	{
		// set data on server
		if (whatParameters & (SODAC_GRPPAR_UPDATERATE | SODAC_GRPPAR_DEADBAND))
		{
			// reestablish subscription
			SODaCGroup* pDaCGroup = getObjectDaCGroup();
			BYTE previousState = pDaCGroup->getObjectState();
			pDaCGroup->setTargetObjectState(SOCltElement::disconnected);
			res = doStateTransitionImpl(TRUE, SOCltElement::disconnected);
			if (res == S_OK)
			{
				pDaCGroup->setTargetObjectState(previousState);
				res = doStateTransitionImpl(TRUE, previousState);
			}
		}
	}

	return res;
} // update


//-----------------------------------------------------------------------------
// syncRead
//
// - read item values synchronous
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors for specific
//                      items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCTPGroupImpl::syncReadImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	OUT OPTIONAL SOCmnVariant* pValues,
	OUT OPTIONAL WORD* pQualities,
	OUT OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->syncRead(pDaCGroup, getServerHandle(), count, itemArray, maxAge, pValues, pQualities, pTimestamps, pErrors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::syncRead [0x%X]"), (unsigned int)res));
	return res;
} // syncRead



//-----------------------------------------------------------------------------
// syncWrite
//
// - write item values synchronous
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors for specific
//                      items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCTPGroupImpl::syncWriteImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->syncWrite(pDaCGroup, getServerHandle(), count, itemArray, pValues, pQualities, pTimestamps, pErrors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::syncWrite [0x%X]"), (unsigned int)res));
	return res;
} // syncWrite


//-----------------------------------------------------------------------------
// asyncRead
//
// - read item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but one or more errors
//                               for specific items
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCTPGroupImpl::asyncReadImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD maxAge,
	IN OPTIONAL DWORD transactionID,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->asyncRead(pDaCGroup, getServerHandle(), count, itemArray, maxAge, transactionID, cancelID, pErrors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::AsyncRead cnt:%lu ; transaction id = 0x%X [0x%X]"), count, (unsigned int)transactionID, (unsigned int)res));
	return res;
} // asyncRead


//-----------------------------------------------------------------------------
// asyncWrite
//
// - write item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but one or more errors
//                               for specific items
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//
HRESULT SODaCTPGroupImpl::asyncWriteImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD transactionID,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->asyncWrite(pDaCGroup, getServerHandle(), count, itemArray, transactionID, pValues, pQualities, pTimestamps, cancelID, pErrors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::AsyncWrite [0x%X]"), (unsigned int)res));
	return res;
} // asyncWrite


//-----------------------------------------------------------------------------
// asyncRefresh
//
// - refresh item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCTPGroupImpl::asyncRefreshImpl(
	IN DWORD maxAge,                  // data source
	IN OPTIONAL DWORD transactionID,  // transaction id (provided by client)
	OUT OPTIONAL DWORD* cancelID)     // cancel id (provided by server)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->refresh(pDaCGroup, getServerHandle(), maxAge, transactionID, cancelID);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::Refresh transaction id = 0x%X [0x%X]"), transactionID, res));
	return res;
} // asyncRefresh


//-----------------------------------------------------------------------------
// asyncCancel
//
// - cancel asynchronous request
//
// returns:
//		S_OK                   - operation succeeded
//      E_FAIL                 - operation failed
//      E_NOINTERFACE          - interface not supported
//
HRESULT SODaCTPGroupImpl::asyncCancelImpl(
	IN DWORD /* cancelID */)    // cancel id (provided by server)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	getObjectDaCGroup();
	HRESULT res = S_OK;
	res = E_NOTIMPL;
	return res;
} // asyncCancel


//-----------------------------------------------------------------------------
// updateItems
//
// - send the requested datatypes of the items to the server
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors
//                      for specific items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCTPGroupImpl::updateItemsImpl(
	IN BOOL /* fromServer */,                 // update data from server or to server
	IN SOCmnList<SODaCItem>* /* itemList */,  // item list
	IN OPTIONAL DWORD /* whatParameters */)   // update what item properties
{
	getObjectDaCGroup();
	HRESULT res = S_OK;
	return res;
} // updateItems



HRESULT SODaCTPGroupImpl::validateItems(IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
	pDaCGroup->beforeInterfaceCall();
	res = tpCall->validateItems(pDaCGroup, getServerHandle(), itemList, pErrors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("TPDA::Group::ValidateItems [0x%X]"), (unsigned int)res));
	return res;
}


//-----------------------------------------------------------------------------
// SODaCTPGroupDefault                                                       |
//-----------------------------------------------------------------------------

SODaCTPGroupDefault::SODaCTPGroupDefault(void)
	: SODaCGroup()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCTPGroupDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError

#endif  // SOFEATURE_TUNNEL

#endif  //  SODAC

