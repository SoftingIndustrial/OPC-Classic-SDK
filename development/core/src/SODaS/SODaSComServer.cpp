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
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSComServer.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : COM OPC Server object                                       |
//                - SODaSComServer: OPC DA server COM object                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_DCOM

#include "SODaSEntry.h"
#include "SODaSComServer.h"
#include "SODaSComGroup.h"
#include "SODaSServer.h"
#include "SODaSGroup.h"
#include "SODaSNode.h"
#include "SOCmnTrace.h"
#include "SOSrvComEnum.h"
#include "SODaSComEnum.h"
#include <opcae_er.h>
#include <opcerror.h>


//-----------------------------------------------------------------------------
// SODaSComServer                                                             |
//-----------------------------------------------------------------------------

SODaSComServer::SODaSComServer()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_serverInstance = 0;
	m_me = NULL;
	_TRACE_CREATE_COMOBJ();
}

SODaSComServer::~SODaSComServer()
{
	_TRACE_DELETE_COMOBJ();
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> root = engine->getObjectRoot();

	// remove tree object from tree
	if (root.isNotNull())
	{
		root->removeBranch(m_me);
	}

	// release tree object
	m_me->release();
	lastCOMObjectActions();
}


//-----------------------------------------------------------------------------
// afterConstructorInit
//
// - all initialisation which may needs virtual methods
//
HRESULT SODaSComServer::afterConstructorInit(void)
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSObjectRoot> root;
	SOCmnPointer<SODaSCreator> creator;
	// create OPC tree object
	creator = engine->getCreator();
	m_me = creator->createServer(FALSE, m_serverInstance);
	HRESULT result = m_me->setFlags(SODASSERVER_FLAG_DCOM);
	m_me->m_me = this;

	if (result != S_OK)
	{
		m_me->disconnectClient();
		return result;
	}

	// add to tree
	root = engine->getOPCRoot();
	root->addBranch(m_me);
	return S_OK;
} // afterConstructorInit


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SODaSComServer::lockMe(void)
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
void SODaSComServer::unlockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.unlock();
	}
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite the SODaSComServer object from the SODaSServer object
//
void SODaSComServer::disuniteMe(void)
{
	if (m_me)
	{
		m_me->m_me = NULL;
	}
} // disuniteMe


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaSComServer::getObjTraceId(void)
{
	if (m_objectTraceId.IsEmpty())
	{
		if (m_me)
		{
			m_objectTraceId.Format(_T("(srv|%8.8X| %s)"), this->getOpcHandle(), (LPCTSTR)m_me->getClientName());
		}
		else
		{
			m_objectTraceId.Format(_T("(srv|%8.8X)"), this->getOpcHandle());
		}
	}

	return m_objectTraceId;
} // getObjTraceId


//-----------------------------------------------------------------------------
// setObjTraceId
//
// - set objects trace string id
//
// returns:
//		TRUE
//
BOOL SODaSComServer::setObjTraceId(
	IN LPCTSTR traceId)
{
	if (traceId)
	{
		m_objectTraceId = traceId;
	}
	else
	{
		m_objectTraceId.empty();
	}

	return TRUE;
} // setObjTraceId


//-----------------------------------------------------------------------------
// createOPCGroup
//
// - creates a COM SODaSComGroup object
//
// returns:
//		result of QueryInterface
//
HRESULT SODaSComServer::createOPCGroup(
	IN SODaSGroup* grp,         // generic group object
	IN REFIID riid,             // id of requested interface
	OUT LPUNKNOWN* ppUnk,       // pointer to requested interface
	IN OPTIONAL BOOL doAddRef)  // AddRef the created object?
{
	HRESULT res;
	// create COM group object
	SOSrvComObject<SODaSComGroup> *group;
	SOCMN_ALLOCATE_OBJECT(group, SOSrvComObject<SODaSComGroup>);
	group->init(this, grp); // when group object is deleted it releases grp
	// query the requested interface
	res = group->QueryInterface(riid, (void**)ppUnk);

	if (SUCCEEDED(res))
	{
		if (doAddRef)
		{
			group->internalAddRef();    // add reference for creation,
			// will released via RemoveGroup
		}
	}
	else
	{
		// can't query the interface
		// delete both group objects
		m_me->removeGroup(grp->getServerHandle(), TRUE);
		SOCMN_FREE_OBJECT(group)
	}

	return res;
} // createOPCGroup


//-----------------------------------------------------------------------------
// IOPCCommon                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetLocaleID (interface IOPCCommon)
//
// - set default locale id of server
//
// returns:
//		S_OK          - set locale id
//		E_INVALIDARG  - invalid locale id for this server
//
STDMETHODIMP SODaSComServer::SetLocaleID(
	IN LCID dwLcid) // LCID
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCCommon::SetLocaleID, SetLocaleID(dwLcid))

	if (m_me->setLCID(dwLcid))
	{
		res = S_OK;
	}
	else
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(SetLocaleID(res, dwLcid))
} // SetLocaleID


//-----------------------------------------------------------------------------
// GetLocaleID (interface IOPCCommon)
//
// - get default locale id of server
//
// returns:
//		S_OK          - got locale id
//		E_INVALIDARG  - invalid output parameter
//
STDMETHODIMP SODaSComServer::GetLocaleID(
	OUT LCID* pdwLcid)  // LCID
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCCommon::GetLocaleID, GetLocaleID(pdwLcid))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwLcid)
	SOSRV_END_PARAMS_CHECK(GetLocaleID(res, pdwLcid))
	*pdwLcid = m_me->getLCID();
	SOSRV_END_INTERFACE_FUNCTION(GetLocaleID(res, pdwLcid))
} // GetLocaleID


//-----------------------------------------------------------------------------
// QueryAvailableLocaleIDs (interface IOPCCommon)
//
// - get all available locale ids
//
// returns:
//		S_OK          - got all ids
//		E_INVALIDARG  - invalid output parameter
//		E_OUTOFMEMORY - out of memory
//
STDMETHODIMP SODaSComServer::QueryAvailableLocaleIDs(
	OUT DWORD* pdwCount,    // number of available locale ids
	OUT LCID** pdwLcid)     // list of the available locale ids
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCCommon::QueryAvailableLocaleIDs, QueryAvailableLocaleIDs(pdwCount, pdwLcid))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCount)
	SOSRV_OUT_PARAM_PTR(pdwLcid)
	SOSRV_END_PARAMS_CHECK(QueryAvailableLocaleIDs(res, pdwCount, pdwLcid))
	SOCmnDWORDList lcidList;
	SOCmnListPosition pos;
	DWORD i;
	lcidList.create();
	m_me->queryAvailableLCIDs(lcidList);
	*pdwCount = lcidList.getCount();

	if (*pdwCount)
	{
		*pdwLcid = (LCID*)CoTaskMemAlloc((*pdwCount) * sizeof(LCID));

		if (*pdwLcid)
		{
			i = 0;
			pos = lcidList.getStartPosition();

			while (pos)
			{
				(*pdwLcid)[i] = (LCID)lcidList.getNext(pos);
				i++;
			}
		}
		else
		{
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QueryAvailableLocaleIDs(res, pdwCount, pdwLcid))
} // QueryAvailableLocaleIDs


//-----------------------------------------------------------------------------
// GetErrorString (interface IOPCCommon)
//
// - get error string for an error code
//
// returns:
//		S_OK          - got error string
//		E_INVALIDARG  - no string for the error code
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SODaSComServer::GetErrorString(
	IN HRESULT dwError,     // error code
	OUT LPWSTR* ppString)   // error text
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCCommon::GetErrorString, GetErrorString(dwError, ppString))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppString)
	SOSRV_END_PARAMS_CHECK(GetErrorString(res, dwError, ppString))
	res = getErrorString(dwError, FALSE, 0, ppString);
	SOSRV_END_INTERFACE_FUNCTION(GetErrorString(res, dwError, ppString))
} // GetErrorString


//-----------------------------------------------------------------------------
// getErrorString
//
// - get error string for an error code
//
// returns:
//		S_OK          - got error string
//		E_INVALIDARG  - no string for the error code
//		E_OUTOFMEMORY - can't allocate memory
//
HRESULT SODaSComServer::getErrorString(
	IN HRESULT dwError,     // error code
	IN BOOL useLocale,      // use locale id
	IN LCID dwLocale,       // locale for string
	OUT LPWSTR* ppString)   // error text
{
	USES_CONVERSION;
	HRESULT res;
	SOCmnString message;

	// get error string
	if (useLocale)
	{
		res = m_me->getErrorString(dwError, dwLocale, message);
	}
	else
	{
		res = m_me->getErrorString(dwError, message);
	}

	// copy error string
	if (SUCCEEDED(res))
	{
		*ppString = SOCmnString::cloneToWCHAR(message);

		if (!(*ppString))
		{
			res = E_OUTOFMEMORY;
		}
	}

	return res;
} // getErrorString


//-----------------------------------------------------------------------------
// SetClientName
//
// - set the name of the client
//
// returns:
//		S_OK
//
STDMETHODIMP SODaSComServer::SetClientName(
	IN LPCWSTR szName)  // client name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCCommon::SetClientName, SetClientName(szName))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM_PTR(szName)
	SOSRV_END_PARAMS_CHECK(SetClientName(res, szName))
	USES_CONVERSION;
	SOCmnString name(W2CT(szName));
	m_me->setClientName(name);
	setObjTraceId(NULL);
	m_me->setObjTraceId(NULL);
	SOSRV_END_INTERFACE_FUNCTION(SetClientName(res, szName))
} // SetClientName



//-----------------------------------------------------------------------------
// IOPCServer                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AddGroup (interface IOPCServer)
//
// - adds a group to the server
// - creates tree and COM group object
// - set initialization parameter
//
// returns:
//		S_OK                  - added group
//		OPC_S_UNSUPPORTEDRATE - changed update rate (but added group)
//		E_NOINTERFACE         - object don't support requested interface
//		E_INVALIDARG          - bad arguments (NULL for manadatory out value
//                              or bad name)
//		OPC_E_DUPLICATENAME   - group name not unique
//
STDMETHODIMP SODaSComServer::AddGroup(
	IN LPCWSTR szName,              // name of group
	IN BOOL bActive,                // activation state
	IN DWORD dwRequestedUpdateRate, // requested update rate
	IN OPCHANDLE hClientGroup,      // client handle
	IN LONG* pTimeBias,             // time zone
	IN FLOAT* pPercentDeadband,     // deadband
	IN DWORD dwLCID,                // LCID
	OUT OPCHANDLE* phServerGroup,   // server handle
	OUT DWORD* pRevisedUpdateRate,  // revised update rate
	IN REFIID riid,                 // id of requested interface
	OUT LPUNKNOWN* ppUnk)           // pointer to requested interface
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::AddGroup, AddGroup(szName, bActive, dwRequestedUpdateRate, hClientGroup, pTimeBias, pPercentDeadband,    dwLCID, phServerGroup, pRevisedUpdateRate, riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(phServerGroup)
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_IN_PARAM_IF_PTR(pPercentDeadband, ((*pPercentDeadband >= 0.0) && (*pPercentDeadband <= 100.0)))
	SOSRV_END_PARAMS_CHECK(AddGroup(res, szName, bActive, dwRequestedUpdateRate, hClientGroup, pTimeBias, pPercentDeadband, dwLCID, phServerGroup, pRevisedUpdateRate, riid, ppUnk))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szName));
	SOCmnPointer<SODaSGroup> grp;
	// create tree group object
	res = m_me->addGroup(name, (bActive != 0), dwRequestedUpdateRate, hClientGroup,
						 pTimeBias, pPercentDeadband, dwLCID, (SODaSGroup**)grp, pRevisedUpdateRate, FALSE);

	if (SUCCEEDED(res))
	{
		HRESULT resQI;

		// create COM group object
		if (SUCCEEDED(resQI = createOPCGroup(grp, riid, ppUnk)))
		{
			*phServerGroup = grp->getServerHandle();
#ifdef SOFEATURE_WATCH

			if (m_me->getCollectChanges())
			{
				SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
				entry->watchAddObject(m_me, grp);
			}

#endif
		}
		else
		{
			res = resQI;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(AddGroup(res, szName, bActive, dwRequestedUpdateRate, hClientGroup, pTimeBias, pPercentDeadband,   dwLCID, phServerGroup, pRevisedUpdateRate, riid, ppUnk))
} // AddGroup


//-----------------------------------------------------------------------------
// GetErrorString (interface IOPCServer)
//
// - get error string for an error code
//
// returns:
//		S_OK          - got error string
//		E_INVALIDARG  - no string for the error code
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SODaSComServer::GetErrorString(
	IN HRESULT dwError,     // error code
	IN LCID dwLocale,       // locale for string
	OUT LPWSTR* ppString)   // error string
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::GetErrorString, GetErrorString(dwError, dwLocale, ppString))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppString)
	SOSRV_END_PARAMS_CHECK(GetErrorString(res, dwError, dwLocale, ppString))
	res = getErrorString(dwError, TRUE, dwLocale, ppString);
	SOSRV_END_INTERFACE_FUNCTION(GetErrorString(res, dwError, dwLocale, ppString))
} // GetErrorString


