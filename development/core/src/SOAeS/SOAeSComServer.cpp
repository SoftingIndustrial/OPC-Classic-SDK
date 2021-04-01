#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_DCOM

#include "SOAeSEntry.h"
#include "SOAeSComServer.h"
#include "SOAeSComSubscription.h"
#include "SOAeSComAreaBrowser.h"
#include "SOAeSServer.h"
#include "SOCmnTrace.h"
#include "SOSrvComEnum.h"
#include <opcerror.h>
#include <opc_ae.h>


//-----------------------------------------------------------------------------
// SOAeSComServer                                                             |
//-----------------------------------------------------------------------------

SOAeSComServer::SOAeSComServer()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_me = NULL;
	m_serverInstance = 0;
	_TRACE_CREATE_COMOBJ();
}

SOAeSComServer::~SOAeSComServer()
{
	_TRACE_DELETE_COMOBJ();
	SOCmnPointer<SOAeSEntry> engine = ::getSOAeSEntry();
	SOCmnPointer<SOSrvObjectRoot> root = engine->getObjectRoot();
	// remove tree object from tree
	root->removeBranch(m_me);
	// release tree object
	m_me->release();
	lastCOMObjectActions();
}


//-----------------------------------------------------------------------------
// afterConstructorInit
//
// - all initialisation which may needs virtual methods
//
HRESULT SOAeSComServer::afterConstructorInit(void)
{
	SOCmnPointer<SOAeSEntry> engine = ::getSOAeSEntry();
	SOCmnPointer<SOSrvObjectRoot> root;
	SOCmnPointer<SOAeSCreator> creator;
	// create OPC tree object
	creator = engine->getCreator();
	m_me = creator->createServer(m_serverInstance);
	HRESULT result = m_me->setFlags(SOAESSERVER_FLAG_DCOM);
	m_me->m_me = this;

	if (result != S_OK)
	{
		m_me->disconnectClient();
		return result;
	}

	// add to tree
	root = engine->getObjectRoot();
	root->addBranch(m_me);
	return S_OK;
} // afterConstructorInit


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SOAeSComServer::lockMe(void)
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
void SOAeSComServer::unlockMe(void)
{
	if (m_me)
	{
		m_me->m_meSync.unlock();
	}
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite the SOAeSComServer object from the SOAeSServer object
//
void SOAeSComServer::disuniteMe(void)
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
LPCTSTR SOAeSComServer::getObjTraceId(void)
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
BOOL SOAeSComServer::setObjTraceId(
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
STDMETHODIMP SOAeSComServer::SetLocaleID(
	IN LCID dwLcid) // LCID
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCCommon::SetLocaleID, SetLocaleID(dwLcid))

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
STDMETHODIMP SOAeSComServer::GetLocaleID(
	OUT LCID* pdwLcid)  // LCID
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCCommon::GetLocaleID, GetLocaleID(pdwLcid))
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
STDMETHODIMP SOAeSComServer::QueryAvailableLocaleIDs(
	OUT DWORD* pdwCount,    // number of available locale ids
	OUT LCID** pdwLcid)     // list of the available locale ids
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCCommon::QueryAvailableLocaleIDs, QueryAvailableLocaleIDs(pdwCount, pdwLcid))
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
				*((*pdwLcid) + i) = (LCID)lcidList.getNext(pos);
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
STDMETHODIMP SOAeSComServer::GetErrorString(
	IN HRESULT dwError,     // error code
	OUT LPWSTR* ppString)   // error text
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCCommon::GetErrorString, GetErrorString(dwError, ppString))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(ppString)
	SOSRV_END_PARAMS_CHECK(GetErrorString(res, dwError, ppString))
	USES_CONVERSION;
	SOCmnString message;
	// get error string
	res = m_me->getErrorString(dwError, message);

	// copy error string
	if (SUCCEEDED(res))
	{
		*ppString = SOCmnString::cloneToWCHAR(message);

		if (!(*ppString))
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, GetErrorString(res, dwError, ppString))
		}

	SOSRV_END_INTERFACE_FUNCTION(GetErrorString(res, dwError, ppString))
} // GetErrorString


//-----------------------------------------------------------------------------
// SetClientName
//
// - set the name of the client
//
// returns:
//		S_OK
//
STDMETHODIMP SOAeSComServer::SetClientName(
	IN LPCWSTR szName)  // client name
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCCommon::SetClientName, SetClientName(szName))
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
// IOPCEventServer                                                            |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetStatus (interface IOPCEventServer)
//
// - get status of server
//
// returns:
//		S_OK          - return status
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::GetStatus(
	OUT OPCEVENTSERVERSTATUS** eventServerStatus)   // server status
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::GetStatus, GetStatus(eventServerStatus))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(eventServerStatus)
	SOSRV_END_PARAMS_CHECK(GetStatus(res, eventServerStatus))
	// allocate server sturcture
	*eventServerStatus = (OPCEVENTSERVERSTATUS*)CoTaskMemAlloc(sizeof(OPCEVENTSERVERSTATUS));

	if (*eventServerStatus == NULL)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, GetStatus(res, eventServerStatus))
		// fill structure
		res = m_me->getStatus(*eventServerStatus);

	if (FAILED(res))
	{
		CoTaskMemFree(*eventServerStatus);
		*eventServerStatus = NULL;
	}

	SOSRV_END_INTERFACE_FUNCTION(GetStatus(res, eventServerStatus))
} // GetStatus


//-----------------------------------------------------------------------------
// CreateEventSubscription (interface IOPCEventServer)
//
// - create event subscription object
//
// returns:
//		S_OK                    - created subscription
//		OPC_S_INVALIDBUFFERTIME - invalid buffer time
//		OPC_S_INVALIDMAXSIZE    - invalid max size
//		E_INVALIDARG            - invalid argument
//		E_OUTOFMEMORY           - can't allocate memory
//
STDMETHODIMP SOAeSComServer::CreateEventSubscription(
	IN BOOL active,
	IN DWORD bufferTime,
	IN DWORD maxSize,
	IN OPCHANDLE clientSubscriptionHandle,
	IN REFIID riid,
	OUT LPUNKNOWN* unknown,
	OUT DWORD* revisedBufferTime,
	OUT DWORD* revisedMaxSize)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::CreateEventSubscription, CreateEventSubscription(active, bufferTime, maxSize, clientSubscriptionHandle, riid, unknown, revisedBufferTime, revisedMaxSize))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(unknown)
	SOSRV_OUT_PARAM_PTR(revisedBufferTime)
	SOSRV_OUT_PARAM_PTR(revisedMaxSize)
	SOSRV_END_PARAMS_CHECK(CreateEventSubscription(res, active, bufferTime, maxSize, clientSubscriptionHandle, riid, unknown, revisedBufferTime, revisedMaxSize))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	SOCmnPointer<SOAeSSubscription> sus;
	// create tree group object
	res = m_me->createSubscription(active, bufferTime, maxSize, clientSubscriptionHandle,
								   (SOAeSSubscription**)sus, revisedBufferTime, revisedMaxSize);

	if (SUCCEEDED(res))
	{
		HRESULT resQI;
		// create COM subscription object
		SOSrvComObject<SOAeSComSubscription> *subscr;
		SOCMN_ALLOCATE_OBJECT(subscr, SOSrvComObject<SOAeSComSubscription>)
		subscr->init(this, sus);    // when subscr object is deleted it releases sus
		// query the requested interface
		resQI = subscr->QueryInterface(riid, (void**)unknown);

		if (FAILED(resQI))
		{
			// can't query the interface
			// delete both subscription objects
			m_me->removeLeaf(sus);
			delete subscr;
			res = resQI;
		}

#ifdef SOFEATURE_WATCH
		else if (m_me->getCollectChanges())
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchAddObject(m_me, sus);
		}

