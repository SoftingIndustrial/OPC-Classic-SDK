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
//  Filename    : SODaSServer.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server object related classes                           |
//                - SODaSServer: OPC DA server object                         |
//                - SODaSObjectRoot: root object of OPC runtime object tree   |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSServer.h"
#include "SODaSCache.h"
#include "SODaSItemTag.h"
#include "SODaSNode.h"
#include "SODaSEntry.h"
#include "SODaSGroup.h"
#include "SODaSItem.h"
#include "SOCmnTrace.h"
#ifdef SOFEATURE_DCOM
#include "dllmain.h"
#include "SODaSComGroup.h"
#include "SODaSComServer.h"
#endif
#ifdef SOFEATURE_SOAP
#include "SODaSXmlServer.h"
#endif
#include <opcerror.h>


//-----------------------------------------------------------------------------
// SODaSServer                                                                |
//-----------------------------------------------------------------------------

SODaSServer::SODaSServer(
	IN OPTIONAL BOOL publicGroups,              // creating public groups server
	IN OPTIONAL BYTE serverInstance,        // server instance number
	IN OPTIONAL DWORD minRequestLoopTime)   // minimal request loop time
	: SOCmnElementListBranch(0),
	  SOSrvServer(SOCMNOBJECT_TYPE_SERVER_DA, serverInstance)
{
	// create branch map
	if (m_branchList.isInit())
	{
		m_branchList.destroy();
	}

	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 17;
	m_branchList.create(&cfg);
	m_publicGroupTemplates = (BYTE)publicGroups;
	m_bandwidth = 0;
	m_dueTimeRead = INFINITE;
	m_dueTimeWrite = INFINITE;
	m_minRequestLoopTime = minRequestLoopTime;
	m_transactionList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_curBrowse = getBrowserRoot();

	if (!isPublicGroups())
	{
		// not public groups template server
		// create asynchronous work thread
		createWorkThread();
		m_flags = 0;
	}
	else
	{
		m_clientName = _T("Public Group Templates");
		m_flags = SODASSERVER_FLAG_PUBLIC_GROUPS;
	}

	srand((unsigned int)time(NULL));
	byte secretLen = 16 + static_cast<byte>(rand() % 16);
	
	for (byte b = 0; b < secretLen; ++b)
	{
		m_secretString += (char)(32 + rand() % 94); // only standard characters, no need to over-complicate things
	}

#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SERVER_DA);
#endif
}


SODaSServer::~SODaSServer(void)
{
#ifdef SOFEATURE_TUNNEL
	m_tpCon.release();
#endif
	m_transactionList.destroy();
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_SERVER_DA);
#endif
}


DWORD SODaSServer::setFlags(BYTE flags)
{
	m_flags = flags;
	return S_OK;
}


//-----------------------------------------------------------------------------
// doWork
//
// - do work of one working thread loop
//
// returns:
//      minimal time till next call
//
DWORD SODaSServer::doWork(
	IN DWORD startTime)     // start time of work loop
{
	DWORD waitTime;
	SOCmnDateTime ftNow;
	ftNow.now();
	waitTime = startCyclicTransactions(startTime, INFINITE);
	waitTime = updateTransactions(startTime, waitTime);
#ifdef SOFEATURE_SOAP
	waitTime = handleXMLMethods(ftNow, startTime, waitTime);
	waitTime = checkSubscriptions(ftNow, startTime, waitTime);
#endif
#ifdef SOFEATURE_DCOM
	waitTime = sendKeepAlive(ftNow, startTime, waitTime);
#endif
	return waitTime;
} // requestThread


//-----------------------------------------------------------------------------
// startCyclicTransactions
//
// - start all cyclic transactions depending on the update rate
//
// returns:
//      time in milliseconds until next call is needed
//
DWORD SODaSServer::startCyclicTransactions(
	IN DWORD loopStart, // current time
	IN DWORD minSpan)   // minimal time till next call
{
	SODaSGroup* grp;
	SOCmnPointer<SODaSTransaction> tac;
	DWORD actSpan;
	int             nTmpGroups;
	SODaSGroup**    pTmpGroupList = allocGroupsAsArray(&nTmpGroups);

	// for every group
	for (int i = 0; i < nTmpGroups; i++)
	{
		grp = pTmpGroupList[i];
		actSpan = INFINITE;

		// check if to start the cyclic transaction
		if (grp->updateSpan(loopStart, &actSpan))
		{
			// update rate ellapsed start the transaction
			tac = grp->getCyclicTransaction();
			// start the cyclic requests of the group
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("start cyclic transaction %s"), tac->getObjTraceId()))
			tac->start(this);
		}

		if (actSpan < minSpan)
		{
			minSpan = actSpan;
		}
	}

	freeGroupsAsArray(pTmpGroupList);
	return minSpan;
} // startCyclicTransactions


//-----------------------------------------------------------------------------
// updateTransactions
//
// - check all transactions if all requests of the transaction are completed
//
// returns:
//      time in milliseconds until next call is needed
//
DWORD SODaSServer::updateTransactions(
	IN DWORD loopStart, // current time
	IN DWORD minSpan)   // minimal time till next call
{
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnListPosition pos;
	SOCmnListPosition thisPos;
	DWORD dueTime;
	DWORD actSpan;
	SOCmnPointer<SODaSGroup> groupOfCycTac;
	m_transactionList.lock();
	m_transactionList.setDirty(FALSE);
	pos = m_transactionList.getStartPosition();
	m_transactionList.unlock();

	while (pos)
	{
		m_transactionList.lock();

		if (m_transactionList.isDirty())
		{
			// check current position!
			SOCmnListPosition checkPos;
			BOOL found = FALSE;
			checkPos = m_transactionList.getStartPosition();

			while (checkPos)
			{
				if (checkPos == pos)
				{
					found = TRUE;
					break;
				}

				m_transactionList.getNext(checkPos);
			}

			if (!found)
			{
				pos = m_transactionList.getStartPosition();
			}

			m_transactionList.setDirty(FALSE);

			if (pos == NULL)
			{
				m_transactionList.unlock();
				break;
			}
		}

		thisPos = pos;
		tac = m_transactionList.getNext(pos);
		tac.addRef();

		if (tac->isCyclic())
		{
			groupOfCycTac = tac->getGroup();    // hold reference to group of cyclic transaction
		}

		m_transactionList.unlock();

		// maximal request time
		if (tac->getType() != SODaSTransaction::asyncWrite)
		{
			dueTime = m_dueTimeRead;
		}
		else
		{
			dueTime = m_dueTimeWrite;
		}

		actSpan = INFINITE;

		if (tac->update(this, loopStart, dueTime, &actSpan))
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("completed transaction %s"), tac->getObjTraceId()))

			if (tac->getType() == SODaSTransaction::cyclic)
			{
				SOCmnList<SODaSRequest> reqList(tac->getRequestList());
				SODaSRequest* req;
				SOCmnListPosition reqpos, reqcurpos, reqprevpos;
				reqcurpos = NULL;
				reqpos = reqList.getStartPosition();

				while (reqpos)
				{
					reqprevpos = reqcurpos;
					reqcurpos = reqpos;
					req = reqList.getNext(reqpos);

					// remove all report requests after completed the transaction
					if (req->getType() == SODaSRequest::report)
					{
						if (req->getObjectState() & SODASREQUEST_FLAG_PROCESSED)
						{
							SOCmnPointer<SODaSItem> item = req->getItem();
							SOCmnPointer<SODaSRequest> cycRepReq;
							cycRepReq = item->getCyclicRequest();

							if (req == (SODaSRequest*)cycRepReq)
							{
								item->setCyclicRequest(NULL);
							}

							reqList.removeAt(reqcurpos, reqprevpos);
							reqcurpos = reqprevpos;
						}
						else
						{
							signalWorkEvent();
						}
					}
				}

				reqList.autoUnlock();
			}
			else
			{
				m_transactionList.lock();

				if (m_transactionList.isDirty())
				{
					thisPos = m_transactionList.findObject((SODaSTransaction*)tac);
				}

				if (thisPos)
				{
					m_transactionList.removeAt(thisPos);// remove from list
					tac.release();                  // free transaction (list)
				}

				m_transactionList.unlock();
			}
		}

		if (actSpan < minSpan)
		{
			minSpan = actSpan;
		}

		groupOfCycTac.release();
	}

	return minSpan;
} // updateTransactions


#ifdef SOFEATURE_SOAP
DWORD SODaSServer::insertXMLMethod(
	IN BYTE xmlMethodType,
	IN SOSrvHTTPHandler* pHandler,
	IN SOSrvHTTPHandlerData* pHTTPData,
	IN DWORD lenBody,
	IN BYTE* pBody)
{
	SOCmnPointer<SODaSXmlMethod> mth;
	SOCMN_ALLOCATE_OBJECT_C(mth, SODaSXmlMethod(xmlMethodType, pHandler, pHTTPData, lenBody, pBody), (SODaSXmlMethod*))
	SOCmnList<SOSrvXmlMethod> xmlMethodList(getXmlMethodList());

	if (xmlMethodList.add(mth))
	{
		signalWorkEvent();
	}
	else
	{
		return (DWORD)E_FAIL;
	}

	return S_OK;
}

DWORD SODaSServer::handleXMLMethods(
	IN SOCmnDateTime& now,
	IN DWORD /* nowTick */,
	IN DWORD minSpan)
{
	SOCmnList<SOSrvXmlMethod> xmlMethodList(getXmlMethodList());
	SOCmnPointer<SOSrvXmlMethod> mth;
	SOCmnPointer<SODaSXmlServer> xmlServer = (SODaSXmlServer*)getHTTPReceiver();
	DWORD actSpan;

	if (xmlServer.isNotNull())
	{
		do
		{
			mth = xmlMethodList.pop();
			xmlMethodList.autoUnlock();

			if (mth.isNotNull())
			{
				actSpan = xmlServer->handleXmlMethod(this, (SODaSXmlMethod*)(SOSrvXmlMethod*)mth, now);

				if (actSpan < minSpan)
				{
					minSpan = actSpan;
				}
			}
		}
		while (mth.isNotNull());
	}

	return minSpan;
}


//-----------------------------------------------------------------------------
// checkHoldTime
//
// - check hold and wait time
//   The main reason for this method is that the hold time is supplied in absolute
//   time
//
// returns:
//      TRUE  - vaild times
//      FALSE - invaild times
//
BOOL SODaSServer::checkHoldTime(
	LONGLONG holdTime,
	DWORD waitTime,
	DWORD updateRate,
	DWORD pingRate,
	SODaSTransaction* /* pTac */)
{
	// OPC Toolkit assumptions: could be overloaded
	// - holdTime should be in the future or not too long ago (25s)
	// - holdTime should be less than 20 x updateRate
	// - waitTime should be less than ping rate
	if (holdTime < 0)
	{
		if (holdTime < -30000)
		{
			return FALSE;
		}
	}
	else
	{
		LONGLONG uR = updateRate * 20;

		if (holdTime > uR)
		{
			return FALSE;
		}
	}

	if (waitTime > pingRate)
	{
		return FALSE;
	}

	return TRUE;
}

DWORD SODaSServer::checkSubscriptions(
	IN SOCmnDateTime& now,
	IN DWORD /* nowTick */,
	IN DWORD minSpan)
{
	{
		SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
		SOCmnPointer<SODaSServer> subServer = entryDA->getXMLSubscriptionServer();

		if ((SODaSServer*)subServer != this)
		{
			return minSpan;    // only relevant for the XML subscription server
		}
	}
	SOCmnList<SODaSGroup> listG((SOCmnList<SODaSGroup> *)getBranchList());
	SOCmnListPosition posG;
	SODaSGroup* pG;
	SOCmnPointer<SODaSTransaction> cycTac;
	SOCmnDateTime lastSendTime;
	DWORD pingSpan;
	DWORD pingRate;
	ULONGLONG nowMS = now.getMicroseconds();
	ULONGLONG lstMS;
	posG = listG.getStartPosition();

	while (posG)
	{
		pG = listG.getNext(posG);
		cycTac = pG->getCyclicTransaction();
		cycTac->getLastSendDataChangeTime(lastSendTime);
		lstMS = lastSendTime.getMicroseconds();
		pingRate = pG->getPingRate();

		if (nowMS > lstMS)
		{
			pingSpan = (DWORD)((nowMS - lstMS) / 1000);

			if (pingSpan >= pingRate)
			{
				// ping rate ellapsed
				if (!pG->hasPolledRefreshTransactions())
				{
					// -> remove subscription
					_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("Remove Subscription 0x%8.8lX - ping rate %lu ms ellapsed"), (DWORD)pG->getOpcHandle(), pingRate));
					removeGroup((DWORD)pG->getOpcHandle());
				}
				else
				{
					DWORD updateRate = pG->getUpdateRate();

					if (minSpan > updateRate)
					{
						minSpan = updateRate;
					}
				}
			}
			else if (minSpan > pingRate - pingSpan)
			{
				minSpan = pingRate - pingSpan;
			}
		}
		else
		{
			// last send time in the future
			pingSpan = (DWORD)((lstMS - nowMS) / 1000);

			if (pingSpan > 1000) // more than a second in the future
			{
				cycTac->updateLastSendDataChangeTime();    // update last send time, maybe the clock was adapted
			}

			if (minSpan > pingRate)
			{
				minSpan = pingRate;
			}
		}
	}

	return minSpan;
}