//-----------------------------------------------------------------------------
// GetGroupByName (interface IOPCServer)
//
// - get group interface by the name of the group
//
// returns:
//		S_OK          - interface return
//		E_NOINTERFACE - object don't support requested interface
//		E_INVALIDARG  - no group with the name
//
STDMETHODIMP SODaSComServer::GetGroupByName(
	IN LPCWSTR szName,      // group name
	IN REFIID riid,         // interface id
	OUT LPUNKNOWN* ppUnk)   // interface pointer
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::GetGroupByName, GetGroupByName(szName, riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_END_PARAMS_CHECK(GetGroupByName(res, szName, riid, ppUnk))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szName));
	SOCmnPointer<SODaSGroup> grp;
	grp = m_me->getGroupByName(name);

	if (!!grp)
	{
		SOSrvComObject<SODaSComGroup> *group = (SOSrvComObject<SODaSComGroup> *)grp->getMe();

		if (group)
		{
			res = group->QueryInterface(riid, (void**)ppUnk);
			grp->releaseMe(group);
		}
	}
	else
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetGroupByName(res, szName, riid, ppUnk))
} // GetGroupByName


//-----------------------------------------------------------------------------
// GetStatus (interface IOPCServer)
//
// - gets the server status
//
// returns:
//		S_OK          - return status sturcture
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SODaSComServer::GetStatus(
	OUT OPCSERVERSTATUS** ppServerStatus)   // server status
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::GetStatus, GetStatus(ppServerStatus))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppServerStatus)
	SOSRV_END_PARAMS_CHECK(GetStatus(res, ppServerStatus))
	// allocate server sturcture
	*ppServerStatus = (OPCSERVERSTATUS*)CoTaskMemAlloc(sizeof(OPCSERVERSTATUS));

	if (*ppServerStatus != NULL)
	{
		// fill structure
		res = m_me->getStatus(*ppServerStatus);

		if (FAILED(res))
		{
			CoTaskMemFree(*ppServerStatus);
			*ppServerStatus = NULL;
		}
	}
	else
	{
		res = E_OUTOFMEMORY;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetStatus(res, ppServerStatus))
} // GetStatus


//-----------------------------------------------------------------------------
// RemoveGroup (interface IOPCServer)
//
// - removes a group from the server
//
// returns:
//		S_OK         - removed
//		OPC_S_INUSE  - group is in use by others
//		E_INVALIDARG - bad server handle
//
STDMETHODIMP SODaSComServer::RemoveGroup(
	IN OPCHANDLE hServerGroup,  // server handle
	IN BOOL bForce)             // force remove
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::RemoveGroup, RemoveGroup(hServerGroup, bForce))
	ULONG refCnt;
	// get group
	SOSrvComObject<SODaSComGroup> *group = NULL;
	SOCmnPointer<SODaSGroup> grp;
	grp = m_me->getGroupByHandle(hServerGroup);

	if (grp.isNotNull())
	{
		if (bForce)
		{
			group = (SOSrvComObject<SODaSComGroup> *)grp->getMe();
		}
		else
		{
			group = (SOSrvComObject<SODaSComGroup> *)grp->m_me;
		}
	}

	res = m_me->removeGroup(hServerGroup);

	if (SUCCEEDED(res))
	{
		// group found and removed from tree
		// -> hServerGroup is the pointer to the SODaSComGroup object
		group->internalRelease(&refCnt);

		if (refCnt)
		{
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("removed group %s in use"), group->getObjTraceId()));

			if (bForce)
			{
				// force removal
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("force removal of group %s; reference count %lu"), group->getObjTraceId(), refCnt));
				CoDisconnectObject((IOPCItemMgt*)group, 0);
				res = S_OK;
			}
			else
			{
				// remove server connection of COM group
				group->releaseComServer();
				res = OPC_S_INUSE;
			}
		}
		else
		{
			res = S_OK;
		}
	}

	if ((bForce) && (group != NULL))
	{
		grp->releaseMe(group);
	}

	SOSRV_END_INTERFACE_FUNCTION(RemoveGroup(res, hServerGroup, bForce))
} // RemoveGroup


//-----------------------------------------------------------------------------
// CreateGroupEnumerator (interface IOPCServer)
//
// - creates enumerator over group objects
//
// returns:
//		S_OK         - enumerator returned
//		S_FALSE      - empty enumerator returned
//		E_INVALIDARG - bad argument
//
STDMETHODIMP SODaSComServer::CreateGroupEnumerator(
	IN OPCENUMSCOPE dwScope,    // which groups
	IN REFIID riid,             // which enumerator
	OUT LPUNKNOWN* ppUnk)       // enumerator
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServer::CreateGroupEnumerator, CreateGroupEnumerator(dwScope, riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_CHECK(((riid == IID_IEnumString) || (riid == IID_IEnumUnknown)), E_NOINTERFACE)
	SOSRV_END_PARAMS_CHECK(CreateGroupEnumerator(res, dwScope, riid, ppUnk))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	// create the list
	SOSrvComEnumUnknownObject* unkList = NULL;
	SOSrvComEnumStringObject* strList = NULL;
	BOOL makeStrList = (riid == IID_IEnumString);

	if (makeStrList)
	{
		SOCMN_ALLOCATE_OBJECT(strList, SOSrvComEnumStringObject)
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(unkList, SOSrvComEnumUnknownObject)
	}

	// public groups
	if ((dwScope == OPC_ENUM_PUBLIC_CONNECTIONS) ||
		(dwScope == OPC_ENUM_PUBLIC) ||
		(dwScope == OPC_ENUM_ALL_CONNECTIONS) ||
		(dwScope == OPC_ENUM_ALL))
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSServer> pgserver = engine->getPublicGroups();

		if ((SODaSServer*)pgserver)
		{
			SOCmnList<SOCmnElement> groupList(pgserver->getBranchList());
			SODaSGroup* grp;
			SOCmnListPosition pos;
			IUnknown* unknown;
			// for all groups of the server
			pos = groupList.getStartPosition();

			while (pos)
			{
				grp = (SODaSGroup*)groupList.getNext(pos);

				if (makeStrList)
				{
					strList->m_list.add(grp->getName());
				}
				else
				{
					USES_CONVERSION;
					SOCmnString grpName = grp->getName();

					if (SUCCEEDED(GetPublicGroupByName(T2COLE(grpName), IID_IUnknown, &unknown)))
					{
						unkList->m_list.add(unknown);
						unknown->Release();
					}
				}
			}
		}
	}

	// private groups
	if ((dwScope == OPC_ENUM_PRIVATE_CONNECTIONS) ||
		(dwScope == OPC_ENUM_PRIVATE) ||
		(dwScope == OPC_ENUM_ALL_CONNECTIONS) ||
		(dwScope == OPC_ENUM_ALL))
	{
		SOCmnList<SOCmnElement> groupList(m_me->getBranchList());
		SODaSGroup* grp;
		SOSrvComObject<SODaSComGroup> *group;
		SOCmnListPosition pos;
		IUnknown* unknown;
		// for all groups of the server
		pos = groupList.getStartPosition();

		while (pos)
		{
			grp = (SODaSGroup*)groupList.getNext(pos);

			if (!grp->isPublicGroup())
			{
				if (makeStrList)
				{
					strList->m_list.add(grp->getName());
				}
				else
				{
					group = (SOSrvComObject<SODaSComGroup> *)grp->getMe();

					if (group)
					{
						group->QueryInterface(IID_IUnknown, (void**)&unknown);
						unkList->m_list.add(unknown);
						unknown->Release();
						grp->releaseMe(group);
					}
				}
			}
		}
	}

	// reset list
	if (makeStrList)
	{
		if (strList->m_list.isEmpty())
		{
			res = S_FALSE;    // no list entry -> return OK but nothing found
		}

		strList->Reset();
		strList->QueryInterface(IID_IEnumString, (void**)ppUnk);
	}
	else
	{
		if (unkList->m_list.IsEmpty())
		{
			res = S_FALSE;    // no list entry -> return OK but nothing found
		}

		unkList->Reset();
		unkList->QueryInterface(IID_IEnumUnknown, (void**)ppUnk);
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(CreateGroupEnumerator(res, dwScope, riid, ppUnk), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // CreateGroupEnumerator



//-----------------------------------------------------------------------------
// IOPCServerPublicGroups                                                     |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetPublicGroupByName (interface IOPCServerPublicGroups)
//
// - get public group interface by the name of the group
//
// returns:
//		S_OK           - interface return
//		OPC_E_NOTFOUND - no public groups server or group not found
//		E_NOINTERFACE  - object don't support requested interface
//		E_INVALIDARG   - invalid output buffer
//
STDMETHODIMP SODaSComServer::GetPublicGroupByName(
	IN LPCWSTR szName,      // group name
	IN REFIID riid,         // interface id
	OUT LPUNKNOWN* ppUnk)   // interface pointer
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServerPublicGroups::GetPublicGroupByName, GetPublicGroupByName(szName, riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_END_PARAMS_CHECK(GetPublicGroupByName(res, szName, riid, ppUnk))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szName));
	// find existing instance of public group
	SOCmnPointer<SODaSGroup> grp;
	grp = m_me->getGroupByName(name);

	if ((SODaSGroup*)grp)
	{
		if (grp->isPublic())
		{
			SOSrvComObject<SODaSComGroup> *group = (SOSrvComObject<SODaSComGroup> *)grp->getMe();

			if (group)
			{
				res = group->QueryInterface(riid, (void**)ppUnk);
				grp->releaseMe(group);
			}
			else
			{
				res = E_FAIL;
			}

			SOSRV_RETURN_INTERFACE_FUNCTION(GetPublicGroupByName(res, szName, riid, ppUnk))
		}
	}

	// no exisiting public group
	// -> lookup public group template
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSServer> pgserver = engine->getPublicGroups();

	if (!pgserver)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no public group server")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(OPC_E_NOTFOUND, GetPublicGroupByName(res, szName, riid, ppUnk))
	}

	grp = pgserver->getGroupByName(name);

	if (!grp)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no public group")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(OPC_E_NOTFOUND, GetPublicGroupByName(res, szName, riid, ppUnk))
	}

	// found public group template
	// -> clone template of group
	SOCmnPointer<SODaSGroup> clone;
	res = grp->clone(m_me, name, (SODaSGroup**)clone);

	if (SUCCEEDED(res))
	{
		HRESULT resQI;

		if (FAILED(resQI = createOPCGroup(clone, riid, ppUnk, FALSE)))
		{
			res = resQI;
		}

#ifdef SOFEATURE_WATCH
		else if (m_me->getCollectChanges())
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchAddObject(m_me, clone);
		}

#endif
	}

	SOSRV_END_INTERFACE_FUNCTION(GetPublicGroupByName(res, szName, riid, ppUnk))
} // GetPublicGroupByName


