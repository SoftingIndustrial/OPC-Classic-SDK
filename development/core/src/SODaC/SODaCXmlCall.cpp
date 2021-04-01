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
//                           OPC Toolkit - SODAC                              |
//                                                                            |
//  Filename    : SODaCXmlCall.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC XML-DA client                                           |
//                - SODaCXmlCall: OPC XML-DA HTTP call class                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_SOAP

#include "SODaCXmlCall.h"
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SODaCXmlCall                                                               |
//-----------------------------------------------------------------------------
static LPCTSTR s_xmlStart = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://opcfoundation.org/webservices/XMLDA/1.0/\">");
static LPCTSTR s_xmlEnd = _T("</soap:Envelope>");

SODaCXmlCall::SODaCXmlCall(void)
{
	m_pRequestOptions = NULL;
	m_soapMethod = 0;
}

SODaCXmlCall::SODaCXmlCall(IN SOCltHTTPConnection* pCon)
{
	m_pRequestOptions = NULL;
	m_soapMethod = 0;
	setHTTPConnection(pCon);
}

SODaCXmlCall::~SODaCXmlCall(void)
{
	if (m_pRequestOptions)
	{
		delete m_pRequestOptions;
		m_pRequestOptions = NULL;
	}

	m_group.release();
}


void SODaCXmlCall::getRequestOptionsXML(IN SODaCXmlRequestOptions* pOptions, OUT SOCmnStringEx& reqOptions)
{
	reqOptions.empty();

	if (!pOptions)
	{
		pOptions = getRequestOptions();
	}

	if (pOptions)
	{
		SOCmnStringEx option;
		reqOptions.getBuffer(128);
		reqOptions = _T("<Options");

		if (pOptions->m_returnErrorText == FALSE)
		{
			option = _T(" ReturnErrorText=\"false\"");
			reqOptions += option;
		}

		if (pOptions->m_returnDiagnosticInfo == TRUE)
		{
			option = _T(" ReturnDiagnosticInfo=\"true\"");
			reqOptions += option;
		}

		if (pOptions->m_returnItemTime == TRUE)
		{
			option = _T(" ReturnItemTime=\"true\"");
			reqOptions += option;
		}

		if (pOptions->m_returnItemPath == TRUE)
		{
			option = _T(" ReturnItemPath=\"true\"");
			reqOptions += option;
		}

		if (pOptions->m_returnItemName == TRUE)
		{
			option = _T(" ReturnItemName=\"true\"");
			reqOptions += option;
		}

		if (pOptions->m_requestDeadline.isSet())
		{
			option.format(_T(" RequestDeadline=\"%s\""), (LPCTSTR)pOptions->m_requestDeadline.getXMLDateTime());
			reqOptions += option;
		}

		if (!pOptions->m_localeID.isEmpty())
		{
			option.format(_T(" LocaleID=\"%s\""), (LPCTSTR)pOptions->m_localeID);
			reqOptions += option;
		}

		reqOptions += _T("/>");
	}
}

void SODaCXmlCall::addDefaultHTTPAttributes(void)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	attributeList.add(_T("text/xml; charset=utf-8"), _T("Content-Type"));
	attributeList.add(_T("Keep-Alive"), _T("Connection"));
	SOCmnPointer<SOCltHTTPConnection> httpCon(getHTTPConnection());

	if (httpCon.isNotNull())
	{
		SOCmnPointer<SOCltServer> server = httpCon->getServer();

		if (server.isNotNull())
		{
			SOCmnString user = server->getUserName();

			if (!user.isEmpty())
			{
				SOCmnString userPwd;
				LPTSTR pUserPwd;
				SOCmnString authorization;
				SOCmnVariant charArray;
				SAFEARRAYBOUND charArrayDim;
				unsigned char* pCharArray;
				SOCmnString userPwd64;
				DWORD userPwdLen;
				DWORD i;
				USES_CONVERSION;
				userPwd.format(_T("%s:%s"), (LPCTSTR)user, (LPCTSTR)(server->getPassword()));
				userPwdLen = userPwd.getLength();
				pUserPwd = userPwd.getBuffer(0);
				charArrayDim.lLbound = 0;
				charArrayDim.cElements = userPwdLen;
				charArray.vt = VT_UI1 | VT_ARRAY;
				charArray.parray = SOCmnVariant::safeArrayCreate(VT_UI1, 1, &charArrayDim);
				SafeArrayAccessData(charArray.parray, (void**)&pCharArray);

				for (i = 0; i < userPwdLen; i++)
				{
					pCharArray[i] = (char)pUserPwd[i];
				}

				SafeArrayUnaccessData(charArray.parray);
				userPwd64 = SOCmnXmlValue::encodeBytes2base64(charArray);
				authorization.format(_T("Basic %s"), (LPCTSTR)userPwd64);
				attributeList.add(authorization, _T("Authorization"));
			}
		}
	}
}

HRESULT SODaCXmlCall::getStatus(IN SODaCXmlRequestOptions* pOptions)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx reqOptions;
	m_soapMethod = SODACXMLCALL_GETSTATUS;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/GetStatus\""), _T("SOAPAction"));

	// request options
	if (!pOptions)
	{
		pOptions = getRequestOptions();
	}

	if (pOptions)
		if (!pOptions->m_localeID.isEmpty())
		{
			reqOptions.format(_T("LocaleID=\"%s\""), (LPCTSTR)pOptions->m_localeID);
		}

	reqBody.getBuffer(512);
	reqBody.format(_T("%s<soap:Body><GetStatus %s/></soap:Body>%s"), s_xmlStart, (LPCTSTR)reqOptions, s_xmlEnd);
	return createRequestData(&reqBody);
} // getStatus

HRESULT SODaCXmlCall::subscribe(
	IN SODaCGroup* pGroup,
	IN SODaCXmlRequestOptions* pOptions,
	OUT DWORD* pItemCnt)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szSubscribeAttrs;
	SOCmnStringEx szItemList;
	SOCmnStringEx szDeadbandGrp;
	SOCmnStringEx szDeadband;
	SOCmnStringEx szItem;
	SOCmnStringEx szReqType;
	SOCmnStringEx exHlp1;
	exHlp1.getBuffer(128);
	SOCmnStringEx exHlp2;
	exHlp2.getBuffer(128);
	SOCmnString helper;
	DWORD pingRate;
	DWORD updateRate = pGroup->getUpdateRate();
	SOCmnList<SODaCItem> itemList;
	SOCmnListPosition pos;
	SODaCItem* pItem;
	DWORD itemCnt = 0;
	m_group = (SODaCGroup*)pGroup;
	m_group.addRef();
	m_soapMethod = SODACXMLCALL_SUBSCRIBE;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/Subscribe\""), _T("SOAPAction"));
	getRequestOptionsXML(pOptions, szReqOptions);

	// subscription attributes
	if (updateRate < SOCMN_TIME_4DAYS)
	{
		pingRate = updateRate * 5;
	}
	else
	{
		pingRate = SOCMN_TIME_40DAYS;
	}

	szSubscribeAttrs.format(_T("ReturnValuesOnReply =\"true\" SubscriptionPingRate=\"%lu\""), pingRate);

	// item list attributes
	if (pGroup->getDeadBand() != 0.0)
	{
		szDeadbandGrp.format(_T("Deadband=\"%g\" "), pGroup->getDeadBand());
	}

	szItemList.format(_T("<ItemList %sRequestedSamplingRate=\"%lu\">"), (LPCTSTR)szDeadbandGrp, updateRate);
	// item elements
	itemList = (SOCmnList<SODaCItem> *)pGroup->getLeafList();
	pos = itemList.getStartPosition();

	while (pos)
	{
		pItem = itemList.getNext(pos);
		szReqType.empty();
		szDeadband.empty();

		if (pItem->getTargetObjectState() == SOCltElement::started)
		{
			if (pItem->getReqDatatype() != VT_EMPTY)
			{
				szReqType.format(_T(" ReqType=\"%s\""), SOCmnVariant::getXMLDataType(pItem->getReqDatatype(), helper));
			}

			if (pItem->getDeadBand() != SODACITEM_NO_DEADBAND)
			{
				szDeadband.format(_T(" Deadband=\"%g\""), pItem->getDeadBand());
			}

			szItem.format(_T("<Items%s ItemPath=\"%s\" ItemName=\"%s\" ClientItemHandle=\"%8.8lX\"%s/>"),
						  (LPCTSTR)szReqType,
						  (LPCTSTR)SOCmnStringEx::getXMLString(pItem->getItemPath(), exHlp1),
						  (LPCTSTR)SOCmnStringEx::getXMLString(pItem->getItemID(), exHlp2),
						  (DWORD)pItem->getOpcHandle(),
						  (LPCTSTR)szDeadband);
			szItemList += szItem;
			itemCnt++;
		}
	}

	szItemList += _T("</ItemList>");
	reqBody.format(_T("%s<soap:Body><Subscribe %s>%s%s</Subscribe></soap:Body>%s"), s_xmlStart, (LPCTSTR)szSubscribeAttrs, (LPCTSTR)szReqOptions, (LPCTSTR)szItemList, s_xmlEnd);

	if (pItemCnt)
	{
		*pItemCnt = itemCnt;
	}

	return createRequestData(&reqBody);
}

