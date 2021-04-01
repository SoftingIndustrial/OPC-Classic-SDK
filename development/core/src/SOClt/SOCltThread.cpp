#include "stdafx.h"

#ifdef SOCLT

#include "SOCltThread.h"
#include "SOCmnHandleManager.h"

#define END_EVENT       0
#define WAKEUP_EVENT    1
#define CYCLIC_EVENT    2

//-----------------------------------------------------------------------------
// SOCltWorkerThread                                                          |
//-----------------------------------------------------------------------------
SOCltWorkerThread::SOCltWorkerThread()
{
	// create events for communication with the worker thread
	m_evtWakeup = createEvent();
	m_evtCyclic = createEvent();
	m_error = NULL;
	m_taskQueue.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_cyclicQueue.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	// create thread
	start(NULL, 0);
}

SOCltWorkerThread::~SOCltWorkerThread()
{
	if (m_started)
	{
		shutdown(INFINITE);
	}
}

//-----------------------------------------------------------------------------
// shutdown
//
void SOCltWorkerThread::shutdown(IN DWORD timeout)
{
	m_taskQueue.removeAll();
	m_cyclicQueue.removeAll();

	if (!stop(timeout))
	{
		SOCmnString szTraceID;
		szTraceID.format(_T("Worker Thread"));
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_CLT_THREAD, szTraceID, (_T("thread couldn't be stopped in time!!")));
	}

	return;
}

//-----------------------------------------------------------------------------
// cleanUp
//
void SOCltWorkerThread::cleanUp(void)
{
	// Task being currently in the queue will never get
	// executeted. Call reportCompletion so that objects
	// waiting on a task get signaled.
	SOCmnList<SOCltTask> queue(m_taskQueue);
	SOCmnListPosition pos = queue.getStartPosition();

	while (pos)
	{
		SOCltTask* task = queue.getNext(pos);
		task->setResult(E_ABORT);
		task->reportCompletion();
	}

	queue.removeAll();
	SOCmnList<SOCltCyclicTask> queue_cyc(m_cyclicQueue);
	pos = queue_cyc.getStartPosition();

	while (pos)
	{
		SOCltCyclicTask* task = queue_cyc.getNext(pos);
		task->setResult(E_ABORT);
		task->reportCompletion();
	}

	queue_cyc.removeAll();
}

//-----------------------------------------------------------------------------
// setThreadPriority
//
BOOL SOCltWorkerThread::setThreadPriority(IN int priority)
{
	_ASSERTION(isRunning() == TRUE, _T("thread isn't running!"));
	return setPriority(priority);
} // setThreadPriority

//-----------------------------------------------------------------------------
// addTask
//

BOOL SOCltWorkerThread::addTask(SOCltTask* task, BOOL wakeup /*= TRUE*/)
{
	_ASSERT_PTR(task);
	BOOL ok = TRUE;
	SOCmnList<SOCltTask> queue(m_taskQueue);
	ok = queue.add(task);

	if (ok && wakeup)
	{
		wakeUp();
	}

	return ok;
} // addTask


//-----------------------------------------------------------------------------
// addCyclicTask
//
BOOL SOCltWorkerThread::addCyclicTask(IN SOCltCyclicTask* task, DWORD* cookie /*= NULL*/)
{
	_ASSERT_PTR(task);
	BOOL ok = TRUE;
	SOCmnList<SOCltCyclicTask> queue(m_cyclicQueue);
	// queue in new task
	ok = queue.add(task, (DWORD)task->getOpcHandle());

	if (cookie)
	{
		*cookie = (DWORD)task->getOpcHandle();
	}

	// wake up
	ok = getEvents()->signal(m_evtCyclic);
	return ok;
} // addCyclicTask


//-----------------------------------------------------------------------------
// removeCyclicTask
//
BOOL SOCltWorkerThread::removeCyclicTask(IN DWORD cookie)
{
	SOCmnList<SOCltCyclicTask> list(m_cyclicQueue);
	return list.removeKey(cookie);
} // removeCyclicTask


//-----------------------------------------------------------------------------
// getCyclicTask
//
SOCltCyclicTask* SOCltWorkerThread::getCyclicTask(IN DWORD cookie)
{
	SOCmnList<SOCltCyclicTask> list(m_cyclicQueue);
	return list.findKey(cookie);
}


//-----------------------------------------------------------------------------
// wakeUp
//
void SOCltWorkerThread::wakeUp()
{
	_ASSERTION(isRunning() == TRUE, _T("thread isn't running!"));
	getEvents()->signal(m_evtWakeup);
} // wakeUp

void SOCltWorkerThread::wakeUpCyclic()
{
	_ASSERTION(isRunning() == TRUE, _T("thread isn't running!"));
	getEvents()->signal(m_evtCyclic);
} // wakeUp