BOOL SODaSServer::checkAuthorization(IN SODaSXmlMethod* /* pMth */)
{
	return TRUE;
	/* Sample implementation - allows writing only for user Softing:Softing

	    BOOL ret = TRUE;
	    if (pMth->getMethod() == SODASXMLMETHOD_WRITE)
	    {
	        SOCmnPointer<SOSrvHTTPHandler> httpH = pMth->getHTTPHandler();
	        SOCmnStringList attributeList(httpH->getAttributeList());
	        SOCmnString authorization;

	        if (attributeList.findKey(_T("authorization"), &authorization))
	        {
	            if (_tcsncmp(_T("Basic "), authorization, 6) == 0)
	            {
	                SOCmnString userPwd64;
	                SOCmnString userPwd;

	                userPwd64 = authorization.right(authorization.getLength() - 6);
	                SOCmnXmlValue::decodeBase64String2String(userPwd64, userPwd);

	                if (userPwd != _T("Softing:Softing"))
	                {
	                    httpH->sndHTTPErrorResponse(pMth->getClientId(), pMth->getFlags(), SOCMNHTTP_CODE_FORBIDDEN);
	                    ret = FALSE;
	                }
	            }
	        }
	        else
	        { // request authorization
	            SOCmnString szAuthenticate = _T("WWW-Authenticate: Basic realm=\"Write\"\r\n");
	            SOCmnPointer<SOSrvHTTPHandlerData> httpHD = pMth->getHTTPHandlerData();
	            httpH->sndHTTPErrorResponse(httpHD, pMth->getFlags(), SOCMNHTTP_CODE_UNAUTHORIZED, szAuthenticate);
	            ret = FALSE;
	        }
	    }

	    return ret;
	*/
}

BOOL SODaSServer::getVendorXMLNamespace(
	IN WORD /* forWhat */,
	OUT  SOCmnString& szVendorNS)
{
	szVendorNS.empty();
	return FALSE;
}

#endif

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// sendKeepAlive
//
// - check all groups if to send keep alive notifications
//
// returns:
//      time in milliseconds until next call is needed
//
DWORD SODaSServer::sendKeepAlive(
	IN SOCmnDateTime& now, // current time
	IN DWORD /* nowTick */, // current time
	IN DWORD minSpan) // minimal time till next call
{
	SODaSGroup*     grp;
	DWORD           actSpan;
	DWORD           keepAlive;
	int             nTmpGroups;
	SODaSGroup**    pTmpGroupList = allocGroupsAsArray(&nTmpGroups);

	// for every group
	for (int i = 0; i < nTmpGroups;  i++)
	{
		grp = pTmpGroupList[i];
		actSpan = INFINITE;
		keepAlive = grp->getKeepAliveTime(); // send 10 ms before time ellapsed

		if ((grp->getActive()) && (keepAlive > 0))
		{
			SOCmnPointer<SODaSTransaction> trans = grp->getCyclicTransaction();
			SOCmnDateTime lastSDC;
			trans->getLastSendDataChangeTime(lastSDC);
			DWORD lastSDCMS = (DWORD)((now.getMicroseconds() - lastSDC.getMicroseconds()) / 1000);

			if (lastSDCMS >= keepAlive)
			{
				// send keep alive
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("send keep alive %s"), grp->getObjTraceId()))
				trans->sendKeepAlive();
				actSpan = keepAlive;
			}
			else
			{
				// calc span
				actSpan = keepAlive - lastSDCMS;
			}
		}

		if (actSpan < minSpan)
		{
			minSpan = actSpan;
		}
	}

	freeGroupsAsArray(pTmpGroupList);
	return minSpan;
} // sendKeepAlive


//-----------------------------------------------------------------------------
// getMe
//
// - get reference to COM object
//
// returns:
//      pointer of COM object
//
void* SODaSServer::getMe(void)
{
	m_meSync.lock();
	SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)m_me;

	if (srv)
	{
		srv->tempAddRef();
	}

	m_meSync.unlock();
	return m_me;
} // getMe


//-----------------------------------------------------------------------------
// releaseMe
//
// - release reference to COM object obtained by getMe
//
// returns:
//      temporary reference count of COM object
//
ULONG SODaSServer::releaseMe(
	IN void* /* me */)    // pointer to COM object
{
	ULONG uRet = 0;
	this->addRef();
	m_meSync.lock();
	SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)m_me;

	if (srv)
	{
		uRet = srv->tempRelease();
	}

	m_meSync.unlock();
	this->release();
	return uRet;
} // releaseMe

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaSServer::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a group object
//
// returns:
//      TRUE  - added
//      FALSE - not added
//
BOOL SODaSServer::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_GROUP))
	{
		m_branchList.doRehash(TRUE);
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// findBranch
//
// - find branch below the branch
//
// returns:
//		pointer to the branch - branch found
//		NULL - no branch found
//
SOCmnElement* SODaSServer::findBranch(
	IN LPCTSTR name,        // name of branch
	IN OPTIONAL BOOL deep)  // deep search or only direct child
{
	SOCmnElement* branch = NULL;
	SOCmnString path;
	LPTSTR child = NULL;
	LPTSTR remainingPath = NULL;

	if (!name)
	{
		return NULL;
	}

	path = name;
	reducePath(path, child, remainingPath, TRUE);   // slip path

	if ((!deep) && (remainingPath)) // no deep search and remaining path
	{
		return NULL;    // -> name can't be a direct child of this node
	}

	SOCmnList<SOCmnElement> branchList(getBranchList());

	if (!!branchList)
	{
		SOCmnPointer<SOCmnElement> search;
		SOCmnElement* pSearch;
		SOCmnListPosition pos;
		pos = branchList.getStartPosition();

		while (pos)
		{
			pSearch = branchList.getNext(pos);

			if (pSearch->hasKey(child))
			{
				search = pSearch;
				search.addRef();
				break;
			}
		}

		if (!!search)
		{
			if ((!deep) || (!remainingPath))
			{
				// no deep search or no remaining path
				branch = search;
				branch->addRef();
			}
			else
			{
				// deep search and remaining path
				branch = search->findBranch(remainingPath, TRUE);
			}
		}
	}

	return branch;
} // findBranch


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//      trace id string
//
LPCTSTR SODaSServer::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)getClientName());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// getGroupByHandle
//
// - retrieve group pointer by the server handle value
//
// returns:
//      pointer to the group
//      NULL - no group with this server handle
//
SODaSGroup* SODaSServer::getGroupByHandle(
	IN OPCHANDLE serverHandle)  // server handle
{
	SOCmnList<SOCmnElement> branchList(getBranchList());
	return (SODaSGroup*)branchList.findKey(serverHandle);
} // getGroupByHandle


//-----------------------------------------------------------------------------
// allocGroupsAsArray
//
// - retrieve a (zero terminated) array of SODaSGroup*,
//   every group in the array has been AddRef()ed.
//
// returns:
//      pointer to the group array
//      NULL - out of memory
//
SODaSGroup** SODaSServer::allocGroupsAsArray(
	OUT int* pnGroups)
{
	SOCmnList<SOCmnElement> groupList(getBranchList());
	int                     nGroups = groupList.getCount();
	SODaSGroup**            pGroups;
	SOCMN_ALLOCATE_MEMORY_C(pGroups, (nGroups + 1)*sizeof(SODaSGroup*), (SODaSGroup**));

	if (!pGroups)
	{
		*pnGroups = 0;
		return NULL; //out of mem...
	}

	*pnGroups = nGroups;
	int i = 0;
	SOCmnListPosition pos = groupList.getStartPosition();

	while (pos && i < nGroups)
	{
		pGroups[i] = (SODaSGroup*)groupList.getNext(pos);
		pGroups[i++]->addRef();
	}

	pGroups[i] = NULL; //zero-term the array
	return pGroups;
}

//-----------------------------------------------------------------------------
// freeGroupsAsArray
//
// - call Release() for all groups in the array and free the array itself
//
void SODaSServer::freeGroupsAsArray(SODaSGroup** pGroups)
{
	if (pGroups)
	{
		for (int i = 0; pGroups[i]; i++)
		{
			pGroups[i]->release();
		}

		SOCMN_FREE_MEMORY(pGroups);
	}
}


//-----------------------------------------------------------------------------
// getGroupByName
//
// - retrieve group pointer by the name of the group
//
// returns:
//      pointer to the group
//      NULL - no group with this name
//
SODaSGroup* SODaSServer::getGroupByName(
	IN LPCTSTR name)    // group name
{
	SOCmnList<SOCmnElement> branchList(getBranchList());
	SOCmnListPosition pos;
	SODaSGroup* pGroup;
	pos = branchList.getStartPosition();

	while (pos)
	{
		pGroup = (SODaSGroup*)branchList.getNext(pos);

		if (_tcscmp(pGroup->getName(), name) == 0)
		{
			pGroup->addRef();
			return pGroup;
		}
	}

	return NULL;
} // getGroupByName


//-----------------------------------------------------------------------------
// addGroup
//
// - adds a group to the server
// - creates tree group object and put it to the tree
// - set initialization parameter
//
// returns:
//      S_OK                  - added group
//      OPC_S_UNSUPPORTEDRATE - changed update rate (but added group)
//      E_INVALIDARG          - bad name)
//      OPC_E_DUPLICATENAME   - group name not unique
//
HRESULT SODaSServer::addGroup(
	IN LPCTSTR name,            // name of group
	IN BOOL active,             // activation state
	IN DWORD reqUpdateRate,     // requested update rate
	IN OPCHANDLE clientHandle,  // client handle
	IN LONG* pTimeBias,         // time zone
	IN FLOAT* pPercentDeadband, // deadband
	IN DWORD lcid,              // LCID
	OUT SODaSGroup** group,     // created group
	OUT DWORD* revUpdateRate,   // revised update rate
	IN OPTIONAL BOOL addWatchChange)
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSTransaction> trans;
	SOCmnString grpName;
	SOCmnPointer<SODaSCreator> creator;
	HRESULT res = S_OK;
	SODaSGroup* grp;

	// check the name
	if ((name) && (_tcslen(name) != 0))
	{
		// valid name ?
		if (FAILED(validateName(name, FALSE)))
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't add group; no valid name %s"), name))
			return E_INVALIDARG;
		}

		// unique name ?
		if (isUniqueChildName(name))
		{
			grpName = name;
		}
		else
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't add group; no unique name %s"), name))
			return OPC_E_DUPLICATENAME;
		}
	}

	// check LCID
	if (!supportLCID(lcid))
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't add group; no supported LCID 0x%lX"), lcid))
		return E_INVALIDARG;
	}

	// create OPC tree object
	creator = engine->getCreator();
	grp = creator->createGroup(this);

	if (grp == NULL)
	{
		return E_ACCESSDENIED;
	}

	if ((!name) || (_tcslen(name) == 0))
	{
		// no name -> generate unique name
		grpName.Format(_T("GRP%8.8lX"), (DWORD)grp->getOpcHandle());
	}

	// set name
	grp->setName(grpName);
	// add to tree
	addBranch(grp);
	// add cyclic transaction
	trans = grp->getCyclicTransaction();
	addTransaction(trans);
	// set group parameters
	grp->setActive(active);
	grp->setUpdateRate(reqUpdateRate);
	grp->setClientHandle(clientHandle);

	if (pTimeBias)
	{
#ifdef SOFEATURE_DCOM
		grp->setTimeBias(*pTimeBias);
#endif
	}

	if (pPercentDeadband)
	{
		grp->setDeadBand(*pPercentDeadband);
	}

	grp->setLCID(lcid);

	if (revUpdateRate != NULL)
	{
		*revUpdateRate = grp->getUpdateRate();
	}

#ifdef SOFEATURE_WATCH

	if ((addWatchChange) && (getCollectChanges()))
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
		entry->watchAddObject(this, grp);
	}

#endif

	// check update rate
	if (reqUpdateRate != grp->getUpdateRate())
	{
		res = OPC_S_UNSUPPORTEDRATE;
	}

	if (group != NULL)
	{
		*group = grp;
	}
	else
	{
		grp->release();
	}

	return res;
} // addGroup


//-----------------------------------------------------------------------------
// removeGroup
//
// - removes a group to the server
//
// returns:
//      S_OK         - removed
//      E_INVALIDARG - bad server handle
//
HRESULT SODaSServer::removeGroup(
	IN OPCHANDLE serverHandle,      // server handle
	IN OPTIONAL BOOL remPublicGrp)  // remove public group objects
{
	SOCmnPointer<SODaSGroup> group;
	SOCmnPointer<SODaSTransaction> trans;
	HRESULT res = S_OK;
	// get group
	group = getGroupByHandle(serverHandle);

	if (!group)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't remove group; no group with handle 0x%lX"), serverHandle))
		return E_INVALIDARG;    // no group with this handle
	}

	if ((group->isPublicGroup()) && (!remPublicGrp))
	{
		// don't remove public group objects !!
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("don't remove public group objects")))
		return E_INVALIDARG;
	}

	if (isPublicGroups())
	{
		// public group template
		if (!canRemovePublicGroup(group))
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't remove this public group")))
			return E_FAIL;
		}
	}

	// remove and disconnect all item objects
	SOCmnListPosition pos;
	SOCmnList<SODaSItem> list((SOCmnList<SODaSItem> *)group->getLeafList());

	if (list.isInit())
	{
		if (!list.isEmpty())
		{
			SODaSItem* item;
			pos = list.getStartPosition();

			while (pos)
			{
				item = list.getNext(pos);
				group->removeItem((OPCHANDLE)item->getOpcHandle());
			}

			group->onRemoveItems();
		}
	}

	list.autoUnlock();
	group->setRemoved();    // removed flag
	// remove cyclic transaction
	trans = group->getCyclicTransaction();
	removeTransaction(trans);
	removeBranch(group);

	if (group->getRefCount() > 2)   // one ref from COM object and on of the
	{
		// local group pointer are OK
		res = OPC_S_INUSE;
	}

	return res;
} // removeGroup


