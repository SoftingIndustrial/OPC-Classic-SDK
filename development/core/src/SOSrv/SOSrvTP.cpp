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
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvTP.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : TP server related classes                                   |
//                - SOSrvTP: TP server object                                 |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_TUNNEL

#include "SOSrv.h"
#include "SOCmnTP.h"
#include "SOSrvTP.h"


//-----------------------------------------------------------------------------
// SOSrvTP                                                                    |
//-----------------------------------------------------------------------------

SOSrvTP::SOSrvTP(void)
	: SOCmnSocketSrv(TP_DEFAULT_PORT),
	  m_bClientAuthenticationEnabled(FALSE)
{
	m_receiverList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_PermissibleCredentialsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
}

SOSrvTP::~SOSrvTP(void)
{
	m_receiverList.destroy();
}

BOOL SOSrvTP::addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN SOCmnObject** pSocket)
{
	BOOL ret = SOCmnSocketSrv::addClient(hSocket, pSockAddr, pSocket);

	if (ret)
	{
		SOCmnPointer<SOSrvTPConnection> pConnection = new SOSrvTPConnection();
		setClientHandle(*pSocket, pConnection);
		pConnection->init(this, *pSocket);
		setTimeOut(*pSocket, 60000);
	}

	return ret;
}

BOOL SOSrvTP::removeClient(SOCmnObject* pSocket, IN LONG reason)
{
	SOCmnPointer<SOSrvTPConnection> pConnection = (SOSrvTPConnection*)getClientHandle(pSocket);

	if (pConnection.isNotNull() == TRUE)
	{
		pConnection->close();
	} // end if

	return SOCmnSocketSrv::removeClient(pSocket, reason);
}

void SOSrvTP::rcvData(IN SOCmnObject* pSocket, LONG size, BYTE* pData)
{
	SOCmnPointer<SOSrvTPConnection> pConnection = (SOSrvTPConnection*)getClientHandle(pSocket);
	pConnection->rcvData(size, pData);
}

BOOL SOSrvTP::isClientAuthenticationEnabled()
{
	return (m_bClientAuthenticationEnabled);
}

void SOSrvTP::enableClientAuthentication(BOOL bEnable)
{
	m_bClientAuthenticationEnabled = bEnable;
}

SOCmnList<SOCmnTPCredentials> *SOSrvTP::getPermissibleCredentialsList()
{
	return (&m_PermissibleCredentialsList);
}

BOOL SOSrvTP::setPermissibleCredentialsList(SOCmnList<SOCmnTPCredentials> *pCredentialsList)
{
	return (m_PermissibleCredentialsList.clone(pCredentialsList));
}


