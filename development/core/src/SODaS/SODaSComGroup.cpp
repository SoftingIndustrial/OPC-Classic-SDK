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
//  Filename    : SODaSComGroup.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : COM OPC Group object                                        |
//                - SODaSComGroup: OPC DA group COM object                    |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_DCOM

#include "SODaS.h"
#include "SODaSEntry.h"
#include "SODaSComServer.h"
#include "SODaSComGroup.h"
#include "SODaSServer.h"
#include "SODaSGroup.h"
#include "SODaSItem.h"
#include "SOSrvComEnum.h"
#include "SODaSComEnum.h"
#include "SOCmnTrace.h"
#include <opcerror.h>
#include <opcda.h>


#ifdef SOFEATURE_DA10
// OPC DA V1.0 clipboard formats
const UINT OPCSTMFORMATDATA = ::RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
const UINT OPCSTMFORMATDATATIME = ::RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
const UINT OPCSTMFORMATWRITECOMPLETE = ::RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));
#else
const UINT OPCSTMFORMATDATA = 0;
const UINT OPCSTMFORMATDATATIME = 0;
const UINT OPCSTMFORMATWRITECOMPLETE = 0;
#endif

//-----------------------------------------------------------------------------
// SODaSComGroup                                                              |
//-----------------------------------------------------------------------------

SODaSComGroup::SODaSComGroup()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_server = NULL;
	m_me = NULL;
	_TRACE_CREATE_COMOBJ();
}

SODaSComGroup::~SODaSComGroup()
{
	_TRACE_DELETE_COMOBJ();

	if (m_me)
	{
		if (m_me->isPublicGroup())
		{
			// public group COM object destroys the tree objects
			SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
			SOCmnPointer<SODaSTransaction> tac = (SODaSTransaction*)m_me->getCyclicTransaction();
			// remove item objects
			m_me->removeAllLeafs(FALSE);

			if ((SODaSServer*)srv)
			{
				// remove transaction from server
				srv->removeTransaction(tac);
				// remove tree object from server
				srv->removeBranch(m_me);
			}
		}
	}

	// release server object
	releaseComServer();

	if (m_me)
	{
		m_me->m_me = NULL;
		m_me->release();    // release tree object
	}

	lastCOMObjectActions();
}

//-----------------------------------------------------------------------------
// init
//
// - initialize COM group object
//
void SODaSComGroup::init(
	SODaSComServer* server, // COM server object
	SODaSGroup* group)      // group object
{
	m_server = server;
	m_me = group;
	m_me->m_me = this;
	m_me->addRef();
	// keep server alive till group is freed
	((SOSrvComObject<SODaSComServer> *)m_server)->internalAddRef();
} // init


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SODaSComGroup::lockMe(void)
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
void SODaSComGroup::unlockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.unlock();
	}
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite the SODaSComGroup object from the SODaSGroup object
//
void SODaSComGroup::disuniteMe(void)
{
	if (m_me)
	{
		m_me->m_me = NULL;
	}
} // disuniteMe


//-----------------------------------------------------------------------------
// releaseComServer
//
// - release SODaSComServer object
//
void SODaSComGroup::releaseComServer(void)
{
	if (m_server)
	{
		((SOSrvComObject<SODaSComServer> *)m_server)->internalRelease();
		m_server = NULL;
	}
} // releaseComServer


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//      trace id string
//
LPCTSTR SODaSComGroup::getObjTraceId(void)
{
	if (m_objectTraceId.IsEmpty())
	{
		if (m_me)
		{
			m_objectTraceId.Format(_T("(grp|%8.8X| %s)"), this->getOpcHandle(), (LPCTSTR)m_me->getName());
		}
		else
		{
			m_objectTraceId.Format(_T("(grp|%8.8X)"), this->getOpcHandle());
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
//      TRUE
//
BOOL SODaSComGroup::setObjTraceId(
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
// IOPCGroupStateMgt                                                          |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetState (interface IOPCGroupStateMgt)
//
// - get attributes of the group
//
// returns:
//      S_OK          - all got
//      E_FAIL        - group allready removed
//      E_OUTOFMEMORY - can't allocate memory for name string
//
STDMETHODIMP SODaSComGroup::GetState(
	OUT DWORD* pUpdateRate,         // update rate
	OUT BOOL* pActive,              // activation state
	OUT LPWSTR* ppName,             // name
	OUT LONG* pTimeBias,            // time zone
	OUT FLOAT* pDeadband,           // deadband
	OUT DWORD* pLCID,               // locale id
	OUT OPCHANDLE* phClientGroup,   // client handle
	OUT OPCHANDLE* phServerGroup)   // server handle
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt::GetState, GetState(pUpdateRate, pActive, ppName, pTimeBias, pDeadband, pLCID, phClientGroup, phServerGroup))

	// check if the group was already removed
	if (m_me->isRemoved())
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, GetState(res, pUpdateRate, pActive, ppName, pTimeBias, pDeadband, pLCID, phClientGroup, phServerGroup))
		if (pUpdateRate)
		{
			*pUpdateRate = m_me->getUpdateRate();
		}

	if (pActive)
	{
		*pActive = m_me->getActive();
	}

	if (pTimeBias)
	{
		*pTimeBias = m_me->getTimeBias();
	}

	if (pDeadband)
	{
		*pDeadband = m_me->getDeadBand();
	}

	if (pLCID)
	{
		*pLCID = m_me->getLCID();
	}

	if (phClientGroup)
	{
		*phClientGroup = m_me->getClientHandle();
	}

	if (phServerGroup)
	{
		*phServerGroup = m_me->getServerHandle();
	}

	if (ppName)
	{
		*ppName = SOCmnString::cloneToWCHAR(m_me->getName());

		if (*ppName == NULL)
		{
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(GetState(res, pUpdateRate, pActive, ppName, pTimeBias, pDeadband, pLCID, phClientGroup, phServerGroup))
} // GetState


//-----------------------------------------------------------------------------
// SetState (interface IOPCGroupStateMgt)
//
// - sets the attributes of the group
//
// returns:
//      S_OK                  - set attributes
//      E_FAIL                - group allready removed
//      OPC_S_UNSUPPORTEDRATE - update rate is not the requested
//
STDMETHODIMP SODaSComGroup::SetState(
	IN DWORD* pRequestedUpdateRate, // reqested update rate
	OUT DWORD* pRevisedUpdateRate,  // revised update rate
	IN BOOL* pActive,               // activation state
	IN LONG* pTimeBias,             // time zone
	IN FLOAT* pPercentDeadband,     // deadband
	IN DWORD* pLCID,                // locale id
	IN OPCHANDLE* phClientGroup)    // client handle
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt::SetState, SetState(pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pRevisedUpdateRate)
	SOSRV_END_PARAMS_CHECK(SetState(res, pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))
	DWORD setUpdateRate;
	BOOL prevActive;

	// check if the group was already removed
	if (m_me->isRemoved())
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, SetState(res, pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))

		// set group parameters
		if (pLCID)
		{
			SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

			if (!!srv)
			{
				if (!srv->supportLCID(*pLCID))
					SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetState(res, pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))
					m_me->setLCID(*pLCID);
			}
			else
				SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, SetState(res, pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))
			}

	if (pActive)
	{
		prevActive = m_me->getActive();

		if ((*pActive) && (!prevActive))
		{
			// reset send values on activation
			m_me->resetSendValues();
		}

		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
		SOCmnList<SODaSTransaction> tl = srv->getTransactionList();
		tl.lock();
		m_me->setActive((*pActive) != 0);
		m_me->onChangeStateItems();

		if ((*pActive) && (!prevActive))
		{
			// send current values if the group has been activated
			SOCmnPointer<SODaSTransaction> dest;
			m_me->copyCyclicTransaction(0, SODaSRequest::asyncInitNotify, 0, SODaSTransaction::allConnections, (SODaSTransaction**)dest);

			if ((dest->getRequestCount() > 0) && (srv.isNotNull()))
			{
				dest->setTransactionId(0);  // no cyclic transaction but id is 0
				dest->start(srv);
				srv->addTransaction(dest);
			}
		}

		tl.unlock();
	}

	if (pRequestedUpdateRate)
	{
		m_me->setUpdateRate(*pRequestedUpdateRate);
	}

	if (phClientGroup)
	{
		m_me->setClientHandle(*phClientGroup);
	}

	if (pTimeBias)
	{
		m_me->setTimeBias(*pTimeBias);
	}

	if (pPercentDeadband)
	{
		m_me->setDeadBand(*pPercentDeadband);
	}

	// check update rate
	setUpdateRate = m_me->getUpdateRate();

	if (pRequestedUpdateRate)
	{
		if (setUpdateRate != *pRequestedUpdateRate)
		{
			res = OPC_S_UNSUPPORTEDRATE;
		}
	}

	if (pRevisedUpdateRate)
	{
		*pRevisedUpdateRate = setUpdateRate;
	}

	SOSRV_END_INTERFACE_FUNCTION(SetState(res, pRequestedUpdateRate, pRevisedUpdateRate, pActive, pTimeBias, pPercentDeadband, pLCID, phClientGroup))
} // SetState


//-----------------------------------------------------------------------------
// SetName (interface IOPCGroupStateMgt)
//
// - sets the name of the group
//
// returns:
//      S_OK                - set name
//      E_FAIL              - group allready removed
//      E_INVALIDARG        - no valid name or parameter
//      OPC_E_DUPLICATENAME - no unique name
//
STDMETHODIMP SODaSComGroup::SetName(
	IN LPCWSTR szName)  // name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt::SetName, SetName(szName))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM_PTR(szName)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(!(m_me->isPublic()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(SetName(res, szName))
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szName));
	res = m_me->setName(name);
	SOSRV_END_INTERFACE_FUNCTION(SetName(res, szName))
} // SetName


