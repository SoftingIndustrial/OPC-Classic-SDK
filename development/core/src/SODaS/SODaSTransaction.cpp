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
//  Filename    : SODaSTransaction.cpp                                        |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Class for asynchonous transactions                          |
//                - SODaSTransaction: transaction                             |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "opcerror.h"
#include "SODaSRequest.h"
#include "SODaSItem.h"
#include "SODaSItemTag.h"
#include "SODaSCache.h"
#include "SODaSTransaction.h"
#include "SOCmnTrace.h"
#include "SODaSServer.h"
#include "SODaSEntry.h"
#ifdef SOFEATURE_DCOM
#include "SODaSComGroup.h"
#endif
#ifdef SOFEATURE_SOAP
#include "SODaSXmlServer.h"
#endif
#ifdef SOFEATURE_TUNNEL
#include "SODaSTPServer.h"
#endif

//-----------------------------------------------------------------------------
// SODaSTransaction                                                           |
//-----------------------------------------------------------------------------

SODaSTransaction::SODaSTransaction(
	IN SODaSGroup* group,                       // group
	IN BYTE type,                               // transaction type
	IN BYTE connectionType,                     // connection type
	IN OPTIONAL DWORD connection)               // connection id
	: SOCmnObject(SOCMNOBJECT_TYPE_TRANSACTION)
{
	m_group = group;
	setObjTraceId(NULL);
	m_type = type;
	m_connection = connection;
	setObjectState(SODaSTransaction::inactive);
	m_connectionType = connectionType;
	m_lastSendDataChangeTime.now();
	m_clientTransId = 0;

	if (type == SODaSTransaction::cyclic)
	{
		m_transactionId = 0;
	}
	else
	{
		if (m_group)
		{
			m_group->addRef();
		}

		m_transactionId = (DWORD)this->getOpcHandle();
	}

	m_reqStartedCnt = 0;
	m_reqList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT |
					 SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_flag = SODASTRANSACTION_FLAG_NO_CLIENT_TRANSACTION_ID;
#ifdef SOFEATURE_SOAP
	m_xmlMethod = NULL;
	m_waitTime = 0;
	m_holdTime = 0;
	m_reCheckSpan = SOCMN_TIME_40DAYS;
#endif
	m_traceAdditionOnUpdate = FALSE;
}


