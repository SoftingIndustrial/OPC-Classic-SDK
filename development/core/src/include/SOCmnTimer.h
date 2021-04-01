#include "SOCmnOsCompat.h"

#ifndef _SOCMNTIMER_H_
#define _SOCMNTIMER_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


//SOCMN
#define SOCMNTIMER_MAXCNT 20

typedef VOID (_stdcall* SOCMNTIMERPROC)(DWORD id, void* pData, DWORD tickCount);

class SOCmnTimerData : public SOCmnData
{
public:
	SOCmnTimerData();
	~SOCmnTimerData();
	DWORD m_dwTimeToWait;
	DWORD m_dwInitTime;
	DWORD m_dwID;

	DWORD m_dwKillEvent;
	DWORD m_dwTimeEvent;
	DWORD m_dwCallEvent;

	BOOL  m_boSavePeriodic;
	BOOL  m_boPeriodic;
	BOOL  m_boOnce;
	BOOL  m_boRunning;
	void*  m_pData;
	SOCMNTIMERPROC m_tpTimerProc;
};

class  SOCmnTimer: public SOCmnThread
{
public:
	SOCmnTimer();
	~SOCmnTimer();

	virtual DWORD setTimer(DWORD dwTimeToWait, void* timerContext = NULL, SOCMNTIMERPROC tpTimerProc = NULL , BOOL boPeriodic = FALSE);
	virtual DWORD changeTimer(DWORD dwID, DWORD dwTimeToWait, void* timerContext = NULL, SOCMNTIMERPROC tpTimerProc = NULL , BOOL boPeriodic = FALSE);
	virtual void onTimer(DWORD dwTimer);
	virtual BOOL killTimer(DWORD dwTimer);
	virtual BOOL callTimer(DWORD dwTimer);
	virtual BOOL resetTimer(DWORD dwTimer);
	virtual BOOL destroyTimer();

	DWORD getTimerCount(void)
	{
		SOCmnList<SOCmnTimerData> timerList(m_tlTimerList);
		return timerList.getCount();
	}

protected:
	BOOL m_boStarted;
	SOCmnList<SOCmnTimerData> m_tlTimerList;
	DWORD m_workEventID;

	virtual DWORD work(void);
	virtual void processTimer();
	void processEvent(DWORD dwTimer);
	virtual DWORD calctimeout();
};

#pragma pack(pop)

#endif