//-----------------------------------------------------------------------------
// RemovePublicGroup (interface IOPCServerPublicGroups)
//
// - removes a public group from the server
//
// returns:
//		S_OK         - removed
//		E_FAIL       - group removal rejected or no public groups server
//		OPC_S_INUSE  - group template removed but group object still exists
//		E_INVALIDARG - bad server handle
//
STDMETHODIMP SODaSComServer::RemovePublicGroup(
	IN OPCHANDLE hServerGroup,  // server handle
	IN BOOL bForce)             // force remove
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCServerPublicGroups::RemovePublicGroup, RemovePublicGroup(hServerGroup, bForce))
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSServer> pgserver = engine->getPublicGroups();

	if (!!pgserver)
	{
		SOCmnList<SODaSGroup> pGList((SOCmnList<SODaSGroup> *)pgserver->getBranchList());
		SODaSGroup* pG;
		SOCmnListPosition pGpos;
		SOCmnPointer<SODaSGroup> grpTempl;
		pGpos = pGList.getStartPosition();

		while (pGpos)
		{
			pG = pGList.getNext(pGpos);

			if (pG->getServerHandle() == hServerGroup)
			{
				grpTempl = pG;
				grpTempl.addRef();
				break;
			}
		}

		pGList.autoUnlock();

		if (grpTempl.isNotNull())
		{
			res = pgserver->removeGroup((OPCHANDLE)grpTempl->getOpcHandle());

			if ((SUCCEEDED(res)) && (grpTempl.isNotNull()))
			{
				// check if group object using the template existis
				SOCmnList<SOCmnElement> groupList(m_me->getBranchList());
				SODaSGroup* grp;
				SOCmnListPosition pos;
				// for all groups of the server
				pos = groupList.getStartPosition();

				while (pos)
				{
					grp = (SODaSGroup*)groupList.getNext(pos);

					if (grp->isPublicGroup())
					{
						if (grp->getServerHandle() == hServerGroup)
						{
							grp->addRef();
							groupList.autoUnlock();

							if (bForce)
							{
								SOSrvComObject<SODaSComGroup> *group = NULL;
								group = (SOSrvComObject<SODaSComGroup> *)grp->getMe();
								m_me->removeGroup((OPCHANDLE)grp->getOpcHandle(), TRUE);

								if (group)
								{
									CoDisconnectObject((IOPCItemMgt*)group, 0);
								}

								grp->releaseMe(group);
								res = S_OK;
							}
							else
							{
								res = OPC_S_INUSE;
							}

							grp->release();
							break;
						}
					}
				}
			}
		}
		else
		{
			res = E_INVALIDARG;
		}
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(RemovePublicGroup(res, hServerGroup, bForce))
} // RemovePublicGroup



//-----------------------------------------------------------------------------
// IOPCItemProperties                                                         |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// QueryAvailableProperties (interface IOPCItemProperties)
//
// - query all available properties of an item id
//
// returns:
//		S_OK                - successful queried properties
//		E_FAIL              - function failed
//		E_OUTOFMEMORY       - out of memeory
//		E_INVALIDARG        - invalid argument
//		OPC_E_UNKNOWNITEMID - item id is not in the servers address space
//
STDMETHODIMP SODaSComServer::QueryAvailableProperties(
	IN LPWSTR szItemID,             // tag path
	OUT DWORD* pdwCount,            // number of available properties
	OUT DWORD** ppPropertyIDs,      // property ids
	OUT LPWSTR** ppDescriptions,    // property descriptions
	OUT VARTYPE** ppvtDataTypes)    // property datatypes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemProperties::QueryAvailableProperties, QueryAvailableProperties(szItemID, pdwCount, ppPropertyIDs, ppDescriptions, ppvtDataTypes))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCount)
	SOSRV_OUT_PARAM_PTR(ppPropertyIDs)
	SOSRV_OUT_PARAM_PTR(ppDescriptions)
	SOSRV_OUT_PARAM_PTR(ppvtDataTypes)
	SOSRV_END_PARAMS_CHECK(QueryAvailableProperties(res, szItemID, pdwCount, ppPropertyIDs, ppDescriptions, ppvtDataTypes))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	USES_CONVERSION;
	SOCmnString itemId(W2CT(szItemID));
	SOCmnPointer<SOCmnElement> element;

	// check for vaild item id
	if (!m_me->isNameSpaceElement(itemId, &element))
		SOSRV_RETURN_INTERFACE_FUNCTION_RES((m_me->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID, QueryAvailableProperties(res, szItemID, pdwCount, ppPropertyIDs, ppDescriptions, ppvtDataTypes))
		SOCmnList<SODaSPropertyData> propList;

	SOCmnListPosition pos;
	SODaSPropertyData* propData;
	propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	res = m_me->getProperties(itemId, element, 0, NULL, FALSE, TRUE, &propList); // returnValues=TRUE while "dataType" for property "value" is required

	// copy property data to out buffers
	if (SUCCEEDED(res) && (propList))
	{
		DWORD i;
		res = S_OK;
		*pdwCount = propList.GetCount();
		*ppPropertyIDs = (DWORD*)::CoTaskMemAlloc(*pdwCount * sizeof(DWORD));
		*ppDescriptions = (LPWSTR*)::CoTaskMemAlloc(*pdwCount * sizeof(LPWSTR));
		*ppvtDataTypes = (VARTYPE*)::CoTaskMemAlloc(*pdwCount * sizeof(VARTYPE));

		if ((*ppPropertyIDs == NULL) || (*ppDescriptions == NULL) || (*ppvtDataTypes == NULL))
		{
			res = E_OUTOFMEMORY;
		}

		if (SUCCEEDED(res))
		{
			pos = propList.GetHeadPosition();

			for (i = 0; i < *pdwCount; i++)
			{
				propData = propList.GetNext(pos);
				(*ppDescriptions)[i] = SOCmnString::cloneToWCHAR(propData->m_descr);

				if ((*ppDescriptions)[i] == NULL)
				{
					res = E_OUTOFMEMORY;
					break;
				}

				(*ppPropertyIDs)[i] = propData->m_pid;
				(*ppvtDataTypes)[i] = propData->m_datatype;
			}
		}

		if (res == E_OUTOFMEMORY)
		{
			if (*ppPropertyIDs != NULL)
			{
				CoTaskMemFree(*ppPropertyIDs);
				*ppPropertyIDs = NULL;
			}

			if (*ppDescriptions != NULL)
			{
				for (i = 0; i < *pdwCount; i++)
				{
					if ((*ppDescriptions)[i])
					{
						CoTaskMemFree((*ppDescriptions)[i]);
					}
				}

				CoTaskMemFree(*ppDescriptions);
				*ppDescriptions = NULL;
			}

			if (ppvtDataTypes != NULL)
			{
				CoTaskMemFree(*ppvtDataTypes);
				*ppvtDataTypes = NULL;
			}

			*pdwCount = 0;
			res = E_OUTOFMEMORY;
		}
	}

	// free memory
	propList.destroy();
	SOSRV_END_INTERFACE_FUNCTION(QueryAvailableProperties(res, szItemID, pdwCount, ppPropertyIDs, ppDescriptions, ppvtDataTypes))
} // QueryAvailableProperties


