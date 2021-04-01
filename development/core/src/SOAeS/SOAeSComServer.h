#ifndef _SOAESCOMSERVER_H_
#define _SOAESCOMSERVER_H_

#ifdef SOFEATURE_DCOM

#include <opccomn.h>
#include <opc_ae.h>
#include "SOSrvComObject.h"
#include "SOProxy.h"

extern CLSID g_clsidAE0;
extern CLSID g_clsidAE1;
extern CLSID g_clsidAE2;
extern CLSID g_clsidAE3;
extern CLSID g_clsidAE4;
extern CLSID g_clsidAE5;
extern CLSID g_clsidAE6;
extern CLSID g_clsidAE7;
extern CLSID g_clsidAE8;
extern CLSID g_clsidAE9;


#define SOAESCOMSERVER_WAIT_FOR_AREASPACE_STARTUP \
    if ((res = m_me->waitForAreaSpaceStartup()) != S_OK) \
    { \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL); \
        _TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res; \
    } \
 
#define SOAESCOMSERVER_WAIT_FOR_EVENTSPACE_STARTUP \
    if ((res = m_me->waitForEventSpaceStartup()) != S_OK) \
    { \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL); \
        _TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res; \
    } \
 
#define SOAESCOMSERVER_WAIT_FOR_IO_STARTUP \
    if ((res = m_me->waitForIOStartup()) != S_OK) \
    { \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL); \
        _TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res; \
    } \
 

//-----------------------------------------------------------------------------
// SOAeSComServer                                                             |
//-----------------------------------------------------------------------------

class SOAeSServer;
class SOAeSComSubscription;
class SOAeSComAreaBrowser;

class SOAeSComServer :
	public IOPCCommon,
	public IOPCEventServer,
	public IOPCSecurityPrivate,
	public ISoftingOPCWatch,
	public ISoftingOPCTrace,
	public IConnectionPointContainerImpl<SOAeSComServer>,
	public IConnectionPointImpl<SOAeSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>,
	public CComObjectRoot,
	public SOSrvComBase
{
	friend SOAeSComSubscription;
	friend SOAeSComAreaBrowser;

public:
	SOAeSComServer();
	~SOAeSComServer();

	BEGIN_COM_MAP(SOAeSComServer)
	COM_INTERFACE_ENTRY(IOPCCommon)
	COM_INTERFACE_ENTRY(IOPCEventServer)
	COM_INTERFACE_ENTRY(IOPCSecurityPrivate)
#if _MSC_VER >= 1200
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#else
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
#endif
	COM_INTERFACE_ENTRY(ISoftingOPCWatch)
	COM_INTERFACE_ENTRY(ISoftingOPCTrace)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SOAeSComServer)
	CONNECTION_POINT_ENTRY(IID_IOPCShutdown)
	END_CONNECTION_POINT_MAP()

	// DECLARE_REGISTRY_...
	static HRESULT WINAPI UpdateRegistry(BOOL doRegister)
	{
		SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
		return entry->updateRegistry(doRegister);
	}

	// IOPCCommon interface
	STDMETHODIMP SetLocaleID(IN LCID dwLcid);

	STDMETHODIMP GetLocaleID(OUT LCID* pdwLcid);

	STDMETHODIMP QueryAvailableLocaleIDs(OUT DWORD* pdwCount,
										 OUT LCID** pdwLcid);

	STDMETHODIMP GetErrorString(IN HRESULT dwError, OUT LPWSTR* ppString);

	STDMETHODIMP SetClientName(IN LPCWSTR szName);

	// IOPCEventServer interface
	STDMETHODIMP GetStatus(OUT OPCEVENTSERVERSTATUS** eventServerStatus);

	STDMETHODIMP CreateEventSubscription(IN BOOL active, IN DWORD bufferTime,
										 IN DWORD maxSize, IN OPCHANDLE clientSubscriptionHandle,
										 IN REFIID riid, OUT LPUNKNOWN* unknown, OUT DWORD* revisedBufferTime,
										 OUT DWORD* revisedMaxSize);

	STDMETHODIMP QueryAvailableFilters(OUT DWORD* filterMask);

	STDMETHODIMP QueryEventCategories(IN DWORD eventType, OUT DWORD* count,
									  OUT DWORD** eventCategories, OUT LPWSTR** eventCategoryDescs);

	STDMETHODIMP QueryConditionNames(IN DWORD eventCategory, OUT DWORD* count,
									 OUT LPWSTR** conditionNames);

	STDMETHODIMP QuerySubConditionNames(IN LPWSTR conditionName, OUT DWORD* count,
										OUT LPWSTR** subConditionNames);

	STDMETHODIMP QuerySourceConditions(IN LPWSTR source, OUT DWORD* count,
									   OUT LPWSTR** conditionNames);

	STDMETHODIMP QueryEventAttributes(IN DWORD eventCategory, OUT DWORD* count,
									  OUT DWORD** attrIDs, OUT LPWSTR** attrDescs, OUT VARTYPE** attrTypes);

	STDMETHODIMP TranslateToItemIDs(IN LPWSTR source, IN DWORD eventCategory,
									IN LPWSTR conditionName, IN LPWSTR subconditionName, IN DWORD count,
									IN DWORD* assocAttrIDs, OUT LPWSTR** attrItemIDs,
									OUT LPWSTR** nodeNames, OUT CLSID** clsids);

	STDMETHODIMP GetConditionState(IN LPWSTR source, IN LPWSTR conditionName,
								   IN DWORD numEventAttrs, IN DWORD* attributeIDs,
								   OUT OPCCONDITIONSTATE** conditionState);

	STDMETHODIMP EnableConditionByArea(IN DWORD numAreas, IN LPWSTR* areas);

	STDMETHODIMP EnableConditionBySource(IN DWORD numSources, IN LPWSTR* sources);

	STDMETHODIMP DisableConditionByArea(IN DWORD numAreas, IN LPWSTR* areas);

	STDMETHODIMP DisableConditionBySource(IN DWORD numSources, IN LPWSTR* sources);

	STDMETHODIMP AckCondition(IN DWORD count, IN LPWSTR acknowledgerID,
							  IN LPWSTR comment, IN LPWSTR* source, IN LPWSTR* conditionName,
							  IN FILETIME* activeTime, IN DWORD* cookie, OUT HRESULT** errors);

	STDMETHODIMP CreateAreaBrowser(IN REFIID riid, OUT LPUNKNOWN* unknown);

	// IOPCSecurityPrivate interface
	STDMETHODIMP IsAvailablePriv(OUT BOOL* pbAvailable);

	STDMETHODIMP Logon(IN LPCWSTR szUserID, IN LPCWSTR szPassword);

	STDMETHODIMP Logoff(void);

	// IConnectionPoint
	STDMETHODIMP Advise(IN IUnknown* unknown, OUT DWORD* cookie);

	STDMETHODIMP Unadvise(IN DWORD cookie);

	// ISoftingOPCWatch
	STDMETHODIMP GetChildren(IN LPCWSTR parent, IN BOOL withAttributes,
							 OUT LPWSTR* children);

	STDMETHODIMP CollectChanges(IN LPCWSTR objects, IN DWORD doCollect);

	STDMETHODIMP GetChanges(OUT LPWSTR* changes);

	STDMETHODIMP GetStatistics(OUT LPWSTR* statistics);

	STDMETHODIMP GetObject(IN LPCWSTR objectDescription, OUT LPWSTR* object);

	// ISoftingOPCTrace
	STDMETHODIMP SetTraceOptions(IN LPCWSTR options);

	STDMETHODIMP GetTraceOptions(OUT LPWSTR* options);

	STDMETHODIMP SaveTraceOptions(void);

	STDMETHODIMP GetTrace(OUT LPWSTR* trace);

	// client shutdown
	HRESULT sendShutdownRequest(IN LPCTSTR reason);
	BOOL connectedShutdownInterface(void);

	// check if connection to V2.0 OPC client is OK
	BOOL checkClientConnection(void);

	// object trace id
	LPCTSTR getObjTraceId(void);
	BOOL setObjTraceId(IN LPCTSTR traceId);

	// do after constructor initianisation
	virtual HRESULT afterConstructorInit(void);

	void releaseAllAdvises(void);

