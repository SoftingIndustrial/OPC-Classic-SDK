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
//  Filename    : SODaCComBrowse.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA Browsing objects                                         |
//                - SODaCComNameSpaceBrowserImpl: name space browsing root object
//                - SODaCNode: node browser object class                      |
//                - SODaCTag: tag browser object class                        |
//                - SODaCProperty: property browser object class              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_DCOM

#include "SODaCBrowse.h"
#include "SODaCServer.h"

// The return is pointing to a new created string.
// So it must be deleted from outside this function.
LPCTSTR convertWCHAR2LPCTSTR(wchar_t* wcharStr)
{
	if (!wcharStr)
	{
		return NULL;
	}

#if defined(UNICODE) || defined(_UNICODE)
	wchar_t* wchStr = new wchar_t[wcslen(wcharStr) + 1] ;
	wcscpy(wchStr, wcharStr);
	return wchStr;
#else
	unsigned int strLength = ::WideCharToMultiByte(CP_UTF8, 0, wcharStr, (int)wcslen(wcharStr), 0, 0 , 0, 0);
	char* charStr = new char[strLength + 1];
	//  Could use to check the result
	::WideCharToMultiByte(CP_UTF8, 0, wcharStr, (int)wcslen(wcharStr), charStr, strLength , 0, 0);
	charStr[strLength] = '\0';
	return charStr;
#endif
}

//-----------------------------------------------------------------------------
// SODaCComNameSpaceBrowserImpl                                                      |
//-----------------------------------------------------------------------------

SODaCComNameSpaceBrowserImpl::SODaCComNameSpaceBrowserImpl(SODaCServer* /*pSrv*/)
{
	m_supportBrowseTo = TRUE;
}

SODaCComNameSpaceBrowserImpl::~SODaCComNameSpaceBrowserImpl()
{
}

BOOL SODaCComNameSpaceBrowserImpl::changeToRoot()
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	HRESULT res;
	CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
	server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

	if (!browseSAS)
	{
		return FALSE;
	}

	if (m_supportBrowseTo)
	{
		server->beforeInterfaceCall();

		if (pBrowser->getForceBrowseUp())
		{
			res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO [0x%X]"), res));
		}
		else
		{
			res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_TO, L"");
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO [0x%X]"), res));
		}

		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO [0x%X]"), res));

		if (FAILED(res))
		{
			m_supportBrowseTo = FALSE;
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// An older m_server may not support OPC_BROWSE_TO flag.
		// Try it the other way.
		server->beforeInterfaceCall();

		while (SUCCEEDED(res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_UP, L"")))
		{
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_UP [0x%X]"), res));
		}

		server->afterInterfaceCall();
		res = S_OK;
	}

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return FALSE;
	}

	return TRUE;
}


