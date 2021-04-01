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
//  Filename    : SODaSRequest.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : I/O Request classes                                         |
//                - SODaSRequest: base class for I/O requests                 |
//                - SODaSRequestSyncReadData: requests synchron read data     |
//                - SODaSRequestSyncWriteData: requests synchron write data   |
//                - SODaSRequestAsyncWriteData: requests asynchron write data |
//                - SODaSRequestSyncReadServerData: requests synchron   read  |
//                  requests started via the server object                    |
//                - SODaSRequestDelegateData: requests delegated data         |
//                - SODaSRequestReportData: requests report data              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include <math.h>
#include "opcerror.h"
#include "SODaSRequest.h"
#include "SODaSItem.h"
#include "SODaSItemTag.h"
#include "SODaSCache.h"
#include "SODaSServer.h"
#include "SODaSEntry.h"
#include "SOCmnTrace.h"


//-----------------------------------------------------------------------------
// SODaSRequest                                                               |
//-----------------------------------------------------------------------------

SODaSRequest::SODaSRequest(
	IN BYTE operation,  // request operation
	IN WORD type)       // request type
	: SOCmnObject(SOCMNOBJECT_TYPE_REQUEST)
{
	m_operation = operation;
	m_type = type;
	m_item = NULL;
	m_flag = 0;
	setObjectState(stateInitialized);
	m_startTime = 0;
	m_result = S_OK;

	switch (m_type)
	{
	case SODaSRequest::syncDevice:
	case SODaSRequest::syncCache:
	case SODaSRequest::syncMaxAge:
	case SODaSRequest::serverDevice:
	case SODaSRequest::serverMaxAge:
		if (operation == SODaSRequest::read)
			SOCMN_ALLOCATE_OBJECT(m_depSyncReadData, SODaSRequestSyncReadData())
			else
				SOCMN_ALLOCATE_OBJECT(m_depSyncWriteData, SODaSRequestSyncWriteData())
				break;

	case SODaSRequest::asyncMaxAge:
		SOCMN_ALLOCATE_OBJECT(m_depAsyncReadData, SODaSRequestAsyncReadData())
	case SODaSRequest::asyncDevice:
		if (operation == SODaSRequest::write)
			SOCMN_ALLOCATE_OBJECT(m_depAsyncWriteData, SODaSRequestAsyncWriteData())
			break;

	case SODaSRequest::report:
		SOCMN_ALLOCATE_OBJECT(m_depReportData, SODaSRequestReportData())
		setObjectState(stateCompleted);
#ifdef SOFEATURE_WATCH
		SODaSWatchHandler::doRequestStatistics(this, 0);
#endif
		break;

	case SODaSRequest::delegatedDevice:
		SOCMN_ALLOCATE_OBJECT(m_depDelegateData, SODaSRequestDelegateData)
		break;
	}
}

SODaSRequest::~SODaSRequest()
{
	// release objects
	setItem(NULL);
	setServer(NULL);

	switch (m_type)
	{
	case SODaSRequest::syncDevice:
	case SODaSRequest::syncCache:
	case SODaSRequest::syncMaxAge:
	case SODaSRequest::serverDevice:
	case SODaSRequest::serverMaxAge:
		if (m_operation == SODaSRequest::read)
			SOCMN_FREE_OBJECT(m_depSyncReadData)
			else
				SOCMN_FREE_OBJECT(m_depSyncWriteData)
				break;

	case SODaSRequest::asyncMaxAge:
		SOCMN_FREE_OBJECT(m_depAsyncReadData)
	case SODaSRequest::asyncDevice:
		if (m_operation == SODaSRequest::write)
			SOCMN_FREE_OBJECT(m_depAsyncWriteData)
			break;

	case SODaSRequest::delegatedDevice:
		SOCMN_FREE_OBJECT(m_depDelegateData)
		break;

	case SODaSRequest::report:
		SOCMN_FREE_OBJECT(m_depReportData)
		break;
	}
}


