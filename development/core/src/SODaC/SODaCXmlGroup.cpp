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
//  Filename    : SODaCXmlGroup.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Group object related classes                                |
//                - SODaCXmlGroupImpl: proxy for OPC group object             |
//                - SODaCXmlGroupImplDefault: default group class for creator |
//                - SODaCTaskSyncRead: synchron read task                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_SOAP

#include <opcda.h>
#include "SODaCServer.h"
#include "SODaCXmlGroup.h"
#include "SODaCXmlCall.h"
#include "SODaCItem.h"
#include "SODaCEntry.h"


//-----------------------------------------------------------------------------
// SODaCXmlGroupImpl                                                          |
//-----------------------------------------------------------------------------

SODaCXmlGroupImpl::SODaCXmlGroupImpl(void)
{
}

void SODaCXmlGroupImpl::init(SODaCServer* pServer)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	pDaCGroup->m_dataRetrieval = SODaCGroup::drPolledRefresh;
	SOCmnPointer<SODaCXmlTaskPolledRefresh> task;
	SOCMN_ALLOCATE_OBJECT_C(task, SODaCXmlTaskPolledRefresh(pDaCGroup, this), (SODaCXmlTaskPolledRefresh*));
	pServer->getWorkerThread()->addCyclicTask((SODaCXmlTaskPolledRefresh*)task, &m_taskPolledRefresh);
}

SODaCXmlGroupImpl::~SODaCXmlGroupImpl(void)
{
	if (m_httpCyclicCon.isNotNull())
	{
		m_httpCyclicCon->disconnect();
		m_httpCyclicCon.release();
	}

	m_polledRefreshReply.release();
}


//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the group if the target state differs from the
//   actual state of the group

HRESULT SODaCXmlGroupImpl::doStateTransitionImpl(BOOL deep, IN BYTE stateDeep)
{
	SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
	SOCmnPointer<SOCltHTTP> httpClient = entryDaC->getHTTPClient();
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	BYTE targetState = pDaCGroup->getTargetObjectState();
	HRESULT res = S_OK;
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> httpCall;
	SOCmnString sHandle;

	// check server object
	if (!server)
	{
		pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
		return E_FAIL;
	}

	if ((deep) && (stateDeep != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE))
	{
		pDaCGroup->setItemsTargetState(targetState);
	}

	if (!server->isConnected())
	{
		pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
		return E_FAIL;
	}

	if (((pDaCGroup->getObjectState() == SOCltElement::started) && (pDaCGroup->getTargetObjectState() == SOCltElement::started)) ||
		((pDaCGroup->getObjectState() == SOCltElement::connected) && (pDaCGroup->getTargetObjectState() == SOCltElement::connected)))
	{
		// check for items to add to subscription
		SOCmnList<SODaCItem> itemList((SOCmnList<SODaCItem> *)pDaCGroup->getLeafList());
		SOCmnListPosition posI;
		SODaCItem* pItem;
		posI = itemList.getStartPosition();

		while (posI)
		{
			pItem = itemList.getNext(posI);

			if (pItem->getObjectState() != pItem->getTargetObjectState())
			{
				itemList.autoUnlock();

				if (pDaCGroup->performStateTransitionAction(SODAC_ITEM_ACTION_GET_ITEM_PARS))
				{
					pDaCGroup->updateItems(TRUE, (SOCmnList<SODaCItem> *)pDaCGroup->getLeafList(), SODAC_ITMPAR_ALL);
				}

				if (pDaCGroup->getTargetObjectState() == SOCltElement::connected)
				{
					break;
				}

				// disconnect current subscription
				itemList.autoUnlock();
				cancelSubscription(SOCltElement::disconnected);
				// connect
				res = establishSubscription();
				break;
			}
		}
	}

	while ((SUCCEEDED(res)) && (pDaCGroup->getObjectState() != targetState))
	{
		pDaCGroup->setStateTransitionActive(TRUE);

		switch (pDaCGroup->getObjectState())
		{
		case SOCltElement::disconnected:
			if (pDaCGroup->performStateTransitionAction(SODAC_ITEM_ACTION_GET_ITEM_PARS))
			{
				pDaCGroup->updateItems(TRUE, (SOCmnList<SODaCItem> *)pDaCGroup->getLeafList(), SODAC_ITMPAR_ALL);
			}

			// intentional fall through
		case SOCltElement::connected:
		{
			if (targetState == SOCltElement::started)
			{
				res = establishSubscription();
			}
			else
			{
				// nothing to do for other state transitions
				pDaCGroup->setObjectState(targetState);
				pDaCGroup->setItemsState(targetState);
			}
		}
		break;

		case SOCltElement::started:
		{
			if ((targetState == SOCltElement::disconnected) ||
				(targetState == SOCltElement::connected))
			{
				res = cancelSubscription(targetState);
			}
		}
		break;
		}

		pDaCGroup->setStateTransitionActive(FALSE);
	}

	if (pDaCGroup->getObjectState() != SOCltElement::disconnected)
	{
		SOCmnList<SODaCItem> itemList((SOCmnList<SODaCItem> *)pDaCGroup->getLeafList());
		SOCmnListPosition posI;
		SODaCItem* pItem;
		// correct the disconnected and connected states
		posI = itemList.getStartPosition();

		while (posI)
		{
			pItem = itemList.getNext(posI);

			if ((pItem->getObjectState() == SOCltElement::disconnected) &&
				(pItem->getTargetObjectState() != SOCltElement::disconnected))
			{
				pItem->setObjectState(SOCltElement::connected);
			}

			if ((pItem->getObjectState() != SOCltElement::disconnected) &&
				(pItem->getTargetObjectState() == SOCltElement::disconnected))
			{
				pItem->setObjectState(SOCltElement::disconnected);
			}
		}
	}

	return res;
} // doStateTransition