#endif
	}

	SOSRV_END_INTERFACE_FUNCTION(CreateEventSubscription(res, active, bufferTime, maxSize, clientSubscriptionHandle, riid, unknown, revisedBufferTime, revisedMaxSize))
} // CreateEventSubscription


//-----------------------------------------------------------------------------
// QueryAvailableFilters (interface IOPCEventServer)
//
// - return the supported filter types
//
// returns:
//		S_OK         - return filters
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOAeSComServer::QueryAvailableFilters(
	OUT DWORD* filterMask)  // supported filters
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QueryAvailableFilters, QueryAvailableFilters(filterMask))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(filterMask)
	SOSRV_END_PARAMS_CHECK(QueryAvailableFilters(res, filterMask))
	SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP
	*filterMask = m_me->queryAvailableFilters();
	SOSRV_END_INTERFACE_FUNCTION(QueryAvailableFilters(res, filterMask))
} // QueryAvailableFilters


//-----------------------------------------------------------------------------
// QueryEventCategories (interface IOPCEventServer)
//
// - query the available categories dependend from the event type
//
// returns:
//		S_OK          - return categories
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::QueryEventCategories(
	IN DWORD eventType,                 // event type
	OUT DWORD* count,                   // number of event categories
	OUT DWORD** eventCategories,        // category ids
	OUT LPWSTR** eventCategoryDescs)    // category names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QueryEventCategories, QueryEventCategories(eventType, count, eventCategories, eventCategoryDescs))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(eventCategories)
	SOSRV_OUT_PARAM_PTR(eventCategoryDescs)
	SOSRV_IN_PARAM((eventType > 0) && (eventType <= OPC_ALL_EVENTS))
	SOSRV_END_PARAMS_CHECK(QueryEventCategories(res, eventType, count, eventCategories, eventCategoryDescs))
	SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnListPosition pos;
	SOAeSCategory* cat;
	DWORD i = 0;
	BOOL outOfMemory = FALSE;
	SOCmnList<SOCmnElement> catList(eventSpace->getBranchList());
	*count = catList.getCount();

	if (*count > 0)
	{
		*eventCategories = (DWORD*) CoTaskMemAlloc(*count * sizeof(DWORD));
		memset(*eventCategories, 0, *count * sizeof(DWORD));
		*eventCategoryDescs = (LPWSTR*) CoTaskMemAlloc(*count * sizeof(LPWSTR));
		memset(*eventCategoryDescs, 0, *count * sizeof(LPWSTR));

		if ((!*eventCategories) || (!*eventCategoryDescs))
		{
			outOfMemory = TRUE;
		}

		pos = catList.getStartPosition();

		while ((pos) && (!outOfMemory))
		{
			cat = (SOAeSCategory*)catList.getNext(pos);

			if (cat->getEventType() & eventType)
			{
				// category id
				(*eventCategories)[i] = cat->getNameInt();
				// category description
				(*eventCategoryDescs)[i] = SOCmnString::cloneToWCHAR(cat->getDescription());

				if (!(*eventCategoryDescs)[i])
				{
					outOfMemory = TRUE;
				}

				i++;
			}
		}

		if (*count != i)
		{
			*count = i;

			if (*count > 0)
			{
				*eventCategories = (DWORD*)CoTaskMemRealloc(*eventCategories,  *count * sizeof(DWORD));
				*eventCategoryDescs = (LPWSTR*)CoTaskMemRealloc(*eventCategoryDescs,  *count * sizeof(LPWSTR));

				if ((eventCategories == NULL) || (eventCategoryDescs == NULL))
				{
					outOfMemory = TRUE;
				}
			}
			else
			{
				if (*eventCategories)
				{
					CoTaskMemFree(*eventCategories);
				}

				if (*eventCategoryDescs)
				{
					CoTaskMemFree(*eventCategoryDescs);
				}

				*eventCategories = NULL;
				*eventCategoryDescs = NULL;
			}
		}

		if (outOfMemory)
		{
			if (*eventCategories)
			{
				CoTaskMemFree(*eventCategories);
			}

			if (*eventCategoryDescs)
			{
				for (i = 0; i < *count; i++)
				{
					if ((*eventCategoryDescs)[i])
					{
						CoTaskMemFree((*eventCategoryDescs)[i]);
					}
				}

				CoTaskMemFree(*eventCategoryDescs);
			}

			*count = 0;
			*eventCategories = NULL;
			*eventCategoryDescs = NULL;
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QueryEventCategories(res, eventType, count, eventCategories, eventCategoryDescs))
} // QueryEventCategories


//-----------------------------------------------------------------------------
// QueryConditionNames (interface IOPCEventServer)
//
// - query the condition names of an event category
//
// returns:
//		S_OK          - return condition names
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::QueryConditionNames(
	IN DWORD eventCategory,         // event category
	OUT DWORD* count,               // number of condition names
	OUT LPWSTR** conditionNames)    // condition names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QueryConditionNames, QueryConditionNames(eventCategory, count, conditionNames))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(conditionNames)
	SOSRV_END_PARAMS_CHECK(QueryConditionNames(res, eventCategory, count, conditionNames))
	SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSCategory> cat;
	// get event category
	cat = eventSpace->findCategory(eventCategory);

	if (!cat)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, QueryConditionNames(res, eventCategory, count, conditionNames))
		if (cat->getEventType() != OPC_CONDITION_EVENT)
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, QueryConditionNames(res, eventCategory, count, conditionNames))
			SOCmnList<SOCmnElement> condClassList(cat->getBranchList());

	SOAeSConditionClass* condClass;
	SOCmnListPosition pos;
	DWORD i = 0;
	BOOL outOfMemory = FALSE;
	*count = condClassList.getCount();

	if (*count > 0)
	{
		*conditionNames = (LPWSTR*) CoTaskMemAlloc(*count * sizeof(LPWSTR));
		memset(*conditionNames, 0, *count * sizeof(LPWSTR));

		if (!*conditionNames)
		{
			outOfMemory = TRUE;
		}

		pos = condClassList.getStartPosition();

		while ((pos) && (!outOfMemory))
		{
			condClass = (SOAeSConditionClass*)condClassList.getNext(pos);
			// condition name
			(*conditionNames)[i] = SOCmnString::cloneToWCHAR(condClass->getName());

			if (!(*conditionNames)[i])
			{
				outOfMemory = TRUE;
			}

			i++;
		}

		if (outOfMemory)
		{
			if (*conditionNames)
			{
				for (i = 0; i < *count; i++)
				{
					if ((*conditionNames)[i])
					{
						CoTaskMemFree((*conditionNames)[i]);
					}
				}

				CoTaskMemFree(*conditionNames);
			}

			*count = 0;
			*conditionNames = NULL;
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QueryConditionNames(res, eventCategory, count, conditionNames))
} // QueryConditionNames


