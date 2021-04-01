#include "stdafx.h"

#ifdef SOAEC

#ifdef SOFEATURE_DCOM

#include "SOAeCEntry.h"
#include "SOAeCServer.h"
#include "SOAeCSubscription.h"
#include "SOAeCComSubscription.h"
#include "SOAeCCallback.h"
#include "SOAeCEvent.h"


//-----------------------------------------------------------------------------
// SOAeCComSubscriptionImpl                                                   |
//-----------------------------------------------------------------------------

SOAeCComSubscriptionImpl::SOAeCComSubscriptionImpl()
{
	m_sink = NULL;
}

SOAeCComSubscriptionImpl::~SOAeCComSubscriptionImpl()
{
	if (m_sink)
	{
		m_sink->setSubscription(NULL);
		m_sink->Release();
		m_sink = NULL;
	}
}


HRESULT SOAeCComSubscriptionImpl::queryInterfaceImpl(IN REFIID riid, IN void** interf)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_NOINTERFACE;
	}

	if (!m_submgt)
	{
		return E_NOINTERFACE;
	}

	if ((IID_IUnknown == riid) || (IID_IOPCEventSubscriptionMgt == riid))
	{
		res = m_submgt.getInterface((IOPCEventSubscriptionMgt**)interf);
	}
	else
	{
		CComPtr<IOPCEventSubscriptionMgt> pUnk;
		res = m_submgt.getInterface(&pUnk);

		if (SUCCEEDED(res))
		{
			pAeCSubscription->beforeInterfaceCall();
			res = pUnk->QueryInterface(riid, (void**)interf);
			pAeCSubscription->afterInterfaceCall();
		}

#ifndef _DEBUG
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IUnknown::QueryInterface [0x%X] "), res));
#else
		SOCmnString iidString;
		IID2string(&iidString, riid);
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IUnknown::QueryInterface %s [0x%X] "), (LPCTSTR)iidString, res));
#endif  //  _DEBUG
	}

	return res;
}

HRESULT SOAeCComSubscriptionImpl::doStateTransitionImpl(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;
	SOCmnString resDescr;
	BYTE targetState = pAeCSubscription->getTargetObjectState();
	USES_CONVERSION;

	while ((SUCCEEDED(res)) && (pAeCSubscription->getObjectState() != targetState))
	{
		switch (pAeCSubscription->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			//-- connect or start group --
			SOCmnPointer<SOAeCServer> srv = pAeCSubscription->getServer();

			// check server object
			if (!srv)
			{
				pAeCSubscription->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
				return E_FAIL;
			}

			if (!srv->isConnected())
			{
				pAeCSubscription->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
				return E_FAIL;
			}

			CComPtr<IOPCEventServer> pIOPCServer;

			if (FAILED(res = srv->queryInterface(IID_IOPCEventServer, (void**)&pIOPCServer)))
			{
				pAeCSubscription->setLastError(res, SOCLT_ERR_COM_QI);
				return res;
			}

			if (targetState == SOCltElement::connected)
			{
				pAeCSubscription->m_bActive = FALSE;    // connect
			}
			else
			{
				pAeCSubscription->m_bActive = TRUE;    // start
			}

			// Use client object address as the client side handle
			OPCHANDLE hClientSubscription = (OPCHANDLE)pAeCSubscription->getOpcHandle();
			CComPtr<IOPCEventSubscriptionMgt> pMgt;
			pAeCSubscription->beforeInterfaceCall();
			res = pIOPCServer->CreateEventSubscription(pAeCSubscription->m_bActive,
													   pAeCSubscription->m_dwBufferTime,
													   pAeCSubscription->m_dwMaxSize,
													   hClientSubscription,
													   IID_IOPCEventSubscriptionMgt,
													   (IUnknown**)&pMgt,
													   &pAeCSubscription->m_dwRevisedBufferTime,
													   &pAeCSubscription->m_dwRevisedMaxSize);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::CreateEventSubscription [0x%X] "), res));
			pAeCSubscription->setLastError(res, SOAEC_ERR_ADD_SUBSCRIPTION);
			res = m_submgt.putInterface(pMgt, FALSE);

			// Set filter

			if (SUCCEEDED(res))
			{
				res = pAeCSubscription->doUpdate(FALSE, SOAEC_SUBSPAR_FILTER | SOAEC_SUBSPAR_RETURNED_ATTRIBUTES);
			}

			// Establish sink connection
			if (!m_sink)
			{
				SOCMN_ALLOCATE_OBJECT(m_sink, SOAeCEventSink(pAeCSubscription));
			}

			if (SUCCEEDED(res) && m_sink)
			{
				res = m_sink->advise();
			}

			if (SUCCEEDED(res))
			{
				pAeCSubscription->setObjectState(pAeCSubscription->m_bActive ? (BYTE)SOCltElement::started : (BYTE)SOCltElement::connected);

				// Invoke callbacks for revised values
				if (pAeCSubscription->m_dwBufferTime != pAeCSubscription->m_dwRevisedBufferTime)
				{
					INVOKE_CALLBACK_ON_PTR(pAeCSubscription, SOAeCSubscription, onSetRevisedBufferTime);
				}

				if (pAeCSubscription->m_dwMaxSize != pAeCSubscription->m_dwRevisedMaxSize)
				{
					INVOKE_CALLBACK_ON_PTR(pAeCSubscription, SOAeCSubscription, onSetRevisedMaxSize);
				}
			}
			else
			{
				// undo all if failed
				pAeCSubscription->doDisconnect();
			}
		}
		break;

		case SOCltElement::connected:
		case SOCltElement::started:
		{
			if (targetState == SOCltElement::disconnected)
			{
				res = pAeCSubscription->doDisconnect();
				pAeCSubscription->setLastError(res, SOCLT_ERR_DISCONNECT);
			}
			else
			{
				//-- start or stop group --
				pAeCSubscription->m_bActive = (targetState == SOCltElement::started) ? TRUE : FALSE;
				res = pAeCSubscription->doUpdate(FALSE, SOAEC_SUBSPAR_ACTIVE);

				if (SUCCEEDED(res))
				{
					pAeCSubscription->setObjectState(targetState);
				}
			}
		}
		break;
		}
	}

	return res;
}