HRESULT SODaCXmlGroupImpl::cancelSubscription(IN BYTE targetState)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
	SOCmnString sHandle;
	HRESULT res = S_OK;
	// disconnect current subscription
	sHandle = getServerSubHandle();

	if (!sHandle.isEmpty())
	{
		res = doSubscriptionCancel(httpCon, targetState);
		setServerSubHandle(_T(""));
	}
	else
	{
		pDaCGroup->setObjectState(targetState);
		pDaCGroup->setItemsState(targetState);
	}

	return res;
}

HRESULT SODaCXmlGroupImpl::establishSubscription(void)
{
	SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
	SOCmnPointer<SOCltHTTP> httpClient = entryDaC->getHTTPClient();
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> httpCall;
	SOCmnString sHandle;
	HRESULT res = S_OK;
	res = doSubscribe(httpCon);
	sHandle = getServerSubHandle();

	if ((SUCCEEDED(res)) && (!sHandle.isEmpty()))
	{
		SODaCXmlRequestOptions* pReqOpts;
		SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(m_httpCyclicCon), (SODaCXmlCall*));
		SOCMN_ALLOCATE_OBJECT_C(pReqOpts, SODaCXmlRequestOptions(), (SODaCXmlRequestOptions*));
		pReqOpts->m_returnItemTime = TRUE;
		httpCall->setRequestOptions(pReqOpts);
		httpCall->subscriptionPolledRefresh(this, (pDaCGroup->getDataRetrieval() == SODaCGroup::drPolledRefresh));
		res = m_httpCyclicCon->invoke(httpCall, FALSE);
		httpCall.release();

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SOCmnObject> socket(m_httpCyclicCon->getSocket());
			httpClient->resetIdleTimer(socket);
			httpClient->setTimeOut(socket, pDaCGroup->getCyclicTimeOut());
		}
	}

	return res;
}

HRESULT SODaCXmlGroupImpl::doSubscribe(IN SOCltHTTPConnection* httpCon)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCXmlCall> httpCall;
	SODaCXmlRequestOptions reqOpts;
	HRESULT res;
	DWORD itemCnt = 0;

	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	reqOpts.m_returnItemTime = TRUE;
	httpCall->subscribe(pDaCGroup, &reqOpts, &itemCnt);

	if (itemCnt == 0)
	{
		pDaCGroup->setObjectState(SOCltElement::started);
		return S_OK;
	}

	pDaCGroup->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCGroup->afterInterfaceCall();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();

	DWORD reqTimeout = httpCall->getTimeout();

	if ((reqTimeout == INFINITE) || ((reqTimeout == SOCMN_TIME_40DAYS)))
	{
		if (server.isNotNull())
		{
			reqTimeout = server->getRequestTimeout();
		}
	}

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getSubscribeResponse(&response);

		if (SUCCEEDED(res))
		{
			pDaCGroup->setObjectState(SOCltElement::started);
			SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
			SOCmnPointer<SOCltHTTP> httpClient = entryDaC->getHTTPClient();

			if (m_httpCyclicCon.isNotNull())
			{
				SOCmnPointer<SOCmnObject> socket(m_httpCyclicCon->getSocket());
				httpClient->setTimeOut(socket, reqTimeout);
				m_httpCyclicCon->disconnect();
				m_httpCyclicCon.release();
			}

			SOCMN_ALLOCATE_OBJECT_C(m_httpCyclicCon, SOCltHTTPConnection(), (SOCltHTTPConnection*));
			m_httpCyclicCon->setServer(server);
			m_httpCyclicCon->setDefaultURL(server->getHTTPPath());
			m_httpCyclicCon->setPort(server->getIPPort());
			SOCmnString hostName = server->getNodeName();

			if (!hostName.isEmpty())
			{
				m_httpCyclicCon->setHostName(hostName);
			}

			httpClient->addConnection(m_httpCyclicCon);
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			SOCmnListPosition pos;
			SOCmnPointer<SODaCItem> item;
			BOOL initV;
			SOCmnVariant itemValue;
			HRESULT itemResult;
			WORD itemQuality;
			SOCmnDateTime itemTimestamp;
			SOCmnString convHlp;

			if (xmlOptions.findKey(_T("ServerSubHandle"), &convHlp))
			{
				setServerSubHandle(convHlp);
			}

			pDaCGroup->m_revUpdateRate = pDaCGroup->m_reqUpdateRate;
			pos = xmlData.getStartPosition();

			while (pos)
			{
				pResData = xmlData.getNext(pos);
				item = pDaCGroup->getItemByClientHandle(_tcstoul(pResData->m_clientItemHandle, NULL, 16));

				if (item.isNotNull())
				{
					if (pResData->m_resultID.isEmpty())
					{
						itemResult = S_OK;
					}
					else
					{
						itemResult = httpCall->getErrorCodeFromXML(pResData->m_resultID);
					}

					if (SUCCEEDED(itemResult))
					{
						item->setObjectState(SOCltElement::started);
						initV = itemValue.initFromXMLValue(&pResData->m_value);
						itemQuality = httpCall->getDCOMQuality(pResData->m_quality, pResData->m_qualityLimit, pResData->m_qualityVendor);
						itemTimestamp.setXMLDateTime(pResData->m_timestamp);

						if (initV)
						{
							if (item->getNativeDatatype() == VT_EMPTY)
							{
								item->setNativeDatatype(itemValue.vt);
							}

							item->setRead(itemResult, &itemValue, itemQuality, &itemTimestamp, 0);
						}
					}

					if (FAILED(itemResult))
					{
						item->setRead(itemResult, NULL, 0, NULL, 0);
					}
				}
			}
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("Subscribe [0x%lX]"), res));
	return res;
}


