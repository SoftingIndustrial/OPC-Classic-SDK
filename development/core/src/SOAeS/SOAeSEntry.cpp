#include "stdafx.h"

#ifdef SOAES

#include "SOVersion.h"
#include "SOAeSEntry.h"
#include "SOAeSServer.h"
#ifdef SOFEATURE_DCOM
#include "SOAeSComServer.h"
#endif
#include "SOAeSCategory.h"
#include <comcat.h>
#ifdef SOFEATURE_TUNNEL
#include "SOAeSTPServer.h"
#include "SOAeSTPSubscription.h"
#include "SOAeSTPAreaBrowser.h"
#endif


//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

#ifndef SOFEATURE_COMBINED_SERVER
CComModule _Module;
#endif

static SOAeSEntry* g_aesEntry = NULL;
static SOCmnSync g_aesEntrySync;

// ATL global variables
CLSID g_clsidAE0;
CLSID g_clsidAE1;
CLSID g_clsidAE2;
CLSID g_clsidAE3;
CLSID g_clsidAE4;
CLSID g_clsidAE5;
CLSID g_clsidAE6;
CLSID g_clsidAE7;
CLSID g_clsidAE8;
CLSID g_clsidAE9;

static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

BEGIN_OBJECT_MAP(g_objectMap)
OBJECT_ENTRY(g_clsidAE0, SOAeSComServer0)
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL},
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL},
#endif
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL},
#endif
END_OBJECT_MAP()

#ifdef SOFLAG_DEMO_VERSION
#ifndef SOFEATURE_COMBINED_SERVER
#ifndef SOFLAG_NO_DLLS
DWORD g_demoStamp = GetTickCount();
#endif
#endif
#endif


//-----------------------------------------------------------------------------
// SOAeSCreator                                                               |
//-----------------------------------------------------------------------------

SOAeSServer* SOAeSCreator::createServer(IN BYTE serverInstance)
{
	SOAeSServer* server;
	SOCMN_ALLOCATE_OBJECT(server, SOAeSServer(serverInstance)) return server;
}

SOAeSSubscription* SOAeSCreator::createSubscription(IN SOAeSServer* /* server */)
{
	SOAeSSubscription* sus;
	SOCMN_ALLOCATE_OBJECT(sus, SOAeSSubscriptionDefault()) return sus;
}

SOAeSAreaBrowser* SOAeSCreator::createAreaBrowser(IN SOAeSServer* /* server */)
{
	SOAeSAreaBrowser* ab;
	SOCMN_ALLOCATE_OBJECT(ab, SOAeSAreaBrowser()) return ab;
}

SOAeSEvent* SOAeSCreator::createConditionEvent(IN SOAeSCondition* /* cond */)
{
	SOAeSEvent* event;
	SOCMN_ALLOCATE_OBJECT(event, SOAeSEventConditionDefault()) return event;
}

SOAeSEvent* SOAeSCreator::createSimpleEvent(void)
{
	SOAeSEvent* event;
	SOCMN_ALLOCATE_OBJECT(event, SOAeSEventSimpleDefault()) return event;
}

SOAeSEvent* SOAeSCreator::createTrackingEvent(void)
{
	SOAeSEvent* event;
	SOCMN_ALLOCATE_OBJECT(event, SOAeSEventTrackingDefault()) return event;
}

SOAeSAttributeValue* SOAeSCreator::createAttributeValue(void)
{
	SOAeSAttributeValue* av;
	SOCMN_ALLOCATE_OBJECT(av, SOAeSAttributeValue()) return av;
}

SOAeSAttributeValue* SOAeSCreator::createAttributeValue(IN SOAeSCondition* /* cond */)
{
	SOAeSAttributeValue* av;
	SOCMN_ALLOCATE_OBJECT(av, SOAeSAttributeValue()) return av;
}

SOAeSAreaSpaceRoot* SOAeSCreator::createAreaSpaceRoot(IN BYTE /* serverInstance */)
{
	SOAeSAreaSpaceRoot* root;
	SOCMN_ALLOCATE_OBJECT(root, SOAeSAreaSpaceRoot()) return root;
}

