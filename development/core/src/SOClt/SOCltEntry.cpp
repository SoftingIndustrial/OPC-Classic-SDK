#include "stdafx.h"

#ifdef SOCLT

#include "SOCltEntry.h"
#include "SOCltThread.h"

//-----------------------------------------------------------------------------
// Demo version check
//-----------------------------------------------------------------------------

#ifndef SOFLAG_CLIENT_AND_SERVER
#ifdef SOFLAG_DEMO_VERSION
DWORD g_demoStamp = GetTickCount();
#endif
#endif

SOCltEntry* g_cltEntry = NULL;

void setSOCltEntry(
	IN SOCltEntry* entry)
{
	g_cltEntry = entry;
}

SOCltEntry* getSOCltEntry(void)
{
	if (g_cltEntry)
	{
		g_cltEntry->addRef();
	}

	return g_cltEntry;
}



//-----------------------------------------------------------------------------
// SOCltCreator                                                               |
//-----------------------------------------------------------------------------

SOCltCreator::SOCltCreator(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_CALLBACK)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("CREATOR"));
	setObjTraceId(id);
}

SOCltCreator::~SOCltCreator(void)
{
}

SOCltTaskConnectionMonitor* SOCltCreator::createTaskConnectionMonitor(IN SOCltElement* pServer, IN DWORD cycle)
{
	SOCltTaskConnectionMonitor* p = NULL;
	SOCMN_ALLOCATE_OBJECT_C(p, SOCltTaskConnectionMonitor(pServer, cycle), (SOCltTaskConnectionMonitor*));
	return p;
}

SOCltTaskStateTransition* SOCltCreator::createTaskStateTransition(IN SOCltElement* pElement, IN BYTE targetState, IN BOOL deep)
{
	SOCltTaskStateTransition* task = NULL;
	SOCMN_ALLOCATE_OBJECT_C(task, SOCltTaskStateTransition(pElement, targetState, deep), (SOCltTaskStateTransition*));
	return task;
}

//-----------------------------------------------------------------------------
// SOCltEntry                                                                 |
//-----------------------------------------------------------------------------

#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
//
// Callback dispatching

#ifndef SOOS_WINDOWS_CE_HEADLESS
DWORD SOCltEntry::s_messageCallbackNotify = ::RegisterWindowMessage(_T("DISPATCHCALLBACKS"));
#else
DWORD SOCltEntry::s_messageCallbackNotify = 0;
#endif

static HWND g_hwndCallbackNotify = NULL;
static DWORD g_mainThreadID = 0;
static SOCmnSync g_callbackQueueSync;
static SOCmnList<SOCltCallback> g_callbackQueue;

#endif // SOOS_WINDOWS


SOCltEntry::SOCltEntry(IN DWORD objType)
	:  SOCltElement(SOCMNOBJECT_TYPE_ENTRY | SOCMNOBJECT_TYPE_ROOT | objType)
{
	m_terminateLock = TRUE;
#ifdef SOFEATURE_DCOM
	m_coInit = 0;
#endif
	m_pWorkThread = NULL;
	m_createWorkThread = TRUE;
}


SOCltEntry::~SOCltEntry()
{
	if (m_pWorkThread)
	{
		delete m_pWorkThread;
	}
}

SOCltWorkerThread* SOCltEntry::getWorkerThread(void)
{
	if (m_createWorkThread)
	{
		if (m_pWorkThread == NULL)
		{
			SOCMN_ALLOCATE_OBJECT(m_pWorkThread, SOCltWorkerThread);
		}

		return m_pWorkThread;
	}
	else
	{
		return NULL;
	}
}

//-----------------------------------------------------------------------------
// initialize
//
// - initializes session
//
// returns:
//		TRUE  - initialized
//		FALSE - error during initializing
//
BOOL SOCltEntry::initialize(
	IN DWORD coInit)    // COM init mode
{
	HRESULT res = S_OK;
#ifdef SOFEATURE_DCOM
	res = ::CoInitializeEx(NULL, coInit);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoInitializeEx [0x%X] "), res));

	if (SUCCEEDED(res))
	{
		m_coInit = coInit;
	}

#endif // SOFEATURE_DCOM
#ifdef SOOS_WINDOWS
	g_callbackQueueSync.lock();

	if (!g_callbackQueue.isInit())
	{
		g_callbackQueue.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_DATA);
	}

	g_callbackQueueSync.unlock();
#endif // SOOS_WINDOWS
	setLastError(res, SOCLT_ERR_COM_INIT);
	return SUCCEEDED(res);
} // initialize

