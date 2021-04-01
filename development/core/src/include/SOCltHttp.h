#ifndef _SOCLTHTTP_H_
#define _SOCLTHTTP_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


#include "SOCmnSocket.h"
#include "SOCmnHttp.h"

#define SOCLTHTTP_CALL_DISCONNECT  5000
#define SOCLTHTTP_CALL_TIMEOUT     5001
#define SOCLTHTTP_CALL_CANTSEND    5002
#define SOCLTHTTP_CALL_CANCELED    5100

//-----------------------------------------------------------------------------
// SOCltHTTPCall                                                              |
//-----------------------------------------------------------------------------

class SOCltHTTP;
class SOCltHTTPConnection;

class SOCLT_EXPORT SOCltHTTPCall : public SOCmnObject
{
	friend class SOCltHTTP;
	friend class SOCltHTTPConnection;
public:
	enum state
	{
		idle = 0,
		active,
		finished
	};

	SOCltHTTPCall(void);

	virtual LPCTSTR getObjTraceId(void);

	virtual HRESULT createRequestData(IN SOCmnStringEx* pReqBody = NULL);
	virtual void getRequestData(OUT DWORD* pReqSize, OUT BYTE** ppReqData);
	virtual void setRequestData(IN DWORD reqSize, IN BYTE* pReqData);

	void setHTTPMethod(WORD IN method)
	{
		m_httpMethod = method;
	}
	WORD getHTTPMethod(void)
	{
		return m_httpMethod;
	}
	void setURL(IN LPCTSTR url);
	LPCTSTR getURL(void);

	virtual void getResponseBody(OUT DWORD* pResSize, OUT BYTE** ppResData);
	virtual void setResponse(IN SOCltHTTPConnection* pCon, IN DWORD resSize, IN BYTE* pResData);

	virtual void resetData(IN BOOL onlyResponse);

	WORD getHTTPVersion(void)
	{
		return m_httpVersion;
	}
	WORD getHTTPStatusCode(void)
	{
		return m_httpStatusCode;
	}
	void setHTTPStatusCode(WORD s)
	{
		m_httpStatusCode = s;
	}
	SOCmnStringList* getHTTPAttributeList(void)
	{
		return &m_attributeList;
	}

	BOOL isSynchronous(void)
	{
		return m_synchronous;
	}
	BOOL isAsynchronous(void)
	{
		return !m_synchronous;
	}

	DWORD getTimeout(void);
	void setTimeout(DWORD dw);

	DWORD getActivationTime(void)
	{
		return m_activationTime;
	}

	void setHTTPConnection(IN SOCltHTTPConnection* pConnection);
	SOCltHTTPConnection* getHTTPConnection(void);

	SOCmnEvent* getSyncInvokeRcvDataEvent(void)
	{
		return &m_syncInvokeRcvDataEvent;
	}

protected:
	SOCmnPointer<SOCltHTTPConnection> m_httpConnection;
	BYTE* m_pReqData;
	DWORD m_reqSize;
	WORD m_httpMethod;
	SOCmnString m_url;
	BYTE* m_pResData;
	DWORD m_resSize;
	WORD m_httpVersion;
	WORD m_httpStatusCode;
	SOCmnStringList m_attributeList;
	DWORD m_timeout;
	DWORD m_activationTime;
	BYTE m_synchronous;
	SOCmnEvent m_syncInvokeRcvDataEvent;

	virtual ~SOCltHTTPCall(void);

	void setSynchronous(BOOL sync)
	{
		m_synchronous = (BYTE)sync;
	}

	virtual void onComplete(void);
}; // SOCltHTTPCall



//-----------------------------------------------------------------------------
// SOCltHTTP                                                                  |
//-----------------------------------------------------------------------------

class SOCltHTTPConnection;

class SOCLT_EXPORT SOCltHTTP : public SOCmnSocketClient
{
public:
	SOCltHTTP(void);

