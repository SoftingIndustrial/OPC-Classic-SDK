#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCServer.h"
#include "SOAeCComServer.h"

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SOAeCComServerImpl                                                             |
//-----------------------------------------------------------------------------

SOAeCComServerImpl::SOAeCComServerImpl()
{
}

SOAeCComServerImpl::~SOAeCComServerImpl()
{
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//		returns of unadvise
//
HRESULT SOAeCComServerImpl::doDisconnectImpl(void)
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;

	// disconnect shutdown event sink
	if (pCltComImpl->m_shutdown)
	{
		pCltComImpl->m_shutdown->unadvise(pAeCServer->getConnectionMonitorStatus());
	}

	// release all server interfaces
	pCltComImpl->m_unknown = NULL;
	pCltComImpl->m_common = NULL;
	pCltComImpl->m_secPrivate = NULL;
	m_server = NULL;
	pAeCServer->setObjectState(SOCltElement::disconnected);
	return res;
} // doDisconnectImpl

//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//		TRUE  - transit to new state
//		FALSE - error during state change
//
HRESULT SOAeCComServerImpl::doStateTransitionImpl()
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;
	BYTE targetState = pAeCServer->getTargetObjectState();

	while ((SUCCEEDED(res)) && (pAeCServer->getObjectState() != targetState))
	{
		switch (pAeCServer->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			// disconnected
			//-- connect server --
			MULTI_QI mqi[4];
			COSERVERINFO sin, *sinptr;
			SOCmnString errDescr;
			USES_CONVERSION;
			::ZeroMemory(mqi, sizeof(mqi));

			// create OPC server object via COM
			if (!pAeCServer->m_nodeName.IsEmpty())
			{
				// remote server
				sinptr = &sin;
				sin.dwReserved1 = 0;
				sin.dwReserved2 = 0;
				sin.pwszName = T2W((LPTSTR)((LPCTSTR)pAeCServer->m_nodeName));
				sin.pAuthInfo = 0;
			}
			else
			{
				// local server, inproc server or inproc handler
				sinptr = 0;
			}

			mqi[0].pIID = &IID_IUnknown;
			mqi[1].pIID = &IID_IOPCCommon;
			mqi[2].pIID = &IID_IOPCEventServer;
			mqi[3].pIID = &IID_IOPCSecurityPrivate;

			if ((SUCCEEDED(res)) && (pAeCServer->getConnectionMonitorStatus()))
			{
				res = 0x800706ba; // The RPC server is unavailable.
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("::CoCreateInstanceEx skipped, ConnectionMonitor is marked disconnected: [0x%X] "), res));
			}

			if (SUCCEEDED(res))
			{
				pAeCServer->beforeInterfaceCall();
				res = ::CoCreateInstanceEx(pCltComImpl->m_clsid, NULL, pCltComImpl->m_clsctx, sinptr, _COUNTOF(mqi), mqi);
				pAeCServer->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("::CoCreateInstanceEx() [0x%X] "), res));
			}

			if (SUCCEEDED(res))
			{
				// created object in OPC server
				pCltComImpl->m_unknown = SUCCEEDED(mqi[0].hr) ? (IUnknown*)mqi[0].pItf : 0;
				pCltComImpl->m_common = SUCCEEDED(mqi[1].hr) ? (IOPCCommon*)mqi[1].pItf : 0;
				m_server = SUCCEEDED(mqi[2].hr) ? (IOPCEventServer*)mqi[2].pItf : 0;
				pCltComImpl->m_secPrivate = SUCCEEDED(mqi[3].hr) ? (IOPCSecurityPrivate*)mqi[3].pItf : 0;

				if (SUCCEEDED(mqi[0].hr))
				{
					mqi[0].pItf->Release();
				}

				if (SUCCEEDED(mqi[1].hr))
				{
					mqi[1].pItf->Release();
				}

				if (SUCCEEDED(mqi[2].hr))
				{
					mqi[2].pItf->Release();
				}

				if (SUCCEEDED(mqi[3].hr))
				{
					mqi[3].pItf->Release();
				}
			}

			res = S_OK; // ignore not provided interfaces for return
			// logon
			pCltComImpl->logon();

			if (SUCCEEDED(res) && pCltComImpl->m_shutdown)
			{
				// connect shutdown event sink
				res = pCltComImpl->m_shutdown->advise();
			}

			// get/set server object attributes
			if (SUCCEEDED(res))
			{
				// set valid object attributes
				if (pAeCServer->m_validParameter)
				{
					res = pAeCServer->doUpdate(FALSE, pAeCServer->m_validParameter);
				}
			}
			else
			{
				pAeCServer->setLastError(res, SOCLT_ERR_CREATE_SERVER);
			}

			if (SUCCEEDED(res))
			{
				pAeCServer->setObjectState(SOCltElement::connected);
				pAeCServer->setLastError(S_OK, 0);
			}
			else
			{
				// something failed -> release actions
				pAeCServer->doDisconnect();
			}
		}
		break;

		case SOCltElement::started:
		case SOCltElement::connected:
		{
			if (targetState == SOCltElement::disconnected)
			{
				//-- disconnect server --
				res = pAeCServer->doDisconnect();
				pAeCServer->setLastError(res, SOCLT_ERR_DISCONNECT);
			}
			else
			{
				// start server
				pAeCServer->setObjectState(targetState);
				pAeCServer->setLastError(S_OK, 0);
			}
		}
		break;
		}
	}

	return res;
} // doStateTransition