HRESULT SODaCXmlGroupImpl::doSubscriptionCancel(IN SOCltHTTPConnection* httpCon, IN BYTE state)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res;
	SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
	SOCmnPointer<SOCltHTTP> httpClient = entryDaC->getHTTPClient();
	// stop polled refresh
	SOCmnSingleLock<SOCmnSync> callLock(m_httpCyclicCon->getCallLock());
	SOCmnPointer<SODaCXmlCall> call = (SODaCXmlCall*)m_httpCyclicCon->getHTTPCall();
	callLock.unlock();

	if (call.isNotNull())
	{
		if (call->getSOAPMethod() == SODACXMLCALL_SUBSCRIPTIONPOLLEDREFRESH)
		{
			call->resetData(TRUE);
			call->setHTTPStatusCode(SOCLTHTTP_CALL_CANCELED);
			m_httpCyclicCon->finishCall(call);
			httpClient->addCompleteCall(call);
		}
	}

	// send subscription cancel call
	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	httpCall->subscriptionCancel(this);
	pDaCGroup->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCGroup->afterInterfaceCall();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();

	DWORD reqTimeout = httpCall->getTimeout();

	if ((reqTimeout == INFINITE) || ((reqTimeout == SOCMN_TIME_40DAYS)))
	{
		if (server.isNotNull())
		{
			reqTimeout = server->getRequestTimeout();
		}
	}

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getSubscriptionCancelResponse(&response);
	}

	// close connection
	if (m_httpCyclicCon.isNotNull())
	{
		SOCmnPointer<SOCmnObject> socket(m_httpCyclicCon->getSocket());
		httpClient->setTimeOut(socket, reqTimeout);
		m_httpCyclicCon->disconnect();
		m_httpCyclicCon.release();
	}

	pDaCGroup->setObjectState(state);
	pDaCGroup->setItemsState(state);
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("SubscriptionCancel [0x%lX]"), res));
	return res;
}


