#ifndef _SOAESWATCH_H_
#define _SOAESWATCH_H_

#ifdef SOFEATURE_WATCH

#include "SOSrvWatch.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOAeSWatchHandler                                                          |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSWatchHandler : public SOSrvWatchHandler
{
public:
	SOAeSWatchHandler(void);

	static void doEventStatistics(IN SOAeSEvent* event);

protected:
	struct SOAeSWatchHandlerStatisticsData
	{
		// event notifications
		ULONG simpleEvent;
		ULONG trackingEvent;
		ULONG condEvent;
	} m_statistics;

	virtual SOCmnObject* getObject(IN LPCTSTR objSpace, IN LPCTSTR objType);
	virtual void getRootObjects(OUT SOCmnList<SOCmnObject> objList);

	virtual HRESULT getStatistics(IN SOCmnStringEx& szXML);
};

#pragma pack(pop)

#endif // SOFEATURE_WATCH

#endif
