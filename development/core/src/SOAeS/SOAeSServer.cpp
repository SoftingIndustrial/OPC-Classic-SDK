#include "stdafx.h"

#ifdef SOAES

#include "SOAeSServer.h"
#include "SOAeSEntry.h"
#include "SOCmnTrace.h"
#include "dllmain.h"
#include "SOAeSComServer.h"
#include "SOAeSComSubscription.h"
#include "SOAeSComAreaBrowser.h"
#include <opcae_er.h>
#include <opcaedef.h>

//-----------------------------------------------------------------------------
// SOAeSServer                                                                |
//-----------------------------------------------------------------------------

SOAeSServer::SOAeSServer(
	IN OPTIONAL BYTE serverInstance)            // server instance
	:  SOSrvServer(SOCMNOBJECT_TYPE_SERVER_AE, serverInstance)
{
	m_lastUpdateTime.dwLowDateTime  = 0L;
	m_lastUpdateTime.dwHighDateTime = 0L;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SERVER_AE);
#endif
	// create asynchronous worker thread
	createWorkThread();
}


SOAeSServer::~SOAeSServer(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_SERVER_AE);
#endif
}

HRESULT SOAeSServer::setFlags(BYTE flags)
{
	m_flags = flags;
	return S_OK;
}


//-----------------------------------------------------------------------------
// doWork
//
// - do work of one working thread loop
//
// returns:
//		minimal time till next call
//
DWORD SOAeSServer::doWork(
	IN DWORD startTime)     // start time of work loop
{
	DWORD waitTime;
	// subscription normal event work
	waitTime = sendEventNotifications(startTime, INFINITE);

	// handle refresh on the subscriptions
	if (waitTime > 0)
	{
		waitTime = handleRefresh(startTime, waitTime);
	}

	return waitTime;
} // doWork

//-----------------------------------------------------------------------------
// allocSubscriptionsAsArray
//
// - retrieve a (zero terminated) array of SOAeSSubscription*,
//   every subscription in the array has been AddRef()ed.
//
// returns:
//      pointer to the subscription array
//      NULL - out of memory
//
SOAeSSubscription** SOAeSServer::allocSubscriptionsAsArray(
	OUT int* pnSubscriptions)
{
	SOCmnList<SOCmnElement> subList(getLeafList());
	int                     nSubs = subList.getCount();
	SOAeSSubscription**         pSubs;
	SOCMN_ALLOCATE_MEMORY_C(pSubs, (nSubs + 1)*sizeof(SOAeSSubscription*), (SOAeSSubscription**));

	if (!pSubs)
	{
		*pnSubscriptions = 0;
		return NULL; //out of mem...
	}

	*pnSubscriptions = nSubs;
	int i = 0;
	SOCmnListPosition pos = subList.getStartPosition();

	while (pos && i < nSubs)
	{
		pSubs[i] = (SOAeSSubscription*)subList.getNext(pos);
		pSubs[i++]->addRef();
	}

	pSubs[i] = NULL; //zero-term the array
	return pSubs;
}

//-----------------------------------------------------------------------------
// freeSubscriptionsAsArray
//
// - call Release() for all subscriptions in the array and free the array itself
//
void SOAeSServer::freeSubscriptionsAsArray(SOAeSSubscription** pSubscriptions)
{
	if (pSubscriptions)
	{
		for (int i = 0; pSubscriptions[i]; i++)
		{
			pSubscriptions[i]->release();
		}

		SOCMN_FREE_MEMORY(pSubscriptions);
	}
}

//-----------------------------------------------------------------------------
// sendEventNotifications
//
// - send event notifications for all subscriptions
//
// returns:
//		time in milliseconds until next call is needed
//
DWORD SOAeSServer::sendEventNotifications(
	IN DWORD now,       // current time
	IN DWORD minSpan)   // minimal time till next call
{
	SOCmnList<SOAeSSubscription> list((SOCmnList<SOAeSSubscription> *)getLeafList());
	SOCmnPointer<SOAeSSubscription> subscr;
	SOCmnListPosition pos;
	DWORD actSpan;
	BOOL end;
	DWORD listCnt;
	// for every subscription
	list.dontAutoLock();
	list.lock();
	list.setDirty(FALSE);
	pos = list.getStartPosition();
	list.unlock();

	while (pos)
	{
		list.lock();

		if (list.isDirty())
		{
			// check current position!
			SOCmnListPosition checkPos;
			BOOL found = FALSE;
			checkPos = list.getStartPosition();

			while (checkPos)
			{
				if (checkPos == pos)
				{
					found = TRUE;
					break;
				}

				list.getNext(checkPos);
			}

			if (!found)
			{
				pos = list.getStartPosition();
			}

			list.setDirty(FALSE);

			if (pos == NULL)
			{
				list.unlock();
				break;
			}
		}

		subscr = list.getNext(pos);
		subscr.addRef();
		list.unlock();
		end = FALSE;
		actSpan = 0;

		do
		{
			// send events
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("send events %s"), subscr->getObjTraceId()))

			if ((listCnt = subscr->doSendEvents(FALSE)) > 0)
			{
				// not all events send
				// check max size:
				// if count is greater than maxSize send the next events
				// if not check the send time
				if ((subscr->getMaxSize() == 0) || (listCnt <= subscr->getMaxSize()))
				{
					// get time till next send is allowed
					actSpan = subscr->checkSendTime(now);

					if (actSpan > 0)
					{
						end = TRUE;    // not now
					}
				}
			}
			else
			{
				end = TRUE;    // all events of the list send
			}
		}
		while (!end);

		if ((actSpan > 0) && (actSpan < minSpan))
		{
			minSpan = actSpan;
		}

		// check for the end event of the work thread

		if (getEndEvent().waitFor(0))
		{
			return 0;
		}
	}

	return minSpan;
} // sendEventNotifications