BOOL SODaCComNameSpaceBrowserImpl::expand20(IN SOCltBrowseObject* pParent, IN LPCTSTR pItemID, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();

	if (pBrowser->getForceBrowseUp())
	{
		m_supportBrowseTo = FALSE;
		pBrowser->setRetrieveItemID(TRUE);
	}

	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	SOCmnString itemID(pItemID);
	CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
	HRESULT res;
	USES_CONVERSION;
	BOOL changedBrowsePos = FALSE;
	res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

	if (FAILED(res))
	{
		return FALSE;
	}

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	if ((SOCMNOBJECT_TYPE_PROPERTY != type) || (itemID.isEmpty()))
	{
		// not only browsing properties and if so have a item id
		if (m_supportBrowseTo)
		{
			if (((!itemID.isEmpty()) ||                                // have item id or
				 (pParent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))) && // parent is root and
				(!pParent->is(SOCMNOBJECT_TYPE_TAG)))                 // parent is not a tag
			{
				// change browse position to parent
				server->beforeInterfaceCall();

				if (pBrowser->getForceBrowseUp())
				{
					res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_UP [0x%X]"), res));
				}
				else
				{
					res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_TO, T2W(itemID));
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO %s [0x%X]"), (LPCTSTR)itemID, res));
				}

				server->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO %s [0x%X]"), (LPCTSTR)itemID, res));

				if (SUCCEEDED(res))
				{
					changedBrowsePos = TRUE;
				}
			}
		}

		if (!changedBrowsePos)
		{
			// no BROWSE_TO or empty item id
			if (m_supportBrowseTo)
			{
				// try to go directly to the parents parent
				SOCmnPointer<SOCltBrowseObject> parentsParent = (SOCltBrowseObject*)pParent->getParentBranch();

				if (parentsParent.isNotNull())
				{
					SOCmnString parentsParentItemID;
					parentsParentItemID = parentsParent->getItemID(FALSE);

					if (!parentsParentItemID.isEmpty())
					{
						server->beforeInterfaceCall();

						if (pBrowser->getForceBrowseUp())
						{
							res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
							_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_UP [0x%X]"), res));
						}
						else
						{
							res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_TO, T2W(parentsParentItemID));
							_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO %s [0x%X]"), (LPCTSTR)parentsParentItemID, res));
						}

						server->afterInterfaceCall();
						_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_TO %s [0x%X]"), (LPCTSTR)parentsParentItemID, res));

						if (SUCCEEDED(res))
						{
							changedBrowsePos = TRUE;
						}
					}
				}
			}

			if (!changedBrowsePos)
			{
				// no BROWSE_TO
				// Could be anywhere now. Change to the root
				if (!changeToRoot())
				{
					return FALSE;
				}

				// travel down to the parent of the parent
				SOCmnListPosition pos;
				SOCmnStringList* pPathList = pParent->getPathList();
				SOCmnString pathPart;

				if (pPathList)
				{
					pos = pPathList->getStartPosition();

					while (pos)
					{
						pathPart = pPathList->getNext(pos);

						if (!pathPart.isEmpty())
						{
							server->beforeInterfaceCall();
							res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pathPart));
							server->afterInterfaceCall();
							_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

							if (FAILED(res))
							{
								server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
								return FALSE;
							}
						}
					}
				}
			}

			if ((itemID.isEmpty()) && (!pParent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER)))
			{
				SOCltAutoMemory<WCHAR> wszFullPath;

				if (SOCMNOBJECT_TYPE_PROPERTY == (SOCMNOBJECT_TYPE_PROPERTY & type))
				{
					// Get item ID (need it later, when browsing props)
					server->beforeInterfaceCall();
					res = browseSAS->GetItemID(T2W((LPTSTR)(LPCTSTR)pParent->getName()), &wszFullPath);
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::GetItemID %s [0x%X]"), (LPCTSTR)pParent->getName(), res));

					if (SUCCEEDED(res))
					{
						itemID = W2T(wszFullPath);
						pParent->setItemID(itemID, itemID);
					}
				}
			}

			if (pBrowser->getForceBrowseUp())
			{
				if ((!pParent->is(SOCMNOBJECT_TYPE_TAG)) && (!pParent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER)))
				{
					// Change browse pos to parent
					server->beforeInterfaceCall();
					// For DOWN, you must specify the name of the branch to move into;
					// This would be one of the strings returned from BrowseOPCItemIDs.
					// For UP this parameter is ignored and should point to a NUL string.
					res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pParent->getName()));
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pParent->getName(), res));
				}
				else // pParent is SOCMNOBJECT_TYPE_TAG  OR  pParent is SOCMNOBJECT_TYPE_NAMESPACE_BROWSER
				{
					if ((type != SOCMNOBJECT_TYPE_PROPERTY) && (!pParent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER)))
					{
						_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CLIENT, pParent, (_T("SODaCComNameSpaceBrowserImpl::expand20 unable to browse a tag (LEAF).")));
						return FALSE;
					}
				}
			}
			else
			{
				if (!pParent->is(SOCMNOBJECT_TYPE_TAG))
				{
					// Change browse pos to parent
					server->beforeInterfaceCall();
					res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pParent->getName()));
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pParent->getName(), res));
				}
				else // pParent is SOCMNOBJECT_TYPE_TAG
				{
					if ((type != SOCMNOBJECT_TYPE_PROPERTY) && (!pParent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER)))
					{
						_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CLIENT, pParent, (_T("SODaCComNameSpaceBrowserImpl::expand20 unable to browse a tag (LEAF).")));
						return FALSE;
					}
				}
			}
		}
	}

	SOCmnString elementNameFilter = pBrowser->getElementNameFilter();
	LPWSTR wszFilterCriteria = elementNameFilter.isEmpty() ? L"" :  T2OLE((LPTSTR)(LPCTSTR)elementNameFilter);
	LPOLESTR wzName[1024];
	DWORD cnt;
	// Nodes
	//

	if (SOCMNOBJECT_TYPE_NODE == (type & SOCMNOBJECT_TYPE_NODE))
	{
		// Get the enumerator
		CComPtr<IEnumString> pEnum;
		server->beforeInterfaceCall();
		//The Client can browse the items below (contained in) the current position via BrowseOPCItemIDs
		res = browseSAS->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0, &pEnum);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::BrowseOPCItemIDs [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		// fill the property list
		while (SUCCEEDED(res = pEnum->Next(_COUNTOF(wzName), wzName, &cnt)))
		{
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

			for (DWORD i = 0 ; i < cnt; i++)
			{
				if (NULL == wzName[i])
				{
					continue;
				}

				SODaCNode* node = NULL;
				LPCTSTR szName = NULL;
				// At this point parameter is never NULL.
				szName = convertWCHAR2LPCTSTR(wzName[i]);
				node = creator->createNode((SOCltBrowseObject*)pParent, szName);
				node->setFlags(OPC_BROWSE_HASCHILDREN);

				if ((m_supportBrowseTo) || (pBrowser->getRetrieveItemID()))
				{
					SOCltAutoMemory<WCHAR> wszFullPath;
					server->beforeInterfaceCall();
					// The name of a BRANCH or LEAF at the current level.
					res = browseSAS->GetItemID(wzName[i], &wszFullPath);
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::GetItemID %s [0x%X]"), (LPCTSTR)szName, res));

					if (SUCCEEDED(res))
					{
						if (wszFullPath)
						{
							LPCTSTR itemIdStr = NULL;
							// At this point parameter is never NULL.
							itemIdStr = convertWCHAR2LPCTSTR(wszFullPath);
							node->setItemID(itemIdStr, itemID);
							delete [] itemIdStr;
						}
						else
						{
							node->setItemID(_T(""), itemID);
						}
					}
					else
					{
						node->setItemID(_T(""), itemID);
					}
				}

				objList.add(static_cast<SOCltBrowseObject*>(node));
				node->release();
				::CoTaskMemFree(wzName[i]);
				delete [] szName;
			}

			if (0 == cnt || S_FALSE == res)
			{
				break;
			}
		}

		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}
	}

	//
	// Tags
	//
	if (SOCMNOBJECT_TYPE_TAG == (type & SOCMNOBJECT_TYPE_TAG))
	{
		// Get the enumerator
		CComPtr<IEnumString> pEnum;
		server->beforeInterfaceCall();
		res = browseSAS->BrowseOPCItemIDs(OPC_LEAF, wszFilterCriteria, pBrowser->getDatatypeFilter(), pBrowser->getAccessRightsFilter(), &pEnum);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::BrowseOPCItemIDs [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		// fill the property list
		while (SUCCEEDED(res = pEnum->Next(_COUNTOF(wzName), wzName, &cnt)))
		{
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

			for (DWORD i = 0 ; i < cnt; i++)
			{
				if (NULL == wzName[i])
				{
					continue;
				}

				SODaCTag* tag = NULL;
				LPCTSTR szName = NULL;
				// At this point parameter is never NULL.
				szName = convertWCHAR2LPCTSTR(wzName[i]);
				tag = creator->createTag((SOCltBrowseObject*)pParent, szName);
				tag->setFlags(OPC_BROWSE_ISITEM);

				if (pBrowser->getRetrieveItemID())
				{
					SOCltAutoMemory<WCHAR> wszFullPath;
					server->beforeInterfaceCall();
					res = browseSAS->GetItemID(wzName[i], &wszFullPath);
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::GetItemID %s [0x%X]"), szName, res));

					if (SUCCEEDED(res))
					{
						if (wszFullPath)
						{
							LPCTSTR itemIdStr = NULL;
							// At this point parameter is never NULL.
							itemIdStr = convertWCHAR2LPCTSTR(wszFullPath);
							tag->setItemID(itemIdStr, itemID);
							delete [] itemIdStr;
						}
						else
						{
							tag->setItemID(_T(""), itemID);
						}
					}
					else
					{
						tag->setItemID(_T(""), itemID);
					}
				}

				objList.add(static_cast<SOCltBrowseObject*>(tag));
				tag->release();
				::CoTaskMemFree(wzName[i]);
				delete [] szName;
			}

			if (0 == cnt || S_FALSE == res)
			{
				break;
			}
		}

		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}
	}

	if (SOCMNOBJECT_TYPE_PROPERTY == (SOCMNOBJECT_TYPE_PROPERTY & type))
	{
		if (itemID.isEmpty())
		{
			return FALSE;
		}

		CComPtr<IOPCItemProperties> pProps;
		res = server->queryInterface(IID_IOPCItemProperties, (void**)&pProps);

		if (res == E_NOINTERFACE)
		{
			server->setLastError(res, SOCLT_ERR_ITEM_PROPS_NOT_SUPPORTED);
			return FALSE;
		}
		else if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		SOCltAutoMemory<DWORD> PropertyIDs;
		SOCltAutoMemory<LPWSTR> wszDescriptions;
		SOCltAutoMemory<VARTYPE> vtDataTypes;
		LPWSTR wszFullPath = T2W(itemID);
		server->beforeInterfaceCall();
		res = pProps->QueryAvailableProperties(wszFullPath, &cnt,
											   &PropertyIDs, &wszDescriptions, &vtDataTypes);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCItemProperties::QueryAvailableProperties [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		SOCmnString fullPath = W2T(wszFullPath);

		for (DWORD i = 0 ; i < cnt ; i++)
		{
			SODaCProperty* prop;
			prop = creator->createProperty((SOCltBrowseObject*)pParent, W2CT(wszDescriptions[i]), PropertyIDs[i], vtDataTypes[i], fullPath, _T(""));
			::CoTaskMemFree(wszDescriptions[i]);
			objList.add(static_cast<SOCltBrowseObject*>(prop));
			prop->release();
		}
	}

	return TRUE;
}



