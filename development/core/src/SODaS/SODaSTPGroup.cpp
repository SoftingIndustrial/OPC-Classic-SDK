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
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSTPGroup.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : TP server related classes                                   |
//                - SOSrvTP: TP server object                                 |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_TUNNEL

#include "SODaSTPServer.h"
#include "SODaSTPGroup.h"
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SODaSTPGroup                                                            |
//-----------------------------------------------------------------------------

SODaSTPGroup::SODaSTPGroup(void)
{
}

SODaSTPGroup::~SODaSTPGroup(void)
{
}

//-----------------------------------------------------------------------------
// handleTPRequest
//
// - processing of TP request
//
// returns:
//		TRUE  - receiver has processed or is processing the request
//		FALSE - unknown request for this receiver
//
LONG SODaSTPGroup::handleRequest(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId)
{
	if (interfaceId != TP_INTERFACE_DA_GROUP)
	{
		return E_FAIL;
	}

	SOSRV_TP_CHECK_DEMO
	SOCmnPointer<SOSrvTPConnection> tpCon = (SOSrvTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SODaSServer> server = (SODaSServer*)tpCon->getServer();

	if (!server)
	{
		return E_FAIL;
	}

	switch (functionId)
	{
	case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
		tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
		break;

	case TP_FUNCTION_DA_GROUP_ADDITEMS:
		addItems(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_REMOVEITEMS:
		removeItems(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_SYNCREAD:
		syncRead(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_SYNCWRITE:
		syncWrite(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_ASYNCREAD:
		asyncRead(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_ASYNCWRITE:
		asyncWrite(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_VALIDATEITEMS:
		validateItems(server, pCall);
		break;

	case TP_FUNCTION_DA_GROUP_REFRESH:
		refresh(server, pCall);
		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}


void SODaSTPGroup::addItems(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_ADDITEMS, TPDA::Group::AddItems)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	DWORD itemClientHandle;
	SOCmnString itemId;
	BYTE active;
	WORD reqDatatype;
	SOCmnString accessPath;
	BYTE valDeadband;
	FLOAT deadband;
	WORD nativeDatatype;
	DWORD accessRights;
	SOCmnPointer<SODaSItem> item;
	SOCmnPointer<SODaSItemTag> itemTag;
	SOCmnVariant euInfo;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&itemCnt);
	// allocate block for entries in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(group->getLeafList());
		DWORD freeListEntries;
		// get number of available allocated list entries
		freeListEntries = itemList.getAvailableAllocatedEntries();

		if (freeListEntries < itemCnt)
		{
			itemList.allocateBlock(itemCnt - freeListEntries);
		}

		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> ctac = group->getCyclicTransaction();

		if (!!ctac)
		{
			if (ctac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(ctac->getRequestList());
				freeListEntries = rList.getAvailableAllocatedEntries();

				if (freeListEntries < itemCnt)
				{
					rList.allocateBlock(itemCnt - freeListEntries);
				}
			}
		}
	}
	// generate initial value read
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnList<SODaSRequest> reqList;

	if (group->getActive())
	{
		// create a asynchronous read transaction with the connection id of the cyclic notification transaction
		tac = creator->createTransaction(group, SODaSTransaction::refresh, SODaSTransaction::allConnections, 0);
		reqList = tac->getRequestList();
	}

	// call method to prepare for add items for dynamic namespace
	DWORD preValidateItemsCookie = 0;
	SOCmnList<SODaSItemData> itemDataList;

	if (group->hasPrePostValidateItems())
	{
		DWORD curReadPos = request->GetReadPos();
		SODaSItemData* pItemData;
		itemDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

		for (DWORD i = 0; i < itemCnt; i++)
		{
			SOCMN_ALLOCATE_OBJECT(pItemData, SODaSItemData);
			ok &= request->UnMarshalLong(&itemClientHandle);
			ok &= request->UnMarshalString(&itemId);
			ok &= request->UnMarshalChar(&active);
			ok &= request->UnMarshalShort(&reqDatatype);
			ok &= request->UnMarshalString(&accessPath);
			ok &= request->UnMarshalChar(&valDeadband);
			ok &= request->UnMarshalFloat(&deadband);

			if (ok)
			{
				pItemData->m_itemID = itemId;
				pItemData->m_reqDatatype = reqDatatype;
				pItemData->m_itemPath = accessPath;
				itemDataList.add(pItemData);
			}
			else
			{
				// !ok
				SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
			}
		}

		preValidateItemsCookie = group->preValidateItems(TRUE, &itemDataList);
		request->SetReadPos(curReadPos); // reset read position
	}

	// build response
	HRESULT itemRes;
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_ADDITEMS);         // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&itemClientHandle);
		ok &= request->UnMarshalString(&itemId);
		ok &= request->UnMarshalChar(&active);
		ok &= request->UnMarshalShort(&reqDatatype);
		ok &= request->UnMarshalString(&accessPath);
		ok &= request->UnMarshalChar(&valDeadband);
		ok &= request->UnMarshalFloat(&deadband);

		if (ok)
		{
			itemRes = group->addItem(itemId, accessPath, (active != 0), itemClientHandle, reqDatatype, &item, &nativeDatatype, &accessRights);
			response->MarshalLong(itemRes);

			if (SUCCEEDED(itemRes))
			{
				itemTag = item->getItemTag();
				response->MarshalLong(item->getServerHandle());
				response->MarshalShort(nativeDatatype);
				response->MarshalLong(accessRights);
				response->MarshalLong(itemTag->getEUType());
				itemTag->getEUInfo(euInfo);
				response->MarshalVariant(euInfo);

				if (!valDeadband)
				{
					response->MarshalFloat(-1.0);
				}
				else
				{
					if (SUCCEEDED(item->getDeadBand(deadband)))
					{
						response->MarshalFloat(deadband);
					}
					else
					{
						response->MarshalFloat(-1.0);
					}
				}

				if ((group->getActive()) && (active != 0) && (accessRights & OPC_READABLE))
				{
					// active item
					// create cache request to send the current value to the client
					SOCmnPointer<SODaSRequest> req;
					req = creator->createRequest(server, item, itemTag, SODaSRequest::read, SODaSRequest::asyncInitNotify);

					if (req.isNotNull())
					{
						req->setItem(item);
						req->setCompletedEvent(server->getWorkEvent());
						req->setIsRefresh();
						reqList.add(req);
					}
				}

				item.release();
			}
			else
			{
				res = S_FALSE;
			}
		}
	}

	if (!ok)
	{
		// !ok
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);

	if (preValidateItemsCookie != 0)
	{
		group->postValidateItems(preValidateItemsCookie, &itemDataList);
	}

	itemDataList.destroy();
	group->onAddItems();

	if (group->getActive())
	{
		if (!reqList.isEmpty())
		{
			// start the initial read transaction
			server->addTransaction(tac);
			tac->start(server);
		}
	}

	// free unused blocks in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(group->getLeafList());
		itemList.freeUnusedBlocks();
		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> ctac = group->getCyclicTransaction();

		if (!!ctac)
		{
			if (ctac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(ctac->getRequestList());
				rList.freeUnusedBlocks();
			}
		}
	}

	SOSRV_END_TP_FUNCTION
} // addItems


void SODaSTPGroup::removeItems(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_REMOVEITEMS, TPDA::Group::RemoveItems)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	DWORD itemServerHandle;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&itemCnt);
	// build response
	HRESULT itemRes;
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_REMOVEITEMS);      // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&itemServerHandle);
		itemRes = group->removeItem(itemServerHandle);
		response->MarshalLong(itemRes);

		if (FAILED(itemRes))
		{
			res = S_FALSE;
		}
	}

	if (!ok)
	{
		// !ok
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	group->onRemoveItems();

	if (group->getUpdateRate() == 0)
	{
		// recalc notify all changes
		group->calcNotifyAllChanges();
	}

	// free unused blocks in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(group->getLeafList());
		itemList.freeUnusedBlocks();
		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> tac = group->getCyclicTransaction();

		if (!!tac)
		{
			if (tac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(tac->getRequestList());
				rList.freeUnusedBlocks();
			}
		}
	}
	SOSRV_END_TP_FUNCTION
} // removeItems


