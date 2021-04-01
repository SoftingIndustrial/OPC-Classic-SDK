#ifndef _SOSRVCOMOBJECT_H_
#define _SOSRVCOMOBJECT_H_

#ifdef SOFEATURE_DCOM

#include "SOCmnTrace.h"

#ifndef SOFLAG_DEMO_VERSION
#define SOSRV_CALL_CHECK_DEMO_DA
#define SOSRV_CALL_CHECK_DEMO_AE
#define SOSRV_ADVISE_CHECK_DEMO_DA (TRUE)
#define SOSRV_ADVISE_CHECK_DEMO_AE (TRUE)
#else
extern DWORD g_demoStamp;
#ifndef SOSRV_CALL_CHECK_DEMO_DA
#define SOSRV_CALL_CHECK_DEMO_DA                                                \
    if (getTimeSpan(g_demoStamp, GetTickCount()) > 5400000)                 \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s Runtime (90 minutes) of Demo Version elapsed"), l_ifFuncName)); \
        return E_ACCESSDENIED;                                              \
    }
#endif
#ifndef SOSRV_CALL_CHECK_DEMO_AE
#define SOSRV_CALL_CHECK_DEMO_AE                                                \
    if (getTimeSpan(g_demoStamp, GetTickCount()) > 5400000)                 \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s Runtime (90 minutes) of Demo Version elapsed"), l_ifFuncName)); \
        return E_ACCESSDENIED;                                              \
    }
#endif

#ifndef SOSRV_ADVISE_CHECK_DEMO_DA
#define SOSRV_ADVISE_CHECK_DEMO_DA  (getTimeSpan(g_demoStamp, GetTickCount()) < 900000)
#endif
#ifndef SOSRV_ADVISE_CHECK_DEMO_AE
#define SOSRV_ADVISE_CHECK_DEMO_AE  (getTimeSpan(g_demoStamp, GetTickCount()) < 900000)
#endif
#endif

#ifdef SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_BEGIN_INTERFACE_FUNCTION(specID, ifFuncName, callback)                \
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); \
    LPCTSTR l_ifFuncName = _T(#ifFuncName);                                 \
    HRESULT res = S_OK;                                                     \
    LONGLONG pcStart, pcEnd;            \
    SOCmnString szElapsedTime; \
    HANDLE hCurT; \
    int curTPrio; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        hCurT = GetCurrentThread(); \
        curTPrio = GetThreadPriority(hCurT); \
        SetThreadPriority(hCurT, THREAD_PRIORITY_HIGHEST); \
        _TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("%s >>"), l_ifFuncName));   \
        SOSRV_CALL_CHECK_DEMO_##specID                                              \
        _TRACE_START_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                     \
        HRESULT overwRes;                                                   \
        overwRes = m_me->onB##callback;                                     \
        if (overwRes != E_NOTIMPL)                                          \
        {                                                                   \
            _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                  \
            _TRACE((overwRes == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, overwRes)); \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
        overwRes = m_me->checkAuthorizationDCOM(l_ifFuncName);                                      \
        if (overwRes != S_OK)                                           \
        {                                                                   \
            _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                  \
            _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, overwRes)); \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
    }
#else
#define SOSRV_BEGIN_INTERFACE_FUNCTION(specID, ifFuncName, callback)                \
    HRESULT res = S_OK;                                                     \
    HANDLE hCurT; \
    int curTPrio; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        hCurT = GetCurrentThread(); \
        curTPrio = GetThreadPriority(hCurT); \
        SetThreadPriority(hCurT, THREAD_PRIORITY_HIGHEST); \
        SOSRV_CALL_CHECK_DEMO_##specID                                              \
        HRESULT overwRes;                                                   \
        overwRes = m_me->onB##callback;                                     \
        if (overwRes != E_NOTIMPL)                                          \
        {                                                                   \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
        overwRes = m_me->checkAuthorizationDCOM(l_ifFuncName);                                      \
        if (overwRes != S_OK)                                           \
        {                                                                   \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
    }
#endif // SOFEATURE_CMN_TRACE
#else // SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_BEGIN_INTERFACE_FUNCTION(specID, ifFuncName, callback)                \
    LPCTSTR l_ifFuncName = _T(#ifFuncName);                                 \
    HRESULT res = S_OK;                                                     \
    LONGLONG pcStart, pcEnd;            \
    SOCmnString szElapsedTime; \
    HANDLE hCurT; \
    int curTPrio; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        hCurT = GetCurrentThread(); \
        curTPrio = GetThreadPriority(hCurT); \
        SetThreadPriority(hCurT, THREAD_PRIORITY_HIGHEST); \
        _TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("%s >>"), l_ifFuncName));   \
        SOSRV_CALL_CHECK_DEMO_##specID                                              \
        _TRACE_START_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                     \
        HRESULT overwRes;                                                   \
        overwRes = m_me->checkAuthorizationDCOM(l_ifFuncName);                                      \
        if (overwRes != S_OK)                                           \
        {                                                                   \
            _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                  \
            _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, overwRes)); \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
    }
