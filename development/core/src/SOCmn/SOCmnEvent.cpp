#include "stdafx.h"
#include "SOCmn.h"
#include "SOCmnEvent.h"

//-----------------------------------------------------------------------------
// SOCmnEvent                                                                 |
//-----------------------------------------------------------------------------

#ifdef SOOS_WINDOWS
#define SOCMNEVENT_TYPE_POOLED 0x01
#endif

SOCmnEvent::SOCmnEvent(void)
{
#ifdef SOOS_LINUX
#ifdef SODEVENV_CYGWIN
	memset(&m_mutex, 0, sizeof(m_mutex));
	memset(&m_cond, 0, sizeof(m_cond));
#endif
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
	m_manualReset = FALSE;
	m_signal = FALSE;
	m_isInit = FALSE;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	m_handle = 0;
	m_flags = 0;
#endif
}

SOCmnEvent::~SOCmnEvent(void)
{
#ifdef SOOS_LINUX
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	if (m_handle != 0)
	{
		if (m_flags & SOCMNEVENT_TYPE_POOLED)
		{
			getEventPool()->pushEvent(m_handle);
		}
		else
		{
			::CloseHandle(m_handle);
		}

		m_handle = 0;
	}

#endif
}

BOOL SOCmnEvent::create(IN OPTIONAL BOOL manualReset, IN OPTIONAL BOOL initialState, IN OPTIONAL LPCTSTR name)
{
#ifdef SOOS_LINUX
	_ASSERTION(name == NULL, "linux: named events not implemented");
	_ASSERTION(!m_isInit, "event::create was already called");
	m_manualReset = manualReset;
	m_signal = initialState;
	m_isInit = TRUE;
	return (m_isInit);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	if ((!manualReset) && (!name))
	{
		m_handle = getEventPool()->popEvent();

		if (m_handle != 0)
		{
			if (initialState)
			{
				signal();
			}

			m_flags |= SOCMNEVENT_TYPE_POOLED;
		}
	}
	else
	{
		m_handle = ::CreateEvent(NULL, manualReset, initialState, name);
	}

	return (m_handle != 0);
#endif
}

#ifdef SOOS_WINDOWS
BOOL SOCmnEvent::create(IN SECURITY_ATTRIBUTES* pSD, IN BOOL manualReset, IN BOOL initialState, IN LPCTSTR name)
{
	m_handle = ::CreateEvent(pSD, manualReset, initialState, name);
	return (m_handle != 0);
}
#endif


//-----------------------------------------------------------------------------
// SOCmnEvents                                                                |
//-----------------------------------------------------------------------------

SOCmnEvents::SOCmnEvents(void)
{
#ifdef SOOS_LINUX
#ifdef SODEVENV_CYGWIN
	memset(&m_mutex, 0, sizeof(m_mutex));
	memset(&m_cond, 0, sizeof(m_cond));
#endif
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
	m_eventCnt    = 0;
	m_manualReset = NULL;
	m_signal      = NULL;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	m_pHandles = NULL;
	m_pFlags = NULL;
	m_eventCnt = 0;
#endif
}

SOCmnEvents::~SOCmnEvents(void)
{
#ifdef SOOS_LINUX
	free(m_manualReset);
	free(m_signal);
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

	if (m_pHandles)
	{
		DWORD i;

		for (i = 0; i < m_eventCnt; i++)
		{
			if (m_pHandles[i] != 0)
			{
				if (m_pFlags[i] & SOCMNEVENT_TYPE_POOLED)
				{
					getEventPool()->pushEvent(m_pHandles[i]);
				}
				else
				{
					::CloseHandle(m_pHandles[i]);
				}

				m_pHandles[i] = 0;
			}
		}

		free(m_pHandles);
	}

	if (m_pFlags)
	{
		free(m_pFlags);
	}

#endif
}

