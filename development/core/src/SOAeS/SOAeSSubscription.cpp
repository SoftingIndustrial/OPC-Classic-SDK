#include "stdafx.h"

#ifdef SOAES

#include "SOAeSComSubscription.h"
#include <opc_ae.h>

static TCHAR s_emptyString[2] = _T("");


//-----------------------------------------------------------------------------
// SOAeSSubscription                                                          |
//-----------------------------------------------------------------------------

SOAeSSubscription::SOAeSSubscription(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_SUBSCRIPTION)
{
	m_me = NULL;
	m_bufferTime = 0;
	m_maxSize = 0;
	m_clientHandle = 0;
	m_active = FALSE;
	m_retAttrList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_SYNCED);
	m_sendEventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_sendTime = 0;
	m_flags = SOAESSUBSCRIPTION_FLAG_FORCE_ASYNC_SEND_EVENTS;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SUBSCRIPTION);
#endif
}


SOAeSSubscription::~SOAeSSubscription(void)
{
	SOAeSSubscriptionReturnedAttributes* retAttr;
	SOCmnListPosition pos;
	// clear returned attributes
	pos = m_retAttrList.getStartPosition();

	while (pos)
	{
		retAttr = m_retAttrList.getNext(pos);
		delete retAttr;
	}

	m_retAttrList.destroy();
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_SUBSCRIPTION);
#endif
}


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
BOOL SOAeSSubscription::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOCmnElement> prevPar = getParentBranch();

	if ((prevPar.isNotNull()) && (parent == NULL))
	{
		// remove from tree
		if ((prevPar->getCollectChanges()) || (getCollectChanges()))
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchRemoveObject(prevPar, this);
		}
	}

#endif
	return SOCmnElement::setParentBranch(parent);
} // setParentBranch


//-----------------------------------------------------------------------------
// getMe
//
// - get reference to COM object
//
// returns:
//		pointer of COM object
//
void* SOAeSSubscription::getMe(void)
{
	m_meSync.lock();
	SOSrvComObject<SOAeSComSubscription> *sus = (SOSrvComObject<SOAeSComSubscription> *)m_me;

	if (sus)
	{
		sus->tempAddRef();
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
ULONG SOAeSSubscription::releaseMe(
	IN void* /* me */)  // pointer to COM object
{
	SOSrvComObject<SOAeSComSubscription> *sus = (SOSrvComObject<SOAeSComSubscription> *)m_me;

	if (sus)
	{
		return sus->tempRelease();
	}

	return 0;
} // releaseMe


//-----------------------------------------------------------------------------
// setActive
//
// - set activation state
// - clear event queue
//
// returns:
//		TRUE
//
BOOL SOAeSSubscription::setActive(
	IN BOOL active) // activation state
{
	onSetActive(active);
	m_active = (BYTE)active;
	m_sendEventList.lock();
	m_sendEventList.removeAll();
	m_sendEventList.unlock();
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, m_active));
	}

#endif
	return TRUE;
} // setActive


//-----------------------------------------------------------------------------
// setBufferTime
//
// - set buffer time
//
// returns:
//		TRUE
//
BOOL SOAeSSubscription::setBufferTime(
	IN DWORD bufferTime)    // buffer time
{
	if (SOCMN_TIME_40DAYS < bufferTime)
	{
		bufferTime = SOCMN_TIME_40DAYS;
	}

	onSetBufferTime(bufferTime);
	m_bufferTime = bufferTime;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_BUFFERTIME_T, buffer.format(_T("%lu"), m_bufferTime));
	}

#endif
	return TRUE;
} // setBufferTime


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOAeSSubscription::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		SOCmnPointer<SOAeSServer> srv = (SOAeSServer*)getParentBranch();

		if (!!srv)
		{
			id.format(_T("[%s|%8.8X| %s]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle(), (LPCTSTR)srv->getClientName());
		}
		else
		{
			id.format(_T("[%s|%8.8X|]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle());
		}

		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// setMaxSize
//
// - set max size
//
// returns:
//		TRUE
//
BOOL SOAeSSubscription::setMaxSize(
	IN DWORD maxSize)   // max size
{
	if (maxSize > 0xFFFFFFFD)
	{
		maxSize = 0xFFFFFFFD;
	}

	onSetMaxSize(maxSize);
	m_maxSize = maxSize;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_MAXSIZE_T, buffer.format(_T("%lu"), m_maxSize));
	}

#endif
	return TRUE;
} // setMaxSize


//-----------------------------------------------------------------------------
// setClientHandle
//
// - set client handle
//
// returns:
//		TRUE
//
BOOL SOAeSSubscription::setClientHandle(
	IN OPCHANDLE clientHandle)  // client handle
{
	onSetClientHandle(clientHandle);
	m_clientHandle = clientHandle;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_CLIENTHANDLE_T, buffer.format(_T("%lu"), m_clientHandle));
	}

#endif
	return TRUE;
} // setClientHandle


//-----------------------------------------------------------------------------
// onSetActive
//
// - called before activation state is set
// - default implementation: do nothing
//
void SOAeSSubscription::onSetActive(
	IN BOOL /* active */)   // activation state
{} // onSetActive


//-----------------------------------------------------------------------------
// onSetBufferTime
//
// - called before buffer time is set
// - default implementation: do nothing
//
void SOAeSSubscription::onSetBufferTime(
	IN DWORD /* bufferTime */)  // buffer time
{} // onSetBufferTime


//-----------------------------------------------------------------------------
// onSetMaxSize
//
// - called before max size is set
// - default implementation: do nothing
//
void SOAeSSubscription::onSetMaxSize(
	IN DWORD /* maxSize */) // max size
{} // onSetMaxSize


//-----------------------------------------------------------------------------
// onSetClientHandle
//
// - called before client handle is set
// - default implementation: do nothing
//
void SOAeSSubscription::onSetClientHandle(
	IN OPCHANDLE /* clientHandle */)    // client handle
{} // onSetClientHandle


//-----------------------------------------------------------------------------
// createConnectedEventSinkInterfaceList
//
// - create a list of all connected event sink interfaces
//
// returns:
//		TRUE
//
BOOL SOAeSSubscription::createConnectedEventSinkInterfaceList(
	OUT SOCmnList<IOPCEventSink> *eventSinkList)    // interface list
{
	if (eventSinkList == NULL)
	{
		return FALSE;
	}

	SOSrvComObject<SOAeSComSubscription> *sus = (SOSrvComObject<SOAeSComSubscription> *)getMe();

	if (!sus)
	{
		return FALSE;
	}

	IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>* p = (SOAeSComSubscription*)sus;
	IUnknown** pp;
	eventSinkList->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_INTERFACE);
	sus->getAdvisesSync()->lock();
	pp = p->m_vec.begin();

	while (pp < p->m_vec.end())
	{
		if (*pp != NULL)
		{
			eventSinkList->add((IOPCEventSink*)*pp);
		}

		pp++;
	}

	sus->getAdvisesSync()->unlock();
	releaseMe(sus);
	eventSinkList->autoUnlock();
	return TRUE;
} // createConnectedEventSinkInterfaceList


