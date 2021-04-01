#include "stdafx.h"
#include "SOCmn.h"
#include "SOCmnSync.h"


//-----------------------------------------------------------------------------
// SOCmnSync                                                                  |
//-----------------------------------------------------------------------------

SOCmnSync::SOCmnSync()
{
#ifdef SOOS_WINDOWS
	m_lockCount = -1;
	m_ownThreadId = 0;
	m_recurseCount = 0;
	m_waitEvent = ::popEvent();
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	pthread_mutexattr_t attr;
#ifdef SODEVENV_CYGWIN
	memset(&attr, 0, sizeof(attr));
	memset(&m_mutex, 0, sizeof(m_mutex));
#endif
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
}

SOCmnSync::~SOCmnSync()
{
#ifdef SOOS_WINDOWS
	::pushEvent(m_waitEvent);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	pthread_mutex_destroy(&m_mutex);
#endif
}


//-----------------------------------------------------------------------------
// lock
//
// - gain access to the synchronisation object
//
// returns:
//		TRUE  - got access to the object
//
BOOL SOCmnSync::lock(IN OPTIONAL DWORD timeout)
{
#ifdef SOOS_LINUX
	BOOL res;

	if (timeout == INFINITE)
	{
		pthread_mutex_lock(&m_mutex);
		res = TRUE;
	}
	else if (timeout == 0)
	{
		res = !pthread_mutex_trylock(&m_mutex);
	}
	else
	{
#ifndef SODEVENV_CYGWIN
		struct timespec ts;
		struct timeval  tv;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + timeout / 1000;
		ts.tv_nsec = 1000 * tv.tv_usec;
		ts.tv_nsec += 1000000 * (timeout % 1000);

		if (ts.tv_nsec  >  999999999)
		{
			ts.tv_nsec -= 1000000000;
			ts.tv_sec  ++;
		}

		res = !pthread_mutex_timedlock(&m_mutex, &ts);
#else
		res = !pthread_mutex_lock(&m_mutex);
#endif
	}

	return res;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	DWORD curThreadId = GetCurrentThreadId();

	if (InterlockedIncrement(&m_lockCount) == 0)
	{
		// thread now owns the SOCmnSync
		m_ownThreadId = curThreadId;
		m_recurseCount = 1;
	}
	else
	{
		if (m_ownThreadId == curThreadId)
		{
			// still owning SOCmnSync
			m_recurseCount++;
		}
		else
		{
			// not owning SOCmnSync
			DWORD res;
			res = ::WaitForSingleObject(m_waitEvent, timeout);

			if (res == WAIT_TIMEOUT)
			{
				// stop waiting for the lock
				InterlockedDecrement(&m_lockCount);
				return FALSE;
			}

			// thread now owns the SOCmnSync
			m_ownThreadId = curThreadId;
			m_recurseCount = 1;
		}
	}

#endif
	return TRUE;
} // lock


//-----------------------------------------------------------------------------
// unlock
//
// - release access to the synchronisation object
//
// returns:
//		TRUE  - released access to the object
//		FALSE - not owned the object
//
BOOL SOCmnSync::unlock(void)
{
#ifdef SOOS_LINUX
	return !pthread_mutex_unlock(&m_mutex);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	DWORD curThreadId = GetCurrentThreadId();
	BOOL unlocked = TRUE;

	if (m_ownThreadId == curThreadId)
	{
		m_recurseCount--;

		if (m_recurseCount > 0)
		{
			// still own the SOCmnSync
			InterlockedDecrement(&m_lockCount);
		}
		else
		{
			// last unlock
			m_ownThreadId = 0;

			if (InterlockedDecrement(&m_lockCount) >= 0)
			{
				// other thread is waiting
				::SetEvent(m_waitEvent);    // signal the event
			}
		}
	}
	else
	{
		unlocked = FALSE;    // object not owned by the thread
	}

	return unlocked;
#endif
} // unlock



//-----------------------------------------------------------------------------
// SOCmnSyncCounter                                                                 |
//-----------------------------------------------------------------------------
SOCmnSyncCounter::SOCmnSyncCounter(SOCmnSync* aSyncObject, SOCmnSync* aCounterLock)
{
	m_syncObject = aSyncObject;
	m_key = 0;
	m_counterLock   = aCounterLock;
}