//-----------------------------------------------------------------------------
// handleRefresh
//
// - send event notifications for all subscriptions
//
// returns:
//		time in milliseconds until next call is needed
//
DWORD SOAeSServer::handleRefresh(
	IN DWORD /* now */,    // current time
	IN DWORD minSpan)      // minimal time till next call
{
	SOCmnList<SOAeSSubscription> list((SOCmnList<SOAeSSubscription> *)getLeafList());
	SOCmnPointer<SOAeSSubscription> subscr;
	SOCmnListPosition pos;
	DWORD actSpan;
	SOCmnPointer<SOAeSRefresh> refresh;
	// for every subscription
	list.dontAutoLock();
	list.lock();
	list.setDirty(FALSE);
	pos = list.getStartPosition();
	list.unlock();

	while (pos)
	{
		list.lock();

		if (list.isDirty())
		{
			// check current position!
			SOCmnListPosition checkPos;
			BOOL found = FALSE;
			checkPos = list.getStartPosition();

			while (checkPos)
			{
				if (checkPos == pos)
				{
					found = TRUE;
					break;
				}

				list.getNext(checkPos);
			}

			if (!found)
			{
				pos = list.getStartPosition();
			}

			list.setDirty(FALSE);

			if (pos == NULL)
			{
				list.unlock();
				break;
			}
		}

		subscr = list.getNext(pos);
		subscr.addRef();
		list.unlock();
		refresh = subscr->getRefresh();
		actSpan = 0;

		if (!!refresh)
		{
			// refresh object set
			BYTE state = refresh->getObjectState();
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("handle refresh %s; state 0x%X"), subscr->getObjTraceId(), state))

			// check state of refresh
			if (state & (BYTE)SOAeSRefresh::created)
			{
				// collect all events from the condition objects
				SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
				SOCmnPointer<SOAeSAreaSpaceRoot> areaRoot = entry->getAreaSpaceRoot();
				SOCmnList<SOAeSEvent> eventList = refresh->getEventList();
				// for all conditions ...
				aggregateConditions(areaRoot, &eventList);
				// set aggregated state
				state &= ~SOAeSRefresh::created;
				state |= SOAeSRefresh::aggregated;
				refresh->setObjectState(state);
			}

			if ((state & (BYTE)SOAeSRefresh::collected) &&
				(state & (BYTE)SOAeSRefresh::aggregated))
			{
				// all events collected
				SOCmnListPosition refPos;
				SOAeSEvent* event;
				SOCmnList<SOAeSEvent> eventList = refresh->getEventList();
				// filter the refresh events
				refPos = eventList.getStartPosition();

				while (refPos)
				{
					event = eventList.getNext(refPos);

					if (!subscr->filterEvent(event))
					{
						// event filtered out
						eventList.removeObject(event);
					}
				}

				// switch to send state
				state = SOAeSRefresh::send;
				refresh->setObjectState(state);
			}

			if (state & (BYTE)SOAeSRefresh::canceled)
			{
				// refresh cancled
				// send last refresh notification for cancel
				subscr->doSendEvents(TRUE);
				// release refresh object
				subscr->releaseRefresh();
			}
			else if (state & (BYTE)SOAeSRefresh::send)
			{
				// send refresh events
				DWORD remainingCnt;

				do
				{
					// send events
					remainingCnt = subscr->doSendEvents(TRUE);
				}
				while (remainingCnt > 0);

				// release refresh object
				subscr->releaseRefresh();
			}
		}

		if ((actSpan > 0) && (actSpan < minSpan))
		{
			minSpan = actSpan;
		}
	}

	return minSpan;
} // handleRefresh


//-----------------------------------------------------------------------------
// aggregateConditions
//
// - collect events for all active or inactive unacked conditions
//
void SOAeSServer::aggregateConditions(
	IN SOCmnElement* parent,                // parent element
	OUT SOCmnList<SOAeSEvent> *eventList)   // event list
{
	SOCmnList<SOAeSArea> areaList;
	SOAeSArea* area;
	SOCmnListPosition posA;
	SOCmnList<SOAeSSource> sourceList;
	SOAeSSource* source;
	SOCmnListPosition posS;
	SOCmnList<SOAeSCondition> condList;
	SOAeSCondition* cond;
	SOCmnListPosition posC;
	condList = (SOCmnList<SOAeSCondition> *)parent->getAdditionalList();

	if (!!condList)
	{
		SOCmnPointer<SOAeSEvent> event;
		WORD condState;
		posC = condList.getStartPosition();

		while (posC)
		{
			cond = condList.getNext(posC);
			condState = cond->getObjectState();

			if ((condState & OPC_CONDITION_ACTIVE) ||
				((!(condState & OPC_CONDITION_ACTIVE)) && (!(condState & OPC_CONDITION_ACKED))))
			{
				// conditin active or (inactive and unacknowledged)
				event = cond->createEvent(0);
				eventList->add((SOAeSEvent*)event);
			}
		}
	}

	sourceList = (SOCmnList<SOAeSSource> *)parent->getLeafList();

	if (!!sourceList)
	{
		posS = sourceList.getStartPosition();

		while (posS)
		{
			source = sourceList.getNext(posS);
			aggregateConditions(source, eventList);
		}
	}

	areaList = (SOCmnList<SOAeSArea> *)parent->getBranchList();

	if (!!areaList)
	{
		posA = areaList.getStartPosition();

		while (posA)
		{
			area = areaList.getNext(posA);
			aggregateConditions(area, eventList);
		}
	}
} // aggregateConditions