DWORD SOCmnEvents::create(IN OPTIONAL BOOL manualReset, IN OPTIONAL BOOL initialState, IN OPTIONAL LPCTSTR name)
{
#ifdef SOOS_LINUX
	_ASSERTION(name == NULL, "linux: named events not implemented");
	const DWORD minalloc = 16;
	DWORD eventId = SOCMNEVENT_INVALID_ID;

	if ((m_eventCnt % minalloc) == 0)
	{
		m_manualReset = (BOOL*)realloc(m_manualReset,
									   sizeof(BOOL) * (m_eventCnt + minalloc));
		m_signal = (BOOL*)realloc(m_signal,
								  sizeof(BOOL) * (m_eventCnt + minalloc));

		if (!m_manualReset || !m_signal)
		{
			return SOCMNEVENT_INVALID_ID;
		}
	}

	m_manualReset[m_eventCnt] = manualReset;
	m_signal[m_eventCnt]      = initialState;
	eventId = m_eventCnt;
	m_eventCnt++;
	return eventId;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	HANDLE handle = 0;
	BYTE flags = 0;
	DWORD eventId = SOCMNEVENT_INVALID_ID;

	if ((!manualReset) && (!name))
	{
		handle = getEventPool()->popEvent();

		if (handle != 0)
		{
			if (initialState)
			{
				::SetEvent(handle);
			}

			flags |= SOCMNEVENT_TYPE_POOLED;
		}
	}
	else
	{
		handle = ::CreateEvent(NULL, manualReset, initialState, name);
	}

	if (handle != 0)
	{
		if ((m_eventCnt % 2) == 0)
		{
			m_pHandles = (HANDLE*)realloc(m_pHandles, (sizeof HANDLE) * (m_eventCnt + 2));
			m_pFlags = (BYTE*)realloc(m_pFlags, m_eventCnt + 2);

			if ((!m_pHandles) || (!m_pFlags))
			{
				return SOCMNEVENT_INVALID_ID;
			}
		}

		m_pHandles[m_eventCnt] = handle;
		m_pFlags[m_eventCnt] = flags;
		eventId = WAIT_OBJECT_0 + m_eventCnt;
		m_eventCnt++;
	}

	return eventId;
#endif
}

#ifdef SOOS_WINDOWS
DWORD SOCmnEvents::create(IN SECURITY_ATTRIBUTES* pSD, IN BOOL manualReset, IN BOOL initialState, IN LPCTSTR name)
{
	HANDLE handle = 0;
	BYTE flags = 0;
	DWORD eventId = SOCMNEVENT_INVALID_ID;
	handle = ::CreateEvent(pSD, manualReset, initialState, name);

	if (handle != 0)
	{
		if ((m_eventCnt % 2) == 0)
		{
			m_pHandles = (HANDLE*)realloc(m_pHandles, (sizeof HANDLE) * (m_eventCnt + 2));
			m_pFlags = (BYTE*)realloc(m_pFlags, m_eventCnt + 2);

			if ((!m_pHandles) || (!m_pFlags))
			{
				return SOCMNEVENT_INVALID_ID;
			}
		}

		m_pHandles[m_eventCnt] = handle;
		m_pFlags[m_eventCnt] = flags;
		eventId = WAIT_OBJECT_0 + m_eventCnt;
		m_eventCnt++;
	}

	return eventId;
}
#endif


#ifdef SOOS_LINUX
BOOL SOCmnEvent::signal(void)
{
	pthread_mutex_lock(&m_mutex);
	m_signal = TRUE;
	pthread_mutex_unlock(&m_mutex);

	if (m_manualReset)
	{
		pthread_cond_broadcast(&m_cond);
	}
	else
	{
		pthread_cond_signal(&m_cond);
	}

	return (TRUE);
}

BOOL SOCmnEvent::reset(void)
{
	pthread_mutex_lock(&m_mutex);
	m_signal = FALSE;
	pthread_mutex_unlock(&m_mutex);
	return (TRUE);
}

BOOL SOCmnEvent::waitFor(IN OPTIONAL DWORD timeout)
{
	BOOL retval = FALSE;
	pthread_mutex_lock(&m_mutex);

	if (!m_signal && timeout != 0)
	{
		if (timeout == INFINITE)
		{
			while (!m_signal)
			{
				pthread_cond_wait(&m_cond, &m_mutex);
			}
		}
		else // (timeout<INFINITE)
		{
			struct timeval now;
			struct timespec later;
			gettimeofday(&now, NULL);
#ifdef SODEVENV_CYGWIN

			if (timeout > 0x7FFFFFFF)
			{
				timeout = 0x7FFFFFFF;
			}

#endif
			later.tv_sec = now.tv_sec + timeout / 1000;
			later.tv_nsec = 1000 * now.tv_usec;
			later.tv_nsec += 1000000 * (timeout % 1000);

			if (later.tv_nsec  >  999999999)
			{
				later.tv_nsec -= 1000000000;
				later.tv_sec++;
			}

			while (!m_signal)
			{
				int res = pthread_cond_timedwait(&m_cond, &m_mutex, &later);

				if (res == ETIMEDOUT || time(NULL) < now.tv_sec)
				{
					break;
				}

				/* other values: res == EINTR or res == 0 */
			}
		}
	}

	if (m_signal)
	{
		retval = TRUE;

		if (!m_manualReset)
		{
			m_signal = FALSE;
		}
	}

	pthread_mutex_unlock(&m_mutex);
	return retval;
}

