#ifndef _SODASCOMSERVER_H_
#define _SODASCOMSERVER_H_

#ifdef SOFEATURE_DCOM

#include <opccomn.h>
#include "SOSrvComObject.h"
#include "SODaSComGroup.h"
#include "SODaSItemTag.h"
#include "SODaSTag.h"
#include "SOProxy.h"

// COM class id of OPC-Server object
extern CLSID g_clsidDA0;
extern CLSID g_clsidDA1;
extern CLSID g_clsidDA2;
extern CLSID g_clsidDA3;
extern CLSID g_clsidDA4;
extern CLSID g_clsidDA5;
extern CLSID g_clsidDA6;
extern CLSID g_clsidDA7;
extern CLSID g_clsidDA8;
extern CLSID g_clsidDA9;


#define SODASCOMSERVER_WAIT_FOR_NAMESPACE_STARTUP \
    if ((res = m_me->waitForNameSpaceStartup()) != S_OK) \
    { \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL); \
        _TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res; \
    } \
 
#define SODASCOMSERVER_WAIT_FOR_IO_STARTUP \
    if ((res = m_me->waitForIOStartup()) != S_OK) \
    { \
        _TRACE_STOP_TIME(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL); \
        _TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("%s (%s ms)[0x%X]"), l_ifFuncName, _TRACE_ELAPSED_TIME, res)); \
        return res; \
    } \
 

//-----------------------------------------------------------------------------
// SODaSComServer                                                             |
//-----------------------------------------------------------------------------

class SODaSServer;

