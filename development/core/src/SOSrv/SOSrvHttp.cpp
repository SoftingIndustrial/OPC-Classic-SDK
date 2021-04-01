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
//  Filename    : SOSrvHTTP.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : HTTP server related classes                                 |
//                - SOSrvHTTP: HTTP server object                             |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_SOAP

#include "SOSrvHttp.h"

#ifndef SOFLAG_DEMO_VERSION
#define SOSRV_HTTP_CHECK_DEMO(a) a = SOCMNHTTP_CODEI_NOT_HANDLED;
#else
extern DWORD g_demoStamp;
#ifndef SOSRV_HTTP_CHECK_DEMO
#define SOSRV_HTTP_CHECK_DEMO(a)                                                \
    if (getTimeSpan(g_demoStamp, GetTickCount()) > 5400000)                 \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Runtime (90 minutes) of Demo Version elapsed"))); \
        a =  SOCMNHTTP_CODE_FORBIDDEN;                                              \
        sndHTTPErrorResponse(srvData, srvData->m_flags, a); \
    } \
    else \
    { \
        a = SOCMNHTTP_CODEI_NOT_HANDLED; \
    }
#endif
#endif



//-----------------------------------------------------------------------------
// SOSrvHTTP                                                                  |
//-----------------------------------------------------------------------------

SOSrvHTTP::SOSrvHTTP(void)
{
}

SOSrvHTTP::~SOSrvHTTP(void)
{
}

void SOSrvHTTP::setHTTPHandler(IN SOSrvHTTPHandler* hHTTP)
{
	if (m_hHTTP.isNotNull())
	{
		m_hHTTP->setHTTPServer(NULL);
	}

	m_hHTTP = hHTTP;
	m_hHTTP.addRef();

	if (m_hHTTP.isNotNull())
	{
		m_hHTTP->setHTTPServer(this);
	}
}


BOOL SOSrvHTTP::addClient(
	IN SOCKET hSocket,
	IN struct sockaddr_in* pSockAddr,
	IN SOCmnObject** /* pSocket */)
{
	SOCmnPointer<SOCmnObject> socket;
	BOOL ret = SOCmnSocketSrv::addClient(hSocket, pSockAddr, &socket);

	if ((ret) && (m_hHTTP.isNotNull()))
	{
		ret = m_hHTTP->addClient(socket, (DWORD)hSocket);
	}

	return ret;
}

BOOL SOSrvHTTP::removeClient(SOCmnObject* pSocket, IN LONG reason)
{
	if (!pSocket)
	{
		return FALSE;
	}

	BOOL ret = TRUE;

	if ((m_hHTTP.isNotNull()))
	{
		ret = m_hHTTP->removeClient(pSocket, reason);
	}

	ret = SOCmnSocketSrv::removeClient(pSocket, reason) && ret;
	return ret;
}

void SOSrvHTTP::rcvData(IN SOCmnObject* pSocket, LONG size, BYTE* pData)
{
	if (!pSocket)
	{
		return;
	}

	SOCmnPointer<SOSrvHTTPHandlerData> srvData = (SOSrvHTTPHandlerData*)getClientHandle(pSocket);

	if ((m_hHTTP.isNotNull()) && (srvData.isNotNull()))
	{
		m_hHTTP->rcvData(srvData, size, pData);
	}
}


//-----------------------------------------------------------------------------
// SOSrvHTTPHandlerData                                                         |
//-----------------------------------------------------------------------------

SOSrvHTTPHandlerData::SOSrvHTTPHandlerData(void)
{
	m_rcvBuffer = (BYTE*)malloc(4096);

	if (m_rcvBuffer)
	{
		m_rcvBufferSize = 4096;
	}
	else
	{
		m_rcvBufferSize = 0;
	}

	m_rcvDataSize = 0;
	m_sndBuffer = (BYTE*)malloc(4096);

	if (m_sndBuffer)
	{
		m_sndBufferSize = 4096;
	}
	else
	{
		m_sndBufferSize = 0;
	}

	m_sndDataSize = 0;
	m_clientId = 0;
	m_resPending = FALSE;
	m_flags = 0;
	m_sndFlags = 0;
	m_rcvTime.now();
}

