//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSWatch.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server monitoring classes                               |
//                - SODaSWatch: DA watch handler object                       |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_WATCH

#include <SODaSWatch.h>


//-----------------------------------------------------------------------------
// SODaSWatchHandler                                                          |
//-----------------------------------------------------------------------------

SODaSWatchHandler::SODaSWatchHandler(void)
{
	memset(&m_statistics, 0, sizeof(SODaSWatchHandlerStatisticsData));
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
SOCmnObject* SODaSWatchHandler::getObject(
	IN LPCTSTR objSpace, // object space
	IN LPCTSTR objType)  // object type
{
	DWORD type = SOSrvWatch::convertObjectType(objType);

	if (type == SOCMNOBJECT_TYPE_ROOT)
	{
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();

		if (_tcscmp(objSpace, SOWATCH_SPACE_DA_OBJECT) == 0)
		{
			return entry->getObjectRoot();
		}
		else if (_tcscmp(objSpace, SOWATCH_SPACE_DA_NAMESPACE) == 0)
		{
			return entry->getNameSpaceRoot();
		}
	}

	return NULL;
} // getObject


//-----------------------------------------------------------------------------
// getNameSpaceObjectByItemID
//
// - get a DA namespace object by it's item id
//
// returns:
//		NULL - no valid object
//    pointer to the object
//
SOCmnObject* SODaSWatchHandler::getNameSpaceObjectByItemID(
	IN LPCTSTR itemID) // item id
{
	SODaSTag* tag;
	tag = SODaSNameSpaceRoot::getTag(itemID);

	if (tag != NULL)
	{
		return tag;
	}

	return NULL;
} // getNameSpaceObjectByItemID


//-----------------------------------------------------------------------------
// getRootObjects
//
// - get root objects
//
void SODaSWatchHandler::getRootObjects(
	OUT SOCmnList<SOCmnObject> objList) // object list
{
	SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> objRoot =  entry->getObjectRoot();
	SOCmnPointer<SODaSNameSpaceRoot> nsRoot =  entry->getNameSpaceRoot();
	objList.add(objRoot);
	objList.add(nsRoot);
} // getRootObjects


//-----------------------------------------------------------------------------
// hasCacheObject (OPC DA)
//
// - checks if the item tag object has the cache object
//
// returns:
//		TRUE  - tag has cache object
//		FALSE - tag has not cache object
//
BOOL SODaSWatchHandler::hasCacheObject(
	IN SOCmnObject* tag,
	IN SOCmnObject* cache,
	OUT BOOL& uniqueCache)
{
	SOCmnPointer<SODaSCache> tagCache;
	tagCache = ((SODaSItemTag*)tag)->getCache();

	if (!tagCache)
	{
		return FALSE;
	}

	if (((SOCmnObject*)(SODaSCache*)cache) == ((SODaSCache*)tagCache))
	{
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SODaSNameSpaceRoot> root = entry->getNameSpaceRoot();
		uniqueCache = !root->getUseTagLinks();
		return TRUE;
	}

	return FALSE;
} // hasCacheObject


//-----------------------------------------------------------------------------
// setCollectCacheChanges
//
// - set if to collect changes for the cache of a item tag object
//
// returns:
//		TRUE  - set
//		FALSE - error
//
BOOL SODaSWatchHandler::setCollectCacheChanges(
	IN SOCmnObject* tag, // item tag
	IN BOOL doCollect)   // do collect changes
{
	SOCmnPointer<SODaSCache> tagCache;
	tagCache = ((SODaSItemTag*)tag)->getCache();

	if (!tagCache)
	{
		return TRUE;
	}

	if (doCollect)
	{
		tagCache->setCollectChanges(SOCMNOBJECT_FLAG_COLLECT_CHANGES | SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES);
	}
	else
	{
		tagCache->setCollectChanges(0);
	}

	return TRUE;
} // setCollectCacheChanges


//-----------------------------------------------------------------------------
// getStatistics
//
// - get the statistics
//
// returns:
//		S_OK   - got statistics
//		E_FAIL - error
//
HRESULT SODaSWatchHandler::getStatistics(
	SOCmnStringEx& szXML)
{
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRCYCREAD, m_statistics.cyclicRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRCHANGEREPORT, m_statistics.reportRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRACACHEREAD, m_statistics.asyncCacheRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRADEVREAD, m_statistics.asyncDeviceRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRSDEVREAD, m_statistics.syncDeviceRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRSCACHEREAD, m_statistics.syncCacheRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRAWRITE, m_statistics.asyncWrite);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRSWRITE, m_statistics.syncWrite);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRREADFAILED, m_statistics.failedReadOrBadQuality);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRWRITEFAILED, m_statistics.failedWrite);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_MAXREADTIME, m_statistics.maxTimeRead);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_MAXWRITETIME, m_statistics.maxTimeWrite);
	LONGLONG reads;
	LONGLONG writes;
	ULONG avReadTime = 0;
	ULONG avWriteTime = 0;
	reads = m_statistics.asyncDeviceRead + m_statistics.syncDeviceRead  + m_statistics.cyclicRead;

	if (reads)
	{
		avReadTime = (ULONG)(m_statistics.timeRead / reads);
	}

	writes = m_statistics.asyncWrite + m_statistics.syncWrite;

	if (writes)
	{
		avWriteTime = (ULONG)(m_statistics.timeWrite / writes);
	}

	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_AVREADTIME, avReadTime);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_AVWRITETIME, avWriteTime);
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->getObjectCountStatistics(szXML);
	memset(&m_statistics, 0, sizeof(SODaSWatchHandlerStatisticsData));
	return S_OK;
} // getStatistics


