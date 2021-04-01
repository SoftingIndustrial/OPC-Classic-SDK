#include "stdafx.h"

#ifdef SOAEC

#ifdef SOFEATURE_TUNNEL

#include "SOAeCBrowse.h"
#include "SOAeCTPBrowse.h"
#include "SOAeCTPCall.h"

//-----------------------------------------------------------------------------
// SOAeCTPAreaBrowserImpl                                                    |
//-----------------------------------------------------------------------------

SOAeCTPAreaBrowserImpl::SOAeCTPAreaBrowserImpl(SOAeCServer* server)
{
	m_serverHandle = 0;
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	server->beforeInterfaceCall();
	res = tpCall->addAreaBrowser(m_serverHandle);
	server->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::AddAreaBrowser [0x%X]"), res));
	m_server = server;
	m_server.addRef();
}

SOAeCTPAreaBrowserImpl::~SOAeCTPAreaBrowserImpl()
{
	if (m_serverHandle != 0)
	{
		SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)m_server->getTPConnection();
		SOCmnPointer<SOAeCTPCall> tpCall;
		HRESULT res;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
		m_server->beforeInterfaceCall();
		res = tpCall->removeAreaBrowser(m_serverHandle);
		m_server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, m_server, (_T("TPAE::Server::RemoveAreaBrowser [0x%X]"), res));
	}

	m_server.release();
}

BOOL SOAeCTPAreaBrowserImpl::expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SOAeCEntry> entryAeC = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entryAeC->getCreator();
	SOAeCAreaBrowser* pBrowser = getObjectSOAeCAreaBrowser();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pBrowser->getServer();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCmnList<SOAeCTPCallData> dataList;
	SOCmnListPosition posD;
	SOAeCTPCallData* pData;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	if ((pObject->is(SOCMNOBJECT_TYPE_AREA_BROWSER)) || (pObject->is(SOCMNOBJECT_TYPE_AREA)))
	{
		// browse to root
		server->beforeInterfaceCall();
		res = tpCall->changeBrowsePosition(m_serverHandle, OPCAE_BROWSE_TO, _T(""));
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_TO [0x%X]"), res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		if (pObject->is(SOCMNOBJECT_TYPE_AREA))
		{
			// browse down to the parent
			SOCmnStringList pathList = ((SOAeCArea*)pObject)->getPathList();
			SOCmnListPosition pos;
			SOCmnString pathPart;
			pos = pathList.getStartPosition();

			while (pos)
			{
				pathPart = pathList.getNext(pos);
				server->beforeInterfaceCall();
				res = tpCall->changeBrowsePosition(m_serverHandle, OPCAE_BROWSE_DOWN, (LPCTSTR)pathPart);
				server->afterInterfaceCall();
				_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

				if (FAILED(res))
				{
					server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
					return FALSE;
				}
			}

			pathPart = ((SOAeCArea*)pObject)->getName();
			server->beforeInterfaceCall();
			res = tpCall->changeBrowsePosition(m_serverHandle, OPCAE_BROWSE_DOWN, (LPCTSTR)pathPart);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN %s [0x%X]"), pathPart, res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}
		}

		if (SOCMNOBJECT_TYPE_AREA == (type & SOCMNOBJECT_TYPE_AREA))
		{
			SOCmnPointer<SOAeCArea> area;
			server->beforeInterfaceCall();
			res = tpCall->browseOPCAreas(m_serverHandle, OPC_AREA, _T(""), &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::BrowseOPCAreas OPC_AREA [0x%X]"), res));

			if (SUCCEEDED(res))
			{
				posD = dataList.getStartPosition();

				while (posD)
				{
					pData = dataList.getNext(posD);
					area = creator->createArea(pObject, pData->m_description);
					objList.add(area);
				}
			}
			else
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}
		}

		if (SOCMNOBJECT_TYPE_SOURCE == (type & SOCMNOBJECT_TYPE_SOURCE))
		{
			SOCmnString filterCriteria(pBrowser->getElementNameFilter());
			SOCmnPointer<SOAeCSource> source;
			server->beforeInterfaceCall();
			res = tpCall->browseOPCAreas(m_serverHandle, OPC_SOURCE, filterCriteria, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::BrowseOPCAreas OPC_SOURCE [0x%X]"), res));

			if (SUCCEEDED(res))
			{
				posD = dataList.getStartPosition();

				while (posD)
				{
					pData = dataList.getNext(posD);
					source = creator->createSource(pObject, pData->m_description);
					objList.add(source);
				}
			}
			else
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}
		}
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		if (SOCMNOBJECT_TYPE_CONDITION != (SOCMNOBJECT_TYPE_CONDITION & type))
		{
			return FALSE;
		}

		SOCmnPointer<SOAeCCondition> cond;
		SOCmnString path = pObject->getPath();

		if (path.isEmpty())
		{
			return FALSE;
		}

		server->beforeInterfaceCall();
		res = tpCall->querySourceConditions(path, &dataList);
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QuerySourceConditions %s [0x%X]"), path, res));

		if (SUCCEEDED(res))
		{
			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				cond = creator->createCondition(pObject, pData->m_description);
				objList.add(cond);
			}
		}
		else
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}
	}

	return TRUE;
}