HRESULT SOAeCComServerImpl::doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters)
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	CComPtr<IOPCCommon> pIOPCCommon;

	if (FAILED(res = pCltComImpl->m_common.getInterface(&pIOPCCommon)))
	{
		return res;
	}

	if (fromServer)
	{
		if (whatParameters & SOCLT_SRVPAR_LCID)
		{
			// get locale id from the server
			LCID lcid;
			pAeCServer->beforeInterfaceCall();
			res = pIOPCCommon->GetLocaleID(&lcid);
			pAeCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCCommon::GetLocaleID [0x%X] "), res));
			pAeCServer->setLastError(res, SOCLT_ERR_GET_LOCALE_ID);

			if (SUCCEEDED(res))
			{
				DWORD validPars = pAeCServer->m_validParameter;
				pAeCServer->setLCID(lcid);
				pAeCServer->m_validParameter = validPars;
			}
		}
	}
	else
	{
		if (SUCCEEDED(res) && (whatParameters & SOCLT_SRVPAR_LCID))
		{
			// set locale id to the server
			pAeCServer->beforeInterfaceCall();
			res = pIOPCCommon->SetLocaleID(pAeCServer->m_lcid);
			pAeCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCCommon::SetLocaleID [0x%X] "), res));
			pAeCServer->setLastError(res, SOCLT_ERR_SET_LOCALE_ID);
		}

		if (SUCCEEDED(res) && (whatParameters & SOCLT_SRVPAR_CLIENT_NAME))
		{
			// set client name to the server
			USES_CONVERSION;
			SOCmnString cname;
			cname = pAeCServer->getClientName();
			pAeCServer->beforeInterfaceCall();
			res = pIOPCCommon->SetClientName(T2CW((LPCTSTR)cname));
			pAeCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCCommon::SetClientName [0x%X] "), res));
			pAeCServer->setLastError(res, SOCLT_ERR_SET_CLIENT_NAME);
		}
	}

	return res;
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
HRESULT SOAeCComServerImpl::queryInterfaceImpl(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_NOINTERFACE;
	}

	if (!pCltComImpl->m_unknown)
	{
		return E_NOINTERFACE;
	}

	if (IID_IUnknown == riid)
	{
		res = pCltComImpl->m_unknown.getInterface((IUnknown**)interf);
	}
	else if (IID_IOPCCommon == riid)
	{
		res = pCltComImpl->m_common.getInterface((IOPCCommon**)interf);
	}
	else if (IID_IOPCEventServer == riid)
	{
		res = m_server.getInterface((IOPCEventServer**)interf);
	}
	else
	{
		CComPtr<IUnknown> pUnk;
		res = pCltComImpl->m_unknown.getInterface(&pUnk);

		if (SUCCEEDED(res))
		{
			pAeCServer->beforeInterfaceCall();
			res = pUnk->QueryInterface(riid, (void**)interf);
			pAeCServer->afterInterfaceCall();
#ifndef _DEBUG
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IUnknown::QueryInterface [0x%X] "), res));
#else
			SOCmnString iidString;
			IID2string(&iidString, riid);
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IUnknown::QueryInterface %s [0x%X] "), (LPCTSTR)iidString, res));
#endif //_DEBUG
		}
	}

	pAeCServer->setLastError(res, SOCLT_ERR_COM_QI);
	return res;
} // queryInterface


SOAeCAreaBrowser* SOAeCComServerImpl::getAreaBrowserImpl()
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCAreaBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCComAreaBrowser(pAeCServer));
	return (SOAeCAreaBrowser*)p;
}

