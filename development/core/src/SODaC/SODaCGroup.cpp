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
//  Filename    : SODaCGroup.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Group object related classes                                |
//                - SODaCGroup: proxy for OPC group object                    |
//                - SODaCGroupDefault: default group class for creator        |
//                - SODaCTaskSyncRead: synchron read task                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#include <opcda.h>
#include "SODaCServer.h"
#include "SODaCGroup.h"
#include "SODaCItem.h"
#include "SODaCEntry.h"
#include "SODaCCallback.h"

//-----------------------------------------------------------------------------
// SODaCGroup                                                                 |
//-----------------------------------------------------------------------------

SODaCGroup::SODaCGroup(void)
	: SOCmnElementListLeaf(0),
	  SOCltElement(SOCMNOBJECT_TYPE_GROUP)
{
	// create leaf map
	if (m_leafList.isInit())
	{
		m_leafList.destroy();
	}

	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 53;
	m_leafList.create(&cfg);
	m_reqUpdateRate = 1000;
	m_revUpdateRate = 1000;
	m_reqKeepAliveTime = 0;
	m_revKeepAliveTime = 0;
	m_timeBias = 0;
	m_lcid = 0;
	m_deadBand = 0.0;
	m_connectionType = ctNone;
	m_validParameter = 0;
	m_public = FALSE;
	m_isPublic = FALSE;
	m_stateTransitionActive = FALSE;
	m_initCnt = 0;
	m_correctNTtimer = 0;
	m_initTime = ::GetTickCount();
	m_asyncDirty = FALSE;
	setObjectState(disconnected);
	setTargetObjectState(disconnected);
}

SODaCGroup::~SODaCGroup(void)
{
}

