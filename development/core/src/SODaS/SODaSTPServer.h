//-----------------------------------------------------------------------------
#ifndef _SODASTPSERVER_H_
#define _SODASTPSERVER_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#ifndef SOFLAG_DEMO_VERSION
#define SOSRV_TP_CHECK_DEMO
#define SOSRV_TP_ADVISE_CHECK_DEMO (TRUE)
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

#include "SOSrvTP.h"

class SODAS_EXPORT SODaSTPServer : public SOCmnTPReceiver
{
public:
	SODaSTPServer(void);

	virtual void closeConnection(IN SOCmnTPConnection* pCon);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);
	void getStatus(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void setClientName(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void browse(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void getProperties(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void addGroup(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void removeGroup(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void read(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void write(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);

protected:
	virtual ~SODaSTPServer(void);
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SODASTPSERVER_H_