//-----------------------------------------------------------------------------
// getMe
//
// - get reference to COM object
//
// returns:
//		pointer of COM object
//
void* SOAeSServer::getMe(void)
{
	m_meSync.lock();
	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)m_me;

	if (srv)
	{
		srv->tempAddRef();
	}

	m_meSync.unlock();
	return m_me;
} // getMe


//-----------------------------------------------------------------------------
// releaseMe
//
// - release reference to COM object obtained by getMe
//
// returns:
//		temporary reference count of COM object
//
ULONG SOAeSServer::releaseMe(
	IN void* /* me */)  // pointer to COM object
{
	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)m_me;

	if (srv)
	{
		return srv->tempRelease();
	}

	return 0;
} // releaseMe


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSServer::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addLeaf
//
// - add leaf into server object, if it's a subscription object
//
// returns:
//      TRUE  - subscription added
//      FALSE - not added
//
BOOL SOAeSServer::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addAdditionalElement
//
// - add leaf into server object, if it's a area browser object
//
// returns:
//      TRUE  - subscription added
//		FALSE - not added
//
BOOL SOAeSServer::addAdditionalElement(
	IN SOCmnElement* newEl)
{
	if (!newEl)
	{
		return FALSE;
	}

	if (newEl->is(SOCMNOBJECT_TYPE_AREA_BROWSER))
	{
		return SOCmnElement::addAdditionalElement(newEl);
	}
	else
	{
		return FALSE;
	}
} // addAdditionalElement


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOAeSServer::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[%s|%8.8X| %s]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle(), (LPCTSTR)getClientName());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// getStatus
//
// - get status of server
//
// returns:
//		S_OK          - filled status sturcture
//		E_OUTOFMEMORY - can't allocate vendor string memory
//
HRESULT SOAeSServer::getStatus(
	OUT OPCEVENTSERVERSTATUS* status)   // server status
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnString vendor = entry->getVendorInfo();
	USES_CONVERSION;
	status->szVendorInfo = (LPWSTR)CoTaskMemAlloc(2 * vendor.GetLength() + 2);

	if (status->szVendorInfo == NULL)
	{
		return E_OUTOFMEMORY;
	}

	wcscpy(status->szVendorInfo, T2COLE((LPCTSTR)vendor));
	SOCmnDateTime startTime;
	entry->getStartTime(&startTime);
	startTime.get(&status->ftStartTime);
	GetSystemTimeAsFileTime(&status->ftCurrentTime);
	getUpdateTime(&status->ftLastUpdateTime);
	status->dwServerState = entry->getServerState();
	WORD wPatchVersion = 0;
	entry->getVersionInfo(&status->wMajorVersion, &status->wMinorVersion,
						  &wPatchVersion, &status->wBuildNumber);
	status->wBuildNumber += wPatchVersion * 10000u;
	return S_OK;
} // getStatus


//-----------------------------------------------------------------------------
// createSubscription
//
// - create subscription object
//
// returns:
//		S_OK                    - object created
//		OPC_S_INVALIDBUFFERTIME - invalid buffer time
//		OPC_S_INVALIDMAXSIZE    - invalid max size
//		E_OUTOFMEMORY           - out of memory
//
HRESULT SOAeSServer::createSubscription(
	IN BOOL active,                         // active
	IN DWORD bufferTime,                    // requested buffer time
	IN DWORD maxSize,                       // requested max size
	IN OPCHANDLE clientSubscriptionHandle,  // client handle
	OUT SOAeSSubscription** subsrc,         // subscription object
	OUT DWORD* revisedBufferTime,           // buffer time
	OUT DWORD* revisedMaxSize)              // max size
{
	HRESULT res = S_OK;
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOAeSSubscription* sus;
	SOCmnPointer<SOAeSCreator> creator;
	// create subscription object
	creator = entry->getCreator();
	sus = creator->createSubscription(this);

	if (!sus)
	{
		return E_OUTOFMEMORY;
	}

	// add to subscription list
	addLeaf(sus);
	// set subscription parameters
	sus->setActive(active);
	sus->setBufferTime(bufferTime);
	sus->setMaxSize(maxSize);
	sus->setClientHandle(clientSubscriptionHandle);

	// fill the output parameter
	if (revisedBufferTime)
	{
		*revisedBufferTime = sus->getBufferTime();

		if (*revisedBufferTime != bufferTime)
		{
			res = OPC_S_INVALIDBUFFERTIME;
		}
	}

	if (revisedMaxSize)
	{
		*revisedMaxSize = sus->getMaxSize();

		if (*revisedMaxSize != maxSize)
		{
			res = OPC_S_INVALIDMAXSIZE;
		}
	}

	if (subsrc != NULL)
	{
		*subsrc = sus;
	}
	else
	{
		sus->release();
	}

	return res;
} // createSubscription


//-----------------------------------------------------------------------------
// createAreaBrowser
//
// - create area browser object
//
// returns:
//		S_OK                    - object created
//		E_OUTOFMEMORY           - out of memory
//
HRESULT SOAeSServer::createAreaBrowser(
	OUT SOAeSAreaBrowser** areaBrowser) // created area browser object
{
	HRESULT res = S_OK;
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOAeSAreaBrowser* aB;
	SOCmnPointer<SOAeSCreator> creator;
	// create subscription object
	creator = entry->getCreator();
	aB = creator->createAreaBrowser(this);

	if (!aB)
	{
		return E_OUTOFMEMORY;
	}

	// add to area browser list
	addAdditionalElement(aB);

	if (areaBrowser != NULL)
	{
		*areaBrowser = aB;
	}
	else
	{
		aB->release();
	}

	return res;
} // createAreaBrowser


