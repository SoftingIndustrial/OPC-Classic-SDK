#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_DCOM

#include "SOCltElement.h"
#include "SOCltShutdown.h"
#include "SOCltServer.h"


//-----------------------------------------------------------------------------
// SOCltShutdown                                                              |
//-----------------------------------------------------------------------------

SOCltShutdown::SOCltShutdown(
	IN SOCltServer* server) // server object
{
	m_server.setWithAddRef(server);
	m_refCount = 0;
	m_cookie = 0;
	m_established = FALSE;
	AddRef();
}

SOCltShutdown::~SOCltShutdown()
{
}


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOCltShutdown::getObjTraceId(void)
{
	SOCltComGuard gurad(this);
	SOCmnPointer<SOCltServer> server(m_server);

	if (server.isNotNull())
	{
		return server->getObjTraceId();
	}
	else
	{
		return _T("");
	}
} // getObjTraceId


//-----------------------------------------------------------------------------
// AddRef
//
// - add reference to count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SOCltShutdown::AddRef(void)
{
	return interlockedIncrement(&m_refCount);
} // AddRef


//-----------------------------------------------------------------------------
// Release
//
// - release one reference from count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SOCltShutdown::Release(void)
{
	ULONG currentCount = ::InterlockedDecrement(&m_refCount);

	if (currentCount == 0)
	{
		delete this;
	}

	return currentCount;
} // Release


//-----------------------------------------------------------------------------
// QueryInterface
//
// - query for an interface of the object
//
// returns:
//		S_OK          - interface pointer returned
//		E_NOINTERFACE - interface not supported
//
STDMETHODIMP SOCltShutdown::QueryInterface(
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
		*ppInterface = (IUnknown*) this;
	}
	else if (iid == _ATL_IIDOF(IOPCShutdown))
	{
		*ppInterface = (IUnknown*) this;
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
// ShutdownRequest
//
// - called if server requests shutdown of all client connections
//
// returns:
//		S_OK
//
STDMETHODIMP SOCltShutdown::ShutdownRequest(
	IN LPCWSTR reason)  // shutdown reason
{
	SOCltComGuard gurad(this);
	USES_CONVERSION;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("Recieved shutdown request : %s"), W2CT(reason)));
	onShutdown(W2CT(reason));
	return S_OK;
} // ShutdownRequest


//-----------------------------------------------------------------------------
// advise
//
// - advise the shutdown event sink
//
// returns:
//		S_OK                   - connection installed
//		E_NOINTERFACE          - IConnectionPointContainer interface not supported by server -> OPC V1.0A
//		CONNECT_E_NOCONNECTION - IOPCShutdown is not supported by connection point container
//		CONNECT_E_ADVISELIMIT  - the connection point has already reached its limit of connections and cannot accept any more
//		E_FAIL
//
HRESULT SOCltShutdown::advise(void)
{
	CComPtr<IConnectionPointContainer> cpc;
	CComPtr<IConnectionPoint> cp;
	HRESULT res;

	SOCmnPointer<SOCltServer> server(m_server);

	if (!server)
	{
		return E_FAIL;
	}

	res = server->queryInterface(IID_IConnectionPointContainer, (void**)&cpc);

	if (SUCCEEDED(res))
	{
		server->beforeInterfaceCall();
		res = cpc->FindConnectionPoint(_ATL_IIDOF(IOPCShutdown), &cp);
		server->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		server->beforeInterfaceCall();
		res = cp->Advise((IUnknown*)this, &m_cookie);
		server->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPoint::Advise [0x%X] "), res));

		if (SUCCEEDED(res))
		{
			m_established = TRUE;
		}
	}

	server->setLastError(res, SOCLT_ERR_ADVISE_SHUTDOWN);
	return res;
} // advise


//-----------------------------------------------------------------------------
// unadvise
//
// - unadvise the shutdown event sink
//
// returns:
//		S_OK                   - connection unadvised or no connection was advised
//		E_NOINTERFACE          - IConnectionPointContainer interface not supported by server -> OPC V1.0A
//		CONNECT_E_NOCONNECTION - IOPCShutdown is not supported by connection point container
//		CONNECT_E_ADVISELIMIT  - the connection point has already reached its limit of connections and cannot accept any more
//		E_FAIL
//
HRESULT SOCltShutdown::unadvise(BOOL connectionMonitorStatus)
{
	CComPtr<IConnectionPointContainer> cpc;
	CComPtr<IConnectionPoint> cp;
	HRESULT res;

	if (!m_established)
	{
		return S_OK;    // no connection was advised
	}

	SOCmnPointer<SOCltServer> server(m_server);

	if (!server)
	{
		return E_FAIL;
	}

	// get connection point container of the OPC server object
	res = server->queryInterface(IID_IConnectionPointContainer, (void**)&cpc);

	if ((SUCCEEDED(res)) && (connectionMonitorStatus))
	{
		res = 0x800706ba; // The RPC server is unavailable.
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint skipped, ConnectionMonitor is marked disconnected: [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		server->beforeInterfaceCall();
		res = cpc->FindConnectionPoint(_ATL_IIDOF(IOPCShutdown), &cp);
		server->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		server->beforeInterfaceCall();
		res = cp->Unadvise(m_cookie);
		server->afterInterfaceCall();
	}

	if (FAILED(res))
	{
		res = ::CoDisconnectObject(static_cast<IUnknown*>(this), 0);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoDisconnectObject() [0x%X] "), res));
	}

	m_cookie = 0;
	m_established = FALSE;
	server->setLastError(res, SOCLT_ERR_UNADVISE_SHUTDOWN);
	return res;
} // unadvise


//-----------------------------------------------------------------------------
// setServer
//
// - called by SOCltComServerImpl on terminate
//
void SOCltShutdown::setServer(SOCltServer* pServer)
{
	m_server.setWithAddRef(pServer);
}


//-----------------------------------------------------------------------------
// onShutdown
//
// - called if server requests shutdown of all client connections
//
void SOCltShutdown::onShutdown(
	IN LPCTSTR reason)  // shutdown reason
{
	SOCmnPointer<SOCltServer> server(m_server);

	if (server.isNotNull())
	{
		server->shutdown(reason);
	}
}


//-----------------------------------------------------------------------------
// SOCltComGuard                                                              |
//-----------------------------------------------------------------------------

SOCltComGuard::SOCltComGuard(IUnknown* p)
{
	if (p!= NULL)
	{
		m_iUnknownInterface = p;
		m_iUnknownInterface->AddRef();
	}
}

SOCltComGuard::~SOCltComGuard()
{
	if (m_iUnknownInterface != NULL)
	{
		m_iUnknownInterface->Release();
	}
}

#endif // SOFEATURE_DCOM

#endif // SOCLT