//-----------------------------------------------------------------------------
// QuerySubConditionNames (interface IOPCEventServer)
//
// - query the subcondition names of a condtion
//
// returns:
//		S_OK          - return condition names
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::QuerySubConditionNames(
	IN LPWSTR conditionName,        // condition name
	OUT DWORD* count,               // number of subcondition names
	OUT LPWSTR** subConditionNames) // subcondition names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QuerySubConditionNames, QuerySubConditionNames(conditionName, count, subConditionNames))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(subConditionNames)
	SOSRV_END_PARAMS_CHECK(QuerySubConditionNames(res, conditionName, count, subConditionNames))
	SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP
	USES_CONVERSION;
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSConditionClass> cond;
	SOCmnString name(W2CT(conditionName));
	// get event category
	cond = eventSpace->findConditionClass(name);

	if (!cond)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, QuerySubConditionNames(res, conditionName, count, subConditionNames))
		SOCmnList<SOCmnElement> subcondClassList;

	SOCmnList<SOCmnElement> *pLeafList = cond->getLeafList();

	if (pLeafList)
	{
		// multiple state condition
		subcondClassList = pLeafList;
		*count = subcondClassList.getCount();
	}
	else
	{
		// single state conditions
		subcondClassList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
		subcondClassList.add(cond);
		*count = 1;
	}

	if (*count > 0)
	{
		SOCmnElement* subcondClass;
		SOCmnListPosition pos;
		DWORD i = 0;
		BOOL outOfMemory = FALSE;
		*subConditionNames = (LPWSTR*) CoTaskMemAlloc(*count * sizeof(LPWSTR));
		memset(*subConditionNames, 0, *count * sizeof(LPWSTR));

		if (!*subConditionNames)
		{
			outOfMemory = TRUE;
		}

		pos = subcondClassList.getStartPosition();

		while ((pos) && (!outOfMemory))
		{
			subcondClass = subcondClassList.getNext(pos);
			// subcondition names
			(*subConditionNames)[i] = SOCmnString::cloneToWCHAR(subcondClass->getName());

			if (!(*subConditionNames)[i])
			{
				outOfMemory = TRUE;
			}

			i++;
		}

		if (outOfMemory)
		{
			if (*subConditionNames)
			{
				for (i = 0; i < *count; i++)
				{
					if ((*subConditionNames)[i])
					{
						CoTaskMemFree((*subConditionNames)[i]);
					}
				}

				CoTaskMemFree(*subConditionNames);
			}

			*count = 0;
			*subConditionNames = NULL;
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QuerySubConditionNames(res, conditionName, count, subConditionNames))
} // QuerySubConditionNames


//-----------------------------------------------------------------------------
// QuerySourceConditions (interface IOPCEventServer)
//
// - query the condition names of a source
//
// returns:
//		S_OK          - return condition names
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::QuerySourceConditions(
	IN LPWSTR sourceN,              // fully qualified source name
	OUT DWORD* count,               // number of conditions
	OUT LPWSTR** conditionNames)    // condition names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QuerySourceConditions, QuerySourceConditions(sourceN, count, conditionNames))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(conditionNames)
	SOSRV_END_PARAMS_CHECK(QuerySourceConditions(res, sourceN, count, conditionNames))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	USES_CONVERSION;
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpace = entry->getAreaSpaceRoot();
	SOCmnPointer<SOAeSSource> source;
	SOCmnString sourceName(W2CT(sourceN));
	SOCmnList<SOCmnElement> condList;
	// find source object
	source = (SOAeSSource*)areaSpace->findLeaf(sourceName, TRUE);

	if (!source)
	{
		// check source name if no object exists
		if (!m_me->isSourceName(sourceName))
		{
			// invalid source name
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, QuerySourceConditions(res, sourceN, count, conditionNames))
		}
	}
	else
	{
		condList = source->getAdditionalList();
	}

	SOCmnStringList condNameList;
	condNameList.create();
	m_me->getConditionNamesOfSourceName(sourceName, condNameList);
	*count = condNameList.getCount();

	if (!!condList)
	{
		*count += condList.getCount();
	}

	if (*count > 0)
	{
		SOAeSCondition* cond;
		SOCmnPointer<SOAeSConditionClass> condClass;
		SOCmnListPosition pos;
		DWORD i = 0;
		BOOL outOfMemory = FALSE;
		*conditionNames = (LPWSTR*) CoTaskMemAlloc(*count * sizeof(LPWSTR));
		memset(*conditionNames, 0, *count * sizeof(LPWSTR));

		if (!*conditionNames)
		{
			outOfMemory = TRUE;
		}

		// names of condition objects
		if (!!condList)
		{
			pos = condList.getStartPosition();

			while ((pos) && (!outOfMemory))
			{
				cond = (SOAeSCondition*)condList.getNext(pos);
				condClass = cond->getConditionClass();

				if (!!condClass)
				{
					// condition name is the name of the condition class
					(*conditionNames)[i] = SOCmnString::cloneToWCHAR(condClass->getName());

					if (!(*conditionNames)[i])
					{
						outOfMemory = TRUE;
					}

					i++;
				}
			}
		}

		// condition names
		pos = condNameList.getStartPosition();

		while ((pos) && (!outOfMemory))
		{
			(*conditionNames)[i] = SOCmnString::cloneToWCHAR(condNameList.getNext(pos));

			if (!(*conditionNames)[i])
			{
				outOfMemory = TRUE;
			}

			i++;
		}

		if (outOfMemory)
		{
			if (*conditionNames)
			{
				for (i = 0; i < *count; i++)
				{
					if ((*conditionNames)[i])
					{
						CoTaskMemFree((*conditionNames)[i]);
					}
				}

				CoTaskMemFree(*conditionNames);
			}

			*count = 0;
			*conditionNames = NULL;
			SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, QuerySourceConditions(res, sourceN, count, conditionNames))
		}

		if (*count > i)
		{
			*count = i;
			*conditionNames = (LPWSTR*)CoTaskMemRealloc(*conditionNames,  *count * sizeof(LPWSTR));
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QuerySourceConditions(res, sourceN, count, conditionNames))
} // QuerySourceConditions