class SODaSComServer :
	public IOPCCommon,
	public IOPCServer,
	public IOPCItemProperties,
	public IOPCBrowseServerAddressSpace,
	public IOPCServerPublicGroups,
	public IOPCBrowse,
	public IOPCItemIO,
	public IOPCSecurityPrivate,
	public IPersistFile,
	public ISoftingOPCWatch,
	public ISoftingOPCTrace,
	public IConnectionPointContainerImpl<SODaSComServer>,
	public IConnectionPointImpl<SODaSComServer, &IID_IOPCShutdown, CComDynamicUnkArray>,
	public CComObjectRoot,
	public SOSrvComBase
{
	friend SODaSServer;
public:
	SODaSComServer();
	~SODaSComServer();

	BEGIN_COM_MAP(SODaSComServer)
	COM_INTERFACE_ENTRY(IOPCCommon)
	COM_INTERFACE_ENTRY(IOPCServer)
	COM_INTERFACE_ENTRY(IOPCItemProperties)
	COM_INTERFACE_ENTRY(IOPCServerPublicGroups)
	COM_INTERFACE_ENTRY(IOPCBrowseServerAddressSpace)
	COM_INTERFACE_ENTRY(IOPCBrowse)
	COM_INTERFACE_ENTRY(IOPCItemIO)
	COM_INTERFACE_ENTRY(IOPCSecurityPrivate)
	COM_INTERFACE_ENTRY(IPersistFile)
#if _MSC_VER >= 1200
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#else
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
#endif
	COM_INTERFACE_ENTRY(ISoftingOPCWatch)
	COM_INTERFACE_ENTRY(ISoftingOPCTrace)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SODaSComServer)
	CONNECTION_POINT_ENTRY(IID_IOPCShutdown)
	END_CONNECTION_POINT_MAP()

	// DECLARE_REGISTRY_...
	static HRESULT WINAPI UpdateRegistry(BOOL doRegister)
	{
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
		return entry->updateRegistry(doRegister);
	}

	// IOPCCommon interface
	STDMETHODIMP SetLocaleID(IN LCID dwLcid);

	STDMETHODIMP GetLocaleID(OUT LCID* pdwLcid);

	STDMETHODIMP QueryAvailableLocaleIDs(OUT DWORD* pdwCount,
										 OUT LCID** pdwLcid);

	STDMETHODIMP GetErrorString(IN HRESULT dwError, OUT LPWSTR* ppString);

	STDMETHODIMP SetClientName(IN LPCWSTR szName);

	// IOPCServer interface
	STDMETHODIMP AddGroup(IN LPCWSTR szName, IN BOOL bActive,
						  IN DWORD dwRequestedUpdateRate, IN OPCHANDLE hClientGroup,
						  IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD dwLCID,
						  OUT OPCHANDLE* phServerGroup, OUT DWORD* pRevisedUpdateRate,
						  IN REFIID riid, OUT LPUNKNOWN* ppUnk);

	STDMETHODIMP GetErrorString(IN HRESULT dwError, IN LCID dwLocale,
								OUT LPWSTR* ppString);

	STDMETHODIMP GetGroupByName(IN LPCWSTR szName, IN REFIID riid,
								OUT LPUNKNOWN* ppUnk);

	STDMETHODIMP GetStatus(OUT OPCSERVERSTATUS** ppServerStatus);

	STDMETHODIMP RemoveGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce);

	STDMETHODIMP CreateGroupEnumerator(IN OPCENUMSCOPE dwScope,
									   IN REFIID riid, OUT LPUNKNOWN* ppUnk);

	// IOPCServerPublicGroups interface
	STDMETHODIMP GetPublicGroupByName(IN LPCWSTR szName, IN REFIID riid,
									  OUT LPUNKNOWN* ppUnk);

	STDMETHODIMP RemovePublicGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce);

	// IOPCItemProperties interface
	STDMETHODIMP QueryAvailableProperties(IN LPWSTR szItemID,
										  OUT DWORD* pdwCount, OUT DWORD** ppPropertyIDs,
										  OUT LPWSTR** ppDescriptions, OUT VARTYPE** ppvtDataTypes);

	STDMETHODIMP GetItemProperties(IN LPWSTR szItemID, IN DWORD dwCount,
								   IN DWORD* pdwPropertyIDs, OUT VARIANT** ppvData,
								   OUT HRESULT** ppErrors);

	STDMETHODIMP LookupItemIDs(IN LPWSTR szItemID, IN DWORD dwCount,
							   IN DWORD* pdwPropertyIDs, OUT LPWSTR** ppszNewItemIDs,
							   OUT HRESULT** ppErrors);

	// IOPCBrowseServerAddressSpace interface
	STDMETHODIMP QueryOrganization(OUT OPCNAMESPACETYPE* pNameSpaceType);

	STDMETHODIMP ChangeBrowsePosition(IN OPCBROWSEDIRECTION dwBrowseDirection,
									  IN LPCWSTR szString);

	STDMETHODIMP BrowseOPCItemIDs(IN OPCBROWSETYPE dwBrowseFilterType,
								  IN LPCWSTR szFilterCriteria, IN VARTYPE vtDataTypeFilter,
								  IN DWORD dwAccessRightsFilter, OUT LPENUMSTRING* ppIEnumString);

	STDMETHODIMP GetItemID(IN LPWSTR szItemDataID, OUT LPWSTR* szItemID);

	STDMETHODIMP BrowseAccessPaths(IN LPCWSTR szItemID,
								   OUT LPENUMSTRING* ppIEnumString);

	// IOPCBrowse interface
	STDMETHODIMP GetProperties(IN DWORD itemCount, IN LPWSTR* pItemIDs,
							   IN BOOL returnPropertyValues, IN DWORD propertyCount,
							   IN DWORD* pPropertyIDs, OUT OPCITEMPROPERTIES** ppItemProperties);

	STDMETHODIMP Browse(IN LPWSTR itemID, IN OUT LPWSTR* pContinuationPoint,
						IN DWORD maxElementsReturned, IN OPCBROWSEFILTER browseFilter,
						IN LPWSTR elementNameFilter, IN LPWSTR vendorFilter, IN BOOL returnAllProperties,
						IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs,
						OUT BOOL* pMoreElements, OUT DWORD* pCount, OUT OPCBROWSEELEMENT** ppBrowseElements);

	// IOPCItemIO interface
	STDMETHODIMP Read(IN DWORD count, IN LPCWSTR* pItemIDs, IN DWORD* pMaxAge,
					  OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps,
					  OUT HRESULT** ppErrors);

	STDMETHODIMP WriteVQT(IN DWORD count, IN LPCWSTR* pItemIDs,
						  IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);

	// IOPCSecurityPrivate interface
	STDMETHODIMP IsAvailablePriv(OUT BOOL* pbAvailable);

	STDMETHODIMP Logon(IN LPCWSTR szUserID, IN LPCWSTR szPassword);

	STDMETHODIMP Logoff(void);

	// IPersistFile interface
	STDMETHODIMP GetClassID(OUT CLSID* pClassID);

	STDMETHODIMP IsDirty(void);

	STDMETHODIMP Load(IN LPCOLESTR pszFileName, IN DWORD dwMode);

	STDMETHODIMP Save(IN LPCOLESTR pszFileName, IN BOOL fRemember);

	STDMETHODIMP SaveCompleted(IN LPCOLESTR pszFileName);

	STDMETHODIMP GetCurFile(OUT LPOLESTR* ppszFileName);

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

	// error strings
	HRESULT getErrorString(IN HRESULT dwError, IN BOOL useLocale,
						   IN LCID dwLocale, OUT LPWSTR* ppString);

	// client shutdown
	HRESULT sendShutdownRequest(IN LPCTSTR reason);
	BOOL connectedShutdownInterface(void);

	// check if connection to V2.0 OPC client is OK
	BOOL checkClientConnection(void);

	// object trace id
	LPCTSTR getObjTraceId(void);
	BOOL setObjTraceId(IN LPCTSTR traceId);

	// create COM group object
	HRESULT createOPCGroup(IN SODaSGroup* grp, IN REFIID riid,
						   OUT LPUNKNOWN* ppUnk, IN OPTIONAL BOOL doAddRef = TRUE);

	// do after constructor initianisation
	virtual HRESULT afterConstructorInit(void);

	void releaseAllAdvises(void);

