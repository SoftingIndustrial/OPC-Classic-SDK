#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_TUNNEL

#include "SOAeSTPServer.h"
#include "SOAeSComServer.h"
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SODaSTPServer                                                              |
//-----------------------------------------------------------------------------
SOAeSTPServer::SOAeSTPServer(void)
{
}

SOAeSTPServer::~SOAeSTPServer(void)
{
}

void SOAeSTPServer::closeConnection(IN SOCmnTPConnection* tpCon)
{
	SOCmnPointer<SOSrvServer> server = (SOSrvServer*)((SOSrvTPConnection*)tpCon)->getServer();

	if (server.isNotNull())
	{
		if (server->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			((SOSrvTPConnection*)tpCon)->setServer(NULL);
			SOCmnList<SOCmnElement> subscriptionList(server->getLeafList());
			SOAeSSubscription* subscription = NULL;
			SOCmnListPosition pos;
			// remove the subscriptions
			pos = subscriptionList.getStartPosition();

			while (pos)
			{
				subscription = (SOAeSSubscription*)subscriptionList.getNext(pos);
				server->removeLeaf(subscription);
			}   //  end while

			SOCmnPointer<SOSrvEntry> entry = ::getSOAeSEntry();
			SOCmnPointer<SOSrvObjectRoot> root = entry->getObjectRoot();
			root->removeBranch(server);
		}   //  end if
	}
}   //  end closeConnection