//-----------------------------------------------------------------------------
// CloneGroup (interface IOPCGroupStateMgt)
//
// - creates a copy of the group
//
// returns:
//      S_OK                - created new group
//      E_FAIL              - group allready removed
//      E_INVALIDARG        - group name not valid
//      OPC_E_DUPLICATENAME - group name not unique
//
STDMETHODIMP SODaSComGroup::CloneGroup(
	IN LPCWSTR szName,      // name
	IN REFIID riid,         // interface id
	OUT LPUNKNOWN* ppUnk)   // interface pointer
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt::CloneGroup, CloneGroup(szName, riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(CloneGroup(res, szName, riid, ppUnk))
	USES_CONVERSION;
	SOCmnString name(OLE2CT(szName));
	SOCmnPointer<SODaSGroup> grp;
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
	// create tree group object
	res = m_me->clone(srv, name, (SODaSGroup**)grp);

	if (SUCCEEDED(res))
	{
		// create COM group object
		if (m_server)
		{
			HRESULT resQI;

			if (FAILED(resQI = m_server->createOPCGroup(grp, riid, ppUnk)))
			{
				res = resQI;
			}

#ifdef SOFEATURE_WATCH
			else if (srv->getCollectChanges())
			{
				SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
				entry->watchAddObject(srv, grp);
			}

#endif
		}
		else
		{
			res = E_FAIL;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(CloneGroup(res, szName, riid, ppUnk))
} // CloneGroup


//-----------------------------------------------------------------------------
// IOPCGroupStateMgt2                                                         |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetKeepAlive (interface IOPCGroupStateMgt2)
//
// - sets the keep alive time of the group
//
// returns:
//      S_OK                  - set time
//      E_FAIL                - group allready removed
//      E_INVALIDARG          - NULL pointer parameter
//    OPC_S_UNSUPPORTEDRATE - the requested keep alive time is not supported
//
STDMETHODIMP SODaSComGroup::SetKeepAlive(
	IN DWORD keepAliveTime,           // requested keep alive time in ms
	OUT DWORD* pRevisedKeepAliveTime) // revised keep alive time in ms
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt2::SetKeepAlive, SetKeepAlive(keepAliveTime, pRevisedKeepAliveTime))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pRevisedKeepAliveTime)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(SetKeepAlive(res, keepAliveTime, pRevisedKeepAliveTime))
	DWORD revKAT;
	// set time
	m_me->setKeepAliveTime(keepAliveTime);
	// check time
	revKAT = m_me->getKeepAliveTime();

	if (revKAT != keepAliveTime)
	{
		res = OPC_S_UNSUPPORTEDRATE;
	}

	if (pRevisedKeepAliveTime)
	{
		*pRevisedKeepAliveTime = revKAT;
	}

	SOSRV_END_INTERFACE_FUNCTION(SetKeepAlive(res, keepAliveTime, pRevisedKeepAliveTime))
} // SetKeepAlive


//-----------------------------------------------------------------------------
// GetKeepAlive (interface IOPCGroupStateMgt2)
//
// - gets the keep alive time of the group
//
// returns:
//      S_OK                  - provided time
//      E_FAIL                - group allready removed
//      E_INVALIDARG          - NULL pointer parameter
//
STDMETHODIMP SODaSComGroup::GetKeepAlive(
	OUT DWORD* pKeepAliveTime) // keep alive time of the group
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCGroupStateMgt2::GetKeepAlive, GetKeepAlive(pKeepAliveTime))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pKeepAliveTime)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(GetKeepAlive(res, pKeepAliveTime))
	*pKeepAliveTime = m_me->getKeepAliveTime();
	SOSRV_END_INTERFACE_FUNCTION(GetKeepAlive(res, pKeepAliveTime))
} // GetKeepAlive



//-----------------------------------------------------------------------------
// IOPCPublicGroupStateMgt                                                    |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetState (interface IOPCPublicGroupStateMgt)
//
// - checks if the group is public or not
//
// returns:
//      S_OK          - return if is public group
//      E_INVALIDARG  - NULL pointer parameter
//
STDMETHODIMP SODaSComGroup::GetState(
	OUT BOOL* pPublic)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCPublicGroupStateMgt::GetState, GetState(pPublic))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pPublic)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(GetState(res, pPublic))
	*pPublic = m_me->isPublicGroup();
	SOSRV_END_INTERFACE_FUNCTION(GetState(res, pPublic))
} // GetState


//-----------------------------------------------------------------------------
// MoveToPublic (interface IOPCPublicGroupStateMgt)
//
// - converts a private group into a public
//
// returns:
//      S_OK                - group is now a public group
//      OPC_E_DUPLICATENAME - group name is no unique public group name
//
STDMETHODIMP SODaSComGroup::MoveToPublic(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCPublicGroupStateMgt::MoveToPublic, MoveToPublic())
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSServer> pgserver = engine->getPublicGroups();

	// check if the group was already removed
	if ((m_me->isRemoved()) || (!pgserver))
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, MoveToPublic(res))
		// copy group to public groups server
		SOCmnPointer<SODaSGroup> clone;

	res = m_me->clone(pgserver, m_me->getName(), (SODaSGroup**)clone);

	if (SUCCEEDED(res))
	{
		// move group to public
		// -> creates a public template with server handle of the group
		clone->m_serverHandle = m_me->m_serverHandle;
#ifdef SOFEATURE_WATCH

		if (pgserver->getCollectChanges())
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchAddObject(pgserver, clone);
		}

#endif
		m_me->m_publicTemplate = (SODaSGroup*)clone;
#ifdef SOFEATURE_WATCH

		if (m_me->getCollectChanges())
		{
			SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
			SOCmnString buffer;
			SOSrvWatch::attributeChange(m_me, SOWATCH_ATTRIBUTE_PUBLIC_T, conv->publicState2string(buffer, m_me->isPublic(), m_me->isPublicGroup()));
		}

#endif
		// release server reference for instance of public group COM object
		SOSrvComObject<SODaSComGroup> *me = (SOSrvComObject<SODaSComGroup> *)m_me->m_me;
		me->internalRelease();
	}

	SOSRV_END_INTERFACE_FUNCTION(MoveToPublic(res))
} // MoveToPublic




//-----------------------------------------------------------------------------
// IOPCItemMgt                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// itemArrayCheckAndAlloc
//
// - checks pointer and allocates error return buffer for item management
//   routines
//
// returns:
//      S_OK          - created all items without error
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
HRESULT SODaSComGroup::itemArrayCheckAndAlloc(
	IN DWORD dwNumItems,    // number of items
	IN void* p1,            // pointer which should not be NULL
	IN void* p2,            // pointer which should not be NULL
	OUT void** p3,          // pointer pointer which should not be NULL
	OUT HRESULT** ppErrors) // errors
{
	// check pointer parameter
	if ((!p3) || (!ppErrors))
	{
		if (p3)
		{
			*p3 = NULL;
		}

		if (ppErrors)
		{
			*ppErrors = NULL;
		}

		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("IOPCItemMgt::xxx Invalid parameter")));
		return E_INVALIDARG;
	}

	if ((!p1) || (!p2))
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("IOPCItemMgt::xxx Invalid parameter")));
		return E_INVALIDARG;
	}

	if (dwNumItems == 0)
	{
		// work on no items -> invalid argument
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("IOPCItemMgt::xxx Passed 0 item handles")));
		return E_INVALIDARG;
	}

	if (m_me->isRemoved())  // group already removed ?
	{
		return E_FAIL;
	}

	// create return data
	*ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems * sizeof(HRESULT));

	if (*ppErrors == NULL)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
} // itemArrayCheckAndAlloc