SODaSTransaction::~SODaSTransaction(void)
{
#ifdef SOFEATURE_SOAP

	if (m_xmlMethod != NULL)
	{
		m_xmlMethod->release();
		m_xmlMethod = NULL;
	}

#endif
	m_reqList.destroy();

	if (m_type != SODaSTransaction::cyclic)
		if (m_group)
		{
			m_group->release();
		}
}


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaSTransaction::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		SOCmnString type;
		SOCmnString version;

		switch (m_connectionType)
		{
		case SODaSTransaction::dataObject:
			version = _T(";V1)");
			break;

		case SODaSTransaction::connectionPoint:
			version = _T(";V2)");
			break;

		case SODaSTransaction::synchronous:
			version = _T(";SY)");
			break;

		case SODaSTransaction::asyncSOAPMethod:
			version = _T(";XML)");
			break;

		case SODaSTransaction::tunnel:
			version = _T(";TP)");
			break;

		default:
			version = _T(")");
			break;
		}

		switch (m_type)
		{
		case SODaSTransaction::cyclic:
			type = _T("cyclic");
			break;

		case SODaSTransaction::refresh:
			type = _T("refresh");
			break;

		case SODaSTransaction::asyncWrite:
			type = _T("write");
			break;

		case SODaSTransaction::asyncRead:
			type = _T("read");
			break;

		case SODaSTransaction::syncWrite:
			type = _T("write");
			break;

		case SODaSTransaction::syncRead:
			type = _T("read");
			break;

		case SODaSTransaction::serverRead:
			type = _T("reaD");
			break;

		case SODaSTransaction::polledRefresh:
			type = _T("polledRef");
			break;
		}

		if (m_group)
		{
			id.format(_T("[%s|%8.8lX| %s (%s%s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)m_group->getName(), (LPCTSTR)type, (LPCTSTR)version);
		}
		else
		{
			id.format(_T("[%s|%8.8lX| (%s%s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)type, (LPCTSTR)version);
		}

		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// getGroup
//
// - get group object of the transaction
//
// returns:
//		group object of the transaction
//
SODaSGroup* SODaSTransaction::getGroup(void)
{
	if (m_group)
	{
		m_group->addRef();
	}

	return m_group;
} // getGroup


//-----------------------------------------------------------------------------
// insertRequest
//
// - insert request to transaction
//
// returns:
//		TRUE  - request added
//		FALSE - can't insert request
//
BOOL SODaSTransaction::insertRequest(
	IN SODaSRequest* req)   // request
{
	SOCmnList<SODaSRequest> reqList(getRequestList());
	return reqList.add(req);
} // insertRequest


//-----------------------------------------------------------------------------
// removeRequest
//
// - remove request from transaction
//
// returns:
//		TRUE  - request removed
//		FALSE - can't remove request
//
BOOL SODaSTransaction::removeRequest(
	IN SODaSRequest* req)   // request
{
	req->addRef();
	SOCmnList<SODaSRequest> reqList(getRequestList());
	BOOL removeSucceded = reqList.removeObject(req);
	reqList.autoUnlock();

	if (removeSucceded)
	{
		req->cancel(S_OK);		
	}
	else
	{
		req->cancel();
	}
	req->release();

	return removeSucceded;

} // removeRequest


//-----------------------------------------------------------------------------
// getRequestByItem
//
// - get request by the item of the request
// - returns the first request in the list with the item
//
// returns:
//		pointer to request
//		NULL - no request for the item
//
SODaSRequest* SODaSTransaction::getRequestByItem(
	IN SODaSItem* srcItem)  // item
{
	SOCmnList<SODaSRequest> reqList(getRequestList());
	SOCmnPointer<SODaSItem> item;
	SODaSRequest* req;
	SOCmnListPosition pos;
	pos = reqList.getStartPosition();

	while (pos)
	{
		req = reqList.getNext(pos);
		item = req->getItem();

		if (((SODaSItem*)item) && (srcItem == (SODaSItem*)item))
		{
			req->addRef();
			return req;
		}
	}

	return NULL;
} // getRequestByItem


//-----------------------------------------------------------------------------
// getSyncTimeout
//
DWORD SODaSTransaction::getSyncTimeout(void)
{
	return 1000;
}   //  end getSyncTimeout


//-----------------------------------------------------------------------------
// start
//
// - start all requests of the transaction
//
void SODaSTransaction::start(IN SODaSServer* pServer)
{
	SOCmnList<SODaSRequest> reqList(getRequestList());
	SOCmnListPosition posReq;
	SODaSRequest* req;
	BOOL hasTransRequests = FALSE;

	if (isStarted())
	{
		// transaction not completed
		if ((m_flag & SODASTRANSACTION_FLAG_NO_CANCEL_ON_START) == 0)
		{
			// cancel not completed requests on restart
			posReq = reqList.getStartPosition();

			while (posReq)
			{
				req = reqList.getNext(posReq);

				if ((req->getType() != SODaSRequest::report) &&
					(!((req->isCompleted()) || (!req->isStarted()))))
				{
					// previous cyclic request running
					// -> cancel previous try, forget the new one
					req->cancel(E_FAIL, TRUE, OPC_QUALITY_LAST_USABLE);
					req->m_flag |= SODASREQUEST_FLAG_CANCELED;
				}

				// mark all requests to process
				req->setObjectState(req->getObjectState() | SODASREQUEST_FLAG_TOPROCESS);
			}

			reqList.autoUnlock();
		}
		else
		{
			posReq = reqList.getStartPosition();

			while (posReq)
			{
				req = reqList.getNext(posReq);

				if (req->isCompleted())
				{
					// process the completed requests
					req->setObjectState(req->getObjectState() | SODASREQUEST_FLAG_TOPROCESS);
				}
			}

			reqList.autoUnlock();
		}

		complete(pServer);
	}

	HRESULT res = rejectRequests(pServer);

	if (!FAILED(res))
	{
		posReq = reqList.getStartPosition();
		m_reqStartedCnt = reqList.getCount();

		while (posReq)
		{
			req = reqList.getNext(posReq);

			if (req->getType() != SODaSRequest::report)
			{
				if ((req->m_flag & SODASREQUEST_FLAG_CANCELED) == 0)
				{
					if (isCyclic() && req->isCompleted())
					{
						req->reset();
					}

					if (SUCCEEDED(req->getResult()))
					{
						req->start(FALSE);

						if (!hasTransRequests)
						{
							SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();

							if (itemTag.isNotNull())
								if (itemTag->getDeviceIOMode(req) == SODaSItemTag::transaction)
								{
									hasTransRequests = TRUE;
								}
						}
					}
					else
					{
						// error before start (bad type, bad rights, ...)
						req->complete(req->getResult(), FALSE, OPC_QUALITY_GOOD, TRUE);
					}
				}
				else
				{
					// the canceled request will be added the next time again
					req->m_flag &= ~SODASREQUEST_FLAG_CANCELED;
				}
			}
		}

		if (hasTransRequests)
		{
			handleRequests(getRequestList());
		}

		if (!reqList.isEmpty())
		{
			SODaSRequest::signalQueueEvents(&reqList);
			setObjectState(SODaSTransaction::active);
		}
	}
	else
	{
		posReq = reqList.getStartPosition();

		while (posReq)
		{
			req = reqList.getNext(posReq);
			req->complete(res, FALSE, OPC_QUALITY_BAD, TRUE);
		}

		setObjectState(SODaSTransaction::active);
	}
} // start


//-----------------------------------------------------------------------------
// rejectRequests
//
// - handle all transaction requests of the transaction
//
HRESULT SODaSTransaction::rejectRequests(
	IN SODaSServer* pServer)
{
	return S_OK;
}


//-----------------------------------------------------------------------------
// handleRequests
//
// - handle all transaction requests of the transaction
//
void SODaSTransaction::handleRequests(
	IN SOCmnList<SODaSRequest>* /* reqList */)
{
	_ASSERTION(FALSE, "SODaSTransaction::handleRequests must be implemented in child class for transaction I/O");
} // handleRequests


//-----------------------------------------------------------------------------
// cancelRequests
//
// - cancel all requests of the transaction
//
void SODaSTransaction::cancelRequests(void)
{
	SOCmnList<SODaSRequest> reqList(getRequestList());
	SOCmnListPosition posReq;
	SODaSRequest* req;
	posReq = reqList.getStartPosition();

	while (posReq)
	{
		req = reqList.getNext(posReq);
		req->cancel();
	}

	m_reqStartedCnt = 0;
} // cancelRequests

//-----------------------------------------------------------------------------
// notiofyCompletedInternally
//
// - notify upper layers that request was internally completed
//
void SODaSTransaction::notiofyCompletedInternally(IN SODaSRequest* pRequest)
{
	_ASSERTION(FALSE, "SODaSTransaction::notiofyCompletedInternally must be implemented in child class for transaction I/O");
} // notiofyCompletedInternally

//-----------------------------------------------------------------------------
// update
//
// - check if all requests of the transaction are completed
// - if all are completed call the client
//
// returns:
//		TRUE  - completed transaction
//		FALSE - not all requests of the transaction are completed
//
BOOL SODaSTransaction::update(
	IN SODaSServer* pServer,
	IN DWORD now,       // current time
	IN DWORD dueTime,   // maximal time for the requests
	IN OUT DWORD* span) // time in ms till next call is needed
{
	BOOL fire = FALSE;
	DWORD updSpan = INFINITE;

	if (isStarted())
	{
		SOCmnList<SODaSRequest> reqList(getRequestList());
		SOCmnListPosition posReq;
		SODaSRequest* req;

		if (getType() != SODaSTransaction::polledRefresh)
		{
			DWORD compCnt = 0;
			DWORD reqTime;
			// count the completed requests
			posReq = reqList.getStartPosition();

			while (posReq)
			{
				req = reqList.getNext(posReq);

				if (req->isStarted())
				{
					// request is started
					if (req->isCompleted())
					{
						// request is completed
						compCnt++;
					}
					else
					{
						if (dueTime != INFINITE)
						{
							// check max completion time
							reqTime = getTimeSpan(req->getStartTime(), now);

							if (reqTime < dueTime)
							{
								// calculate span to max completion time
								DWORD reqSpan = dueTime - reqTime;

								if (updSpan > reqSpan)
								{
									updSpan = reqSpan;
									break;
								}
							}
							else
							{
								// max completion time ellapsed
								// -> abort request
								req->cancel(E_FAIL, TRUE);
								compCnt++;
							}
						}
						else
						{
							// no max completion time
							break;
						}
					}
				}
				else
				{
					// handle not started requests as completed
					compCnt++;
				}
			}

			if (compCnt >= __min(m_reqStartedCnt, reqList.getCount()))
			{
				// all requests completed
				BOOL atLeastOneReqSuc = ((isCyclic()) && (m_group->getUpdateRate() == 0) && (!m_group->isNotifyAllChanges())) ? FALSE : TRUE;
				SOCmnEventPointer nullEvent;
				// mark all requests to process
				posReq = reqList.getStartPosition();

				while (posReq)
				{
					req = reqList.getNext(posReq);
					req->setObjectState(req->getObjectState() | SODASREQUEST_FLAG_TOPROCESS);

					if (req->getType() != SODaSRequest::cyclic)
					{
						req->setCompletedEvent(nullEvent);
					}

					if (!atLeastOneReqSuc)
					{
						SOCmnPointer<SODaSCache> cache = req->getCache();

						if (cache.isNotNull())
							if (((req->getFlag() & SODASREQUEST_FLAG_CACHE_AGE) == 0) && (SUCCEEDED(req->getResult())) && ((cache->getQuality() & OPC_QUALITY_GOOD) == OPC_QUALITY_GOOD))
							{
								SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();

								if (itemTag.isNotNull())
								{
									atLeastOneReqSuc = (itemTag->getDeviceIOMode(req) == SODaSItemTag::queue);
								}
							}
					}
				}

				reqList.autoUnlock();
				// complete transaction
				complete(pServer);
				fire = TRUE;

				if (isCyclic())
				{
					if ((m_group->getUpdateRate() == 0) && (!m_group->isNotifyAllChanges()))
					{
						SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_group->getParentBranch();

						if (atLeastOneReqSuc)
						{
							updSpan = 0;
						}
						else
						{
							updSpan = 100;
						}
					}
				}
			}
		}
		else
		{
			// type == polledRefresh
#ifdef SOFEATURE_SOAP
			if ((now >= m_holdTime) && ((now - m_holdTime) < SOCMN_TIME_40DAYS))
			{
				// hold time ellapsed
				if (now >= m_waitTime && ((now - m_waitTime) < SOCMN_TIME_40DAYS))
				{
					// wait time ellapsed
					complete(pServer);
					fire = TRUE;
					updSpan = 0;
				}
				else
				{
					// check for changes
					SOCmnPointer<SODaSCache> cache;
					SOCmnPointer<SODaSItem> item;
					SOCmnPointer<SODaSServer> server;
					WORD quality;
					SOCmnDateTime timeStamp;
					SOCmnVariant value;
					BOOL hasChanged = FALSE;
					posReq = reqList.getStartPosition();

					while (posReq)
					{
						req = reqList.getNext(posReq);
						item = req->getItem();
						cache = req->getCache();
						server = req->getServer();

						if ((cache.isNotNull()) && (item.isNotNull()) && (server.isNotNull()))
						{
							if (cache->get(value, &quality, &timeStamp, req, item->getReqDatatype(), server->getLCID(), NULL))
							{
								hasChanged = req->hasChanged(0, value, quality);
							}
							else
							{
								hasChanged = TRUE;
							}

							if (hasChanged)
							{
								break;
							}
						}
					}

					reqList.autoUnlock();

					if (hasChanged)
					{
						complete(pServer);
						fire = TRUE;
						updSpan = 0;
					}
					else
					{
						updSpan = m_waitTime - now;

						if (m_reCheckSpan < updSpan)
						{
							updSpan = m_reCheckSpan;
						}
					}
				}
			}
			else
			{
				updSpan = m_holdTime - now;
			}

#endif // SOFEATURE_SOAP
		}
	}

	if (span)
	{
		*span = updSpan;
	}

	return fire;
} // update


void SODaSTransaction::timeoutTransaction(IN SODaSServer* pServer, IN DWORD now, IN DWORD dueTime, OUT DWORD* span)
{
	return;
}

//-----------------------------------------------------------------------------
// complete
//
// - complete the transaction, call the client with the results of the I/O
//
BOOL SODaSTransaction::complete(IN SODaSServer* pServer)
{
#ifdef SOFLAG_DEMO_VERSION
#ifdef SOFEATURE_DCOM

	if ((pServer->getFlags() & SODASSERVER_FLAG_DCOM) == SODASSERVER_FLAG_DCOM)
	{
		// check advises only for DCOM servers !
		if (!(SOSRV_ADVISE_CHECK_DEMO_DA))
		{
			// set all report requests to processed
			SOCmnList<SODaSRequest> reqList(getRequestList());
			SOCmnListPosition posReq;
			SODaSRequest* req;
			posReq = reqList.getStartPosition();

			while (posReq)
			{
				req = reqList.getNext(posReq);

				if (req->getType() == SODaSRequest::report)
				{
					req->setObjectState(req->getObjectState() | SODASREQUEST_FLAG_PROCESSED);
				}
			}

			reqList.autoUnlock();
			return TRUE;
		}
	}

#endif
#ifdef SOFEATURE_TUNNEL

	if ((pServer->getFlags() & SODASSERVER_FLAG_TUNNEL) == SODASSERVER_FLAG_TUNNEL)
	{
		// check advises only for OPC Tunnel servers !
		if (!(SOSRV_TP_ADVISE_CHECK_DEMO))
		{
			// set all report requests to processed
			SOCmnList<SODaSRequest> reqList(getRequestList());
			SOCmnListPosition posReq;
			SODaSRequest* req;
			posReq = reqList.getStartPosition();

			while (posReq)
			{
				req = reqList.getNext(posReq);

				if (req->getType() == SODaSRequest::report)
				{
					req->setObjectState(req->getObjectState() | SODASREQUEST_FLAG_PROCESSED);
				}
			}

			reqList.autoUnlock();
			return TRUE;
		}
	}

#endif
#endif

	if (!isStarted())
	{
		return FALSE;
	}

	setObjectState(SODaSTransaction::inactive);
	m_lastCompletionTime.now();
	BOOL ret = TRUE;
#ifdef SOFEATURE_DCOM

	if ((m_connectionType == SODaSTransaction::allConnections) || (m_connectionType == SODaSTransaction::connectionPoint))
	{
		// OPC DA V2.0
		SOCmnListPosition pos;
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_CONNECTIONPOINT);
		{
			SODaSDataCallback advise;

			while (pos)
			{
				m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_CONNECTIONPOINT);

				if (m_type == SODaSTransaction::asyncWrite)
				{
					sendWriteData(&advise);
				}
				else
				{
					if (getRequestType() == SODaSRequest::asyncInitNotify)
					{
						Sleep(1);
						// Need a short delay for the CTT V2.15 (June 2008) to pass.
						// This CTT version has failures in the CheckUpdateRate tests if the data change callback
						// arrives in the client before the SetState call on the group returns.
						// Remove this Sleep after this bug in the CTT is fixed.
					}

					if ((m_group->getEnable()) || ((m_type != SODaSTransaction::cyclic) && (getRequestType() != SODaSRequest::asyncInitNotify)))
					{
						if (FAILED(sendReadData(&advise)))
						{
							ret = FALSE;
						}
					}
				}
			}
		}
		m_group->doAdviseUnlock();
	}

	if ((m_connectionType == SODaSTransaction::allConnections) || (m_connectionType == SODaSTransaction::dataObject))
	{
		// OPC DA V1.0
		SOCmnListPosition pos;
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_DATAOBJECT);
		{
			SODaSDataCallback advise;

			while (pos)
			{
				m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_DATAOBJECT);

				if ((m_type == SODaSTransaction::cyclic) || (m_connection == advise.m_connectionId) ||
					(m_connectionType == SODaSTransaction::allConnections))
				{
					if (m_type == SODaSTransaction::asyncWrite)
					{
						if (advise.m_format == OPCSTMFORMATWRITECOMPLETE)
						{
							sendWriteData(&advise);
						}
					}
					else
					{
						if ((advise.m_format == OPCSTMFORMATDATA) || (advise.m_format == OPCSTMFORMATDATATIME))
						{
							if (FAILED(sendReadData(&advise)))
							{
								ret = FALSE;
							}
						}
					}
				}
			}
		}
		m_group->doAdviseUnlock();
	}

#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_TUNNEL

	if ((m_connectionType == SODaSTransaction::allConnections) || (m_connectionType == SODaSTransaction::tunnel))
	{
		// OPC Tunnel
#ifdef SOFEATURE_DCOM
		SOCmnListPosition pos;
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_TUNNEL);

		while (pos)
		{
			SODaSDataCallback advise;
			m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_TUNNEL);

			if (m_type == SODaSTransaction::asyncWrite)
			{
				sendWriteDataTP(&advise);
			}
			else
			{
				if ((m_group->getEnable()) || ((m_type != SODaSTransaction::cyclic) && (getRequestType() != SODaSRequest::asyncInitNotify)))
				{
					if (FAILED(sendReadDataTP(&advise)))
					{
						ret = FALSE;
					}
				}
			}
		}

		m_group->doAdviseUnlock();
