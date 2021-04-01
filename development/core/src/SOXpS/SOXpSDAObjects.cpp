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
//                           OPC Toolkit - SOXpS                              |
//                                                                            |
//  Filename    : SOXpSDAObjects.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA object classes                                           |
//                - SOXpSTag: namespace tag objects                           |
//                - SOXpSProperty: namespace property objects                 |
//                - SODaSItemTagConnectQueueOneItemsStore: queue interface of tags      |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "SOXpS.h"
#include "SOXpSDAObjects.h"
#include "DllMain.h"

#define SOXPS_REQUEST_MAX_REQUESTS	100


HANDLE g_requestThreadEndEvent = INVALID_HANDLE_VALUE;
static HANDLE g_requestThreadWorkEvent = INVALID_HANDLE_VALUE;
SOCmnEvent *g_requestThreadWorkEventObj = NULL;
SOCmnList<SODaSRequest> g_requestThreadReadQueue;
SOCmnList<SODaSRequest> g_requestThreadWriteQueue;

extern BOOL g_tsChangeSendsValue;

void fillRequestData(
	BYTE ioOperation,
	SODaSRequest *req,
	SODaSItemTag *itemTag,
	SOXpSDARequestData *requestData,
	SOCmnString *itemPath)
{
	req->addRef();
	req->setUserData(SOXPS_REQUEST_STATE_NOT_HANDLED);
	// set request pending
	req->pending();

	// fill request data struct
	requestData->m_itemHandle = (SOXpSItemHandle)(SODaSItemTag *)itemTag;
	*itemPath = itemTag->getItemID();
	requestData->m_itemPath = (LPCTSTR)(*itemPath);
	requestData->m_requestHandle = (SOXpSRequestHandle)req;
	requestData->m_requestState = SOXPS_REQUEST_STATE_NOT_HANDLED;
	requestData->m_result = S_OK;

	if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
		requestData->m_itemUserData = ((SOXpSTag *)(SODaSItemTag *)itemTag)->getXpSUserData();
	else
	{
		if (itemTag->is(SOCMNOBJECT_TYPE_PROPERTY))
			requestData->m_itemUserData = ((SOXpSProperty *)(SODaSItemTag *)itemTag)->getXpSUserData();
		else
			requestData->m_itemUserData = ((SOXpSNode *)(SODaSItemTag *)itemTag)->getXpSUserData();
	}

	if (ioOperation == SOXPS_REQUEST_OPERATION_READ)
	{
		if (g_initData->m_provideCacheValueOnRead)
		{
			SOCmnPointer<SODaSCache> cache;
			SOCmnVariant cVal;
			SOCmnDateTime cTS;
			cache = req->getCache();
			cache->getValue(cVal);
			cVal.copyTo(&(requestData->m_value));
			requestData->m_quality = cache->getQuality();
			cache->getTimeStamp(cTS);
			cTS.get(&requestData->m_timeStamp);
		}
		else
		{
			::VariantInit(&(requestData->m_value));
			requestData->m_quality = 0;
			requestData->m_timeStamp.dwHighDateTime = 0;
			requestData->m_timeStamp.dwLowDateTime = 0;
		}
	}
	else
	{
		req->getWriteValue(&(requestData->m_value));
		requestData->m_quality = SOXPS_QUALITY_GOOD;
		memset(&requestData->m_timeStamp, 0, sizeof(FILETIME));
	}
}

void fillRequestData(
	BYTE ioOperation,
	DWORD *numRequests,
	SOXpSDARequestData *requestData,
	SOCmnString *itemPath,
	SOCmnList<SODaSRequest> *requestQueue,
	BYTE deviceIOModeFilter)
{
	SOCmnListPosition pos;
	SODaSRequest *req;

	// get started requests of queue
	requestQueue->lock();
	*numRequests = 0;
	pos = requestQueue->getStartPosition();
	while ((pos) && (*numRequests < SOXPS_REQUEST_MAX_REQUESTS))
	{ // while request list is not empty
		req = requestQueue->getNext(pos);

		if (req->getObjectState() == SODASREQUEST_FLAG_STARTED)
		{
			SOCmnPointer<SODaSItemTag> itemTag;
			itemTag = req->getItemTag();
			if (deviceIOModeFilter)
			{
				if (itemTag->getDeviceIOMode(req) != deviceIOModeFilter)
					continue;
			}

			fillRequestData(ioOperation, req, itemTag, &requestData[*numRequests], &itemPath[*numRequests]);
			(*numRequests)++;
		}
	}
	requestQueue->unlock();
}