//-----------------------------------------------------------------------------
// canRemovePublicGroup
//
// - check if it's allowed to remove a public group
//
// returns:
//      TRUE  - yes remove it
//      FALSE - dont remove it
//
BOOL SODaSServer::canRemovePublicGroup(
	IN SODaSGroup* /* group */)   // group object
{
	return TRUE;
} // canRemovePublicGroup


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// getStatus
//
// - gets the server status
//
// returns:
//      S_OK          - filled status sturcture
//      E_OUTOFMEMORY - can't allocate vendor string memory
//
HRESULT SODaSServer::getStatus(
	OUT OPCSERVERSTATUS* status)    // server status
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnString vendor = engine->getVendorInfo();
	SOCmnList<SOCmnElement> list(getBranchList());
	USES_CONVERSION;
	status->szVendorInfo = (LPWSTR)CoTaskMemAlloc(2 * vendor.GetLength() + 2);

	if (status->szVendorInfo == NULL)
	{
		return E_OUTOFMEMORY;
	}

	wcscpy(status->szVendorInfo, T2COLE((LPCTSTR)vendor));
	SOCmnDateTime startTime, lastUpdateTime;
	engine->getStartTime(&startTime);
	startTime.get(&status->ftStartTime);
	GetSystemTimeAsFileTime(&status->ftCurrentTime);
	getUpdateTime(lastUpdateTime);
	lastUpdateTime.get(&status->ftLastUpdateTime);
	status->dwServerState = engine->getServerState();
	status->dwGroupCount = list.getCount();
	status->dwBandWidth = getBandwidth();
	WORD wPatchVersion = 0;
	engine->getVersionInfo(&status->wMajorVersion, &status->wMinorVersion,
						   &wPatchVersion, &status->wBuildNumber);
	status->wBuildNumber += wPatchVersion * 10000u;
	return S_OK;
} // getStatus

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// getBandwidth
//
// - gets the bandwidth of the whole OPC server
//
// returns:
//      server bandwidth
//
DWORD SODaSServer::getBandwidth(void)
{
	return 0xFFFFFFFF;
} // getBandwidth


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// startShutdownRequest
//
// - add shutdown request to the shutdown list of the entry
//
// returns:
//      S_OK    - added shutdown request
//      E_FAIL  - no COM server object
//
HRESULT SODaSServer::startShutdownRequest(
	IN LPCTSTR reason,  // shutdown reason
	IN HANDLE event)    // noftification event
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();

	if (!m_me)
	{
		return E_FAIL;
	}

	if (isPublicGroups())
	{
		return S_FALSE;
	}

	engine->addShutdownRequest(this, reason, event);
	return S_OK;
} // startShutdownRequest