SOAeSEventSpaceRoot* SOAeSCreator::createEventSpaceRoot(void)
{
	SOAeSEventSpaceRoot* root;
	SOCMN_ALLOCATE_OBJECT(root, SOAeSEventSpaceRoot()) return root;
}

SOAeSObjectRoot* SOAeSCreator::createObjectRoot(void)
{
	SOAeSObjectRoot* root;
	SOCMN_ALLOCATE_OBJECT(root, SOAeSObjectRoot()) return root;
}




//-----------------------------------------------------------------------------
// SOAeSEntry                                                                 |
//-----------------------------------------------------------------------------

SOAeSEntry* getSOAeSEntry(void)
{
	SOCmnSingleLock<SOCmnSync> lock(&g_aesEntrySync);

	if (g_aesEntry)
	{
		g_aesEntry->addRef();
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(g_aesEntry, SOAeSEntry())
	}

	return g_aesEntry;
}



SOAeSEntry::SOAeSEntry(void)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("AeS Entry"));
	setObjTraceId(id);
	// server information
	m_vendorInfo = _T("Softing OPC Alarms & Events Server Development Kit");
	m_majorVersion = MAJOR_VERSION_NUMBER;
	m_minorVersion = MINOR_VERSION_NUMBER;
	m_patchVersion = PATCH_VERSION_NUMBER;
	m_buildNumber = BUILD_NUMBER;
	m_serverState = OPCAE_STATUS_NOCONFIG;
	m_serverStateManuallySet = FALSE;
	memset(&m_statistics, 0, sizeof(SOAeSEntryStatisticsData));
	// AE object
	SOCMN_ALLOCATE_OBJECT(m_objectRoot, SOAeSObjectRoot())
	SOCMN_ALLOCATE_OBJECT(m_creator, SOAeSCreator())
	// event space
	m_eventSpaceRoot = NULL;
	// condition group list
	m_condGroupList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	// condition survey handles
	m_condSurveyThread = INVALID_HANDLE_VALUE;
	m_condSurveyEvents[SOCMN_EVENT_END] = INVALID_HANDLE_VALUE;
	m_condSurveyEvents[SOCMN_EVENT_WORK] = INVALID_HANDLE_VALUE;
	m_checkConditions = FALSE;
	// register categories
	setRegisterCategories(SOSRVENTRY_CATEGORY_AE10);
	::setSOSrvEntry(this);  // set entry in SOSrv DLL
}


SOAeSEntry::~SOAeSEntry(void)
{
	interlockedIncrement(&m_refCount);

	// destroy area space
	for (BYTE i = 0; i < SOAES_SERVER_INSTANCE_COUNT; i++)
	{
		if (m_areaSpaceRoot[i].isNotNull())
		{
			m_areaSpaceRoot[i]->removeAllAdditionalElements();
			m_areaSpaceRoot[i]->removeAllLeafs();
			m_areaSpaceRoot[i]->removeAllBranches();
		}
	}

	// destroy event space
	if (m_eventSpaceRoot.isNotNull())
	{
		m_eventSpaceRoot->removeAllAdditionalElements();
		m_eventSpaceRoot->removeAllLeafs();
		m_eventSpaceRoot->removeAllBranches();
	}

	// free events
	if (m_condSurveyEvents[SOCMN_EVENT_END] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_condSurveyEvents[SOCMN_EVENT_END]);
	}

	if (m_condSurveyEvents[SOCMN_EVENT_WORK] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_condSurveyEvents[SOCMN_EVENT_WORK]);
	}

	m_creator->release();
	g_aesEntry = NULL;
	::setSOSrvEntry(NULL);
	InterlockedDecrement(&m_refCount);
}


//-----------------------------------------------------------------------------
// surveyThreadMain
//
// - condition survey thread of the entry
//
// returns:
//		thread exit code
//
DWORD WINAPI surveyThreadMain(
	IN LPVOID context)  // application object
{
	SOAeSEntry* ent = (SOAeSEntry*)context;
	return ent->surveyThread();
} // surveyThreadMain


