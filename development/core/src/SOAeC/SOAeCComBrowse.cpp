#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCBrowse.h"
#include "SOAeCComBrowse.h"
#include "SOAeCServer.h"


//-----------------------------------------------------------------------------
// SOAeCComAreaBrowserImpl                                                    |
//-----------------------------------------------------------------------------

SOAeCComAreaBrowserImpl::SOAeCComAreaBrowserImpl(SOAeCServer* server)
{
	HRESULT res;

	if (SUCCEEDED(res = server->queryInterface(IID_IOPCEventServer, (void**)&m_eventServer)))
	{
		server->beforeInterfaceCall();
		res = m_eventServer->CreateAreaBrowser(IID_IOPCEventAreaBrowser, (IUnknown**)&m_eventAreaBrowser);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::CreateAreaBrowser [0x%X]"), res));
	}

	if (FAILED(res))
	{
		server->setLastError(res, SOAEC_ERR_CREATE_BROWSE_OBJECT);
	}
}

SOAeCComAreaBrowserImpl::~SOAeCComAreaBrowserImpl()
{
}

BOOL SOAeCComAreaBrowserImpl::expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOAeCAreaBrowser* pBrowser = getObjectSOAeCAreaBrowser();
	SOCmnPointer<SOAeCServer> server = pBrowser->getServer();
	SOCmnPointer<SOAeCEntry> entryAeC = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entryAeC->getCreator();

	if (!m_eventAreaBrowser)
	{
		return FALSE;
	}

	HRESULT res;
	USES_CONVERSION;

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	if ((pObject->is(SOCMNOBJECT_TYPE_AREA_BROWSER)) || (pObject->is(SOCMNOBJECT_TYPE_AREA)))
	{
		// Could be anywhere now. Change to the root
		// The second param should actually be NULL, bug in the IDL.
		server->beforeInterfaceCall();
		res = m_eventAreaBrowser->ChangeBrowsePosition(OPCAE_BROWSE_TO, L"");
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_TO, [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		if (pObject->is(SOCMNOBJECT_TYPE_AREA))
		{
			// Travel down to the parent
			SOCmnStringList pathList = ((SOAeCArea*)pObject)->getPathList();
			SOCmnListPosition pos;
			SOCmnString pathPart;
			pos = pathList.getStartPosition();

			while (pos)
			{
				pathPart = pathList.getNext(pos);
				server->beforeInterfaceCall();
				res = m_eventAreaBrowser->ChangeBrowsePosition(OPCAE_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pathPart));
				server->afterInterfaceCall();
				_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

				if (FAILED(res))
				{
					server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
					return FALSE;
				}
			}

			pathPart = ((SOAeCArea*)pObject)->getName();
			server->beforeInterfaceCall();
			res = m_eventAreaBrowser->ChangeBrowsePosition(OPCAE_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pathPart));
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}
		}

		SOCmnString filterCriteria(pBrowser->getElementNameFilter());
		LPWSTR lpwszFilterCriteria = filterCriteria.isEmpty() ? L"" :  T2OLE((LPTSTR)(LPCTSTR)filterCriteria);
		LPOLESTR wzName[100];
		DWORD cnt;

		//
		// Areas
		//

		if (SOCMNOBJECT_TYPE_AREA == (type & SOCMNOBJECT_TYPE_AREA))
		{
			// Get the enumerator
			CComPtr<IEnumString> pEnum;
			server->beforeInterfaceCall();
			res = m_eventAreaBrowser->BrowseOPCAreas(OPC_AREA, L"", &pEnum);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::BrowseOPCAreas OPC_AREA [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			// fill the property list
			if (S_OK == res)
			{
				_ASSERTION(pEnum, "Invalid enumerator pointer");

				while (SUCCEEDED(res = pEnum->Next(_COUNTOF(wzName), wzName, &cnt)))
				{
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IEnumString::Next [0x%X]"), res));

					for (DWORD i = 0 ; i < cnt; i++)
					{
						if (NULL == wzName[i])
						{
							continue;
						}

						SOAeCArea* area;
						LPCTSTR szName;
						szName = W2CT(wzName[i]);
						area = creator->createArea(pObject, szName);
						objList.add(static_cast<SOCltBrowseObject*>(area));
						area->release();
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
			}
		}

		//
		// Sources
		//

		if (SOCMNOBJECT_TYPE_SOURCE == (type & SOCMNOBJECT_TYPE_SOURCE))
		{
			// Get the enumerator
			CComPtr<IEnumString> pEnum;
			server->beforeInterfaceCall();
			res = m_eventAreaBrowser->BrowseOPCAreas(OPC_SOURCE, lpwszFilterCriteria, &pEnum);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::BrowseOPCAreas OPC_SOURCE [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			if (S_OK == res)
			{
				_ASSERTION(pEnum, "Invalid enumerator pointer");

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

						SOAeCSource* source;
						LPCTSTR szName;
						szName = W2CT(wzName[i]);
						source = creator->createSource(pObject, szName);
						objList.add(static_cast<SOCltBrowseObject*>(source));
						source->release();
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
			}
		}
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		CComPtr<IOPCEventServer> eventServer;
		res = queryInterface(IID_IOPCEventServer, (void**)&eventServer);

		if (FAILED(res))
		{
			return FALSE;
		}

		if (SOCMNOBJECT_TYPE_CONDITION != (SOCMNOBJECT_TYPE_CONDITION & type))
		{
			return FALSE;
		}

		SOCmnString path = pObject->getPath();

		if (path.isEmpty())
		{
			return FALSE;
		}

		DWORD cnt;
		SOCltAutoMemory<LPWSTR> wzName;
		server->beforeInterfaceCall();
		res = eventServer->QuerySourceConditions(T2W((LPTSTR)(LPCTSTR)path), &cnt, &wzName);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QuerySourceConditions [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		for (DWORD i = 0 ; i < cnt; i++)
		{
			SOAeCCondition* cond;
			LPCTSTR szName;
			szName = W2CT(wzName[i]);
			cond = creator->createCondition(pObject, szName);
			::CoTaskMemFree(wzName[i]);
			objList.add(static_cast<SOCltBrowseObject*>(cond));
			cond->release();
		}
	}

	return TRUE;
}


SOCmnString SOAeCComAreaBrowserImpl::getPathImpl(IN SOCltBrowseObject* pObject)
{
	SOAeCAreaBrowser* pBrowser = getObjectSOAeCAreaBrowser();
	SOCmnPointer<SOAeCServer> server = pBrowser->getServer();
	CComPtr<IOPCEventAreaBrowser> browser;
	HRESULT res;
	USES_CONVERSION;
	SOCmnString path;
	res = queryInterface(IID_IOPCEventAreaBrowser, (void**)&browser);

	if (FAILED(res))
	{
		return path;
	}

	// Change to the root
	server->beforeInterfaceCall();
	res = browser->ChangeBrowsePosition(OPCAE_BROWSE_TO, L"");
	server->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_TO, [0x%X]"), res));

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return path;
	}

	// Travel down to the parent
	SOCmnStringList pathList = ((SOAeCArea*)pObject)->getPathList();
	SOCmnListPosition pos;
	pos = pathList.getStartPosition();

	while (pos)
	{
		server->beforeInterfaceCall();
		res = browser->ChangeBrowsePosition(OPCAE_BROWSE_DOWN, T2W((LPTSTR)(LPCTSTR)pathList.getNext(pos)));
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return path;
		}
	}

	// Get the fully qualified name
	SOCltAutoMemory<WCHAR> wszQName;

	if (pObject->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		server->beforeInterfaceCall();
		res = browser->GetQualifiedSourceName(T2W((LPTSTR)(LPCTSTR)pObject->getName()), &wszQName);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::GetQualifiedSourceName [0x%X]"), res));
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_AREA))
	{
		server->beforeInterfaceCall();
		res = browser->GetQualifiedAreaName(T2W((LPTSTR)(LPCTSTR)pObject->getName()), &wszQName);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventAreaBrowser::GetQualifiedAreaName [0x%X]"), res));
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return path;
	}

	path = W2T(wszQName);
	return path;
}