//-----------------------------------------------------------------------------
// getReturnedAttributes
//
// - get the returned attributes list for a event category
//
// returns:
//		returned attributes list
//
SOCmnList<SOAeSAttribute> *SOAeSSubscription::getReturnedAttributeList(
	IN DWORD category)  // category
{
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnList<SOCmnElement> catList(eventSpace->getBranchList());
	SOCmnPointer<SOAeSCategory> cat;
	// get event category
	cat = (SOAeSCategory*)catList.findKey(category);

	if (!cat)
	{
		return NULL;
	}

	catList.autoUnlock();
	SOCmnList<SOAeSSubscriptionReturnedAttributes> retAttrList(m_retAttrList);
	SOAeSSubscriptionReturnedAttributes* retAttr;
	// find existing list entry
	retAttr = retAttrList.findKey(category);

	if (!retAttr)
	{
		retAttr = new SOAeSSubscriptionReturnedAttributes();
		retAttr->m_category = cat;
		retAttrList.add(retAttr, category);
	}

	return (SOCmnList<SOAeSAttribute> *)&retAttr->m_attributeList;
} // getReturnedAttributes


//-----------------------------------------------------------------------------
// selectReturnedAttributes
//
// - set the returned attributes for a event category
//
// returns:
//		S_OK         - all attributes set
//		E_INVALIDARG - invalid argument
//
BOOL SOAeSSubscription::selectReturnedAttributes(
	IN DWORD category,      // event category
	IN DWORD attrCount,     // number of attributes
	IN DWORD* attrArray)    // attribute array
{
	HRESULT res = S_OK;
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnList<SOCmnElement> catList(eventSpace->getBranchList());
	SOCmnPointer<SOAeSCategory> cat;
	// get event category
	cat = (SOAeSCategory*)catList.findKey(category);

	if (!cat)
	{
		return E_INVALIDARG;
	}
	
	catList.autoUnlock();
	// check attribute ids
	SOCmnList<SOCmnElement> attrList(cat->getAdditionalList());
	SOCmnPointer<SOAeSAttribute> attr;
	DWORD i;

	DWORD validAttrCount = 0;
	for (i = 0; i < attrCount; i++)
	{
		attr = (SOAeSAttribute*)attrList.findKey(attrArray[i]);

		if (!!attr)
		{
			++validAttrCount;
		}
	}

	if ((validAttrCount == 0) && (attrCount > 0))
	{
		return E_INVALIDARG;
	}

	SOCmnList<SOAeSAttribute> retAttrList(getReturnedAttributeList(category));
	// select the attributes
	retAttrList.removeAll();

	for (i = 0; i < attrCount; i++)
	{
		attr = (SOAeSAttribute*)attrList.findKey(attrArray[i]);

		if (attr.isNotNull())
		{
			retAttrList.add((SOAeSAttribute*)attr);
		}
	}

	onSelectReturnedAttributes((SOAeSCategory*)cat, &retAttrList);
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_SELECTEDATTRIBUTES_T, returnedAttribues2string(buffer));
	}

#endif
	return res;
} // selectReturnedAttributes


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// returnedAttribues2string
//
// - convert the returned attributes to a string
//
// returns:
//		attributes string
//
LPCTSTR SOAeSSubscription::returnedAttribues2string(IN SOCmnString& buffer)
{
	// string format: [<category>:<attribute1>,<attribute2>...][...]
	SOCmnList<SOAeSSubscriptionReturnedAttributes> retAttrList(m_retAttrList);
	SOAeSSubscriptionReturnedAttributes* retAttr;
	SOCmnListPosition retAttrPos;
	SOCmnString catBuf;
	SOCmnList<SOAeSAttribute> attributeList;
	SOAeSAttribute* attribute;
	SOCmnListPosition attributePos;
	BOOL addComma;
	buffer.empty();
	retAttrPos = retAttrList.getStartPosition();

	while (retAttrPos)
	{
		retAttr = retAttrList.getNext(retAttrPos);
		attributeList = retAttr->m_attributeList;

		if (!attributeList.isEmpty())
		{
			catBuf.format(_T("[%s:"), retAttr->m_category->getDescription());
			attributePos = attributeList.getStartPosition();
			addComma = FALSE;

			while (attributePos)
			{
				if (addComma)
				{
					catBuf += _T(",");
				}

				attribute = attributeList.getNext(attributePos);
				catBuf += attribute->getDescription();
				addComma = TRUE;
			}

			catBuf += _T("]");
			buffer += catBuf;
		}
	}

	return buffer;
}

#endif

//-----------------------------------------------------------------------------
// onSelectReturnedAttributes
//
// - called after the returned attributes are set
// - default implementation: do nothing
//
void SOAeSSubscription::onSelectReturnedAttributes(
	IN SOAeSCategory* /* category */,
	IN SOCmnList<SOAeSAttribute>* /* attributeList */)
{} // onSelectReturnedAttributes


//-----------------------------------------------------------------------------
// addEvent
//
// - filter event and add it to the event list
//
// returns:
//		TRUE  - event not filtered out
//		FALSE - event filtered out
//
BOOL SOAeSSubscription::addEvent(
	IN SOAeSEvent* event)   // event
{
	if (!event)
	{
		return FALSE;
	}

	// check if subscription is active - only active subscriptions send event
	// notifiactions to the clients
	if (!getActive())
	{
		return FALSE;
	}

	// check if filter matches
	if (filterEvent(event))
	{
		// event matches the filter
		// send it to the client
		m_sendEventList.lock();
		m_sendEventList.add(event);
		m_sendEventList.unlock();
		return TRUE;
	}

	// event filtered out
	return FALSE;
} // addEvent


//-----------------------------------------------------------------------------
// sendEvents
//
// - send events of the event list to the client
//
void SOAeSSubscription::sendEvents(void)
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();

	if ((getFlags() & SOAESSUBSCRIPTION_FLAG_FORCE_ASYNC_SEND_EVENTS) ||
		(entry->getCoInit() != COINIT_MULTITHREADED))
	{
		// STA or forced
		SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

		if (parent.isNotNull())
		{
			parent->signalWorkEvent();    // STA work only can be done from toolkit
		}
	}
	else
	{
		// MTA
		DWORD listCnt;
		BOOL end = FALSE;
		DWORD now = ::GetTickCount();

		do
		{
			// send events
			if ((listCnt = doSendEvents(FALSE)) > 0)
			{
				// not all events send
				// check max size:
				// if count is greate than maxSize send the next events
				// if not check the send time
				if ((getMaxSize() == 0) || (listCnt <= getMaxSize()))
				{
					// get time till next send is allowed
					if (checkSendTime(now) > 0)
					{
						end = TRUE; // not now
						SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

						if (parent.isNotNull())
						{
							parent->signalWorkEvent();    // still work to do
						}
					}
				}
			}
			else
			{
				end = TRUE;    // all events of the list send
			}
		}
		while (!end);
	}
} // sendEvents