//-----------------------------------------------------------------------------
// sendShutdownRequest
//
// - send shutdown request to the OPC client
//
// returns:
//      S_OK    - send shutdown request
//      E_FAIL  - no COM server object
//
HRESULT SODaSServer::sendShutdownRequest(
	IN LPCTSTR reason)  // shutdown reason
{
	if (isPublicGroups())
	{
		return S_FALSE;
	}

	SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)getMe();
	HRESULT res;

	if (srv)
	{
		res = srv->sendShutdownRequest(reason);
		releaseMe(srv);
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // sendShutdownRequest

#endif // SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// String based overwritables

//-----------------------------------------------------------------------------
// queryAvailablePropertiesByItemID
//
// - get the available properties of a string based item id
//
void SODaSServer::queryAvailablePropertiesByItemID(
	IN LPCTSTR itemId,                              // item id
	IN BOOL isSOAP,
	IN BOOL returnPropertyValues,
	OUT SOCmnList<SODaSPropertyData> &propertyList) // property data list
{
	VARTYPE datatype;

	if (isTagName(itemId))
	{
		datatype = getTagDatatypeByItemID(itemId);
		getStandardTagProperties(datatype, isSOAP, propertyList);
	}
	else if (isNodeName(itemId))
	{
		datatype = getNodeDatatypeByItemID(itemId);

		if (datatype != VT_EMPTY)
		{
			getStandardTagProperties(datatype, isSOAP, propertyList);
		}

		getStandardNodeProperties(isSOAP, propertyList);
	}
} // queryAvailablePropertiesByItemID


//-----------------------------------------------------------------------------
// isPropertyName
//
// - check if the string based item supports the property id
//
// returns:
//      TRUE  - valid property id, item id combination
//      FALSE - item id doesn't support the property
//
BOOL SODaSServer::isPropertyName(
	IN LPCTSTR itemId,              // item id
	IN DWORD propId,                // property id
	IN LPCTSTR /* propIdString */,  // property id string
	SODaSPropertyData* /* pPropData */)
{
	if (isStandardTagProperty(propId))
	{
		if (isTagName(itemId))
		{
			return TRUE;
		}

		if (isNodeName(itemId))
		{
			return (VT_EMPTY != getNodeDatatypeByItemID(itemId));
		}
	}

	if ((isStandardNodeProperty(propId)) && (isNodeName(itemId)))
	{
		return TRUE;
	}

	return FALSE;
} // isPropertyName


//-----------------------------------------------------------------------------
// isNodeName
//
// - check if the string represents a valid node
//
// returns:
//      TRUE  - valid node name
//      FALSE - no node with this name
//
BOOL SODaSServer::isNodeName(
	IN LPCTSTR /* fullName */)    // node name
{
	return FALSE;
} // isNodeName


//-----------------------------------------------------------------------------
// isTagName
//
// - check if the string represents a valid tag
//
// returns:
//      TRUE  - valid tag name
//      FALSE - no tag with this name
//
BOOL SODaSServer::isTagName(
	IN LPCTSTR /* fullName */)    // tag name
{
	return FALSE;
} // isTagName


//-----------------------------------------------------------------------------
// getAccessPathsByItemID
//
// - get the access paths of the string based tag
//
// returns:
//      S_OK          - got access path
//      S_FALSE       - no access path
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - error allocating buffer for return
//
HRESULT SODaSServer::getAccessPathsByItemID(
	IN LPCTSTR /* name */,                // fully qualified tag name
	OUT SOCmnStringList& /* pathList */)  // access path list
{
	return S_FALSE;
} // getAccessPathsByItemID


//-----------------------------------------------------------------------------
// getTagDatatypeByItemID
//
// - get the datatype of the string based tag
//
// returns:
//      datatype of tag
//
VARTYPE SODaSServer::getTagDatatypeByItemID(
	IN LPCTSTR /* fullName */)    // fully qualified tag name
{
	return VT_EMPTY;
} // getTagDatatypeByItemID


//-----------------------------------------------------------------------------
// getNodeDatatypeByItemID
//
// - get the datatype of the string based tag
//
// returns:
//    VT_EMPTY - no I/O to this node supported
//      datatype of tag
//
VARTYPE SODaSServer::getNodeDatatypeByItemID(
	IN LPCTSTR /* fullName */)    // fully qualified node name
{
	return VT_EMPTY;
} // getNodeDatatypeByItemID


//-----------------------------------------------------------------------------
// getAccessRightsByItemID
//
// - get the datatype of the string based tag
//
// returns:
//      datatype of tag
//
DWORD SODaSServer::getAccessRightsByItemID(
	IN LPCTSTR /* fullName */)    // fully qualified tag name
{
	return (OPC_READABLE | OPC_WRITEABLE);
} // getAccessRightsByItemID


//-----------------------------------------------------------------------------
// getChildNamesOfNodeName
//
// - get the child names of the string based node
//
void SODaSServer::getChildNamesOfNodeName(
	IN LPCTSTR /* nodeName */,            // node name
	IN enum SOSrvBrowser::browserFilterType /* browseType */, // browse what
	IN LPCTSTR /* filterCriteria */,      // string filter for child names
	IN VARTYPE /* dataTypeFilter */,      // datatype filter
	IN DWORD /* accessRightsFilter */,    // access rights filter
	OUT SOCmnStringList& /* nameList */)  // child name list
{
} // getChildNamesOfNodeName


//-----------------------------------------------------------------------------
// SOSrvBrowser overwritables

//-----------------------------------------------------------------------------
// getChildNames
//
// - called to receive the string based child names
// - calls the method of the server object
//
// returns:
//      S_OK
//
HRESULT SODaSServer::getChildNames(
	IN enum browserFilterType browseType,   // browse type
	IN LPCTSTR filterCriteria,              // string filter
	IN VARTYPE dataTypeFilter,              // data type filter
	IN DWORD accessRightsFilter,            // access filter
	OUT SOCmnStringList& nameList)          // list of dynamic child names
{
	getChildNamesOfNodeName(m_curBrowsePath, browseType, filterCriteria, dataTypeFilter, accessRightsFilter, nameList);
	return S_OK;
} // getChildNames


//-----------------------------------------------------------------------------
// getObjectSrvBrowser
//
// - get browser server object
//
// returns:
//      browser server object
//
SOCmnObject* SODaSServer::getObjectSrvBrowser(void)
{
	return (SOCmnObject*)this;
} // getObjectSrvBrowser


//-----------------------------------------------------------------------------
// isBranchName
//
// - check if the string represents a valid branch of the namespace tree
//
// returns:
//      TRUE  - valid branch name
//      FALSE - no branch with this name
//
BOOL SODaSServer::isBranchName(
	IN LPCTSTR fullName)    // branch name
{
	return isNodeName(fullName);
} // isBranchName


//-----------------------------------------------------------------------------
// isLeafName
//
// - check if the string represents a valid leaf of the namespace tree
//
// returns:
//      TRUE  - valid leaf name
//      FALSE - no leaf with this name
//
BOOL SODaSServer::isLeafName(
	IN LPCTSTR fullName)    // leaf name
{
	return isTagName(fullName);
} // isLeafName


//-----------------------------------------------------------------------------
// getBrowserRoot
//
// - get root object of browser tree
//
// returns:
//      namespace root
//
SOCmnElement* SODaSServer::getBrowserRoot(void)
{
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	return entry->getNameSpaceRoot(getServerInstance());
} // getBrowserRoot


//-----------------------------------------------------------------------------
// ignoreFilterTypeOnFlatOrg
//
// - ignore the filter if the namespace is flat
//
// returns:
//      TRUE for OPC DA
//
BOOL SODaSServer::ignoreFilterTypeOnFlatOrg(void)
{
	return TRUE;
} // ignoreFilterTypeOnFlatOrg


//-----------------------------------------------------------------------------
// matchStringFilter
//
// - string filter evaluation method
// - the default implementation takes the OPC Foundation algorithm
//
// returns:
//      TRUE  - string matches filter
//      FALSE - string don't matches filter
//
BOOL SODaSServer::matchStringFilter(
	IN LPCTSTR string,  // string
	IN LPCTSTR filter)  // filter
{
	return SOSrvServer::opcMatchStringFilter(string, filter, TRUE);
} // matchStringFilter


//-----------------------------------------------------------------------------
// matchAdditionalFilters
//
// - match the additional filters of DA
//
// returns:
//      TRUE  - object matches the filter
//      FALSE - object dont matches the filter
//
BOOL SODaSServer::matchAdditionalFilters(
	IN SOCmnElement* obj,           // object
	IN VARTYPE dataTypeFilter,      // datatype filter
	IN DWORD accessRightsFilter)    // access rights filter
{
	SODaSItemTag* tag = (SODaSItemTag*)obj;
	BOOL addToList = TRUE;

	// data type filter
	if (dataTypeFilter != VT_EMPTY)
	{
		if (dataTypeFilter != tag->getNativeDatatype())
		{
			addToList = FALSE;
		}
	}

	// access rights filter
	if (addToList)
	{
		if (accessRightsFilter != 0)
		{
			if ((accessRightsFilter & tag->getAccessRights(this)) == 0)
			{
				addToList = FALSE;
			}
		}
	}

	return addToList;
} // matchAdditionalFilters


//-----------------------------------------------------------------------------
// matchVendorFilter
//
// - match the vendor filter of DA 3.0
//
// returns:
//      TRUE  - object matches the filter
//      FALSE - object dont matches the filter
//
BOOL SODaSServer::matchVendorFilter(
	IN SOCmnElement* /* obj */,    // itemTag object
	IN LPCTSTR /* vendorFilter */) // vendor filter
{
	return TRUE;
} // matchVendorFilter


//-----------------------------------------------------------------------------
// isInvalidOrUnknownItemID
//
// - check if the item id is invalid or unknown
//
// returns:
//      TRUE  - invalid
//      FALSE - unknown
//
BOOL SODaSServer::isInvalidOrUnknownItemID(
	LPCTSTR itemID)
{
	if (_tcslen(itemID) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // isInvalidOrUnknownItemID



SODaSTag* SODaSServer::createStringBasedTag(LPCTSTR itemId)
{
	SODaSTag* tag = NULL;

	if (isTagName(itemId))
	{
		tag = createTag(itemId);
	}

	return tag;
}


SODaSNode* SODaSServer::createStringBasedNode(LPCTSTR itemId)
{
	SODaSNode* node = NULL;

	if (isNodeName(itemId))
	{
		node = createNode(itemId);
	}

	return node;
}


//-----------------------------------------------------------------------------
// getNameSpaceElement
//
// - get namespace element by it's id
//
// returns:
//      pointer namespace object
//    NULL - unknown id
//
SOCmnElement* SODaSServer::getNameSpaceElement(
	LPCTSTR itemId) // item id
{
	SOCmnPointer<SOCmnElement> element;
	element = SODaSNameSpaceRoot::getElement(itemId, m_serverInstance);

	if (!element)
	{
		element = createStringBasedTag(itemId);

		if (!element)
		{
			element = createStringBasedNode(itemId);
		}
	}

	element.addRef();
	return element;
} // getNameSpaceElement


//-----------------------------------------------------------------------------
// isNameSpaceElement
//
// - check if item id is a namespace element
//
// returns:
//      TRUE  - is valid item id
//    FALSE - invalid item id
//
BOOL SODaSServer::isNameSpaceElement(
	IN LPCTSTR itemId,
	OPTIONAL OUT SOCmnElement** nsElement)
{
	SOCmnPointer<SOCmnElement> element;

	if (!browseOnlyStringBased())
	{
		element = SODaSNameSpaceRoot::getElement(itemId, m_serverInstance);

		if ((!element) && (!isTagName(itemId)) && (!isNodeName(itemId)))
		{
			return FALSE;
		}
	}
	else
	{
		if ((!isTagName(itemId)) && (!isNodeName(itemId)))
		{
			return FALSE;
		}
	}

	if ((nsElement) && (element.isNotNull()))
	{
		*nsElement = (SOCmnElement*)element;
		(*nsElement)->addRef();
	}

	return TRUE;
} // isNameSpaceElement


//-----------------------------------------------------------------------------
// getItemTag
//
// - get (create) the item tag object by it's id
//
// returns:
//      S_OK                - return tag
//      OPC_E_UNKNOWNITEMID - unknown id
//    OPC_E_INVALIDITEMID - invalid id
//
HRESULT SODaSServer::getItemTag(
	IN LPCTSTR id,            // item id
	OUT SODaSItemTag** ppTag) // item tag object
{
	if (id[0] == _T('\0'))
	{
		return OPC_E_INVALIDITEMID;    // empty string -> error, not the root
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root;
	root = engine->getNameSpaceRoot(getServerInstance());

	if (!root)
	{
		return E_OUTOFMEMORY;
	}

	SOCmnPointer<SODaSTag> tag;
	SOCmnPointer<SODaSProperty> prop;
	SOCmnPointer<SODaSNode> node;
	SODaSItemTag* itemTag = NULL;
	LPCTSTR bad = NULL;
	TCHAR propDel;
	TCHAR* propStart = NULL;
	DWORD propId = 0;
	SOCmnString itemId = id;
	BOOL ok = TRUE;
	// check if item id contains property id
	propDel = engine->getPropertyDelimiter();

	if ((propStart = (LPTSTR)_tcschr(id, propDel)) != NULL)
	{
		// found property delimiter
		// remove property part from tag name
		*(((LPTSTR)(LPCTSTR)itemId) + (propStart - id)) = _T('\0');
		id = (LPCTSTR)itemId;
		propStart++;
		propId = getPropertyIDByItemID(id, propStart);

		if (propId == 0)
		{
			return OPC_E_UNKNOWNITEMID;
		}
	}

	// validate item id -> check for not allowed chars
	root->getTreeDelimiter(NULL, &bad);
	ok &= (_tcslen(id) != 0);
	ok &= (_tcspbrk(id, bad) == NULL);

	if (!ok)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; no valid id %s"), id))
		return OPC_E_INVALIDITEMID;
	}

	if (propStart == NULL)
	{
		// find namespace tag or node
		tag = (SODaSTag*)root->findLeaf(id, TRUE);

		if (tag.isNotNull())
		{
			itemTag = (SODaSItemTag*)tag;
		}
		else
		{
			tag = createStringBasedTag(id);

			if (tag.isNotNull())
			{
				itemTag = (SODaSItemTag*)tag;
			}

			if (!itemTag)
			{
				// check branch
				node = (SODaSNode*)root->findBranch(id, TRUE);

				if (!node)
				{
					node = createStringBasedNode(id);

					if (!node)
					{
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; unknown id %s"), id))
						return (isInvalidOrUnknownItemID(itemId)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID;
					}
					else
					{
						itemTag = (SODaSItemTag*)node;
					}
				}
				else
				{
					itemTag = (SODaSItemTag*)node;
				}

				if (itemTag)
				{
					if (itemTag->getDeviceIOMode(NULL) == SODaSItemTag::noDeviceIO)
					{
						itemTag = NULL;
						return OPC_E_UNKNOWNITEMID;
					}
				}
			}
		}
	}
	else
	{
		// find property
		SOCmnElement* propParent = NULL;
		tag = (SODaSTag*)root->findLeaf(id, TRUE);

		if (!tag)
		{
			tag = createStringBasedTag(id);
		}

		if (!!tag)
		{
			// property of tag
			propParent = (SODaSTag*)tag;
		}
		else
		{
			// may property of node
			node = (SODaSNode*)root->findBranch(id, TRUE);

			if (!node)
			{
				node = createStringBasedNode(id);
			}

			propParent = (SODaSNode*)node;
		}

		if (propParent)
		{
			prop = createProperty(propParent, id, propId, propStart);

			if (!prop)
			{
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; %s has no property %lu"), id, propId))
				return (isInvalidOrUnknownItemID(id)) ? OPC_E_INVALIDITEMID : OPC_E_UNKNOWNITEMID;
			}
		}
		else
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; %s is no node or tag"), id))
			return OPC_E_UNKNOWNITEMID;
		}

		itemTag = (SODaSItemTag*)prop;
	}

	if (ppTag)
	{
		*ppTag = itemTag;
		(*ppTag)->addRef();
	}

	return S_OK;
} // getItemTag


DWORD SODaSServer::getItemID(IN LPCTSTR itemPath, IN LPCTSTR itemName, OUT SOCmnString& itemID)
{
	if (itemPath)
		if (_tcslen(itemPath) > 0)
		{
			return OPC_E_UNKNOWNITEMPATH;
		}

	itemID = itemName;
	return S_OK;
}


//-----------------------------------------------------------------------------
// createTag
//
// - create object based tag from tag name
//
// returns:
//      pointer to tag object
//      NULL - can't create tag
//
SODaSTag* SODaSServer::createTag(
	IN LPCTSTR fullName)    // tag name
{
	SODaSTag* tag = NULL;

	if (isTagName(fullName))
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSNameSpaceRoot> root = engine->getNameSpaceRoot(getServerInstance());
		SOCmnString path(fullName);
		SOCmnPointer<SOCmnElement> node;
		LPTSTR nodeName = NULL;
		LPTSTR tagName = NULL;
		// get node of tag
		root->reducePath((LPCTSTR)path, nodeName, tagName, FALSE);

		if (nodeName)
		{
			// check if node object exists
			node = root->findBranch(nodeName, TRUE);

			if (!node)  // no node found
			{
				if (isNodeName(nodeName))
				{
					node = createNode(nodeName);    // create node
				}
			}
		}
		else
		{
			node = engine->getNameSpaceRoot(getServerInstance());
		}

		if (!!node)
		{
			// node found or created
			// create tag object
			SOCmnPointer<SODaSCreator> creator = engine->getCreator();
			tag = creator->createTag(fullName);

			if (!!tag)
			{
				tag->setName(tagName);
				// add tag to the namespace tree
				node->addLeaf(tag);
				initTag(tag);

				if (tag->getNativeDatatype() == VT_EMPTY)
				{
					tag->setNativeDatatype(getTagDatatypeByItemID(fullName));
				}

				if (tag->areDefaultAccessRights())
				{
					tag->setAccessRights(getAccessRightsByItemID(fullName));
				}

				tag->setDynamic(TRUE);
			}
		}
	}

	return tag;
} // createTag


//-----------------------------------------------------------------------------
// initTag
//
// - initialize created tag
//
void SODaSServer::initTag(IN SODaSTag* /* tag */)
{} // initTag


//-----------------------------------------------------------------------------
// createNode
//
// - create object based node from node name
//
// returns:
//      pointer to node object
//      NULL - can't create node
//
SODaSNode* SODaSServer::createNode(
	IN LPCTSTR fullName)    // node name
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root = engine->getNameSpaceRoot(getServerInstance());
	SOCmnPointer<SOCmnElement> parentNode;
	SOCmnString path(fullName);
	LPTSTR nodeName = NULL;
	LPTSTR remPath = NULL;
	SODaSNode* node = NULL;
	root->reducePath((LPCTSTR)path, remPath, nodeName, FALSE);

	// get parent node
	if (remPath)
	{
		// check if node object exists
		parentNode = root->findBranch(remPath, TRUE);

		if (!parentNode)    // no node found
		{
			if (isNodeName(remPath))
			{
				parentNode = createNode(remPath);    // create node
			}
		}
	}
	else
	{
		parentNode = engine->getNameSpaceRoot(getServerInstance());
	}

	// create node
	if (!!parentNode)
	{
		if (isNodeName(fullName))
		{
			SOCmnPointer<SODaSCreator> creator = engine->getCreator();
			node = creator->createNode(fullName);

			if (!!node)
			{
				node->setName(nodeName);
				// add node to the namespace tree
				parentNode->addBranch(node);
				initNode(node);
				node->setDynamic(TRUE);
			}
		}
	}

	return node;
} // createNode


//-----------------------------------------------------------------------------
// initNode
//
// - initialize created node
//
void SODaSServer::initNode(IN SODaSNode* /* pNode */)
{} // initNode


//-----------------------------------------------------------------------------
// createProperty
//
// - create object based property from property id and add it to the element
//
// returns:
//      pointer to node object
//      NULL - can't create node
//
SODaSProperty* SODaSServer::createProperty(
	IN SOCmnElement* nsElement,
	IN LPCTSTR itemId,
	IN DWORD propId,
	IN LPCTSTR propName)
{
	SOCmnList<SODaSProperty> *pList;
	SODaSProperty* prop = NULL;
	SODaSPropertyData propData;
	pList = (SOCmnList<SODaSProperty> *)nsElement->getAdditionalList();

	if (pList == NULL)  // no list can't add property
	{
		return NULL;
	}

	SOCmnList<SODaSProperty> propList(pList);

	if (!!propList)
	{
		prop = propList.findKey(propId);
	}

	if (!prop)
	{
		// no property with the id in elements try to create
		BOOL add = FALSE;

		if (nsElement->is(SOCMNOBJECT_TYPE_TAG))
		{
			if (isStandardTagProperty(propId))
			{
				add = TRUE;
			}
		}
		else
		{
			if (((SODaSItemTag*)nsElement)->getDeviceIOMode(NULL) != SODaSItemTag::noDeviceIO)
				if (isStandardTagProperty(propId))
				{
					add = TRUE;
				}

			if (isStandardNodeProperty(propId))
			{
				add = TRUE;
			}
		}

		if (!add)
			if (((SODaSItemTag*)nsElement)->hasProperty(propId, &propData))
			{
				add = TRUE;
			}

		if (!add)
			if (isPropertyName(itemId, propId, propName, &propData))
			{
				add = TRUE;
			}

		if (add)
		{
			SODaSItemTag* tag = (SODaSItemTag*)((SOCmnElement*)nsElement);
			prop = tag->addProperty(propId, propName);

			if (prop)
			{
				if (!isStandardTagProperty(propId))
				{
					if (tag->initProperty(prop, &propData) == FALSE)
					{
						initProperty(prop, &propData);
					}
				}

				prop->setDynamic(TRUE);

				if (propId == 2)
				{
					prop->setNativeDatatype(tag->getNativeDatatype());
				}
			}
		}
	}

	return prop;
} // createProperty


//-----------------------------------------------------------------------------
// initProperty
//
// - initialize created property
//
void SODaSServer::initProperty(
	IN SODaSProperty* /* property */,
	SODaSPropertyData* /* pPropData */)
{} // initProperty


//-----------------------------------------------------------------------------
// canCreateProperty
//
// - checks if a namespace element supports a given property id
//
// returns:
//      S_OK              - is property of tag
//      OPC_E_INVALID_PID - not property of tag
//
HRESULT SODaSServer::canCreateProperty(
	IN SOCmnElement* nsElement, // namespace element
	IN LPCTSTR itemId,          // itemId
	IN DWORD propId,            // property id
	IN SODaSPropertyData* pPropData)
{
	// standard property
	if (nsElement)
	{
		SOCmnList<SODaSProperty> *pList;
		pList = (SOCmnList<SODaSProperty> *)nsElement->getAdditionalList();

		if (pList == NULL)
		{
			return OPC_E_INVALID_PID;
		}

		if (nsElement->is(SOCMNOBJECT_TYPE_TAG))
		{
			if (isStandardTagProperty(propId))
			{
				return S_OK;
			}
		}
		else
		{
			if (((SODaSItemTag*)nsElement)->getDeviceIOMode(NULL) != SODaSItemTag::noDeviceIO)
				if (isStandardTagProperty(propId))
				{
					return S_OK;
				}

			if (isStandardNodeProperty(propId))
			{
				return S_OK;
			}
		}

		if (((SODaSItemTag*)nsElement)->hasProperty(propId, pPropData))
		{
			return S_OK;
		}

		SOCmnPointer<SODaSProperty> prop;
		SOCmnList<SODaSProperty> propList(pList);

		if (!!propList)
		{
			prop = propList.findKey(propId);

			if ((SODaSProperty*)prop)
			{
				return S_OK;
			}
		}

		if (isPropertyName(itemId, propId, NULL, pPropData))
		{
			return S_OK;    // dynamic property
		}
	}
	else
	{
		if (isPropertyName(itemId, propId, NULL, pPropData))
		{
			return S_OK;    // dynamic property
		}
	}

	return OPC_E_INVALID_PID;
} // canCreateProperty


//-----------------------------------------------------------------------------
// getPropertyIdByItemID
//
// - get the property id by the property name
//
// returns:
//      property id
//      0 - invalid property id
//
DWORD SODaSServer::getPropertyIDByItemID(
	IN LPCTSTR /* parentName */,    // name of properties parent in namespace tree
	IN LPCTSTR propName)            // property part of the item id
{
	return _tcstoul(propName, NULL, 0);
} // getPropertyIdByItemID


//-----------------------------------------------------------------------------
// getPropertyItemID
//
// - get the item id for a property
//
// returns:
//      S_OK              - is property of tag
//      OPC_E_INVALID_PID - not property of tag
//
HRESULT SODaSServer::getPropertyItemID(
	IN SOCmnElement* nsElement, // namespace element
	IN LPCTSTR itemId,                  // parents itemId
	IN DWORD propId,                        // property id
	OUT SOCmnString& propItemID)// property item id
{
	HRESULT res;

	if (SUCCEEDED(res = canCreateProperty(nsElement, itemId, propId, NULL)))
	{
		SOCmnPointer<SODaSEntry> entry;
		TCHAR propDelim;
		entry = ::getSODaSEntry();
		propDelim = entry->getPropertyDelimiter();
		propItemID.format(_T("%s%c%lu"), itemId, propDelim, propId);
	}

	return res;
} // getPropertyItemID


DWORD SODaSServer::getPropertyIDByName(IN LPCTSTR propName)
{
	return SODaSProperty::getStandardPropertyIDByName(propName);
}

LPCTSTR SODaSServer::getPropertyNameByID(IN DWORD propID, OUT SOCmnString& propName)
{
	return SODaSProperty::getStandardPropertyNameByID(propID, propName);
}

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// disconnectClient
//
// - disconnect the OPC client (forced shutdown)
//
void SODaSServer::disconnectClient(void)
{
	if (m_isDisconnectingClients)
	{
		return;
	}

	if (isPublicGroups())
	{
		return;
	}

	SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)getMe();

	if (srv == NULL)
	{
		return;
	}

	SOCmnString srvID = getObjTraceId();
	SOSrvComObject<SODaSComGroup> *grp;
	SOCmnList<SOCmnElement> groupList;
	SODaSGroup* group;
	SOCmnListPosition posGrp;
	DWORD extRefs;
	m_isDisconnectingClients = TRUE;
	_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC, (_T("disconnecting client %s"), (LPCTSTR)getName()))
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC, (_T("disconnecting refCnt:%u intRefCnt:%u"), srv->m_dwRef, (ULONG)srv->m_internalCnt))
	// remove the groups
	groupList = getBranchList();
	posGrp = groupList.getStartPosition();
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC, (_T("deactivating groups")))

	while (posGrp)
	{
		group = (SODaSGroup*)groupList.getNext(posGrp);
		group->setActive(FALSE);
	}

	posGrp = groupList.getStartPosition();

	while (posGrp)
	{
		group = (SODaSGroup*)groupList.getNext(posGrp);
		grp = (SOSrvComObject<SODaSComGroup> *)group->getMe();

		if (grp)
		{
			SOCmnString grpID = grp->getObjTraceId();
			extRefs = grp->m_dwRef - (grp->m_internalCnt + grp->m_tempCnt);
			_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC, grpID, (_T("disconnecting refCnt:%u intRefCnt:%u"), grp->m_dwRef, (ULONG)grp->m_internalCnt));
			removeGroup(group->getServerHandle(), TRUE);
			// release the internal reference added when creating the group via AddGroup
			// or CloneGroup (in SODaSComServer::createOPCGroup)
			grp->releaseComServer();
			// cut connection between COM group and COM server object
			// COM server object deleted afterwards.
			// if COM call is active on group CoDisconnectObject will not call the destructor of the COM group!

			if (grp->m_internalCnt > 0)
				while (grp->internalRelease())
					;

			if (extRefs)
			{
				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC, grpID, (_T("Call CoDisconnectObject refCnt:%u intRefCnt:%u"), grp->m_dwRef, (ULONG)grp->m_internalCnt));
				CoDisconnectObject((IOPCItemMgt*)grp, 0);
				_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC, grpID, (_T("Finished CoDisconnectObject")));
			}

			group->releaseMe(grp);  // use after release is ok in this case!
		}
	}

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC, (_T("disconnected all groups")))
	groupList.autoUnlock();
	srv->releaseAllAdvises();
	extRefs = srv->m_dwRef - (srv->m_internalCnt + srv->m_tempCnt);

	while (srv->internalRelease())
		;

	if (extRefs)
	{
		CoDisconnectObject((IOPCServer*)srv, 0);
	}

	releaseMe(srv); // use after release is ok in this case!
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC, srvID, (_T("disconnected client")))
} // disconnectClient


