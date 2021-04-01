#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_DCOM

#include "SOAeSComSubscription.h"
#include "SOAeSComServer.h"
#include <opcae_er.h>


//-----------------------------------------------------------------------------
// SOAeSComSubscription                                                       |
//-----------------------------------------------------------------------------

SOAeSComSubscription::SOAeSComSubscription()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_server = NULL;
	m_me = NULL;
	_TRACE_CREATE_COMOBJ();
}

SOAeSComSubscription::~SOAeSComSubscription()
{
	_TRACE_DELETE_COMOBJ();

	if (m_server)
	{
		// release server object
		m_server->m_me->removeLeaf(m_me);
		releaseComServer();
	}

	if (m_me)
	{
		// release refresh object if one is in progress
		SOCmnPointer<SOAeSRefresh> refresh = m_me->getRefresh();

		if (!!refresh)
		{
			refresh->cancel();
		}

		refresh.release();
		// release tree object
		m_me->release();
	}

	lastCOMObjectActions();
}


//-----------------------------------------------------------------------------
// init
//
// - initialize the subscription COM object
//
void SOAeSComSubscription::init(
	IN SOAeSComServer* server,      // server COM object
	IN SOAeSSubscription* subscr)   // subscription object
{
	m_server = server;
	m_me = subscr;
	m_me->m_me = this;
	m_me->addRef();
	// keep server alive till subscription is freed
	((SOSrvComObject<SOAeSComServer> *)m_server)->internalAddRef();
} // init


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SOAeSComSubscription::lockMe(void)
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
void SOAeSComSubscription::unlockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.unlock();
	}
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite the SOAeSComSubscription object from the SOAeSSubscription object
//
void SOAeSComSubscription::disuniteMe(void)
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
void SOAeSComSubscription::releaseComServer(void)
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
LPCTSTR SOAeSComSubscription::getObjTraceId(void)
{
	if (m_objectTraceId.IsEmpty())
	{
		if (m_server)
			if (m_server->m_me)
			{
				m_objectTraceId.Format(_T("(sus|%8.8X| %s)"), this->getOpcHandle(), (LPCTSTR)m_server->m_me->getClientName());
				return m_objectTraceId;
			}

		m_objectTraceId.Format(_T("(sus|%8.8X)"), this->getOpcHandle());
	}

	return m_objectTraceId;
} // getObjTraceId