//-----------------------------------------------------------------------------
// GetItemProperties (interface IOPCItemProperties)
//
// - get the values of some item properties
//
// returns:
//		S_OK                - successful queried properties
//		E_FAIL              - function failed
//		E_OUTOFMEMORY       - out of memeory
//		E_INVALIDARG        - invalid argument
//		OPC_E_UNKNOWNITEMID - item id is not in the servers address space
//
STDMETHODIMP SODaSComServer::GetItemProperties(
	IN LPWSTR szItemID,         // tag path
	IN DWORD dwCount,           // number of properties
	IN DWORD* pdwPropertyIDs,   // property ids
	OUT VARIANT** ppvData,      // property data
	OUT HRESULT** ppErrors)     // read errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemProperties::GetItemProperties, GetItemProperties(szItemID, dwCount, pdwPropertyIDs, ppvData, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_OUT_PARAM_PTR(ppvData)
	SOSRV_IN_PARAM(dwCount != 0)
	SOSRV_IN_PARAM(pdwPropertyIDs != NULL)
	SOSRV_END_PARAMS_CHECK(GetItemProperties(res, szItemID, dwCount, pdwPropertyIDs, ppvData, ppErrors))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	SOCmnString itemId(W2CT(szItemID));
	DWORD accessRights = OPC_READABLE;
	// create tag object
	SOCmnPointer<SOCmnElement> element = m_me->getNameSpaceElement(itemId);

	if (!element)
	{
		SOSRV_RETURN_INTERFACE_FUNCTION_RES((m_me->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID, GetItemProperties(res, szItemID, dwCount, pdwPropertyIDs, ppvData, ppErrors))
	}

	accessRights = ((SODaSItemTag*)(SOCmnElement*)element)->getAccessRights(m_me);
	// create return data
	*ppvData = (LPVARIANT) CoTaskMemAlloc(dwCount * sizeof(VARIANT));
	*ppErrors = (HRESULT*) CoTaskMemAlloc(dwCount * sizeof(HRESULT));

	if ((*ppvData) && (*ppErrors))
	{
		res = m_me->readProperties(itemId, element, accessRights, NULL, dwCount, pdwPropertyIDs, FALSE, NULL, *ppvData, NULL, *ppErrors);
	}
	else
	{
		res = E_OUTOFMEMORY;
	}

	if (FAILED(res))
	{
		if (*ppvData)
		{
			CoTaskMemFree(*ppvData);
			*ppvData = NULL;
		}

		if (*ppErrors)
		{
			CoTaskMemFree(*ppErrors);
			*ppErrors = NULL;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(GetItemProperties(res, szItemID, dwCount, pdwPropertyIDs, ppvData, ppErrors))
} // GetItemProperties


//-----------------------------------------------------------------------------
// LookupItemIDs (interface IOPCItemProperties)
//
// - get item ids for some item properties
//
// returns:
//		S_OK                - successful queried properties
//		E_FAIL              - function failed
//		E_OUTOFMEMORY       - out of memeory
//		E_INVALIDARG        - invalid argument
//		OPC_E_UNKNOWNITEMID - item id is not in the servers address space
//
STDMETHODIMP SODaSComServer::LookupItemIDs(
	IN LPWSTR szItemID,             // tag path
	IN DWORD dwCount,               // number of properties
	IN DWORD* pdwPropertyIDs,       // property ids
	OUT LPWSTR** ppszNewItemIDs,    // item ids of properties
	OUT HRESULT** ppErrors)         // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemProperties::LookupItemIDs, LookupItemIDs(szItemID, dwCount, pdwPropertyIDs, ppszNewItemIDs, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_OUT_PARAM_PTR(ppszNewItemIDs)
	SOSRV_IN_PARAM(dwCount != 0)
	SOSRV_IN_PARAM(pdwPropertyIDs != NULL)
	SOSRV_IN_PARAM_PTR(szItemID)
	SOSRV_IN_PARAM_RES((wcslen(szItemID) > 0), OPC_E_INVALIDITEMID)
	SOSRV_END_PARAMS_CHECK(LookupItemIDs(res, szItemID, dwCount, pdwPropertyIDs, ppszNewItemIDs, ppErrors))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	USES_CONVERSION;
	SOCmnString itemId(W2CT(szItemID));
	SOCmnString propPath;
	DWORD i;
	SOCmnPointer<SOCmnElement> element;

	// check for vaild item id
	if (!m_me->isNameSpaceElement(itemId, &element))
		SOSRV_RETURN_INTERFACE_FUNCTION_RES((m_me->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID, LookupItemIDs(res, szItemID, dwCount, pdwPropertyIDs, ppszNewItemIDs, ppErrors))
		// create return data
		*ppszNewItemIDs = (LPWSTR*) CoTaskMemAlloc(dwCount * sizeof(LPWSTR));

	*ppErrors = (HRESULT*) CoTaskMemAlloc(dwCount * sizeof(HRESULT));

	if ((*ppszNewItemIDs == NULL) || (*ppErrors == NULL))
	{
		res = E_OUTOFMEMORY;
	}

	// for every property id
	for (i = 0; ((SUCCEEDED(res)) && (i < dwCount)); i++)
	{
		if ((pdwPropertyIDs[i] >= 1) && (pdwPropertyIDs[i] <= 99))
		{
			(*ppszNewItemIDs)[i] = NULL;    // no item ids for pid 1 till 99
			(*ppErrors)[i] = OPC_E_INVALID_PID;
		}
		else
		{
			if (SUCCEEDED((*ppErrors)[i] = m_me->getPropertyItemID((SOCmnElement*)element, itemId, pdwPropertyIDs[i], propPath)))
			{
				(*ppszNewItemIDs)[i] = (LPWSTR) CoTaskMemAlloc((propPath.GetLength() + 1) * sizeof(WCHAR));
				wcscpy((*ppszNewItemIDs)[i], T2CW(propPath));
				propPath.empty();
			}
			else
			{
				(*ppszNewItemIDs)[i] = NULL;
			}
		}

		if (FAILED((*ppErrors)[i]))
		{
			res = S_FALSE;
		}
	}

	if (res == E_OUTOFMEMORY)
	{
		if (*ppszNewItemIDs)
		{
			CoTaskMemFree(*ppszNewItemIDs);
			*ppszNewItemIDs = NULL;
		}

		if (*ppErrors)
		{
			CoTaskMemFree(*ppErrors);
			*ppErrors = NULL;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(LookupItemIDs(res, szItemID, dwCount, pdwPropertyIDs, ppszNewItemIDs, ppErrors))
} // LookupItemIDs



//-----------------------------------------------------------------------------
// IOPCItemIO                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Read (interface IOPCItemIO)
//
// - read one or more item values
//
// returns:
//		S_OK                - successfully read all values
//    S_FALSE             - at least one read failed
//		E_FAIL              - function failed
//		E_OUTOFMEMORY       - out of memeory
//		E_INVALIDARG        - invalid argument
//
STDMETHODIMP SODaSComServer::Read(
	IN DWORD count,              // number of items to read
	IN LPCWSTR* pItemIDs,        // item id's
	IN DWORD* pMaxAge,           // max ages of cache values
	OUT VARIANT** ppValues,      // read values
	OUT WORD** ppQualities,      // read qualities
	OUT FILETIME** ppTimeStamps, // read timestamps
	OUT HRESULT** ppErrors)      // results of each single read operation
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemIO::Read, Read(count, pItemIDs, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppValues)
	SOSRV_OUT_PARAM_PTR(ppQualities)
	SOSRV_OUT_PARAM_PTR(ppTimeStamps)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(pItemIDs != NULL)
	SOSRV_IN_PARAM(pMaxAge != NULL)
	SOSRV_END_PARAMS_CHECK(Read(res, count, pItemIDs, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	// create return data
	*ppValues = (VARIANT*) CoTaskMemAlloc(count * sizeof(VARIANT));
	*ppQualities = (WORD*) CoTaskMemAlloc(count * sizeof(WORD));
	*ppTimeStamps = (FILETIME*) CoTaskMemAlloc(count * sizeof(FILETIME));
	*ppErrors = (HRESULT*) CoTaskMemAlloc(count * sizeof(HRESULT));

	if ((*ppValues == NULL) || (*ppQualities == NULL)   || (*ppTimeStamps == NULL)  || (*ppErrors == NULL))
	{
		if (*ppValues)
		{
			CoTaskMemFree(*ppValues);
			*ppValues = NULL;
		}

		if (*ppQualities)
		{
			CoTaskMemFree(*ppQualities);
			*ppQualities = NULL;
		}

		if (*ppTimeStamps)
		{
			CoTaskMemFree(*ppTimeStamps);
			*ppTimeStamps = NULL;
		}

		if (*ppErrors)
		{
			CoTaskMemFree(*ppErrors);
			*ppErrors = NULL;
		}

		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, Read(res, count, pItemIDs, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnString itemId;
	DWORD i;
	// create read transaction
	tac = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(count);
	}
	// for every item
	SOCmnEvents events;
	DWORD reqEventId = events.create();

	for (i = 0; ((SUCCEEDED(res)) && (i < count)); i++)
	{
		SOCmnPointer<SODaSItemTag> itemTag;
		// init value buffer
		::VariantInit(&((*ppValues)[i]));
		// get item tag object
		itemId.fromWCHAR(pItemIDs[i]);
		(*ppErrors)[i] = m_me->getItemTag(itemId, &itemTag);

		if (SUCCEEDED((*ppErrors)[i]))
		{
			// valid item id
			// check access rights
			if ((itemTag->getAccessRights(m_me) & OPC_READABLE) == OPC_READABLE)
			{
				SOCmnPointer<SODaSRequest> req;
				req = creator->createRequest(m_me, NULL, itemTag, SODaSRequest::read, SODaSRequest::serverMaxAge);

				if (req.isNotNull())
				{
					req->setItemTag(itemTag);
					req->setServer(m_me);
					req->setResultAddress((*ppErrors) + i);
					req->setResult(S_OK);
					req->setMaxAge(pMaxAge[i]);
					req->setReadValueAddress((*ppValues) + i);
					req->setReadQualityAddress((*ppQualities) + i);
					req->setReadTimeStampAddress((*ppTimeStamps) + i);
					req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
					tac->insertRequest(req);
				}
			}
			else
			{
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
			}
		}

		if (FAILED((*ppErrors)[i]))
		{
			// set failed values for quality and timestamp
			(*ppQualities)[i] = OPC_QUALITY_BAD;
			((*ppTimeStamps)[i]).dwLowDateTime  = 0L;
			((*ppTimeStamps)[i]).dwHighDateTime  = 0L;
			res = S_FALSE;
		}
	}

	// start read operations
	if (tac->getRequestCount() > 0)
	{
		tac->execute(m_me, &events, reqEventId, m_me->m_dueTimeRead);
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();
	_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)
	{
		if (SUCCEEDED(res))
		{
			SOCmnString trcHelpSZ, trcHelp2SZ;

			for (i = 0; i < count; i++)
			{
				itemId.fromWCHAR(pItemIDs[i]);
				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, itemId, (_T(" [%3.3d] value:%s ; quality:0x%X ; timestamp:%s [0x%X]"), i, variantWithType2string(&trcHelpSZ, &((*ppValues)[i])), (*ppQualities)[i], filetime2string(&trcHelp2SZ, &((*ppTimeStamps)[i])), (*ppErrors)[i]));
			}
		}
	}
	SOSRV_END_INTERFACE_FUNCTION(Read(res, count, pItemIDs, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
} // Read


//-----------------------------------------------------------------------------
// WriteVQT (interface IOPCItemIO)
//
// - wirte one or more item values with quality and timestamp
//
// returns:
//		S_OK                - successfully wrote all values
//    S_FALSE             - at least one write failed
//    OPC_E_NOTSUPPORTED  - unsupported combination of value, quality and timestamp
//		E_FAIL              - function failed
//		E_OUTOFMEMORY       - out of memeory
//		E_INVALIDARG        - invalid argument
//
STDMETHODIMP SODaSComServer::WriteVQT(
	IN DWORD count,          // number of items to read
	IN LPCWSTR* pItemIDs,    // item id's
	IN OPCITEMVQT* pItemVQT, // write value, quality and timestamp
	OUT HRESULT** ppErrors)  // results of each single write operation
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemIO::WriteVQT, WriteVQT(count, pItemIDs, pItemVQT, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(pItemIDs != NULL)
	SOSRV_IN_PARAM(pItemVQT != NULL)
	SOSRV_END_PARAMS_CHECK(WriteVQT(res, count, pItemIDs, pItemVQT, ppErrors))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SODASCOMSERVER_WAIT_FOR_IO_STARTUP
	USES_CONVERSION;
	// create return data
	*ppErrors = (HRESULT*) CoTaskMemAlloc(count * sizeof(HRESULT));

	if (*ppErrors == NULL)
	{
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, WriteVQT(res, count, pItemIDs, pItemVQT, ppErrors))
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnString itemId;
	DWORD i;
	// create write transaction
	tac = creator->createTransaction(NULL, SODaSTransaction::serverWrite, SODaSTransaction::synchronous, 0);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(count);
	}
	// for every item
	SOCmnEvents events;
	DWORD reqEventId = events.create();

	for (i = 0; ((SUCCEEDED(res)) && (i < count)); i++)
	{
		SOCmnPointer<SODaSItemTag> itemTag;
		// get item tag object
		itemId.fromWCHAR(pItemIDs[i]);
		(*ppErrors)[i] = m_me->getItemTag(itemId, &itemTag);

		if (SUCCEEDED((*ppErrors)[i]))
		{
			// valid item id
			// check access rights
			if ((itemTag->getAccessRights(m_me) & OPC_WRITEABLE) == OPC_WRITEABLE)
			{
				SOCmnPointer<SODaSRequest> req;
				req = creator->createRequest(m_me, NULL, itemTag, SODaSRequest::write, SODaSRequest::serverDevice);

				if (req.isNotNull())
				{
					req->setItemTag(itemTag);
					req->setServer(m_me);
					req->setResultAddress((*ppErrors) + i);
					req->setResult(S_OK);
					BYTE writeQT = 0;

					if (pItemVQT[i].bQualitySpecified)
					{
						writeQT |= SODASREQUEST_FLAG_QUALITY;
					}

					if (pItemVQT[i].bTimeStampSpecified)
					{
						writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
					}

					if (writeQT == 0)
					{
						if (!req->setWriteValue(&(pItemVQT[i].vDataValue)))
						{
							(*ppErrors)[i] = req->getResult();
						}
					}
					else
					{
						if (req->setWriteValue(&(pItemVQT[i].vDataValue)))
						{
							if (itemTag->supportWriteQT(writeQT))
							{
								if (pItemVQT[i].bQualitySpecified)
								{
									req->setWriteQuality(pItemVQT[i].wQuality);
								}

								if (pItemVQT[i].bTimeStampSpecified)
								{
									req->setWriteTimeStamp(SOCmnDateTime(&pItemVQT[i].ftTimeStamp));
								}
							}
							else
							{
								(*ppErrors)[i] = OPC_E_NOTSUPPORTED;
							}
						}
						else
						{
							(*ppErrors)[i] = req->getResult();
						}
					}

					if (SUCCEEDED((*ppErrors)[i]))
					{
						req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
						tac->insertRequest(req);
					}
				}
			}
			else
			{
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
			}
		}

		if (FAILED((*ppErrors)[i]))
		{
			// set failed values for quality and timestamp
			res = S_FALSE;

			if (itemTag.isNotNull())
			{
				_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, itemTag, (_T(" [%3.3d] [0x%X]"), i, (*ppErrors)[i]));
			}
			else
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T(" [%3.3d] [0x%X]"), i, (*ppErrors)[i]));
			}
		}
		else
		{
			_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)
			{
				SOCmnString trcHelpSZ;
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, itemTag, (_T(" [%3.3d] %s"), i, variantWithType2string(&trcHelpSZ, &(pItemVQT[i].vDataValue))));
			}
		}
	}

	// start write operations
	if (SUCCEEDED(res))
	{
		DWORD tacReqCnt = tac->getRequestCount();

		if (tacReqCnt > 0)
		{
			tac->execute(m_me, &events, reqEventId, m_me->m_dueTimeWrite);

			for (i = 0; i < tacReqCnt; i++)
			{
				if (FAILED((*ppErrors)[i]))
				{
					res = S_FALSE;
					break;
				}
			}
		}

		tac->setCompletedEvent(NULL);
		tac->emptyRequestsCompletedEvent();
	}
	else
	{
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(WriteVQT(res, count, pItemIDs, pItemVQT, ppErrors))
} // WriteVQT



//-----------------------------------------------------------------------------
// IOPCBrowse                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Browse (interface IOPCBrowse)
//
// - browse the namespace elements
//
// returns:
//    S_OK - The function was successful.
//    S_FALSE - The operation succeeded but there are one or more errors in ppBrowseElements.
//              Refer to individual error returns for more information.
//    OPC_E_UNKNOWNITEMID - The item is not known to the server.
//    OPC_E_INVALIDITEMID - The szItemID does not conform to the server's syntax.
//    OPC_E_INVALIDFILTER - The filter string (element or vendor) is not valid.
//    OPC_E_INVALIDCONTINUATIONPOINT - The specified continuation point is not valid.
//    E_OUTOFMEMORY - The operation could not be executed due to memory limitations.
//    E_INVALIDARG - An invalid argument was passed.
//    E_FAIL - The operation failed.
//
STDMETHODIMP SODaSComServer::Browse(
	IN LPWSTR itemID,                        // item id
	IN OUT LPWSTR* pContinuationPoint,       // continue point
	IN DWORD maxElementsReturned,            // maximum elements returned
	IN OPCBROWSEFILTER browseFilter,         // browse filter
	IN LPWSTR elementNameFilter,             // element string filter
	IN LPWSTR vendorFilter,                  // vendor specific filter
	IN BOOL returnAllProperties,             // return all properties?
	IN BOOL returnPropertyValues,            // return property values?
	IN DWORD propertyCount,                  // number of properties
	IN DWORD* pPropertyIDs,                  // id's of properties
	OUT BOOL* pMoreElements,                 // more than max elements?
	OUT DWORD* pCount,                       // number of elements
	OUT OPCBROWSEELEMENT** ppBrowseElements) // elements
{
	USES_CONVERSION;
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowse::Browse, Browse(itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("IOPCBrowse::Browse >>  itemID: %s, browseFilter: %lu, returnAllProperties: %s, propertyCount: %lu, returnPropertyValues: %s "), W2CT(itemID), (DWORD)browseFilter, BOOL2STR(returnAllProperties), propertyCount, BOOL2STR(returnPropertyValues)));
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM(pMoreElements, FALSE)
	SOSRV_OUT_PARAM(pCount, 0)
	SOSRV_OUT_PARAM_PTR(ppBrowseElements)
	SOSRV_IN_PARAM_PTR(pContinuationPoint);
	SOSRV_IN_PARAM(((browseFilter == OPC_BROWSE_FILTER_ALL) || (browseFilter == OPC_BROWSE_FILTER_BRANCHES) || (browseFilter == OPC_BROWSE_FILTER_ITEMS)))
	SOSRV_END_PARAMS_CHECK(Browse(res, itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP

	if (returnPropertyValues)
		SODASCOMSERVER_WAIT_FOR_IO_STARTUP

	SOCmnString itemId(W2CT(itemID));
	SOCmnPointer<SOCmnElement> element;
	SOSrvBrowser::browserFilterType fType = SOSrvBrowser::typeAll;
	SOCmnList<SODaSBrowseElementData> listBE;
	SOCmnString contPoint = W2T(*pContinuationPoint);
	DWORD i, j, k;
	*pContinuationPoint = NULL;

	if (maxElementsReturned == 0)
	{
		maxElementsReturned = 0xFFFFFFFF;
	}

	listBE.create((SOCmnListHeader::isRehashingEnabled() ? SOCMNLIST_TYPE_ORG_MAP : SOCMNLIST_TYPE_ORG_LIST) | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

	// check for valid item id
	if (itemId.getLength() != 0)
	{
		if (!m_me->isNameSpaceElement(itemId, &element))
			SOSRV_RETURN_INTERFACE_FUNCTION_RES((m_me->isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID, Browse(res, itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
		}
	else
	{
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
		element = entry->getNameSpaceRoot(m_me->getServerInstance());
	}

	switch (browseFilter)
	{
	case OPC_BROWSE_FILTER_ALL:
		fType = SOSrvBrowser::typeAll;
		break;

	case OPC_BROWSE_FILTER_BRANCHES:
		fType = SOSrvBrowser::typeBranch;
		break;

	case OPC_BROWSE_FILTER_ITEMS:
		fType = SOSrvBrowser::typeItem;
		break;
	}

	// get the browse elements
	res = m_me->browse(itemId, element, fType, W2T(elementNameFilter), W2T(vendorFilter), maxElementsReturned, contPoint, *pMoreElements, &listBE);

	if (SUCCEEDED(res))
	{
		*pCount = listBE.getCount();
		*pContinuationPoint = contPoint.cloneToWCHAR();

		if ((!contPoint.isEmpty()) && (*pMoreElements == TRUE))
		{
			*pMoreElements = FALSE;
		}

		if (*pCount > 0)
		{
			*ppBrowseElements = (OPCBROWSEELEMENT*) CoTaskMemAlloc(*pCount * sizeof(OPCBROWSEELEMENT));

			if (*ppBrowseElements == NULL)
			{
				SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, Browse(res, itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
			}

			memset(*ppBrowseElements, 0, (*pCount * sizeof(OPCBROWSEELEMENT)));
			SODaSBrowseElementData* pBE;
			SOCmnListPosition posBE;
			DWORD propCnt = 0;      // default for return all
			DWORD* pPropIDs = NULL; // default for return all
			BOOL doGetProps = TRUE;
			SOCmnList<SODaSPropertyData> listPD;
			SOCmnListPosition posPD;
			SODaSPropertyData* pPD;
			listPD.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

			if (!returnAllProperties)
			{
				if (propertyCount > 0)
				{
					propCnt = propertyCount;
					pPropIDs = pPropertyIDs;
				}
				else
				{
					doGetProps = FALSE;
				}
			}

			posBE = listBE.getStartPosition();
			i = 0;

			while (posBE)
			{
				pBE = listBE.getNext(posBE);
				// set the browse elements in the output buffer
				(*ppBrowseElements)[i].szName = pBE->m_name.cloneToWCHAR();
				(*ppBrowseElements)[i].szItemID = pBE->m_itemID.cloneToWCHAR();
				(*ppBrowseElements)[i].dwFlagValue = pBE->m_flags;

				if (doGetProps)
				{
					// get the properties of the namespace element
					(*ppBrowseElements)[i].ItemProperties.hrErrorID = m_me->getProperties(pBE->m_itemID, pBE->m_nsElement, propCnt, pPropIDs, FALSE, returnPropertyValues, &listPD);

					if (SUCCEEDED((*ppBrowseElements)[i].ItemProperties.hrErrorID))
					{
						if ((*ppBrowseElements)[i].ItemProperties.hrErrorID != S_OK)
						{
							res = S_FALSE;
						}

						// allocate property memory
						(*ppBrowseElements)[i].ItemProperties.dwNumProperties = listPD.getCount();
						(*ppBrowseElements)[i].ItemProperties.pItemProperties = (OPCITEMPROPERTY*) CoTaskMemAlloc((*ppBrowseElements)[i].ItemProperties.dwNumProperties * sizeof(OPCITEMPROPERTY));

						if ((*ppBrowseElements)[i].ItemProperties.pItemProperties == NULL)
						{
							*pCount = 0;
							CoTaskMemFree(*pContinuationPoint);
							*pContinuationPoint = NULL;

							for (j = 0; j < i; j++)
							{
								if ((*ppBrowseElements)[j].szName)
								{
									CoTaskMemFree((*ppBrowseElements)[j].szName);
								}

								if ((*ppBrowseElements)[j].szItemID)
								{
									CoTaskMemFree((*ppBrowseElements)[j].szItemID);
								}

								if ((*ppBrowseElements)[j].ItemProperties.pItemProperties)
								{
									for (k = 0; k < (*ppBrowseElements)[j].ItemProperties.dwNumProperties; k++)
									{
										if ((*ppBrowseElements)[j].ItemProperties.pItemProperties[k].szItemID)
										{
											CoTaskMemFree((*ppBrowseElements)[j].ItemProperties.pItemProperties->szItemID);
										}

										if ((*ppBrowseElements)[j].ItemProperties.pItemProperties[k].szDescription)
										{
											CoTaskMemFree((*ppBrowseElements)[j].ItemProperties.pItemProperties->szDescription);
										}

										SOCmnVariant::variantClear(&(*ppBrowseElements)[j].ItemProperties.pItemProperties[k].vValue);
									}

									CoTaskMemFree((*ppBrowseElements)[j].ItemProperties.pItemProperties);
								}
							}

							CoTaskMemFree(*ppBrowseElements);
							*ppBrowseElements = NULL;
							SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, Browse(res, itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
						}

						memset((*ppBrowseElements)[i].ItemProperties.pItemProperties, 0, ((*ppBrowseElements)[i].ItemProperties.dwNumProperties * sizeof(OPCITEMPROPERTY)));
						posPD = listPD.getStartPosition();
						k = 0;

						while (posPD)
						{
							pPD = listPD.getNext(posPD);
							(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].vtDataType = pPD->m_datatype;
							(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].dwPropertyID = pPD->m_pid;

							if (!pPD->m_descr.isEmpty())
							{
								(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].szDescription = pPD->m_descr.cloneToWCHAR();
							}

							if (!pPD->m_itemID.isEmpty())
							{
								(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].szItemID = pPD->m_itemID.cloneToWCHAR();
							}

							pPD->m_value.copyTo(&(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].vValue);
							(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].hrErrorID = pPD->m_result;
							k++;
						}
					}
					else
					{
						(*ppBrowseElements)[i].ItemProperties.dwNumProperties = 0;
						(*ppBrowseElements)[i].ItemProperties.pItemProperties = NULL;
						res = S_FALSE;
					}

					if (returnPropertyValues)
					{
						DWORD accessRights = OPC_READABLE;
						SOCmnPointer<SOCmnElement> objBE;

						if (pBE->m_nsElement.isNotNull())
						{
							objBE = pBE->m_nsElement;
						}
						else
						{
							objBE = m_me->getNameSpaceElement(pBE->m_itemID);
						}

						if (objBE.isNotNull())
						{
							accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(m_me);

							if (S_OK != m_me->readProperties(pBE->m_itemID, objBE, accessRights, &listPD, 0, NULL, FALSE, &(*ppBrowseElements)[i].ItemProperties, NULL, NULL, NULL))
							{
								(*ppBrowseElements)[i].ItemProperties.hrErrorID = S_FALSE;
								res = S_FALSE;
							}
						}
						else
						{
							for (k = 0; k < (*ppBrowseElements)[i].ItemProperties.dwNumProperties; k++)
							{
								(*ppBrowseElements)[i].ItemProperties.pItemProperties[k].hrErrorID = E_FAIL;
							}

							(*ppBrowseElements)[i].ItemProperties.hrErrorID = S_FALSE;
							res = S_FALSE;
						}
					}
				}
				else
				{
					(*ppBrowseElements)[i].ItemProperties.hrErrorID = S_OK;
					(*ppBrowseElements)[i].ItemProperties.dwNumProperties = 0;
					(*ppBrowseElements)[i].ItemProperties.pItemProperties = NULL;
				}

				listPD.removeAll();
				i++;
			}
		}
	}

	_TRACE_SUCCEEDED(res, SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("IOPCBrowse::Browse <<  count: %lu, moreElement: %s, continuationPoint: %s"), *pCount, BOOL2STR(*pMoreElements), W2CT(*pContinuationPoint)));
	SOSRV_END_INTERFACE_FUNCTION(Browse(res, itemID, pContinuationPoint, maxElementsReturned, browseFilter, elementNameFilter, vendorFilter, returnAllProperties, returnPropertyValues, propertyCount, pPropertyIDs, pMoreElements, pCount, ppBrowseElements))
} // Browse


//-----------------------------------------------------------------------------
// GetProperties (interface IOPCBrowse)
//
// - get properties of namespace elements
//
// returns:
//    S_OK - The function was successful.
//    S_FALSE - The operation succeeded but there are one or more errors in
//               ppItemProperties. Refer to individual error returns for more information.
//               S_FALSE is also returned, if hrErrorID of one OPCITEMPROPERTIES structure is S_FALSE.
//    E_OUTOFMEMORY - Not enough Memory.
//    E_INVALIDARG - An invalid argument was passed.
//    E_FAIL - The function failed.
//
STDMETHODIMP SODaSComServer::GetProperties(
	IN DWORD itemCount,                       // number of items
	IN LPWSTR* pItemIDs,                      // item is's
	IN BOOL returnPropertyValues,             // return value of properties
	IN DWORD propertyCount,                   // number of properties
	IN DWORD* pPropertyIDs,                   // id's of properties
	OUT OPCITEMPROPERTIES** ppItemProperties) // properties
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowse::GetProperties, GetProperties(itemCount, pItemIDs, returnPropertyValues, propertyCount, pPropertyIDs, ppItemProperties))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(itemCount > 0)
	SOSRV_OUT_PARAM_PTR(ppItemProperties)
	SOSRV_END_PARAMS_CHECK(GetProperties(res, itemCount, pItemIDs, returnPropertyValues, propertyCount, pPropertyIDs, ppItemProperties))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP

	if (returnPropertyValues)
		SODASCOMSERVER_WAIT_FOR_IO_STARTUP

	USES_CONVERSION;
	DWORD i, j, k;
	SOCmnString itemId;
	SOCmnPointer<SOCmnElement> element;
	SOCmnList<SODaSPropertyData> listPD;
	SOCmnListPosition posPD;
	SODaSPropertyData* pPD;
	listPD.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
	// allocate return buffer
	*ppItemProperties = (OPCITEMPROPERTIES*) CoTaskMemAlloc(itemCount * sizeof(OPCITEMPROPERTIES));

	if (*ppItemProperties == NULL)
	{
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, GetProperties(res, itemCount, pItemIDs, returnPropertyValues, propertyCount, pPropertyIDs, ppItemProperties))
	}

	memset(*ppItemProperties, 0, (itemCount * sizeof(OPCITEMPROPERTIES)));

	for (i = 0; i < itemCount; i++)
	{
		// check for vaild item id
		itemId.fromWCHAR(pItemIDs[i]);

		if (m_me->isNameSpaceElement(itemId, &element))
		{
			(*ppItemProperties)[i].hrErrorID = m_me->getProperties(itemId, element, propertyCount, pPropertyIDs, FALSE, returnPropertyValues, &listPD);

			if (SUCCEEDED((*ppItemProperties)[i].hrErrorID))
			{
				// allocate property memory
				(*ppItemProperties)[i].dwNumProperties = listPD.getCount();
				(*ppItemProperties)[i].pItemProperties = (OPCITEMPROPERTY*) CoTaskMemAlloc((*ppItemProperties)[i].dwNumProperties * sizeof(OPCITEMPROPERTY));

				if ((*ppItemProperties)[i].pItemProperties == NULL)
				{
					for (j = 0; j < i; j++)
					{
						if ((*ppItemProperties)[j].pItemProperties)
						{
							for (k = 0; k < (*ppItemProperties)[j].dwNumProperties; k++)
							{
								if ((*ppItemProperties)[j].pItemProperties[k].szItemID)
								{
									CoTaskMemFree((*ppItemProperties)[j].pItemProperties->szItemID);
								}

								if ((*ppItemProperties)[j].pItemProperties[k].szDescription)
								{
									CoTaskMemFree((*ppItemProperties)[j].pItemProperties->szDescription);
								}

								SOCmnVariant::variantClear(&(*ppItemProperties)[j].pItemProperties[k].vValue);
							}

							CoTaskMemFree((*ppItemProperties)[j].pItemProperties);
						}
					}

					CoTaskMemFree(*ppItemProperties);
					*ppItemProperties = NULL;
					SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, GetProperties(res, itemCount, pItemIDs, returnPropertyValues, propertyCount, pPropertyIDs, ppItemProperties))
				}

				memset((*ppItemProperties)[i].pItemProperties, 0, ((*ppItemProperties)[i].dwNumProperties * sizeof(OPCITEMPROPERTY)));
				posPD = listPD.getStartPosition();
				k = 0;

				while (posPD)
				{
					pPD = listPD.getNext(posPD);
					(*ppItemProperties)[i].pItemProperties[k].vtDataType = pPD->m_datatype;
					(*ppItemProperties)[i].pItemProperties[k].dwPropertyID = pPD->m_pid;
					(*ppItemProperties)[i].pItemProperties[k].szDescription = pPD->m_descr.cloneToWCHAR();
					(*ppItemProperties)[i].pItemProperties[k].szItemID = pPD->m_itemID.cloneToWCHAR();
					pPD->m_value.copyTo(&(*ppItemProperties)[i].pItemProperties[k].vValue);
					(*ppItemProperties)[i].pItemProperties[k].hrErrorID = pPD->m_result;
					k++;
				}

				if (returnPropertyValues)
				{
					DWORD accessRights = OPC_READABLE;
					SOCmnPointer<SOCmnElement> objBE;

					if (element.isNotNull())
					{
						objBE = element;
					}
					else
					{
						objBE = m_me->getNameSpaceElement(itemId);
					}

					if (objBE.isNotNull())
					{
						accessRights = ((SODaSItemTag*)(SOCmnElement*)objBE)->getAccessRights(m_me);
						res = m_me->readProperties(itemId, objBE, accessRights, &listPD, 0, NULL, FALSE, &(*ppItemProperties)[i], NULL, NULL, NULL);
					}
					else
					{
						for (k = 0; k < (*ppItemProperties)[i].dwNumProperties; k++)
						{
							(*ppItemProperties)[i].pItemProperties[k].hrErrorID = E_FAIL;
						}

						res = S_FALSE;
					}
				}
				else
				{
					res = (*ppItemProperties)[i].hrErrorID;
				}
			}
			else
			{
				(*ppItemProperties)[i].dwNumProperties = 0;
				(*ppItemProperties)[i].pItemProperties = NULL;
				res = S_FALSE;
			}

			element.release();
		}
		else
		{
			(*ppItemProperties)[i].hrErrorID = OPC_E_UNKNOWNITEMID;
			(*ppItemProperties)[i].dwNumProperties = 0;
			(*ppItemProperties)[i].pItemProperties = NULL;
			res = S_FALSE;
		}

		listPD.removeAll();
	}

	SOSRV_END_INTERFACE_FUNCTION(GetProperties(res, itemCount, pItemIDs, returnPropertyValues, propertyCount, pPropertyIDs, ppItemProperties))
} // GetProperties



//-----------------------------------------------------------------------------
// IOPCBrowseServerAddressSpace                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// QueryOrganization (interface IOPCBrowseServerAddressSpace)
//
// - checks the organisation type of the namespace
//
// returns:
//		S_OK         - returned type
//		E_INVALIDARG - bad arguments
//
STDMETHODIMP SODaSComServer::QueryOrganization(
	OUT OPCNAMESPACETYPE* pNameSpaceType)   // organisation type of the namespace
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowseServerAddressSpace::QueryOrganization, QueryOrganization(pNameSpaceType))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM(pNameSpaceType, OPC_NS_FLAT)
	SOSRV_END_PARAMS_CHECK(QueryOrganization(res, pNameSpaceType))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	*pNameSpaceType = (OPCNAMESPACETYPE)m_me->queryOrganization();
	SOSRV_END_INTERFACE_FUNCTION(QueryOrganization(res, pNameSpaceType))
} // QueryOrganization


//-----------------------------------------------------------------------------
// ChangeBrowsePosition (interface IOPCBrowseServerAddressSpace)
//
// - changes the current position in the namespace
//
// returns:
//		S_OK         - position changed
//		E_INVALIDARG - bad name or direction
//
STDMETHODIMP SODaSComServer::ChangeBrowsePosition(
	IN OPCBROWSEDIRECTION dwBrowseDirection,    // direction
	IN LPCWSTR szString)                        // down node name
{
	USES_CONVERSION;
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowseServerAddressSpace::ChangeBrowsePosition, ChangeBrowsePosition(dwBrowseDirection, szString))
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  direction: %u, name: %s"), dwBrowseDirection, W2CT(szString)));
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  current browse path: %s"), (LPCTSTR)m_me->m_curBrowsePath));
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(!((dwBrowseDirection == OPC_BROWSE_DOWN) && ((szString == NULL) || (wcslen(szString) == 0))))
	SOSRV_IN_PARAM(!((dwBrowseDirection == OPC_BROWSE_UP) && ((szString == NULL) || (wcslen(szString) != 0))))
	SOSRV_IN_PARAM(!((dwBrowseDirection != OPC_BROWSE_UP) && (dwBrowseDirection != OPC_BROWSE_TO) && (dwBrowseDirection != OPC_BROWSE_DOWN)))
	SOSRV_END_PARAMS_CHECK(ChangeBrowsePosition(res, dwBrowseDirection, szString))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	res = m_me->changeBrowsePosition((SOSrvBrowser::browserDirection)dwBrowseDirection, OLE2CT(szString));

	if (res == OPC_E_INVALIDBRANCHNAME)
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(ChangeBrowsePosition(res, dwBrowseDirection, szString))
} // ChangeBrowsePosition