BOOL SODaCComNameSpaceBrowserImpl::expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR /*itemPath*/, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	USES_CONVERSION;
	HRESULT res;
	DWORD browseFilter = 0;
	BOOL moreElements;
	DWORD cnt;
	OPCBROWSEELEMENT* pElements;
	SOCmnString cpSZ;
	LPWSTR cp;
	DWORD dummy;
	CComPtr<IOPCBrowse> browse;
	server->queryInterface(IID_IOPCBrowse, (void**)&browse);

	if (!browse)
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
		cnt = pBrowser->getMaxElements();
		cp = cpSZ.cloneToWCHAR();
		server->beforeInterfaceCall();
		res = browse->Browse(T2W((LPTSTR)itemID), &cp, pBrowser->getMaxElements(), (OPCBROWSEFILTER)browseFilter, T2W(elementNameFilter), T2W(vendorFilter),
							 pBrowser->getReturnProperties(), pBrowser->getRetrievePropertyValues(), 0, &dummy, &moreElements, &cnt, &pElements);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowse::Browse %s [0x%X]"), (LPCTSTR)itemID, res));

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SOCltBrowseObject> bo;
			DWORD i, k;

			if (pElements != NULL)
			{				
				for (i = 0; i < cnt; i++)
				{
					bo = createBrowseObject(pParent, &pElements[i]);

					if (bo.isNotNull())
					{
						objList.add(bo);
					}

					// free memory
					CoTaskMemFree(pElements[i].szItemID);
					CoTaskMemFree(pElements[i].szName);

					for (k = 0; k < pElements[i].ItemProperties.dwNumProperties; k++)
					{
						CoTaskMemFree(pElements[i].ItemProperties.pItemProperties[k].szDescription);
						CoTaskMemFree(pElements[i].ItemProperties.pItemProperties[k].szItemID);
						VariantClear(&pElements[i].ItemProperties.pItemProperties[k].vValue);
					}

					CoTaskMemFree(pElements[i].ItemProperties.pItemProperties);
				}
			
				CoTaskMemFree(pElements);
			}

			if (cp)
			{
				contPoint = W2T(cp);
			}
		}
		else
		{
			server->setLastError(res, SODAC_ERR_SRV_BROWSE);
			return FALSE;
		}

		if (cp)
		{
			CoTaskMemFree(cp);
		}
	}

	if (SOCMNOBJECT_TYPE_PROPERTY == (type & SOCMNOBJECT_TYPE_PROPERTY))
	{
		LPWSTR wItemID = T2W((LPTSTR)itemID);
		OPCITEMPROPERTIES* pItemProperties;
		dummy = 0;
		server->beforeInterfaceCall();
		res = browse->GetProperties(1, &wItemID, pBrowser->getRetrievePropertyValues(), 0, &dummy, &pItemProperties);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowse::GetProperties [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			SOCmnPointer<SOCltBrowseObject> bo;
			DWORD i;

			if (SUCCEEDED(pItemProperties->hrErrorID))
			{
				for (i = 0; i < pItemProperties->dwNumProperties; i++)
				{
					bo = createBrowseObject(pParent, &pItemProperties->pItemProperties[i]);

					if (bo.isNotNull())
					{
						objList.add(bo);
					}

					::CoTaskMemFree(pItemProperties->pItemProperties[i].szDescription);
					::CoTaskMemFree(pItemProperties->pItemProperties[i].szItemID);
					::VariantClear(&pItemProperties->pItemProperties[i].vValue);
				}
			}
			else
			{
				server->setLastError(pItemProperties->hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
			}
		}
		else
		{
			server->setLastError(res, SODAC_ERR_SRV_GET_PROPERTIES);
		}

		::CoTaskMemFree(pItemProperties->pItemProperties);
		::CoTaskMemFree(pItemProperties);
	}

	return TRUE;
}