SOAeCEventBrowser* SOAeCComServerImpl::getEventBrowserImpl(void)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCEventBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCComEventBrowser(pAeCServer));
	return p;
}



SOCltTask* SOAeCComServerImpl::createAckTask(IN SOCmnList<SOAeCEvent> *events, IN LPCTSTR acknowledgerID, IN LPCTSTR comment)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCComAckTask* pTask;
	SOCMN_ALLOCATE_OBJECT(pTask, SOAeCComAckTask(pAeCServer, events, acknowledgerID, comment));
	return pTask;
}


// IOPCEventServer  interface
HRESULT SOAeCComServerImpl::getStatus(OUT SOAeCServerStatus& status)
{
	OPCEVENTSERVERSTATUS serverStatus;
	SOCmnString vendorInfo;
	HRESULT res;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	status.clear();
	res = getStatus(&serverStatus, &vendorInfo);

	if (SUCCEEDED(res))
	{
		status.m_vendorInfo = vendorInfo;
		status.m_state = serverStatus.dwServerState;
		status.m_productVersion.format(_T("%u.%2.2u.%4.4u"), serverStatus.wMajorVersion, serverStatus.wMinorVersion, serverStatus.wBuildNumber);
		status.m_currentTime.set(&serverStatus.ftCurrentTime);
		status.m_startTime.set(&serverStatus.ftStartTime);
		status.m_lastUpdateTime.set(&serverStatus.ftLastUpdateTime);
	}

	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	pCltComImpl->queryAvailableLCIDs(status.m_supportedLCIDs);
	return res;
}

//-----------------------------------------------------------------------------
//
// getStatus

HRESULT SOAeCComServerImpl::getStatus(OUT OPCEVENTSERVERSTATUS* serverStatus, OUT SOCmnString* vendorInfo)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = E_FAIL;
	OPCEVENTSERVERSTATUS* status;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	// check output parameter
	if (NULL == serverStatus)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_OUTPUT);
		return E_INVALIDARG;
	}

	if (!m_server)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	// call function on OPC Server
	pAeCServer->beforeInterfaceCall();
	res = server->GetStatus(&status);
	pAeCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::GetStatus [0x%X]"), res));
	pAeCServer->setLastError(res, SOCLT_ERR_SRV_GET_STATUS);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		// copy status
		memcpy(serverStatus, status, sizeof(OPCEVENTSERVERSTATUS));
		serverStatus->szVendorInfo = NULL;  // vendor info only in string

		// copy vendor info
		if (vendorInfo)
		{
			*vendorInfo = W2CT(status->szVendorInfo);
		}

		CoTaskMemFree(status->szVendorInfo);
		CoTaskMemFree(status);
	}

	return res;
}

//-----------------------------------------------------------------------------
//
// enableConditionByArea

HRESULT SOAeCComServerImpl::enableConditionByArea(LPCTSTR area, IN BOOL enable)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	if (NULL == area)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	USES_CONVERSION;
	LPWSTR lpWarea = T2W((LPTSTR)area);

	if (enable)
	{
		pAeCServer->beforeInterfaceCall();
		res = server->EnableConditionByArea(1, &lpWarea);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::EnableConditionByArea [0x%X]"), res));
	}
	else
	{
		pAeCServer->beforeInterfaceCall();
		res = server->DisableConditionByArea(1, &lpWarea);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::DisableConditionByArea [0x%X]"), res));
	}

	return res;
}

HRESULT SOAeCComServerImpl::enableConditionByArea(SOCmnStringList* areas, BOOL enable)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;
	USES_ALLOCA_WSTR;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	if (NULL == areas)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	LPWSTR _wszNULL = L"";
	DWORD dwCount = areas->getCount();
	LPWSTR* lpwszAreas = &_wszNULL;

	if (dwCount)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	lpwszAreas = (LPWSTR*)alloca(sizeof(LPWSTR) * dwCount);

	if (0 == lpwszAreas)
	{
		pAeCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return res;
	}

	SOCmnListPosition pos = areas->GetHeadPosition();

	for (DWORD i = 0 ; pos ; i++)
	{
		SOCmnString str = areas->getNext(pos);
		lpwszAreas[i] = ALLOCA_WSTR((LPCTSTR)str);
	}

	if (enable)
	{
		pAeCServer->beforeInterfaceCall();
		res = server->EnableConditionByArea(dwCount, lpwszAreas);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::EnableConditionByArea [0x%X]"), res));
	}
	else
	{
		pAeCServer->beforeInterfaceCall();
		res = server->DisableConditionByArea(dwCount, lpwszAreas);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::DisableConditionByArea [0x%X]"), res));
	}

	pAeCServer->setLastError(res, SOAEC_ERR_ENABLE_CONDITIONS);
	return res;
}

