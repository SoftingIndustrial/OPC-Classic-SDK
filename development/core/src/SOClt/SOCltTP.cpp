#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_TUNNEL

#include "SOClt.h"
#include "SOCmnTP.h"
#include "SOCltTP.h"

//-----------------------------------------------------------------------------
// SOCltTP                                                                    |
//-----------------------------------------------------------------------------

SOCltTP::SOCltTP(void)
{
	m_receiverList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
}

SOCltTP::~SOCltTP(void)
{
}

BOOL SOCltTP::addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN OUT SOCmnObject** pSocket)
{
	BOOL ret = SOCmnSocketClient::addClient(hSocket, pSockAddr, pSocket);

	if (ret)
	{
		SOCmnPointer<SOCltTPConnection> pConnection = (SOCltTPConnection*)getClientHandle(*pSocket);
		ret = pConnection->init(this, *pSocket);

		if (!ret)
		{
			// remove client if sending of the init message failed
			removeClient(*pSocket);
		}
	}

	return ret;
}

BOOL SOCltTP::removeClient(SOCmnObject* pSocket, IN LONG reason)
{
	SOCmnPointer<SOCltTPConnection> pConnection = (SOCltTPConnection*)getClientHandle(pSocket);
	BOOL ret;
	ret = SOCmnSocketClient::removeClient(pSocket, reason);

	if (pConnection.isNotNull())
	{
		pConnection->close();
	}

	return ret;
}

void SOCltTP::rcvData(IN SOCmnObject* pSocket, LONG size, BYTE* pData)
{
	SOCmnPointer<SOCltTPConnection> pConnection = (SOCltTPConnection*)getClientHandle(pSocket);
	pConnection->rcvData(size, pData);
}

void SOCltTP::timeOut(IN SOCmnObject* pSocket)
{
	SOCmnPointer<SOCltTPConnection> pConnection = (SOCltTPConnection*)getClientHandle(pSocket);
	pConnection->sendPing();
}

//-----------------------------------------------------------------------------
// SOCltTPConnection                                                          |
//-----------------------------------------------------------------------------

SOCltTPConnection::SOCltTPConnection(void)
{
	m_timeout = 5000;
	m_connectEvent.create();
}

SOCltTPConnection::~SOCltTPConnection(void)
{
}

SOCmnTPCredentials* SOCltTPConnection::getCredentials()
{
	SOCmnSingleLock<SOCmnSync> lock(&m_sync);
	m_pCredentials.addRef();
	return (m_pCredentials);
}

void SOCltTPConnection::setCredentials(SOCmnTPCredentials* pCredentials)
{
	SOCmnSingleLock<SOCmnSync> lock(&m_sync);
	m_pCredentials = pCredentials;
	m_pCredentials.addRef();
}

bool SOCltTPConnection::init(SOCmnSocket* pSrv, SOCmnObject* pSocket)
{
	bool ret;
	ret = SOCmnTPConnection::init(pSrv, pSocket);

	if (ret)
	{
		ret = sendInitMessage();
		m_waitForPing = false;
	}

	return ret;
}

void SOCltTPConnection::close(void)
{
	SOCmnTPConnection::close();
	m_connectEvent.signal();
}

HRESULT SOCltTPConnection::invoke(SOCmnTPCall* pCall, BOOL waitForReply)
{
	if (!SOCLT_TP_CHECK_DEMO)
	{
		return SOCmnTPConnection::invoke(pCall, waitForReply);
	}

	return E_ACCESSDENIED;
}

void SOCltTPConnection::sendPing(void)
{
	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (m_waitForPing)
	{
		m_srv->removeClient(sock);
	}
	else
	{
		SOCmnPointer <SOCmnTP_Marshaler> pMarshaler = new SOCmnTP_Marshaler(m_flags);
		pMarshaler->BeginMarshal(TP_IDLE_MESSAGE_LEN);
		pMarshaler->MarshalLong(TP_IDLE_MESSAGE_LEN);

		if (m_srv->sndData(sock, TP_IDLE_MESSAGE_LEN, pMarshaler->GetMarshalData()) == SOCKET_ERROR)
		{
			return;
		}

		m_waitForPing = true;
		m_srv->resetIdleTimer(sock);
	}
}
bool SOCltTPConnection::handlePingMessage()
{
	m_waitForPing = false;
	return true;
}