SOCltBrowseObject* SODaCComNameSpaceBrowserImpl::createBrowseObject(
	IN SOCltBrowseObject* pParent,
	IN OPCBROWSEELEMENT* pElement)
{
	if (!pElement)
	{
		return NULL;
	}

	SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
	USES_CONVERSION;
	// create element
	SOCltBrowseObject* pBO = NULL;
	SOCmnList<SODaCProperty> propList;
	SOCmnString parentItemID;

	if (pParent)
	{
		parentItemID = pParent->getItemID();
	}

	if ((pElement->dwFlagValue & OPC_BROWSE_HASCHILDREN) ||                            // children
		((pElement->dwFlagValue & (OPC_BROWSE_HASCHILDREN | OPC_BROWSE_ISITEM)) == 0)) // no children and no item
	{
		// -> node
		SODaCNode* pNode;
		LPCTSTR szName;
		szName = W2CT(pElement->szName);
		pNode = creator->createNode(pParent, szName);
		pNode->setItemID(W2CT(pElement->szItemID), parentItemID);
		pNode->setFlags(pElement->dwFlagValue);
		pBO = pNode;

		if (pElement->ItemProperties.dwNumProperties > 0)
		{
			propList = pNode->getPropertyList();
		}
	}
	else // no children and item
	{
		// -> tag
		SODaCTag* pTag;
		LPCTSTR szName;
		szName = W2CT(pElement->szName);
		pTag = creator->createTag(pParent, szName);
		pTag->setItemID(W2CT(pElement->szItemID), parentItemID);
		pTag->setFlags(pElement->dwFlagValue);
		pBO = pTag;

		if (pElement->ItemProperties.dwNumProperties > 0)
		{
			propList = pTag->getPropertyList();
		}
	}

	if (SUCCEEDED(pElement->ItemProperties.hrErrorID))
	{
		// create properties
		if (pElement->ItemProperties.dwNumProperties > 0)
		{
			SOCmnPointer<SODaCProperty> prop;
			DWORD i;

			for (i = 0; i < pElement->ItemProperties.dwNumProperties; i++)
			{
				if (SUCCEEDED(pElement->ItemProperties.pItemProperties[i].hrErrorID))
				{
					prop = creator->createProperty(pParent, W2CT(pElement->ItemProperties.pItemProperties[i].szDescription), pElement->ItemProperties.pItemProperties[i].dwPropertyID, pElement->ItemProperties.pItemProperties[i].vtDataType, W2CT(pElement->szItemID), _T(""));
					prop->setItemID(W2CT(pElement->ItemProperties.pItemProperties[i].szItemID), parentItemID);
					prop->setValue(&pElement->ItemProperties.pItemProperties[i].vValue);
					propList.add(prop);
				}
				else
				{
					SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
					SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
					_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, server, (_T("error set on browse element property %d [0x%X]"), i, pElement->ItemProperties.pItemProperties[i].hrErrorID));
					server->setLastError(pElement->ItemProperties.pItemProperties[i].hrErrorID, SOCLT_ERR_BROWSE_UNEXPECTED);
				}
			}
		}
	}
	else
	{
		SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
		SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
		_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, server, (_T("error set on browse element properties [0x%X]"), pElement->ItemProperties.hrErrorID));
		server->setLastError(pElement->ItemProperties.hrErrorID, SOCLT_ERR_BROWSE_UNEXPECTED);
	}

	return pBO;
}