HRESULT SODaCXmlCall::subscriptionCancel(
	IN SODaCXmlGroupImpl* pGroupImpl)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szSubscriptionCancelAttrs;
	m_soapMethod = SODACXMLCALL_SUBSCRIPTIONCANCEL;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/SubscriptionCancel\""), _T("SOAPAction"));
	szSubscriptionCancelAttrs.format(_T("ServerSubHandle=\"%s\""), (LPCTSTR)pGroupImpl->getServerSubHandle());
	reqBody.format(_T("%s<soap:Body><SubscriptionCancel %s/></soap:Body>%s"), s_xmlStart, (LPCTSTR)szSubscriptionCancelAttrs, s_xmlEnd);
	return createRequestData(&reqBody);
}


HRESULT SODaCXmlCall::subscriptionPolledRefresh(
	IN SODaCXmlGroupImpl* pGroupImpl,
	IN BOOL useGroupsUpdateRateAndKeepAliveTime,
	IN SOCmnDateTime* pHoldTime,
	IN DWORD waitTime,
	IN BOOL returnAllItems,
	IN SODaCXmlRequestOptions* pOptions)
{
	SODaCGroup* pGroup = pGroupImpl->getObjectDaCGroup();

	if (!pGroup)
	{
		return E_FAIL;
	}

	m_group = (SODaCGroup*)pGroup;
	m_group.addRef();
	m_pGroupXmlImpl = pGroupImpl;
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pGroup->getParentBranch();
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szSubscrPRAttrs;
	SOCmnStringEx szServerSubHandles;
	SOCmnDateTime now;
	m_soapMethod = SODACXMLCALL_SUBSCRIPTIONPOLLEDREFRESH;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/SubscriptionPolledRefresh\""), _T("SOAPAction"));
	getRequestOptionsXML(pOptions, szReqOptions);

	if (useGroupsUpdateRateAndKeepAliveTime)
	{
		DWORD keepAliveTime = m_group->getKeepAliveTime();
		DWORD updateRate = m_group->getUpdateRate();

		if (keepAliveTime > updateRate)
		{
			waitTime = keepAliveTime - updateRate;
		}
		else
		{
			waitTime = 0;
		}

		pHoldTime = &now;
		now.now();
		now.addMicroseconds((ULONGLONG)(updateRate) * 1000);
	}

	if ((server.isNotNull()) && (pHoldTime))
	{
		pHoldTime->addMicroseconds(server->getTimeDifference());
	}

	if (pHoldTime)
	{
		szSubscrPRAttrs.format(_T("HoldTime=\"%s\" WaitTime=\"%lu\""), (LPCTSTR)pHoldTime->getXMLDateTime(), waitTime);
	}
	else
	{
		szSubscrPRAttrs.format(_T("WaitTime=\"%lu\""), waitTime);
	}

	if (returnAllItems)
	{
		szSubscrPRAttrs += _T(" ReturnAllItems=\"true\"");
	}

	szServerSubHandles.format(_T("<ServerSubHandles>%s</ServerSubHandles>"), m_pGroupXmlImpl->getServerSubHandle());
	reqBody.format(_T("%s<soap:Body><SubscriptionPolledRefresh %s>%s%s</SubscriptionPolledRefresh></soap:Body>%s"), s_xmlStart, (LPCTSTR)szSubscrPRAttrs, (LPCTSTR)szReqOptions, (LPCTSTR)szServerSubHandles, s_xmlEnd);
	return createRequestData(&reqBody);
}


HRESULT SODaCXmlCall::browse(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN LPCTSTR cp, IN DWORD maxElements, IN DWORD browseFilter, IN LPCTSTR elementNameFilter,
							 IN LPCTSTR vendorFilter, IN BOOL returnProperties, IN BOOL retrievePropertyValues, IN SOCmnStringList* pPropertyList, IN SODaCXmlRequestOptions* pOptions)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szCP;
	SOCmnStringEx szMaxElements;
	SOCmnStringEx szBrowseFilter;
	SOCmnStringEx szElementNameFilter;
	SOCmnStringEx szVendorFilter;
	SOCmnStringEx szReturnProperties;
	SOCmnStringEx szReturnErrorText;
	SOCmnStringEx szRetrievePropertyValues;
	SOCmnStringEx szBrowseAtts;
	SOCmnStringEx szBrowseProps;
	SOCmnString iPath;
	SOCmnStringEx exHlp1;
	exHlp1.getBuffer(128);
	SOCmnStringEx exHlp2;
	exHlp2.getBuffer(128);
	m_soapMethod = SODACXMLCALL_BROWSE;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/Browse\""), _T("SOAPAction"));

	// request options
	if (!pOptions)
	{
		pOptions = getRequestOptions();
	}

	if (pOptions)
	{
		if (!pOptions->m_localeID.isEmpty())
		{
			szReqOptions.format(_T(" LocaleID=\"%s\""), (LPCTSTR)pOptions->m_localeID);
		}

		if (pOptions->m_returnErrorText)
		{
			szReturnErrorText = _T(" ReturnErrorText=\"true\"");
		}
	}

	if (cp)
		if (_tcslen(cp) > 0)
		{
			szCP.format(_T(" ContinuationPoint=\"%s\""), (LPCTSTR)cp);
		}

	if (maxElements != 0)
	{
		szMaxElements.format(_T(" MaxElementsReturned=\"%lu\""), maxElements);
	}

	if (browseFilter != OPC_BROWSE_FILTER_ALL)
	{
		if (browseFilter == OPC_BROWSE_FILTER_BRANCHES)
		{
			szBrowseFilter = _T(" BrowseFilter=\"branch\"");
		}
		else
		{
			szBrowseFilter = _T(" BrowseFilter=\"item\"");
		}
	}

	if (elementNameFilter)
		if (_tcslen(elementNameFilter) > 0)
		{
			szElementNameFilter.format(_T(" ElementNameFilter=\"%s\""), (LPCTSTR)elementNameFilter);
		}

	if (vendorFilter)
		if (_tcslen(vendorFilter) > 0)
		{
			szVendorFilter.format(_T(" VendorFilter=\"%s\""), (LPCTSTR)vendorFilter);
		}

	if (returnProperties)
	{
		szReturnProperties = _T(" ReturnAllProperties=\"true\"");
	}

	if (retrievePropertyValues)
	{
		szRetrievePropertyValues = _T(" ReturnPropertyValues=\"true\"");
	}

	if (itemPath)
	{
		iPath = itemPath;
	}

	szBrowseAtts.format(_T("%s ItemName=\"%s\" ItemPath=\"%s\"%s%s%s%s%s%s%s%s"),
						(LPCTSTR)szReqOptions,
						(LPCTSTR)SOCmnStringEx::getXMLString(itemID, exHlp1),
						(LPCTSTR)SOCmnStringEx::getXMLString(iPath, exHlp2),
						(LPCTSTR)szCP,
						(LPCTSTR)szMaxElements,
						(LPCTSTR)szBrowseFilter,
						(LPCTSTR)szElementNameFilter,
						(LPCTSTR)szVendorFilter,
						(LPCTSTR)szReturnProperties,
						(LPCTSTR)szRetrievePropertyValues,
						(LPCTSTR)szReturnErrorText);

	if (pPropertyList)
	{
		SOCmnStringList propList(pPropertyList);
		SOCmnListPosition pos;
		SOCmnString propName;
		SOCmnStringEx szProp;
		pos = propList.getStartPosition();

		while (pos)
		{
			propName = propList.getNext(pos);

			if (_tcsncmp(propName, _T("http://"), 7) != 0)
			{
				szProp.format(_T("<PropertyNames>%s</PropertyNames>"), (LPCTSTR)propName);
			}
			else
			{
				int nsEnd = propName.find(_T('|'));
				SOCmnString ns = propName.left(nsEnd - 1);
				SOCmnString nsPropName = (LPCTSTR)propName + nsEnd + 1;
				exHlp1.empty();
				szProp.format(_T("<PropertyNames xmlns:cpn=\"%s\">cpn:%s</PropertyNames>"),
							  (LPCTSTR)ns,
							  (LPCTSTR)SOCmnStringEx::getXMLString(nsPropName, exHlp1));
			}

			szBrowseProps += szProp;
		}
	}

	reqBody.getBuffer(512);
	reqBody.format(_T("%s<soap:Body><Browse%s>%s</Browse></soap:Body>%s"), s_xmlStart, (LPCTSTR)szBrowseAtts, (LPCTSTR)szBrowseProps, s_xmlEnd);
	return createRequestData(&reqBody);
} // browse

