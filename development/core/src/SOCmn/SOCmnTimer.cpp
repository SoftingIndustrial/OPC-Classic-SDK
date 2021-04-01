#include "stdafx.h"
#include "SOCmnTimer.h"
#include <SOCmn.h>

SOCmnTimerData::SOCmnTimerData()
{
	m_dwKillEvent = 0;
	m_dwTimeEvent = 0;
	m_dwTimeToWait = INFINITE;
	m_dwInitTime = 0;
	m_dwID = 0;
	m_tpTimerProc = NULL;
	m_boPeriodic = FALSE;
	m_boSavePeriodic = FALSE;
	m_boRunning = FALSE;
	m_boOnce = FALSE;
	m_dwCallEvent = 0;
	m_boRunning = FALSE;
	m_pData = NULL;
}

SOCmnTimerData::~SOCmnTimerData()
{
}


//-----------------------------------------------------------------------------
// SOCmnTimer
//
// - Class implements a (periodic) Timer
//
SOCmnTimer::SOCmnTimer()
{
	m_boStarted = FALSE ;
	m_tlTimerList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_workEventID = createEvent();
}

//-----------------------------------------------------------------------------
// ~SOCmnTimer
//
// Destructs the Timer;
//
SOCmnTimer::~SOCmnTimer()
{
	if (m_boStarted)
	{
		getEvents()->signal(getStopEventId());
		m_boStarted = FALSE;
	}
}


//-----------------------------------------------------------------------------
// processEvent(void)
//
// -  routine to Process a TimerEvent that Has Occurred
//
void SOCmnTimer::processEvent(DWORD dwTimer)
{
	SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);
	SOCmnListPosition pos;
	pos = timerList.getStartPosition();

	while (pos)
	{
		SOCmnTimerData* pTimer  = timerList.getNext(pos);

		if (pTimer->m_dwCallEvent == dwTimer && pTimer->m_boRunning)
		{
			onTimer(pTimer->m_dwID);
		}
		else if (pTimer->m_dwKillEvent == dwTimer)
		{
			pTimer->m_boOnce = FALSE;
			pTimer->m_boPeriodic = FALSE;
			pTimer->m_boRunning = FALSE;
		}
		else if (pTimer->m_dwTimeEvent == dwTimer)
		{
			pTimer->m_boRunning = TRUE;
			pTimer->m_boOnce = !pTimer->m_boSavePeriodic;
			pTimer->m_boPeriodic = pTimer->m_boSavePeriodic;
			pTimer->m_dwInitTime = GetTickCount();
		}
	}
}