//-----------------------------------------------------------------------------
// IOPCEventSubscriptionMgt                                                   |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetFilter (interface IOPCEventSubscriptionMgt)
//
// - set the filter arguments of the subscription
//
// returns:
//		S_OK          - set filter
//		S_FALSE       - one ore more of the filter criterias ware ignored
//		E_FAIL        - operation failed
//		E_INVALIDARG  - invalid parameter
//		E_OUTOFMEMORY - out of memory
//		OPC_E_BUSY    - server is busy
//
STDMETHODIMP SOAeSComSubscription::SetFilter(
	IN DWORD eventType,             // event type
	IN DWORD numCategories,         // number of categories
	IN DWORD* eventCategoryArray,   // category array
	IN DWORD lowSeverity,           // low severity
	IN DWORD highSeverity,          // high severity
	IN DWORD numAreas,              // number of areas
	IN LPWSTR* areaArray,           // area array
	IN DWORD numSources,            // number of sources
	IN LPWSTR* sourceArray)         // source array
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::SetFilter, SetFilter(eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM((eventType > 0) && (eventType <= OPC_ALL_EVENTS))
	SOSRV_IN_PARAM(lowSeverity > 0)
	SOSRV_IN_PARAM(lowSeverity <= 1000)
	SOSRV_IN_PARAM(highSeverity > 0)
	SOSRV_IN_PARAM(highSeverity <= 1000)
	SOSRV_IN_PARAM(lowSeverity <= highSeverity)
	SOSRV_IN_PARAM(!((numCategories > 0) && (!eventCategoryArray)))
	SOSRV_IN_PARAM(!((numAreas > 0) && (!areaArray)))
	SOSRV_IN_PARAM(!((numSources > 0) && (!sourceArray)))
	SOSRV_END_PARAMS_CHECK(SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))

	if (m_me->isRefreshInProgress())
	{
		// a refresh is currently in progress
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(OPC_E_BUSY, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
	}

	USES_CONVERSION;
	SOCmnDWORDList categoryList;    // event categories list
	SOCmnStringList areaList;       // area list
	SOCmnStringList sourceList;     // source list
	SOCmnDWORDList* cList = NULL;
	SOCmnStringList* aList = NULL;
	SOCmnStringList* sList = NULL;
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpace = entry->getAreaSpaceRoot();
	DWORD i;

	// create and fill lists
	if (numCategories > 0)
	{
		SOCmnPointer<SOAeSCategory> cat;
		categoryList.create();

		for (i = 0; i < numCategories; i++)
		{
			// check if valid category
			cat = eventSpace->findCategory(eventCategoryArray[i]);

			if (!cat)
				SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
				categoryList.add(eventCategoryArray[i]);
		}

		cList = &categoryList;
	}

	if (numAreas > 0)
	{
		SOCmnPointer<SOAeSArea> area;
		LPCTSTR areaName;
		areaList.create();

		for (i = 0; i < numAreas; i++)
		{
			if (!areaArray[i])
				SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
				areaName = W2CT(areaArray[i]);

			if (!m_server->m_me->includesFilterChars(areaName))
			{
				// check if valid area
				area = (SOAeSArea*)areaSpace->findBranch(areaName, TRUE);

				if (!area)
					if (!m_server->m_me->isAreaName(areaName))
						SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
					}

			areaList.add(areaName);
		}

		aList = &areaList;
	}

	if (numSources > 0)
	{
		SOCmnPointer<SOAeSSource> source;
		LPCTSTR srcName;
		sourceList.create();

		for (i = 0; i < numSources; i++)
		{
			if (!sourceArray[i])
				SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
				srcName = W2CT(sourceArray[i]);	

			if (!m_server->m_me->includesFilterChars(srcName))
			{
				source = (SOAeSSource*)areaSpace->findLeaf(srcName, TRUE);

				if (!source)
					if (!m_server->m_me->isSourceName(srcName))
						SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
					}

			sourceList.add(srcName);
		}

		sList = &sourceList;
	}

	// set filter
	res = m_me->setFilter(eventType, cList, lowSeverity, highSeverity, aList, sList);
	// destroy lists
	categoryList.destroy();
	areaList.destroy();
	sourceList.destroy();
	SOSRV_END_INTERFACE_FUNCTION(SetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
} // SetFilter


//-----------------------------------------------------------------------------
// GetFilter (interface IOPCEventSubscriptionMgt)
//
// - get the filter arguments of the subscription
//
// returns:
//		S_OK          - get filter
//		E_FAIL        - operation failed
//		E_INVALIDARG  - invalid parameter
//		E_OUTOFMEMORY - out of memory
//
STDMETHODIMP SOAeSComSubscription::GetFilter(
	OUT DWORD* eventType,           // event type
	OUT DWORD* numCategories,       // number of categories
	OUT DWORD** eventCategoryArray, // category array
	OUT DWORD* lowSeverity,         // low severity
	OUT DWORD* highSeverity,        // high severity
	OUT DWORD* numAreas,            // number of areas
	OUT LPWSTR** areaArray,         // area array
	OUT DWORD* numSources,          // number of sources
	OUT LPWSTR** sourceArray)       // source array
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::GetFilter, GetFilter(eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(eventType)
	SOSRV_OUT_PARAM_PTR(numCategories)
	SOSRV_OUT_PARAM_PTR(eventCategoryArray)
	SOSRV_OUT_PARAM_PTR(lowSeverity)
	SOSRV_OUT_PARAM_PTR(highSeverity)
	SOSRV_OUT_PARAM_PTR(numAreas)
	SOSRV_OUT_PARAM_PTR(areaArray)
	SOSRV_OUT_PARAM_PTR(numSources)
	SOSRV_OUT_PARAM_PTR(sourceArray)
	SOSRV_END_PARAMS_CHECK(GetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
	*eventType = OPC_ALL_EVENTS;
	*lowSeverity = 1;
	*highSeverity = 1000;
	USES_CONVERSION;
	SOCmnDWORDList categoryList;
	SOCmnStringList areaList;
	SOCmnStringList sourceList;
	SOCmnListPosition pos;
	BOOL outOfMemory = FALSE;
	DWORD i;
	// get filter
	res = m_me->getFilter(eventType, &categoryList, lowSeverity, highSeverity, &areaList, &sourceList);

	if (SUCCEEDED(res))
	{
		// allocate the output data
		if (!categoryList.isEmpty())
		{
			*numCategories = categoryList.getCount();
			*eventCategoryArray = (DWORD*) CoTaskMemAlloc(*numCategories * sizeof(DWORD));
			memset(*eventCategoryArray, 0, *numCategories * sizeof(DWORD));
		}

		if (!areaList.isEmpty())
		{
			*numAreas = areaList.getCount();
			*areaArray = (LPWSTR*) CoTaskMemAlloc(*numAreas * sizeof(LPWSTR));
			memset(*areaArray, 0, *numAreas * sizeof(LPWSTR));
		}

		if (!sourceList.isEmpty())
		{
			*numSources = sourceList.getCount();
			*sourceArray = (LPWSTR*) CoTaskMemAlloc(*numSources * sizeof(LPWSTR));
			memset(*sourceArray, 0, *numSources * sizeof(LPWSTR));
		}

		if (((*numCategories > 0) && (!*eventCategoryArray)) ||
			((*numAreas > 0) && (!*areaArray)) || ((*numSources > 0) && (!*sourceArray)))
		{
			outOfMemory = TRUE;
		}

		// copy the list entries to the output arrays
		if (!categoryList.isEmpty())
		{
			DWORD category;
			pos = categoryList.getStartPosition();
			i = 0;

			while ((pos) && (!outOfMemory))
			{
				category = (DWORD)categoryList.getNext(pos);
				(*eventCategoryArray)[i] = category;
				i++;
			}
		}

		if (!areaList.isEmpty())
		{
			pos = areaList.getStartPosition();
			i = 0;

			while ((pos) && (!outOfMemory))
			{
				(*areaArray)[i] = SOCmnString::cloneToWCHAR(areaList.getNext(pos));

				if (!(*areaArray)[i])
				{
					outOfMemory = TRUE;
				}

				i++;
			}
		}

		if (!sourceList.isEmpty())
		{
			pos = sourceList.getStartPosition();
			i = 0;

			while ((pos) && (!outOfMemory))
			{
				(*sourceArray)[i] = SOCmnString::cloneToWCHAR(sourceList.getNext(pos));

				if (!(*sourceArray)[i])
				{
					outOfMemory = TRUE;
				}

				i++;
			}
		}

		if (outOfMemory)
		{
			if (*eventCategoryArray)
			{
				CoTaskMemFree(*eventCategoryArray);
			}

			if (*areaArray)
			{
				for (i = 0; i < *numAreas; i++)
				{
					if ((*areaArray)[i])
					{
						CoTaskMemFree((*areaArray)[i]);
					}
				}

				CoTaskMemFree(*areaArray);
			}

			if (*sourceArray)
			{
				for (i = 0; i < *numSources; i++)
				{
					if ((*sourceArray)[i])
					{
						CoTaskMemFree((*sourceArray)[i]);
					}
				}

				CoTaskMemFree(*sourceArray);
			}

			*eventCategoryArray = NULL;
			*numCategories = 0;
			*areaArray = NULL;
			*numAreas = 0;
			*sourceArray = NULL;
			*numSources = 0;
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(GetFilter(res, eventType, numCategories, eventCategoryArray, lowSeverity, highSeverity, numAreas, areaArray, numSources, sourceArray))
} // GetFilter


//-----------------------------------------------------------------------------
// SelectReturnedAttributes (interface IOPCEventSubscriptionMgt)
//
// - select the returned attributes for a event category
//
// returns:
//		S_OK         - all attributes set
//		S_FALSE      - one or more attributes ignored
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComSubscription::SelectReturnedAttributes(
	IN DWORD eventCategory, // event category
	IN DWORD count,         // number of attributes
	IN DWORD* attributeIDs) // attribute array
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::SelectReturnedAttributes, SelectReturnedAttributes(eventCategory, count, attributeIDs))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(!((count != 0) && (attributeIDs == NULL)))
	SOSRV_END_PARAMS_CHECK(SelectReturnedAttributes(res, eventCategory, count, attributeIDs))
	res = m_me->selectReturnedAttributes(eventCategory, count, attributeIDs);
	SOSRV_END_INTERFACE_FUNCTION(SelectReturnedAttributes(res, eventCategory, count, attributeIDs))
} // SelectReturnedAttributes


//-----------------------------------------------------------------------------
// GetReturnedAttributes (interface IOPCEventSubscriptionMgt)
//
// - get the returned attributes for a event category
//
// returns:
//		S_OK         - all attributes set
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComSubscription::GetReturnedAttributes(
	IN DWORD eventCategory,     // event category
	OUT DWORD* count,           // number of attributes
	OUT DWORD** attributeIDs)   // attribute array
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::GetReturnedAttributes, GetReturnedAttributes(eventCategory, count, attributeIDs))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(attributeIDs)
	SOSRV_END_PARAMS_CHECK(GetReturnedAttributes(res, eventCategory, count, attributeIDs))
	SOCmnList<SOAeSAttribute> retAttrList(m_me->getReturnedAttributeList(eventCategory));

	if (!retAttrList)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, GetReturnedAttributes(res, eventCategory, count, attributeIDs))
		SOCmnListPosition pos;

	SOAeSAttribute* attr;
	DWORD i = 0;
	*count = retAttrList.getCount();

	if (*count > 0)
	{
		*attributeIDs = (DWORD*) CoTaskMemAlloc(*count * sizeof(DWORD));
		memset(*attributeIDs, 0, *count * sizeof(DWORD));

		if (*attributeIDs)
		{
			pos = retAttrList.getStartPosition();

			while (pos)
			{
				attr = (SOAeSAttribute*)retAttrList.getNext(pos);
				(*attributeIDs)[i] = attr->getNameInt();
				i++;
			}
		}
		else
		{
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(GetReturnedAttributes(res, eventCategory, count, attributeIDs))
} // GetReturnedAttributes


//-----------------------------------------------------------------------------
// Refresh (interface IOPCEventSubscriptionMgt)
//
// - refresh all active and inactive, unacknowledged conditions
//
// returns:
//		S_OK         - refresh started
//		OPC_E_BUSY   - another refresh in progress
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComSubscription::Refresh(
	IN DWORD connection)    // advise connection to refresh
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::Refresh, Refresh(connection))

	if (validConnection(connection))
	{
		SOCmnPointer<SOAeSRefresh> refresh;
		// check if another refresh is in progress
		refresh = m_me->createRefresh(connection);

		if (!refresh)
		{
			res = OPC_E_BUSY;
		}
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(Refresh(res, connection))
} // Refresh


