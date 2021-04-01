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
//  Filename    : SODaSXmlServer.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : XML DA Server related class                                 |
//                - SODaSXmlServer: XML DA Server class                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_SOAP

#include "opcerror.h"
#include "SODaSXmlServer.h"


//-----------------------------------------------------------------------------
// SODaSXmlServer                                                             |
//-----------------------------------------------------------------------------

SODaSXmlServer::SODaSXmlServer(void)
{
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	m_groupIdToSessionObjectMap.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
}

void SODaSXmlServer::removeClient(IN SOSrvHTTPHandlerData* pHTTPData)
{
	if (pHTTPData->m_object.isNotNull())
	{
		SODaSServer* pServer = (SODaSServer*)(SOCmnObject*)pHTTPData->m_object;
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SOSrvObjectRoot> root;
		{
			SOCmnList<SODaSTransaction> tacList(pServer->getTransactionList());
			tacList.removeAll();
		}
		root = entry->getObjectRoot();
		root->removeBranch(pServer);
		pServer->removeAllAdditionalElements();
		pServer->removeAllLeaves();
		pServer->removeAllBranches();
		pHTTPData->m_object.release();
	}

	SOCmnSingleLock<SOCmnSync> locker(m_groupIdToSessionObjectMap.getSyncObject());

	BOOL removed = m_groupIdToSessionObjectMap.removeObject(pHTTPData);

	while (removed)
	{
		removed = m_groupIdToSessionObjectMap.removeObject(pHTTPData);
	}
}


WORD SODaSXmlServer::handleHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody)
{
	// check for XML DA SOAP request
	if (pHandler->getHTTPMethod() == SOCMNHTTP_METHOD_POST)
	{
		SOCmnStringList attributeList(pHandler->getAttributeList());
		SOCmnString SOAPAction;

		if (attributeList.findKey(_T("soapaction"), &SOAPAction))
		{
			// SOAP request
			SOAPAction.makeLower();

			if (_tcsncmp(SOAPAction, _T("\"http://opcfoundation.org/webservices/xmlda/1.0/"), 48) == 0)
			{
				// OPC Foundation XML DA method
				// check if DA Server object is connected to the client connection
				if (!pHTTPData->m_object)
				{
					// no server object -> create one
					SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
					SOCmnPointer<SOSrvObjectRoot> root;
					SOCmnPointer<SODaSCreator> creator;
					SODaSServer* pServer;
					SOCmnString clientName;
					// create server object and add to object tree
					creator = entry->getCreator();
					pServer = creator->createServer(FALSE, 0);
					DWORD result = pServer->setFlags(SODASSERVER_FLAG_SOAP | SODASSERVER_FLAG_HTTP_RECEIVER);

					if (result != S_OK)
					{
						sndSOAPFault(pHandler, pHTTPData, pHTTPData->m_flags, pServer, (DWORD)E_FAIL);
						return SOCMNHTTP_CODE_FORBIDDEN;
					}

					pServer->setClientName(clientName.format(_T("%lu"), pHTTPData->m_clientId));
					struct sockaddr_in name;
					socklen_t namelen = sizeof(sockaddr_in);

					if (getpeername(pHTTPData->m_socket->m_socket, (sockaddr*)&name, &namelen) != SOCKET_ERROR)
					{
						SOCmnString id;
						DWORD dwAddr = ntohl(name.sin_addr.s_addr);
						pServer->setClientName(clientName.format(_T("%d.%d.%d.%d:%u"), (int)(0xFF & (dwAddr >> 24)), (int)(0xFF & (dwAddr >> 16)), (int)(0xFF & (dwAddr >> 8)), (int)(0xFF & dwAddr), ntohs(name.sin_port)));
						id.format(_T("[%s|%8.8lX|%lu|%s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), pHTTPData->m_clientId, (LPCTSTR)pServer->getClientName());
						pServer->setObjTraceId(id);
					}

					pHTTPData->m_object = pServer;
					pServer->m_httpReceiver.setWithAddRef(this);
					root = entry->getObjectRoot();
					root->addBranch(pServer);
				}

				SOCmnPointer<SODaSServer> server;
				server.setWithAddRef((SODaSServer*)(SOCmnObject*)pHTTPData->m_object);

				if (!server)
				{
					return SOCMNHTTP_CODEI_NOT_HANDLED;
				}

				// insert the actions to the SOAP request handling queue of the DA Server work thread
				SOCmnString SOAPMethod(((LPCTSTR)SOAPAction) + 48);
				DWORD errorCode = 0xFFFFFFFF;

				if (SOAPMethod == _T("subscriptionpolledrefresh\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_SUBSCRIPTIONPOLLEDREFRESH, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("read\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_READ, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("write\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_WRITE, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("browse\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_BROWSE, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("getproperties\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_GETPROPERTIES, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("subscribe\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_SUBSCRIBE, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("subscriptioncancel\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_SUBSCRIPTIONCANCEL, pHandler, pHTTPData, lenBody, pBody);
				}
				else if (SOAPMethod == _T("getstatus\""))
				{
					errorCode = server->insertXMLMethod(SODASXMLMETHOD_GETSTATUS, pHandler, pHTTPData, lenBody, pBody);
				}

				switch (errorCode)
				{
				case S_OK:
					return SOCMNHTTP_CODE_OK;
					break;

				case 0xFFFFFFFF:
					return SOCMNHTTP_CODEI_NOT_HANDLED;
					break;

				default:
					sndSOAPFault(pHandler, pHTTPData, pHTTPData->m_flags, server, errorCode);
					return SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
					break;
				}
			}
		}
	}

	// not handled until now -> let base implementation handle the request
	return SOCMNHTTP_CODEI_NOT_HANDLED;
}


void SODaSXmlServer::sndSOAPFault(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN SODaSServer* pServer, IN DWORD errorCode)
{
	SOCmnStringEx faultHeader;
	SOCmnStringEx faultBody;
	SOCmnString faultCode;
	SOCmnString faultString;
	SOCmnDateTime now;
	SOCmnStringEx szDefResAttrs;
	now.now();
	pServer->getErrorString(errorCode, faultString);
	pServer->getErrorCodeSZ(errorCode, faultCode);
	faultBody.format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\
<soap:Body><soap:Fault>\
<faultcode xmlns:opc=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">opc:%s</faultcode>\
<faultstring>%s</faultstring>\
</soap:Fault></soap:Body></soap:Envelope>"), (LPCTSTR)faultCode, (LPCTSTR)faultString);
	getResponseAttributes(flags, szDefResAttrs);
	faultHeader.format(_T("HTTP/1.1 200 OK\r\nServer: Softing OPC Toolkit\r\nDate: %s\r\nCache-Control: private\r\n%sContent-Type: text/xml; charset=utf-8\r\nContent-Length: %lu\r\n\r\n"), (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, faultBody.getLength());
	pHandler->queueSndData(pSrvData, flags, faultHeader, faultBody);
}


DWORD SODaSXmlServer::handleXmlMethod(
	IN SODaSServer* pServer,
	IN SODaSXmlMethod* pMth,
	IN SOCmnDateTime& /* now */)
{
	HRESULT errorCode = S_OK;

	if (pServer->checkAuthorization(pMth))
	{
		switch (pMth->getMethod())
		{
		case SODASXMLMETHOD_SUBSCRIPTIONPOLLEDREFRESH:
			errorCode = subscriptionPolledRefresh(pServer, pMth);
			break;

		case SODASXMLMETHOD_READ:
			errorCode = read(pServer, pMth);
			break;

		case SODASXMLMETHOD_WRITE:
			errorCode = write(pServer, pMth);
			break;

		case SODASXMLMETHOD_BROWSE:
			errorCode = browse(pServer, pMth);
			break;

		case SODASXMLMETHOD_GETPROPERTIES:
			errorCode = getProperties(pServer, pMth);
			break;

		case SODASXMLMETHOD_SUBSCRIBE:
			errorCode = subscribe(pServer, pMth);
			break;

		case SODASXMLMETHOD_SUBSCRIPTIONCANCEL:
			errorCode = subscriptionCancel(pServer, pMth);
			break;

		case SODASXMLMETHOD_GETSTATUS:
			errorCode = getStatus(pServer, pMth);
			break;

		default:
			errorCode = E_FAIL;
			break;
		}
	}

	if (errorCode != S_OK)
	{
		SOCmnPointer<SOSrvHTTPHandler> httpH = pMth->getHTTPHandler();
		SOCmnPointer<SOSrvHTTPHandlerData> httpD = pMth->getHTTPHandlerData();
		sndSOAPFault(httpH, httpD, pMth->getFlags(), pServer, errorCode);
	}

	return INFINITE;
}


void SODaSXmlServer::setReqLocaleID(IN SODaSServer* pServer, IN SODaSXmlMethod* pXmlMth)
{
	SOCmnStringList xmlOptions(pXmlMth->getMethodOptions());
	SOCmnString reqLocId;
	xmlOptions.findKey(_T("LocaleID"), &reqLocId);

	if (!reqLocId.isEmpty())
	{
		pServer->setReqLocaleID(reqLocId);
	}
}

LPCTSTR SODaSXmlServer::getReplyBase(SOCmnStringEx& dest, LPCTSTR rbName, IN SOCmnDateTime* pNow, IN SODaSServer* pServer, IN SODaSXmlMethod* pXmlMth)
{
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnStringList xmlOptions(pXmlMth->getMethodOptions());
	SOCmnString clientReqHandle;
	BOOL recClientReqHandle;
	SOCmnString reqLocId;
	SOCmnString revLocId;
	BOOL recLocId;
	SOCmnString serverState;
	SOCmnString szCRH;
	SOCmnString szRLI;
	SOCmnStringEx exHlp;
	exHlp.getBuffer(48);
	recClientReqHandle = xmlOptions.findKey(_T("ClientRequestHandle"), &clientReqHandle);
	recLocId = xmlOptions.findKey(_T("LocaleID"), &reqLocId);

	if (recClientReqHandle)
	{
		szCRH.format(_T(" ClientRequestHandle=\"%s\""), (LPCTSTR)clientReqHandle);
	}

//	if (recLocId)
	{
		LCID lcid;
		lcid = pServer->getLCID();
		pServer->convertLCIDtoLocaleID(lcid, revLocId);

		if (_tcsicmp(reqLocId, revLocId) == 0)
		{
			revLocId.empty();
		}
		else
		{
			szRLI.format(_T(" RevisedLocaleID=\"%s\""), (LPCTSTR)revLocId);
		}
	}
	serverState = entry->getServerStateString();
	dest.format(_T("<%s RcvTime=\"%s\" ReplyTime=\"%s\"%s%s ServerState=\"%s\"/>"), rbName, (LPCTSTR)pXmlMth->getRcvTime().getXMLDateTime(), (LPCTSTR)pNow->getXMLDateTime(), (LPCTSTR)szCRH, (LPCTSTR)szRLI, (LPCTSTR)serverState);
	return dest;
}

void SODaSXmlServer::addAbnormalResult(IN SOCmnDWORDList* pAbnormalResultList, IN DWORD propError)
{
	if (!pAbnormalResultList->isInit())
	{
		pAbnormalResultList->create();
	}

	if (pAbnormalResultList->findObject(propError) == NULL)
	{
		pAbnormalResultList->add(propError);
	}
}

void SODaSXmlServer::getErrorTexts(IN SODaSServer* pServer, IN SOCmnDWORDList* pAbnormalResultList, OUT SOCmnStringEx& resErrors)
{
	if (pAbnormalResultList->isInit())
	{
		resErrors.getBuffer(pAbnormalResultList->getCount() * 192);
		SOCmnStringEx errorTxt;
		SOCmnString errCode, errTxt;
		SOCmnListPosition errorPos;
		SOCmnStringEx exHlp;
		exHlp.getBuffer(128);
		DWORD error;
		errorTxt.getBuffer(64);
		errorPos = pAbnormalResultList->getStartPosition();

		while (errorPos)
		{
			error = pAbnormalResultList->getNext(errorPos);
			pServer->getErrorCodeSZ((DWORD)error, errCode);
			pServer->getErrorString((HRESULT)error, errTxt);
			errorTxt.format(_T("<Errors ID=\"%s\"><Text>%s</Text></Errors>"), (LPCTSTR)errCode, (LPCTSTR)SOCmnStringEx::getXMLString(errTxt, exHlp));
			resErrors += errorTxt;
		}

		pAbnormalResultList->destroy();
	}
}

void SODaSXmlServer::sendResponse(
	IN SODaSServer* /* pServer */,
	IN SODaSXmlMethod* mth,
	SOCmnDateTime* pNow,
	SOCmnStringEx* pResHeader,
	SOCmnStringEx* pResBody)
{
	SOCmnStringEx szDefResAttrs;
	BYTE* pBody = NULL;
	DWORD lenBody = 0;

	if (pResBody)
	{
		pBody = pResBody->cloneToUTF8(&lenBody);
	}

	getResponseAttributes(mth->getFlags(), szDefResAttrs);
	pResHeader->getBuffer(256);
	pResHeader->format(_T("HTTP/1.1 200 OK\r\nServer: Softing OPC Toolkit\r\nDate: %s\r\nCache-Control: private, max-age=0\r\n%sContent-Type: text/xml; charset=utf-8\r\nContent-Length: %lu\r\n\r\n"), (LPCTSTR)pNow->getHTTPDate(), (LPCTSTR)szDefResAttrs, lenBody);
	SOCmnPointer<SOSrvHTTPHandler> httpH = mth->getHTTPHandler();
	SOCmnPointer<SOSrvHTTPHandlerData> httpD = mth->getHTTPHandlerData();
	httpH->queueSndData(httpD, mth->getFlags(), *pResHeader, lenBody, pBody);

	if (pBody)
	{
		freeMemory(pBody);
	}
}

void SODaSXmlServer::getXMLStart(IN SODaSServer* pServer, IN BYTE methodID, OUT SOCmnStringEx& resBody)
{
	SOCmnString vendorSpecificNSE, vendorSpecificNSP, vendorSpecificURL;

	if (pServer->getVendorXMLNamespace(SODASSERVER_XMLNS_ERROR, vendorSpecificURL))
	{
		vendorSpecificNSE.format(_T(" xmlns:vse=\"%s\""), (LPCTSTR)vendorSpecificURL);
		vendorSpecificURL.empty();
	}

	if ((methodID == SODASXMLMETHOD_GETPROPERTIES) || (methodID == SODASXMLMETHOD_BROWSE))
	{
		if (pServer->getVendorXMLNamespace(SODASSERVER_XMLNS_PROPERTY, vendorSpecificURL))
		{
			vendorSpecificNSP.format(_T(" xmlns:vsp=\"%s\""), (LPCTSTR)vendorSpecificURL);
			vendorSpecificURL.empty();
		}
	}

	resBody.format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"%s%s>"), (LPCTSTR)vendorSpecificNSE, (LPCTSTR)vendorSpecificNSP);
}

HRESULT SODaSXmlServer::getStatus(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(GetStatus)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_GETSTATUS, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|GetStatus"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnDateTime srvStartTime;
		WORD majorVersion;
		WORD minorVersion;
		WORD patchVersion;
		WORD buildNumber;
		SOCmnString szVendorInfo;
		SOCmnStringEx xmlLocalIds;
		SOCmnString xmlLocalId;
		SOCmnString szRevLocId;
		SOCmnString szLocId;
		LCID lcid;
		SOCmnDWORDList lcidList;
		SOCmnListPosition pos;
		entry->getStartTime(&srvStartTime);
		szVendorInfo = entry->getVendorInfo();
		entry->getVersionInfo(&majorVersion, &minorVersion, &patchVersion, &buildNumber);
		lcid = pServer->getLCID();
		pServer->convertLCIDtoLocaleID(lcid, szRevLocId);
		lcidList.create();
		pServer->queryAvailableLCIDs(lcidList);
		pos = lcidList.getStartPosition();
		xmlLocalIds.getBuffer(lcidList.getCount() * 50);

		while (pos)
		{
			lcid = (LCID)lcidList.getNext(pos);

			if ((lcid == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT)) ||
				(lcid == MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT)) ||
				(lcid == MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT)))
			{
				continue;
			}

			pServer->convertLCIDtoLocaleID(lcid, szLocId);
			xmlLocalId.format(_T("<SupportedLocaleIDs>%s</SupportedLocaleIDs>"), (LPCTSTR)szLocId);
			xmlLocalIds += xmlLocalId;
		}

		// build response
		SOCmnDateTime now;
		SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
		SOCmnStringEx exHlp;
		exHlp.getBuffer(128);
		pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
		now.now();
		pResBody->getBuffer(1200);
		pReplyBase->getBuffer(256);
		getReplyBase(*pReplyBase, _T("GetStatusResult"), &now, pServer, mth);
		getXMLStart(pServer, SODASXMLMETHOD_STATE_GETSTATUS, *pResHeader); // uses only the buffer, is not the header
		pResBody->format(_T("%s<soap:Body><GetStatusResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">\
%s\
<Status StartTime=\"%s\" ProductVersion=\"%u.%2.2u.%u.%4.4u\">\
<VendorInfo>%s</VendorInfo>%s\
<SupportedInterfaceVersions>XML_DA_Version_1_0</SupportedInterfaceVersions>\
</Status>\
</GetStatusResponse></soap:Body></soap:Envelope>"),
						 (LPCTSTR)*pResHeader, (LPCTSTR)*pReplyBase, (LPCTSTR)srvStartTime.getXMLDateTime(), majorVersion, minorVersion, patchVersion, buildNumber, (LPCTSTR)SOCmnStringEx::getXMLString(szVendorInfo, exHlp), (LPCTSTR)xmlLocalIds);
		// send response
		sendResponse(pServer, mth, &now, pResHeader, pResBody);
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_SOAP_FUNCTION
} // getStatus