SOCltBrowseObject* SODaCComNameSpaceBrowserImpl::createBrowseObject(
	IN SOCltBrowseObject* pParent,
	IN OPCITEMPROPERTY* pElement)
{
	if (!pElement)
	{
		return NULL;
	}

	if (SUCCEEDED(pElement->hrErrorID))
	{
		USES_CONVERSION;
		SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
		SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
		SODaCProperty* prop;
		SOCmnString parentItemID;

		if (pParent)
		{
			parentItemID = pParent->getItemID();
		}

		prop = creator->createProperty(pParent, W2CT(pElement->szDescription), pElement->dwPropertyID, pElement->vtDataType, W2CT(pElement->szItemID), _T(""));
		prop->setItemID(W2CT(pElement->szItemID), parentItemID);
		prop->setValue(&pElement->vValue);
		return prop;
	}
	else
	{
		SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
		SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
		_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, server, (_T("error set on get property element [0x%X]"), pElement->hrErrorID));
		server->setLastError(pElement->hrErrorID, SOCLT_ERR_BROWSE_UNEXPECTED);
		return NULL;
	}
}

BOOL SODaCComNameSpaceBrowserImpl::expandFlat(OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	HRESULT res;
	USES_CONVERSION;
	CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
	res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

	if (FAILED(res))
	{
		return FALSE;
	}

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	SOCmnString elementNameFilter = pBrowser->getElementNameFilter();
	LPWSTR wszFilterCriteria = elementNameFilter.isEmpty() ? L"" :  T2OLE((LPTSTR)(LPCTSTR)elementNameFilter);
	LPOLESTR wzName[1024];
	DWORD cnt;
	//
	// Tags
	//
	// Get the enumerator
	CComPtr<IEnumString> pEnum;
	server->beforeInterfaceCall();
	res = browseSAS->BrowseOPCItemIDs(OPC_FLAT, wszFilterCriteria, pBrowser->getDatatypeFilter(), pBrowser->getAccessRightsFilter(), &pEnum);
	server->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::BrowseOPCItemIDs [0x%X]"), res));

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return FALSE;
	}

	// fill the property list

	while (SUCCEEDED(res = pEnum->Next(_COUNTOF(wzName), wzName, &cnt)))
	{
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

		for (DWORD i = 0 ; i < cnt; i++)
		{
			if (NULL == wzName[i])
			{
				continue;
			}

			SODaCTag* tag;
			LPCTSTR szName;
			szName = W2CT(wzName[i]);
			tag = creator->createTag(pBrowser, szName);
			objList.add(static_cast<SOCltBrowseObject*>(tag));
			tag->release();
			::CoTaskMemFree(wzName[i]);
		}

		if (0 == cnt || S_FALSE == res)
		{
			break;
		}
	}

	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return FALSE;
	}

	return TRUE;
}


