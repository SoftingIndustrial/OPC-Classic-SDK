#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef TBC_OS_LINUX
#include "pthread.h"
#ifndef INFINITE
#define INFINITE ((unsigned long)0xFFFFFFFF)
#endif
#endif

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

//-----------------------------------------------------------------------------
// Mutex
//
class TBC_EXPORT Mutex
{

public:
	Mutex();
	virtual ~Mutex();

	// gain access to the mutex object
	BOOL lock(unsigned long timeout = INFINITE);

	// release access to the mutex object
	BOOL unlock(void);

protected:

#ifdef TBC_OS_LINUX
	pthread_mutex_t m_mutex;
#endif  //  end TBC_OS_LINUX

#ifdef TBC_OS_WINDOWS
	HANDLE m_mutex; // handle of system mutex
#endif  //  end TBC_OS_WINDOWS

}; // SOCmnMutex


#pragma pack(pop)
#endif