SOCmnString SOAeCTPAreaBrowserImpl::getPathImpl(IN SOCltBrowseObject* pObject)
{
	SOAeCAreaBrowser* pBrowser = getObjectSOAeCAreaBrowser();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pBrowser->getServer();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCmnString path;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	// browse to root
	server->beforeInterfaceCall();
	res = tpCall->changeBrowsePosition(m_serverHandle, OPCAE_BROWSE_TO, _T(""));
	server->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_TO [0x%X]"), res));

	if (FAILED(res))
	{
		server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
		return path;
	}

	// browse down to the parent
	SOCmnStringList pathList = ((SOAeCArea*)pObject)->getPathList();
	SOCmnListPosition pos;
	SOCmnString pathPart;
	SOCmnString qName;
	pos = pathList.getStartPosition();

	while (pos)
	{
		pathPart = pathList.getNext(pos);
		server->beforeInterfaceCall();
		res = tpCall->changeBrowsePosition(m_serverHandle, OPCAE_BROWSE_DOWN, (LPCTSTR)pathPart);
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::ChangeBrowsePosition OPCAE_BROWSE_DOWN %s [0x%X]"), (LPCTSTR)pathPart, res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return path;
		}
	}

	// get the fully qualified name
	pathPart = pObject->getName();

	if (pObject->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		server->beforeInterfaceCall();
		res = tpCall->getQualifiedSourceName(m_serverHandle, pathPart, qName);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::GetQualifiedSourceName %s [0x%X]"), pathPart, res));
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_AREA))
	{
		server->beforeInterfaceCall();
		res = tpCall->getQualifiedAreaName(m_serverHandle, pathPart, qName);
		server->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::AreaBrowser::GetQualifiedAreaName %s [0x%X]"), pathPart, res));
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

	path = qName;
	return path;
}


//-----------------------------------------------------------------------------
// SOAeCTPAreaBrowser                                                        |
//-----------------------------------------------------------------------------

SOAeCTPAreaBrowser::SOAeCTPAreaBrowser(SOAeCServer* pServer)
	: SOAeCAreaBrowser(pServer),
	  SOAeCTPAreaBrowserImpl(pServer)
{
}



//-----------------------------------------------------------------------------
// SOAeCTPEventBrowserImpl                                                    |
//-----------------------------------------------------------------------------

SOAeCTPEventBrowserImpl::SOAeCTPEventBrowserImpl(SOAeCServer* /* pSrv */)
{
}

SOAeCTPEventBrowserImpl::~SOAeCTPEventBrowserImpl()
{
}