SOSrvHTTPHandlerData::~SOSrvHTTPHandlerData(void)
{
	if (m_rcvBuffer)
	{
		free(m_rcvBuffer);
	}

	if (m_sndBuffer)
	{
		free(m_sndBuffer);
	}

	m_object.release();
	m_socket.release();
}

//-----------------------------------------------------------------------------
// SOSrvHTTPHandler                                                           |
//-----------------------------------------------------------------------------

SOSrvHTTPHandler::SOSrvHTTPHandler(void)
	: SOCmnHTTPHandler()
{
	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED;
	m_receiverList.create(&cfg);
	m_checkClientInterval = 30000; // 30 seconds
	m_clientInactiveTime = 180000; // 3 minutes
	m_sendContinue = FALSE;
}

SOSrvHTTPHandler::~SOSrvHTTPHandler(void)
{
	m_workThread.stop(10000);
	m_srvList.destroy();
	m_decodeList.destroy();
	m_sndList.destroy();
	m_receiverList.destroy();
}

void SOSrvHTTPHandler::setHTTPServer(SOSrvHTTP* pS)
{
	m_serverHTTP = pS;
	m_serverHTTP.addRef();
}

SOSrvHTTP* SOSrvHTTPHandler::getHTTPServer(void)
{
	m_serverHTTP.addRef();
	return m_serverHTTP;
}

BOOL SOSrvHTTPHandler::initialize(void)
{
	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	m_srvList.create(&cfg);
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	m_decodeList.create(&cfg);
	m_decodeEventId = m_workThread.createEvent();
	m_sndList.create(&cfg);
	m_sndEventId = m_workThread.createEvent();
	return m_workThread.start(workThreadProc, (void*)this);
}

BOOL SOSrvHTTPHandler::addClient(IN SOCmnObject* pSocket, IN DWORD clientId)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("Add client %lu 0x%8.8lX"), clientId, (DWORD)pSocket));
	SOCmnList<SOSrvHTTPHandlerData> srvList(getServerList());
	SOCmnPointer<SOSrvHTTPHandlerData> srvData;
	BOOL ret;
	SOCMN_ALLOCATE_OBJECT_C(srvData, SOSrvHTTPHandlerData, (SOSrvHTTPHandlerData*))
	srvData->m_clientId = clientId;
	srvData->m_socket = (SOCmnSocketData*)pSocket;
	srvData->m_socket.addRef();
	ret = srvList.add(srvData, pSocket->getOpcHandle());
	srvList.autoUnlock();
	m_serverHTTP->setClientHandle(pSocket, srvData);
	SOCmnList<SOSrvHTTPReceiver> receiverList(getReceiverList());
	SOCmnListPosition posRec;
	SOSrvHTTPReceiver* pRec;
	posRec = receiverList.getStartPosition();

	while (posRec)
	{
		pRec = receiverList.getNext(posRec);
		pRec->addClient(srvData);
	}

	receiverList.autoUnlock();
	return ret;
}

BOOL SOSrvHTTPHandler::removeClient(IN SOCmnObject* pSocket, LONG reason)
{
	SOCmnList<SOSrvHTTPHandlerData> srvList(getServerList());
	SOCmnPointer<SOSrvHTTPHandlerData> srvData = srvList.findKey(pSocket->getOpcHandle());
	srvList.removeKey(pSocket->getOpcHandle());
	srvList.autoUnlock();
	m_serverHTTP->setClientHandle(pSocket, NULL);

	if (srvData.isNotNull())
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("Remove client %lu 0x%8.8lX (reason=%ld)"), srvData->m_clientId, (DWORD)(SOCmnObject*)srvData->m_socket, reason));
		SOCmnList<SOSrvHTTPReceiver> receiverList(getReceiverList());
		SOCmnListPosition posRec;
		SOSrvHTTPReceiver* pRec;
		posRec = receiverList.getStartPosition();

		while (posRec)
		{
			pRec = receiverList.getNext(posRec);
			pRec->removeClient(srvData);
		}

		srvData->m_clientId = 0;
		srvData.release(); // should free srvData
		return TRUE;
	}

	return FALSE;
}