HRESULT SODaCXmlCall::getProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN SOCmnStringList* pItemIDList, IN SOCmnStringList* pItemPathList, IN BOOL retrievePropertyValues, IN BOOL returnAllProperties, IN SOCmnStringList* pPropertyList,
									IN SODaCXmlRequestOptions* pOptions)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szGPAtts;
	SOCmnStringEx szGPProps;
	SOCmnStringEx szGPItems;
	SOCmnStringEx szReturnProperties;
	SOCmnStringEx szReturnErrorText;
	SOCmnStringEx szRetrievePropertyValues;
	SOCmnStringEx szProp;
	SOCmnStringEx szItem;
	SOCmnListPosition posP;
	SOCmnListPosition posI;
	SOCmnString iPath;
	SOCmnStringEx exHlp1;
	exHlp1.getBuffer(128);
	SOCmnStringEx exHlp2;
	exHlp2.getBuffer(128);
	m_soapMethod = SODACXMLCALL_GETPROPERTIES;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/GetProperties\""), _T("SOAPAction"));

	// request options
	if (!pOptions)
	{
		pOptions = getRequestOptions();
	}

	if (pOptions)
	{
		if (!pOptions->m_localeID.isEmpty())
		{
			szReqOptions.format(_T(" LocaleID=\"%s\""), (LPCTSTR)pOptions->m_localeID);
		}

		if (pOptions->m_returnErrorText)
		{
			szReturnErrorText = _T(" ReturnErrorText=\"true\"");
		}
	}

	if (returnAllProperties)
	{
		szReturnProperties = _T(" ReturnAllProperties=\"true\"");
	}

	if (retrievePropertyValues)
	{
		szRetrievePropertyValues = _T(" ReturnPropertyValues=\"true\"");
	}

	szGPAtts.format(_T("%s%s%s%s"), (LPCTSTR)szReqOptions, (LPCTSTR)szReturnProperties, (LPCTSTR)szRetrievePropertyValues, (LPCTSTR)szReturnErrorText);

	if (itemID)
	{
		if (itemPath)
		{
			iPath = itemPath;
		}

		szGPItems.format(_T("<ItemIDs ItemPath=\"%s\" ItemName=\"%s\"/>"),
						 (LPCTSTR)SOCmnStringEx::getXMLString(iPath, exHlp1),
						 (LPCTSTR)SOCmnStringEx::getXMLString(itemID, exHlp2));
	}
	else if (pItemIDList)
	{
		SOCmnString iID;
		SOCmnListPosition posH;
		posI = pItemIDList->getStartPosition();

		if (pItemPathList)
		{
			posH = pItemPathList->getStartPosition();
		}
		else
		{
			posH = (SOCmnListPosition)1;
		}

		while ((posI) && (posH))
		{
			iID = pItemIDList->getNext(posI);

			if (pItemPathList)
			{
				iPath = pItemPathList->getNext(posH);
			}

			exHlp1.empty();
			exHlp2.empty();
			szItem.format(_T("<ItemIDs ItemPath=\"%s\" ItemName=\"%s\"/>"),
						  (LPCTSTR)SOCmnStringEx::getXMLString(iPath, exHlp1),
						  (LPCTSTR)SOCmnStringEx::getXMLString(iID, exHlp2));
			szGPItems += szItem;
		}
	}

	if ((!returnAllProperties) && (pPropertyList))
	{
		SOCmnString propName;
		posP = pPropertyList->getStartPosition();

		while (posP)
		{
			propName = pPropertyList->getNext(posP);

			if (_tcsncmp(propName, _T("http://"), 7) != 0)
			{
				szProp.format(_T("<PropertyNames>%s</PropertyNames>"), (LPCTSTR)propName);
			}
			else
			{
				int nsEnd = propName.find(_T('|'));
				SOCmnString ns = propName.left(nsEnd);
				SOCmnString nsPropName = (LPCTSTR)propName + nsEnd + 1;
				exHlp1.empty();
				szProp.format(_T("<PropertyNames xmlns:cpn=\"%s\">cpn:%s</PropertyNames>"),
							  (LPCTSTR)ns,
							  (LPCTSTR)SOCmnStringEx::getXMLString(nsPropName, exHlp1));
			}

			szGPProps += szProp;
		}
	}

	reqBody.getBuffer(512);
	reqBody.format(_T("%s<soap:Body><GetProperties%s>%s%s</GetProperties></soap:Body>%s"), s_xmlStart, (LPCTSTR)szGPAtts, (LPCTSTR)szGPItems, (LPCTSTR)szGPProps, s_xmlEnd);
	return createRequestData(&reqBody);
} // getProperties


HRESULT SODaCXmlCall::write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues, IN WORD* pQualities, IN SOCmnDateTime* pTimestamps, IN SODaCXmlRequestOptions* pOptions)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szGPItems;
	SOCmnStringEx szItem;
	SOCmnStringEx szT, szV;
	SOCmnStringEx itemPath;
	SOCmnStringEx timestamp;
	SOCmnStringEx quality;
	DWORD i;
	SOCmnStringEx exHlp1;
	exHlp1.getBuffer(128);
	SOCmnStringEx exHlp2;
	exHlp2.getBuffer(128);
	m_soapMethod = SODACXMLCALL_WRITE;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/Write\""), _T("SOAPAction"));
	getRequestOptionsXML(pOptions, szReqOptions);

	for (i = 0; i < count; i++)
	{
		// for every item
		SOCmnVariant::getXMLValue(pValues[i], szT, szV);

		if (pItemPaths)
		{
			itemPath = pItemPaths[i];
		}

		if (pTimestamps)
		{
			timestamp.empty();

			if (pTimestamps[i].isSet())
			{
				timestamp.format(_T(" Timestamp=\"%s\""), (LPCTSTR)pTimestamps[i].getXMLDateTime());
			}
		}

		if (pQualities)
		{
			quality.empty();

			if (pQualities[i] != SODAC_QUALITY_NONE)
			{
				quality.getBuffer(128);
				getXMLQuality(pQualities[i], TRUE, quality);
			}
		}

		exHlp1.empty();
		exHlp2.empty();
		szItem.format(_T("<Items ItemPath=\"%s\" ItemName=\"%s\"%s><Value xsi:type=\"%s\">%s</Value>%s</Items>"),
					  (LPCTSTR)SOCmnStringEx::getXMLString(itemPath, exHlp1),
					  (LPCTSTR)SOCmnStringEx::getXMLString(pItemIDs[i], exHlp2),
					  (LPCTSTR)timestamp,
					  (LPCTSTR)szT,
					  (LPCTSTR)szV,
					  (LPCTSTR)quality);
		szGPItems += szItem;
	}

	reqBody.getBuffer(512);
	reqBody.format(_T("%s<soap:Body><Write ReturnValuesOnReply=\"false\">%s<ItemList>%s</ItemList></Write></soap:Body>%s"), s_xmlStart, (LPCTSTR)szReqOptions, (LPCTSTR)szGPItems, s_xmlEnd);
	return createRequestData(&reqBody);
}