//-----------------------------------------------------------------------------
// BrowseOPCItemIDs (interface IOPCBrowseServerAddressSpace)
//
// - browses the current branch for elements
//
// returns:
//		S_OK         - string enumerator returned
//		S_FALSE      - empty string enumerator returned
//		E_INVALIDARG - bad argument
//
STDMETHODIMP SODaSComServer::BrowseOPCItemIDs(
	IN OPCBROWSETYPE dwBrowseFilterType,    // get what? tags or nodes
	IN LPCWSTR szFilterCriteria,            // string filter
	IN VARTYPE vtDataTypeFilter,            // datatype filter
	IN DWORD dwAccessRightsFilter,          // access rights filter
	OUT LPENUMSTRING* ppIEnumString)        // child names
{
	USES_CONVERSION;
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowseServerAddressSpace::BrowseOPCItemIDs, BrowseOPCItemIDs(dwBrowseFilterType, szFilterCriteria, vtDataTypeFilter, dwAccessRightsFilter, ppIEnumString))
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  string filter: %s, data type filter: 0x%4.4X, access rights filter: %lu"), W2CT(szFilterCriteria), vtDataTypeFilter, dwAccessRightsFilter));
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  current browse path: %s"), (LPCTSTR)m_me->m_curBrowsePath));
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppIEnumString)
	SOSRV_IN_PARAM(!((dwBrowseFilterType != OPC_BRANCH) && (dwBrowseFilterType != OPC_LEAF) && (dwBrowseFilterType != OPC_FLAT)))
	SOSRV_IN_PARAM(dwAccessRightsFilter <= 3)
	SOSRV_END_PARAMS_CHECK(BrowseOPCItemIDs(res, dwBrowseFilterType, szFilterCriteria, vtDataTypeFilter, dwAccessRightsFilter, ppIEnumString))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	SOSrvComEnumStringObject* itemNameList;
	SOSrvBrowser::browserFilterType fType = SOSrvBrowser::typeAll;

	switch (dwBrowseFilterType)
	{
	case OPC_LEAF:
		fType = SOSrvBrowser::typeLeaf;
		break;

	case OPC_BRANCH:
		fType = SOSrvBrowser::typeBranch;
		break;

	case OPC_FLAT:
		fType = SOSrvBrowser::typeFlat;
		break;
	}

	SOCMN_ALLOCATE_OBJECT(itemNameList, SOSrvComEnumStringObject)

	if (SODaSCache::defaultSupportDatatype(vtDataTypeFilter))
	{
		res = m_me->browseCurrentBranch(fType, OLE2CT(szFilterCriteria),
										vtDataTypeFilter, dwAccessRightsFilter, itemNameList->m_list);
	}

	if (SUCCEEDED(res))
	{
		// no list entry -> return OK but nothing found
		if (itemNameList->m_list.isEmpty())
		{
			res = S_FALSE;
		}

		itemNameList->Reset();
		itemNameList->QueryInterface(IID_IEnumString, (void**)ppIEnumString);
	}
	else
	{
		SOCMN_FREE_OBJECT(itemNameList) // free list enumerator
		*ppIEnumString = NULL;
	}

