#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCServer.h"
#include "SOAeCSubscription.h"
#include "SOAeCEntry.h"
#include "SOAeCCallback.h"
#include "SOAeCEvent.h"
#include "SOAeCBrowse.h"


//-----------------------------------------------------------------------------
// SOAeCConditionState                                                        |
//-----------------------------------------------------------------------------

SOAeCConditionState::SOAeCConditionState()
{
	m_state = 0;
	m_quality = 0;
	ZeroMemory(&m_lastAckTime, sizeof(m_lastAckTime));
	ZeroMemory(&m_subCondLastActive, sizeof(m_subCondLastActive));
	ZeroMemory(&m_condLastActive, sizeof(m_condLastActive));
	ZeroMemory(&m_condLastInactive, sizeof(m_condLastInactive));
}

SOAeCConditionState::~SOAeCConditionState()
{
}




//-----------------------------------------------------------------------------
// SOAeCSubConditionState                                                     |
//-----------------------------------------------------------------------------

SOAeCSubConditionState::SOAeCSubConditionState()
{
	m_severity = 0;
}

SOAeCSubConditionState::~SOAeCSubConditionState()
{
}




//-----------------------------------------------------------------------------
// SOAeCServer                                                                |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor

SOAeCServer::SOAeCServer(
	IN OPTIONAL WORD subsListType)
	:  SOCltServer(SOCMNOBJECT_TYPE_SERVER_AE),
	   SOCmnElementListLeaf(subsListType)
{
	// create leaf map
	if (m_leafList.isInit())
	{
		m_leafList.destroy();
	}

	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 17;
	m_leafList.create(&cfg);
	m_version = SOAEC_VERSION_OPC_AE10;
}

//-----------------------------------------------------------------------------
// Destructor

SOAeCServer::~SOAeCServer()
{
}

//-----------------------------------------------------------------------------
// Serialization

BOOL SOAeCServer::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;
	SOCmnList<SOCmnElement> list(getLeafList());
	SOAeCSubscription* subs;

	if (isStoring)
	{
		if (!getPersistence())
		{
			return TRUE;
		}

		// store own data
		ar << getValidParameters();
		ar << getServerName();
		ar << getNodeName();
		ar << getClassID();
		ar << getClsctx();
		ar << getClientName();
		ar << getLCID();
		ar << getUserName();
		ar << getPassword();
		// store children
		DWORD dwCount = 0;
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			subs = (SOAeCSubscription*)list.getNext(pos);

			if (subs->getPersistence())
			{
				dwCount++;
			}
		}

		ar << dwCount;
		pos = list.getStartPosition();

		while (pos && ok)
		{
			subs = (SOAeCSubscription*)list.getNext(pos);

			if (subs->getPersistence())
			{
				ok = subs->serialize(ar, isStoring);
			}
		}
	}
	else
	{
		// load own data
		SOCmnPointer<SOAeCEntry> entry = getSOAeCEntry();
		CLSID clsid = CLSID_NULL;
		SOCmnString nodeName;
		DWORD clsctx = 0;
		SOCmnString clientName;
		SOCmnString serverName;
		LCID lcid = 0;
		ar >> m_validParameter;
		ar >> serverName;
		setServerName(serverName);
		ar >> nodeName;
		setNodeName(nodeName);
		ar >> clsid;
		setClassID(clsid);
		ar >> clsctx;
		setClsctx(clsctx);
		ar >> clientName;

		if (m_validParameter & SOCLT_SRVPAR_CLIENT_NAME)
		{
			setClientName(clientName);
		}

		ar >> lcid;

		if (m_validParameter & SOCLT_SRVPAR_LCID)
		{
			setLCID(lcid);
		}

		if (entry->getFileVersion() >= MAKELONG(01, 4))
		{
			SOCmnString userName;
			SOCmnString password;
			ar >> userName;
			setUserName(userName);
			ar >> password;
			setPassword(password);
		}

		// load childeren
		DWORD dwCount = 0;
		ar >> dwCount;

		if (dwCount > 0)
		{
			while (dwCount-- && ok)
			{
				// create subscription
				subs = addSubscription();

				if (!!subs)
				{
					ok = subs->serialize(ar, isStoring);
					subs->release();
				}
			}
		}
	}

	return ok;
} // serialize