//-----------------------------------------------------------------------------
//
// enableConditionBySource

HRESULT SOAeCComServerImpl::enableConditionBySource(LPCTSTR source, IN BOOL enable)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	if (NULL == source)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	LPWSTR lpWsrc = T2W((LPTSTR)source);

	if (enable)
	{
		pAeCServer->beforeInterfaceCall();
		res = server->EnableConditionBySource(1, &lpWsrc);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::EnableConditionBySource [0x%X]"), res));
	}
	else
	{
		pAeCServer->beforeInterfaceCall();
		res = server->DisableConditionBySource(1, &lpWsrc);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::DisableConditionBySource [0x%X]"), res));
	}

	return res;
}

//-----------------------------------------------------------------------------
//
// enableConditionBySource

HRESULT SOAeCComServerImpl::enableConditionBySource(IN SOCmnStringList* sources, IN BOOL enable)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;
	USES_ALLOCA_WSTR;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	if (NULL == sources)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	LPWSTR _wszNull = L"";
	DWORD dwCount = sources->getCount();
	LPWSTR* lpwszSources = &_wszNull;

	if (0 == dwCount)
	{
		pAeCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	lpwszSources = (LPWSTR*)alloca(sizeof(LPWSTR) * dwCount);

	if (0 == lpwszSources)
	{
		pAeCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return res;
	}

	SOCmnListPosition pos = sources->GetHeadPosition();

	for (DWORD i = 0 ; pos ; i++)
	{
		SOCmnString str = sources->getNext(pos);
		lpwszSources[i] = ALLOCA_WSTR((LPCTSTR)str);
	}

	if (enable)
	{
		pAeCServer->beforeInterfaceCall();
		res = server->EnableConditionBySource(dwCount, lpwszSources);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::EnableConditionBySource [0x%X]"), res));
	}
	else
	{
		pAeCServer->beforeInterfaceCall();
		res = server->DisableConditionBySource(dwCount, lpwszSources);
		pAeCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::DisableConditionBySource [0x%X]"), res));
	}

	pAeCServer->setLastError(res, SOAEC_ERR_ENABLE_CONDITIONS);
	return res;
}



//-----------------------------------------------------------------------------
//
// translateToItemIDs

HRESULT SOAeCComServerImpl::translateToItemIDs(
	IN LPCTSTR source,
	IN DWORD eventCategory,
	IN LPCTSTR conditionName,
	IN LPCTSTR subconditionName,
	IN SOCmnDWORDList* assocAttrIDs,
	OUT SOCmnList<SOAeCTranslateResult>& translateResults)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	USES_CONVERSION;
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	_ASSERTION(source, "NULL pointer passed.");
	_ASSERTION(conditionName, "NULL pointer passed.");
	_ASSERTION(subconditionName, "NULL pointer passed.");
	_ASSERTION(assocAttrIDs, "NULL pointer passed.");
	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	DWORD cnt = assocAttrIDs->getCount();
	DWORD* dwAttrIDs = (DWORD*)_alloca(cnt * sizeof(DWORD));

	if (NULL == dwAttrIDs)
	{
		pAeCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	SOCmnListPosition pos = assocAttrIDs->getStartPosition();
	DWORD i;

	for (i = 0; pos; i++)
	{
		dwAttrIDs[i] = assocAttrIDs->getNext(pos);
	}

	LPWSTR* szAttrItemIDs = 0;
	LPWSTR* szNodeNames = 0;
	CLSID* clsids = 0;
	pAeCServer->beforeInterfaceCall();
	res = server->TranslateToItemIDs(T2W((LPTSTR)source), eventCategory,
									 T2W((LPTSTR)conditionName), T2W((LPTSTR)subconditionName),
									 cnt, dwAttrIDs,
									 &szAttrItemIDs, &szNodeNames, &clsids);
	pAeCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::TranslateToItemIDs [0x%X]"), res));

	if (FAILED(res))
	{
		pAeCServer->setLastError(res, SOAEC_ERR_TRANSLATE_TO_ITEM_IDS);
		return res;
	}

	translateResults.create(SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_LIST);
	translateResults.allocateBlock(cnt);

	for (i = 0 ; i < cnt; i++)
	{
		SOAeCTranslateResult* pTR = new SOAeCTranslateResult;

		if (!pTR)
		{
			pAeCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return NULL;
		}

		pTR->m_attrItemID.fromWCHAR(szAttrItemIDs[i]);
		pTR->m_nodeName.fromWCHAR(szNodeNames[i]);
		pTR->m_clsid = clsids[i];
		translateResults.add(pTR);
	}

	pAeCServer->setLastError(S_OK, 0);
	return S_OK;
}