BOOL SOCmnEvent::isInit(void)
{
	return (m_isInit);
}

BOOL SOCmnEvents::signal(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		pthread_mutex_lock(&m_mutex);
		m_signal[eventId] = TRUE;
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_broadcast(&m_cond);
		return TRUE;
	}

	return FALSE;
}

BOOL SOCmnEvents::reset(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		pthread_mutex_lock(&m_mutex);
		m_signal[eventId] = FALSE;
		pthread_mutex_unlock(&m_mutex);
		return TRUE;
	}

	return FALSE;
}

DWORD SOCmnEvents::waitForAll(IN OPTIONAL BOOL waitAll, IN OPTIONAL DWORD timeout)
{
	DWORD retval;
	DWORD i;
#define REEVALUATE_WAIT_CONDITION {                             \
        retval = SOCMNEVENT_INVALID_ID;                         \
        if (waitAll) {                                          \
            for (i=0; i<m_eventCnt; i++)                        \
                if (!m_signal[i])                               \
                    break;                                      \
            if (i == m_eventCnt)                                \
                retval = 0;                                     \
        } else {                                                \
            for (i=0; i<m_eventCnt; i++)                        \
                if (m_signal[i])                                \
                    break;                                      \
            if (i < m_eventCnt)                                 \
                retval = i;                                     \
        } /************************************* END OF MACRO */}
	pthread_mutex_lock(&m_mutex);
	REEVALUATE_WAIT_CONDITION;

	if (retval == SOCMNEVENT_INVALID_ID && timeout != 0)
	{
		if (timeout == INFINITE)
		{
			while (retval == SOCMNEVENT_INVALID_ID)
			{
				pthread_cond_wait(&m_cond, &m_mutex);
				REEVALUATE_WAIT_CONDITION;
			}
		}
		else // (timeout<INFINITE)
		{
			struct timeval now;
			struct timespec later;
			gettimeofday(&now, NULL);
#ifdef SODEVENV_CYGWIN

			if (timeout > 0x7FFFFFFF)
			{
				timeout = 0x7FFFFFFF;
			}

#endif
			later.tv_sec = now.tv_sec + timeout / 1000;
			later.tv_nsec = 1000 * now.tv_usec;
			later.tv_nsec += 1000000 * (timeout % 1000);

			if (later.tv_nsec  >  999999999)
			{
				later.tv_nsec -= 1000000000;
				later.tv_sec++;
			}

			while (retval == SOCMNEVENT_INVALID_ID)
			{
				int res = pthread_cond_timedwait(&m_cond, &m_mutex, &later);

				if (res == ETIMEDOUT || time(NULL) < now.tv_sec)
				{
					break;
				}

				/* other values: res == EINTR or res == 0 */
				REEVALUATE_WAIT_CONDITION;
			}
		}
	}

	if (retval != SOCMNEVENT_INVALID_ID)
	{
		if (waitAll)
		{
			for (i = 0; i < m_eventCnt; i++)
			{
				_ASSERTION(m_signal[i], "SOCmnEvents::waitForAll error 1");

				if (!m_manualReset[i])
				{
					m_signal[i] = FALSE;
				}
			}
		}
		else
		{
			_ASSERTION(m_signal[retval], "SOCmnEvents::waitForAll error 2");

			if (!m_manualReset[retval])
			{
				m_signal[retval] = FALSE;
			}
		}
	}

	pthread_mutex_unlock(&m_mutex);
	return retval;
}