void removeHandledRequests(
	SOCmnList<SODaSRequest> *requestQueue)
{
	SOCmnListPosition pos, curPos;
	SODaSRequest *req;

	requestQueue->lock();
	pos = requestQueue->getStartPosition();
	while (pos)
	{
		curPos = pos;
		req = requestQueue->getNext(pos);

		if ((req->isCompleted()) &&
			(req->getUserData() != SOXPS_REQUEST_STATE_NOT_HANDLED) )
		{
			requestQueue->removeAt(curPos);
		}
	}
	requestQueue->unlock();
}


BOOL processRequestData(
	IN DWORD numRequests,
	IN SOXpSDARequestData *requestData)
{
	SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root = entry->getNameSpaceRoot();
	SOCmnPointer<SODaSCache> cache;
	SODaSItemTag *itemTag;
	SOCmnPointer<SODaSTag> tag;
	SOCmnPointer<SODaSProperty> prop;
	SOCmnPointer<SODaSNode> node;
	SOCmnElement *propParent = NULL;
	SODaSRequest *req;
	SOCmnString itemId;
	DWORD i;
	BOOL ret = TRUE;
	SOCmnVariant value;
	SOCmnDateTime timeStamp;

	for (i = 0; i < numRequests; i++)
	{
		if (requestData[i].m_requestState == SOXPS_REQUEST_STATE_HANDLED)
		{
			if (requestData[i].m_requestHandle)
			{
				req = (SODaSRequest *)requestData[i].m_requestHandle;
				req->setUserData(SOXPS_REQUEST_STATE_HANDLED);
				if (req->getOperation() == SODaSRequest::read)
				{
					cache = req->getCache();
					if (cache.isNotNull())
					{
						value = &requestData[i].m_value;
						cache->setValue(value);
						cache->setQuality(requestData[i].m_quality);
						if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
							(requestData[i].m_timeStamp.dwHighDateTime != 0))
						{
							timeStamp = &requestData[i].m_timeStamp;
							cache->setTimeStamp(timeStamp);
						}
					}

					// only with 'TimestampChangeSendsValue != 0'
					if (g_tsChangeSendsValue)	{
						SOCmnPointer<SOXpSTag> tag = (SOXpSTag*)req->getItemTag();

						SOCmnList<SOCmnObject> items;

						if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
							(requestData[i].m_timeStamp.dwHighDateTime != 0))
						{
							items.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
							((SOXpSTag*)tag)->getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, items);

							SOCmnListPosition pos = items.getStartPosition();
							while(pos)	{
								SOXpSItem* pItem = (SOXpSItem*)items.getNext(pos);
								pItem->clearAllSendTimeStamps();
							}
						}
					}


					req->complete(requestData[i].m_result);
				}
				else
				{
					BOOL updateCacheWithWrite = (requestData[i].m_value.vt != VT_EMPTY);

					req->complete(requestData[i].m_result, TRUE, requestData[i].m_quality, FALSE, updateCacheWithWrite);
					if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
						(requestData[i].m_timeStamp.dwHighDateTime != 0))
					{
						cache = req->getCache();
						if (cache.isNotNull())
						{
							timeStamp = &requestData[i].m_timeStamp;
							cache->setTimeStamp(timeStamp);
						}
					}
				}
				req->release();
			}
			else
			{ // no request
				if (requestData[i].m_itemHandle == NULL)
				{
					SOCmnPointer<SODaSObjectRoot> oroot = (SODaSObjectRoot *)entry->getObjectRoot();
					SOCmnList<SODaSServer> *srvList = oroot->getServerList();
					SODaSServer *srv = srvList->getHead();;
					if (srv)
						srv->getItemTag(requestData[i].m_itemPath, &itemTag);
				}
				else
				{
					itemTag = (SODaSItemTag *)requestData[i].m_itemHandle;
					itemTag->addRef();
				}

				if (itemTag)
				{
					if (itemTag->getUpdateMode() != SODaSItemTag::report)
					{
						cache = itemTag->getCache();
						value = &requestData[i].m_value;
						cache->setValue(value);
						cache->setQuality(requestData[i].m_quality);
						if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
							(requestData[i].m_timeStamp.dwHighDateTime != 0))
						{
							timeStamp = &requestData[i].m_timeStamp;
							cache->setTimeStamp(timeStamp);
						}
					}
					else
					{
						value = &requestData[i].m_value;
						if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
							(requestData[i].m_timeStamp.dwHighDateTime != 0))
						{
							timeStamp = &requestData[i].m_timeStamp;
							itemTag->notifyAllConnectedObjects(&value, requestData[i].m_quality, &timeStamp);
						}
						else
							itemTag->notifyAllConnectedObjects(&value, requestData[i].m_quality);
					}

					// only with 'TimestampChangeSendsValue != 0'
					if (g_tsChangeSendsValue)	{
						SOCmnList<SOCmnObject> items;

						if ((requestData[i].m_timeStamp.dwLowDateTime != 0) ||
							(requestData[i].m_timeStamp.dwHighDateTime != 0))
						{
							items.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
							((SOXpSTag*)itemTag)->getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, items);

							SOCmnListPosition pos = items.getStartPosition();
							while(pos)	{
								SOXpSItem* pItem = (SOXpSItem*)items.getNext(pos);
								pItem->clearAllSendTimeStamps();
							}
						}
					}

					itemTag->release();
				}
				else
				{
					ret = FALSE;
					SetLastError(ERROR_PATH_NOT_FOUND);
				}
			}
		}
		else if (requestData[i].m_requestState == SOXPS_REQUEST_STATE_HANDLED_USE_CACHE)
		{
			SOCmnPointer<SODaSItemTag> itemTag;
			if (requestData[i].m_requestHandle)
			{
				req = (SODaSRequest *)requestData[i].m_requestHandle;
				req->setUserData(SOXPS_REQUEST_STATE_HANDLED);
				if (req->getOperation() == SODaSRequest::read)
				{
					req->complete(requestData[i].m_result);
				}
				req->release();
			}
			else
			{
				ret = FALSE;
				SetLastError(ERROR_INVALID_HANDLE);
			}
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
// requestThreadMain
//
// - thread routine of request thread (queued I/O)
//
// returns:
//		thread end state
//
DWORD WINAPI requestThreadMain(
	IN LPVOID context)
{
	HANDLE eventWait[2];
	BOOL end = FALSE;
	BOOL doWait = TRUE;
	DWORD ret;
	SOXpSDARequestData requestData[SOXPS_REQUEST_MAX_REQUESTS];
	SOCmnString itemPath[SOXPS_REQUEST_MAX_REQUESTS];
	DWORD numRequests;
	DWORD i;

	if (g_fpSOXpSHandleDARequests == NULL)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("No SOXpSHandleDARequests but started request thread")));
		return -1;
	}

	// create events
	SOCmnEventPool *eventPool = getEventPool();
	g_requestThreadEndEvent = eventPool->popEvent();
	g_requestThreadWorkEvent = eventPool->popEvent();
	g_requestThreadWorkEventObj = new SOCmnEvent();
	g_requestThreadWorkEventObj->setHandle(g_requestThreadWorkEvent);
	eventWait[SOCMN_EVENT_WORK] = g_requestThreadWorkEvent;
	eventWait[SOCMN_EVENT_END] = g_requestThreadEndEvent;

	// create queues
	g_requestThreadReadQueue.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	g_requestThreadWriteQueue.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);

	while(!end)
	{
		SOCMN_TRY
		{
			if (doWait)
				ret = ::WaitForMultipleObjectsEx(2, eventWait, FALSE, 300000, TRUE);
			else
				ret = WAIT_OBJECT_0 + SOCMN_EVENT_WORK;

			doWait = TRUE;
			switch (ret - WAIT_OBJECT_0)
			{
				case SOCMN_EVENT_END:
					end = TRUE;
				break;
				default:
				{
					// fill request data struct
					fillRequestData(SOXPS_REQUEST_OPERATION_WRITE, &numRequests, requestData, itemPath, &g_requestThreadWriteQueue, 0);
					if (numRequests == SOXPS_REQUEST_MAX_REQUESTS)
						doWait = FALSE;

					// call DLL
					if (numRequests > 0)
					{
						g_fpSOXpSHandleDARequests(SOXPS_REQUEST_OPERATION_WRITE, numRequests, requestData);
						processRequestData(numRequests, requestData);
						for (i = 0; i < numRequests; i++)
							::VariantClear(&requestData[i].m_value);
					}

					// remove all completed request
					removeHandledRequests(&g_requestThreadWriteQueue);


					// fill request data struct
					fillRequestData(SOXPS_REQUEST_OPERATION_READ, &numRequests, requestData, itemPath, &g_requestThreadReadQueue, 0);
					if (numRequests == SOXPS_REQUEST_MAX_REQUESTS)
						doWait = FALSE;

					// call DLL
					if (numRequests > 0)
					{
						g_fpSOXpSHandleDARequests(SOXPS_REQUEST_OPERATION_READ, numRequests, requestData);
						processRequestData(numRequests, requestData);
						for (i = 0; i < numRequests; i++)
							::VariantClear(&requestData[i].m_value);
					}

					// remove all completed request
					removeHandledRequests(&g_requestThreadReadQueue);
				}
				break;
			}
		}
		SOCMN_CATCH(...)
		{
		}
	}

	g_requestThreadReadQueue.destroy();
	g_requestThreadWriteQueue.destroy();

	::CloseHandle(g_requestThreadEndEvent);
	delete g_requestThreadWorkEventObj;

	return 0;
} // requestThreadMain


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void handleRequest(
	IN SODaSItemTag *pItemTag, // item tag
	IN SODaSRequest *pReq)     // request
{
	BYTE ioOp = (pReq->getOperation() == SODaSRequest::read) ? SOXPS_REQUEST_OPERATION_READ : SOXPS_REQUEST_OPERATION_WRITE;
	SOXpSDARequestData requestData;
	SOCmnString itemPath;

	// fill request data struct
	fillRequestData(ioOp, pReq, pItemTag, &requestData, &itemPath);

	// call DLL
	g_fpSOXpSHandleDARequests(ioOp, 1, &requestData);
	processRequestData(1, &requestData);
	::VariantClear(&requestData.m_value);
} // handleRequest