//-----------------------------------------------------------------------------
// processTimer(void)
//
// - main routine to check if TimerEvent Has Occurred
//
// returns:
//		min of time to wait;
//
//
void SOCmnTimer::processTimer()
{
	DWORD dwElapsedTime = INFINITE;
	DWORD dwNow = GetTickCount();
	SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);
	SOCmnListPosition pos;
	pos = timerList .getStartPosition();

	while (pos)
	{
		SOCmnTimerData* pTimer  = timerList.getNext(pos);
		dwElapsedTime = dwNow - pTimer->m_dwInitTime; // TODO REPLACE -

		if (dwElapsedTime >= pTimer->m_dwTimeToWait)
		{
			if (pTimer->m_boPeriodic || pTimer->m_boOnce)
			{
				if (pTimer->m_boPeriodic)
				{
					pTimer->m_dwInitTime = pTimer->m_dwInitTime + pTimer->m_dwTimeToWait;
				}

				onTimer(pTimer->m_dwID);
			}

			if (pTimer->m_boOnce)
			{
				pTimer->m_boRunning  = FALSE;
				pTimer->m_boOnce     = FALSE;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// calctimeout(void)
//
// - Calculates time for waitForEvents
//
// returns:
//		min of time to wait;
//
DWORD SOCmnTimer::calctimeout()
{
	DWORD dwTimeOut = INFINITE;
	DWORD dwElapsedTime = INFINITE;
	DWORD dwNow = GetTickCount();
	SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);
	SOCmnListPosition pos;
	pos = timerList.getStartPosition();

	while (pos)
	{
		SOCmnTimerData* pTimer  = timerList.getNext(pos);

		if (pTimer->m_boRunning)
		{
			dwElapsedTime = getTimeSpan(pTimer->m_dwInitTime, dwNow);

			if (dwElapsedTime < pTimer->m_dwTimeToWait)
			{
				DWORD dwDifference = getTimeSpan(dwElapsedTime, pTimer->m_dwTimeToWait);
				dwTimeOut = __min(dwTimeOut, dwDifference);
			}
			else
			{
				return 0;
			}
		}
	}

	return dwTimeOut;
}

//-----------------------------------------------------------------------------
// work(void)
//
// - main routine to check if TimerEvent Has Occurred
//
// returns:
//		getStopEventId() (i.e. 0)
//
DWORD SOCmnTimer::work(void)
{
	DWORD dwEvent, dwTimeOut;
	_ASSERTION(isOwnThread(), "isOwnThread");
	_ASSERTION(setPriority(THREAD_PRIORITY_NORMAL), "PRIORITY_NORMAL");

	do
	{
		dwTimeOut = calctimeout();
		dwEvent = waitForEvents(dwTimeOut);

		if (dwEvent == m_workEventID)
		{
			continue; // recalculate timeouts
		}
		else if (dwEvent == SOCMNEVENT_INVALID_ID)
		{
			processTimer(); // Time elapsed
		}
		else if (dwEvent != getStopEventId())
		{
			processEvent(dwEvent); // Event occured
		}

		//_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ALL, _T("SOCmnTimer::work"), (_T("received event %lx (GetTickCount=%ld)"), dwEvent, GetTickCount()));
	}
	while (dwEvent != getStopEventId());

	return dwEvent;
}

//-----------------------------------------------------------------------------
// OnTimer
//
// - Called if TimerEvent has occurred
//		may be overloaded
//
void SOCmnTimer::onTimer(DWORD dwTimer)
{
	SOCmnListPosition pos;
	SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);
	pos = timerList.getStartPosition();

	while (pos)
	{
		SOCmnTimerData* pTimer  = timerList.getNext(pos);

		if (pTimer->m_dwID == dwTimer &&  pTimer->m_tpTimerProc)
		{
			pTimer->m_tpTimerProc(dwTimer, pTimer->m_pData, GetTickCount());
		}
	}
}

//-----------------------------------------------------------------------------
// SetTimer
//
//  DWORD dwTimeToWait       : time until TimeEvent occurs
//	void *timerContext        : optional Pointer to Data;
//  SOCMNTIMERPROC tpTimeProc: optional Timeproc
//	BOOL boPeriodic           : optional whether timer is periodic
//
//  return value : Timer ID of changed/created timer
//
// - Called to set up (i.e. create) a timer
//		may be overloaded but the inherited function should be called ...
//
DWORD SOCmnTimer::setTimer(DWORD dwTimeToWait, void* timerContext, SOCMNTIMERPROC tpTimerProc , BOOL boPeriodic)
{
	SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);

	if (!m_boStarted)
	{
		m_boStarted = start(NULL, 0);
	}

	if (m_boStarted)
	{
		if (timerList.getCount() > SOCMNTIMER_MAXCNT)
		{
			return 0xFFFFFFFF;
		}

		SOCmnTimerData* pTimer = new SOCmnTimerData;
		pTimer->m_dwKillEvent = createEvent();
		pTimer->m_dwTimeEvent = createEvent();
		pTimer->m_dwCallEvent = createEvent();
		pTimer->m_dwID = pTimer->m_dwCallEvent;
		pTimer->m_boOnce = !boPeriodic;

		if (dwTimeToWait < 3)
		{
			dwTimeToWait = 2;
		}

		pTimer->m_dwTimeToWait = dwTimeToWait;
		pTimer->m_tpTimerProc = tpTimerProc;
		pTimer->m_boPeriodic = boPeriodic;
		pTimer->m_boSavePeriodic = boPeriodic;
		pTimer->m_pData = timerContext;
		pTimer->m_dwInitTime = GetTickCount();
		pTimer->m_boRunning = TRUE;
		m_tlTimerList.add(pTimer);
		getEvents()->signal(pTimer->m_dwTimeEvent);
		getEvents()->signal(m_workEventID);
		return pTimer->m_dwID;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}
//-----------------------------------------------------------------------------
// ChangeTimer
//
//  DWORD dwID               : should be a valid timer ID if it is NULL a new timer is created
//  DWORD dwTimeToWait       : time until TimeEvent occurs
//	void *timerContext        : optional Pointer to Data;
//  SOCMNTIMERPROC tpTimeProc: optional Timeproc
//	BOOL boPeriodic           : optional whether timer is periodic
//
//  return value : Timer ID of changed/created timer
//
// - Called to Change or create a timer
//		may be overloaded but the inherited function should be called ...
//
DWORD SOCmnTimer::changeTimer(DWORD dwID, DWORD dwTimeToWait, void* timerContext, SOCMNTIMERPROC tpTimerProc , BOOL boPeriodic)
{
	if (dwID == 0)
	{
		return setTimer(dwTimeToWait, timerContext,  tpTimerProc , boPeriodic);
	}
	else
	{
		SOCmnListPosition pos;
		pos = m_tlTimerList.getStartPosition();

		while (pos)
		{
			SOCmnTimerData* pTimer  = m_tlTimerList.getNext(pos);

			if (pTimer->m_dwID == dwID)
			{
				pTimer->m_boOnce = !boPeriodic;
				pTimer->m_boPeriodic = boPeriodic;
				pTimer->m_boSavePeriodic = boPeriodic;

				if (dwTimeToWait < 3)
				{
					dwTimeToWait = 2;
				}

				pTimer->m_dwTimeToWait = dwTimeToWait;

				if (tpTimerProc)
				{
					pTimer->m_tpTimerProc = tpTimerProc;
				}

				if (timerContext)
				{
					pTimer->m_pData = timerContext;
				}

				pTimer->m_boRunning = TRUE;
				pTimer->m_dwInitTime = GetTickCount();
				getEvents()->signal(pTimer->m_dwTimeEvent);
				return dwID;
			}
		}

		return 0xFFFFFFFF;
	}
}

//-----------------------------------------------------------------------------
// KillTimer
//
//
// - Called to suspend(!) the timer
//		may be overloaded but the inherited function should be called ...
//
BOOL SOCmnTimer::killTimer(DWORD dwTimer)
{
	if (m_boStarted)
	{
		SOCmnListPosition pos;
		pos = m_tlTimerList.getStartPosition();

		while (pos)
		{
			SOCmnTimerData* pTimer  = m_tlTimerList.getNext(pos);

			if (pTimer->m_dwID == dwTimer)
			{
				pTimer->m_boRunning = FALSE;
				return getEvents()->signal(pTimer->m_dwKillEvent);
			}
		}

		return FALSE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// DestroyTimer
//
//
// - Called to terminate the timer
//		may be overloaded but the inherited function should be called ...
//
BOOL SOCmnTimer::destroyTimer()
{
	if (m_boStarted)
	{
		getEvents()->signal(getStopEventId());
		m_boStarted = FALSE;
		return stop();
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// ResetTimer
//
//
// - Called to re-init the timer
//		may be overloaded but the inherited function should be called ...
//
BOOL SOCmnTimer::resetTimer(DWORD dwTimer)
{
	if (m_boStarted)
	{
		SOCmnListPosition pos;
		pos = m_tlTimerList.getStartPosition();

		while (pos)
		{
			SOCmnTimerData* pTimer  = m_tlTimerList.getNext(pos);

			if (pTimer->m_dwID == dwTimer)
			{
				return getEvents()->signal(pTimer->m_dwTimeEvent);
			}
		}

		return FALSE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// CallTimer
//
//
// - Called to immediatly let occur the TimerEvent
//		may be overloaded but the inherited function should be called ...
//
BOOL SOCmnTimer::callTimer(DWORD dwTimer)
{
	if (m_boStarted)
	{
		SOCmnListPosition pos;
		pos = m_tlTimerList.getStartPosition();

		while (pos)
		{
			SOCmnTimerData* pTimer  = m_tlTimerList.getNext(pos);

			if (pTimer->m_dwID == dwTimer)
			{
				return getEvents()->signal(pTimer->m_dwCallEvent);
			}
		}

		return FALSE;
	}
	else
	{
		return FALSE;
	}
}