SOCmnSyncCounter::~SOCmnSyncCounter(void)
{
	delete m_syncObject;
	m_syncObject = NULL;
	delete m_counterLock;
	m_counterLock = NULL;
}



//-----------------------------------------------------------------------------
// SOCmnMutex                                                                 |
//-----------------------------------------------------------------------------

SOCmnMutex::SOCmnMutex()
{
#ifdef SOOS_WINDOWS
	m_mutex = CreateMutex(NULL, FALSE, NULL);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	pthread_mutexattr_t attr;
#ifdef SODEVENV_CYGWIN
	memset(&attr, 0, sizeof(attr));
	memset(&m_mutex, 0, sizeof(m_mutex));
#endif
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
}

SOCmnMutex::~SOCmnMutex()
{
#ifdef SOOS_WINDOWS

	if (m_mutex != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_mutex);
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	pthread_mutex_destroy(&m_mutex);
#endif
}


//-----------------------------------------------------------------------------
// lock
//
// - gain access to the synchronisation object
//
// returns:
//		TRUE  - got access to the object
//		FALSE - no access to the object
//
BOOL SOCmnMutex::lock(
	IN OPTIONAL DWORD timeout)  // maximum time to wait for mutex
{
#ifdef SOOS_WINDOWS
	DWORD res;
	res = ::WaitForSingleObject(m_mutex, timeout);
	return (res == WAIT_OBJECT_0);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	BOOL res;

	if (timeout == INFINITE)
	{
		pthread_mutex_lock(&m_mutex);
		res = TRUE;
	}
	else if (timeout == 0)
	{
		res = !pthread_mutex_trylock(&m_mutex);
	}
	else
	{
#ifndef SODEVENV_CYGWIN
		struct timespec ts;
		struct timeval  tv;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + timeout / 1000;
		ts.tv_nsec = 1000 * tv.tv_usec;
		ts.tv_nsec += 1000000 * (timeout % 1000);

		if (ts.tv_nsec  >  999999999)
		{
			ts.tv_nsec -= 1000000000;
			ts.tv_sec  ++;
		}

		res = !pthread_mutex_timedlock(&m_mutex, &ts);
#else
		res = !pthread_mutex_lock(&m_mutex);
#endif
	}

	return res;
#endif
} // lock


//-----------------------------------------------------------------------------
// unlock
//
// - release access to the synchronisation object
//
// returns:
//		TRUE  - released access to the object
//		FALSE - not owned the object
//
BOOL SOCmnMutex::unlock(void)
{
#ifdef SOOS_WINDOWS
	return ReleaseMutex(m_mutex);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return !pthread_mutex_unlock(&m_mutex);
#endif
} // unlock




//-----------------------------------------------------------------------------
// SOCmnObjectLock                                                            |
//-----------------------------------------------------------------------------

SOCmnObjectLock* g_objectLock = NULL;

void createObjectLock(void)
{
	if (!g_objectLock)
		SOCMN_ALLOCATE_OBJECT(g_objectLock, SOCmnObjectLock())
	}

void destroyObjectLock(void)
{
	if (g_objectLock)
	{
		SOCMN_FREE_OBJECT(g_objectLock)
	}
}

SOCmnObjectLock::SOCmnObjectLock(void)
{
	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 499;
	m_syncCounterList.create(&cfg);
	SOCmnSync* pSync;
	SOCmnSync* pCounterLock;
	DWORD j;

	for (j = 0; j < SOCMNOBJECTLOCK_ARRAY_MASK + 1; j++)
	{
		SOCMN_ALLOCATE_OBJECT(pSync, SOCmnSync());
		SOCMN_ALLOCATE_OBJECT(pCounterLock, SOCmnSync());
		SOCMN_ALLOCATE_OBJECT_C(m_syncCounterArray[j], SOCmnSyncCounter(pSync, pCounterLock), (SOCmnSyncCounter*));
		m_syncCounterArray[j]->setKey(0xabcd7531);
	}

	m_countSyncCreated = 0;
	m_countSyncDestroyed = 0;
	m_countSyncArrayUsed = 0;
}