//-----------------------------------------------------------------------------
// SOXpSNode                                                                  |
//-----------------------------------------------------------------------------

SOXpSNode::SOXpSNode(void)
{
	m_xpsUserData = 0;
	m_deviceState = SOWATCH_DEVSTATE_OK;
}
SOCmnString SOXpSNode::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSNode::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SOXpSNode::handleRequest(
    IN SODaSRequest *request)   // request
{
	::handleRequest((SODaSItemTag *)this, request);
} // handleRequest

//-----------------------------------------------------------------------------
// setUpdateMode
//
// - set the update mode of the item tag
//
void SOXpSNode::setUpdateMode(
    IN BYTE updateMode) // update mode
{
	if (updateMode == SODaSItemTag::report)
		createItemList();
	SODaSNode::setUpdateMode(updateMode);
} // setDeviceIOMode



//-----------------------------------------------------------------------------
// SOXpSTag                                                                   |
//-----------------------------------------------------------------------------

SOXpSTag::SOXpSTag()
{
	m_xpsUserData = 0;
	m_deviceState = SOWATCH_DEVSTATE_OK;
}

SOCmnString SOXpSTag::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSTag::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SOXpSTag::handleRequest(
    IN SODaSRequest *request)   // request
{
	::handleRequest((SODaSItemTag *)this, request);
} // handleRequest

