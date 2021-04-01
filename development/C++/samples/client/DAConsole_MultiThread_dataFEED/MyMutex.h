#include "stdafx.h"
#include "Mutex.h"

#pragma once

#ifndef _MY_MUTEX_H_

#define _MY_MUTEX_H_

class MyMutex : public Mutex {
public:
	virtual BOOL aLock(unsigned long timeout = INFINITE)
	{
		if (lock(timeout))
		{
			m_ownerThreadId = GetCurrentThreadId();
			return TRUE;
		}
		return FALSE;
	}

	virtual BOOL aUnlock()
	{
		if (unlock())
		{
			m_ownerThreadId = 0;
			return TRUE;
		}
		return FALSE;
	}
private:
	DWORD m_ownerThreadId;
};

#endif // _MY_MUTEX_H_