BOOL SOAeCTPEventBrowserImpl::expandImpl(IN SOCltBrowseObject* pObject, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SOCmnPointer<SOAeCEntry> entryAeC = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entryAeC->getCreator();
	SOAeCEventBrowser* pBrowser = getObjectSOAeCEventBrowser();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pBrowser->getServer();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCmnList<SOAeCTPCallData> dataList;
	SOCmnListPosition posD;
	SOAeCTPCallData* pData;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));

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
		SOCmnPointer<SOAeCCategory> cat;

		if ((filterCategories & OPC_SIMPLE_EVENT) == OPC_SIMPLE_EVENT)
		{
			server->beforeInterfaceCall();
			res = tpCall->queryEventCategories(OPC_SIMPLE_EVENT, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QueryEventCategories OPC_SIMPLE_EVENT [0x%X] "), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				cat = creator->createCategory(pObject, pData->m_description);
				cat->m_id = pData->m_id;
				cat->m_eventType = OPC_SIMPLE_EVENT;
				objList.add(cat);
			}
		}

		if ((filterCategories & OPC_TRACKING_EVENT) == OPC_TRACKING_EVENT)
		{
			server->beforeInterfaceCall();
			res = tpCall->queryEventCategories(OPC_TRACKING_EVENT, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QueryEventCategories OPC_TRACKING_EVENT [0x%X] "), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				cat = creator->createCategory(pObject, pData->m_description);
				cat->m_id = pData->m_id;
				cat->m_eventType = OPC_TRACKING_EVENT;
				objList.add(cat);
			}
		}

		if ((filterCategories & OPC_CONDITION_EVENT) == OPC_CONDITION_EVENT)
		{
			server->beforeInterfaceCall();
			res = tpCall->queryEventCategories(OPC_CONDITION_EVENT, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QueryEventCategories OPC_CONDITION_EVENT [0x%X] "), res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				cat = creator->createCategory(pObject, pData->m_description);
				cat->m_id = pData->m_id;
				cat->m_eventType = OPC_CONDITION_EVENT;
				objList.add(cat);
			}
		}
	} // SOCMNOBJECT_TYPE_EVENT_BROWSER
	else if (pObject->is(SOCMNOBJECT_TYPE_CATEGORY))
	{
		DWORD catID = ((SOAeCCategory*)pObject)->getCategoryID();
		DWORD eventType = ((SOAeCCategory*)pObject)->getEventType();
		SOCmnPointer<SOAeCAttribute> attr;
		SOCmnPointer<SOAeCConditionClass> condC;

		if (SOCMNOBJECT_TYPE_ATTRIBUTE == (type & SOCMNOBJECT_TYPE_ATTRIBUTE))
		{
			server->beforeInterfaceCall();
			res = tpCall->queryEventAttributes(catID, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QueryEventAttributes category: %lu [0x%X] "), catID, res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				SOCMN_ALLOCATE_OBJECT_C(attr, SOAeCAttribute(pObject, pData->m_description), (SOAeCAttribute*));
				attr->m_id = pData->m_id;
				attr->m_type = pData->m_datatype;
				objList.add(attr);
			}
		}

		if ((SOCMNOBJECT_TYPE_CONDITION_CLASS == (type & SOCMNOBJECT_TYPE_CONDITION_CLASS)) &&
			(eventType == OPC_CONDITION_EVENT))
		{
			server->beforeInterfaceCall();
			res = tpCall->queryConditionNames(catID, &dataList);
			server->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QueryConditionNames category: %lu [0x%X] "), catID, res));

			if (FAILED(res))
			{
				server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
				return FALSE;
			}

			posD = dataList.getStartPosition();

			while (posD)
			{
				pData = dataList.getNext(posD);
				SOCMN_ALLOCATE_OBJECT_C(condC, SOAeCConditionClass(pObject, pData->m_description), (SOAeCConditionClass*));
				objList.add(condC);
			}
		}
	} // SOCMNOBJECT_TYPE_CATEGORY
	else if (pObject->is(SOCMNOBJECT_TYPE_CONDITION_CLASS))
	{
		if (SOCMNOBJECT_TYPE_SUBCONDITION_CLASS != (type & SOCMNOBJECT_TYPE_SUBCONDITION_CLASS))
		{
			return FALSE;
		}

		SOCmnString condName = pObject->getName();
		SOCmnPointer<SOAeCSubConditionClass> subCondC;
		server->beforeInterfaceCall();
		res = tpCall->querySubConditionNames(condName, &dataList);
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPAE::Server::QuerySubConditionNames condition: %s [0x%X] "), condName, res));

		if (FAILED(res))
		{
			server->setLastError(res, SOCLT_ERR_BROWSE_UNEXPECTED);
			return FALSE;
		}

		posD = dataList.getStartPosition();

		while (posD)
		{
			pData = dataList.getNext(posD);
			SOCMN_ALLOCATE_OBJECT_C(subCondC, SOAeCSubConditionClass(pObject, pData->m_description), (SOAeCSubConditionClass*));
			objList.add(subCondC);
		}
	} // SOCMNOBJECT_TYPE_CONDITION_CLASS

	return TRUE;
}


//-----------------------------------------------------------------------------
// SOAeCTPEventBrowser                                                        |
//-----------------------------------------------------------------------------

SOAeCTPEventBrowser::SOAeCTPEventBrowser(SOAeCServer* pServer)
	: SOAeCEventBrowser(pServer),
	  SOAeCTPEventBrowserImpl(pServer)
{
}

#endif // SOFEATURE_TUNNEL

#endif // SOAEC
