#ifndef _SOCMNTHREAD_H_
#define _SOCMNTHREAD_H_

#include "SOCmnEvent.h"

#pragma pack(push, 4)


#ifdef SOOS_LINUX
/* these are the parameter values to SOCmnThread::setPriority
 */
#define THREAD_PRIORITY_IDLE           -4
#define THREAD_PRIORITY_ABOVE_IDLE     -3
#define THREAD_PRIORITY_LOWEST         -2
#define THREAD_PRIORITY_BELOW_NORMAL   -1
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_ABOVE_NORMAL   +1
#define THREAD_PRIORITY_HIGHEST        +2
#define THREAD_PRIORITY_TIME_CRITICAL  +3
#endif // SOOS_LINUX

//-----------------------------------------------------------------------------
// SOCmnThread                                                                |
//-----------------------------------------------------------------------------

typedef DWORD (_stdcall* SOCmnThreadProc)(IN void* context);

class SOCMN_EXPORT SOCmnThread
{
public:
	SOCmnThread(void);
	virtual ~SOCmnThread(void);

	virtual BOOL start(IN SOCmnThreadProc threadProc, IN OPTIONAL void* pContext = NULL);
	virtual BOOL stop(IN OPTIONAL DWORD waitForEndTimeout = INFINITE);
	virtual BOOL detach(void);

	DWORD waitForEvents(IN OPTIONAL DWORD timeout = INFINITE)
	{
		return m_events.waitForAll(FALSE, timeout);
	}
	DWORD createEvent(void)
	{
		return m_events.create();
	}
	SOCmnEvents* getEvents(void)
	{
		return &m_events;
	}

	BOOL waitForStopEvent(IN OPTIONAL DWORD timeout = INFINITE)
	{
		return m_events.waitFor(getStopEventId(), timeout);
	}
	DWORD getStopEventId(void) const
	{
		return 0;
	}

	BOOL setPriority(IN INT priority);

	BOOL isRunning(void) const
	{
		return m_started;
	}

	BOOL isDetached(void) const
	{
		return m_detached;
	}

	BOOL isOwnThread(void) const
#ifdef SOOS_WINDOWS
	{
		return GetCurrentThreadId() == m_id;
	}
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	{
		return pthread_equal(pthread_self(), m_thread);
	}
#endif


protected:
	SOCmnEvents m_events;
	BYTE m_started;
	BYTE m_detached;

#ifdef SOOS_WINDOWS

	DWORD       m_id;
	HANDLE      m_handle;

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

	pthread_t m_thread;

#endif

	virtual DWORD work(void);

protected:
	SOCmnThreadProc m_context1;
	void*           m_context2;
	SOCmnSync       m_startup;

#ifdef SOOS_LINUX
	static void* SOCmnThreadP(void* context);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	static DWORD WINAPI SOCmnThreadP(void* context);
#endif
}; // SOCmnThread

class SOCMN_EXPORT SOCmnWorkItem : public SOCmnObject
{
public:
	virtual void execute(void) = 0;
};

class SOCMN_EXPORT SOCmnThreadPool
{
public:
	SOCmnThreadPool(int numThreads);
	virtual ~SOCmnThreadPool();
	virtual BOOL start();
	virtual BOOL stop();
	virtual BOOL addWorkItem(SOCmnWorkItem* pWorkItem);

protected:
	class SOCMN_EXPORT SOCmnWorkerThread : public SOCmnThread
	{
	public:
		SOCmnWorkerThread(SOCmnThreadPool* parent, int myindex) : m_parent(parent), m_myindex(myindex) {}

	protected:
		virtual DWORD work(void);
		SOCmnThreadPool* const m_parent;
		const int m_myindex;
	};
	friend class SOCmnWorkerThread;

	const int m_nWorkerThreads;
	SOCmnWorkerThread** m_pWorkerThreads;
	SOCmnList<SOCmnWorkItem> m_workItemList;
	SOCmnEvents m_events;
	DWORD m_stopEvent;
	DWORD m_workEvent;
}; // SOCmnThreadPool

#pragma pack(pop)
#endif // _SOCMNTHREAD_H_