//-----------------------------------------------------------------------------
// surveyThread
//
// - condition survey thread of the entry
//
// returns:
//		thread exit code
//
DWORD SOAeSEntry::surveyThread(void)
{
	BOOL end = FALSE;
	DWORD ret;
	DWORD waitTime = 0;
	DWORD waitTimeSI;
	DWORD loopStart;
	DWORD loopTime = 0;
	BYTE i;
	SOCMN_TRY
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("started condition survey thread")));

		while (!end)
		{
			ret = ::WaitForMultipleObjects(2, m_condSurveyEvents, FALSE, waitTime);

			switch (ret - WAIT_OBJECT_0)
			{
			case SOCMN_EVENT_END:
			{
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("terminate condition survey thread")));
				end = TRUE;
			}
			break;

			case SOCMN_EVENT_WORK:

				// fall through default
			default:
			{
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("survey loop >>>")));
				loopStart = ::GetTickCount();
				waitTime = INFINITE;

				// update attributes and check conditions
				for (i = 0; i < SOAES_SERVER_INSTANCE_COUNT; i++)
				{
					if (m_areaSpaceRoot[i].isNotNull())
					{
						waitTimeSI = updateConditions(m_areaSpaceRoot[i], loopStart, INFINITE);

						if (waitTimeSI < waitTime)
						{
							waitTime = waitTimeSI;
						}
					}
				}

				// max wait time is 40 days
				if (SOCMN_TIME_40DAYS < waitTime)
				{
					waitTime = SOCMN_TIME_40DAYS;
				}
				else
				{
					loopTime = getTimeSpan(loopStart, ::GetTickCount());

					if ((loopTime != 0) && (waitTime >= loopTime))
					{
						waitTime -= loopTime;
					}
				}

				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("survey loop next:%lu needed:%lu <<<"), waitTime, loopTime));
			}
			break;
			}
		}
	}
	SOCMN_CATCH(SOCmnException e)
	{
#ifndef SOOS_WINDOWS_CE
		DWORD tid;
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("EXCEPTION in condition survey Thread %s"), (LPCTSTR)e.m_description));
		_ASSERTION(FALSE, "EXCEPTION in condition survey Thread!");
		// restart work thread
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("restart condition survey thread")));
		m_condSurveyThread = ::CreateThread(NULL, 0, surveyThreadMain, (void*)this, 0, &tid);

		if (m_condSurveyThread != INVALID_HANDLE_VALUE)
		{
			::SetThreadPriority(m_condSurveyThread, THREAD_PRIORITY_HIGHEST);
		}

		return 1;
#endif
	}
	SOCMN_CATCH(...)
	{
#ifndef SOOS_WINDOWS_CE
		DWORD tid;
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("EXCEPTION in condition survey Thread")));
		_ASSERTION(FALSE, "EXCEPTION in condition survey Thread!");
		// restart work thread
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("restart condition survey thread")));
		m_condSurveyThread = ::CreateThread(NULL, 0, surveyThreadMain, (void*)this, 0, &tid);

		if (m_condSurveyThread != INVALID_HANDLE_VALUE)
		{
			::SetThreadPriority(m_condSurveyThread, THREAD_PRIORITY_HIGHEST);
		}

		return 1;
#endif
	}
	return 0;
} // surveyThread


//-----------------------------------------------------------------------------
// updateConditions
//
// - update the condition
//
// returns:
//		time in milliseconds until next call is needed
//
DWORD SOAeSEntry::updateConditions(
	IN SOCmnElement* /* parent */,   // parent element
	IN DWORD now,                    // start time of loop
	IN DWORD minSpan)                // minimal time till next call
{
	SOCmnList<SOAeSConditionGroup> condGroupList(getConditionGroupList());
	SOAeSConditionGroup* condGroup;
	SOCmnListPosition posG;
	DWORD actSpan;
	posG = condGroupList.getStartPosition();

	while (posG)
	{
		condGroup = condGroupList.getNext(posG);
		actSpan = INFINITE;

		// check if to start the cyclic transaction
		if (condGroup->updateSpan(now, &actSpan))
		{
			condGroup->checkConditions();
		}

		if (actSpan < minSpan)
		{
			minSpan = actSpan;
		}
	}

	return minSpan;
} // updateConditions


