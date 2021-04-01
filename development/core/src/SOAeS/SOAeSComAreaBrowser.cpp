#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_DCOM

#include "SOAeSComAreaBrowser.h"
#include "SOAeSComServer.h"
#include "SOSrvComEnum.h"
#include <opc_ae.h>


//-----------------------------------------------------------------------------
// SOAeSComAreaBrowser                                                        |
//-----------------------------------------------------------------------------

SOAeSComAreaBrowser::SOAeSComAreaBrowser()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	_TRACE_CREATE_COMOBJ();
	m_server = NULL;
	m_me = NULL;
}

SOAeSComAreaBrowser::~SOAeSComAreaBrowser()
{
	_TRACE_DELETE_COMOBJ();

	if (m_server)
	{
		// release server object
		m_server->m_me->removeAdditionalElement(m_me);
		releaseComServer();
	}

	if (m_me)
	{
		m_me->release();    // release tree object
	}

	lastCOMObjectActions();
}


//-----------------------------------------------------------------------------
// init
//
// - initialize the subscription COM object
//
void SOAeSComAreaBrowser::init(
	IN SOAeSComServer* server,          // server COM object
	IN SOAeSAreaBrowser* areaBrowser)   // area browser object
{
	m_server = server;
	m_me = areaBrowser;
	m_me->m_me = this;
	m_me->addRef();
	m_me->m_curBrowse = m_me->getBrowserRoot();
	// keep server alive till subscription is freed
	((SOSrvComObject<SOAeSComServer> *)m_server)->internalAddRef();
} // init


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SOAeSComAreaBrowser::lockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.lock();
	}
} // lockMe


//-----------------------------------------------------------------------------
// unlockMe
//
// - unlock release and access from non COM object
//
void SOAeSComAreaBrowser::unlockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.unlock();
	}
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite the SOAeSComAreaBrowser object from the SOAeSAreaBrowser object
//
void SOAeSComAreaBrowser::disuniteMe(void)
{
	if (m_me)
	{
		m_me->m_me = NULL;
	}
} // disuniteMe


//-----------------------------------------------------------------------------
// releaseComServer
//
// - release SOAeSComServer object
//
void SOAeSComAreaBrowser::releaseComServer(void)
{
	if (m_server)
	{
		((SOSrvComObject<SOAeSComServer> *)m_server)->internalRelease();
		m_server = NULL;
	}
} // releaseComServer


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOAeSComAreaBrowser::getObjTraceId(void)
{
	if (m_objectTraceId.IsEmpty())
	{
		m_objectTraceId.Format(_T("(arb|%8.8X)"), this->getOpcHandle());
	}

	return m_objectTraceId;
} // getObjTraceId



//-----------------------------------------------------------------------------
// IOPCEventAreaBrowser                                                       |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ChangeBrowsePosition (interface IOPCEventAreaBrowser)
//
// - change the current browse position
//
// returns:
//		S_OK                        - position changed
//		E_INVALIDARG                - invalid argument or called up to root node
//		OPC_E_INVALIDBRANCHNAME     - bad branch name on down or to
//
STDMETHODIMP SOAeSComAreaBrowser::ChangeBrowsePosition(
	IN OPCAEBROWSEDIRECTION browseDirection,    // browse direction
	IN LPCWSTR string)                          // down or to string
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventAreaBrowser::ChangeBrowsePosition, ChangeBrowsePosition(browseDirection, string))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(!((browseDirection == OPCAE_BROWSE_DOWN) && ((string == NULL) || (wcslen(string) == 0))))
	SOSRV_IN_PARAM(!((browseDirection != OPCAE_BROWSE_UP) && (browseDirection != OPCAE_BROWSE_TO) && (browseDirection != OPCAE_BROWSE_DOWN)))
	SOSRV_END_PARAMS_CHECK(ChangeBrowsePosition(res, browseDirection, string))
	USES_CONVERSION;
	// call implementation in browser object
	res = m_me->changeBrowsePosition((SOSrvBrowser::browserDirection)browseDirection, OLE2CT(string));
	SOSRV_END_INTERFACE_FUNCTION(ChangeBrowsePosition(res, browseDirection, string))
} // ChangeBrowsePosition