void SODaSTPGroup::validateItems(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_VALIDATEITEMS, TPDA::Group::ValidateItems)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	SOCmnString itemId;
	WORD reqDatatype;
	SOCmnString accessPath;
	WORD nativeDatatype;
	DWORD accessRights;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&itemCnt);
	// call method to prepare for add items for dynamic namespace
	DWORD preValidateItemsCookie = 0;
	SOCmnList<SODaSItemData> itemDataList;

	if (group->hasPrePostValidateItems())
	{
		DWORD curReadPos = request->GetReadPos();
		SODaSItemData* pItemData;
		itemDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

		for (DWORD i = 0; i < itemCnt; i++)
		{
			SOCMN_ALLOCATE_OBJECT(pItemData, SODaSItemData);
			ok &= request->UnMarshalString(&itemId);
			ok &= request->UnMarshalShort(&reqDatatype);
			ok &= request->UnMarshalString(&accessPath);

			if (ok)
			{
				pItemData->m_itemID = itemId;
				pItemData->m_reqDatatype = reqDatatype;
				pItemData->m_itemPath = accessPath;
				itemDataList.add(pItemData);
			}
			else
			{
				// !ok
				SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
			}
		}

		preValidateItemsCookie = group->preValidateItems(FALSE, &itemDataList);
		request->SetReadPos(curReadPos); // reset read position
	}

	// build response
	HRESULT itemRes;
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_VALIDATEITEMS);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalString(&itemId);
		ok &= request->UnMarshalShort(&reqDatatype);
		ok &= request->UnMarshalString(&accessPath);

		if (ok)
		{
			itemRes = group->validateItem(itemId, accessPath, reqDatatype, &nativeDatatype, &accessRights);
			response->MarshalLong(itemRes);

			if (SUCCEEDED(itemRes))
			{
				response->MarshalShort(nativeDatatype);
				response->MarshalLong(accessRights);
			}
			else
			{
				res = S_FALSE;
			}
		}
	}

	if (!ok)
	{
		// !ok
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	if (preValidateItemsCookie != 0)
	{
		group->postValidateItems(preValidateItemsCookie, &itemDataList);
	}

	itemDataList.destroy();
	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
} // validateItems