//-----------------------------------------------------------------------------
// getConditionGroup
//
// - get condition group with the specified update rate
//
// returns:
//		pointer to the condition group
//
SOAeSConditionGroup* SOAeSEntry::getConditionGroup(
	IN DWORD updateRate)    // update rate
{
	SOCmnList<SOAeSConditionGroup> condGroupList(getConditionGroupList());
	SOAeSConditionGroup* condGroup;
	SOCmnListPosition posG;
	DWORD grpUR;

	if (updateRate < 10)
	{
		updateRate = 10;
	}

	posG = condGroupList.getStartPosition();

	while (posG)
	{
		condGroup = condGroupList.getNext(posG);
		grpUR = condGroup->getUpdateRate();

		if ((grpUR == updateRate) ||
			((grpUR + 5 > updateRate) && (grpUR - 4 < updateRate)))
		{
			condGroup->addRef();
			return condGroup;
		}
	}

	// no group found -> create it
	SOCMN_ALLOCATE_OBJECT(condGroup, SOAeSConditionGroup(updateRate))
	condGroupList.add(condGroup);
	return condGroup;
} // getConditionGroup


void SOAeSEntry::connectAttributeValueUpdate(void)
{
	SOCmnList<SOAeSConditionGroup> condGroupList(getConditionGroupList());
	SOAeSConditionGroup* condGroup;
	SOCmnListPosition posG;
	posG = condGroupList.getStartPosition();

	while (posG)
	{
		condGroup = condGroupList.getNext(posG);
		condGroup->connectAttributeValueUpdate();
	}
}


//-----------------------------------------------------------------------------
// start
//
// - start main thread and the condition survey thread of the entry
//
// returns:
//		TRUE  - thread started
//		FALSE - error creating the thread
//
BOOL SOAeSEntry::start(void)
{
	BOOL ret;
	ret = SOSrvEntry::start();
	startSurvey();
	return ret;
} // start


//-----------------------------------------------------------------------------
// startSurvey
//
// - start the condition survey thread of the entry
//
void SOAeSEntry::startSurvey(void)
{
	// create survey thread
	if (m_condSurveyThread == INVALID_HANDLE_VALUE)
	{
		if (m_checkConditions)
		{
			DWORD tid;

			if (m_condSurveyEvents[SOCMN_EVENT_END] == INVALID_HANDLE_VALUE)
			{
				m_condSurveyEvents[SOCMN_EVENT_END] = getEventPool()->popEvent();
			}

			if (m_condSurveyEvents[SOCMN_EVENT_WORK] == INVALID_HANDLE_VALUE)
			{
				m_condSurveyEvents[SOCMN_EVENT_WORK] = getEventPool()->popEvent();
			}

			m_condSurveyThread = ::CreateThread(NULL, 0,
												(LPTHREAD_START_ROUTINE) &surveyThreadMain,
												this, 0, &tid);

			if (m_condSurveyThread != INVALID_HANDLE_VALUE)
			{
				::SetThreadPriority(m_condSurveyThread, THREAD_PRIORITY_HIGHEST);
			}
		}
	}
} // startSurvey


//-----------------------------------------------------------------------------
// stop
//
// - stop the main thread and the condition survey thread of the entry
//
// returns:
//		TRUE  - thread stoped
//		FALSE - can't stop thread
//
BOOL SOAeSEntry::stop(void)
{
	stopSurvey();
	return SOSrvEntry::stop();
} // stop