//-----------------------------------------------------------------------------
// setItem
//
// - set item object
// - release previous item
//
// returns:
//		TRUE
//
BOOL SODaSRequest::setItem(
	IN SODaSItem* item) // item object
{
	if (m_type & SODASREQUEST_TYPE_ITEM)
	{
		SODaSItem* prevItem;

		{ // keep together for faster lock release
			SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
			prevItem = m_item;

			if (item)
			{
				setObjTraceId(NULL);
				item->addRef();
			}

			m_item = item;
		}

		if (prevItem)
		{
			prevItem->release();
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // setItem


//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous item tag
//
// returns:
//		TRUE
//
BOOL SODaSRequest::setItemTag(
	IN SODaSItemTag* itemTag) // tag object
{
	SOCmnPointer<SODaSItem> item = getItem();

	if (item.isNotNull())
	{
		return FALSE;
	}
	else
	{
		if (itemTag)
		{
			setObjTraceId(NULL);
		}

		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			if (m_operation == SODaSRequest::read)
			{
				return m_depSyncReadData->setItemTag(itemTag);
			}
			else
			{
				return m_depSyncWriteData->setItemTag(itemTag);
			}
		}
		else if (m_type == SODaSRequest::delegatedDevice)
		{
			return m_depDelegateData->setItemTag(itemTag);
		}
	}

	return FALSE;
} // setItemTag


//-----------------------------------------------------------------------------
// setServer
//
// - set server object
// - release previous server
//
// returns:
//		TRUE
//
BOOL SODaSRequest::setServer(
	IN SODaSServer* server) // server object
{
	if (m_type & SODASREQUEST_TYPE_SERVER)
	{
		SODaSServer* prevServer;

		{ // keep together for faster lock release
			SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
			prevServer = m_server;

			if (server)
			{
				setObjTraceId(NULL);
				server->addRef();
			}

			m_server = server;
		}

		if (prevServer)
		{
			prevServer->release();
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // setServer


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaSRequest::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
		SOCmnString id;
		SOCmnString descr;

		switch (m_type)
		{
		case SODaSRequest::syncCache:
			descr = _T("scri");
			break;

		case SODaSRequest::syncDevice:
			if (m_operation == SODaSRequest::read)
			{
				descr = _T("sdri");
			}
			else
			{
				descr = _T("s wi");
			}

			break;

		case SODaSRequest::asyncCache:
			descr = _T("acri");
			break;

		case SODaSRequest::asyncDevice:
			if (m_operation == SODaSRequest::read)
			{
				descr = _T("adri");
			}
			else
			{
				descr = _T("a wi");
			}

			break;

		case SODaSRequest::polledRefresh:
			descr = _T("acrp");
			break;

		case SODaSRequest::cyclic:
			descr = _T("cyc");
			break;

		case SODaSRequest::serverDevice:
			descr = _T("sdrs");
			break;

		case SODaSRequest::serverMaxAge:
			descr = _T("sars");
			break;

		case SODaSRequest::delegatedDevice:
			descr = _T("del");
			break;

		case SODaSRequest::report:
			descr = _T("rep");
			break;
		}

		if (!!itemTag)
		{
			id.format(_T("[%s|%8.8lX| %s(%s)]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)itemTag->getName(), (LPCTSTR)descr);
		}
		else
		{
			id.format(_T("[%s|%8.8lX| (%s)]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)descr);
		}

		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// getItem
//
// - get item object
//
// returns:
//		pointer to item object (maybe NULL)
//
SODaSItem* SODaSRequest::getItem(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (m_type & SODASREQUEST_TYPE_ITEM)
	{
		if (m_item)
		{
			m_item->addRef();
		}

		return m_item;
	}
	else
	{
		return NULL;
	}
} // getItem


//-----------------------------------------------------------------------------
// getItemTag
//
// - get tag object
//
// returns:
//		pointer to tag object
//
SODaSItemTag* SODaSRequest::getItemTag(void)
{
	SOCmnPointer<SODaSItem> item = getItem();

	if (item.isNotNull())
	{
		return item->getItemTag();
	}
	else
	{
		if ((m_type & (SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_SERVER)) == (SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_SERVER))
		{
			if (m_operation == SODaSRequest::read)
			{
				return m_depSyncReadData->getItemTag();
			}
			else
			{
				return m_depSyncWriteData->getItemTag();
			}
		}
		else if (m_type == SODaSRequest::delegatedDevice)
		{
			return m_depDelegateData->getItemTag();
		}
	}

	return NULL;
} // getItemTag


//-----------------------------------------------------------------------------
// getCache
//
// - get cache object
//
// returns:
//		pointer to cache object
//
SODaSCache* SODaSRequest::getCache(void)
{
	SOCmnPointer<SODaSItem> item = getItem();

	if (item.isNotNull())
	{
		return item->getCache();
	}
	else
	{
		if ((m_type & (SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_SERVER)) == (SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_SERVER))
		{
			if (m_operation == SODaSRequest::read)
			{
				return m_depSyncReadData->getCache();
			}
			else
			{
				return m_depSyncWriteData->getCache();
			}
		}
		else if (m_type == SODaSRequest::delegatedDevice)
		{
			return m_depDelegateData->getCache();
		}
	}

	return NULL;
} // getCache


//-----------------------------------------------------------------------------
// getServer
//
// - get server object
//
// returns:
//		pointer to cache object
//
SODaSServer* SODaSRequest::getServer(void)
{
	if (m_type & SODASREQUEST_TYPE_ITEM)
	{
		SOCmnPointer<SODaSItem> item = getItem();

		if (item.isNotNull())
		{
			SOCmnPointer<SODaSGroup> group = (SODaSGroup*)item->getParentBranch();

			if (group.isNotNull())
			{
				return (SODaSServer*)(SOCmnElement*)group->getParentBranch();
			}
		}

		return NULL;
	}
	else
	{
		// SODASREQUEST_TYPE_SERVER
		SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

		if (m_server)
		{
			m_server->addRef();
		}

		return m_server;
	}
} // getServer


//-----------------------------------------------------------------------------
// start
//
// - start request, set started flag
// - add to queue
// - signal start event
//
// returns:
//		TRUE  - request started
//		FALSE - can't start request
//
BOOL SODaSRequest::start(
	IN OPTIONAL BOOL signalQueue)
{
	SOCmnPointer<SODaSItemTag> tag = getItemTag();

	if (!tag)
	{
		// item tag was released during starting of the request
		complete(OPC_E_INVALIDITEMID, FALSE, OPC_QUALITY_UNCERTAIN, TRUE);
		return FALSE;   // -> error
	}

	SOCmnPointer<SODaSCache> cache = tag->getCache();

	if (!cache)
	{
		complete(OPC_E_INVALIDITEMID, FALSE, OPC_QUALITY_UNCERTAIN, TRUE);
		return FALSE; // cache not exists/ just report error
	}

	{
		SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

		if (isStarted())
		{
			return TRUE;
		}

		setObjectState(stateStarted);
	}

	m_startTime = ::GetTickCount();

	bool cacheExpired = false;
	if (cache->isSet())
	{
		SOCmnDateTime timeNow, timeCache;
		timeNow.now();
		cache->getTimeStamp(timeCache);
		timeCache.addMicroseconds(getMaxAge() * 1000);
		if (timeNow.getMicroseconds() > timeCache.getMicroseconds())
		{
			cacheExpired = true;
		}
	}

	if ((isDeviceRequest()) || (!cache->isSet()) || (cacheExpired))
	{
		// check cache age
		if ((m_type == SODaSRequest::cyclic) || (m_type & SODASREQUEST_TYPE_MAXAGE))
		{
			SOCmnDateTime nowFT;
			nowFT.now();
			ULONGLONG nowMS = nowFT.getMicroseconds();
			m_flag &= ~SODASREQUEST_FLAG_CACHE_AGE;

			if (cache.isNotNull())
			{
				SOCmnDateTime cacheTime;
				cache->getTimeStamp(this, cacheTime);
				ULONGLONG cacheTimeMS = cacheTime.getMicroseconds();

				if (m_type == SODaSRequest::cyclic)
				{
					// global cache age for cyclic requests
					DWORD maxCacheAge = tag->getMaxAgeCyclicReq();

					if (maxCacheAge != 0)
					{
						if (maxCacheAge == 0xFFFFFFFF)
						{
							// must be updated within the last group update rate period
							SOCmnPointer<SODaSItem> item = getItem();

							if (item.isNotNull())
							{
								SOCmnPointer<SODaSGroup> group = (SODaSGroup*)item->getParentBranch();

								if (group.isNotNull())
								{
									SOCmnPointer<SODaSTransaction> trans = group->getCyclicTransaction();
									SOCmnDateTime lastTransTime;
									trans->getLastCompletionTime(lastTransTime);
									ULONGLONG lastTransTimeMS = lastTransTime.getMicroseconds();

									if ((cacheTimeMS > lastTransTimeMS) && (lastTransTimeMS != 0))
									{
										// newer cache
										m_flag |= SODASREQUEST_FLAG_CACHE_AGE;
										complete();
										return TRUE;
									}
								}
							}
						}
						else
						{
							if (((nowMS - cacheTimeMS) / 1000) < (ULONGLONG)maxCacheAge)
							{
								m_flag |= SODASREQUEST_FLAG_CACHE_AGE;
								complete();
								return TRUE;
							}
						}
					}
				}
				else if (m_type & SODASREQUEST_TYPE_MAXAGE)
				{
					if (getMaxAge() > 0)
					{
						if (((nowMS - cacheTimeMS) / 1000) <= (ULONGLONG)getMaxAge())
						{
							m_flag |= SODASREQUEST_FLAG_CACHE_AGE;
							complete();
							return TRUE;
						}
					}
				}
			}
		}

		// device request
		switch (tag->getDeviceIOMode(this))
		{
		case SODaSItemTag::direct:
		{
			tag->handleRequest(this);
		}
		break;

		case SODaSItemTag::queue:
		{
			// insert request into item queue and set start event
			// get the right queue and event
			tag->addToQueue(this, m_operation == SODaSRequest::read);

			if (signalQueue)
			{
				tag->signalQueueEvent(m_operation == SODaSRequest::read);
			}
		}
		break;

		case SODaSItemTag::cache:
		case SODaSItemTag::constant:
		{
			complete();
		}
		break;

		case SODaSItemTag::delegate:
		{
			startDelegateRequest();
		}
		break;

		case SODaSItemTag::transaction:
			break;
		}
	}
	else
	{
		// cache request
		complete();
	}

	return TRUE;
} // start


//-----------------------------------------------------------------------------
// complete
//
// - complete request, set completed flag
// - signal completed event if set
//
// returns:
//		TRUE  - request completed
//		FALSE - can't complete request
//
BOOL SODaSRequest::complete(
	IN OPTIONAL HRESULT result,
	IN OPTIONAL BOOL validWriteQuality,
	IN OPTIONAL WORD writeQuality,
	IN OPTIONAL BOOL errorBeforeStart,
	IN OPTIONAL BOOL writeChangeCache)
{
	DWORD completionSpan;
	{
		SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

		if (isCompleted())
		{
			return TRUE;
		}

		if ((!isStarted()) && (!errorBeforeStart))
		{
			return FALSE;
		}

		setResult(result);  // request completed -> request handled
	}

	if (!errorBeforeStart)
	{
		SOCmnPointer<SODaSItemTag> tag = getItemTag();

		if (tag.isNotNull())
		{
			SOCmnPointer<SODaSCache> cache = tag->getCache();

			if (cache.isNotNull())
			{
				SOCmnPointer<SODaSItem> item = getItem();

				if (m_operation == SODaSRequest::write)
				{
					// write operation
					if (writeChangeCache)
					{
						// update cache
						WORD cacheQuality, quality;
						cacheQuality = cache->getQuality(this);

						if (SUCCEEDED(getResult()))
						{
							SOCmnVariant cacheValue, writeValue;
							cache->get(cacheValue, NULL, NULL, this);
							getWriteValue(&writeValue);
							cache->updateValue(writeValue, this);

							if (!(getValidWriteQT() & SODASREQUEST_FLAG_QUALITY))
							{
								if (!validWriteQuality)
								{
									quality = OPC_QUALITY_GOOD;
								}
								else
								{
									quality = writeQuality;
								}
							}
							else
							{
								quality = getWriteQuality();
							}

							cache->setQuality(quality, this);

							if (getValidWriteQT() & SODASREQUEST_FLAG_TIMESTAMP)
							{
								SOCmnDateTime ts;
								getWriteTimeStamp(ts);
								cache->setTimeStamp(ts, this);
							}

							if (tag->getUpdateMode() == SODaSItemTag::report)
							{
								if ((quality != cacheQuality) || (writeValue != cacheValue))
								{
									tag->notifyAllConnectedObjects();
								}
							}
						}
						else
						{
							// write error
							if (!validWriteQuality)
							{
								SOCmnDateTime timeStamp;
								cache->getTimeStamp(this, timeStamp);

								if (!timeStamp.isSet())
								{
									quality = OPC_QUALITY_LAST_KNOWN;
								}
								else
								{
									quality = OPC_QUALITY_COMM_FAILURE;
								}
							}
							else
							{
								quality = writeQuality;
							}

							cache->setQuality(quality, this);

							if (tag->getUpdateMode() == SODaSItemTag::report)
							{
								if (quality != cacheQuality)
								{
									tag->notifyAllConnectedObjects();
								}
							}
						}
					}

					if (item.isNotNull())
					{
						item->onWrite();
					}
				}
				else
				{
					// read operation
					if ((tag->getDeviceIOMode(this) == SODaSItemTag::delegate) && SUCCEEDED(result))
					{
						completeDelegateRequest();
					}

					if (item.isNotNull())
					{
						item->onRead(isDeviceRequest());
					}

					if ((m_type != SODaSRequest::report) && (tag->getUpdateMode() == SODaSItemTag::report))
					{
						// values reported event driven must have a current timestamp
						SOCmnDateTime now;
						now.now();
						if ((m_type & SODaSRequest::syncMaxAge) == SODaSRequest::syncMaxAge)
						{
							cache->setTimeStamp(now);
						}
						else
						{
							this->setReportTimeStamp(now);
						}
					}
					if ((m_type == SODaSRequest::polledRefresh) && (item.isNotNull()))
					{
						WORD cacheQuality = cache->getQuality(this);

						if ((cacheQuality & OPC_STATUS_MASK) < OPC_QUALITY_GOOD)
						{
							// check result of cyclic request
							SOCmnPointer<SODaSRequest> cycReq = item->getCyclicRequest();

							if (cycReq.isNotNull())
							{
								HRESULT cycRes = cycReq->getResult();

								if (cycRes != S_OK)
								{
									setResult(cycRes);
								}
							}
						}
					}

					if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
					{
						switch (m_type)
						{
						case SODaSRequest::syncDevice:
						case SODaSRequest::syncCache:
						{
#ifdef SOFEATURE_DCOM
							SOCmnVariant vV;
							SOCmnDateTime tV;
							OPCITEMSTATE* value;
							value = getItemStateAddress();

							if (SUCCEEDED(getResult()))
							{
								item = getItem();

								if (item.isNotNull())
								{
									HRESULT convRes = S_OK;
									value->hClient = item->getClientHandle();

									if (cache->get(vV, &value->wQuality, &tV, this, item->getReqDatatype(), getLCID(), &convRes))
									{
										vV.copyTo(&value->vDataValue);
										tV.get(&value->ftTimeStamp);
									}
									else
									{
										if (SUCCEEDED(getResult()))
										{
											setResult(convRes);
										}
									}

									if (m_type == SODaSRequest::syncCache)
									{
										if (!item->isActive())
										{
											value->wQuality = OPC_QUALITY_OUT_OF_SERVICE;
										}
									}
								}
								else
								{
									setResult(E_FAIL);
								}
							}

							if (FAILED(getResult()))
							{
								value->vDataValue.vt = VT_EMPTY;
								item = getItem();

								if (item.isNotNull())
								{
									value->hClient = item->getClientHandle();
								}
							}

#endif
						}
						break;

						case SODaSRequest::syncMaxAge:
						case SODaSRequest::serverDevice:
						case SODaSRequest::serverMaxAge:
						{
							SOCmnVariant vV;
							SOCmnDateTime tV;
#ifdef SOFEATURE_DCOM
							LPVARIANT value;
							value = getReadValueAddress();
#endif
#ifdef SOFEATURE_SOAP
							SOCmnVariant* pV;
							getReadValueAddress(pV);
#endif

							if (SUCCEEDED(getResult()))
							{
								SOCmnPointer<SODaSServer> server = getServer();
								WORD* quality = getReadQualityAddress();
#ifdef SOFEATURE_DCOM
								FILETIME* timeStamp = getReadTimeStampAddress();
#endif
#ifdef SOFEATURE_SOAP
								SOCmnDateTime* pT;
								getReadTimeStampAddress(pT);
#endif
								LCID lcid = MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT);
								HRESULT convRes = S_OK;

								if (server.isNotNull())
								{
									lcid = server->getLCID();
								}

								VARTYPE reqVarType = getReqDatatype();

								if (item.isNotNull() == TRUE)
								{
									reqVarType = item->getReqDatatype();
								}

								if (cache->get(vV, quality, &tV, this, reqVarType, lcid, &convRes))
								{
#ifdef SOFEATURE_DCOM

									if (value)
									{
										vV.copyTo(value);
									}

									if (timeStamp)
									{
										tV.get(timeStamp);
									}

#endif
#ifdef SOFEATURE_SOAP

									if (pV)
									{
										*pV = vV;
									}

									if (pT)
									{
										*pT = tV;
									}

#endif
								}
								else
								{
									if (SUCCEEDED(getResult()))
									{
										setResult(convRes);
									}
								}
							}
							else
							{
#ifdef SOFEATURE_DCOM

								if (value)
								{
									value->vt = VT_EMPTY;
								}

#endif
#ifdef SOFEATURE_SOAP

								if (pV)
								{
									pV->clear();
								}

#endif
							}
						}
						break;
						}
					}
				}

				if (m_type == SODaSRequest::delegatedDevice)
				{
					SOCmnPointer<SODaSRequest> req = getOriginalRequest();
					req->complete(result, validWriteQuality, writeQuality, FALSE);
					setOriginalRequest(NULL);
					release();
				}
			}
		}
	}

	//  lock this for the state change and the completeEvent signal
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	setObjectState(stateCompleted);
	completionSpan = getTimeSpan(m_startTime, ::GetTickCount());
#ifdef SOFEATURE_WATCH
	SODaSWatchHandler::doRequestStatistics(this, completionSpan);
#endif

	if (((m_flag & SODASREQUEST_FLAG_CACHE_AGE) == 0) || (m_type != SODaSRequest::cyclic))
	{
		m_completedEvent.signal();
	}

	return TRUE;
} // complete


//-----------------------------------------------------------------------------
// pending
//
// - set request pending
//
// returns:
//		TRUE  - request set pending
//		FALSE - can't set request pending
//
BOOL SODaSRequest::pending(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (isPending())
	{
		return TRUE;
	}

	if ((!isStarted()) || (isCompleted()))
	{
		return FALSE;
	}

	setObjectState(statePending);
	return TRUE;
} // pending


//-----------------------------------------------------------------------------
// cancel
//
// - cancel request, set cancel flag
//
// returns:
//		TRUE  - request canceled
//		FALSE - can't cancel request
//
BOOL SODaSRequest::cancel(
	IN OPTIONAL HRESULT compResult,         // result
	IN OPTIONAL BOOL changeCacheQuality,    // quality
	IN OPTIONAL WORD cacheQuality)          // do change the cache?
{
	{
		SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

		if (isCanceled())
		{
			return TRUE;
		}

		if (isCompleted())
		{
			return FALSE;
		}

		SOCmnPointer<SODaSItem> item = getItem();
		if (item.isNotNull())
		{
			SOCmnPointer<SODaSGroup> group = (SODaSGroup*)item->getParentBranch();
			if (group.isNotNull())
			{
				if (m_type == SODaSRequest::cyclic)
				{
					SOCmnPointer<SODaSTransaction> trans = group->getCyclicTransaction();
					trans->notiofyCompletedInternally(this);
				}
				else
				{
					SOCmnPointer<SODaSServer> server = (SODaSServer*)group->getParentBranch();
					if (server.isNotNull())
					{
						SOCmnList<SODaSTransaction> tacList(server->getTransactionList());
						SODaSTransaction* pTac;
						SOCmnListPosition posTac;
						SOCmnPointer<SODaSGroup> tacGrp;
						
						posTac = tacList.getStartPosition();
						while (posTac)
						{
							pTac = tacList.getNext(posTac);
							if (pTac->getType() != SODaSTransaction::cyclic)
							{
								tacGrp = pTac->getGroup();
								if ((SODaSGroup*)tacGrp == group)
								{
									pTac->notiofyCompletedInternally(this);
								}
							}
						}
					}
				}
			}
		}
		complete(compResult);
		setObjectState(stateCanceled);
	}

	if (changeCacheQuality)
	{
		SOCmnPointer<SODaSCache> cache = getCache();

		if (cache.isNotNull())
		{
			cache->setQuality(cacheQuality, this);
		}
	}

	SOCmnPointer<SODaSItemTag> tag = getItemTag();

	if ((SODaSItemTag*)tag)
	{
		if ((isDeviceRequest()) && (isStarted()) && (tag->getDeviceIOMode(this) == SODaSItemTag::queue))
		{
			tag->signalQueueEvent(m_operation == SODaSRequest::read);
		}
	}

	return TRUE;
} // cancel


//-----------------------------------------------------------------------------
// reset
//
// - reset the state of the request
//
void SODaSRequest::reset(void)
{
	setObjectState(stateInitialized);
	setResult(S_OK);
} // reset


//-----------------------------------------------------------------------------
// hasChanged
//
// - check if the value of the requests cache has changed since last advise
//
// returns:
//		TRUE  - changed
//		FALSE - same
//
BOOL SODaSRequest::hasChanged(
	IN size_t connection,    // connection id
	IN SOCmnVariant& value,
	IN WORD quality)
{
	if ((m_type != SODaSRequest::asyncInitNotify) && (m_type != SODaSRequest::cyclic) &&
		(m_type != SODaSRequest::report) && (m_type != SODaSRequest::polledRefresh))
	{
		return TRUE;
	}

	SOCmnPointer<SODaSItem> item = getItem();

	if (!item)
	{
		return TRUE;
	}

	if (!item->isActive())
	{
		return FALSE;
	}

	//-- cyclic requests --
	SOCmnVariant prevSendValue;
	WORD prevSendQuality;
	BOOL valueChanged;

	//-- get previous send value for the connection --
	if (!item->getSendValue(connection, prevSendValue, &prevSendQuality))
	{
		if ((m_type == SODaSRequest::cyclic) || (m_type == SODaSRequest::report))
		{
			SOCmnPointer<SODaSGroup> group = (SODaSGroup*)item->getParentBranch();

			if (group.isNotNull())
			{
				if (group->rejectCyclicDataBeforeInit())
				{
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("cyclic/report before asyncInitNotify")));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	//-- compare quality --
	if (prevSendQuality != quality)
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("changed quality")));
		SOCmnVariant::variantClear(&prevSendValue);
		return TRUE;
	}

	//-- compare value --
	valueChanged = !(prevSendValue == (LPCVARIANT)value);

	//-- percent deadband --
	if (valueChanged)
	{
		// check deadband only if value changed.
		SOCmnPointer<SODaSGroup> group = (SODaSGroup*)item->getParentBranch();

		if (group.isNotNull())
		{
			SOCmnPointer<SODaSItemTag> tag = getItemTag();

			if (tag.isNotNull())
			{
				SOCmnPointer<SODaSCache> cache = getCache();

				if (tag->getEUType() == OPC_ANALOG)
				{
					// percent deadband only counts for analog tags
					SOCmnVariant limits;
					SOCmnVariant locvalue;
					double* limit;
					double deadband = 0.0;
					FLOAT percDB = group->getDeadBand();  // group deadband (default 0.0)
					item->getDeadBand(percDB);            // item deadband (if available)
					// calculate deadband
					tag->getEUInfo(limits);

					if (limits.vt == (VT_R8 | VT_ARRAY))
					{
						if (limits.parray->rgsabound[0].cElements == 2)
						{
							SafeArrayAccessData(limits.parray, (void**)&limit);
							deadband = (limit[1] - limit[0]) * ((double)percDB / 100.0);
							SafeArrayUnaccessData(limits.parray);
						}
					}

					// convert values to double
					if ((cache->changeDatatype(prevSendValue, VT_R8, prevSendValue)) &&
						(cache->changeDatatype(value, VT_R8, locvalue)))
					{
						valueChanged = (fabs(prevSendValue.dblVal - locvalue.dblVal) > fabs(deadband));
						_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("hasChanged %s deadband %lf difference %lf"), BOOL2STR(valueChanged), fabs(deadband), fabs(prevSendValue.dblVal - locvalue.dblVal)));
					}
				}
			}
		}
	}

	SOCmnVariant::variantClear(&prevSendValue);

	if (valueChanged)
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("changed value")));
		return TRUE;
	}

	return FALSE;
} // hasChanged


//-----------------------------------------------------------------------------
// setResultAddress
//
// - set the result address of the request
// - only for synchron requests
//
void SODaSRequest::setResultAddress(
	IN HRESULT* resAddr)    // result address
{
	if (m_type & SODASREQUEST_TYPE_SYNC)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			if (m_operation == SODaSRequest::write)
			{
				m_depSyncWriteData->setResultAddress(resAddr);
			}
			else
			{
				m_depSyncReadData->setResultAddress(resAddr);
			}
		}
	}
} // setResultAddress


