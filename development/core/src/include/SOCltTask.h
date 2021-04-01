#ifndef _SOCLTTASK_H_
#define _SOCLTTASK_H_

#include "SOCltElement.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCltTask                                                                  |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltTask : public SOCmnObject
{
public:
	SOCltTask(IN SOCltElement* element);

	virtual void setCompletionEvent(IN SOCmnEventPointer handle);
	virtual SOCmnEventPointer getCompletionEvent();

	// Called by the worker thread
	virtual void execute() = 0;
	virtual void reportCompletion();

	HRESULT getResult(void);
	void setResult(IN HRESULT res);

protected:

	SOCmnPointer<SOCltElement> m_element;
	SOCmnEventPointer m_event;
	HRESULT m_res;

	virtual ~SOCltTask();
}; // SOCltTask

inline HRESULT SOCltTask::getResult()
{
	return m_res;
}

inline void SOCltTask::setResult(IN HRESULT res)
{
	m_res = res;
}

inline void SOCltTask::setCompletionEvent(IN SOCmnEventPointer handle)
{
	m_event = handle;
}

inline SOCmnEventPointer SOCltTask::getCompletionEvent()
{
	return m_event;
}




//-----------------------------------------------------------------------------
// SOCltTaskUpdate                                                            |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltTaskUpdate : public SOCltTask
{
public:
	SOCltTaskUpdate(IN SOCltElement* element, IN BOOL from_server, IN DWORD whatParameters);
	virtual void execute();

protected:
	BOOL m_fromServer;        // parameter to doUpdate
	DWORD m_whatParameters;   // parameter to doUpdate
};  // SOCltTaskUpdate




//-----------------------------------------------------------------------------
// SOCltTaskStateTransition                                                   |
//-----------------------------------------------------------------------------

#define SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE 0xFF

class SOCLT_EXPORT SOCltTaskStateTransition : public SOCltTask
{
public:
	SOCltTaskStateTransition(IN SOCltElement* element, IN BYTE targetState, IN BOOL deep);
	virtual void execute();
	virtual void reportCompletion();

protected:
	BYTE m_targetState;   // object's target state
	BOOL m_deep;          // TRUE, if should be applied to the children as well
}; // SOCltTaskStateTransition




//-----------------------------------------------------------------------------
// SOCltCyclicTask                                                            |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltCyclicTask : public SOCltTask
{
public:
	SOCltCyclicTask(IN SOCltElement* element, IN DWORD cycle);
	void reportCompletion();

	DWORD getCycle();
	void setCycle(DWORD cycle);

	// Returns time (in ticks from the system start)
	// the task should be executed for the next time
	virtual DWORD getNextTimeToRun(void);

protected:
	DWORD m_cycle;      // cycle (ticks)
	DWORD m_lastCall;   // last call time
}; // SOCltCyclicTask

inline DWORD SOCltCyclicTask::getCycle()
{
	return m_cycle;
}

inline void SOCltCyclicTask::setCycle(DWORD cycle)
{
	m_cycle = cycle;
}




//-----------------------------------------------------------------------------
// SOCltTaskConnectionMonitor                                                 |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltTaskConnectionMonitor : public SOCltCyclicTask
{
public:
	SOCltTaskConnectionMonitor(IN SOCltElement* element, IN DWORD cycle);
	virtual void execute(void);
	virtual DWORD getNextTimeToRun(void);

	DWORD m_attempts;             // max. # of attempts to reconnect server in the 1st phase
	DWORD m_cycle1;               // cycle for the 1st phase
	DWORD m_cycle2;               // cycle for the 2nd phase

protected:
	virtual ~SOCltTaskConnectionMonitor();

	virtual void doDisconnectElement(IN SOCltElement* element);
	void doDisconnectElementsInList(IN SOCmnList<SOCmnElement>* plist);
	virtual void doReconnectElement(IN SOCltElement* element);
	void doReconnectElementsInList(IN SOCmnList<SOCmnElement>* plist);

	typedef enum _tagcmState
	{
		is_idle = 0,
		reconnect1 = 1,
		reconnect2 = 2
	} cmState;

	DWORD m_cnt_attempts;         // attempts counter
	cmState m_state;

}; //SOCltTaskConnectionMonitor

#pragma pack(pop)

#endif // _SOCLTTASK_H_