//-----------------------------------------------------------------------------
// handleTPRequest
//
// - processing of TP request
//
// returns:
//		TRUE  - receiver has processed or is processing the request
//		FALSE - unknown request for this receiver
//
LONG SOAeSTPServer::handleRequest(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId)
{
	if ((interfaceId != TP_INTERFACE_AE_SERVER) && (interfaceId != TP_INTERFACE_CMN_SERVER))
	{
		return E_FAIL;
	}

	SOSRV_TP_CHECK_DEMO
	SOCmnPointer<SOSrvTPConnection> tpCon = (SOSrvTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SOAeSServer> server = (SOAeSServer*)tpCon->getServer();

	if (!server)
	{
		// no server object asociated with the connection -> create one
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOSrvObjectRoot> root;
		SOCmnPointer<SOAeSCreator> creator;
		SOCmnString clientName;
		SOCmnPointer<SOCmnSocketData> socketData = (SOCmnSocketData*)tpCon->getSocket();

		if (!socketData)
		{
			return E_FAIL;
		}

		// create server object and add to object tree
		creator = entry->getCreator();
		server = creator->createServer(0);
		server->setFlags(SOAESSERVER_FLAG_TUNNEL);
		struct sockaddr_in name;
		socklen_t namelen = sizeof(sockaddr_in);

		if (getpeername(socketData->m_socket, (sockaddr*)&name, &namelen) != SOCKET_ERROR)
		{
			SOCmnString id;
			DWORD dwAddr = ntohl(name.sin_addr.s_addr);
			server->setClientName(clientName.format(_T("%d.%d.%d.%d:%u"), (int)(0xFF & (dwAddr >> 24)), (int)(0xFF & (dwAddr >> 16)), (int)(0xFF & (dwAddr >> 8)), (int)(0xFF & dwAddr), ntohs(name.sin_port)));
			id.format(_T("[%s|%8.8lX|%s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)server->getClientName());
			server->setObjTraceId(id);
		}

		root = entry->getObjectRoot();
		root->addBranch(server);
		server->setTPConnection(tpCon);
		tpCon->setServer(server);
	}

	if (interfaceId == TP_INTERFACE_AE_SERVER)
	{
		switch (functionId)
		{
		case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
			tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
			break;

		case TP_FUNCTION_AE_SERVER_GETSTATUS:
			getStatus(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_ADDSUBSCRIPTION:
			addSubscription(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_REMOVESUBSCRIPTION:
			removeSubscription(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_QUERYEVENTCATEGORIES:
			queryEventCategories(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES:
			queryConditionNames(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_QUERYSUBCONDITIONNAMES:
			querySubConditionNames(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_QUERYSOURCECONDITIONS:
			querySourceConditions(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_QUERYEVENTATTRIBUTES:
			queryEventAttributes(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_ACKCONDITION:
			ackCondition(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_ADDAREABROWSER:
			addAreaBrowser(server, pCall);
			break;

		case TP_FUNCTION_AE_SERVER_REMOVEAREABROWSER:
			removeAreaBrowser(server, pCall);
			break;

			/* not used until now
			            case TP_FUNCTION_AE_SERVER_QUERYAVAILABLEFILTERS:
			                queryAvailableFilters(server, pCall);
			            break;
			            case TP_FUNCTION_AE_SERVER_GETCONDITIONSTATE:
			                getConditionState(server, pCall);
			            break;
			            case TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYAREA:
			                enableConditionByArea(server, pCall);
			            break;
			            case TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYSOURCE:
			                enableConditionBySource(server, pCall);
			            break;
			            case TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYAREA:
			                disableConditionByArea(server, pCall);
			            break;
			            case TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYSOURCE:
			                disableConditionBySource(server, pCall);
			            break;
			not used until now */
		default:
			return E_FAIL;
			break;
		}
	}
	else
	{
		// TP_INTERFACE_CMN_SERVER
		return E_FAIL;
	}

	return S_OK;
}   //  end handleRequest


//-----------------------------------------------------------------------------
//	getStatus
//
void SOAeSTPServer::getStatus(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_GETSTATUS,
							TPAE::Server::GetStatus)
	SOCmnPointer<SOAeSEntry> entryAE = ::getSOAeSEntry();
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	//SOAeSServer *server = pServer;
	DWORD serverState;
	SOCmnDateTime srvStartTime;
	SOCmnDateTime currentTime;
	SOCmnDateTime lastUpdateTime;
	SOCmnString vendorInfo;
	SOCmnString productVersion;
	WORD majorVersion;
	WORD minorVersion;
	WORD patchVersion;
	WORD buildNumber;
	SOCmnDWORDList lcidList;
	currentTime.now();
	entryAE->getStartTime(&srvStartTime);
	vendorInfo = entryAE->getVendorInfo();
	entryAE->getVersionInfo(&majorVersion, &minorVersion, &patchVersion, &buildNumber);
	productVersion.format(_T("%u.%2.2u.%u.%4.4u"), majorVersion, minorVersion, patchVersion, buildNumber);
	serverState = entryAE->getServerState();
	lcidList.create();
	pServer->queryAvailableLCIDs(lcidList);
	pServer->getUpdateTime(lastUpdateTime);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 128 +
						   lcidList.getCount() * sizeof(DWORD));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_GETSTATUS);       // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                  // result
	response->MarshalLong(serverState);
	response->MarshalDate(srvStartTime);
	response->MarshalDate(currentTime);
	response->MarshalString(vendorInfo);
	response->MarshalString(productVersion);
	response->MarshalDWORDList(&lcidList);
	response->MarshalDate(lastUpdateTime);
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  getStatus


//-----------------------------------------------------------------------------
//	addSubscription
//
void SOAeSTPServer::addSubscription(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_ADDSUBSCRIPTION,
							TPAE::Server::AddSubscription)
	SOCmnPointer<SOAeSEntry> entryAE = ::getSOAeSEntry();
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	//  In
	BYTE active;
	DWORD bufferTime;
	DWORD maxSize;
	DWORD clientSubscriptionHandle;
	//  Out
	DWORD revisedBufferTime;
	DWORD revisedMaxSize;
	BOOL ok = TRUE;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&clientSubscriptionHandle);
	ok &= request->UnMarshalChar(&active);
	ok &= request->UnMarshalLong(&bufferTime);
	ok &= request->UnMarshalLong(&maxSize);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	SOAESTPSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESTPSERVER_WAIT_FOR_IO_STARTUP
	SOCmnPointer<SOAeSSubscription> sus;
	// create tree group object
	res = server->createSubscription((active != 0), bufferTime, maxSize, clientSubscriptionHandle,
									 (SOAeSSubscription**)sus, &revisedBufferTime, &revisedMaxSize);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 128);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_ADDSUBSCRIPTION);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result

	if (SUCCEEDED(res))
	{
		DWORD key = 0xFFFFFFFF;
		sus->getKey(key);
		response->MarshalLong(key);
		response->MarshalLong(revisedBufferTime);
		response->MarshalLong(revisedMaxSize);
	}   //  end if

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end addSubscription

void SOAeSTPServer::removeSubscription(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_REMOVESUBSCRIPTION, TPAE::Server::RemoveSubscription)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD subscriptionHandle;
	BYTE forced;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&subscriptionHandle);
	ok &= request->UnMarshalChar(&forced);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	// unadvise callback
	SOCmnPointer<SOAeSSubscription> subscription;
	subscription = server->getSubscriptionByHandle(subscriptionHandle);

	if (!subscription)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	// remove subscription
	server->removeLeaf(subscription);
	// release refresh object if one is in progress
	SOCmnPointer<SOAeSRefresh> refresh = subscription->getRefresh();

	if (!!refresh)
	{
		refresh->cancel();
	}

	refresh.release();
	subscription.release();
	// build response
	tpCon->sendReply(pCall, TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_REMOVESUBSCRIPTION, res);
	SOSRV_END_TP_FUNCTION
} // removeSubscription


//-----------------------------------------------------------------------------
//	queryEventCategories
//
void SOAeSTPServer::queryEventCategories(
	IN SOAeSServer* /* pServer */,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYEVENTCATEGORIES,
							TPAE::Server::queryEventCategories)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	//  input param
	BOOL ok = TRUE;
	DWORD eventType;
	DWORD catCnt = 0;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&eventType);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOCmnList<SOCmnElement> categoryList(eventSpace->getBranchList());
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) +
						   64 * categoryList.getCount());
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYEVENTCATEGORIES);  // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK);                                            // result
	SOCmnListPosition pos = categoryList.getStartPosition();
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);  // categroy count

	while (pos)
	{
		SOAeSCategory* category = (SOAeSCategory*)categoryList.getNext(pos);

		if (category->getEventType() & eventType)
		{
			// category id
			response->MarshalLong(category->getNameInt());
			// category description
			response->MarshalString(category->getDescription());
			catCnt++;
		}   //  end if
	}   //  end while

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
	response->MarshalLong(catCnt);                           // final category count
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end queryEventCategories