HRESULT SODaCXmlGroupImpl::handleSubscriptionPolledRefreshResponse(IN SODaCXmlCall* httpCall)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCXmlResponse> response;
	HRESULT res;
	// for advise call
	HRESULT masterQuality = S_OK;
	HRESULT masterError = S_OK;
	DWORD itemCnt = 0;
	SODaCItem** ppItems = NULL;
	SOCmnVariant* pValues = NULL;
	WORD* pQualities = NULL;
	SOCmnDateTime* pTimestamps = NULL;
	HRESULT* pErrors = NULL;
	DWORD i;
	res = httpCall->getSubscriptionPolledRefreshResponse(&response);

	if (SUCCEEDED(res))
	{
		res = response->getFault();
	}

	if (SUCCEEDED(res))
	{
		SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
		SODaCXmlResponseData* pResData;
		SOCmnListPosition pos;
		SOCmnPointer<SODaCItem> item;
		BOOL initV;
		SOCmnVariant itemValue;
		HRESULT itemResult;
		WORD itemQuality;
		SOCmnDateTime itemTimestamp;
		itemCnt = xmlData.getCount();
		ppItems = (SODaCItem**) malloc(itemCnt * sizeof(SODaCItem*));
		pValues = new SOCmnVariant[itemCnt];
		pQualities = new WORD[itemCnt];
		pTimestamps = new SOCmnDateTime[itemCnt];
		pErrors = new HRESULT[itemCnt];
		itemCnt = 0;
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			pResData = xmlData.getNext(pos);
			//	TODO: POC/IOP -> this needs revision
			item = pDaCGroup->getItemByClientHandle(_tcstoul(pResData->m_clientItemHandle, NULL, 16));

			if (item.isNotNull())
			{
				item->addRef();
				ppItems[itemCnt] = (SODaCItem*)item;

				if (pResData->m_resultID.isEmpty())
				{
					itemResult = S_OK;
				}
				else
				{
					itemResult = httpCall->getErrorCodeFromXML(pResData->m_resultID);
				}

				if (SUCCEEDED(itemResult))
				{
					initV = itemValue.initFromXMLValue(&pResData->m_value);
					itemQuality = httpCall->getDCOMQuality(pResData->m_quality, pResData->m_qualityLimit, pResData->m_qualityVendor);
					itemTimestamp.setXMLDateTime(pResData->m_timestamp);

					if (initV)
					{
						item->setRead(itemResult, &itemValue, itemQuality, &itemTimestamp, 0);
						pValues[itemCnt].initFrom(itemValue);
						pQualities[itemCnt] = itemQuality;
						pTimestamps[itemCnt] = itemTimestamp;
						pErrors[itemCnt] = S_OK;
					}
				}

				if (FAILED(itemResult))
				{
					item->setRead(itemResult, NULL, 0, NULL, 0);
					pErrors[itemCnt] = itemResult;
					pQualities[itemCnt] = 0;
				}

				itemCnt++;
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, _T("SODaCXmlGroupImpl::handleSubscriptionPolledRefreshResponse"), (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, (LPCTSTR)pResData->m_clientItemHandle));
			}
			++i;
		}
	}
	else
	{
		masterError = res;
		masterQuality = S_FALSE;
	}

	if (pDaCGroup)
	{
		pDaCGroup->onReadAdvise(TRUE, 0, masterQuality, masterError, itemCnt, ppItems, pValues, pQualities, pTimestamps, pErrors);
	}

	if (ppItems)
	{
		for (i = 0; i < itemCnt; i++)
		{
			ppItems[i]->release();
		}

		free(ppItems);
	}

	if (pValues)
	{
		delete[] pValues;
	}

	if (pQualities)
	{
		delete[] pQualities;
	}

	if (pTimestamps)
	{
		delete[] pTimestamps;
	}

	if (pErrors)
	{
		delete[] pErrors;
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_ADVISE_C, pDaCGroup, (_T("SubscriptionPolledRefresh [0x%lX]"), res));
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
HRESULT SODaCXmlGroupImpl::doUpdateImpl(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
//	SODaCGroup *pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	if (!fromServer)
	{
		// set data on server
		if (whatParameters & (SODAC_GRPPAR_UPDATERATE | SODAC_GRPPAR_DEADBAND))
		{
			// reestablish subscription
			cancelSubscription(SOCltElement::disconnected);
			res = establishSubscription();
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
HRESULT SODaCXmlGroupImpl::syncReadImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	OUT OPTIONAL SOCmnVariant* pValuesI,
	OUT OPTIONAL WORD* pQualitiesI,
	OUT OPTIONAL SOCmnDateTime* pTimestampsI,
	OUT OPTIONAL HRESULT* pErrorsI)
{
	return doSyncRead(count, itemArray, maxAge, SODACITEM_TRANSACTIONID_SYNC, pValuesI, pQualitiesI, pTimestampsI, pErrorsI);
}

HRESULT SODaCXmlGroupImpl::doSyncRead(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	IN DWORD transactionId,
	OUT SOCmnVariant* pValuesI,
	OUT WORD* pQualitiesI,
	OUT SOCmnDateTime* pTimestampsI,
	OUT HRESULT* pErrorsI)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnString* pItemIDArray;
	SOCmnString* pItemPathArray;
	SOCmnVariant* pValues;
	WORD* pQualities;
	SOCmnDateTime* pTimestamps;
	HRESULT* pErrors;
	DWORD i;
	pItemIDArray = new SOCmnString[count];

	if (!pItemIDArray)
	{
		return E_OUTOFMEMORY;
	}

	for (i = 0; i < count; i++)
	{
		pItemIDArray[i] = itemArray[i]->getItemID();
	}

	pItemPathArray = new SOCmnString[count];

	if (!pItemPathArray)
	{
		delete[] pItemIDArray;
		return E_OUTOFMEMORY;
	}

	for (i = 0; i < count; i++)
	{
		pItemPathArray[i] = itemArray[i]->getItemPath();
	}

	if (pValuesI)
	{
		pValues = pValuesI;
	}
	else
	{
		pValues = new SOCmnVariant[count];
	}

	if (pQualitiesI)
	{
		pQualities = pQualitiesI;
	}
	else
	{
		pQualities = new WORD[count];
	}

	if (pTimestampsI)
	{
		pTimestamps = pTimestampsI;
	}
	else
	{
		pTimestamps = new SOCmnDateTime[count];
	}

	if (pErrorsI)
	{
		pErrors = pErrorsI;
	}
	else
	{
		pErrors = new HRESULT[count];
	}

	res = server->read(count, pItemIDArray, pItemPathArray, maxAge, pValues, pQualities, pTimestamps, pErrors);

	for (i = 0; i < count; i++)
	{
		if (SUCCEEDED(res))
		{
			itemArray[i]->setRead(pErrors[i], &pValues[i], pQualities[i], &pTimestamps[i], transactionId);

			if (SUCCEEDED(pErrors[i]))
				if (itemArray[i]->getNativeDatatype() == VT_EMPTY)
				{
					itemArray[i]->setNativeDatatype(pValues[i].vt);
				}
		}
		else
		{
			itemArray[i]->setRead(res, NULL, 0, NULL, transactionId);
		}
	}

	delete[] pItemIDArray;
	delete[] pItemPathArray;

	if (!pValuesI)
	{
		delete[] pValues;
	}

	if (!pQualitiesI)
	{
		delete[] pQualities;
	}

	if (!pTimestampsI)
	{
		delete[] pTimestamps;
	}

	if (!pErrorsI)
	{
		delete[] pErrors;
	}

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
HRESULT SODaCXmlGroupImpl::syncWriteImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrorsI)
{
	return doSyncWrite(count, itemArray, pValues, pQualities, pTimestamps, SODACITEM_TRANSACTIONID_SYNC, pErrorsI);
}

HRESULT SODaCXmlGroupImpl::doSyncWrite(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN SOCmnVariant* pValuesI,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	IN DWORD transactionId,
	OUT HRESULT* pErrorsI)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnString* pItemIDArray;
	SOCmnString* pItemPathArray;
	DWORD i;
	HRESULT* pErrors = NULL;
	SOCmnVariant* pValues = NULL;
	pItemIDArray = new SOCmnString[count];

	if (!pItemIDArray)
	{
		return E_OUTOFMEMORY;
	}

	for (i = 0; i < count; i++)
	{
		pItemIDArray[i] = itemArray[i]->getItemID();
	}

	pItemPathArray = new SOCmnString[count];

	if (!pItemPathArray)
	{
		delete[] pItemIDArray;
		return E_OUTOFMEMORY;
	}

	for (i = 0; i < count; i++)
	{
		pItemPathArray[i] = itemArray[i]->getItemPath();
	}

	if (pErrorsI)
	{
		pErrors = pErrorsI;
	}
	else
	{
		pErrors = new HRESULT[count];
	}

	if (pValuesI)
	{
		pValues = pValuesI;
	}
	else
	{
		pValues = new SOCmnVariant[count];

		for (i = 0; i < count; i++)
		{
			itemArray[i]->getWriteValue(pValues[i]);
		}
	}

	res = server->write(count, pItemIDArray, pItemPathArray, pValues, pQualities, pTimestamps, pErrors);

	for (i = 0; i < count; i++)
	{
		if (SUCCEEDED(res))
		{
			itemArray[i]->setWrite(pErrors[i], transactionId);
		}
		else
		{
			itemArray[i]->setWrite(res, transactionId);
		}
	}

	delete[] pItemIDArray;
	delete[] pItemPathArray;

	if (!pErrorsI)
	{
		delete[] pErrors;
	}

	if (!pValuesI)
	{
		delete[] pValues;
	}

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
HRESULT SODaCXmlGroupImpl::asyncReadImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD maxAge,
	IN OPTIONAL DWORD transactionID,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (cancelID)
	{
		cancelID = 0;
	}

	return doSyncRead(count, itemArray, maxAge, transactionID, NULL, NULL, NULL, pErrors);
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
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no write advise connection
//
HRESULT SODaCXmlGroupImpl::asyncWriteImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD transactionID,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (cancelID)
	{
		cancelID = 0;
	}

	return doSyncWrite(count, itemArray, pValues, pQualities, pTimestamps, transactionID, pErrors);
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
HRESULT SODaCXmlGroupImpl::asyncRefreshImpl(
	IN DWORD maxAge,            // data source
	IN OPTIONAL DWORD transactionID,    // transaction id (provided by client)
	OUT OPTIONAL DWORD* cancelID)       // cancel id (provided by server)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SODaCItem** itemArray;
	DWORD count;
	SOCmnList<SODaCItem> itemList((SOCmnList<SODaCItem>*)pDaCGroup->getLeafList());

	if (cancelID)
	{
		cancelID = 0;
	}

	if (pDaCGroup->getObjectState() != SOCltElement::started)
	{
		return S_OK;
	}

	count = itemList.getCount();

	if (count > 0)
	{
		SOCmnListPosition pos;
		DWORD i = 0;
		itemArray = (SODaCItem**)malloc(count * sizeof(SODaCItem*));
		pos = itemList.getStartPosition();

		while (pos)
		{
			itemArray[i] = itemList.getNext(pos);

			if (itemArray[i]->isStarted())
			{
				itemArray[i]->addRef();
				i++;
			}
		}

		count = i;
	}
	else
	{
		return S_OK;
	}

	itemList.autoLock();
	return doSyncRead(count, itemArray, maxAge, transactionID, NULL, NULL, NULL, NULL);
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
HRESULT SODaCXmlGroupImpl::asyncCancelImpl(
	IN DWORD /* cancelID */)    // cancel id (provided by server)
{
	return S_OK;
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
HRESULT SODaCXmlGroupImpl::updateItemsImpl(
	IN BOOL fromServer,                 // update data from server or to server
	IN SOCmnList<SODaCItem> *itemList,      // item list
	IN OPTIONAL DWORD whatParameters)   // update what item properties
{
	HRESULT res = S_OK;

	if (fromServer)
	{
		// read from server
		if (SOCLT_XML_CHECK_DEMO)
		{
			return E_FAIL;
		}

		SODaCGroup* pDaCGroup = getObjectDaCGroup();
		SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
		SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
		SOCmnPointer<SODaCXmlCall> httpCall;
		SOCmnStringList propNameList;
		SOCmnStringList itemIDList;
		SOCmnStringList itemPathList;
		SOCmnListPosition posI;
		SODaCItem* pItem;
		SODaCXmlRequestOptions reqOptions;
		propNameList.create();
		propNameList.add(_T("dataType"));
		propNameList.add(_T("accessRights"));

		if (whatParameters & SODAC_ITMPAR_EU)
		{
			propNameList.add(_T("euInfo"));
			propNameList.add(_T("highEU"));
			propNameList.add(_T("lowEU"));
			propNameList.add(_T("euType"));
		}

		itemIDList.create();
		itemPathList.create();
		posI = itemList->getStartPosition();

		while (posI)
		{
			pItem = itemList->getNext(posI);

			if (pItem->getTargetObjectState() != SOCltElement::disconnected)
			{
				itemIDList.add(pItem->getItemID());
				itemPathList.add(pItem->getItemPath());
			}
		}

		if (itemIDList.getCount() == 0)
		{
			return res;
		}

		SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
		httpCall->getProperties(NULL, NULL, &itemIDList, &itemPathList, TRUE, FALSE, &propNameList, server->fillRequestOptions(&reqOptions));
		pDaCGroup->beforeInterfaceCall();
		res = httpCon->invoke(httpCall, TRUE);
		pDaCGroup->afterInterfaceCall();

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SODaCXmlResponse> response;
			res = httpCall->getGetPropertiesResponse(&response);

			if (SUCCEEDED(res))
			{
				SOCmnStringList xmlOptions(response->getResponseOptions());
				SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
				SODaCXmlResponseData* pResData;
				SOCmnListPosition pos;
				double highEU, lowEU;
				SOCmnVariant euInfo;
				SOCmnVariant vHlp;
				pos = xmlData.getStartPosition();
				posI = itemList->getStartPosition();
				pItem = NULL;

				while (pos)
				{
					pResData = xmlData.getNext(pos);

					if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ELEMENT)
					{
						if (posI)
						{
							pItem = itemList->getNext(posI);
						}
						else
						{
							pItem = NULL;
							res = E_FAIL;
							break;
						}

						euInfo.clear();
						highEU = 0;
						lowEU = 0;
					}
					else if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_PROPERTY)
					{
						if (!pItem)
						{
							res = E_FAIL;
							break;
						}

						if (FAILED(httpCall->getErrorCodeFromXML(pResData->m_resultID)))
						{
							continue;
						}

						if (pResData->m_name == _T("dataType"))
						{
							VARTYPE dt;
							SOCmnString dtSZ(pResData->m_value.m_value);
							response->expandNamespace(dtSZ);
							SOCmnVariant::getVARTYPE(dtSZ, dt);
							pItem->setNativeDatatype(dt);
						}
						else if (pResData->m_name == _T("accessRights"))
						{
							DWORD accessRights = 0;

							if (pResData->m_value.m_value == _T("readWritable"))
							{
								accessRights = OPC_READABLE | OPC_WRITEABLE;
							}
							else if (pResData->m_value.m_value == _T("readable"))
							{
								accessRights = OPC_READABLE;
							}
							else if (pResData->m_value.m_value == _T("writable"))
							{
								accessRights = OPC_WRITEABLE;
							}

							pItem->setAccessRights(accessRights);
						}
						else if (pResData->m_name == _T("highEU"))
						{
							vHlp.clear();

							if (vHlp.initFromXMLValue(&pResData->m_value))
							{
								highEU = vHlp.dblVal;
							}
						}
						else if (pResData->m_name == _T("lowEU"))
						{
							vHlp.clear();

							if (vHlp.initFromXMLValue(&pResData->m_value))
							{
								lowEU = vHlp.dblVal;
							}
						}
						else if (pResData->m_name == _T("euInfo"))
						{
							euInfo.initFromXMLValue(&pResData->m_value);
						}
						else if (pResData->m_name == _T("euType"))
						{
							if (pResData->m_value.m_value == _T("analog"))
							{
								vHlp.clear();
								SAFEARRAYBOUND dim;
								LONG idx;
								dim.lLbound = 0;
								dim.cElements = 2;
								vHlp.vt = (VT_ARRAY | VT_R8);
								vHlp.parray = SOCmnVariant::safeArrayCreate(VT_R8, 1, &dim);
								idx = 0;
								SafeArrayPutElement(vHlp.parray, &idx, &lowEU);
								idx = 1;
								SafeArrayPutElement(vHlp.parray, &idx, &highEU);
								pItem->setEU(OPC_ANALOG, vHlp);
							}
							else if (pResData->m_value.m_value == _T("enumerated"))
							{
								pItem->setEU(OPC_ENUMERATED, euInfo);
							}
						}
					}
				}
			}
		}

		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("GetProperties [0x%lX]"), res));
	}
	else
	{
		// set data on server
		if (whatParameters & SODAC_ITMPAR_DEADBAND)
		{
			// reestablish subscription
			cancelSubscription(SOCltElement::disconnected);
			res = establishSubscription();
		}
	}

	return res;
} // updateItems