#ifdef SOFEATURE_CMN_TRACE
	_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)
	{
		if (res == S_OK)
		{
			SOCmnListPosition pos;
			SOCmnString str;
			DWORD i = 0;
			pos = itemNameList->m_list.getStartPosition();
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  item count: %lu"), itemNameList->m_list.getCount()));

			while (pos)
			{
				str = itemNameList->m_list.getNext(pos);
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  [%u]: %s"), i, (LPCTSTR)str));
				i++;
			}
		}
	}
#endif
	SOSRV_END_INTERFACE_FUNCTION_TL(BrowseOPCItemIDs(res, dwBrowseFilterType, szFilterCriteria, vtDataTypeFilter, dwAccessRightsFilter, ppIEnumString), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // BrowseOPCItemIDs


//-----------------------------------------------------------------------------
// GetItemID (interface IOPCBrowseServerAddressSpace)
//
// - get the full qualified path for a tag or node name
//
// returns:
//		S_OK          - returned fully qualified name
//		E_INVALIDARG  - return pointer was NULL
//		E_OUTOFMEMORY - error allocating buffer for return
//
STDMETHODIMP SODaSComServer::GetItemID(
	IN LPWSTR szItemDataID, // element name
	OUT LPWSTR* szItemID)   // fully qualified name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowseServerAddressSpace::GetItemID, GetItemID(szItemDataID, szItemID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(szItemID)
	SOSRV_END_PARAMS_CHECK(GetItemID(res, szItemDataID, szItemID))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szItemDataID));
	SOCmnString path;

	// get the path and check if valid element
	if (m_me->getQualifiedElementName(name, path))
	{
		// item id is valid
		// copy path
		*szItemID = SOCmnString::cloneToWCHAR(path);

		if (!(*szItemID))
		{
			res = E_OUTOFMEMORY;
		}
	}
	else
	{
		res = E_INVALIDARG;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetItemID(res, szItemDataID, szItemID))
} // GetItemID