//-----------------------------------------------------------------------------
// stopSurvey
//
// - stop the condition survey thread of the entry
//
void SOAeSEntry::stopSurvey(void)
{
	if (m_condSurveyThread != INVALID_HANDLE_VALUE)
	{
		::SetEvent(m_condSurveyEvents[SOCMN_EVENT_END]);

		if (WAIT_FAILED == ::WaitForSingleObject(m_condSurveyThread, 10000))
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ENTRY, (_T("Can't terminate condition survey thread")));
		}

		::CloseHandle(m_condSurveyThread);
		m_condSurveyThread = INVALID_HANDLE_VALUE;

		if (m_condSurveyEvents[SOCMN_EVENT_END] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_condSurveyEvents[SOCMN_EVENT_END]);
		}

		if (m_condSurveyEvents[SOCMN_EVENT_WORK] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_condSurveyEvents[SOCMN_EVENT_WORK]);
		}

		m_condSurveyEvents[SOCMN_EVENT_END] = INVALID_HANDLE_VALUE;
		m_condSurveyEvents[SOCMN_EVENT_WORK] = INVALID_HANDLE_VALUE;
	}
} // stopSurvey


//-----------------------------------------------------------------------------
// getEventSpaceRoot
//
// - get eventspace root object
//
// returns:
//		eventspace root
//
SOAeSEventSpaceRoot* SOAeSEntry::getEventSpaceRoot(void)
{
	if (!m_eventSpaceRoot)
	{
		m_eventSpaceRoot = m_creator->createEventSpaceRoot();
	}

	m_eventSpaceRoot.addRef();
	return (SOAeSEventSpaceRoot*)m_eventSpaceRoot;
} // getEventSpaceRoot


//-----------------------------------------------------------------------------
// createNewEventSpaceRoot
//
// - release the old event space root object -> a new one is created via the next access
//
void SOAeSEntry::createNewEventSpaceRoot(void)
{
	if (m_eventSpaceRoot.isNotNull())
	{
		m_eventSpaceRoot.release();
	}
} // createNewEventSpaceRoot


//-----------------------------------------------------------------------------
// getAreaSpaceRoot
//
// - get areaspace root object
//
// returns:
//		areaspace root
//
SOAeSAreaSpaceRoot* SOAeSEntry::getAreaSpaceRoot(
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SOAES_SERVER_INSTANCE_COUNT)
	{
		return NULL;
	}

	if (!m_areaSpaceRoot[serverInstance])
	{
		m_areaSpaceRoot[serverInstance] = m_creator->createAreaSpaceRoot(serverInstance);
	}

	m_areaSpaceRoot[serverInstance].addRef();
	return m_areaSpaceRoot[serverInstance];
} // getAreaSpaceRoot


//-----------------------------------------------------------------------------
// createNewAreaSpaceRoot
//
// - release the old area space root object -> a new one is created via the next access
//
void SOAeSEntry::createNewAreaSpaceRoot(
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SOAES_SERVER_INSTANCE_COUNT)
	{
		return;
	}

	if (m_areaSpaceRoot[serverInstance].isNotNull())
	{
		m_areaSpaceRoot[serverInstance].release();
	}

	// reset the browse position of all servers to the new namespace root
	SOCmnList<SOAeSServer> serverList((SOCmnList<SOAeSServer> *)m_objectRoot->getBranchList());
	SOCmnListPosition posSrv;
	SOAeSServer* server;
	SOCmnList<SOAeSAreaBrowser> browserList;
	SOCmnListPosition posBrw;
	SOAeSAreaBrowser* browser;
	posSrv = serverList.getStartPosition();

	while (posSrv)
	{
		server = serverList.getNext(posSrv);
		browserList = (SOCmnList<SOAeSAreaBrowser> *)server->getAdditionalList();
		posBrw = browserList.getStartPosition();

		while (posBrw)
		{
			browser = browserList.getNext(posBrw);
			browser->changeBrowsePosition(SOSrvBrowser::dirTo, _T(""));
		}

		browserList.autoUnlock();
	}

	serverList.autoUnlock();
} // createNewAreaSpaceRoot


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// addWatchObject
//
// - create and add watch object to watch list
//
// returns:
//		pointer to watch object
//
SOSrvWatch* SOAeSEntry::addWatchObject(void)
{
	SOSrvWatch* watch = SOSrvEntry::addWatchObject();

	if (watch)
	{
		SOCmnPointer<SOAeSWatchHandler> wh = new SOAeSWatchHandler();
		watch->setAEHandler(wh);
	}

	return watch;
} // addWatchObject