//-----------------------------------------------------------------------------
// initializeSecurity
//
// - initializes security
//
// returns:
//		TRUE  - initialized
//		FALSE - error during initializing
//
BOOL SOCltEntry::initializeSecurity(IN DWORD authLevel,
									IN DWORD impLevel,
									IN DWORD capabilities)
{
	static bool s_dcomInitialized = false;

#ifdef SOFEATURE_DCOM
	if (!s_dcomInitialized)
	{
		s_dcomInitialized = true;
		SOCmnString buffer;

		if ((authLevel != RPC_C_AUTHN_LEVEL_DEFAULT) && (impLevel != RPC_C_IMP_LEVEL_DEFAULT))
		{
			HRESULT res = CoInitializeSecurity(0, -1, 0, 0, authLevel,
				impLevel, 0, capabilities, 0);
			_TRACEO(FAILED(res) ? SOCMNTRACE_L_ERR : SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, this, (_T("DCOM client CoInitializeSecurity (a:%d, i:%d) result: 0x%X"), authLevel, impLevel, res));
			buffer.format(_T("DCOM client initialized: a=%d, i=%d, res=0x%08X."), authLevel, impLevel, res);
			getSOCmnTrace()->addDcomInitStatus((LPCTSTR)buffer);
			return SUCCEEDED(res);
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, this, (_T("DCOM client CoInitializeSecurity skipped")));
			buffer.format(_T("DCOM client initialization skipped: a=%d, i=%d"), authLevel, impLevel);
			getSOCmnTrace()->addDcomInitStatus((LPCTSTR)buffer);
		}
	}
#endif

	return TRUE;
} // initializeSecurity

//-----------------------------------------------------------------------------
// terminate
//
// - reset terminate lock
//
BOOL SOCltEntry::terminate(void)
{
	_TRACE(TL_INF, SOCMNTRACE_G_ENTRY, (_T("terminate")));
	m_terminateLock = FALSE;
	disconnect();
#ifdef SOOS_WINDOWS
	g_callbackQueueSync.lock();
	g_callbackQueue.destroy();
	g_callbackQueueSync.unlock();
#endif // SOOS_WINDOWS
	removeAllAdditionalElements(TRUE);
	removeAllLeafs(TRUE);
	removeAllBranches(TRUE);
#ifdef SOFEATURE_DCOM

	if (m_coInit != 0)
	{
		::CoUninitialize();
	}

#endif //SOFEATURE_DCOM

	if (m_pWorkThread)
	{
		m_pWorkThread->shutdown();
	}

	_TRACE(TL_DEB, SOCMNTRACE_G_ENTRY, (_T("<terminate")));
	return TRUE;
} // terminate

HRESULT SOCltEntry::doStateTransition(IN BOOL deep, BYTE stateDeep)
{
	HRESULT res = S_OK;
	BYTE targetState = getTargetObjectState();

	if (disconnected == targetState)
	{
		doRecurseChildren(getBranchList(), targetState);
		doRecurseChildren(getLeafList(), targetState);
		doRecurseChildren(getAdditionalList(), targetState);
	}

	setObjectState(getTargetObjectState());

	if (deep && disconnected != targetState)
	{
		// Recurse children
		if (S_OK != doRecurseChildren(getBranchList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getLeafList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getAdditionalList(), stateDeep))
		{
			res = S_FALSE;
		}
	}

	return res;
}

HRESULT SOCltEntry::doRecurseChildren(SOCmnList<SOCmnElement>* plist, IN BYTE state)
{
	BOOL ok = TRUE;

	if (NULL == plist)
	{
		return S_OK;
	}

	SOCmnList<SOCmnElement> list(plist);
	SOCltElement* element = NULL;
	DWORD max_handles = list.getCount();

	if (max_handles == 0)
	{
		return S_OK;
	}

	//SOCmnEvents handles;
	SOCmnEvent* handles = new SOCmnEvent[max_handles];
	DWORD cnt = 0;
	SOCmnListPosition pos = list.getStartPosition();

	while (pos)
	{
		element = (SOCltElement*)list.getNext(pos);
		SOCmnEventPointer wait;
		SOCltWorkerThread* thread = element->getWorkerThread();
		// Each server must have its own working thread.
		// It does make sense to accomplish the tasks
		// on them in paralell, no matter how this
		// task is executed, either synch or asynch.
		wait.set(SOCmnEventPointer(&handles[cnt]));

		if (thread && !thread->isOwnThread())
		{
			handles[cnt++].create();
		}

		// Calling start(), connect(), disconnect()
		// instead of doStateTransition() will create
		// a separate task
		switch (state)
		{
		case SOCltElement::started:
		{
			ok &= SUCCEEDED(element->start(TRUE, SOCLT_EXEC_ASYNC, wait));
		}
		break;

		case SOCltElement::connected:
		{
			ok &= SUCCEEDED(element->connect(TRUE, SOCLT_EXEC_ASYNC, wait));
		}
		break;

		case SOCltElement::disconnected:
		{
			ok &= SUCCEEDED(element->disconnect(SOCLT_EXEC_ASYNC, wait));
		}
		break;
		}// end switch
	}

	list.autoUnlock();

	// wait for completion
	if (cnt > 0)
	{
		DWORD i;

		for (i = 0 ; i < cnt ; i++)
		{
			handles[i].waitFor();
		}

		delete [] handles;

		if (ok)
		{
			pos = list.getStartPosition();

			while (pos)
			{
				element = (SOCltElement*)list.getNext(pos);

				if (element->getObjectState() != state)
				{
					ok = FALSE;
					break;
				}
			}
		}
	}

	return ok ? S_OK : S_FALSE;
}