//-----------------------------------------------------------------------------
//	queryConditionNames
//
void SOAeSTPServer::queryConditionNames(
	IN SOAeSServer* /* pServer */,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES,
							TPAE::Server::queryConditionNames)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	//  input param
	BOOL ok = TRUE;
	DWORD eventCategory;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&eventCategory);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	// get event category
	SOCmnPointer<SOAeSCategory> category = eventSpace->findCategory(eventCategory);

	if (!category)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
	}

	if (category->getEventType() != OPC_CONDITION_EVENT)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
	}

	SOCmnList<SOCmnElement> conditionClasses(category->getBranchList());
	response->MarshalLong(conditionClasses.getCount());
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64 * conditionClasses.getCount());
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK);                                            // result
	SOCmnListPosition pos = conditionClasses.GetHeadPosition();
	response->MarshalLong(conditionClasses.getCount());     // condition class count

	while (pos)
	{
		SOAeSConditionClass* conditionClass = (SOAeSConditionClass*)conditionClasses.getNext(pos);
		// condition name
		response->MarshalString(conditionClass->getName());
	}   //  end while

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end queryConditionNames


//-----------------------------------------------------------------------------
//	querySubConditionNames
//
void SOAeSTPServer::querySubConditionNames(
	IN SOAeSServer* /* pServer */,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYSUBCONDITIONNAMES,
							TPAE::Server::queryConditionNames)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	//  input param
	BOOL ok = TRUE;
	DWORD count;
	SOCmnString conditionName;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&conditionName);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOCmnPointer<SOAeSConditionClass> conditionClass = eventSpace->findConditionClass(conditionName);

	if (!conditionClass)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
	}   //  end if

	SOCmnList<SOCmnElement> subcondClassList;
	SOCmnList<SOCmnElement> *pLeafList = conditionClass->getLeafList();

	if (pLeafList)
	{
		// multiple state condition
		subcondClassList = pLeafList;
		count = subcondClassList.getCount();
	}
	else
	{
		// single state conditions
		subcondClassList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
		subcondClassList.add(conditionClass);
		count = 1;
	}   //  end if ... else

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) +
						   sizeof(DWORD) +
						   64 * count);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYSUBCONDITIONNAMES);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK);                                            // result
	response->MarshalLong(count);                                                       // subcondition count
	SOCmnListPosition pos = subcondClassList.GetHeadPosition();

	while (pos)
	{
		SOCmnElement* subconditonClass = subcondClassList.getNext(pos);
		// add subcondition name
		response->MarshalString(subconditonClass->getName());
	}   //  end while

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end querySubConditionNames