//-----------------------------------------------------------------------------
// incObjectCountStatistics
//
// - increment one object statistics count
//
void SOAeSEntry::incObjectCountStatistics(
	IN DWORD objType)  // object type
{
	switch (objType)
	{
	case SOCMNOBJECT_TYPE_SERVER_AE:
		m_statistics.serverObjs++;
		break;

	case SOCMNOBJECT_TYPE_SUBSCRIPTION:
		m_statistics.subscriptionObjs++;
		break;

	case SOCMNOBJECT_TYPE_AREA_BROWSER:
		m_statistics.areaBrowserObjs++;
		break;

	case SOCMNOBJECT_TYPE_AREA:
		m_statistics.areaObjs++;
		break;

	case SOCMNOBJECT_TYPE_SOURCE:
		m_statistics.sourceObjs++;
		break;

	case SOCMNOBJECT_TYPE_CONDITION:
		m_statistics.conditionObjs++;
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE:
		m_statistics.attributeValueObjs++;
		break;

	case SOCMNOBJECT_TYPE_CATEGORY:
		m_statistics.categoryObjs++;
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE:
		m_statistics.attributeObjs++;
		break;

	case SOCMNOBJECT_TYPE_CONDITION_CLASS:
		m_statistics.conditionClassObjs++;
		break;

	case SOCMNOBJECT_TYPE_SUBCONDITION_CLASS:
		m_statistics.subConditionClassObjs++;
		break;
	}
} // incObjectCountStatistics


//-----------------------------------------------------------------------------
// decObjectCountStatistics
//
// - decrement one object statistics count
//
void SOAeSEntry::decObjectCountStatistics(
	IN DWORD objType) // object type
{
	switch (objType)
	{
	case SOCMNOBJECT_TYPE_SERVER_AE:
		m_statistics.serverObjs--;
		break;

	case SOCMNOBJECT_TYPE_SUBSCRIPTION:
		m_statistics.subscriptionObjs--;
		break;

	case SOCMNOBJECT_TYPE_AREA_BROWSER:
		m_statistics.areaBrowserObjs--;
		break;

	case SOCMNOBJECT_TYPE_AREA:
		m_statistics.areaObjs--;
		break;

	case SOCMNOBJECT_TYPE_SOURCE:
		m_statistics.sourceObjs--;
		break;

	case SOCMNOBJECT_TYPE_CONDITION:
		m_statistics.conditionObjs--;
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE:
		m_statistics.attributeValueObjs--;
		break;

	case SOCMNOBJECT_TYPE_CATEGORY:
		m_statistics.categoryObjs--;
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE:
		m_statistics.attributeObjs--;
		break;

	case SOCMNOBJECT_TYPE_CONDITION_CLASS:
		m_statistics.conditionClassObjs--;
		break;

	case SOCMNOBJECT_TYPE_SUBCONDITION_CLASS:
		m_statistics.subConditionClassObjs--;
		break;
	}
} // decObjectCountStatistics


void SOAeSEntry::changeConditionStateStatistics(IN BYTE prevState, IN BYTE newState)
{
	if (prevState & OPC_CONDITION_ACTIVE)
	{
		// active
		if (prevState & OPC_CONDITION_ACKED)
		{
			m_statistics.actAckCond--;    // acknowledged
		}
		else
		{
			m_statistics.actNAckCond--;    // not acknowledged
		}
	}
	else
	{
		// not active
		if (!(prevState & OPC_CONDITION_ACKED))
		{
			m_statistics.nActNAckCond--;    // not acknowledged
		}
	}

	if (newState & OPC_CONDITION_ACTIVE)
	{
		// active
		if (newState & OPC_CONDITION_ACKED)
		{
			m_statistics.actAckCond++;    // acknowledged
		}
		else
		{
			m_statistics.actNAckCond++;    // not acknowledged
		}
	}
	else
	{
		// not active
		if (!(newState & OPC_CONDITION_ACKED))
		{
			m_statistics.nActNAckCond++;    // not acknowledged
		}
	}
}