//-----------------------------------------------------------------------------
// handleLastExternalReleaseWithInternalRefs
//
// - released last external reference with pending internal references
//
// returns:
//      TRUE  - object deleted
//      FALSE - object still exists
//
BOOL SODaSServer::handleLastExternalReleaseWithInternalRefs(
	IN void* object,    // object with last external release
	IN void* comServer) // com server object
{
	SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)comServer;
	SOSrvComObject<SODaSComGroup> *grp;
	SOCmnList<SOCmnElement> groupList;
	SODaSGroup* group;
	SOCmnListPosition posGrp;
	BOOL ret = FALSE;

	if (isPublicGroups())
	{
		return FALSE;
	}

	if (!srv)
	{
		return FALSE;
	}

	// neeed no external server reference
	if (srv->m_dwRef > srv->m_internalCnt)
	{
		return FALSE;
	}

	srv->tempAddRef();
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OBJ_REF, (_T("no external COM references but internal")))

	if (object != (void*)comServer)
	{
		// group COM object released!
		grp = (SOSrvComObject<SODaSComGroup> *)object;

		if (grp->m_dwRef == grp->m_internalCnt)
		{
			// no external group reference !
			_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, grp->getObjTraceId(), (_T("disconnecting refCnt:%u intRefCnt:%u"), grp->m_dwRef, (ULONG)grp->m_internalCnt));
			grp->lockMe();
			grp->disuniteMe();
			grp->unlockMe();
			ret = TRUE;
			// remove group and all internal references
			removeGroup((OPCHANDLE)grp->m_me->getOpcHandle(), TRUE);

			if (grp->m_internalCnt > 0)
				while (grp->internalRelease())
					;
		}
	}

	// check all other groups!
	groupList = getBranchList();
	groupList.autoLock();
	posGrp = groupList.getStartPosition();

	while(posGrp)
	{
		group = (SODaSGroup *)groupList.getNext(posGrp);
		if (group != NULL)
		{	
			group->addRef();
			groupList.setDirty(FALSE);
			groupList.autoUnlock();

			grp = (SOSrvComObject<SODaSComGroup> *)group->getMe();

			if (grp)
			{
				if (grp->m_dwRef == grp->m_internalCnt)
				{ // no external group reference !
					// remove group and all internal references
					_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, grp->getObjTraceId(), (_T("disconnecting refCnt:%u intRefCnt:%u"), grp->m_dwRef, (ULONG)grp->m_internalCnt));
					grp->lockMe();
					grp->disuniteMe();
					grp->unlockMe();
					removeGroup(group->getServerHandle(), TRUE);
					if (grp->m_internalCnt > 0)
						while (grp->internalRelease())
							;
				}
				group->releaseMe(grp);
			}
			
			group->release();
			groupList.autoLock();
			if (groupList.isDirty())
			{
				posGrp = groupList.getStartPosition();
			}
		}
	}
	groupList.autoUnlock();//*57* moved upward to avoid possible deadlock (once occured on quad core PC)

	if (object == (void*)comServer)
	{
		ret = (srv->m_dwRef == 1);

		if (ret)
		{
			srv->lockMe();
			srv->disuniteMe();
			srv->unlockMe();
		}
	}

	srv->tempRelease();
	return ret;
} // handleLastExternalReleaseWithInternalRefs


//-----------------------------------------------------------------------------
// checkClientConnection
//
// - check if the connection to the client still exitsts
//
// returns:
//      TRUE  - client is running
//      FALSE - client does not react
//
BOOL SODaSServer::checkClientConnection(OPTIONAL IN BOOL checkAllIFs)
{
	BOOL conOK = TRUE;
	BOOL checked = FALSE;

	if (!isPublicGroups())
	{
		SOSrvComObject<SODaSComServer> *srv = (SOSrvComObject<SODaSComServer> *)getMe();

		if (!srv)
		{
			return FALSE;
		}

		if (srv->connectedShutdownInterface())
		{
			// OPC Spec V2.0 client with connected shutdown interface
			conOK = srv->checkClientConnection();

			if (!checkAllIFs)
			{
				checked = TRUE;
			}
		}

		if (!checked)
		{
			// OPC Spec V1.0A client or OPC Spec V2.0 client with no connected shutdown interface
			// connection check only possible if advised async group I/O
			int          nTmpGroups;
			SODaSGroup** pTmpGroupList = allocGroupsAsArray(&nTmpGroups);

			for (int i = 0; !checked && i < nTmpGroups;  i++)
			{
				SODaSGroup* pGroup = pTmpGroupList[i];
				SOCmnListPosition pos = pGroup->getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_ALL);
				{
					SODaSDataCallback advise;

					while (pos)
					{
						IUnknown* unk;
						HRESULT res;

						if (!checkAllIFs)
						{
							checked = TRUE;
						}

						pGroup->getNextAdvise(pos, advise, SODASDATACALLBACK_CT_ALL);

						if (advise.m_connectionType == SODASDATACALLBACK_CT_DATAOBJECT)
						{
							res = advise.m_adviseSink->QueryInterface(IID_IOPCServer, (void**)&unk);
						}
						else
						{
							res = advise.m_dataCallback->QueryInterface(IID_IOPCServer, (void**)&unk);
						}

						if (FAILED(res))
						{
							if (SOSRV_IS_RPC_ERROR(res))
							{
								conOK = FALSE;
								pos = NULL;
								break;
							}
						}
						else
						{
							unk->Release();
						}
					}
				}
				pGroup->doAdviseUnlock();
			}

			freeGroupsAsArray(pTmpGroupList);
		}

		releaseMe(srv);
	}

	return conOK;
} // checkClientConnection

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// addTransaction
//
// - add transaction to the servers transaction list
//
void SODaSServer::addTransaction(
	IN SODaSTransaction* trans) // transaction
{
	if (isPublicGroups())
	{
		return;
	}

	m_transactionList.lock();

	if (!trans->isCyclic())
	{
		m_transactionList.addHead(trans);
	}
	else
	{
		m_transactionList.add(trans);
	}

	m_transactionList.unlock();
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SRV_THREAD, (_T("added transaction %s"), trans->getObjTraceId()))
	signalWorkEvent();
} // addTransaction


//-----------------------------------------------------------------------------
// removeTransaction
//
// - remove transaction from the servers transaction list
//
// returns:
//      TRUE  - removed transaction
//      FALSE - can't remove transaction
//
BOOL SODaSServer::removeTransaction(
	IN SODaSTransaction* trans) // transaction
{
	if (isPublicGroups())
	{
		return FALSE;
	}

	SODaSTransaction* tac = NULL;
	SOCmnListPosition pos = NULL;
	m_transactionList.lock();

	if (trans)
	{
		pos = m_transactionList.findObject(trans);
		tac = trans;
	}

	if (pos)
	{
		tac->addRef();
		m_transactionList.removeAt(pos);
		m_transactionList.unlock();
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SRV_THREAD, (_T("removed transaction %s"), tac->getObjTraceId()))
		tac->cancelRequests();
		tac->release();
		return TRUE;
	}
	else
	{
		m_transactionList.unlock();
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SRV_THREAD, (_T("< removeTransaction; transaction not found")))
		return FALSE;
	}
} // removeTransaction


//-----------------------------------------------------------------------------
// cancelTransaction
//
// - cancel an transaction
//
// returns:
//      S_OK   - transaction canceled
//      E_FAIL - nothing to cancel
//
HRESULT SODaSServer::cancelTransaction(
	IN DWORD transId)
{
	if (transId == 0)
	{
		return E_FAIL;    // tried to cancel cyclic transaction
	}

	SOCmnPointer<SODaSTransaction> tac;
	SODaSTransaction* pTac;
	SOCmnListPosition pos, curPos;
	HRESULT res = E_FAIL;
	m_transactionList.lock();
	pos = m_transactionList.getStartPosition();

	while (pos)
	{
		curPos = pos;
		pTac = m_transactionList.getNext(pos);

		if (pTac->getTransactionId() == transId)
		{
			tac = pTac;
			tac.addRef();
			m_transactionList.removeAt(curPos);
			break;
		}
	}

	m_transactionList.unlock();

	if (tac.isNotNull())
	{
		// found the transaction
		SOCmnList<SODaSRequest> reqList = tac->getRequestList();
		reqList.lock();

		if (tac->isStarted())
		{
			tac->cancel(this);
			res = S_OK;
		}

		reqList.unlock();
	}

	return res;
} // cancelTransaction

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//      E_NOTIMPL     - no query interface implementation for this interface
//      S_OK          - interface delivered
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaSServer::queryInterface(
	IN REFIID /* iid */,      // interface id
	IN void** /* object */)   // interface pointer
{
	return E_NOTIMPL;    // queryInterface
}

#endif