//-----------------------------------------------------------------------------
// setUpdateMode
//
// - set the update mode of the item tag
//
void SOXpSTag::setUpdateMode(
    IN BYTE updateMode) // update mode
{
	if (updateMode == SODaSItemTag::report || g_tsChangeSendsValue)
		createItemList();
	SODaSTag::setUpdateMode(updateMode);
} // setDeviceIOMode


//-----------------------------------------------------------------------------
// SOXpSProperty                                                              |
//-----------------------------------------------------------------------------

SOXpSProperty::SOXpSProperty(void)
 : SODaSProperty()
{
	m_xpsUserData = 0;
	m_deviceState = SOWATCH_DEVSTATE_OK;
}
SOCmnString SOXpSProperty::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSProperty::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SOXpSProperty::handleRequest(
    IN SODaSRequest *request)   // request
{
	::handleRequest((SODaSItemTag *)this, request);
} // handleRequest

//-----------------------------------------------------------------------------
// setUpdateMode
//
// - set the update mode of the item tag
//
void SOXpSProperty::setUpdateMode(
    IN BYTE updateMode) // update mode
{
	if (updateMode == SODaSItemTag::report)
		createItemList();
	SODaSProperty::setUpdateMode(updateMode);
} // setDeviceIOMode



//-----------------------------------------------------------------------------
// SODaSItemTagConnectQueueOneItemsStore                                      |
//-----------------------------------------------------------------------------