HRESULT SODaCXmlCall::read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, IN SODaCXmlRequestOptions* pOptions)
{
	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnStringEx reqBody;
	SOCmnStringEx szReqOptions;
	SOCmnStringEx szGPItems;
	SOCmnStringEx szItem;
	SOCmnStringEx itemPath;
	DWORD i;
	SOCmnStringEx exHlp1;
	exHlp1.getBuffer(128);
	SOCmnStringEx exHlp2;
	exHlp2.getBuffer(128);
	m_soapMethod = SODACXMLCALL_READ;
	setHTTPMethod(SOCMNHTTP_METHOD_POST);
	addDefaultHTTPAttributes();
	attributeList.add(_T("\"http://opcfoundation.org/webservices/XMLDA/1.0/Read\""), _T("SOAPAction"));
	getRequestOptionsXML(pOptions, szReqOptions);

	for (i = 0; i < count; i++)
	{
		// for every item
		if (pItemPaths)
		{
			itemPath = pItemPaths[i];
		}

		exHlp1.empty();
		exHlp2.empty();
		szItem.format(_T("<Items ItemPath=\"%s\" ItemName=\"%s\"/>"),
					  (LPCTSTR)SOCmnStringEx::getXMLString(itemPath, exHlp1),
					  (LPCTSTR)SOCmnStringEx::getXMLString(pItemIDs[i], exHlp2));
		szGPItems += szItem;
	}

	reqBody.getBuffer(512);
	reqBody.format(_T("%s<soap:Body><Read>%s<ItemList MaxAge=\"%lu\">%s</ItemList></Read></soap:Body>%s"), s_xmlStart, (LPCTSTR)szReqOptions, maxAge, (LPCTSTR)szGPItems, s_xmlEnd);
	return createRequestData(&reqBody);
}


HRESULT SODaCXmlCall::getResponse(IN BYTE methodType, IN BYTE resonseType, IN LPCTSTR responseText, OUT SODaCXmlResponse** ppRes)
{
	if (m_soapMethod != methodType)
	{
		return E_FAIL;
	}

	if (getHTTPStatusCode() != SOCMNHTTP_CODE_OK)
	{
		return (OPC_E_HTTP_STATUS_BASE | getHTTPStatusCode());
	}

	DWORD bodyLen;
	BYTE* pBody;
	getResponseBody(&bodyLen, &pBody);

	if ((!pBody) || (bodyLen == 0))
	{
		return E_FAIL;
	}

	SOCmnPointer<SODaCXmlResponse> mth;
	HRESULT res = S_OK;
	SOCMN_ALLOCATE_OBJECT_C(mth, SODaCXmlResponse(), (SODaCXmlResponse*));
	mth->setExpectedMethod(resonseType, responseText);
	DWORD findBOM = 0; // UTF Byte order mask

	for (findBOM = 0; findBOM < bodyLen; findBOM++)
	{
		if (pBody[findBOM] == '<')
		{
			break;
		}
	}

	if (findBOM != 0)
	{
		// UTF8 BOM: EF BB BF
		if (findBOM != 3)
		{
			return E_FAIL;
		}

		if ((pBody[0] != 0xEF) || (pBody[1] != 0xBB) || (pBody[2] != 0xBF))
		{
			return E_FAIL;
		}
	}

	if ((mth->parse((LPCSTR)pBody + findBOM, bodyLen - findBOM)) && (mth->getObjectState() != SOCMNXMLMETHOD_STATE_ERROR))
	{
		doTimeSynchonisation(mth);

		if (ppRes)
		{
			*ppRes = mth;
			(*ppRes)->addRef();
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
}

HRESULT SODaCXmlCall::getStatusResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_GETSTATUS, SODACXMLRESPONSE_STATE_GETSTATUSRESPONSE, _T("GetStatusResponse"), ppRes);
}

HRESULT SODaCXmlCall::getSubscribeResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_SUBSCRIBE, SODACXMLRESPONSE_STATE_SUBSCRIBERESPONSE, _T("SubscribeResponse"), ppRes);
}

HRESULT SODaCXmlCall::getSubscriptionCancelResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_SUBSCRIPTIONCANCEL, SODACXMLRESPONSE_STATE_SUBSCRIPTIONCANCELRESPONSE, _T("SubscriptionCancelResponse"), ppRes);
}

HRESULT SODaCXmlCall::getSubscriptionPolledRefreshResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_SUBSCRIPTIONPOLLEDREFRESH, SODACXMLRESPONSE_STATE_SUBSCRIPTIONPOLLEDREFRESHRESPONSE, _T("SubscriptionPolledRefreshResponse"), ppRes);
}

HRESULT SODaCXmlCall::getBrowseResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_BROWSE, SODACXMLRESPONSE_STATE_BROWSERESPONSE, _T("BrowseResponse"), ppRes);
}

HRESULT SODaCXmlCall::getGetPropertiesResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_GETPROPERTIES, SODACXMLRESPONSE_STATE_GETPROPERTIESRESPONSE, _T("GetPropertiesResponse"), ppRes);
}

HRESULT SODaCXmlCall::getWriteResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_WRITE, SODACXMLRESPONSE_STATE_WRITERESPONSE, _T("WriteResponse"), ppRes);
}

HRESULT SODaCXmlCall::getReadResponse(OUT SODaCXmlResponse** ppRes)
{
	return getResponse(SODACXMLCALL_READ, SODACXMLRESPONSE_STATE_READRESPONSE, _T("ReadResponse"), ppRes);
}


void SODaCXmlCall::doTimeSynchonisation(SODaCXmlResponse* pRes)
{
	// calculate time difference between client and server
	SOCmnDateTime replyTime;
	SOCmnString replyTimeSZ;
	SOCmnDateTime sendTime;
	SOCmnString sendTimeSZ;
	SOCmnDateTime now;
	SOCmnDateTime send;
	LONGLONG timeDiffReply = 0;
	LONGLONG timeDiffSend = 0;
	SOCmnStringList attrList(pRes->getResponseOptions());
	now.now();
	send = now;
	send.addMicroseconds(-1000 * (LONGLONG)(GetTickCount() - getActivationTime()));
	sendTimeSZ = attrList.findKey(_T("RcvTime"));
	replyTimeSZ = attrList.findKey(_T("ReplyTime"));
	SOCmnPointer<SODaCGroup> group(getGroup());

	if (group.isNotNull())
	{
		SOCmnPointer<SODaCServer> server = (SODaCServer*)group->getParentBranch();

		if (server.isNotNull())
		{
			if (!replyTimeSZ.isEmpty())
			{
				replyTime.setXMLDateTime(replyTimeSZ);
				timeDiffReply = replyTime.getMicroseconds() - now.getMicroseconds();
			}

			if (!sendTimeSZ.isEmpty())
			{
				sendTime.setXMLDateTime(sendTimeSZ);
				timeDiffSend = sendTime.getMicroseconds() - send.getMicroseconds();
			}

			server->setTimeDifference(timeDiffSend, timeDiffReply);
		}
	}
}

void SODaCXmlCall::onComplete(void)
{
	if (m_soapMethod == SODACXMLCALL_SUBSCRIPTIONPOLLEDREFRESH)
	{
		SOCmnPointer<SODaCGroup> group = getGroup();

		if ((m_pGroupXmlImpl) && (group.isNotNull()))
		{
			SOCltWorkerThread* pThread = group->getWorkerThread();
			m_pGroupXmlImpl->setPolledRefreshReply(this);

			if (pThread)
			{
				pThread->wakeUpCyclic();
			}
		}
	}
}


