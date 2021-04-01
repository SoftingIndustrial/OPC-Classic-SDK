#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_SOAP

#include "SOCltHttp.h"

//-----------------------------------------------------------------------------
// SOCltHTTP                                                                  |
//-----------------------------------------------------------------------------

SOCltHTTP::SOCltHTTP(void)
{
	m_completeList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_activeList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_completeEventID = SOCMNEVENT_INVALID_ID;
	m_activeEventID = SOCMNEVENT_INVALID_ID;
}

SOCltHTTP::~SOCltHTTP(void)
{
	m_completeList.destroy();
	m_activeList.destroy();
}

BOOL SOCltHTTP::start(void)
{
	BOOL ret = SOCmnSocketClient::start();

	if (ret)
	{
		if (m_completeEventID == SOCMNEVENT_INVALID_ID)
		{
			m_completeEventID = m_completeAndTimeoutThread.createEvent();
		}

		if (m_activeEventID == SOCMNEVENT_INVALID_ID)
		{
			m_activeEventID = m_completeAndTimeoutThread.createEvent();
		}

		if (!m_completeAndTimeoutThread.isRunning())
		{
			ret = m_completeAndTimeoutThread.start(completeAndTimeoutThreadProc, (void*)this);

			if (ret)
			{
				m_completeAndTimeoutThread.setPriority(THREAD_PRIORITY_HIGHEST);
			}
		}
	}

	return ret;
}

BOOL SOCltHTTP::stop(void)
{
	BOOL ret = SOCmnSocketClient::stop();
	m_completeAndTimeoutThread.stop();
	return ret;
}

BOOL SOCltHTTP::addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN SOCmnObject** ppSocket)
{
	BOOL ret = SOCmnSocketClient::addClient(hSocket, pSockAddr, ppSocket);

	if (ret)
	{
		SOCmnPointer<SOCltHTTPConnection> clientConnection = (SOCltHTTPConnection*)getClientHandle(*ppSocket);
		clientConnection->m_socket = *ppSocket;
		clientConnection->m_socket.addRef();
	}
	else
	{
		SOCltHTTPConnection* pClientConnection = (SOCltHTTPConnection*)*ppSocket;
		pClientConnection->m_connected = FALSE;
		pClientConnection->m_isConnectedEvent.signal();
	}

	return ret;
}

void SOCltHTTP::connectedClient(IN SOCmnObject* pSocket)
{
	if (!pSocket)
	{
		return;
	}

	// set connected flag in client connnection object
	SOCmnPointer<SOCltHTTPConnection> clientConnection;
	clientConnection = (SOCltHTTPConnection*)getClientHandle(pSocket);
	clientConnection->m_connected = TRUE;
	clientConnection->m_isConnectedEvent.signal();
}

BOOL SOCltHTTP::removeClient(SOCmnObject* pSocket, IN LONG reason)
{
	if (!pSocket)
	{
		return FALSE;
	}

	SOCmnPointer<SOCltHTTPConnection> clientConnection;
	clientConnection = (SOCltHTTPConnection*)getClientHandle(pSocket);
	BOOL ret;
	ret = SOCmnSocketClient::removeClient(pSocket, reason);

	if (clientConnection.isNotNull())
	{
		clientConnection->close(reason);
	}

	return ret;
}

void SOCltHTTP::rcvData(IN SOCmnObject* pSocket, LONG size, BYTE* pData)
{
	if (!pSocket)
	{
		return;
	}

	SOCmnPointer<SOCltHTTPConnection> clientConnection;
	clientConnection = (SOCltHTTPConnection*)getClientHandle(pSocket);

	if (clientConnection.isNotNull())
	{
		clientConnection->rcvData(size, pData);
	}
}

BOOL SOCltHTTP::addConnection(IN SOCltHTTPConnection* pCon)
{
	if (pCon)
	{
		pCon->setHTTPClient(this);
	}

	return TRUE;
}