//-----------------------------------------------------------------------------
// getResult
//
// - get the result of the request execution
//
// returns:
//		result
//
HRESULT SODaSRequest::getResult(void)
{
	if (m_type & SODASREQUEST_TYPE_CYCLIC)
	{
		return m_result;
	}
	else if (m_type & SODASREQUEST_TYPE_REPORT)
	{
		return m_depReportData->getResult();
	}
	else if (m_type & SODASREQUEST_TYPE_ASYNC)
	{
		if (m_operation == SODaSRequest::write)
		{
			if (m_type != SODaSRequest::delegatedDevice)
			{
				return m_depAsyncWriteData->getResult();
			}
			else
			{
				return m_depDelegateData->getResult();
			}
		}
		else
		{
			// read
			if (m_type == SODaSRequest::asyncMaxAge)
			{
				return m_depAsyncReadData->getResult();
			}
			else if (m_type == SODaSRequest::delegatedDevice)
			{
				return m_depDelegateData->getResult();
			}
			else
			{
				return m_result;
			}
		}
	}
	else
	{
		// sync
		if (m_operation == SODaSRequest::write)
		{
			return m_depSyncWriteData->getResult();
		}
		else
		{
			return m_depSyncReadData->getResult();
		}
	}

	//  return E_FAIL; // superflous
} // getResult


