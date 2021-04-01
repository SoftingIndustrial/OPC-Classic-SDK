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
//  Filename    : SODaSTPServer.cpp                                           |
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
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SODaSTPServer                                                            |
//-----------------------------------------------------------------------------

SODaSTPServer::SODaSTPServer(void)
{
}

SODaSTPServer::~SODaSTPServer(void)
{
}

void SODaSTPServer::closeConnection(IN SOCmnTPConnection* tpCon)
{
	SOCmnPointer<SOSrvServer> server = (SOSrvServer*)((SOSrvTPConnection*)tpCon)->getServer();

	if (server.isNotNull())
	{
		if (server->is(SOCMNOBJECT_TYPE_SERVER_DA))
		{
			((SOSrvTPConnection*)tpCon)->setServer(NULL);
			SOCmnList<SOCmnElement> groupList(server->getBranchList());
			SODaSGroup* group = NULL;
			SOCmnListPosition posGrp;

			groupList.dontAutoLock();
			groupList.lock();
			groupList.setDirty(FALSE);
			posGrp = groupList.getStartPosition();

			while(posGrp)
			{
				groupList.lock();
				if (groupList.isDirty())
				{
					posGrp = groupList.getStartPosition();

					groupList.setDirty(FALSE);
					if (posGrp == NULL)
					{
						groupList.unlock();
						break;
					}
				}

				group = (SODaSGroup *)groupList.getNext(posGrp);
				group->addRef();
				groupList.unlock();

				((SODaSServer *)(SOSrvServer *)server)->removeGroup(group->getServerHandle(), TRUE);
				group->release();

			}

			groupList.unlock();
			SOCmnPointer<SOSrvEntry> entry = ::getSODaSEntry();
			SOCmnPointer<SOSrvObjectRoot> root;
			root = entry->getObjectRoot();
			root->removeBranch(server);
		}
	}
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
LONG SODaSTPServer::handleRequest(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId)
{
	if ((interfaceId != TP_INTERFACE_DA_SERVER) && (interfaceId != TP_INTERFACE_CMN_SERVER))
	{
		return E_FAIL;
	}

	SOSRV_TP_CHECK_DEMO
	SOCmnPointer<SOSrvTPConnection> tpCon = (SOSrvTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SODaSServer> server = (SODaSServer*)tpCon->getServer();

	if (!server)
	{
		// no server object asociated with the connection -> create one
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SOSrvObjectRoot> root;
		SOCmnPointer<SODaSCreator> creator;
		SOCmnString clientName;
		SOCmnPointer<SOCmnSocketData> socketData = (SOCmnSocketData*)tpCon->getSocket();

		if (!socketData)
		{
			return E_FAIL;
		}

		// create server object and add to object tree
		creator = entry->getCreator();
		server = creator->createServer(FALSE, 0);
		server->setFlags(SODASSERVER_FLAG_TUNNEL);
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

	if (interfaceId == TP_INTERFACE_DA_SERVER)
	{
		switch (functionId)
		{
		case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
			tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
			break;

		case TP_FUNCTION_DA_SERVER_GETSTATUS:
			getStatus(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_BROWSE:
			browse(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_GETPROPERTIES:
			getProperties(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_READ:
			read(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_WRITE:
			write(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_ADDGROUP:
			addGroup(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_REMOVEGROUP:
			removeGroup(server, pCall);
			break;

		case TP_FUNCTION_DA_SERVER_SETCLIENTNAME:
			setClientName(server, pCall);
			break;

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
}


void SODaSTPServer::getStatus(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_GETSTATUS, TPDA::Server::GetStatus)
	SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SODaSServer* server = pServer;
	SOCmnList<SOCmnElement> groupList(server->getBranchList());
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
	SOCmnDWORDList supportedInterfaces;
	DWORD groupCnt;
	DWORD bandwidth;
	SOCmnString statusInfo;
	currentTime.now();
	entryDA->getStartTime(&srvStartTime);
	vendorInfo = entryDA->getVendorInfo();
	entryDA->getVersionInfo(&majorVersion, &minorVersion, &patchVersion, &buildNumber);
	productVersion.format(_T("%u.%2.2u.%u.%4.4u"), majorVersion, minorVersion, patchVersion, buildNumber);
	serverState = entryDA->getServerState();
	lcidList.create();
	pServer->queryAvailableLCIDs(lcidList);
	supportedInterfaces.create();
	pServer->getUpdateTime(lastUpdateTime);
	groupCnt = groupList.getCount();
	groupList.autoUnlock();
	bandwidth = server->getBandwidth();
	statusInfo = entryDA->getServerStateString();
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_GETSTATUS);       // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(S_OK);                                  // result
	response->MarshalLong(serverState);
	response->MarshalDate(srvStartTime);
	response->MarshalDate(currentTime);
	response->MarshalString(vendorInfo);
	response->MarshalString(productVersion);
	response->MarshalDWORDList(&lcidList);
	response->MarshalDate(lastUpdateTime);
	response->MarshalLong(groupCnt);
	response->MarshalLong(bandwidth);
	response->MarshalString(statusInfo);
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
} // getStatus


void SODaSTPServer::browse(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_BROWSE, TPDA::Server::Browse)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	SOCmnString itemId;
	SOCmnString cp;
	DWORD maxElements;
	DWORD browseFilter;
	SOCmnString elementNameFilter;
	SOCmnString vendorFilter;
	BYTE returnAllProperties;
	BYTE retrievePropertyValues;
	DWORD propInCnt;
	DWORD* pPropertyIDs = NULL;
	BOOL moreElements;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&itemId);
	ok &= request->UnMarshalString(&cp);
	ok &= request->UnMarshalLong(&maxElements);
	ok &= request->UnMarshalLong(&browseFilter);
	ok &= request->UnMarshalString(&elementNameFilter);
	ok &= request->UnMarshalString(&vendorFilter);
	ok &= request->UnMarshalChar(&returnAllProperties);
	ok &= request->UnMarshalChar(&retrievePropertyValues);
	ok &= request->UnMarshalArrayLen(&propInCnt);

	if ((ok) && (propInCnt > 0))
	{
		pPropertyIDs = (DWORD*) malloc(propInCnt * sizeof(DWORD));

		if (!pPropertyIDs)
		{
			SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
		}

		DWORD propertyID;
		SOCmnString propertyName;
		DWORD i;

		for (i = 0; i < propInCnt; i++)
		{
			ok &= request->UnMarshalLong(&propertyID);
			ok &= request->UnMarshalString(&propertyName);

			if (ok)
			{
				if (propertyID != 0)
				{
					pPropertyIDs[i] = propertyID;
				}
				else
				{
					pPropertyIDs[i] = server->getPropertyIDByName(propertyName);
				}
			}
		}
	}

	if (!ok)
	{
		// check marshaling
		if (pPropertyIDs)
		{
			free(pPropertyIDs);
		}

		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	// check input
	if (!(((browseFilter == OPC_BROWSE_FILTER_ALL) || (browseFilter == OPC_BROWSE_FILTER_BRANCHES) || (browseFilter == OPC_BROWSE_FILTER_ITEMS))))
	{
		if (pPropertyIDs)
		{
			free(pPropertyIDs);
		}

		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	SODASTPSERVER_WAIT_FOR_NAMESPACE_STARTUP

	if (retrievePropertyValues)
		SODASTPSERVER_WAIT_FOR_IO_STARTUP
		SOCmnPointer<SOCmnElement> element;

	SOSrvBrowser::browserFilterType fType;
	SOCmnList<SODaSBrowseElementData> listBE;

	if (maxElements == 0)
	{
		maxElements = 0xFFFFFFFF;
	}

	listBE.create((SOCmnListHeader::isRehashingEnabled() ? SOCMNLIST_TYPE_ORG_MAP : SOCMNLIST_TYPE_ORG_LIST) | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

	// check for valid item id
	if (itemId.getLength() != 0)
	{
		if (!server->isNameSpaceElement(itemId, &element))
		{
			if (pPropertyIDs)
			{
				free(pPropertyIDs);
			}

			SOSRV_RETURN_TP_FUNCTION_RES((server->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID)
		}
	}
	else
	{
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
		element = entry->getNameSpaceRoot(server->getServerInstance());
	}

	switch (browseFilter)
	{
	case OPC_BROWSE_FILTER_ALL:
		fType = SOSrvBrowser::typeAll;
		break;

	case OPC_BROWSE_FILTER_BRANCHES:
		fType = SOSrvBrowser::typeBranch;
		break;

	case OPC_BROWSE_FILTER_ITEMS:
		fType = SOSrvBrowser::typeItem;
		break;

	default:
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	// get the browse elements
	res = server->browse(itemId, element, fType, elementNameFilter, vendorFilter, maxElements, cp, moreElements, &listBE);

	if (SUCCEEDED(res))
	{
		DWORD count = listBE.getCount();
		// build response
		response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16 + (count * 256));
		response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
		response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
		response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
		response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
		response->MarshalChar(TP_FUNCTION_DA_SERVER_BROWSE);          // function id
		response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags

		if ((!cp.isEmpty()) && (moreElements == TRUE))
		{
			moreElements = FALSE;
		}

		response->MarshalLong(res);                                   // pro forma result
		response->MarshalString(cp);
		response->MarshalChar((u_char)moreElements);
		response->MarshalLong(count);

		if (count > 0)
		{
			SODaSBrowseElementData* pBE;
			SOCmnListPosition posBE;
			DWORD propCnt = 0;      // default for return all
			DWORD* pPropIDs = NULL; // default for return all
			BOOL doGetProps = TRUE;
			SOCmnList<SODaSPropertyData> listPD;
			SOCmnListPosition posPD;
			SODaSPropertyData* pPD;
			HRESULT propertiesResult;
			DWORD propertyCnt;
			DWORD k;
			listPD.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

			if (!returnAllProperties)
			{
				if (propInCnt > 0)
				{
					propCnt = propInCnt;
					pPropIDs = pPropertyIDs;
				}
				else
				{
					doGetProps = FALSE;
				}
			}

			posBE = listBE.getStartPosition();

			while (posBE)
			{
				pBE = listBE.getNext(posBE);
				// browse element data
				response->MarshalString(pBE->m_name);
				response->MarshalString(pBE->m_itemID);
				response->MarshalLong(pBE->m_flags);

				if (doGetProps)
				{
					// get the properties of the namespace element
					propertiesResult = server->getProperties(pBE->m_itemID, pBE->m_nsElement, propCnt, pPropIDs, FALSE, retrievePropertyValues, &listPD);

					if (SUCCEEDED(propertiesResult))
					{
						propertyCnt = listPD.getCount();
						SOCmnVariant** pvData = NULL;
						HRESULT* pErrors = NULL;
						SOCmnVariant emptyVal;

						if (retrievePropertyValues)
						{
							DWORD accessRights = OPC_READABLE;
							SOCmnPointer<SOCmnElement> objBE;
							pvData = (SOCmnVariant**) malloc(propCnt * sizeof(SOCmnVariant*));
							pErrors = (HRESULT*) malloc(propCnt * sizeof(HRESULT));

							if ((!pvData) || (!pErrors))
							{
								if (pvData)
								{
									free(pvData);
								}

								if (pErrors)
								{
									free(pErrors);
								}

								if (pPropertyIDs)
								{
									free(pPropertyIDs);
								}

								SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
							}

							for (k = 0; k < propertyCnt; k++)
							{
								pvData[k] = new SOCmnVariant();
								pErrors[k] = E_FAIL;
							}

							if (pBE->m_nsElement.isNotNull())
							{
								objBE = pBE->m_nsElement;
							}
							else
							{
								objBE = server->getNameSpaceElement(pBE->m_itemID);
							}

							if (objBE.isNotNull())
							{
								accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(server);

								if (S_OK != server->readProperties(pBE->m_itemID, objBE, accessRights, &listPD, 0, NULL, FALSE, NULL, NULL, pvData, pErrors))
								{
									propertiesResult = S_FALSE;
									res = S_FALSE;
								}
							}
							else
							{
								for (k = 0; k < propertyCnt; k++)
								{
									pErrors[k] = E_FAIL;
								}

								propertiesResult = S_FALSE;
								res = S_FALSE;
							}
						}

						if (propertiesResult != S_OK)
						{
							res = S_FALSE;
						}

						response->MarshalLong(propertiesResult);
						response->MarshalLong(propertyCnt);
						posPD = listPD.getStartPosition();
						k = 0;

						while (posPD)
						{
							pPD = listPD.getNext(posPD);
							response->MarshalLong(pPD->m_result);
							response->MarshalShort(pPD->m_datatype);
							response->MarshalLong(pPD->m_pid);
							response->MarshalString(pPD->m_name);
							response->MarshalString(pPD->m_itemID);
							response->MarshalString(pPD->m_descr);

							if (pvData)
							{
								response->MarshalVariant(pvData[k]);
							}
							else
							{
								response->MarshalVariant(emptyVal);
							}
						}

						if (pvData)
						{
							free(pvData);
						}

						if (pErrors)
						{
							free(pErrors);
						}
					}
					else
					{
						response->MarshalLong(propertiesResult);
						res = S_FALSE;
					}
				}
				else
				{
					// no properties
					response->MarshalLong(S_OK);
					response->MarshalLong(0);
				}

				listPD.removeAll();
			}
		}

		response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
		response->MarshalLong(res);                                 // final result
		tpCon->reply(pCall);
	}
	else
	{
		tpCon->sendReply(pCall, TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_BROWSE, res);
	}

	if (pPropertyIDs)
	{
		free(pPropertyIDs);
	}

	SOSRV_END_TP_FUNCTION
} // browse


void SODaSTPServer::getProperties(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_GETPROPERTIES, TPDA::Server::GetProperties)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD itemIdCnt;
	SOCmnString itemId;
	SOCmnStringList itemIdList;
	BYTE retrievePropertyValues;
	DWORD propInCnt;
	DWORD* pPropertyIDs = NULL;
	DWORD i, k;
	itemIdList.create();
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&itemIdCnt);

	for (i = 0; i < itemIdCnt; i++)
	{
		ok &= request->UnMarshalString(&itemId);

		if (ok)
		{
			itemIdList.add(itemId);
		}
	}

	ok &= request->UnMarshalChar(&retrievePropertyValues);
	ok &= request->UnMarshalArrayLen(&propInCnt);

	if ((ok) && (propInCnt > 0))
	{
		pPropertyIDs = (DWORD*) malloc(propInCnt * sizeof(DWORD));

		if (!pPropertyIDs)
		{
			SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
		}

		DWORD propertyID;
		SOCmnString propertyName;

		for (i = 0; i < propInCnt; i++)
		{
			ok &= request->UnMarshalLong(&propertyID);
			ok &= request->UnMarshalString(&propertyName);

			if (ok)
			{
				if (propertyID != 0)
				{
					pPropertyIDs[i] = propertyID;
				}
				else
				{
					pPropertyIDs[i] = server->getPropertyIDByName(propertyName);
				}
			}
		}
	}

	if (!ok)
	{
		// check marshaling
		if (pPropertyIDs)
		{
			free(pPropertyIDs);
		}

		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	SODASTPSERVER_WAIT_FOR_NAMESPACE_STARTUP

	if (retrievePropertyValues)
		SODASTPSERVER_WAIT_FOR_IO_STARTUP
		// build response
		response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + (itemIdCnt * 256));

	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_GETPROPERTIES);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // pro forma result
	SOCmnListPosition pos;
	HRESULT itemRes;
	SOCmnPointer<SOCmnElement> element;
	SOCmnList<SODaSPropertyData> listPD;
	SOCmnListPosition posPD;
	SODaSPropertyData* pPD;
	pos = itemIdList.getStartPosition();

	while (pos)
	{
		itemId = itemIdList.getNext(pos);

		// check for vaild item id
		if (pServer->isNameSpaceElement(itemId, &element))
		{
			itemRes = pServer->getProperties(itemId, element, propInCnt, pPropertyIDs, FALSE, retrievePropertyValues, &listPD);
			DWORD propCnt = listPD.getCount();
			SOCmnVariant** pvData = NULL;
			HRESULT* pErrors = NULL;
			SOCmnVariant emptyVal;

			if (SUCCEEDED(itemRes))
			{
				if ((retrievePropertyValues) && (propCnt > 0))
				{
					DWORD accessRights = OPC_READABLE;
					SOCmnPointer<SOCmnElement> objBE;
					pvData = (SOCmnVariant**) malloc(propCnt * sizeof(SOCmnVariant*));
					pErrors = (HRESULT*) malloc(propCnt * sizeof(HRESULT));

					if ((!pvData) || (!pErrors))
					{
						if (pvData)
						{
							free(pvData);
						}

						if (pErrors)
						{
							free(pErrors);
						}

						if (pPropertyIDs)
						{
							free(pPropertyIDs);
						}

						SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
					}

					for (k = 0; k < propCnt; k++)
					{
						pvData[k] = new SOCmnVariant();
					}

					if (element.isNotNull())
					{
						objBE = element;
					}
					else
					{
						objBE = server->getNameSpaceElement(itemId);
					}

					if (objBE.isNotNull())
					{
						accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(pServer);

						if (S_OK != server->readProperties(itemId, objBE, accessRights, &listPD, 0, NULL, FALSE, NULL, NULL, pvData, pErrors))
						{
							itemRes = S_FALSE;
						}
					}
					else
					{
						// browse object but can not create it !
						itemRes = OPC_E_UNKNOWNITEMID;
					}
				}
			}

			response->MarshalLong(itemRes);                           // item result

			if (itemRes != S_OK)
			{
				res = S_FALSE;
			}

			if (SUCCEEDED(itemRes))
			{
				response->MarshalLong(propCnt);                         // property count
				posPD = listPD.getStartPosition();
				k = 0;

				while (posPD)
				{
					pPD = listPD.getNext(posPD);
					response->MarshalLong(pPD->m_result);
					response->MarshalShort(pPD->m_datatype);
					response->MarshalLong(pPD->m_pid);
					response->MarshalString(pPD->m_name);
					response->MarshalString(pPD->m_itemID);
					response->MarshalString(pPD->m_descr);

					if (pvData)
					{
						response->MarshalVariant(pvData[k]);
					}
					else
					{
						response->MarshalVariant(emptyVal);
					}

					k++;
				}
			}

			if (pvData != NULL)
			{
				for (k = 0; k < propCnt; k++)
				{
					if (pvData[k] != NULL)
					{
						delete pvData[k];
						pvData[k] = NULL;
					}
				}

				free(pvData);
				pvData = NULL;
			}

			if (pErrors != NULL)
			{
				free(pErrors);
				pErrors =  NULL;
			}

			element.release();
			listPD.removeAll();
		}
		else
		{
			response->MarshalLong((u_long)OPC_E_UNKNOWNITEMID);               // item result
			res = S_FALSE;
		}
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	tpCon->reply(pCall);

	if (pPropertyIDs)
	{
		free(pPropertyIDs);
	}

	SOSRV_END_TP_FUNCTION
} // getProperties



void SODaSTPServer::addGroup(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_ADDGROUP, TPDA::Server::AddGroup)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD groupClientHandle;
	SOCmnString grpName;
	BYTE active;
	BYTE enable;
	DWORD updateRate;
	DWORD keepAliveTime;
	FLOAT deadBand;
	BYTE valTimeBias;
	LONG timeBias;
	BYTE valLCID;
	DWORD lcid;
	DWORD revUpdateRate;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&groupClientHandle);
	ok &= request->UnMarshalString(&grpName);
	ok &= request->UnMarshalChar(&active);                                // active
	ok &= request->UnMarshalChar(&enable);                                // enable
	ok &= request->UnMarshalLong(&updateRate);                            // update rate
	ok &= request->UnMarshalLong(&keepAliveTime);                         // keep alive time
	ok &= request->UnMarshalFloat(&deadBand);                             // deadband
	ok &= request->UnMarshalChar(&valTimeBias);
	ok &= request->UnMarshalLong(&timeBias);
	ok &= request->UnMarshalChar(&valLCID);
	ok &= request->UnMarshalLong(&lcid);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	if (!((deadBand >= 0.0) && (deadBand <= 100.0)))
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	SODASTPSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASTPSERVER_WAIT_FOR_IO_STARTUP
	SOCmnPointer<SODaSGroup> grp;
	LONG* pTimeBias = NULL;

	if (valTimeBias)
	{
		pTimeBias = &timeBias;
	}

	if (!valLCID)
	{
		lcid = 0;
	}

	// add group
	res = server->addGroup(grpName, (active != 0), updateRate, groupClientHandle, pTimeBias, &deadBand, lcid, (SODaSGroup**)grp, &revUpdateRate);
#ifdef SOFEATURE_DCOM /* TODO: portable? */

	if (SUCCEEDED(res))
	{
		// add advise
		res = grp->adviseDataCallback(SODASDATACALLBACK_CT_TUNNEL, NULL, 0, NULL);

		if (FAILED(res))
		{
			server->removeGroup(grp->getServerHandle());
		}
	}

#endif
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_ADDGROUP);        // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags

	if (SUCCEEDED(res))
	{
		// keep alive time
#ifdef SOFEATURE_DCOM /* TODO: portable? */
		grp->setKeepAliveTime(keepAliveTime);
		DWORD revKeepAliveTime = grp->getKeepAliveTime();

		if ((res == S_OK) && (revKeepAliveTime != keepAliveTime))
		{
			res = OPC_S_UNSUPPORTEDRATE;
		}

#endif

		// enable
		if (!enable)
		{
			grp->setEnable(FALSE);
		}

		response->MarshalLong(res);                                 // result
		response->MarshalLong(grp->getServerHandle());
		response->MarshalString(grp->getName());
		response->MarshalLong(grp->getUpdateRate());
#ifdef SOFEATURE_DCOM /* portable? */
		response->MarshalLong(grp->getKeepAliveTime());
		response->MarshalLong(grp->getTimeBias());
#else
		response->MarshalLong(0);
		response->MarshalLong(timezone / 60);
#endif
		response->MarshalLong(grp->getLCID());
	}
	else
	{
		response->MarshalLong(res);    // result
	}

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
} // addGroup


void SODaSTPServer::removeGroup(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_REMOVEGROUP, TPDA::Server::RemoveGroup)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD groupHandle;
	BYTE forced;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&groupHandle);
	ok &= request->UnMarshalChar(&forced);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}

	// unadvise callback
	SOCmnPointer<SODaSGroup> group;
	group = server->getGroupByHandle(groupHandle);

	if (!group)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

#ifdef SOFEATURE_DCOM /* TODO: portable? */
	group->unadviseDataCallback(SODASDATACALLBACK_CT_TUNNEL, 1);
#endif
	group.release();
	// remove group
	res = server->removeGroup(groupHandle);
	// build response
	tpCon->sendReply(pCall, TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_REMOVEGROUP, res);
	SOSRV_END_TP_FUNCTION
} // removeGroup

void SODaSTPServer::setClientName(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_SETCLIENTNAME, TPDA::Server::SetClientName)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	SOCmnString clientName;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&clientName);

	if (ok && !clientName.isEmpty())
	{
		res = (server->setClientName(clientName)) ? S_OK : E_FAIL;
	}   //  end if

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_SETCLIENTNAME);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                  // result
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
} // setClientName