//-----------------------------------------------------------------------------
// QueryEventAttributes (interface IOPCEventServer)
//
// - query the available attributes of categories
//
// returns:
//		S_OK          - return categories
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::QueryEventAttributes(
	IN DWORD eventCategory,     // category
	OUT DWORD* count,           // number of attributes
	OUT DWORD** attrIDs,        // attribute IDs
	OUT LPWSTR** attrDescs,     // attribute descriptions
	OUT VARTYPE** attrTypes)    // attribute datatypes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::QueryEventAttributes, QueryEventAttributes(eventCategory, count, attrIDs, attrDescs, attrTypes))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(count)
	SOSRV_OUT_PARAM_PTR(attrIDs)
	SOSRV_OUT_PARAM_PTR(attrDescs)
	SOSRV_OUT_PARAM_PTR(attrTypes)
	SOSRV_END_PARAMS_CHECK(QueryEventAttributes(res, eventCategory, count, attrIDs, attrDescs, attrTypes))
	SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSCategory> cat;
	// get event category
	cat = eventSpace->findCategory(eventCategory);

	if (!cat)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, QueryEventAttributes(res, eventCategory, count, attrIDs, attrDescs, attrTypes))
		SOCmnList<SOCmnElement> attrList(cat->getAdditionalList());

	SOAeSAttribute* attr;
	SOCmnListPosition pos;
	DWORD i = 0;
	BOOL outOfMemory = FALSE;
	*count = attrList.getCount();

	if (*count > 0)
	{
		*attrIDs = (DWORD*) CoTaskMemAlloc(*count * sizeof(DWORD));
		memset(*attrIDs, 0, *count * sizeof(DWORD));
		*attrDescs = (LPWSTR*) CoTaskMemAlloc(*count * sizeof(LPWSTR));
		memset(*attrDescs, 0, *count * sizeof(LPWSTR));
		*attrTypes = (VARTYPE*) CoTaskMemAlloc(*count * sizeof(VARTYPE));
		memset(*attrTypes, 0, *count * sizeof(VARTYPE));

		if ((!*attrIDs) || (!*attrDescs) || (!*attrTypes))
		{
			outOfMemory = TRUE;
		}

		pos = attrList.getStartPosition();

		while ((pos) && (!outOfMemory))
		{
			attr = (SOAeSAttribute*)attrList.getNext(pos);
			// attribute id
			(*attrIDs)[i] = attr->getNameInt();
			// attribute description
			(*attrDescs)[i] = SOCmnString::cloneToWCHAR(attr->getDescription());

			if (!(*attrDescs)[i])
			{
				outOfMemory = TRUE;
			}

			// attribute datatype
			(*attrTypes)[i] = attr->getDatatype();
			i++;
		}

		if (outOfMemory)
		{
			if (*attrIDs)
			{
				CoTaskMemFree(*attrIDs);
			}

			if (*attrDescs)
			{
				for (i = 0; i < *count; i++)
				{
					if ((*attrDescs)[i])
					{
						CoTaskMemFree((*attrDescs)[i]);
					}
				}

				CoTaskMemFree(*attrDescs);
			}

			if (*attrTypes)
			{
				CoTaskMemFree(*attrTypes);
			}

			*count = 0;
			*attrIDs = NULL;
			*attrDescs = NULL;
			*attrTypes = NULL;
			res = E_OUTOFMEMORY;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(QueryEventAttributes(res, eventCategory, count, attrIDs, attrDescs, attrTypes))
} // QueryEventAttributes


//-----------------------------------------------------------------------------
// TranslateToItemIDs (interface IOPCEventServer)
//
// - get the OPC DA item IDs for the attributes of a (sub)condition
//
// returns:
//		S_OK          - return condition state
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//		E_NOTIMPL     - not implemented
//
STDMETHODIMP SOAeSComServer::TranslateToItemIDs(
	IN LPWSTR source,           // source name
	IN DWORD eventCategory,     // category id
	IN LPWSTR conditionName,    // condition name
	IN LPWSTR subconditionName, // subcondition name
	IN DWORD count,             // attribute count
	IN DWORD* assocAttrIDs,     // attribute ids
	OUT LPWSTR** attrItemIDs,   // item ids
	OUT LPWSTR** nodeNames,     // node names
	OUT CLSID** clsids)         // server class ids
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::TranslateToItemIDs, TranslateToItemIDs(source, eventCategory, conditionName, subconditionName, count, assocAttrIDs, attrItemIDs, nodeNames, clsids))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(attrItemIDs)
	SOSRV_OUT_PARAM_PTR(nodeNames)
	SOSRV_OUT_PARAM_PTR(clsids)
	SOSRV_IN_PARAM(count > 0)
	SOSRV_IN_PARAM(!((count > 0) && (assocAttrIDs == NULL)))
	SOSRV_END_PARAMS_CHECK(TranslateToItemIDs(res, source, eventCategory, conditionName, subconditionName, count, assocAttrIDs, attrItemIDs, nodeNames, clsids))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP

	// check if server should support this fuction
	if (!m_me->supportTranslateToItemIDs())
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_NOTIMPL, TranslateToItemIDs(res, source, eventCategory, conditionName, subconditionName, count, assocAttrIDs, attrItemIDs, nodeNames, clsids))
		// get event category
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();

	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSCategory> cat;
	DWORD i;
	cat = eventSpace->findCategory(eventCategory);

	if (!cat)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_INVALIDARG, TranslateToItemIDs(res, source, eventCategory, conditionName, subconditionName, count, assocAttrIDs, attrItemIDs, nodeNames, clsids))
		// check attribute ids
		SOCmnList<SOAeSAttribute> attrList = (SOCmnList<SOAeSAttribute> *)cat->getAdditionalList();

	SOCmnList<SOAeSAttributeValueDADescription> descrDAList;
	SOAeSAttributeValueDADescription* descrDA = NULL;
	DWORD attrID = 0xFFFFFFFF;
	SOCmnListPosition pos;
	SOCmnListPosition actPos = NULL;
	BOOL outOfMemory = FALSE;
	descrDAList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);

	for (i = 0; i < count; i++)
	{
		if (attrList.findKey(assocAttrIDs[i], NULL))
		{
			SOCMN_ALLOCATE_OBJECT(descrDA, SOAeSAttributeValueDADescription())
			descrDA->m_attrID = assocAttrIDs[i];
			descrDAList.add(descrDA);
		}
	}

	// fill DA description list
	if (!descrDAList.isEmpty())
	{
		descrDAList.autoUnlock();
		// look for the condition object
		USES_CONVERSION;
		SOCmnString srcName(W2CT(source));
		SOCmnString condName(W2CT(conditionName));
		SOCmnString subCondName(W2CT(subconditionName));
		SOCmnPointer<SOAeSAreaSpaceRoot> root = entry->getAreaSpaceRoot();
		SOCmnPointer<SOAeSSource> src = (SOAeSSource*)root->findLeaf(srcName, TRUE);
		SOCmnPointer<SOAeSCondition> cond;

		if (!!src)
		{
			// source object exists
			cond = (SOAeSCondition*)src->findAdditionalElement(condName);
		}

		if (!!cond)
		{
			res = cond->translateToItemIDs(subCondName, &descrDAList);
		}
		else
		{
			// havn't found condition object
			res = m_me->translateToItemIDsByName(srcName, cat, condName, subCondName, &descrDAList);
		}
	}

	if (SUCCEEDED(res))
	{
		// fill output data
		*attrItemIDs = (LPWSTR*)CoTaskMemAlloc(count * sizeof(LPWSTR));
		*nodeNames = (LPWSTR*)CoTaskMemAlloc(count * sizeof(LPWSTR));
		*clsids = (CLSID*)CoTaskMemAlloc(count * sizeof(CLSID));

		if ((!(*attrItemIDs)) || (!(*nodeNames)) || (!(*clsids)))
		{
			outOfMemory = TRUE;
		}

		pos = descrDAList.getStartPosition();

		if (pos)
		{
			actPos = pos;
			descrDA = descrDAList.getNext(pos);
			attrID = descrDA->m_attrID;
		}

		for (i = 0; ((i < count) && (!outOfMemory)); i++)
		{
			if ((actPos) && (attrID == assocAttrIDs[i]))
			{
				(*attrItemIDs)[i] = SOCmnString::cloneToWCHAR(descrDA->m_itemID);
				(*nodeNames)[i] = SOCmnString::cloneToWCHAR(descrDA->m_nodeName);
				memcpy(&(*clsids)[i], &descrDA->m_clsid, sizeof(CLSID));
				actPos = pos;

				if (pos)
				{
					descrDA = descrDAList.getNext(pos);
					attrID = descrDA->m_attrID;
				}
			}
			else
			{
				// fill with empty values
				(*attrItemIDs)[i] = (LPWSTR)CoTaskMemAlloc(sizeof(WCHAR));

				if ((*attrItemIDs)[i])
				{
					memset((*attrItemIDs)[i], 0, sizeof(WCHAR));
				}

				(*nodeNames)[i] = (LPWSTR)CoTaskMemAlloc(sizeof(WCHAR));

				if ((*nodeNames)[i])
				{
					memset((*nodeNames)[i], 0, sizeof(WCHAR));
				}

				memset(&(*clsids)[i], 0, sizeof(CLSID));
			}

			if ((!(*attrItemIDs)[i]) || (!(*nodeNames)[i]))
			{
				outOfMemory = TRUE;
			}
		}

		if (outOfMemory)
		{
			if (*attrItemIDs)
			{
				for (i = 0; i < count; i++)
					if ((*attrItemIDs)[i])
					{
						CoTaskMemFree((*attrItemIDs)[i]);
					}

				CoTaskMemFree(*attrItemIDs);
			}

			if (*nodeNames)
			{
				for (i = 0; i < count; i++)
					if ((*nodeNames)[i])
					{
						CoTaskMemFree((*nodeNames)[i]);
					}

				CoTaskMemFree(*nodeNames);
			}

			if (*clsids)
			{
				CoTaskMemFree(*clsids);
			}

			*attrItemIDs = NULL;
			*nodeNames = NULL;
			*clsids = NULL;
		}
	}

	// free list
	descrDAList.destroy();
	SOSRV_END_INTERFACE_FUNCTION(TranslateToItemIDs(res, source, eventCategory, conditionName, subconditionName, count, assocAttrIDs, attrItemIDs, nodeNames, clsids))
} // TranslateToItemIDs