void SOCltHTTP::addCompleteCall(SOCltHTTPCall* pCall)
{
	SOCmnList<SOCltHTTPCall> completeList(getCompleteList());
	completeList.add(pCall);
	m_completeAndTimeoutThread.getEvents()->signal(m_completeEventID);
}

void SOCltHTTP::addActiveCall(SOCltHTTPCall* pCall)
{
	SOCmnList<SOCltHTTPCall> activeList(getActiveList());
	activeList.add(pCall);
	m_completeAndTimeoutThread.getEvents()->signal(m_activeEventID);
}

DWORD SOCltHTTP::completeAndTimeoutThreadProc(void* context)
{
	SOCltHTTP* clt = (SOCltHTTP*)context;
	return clt->completeAndTimeoutThread();
}

void SOCltHTTP::timeOut(IN SOCmnObject* pSocket)
{
	SOCmnPointer<SOCltHTTPConnection> pConnection = (SOCltHTTPConnection*)getClientHandle(pSocket);
	SOCmnSocketClient::timeOut(pSocket);

	if (pConnection.isNotNull())
	{
		SOCmnPointer<SOCltServer> server(pConnection->getServer());

		if (server.isNotNull())
		{
			server->connectionFailure();
		}
	}
}


DWORD SOCltHTTP::completeAndTimeoutThread(void)
{
	BOOL end = FALSE;
	DWORD ret;
	DWORD waitTime = SOCMN_TIME_40DAYS;

	while (!end)
	{
		if (waitTime != 0)
		{
			ret = m_completeAndTimeoutThread.waitForEvents(waitTime);
		}
		else
		{
			ret = SOCMNEVENT_INVALID_ID;
		}

		if (ret == m_completeAndTimeoutThread.getStopEventId())
		{
			// end
			end = TRUE;
		}
		else
		{
			// remove finished calls from active list
			SOCmnList<SOCltHTTPCall> activeList(getActiveList());
			SOCmnListPosition posAPrev, posACur, posANext;
			SOCltHTTPCall* pCallA;
			posACur = NULL;
			posANext = activeList.getStartPosition();

			while (posANext)
			{
				posAPrev = posACur;
				posACur = posANext;
				pCallA = activeList.getNext(posANext);

				if (pCallA->getObjectState() != SOCltHTTPCall::active)
				{
					activeList.removeAt(posACur, posAPrev);
					posACur = posAPrev;
				}
			}

			activeList.autoUnlock();
			// complete the calls
			SOCmnList<SOCltHTTPCall> completeList(getCompleteList());
			SOCmnPointer<SOCltHTTPCall> callC;
			callC = completeList.pop();
			completeList.autoUnlock();

			while (callC.isNotNull())
			{
				callC->onComplete();
				callC = completeList.pop();
				completeList.autoUnlock();
			}

			// calculate new timeouts and complete timed out calls
			DWORD now = GetTickCount();
			DWORD actTime;
			DWORD timeout;
			DWORD ellapsed;
			waitTime = SOCMN_TIME_40DAYS;
			posACur = NULL;
			posANext = activeList.getStartPosition();

			while (posANext)
			{
				posAPrev = posACur;
				posACur = posANext;
				pCallA = activeList.getNext(posANext);

				if (pCallA->getObjectState() == SOCltHTTPCall::active)
				{
					actTime = pCallA->getActivationTime();
					timeout = pCallA->getTimeout();

					if ((timeout == INFINITE) || ((timeout == SOCMN_TIME_40DAYS)))
					{
						SOCmnPointer<SOCltHTTPConnection> httpCon = pCallA->getHTTPConnection();

						if (httpCon.isNotNull())
						{
							SOCmnPointer<SOCltServer> server = httpCon->getServer();

							if (server.isNotNull())
							{
								timeout = server->getRequestTimeout();
							}
						}
					}

					ellapsed = getTimeSpan(actTime, now);

					if (ellapsed < timeout)
					{
						waitTime = __min(waitTime, timeout - ellapsed);
					}
					else
					{
						_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("timeout: %lums"), timeout));
						SOCmnPointer<SOCltHTTPConnection> con = pCallA->getHTTPConnection();
						pCallA->m_httpStatusCode = SOCLTHTTP_CALL_TIMEOUT;
						con->finishCall(pCallA);

						if (con.isNotNull())
						{
							con->disconnect();
						}

						pCallA->onComplete();
					}
				}

				if (pCallA->getObjectState() != SOCltHTTPCall::active)
				{
					activeList.removeAt(posACur, posAPrev);
					posACur = posAPrev;
				}
			}

			activeList.autoUnlock();
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------
// SOCltHTTPConnection                                                        |
//-----------------------------------------------------------------------------

SOCltHTTPConnection::SOCltHTTPConnection(void)
{
	m_connected = FALSE;
	m_port = 80;
	m_hostName = _T("localhost");
	m_invokeStartCallEvent.create();
	m_isConnectedEvent.create();
	m_pResData = NULL;
	m_resSize = 0;
	m_resBufferSize = 0;
	m_callList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
}

SOCltHTTPConnection::~SOCltHTTPConnection(void)
{
	m_callList.destroy();
	m_call.release();

	if (m_pResData)
	{
		free(m_pResData);
	}

	m_httpClient.release();
	m_socket.release();
	m_server.release();
}

void SOCltHTTPConnection::close(LONG reason)
{
	m_socket.release();
	m_connected = FALSE;
	rcvData(0, NULL); // may finish call waiting for connection close
	BOOL waitForCl = (getObjectState() == waitForClose) ? TRUE : FALSE;
	setObjectState(SOCltHTTPConnection::idle);

	if (!waitForCl)
	{
		SOCmnList<SOCltHTTPCall> callList(m_callList);
		callList.autoLock();
		SOCmnPointer<SOCltHTTPCall> call = getHTTPCall();
		SOCltHTTPCall* pCall;
		SOCmnListPosition posN, posP, posC;
		WORD httpStatus;

		if (reason == -1)
		{
			httpStatus = SOCLTHTTP_CALL_CANCELED;
		}
		else
		{
			httpStatus = SOCLTHTTP_CALL_DISCONNECT;
		}

		// finish active call
		if (call.isNotNull())
		{
			if (call->getObjectState() == SOCltHTTPCall::active)
			{
				call->resetData(TRUE);
				call->m_httpStatusCode = httpStatus;
				finishCall(call, FALSE);

				if (call->isSynchronous())
				{
					call->getSyncInvokeRcvDataEvent()->signal();
				}
				else
				{
					m_httpClient->addCompleteCall(call);
				}
			}
		}

		m_call.release();
		// cleanup call list
		posN = callList.getStartPosition();

		while (posN)
		{
			posC = posN;
			posP = posC;
			pCall = callList.getNext(posN);
			pCall->resetData(TRUE);
			pCall->m_httpStatusCode = httpStatus;
			finishCall(pCall, FALSE);

			if (pCall->isAsynchronous())
			{
				m_httpClient->addCompleteCall(pCall);
				callList.removeAt(posC, posP);
				posC = posP;
			}
		}

		if (!callList.isEmpty())
		{
			m_invokeStartCallEvent.signal();
		}

		callList.autoUnlock();
	}

	m_isConnectedEvent.signal();
}

LPCTSTR SOCltHTTPConnection::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[HCO|%8.8lX|]"), (DWORD)this->getOpcHandle());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId

HRESULT SOCltHTTPConnection::invoke(IN SOCltHTTPCall* pCall, IN BOOL synchronous)
{
	pCall->setObjectState(idle);
	pCall->resetData(TRUE);
	pCall->setHTTPConnection(this);
	pCall->setSynchronous(synchronous);
	m_callList.lock();

	DWORD reqTimeout = pCall->getTimeout();

	if ((reqTimeout == INFINITE) || ((reqTimeout == SOCMN_TIME_40DAYS)))
	{
		SOCmnPointer<SOCltServer> server = getServer();

		if (server.isNotNull())
		{
			reqTimeout = server->getRequestTimeout();
		}
	}

	if (!m_call)
	{
		m_call = pCall;
		m_call.addRef();
		m_callList.unlock();
	}
	else
	{
		if ((SOCltHTTPCall*)m_call != pCall)  // this is true if invoke is called from finishCall
		{
			// another call is being executed
			m_callList.add(pCall);
			m_callList.unlock();

			if (synchronous)
			{
				while (TRUE)
				{
					SOCltHTTPCall* pHead;
					m_invokeStartCallEvent.waitFor(reqTimeout);
					m_callList.lock();

					if (m_call.isNotNull())
					{
						// another thread already started a call
						m_callList.unlock();
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("another call already started")));
						continue;
					}

					pHead = m_callList.getHead();

					if (pHead == pCall)
					{
						pCall->addRef();
						m_callList.removeHead();

						if (pCall->getObjectState() != SOCltHTTPCall::idle)
						{
							// call already finished e.g. timeout or no connection
							if (!m_callList.isEmpty())
							{
								m_invokeStartCallEvent.signal();
							}

							m_callList.unlock();
							pCall->release();
							return OPC_E_HTTP_STATUS_BASE | pCall->getHTTPStatusCode();
						}

						m_call = pCall;
						m_callList.unlock();
						break;
					}

					m_callList.unlock();
					m_invokeStartCallEvent.signal();
				}
			}
			else
			{
				// added asynchronous call to call list.
				return S_OK;
			}
		}
		else
		{
			m_callList.unlock();
		}
	}

	if (getObjectState() == waitForClose)
	{
		// wait for connection beeing closed from the server and
		// not closed yet -> Disconnect self from the server and send request on new connection.
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("Close connection which is marked to be closed")));
		disconnect();
	}

	if (!isConnected())
	{
		// no connection established
		m_isConnectedEvent.reset();

		_TRACEO(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, m_httpClient, (_T("conencting...")));
		// try to connect to HTTP server
		if (!m_httpClient->connect(this, m_hostName, m_port))
		{
			pCall->setHTTPStatusCode(SOCLTHTTP_CALL_DISCONNECT);
			finishCall(pCall);
			return OPC_E_HTTP_STATUS_BASE | SOCLTHTTP_CALL_DISCONNECT; // can't connect
		}

		m_isConnectedEvent.waitFor(reqTimeout);

		if (!isConnected())
		{
			pCall->setHTTPStatusCode(SOCLTHTTP_CALL_DISCONNECT);
			finishCall(pCall);
			return OPC_E_HTTP_STATUS_BASE | SOCLTHTTP_CALL_DISCONNECT; // can't connect
		}
	}

	DWORD reqSize = 0;
	BYTE* pReqData = NULL;
	pCall->getRequestData(&reqSize, &pReqData);
	pCall->setObjectState(SOCltHTTPCall::active);
	setObjectState(waitForResponse);

	if (SOCKET_ERROR == m_httpClient->sndData(m_socket, reqSize, pReqData))
	{
		// error sending request
		pCall->setHTTPStatusCode(SOCLTHTTP_CALL_CANTSEND);
		finishCall(pCall);
		return OPC_E_HTTP_STATUS_BASE | SOCLTHTTP_CALL_CANTSEND; // can't send
	}

	pCall->m_activationTime = GetTickCount();

	if (!synchronous)
	{
		m_httpClient->addActiveCall(pCall);
		return S_OK; // started HTTP call asynchronous -> send the request
	}

	BOOL waitSuccess;
	waitSuccess = pCall->getSyncInvokeRcvDataEvent()->waitFor(reqTimeout);

	if (!waitSuccess)
	{
		pCall->setHTTPStatusCode(SOCLTHTTP_CALL_TIMEOUT);
		finishCall(pCall);
		return OPC_E_HTTP_STATUS_BASE | SOCLTHTTP_CALL_TIMEOUT; // timeout
	}

	return S_OK;
}