//-----------------------------------------------------------------------------
// getSpecID
//
// - get the specification id of the server
//
// returns:
//      SOSRVSERVER_SPECID_DA - data access server object
//
DWORD SODaSServer::getSpecID(void)
{
	return SOSRVSERVER_SPECID_DA;    // getSpecID
}


//-----------------------------------------------------------------------------
// isStandardTagProperty
//
// - check if the property id is a standard tag property id
//
// returns:
//      TRUE  - standard propery
//      FALSE - no standard propery
//
BOOL SODaSServer::isStandardTagProperty(
	IN DWORD propId)    // property id
{
	return SODaSProperty::isStandardTagProperty(propId);
} // isStandardTagProperty


//-----------------------------------------------------------------------------
// isStandardNodeProperty
//
// - check if the property id is a standard node property id
//
// returns:
//      TRUE  - standard propery
//      FALSE - no standard propery
//
BOOL SODaSServer::isStandardNodeProperty(
	IN DWORD propId)    // property id
{
	return SODaSProperty::isStandardNodeProperty(propId);
} // isStandardNodeProperty


//-----------------------------------------------------------------------------
// getStandardTagProperties
//
// - get the standard properties of a tag
//
void SODaSServer::getStandardTagProperties(
	IN VARTYPE tagNativeDatatype,                   // native datatype of the tag
	IN BOOL isSOAP,
	OUT SOCmnList<SODaSPropertyData> &propertyList) // property data list
{
	SODaSProperty::getStandardTagProperties(tagNativeDatatype, isSOAP, propertyList);
} // getStandardTagProperties


//-----------------------------------------------------------------------------
// getStandardNodeProperties
//
// - get the standard properties of a node
//
void SODaSServer::getStandardNodeProperties(
	IN BOOL isSOAP,
	OUT SOCmnList<SODaSPropertyData> &propertyList) // property data list
{
	SODaSProperty::getStandardNodeProperties(isSOAP, propertyList);
} // getStandardNodeProperties

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// getCLSID
//
// - get the CLSID of the sever
//
void SODaSServer::getCLSID(
	OUT CLSID& classID) // class id
{
	switch (m_serverInstance)
	{
	case 0:
		classID = g_clsidDA0;
		break;

	case 1:
		classID = g_clsidDA1;
		break;

	case 2:
		classID = g_clsidDA2;
		break;

	case 3:
		classID = g_clsidDA3;
		break;

	case 4:
		classID = g_clsidDA4;
		break;

	case 5:
		classID = g_clsidDA5;
		break;

	case 6:
		classID = g_clsidDA6;
		break;

	case 7:
		classID = g_clsidDA7;
		break;

	case 8:
		classID = g_clsidDA8;
		break;

	case 9:
		classID = g_clsidDA9;
		break;
	}
} // getCLSID

#endif

//-----------------------------------------------------------------------------
// waitForNameSpaceStartup
//
// - wait for the name space creation to start up
//
// returns:
//      S_OK - continue COM interface function call
//      any error returns COM interface with this error
//
HRESULT SODaSServer::waitForNameSpaceStartup(void)
{
	return S_OK;
} // waitForNameSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//      S_OK - continue COM interface function call
//      any error returns COM interface with this error
//
HRESULT SODaSServer::waitForIOStartup(void)
{
	return S_OK;
} // waitForIOStartup


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSServer::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_SERVER);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOCmnDateTime lastUpdateTime;
#ifdef SOFEATURE_DCOM
		CLSID clsid;
#endif
#ifdef SOFEATURE_DCOM
		getCLSID(clsid);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLSID_T, conv->CLSID2string(buffer, clsid));
#endif
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLIENTNAME_T, getClientName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_LCID_T, conv->LCID2string(buffer, getLCID()));
#ifdef SOFEATURE_DCOM
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SERVERSTATE_T, conv->serverState2string(buffer, entry->getServerState()));
#endif
		getUpdateTime(lastUpdateTime);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_LASTUPDATETIME_T, conv->datetime2string(buffer, lastUpdateTime));

		if (getBandwidth() != 0xFFFFFFFF)
		{
			buffer.format(_T("%lu%%"), getBandwidth());
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_BANDWIDTH_T, buffer);
		}

		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData
#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// browse
//
// - browse the namespace elements
//
// returns:
//    S_OK - The function was successful.
//    OPC_E_INVALIDFILTER - The filter string (element or vendor) is not valid.
//    OPC_E_INVALIDCONTINUATIONPOINT - The specified continuation point is not valid.
//    E_OUTOFMEMORY - The operation could not be executed due to memory limitations.
//    E_FAIL - The operation failed.
//
HRESULT SODaSServer::browse(
	IN LPCTSTR itemId,               // item id
	IN SOCmnElement* pElement,       // item tag (may be NULL)
	IN enum browserFilterType fType, // browse type
	IN LPCTSTR elementNameFilter,    // string filter
	IN LPCTSTR vendorFilter,         // vendor filter
	IN DWORD maxElementsReturned,    // max elements to return
	IN OUT SOCmnString& contPoint,   // continuation point
	OUT BOOL& moreElements,          // have more elements (only if no continuation point returned)
	OUT SOCmnList<SODaSBrowseElementData> *pListBE) // browse elements list
{
	SOCmnList<SODaSBrowseElementData> listBE(pListBE);
	SODaSBrowseElementData* pBE;
	HRESULT res = S_OK;
	SOCmnString cpSData;
	SOCmnListPosition cpOPos = NULL;
	TCHAR cpOList = _T(' ');
	BYTE cpType = 0;
	DWORD elRetCnt = 0;
	BOOL maxElements = FALSE;
	moreElements = FALSE;

	// check continuation point
	if (!contPoint.isEmpty())
	{
		DWORD cpLen = contPoint.getLength();

		if (cpLen < 3)
		{
			return OPC_E_INVALIDCONTINUATIONPOINT;
		}

		LPCTSTR cp = (LPCTSTR)contPoint;

		if (_tcsncmp(cp, _T("O:"), 2) == 0)
		{
			// object based
#ifdef WIN64
			if ((cpLen != 28) || ((cp[2] != _T('N')) && (cp[2] != _T('T'))) || ((cp[2] == _T('T')) && (fType == typeBranch)) || (cp[19] != _T('H')))
			{
				return OPC_E_INVALIDCONTINUATIONPOINT;
			}

			SOCmnString hash = contPoint.right(8);
			SOCmnString toHash = contPoint.left(20);

			if (toHash.hashDJB2(m_secretString) != hash)
			{
				return OPC_E_INVALIDCONTINUATIONPOINT;
			}

			SOCmnString half = contPoint.mid(3,8);
			ptrdiff_t ptr = _tcstoul((LPCTSTR)half, NULL, 16);
			half = contPoint.mid(11,8);
			ptr <<= 32;
			ptr += _tcstoul((LPCTSTR)half, NULL, 16);
			
			cpOPos = (SOCmnListPosition)ptr;
#else
			if ((cpLen != 20) || ((cp[2] != _T('N')) && (cp[2] != _T('T'))) || ((cp[2] == _T('T')) && (fType == typeBranch)) || (cp[11] != _T('H')))
			{
				return OPC_E_INVALIDCONTINUATIONPOINT;
			}

			SOCmnString hash = contPoint.right(8);
			SOCmnString toHash = contPoint.left(12);

			if (toHash.hashDJB2(m_secretString) != hash)
			{
				return OPC_E_INVALIDCONTINUATIONPOINT;
			}

			cpOPos = (SOCmnListPosition)_tcstoul(&cp[3], NULL, 16);
#endif
			cpOList = cp[2];
			cpType = 1;
		}
		else if (_tcsncmp(cp, _T("S:"), 2) == 0)
		{
			// string based
			cpSData = contPoint.right(cpLen - 2);
			cpType = 2;
		}
		else
		{
			return OPC_E_INVALIDCONTINUATIONPOINT;
		}
	}

	contPoint.empty();

	if ((!browseOnlyStringBased()) && (pElement != NULL) && (cpType != 2))
	{
		//-- object based --
		SOCmnListPosition pos;
		SOCmnListPosition posCur;
		BOOL addToList;
		DWORD iLst = 0;

		for (iLst = 0; iLst < 2; iLst++)
		{
			if (((iLst == 0) && ((cpType == 0) || ((cpType == 1) && (cpOList == _T('N'))))) ||    // branches
				((iLst == 1) && ((((DWORD)fType) & SOSRVBROWSER_TYPE_ITEM) == SOSRVBROWSER_TYPE_ITEM) && ((cpType == 0) || ((cpType == 1) && (cpOList == _T('T'))))))
			{
				SOCmnList<SOCmnElement> list;
				SODaSItemTag* pIT;
				BOOL hasChildren;
				BOOL isItem;

				if (iLst == 0)
				{
					list = pElement->getBranchList();
				}
				else
				{
					list = pElement->getLeafList();
				}

				if (list.isInit())
				{
					pos = list.getStartPosition();

					if (cpType == 1)
					{
						// find continuation position
						DWORD oT;
						DWORD iT;
						SOCMN_TRY
						{
							pos = cpOPos;
							pIT = (SODaSItemTag*)list.getNext(pos);
							oT = pIT->getObjectType()& SOCMNOBJECT_TYPE_ITEM_TAG;
							iT = SOCMNOBJECT_TYPE_ITEM_TAG;

							if (oT != iT)
							{
								res = OPC_E_INVALIDCONTINUATIONPOINT;
							}
						}
						SOCMN_CATCH(...)
						{
							res = OPC_E_INVALIDCONTINUATIONPOINT;
						}
					}

					cpType = 0;

					if ((SUCCEEDED(res)) && (pos))
					{
						while ((pos) && (elRetCnt <= maxElementsReturned))
						{
							posCur = pos;
							pIT = (SODaSItemTag*)list.getNext(pos);
							addToList = TRUE;
							isItem = ((pIT->getDeviceIOMode(NULL) != SODaSItemTag::noDeviceIO));

							if (pIT->is(SOCMNOBJECT_TYPE_NODE))
							{
								hasChildren = pIT->hasChildren();
							}
							else
							{
								hasChildren = FALSE;
							}

							// check browsable state of item tag
							if (!pIT->isBrowsable(this))
							{
								addToList = FALSE;
							}

							// filter type
							if ((addToList) && ((((DWORD)fType) & SOSRVBROWSER_TYPE_BRANCH) == 0)) // take only items
								if (!isItem)
								{
									addToList = FALSE;
								}

							if ((addToList) && ((elementNameFilter) && (_tcslen(elementNameFilter) != 0)))
							{
								// string filter
								addToList = matchStringFilter(pIT->getName(), elementNameFilter);
							}

							if ((addToList) && ((vendorFilter) && (_tcslen(vendorFilter) != 0)))
							{
								// vendor filter
								addToList = matchVendorFilter(pIT, vendorFilter);
							}

							if (addToList)
							{
								if (maxElements)
								{
									moreElements = TRUE;
									return S_OK;
								}

								SOCMN_ALLOCATE_OBJECT_C(pBE, SODaSBrowseElementData, (SODaSBrowseElementData*))
								pBE->m_name = pIT->getName();
								pBE->m_itemID = pIT->getItemID();
								pBE->m_datatype = pIT->getNativeDatatype();
								pBE->m_accessRights = pIT->getAccessRights();
								pBE->m_flags = 0;

								if (isItem)
								{
									pBE->m_flags |= OPC_BROWSE_ISITEM;
								}

								if (hasChildren)
								{
									pBE->m_flags |= OPC_BROWSE_HASCHILDREN;
								}

								pBE->m_nsElement = pIT;
								pBE->m_nsElement.addRef();
								listBE.add(pBE, pBE->m_itemID);
								elRetCnt++;

								if (elRetCnt == maxElementsReturned) // reached max elements
								{
									// set continuation point
									maxElements = TRUE;
									SOCmnString hash;

									if (iLst == 0)
									{
#ifdef WIN64
										contPoint.format(_T("O:N%16.16lXH"), (ptrdiff_t)posCur);
#else
										contPoint.format(_T("O:N%8.8lXH"), (DWORD)posCur);
#endif
									}
									else
									{
#ifdef WIN64
										contPoint.format(_T("O:T%16.16lXH"), (ptrdiff_t)posCur);
#else
										contPoint.format(_T("O:T%8.8lXH"), (DWORD)posCur);
#endif
									}
									hash = contPoint.hashDJB2(m_secretString);
									contPoint += hash;
								}
							}
						}
					}
				}
				else if (cpType != 0)
				{
					res = OPC_E_INVALIDCONTINUATIONPOINT;
				}

				if (FAILED(res))
				{
					return res;
				}
			}
		}
	}

	contPoint.empty(); // continuation point set before in not valid because we have not got another element in the object based area
	//-- string based --
	// get the children from the application
	res = getChildrenData(itemId, fType, elementNameFilter, vendorFilter, maxElementsReturned - elRetCnt, cpSData, moreElements, pListBE);

	if ((SUCCEEDED(res)) && (!cpSData.isEmpty()))
	{
		contPoint.format(_T("S:%s"), (LPCTSTR)cpSData);
	}

	if (!contPoint.isEmpty())
		if (_tcsncmp((LPCTSTR)contPoint, _T("O:"), 2) == 0)
		{
			contPoint.empty();    // max element was last element
		}

	return res;
} // browse