//-----------------------------------------------------------------------------
// getObjectCountStatistics
//
// - get the statistics
//
void SOAeSEntry::getObjectCountStatistics(
	IN SOCmnStringEx& szXML)
{
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRSERVEROBJ, m_statistics.serverObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRSUBSCRIPTIONOBJ, m_statistics.subscriptionObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRAREABROWSEROBJ, m_statistics.areaBrowserObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRAREAOBJ, m_statistics.areaObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRSOURCEOBJ, m_statistics.sourceObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRCONDITIONOBJ, m_statistics.conditionObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRATTRIBUTEVALUEOBJ, m_statistics.attributeValueObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRCATEGORYOBJ, m_statistics.categoryObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRATTRIBUTEOBJ, m_statistics.attributeObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRCONDITIONCLASSOBJ, m_statistics.conditionClassObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRSUBCONDITIONCLASSOBJ, m_statistics.subConditionClassObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRACTNACKCOND, m_statistics.actNAckCond);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRACTACKCOND, m_statistics.actAckCond);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_AE_NRNACTNACKCOND, m_statistics.nActNAckCond);
} // getObjectCountStatistics

#endif // SOFEATURE_WATCH


//-----------------------------------------------------------------------------
// init
//
// - initialize ATL module
//
void SOAeSEntry::init(
	IN HINSTANCE instanceHandle)    // instance handle of the application
{
	g_clsidAE0 = m_clsid;
	_Module.Init(g_objectMap, instanceHandle);
} // init


//-----------------------------------------------------------------------------
// term
//
// - terminate ATL module
//
void SOAeSEntry::term(void)
{
	_Module.Term();
} // term


#ifdef SOFEATURE_TUNNEL

BOOL SOAeSEntry::addTPAEReceiver(void)
{
	BOOL ret;
	SOCmnPointer<SOAeSTPServer> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SOAeSTPServer(), (SOAeSTPServer*))
	ret = m_srvTP->addTPReceiver(tpRec);
	SOCmnPointer<SOAeSTPSubscription> tpRecS;
	SOCMN_ALLOCATE_OBJECT_C(tpRecS, SOAeSTPSubscription(), (SOAeSTPSubscription*))
	ret &= m_srvTP->addTPReceiver(tpRecS);
	SOCmnPointer<SOAeSTPAreaBrowser> tpRecA;
	SOCMN_ALLOCATE_OBJECT_C(tpRecA, SOAeSTPAreaBrowser(), (SOAeSTPAreaBrowser*))
	ret &= m_srvTP->addTPReceiver(tpRecA);
	return ret;
}

#endif

//-----------------------------------------------------------------------------
// updateRegistryWithRGS
//
// - update registry with ATL registar script
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::updateRegistryWithRGS(
	IN BOOL bRegister,          // register or unregister
	IN void* replacementArray)  // replacement array for ATL registar
{
#ifndef SOOS_WINDOWS_CE
#if (_MSC_VER > 1200)
	HINSTANCE hInstSave = _AtlBaseModule.m_hInst;
	_AtlBaseModule.m_hInst = m_appInstanceHandle;
#endif
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, bRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#if (_MSC_VER > 1200)
	_AtlBaseModule.m_hInst = hInstSave;
#endif
#else
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, bRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#endif
	return res;
} // updateRegistryWithRGS


//-----------------------------------------------------------------------------
// regServer
//
// - register ATL module
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::regServer(void)
{
	return _Module.RegisterServer(FALSE);
} // regServer


//-----------------------------------------------------------------------------
// unregServer
//
// - unregister ATL module
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::unregServer(void)
{
	return _Module.UnregisterServer();
} // unregServer


//-----------------------------------------------------------------------------
// getClassObject
//
// - get class factory form ATL module
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::getClassObject(
	IN REFCLSID rclsid,     // class id
	IN REFIID riid,         // interface id
	OUT LPVOID* ppvObj)     // class factory object
{
	return _Module.GetClassObject(rclsid, riid, ppvObj);
} // getClassObject


