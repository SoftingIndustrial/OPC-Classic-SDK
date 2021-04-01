#ifndef _SOCMNSYNC_H_
#define _SOCMNSYNC_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"
#include "SOCmnList.h"

#ifdef SOOS_LINUX
#include "pthread.h"
#ifndef INFINITE
#define INFINITE ((DWORD) -1)
#endif
#endif

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCmnSync                                                                  |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnSync
{
public:
	SOCmnSync();
	~SOCmnSync();

	// gain access to the sync object
	BOOL Lock(void);
	BOOL lock(IN OPTIONAL DWORD timeout = INFINITE);

	// release access to the sync object
	BOOL Unlock(void);
	BOOL unlock(void);

protected:
#ifdef SOOS_LINUX

	pthread_mutex_t m_mutex;

#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	HANDLE m_waitEvent;     // wait event if other thread owns sync
	DWORD m_ownThreadId;    // id of owning thread
	LONG m_lockCount;       // current number of locks
	LONG m_recurseCount;    // recurse lock calls of the thread

#endif
}; // SOCmnSync

inline BOOL SOCmnSync::Lock(void)
{
	return lock();
}

inline BOOL SOCmnSync::Unlock(void)
{
	return unlock();
}



//-----------------------------------------------------------------------------
// SOCmnSyncCounter                                                                 |
//-----------------------------------------------------------------------------
class SOCMN_EXPORT SOCmnSyncCounter : public SOCmnObject
{
public:
	SOCmnSyncCounter(SOCmnSync* aSyncObject, SOCmnSync* aCounterLock);
	~SOCmnSyncCounter();

	SOCmnSync* getSyncObject(void)
	{
		return m_syncObject;
	}
	SOCmnSync* getCounterLock(void)
	{
		return m_counterLock;
	}

	size_t getKey(void)
	{
		return m_key;
	}
	void setKey(size_t key)
	{
		m_key = key;
	}

protected:
	SOCmnSync* m_syncObject;
	size_t m_key;
	SOCmnSync* m_counterLock;
};



//-----------------------------------------------------------------------------
// SOCmnMutex                                                                 |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnMutex
{
public:
	SOCmnMutex();
	~SOCmnMutex();

	// gain access to the mutex object
	BOOL lock(IN OPTIONAL DWORD timeout = INFINITE);

	// release access to the mutex object
	BOOL unlock(void);

protected:
#ifdef SOOS_LINUX

	pthread_mutex_t m_mutex;

#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	HANDLE m_mutex; // handle of system mutex

#endif
}; // SOCmnMutex




//-----------------------------------------------------------------------------
// SOCmnSingleLock                                                            |
//-----------------------------------------------------------------------------

template <class T> class SOCmnSingleLock
{
public:
	SOCmnSingleLock(IN T* sync)
	{
		m_sync = sync;
		lock();
	}

	~SOCmnSingleLock()
	{
		unlock();
	}

	// gain access to the sync object
	BOOL lock(void)
	{
		if (m_sync)
		{
			return m_sync->lock();
		}
		else
		{
			return FALSE;
		}
	}

	// release access to the sync object
	BOOL unlock(void)
	{
		if (m_sync)
		{
			return m_sync->unlock();
		}
		else
		{
			return FALSE;
		}
	}

protected:
	T* m_sync;
}; // SOCmnSingleLock




//-----------------------------------------------------------------------------
// SOCmnObjectLock                                                            |
//-----------------------------------------------------------------------------

class SOCmnObject;

#define SOCMNOBJECTLOCK_ARRAY_MASK 0x07FF

class SOCMN_EXPORT SOCmnObjectLock
{
public:
	SOCmnObjectLock();
	~SOCmnObjectLock();

	// gain access to the sync object
	static BOOL lock(SOCmnObject* pObj)
	{
		return getObjectLock(pObj)->lock();
	}

	// release access to the sync object
	static BOOL unlock(SOCmnObject* pObj)
	{
		return getObjectLock(pObj)->unlock();
	}

	static SOCmnSync* getObjectLock(IN SOCmnObject* pObj);
	static SOCmnSync* getObjectLockV(IN void* pVoid);

	static void releaseObjectLock(IN SOCmnObject* pObj, IN SOCmnSync* sync);
	static void releaseObjectLockV(IN void* pVoid, IN SOCmnSync* sync);

	void checkSyncs(void);

protected:
	SOCmnList<SOCmnSyncCounter> m_syncCounterList;
	SOCmnSyncCounter* m_syncCounterArray[SOCMNOBJECTLOCK_ARRAY_MASK + 1];

	LONG m_countSyncCreated;
	LONG m_countSyncDestroyed;
	LONG m_countSyncArrayUsed;

	SOCmnSync* doGetObjectLock(IN SOCmnObject* pObj, IN void* pVoid);
	void doReleaseObjectLock(IN SOCmnObject* pObj, IN void* pVoid, IN SOCmnSync* sync);
}; // SOCmnObjectLock

void createObjectLock(void);
void destroyObjectLock(void);



//-----------------------------------------------------------------------------
// SOCmnSingleObjectLock                                                      |
//-----------------------------------------------------------------------------

template <class T> class SOCmnSingleObjectLock
{
public:
	SOCmnSingleObjectLock(IN SOCmnObject* pObj)
	{
		m_obj = pObj;
		m_v = NULL;
		m_sync = SOCmnObjectLock::getObjectLock(pObj);
		lock();
	}
	SOCmnSingleObjectLock(IN SOCmnSync* sync, IN void* pV)
	{
		m_v = pV;
		m_obj = NULL;
		m_sync = sync;
		lock();
	}

	~SOCmnSingleObjectLock()
	{
		unlock();

		if (m_obj)
		{
			SOCmnObjectLock::releaseObjectLock(m_obj, m_sync);
		}
		else
		{
			SOCmnObjectLock::releaseObjectLockV(m_v, m_sync);
		}
	}

	// gain access to the sync object
	BOOL lock(void)
	{
		if (m_sync)
		{
			return m_sync->lock();
		}
		else
		{
			return FALSE;
		}
	}

	// release access to the sync object
	BOOL unlock(void)
	{
		if (m_sync)
		{
			return m_sync->unlock();
		}
		else
		{
			return FALSE;
		}
	}

protected:
	T* m_sync;
	SOCmnObject* m_obj;
	void* m_v;
}; // SOCmnSingleObjectLock

#pragma pack(pop)
#endif