//-----------------------------------------------------------------------------
// fireEvent
//
// - filter event and send it to the client
//
// returns:
//		TRUE  - event not filtered out
//		FALSE - event filtered out
//
BOOL SOAeSSubscription::fireEvent(
	IN SOAeSEvent* event)   // event
{
	if (!addEvent(event))
	{
		return FALSE;
	}

	// event matches the filter
	// send it to the client
	sendEvents();
	return TRUE;
} // fireEvent


//-----------------------------------------------------------------------------
// fireEvents
//
// - filter event and send it to the client
//
// returns:
//		TRUE  - not all events filtered out
//		FALSE - all events filtered out
//
BOOL SOAeSSubscription::fireEvents(
	IN SOCmnList<SOAeSEvent> *eventList)
{
	if (!eventList)
	{
		return FALSE;
	}

	// check if subscription is active - only active subscriptions send event
	// notifiactions to the clients
	if (!getActive())
	{
		return FALSE;
	}

	SOCmnListPosition pos;
	SOAeSEvent* event;
	BOOL ret = FALSE;
	pos = eventList->getStartPosition();

	while (pos)
	{
		event = eventList->getNext(pos);

		// check if filter matches
		if (filterEvent(event))
		{
			m_sendEventList.lock();
			m_sendEventList.add(event);
			m_sendEventList.unlock();
			ret = TRUE;
		}
	}

	if (m_sendEventList.getCount())
	{
		// at least one event matches the filter, or
		// -> send them to the client
		sendEvents();
	}

	// event filtered out
	return ret;
} // fireEvents


//-----------------------------------------------------------------------------
// doSendEvents
//
// - send event to the client
//
// returns:
//		number of entries in the event list
//
DWORD SOAeSSubscription::doSendEvents(
	IN BOOL refresh)    // take the refresh list
{
	SOCmnList<SOAeSEvent> eventList;
	DWORD sendSpan;
	DWORD refreshConnection = 0;

	// get the right event list
	if (!refresh)
	{
		sendSpan = checkSendTime(GetTickCount());
		eventList = m_sendEventList;
	}
	else
	{
		sendSpan = 0;
		SOCmnPointer<SOAeSRefresh> refObj = getRefresh();

		if (!refObj)
		{
			return 0;   // no refresh -> no events in refresh list
		}

		eventList = refObj->getEventList();
		refreshConnection = refObj->getConnection();
	}

	DWORD count = eventList.getCount(); // locks list
	DWORD sizeSend;

	// check event list
	if (!refresh)
		if (count == 0)
		{
			return 0;    // no event to send
		}

	// check send and buffer time and max size
	if ((0 != sendSpan) && ((m_maxSize == 0) || (count <= m_maxSize)))
	{
		return count;
	}

	if (m_maxSize != 0)
	{
		sizeSend = (m_maxSize < count) ? m_maxSize : count;
	}
	else
	{
		sizeSend = count;
	}

	SOAeSEvent* event;
	SOCmnListPosition pos;
	DWORD i;
	BOOL lastRefresh = FALSE;

	// check if last refresh notification
	if ((refresh) && (sizeSend == count))
	{
		lastRefresh = TRUE;
	}

	SOSrvComObject<SOAeSComSubscription> *sus = (SOSrvComObject<SOAeSComSubscription> *)getMe();

	if ((sus != NULL) && (SOSRV_ADVISE_CHECK_DEMO_AE))
	{
		// DCOM
		IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>* p = (SOAeSComSubscription*)sus;
		IUnknown** pp;
		IOPCEventSink* eventSink;
		DWORD clientHandle = getClientHandle();
		ONEVENTSTRUCT* eventData = (ONEVENTSTRUCT*)CoTaskMemAlloc(sizeof(ONEVENTSTRUCT) * sizeSend);

		if (eventData)
		{
			memset(eventData, 0, sizeof(ONEVENTSTRUCT) * sizeSend);
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process event callback")))
			pos = eventList.getStartPosition();
			i = 0;

			while ((pos) && (i < sizeSend))
			{
				event = eventList.getNext(pos);

				if (!fillEventStruct(event, eventData + i))
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SRV_THREAD, (_T("doSendEvents: Out of memory")));
					releaseMe(sus);
					freeEventStructArray(sizeSend, eventData);
					return 0;
				}

				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, event->getObjTraceId(), (_T("send source:%s; message:%s; severity:%lu"),
					(LPCTSTR)event->getSourceName(), (LPCTSTR)event->getMessage(), event->getSeverity()));
				i++;
			}
		}
		else
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SRV_THREAD, (_T("doSendEvents: Out of memory")));
			releaseMe(sus);
			return 0;
		}

		// remove events from the list
		if (sizeSend == count)
		{
			eventList.removeAll();
		}
		else
		{
			for (i = 0; i < sizeSend; i++)
			{
				eventList.removeHead();
			}
		}

		eventList.autoUnlock();

		// set send time
		if (!refresh)
		{
			if (0 == (m_sendTime = GetTickCount()))
			{
				m_sendTime = 1;
			}
		}

		sus->getAdvisesSync()->lock();
#if _ATL_VER >= 0x0700
		int vecIdx = 0;
