#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_DCOM

#include "SOCltComServer.h"
#include "SOCltShutdown.h"

//-----------------------------------------------------------------------------
// SOCltComServerImpl                                                             |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor

SOCltComServerImpl::SOCltComServerImpl(void)
{
	::ZeroMemory(&m_clsid, sizeof(m_clsid));
	m_clsctx = CLSCTX_SERVER;
	HRESULT res = ::CoGetMalloc(MEMCTX_TASK, &m_malloc);

	if (!SUCCEEDED(res))
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, _T("SOCltComServerImpl"), (_T("ctor CoGetMalloc() cailure [0x%X]"), res));
		_ASSERTION(TRUE, "::CoGetMalloc() failed.");
	}
}

void SOCltComServerImpl::initClt(void)
{
	SOCltServer* pCltServer = getObjectCltServer();
	SOCMN_ALLOCATE_OBJECT(m_shutdown, SOCltShutdown(pCltServer));
}

//-----------------------------------------------------------------------------
// Destructor

SOCltComServerImpl::~SOCltComServerImpl()
{
	terminate();
}

void SOCltComServerImpl::terminate(void)
{
	m_malloc = NULL;
	m_unknown = NULL;
	m_common = NULL;
	m_secPrivate = NULL;

	if (m_shutdown)
	{
		m_shutdown->setServer(NULL);
		m_shutdown->Release();
		m_shutdown = NULL;
	}
}

//-----------------------------------------------------------------------------
// queryAvailableLCIDs
//
// - get the available locale ids from the server
//
// returns:
//		S_OK          - operation succeeded
//		E_INVALIDARG  - invalid argument
//		E_FAIL        - operation failed
//		E_OUTOFMEMORY - not enough memory
//		E_NOINTERFACE - can't get server interface
//
HRESULT SOCltComServerImpl::queryAvailableLCIDs(
	OUT SOCmnDWORDList& list)
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");
	HRESULT res;

	if (!list.isInit())
	{
		list.create();
	}

	CComPtr<IOPCCommon> pIOPCCommon;

	if (FAILED(res = m_common.getInterface(&pIOPCCommon)))
	{
		return res;
	}

	DWORD cntLCIDs;
	SOCltAutoMemory<LCID> lcids;
	pCltServer->beforeInterfaceCall();
	res = pIOPCCommon->QueryAvailableLocaleIDs(&cntLCIDs, &lcids);
	pCltServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pCltServer, (_T("IOPCCommon::QueryAvailableLocaleIDs [0x%X]"), res));
	pCltServer->setLastError(res, SOCLT_ERR_SRV_QUERY_LCIDS);

	if (SUCCEEDED(res))
	{
		for (DWORD i = 0 ; i < cntLCIDs; i++)
		{
			list.add(lcids[i]);
		}
	}

	return res;
} // queryAvailableLCIDs


//-----------------------------------------------------------------------------
// getErrorString
//
// - get error description from server
//
// returns:
//		S_OK          - operation succeeded
//		E_INVALIDARG  - invalid argument
//		E_FAIL        - operation failed
//		E_OUTOFMEMORY - not enough memory
//		E_NOINTERFACE - can't get server interface
//
HRESULT SOCltComServerImpl::getErrorString(
	IN HRESULT error,       // error code
	OUT SOCmnString* string)    // error text
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");
	HRESULT res;

	// check output parameter
	if (string == NULL)
	{
		pCltServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_OUTPUT);
		return E_INVALIDARG;
	}

	CComPtr<IOPCCommon> pIOPCCommon;

	if (FAILED(res = m_common.getInterface(&pIOPCCommon)))
	{
		return res;
	}

	SOCltAutoMemory<WCHAR> str;
	pCltServer->beforeInterfaceCall();
	res = pIOPCCommon->GetErrorString(error, &str);
	pCltServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pCltServer, (_T("IOPCCommon::GetErrorString [0x%X]"), res));
	pCltServer->setLastError(res, SOCLT_ERR_GET_ERROR_STRING);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		*string = W2CT(str);
	}

	return res;
} // getErrorString



BOOL SOCltComServerImpl::logon(void)
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");

	if (!pCltServer->m_userName.isEmpty())
	{
		return logon(pCltServer->m_userName, pCltServer->m_password);
	}
	else
	{
		return FALSE;
	}
}

BOOL SOCltComServerImpl::logon(
	IN LPCTSTR userName,
	IN LPCTSTR password)
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");
	CComPtr<IOPCSecurityPrivate> pIOPCSecurityPrivate;
	HRESULT res;

	if (FAILED(res = m_secPrivate.getInterface(&pIOPCSecurityPrivate)))
	{
		return FALSE;
	}

	LPCWSTR wUserName;
	LPCWSTR wPassword;
	USES_CONVERSION;
	wUserName = T2CW(userName);
	wPassword = T2CW(password);
	pCltServer->beforeInterfaceCall();
	res = pIOPCSecurityPrivate->Logon(wUserName, wPassword);
	pCltServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pCltServer, (_T("IOPCSecurityPrivate::Logon [0x%X]"), res));
	pCltServer->setLastError(res, SOCLT_ERR_LOGON);
	return SUCCEEDED(res);
}

BOOL SOCltComServerImpl::logoff(void)
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");
	CComPtr<IOPCSecurityPrivate> pIOPCSecurityPrivate;
	HRESULT res;

	if (FAILED(res = m_secPrivate.getInterface(&pIOPCSecurityPrivate)))
	{
		return FALSE;
	}

	pCltServer->beforeInterfaceCall();
	res = pIOPCSecurityPrivate->Logoff();
	pCltServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pCltServer, (_T("IOPCSecurityPrivate::Logoff [0x%X]"), res));
	pCltServer->setLastError(res, SOCLT_ERR_LOGOFF);
	return SUCCEEDED(res);
}

BOOL SOCltComServerImpl::isPrivateSecurityAvailable(void)
{
	SOCltServer* pCltServer = getObjectCltServer();
	_ASSERTION(pCltServer, "No SOCltServer object");
	CComPtr<IOPCSecurityPrivate> pIOPCSecurityPrivate;
	HRESULT res;
	BOOL isAvailable = FALSE;

	if (FAILED(res = m_secPrivate.getInterface(&pIOPCSecurityPrivate)))
	{
		return FALSE;
	}

	pCltServer->beforeInterfaceCall();
	res = pIOPCSecurityPrivate->IsAvailablePriv(&isAvailable);
	pCltServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pCltServer, (_T("IOPCSecurityPrivate::IsAvailablePriv [0x%X]"), res));
	pCltServer->setLastError(res, SOCLT_ERR_ISAVAILABLEPRIV);

	if (FAILED(res))
	{
		isAvailable = FALSE;
	}

	return isAvailable;
}

#endif // SOFEATURE_DCOM

#endif // SOCLT