//-----------------------------------------------------------------------------
// AddItems (interface IOPCItemMgt)
//
// - add items to the group
//
// returns:
//      S_OK          - created all items without error
//      S_FALSE       - created some, but not all items without error
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//      OPC_E_PUBLIC  - can't add items to public group
//
STDMETHODIMP SODaSComGroup::AddItems(
	IN DWORD dwNumItems,                // number of items
	IN OPCITEMDEF* pItemArray,          // item input parameter
	OUT OPCITEMRESULT** ppAddResults,   // item result parameter
	OUT HRESULT** ppErrors)             // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::AddItems, AddItems(dwNumItems, pItemArray, ppAddResults, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppAddResults)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(pItemArray != NULL)
	SOSRV_END_PARAMS_CHECK(AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
	USES_CONVERSION;
	SODaSItem* item;
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!srv)
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("No server object for AddItems")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
	}

	SOCmnSingleLock<SOCmnSync> lock(srv->getWorkThreadLock());

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, pItemArray, pItemArray, (void**)ppAddResults, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
		if (m_me->isPublic())   // public group
		{
			// can't add items to public group
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("Can't add items to public group")));
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(OPC_E_PUBLIC, AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
		}

	// create return data
	*ppAddResults = (OPCITEMRESULT*) CoTaskMemAlloc(dwNumItems * sizeof(OPCITEMRESULT));

	if (*ppAddResults == NULL)
	{
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
	}

	// allocate block for entries in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(m_me->getLeafList());
		DWORD freeListEntries;
		// get number of available allocated list entries
		freeListEntries = itemList.getAvailableAllocatedEntries();

		if (freeListEntries < dwNumItems)
		{
			itemList.allocateBlock(dwNumItems - freeListEntries);
		}

		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> ctac = m_me->getCyclicTransaction();

		if (!!ctac)
		{
			if (ctac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(ctac->getRequestList());
				freeListEntries = rList.getAvailableAllocatedEntries();

				if (freeListEntries < dwNumItems)
				{
					rList.allocateBlock(dwNumItems - freeListEntries);
				}
			}
		}
	}
	// generate initial value read
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnList<SODaSRequest> reqList;

	if (m_me->m_active)
	{
		// create a asynchronous read transaction with the connection id of the cyclic notification transaction
		tac = creator->createTransaction(m_me, SODaSTransaction::refresh, SODaSTransaction::allConnections, 0);
		reqList = tac->getRequestList();
	}

	// call method to prepare for add items for dynamic namespace
	DWORD preValidateItemsCookie = 0;
	SOCmnList<SODaSItemData> itemDataList;

	if (m_me->hasPrePostValidateItems())
	{
		SODaSItemData* pItemData;
		itemDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

		for (DWORD i = 0; i < dwNumItems; i++)
		{
			SOCMN_ALLOCATE_OBJECT(pItemData, SODaSItemData);
			pItemData->m_itemID = pItemArray[i].szItemID;
			pItemData->m_reqDatatype = pItemArray[i].vtRequestedDataType;
			pItemData->m_itemPath = pItemArray[i].szAccessPath;
			itemDataList.add(pItemData);
		}

		preValidateItemsCookie = m_me->preValidateItems(TRUE, &itemDataList);
	}

	// add every item
	for (DWORD i = 0; i < dwNumItems; i++)
	{
		(*ppErrors)[i] = m_me->addItem(SOCmnString(pItemArray[i].szItemID),
									   SOCmnString(pItemArray[i].szAccessPath), (pItemArray[i].bActive != 0),
									   pItemArray[i].hClient, pItemArray[i].vtRequestedDataType,
									   &item, &((*ppAddResults)[i].vtCanonicalDataType),
									   &((*ppAddResults)[i].dwAccessRights));
		(*ppAddResults)[i].wReserved = 0;
		(*ppAddResults)[i].dwBlobSize = 0;
		(*ppAddResults)[i].pBlob = NULL;

		if (SUCCEEDED((*ppErrors)[i]))
		{
			(*ppAddResults)[i].hServer = item->getServerHandle();

			if ((m_me->m_active) && (pItemArray[i].bActive != 0) && ((*ppAddResults)[i].dwAccessRights & OPC_READABLE))
			{
				// active item
				// create cache request to send the current value to the client
				SOCmnPointer<SODaSItemTag> itemTag = item->getItemTag();
				SOCmnPointer<SODaSRequest> req;
				req = creator->createRequest(srv, item, itemTag, SODaSRequest::read, SODaSRequest::asyncInitNotify);

				if (req.isNotNull())
				{
					req->setItem(item);
					req->setCompletedEvent(srv->getWorkEvent());
					req->setIsRefresh();
					reqList.add(req);
				}
			}

			item->release();
		}
		else
		{
			(*ppAddResults)[i].hServer = 0;
			res = S_FALSE;
		}
	}

	if (preValidateItemsCookie != 0)
	{
		m_me->postValidateItems(preValidateItemsCookie, &itemDataList);
	}

	itemDataList.destroy();
	m_me->onAddItems();

	if (m_me->m_active)
	{
		if (!reqList.isEmpty())
		{
			// start the initial read transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}

	// free unused blocks in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(m_me->getLeafList());
		itemList.freeUnusedBlocks();
		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> ctac = m_me->getCyclicTransaction();

		if (!!ctac)
		{
			if (ctac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(ctac->getRequestList());
				rList.freeUnusedBlocks();
			}
		}
	}
	SOSRV_END_INTERFACE_FUNCTION(AddItems(res, dwNumItems, pItemArray, ppAddResults, ppErrors))
} // AddItems


//-----------------------------------------------------------------------------
// ValidateItems (interface IOPCItemMgt)
//
// - validate items
//
// returns:
//      S_OK          - validated all items without error
//      S_FALSE       - validated some, but not all items without error
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::ValidateItems(
	IN DWORD dwNumItems,                    // number of items
	IN OPCITEMDEF* pItemArray,              // item input parameter
	IN BOOL bBlobUpdate ,
	OUT OPCITEMRESULT** ppValidationResults,// item result parameter
	OUT HRESULT** ppErrors)                 // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::ValidateItems, ValidateItems(dwNumItems, pItemArray, bBlobUpdate, ppValidationResults, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppValidationResults)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(pItemArray != NULL)
	SOSRV_END_PARAMS_CHECK(ValidateItems(res, dwNumItems, pItemArray, bBlobUpdate, ppValidationResults, ppErrors))
	USES_CONVERSION;

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, pItemArray, pItemArray, (void**)ppValidationResults, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(ValidateItems(res, dwNumItems, pItemArray, bBlobUpdate, ppValidationResults, ppErrors))
		// create return data
		*ppValidationResults = (OPCITEMRESULT*) CoTaskMemAlloc(dwNumItems * sizeof(OPCITEMRESULT));

	if (*ppValidationResults == NULL)
	{
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, ValidateItems(res, dwNumItems, pItemArray, bBlobUpdate, ppValidationResults, ppErrors))
	}

	// call method to prepare for add items for dynamic namespace
	DWORD preValidateItemsCookie = 0;
	SOCmnList<SODaSItemData> itemDataList;

	if (m_me->hasPrePostValidateItems())
	{
		SODaSItemData* pItemData;
		itemDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);

		for (DWORD i = 0; i < dwNumItems; i++)
		{
			SOCMN_ALLOCATE_OBJECT(pItemData, SODaSItemData);
			pItemData->m_itemID = pItemArray[i].szItemID;
			pItemData->m_reqDatatype = pItemArray[i].vtRequestedDataType;
			pItemData->m_itemPath = pItemArray[i].szAccessPath;
			itemDataList.add(pItemData);
		}

		preValidateItemsCookie = m_me->preValidateItems(FALSE, &itemDataList);
	}

	// validate every item
	for (DWORD i = 0; i < dwNumItems; i++)
	{
		(*ppErrors)[i] = m_me->validateItem(SOCmnString(pItemArray[i].szItemID),
											SOCmnString(pItemArray[i].szAccessPath), pItemArray[i].vtRequestedDataType,
											&((*ppValidationResults)[i].vtCanonicalDataType),
											&((*ppValidationResults)[i].dwAccessRights));
		(*ppValidationResults)[i].hServer = NULL;
		(*ppValidationResults)[i].wReserved = 0;
		(*ppValidationResults)[i].dwBlobSize = 0;
		(*ppValidationResults)[i].pBlob = NULL;

		if (FAILED((*ppErrors)[i]))
		{
			res = S_FALSE;
		}
	}

	if (preValidateItemsCookie != 0)
	{
		m_me->postValidateItems(preValidateItemsCookie, &itemDataList);
	}

	itemDataList.destroy();
	SOSRV_END_INTERFACE_FUNCTION(ValidateItems(res, dwNumItems, pItemArray, bBlobUpdate, ppValidationResults, ppErrors))
} // ValidateItems


//-----------------------------------------------------------------------------
// RemoveItems (interface IOPCItemMgt)
//
// - remove items
//
// returns:
//      S_OK          - removed all items without error
//      S_FALSE       - some item handles were invalid
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::RemoveItems(
	IN DWORD dwNumItems,    // number of items
	IN OPCHANDLE* phServer, // items
	OUT HRESULT** ppErrors) // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::RemoveItems, RemoveItems(dwNumItems, phServer, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(RemoveItems(res, dwNumItems, phServer, ppErrors))
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!srv)
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("No server object for RemoveItems")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, RemoveItems(res, dwNumItems, phServer, ppErrors))
	}

	SOCmnSingleLock<SOCmnSync> lock(srv->getWorkThreadLock());

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(RemoveItems(res, dwNumItems, phServer, ppErrors))
		if (m_me->isPublic())   // public group
		{
			// can't remove items from public group
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("Can't remove items from public group")));
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(OPC_E_PUBLIC, RemoveItems(res, dwNumItems, phServer, ppErrors))
		}

	// remove every item
	for (DWORD i = 0; i < dwNumItems; i++)
	{
		(*ppErrors)[i] = m_me->removeItem(phServer[i]);

		if (FAILED((*ppErrors)[i]))
		{
			res = S_FALSE;
		}
	}

	m_me->onRemoveItems();

	if (m_me->getUpdateRate() == 0)
	{
		// recalc notify all changes
		m_me->calcNotifyAllChanges();
	}

	// free unused blocks in item list of the group and request list of the cyclic transaction
	{
		SOCmnList<SOCmnElement> itemList(m_me->getLeafList());
		itemList.freeUnusedBlocks();
		itemList.autoUnlock();
		SOCmnPointer<SODaSTransaction> tac = m_me->getCyclicTransaction();

		if (!!tac)
		{
			if (tac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> rList(tac->getRequestList());
				rList.freeUnusedBlocks();
			}
		}
	}
	SOSRV_END_INTERFACE_FUNCTION(RemoveItems(res, dwNumItems, phServer, ppErrors))
} // RemoveItems