#else

		if (m_type == SODaSTransaction::asyncWrite)
		{
			sendWriteDataTP(NULL);
		}
		else
		{
			if ((m_group->getEnable()) || ((m_type != SODaSTransaction::cyclic) && (getRequestType() != SODaSRequest::asyncInitNotify)))
			{
				if (FAILED(sendReadDataTP(NULL)))
				{
					ret = FALSE;
				}
			}
		}

#endif
	}

#endif // SOFEATURE_TUNNEL
#ifdef SOFEATURE_SOAP

	if (m_connectionType == SODaSTransaction::asyncSOAPMethod)
	{
		// OPC XML DA 1.0
		SOCmnPointer<SODaSXmlServer> xmlServer = (SODaSXmlServer*)pServer->getHTTPReceiver();
		xmlServer->sendPolledRefreshResponse(pServer, m_xmlMethod, this);
	}

#endif // SOFEATURE_SOAP
	// mark all requests which were to process as processed
	SOCmnList<SODaSRequest> reqList(getRequestList());
	SOCmnListPosition posReq;
	SODaSRequest* req;
	BYTE reqObjState;
	posReq = reqList.getStartPosition();

	while (posReq)
	{
		req = reqList.getNext(posReq);
		reqObjState = req->getObjectState();

		if ((reqObjState & SODASREQUEST_FLAG_TOPROCESS) && (!(reqObjState & SODASREQUEST_FLAG_PROCESSED)))
		{
			req->setObjectState(reqObjState | SODASREQUEST_FLAG_PROCESSED);
		}
	}

	m_reqStartedCnt = 0;
	reqList.autoUnlock();

	// signal end of synchronous transaction
	if (m_connectionType == SODaSTransaction::synchronous)
	{
		m_completionEvent.signal();
	}

	return ret;
} // complete