//-----------------------------------------------------------------------------
// BrowseAccessPaths (interface IOPCBrowseServerAddressSpace)
//
// - get the access path for a fully qualified tag
//
// returns:
//		S_OK          - got access path
//		S_FALSE       - no access path
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - error allocating buffer for return
//
STDMETHODIMP SODaSComServer::BrowseAccessPaths(
	IN LPCWSTR szItemID,                // fully qualified tag name
	OUT LPENUMSTRING* ppIEnumString)    // access path list enumerator
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCBrowseServerAddressSpace::BrowseAccessPaths, BrowseAccessPaths(szItemID, ppIEnumString))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppIEnumString)
	SOSRV_END_PARAMS_CHECK(BrowseAccessPaths(res, szItemID, ppIEnumString))
	SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP
	USES_CONVERSION;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root;
	SOCmnString name(W2CT(szItemID));
	SOCmnPointer<SOCmnElement> element;
	SOSrvComEnumStringObject* accessPathList;

	// check for vaild item id
	if (!m_me->isNameSpaceElement(name, &element))
	{
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, BrowseAccessPaths(res, szItemID, ppIEnumString))
	}

	SOCMN_ALLOCATE_OBJECT(accessPathList, SOSrvComEnumStringObject)

	if (element.isNotNull())
	{
		((SODaSItemTag*)(SOCmnElement*)element)->getAccessPaths(&accessPathList->m_list);
		res = S_OK;
	}
	else
	{
		if (m_me->isTagName(name))
		{
			res = m_me->getAccessPathsByItemID(name, accessPathList->m_list);
		}
		else
		{
			if (m_me->isNodeName(name))
			{
				if (m_me->getNodeDatatypeByItemID(name) != VT_EMPTY)
				{
					res = m_me->getAccessPathsByItemID(name, accessPathList->m_list);
				}
			}

			res = E_INVALIDARG;
		}
	}

	if (SUCCEEDED(res))
	{
#ifndef SOFLAG_WORKAROUND_RSI_CLIENT_BUG

		// no list entry -> return OK but nothing found
		if (accessPathList->m_list.isEmpty())
		{
			res = S_FALSE;
		}

		accessPathList->Reset();
		accessPathList->QueryInterface(IID_IEnumString, (void**)ppIEnumString);
#else

		// no list entry -> return OK but nothing found
		if (accessPathList->m_list.isEmpty())
		{
			res = S_FALSE;
			SOCMN_FREE_OBJECT(accessPathList) // free list enumerator
			*ppIEnumString = NULL;
		}
		else
		{
			accessPathList->Reset();
			accessPathList->QueryInterface(IID_IEnumString, (void**)ppIEnumString);
		}

#endif // SOFLAG_WORKAROUND_RSI_CLIENT_BUG
	}
	else
	{
		SOCMN_FREE_OBJECT(accessPathList)   // free list enumerator
		*ppIEnumString = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(BrowseAccessPaths(res, szItemID, ppIEnumString), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // BrowseAccessPaths



//-----------------------------------------------------------------------------
// IOPCSecurityPrivate                                                        |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// IsAvailablePriv (interface IOPCSecurityPrivate)
//
// - Query the current security configuration of the OPC server to determine
//   if the current server configuration provides OPC Security by private
//   credentials.
//
// returns:
//   S_OK         - The operation succeeded.
//   E_FAIL       - The operation failed.
//   E_INVALIDARG - An argument to the function was invalid
//
STDMETHODIMP SODaSComServer::IsAvailablePriv(
	OUT BOOL* pbAvailable) // is private security available
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSecurityPrivate::IsAvailablePriv, IsAvailablePriv(pbAvailable))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pbAvailable)
	SOSRV_END_PARAMS_CHECK(IsAvailablePriv(res, pbAvailable))
	*pbAvailable = m_me->isPrivateSecurityAvailable();
	SOSRV_END_INTERFACE_FUNCTION(IsAvailablePriv(res, pbAvailable))
} // IsAvailablePriv


//-----------------------------------------------------------------------------
// Logon (interface IOPCSecurityPrivate)
//
// - Changes the identity of the client application’s user. Future access to
//   any security objects will be authorized with the new user’s credentials
//   until a subsequent call to Logoff() or Logon(). For OPC Servers which also
//   implement IOPCSecurityNT, access checking with NT credentials will be disabled
//   until Logoff() is called.
//
// returns:
//   S_OK                  - The operation succeeded.
//   E_FAIL                - The operation failed.
//   E_ACCESSDENIED        - The credentials passed in could not be authenticated.
//   OPC_S_LOW_AUTHN_LEVEL - Server expected higher level of packet privacy
//
STDMETHODIMP SODaSComServer::Logon(
	IN LPCWSTR szUserID,   // The user’s logon name
	IN LPCWSTR szPassword) // The user’s password
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSecurityPrivate::Logon, Logon(szUserID, szPassword))
	USES_CONVERSION;
	LPCTSTR userName = (OLE2CT(szUserID));
	LPCTSTR password = (OLE2CT(szPassword));
	res = m_me->logon(userName, password) ? S_OK : E_ACCESSDENIED;
	SOSRV_END_INTERFACE_FUNCTION(Logon(res, szUserID, szPassword))
} // Logon


//-----------------------------------------------------------------------------
// Logoff (interface IOPCSecurityPrivate)
//
// - Remove the private credential established by the previous call to Logon().
//   OPC Security reverts to the state before the first call of Logon(), there
//   are no private credentials active for the client.
//
// returns:
//   S_OK   - The operation succeeded.
//   E_FAIL - The operation failed.
//
STDMETHODIMP SODaSComServer::Logoff(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSecurityPrivate::Logoff, Logoff())
	res = m_me->logoff() ? S_OK : E_FAIL;
	SOSRV_END_INTERFACE_FUNCTION(Logoff(res))
} // Logoff



//-----------------------------------------------------------------------------
// IPersistFile                                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetClassID (interface IPersist)
//
// - get the class id of the object
//
// returns:
//		S_OK         - returned class id
//		E_INVALIDARG - bad arguments
//
STDMETHODIMP SODaSComServer::GetClassID(
	OUT CLSID* pClassID)    // class id
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::GetClassID, GetClassID(pClassID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(pClassID != NULL)
	SOSRV_END_PARAMS_CHECK(GetClassID(res, pClassID))
	m_me->getCLSID(*pClassID);
	SOSRV_END_INTERFACE_FUNCTION(GetClassID(res, pClassID))
} // GetClassID


//-----------------------------------------------------------------------------
// IsDirty (interface IPersistFile)
//
// - check if the configuration file has changed since last save
//
// returns:
//		S_OK    - configuration file has changed
//		S_FALSE - configuration file hasn't changed
//
STDMETHODIMP SODaSComServer::IsDirty(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::IsDirty, IsDirty())
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSConfigFileHandler> cfg = engine->getConfigurationFileHandler();
	res = S_FALSE;

	if ((SODaSConfigFileHandler*)cfg)
	{
		// call configuration file handler
		if (cfg->isDirty(m_me))
		{
			res = S_OK;
		}
		else
		{
			res = S_FALSE;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(IsDirty(res), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // IsDirty


//-----------------------------------------------------------------------------
// Load (interface IPersistFile)
//
// - called to load a configuration file
//
// returns:
//		all allowed returns of IPersistFile::Load
//
STDMETHODIMP SODaSComServer::Load(
	IN LPCOLESTR pszFileName,   // file name
	IN DWORD dwMode)            // open mode
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::Load, Load(pszFileName, dwMode))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(pszFileName != NULL)
	SOSRV_END_PARAMS_CHECK(Load(res, pszFileName, dwMode))
	USES_CONVERSION;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSConfigFileHandler> cfg = engine->getConfigurationFileHandler();
	SOCmnString fileName(OLE2CT(pszFileName));
	res = E_FAIL;

	if ((SODaSConfigFileHandler*)cfg)
	{
		// call configuration file handler
		res = cfg->load(m_me, fileName, dwMode);
	}

	SOSRV_END_INTERFACE_FUNCTION(Load(res, pszFileName, dwMode))
} // Load


//-----------------------------------------------------------------------------
// Save (interface IPersistFile)
//
// - called to store a configuration file
//
// returns:
//		all allowed returns of IPersistFile::Save
//
STDMETHODIMP SODaSComServer::Save(
	IN LPCOLESTR pszFileName,   // file name
	IN BOOL fRemember)          // set current file to the saved file?
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::Save, Save(pszFileName, fRemember))
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSConfigFileHandler> cfg = engine->getConfigurationFileHandler();
	SOCmnString fileName;
	LPCTSTR name = NULL;
	USES_CONVERSION;
	res = E_FAIL;

	if (pszFileName != NULL)
	{
		fileName = OLE2CT(pszFileName);
		name = (LPCTSTR) fileName;
	}

	if ((SODaSConfigFileHandler*)cfg)
	{
		// call configuration file handler
		res = cfg->save(m_me, name, (fRemember != 0));
	}

	SOSRV_END_INTERFACE_FUNCTION(Save(res, pszFileName, fRemember))
} // Save


//-----------------------------------------------------------------------------
// SaveCompleted (interface IPersistFile)
//
// - complete saving of the configuration file
//
// returns:
//		all allowed returns of IPersistFile::SaveCompleted
//
STDMETHODIMP SODaSComServer::SaveCompleted(
	IN LPCOLESTR pszFileName)   // file name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::SaveCompleted, SaveCompleted(pszFileName))
	SOSRV_END_INTERFACE_FUNCTION(SaveCompleted(res, pszFileName))
} // SaveCompleted