//-----------------------------------------------------------------------------
// fillConditionState
//
// - fill the condition state struct with the condition attributes
//
// returns:
//		TRUE  - struct filled
//		FALSE - out of memory
//
BOOL SOAeSComServer::fillConditionState(
	IN SOAeSCondition* cond,                // condition
	IN DWORD numEventAttrs,                 // count of attributes to return
	IN DWORD* attributeIDs,                 // ids of attributes to return
	OUT OPCCONDITIONSTATE* conditionState)  // condition state struct
{
	USES_CONVERSION;
	SOCmnPointer<SOAeSSubConditionClass> actSubCond;
	SOCmnPointer<SOAeSConditionClass> condClass = cond->getConditionClass();
	DWORD i;
	memset(conditionState, 0, sizeof(OPCCONDITIONSTATE));
	// fill output structure
	conditionState->wState = cond->getObjectState();
	actSubCond = cond->getActSubConditionClass();

	if (((conditionState->wState & OPC_CONDITION_ACTIVE) != 0))
	{
		// active
		if (!!actSubCond)
		{
			conditionState->szActiveSubCondition = SOCmnString::cloneToWCHAR(actSubCond->getName());
			conditionState->szASCDefinition = SOCmnString::cloneToWCHAR(actSubCond->getDefinition());
		}
		else
		{
			conditionState->szActiveSubCondition = SOCmnString::cloneToWCHAR(condClass->getName());
			conditionState->szASCDefinition = SOCmnString::cloneToWCHAR(condClass->getDefinition());
		}

		if ((!conditionState->szActiveSubCondition) || (!conditionState->szASCDefinition))
		{
			return FALSE;
		}

		conditionState->dwASCSeverity = cond->getSeverity();
		conditionState->szASCDescription = SOCmnString::cloneToWCHAR(cond->getMessage());

		if (!conditionState->szASCDescription)
		{
			return FALSE;
		}
	}

	conditionState->wQuality = cond->getQuality();
	cond->getAckTime(&conditionState->ftLastAckTime);
	cond->getActSubConditionTime(&conditionState->ftSubCondLastActive);
	cond->getActiveTime(&conditionState->ftCondLastActive);
	cond->getInactiveTime(&conditionState->ftCondLastInactive);

	if (!cond->getAckID().isEmpty())
	{
		conditionState->szAcknowledgerID = SOCmnString::cloneToWCHAR(cond->getAckID());

		if (!conditionState->szAcknowledgerID)
		{
			return FALSE;
		}
	}

	if (!cond->getAckComment().isEmpty())
	{
		conditionState->szComment = SOCmnString::cloneToWCHAR(cond->getAckComment());

		if (!conditionState->szComment)
		{
			return FALSE;
		}
	}

	// subcondition descriptions
	BOOL isMultiple = FALSE;
	SOCmnList<SOAeSSubConditionClass> *pSubCondList = (SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList();

	if (pSubCondList != NULL)
	{
		isMultiple = !pSubCondList->isEmpty();
	}

	if (isMultiple)
	{
		// multiple state condition
		SOCmnList<SOAeSSubConditionClass> subCondList = pSubCondList;
		SOAeSSubConditionClass* subCondClass;
		SOCmnListPosition pos;
		DWORD subCondCnt;
		pos = subCondList.getStartPosition();
		subCondCnt = subCondList.getCount();
		conditionState->dwNumSCs = subCondCnt;
		conditionState->pszSCNames = (LPWSTR*)CoTaskMemAlloc(subCondCnt * sizeof(LPWSTR));

		if (!conditionState->pszSCNames)
		{
			return FALSE;
		}

		conditionState->pszSCDefinitions = (LPWSTR*)CoTaskMemAlloc(subCondCnt * sizeof(LPWSTR));

		if (!conditionState->pszSCDefinitions)
		{
			return FALSE;
		}

		conditionState->pszSCDescriptions = (LPWSTR*)CoTaskMemAlloc(subCondCnt * sizeof(LPWSTR));

		if (!conditionState->pszSCDescriptions)
		{
			return FALSE;
		}

		conditionState->pdwSCSeverities = (DWORD*)CoTaskMemAlloc(subCondCnt * sizeof(DWORD));

		if (!conditionState->pdwSCSeverities)
		{
			return FALSE;
		}

		i = 0;

		while (pos)
		{
			subCondClass = subCondList.getNext(pos);
			conditionState->pszSCNames[i] = SOCmnString::cloneToWCHAR(subCondClass->getName());

			if (!conditionState->pszSCNames[i])
			{
				return FALSE;
			}

			conditionState->pszSCDefinitions[i] = SOCmnString::cloneToWCHAR(subCondClass->getDefinition());

			if (!conditionState->pszSCDefinitions[i])
			{
				return FALSE;
			}

			conditionState->pszSCDescriptions[i] = SOCmnString::cloneToWCHAR(subCondClass->getDescription());

			if (!conditionState->pszSCDescriptions[i])
			{
				return FALSE;
			}

			conditionState->pdwSCSeverities[i] = subCondClass->getSeverity();
			i++;
		}
	}
	else
	{
		// single state condition
		conditionState->dwNumSCs = 1;
		conditionState->pszSCNames = (LPWSTR*)CoTaskMemAlloc(sizeof(LPWSTR));

		if (!conditionState->pszSCNames)
		{
			return FALSE;
		}

		conditionState->pszSCDefinitions = (LPWSTR*)CoTaskMemAlloc(sizeof(LPWSTR));

		if (!conditionState->pszSCDefinitions)
		{
			return FALSE;
		}

		conditionState->pszSCDescriptions = (LPWSTR*)CoTaskMemAlloc(sizeof(LPWSTR));

		if (!conditionState->pszSCDescriptions)
		{
			return FALSE;
		}

		conditionState->pdwSCSeverities = (DWORD*)CoTaskMemAlloc(sizeof(DWORD));

		if (!conditionState->pdwSCSeverities)
		{
			return FALSE;
		}

		conditionState->pszSCNames[0] = SOCmnString::cloneToWCHAR(condClass->getName());

		if (!conditionState->pszSCNames[0])
		{
			return FALSE;
		}

		conditionState->pszSCDefinitions[0] = SOCmnString::cloneToWCHAR(condClass->getDefinition());

		if (!conditionState->pszSCDefinitions[0])
		{
			return FALSE;
		}

		conditionState->pszSCDescriptions[0] = SOCmnString::cloneToWCHAR(condClass->getActiveMessage());

		if (!conditionState->pszSCDescriptions[0])
		{
			return FALSE;
		}

		conditionState->pdwSCSeverities[0] = condClass->getActiveSeverity();
	}

	// attributes
	SOCmnList<SOAeSAttributeValue> attrValList = cond->getAttributeValueList();
	SOCmnPointer<SOAeSAttributeValue> attrVal;
	conditionState->pEventAttributes = (VARIANT*)CoTaskMemAlloc(numEventAttrs * sizeof(VARIANT));

	if (!conditionState->pEventAttributes)
	{
		return FALSE;
	}

	conditionState->pErrors = (HRESULT*)CoTaskMemAlloc(numEventAttrs * sizeof(HRESULT));

	if (!conditionState->pErrors)
	{
		return FALSE;
	}

	conditionState->dwNumEventAttrs = numEventAttrs;

	for (i = 0; i < numEventAttrs; i++)
	{
		attrVal = attrValList.findKey(attributeIDs[i]);

		if (!!attrVal)
		{
			attrVal->getValue(&conditionState->pEventAttributes[i]);
			conditionState->pErrors[i] = S_OK;
		}
		else
		{
			::VariantInit(&conditionState->pEventAttributes[i]);
			conditionState->pErrors[i] = E_FAIL;
		}
	}

	return TRUE;
} // fillConditionState


//-----------------------------------------------------------------------------
// GetConditionState (interface IOPCEventServer)
//
// - get the actual state of the condition
//
// returns:
//		S_OK          - return condition state
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//		E_NOTIMPL     - not implemented
//		OPC_E_NOINFO  - currently no info
//
STDMETHODIMP SOAeSComServer::GetConditionState(
	IN LPWSTR source,                       // source name
	IN LPWSTR conditionName,                // condition name
	IN DWORD numEventAttrs,                 // count of attributes to return
	IN DWORD* attributeIDs,                 // ids of attributes to return
	OUT OPCCONDITIONSTATE** conditionState) // condition state
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::GetConditionState, GetConditionState(source, conditionName, numEventAttrs, attributeIDs, conditionState))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(conditionState)
	SOSRV_IN_PARAM(!((numEventAttrs > 0) && (attributeIDs == NULL)))
	SOSRV_END_PARAMS_CHECK(GetConditionState(res, source, conditionName, numEventAttrs, attributeIDs, conditionState))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP

	// check if server should support this fuction
	if (!m_me->supportGetConditionState())
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_NOTIMPL, GetConditionState(res, source, conditionName, numEventAttrs, attributeIDs, conditionState))
		// look for the condition object
		USES_CONVERSION;

	SOCmnString srcName(W2CT(source));
	SOCmnString condName(W2CT(conditionName));
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> root = entry->getAreaSpaceRoot();
	SOCmnPointer<SOAeSSource> src = (SOAeSSource*)root->findLeaf(srcName, TRUE);
	SOCmnPointer<SOAeSCondition> cond;

	if (!!src)
	{
		// source object exists
		cond = (SOAeSCondition*)src->findAdditionalElement(condName);
	}

	if (!cond)
	{
		// havn't found condition object
		// give the application the change to return the condition state
		res = m_me->createConditionByName(srcName, condName, &cond);
	}

	if (SUCCEEDED(res))
	{
		if (!!cond)
		{
			// condition object exists
			*conditionState = (OPCCONDITIONSTATE*) CoTaskMemAlloc(sizeof(OPCCONDITIONSTATE));

			if (*conditionState)
			{
				if (!fillConditionState(cond, numEventAttrs, attributeIDs, *conditionState))
				{
					DWORD i;
					res = E_OUTOFMEMORY;

					// free memory
					if ((*conditionState)->szActiveSubCondition)
					{
						CoTaskMemFree((*conditionState)->szActiveSubCondition);
					}

					if ((*conditionState)->szASCDefinition)
					{
						CoTaskMemFree((*conditionState)->szASCDefinition);
					}

					if ((*conditionState)->szASCDescription)
					{
						CoTaskMemFree((*conditionState)->szASCDescription);
					}

					if ((*conditionState)->szAcknowledgerID)
					{
						CoTaskMemFree((*conditionState)->szAcknowledgerID);
					}

					if ((*conditionState)->szComment)
					{
						CoTaskMemFree((*conditionState)->szComment);
					}

					if ((*conditionState)->pszSCNames)
					{
						for (i = 0; i < (*conditionState)->dwNumSCs; i ++)
							if ((*conditionState)->pszSCNames[i])
							{
								CoTaskMemFree((*conditionState)->pszSCNames[i]);
							}

						CoTaskMemFree((*conditionState)->pszSCNames);
					}

					if ((*conditionState)->pszSCDefinitions)
					{
						for (i = 0; i < (*conditionState)->dwNumSCs; i ++)
							if ((*conditionState)->pszSCDefinitions[i])
							{
								CoTaskMemFree((*conditionState)->pszSCDefinitions[i]);
							}

						CoTaskMemFree((*conditionState)->pszSCDefinitions);
					}

					if ((*conditionState)->pszSCDescriptions)
					{
						for (i = 0; i < (*conditionState)->dwNumSCs; i ++)
							if ((*conditionState)->pszSCDescriptions[i])
							{
								CoTaskMemFree((*conditionState)->pszSCDescriptions[i]);
							}

						CoTaskMemFree((*conditionState)->pszSCDescriptions);
					}

					if ((*conditionState)->pEventAttributes)
					{
						for (i = 0; i < (*conditionState)->dwNumEventAttrs; i ++)
						{
							SOCmnVariant::variantClear(&(*conditionState)->pEventAttributes[i]);
						}

						CoTaskMemFree((*conditionState)->pEventAttributes);
					}

					if ((*conditionState)->pErrors)
					{
						CoTaskMemFree((*conditionState)->pErrors);
					}

					CoTaskMemFree(*conditionState);
					*conditionState = NULL;
				}
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
	}

	SOSRV_END_INTERFACE_FUNCTION(GetConditionState(res, source, conditionName, numEventAttrs, attributeIDs, conditionState))
} // GetConditionState


//-----------------------------------------------------------------------------
// enableConditions
//
// - en/disable conditions of some areas or sources
//
// returns:
//		S_OK          - condition enabled
//		E_INVALIDARG  - invalid argument
//		E_NOTIMPL     - not implemented
//
HRESULT SOAeSComServer::enableConditions(
	IN BOOL enable,         // enable or disable
	IN BOOL areaOrSource,   // area or source
	IN DWORD num,           // number of areas
	IN LPWSTR* names)       // area names
{
	// check if server should support this fuction
	if (!m_me->supportEnableCondition(enable, areaOrSource))
	{
		return E_NOTIMPL;
	}

	USES_CONVERSION;
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> root = entry->getAreaSpaceRoot();
	SOCmnList<SOAeSCondition> allCondList;
	SOAeSCondition* cond;
	SOCmnListPosition condPos;
	SOCmnString name;
	DWORD i;
	allCondList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

	if (areaOrSource)
	{
		SOCmnPointer<SOAeSArea> area;

		// all area names
		for (i = 0; i < num; i++)
		{
			name = W2CT(names[i]);
			area = (SOAeSArea*)root->findBranch(name, TRUE);

			if ((SOAeSArea*)area)
			{
				// area object exists
				SOAeSAreaSpaceRoot::getConditions(area, allCondList);
			}
			else
			{
				if (!m_me->isAreaName(name))
				{
					_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("enableConditions: called with invalid area name")));
					return E_INVALIDARG;
				}
			}
		}
	}
	else
	{
		SOCmnPointer<SOAeSSource> source;
		SOCmnList<SOAeSCondition> condList;

		// all source names
		for (i = 0; i < num; i++)
		{
			name = W2CT(names[i]);
			source = (SOAeSSource*)root->findLeaf(name, TRUE);

			if ((SOAeSSource*)source)
			{
				// source object exists
				condList = (SOCmnList<SOAeSCondition> *)source->getAdditionalList();

				if (!!condList)
				{
					condPos = condList.getStartPosition();

					while (condPos)
					{
						cond = condList.getNext(condPos);
						allCondList.add(cond);
					}
				}
			}
			else
			{
				if (!m_me->isSourceName(name))
				{
					_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_CALL, (_T("enableConditions: called with invalid source name")));
					return E_INVALIDARG;
				}
			}
		}

		condList.autoUnlock();
	}

	// all area or source names are valid
	// enable all condition objects
	condPos = allCondList.getStartPosition();

	while (condPos)
	{
		cond = allCondList.getNext(condPos);
		cond->enable(enable);
	}

	// call all enable overwritables for string based area space
	for (i = 0; i < num; i++)
	{
		name = W2CT(names[i]);

		if (areaOrSource)
		{
			m_me->enableConditionByAreaName(enable, name);
		}
		else
		{
			m_me->enableConditionBySourceName(enable, name);
		}
	}

	return S_OK;
} // enableConditions