HRESULT SOAeCComSubscriptionImpl::doDisconnectImpl()
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	// Disconnect shutdown event sink

	if (m_sink)
	{
		m_sink->unadvise(pAeCSubscription->getConnectionMonitorStatus());
		m_sink->setSubscription(NULL);
		m_sink->Release();
		m_sink = NULL;
	}

	// Release all server interfaces
	if (pAeCSubscription->getConnectionMonitorStatus())
	{
		m_submgt.putInterface(NULL, TRUE);
	}
	else
	{
		m_submgt = NULL;
	}
	pAeCSubscription->setObjectState(SOCltElement::disconnected);
	return res;
}

HRESULT SOAeCComSubscriptionImpl::doUpdateImpl(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	if (!m_submgt)
	{
		return E_NOINTERFACE;
	}

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_FILTER))
	{
		res = doUpdateFilter(fromServer);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_GET_FILTER : SOAEC_ERR_SET_FILTER);

		if (E_NOTIMPL == res)
		{
			// if the filtering is not implemented just ignore this
			_TRACEO(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::GetFilter E_FAIL ignored")));
			res = S_FALSE;
		}
	}

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_RETURNED_ATTRIBUTES))
	{
		res = doUpdateReturnedAttributes(fromServer);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_SUBS_GET_ATTR : SOAEC_ERR_SUBS_SET_ATTR);
	}

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_STATE))
	{
		res = doUpdateSubscriptionState(fromServer, whatParameters);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_SUBS_GET_STATE : SOAEC_ERR_SUBS_SET_STATE);
	}

	return res;
} // update

//----------------------------------------------------------------------------------
// Update helpers:
//
//		doUpdateFilter
//		doUpdateReturnedAttributes
//		doUpdateSubscriptionState
//