//-----------------------------------------------------------------------------
// canUnloadNow
//
// - check if the server has any object created
//
// returns:
//		S_OK    - dll can be unloaded
//		S_FALSE - dll can't be unloaded
//
HRESULT SOAeSEntry::canUnloadNow(void)
{
	LONG lockCount = _Module.GetLockCount();
	return (lockCount == 0) ? S_OK : S_FALSE;
} // canUnloadNow


//-----------------------------------------------------------------------------
// registerClassObjects
//
// - register class objects for OutProc servers
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::registerClassObjects(void)
{
	return _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
} // registerClassObjects


//-----------------------------------------------------------------------------
// revokeClassObjects
//
// - revoke class objects for OutProc servers
//
// returns:
//		HRESULTs of ATL method
//
HRESULT SOAeSEntry::revokeClassObjects(void)
{
	return _Module.RevokeClassObjects();
} // revokeClassObjects

BOOL SOAeSEntry::addComObjMapEntry(
	IN const CLSID* pClsid,
	IN void* pfnUpdateRegistry,
	IN void* pfnGetClassObject,
	IN void* pfnCreateInstance,
	IN void* pfnGetObjectDescription,
	IN void* pfnGetCategoryMap,
	IN void* pfnObjectMain)
{
	DWORD i;

	for (i = 1; i < 11; i++)
	{
		if (NULL == g_objectMap[i].pclsid)
		{
			g_objectMap[i].pclsid = pClsid;
			g_objectMap[i].pfnUpdateRegistry = (HRESULT(WINAPI*)(BOOL))pfnUpdateRegistry;
			g_objectMap[i].pfnGetClassObject = (_ATL_CREATORFUNC*)pfnGetClassObject;
			g_objectMap[i].pfnCreateInstance = (_ATL_CREATORFUNC*)pfnCreateInstance;
			g_objectMap[i].pCF = NULL;
			g_objectMap[i].dwRegister = 0;
			g_objectMap[i].pfnGetObjectDescription = (_ATL_DESCRIPTIONFUNC*)pfnGetObjectDescription;
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
			g_objectMap[i].pfnGetCategoryMap = (_ATL_CATMAPFUNC*)pfnGetCategoryMap;
#endif
			g_objectMap[i].pfnObjectMain = (void (WINAPI*)(bool))pfnObjectMain;
#endif
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SOAeSEntry::addComObjMapEntry(IN const CLSID* clsid, IN DWORD cnt)
{
	BOOL ret = FALSE;

	switch (cnt)
	{
	case 1:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer1));
		g_clsidAE1 = *clsid;
		break;

	case 2:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer2));
		g_clsidAE2 = *clsid;
		break;

	case 3:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer3));
		g_clsidAE3 = *clsid;
		break;

	case 4:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer4));
		g_clsidAE4 = *clsid;
		break;

	case 5:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer5));
		g_clsidAE5 = *clsid;
		break;

	case 6:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer6));
		g_clsidAE6 = *clsid;
		break;

	case 7:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer7));
		g_clsidAE7 = *clsid;
		break;

	case 8:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer8));
		g_clsidAE8 = *clsid;
		break;

	case 9:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer9));
		g_clsidAE9 = *clsid;
		break;
	}

	return ret;
}


//-----------------------------------------------------------------------------
// setCreator
//
// - set OPC object factory
//
void SOAeSEntry::setCreator(
	IN SOAeSCreator* creator)   // OPC object factory
{
	if (creator)
	{
		if (m_creator)
		{
			m_creator->release();
		}

		m_creator = creator;

		if (m_creator)
		{
			m_creator->addRef();
		}
	}
} // setCreator


//-----------------------------------------------------------------------------
// getSrvCreator
//
// - get toolkit object factory
//
// returns:
//		toolkit object factory
//
SOSrvCreator* SOAeSEntry::getSrvCreator(void)
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return (SOSrvCreator*)m_creator;
} // getSrvCreator

static TCHAR s_serverStateString[6][10] =
{ _T("running"), _T("failed"), _T("noConfig"), _T("suspended"), _T("test"), _T("commFault") };


LPCTSTR SOAeSEntry::getServerStateString(void) const
{
	return s_serverStateString[m_serverState - 1];
}

#endif // SOAES
