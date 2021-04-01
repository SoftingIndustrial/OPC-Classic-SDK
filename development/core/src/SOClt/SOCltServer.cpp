#include "stdafx.h"

#ifdef SOCLT

#include "SOCltElement.h"
#include "SOCltThread.h"
#include "SOCltServer.h"
#include "SOCltShutdown.h"
#include "SOCltStorage.h"

//-----------------------------------------------------------------------------
// SOCltServer                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor

SOCltServer::SOCltServer(IN OPTIONAL DWORD objType)
	:  SOCltElement(objType)
{
	SOCmnString name;
#ifdef SOOS_WINDOWS
	m_lcid = ::GetSystemDefaultLCID();
#else
	m_lcid = MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT);
#endif
	m_validParameter = 0;   // valid parameters flag
	m_requestTimeout = SOCMN_TIME_40DAYS;
	setObjectState(disconnected);
	setTargetObjectState(disconnected);
	m_cookieConnectMonitor = 0;
	m_protocol = SOCLT_PROTOCOL_DCOM;
	m_performInitialRead = TRUE;
}

//-----------------------------------------------------------------------------
// Destructor

SOCltServer::~SOCltServer()
{
}

//-----------------------------------------------------------------------------
// getWorkerThread

SOCltWorkerThread* SOCltServer::getWorkerThread(void)
{
	return &m_workThread;
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOCltServer::getObjectCmnElementIList(void)
{
	return static_cast<SOCmnObject*>(this);
} // getObjectCmnElementIList

//-----------------------------------------------------------------------------
// shutdown
//
// - shutdown all connections to the server
//
void SOCltServer::shutdown(
	IN LPCTSTR /* reason */)    // shutdown reason
{
	disconnect();
	INVOKE_CALLBACK(SOCltServer, onShutdown)
} // shutdown


//-----------------------------------------------------------------------------
// onShutdown
//
// - called if server requested shutdown
//
void SOCltServer::onShutdown(void)
{} // onShutdown


//-----------------------------------------------------------------------------
// setLCID
//
// - set local Id
//
// returns:
//		TRUE
//
BOOL SOCltServer::setLCID(
	IN DWORD lcid)  // local Id
{
	m_validParameter |= SOCLT_SRVPAR_LCID;
	m_lcid = lcid;
	INVOKE_CALLBACK(SOCltServer, onSetLCID);
	return TRUE;
} // setLCID


//-----------------------------------------------------------------------------
// onSetLCID
//
// - called before local id is set
// - default implementation: do nothing
//
void SOCltServer::onSetLCID()
{} // onSetLCID

//-----------------------------------------------------------------------------
// setRequestTimeout
//
// - set request timeout
//
// returns:
//		TRUE
//
BOOL SOCltServer::setRequestTimeout(
	IN DWORD requestTimeout)    // request timeout
{
	m_requestTimeout = requestTimeout;
	INVOKE_CALLBACK(SOCltServer, onSetRequestTimeout);
	return TRUE;
} // setRequestTimeout

//-----------------------------------------------------------------------------
// onSetRequestTimeout
//
// - called before requestTimeout is set
// - default implementation: do nothing
//
void SOCltServer::onSetRequestTimeout()
{} // onSetRequestTimeout

//-----------------------------------------------------------------------------
// setClientName
//
// - set client name
//
// returns:
//		TRUE
//
BOOL SOCltServer::setClientName(
	IN LPCTSTR clientName)  // client name
{
	m_validParameter |= SOCLT_SRVPAR_CLIENT_NAME;
	m_clientName = clientName;
	INVOKE_CALLBACK(SOCltServer, onSetClientName);
	return TRUE;
} // setClientName


//-----------------------------------------------------------------------------
// onSetClientName
//
// - called before client name is set
// - default implementation: do nothing
//
void SOCltServer::onSetClientName(void)
{} // onSetClientName

//-----------------------------------------------------------------------------
// setPerformInitialRead
//
// - set whether to perform initial read or not
//
// returns:
//		TRUE
//
BOOL SOCltServer::setPerformInitialRead(
		IN BYTE performInitialRead)  // client name
{
	m_performInitialRead = performInitialRead;
	INVOKE_CALLBACK(SOCltServer, onSetPerformInitialRead);
	return TRUE;
} // setPerformInitialRead


//-----------------------------------------------------------------------------
// onSetPerformInitialRead
//
// - called before perform initial read is set
// - default implementation: do nothing
//
void SOCltServer::onSetPerformInitialRead(void)
{} // onSetPerformInitialRead


//-----------------------------------------------------------------------------
// setServerName
//
// - set server name
//
// returns:
//		TRUE
//
BOOL SOCltServer::setServerName(
	IN LPCTSTR serverName)  // server name
{
	m_serverName = serverName;
	INVOKE_CALLBACK(SOCltServer, onSetServerName);
	return TRUE;
} // setClientName


//-----------------------------------------------------------------------------
// onSetServerName
//
// - called before server name is set
// - default implementation: do nothing
//
void SOCltServer::onSetServerName(void)
{} // onSetServerName



//-----------------------------------------------------------------------------
// setParentBranch
//
BOOL SOCltServer::setParentBranch(IN SOCmnElement* parent)
{
	if (!SOCltElement::setParentBranch(parent))
	{
		return FALSE;
	}

	// Install a Connection Monitor task.
	// Attention: installing the task creates
	// a reference cycle. The best place to break it
	// is when parent branch is set to NULL.

	if (parent)
	{
		if (0 == m_cookieConnectMonitor)
		{
			addConnectionMonitor(5000, 10, 10000, 300000);
		}
	}
	else
	{
		removeConnectionMonitor();
		terminate();
		m_workThread.shutdown();
	}

	return TRUE;
} // setParentBranch


BOOL SOCltServer::addConnectionMonitor(DWORD checkCycle, DWORD connect1Attempts, DWORD connect1Cycle, DWORD connect2Cycle)
{
	removeConnectionMonitor();
	SOCmnPointer<SOCltTaskConnectionMonitor> task;
	SOCmnPointer<SOCltEntry> entry = ::getSOCltEntry();
	SOCmnPointer<SOCltCreator> creator = entry->getCltCreator();
	task = creator->createTaskConnectionMonitor(this, checkCycle);
	task->m_attempts = connect1Attempts;
	task->m_cycle1 = connect1Cycle;
	task->m_cycle2 = connect2Cycle;
	return m_workThread.addCyclicTask((SOCltTaskConnectionMonitor*)task, &m_cookieConnectMonitor);
}

BOOL SOCltServer::removeConnectionMonitor(void)
{
	BOOL ret = FALSE;

	if (0 != m_cookieConnectMonitor)
	{
		ret = m_workThread.removeCyclicTask(m_cookieConnectMonitor);
		m_cookieConnectMonitor = 0;
	}

	return ret;
}

BOOL SOCltServer::setUserName(
	IN LPCTSTR userName)
{
	m_userName = userName;
	return TRUE;
}

BOOL SOCltServer::setPassword(
	IN LPCTSTR password)
{
	m_password = password;
	return TRUE;
}

//-----------------------------------------------------------------------------
// getObjectCltServer
//
// - get object (type SOCltServer) using the class
//
// returns:
//		object
//
SOCltServer* SOCltServer::getObjectCltServer(void)
{
	return this;
} // getObjectCltServer


#ifdef SOFEATURE_SOAP

BOOL SOCltServer::convertLCIDtoLocaleID(LCID lcid, SOCmnString& localeId)
{
	BOOL ret = TRUE;
#ifdef SOOS_WINDOWS

	// handle windows default lcids
	if (lcid == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
	{
		lcid = GetSystemDefaultLCID();
	}

	if (lcid == MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
	{
		lcid = GetUserDefaultLCID();
	}

#endif

	switch (lcid)
	{
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
		localeId = _T("en-US");
		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		localeId = _T("de-DE");
		break;

	case MAKELCID(MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), SORT_DEFAULT):
		localeId = _T("sv-SE");
		break;

	default:
	{
		WORD lang = LANGIDFROMLCID(lcid);

		switch (PRIMARYLANGID(lang))
		{
		case LANG_ENGLISH:
			localeId = _T("en");
			break;

		case LANG_GERMAN:
			localeId = _T("de");
			break;

		case LANG_SWEDISH:
			localeId = _T("sv");
			break;

		default:
			ret = FALSE;
			localeId = _T("en-US"); // default is enlish US
			break;
		}
	}
	break;
	}

	return ret;
}

BOOL SOCltServer::convertLocaleIDtoLCID(LPCTSTR localeId, LCID& lcid)
{
	BOOL ret = TRUE;

	if (_tcsncmp(localeId, _T("en"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	}
	else if (_tcsncmp(localeId, _T("de"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
	}
	else if (_tcsncmp(localeId, _T("sv"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), SORT_DEFAULT);
	}
	else
	{
		ret = FALSE;
		lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	}

	return ret;
}

#endif // SOFEATURE_SOAP


SOCltServerICommunication::SOCltServerICommunication(void)
{}
SOCltServerICommunication::~SOCltServerICommunication(void)
{}
void SOCltServerICommunication::initClt(void)
{}
void SOCltServerICommunication::terminate(void)
{}
SOCltServer* SOCltServerICommunication::getObjectCltServer(void)
{
	return NULL;
}

#ifdef SOFEATURE_DCOM
HRESULT SOCltServerICommunication::getErrorString(IN HRESULT /* error */, OUT SOCmnString* /* string */)
{
	return E_FAIL;
}
BOOL SOCltServerICommunication::setClassID(IN REFCLSID /* clsid */)
{
	return FALSE;
}
static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
CLSID SOCltServerICommunication::getClassID(void) const
{
	return CLSID_NO;
}
BOOL SOCltServerICommunication::setClsctx(IN DWORD /* clsctx */)
{
	return FALSE;
}
DWORD SOCltServerICommunication::getClsctx(void) const
{
	return 0;
}
#endif

#endif // SOCLT