//-----------------------------------------------------------------------------
// SetActiveState (interface IOPCItemMgt)
//
// - set activation state of the items
//
// returns:
//      S_OK          - set activation state of all items without error
//      S_FALSE       - some item handles were invalid
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::SetActiveState(
	IN DWORD dwNumItems,    // number of items
	IN OPCHANDLE* phServer, // items
	IN BOOL bActive,        // activation state
	OUT HRESULT** ppErrors) // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::SetActiveState, SetActiveState(dwNumItems, phServer, bActive, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(SetActiveState(res, dwNumItems, phServer, bActive, ppErrors))
	SOCmnPointer<SODaSItem> item;

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(SetActiveState(res, dwNumItems, phServer, bActive, ppErrors))
		// generate initial value read
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();

	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnList<SODaSRequest> reqList;

	if ((m_me->m_active) && (bActive))
	{
		// create a asynchronous read transaction with the connection id of the cyclic notification transaction
		tac = creator->createTransaction(m_me, SODaSTransaction::refresh, SODaSTransaction::allConnections, 0);
		reqList = tac->getRequestList();
	}

	// set activation state of every item
	for (DWORD i = 0; i < dwNumItems; i++)
	{
		// get item
		SOCMN_TRY
		{
			item = m_me->getItemByHandle(phServer[i]);
		} SOCMN_CATCH(...)
		{
			item = NULL;
		}

		if ((SODaSItem*)item)
		{
			if (item->setActive(bActive != 0))
			{
				(*ppErrors)[i] = S_OK;

				if ((m_me->m_active) && (bActive))
				{
					// create cache request to send the current value to the client
					SOCmnPointer<SODaSItemTag> itemTag = item->getItemTag();

					if (itemTag->getAccessRights(srv) & OPC_READABLE)
					{
						SOCmnPointer<SODaSRequest> req;
						req = creator->createRequest(srv, item, itemTag, SODaSRequest::read, SODaSRequest::asyncInitNotify);

						if (req.isNotNull())
						{
							req->setItem(item);
							req->setCompletedEvent(srv->getWorkEvent());
							req->setIsRefresh();
							reqList.add(req);
						}
					}
				}
			}
			else
			{
				(*ppErrors)[i] = E_FAIL;
				res = S_FALSE;
			}
		}
		else
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
	}

	m_me->onChangeStateItems();

	if ((m_me->m_active) && (bActive))
	{
		if (!reqList.isEmpty())
		{
			// start the initial read transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(SetActiveState(res, dwNumItems, phServer, bActive, ppErrors))
} // SetActiveState


//-----------------------------------------------------------------------------
// SetClientHandles (interface IOPCItemMgt)
//
// - set client handle of the items
//
// returns:
//      S_OK          - set client handle of all items without error
//      S_FALSE       - some item handles were invalid
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::SetClientHandles(
	IN DWORD dwNumItems,    // number of items
	IN OPCHANDLE* phServer, // items
	IN OPCHANDLE* phClient, // client handles
	OUT HRESULT** ppErrors) // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::SetClientHandles, SetClientHandles(dwNumItems, phServer, phClient, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM(phClient != NULL)
	SOSRV_END_PARAMS_CHECK(SetClientHandles(res, dwNumItems, phServer, phClient, ppErrors))
	SOCmnPointer<SODaSItem> item;

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phClient, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(SetClientHandles(res, dwNumItems, phServer, phClient, ppErrors))

		// remove every item
		for (DWORD i = 0; i < dwNumItems; i++)
		{
			// get item
			SOCMN_TRY
			{
				item = m_me->getItemByHandle(phServer[i]);
			} SOCMN_CATCH(...)
			{
				item = NULL;
			}

			if ((SODaSItem*)item)
			{
				item->setClientHandle(phClient[i]);
				(*ppErrors)[i] = S_OK;
			}
			else
			{
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				res = S_FALSE;
			}
		}

	SOSRV_END_INTERFACE_FUNCTION(SetClientHandles(res, dwNumItems, phServer, phClient, ppErrors))
} // SetClientHandles


//-----------------------------------------------------------------------------
// SetDatatypes (interface IOPCItemMgt)
//
// - set client handle of the items
//
// returns:
//      S_OK          - set client handle of all items without error
//      S_FALSE       - some item handles were invalid
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::SetDatatypes(
	IN DWORD dwNumItems,    // number of items
	IN OPCHANDLE* phServer, // items
	IN VARTYPE* pRequestedDatatypes,
	OUT HRESULT** ppErrors) // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::SetDatatypes, SetDatatypes(dwNumItems, phServer, pRequestedDatatypes, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM(pRequestedDatatypes != NULL)
	SOSRV_END_PARAMS_CHECK(SetDatatypes(res, dwNumItems, phServer, pRequestedDatatypes, ppErrors))
	SOCmnPointer<SODaSItem> item;

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, pRequestedDatatypes, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(SetDatatypes(res, dwNumItems, phServer, pRequestedDatatypes, ppErrors))

		// remove every item
		for (DWORD i = 0; i < dwNumItems; i++)
		{
			// get item
			SOCMN_TRY
			{
				item = m_me->getItemByHandle(phServer[i]);
			} SOCMN_CATCH(...)
			{
				item = NULL;
			}

			if ((SODaSItem*)item)
			{
				(*ppErrors)[i] = item->setReqDatatype(pRequestedDatatypes[i]);
			}
			else
			{
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			}

			if (FAILED((*ppErrors)[i]))
			{
				res = S_FALSE;
			}
		}

	SOSRV_END_INTERFACE_FUNCTION(SetDatatypes(res, dwNumItems, phServer, pRequestedDatatypes, ppErrors))
} // SetDatatypes