//-----------------------------------------------------------------------------
// queryAvailableFilters
//
// - get the supported filter types for the subscription
//
// returns:
//		supported filter types
//
DWORD SOAeSServer::queryAvailableFilters(void)
{
	return (OPC_FILTER_BY_EVENT | OPC_FILTER_BY_CATEGORY | OPC_FILTER_BY_SEVERITY |
			OPC_FILTER_BY_AREA | OPC_FILTER_BY_SOURCE);
} // queryAvailableFilters


//-----------------------------------------------------------------------------
// isAreaName
//
// - check if the area name is part of the string based area space
//
// returns:
//		TRUE  - yes it's part of the area space
//		FALSE - not part of the area space
//
BOOL SOAeSServer::isAreaName(
	IN LPCTSTR /* areaName */)  // area name
{
	return FALSE;    // isAreaName
}


//-----------------------------------------------------------------------------
// isSourceName
//
// - check if the source name is part of the string based area space
//
// returns:
//		TRUE  - yes it's part of the area space
//		FALSE - not part of the area space
//
BOOL SOAeSServer::isSourceName(
	IN LPCTSTR /* sourceName */)    // source name
{
	return FALSE;    // isSourceName
}


//-----------------------------------------------------------------------------
// getChildNamesOfAreaName
//
// - get the child (source or area) names of the string based area
//
void SOAeSServer::getChildNamesOfAreaName(
	IN LPCTSTR /* areaName */,              // area name
	BOOL /* areaOrSourceChilds */,          // area names or source names
	LPCTSTR /* stringFilter */,             // string filter for child names
	OUT SOCmnStringList& /* nameList */)    // child name list
{
} // getChildNamesOfAreaName


//-----------------------------------------------------------------------------
// browseOnlyStringBased
//
// - use only the string based address space for browsing
//
// returns:
//		TRUE  - only string based
//		FALSE - object based and string based
//
BOOL SOAeSServer::browseOnlyStringBased(void)
{
	return FALSE;    // browseOnlyStringBased
}


//-----------------------------------------------------------------------------
// getConditionNamesOfSourceName
//
// - get the condition names of the string based source
//
void SOAeSServer::getConditionNamesOfSourceName(
	IN LPCTSTR /* sourceName */,                // source name
	OUT SOCmnStringList& /* condNameList */)    // condtion name list
{} // getConditionNamesOfSourceName


//-----------------------------------------------------------------------------
// supportEnableCondition
//
// - support en/disabling of conditions by area or source
//
// returns:
//		TRUE  - supported
//		FALSE - not supported
//
BOOL SOAeSServer::supportEnableCondition(
	IN BOOL /* enable */,           // en/disable
	IN BOOL /* areaOrSource */)     // area or source
{
	return TRUE;
} // supportEnableCondition


//-----------------------------------------------------------------------------
// supportGetConditionState
//
// - support get state of conditions
//
// returns:
//		TRUE  - supported
//		FALSE - not supported
//
BOOL SOAeSServer::supportGetConditionState(void)
{
	return TRUE;
} // supportGetConditionState


//-----------------------------------------------------------------------------
// supportTranslateToItemIDs
//
// - support translate to item ids
//
// returns:
//		TRUE  - supported
//		FALSE - not supported
//
BOOL SOAeSServer::supportTranslateToItemIDs(void)
{
	return TRUE;
} // supportTranslateToItemIDs


//-----------------------------------------------------------------------------
// enableConditionByAreaName
//
// - enable a condition by area name
//
// returns:
//		TRUE
//
BOOL SOAeSServer::enableConditionByAreaName(
	IN BOOL /* enable */,       // en/disable
	IN LPCTSTR /* areaName */)  // area name
{
	return TRUE;
} // enableConditionByAreaName


//-----------------------------------------------------------------------------
// enableConditionBySourceName
//
// - enable a condition by source name
//
// returns:
//		TRUE
//
BOOL SOAeSServer::enableConditionBySourceName(
	IN BOOL /* enable */,           // en/disable
	IN LPCTSTR /* sourceName */)    // source name
{
	return TRUE;
} // enableConditionBySourceName


//-----------------------------------------------------------------------------
// refreshConditions
//
// - refresh conditions; add events for all conditions to the event list of the
//   refresh object. Set the state of the refresh object to collected after that.
//
void SOAeSServer::refreshConditions(
	IN SOAeSRefresh* refresh)   // refresh object
{
	refresh->endCollection();
} // refreshConditions


//-----------------------------------------------------------------------------
// cancelRefreshConditions
//
// - cancel refresh of conditions
//
void SOAeSServer::cancelRefreshConditions(
	IN SOAeSRefresh* /* refresh */) // refresh object
{} // cancelRefreshConditions


//-----------------------------------------------------------------------------
// createConditionByName
//
// - create condition object for GetConditionState call of the client
//
// returns:
//		S_OK         - object created
//		E_INVALIDARG - no condition object with this names
//		OPC_E_NOINFO - no condition state info available
//
HRESULT SOAeSServer::createConditionByName(
	IN LPCTSTR /* sourceName */,            // source name
	IN LPCTSTR /* conditionName */,     // condition name
	OUT SOAeSCondition** /* condition */)   // condition object
{
	return E_INVALIDARG;
} // createConditionByName


//-----------------------------------------------------------------------------
// ackConditionByName
//
// - acknowledge condition
//
// returns:
//		S_OK               - acknowledge succeeded
//		OPC_S_ALREADYACKED - condition has already been acked
//		E_INVALIDARG       - bad parameter
//		OPC_E_INVALIDTIME  - time does not match latest activation time
//
HRESULT SOAeSServer::ackConditionByName(
	IN LPCTSTR /* sourceName */,    // source name
	IN LPCTSTR /* conditionName */, // condition name
	IN LPCTSTR /* ackID */,         // acknowledger ID
	IN LPCTSTR /* ackComment */,    // acknowledger comment
	LPFILETIME /* activeTime */,    // active time
	DWORD /* cookie */)             // notification cookie
{
	return E_INVALIDARG;
} // ackConditionByName