//-----------------------------------------------------------------------------
// release
//
// - decrement reference counter
// - free object if no reference left and terminate lock not set
//
LONG SOCltEntry::release(void)
{
	BOOL terminate = m_terminateLock;
	LONG refCount = interlockedDecrement(&m_refCount);
	_TRACE(TL_DEB, SOCMNTRACE_G_OBJ_REF, (_T("release %li locked:%s"), refCount, BOOL2STR(terminate)))

	if ((refCount <= 0) && (!terminate))
	{
		delete this;    // last reference and terminate flag set -> free object
		return 0;
	}

	return refCount;
} // release

#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
//
// setNotifyWindow
void SOCltEntry::setNotifyWindow(IN HWND hwnd)
{
	_ASSERTION(::IsWindow(hwnd), "Invalid window handle");
	g_hwndCallbackNotify = hwnd;
	g_mainThreadID = ::GetCurrentThreadId();
}

//-----------------------------------------------------------------------------
//
// dispatchCallbacks
void SOCltEntry::dispatchCallbacks()
{
	while (1)
	{
		SOCltCallback cb;
		g_callbackQueueSync.lock();
		SOCltCallback* pcb = g_callbackQueue.getHead();

		if (pcb)
		{
			cb = *pcb; // save the data, since removeHead() will call cdor
			cb.e->addRef();
		}

		g_callbackQueue.removeHead();
		g_callbackQueueSync.unlock();

		// DON'T hold the queue locked while
		// executing the callback!

		if (pcb)
		{
			cb.execute();
		}
		else
		{
			break;
		}
	}
}

#endif // SOOS_WINDOWS


BYTE SOCltEntry::getProtocolByURL(IN LPCTSTR url)
{
	BYTE proto = SOCLT_PROTOCOL_UNKNOWN;
	SOCmnString i1(url);
	SOCmnString protocol;
	int ppos;
	ppos = i1.find(_T("://"));

	if (ppos != -1)
	{
		protocol = (LPCTSTR)i1.left(ppos);       // the left of "://"
	}

	if (protocol == _T("opcda"))
	{
		proto = SOCLT_PROTOCOL_DCOM;
	}
	else if (protocol == _T("http"))
	{
		proto = SOCLT_PROTOCOL_SOAP;
	}
	else if (protocol == _T("opcae"))
	{
		proto = SOCLT_PROTOCOL_DCOM;
	}

#ifdef SOFEATURE_TUNNEL
	else if (protocol == _T("tpda"))
	{
		proto = SOCLT_PROTOCOL_TUNNEL;
	}
	else if (protocol == _T("tpae"))
	{
		proto = SOCLT_PROTOCOL_TUNNEL;
	}

#endif
	return proto;
}

BYTE SOCltEntry::getSpecificationByURL(IN LPCTSTR url)
{
	BYTE spec = SOCLT_SPECIFICATION_UNKNOWN;
	SOCmnString i1(url);
	SOCmnString protocol;
	int ppos;
	ppos = i1.find(_T("://"));

	if (ppos != -1)
	{
		protocol = (LPCTSTR)i1.left(ppos);       // the left of "://"
	}

	if (protocol == _T("opcda"))
	{
		spec = SOCLT_SPECIFICATION_DA;
	}
	else if (protocol == _T("http"))
	{
		spec = SOCLT_SPECIFICATION_DA;
	}
	else if (protocol == _T("opcae"))
	{
		spec = SOCLT_SPECIFICATION_AE;
	}

#ifdef SOFEATURE_TUNNEL
	else if (protocol == _T("tpda"))
	{
		spec = SOCLT_SPECIFICATION_DA;
	}
	else if (protocol == _T("tpae"))
	{
		spec = SOCLT_SPECIFICATION_AE;
	}

#endif
	return spec;
}


#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
//
// SOCltInvokeCallback
void SOCltInvokeCallback(const SOCltCallback& clb)
{
	// Execute immideately, if we get called
	// in the context of the main thread or no callback
	// thread or window is specified
	if (g_mainThreadID == ::GetCurrentThreadId() || NULL == g_hwndCallbackNotify)
	{
		clb.execute();
		return;
	}

	g_callbackQueueSync.lock();

	// No callbacks after the entry has been destroyed.

	if (g_callbackQueue.isInit())
	{
		SOCltCallback* pdata = NULL;
		SOCMN_ALLOCATE_OBJECT(pdata, SOCltCallback);

		if (pdata)
		{
			*pdata = clb;
			pdata->e->addRef();
			g_callbackQueue.add(pdata);
		}

		::PostMessage(g_hwndCallbackNotify, SOCltEntry::s_messageCallbackNotify, (WPARAM)0, (LPARAM)0);
	}

	g_callbackQueueSync.unlock();
}

#endif // SOOS_WINDOWS

#endif // SOCLT