void SODaSWatchHandler::doRequestStatistics(IN SODaSRequest* req, IN DWORD completionSpan)
{
	SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
	SOCmnList<SOSrvWatch> watchList(entry->getWatchList());
	SOCmnListPosition pos = watchList.getStartPosition();
	SOSrvWatch* watch;
	SOCmnPointer<SODaSWatchHandler> DAHandler;

	while (pos)
	{
		watch = watchList.getNext(pos);
		DAHandler = (SODaSWatchHandler*)watch->getDAHandler();

		if (DAHandler.isNotNull())
		{
			SOCmnSingleLock<SOCmnSync> lock(watch->getStatisticsLock());
			HRESULT reqRes = req->getResult();
			SODaSRequest::requestOperation operation = req->getOperation();
			BOOL deviceRead = FALSE;

			if ((SUCCEEDED(reqRes)) && (operation == SODaSRequest::read))
			{
				if (req->getType() != SODaSRequest::report)
				{
					SOCmnPointer<SODaSCache> cache = req->getCache();

					if (cache.isNotNull())
					{
						if ((cache->getQuality(req) & OPC_QUALITY_GOOD) != OPC_QUALITY_GOOD)
						{
							reqRes = E_FAIL;
						}
					}
					else
					{
						reqRes = E_FAIL;
					}
				}
				else if ((req->getReportQuality() & OPC_QUALITY_GOOD) != OPC_QUALITY_GOOD)
				{
					reqRes = E_FAIL;
				}
			}

			if (SUCCEEDED(reqRes))
			{
				switch (req->getType())
				{
				case SODaSRequest::cyclic:
					DAHandler->m_statistics.cyclicRead++;
					deviceRead = TRUE;
					break;

				case SODaSRequest::report:
					DAHandler->m_statistics.reportRead++;
					break;

				case SODaSRequest::asyncDevice:
				case SODaSRequest::asyncMaxAge:
					if (operation == SODaSRequest::read)
					{
						DAHandler->m_statistics.asyncDeviceRead++;
						deviceRead = TRUE;
					}
					else
					{
						DAHandler->m_statistics.asyncWrite++;
					}

					break;

				case SODaSRequest::asyncCache:
					DAHandler->m_statistics.asyncCacheRead++;
					break;

				case SODaSRequest::serverDevice:
				case SODaSRequest::serverMaxAge:
				case SODaSRequest::syncDevice:
				case SODaSRequest::syncMaxAge:
					if (operation == SODaSRequest::read)
					{
						DAHandler->m_statistics.syncDeviceRead++;
						deviceRead = TRUE;
					}
					else
					{
						DAHandler->m_statistics.syncWrite++;
					}

					break;

				case SODaSRequest::syncCache:
					DAHandler->m_statistics.syncCacheRead++;
					break;

				default:
					break;
				}

				if (deviceRead)
				{
					DAHandler->m_statistics.timeRead += completionSpan;

					if (completionSpan > DAHandler->m_statistics.maxTimeRead)
					{
						DAHandler->m_statistics.maxTimeRead = completionSpan;
					}
				}

				if (operation == SODaSRequest::write)
				{
					DAHandler->m_statistics.timeWrite += completionSpan;

					if (completionSpan > DAHandler->m_statistics.maxTimeWrite)
					{
						DAHandler->m_statistics.maxTimeWrite = completionSpan;
					}
				}
			}
			else
			{
				if (operation == SODaSRequest::write)
				{
					DAHandler->m_statistics.failedWrite++;
				}
				else
				{
					DAHandler->m_statistics.failedReadOrBadQuality++;
				}
			}
		}
	}
}

#endif // SOFEATURE_WATCH

#endif // SODAS