//-----------------------------------------------------------------------------
// translateToItemIDs
//
// - get the OPC DA item IDs for the attributes of a (sub)condition
//
// returns:
//		E_INVALIDARG
//
HRESULT SOAeSServer::translateToItemIDsByName(
	IN LPCTSTR /* sourceName */,            // source name
	IN SOAeSCategory* /* category */,       // category
	IN LPCTSTR /* conditionName */,     // condition name
	IN LPCTSTR /* subConditionName */,  // subcondition name
	OUT SOCmnList<SOAeSAttributeValueDADescription>* /*descrDAList */)  // da
{
	return E_INVALIDARG;
} // translateToItemIDs


//-----------------------------------------------------------------------------
// startShutdownRequest
//
// - add shutdown request to the shutdown list of the entry
//
// returns:
//		S_OK    - added shutdown request
//		E_FAIL  - no COM server object
//
HRESULT SOAeSServer::startShutdownRequest(
	IN LPCTSTR reason,  // shutdown reason
	IN HANDLE event)    // noftification event
{
	SOCmnPointer<SOAeSEntry> engine = ::getSOAeSEntry();

	if (!m_me)
	{
		return E_FAIL;
	}

	engine->addShutdownRequest(this, reason, event);
	return S_OK;
} // startShutdownRequest


//-----------------------------------------------------------------------------
// sendShutdownRequest
//
// - send shutdown request to the OPC client
//
// returns:
//		S_OK    - send shutdown request
//		E_FAIL  - no COM server object
//
HRESULT SOAeSServer::sendShutdownRequest(
	IN LPCTSTR reason)  // shutdown reason
{
	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)getMe();
	HRESULT res;

	if (srv)
	{
		res = srv->sendShutdownRequest(reason);
		releaseMe(srv);
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // sendShutdownRequest


//-----------------------------------------------------------------------------
// checkClientConnection
//
// - check if the connection to the client still exitsts
//
// returns:
//		TRUE  - client is running
//		FALSE - client does not react
//
BOOL SOAeSServer::checkClientConnection(OPTIONAL IN BOOL checkAllIFs)
{
	BOOL conOK = TRUE;
	BOOL checked = FALSE;
	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)getMe();

	if (!srv)
	{
		return FALSE;
	}

	if (srv->connectedShutdownInterface())
	{
		conOK = srv->checkClientConnection();

		if (!checkAllIFs)
		{
			checked = TRUE;
		}
	}

	if (!checked)
	{
		int nTmpSubs;
		SOAeSSubscription** pTmpSubList = allocSubscriptionsAsArray(&nTmpSubs);
		SOCmnList<IOPCEventSink> esList;
		IOPCEventSink* es;
		SOCmnListPosition esPos;
		IUnknown* unk;
		HRESULT res;

		for (int i = 0; !checked && i < nTmpSubs;  i++)
		{
			SOAeSSubscription* pSub = pTmpSubList[i];

			if (pSub->createConnectedEventSinkInterfaceList(&esList))
			{
				esPos = esList.getStartPosition();

				while ((esPos) && (!checked))
				{
					if (!checkAllIFs)
					{
						checked = TRUE;
					}

					es = esList.getNext(esPos);
					res = es->QueryInterface(IID_IOPCEventServer, (void**)&unk);

					if (FAILED(res))
					{
						if (SOSRV_IS_RPC_ERROR(res))
						{
							conOK = FALSE;
							esPos = NULL;
							break;
						}
					}
					else
					{
						unk->Release();
					}
				}
			}

			esList.destroy();
		}

		freeSubscriptionsAsArray(pTmpSubList);
	}

	releaseMe(srv);
	return conOK;
} // checkClientConnection


//-----------------------------------------------------------------------------
// createConnectedShutdownInterfaceList
//
// - create a list of all connected shutdown interfaces
//
// returns:
//		TRUE
//
BOOL SOAeSServer::createConnectedShutdownInterfaceList(
	OUT SOCmnList<IOPCShutdown> *shutdownList)  // interface list
{
	if (shutdownList == NULL)
	{
		return FALSE;
	}

	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)getMe();

	if (!srv)
	{
		return FALSE;
	}

	IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>* p = (SOAeSComServer*)srv;
	IUnknown** pp;
	shutdownList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_INTERFACE);
	pp = p->m_vec.begin();

	while (pp < p->m_vec.end())
	{
		if (*pp != NULL)
		{
			shutdownList->add((IOPCShutdown*)*pp);
		}

		pp++;
	}

	shutdownList->autoUnlock();
	releaseMe(srv);
	return TRUE;
} // createConnectedShutdownInterfaceList


