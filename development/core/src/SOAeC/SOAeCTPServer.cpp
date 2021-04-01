#include "stdafx.h"

#ifdef SOAEC

#ifdef SOFEATURE_TUNNEL

#include "SOAeCServer.h"
#include "SOAeCTPServer.h"
#include "SOAeCTPCall.h"


//-----------------------------------------------------------------------------
// SOAeCTPServerImpl                                                             |
//-----------------------------------------------------------------------------

SOAeCTPServerImpl::SOAeCTPServerImpl()
{
	SOCMN_ALLOCATE_OBJECT_C(m_tpCon, SOCltTPConnection(), (SOCltTPConnection*));
	m_ipPort = TP_DEFAULT_PORT;
}

SOAeCTPServerImpl::~SOAeCTPServerImpl()
{
	m_tpCon.release();
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//		S_OK
//
HRESULT SOAeCTPServerImpl::doDisconnectImpl(void)
{
	SOCmnPointer<SOAeCEntry> entryAeC = getSOAeCEntry();
	SOCmnPointer<SOCltTP> tpClient = entryAeC->getTPClient();
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOCmnPointer<SOCmnObject> socket = m_tpCon->getSocket();
	m_tpCon->setServer(NULL);

	if (socket.isNotNull())
	{
		tpClient->removeClient(socket);
	}

	pAeCServer->setObjectState(SOCltElement::disconnected);
	return S_OK;
} // doDisconnectImpl


//-----------------------------------------------------------------------------
// doStateTransitionImpl
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//      S_OK  - transit to new state
//      E_FAIL - error during state change
//
HRESULT SOAeCTPServerImpl::doStateTransitionImpl(void)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	BYTE targetState = pAeCServer->getTargetObjectState();
	HRESULT res = S_OK;

	if (pAeCServer->getObjectState() != targetState)
	{
		SOCmnPointer<SOAeCEntry> entryAeC = getSOAeCEntry();
		SOCmnPointer<SOCltTP> tpClient = entryAeC->getTPClient();
		SOCmnEvent* pConEv = m_tpCon->getConnectEvent();
		SOCmnPointer<SOCmnObject> socket = m_tpCon->getSocket();

		switch (targetState)
		{
		case SOCltElement::connected:
		case SOCltElement::started:
		{
			if (!socket)
			{
				SOCmnString hostName = pAeCServer->getNodeName();

				if (hostName.isEmpty())
				{
					hostName = _T("localhost");
				}

				pConEv->reset();

				if (tpClient->connect((SOCltTPConnection*)m_tpCon, (LPCTSTR)hostName, getIPPort()))
				{
					pConEv->waitFor();
				}

				if (m_tpCon->isInit())
				{
					m_tpCon->setServer(pAeCServer);
					pAeCServer->setObjectState(targetState);
				}
				else
				{
					pAeCServer->setLastError(E_FAIL, SOCLT_ERR_CREATE_SERVER);
					pAeCServer->setObjectState(SOCltElement::disconnected);
					res = E_FAIL;
				}
			}
			else
			{
				// already connected
				pAeCServer->setObjectState(targetState);
			}
		}
		break;

		case SOCltElement::disconnected:
		{
			res = pAeCServer->doDisconnect();
		}
		break;
		}
	}

	return res;
} // doStateTransitionImpl


HRESULT SOAeCTPServerImpl::doUpdateImpl(
	IN BOOL /* fromServer */,
	IN OPTIONAL DWORD /* whatParameters */)
{
	return E_FAIL;
}


void SOAeCTPServerImpl::connectionFailureImpl(void)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	m_tpCon->setServer(NULL);
	pAeCServer->setSubscriptionsState(SOCltElement::disconnected);
	pAeCServer->setObjectState(SOCltElement::disconnected);
	pAeCServer->performStateTransition(TRUE, SOCLT_EXEC_ASYNC);
}

SOAeCAreaBrowser* SOAeCTPServerImpl::getAreaBrowserImpl()
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCAreaBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCTPAreaBrowser(pAeCServer));
	return (SOAeCAreaBrowser*)p;
}

