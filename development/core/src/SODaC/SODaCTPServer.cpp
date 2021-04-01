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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCTPServer.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : SOAP Server object related classes                          |
//                - SODaCTPServerImpl: proxy for OPC server object            |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_TUNNEL

#include <opcda.h>
#include "SODaCTPServer.h"
#include "SODaCServer.h"
#include "SODaCTPCall.h"


//-----------------------------------------------------------------------------
// SODaCTPServerImpl                                                         |
//-----------------------------------------------------------------------------

SODaCTPServerImpl::SODaCTPServerImpl()
{
	SOCMN_ALLOCATE_OBJECT_C(m_tpCon, SOCltTPConnection(), (SOCltTPConnection*));
	m_ipPort = TP_DEFAULT_PORT;
}

SODaCTPServerImpl::~SODaCTPServerImpl()
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
HRESULT SODaCTPServerImpl::doDisconnectImpl(BOOL /* connectionMonitorStatus */)
{
	SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
	SOCmnPointer<SOCltTP> tpClient = entryDaC->getTPClient();
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SOCmnObject> socket = m_tpCon->getSocket();
	m_tpCon->setServer(NULL);

	if (socket.isNotNull())
	{
		tpClient->removeClient(socket);
	}

	pDaCServer->setObjectState(SOCltElement::disconnected);
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
HRESULT SODaCTPServerImpl::doStateTransitionImpl(void)
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	BYTE targetState = pDaCServer->getTargetObjectState();
	HRESULT res = S_OK;

	if (pDaCServer->getObjectState() != targetState)
	{
		SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
		SOCmnPointer<SOCltTP> tpClient = entryDaC->getTPClient();
		SOCmnEvent* pConEv = m_tpCon->getConnectEvent();
		SOCmnPointer<SOCmnObject> socket = m_tpCon->getSocket();

		switch (targetState)
		{
		case SOCltElement::connected:
		case SOCltElement::started:
		{
			if (!socket)
			{
				SOCmnString hostName = pDaCServer->getNodeName();

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
					m_tpCon->setServer(pDaCServer);
					pDaCServer->setObjectState(targetState);

					// set valid object attributes
					if (pDaCServer->m_validParameter)
					{
						res = pDaCServer->doUpdate(FALSE, pDaCServer->m_validParameter);
						pDaCServer->setLastError(res, 0);
					}

					pDaCServer->setLastError(S_OK, 0);
				}
				else
				{
					pDaCServer->setLastError(E_FAIL, SOCLT_ERR_CREATE_SERVER);
					res = E_FAIL;
				}
			}
			else
			{
				// already connected
				pDaCServer->setObjectState(targetState);
			}
		}
		break;

		case SOCltElement::disconnected:
		{
			res = pDaCServer->doDisconnect();
		}
		break;
		}
	}

	return res;
} // doStateTransitionImpl


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
HRESULT SODaCTPServerImpl::doUpdateImpl(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res = E_INVALIDARG;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(m_tpCon), (SODaCTPCall*));

	if (fromServer)
	{
		//  Nothing implemented so far
	}
	else
	{
		if (whatParameters & SOCLT_SRVPAR_CLIENT_NAME)
		{
			// set client name to the server
			HRESULT resSC;
			SOCmnString clientName = pDaCServer->getClientName();
			pDaCServer->beforeInterfaceCall();
			resSC = tpCall->setClientName(clientName); // don't use res for compatibility with old tunnels which not support setClientName
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("TPDA::Server::SetClientName [0x%X]"), (unsigned int)resSC));
			pDaCServer->setLastError(resSC, SOCLT_ERR_SET_CLIENT_NAME);
			res = (resSC == E_NOTIMPL) ? S_OK : resSC;
		}   //  end if
	}   //  end if ... else

	return res;
} // update