void SODaSTPServer::write(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_WRITE, TPDA::Server::Write)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD count;
	HRESULT* pErrors;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&count);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 4));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_WRITE);           // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(S_OK);                                  // pro forma result
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;
	// create write transaction
	tac = creator->createTransaction(NULL, SODaSTransaction::serverWrite, SODaSTransaction::synchronous, 0);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(count);
	}
	pErrors = new HRESULT[count];
	// for every item
	SOCmnString itemID;
	SOCmnVariant value;
	BYTE qualitySpecified;
	WORD quality = OPC_QUALITY_GOOD;
	BYTE timestampSpecified;
	SOCmnDateTime timestamp;
	SOCmnEvents events;
	DWORD reqEventId = events.create();

	for (i = 0; ((ok) && (i < count)); i++)
	{
		SOCmnPointer<SODaSItemTag> itemTag;
		ok &= request->UnMarshalString(&itemID);
		ok &= request->UnMarshalVariant(&value);
		ok &= request->UnMarshalChar(&qualitySpecified);

		if ((ok) && (qualitySpecified))
		{
			ok &= request->UnMarshalShort(&quality);
		}

		ok &= request->UnMarshalChar(&timestampSpecified);

		if ((ok) && (timestampSpecified))
		{
			ok &= request->UnMarshalDate(&timestamp);
		}

		if (!ok)
		{
			break;
		}

		// get item tag object
		pErrors[i] = server->getItemTag(itemID, &itemTag);

		if (SUCCEEDED(pErrors[i]))
		{
			// valid item id
			// check access rights
			if ((itemTag->getAccessRights(server) & OPC_WRITEABLE) == OPC_WRITEABLE)
			{
				SOCmnPointer<SODaSRequest> req;
				req = creator->createRequest(server, NULL, itemTag, SODaSRequest::write, SODaSRequest::serverDevice);

				if (req.isNotNull())
				{
					req->setItemTag(itemTag);
					req->setServer(server);
					req->setResultAddress(pErrors + i);
					req->setResult(S_OK);
					BYTE writeQT = 0;

					if (qualitySpecified)
					{
						writeQT |= SODASREQUEST_FLAG_QUALITY;
					}

					if (timestampSpecified)
					{
						writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
					}

					if (writeQT == 0)
					{
						if (!req->setWriteValue(value))
						{
							pErrors[i] = req->getResult();
						}
					}
					else
					{
						if (req->setWriteValue(value))
						{
							if (itemTag->supportWriteQT(writeQT))
							{
								if (qualitySpecified)
								{
									req->setWriteQuality(quality);
								}

								if (timestampSpecified)
								{
									req->setWriteTimeStamp(timestamp);
								}
							}
							else
							{
								pErrors[i] = OPC_E_NOTSUPPORTED;
							}
						}
						else
						{
							pErrors[i] = req->getResult();
						}
					}

					if (SUCCEEDED(pErrors[i]))
					{
						req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
						tac->insertRequest(req);
					}
				}
			}
			else
			{
				pErrors[i] = OPC_E_BADRIGHTS;
			}
		}

		if (FAILED(pErrors[i]))
		{
			// set failed values for quality and timestamp
			res = S_FALSE;

			if (itemTag.isNotNull())
			{
				_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, itemTag, (_T(" [%3.3ld] [0x%lX]"), i, pErrors[i]));
			}
			else
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T(" [%3.3ld] [0x%lX]"), i, pErrors[i]));
			}
		}
		else
		{
			_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)
			{
				SOCmnString trcHelpSZ;
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, itemTag, (_T(" [%3.3ld] %s"), i, variantWithType2string(&trcHelpSZ, value)));
			}
		}
	}

	// start write operations
	if (ok && SUCCEEDED(res))
	{
		DWORD tacReqCnt = tac->getRequestCount();

		if (tacReqCnt > 0)
		{
			tac->execute(server, &events, reqEventId, server->m_dueTimeWrite);
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();

		for (i = 0; i < count; i++)
		{
			response->MarshalLong(pErrors[i]);

			if (FAILED(pErrors[i]))
			{
				res = S_FALSE;
			}
		}
	}

	if (!ok)
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	tpCon->reply(pCall);
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // write


void SODaSTPServer::read(
	IN SODaSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_DA_SERVER, TP_FUNCTION_DA_SERVER_READ, TPDA::Server::Read)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SODaSServer* server = pServer;
	HRESULT res = S_OK;
	BOOL ok;
	DWORD count;
	SOCmnVariant* pValues;
	WORD* pQualities;
	SOCmnDateTime* pTimestamps;
	HRESULT* pErrors;
	DWORD maxAge;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalArrayLen(&count);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 32));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	response->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	response->MarshalChar(TP_FUNCTION_DA_SERVER_READ);            // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(S_OK);                                  // pro forma result
	pValues = new SOCmnVariant[count];
	pQualities = new WORD[count];
	pTimestamps = new SOCmnDateTime[count];
	pErrors = new HRESULT[count];
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;
	// create read transaction
	tac = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(count);
	}
	// for every item
	SOCmnString itemID;
	SOCmnEvents events;
	DWORD reqEventId = events.create();

	for (i = 0; ((ok) && (i < count)); i++)
	{
		SOCmnPointer<SODaSItemTag> itemTag;
		ok &= request->UnMarshalString(&itemID);
		ok &= request->UnMarshalLong(&maxAge);

		if (!ok)
		{
			break;
		}

		// get item tag object
		pErrors[i] = server->getItemTag(itemID, &itemTag);

		if (SUCCEEDED(pErrors[i]))
		{
			// valid item id
			// check access rights
			if ((itemTag->getAccessRights(server) & OPC_READABLE) == OPC_READABLE)
			{
				SOCmnPointer<SODaSRequest> req;
				req = creator->createRequest(server, NULL, itemTag, SODaSRequest::read, SODaSRequest::serverMaxAge);

				if (req.isNotNull())
				{
					req->setItemTag(itemTag);
					req->setServer(server);
					req->setResultAddress(pErrors + i);
					req->setResult(S_OK);
					req->setMaxAge(maxAge);
					req->setReadValueAddress(pValues + i);
					req->setReadQualityAddress(pQualities + i);
					req->setReadTimeStampAddress(pTimestamps + i);
					req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
					tac->insertRequest(req);
				}
			}
			else
			{
				pErrors[i] = OPC_E_BADRIGHTS;
			}
		}

		if (FAILED(pErrors[i]))
		{
			// set failed values for quality and timestamp
			pQualities[i] = OPC_QUALITY_BAD;
			pTimestamps[i].clear();
			res = S_FALSE;
		}
	}

	if (ok && SUCCEEDED(res))
	{
		// start read operations
		if (tac->getRequestCount() > 0)
		{
			tac->execute(server, &events, reqEventId, server->m_dueTimeRead);
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();

		for (i = 0; i < count; i++)
		{
			response->MarshalLong(pErrors[i]);

			if (SUCCEEDED(pErrors[i]))
			{
				response->MarshalVariant(pValues[i]);
				response->MarshalShort(pQualities[i]);
				response->MarshalDate(pTimestamps[i]);
			}
			else
			{
				res = S_FALSE;
			}
		}
	}

	if (!ok)
	{
		res = RPC_E_INVALID_DATA;
	}

	response->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER));
	response->MarshalLong(res);                                 // final result
	tpCon->reply(pCall);
	delete[] pValues;
	delete[] pQualities;
	delete[] pTimestamps;
	delete[] pErrors;
	SOSRV_END_TP_FUNCTION
} // read

#endif // SOFEATURE_TUNNEL

#endif // SODAS