//-----------------------------------------------------------------------------
// EnableConditionByArea (interface IOPCEventServer)
//
// - enable conditions of some areas
//
// returns:
//		S_OK          - condition enabled
//		E_INVALIDARG  - invalid argument
//		E_NOTIMPL     - not implemented
//
STDMETHODIMP SOAeSComServer::EnableConditionByArea(
	IN DWORD numAreas,  // number of areas
	IN LPWSTR* areas)   // area names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::EnableConditionByArea, EnableConditionByArea(numAreas, areas))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(numAreas > 0)
	SOSRV_IN_PARAM(areas != NULL)
	SOSRV_END_PARAMS_CHECK(EnableConditionByArea(res, numAreas, areas))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	res = enableConditions(TRUE, TRUE, numAreas, areas);
	SOSRV_END_INTERFACE_FUNCTION(EnableConditionByArea(res, numAreas, areas))
} // EnableConditionByArea


//-----------------------------------------------------------------------------
// EnableConditionBySource (interface IOPCEventServer)
//
// - enable conditions of some sources
//
// returns:
//		S_OK          - condition enabled
//		E_INVALIDARG  - invalid argument
//		E_NOTIMPL     - not implemented
//
STDMETHODIMP SOAeSComServer::EnableConditionBySource(
	IN DWORD numSources,    // number of sources
	IN LPWSTR* sources)     // source names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::EnableConditionBySource, EnableConditionBySource(numSources, sources))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(numSources > 0)
	SOSRV_IN_PARAM(sources != NULL)
	SOSRV_END_PARAMS_CHECK(EnableConditionBySource(res, numSources, sources))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	res = enableConditions(TRUE, FALSE, numSources, sources);
	SOSRV_END_INTERFACE_FUNCTION(EnableConditionBySource(res, numSources, sources))
} // EnableConditionBySource