void SODaSTransaction::execute(
	IN SODaSServer* pServer,
	SOCmnEvents* pEvents,
	DWORD reqEventId,
	DWORD dueTime)
{
	DWORD tacEventId = pEvents->create();
	DWORD waitRet;
	DWORD now;
	DWORD span;
	setCompletedEvent(SOCmnEventPointer(pEvents, tacEventId));
	start(pServer);

	do
	{
		waitRet = pEvents->waitForAll(FALSE, getTransactionTimeout());  //wait timeout

		if (waitRet == SOCMNEVENT_INVALID_ID)
		{
			// timeout
			m_traceAdditionOnUpdate = TRUE;
		}

		if (waitRet != tacEventId)
		{
			now = GetTickCount();
			span = INFINITE;
			update(pServer, now, dueTime, &span);
		}

		if (waitRet == SOCMNEVENT_INVALID_ID)
		{
			// timeout
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("SODaSTransaction::execute ; TIMEOUT")));

			if (getStopTraceOnTransactionTimeout() == TRUE)
			{
				// stop trace to file
				SOCmnTrace* trace = ::getSOCmnTrace();
				trace->m_lock.lock();
				trace->enableTraceToFile(FALSE);
				trace->m_lock.unlock();
			}

			cancelRequests();
			now = GetTickCount();
			timeoutTransaction(pServer, now, dueTime, &span);
		}
	}
	while ((waitRet != tacEventId) && (waitRet != SOCMNEVENT_INVALID_ID));
}

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// sendReadData
//
// - create OPC DA V2.0 read advise data
//
// returns:
//		S_OK          - data created at least one item
//		S_FALSE       - data created no items
//		E_OUTOFMEMORY - out of memory
//
HRESULT SODaSTransaction::sendReadData(
	IN SODaSDataCallback* advise)   // advise
{
	HRESULT res = S_OK;
	SOCmnPointer<SODaSItem> item;
	SODaSRequest* req;
	SOCmnListPosition pos = NULL;
	SOCmnListPosition actPos;
	SOCmnList<SODaSRequest> reqList(getRequestList());
	WORD quality;
	SOCmnDateTime timeStamp;
	SOCmnVariant value;
	LCID localeID;
	size_t connection = 0;
	DWORD itemCount;
	BOOL hasChanged;
	BYTE reqObjState;
	BOOL notifyAllChangesV;
	SOCmnString trcHelpSZ;
	USES_CONVERSION;
	ULONG i;
	WORD itemTL;
	BOOL isOnDataChange = (!((m_type == SODaSTransaction::asyncRead) && ((m_flag & SODASTRANSACTION_FLAG_NO_CLIENT_TRANSACTION_ID) == 0)));
	// V2.0
	DWORD groupClientHandle = m_group->getClientHandle();
	DWORD clientTransId = getClientTransactionId();
	HRESULT masterQuality = S_OK;
	HRESULT masterError = S_OK;
	OPCHANDLE* itemClientHandles = NULL;
	VARIANT* itemValues = NULL;
	WORD* itemQualities = NULL;
	FILETIME* itemTimeStamps = NULL;
	HRESULT* itemErrors = NULL;
	DWORD allocatedBufferIdx = 0;
	CComPtr<IMalloc> mallocIf;
	// V1.0
	SOCmnPointer<SODaSItemTag> tag;
	OPCGROUPHEADER* groupHeader = NULL;
	OPCITEMHEADER1* itemHeader = NULL;
	BYTE* pHeader = NULL;
	BYTE* pVariants = NULL;
	BYTE* pActHeader = NULL;
	BYTE* pActVariants = NULL;
	DWORD actHeaderOff = 0;
	DWORD actVariantsOff = 0;
	DWORD headerSize = 0;
	DWORD variantsSize = 0;
	DWORD itemHeaderSize = 0;
	VARIANT* pValue;
	DWORD valueSize;
	BOOL isBSTR;
	BOOL isARRAY;
	BYTE* pMemory;
	// get locale id
	localeID = m_group->getLCID();

#ifdef _WIN64
		// NOTE: OPCDA1.0 uses a memory block for transferring data between client and server.
		//       The memory contains structures with platform-dependent members.
		//       Sizeof(VARIANT) is 16 for 32-bit systems and 24 for 64-bit systems.
		//       Since OPC DA v1.0a dates back in 1997, it is a common agreement that OPCDA1.0 servers are 32-bit only.
		//       Remove this at your own risk!
	if (advise->m_connectionType != SODASDATACALLBACK_CT_CONNECTIONPOINT)
	{
		// V 1.0
		return E_INVALIDARG;
	}
#endif


	if (localeID == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
	{
		// no group specific lcid set
		SOCmnPointer<SODaSServer> srv((SODaSServer*)m_group->getParentBranch());

		if (srv.isNotNull())
		{
			localeID = srv->getLCID();
		}
	}

	::CoGetMalloc(MEMCTX_TASK, &mallocIf);

	do
	{
		//-- initialize locale variables and allocate buffers --
		masterError = S_OK;

		if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
		{
			// V 2.0
			if (isOnDataChange)
			{
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V2.0 data change callback")))
			}
			else
			{
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V2.0 read complete callback")))
			}

			connection = (size_t)advise->m_dataCallback;
			allocatedBufferIdx = 0;
			masterQuality = S_OK;
		}
		else
		{
			// V 1.0
			if (advise->m_format == OPCSTMFORMATDATA)
			{
				itemHeaderSize = sizeof(OPCITEMHEADER2);
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V1.0 data change callback")))
			}
			else
			{
				itemHeaderSize = sizeof(OPCITEMHEADER1);
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V1.0 data change callback (with timestamp)")))
			}

			// allocate memory
			pHeader = (BYTE*)malloc(1024);
			pVariants = (BYTE*)malloc(1024);

			if ((pHeader == NULL) || (pVariants == NULL))
			{
				return E_OUTOFMEMORY;
			}

			headerSize = 1024;
			variantsSize = 1024;
			pActHeader = pHeader;
			pActVariants = pVariants;
			actVariantsOff = 0;
			// group header
			groupHeader = (OPCGROUPHEADER*)pActHeader;
			groupHeader->dwSize = 0;
			groupHeader->hClientGroup = m_group->getClientHandle();
			groupHeader->dwTransactionID = m_transactionId;
			groupHeader->hrStatus = S_OK;
			groupHeader->dwItemCount = 0;
			pActHeader += sizeof(OPCGROUPHEADER);
			actHeaderOff = sizeof(OPCGROUPHEADER);
			connection = advise->m_connectionId;
		}

		//-- get item attributes and values to send --
		itemCount = 0;
		reqList.autoLock();
		notifyAllChangesV = ((m_type == SODaSTransaction::cyclic) && (m_group->isNotifyAllChanges() || notifyAllChanges()));

		if (pos == NULL)
		{
			reqList.setDirty(FALSE);
			pos = reqList.getStartPosition();
		}

		while (pos)
		{
//			if (notifyAllChangesV)
//			{ //-- check if list changed during last lock --
			// this is only possible for notifiy all changes mode and update rate 0
			// the list is not locked for all requests if one item is included twice
			// in the list. Then it's possible that the list may changes.
			if (reqList.isDirty())
			{
				// only possible if one item is twice in the request list
				// check current position!
				SOCmnListPosition checkPos;
				BOOL found = FALSE;
				checkPos = reqList.getStartPosition();

				while (checkPos)
				{
					if (checkPos == pos)
					{
						found = TRUE;
						break;
					}

					reqList.getNext(checkPos);
				}

				if (!found)
				{
					pos = reqList.getStartPosition();    // may lead to multiple notifications (but better than lose data)
				}

				reqList.setDirty(FALSE);

				if (pos == NULL)
				{
					break;
				}
			}

//			}
			//-- get the next request --
			actPos = pos;
			req = reqList.getNext(pos);
			reqObjState = req->getObjectState();

			if (!(reqObjState & SODASREQUEST_FLAG_TOPROCESS))
			{
				continue;    // only process the requests marked to process
			}

			item = req->getItem();
			itemTL = SOCMNTRACE_L_DEB;

			if (notifyAllChangesV && checkItemTwice())
			{
				//-- check if one item is twice in the request list --
				BOOL itemTwice = FALSE;
				BYTE* pCheckTwiceHeader = NULL;

				if (advise->m_connectionType == SODASDATACALLBACK_CT_DATAOBJECT)
				{
					pCheckTwiceHeader = (BYTE*)(groupHeader + 1);
				}

				for (i = 0; i < itemCount; i++)
				{
					if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
					{
						if (item->getClientHandle() == itemClientHandles[i])
						{
							itemTwice = TRUE;
							break;
						}
					}
					else if (pCheckTwiceHeader != NULL)
					{
						if (item->getClientHandle() == ((OPCITEMHEADER2*)pCheckTwiceHeader)->hClient)
						{
							itemTwice = TRUE;
							break;
						}

						pCheckTwiceHeader += itemHeaderSize;
					}
					else
					{
						//  ERROR
						break;
					}
				}

				if (itemTwice)
				{
					pos = actPos; // reset list position to current
					break;        // send the till now collected data
				}
			}

			//-- set the request to processed --
			req->setObjectState(reqObjState | SODASREQUEST_FLAG_PROCESSED);
			//-- check if value changed compared to last send value --
			value.clear();

			if (!notifyAllChangesV || req->getType() != SODaSRequest::report)
			{
				if (SUCCEEDED(req->getResult()))
				{
					SOCmnPointer<SODaSCache> cache;
					cache = req->getCache();

					if (cache.isNotNull())
					{
						HRESULT convRes = S_OK;

						if (!cache->get(value, &quality, &timeStamp, req, item->getReqDatatype(), localeID, &convRes))
							if (SUCCEEDED(req->getResult()))
							{
								req->setResult(convRes);
							}

						hasChanged = req->hasChanged(connection, value, quality);
					}
					else
					{
						quality = OPC_QUALITY_CONFIG_ERROR;
						timeStamp.now();
						hasChanged = FALSE;
					}
				}
				else
				{
					quality = 0;
					timeStamp.now();
					hasChanged = TRUE;

					if ((m_type != SODaSTransaction::asyncRead) && (req->getResult() == OPC_E_INVALIDHANDLE))
					{
						hasChanged = FALSE;
					}
				}
			}
			else
			{
				if (req->getType() == SODaSRequest::report)
				{
					if (SUCCEEDED(req->getResult()))
					{
						quality = req->getReportQuality();
						req->getReportValue(value);

						if (item->getReqDatatype() != VT_EMPTY)
						{
							HRESULT convRes = S_OK;

							if (!SOCmnVariant::changeDatatype(value, item->getReqDatatype(), localeID, &value, &convRes))
								if (SUCCEEDED(req->getResult()))
								{
									req->setResult(convRes);
								}
						}

						req->getReportTimeStamp(timeStamp);
						hasChanged = req->hasChanged(connection, value, quality);
					}
					else
					{
						hasChanged = TRUE;
						quality = 0;
						timeStamp.now();

						if ((m_type != SODaSTransaction::asyncRead) && (req->getResult() == OPC_E_INVALIDHANDLE))
						{
							hasChanged = FALSE;
						}
					}
				}
				else
				{
					_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE, (_T("update all changes with poll cyclic items")))
					quality = 0;
					timeStamp.now();
					hasChanged = FALSE;
				}
			}

			if (hasChanged)
			{
				//-- request value changed compared to last send value --
				if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
				{
					// V 2.0
					// (re)allocate the array
					if (itemCount >= allocatedBufferIdx)
					{
						allocatedBufferIdx += 50;
						itemValues = (VARIANT*)mallocIf->Realloc(itemValues, sizeof(VARIANT) * allocatedBufferIdx);

						if (itemValues)
						{
							for (i = itemCount; i < allocatedBufferIdx; i++)
							{
								::VariantInit(&itemValues[i]);
							}
						}

						itemClientHandles = (OPCHANDLE*)mallocIf->Realloc(itemClientHandles, sizeof(OPCHANDLE) * allocatedBufferIdx);
						itemQualities = (WORD*)mallocIf->Realloc(itemQualities, sizeof(WORD) * allocatedBufferIdx);
						itemTimeStamps = (FILETIME*)mallocIf->Realloc(itemTimeStamps, sizeof(FILETIME) * allocatedBufferIdx);
						itemErrors = (HRESULT*)mallocIf->Realloc(itemErrors, sizeof(HRESULT) * allocatedBufferIdx);

						if ((!itemClientHandles) || (!itemQualities) || (!itemTimeStamps) ||
							(!itemErrors) || (!itemValues))
						{
							if (itemClientHandles)
							{
								mallocIf->Free(itemClientHandles);
							}

							if (itemQualities)
							{
								mallocIf->Free(itemQualities);
							}

							if (itemTimeStamps)
							{
								mallocIf->Free(itemTimeStamps);
							}

							if (itemErrors)
							{
								mallocIf->Free(itemErrors);
							}

							if (itemValues)
							{
								for (i = 0; i < itemCount; i++)
								{
									SOCmnVariant::variantClear(&itemValues[i]);
								}

								mallocIf->Free(itemValues);
							}

							return E_OUTOFMEMORY;
						}
					}

					// copy the data
					itemClientHandles[itemCount] = item->getClientHandle();
					itemQualities[itemCount] = quality;
					timeStamp.get(&itemTimeStamps[itemCount]);
					value.copyTo(&itemValues[itemCount]);
					itemErrors[itemCount] = req->getResult();

					// cache request on inactive item -> quality is out of service
					if ((req->isCacheRequest()) && (!(item->getActive() && m_group->getActive())))
					{
						itemQualities[itemCount] = OPC_QUALITY_OUT_OF_SERVICE;
					}

					// if no good quality -> change master quality
					if ((itemQualities[itemCount] & OPC_QUALITY_GOOD) != OPC_QUALITY_GOOD)
					{
						masterQuality = S_FALSE;
						itemTL = SOCMNTRACE_L_INF;
					}

					// if not S_OK as error -> change master error
					if (itemErrors[itemCount] != S_OK)
					{
						masterError = S_FALSE;
						itemTL = SOCMNTRACE_L_ERR;
					}

					// set the last send value of the connection for last hasChanged check
					_TRACEC(itemTL, SOCMNTRACE_G_OPC_ADVISE, item->getObjTraceId(), (_T(" [%3.3d] send value:%s; quality:0x%X; [0x%X]"), itemCount, variantWithType2string(&trcHelpSZ, &itemValues[itemCount]), itemQualities[itemCount], itemErrors[itemCount]));

					if (isOnDataChange)
					{
						item->setSendValue(connection, value, itemQualities[itemCount]);
					}
				}
				else
				{
					// V 1.0
					VARTYPE natDT = VT_EMPTY;
					tag = req->getItemTag();

					if (tag.isNotNull())
					{
						natDT = tag->getNativeDatatype();
					}

					tag.release();

					//-- item header --
					if ((actHeaderOff + itemHeaderSize) > headerSize)
					{
						// realloc header
						headerSize += 1024;
						pHeader = (BYTE*)realloc(pHeader, headerSize);

						if (pHeader == NULL)
						{
							return E_OUTOFMEMORY;
						}

						pActHeader = pHeader + actHeaderOff;
						groupHeader = (OPCGROUPHEADER*)pHeader;
					}

					// set item header
					itemHeader = (OPCITEMHEADER1*)pActHeader;
					itemHeader->hClient = item->getClientHandle();
					itemHeader->dwValueOffset = actVariantsOff;
					itemHeader->wQuality = quality;
					itemHeader->wReserved = 0;

					if (advise->m_format == OPCSTMFORMATDATATIME)
					{
						timeStamp.get(&itemHeader->ftTimeStampItem);
					}

					// cache request on inactive item -> quality is out of service
					if ((req->isCacheRequest()) && (!(item->getActive() && m_group->getActive())))
					{
						itemHeader->wQuality = OPC_QUALITY_OUT_OF_SERVICE;
					}

					// if no good quality -> change group header status
					if ((itemHeader->wQuality & OPC_QUALITY_GOOD) != OPC_QUALITY_GOOD)
					{
						groupHeader->hrStatus = S_FALSE;
						itemTL = SOCMNTRACE_L_INF;
					}

					//-- item value --
					valueSize = SODaSCache::getValueSize(value);

					if ((actVariantsOff + valueSize) > variantsSize)
					{
						// realloc header
						variantsSize += (valueSize < 1024) ? 1024 : valueSize;
						pVariants = (BYTE*)realloc(pVariants, variantsSize);

						if (pVariants == NULL)
						{
							return E_OUTOFMEMORY;
						}

						pActVariants = pVariants + actVariantsOff;
					}

					pValue = (VARIANT*)(pActVariants);
					memcpy(pValue, &value, sizeof(VARIANT));
					isBSTR = ((item->getReqDatatype() == VT_BSTR) ||
							  ((item->getReqDatatype() == VT_EMPTY) &&
							   (natDT == VT_BSTR)));
					isARRAY = (((item->getReqDatatype() & VT_ARRAY) == VT_ARRAY) ||
							   ((item->getReqDatatype() == VT_EMPTY) &&
								((natDT & VT_ARRAY) == VT_ARRAY)));

					if (isBSTR)
					{
						// string
						DWORD* pBstrCount = (DWORD*)(pValue + 1);
						*pBstrCount = valueSize - sizeof(VARIANT) - 6;
						pValue->bstrVal = (BSTR)(pBstrCount + 1);
						wcscpy(pValue->bstrVal, value.bstrVal);
						pValue->bstrVal = NULL;
					}

					if (isARRAY)
					{
						SAFEARRAY* pArray = (SAFEARRAY*)(pValue + 1);
						BYTE* data = (BYTE*)(pArray + 1);
						ULONG dataSize = (value.parray->cbElements * value.parray->rgsabound[0].cElements);
						void* saData;
						memcpy(pArray, value.parray, sizeof(SAFEARRAY));
						pArray->pvData = NULL;
						SafeArrayAccessData(value.parray, &saData);
						memcpy(data, saData, dataSize);

						if (value.vt == (VT_BSTR | VT_ARRAY))
						{
							BSTR* bstr = (BSTR*)saData;
							BYTE* pActBArray = data + dataSize;
							DWORD* pBstrCount;
							BSTR pBstr;

							for (i = 0; i < value.parray->rgsabound[0].cElements; i++)
							{
								pBstrCount = (DWORD*)(pActBArray);
								pBstr = (BSTR)(pBstrCount + 1);
								*pBstrCount = (DWORD)(_tcslen(OLE2CT(bstr[i])) * 2);
								wcscpy(pBstr, bstr[i]);
								pActBArray += *pBstrCount + 6;
							}
						}

						SafeArrayUnaccessData(value.parray);
					}

					_TRACEC(itemTL, SOCMNTRACE_G_OPC_ADVISE, item->getObjTraceId(), (_T(" [%3.3d] send value:%s; quality:0x%X"), itemCount, variantWithType2string(&trcHelpSZ, &value), itemHeader->wQuality));
					item->setSendValue(connection, value, itemHeader->wQuality);
					pActHeader += itemHeaderSize;
					pActVariants += valueSize;
					actHeaderOff = (DWORD)(pActHeader - pHeader);
					actVariantsOff = (DWORD)(pActVariants - pVariants);
				}

				itemCount++;
			}
		} // end of while(pos)

		reqList.autoUnlock();

		//-- send the data to the client --
		if (itemCount > 0)
		{
			if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
			{
				// V 2.0
				if (!isOnDataChange)
				{
					_TRACE(masterError == S_OK ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("call read complete callback for %lu items [0x%X][0x%X]"), itemCount, masterError, masterQuality))
					SOCMN_TRY
					{
						res = advise->m_dataCallback->OnReadComplete(clientTransId, groupClientHandle, masterQuality, masterError,
						itemCount, itemClientHandles, itemValues, itemQualities, itemTimeStamps, itemErrors);

						_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCDataCallback::OnReadComplete [0x%X]"), res));

						if (SOSRV_IS_RPC_ERROR(res))
						{
							// client advise object not available anymore
							disconnectClient(); // -> disconnect client
						}
					}
					SOCMN_CATCH(...)
					{
						// exception in callback (only inproc)
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnReadComplete")));
						res = RPC_E_SERVERFAULT;
					}
				}
				else
				{
					_TRACE((masterError == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("call V2.0 data change callback for %lu items; [0x%X][0x%X]"), itemCount, masterError, masterQuality))
					SOCMN_TRY
					{
						m_lastSendDataChangeTime.now();
						res = advise->m_dataCallback->OnDataChange(clientTransId, groupClientHandle, masterQuality, masterError,
						itemCount, itemClientHandles, itemValues, itemQualities, itemTimeStamps, itemErrors);

						_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCDataCallback::OnDataChange [0x%X]"), res));

						if (SOSRV_IS_RPC_ERROR(res))
						{
							// client advise object not available anymore
							disconnectClient(); // -> disconnect client
						}
					}
					SOCMN_CATCH(...)
					{
						// exception in callback (only inproc)
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnDataChange")));
						res = RPC_E_SERVERFAULT;
					}
				}
			}
			else
			{
				// V1.0
				STGMEDIUM medium;
				FORMATETC formatetc;
				medium.tymed = TYMED_HGLOBAL;
				medium.pUnkForRelease = NULL;
				formatetc.cfFormat = advise->m_format;
				formatetc.dwAspect = DVASPECT_CONTENT;
				formatetc.ptd = NULL;
				formatetc.tymed = TYMED_HGLOBAL;
				formatetc.lindex = -1;
				// set item and size
				groupHeader->dwItemCount = itemCount;
				groupHeader->dwSize = actHeaderOff + actVariantsOff;
				// correct value offsets
				pActHeader = (BYTE*)(groupHeader + 1);

				for (i = 0; i < groupHeader->dwItemCount; i++)
				{
					itemHeader = (OPCITEMHEADER1*)pActHeader;
					itemHeader->dwValueOffset += actHeaderOff;
					pActHeader += itemHeaderSize;
				}

				// copy data to HGLOBAL
				medium.hGlobal = GlobalAlloc(GMEM_MOVEABLE, groupHeader->dwSize);

				if (medium.hGlobal == NULL)
				{
					return E_OUTOFMEMORY;
				}

				pMemory = (BYTE*)GlobalLock(medium.hGlobal);
				memcpy(pMemory, pHeader, actHeaderOff);
				memcpy(pMemory + actHeaderOff, pVariants, actVariantsOff);
				GlobalUnlock(medium.hGlobal);
				_TRACE((groupHeader->hrStatus == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("call V1.0 data change callback for %lu items; result: 0x%X"), groupHeader->dwItemCount, groupHeader->hrStatus))
				// send advise
				SOCMN_TRY
				{
					if (m_type == SODaSTransaction::cyclic)
					{
						m_lastSendDataChangeTime.now();
					}

					advise->m_adviseSink->OnDataChange(&formatetc, &medium);
					res = S_OK;
				}
				SOCMN_CATCH(...)
				{
					// exception in callback (only inproc)
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnDataChange V1(read)")));
					res = RPC_E_SERVERFAULT;
				}
				::ReleaseStgMedium(&medium);
			}

			// new update time
			if (isOnDataChange)
			{
				SOCmnPointer<SODaSServer> srv = (SODaSServer*)m_group->getParentBranch();

				if (srv.isNotNull())
				{
					srv->m_lastUpdateTime.now();
#ifdef SOFEATURE_WATCH

					if (srv->getCollectChanges() & SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES)
					{
						SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
						SOCmnString buffer;
						SOSrvWatch::attributeChange(srv, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, srv->m_lastUpdateTime.toString(buffer, conv->getLCID()));
					}

#endif
				}

				m_group->onDataChange(this, advise);
			}
			else
			{
				m_group->onReadComplete(this, advise);
			}
		}
		else
		{
			res = S_FALSE;
		}

		//-- free buffers --
		if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
		{
			// V 2.0
			// free the allocated memory
			if (itemClientHandles)
			{
				mallocIf->Free(itemClientHandles);
			}

			if (itemQualities)
			{
				mallocIf->Free(itemQualities);
			}

			if (itemTimeStamps)
			{
				mallocIf->Free(itemTimeStamps);
			}

			if (itemErrors)
			{
				mallocIf->Free(itemErrors);
			}

			if (itemValues)
			{
				for (i = 0; i < itemCount; i++)
				{
					SOCmnVariant::variantClear(&itemValues[i]);
				}

				mallocIf->Free(itemValues);
			}

			itemClientHandles = NULL;
			itemQualities = NULL;
			itemTimeStamps = NULL;
			itemErrors = NULL;
			itemValues = NULL;
		}
		else
		{
			// V 1.0
			if (pHeader)
			{
				free(pHeader);
			}

			if (pVariants)
			{
				free(pVariants);
			}

			pHeader = NULL;
			pVariants = NULL;
		}
	}
	while ((pos != NULL) && (notifyAllChangesV) && (SUCCEEDED(res)));   // only on report and zero update rate groups it's possible to have pos != NULL

	return res;
} // sendReadData