LPCTSTR SODaCComNameSpaceBrowserImpl::retrieveItemID(SOCltBrowseObject* pObj)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	SOCmnString path;
	HRESULT res;
	SOCmnPointer<SOCltBrowseObject> parent = (SOCltBrowseObject*)pObj->getParentBranch();
	USES_CONVERSION;
	SOCmnString parentItemID;

	if (parent.isNotNull())
	{
		parentItemID = parent->getItemID();
	}

	if (!pObj->is(SOCMNOBJECT_TYPE_PROPERTY))
	{
		CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
		res = server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);

		if (FAILED(res))
		{
			return path;
		}

		// Change to the root
		if (!changeToRoot())
		{
			return path;
		}

		// Travel down to the parent
		SOCmnListPosition pos;
		SOCmnStringList* pathList = pObj->getPathList();
		SOCmnString pathPart;

		if (pathList)
		{
			pos = pathList->getStartPosition();

			while (pos)
			{
				server->beforeInterfaceCall();
				pathPart = pathList->getNext(pos);

				if (!pathPart.isEmpty())
				{
					res = browseSAS->ChangeBrowsePosition(OPC_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pathPart));
					server->afterInterfaceCall();
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition OPC_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

					if (FAILED(res))
					{
						server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
						return path;
					}
				}
			}
		}

		// Get the fully qualified name
		SOCltAutoMemory<WCHAR> wszID;
		server->beforeInterfaceCall();
		res = browseSAS->GetItemID(T2W((LPTSTR)(LPCTSTR)pObj->getName()), &wszID);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCBrowseServerAddressSpace::GetItemID %s [0x%X]"), (LPCTSTR)pObj->getName(), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return path;
		}

		path = W2T(wszID);
		pObj->setItemID(path, parentItemID);
		return path;
	}
	else
	{
		// property
		SODaCProperty* pProp = (SODaCProperty*)pObj;

		if (pProp->getPropertyID() <= 99)
		{
			return path;
		}

		CComPtr<IOPCItemProperties> pProps;
		res = server->queryInterface(IID_IOPCItemProperties, (void**)&pProps);

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return path;
		}

		SOCltAutoMemory<LPWSTR> wszPropPath;
		SOCltAutoMemory<HRESULT> hrErrors;
		DWORD propID = pProp->getPropertyID();
		server->beforeInterfaceCall();
		res = pProps->LookupItemIDs(T2W((LPTSTR)(LPCTSTR)pProp->getParentItemID()), (DWORD)1, &propID, &wszPropPath, &hrErrors);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCItemProperties::LookupItemIDs [0x%X]"), res));
		DWORD dwErr = S_OK;

		if (FAILED(res))
		{
			dwErr = (HRESULT_FACILITY(res) == FACILITY_ITF) ? SODAC_ERR_NO_MAPPING_FOR_PROPERTIES : SOCLT_ERR_BROWSE_UNEXPECTED;
			server->setLastError(res, dwErr);
			return path;
		}

		if (FAILED(hrErrors[0]))
		{
			dwErr = (HRESULT_FACILITY(res) == FACILITY_ITF) ? SODAC_ERR_NO_MAPPING_FOR_PROPERTIES : SOCLT_ERR_BROWSE_UNEXPECTED;
			server->setLastError(hrErrors[0], SOCLT_ERR_BROWSE_UNEXPECTED);
			return path;
		}

		path = W2T(wszPropPath[0]);
		::CoTaskMemFree(wszPropPath[0]);
		pObj->setItemID(path, parentItemID);
		return path;
	}
}


