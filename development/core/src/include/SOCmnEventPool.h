#ifndef _SOCMNEVENTPOOL_H_
#define _SOCMNEVENTPOOL_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#ifdef SOOS_WINDOWS

#pragma pack(push, 4)



class SOCMN_EXPORT SOCmnEventPool
{
public:
	SOCmnEventPool();
	~SOCmnEventPool();

	// event pool
	HANDLE popEvent(void);
	void pushEvent(HANDLE event);
	void freeAllEvents(void);

protected:
	SOCmnList<void> m_eventPool;            // event pool
}; // SOCmnEventPool


SOCMN_EXPORT SOCmnEventPool* getEventPool(void);
void createEventPool(void);
void destroyEventPool(void);
HANDLE popEvent(void);
void pushEvent(HANDLE event);

#pragma pack(pop)

#endif // SOOS_WINDOWS

#endif