//-----------------------------------------------------------------------------
// getChildrenData
//
// - browse the string-based namespace elements
//
// returns:
//    S_OK - The function was successful.
//    OPC_E_INVALIDFILTER - The filter string (element or vendor) is not valid.
//    OPC_E_INVALIDCONTINUATIONPOINT - The specified continuation point is not valid.
//    E_OUTOFMEMORY - The operation could not be executed due to memory limitations.
//    E_FAIL - The operation failed.
//
HRESULT SODaSServer::getChildrenData(
	IN LPCTSTR /* itemId */,               // item id
	IN enum browserFilterType /* fType */, // browse type
	IN LPCTSTR /* elementNameFilter */,    // string filter
	IN LPCTSTR /* vendorFilter */,         // vendor filter
	IN DWORD /* maxElementsReturned */,    // max elements to return
	IN OUT SOCmnString& /* contPoint */,   // continuation point
	OUT BOOL& /* moreElements */,          // have more elements (only if no continuation point returned)
	OUT SOCmnList<SODaSBrowseElementData>* /* pListBE */) // browse elements list
{
	return S_OK;
} // getChildrenData


//-----------------------------------------------------------------------------
// getProperties
//
// - get properties of namespace elements
//
// returns:
//    S_OK - The function was successful.
//    S_FALSE - The operation succeeded but there are one or more errors in
//               ppItemProperties. Refer to individual error returns for more information.
//               S_FALSE is also returned, if hrErrorID of one OPCITEMPROPERTIES structure is S_FALSE.
//    E_OUTOFMEMORY - Not enough Memory.
//    E_INVALIDARG - An invalid argument was passed.
//    E_FAIL - The function failed.
//
HRESULT SODaSServer::getProperties(
	IN LPCTSTR itemId,            // item id
	IN SOCmnElement* pElement,    // item tag
	IN DWORD propCnt,             // number of properties
	IN DWORD* pPropIDs,           // property id's
	IN BOOL isSOAP,
	IN BOOL returnPropertyValues,
	OUT SOCmnList<SODaSPropertyData> *pListPD) // property data list
{
	HRESULT res = S_OK;
	SOCmnList<SODaSPropertyData> propList;
	SOCmnListPosition pos;
	SODaSPropertyData* propData;
	BOOL addToList;
	USES_CONVERSION;

	// create output list
	if (!pListPD->isInit())
	{
		pListPD->create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	}

	if ((propCnt == 0) || (pPropIDs == NULL))
	{
		propList = pListPD;    // all properties
	}
	else
	{
		propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	}

	// get static properties
	if (!!pElement)
	{
		SOCmnList<SODaSProperty> *pList;
		pList = (SOCmnList<SODaSProperty> *)pElement->getAdditionalList();

		if (pList != NULL)
		{
			SOCmnList<SODaSProperty> ePropList(pList);
			SODaSItemTag* pItemTag = (SODaSItemTag*)((SOCmnElement*)pElement);

			if (pElement->is(SOCMNOBJECT_TYPE_TAG))
			{
				getStandardTagProperties(pItemTag->getNativeDatatype(), isSOAP, propList);
			}
			else
			{
				if (pItemTag->getDeviceIOMode(NULL) != SODaSItemTag::noDeviceIO)
				{
					getStandardTagProperties(pItemTag->getNativeDatatype(), isSOAP, propList);
				}

				getStandardNodeProperties(isSOAP, propList);
			}

			if (!pItemTag->getProperties(isSOAP, &propList))
			{
				if (!!ePropList)
				{
					SODaSProperty* prop;
					pos = ePropList.getStartPosition();

					while (pos)
					{
						prop = ePropList.getNext(pos);

						if (pElement->is(SOCMNOBJECT_TYPE_TAG))
						{
							addToList = !isStandardTagProperty(prop->getPropertyId());
						}
						else
						{
							addToList = !isStandardNodeProperty(prop->getPropertyId());
						}

						if (addToList)
						{
							addToList = !pItemTag->hasProperty(prop->getPropertyId(), NULL);
						}

						if (addToList)
						{
							switch (prop->getPropertyId())
							{
							case 7:
								addToList = !isSOAP;
								break;

							case 2507:
								addToList = isSOAP;
								break;
							}
						}

						if (addToList)
						{
							SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
							prop->getPropertyData(propData);
							propList.add(propData);
						}
					}
				}
			}
		}

		// get dynamic properties
		if (!propList.isEmpty())
		{
			SOCmnList<SODaSPropertyData> dynList;
			dynList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			queryAvailablePropertiesByItemID(itemId, isSOAP, returnPropertyValues, dynList);

			if (!dynList.isEmpty())
			{
				SOCmnListPosition posS;
				SODaSPropertyData* propDataS;
				// make one list out of the two and remove duplicates
				pos = dynList.getStartPosition();

				while (pos)
				{
					propData = dynList.getNext(pos);
					addToList = TRUE;
					posS = propList.getStartPosition();

					while (posS)
					{
						propDataS = propList.getNext(posS);

						if (propData->m_pid == propDataS->m_pid)
						{
							addToList = FALSE;
							break;
						}
					}

					if (addToList)
					{
						SODaSPropertyData* propDataC;
						SOCMN_ALLOCATE_OBJECT_C(propDataC, SODaSPropertyData, (SODaSPropertyData*))
						*propDataC = *propData;
						propList.add(propDataC);
					}
				}
			}

			dynList.destroy();
		}
		else
		{
			queryAvailablePropertiesByItemID(itemId, isSOAP, returnPropertyValues, propList);
		}
	}
	else
	{
		// get dynamic properties
		queryAvailablePropertiesByItemID(itemId, isSOAP, returnPropertyValues, propList);
	}

	if ((propCnt > 0) && (pPropIDs != NULL))
	{
		// reduce property list
		DWORD i;
		BOOL found;

		for (i = 0; i < propCnt; i++)
		{
			found = FALSE;
			pos = propList.getStartPosition();

			while (pos)
			{
				propData = propList.getNext(pos);

				if (propData->m_pid == pPropIDs[i])
				{
					SODaSPropertyData* propDataC;
					SOCMN_ALLOCATE_OBJECT_C(propDataC, SODaSPropertyData, (SODaSPropertyData*))
					*propDataC = *propData;
					pListPD->add(propDataC);
					found = TRUE;
					break;
				}

				if ((pPropIDs[i] > 2500) && (pPropIDs[i] < 5000))
				{
					if (propData->m_pid == (pPropIDs[i] - 2500))
					{
						SODaSPropertyData* propDataC;
						SOCMN_ALLOCATE_OBJECT_C(propDataC, SODaSPropertyData, (SODaSPropertyData*))
						*propDataC = *propData;
						propDataC->m_pid += 2500;
						pListPD->add(propDataC);
						found = TRUE;
						break;
					}
				}
			}

			if (!found)
			{
				SODaSPropertyData* propDataE;
				SOCMN_ALLOCATE_OBJECT_C(propDataE, SODaSPropertyData, (SODaSPropertyData*))
				propDataE->m_result = OPC_E_INVALID_PID;
				propDataE->m_pid = pPropIDs[i];
				pListPD->add(propDataE);
				res = S_FALSE;
			}
		}

		propList.destroy();
	}

	return res;
} // getProperties


HRESULT SODaSServer::readProperties(
	IN LPCTSTR itemId,
	IN SOCmnElement* pElement,
	IN DWORD accessRights,
	IN SOCmnList<SODaSPropertyData> *pListPD,
	IN DWORD propCnt,
	IN DWORD* pPropIDs,
	IN BOOL isSOAP,
	OUT OPCITEMPROPERTIES* pItemProperties,
	OUT LPVARIANT pVariants,
	OUT SOCmnVariant** ppVar,
	OUT HRESULT* pErrors)
{
	HRESULT res = S_OK;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSProperty> prop;
	SOCmnPointer<SODaSRequest> req;
	SOCmnPointer<SODaSTransaction> tac;
	DWORD i;
	DWORD count = 0;
	BOOL useList;
	BOOL useItemProperties;

	if ((pListPD) && (pListPD->isInit()) && (pItemProperties != NULL))
	{
		count = pListPD->getCount();
		useList = TRUE;
		useItemProperties = TRUE;
	}
	else if ((propCnt > 0) && (pPropIDs) && ((pVariants) || (ppVar)) && (pErrors))
	{
		count = propCnt;
		useList = FALSE;
		useItemProperties = FALSE;
	}
	else if ((pListPD) && (pListPD->isInit()) && ((pVariants) || (ppVar)) && (pErrors))
	{
		count = pListPD->getCount();
		useList = TRUE;
		useItemProperties = FALSE;
	}
	else
	{
		return E_FAIL;
	}

	// create transaction
	tac = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(count);
	}
	// for every property id
	SOCmnEvents events;
	DWORD reqEventId = events.create();
	DWORD propID;
	SOCmnListPosition pos;
	SODaSPropertyData* pPD;
#ifdef SOFEATURE_DCOM
	LPVARIANT pValue = NULL;
#endif
	SOCmnVariant* pV = NULL;
	HRESULT* pError;

	if (useList)
	{
		pos = pListPD->getStartPosition();
	}

	for (i = 0; ((SUCCEEDED(res)) && (i < count)); i++)
	{
		if (useList)
		{
			pPD = pListPD->getNext(pos);

			if (FAILED(pPD->m_result))
			{
				res = S_FALSE;
				continue;
			}

			propID = pPD->m_pid;
		}
		else
		{
			propID = pPropIDs[i];
		}

		if (useItemProperties)
		{
#ifdef SOFEATURE_DCOM
			pValue = &pItemProperties->pItemProperties[i].vValue;
#endif
			pError = &pItemProperties->pItemProperties[i].hrErrorID;
		}
		else
		{
#ifdef SOFEATURE_DCOM

			if (pVariants)
			{
				pValue = &pVariants[i];
			}
			else
			{
				pValue = NULL;
			}

#endif

			if (ppVar)
			{
				pV = ppVar[i];
			}
			else
			{
				pV = NULL;
			}

			pError = &pErrors[i];
		}

		// check the access rights
		if (!(accessRights & OPC_READABLE))
			if (((propID >= 2) && (propID <= 4)) || (propID == 2503))
			{
				if (!isSOAP)
				{
					*pError = OPC_E_BADRIGHTS;
				}
				else
				{
					*pError = OPC_E_WRITEONLY;
				}

#ifdef SOFEATURE_DCOM

				if (pValue)
				{
					::VariantInit(pValue);
				}

#endif

				if (pV)
				{
					pV->clear();
				}

				res = S_FALSE;
				continue;
			}

		prop = createProperty(pElement, itemId, propID, NULL);
#ifdef SOFEATURE_DCOM

		if (pValue)
		{
			::VariantInit(pValue);
		}

#endif

		if (pV)
		{
			pV->clear();
		}

		if ((SODaSProperty*)prop)
		{
			// sync property requests
			*pError = S_OK;
			req = creator->createRequest(this, NULL, prop, SODaSRequest::read, SODaSRequest::serverDevice);

			if (req.isNotNull())
			{
				if (isSOAP)
				{
					req->setFlag(SODASREQUEST_FLAG_SOAP);
				}

				req->setItemTag(prop);
				req->setServer(this);
				req->setResultAddress(pError);
				req->setResult(S_OK);
#ifdef SOFEATURE_DCOM

				if (pValue)
				{
					req->setReadValueAddress(pValue);
				}

#endif

				if (pV)
				{
					req->setReadValueAddress(pV);
				}

				req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
				tac->insertRequest(req);
				req.release();
			}
		}
		else
		{
			*pError = OPC_E_INVALID_PID;
			res = S_FALSE;
		}
	}

	if ((SUCCEEDED(res)) && (tac->getRequestCount() > 0))
	{
		tac->execute(this, &events, reqEventId, m_dueTimeRead);
	}

	tac->setCompletedEvent(NULL);
	tac->emptyRequestsCompletedEvent();

	if (SUCCEEDED(res))
	{
		// check for read errors
		for (i = 0; i < count; i++)
		{
			if (useItemProperties)
			{
				if (FAILED(pItemProperties->pItemProperties[i].hrErrorID))
				{
					res = S_FALSE;
				}
				else
				{
					if (pItemProperties->pItemProperties[i].vtDataType == VT_EMPTY)
					{
						pItemProperties->pItemProperties[i].vtDataType = pItemProperties->pItemProperties[i].vValue.vt;
					}
				}
			}
			else
			{
				if (FAILED(pErrors[i]))
				{
					res = S_FALSE;
					break;
				}
			}
		}
	}

	return res;
} // readProperties