//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the OPC Server
//
// returns:
//		S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SOAeCComAreaBrowserImpl::queryInterface(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	HRESULT res = E_NOINTERFACE;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_NOINTERFACE;
	}

	if (IID_IOPCEventAreaBrowser == riid)
	{
		res = !m_eventAreaBrowser ? E_NOINTERFACE : m_eventAreaBrowser.QueryInterface((IOPCEventAreaBrowser**)interf);
	}
	else if (IID_IOPCEventServer == riid)
	{
		res = !m_eventServer ? E_NOINTERFACE : m_eventServer.QueryInterface((IOPCEventServer**)interf);
	}

	return res;
} // queryInterface



//-----------------------------------------------------------------------------
// SOAeCComAreaBrowser                                                        |
//-----------------------------------------------------------------------------

SOAeCComAreaBrowser::SOAeCComAreaBrowser(SOAeCServer* pServer)
	: SOAeCAreaBrowser(pServer),
	  SOAeCComAreaBrowserImpl(pServer)
{
}






//-----------------------------------------------------------------------------
// SOAeCComEventBrowserImpl                                                    |
//-----------------------------------------------------------------------------

SOAeCComEventBrowserImpl::SOAeCComEventBrowserImpl(SOAeCServer* server)
{
	HRESULT res;
	res = server->queryInterface(IID_IOPCEventServer, (void**)&m_eventServer);

	if (FAILED(res))
	{
		server->setLastError(res, SOAEC_ERR_CREATE_BROWSE_OBJECT);
	}
}