void SOSrvHTTPHandler::checkClients(void)
{
	_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Checking (each %lums) for clients inactive more than %lums."), m_checkClientInterval, m_clientInactiveTime));
	SOCmnList<SOSrvHTTPHandlerData> srvList(getServerList());
	SOSrvHTTPHandlerData* srvData;
	SOCmnListPosition pos;
	SOCmnDateTime now;
	ULONGLONG nowMS, rcvMS;
	SOCmnList<SOSrvHTTPHandlerData> removeList;
	now.now();
	nowMS = now.getMicroseconds();
	pos = srvList.getStartPosition();

	while (pos)
	{
		srvData = srvList.getNext(pos);
		rcvMS = srvData->m_rcvTime.getMicroseconds();

		if (nowMS >= rcvMS)
		{
			DWORD inactiveTime = (DWORD)((nowMS - rcvMS) / 1000);

			if ((inactiveTime > m_clientInactiveTime) &&
				(srvData->m_resPending == FALSE))  // no response pending
			{
				// add entry to remove list
				if (!removeList.isInit())
				{
					removeList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
				}

				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("    Marking socket %lu (inactive for %lums) for disconnection."), srvData->m_socket->m_socket, inactiveTime));
				removeList.add(srvData);
			}
		}
		else if (rcvMS - nowMS > 1000000)
		{
			// last rcv time more than a second in the future
			srvData->m_rcvTime.now();
		}
	}

	srvList.autoUnlock();

	if (removeList.isInit())
	{
		// close the sockets
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("    Now closing %lu sockets."), removeList.GetCount()));
		pos = removeList.getStartPosition();

		while (pos)
		{
			srvData = removeList.getNext(pos);
			m_serverHTTP->removeClient(srvData->m_socket);
		}

		removeList.destroy();
	}
}


void SOSrvHTTPHandler::rcvData(IN SOSrvHTTPHandlerData* srvData, IN LONG size, IN BYTE* pData)
{
	SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);
	srvData->m_rcvTime.now();

	// check buffer size
	if (srvData->m_rcvDataSize + size + 1 > srvData->m_rcvBufferSize)
	{
		srvData->m_rcvBufferSize = __max(srvData->m_rcvBufferSize + 4096, srvData->m_rcvDataSize + size + 1);
		srvData->m_rcvBuffer = (BYTE*)realloc(srvData->m_rcvBuffer, srvData->m_rcvBufferSize);

		if (!srvData->m_rcvBuffer)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Out of memory - alloc:%lu"), srvData->m_rcvBufferSize));
			return;
		}
	}

	// copy new data
	if (srvData->m_rcvDataSize == 0)
	{
		memcpy(srvData->m_rcvBuffer, pData, size);
		srvData->m_rcvDataSize = size + 1;
	}
	else
	{
		memcpy(srvData->m_rcvBuffer - 1 + srvData->m_rcvDataSize, pData, size);
		srvData->m_rcvDataSize += size;
	}

	srvData->m_rcvBuffer[srvData->m_rcvDataSize - 1] = '\0';
	lock.unlock();
	// signal decode thread
	SOCmnList<SOSrvHTTPHandlerData> decodeList(getDecodeList());
	decodeList.add(srvData);
	decodeList.autoUnlock();
	m_workThread.getEvents()->signal(m_decodeEventId);
}

void SOSrvHTTPHandler::queueSndData(IN SOSrvHTTPHandlerData* srvData, IN BYTE flags, IN LONG size, IN BYTE* pData)
{
	SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);

	// check buffer size
	if (srvData->m_sndDataSize + size > srvData->m_sndBufferSize)
	{
		srvData->m_sndBufferSize = __max(srvData->m_sndBufferSize + 4096, srvData->m_sndDataSize + size);
		srvData->m_sndBuffer = (BYTE*)realloc(srvData->m_sndBuffer, srvData->m_sndBufferSize);

		if (!srvData->m_sndBuffer)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Out of memory - alloc:%lu"), srvData->m_sndBufferSize));
			return;
		}
	}

	// copy new data
	memcpy(srvData->m_sndBuffer + srvData->m_sndDataSize, pData, size);
	srvData->m_sndDataSize += size;
	srvData->m_sndFlags = flags;
	lock.unlock();
	// signal work thread
	SOCmnList<SOSrvHTTPHandlerData> sndList(getSndList());
	sndList.add(srvData);
	sndList.autoUnlock();
	m_workThread.getEvents()->signal(m_sndEventId);
}

