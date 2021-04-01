#ifndef _SOSRVHTTP_H_
#define _SOSRVHTTP_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


class SOSrvHTTPHandler;
class SOSrvHTTPHandlerData;

#define SOSRVHTTP_FLAG_CONNECTION_CLOSE		  0x01
#define MAX_ACCEPTED_HTTP_HEADER_SIZE_BYTES   16384

class SOSRV_EXPORT SOSrvHTTPReceiver : public SOCmnObject
{
public:
	SOSrvHTTPReceiver(void);

	virtual void addClient(IN SOSrvHTTPHandlerData* pHTTPData);
	virtual void removeClient(IN SOSrvHTTPHandlerData* pHTTPData);

	virtual WORD handleHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);
	static void getResponseAttributes(IN BYTE flags, OUT SOCmnStringEx& szDefResAttrs);

	WORD getHTTPMethod(void)
	{
		return m_httpMethod;
	}
	void setHTTPMethod(WORD method)
	{
		m_httpMethod = method;
	}
	LPCTSTR getURL(void)
	{
		return m_httpURL;
	}
	void setURL(LPCTSTR url)
	{
		m_httpURL = url;
	}

protected:
	WORD m_httpMethod;
	SOCmnString m_httpURL;

	virtual ~SOSrvHTTPReceiver(void);
};

//-----------------------------------------------------------------------------
// SOSrvHTTPHandlerData                                                         |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvHTTPHandlerData : public SOCmnObject
{
public:
	SOSrvHTTPHandlerData(void);

	SOCmnPointer<SOCmnObject> m_object;
	SOCmnPointer<SOCmnSocketData> m_socket;
	DWORD m_clientId;
	BYTE* m_rcvBuffer;
	LONG m_rcvBufferSize;
	LONG m_rcvDataSize;
	SOCmnDateTime m_rcvTime;
	BYTE* m_sndBuffer;
	LONG m_sndBufferSize;
	LONG m_sndDataSize;
	BYTE m_resPending;
	BYTE m_flags;
	BYTE m_sndFlags;
	SOCmnSync m_lock;

protected:
	virtual ~SOSrvHTTPHandlerData(void);
};

//-----------------------------------------------------------------------------
// SOSrvHTTPHandler                                                           |
//-----------------------------------------------------------------------------

#include "SOCmnObject.h"
#include "SOCmnHttp.h"

class SOSrvHTTP;

class SOSRV_EXPORT SOSrvHTTPHandler : public SOCmnHTTPHandler
{
public:
	SOSrvHTTPHandler(void);

	virtual void setHTTPServer(SOSrvHTTP* pS);
	virtual SOSrvHTTP* getHTTPServer(void);

	virtual BOOL addClient(IN SOCmnObject* pSocket, IN DWORD clientId);
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason);
	virtual void checkClients(void);

	virtual void rcvData(IN SOSrvHTTPHandlerData* srvData, IN LONG size, IN BYTE* pData);
	virtual LONG sndData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData);

	virtual BOOL getHTTPErrorResponse(WORD errorCode, IN BYTE flags, IN LPCTSTR addHeader, LONG* pSize, BYTE** ppData);
	virtual BOOL initialize(void);

	void queueSndData(IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN LONG size, IN BYTE* pData);
	void queueSndData(IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN SOCmnStringEx& header, IN LONG bodyLength, IN BYTE* pBodyData);
	void queueSndData(IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN SOCmnStringEx& header, IN SOCmnStringEx& body);

	void sndHTTPErrorResponse(IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags, IN WORD errorCode, OPTIONAL IN LPCTSTR addHeader = NULL);

	SOCmnList<SOSrvHTTPHandlerData> *getServerList(void)
	{
		return &m_srvList;
	}
	SOCmnList<SOSrvHTTPHandlerData> *getDecodeList(void)
	{
		return &m_decodeList;
	}
	SOCmnList<SOSrvHTTPHandlerData> *getSndList(void)
	{
		return &m_sndList;
	}
	SOCmnList<SOSrvHTTPReceiver> *getReceiverList(void)
	{
		return &m_receiverList;
	}

	BOOL addHTTPReceiver(SOSrvHTTPReceiver* pRec)
	{
		return m_receiverList.add(pRec);
	}

	// parameter for the check routines HTTP server
	void setCheckParameter(IN DWORD clientCheckInterval, IN DWORD clientInactiveTime)
	{
		m_checkClientInterval = clientCheckInterval;
		m_clientInactiveTime = clientInactiveTime;
	}

protected:
	virtual ~SOSrvHTTPHandler(void);
	SOCmnPointer<SOSrvHTTP> m_serverHTTP;

	SOCmnList<SOSrvHTTPHandlerData> m_srvList;
	SOCmnThread m_workThread;
	SOCmnList<SOSrvHTTPHandlerData> m_decodeList;
	DWORD m_decodeEventId;
	SOCmnList<SOSrvHTTPHandlerData> m_sndList;
	DWORD m_sndEventId;
	SOCmnList<SOSrvHTTPReceiver> m_receiverList;
	DWORD m_checkClientInterval;
	DWORD m_clientInactiveTime;
	BYTE m_sendContinue;

	virtual DWORD workThread(void);
	static DWORD _stdcall workThreadProc(void* context);
}; // SOSrvHTTPHandler


#include "SOCmnSocket.h"

//-----------------------------------------------------------------------------
// SOSrvHTTP                                                                  |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvHTTP : public SOCmnSocketSrv
{
public:
	SOSrvHTTP(void);

	void setHTTPHandler(IN SOSrvHTTPHandler* hHTTP);
	SOSrvHTTPHandler* getHTTPHandler(void)
	{
		m_hHTTP.addRef();
		return m_hHTTP;
	}

protected:
	virtual ~SOSrvHTTP();

	SOCmnPointer<SOSrvHTTPHandler> m_hHTTP;

	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** pSocket);
	virtual void rcvData(IN SOCmnObject* pClient, IN LONG size, IN BYTE* pData);

public:
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason = SOCMNSOCKET_REQ_GRACEFUL_CLOSE);
}; // SOSrvHTTP

#pragma pack(pop)

#endif

#endif