void SOCltHTTPConnection::finishCall(IN SOCltHTTPCall* pCall, BOOL restartNext)
{
	pCall->setObjectState(SOCltHTTPCall::finished);

	if (getObjectState() == waitForResponse)
	{
		setObjectState(idle);
	}

	SOCmnList<SOCltHTTPCall> callList(m_callList);
	callList.autoLock();
	m_call.release();

	if ((restartNext) && (!callList.isEmpty()))
	{
		SOCmnPointer<SOCltHTTPCall> head;
		head = callList.getHead();
		head.addRef();

		if (head->isAsynchronous())
		{
			callList.removeHead();

			while (head->getObjectState() != SOCltHTTPCall::idle)
			{
				// call already finished e.g. timeout or no connection
				m_httpClient->addCompleteCall(head);
				head = m_callList.getHead();
				head.addRef();

				if (!head)
				{
					break;
				}

				if (head->isSynchronous())
				{
					m_invokeStartCallEvent.signal();
					return;
				}

				callList.removeHead();
			}

			if (head.isNotNull())
			{
				// start asynchronous call
				m_call = head;
				callList.autoUnlock();
				invoke(head, FALSE);
			}
		}
		else
		{
			// synchronous -> signal other thread
			m_invokeStartCallEvent.signal();
		}
	}
}