//-----------------------------------------------------------------------------
// CreateEnumerator (interface IOPCItemMgt)
//
// - create item enumerator
//
// returns:
//      S_OK          - set client handle of all items without error
//      S_FALSE       - some item handles were invalid
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//
STDMETHODIMP SODaSComGroup::CreateEnumerator(
	IN REFIID riid,         // interface id
	OUT LPUNKNOWN* ppUnk)   // interface pointer
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemMgt::CreateEnumerator, CreateEnumerator(riid, ppUnk))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppUnk)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK((riid == IID_IEnumOPCItemAttributes), E_NOINTERFACE)
	SOSRV_END_PARAMS_CHECK(CreateEnumerator(res, riid, ppUnk))
	m_me->preCreateItemEnumerator();
	SOSrvComObject<SODaSComEnumItem> *attrList;
	SOCmnList<SOCmnElement> list(m_me->getLeafList());
	SODaSItem* item;
	SOCmnListPosition pos;
	SOCMN_ALLOCATE_OBJECT(attrList, SOSrvComObject<SODaSComEnumItem>)
	// for all items of the group
	pos = list.getStartPosition();

	while (pos)
	{
		item = (SODaSItem*)list.getNext(pos);
		item->addRef();
		attrList->m_list.AddTail(item);
	}

	list.autoUnlock();

	// reset list
	if (attrList->m_list.IsEmpty())
	{
		res = S_FALSE;      // no list entry -> return OK but nothing found
		SOCMN_FREE_OBJECT(attrList) // free list enumerator
		*ppUnk = NULL;
	}
	else
	{
		attrList->Reset();
		res = attrList->QueryInterface(riid, (void**)ppUnk);
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(CreateEnumerator(res, riid, ppUnk), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // CreateEnumerator



//-----------------------------------------------------------------------------
// IOPCItemDeadbandMgt                                                        |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetItemDeadband (interface IOPCItemDeadbandMgt)
//
// - Set item deadband values
//
// returns:
//    S_OK                       - The function was successful.
//    S_FALSE                    - The function was partially successful.
//                                 See the ppErrors to determine what happened
//    E_INVALIDARG               - An argument to the function was invalid.
//    OPC_E_DEADBANDNOTSUPPORTED - The server does not support deadband.
//    E_FAIL                     - The function was unsuccessful.
//
STDMETHODIMP SODaSComGroup::SetItemDeadband(
	IN DWORD count,             // number of items
	IN OPCHANDLE* phServer,     // items server handles
	IN FLOAT* pPercentDeadband, // items deadband settings
	OUT HRESULT** ppErrors)     // errors per item
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemDeadbandMgt::SetItemDeadband, SetItemDeadband(count, phServer, pPercentDeadband, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM(pPercentDeadband != NULL)
	SOSRV_END_PARAMS_CHECK(SetItemDeadband(res, count, phServer, pPercentDeadband, ppErrors))

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, pPercentDeadband, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(SetItemDeadband(res, count, phServer, pPercentDeadband, ppErrors))
		SOCmnPointer<SODaSItem> item;

	DWORD i;

	for (i = 0; i < count; i++)
	{
		if ((pPercentDeadband[i] >= 0.0) && (pPercentDeadband[i] <= 100.0))
		{
			// get item
			SOCMN_TRY
			{
				item = m_me->getItemByHandle(phServer[i]);
			} SOCMN_CATCH(...)
			{
				item = NULL;
			}

			if (item.isNotNull())
			{
				(*ppErrors)[i] = item->setDeadBand(pPercentDeadband[i]);

				if (FAILED((*ppErrors)[i]))
				{
					res = S_FALSE;
				}
			}
			else
			{
				// invalid item handle
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				res = S_FALSE;
			}
		}
		else
		{
			// invalid deadband value
			(*ppErrors)[i] = E_INVALIDARG;
			res = S_FALSE;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(SetItemDeadband(res, count, phServer, pPercentDeadband, ppErrors))
} // SetItemDeadband


//-----------------------------------------------------------------------------
// GetItemDeadband (interface IOPCItemDeadbandMgt)
//
// - Get item deadband values
//
// returns:
//    S_OK                       - The function was successful.
//    S_FALSE                    - The function was partially successful.
//                                 See the ppErrors to determine what happened
//    E_INVALIDARG               - An argument to the function was invalid.
//    OPC_E_DEADBANDNOTSUPPORTED - The server does not support deadband.
//    E_FAIL                     - The function was unsuccessful.
//
STDMETHODIMP SODaSComGroup::GetItemDeadband(
	IN DWORD count,                // number of items
	IN OPCHANDLE* phServer,        // items server handles
	OUT FLOAT** ppPercentDeadband, // items deadband settings
	OUT HRESULT** ppErrors)        // errors per item
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemDeadbandMgt::GetItemDeadband, GetItemDeadband(count, phServer, ppPercentDeadband, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_OUT_PARAM_PTR(ppPercentDeadband)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(GetItemDeadband(res, count, phServer, ppPercentDeadband, ppErrors))

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(GetItemDeadband(res, count, phServer, ppPercentDeadband, ppErrors))
		// create return data
		*ppPercentDeadband = (FLOAT*) CoTaskMemAlloc(count * sizeof(FLOAT));

	if (*ppPercentDeadband == NULL)
	{
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, GetItemDeadband(res, count, phServer, ppPercentDeadband, ppErrors))
	}

	SOCmnPointer<SODaSItem> item;
	DWORD i;

	for (i = 0; i < count; i++)
	{
		// get item
		SOCMN_TRY
		{
			item = m_me->getItemByHandle(phServer[i]);
		} SOCMN_CATCH(...)
		{
			item = NULL;
		}

		if (item.isNotNull())
		{
			(*ppErrors)[i] = item->getDeadBand((*ppPercentDeadband)[i]);

			if (FAILED((*ppErrors)[i]))
			{
				(*ppPercentDeadband)[i] = 0.0;
				res = S_FALSE;
			}
		}
		else
		{
			// invalid item handle
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION_TL(GetItemDeadband(res, count, phServer, ppPercentDeadband, ppErrors), (SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR))
} // GetItemDeadband


//-----------------------------------------------------------------------------
// ClearItemDeadband (interface IOPCItemDeadbandMgt)
//
// - Clear item deadband values
//
// returns:
//    S_OK                       - The function was successful.
//    S_FALSE                    - The function was partially successful.
//                                 See the ppErrors to determine what happened
//    E_INVALIDARG               - An argument to the function was invalid.
//    OPC_E_DEADBANDNOTSUPPORTED - The server does not support deadband.
//    E_FAIL                     - The function was unsuccessful.
//
STDMETHODIMP SODaSComGroup::ClearItemDeadband(
	IN DWORD count,             // number of items
	IN OPCHANDLE* phServer,     // items server handles
	OUT HRESULT** ppErrors)     // errors per item
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCItemDeadbandMgt::ClearItemDeadband, ClearItemDeadband(count, phServer, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(ClearItemDeadband(res, count, phServer, ppErrors))

	// item management standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(ClearItemDeadband(res, count, phServer, ppErrors))
		SOCmnPointer<SODaSItem> item;

	DWORD i;

	for (i = 0; i < count; i++)
	{
		// get item
		SOCMN_TRY
		{
			item = m_me->getItemByHandle(phServer[i]);
		} SOCMN_CATCH(...)
		{
			item = NULL;
		}

		if (item.isNotNull())
		{
			(*ppErrors)[i] = item->clearDeadBand();

			if (FAILED((*ppErrors)[i]))
			{
				res = S_FALSE;
			}
		}
		else
		{
			// invalid item handle
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(ClearItemDeadband(res, count, phServer, ppErrors))
} // ClearItemDeadband



//-----------------------------------------------------------------------------
// IOPCSyncIO                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Read (interface IOPCSyncIO)
//
// - Synchronous read of item values
//
// returns:
//      S_OK          - read values of all items without error
//      S_FALSE       - at least one read failed
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Read(
	IN OPCDATASOURCE dwSource,      // data source: cache or device
	IN DWORD dwNumItems,            // number of items
	IN OPCHANDLE* phServer,         // items
	OUT OPCITEMSTATE** ppItemValues,// values
	OUT HRESULT** ppErrors)         // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSyncIO::Read, Read(dwSource, dwNumItems, phServer, ppItemValues, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_OUT_PARAM_PTR(ppItemValues)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM((dwSource == OPC_DS_DEVICE) || (dwSource == OPC_DS_CACHE))
	SOSRV_END_PARAMS_CHECK(Read(res, dwSource, dwNumItems, phServer, ppItemValues, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phServer, (void**)ppItemValues, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Read(res, dwSource, dwNumItems, phServer, ppItemValues, ppErrors))
		// create return data
		*ppItemValues = (OPCITEMSTATE*) CoTaskMemAlloc(dwNumItems * sizeof(OPCITEMSTATE));

	if (*ppItemValues == NULL)
	{
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, Read(res, dwSource, dwNumItems, phServer, ppItemValues, ppErrors))
	}

	SOCmnEvents events;
	DWORD reqEventId = events.create();
	res = m_me->createTransaction(TRUE, SODaSTransaction::synchronous, 0, dwSource, SOCmnEventPointer(&events, reqEventId), dwNumItems, phServer, NULL, ppItemValues, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, NULL, ppErrors);

	if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
	{
		// transaction was created with no error
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

		if (!!srv)
		{
			// -> start sync read transaction
			tac->execute(srv, &events, reqEventId, srv->getDuetimeRead());
		}
		else
		{
			res = E_FAIL;
		}
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();

	if (SUCCEEDED(res))
	{
		for (i = 0; i < dwNumItems; i++)
		{
			if (FAILED((*ppErrors)[i]))
			{
				(*ppItemValues)[i].vDataValue.vt = VT_EMPTY;
				res = S_FALSE;
			}
		}
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free item values
		CoTaskMemFree(*ppItemValues);
		*ppItemValues = NULL;
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Read(res, dwSource, dwNumItems, phServer, ppItemValues, ppErrors))
} // Read


//-----------------------------------------------------------------------------
// Write (interface IOPCSyncIO)
//
// - Synchronous write of item values
//
// returns:
//      S_OK          - written values of all items without error
//      S_FALSE       - at least one write failed
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Write(
	IN DWORD dwNumItems,        // number of items
	IN OPCHANDLE* phServer,     // item handles
	IN VARIANT* pItemValues,    // write values
	OUT HRESULT** ppErrors)     // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSyncIO::Write, Write(dwNumItems, phServer, pItemValues, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(pItemValues)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(Write(res, dwNumItems, phServer, pItemValues, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, pItemValues, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Write(res, dwNumItems, phServer, pItemValues, ppErrors))
		SOCmnEvents events;

	DWORD reqEventId = events.create();
	res = m_me->createTransaction(FALSE, SODaSTransaction::synchronous, 0, OPC_DS_DEVICE, SOCmnEventPointer(&events, reqEventId), dwNumItems, phServer, pItemValues, NULL, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, NULL, ppErrors);

	if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
	{
		// transaction was created with no error
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

		if (!!srv)
		{
			// -> start sync write transaction
			tac->execute(srv, &events, reqEventId, srv->getDuetimeWrite());

			for (i = 0; i < dwNumItems; i++)
			{
				if (FAILED((*ppErrors)[i]))
				{
					res = S_FALSE;
				}
			}
		}
		else
		{
			res = E_FAIL;
		}
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();

	// check for errors
	if (FAILED(res))
	{
		// failed return code
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Write(res, dwNumItems, phServer, pItemValues, ppErrors))
} // Write



//-----------------------------------------------------------------------------
// IOPCSyncIO2                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Read (interface IOPCSyncIO2)
//
// - Synchronous read of item values with max age
//
// returns:
//      S_OK          - read values of all items without error
//      S_FALSE       - at least one read failed
//      E_FAIL        - group allready removed
//      E_INVALIDARG  - NULL pointer parameter
//      E_OUTOFMEMORY - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::ReadMaxAge(
	IN DWORD count,              // number of items to read
	IN OPCHANDLE* phServer,      // item server handles
	IN DWORD* pMaxAge,           // max ages of the cache values
	OUT VARIANT** ppValues,      // read values
	OUT WORD** ppQualities,      // read qualities
	OUT FILETIME** ppTimeStamps, // read timestamps
	OUT HRESULT** ppErrors)      // results of read operations
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSyncIO2::ReadMaxAge, ReadMaxAge(count, phServer, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_OUT_PARAM_PTR(ppValues)
	SOSRV_OUT_PARAM_PTR(ppQualities)
	SOSRV_OUT_PARAM_PTR(ppTimeStamps)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM(pMaxAge != NULL)
	SOSRV_END_PARAMS_CHECK(ReadMaxAge(res, count, phServer, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, phServer, (void**)ppValues, ppErrors)))
	{
		SOSRV_RETURN_INTERFACE_FUNCTION(ReadMaxAge(res, count, phServer, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	}

	// create return data
	*ppValues = (VARIANT*) CoTaskMemAlloc(count * sizeof(VARIANT));
	*ppQualities = (WORD*) CoTaskMemAlloc(count * sizeof(WORD));
	*ppTimeStamps = (FILETIME*) CoTaskMemAlloc(count * sizeof(FILETIME));

	if ((*ppValues == NULL) || (*ppQualities == NULL)   || (*ppTimeStamps == NULL))
	{
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;

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

		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, ReadMaxAge(res, count, phServer, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
	}

	SOCmnEvents events;
	DWORD reqEventId = events.create();
	res = m_me->createTransaction(TRUE, SODaSTransaction::synchronous, 0, 0, SOCmnEventPointer(&events, reqEventId), count, phServer, NULL, NULL, pMaxAge, ppValues, ppQualities, ppTimeStamps, NULL, (SODaSTransaction**)tac, NULL, ppErrors);

	if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
	{
		// transaction was created with no error
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

		if (!!srv)
		{
			// -> start sync read transaction
			tac->execute(srv, &events, reqEventId, srv->getDuetimeRead());
		}
		else
		{
			res = E_FAIL;
		}
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();

	if (SUCCEEDED(res))
	{
		for (i = 0; i < count; i++)
		{
			if (FAILED((*ppErrors)[i]))
			{
				(*ppValues)[i].vt = VT_EMPTY;
				res = S_FALSE;
			}
		}
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free allocated memory
		CoTaskMemFree(*ppValues);
		*ppValues = NULL;
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
		CoTaskMemFree(*ppQualities);
		*ppQualities = NULL;
		CoTaskMemFree(*ppTimeStamps);
		*ppTimeStamps = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(ReadMaxAge(res, count, phServer, pMaxAge, ppValues, ppQualities, ppTimeStamps, ppErrors))
} // ReadMaxAge


//-----------------------------------------------------------------------------
// WriteVQT (interface IOPCSyncIO2)
//
// - Synchronous write of item values, qualities and timestamps
//
// returns:
//      S_OK               - written values of all items without error
//      S_FALSE            - at least one write failed
//    OPC_E_NOTSUPPORTED - unsupported combination of value, quality and timestamp
//      E_FAIL             - group allready removed
//      E_INVALIDARG       - NULL pointer parameter
//      E_OUTOFMEMORY      - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::WriteVQT(
	IN DWORD count,          // number of items
	IN OPCHANDLE* phServer,  // item server handles
	IN OPCITEMVQT* pItemVQT, // write values, qualities and timestamps
	OUT HRESULT** ppErrors)  // results of write operations
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCSyncIO2::WriteVQT, WriteVQT(count, phServer, pItemVQT, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(pItemVQT)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_END_PARAMS_CHECK(WriteVQT(res, count, phServer, pItemVQT, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, pItemVQT, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(WriteVQT(res, count, phServer, pItemVQT, ppErrors))
		SOCmnEvents events;

	DWORD reqEventId = events.create();
	res = m_me->createTransaction(FALSE, SODaSTransaction::synchronous, 0, OPC_DS_DEVICE, SOCmnEventPointer(&events, reqEventId), count, phServer, NULL, NULL, NULL, NULL, NULL, NULL, pItemVQT, (SODaSTransaction**)tac, NULL, ppErrors);

	if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
	{
		// transaction was created with no error
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

		if (!!srv)
		{
			// -> start sync write transaction
			tac->execute(srv, &events, reqEventId, srv->getDuetimeWrite());

			for (i = 0; i < count; i++)
			{
				if (FAILED((*ppErrors)[i]))
				{
					res = S_FALSE;
				}
			}
		}
		else
		{
			res = E_FAIL;
		}
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();

	// check for errors
	if (FAILED(res))
	{
		// failed return code
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(WriteVQT(res, count, phServer, pItemVQT, ppErrors))
} // WriteVQT


//-----------------------------------------------------------------------------
// IOPCAsyncIO                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Read (interface IOPCAsyncIO)
//
// - Asynchronous read of item values
//
// returns:
//      S_OK                   - reads of all items initialised without error
//      S_FALSE                - at least one read initialisation failed
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Read(
	IN DWORD dwConnection,      // connection id
	IN OPCDATASOURCE dwSource,  // data source: cache or device
	IN DWORD dwNumItems,        // number of items
	IN OPCHANDLE* phServer,     // item handles
	OUT DWORD* pTransactionID,  // transaction id (server given)
	OUT HRESULT** ppErrors)     // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO::Read, Read(dwConnection, dwSource, dwNumItems, phServer, pTransactionID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pTransactionID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(phServer)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_IN_PARAM((dwSource == OPC_DS_DEVICE) || (dwSource == OPC_DS_CACHE))
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_DATAOBJECT, dwConnection, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Read(res, dwConnection, dwSource, dwNumItems, phServer, pTransactionID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Read(res, dwConnection, dwSource, dwNumItems, phServer, pTransactionID, ppErrors))
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(TRUE, SODaSTransaction::dataObject, dwConnection, dwSource, srv->getWorkEvent(), dwNumItems, phServer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, pTransactionID, ppErrors);

		if ((S_OK == res) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			tac->start(srv);
			srv->addTransaction(tac);
		}
		else
		{
			// transaction was created with an error
			if (SUCCEEDED(res))
			{
				// result was S_FALSE (invalid server handle)
				// free transaction
				*pTransactionID = 0;
				tac.release();  // free transaction
			}
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// failed return code
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Read(res, dwConnection, dwSource, dwNumItems, phServer, pTransactionID, ppErrors))
} // Read


//-----------------------------------------------------------------------------
// Write (interface IOPCAsyncIO)
//
// - Asynchronous write of item values
//
// returns:
//      S_OK                   - writtes of all items initialised without error
//      S_FALSE                - at least one write initialisation failed
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Write(
	IN DWORD dwConnection,      // connection id
	IN DWORD dwNumItems,        // number of items
	IN OPCHANDLE* phServer,     // item handles
	IN VARIANT* pItemValues,    // write values
	OUT DWORD* pTransactionID,  // transaction id (server given)
	OUT HRESULT** ppErrors)     // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO::Write, Write(dwConnection, dwNumItems, phServer, pItemValues, pTransactionID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pTransactionID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(pItemValues)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_DATAOBJECT, dwConnection, FALSE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Write(res, dwConnection, dwNumItems, phServer, pItemValues, pTransactionID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, pItemValues, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Write(res, dwConnection, dwNumItems, phServer, pItemValues, pTransactionID, ppErrors))
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(FALSE, SODaSTransaction::dataObject, dwConnection, OPC_DS_DEVICE, srv->getWorkEvent(), dwNumItems, phServer, pItemValues, NULL, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, pTransactionID, ppErrors);

		if ((S_OK == res) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// -> start async write transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
		else
		{
			// transaction was created with an error
			if (SUCCEEDED(res))
			{
				// result was S_FALSE (invalid server handle)
				// free transaction
				*pTransactionID = 0;
				tac.release();  // free transaction
			}
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// failed return code
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Write(res, dwConnection, dwNumItems, phServer, pItemValues, pTransactionID, ppErrors))
} // Write


//-----------------------------------------------------------------------------
// Refresh (interface IOPCAsyncIO)
//
// - Asynchronous refresh of all active items of the active group
//
// returns:
//      S_OK                   - refresh initialised
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Refresh(
	IN DWORD dwConnection,      // connection id
	IN OPCDATASOURCE dwSource,  // data source: cache or device
	OUT DWORD* pTransactionID)  // transaction id (server given)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO::Refresh, Refresh(dwConnection, dwSource, pTransactionID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pTransactionID)
	SOSRV_IN_PARAM((dwSource == OPC_DS_DEVICE) || (dwSource == OPC_DS_CACHE))
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_DATAOBJECT, dwConnection, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Refresh(res, dwConnection, dwSource, pTransactionID))
	SOCmnPointer<SODaSTransaction> dest;

	// check group and item activation
	if (!m_me->getActive())
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("group not active")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, Refresh(res, dwConnection, dwSource, pTransactionID))
	}

	SODaSRequest::requestType reqType;

	if (dwSource == OPC_DS_CACHE)
	{
		reqType = SODaSRequest::asyncCache;
	}
	else
	{
		reqType = SODaSRequest::asyncDevice;
	}

	m_me->copyCyclicTransaction(dwConnection, reqType, 0, SODaSTransaction::dataObject, (SODaSTransaction**)dest);

	if (dest->getRequestCount() == 0)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no active item in group")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, Refresh(res, dwConnection, dwSource, pTransactionID))
	}

	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!!srv)
	{
		// -> start async refresh transaction
		dest->start(srv);
		srv->addTransaction(dest);
		*pTransactionID = dest->getTransactionId();
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Refresh(res, dwConnection, dwSource, pTransactionID))
} // Refresh


//-----------------------------------------------------------------------------
// Cancel (interface IOPCAsyncIO)
//
// - Asynchronous refresh of all active items of the active group
//
// returns:
//      S_OK   - canceled refresh
//      E_FAIL - too late, invalid transaction id or group allready removed
//
STDMETHODIMP SODaSComGroup::Cancel(
	IN DWORD dwTransactionID)   // transaction id (server given)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO::Cancel, Cancel(dwTransactionID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(Cancel(res, dwTransactionID))
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!!srv)
	{
		res = srv->cancelTransaction(dwTransactionID);
	}

	SOSRV_END_INTERFACE_FUNCTION(Cancel(res, dwTransactionID))
} // Cancel



//-----------------------------------------------------------------------------
// IOPCAsyncIO2                                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Read (interface IOPCAsyncIO2)
//
// - Asynchronous device read of item values
//
// returns:
//      S_OK                   - reads of all items initialised without error
//      S_FALSE                - at least one read initialisation failed
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Read(
	IN DWORD dwNumItems,        // number of items
	IN OPCHANDLE* phServer,     // item handles
	IN DWORD dwTransactionID,   // transaction id (client given)
	OUT DWORD* pdwCancelID,     // cancel id (server given)
	OUT HRESULT** ppErrors)     // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO2::Read, Read(dwNumItems, phServer, dwTransactionID, pdwCancelID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCancelID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(phServer)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Read(res, dwNumItems, phServer, dwTransactionID, pdwCancelID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, phServer, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Read(res, dwNumItems, phServer, dwTransactionID, pdwCancelID, ppErrors))
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(TRUE, SODaSTransaction::connectionPoint, 0, OPC_DS_DEVICE, srv->getWorkEvent(), dwNumItems, phServer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, pdwCancelID, ppErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(dwTransactionID);
			// -> start async read transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Read(res, dwNumItems, phServer, dwTransactionID, pdwCancelID, ppErrors))
} // Read


//-----------------------------------------------------------------------------
// Write (interface IOPCAsyncIO2)
//
// - Asynchronous write of item values
//
// returns:
//      S_OK                   - writtes of all items initialised without error
//      S_FALSE                - at least one write initialisation failed
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Write(
	IN DWORD dwNumItems,        // number of items
	IN OPCHANDLE* phServer,     // item handles
	IN VARIANT* pItemValues,    // write values
	IN DWORD dwTransactionID,   // transaction id (client given)
	OUT DWORD* pdwCancelID,     // cancel id (server given)
	OUT HRESULT** ppErrors)     // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO2::Write, Write(dwNumItems, phServer, pItemValues, dwTransactionID, pdwCancelID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCancelID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(pItemValues)
	SOSRV_IN_PARAM(dwNumItems != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, FALSE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Write(res, dwNumItems, phServer, pItemValues, dwTransactionID, pdwCancelID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(dwNumItems, phServer, pItemValues, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(Write(res, dwNumItems, phServer, pItemValues, dwTransactionID, pdwCancelID, ppErrors))
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(FALSE, SODaSTransaction::connectionPoint, 0, OPC_DS_DEVICE, srv->getWorkEvent(), dwNumItems, phServer, pItemValues, NULL, NULL, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, pdwCancelID, ppErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(dwTransactionID);
			// -> start async write transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Write(res, dwNumItems, phServer, pItemValues, dwTransactionID, pdwCancelID, ppErrors))
} // Write


//-----------------------------------------------------------------------------
// Refresh2 (interface IOPCAsyncIO2)
//
// - Asynchronous refresh of all active items of the active group
//
// returns:
//      S_OK                   - refresh initialised
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::Refresh2(
	IN OPCDATASOURCE dwSource,  // data source: cache or device
	IN DWORD dwTransactionID,   // transaction id (client given)
	OUT DWORD* pdwCancelID)     // cancel id (server given)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO2::Refresh2, Refresh2(dwSource, dwTransactionID, pdwCancelID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCancelID)
	SOSRV_IN_PARAM((dwSource == OPC_DS_DEVICE) || (dwSource == OPC_DS_CACHE))
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(Refresh2(res, dwSource, dwTransactionID, pdwCancelID))
	SOCmnPointer<SODaSTransaction> dest;

	// check group and item activation
	if (!m_me->getActive())
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("group not active")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, Refresh2(res, dwSource, dwTransactionID, pdwCancelID))
	}

	SODaSRequest::requestType reqType;

	if (dwSource == OPC_DS_CACHE)
	{
		reqType = SODaSRequest::asyncCache;
	}
	else
	{
		reqType = SODaSRequest::asyncDevice;
	}

	m_me->copyCyclicTransaction(0, reqType, 0, SODaSTransaction::connectionPoint, (SODaSTransaction**)dest);
	dest->setClientTransactionId(dwTransactionID);

	if (dest->getRequestCount() == 0)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no active item in group")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, Refresh2(res, dwSource, dwTransactionID, pdwCancelID))
	}

	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!!srv)
	{
		// -> start async refresh transaction
		dest->start(srv);
		srv->addTransaction(dest);
		*pdwCancelID = dest->getTransactionId();
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Refresh2(res, dwSource, dwTransactionID, pdwCancelID))
} // Refresh2


//-----------------------------------------------------------------------------
// Cancel2 (interface IOPCAsyncIO2)
//
// - Asynchronous refresh of all active items of the active group
//
// returns:
//      S_OK   - canceled refresh
//      E_FAIL - too late, invalid transaction id or group allready removed
//
STDMETHODIMP SODaSComGroup::Cancel2(
	IN DWORD dwCancelID)    // cancel id (server given)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO::Cancel2, Cancel2(dwCancelID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(Cancel2(res, dwCancelID))
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!!srv)
	{
		res = srv->cancelTransaction(dwCancelID);
	}

	SOSRV_END_INTERFACE_FUNCTION(Cancel2(res, dwCancelID))
} // Cancel2