void SODaSTPGroup::syncWrite(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_SYNCWRITE, TPDA::Group::syncWrite)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&itemCnt);
	// build response
	HRESULT* pErrors;
	SOCmnVariant* pValues;
	WORD* pQualities;
	BOOL* pQualitySpecified;
	SOCmnDateTime* pTimeStamps;
	BOOL* pTimeStampSpecified;
	DWORD* pItemServerHandles;
	BYTE qualitySpecified;
	BYTE timestampSpecified;
	pErrors = new HRESULT[itemCnt];
	pValues = new SOCmnVariant[itemCnt];
	pQualities = new WORD[itemCnt];
	pQualitySpecified = new BOOL[itemCnt];
	pTimeStamps = new SOCmnDateTime[itemCnt];
	pTimeStampSpecified = new BOOL[itemCnt];
	pItemServerHandles = new DWORD[itemCnt];

	if (!pErrors || !pValues || !pQualities || !pQualitySpecified ||
		!pTimeStamps || !pTimeStampSpecified || !pItemServerHandles)
	{
		delete[] pItemServerHandles;
		delete[] pValues;
		delete[] pQualities;
		delete[] pQualitySpecified;
		delete[] pTimeStamps;
		delete[] pTimeStampSpecified;
		delete[] pErrors;
		SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
	}

	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_SYNCWRITE);        // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&pItemServerHandles[i]);
		ok &= request->UnMarshalVariant(&pValues[i]);
		ok &= request->UnMarshalChar(&qualitySpecified);

		if (ok)
		{
			pQualitySpecified[i] = qualitySpecified;

			if (pQualitySpecified[i])
			{
				ok &= request->UnMarshalShort(&pQualities[i]);
			}
		}

		ok &= request->UnMarshalChar(&timestampSpecified);

		if (ok)
		{
			pTimeStampSpecified[i] = timestampSpecified;

			if (pTimeStampSpecified[i])
			{
				ok &= request->UnMarshalDate(&pTimeStamps[i]);
			}
		}
	}

	if (ok)
	{
		SOCmnPointer<SODaSTransaction> tac;
		SOCmnEvents events;
		DWORD reqEventId = events.create();
		res = group->createTransaction(FALSE, SODaSTransaction::synchronous, 0, OPC_DS_DEVICE, SOCmnEventPointer(&events, reqEventId), itemCnt, pItemServerHandles, NULL, pValues, pQualities, pQualitySpecified, pTimeStamps, pTimeStampSpecified, (SODaSTransaction**)tac, NULL, pErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// -> start sync write transaction
			tac->execute(server, &events, reqEventId, server->m_dueTimeWrite);
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();

		for (i = 0; i < itemCnt; i++)
		{
			response->MarshalLong(pErrors[i]);
		}
	}
	else
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	delete[] pItemServerHandles;
	delete[] pValues;
	delete[] pQualities;
	delete[] pQualitySpecified;
	delete[] pTimeStamps;
	delete[] pTimeStampSpecified;
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // syncWrite




void SODaSTPGroup::syncRead(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_SYNCREAD, TPDA::Group::syncRead)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&itemCnt);
	// build response
	HRESULT* pErrors;
	SOCmnVariant* pValues;
	WORD* pQualities;
	SOCmnDateTime* pTimeStamps;
	DWORD* pItemServerHandles;
	DWORD* pMaxAge;
	pErrors = new HRESULT[itemCnt];
	pMaxAge = new DWORD[itemCnt];
	pValues = new SOCmnVariant[itemCnt];
	pQualities = new WORD[itemCnt];
	pTimeStamps = new SOCmnDateTime[itemCnt];
	pItemServerHandles = new DWORD[itemCnt];

	if (!pErrors || !pMaxAge || !pValues ||
		!pQualities || !pTimeStamps || !pItemServerHandles)
	{
		delete[] pItemServerHandles;
		delete[] pMaxAge;
		delete[] pValues;
		delete[] pQualities;
		delete[] pTimeStamps;
		delete[] pErrors;
		SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
	}

	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_SYNCREAD);         // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&pItemServerHandles[i]);
		ok &= request->UnMarshalLong(&pMaxAge[i]);
	}

	if (ok)
	{
		SOCmnPointer<SODaSTransaction> tac;
		SOCmnEvents events;
		DWORD reqEventId = events.create();
		res = group->createTransaction(TRUE, SODaSTransaction::synchronous, 0, 0, SOCmnEventPointer(&events, reqEventId), itemCnt, pItemServerHandles, pMaxAge, pValues, pQualities, NULL, pTimeStamps, NULL, (SODaSTransaction**)tac, NULL, pErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// -> start sync read transaction
			tac->execute(server, &events, reqEventId, server->m_dueTimeRead);
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();

		for (i = 0; i < itemCnt; i++)
		{
			response->MarshalLong(pErrors[i]);

			if (SUCCEEDED(pErrors[i]))
			{
				response->MarshalVariant(pValues[i]);
				response->MarshalShort(pQualities[i]);
				response->MarshalDate(pTimeStamps[i]);
			}
		}
	}
	else
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	delete[] pItemServerHandles;
	delete[] pMaxAge;
	delete[] pValues;
	delete[] pQualities;
	delete[] pTimeStamps;
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // syncRead




void SODaSTPGroup::asyncWrite(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_ASYNCWRITE, TPDA::Group::AsyncWrite)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	DWORD transactionID;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&transactionID);
	ok &= request->UnMarshalArrayLen(&itemCnt);
	// build response
	HRESULT* pErrors;
	SOCmnVariant* pValues;
	WORD* pQualities;
	BOOL* pQualitySpecified;
	SOCmnDateTime* pTimeStamps;
	BOOL* pTimeStampSpecified;
	DWORD* pItemServerHandles;
	BYTE qualitySpecified;
	BYTE timestampSpecified;
	pErrors = new HRESULT[itemCnt];
	pValues = new SOCmnVariant[itemCnt];
	pQualities = new WORD[itemCnt];
	pQualitySpecified = new BOOL[itemCnt];
	pTimeStamps = new SOCmnDateTime[itemCnt];
	pTimeStampSpecified = new BOOL[itemCnt];
	pItemServerHandles = new DWORD[itemCnt];

	if (!pErrors || !pValues || !pQualities || !pQualitySpecified ||
		!pTimeStamps || !pTimeStampSpecified || !pItemServerHandles)
	{
		delete[] pItemServerHandles;
		delete[] pValues;
		delete[] pQualities;
		delete[] pQualitySpecified;
		delete[] pTimeStamps;
		delete[] pTimeStampSpecified;
		delete[] pErrors;
		SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
	}

	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_ASYNCWRITE);       // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&pItemServerHandles[i]);
		ok &= request->UnMarshalVariant(&pValues[i]);
		ok &= request->UnMarshalChar(&qualitySpecified);

		if (ok)
		{
			pQualitySpecified[i] = qualitySpecified;

			if (pQualitySpecified[i])
			{
				ok &= request->UnMarshalShort(&pQualities[i]);
			}
		}

		ok &= request->UnMarshalChar(&timestampSpecified);

		if (ok)
		{
			pTimeStampSpecified[i] = timestampSpecified;

			if (pTimeStampSpecified[i])
			{
				ok &= request->UnMarshalDate(&pTimeStamps[i]);
			}
		}
	}

	if (ok)
	{
		SOCmnPointer<SODaSTransaction> tac;
		DWORD cancelID;
		res = group->createTransaction(FALSE, SODaSTransaction::tunnel, 0, OPC_DS_DEVICE, server->getWorkEvent(), itemCnt, pItemServerHandles, NULL, pValues, pQualities, pQualitySpecified, pTimeStamps, pTimeStampSpecified, (SODaSTransaction**)tac, &cancelID, pErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(transactionID);
			// -> start async write transaction
			tac->start(server);
			server->addTransaction(tac);
		}

		response->MarshalLong(cancelID);

		for (i = 0; i < itemCnt; i++)
		{
			response->MarshalLong(pErrors[i]);
		}
	}
	else
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	delete[] pItemServerHandles;
	delete[] pValues;
	delete[] pQualities;
	delete[] pQualitySpecified;
	delete[] pTimeStamps;
	delete[] pTimeStampSpecified;
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // asyncWrite