void SOCltHTTPConnection::rcvData(LONG resSize, BYTE* pResData)
{
	if ((getObjectState() == waitForResponse) || (getObjectState() == waitForClose))
	{
		getCallLock()->lock();
		SOCmnPointer<SOCltHTTPCall> call = getHTTPCall();

		if (call.isNotNull())
		{
			if (resSize > 0)
			{
				if (m_resBufferSize < (m_resSize + resSize + 1))
				{
					m_pResData = (BYTE*)realloc(m_pResData, m_resSize + resSize + 1);
				}

				if (m_pResData)
				{
					memcpy(m_pResData + m_resSize, pResData, resSize);
					m_resSize += resSize;
					m_pResData[m_resSize] = 0;
				}
			}

			BOOL end = FALSE;
			BYTE* pBodyS;
			DWORD bodyLen = 0;
			DWORD discardTo;
			SOCmnString connectionAttribute;
			SOCmnStringList attributeList(getAttributeList());

			while ((!end) && (m_resSize > 0))
			{
				end = TRUE;

				// check if header is complete
				// note: if a NUL char is in the data stream the strstr
				// functions cannot find the end of header mark
				// so stop waiting for more header if a NUL byte was received.
				// NUL bytes in the content section are okay.
				if ((strstr((CHAR*)m_pResData, "\n\n") == NULL) &&
					(strstr((CHAR*)m_pResData, "\n\r\n") == NULL) &&
					(strlen((CHAR*)m_pResData) == (DWORD)m_resSize - 1))
				{
					break;    // HTTP header not complete
				}

				// decode message
				pBodyS = NULL;
				discardTo = decodeHTTPResponseHeader(m_resSize, m_pResData, &pBodyS);

				if (getHTTPStatusCode() != 100) // check for 100-continue
				{
					attributeList = getAttributeList();
					DWORD attributesCount = attributeList.getCount();

					if (attributesCount == 0)
					{
						break;
					}

					connectionAttribute = attributeList.findKey(_T("connection"));

					if (_tcsicmp(connectionAttribute, _T("close")) == 0)
					{
						setObjectState(waitForClose);
					}

					if (getHTTPStatusCode() == 200)
					{
						if (getContentsLength() != 0)
						{
							if ((bodyLen = getContentsLength()) != 0)
							{
								// check if complete contents arrived
								if ((DWORD)m_resSize < discardTo + bodyLen)
								{
									break;
								}

								discardTo += bodyLen + 1;
							}
						}
						else
						{
							// no Content-length -> Connection: close or Transfer-Encoding: chunked
							if (_tcsicmp(connectionAttribute, _T("close")) == 0)
							{
								if (isConnected())
								{
									break;    // wait for connection to be closed
								}

								// connection closed -> response is all data received
								bodyLen = m_resSize - discardTo;
								discardTo += bodyLen + 1;
							}
							else
							{
								SOCmnString transEncAttr;
								transEncAttr = attributeList.findKey(_T("transfer-encoding"));

								if (_tcsicmp(transEncAttr, _T("chunked")) == 0)
								{
									// chunked
									bodyLen = m_resSize - discardTo;

									if ((!pBodyS) || (bodyLen == 0))
									{
										break;    // waiting for first chunk
									}

									BOOL allChunks = FALSE;
									BOOL errorOnChunks = FALSE;
									DWORD chunkLenIdx = 0;
									DWORD chunkLenSize;
									LPSTR chunkCRLFIdx = NULL;
									DWORD chunkLen;

									// check if received all chunks
									while (!allChunks)
									{
										if (chunkLenIdx >= bodyLen)
										{
											break;
										}

										chunkLen = strtoul((char*)(pBodyS + chunkLenIdx), &chunkCRLFIdx, 16);

										if (!chunkCRLFIdx)
										{
											errorOnChunks = TRUE;
											break;
										}

										chunkLenSize = (DWORD)(chunkCRLFIdx - (char*)pBodyS) - chunkLenIdx;

										if (chunkLenSize + chunkLenIdx + 2 > bodyLen)
										{
											break;
										}

										if ((*(chunkCRLFIdx) != '\r') || (*(chunkCRLFIdx + 1) != '\n'))
										{
											errorOnChunks = TRUE;
											break;
										}

										if (chunkLen != 0)
										{
											chunkLenIdx += chunkLen + chunkLenSize + 4;    // chunk size + chunk len size + 2 * CRLF
										}
										else
										{
											allChunks = TRUE;
										}
									}

									if (errorOnChunks)
									{
										m_resSize = 0;
									}

									if (!allChunks)
									{
										break;
									}

									// copy body to a flat data stream
									DWORD curDataIdx = 0;
									allChunks = FALSE;
									chunkLenIdx = 0;

									while (!allChunks)
									{
										chunkLen = strtoul((char*)(pBodyS + chunkLenIdx), &chunkCRLFIdx, 16);
										chunkLenSize = (DWORD)(chunkCRLFIdx - (char*)pBodyS) - chunkLenIdx;

										if (chunkLen != 0)
										{
											memmove(pBodyS + curDataIdx, pBodyS + (chunkLenIdx + chunkLenSize + 2), chunkLen);
											curDataIdx += chunkLen;
										}
										else
										{
											allChunks = TRUE;
										}

										chunkLenIdx += chunkLen + chunkLenSize + 4; // chunk size + chunk len size + 2 * CRLF
									}

									bodyLen = curDataIdx;
									*(pBodyS + chunkLenIdx) = 0;
									discardTo += chunkLenIdx;
								}
								else
								{
									// invalid HTTP response - no body size
									m_resSize = 0;
									break;
								}
							}
						}

						// end call
						call->setResponse(this, bodyLen, pBodyS);
						finishCall(call);

						if (call->isSynchronous())
						{
							call->getSyncInvokeRcvDataEvent()->signal();
						}
						else
						{
							m_httpClient->addCompleteCall(call);
						}
					}
					else
					{
						// status not 200-ok
						call->setHTTPStatusCode(getHTTPStatusCode());
						finishCall(call);

						if (call->isSynchronous())
						{
							call->getSyncInvokeRcvDataEvent()->signal();
						}
						else
						{
							m_httpClient->addCompleteCall(call);
						}
					}
				}
				if (m_resSize > 0)
				{
					if (discardTo >= (DWORD)m_resSize - 1)
					{
						m_resSize = 0;
					}
					else
					{
						if (discardTo > 0)
						{
							end = FALSE;
							memmove(m_pResData, m_pResData + discardTo, m_resSize - discardTo);
							m_resSize -= discardTo;
							continue;
						}
					}
				}
			}
		}
		else
		{
			m_resSize = 0;
		}
		getCallLock()->unlock();
	}
	else
	{
		m_resSize = 0;
	}
}