SOCmnObjectLock::~SOCmnObjectLock(void)
{
	checkSyncs();
	DWORD j;
	size_t key;

	for (j = 0; j < SOCMNOBJECTLOCK_ARRAY_MASK + 1; j++)
	{
		if (m_syncCounterArray[j]->release() != 0)
		{
			key = m_syncCounterArray[j]->getKey();
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnObjectLock"), (_T("object in array 0x%lX "), key));

			while (m_syncCounterArray[j]->release() > 1)
				;
		}
	}

	if (!m_syncCounterList.isEmpty())
	{
		_ASSERTION(FALSE, "delete SOCmnObjectLock with syncs in the list");
		SOCmnListPosition pos = m_syncCounterList.getStartPosition();

		while (pos)
		{
			SOCmnSyncCounter* syncCounter = m_syncCounterList.getNext(pos, &key);

			if (syncCounter != NULL)
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnObjectLock"), (_T("object in list 0x%lX "), key));

				while (syncCounter->release() > 1)
					;

				syncCounter = NULL;
			}
		}
	}

	m_syncCounterList.destroy();
}

SOCmnSync* SOCmnObjectLock::getObjectLock(IN SOCmnObject* pObj)
{
	if (g_objectLock)
	{
		return g_objectLock->doGetObjectLock(pObj, NULL);
	}

	return NULL;
}

SOCmnSync* SOCmnObjectLock::getObjectLockV(IN void* pVoid)
{
	if (g_objectLock)
	{
		return g_objectLock->doGetObjectLock(NULL, pVoid);
	}

	return NULL;
}

SOCmnSync* SOCmnObjectLock::doGetObjectLock(IN SOCmnObject* pObj, IN void* pVoid)
{
	SOCmnList<SOCmnSyncCounter> syncCounterList(m_syncCounterList);
	void* pV = (pObj != NULL) ? (void*)pObj->getOpcHandle() : pVoid;
	// use array
	DWORD idx;
	idx = ((DWORD)(void*)pV) >> 4;
	idx &= SOCMNOBJECTLOCK_ARRAY_MASK;
	SOCmnSync* pArrayLock = m_syncCounterArray[idx]->getCounterLock();
	pArrayLock->lock();

	if (m_syncCounterArray[idx]->getRefCount() == 1)
	{
		// index not used
		m_syncCounterArray[idx]->setKey((size_t)pV);
		m_syncCounterArray[idx]->addRef();
		interlockedIncrement(&m_countSyncArrayUsed);
		pArrayLock->unlock();
		return m_syncCounterArray[idx]->getSyncObject();
	}
	else
	{
		// index used
		// check if array is used for the pV
		if (m_syncCounterArray[idx]->getKey() == (size_t)pV)
		{
			m_syncCounterArray[idx]->addRef();
			pArrayLock->unlock();
			return m_syncCounterArray[idx]->getSyncObject();
		}
	}

	// add reference for using list to the array
	// the array will only be used if all list entries for this index are freed
	m_syncCounterArray[idx]->addRef();
	pArrayLock->unlock();
	// use list
	syncCounterList.autoLock();
	SOCmnPointer<SOCmnSyncCounter> syncCounter = syncCounterList.findKey((size_t)pV);

	if (!syncCounter)
	{
		SOCmnSync* pSync;
		SOCmnSync* pCounterLock;
		SOCMN_ALLOCATE_OBJECT(pSync, SOCmnSync());

		if (pSync == NULL)
		{
			return NULL;
		}

		SOCMN_ALLOCATE_OBJECT(pCounterLock, SOCmnSync());

		if (pCounterLock == NULL)
		{
			return NULL;
		}

		SOCMN_ALLOCATE_OBJECT_C(syncCounter, SOCmnSyncCounter(pSync, pCounterLock), (SOCmnSyncCounter*));
		syncCounter->setKey((size_t)pV);
		syncCounterList.add(syncCounter, (size_t)pV);
		syncCounter.addRef();
		interlockedIncrement(&m_countSyncCreated);
		_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG)
		{
			if (pObj)
			{
				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG, _T("SOCmnObjectLock"), (_T("created 0x%lX | %s | (%lu)"), pObj->getOpcHandle(), pObj->getObjTraceId(), m_countSyncCreated));
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG, _T("SOCmnObjectLock"), (_T("created 0x%lX | pV | (%lu)"), (size_t)pV, m_countSyncCreated));
			}
		}
		return pSync;
	}

	syncCounter.addRef();
	return syncCounter->getSyncObject();
}
//#endif
void SOCmnObjectLock::releaseObjectLock(IN SOCmnObject* pObj, IN SOCmnSync* sync)
{
	if (g_objectLock)
	{
		g_objectLock->doReleaseObjectLock(pObj, NULL, sync);
	}
}