HRESULT SODaSXmlServer::browse(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(Browse)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_BROWSE, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Browse"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP
		SODASXML_TRACE_INPUT(pServer, mth)
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SOCmnListPosition pos;
		SODaSXmlMethodData* pMethodData;
		SOCmnString itemPath;
		SOCmnString itemName;
		SOCmnString itemId;
		SOCmnPointer<SOCmnElement> element;
		SOSrvBrowser::browserFilterType fType = SOSrvBrowser::typeAll;
		SOCmnList<SODaSBrowseElementData> listBE;
		SOCmnString contPoint;
		DWORD maxElementsReturned = 0;
		SOCmnString szHlp;
		SOCmnString szBrowseFilter;
		SOCmnString elementNameFilter;
		SOCmnString vendorFilter;
		BOOL returnAllProperties = FALSE;
		BOOL returnPropertyValues = FALSE;
		BOOL returnErrorText = FALSE;
		DWORD propertyCount = 0;
		DWORD* pPropertyIDs = NULL;
		BOOL moreElements;
		DWORD elCnt;
		DWORD i, k;
		BOOL doGetProps = TRUE;
		DWORD propCnt = 0;      // default for return all
		DWORD* pPropIDs = NULL; // default for return all
		SOCmnStringList inPropNamesList;
		SOCmnStringEx exHlp, exHlp2;
		exHlp.getBuffer(128);
		exHlp2.getBuffer(128);
		// get input attributes
		xmlOptions.findKey(_T("ItemPath"), &itemPath);
		xmlOptions.findKey(_T("ItemName"), &itemName);

		if (FAILED(res = pServer->getItemID(itemPath, itemName, itemId)))
		{
			SOSRV_RETURN_SOAP_FUNCTION_RES(res)
		}

		xmlOptions.findKey(_T("ContinuationPoint"), &contPoint);

		if (xmlOptions.findKey(_T("MaxElementsReturned"), &szHlp))
		{
			maxElementsReturned = _tcstoul(szHlp, NULL, 10);
		}

		xmlOptions.findKey(_T("BrowseFilter"), &szBrowseFilter);
		xmlOptions.findKey(_T("ElementNameFilter"), &elementNameFilter);
		xmlOptions.findKey(_T("VendorFilter"), &vendorFilter);

		if (xmlOptions.findKey(_T("ReturnAllProperties"), &szHlp))
		{
			returnAllProperties = XMLSTR2BOOL(szHlp);
		}

		if (xmlOptions.findKey(_T("ReturnPropertyValues"), &szHlp))
		{
			returnPropertyValues = XMLSTR2BOOL(szHlp);
		}

		if (xmlOptions.findKey(_T("ReturnErrorText"), &szHlp))
		{
			returnErrorText = XMLSTR2BOOL(szHlp);
		}

		if (returnPropertyValues)
			SODASXMLSERVER_WAIT_FOR_IO_STARTUP
			if (!returnAllProperties)
			{
				// check property count
				propertyCount = xmlData.getCount();

				if (propertyCount)
				{
					inPropNamesList.create();
					pPropertyIDs = (DWORD*)malloc(propertyCount * sizeof(DWORD));
					pos = xmlData.getStartPosition();
					i = 0;

					while (pos)
					{
						pMethodData = xmlData.getNext(pos);
						pPropertyIDs[i] = pServer->getPropertyIDByName(pMethodData->m_propertyName);
						inPropNamesList.add(pMethodData->m_propertyName);
						i++;
					}

					propCnt = propertyCount;
					pPropIDs = pPropertyIDs;
				}
				else
				{
					doGetProps = FALSE;
				}
			}

		// correct max elements count for unlimited elements
		if (maxElementsReturned == 0)
		{
			maxElementsReturned = 0xFFFFFFFF;
		}

		listBE.create((SOCmnListHeader::isRehashingEnabled() ? SOCMNLIST_TYPE_ORG_MAP : SOCMNLIST_TYPE_ORG_LIST) | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

		// check for valid item id
		if (itemId.getLength() != 0)
		{
			if (!pServer->isNameSpaceElement(itemId, &element))
			{
				if (pPropertyIDs)
				{
					free(pPropertyIDs);
				}

				SOSRV_RETURN_SOAP_FUNCTION_RES((pServer->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID)
			}
		}
		else
		{
			// no id take root
			SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
			element = entry->getNameSpaceRoot(0);
		}

		// set filter
		if (_tcsicmp(szBrowseFilter, _T("all")) == 0)
		{
			fType = SOSrvBrowser::typeAll;
		}
		else if (_tcsicmp(szBrowseFilter, _T("branch")) == 0)
		{
			fType = SOSrvBrowser::typeBranch;
		}
		else if (_tcsicmp(szBrowseFilter, _T("item")) == 0)
		{
			fType = SOSrvBrowser::typeItem;
		}

		// get the browse elements
		res = pServer->browse(itemId, element, fType, elementNameFilter, vendorFilter, maxElementsReturned, contPoint, moreElements, &listBE);

		if (SUCCEEDED(res))
		{
			// build response
			SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
			SOCmnDWORDList abnormalResultList;
			SOCmnStringEx resErrors;
			SOCmnStringEx resAttributes;
			SOCmnDateTime now;
			DWORD estBodySize;
			DWORD estElSize;
			pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
			now.now();
			pReplyBase->getBuffer(256);
			getReplyBase(*pReplyBase, _T("BrowseResult"), &now, pServer, mth);

			if (contPoint.isEmpty())
			{
				resAttributes.format(_T("MoreElements=\"%s\""), BOOL2XMLSTR(moreElements));
			}
			else
			{
				resAttributes.format(_T("ContinuationPoint=\"%s\" MoreElements=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(contPoint, exHlp), BOOL2XMLSTR(moreElements));
			}

			elCnt = listBE.getCount();
			estElSize = 128 + (doGetProps * (((propertyCount != 0) ? propertyCount : 6) * (96 + (returnPropertyValues * 64))));
			estBodySize = 512 + (elCnt * estElSize);
			pResBody->getBuffer(estBodySize);
			getXMLStart(pServer, SODASXMLMETHOD_STATE_BROWSE, *pResHeader); // uses only the buffer, is not the header
			pResBody->format(_T("%s<soap:Body><BrowseResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\" %s>\
%s"), (LPCTSTR)*pResHeader, (LPCTSTR)resAttributes, (LPCTSTR)*pReplyBase);

			if (elCnt > 0)
			{
				SODaSBrowseElementData* pBE;
				SOCmnListPosition posBE;
				SOCmnList<SODaSPropertyData> listPD;
				SOCmnListPosition posPD;
				SODaSPropertyData* pPD;
				SOCmnStringEx szElement;
				SOCmnStringEx szProperty;
				DWORD resItemProperties;
				SOCmnStringEx szItemName;
				SOCmnStringEx szDescription;
				SOCmnStringEx szPropError;
				SOCmnString convHlp;
				DWORD propError;
				SOCmnStringEx szT, szV, szTAtts;
				szElement.getBuffer(estElSize);
				szItemName.getBuffer(128);
				szPropError.getBuffer(64);
				szDescription.getBuffer(64);
				szT.getBuffer(64);
				szV.getBuffer(128);
				listPD.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
				posBE = listBE.getStartPosition();
				i = 0;

				while (posBE)
				{
					pBE = listBE.getNext(posBE);
					szElement.format(_T("<Elements Name=\"%s\" ItemPath=\"\" ItemName=\"%s\" IsItem=\"%s\" HasChildren=\"%s\">"), SOCmnStringEx::getXMLString((LPCTSTR)pBE->m_name, exHlp), (LPCTSTR)SOCmnStringEx::getXMLString(pBE->m_itemID, exHlp2), BOOL2XMLSTR(pBE->m_flags & OPC_BROWSE_ISITEM), BOOL2XMLSTR(pBE->m_flags & OPC_BROWSE_HASCHILDREN));

					if (doGetProps)
					{
						// get the properties of the namespace element
						resItemProperties = pServer->getProperties(pBE->m_itemID, pBE->m_nsElement, propCnt, pPropIDs, TRUE, returnPropertyValues, &listPD);

						if (SUCCEEDED(resItemProperties))
						{
							propCnt = listPD.getCount();
							SOCmnVariant** pvData = NULL;
							HRESULT* pErrors = NULL;

							if ((returnPropertyValues) && (propCnt > 0))
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

									SOSRV_RETURN_SOAP_FUNCTION_RES(E_OUTOFMEMORY);
								}

								for (k = 0; k < propCnt; k++)
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
									objBE = pServer->getNameSpaceElement(pBE->m_itemID);
								}

								if (objBE.isNotNull())
								{
									accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(pServer);
									pServer->readProperties(pBE->m_itemID, objBE, accessRights, &listPD, 0, NULL, TRUE, NULL, NULL, pvData, pErrors);
								}
								else
								{
									// browse object but can not create it !
									if (pPropertyIDs)
									{
										free(pPropertyIDs);
									}

									SOSRV_RETURN_SOAP_FUNCTION_RES(E_FAIL);
								}
							}

							posPD = listPD.getStartPosition();
							k = 0;

							while (posPD)
							{
								pPD = listPD.getNext(posPD);

								if (pPD->m_name.isEmpty())
									if (inPropNamesList.isInit())
									{
										pPD->m_name = inPropNamesList.getIndex(k);
									}

								if (!pPD->m_itemID.isEmpty())
								{
									szItemName.format(_T(" ItemPath=\"\" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_itemID, exHlp));
								}
								else
								{
									szItemName.empty();
								}

								if (!pPD->m_descr.isEmpty())
								{
									szDescription.format(_T(" Description=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_descr, exHlp));
								}
								else
								{
									szDescription.empty();
								}

								// element errors
								szPropError.empty();
								propError = S_OK;

								if (FAILED(pPD->m_result)) // invalid pid
								{
									propError = pPD->m_result;
								}
								else if (pErrors)
								{
									if (FAILED(pErrors[k]))  // read error
									{
										propError = pErrors[k];
									}
								}

								if (propError != S_OK)
								{
									pServer->getErrorCodeSZ((DWORD)propError, convHlp);
									szPropError.format(_T(" ResultID=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(convHlp, exHlp));

									if (returnErrorText)
									{
										addAbnormalResult(&abnormalResultList, propError);
									}
								}

								if ((!returnPropertyValues) || (propError != S_OK))
								{
									szProperty.format(_T("<Properties Name=\"%s\"%s%s%s/>"), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_name, exHlp), (LPCTSTR)szDescription, (LPCTSTR)szItemName, (LPCTSTR)szPropError);
								}
								else
								{
									// read value and no error -> add value to output
									SOCmnVariant::getXMLValue(pvData[k], szT, szV);
									szTAtts.empty();

									if (pPD->m_pid == 2501)
									{
										szT = _T("xsd:QName");
									}
									else if (pPD->m_pid == 2503)
									{
										szT = _T("OPCQuality");
										szTAtts = szV;
										szTAtts.replace(_T("&quot;"), _T("\""));
										szV.empty();
									}

									szProperty.getBuffer(256);
									szProperty.format(_T("<Properties Name=\"%s\"%s%s%s><Value xsi:type=\"%s\"%s>%s</Value></Properties>"), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_name, exHlp), (LPCTSTR)szDescription, (LPCTSTR)szItemName, (LPCTSTR)szPropError, (LPCTSTR)szT, (LPCTSTR)szTAtts, (LPCTSTR)szV);
								}

								szElement += szProperty;

								if (pvData)
								{
									delete pvData[k];
								}

								k++;
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
							// general fault e.g. out of memory
							if (pPropertyIDs)
							{
								free(pPropertyIDs);
							}

							SOSRV_RETURN_SOAP_FUNCTION_RES(resItemProperties);
						}
					}

					listPD.removeAll();
					i++;
					szElement += _T("</Elements>");
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szElement));
					*pResBody += szElement;
				}

				if (returnErrorText)
				{
					getErrorTexts(pServer, &abnormalResultList, resErrors);

					if (!resErrors.isEmpty())
					{
						*pResBody += resErrors;
					}
				}
			}

			*pResBody += _T("</BrowseResponse></soap:Body></soap:Envelope>");
			// send response
			sendResponse(pServer, mth, &now, pResHeader, pResBody);
		}

		free(pPropertyIDs);
	}
	else
	{
		res = E_FAIL;    // parse error
	}

	SOSRV_END_SOAP_FUNCTION
}

HRESULT SODaSXmlServer::getProperties(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(GetProperties)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_GETPROPERTIES, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|GetProperties"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SOCmnListPosition pos;
		SODaSXmlMethodData* pMethodData;
		DWORD propertyCount = 0;
		DWORD* pPropertyIDs = NULL;
		SOCmnString itemId;
		SOCmnPointer<SOCmnElement> element;
		SOCmnList<SODaSPropertyData> listPD;
		SOCmnListPosition posPD;
		SODaSPropertyData* pPD;
		DWORD k;
		BOOL returnAllProperties = FALSE;
		BOOL returnPropertyValues = FALSE;
		BOOL returnErrorText = FALSE;
		SOCmnString convHlp;
		SOCmnStringList inPropNamesList;
		SOCmnStringEx exHlp, exHlp2;
		exHlp.getBuffer(128);
		exHlp2.getBuffer(128);

		if (xmlOptions.findKey(_T("ReturnAllProperties"), &convHlp))
		{
			returnAllProperties = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnPropertyValues"), &convHlp))
		{
			returnPropertyValues = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnErrorText"), &convHlp))
		{
			returnErrorText = XMLSTR2BOOL(convHlp);
		}

		if (returnPropertyValues)
			SODASXMLSERVER_WAIT_FOR_IO_STARTUP
			if (!returnAllProperties)
			{
				// get property ids
				inPropNamesList.create();
				pos = xmlData.getStartPosition();

				while (pos)
				{
					pMethodData = xmlData.getNext(pos);

					if (pMethodData->m_type == SODASXMLMETHODDATA_TYPE_PROPERTYNAME)
					{
						if ((propertyCount % 16) == 0)
						{
							pPropertyIDs = (DWORD*)realloc(pPropertyIDs, sizeof(DWORD) * 16);

							if (!pPropertyIDs)
							{
								SOSRV_RETURN_SOAP_FUNCTION_RES(E_OUTOFMEMORY);
							}
						}

						pPropertyIDs[propertyCount] = pServer->getPropertyIDByName(pMethodData->m_propertyName);
						inPropNamesList.add(pMethodData->m_propertyName);
						propertyCount++;
					}
				}

				if (propertyCount == 0)
				{
					SOSRV_RETURN_SOAP_FUNCTION_RES(E_FAIL);
				}
			}

		listPD.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
		SOCmnStringEx szItemProperties;
		SOCmnStringEx szProperty;
		SOCmnStringEx szItemName;
		SOCmnStringEx szPropError;
		SOCmnStringEx szDescription;
		DWORD propError;
		SOCmnDateTime now;
		HRESULT itemRes;
		SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
		SOCmnDWORDList abnormalResultList;
		SOCmnStringEx resErrors;
		SOCmnStringEx szT, szV, szTAtts;
		pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
		DWORD estPropSize, estBodySize;
		now.now();
		pReplyBase->getBuffer(256);
		getReplyBase(*pReplyBase, _T("GetPropertiesResult"), &now, pServer, mth);
		estPropSize = (((propertyCount != 0) ? propertyCount : 6) * (96 + (returnPropertyValues * 64)));
		estBodySize = 512 + ((xmlData.getCount() - propertyCount) * estPropSize);
		pResBody->getBuffer(estBodySize);
		getXMLStart(pServer, SODASXMLMETHOD_STATE_GETPROPERTIES, *pResHeader); // uses only the buffer, is not the header
		pResBody->format(_T("%s<soap:Body><GetPropertiesResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">%s"), (LPCTSTR)*pResHeader, (LPCTSTR)*pReplyBase);
		szItemProperties.getBuffer(estPropSize);
		szProperty.getBuffer(256);
		szItemName.getBuffer(128);
		szPropError.getBuffer(64);
		szDescription.getBuffer(64);
		szT.getBuffer(64);
		szV.getBuffer(128);
		pos = xmlData.getStartPosition();

		while (pos)
		{
			pMethodData = xmlData.getNext(pos);

			if (pMethodData->m_type != SODASXMLMETHODDATA_TYPE_ITEM)
			{
				continue;
			}

			// check for vaild item id
			itemRes = pServer->getItemID(pMethodData->m_itemPath, pMethodData->m_itemName, itemId);

			if (SUCCEEDED(itemRes))
			{
				element.release();

				if (pServer->isNameSpaceElement(itemId, &element))
				{
					itemRes = pServer->getProperties(itemId, element, propertyCount, pPropertyIDs, TRUE, returnPropertyValues, &listPD);

					if (SUCCEEDED(itemRes))
					{
						szItemProperties.format(_T("<PropertyLists ItemPath=\"\" ItemName=\"%s\">"), (LPCTSTR)SOCmnStringEx::getXMLString(itemId, exHlp));
						DWORD propCnt = listPD.getCount();
						SOCmnVariant** pvData = NULL;
						HRESULT* pErrors = NULL;

						if ((returnPropertyValues) && (propCnt > 0))
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

								SOSRV_RETURN_SOAP_FUNCTION_RES(E_OUTOFMEMORY);
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
								objBE = pServer->getNameSpaceElement(itemId);
							}

							if (objBE.isNotNull())
							{
								accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(pServer);
								pServer->readProperties(itemId, objBE, accessRights, &listPD, 0, NULL, TRUE, NULL, NULL, pvData, pErrors);
							}
							else
							{
								// browse object but can not create it !
								itemRes = OPC_E_UNKNOWNITEMID;
							}
						}

						if (SUCCEEDED(itemRes))
						{
							posPD = listPD.getStartPosition();
							k = 0;

							while (posPD)
							{
								pPD = listPD.getNext(posPD);

								if (pPD->m_name.isEmpty())
									if (inPropNamesList.isInit())
									{
										pPD->m_name = inPropNamesList.getIndex(k);
									}

								if (!pPD->m_itemID.isEmpty())
								{
									szItemName.format(_T(" ItemPath=\"\" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_itemID, exHlp));
								}
								else
								{
									szItemName.empty();
								}

								if (!pPD->m_descr.isEmpty())
								{
									szDescription.format(_T(" Description=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_descr, exHlp));
								}
								else
								{
									szDescription.empty();
								}

								szPropError.empty();
								propError = S_OK;

								if (FAILED(pPD->m_result)) // invalid pid
								{
									propError = pPD->m_result;
								}
								else if (pErrors)
								{
									if (FAILED(pErrors[k]))  // read error
									{
										propError = pErrors[k];
									}
								}

								if (propError != S_OK)
								{
									pServer->getErrorCodeSZ((DWORD)propError, convHlp);
									szPropError.format(_T(" ResultID=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(convHlp, exHlp));

									if (returnErrorText)
									{
										addAbnormalResult(&abnormalResultList, propError);
									}
								}

								if ((!returnPropertyValues) || (propError != S_OK))
								{
									szProperty.format(_T("<Properties Name=\"%s\"%s%s%s/>"), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_name, exHlp), (LPCTSTR)szDescription, (LPCTSTR)szItemName, (LPCTSTR)szPropError);
								}
								else
								{
									// read value and no error -> add value to output
									SOCmnVariant::getXMLValue(pvData[k], szT, szV);
									szTAtts.empty();

									if (pPD->m_pid == 2501)
									{
										szT = _T("xsd:QName");
									}
									else if (pPD->m_pid == 2503)
									{
										szT = _T("OPCQuality");
										szTAtts = szV;
										szTAtts.replace(_T("&quot;"), _T("\""));
										szV.empty();
									}

									szProperty.getBuffer(256);
									szProperty.format(_T("<Properties Name=\"%s\"%s%s%s><Value xsi:type=\"%s\"%s>%s</Value></Properties>"), (LPCTSTR)SOCmnStringEx::getXMLString(pPD->m_name, exHlp), (LPCTSTR)szDescription, (LPCTSTR)szItemName, (LPCTSTR)szPropError, (LPCTSTR)szT, (LPCTSTR)szTAtts, (LPCTSTR)szV);
								}

								szItemProperties += szProperty;

								if (pvData)
								{
									delete pvData[k];
								}

								k++;
							}

							if (pvData)
							{
								free(pvData);
							}

							if (pErrors)
							{
								free(pErrors);
							}

							listPD.removeAll();
							szItemProperties += _T("</PropertyLists>");
						}
					}
				}
				else
				{
					// ! isNameSpaceElement
					if (!itemId.isEmpty())
					{
						itemRes = (pServer->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID;
					}
					else
					{
						itemRes = OPC_E_INVALIDITEMID;
					}
				}
			}

			if (FAILED(itemRes))
			{
				// failed to get or read properties
				szItemProperties.format(_T("<PropertyLists ItemName=\"%s\" ResultID=\"%s\"/>"), (LPCTSTR)SOCmnStringEx::getXMLString(itemId, exHlp), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)itemRes, convHlp), exHlp2));

				if (returnErrorText)
				{
					addAbnormalResult(&abnormalResultList, itemRes);
				}
			}

			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szItemProperties));
			*pResBody += szItemProperties;
		}

		if (returnErrorText)
		{
			getErrorTexts(pServer, &abnormalResultList, resErrors);

			if (!resErrors.isEmpty())
			{
				*pResBody += resErrors;
			}
		}

		*pResBody += _T("</GetPropertiesResponse></soap:Body></soap:Envelope>");
		// send response
		sendResponse(pServer, mth, &now, pResHeader, pResBody);
		free(pPropertyIDs);
	}
	else
	{
		res = E_FAIL;    // parse error
	}

	SOSRV_END_SOAP_FUNCTION
}


HRESULT SODaSXmlServer::read(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(Read)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_READ, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Read"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP
		SODASXMLSERVER_WAIT_FOR_IO_STARTUP
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SODaSXmlMethodData* pMethodData;
		DWORD count = xmlData.getCount();
		SOCmnVariant** ppValues;       // read values
		WORD* pQualities;       // read qualities
		SOCmnDateTime** ppTimeStamps; // read timestamps
		HRESULT* pErrors;       // results of each single read operation
		SODaSRequest** ppRequests;
		SOCmnDWORDList abnormalResultList;
		SOCmnStringEx resErrors;
		// request options
		BOOL returnDiagnosticInfo = FALSE;
		BOOL returnItemTime = FALSE;
		BOOL returnErrorText = TRUE;
		BOOL returnItemPath = FALSE;
		BOOL returnItemName = FALSE;
		SOCmnDateTime requestDeadline;
		SOCmnString convHlp;
		VARTYPE itemDatatype, tacDatatype;
		DWORD itemMaxAge, tacMaxAge;
		HRESULT tacError = S_OK;
		SOCmnStringEx exHlp;
		exHlp.getBuffer(128);

		// get request options
		if (xmlOptions.findKey(_T("ReturnDiagnosticInfo"), &convHlp))
		{
			returnDiagnosticInfo = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemTime"), &convHlp))
		{
			returnItemTime = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnErrorText"), &convHlp))
		{
			returnErrorText = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemPath"), &convHlp))
		{
			returnItemPath = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemName"), &convHlp))
		{
			returnItemName = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("RequestDeadline"), &convHlp))
		{
			SOCmnDateTime now;
			ULONGLONG msN, msDL;
			requestDeadline.setXMLDateTime(convHlp);
			now.now();
			msN = now.getMicroseconds();
			msDL = requestDeadline.getMicroseconds();

			if (msDL > msN)
			{
				pServer->setDuetimeRead((DWORD)((msDL - msN) / 1000));
			}
			else
			{
				return OPC_E_TIMEDOUT;
			}
		}

		// get item list options
		if (xmlOptions.findKey(_T("MaxAge"), &convHlp))
		{
			tacMaxAge = _tcstoul((LPCTSTR)convHlp, NULL, 10);
		}
		else
		{
			tacMaxAge = 0;
		}

		if (xmlOptions.findKey(_T("ReqType"), &convHlp))
		{
			if (!SOCmnVariant::getVARTYPE((LPCTSTR)convHlp, tacDatatype))
			{
				tacError = OPC_E_BADTYPE;
			}
		}
		else
		{
			tacDatatype = VT_EMPTY;
		}

		// allocate buffers
		ppValues = (SOCmnVariant**) malloc(count * sizeof(SOCmnVariant*));
		pQualities = (WORD*) malloc(count * sizeof(WORD));
		ppTimeStamps = (SOCmnDateTime**) malloc(count * sizeof(SOCmnDateTime*));
		pErrors = (HRESULT*) malloc(count * sizeof(HRESULT));
		ppRequests = (SODaSRequest**)malloc(count * sizeof(SODaSRequest*));

		if ((ppValues == NULL)  || (pQualities == NULL) || (ppTimeStamps == NULL) || (pErrors == NULL) || (ppRequests == NULL))
		{
			if (ppValues)
			{
				free(ppValues);
				ppValues = NULL;
			}

			if (pQualities)
			{
				free(pQualities);
				pQualities = NULL;
			}

			if (ppTimeStamps)
			{
				free(ppTimeStamps);
				ppTimeStamps = NULL;
			}

			if (pErrors)
			{
				free(pErrors);
				pErrors = NULL;
			}

			if (ppRequests)
			{
				free(ppRequests);
				ppRequests = NULL;
			}

			return E_OUTOFMEMORY;
		}

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
		SOCmnListPosition pos;
		SOCmnEvents events;
		DWORD reqEventId = events.create();
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			SOCmnString itemId;
			SOCmnPointer<SODaSItemTag> itemTag;
			pMethodData = xmlData.getNext(pos);
			ppRequests[i] = NULL;
			ppValues[i] = new SOCmnVariant();
			ppTimeStamps[i] = new SOCmnDateTime();

			if (SUCCEEDED(tacError))
			{
				// get item tag object
				pErrors[i] = pServer->getItemID(pMethodData->m_itemPath, pMethodData->m_itemName, itemId);

				if (SUCCEEDED(pErrors[i]))
				{
					pErrors[i] = pServer->getItemTag(itemId, &itemTag);

					if (SUCCEEDED(pErrors[i]))
					{
						// valid item id
						// check access rights
						if ((itemTag->getAccessRights(pServer) & OPC_READABLE) == OPC_READABLE)
						{
							// get item options
							if (pMethodData->m_maxAge.isEmpty())
							{
								itemMaxAge = tacMaxAge;
							}
							else
							{
								itemMaxAge = _tcstoul((LPCTSTR)pMethodData->m_maxAge, NULL, 10);
							}

							if (pMethodData->m_reqType.isEmpty())
							{
								itemDatatype = tacDatatype;
							}
							else
							{
								if (!SOCmnVariant::getVARTYPE((LPCTSTR)pMethodData->m_reqType, itemDatatype))
								{
									pErrors[i] = OPC_E_BADTYPE;
									i++;
									continue;
								}
							}

							if (!checkXMLDATypeConversion(itemTag->getNativeDatatype(), itemDatatype))
							{
								pErrors[i] = OPC_E_BADTYPE;
								i++;
								continue;
							}

							// create request
							SOCmnPointer<SODaSRequest> req;
							req = creator->createRequest(pServer, NULL, itemTag, SODaSRequest::read, SODaSRequest::serverMaxAge);

							if (req.isNotNull())
							{
								req->setFlag(SODASREQUEST_FLAG_SOAP);
								req->setItemTag(itemTag);
								req->setServer(pServer);
								req->setResultAddress(pErrors + i);
								req->setResult(S_OK);
								req->setMaxAge(itemMaxAge);
								req->setReadValueAddress(ppValues[i]);
								req->setReadQualityAddress(pQualities + i);
								req->setReadTimeStampAddress(ppTimeStamps[i]);
								req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
								req->setReqDatatype(itemDatatype);
								*(ppRequests + i) = (SODaSRequest*)req;
								ppRequests[i]->addRef();
								tac->insertRequest(req);
							}
						}
						else
						{
							pErrors[i] = OPC_E_WRITEONLY;
						}
					}
					else
					{
						if (!itemId.isEmpty())
						{
							pErrors[i] = (pServer->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID;
						}
						else
						{
							pErrors[i] = OPC_E_INVALIDITEMID;
						}
					}
				}
			}
			else
			{
				pErrors[i] = tacError;
			}

			i++;
		}

		// start read operations
		if (tac->getRequestCount() > 0)
		{
			tac->execute(pServer, &events, reqEventId, pServer->m_dueTimeRead);
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();
		// build response
		SOCmnDateTime now;
		SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
		pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
		pReplyBase->getBuffer(256);
		now.now();
		getReplyBase(*pReplyBase, _T("ReadResult"), &now, pServer, mth);
		getXMLStart(pServer, SODASXMLMETHOD_STATE_READ, *pResHeader); // uses only the buffer, is not the header
		pResBody->format(_T("%s<soap:Body><ReadResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">%s<RItemList>"), (LPCTSTR)*pResHeader, (LPCTSTR)*pReplyBase);
		// for every item
		SOCmnStringEx szItem;
		SOCmnStringEx szTimeStamp;
		SOCmnStringEx szValue;
		SOCmnStringEx szQuality;
		SOCmnStringEx szResult;
		SOCmnStringEx szItemName;
		SOCmnStringEx szItemPath;
		SOCmnStringEx szClientItemHandle;
		SOCmnStringEx szDiagnosticInfo;
		SOCmnStringEx szT, szV;
		szItem.getBuffer(256);
		szValue.getBuffer(128);
		szT.getBuffer(64);
		szV.getBuffer(128);
		szResult.getBuffer(64);
		szItemName.getBuffer(128);
		szClientItemHandle.getBuffer(64);
		pResBody->getBuffer(256 + (tac->getRequestCount() * 256));
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			pMethodData = xmlData.getNext(pos);
			szItemName.empty();
			szTimeStamp.empty();
			szValue.empty();
			szQuality.empty();
			szResult.empty();
			szClientItemHandle.empty();
			szItemPath.empty();

			if (SUCCEEDED(pErrors[i]))
			{
				if (returnItemTime)
				{
					szTimeStamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)ppTimeStamps[i]->getXMLDateTime());
				}

				// special XML DA conversion boolean to string
				if (ppRequests[i])
				{
					SOCmnPointer<SODaSItemTag> itemTag = ppRequests[i]->getItemTag();

					if (itemTag.isNotNull())
					{
						doXMLDATypeConversion(itemTag->getNativeDatatype(), ppRequests[i]->getReqDatatype(), *ppValues[i]);
					}
				}

				SOCmnVariant::getXMLValue(ppValues[i], szT, szV);
				szValue.format(_T("<Value xsi:type=\"%s\">%s</Value>"), (LPCTSTR)szT, (LPCTSTR)szV);

				if (pQualities[i] != OPC_QUALITY_GOOD)
				{
					szQuality.getBuffer(128);
					pServer->getXMLQuality(pQualities[i], TRUE, szQuality);
				}
			}
			else
			{
				if (pErrors[i] == (HRESULT)0x80020005) // DISP_E_TYPEMISMATCH
				{
					pErrors[i] = OPC_E_BADTYPE;
				}

				szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)pErrors[i], convHlp), exHlp));

				if (returnErrorText)
				{
					addAbnormalResult(&abnormalResultList, (DWORD)pErrors[i]);
				}
			}

			if (!pMethodData->m_clientItemHandle.isEmpty())
			{
				szClientItemHandle.format(_T(" ClientItemHandle=\"%s\""), 
					SOCmnStringEx::getXMLString(pMethodData->m_clientItemHandle, exHlp));
			}

			if (returnItemPath)
			{
				szItemPath.format(_T(" ItemPath=\"%s\""), (LPCTSTR)pMethodData->m_itemPath);
			}

			if (returnItemName)
			{
				szItemName.format(_T(" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pMethodData->m_itemName, exHlp));
			}

			szItem.format(_T("<Items%s%s%s%s%s>%s%s%s</Items>"), (LPCTSTR)szItemPath, (LPCTSTR)szItemName, (LPCTSTR)szClientItemHandle, (LPCTSTR)szTimeStamp, (LPCTSTR)szResult, (LPCTSTR)szDiagnosticInfo, (LPCTSTR)szValue, (LPCTSTR)szQuality);
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szItem));
			*pResBody += szItem;

			if (ppRequests[i])
			{
				ppRequests[i]->release();
			}

			delete ppValues[i];
			delete ppTimeStamps[i];
			i++;
		}

		free(ppValues);
		free(ppTimeStamps);
		free(pQualities);
		free(pErrors);
		free(ppRequests);
		*pResBody += _T("</RItemList>");

		if (returnErrorText)
		{
			// error texts
			getErrorTexts(pServer, &abnormalResultList, resErrors);

			if (!resErrors.isEmpty())
			{
				*pResBody += resErrors;
			}
		}

		*pResBody += _T("</ReadResponse></soap:Body></soap:Envelope>");
		// send response
		sendResponse(pServer, mth, &now, pResHeader, pResBody);
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_SOAP_FUNCTION
} // read


HRESULT SODaSXmlServer::write(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(Write)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_WRITE, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Write"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP
		SODASXMLSERVER_WAIT_FOR_IO_STARTUP
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SODaSXmlMethodData* pMethodData;
		DWORD count = xmlData.getCount();
		HRESULT* pErrors;
		SODaSRequest** ppRequests;
		VARTYPE* pWriteDT;
		SOCmnDWORDList abnormalResultList;
		SOCmnStringEx resErrors;
		// request options
		BOOL returnDiagnosticInfo = FALSE;
		BOOL returnItemTime = FALSE;
		BOOL returnErrorText = TRUE;
		BOOL returnItemPath = FALSE;
		BOOL returnItemName = FALSE;
		SOCmnDateTime requestDeadline;
		SOCmnString convHlp;
		BOOL returnValuesOnReply = FALSE;
		SOCmnStringEx exHlp;
		exHlp.getBuffer(128);

		// get request options
		if (xmlOptions.findKey(_T("ReturnDiagnosticInfo"), &convHlp))
		{
			returnDiagnosticInfo = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemTime"), &convHlp))
		{
			returnItemTime = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnErrorText"), &convHlp))
		{
			returnErrorText = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemPath"), &convHlp))
		{
			returnItemPath = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemName"), &convHlp))
		{
			returnItemName = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("RequestDeadline"), &convHlp))
		{
			SOCmnDateTime now;
			ULONGLONG msN, msDL;
			requestDeadline.setXMLDateTime(convHlp);
			now.now();
			msN = now.getMicroseconds();
			msDL = requestDeadline.getMicroseconds();

			if (msDL >= msN)
			{
				pServer->setDuetimeWrite((DWORD)((msDL - msN) / 1000));
			}
			else
			{
				return OPC_E_TIMEDOUT;
			}
		}

		if (xmlOptions.findKey(_T("ReturnValuesOnReply"), &convHlp))
		{
			returnValuesOnReply = XMLSTR2BOOL(convHlp);
		}

		// allocate result memory
		pErrors = (HRESULT*)malloc(count * sizeof(HRESULT));
		ppRequests = (SODaSRequest**)malloc(count * sizeof(SODaSRequest*));
		pWriteDT = (VARTYPE*)malloc(count * sizeof(VARTYPE));

		if ((pErrors == NULL) || (ppRequests == NULL) || (pWriteDT == NULL))
		{
			if (pErrors)
			{
				free(pErrors);
			}

			if (ppRequests)
			{
				free(ppRequests);
			}

			if (pWriteDT)
			{
				free(pWriteDT);
			}

			return E_OUTOFMEMORY;
		}

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
		// for every item
		SOCmnEvents events;
		DWORD reqEventId = events.create();
		SOCmnListPosition pos;
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			SOCmnString itemId;
			SOCmnPointer<SODaSItemTag> itemTag;
			SOCmnVariant writeValue;
			pMethodData = xmlData.getNext(pos);
			ppRequests[i] = NULL;
			pWriteDT[i] = VT_EMPTY;
			// get item tag object
			pErrors[i] = pServer->getItemID(pMethodData->m_itemPath, pMethodData->m_itemName, itemId);

			if (SUCCEEDED(pErrors[i]))
			{
				pErrors[i] = pServer->getItemTag(itemId, &itemTag);

				if (SUCCEEDED(pErrors[i]))
				{
					// valid item id
					// check access rights
					if ((itemTag->getAccessRights(pServer) & OPC_WRITEABLE) == OPC_WRITEABLE)
					{
						SOCmnPointer<SODaSRequest> req;
						BOOL initWV;
						req = creator->createRequest(pServer, NULL, itemTag, SODaSRequest::write, SODaSRequest::serverDevice);

						if (req.isNotNull())
						{
							req->setFlag(SODASREQUEST_FLAG_SOAP);
							req->setItemTag(itemTag);
							req->setServer(pServer);
							req->setResultAddress(pErrors + i);
							req->setResult(S_OK);
							initWV = writeValue.initFromXMLValue(&pMethodData->m_value);

							if (initWV)
							{
								if (checkXMLDATypeConversion(itemTag->getNativeDatatype(), writeValue.vt))
								{
									BYTE writeQT = 0;
									pWriteDT[i] = writeValue.vt;

									if ((!pMethodData->m_quality.isEmpty()) || (!pMethodData->m_qualityLimit.isEmpty()) || (!pMethodData->m_qualityVendor.isEmpty()))
									{
										writeQT |= SODASREQUEST_FLAG_QUALITY;
									}

									if (!pMethodData->m_timestamp.isEmpty())
									{
										writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
									}

									if (writeQT == 0)
									{
										if (!req->setWriteValue(&writeValue))
										{
											pErrors[i] = req->getResult();

											if (pErrors[i] != OPC_E_BADTYPE)
											{
												pErrors[i] = OPC_E_RANGE;
											}
										}
									}
									else
									{
										if (req->setWriteValue(&writeValue))
										{
											if (itemTag->supportWriteQT(writeQT))
											{
												if (writeQT & SODASREQUEST_FLAG_QUALITY)
												{
													// write quality
													WORD q;
													q = pServer->getDCOMQuality((LPCTSTR)pMethodData->m_quality, (LPCTSTR)pMethodData->m_qualityLimit, (LPCTSTR)pMethodData->m_qualityVendor);
													req->setWriteQuality(q);
												}

												if (writeQT & SODASREQUEST_FLAG_TIMESTAMP)
												{
													// write timestamp
													SOCmnDateTime ts;
													ts.setXMLDateTime(pMethodData->m_timestamp);
													req->setWriteTimeStamp(ts);
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

											if (pErrors[i] != OPC_E_BADTYPE)
											{
												pErrors[i] = OPC_E_RANGE;
											}
										}
									}

									if (SUCCEEDED(pErrors[i]))
									{
										req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
										*(ppRequests + i) = (SODaSRequest*)req;
										ppRequests[i]->addRef();
										tac->insertRequest(req);
									}
								}
								else
								{
									pErrors[i] = OPC_E_BADTYPE;
								}
							}
							else
							{
								pErrors[i] = OPC_E_BADTYPE;
							}
						}
						else
						{
							pErrors[i] = OPC_E_BADTYPE;
						}
					}
					else
					{
						pErrors[i] = OPC_E_READONLY;
					}
				}
				else
				{
					if (!itemId.isEmpty())
					{
						pErrors[i] = (pServer->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID;
					}
					else
					{
						pErrors[i] = OPC_E_INVALIDITEMID;
					}
				}
			}

			i++;
		}

		// start write operations
		if (SUCCEEDED(res))
		{
			if (tac->getRequestCount() > 0)
			{
				tac->execute(pServer, &events, reqEventId, pServer->m_dueTimeWrite);
			}
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();
		// build response
		SOCmnDateTime now;
		SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
		pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
		now.now();
		getReplyBase(*pReplyBase, _T("WriteResult"), &now, pServer, mth);
		getXMLStart(pServer, SODASXMLMETHOD_STATE_WRITE, *pResHeader); // uses only the buffer, is not the header
		pResBody->format(_T("%s<soap:Body><WriteResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">%s<RItemList>"), (LPCTSTR)*pResHeader, (LPCTSTR)*pReplyBase);
		// for every item
		SOCmnStringEx szItem;
		SOCmnStringEx szTimeStamp;
		SOCmnStringEx szValue;
		SOCmnStringEx szQuality;
		SOCmnStringEx szResult;
		SOCmnStringEx szItemName;
		SOCmnStringEx szItemPath;
		SOCmnStringEx szClientItemHandle;
		SOCmnStringEx szDiagnosticInfo;
		SOCmnStringEx szT, szV;
		SOCmnPointer<SODaSCache> cache;
		SOCmnVariant cacheValue;
		WORD cacheQuality = OPC_QUALITY_GOOD;
		SOCmnDateTime cacheTimestamp;
		szItem.getBuffer(256);
		szValue.getBuffer(128);
		szT.getBuffer(64);
		szV.getBuffer(128);
		szResult.getBuffer(64);
		szItemName.getBuffer(128);
		szClientItemHandle.getBuffer(64);
		pResBody->getBuffer(256 + (tac->getRequestCount() * 256));
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			pMethodData = xmlData.getNext(pos);
			szItemName.empty();
			szTimeStamp.empty();
			szQuality.empty();
			szValue.empty();
			szResult.empty();
			szClientItemHandle.empty();
			szItemPath.empty();

			if (SUCCEEDED(pErrors[i]))
			{
				SOCmnPointer<SODaSItemTag> itemTag;

				if ((returnItemTime) || (returnValuesOnReply))
				{
					cache = ppRequests[i]->getCache();
					cache->get(cacheValue, &cacheQuality, &cacheTimestamp, ppRequests[i], pWriteDT[i], pServer->getLCID());
					itemTag = ppRequests[i]->getItemTag();
				}

				if (returnItemTime)
				{
					if ((itemTag->getAccessRights(pServer) & OPC_READABLE) == OPC_READABLE)
					{
						szTimeStamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)cacheTimestamp.getXMLDateTime());
					}
					else
					{
						// writeonly values can not access the cache with the request
						szTimeStamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)now.getXMLDateTime());
					}
				}

				if (returnValuesOnReply)
				{
					if ((itemTag->getAccessRights(pServer) & OPC_READABLE) == OPC_READABLE)
					{
						doXMLDATypeConversion(itemTag->getNativeDatatype(), pWriteDT[i], cacheValue);
						SOCmnVariant::getXMLValue(cacheValue, szT, szV);
						szValue.format(_T("<Value xsi:type=\"%s\">%s</Value>"), (LPCTSTR)szT, (LPCTSTR)szV);

						if (cacheQuality != OPC_QUALITY_GOOD)
						{
							szQuality.getBuffer(128);
							pServer->getXMLQuality(cacheQuality, TRUE, szQuality);
						}
					}
					else
					{
						pErrors[i] = OPC_E_WRITEONLY;
					}
				}
			}

			if (FAILED(pErrors[i]))
			{
				szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)pErrors[i], convHlp), exHlp));

				if (returnErrorText)
				{
					addAbnormalResult(&abnormalResultList, (DWORD)pErrors[i]);
				}
			}

			if (!pMethodData->m_clientItemHandle.isEmpty())
			{
				szClientItemHandle.format(_T(" ClientItemHandle=\"%s\""), 
					SOCmnStringEx::getXMLString(pMethodData->m_clientItemHandle, exHlp));
			}

			if (returnItemPath)
			{
				szItemPath.format(_T(" ItemPath=\"%s\""), (LPCTSTR)pMethodData->m_itemPath);
			}

			if (returnItemName)
			{
				szItemName.format(_T(" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pMethodData->m_itemName, exHlp));
			}

			szItem.format(_T("<Items%s%s%s%s%s>%s%s%s</Items>"), (LPCTSTR)szItemPath, (LPCTSTR)szItemName, (LPCTSTR)szClientItemHandle, (LPCTSTR)szTimeStamp, (LPCTSTR)szResult, (LPCTSTR)szDiagnosticInfo, (LPCTSTR)szValue, (LPCTSTR)szQuality);
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szItem));
			*pResBody += szItem;

			if (ppRequests[i])
			{
				ppRequests[i]->release();
			}

			i++;
		}

		*pResBody += _T("</RItemList>");

		if (returnErrorText)
		{
			// error texts
			getErrorTexts(pServer, &abnormalResultList, resErrors);

			if (!resErrors.isEmpty())
			{
				*pResBody += resErrors;
			}
		}

		if (ppRequests)
		{
			free(ppRequests);
		}

		if (pWriteDT)
		{
			free(pWriteDT);
		}

		if (pErrors)
		{
			free(pErrors);
		}

		*pResBody += _T("</WriteResponse></soap:Body></soap:Envelope>");
		// send response
		sendResponse(pServer, mth, &now, pResHeader, pResBody);
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_SOAP_FUNCTION
} // write


HRESULT SODaSXmlServer::subscribe(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(Subscribe)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_SUBSCRIBE, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Subscribe"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP
		SODASXMLSERVER_WAIT_FOR_IO_STARTUP
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SODaSXmlMethodData* pMethodData;
		DWORD count = xmlData.getCount();
		SOCmnVariant** ppValues = NULL;       // read values
		WORD* pQualities = NULL;       // read qualities
		SOCmnDateTime** ppTimeStamps = NULL; // read timestamps
		HRESULT* pErrors = NULL;       // results of each single read operation
		SODaSRequest** ppRequests = NULL;
		SOCmnDWORDList abnormalResultList;
		SOCmnStringEx resErrors;
		SOCmnListPosition pos;
		DWORD accessRights = 0;
		DWORD i;
		// request options
		BOOL returnDiagnosticInfo = FALSE;
		BOOL returnItemTime = FALSE;
		BOOL returnErrorText = TRUE;
		BOOL returnItemPath = FALSE;
		BOOL returnItemName = FALSE;
		SOCmnDateTime requestDeadline;
		SOCmnString convHlp;
		VARTYPE itemDatatype, grpDatatype;
		HRESULT grpError = S_OK;
		FLOAT grpDeadband = 0.0;
		FLOAT itemDeadband;
		BOOL returnValuesOnReply = FALSE;
		DWORD grpReqUpdateRate;
		DWORD grpRevUpdateRate = (DWORD)0x7FFFFFFF;
		DWORD subPingRate = 0;
		SOCmnStringEx exHlp;
		exHlp.getBuffer(128);

		// get request options
		if (xmlOptions.findKey(_T("ReturnDiagnosticInfo"), &convHlp))
		{
			returnDiagnosticInfo = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemTime"), &convHlp))
		{
			returnItemTime = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnErrorText"), &convHlp))
		{
			returnErrorText = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemPath"), &convHlp))
		{
			returnItemPath = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("ReturnItemName"), &convHlp))
		{
			returnItemName = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("RequestDeadline"), &convHlp))
		{
			SOCmnDateTime now;
			ULONGLONG msN, msDL;
			requestDeadline.setXMLDateTime(convHlp);
			now.now();
			msN = now.getMicroseconds();
			msDL = requestDeadline.getMicroseconds();

			if (msDL >= msN)
			{
				pServer->setDuetimeRead((DWORD)((msDL - msN) / 1000));
			}
			else
			{
				return OPC_E_TIMEDOUT;
			}
		}

		// get item list options
		if (xmlOptions.findKey(_T("SubscriptionPingRate"), &convHlp))
		{
			subPingRate = _tcstoul(convHlp, NULL, 10);
		}

		if (xmlOptions.findKey(_T("RequestedSamplingRate"), &convHlp))
		{
			grpReqUpdateRate = _tcstoul(convHlp, NULL, 10);
		}
		else
		{
			grpReqUpdateRate = 0; // take the minial update Rate
			pMethodData = xmlData.getHead();

			if (pMethodData)
			{
				grpReqUpdateRate = _tcstoul(pMethodData->m_reqSamplingRate, NULL, 10);
			}
		}

		if (xmlOptions.findKey(_T("ReturnValuesOnReply"), &convHlp))
		{
			returnValuesOnReply = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("Deadband"), &convHlp))
		{
			grpDeadband = (FLOAT)_tcstod(convHlp, NULL);
		}

		if (xmlOptions.findKey(_T("ReqType"), &convHlp))
		{
			if (!SOCmnVariant::getVARTYPE((LPCTSTR)convHlp, grpDatatype))
			{
				grpError = OPC_E_BADTYPE;
			}
		}
		else
		{
			grpDatatype = VT_EMPTY;
		}

		// allocate buffers
		if (returnValuesOnReply)
		{
			ppValues = (SOCmnVariant**) malloc(count * sizeof(SOCmnVariant*));
			pQualities = (WORD*) malloc(count * sizeof(WORD));
			ppTimeStamps = (SOCmnDateTime**) malloc(count * sizeof(SOCmnDateTime*));
			pErrors = (HRESULT*) malloc(count * sizeof(HRESULT));
			ppRequests = (SODaSRequest**)malloc(count * sizeof(SODaSRequest*));

			if ((ppValues == NULL)  || (pQualities == NULL) || (ppTimeStamps == NULL) || (pErrors == NULL) || (ppRequests == NULL))
			{
				if (ppValues)
				{
					free(ppValues);
					ppValues = NULL;
				}

				if (pQualities)
				{
					free(pQualities);
					pQualities = NULL;
				}

				if (ppTimeStamps)
				{
					free(ppTimeStamps);
					ppTimeStamps = NULL;
				}

				if (pErrors)
				{
					free(pErrors);
					pErrors = NULL;
				}

				if (ppRequests)
				{
					free(ppRequests);
					ppRequests = NULL;
				}

				return E_OUTOFMEMORY;
			}

			for (i = 0; i < count; i++)
			{
				ppValues[i] = new SOCmnVariant();
				ppTimeStamps[i] = new SOCmnDateTime();
				ppRequests[i] = NULL;
			}
		}

		// create group and items for the subscription
		SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
		SOCmnPointer<SODaSServer> subServer = entryDA->getXMLSubscriptionServer();
		SOCmnPointer<SODaSGroup> grp;

		if (SUCCEEDED(grpError))
		{
			grpError = subServer->addGroup(_T(""), TRUE, grpReqUpdateRate, 0, NULL, &grpDeadband, pServer->getLCID(), (SODaSGroup**)grp, &grpRevUpdateRate, FALSE);
		}
		else
		{
			subServer->addGroup(_T(""), TRUE, grpReqUpdateRate, 0, NULL, &grpDeadband, pServer->getLCID(), (SODaSGroup**)grp, &grpRevUpdateRate, FALSE);
		}

		if (SUCCEEDED(grpError))
		{
			// added group
#ifdef SOFEATURE_WATCH
			if (pServer->getCollectChanges())
			{
				SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
				entry->watchAddObject(subServer, grp);
			}

#endif
			grp->setPingRate(subPingRate);
			// allocate block for entries in item list of the group and request list of the cyclic transaction
			{
				SOCmnList<SOCmnElement> itemList(grp->getLeafList());
				DWORD freeListEntries;
				// get number of available allocated list entries
				freeListEntries = itemList.getAvailableAllocatedEntries();

				if (freeListEntries < count)
				{
					itemList.allocateBlock(count - freeListEntries);
				}

				itemList.autoUnlock();
				SOCmnPointer<SODaSTransaction> ctac = grp->getCyclicTransaction();

				if (!!ctac)
				{
					if (ctac->getType() == SODaSTransaction::cyclic)
					{
						SOCmnList<SODaSRequest> rList(ctac->getRequestList());
						freeListEntries = rList.getAvailableAllocatedEntries();

						if (freeListEntries < count)
						{
							rList.allocateBlock(count - freeListEntries);
						}
					}
				}
			}
			// generate initial value read
			SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
			SOCmnPointer<SODaSCreator> creator = engine->getCreator();
			SOCmnPointer<SODaSTransaction> tac;
			SOCmnList<SODaSRequest> reqList;

			if (returnValuesOnReply)
			{
				// create a asynchronous read transaction with the connection id of the cyclic notification transaction
				tac = creator->createTransaction(grp, SODaSTransaction::syncRead, SODaSTransaction::synchronous, 0);
				reqList = tac->getRequestList();
			}

			// call method to prepare for add items for dynamic namespace
			DWORD preValidateItemsCookie = 0;
			SOCmnList<SODaSItemData> itemDataList;

			if (grp->hasPrePostValidateItems())
			{
				SODaSItemData* pItemData;
				itemDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
				pos = xmlData.getStartPosition();

				while (pos)
				{
					pMethodData = xmlData.getNext(pos);
					itemDatatype = grpDatatype;

					if (!pMethodData->m_reqType.isEmpty())
					{
						if (!SOCmnVariant::getVARTYPE((LPCTSTR)pMethodData->m_reqType, itemDatatype))
						{
							pMethodData->m_result = OPC_E_BADTYPE;
						}
					}

					SOCMN_ALLOCATE_OBJECT(pItemData, SODaSItemData);
					pItemData->m_itemID =  pMethodData->m_itemName;
					pItemData->m_reqDatatype = itemDatatype;
					pItemData->m_itemPath = pMethodData->m_itemPath;
					pItemData->m_result = pMethodData->m_result;
					itemDataList.add(pItemData);
				}

				preValidateItemsCookie = grp->preValidateItems(TRUE, &itemDataList);
			}

			// add items
			SOCmnEvents events;
			DWORD reqEventId = events.create();
			pos = xmlData.getStartPosition();
			i = 0;

			while (pos)
			{
				pMethodData = xmlData.getNext(pos);

				if (pMethodData->m_reqType.isEmpty())
				{
					itemDatatype = grpDatatype;
				}
				else
				{
					if (!SOCmnVariant::getVARTYPE((LPCTSTR)pMethodData->m_reqType, itemDatatype))
					{
						pMethodData->m_result = OPC_E_BADTYPE;
					}
				}

				if (SUCCEEDED(pMethodData->m_result))
				{
					SOCmnString itemId;
					SOCmnPointer<SODaSItem> item;
					pMethodData->m_result = pServer->getItemID(pMethodData->m_itemPath, pMethodData->m_itemName, itemId);

					if (SUCCEEDED(pMethodData->m_result))
					{
						pMethodData->m_result = grp->addItem(itemId, _T(""), TRUE, 0, itemDatatype, &item, NULL, &accessRights);

						if (SUCCEEDED(pMethodData->m_result))
						{
							if (!pMethodData->m_deadband.isEmpty())
							{
								itemDeadband = (FLOAT)_tcstod(pMethodData->m_deadband, NULL);

								if (itemDeadband != 0.0) // ignore default deadband
								{
									pMethodData->m_result = item->setDeadBand(itemDeadband);
								}
							}

							if (!pMethodData->m_clientItemHandle.isEmpty())
							{
								item->setXMLClientHandle((LPCTSTR)pMethodData->m_clientItemHandle);
							}
						}
					}

					if ((SUCCEEDED(pMethodData->m_result)) && (returnValuesOnReply))
					{
						if (accessRights & OPC_READABLE)
						{
							// create cache request to send the current value to the client
							SOCmnPointer<SODaSItemTag> itemTag = item->getItemTag();

							if (checkXMLDATypeConversion(itemTag->getNativeDatatype(), itemDatatype))
							{
								SOCmnPointer<SODaSRequest> req;
								req = creator->createRequest(pServer, item, itemTag, SODaSRequest::read, SODaSRequest::syncMaxAge);

								if (req.isNotNull())
								{
									req->setFlag(SODASREQUEST_FLAG_SOAP);
									req->setItemTag(itemTag);
									req->setItem(item);
									req->setReqDatatype(itemDatatype);
									req->setResultAddress(pErrors + i);
									req->setResult(S_OK);
									req->setMaxAge(INFINITE);
									req->setReadValueAddress(ppValues[i]);
									req->setReadQualityAddress(pQualities + i);
									req->setReadTimeStampAddress(ppTimeStamps[i]);
									req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
									*(ppRequests + i) = (SODaSRequest*)req;
									ppRequests[i]->addRef();
									tac->insertRequest(req);
								}
								else
								{
									pErrors[i] = OPC_E_BADTYPE;
								}
							}
							else
							{
								pErrors[i] = OPC_E_BADTYPE;
							}
						}
						else
						{
							pErrors[i] = OPC_E_WRITEONLY;
						}
					}
				}

				i++;
			}

			if (preValidateItemsCookie != 0)
			{
				grp->postValidateItems(preValidateItemsCookie, &itemDataList);
			}

			itemDataList.destroy();
			grp->onAddItems();
			// recalculate revised update rate after the items are added
			grp->recalcRevUpdateRate();
			grpRevUpdateRate = grp->getUpdateRate();

			// start read operations
			if (returnValuesOnReply)
			{
				if (tac->getRequestCount() > 0)
				{
					tac->execute(pServer, &events, reqEventId, pServer->m_dueTimeRead);
				}

				tac->setCompletedEvent(NULL);
				tac->emptyRequestsCompletedEvent();
			}
		}

		// build response
		SOCmnDateTime now;
		SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
		SOCmnStringEx szSubHandle;
		pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
		now.now();
		pReplyBase->getBuffer(256);

		if (grp.isNotNull())
		{
			SOCmnList<SOCmnElement> itemList(grp->getLeafList());

			if (itemList.getCount() > 0)
			{
				szSubHandle.format(_T("0x%8.8lX"), (DWORD)grp->getOpcHandle());
				SOCmnPointer<SOSrvHTTPHandlerData> sessionObject = mth->getHTTPHandlerData();
				m_groupIdToSessionObjectMap.add(sessionObject, (DWORD)grp->getOpcHandle());
			}
		}

		getReplyBase(*pReplyBase, _T("SubscribeResult"), &now, pServer, mth);
		getXMLStart(pServer, SODASXMLMETHOD_STATE_SUBSCRIBE, *pResHeader); // uses only the buffer, is not the header
		pResBody->format(_T("%s<soap:Body><SubscribeResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\" ServerSubHandle=\"%s\">%s<RItemList RevisedSamplingRate=\"%lu\">"), (LPCTSTR)*pResHeader, (LPCTSTR)szSubHandle, (LPCTSTR)*pReplyBase, grpRevUpdateRate);
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: ServerSubHandle=\"%s\" RevisedSamplingRate=\"%lu\""), (LPCTSTR)szSubHandle, grpRevUpdateRate));
		// for every item
		SOCmnStringEx szItem;
		SOCmnStringEx szTimeStamp;
		SOCmnStringEx szValue;
		SOCmnStringEx szQuality;
		SOCmnStringEx szResult;
		SOCmnStringEx szItemName;
		SOCmnStringEx szItemPath;
		SOCmnStringEx szClientItemHandle;
		SOCmnStringEx szDiagnosticInfo;
		SOCmnStringEx szT, szV;
		SOCmnStringEx szRevItemSamplingRate;
		SOCmnStringEx szRItemList;
		BOOL addRItemList;
		szItem.getBuffer(256);
		szValue.getBuffer(128);
		szT.getBuffer(64);
		szV.getBuffer(128);
		szResult.getBuffer(64);
		szItemName.getBuffer(128);
		szClientItemHandle.getBuffer(64);
		szRItemList.getBuffer(256 + (count * 256));
		pos = xmlData.getStartPosition();
		i = 0;

		while (pos)
		{
			addRItemList = FALSE;
			pMethodData = xmlData.getNext(pos);

			if (FAILED(grpError))
			{
				pMethodData->m_result = grpError;
			}

			szItemName.empty();
			szTimeStamp.empty();
			szQuality.empty();
			szResult.empty();
			szClientItemHandle.empty(), szRevItemSamplingRate.empty();
			szItemPath.empty();

			if (SUCCEEDED(pMethodData->m_result))
			{
				if (returnValuesOnReply)
				{
					addRItemList = TRUE;

					if (SUCCEEDED(pErrors[i]))
					{
						if (returnItemTime)
						{
							szTimeStamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)ppTimeStamps[i]->getXMLDateTime());
						}

						// special XML DA conversion boolean to string
						if (ppRequests[i])
							if (ppRequests[i]->getReqDatatype() == VT_BSTR)
							{
								SOCmnPointer<SODaSItemTag> itemTag = ppRequests[i]->getItemTag();

								if (itemTag.isNotNull())
									if (itemTag->getNativeDatatype() == VT_BOOL)
									{
										// convert "0" -> "false", "-1" -> "true"
										USES_CONVERSION;
										LPTSTR curVal = OLE2T(ppValues[i]->bstrVal);

										if (_tcscmp(curVal, _T("0")) == 0)
										{
											ppValues[i]->setBSTR(_T("false"));
										}
										else
										{
											ppValues[i]->setBSTR(_T("true"));
										}
									}
							}

						SOCmnVariant::getXMLValue(ppValues[i], szT, szV);
						szValue.format(_T("<Value xsi:type=\"%s\">%s</Value>"), (LPCTSTR)szT, (LPCTSTR)szV);

						if (pQualities[i] != OPC_QUALITY_GOOD)
						{
							szQuality.getBuffer(128);
							pServer->getXMLQuality(pQualities[i], TRUE, szQuality);
						}

						if (ppRequests[i])
						{
							SOCmnPointer<SODaSItem> item = ppRequests[i]->getItem();
							item->setSendValue(0, *ppValues[i], pQualities[i]);
						}
					}
					else
					{
						szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)pErrors[i], convHlp), exHlp));

						if (returnErrorText)
						{
							addAbnormalResult(&abnormalResultList, (DWORD)pErrors[i]);
						}
					}
				}
			}
			else
			{
				szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)pMethodData->m_result, convHlp), exHlp));

				if (returnErrorText)
				{
					addAbnormalResult(&abnormalResultList, (DWORD)pMethodData->m_result);
				}

				addRItemList = TRUE;
			}

			if (addRItemList)
			{
				if (!pMethodData->m_clientItemHandle.isEmpty())
				{
					szClientItemHandle.format(_T(" ClientItemHandle=\"%s\""), 
						SOCmnStringEx::getXMLString((LPCTSTR)pMethodData->m_clientItemHandle, exHlp));
				}

				if (returnItemPath)
				{
					szItemPath.format(_T(" ItemPath=\"%s\""), (LPCTSTR)pMethodData->m_itemPath);
				}

				if (returnItemName)
				{
					szItemName.format(_T(" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(pMethodData->m_itemName, exHlp));
				}

				if (!pMethodData->m_reqSamplingRate.isEmpty())
				{
					szRevItemSamplingRate.format(_T(" RevisedSamplingRate=\"%lu\""), grpRevUpdateRate);

					if ((SUCCEEDED(pMethodData->m_result)) && (_tcstoul(pMethodData->m_reqSamplingRate, NULL, 10) != grpRevUpdateRate))
					{
						szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ(OPC_S_UNSUPPORTEDRATE, convHlp), exHlp));

						if (returnErrorText)
						{
							addAbnormalResult(&abnormalResultList, OPC_S_UNSUPPORTEDRATE);
						}
					}
				}

				szItem.format(_T("<Items><ItemValue%s%s%s%s%s%s>%s%s%s</ItemValue></Items>"), (LPCTSTR)szRevItemSamplingRate, (LPCTSTR)szItemPath, (LPCTSTR)szItemName, (LPCTSTR)szClientItemHandle, (LPCTSTR)szTimeStamp, (LPCTSTR)szResult, (LPCTSTR)szDiagnosticInfo, (LPCTSTR)szValue, (LPCTSTR)szQuality);
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szItem));
				szRItemList += szItem;
			}

			if (ppRequests)
				if (ppRequests[i])
				{
					ppRequests[i]->release();
				}

			if (ppValues)
			{
				delete ppValues[i];
			}

			if (ppTimeStamps)
			{
				delete ppTimeStamps[i];
			}

			i++;
		}

		if (ppValues)
		{
			free(ppValues);
		}

		if (ppTimeStamps)
		{
			free(ppTimeStamps);
		}

		if (pQualities)
		{
			free(pQualities);
		}

		if (pErrors)
		{
			free(pErrors);
		}

		if (ppRequests)
		{
			free(ppRequests);
		}

		if (!szRItemList.isEmpty())
		{
			pResBody->getBuffer(256 + szRItemList.getLength());
			*pResBody += szRItemList;
		}

		*pResBody += _T("</RItemList>");

		if (returnErrorText)
		{
			// error texts
			getErrorTexts(pServer, &abnormalResultList, resErrors);

			if (!resErrors.isEmpty())
			{
				*pResBody += resErrors;
			}
		}

		*pResBody += _T("</SubscribeResponse></soap:Body></soap:Envelope>");
		// send response
		sendResponse(pServer, mth, &now, pResHeader, pResBody);
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_SOAP_FUNCTION
} // subscribe


