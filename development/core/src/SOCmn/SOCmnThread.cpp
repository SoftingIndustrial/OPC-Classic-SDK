#include "stdafx.h"
#include "SOCmn.h"
#include "SOCmnThread.h"
#include "SOCmnEvent.h"


//-----------------------------------------------------------------------------
// SOCmnThread                                                                |
//-----------------------------------------------------------------------------


SOCmnThread::SOCmnThread(void)
{
	m_started = FALSE;
	m_detached = FALSE;
#ifdef SOOS_WINDOWS
	m_handle = NULL;
	m_id = 0;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_thread  = 0;
#endif
	m_events.create(); // create stop event
}

SOCmnThread::~SOCmnThread(void)
{
	if (!m_detached)
	{
		stop();
	}
}

BOOL SOCmnThread::start(IN SOCmnThreadProc threadProc, IN OPTIONAL void* pContext)
{
	m_startup.lock();

	if (!m_started && m_events.isInit(getStopEventId()))
	{
		// create thread
		m_context1 = threadProc;
		m_context2 = pContext;
#ifdef SOOS_WINDOWS
		m_handle = ::CreateThread(NULL, 0, SOCmnThreadP, (void*)this, 0, &m_id);
		m_started = (m_handle != NULL);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		m_started = !pthread_create(&m_thread, NULL,
									SOCmnThreadP, (void*)this);
#endif
	}

	m_startup.unlock();
	return (m_started);
}

BOOL SOCmnThread::stop(IN OPTIONAL DWORD waitForEndTimeout)
{
	BOOL ret = FALSE;

	if (m_started && m_events.isInit(getStopEventId()))
	{
		m_events.signal(getStopEventId());

		if (!m_detached)
		{
			_ASSERTION(!isOwnThread(), "SOCmnThread::stop called in own thread!");
#ifdef SOOS_WINDOWS

			if (WAIT_OBJECT_0 == WaitForSingleObject(m_handle, waitForEndTimeout))
			{
				ret = TRUE;
			}
			else
			{
				_ASSERTION(ret, "fatal error: thread did not self-terminate!");
				TerminateThread(m_handle, 0);
			}

			CloseHandle(m_handle);
			m_handle = NULL;
			m_id = 0;
			m_started = FALSE;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			ret = pthread_join(m_thread, NULL) == 0;
			m_thread = 0;
			m_started = FALSE;
#endif
		}
	}

	return ret;
}

BOOL SOCmnThread::detach(void)
{
	BOOL ret = FALSE;

	if (m_started && !m_detached)
	{
		m_detached = TRUE;
#ifdef SOOS_WINDOWS
		ret = CloseHandle(m_handle);
		m_handle = NULL;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		ret = !pthread_detach(m_thread);
		// SOCmnThread::isOwnThread may still reference m_thread...
#endif
	}

	return ret;
}

DWORD SOCmnThread::work(void)
{
	waitForStopEvent();
	return 0;
}

#ifdef SOOS_LINUX
void* SOCmnThread::SOCmnThreadP(void* context)
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
DWORD WINAPI SOCmnThread::SOCmnThreadP(void* context)
#endif
{
	DWORD result = 0;
	SOCmnThread*    pThisThread   = (SOCmnThread*) context;
	SOCmnThreadProc threadProc    = pThisThread->m_context1;
	void*           threadContext = pThisThread->m_context2;
	// isOwnThread & setPriority are known to be unreliable
	// while the start function is unfinished, so wait a while
	pThisThread->m_startup.lock();
	pThisThread->m_startup.unlock();
#ifdef NDEBUG
	BOOL end;

	do
	{
		end = TRUE;
		SOCMN_TRY
		{
#endif // NDEBUG


			if (threadProc != NULL)
			{
				result = (*threadProc)(threadContext);
			}
			else
			{ result = pThisThread->work(); }
#ifdef NDEBUG
		}
		SOCMN_CATCH(...)
		{
#ifndef SOOS_WINDOWS_CE
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SRV_THREAD, _T("THREAD"), (_T("EXCEPTION in thread")));
			_ASSERTION(FALSE, "EXCEPTION in thread");
			end = FALSE;
			Sleep(100);
#endif
		}
	}
	while (!end);

#endif // NDEBUG
#ifdef SOOS_LINUX
	return (void*)result;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	return result;
#endif
}