HRESULT SODaCXmlCall::getErrorCodeFromXML(IN LPCTSTR errCodeSZ)
{
	SOCmnString errSZ(errCodeSZ);
	HRESULT error = E_FAIL;
	errSZ.makeUpper();

	switch (_tcslen(errSZ))
	{
	case 0:
		error = S_OK;
		break;

	case 4:
		if (_tcscmp(errSZ, _T("S_OK")) == 0)
		{
			error = S_OK;
		}

		break;

	case 6:
		if (_tcscmp(errSZ, _T("E_FAIL")) == 0)
		{
			error = E_FAIL;
		}
		else if (_tcscmp(errSZ, _T("E_BUSY")) == 0)
		{
			error = OPC_E_BUSY_XMLDA;
		}

		break;

	case 7:
		if (_tcscmp(errSZ, _T("S_CLAMP")) == 0)
		{
			error = OPC_S_CLAMP;
		}
		else if (_tcscmp(errSZ, _T("E_RANGE")) == 0)
		{
			error = OPC_E_RANGE;
		}

		break;

	case 8:
		if (_tcscmp(errSZ, _T("E_PUBLIC")) == 0)
		{
			error = OPC_E_PUBLIC;
		}

		break;

	case 9:
		if (_tcscmp(errSZ, _T("E_BADTYPE")) == 0)
		{
			error = OPC_E_BADTYPE;
		}

		break;

	case 10:
		if (_tcscmp(errSZ, _T("E_READONLY")) == 0)
		{
			error = OPC_E_READONLY;
		}
		else if (_tcscmp(errSZ, _T("E_TIMEDOUT")) == 0)
		{
			error = OPC_E_TIMEDOUT;
		}

		break;

	case 11:
		if (_tcscmp(errSZ, _T("E_WRITEONLY")) == 0)
		{
			error = OPC_E_WRITEONLY;
		}

		break;

	case 12:
		if (_tcscmp(errSZ, _T("E_INVALIDARG")) == 0)
		{
			error = E_INVALIDARG;
		}
		else if (_tcscmp(errSZ, _T("E_INVALIDPID")) == 0)
		{
			error = OPC_E_INVALID_PID;
		}
		else if (_tcscmp(errSZ, _T("E_RATENOTSET")) == 0)
		{
			error = OPC_E_RATENOTSET;
		}

		break;

	case 13:
		if (_tcscmp(errSZ, _T("E_OUTOFMEMORY")) == 0)
		{
			error = E_OUTOFMEMORY;
		}
		else if (_tcscmp(errSZ, _T("E_SERVERSTATE")) == 0)
		{
			error = OPC_E_SERVERSTATE;
		}
		else if (_tcscmp(errSZ, _T("E_NOBUFFERING")) == 0)
		{
			error = OPC_E_NOBUFFERING;
		}

		break;

	case 14:
		if (_tcscmp(errSZ, _T("E_NOTSUPPORTED")) == 0)
		{
			error = OPC_E_NOTSUPPORTED;
		}

		break;

	case 15:
		if (_tcscmp(errSZ, _T("E_ACCESS_DENIED")) == 0)
		{
			error = E_ACCESSDENIED;
		}
		else if (_tcscmp(errSZ, _T("E_INVALIDFILTER")) == 0)
		{
			error = OPC_E_INVALIDFILTER;
		}

		break;

	case 16:
		if (_tcscmp(errSZ, _T("E_NOSUBSCRIPTION")) == 0)
		{
			error = OPC_E_NOSUBSCRIPTION;
		}
		else if (_tcscmp(errSZ, _T("E_DEADBANDNOTSET")) == 0)
		{
			error = OPC_E_DEADBANDNOTSET;
		}

		break;

	case 17:
		if (_tcscmp(errSZ, _T("E_INVALIDITEMNAME")) == 0)
		{
			error = OPC_E_INVALIDITEMNAME;
		}
		else if (_tcscmp(errSZ, _T("E_UNKNOWNITEMNAME")) == 0)
		{
			error = OPC_E_UNKNOWNITEMNAME;
		}
		else if (_tcscmp(errSZ, _T("E_INVALIDHOLDTIME")) == 0)
		{
			error = OPC_E_INVALIDHOLDTIME;
		}
		else if (_tcscmp(errSZ, _T("S_UNSUPPORTEDRATE")) == 0)
		{
			error = OPC_S_UNSUPPORTEDRATE;
		}
		else if (_tcscmp(errSZ, _T("E_INVALIDITEMPATH")) == 0)
		{
			error = OPC_E_INVALIDITEMPATH;
		}
		else if (_tcscmp(errSZ, _T("E_UNKNOWNITEMPATH")) == 0)
		{
			error = OPC_E_UNKNOWNITEMPATH;
		}

		break;

	case 19:
		if (_tcscmp(errSZ, _T("S_DATAQUEUEOVERFLOW")) == 0)
		{
			error = OPC_S_DATAQUEUEOVERFLOW;
		}

		break;

	case 22:
		if (_tcscmp(errSZ, _T("E_DEADBANDNOTSUPPORTED")) == 0)
		{
			error = OPC_E_DEADBANDNOTSUPPORTED;
		}

		break;

	case 26:
		if (_tcscmp(errSZ, _T("E_INVALIDCONTINUATIONPOINT")) == 0)
		{
			error = OPC_E_INVALIDCONTINUATIONPOINT;
		}

		break;
	}

	return error;
}

void SODaCXmlCall::getXMLQuality(WORD quality, IN BOOL withElement, OUT SOCmnStringEx& xmlQuality)
{
	WORD q = quality & OPC_STATUS_MASK;
	WORD l = quality & OPC_LIMIT_MASK;

	if (withElement)
	{
		xmlQuality = _T("<Quality");
	}

	if (q != OPC_QUALITY_GOOD)
	{
		switch (q)
		{
		case OPC_QUALITY_BAD:
			xmlQuality += _T(" QualityField=\"bad\"");
			break;

		case OPC_QUALITY_CONFIG_ERROR:
			xmlQuality += _T(" QualityField=\"badConfigurationError\"");
			break;

		case OPC_QUALITY_NOT_CONNECTED:
			xmlQuality += _T(" QualityField=\"badNotConnected\"");
			break;

		case OPC_QUALITY_DEVICE_FAILURE:
			xmlQuality += _T(" QualityField=\"badDeviceFailure\"");
			break;

		case OPC_QUALITY_SENSOR_FAILURE:
			xmlQuality += _T(" QualityField=\"badSensorFailure\"");
			break;

		case OPC_QUALITY_LAST_KNOWN:
			xmlQuality += _T(" QualityField=\"badLastKnownValue\"");
			break;

		case OPC_QUALITY_COMM_FAILURE:
			xmlQuality += _T(" QualityField=\"badCommFailure\"");
			break;

		case OPC_QUALITY_OUT_OF_SERVICE:
			xmlQuality += _T(" QualityField=\"badOutOfService\"");
			break;

		case OPC_QUALITY_WAITING_FOR_INITIAL_DATA:
			xmlQuality += _T(" QualityField=\"badWaitingForInitialData\"");
			break;

		case OPC_QUALITY_UNCERTAIN:
			xmlQuality += _T(" QualityField=\"uncertain\"");
			break;

		case OPC_QUALITY_LAST_USABLE:
			xmlQuality += _T(" QualityField=\"uncertainLastUsableValue\"");
			break;

		case OPC_QUALITY_SENSOR_CAL:
			xmlQuality += _T(" QualityField=\"uncertainSensorNotAccurate\"");
			break;

		case OPC_QUALITY_EGU_EXCEEDED:
			xmlQuality += _T(" QualityField=\"uncertainEUExceeded\"");
			break;

		case OPC_QUALITY_SUB_NORMAL:
			xmlQuality += _T(" QualityField=\"uncertainSubNormal\"");
			break;

		case OPC_QUALITY_LOCAL_OVERRIDE:
			xmlQuality += _T(" QualityField=\"goodLocalOverride\"");
			break;
		}
	}

	if (l != OPC_LIMIT_OK)
	{
		switch (l)
		{
		case OPC_LIMIT_LOW:
			xmlQuality += _T(" LimitField=\"low\"");
			break;

		case OPC_LIMIT_HIGH:
			xmlQuality += _T(" LimitField=\"high\"");
			break;

		case OPC_LIMIT_CONST:
			xmlQuality += _T(" LimitField=\"constant\"");
			break;
		}
	}

	if (withElement)
	{
		xmlQuality += _T("/>");
	}
}