bool SOCltTPConnection::handleInitMessage()
{
	TP_INITIAL_MESSAGE_HEADER response;
	// Unmarshal header of initial response message. Payload follows depending
	// on the flags. Stop connecting if any of the following conditions apply:
	// 1) The header cannot be unmarshaled.
	// 2) The magic value is wrong.
	// 3) The server is the prototype server.
	// -----------------------------------------------------------------------
	m_unmarshal->SetReadPos(0);

	if (!unmarshalInitialMessageHeader(m_unmarshal, response))
	{
		return (false);
	}

	if ((response.magic_value != TP_MAGIC_VALUE_TPV1) && (response.magic_value != TP_MAGIC_VALUE_ADD_ACCEPT))
	{
		return (false);
	}

	if (response.protocol_version == 0)
	{
		return (false);
	}

	m_flags = response.flags & TP_INIT_FLAG_PROTOCOL_MASK;

	if ( (m_flags & TP_INIT_FLAG_ACCEPT_FULL_DATE) != TP_INIT_FLAG_ACCEPT_FULL_DATE)
	{
		m_flags &= (~TP_INIT_FLAG_USE_FULL_DATE);
	}

	// If the server did not establish a session (indicated by a cleared
	// TP_INIT_FLAG_NEW_SESSION_ID flag) we shutdown the connection.
	// -----------------------------------------------------------------
	m_init = true;
	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (!(response.flags & TP_INIT_FLAG_NEW_SESSION_ID))
	{
		m_srv->shutdownClient(sock);
		return (true);
	};

	// Signal we are connected to the server.
	// --------------------------------------
	m_srv->setTimeOut(sock, m_timeout);

	m_connectEvent.signal();

	return true;
}

bool SOCltTPConnection::sendInitMessage()
{
	SOCmnString strUserName;
	SOCmnString strPassword;
	SOCmnPointer <SOCmnTP_Marshaler> pMarshaler;
	TP_INITIAL_MESSAGE_HEADER request;
	SOCmnPointer<SOCmnTPCredentials> pCredentials;
	u_char* pRequestData;
	// Build initial request message and marshal it into a buffer. The size
	// passed to BeginMarshal is an estimation of the maximum size required.
	// The marshaler will reallocate if it turns out to be too small.
	// ---------------------------------------------------------------------
	request.message_len = sizeof(TP_INITIAL_MESSAGE_HEADER);
	request.magic_value = TP_MAGIC_VALUE_USE_CLT;
	request.protocol_version = TP_PROTOCOL_VERSION;
	request.flags = TP_INIT_FLAG_NEW_SESSION_ID | TP_INIT_FLAG_CREDENTIALS_PROVIDED | TP_INIT_FLAG_USE_FULL_DATE;
	request.reserved = 0;
#ifdef SOOS_WINDOWS_CE
	request.flags |= TP_INIT_FLAG_USE_UTF16_STRINGS;
#endif
#ifdef SOOS_LINUX
	request.flags |= TP_INIT_FLAG_NOT_SUPPORT_UTF16_STRINGS;
#endif
	pMarshaler = new SOCmnTP_Marshaler(0); // not protocol flags allowed for the marshaling of the init message!
	pMarshaler->BeginMarshal(sizeof(TP_INITIAL_MESSAGE_HEADER) + 200);
	marshalInitialMessageHeader(pMarshaler, request);
	pCredentials = getCredentials();
	marshalCredentials(pMarshaler, pCredentials);
	pMarshaler->SetWritePos(0);
	request.message_len = pMarshaler->GetMarshalDataLen();
	pMarshaler->MarshalLong(request.message_len);
	// Send initial request message to server.
	// ---------------------------------------
	pRequestData = pMarshaler->GetMarshalData();

	if (pRequestData == NULL)
	{
		return (false);
	}

	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (m_srv->sndData(sock, request.message_len, pRequestData) == SOCKET_ERROR)
	{
		return (false);
	}

	m_srv->setTimeOut(sock, m_timeout);
	return (true);
}

SOCmnList<SOCmnTPReceiver> *SOCltTPConnection::getReceiverList(void)
{
	if (m_srv.isNotNull())
	{
		return ((SOCltTP*)(SOCmnSocket*)m_srv)->getTPReceiverList();
	}
	else
	{
		return NULL;
	}
}

#endif // SOFEATURE_TUNNEL

#endif // SOCLT