BOOL SOCmnThread::setPriority(IN INT priority)
{
	if (!m_started || m_detached)
	{
		return FALSE;
	}

#ifdef SOOS_WINDOWS
	return SetThreadPriority(m_handle, priority);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	int policy;
	struct sched_param sched;

	switch (priority)
	{
	case THREAD_PRIORITY_TIME_CRITICAL:
		policy = SCHED_RR;
		sched.sched_priority = 2;
		break;

	case THREAD_PRIORITY_HIGHEST:
		policy = SCHED_RR;
		sched.sched_priority = 1;
		break;

	default:
		policy = SCHED_OTHER;
		sched.sched_priority = 0;
		break;
	}

	return !pthread_setschedparam(m_thread, policy, &sched);
#endif
}

SOCmnThreadPool::SOCmnThreadPool(int numThreads)
	: m_nWorkerThreads(numThreads)
{
	m_workItemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO |
						  SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_stopEvent = m_events.create(TRUE, FALSE);
	m_workEvent = m_events.create(FALSE, FALSE);
	m_pWorkerThreads = new SOCmnWorkerThread*[m_nWorkerThreads];

	for (int i = 0; i < m_nWorkerThreads; i++)
	{
		m_pWorkerThreads[i] = new SOCmnWorkerThread(this, i);
	}
}

SOCmnThreadPool::~SOCmnThreadPool(void)
{
	stop();

	for (int i = 0; i < m_nWorkerThreads; i++)
	{
		delete m_pWorkerThreads[i];
	}

	delete []m_pWorkerThreads;
}

BOOL SOCmnThreadPool::start(void)
{
	m_events.reset(m_stopEvent);

	if (m_nWorkerThreads > 0)
	{
		m_pWorkerThreads[0]->start(NULL);
	}

	return TRUE;
}

BOOL SOCmnThreadPool::stop()
{
	m_events.signal(m_stopEvent);

	for (int i = 0; i < m_nWorkerThreads; i++)
	{
		m_pWorkerThreads[i]->stop();
	}

	return TRUE;
}

BOOL SOCmnThreadPool::addWorkItem(SOCmnWorkItem* pWorkItem)
{
	SOCmnList<SOCmnWorkItem> workItemList(&m_workItemList);
	workItemList.add(pWorkItem);
	workItemList.autoUnlock();
	m_events.signal(m_workEvent);
	return TRUE;
}

DWORD SOCmnThreadPool::SOCmnWorkerThread::work(void)
{
	if (m_myindex == 0)
	{
		int nextworker = m_myindex + 1;

		/* manager thread, runs with THREAD_PRIORITY_NORMAL */
		while (m_parent->m_events.waitForAll() == m_parent->m_workEvent)
		{
			/* work stays undone, because managers dont like to work... */
			m_parent->m_events.signal(m_parent->m_workEvent);
			/* try harder to let a worker threads run,
			 * if the priority-method fails...
			 * note: this turns out to be needed under linux
			 *       if the process has no root-privileges, because
			 *       SOCmnThread::setPriority fails in this context.
			 */
#ifdef SOOS_WINDOWS
			Sleep(0);
#endif
#ifdef SOOS_LINUX
			sched_yield();
#endif
			SOCmnList<SOCmnWorkItem> workItemList(&m_parent->m_workItemList);

			if (workItemList.isEmpty())
			{
				continue;
			}

			workItemList.autoUnlock();

			if (nextworker < m_parent->m_nWorkerThreads)
			{
				m_parent->m_pWorkerThreads[nextworker]->start(NULL);
				m_parent->m_pWorkerThreads[nextworker]->setPriority(THREAD_PRIORITY_HIGHEST);
				nextworker++;
				Sleep(10);
				continue;
			}

			m_parent->m_pWorkerThreads[m_myindex]->setPriority(THREAD_PRIORITY_HIGHEST);
			goto sometimes_managers_have_to_work_too;
		}
	}
	else
	{
sometimes_managers_have_to_work_too:

		/* worker thread, runs with THREAD_PRIORITY_HIGHEST */
		while (m_parent->m_events.waitForAll() == m_parent->m_workEvent)
		{
			SOCmnList<SOCmnWorkItem> workItemList(&m_parent->m_workItemList);
			SOCmnPointer<SOCmnWorkItem> workItem = workItemList.pop();

			if (workItem.isNotNull())
			{
				BOOL emptyWorkList = workItemList.isEmpty();
				workItemList.autoUnlock();

				if (!emptyWorkList)
				{
					m_parent->m_events.signal(m_parent->m_workEvent);
				}

				workItem->execute();
			}
		}
	}

	return 0;
}