void SOCmnObjectLock::releaseObjectLockV(IN void* pVoid, IN SOCmnSync* sync)
{
	if (g_objectLock)
	{
		g_objectLock->doReleaseObjectLock(NULL, pVoid, sync);
	}
}

//#ifdef SOOS_WINDOWS
void SOCmnObjectLock::doReleaseObjectLock(IN SOCmnObject* pObj, IN void* pVoid, IN SOCmnSync* sync)
{
	SOCmnList<SOCmnSyncCounter> syncCounterList(m_syncCounterList);
	void* pV = (pObj != NULL) ? (void*)pObj->getOpcHandle() : pVoid;
	// use array
	DWORD idx;
	idx = ((DWORD)(void*)pV) >> 4;
	idx &= SOCMNOBJECTLOCK_ARRAY_MASK;
	SOCmnSync* pArrayLock = m_syncCounterArray[idx]->getCounterLock();
	pArrayLock->lock();

	if (m_syncCounterArray[idx]->getRefCount() == 1)
	{
		// index not used
		_ASSERTION(FALSE, "no reference in array for sync in SOCmnObjectLock");
		pArrayLock->unlock();
		return;
	}
	else
	{
		// index used
		// check if array is used for the pV
		if (m_syncCounterArray[idx]->getKey() == (size_t)pV)
		{
			if (m_syncCounterArray[idx]->release() == 1)
			{
				m_syncCounterArray[idx]->setKey(0xabcd7531);
				interlockedDecrement(&m_countSyncArrayUsed);
			}

			pArrayLock->unlock();
			return;
		}
	}

	// release reference for using list
	if (m_syncCounterArray[idx]->release() == 1)
	{
		m_syncCounterArray[idx]->setKey(0xabcd7531);
		interlockedDecrement(&m_countSyncArrayUsed);
	}

	pArrayLock->unlock();
	// use list
	syncCounterList.autoLock();
	SOCmnPointer<SOCmnSyncCounter> syncCounter = syncCounterList.findKey((size_t)pV);

	if (!syncCounter)
	{
		_ASSERTION(FALSE, "release sync not created with SOCmnObjectLock");
		delete sync;
	}
	else
	{
		if (syncCounter->release() == 2)
		{
			syncCounterList.removeKey((size_t)pV);
			interlockedIncrement(&m_countSyncDestroyed);
			_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG)
			{
				if (pObj)
				{
					_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG, _T("SOCmnObjectLock"), (_T("destroyed 0x%lX | %s | (%ld)"),pObj->getOpcHandle(), pObj->getObjTraceId(), m_countSyncDestroyed));
				}
				else
				{
					_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJECTLOG, _T("SOCmnObjectLock"), (_T("destroyed 0x%lX | pV | (%ld)"), (size_t)pV, m_countSyncDestroyed));
				}
			}
		}
	}
}
//#endif

void SOCmnObjectLock::checkSyncs(void)
{
	SOCmnList<SOCmnSyncCounter> syncCounterList(m_syncCounterList);
	syncCounterList.autoLock();
	LONG useCnt = 0;
	DWORD j;

	for (j = 0; j < SOCMNOBJECTLOCK_ARRAY_MASK + 1; j++)
	{
		if (m_syncCounterArray[j]->getRefCount() > 1)
		{
			useCnt++;
		}
	}

	if (useCnt != m_countSyncArrayUsed)
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnObjectLock"), (_T("SOCmnObjectLock invalid counts! array use %ld != sync with ref counts %ld"), m_countSyncArrayUsed, useCnt));
	}

	if (m_countSyncCreated != (LONG)(m_countSyncDestroyed + syncCounterList.getCount()))
	{
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnObjectLock"), (_T("SOCmnObjectLock invalid counts! created %ld != %ld ( destroyed %ld + in use %ld )"), m_countSyncCreated, (m_countSyncDestroyed + syncCounterList.getCount()), m_countSyncDestroyed, syncCounterList.getCount()));
	}
}