HRESULT SODaSXmlServer::subscriptionPolledRefresh(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(SubscriptionPolledRefresh)
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_SUBSCRIPTIONPOLLEDREFRESH, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|SubscriptionPolledRefresh"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnListPosition pos;
		SODaSXmlMethodData* pMethodData;
		SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
		SOCmnPointer<SODaSCreator> creator = entryDA->getCreator();
		SOCmnPointer<SODaSTransaction> tac;
		DWORD reCheckSpan = SOCMN_TIME_40DAYS;
		DWORD pingRate = SOCMN_TIME_40DAYS;
		SOCmnPointer<SODaSServer> subServer = entryDA->getXMLSubscriptionServer();
		SOCmnString convHlp;
		SOCmnDateTime holdTimeP;
		DWORD waitTimeP = 0;
		BOOL returnAllItems = FALSE;
		DWORD waitTimeTC;
		DWORD holdTimeTC;
		DWORD nowTC;
		SOCmnDateTime now;
		LONGLONG holdSpan;
		BOOL validSub;
		nowTC = GetTickCount();
		now.now();

		// check and calculate hold and wait time
		if (xmlOptions.findKey(_T("ReturnAllItems"), &convHlp))
		{
			returnAllItems = XMLSTR2BOOL(convHlp);
		}

		if (xmlOptions.findKey(_T("RequestDeadline"), &convHlp))
		{
			SOCmnDateTime requestDeadline;
			ULONGLONG msN, msDL;
			requestDeadline.setXMLDateTime(convHlp);
			msN = now.getMicroseconds();
			msDL = requestDeadline.getMicroseconds();

			if (msDL < msN)
			{
				return OPC_E_TIMEDOUT;
			}
		}

		if (xmlOptions.findKey(_T("HoldTime"), &convHlp))
		{
			holdTimeP.setXMLDateTime(convHlp);

			if (!returnAllItems)
				if (xmlOptions.findKey(_T("WaitTime"), &convHlp))
				{
					waitTimeP = _tcstoul(convHlp, NULL, 0);
				}
		}
		else
		{
			holdTimeP.now();
		}

		holdSpan = holdTimeP.getMicroseconds() - now.getMicroseconds();
		holdSpan /= 1000;

		if (holdSpan >= 0)
		{
			holdTimeTC = nowTC + (DWORD)holdSpan;
		}
		else
		{
			holdTimeTC = nowTC;
		}

		waitTimeTC = holdTimeTC + waitTimeP;
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  TIMES: now: %lu  hold: %lu  wait: %lu"), nowTC, holdTimeTC, waitTimeTC));
		// create polled refresh transaction
		tac = creator->createTransaction(NULL, SODaSTransaction::polledRefresh, SODaSTransaction::asyncSOAPMethod, 0);
		// start all refreshes in the groups
		SOCmnPointer<SODaSGroup> grp;
		SOCmnStringEx szInvalSubH;
		pos = xmlData.getStartPosition();

		while (pos)
		{
			pMethodData = xmlData.getNext(pos);
			validSub = FALSE;

			if (pMethodData->m_serverSubHandle.getLength() == 10)
			{
				grp = subServer->getGroupByHandle(_tcstoul(pMethodData->m_serverSubHandle, NULL, 0));

				if (grp.isNotNull())
				{
					SOCmnPointer<SOSrvHTTPHandlerData> sessionObject;
					{
						SOCmnSingleLock<SOCmnSync> locker(m_groupIdToSessionObjectMap.getSyncObject());
						sessionObject = m_groupIdToSessionObjectMap.findKey(grp->getOpcHandle());
					}

					if (sessionObject.isNotNull())
					{
						sessionObject->m_rcvTime.now();
					}

					// valid group handle
					DWORD grpUpdateRate = grp->getUpdateRate();
					DWORD grpPingRate   = grp->getPingRate();

					if (reCheckSpan > grpUpdateRate)
					{
						reCheckSpan = grpUpdateRate;
					}

					if (pingRate > grpPingRate)
					{
						pingRate = grpPingRate;
					}

					// create a polled refresh request for each item of the group
					SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)grp->getLeafList());
					SODaSItem* pItem;
					SOCmnListPosition posItem;
					posItem = itemList.getStartPosition();

					while (posItem)
					{
						pItem = itemList.getNext(posItem);
						SOCmnPointer<SODaSItemTag> itemTag = pItem->getItemTag();
						SOCmnPointer<SODaSRequest> req;
						req = creator->createRequest(pServer, pItem, itemTag, SODaSRequest::read, SODaSRequest::polledRefresh);

						if (req.isNotNull())
						{
							req->setFlag(SODASREQUEST_FLAG_SOAP);
							req->setItem(pItem);
							req->setResult(S_OK);
							tac->insertRequest(req);
						}
					}

					validSub = TRUE;
				}
			}

			if (!validSub)
			{
				// no valid group handle
				szInvalSubH.format(_T("<InvalidServerSubHandles>%s</InvalidServerSubHandles>"), (LPCTSTR)pMethodData->m_serverSubHandle);
				mth->m_methodResults += szInvalSubH;
			}

			grp.release();
		}

		if (tac->getRequestCount() > 0)
		{
			// check hold and wait time
			if (pServer->checkHoldTime(holdSpan, waitTimeP, reCheckSpan, pingRate, tac))
			{
				tac->setXMLMethod(mth);
				tac->setHoldTime(holdTimeTC);
				tac->setWaitTime(waitTimeTC);
				tac->setReCheckSpan(reCheckSpan);
				// add polled refresh transaction to the server and start it
				tac->start(pServer);
				pServer->addTransaction(tac);

				// the response is send asynchronous to the request
				// S_OK here means refresh request started successfully
				if (res == S_OK)
				{
					return S_OK;    // return before SOSRV_END_SOAP_FUNCTION because the method will be completed asynchronous
				}
			}
			else
			{
				// invalid hold time
				res = OPC_E_INVALIDHOLDTIME;
			}
		}
		else
		{
			// no valid request from this transaction
			sendPolledRefreshResponse(pServer, mth, tac);
			return S_OK;    // return before SOSRV_END_SOAP_FUNCTION because the method will be completed asynchronous
		}
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_SOAP_FUNCTION
}