void SODaCXmlGroupImpl::setPolledRefreshReply(SOCltHTTPCall* pCall)
{
	m_polledRefreshReply.setWithAddRef(pCall);
}

SOCltHTTPCall* SODaCXmlGroupImpl::getPolledRefreshReply(void)
{
	m_polledRefreshReply.addRef();
	return m_polledRefreshReply;
}

HRESULT SODaCXmlGroupImpl::validateItems(IN SOCmnList<SODaCItem> *pItemList, OUT HRESULT* pErrorOut)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
	SOCmnList<SODaCItem> itemList(pItemList);
	SOCmnStringList itemIDArrayList;
	SOCmnStringList itemPathArrayList;
	SOCmnStringList propertyList;
	SOCmnListPosition pos;
	SOCmnListPosition posI;
	SODaCItem* pItem;
	SODaCXmlRequestOptions reqOptions;
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res = S_OK;
	HRESULT itemResult;
	SOCmnList<SODaCItem> readItemList;
	LONG idxEO;
	DWORD itemCount = itemList.getCount();

	if (pErrorOut)
	{
		memset(pErrorOut, S_OK, sizeof(HRESULT) * itemCount);
	}

	itemIDArrayList.create();
	itemPathArrayList.create();
	propertyList.create();
	posI = itemList.getStartPosition();

	while (posI)
	{
		pItem = itemList.getNext(posI);
		itemIDArrayList.add(pItem->getItemID());
		itemPathArrayList.add(pItem->getItemPath());
	}

	if (itemIDArrayList.getCount() == 0)
	{
		return res;
	}

	propertyList.add(_T("dataType"));
	propertyList.add(_T("accessRights"));
	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	httpCall->getProperties(NULL, NULL, &itemIDArrayList, &itemPathArrayList, TRUE, FALSE, &propertyList, server->fillRequestOptions(&reqOptions));
	pDaCGroup->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCGroup->afterInterfaceCall();

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getGetPropertiesResponse(&response);

		if (SUCCEEDED(res))
		{
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			pos = xmlData.getStartPosition();
			posI = itemList.getStartPosition();
			pItem = NULL;
			idxEO = -1;

			while (pos)
			{
				pResData = xmlData.getNext(pos);

				if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ELEMENT)
				{
					if (!posI)
					{
						res = E_FAIL;
						break;
					}

					pItem = itemList.getNext(posI);
					idxEO++;

					if ((pItem->getItemID() != pResData->m_itemName) ||
						(pItem->getItemPath() != pResData->m_itemPath))
					{
						res = E_FAIL;
						break;
					}

					if (pResData->m_resultID.isEmpty())
					{
						itemResult = S_OK;
					}
					else
					{
						itemResult = httpCall->getErrorCodeFromXML(pResData->m_resultID);

						if ((pErrorOut) && ((idxEO >= 0) && (idxEO < (LONG)itemCount)))
						{
							pErrorOut[idxEO] = itemResult;
						}   //  end if
					}   //  end if..else

					if (FAILED(itemResult))
					{
						if (!readItemList.isInit())
						{
							readItemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);
						}

						readItemList.add(pItem);
						pItem = NULL;
					}
				}

				if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_PROPERTY)
				{
					if (!pItem)
					{
						res = E_FAIL;
						break;
					}

					if (pResData->m_resultID.isEmpty())
					{
						itemResult = S_OK;
					}
					else
					{
						itemResult = httpCall->getErrorCodeFromXML(pResData->m_resultID);
					}

					if (SUCCEEDED(itemResult))
					{
						SOCmnVariant propValue;

						if (propValue.initFromXMLValue(&pResData->m_value))
						{
							if (pResData->m_name == _T("dataType"))
							{
								SOCmnString propDT;
								VARTYPE dt;
								propValue.getBSTR(propDT);

								if (response->includesNamespacePrefix(propDT))
								{
									// property name with namespace
									response->expandNamespace(propDT);
									response->removeOPCXMLDANamespace(propDT); // remove default OPC XML DA NS
									propValue.setBSTR(propDT);
								}

								if (SOCmnVariant::getVARTYPE(propDT, dt))
								{
									pItem->setNativeDatatype(dt);
								}
								else
								{
									pItem->setLastError(E_FAIL, SODAC_ERR_GRP_VALIDATE_ITEMS);

									if ((pErrorOut) && ((idxEO > 0) && (idxEO < (LONG)itemCount)))
									{
										pErrorOut[idxEO] = E_FAIL;
									}

									res = S_FALSE;
								}
							}
							else if (pResData->m_name == _T("accessRights"))
							{
								DWORD accessRights = OPC_READABLE | OPC_WRITEABLE;
								SOCmnString arSZ;
								propValue.getBSTR(arSZ);

								if (arSZ.find(_T("ead")) == -1)
								{
									accessRights &= ~OPC_READABLE;
								}

								if (arSZ.find(_T("rit")) == -1)
								{
									accessRights &= ~OPC_WRITEABLE;
								}

								pItem->setAccessRights(accessRights);
							}
						}
						else
						{
							pItem->setLastError(E_FAIL, SODAC_ERR_GRP_VALIDATE_ITEMS);

							if ((pErrorOut) && ((idxEO > 0) && (idxEO < (LONG)itemCount)))
							{
								pErrorOut[idxEO] = E_FAIL;
							}

							res = S_FALSE;
						}
					}
				}
			}
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("GetProperties [0x%lX]"), res));

	if (readItemList.isInit())
	{
		HRESULT readItemRes;
		SOCmnVariant* pValues;
		HRESULT* pErrors;
		DWORD count = readItemList.getCount();
		SOCmnListPosition posRI;
		DWORD idxRI = 0;
		pValues = new SOCmnVariant[count];
		pErrors = new HRESULT[count];
		readItemRes = pDaCGroup->syncRead(&readItemList, SODAC_MAXAGE_CACHE, pValues, NULL, NULL, pErrors);

		if (SUCCEEDED(readItemRes))
		{
			posRI = readItemList.getStartPosition();

			while (posRI)
			{
				pItem = readItemList.getNext(posRI);

				if (SUCCEEDED(pErrors[idxRI]))
				{
					pItem->setNativeDatatype(pValues[idxRI].vt);
					pItem->setAccessRights(OPC_READABLE);
				}
				else
				{
					if (pErrorOut)
					{
						SOCmnListPosition posIFind;
						SODaCItem* pItemFind;
						posIFind = itemList.getStartPosition();
						idxEO = 0;

						while (posI)
						{
							pItemFind = itemList.getNext(posIFind);

							if (pItem == pItemFind)
							{
								pErrorOut[idxEO] = pErrors[idxRI];
								break;
							}

							idxEO++;
						}
					}

					pItem->setLastError(pErrors[idxRI], SODAC_ERR_GRP_VALIDATE_ITEMS);
					res = S_FALSE;
				}

				idxRI++;
			}
		}
		else
		{
			res = S_FALSE;
		}

		delete[] pValues;
		delete[] pErrors;
		readItemList.destroy();
	}

	return res;
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//		returns of unadvise
//
HRESULT SODaCXmlGroupImpl::doDisconnectImpl(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnPointer<SOCltHTTPConnection> pHttpCon = (SOCltHTTPConnection*)server->getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> pHttpCall;
	SOCMN_ALLOCATE_OBJECT_C(pHttpCall, SODaCXmlCall(pHttpCon), (SODaCXmlCall*));
	HRESULT res = S_OK;
	//-- remove group --
	pDaCGroup->beforeInterfaceCall();
	res = pHttpCall->subscriptionCancel(this);
	pDaCGroup->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("SODaCXmlGroupImpl::Server::RemoveGroup [0x%X] "), (unsigned int)res));
	pDaCGroup->setLastError(res, SODAC_ERR_REMOVE_GROUP);
	pDaCGroup->setObjectState(SOCltElement::disconnected);
	return res;
}