//-----------------------------------------------------------------------------
//
// getConditionState

HRESULT SOAeCComServerImpl::getConditionState(
	IN LPCTSTR source,
	IN LPCTSTR condName,
	IN OPTIONAL SOCmnDWORDList* eventAttrIDs /*=NULL*/,
	OUT SOAeCConditionState& state)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	HRESULT res = S_OK;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	_ASSERTION(source, "NULL pointer passed.");
	_ASSERTION(condName, "NULL pointer passed.");
	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	DWORD cnt = 0;
	DWORD* dwAttrIDs = &cnt;

	if (!!eventAttrIDs)
	{
		cnt = eventAttrIDs->getCount();
		dwAttrIDs = (DWORD*)_alloca(cnt * sizeof(DWORD));

		if (NULL == dwAttrIDs)
		{
			pAeCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		SOCmnListPosition pos = eventAttrIDs->getStartPosition();

		for (DWORD i = 0; pos; i++)
		{
			dwAttrIDs[i] = eventAttrIDs->getNext(pos);
		}
	}

	OPCCONDITIONSTATE* pcst = 0;
	pAeCServer->beforeInterfaceCall();
	res = server->GetConditionState(T2W((LPTSTR)source), T2W((LPTSTR)condName), cnt, dwAttrIDs, &pcst);
	pAeCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::GetConditionState [0x%X]"), res));

	if (FAILED(res))
	{
		pAeCServer->setLastError(res, SOAEC_ERR_GET_CONDITION_STATE);
		return res;
	}

	state.m_state = pcst->wState;
	state.m_asc.m_name = W2T(pcst->szActiveSubCondition);
	state.m_asc.m_description = W2T(pcst->szASCDescription);
	state.m_asc.m_definition = W2T(pcst->szASCDefinition);
	state.m_asc.m_severity = pcst->dwASCSeverity;
	state.m_quality = pcst->wQuality;
	state.m_lastAckTime = pcst->ftLastAckTime;
	state.m_subCondLastActive = pcst->ftSubCondLastActive;
	state.m_condLastActive = pcst->ftCondLastActive;
	state.m_condLastInactive = pcst->ftCondLastInactive;
	state.m_acknowledgerID = W2T(pcst->szAcknowledgerID);
	state.m_comment = W2T(pcst->szComment);
	::CoTaskMemFree(pcst->szActiveSubCondition);
	::CoTaskMemFree(pcst->szASCDefinition);
	::CoTaskMemFree(pcst->szASCDescription);
	::CoTaskMemFree(pcst->szAcknowledgerID);
	::CoTaskMemFree(pcst->szComment);
	state.m_subconds.create(SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_LIST);

	for (DWORD dwNumSCs = 0; dwNumSCs < pcst->dwNumSCs; dwNumSCs++)
	{
		SOAeCSubConditionState* pscs = 0;
		SOCMN_ALLOCATE_OBJECT(pscs, SOAeCSubConditionState);
		pscs->m_name.fromWCHAR(pcst->pszSCNames[dwNumSCs]);
		pscs->m_description.fromWCHAR(pcst->pszSCDefinitions[dwNumSCs]);
		pscs->m_definition.fromWCHAR(pcst->pszSCDescriptions[dwNumSCs]);
		pscs->m_severity = pcst->pdwSCSeverities[dwNumSCs];
		::CoTaskMemFree(pcst->pszSCNames[dwNumSCs]);
		::CoTaskMemFree(pcst->pszSCDefinitions[dwNumSCs]);
		::CoTaskMemFree(pcst->pszSCDescriptions[dwNumSCs]);
		state.m_subconds.add(pscs);
	}

	state.m_eventAttrs.create(SOCMNLIST_TYPE_OBJ_VARIANT | SOCMNLIST_TYPE_ORG_LIST);

	for (DWORD dwNumEventAttrs = 0; dwNumEventAttrs < pcst->dwNumEventAttrs; dwNumEventAttrs++)
	{
		state.m_eventAttrs.add((SOCmnVariant*)(pcst->pEventAttributes + dwNumEventAttrs));
		::VariantClear(pcst->pEventAttributes + dwNumEventAttrs);
	}

	::CoTaskMemFree(pcst->pEventAttributes);
	::CoTaskMemFree(pcst->pErrors);
	::CoTaskMemFree(pcst);
	pAeCServer->setLastError(S_OK, 0);
	return S_OK;
}