HRESULT SOAeCComSubscriptionImpl::doUpdateFilter(BOOL fromServer)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	_ASSERTION(!!m_submgt, "No interface");
	LPWSTR wszNullStr = L"";        // to avoid NULL COM pointers
	DWORD dwNull = 0;                       // to avoid NULL COM pointers
	LPWSTR* ppwszAreaList = &wszNullStr;
	LPWSTR* ppwszSourceList = &wszNullStr;
	LPDWORD pdwEventCategories = &dwNull;
	DWORD dwNumAreas = 0;
	DWORD dwNumSources = 0;
	DWORD dwNumEventCategories = 0;
	DWORD dwEventTypes = 0;
	DWORD dwLowServerity = 0;
	DWORD dwHighServerity = 0;
	CComPtr<IOPCEventSubscriptionMgt> pMgt;

	if (FAILED(res = m_submgt.getInterface(&pMgt)))
	{
		return res;
	}

	if (fromServer)
	{
		pAeCSubscription->beforeInterfaceCall();
		res = pMgt->GetFilter(
				  &dwEventTypes,
				  &dwNumEventCategories, &pdwEventCategories,
				  &dwLowServerity, &dwHighServerity,
				  &dwNumAreas, &ppwszAreaList,
				  &dwNumSources, &ppwszSourceList);
		pAeCSubscription->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::GetFilter [0x%X] "), res));

		if (FAILED(res))
		{
			return res;
		}

		pAeCSubscription->m_dwEventTypes = dwEventTypes;
		pAeCSubscription->m_dwLowSeverity = dwLowServerity;
		pAeCSubscription->m_dwHighSeverity = dwHighServerity;
		{
			SOCmnDWORDList list(pAeCSubscription->getCategoryList());
			list.removeAll();

			for (DWORD i = 0 ; i < dwNumEventCategories; i++)
			{
				list.add(pdwEventCategories[i]);
			}

			::CoTaskMemFree(pdwEventCategories);
		}
		{
			SOCmnStringList list(pAeCSubscription->getAreaList());
			list.removeAll();

			for (DWORD i = 0 ; i < dwNumAreas; i++)
			{
				SOCmnString s;
				s.fromWCHAR(ppwszAreaList[i]);
				list.add(s);
				::CoTaskMemFree(ppwszAreaList[i]);
			}

			::CoTaskMemFree(ppwszAreaList);
		}
		{
			SOCmnStringList list(pAeCSubscription->getSourceList());
			list.removeAll();

			for (DWORD i = 0 ; i < dwNumSources; i++)
			{
				SOCmnString s;
				s.fromWCHAR(ppwszSourceList[i]);
				list.add(s);
				::CoTaskMemFree(ppwszSourceList[i]);
			}

			::CoTaskMemFree(ppwszSourceList);
		}
	}
	else // to server
	{
		USES_ALLOCA_WSTR;
		res = S_OK;
		SOCmnListPosition pos;

		// category list

		if (SUCCEEDED(res))
		{
			SOCmnDWORDList list(pAeCSubscription->getCategoryList());
			pos = list.getStartPosition();
			dwNumEventCategories = list.getCount();

			if (0 != dwNumEventCategories)
			{
				pdwEventCategories = (LPDWORD)_alloca(dwNumEventCategories * sizeof(DWORD));

				if (NULL == pdwEventCategories)
				{
					res = E_OUTOFMEMORY;
				}

				for (DWORD i = 0 ; pos && SUCCEEDED(res) ; i++)
				{
					pdwEventCategories[i] = list.getNext(pos);
				}
			}
		}

		// source list

		if (SUCCEEDED(res))
		{
			SOCmnStringList list(pAeCSubscription->getSourceList());
			pos = list.getStartPosition();
			dwNumSources = list.getCount();

			if (0 != dwNumSources)
			{
				ppwszSourceList = (LPWSTR*)_alloca(dwNumSources * sizeof(LPWSTR));

				if (NULL == ppwszSourceList)
				{
					res = E_OUTOFMEMORY;
				}

				for (DWORD i = 0 ; pos && SUCCEEDED(res) ; i++)
				{
					SOCmnString str = list.getNext(pos);
					ppwszSourceList[i] = ALLOCA_WSTR((LPCTSTR)str);
				}
			}
		}

		// area list
		{
			SOCmnStringList list(pAeCSubscription->getAreaList());
			pos = list.getStartPosition();
			dwNumAreas = list.getCount();

			if (0 != dwNumAreas)
			{
				ppwszAreaList = (LPWSTR*)_alloca(dwNumAreas * sizeof(LPWSTR));

				if (NULL == ppwszAreaList)
				{
					res = E_OUTOFMEMORY;
				}

				for (DWORD i = 0 ; pos && SUCCEEDED(res) ; i++)
				{
					SOCmnString str = list.getNext(pos);
					ppwszAreaList[i] = ALLOCA_WSTR((LPCTSTR)str);
				}
			}
		}

		if (SUCCEEDED(res))
		{
			pAeCSubscription->beforeInterfaceCall();
			res = pMgt->SetFilter(
					  pAeCSubscription->m_dwEventTypes,
					  dwNumEventCategories, pdwEventCategories,
					  pAeCSubscription->m_dwLowSeverity,
					  pAeCSubscription->m_dwHighSeverity,
					  dwNumAreas,  ppwszAreaList,
					  dwNumSources,  ppwszSourceList);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::SetFilter [0x%X] "), res));

			if (S_FALSE == res)
			{
				// One or more of the specified filter criteria
				// were ignored. Request filters from server
				res = doUpdateFilter(TRUE);
				pAeCSubscription->setLastError(res, SOAEC_ERR_GET_FILTER);
			}
		}
	} // from/to server

	return res;
}