//-----------------------------------------------------------------------------
// sendWriteData
//
// - create OPC DA V2.0 write advise data
//
// returns:
//		S_OK          - data created at least one item
//		S_FALSE       - data created no items
//		E_OUTOFMEMORY - out of memory
//
HRESULT SODaSTransaction::sendWriteData(
	IN SODaSDataCallback* advise)   // advise
{
	SODaSRequest* req;
	SOCmnPointer<SODaSItem> item;
	SOCmnListPosition pos;
	SOCmnList<SODaSRequest> reqList(getRequestList());
	HRESULT res = S_OK;
	HRESULT reqRes;
	BYTE reqObjState;
	DWORD nrReqs = getRequestCount();
	// V2.0
	HRESULT masterError = S_OK;
	CComPtr<IMalloc> mallocIf;
	DWORD itemCount = 0;
	OPCHANDLE* itemClientHandles = NULL;
	HRESULT* itemErrors = NULL;
	DWORD groupClientHandle = m_group->getClientHandle();
	DWORD clientTransId = getClientTransactionId();
	// V1.0
	DWORD size;
	OPCGROUPHEADERWRITE* groupHeader = NULL;
	OPCITEMHEADERWRITE* itemHeader = NULL;
	BYTE* actPtr = NULL;
	STGMEDIUM medium;
	medium.hGlobal = NULL;

#ifdef _WIN64
	// NOTE: OPCDA1.0 uses a memory block for transferring data between client and server.
	//       The memory contains structures with platform-dependent members.
	//       Sizeof(VARIANT) is 16 for 32-bit systems and 24 for 64-bit systems.
	//       Since OPC DA v1.0a dates back in 1997, it is a common agreement that OPCDA1.0 servers are 32-bit only.
	//       Remove this at your own risk!
	if (advise->m_connectionType != SODASDATACALLBACK_CT_CONNECTIONPOINT)
	{
		return E_INVALIDARG;
	}
#endif

	if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V2.0 write complete callback")))
		::CoGetMalloc(MEMCTX_TASK, &mallocIf);
		// allocate memory
		itemClientHandles = (OPCHANDLE*)mallocIf->Alloc(sizeof(OPCHANDLE) * nrReqs);
		itemErrors = (HRESULT*)mallocIf->Alloc(sizeof(HRESULT) * nrReqs);

		if ((!itemClientHandles) || (!itemErrors))
		{
			if (itemClientHandles)
			{
				mallocIf->Free(itemClientHandles);
			}

			if (itemErrors)
			{
				mallocIf->Free(itemErrors);
			}

			return E_OUTOFMEMORY;
		}
	}
	else
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process V1.0 write complete callback")));
		// allocate memory
		size = sizeof(OPCGROUPHEADERWRITE) + (nrReqs * (sizeof(OPCITEMHEADERWRITE)));
		medium.hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);

		if (medium.hGlobal == NULL)
		{
			return E_OUTOFMEMORY;
		}

		actPtr = (BYTE*)GlobalLock(medium.hGlobal);
		// group header
		groupHeader = (OPCGROUPHEADERWRITE*)actPtr;
		groupHeader->hClientGroup = m_group->getClientHandle();
		groupHeader->dwTransactionID = m_transactionId;
		groupHeader->hrStatus = S_OK;
		groupHeader->dwItemCount = nrReqs;
		actPtr += sizeof(OPCGROUPHEADERWRITE);
	}

	// item header
	pos = reqList.getStartPosition();

	while (pos)
	{
		req = reqList.getNext(pos);
		reqObjState = req->getObjectState();

		if (!(reqObjState & SODASREQUEST_FLAG_TOPROCESS))
		{
			continue;    // only process the requests marked to process
		}

		req->setObjectState(reqObjState | SODASREQUEST_FLAG_PROCESSED);
		item = req->getItem();
		reqRes = req->getResult();
		_TRACEC((reqRes == S_OK) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, item->getObjTraceId(), (_T(" [%3.3d] send result:0x%X"), itemCount, reqRes));

		if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
		{
			// copy the data
			itemClientHandles[itemCount] = item->getClientHandle();
			itemErrors[itemCount] = req->getResult();
		}
		else
		{
			itemHeader = (OPCITEMHEADERWRITE*)actPtr;
			itemHeader->hClient = item->getClientHandle();
			itemHeader->dwError = reqRes;
			actPtr += sizeof(OPCITEMHEADERWRITE);
		}

		itemCount ++;

		// if not S_OK as error -> change master error
		if (reqRes != S_OK)
		{
			masterError = S_FALSE;
		}
	}

	reqList.autoUnlock();

	if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
	{
		_TRACE((masterError == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("call V2.0 write complete callback for %lu items; result: 0x%X"), itemCount, masterError))
		SOCMN_TRY
		{
			res = advise->m_dataCallback->OnWriteComplete(clientTransId, groupClientHandle, masterError,
			itemCount, itemClientHandles, itemErrors);
			_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCDataCallback::OnWriteComplete [0x%X]"), res));

			if (SOSRV_IS_RPC_ERROR(res))
			{
				// client advise object not available anymore
				disconnectClient(); // -> disconnect client
			}
		}
		SOCMN_CATCH(...)
		{
			// exception in callback (only inproc)
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnWriteComplete")));
			res = RPC_E_SERVERFAULT;
		}

		// free the allocated memory
		if (itemClientHandles)
		{
			mallocIf->Free(itemClientHandles);
		}

		if (itemErrors)
		{
			mallocIf->Free(itemErrors);
		}
	}
	else
	{
		FORMATETC formatetc;
		_TRACE((masterError == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("call V1.0 write complete callback for %lu items; result: 0x%X"), groupHeader->dwItemCount, masterError))
		groupHeader->hrStatus = masterError;
		// unlock memory
		GlobalUnlock(medium.hGlobal);
		formatetc.dwAspect = DVASPECT_CONTENT;
		formatetc.ptd = NULL;
		formatetc.tymed = TYMED_HGLOBAL;
		formatetc.lindex = -1;
		formatetc.cfFormat = advise->m_format;
		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = NULL;
		// send advise
		SOCMN_TRY
		{
			advise->m_adviseSink->OnDataChange(&formatetc, &medium);
		}
		SOCMN_CATCH(...)
		{
			// exception in callback (only inproc)
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnDataChange V1(write)")));
		}
		::ReleaseStgMedium(&medium);
	}

	m_group->onWriteComplete(this, advise);
	return res;
} // sendWriteData

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_TUNNEL
HRESULT SODaSTransaction::sendReadDataTP(IN SODaSDataCallback* advise)
{
	HRESULT res = S_OK;
	SOCmnPointer<SODaSItem> item;
	SODaSRequest* req;
	SOCmnListPosition pos = NULL;
	SOCmnListPosition actPos;
	SOCmnList<SODaSRequest> reqList(getRequestList());
	WORD quality;
	SOCmnDateTime timeStamp;
	SOCmnVariant value;
	LCID localeID;
	DWORD connection = 0;
	DWORD itemCount;
	BOOL hasChanged;
	BYTE reqObjState;
	BOOL notifyAllChangesV;
	SOCmnString trcHelpSZ;
	WORD itemTL;
	HRESULT masterQuality = S_OK;
	HRESULT masterError = S_OK;
	DWORD clientTransId = getClientTransactionId();
	SOCmnPointer<SODaSServer> server((SODaSServer*)m_group->getParentBranch());
	SOCmnPointer<SOCmnTPCall> tpCall;
	SOCmnPointer<SOSrvTPConnection> tpCon;
	SOCmnPointer<SOCmnTP_Marshaler> notification;
	BOOL isOnDataChange = (m_type != SODaSTransaction::asyncRead);

	if (!server)
	{
		return E_FAIL;
	}

	tpCon = server->getTPConnection();

	if (!tpCon)
	{
		return E_FAIL;
	}

	// get locale id
	localeID = m_group->getLCID();

	if (localeID == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
	{
		// no group specific lcid set
		localeID = server->getLCID();
	}

	do
	{
		//-- initialize locale variables and allocate buffers --
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SOCmnTPCall(tpCon), (SOCmnTPCall*));
		notification = tpCall->getSndMsg();
		notification->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
		notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
		notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
		notification->MarshalLong(m_group->getClientHandle());            // object id
		notification->MarshalShort(TP_INTERFACE_DA_CALLBACK);             // interface id
		masterError = S_OK;
		masterQuality = S_OK;

		if (isOnDataChange)
		{
			notification->MarshalChar(TP_FUNCTION_DA_CALLBACK_ONDATACHANGE);   // function id
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process TP data change callback")))
		}
		else
		{
			notification->MarshalChar(TP_FUNCTION_DA_CALLBACK_ONREADCOMPLETE); // function id
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process TP read complete callback")))
		}

		notification->MarshalChar(0);                                     // flags
		notification->MarshalLong(clientTransId);
		// pro forma marshaling the correct values will be written before sending
		notification->MarshalLong(masterError);
		notification->MarshalLong(masterQuality);
		notification->MarshalLong(0);                                     // item count ???
		//-- get item attributes and values to send --
		itemCount = 0;
		reqList.autoLock();
		notifyAllChangesV = ((m_type == SODaSTransaction::cyclic) && (m_group->isNotifyAllChanges() || notifyAllChanges()));
		DWORD i = 0;
		DWORD itemClientHandlesCount = reqList.getCount();
		DWORD* itemClientHandles = new DWORD[itemClientHandlesCount];

		if (pos == NULL)
		{
			reqList.setDirty(FALSE);
			pos = reqList.getStartPosition();
		}

		while (pos)
		{
//			if (notifyAllChangesV)
//			{ //-- check if list changed during last lock --
			// this is only possible for notifiy all changes mode and update rate 0
			// the list is not locked for all requests if one item is included twice
			// in the list. Then it's possible that the list may changes.
			if (reqList.isDirty())
			{
				// only possible if one item is twice in the request list
				// check current position!
				SOCmnListPosition checkPos;
				BOOL found = FALSE;
				checkPos = reqList.getStartPosition();

				while (checkPos)
				{
					if (checkPos == pos)
					{
						found = TRUE;
						break;
					}

					reqList.getNext(checkPos);
				}

				if (!found)
				{
					pos = reqList.getStartPosition();    // may lead to multiple notifications (but better than lose data)
				}

				reqList.setDirty(FALSE);

				if (pos == NULL)
				{
					break;
				}
			}

//			}
			//-- get the next request --
			actPos = pos;
			req = reqList.getNext(pos);
			reqObjState = req->getObjectState();

			if (!(reqObjState & SODASREQUEST_FLAG_TOPROCESS))
			{
				continue;    // only process the requests marked to process
			}

			item = req->getItem();
			itemTL = SOCMNTRACE_L_DEB;

			if (notifyAllChangesV && checkItemTwice())
			{
				//-- check if one item is twice in the request list --
				BOOL itemTwice = FALSE;

				for (i = 0; i < itemCount; i++)
				{
					if (advise->m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
					{
						if (i < itemClientHandlesCount &&
							item->getClientHandle() == itemClientHandles[i])
						{
							itemTwice = TRUE;
							break;
						}
					}
					else
					{
						break;
					}
				}

				if (itemTwice)
				{
					pos = actPos; // reset list position to current
					break;        // send the till now collected data
				}
			}

			//-- set the request to processed --
			req->setObjectState(reqObjState | SODASREQUEST_FLAG_PROCESSED);
			//-- check if value changed compared to last send value --
			value.clear();

			if (!notifyAllChangesV || req->getType() != SODaSRequest::report)
			{
				if (SUCCEEDED(req->getResult()))
				{
					SOCmnPointer<SODaSCache> cache;
					cache = req->getCache();

					if (cache.isNotNull())
					{
						HRESULT convRes = S_OK;

						if (!cache->get(value, &quality, &timeStamp, req, item->getReqDatatype(), localeID, &convRes))
							if (SUCCEEDED(req->getResult()))
							{
								req->setResult(convRes);
							}

						hasChanged = req->hasChanged(connection, value, quality);
					}
					else
					{
						quality = OPC_QUALITY_CONFIG_ERROR;
						timeStamp.now();
						hasChanged = FALSE;
					}
				}
				else
				{
					quality = 0;
					timeStamp.now();
					hasChanged = TRUE;
				}
			}
			else
			{
				if (req->getType() == SODaSRequest::report)
				{
					if (SUCCEEDED(req->getResult()))
					{
						quality = req->getReportQuality();
						req->getReportValue(value);

						if (item->getReqDatatype() != VT_EMPTY)
						{
							HRESULT convRes = S_OK;

							if (!SOCmnVariant::changeDatatype(value, item->getReqDatatype(), localeID, &value, &convRes))
								if (SUCCEEDED(req->getResult()))
								{
									req->setResult(convRes);
								}
						}

						req->getReportTimeStamp(timeStamp);
						hasChanged = req->hasChanged(connection, value, quality);
					}
					else
					{
						hasChanged = TRUE;
						quality = 0;
						timeStamp.now();

						if ((m_type != SODaSTransaction::asyncRead) && (req->getResult() == OPC_E_INVALIDHANDLE))
						{
							hasChanged = FALSE;
						}
					}
				}
				else
				{
					_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE, (_T("update all changes with poll cyclic items")))
					quality = 0;
					timeStamp.now();
					hasChanged = FALSE;
				}
			}

			if (hasChanged)
			{
				//-- request value changed compared to last send value --
				// cache request on inactive item -> quality is out of service
				if ((req->isCacheRequest()) && (!(item->getActive() && m_group->getActive())))
				{
					quality = OPC_QUALITY_OUT_OF_SERVICE;
				}

				notification->MarshalLong(item->getClientHandle());
				notification->MarshalLong(req->getResult());
				notification->MarshalVariant(value);
				notification->MarshalShort(quality);
				notification->MarshalDate(timeStamp);

				// if no good quality -> change master quality
				if ((quality & OPC_QUALITY_GOOD) != OPC_QUALITY_GOOD)
				{
					masterQuality = S_FALSE;
					itemTL = SOCMNTRACE_L_INF;
				}

				// if not S_OK as error -> change master error
				if (req->getResult() != S_OK)
				{
					masterError = S_FALSE;
					itemTL = SOCMNTRACE_L_ERR;
				}

				// set the last send value of the connection for last hasChanged check
				SOCmnString t;
				FILETIME FT;
				timeStamp.get(&FT);
				_TRACEC(itemTL, SOCMNTRACE_G_OPC_ADVISE, item->getObjTraceId(), (_T(" [%3.3ld] send value:%s; quality:0x%X [0x%X], timestamp: %s"), itemCount, variantWithType2string(&trcHelpSZ, value), (WORD)quality, req->getResult(), filetime2string(&t, &FT)));

				if (isOnDataChange)
				{
					item->setSendValue(connection, value, quality);
				}

				if (itemCount < itemClientHandlesCount)
				{
					itemClientHandles[itemCount] = item->getClientHandle();
				}

				itemCount++;
			}
		} // end of while(pos)

		delete [] itemClientHandles;
		reqList.autoUnlock();

		//-- send the data to the client --
		if (itemCount > 0)
		{
			notification->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
			notification->MarshalLong(masterError);
			notification->MarshalLong(masterQuality);
			notification->MarshalLong(itemCount);

			if (isOnDataChange)
			{
				_TRACE(masterError == S_OK ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("TPDA::Group::OnDataChange %lu [0x%lX][0x%lX]"), itemCount, (DWORD)masterError, (DWORD)masterQuality))
			}
			else
			{
				_TRACE(masterError == S_OK ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("TPDA::Group::OnReadComplete %lu [0x%lX][0x%lX]"), itemCount, (DWORD)masterError, (DWORD)masterQuality))
			}

			if (isSyncronisationNeeded() == TRUE)
			{
				// needed for storeAndForward detection !!!
				tpCall->setTimeout(getSyncTimeout());
				res = tpCon->invoke(tpCall, TRUE);

				if (FAILED(res))
				{
					// can't send notification
					// save lost values to items
					server->forceDisconnect(_T(""));
					tpCon->close();
					_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE, (_T("senReadDataTP failed to send notification: [0x%X]"), res))
					break;
				}
			}
			else
			{
				res = tpCon->invoke(tpCall, FALSE);

				if (FAILED(res))
				{
					// can't send notification
					tpCon->close(); // -> disconnect client
				}
			}

			// new update time
			if (isOnDataChange)
			{
				server->m_lastUpdateTime.now();
#ifdef SOFEATURE_WATCH

				if (server->getCollectChanges() & SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES)
				{
					SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
					SOCmnString buffer;
					SOSrvWatch::attributeChange(server, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, server->m_lastUpdateTime.toString(buffer, conv->getLCID()));
				}

#endif
				m_group->onDataChange(this, advise);
			}
			else
			{
				m_group->onReadComplete(this, advise);
			}
		}
		else
		{
			res = S_FALSE;
		}
	}
	while ((pos != NULL) && (notifyAllChangesV) && (SUCCEEDED(res)));   // only on report and zero update rate groups it's possible to have pos != NULL

	return res;
}