WORD SODaCXmlCall::getDCOMQuality(IN LPCTSTR quality, IN LPCTSTR limit, IN LPCTSTR /* vendor */)
{
	WORD q = 0;

	if (_tcslen(quality) != 0)
	{
		if ((quality[0] == _T('b')) || (quality[0] == _T('B')))
		{
			if (_tcsicmp(quality, _T("bad")) == 0)
			{
				q = OPC_QUALITY_BAD;
			}
			else if (_tcsicmp(quality, _T("badConfigurationError")) == 0)
			{
				q = OPC_QUALITY_CONFIG_ERROR;
			}
			else if (_tcsicmp(quality, _T("badNotConnected")) == 0)
			{
				q = OPC_QUALITY_NOT_CONNECTED;
			}
			else if (_tcsicmp(quality, _T("badDeviceFailure")) == 0)
			{
				q = OPC_QUALITY_DEVICE_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badSensorFailure")) == 0)
			{
				q = OPC_QUALITY_SENSOR_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badLastKnownValue")) == 0)
			{
				q = OPC_QUALITY_LAST_KNOWN;
			}
			else if (_tcsicmp(quality, _T("badCommFailure")) == 0)
			{
				q = OPC_QUALITY_COMM_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badOutOfService")) == 0)
			{
				q = OPC_QUALITY_OUT_OF_SERVICE;
			}
			else if (_tcsicmp(quality, _T("badWaitingForInitialData")) == 0)
			{
				q = OPC_QUALITY_WAITING_FOR_INITIAL_DATA;
			}
		}
		else if ((quality[0] == _T('u')) || (quality[0] == _T('U')))
		{
			if (_tcsicmp(quality, _T("uncertain")) == 0)
			{
				q = OPC_QUALITY_UNCERTAIN;
			}
			else if (_tcsicmp(quality, _T("uncertainLastUsableValue")) == 0)
			{
				q = OPC_QUALITY_LAST_USABLE;
			}
			else if (_tcsicmp(quality, _T("uncertainSensorNotAccurate")) == 0)
			{
				q = OPC_QUALITY_SENSOR_CAL;
			}
			else if (_tcsicmp(quality, _T("uncertainEUExceeded")) == 0)
			{
				q = OPC_QUALITY_EGU_EXCEEDED;
			}
			else if (_tcsicmp(quality, _T("uncertainSubNormal")) == 0)
			{
				q = OPC_QUALITY_SUB_NORMAL;
			}
			else
			{
				TCHAR shortQ[10] = {0};
				DWORD len = (DWORD) _tcslen(quality);
				if (len > 9)
				{
					memcpy((void*)shortQ, quality, 9 * sizeof(char));
					if (_tcsicmp(shortQ, _T("uncertain")) == 0)
					{
						q = OPC_QUALITY_UNCERTAIN;
					}
				}
			}
		}
		else if ((quality[0] == _T('g')) || (quality[0] == _T('G')))
		{
			if (_tcsicmp(quality, _T("good")) == 0)
			{
				q = OPC_QUALITY_GOOD;
			}
			else if (_tcsicmp(quality, _T("goodLocalOverride")) == 0)
			{
				q = OPC_QUALITY_LOCAL_OVERRIDE;
			}
			else
			{
				TCHAR shortQuality[5] = {0};
				DWORD len = (DWORD) _tcslen(quality);
				if (len > 4)
				{
					memcpy((void*)shortQuality, quality, 4 * sizeof(char));
					if (_tcsicmp(shortQuality, _T("good")) == 0)
					{
						q = OPC_QUALITY_GOOD;
					}
				}
			}
		}
	}
	else
	{
		q = OPC_QUALITY_GOOD;
	}

	if (_tcslen(limit) != 0)
	{
		if (_tcsicmp(limit, _T("low")) == 0)
		{
			q |= OPC_LIMIT_LOW;
		}
		else if (_tcsicmp(limit, _T("high")) == 0)
		{
			q |= OPC_LIMIT_HIGH;
		}
		else if (_tcsicmp(limit, _T("constant")) == 0)
		{
			q |= OPC_LIMIT_CONST;
		}
	}

	return q;
}


//-----------------------------------------------------------------------------
// SODaCXmlRequestOptions                                                     |
//-----------------------------------------------------------------------------

SODaCXmlRequestOptions::SODaCXmlRequestOptions(void)
{
	m_returnErrorText = TRUE;
	m_returnDiagnosticInfo = FALSE;
	m_returnItemTime = FALSE;
	m_returnItemPath = FALSE;
	m_returnItemName = FALSE;
}




//-----------------------------------------------------------------------------
// SODaCXmlResponse                                                           |
//-----------------------------------------------------------------------------

SODaCXmlResponse::SODaCXmlResponse(void)
{
	m_methodOptions.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	m_methodData.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
	m_arrayStack.create();
	m_pCurMethodData = NULL;
	m_charDataType = SODACXMLRESPONSEDATA_TYPE_NONE;
	m_fault = S_OK;
}

SODaCXmlResponse::~SODaCXmlResponse(void)
{
	m_methodOptions.destroy();
	m_methodData.destroy();
	m_arrayStack.destroy();
	SOCMN_FREE_MEMORY(m_pData)
}

void SODaCXmlResponse::handleStartElement(LPCSTR nameA, LPCSTR* atts)
{
	USES_CONVERSION;
#ifdef SOOS_WINDOWS
	_acp = CP_UTF8;
#endif
	LPCTSTR xmlName = A2CT(nameA);
	LPCTSTR elementName = xmlName;
	//  replace the namespace if any
#ifdef SOOS_WINDOWS

	if (_tcsncicmp(elementName, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|"), 48) == 0)
#else
	if (strncasecmp(elementName, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|"), 48) == 0)
#endif
	{
		elementName = elementName + 48;
	}

	LPCTSTR attName, attVal;
	DWORD i;
	_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA)
	{
		// trace elements
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("S: %s"), xmlName));

		for (i = 0; atts[i]; i += 2)
		{
			attName = A2CT(atts[i]);
			attVal = A2CT(atts[i + 1]);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("   %s = %s"), attName, attVal));
		}
	}

	if (getObjectState() < SOCMNXMLMETHOD_STATE_SOAP_BODY)
	{
		SOCmnXmlMethod::handleStartElement(nameA, atts);
	}
	else
	{
		switch (getObjectState())
		{
		case SOCMNXMLMETHOD_STATE_SOAP_BODY: // wait for method
			if (_tcsicmp(elementName, getExpectedMethodString()) == 0)
			{
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					// add the attributes to the method options
					m_methodOptions.add(attVal, attName);
				}

				setObjectState(getExpectedMethodState());
				m_elementStack.push(xmlName);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Not expected method %s: instead %s"), getExpectedMethodString(), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_GETSTATUSRESPONSE:
			if ((_tcsicmp(elementName, _T("GetStatusResult")) == 0) ||
				(_tcsicmp(elementName, _T("Status")) == 0))
			{
				// ReplyBase and Status
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName); // add the attributes to the method options
				}

				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("VendorInfo")) == 0)
			{
				m_elementStack.push(xmlName);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_VENDORINFO;
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_VENDORINFO;
				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
			}
			else if (_tcsicmp(elementName, _T("SupportedLocaleIDs")) == 0)
			{
				m_elementStack.push(xmlName);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_LOCALEID;
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_LOCALEID;
				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
			}
			else if (_tcsicmp(elementName, _T("SupportedInterfaceVersions")) == 0)
			{
				m_elementStack.push(xmlName);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_INTERFACEVERSION;
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_INTERFACEVERSION;
				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
			}
			else if (_tcsicmp(elementName, _T("StatusInfo")) == 0)
			{
				m_elementStack.push(xmlName);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_STATUSINFO;
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_STATUSINFO;
				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on get status call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_BROWSERESPONSE:
			if (_tcsicmp(elementName, _T("BrowseResult")) == 0)
			{
				// ReplyBase
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName); // add the attributes to the method options
				}

				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("Elements")) == 0)
			{
				// Elements
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_ELEMENT;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("Name")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_name);
					}
					else if (_tcsicmp(attName, _T("ItemName")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemName);
					}
					else if (_tcsicmp(attName, _T("ItemPath")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemPath);
					}
					else if (_tcsicmp(attName, _T("IsItem")) == 0)
					{
						pMD->m_isItem = XMLSTR2BOOL(attVal);
					}
					else if (_tcsicmp(attName, _T("HasChildren")) == 0)
					{
						pMD->m_hasChildren = XMLSTR2BOOL(attVal);
					}
				}

				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_ELEMENTS);
			}
			else if (_tcsicmp(elementName, _T("Errors")) == 0)
			{
				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_ERROR);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on browse call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_GETPROPERTIESRESPONSE:
			if (_tcsicmp(elementName, _T("GetPropertiesResult")) == 0)
			{
				// ReplyBase
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName); // add the attributes to the method options
				}

				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("PropertyLists")) == 0)
			{
				// PropertyLists
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_ELEMENT;

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
					else if (_tcsicmp(attName, _T("ResultID")) == 0)
					{
						pMD->m_resultID = attVal;
					}
				}

				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_PROPERTYLISTS);
			}
			else if (_tcsicmp(elementName, _T("Errors")) == 0)
			{
				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_ERROR);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on get properties call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_SUBSCRIBERESPONSE:
		case SODACXMLRESPONSE_STATE_SUBSCRIPTIONPOLLEDREFRESHRESPONSE:
		case SODACXMLRESPONSE_STATE_WRITERESPONSE:
		case SODACXMLRESPONSE_STATE_READRESPONSE:
			if ((_tcsicmp(elementName, _T("SubscribeResult")) == 0) ||
				(_tcsicmp(elementName, _T("SubscriptionPolledRefreshResult")) == 0) ||
				(_tcsicmp(elementName, _T("WriteResult")) == 0) ||
				(_tcsicmp(elementName, _T("ReadResult")) == 0))
			{
				// ReplyBase
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName); // add the attributes to the method options
				}

				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("RItemList")) == 0)
			{
				for (i = 0; atts[i]; i += 2)
				{
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);
					m_methodOptions.add(attVal, attName); // add the attributes to the method options
				}

				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_ITEMLIST);
			}
			else if (_tcsicmp(elementName, _T("InvalidServerSubHandles")) == 0)
			{
				m_fault = OPC_E_INVALIDHANDLE;
				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("Errors")) == 0)
			{
				m_elementStack.push(xmlName);
				setObjectState(SODACXMLRESPONSE_STATE_ERROR);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on subscribe call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_ITEMLIST:
			if ((_tcsicmp(elementName, _T("Items")) == 0) &&
				(m_charDataType == SODACXMLRESPONSEDATA_TYPE_NONE))
			{
				if (getExpectedMethodState() != SODACXMLRESPONSE_STATE_SUBSCRIBERESPONSE)
				{
					SODaCXmlResponseData* pMD;
					SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
					pMD->m_type = SODACXMLRESPONSEDATA_TYPE_ITEM;

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
							pMD->m_clientItemHandle = attVal;
						}
						else if (_tcsicmp(attName, _T("Timestamp")) == 0)
						{
							pMD->m_timestamp = attVal;
						}
						else if (_tcsicmp(attName, _T("ResultID")) == 0)
						{
							pMD->m_resultID = attVal;
						}
					}

					m_pCurMethodData = pMD;
					m_methodData.add(pMD);
					m_charDataType = SODACXMLRESPONSEDATA_TYPE_ITEM;
				}

				m_elementStack.push(xmlName);
			}
			else if ((_tcsicmp(elementName, _T("ItemValue")) == 0) &&
					 (m_charDataType == SODACXMLRESPONSEDATA_TYPE_NONE))
			{
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_ITEM;

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
						pMD->m_clientItemHandle = attVal;
					}
					else if (_tcsicmp(attName, _T("Timestamp")) == 0)
					{
						pMD->m_timestamp = attVal;
					}
					else if (_tcsicmp(attName, _T("ResultID")) == 0)
					{
						pMD->m_resultID = attVal;
					}
				}

				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_ITEM;
				m_elementStack.push(xmlName);
			}
			else if ((_tcsicmp(elementName, _T("Value")) == 0) &&
					 (m_charDataType == SODACXMLRESPONSEDATA_TYPE_ITEM))
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

						if (_tcsstr(attVal, _T("Array")) == NULL)
						{
							m_pCurMethodData->m_value.m_simpleType = TRUE;
							m_pCurSimpleXmlValue = &m_pCurMethodData->m_value;
							m_charDataType = SODACXMLRESPONSEDATA_TYPE_VALUE;
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
							setObjectState(SODACXMLRESPONSE_STATE_VALUEARRAY);
							m_charDataType = SODACXMLRESPONSEDATA_TYPE_VALUE;
						}

						m_elementStack.push(xmlName);
						haveType = TRUE;
					}
				}

				if (!haveType)
				{
					setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid value; no type")));
				}
			}
			else if ((_tcsicmp(elementName, _T("Quality")) == 0) &&
					 (m_charDataType == SODACXMLRESPONSEDATA_TYPE_ITEM))
			{
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_QUALITY;

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

				m_elementStack.push(xmlName);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_ELEMENTS:
		case SODACXMLRESPONSE_STATE_PROPERTYLISTS:
			if (_tcsicmp(elementName, _T("Properties")) == 0)
			{
				SODaCXmlResponseData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SODaCXmlResponseData)
				pMD->m_type = SODACXMLRESPONSEDATA_TYPE_PROPERTY;

				for (i = 0; atts[i]; i += 2)
				{
					// add the attributes to the method options
					attName = A2CT(atts[i]);
					attVal = A2CT(atts[i + 1]);

					if (_tcsicmp(attName, _T("Name")) == 0)
					{
						SOCmnString propName = attVal;

						if (includesNamespacePrefix(propName))
						{
							// property name with namespace
							expandNamespace(propName);
							removeOPCXMLDANamespace(propName); // remove default OPC XML DA NS
						}

						SOCmnStringEx::setXMLString(propName, pMD->m_name);
					}
					else if (_tcsicmp(attName, _T("ItemName")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemName);
					}
					else if (_tcsicmp(attName, _T("ItemPath")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_itemPath);
					}
					else if (_tcsicmp(attName, _T("Description")) == 0)
					{
						SOCmnStringEx::setXMLString(attVal, pMD->m_description);
					}
					else if (_tcsicmp(attName, _T("ResultID")) == 0)
					{
						pMD->m_resultID = attVal;
					}
				}

				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				m_elementStack.push(xmlName);
			}
			else if (_tcsicmp(elementName, _T("Value")) == 0)
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

						if (_tcsstr(attVal, _T("Array")) == NULL)
						{
							m_pCurMethodData->m_value.m_simpleType = TRUE;
							m_pCurSimpleXmlValue = &m_pCurMethodData->m_value;
							m_charDataType = SODACXMLRESPONSEDATA_TYPE_VALUE;

							if (_tcsicmp(attVal, _T("OPCQuality")) == 0)
							{
								// default:
								//   * QualityField (Good)
								//   * LimitField (None)
								//   * VendorField (0)
								m_pCurMethodData->m_value.m_value = _T("192"); // good
							}
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
							setObjectState(SODACXMLRESPONSE_STATE_VALUEARRAY);
						}

						m_elementStack.push(xmlName);
						haveType = TRUE;
					}
					else if (_tcsicmp(attName, _T("QualityField")) == 0)
					{
						// if quality is set, discard initial good quality
						WORD currQuality = (WORD) _tstoi(m_pCurMethodData->m_value.m_value);
						if (currQuality == 192)
						{
							currQuality = 0;
						}
						currQuality |= SODaCXmlCall::getDCOMQuality(attVal, _T(""), _T(""));
						TCHAR charQual[16] = {0}; 
						m_pCurMethodData->m_value.m_value = _itot(currQuality, charQual, 10);
					}
					else if (_tcsicmp(attName, _T("LimitField")) == 0)
					{
						// if quality is set, discard initial good quality
						WORD currQuality = (WORD) _tstoi(m_pCurMethodData->m_value.m_value);
						if (currQuality == 192)
						{
							currQuality = 0;
						}
						currQuality |= SODaCXmlCall::getDCOMQuality(_T(""), attVal, _T(""));
						TCHAR charQual[16] = {0}; 
						m_pCurMethodData->m_value.m_value = _itot(currQuality, charQual, 10);
					}
					else if (_tcsicmp(attName, _T("VendorField")) == 0)
					{
						// if quality is set, discard initial good quality
						WORD currQuality = (WORD) _tstoi(m_pCurMethodData->m_value.m_value);
						if (currQuality == 192)
						{
							currQuality = 0;
						}
						currQuality |= SODaCXmlCall::getDCOMQuality(_T(""), _T(""), attVal);
						TCHAR charQual[16] = {0}; 
						m_pCurMethodData->m_value.m_value = _itot(currQuality, charQual, 10);
					}
				}

				if (!haveType)
				{
					setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid value; no type")));
				}
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid element on call: %s"), elementName));
			}

			break;

		case SODACXMLRESPONSE_STATE_VALUEARRAY:
		{
			SOCmnString arrElDT;
			BOOL haveType = FALSE;
			SOCmnXmlValue* pCurArray = (SOCmnXmlValue*)m_arrayStack.getHead();
			SOCmnXmlValue* pXmlVal;
			SOCMN_ALLOCATE_OBJECT(pXmlVal, SOCmnXmlValue)

			if (_tcsicmp(elementName, _T("anyType")) != 0)
			{
				arrElDT = elementName;
				arrElDT.replace(_T(""), _T("http://www.w3.org/2001/XMLSchema|"));
				pXmlVal->m_datatype = arrElDT;
				expandNamespace(pXmlVal->m_datatype);
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

						if (_tcsstr(attVal, _T("Array")) == NULL)
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
							setObjectState(SODACXMLRESPONSE_STATE_VALUEARRAY);
						}

						haveType = TRUE;
						m_arrayStack.push((void*)pXmlVal);
					}
				}
			}

			if (haveType)
			{
				m_elementStack.push(xmlName);
				m_charDataType = SODACXMLRESPONSEDATA_TYPE_VALUEARRAY;
			}
			else
			{
				delete pXmlVal;
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("Invalid array datatype: %s"), elementName));
			}
		}
		break;

		case SODACXMLRESPONSE_STATE_ERROR:
			m_elementStack.push(xmlName);
			break;

		default:
			SOCmnXmlMethod::handleStartElement(nameA, atts);
			break;
		}
	}
}