//-----------------------------------------------------------------------------
// addLeaf
//
// - add leaf to server object
//
// returns:
//		FALSE - not added
//
BOOL SOAeCServer::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - not allowed
//
// returns:
//		TRUE  - added
//		FALSE - not added
//
BOOL SOAeCServer::addBranch(
	IN SOCmnElement* /* newBranch */)
{
	_ASSERTION(FALSE, "Function call not allowed");
	return FALSE;
} // addBranch



//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//		TRUE  - transit to new state
//		FALSE - error during state change
//
HRESULT SOAeCServer::doStateTransition(BOOL deep, IN BYTE stateDeep)
{
	HRESULT res = S_OK;
	BYTE targetState = getTargetObjectState();

	if (disconnected == targetState)
	{
		doRecurseChildren(getBranchList(), disconnected);
		doRecurseChildren(getLeafList(), disconnected);
		doRecurseChildren(getAdditionalList(), disconnected);
	}

	res = doStateTransitionImpl();

	if (deep && SUCCEEDED(res) && disconnected != targetState)
	{
		if (S_OK != doRecurseChildren(getBranchList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getLeafList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getAdditionalList(), stateDeep))
		{
			res = S_FALSE;
		}
	}

	return res;
} // doStateTransition


HRESULT SOAeCServer::doDisconnect(void)
{
	return doDisconnectImpl();
}

//-----------------------------------------------------------------------------
// addSubscription
//
// - creates an empty subscription and inserts it into the
//   subscription list of the server
//
// returns:
//		pointer to the new subscription
//		NULL - error creating the subscription
//
SOAeCSubscription* SOAeCServer::addSubscription(void)
{
	SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creator = entry->getCreator();
	SOAeCSubscription* subs = creator->createSubscription(getProtocol(), this);

	if (subs)
	{
		addLeaf((SOAeCSubscription*)subs);
	}

	return subs;
} // createSubscription


//-----------------------------------------------------------------------------
// update
//
// - update the server parameter
//
// returns:
//      S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SOAeCServer::doUpdate(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	return doUpdateImpl(fromServer, whatParameters);
} // update


void SOAeCServer::connectionFailure(void)
{
	connectionFailureImpl();
}

//-----------------------------------------------------------------------------
//
// ackConditions

BOOL SOAeCServer::ackConditions(
	IN SOCmnList<SOAeCEvent> *events,
	IN OPTIONAL LPCTSTR acknowledgerID, /* = NULL */
	IN OPTIONAL LPCTSTR comment,        /* = NULL */
	IN OPTIONAL BOOL waitopt,           /* = SOCLT_EXEC_AUTO_SYNC */
	OPTIONAL SOCmnEventPointer handle   /* = NULL */)
{
	if (events->isEmpty())
	{
		setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return FALSE;
	}

	BOOL ok = FALSE;
	SOCmnPointer<SOCltTask> task;
	task = createAckTask(events, acknowledgerID, comment);

	if (task.isNotNull())
	{
		ok = SUCCEEDED(executeTask(task, waitopt, handle));
	}

	return ok;
}

SOAeCSubscription* SOAeCServer::getSubscriptionByClientHandle(
	IN OPCHANDLE clientHandle)  // client handle
{
	SOCmnList<SOCmnElement> leafList(getLeafList());
	return (SOAeCSubscription*)leafList.findKey(clientHandle);
}

//-----------------------------------------------------------------------------
//
// getAreaBrowser


SOAeCAreaBrowser* SOAeCServer::getAreaBrowser(void)
{
	return getAreaBrowserImpl();
}

//-----------------------------------------------------------------------------
//
// getEventBrowser

SOAeCEventBrowser* SOAeCServer::getEventBrowser(void)
{
	return getEventBrowserImpl();
}

//-----------------------------------------------------------------------------
// setSubscriptionsState
//
// - set the state of all items of the group
//
void SOAeCServer::setSubscriptionsState(
	IN BYTE state)    // group target state
{
	SOCmnList<SOCmnElement> list(getLeafList());
	SOCmnListPosition pos;
	SOAeCSubscription* subs;
	pos = list.getStartPosition();

	while (pos)
	{
		subs = (SOAeCSubscription*)list.getNext(pos);
		subs->setObjectState(state);
	}
} // setSubscriptionsState


#ifdef SOFEATURE_DCOM