void SODaSXmlServer::sendPolledRefreshResponse(
	IN SODaSServer* pServer,
	IN SODaSXmlMethod* mth,
	IN SODaSTransaction* pTac)
{
	SOCmnStringList xmlOptions(mth->getMethodOptions());
	SOCmnDWORDList abnormalResultList;
	SOCmnStringEx resErrors;
	SOCmnString convHlp;
	BOOL returnAllItems = FALSE;
	BOOL returnDiagnosticInfo = FALSE;
	BOOL returnItemTime = FALSE;
	BOOL returnErrorText = TRUE;
	BOOL returnItemPath = FALSE;
	BOOL returnItemName = FALSE;
	SOCmnStringEx exHlp;
	exHlp.getBuffer(128);

	// get request options
	if (xmlOptions.findKey(_T("ReturnDiagnosticInfo"), &convHlp))
	{
		returnDiagnosticInfo = XMLSTR2BOOL(convHlp);
	}

	if (xmlOptions.findKey(_T("ReturnItemTime"), &convHlp))
	{
		returnItemTime = XMLSTR2BOOL(convHlp);
	}

	if (xmlOptions.findKey(_T("ReturnErrorText"), &convHlp))
	{
		returnErrorText = XMLSTR2BOOL(convHlp);
	}

	if (xmlOptions.findKey(_T("ReturnItemPath"), &convHlp))
	{
		returnItemPath = XMLSTR2BOOL(convHlp);
	}

	if (xmlOptions.findKey(_T("ReturnItemName"), &convHlp))
	{
		returnItemName = XMLSTR2BOOL(convHlp);
	}

	// get subscription polled refresh options
	if (xmlOptions.findKey(_T("ReturnAllItems"), &convHlp))
	{
		returnAllItems = XMLSTR2BOOL(convHlp);
	}

	// build response
	SOCmnDateTime now;
	SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
	SOCmnStringEx szSubHandle;
	pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
	now.now();

	if ((now.getMicroseconds() - mth->getRcvTime().getMicroseconds()) < 1000) // answer in less than a millisecond
	{
		// CTT need! Wait to be not too fast
		now.addMicroseconds(1000);
	}

	pReplyBase->getBuffer(256);
	getReplyBase(*pReplyBase, _T("SubscriptionPolledRefreshResult"), &now, pServer, mth);
	getXMLStart(pServer, SODASXMLMETHOD_STATE_SUBSCRIPTIONPOLLEDREFRESH, *pResHeader); // uses only the buffer, is not the header
	pResBody->format(_T("%s<soap:Body><SubscriptionPolledRefreshResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">%s"), (LPCTSTR)*pResHeader, (LPCTSTR)*pReplyBase);

	if (!mth->m_methodResults.isEmpty())
	{
		*pResBody += mth->m_methodResults; // invalid subscription handles
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)mth->m_methodResults));
	}

	// for every item
	SOCmnStringEx szItem;
	SOCmnStringEx szTimeStamp;
	SOCmnStringEx szValue;
	SOCmnStringEx szQuality;
	SOCmnStringEx szResult;
	SOCmnStringEx szItemName;
	SOCmnStringEx szItemPath;
	SOCmnStringEx szClientItemHandle;
	SOCmnStringEx szXMLItemHandle;
	SOCmnStringEx szDiagnosticInfo;
	SOCmnStringEx szRListHeader;
	SOCmnStringEx szT, szV;
	SOCmnStringEx szRItemList;
	BOOL addRItemList = FALSE;
	SOCmnList<SODaSRequest> reqList(pTac->getRequestList());
	SOCmnListPosition posReq;
	SODaSRequest* req;
	SOCmnPointer<SODaSCache> cache;
	SOCmnPointer<SODaSItem> item;
	SOCmnPointer<SODaSGroup> group;
	SOCmnPointer<SODaSItemTag> itemTag;
	SOCmnPointer<SODaSServer> server;
	DWORD curSubHandle = 0;
	WORD quality;
	SOCmnDateTime timeStamp;
	SOCmnVariant value;
	BOOL hasChanged;
	szItem.getBuffer(256);
	szValue.getBuffer(128);
	szT.getBuffer(64);
	szV.getBuffer(128);
	szResult.getBuffer(64);
	szItemName.getBuffer(128);
	szClientItemHandle.getBuffer(64);
	szRItemList.getBuffer(256 + (pTac->getRequestCount() * 256));

	if (returnItemPath)
	{
		szItemPath = _T(" ItemPath=\"\"");
	}

	posReq = reqList.getStartPosition();

	while (posReq)
	{
		req = reqList.getNext(posReq);
		item = req->getItem();
		group = (SODaSGroup*)item->getParentBranch();
		itemTag = item->getItemTag();

		if (!group || !itemTag)
		{
			//  orphan request drop it
			req->isCompleted();
			continue;
		}

		cache = itemTag->getCache();
		server = req->getServer();

		if (curSubHandle != (DWORD)group->getOpcHandle())
		{
			// new subscription
			if ((curSubHandle != 0) && (addRItemList))
			{
				szRItemList += _T("</RItemList>");
				*pResBody += szRItemList;
			}

			curSubHandle = (DWORD)group->getOpcHandle();
			addRItemList = FALSE;
			szRItemList.empty();
			szRListHeader.format(_T("<RItemList SubscriptionHandle=\"0x%8.8lX\">"), curSubHandle);
			szRItemList += szRListHeader;

			if (group.isNotNull())
			{
				// update last send data change time of the group
				SOCmnPointer<SODaSTransaction> cycTac = group->getCyclicTransaction();
				cycTac->updateLastSendDataChangeTime();
			}
		}

		if ((cache.isNotNull()) && (item.isNotNull()) && (itemTag.isNotNull()) && (server.isNotNull()))
		{
			HRESULT convRes;

			if (cache->get(value, &quality, &timeStamp, req, item->getReqDatatype(), server->getLCID(), &convRes))
			{
				if (!returnAllItems)
				{
					hasChanged = req->hasChanged(0, value, quality);
				}
				else
				{
					hasChanged = TRUE;
				}
			}
			else
			{
				hasChanged = TRUE;

				if (SUCCEEDED(req->getResult()))
				{
					req->setResult(convRes);
				}
			}

			if (hasChanged)
			{
				item->setSendValue(0, value, quality);
				szItemName.empty();
				szTimeStamp.empty();
				szValue.empty();
				szQuality.empty();
				szResult.empty();
				szClientItemHandle.empty();

				if (SUCCEEDED(req->getResult()))
				{
					if (returnItemTime)
					{
						szTimeStamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)timeStamp.getXMLDateTime());
					}

					// special XML DA conversion boolean to string
					if (req)
					{
						itemTag = req->getItemTag();

						if (itemTag.isNotNull())
						{
							doXMLDATypeConversion(itemTag->getNativeDatatype(), req->getReqDatatype(), value);
						}
					}

					SOCmnVariant::getXMLValue(value, szT, szV);
					szValue.format(_T("<Value xsi:type=\"%s\">%s</Value>"), (LPCTSTR)szT, (LPCTSTR)szV);

					if (quality != OPC_QUALITY_GOOD)
					{
						szQuality.getBuffer(128);
						pServer->getXMLQuality(quality, TRUE, szQuality);
					}
				}
				else
				{
					szResult.format(_T(" ResultID=\"%s\""), SOCmnStringEx::getXMLString(pServer->getErrorCodeSZ((DWORD)req->getResult(), convHlp), exHlp));

					if (returnErrorText)
					{
						addAbnormalResult(&abnormalResultList, (DWORD)req->getResult());
					}
				}

				if (item->getXMLClientHandle(szXMLItemHandle))
					if (!szXMLItemHandle.isEmpty())
					{
						szXMLItemHandle.doXMLReplacements(szXMLItemHandle);
						szClientItemHandle.format(_T(" ClientItemHandle=\"%s\""), (LPCTSTR)szXMLItemHandle);
					
					}

				if (returnItemName)
				{
					szItemName.format(_T(" ItemName=\"%s\""), (LPCTSTR)SOCmnStringEx::getXMLString(itemTag->getItemID(), exHlp));
				}

				szItem.format(_T("<Items%s%s%s%s%s>%s%s%s</Items>"), (LPCTSTR)szItemPath, (LPCTSTR)szItemName, (LPCTSTR)szClientItemHandle, (LPCTSTR)szTimeStamp, (LPCTSTR)szResult, (LPCTSTR)szDiagnosticInfo, (LPCTSTR)szValue, (LPCTSTR)szQuality);
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  OUT: %s"), (LPCTSTR)szItem));
				szRItemList += szItem;
				addRItemList = TRUE;
			}
		}
	}

	if (addRItemList > 0)
	{
		szRItemList += _T("</RItemList>");
		*pResBody += szRItemList;
	}

	if (returnErrorText)
	{
		// error texts
		getErrorTexts(pServer, &abnormalResultList, resErrors);

		if (!resErrors.isEmpty())
		{
			*pResBody += resErrors;
		}
	}

	*pResBody += _T("</SubscriptionPolledRefreshResponse></soap:Body></soap:Envelope>");
	// send response
	sendResponse(pServer, mth, &now, pResHeader, pResBody);
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, pServer, (_T("SubscriptionPolledRefresh")));
}