//-----------------------------------------------------------------------------
// setResult
//
// - set the result of the request execution
//
void SODaSRequest::setResult(
	IN HRESULT res)
{
	if (m_type & SODASREQUEST_TYPE_CYCLIC)
	{
		m_result = res;
	}
	else if (m_type & SODASREQUEST_TYPE_REPORT)
	{
		m_depReportData->setResult(res);
	}
	else if (m_type & SODASREQUEST_TYPE_ASYNC)
	{
		if (m_operation == SODaSRequest::write)
		{
			m_depAsyncWriteData->setResult(res);
		}
		else
		{
			if (m_type == SODaSRequest::asyncMaxAge)
			{
				m_depAsyncReadData->setResult(res);
			}
			else if (m_type == SODaSRequest::delegatedDevice)
			{
				m_depDelegateData->setResult(res);
			}
			else
			{
				m_result = res;
			}
		}
	}
	else
	{
		// sync
		if (m_operation == SODaSRequest::write)
		{
			m_depSyncWriteData->setResult(res);
		}
		else
		{
			m_depSyncReadData->setResult(res);
		}
	}
} // setResult


//-----------------------------------------------------------------------------
// getWriteValue
//
// - get the write value of the request
// - only for write requests
//
// returns:
//		TRUE  - ok
//		FALSE - no write value
//
BOOL SODaSRequest::getWriteValue(
	OUT LPVARIANT value)    // write value
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			return m_depSyncWriteData->getWriteValue(value);
		}
		else
		{
			if (m_type != SODaSRequest::delegatedDevice)
			{
				return m_depAsyncWriteData->getWriteValue(value);
			}
			else
			{
				return m_depDelegateData->getWriteValue(value);
			}
		}
	}

	return FALSE;
} // getWriteValue


//-----------------------------------------------------------------------------
// setWriteValue
//
// - set the write value of the request
// - only for write requests
//
// returns:
//		TRUE  - ok
//		FALSE - no write value
//
BOOL SODaSRequest::setWriteValue(
	IN LPCVARIANT value)    // write value
{
	if (m_operation == SODaSRequest::write)
	{
		SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
		LCID localeID = getLCID();

		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			return m_depSyncWriteData->setWriteValue(value, localeID, itemTag, isSOAPRequest());
		}
		else
		{
			return m_depAsyncWriteData->setWriteValue(value, localeID, itemTag, isSOAPRequest());
		}
	}

	return FALSE;
} // setWriteValue


//-----------------------------------------------------------------------------
// setWriteQuality
//
// - set the write quality of the request
// - only for write requests
//
void SODaSRequest::setWriteQuality(
	IN WORD writeQuality) // quality to write
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			m_depSyncWriteData->setWriteQuality(writeQuality);
		}
		else
		{
			m_depAsyncWriteData->setWriteQuality(writeQuality);
		}
	}
} // setWriteQuality


//-----------------------------------------------------------------------------
// getWriteQuality
//
// - get the write quality of the request
// - only for write requests
//
// returns:
//		the write quality
//
WORD SODaSRequest::getWriteQuality(void)
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			return m_depSyncWriteData->getWriteQuality();
		}
		else
		{
			if (m_type != SODaSRequest::delegatedDevice)
			{
				return m_depAsyncWriteData->getWriteQuality();
			}
			else
			{
				return m_depDelegateData->getWriteQuality();
			}
		}
	}

	return 0;
} // getWriteQuality


//-----------------------------------------------------------------------------
// setWriteTimeStamp
//
// - set the write quality of the request
// - only for write requests
//
void SODaSRequest::setWriteTimeStamp(
	IN const SOCmnDateTime& writeTimeStamp) // timestamp to write
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			m_depSyncWriteData->setWriteTimeStamp(writeTimeStamp);
		}
		else
		{
			m_depAsyncWriteData->setWriteTimeStamp(writeTimeStamp);
		}
	}
} // setWriteTimeStamp