//-----------------------------------------------------------------------------
// SODaCXmlTaskPolledRefresh                                                  |
//-----------------------------------------------------------------------------

SODaCXmlTaskPolledRefresh::SODaCXmlTaskPolledRefresh(
	IN SODaCGroup* pGroup, IN SODaCXmlGroupImpl* pXmlGroupImpl)
	: SOCltCyclicTask(pGroup, SOCMN_TIME_40DAYS)
{
	m_pXmlGroupImpl = pXmlGroupImpl;
	m_lastCall = ::GetTickCount();
}

SODaCXmlTaskPolledRefresh::~SODaCXmlTaskPolledRefresh(void)
{
	m_pXmlGroupImpl->setPolledRefreshReply(NULL);
}

DWORD SODaCXmlTaskPolledRefresh::getNextTimeToRun(void)
{
	SODaCGroup* group = (SODaCGroup*)(SOCltElement*)m_element;
	SOCmnPointer<SODaCXmlCall> polledRefresh = (SODaCXmlCall*)m_pXmlGroupImpl->getPolledRefreshReply();
	DWORD now = GetTickCount();

	if (group->getObjectState() != SOCltElement::started)
	{
		return now + SOCMN_TIME_1DAY;
	}

	if (polledRefresh.isNotNull())
	{
		return now;    // received polled refresh reply
	}

	if (group->getDataRetrieval() == SODaCGroup::drSyncRead)
	{
		return m_lastCall + group->getUpdateRate();
	}
	else
	{
		return now + SOCMN_TIME_1DAY;
	}
}