HRESULT SODaSXmlServer::subscriptionCancel(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOSRV_BEGIN_SOAP_FUNCTION(SubscriptionCancel)
	res = E_FAIL;
	// parse request
	mth->setExpectedMethod(SODASXMLMETHOD_STATE_SUBSCRIPTIONCANCEL, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|SubscriptionCancel"));

	if ((mth->parse()) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		SODASXML_TRACE_INPUT(pServer, mth)
		// get request options
		setReqLocaleID(pServer, mth);
		// do work
		SOCmnStringList xmlOptions(mth->getMethodOptions());
		SOCmnString clientRequestHandle;
		SOCmnString convHlp;
		DWORD grpCookie = 0;
		SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
		SOCmnPointer<SODaSServer> subServer = entryDA->getXMLSubscriptionServer();

		if (xmlOptions.findKey(_T("ServerSubHandle"), &convHlp))
		{
			grpCookie = _tcstoul(convHlp, NULL, 0);
		}

		xmlOptions.findKey(_T("ClientRequestHandle"), &clientRequestHandle);

		if (grpCookie != 0)
		{
			SOCmnPointer<SODaSGroup> grp = subServer->getGroupByHandle(grpCookie);

			if (grp.isNotNull())
			{
				grp->cancelPolledRefreshTransactions();
				SOCmnSingleLock<SOCmnSync> locker(m_groupIdToSessionObjectMap.getSyncObject());
				m_groupIdToSessionObjectMap.removeKey(grp->getOpcHandle());
			}

			grp.release();
			res = subServer->removeGroup(grpCookie);

			if (SUCCEEDED(res))
			{
				SOCmnString szCRH;
				SOCmnDateTime now;
				SOCmnStringEx* pResHeader, *pReplyBase, *pResBody;
				pServer->getXMLBuffers(&pResHeader, &pReplyBase, &pResBody);
				now.now();

				if (!clientRequestHandle.isEmpty())
				{
					szCRH.format(_T(" ClientRequestHandle=\"%s\""), (LPCTSTR)clientRequestHandle);
				}

				getXMLStart(pServer, SODASXMLMETHOD_STATE_SUBSCRIPTIONCANCEL, *pResHeader); // uses only the buffer, is not the header
				pResBody->format(_T("%s<soap:Body><SubscriptionCancelResponse xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\"%s/></soap:Body></soap:Envelope>"), (LPCTSTR)*pResHeader, (LPCTSTR)szCRH);
				sendResponse(pServer, mth, &now, pResHeader, pResBody);
			}
			else
			{
				res = OPC_E_NOSUBSCRIPTION;
			}
		}
	}

	SOSRV_END_SOAP_FUNCTION
}


void SODaSXmlServer::doXMLDATypeConversion(VARTYPE nativeDT, VARTYPE reqDT, SOCmnVariant& value)
{
	// special XML DA conversion boolean to string
	if (reqDT == VT_BSTR)
	{
		if (nativeDT == VT_BOOL)
		{
			// convert "0" -> "false", "-1" -> "true"
			USES_CONVERSION;
			LPTSTR curVal = OLE2T(value.bstrVal);

			if (_tcscmp(curVal, _T("0")) == 0)
			{
				value.setBSTR(_T("false"));
			}
			else
			{
				value.setBSTR(_T("true"));
			}
		}
	}
}

BOOL SODaSXmlServer::checkXMLDATypeConversion(VARTYPE nativeDT, VARTYPE reqDT)
{
	if (reqDT == VT_DATE)
	{
		switch (nativeDT)
		{
		case VT_DATE:
		case VT_BSTR:
			break;

		default:
			return FALSE;
			break;
		}
	}

	return TRUE;
}

void SODaSXmlServer::traceInput(IN SODaSServer* pServer, IN SODaSXmlMethod* mth)
{
	SOCmnStringList xmlOptions(mth->getMethodOptions());
	SOCmnList<SODaSXmlMethodData> xmlData(mth->getMethodData());
	SOCmnListPosition posO;
	SOCmnListPosition posD;
	SOCmnString val, key;
	SODaSXmlMethodData* pData;
	posO = xmlOptions.getStartPosition();

	while (posO)
	{
		val = xmlOptions.getNext(posO, &key);
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  %s = %s"), (LPCTSTR)key, (LPCTSTR)val));
	}

	posD = xmlData.getStartPosition();

	while (posD)
	{
		pData = xmlData.getNext(posD);

		switch (pData->m_type)
		{
		case SODASXMLMETHODDATA_TYPE_PROPERTYNAME:
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  property: %s"), (LPCTSTR)pData->m_propertyName));
			break;

		case SODASXMLMETHODDATA_TYPE_ITEM:
		{
			switch (mth->getExpectedMethodState())
			{
			case SODASXMLMETHOD_STATE_READ:
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  item: %s ; handle: %s ; maxAge: %s ; reqType: %s"), (LPCTSTR)pData->m_itemName, (LPCTSTR)pData->m_clientItemHandle, (LPCTSTR)pData->m_maxAge, (LPCTSTR)pData->m_reqType));
				break;

			case SODASXMLMETHOD_STATE_WRITE:
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  item: %s ; handle: %s ; value: %s %s ; quality: %s %s %s ; timestamp: %s"), (LPCTSTR)pData->m_itemName, (LPCTSTR)pData->m_clientItemHandle, (LPCTSTR)pData->m_value.m_datatype, (LPCTSTR)pData->m_value.m_value, (LPCTSTR)pData->m_quality, (LPCTSTR)pData->m_qualityLimit, (LPCTSTR)pData->m_qualityVendor, (LPCTSTR)pData->m_timestamp));
				break;

			case SODASXMLMETHOD_STATE_SUBSCRIBE:
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  item: %s ; handle: %s ; deadband: %s ; reqType: %s ; reqSamplingRate: %s ; enableBuf: %s"), (LPCTSTR)pData->m_itemName, (LPCTSTR)pData->m_clientItemHandle, (LPCTSTR)pData->m_deadband, (LPCTSTR)pData->m_reqType, (LPCTSTR)pData->m_reqSamplingRate, (LPCTSTR)pData->m_enableBuffering));
				break;

			default:
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("  IN:  item: %s"), (LPCTSTR)pData->m_itemName));
				break;
			}
		}
		break;
		}
	}
}