#else
#define SOSRV_BEGIN_INTERFACE_FUNCTION(specID, ifFuncName, callback)                \
    HRESULT res = S_OK;                                                     \
    HANDLE hCurT; \
    int curTPrio; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        hCurT = GetCurrentThread(); \
        curTPrio = GetThreadPriority(hCurT); \
        SetThreadPriority(hCurT, THREAD_PRIORITY_HIGHEST); \
        SOSRV_CALL_CHECK_DEMO_##specID                                              \
        HRESULT overwRes;                                                   \
        overwRes = m_me->checkAuthorizationDCOM(l_ifFuncName);                                      \
        if (overwRes != S_OK)                                           \
        {                                                                   \
            SetThreadPriority(hCurT, curTPrio); \
            return overwRes;                                                \
        }                                                                   \
    }
#endif // SOFEATURE_CMN_TRACE
#endif // SOFEATURE_COMIF_HOOKS

#ifndef SOOS_WINDOWS_CE
#ifdef SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
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
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(traceLevel, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
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
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#else
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        m_me->onE##callback;                                                \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = RPC_E_SERVERFAULT;                                        \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        m_me->onE##callback;                                                \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = RPC_E_SERVERFAULT;                                        \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#endif // SOFEATURE_CMN_TRACE
#else // SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
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
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(traceLevel, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
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
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#else
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = RPC_E_SERVERFAULT;                                        \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = RPC_E_SERVERFAULT;                                        \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = RPC_E_SERVERFAULT;                                            \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#endif // SOFEATURE_CMN_TRACE
#endif // SOFEATURE_COMIF_HOOKS
#else // SOOS_WINDOWS_CE
#ifdef SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(traceLevel, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#else // SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        m_me->onE##callback;                                                \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        m_me->onE##callback;                                                \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#endif // SOFEATURE_CMN_TRACE
#else // SOFEATURE_COMIF_HOOKS
#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(SUCCEEDED(res) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(traceLevel, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#else // SOFEATURE_CMN_TRACE
#define SOSRV_END_INTERFACE_FUNCTION(callback)                              \
    {                                                                       \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;

#define SOSRV_END_INTERFACE_FUNCTION_TL(callback, traceLevel)                               \
    {                                                                       \
    }                                                                       \
    }                                                                       \
    SetThreadPriority(hCurT, curTPrio); \
    return res;
#endif // SOFEATURE_CMN_TRACE
#endif // SOFEATURE_COMIF_HOOKS
#endif // SOOS_WINDOWS_CE

#ifdef SOFEATURE_COMIF_HOOKS
#define SOSRV_RETURN_INTERFACE_FUNCTION(callback)                           \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                         \
    }

#define SOSRV_RETURN_INTERFACE_FUNCTION_RES(result, callback)               \
    {                                                                       \
        res = result;                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                         \
    }
#else // SOFEATURE_COMIF_HOOKS
#define SOSRV_RETURN_INTERFACE_FUNCTION(callback)                           \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                         \
    }

#define SOSRV_RETURN_INTERFACE_FUNCTION_RES(result, callback)               \
    {                                                                       \
        res = result;                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                         \
    }
#endif // SOFEATURE_COMIF_HOOKS