HRESULT SOAeCComSubscriptionImpl::doUpdateReturnedAttributes(IN BOOL fromServer)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;
	HRESULT resRAC;
	_ASSERTION(!!m_submgt, "No interface");

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	CComPtr<IOPCEventSubscriptionMgt> pMgt;

	if (FAILED(res = m_submgt.getInterface(&pMgt)))
	{
		return res;
	}

	SOAeCReturnedAttrListList retAttrList(pAeCSubscription->m_retAttrList);
	SOCmnListPosition pos = retAttrList.getStartPosition();

	if (fromServer)
	{
		while (pos)
		{
			size_t category = 0;
			LPDWORD lpdwAttrIDs = NULL;
			DWORD dwCount = 0;
			// Request only categories already in the list
			// (the others will be empty anyway, unless set
			// by the user directly via OPC API)
			SOCmnDWORDList list(retAttrList.getNext(pos, &category));
			pAeCSubscription->beforeInterfaceCall();
			resRAC = pMgt->GetReturnedAttributes(
						 (DWORD)category, &dwCount, &lpdwAttrIDs);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::GetReturnedAttributes [0x%X] "), resRAC));

			if (SUCCEEDED(resRAC))
			{
				list.removeAll();

				for (DWORD i = 0 ; i < dwCount ; i++)
				{
					list.add(lpdwAttrIDs[i]);
				}

				list.setDirty(FALSE);
				::CoTaskMemFree(lpdwAttrIDs);
			}
			else
			{
				res = S_FALSE;
			}
		}
	}
	else // to server
	{
		while (pos)
		{
			DWORD _dwNull = 0;
			size_t category = 0;
			LPDWORD lpdwAttrIDs = &_dwNull;
			DWORD dwCount = 0;
			SOCmnDWORDList list(retAttrList.getNext(pos, &category));
			dwCount = list.getCount();

			if (dwCount)
			{
				if (0 == (lpdwAttrIDs = (LPDWORD)alloca(sizeof(DWORD) * dwCount)))
				{
					return E_OUTOFMEMORY;
				}

				SOCmnListPosition pos2 = list.GetHeadPosition();

				for (DWORD i = 0 ; pos2 ; i++)
				{
					lpdwAttrIDs[i] = list.getNext(pos2);
				}
			}

			pAeCSubscription->beforeInterfaceCall();
			resRAC = pMgt->SelectReturnedAttributes(
						 (DWORD)category, dwCount, lpdwAttrIDs);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::SetReturnedAttributes [0x%X] "), resRAC));

			if (FAILED(resRAC))
			{
				res = S_FALSE;
			}
		}
	} // from/to server

	return res;
}


