#include "stdafx.h"

#ifdef SOAEC

#ifdef SOFEATURE_TUNNEL

#include "SOAeCTPCall.h"
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SOAeCTPCall                                                                |
//-----------------------------------------------------------------------------

SOAeCTPCall::SOAeCTPCall(IN SOCltTPConnection* pCon)
	: SOCmnTPCall(pCon)
{
}

SOAeCTPCall::~SOAeCTPCall(void)
{
}

HRESULT SOAeCTPCall::getStatus(
	OUT SOAeCServerStatus& status)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_GETSTATUS);       // function id
	request->MarshalChar(0);                                     // flags
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			ok &= response->UnMarshalLong(&status.m_state);
			ok &= response->UnMarshalDate(&status.m_startTime);
			ok &= response->UnMarshalDate(&status.m_currentTime);
			ok &= response->UnMarshalString(&status.m_vendorInfo);
			ok &= response->UnMarshalString(&status.m_productVersion);
			ok &= response->UnMarshalDWORDList(&status.m_supportedLCIDs);
			ok &= response->UnMarshalDate(&status.m_lastUpdateTime);
		}

		if (!ok)
		{
			status.clear();
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::addSubscription(
	IN SOAeCSubscription* pSubscription,
	OUT DWORD& subscriptionServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_ADDSUBSCRIPTION);        // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong((DWORD)pSubscription->getOpcHandle());
	request->MarshalChar(pSubscription->getTargetObjectState() == SOCltElement::started); // active
	request->MarshalLong(pSubscription->getBufferTime());
	request->MarshalLong(pSubscription->getMaxSize());
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD revBufferTime;
			DWORD revMaxSize;
			ok &= response->UnMarshalLong(&subscriptionServerHandle);
			ok &= response->UnMarshalLong(&revBufferTime);
			ok &= response->UnMarshalLong(&revMaxSize);

			if (ok)
			{
				pSubscription->m_dwRevisedBufferTime = revBufferTime;
				INVOKE_CALLBACK_ON_PTR(pSubscription, SOAeCSubscription, onSetRevisedBufferTime);
				pSubscription->m_dwRevisedMaxSize = revMaxSize;
				INVOKE_CALLBACK_ON_PTR(pSubscription, SOAeCSubscription, onSetRevisedMaxSize);
			}
			else
			{
				subscriptionServerHandle = 0;
				res = RPC_E_INVALID_DATA;
			}
		}
	}

	return res;
}