//-----------------------------------------------------------------------------
// disconnectClient
//
// - disconnect the OPC client (forced shutdown)
//
void SOAeSServer::disconnectClient(void)
{
	SOSrvComObject<SOAeSComServer> *srv = (SOSrvComObject<SOAeSComServer> *)getMe();

	if (!srv)
	{
		return;
	}

	DWORD extRefs;
	_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OBJ_REF, (_T("disconnect client %s"), (LPCTSTR)getName()))
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("disconnecting refCnt:%u intRefCnt:%u"), srv->m_dwRef, (ULONG)srv->m_internalCnt))
	SOSrvComObject<SOAeSComSubscription> *subscr;
	SOSrvComObject<SOAeSComAreaBrowser> *areaBrowser;
	SOCmnList<SOCmnElement> susList;
	SOCmnList<SOCmnElement> abList;
	SOAeSSubscription* sus;
	SOAeSAreaBrowser* ab;
	SOCmnListPosition pos;
	// disconnect the subscriptions
	susList = getLeafList();
	pos = susList.getStartPosition();

	while (pos)
	{
		sus = (SOAeSSubscription*)susList.getNext(pos);
		subscr = (SOSrvComObject<SOAeSComSubscription> *)sus->getMe();

		if (subscr)
		{
			extRefs = subscr->m_dwRef - (subscr->m_internalCnt + subscr->m_tempCnt);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("disconnecting subscription refCnt:%u intRefCnt:%u"), subscr->m_dwRef, (ULONG)subscr->m_internalCnt));
			subscr->releaseComServer();

			if (subscr->m_internalCnt > 0)
				while (subscr->internalRelease())
					;

			sus->releaseMe(subscr);

			if (extRefs)
			{
				CoDisconnectObject((IOPCEventSubscriptionMgt*)subscr, 0);
			}
		}
	}

	susList.removeAll();
	susList.autoUnlock();
	// disconnect area browser
	abList = getAdditionalList();
	pos = abList.getStartPosition();

	while (pos)
	{
		ab = (SOAeSAreaBrowser*)abList.getNext(pos);
		areaBrowser = (SOSrvComObject<SOAeSComAreaBrowser> *)ab->getMe();

		if (areaBrowser)
		{
			extRefs = areaBrowser->m_dwRef - (areaBrowser->m_internalCnt  + areaBrowser->m_tempCnt);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("disconnecting area browser refCnt:%u intRefCnt:%u"), areaBrowser->m_dwRef, (ULONG)areaBrowser->m_internalCnt));
			areaBrowser->releaseComServer();

			if (areaBrowser->m_internalCnt > 0)
				while (areaBrowser->internalRelease())
					;

			ab->releaseMe(areaBrowser);

			if (extRefs)
			{
				CoDisconnectObject((IOPCEventAreaBrowser*)areaBrowser, 0);
			}
		}
	}

	abList.removeAll();
	abList.autoUnlock();
	srv->releaseAllAdvises();
	// disconnect server
	extRefs = srv->m_dwRef - (srv->m_internalCnt + srv->m_tempCnt);

	while (srv->internalRelease())
		;

	releaseMe(srv); // use after release is ok in this case!

	if (extRefs)
	{
		CoDisconnectObject((IOPCServer*)srv, 0);
	}
} // disconnectClient


//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//		E_NOTIMPL     - no query interface implementation for this interface
//		S_OK          - interface delivered
//		E_NOINTERFACE - interface not supported
//
HRESULT SOAeSServer::queryInterface(
	IN REFIID /* iid */,        // interface id
	IN void** /* object */) // interface pointer
{
	return E_NOTIMPL;    // queryInterface
}


//-----------------------------------------------------------------------------
// getSpecID
//
// - get the specification id of the server
//
// returns:
//		SOSRVSERVER_SPECID_AE - alarms & events server object
//
DWORD SOAeSServer::getSpecID(void)
{
	return SOSRVSERVER_SPECID_AE;    // getSpecID
}


//-----------------------------------------------------------------------------
// getCLSID
//
// - get the CLSID of the sever
//
void SOAeSServer::getCLSID(
	OUT CLSID& classID) // class id
{
	switch (m_serverInstance)
	{
	case 0:
		classID = g_clsidAE0;
		break;

	case 1:
		classID = g_clsidAE1;
		break;

	case 2:
		classID = g_clsidAE2;
		break;

	case 3:
		classID = g_clsidAE3;
		break;

	case 4:
		classID = g_clsidAE4;
		break;

	case 5:
		classID = g_clsidAE5;
		break;

	case 6:
		classID = g_clsidAE6;
		break;

	case 7:
		classID = g_clsidAE7;
		break;

	case 8:
		classID = g_clsidAE8;
		break;

	case 9:
		classID = g_clsidAE9;
		break;
	}
} // getCLSID


//-----------------------------------------------------------------------------
// waitForAreaSpaceStartup
//
// - wait for the area space creation to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOAeSServer::waitForAreaSpaceStartup(void)
{
	return S_OK;
} // waitForAreaSpaceStartup

HRESULT SOAeSServer::waitForEventSpaceStartup(void)
{
	return S_OK;
} // waitForEventSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOAeSServer::waitForIOStartup(void)
{
	return S_OK;
} // waitForIOStartup

//-----------------------------------------------------------------------------
// getSubscriptionByHandle
//
SOAeSSubscription* SOAeSServer::getSubscriptionByHandle(IN OPCHANDLE aServerHandle)
{
	SOCmnList<SOAeSSubscription> subscriptionList(getSubscriptionList());
	return subscriptionList.findKey(aServerHandle);
}   //  end getSubscriptionByHandle


//-----------------------------------------------------------------------------
// getAreaBrowserByHandle
//
SOAeSAreaBrowser* SOAeSServer::getAreaBrowserByHandle(IN OPCHANDLE aServerHandle)
{
	SOCmnList<SOAeSAreaBrowser> areaBrowserList(getAreaBrowserList());
	return areaBrowserList.findKey(aServerHandle);
}   //  end getAreaBrowserByHandle


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSServer::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_SERVER);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		FILETIME lastUpdateTime;
		CLSID clsid;
		SOCmnStringEx exHlp;
		getUpdateTime(&lastUpdateTime);
		getCLSID(clsid);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLSID_T, conv->CLSID2string(buffer, clsid));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLIENTNAME_T, SOCmnStringEx::setXMLString(getClientName(), exHlp));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_LCID_T, conv->LCID2string(buffer, getLCID()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SERVERSTATE_T, conv->serverState2string(buffer, entry->getServerState()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, conv->filetime2string(buffer, &lastUpdateTime));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_AVAILABLEFILTERS_T, conv->filters2string(buffer, queryAvailableFilters()));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH




