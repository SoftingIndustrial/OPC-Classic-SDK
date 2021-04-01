#ifndef _POSIX_EVENT_H_
#define _POSIX_EVENT_H_

#ifndef INFINITE
#define INFINITE ((DWORD) -1)
#endif

#define SOCMNEVENT_INVALID_ID 0xFFFFFFFF

/**
 * @brief Posix Event
 */
class PosixEvent
{
public:

	/*! @brief Constructor */
	PosixEvent();

	/*! @brief Destructor */
	~PosixEvent();

	/*! @brief Initialization */
	bool create(bool manualReset = false, bool initialState = false);

	/*! @brief Emit signal */
	bool signal();

	/*! @brief Resets internal flag */
	bool reset();

	/*! @brief Waits specific timeout interval */
	bool waitFor(DWORD timeout = INFINITE);

	/*! @brief True if initialized */
	bool isInit();

protected:

	/*! @brief Posix mutex */
	pthread_mutex_t m_mutex;

	/*! @brief Posix condition */
	pthread_cond_t  m_cond;

	/*! @brief Manual reset */
	bool m_manualReset;

	/*! @brief Internal flag, true if signalled */
	bool m_signal;

	/*! @brief Flag for being initialized */
	bool m_isInit;

};

/**
 * @brief Posix Events
 */
class PosixEvents
{
public:

	/*! @brief Constructor */
	PosixEvents();

	/*! @brief Destructor */
	~PosixEvents();

	/*! @brief Creates an event */
	DWORD create(bool manualReset = false, bool initialState = false);

	/*! @brief Emits signal for specific event */
	bool signal(DWORD eventId);

	/*! @brief Resets internal flag for specific event */
	bool reset(DWORD eventId);

	/*! @brief Waits for all events */
	DWORD waitForAll(bool waitAll = false, DWORD timeout = INFINITE);

	/*! @brief Waits for specific a specific event */
	bool waitFor(DWORD eventId, DWORD timeout);

	/*! @brief True specific eventId is valid */
	bool isInit(DWORD eventId);

protected:

	/*! @brief Posix mutex */
	pthread_mutex_t m_mutex;

	/*! @brief Posix condition */
	pthread_cond_t  m_cond;

	/*! @brief Event count */
	DWORD m_eventCnt;

	/*! @brief Manual resets */
	bool* m_manualReset;

	/*! @brief Internal flags */
	bool* m_signal;

};

#endif