void SOCltHTTPConnection::disconnect(void)
{
	if (m_socket.isNotNull())
	{
		m_isConnectedEvent.reset();
		m_httpClient->shutdownClient(m_socket);
		m_isConnectedEvent.waitFor();
	}
}



//-----------------------------------------------------------------------------
// SOCltHTTPCall                                                              |
//-----------------------------------------------------------------------------

SOCltHTTPCall::SOCltHTTPCall(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	setObjectState(idle);
	m_pResData = NULL;
	m_resSize = 0;
	m_httpMethod = 0;
	m_pReqData = NULL;
	m_reqSize = 0;
	m_httpVersion = 0;
	m_httpStatusCode = 0;
	m_attributeList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	m_timeout = SOCMN_TIME_40DAYS;
	m_activationTime = 0;
	m_synchronous = TRUE;
	m_syncInvokeRcvDataEvent.create();
}

SOCltHTTPCall::~SOCltHTTPCall(void)
{
	if (m_pResData)
	{
		free(m_pResData);
	}

	if (m_pReqData)
	{
		freeMemory(m_pReqData);
	}

	m_attributeList.destroy();
}

LPCTSTR SOCltHTTPCall::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[HCL|%8.8lX|]"), (DWORD)this->getOpcHandle());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId

void SOCltHTTPCall::setURL(IN LPCTSTR url)
{
	m_url = url;
}

