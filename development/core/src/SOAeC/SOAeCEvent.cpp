#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCEvent.h"
#include "SOAeCServer.h"
#include "SOAeCSubscription.h"

//-----------------------------------------------------------------------------
// SOAeCEvent                                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// SOAeCEvent
SOAeCEvent::SOAeCEvent()
	: SOCltElement(SOCMNOBJECT_TYPE_EVENT)
{
}

SOAeCEvent::SOAeCEvent(IN SOAeCSubscription* subs, IN ONEVENTSTRUCT* pData)
	: SOCltElement(SOCMNOBJECT_TYPE_EVENT)
{
	_ASSERT_PTR(subs);
	_ASSERTION(subs->is(SOCMNOBJECT_TYPE_SUBSCRIPTION), "Subscription object expected");
	m_ackRes = S_OK;
	m_changeMask = 0;
	m_newState = 0;
	m_quality = OPC_QUALITY_GOOD;
	m_bAckRequired = FALSE;
	m_ftActiveTime.dwHighDateTime = 0;
	m_ftActiveTime.dwLowDateTime = 0;
	m_dwCookie = 0;
	m_listEventAttrs.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_VARIANT);
	subs->addRef();
	m_subs = subs;

	if (pData)
	{
		copyData(*pData);
	}
}

//-----------------------------------------------------------------------------
//
// copyData
void SOAeCEvent::copyData(IN const ONEVENTSTRUCT& src)
{
	USES_CONVERSION;
	// Data valid for all events
	m_source = W2T(src.szSource);
	m_ftTime = src.ftTime;
	m_message = W2T(src.szMessage);
	m_eventType = src.dwEventType;
	m_eventCategory = src.dwEventCategory;
	m_severity = src.dwSeverity;
	m_dwCookie = src.dwCookie;
	m_wReserved = src.wReserved;
	m_dwNumEventAttrs = src.dwNumEventAttrs;
	SOCmnList<SOCmnVariant> list(m_listEventAttrs);
	list.allocateBlock(src.dwNumEventAttrs);

	for (DWORD i = 0 ; i < src.dwNumEventAttrs ; i++)
	{
		list.add((SOCmnVariant*)(src.pEventAttributes + i));
	}

	// Following members are valid for conditional
	// events only

	if (src.dwEventType == OPC_CONDITION_EVENT)
	{
		m_changeMask = src.wChangeMask;
		m_newState = src.wNewState;
		m_conditionName = W2T(src.szConditionName);
		m_subconditionName = W2T(src.szSubconditionName);
		m_quality = src.wQuality;
		m_bAckRequired = src.bAckRequired;
		m_ftActiveTime = src.ftActiveTime;
	}

	m_actorID =  W2T(src.szActorID);
}


//-----------------------------------------------------------------------------
//
// ~SOAeCEvent

SOAeCEvent::~SOAeCEvent()
{
	m_listEventAttrs.destroy();
}


//-----------------------------------------------------------------------------
//
// getWorkerThread
//
// Overrdies getWorkerThread in order to enable async
// operations (acknowledgment)

SOCltWorkerThread* SOAeCEvent::getWorkerThread(void)
{
	return m_subs->getWorkerThread();
}

//-----------------------------------------------------------------------------
//
// onEventReceived
void SOAeCEvent::report()
{
	INVOKE_CALLBACK(SOAeCEvent, onEventReceived);
}

//-----------------------------------------------------------------------------
//
// onEventReceived

void SOAeCEvent::onEventReceived(void)
{
} // onEventReceived

//-----------------------------------------------------------------------------
//
// onEventAcknowledged

void SOAeCEvent::onEventAcknowledged(void)
{
} // onEventAcknowledged


//-----------------------------------------------------------------------------
//
// acknowledge

BOOL SOAeCEvent::ackCondition(
	IN OPTIONAL LPCTSTR acknowledgerID,   /* = NULL */
	IN OPTIONAL LPCTSTR comment,          /* = NULL */
	IN OPTIONAL BOOL waitopt,             /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle  /* = NULL */)
{
	BOOL ok = FALSE;
	_ASSERTION(isConditional(), "Only conditional events can be acknowledged");

	if (m_subs.isNotNull())
	{
		SOCmnPointer<SOAeCServer> server;
		server = (SOAeCServer*)m_subs->getParentBranch();

		if (server.isNotNull())
		{
			SOCmnList<SOAeCEvent> events;
			events.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);
			events.add(this);
			ok = server->ackConditions(&events, acknowledgerID, comment, waitopt, handle);
			/*
			            SOCmnPointer<SOAeCTaskAckEvent> task;
			            SOCMN_ALLOCATE_OBJECT_C( task, SOAeCTaskAckEvent(server, this), (SOAeCTaskAckEvent*));
			*/
//			task->m_ackID = acknowledgerID;
//			task->m_comment = comment;
//			ok = executeTask(task, waitopt, handle);
		}
	}

	return ok;
}