//-----------------------------------------------------------------------------
// getWriteTimeStamp
//
// - get the write quality of the request
// - only for write requests
//
void SODaSRequest::getWriteTimeStamp(
	OUT SOCmnDateTime& writeTimeStamp) // timestamp
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			m_depSyncWriteData->getWriteTimeStamp(writeTimeStamp);
		}
		else
		{
			if (m_type != SODaSRequest::delegatedDevice)
			{
				m_depAsyncWriteData->getWriteTimeStamp(writeTimeStamp);
			}
			else
			{
				m_depDelegateData->getWriteTimeStamp(writeTimeStamp);
			}
		}
	}
} // getWriteTimeStamp


//-----------------------------------------------------------------------------
// getValidWriteQT
//
// - get the valid flags for write quality and timestamp
// - only for write requests
//
// returns:
//		the valid flags for write quality and timestamp
//
BYTE SODaSRequest::getValidWriteQT(void)
{
	if (m_operation == SODaSRequest::write)
	{
		if ((m_type & SODASREQUEST_TYPE_SYNC) == SODASREQUEST_TYPE_SYNC)
		{
			return m_depSyncWriteData->getValidQT();
		}
		else
		{
			if (m_type != SODaSRequest::delegatedDevice)
			{
				return m_depAsyncWriteData->getValidQT();
			}
			else
			{
				return m_depDelegateData->getValidQT();
			}
		}
	}

	return 0;
} // getValidWriteQT


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setItemStateAddress
//
// - set the address of item state struct of the request
// - only for synchron read requests
//
void SODaSRequest::setItemStateAddress(
	IN OPCITEMSTATE* itemStateAddr) // address of item state struct
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
		if (m_type & (SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_CACHE))
		{
			m_depSyncReadData->setItemStateAddress(itemStateAddr);
		}
} // setItemStateAddress


//-----------------------------------------------------------------------------
// getItemStateAddress
//
// - get the address of item state struct of the request
// - only for synchron read requests
//
// returns:
//		address of item state struct
//
OPCITEMSTATE* SODaSRequest::getItemStateAddress(void)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
		if (m_type & (SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_CACHE))
		{
			return m_depSyncReadData->getItemStateAddress();
		}

	return NULL;
} // getItemStateAddress

#endif
#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setReadValueAddress
//
// - set the address of read value of the request
// - only for synchron property read requests
//
void SODaSRequest::setReadValueAddress(
	IN LPVARIANT readValueAddr) // address of read value
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadValueAddress(readValueAddr);
	}
} // setReadValueAddress


//-----------------------------------------------------------------------------
// getReadValueAddress
//
// - get the address of read value of the request
// - only for synchron property read requests
//
// returns:
//		address of read value
//
LPVARIANT SODaSRequest::getReadValueAddress(void)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		return m_depSyncReadData->getReadValueAddress();
	}

	return NULL;
} // getReadValueAddress

#endif
#ifdef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// setReadValueAddress
//
// - set the address of read value of the request
// - only for synchron property read requests
//
void SODaSRequest::setReadValueAddress(
	IN SOCmnVariant* readValueAddr) // address of read value
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadValueAddress(readValueAddr);
	}
} // setReadValueAddress


//-----------------------------------------------------------------------------
// getReadValueAddress
//
// - get the address of read value of the request
// - only for synchron property read requests
//
// returns:
//		address of read value
//
void SODaSRequest::getReadValueAddress(SOCmnVariant *&readValueAddr)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->getReadValueAddress(readValueAddr);
	}
} // getReadValueAddress

#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// setReadValueAddress
//
// - set the address of read value of the request
// - only for synchron property read requests
//
void SODaSRequest::setReadValueAddress(
	IN SOCmnVariant* readValueAddr) // address of read value
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadValueAddress(readValueAddr);
	}
} // setReadValueAddress


//-----------------------------------------------------------------------------
// getReadValueAddress
//
// - get the address of read value of the request
// - only for synchron property read requests
//
// returns:
//		address of read value
//
void SODaSRequest::getReadValueAddress(SOCmnVariant *&readValueAddr)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->getReadValueAddress(readValueAddr);
	}
} // getReadValueAddress

#endif // SOFEATURE_SOAP
#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// setReadQualityAddress
//
// - set the address of read quality of the request
// - only for synchron server read requests
//
void SODaSRequest::setReadQualityAddress(
	IN WORD* readQualityAddr)   // address of read value
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadQualityAddress(readQualityAddr);
	}
} // setReadQualityAddress


//-----------------------------------------------------------------------------
// getReadQualityAddress
//
// - get the address of read quality of the request
// - only for synchron server read requests
//
// returns:
//		address of read quality
//
WORD* SODaSRequest::getReadQualityAddress(void)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		return m_depSyncReadData->getReadQualityAddress();
	}

	return NULL;
} // getReadQualityAddress


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setReadTimeStampAddress
//
// - set the address of read time stamp of the request
// - only for synchron server read requests
//
void SODaSRequest::setReadTimeStampAddress(
	IN FILETIME* readTimeStampAddr) // address of read timestamp
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadTimeStampAddress(readTimeStampAddr);
	}
} // setReadTimeStampAddress


//-----------------------------------------------------------------------------
// getReadTimeStampAddress
//
// - get the address of read time stamp of the request
// - only for synchron server read requests
//
// returns:
//		address of read timestamp
//
FILETIME* SODaSRequest::getReadTimeStampAddress(void)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		return m_depSyncReadData->getReadTimeStampAddress();
	}

	return NULL;
} // getReadTimeStampAddress

#endif
#ifdef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// setReadTimeStampAddress
//
// - set the address of read time stamp of the request
// - only for synchron server read requests
//
void SODaSRequest::setReadTimeStampAddress(
	IN SOCmnDateTime* readTimeStampAddr)    // address of read timestamp
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadTimeStampAddress(readTimeStampAddr);
	}
} // setReadTimeStampAddress


//-----------------------------------------------------------------------------
// getReadTimeStampAddress
//
// - get the address of read time stamp of the request
// - only for synchron server read requests
//
// returns:
//		address of read timestamp
//
void SODaSRequest::getReadTimeStampAddress(SOCmnDateTime *&pDT)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->getReadTimeStampAddress(pDT);
	}
	else
	{
		pDT = NULL;
	}
} // getReadTimeStampAddress

#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// setReadTimeStampAddress
//
// - set the address of read time stamp of the request
// - only for synchron server read requests
//
void SODaSRequest::setReadTimeStampAddress(
	IN SOCmnDateTime* readTimeStampAddr)    // address of read timestamp
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReadTimeStampAddress(readTimeStampAddr);
	}
} // setReadTimeStampAddress


//-----------------------------------------------------------------------------
// getReadTimeStampAddress
//
// - get the address of read time stamp of the request
// - only for synchron server read requests
//
// returns:
//		address of read timestamp
//
void SODaSRequest::getReadTimeStampAddress(SOCmnDateTime *&pDT)
{
	if ((m_type & SODASREQUEST_TYPE_SYNC) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->getReadTimeStampAddress(pDT);
	}
	else
	{
		pDT = NULL;
	}
} // getReadTimeStampAddress

#endif // SOFEATURE_SOAP
#endif // SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// setMaxAge
//
// - set the maximum cache of the request
// - only for synchron server read requests
//
void SODaSRequest::setMaxAge(
	IN DWORD maxAge) // maximum cache age
{
	if ((m_operation == SODaSRequest::read) && (m_type & (SODASREQUEST_TYPE_MAXAGE)))
	{
		if (m_type & SODASREQUEST_TYPE_SYNC)
		{
			m_depSyncReadData->setMaxAge(maxAge);
		}
		else if (m_type & SODASREQUEST_TYPE_ASYNC)
		{
			m_depAsyncReadData->setMaxAge(maxAge);
		}
	}
} // setMaxAge


//-----------------------------------------------------------------------------
// getMaxAge
//
// - get the address of read time stamp of the request
// - only for synchron server read requests
//
// returns:
//		address of read timestamp
//
DWORD SODaSRequest::getMaxAge(void)
{
	if ((m_operation == SODaSRequest::read) && (m_type & (SODASREQUEST_TYPE_MAXAGE)))
	{
		if (m_type & SODASREQUEST_TYPE_SYNC)
		{
			return m_depSyncReadData->getMaxAge();
		}
		else if (m_type & SODASREQUEST_TYPE_ASYNC)
		{
			return m_depAsyncReadData->getMaxAge();
		}
	}

	return 0;
} // getMaxAge