//-----------------------------------------------------------------------------
// CancelRefresh (interface IOPCEventSubscriptionMgt)
//
// - cancel refresh operation
//
// returns:
//		S_OK         - refresh started
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComSubscription::CancelRefresh(
	IN DWORD connection)    // advise connection to cancel refresh
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::CancelRefresh, CancelRefresh(connection))

	if (validConnection(connection))
	{
		res = m_me->cancelRefresh(connection);
	}
	else
	{
		res = E_FAIL;
	}

	SOSRV_END_INTERFACE_FUNCTION(CancelRefresh(res, connection))
} // CancelRefresh


//-----------------------------------------------------------------------------
// validConnection
//
// - check if the connection exists
//
// returns:
//		TRUE  - valid connection
//		FALSE - invalid connection
//
BOOL SOAeSComSubscription::validConnection(
	IN DWORD connection)    // advise connection
{
	// check if valid connection
	IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>* p = this;
	BOOL conOK = FALSE;

	if (m_advisesSync.lock(0))
	{
#if _ATL_VER >= 0x0700
		// since ATL 7.0: cookie minus one is index into array
		conOK = ((connection != 0) && (p->m_vec.GetSize() >= (int)connection));
#else
		IUnknown** pp;
		pp = p->m_vec.begin();

		while (pp < p->m_vec.end())
		{
			// before: the pointer to the element was the cookie
			if ((DWORD)*pp == connection)
			{
				conOK = TRUE;
				break;
			}

			pp++;
		}

#endif
		m_advisesSync.unlock();
	}
	else
	{
		// can't get the advise lock. This is the case if the Refresh or CancelRefresh
		// is called from the OnEvent callback.
		// assume that the connection is OK. If not, no callback will be send.
		return TRUE;
	}

	return conOK;
} // validConnection