void SOSrvHTTPHandler::queueSndData(IN SOSrvHTTPHandlerData* srvData, IN BYTE flags, IN SOCmnStringEx& header, IN LONG sizeB, IN BYTE* pBodyData)
{
	USES_CONVERSION;
	SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);
	LONG sizeH = header.getLength();
	LONG size = sizeH + sizeB;

	// check buffer size
	if (srvData->m_sndDataSize + size > srvData->m_sndBufferSize)
	{
		srvData->m_sndBufferSize = __max(srvData->m_sndBufferSize + 4096, srvData->m_sndDataSize + size);
		srvData->m_sndBuffer = (BYTE*)realloc(srvData->m_sndBuffer, srvData->m_sndBufferSize);

		if (!srvData->m_sndBuffer)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Out of memory - alloc:%lu"), srvData->m_sndBufferSize));
			return;
		}
	}

	// copy new data
	memcpy(srvData->m_sndBuffer + srvData->m_sndDataSize, T2CA((LPCTSTR)header), sizeH);
	memcpy(srvData->m_sndBuffer + srvData->m_sndDataSize + sizeH, pBodyData, sizeB);
	srvData->m_sndDataSize += size;
	srvData->m_sndFlags = flags;
	lock.unlock();
	// signal work thread
	SOCmnList<SOSrvHTTPHandlerData> sndList(getSndList());
	sndList.add(srvData);
	sndList.autoUnlock();
	m_workThread.getEvents()->signal(m_sndEventId);
}


void SOSrvHTTPHandler::queueSndData(IN SOSrvHTTPHandlerData* srvData, IN BYTE flags, IN SOCmnStringEx& header, IN SOCmnStringEx& body)
{
	SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);
	DWORD sizeH = 0;
	DWORD sizeB = 0;
	BYTE* pByteHeader = header.cloneToUTF8(&sizeH);
	BYTE* pByteBody = body.cloneToUTF8(&sizeB);
	LONG size = sizeH + sizeB;

	// check buffer size
	if (srvData->m_sndDataSize + size > srvData->m_sndBufferSize)
	{
		srvData->m_sndBufferSize = __max(srvData->m_sndBufferSize + 4096, srvData->m_sndDataSize + size);
		srvData->m_sndBuffer = (BYTE*)realloc(srvData->m_sndBuffer, srvData->m_sndBufferSize);

		if (!srvData->m_sndBuffer)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("Out of memory - alloc:%lu"), srvData->m_sndBufferSize));

			if (pByteHeader != NULL)
			{
				free(pByteHeader);
			}

			if (pByteBody != NULL)
			{
				free(pByteBody);
			}

			return;
		}
	}

	// copy new data
	memcpy(srvData->m_sndBuffer + srvData->m_sndDataSize, pByteHeader, sizeH);
	memcpy(srvData->m_sndBuffer + srvData->m_sndDataSize + sizeH, pByteBody, sizeB);
	srvData->m_sndDataSize += size;
	srvData->m_sndFlags = flags;
	lock.unlock();
	// signal work thread
	SOCmnList<SOSrvHTTPHandlerData> sndList(getSndList());
	sndList.add(srvData);
	sndList.autoUnlock();
	m_workThread.getEvents()->signal(m_sndEventId);

	if (pByteHeader != NULL)
	{
		free(pByteHeader);
	}

	if (pByteBody != NULL)
	{
		free(pByteBody);
	}
}