//-----------------------------------------------------------------------------
//	querySourceConditions
//
void SOAeSTPServer::querySourceConditions(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYSOURCECONDITIONS,
							TPAE::Server::querySourceConditions)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpace = entry->getAreaSpaceRoot();
	SOAeSServer* server = pServer;
	//  input param
	BOOL ok = TRUE;
	DWORD count = 0;
	SOCmnString sourceName;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&sourceName);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOCmnPointer<SOAeSSource> source = (SOAeSSource*)areaSpace->findLeaf(sourceName, TRUE);
	SOCmnList<SOCmnElement> conditionList;

	if (!source)
	{
		// check source name if no object exists
		if (!server->isSourceName(sourceName))
		{
			// invalid source name
			SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
		}   //  end if
	}
	else
	{
		conditionList = source->getAdditionalList();
	}   //  end if ... else

	SOCmnStringList conditionNameList;
	conditionNameList.create();
	server->getConditionNamesOfSourceName(sourceName, conditionNameList);
	count = conditionNameList.getCount();

	if (!!conditionList)
	{
		count += conditionList.getCount();
	}   //  end if

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64 * count);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYSOURCECONDITIONS); // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK);                                            // result
	response->MarshalLong(count);
	SOCmnListPosition pos;

	//  names of condition objects
	if (!!conditionList)
	{
		pos = conditionList.getStartPosition();

		while (pos)
		{
			SOAeSCondition* cond = (SOAeSCondition*)conditionList.getNext(pos);
			SOAeSConditionClass* condClass = cond->getConditionClass();

			if (!!condClass)
			{
				response->MarshalString(condClass->getName());
			}   //  end if
		}   //  end while
	}   //  end if

	//  condition names
	pos = conditionNameList.getStartPosition();

	while (pos)
	{
		response->MarshalString(conditionNameList.getNext(pos));
	}   //  end while

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end querySourceConditions


//-----------------------------------------------------------------------------
//	queryEventAttributes
//
void SOAeSTPServer::queryEventAttributes(
	IN SOAeSServer* /* pServer */,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYEVENTATTRIBUTES,
							TPAE::Server::queryEventAttributes)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	//  input param
	BOOL ok = TRUE;
	DWORD eventCategory;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&eventCategory);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOCmnPointer<SOAeSCategory> cat = eventSpace->findCategory(eventCategory);

	if (!cat)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
	}   //  end if

	// build response
	SOCmnList<SOCmnElement> attrList(cat->getAdditionalList());
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64 * attrList.getCount());
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK);                                            // result
	SOCmnListPosition pos = attrList.getStartPosition();
	response->MarshalLong(attrList.getCount());             // attribute count

	while (pos)
	{
		SOAeSAttribute* attr = (SOAeSAttribute*)attrList.getNext(pos);
		response->MarshalLong(attr->getNameInt());
		response->MarshalString(attr->getDescription());
		response->MarshalShort(attr->getDatatype());
	}   //  end while

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end queryEventAttributes