HRESULT SOAeCTPCall::removeSubscription(
	IN SOAeCSubscription* /*pSubscription */,
	IN DWORD subscriptionServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_REMOVESUBSCRIPTION);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(subscriptionServerHandle);
	request->MarshalChar(FALSE);                                 // forced
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::addAreaBrowser(
	OUT DWORD& areaBrowserServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_ADDAREABROWSER);  // function id
	request->MarshalChar(0);                                     // flags
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			ok &= response->UnMarshalLong(&areaBrowserServerHandle);
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::removeAreaBrowser(
	IN DWORD areaBrowserServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_REMOVEAREABROWSER);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(areaBrowserServerHandle);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::queryEventCategories(IN DWORD eventType, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYEVENTCATEGORIES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(eventType);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD categoryCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&categoryCount);

			for (i = 0; ((i < categoryCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalLong(&pOutData->m_id);
				ok &= response->UnMarshalString(&pOutData->m_description);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::queryEventAttributes(IN DWORD catID, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYEVENTATTRIBUTES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(catID);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD attrCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&attrCount);

			for (i = 0; ((i < attrCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalLong(&pOutData->m_id);
				ok &= response->UnMarshalString(&pOutData->m_description);
				ok &= response->UnMarshalShort(&pOutData->m_datatype);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::queryConditionNames(IN DWORD catID, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(catID);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD condCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&condCount);

			for (i = 0; ((i < condCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalString(&pOutData->m_description);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::querySubConditionNames(IN LPCTSTR condName, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYSUBCONDITIONNAMES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(condName);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD subCondCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&subCondCount);

			for (i = 0; ((i < subCondCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalString(&pOutData->m_description);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::querySourceConditions(IN LPCTSTR sourceQName, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYSOURCECONDITIONS);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(sourceQName);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD condCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&condCount);

			for (i = 0; ((i < condCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalString(&pOutData->m_description);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::ackCondition(IN SOCmnList<SOAeCEvent> *pEventList, IN LPCTSTR ackID, IN LPCTSTR ackComment)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_AE_SERVER_ACKCONDITION);    // function id
	request->MarshalChar(0);                                     // flags
	SOCmnList<SOAeCEvent> eventList(pEventList);
	DWORD eventCount = eventList.getCount();
	SOAeCEvent* pEvent;
	SOCmnListPosition pos;
	request->MarshalString(ackID);
	request->MarshalString(ackComment);
	request->MarshalLong(eventCount);
	pos = eventList.getStartPosition();

	while (pos)
	{
		pEvent = eventList.getNext(pos);
		request->MarshalString(pEvent->m_source);
		request->MarshalString(pEvent->m_conditionName);
		request->MarshalDate(SOCmnDateTime(&pEvent->m_ftActiveTime));
		request->MarshalLong(pEvent->m_dwCookie);
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT ackRes;
			DWORD i;
			pos = eventList.getStartPosition();

			for (i = 0; ((ok) && (pos) && (i < eventCount)); i++)
			{
				pEvent = eventList.getNext(pos);
				ok = response->UnMarshalLong(&ackRes);

				if (ok)
				{
					pEvent->m_ackRes = ackRes;
					INVOKE_CALLBACK_ON_PTR(pEvent, SOAeCEvent, onEventAcknowledged)
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::changeBrowsePosition(IN DWORD areaBrowserHandle, IN DWORD browseDirection, IN LPCTSTR name)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(areaBrowserHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_CHANGEBROWSEPOSITION);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(browseDirection);
	request->MarshalString(name);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::browseOPCAreas(IN DWORD areaBrowserHandle, IN DWORD typeFilter, IN LPCTSTR elementNameFilter, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(areaBrowserHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_BROWSEOPCAREAS);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(typeFilter);
	request->MarshalString(elementNameFilter);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD elementCount;
			SOAeCTPCallData* pOutData;
			DWORD i;

			if (!pCallOutDataList->isInit())
			{
				pCallOutDataList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			pCallOutDataList->removeAll();
			ok &= response->UnMarshalLong(&elementCount);

			for (i = 0; ((i < elementCount) && (ok)); i++)
			{
				pOutData = new SOAeCTPCallData();
				ok &= response->UnMarshalString(&pOutData->m_description);

				if (ok)
				{
					pCallOutDataList->add(pOutData);
				}
				else
				{
					delete pOutData;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::getQualifiedSourceName(IN DWORD areaBrowserHandle, IN LPCTSTR name, OUT SOCmnString& qName)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(areaBrowserHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDSOURCENAME);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(name);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (SUCCEEDED(res) && (ok))
		{
			ok = response->UnMarshalString(&qName);
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::getQualifiedAreaName(IN DWORD areaBrowserHandle, IN LPCTSTR name, OUT SOCmnString& qName)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(areaBrowserHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDAREANAME);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(name);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (SUCCEEDED(res) && (ok))
		{
			ok = response->UnMarshalString(&qName);
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

// subscription object

HRESULT SOAeCTPCall::getReturnedAttributes(IN DWORD subscriptionServerHandle, IN DWORD category, IN OUT SOCmnDWORDList* pListRAC)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(subscriptionServerHandle);              // object id
	request->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_GETRETURNEDATTRIBUTES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(category);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		SOCmnDWORDList attrList(pListRAC);
		DWORD attrCount;
		DWORD attrID;
		DWORD i;
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (ok)
		{
			ok = response->UnMarshalLong(&attrCount);

			for (i = 0; ((i < attrCount) && (ok)); i++)
			{
				ok = response->UnMarshalLong(&attrID);

				if (ok)
				{
					attrList.add(attrID);
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::selectReturnedAttributes(IN DWORD subscriptionServerHandle, IN DWORD category, IN SOCmnDWORDList* pListRAC)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(subscriptionServerHandle);              // object id
	request->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_SELECTRETURNEDATTRIBUTES);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(category);
	SOCmnListPosition pos;
	SOCmnDWORDList attrList(pListRAC);
	DWORD attrID;
	DWORD attrCount;
	attrCount = attrList.getCount();
	request->MarshalLong(attrCount);
	pos = attrList.getStartPosition();

	while (pos)
	{
		attrID = attrList.getNext(pos);
		request->MarshalLong(attrID);
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SOAeCTPCall::refresh(IN DWORD subscriptionServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(subscriptionServerHandle);              // object id
	request->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_REFRESH);     // function id
	request->MarshalChar(0);                                     // flags
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SOAeCTPCall::cancelRefresh(IN DWORD subscriptionServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(subscriptionServerHandle);              // object id
	request->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);         // interface id
	request->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_CANCELREFRESH);     // function id
	request->MarshalChar(0);                                     // flags
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


//-----------------------------------------------------------------------------
// SOAeCTPReceiver                                                            |
//-----------------------------------------------------------------------------

SOAeCTPReceiver::SOAeCTPReceiver(void)
{
}

void SOAeCTPReceiver::closeConnection(IN SOCmnTPConnection* pCon)
{
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)((SOCltTPConnection*)pCon)->getServer();

	if (server.isNotNull())
	{
		server->connectionFailure();
	}
}

LONG SOAeCTPReceiver::handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	if (interfaceId != TP_INTERFACE_AE_CALLBACK)
	{
		return E_FAIL;
	}

	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)tpCon->getServer();

	if (!server)
	{
		return E_FAIL;
	}

	switch (functionId)
	{
	case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
		tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
		break;

	case TP_FUNCTION_AE_CALLBACK_ONEVENT:
		onEvent(server, pCall);
		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}


void SOAeCTPReceiver::onEvent(IN SOAeCServer* pServer, IN SOCmnTPCall* pCall)
{
	SOCmnPointer<SOCmnTP_UnMarshaler> notification = pCall->getRcvMsg();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SOAeCSubscription> subscription;
	DWORD subscriptionClientHandle;
	// unmarshal input
	subscriptionClientHandle = pCall->getObjectId();
	subscription = pServer->getSubscriptionByClientHandle(subscriptionClientHandle);

	if (!subscription)
	{
		tpCon->sendReply(pCall, TP_INTERFACE_AE_CALLBACK, TP_FUNCTION_AE_CALLBACK_ONEVENT, E_INVALIDARG);
		return;
	}

	SOCmnPointer<SOAeCEntry> entry = getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entry->getCreator();
	BYTE refresh, lastRefresh;
	DWORD i, j;
	BOOL ok;
	DWORD eventCount;
	SOCmnDateTime timeConvHlp;
	BYTE byteHlp;
	DWORD attrCount;
	SOAeCEvent** pEventArray = NULL;
	notification->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = notification->UnMarshalChar(&refresh);
	ok &= notification->UnMarshalChar(&lastRefresh);
	ok &= notification->UnMarshalLong(&eventCount);                              // event count

	if (ok)
	{
		pEventArray = (SOAeCEvent**)malloc(eventCount * sizeof(SOAeCEvent*));
		memset(pEventArray, 0, eventCount * sizeof(SOAeCEvent*));
	}

	for (i = 0; ((ok) && (i < eventCount)); i++)
	{
		pEventArray[i] = creator->createEvent(subscription, NULL);
		ok &= notification->UnMarshalShort(&pEventArray[i]->m_changeMask);
		ok &= notification->UnMarshalShort(&pEventArray[i]->m_newState);
		ok &= notification->UnMarshalString(&pEventArray[i]->m_source);
		ok &= notification->UnMarshalDate(&timeConvHlp);
		timeConvHlp.get(&pEventArray[i]->m_ftTime);
		ok &= notification->UnMarshalString(&pEventArray[i]->m_message);
		ok &= notification->UnMarshalLong(&pEventArray[i]->m_eventType);
		ok &= notification->UnMarshalLong(&pEventArray[i]->m_eventCategory);
		ok &= notification->UnMarshalLong(&pEventArray[i]->m_severity);
		ok &= notification->UnMarshalString(&pEventArray[i]->m_conditionName);
		ok &= notification->UnMarshalString(&pEventArray[i]->m_subconditionName);
		ok &= notification->UnMarshalShort(&pEventArray[i]->m_quality);
		ok &= notification->UnMarshalChar(&byteHlp);
		pEventArray[i]->m_bAckRequired = byteHlp;
		ok &= notification->UnMarshalDate(&timeConvHlp);
		timeConvHlp.get(&pEventArray[i]->m_ftActiveTime);
		ok &= notification->UnMarshalLong(&pEventArray[i]->m_dwCookie);
		ok &= notification->UnMarshalString(&pEventArray[i]->m_actorID);
		ok &= notification->UnMarshalLong(&attrCount);

		if ((ok) && (attrCount != 0))
		{
			SOCmnList<SOCmnVariant> attrList(pEventArray[i]->m_listEventAttrs);
			SOCmnVariant attrVariant;
			attrList.allocateBlock(attrCount);

			for (j = 0; ((ok) && (j < attrCount)); j++)
			{
				ok &= notification->UnMarshalVariant(&attrVariant);
				attrList.add(&attrVariant);
				attrVariant.clear();
			}
		}

		if (ok)
		{
			pEventArray[i]->report();
		}
	}

	if (ok)
	{
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pServer, (_T("TPAE::Server::OnEvent %lu"), eventCount));
		subscription->onReceivedEvents(eventCount, pEventArray, refresh, lastRefresh);
	}
	else
	{
		_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pServer, (_T("TP Protocol unmarshaling error: TPAE::Server::OnEvent")));
	}

	if (pEventArray)
	{
		for (i = 0; i < eventCount; i++)
		{
			if (pEventArray[i])
			{
				pEventArray[i]->release();
			}
		}

		free(pEventArray);
	}

	tpCon->sendReply(pCall, TP_INTERFACE_AE_CALLBACK, TP_FUNCTION_AE_CALLBACK_ONEVENT, ok ? S_OK : RPC_E_INVALID_DATA);
	return;
}

#endif // SOFEATURE_TUNNEL

#endif // SOAEC