DWORD SOSrvHTTPHandler::workThread(void)
{
	BYTE* pBodyS;
	DWORD bodyLen;
	DWORD discardTo;
	WORD resCode = SOCMNHTTP_CODEI_NOT_HANDLED;
	DWORD waitRes;
	DWORD nowTC;
	DWORD checkClientsTC = GetTickCount();
	DWORD waitTime = m_checkClientInterval;

	while (TRUE)
	{
		waitRes = m_workThread.waitForEvents(waitTime);

		if (waitRes == m_decodeEventId)
		{
			// decode
			SOCmnList<SOSrvHTTPHandlerData> decodeList(getDecodeList());
			BOOL end = FALSE;
			SOCmnList<SOSrvHTTPReceiver> receiverList(getReceiverList());
			SOCmnListPosition posRec;
			SOSrvHTTPReceiver* pRec;
			SOCmnString recURL;
			SOCmnString expect;
			SOCmnString connection;
			SOCmnString reqURL;

			for (;;)
			{
				SOCmnPointer<SOSrvHTTPHandlerData> srvData;
				srvData = decodeList.pop();
				decodeList.autoUnlock();

				if (!srvData)
				{
					break;
				}

				{
					SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);

					if (srvData->m_rcvDataSize <= 0)
					{
						continue;
					}

					end = FALSE;

					do
					{
						// check if header is complete
						// note: if a NUL char is in the data stream the strstr
						// functions cannot find the end of header mark
						// so stop waiting for more header if a NUL byte was received.
						// NUL bytes in the content section are okay.
						if ((strstr((CHAR*)srvData->m_rcvBuffer, "\n\n") == NULL) &&
							(strstr((CHAR*)srvData->m_rcvBuffer, "\n\r\n") == NULL) &&
							(strlen((CHAR*)srvData->m_rcvBuffer) == (DWORD)srvData->m_rcvDataSize - 1))
						{
							break;    // do while(!end)
						}

						// decode message
						pBodyS = NULL;
						discardTo = decodeHTTPRequestHeader(srvData->m_rcvDataSize, srvData->m_rcvBuffer, &pBodyS);

						// the pBodyS is set only if the HTTP-header contents has been completely parsed
						// if the header is still incomplete and the size exceeds 16k than close connection and reject the request
						if ( (!pBodyS) && (srvData->m_rcvDataSize >= MAX_ACCEPTED_HTTP_HEADER_SIZE_BYTES) )
						{
							srvData->m_flags |= SOSRVHTTP_FLAG_CONNECTION_CLOSE;
							sndHTTPErrorResponse(srvData, srvData->m_flags, SOCMNHTTP_CODE_BAD_REQUEST);
							break;    // do while(!end)
						}

						// check for Expect: 100-continue
						if ((m_attributeList.findKey(_T("expect"), &expect)) && (!m_sendContinue))
						{
							expect.makeLower();

							if (expect == _T("100-continue"))
							{
								sndHTTPErrorResponse(srvData, srvData->m_flags, SOCMNHTTP_CODE_CONTINUE);
								m_sendContinue = TRUE;
							}
						}

						// check for Connection: close
						if (m_attributeList.findKey(_T("connection"), &connection))
						{
							connection.makeLower();

							if (connection == _T("close"))
							{
								srvData->m_flags |= SOSRVHTTP_FLAG_CONNECTION_CLOSE;
							}
						}

						// check for HTTP 1.0 -> close connection
						if (m_httpVersion == 0x0100)
						{
							srvData->m_flags |= SOSRVHTTP_FLAG_CONNECTION_CLOSE;
						}

						if ((bodyLen = getContentsLength()) != 0)
						{
							// check if complete contents arrived
							if ((DWORD)srvData->m_rcvDataSize < discardTo + bodyLen + 1) // m_rcvDataSize is always terminated with '\0' -> + 1!
							{
								break;
							}

							discardTo += bodyLen + 1;
						}

						// start handle message
						srvData->m_resPending = TRUE;
						resCode = SOCMNHTTP_CODEI_NOT_HANDLED;
						// remove the HTTP query string ? from the URL for handler check
						reqURL = m_httpURL;
						int queryStartIdx;

						if ((queryStartIdx = reqURL.find(_T('?'))) != -1)
						{
							reqURL = reqURL.left(queryStartIdx);
						}

						posRec = receiverList.getStartPosition();
						SOSRV_HTTP_CHECK_DEMO(resCode);

						while ((posRec) && (resCode == SOCMNHTTP_CODEI_NOT_HANDLED))
						{
							pRec = receiverList.getNext(posRec);
							recURL = pRec->getURL();

							if (((pRec->getHTTPMethod() & m_httpMethod) == m_httpMethod) &&  // receiver for this method
								((recURL.isEmpty()) || (_tcsicmp(recURL, reqURL) == 0)))     // URL case insensitive
							{
								// handleHTTPRequest works asynchronous!
								// it only start the request handling if this is more than building the result string
								if ((resCode = pRec->handleHTTPRequest(this, srvData, bodyLen, pBodyS)) != SOCMNHTTP_CODEI_NOT_HANDLED)
								{
									break;
								}
							}
						}

						receiverList.autoUnlock();

						if (resCode == SOCMNHTTP_CODEI_NOT_HANDLED)
						{
							// request not handled
							if ((m_httpMethod == 0) || (!((m_httpVersion == MAKEWORD(1, 1)) || (m_httpVersion == MAKEWORD(0, 1)))))
							{
								resCode =  SOCMNHTTP_CODE_BAD_REQUEST;    // no HTTP request
							}
							else
							{
								resCode = SOCMNHTTP_CODE_NOT_FOUND;    // in most reasons URL not found
							}

							// send error response
							sndHTTPErrorResponse(srvData, srvData->m_flags, resCode);
						}

						// discard message
						m_sendContinue = FALSE;
						srvData->m_flags = 0;

						if (discardTo >= (DWORD)srvData->m_rcvDataSize - 1)
						{
							srvData->m_rcvDataSize = 0;
							end = TRUE;
						}
						else
						{
							if (discardTo > 0)
							{
								memmove(srvData->m_rcvBuffer, srvData->m_rcvBuffer + discardTo, srvData->m_rcvDataSize - discardTo);
								srvData->m_rcvDataSize -= discardTo;
							}
						}
					}
					while (!end);
				} // srvData lock
			} // for(;;)
		}
		else if (waitRes == m_sndEventId)
		{
			// send
			SOCmnList<SOSrvHTTPHandlerData> sndList(getSndList());

			for (;;)
			{
				SOCmnPointer<SOSrvHTTPHandlerData> srvData;
				srvData = sndList.pop();
				sndList.autoUnlock();

				if (!srvData)
				{
					break;
				}

				{
					SOCmnSingleLock<SOCmnSync> lock(&srvData->m_lock);
					sndData(srvData->m_socket, srvData->m_sndDataSize, srvData->m_sndBuffer);
					srvData->m_sndDataSize = 0;
					srvData->m_resPending = FALSE;

					if (srvData->m_sndFlags & SOSRVHTTP_FLAG_CONNECTION_CLOSE)
					{
						m_serverHTTP->shutdownClient(srvData->m_socket);
					}
				}
			}
		}
		else if (waitRes == m_workThread.getStopEventId())
		{
			return 0;    // received end event
		}

		nowTC = GetTickCount();
		waitTime = getTimeSpan(checkClientsTC, nowTC);

		if (waitTime >= m_checkClientInterval)
		{
			// every 10 seconds
			checkClientsTC = GetTickCount();
			waitTime = m_checkClientInterval;
			// check the socket connections from the clients and close the unused if neccessary
			checkClients();
		}
		else
		{
			waitTime = m_checkClientInterval - waitTime;
		}
	}

	return 0;
}