	virtual BOOL addConnection(IN SOCltHTTPConnection* pCon);
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason = SOCMNSOCKET_REQ_GRACEFUL_CLOSE);

	void addCompleteCall(SOCltHTTPCall* pCall);
	SOCmnList<SOCltHTTPCall> *getCompleteList(void)
	{
		return &m_completeList;
	}

	void addActiveCall(SOCltHTTPCall* pCall);
	SOCmnList<SOCltHTTPCall> *getActiveList(void)
	{
		return &m_activeList;
	}

	virtual BOOL start(void);
	virtual BOOL stop(void);

	virtual void timeOut(IN SOCmnObject* pSocket);

protected:
	static DWORD _stdcall completeAndTimeoutThreadProc(void* context);
	SOCmnThread m_completeAndTimeoutThread;
	DWORD m_completeEventID;
	DWORD m_activeEventID;

	SOCmnList<SOCltHTTPCall> m_completeList;
	SOCmnList<SOCltHTTPCall> m_activeList;

	virtual ~SOCltHTTP();

	virtual DWORD completeAndTimeoutThread(void);

	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN OUT SOCmnObject** ppSocket);
	virtual void connectedClient(IN SOCmnObject* pSocket);
	virtual void rcvData(IN SOCmnObject* pClient, IN LONG size, IN BYTE* pData);
}; // SOCltHTTP



//-----------------------------------------------------------------------------
// SOCltHTTPConnection                                                        |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltHTTPConnection : public SOCmnHTTPHandler
{
	friend class SOCltHTTP;
public:
	enum state
	{
		idle = 0,
		waitForResponse = 1,
		waitForClose = 2
	};

	SOCltHTTPConnection(void);

	void close(LONG reason);

	virtual LPCTSTR getObjTraceId(void);

	virtual HRESULT invoke(IN SOCltHTTPCall* pCall, IN BOOL synchronous);

	virtual void disconnect(void);
	virtual void finishCall(IN SOCltHTTPCall* pCall, BOOL restartNext = TRUE);

	void setHTTPClient(IN SOCltHTTP* pClient)
	{
		m_httpClient = pClient;
		m_httpClient.addRef();
	}
	SOCltHTTP* getHTTPClient(void)
	{
		m_httpClient.addRef();
		return m_httpClient;
	}

	SOCltHTTPCall* getHTTPCall(void)
	{
		m_call.addRef();
		return m_call;
	}

	void setHostName(IN LPCTSTR hostName)
	{
		m_hostName = hostName;
	}
	LPCTSTR getHostName(void)
	{
		return (LPCTSTR)m_hostName;
	}

	void setPort(IN WORD port)
	{
		m_port = port;
	}
	WORD getPort(void)
	{
		return m_port;
	}

	void setDefaultURL(IN LPCTSTR url)
	{
		m_defURL = url;
	}
	LPCTSTR getDefaultURL(void)
	{
		return (LPCTSTR)m_defURL;
	}

	BOOL isConnected(void)
	{
		return m_connected;
	}

	SOCmnSync* getCallLock(void)
	{
		return m_callList.getSyncObject();
	}

	SOCltServer* getServer(void)
	{
		m_server.addRef();
		return m_server;
	}
	void setServer(SOCltServer* s)
	{
		m_server = s;
		m_server.addRef();
	}

	SOCmnObject* getSocket(void)
	{
		m_socket.addRef();
		return m_socket;
	}

protected:
	SOCmnPointer<SOCltServer> m_server;
	SOCmnPointer<SOCltHTTP> m_httpClient;
	SOCmnPointer<SOCmnObject> m_socket;   // socket object
	SOCmnPointer<SOCltHTTPCall> m_call;
	SOCmnList<SOCltHTTPCall> m_callList;
	SOCmnEvent m_invokeStartCallEvent;
	SOCmnEvent m_isConnectedEvent;
	SOCmnString m_hostName;
	SOCmnString m_defURL;
	BYTE* m_pResData;
	DWORD m_resSize;
	DWORD m_resBufferSize;
	WORD m_port;
	BYTE m_connected;

	virtual ~SOCltHTTPConnection(void);
	virtual void rcvData(LONG size, BYTE* pData);

}; // SOCltHTTPConnection

#pragma pack(pop)

#endif

#endif