#endif
		pp = p->m_vec.begin();

		while (pp < p->m_vec.end())
		{
#if _ATL_VER >= 0x0700
			vecIdx++;
#endif
			eventSink = (IOPCEventSink*)*pp;
			pp++;

			if (eventSink)
			{
				if (refresh)
				{
#if _ATL_VER >= 0x0700

					if (vecIdx != (int)refreshConnection)
#else
					if ((DWORD)eventSink != refreshConnection)
#endif
						continue;

					SOCmnPointer<SOAeSRefresh> refObj = getRefresh();

					if (!refObj)
					{
						continue;
					}

					if (refObj->getObjectState() == SOAeSRefresh::canceled)
					{
						continue;
					}
				}

				// send event notification
				SOCmnPointer<SOAeSServer> server = (SOAeSServer*)getParentBranch();
				SOCMN_TRY
				{
					HRESULT res;
					res = eventSink->OnEvent(clientHandle, refresh, lastRefresh, sizeSend, eventData);
					_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call event callback for %lu events; refresh: %s"), sizeSend, BOOL2STR(refresh)))
					_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCEventCallback::OnEvent [0x%X]"), res));

					if (SOSRV_IS_RPC_ERROR(res))
					{
						// client advise object not available anymore
						if (server.isNotNull())
						{
							server->disconnectClient();    // -> disconnect client
						}
					}
				}
				SOCMN_CATCH(...)
				{
					// exception in callback (only inproc)
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnEvent")));
				}

				if (!!server)
				{
					::GetSystemTimeAsFileTime(&server->m_lastUpdateTime);
#ifdef SOFEATURE_WATCH

					if (server->getCollectChanges() & SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES)
					{
						SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
						SOCmnString buffer;
						SOSrvWatch::attributeChange(server, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, conv->filetime2string(buffer, &server->m_lastUpdateTime));
					}

#endif
				}
			}
		}

		sus->getAdvisesSync()->unlock();
		releaseMe(sus);
		freeEventStructArray(sizeSend, eventData);
		return count - sizeSend;
	}
	else
	{
		// no COM object for the subscription or demo timout
#ifdef SOFEATURE_TUNNEL
		SOCmnPointer<SOAeSServer> server = (SOAeSServer*)getParentBranch();

		if ((server.isNotNull()) && (SOSRV_TP_ADVISE_CHECK_DEMO))
		{
			SOCmnPointer<SOSrvTPConnection> tpCon = server->getTPConnection();

			if (tpCon.isNotNull())
			{
				// it is a tunnel subscription object
				SOCmnPointer<SOCmnTPCall> tpCall;
				SOCmnPointer<SOCmnTP_Marshaler> notification;
				FILETIME timeConvHlp;
				SOCmnPointer<SOAeSCategory> cat;
				SOCmnList<SOAeSAttribute> attrList;
				DWORD attrCount;
				BOOL doSendTP = TRUE;
				SOCMN_ALLOCATE_OBJECT_C(tpCall, SOCmnTPCall(tpCon), (SOCmnTPCall*));
				notification = tpCall->getSndMsg();
				notification->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256 * sizeSend);
				notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
				notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
				notification->MarshalLong(getClientHandle());                     // object id
				notification->MarshalShort(TP_INTERFACE_AE_CALLBACK);             // interface id
				notification->MarshalChar(TP_FUNCTION_AE_CALLBACK_ONEVENT);       // function id
				notification->MarshalChar(0);                                     // flags
				notification->MarshalChar((BYTE)refresh);
				notification->MarshalChar((BYTE)lastRefresh);
				notification->MarshalLong((BYTE)sizeSend);                              // event count
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process event callback")))
				pos = eventList.getStartPosition();
				i = 0;

				while ((pos) && (i < sizeSend))
				{
					event = eventList.getNext(pos);
					cat = event->getCategory();
					attrList = getReturnedAttributeList(cat->getNameInt());
					attrCount = attrList.getCount();
					notification->MarshalShort(event->getChangeMask());
					notification->MarshalShort(event->getNewState());
					notification->MarshalString(event->getSourceName());
					event->getOccurrenceTime(&timeConvHlp);
					notification->MarshalDate(SOCmnDateTime(&timeConvHlp));
					notification->MarshalString(event->getMessage());
					notification->MarshalLong(cat->getEventType());
					notification->MarshalLong(cat->getNameInt());
					notification->MarshalLong(event->getSeverity());
					notification->MarshalString(event->getConditionName());
					notification->MarshalString(event->getSubConditionName());
					notification->MarshalShort(event->getQuality());
					notification->MarshalChar((BYTE)event->getAckRequired());
					event->getActiveTime(&timeConvHlp);
					notification->MarshalDate(SOCmnDateTime(&timeConvHlp));
					notification->MarshalLong(event->getCookie());
					notification->MarshalString(event->getActorID());
					notification->MarshalLong(attrCount);

					if (attrCount)
					{
						SOCmnList<SOAeSAttributeValue> attrValList;
						SOCmnPointer<SOAeSAttributeValue> attrVal;
						SOCmnListPosition posAttr;
						SOAeSAttribute* attr;
						SOCmnVariant attrVariant;
						DWORD jj = 0;
						attrValList = event->getAttributeValueList();
						posAttr = attrList.getStartPosition();

						while (posAttr)
						{
							attr = attrList.getNext(posAttr);
							attrVal = attrValList.findKey(attr->getNameInt());

							if (!!attrVal)
							{
								attrVal->getValue(attr->getDatatype(), (LPVARIANT)attrVariant);
							}

							notification->MarshalVariant(attrVariant);
							attrVariant.clear();
							jj++;
						}
					}

					_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, event->getObjTraceId(), (_T("send source:%s; message:%s; severity:%lu"), event->getSourceName(), event->getMessage(), event->getSeverity()));
					i++;
				}

				// remove events from the list
				if (sizeSend == count)
				{
					eventList.removeAll();
				}
				else
				{
					for (i = 0; i < sizeSend; i++)
					{
						eventList.removeHead();
					}
				}

				eventList.autoUnlock();

				// set send time
				if (!refresh)
				{
					if (0 == (m_sendTime = GetTickCount()))
					{
						m_sendTime = 1;
					}
				}

				if (refresh)
				{
					SOCmnPointer<SOAeSRefresh> refObj = getRefresh();

					if (!refObj)
					{
						doSendTP = FALSE;
					}

					if (refObj->getObjectState() == SOAeSRefresh::canceled)
					{
						doSendTP = FALSE;
					}
				}

				if (doSendTP)
				{
					// send event notification
					HRESULT res;
					res = tpCon->invoke(tpCall, FALSE);
					_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call event callback for %lu events; refresh: %s"), sizeSend, BOOL2STR(refresh)))
					_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCEventCallback::OnEvent [0x%X]"), res));

					if (FAILED(res))
					{
						// can't send notification
						server->disconnectClient(); // -> disconnect client
					}
				}

				::GetSystemTimeAsFileTime(&server->m_lastUpdateTime);
#ifdef SOFEATURE_WATCH

				if (server->getCollectChanges() & SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES)
				{
					SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
					SOCmnString buffer;
					SOSrvWatch::attributeChange(server, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, conv->filetime2string(buffer, &server->m_lastUpdateTime));
				}

#endif
				return count - sizeSend;
			}
		}

#endif
		// -> termination phase or demo timout; reject all events
		eventList.removeAll();
		return 0;
	}
} // doSendEvents