//-----------------------------------------------------------------------------
// SetEnable (interface IOPCAsyncIO2)
//
// - En/disable OnDataChange Callback
//
// returns:
//      S_OK                   - set enable
//      E_FAIL                 - can't set enable
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//
STDMETHODIMP SODaSComGroup::SetEnable(
	IN BOOL bEnable)    // en/disable
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO2::SetEnable, SetEnable(bEnable))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(SetEnable(res, bEnable))
	BOOL prevEnable = m_me->getEnable();

	if (m_me->setEnable((bEnable != 0)))
	{
		res = S_OK;

		if ((bEnable) && (!prevEnable))
		{
			SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
			SOCmnPointer<SODaSTransaction> dest;
			m_me->copyCyclicTransaction(0, SODaSRequest::asyncInitNotify, 0, SODaSTransaction::connectionPoint, (SODaSTransaction**)dest);

			if ((dest->getRequestCount() > 0) && (srv.isNotNull()))
			{
				dest->setTransactionId(0);  // no cyclic transaction but id is 0
				dest->start(srv);
				srv->addTransaction(dest);
			}
		}
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(SetEnable(res, bEnable))
} // SetEnable


//-----------------------------------------------------------------------------
// GetEnable (interface IOPCAsyncIO2)
//
// - get en/disable state of OnDataChange Callback
//
// returns:
//      S_OK                   - set enable
//      E_FAIL                 - can't set enable
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//
STDMETHODIMP SODaSComGroup::GetEnable(
	OUT BOOL* pbEnable) // en/disabled
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO2::GetEnable, GetEnable(pbEnable))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pbEnable)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(GetEnable(res, pbEnable))
	*pbEnable = m_me->getEnable();
	SOSRV_END_INTERFACE_FUNCTION(GetEnable(res, pbEnable))
} // GetEnable


