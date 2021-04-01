#include "OSCompat.h"
#include "PosixEvents.h"

/**
 * @brief Constructor
 */
PosixEvent::PosixEvent()
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
	m_manualReset = false;
	m_signal = false;
	m_isInit = false;
}

/**
 * @brief Destructor
 */
PosixEvent::~PosixEvent()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
}

/**
 * @brief Some initializations
 * @param manualReset
 * @param initialState
 * @return Always returns true
 */
bool PosixEvent::create(bool manualReset, bool initialState)
{
	m_manualReset = manualReset;
	m_signal = initialState;
	m_isInit = true;
	return m_isInit;
}

/**
 * @brief Emits the signal
 * @return Always returns true
 */
bool PosixEvent::signal()
{
	pthread_mutex_lock(&m_mutex);
	m_signal = true;
	pthread_mutex_unlock(&m_mutex);

	if (m_manualReset)
	{
		pthread_cond_broadcast(&m_cond);
	}
	else
	{
		pthread_cond_signal(&m_cond);
	}

	return true;
}

/**
 * @brief Resets the internal signal flag
 * @return Always returns true
 */
bool PosixEvent::reset()
{
	pthread_mutex_lock(&m_mutex);
	m_signal = false;
	pthread_mutex_unlock(&m_mutex);
	return true;
}

/**
 * @brief Waits timeout interval
 * @param timeout Interval to wait
 * @return True if signalled
 */
bool PosixEvent::waitFor(DWORD timeout)
{
	bool retval = false;
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
			}
		}
	}

	if (m_signal)
	{
		retval = true;

		if (!m_manualReset)
		{
			m_signal = false;
		}
	}

	pthread_mutex_unlock(&m_mutex);
	return retval;
}

/**
 * @brief Waits timeout interval
 * @return True is initialized
 */
bool PosixEvent::isInit()
{
	return m_isInit;
}

/**
 * @brief Constructor
 */
PosixEvents::PosixEvents()
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
	m_eventCnt    = 0;
	m_manualReset = NULL;
	m_signal      = NULL;
}

/**
 * @brief Destructor
 */
PosixEvents::~PosixEvents()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
	free(m_manualReset);
	free(m_signal);
}

/**
 * @brief Creates an event
 * @param manualReset Manual reset
 * @param initialState Initial state
 * @return Id of created event
 */
DWORD PosixEvents::create(bool manualReset, bool initialState)
{
	const DWORD minalloc = 16;
	DWORD eventId = SOCMNEVENT_INVALID_ID;

	if ((m_eventCnt % minalloc) == 0)
	{
		m_manualReset = (bool*)realloc(m_manualReset, sizeof(bool) * (m_eventCnt + minalloc));
		m_signal = (bool*)realloc(m_signal, sizeof(bool) * (m_eventCnt + minalloc));

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
}

/**
 * @brief Emits signal for the specific event
 * @param eventId
 * @return True if valid event id
 */
bool PosixEvents::signal(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		// Setting internal flag to true
		pthread_mutex_lock(&m_mutex);
		m_signal[eventId] = true;
		pthread_mutex_unlock(&m_mutex);
		// Emit signal
		pthread_cond_broadcast(&m_cond);
		return true;
	}

	return false;
}

/**
 * @brief Resets internal flag for the specific event
 * @param eventId
 * @return True if valid event id
 */
bool PosixEvents::reset(DWORD eventId)
{
	if (eventId < m_eventCnt)
	{
		// Setting internal flag to true
		pthread_mutex_lock(&m_mutex);
		m_signal[eventId] = false;
		pthread_mutex_unlock(&m_mutex);
		return true;
	}

	return false;
}

/**
 * @brief Waits for all events
 * @param waitAll
 * @param timeout
 * @return Event id for the signalled event
 */
DWORD PosixEvents::waitForAll(bool waitAll, DWORD timeout)
{
	DWORD retval;
	DWORD i;
#define REEVALUATE_WAIT_CONDITION                           \
    {                                                       \
    retval = SOCMNEVENT_INVALID_ID;                         \
    if (waitAll)                                            \
    {                                                       \
        for (i=0; i<m_eventCnt; i++)                        \
    {                                                       \
            if (!m_signal[i])                               \
            {                                               \
                break;                                      \
            }                                               \
    }                                                       \
        if (i == m_eventCnt)                                \
        {                                                   \
            retval = 0;                                     \
        }                                                   \
    }                                                       \
    else                                                    \
    {                                                       \
        for (i=0; i<m_eventCnt; i++)                        \
        {                                                   \
            if (m_signal[i])                                \
            {                                               \
                break;                                      \
            }                                               \
        }                                                   \
        if (i < m_eventCnt)                                 \
        {                                                   \
            retval = i;                                     \
        }                                                   \
    } /************************************* END OF MACRO */}
	pthread_mutex_lock(&m_mutex);
	REEVALUATE_WAIT_CONDITION;

	if (retval == SOCMNEVENT_INVALID_ID && timeout != 0)
	{
		if (timeout == INFINITE)
		{
			while (retval == SOCMNEVENT_INVALID_ID)
			{
				pthread_cond_wait(
					&m_cond,
					&m_mutex);
				REEVALUATE_WAIT_CONDITION;
			}
		}
		else // (timeout<INFINITE)
		{
			struct timeval now;
			struct timespec later;
			gettimeofday(
				&now,
				NULL);
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
				int res = pthread_cond_timedwait(
							  &m_cond,
							  &m_mutex,
							  &later);

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
				if (!m_manualReset[i])
				{
					m_signal[i] = false;
				}
			}
		}
		else
		{
			if (!m_manualReset[retval])
			{
				m_signal[retval] = false;
			}
		}
	}

	pthread_mutex_unlock(&m_mutex);
	return retval;
}

/**
 * @brief Waits for a specific event
 * @param eventId Specific event id
 * @param timeout Timeout interval
 * @return True if event was signalled
 */
bool PosixEvents::waitFor(DWORD eventId, DWORD timeout)
{
	bool retval = false;

	if (eventId < m_eventCnt)
	{
		pthread_mutex_lock(&m_mutex);

		if (!m_signal[eventId] && timeout != 0)
		{
			if (timeout == INFINITE)
			{
				while (!m_signal[eventId])
				{
					pthread_cond_wait(
						&m_cond,
						&m_mutex);
				}
			}
			else // (timeout<INFINITE)
			{
				struct timeval now;
				struct timespec later;
				gettimeofday(&now, NULL);
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
					int res = pthread_cond_timedwait(
								  &m_cond,
								  &m_mutex,
								  &later);

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

/**
 * @brief isInit
 * @param eventId
 * @return True if valid event id
 */
bool PosixEvents::isInit(DWORD eventId)
{
	return (eventId < m_eventCnt);
}

