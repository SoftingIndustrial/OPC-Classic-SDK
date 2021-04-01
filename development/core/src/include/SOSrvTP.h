#ifndef _SOSRVTP_H_
#define _SOSRVTP_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#ifndef SOFLAG_DEMO_VERSION
#ifndef SOSRV_TP_CHECK_DEMO
#define SOSRV_TP_CHECK_DEMO
#endif
#ifndef SOSRV_TP_ADVISE_CHECK_DEMO
#define SOSRV_TP_ADVISE_CHECK_DEMO (TRUE)
#endif
#else
extern DWORD g_demoStamp;
#ifndef SOSRV_TP_CHECK_DEMO
#define SOSRV_TP_CHECK_DEMO                                             \
    if (getTimeSpan(g_demoStamp, GetTickCount()) > 5400000)                 \
    {                                                                       \
        sendReply(pCall, interfaceId, functionId, E_ACCESSDENIED); \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Runtime (90 minutes) of Demo Version elapsed"))); \
        return FALSE;                                               \
    }
#endif
#ifndef SOSRV_TP_ADVISE_CHECK_DEMO
#define SOSRV_TP_ADVISE_CHECK_DEMO  (getTimeSpan(g_demoStamp, GetTickCount()) < 5400000)
#endif
#endif

#define SODASTPSERVER_WAIT_FOR_NAMESPACE_STARTUP \
    res = server->waitForNameSpaceStartup(); \
 
#define SODASTPSERVER_WAIT_FOR_IO_STARTUP \
    res = server->waitForNameSpaceStartup(); \
 
#define SOAESTPSERVER_WAIT_FOR_AREASPACE_STARTUP \
    res = server->waitForAreaSpaceStartup();

#define SOAESTPSERVER_WAIT_FOR_IO_STARTUP \
    res = server->waitForIOStartup();


#define SOSRV_BEGIN_TP_FUNCTION(interfaceIdName, functionIdName, ifFuncName)                \
    SOCmnPointer<SOSrvTPConnection> tpCon = (SOSrvTPConnection *)pCall->getTPConnection(); \
    WORD interfaceId = interfaceIdName;  \
    BYTE functionId = functionIdName; \
    LPCTSTR l_ifFuncName = _T(#ifFuncName);                                 \
    HRESULT res = S_OK;                                                     \
    LONGLONG pcStart, pcEnd;            \
    SOCmnString szElapsedTime; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("%s >>"), l_ifFuncName));   \
        _TRACE_START_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                     \
    }

#ifndef SOOS_WINDOWS_CE
#define SOSRV_END_TP_FUNCTION                               \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception: %s"), l_ifFuncName, (LPCTSTR)e.m_description)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = RPC_E_SERVERFAULT;                                        \
        tpCon->sendReply(pCall, interfaceId, functionId, res); \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
        tpCon->sendReply(pCall, interfaceId, functionId, res); \
    }                                                                       \
    return;
#else
#define SOSRV_END_TP_FUNCTION                               \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    return;
#endif


#define SOSRV_RETURN_TP_FUNCTION_RES(result)                \
    {                                                                       \
        res = result;                                                       \
        tpCon->sendReply(pCall, interfaceId, functionId, res); \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return;                                                         \
    }

class SOSrvTPConnection;
class SOSRV_EXPORT SOSrvTP : public SOCmnSocketSrv
{
public:
	SOSrvTP();

protected:
	~SOSrvTP();
	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, OUT SOCmnObject** pSocket);
	virtual void rcvData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData);

	SOCmnList<SOCmnTPReceiver> m_receiverList;

	BOOL m_bClientAuthenticationEnabled;

	SOCmnList<SOCmnTPCredentials> m_PermissibleCredentialsList;

public:
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason = SOCMNSOCKET_REQ_GRACEFUL_CLOSE);

	SOCmnList<SOCmnTPReceiver> *getTPReceiverList(void)
	{
		return &m_receiverList;
	}
	BOOL addTPReceiver(SOCmnTPReceiver* pRec)
	{
		return m_receiverList.add(pRec);
	}

	BOOL isClientAuthenticationEnabled();
	void enableClientAuthentication(BOOL bEnable);

	SOCmnList<SOCmnTPCredentials> *getPermissibleCredentialsList();
	BOOL setPermissibleCredentialsList(SOCmnList<SOCmnTPCredentials> *pCredentialsList);
};

class SOSrvServer;
class SOSRV_EXPORT SOSrvTPConnection : public SOCmnTPConnection
{
public:
	SOSrvTPConnection();
	~SOSrvTPConnection();

	virtual void close(void);

	virtual void addRequestToWorkQueue(SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);
	virtual SOCmnList<SOCmnTPReceiver> *getReceiverList(void);

	SOSrvServer* getServer(void)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_sync);
		m_server.addRef();
		return m_server;
	}
	void setServer(SOSrvServer* s)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_sync);
		m_server = s;
		m_server.addRef();
	}

protected:
	u_short establishSession(u_short unFlags, const TP_SESSION_CONTEXT& sessionContext,
							 SOCmnTPCredentials* pClientCredentials, TP_SESSION_ID& sessionId);

	SOCmnPointer<SOSrvServer> m_server;

	virtual bool handleInitMessage();
	virtual bool handlePingMessage();
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SOSRVTP_H_