//-----------------------------------------------------------------------------
// SOAeCTaskAckEvent                                                          |
//-----------------------------------------------------------------------------

SOAeCTaskAckEvent::SOAeCTaskAckEvent(SOAeCServer* pServer, IN SOAeCEvent* pEvent)
	:   SOCltTask((SOCltElement*)pServer)
{
	m_eventAE = pEvent;
	m_eventAE.addRef();
}

SOAeCTaskAckEvent::SOAeCTaskAckEvent(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events)
	:   SOCltTask((SOCltElement*)pServer)
{
	_ASSERT_PTR(events);
	m_listEvents.clone(*events);
}

SOAeCTaskAckEvent::~SOAeCTaskAckEvent()
{
	m_eventAE.release();

	if (m_listEvents.isInit())
	{
		m_listEvents.destroy();
	}
}

//-----------------------------------------------------------------------------
//
// execute

void SOAeCTaskAckEvent::execute(void)
{
	if (m_eventAE.isNotNull())
	{
		executeSingle();
	}
	else
	{
		executeMultiple();
	}
}

//-----------------------------------------------------------------------------
//
// executeSingle

void SOAeCTaskAckEvent::executeSingle(void)
{
	HRESULT res = S_OK;
	SOAeCServer* server = (SOAeCServer*)(SOCltElement*)m_element;
	CComPtr<IOPCEventServer> pSrv;
	res = server->queryInterface(IID_IOPCEventServer, (void**)&pSrv);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		LPWSTR wszAckID = m_ackID.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_ackID);
		LPWSTR wszComment = m_comment.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_comment);
		LPWSTR wszSrc = T2W((LPTSTR)(LPCTSTR)m_eventAE->m_source);
		LPWSTR wszCondName = T2W((LPTSTR)(LPCTSTR)m_eventAE->m_conditionName);
		SOCltAutoMemory<HRESULT> pErr;
		server->beforeInterfaceCall();
		res = pSrv->AckCondition(1, wszAckID, wszComment, &wszSrc , &wszCondName,
								 &m_eventAE->m_ftActiveTime, &m_eventAE->m_dwCookie, &pErr);
		server->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCEventServer::AckCondition [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			m_eventAE->m_ackRes = pErr[0];
		}

		INVOKE_CALLBACK_ON_PTR((SOAeCEvent*)m_eventAE, SOAeCEvent, onEventAcknowledged)
	}

	setResult(res);
	return;
}

//-----------------------------------------------------------------------------
//
// executeMultiple

void SOAeCTaskAckEvent::executeMultiple(void)
{
	HRESULT res = S_OK;
	// Use the first in list to find out the server
	SOAeCServer* srv = (SOAeCServer*)(SOCltElement*)m_element;
	CComPtr<IOPCEventServer> pSrv;
	res = srv->queryInterface(IID_IOPCEventServer, (void**)&pSrv);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		LPWSTR wszAckID = m_ackID.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_ackID);
		LPWSTR wszComment = m_comment.isEmpty() ? L"" : T2W((LPTSTR)(LPCTSTR)m_comment);
		DWORD dwCount = m_listEvents.getCount();
		LPWSTR* wszSources = (LPWSTR*)_alloca(dwCount * sizeof(LPWSTR*));
		LPWSTR* wszConditions = (LPWSTR*)_alloca(dwCount * sizeof(LPWSTR*));
		FILETIME* ftTimes = (FILETIME*)_alloca(dwCount * sizeof(FILETIME));
		DWORD* dwCookies = (DWORD*)_alloca(dwCount * sizeof(DWORD));
		SOCmnListPosition pos = m_listEvents.getStartPosition();
		int i;

		for (i = 0; pos; i++)
		{
			SOAeCEvent* event = m_listEvents.getNext(pos);
			ftTimes[i] = event->m_ftActiveTime;
			dwCookies[i] = event->m_dwCookie;
			wszSources[i] = T2W((LPTSTR)(LPCTSTR)event->m_source);
			wszConditions[i] = T2W((LPTSTR)(LPCTSTR)event->m_conditionName);
		}

		SOCltAutoMemory<HRESULT> pErr;
		srv->beforeInterfaceCall();
		res = pSrv->AckCondition(dwCount, wszAckID, wszComment, wszSources , wszConditions,
								 ftTimes, dwCookies, &pErr);
		srv->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCEventServer::AckCondition [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			pos = m_listEvents.getStartPosition();
			i = 0;

			while (pos)
			{
				SOAeCEvent* event = m_listEvents.getNext(pos);
				event->m_ackRes = pErr[i++];
				INVOKE_CALLBACK_ON_PTR(event, SOAeCEvent, onEventAcknowledged)
			}
		}
	}

	setResult(res);
	return;
}

#endif // SOAEC