HRESULT SODaCTPServerImpl::getStatus(OUT SODaCServerStatus& status)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(m_tpCon), (SODaCTPCall*));
	pDaCServer->beforeInterfaceCall();
	res = tpCall->getStatus(status);
	pDaCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("TPDA::Server::GetStatus [0x%X]"), (unsigned int)res));

	if (FAILED(res))
	{
		pDaCServer->setLastError(res, SOCLT_ERR_SRV_GET_STATUS);
	}

	return res;
}


HRESULT SODaCTPServerImpl::queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propertyList)
{
	return getPropertiesData(itemID, itemPath, whatData, propertyList);
}


HRESULT SODaCTPServerImpl::getPropertiesData(
	IN LPCTSTR itemID,
	IN LPCTSTR /* itemPath */,
	IN WORD whatData,
	OUT SOCmnList<SODaCPropertyData> &propList)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res;
	BOOL getPropertyValues = ((whatData & SODAC_PROPDATA_VALUE) == SODAC_PROPDATA_VALUE);

	if (!propList.isInit())
	{
		propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	}

	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(m_tpCon), (SODaCTPCall*));
	pDaCServer->beforeInterfaceCall();
	res = tpCall->getProperties(itemID, getPropertyValues, propList.isEmpty(), propList);
	pDaCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("TPDA::Server::GetProperties [0x%X]"), (unsigned int)res));

	if (FAILED(res))
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_PROPERTIES);
	}

	return res;
}


HRESULT SODaCTPServerImpl::read(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN SOCmnString* /* pItemPaths */,
	IN DWORD maxAge,
	OUT SOCmnVariant* pValues,
	OUT WORD* pQualities,
	OUT SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(m_tpCon), (SODaCTPCall*));
	pDaCServer->beforeInterfaceCall();
	res = tpCall->read(count, pItemIDs, maxAge, pValues, pQualities, pTimestamps, pErrors);
	pDaCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("TPDA::Server::Read [0x%X]"), (unsigned int)res));

	if (FAILED(res))
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_READ_ITEM_VALUE);
	}

	return res;
}


HRESULT SODaCTPServerImpl::write(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN SOCmnString* /* pItemPaths */,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCTPCall> tpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(m_tpCon), (SODaCTPCall*));
	pDaCServer->beforeInterfaceCall();
	res = tpCall->write(count, pItemIDs, pValues, pQualities, pTimestamps, pErrors);
	pDaCServer->afterInterfaceCall();
	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("TPDA::Server::Write [0x%X]"), (unsigned int)res));

	if (FAILED(res))
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_WRITE_ITEM_VALUE);
	}

	return res;
}


WORD SODaCTPServerImpl::getSupportedOPCVersion(void)
{
	return SODAC_VERSION_OPC_TPDA;
}


BOOL SODaCTPServerImpl::forceOPCVersion(WORD opcVersion)
{
	if ((opcVersion == SODAC_VERSION_OPC_TPDA) || (opcVersion == SODAC_VERSION_OPC_USEDEFAULT))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


SODaCNameSpaceBrowser* SODaCTPServerImpl::getNameSpaceBrowserImpl()
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SODaCTPNameSpaceBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SODaCTPNameSpaceBrowser(pDaCServer));
	return (SODaCNameSpaceBrowser*)p;
}


void SODaCTPServerImpl::connectionFailureImpl(void)
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	m_tpCon->setServer(NULL);
	pDaCServer->setGroupsState(SOCltElement::disconnected, TRUE);
	pDaCServer->setObjectState(SOCltElement::disconnected);
	pDaCServer->performStateTransition(TRUE, SOCLT_EXEC_ASYNC);
}


BOOL SODaCTPServerImpl::logon(IN LPCTSTR userName, IN LPCTSTR password)
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


BOOL SODaCTPServerImpl::logoff(void)
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
// SODaCTPServerDefault                                                      |
//-----------------------------------------------------------------------------

SODaCTPServerDefault::SODaCTPServerDefault(void)
	: SODaCServer()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaCTPServerDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


#endif // SOFEATURE_TUNNEL

#endif // SODAC