void SODaCComNameSpaceBrowserImpl::initUse()
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
	CComPtr<IOPCBrowse> browse;
	CComPtr<IOPCItemProperties> itemProperties;
	server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS);
	server->queryInterface(IID_IOPCItemProperties, (void**)&itemProperties);
	server->queryInterface(IID_IOPCBrowse, (void**)&browse);

	if (!!browse)
	{
		pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE);
	}
	else
	{
		if (!!browseSAS)
		{
			pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE);

			if (!!itemProperties)
			{
				pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES);
			}
		}
		else
		{
			pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_NO_BROWSE_INTERFACE);
		}
	}
}

void SODaCComNameSpaceBrowserImpl::use20(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	CComPtr<IOPCItemProperties> itemProperties;
	server->queryInterface(IID_IOPCItemProperties, (void**)&itemProperties);

	if (!itemProperties)
	{
		pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE);
	}
	else
	{
		pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES);
	}
}

void SODaCComNameSpaceBrowserImpl::use30(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE);
}

BOOL SODaCComNameSpaceBrowserImpl::support20(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	CComPtr<IOPCBrowseServerAddressSpace> browseSAS;
	return SUCCEEDED(server->queryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&browseSAS));
}

BOOL SODaCComNameSpaceBrowserImpl::support30(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	CComPtr<IOPCBrowse> browse;
	return SUCCEEDED(server->queryInterface(IID_IOPCBrowse, (void**)&browse));
}



SODaCComNameSpaceBrowser::SODaCComNameSpaceBrowser(SODaCServer* pServer)
	: SODaCNameSpaceBrowser(pServer),
	  SODaCComNameSpaceBrowserImpl(pServer)
{
	initUse();
}

#endif // SOFEATURE_DCOM

#endif // SODAC