BOOL SOCmnEvents::waitFor(DWORD eventId, IN OPTIONAL DWORD timeout)
{
	BOOL retval = FALSE;

	if (eventId < m_eventCnt)
	{
		pthread_mutex_lock(&m_mutex);

		if (!m_signal[eventId] && timeout != 0)
		{
			if (timeout == INFINITE)
			{
				while (!m_signal[eventId])
				{
					pthread_cond_wait(&m_cond, &m_mutex);
				}
			}
			else // (timeout<INFINITE)
			{
				struct timeval now;
				struct timespec later;
				gettimeofday(&now, NULL);
#ifdef SODEVENV_CYGWIN

				if (timeout > 0x7FFFFFFF)
				{
					timeout = 0x7FFFFFFF;
				}

#endif
				later.tv_sec = now.tv_sec + timeout / 1000;
				later.tv_nsec = 1000 * now.tv_usec;
				later.tv_nsec += 1000000 * (timeout % 1000);

				if (later.tv_nsec  >  999999999)
				{
					later.tv_nsec -= 1000000000;
					later.tv_sec++;
				}

				while (!m_signal[eventId])
				{
					int res = pthread_cond_timedwait(&m_cond, &m_mutex, &later);

					if (res == ETIMEDOUT || time(NULL) < now.tv_sec)
					{
						break;
					}

					/* other values: res == EINTR or res == 0 */
				}
			}
		}

		if (m_signal[eventId])
		{
			retval = TRUE;

			if (!m_manualReset[eventId])
			{
				m_signal[eventId] = FALSE;
			}
		}

		pthread_mutex_unlock(&m_mutex);
	}

	return retval;
}

BOOL SOCmnEvents::isInit(DWORD eventId)
{
	return (eventId < m_eventCnt);
}

#endif // SOOS_LINUX



SOCmnEventPointer::SOCmnEventPointer(void)
{
	m_id = SOCMNEVENT_INVALID_ID;
	m_pEvent = NULL;
}


SOCmnEventPointer::SOCmnEventPointer(SOCmnEvent* pEvent)
{
	m_id = SOCMNEVENT_INVALID_ID;
	m_pEvent = pEvent;
}

SOCmnEventPointer::SOCmnEventPointer(SOCmnEvents* pEvents, DWORD eventId)
{
	m_id = eventId;

	if (eventId != SOCMNEVENT_INVALID_ID)
	{
		m_pEvents = pEvents;
	}
	else
	{
		m_pEvent = NULL;
	}
}

SOCmnEventPointer::SOCmnEventPointer(IN const SOCmnEventPointer& eP)
{
	m_id = eP.m_id;
	m_pEvent = eP.m_pEvent;
}

const SOCmnEventPointer& SOCmnEventPointer::operator=(IN SOCmnEventPointer& eP)
{
	m_id = eP.m_id;
	m_pEvent = eP.m_pEvent;
	return *this;
}

SOCmnEventPointer::~SOCmnEventPointer(void)
{}

void SOCmnEventPointer::set(IN SOCmnEventPointer eP)
{
	m_id = eP.m_id;
	m_pEvent = eP.m_pEvent;
}

BOOL SOCmnEventPointer::signal(void)
{
	if (m_pEvent != NULL)
	{
		if (m_id != SOCMNEVENT_INVALID_ID)
		{
			return m_pEvents->signal(m_id);
		}
		else
		{
			return m_pEvent->signal();
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL SOCmnEventPointer::reset(void)
{
	if (m_pEvent != NULL)
	{
		if (m_id != SOCMNEVENT_INVALID_ID)
		{
			return m_pEvents->reset(m_id);
		}
		else
		{
			return m_pEvent->reset();
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL SOCmnEventPointer::waitFor(IN OPTIONAL DWORD timeout)
{
	if (m_pEvent != NULL)
	{
		if (m_id != SOCMNEVENT_INVALID_ID)
		{
			return m_pEvents->waitFor(m_id, timeout);
		}
		else
		{
			return m_pEvent->waitFor(timeout);
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL SOCmnEventPointer::isInit(void)
{
	if (m_pEvent != NULL)
	{
		if (m_id != SOCMNEVENT_INVALID_ID)
		{
			return m_pEvents->isInit(m_id);
		}
		else
		{
			return m_pEvent->isInit();
		}
	}
	else
	{
		return FALSE;
	}
}