protected:
	SOCmnString m_objectTraceId;    // object trace id string
	SODaSServer* m_me;              // generic server object
	SOCmnMutex m_advisesSync;       // synchronisation for advises array
	BYTE m_serverInstance;          // server class instance number

	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
	virtual BOOL lastExternalReleaseWithInternalRefs(void);

	// connection from server object
	virtual void lockMe(void);
	virtual void unlockMe(void);
	virtual void disuniteMe(void);
}; // SODaSComServer




//-----------------------------------------------------------------------------
// SODaSComServer Instances                                                   |
//-----------------------------------------------------------------------------

class SODaSComServer0 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer0, &g_clsidDA0>
{
public:
	SODaSComServer0();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer0)
};

class SODaSComServer1 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer1, &g_clsidDA1>
{
public:
	SODaSComServer1();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer1)
};

class SODaSComServer2 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer2, &g_clsidDA2>
{
public:
	SODaSComServer2();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer2)
};

class SODaSComServer3 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer3, &g_clsidDA3>
{
public:
	SODaSComServer3();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer3)
};

class SODaSComServer4 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer4, &g_clsidDA4>
{
public:
	SODaSComServer4();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer4)
};

class SODaSComServer5 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer5, &g_clsidDA5>
{
public:
	SODaSComServer5();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer5)
};

class SODaSComServer6 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer6, &g_clsidDA6>
{
public:
	SODaSComServer6();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer6)
};

class SODaSComServer7 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer7, &g_clsidDA7>
{
public:
	SODaSComServer7();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer7)
};

class SODaSComServer8 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer8, &g_clsidDA8>
{
public:
	SODaSComServer8();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer8)
};

class SODaSComServer9 :
	public SODaSComServer,
	public CComCoClass<SODaSComServer9, &g_clsidDA9>
{
public:
	SODaSComServer9();
	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SODaSComServer9)
};

#endif // SOFEATURE_DCOM

#endif