SODaSItemTagConnectQueueOneItemsStore::SODaSItemTagConnectQueueOneItemsStore()
{
	m_activeItems = 0;
}

SODaSItemTagConnectQueueOneItemsStore::~SODaSItemTagConnectQueueOneItemsStore()
{
	m_itemList.destroy();
}

//-----------------------------------------------------------------------------
// getReadEvent
//
// - get event for read request queue
//
// returns:
//		handle of read event
//
SOCmnEventPointer SODaSItemTagConnectQueueOneItemsStore::getReadEvent(void)
{
	return SOCmnEventPointer(g_requestThreadWorkEventObj);
} // getReadEvent


//-----------------------------------------------------------------------------
// getWriteEvent
//
// - get event for write request queue
//
// returns:
//		handle of write event
//
SOCmnEventPointer SODaSItemTagConnectQueueOneItemsStore::getWriteEvent(void)
{
	return SOCmnEventPointer(g_requestThreadWorkEventObj);
} // getWriteEvent


//-----------------------------------------------------------------------------
// getReadQueue
//
// - get read request queue
//
// returns:
//		pointer to read request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagConnectQueueOneItemsStore::getReadQueue(void)
{
	return &g_requestThreadReadQueue;
} // getReadQueue


//-----------------------------------------------------------------------------
// getWriteQueue
//
// - get write request queue
//
// returns:
//		pointer to write request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagConnectQueueOneItemsStore::getWriteQueue(void)
{
	return &g_requestThreadWriteQueue;
} // getWriteQueue


//-----------------------------------------------------------------------------
// createItemList
//
// - create list to store the connected items
//
void SODaSItemTagConnectQueueOneItemsStore::createItemList(void)
{
	if (!m_itemList.isInit())
		m_itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
} // createItemList


//-----------------------------------------------------------------------------
// getAllConnectedObjects
//
// - get all items connected to the item tag
//
void SODaSItemTagConnectQueueOneItemsStore::getAllConnectedObjects(
    IN DWORD objType,                       // object type
    OUT SOCmnList<SOCmnObject> &objList)    // object list
{
    if (objType != SOCMNOBJECT_TYPE_ITEM)
        return;

    SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
    objList.clone((SOCmnList<SOCmnObject> *)&m_itemList);
} // getAllConnectedObjects


//-----------------------------------------------------------------------------
// addConnectedObject
//
// - add connected to the list of the item tag
//
void SODaSItemTagConnectQueueOneItemsStore::addConnectedObject(
    IN SOCmnObject *obj)    // connected object
{
	if (m_itemList.isInit())
	{
    if (obj)
    {
      if (obj->is(SOCMNOBJECT_TYPE_ITEM))
      {
        SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
        m_itemList.add((SODaSItem *) obj);
      }
    }
	}
} // addItem


//-----------------------------------------------------------------------------
// removeConnectedObject
//
// - remove connected from the list of the item tag
//
void SODaSItemTagConnectQueueOneItemsStore::removeConnectedObject(
    IN SOCmnObject *obj)    // connected object
{
	if (m_itemList.isInit())
	{
    if (obj)
    {
      if (obj->is(SOCMNOBJECT_TYPE_ITEM))
      {
        SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
        m_itemList.removeObject((SODaSItem *)obj);
      }
    }
	}
} // removeItem


//-----------------------------------------------------------------------------
// activateConnectedObject
//
// - activate connected object
//
void SODaSItemTagConnectQueueOneItemsStore::activateConnectedObject(
	IN SOCmnObject *obj)    // connected object
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			if (m_activeItems == 0)
				if (g_fpSOXpSActivateDAItems)
				{
					SOXpSDAItemData itemData;
					SODaSItemTag *pObj = (SODaSItemTag *)getObjectDaSItemTagIConnect();
					SOCmnString itemID = pObj->getItemID();

					itemData.m_itemHandle = (SOXpSItemHandle)pObj;
					itemData.m_itemPath = (LPCTSTR)itemID;
					if (pObj->is(SOCMNOBJECT_TYPE_TAG))
						itemData.m_itemUserData = ((SOXpSTag *)pObj)->getXpSUserData();
					else
					{
						if (pObj->is(SOCMNOBJECT_TYPE_PROPERTY))
							itemData.m_itemUserData = ((SOXpSProperty *)pObj)->getXpSUserData();
						else
							itemData.m_itemUserData = ((SOXpSNode *)pObj)->getXpSUserData();
					}

					g_fpSOXpSActivateDAItems(TRUE, 1, &itemData);
				}
			m_activeItems++;
		}
	}
} // activateConnectedObject