void SODaSRequest::setReqDatatype(IN VARTYPE reqDatatype)
{
	if (((m_type == SODaSRequest::serverMaxAge) || (m_type == SODaSRequest::syncMaxAge)) && (m_operation == SODaSRequest::read))
	{
		m_depSyncReadData->setReqDatatype(reqDatatype);
	}
} // setReqDatatype

VARTYPE SODaSRequest::getReqDatatype(void)
{
	if (((m_type == SODaSRequest::serverMaxAge) || (m_type == SODaSRequest::syncMaxAge)) && (m_operation == SODaSRequest::read))
	{
		return m_depSyncReadData->getReqDatatype();
	}

	return VT_EMPTY;
} // getReqDatatype


//-----------------------------------------------------------------------------
// getLCID
//
// - get the locale id of the request
//
// returns:
//		locale id
//
LCID SODaSRequest::getLCID(void)
{
	LCID localeID = MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT);
	SOCmnPointer<SODaSItem> item = getItem();

	if (item.isNotNull())
	{
		SOCmnPointer<SODaSGroup> grp((SODaSGroup*)item->getParentBranch());

		if (grp.isNotNull())
		{
			localeID = grp->getLCID();

			if (localeID == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
			{
				// no group specific lcid set
				SOCmnPointer<SODaSServer> srv((SODaSServer*)grp->getParentBranch());

				if (srv.isNotNull())
				{
					localeID = srv->getLCID();
				}
			}
		}
	}

	return localeID;
} // getLCID


//-----------------------------------------------------------------------------
// startDelegateRequest
//
// - create a delegated request
//
// returns:
//		created request
//
void SODaSRequest::startDelegateRequest(void)
{
	SODaSRequest* req = NULL;
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if ((SODaSItemTag*)itemTag)
	{
		if (itemTag->is(SOCMNOBJECT_TYPE_PROPERTY))
		{
			SODaSProperty* prop = (SODaSProperty*)((SODaSItemTag*)itemTag);
			DWORD pid = prop->getPropertyId();

			if ((((pid >= 2) && (pid <= 4)) || (pid == 2503)) && (getOperation() == SODaSRequest::read))
			{
				SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
				SOCmnPointer<SODaSCreator> creator = engine->getCreator();
				SOCmnPointer<SODaSTag> tag = (SODaSTag*)prop->getParentBranch();
				SOCmnPointer<SODaSTransaction> tac;
				SOCmnPointer<SODaSServer> server = getServer();
				// create transaction
				req = creator->createRequest(NULL, NULL, tag, SODaSRequest::read, SODaSRequest::delegatedDevice);

				if (req != NULL)
				{
					if (isSOAPRequest())
					{
						req->setFlag(SODASREQUEST_FLAG_SOAP);
					}

					req->setServer(server);
					req->setItemTag(tag);
					req->setOriginalRequest(this);
					SOCmnPointer<SODaSServer> srv = getServer();

					if (srv.isNotNull())
					{
						tac = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
						tac->insertRequest(req);
						tac->start(srv);
						srv->addTransaction(tac);
						return; // success exit
					}
					else
					{
						delete req;
					}
				}
			}
		}
	}

	complete(E_FAIL);
} // startDelegateRequest


//-----------------------------------------------------------------------------
// completeDelegateRequest
//
// - complete a delegated request
//
// returns:
//		TRUE  - request completed
//		FALSE - can't complete request
//
BOOL SODaSRequest::completeDelegateRequest(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
	BOOL ret = FALSE;

	if ((SODaSItemTag*)itemTag)
	{
		if (itemTag->is(SOCMNOBJECT_TYPE_PROPERTY))
		{
			SODaSProperty* prop = (SODaSProperty*)((SODaSItemTag*)itemTag);
			DWORD pid = prop->getPropertyId();

			if ((((pid >= 2) && (pid <= 4)) || (pid == 2503)) && (getOperation() == SODaSRequest::read))
			{
				SOCmnPointer<SODaSTag> tag = (SODaSTag*)prop->getParentBranch();
				SOCmnPointer<SODaSCache> tagCache = tag->getCache();
				SOCmnPointer<SODaSCache> cache = getCache();
				cache->setQuality(tagCache->getQuality(this), this);

				switch (pid)
				{
				case 2:
				{
					SOCmnVariant value;
					WORD quality;
					SOCmnDateTime timeStamp;
					tagCache->get(value, &quality, &timeStamp, this);
					cache->update(value, quality, &timeStamp, this);
				}
				break;

				case 3:
				case 2503:
				{
					SOCmnVariant value;

					if (isDCOMRequest())
					{
						value = (SHORT)tagCache->getQuality(this);
					}
					else
					{
#ifdef SOFEATURE_SOAP
						SOCmnPointer<SODaSServer> server = getServer();

						if (server.isNotNull())
						{
							SOCmnStringEx buffer;
							server->getXMLQuality(tagCache->getQuality(this), FALSE, buffer);
							value = (LPCTSTR)buffer;
						}

#endif
					}

					cache->updateValue(value, this);
				}
				break;

				case 4:
				{
					SOCmnVariant value;
					SOCmnDateTime ft;
					tagCache->getTimeStamp(this, ft);
					value.vt = VT_DATE;
					ft.get(&value.date);
					cache->updateValue(value, this);
				}
				break;
				}

				ret = TRUE;
			}
		}
	}

	// signal work event of server to remove finished transactions of delegated requests
	SOCmnPointer<SODaSServer> server = getServer();
	server->signalWorkEvent();
	return ret;
} // completeDelegateRequest


//-----------------------------------------------------------------------------
// setOriginalRequest
//
// - set the original request of the delegated request
//
// returns:
//		TRUE  - request completed
//		FALSE - can't complete request
//
BOOL SODaSRequest::setOriginalRequest(
	IN SODaSRequest* req)   // original request
{
	if (m_type == SODaSRequest::delegatedDevice)
	{
		return m_depDelegateData->setOriginalRequest(req);
	}

	return FALSE;
} // setOriginalRequest


//-----------------------------------------------------------------------------
// getOriginalRequest
//
// - get the original request of the delegated request
//
// returns:
//		original request
//
SODaSRequest* SODaSRequest::getOriginalRequest(void)
{
	if (m_type == SODaSRequest::delegatedDevice)
	{
		return m_depDelegateData->getOriginalRequest();
	}

	return NULL;
} // getOriginalRequest


//-----------------------------------------------------------------------------
// signalQueueEvents
//
// - signal all queue events of the request list
//
void SODaSRequest::signalQueueEvents(
	IN SOCmnList<SODaSRequest> *reqList)    // request list
{
	if (!reqList)
	{
		return;
	}

	SOCmnListPosition pos;
	SODaSRequest* req;
	SOCmnPointer<SODaSItemTag> itemTag;
	SOCmnPointer<SODaSCache> cache;
	SOCmnEventPointer lastQH;
	SOCmnEventPointer qh;
	reqList->lock();
	pos = reqList->getStartPosition();

	while (pos)
	{
		req = reqList->getNext(pos);
		itemTag = req->getItemTag();
		cache = req->getCache();

		if (itemTag.isNotNull() && cache.isNotNull())
		{
			if (((req->isDeviceRequest()) || (!cache->isSet())) && (itemTag->getDeviceIOMode(req) == SODaSItemTag::queue))
			{
				if (req->getOperation() == SODaSRequest::read)
				{
					qh.set(itemTag->getReadEvent());
				}
				else
				{
					qh.set(itemTag->getWriteEvent());
				}

				if (qh.isInit())
				{
					if (lastQH != qh)
					{
						// simple optimation remind last queue handle
						// signal only if queue handle not equal last queue handle
						qh.signal();
						lastQH = qh;
					}
				}
			}
		}
	}

	reqList->unlock();
} // signalQueueEvents


//-----------------------------------------------------------------------------
// setReportValue
//
// - set the report value of the request
//
// returns:
//    TRUE  - value set
//    FALSE - invalid argument or no report request
//
BOOL SODaSRequest::setReportValue(IN SOCmnVariant& value)
{
	if (m_type == SODaSRequest::report)
	{
		return m_depReportData->setReportValue(value);
	}

	return FALSE;
} // setReportValue


//-----------------------------------------------------------------------------
// getReportValue
//
// - get the report value of the request
//
// returns:
//    TRUE  - got value
//    FALSE - invalid argument or no report request
//
BOOL SODaSRequest::getReportValue(OUT SOCmnVariant& value)
{
	if (m_type == SODaSRequest::report)
	{
		return m_depReportData->getReportValue(value);
	}

	return FALSE;
} // getReportValue