#define SOSRV_BEGIN_PARAMS_CHECK()
#define SOSRV_OUT_PARAM_PTR(p) if (p) *p = NULL; else res = E_INVALIDARG;
#define SOSRV_OUT_PARAM(p, val) if (p) *p = val; else res = E_INVALIDARG;
#define SOSRV_IN_PARAM_PTR(p) if (!p) res = E_INVALIDARG;
#define SOSRV_IN_PARAM(expr) if (!(expr)) res = E_INVALIDARG;
#define SOSRV_IN_PARAM_RES(expr, hRes) if (!(expr)) res = hRes;
#define SOSRV_IN_PARAM_IF_PTR(p, expr) if (p) { if (!(expr)) res = E_INVALIDARG; }
#define SOSRV_CHECK(expr, result) if (SUCCEEDED(res)) { if (!(expr)) res = result; }
#ifdef SOFEATURE_COMIF_HOOKS
#define SOSRV_END_PARAMS_CHECK(callback)                                    \
    if (FAILED(res))                                                        \
    {                                                                       \
        m_me->onE##callback;                                                \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                 \
    }
#else // SOFEATURE_COMIF_HOOKS
#define SOSRV_END_PARAMS_CHECK(callback)                                    \
    if (FAILED(res))                                                        \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        SetThreadPriority(hCurT, curTPrio); \
        return res;                                                 \
    }
#endif // SOFEATURE_COMIF_HOOKS


//-----------------------------------------------------------------------------
// SOSrvComObject                                                             |
//-----------------------------------------------------------------------------

template <class Base> class SOSrvComObject : public Base
{
public:
	typedef Base _BaseClass;

	DECLARE_PROTECT_FINAL_CONSTRUCT();

	SOSrvComObject(void* = NULL)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_COM_OBJ_REF, (_T("create")));
		m_internalCnt = 0;
		m_tempCnt = 0;
		_Module.Lock();
	}

	~SOSrvComObject()
	{
		// set refcount to 1 to protect destruction
		m_dwRef = 1L;
		FinalRelease();
		_Module.Unlock();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_COM_OBJ_REF, (_T("free")));
	}

	// add external reference to object
	STDMETHOD_(ULONG, AddRef)()
	{
		ULONG l = InternalAddRef();
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_COM_OBJ_REF, (_T("AddRef %lu"), l));
		return l;
	}

	// add internal reference to object
	ULONG internalAddRef(void)
	{
		m_internalCnt++;
		AddRef();
		return m_internalCnt;
	}

	// add temporary reference to object
	ULONG tempAddRef(void)
	{
		LONG refCount = interlockedIncrement(&m_tempCnt);
		AddRef();
		return refCount;
	}

	// release external reference to object
	STDMETHOD_(ULONG, Release)()
	{
		lockMe();
		ULONG l = release(FALSE);
		return l;
	}

	// release internal reference to object
	ULONG internalRelease(OPTIONAL OUT ULONG* refCnt = NULL)
	{
		ULONG intCnt;
		ULONG rCnt;
		lockMe();

		if (m_internalCnt)
		{
			m_internalCnt--;
			intCnt = m_internalCnt;
			rCnt = release(TRUE);
		}
		else
		{
			intCnt = 0;
			rCnt = m_dwRef;
			unlockMe();
		}

		if (refCnt)
		{
			*refCnt = rCnt;
		}

		return intCnt;
	}

	// release temporary reference to object
	ULONG tempRelease(void)
	{
		LONG refCount;
		lockMe();

		if (m_tempCnt)
		{
			refCount = interlockedDecrement(&m_tempCnt);
			release(TRUE);
		}
		else
		{
			refCount = m_dwRef;
			unlockMe();
		}

		return (ULONG)refCount;
	}

	// release reference to object
	ULONG release(IN BOOL internal)
	{
		ULONG l = InternalRelease();
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_COM_OBJ_REF, (_T("Release %lu"), l));

		if ((!internal) && ((LONG)m_internalCnt == (m_dwRef - m_tempCnt)) && (l > 0))
		{
			unlockMe();

			if (lastExternalReleaseWithInternalRefs())
			{
				return 0;
			}

			return l;
		}

		if (l == 0)
		{
			disuniteMe();
			unlockMe();
			SOCMN_FREE_OBJECT_THIS
		}
		else
		{
			unlockMe();
		}

		return l;
	}

	// get the current reference count
	ULONG getRefCount(void)
	{
		return m_dwRef;
	}

	// query for interface
	STDMETHOD(QueryInterface)(IN REFIID iid, OUT void** ppvObject)
	{
		HRESULT res;
		res = queryInterface(iid, ppvObject);

		if (res == E_NOTIMPL)
		{
			res = _InternalQueryInterface(iid, ppvObject);
		}

		return res;
	}

	static HRESULT WINAPI CreateInstance(OUT SOSrvComObject<Base>** pp);

	LONG m_internalCnt; // internal reference count
	LONG m_tempCnt;     // temporary reference count for object protection
}; // SOSrvComObject