void SODaCXmlTaskPolledRefresh::execute(void)
{
	SODaCGroup* group = (SODaCGroup*)(SOCltElement*)m_element;
	SOCmnPointer<SOCltHTTPConnection> httpCon = m_pXmlGroupImpl->getCyclicHTTPConnection();
	SOCmnPointer<SODaCXmlCall> polledRefresh = (SODaCXmlCall*)m_pXmlGroupImpl->getPolledRefreshReply();
	HRESULT res = S_OK;
	m_pXmlGroupImpl->setPolledRefreshReply(NULL);

	if (group->getObjectState() != SOCltElement::started)
	{
		return;
	}

	if (SOCLT_XML_CHECK_DEMO)
	{
		return;
	}

	if (polledRefresh.isNotNull())
	{
		// received response
		res = m_pXmlGroupImpl->handleSubscriptionPolledRefreshResponse(polledRefresh);

		if (SUCCEEDED(res))
		{
			if (group->getDataRetrieval() == SODaCGroup::drPolledRefresh)
			{
				// restart the polled refresh which is blocking in the server
				if ((group->getObjectState() == SOCltElement::started) && (httpCon.isNotNull()))
				{
					polledRefresh->resetData(FALSE);
					polledRefresh->subscriptionPolledRefresh(m_pXmlGroupImpl, TRUE);
					m_lastCall = ::GetTickCount();
					res = httpCon->invoke(polledRefresh, FALSE);
				}
			}
		}

		polledRefresh.release();
	}
	else
	{
		// poll the values with a polled refesh and no wait
		if (group->getDataRetrieval() == SODaCGroup::drSyncRead)
		{
			if ((group->getObjectState() == SOCltElement::started) && (httpCon.isNotNull()))
			{
				SOCmnPointer<SODaCXmlCall> httpCall;
				SODaCXmlRequestOptions* pReqOpts;
				SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
				SOCMN_ALLOCATE_OBJECT_C(pReqOpts, SODaCXmlRequestOptions(), (SODaCXmlRequestOptions*));
				pReqOpts->m_returnItemTime = TRUE;
				httpCall->setRequestOptions(pReqOpts);
				httpCall->subscriptionPolledRefresh(m_pXmlGroupImpl, FALSE);
				m_lastCall = ::GetTickCount();
				res = httpCon->invoke(httpCall, FALSE);
				httpCall.release();
			}
		}
	}

	if (FAILED(res))
	{
		if (res != (LONG)(OPC_E_HTTP_STATUS_BASE | SOCLTHTTP_CALL_CANCELED))
		{
			group->setObjectState(SOCltElement::disconnected);
			group->setItemsState(SOCltElement::disconnected);
			group->performStateTransition(TRUE);

			if (group->getObjectState() != SOCltElement::started)
			{
				SOCmnPointer<SODaCServer> server = (SODaCServer*)group->getParentBranch();

				if (server.isNotNull())
				{
					server->setObjectState(SOCltElement::disconnected);
					server->performStateTransition(TRUE);
				}
			}
		}
	}
}



