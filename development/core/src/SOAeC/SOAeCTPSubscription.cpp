#include "stdafx.h"

#ifdef SOAEC

#ifdef SOFEATURE_TUNNEL

#include "SOAeCTPCall.h"
#include "SOAeCSubscription.h"
#include "SOAeCTPSubscription.h"

//-----------------------------------------------------------------------------
// SOAeCTPSubscriptionImpl                                                    |
//-----------------------------------------------------------------------------

SOAeCTPSubscriptionImpl::SOAeCTPSubscriptionImpl()
{
	m_serverHandle = 0;
}

SOAeCTPSubscriptionImpl::~SOAeCTPSubscriptionImpl()
{
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the Subscription
//
// returns:
//		S_OK
//
HRESULT SOAeCTPSubscriptionImpl::doDisconnectImpl(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pAeCSubscription->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	HRESULT res = S_OK;
	//-- remove Subscription --
	pAeCSubscription->beforeInterfaceCall();
	res = tpCall->removeSubscription(pAeCSubscription, m_serverHandle);
	pAeCSubscription->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Server::RemoveSubscription [0x%X] "), res));
	pAeCSubscription->setLastError(res, SOAEC_ERR_REMOVE_SUBSCRIPTION);
	pAeCSubscription->setObjectState(SOCltElement::disconnected);
	return res;
} // doDisconnectImpl


//-----------------------------------------------------------------------------
// doStateTransitionImpl
//
// - do the state transition for the Subscription if the target state differs from the
//   actual state of the Subscription
//
// returns:
//      S_OK  - transit to new state
//      E_FAIL - error during state change
//
HRESULT SOAeCTPSubscriptionImpl::doStateTransitionImpl(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	BYTE targetState = pAeCSubscription->getTargetObjectState();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pAeCSubscription->getParentBranch();
	HRESULT res = S_OK;

	// check server object
	if (!server)
	{
		pAeCSubscription->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
		return E_FAIL;
	}

	if (pAeCSubscription->getObjectState() != targetState)
	{
		SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
		SOCmnPointer<SOAeCTPCall> tpCall;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));

		switch (pAeCSubscription->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			//-- connect or start Subscription --
			if (!server->isConnected())
			{
				pAeCSubscription->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
				return E_FAIL;
			}

			pAeCSubscription->beforeInterfaceCall();
			res = tpCall->addSubscription(pAeCSubscription, m_serverHandle);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Server::AddSubscription [0x%X] "), res));
			pAeCSubscription->setLastError(res, SOAEC_ERR_ADD_SUBSCRIPTION);

			if (SUCCEEDED(res))
			{
				res = pAeCSubscription->doUpdate(FALSE, SOAEC_SUBSPAR_FILTER | SOAEC_SUBSPAR_RETURNED_ATTRIBUTES);
			}

			if (SUCCEEDED(res))
			{
				pAeCSubscription->setObjectState(targetState);  // set object state to target state
			}
			else
			{
				// undo all if failed
				res = pAeCSubscription->doDisconnect();
			}
		}
		break;

		default:
			if (targetState == SOCltElement::disconnected)
			{
				res = pAeCSubscription->doDisconnect();
			}
			else
			{
				//-- start or connect Subscription --
				pAeCSubscription->m_bActive = (targetState == SOCltElement::started) ? TRUE : FALSE;
				res = pAeCSubscription->doUpdate(FALSE, SOAEC_SUBSPAR_ACTIVE);

				if (SUCCEEDED(res))
				{
					pAeCSubscription->setObjectState(targetState);
				}
			}

			break;
		}
	}

	return res;
} // doStateTransitionImpl


HRESULT SOAeCTPSubscriptionImpl::doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	HRESULT res = S_OK;

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_FILTER))
	{
//		res = doUpdateFilter(fromServer);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_GET_FILTER : SOAEC_ERR_SET_FILTER);
	}

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_RETURNED_ATTRIBUTES))
	{
		res = doUpdateReturnedAttributes(fromServer);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_SUBS_GET_ATTR : SOAEC_ERR_SUBS_SET_ATTR);
	}

	if (SUCCEEDED(res) && (whatParameters & SOAEC_SUBSPAR_STATE))
	{
//		res = doUpdateSubscriptionState (fromServer, whatParameters);
		pAeCSubscription->setLastError(res, fromServer ? SOAEC_ERR_SUBS_GET_STATE : SOAEC_ERR_SUBS_SET_STATE);
	}

	return res;
}

HRESULT SOAeCTPSubscriptionImpl::doUpdateReturnedAttributes(IN BOOL fromServer)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pAeCSubscription->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	SOAeCReturnedAttrListList listRA(pAeCSubscription->m_retAttrList);
	SOCmnListPosition posRA;
	SOCmnDWORDList listRAC;
	HRESULT res = S_OK;
	HRESULT resRAC;
	size_t category;
	posRA = listRA.getStartPosition();

	if (fromServer)
	{
		while (posRA)
		{
			listRAC = listRA.getNext(posRA, &category);
			listRAC.removeAll();
			pAeCSubscription->beforeInterfaceCall();
			resRAC = tpCall->getReturnedAttributes(m_serverHandle, (DWORD)category, &listRAC);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(resRAC, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Subscription::GetReturnedAttributes [0x%X]"), resRAC));

			if (FAILED(resRAC))
			{
				res = S_FALSE;
			}
		}
	}
	else // to server
	{
		while (posRA)
		{
			listRAC = listRA.getNext(posRA, &category);
			pAeCSubscription->beforeInterfaceCall();
			resRAC = tpCall->selectReturnedAttributes(m_serverHandle, category, &listRAC);
			pAeCSubscription->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(resRAC, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Subscription::SelectReturnedAttributes [0x%X] "), resRAC));

			if (FAILED(resRAC))
			{
				res = S_FALSE;
			}
		}
	}

	return res;
}

BOOL SOAeCTPSubscriptionImpl::refresh(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pAeCSubscription->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	HRESULT res = S_OK;
	pAeCSubscription->beforeInterfaceCall();
	res = tpCall->refresh(m_serverHandle);
	pAeCSubscription->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Subscription::Refresh [0x%X] "), res));
	pAeCSubscription->setLastError(res, SOAEC_ERR_REFRESH);
	return SUCCEEDED(res);
}

BOOL SOAeCTPSubscriptionImpl::cancelRefresh(void)
{
	SOAeCSubscription* pAeCSubscription = getObjectAeCSubscription();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pAeCSubscription->getParentBranch();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)server->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	HRESULT res = S_OK;
	pAeCSubscription->beforeInterfaceCall();
	res = tpCall->cancelRefresh(m_serverHandle);
	pAeCSubscription->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCSubscription, (_T("TPAE::Subscription::CancelRefresh [0x%X] "), res));
	pAeCSubscription->setLastError(res, SOAEC_ERR_CANCEL_REFRESH);
	return SUCCEEDED(res);
}


//-----------------------------------------------------------------------------
// SOAeCTPSubscriptionDefault                                                      |
//-----------------------------------------------------------------------------

SOAeCTPSubscriptionDefault::SOAeCTPSubscriptionDefault(void)
	: SOAeCSubscription()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SOAeCTPSubscriptionDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


#endif // SOFEATURE_TUNNEL

#endif // SOAEC