//-----------------------------------------------------------------------------
// SODaSXmlMethod                                                             |
//-----------------------------------------------------------------------------

SODaSXmlMethod::SODaSXmlMethod(void)
{
	m_methodOptions.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	m_methodData.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
	m_pCurMethodData = NULL;
	m_charDataType = SODASXMLMETHODDATA_TYPE_NONE;
}

SODaSXmlMethod::SODaSXmlMethod(IN BYTE xmlMethodType, IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody)
	: SOSrvXmlMethod(xmlMethodType, pHandler, pHTTPData, lenBody, pBody)
{
	m_methodOptions.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	m_methodData.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
	m_pCurMethodData = NULL;
	m_charDataType = SODASXMLMETHODDATA_TYPE_NONE;
}


SODaSXmlMethod::~SODaSXmlMethod(void)
{
	m_methodOptions.destroy();
	m_methodData.destroy();
	m_arrayStack.destroy();
}

void SODaSXmlMethod::handleStartElement(LPCSTR nameA, LPCSTR* atts)
{
	USES_CONVERSION;
#ifdef SOOS_WINDOWS
	_acp = CP_UTF8;
#endif
	LPCTSTR name = A2CT(nameA);
	LPCTSTR attName, attVal;
	DWORD i;
	_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA)
	{
		// trace elements
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("S: %s"), name));

		for (i = 0; atts[i]; i += 2)
		{
			attName = A2CT(atts[i]);
			attVal = A2CT(atts[i + 1]);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("   %s = %s"), attName, attVal));
		}
	}

	if (getObjectState() < SOCMNXMLMETHOD_STATE_SOAP_BODY)
	{
		SOSrvXmlMethod::handleStartElement(nameA, atts);
	}
	else
	{
		switch (getObjectState())
		{
		case SOCMNXMLMETHOD_STATE_SOAP_BODY: // wait for method
			if (_tcsicmp(name, getExpectedMethodString()) == 0)
			{
				if ((SODASXMLMETHOD_STATE_BROWSE != getExpectedMethodState()) &&
					(SODASXMLMETHOD_STATE_GETPROPERTIES != getExpectedMethodState()))
				{
					for (i = 0; atts[i]; i += 2)
					{
						attName = A2CT(atts[i]);
						attVal = A2CT(atts[i + 1]);
						// add the attributes to the method options
						m_methodOptions.add(attVal, attName);
					}

					setObjectState(getExpectedMethodState());
					m_elementStack.push(name);
				}
				else
				{
					SOCmnStringEx exHlp;

					for (i = 0; atts[i]; i += 2)
					{
						attName = A2CT(atts[i]);
						attVal = A2CT(atts[i + 1]);

						// add the attributes to the method options
						if (_tcsicmp(attName, _T("ItemName")) == 0)
						{
							SOCmnStringEx::setXMLString(attVal, exHlp);
						}
						else if (_tcsicmp(attName, _T("ItemPath")) == 0)
						{
							SOCmnStringEx::setXMLString(attVal, exHlp);
						}
						else if (_tcsicmp(attName, _T("ContinuationPoint")) == 0)
						{
							SOCmnStringEx::setXMLString(attVal, exHlp);
						}
						else if (_tcsicmp(attName, _T("ElementNameFilter")) == 0)
						{
							SOCmnStringEx::setXMLString(attVal, exHlp);
						}
						else if (_tcsicmp(attName, _T("VendorFilter")) == 0)
						{
							SOCmnStringEx::setXMLString(attVal, exHlp);
						}
						else
						{
							exHlp = attVal;
						}

						m_methodOptions.add((LPCTSTR)exHlp, attName);
					}

					setObjectState(getExpectedMethodState());
					m_elementStack.push(name);
				}
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Not expected method %s: instead %s"), getExpectedMethodString(), name));
			}

			break;

		case SODASXMLMETHOD_STATE_BROWSE:
			if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|PropertyNames")) == 0)
			{
				m_charDataType = SODASXMLMETHODDATA_TYPE_PROPERTYNAME;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid element on browse call: %s"), name));
			}

			break;

		case SODASXMLMETHOD_STATE_GETPROPERTIES:
			if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|ItemIDs")) == 0)
			{
				SODaSXmlMethodData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaSXmlMethodData)
				pMD->m_type = SODASXMLMETHODDATA_TYPE_ITEM;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("ItemName")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemName);
					}
					else if (_tcsicmp(attName, _T("ItemPath")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemPath);
					}
				}

				m_methodData.add(pMD);
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|PropertyNames")) == 0)
			{
				m_charDataType = SODASXMLMETHODDATA_TYPE_PROPERTYNAME;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid element on call: %s"), name));
			}

			break;

		case SODASXMLMETHOD_STATE_WRITE:
		case SODASXMLMETHOD_STATE_READ:
		case SODASXMLMETHOD_STATE_SUBSCRIBE:
			if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Options")) == 0)
			{
				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName);
				}

				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|ItemList")) == 0)
			{
				SOCmnString attVal4E;
				setObjectState(SODASXMLMETHOD_STATE_ITEMLIST);

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal4E = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("ReqType")) == 0)
					{
						expandNamespace(attVal4E);
					}
					else if (_tcsicmp(attName, _T("ItemPath")) == 0)
					{
						SOCmnStringEx exHlp;
						SOCmnStringEx::setXMLString(attVal4E, exHlp);
						attVal4E = exHlp;
					}

					m_methodOptions.add(attVal4E, attName);
				}

				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid element on read|write|subscribe call: %s"), name));
			}

			break;

		case SODASXMLMETHOD_STATE_SUBSCRIPTIONPOLLEDREFRESH:
			if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Options")) == 0)
			{
				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName);
				}

				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|ServerSubHandles")) == 0)
			{
				m_charDataType = SODASXMLMETHODDATA_TYPE_SERVERSUBHANDLE;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid element on subscriptionPolledRefresh call: %s"), name));
			}

			break;

		case SODASXMLMETHOD_STATE_ITEMLIST:
			if ((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Items")) == 0) &&
				(m_charDataType == SODASXMLMETHODDATA_TYPE_NONE))
			{
				SODaSXmlMethodData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaSXmlMethodData)
				pMD->m_type = SODASXMLMETHODDATA_TYPE_ITEM;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("ItemName")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemName);
					}
					else if (_tcsicmp(attName, _T("ClientItemHandle")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_clientItemHandle);
					}
					else if (_tcsicmp(attName, _T("ItemPath")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemPath);
					}

					switch (getExpectedMethodState())
					{
					case SODASXMLMETHOD_STATE_READ:
						if (_tcsicmp(attName, _T("MaxAge")) == 0)
						{
							pMD->m_maxAge = attVal;
						}
						else if (_tcsicmp(attName, _T("ReqType")) == 0)
						{
							pMD->m_reqType = attVal;
							expandNamespace(pMD->m_reqType);
						}

						break;

					case SODASXMLMETHOD_STATE_WRITE:
						if (_tcsicmp(attName, _T("ValueTypeQualifier")) == 0)
						{
							pMD->m_valueTypeQualifier = attVal;
						}
						else if (_tcsicmp(attName, _T("Timestamp")) == 0)
						{
							pMD->m_timestamp = attVal;
						}

						break;

					case SODASXMLMETHOD_STATE_SUBSCRIBE:
						if (_tcsicmp(attName, _T("Deadband")) == 0)
						{
							pMD->m_deadband = attVal;
						}
						else if (_tcsicmp(attName, _T("ReqType")) == 0)
						{
							pMD->m_reqType = attVal;
							expandNamespace(pMD->m_reqType);
						}
						else if (_tcsicmp(attName, _T("RequestedSamplingRate")) == 0)
						{
							pMD->m_reqSamplingRate = attVal;
						}
						else if (_tcsicmp(attName, _T("EnableBuffering")) == 0)
						{
							pMD->m_enableBuffering = attVal;
						}

						break;
					}
				}

				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				m_charDataType = SODASXMLMETHODDATA_TYPE_ITEM;
				m_elementStack.push(name);
			}
			else if ((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Value")) == 0) &&
					 (m_charDataType == SODASXMLMETHODDATA_TYPE_ITEM) &&
					 (getExpectedMethodState() == SODASXMLMETHOD_STATE_WRITE))
			{
				BOOL haveType = FALSE;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("http://www.w3.org/2001/XMLSchema-instance|type")) == 0)
					{
						m_pCurMethodData->m_value.m_datatype = attVal;
						expandNamespace(m_pCurMethodData->m_value.m_datatype);

						if (_tcsncmp(m_pCurMethodData->m_value.m_datatype, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Array"), 53) != 0)
						{
							m_pCurMethodData->m_value.m_simpleType = TRUE;
							m_pCurSimpleXmlValue = &m_pCurMethodData->m_value;
							m_charDataType = SODASXMLMETHODDATA_TYPE_VALUE;
						}
						else
						{
							m_pCurMethodData->m_value.m_simpleType = FALSE;

							if (!m_arrayStack)
							{
								m_arrayStack.create();
							}
							else
							{
								m_arrayStack.removeAll();
							}

							m_pCurMethodData->m_value.m_array.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
							m_arrayStack.push((void*)&m_pCurMethodData->m_value);
							setObjectState(SODASXMLMETHOD_STATE_VALUEARRAY);
						}

						m_elementStack.push(name);
						haveType = TRUE;
					}
				}

				if (!haveType)
				{
					setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid value; no type")));
				}
			}
			else if ((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Quality")) == 0) &&
					 (m_charDataType == SODASXMLMETHODDATA_TYPE_ITEM) &&
					 (getExpectedMethodState() == SODASXMLMETHOD_STATE_WRITE))
			{
				m_charDataType = SODASXMLMETHODDATA_TYPE_QUALITY;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("QualityField")) == 0)
					{
						m_pCurMethodData->m_quality = attVal;
					}
					else if (_tcsicmp(attName, _T("LimitField")) == 0)
					{
						m_pCurMethodData->m_qualityLimit = attVal;
					}
					else if (_tcsicmp(attName, _T("VendorField")) == 0)
					{
						m_pCurMethodData->m_qualityVendor = attVal;
					}
				}

				m_elementStack.push(name);
			}
			else if ((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|DiagnosticInfo")) == 0) &&
					 (m_charDataType == SODASXMLMETHODDATA_TYPE_ITEM) &&
					 (getExpectedMethodState() == SODASXMLMETHOD_STATE_WRITE))
			{
				m_charDataType = SODASXMLMETHODDATA_TYPE_DIAGNOSTICINFO;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid element on call: %s"), name));
			}

			break;

		case SODASXMLMETHOD_STATE_VALUEARRAY:
		{
			SOCmnString arrElDT;
			BOOL haveType = FALSE;
			SOCmnXmlValue* pCurArray = (SOCmnXmlValue*)m_arrayStack.getHead();
			SOCmnXmlValue* pXmlVal;
			SOCMN_ALLOCATE_OBJECT(pXmlVal, SOCmnXmlValue)

			if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|anyType")) != 0)
			{
				arrElDT = name;
				arrElDT.replace(_T("http://opcfoundation.org/webservices/XMLDA/1.0/|"), _T("http://www.w3.org/2001/XMLSchema|"));
				pXmlVal->m_datatype = arrElDT;
				pXmlVal->m_simpleType = TRUE;
				m_pCurSimpleXmlValue = pXmlVal;
				pCurArray->m_array.add(pXmlVal);
				haveType = TRUE;
			}
			else
			{
				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("http://www.w3.org/2001/XMLSchema-instance|type")) == 0)
					{
						arrElDT = attVal;
						expandNamespace(arrElDT);
						pXmlVal->m_datatype = arrElDT;
						if (_tcsncmp(arrElDT, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Array"), 53) != 0)						
						{
							pXmlVal->m_simpleType = TRUE;
							m_pCurSimpleXmlValue = pXmlVal;
							pCurArray->m_array.add(pXmlVal);
						}
						else
						{
							m_arrayStack.push((void*)pCurArray);
							pXmlVal->m_simpleType = FALSE;
							pXmlVal->m_array.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
							pCurArray->m_array.add(pXmlVal);
							setObjectState(SODASXMLMETHOD_STATE_VALUEARRAY);
						}

						haveType = TRUE;
						m_arrayStack.push((void*)pXmlVal);
					}
				}
			}

			if (haveType)
			{
				m_elementStack.push(name);
				m_charDataType = SODASXMLMETHODDATA_TYPE_VALUEARRAY;
			}
			else
			{
				delete pXmlVal;
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Invalid array datatype: %s"), name));
			}
		}
		break;

		default:
			SOSrvXmlMethod::handleStartElement(nameA, atts);
			break;
		}
	}
}