//-----------------------------------------------------------------------------
// IOPCAsyncIO3                                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ReadMaxAge (interface IOPCAsyncIO3)
//
// - Asynchronous read of item values
//
// returns:
//      S_OK                   - reads of all items initialised without error
//      S_FALSE                - at least one read initialisation failed
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::ReadMaxAge(
	IN DWORD count,         // number of items
	IN OPCHANDLE* phServer, // items server handle
	IN DWORD* pMaxAge,      // max cache age
	IN DWORD transactionID, // client transaction id
	OUT DWORD* pCancelID,   // server cancel id
	OUT HRESULT** ppErrors) // results of read operations
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO3::ReadMaxAge, ReadMaxAge(count, phServer, pMaxAge, transactionID, pCancelID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pCancelID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(phServer)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(pMaxAge)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(ReadMaxAge(res, count, phServer, pMaxAge, transactionID, pCancelID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, pMaxAge, (void**)ppErrors, ppErrors)))
	{
		SOSRV_RETURN_INTERFACE_FUNCTION(ReadMaxAge(res, count, phServer, pMaxAge, transactionID, pCancelID, ppErrors))
	}

	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(TRUE, SODaSTransaction::connectionPoint, 0, 0, srv->getWorkEvent(), count, phServer, NULL, NULL, pMaxAge, NULL, NULL, NULL, NULL, (SODaSTransaction**)tac, pCancelID, ppErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(transactionID);
			// -> start async read transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(ReadMaxAge(res, count, phServer, pMaxAge, transactionID, pCancelID, ppErrors))
} // ReadMaxAge


//-----------------------------------------------------------------------------
// WriteVQT (interface IOPCAsyncIO3)
//
// - Asynchronous write of item values, qualities and timestamps
//
// returns:
//      S_OK                   - writtes of all items initialised without error
//      S_FALSE                - at least one write initialisation failed
//    OPC_E_NOTSUPPORTED     - unsupported combination of value, quality and timestamp
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::WriteVQT(
	IN DWORD count,          // number of items
	IN OPCHANDLE* phServer,  // items server handles
	IN OPCITEMVQT* pItemVQT, // write values, qualities and timestamps
	IN DWORD transactionID,  // client transaction id
	OUT DWORD* pCancelID,    // server cancel id
	OUT HRESULT** ppErrors)  // result of write operations
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO3::WriteVQT, WriteVQT(count, phServer, pItemVQT, transactionID, pCancelID, ppErrors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pCancelID)
	SOSRV_OUT_PARAM_PTR(ppErrors)
	SOSRV_IN_PARAM(pItemVQT)
	SOSRV_IN_PARAM(count != 0)
	SOSRV_IN_PARAM(phServer != NULL)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, FALSE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(WriteVQT(res, count, phServer, pItemVQT, transactionID, pCancelID, ppErrors))
	SOCmnPointer<SODaSTransaction> tac;

	// item array standard parameter check
	if (FAILED(res = itemArrayCheckAndAlloc(count, phServer, pItemVQT, (void**)ppErrors, ppErrors)))
		SOSRV_RETURN_INTERFACE_FUNCTION(WriteVQT(res, count, phServer, pItemVQT, transactionID, pCancelID, ppErrors))
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (srv.isNotNull())
	{
		res = m_me->createTransaction(FALSE, SODaSTransaction::connectionPoint, 0, OPC_DS_DEVICE, srv->getWorkEvent(), count, phServer, NULL, NULL, NULL, NULL, NULL, NULL, pItemVQT, (SODaSTransaction**)tac, pCancelID, ppErrors);

		if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
		{
			// transaction was created with no error
			// set client transaction id
			tac->setClientTransactionId(transactionID);
			// -> start async write transaction
			tac->start(srv);
			srv->addTransaction(tac);
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (FAILED(res))
	{
		// transaction was created with an error
		// free item specific errors
		CoTaskMemFree(*ppErrors);
		*ppErrors = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(WriteVQT(res, count, phServer, pItemVQT, transactionID, pCancelID, ppErrors))
} // WriteVQT


//-----------------------------------------------------------------------------
// RefreshMaxAge (interface IOPCAsyncIO3)
//
// - Asynchronous refresh of all active items of the active group
//
// returns:
//      S_OK                   - refresh initialised
//      E_FAIL                 - group allready removed
//      E_INVALIDARG           - NULL pointer parameter
//      CONNECT_E_NOCONNECTION - client has not connected advise sink
//      E_OUTOFMEMORY          - can't allocate result buffers
//
STDMETHODIMP SODaSComGroup::RefreshMaxAge(
	IN DWORD maxAge,        // max cache age
	IN DWORD transactionID, // client transaction id
	OUT DWORD* pCancelID)   // server cancel id
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IOPCAsyncIO3::RefreshMaxAge, RefreshMaxAge(maxAge, transactionID, pCancelID))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pCancelID)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_CHECK(m_me->validConnection(SODASDATACALLBACK_CT_CONNECTIONPOINT, 0, TRUE), CONNECT_E_NOCONNECTION)
	SOSRV_END_PARAMS_CHECK(RefreshMaxAge(res, maxAge, transactionID, pCancelID))
	SOCmnPointer<SODaSTransaction> dest;

	// check group and item activation
	if (!m_me->getActive())
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("group not active")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, RefreshMaxAge(res, maxAge, transactionID, pCancelID))
	}

	m_me->copyCyclicTransaction(0, SODaSRequest::asyncMaxAge, maxAge, SODaSTransaction::connectionPoint, (SODaSTransaction**)dest);
	dest->setClientTransactionId(transactionID);

	if (dest->getRequestCount() == 0)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("no active item in group")));
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_FAIL, RefreshMaxAge(res, maxAge, transactionID, pCancelID))
	}

	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

	if (!!srv)
	{
		// -> start async refresh transaction
		dest->start(srv);
		srv->addTransaction(dest);
		*pCancelID = dest->getTransactionId();
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(RefreshMaxAge(res, maxAge, transactionID, pCancelID))
} // RefreshMaxAge



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
//      S_OK                    - the connection has been established
//      E_INVALIDARG            - a parameter value is not valid
//      CONNECT_E_ADVISELIMIT   - the connection point has already reached its
//                                limit of connections
//      CONNECT_E_CANNOTCONNECT - the sink does not support the interface required
//                                by this connection point
//      E_FAIL                  - group already removed
//
STDMETHODIMP SODaSComGroup::Advise(
	IN IUnknown* pUnk,      // client sink
	OUT DWORD* pdwCookie)   // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IConnectionPoint::Advise, Advise(pUnk, pdwCookie))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwCookie)
	SOSRV_IN_PARAM_PTR(pUnk)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(Advise(res, pUnk, pdwCookie))
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();

	if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
	{
		setProxyBlanket(pUnk, entry->getAuthentificationLevel(),
						entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
	}

	SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
	SOCmnList<SODaSTransaction> tl = srv->getTransactionList();
	tl.lock();
	IOPCDataCallback* dataCallback;
	res = pUnk->QueryInterface(IID_IOPCDataCallback, (void**)&dataCallback);

	if (SUCCEEDED(res))
	{
		if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
		{
			setProxyBlanket(dataCallback, entry->getAuthentificationLevel(),
							entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
		}

		res = m_me->adviseDataCallback(SODASDATACALLBACK_CT_CONNECTIONPOINT, dataCallback, 0, pdwCookie);
		dataCallback->Release();
	}
	else
	{
		if (res == E_NOINTERFACE)
		{
			res = CONNECT_E_CANNOTCONNECT;
		}
	}

	if ((SUCCEEDED(res)) && (m_me->getActive()) && (m_me->getEnable()))
	{
		SOCmnPointer<SODaSTransaction> dest;
		m_me->copyCyclicTransaction(*pdwCookie, SODaSRequest::asyncInitNotify, 0, SODaSTransaction::connectionPoint, (SODaSTransaction**)dest);

		if ((dest->getRequestCount() > 0) && (srv.isNotNull()))
		{
			dest->setTransactionId(0);  // no cyclic transaction but id is 0
			dest->start(srv);
			srv->addTransaction(dest);
		}
	}

	tl.unlock();
	SOSRV_END_INTERFACE_FUNCTION(Advise(res, pUnk, pdwCookie))
} // Advise


//-----------------------------------------------------------------------------
// Unadvise (interface IConnectionPoint)
//
// - terminates an advisory connection
//
// returns:
//      S_OK                   - The connection was successfully terminated
//      CONNECT_E_NOCONNECTION - the cookie does not represent a valid connection
//      E_FAIL                 - group already removed
//
STDMETHODIMP SODaSComGroup::Unadvise(
	IN DWORD dwCookie)  // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IConnectionPoint::Unadvise, Unadvise(dwCookie))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(Unadvise(res, dwCookie))
	res = m_me->unadviseDataCallback(SODASDATACALLBACK_CT_CONNECTIONPOINT, dwCookie);
	SOSRV_END_INTERFACE_FUNCTION(Unadvise(res, dwCookie))
} // Unadvise