//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SODaCGroup::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key (name)
//
// returns:
//		object key
//
BOOL SODaCGroup::getKey(
	OUT DWORD& key)     // object key
{
	key = this->getOpcHandle();
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key (name) with given int
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SODaCGroup::hasKey(
	IN DWORD key)   // object key
{
	return (this->getOpcHandle() == key);
} // hasKey


//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCGroup::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCGroup::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a item object
//
// returns:
//		TRUE  - added
//		FALSE - not added
//
BOOL SODaCGroup::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_ITEM))
	{
		m_leafList.doRehash(TRUE);
		return SOCltElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - don't add branch into group object
//
// returns:
//		FALSE - not added
//
BOOL SODaCGroup::addBranch(
	IN SOCmnElement* /* newBranch */)
{
	return FALSE;
} // addBranch


//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the group if the target state differs from the
//   actual state of the group

HRESULT SODaCGroup::doStateTransition(BOOL deep, IN BYTE stateDeep)
{
	return doStateTransitionImpl(deep, stateDeep);
} // doStateTransition

HRESULT SODaCGroup::doDisconnect(void)
{
	return doDisconnectImpl();
}


//-----------------------------------------------------------------------------
// setItemsTargetState
//
// - set the target state of all items of the group
//
void SODaCGroup::setItemsTargetState(
	IN BYTE targetState)    // items target state
{
	SOCmnList<SOCmnElement> list(getLeafList());
	SOCmnListPosition pos;
	SODaCItem* item;
	pos = list.getStartPosition();

	while (pos)
	{
		item = (SODaCItem*)list.getNext(pos);
		item->setTargetObjectState(targetState);
	}
} // setItemsTargetState


//-----------------------------------------------------------------------------
// setItemsState
//
// - set the state of all items of the group
//
void SODaCGroup::setItemsState(
	IN BYTE state)  // items target state
{
	SOCmnList<SOCmnElement> list(getLeafList());
	SOCmnListPosition pos;
	SODaCItem* item;
	pos = list.getStartPosition();

	while (pos)
	{
		item = (SODaCItem*)list.getNext(pos);
		item->setObjectState(state);
	}
} // setItemsState


//-----------------------------------------------------------------------------
// addItem
//
// - create item object and insert it into the group leaf list
// - initialize the item with the parameter data
//
// returns:
//      pointer to item object
//
SODaCItem* SODaCGroup::addItem(
	IN LPCTSTR itemID,              // item id
	IN OPTIONAL VARTYPE reqDatatype,// requested datatype
	IN OPTIONAL LPCTSTR itemOrAccessPath)     // item or access path
{
	USES_CONVERSION;
	// create item
	SOCmnPointer<SODaCEntry> session = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = session->getCreator();
	SODaCItem* item = creator->createItem(this);
	// initialize item
	item->setItemID(itemID);
	item->setReqDatatype(reqDatatype);

	if (getProtocol() != SOCLT_PROTOCOL_SOAP)
	{
		item->setAccessPath(itemOrAccessPath);
	}
	else
	{
		item->setItemPath(itemOrAccessPath);
	}

	// add item to group
	addLeaf(item);
	return item;
} // addItem


//-----------------------------------------------------------------------------
// getItem
//
// - get item object by the item id
//
// returns:
//      NULL - item not found
//      pointer to item object
//
SODaCItem* SODaCGroup::getItem(
	IN LPCTSTR itemID) // item id
{
	SOCmnList<SODaCItem> list = (SOCmnList<SODaCItem> *)getLeafList();
	SOCmnListPosition pos;
	SODaCItem* pItem;
	pos = list.getStartPosition();

	while (pos)
	{
		pItem = list.getNext(pos);

		if (pItem->getItemID() == itemID)
		{
			pItem->addRef();
			return pItem;
		}
	}

	return NULL;
} // getItem


//-----------------------------------------------------------------------------
// getItemByClientHandle
//
// - retrieve item pointer by the client handle value
//
// returns:
//		pointer to the item
//		NULL - no item with this client handle
//
SODaCItem* SODaCGroup::getItemByClientHandle(
	IN OPCHANDLE clientHandle)  // client handle
{
	SOCmnList<SOCmnElement> list(getLeafList());
	SODaCItem* item;
	// check item with standard name = key
	item = (SODaCItem*)list.findKey(clientHandle);

	if (item)
	{
		return item;
	}

	return NULL;
} // getItemByClientHandle


//-----------------------------------------------------------------------------
// getItemByServerHandle
//
// - retrieve item pointer by the server handle value
//
// returns:
//		pointer to the item
//		NULL - no item with this client handle
//
SODaCItem* SODaCGroup::getItemByServerHandle(
	IN OPCHANDLE serverHandle)  // client handle
{
	SODaCItem* item;
	SOCmnList<SOCmnElement> itemList(getLeafList());
	SOCmnListPosition pos;
	pos = itemList.getStartPosition();

	while (pos)
	{
		item = (SODaCItem*)itemList.getNext(pos);

		if (item->getServerHandle() == serverHandle)
		{
			// item found
			item->addRef();
			return item;
		}
	}

	return NULL;
} // getItemByServerHandle



DWORD SODaCGroup::getCyclicTimeOut(void)
{
	DWORD updateRate = getUpdateRate();
	DWORD keepAlive = getKeepAliveTime();
	DWORD cyclicTimeout = 0;
	DWORD additionalCyclicTimeout = 0;
	
	SOCmnPointer<SODaCServer> server = (SODaCServer*)getParentBranch();
	float timeoutPercent = 50.0f;
	DWORD timeoutMin = 1000;
	DWORD timeoutMax = 10000;
	if (server.isNotNull())
	{
		timeoutPercent = server->getTimeoutPercent();
		timeoutMin = server->getTimeoutMin();
		timeoutMax = server->getTimeoutMax();
	}
	
	if (keepAlive != 0)
	{
		if (keepAlive > updateRate)
		{
			cyclicTimeout = keepAlive;
		}
		else
		{
			cyclicTimeout = updateRate;
		}
	}
	else
	{
		cyclicTimeout = updateRate;
	}

	additionalCyclicTimeout = (DWORD)((((float)(cyclicTimeout))/100.0f) * timeoutPercent);
	if (additionalCyclicTimeout < timeoutMin)
	{
		additionalCyclicTimeout = timeoutMin;
	}
	if (additionalCyclicTimeout > timeoutMax)
	{
		additionalCyclicTimeout = timeoutMax;
	}
	cyclicTimeout += additionalCyclicTimeout;
	
	return cyclicTimeout;
}



//-----------------------------------------------------------------------------
// getUpdateRate
//
// - get update rate
//
// returns:
//		update rate
//
DWORD SODaCGroup::getUpdateRate(
	OUT OPTIONAL DWORD* reqUpdateRate)  // requested update rate
{
	if (reqUpdateRate)
	{
		*reqUpdateRate = m_reqUpdateRate;
	}

	if (!isConnected())
	{
		return m_reqUpdateRate;
	}
	else
	{
		return m_revUpdateRate;
	}
} // getUpdateRate


//-----------------------------------------------------------------------------
// setReqUpdateRate
//
// - set update rate
//
// returns:
//		TRUE
//
BOOL SODaCGroup::setReqUpdateRate(
	IN DWORD updateRate)    // update rate
{
	m_validParameter |= SODAC_GRPPAR_UPDATERATE;
	m_reqUpdateRate = updateRate;
	INVOKE_CALLBACK(SODaCGroup, onSetReqUpdateRate);
	return TRUE;
} // setReqUpdateRate


//-----------------------------------------------------------------------------
// getKeepAliveTime
//
// - get update rate
//
// returns:
//		update rate
//
DWORD SODaCGroup::getKeepAliveTime(
	OUT OPTIONAL DWORD* reqKeepAliveTime)   // requested update rate
{
	if (reqKeepAliveTime)
	{
		*reqKeepAliveTime = m_reqKeepAliveTime;
	}

	if (!isConnected())
	{
		return m_reqKeepAliveTime;
	}
	else
	{
		return m_revKeepAliveTime;
	}
} // getKeepAliveTime


//-----------------------------------------------------------------------------
// setReqKeepAliveTime
//
// - set update rate
//
// returns:
//		TRUE
//
BOOL SODaCGroup::setReqKeepAliveTime(
	IN DWORD KeepAliveTime) // update rate
{
	m_validParameter |= SODAC_GRPPAR_KEEPALIVETIME;
	m_reqKeepAliveTime = KeepAliveTime;
	INVOKE_CALLBACK(SODaCGroup, onSetReqKeepAliveTime);
	return TRUE;
} // setReqKeepAliveTime


//-----------------------------------------------------------------------------
// setPublic
//
// - set public flag
//
// returns:
//		TRUE
//
BOOL SODaCGroup::setPublic(
	IN BOOL pub)    // public flag
{
	m_public = (BYTE)pub;
	INVOKE_CALLBACK(SODaCGroup, onSetPublic);
	return TRUE;
} // setPublic


//-----------------------------------------------------------------------------
// setTimeBias
//
// - set time zone
//
// returns:
//		TRUE
//
BOOL SODaCGroup::setTimeBias(
	IN LONG timeBias)   // time zone
{
	m_validParameter |= SODAC_GRPPAR_TIMEBIAS;
	m_timeBias = timeBias;
	INVOKE_CALLBACK(SODaCGroup, onSetTimeBias);
	return TRUE;
} // setTimeBias


//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//		TRUE  - set deadband
//    FALSE - invalid deadband
//
BOOL SODaCGroup::setDeadBand(
	IN FLOAT deadBand)  // deadband
{
	if ((deadBand < 0.0) || (deadBand > 100.0))
	{
		return FALSE;
	}

	m_validParameter |= SODAC_GRPPAR_DEADBAND;
	m_deadBand = deadBand;
	INVOKE_CALLBACK(SODaCGroup, onSetDeadBand);
	return TRUE;
} // setDeadBand


//-----------------------------------------------------------------------------
// setLCID
//
// - set local Id
//
// returns:
//		TRUE
//
BOOL SODaCGroup::setLCID(
	IN DWORD lcid)  // local Id
{
	m_validParameter |= SODAC_GRPPAR_LCID;
	m_lcid = lcid;
	INVOKE_CALLBACK(SODaCGroup, onSetLCID);
	return TRUE;
} // setLCID


//-----------------------------------------------------------------------------
// onSetReqUpdateRate
//
// - called before update rate is set
// - default implementation: do nothing
//
void SODaCGroup::onSetReqUpdateRate(void)
{} // onSetReqUpdateRate


//-----------------------------------------------------------------------------
// onSetRevUpdateRate
//
// - called before update rate is set
// - default implementation: do nothing
//
void SODaCGroup::onSetRevUpdateRate(void)
{} // onSetRevUpdateRate


//-----------------------------------------------------------------------------
// onSetReqKeepAliveTime
//
// - called after keep alive time is set
// - default implementation: do nothing
//
void SODaCGroup::onSetReqKeepAliveTime(void)
{} // onSetReqKeepAliveTime


//-----------------------------------------------------------------------------
// onSetRevKeepAliveTime
//
// - called after keep alive time is set
// - default implementation: do nothing
//
void SODaCGroup::onSetRevKeepAliveTime(void)
{} // onSetRevKeepAliveTime


//-----------------------------------------------------------------------------
// onSetPublic
//
// - called before public flag is set
// - default implementation: do nothing
//
void SODaCGroup::onSetPublic(void)
{} // onSetPublic


//-----------------------------------------------------------------------------
// onSetTimeBias
//
// - called before time zone is set
// - default implementation: do nothing
//
void SODaCGroup::onSetTimeBias(void)
{} // onSetTimeBias


//-----------------------------------------------------------------------------
// onSetDeadBand
//
// - called before deadband is set
// - default implementation: do nothing
//
void SODaCGroup::onSetDeadBand(void)
{} // onSetDeadBand


//-----------------------------------------------------------------------------
// onSetLCID
//
// - called before local id is set
// - default implementation: do nothing
//
void SODaCGroup::onSetLCID(void)
{} // onSetLCID


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
HRESULT SODaCGroup::doUpdate(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	return doUpdateImpl(fromServer, whatParameters);
} // update


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
HRESULT SODaCGroup::updateItems(
	IN BOOL fromServer,                 // update data from server or to server
	IN SOCmnList<SODaCItem> *itemList,      // item list
	IN OPTIONAL DWORD whatParameters)   // update what item properties
{
	return updateItemsImpl(fromServer, itemList, whatParameters);
} // updateItems


//-----------------------------------------------------------------------------
// trustServerItemHandles
//
BOOL SODaCGroup::trustServerItemHandles(void)
{
	return TRUE;
} //trustServerItemHandles


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
HRESULT SODaCGroup::syncRead(
	IN OPTIONAL SOCmnList<SODaCItem> *itemList, // item list
	IN DWORD maxAge,
	OUT OPTIONAL SOCmnVariant* pValues,
	OUT OPTIONAL WORD* pQualities,
	OUT OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	if (NULL == itemList)
	{
		itemList = (SOCmnList<SODaCItem>*)pDaCGroup->getLeafList();
	}

	SODaCItem** itemArray;
	DWORD count = itemListToArray(itemList, FALSE, itemArray);

	if (count > 0)
	{
		res = syncRead(count, itemArray, maxAge, pValues, pQualities, pTimestamps, pErrors);
	}

	freeItemArray(count, itemArray);
	return res;
}


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
HRESULT SODaCGroup::syncWrite(
	IN SOCmnList<SODaCItem> *itemList,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	// check input parameter
	if (itemList == NULL)
	{
		pDaCGroup->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	SODaCItem** itemArray;
	DWORD count = itemListToArray(itemList, FALSE, itemArray);

	if (count > 0)
	{
		res = syncWrite(count, itemArray, pValues, pQualities, pTimestamps, pErrors);
	}

	freeItemArray(count, itemArray);
	return res;
}


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
HRESULT SODaCGroup::asyncRead(
	IN SOCmnList<SODaCItem> *itemList,      // item list
	IN OPTIONAL DWORD maxAge,
	IN OPTIONAL DWORD transactionID,    // transaction id (provided by client)
	OUT OPTIONAL DWORD* cancelID,       // cancel id (provided by server)
	OUT OPTIONAL HRESULT* pErrors)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	if (NULL == itemList)
	{
		itemList = (SOCmnList<SODaCItem>*)pDaCGroup->getLeafList();
	}

	SODaCItem** itemArray;
	DWORD count = itemListToArray(itemList, FALSE, itemArray);
	itemList->autoUnlock();

	if (count > 0)
	{
		res = asyncRead(count, itemArray, maxAge, transactionID, cancelID, pErrors);
	}

	freeItemArray(count, itemArray);
	return res;
}


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
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no write advise connection
//
HRESULT SODaCGroup::asyncWrite(
	IN SOCmnList<SODaCItem> *itemList,      // item list
	IN OPTIONAL DWORD transactionID,    // transaction id (provided by client)
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	// check input parameter
	if (itemList == NULL)
	{
		pDaCGroup->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	SODaCItem** itemArray;
	DWORD count = itemListToArray(itemList, FALSE, itemArray);

	if (count > 0)
	{
		res = asyncWrite(count, itemArray, transactionID, pValues, pQualities, pTimestamps, cancelID, pErrors);
	}

	freeItemArray(count, itemArray);
	return res;
}

DWORD SODaCGroup::itemListToArray(
	IN SOCmnList<SODaCItem> *itemList,
	IN BOOL onlyStarted,
	OUT SODaCItem** &itemArray)
{
	DWORD i = 0;
	DWORD count;
	itemList->dontAutoLock();
	itemList->lock();
	count = itemList->getCount();

	if (count > 0)
	{
		SOCmnListPosition pos;
		itemArray = (SODaCItem**)malloc(count * sizeof(SODaCItem*));
		pos = itemList->getStartPosition();

		while (pos)
		{
			itemArray[i] = itemList->getNext(pos);

			if ((!onlyStarted) || (itemArray[i]->isStarted()))
			{
				itemArray[i]->addRef();
				i++;
			}
		}

		itemList->unlock();
	}
	else
	{
		itemArray = NULL;
		itemList->unlock();
	}

	return i;
}


void SODaCGroup::freeItemArray(
	IN DWORD count,
	IN SODaCItem** itemArray)
{
	DWORD i;

	for (i = 0; i < count; i++)
	{
		itemArray[i]->release();
	}

	if (itemArray)
	{
		free(itemArray);
	}
}

HRESULT SODaCGroup::syncRead(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge,
							 OUT OPTIONAL SOCmnVariant* pValues, OUT OPTIONAL WORD* pQualities,
							 OUT OPTIONAL SOCmnDateTime* pTimestamps, OUT OPTIONAL HRESULT* pErrors)
{
	return syncReadImpl(count, itemArray, maxAge, pValues, pQualities, pTimestamps, pErrors);
}
HRESULT SODaCGroup::syncWrite(IN DWORD count, IN SODaCItem** itemArray, IN OPTIONAL SOCmnVariant* pValues,
							  IN OPTIONAL WORD* pQualities, IN OPTIONAL SOCmnDateTime* pTimestamps, OUT OPTIONAL HRESULT* pErrors)
{
	return syncWriteImpl(count, itemArray, pValues, pQualities, pTimestamps, pErrors);
}
HRESULT SODaCGroup::asyncRead(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge,
							  IN OPTIONAL DWORD transactionID, OUT OPTIONAL DWORD* cancelID, OUT OPTIONAL HRESULT* pErrors)
{
	return asyncReadImpl(count, itemArray, maxAge, transactionID, cancelID, pErrors);
}
HRESULT SODaCGroup::asyncWrite(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD transactionID,
							   IN OPTIONAL SOCmnVariant* pValues, IN OPTIONAL WORD* pQualities, IN OPTIONAL SOCmnDateTime* pTimestamps,
							   OUT OPTIONAL DWORD* cancelID, OUT OPTIONAL HRESULT* pErrors)
{
	return asyncWriteImpl(count, itemArray, transactionID, pValues, pQualities, pTimestamps, cancelID, pErrors);
}
HRESULT SODaCGroup::asyncRefresh(IN DWORD maxAge, IN OPTIONAL DWORD transactionID,
								 OUT OPTIONAL DWORD* cancelID)
{
	return asyncRefreshImpl(maxAge, transactionID, cancelID);
}
HRESULT SODaCGroup::asyncCancel(IN DWORD cancelID)
{
	return asyncCancelImpl(cancelID);
}

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the OPC Server
//
// returns:
//      S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCGroup::queryInterface(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	return queryInterfaceImpl(riid, interf);
}

#endif // SOFEATURE_DCOM


BYTE SODaCGroup::getProtocol(void)
{
	SOCmnPointer<SODaCServer> server = (SODaCServer*)getParentBranch();

	if (server.isNotNull())
	{
		return server->getProtocol();
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Serialization
BOOL SODaCGroup::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;
	SOCmnList<SOCmnElement> list(getLeafList());
	SODaCItem* item;

	if (isStoring)
	{
		if (!getPersistence())
		{
			return TRUE;
		}

		// store own data
		ar << getValidParameters();
		ar << (DWORD)getConnectionType();
		ar << (DWORD)getDataRetrieval();
		ar << getName();
		DWORD updateRate;
		getUpdateRate(&updateRate);
		ar << updateRate;
		DWORD keepAlive;
		getKeepAliveTime(&keepAlive);
		ar << keepAlive;
		ar << (DWORD)getTimeBias();
		ar << getDeadBand();
		ar << (DWORD)getLCID();
		// store children
		DWORD dwCount = 0;
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			item = (SODaCItem*)list.getNext(pos);

			if (item->getPersistence())
			{
				dwCount++;
			}
		}

		ar << dwCount;
		pos = list.getStartPosition();

		while (pos && ok)
		{
			item = (SODaCItem*)list.getNext(pos);

			if (item->getPersistence())
			{
				ok = item->serialize(ar, isStoring);
			}
		}
	}
	else
	{
		// load own data
		SOCmnPointer<SODaCEntry> entry = getSODaCEntry();
		DWORD dw = 0;
		float f = 0.0;
		SOCmnString str;
		ar >> m_validParameter;
		ar >> dw;
		setConnectionType((SODaCGroup::connectionType)dw);
		ar >> dw;
		setDataRetrieval((SODaCGroup::dataRetrieval)dw);
		ar >> str;
		setName(str);
		ar >> dw;

		if (m_validParameter & SODAC_GRPPAR_UPDATERATE)
		{
			setReqUpdateRate(dw);
		}

		if (entry->getFileVersion() >= MAKELONG(20, 3))
		{
			ar >> dw;

			if (m_validParameter & SODAC_GRPPAR_KEEPALIVETIME)
			{
				setReqKeepAliveTime(dw);
			}
		}

		ar >> dw;

		if (m_validParameter & SODAC_GRPPAR_TIMEBIAS)
		{
			setTimeBias((LCID)dw);
		}

		ar >> f;

		if (m_validParameter & SODAC_GRPPAR_DEADBAND)
		{
			setDeadBand(f);
		}

		ar >> dw;

		if (m_validParameter & SODAC_GRPPAR_LCID)
		{
			setLCID((LCID)dw);
		}

		// load children
		DWORD dwCount = 0;
		ar >> dwCount;

		if (dwCount > 0)
		{
			while (dwCount-- && ok)
			{
				// create group
				item = addItem(_T(""));

				if (item)
				{
					ok = item->serialize(ar, isStoring);
					item->release();
				}
			}
		}
	}

	return ok;
} // serialize


SODaCGroup* SODaCGroup::getObjectDaCGroup(void)
{
	return this;
}

//-----------------------------------------------------------------------------
// onReadAdvise
//
// - called after got read advise (OPC V2.0)
// - default implementation: do nothing
//
void SODaCGroup::onReadAdvise(
	IN BYTE /* dataChangeOrRead */,
	IN DWORD /* transactionId */,
	IN HRESULT /* masterQuality */,
	IN HRESULT /* masterError */,
	IN DWORD /* itemCnt */,
	IN SODaCItem** /* ppItems */,
	IN SOCmnVariant* /* pValues */,
	IN WORD* /* pQualities */,
	IN SOCmnDateTime* /* pTimestamps */,
	IN HRESULT* /* pErrors */)
{} // onReadAdvise


//-----------------------------------------------------------------------------
// onWriteAdvise
//
// - called after got write advise (OPC V2.0)
// - default implementation: do nothing
//
void SODaCGroup::onWriteAdvise(
	IN DWORD /* transactionId */,
	IN HRESULT /* masterError */ ,
	IN DWORD /* itemCount */,
	IN SODaCItem** /* ppItems */,
	IN HRESULT* /* pErrors */)
{} // onWriteAdvise



//-----------------------------------------------------------------------------
// SODaCGroupICommunication                                                   |
//-----------------------------------------------------------------------------

SODaCGroupICommunication::SODaCGroupICommunication(void)
{}
SODaCGroupICommunication::~SODaCGroupICommunication(void)
{}
void SODaCGroupICommunication::init(SODaCServer* /* pServer */)
{}
HRESULT SODaCGroupICommunication::doStateTransitionImpl(BOOL /* deep */, IN BYTE /* stateDeep */)
{
	return E_NOTIMPL;
}
HRESULT SODaCGroupICommunication::doUpdateImpl(IN BOOL /* fromServer */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}
HRESULT SODaCGroupICommunication::doDisconnectImpl(void)
{
	return E_NOTIMPL;
}
HRESULT SODaCGroupICommunication::updateItemsImpl(IN BOOL /* fromServer */, IN SOCmnList<SODaCItem>* /* itemList */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}
SODaCGroup* SODaCGroupICommunication::getObjectDaCGroup(void)
{
	return NULL;
}
HRESULT SODaCGroupICommunication::syncReadImpl(
	IN DWORD /* count */,
	IN OPTIONAL SODaCItem** /* itemArray */,
	IN OPTIONAL DWORD /* maxAge */,
	OUT OPTIONAL SOCmnVariant* /* pValues */,
	OUT OPTIONAL WORD* /* pQualities */,
	OUT OPTIONAL SOCmnDateTime* /* pTimestamps */,
	OUT OPTIONAL HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::syncWriteImpl(
	IN DWORD /* count */,
	IN SODaCItem** /* itemArray */,
	IN OPTIONAL SOCmnVariant* /* pValues */,
	IN OPTIONAL WORD* /* pQualities */,
	IN OPTIONAL SOCmnDateTime* /* pTimestamps */,
	OUT OPTIONAL HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::asyncReadImpl(
	IN DWORD /* count */,
	IN OPTIONAL SODaCItem** /* itemArray */,
	IN OPTIONAL DWORD /* maxAge */,
	IN OPTIONAL DWORD /* transactionID */,
	OUT OPTIONAL DWORD* /* cancelID */,
	OUT OPTIONAL HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::asyncWriteImpl(
	IN DWORD /* count */,
	IN OPTIONAL SODaCItem** /* itemArray */,
	IN OPTIONAL DWORD /* transactionID */,
	IN OPTIONAL SOCmnVariant* /* pValues */,
	IN OPTIONAL WORD* /* pQualities */,
	IN OPTIONAL SOCmnDateTime* /* pTimestamps */,
	OUT OPTIONAL DWORD* /* cancelID */,
	OUT OPTIONAL HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::asyncRefreshImpl(
	IN DWORD /* maxAge */,
	IN OPTIONAL DWORD /* transactionID */,
	OUT OPTIONAL DWORD* /* cancelID */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::asyncCancelImpl(IN DWORD /* cancelID */)
{
	return E_NOTIMPL;
}

HRESULT SODaCGroupICommunication::validateItems(
	IN SOCmnList<SODaCItem>* /* pItemList */,
	OUT HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

#ifdef SOFEATURE_DCOM
HRESULT SODaCGroupICommunication::queryInterfaceImpl(
	IN REFIID /* riid */,
	IN void** /* interf */)
{
	return E_NOTIMPL;
}
#endif

#endif // SODAC