//-----------------------------------------------------------------------------
// GetState (interface IOPCEventSubscriptionMgt)
//
// - get the state (attributes) of the subscription
//
// returns:
//		S_OK         - get state
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComSubscription::GetState(
	OUT BOOL* active,                   // active
	OUT DWORD* bufferTime,              // buffer time
	OUT DWORD* maxSize,                 // max size
	OUT OPCHANDLE* clientSubscription)  // client handle
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::GetState, GetState(active, bufferTime, maxSize, clientSubscription))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(active)
	SOSRV_OUT_PARAM_PTR(bufferTime)
	SOSRV_OUT_PARAM_PTR(maxSize)
	SOSRV_OUT_PARAM_PTR(clientSubscription)
	SOSRV_END_PARAMS_CHECK(GetState(res, active, bufferTime, maxSize, clientSubscription))

	if (active)
	{
		*active = m_me->getActive();
	}

	if (bufferTime)
	{
		*bufferTime = m_me->getBufferTime();
	}

	if (maxSize)
	{
		*maxSize = m_me->getMaxSize();
	}

	if (clientSubscription)
	{
		*clientSubscription = m_me->getClientHandle();
	}

	SOSRV_END_INTERFACE_FUNCTION(GetState(res, active, bufferTime, maxSize, clientSubscription))
} // GetState