HRESULT SODaSTransaction::sendWriteDataTP(IN SODaSDataCallback* advise)
{
	SODaSRequest* req;
	SOCmnPointer<SODaSItem> item;
	SOCmnListPosition pos;
	SOCmnList<SODaSRequest> reqList(getRequestList());
	HRESULT res = S_OK;
	HRESULT reqRes;
	BYTE reqObjState;
	DWORD itemCount = 0;
	HRESULT masterError = S_OK;
	DWORD clientTransId = getClientTransactionId();
	SOCmnPointer<SODaSServer> server((SODaSServer*)m_group->getParentBranch());
	SOCmnPointer<SOCmnTPCall> tpCall;
	SOCmnPointer<SOSrvTPConnection> tpCon;
	SOCmnPointer<SOCmnTP_Marshaler> notification;

	if (!server)
	{
		return E_FAIL;
	}

	tpCon = server->getTPConnection();

	if (!tpCon)
	{
		return E_FAIL;
	}

	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process TP write complete callback")))
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOCmnTPCall(tpCon), (SOCmnTPCall*));
	notification = tpCall->getSndMsg();
	masterError = S_OK;
	notification->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
	notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	notification->MarshalLong(m_group->getClientHandle());            // object id
	notification->MarshalShort(TP_INTERFACE_DA_CALLBACK);                // interface id
	notification->MarshalChar(TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE);  // function id
	notification->MarshalChar(0);                                     // flags
	notification->MarshalLong(clientTransId);
	// pro forma marshaling the correct values will be written before sending
	notification->MarshalLong(masterError);
	notification->MarshalLong(0);                                     // item count
	// item header
	pos = reqList.getStartPosition();

	while (pos)
	{
		req = reqList.getNext(pos);
		reqObjState = req->getObjectState();

		if (!(reqObjState & SODASREQUEST_FLAG_TOPROCESS))
		{
			continue;    // only process the requests marked to process
		}

		req->setObjectState(reqObjState | SODASREQUEST_FLAG_PROCESSED);
		item = req->getItem();
		reqRes = req->getResult();
		_TRACEC((reqRes == S_OK) ? SOCMNTRACE_L_DEB : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, item->getObjTraceId(), (_T(" [%3.3ld] send result:0x%lX"), itemCount, (DWORD)reqRes));
		notification->MarshalLong(item->getClientHandle());
		notification->MarshalLong(req->getResult());
		itemCount ++;

		// if not S_OK as error -> change master error
		if (reqRes != S_OK)
		{
			masterError = S_FALSE;
		}
	}

	reqList.autoUnlock();

	//-- send the data to the client --
	if (itemCount > 0)
	{
		notification->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
		notification->MarshalLong(masterError);
		notification->MarshalLong(itemCount);
		_TRACE((masterError == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("TPDA::Group::OnReadComplete %lu [0x%lX]"), itemCount, (DWORD)masterError))
		res = tpCon->invoke(tpCall, FALSE);

		if (FAILED(res))
		{
			// can't send notification
			tpCon->close(); // -> disconnect client
		}
	}

	m_group->onWriteComplete(this, advise);
	return res;
}