//-----------------------------------------------------------------------------
// fillEventStruct
//
// - fill the event struct with the event object attributes
//
// returns:
//		TRUE  - struct filled
//		FALSE - out of memory
//
BOOL SOAeSSubscription::fillEventStruct(
	IN SOAeSEvent* event,           // event
	IN ONEVENTSTRUCT* eventStruct)  // event struct
{
	_ASSERTION((event), "The method called with an invalid event object");
	_ASSERTION((eventStruct), "The method called with an invalid eventStruct memory");
	SOCmnPointer<SOAeSCategory> cat = event->getCategory();
	_ASSERTION(((SOAeSCategory*) cat), "Before calling fillEventStruct the category must be set!");
	SOCmnList<SOAeSAttributeValue> attrValList;
	SOCmnPointer<SOAeSAttributeValue> attrVal;
	SOCmnListPosition pos;
	SOCmnList<SOAeSAttribute> attrList(getReturnedAttributeList(cat->getNameInt()));
	SOAeSAttribute* attr;
	_ASSERTION((attrList.isInit()), "Before calling fillEventStruct the attribute list must be initialized");
	eventStruct->wChangeMask = event->getChangeMask();
	eventStruct->wNewState = event->getNewState();
	eventStruct->szSource = SOCmnString::cloneToWCHAR(event->getSourceName());

	if (!eventStruct->szSource)
	{
		return FALSE;
	}

	event->getOccurrenceTime(&eventStruct->ftTime);
	eventStruct->szMessage = SOCmnString::cloneToWCHAR(event->getMessage());

	if (!eventStruct->szMessage)
	{
		return FALSE;
	}

	eventStruct->dwEventType = cat->getEventType();
	eventStruct->dwEventCategory = cat->getNameInt();
	eventStruct->dwSeverity = event->getSeverity();
	eventStruct->szConditionName = SOCmnString::cloneToWCHAR(event->getConditionName());

	if (!eventStruct->szConditionName)
	{
		return FALSE;
	}

	eventStruct->szSubconditionName = SOCmnString::cloneToWCHAR(event->getSubConditionName());

	if (!eventStruct->szSubconditionName)
	{
		return FALSE;
	}

	eventStruct->wQuality = event->getQuality();
	eventStruct->bAckRequired = event->getAckRequired();
	event->getActiveTime(&eventStruct->ftActiveTime);
	eventStruct->dwCookie = event->getCookie();
	eventStruct->szActorID = SOCmnString::cloneToWCHAR(event->getActorID());

	if (!eventStruct->szActorID)
	{
		return FALSE;
	}

	attrValList = event->getAttributeValueList();
	_ASSERTION((attrValList.isInit()), "Before calling fillEventStruct the attribute value list must be initialized");
	eventStruct->dwNumEventAttrs = attrList.getCount();

	if (eventStruct->dwNumEventAttrs)
	{
		DWORD i = 0;
		eventStruct->pEventAttributes = (LPVARIANT)CoTaskMemAlloc(sizeof(VARIANT) * eventStruct->dwNumEventAttrs);

		if (!eventStruct->pEventAttributes)
		{
			return FALSE;
		}

		pos = attrList.getStartPosition();

		while (pos)
		{
			attr = attrList.getNext(pos);
			attrVal = attrValList.findKey(attr->getNameInt());

			if (!!attrVal)
			{
				attrVal->getValue(attr->getDatatype(), &eventStruct->pEventAttributes[i]);
			}
			else
			{
				::VariantInit(&eventStruct->pEventAttributes[i]);
			}

			i++;
		}
	}

	return TRUE;
} // fillEventStruct


//-----------------------------------------------------------------------------
// freeEventStructArray
//
// - free the event struct array and all included data
//
void SOAeSSubscription::freeEventStructArray(
	IN DWORD numEvents,             // number of event structs
	IN ONEVENTSTRUCT* eventData)    // event struct array
{
	DWORD i;
	DWORD j;

	for (i = 0; i < numEvents; i++)
	{
		if (eventData[i].szSource)
		{
			CoTaskMemFree(eventData[i].szSource);
		}

		if (eventData[i].szMessage)
		{
			CoTaskMemFree(eventData[i].szMessage);
		}

		if (eventData[i].szConditionName)
		{
			CoTaskMemFree(eventData[i].szConditionName);
		}

		if (eventData[i].szSubconditionName)
		{
			CoTaskMemFree(eventData[i].szSubconditionName);
		}

		if (eventData[i].szActorID)
		{
			CoTaskMemFree(eventData[i].szActorID);
		}

		if (eventData[i].pEventAttributes)
		{
			for (j = 0; j < eventData[i].dwNumEventAttrs; j++)
			{
				SOCmnVariant::variantClear(&(eventData[i].pEventAttributes[j]));
			}

			CoTaskMemFree(eventData[i].pEventAttributes);
		}
	}

	CoTaskMemFree(eventData);
} // freeEventStructArray


//-----------------------------------------------------------------------------
// checkSendTime
//
// - check if it's OK to send a new event notification
//
// returns:
//		time in milliseconds until next send is allowed
//
DWORD SOAeSSubscription::checkSendTime(
	IN DWORD now)   // current time
{
	if ((m_bufferTime != 0) && (m_sendTime != 0))
	{
		DWORD span = ::getTimeSpan(m_sendTime, now);

		if (span < m_bufferTime)
		{
			return m_bufferTime - span;
		}

		m_sendTime = 0;
	}

	return 0;
} // checkSendTime


//-----------------------------------------------------------------------------
// createRefresh
//
// - create refresh object
//
// returns:
//		NULL - refresh already in progress
//      pointer to new refresh object
//
SOAeSRefresh* SOAeSSubscription::createRefresh(
	IN DWORD connection)    // advise connection
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (!!m_refresh)
	{
		return NULL;    // already refresh in progress!
	}

	SOCMN_ALLOCATE_OBJECT_C(m_refresh, SOAeSRefresh(this, connection), (SOAeSRefresh*))
	SOAeSRefresh* refresh = (SOAeSRefresh*)m_refresh;
	refresh->addRef();
	return refresh;
} // createRefresh


//-----------------------------------------------------------------------------
// cancelRefresh
//
// - cancel refresh
//
// returns:
//		S_OK   - refresh cancles
//      E_FAIL - can't cancle refresh
//
HRESULT SOAeSSubscription::cancelRefresh(
	IN DWORD connection)    // advise connection
{
	HRESULT res;
	SOCmnPointer<SOAeSRefresh> refresh = getRefresh();

	if (!!refresh)
	{
		if (refresh->getConnection() == connection)
		{
			res = refresh->cancel();
			releaseRefresh();
		}
		else
		{
			res = E_FAIL;
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // cancelRefresh


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
HRESULT SOAeSSubscription::queryInterface(
	IN REFIID /* iid */,        // interface id
	IN void** /* object */)     // interface pointer
{
	return E_NOTIMPL;    // queryInterface
}


#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// checkAuthorizationDCOM
//
// - check if the call to the COM interface function is authorizated
//
// returns:
//   S_OK           - allowed access
//   E_ACCESSDENIED - denied access
//
HRESULT SOAeSSubscription::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
} // checkAuthorizationDCOM
#endif


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSSubscription::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_SUBSCRIPTION);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		BOOL addComma;
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_MAXSIZE_T, getMaxSize());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_BUFFERTIME_T, getBufferTime());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, getActive()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLIENTHANDLE_T, getClientHandle());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SELECTEDATTRIBUTES_T, returnedAttribues2string(buffer));
		{
			// filter
			DWORD eventType;
			SOCmnDWORDList eventCategoryList;
			DWORD lowSeverity;
			DWORD highSeverity;
			SOCmnStringList areaList;
			SOCmnStringList sourceList;
			SOCmnListPosition pos;

			if (SUCCEEDED(getFilter(&eventType, &eventCategoryList, &lowSeverity, &highSeverity, &areaList, &sourceList)))
			{
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_FILTERSEVERITY_T, buffer.format(_T("%lu - %lu"), lowSeverity, highSeverity));
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_FILTERTYPE_T, conv->eventTypeFilter2string(buffer, eventType));

				if (!areaList.isEmpty())
				{
					addComma = FALSE;
					buffer.empty();
					pos = areaList.getStartPosition();

					while (pos)
					{
						if (addComma)
						{
							buffer += _T(",");
						}

						buffer += areaList.getNext(pos);
						addComma = TRUE;
					}

					SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_FILTERAREA_T, buffer);
				}

				if (!sourceList.isEmpty())
				{
					addComma = FALSE;
					buffer.empty();
					pos = sourceList.getStartPosition();

					while (pos)
					{
						if (addComma)
						{
							buffer += _T(",");
						}

						buffer += sourceList.getNext(pos);
						addComma = TRUE;
					}

					SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_FILTERSOURCE_T, buffer);
				}

				if (!eventCategoryList.isEmpty())
				{
					SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
					SOCmnPointer<SOAeSEventSpaceRoot> esRoot = entry->getEventSpaceRoot();
					SOCmnPointer<SOAeSCategory> category;
					addComma = FALSE;
					buffer.empty();
					pos = eventCategoryList.getStartPosition();

					while (pos)
					{
						category = esRoot->findCategory(eventCategoryList.getNext(pos));

						if (category.isNotNull())
						{
							if (addComma)
							{
								buffer += _T(",");
							}

							buffer += category->getDescription();
							addComma = TRUE;
						}
					}

					SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_FILTERCATEGORY_T, buffer);
				}
			}
		}
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif  //  SOFEATURE_WATCH