//-----------------------------------------------------------------------------
// EnumConnections (interface IConnectionPoint)
//
// - creates an enumerator object to iterate through the current
//   connections for this connection point
//
// returns:
//      E_NOTIMPL - Not implemented
//
STDMETHODIMP SODaSComGroup::EnumConnections(IN IEnumConnections** /* ppEnum */)
{
	return E_NOTIMPL;
} // EnumConnections



//-----------------------------------------------------------------------------
// IDataObject                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DAdvise (interface IDataObject)
//
// - establishes a connection between the object and the client's sink
//
// returns:
//      S_OK                    - the connection has been established
//      E_INVALIDARG            - a parameter value is not valid
//      DV_E_DVASPECT           - invalid draw aspect
//      DV_E_TYMED              - invalid tymed
//      DV_E_LINDEX             - invalid lindex
//      DV_E_FORMATETC          - invalid formatetc
//      CONNECT_E_ADVISELIMIT   - the connection point has already reached its
//                                limit of connections
//      E_FAIL                  - group already removed
//
STDMETHODIMP SODaSComGroup::DAdvise(
	IN FORMATETC* format,       // clipboard format
	IN DWORD advf,              // advise flags
	IN IAdviseSink* pAdvSink,   // advise sink
	OUT DWORD* pdwConnection)   // connection id
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IDataObject::DAdvise, DAdvise(format, advf, pAdvSink, pdwConnection))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(pdwConnection)
	SOSRV_IN_PARAM_PTR(pAdvSink)
	SOSRV_IN_PARAM_PTR(format)
	SOSRV_CHECK((format->dwAspect == DVASPECT_CONTENT), DV_E_DVASPECT)
	SOSRV_CHECK((format->ptd == NULL), E_INVALIDARG)
	SOSRV_CHECK((format->tymed == TYMED_HGLOBAL), DV_E_TYMED)
	SOSRV_CHECK((format->lindex == -1), DV_E_LINDEX)
	SOSRV_CHECK(((format->cfFormat == OPCSTMFORMATDATA) || (format->cfFormat == OPCSTMFORMATDATATIME) || (format->cfFormat == OPCSTMFORMATWRITECOMPLETE)), DV_E_FORMATETC)
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(DAdvise(res, format, advf, pAdvSink, pdwConnection))
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();

#ifdef _WIN64
	// NOTE: OPCDA1.0 uses a memory block for transferring data between client and server.
	//       The memory contains structures with platform-dependent members.
	//       Sizeof(VARIANT) is 16 for 32-bit systems and 24 for 64-bit systems.
	//       Since OPC DA v1.0a dates back in 1997, it is a common agreement that OPCDA1.0 servers are 32-bit only.
	//       Remove this at your own risk!
	if (TRUE)
	{
		// avoid warnings
		return E_INVALIDARG;
	}
#endif

	if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
	{
		setProxyBlanket(pAdvSink, entry->getAuthentificationLevel(),
						entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
	}

	res = m_me->adviseDataCallback(SODASDATACALLBACK_CT_DATAOBJECT, pAdvSink, format->cfFormat, pdwConnection);

	if ((SUCCEEDED(res)) && (m_me->getActive()) && (m_me->getEnable()))
	{
		if ((format->cfFormat == OPCSTMFORMATDATA) ||
			(format->cfFormat == OPCSTMFORMATDATATIME))
		{
			SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();
			SOCmnPointer<SODaSTransaction> dest;
			m_me->copyCyclicTransaction(*pdwConnection, SODaSRequest::asyncInitNotify, 0, SODaSTransaction::dataObject, (SODaSTransaction**)dest);

			if ((dest->getRequestCount() > 0) && (srv.isNotNull()))
			{
				dest->setTransactionId(0);  // no cyclic transaction but id is 0
				dest->start(srv);
				srv->addTransaction(dest);
			}
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(DAdvise(res, format, advf, pAdvSink, pdwConnection))
} // DAdvise


//-----------------------------------------------------------------------------
// DUnadvise (interface IDataObject)
//
// - terminates an advisory connection
//
// returns:
//      S_OK                   - The connection was successfully terminated
//      CONNECT_E_NOCONNECTION - the cookie does not represent a valid connection
//      E_FAIL                 - group already removed
//
STDMETHODIMP SODaSComGroup::DUnadvise(
	IN DWORD dwConnection)  // connection id
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(DA, IDataObject::DUnadvise, DUnadvise(dwConnection))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_CHECK(!(m_me->isRemoved()), E_FAIL)
	SOSRV_END_PARAMS_CHECK(DUnadvise(res, dwConnection))
	res = m_me->unadviseDataCallback(SODASDATACALLBACK_CT_DATAOBJECT, dwConnection);
	SOSRV_END_INTERFACE_FUNCTION(DUnadvise(res, dwConnection))
} // DUnadvise

// all other IDataObject functions are not implemented
STDMETHODIMP SODaSComGroup::EnumDAdvise(OUT IEnumSTATDATA** /* ppenumAdvise */)
{
	return E_NOTIMPL;
}
STDMETHODIMP SODaSComGroup::GetData(IN FORMATETC* /* formatIn */, OUT STGMEDIUM* /* medium */)
{
	return DV_E_FORMATETC;
}
STDMETHODIMP SODaSComGroup::QueryGetData(IN FORMATETC* /* format */)
{
	return DV_E_FORMATETC;
}
STDMETHODIMP SODaSComGroup::GetDataHere(IN FORMATETC* /* format */, OUT STGMEDIUM* /* medium */)
{
	return DV_E_FORMATETC;
}
STDMETHODIMP SODaSComGroup::GetCanonicalFormatEtc(IN FORMATETC* /* formatIn */, OUT FORMATETC* /* formatOut */)
{
	return DV_E_FORMATETC;
}
STDMETHODIMP SODaSComGroup::SetData(IN FORMATETC* /* format */, IN STGMEDIUM* /* medium */, OUT BOOL/* fRelease */)
{
	return E_NOTIMPL;
}
STDMETHODIMP SODaSComGroup::EnumFormatEtc(IN DWORD /* dwDirection */, OUT IEnumFORMATETC** /* ppenumFormatEtc */)
{
	return E_NOTIMPL;
}


//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface overwritable method
//
// returns:
//      E_NOTIMPL     - do standard ATL QueryInterface
//      S_OK          - returned interface
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaSComGroup::queryInterface(
	IN REFIID iid,      // interface id
	IN void** object)   // interface
{
	HRESULT res;

	if ((res = m_me->queryInterface(iid, object)) != E_NOTIMPL)
	{
		return res;
	}

	if ((iid != IID_IDataObject) && (iid != IID_IOPCAsyncIO))
	{
		return E_NOTIMPL;
	}
	else
	{
		if (m_me->supportConnectionType(SODASDATACALLBACK_CT_DATAOBJECT))
		{
			return E_NOTIMPL;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
} // queryInterface


//-----------------------------------------------------------------------------
// lastExternalReleaseWithInternalRefs
//
// - released last external reference with pending internal references
//
// returns:
//      TRUE  - object deleted
//      FALSE - object still exists
//
BOOL SODaSComGroup::lastExternalReleaseWithInternalRefs(void)
{
	if (m_me)
	{
		SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_me->getParentBranch();

		if (!!srv)
		{
			return srv->handleLastExternalReleaseWithInternalRefs((void*)this, (void*)m_server);
		}
	}

	return FALSE;
} // lastExternalReleaseWithInternalRefs

#endif // SOFEATURE_DCOM

#endif // SODAS