//-----------------------------------------------------------------------------
// SetState (interface IOPCEventSubscriptionMgt)
//
// - set the state (attributes) of the subscription
//
// returns:
//		S_OK                    - set state
//		OPC_S_INVALIDBUFFERTIME - invalid buffer time
//		OPC_S_INVALIDMAXSIZE    - invalid max size
//		E_INVALIDARG            - invalid argument
//		E_FAIL                  - operation failed
//
STDMETHODIMP SOAeSComSubscription::SetState(
	IN BOOL* active,                    // active
	IN DWORD* bufferTime,               // buffer time
	IN DWORD* maxSize,                  // max size
	IN OPCHANDLE clientSubscription,    // client handle
	OUT DWORD* revisedBufferTime,       // revised buffer time
	OUT DWORD* revisedMaxSize)          // revised max size
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventSubscriptionMgt::SetState, SetState(active, bufferTime, maxSize, clientSubscription, revisedBufferTime, revisedMaxSize))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(revisedBufferTime)
	SOSRV_OUT_PARAM_PTR(revisedMaxSize)
	SOSRV_END_PARAMS_CHECK(SetState(res, active, bufferTime, maxSize, clientSubscription, revisedBufferTime, revisedMaxSize))

	// set the parameter
	if (active)
	{
		m_me->setActive(*active);
	}

	if (bufferTime)
	{
		m_me->setBufferTime(*bufferTime);
	}

	if (maxSize)
	{
		m_me->setMaxSize(*maxSize);
	}

	m_me->setClientHandle(clientSubscription);

	// fill the output parameter
	if (revisedBufferTime)
	{
		*revisedBufferTime = m_me->getBufferTime();

		if (bufferTime)
			if (*revisedBufferTime != *bufferTime)
			{
				res = OPC_S_INVALIDBUFFERTIME;
			}
	}

	if (revisedMaxSize)
	{
		*revisedMaxSize = m_me->getMaxSize();

		if (maxSize)
			if (*revisedMaxSize != *maxSize)
			{
				res = OPC_S_INVALIDMAXSIZE;
			}
	}

	SOSRV_END_INTERFACE_FUNCTION(SetState(res, active, bufferTime, maxSize, clientSubscription, revisedBufferTime, revisedMaxSize))
} // SetState


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
STDMETHODIMP SOAeSComSubscription::Advise(
	IN IUnknown* unknown,   // client sink
	OUT DWORD* cookie)      // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IConnectionPoint::Advise, Advise(unknown, cookie))
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();

	if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
	{
		setProxyBlanket(unknown, entry->getAuthentificationLevel(),
						entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
	}

	if (m_advisesSync.lock(2000))
	{
		res = IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>::Advise(unknown, cookie);
		m_advisesSync.unlock();

		if (SUCCEEDED(res) && (entry->isInitialized(SOSRVENTRY_INIT_SECURITY)))
		{
			IOPCEventSink* eventSink;
			res = unknown->QueryInterface(IID_IOPCEventSink, (void**)&eventSink);

			if (SUCCEEDED(res))
			{
				setProxyBlanket(eventSink, entry->getAuthentificationLevel(),
								entry->getAuthentificationLevelForAdvise(), entry->getImpersonationLevelForAdvise());
				eventSink->Release();
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
STDMETHODIMP SOAeSComSubscription::Unadvise(
	IN DWORD cookie)    // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IConnectionPoint::Unadvise, Unadvise(cookie))

	if (m_advisesSync.lock(2000))
	{
		res = IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>::Unadvise(cookie);
		m_advisesSync.unlock();
	}
	else
	{
		res = E_PENDING;
	}

	SOSRV_END_INTERFACE_FUNCTION(Unadvise(res, cookie))
} // Unadvise


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
HRESULT SOAeSComSubscription::queryInterface(
	IN REFIID iid,      // interface id
	IN void** object)   // interface
{
	return m_me->queryInterface(iid, object);
} // queryInterface

#endif // SOFEATURE_DCOM

#endif // SOAES