#ifdef SOFEATURE_COMIF_HOOKS
// interface function callbacks
HRESULT SOAeSSubscription::onBSetFilter(IN DWORD eventType, IN DWORD numCategories, IN DWORD* eventCategoryArray, IN DWORD lowSeverity, IN DWORD highSeverity, IN DWORD numAreas, IN LPWSTR* areaArray, IN DWORD numSources, IN LPWSTR* sourceArray)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  eType:0x%X lSev:%d hSev:%d nC:%d nA:%d nS:%d"), eventType, lowSeverity, highSeverity, numCategories, numAreas, numSources));
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBGetFilter(OUT DWORD* eventType, OUT DWORD* numCategories, OUT DWORD** eventCategoryArray, OUT DWORD* lowSeverity, OUT DWORD* highSeverity, OUT DWORD* numAreas, OUT LPWSTR** areaArray, OUT DWORD* numSources, OUT LPWSTR** sourceArray)
{
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBSelectReturnedAttributes(IN DWORD eventCategory, IN DWORD count, IN DWORD* attributeIDs)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  category:0x%X cnt:%d"), eventCategory, count));
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBGetReturnedAttributes(IN DWORD eventCategory, OUT DWORD* count, OUT DWORD** attributeIDs)
{
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBRefresh(IN DWORD connection)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  connection:0x%X"), connection));
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBCancelRefresh(IN DWORD connection)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  connection:0x%X"), connection));
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBGetState(OUT BOOL* active, OUT DWORD* bufferTime, OUT DWORD* maxSize, OUT OPCHANDLE* clientSubscription)
{
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBSetState(IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE clientSubscription, OUT DWORD* revisedBufferTime, OUT DWORD* revisedMaxSize)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  act:%d btime:%lu maxSize:%lu hSub:0x%X"), active, bufferTime, maxSize, clientSubscription));
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBAdvise(IN LPUNKNOWN unknown, OUT DWORD* cookie)
{
	return E_NOTIMPL;
}
HRESULT SOAeSSubscription::onBUnadvise(IN DWORD cookie)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), cookie));
	return E_NOTIMPL;
}
void SOAeSSubscription::onESetFilter(IN HRESULT res, IN DWORD eventType, IN DWORD numCategories, IN DWORD* eventCategoryArray, IN DWORD lowSeverity, IN DWORD highSeverity, IN DWORD numAreas, IN LPWSTR* areaArray, IN DWORD numSources, IN LPWSTR* sourceArray) {}
void SOAeSSubscription::onEGetFilter(IN HRESULT res, IN DWORD* eventType, IN DWORD* numCategories, IN DWORD** eventCategoryArray, IN DWORD* lowSeverity, IN DWORD* highSeverity, IN DWORD* numAreas, IN LPWSTR** areaArray, IN DWORD* numSources, IN LPWSTR** sourceArray)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  eType:0x%X lSev:%d hSev:%d nC:%d nA:%d nS:%d"), *eventType, *lowSeverity, *highSeverity, *numCategories, *numAreas, *numSources));
	}
}
void SOAeSSubscription::onESelectReturnedAttributes(IN HRESULT res, IN DWORD eventCategory, IN DWORD count, IN DWORD* attributeIDs) {}
void SOAeSSubscription::onEGetReturnedAttributes(IN HRESULT res, IN DWORD eventCategory, IN DWORD* count, IN DWORD** attributeIDs)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  category:0x%X cnt:%d"), eventCategory, count));
	}
}
void SOAeSSubscription::onERefresh(IN HRESULT res, IN DWORD connection) {}
void SOAeSSubscription::onECancelRefresh(IN HRESULT res, IN DWORD connection) {}
void SOAeSSubscription::onEGetState(IN HRESULT res, IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE* clientSubscription) {}
void SOAeSSubscription::onESetState(IN HRESULT res, IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE clientSubscription, IN DWORD* revisedBufferTime, IN DWORD* revisedMaxSize) {}
void SOAeSSubscription::onEAdvise(IN HRESULT res, IN LPUNKNOWN unknown, IN DWORD* cookie)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), *cookie));
	}
}
void SOAeSSubscription::onEUnadvise(IN HRESULT res, IN DWORD cookie) {}
#endif // SOFEATURE_COMIF_HOOKS




//-----------------------------------------------------------------------------
// SOAeSRefresh                                                               |
//-----------------------------------------------------------------------------

SOAeSRefresh::SOAeSRefresh(
	IN SOAeSSubscription* sub,  // subscription of refresh
	IN DWORD connection)        // advise connection
	: SOCmnObject(SOCMNOBJECT_TYPE_REFRESH)
{
	setObjectState(created);
	m_eventList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	m_subscription = sub;
	m_subscription.addRef();
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
	m_connection = connection;
	// start refresh
	SOCmnPointer<SOAeSServer> server = (SOAeSServer*)sub->getParentBranch();
	// call name based condition refresh
	server->refreshConditions(this);
	// signal work event to do the object based condition refresh
	server->signalWorkEvent();
}


//-----------------------------------------------------------------------------
// cancel
//
// - cancel the refresh
//
// returns:
//		S_OK - refresh was canceled
//
HRESULT SOAeSRefresh::cancel(void)
{
	SOCmnPointer<SOAeSServer> server = (SOAeSServer*)m_subscription->getParentBranch();
	setObjectState(canceled);
	m_eventList.removeAll();
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	m_subscription.release();
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);

	if (!!server)
	{
		// call name based condition refresh cancel
		server->cancelRefreshConditions(this);
		// signal work event to cancel the refresh
		server->signalWorkEvent();
	}

	return S_OK;
} // cancel