void SODaSXmlMethod::handleEndElement(LPCSTR nameA)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("E: %s"), name));

	switch (m_charDataType)
	{
	case SODASXMLMETHODDATA_TYPE_VALUE:
	case SODASXMLMETHODDATA_TYPE_QUALITY:
	case SODASXMLMETHODDATA_TYPE_DIAGNOSTICINFO:
		m_charDataType = SODASXMLMETHODDATA_TYPE_ITEM;
		break;

	case SODASXMLMETHODDATA_TYPE_VALUEARRAY:
		m_charDataType = SODASXMLMETHODDATA_TYPE_VALUE;
		break;

	default:
		m_charDataType = SODASXMLMETHODDATA_TYPE_NONE;
		break;
	}

	if ((getObjectState() == SODASXMLMETHOD_STATE_VALUEARRAY) &&
		((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|Value")) == 0)))
	{
		setObjectState(SODASXMLMETHOD_STATE_ITEMLIST);
	}

	if ((getObjectState() == SODASXMLMETHOD_STATE_ITEMLIST) &&
		((_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|ItemList")) == 0)))
	{
		setObjectState(SODASXMLMETHOD_SUBSCRIBE);
	}

	if (_tcsicmp(name, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|anyType")) == 0)
		if (m_arrayStack.isInit())
		{
			m_arrayStack.pop();
		}

	SOSrvXmlMethod::handleEndElement(nameA);
}

void SODaSXmlMethod::handleCharacterData(LPCSTR sA, INT len)
{
	USES_CONVERSION;
	SOCmnString c;
	SODaSXmlMethodData* pMD;
	c.setCHAR(sA, len);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("C: %s"), (LPCTSTR)c));

	switch (getObjectState())
	{
	case SODASXMLMETHOD_STATE_SUBSCRIPTIONPOLLEDREFRESH:
		if (m_charDataType == SODASXMLMETHODDATA_TYPE_SERVERSUBHANDLE)
		{
			SOCMN_ALLOCATE_OBJECT(pMD, SODaSXmlMethodData)
			pMD->m_type = SODASXMLMETHODDATA_TYPE_SERVERSUBHANDLE;
			pMD->m_serverSubHandle = c;
			m_methodData.add(pMD);
		}

		break;

	case SODASXMLMETHOD_STATE_BROWSE:
		if (m_charDataType == SODASXMLMETHODDATA_TYPE_PROPERTYNAME)
		{
			if (includesNamespacePrefix(c))
			{
				// property name with namespace
				expandNamespace(c);
				removeOPCXMLDANamespace(c); // remove default OPC XML DA NS
			}

			SOCMN_ALLOCATE_OBJECT(pMD, SODaSXmlMethodData)
			pMD->m_type = SODASXMLMETHODDATA_TYPE_PROPERTYNAME;
			SOCmnStringEx::setXMLString(c, pMD->m_propertyName);
			m_methodData.add(pMD);
		}

		break;

	case SODASXMLMETHOD_STATE_GETPROPERTIES:
		if (m_charDataType == SODASXMLMETHODDATA_TYPE_PROPERTYNAME)
		{
			if (includesNamespacePrefix(c))
			{
				// property name with namespace
				expandNamespace(c);
				removeOPCXMLDANamespace(c); // remove default OPC XML DA NS
			}

			SOCMN_ALLOCATE_OBJECT(pMD, SODaSXmlMethodData)
			pMD->m_type = SODASXMLMETHODDATA_TYPE_PROPERTYNAME;
			SOCmnStringEx::setXMLString(c, pMD->m_propertyName);
			m_methodData.add(pMD);
		}

		break;

	case SODASXMLMETHOD_STATE_ITEMLIST:
		if (m_charDataType == SODASXMLMETHODDATA_TYPE_VALUE)
		{
			m_pCurMethodData->m_value.m_value += c;
		}

		break;

	case SODASXMLMETHOD_STATE_VALUEARRAY:
	{
		if (m_charDataType == SODASXMLMETHODDATA_TYPE_VALUEARRAY)
		{
			m_pCurSimpleXmlValue->m_value += c;
		}
	}
	break;
	}
}


BOOL SODaSXmlMethod::removeOPCXMLDANamespace(OUT IN SOCmnString& szXML)
{
	static LPCTSTR opcNSstart = _T("http://opcfoundation.org/webservices/");
	static WORD opcNStartLen = 37;
	LPCTSTR pSZXML = (LPCTSTR)szXML;

	if (_tcsncmp(pSZXML, opcNSstart, opcNStartLen) == 0)
	{
		if (_tcsncmp(pSZXML + opcNStartLen, _T("XMLDA/1.0/"), 10) == 0)
		{
			szXML = szXML.right(szXML.getLength() - opcNStartLen - 10 - 1);
			return TRUE;
		}

		if (_tcsncmp(pSZXML + opcNStartLen, _T("OPCDA/"), 6) == 0)
		{
			// RC1 1.8 Namespace
			szXML = szXML.right(szXML.getLength() - opcNStartLen - 6 - 1);
			return TRUE;
		}
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
// SODaSXmlMethodData                                                         |
//-----------------------------------------------------------------------------

SODaSXmlMethodData::SODaSXmlMethodData(void)
{
	m_type = SODASXMLMETHODDATA_TYPE_NONE;
	m_result = S_OK;
}


SODaSXmlMethodData::~SODaSXmlMethodData(void)
{
	m_value.m_array.destroy();
}

#endif // SOFEATURE_SOAP

#endif // SODAS