#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_COMIF_HOOKS
HRESULT SODaSServer::onBSetLocaleID(IN LCID dwLcid)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  lcid:0x%X"), dwLcid));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetLocaleID(OUT LCID* pdwLcid)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBQueryAvailableLocaleIDs(OUT DWORD* pdwCount, OUT LCID** pdwLcid)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetErrorString(IN HRESULT dwError, OUT LPWSTR* ppString)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorCode:0x%X"), dwError));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBSetClientName(IN LPCWSTR szName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBAdvise(IN LPUNKNOWN unknown, OUT DWORD* cookie)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBUnadvise(IN DWORD cookie)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), cookie));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBAddGroup(IN LPCWSTR szName, IN BOOL bActive, IN DWORD dwRequestedUpdateRate, IN OPCHANDLE hClientGroup, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD dwLCID, OUT OPCHANDLE* phServerGroup, OUT DWORD* pRevisedUpdateRate, IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s act:%d upd:%lu hClt:0x%X"), W2CT(szName), bActive, dwRequestedUpdateRate, hClientGroup));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetErrorString(IN HRESULT dwError, IN LCID dwLocale, OUT LPWSTR* ppString)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorCode:0x%X lcid:%0xX"), dwError, dwLocale));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetGroupByName(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetStatus(OUT OPCSERVERSTATUS** ppServerStatus)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBRemoveGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  hSrv:0x%X"), hServerGroup));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBCreateGroupEnumerator(IN OPCENUMSCOPE dwScope, IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  scope:%lu"), dwScope));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetPublicGroupByName(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBRemovePublicGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  hSrv:0x%X"), hServerGroup));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBQueryAvailableProperties(IN LPWSTR szItemID, OUT DWORD* pdwCount, OUT DWORD** ppPropertyIDs, OUT LPWSTR** ppDescriptions, OUT VARTYPE** ppvtDataTypes)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  itemID:%s"), W2CT(szItemID)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetItemProperties(IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, OUT VARIANT** ppvData, OUT HRESULT** ppErrors)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  itemID:%s cnt:%lu"), W2CT(szItemID), dwCount));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBLookupItemIDs(IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, OUT LPWSTR** ppszNewItemIDs, OUT HRESULT** ppErrors)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  itemID:%s cnt:%lu"), W2CT(szItemID), dwCount));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBQueryOrganization(OUT OPCNAMESPACETYPE* pNameSpaceType)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBChangeBrowsePosition(IN OPCBROWSEDIRECTION dwBrowseDirection, IN LPCWSTR szString)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  dir:%lu str:%s"), dwBrowseDirection, W2CT(szString)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBBrowseOPCItemIDs(IN OPCBROWSETYPE dwBrowseFilterType, IN LPCWSTR szFilterCriteria, IN VARTYPE vtDataTypeFilter, IN DWORD dwAccessRightsFilter, OUT LPENUMSTRING* ppIEnumString)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  ft:%lu sf:%s df:%u af:%lu"), dwBrowseFilterType, W2CT(szFilterCriteria), vtDataTypeFilter, dwAccessRightsFilter));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetItemID(IN LPWSTR szItemDataID, OUT LPWSTR* szItemID)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  itemID:%s"), W2CT(szItemDataID)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBBrowseAccessPaths(IN LPCWSTR szItemID, OUT LPENUMSTRING* ppIEnumString)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  itemID:%s"), W2CT(szItemID)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetClassID(OUT CLSID* pClassID)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBIsDirty(void)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBLoad(IN LPCOLESTR pszFileName, IN DWORD dwMode)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  fileName:%s"), W2CT(pszFileName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBSave(IN LPCOLESTR pszFileName, IN BOOL fRemember)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  fileName:%s"), W2CT(pszFileName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBSaveCompleted(IN LPCOLESTR pszFileName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  fileName:%s"), W2CT(pszFileName)));
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetCurFile(OUT LPOLESTR* ppszFileName)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetChildren(IN LPCWSTR parent, IN BOOL withAttributes, OUT LPWSTR* children)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBCollectChanges(IN LPCWSTR objects, IN DWORD doCollect)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetChanges(OUT LPWSTR* changes)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetStatistics(OUT LPWSTR* statistics)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetObject(IN LPCWSTR objectDescription, OUT LPWSTR* object)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBSetTraceOptions(IN LPCWSTR options)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetTraceOptions(OUT LPWSTR* options)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBSaveTraceOptions(void)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetTrace(OUT LPWSTR* trace)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBRead(IN DWORD count, IN LPCWSTR* pItemIDs, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBWriteVQT(IN DWORD count, IN LPCWSTR* pItemIDs, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBGetProperties(IN DWORD itemCount, IN LPWSTR* pItemIDs, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT OPCITEMPROPERTIES** ppItemProperties)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBBrowse(IN LPWSTR itemID, IN OUT LPWSTR* pContinuationPoint, IN DWORD maxElementsReturned, IN OPCBROWSEFILTER browseFilter, IN LPWSTR elementNameFilter, IN LPWSTR vendorFilter, IN BOOL returnAllProperties, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT BOOL* pMoreElements, OUT DWORD* pCount, OUT OPCBROWSEELEMENT** ppBrowseElements)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBIsAvailablePriv(OUT BOOL* pbAvailable)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBLogon(IN LPCWSTR szUserID, IN LPCWSTR szPassword)
{
	return E_NOTIMPL;
}
HRESULT SODaSServer::onBLogoff(void)
{
	return E_NOTIMPL;
}
void SODaSServer::onESetLocaleID(IN HRESULT res, IN LCID dwLcid) {}
void SODaSServer::onEGetLocaleID(HRESULT res, IN LCID* pdwLcid)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  lcid:0x%X"), *pdwLcid));
	}
}
void SODaSServer::onEQueryAvailableLocaleIDs(IN HRESULT res, IN DWORD* pdwCount, IN LCID** pdwLcid)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  count:0x%X"), *pdwCount));
	}
}
void SODaSServer::onEGetErrorString(IN HRESULT res, IN HRESULT dwError, IN LPWSTR* ppString)
{
	if (SUCCEEDED(res))
	{
		_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorString:%s"), W2CT(*ppString)));
	}
}
void SODaSServer::onESetClientName(IN HRESULT res, IN LPCWSTR szName) {}
void SODaSServer::onEAdvise(IN HRESULT res, IN LPUNKNOWN unknown, IN DWORD* cookie)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), *cookie));
	}
}
void SODaSServer::onEUnadvise(IN HRESULT res, IN DWORD cookie) {}
void SODaSServer::onEAddGroup(IN HRESULT res, IN LPCWSTR szName, IN BOOL bActive, IN DWORD dwRequestedUpdateRate, IN OPCHANDLE hClientGroup, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD dwLCID, OUT OPCHANDLE* phServerGroup, IN DWORD* pRevisedUpdateRate, IN REFIID riid, IN LPUNKNOWN* ppUnk)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  hSrv:0x%X"), *phServerGroup))
}
void SODaSServer::onEGetErrorString(IN HRESULT res, IN HRESULT dwError, IN LCID dwLocale, IN LPWSTR* ppString)
{
	if (SUCCEEDED(res))
	{
		_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  errorString:%s"), W2CT(*ppString)));
	}
}
void SODaSServer::onEGetGroupByName(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk) {}
void SODaSServer::onEGetStatus(IN HRESULT res, IN OPCSERVERSTATUS** ppServerStatus) {}
void SODaSServer::onERemoveGroup(IN HRESULT res, IN OPCHANDLE hServerGroup, IN BOOL bForce) {}
void SODaSServer::onECreateGroupEnumerator(IN HRESULT res, IN OPCENUMSCOPE dwScope, IN REFIID riid, IN LPUNKNOWN* ppUnk) {}
void SODaSServer::onEGetPublicGroupByName(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk) {}
void SODaSServer::onERemovePublicGroup(IN HRESULT res, IN OPCHANDLE hServerGroup, IN BOOL bForce) {}
void SODaSServer::onEQueryAvailableProperties(IN HRESULT res, IN LPWSTR szItemID, IN DWORD* pdwCount, IN DWORD** ppPropertyIDs, IN LPWSTR** ppDescriptions, IN VARTYPE** ppvtDataTypes)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), *pdwCount));
	}
}
void SODaSServer::onEGetItemProperties(IN HRESULT res, IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, IN VARIANT** ppvData, IN HRESULT** ppErrors) {}
void SODaSServer::onELookupItemIDs(IN HRESULT res, IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, IN LPWSTR** ppszNewItemIDs, IN HRESULT** ppErrors) {}
void SODaSServer::onEQueryOrganization(IN HRESULT res, IN OPCNAMESPACETYPE* pNameSpaceType)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  org:%lu"), *pNameSpaceType));
	}
}
void SODaSServer::onEChangeBrowsePosition(IN HRESULT res, IN OPCBROWSEDIRECTION dwBrowseDirection, IN LPCWSTR szString) {}
void SODaSServer::onEBrowseOPCItemIDs(IN HRESULT res, IN OPCBROWSETYPE dwBrowseFilterType, IN LPCWSTR szFilterCriteria, IN VARTYPE vtDataTypeFilter, IN DWORD dwAccessRightsFilter, IN LPENUMSTRING* ppIEnumString) {}
void SODaSServer::onEGetItemID(IN HRESULT res, IN LPWSTR szItemDataID, IN LPWSTR* szItemID)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  qualItemID:%s"), *szItemID));
	}
}
void SODaSServer::onEBrowseAccessPaths(IN HRESULT res, IN LPCWSTR szItemID, IN LPENUMSTRING* ppIEnumString) {}
void SODaSServer::onEGetClassID(IN HRESULT res, IN CLSID* pClassID) {}
void SODaSServer::onEIsDirty(IN HRESULT res) {}
void SODaSServer::onELoad(IN HRESULT res, IN LPCOLESTR pszFileName, IN DWORD dwMode) {}
void SODaSServer::onESave(IN HRESULT res, IN LPCOLESTR pszFileName, IN BOOL fRemember) {}
void SODaSServer::onESaveCompleted(IN HRESULT res, IN LPCOLESTR pszFileName) {}
void SODaSServer::onEGetCurFile(IN HRESULT res, IN LPOLESTR* ppszFileName)
{
	if (SUCCEEDED(res))
	{
		_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  curFileName:%s"), W2CT(*ppszFileName)));
	}
}
void SODaSServer::onEGetChildren(IN HRESULT res, IN LPCWSTR parent, IN BOOL withAttributes, IN LPWSTR* children) {}
void SODaSServer::onECollectChanges(IN HRESULT res, IN LPCWSTR objects, IN DWORD doCollect) {}
void SODaSServer::onEGetChanges(IN HRESULT res, IN LPWSTR* changes) {}
void SODaSServer::onEGetStatistics(IN HRESULT res, IN LPWSTR* statistics) {}
void SODaSServer::onEGetObject(IN HRESULT res, IN LPCWSTR objectDescription, OUT LPWSTR* object) {}
void SODaSServer::onESetTraceOptions(IN HRESULT res, IN LPCWSTR options) {}
void SODaSServer::onEGetTraceOptions(IN HRESULT res, OUT LPWSTR* options) {}
void SODaSServer::onESaveTraceOptions(IN HRESULT res) {}
void SODaSServer::onEGetTrace(IN HRESULT res, OUT LPWSTR* trace) {}
void SODaSServer::onERead(IN HRESULT res, IN DWORD count, IN LPCWSTR* pItemIDs, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors) {}
void SODaSServer::onEWriteVQT(IN HRESULT res, IN DWORD count, IN LPCWSTR* pItemIDs, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors) {}
void SODaSServer::onEGetProperties(IN HRESULT res, IN DWORD itemCount, IN LPWSTR* pItemIDs, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT OPCITEMPROPERTIES** ppItemProperties) {}
void SODaSServer::onEBrowse(IN HRESULT res, IN LPWSTR itemID, IN OUT LPWSTR* pContinuationPoint, IN DWORD maxElementsReturned, IN OPCBROWSEFILTER browseFilter, IN LPWSTR elementNameFilter, IN LPWSTR vendorFilter, IN BOOL returnAllProperties, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT BOOL* pMoreElements, OUT DWORD* pCount, OUT OPCBROWSEELEMENT** ppBrowseElements) {}
void SODaSServer::onEIsAvailablePriv(IN HRESULT res, OUT BOOL* pbAvailable) {}
void SODaSServer::onELogon(IN HRESULT res, IN LPCWSTR szUserID, IN LPCWSTR szPassword) {}
void SODaSServer::onELogoff(IN HRESULT res) {}
#endif // SOFEATURE_COMIF_HOOKS
#endif // SOFEATURE_DCOM



//-----------------------------------------------------------------------------
// SODaSObjectRoot                                                            |
//-----------------------------------------------------------------------------

SODaSObjectRoot::SODaSObjectRoot(void)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("DaS Object Root"));
	setObjTraceId(id);
}


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a server object
// - adds branch to branch list, if branch is the first public groups object
//
// returns:
//      TRUE  - added
//      FALSE - not added
//
BOOL SODaSObjectRoot::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_SERVER_DA))
	{
		// branch is a server object -> add
		if (!((SODaSServer*)newBranch)->isPublicGroups())
		{
			return SOCmnElement::addBranch(newBranch);
		}
		else
		{
			// branch is a public groups object -> check if public groups already exists
			SOCmnListPosition pos;
			SODaSServer* branch;
			SOCmnList<SOCmnElement> list(getBranchList());
			pos = list.getStartPosition();

			while (pos)
			{
				branch = (SODaSServer*)list.getNext(pos);

				if (branch->isPublicGroups())
				{
					// there one public groups object
					return FALSE;
				}
			}

			return SOCmnElement::addBranch(newBranch);  // add to branch list
		}
	}
	else
	{
		return FALSE;
	}
} // addBranch


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSObjectRoot::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(512);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_OBJECTROOT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnDateTime startTime;
		SOCmnString buffer;
		WORD majorVersion, minorVersion, patchVersion, buildNumber;
		entry->getVersionInfo(&majorVersion, &minorVersion, &patchVersion, &buildNumber);
		entry->getStartTime(&startTime);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VERSION_T, buffer.format(_T("%u.%2.2u.%u.%4.4u"), majorVersion, minorVersion, patchVersion, buildNumber));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VENDORINFO_T, entry->getVendorInfo());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_STARTTIME_T, conv->datetime2string(buffer, startTime));
#ifdef SOFEATURE_DCOM
		addDCOMCnfgWatchData(entry, szXML);
#endif
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SODaSBrowseElementData                                                     |
//-----------------------------------------------------------------------------

SODaSBrowseElementData::SODaSBrowseElementData(void)
{
	m_flags = 0;
}

#endif // SODAS