//-----------------------------------------------------------------------------
// DisableConditionByArea (interface IOPCEventServer)
//
// - disable conditions of some areas
//
// returns:
//		S_OK          - condition enabled
//		E_INVALIDARG  - invalid argument
//		E_NOTIMPL     - not implemented
//
STDMETHODIMP SOAeSComServer::DisableConditionByArea(
	IN DWORD numAreas,  // number of areas
	IN LPWSTR* areas)   // area names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::DisableConditionByArea, DisableConditionByArea(numAreas, areas))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(numAreas > 0)
	SOSRV_IN_PARAM(areas != NULL)
	SOSRV_END_PARAMS_CHECK(DisableConditionByArea(res, numAreas, areas))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	res = enableConditions(FALSE, TRUE, numAreas, areas);
	SOSRV_END_INTERFACE_FUNCTION(DisableConditionByArea(res, numAreas, areas))
} // DisableConditionByArea


//-----------------------------------------------------------------------------
// DisableConditionBySource (interface IOPCEventServer)
//
// - disable conditions of some sources
//
// returns:
//		S_OK          - condition enabled
//		E_INVALIDARG  - invalid argument
//		E_NOTIMPL     - not implemented
//
STDMETHODIMP SOAeSComServer::DisableConditionBySource(
	IN DWORD numSources,    // number of sources
	IN LPWSTR* sources)     // source names
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::DisableConditionBySource, DisableConditionBySource(numSources, sources))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_IN_PARAM(numSources > 0)
	SOSRV_IN_PARAM(sources != NULL)
	SOSRV_END_PARAMS_CHECK(DisableConditionBySource(res, numSources, sources))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	res = enableConditions(FALSE, FALSE, numSources, sources);
	SOSRV_END_INTERFACE_FUNCTION(DisableConditionBySource(res, numSources, sources))
} // DisableConditionBySource


