#include "OSCompat.h"
#include "Mutex.h"

//-----------------------------------------------------------------------------
// Mutex - constructor
//
Mutex::Mutex()
{
#ifdef TBC_OS_WINDOWS
	m_mutex = CreateMutex(NULL, FALSE, NULL);
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
	pthread_mutexattr_t attr;
#ifdef SODEVENV_CYGWIN
	memset(&attr, 0, sizeof(attr));
	memset(&m_mutex, 0, sizeof(m_mutex));
#endif
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
}   //  end constructor

Mutex::~Mutex()
{
#ifdef TBC_OS_WINDOWS

	if (m_mutex != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_mutex);
	}   //  end if

#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
	pthread_mutex_destroy(&m_mutex);
#endif
}   //  end destructor


//-----------------------------------------------------------------------------
// lock
//
// - gain access to the synchronisation object
//
// returns:
//		TRUE  - got access to the object
//		FALSE - no access to the object
//
BOOL Mutex::lock(IN unsigned long timeout)
{
#ifdef TBC_OS_WINDOWS
	BOOL res;
	res = ::WaitForSingleObject(m_mutex, timeout);
	return (res == WAIT_OBJECT_0);
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
	BOOL res;

	if (timeout == INFINITE)
	{
		pthread_mutex_lock(&m_mutex);
		res = TRUE;
	}
	else if (timeout == 0)
	{
		res = !pthread_mutex_trylock(&m_mutex);
	}
	else
	{
#ifndef SODEVENV_CYGWIN
		struct timespec ts;
		struct timeval  tv;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + timeout / 1000;
		ts.tv_nsec = 1000 * tv.tv_usec;
		ts.tv_nsec += 1000000 * (timeout % 1000);

		if (ts.tv_nsec  >  999999999)
		{
			ts.tv_nsec -= 1000000000;
			ts.tv_sec  ++;
		}

		res = !pthread_mutex_timedlock(&m_mutex, &ts);
#else
		res = !pthread_mutex_lock(&m_mutex);
#endif
	}

	return res;
#endif
}   //  end lock


//-----------------------------------------------------------------------------
// unlock
//
// - release access to the synchronisation object
//
// returns:
//		TRUE  - released access to the object
//		FALSE - not owned the object
//
BOOL Mutex::unlock(void)
{
#ifdef TBC_OS_WINDOWS
	return ReleaseMutex(m_mutex);
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
	return !pthread_mutex_unlock(&m_mutex);
#endif
}   //  unlock