HRESULT SOAeCComServerImpl::queryAvailableFilters(OUT DWORD* pFilterMask)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	USES_CONVERSION;
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	CComPtr<IOPCEventServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	pAeCServer->beforeInterfaceCall();
	res = server->QueryAvailableFilters(pFilterMask);
	pAeCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("IOPCEventServer::QueryAvailableFilters [0x%X]"), res));
	return res;
}

SOCltComServerImpl* SOAeCComServerImpl::getObjectCltComServerImpl(void)
{
	return NULL;
}



//-----------------------------------------------------------------------------
// SOAeCComServerDefault                                                      |
//-----------------------------------------------------------------------------

SOAeCComServerDefault::SOAeCComServerDefault(void)
	: SOAeCServer()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SOAeCComServerDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


SOCltComServerImpl* SOAeCComServerDefault::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}




//-----------------------------------------------------------------------------
// SOAeCComAckTask                                                          |
//-----------------------------------------------------------------------------

SOAeCComAckTask::SOAeCComAckTask(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events, LPCTSTR acknowledgerID, LPCTSTR comment)
	:   SOCltTask((SOCltElement*)pServer)
{
	_ASSERT_PTR(events);
	m_listEvents.clone(*events);

	if (acknowledgerID)
	{
		m_ackID = acknowledgerID;
	}
	else
	{
#ifndef SOOS_WINDOWS_CE
		TCHAR userName[256];
		DWORD size = 256;

		if (GetUserName(userName, &size))
		{
			m_ackID = userName;
		}

#endif
	}

	m_comment = comment;
}

SOAeCComAckTask::~SOAeCComAckTask()
{
	if (m_listEvents.isInit())
	{
		m_listEvents.destroy();
	}
}

//-----------------------------------------------------------------------------
//
// execute

void SOAeCComAckTask::execute(void)
{
	HRESULT res = S_OK;
	// Use the first in list to find out the server
	SOAeCServer* srv = (SOAeCServer*)(SOCltElement*)m_element;
	CComPtr<IOPCEventServer> pSrv;
	res = srv->queryInterface(IID_IOPCEventServer, (void**)&pSrv);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		LPWSTR wszAckID = m_ackID.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_ackID);
		LPWSTR wszComment = m_comment.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_comment);
		DWORD dwCount = m_listEvents.getCount();
		LPWSTR* wszSources = (LPWSTR*)_alloca(dwCount * sizeof(LPWSTR*));
		LPWSTR* wszConditions = (LPWSTR*)_alloca(dwCount * sizeof(LPWSTR*));
		FILETIME* ftTimes = (FILETIME*)_alloca(dwCount * sizeof(FILETIME));
		DWORD* dwCookies = (DWORD*)_alloca(dwCount * sizeof(DWORD));
		SOCmnListPosition pos = m_listEvents.getStartPosition();
		int i;

		for (i = 0; pos; i++)
		{
			SOAeCEvent* event = m_listEvents.getNext(pos);
			ftTimes[i] = event->m_ftActiveTime;
			dwCookies[i] = event->m_dwCookie;
			wszSources[i] = T2W((LPTSTR)(LPCTSTR)event->m_source);
			wszConditions[i] = T2W((LPTSTR)(LPCTSTR)event->m_conditionName);
		}

		SOCltAutoMemory<HRESULT> pErr;
		srv->beforeInterfaceCall();
		res = pSrv->AckCondition(dwCount, wszAckID, wszComment, wszSources , wszConditions,
								 ftTimes, dwCookies, &pErr);
		srv->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCEventServer::AckCondition [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			pos = m_listEvents.getStartPosition();
			i = 0;

			while (pos)
			{
				SOAeCEvent* event = m_listEvents.getNext(pos);
				event->m_ackRes = pErr[i++];
				INVOKE_CALLBACK_ON_PTR(event, SOAeCEvent, onEventAcknowledged)
			}
		}
	}

	setResult(res);
	return;
}

#endif // SOFEATURE_DCOM

#endif // SOAEC
