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
//  Filename    : SODaCXmlBrowse.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA Browsing objects                                         |
//                - SODaCXmlNameSpaceBrowserImpl: name space browsing root object
//                - SODaCNode: node browser object class                      |
//                - SODaCTag: tag browser object class                        |
//                - SODaCProperty: property browser object class              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_SOAP

#include "SODaCBrowse.h"
#include "SODaCServer.h"
#include "SODaCXmlCall.h"


//-----------------------------------------------------------------------------
// SODaCXmlNameSpaceBrowserImpl                                                      |
//-----------------------------------------------------------------------------

SODaCXmlNameSpaceBrowserImpl::SODaCXmlNameSpaceBrowserImpl(SODaCServer* /* pSrv */)
{
}

SODaCXmlNameSpaceBrowserImpl::~SODaCXmlNameSpaceBrowserImpl()
{
}


BOOL SODaCXmlNameSpaceBrowserImpl::expand20(IN SOCltBrowseObject* /* pParent */, IN LPCTSTR /* pItemID */, IN DWORD /* type */, OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	server->setLastError(E_NOTIMPL, SOCLT_ERR_BROWSE_NOTSUPPORTED);
	return FALSE;
}



BOOL SODaCXmlNameSpaceBrowserImpl::expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR itemPath, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	DWORD browseFilter = 0;
	SOCmnString cpSZ;
	USES_CONVERSION;

	if (SOCLT_XML_CHECK_DEMO)
	{
		return FALSE;
	}

	if (!contPoint.isEmpty())
	{
		cpSZ = contPoint;
	}

	contPoint.empty();

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	// calculate the browse type
	if ((SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM) == (type & (SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM)))
	{
		browseFilter = OPC_BROWSE_FILTER_ALL;
	}
	else
	{
		if (SOCMNOBJECT_TYPE_NODE == (type & SOCMNOBJECT_TYPE_NODE))
		{
			browseFilter = OPC_BROWSE_FILTER_BRANCHES;
		}
		else if (SOCMNOBJECT_TYPE_ITEM == (type & SOCMNOBJECT_TYPE_ITEM))
		{
			browseFilter = OPC_BROWSE_FILTER_ITEMS;
		}
	}

	if (browseFilter != 0)
	{
		SOCmnString elementNameFilter = pBrowser->getElementNameFilter();
		SOCmnString vendorFilter = pBrowser->getVendorFilter();
		SOCmnPointer<SOCltHTTPConnection> httpCon = server->getHTTPConnection();
		SOCmnPointer<SODaCXmlCall> httpCall;
		HRESULT res;
		SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
		httpCall->browse(itemID, itemPath, cpSZ, pBrowser->getMaxElements(), browseFilter, elementNameFilter, vendorFilter, pBrowser->getReturnProperties(), pBrowser->getRetrievePropertyValues());
		server->beforeInterfaceCall();
		res = httpCon->invoke(httpCall, TRUE);
		server->afterInterfaceCall();

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SODaCXmlResponse> response;
			res = httpCall->getBrowseResponse(&response);

			if (SUCCEEDED(res))
			{
				SOCmnStringList xmlOptions(response->getResponseOptions());
				SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
				SOCmnStringEx convHlp;
				SODaCXmlResponseData* pResData;
				SOCmnListPosition pos;
				SOCmnPointer<SOCltBrowseObject> bO;
				SOCmnList<SODaCProperty> propList;
				xmlOptions.findKey(_T("ContinuationPoint"), &contPoint);
				pos = xmlData.getStartPosition();

				while (pos)
				{
					pResData = xmlData.getNext(pos);

					if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ELEMENT)
					{
						if ((pResData->m_hasChildren) ||                            // children
							((!pResData->m_hasChildren) && (!pResData->m_isItem)))  // no children and no item
						{
							// node
							SODaCNode* pNode;
							DWORD flags = 0;
							pNode = creator->createNode(pParent, pResData->m_name);
							convHlp = pResData->m_itemName;
							pNode->setItemID(convHlp, itemID);
							convHlp = pResData->m_itemPath;
							pNode->setItemPath(convHlp);

							if (pResData->m_isItem)
							{
								flags |= OPC_BROWSE_ISITEM;
							}

							if (pResData->m_hasChildren)
							{
								flags |= OPC_BROWSE_HASCHILDREN;
							}

							pNode->setFlags(flags);
							bO = pNode;
							propList = pNode->getPropertyList();
						}
						else
						{
							// tag
							SODaCTag* pTag;
							DWORD flags = 0;
							pTag = creator->createTag(pParent, pResData->m_name);
							convHlp = pResData->m_itemName;
							pTag->setItemID(convHlp, itemID);
							convHlp = pResData->m_itemPath;
							pTag->setItemPath(convHlp);

							if (pResData->m_isItem)
							{
								flags |= OPC_BROWSE_ISITEM;
							}

							if (pResData->m_hasChildren)
							{
								flags |= OPC_BROWSE_HASCHILDREN;
							}

							pTag->setFlags(flags);
							bO = pTag;
							propList = pTag->getPropertyList();
						}

						objList.add(bO);
					}

					if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_PROPERTY)
					{
						SOCmnVariant propValue;
						SOCmnPointer<SODaCProperty> prop;
						SOCmnStringEx propName;
						VARTYPE prDT = VT_EMPTY;
						propName = pResData->m_name;
						DWORD propID = server->getPropertyIDByName(propName);
						SOCmnVariant::getVARTYPE(pResData->m_value.m_datatype, prDT);
						convHlp = pResData->m_description;
						prop = creator->createProperty(bO, convHlp, propID, prDT, bO->getItemID(), bO->getItemPath());
						convHlp = pResData->m_itemName;
						prop->setItemID(convHlp, itemID);
						convHlp = pResData->m_name;
						prop->setPropertyName(convHlp);

						if (propValue.initFromXMLValue(&pResData->m_value))
						{
							if ((propID == 1) && (propValue.vt == VT_BSTR)) // datatype
							{
								SOCmnString propDT;
								propValue.getBSTR(propDT);

								if (response->includesNamespacePrefix(propDT))
								{
									// property name with namespace
									response->expandNamespace(propDT);
									response->removeOPCXMLDANamespace(propDT); // remove default OPC XML DA NS
									propValue.setBSTR(propDT);
								}
							}

							prop->setValue(propValue);
						}

						propList.add(prop);
					}
				}
			}
		}

		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("Browse [0x%lX]"), res));
	}

	if (SOCMNOBJECT_TYPE_PROPERTY == (type & SOCMNOBJECT_TYPE_PROPERTY))
	{
		SOCmnList<SODaCPropertyData> propList;
		HRESULT res;
		res = server->getPropertiesData(itemID, itemPath, (pBrowser->getRetrievePropertyValues()) ? SODAC_PROPDATA_VALUE : 0, propList);

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SODaCProperty> prop;
			SODaCPropertyData* propData;
			SOCmnListPosition posP;
			DWORD propID;
			SOCmnStringEx convHlp;
			SOCmnStringEx propName;
			posP = propList.getStartPosition();

			while (posP)
			{
				propData = propList.getNext(posP);
				propName = propData->m_name;
				propID = server->getPropertyIDByName(propName);
				convHlp = propData->m_description;
				prop = creator->createProperty(pParent, convHlp, propData->m_propertyID, propData->m_datatype, itemID, itemPath);
				convHlp = propData->m_itemID;
				prop->setItemID(convHlp, itemID);
				prop->setPropertyName(propName);
				prop->setValue(propData->m_value);
				objList.add(prop);
			}
		}

		propList.destroy();
	}

	return TRUE;
}


LPCTSTR SODaCXmlNameSpaceBrowserImpl::retrieveItemID(SOCltBrowseObject* pObj)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	SOCmnString path;
	USES_CONVERSION;

	if (!pObj->is(SOCMNOBJECT_TYPE_PROPERTY))
	{
	}
	else
	{
		// property
	}

	return (LPCTSTR)path;
}


void SODaCXmlNameSpaceBrowserImpl::initUse()
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE);
}

void SODaCXmlNameSpaceBrowserImpl::use20(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	server->setLastError(E_NOTIMPL, SOCLT_ERR_BROWSE_NOTSUPPORTED);
}

void SODaCXmlNameSpaceBrowserImpl::use30(void)
{
}

BOOL SODaCXmlNameSpaceBrowserImpl::support20(void)
{
	return FALSE;
}

BOOL SODaCXmlNameSpaceBrowserImpl::support30(void)
{
	return TRUE;
}



SODaCXmlNameSpaceBrowser::SODaCXmlNameSpaceBrowser(SODaCServer* pServer)
	: SODaCXmlNameSpaceBrowserImpl(pServer),
	  SODaCNameSpaceBrowser(pServer)
{
	initUse();
}

#endif // SOFEATURE_SOAP

#endif // SODAC