LPCTSTR SOCltHTTPCall::getURL(void)
{
	return (LPCTSTR) m_url;
}

DWORD SOCltHTTPCall::getTimeout(void)
{
	return m_timeout;
}

void SOCltHTTPCall::setTimeout(DWORD dw)
{
	if (dw < SOCMN_TIME_40DAYS)
	{
		m_timeout = dw;
	}
	else
	{
		m_timeout = SOCMN_TIME_40DAYS;
	}
}


void SOCltHTTPCall::getRequestData(OUT DWORD* pReqSize, OUT BYTE** ppReqData)
{
	*pReqSize = m_reqSize;
	*ppReqData = m_pReqData;
}

void SOCltHTTPCall::resetData(IN BOOL onlyResponse)
{
	if (m_pResData)
	{
		free(m_pResData);
	}

	m_resSize = 0;
	m_pResData = NULL;
	m_httpStatusCode = 0;
	m_httpVersion = 0;
	m_attributeList.removeAll();

	if (!onlyResponse)
	{
		if (m_pReqData)
		{
			freeMemory(m_pReqData);
		}

		m_pReqData = NULL;
		m_reqSize = 0;
	}
}

void SOCltHTTPCall::setRequestData(IN DWORD reqSize, IN BYTE* pReqData)
{
	if (m_pReqData)
	{
		freeMemory(m_pReqData);
	}

	m_pReqData = (BYTE*)allocateMemory(reqSize + 1);

	if (m_pReqData)
	{
		m_reqSize = reqSize;
		memcpy(m_pReqData, pReqData, reqSize);
		m_pReqData[reqSize] = 0;
	}
}