HRESULT SOAeCServer::queryInterface(IN REFIID riid, IN void** interf)
{
	return queryInterfaceImpl(riid, interf);
}

#endif // SOFEATURE_DCOM

SOAeCServer* SOAeCServer::getObjectAeCServer(void)
{
	return this;
}




SOAeCServerStatus::SOAeCServerStatus(void)
{
	m_state = 0;
	m_supportedLCIDs.create();
}

SOAeCServerStatus::~SOAeCServerStatus(void)
{
	m_supportedLCIDs.destroy();
}

void SOAeCServerStatus::clear(void)
{
	m_state = 0;
	m_startTime.clear();
	m_currentTime.clear();
	m_lastUpdateTime.clear();
	m_vendorInfo.empty();
	m_productVersion.empty();
	m_supportedLCIDs.removeAll();
}


//-----------------------------------------------------------------------------
// SOAeCTranslateResult                                                       |
//-----------------------------------------------------------------------------

SOAeCTranslateResult::SOAeCTranslateResult()
{
	::ZeroMemory(&m_clsid, sizeof(m_clsid));
}


SOAeCTranslateResult::~SOAeCTranslateResult()
{
}




//-----------------------------------------------------------------------------
// SOAeCServerICommunication                                                  |
//-----------------------------------------------------------------------------

SOAeCServerICommunication::SOAeCServerICommunication(void)
{}
SOAeCServerICommunication::~SOAeCServerICommunication(void)
{}
void SOAeCServerICommunication::init(void)
{}
SOAeCServer* SOAeCServerICommunication::getObjectAeCServer(void)
{
	return NULL;
}
HRESULT SOAeCServerICommunication::doStateTransitionImpl(void)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::doDisconnectImpl(void)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::doUpdateImpl(IN BOOL /* fromServer */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}
BOOL SOAeCServerICommunication::serializeImpl(IN OUT SOCltStorage& /* ar */, IN BOOL /* isStoring */)
{
	return FALSE;
}
void SOAeCServerICommunication::connectionFailureImpl(void)
{}
HRESULT SOAeCServerICommunication::getStatus(OUT SOAeCServerStatus& /* status */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::enableConditionByArea(IN SOCmnStringList* /* areas */, IN BOOL /* enable */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::enableConditionByArea(IN LPCTSTR /* area */, IN BOOL /* enable */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::enableConditionBySource(IN SOCmnStringList* /* sources */, IN BOOL /*enable */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::enableConditionBySource(IN LPCTSTR /* source */, IN BOOL /* enable */)
{
	return E_NOTIMPL;
}
SOCltTask* SOAeCServerICommunication::createAckTask(IN SOCmnList<SOAeCEvent>* /* events */, IN LPCTSTR /* acknowledgerID */, IN LPCTSTR /* comment */)
{
	return NULL;
}
HRESULT SOAeCServerICommunication::translateToItemIDs(IN LPCTSTR /* source */, IN DWORD /* eventCategory */, IN LPCTSTR /* conditionName */, IN LPCTSTR /* subconditionName */, IN SOCmnDWORDList* /* assocAttrIDs */, OUT SOCmnList<SOAeCTranslateResult>& /* translateResults */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::getConditionState(IN LPCTSTR /* source */, IN LPCTSTR /*condName */, IN OPTIONAL SOCmnDWORDList* /* eventAttrIDs */, OUT SOAeCConditionState& /* state */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCServerICommunication::queryAvailableFilters(OUT DWORD* /* pFilterMask */)
{
	return E_NOTIMPL;
}
SOAeCAreaBrowser* SOAeCServerICommunication::getAreaBrowserImpl(void)
{
	return NULL;
}
SOAeCEventBrowser* SOAeCServerICommunication::getEventBrowserImpl(void)
{
	return NULL;
}

#ifdef SOFEATURE_DCOM
HRESULT SOAeCServerICommunication::queryInterfaceImpl(IN REFIID /* riid */, IN void** /* interf */)
{
	return E_NOTIMPL;
}
#endif

#ifdef SOFEATURE_TUNNEL
// Softing OPC Tunnel specific
SOCltTPConnection* SOAeCServerICommunication::getTPConnection(void)
{
	return NULL;
}
void SOAeCServerICommunication::setIPPort(WORD /* port */)
{}
WORD SOAeCServerICommunication::getIPPort(void)
{
	return 0;
}
#endif

#endif // SOAEC