class SOSrvTPWorkItem : public SOCmnWorkItem
{
public:
	SOSrvTPWorkItem(SOSrvTPConnection* pConn, SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
	{
		m_conn = pConn;
		m_conn.addRef();
		m_call = pCall;
		m_call.addRef();
		m_interfaceId = interfaceId;
		m_functionId = functionId;
	}
	void execute(void)
	{
		m_conn->handleRequest(m_call, m_interfaceId, m_functionId);
	}

protected:
	SOCmnPointer<SOSrvTPConnection> m_conn;
	SOCmnPointer<SOCmnTPCall> m_call;
	WORD        m_interfaceId;
	BYTE        m_functionId;

};

class SOSrvTPCloseEvent : public SOCmnWorkItem
{
public:
	SOSrvTPCloseEvent(SOSrvTPConnection* pConn)
	{
		m_conn = pConn;
		m_conn.addRef();
	}
	void execute(void)
	{
		m_conn->handleClose();
	}

protected:
	SOCmnPointer<SOSrvTPConnection> m_conn;
};

//-----------------------------------------------------------------------------
// SOSrvConnection                                                            |
//-----------------------------------------------------------------------------

SOSrvTPConnection::SOSrvTPConnection()
{
}

SOSrvTPConnection::~SOSrvTPConnection()
{
}

void SOSrvTPConnection::close(void)
{
	SOCmnPointer<SOSrvTPCloseEvent> pCloseEvent = new SOSrvTPCloseEvent(this);
	SOCmnPointer<SOSrvEntry> engine = ::getSOSrvEntry();
	engine->addWorkItem(pCloseEvent);
}

void SOSrvTPConnection::addRequestToWorkQueue(SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
{
	SOCmnPointer<SOSrvTPWorkItem> pWorkItem = new SOSrvTPWorkItem(this, pCall, interfaceId, functionId);
	SOCmnPointer<SOSrvEntry> engine = ::getSOSrvEntry();
	engine->addWorkItem(pWorkItem);
}

bool SOSrvTPConnection::handlePingMessage()
{
	SOCmnPointer<SOCmnObject> sock = getSocket();
	m_srv->sndData(sock, m_unmarshal->GetMarshalDataLen(), m_unmarshal->GetMarshalData());
	return true;
}

bool SOSrvTPConnection::handleInitMessage()
{
	SOCmnPointer <SOCmnTP_Marshaler> pMarshaler;
	TP_INITIAL_MESSAGE_HEADER request;
	TP_SESSION_CONTEXT sessionContext;
	SOCmnPointer<SOCmnTPCredentials> pCredentials;
	u_short unFlags;
	TP_SESSION_ID sessionId;
	TP_INITIAL_MESSAGE_HEADER response;
	u_char* pResponseData;
	// Unmarshal header of initial request message. Payload follows depending on
	// the flags. Reject connect request if any of the following conditions apply:
	// 1) The header cannot be unmarshaled.
	// 2) The magic value is wrong.
	// 3) The client is the prototype client.
	// ---------------------------------------------------------------------------
	m_unmarshal->SetReadPos(0);

	if (!unmarshalInitialMessageHeader(m_unmarshal, request))
	{
		return (false);
	}

	if ((request.magic_value != TP_MAGIC_VALUE_TPV1) && (request.magic_value != TP_MAGIC_VALUE_ADD_ACCEPT))
	{
		return (false);
	}

	if (request.protocol_version == 0)
	{
		return (false);
	}

	// Unmarshal session context if the client asked us to reestablish a session.
	// Reject connect request if the session context cannot be unmarshaled.
	// --------------------------------------------------------------------------

	if (request.flags & TP_INIT_FLAG_RESTORE_SESSION)
	{
		if (!unmarshalSessionContext(m_unmarshal, sessionContext))
		{
			return (false);
		}
	};

	// Unmarshal credentials if the client has provided them. Reject
	// connect request if the credentials cannot be unmarshaled.
	// -------------------------------------------------------------
	if (request.flags & TP_INIT_FLAG_CREDENTIALS_PROVIDED)
	{
		if (!unmarshalCredentials(m_unmarshal, pCredentials))
		{
			return (false);
		}
	};

	m_flags = request.flags & TP_INIT_FLAG_PROTOCOL_MASK;

	if (m_flags & TP_INIT_FLAG_USE_FULL_DATE_DEPRECATED)
	{
		// The Client side uses problematic 0x0200 as indicator for using FULL DATE
		m_flags |= TP_INIT_FLAG_USE_FULL_DATE;
	}
	

#ifdef SOOS_LINUX
	m_flags &= !TP_INIT_FLAG_USE_UTF16_STRINGS;

#endif
	// Establish session based on the request attributes. Returns session flags
	// to be sent back to the client (0 if session could not be established).
	// ------------------------------------------------------------------------
	m_init = true;

	unFlags = establishSession(request.flags, sessionContext, pCredentials, sessionId);

	// Build initial response message and marshal it into a buffer.
	// ------------------------------------------------------------
	pMarshaler = new SOCmnTP_Marshaler(m_flags);

	pMarshaler->BeginMarshal(sizeof(TP_INITIAL_MESSAGE_HEADER) + sizeof(TP_SESSION_ID));

	response.message_len = TP_DUMMY_VALUE_FILLED_IN_LATER;

	response.magic_value = TP_MAGIC_VALUE_USE_SRV;

	response.protocol_version = TP_PROTOCOL_VERSION;

	response.flags = unFlags | m_flags | TP_INIT_FLAG_ACCEPT_FULL_DATE;

	response.reserved = 0;

#ifdef SOOS_WINDOWS_CE
	if (!(request.flags & TP_INIT_FLAG_NOT_SUPPORT_UTF16_STRINGS))
	{
		// client supports UTF-16
		response.flags |= TP_INIT_FLAG_USE_UTF16_STRINGS;
		m_flags |= TP_INIT_FLAG_USE_UTF16_STRINGS;
	}

#endif
#ifdef SOOS_LINUX
	response.flags |= TP_INIT_FLAG_NOT_SUPPORT_UTF16_STRINGS;
#endif
	marshalInitialMessageHeader(pMarshaler, response);

	if (unFlags & TP_INIT_FLAG_NEW_SESSION_ID)
	{
		marshalSessionId(pMarshaler, sessionId);
	}

	response.message_len = pMarshaler->GetMarshalDataLen();
	pMarshaler->SetWritePos(0);
	pMarshaler->MarshalLong(response.message_len);
	// Send initial response message to client.
	// ----------------------------------------
	pResponseData = pMarshaler->GetMarshalData();

	if (pResponseData == NULL)
	{
		return (false);
	}

	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (m_srv->sndData(sock, response.message_len, pResponseData) == SOCKET_ERROR)
	{
		return (false);
	}

	// Shutdown client connection if session could not be established.
	// ---------------------------------------------------------------

	if (unFlags == 0)
	{
		m_srv->shutdownClient(sock);
	}

	return (true);
}

u_short SOSrvTPConnection::establishSession(
	u_short unFlags,
	const TP_SESSION_CONTEXT& /* sessionContext */,
	SOCmnTPCredentials* pClientCredentials,
	TP_SESSION_ID& sessionId)
{
	SOSrvTP* pSrvTP;

	// Currently we do not support TP_INIT_FLAG_RESTORE_SESSION.
	// ---------------------------------------------------------

	if (!(unFlags & TP_INIT_FLAG_NEW_SESSION_ID))
	{
		return (0);
	}

	// If we have server credentials, check client credentials against
	// them. Reject connect requests with invalid client credentials.
	// ---------------------------------------------------------------
	pSrvTP = (SOSrvTP*)(SOCmnSocket*)m_srv;

	if (pSrvTP->isClientAuthenticationEnabled())
	{
		SOCmnList<SOCmnTPCredentials> *pPermissibleCredentialsList;
		bool bValidCredentials = false;
		SOCmnListPosition iterator;
		pPermissibleCredentialsList = pSrvTP->getPermissibleCredentialsList();
		iterator = pPermissibleCredentialsList->getStartPosition();

		while (iterator != NULL)
		{
			SOCmnTPCredentials* pCredentials;
			pCredentials = pPermissibleCredentialsList->getNext(iterator);

			if (pCredentials->check(pClientCredentials))
			{
				bValidCredentials = true;
				break;
			};
		};

		if (!bValidCredentials)
		{
			return (0);
		}
	};

	// New session established.
	// ------------------------
	sessionId.server_id[0] = 0;

	sessionId.server_id[1] = 0;

	sessionId.server_id[2] = 0;

	sessionId.server_id[3] = 0;

	return (TP_INIT_FLAG_NEW_SESSION_ID);
}

SOCmnList<SOCmnTPReceiver> *SOSrvTPConnection::getReceiverList(void)
{
	if (m_srv.isNotNull())
	{
		return ((SOSrvTP*)(SOCmnSocket*)m_srv)->getTPReceiverList();
	}
	else
	{
		return NULL;
	}
}

#endif // SOFEATURE_TUNNEL

#endif  // SOSRV