//-----------------------------------------------------------------------------
// SODaCXmlTaskGetStatus                                                      |
//-----------------------------------------------------------------------------

SOCltTaskXmlGetStatus::SOCltTaskXmlGetStatus(
	IN SOCltElement* pServer, IN const DWORD& cycle)
	: SOCltCyclicTask(pServer, cycle)
{
}

SOCltTaskXmlGetStatus::~SOCltTaskXmlGetStatus(void)
{
}

DWORD SOCltTaskXmlGetStatus::getNextTimeToRun(void)
{
	SODaCServer* pDaCServer = (SODaCServer*)(SOCltElement*)m_element;

	SOCmnList<SOCmnElement> branchList(pDaCServer->getBranchList());
	SOCmnListPosition pos = branchList;
	SODaCGroup* pGroup;

	while (pos)
	{
		pGroup = (SODaCGroup*)branchList.GetNext(pos);

		if (pGroup->getObjectState() == SOCltElement::started)
		{
			return m_lastCall + getCycle();
		}
	}

	return m_lastCall + SOCMN_TIME_1DAY;
}

void SOCltTaskXmlGetStatus::execute(void)
{
	SODaCServer* pDaCServer = (SODaCServer*)(SOCltElement*)m_element;
	SOCmnPointer<SOCltHTTPConnection> httpConnection = pDaCServer->getHTTPConnection();
	m_lastCall = ::GetTickCount();

	SOCmnList<SOCmnElement> branchList(pDaCServer->getBranchList());
	SOCmnListPosition pos = branchList;
	SODaCGroup* pGroup;

	while (pos)
	{
		pGroup = (SODaCGroup*)branchList.GetNext(pos);

		if (pGroup->getObjectState() == SOCltElement::started)
		{
			if (SOCLT_XML_CHECK_DEMO)
			{
				return;
			}

			// avoid keeping a lock on the server`s subscription list... only used above when checking group state (active/started)
			branchList.detach();
			SOCmnPointer<SODaCXmlCall> httpCall;
			HRESULT res;
			SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpConnection), (SODaCXmlCall*));
			SODaCXmlRequestOptions reqOptions;
			httpCall->getStatus(&reqOptions);
			pDaCServer->beforeInterfaceCall();
			res = httpConnection->invoke(httpCall, TRUE);
			pDaCServer->afterInterfaceCall();
			// only need to call GetStatus once per server, when finding first group which is active
			return;
		}
	}
}



//-----------------------------------------------------------------------------
// SODaCXmlGroupDefault                                                       |
//-----------------------------------------------------------------------------

SODaCXmlGroupDefault::SODaCXmlGroupDefault(void)
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
SOCmnObject* SODaCXmlGroupDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError

#endif // SOFEATURE_SOAP

#endif // SODAC