//-----------------------------------------------------------------------------
//	ackCondition
//
void SOAeSTPServer::ackCondition(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_ACKCONDITION, TPAE::Server::AckCondition)
	SOCmnPointer<SOAeSEntry> entryAE = ::getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpaceRoot = entryAE->getAreaSpaceRoot();
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD count;                // number of acks
	SOCmnString acknowledgerID; // acknowledger ID
	SOCmnString comment;        // comment
	SOCmnString sourceName;     // source names
	SOCmnString conditionName;  // condition names
	SOCmnDateTime activeTime;   // activation times
	FILETIME activeFileTime;
	DWORD cookie;               // cookies
	HRESULT res = S_OK;
	BOOL ok = TRUE;
	DWORD i = 0;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&acknowledgerID);
	ok &= request->UnMarshalString(&comment);
	ok &= request->UnMarshalLong(&count);

	if (!ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOAESTPSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESTPSERVER_WAIT_FOR_IO_STARTUP
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + count * sizeof(DWORD));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_ACKCONDITION);  // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(S_OK); // global result may be changed later;

	for (i = 0; i < count; i++)
	{
		SOCmnPointer<SOAeSSource> source;
		SOCmnPointer<SOAeSCondition> condition;
		// find condition object
		ok &= request->UnMarshalString(&sourceName);
		ok &= request->UnMarshalString(&conditionName);
		ok &= request->UnMarshalDate(&activeTime);
		ok &= request->UnMarshalLong(&cookie);

		if (ok)
		{
			HRESULT result = E_FAIL;
			source = (SOAeSSource*)areaSpaceRoot->findLeaf(sourceName, TRUE);

			if (source.isNotNull())
			{
				// source object exists
				condition = (SOAeSCondition*)source->findAdditionalElement(conditionName);
			}   //  end if

			activeTime.get(&activeFileTime);

			if (condition.isNotNull())
			{
				// condition object exists
				result = condition->acknowledge(acknowledgerID, comment, &activeFileTime, cookie);
			}
			else
			{
				// havn't found condition object
				result = pServer->ackConditionByName(sourceName, conditionName, acknowledgerID, comment, &activeFileTime, cookie);
			}   //  end if ... else

			//  add the result to the TP reply
			response->MarshalLong(result);

			if (FAILED(result))
			{
				res = S_FALSE;
			}
		}   //  end if
		else // !ok
		{
			SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
		}   //  end if
	}   //  end for

	if (res != S_OK)
	{
		response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
		response->MarshalLong(res);                                 // final result
	}

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end ackCondition


//-----------------------------------------------------------------------------
//	createEventSubscription
//
void SOAeSTPServer::addAreaBrowser(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_ADDAREABROWSER,
							TPAE::Server::AddAreaBrowser)
	SOCmnPointer<SOAeSEntry> entryAE = ::getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpaceRoot = entryAE->getAreaSpaceRoot();
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	SOAESTPSERVER_WAIT_FOR_AREASPACE_STARTUP
	// create area browser object
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	res = server->createAreaBrowser((SOAeSAreaBrowser**)areaBrowser);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
	response->MarshalChar(TP_FUNCTION_AE_SERVER_ADDAREABROWSER);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

	if (SUCCEEDED(res))
	{
		DWORD areaBrowserKey = 0xFFFFFFFF; // -1

		if (areaBrowser->getKey(areaBrowserKey))
		{
			response->MarshalLong(res);
			response->MarshalLong(areaBrowserKey);      // object id
		}
		else
		{
			response->MarshalLong((unsigned long)E_OUTOFMEMORY);
		}   //  end if ... else
	}
	else
	{
		response->MarshalLong(res);
	}   //  end if ... else

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end createAreaBrowser