DWORD _stdcall SOSrvHTTPHandler::workThreadProc(void* context)
{
	SOSrvHTTPHandler* pT = (SOSrvHTTPHandler*)context;
	return pT->workThread();
}


LONG SOSrvHTTPHandler::sndData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData)
{
	if (!pSocket)
	{
		return -1;
	}

	LONG res = -1;

	if (m_serverHTTP.isNotNull())
	{
		res = m_serverHTTP->sndData(pSocket, size, pData);
	}

	return res;
}


static CHAR s_errHTTP100[2][20] =
{
	"Continue",
	"Switching Protocols"
};

static CHAR s_errHTTP300[8][20] =
{
	"Multiple Choices",
	"Moved Permanently",
	"Found",
	"See Other",
	"Not Modified",
	"Use Proxy",
	" ",
	"Temporary Redirect"
};

static CHAR s_errHTTP400[18][41] =
{
	"Bad Request",
	"Unauthorized",
	"Payment Required",
	"Forbidden",
	"Not Found",
	"Method Not Allowed",
	"Not Acceptable",
	"Proxy Authentication Required",
	"Request Timeout",
	"Conflict",
	"Gone",
	"Length Required",
	"Precondition Failed",
	"Request Entity Too Large",
	"Request URI Too Long",
	"Unsupported Media Type",
	"Requested Range Not Satisfiable",
	"Expectiation Failed"
};

static CHAR s_errHTTP500[6][41] =
{
	"Internal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Service Unavailable",
	"Gateway Timeout",
	"HTTP Version Not Supported"
};