//-----------------------------------------------------------------------------
// deactivateConnectedObject
//
// - deactivate connected object
//
void SODaSItemTagConnectQueueOneItemsStore::deactivateConnectedObject(
	IN SOCmnObject *obj)    // connected object
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			m_activeItems--;
			if (m_activeItems == 0)
				if (g_fpSOXpSActivateDAItems)
				{
					SOXpSDAItemData itemData;
					SODaSItemTag *pObj = (SODaSItemTag *)getObjectDaSItemTagIConnect();
					SOCmnString itemID = pObj->getItemID();

					itemData.m_itemHandle = (SOXpSItemHandle)pObj;
					itemData.m_itemPath = (LPCTSTR)itemID;
					if (pObj->is(SOCMNOBJECT_TYPE_TAG))
						itemData.m_itemUserData = ((SOXpSTag *)pObj)->getXpSUserData();
					else
					{
						if (pObj->is(SOCMNOBJECT_TYPE_PROPERTY))
							itemData.m_itemUserData = ((SOXpSProperty *)pObj)->getXpSUserData();
						else
							itemData.m_itemUserData = ((SOXpSNode *)pObj)->getXpSUserData();
					}

					g_fpSOXpSActivateDAItems(FALSE, 1, &itemData);
				}
		}
	}
} // deactivateConnectedObject




//-----------------------------------------------------------------------------
// SOXpSTransaction                                                           |
//-----------------------------------------------------------------------------

SOXpSTransaction::SOXpSTransaction(IN SODaSGroup *group, IN BYTE type, IN BYTE connectionType, IN OPTIONAL DWORD connection)
 : SODaSTransaction(group, type, connectionType, connection)
{}

//-----------------------------------------------------------------------------
// handleRequests
//
// - handle all transaction requests of the transaction
//
void SOXpSTransaction::handleRequests(
	IN SOCmnList<SODaSRequest> *reqList)
{
	BYTE ioOp = (isRead()) ? SOXPS_REQUEST_OPERATION_READ : SOXPS_REQUEST_OPERATION_WRITE;
	DWORD numRequests;
	SOXpSDARequestData requestData[SOXPS_REQUEST_MAX_REQUESTS];
	SOCmnString itemPath[SOXPS_REQUEST_MAX_REQUESTS];
	DWORD i;

	do
	{
		numRequests = 0;

		// fill request data struct
		fillRequestData(ioOp, &numRequests, requestData, itemPath, reqList, SODaSItemTag::transaction);

		// call DLL
		if (numRequests > 0)
		{
			g_fpSOXpSHandleDARequests(ioOp, numRequests, requestData);
			processRequestData(numRequests, requestData);
			for (i = 0; i < numRequests; i++)
				::VariantClear(&requestData[i].m_value);
		}
	} while (numRequests == SOXPS_REQUEST_MAX_REQUESTS);
} // handleRequests



//-----------------------------------------------------------------------------
// SOXpSDAServer                                                              |
//-----------------------------------------------------------------------------

extern HANDLE g_startupEvent;


SOXpSDAServer::SOXpSDAServer(void)
{}

//-----------------------------------------------------------------------------
// waitForNameSpaceStartup
//
// - wait for the name space creation to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOXpSDAServer::waitForNameSpaceStartup(void)
{
	WaitForSingleObject(g_startupEvent, INFINITE);
	return S_OK;
} // waitForNameSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOXpSDAServer::waitForIOStartup(void)
{
	WaitForSingleObject(g_startupEvent, INFINITE);
	return S_OK;
} // waitForIOStartup


//-----------------------------------------------------------------------------
// SOXpSDARequest                                                              |
//-----------------------------------------------------------------------------

extern BOOL g_tsChangeSendsValue;

