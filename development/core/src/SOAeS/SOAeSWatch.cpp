#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_WATCH

#include <SOAeSWatch.h>


//-----------------------------------------------------------------------------
// SOAeSWatchHandler                                                          |
//-----------------------------------------------------------------------------

SOAeSWatchHandler::SOAeSWatchHandler(void)
{
	memset(&m_statistics, 0, sizeof(SOAeSWatchHandlerStatisticsData));
}


//-----------------------------------------------------------------------------
// getObject
//
// - get the toolkit object from it's description elements
//
// returns:
//		NULL - no valid object
//    pointer to the object
//
SOCmnObject* SOAeSWatchHandler::getObject(
	IN LPCTSTR objSpace, // object space
	IN LPCTSTR objType)  // object type
{
	DWORD type = SOSrvWatch::convertObjectType(objType);

	if (type == SOCMNOBJECT_TYPE_ROOT)
	{
		SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();

		if (_tcscmp(objSpace, SOWATCH_SPACE_AE_OBJECT) == 0)
		{
			return entry->getObjectRoot();
		}
		else if (_tcscmp(objSpace, SOWATCH_SPACE_AE_AREASPACE) == 0)
		{
			return entry->getAreaSpaceRoot();
		}
		else if (_tcscmp(objSpace, SOWATCH_SPACE_AE_EVENTSPACE) == 0)
		{
			return entry->getEventSpaceRoot();
		}
	}

	return NULL;
} // getObject


//-----------------------------------------------------------------------------
// getRootObjects
//
// - get root objects
//
void SOAeSWatchHandler::getRootObjects(
	OUT SOCmnList<SOCmnObject> objList) // object list
{
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOSrvObjectRoot> objRoot =  entry->getObjectRoot();
	SOCmnPointer<SOAeSAreaSpaceRoot> arRoot =  entry->getAreaSpaceRoot();
	SOCmnPointer<SOAeSEventSpaceRoot> evRoot =  entry->getEventSpaceRoot();
	objList.add(objRoot);
	objList.add(arRoot);
	objList.add(evRoot);
} // getRootObjects


//-----------------------------------------------------------------------------
// getStatistics
//
// - get the statistics
//
// returns:
//		S_OK   - got statistics
//		E_FAIL - error
//
HRESULT SOAeSWatchHandler::getStatistics(
	SOCmnStringEx& szXML)
{
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRSIMPLEEVENTS, m_statistics.simpleEvent);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRTRACKINGEVENTS, m_statistics.trackingEvent);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRCONDITIONALEVENTS, m_statistics.condEvent);
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->getObjectCountStatistics(szXML);
	memset(&m_statistics, 0, sizeof(SOAeSWatchHandlerStatisticsData));
	return S_OK;
} // getStatistics


void SOAeSWatchHandler::doEventStatistics(IN SOAeSEvent* event)
{
	SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
	SOCmnList<SOSrvWatch> watchList(entry->getWatchList());
	SOCmnListPosition pos = watchList.getStartPosition();
	SOSrvWatch* watch;
	SOCmnPointer<SOAeSWatchHandler> AEHandler;

	while (pos)
	{
		watch = watchList.getNext(pos);
		AEHandler = (SOAeSWatchHandler*)watch->getAEHandler();

		if (AEHandler.isNotNull())
		{
			SOCmnPointer<SOAeSCategory> cat = event->getCategory();

			if (cat.isNotNull())
			{
				SOCmnSingleLock<SOCmnSync> lock(watch->getStatisticsLock());

				switch (cat->getEventType())
				{
				case OPC_SIMPLE_EVENT:
					AEHandler->m_statistics.simpleEvent++;
					break;

				case OPC_TRACKING_EVENT:
					AEHandler->m_statistics.trackingEvent++;
					break;

				case OPC_CONDITION_EVENT:
					AEHandler->m_statistics.condEvent++;
					break;
				}
			}
		}
	}
}

#endif // SOFEATURE_WATCH

#endif // SOAES