//-----------------------------------------------------------------------------
// setReportTimeStamp
//
// - set the report time stamp of the request
//
// returns:
//    TRUE  - time stamp set
//    FALSE - invalid argument or no report request
//
BOOL SODaSRequest::setReportTimeStamp(IN SOCmnDateTime& timeStamp)
{
	if (m_type == SODaSRequest::report)
	{
		return m_depReportData->setReportTimeStamp(timeStamp);
	}

	return FALSE;
} // setReportTimeStamp


//-----------------------------------------------------------------------------
// getReportTimeStamp
//
// - get the report time stamp of the request
//
// returns:
//    TRUE  - got time stamp
//    FALSE - invalid argument or no report request
//
BOOL SODaSRequest::getReportTimeStamp(OUT SOCmnDateTime& timeStamp)
{
	if (m_type == SODaSRequest::report)
	{
		return m_depReportData->getReportTimeStamp(timeStamp);
	}

	return FALSE;
} // getReportTimeStamp


//-----------------------------------------------------------------------------
// setReportQuality
//
// - set the report quality of the request
//
// returns:
//    TRUE  - quality set
//    FALSE - no report request
//
BOOL SODaSRequest::setReportQuality(IN WORD quality)
{
	if (m_type == SODaSRequest::report)
	{
		m_depReportData->setReportQuality(quality);
		return TRUE;
	}

	return FALSE;
} // setReportQuality


//-----------------------------------------------------------------------------
// getReportQuality
//
// - get the report quality of the request
//
// returns:
//    quality
//
WORD SODaSRequest::getReportQuality(void)
{
	if (m_type == SODaSRequest::report)
	{
		return m_depReportData->getReportQuality();
	}

	return OPC_QUALITY_BAD;
} // getReportQuality




//-----------------------------------------------------------------------------
// SODaSRequestSyncReadData                                                   |
//-----------------------------------------------------------------------------

SODaSRequestSyncReadData::SODaSRequestSyncReadData()
{
	m_resAddr = NULL;
	m_readQualityAddr = NULL;
#ifdef SOFEATURE_DCOM
	m_readValueAddr = NULL;
	m_itemStateAddr = NULL;
	m_readTimeStampAddr = NULL;
#endif
	m_maxAge = 0;
	m_itemTag = NULL;
#ifdef SOFEATURE_SOAP
	m_readValueVAddr = NULL;
	m_readTimeStampDTAddr = NULL;
#endif
	m_reqDatatype = VT_EMPTY;
}

SODaSRequestSyncReadData::~SODaSRequestSyncReadData()
{
	setItemTag(NULL);
}

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// setItemStateAddress
//
// - set the address of item state struct of the request
//
void SODaSRequestSyncReadData::setItemStateAddress(
	IN OPCITEMSTATE* itemStateAddr) // address of item state struct
{
	m_itemStateAddr = itemStateAddr;
} // setItemStateAddress

//-----------------------------------------------------------------------------
// getItemStateAddress
//
// - get the address of item state struct of the request
//
// returns:
//		address of item state struct
//
OPCITEMSTATE* SODaSRequestSyncReadData::getItemStateAddress(void)
{
	return m_itemStateAddr;
} // getItemStateAddress
#endif

//-----------------------------------------------------------------------------
// getResult
//
// - get the result of the request execution
//
// returns:
//		result
//
HRESULT SODaSRequestSyncReadData::getResult(void)
{
	if (m_resAddr)
	{
		return *m_resAddr;
	}
	else
	{
		return E_FAIL;
	}
} // getResult


//-----------------------------------------------------------------------------
// setResult
//
// - set the result of the request execution
//
void SODaSRequestSyncReadData::setResult(
	IN HRESULT res) // result
{
	if (m_resAddr)
	{
		*m_resAddr = res;
	}
} // setResult


//-----------------------------------------------------------------------------
// setResultAddress
//
// - set the result address of the request
// - only for synchron requests
//
void SODaSRequestSyncReadData::setResultAddress(
	IN HRESULT* resAddr)    // result address
{
	m_resAddr = resAddr;
}



//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous item tag
//
// returns:
//		TRUE
//
BOOL SODaSRequestSyncReadData::setItemTag(
	IN SODaSItemTag* itemTag)   // item tag
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLockV(this);
	SODaSItemTag* prevItemTag;
	pObjectLock->lock();
	prevItemTag = m_itemTag;

	if (itemTag)
	{
		itemTag->addRef();
	}

	m_itemTag = itemTag;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLockV(this, pObjectLock);

	if (prevItemTag)
	{
		prevItemTag->release();
	}

	return TRUE;
} // setItemTag


//-----------------------------------------------------------------------------
// getItemTag
//
// - get tag object
//
// returns:
//		pointer to tag object
//
SODaSItemTag* SODaSRequestSyncReadData::getItemTag(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);

	if (m_itemTag)
	{
		m_itemTag->addRef();
	}

	return m_itemTag;
} // getItemTag


//-----------------------------------------------------------------------------
// getCache
//
// - get cache object
//
// returns:
//		pointer to cache object
//
SODaSCache* SODaSRequestSyncReadData::getCache(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (itemTag.isNotNull())
	{
		return itemTag->getCache();
	}

	return NULL;
} // getCache


//-----------------------------------------------------------------------------
// setReqDatatype
//
// - set requested data type
//
// returns:
//		S_OK          - support data type
//		OPC_E_BADTYPE - not support data type
//
void SODaSRequestSyncReadData::setReqDatatype(
	IN VARTYPE reqDatatype) // requested data type
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
	HRESULT res;

	if (!itemTag)
	{
		setResult(E_FAIL);
		return;
	}

	if (FAILED(res = itemTag->supportReqDatatype(reqDatatype)))
	{
		setResult(res);
		return;
	}

	m_reqDatatype = reqDatatype;
} // setReqDatatype


//-----------------------------------------------------------------------------
// SODaSRequestSyncWriteData                                                  |
//-----------------------------------------------------------------------------

SODaSRequestSyncWriteData::SODaSRequestSyncWriteData()
{
	m_itemTag = NULL;
	m_resAddr = NULL;
	m_writeValue.Clear();
	m_writeQuality = 0;
	m_validQT = 0;
}

SODaSRequestSyncWriteData::~SODaSRequestSyncWriteData()
{
	m_writeValue.Clear();
	setItemTag(NULL);
}

//-----------------------------------------------------------------------------
// getResult
//
// - get the result of the request execution
//
// returns:
//		result
//
HRESULT SODaSRequestSyncWriteData::getResult(void)
{
	if (m_resAddr)
	{
		return *m_resAddr;
	}
	else
	{
		return E_FAIL;
	}
} // getResult


//-----------------------------------------------------------------------------
// setResult
//
// - set the result of the request execution
//
void SODaSRequestSyncWriteData::setResult(
	IN HRESULT res) // result
{
	if (m_resAddr)
	{
		*m_resAddr = res;
	}
} // setResult


//-----------------------------------------------------------------------------
// setResultAddress
//
// - set the result address of the request
// - only for synchron requests
//
void SODaSRequestSyncWriteData::setResultAddress(
	IN HRESULT* resAddr)    // result address
{
	m_resAddr = resAddr;
} // setResultAddress


//-----------------------------------------------------------------------------
// setWriteValue
//
// - set the write value of the request
//
// returns:
//		TRUE  - ok
//		FALSE - can't convert
//
BOOL SODaSRequestSyncWriteData::setWriteValue(
	IN LPCVARIANT value,    // write value
	IN LCID localeID,       // locale id
	IN SODaSItemTag* tag,   // item tag
	IN BOOL isSOAPRequest)
{
	if (value->vt == VT_EMPTY)
	{
		setResult(OPC_E_BADTYPE);
		return FALSE;
	}

	VARTYPE natDT = tag->getNativeDatatype();
	BOOL ok = TRUE;

	if ((natDT == value->vt) || (natDT == VT_EMPTY))
	{
		m_writeValue = value;
	}
	else
	{
		if (!((isSOAPRequest) && (value->vt == VT_BSTR)))
		{
			SOCmnPointer<SODaSCache> cache = tag->getCache();
			SOCmnVariant tmp;
			HRESULT convRes = S_OK;
			tmp = value;
			ok = cache->changeDatatype(tmp, natDT, tmp, localeID, tag, &convRes);

			if (ok)
			{
				m_writeValue = tmp;
			}
			else
			{
				setResult(convRes);
			}
		}
		else
		{
			// no conversion of string values on XML-DA
			setResult(OPC_E_BADTYPE);
		}
	}

	return ok;
} // setWriteValue


//-----------------------------------------------------------------------------
// getWriteValue
//
// - get the write value of the request
// - only for write requests
//
// returns:
//		TRUE  - ok
//		FALSE - no write value
//
BOOL SODaSRequestSyncWriteData::getWriteValue(
	OUT LPVARIANT value)    // write value
{
	::VariantInit(value);
	return (SOCmnVariant::variantCopy(value, &m_writeValue) == S_OK);
} // getWriteValue