SOAeCComEventBrowserImpl::~SOAeCComEventBrowserImpl()
{
}

BOOL SOAeCComEventBrowserImpl::expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SOAeCEntry> entryAeC = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entryAeC->getCreator();
	SOAeCEventBrowser* pBrowser = getObjectSOAeCEventBrowser();
	SOCmnPointer<SOAeCServer> server = pBrowser->getServer();
	HRESULT res;
	USES_CONVERSION;
	CComPtr<IOPCEventServer> eventServer;
	res = queryInterface(IID_IOPCEventServer, (void**)&eventServer);

	if (FAILED(res))
	{
		return FALSE;
	}

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	if (pObject->is(SOCMNOBJECT_TYPE_EVENT_BROWSER))
	{
		if (SOCMNOBJECT_TYPE_CATEGORY != (SOCMNOBJECT_TYPE_CATEGORY & type))
		{
			return FALSE;
		}

		DWORD filterCategories = pBrowser->getFilter();

		if ((filterCategories & OPC_SIMPLE_EVENT) == OPC_SIMPLE_EVENT)
		{
			DWORD cnt;
			SOCltAutoMemory<DWORD> dwEventCats;
			SOCltAutoMemory<LPWSTR> wszDescriptions;
			server->beforeInterfaceCall();
			res = eventServer->QueryEventCategories(OPC_SIMPLE_EVENT, &cnt, &dwEventCats, &wszDescriptions);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QueryEventCategories [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			for (DWORD i = 0 ; i < cnt; i++)
			{
				SOAeCCategory* cat;
				cat = creator->createCategory(pObject, W2T(wszDescriptions[i]));
				cat->m_id = dwEventCats[i];
				cat->m_eventType = OPC_SIMPLE_EVENT;
				::CoTaskMemFree(wszDescriptions[i]);
				objList.add(static_cast<SOCltBrowseObject*>(cat));
				cat->release();
			}
		}

		if ((filterCategories & OPC_TRACKING_EVENT) == OPC_TRACKING_EVENT)
		{
			DWORD cnt;
			SOCltAutoMemory<DWORD> dwEventCats;
			SOCltAutoMemory<LPWSTR> wszDescriptions;
			server->beforeInterfaceCall();
			res = eventServer->QueryEventCategories(OPC_TRACKING_EVENT, &cnt, &dwEventCats, &wszDescriptions);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QueryEventCategories [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			for (DWORD i = 0 ; i < cnt; i++)
			{
				SOAeCCategory* cat;
				cat = creator->createCategory(pObject, W2T(wszDescriptions[i]));
				cat->m_id = dwEventCats[i];
				cat->m_eventType = OPC_TRACKING_EVENT;
				::CoTaskMemFree(wszDescriptions[i]);
				objList.add(static_cast<SOCltBrowseObject*>(cat));
				cat->release();
			}
		}

		if ((filterCategories & OPC_CONDITION_EVENT) == OPC_CONDITION_EVENT)
		{
			DWORD cnt;
			SOCltAutoMemory<DWORD> dwEventCats;
			SOCltAutoMemory<LPWSTR> wszDescriptions;
			server->beforeInterfaceCall();
			res = eventServer->QueryEventCategories(OPC_CONDITION_EVENT, &cnt, &dwEventCats, &wszDescriptions);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QueryEventCategories [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			for (DWORD i = 0 ; i < cnt; i++)
			{
				SOAeCCategory* cat;
				cat = creator->createCategory(pObject, W2T(wszDescriptions[i]));
				cat->m_id = dwEventCats[i];
				cat->m_eventType = OPC_CONDITION_EVENT;
				::CoTaskMemFree(wszDescriptions[i]);
				objList.add(static_cast<SOCltBrowseObject*>(cat));
				cat->release();
			}
		}
	} // SOCMNOBJECT_TYPE_EVENT_BROWSER
	else if (pObject->is(SOCMNOBJECT_TYPE_CATEGORY))
	{
		DWORD catID = ((SOAeCCategory*)pObject)->getCategoryID();
		DWORD eventType = ((SOAeCCategory*)pObject)->getEventType();

		if (SOCMNOBJECT_TYPE_ATTRIBUTE == (type & SOCMNOBJECT_TYPE_ATTRIBUTE))
		{
			DWORD cnt;
			SOCltAutoMemory<DWORD> dwID;
			SOCltAutoMemory<LPWSTR> wszDescriptions;
			SOCltAutoMemory<VARTYPE> vtType;
			server->beforeInterfaceCall();
			res = eventServer->QueryEventAttributes(catID, &cnt, &dwID, &wszDescriptions, &vtType);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QueryEventAttributes [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			for (DWORD i = 0 ; i < cnt; i++)
			{
				SOAeCAttribute* attr;
				SOCMN_ALLOCATE_OBJECT(attr, SOAeCAttribute((SOAeCCategory*)pObject, wszDescriptions[i]));
				attr->m_id = dwID[i];
				attr->m_type = vtType[i];
				::CoTaskMemFree(wszDescriptions[i]);
				objList.add(static_cast<SOCltBrowseObject*>(attr));
				attr->release();
			}
		}

		if ((SOCMNOBJECT_TYPE_CONDITION_CLASS == (type & SOCMNOBJECT_TYPE_CONDITION_CLASS)) &&
			(eventType == OPC_CONDITION_EVENT))
		{
			DWORD cnt;
			SOCltAutoMemory<LPWSTR> wszNames;
			server->beforeInterfaceCall();
			res = eventServer->QueryConditionNames(catID, &cnt, &wszNames);
			server->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QueryConditionNames [0x%X]"), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			for (DWORD i = 0 ; i < cnt; i++)
			{
				SOAeCConditionClass* cond;
				SOCMN_ALLOCATE_OBJECT(cond, SOAeCConditionClass((SOAeCCategory*)pObject, wszNames[i]));
				::CoTaskMemFree(wszNames[i]);
				objList.add(static_cast<SOCltBrowseObject*>(cond));
				cond->release();
			}
		}
	} // SOCMNOBJECT_TYPE_CATEGORY
	else if (pObject->is(SOCMNOBJECT_TYPE_CONDITION_CLASS))
	{
		if (SOCMNOBJECT_TYPE_SUBCONDITION_CLASS != (type & SOCMNOBJECT_TYPE_SUBCONDITION_CLASS))
		{
			return FALSE;
		}

		SOCmnString name = pObject->getName();
		DWORD cnt;
		SOCltAutoMemory<LPWSTR> wszNames;
		server->beforeInterfaceCall();
		res = eventServer->QuerySubConditionNames(T2W((LPTSTR)(LPCTSTR)name), &cnt, &wszNames);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("IOPCEventServer::QuerySubConditionNames [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		for (DWORD i = 0 ; i < cnt; i++)
		{
			SOAeCSubConditionClass* cond;
			SOCMN_ALLOCATE_OBJECT(cond, SOAeCSubConditionClass((SOAeCConditionClass*)pObject, wszNames[i]));
			::CoTaskMemFree(wszNames[i]);
			objList.add(static_cast<SOCltBrowseObject*>(cond));
			cond->release();
		}
	} // SOCMNOBJECT_TYPE_CONDITION_CLASS

	return TRUE;
}



//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the OPC Server
//
// returns:
//		S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SOAeCComEventBrowserImpl::queryInterface(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	HRESULT res = E_NOINTERFACE;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_NOINTERFACE;
	}

	if (IID_IOPCEventServer == riid)
	{
		res = !m_eventServer ? E_NOINTERFACE : m_eventServer.QueryInterface((IOPCEventServer**)interf);
	}

	return res;
} // queryInterface




//-----------------------------------------------------------------------------
// SOAeCComEventBrowser                                                        |
//-----------------------------------------------------------------------------

SOAeCComEventBrowser::SOAeCComEventBrowser(SOAeCServer* pServer)
	: SOAeCEventBrowser(pServer),
	  SOAeCComEventBrowserImpl(pServer)
{
}

#endif // SOAEC