//-----------------------------------------------------------------------------
// AckCondition (interface IOPCEventServer)
//
// - acknowledge one or more condition notifications
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - one or more errors in errors
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - can't allocate memory
//
STDMETHODIMP SOAeSComServer::AckCondition(
	IN DWORD count,             // number of acks
	IN LPWSTR acknowledgerID,   // acknowledger ID
	IN LPWSTR comment,          // comment
	IN LPWSTR* source,          // source names
	IN LPWSTR* conditionName,   // condition names
	IN FILETIME* activeTime,    // activation times
	IN DWORD* cookie,           // cookies
	OUT HRESULT** errors)       // errors
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::AckCondition, AckCondition(count, acknowledgerID, comment, source, conditionName, activeTime, cookie, errors))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(errors)
	SOSRV_IN_PARAM_PTR(acknowledgerID)
	SOSRV_IN_PARAM_PTR(comment)
	SOSRV_IN_PARAM(wcslen(acknowledgerID) > 0)
	SOSRV_IN_PARAM(count > 0)
	SOSRV_IN_PARAM((source != NULL) && (conditionName != NULL) && (activeTime != NULL) && (cookie != NULL))
	SOSRV_END_PARAMS_CHECK(AckCondition(res, count, acknowledgerID, comment, source, conditionName, activeTime, cookie, errors))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	SOAESCOMSERVER_WAIT_FOR_IO_STARTUP
	// allocate error return memory
	*errors = (HRESULT*) CoTaskMemAlloc(sizeof(HRESULT) * count);

	if (*errors == NULL)
		SOSRV_RETURN_INTERFACE_FUNCTION_RES(E_OUTOFMEMORY, AckCondition(res, count, acknowledgerID, comment, source, conditionName, activeTime, cookie, errors))
		USES_CONVERSION;

	SOCmnString ackID(W2CT(acknowledgerID));
	SOCmnString ackComment(W2CT(comment));
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> root = entry->getAreaSpaceRoot();
	SOCmnPointer<SOAeSSource> src;
	SOCmnPointer<SOAeSCondition> cond;
	SOCmnString srcName;
	SOCmnString condName;
	DWORD i;

	for (i = 0; i < count; i++)
	{
		// find condition object
		srcName = W2CT(source[i]);
		condName = W2CT(conditionName[i]);
		src = (SOAeSSource*)root->findLeaf(srcName, TRUE);

		if (!!src)
		{
			// source object exists
			cond = (SOAeSCondition*)src->findAdditionalElement(condName);
		}

		if (!!cond)
		{
			// condition object exists
			(*errors)[i] = cond->acknowledge(ackID, ackComment, &activeTime[i], cookie[i]);
		}
		else
		{
			// havn't found condition object
			// call name based virtual
			(*errors)[i] = m_me->ackConditionByName(srcName, condName, ackID, ackComment, &activeTime[i], cookie[i]);
		}

		if ((*errors)[i] != S_OK)
		{
			res = S_FALSE;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(AckCondition(res, count, acknowledgerID, comment, source, conditionName, activeTime, cookie, errors))
} // AckCondition


//-----------------------------------------------------------------------------
// CreateAreaBrowser (interface IOPCEventServer)
//
// - create area browser object
//
// returns:
//		S_OK                    - created area browser
//		E_INVALIDARG            - invalid argument
//		E_OUTOFMEMORY           - can't allocate memory
//
STDMETHODIMP SOAeSComServer::CreateAreaBrowser(
	IN REFIID riid,         // interface id
	OUT LPUNKNOWN* unknown) // interface pointer
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCEventServer::CreateAreaBrowser, CreateAreaBrowser(riid, unknown))
	SOSRV_BEGIN_PARAMS_CHECK()
	SOSRV_OUT_PARAM_PTR(unknown)
	SOSRV_END_PARAMS_CHECK(CreateAreaBrowser(res, riid, unknown))
	SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP
	// create area browser object
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	res = m_me->createAreaBrowser((SOAeSAreaBrowser**)areaBrowser);

	if (SUCCEEDED(res))
	{
		HRESULT resQI;
		// create COM area browser object
		SOSrvComObject<SOAeSComAreaBrowser> *aB;
		SOCMN_ALLOCATE_OBJECT(aB, SOSrvComObject<SOAeSComAreaBrowser>)
		aB->init(this, areaBrowser);
		// query the requested interface
		resQI = aB->QueryInterface(riid, (void**)unknown);

		if (FAILED(resQI))
		{
			// can't query the interface
			// delete both subscription objects
			m_me->removeAdditionalElement(areaBrowser);
			delete aB;
			res = resQI;
		}
	}

	SOSRV_END_INTERFACE_FUNCTION(CreateAreaBrowser(res, riid, unknown))
} // CreateAreaBrowser



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
STDMETHODIMP SOAeSComServer::IsAvailablePriv(
	OUT BOOL* pbAvailable) // is private security available
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCSecurityPrivate::IsAvailablePriv, IsAvailablePriv(pbAvailable))
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
STDMETHODIMP SOAeSComServer::Logon(
	IN LPCWSTR szUserID,   // The user’s logon name
	IN LPCWSTR szPassword) // The user’s password
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCSecurityPrivate::Logon, Logon(szUserID, szPassword))
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
STDMETHODIMP SOAeSComServer::Logoff(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IOPCSecurityPrivate::Logoff, Logoff())
	res = m_me->logoff() ? S_OK : E_FAIL;
	SOSRV_END_INTERFACE_FUNCTION(Logoff(res))
} // Logoff



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
STDMETHODIMP SOAeSComServer::Advise(
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
		res = IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>::Advise(unknown, cookie);
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
STDMETHODIMP SOAeSComServer::Unadvise(
	IN DWORD cookie)    // connection cookie
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, IConnectionPoint::Unadvise, Unadvise(cookie))

	if (m_advisesSync.lock(2000))
	{
		res = IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>::Unadvise(cookie);
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
STDMETHODIMP SOAeSComServer::GetChildren(
	IN LPCWSTR parent,      // parent object
	IN BOOL withAttributes, // add the object attributes
	OUT LPWSTR* children)   // children descriptions
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCWatch::GetChildren, GetChildren(parent, withAttributes, children))
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
STDMETHODIMP SOAeSComServer::CollectChanges(
	IN LPCWSTR objects,
	IN DWORD doCollect)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCWatch::CollectChanges, CollectChanges(objects, doCollect))
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
STDMETHODIMP SOAeSComServer::GetChanges(
	OUT LPWSTR* changes) // changes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCWatch::GetChanges, GetChanges(changes))
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
STDMETHODIMP SOAeSComServer::GetStatistics(
	OUT LPWSTR* statistics) // statistic information
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCWatch::GetStatistics, GetStatistics(statistics))
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
STDMETHODIMP SOAeSComServer::GetObject(
	IN LPCWSTR objectDescription, // object description
	OUT LPWSTR* object)           // object with all attributes
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCWatch::GetObject, GetObject(objectDescription, object))
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

STDMETHODIMP SOAeSComServer::SetTraceOptions(
	IN LPCWSTR options)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCTrace::SetTraceOptions, SetTraceOptions(options))
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


STDMETHODIMP SOAeSComServer::GetTraceOptions(OUT LPWSTR* options)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCTrace::GetTraceOptions, GetTraceOptions(options))
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


STDMETHODIMP SOAeSComServer::SaveTraceOptions(void)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCTrace::SaveTraceOptions, SaveTraceOptions())
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


STDMETHODIMP SOAeSComServer::GetTrace(
	OUT LPWSTR* trcMsgs)
{
	SOSRV_BEGIN_INTERFACE_FUNCTION(AE, ISoftingOPCTrace::GetTrace, GetTrace(trcMsgs))
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
HRESULT SOAeSComServer::sendShutdownRequest(
	IN LPCTSTR reason)  // shutdown reason
{
	USES_CONVERSION;
	IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
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
			cookieList.add(*pp);
		}

		pp++;
	}

	m_advisesSync.unlock();
	// loop all clients
	((SOSrvComObject<SOAeSComServer> *)this)->internalAddRef();
	pos = cookieList.getStartPosition();

	while (pos)
	{
		pIShutdown = (IOPCShutdown*)cookieList.getNext(pos);
		// call shutdown
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

	((SOSrvComObject<SOAeSComServer> *)this)->internalRelease();
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
BOOL SOAeSComServer::connectedShutdownInterface(void)
{
	IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
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
BOOL SOAeSComServer::checkClientConnection(void)
{
	IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
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
			res = pIShutdown->QueryInterface(IID_IOPCEventServer, (void**)&unk);

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
void SOAeSComServer::releaseAllAdvises(void)
{
	IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = this;
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
// - query interface overwritable method
//
// returns:
//		E_NOTIMPL     - do standard ATL QueryInterface
//		S_OK          - returned interface
//		E_NOINTERFACE - interface not supported
//
HRESULT SOAeSComServer::queryInterface(
	IN REFIID iid,      // interface id
	IN void** object)   // interface
{
	HRESULT res;

	if ((res = m_me->queryInterface(iid, object)) != E_NOTIMPL)
	{
		return res;
	}

	if ((iid == IID_ISoftingOPCWatch) || (iid == IID_ISoftingOPCTrace))
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
// SOAeSComServer Instances                                                   |
//-----------------------------------------------------------------------------

SOAeSComServer0::SOAeSComServer0()
{
	m_serverInstance = 0;
}

SOAeSComServer1::SOAeSComServer1()
{
	m_serverInstance = 1;
}

SOAeSComServer2::SOAeSComServer2()
{
	m_serverInstance = 2;
}

SOAeSComServer3::SOAeSComServer3()
{
	m_serverInstance = 3;
}

SOAeSComServer4::SOAeSComServer4()
{
	m_serverInstance = 4;
}

SOAeSComServer5::SOAeSComServer5()
{
	m_serverInstance = 5;
}

SOAeSComServer6::SOAeSComServer6()
{
	m_serverInstance = 6;
}

SOAeSComServer7::SOAeSComServer7()
{
	m_serverInstance = 7;
}

SOAeSComServer8::SOAeSComServer8()
{
	m_serverInstance = 8;
}

SOAeSComServer9::SOAeSComServer9()
{
	m_serverInstance = 9;
}

#endif // SOFEATURE_DCOM

#endif // SOAES