//-----------------------------------------------------------------------------
// BrowseOPCAreas (interface IOPCEventAreaBrowser)
//
// - browses the current position for areas or sources
//
// returns:
//		S_OK          - filled list
//		S_FALSE       - nothing to add to list
//		E_INVALIDARG  - invalid argument
//
STDMETHODIMP SOAeSComAreaBrowser::BrowseOPCAreas(
	IN OPCAEBROWSETYPE browseFilterType,    // get what? areas or sources
	IN LPCWSTR filterCriteria,              // string filter
	OUT LPENUMSTRING* enumString)           // child names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventAreaBrowser::BrowseOPCAreas, BrowseOPCAreas(browseFilterType, filterCriteria, enumString))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(enumString)
	SOSRV_IN_PARAM(!((browseFilterType != OPC_AREA) && (browseFilterType != OPC_SOURCE)))
	SOSRV_END_PARAMS_CHECK(BrowseOPCAreas(res, browseFilterType, filterCriteria, enumString))
	USES_CONVERSION;
	// call implementation in browser object
	SOSrvComEnumStringObject* itemNameList;
	SOSrvBrowser::browserFilterType fType;

	switch (browseFilterType)
	{
	case OPC_SOURCE:
		fType = SOSrvBrowser::typeLeaf;
		break;

	case OPC_AREA:
		fType = SOSrvBrowser::typeBranch;
		break;

	default:
		return E_INVALIDARG;
	}

	SOCMN_ALLOCATE_OBJECT(itemNameList, SOSrvComEnumStringObject)
	res = m_me->browseCurrentBranch(fType, OLE2CT(filterCriteria),
									VT_EMPTY, 0, itemNameList->m_list);

	if (SUCCEEDED(res))
	{
		// no list entry -> return OK but nothing found
		if (itemNameList->m_list.isEmpty())
		{
			res = S_FALSE;
		}

		itemNameList->Reset();
		itemNameList->QueryInterface(IID_IEnumString, (void**)enumString);
	}
	else
	{
		delete itemNameList;    // free list enumerator
		*enumString = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(BrowseOPCAreas(res, browseFilterType, filterCriteria, enumString), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // BrowseOPCAreas


//-----------------------------------------------------------------------------
// GetQualifiedAreaName (interface IOPCEventAreaBrowser)
//
// - get the fully qualified name of a area
//
// returns:
//		S_OK          - got name
//		E_OUTOFMEMORY - out of memory
//		E_INVALIDARG  - invalid argument
//
STDMETHODIMP SOAeSComAreaBrowser::GetQualifiedAreaName(
	IN LPCWSTR areaName,            // area name
	OUT LPWSTR* qualifiedAreaName)  // fully qualified area name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventAreaBrowser::GetQualifiedAreaName, GetQualifiedAreaName(areaName, qualifiedAreaName))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(qualifiedAreaName)
	SOSRV_IN_PARAM_PTR(areaName)
	SOSRV_END_PARAMS_CHECK(GetQualifiedAreaName(res, areaName, qualifiedAreaName))
	USES_CONVERSION;
	SOCmnString name(OLE2CT(areaName));
	SOCmnString path;

	// get the path
	if (m_me->getQualifiedBranchName(name, path))
	{
		// copy path
		*qualifiedAreaName = (LPWSTR) ::CoTaskMemAlloc((path.getLength() + 1) * sizeof(WCHAR));

		if (*qualifiedAreaName != NULL)
		{
			::wcscpy(*qualifiedAreaName, T2COLE((LPCTSTR)path));
			res =  S_OK;
		}
		else
		{
			res = E_OUTOFMEMORY;
		}
	}
	else
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetQualifiedAreaName(res, areaName, qualifiedAreaName))
} // SOAeSComAreaBrowser


//-----------------------------------------------------------------------------
// GetQualifiedSourceName (interface IOPCEventAreaBrowser)
//
// - get the fully qualified name of a sorce
//
// returns:
//		S_OK          - got name
//		E_OUTOFMEMORY - out of memory
//		E_INVALIDARG  - invalid argument
//
STDMETHODIMP SOAeSComAreaBrowser::GetQualifiedSourceName(
	IN LPCWSTR sourceName,              // source name
	OUT LPWSTR* qualifiedSourceName)    // fully qualified source name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventAreaBrowser::GetQualifiedAreaName, GetQualifiedSourceName(sourceName, qualifiedSourceName))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(qualifiedSourceName)
	SOSRV_IN_PARAM_PTR(sourceName)
	SOSRV_END_PARAMS_CHECK(GetQualifiedSourceName(res, sourceName, qualifiedSourceName))
	USES_CONVERSION;
	SOCmnString name(OLE2CT(sourceName));
	SOCmnString path;

	// get the path and check if valid element
	if (m_me->getQualifiedLeafName(name, path))
	{
		// item id is valid
		// copy path
		*qualifiedSourceName = (LPWSTR) ::CoTaskMemAlloc((path.getLength() + 1) * sizeof(WCHAR));

		if (*qualifiedSourceName != NULL)
		{
			::wcscpy(*qualifiedSourceName, T2COLE((LPCTSTR)path));
			res = S_OK;
		}
		else
		{
			res = E_OUTOFMEMORY;
		}
	}
	else
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetQualifiedSourceName(res, sourceName, qualifiedSourceName))
} // GetQualifiedSourceName


//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface overwritable method
//
// returns:
//		E_NOTIMPL     - do standard ATL QueryInterface
//		S_OK          - returned interface
//		E_NOINTERFACE - interface not supported
//
HRESULT SOAeSComAreaBrowser::queryInterface(
	IN REFIID iid,      // interface id
	IN void** object)   // interface
{
	return m_me->queryInterface(iid, object);
} // queryInterface

#endif // SOFEATURE_DCOM

#endif // SOAES
