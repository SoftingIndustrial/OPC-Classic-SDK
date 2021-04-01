#ifndef _SOCMNEVENT_H_
#define _SOCMNEVENT_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#ifdef SOOS_LINUX
#include "pthread.h"
#ifndef INFINITE
#define INFINITE ((DWORD) -1)
#endif
#endif

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCmnEvent                                                                 |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnEvent
{
public:
	SOCmnEvent(void);
	~SOCmnEvent(void);

	BOOL create(IN OPTIONAL BOOL manualReset = FALSE, IN OPTIONAL BOOL initialState = FALSE, IN OPTIONAL LPCTSTR name = NULL);
	BOOL signal(void);
	BOOL reset(void);
	BOOL waitFor(IN OPTIONAL DWORD timeout = INFINITE);
	BOOL isInit(void);

#ifdef SOOS_WINDOWS
	HANDLE getHandle(void)
	{
		return m_handle;
	}
	void setHandle(HANDLE handle)
	{
		m_handle = handle;
	}
	BOOL create(IN SECURITY_ATTRIBUTES* pSD, IN BOOL manualReset, IN BOOL initialState, IN LPCTSTR name);
#endif

protected:
#ifdef SOOS_LINUX

	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;

	BOOL m_manualReset;
	BOOL m_signal;
	BOOL m_isInit;

#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	HANDLE m_handle;
	BYTE m_flags;

#endif
}; // SOCmnEvent

#define SOCMNEVENT_INVALID_ID 0xFFFFFFFF

class SOCMN_EXPORT SOCmnEvents
{
public:
	SOCmnEvents(void);
	~SOCmnEvents(void);

	DWORD create(IN OPTIONAL BOOL manualReset = FALSE, IN OPTIONAL BOOL initialState = FALSE, IN OPTIONAL LPCTSTR name = NULL);
	BOOL signal(DWORD eventId);
	BOOL reset(DWORD eventId);
	DWORD waitForAll(IN OPTIONAL BOOL waitAll = FALSE, IN OPTIONAL DWORD timeout = INFINITE);
	BOOL waitFor(DWORD eventId, IN OPTIONAL DWORD timeout = INFINITE);

	BOOL isInit(DWORD eventId);

#ifdef SOOS_WINDOWS
	DWORD create(IN SECURITY_ATTRIBUTES* pSD, IN BOOL manualReset, IN BOOL initialState, IN LPCTSTR name);
#endif

protected:
#ifdef SOOS_LINUX

	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;

	DWORD m_eventCnt;
	BOOL* m_manualReset;
	BOOL* m_signal;

#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	HANDLE* m_pHandles;
	DWORD m_eventCnt;
	BYTE* m_pFlags;

#endif
}; // SOCmnEvents

#ifdef SOOS_WINDOWS
inline BOOL SOCmnEvent::signal(void)
{
	return ::SetEvent(m_handle);
}
inline BOOL SOCmnEvent::reset(void)
{
	return ::ResetEvent(m_handle);
}
inline BOOL SOCmnEvent::waitFor(IN OPTIONAL DWORD timeout)
{
	return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_handle, timeout));
}
inline BOOL SOCmnEvent::isInit(void)
{
	return (m_handle != 0);
}
inline BOOL SOCmnEvents::signal(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		return ::SetEvent(m_pHandles[eventId]);
	}
	else
	{
		return FALSE;
	}
};
inline BOOL SOCmnEvents::reset(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		return ::ResetEvent(m_pHandles[eventId]);
	}
	else
	{
		return FALSE;
	}
};
inline DWORD SOCmnEvents::waitForAll(IN OPTIONAL BOOL waitAll, IN OPTIONAL DWORD timeout)
{
	DWORD res = ::WaitForMultipleObjects(m_eventCnt, m_pHandles, waitAll, timeout);

	if (res == WAIT_TIMEOUT)
	{
		return SOCMNEVENT_INVALID_ID;
	}
	else
	{
		return (res - WAIT_OBJECT_0);
	}
}
inline BOOL SOCmnEvents::waitFor(DWORD eventId, IN OPTIONAL DWORD timeout)
{
	if ((eventId) < m_eventCnt)
		return
			(WAIT_OBJECT_0 == ::WaitForSingleObject(m_pHandles[eventId], timeout));
	else
	{
		return FALSE;
	}
}
inline BOOL SOCmnEvents::isInit(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		return (m_pHandles[eventId] != 0);
	}
	else
	{
		return FALSE;
	}
}
#endif // SOOS_WINDOWS


class SOCMN_EXPORT SOCmnEventPointer
{
public:
	SOCmnEventPointer(void);
	SOCmnEventPointer(SOCmnEvent* pEvent);
	SOCmnEventPointer(SOCmnEvents* pEvents, DWORD eventId);
	SOCmnEventPointer(IN const SOCmnEventPointer& eP);
	~SOCmnEventPointer(void);

	const SOCmnEventPointer& operator=(IN SOCmnEventPointer& eP);
	void set(IN SOCmnEventPointer eP);

	BOOL isInit(void);
	BOOL signal(void);
	BOOL reset(void);
	BOOL waitFor(IN OPTIONAL DWORD timeout = INFINITE);

	BOOL operator==(IN const SOCmnEventPointer& ep) const
	{
		return ((m_id == ep.m_id) && (m_pEvent == ep.m_pEvent));
	}
	BOOL operator!=(IN const SOCmnEventPointer& ep) const
	{
		return ((m_id != ep.m_id) || (m_pEvent != ep.m_pEvent));
	}

protected:
	DWORD m_id;
	union
	{
		SOCmnEvent* m_pEvent;
		SOCmnEvents* m_pEvents;
	};
}; // SOCmnEventPointer



#pragma pack(pop)
#endif