//-----------------------------------------------------------------------------
// hasChanged
//
// - check if the value of the requests cache has changed since last advise
//
// returns:
//		TRUE  - changed
//		FALSE - same
//
BOOL SOXpSDARequest::hasChanged(IN DWORD connection, IN SOCmnVariant &value, IN WORD quality)
{
	if (g_tsChangeSendsValue)	{
		BOOL ret = SODaSRequest::hasChanged(connection, value, quality);

		// return TRUE immediately, when value or quality changed
		if (ret) return ret;

		SOCmnPointer<SOXpSItem> pItem = (SOXpSItem*)getItem();
		if (pItem.isNotNull())
		{
			SOCmnString szlastTS, szCurTS, szNullTS;
			SOCmnDateTime lastTS, curTS, nullTS;

			pItem->getSendTimeStamp(connection, &szlastTS);

			szNullTS = nullTS.getXMLDateTime();

			return (SOCmnString::areEqual(szNullTS, szlastTS) || szlastTS.isEmpty());

/*			SOCmnPointer<SODaSCache> cache = this->getCache();
			if (cache.isNotNull())
			{

				cache->getTimeStamp(curTS);
				szCurTS = curTS.getXMLDateTime();



				BOOL bChanged = !lastTS.setXMLDateTime(szlastTS);

				if (!bChanged)	bChanged = !SOCmnString::areEqual(szCurTS, szlastTS);

				return bChanged;
			}
*/
		}

		return ret;
	}

	return SODaSRequest::hasChanged(connection, value, quality);
}


//-----------------------------------------------------------------------------
// SOXpSItem                                                        |
//-----------------------------------------------------------------------------

SOCmnSync s_tsSync;

SOXpSItem::~SOXpSItem()
{
	destroyCyclicData();
}

//-----------------------------------------------------------------------------
// createCyclicData
//
// - create cyclic data
//
void SOXpSItem::createCyclicData(void)
{
	SODaSItemDefault::createCyclicData();

	s_tsSync.lock();
	m_sendTimeStamps.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_STRING);
	s_tsSync.unlock();
} // createCyclicData


//-----------------------------------------------------------------------------
// destroyCyclicData
//
// - destroy cyclic data
//
void SOXpSItem::destroyCyclicData(void)
{
	SODaSItemDefault::destroyCyclicData();

	s_tsSync.lock();
	m_sendTimeStamps.destroy();
	s_tsSync.unlock();
} // destroyCyclicData

/*
//-----------------------------------------------------------------------------
// getSendValue
//
// - get the value last send to one client
//
// returns:
//		TRUE  - got value
//		FALSE - can't get value set
//
BOOL SOXpSItem::getSendValue(
	IN DWORD connection,	// connection id
	OUT SOCmnVariant &value,	// send value
	OUT WORD *quality)		// send quality
{
	BOOL ret = FALSE;

	if ((ret = SODaSItemDefault::getSendValue(connection, value, quality)) == TRUE)
	{
		if (!m_sendTimeStamps.isInit())
			return FALSE;

		s_cyclicSync.lock();

		s_cyclicSync.unlock();
	}

	return ret;
} // getSendValue
*/

//-----------------------------------------------------------------------------
// getSendTimeStamp
//
// - get the timestamp of last send value to one client
//
// returns:
//		TRUE  - got ts
//		FALSE - can't get value set
//
BOOL SOXpSItem::getSendTimeStamp(
 	IN DWORD connection,	// connection id
	IN SOCmnString* pTS)	// send value
{
	BOOL ret;

	if (!m_sendTimeStamps.isInit())
		return FALSE;

	s_tsSync.lock();
	ret = m_sendTimeStamps.findKey(connection, pTS);
	s_tsSync.unlock();

	return ret;
}

//-----------------------------------------------------------------------------
// setSendValue
//
// - set the value send to one client
//
// returns:
//		TRUE  - value set
//		FALSE - can't set value set
//
BOOL SOXpSItem::setSendValue(
	IN DWORD connection,	// connection id
	IN SOCmnVariant &value,	// send value
	IN WORD quality)		// send quality
{
	SOCmnDateTime sendTS; sendTS.now();
	BOOL ret = FALSE;

	if ((ret = SODaSItemDefault::setSendValue(connection, value, quality)) == TRUE)
	{
		if (!m_sendTimeStamps.isInit())
			return FALSE;

		s_tsSync.lock();
		m_sendTimeStamps.update(sendTS.getXMLDateTime(), connection);
		s_tsSync.unlock();
	}

	return ret;
} // setSendValue


