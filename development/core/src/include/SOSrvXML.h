#ifndef _SOSRVXML_H_
#define _SOSRVXML_H_

#ifdef SOFEATURE_XML_METHOD

#pragma pack(push, 4)


#include "SOCmnXML.h"
#include "SOSrvHttp.h"

//-----------------------------------------------------------------------------
// SOSrvXmlMethod                                                             |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvXmlMethod : public SOCmnXmlMethod
{
public:
	SOSrvXmlMethod(void);
#ifdef SOFEATURE_SOAP
	SOSrvXmlMethod(IN BYTE xmlMethodType, IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);
#endif

#ifdef SOFEATURE_SOAP
	SOSrvHTTPHandler* getHTTPHandler(void)
	{
		m_httpHandler.addRef();
		return m_httpHandler;
	}
	SOSrvHTTPHandlerData* getHTTPHandlerData(void)
	{
		m_httpHandlerData.addRef();
		return m_httpHandlerData;
	}
#endif

protected:
	virtual ~SOSrvXmlMethod(void);

#ifdef SOFEATURE_SOAP
	SOCmnPointer<SOSrvHTTPHandler> m_httpHandler;
	SOCmnPointer<SOSrvHTTPHandlerData> m_httpHandlerData;
#endif
}; // SOSrvXmlMethod


#ifdef SOFEATURE_CMN_TRACE
#define SOSRV_BEGIN_SOAP_FUNCTION(ifFuncName)               \
    HRESULT res = S_OK; \
    LPCTSTR l_ifFuncName = _T(#ifFuncName);                                 \
    LONGLONG pcStart, pcEnd;            \
    SOCmnString szElapsedTime; \
    SOCMN_TRY {                                                                 \
    {                                                                       \
        _TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s >>"), l_ifFuncName)); \
        _TRACE_START_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                     \
    }
#else
#define SOSRV_BEGIN_SOAP_FUNCTION(ifFuncName)               \
    HRESULT res = S_OK; \
    SOCMN_TRY {
#endif // SOFEATURE_CMN_TRACE

#ifndef SOOS_WINDOWS_CE

#define SOSRV_END_SOAP_FUNCTION                             \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACEO((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    SOCMN_CATCH(SOCmnException e)                                               \
    {                                                                       \
        _TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s: Exception: %s"), l_ifFuncName, (LPCTSTR)e.m_description)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        if (e.m_id == SOCMNEXCEPTION_ID_OUTOFMEMORY)                            \
            res = E_OUTOFMEMORY;                                            \
        else                                                                \
            res = E_FAIL;                                       \
    }                                                                       \
    SOCMN_CATCH(...)                                                                \
    {                                                                       \
        _TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s: Exception"), l_ifFuncName)); \
        _ASSERTION(FALSE, "Exception in OPC Function");                     \
        res = E_FAIL;                                           \
    }                                                                       \
    return res;

#else

#define SOSRV_END_SOAP_FUNCTION                             \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACEO((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
    }                                                                       \
    }                                                                       \
    return res;
#endif

#define SOSRV_RETURN_SOAP_FUNCTION                          \
    {                                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACEO((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res;                                                         \
    }

#define SOSRV_RETURN_SOAP_FUNCTION_RES(result)              \
    {                                                                       \
        res = result;                                                       \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL);                      \
        _TRACEO((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, pServer, (_T("%s (%s ms)[0x%lX]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res;                                                         \
    }

#pragma pack(pop)

#endif // SOFEATURE_XML_METHOD

#endif