//-----------------------------------------------------------------------------
// SOAeSSubscriptionReturnedAttributes                                        |
//-----------------------------------------------------------------------------

SOAeSSubscriptionReturnedAttributes::SOAeSSubscriptionReturnedAttributes()
{
	m_attributeList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
}

SOAeSSubscriptionReturnedAttributes::~SOAeSSubscriptionReturnedAttributes()
{
	m_category.release();

	if (m_attributeList)
	{
		m_attributeList.destroy();
	}
}




//-----------------------------------------------------------------------------
// SOAeSSubscriptionIFilter                                                   |
//-----------------------------------------------------------------------------

SOAeSSubscriptionIFilter::SOAeSSubscriptionIFilter()
{}

SOAeSSubscriptionIFilter::~SOAeSSubscriptionIFilter()
{}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOAeSSubscriptionIFilter::getIntTraceId(void)
{
	SOCmnObject* obj = getObjectAeSSubscriptionIFilter();

	if (obj != NULL)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return s_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectAeSSubscriptionIFilter
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSSubscriptionIFilter::getObjectAeSSubscriptionIFilter(void)
{
	return NULL;
} // getObjectAeSSubscriptionIFilter


//-----------------------------------------------------------------------------
// setFilter
//
// - set filter parameters
//
// returns:
//		S_FALSE - ignored one or more filter criterias
//
HRESULT SOAeSSubscriptionIFilter::setFilter(
	IN DWORD /* eventType */,                   // event type
	IN SOCmnDWORDList* /* eventCategoryList */, // event category list
	IN DWORD /* lowSeverity */,                 // low severity
	IN DWORD /* highSeverity */,                // high severity
	IN SOCmnStringList* /* areaList */,         // area list
	IN SOCmnStringList*  /* sourceList */)      // source list
{
	// ignore all criterias
	onSetFilter(OPC_ALL_EVENTS, NULL, 1, 1000, NULL, NULL);
	return S_FALSE;
} // setFilter


//-----------------------------------------------------------------------------
// getFilter
//
// - get filter parameters
//
// returns:
//		S_OK - returned the filter criterias
//
HRESULT SOAeSSubscriptionIFilter::getFilter(
	OUT DWORD* eventType,                   // event type
	OUT SOCmnDWORDList* eventCategoryList,  // event category list
	OUT DWORD* lowSeverity,                 // low severity
	OUT DWORD* highSeverity,                // high severity
	OUT SOCmnStringList* areaList,          // area list
	OUT SOCmnStringList* sourceList)        // source list
{
	// return the criterias for no filtering
	if (eventType)
	{
		*eventType = OPC_ALL_EVENTS;
	}

	if (lowSeverity)
	{
		*lowSeverity = 1;
	}

	if (highSeverity)
	{
		*highSeverity = 1000;
	}

	if (eventCategoryList)
	{
		eventCategoryList->create();
	}

	if (areaList)
	{
		areaList->create();
	}

	if (sourceList)
	{
		sourceList->create();
	}

	return S_OK;
} // getFilter


//-----------------------------------------------------------------------------
// onSetFilter
//
// - called before filter is set
// - default implementation: do nothing
//
void SOAeSSubscriptionIFilter::onSetFilter(
	IN DWORD /* eventType */,                   // event type
	IN SOCmnDWORDList* /* eventCategoryList */, // event category list
	IN DWORD /* lowSeverity */,                 // low severity
	IN DWORD /* highSeverity */,                // high severity
	IN SOCmnStringList* /* areaList */,         // area list
	IN SOCmnStringList* /* sourceList */)       // source list
{
} // onSetFilter


//-----------------------------------------------------------------------------
// filterEvent
//
// - apply the filter on an event
//
// returns:
//		TRUE  - the event matches the filter
//      FALSE - the event is filtered out
//
BOOL SOAeSSubscriptionIFilter::filterEvent(
	IN SOAeSEvent* /* event */) // event
{
	return TRUE;
} // filterEvent


//-----------------------------------------------------------------------------
// matchStringFilter
//
// - check if the string filter matches the string
// - the default implenetation uses the OPC Foundation algorithm
//
// returns:
//		TRUE  - filter matches
//		FALSE - filter don't match
//
BOOL SOAeSSubscriptionIFilter::matchStringFilter(
	IN LPCTSTR string,  // string
	IN LPCTSTR filter)  // filter
{
	return SOSrvServer::opcMatchStringFilter(string, filter, TRUE);
} // matchStringFilter




//-----------------------------------------------------------------------------
// SOAeSSubscriptionFilterFull                                                |
//-----------------------------------------------------------------------------

SOAeSSubscriptionFilterFull::SOAeSSubscriptionFilterFull()
{
	m_eventType = OPC_ALL_EVENTS;
	m_lowSeverity = 1;
	m_highSeverity = 1000;
	m_categoryList.create();
	m_areaList.create();
	m_sourceList.create();
}


//-----------------------------------------------------------------------------
// setFilter
//
// - set filter parameters
//
// returns:
//		S_OK - set all filter criterias
//
HRESULT SOAeSSubscriptionFilterFull::setFilter(
	IN DWORD eventType,                     // event type
	IN SOCmnDWORDList* eventCategoryList,   // event category list
	IN DWORD lowSeverity,                   // low severity
	IN DWORD highSeverity,                  // high severity
	IN SOCmnStringList* areaList,           // area list
	IN SOCmnStringList* sourceList)         // source list
{
	// check parameters
	eventType = (eventType & OPC_ALL_EVENTS);
	lowSeverity = (lowSeverity <= 1000) ? lowSeverity : 1000;
	highSeverity = (highSeverity <= 1000) ? highSeverity : 1000;
	onSetFilter(eventType, eventCategoryList, lowSeverity, highSeverity, areaList, sourceList);
	m_sync.lock();
	// set parameters
	m_eventType = eventType;
	m_lowSeverity = lowSeverity;
	m_highSeverity = highSeverity;
	m_categoryList.removeAll();

	if (eventCategoryList)
	{
		m_categoryList.clone(*eventCategoryList);
	}

	m_areaList.removeAll();

	if (areaList)
	{
		m_areaList.clone(*areaList);
	}

	m_sourceList.removeAll();

	if (sourceList)
	{
		m_sourceList.clone(*sourceList);
	}

	m_sync.unlock();
#ifdef SOFEATURE_WATCH
	SOCmnObject* subscr = getObjectAeSSubscriptionIFilter();

	if (subscr)
	{
		if (subscr->getCollectChanges())
		{
			SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
			SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
			SOCmnString buffer;
			BOOL addComma;
			SOCmnListPosition pos;
			SOSrvWatch::attributeChange(subscr, SOWATCH_ATTRIBUTE_FILTERSEVERITY_T, buffer.format(_T("%lu - %lu"), lowSeverity, highSeverity));
			SOSrvWatch::attributeChange(subscr, SOWATCH_ATTRIBUTE_FILTERTYPE_T, conv->eventTypeFilter2string(buffer, eventType));

			if (areaList)
				if (!areaList->isEmpty())
				{
					addComma = FALSE;
					buffer.empty();
					pos = areaList->getStartPosition();

					while (pos)
					{
						if (addComma)
						{
							buffer += _T(",");
						}

						buffer += areaList->getNext(pos);
						addComma = TRUE;
					}

					SOSrvWatch::attributeChange(subscr, SOWATCH_ATTRIBUTE_FILTERAREA_T, buffer);
				}

			if (sourceList)
				if (!sourceList->isEmpty())
				{
					addComma = FALSE;
					buffer.empty();
					pos = sourceList->getStartPosition();

					while (pos)
					{
						if (addComma)
						{
							buffer += _T(",");
						}

						buffer += sourceList->getNext(pos);
						addComma = TRUE;
					}

					SOSrvWatch::attributeChange(subscr, SOWATCH_ATTRIBUTE_FILTERSOURCE_T, buffer);
				}

			if (eventCategoryList)
				if (!eventCategoryList->isEmpty())
				{
					SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
					SOCmnPointer<SOAeSEventSpaceRoot> esRoot = entry->getEventSpaceRoot();
					SOCmnPointer<SOAeSCategory> category;
					addComma = FALSE;
					buffer.empty();
					pos = eventCategoryList->getStartPosition();

					while (pos)
					{
						category = esRoot->findCategory(eventCategoryList->getNext(pos));

						if (category.isNotNull())
						{
							if (addComma)
							{
								buffer += _T(",");
							}

							buffer += category->getDescription();
							addComma = TRUE;
						}
					}

					SOSrvWatch::attributeChange(subscr, SOWATCH_ATTRIBUTE_FILTERCATEGORY_T, buffer);
				}
		}
	}

#endif // SOFEATURE_WATCH
	return S_OK;
} // setFilter


//-----------------------------------------------------------------------------
// getFilter
//
// - get filter parameters
//
// returns:
//		S_OK - returned the filter criterias
//
HRESULT SOAeSSubscriptionFilterFull::getFilter(
	OUT DWORD* eventType,                   // event type
	OUT SOCmnDWORDList* eventCategoryList,  // event category list
	OUT DWORD* lowSeverity,                 // low severity
	OUT DWORD* highSeverity,                // high severity
	OUT SOCmnStringList* areaList,          // area list
	OUT SOCmnStringList* sourceList)        // source list
{
	// return the criterias
	m_sync.lock();

	if (eventType)
	{
		*eventType = m_eventType;
	}

	if (lowSeverity)
	{
		*lowSeverity = m_lowSeverity;
	}

	if (highSeverity)
	{
		*highSeverity = m_highSeverity;
	}

	if (eventCategoryList)
	{
		eventCategoryList->clone(m_categoryList);
	}

	if (areaList)
	{
		areaList->clone(m_areaList);
	}

	if (sourceList)
	{
		sourceList->clone(m_sourceList);
	}

	m_sync.unlock();
	return S_OK;
} // getFilter


//-----------------------------------------------------------------------------
// filterEvent
//
// - apply the filter on an event
//
// returns:
//		TRUE  - the event matches the filter
//      FALSE - the event is filtered out
//
BOOL SOAeSSubscriptionFilterFull::filterEvent(
	IN SOAeSEvent* event)   // event
{
	if (!event)
	{
		return FALSE;
	}

	SOCmnPointer<SOAeSCategory> cat = event->getCategory();

	if (!cat)
	{
		return FALSE;
	}

	m_sync.lock();

	// check event type
	if ((m_eventType & cat->getEventType()) == 0)
	{
		m_sync.unlock();
		_TRACEI(SOCMNTRACE_L_INF, SOCMNTRACE_G_SUS_FILTER, (_T("filtered out %s - event type (E 0x%X F 0x%X)"), (LPCTSTR)event->getObjTraceId(), cat->getEventType(), m_eventType));
		return FALSE;
	}

	// check severity
	DWORD severity = event->getSeverity();

	if ((m_lowSeverity > severity) || (m_highSeverity < severity))
	{
		m_sync.unlock();
		_TRACEI(SOCMNTRACE_L_INF, SOCMNTRACE_G_SUS_FILTER, (_T("filtered out %s - severity (L %lu E %lu H %lu)"), (LPCTSTR)event->getObjTraceId(), m_lowSeverity, severity, m_highSeverity));
		return FALSE;
	}

	// check category
	if (!m_categoryList.isEmpty())
	{
		SOCmnDWORDList categoryList(m_categoryList);

		if (!categoryList.findObject(cat->getNameInt()))
		{
			m_sync.unlock();
			_TRACEI(SOCMNTRACE_L_INF, SOCMNTRACE_G_SUS_FILTER, (_T("filtered out %s - category %lu %s"), (LPCTSTR)event->getObjTraceId(), cat->getNameInt(), (LPCTSTR)cat->getDescription()));
			return FALSE;
		}
	}

	// check area
	if (!m_areaList.isEmpty())
	{
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOAeSAreaSpaceRoot> areaRoot = entry->getAreaSpaceRoot();
		SOCmnStringList areaList(m_areaList);
		SOCmnString srcName = event->getSourceName();
		SOCmnListPosition pos;
		LPTSTR areaName = NULL;
		LPTSTR sourceID = NULL;
		SOCmnString name;
		BOOL found = FALSE;
		areaRoot->reducePath(srcName, areaName, sourceID, FALSE);

		if (areaName)
		{
			pos = areaList.getStartPosition();

			while ((pos) && (!found))
			{
				name = areaList.getNext(pos);

				if (matchStringFilter(areaName, name))
				{
					found = TRUE;
				}
			}

			if (!found)
			{
				m_sync.unlock();
				_TRACEI(SOCMNTRACE_L_INF, SOCMNTRACE_G_SUS_FILTER, (_T("filtered out %s - area"), (LPCTSTR)event->getObjTraceId()));
				return FALSE;
			}
		}
	}

	// check source
	if (!m_sourceList.isEmpty())
	{
		SOCmnString srcName = event->getSourceName();
		SOCmnStringList srcList(m_sourceList);
		SOCmnListPosition pos;
		SOCmnString name;
		BOOL found = FALSE;
		pos = srcList.getStartPosition();

		while ((pos) && (!found))
		{
			name = srcList.getNext(pos);

			if (matchStringFilter(srcName, name))
			{
				found = TRUE;
			}
		}

		if (!found)
		{
			m_sync.unlock();
			_TRACEI(SOCMNTRACE_L_INF, SOCMNTRACE_G_SUS_FILTER, (_T("filtered out %s - source"), (LPCTSTR)event->getObjTraceId()));
			return FALSE;
		}
	}

	m_sync.unlock();
	return TRUE;
} // filterEvent




//-----------------------------------------------------------------------------
// SOAeSSubscriptionDefault                                                   |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getObjectAeSSubscriptionIFilter
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSSubscriptionDefault::getObjectAeSSubscriptionIFilter(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSSubscriptionIFilter

#endif // SOAES