//-----------------------------------------------------------------------------
// work
//
// - thread routine of work thread
//
// returns:
//		thread end state
//
DWORD SOCltWorkerThread::work(void)
{
	BOOL end = FALSE;
	static TCHAR* szTraceID = _T("Worker Thread");
#ifdef SOFEATURE_DCOM
	// init COM
	HRESULT res;

	if (FAILED(res = ::CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		if (m_error)
		{
			m_error->setLastError(res, SOCLT_ERR_COM_INIT);
		}

		return res;
	}

#endif
	DWORD nextTimeToRunThread = INFINITE;
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_CLT_THREAD, szTraceID, (_T("started")));

	while (!end)
	{
		HRESULT hrTask = S_OK;
		SOCMN_TRY
		{
			DWORD waitTime = nextTimeToRunThread == INFINITE ? INFINITE : getTimeSpan(::GetTickCount(), nextTimeToRunThread);

			if (waitTime > SOCMN_TIME_40DAYS && waitTime != INFINITE)
			{
				waitTime = 0;
			}

			DWORD ret = getEvents()->waitForAll(FALSE, waitTime);
			_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_CLT_THREAD, szTraceID, (_T("running")));

			if (ret == getStopEventId())
			{
				// terminate worker thread
				end = TRUE;
			}
			else if ((ret == SOCMNEVENT_INVALID_ID) || (ret == m_evtCyclic))
			{
				SOCmnList<SOCltCyclicTask> list(m_cyclicQueue);
				SOCmnListPosition pos;
				list.dontAutoLock();
				list.lock();
				list.setDirty(FALSE);
				pos = list.getStartPosition();
				list.unlock();
				nextTimeToRunThread = INFINITE;

				while (pos)
				{
					SOCmnPointer<SOCltCyclicTask> task;
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

					task = list.getNext(pos);
					task.addRef();
					list.unlock();
					DWORD timeToRun = task->getNextTimeToRun();
					DWORD timeSpan = getTimeSpan(::GetTickCount(), timeToRun);

					if ((0 == timeSpan) || (SOCMN_TIME_40DAYS < timeSpan))
					{
						task->execute();
						hrTask = task->getResult();
						task->reportCompletion();
						timeToRun = task->getNextTimeToRun();
					}

					if (nextTimeToRunThread == INFINITE)
					{
						nextTimeToRunThread = timeToRun;
					}
					else
					{
						ULONG span;

						if (timeToRun >= nextTimeToRunThread)
						{
							span = timeToRun - nextTimeToRunThread;
						}
						else
						{
							span = nextTimeToRunThread - timeToRun;
						}

						if (span < SOCMN_TIME_40DAYS)
						{
							nextTimeToRunThread = __min(nextTimeToRunThread, timeToRun);
						}
						else
						{
							nextTimeToRunThread = __max(nextTimeToRunThread, timeToRun);
						}
					}
				}
			}
			else if (ret == m_evtWakeup)
			{
				while (1)
				{
					m_taskQueue.lock();
					SOCltTask* task = m_taskQueue.getHead();

					if (task)
					{
						task->addRef();
					}

					m_taskQueue.removeHead();
					m_taskQueue.unlock();

					if (NULL == task)
					{
						break;
					}

					task->execute();
					task->reportCompletion();
					task->release();
				}
			}

#ifdef SOFEATURE_DCOM
			// Analyze HRESULT returned by the task to see
			// if an RPC error has occured
			BOOL bRPCError = FALSE;

			switch (hrTask)
			{
			default:
				break;

				//case HRESULT_FROM_WIN32(RPC_S_CALL_FAILED):
			case 0x800706be:

				//case HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE):
			case 0x800706ba:

				//case HRESULT_FROM_WIN32(RPC_S_CALL_FAILED_DNE):
			case 0x800706bf:
#if _MSC_VER > 1100
			case RPC_E_TIMEOUT:
#endif
				bRPCError = TRUE;
				break;
			}

			if (bRPCError)
			{
				// empty the task queue
				SOCmnList<SOCltTask> queue(m_taskQueue);

				while (1)
				{
					SOCltTask* task = queue.getHead();

					if (task)
					{
						task->addRef();
					}

					queue.removeHead();

					if (NULL == task)
					{
						break;
					}

					task->setResult(E_ABORT);
					task->reportCompletion();
					task->release();
				}
			}

#endif // SOFEATURE_DCOM
		}
		SOCMN_CATCH(...)
		{
			if (m_error)
			{
				m_error->setLastError(E_FAIL, SOCLT_ERR_EXCEPTION);
			}
		}
	}

	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_CLT_THREAD, szTraceID, (_T("cleaning up...")));
	cleanUp();
	// Terminate
#ifdef SOFEATURE_DCOM
	::CoUninitialize();
#endif
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_CLT_THREAD, szTraceID, (_T("terminated")));
	return 0;
} // workThread

#endif // SOCLT