//-----------------------------------------------------------------------------
// CreateInstance
//
// - create a object instance
//
// returns:
//		HRESULT
//
template <class Base> HRESULT WINAPI SOSrvComObject<Base>::CreateInstance(
	OUT SOSrvComObject<Base>** pp)  // object
{
	_ASSERTION((pp != NULL), "Invalid out pararameter pp in SOSrvComObject<Base>::CreateInstance");
	HRESULT hRes = E_OUTOFMEMORY;
	SOSrvComObject<Base>* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SOSrvComObject<Base>())

	if (p != NULL)
	{
		p->SetVoid(NULL);
		p->InternalFinalConstructAddRef();
		hRes = p->FinalConstruct();
		p->InternalFinalConstructRelease();

		if (hRes != S_OK)
		{
			SOCMN_FREE_OBJECT(p)
		}
	}

	*pp = p;
	return hRes;
} // CreateInstance




//-----------------------------------------------------------------------------
// SOSrvComCreator                                                            |
//-----------------------------------------------------------------------------

template <class T1>
class SOSrvComCreator
{
public:
	static HRESULT WINAPI CreateInstance(void* pv, REFIID riid, LPVOID* ppv)
	{
		_ASSERTION(*ppv == NULL, "CreateInstance invalid OUT parameter");
		HRESULT hRes = E_OUTOFMEMORY;
		T1* p = NULL;
		ATLTRY(p = new T1(pv))

		if (p != NULL)
		{
			p->InternalFinalConstructAddRef();
			hRes = p->afterConstructorInit();           // do after constructor initialisation

			if (hRes == S_OK)
			{
				p->SetVoid(pv);
				hRes = p->FinalConstruct();

				if (hRes == S_OK)
				{
					hRes = p->QueryInterface(riid, ppv);
				}

				p->InternalFinalConstructRelease(); // ATL bug! do release AFTER query interface
			}

			if (hRes != S_OK)
			{
				delete p;
			}
		}

		return hRes;
	}
}; // SOSrvComCreator

#define DECLARE_OPCOBJECT_NOT_AGGREGATABLE(x) public:\
    typedef CComCreator2< SOSrvComCreator< SOSrvComObject< x > >, CComFailCreator<CLASS_E_NOAGGREGATION> > _CreatorClass;




//-----------------------------------------------------------------------------
// SOSrvComBase                                                               |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvComBase
{
public:
	SOSrvComBase();

	~SOSrvComBase();

	DWORD getOpcHandle() const;

	// do after constructor initianisation
	virtual HRESULT afterConstructorInit(void);

protected:

	// lock release and access from non COM object
	virtual void lockMe(void);
	virtual void unlockMe(void);

	// disconnect non COM object
	virtual void disuniteMe(void);

	// query interface
	virtual HRESULT queryInterface(IN REFIID iid, OUT void** object);

	// released last external reference with pending internal references
	virtual BOOL lastExternalReleaseWithInternalRefs(void);

	// last actions on the COM object
	virtual void lastCOMObjectActions(void);

	// set the security blanket for callback interfaces
	static HRESULT setProxyBlanket(IN IUnknown* pCBIf, IN DWORD srvAuthnLevel,
								   IN DWORD authnLevel, IN DWORD impLevel);

private:
	DWORD m_opcHandle;
}; // SOSrvComBase

#endif // SOFEATURE_DCOM

#endif