BOOL SOSrvHTTPHandler::getHTTPErrorResponse(WORD errorCode, IN BYTE flags, IN LPCTSTR addHeader, LONG* pSize, BYTE** ppData)
{
	USES_CONVERSION;
	*pSize = 0;
	*ppData = NULL;
	SOCmnStringEx szDefResAttrs;
	SOSrvHTTPReceiver::getResponseAttributes(flags, szDefResAttrs);

	if (addHeader)
	{
		szDefResAttrs += addHeader;
	}

	if ((errorCode == 100) || (errorCode == 101))
	{
		*ppData = (BYTE*)malloc(130);
		sprintf((CHAR*)*ppData, "HTTP/1.1 %d %s\r\n\r\n", errorCode, s_errHTTP100[errorCode - 100]);
		*pSize = (int)strlen((CHAR*)*ppData);
	}
	else if ((errorCode >= 400) && (errorCode <= 417))
	{
		*ppData = (BYTE*)malloc(130 + szDefResAttrs.getLength());
		sprintf((CHAR*)*ppData, "HTTP/1.1 %d %s\r\n%sContent-type: text/plain\r\nContent-length: %d\r\n\r\n%d %s", errorCode, s_errHTTP400[errorCode - 400], T2CA((LPCTSTR)szDefResAttrs), strlen(s_errHTTP400[errorCode - 400]) + 4, errorCode, s_errHTTP400[errorCode - 400]);
		*pSize = (int)strlen((CHAR*)*ppData);
	}
	else if ((errorCode >= 300) && (errorCode <= 307))
	{
		*ppData = (BYTE*)malloc(130 + szDefResAttrs.getLength());
		sprintf((CHAR*)*ppData, "HTTP/1.1 %d %s\r\n%sContent-type: text/plain\r\nContent-length: %d\r\n\r\n%d %s", errorCode, s_errHTTP300[errorCode - 300], T2CA((LPCTSTR)szDefResAttrs), strlen(s_errHTTP300[errorCode - 300]) + 4, errorCode, s_errHTTP300[errorCode - 300]);
		*pSize = (int)strlen((CHAR*)*ppData);
	}
	else if ((errorCode >= 500) && (errorCode <= 505))
	{
		*ppData = (BYTE*)malloc(130 + szDefResAttrs.getLength());
		sprintf((CHAR*)*ppData, "HTTP/1.1 %d %s\r\n%sContent-type: text/plain\r\nContent-length: %d\r\n\r\n%d %s", errorCode, s_errHTTP500[errorCode - 500], T2CA((LPCTSTR)szDefResAttrs), strlen(s_errHTTP500[errorCode - 500]) + 4, errorCode, s_errHTTP500[errorCode - 500]);
		*pSize = (int)strlen((CHAR*)*ppData);
	}

	return ((*pSize) > 0);
}

void SOSrvHTTPHandler::sndHTTPErrorResponse(IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN WORD errorCode, OPTIONAL IN LPCTSTR addHeader)
{
	LONG size;
	BYTE* pData;

	if (getHTTPErrorResponse(errorCode, flags, addHeader, &size, &pData))
	{
		queueSndData(pSrvData, flags, size, pData);
	}

	free(pData);
}



SOSrvHTTPReceiver::SOSrvHTTPReceiver(void)
{}

SOSrvHTTPReceiver::~SOSrvHTTPReceiver(void)
{}

WORD SOSrvHTTPReceiver::handleHTTPRequest(
	IN SOSrvHTTPHandler* /* pHandler */,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN DWORD /* lenBody */,
	IN BYTE* /* pBody */)
{
	return SOCMNHTTP_CODEI_NOT_HANDLED; // not handled
}

void SOSrvHTTPReceiver::removeClient(
	IN SOSrvHTTPHandlerData* pHTTPData)
{
	pHTTPData->m_object.release();
}

void SOSrvHTTPReceiver::addClient(
	IN SOSrvHTTPHandlerData* /* pHTTPData */)
{
}

void SOSrvHTTPReceiver::getResponseAttributes(IN BYTE flags, OUT SOCmnStringEx& szDefResAttrs)
{
	if (flags & SOSRVHTTP_FLAG_CONNECTION_CLOSE)
	{
		szDefResAttrs = _T("Connection: close\r\n");
	}
}

#endif // SOFEATURE_SOAP

#endif // SOSRV
