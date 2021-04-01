#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnEventPool.h>

#ifdef SOOS_WINDOWS
SOCmnEventPool* g_eventPool = NULL;

SOCmnEventPool* getEventPool(void)
{
	_ASSERTION((g_eventPool != NULL), "Before calling getEventPool the pool must be created by createEventPool!");
	return g_eventPool;
}

void createEventPool(void)
{
	if (!g_eventPool)
		SOCMN_ALLOCATE_OBJECT(g_eventPool, SOCmnEventPool())
	}

void destroyEventPool(void)
{
	if (g_eventPool)
	{
		SOCmnEventPool* eventPool = g_eventPool;
		g_eventPool = NULL;
		SOCMN_FREE_OBJECT(eventPool)
	}
}

HANDLE popEvent(void)
{
	if (g_eventPool)
	{
		return g_eventPool->popEvent();
	}
	else
	{
		return ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}
}

void pushEvent(HANDLE event)
{
	if (g_eventPool)
	{
		g_eventPool->pushEvent(event);
	}
	else
	{
		::CloseHandle(event);
	}
}


//-----------------------------------------------------------------------------
// SOCmnEventPool                                                             |
//-----------------------------------------------------------------------------

SOCmnEventPool::SOCmnEventPool()
{
	m_eventPool.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_SYNCED);
}

SOCmnEventPool::~SOCmnEventPool()
{
	freeAllEvents();
	m_eventPool.destroy();
}


HANDLE SOCmnEventPool::popEvent(void)
{
	HANDLE event;

	if (!m_eventPool)
	{
		return ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	m_eventPool.lock();

	if (!m_eventPool.isEmpty())
	{
		event = (HANDLE)m_eventPool.getHead();
		m_eventPool.removeHead();
	}
	else
	{
		event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	m_eventPool.unlock();
	return event;
}

void SOCmnEventPool::pushEvent(HANDLE event)
{
	if (!m_eventPool)
	{
		CloseHandle(event);
		return;
	}

	::ResetEvent(event);
	m_eventPool.lock();
	m_eventPool.add((void*)event);
	m_eventPool.unlock();
}

void SOCmnEventPool::freeAllEvents(void)
{
	HANDLE event;

	if (!m_eventPool)
	{
		return;
	}

	m_eventPool.lock();

	while (!m_eventPool.isEmpty())
	{
		event = (HANDLE)m_eventPool.getHead();
		m_eventPool.removeHead();
		CloseHandle(event);
	}

	m_eventPool.unlock();
}

#endif