void SOAeSTPServer::removeAreaBrowser(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_REMOVEAREABROWSER, TPAE::Server::RemoveAreaBrowser)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD areaBrowserHandle;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&areaBrowserHandle);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	// unadvise callback
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	areaBrowser = server->getAreaBrowserByHandle(areaBrowserHandle);

	if (!areaBrowser)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	// remove subscription
	server->removeAdditionalElement(areaBrowser);
	areaBrowser.release();
	// build response
	tpCon->sendReply(pCall, TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_REMOVEAREABROWSER, res);
	SOSRV_END_TP_FUNCTION
} // removeAreaBrowser

/* not used until now

//-----------------------------------------------------------------------------
//  queryAvailableFilters
//
void SOAeSTPServer::queryAvailableFilters(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall)
{

    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_QUERYAVAILABLEFILTERS,
        TPAE::Server::queryAvailableFilters)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;

    //  no input parameters
    DWORD filterMask = server->queryAvailableFilters();

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_QUERYAVAILABLEFILTERS); // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

    //  add the out param
    response->MarshalLong(filterMask);

    tpCon->reply(pCall);
    SOSRV_END_TP_FUNCTION

}   //  end queryAvailableFilters


//-----------------------------------------------------------------------------
//  getConditionState
//
void SOAeSTPServer::getConditionState(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{

    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_GETCONDITIONSTATE,
        TPAE::Server::getConditionState)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
    SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
    SOAeSServer *server = pServer;

    //  input param
    BOOL ok = TRUE;
    SOCmnString sourceName;
    SOCmnString conditionName;
    SOCmnDWORDList attributeIDs;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalString(&sourceName);
    ok &= request->UnMarshalString(&conditionName);
    ok &= request->UnMarshalDWORDList(&attributeIDs);

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    SOAESTPSERVER_WAIT_FOR_AREASPACE_STARTUP
    SOAESTPSERVER_WAIT_FOR_IO_STARTUP

    if (!server->supportGetConditionState())
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_NOTIMPL)
    }   //  end if

    SOCmnPointer<SOAeSSource> source = (SOAeSSource*)eventSpace->findLeaf(sourceName, TRUE);
    SOCmnPointer<SOAeSCondition> condition;

    if (!!source)
    {
        // source object exists
        condition = (SOAeSCondition*)source->findAdditionalElement(conditionName);
    }   //  end if

    if (!condition)
    {
        // havn't found condition object
        // give the application the change to return the condition state
        res = server->createConditionByName(sourceName, conditionName, &condition);
    }   //  end if

    if (!SUCCEEDED(res))
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
    }   //  end if

    SOCmnPointer<SOAeSSubConditionClass> actSubCond = condition->getActSubConditionClass();
    SOCmnPointer<SOAeSConditionClass> condClass = condition->getConditionClass();
    SOCmnList<SOAeSSubConditionClass> subCondList((SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList());

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64*8 +
        (subCondList.getCount() ? subCondList.getCount():1)*64*3 +
        attributeIDs.getCount()*64);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_GETCONDITIONSTATE); // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

    response->MarshalChar(condition->getObjectState());

    if (((condition->getObjectState() & OPC_CONDITION_ACTIVE) != 0))
    {
        if (!!actSubCond)
        {
            response->MarshalString(actSubCond->getName());
            response->MarshalString(actSubCond->getDefinition());
        }
        else
        {
            response->MarshalString(condClass->getName());
            response->MarshalString(condClass->getDefinition());
        }   //  end if ... else

        response->MarshalLong(condition->getSeverity());
        response->MarshalString(condition->getMessage());
    }   // end if

    response->MarshalShort(condition->getQuality());
    FILETIME time;
    SOCmnDateTime soTime;

    condition->getAckTime(&time);
    soTime.set(&time);
    response->MarshalDate(soTime);

    condition->getActSubConditionTime(&time);
    soTime.set(&time);
    response->MarshalDate(soTime);

    condition->getActiveTime(&time);
    soTime.set(&time);
    response->MarshalDate(soTime);

    condition->getInactiveTime(&time);
    soTime.set(&time);
    response->MarshalDate(soTime);

    response->MarshalString(condition->getAckID());
    response->MarshalString(condition->getAckComment());

    if (!subCondList.isEmpty())
    {
        response->MarshalLong(subCondList.getCount());
        SOAeSSubConditionClass *subCondClass;
        SOCmnListPosition pos = subCondList.getStartPosition();
        while (pos)
        {
            subCondClass = subCondList.getNext(pos);

            response->MarshalString(subCondClass->getName());
            response->MarshalString(subCondClass->getDefinition());
            response->MarshalString(subCondClass->getDescription());
            response->MarshalLong(subCondClass->getSeverity());
        }   //  end while
    }
    else
    {
        response->MarshalLong(1);
        response->MarshalString(condClass->getName());
        response->MarshalString(condClass->getDefinition());
        response->MarshalString(condClass->getActiveMessage());
        response->MarshalLong(condClass->getActiveSeverity());
    }   //  end if ... else

    SOCmnList<SOAeSAttributeValue> attrValList = condition->getAttributeValueList();
    SOCmnPointer<SOAeSAttributeValue> attrVal;

    response->MarshalLong(attributeIDs.getCount());
    SOCmnListPosition pos = attributeIDs.getStartPosition();
    while(pos)
    {
        DWORD attributeID = attributeIDs.getNext(pos);
        SOCmnVariant value;
        DWORD result = E_FAIL;
        attrVal = attrValList.findKey(attributeID);
        if (!!attrVal)
        {
            attrVal->getValue(&value);
            result = S_OK;
        }   //  end if
        response->MarshalLong(attributeID);
        response->MarshalVariant(value);
        response->MarshalLong(result);
    }   //  end while

    tpCon->reply(pCall);
    SOSRV_END_TP_FUNCTION

}   //  end getConditionState


//-----------------------------------------------------------------------------
//  enableConditionByArea
//
void SOAeSTPServer::enableConditionByArea(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{
    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYAREA,
        TPAE::Server::enableConditionByArea)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;

    //  input param
    BOOL ok = TRUE;
    DWORD numAreas;
    SOCmnString area;
    SOCmnStringList areas;
    DWORD index = 0;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalLong(&numAreas);

    while (ok && index < numAreas)
    {
        ok = request->UnMarshalString(&area);
        areas.add(area);
    }   //  end if

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    res = enableConditions(pServer, TRUE, TRUE, areas);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYAREA); // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end enableConditionByArea


//-----------------------------------------------------------------------------
//  enableConditionBySource
//
void SOAeSTPServer::enableConditionBySource(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{
    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYSOURCE,
        TPAE::Server::enableConditionBySource)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;

    //  input param
    BOOL ok = TRUE;
    DWORD numSources;
    SOCmnString source;
    SOCmnStringList sources;
    DWORD index = 0;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalLong(&numSources);

    while (ok && index < numSources)
    {
        ok = request->UnMarshalString(&source);
        sources.add(source);
    }   //  end if

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    res = enableConditions(pServer, TRUE, FALSE, sources);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYSOURCE);   // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end enableConditionBySource


//-----------------------------------------------------------------------------
//  disableConditionByArea
//
void SOAeSTPServer::disableConditionByArea(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{
    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYAREA,
        TPAE::Server::disableConditionByArea)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;

    //  input param
    BOOL ok = TRUE;
    DWORD numAreas;
    SOCmnString area;
    SOCmnStringList areas;
    DWORD index = 0;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalLong(&numAreas);

    while (ok && index < numAreas)
    {
        ok = request->UnMarshalString(&area);
        areas.add(area);
    }   //  end if

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    res = enableConditions(pServer, FALSE, TRUE, areas);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYAREA);    // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end disableConditionByArea


//-----------------------------------------------------------------------------
//  disableConditionBySource
//
void SOAeSTPServer::disableConditionBySource(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{
    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SERVER, TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYSOURCE,
        TPAE::Server::disableConditionBySource)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;

    //  input param
    BOOL ok = TRUE;
    DWORD numSources;
    SOCmnString source;
    SOCmnStringList sources;
    DWORD index = 0;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalLong(&numSources);

    while (ok && index < numSources)
    {
        ok = request->UnMarshalString(&source);
        sources.add(source);
    }   //  end if

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    res = enableConditions(pServer, FALSE, FALSE, sources);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SERVER);             // interface id
    response->MarshalChar(TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYSOURCE);  // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end disableConditionBySource

//-----------------------------------------------------------------------------
//  enableConditions
//
HRESULT SOAeSTPServer::enableConditions(
    SOAeSServer *pServer,
    BOOL enable,
    BOOL areaOrSource,
    SOCmnStringList& aStringList)
{
    // check if server should support this fuction
    if (!pServer->supportEnableCondition(enable, areaOrSource))
    {
        return E_NOTIMPL;
    }   //  end if

    SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
    SOCmnPointer<SOAeSAreaSpaceRoot> root = entry->getAreaSpaceRoot();
    SOCmnList<SOAeSCondition> allCondList;
    SOAeSCondition* cond;
    SOCmnListPosition condPos;
    SOCmnString name;

    allCondList.create(SOCMNLIST_TYPE_ORG_LIST|SOCMNLIST_TYPE_KEY_NO|SOCMNLIST_TYPE_OBJ_OBJECT);

    if (areaOrSource)
    {
        SOCmnPointer<SOAeSArea> area;

        // all area names
        SOCmnListPosition pos = aStringList.GetHeadPosition();
        while (pos)
        {
            name = aStringList.getNext(pos);
            area = (SOAeSArea *)root->findBranch(name, TRUE);
            if ((SOAeSArea *)area)
            {
                // area object exists
                SOAeSAreaSpaceRoot::getConditions(area, allCondList);
            }
            else
            {
                if (!pServer->isAreaName(name))
                {
                    _TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("enableConditions: called with invalid area name")));
                    return E_INVALIDARG;
                }   //  end if
            }   //  end if
        }   //  end while
    }
    else
    {
        SOCmnPointer<SOAeSSource> source;
        SOCmnList<SOAeSCondition> condList;

        // all area names
        SOCmnListPosition pos = aStringList.GetHeadPosition();
        while (pos)
        {
            name = aStringList.getNext(pos);

            source = (SOAeSSource *)root->findLeaf(name, TRUE);

            if ((SOAeSSource *)source)
            { // source object exists
                condList = (SOCmnList<SOAeSCondition> *)source->getAdditionalList();
                if (!!condList)
                {
                    condPos = condList.getStartPosition();
                    while (condPos)
                    {
                        cond = condList.getNext(condPos);
                        allCondList.add(cond);
                    }
                }
            }
            else
            {
                if (!pServer->isSourceName(name))
                {
                    _TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("enableConditions: called with invalid source name")));
                    return E_INVALIDARG;
                }
            }
        }   //  end while
        condList.autoUnlock();
    }

    // all area or source names are valid

    // enable all condition objects
    condPos = allCondList.getStartPosition();
    while (condPos)
    {
        cond = allCondList.getNext(condPos);
        cond->enable(enable);
    }   //  end while

    // call all enable overwritables for string based area space
    SOCmnListPosition pos = aStringList.GetHeadPosition();
    while (pos)
    {
        name = aStringList.getNext(pos);
        if (areaOrSource)
        {
            pServer->enableConditionByAreaName(enable, name);
        }
        else
        {
            pServer->enableConditionBySourceName(enable, name);
        }   //  end if ... else
    }   //  end while

    return S_OK;

}   //  end enableConditions

not used until now */

#endif // SOFEATURE_TUNNEL

#endif // SOAES