void SODaSTPGroup::asyncRead(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_ASYNCREAD, TPDA::Group::AsyncRead)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD i;
	DWORD groupServerHandle;
	DWORD itemCnt;
	DWORD transactionID;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&transactionID);
	ok &= request->UnMarshalArrayLen(&itemCnt);
	// build response
	HRESULT* pErrors;
	DWORD* pItemServerHandles;
	DWORD* pMaxAge;
	pErrors = new HRESULT[itemCnt];
	pMaxAge = new DWORD[itemCnt];
	pItemServerHandles = new DWORD[itemCnt];

	if (!pErrors || !pMaxAge || !pItemServerHandles)
	{
		delete[] pItemServerHandles;
		delete[] pMaxAge;
		delete[] pErrors;
		SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
	}

	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_ASYNCREAD);        // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	for (i = 0; ((i < itemCnt) && (ok)); i++)
	{
		ok &= request->UnMarshalLong(&pItemServerHandles[i]);
		ok &= request->UnMarshalLong(&pMaxAge[i]);
	}

	if (ok)
	{
		SOCmnPointer<SODaSTransaction> tac;
		DWORD cancelID;
		res = group->createTransaction(TRUE, SODaSTransaction::tunnel, 0, 0, server->getWorkEvent(), itemCnt, pItemServerHandles, pMaxAge, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, &cancelID, pErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(transactionID);
			// -> start async read transaction
			tac->start(server);
			server->addTransaction(tac);
		}

		response->MarshalLong(cancelID);

		for (i = 0; i < itemCnt; i++)
		{
			response->MarshalLong(pErrors[i]);
		}
	}
	else
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	delete[] pItemServerHandles;
	delete[] pMaxAge;
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // asyncRead