HRESULT SODaSTransaction::sendKeepAliveTP(
	IN SODaSDataCallback* /* advise */)
{
	HRESULT res = S_OK;
	LCID localeID;
	SOCmnString trcHelpSZ;
	HRESULT masterQuality = S_OK;
	HRESULT masterError = S_OK;
	DWORD clientTransId = getClientTransactionId();
	SOCmnPointer<SODaSServer> server((SODaSServer*)m_group->getParentBranch());
	SOCmnPointer<SOCmnTPCall> tpCall;
	SOCmnPointer<SOSrvTPConnection> tpCon;
	SOCmnPointer<SOCmnTP_Marshaler> notification;
	BOOL isOnDataChange = (m_type != SODaSTransaction::asyncRead);

	if (!server)
	{
		return E_FAIL;
	}

	tpCon = server->getTPConnection();

	if (!tpCon)
	{
		return E_FAIL;
	}

	// get locale id
	localeID = m_group->getLCID();

	if (localeID == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
	{
		// no group specific lcid set
		localeID = server->getLCID();
	}

	//-- initialize locale variables and allocate buffers --
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOCmnTPCall(tpCon), (SOCmnTPCall*));
	notification = tpCall->getSndMsg();
	notification->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
	notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	notification->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	notification->MarshalLong(m_group->getClientHandle());            // object id
	notification->MarshalShort(TP_INTERFACE_DA_CALLBACK);             // interface id
	masterError = S_OK;
	masterQuality = S_OK;

	if (isOnDataChange)
	{
		notification->MarshalChar(TP_FUNCTION_DA_CALLBACK_ONDATACHANGE);   // function id
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process TP data change callback")))
	}
	else
	{
		notification->MarshalChar(TP_FUNCTION_DA_CALLBACK_ONREADCOMPLETE); // function id
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE, (_T("process TP read complete callback")))
	}

	notification->MarshalChar(0);                                     // flags
	notification->MarshalLong(clientTransId);
	// pro forma marshaling the correct values will be written before sending
	notification->MarshalLong(masterError);
	notification->MarshalLong(masterQuality);
	notification->MarshalLong(0);                                     // item count ???
	//-- send the data to the client --
	notification->SetWritePos(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
	notification->MarshalLong(masterError);
	notification->MarshalLong(masterQuality);
	notification->MarshalLong(0); // real Item count

	if (isOnDataChange)
	{
		_TRACE(masterError == S_OK ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("TPDA::Group::OnDataChange - keep alive - %lu [0x%lX][0x%lX]"), 0, (DWORD)masterError, (DWORD)masterQuality))
	}
	else
	{
		_TRACE(masterError == S_OK ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("TPDA::Group::OnReadComplete - keep alive - %lu [0x%lX][0x%lX]"), 0, (DWORD)masterError, (DWORD)masterQuality))
	}

	res = tpCon->invoke(tpCall, FALSE);

	if (FAILED(res))
	{
		// can't send notification
		server->forceDisconnect(_T(""));
		tpCon->close(); // -> disconnect client
	}

	return res;
}   //  end sendKeepAliveTP