#ifdef SOFEATURE_COMIF_HOOKS
// interface function callbacks
HRESULT SOAeSServer::onBSetLocaleID(IN LCID dwLcid)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  lcid:0x%X"), dwLcid));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetLocaleID(OUT LCID* pdwLcid)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQueryAvailableLocaleIDs(OUT DWORD* pdwCount, OUT LCID** pdwLcid)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetErrorString(IN HRESULT dwError, OUT LPWSTR* ppString)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorCode:0x%X"), dwError));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBSetClientName(IN LPCWSTR szName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetStatus(OUT OPCEVENTSERVERSTATUS** eventServerStatus)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBCreateEventSubscription(IN BOOL active, IN DWORD bufferTime, IN DWORD maxSize, IN OPCHANDLE clientSubscriptionHandle, IN REFIID riid, OUT LPUNKNOWN* unknown, OUT DWORD* revisedBufferTime, OUT DWORD* revisedMaxSize)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  act:%d btime:%lu maxSize:%lu hSub:0x%X"), active, bufferTime, maxSize, clientSubscriptionHandle));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQueryAvailableFilters(OUT DWORD* filterMask)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQueryEventCategories(IN DWORD eventType, OUT DWORD* count, OUT DWORD** eventCategories, OUT LPWSTR** eventCategoryDescs)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  type:0x%X"), eventType));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQueryConditionNames(IN DWORD eventCategory, OUT DWORD* count, OUT LPWSTR** conditionNames)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  category:0x%X"), eventCategory));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQuerySubConditionNames(IN LPWSTR conditionName, OUT DWORD* count, OUT LPWSTR** subConditionNames)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  condition:%s"), W2CT(conditionName)));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQuerySourceConditions(IN LPWSTR source, OUT DWORD* count, OUT LPWSTR** conditionNames)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  source:%s"), W2CT(source)));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBQueryEventAttributes(IN DWORD eventCategory, OUT DWORD* count, OUT DWORD** attrIDs, OUT LPWSTR** attrDescs, OUT VARTYPE** attrTypes)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  category:0x%X"), eventCategory));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBTranslateToItemIDs(IN LPWSTR source, IN DWORD eventCategory, IN LPWSTR conditionName, IN LPWSTR subconditionName, IN DWORD count, IN DWORD* assocAttrIDs, OUT LPWSTR** attrItemIDs, OUT LPWSTR** nodeNames, OUT CLSID** clsids)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  src:%s cat:0x%X cond:%s subC:%s"), W2CT(source), eventCategory, W2CT(conditionName), W2CT(subconditionName)));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetConditionState(IN LPWSTR source, IN LPWSTR conditionName, IN DWORD numEventAttrs, IN DWORD* attributeIDs, OUT OPCCONDITIONSTATE** conditionState)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  source:%s condition:%s"), W2CT(source), W2CT(conditionName)));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBEnableConditionByArea(IN DWORD numAreas, IN LPWSTR* areas)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  numAreas:%lu"), numAreas));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBEnableConditionBySource(IN DWORD numSources, IN LPWSTR* sources)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  numSources:%lu"), numSources));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBDisableConditionByArea(IN DWORD numAreas, IN LPWSTR* areas)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  numAreas:%lu"), numAreas));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBDisableConditionBySource(IN DWORD numSources, IN LPWSTR* sources)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  numSources:%lu"), numSources));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBAckCondition(IN DWORD count, IN LPWSTR acknowledgerID, IN LPWSTR comment, IN LPWSTR* source, IN LPWSTR* conditionName, IN FILETIME* activeTime, IN DWORD* cookie, OUT HRESULT** errors)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  ackID:%s comment:%s cnt:%d"), W2CT(acknowledgerID), W2CT(comment), count));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBCreateAreaBrowser(IN REFIID riid, IN LPUNKNOWN* unknown)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBAdvise(IN LPUNKNOWN unknown, OUT DWORD* cookie)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBUnadvise(IN DWORD cookie)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), cookie));
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetChildren(IN LPCWSTR parent, IN BOOL withAttributes, OUT LPWSTR* children)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBCollectChanges(IN LPCWSTR objects, IN DWORD doCollect)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetChanges(OUT LPWSTR* changes)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetStatistics(OUT LPWSTR* statistics)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetObject(IN LPCWSTR objectDescription, OUT LPWSTR* object)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBSetTraceOptions(IN LPCWSTR options)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetTraceOptions(OUT LPWSTR* options)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBSaveTraceOptions(void)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBGetTrace(OUT LPWSTR* trace)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBIsAvailablePriv(OUT BOOL* pbAvailable)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBLogon(IN LPCWSTR szUserID, IN LPCWSTR szPassword)
{
	return E_NOTIMPL;
}
HRESULT SOAeSServer::onBLogoff(void)
{
	return E_NOTIMPL;
}
void SOAeSServer::onESetLocaleID(IN HRESULT res, IN LCID dwLcid) {}
void SOAeSServer::onEGetLocaleID(HRESULT res, IN LCID* pdwLcid)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  lcid:0x%X"), *pdwLcid));
	}
}
void SOAeSServer::onEQueryAvailableLocaleIDs(IN HRESULT res, IN DWORD* pdwCount, IN LCID** pdwLcid)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *pdwCount));
	}
}
void SOAeSServer::onEGetErrorString(IN HRESULT res, IN HRESULT dwError, IN LPWSTR* ppString)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorString:%s"), *ppString));
	}
}
void SOAeSServer::onESetClientName(IN HRESULT res, IN LPCWSTR szName) {}
void SOAeSServer::onEGetStatus(IN HRESULT res, OUT OPCEVENTSERVERSTATUS** eventServerStatus) {}
void SOAeSServer::onECreateEventSubscription(IN HRESULT res, IN BOOL active, IN DWORD bufferTime, IN DWORD maxSize, IN OPCHANDLE clientSubscriptionHandle, IN REFIID riid, IN LPUNKNOWN* unknown, IN DWORD* revisedBufferTime, IN DWORD* revisedMaxSize) {}
void SOAeSServer::onEQueryAvailableFilters(IN HRESULT res, IN DWORD* filterMask)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  filterMask:0x%X"), *filterMask));
	}
}
void SOAeSServer::onEQueryEventCategories(IN HRESULT res, IN DWORD eventType, IN DWORD* count, IN DWORD** eventCategories, IN LPWSTR** eventCategoryDescs)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *count));
	}
}
void SOAeSServer::onEQueryConditionNames(IN HRESULT res, IN DWORD eventCategory, IN DWORD* count, IN LPWSTR** conditionNames)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *count));
	}
}
void SOAeSServer::onEQuerySubConditionNames(IN HRESULT res, IN LPWSTR conditionName, IN DWORD* count, IN LPWSTR** subConditionNames)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *count));
	}
}
void SOAeSServer::onEQuerySourceConditions(IN HRESULT res, IN LPWSTR source, IN DWORD* count, IN LPWSTR** conditionNames)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *count));
	}
}
void SOAeSServer::onEQueryEventAttributes(IN HRESULT res, IN DWORD eventCategory, IN DWORD* count, IN DWORD** attrIDs, IN LPWSTR** attrDescs, IN VARTYPE** attrTypes)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *count));
	}
}
void SOAeSServer::onETranslateToItemIDs(IN HRESULT res, IN LPWSTR source, IN DWORD eventCategory, IN LPWSTR conditionName, IN LPWSTR subconditionName, IN DWORD count, IN DWORD* assocAttrIDs, IN LPWSTR** attrItemIDs, IN LPWSTR** nodeNames, IN CLSID** clsids) {}
void SOAeSServer::onEGetConditionState(IN HRESULT res, IN LPWSTR source, IN LPWSTR conditionName, IN DWORD numEventAttrs, IN DWORD* attributeIDs, IN OPCCONDITIONSTATE** conditionState) {}
void SOAeSServer::onEEnableConditionByArea(IN HRESULT res, IN DWORD numAreas, IN LPWSTR* areas) {}
void SOAeSServer::onEEnableConditionBySource(IN HRESULT res, IN DWORD numSources, IN LPWSTR* sources) {}
void SOAeSServer::onEDisableConditionByArea(IN HRESULT res, IN DWORD numAreas, IN LPWSTR* areas) {}
void SOAeSServer::onEDisableConditionBySource(IN HRESULT res, IN DWORD numSources, IN LPWSTR* sources) {}
void SOAeSServer::onEAckCondition(IN HRESULT res, IN DWORD count, IN LPWSTR acknowledgerID, IN LPWSTR comment, IN LPWSTR* source, IN LPWSTR* conditionName, IN FILETIME* activeTime, IN DWORD* cookie, IN HRESULT** errors) {}
void SOAeSServer::onECreateAreaBrowser(IN HRESULT res, IN REFIID riid, IN LPUNKNOWN* unknown) {}
void SOAeSServer::onEAdvise(IN HRESULT res, IN LPUNKNOWN unknown, IN DWORD* cookie)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), *cookie));
	}
}
void SOAeSServer::onEUnadvise(IN HRESULT res, IN DWORD cookie) {}
void SOAeSServer::onEGetChildren(IN HRESULT res, IN LPCWSTR parent, IN BOOL withAttributes, IN LPWSTR* children) {}
void SOAeSServer::onECollectChanges(IN HRESULT res, IN LPCWSTR objects, IN DWORD doCollect) {}
void SOAeSServer::onEGetChanges(IN HRESULT res, IN LPWSTR* changes) {}
void SOAeSServer::onEGetStatistics(IN HRESULT res, IN LPWSTR* statistics) {}
void SOAeSServer::onEGetObject(IN HRESULT res, IN LPCWSTR objectDescription, OUT LPWSTR* object) {}
void SOAeSServer::onESetTraceOptions(IN HRESULT res, IN LPCWSTR options) {}
void SOAeSServer::onEGetTraceOptions(IN HRESULT res, OUT LPWSTR* options) {}
void SOAeSServer::onESaveTraceOptions(IN HRESULT res) {}
void SOAeSServer::onEGetTrace(IN HRESULT res, OUT LPWSTR* trace) {}
void SOAeSServer::onEIsAvailablePriv(IN HRESULT res, OUT BOOL* pbAvailable) {}
void SOAeSServer::onELogon(IN HRESULT res, IN LPCWSTR szUserID, IN LPCWSTR szPassword) {}
void SOAeSServer::onELogoff(IN HRESULT res) {}
#endif // SOFEATURE_COMIF_HOOKS



//-----------------------------------------------------------------------------
// SOAeSObjectRoot                                                            |
//-----------------------------------------------------------------------------

SOAeSObjectRoot::SOAeSObjectRoot(void)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8X| %s]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle(), _T("AeS Object Root"));
	setObjTraceId(id);
}



#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSObjectRoot::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(512);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_OBJECTROOT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnDateTime startTime;
		SOCmnString buffer;
		WORD majorVersion, minorVersion, patchVersion, buildNumber;
		entry->getVersionInfo(&majorVersion, &minorVersion, &patchVersion, &buildNumber);
		entry->getStartTime(&startTime);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VERSION_T, buffer.format(_T("%u.%2.2u.%u.%4.4u"), majorVersion, minorVersion, patchVersion, buildNumber));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VENDORINFO_T, entry->getVendorInfo());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_STARTTIME_T, startTime.toString(buffer));
		addDCOMCnfgWatchData(entry, szXML);
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#endif // SOAES