void SODaSTPGroup::refresh(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_GROUP, TP_FUNCTION_DA_GROUP_REFRESH, TPDA::Group::Refresh)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	SOCmnPointer<SODaSGroup> group;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD groupServerHandle;
	DWORD maxAge;
	DWORD transactionID;
	// unmarshal input
	groupServerHandle = pCall->getObjectId();
	group = server->getGroupByHandle(groupServerHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&transactionID);
	ok &= request->UnMarshalLong(&maxAge);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	response->MarshalChar(TP_FUNCTION_DA_GROUP_REFRESH);          // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result

	if (ok)
	{
		SOCmnPointer<SODaSTransaction> dest;
		DWORD cancelID = 0;

		// check group and item activation
		if (group->getActive())
		{
			group->copyCyclicTransaction(0, SODaSRequest::asyncMaxAge, maxAge, SODaSTransaction::tunnel, (SODaSTransaction**)dest);
			dest->setClientTransactionId(transactionID);

			if (dest->getRequestCount() > 0)
			{
				// -> start async refresh transaction
				dest->start(server);
				server->addTransaction(dest);
				cancelID = dest->getTransactionId();
				response->MarshalLong(cancelID);
			}
			else
			{
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no active item in group")));
				res = E_FAIL;
			}
		}
		else
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("group not active")));
			res = E_FAIL;
		}
	}
	else
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	// send reply
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
} // refresh


#endif // SOFEATURE_TUNNEL

#endif // SODAS