#endif  //  end SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// cancel
//
// - cancel transaction and create OPC DA V2.0 cancel advise
//
BOOL SODaSTransaction::cancel(SODaSServer* pServer)
{
	if (!isStarted())
	{
		return FALSE;
	}

	setObjectState(SODaSTransaction::inactive);
	cancelRequests();       // cancel the requests
	HRESULT res = S_OK;

	if ((m_transactionId != 0) && (m_connectionType == SODaSTransaction::connectionPoint))
	{
#ifdef SOFEATURE_DCOM
		SOCmnListPosition pos;
		DWORD groupClientHandle = m_group->getClientHandle();
		DWORD clientTransId = getClientTransactionId();
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_CONNECTIONPOINT);
		{
			SODaSDataCallback advise;

			while (pos)
			{
				m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_CONNECTIONPOINT);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call cancel complete callback")))
				SOCMN_TRY
				{
					res = advise.m_dataCallback->OnCancelComplete(clientTransId, groupClientHandle);
					_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCDataCallback::OnCancelComplete [0x%X]"), res));

					if (SOSRV_IS_RPC_ERROR(res))
					{
						// client advise object not available anymore
						disconnectClient(); // -> disconnect client
					}
				}
				SOCMN_CATCH(...)
				{
					// exception in callback (only inproc)
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnCancelComplete")));
					res = RPC_E_SERVERFAULT;
				}
				m_group->onCancelComplete(this, &advise);
			}
		}
		m_group->doAdviseUnlock();
#endif
	}

	return SUCCEEDED(res);
} // cancel


//-----------------------------------------------------------------------------
// sendKeepAlive
//
// - send the keep alive notification to the client
//
void SODaSTransaction::sendKeepAlive(void)
{
#ifdef SOFLAG_DEMO_VERSION
	SOCmnPointer<SODaSServer> pServer((SODaSServer*)m_group->getParentBranch());

	if (pServer.isNotNull())
	{
		if ((pServer->getFlags() & SODASSERVER_FLAG_DCOM) == SODASSERVER_FLAG_DCOM)
		{
			if (!(SOSRV_ADVISE_CHECK_DEMO_DA))
			{
				return;
			}
		}

#ifdef SOFEATURE_TUNNEL

		if ((pServer->getFlags() & SODASSERVER_FLAG_TUNNEL) == SODASSERVER_FLAG_TUNNEL)
		{
			if (!(SOSRV_TP_ADVISE_CHECK_DEMO))
			{
				return;
			}
		}

#endif
	}

#endif

	if (m_type != SODaSTransaction::cyclic)
	{
		return;
	}

#ifdef SOFEATURE_DCOM

	if ((m_connectionType == SODaSTransaction::allConnections) || (m_connectionType == SODaSTransaction::connectionPoint))
	{
		// OPC DA V3.0
		SOCmnListPosition pos;
		HRESULT res;
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_CONNECTIONPOINT);
		{
			SODaSDataCallback advise;

			while (pos)
			{
				m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_CONNECTIONPOINT);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call V2.0 data change callback - keep alive")))
				SOCMN_TRY
				{
					OPCHANDLE hClient = 0;
					VARIANT variant;
					WORD quality = OPC_QUALITY_BAD;
					FILETIME timeStamp = {0, 0};
					HRESULT error = E_FAIL;

					::VariantInit(&variant);
					m_lastSendDataChangeTime.now();

					res = advise.m_dataCallback->OnDataChange(0, m_group->getClientHandle(), S_OK, S_OK,
															  0, &hClient, &variant, &quality, &timeStamp, &error);

					_TRACE_FAILED(res, SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("IOPCDataCallback::OnDataChange [0x%X]"), res));

					if (SOSRV_IS_RPC_ERROR(res))
					{
						// client advise object not available anymore
						disconnectClient(); // -> disconnect client
					}
				}
				SOCMN_CATCH(...)
				{
					// exception in callback (only inproc)
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE, (_T("exception in OnDataChange")));
					res = RPC_E_SERVERFAULT;
				}
			}
		}
		m_group->doAdviseUnlock();
	}

#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_TUNNEL

	if ((m_connectionType == SODaSTransaction::allConnections) || (m_connectionType == SODaSTransaction::tunnel))
	{
		// OPC Tunnel
#ifdef SOFEATURE_DCOM
		SOCmnListPosition pos;
		pos = m_group->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_TUNNEL);
		SODaSDataCallback advise;

		while (pos)
		{
			m_group->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_TUNNEL);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE, (_T("call TPDA data change callback - keep alive")))
			m_lastSendDataChangeTime.now();

			if ((m_group->getEnable()) || ((m_type != SODaSTransaction::cyclic) && (getRequestType() != SODaSRequest::asyncInitNotify)))
			{
				sendKeepAliveTP(&advise);
			}
		}

		m_group->doAdviseUnlock();
#else

		if ((m_group->getEnable()) || ((m_type != SODaSTransaction::cyclic) && (getRequestType() != SODaSRequest::asyncInitNotify)))
		{
			sendKeepAliveTP(NULL);
		}

#endif // SOFEATURE_DCOM
	}

#endif // SOFEATURE_TUNNEL
} // sendKeepAlive


//-----------------------------------------------------------------------------
// disconnectClient
//
// - disconnects the server from the OPC client of the transaction
//
void SODaSTransaction::disconnectClient(void)
{
	SOCmnPointer<SODaSServer> srv((SODaSServer*)m_group->getParentBranch());
#ifdef SOFEATURE_DCOM

	if (srv.isNotNull())
	{
		srv->disconnectClient();
	}

#endif  //  SOFEATURE_DCOM
} // disconnectClient

DWORD SODaSTransaction::getRequestType(void)
{
	SOCmnList<SODaSRequest> reqList(m_reqList);

	if (!reqList.isEmpty())
	{
		SODaSRequest* pReq = reqList.getHead();
		return (DWORD)pReq->getType();
	}

	return 0;
}

BOOL SODaSTransaction::notifyAllChanges(void)
{
	return FALSE;
}

void SODaSTransaction::emptyRequestsCompletedEvent(void)
{
	SOCmnList<SODaSRequest> reqList(m_reqList);
	SOCmnListPosition pos;
	SODaSRequest* pReq;
	pos = reqList.getStartPosition();

	while (pos)
	{
		pReq = reqList.getNext(pos);
		pReq->setCompletedEvent(NULL);
	}
}



#ifdef SOFEATURE_SOAP
void SODaSTransaction::setXMLMethod(IN SODaSXmlMethod* pMth)
{
	if (m_xmlMethod != NULL)
	{
		m_xmlMethod->release();
	}

	m_xmlMethod = pMth;
	m_xmlMethod->addRef();
}

SODaSXmlMethod* SODaSTransaction::getXMLMethod(void)
{
	if (m_xmlMethod != NULL)
	{
		m_xmlMethod->addRef();
	}

	return m_xmlMethod;
}

#endif // SOFEATURE_SOAP

#endif // SODAS