//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous item tag
//
// returns:
//		TRUE
//
BOOL SODaSRequestSyncWriteData::setItemTag(
	IN SODaSItemTag* itemTag)   // item tag
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLockV(this);
	SODaSItemTag* prevItemTag;
	pObjectLock->lock();
	prevItemTag = m_itemTag;

	if (itemTag)
	{
		itemTag->addRef();
	}

	m_itemTag = itemTag;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLockV(this, pObjectLock);

	if (prevItemTag)
	{
		prevItemTag->release();
	}

	return TRUE;
} // setItemTag


//-----------------------------------------------------------------------------
// getItemTag
//
// - get tag object
//
// returns:
//		pointer to tag object
//
SODaSItemTag* SODaSRequestSyncWriteData::getItemTag(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);

	if (m_itemTag)
	{
		m_itemTag->addRef();
	}

	return m_itemTag;
} // getItemTag


//-----------------------------------------------------------------------------
// getCache
//
// - get cache object
//
// returns:
//		pointer to cache object
//
SODaSCache* SODaSRequestSyncWriteData::getCache(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (itemTag.isNotNull())
	{
		return itemTag->getCache();
	}

	return NULL;
} // getCache



//-----------------------------------------------------------------------------
// SODaSRequestAsyncReadData                                                  |
//-----------------------------------------------------------------------------

SODaSRequestAsyncReadData::SODaSRequestAsyncReadData()
{
	m_result = S_OK;
	m_maxAge = 0;
}

SODaSRequestAsyncReadData::~SODaSRequestAsyncReadData()
{
}



//-----------------------------------------------------------------------------
// SODaSRequestAsyncWriteData                                                 |
//-----------------------------------------------------------------------------

SODaSRequestAsyncWriteData::SODaSRequestAsyncWriteData()
{
	m_result = S_OK;
	m_writeValue.Clear();
	m_writeQuality = 0;
	m_validQT = 0;
}

SODaSRequestAsyncWriteData::~SODaSRequestAsyncWriteData()
{
	m_writeValue.Clear();
}

//-----------------------------------------------------------------------------
// setWriteValue
//
// - set the write value of the request
//
// returns:
//		TRUE  - ok
//		FALSE - can't convert
//
BOOL SODaSRequestAsyncWriteData::setWriteValue(
	IN LPCVARIANT value,    // write value
	IN LCID localeID,       // locale id
	IN SODaSItemTag* tag,   // item tag
	IN BOOL isSOAPRequest)
{
	if (value->vt == VT_EMPTY)
	{
		setResult(OPC_E_BADTYPE);
		return FALSE;
	}

	VARTYPE natDT = tag->getNativeDatatype();
	BOOL ok = TRUE;

	if ((natDT == value->vt) || (natDT == VT_EMPTY))
	{
		m_writeValue = value;
	}
	else
	{
		if (!((isSOAPRequest) && (value->vt == VT_BSTR)))
		{
			SOCmnPointer<SODaSCache> cache = tag->getCache();
			SOCmnVariant tmp;
			HRESULT convRes = E_FAIL;
			tmp = value;
			//  reset the ok flag for the cahce validation
			ok = FALSE;

			//  check if cache is valid
			if (cache.isNotNull())
			{
				//  Perform the datachange is safe now
				ok = cache->changeDatatype(tmp, natDT, tmp, localeID, tag, &convRes);
			}

			if (ok)
			{
				m_writeValue = tmp;
			}
			else
			{
				setResult(convRes);
			}
		}
		else
		{
			// no conversion of string values on XML-DA
			setResult(OPC_E_BADTYPE);
		}
	}

	return ok;
} // setWriteValue


//-----------------------------------------------------------------------------
// getWriteValue
//
// - get the write value of the request
//
// returns:
//		TRUE  - ok
//		FALSE - no write value
//
BOOL SODaSRequestAsyncWriteData::getWriteValue(
	OUT LPVARIANT value)    // write value
{
	::VariantInit(value);
	return (SOCmnVariant::variantCopy(value, &m_writeValue) == S_OK);
} // getWriteValue



//-----------------------------------------------------------------------------
// SODaSRequestDelegateData                                                   |
//-----------------------------------------------------------------------------

SODaSRequestDelegateData::SODaSRequestDelegateData()
{
	m_result = S_OK;
	m_itemTag = NULL;
	m_request = NULL;
}

SODaSRequestDelegateData::~SODaSRequestDelegateData()
{
	setItemTag(NULL);
	setOriginalRequest(NULL);
}

//-----------------------------------------------------------------------------
// setOriginalRequest
//
// - set the original request of the delegated request
//
// returns:
//		TRUE  - request completed
//		FALSE - can't complete request
//
BOOL SODaSRequestDelegateData::setOriginalRequest(
	IN SODaSRequest* req)   // request
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLockV(this);
	SODaSRequest* prevRequest;
	pObjectLock->lock();
	prevRequest = m_request;

	if (req)
	{
		req->addRef();
	}

	m_request = req;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLockV(this, pObjectLock);

	if (prevRequest)
	{
		prevRequest->release();
	}

	return TRUE;
} // setOriginalRequest


//-----------------------------------------------------------------------------
// getOriginalRequest
//
// - get the original request of the delegated request
//
// returns:
//		original request
//
SODaSRequest* SODaSRequestDelegateData::getOriginalRequest(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);

	if (m_request)
	{
		m_request->addRef();
	}

	return m_request;
} // getOriginalRequest


//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous item tag
//
// returns:
//		TRUE
//
BOOL SODaSRequestDelegateData::setItemTag(
	IN SODaSItemTag* itemTag)   // item tag
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLockV(this);
	SODaSItemTag* prevItemTag;
	pObjectLock->lock();
	prevItemTag = m_itemTag;

	if (itemTag)
	{
		itemTag->addRef();
	}

	m_itemTag = itemTag;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLockV(this, pObjectLock);

	if (prevItemTag)
	{
		prevItemTag->release();
	}

	return TRUE;
} // setItemTag


//-----------------------------------------------------------------------------
// getItemTag
//
// - get tag object
//
// returns:
//		pointer to tag object
//
SODaSItemTag* SODaSRequestDelegateData::getItemTag(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);

	if (m_itemTag)
	{
		m_itemTag->addRef();
	}

	return m_itemTag;
} // getItemTag


//-----------------------------------------------------------------------------
// getCache
//
// - get cache object
//
// returns:
//		pointer to cache object
//
SODaSCache* SODaSRequestDelegateData::getCache(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (itemTag.isNotNull())
	{
		return itemTag->getCache();
	}

	return NULL;
} // getCache


//-----------------------------------------------------------------------------
// getResult
//
// - get the result of the request execution
//
// returns:
//		result
//
HRESULT SODaSRequestDelegateData::getResult(void)
{
	return m_result;
} // getResult


//-----------------------------------------------------------------------------
// setResult
//
// - set the result of the request execution
//
void SODaSRequestDelegateData::setResult(
	IN HRESULT res) // result
{
	m_result = res;
} // setResult

BOOL SODaSRequestDelegateData::getWriteValue(OUT LPVARIANT value)
{
	SOCmnPointer<SODaSRequest> orgReq = getOriginalRequest();

	if (orgReq.isNotNull())
	{
		return orgReq->getWriteValue(value);
	}

	return FALSE;
} // getWriteValue

WORD SODaSRequestDelegateData::getWriteQuality(void)
{
	SOCmnPointer<SODaSRequest> orgReq = getOriginalRequest();

	if (orgReq.isNotNull())
	{
		return orgReq->getWriteQuality();
	}

	return OPC_QUALITY_BAD;
} // getWriteQuality

void SODaSRequestDelegateData::getWriteTimeStamp(OUT SOCmnDateTime& writeTimeStamp)
{
	SOCmnPointer<SODaSRequest> orgReq = getOriginalRequest();

	if (orgReq.isNotNull())
	{
		orgReq->getWriteTimeStamp(writeTimeStamp);
	}
} // getWriteTimeStamp

BYTE SODaSRequestDelegateData::getValidQT(void)
{
	SOCmnPointer<SODaSRequest> orgReq = getOriginalRequest();

	if (orgReq.isNotNull())
	{
		return orgReq->getValidWriteQT();
	}

	return 0;
} // getValidQT




//-----------------------------------------------------------------------------
// SODaSRequestReportData                                                     |
//-----------------------------------------------------------------------------

SODaSRequestReportData::SODaSRequestReportData()
{
	m_result = S_OK;
	m_quality = OPC_QUALITY_GOOD;
}

SODaSRequestReportData::~SODaSRequestReportData()
{
}

#endif // SODAS
