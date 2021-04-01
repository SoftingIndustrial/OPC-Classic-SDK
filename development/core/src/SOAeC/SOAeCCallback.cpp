#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCServer.h"
#include "SOAeCCallback.h"
#include "SOAeCSubscription.h"

//-----------------------------------------------------------------------------
// SOAeCEventSink                                                             |
//-----------------------------------------------------------------------------

SOAeCEventSink::SOAeCEventSink(
	IN SOAeCSubscription* subs) // group object
{
	_ASSERT_PTR(subs);
	m_subs = subs;
	m_dwCookie = 0;
	m_refCount = 0;
	m_established = FALSE;
	AddRef();
}

SOAeCEventSink::~SOAeCEventSink()
{
	unadvise(FALSE);
}

//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOAeCEventSink::getObjTraceId(void)
{
	SOCltComGuard gurad(this);
	_ASSERT_PTR(m_subs);
	return m_subs->getObjTraceId();
} // getObjTraceId


//-----------------------------------------------------------------------------
// AddRef
//
// - add reference to count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SOAeCEventSink::AddRef(void)
{
	return  interlockedIncrement(&m_refCount);
} // AddRef


//-----------------------------------------------------------------------------
// Release
//
// - release one reference from count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SOAeCEventSink::Release(void)
{
	if (0 == ::InterlockedDecrement(&m_refCount))
	{
		delete this;
		return 0;
	}
	else
	{
		return m_refCount;
	}
}


//-----------------------------------------------------------------------------
// QueryInterface
//
// - query for an interface of the object
//

STDMETHODIMP SOAeCEventSink::QueryInterface(
	IN REFIID iid,          // interface id
	IN LPVOID* ppInterface) // interface pointer
{
	SOCltComGuard gurad(this);

	if (ppInterface == NULL)
	{
		return E_INVALIDARG;
	}

	if (iid == IID_IUnknown)
	{
		*ppInterface = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_IOPCEventSink)
	{
		*ppInterface = static_cast<IOPCEventSink*>(this);
	}
	else
	{
		*ppInterface = NULL;
	}

	if (*ppInterface == NULL)
	{
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
} // QueryInterface


//-----------------------------------------------------------------------------
// OnEvent
//
// - callback from the OPC Server
//

STDMETHODIMP SOAeCEventSink::OnEvent(IN OPCHANDLE hClientSubscription,
									 IN BOOL bRefresh,
									 IN BOOL bLastRefresh,
									 IN DWORD dwCount,
									 IN ONEVENTSTRUCT* pEvents)
{
	SOCltComGuard gurad(this);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IOPCEventSink::OnEvent, with %d events"), dwCount));
	
	SOCMN_TRY
	{
		if (SOCLT_COM_CHECK_DEMO_AE)
		{
			return S_OK;
		}

		if (!m_subs)
		{
			return S_OK;
		}
		m_subs->receivedEvents(hClientSubscription, bRefresh, bLastRefresh, dwCount, pEvents);
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IOPCEventSink::OnEvent")));
		return S_OK;
	}
}

//-----------------------------------------------------------------------------
// advise
//
// - connect the advise sink of the client to the server
//

HRESULT SOAeCEventSink::advise(void)
{
	HRESULT res = S_OK;

	// already advised
	if (isAdvised())
	{
		return S_OK;
	}

	if (!m_subs)
	{
		return E_FAIL;
	}

	CComPtr<IConnectionPointContainer> cpc;
	CComPtr<IConnectionPoint> cp;
	res = m_subs->queryInterface(IID_IConnectionPointContainer, (void**)&cpc);

	if (SUCCEEDED(res))
	{
		m_subs->beforeInterfaceCall();
		res = cpc->FindConnectionPoint(IID_IOPCEventSink, &cp);
		m_subs->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_subs->beforeInterfaceCall();
		res = cp->Advise(static_cast<IUnknown*>(this), &m_dwCookie);
		m_subs->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPoint::Advise [0x%X] "), res));

		if (SUCCEEDED(res))
		{
			m_established = TRUE;
		}
	}

	return res;
} // advise


//-----------------------------------------------------------------------------
// unadvise
//
// - disconnect the advise sink of the client to the server

HRESULT SOAeCEventSink::unadvise(BOOL connectionMonitorStatus)
{
	HRESULT res = S_OK;

	if (!isAdvised())
	{
		return S_OK;
	}

	if (!m_subs)
	{
		return E_FAIL;
	}

	CComPtr<IConnectionPointContainer> pIContainer;
	CComPtr<IConnectionPoint> pIPoint;
	res = m_subs->queryInterface(IID_IConnectionPointContainer, (void**)&pIContainer);

	if ((SUCCEEDED(res)) && (connectionMonitorStatus))
	{
		res = 0x800706ba; // The RPC server is unavailable.
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint skipped, ConnectionMonitor is marked disconnected: [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_subs->beforeInterfaceCall();
		res = pIContainer->FindConnectionPoint(IID_IOPCEventSink, &pIPoint);
		m_subs->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_subs->beforeInterfaceCall();
		res = pIPoint->Unadvise(m_dwCookie);
		m_subs->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPoint::Unadvise [0x%X] "), res));
	}

	if (FAILED(res))
	{
		m_subs->beforeInterfaceCall();
		res = ::CoDisconnectObject(static_cast<IUnknown*>(this), 0);
		m_subs->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoDisconnectObject [0x%X] "), res));
	}

	m_dwCookie = 0;
	m_established = FALSE;
	return res;
} // unadvise

#endif // SOAEC