void SODaCXmlResponse::handleEndElement(LPCSTR nameA)
{
	USES_CONVERSION;
	LPCTSTR xmlName = A2CT(nameA);
	LPCTSTR elementName = xmlName;
	//  replace the namespace if any
#ifdef SOOS_WINDOWS

	if (_tcsncicmp(elementName, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|"), 48) == 0)
#else
	if (strncasecmp(elementName, _T("http://opcfoundation.org/webservices/XMLDA/1.0/|"), 48) == 0)
#endif
	{
		elementName = elementName + 48;
	}

	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("E: %s"), xmlName));

	switch (m_charDataType)
	{
	case SODACXMLRESPONSEDATA_TYPE_VALUE:
	case SODACXMLRESPONSEDATA_TYPE_QUALITY:
	case SODACXMLRESPONSEDATA_TYPE_DIAGNOSTICINFO:
		m_charDataType = SODACXMLRESPONSEDATA_TYPE_ITEM;
		break;

	case SODACXMLRESPONSEDATA_TYPE_VALUEARRAY:
		m_charDataType = SODACXMLRESPONSEDATA_TYPE_VALUE;
		break;

	default:
		m_charDataType = SODACXMLRESPONSEDATA_TYPE_NONE;
		break;
	}

	if ((getObjectState() == SODACXMLRESPONSE_STATE_VALUEARRAY) &&
		((_tcsicmp(elementName, _T("Value")) == 0)))
	{
		if (getExpectedMethodState() == SODACXMLRESPONSE_STATE_GETPROPERTIESRESPONSE)
		{
			setObjectState(SODACXMLRESPONSE_STATE_PROPERTYLISTS);
		}
		else if (getExpectedMethodState() == SODACXMLRESPONSE_STATE_BROWSERESPONSE)
		{
			setObjectState(SODACXMLRESPONSE_STATE_ELEMENTS);
		}
		else
		{
			setObjectState(SODACXMLRESPONSE_STATE_ITEMLIST);
		}
	}

	if ((getObjectState() == SODACXMLRESPONSE_STATE_ELEMENTS) &&
		((_tcsicmp(elementName, _T("Elements")) == 0)))
	{
		setObjectState(SODACXMLRESPONSE_STATE_BROWSERESPONSE);
	}

	if ((getObjectState() == SODACXMLRESPONSE_STATE_PROPERTYLISTS) &&
		((_tcsicmp(elementName, _T("PropertyLists")) == 0)))
	{
		setObjectState(SODACXMLRESPONSE_STATE_GETPROPERTIESRESPONSE);
	}

	if ((getObjectState() == SODACXMLRESPONSE_STATE_ITEMLIST) &&
		((_tcsicmp(elementName, _T("RItemList")) == 0)))
	{
		setObjectState(getExpectedMethodState());
	}

	if (_tcsicmp(elementName, _T("anyType")) == 0)
		if (m_arrayStack.isInit())
		{
			m_arrayStack.pop();
		}

	SOCmnXmlMethod::handleEndElement(nameA);
}

