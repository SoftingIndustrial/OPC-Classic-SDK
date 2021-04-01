#ifndef _SODASWATCH_H_
#define _SODASWATCH_H_

#ifdef SOFEATURE_WATCH

#include "SOSrvWatch.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaSWatchHandler                                                          |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSWatchHandler : public SOSrvWatchHandler
{
public:
	SODaSWatchHandler(void);

	static void doRequestStatistics(IN SODaSRequest* req, IN DWORD completionSpan);

protected:
	struct SODaSWatchHandlerStatisticsData
	{
		// successful I/O requests
		ULONG asyncCacheRead;
		ULONG asyncDeviceRead;
		ULONG asyncWrite;
		ULONG syncCacheRead;
		ULONG syncDeviceRead;
		ULONG syncWrite;
		ULONG cyclicRead;
		ULONG reportRead;

		// failed I/O requests
		ULONG failedReadOrBadQuality;
		ULONG failedWrite;

		// times in milliseconds:
		// complete time in device operation
		ULONGLONG timeRead;
		ULONGLONG timeWrite;
		// average time in device operation
		ULONG averageTimeRead;
		ULONG averageTimeWrite;
		// maximum time in device operation
		ULONG maxTimeRead;
		ULONG maxTimeWrite;
	} m_statistics;


	virtual SOCmnObject* getObject(IN LPCTSTR objSpace, IN LPCTSTR objType);
	virtual void getRootObjects(OUT SOCmnList<SOCmnObject> objList);

	virtual BOOL hasCacheObject(IN SOCmnObject* tag, IN SOCmnObject* cache, OUT BOOL& uniqueCache);
	virtual BOOL setCollectCacheChanges(IN SOCmnObject* tag, IN BOOL doCollect);

	virtual SOCmnObject* getNameSpaceObjectByItemID(IN LPCTSTR itemID);

	virtual HRESULT getStatistics(SOCmnStringEx& szXML);
};

#pragma pack(pop)

#endif // SOFEATURE_WATCH

#endif