//-----------------------------------------------------------------------------
// GetCurFile (interface IPersistFile)
//
// - return the name of the current configuration file
//
// returns:
//		all allowed returns of IPersistFile::GetCurFile
//
STDMETHODIMP SODaSComServer::GetCurFile(
	OUT LPOLESTR* ppszFileName) // current file name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IPersistFile::GetCurFile, GetCurFile(ppszFileName))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppszFileName)
	SOSRV_END_PARAMS_CHECK(GetCurFile(res, ppszFileName))
	SOCmnString fileName;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSConfigFileHandler> cfg = engine->getConfigurationFileHandler();
	res = E_FAIL;

	if ((SODaSConfigFileHandler*)cfg)
	{
		// call configuration file handler
		res = cfg->getCurFile(m_me, fileName);

		if (SUCCEEDED(res))
		{
			*ppszFileName = SOCmnString::cloneToWCHAR(fileName);

			if (!(*ppszFileName))
			{
				res = E_OUTOFMEMORY;
			}
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(GetCurFile(res, ppszFileName))
} // GetCurFile



//-----------------------------------------------------------------------------
// IConnectionPoint                                                           |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Advise (interface IConnectionPoint)
//
// - establishes a connection between the connection point object and
//   the client's sink
//
// returns:
//		S_OK                    - the connection has been established
//		E_POINTER               - a parameter value is not valid
//		CONNECT_E_ADVISELIMIT   - the connection point has already reached its
//                                limit of connections
//		CONNECT_E_CANNOTCONNECT - the sink does not support the interface required
//                                by this connection point
//
STDMETHODIMP SODaSComServer::Advise(
	IN IUnknown* unknown,   // client sink
	OUT DWORD* cookie)      // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IConnectionPoint::Advise, Advise(unknown, cookie))
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();

	if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
	{
		setProxyBlanket(unknown, entry->getAuthentificationLevel(),
						entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
	}

	if (m_advisesSync.lock(2000))
	{
		res = IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>::Advise(unknown, cookie);
		m_advisesSync.unlock();

		if (SUCCEEDED(res) && (entry->isInitialized(SOSRVENTRY_INIT_SECURITY)))
		{
			IOPCShutdown* shutdown;
			res = unknown->QueryInterface(IID_IOPCShutdown, (void**)&shutdown);

			if (SUCCEEDED(res))
			{
				setProxyBlanket(shutdown, entry->getAuthentificationLevel(),
								entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
				shutdown->Release();
			}
		}
	}
	else
	{
		res = E_PENDING;
	}

	SOSRV_END_INTERFACE_FUNCTION(Advise(res, unknown, cookie))
} // Advise


//-----------------------------------------------------------------------------
// Unadvise (interface IConnectionPoint)
//
// - terminates an advisory connection
//
// returns:
//		S_OK                   - The connection was successfully terminated
//		CONNECT_E_NOCONNECTION - the cookie does not represent a valid connection
//
STDMETHODIMP SODaSComServer::Unadvise(
	IN DWORD cookie)    // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IConnectionPoint::Unadvise, Unadvise(cookie))

	if (m_advisesSync.lock(2000))
	{
		res = IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>::Unadvise(cookie);
		m_advisesSync.unlock();
	}
	else
	{
		res = E_PENDING;
	}

	SOSRV_END_INTERFACE_FUNCTION(Unadvise(res, cookie))
} // Unadvise



//-----------------------------------------------------------------------------
// ISoftingOPCWatch                                                           |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetChildren (interface ISoftingOPCWatch)
//
// - browse the available object trees
//
// returns:
//		S_OK         - information retured
//		E_INVALIDARG - invalid parent object
//    E_NOTIMPL    - watch functionality not activated
//
STDMETHODIMP SODaSComServer::GetChildren(
	IN LPCWSTR parent,      // parent object
	IN BOOL withAttributes, // add the object attributes
	OUT LPWSTR* children)   // children descriptions
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCWatch::GetChildren, GetChildren(parent, withAttributes, children))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(children)
	SOSRV_END_PARAMS_CHECK(GetChildren(res, parent, withAttributes, children))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> watch = m_me->getWatchObject();

	if (watch.isNotNull())
	{
		USES_CONVERSION;
		LPTSTR pP = W2T((LPWSTR)parent);
		SOCmnStringEx c;
		res = watch->getChildren(pP, withAttributes, c);

		if (SUCCEEDED(res))
		{
			*children = c.cloneToWCHAR();
		}
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetChildren(res, parent, withAttributes, children), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetChildren


//-----------------------------------------------------------------------------
// CollectChanges (interface ISoftingOPCWatch)
//
// - enable or disable the collection of attribute changes
//
// returns:
//		S_OK         - en/disabled collection of all objects
//		E_INVALIDARG - at least one invalid object
//    E_NOTIMPL    - watch functionality not activated
//
STDMETHODIMP SODaSComServer::CollectChanges(
	IN LPCWSTR objects,
	IN DWORD doCollect)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCWatch::CollectChanges, CollectChanges(objects, doCollect))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> watch = m_me->getWatchObject();

	if (watch.isNotNull())
	{
		USES_CONVERSION;
		LPTSTR pO = W2T((LPWSTR)objects);
		res = watch->collectChanges(pO, doCollect);
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(CollectChanges(res, objects, doCollect), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // CollectChanges


//-----------------------------------------------------------------------------
// GetChanges (interface ISoftingOPCWatch)
//
// - get the changes of the objects
//
// returns:
//		S_OK      - returned the changes information
//    E_NOTIMPL - watch functionality not activated
//
STDMETHODIMP SODaSComServer::GetChanges(
	OUT LPWSTR* changes) // changes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCWatch::GetChanges, GetChanges(changes))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(changes)
	SOSRV_END_PARAMS_CHECK(GetChanges(res, changes))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> watch = m_me->getWatchObject();

	if (watch.isNotNull())
	{
		SOCmnStringEx c;
		res = watch->getChanges(c);

		if (SUCCEEDED(res))
		{
			*changes = c.cloneToWCHAR();
		}
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetChanges(res, changes), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetChanges


//-----------------------------------------------------------------------------
// GetStatistics (interface ISoftingOPCWatch)
//
// - get the statistics infomation of the server
//
// returns:
//		S_OK      - returned the statistic information
//    E_NOTIMPL - watch functionality not activated
//
STDMETHODIMP SODaSComServer::GetStatistics(
	OUT LPWSTR* statistics) // statistic information
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCWatch::GetStatistics, GetStatistics(statistics))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(statistics)
	SOSRV_END_PARAMS_CHECK(GetStatistics(res, statistics))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> watch = m_me->getWatchObject();

	if (watch.isNotNull())
	{
		SOCmnStringEx s;
		res = watch->getStatistics(s);

		if (SUCCEEDED(res))
		{
			*statistics = s.cloneToWCHAR();
		}
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetStatistics(res, statistics), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetStatistics


//-----------------------------------------------------------------------------
// GetObject (interface ISoftingOPCWatch)
//
// - get all attributes of an object
//
// returns:
//		S_OK         - information retured
//		E_INVALIDARG - invalid object description
//    E_NOTIMPL    - watch functionality not activated
//
STDMETHODIMP SODaSComServer::GetObject(
	IN LPCWSTR objectDescription, // object description
	OUT LPWSTR* object)           // object with all attributes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCWatch::GetObject, GetObject(objectDescription, object))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(object)
	SOSRV_END_PARAMS_CHECK(GetObject(res, objectDescription, object))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> watch = m_me->getWatchObject();

	if (watch.isNotNull())
	{
		USES_CONVERSION;
		LPTSTR od = W2T((LPWSTR)objectDescription);
		SOCmnStringEx o;
		res = watch->getObject(od, o);

		if (SUCCEEDED(res))
		{
			*object = o.cloneToWCHAR();
		}
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetObject(res, objectDescription, object), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetObject



//-----------------------------------------------------------------------------
// ISoftingOPCTrace                                                           |
//-----------------------------------------------------------------------------

STDMETHODIMP SODaSComServer::SetTraceOptions(
	IN LPCWSTR options)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCTrace::SetTraceOptions, SetTraceOptions(options))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvTrace> trace = m_me->getTraceObject();

	if (trace.isNotNull())
	{
		USES_CONVERSION;
		LPTSTR pO = W2T((LPWSTR)options);
		res = trace->setOptions(pO);
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(SetTraceOptions(res, options), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // SetTraceOptions


STDMETHODIMP SODaSComServer::GetTraceOptions(OUT LPWSTR* options)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCTrace::GetTraceOptions, GetTraceOptions(options))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(options)
	SOSRV_END_PARAMS_CHECK(GetTraceOptions(res, options))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvTrace> trace = m_me->getTraceObject();

	if (trace.isNotNull())
	{
		SOCmnStringEx o;
		res = trace->getOptions(o);

		if (SUCCEEDED(res))
		{
			*options = o.cloneToWCHAR();
		}
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetTraceOptions(res, options), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetTraceOptions


STDMETHODIMP SODaSComServer::SaveTraceOptions(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCTrace::SaveTraceOptions, SaveTraceOptions())
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvTrace> trace = m_me->getTraceObject();

	if (trace.isNotNull())
	{
		res = trace->saveOptions();
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(SaveTraceOptions(res), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // SaveTraceOptions


STDMETHODIMP SODaSComServer::GetTrace(
	OUT LPWSTR* trcMsgs)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, ISoftingOPCTrace::GetTrace, GetTrace(trcMsgs))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(trcMsgs)
	SOSRV_END_PARAMS_CHECK(GetTrace(res, trcMsgs))
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvTrace> trace = m_me->getTraceObject();

	if (trace.isNotNull())
	{
		res = trace->get(trcMsgs);
	}
	else
#endif
		res = E_NOTIMPL;

	SOSRV_END_INTERFACE_FUNCTION_TL(GetTrace(res, trcMsgs), (SUCCEEDED(res) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR))
} // GetTrace



//-----------------------------------------------------------------------------
// sendShutdownRequest
//
// - send shutdown request to the client
//
// returns:
//		S_OK    - return status
//		S_FALSE - invalid argument
//
HRESULT SODaSComServer::sendShutdownRequest(
	IN LPCTSTR reason)  // shutdown reason
{
	USES_CONVERSION;
	IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
	IUnknown** pp;
	IOPCShutdown* pIShutdown;
	SOCmnVOIDList cookieList;
	SOCmnListPosition pos;
	HRESULT res = S_FALSE;
	// copy advise list
	cookieList.create();
	m_advisesSync.lock();
	pp = p->m_vec.begin();

	while (pp < p->m_vec.end())
	{
		if (*pp != NULL)
		{
			cookieList.add((void*)*pp);
		}

		pp++;
	}

	m_advisesSync.unlock();
	// loop all clients
	((SOSrvComObject<SODaSComServer> *)this)->internalAddRef();
	pos = cookieList.getStartPosition();

	while (pos)
	{
		pIShutdown = (IOPCShutdown*)cookieList.getNext(pos);
		// call shutdown
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call shutdown request; reason:%s"), reason))
		SOCMN_TRY
		{
			res = pIShutdown->ShutdownRequest(T2CW(reason));
			_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCShutdown::ShutdownRequest [0x%X]"), res));

			if (SOSRV_IS_RPC_ERROR(res))
			{
				// client advise object not available anymore
				m_me->disconnectClient(); // -> disconnect client
			}
		}
		SOCMN_CATCH(...)
		{
			// exception in callback (only inproc)
			res = RPC_E_SERVERFAULT;
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in ShutdownRequest")));
		}
	}

	((SOSrvComObject<SODaSComServer> *)this)->internalRelease();
	return res;
} // sendShutdownRequest


//-----------------------------------------------------------------------------
// connectedShutdownInterface
//
// - check if a shutdown interface is connected
//
// returns:
//		TRUE  - interface connected
//		FALSE - no interface connected
//
BOOL SODaSComServer::connectedShutdownInterface(void)
{
	IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
	BOOL ret;
	m_advisesSync.lock();
	ret = (p->m_vec.begin() < p->m_vec.end());
	m_advisesSync.unlock();
	return ret;
} // connectedShutdownInterface


//-----------------------------------------------------------------------------
// checkClientConnection
//
// - check if the client is alive
//
// returns:
//		TRUE  - client reacts
//		FALSE - client don't react
//
BOOL SODaSComServer::checkClientConnection(void)
{
	IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
	IUnknown** pp;
	IOPCShutdown* pIShutdown;
	IUnknown* unk;
	HRESULT res;
	BOOL conOK;
	m_advisesSync.lock();
	conOK = (p->m_vec.begin() < p->m_vec.end());
	pp = p->m_vec.begin();

	while (pp < p->m_vec.end())
	{
		if (*pp != NULL)
		{
			pIShutdown = (IOPCShutdown*)*pp;
			res = pIShutdown->QueryInterface(IID_IOPCServer, (void**)&unk);

			if (FAILED(res))
			{
				if (SOSRV_IS_RPC_ERROR(res))
				{
					conOK = FALSE;
					break;
				}
			}
			else
			{
				unk->Release();
			}
		}

		pp++;
	}

	m_advisesSync.unlock();
	return conOK;
} // checkClientConnection


//-----------------------------------------------------------------------------
// releaseAllAdvises
//
// - release all advise interfaces
//
void SODaSComServer::releaseAllAdvises(void)
{
	IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
	IUnknown** pp;
	m_advisesSync.lock();
	pp = p->m_vec.begin();

	while (pp < p->m_vec.end())
	{
		if (*pp != NULL)
		{
			(*pp)->Release();
		}

		pp++;
	}

	p->m_vec.clear();
	m_advisesSync.unlock();
} // releaseAllAdvises


//-----------------------------------------------------------------------------
// queryInterface
//
// - additional query interface handling
//
// returns:
//		E_NOTIMPL     - do the default queryInterface of the object
//		S_OK          - interface supported. pointer returned
//		E_NOINTERFACE - interface not supported
//
HRESULT SODaSComServer::queryInterface(
	IN REFIID iid,      // interface id
	OUT void** object)  // interface pointer
{
	HRESULT res;

	if ((res = m_me->queryInterface(iid, object)) != E_NOTIMPL)
	{
		return res;
	}

	if (iid == IID_IPersistFile)
	{
		// check if configuration file handler is set
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSConfigFileHandler> cfg = engine->getConfigurationFileHandler();

		// support IPersistFile only if config file handler is set
		if ((SODaSConfigFileHandler*)cfg)
		{
			return E_NOTIMPL;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
	else if ((iid == IID_ISoftingOPCWatch) || (iid == IID_ISoftingOPCTrace))
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();

		if (entry->getServerTypeCOM() == SOSrvEntry::InProc)
		{
			return E_NOINTERFACE;
		}
		else
		{
			return E_NOTIMPL;
		}
	}

	return E_NOTIMPL;
} // queryInterface


//-----------------------------------------------------------------------------
// lastExternalReleaseWithInternalRefs
//
// - released last external reference with pending internal references
//
// returns:
//		TRUE  - object deleted
//		FALSE - object still exists
//
BOOL SODaSComServer::lastExternalReleaseWithInternalRefs(void)
{
	if (m_me)
	{
		return m_me->handleLastExternalReleaseWithInternalRefs((void*)this, (void*)this);
	}
	else
	{
		return FALSE;
	}
} // lastExternalReleaseWithInternalRefs




//-----------------------------------------------------------------------------
// SODaSComServer Instances                                                   |
//-----------------------------------------------------------------------------

SODaSComServer0::SODaSComServer0()
{
	m_serverInstance = 0;
}

SODaSComServer1::SODaSComServer1()
{
	m_serverInstance = 1;
}

SODaSComServer2::SODaSComServer2()
{
	m_serverInstance = 2;
}

SODaSComServer3::SODaSComServer3()
{
	m_serverInstance = 3;
}

SODaSComServer4::SODaSComServer4()
{
	m_serverInstance = 4;
}

SODaSComServer5::SODaSComServer5()
{
	m_serverInstance = 5;
}

SODaSComServer6::SODaSComServer6()
{
	m_serverInstance = 6;
}

SODaSComServer7::SODaSComServer7()
{
	m_serverInstance = 7;
}

SODaSComServer8::SODaSComServer8()
{
	m_serverInstance = 8;
}

SODaSComServer9::SODaSComServer9()
{
	m_serverInstance = 9;
}

#endif // SOFEATURE_DCOM

#endif // SODAS