void SODaCXmlResponse::handleCharacterData(LPCSTR sA, INT len)
{
	USES_CONVERSION;
	SOCmnString c;
//	SODaCXmlResponseData *pMD;
	c.setCHAR(sA, len);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOAP_DATA, (_T("C: %s"), (LPCTSTR)c));

	switch (getObjectState())
	{
	case SODACXMLRESPONSE_STATE_ELEMENTS:
	case SODACXMLRESPONSE_STATE_PROPERTYLISTS:
	case SODACXMLRESPONSE_STATE_ITEMLIST:
		if (m_charDataType == SODACXMLRESPONSEDATA_TYPE_VALUE)
		{
			m_pCurMethodData->m_value.m_value += c;
		}

		break;

	case SODACXMLRESPONSE_STATE_VALUEARRAY:
	{
		if (m_charDataType == SODACXMLRESPONSEDATA_TYPE_VALUEARRAY)
		{
			m_pCurSimpleXmlValue->m_value += c;
		}
	}
	break;

	case SODACXMLRESPONSE_STATE_GETSTATUSRESPONSE:
		if (m_charDataType == SODACXMLRESPONSEDATA_TYPE_VENDORINFO)
		{
			m_pCurMethodData->m_dataSZ += c;
		}
		else if (m_charDataType == SODACXMLRESPONSEDATA_TYPE_LOCALEID)
		{
			m_pCurMethodData->m_dataSZ += c;
		}
		else if (m_charDataType == SODACXMLRESPONSEDATA_TYPE_INTERFACEVERSION)
		{
			m_pCurMethodData->m_dataSZ += c;
		}

		break;
	}
}

BOOL SODaCXmlResponse::removeOPCXMLDANamespace(OUT IN SOCmnString& szXML)
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
// SODaCXmlResponseData                                                         |
//-----------------------------------------------------------------------------

SODaCXmlResponseData::SODaCXmlResponseData(void)
{
	m_type = SODACXMLRESPONSEDATA_TYPE_NONE;
}


SODaCXmlResponseData::~SODaCXmlResponseData(void)
{
	m_value.m_array.destroy();
}

#endif // SOFEATURE_SOAP

#endif // SODAC