HRESULT SOAeCComSubscriptionImpl::doUpdateSubscriptionState(
	IN BOOL fromServer,
	IN DWORD whatParameters)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	_ASSERTION(!!m_submgt, "No interface");

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	HRESULT res = S_OK;
	CComPtr<IOPCEventSubscriptionMgt> pMgt;

	if (FAILED(res = m_submgt.getInterface(&pMgt)))
	{
		return res;
	}

	BOOL  bActive = pAeCSubscription->m_bActive;
	DWORD dwBufferTime = pAeCSubscription->m_dwBufferTime;
	DWORD dwMaxSize = pAeCSubscription->m_dwMaxSize;
	OPCHANDLE hClientSubcription = (OPCHANDLE)pAeCSubscription->getOpcHandle();

	if (fromServer)
	{
		pAeCSubscription->beforeInterfaceCall();
		res = pMgt->GetState(
				  &bActive,
				  &dwBufferTime,
				  &dwMaxSize,
				  &hClientSubcription);
		pAeCSubscription->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::GetState [0x%X] "), res));

		if (SUCCEEDED(res))
		{
			if (whatParameters & SOAEC_SUBSPAR_ACTIVE)
			{
				pAeCSubscription->m_bActive = bActive;
			}

			if (whatParameters & SOAEC_SUBSPAR_BUFFERTIME)
			{
				pAeCSubscription->m_dwBufferTime = dwBufferTime;
			}

			if (whatParameters & SOAEC_SUBSPAR_MAXSIZE)
			{
				pAeCSubscription->m_dwMaxSize = dwMaxSize;
			}
		}
	}
	else // to serever
	{
		pAeCSubscription->beforeInterfaceCall();
		res = pMgt->SetState(
				  whatParameters & SOAEC_SUBSPAR_ACTIVE ? &bActive : NULL,
				  whatParameters & SOAEC_SUBSPAR_BUFFERTIME ? &dwBufferTime : NULL,
				  whatParameters & SOAEC_SUBSPAR_MAXSIZE ? &dwMaxSize : NULL,
				  hClientSubcription,
				  &pAeCSubscription->m_dwRevisedBufferTime,
				  &pAeCSubscription->m_dwRevisedMaxSize);
		pAeCSubscription->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::SetState [0x%X] "), res));
	} // from/to server

	return res;
}



//-----------------------------------------------------------------------------
// refresh
//

BOOL SOAeCComSubscriptionImpl::refresh(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	_ASSERTION(!!m_sink, "No sink");
	CComPtr<IOPCEventSubscriptionMgt> pMgt;

	if (FAILED(res = m_submgt.getInterface(&pMgt)))
	{
		pAeCSubscription->setLastError(res, SOAEC_ERR_REFRESH);
		return FALSE;
	}

	pAeCSubscription->beforeInterfaceCall();
	res = pMgt->Refresh(m_sink->m_dwCookie);
	pAeCSubscription->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::Refresh [0x%X] "), res));
	pAeCSubscription->setLastError(res, SOAEC_ERR_REFRESH);
	return SUCCEEDED(res);
}

BOOL SOAeCComSubscriptionImpl::cancelRefresh(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return E_ACCESSDENIED;
	}

	_ASSERTION(!!m_sink, "No sink");
	CComPtr<IOPCEventSubscriptionMgt> pMgt;

	if (FAILED(res = m_submgt.getInterface(&pMgt)))
	{
		pAeCSubscription->setLastError(res, SOAEC_ERR_CANCEL_REFRESH);
		return FALSE;
	}

	pAeCSubscription->beforeInterfaceCall();
	res = pMgt->CancelRefresh(m_sink->m_dwCookie);
	pAeCSubscription->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("IOPCEventSubscriptionMgt::CancelRefresh [0x%X] "), res));
	pAeCSubscription->setLastError(res, SOAEC_ERR_CANCEL_REFRESH);
	return SUCCEEDED(res);
}

//-----------------------------------------------------------------------------
// onEvent
//

void SOAeCComSubscriptionImpl::receivedEvents(
	IN OPCHANDLE /* hClientSubscription */,
	IN BOOL bRefresh,
	IN BOOL bLastRefresh,
	IN DWORD dwCount,
	IN ONEVENTSTRUCT* pEvents)
{
	if (SOCLT_COM_CHECK_DEMO_AE)
	{
		return;
	}

	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	SOCmnPointer<SOAeCEntry> entry = getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entry->getCreator();
	DWORD i;

	if (!creator)
	{
		return;
	}

	SOAeCEvent** pEventArray;
	pEventArray = (SOAeCEvent**)malloc(dwCount * sizeof(SOAeCEvent*));

	for (i = 0 ; i < dwCount ; i++)
	{
		pEventArray[i] = creator->createEvent(pAeCSubscription, (pEvents + i));

		if (pEventArray[i])
		{
			pEventArray[i]->report();
		}
	}

	pAeCSubscription->onReceivedEvents(dwCount, pEventArray, (BYTE)bRefresh, (BYTE)bLastRefresh);

	for (i = 0 ; i < dwCount ; i++)
	{
		pEventArray[i]->release();
	}

	free(pEventArray);
}

//-----------------------------------------------------------------------------
// SOAeCComSubscriptionDefault                                                |
//-----------------------------------------------------------------------------

SOAeCComSubscriptionDefault::SOAeCComSubscriptionDefault(void)
	: SOAeCSubscription()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeCComSubscriptionDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError

#endif // SOFEATURE_DCOM

#endif // SOAEC