SOAeCEventBrowser* SOAeCTPServerImpl::getEventBrowserImpl(void)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCEventBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCTPEventBrowser(pAeCServer));
	return p;
}

SOCltTask* SOAeCTPServerImpl::createAckTask(IN SOCmnList<SOAeCEvent> *events, IN LPCTSTR acknowledgerID, IN LPCTSTR comment)
{
	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOAeCTPAckTask* pTask;
	SOCMN_ALLOCATE_OBJECT(pTask, SOAeCTPAckTask(pAeCServer, events, acknowledgerID, comment));
	return pTask;
}

HRESULT SOAeCTPServerImpl::getStatus(OUT SOAeCServerStatus& status)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SOAeCServer* pAeCServer = getObjectAeCServer();
	SOCmnPointer<SOAeCTPCall> tpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(m_tpCon), (SOAeCTPCall*));
	pAeCServer->beforeInterfaceCall();
	res = tpCall->getStatus(status);
	pAeCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("TPAE::Server::GetStatus [0x%X]"), res));

	if (FAILED(res))
	{
		pAeCServer->setLastError(res, SOCLT_ERR_SRV_GET_STATUS);
	}

	return res;
}

BOOL SOAeCTPServerImpl::logon(IN LPCTSTR userName, IN LPCTSTR password)
{
	SOCmnPointer<SOCltTPConnection> pConnection = getTPConnection();

	if (!pConnection)
	{
		//  non existing connection !?
		return FALSE;
	}

	SOCmnPointer<SOCmnTPCredentials> pCredentials;
	SOCMN_ALLOCATE_OBJECT_C(pCredentials, SOCmnTPCredentials(userName, password), (SOCmnTPCredentials*));
	pConnection->setCredentials(pCredentials);
	return TRUE;
}

BOOL SOAeCTPServerImpl::logoff(void)
{
	SOCmnPointer<SOCltTPConnection> pConnection = getTPConnection();

	if (!pConnection)
	{
		//  non existing connection !?
		return FALSE;
	}

	pConnection->setCredentials(NULL);
	return TRUE;
}


//-----------------------------------------------------------------------------
// SOAeCTPServerDefault                                                      |
//-----------------------------------------------------------------------------

SOAeCTPServerDefault::SOAeCTPServerDefault(void)
	: SOAeCServer()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SOAeCTPServerDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError



//-----------------------------------------------------------------------------
// SOAeCTPAckTask                                                          |
//-----------------------------------------------------------------------------

SOAeCTPAckTask::SOAeCTPAckTask(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events, LPCTSTR acknowledgerID, LPCTSTR comment)
	:   SOCltTask((SOCltElement*)pServer)
{
	_ASSERT_PTR(events);
	m_listEvents.clone(*events);

	if (acknowledgerID)
	{
		m_ackID = acknowledgerID;
	}
	else
	{
#ifndef SOOS_WINDOWS_CE
		TCHAR userName[256];
		DWORD size = 256;

		if (GetUserName(userName, &size))
		{
			m_ackID = userName;
		}

#endif
	}

	m_comment = comment;
}

SOAeCTPAckTask::~SOAeCTPAckTask()
{
	if (m_listEvents.isInit())
	{
		m_listEvents.destroy();
	}
}

//-----------------------------------------------------------------------------
//
// execute

void SOAeCTPAckTask::execute(void)
{
	SOAeCServer* pAeCServer = (SOAeCServer*)(SOCltElement*)m_element;
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pAeCServer->getTPConnection();
	SOCmnPointer<SOAeCTPCall> tpCall;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SOAeCTPCall(tpCon), (SOAeCTPCall*));
	HRESULT res = S_OK;
	pAeCServer->beforeInterfaceCall();
	res = tpCall->ackCondition(&m_listEvents, m_ackID, m_comment);
	pAeCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pAeCServer, (_T("TPAE::Server::AckCondition [0x%X] "), res));
	setResult(res);
	return;
}


#endif // SOFEATURE_TUNNEL

#endif // SOAEC