protected:
	SOCmnString m_objectTraceId;    // object trace id string
	SOAeSServer* m_me;              // server object
	SOCmnMutex m_advisesSync;       // synchronisation for advises array
	BYTE m_serverInstance;          // server class instance number

	HRESULT enableConditions(IN BOOL enable, IN BOOL areaOrSource, IN DWORD num,
							 IN LPWSTR* names);

	BOOL fillConditionState(IN SOAeSCondition* cond, IN DWORD numEventAttrs,
							IN DWORD* attributeIDs, OUT OPCCONDITIONSTATE* conditionState);

	// query interface
	virtual HRESULT queryInterface(REFIID iid, void** object);

	// connection from server object
	virtual void lockMe(void);
	virtual void unlockMe(void);
	virtual void disuniteMe(void);
}; // SOAeSComServer




//-----------------------------------------------------------------------------
// SOAeSComServer Instances                                                   |
//-----------------------------------------------------------------------------

class SOAeSComServer0 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer0, &g_clsidAE0>
{
public:
	SOAeSComServer0();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer)
};

class SOAeSComServer1 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer1, &g_clsidAE1>
{
public:
	SOAeSComServer1();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer1)
};

class SOAeSComServer2 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer2, &g_clsidAE2>
{
public:
	SOAeSComServer2();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer2)
};

class SOAeSComServer3 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer3, &g_clsidAE3>
{
public:
	SOAeSComServer3();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer3)
};

class SOAeSComServer4 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer4, &g_clsidAE4>
{
public:
	SOAeSComServer4();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer4)
};

class SOAeSComServer5 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer5, &g_clsidAE5>
{
public:
	SOAeSComServer5();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer5)
};

class SOAeSComServer6 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer6, &g_clsidAE6>
{
public:
	SOAeSComServer6();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer6)
};

class SOAeSComServer7 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer7, &g_clsidAE7>
{
public:
	SOAeSComServer7();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer7)
};

class SOAeSComServer8 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer8, &g_clsidAE8>
{
public:
	SOAeSComServer8();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer8)
};

class SOAeSComServer9 :
	public SOAeSComServer,
	public CComCoClass<SOAeSComServer9, &g_clsidAE9>
{
public:
	SOAeSComServer9();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComServer9)
};

#endif // SOFEATURE_DCOM

#endif