HRESULT SOCltHTTPCall::createRequestData(IN SOCmnStringEx* pReqBody)
{
	SOCmnPointer<SOCltHTTPConnection> con(getHTTPConnection());

	if (!con)
	{
		return E_FAIL;
	}

	SOCmnStringEx reqHeader;
	SOCmnStringEx szAttributes, szAttr;
	SOCmnString url;
	szAttr.getBuffer(64);
	szAttributes.getBuffer(256);
	DWORD bodyLen = 0;
	BYTE* pReqBodyA = NULL;

	if (pReqBody)
	{
		pReqBodyA = pReqBody->cloneToUTF8(&bodyLen);

		if (!pReqBodyA)
		{
			m_reqSize = 0;
			return E_OUTOFMEMORY;
		}
	}

	url = getURL();

	if (url.isEmpty())
	{
		url = con->getDefaultURL();
	}

	SOCmnStringList attributeList(getHTTPAttributeList());
	SOCmnListPosition pos;
	SOCmnString attrName, attrVal;
	SOCmnDateTime now;
	now.now();
	pos = attributeList.getStartPosition();

	while (pos)
	{
		attrVal = attributeList.getNext(pos, &attrName);
		szAttr.format(_T("%s: %s\r\n"), (LPCTSTR)attrName, (LPCTSTR)attrVal);
		szAttributes += szAttr;
	}

	attributeList.removeAll();

	if (bodyLen > 0)
	{
		szAttr.format(_T("Content-Length: %lu\r\n"), bodyLen);
		szAttributes += szAttr;
	}

	reqHeader.getBuffer(szAttributes.getLength() + 256);
	reqHeader.format(_T("%s %s HTTP/1.1\r\nUser-Agent: Softing OPC Toolkit\r\nHost: %s:%u\r\nDate: %s\r\n%s\r\n"),
					 SOCmnHTTPHandler::getHTTPMethodString(getHTTPMethod()), (LPCTSTR)url,
					 (LPCTSTR)con->getHostName(), con->getPort(), (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szAttributes);

	// set the data
	if (m_pReqData)
	{
		freeMemory(m_pReqData);
	}

	DWORD headerLen;
	m_pReqData = (BYTE*)reqHeader.cloneToCHAR(&headerLen);

	if (!m_pReqData)
	{
		m_reqSize = 0;
		return E_OUTOFMEMORY;
	}

	m_reqSize = headerLen + bodyLen;

	if (bodyLen)
	{
		m_pReqData = (BYTE*)reallocateMemory(m_pReqData, m_reqSize + 1);

		if (m_pReqData)
		{
			memcpy(m_pReqData + headerLen, pReqBodyA, bodyLen);
			m_pReqData[m_reqSize] = 0;
		}
		else
		{
			m_reqSize = 0;
			return E_OUTOFMEMORY;
		}
	}

	if (pReqBodyA)
	{
		freeMemory(pReqBodyA);
	}

	return S_OK;
}

void SOCltHTTPCall::getResponseBody(OUT DWORD* pResSize, OUT BYTE** ppResData)
{
	*pResSize = m_resSize;
	*ppResData = m_pResData;
}

void SOCltHTTPCall::setResponse(IN SOCltHTTPConnection* pCon, IN DWORD resSize, IN BYTE* pResData)
{
	if (m_pResData)
	{
		free(m_pResData);
	}

	m_resSize = 0;
	m_pResData = NULL;

	if (resSize > 0)
	{
		m_pResData = (BYTE*)malloc(resSize + 1);

		if (m_pResData)
		{
			m_resSize = resSize;
			memcpy(m_pResData, pResData, resSize + 1);
			m_pResData[resSize] = 0;
		}
	}

	m_httpStatusCode = pCon->getHTTPStatusCode();
	m_httpVersion = pCon->getHTTPVersion();
	m_attributeList.removeAll();
	SOCmnListPosition pos;
	SOCmnString szVal, szKey;
	SOCmnStringList attrList(pCon->getAttributeList());
	pos = attrList.getStartPosition();

	while (pos)
	{
		szVal = attrList.getNext(pos, &szKey);
		m_attributeList.add(szVal, szKey);
	}
}

void SOCltHTTPCall::onComplete(void)
{
}

void SOCltHTTPCall::setHTTPConnection(IN SOCltHTTPConnection* pConnection)
{
	m_httpConnection = pConnection;
	m_httpConnection.addRef();
}

SOCltHTTPConnection* SOCltHTTPCall::getHTTPConnection(void)
{
	m_httpConnection.addRef();
	return m_httpConnection;
}

#endif // SOFEATURE_SOAP

#endif // SOCLT
