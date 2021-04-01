#ifndef _SOSRVENTRY_H_
#define _SOSRVENTRY_H_

#include <opcda.h>
#include "SOCmnObject.h"
#include "SOSrvWatch.h"
#ifdef SOFEATURE_SOAP
#include "SOSrvHttp.h"
#endif
#ifdef SOFEATURE_TUNNEL
#include "SOSrvTP.h"
#endif

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOSrvEventHandler                                                          |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvEventHandler : public SOCmnObject
{
public:
	SOSrvEventHandler();

	// released last COM object of the server
	virtual void onReleasedLastOPCObject(void);

	// released a COM object of the server
	virtual void onReleasedOPCObject(void);
}; // SOSrvEventHandler

#define GenericEventHandler SOSrvEventHandler


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SOSrvShutdownData                                                          |
//-----------------------------------------------------------------------------

class SOSrvServer;

class SOSrvShutdownData : public SOCmnData
{
public:
	SOCmnPointer<SOSrvServer> server; // server object
	SOCmnString reason;               // shutdown reason
	HANDLE event;                     // notification event for shutdown completion
}; // SOSrvShutdownData

#endif



//-----------------------------------------------------------------------------
// SOSrvConverter                                                             |
//-----------------------------------------------------------------------------

#define SOSRVCONVERTER_LCID_DEFAULT 0xFFFF

class SOSRV_EXPORT SOSrvConverter : public SOCmnObject
{
public:
	SOSrvConverter();

	virtual LPCTSTR bool2string(IN SOCmnString& buffer, IN BOOL b, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR variant2string(IN SOCmnString& buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR vartype2string(IN SOCmnString& buffer, IN VARTYPE vt, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR accessRights2string(IN SOCmnString& buffer, IN DWORD accessRight, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR quality2string(IN SOCmnString& buffer, IN WORD quality, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
#ifdef SOOS_WINDOWS
	virtual LPCTSTR filetime2string(IN SOCmnString& buffer, IN LPFILETIME filetime, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
#endif
	virtual LPCTSTR datetime2string(IN SOCmnString& buffer, IN SOCmnDateTime& filetime, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
#ifdef SOFEATURE_DCOM
	virtual LPCTSTR IID2string(IN SOCmnString& buffer, IN REFIID riid);
	virtual LPCTSTR CLSID2string(IN SOCmnString& buffer, IN REFCLSID rclsid);
#endif
	virtual LPCTSTR LCID2string(IN SOCmnString& buffer, IN LCID lcid, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR serverState2string(IN SOCmnString& buffer, IN DWORD serverState, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR publicState2string(IN SOCmnString& buffer, IN BOOL pub, IN BOOL noTempl, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR euType2string(IN SOCmnString& buffer, IN OPCEUTYPE euType, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR filters2string(IN SOCmnString& buffer, IN DWORD avFilter, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR eventTypeFilter2string(IN SOCmnString& buffer, IN DWORD eventType, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR conditionState2string(IN SOCmnString& buffer, IN BYTE conditionState, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
#ifdef SOFEATURE_DCOM
	virtual LPCTSTR authentificationLevel2string(IN SOCmnString& buffer, IN DWORD authLevel, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR impersonationLevel2string(IN SOCmnString& buffer, IN DWORD impLevel, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
	virtual LPCTSTR account2string(IN SOCmnString& buffer, IN LPCTSTR identity, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);
#endif
	virtual LPCTSTR result2string(IN SOCmnString& buffer, IN HRESULT res, IN DWORD specId, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);

	LCID getLCID(void) const
	{
		return m_lcid;
	}
	virtual BOOL setLCID(IN LCID lcid);

protected:
	LCID m_lcid;
}; // SOSrvConverter




//-----------------------------------------------------------------------------
// SOSrvCreator                                                               |
//-----------------------------------------------------------------------------

class SOSrvWatch;

class SOSRV_EXPORT SOSrvCreator : public SOCmnObject
{
public:
	SOSrvCreator();
#ifdef SOFEATURE_WATCH
	virtual SOSrvWatch* createWatch(void);
#endif
	virtual SOSrvConverter* createConverter(void);
}; // SOSrvCreator




//-----------------------------------------------------------------------------
// SOSrvEntry                                                                 |
//-----------------------------------------------------------------------------

#ifndef SOOS_WINDOWS_CE
#define SOSRVENTRY_COM_OBJMAP_ENTRY(clsid, class) &clsid, class::UpdateRegistry, class::_ClassFactoryCreatorClass::CreateInstance, class::_CreatorClass::CreateInstance, class::GetObjectDescription, class::GetCategoryMap, class::ObjectMain
#else
#define SOSRVENTRY_COM_OBJMAP_ENTRY(clsid, class) &clsid, class::UpdateRegistry, class::_ClassFactoryCreatorClass::CreateInstance, class::_CreatorClass::CreateInstance, class::GetObjectDescription, NULL, class::ObjectMain
#endif

#define SOSRVENTRY_CATEGORY_DA10    0x01
#define SOSRVENTRY_CATEGORY_DA20    0x02
#define SOSRVENTRY_CATEGORY_DA30    0x04
#define SOSRVENTRY_CATEGORY_AE10    0x10

#define SOSRVENTRY_FLAG_ATL_MODULE      0x01
#define SOSRVENTRY_FLAG_MAIN_THREAD     0x02

#define SOSRVENTRY_INIT_COM                             0x01
#define SOSRVENTRY_INIT_SECURITY                    0x02
#define SOSRVENTRY_INIT_HTTP                            0x04
#define SOSRVENTRY_INIT_TP                              0x08
#define SOSRVENTRY_INIT_HAVETO_UNINIT_COM   0x80

#define OTS_ELEMENTDATA_NONE          0x000 // 0   << keys for set/getStringBasedObjectData
#define OTS_ELEMENTDATA_NAME          0x001 // 1
#define OTS_ELEMENTDATA_ITEMID        0x002 // 2
#define OTS_ELEMENTDATA_ACCESSRIGHTS  0x004 // 3
#define OTS_ELEMENTDATA_IOMODE        0x008 // 4
#define OTS_ELEMENTDATA_HASCHILDREN   0x010 // 5
#define OTS_ELEMENTDATA_ISBROWSABLE   0x020 // 6
#define OTS_ELEMENTDATA_USERDATA      0x040 // 7
#define OTS_ELEMENTDATA_DATATYPE      0x080 // 8
#define OTS_ELEMENTDATA_ELEMENTTYPE   0x100 // 9
#define OTS_ELEMENTDATA_ALL           0x1FF // 511
#define OTS_ELEMENTDATA_ANY           0xFFF // 4095

#define OTS_ELEMENTDATA_DISABLED_QW   0xFFFFFFFFFFFFFFFFULL
#define OTS_ELEMENTDATA_DISABLED_DW   0xFFFFFFFFUL
#define OTS_ELEMENTDATA_INFINITE_QW   0xFFFFFFFFFFFFFFFEULL
#define OTS_ELEMENTDATA_INFINITE_DW   0xFFFFFFFEUL

class SOCmnTrace;
class SOSrvObjectRoot;
class SOSrvWatch;

typedef struct _OTSAddressSpaceElementData OTSAddressSpaceElementData;

typedef struct _OTSAddressSpaceElementDataEx
{
	unsigned long long timeStamp;
	OTSAddressSpaceElementData *elementData;
	unsigned short elementDataFilter;
} OTSAddressSpaceElementDataEx;

class SOSRV_EXPORT SOSrvEntry : public SOCmnObject
{
	friend class SOCmnTrace;

public:
	SOSrvEntry(void);

	enum serverTypeCOM
	{
		InProc,     // COM in process server (DLL)
		OutProc     // COM out of process server (EXE)
	};

	// initialize/terminate toolkit
#ifdef SOFEATURE_DCOM
	virtual BOOL initialize(IN enum serverTypeCOM type, IN REFCLSID clsid,
							IN HINSTANCE hInst, OPTIONAL IN DWORD coInit = COINIT_APARTMENTTHREADED);
#endif
	virtual BOOL terminate(void);
#ifdef SOFEATURE_SOAP
	virtual BOOL initializeHTTP(IN WORD port);
#endif
#ifdef SOFEATURE_TUNNEL
	virtual BOOL initializeTP(IN WORD port, IN BOOL bEnableClientAuthentication, IN SOCmnList<SOCmnTPCredentials> *pPermissibleCredentialsList);
#endif

	// start/stop main thread of toolkit
	//  only if the main thread of the toolkit is running a OPC client will be able
	//  to create COM objects in the server
	virtual BOOL start(void);
	virtual BOOL stop(void);

	void setStringBasedObjectDataExpiry(DWORD milliseconds);
	DWORD getStringBasedObjectDataExpiry() const;
	void setStringBasedMinRecheck(DWORD milliseconds);
	DWORD getStringBasedMinRecheck() const
	{
		return static_cast<DWORD>(m_objectDataRecheckMinWait / 10000); // reduce 100-nanosecond resolution of FILETIME
	}
	void setStringBasedMinCleanupElementsCount(DWORD count);
	DWORD getStringBasedMaxCleanupElementsCount() const
	{
		return static_cast<DWORD>(m_objectDataMinCleanupElementsCount / 10000); // reduce 100-nanosecond resolution of FILETIME
	}
	void setStringBasedMaxCleanupElementsPercent(double percent);
	double getStringBasedMaxCleanupElementsPercent() const
	{
		return m_objectDataMaxCleanedElementsPercent;
	}
	BOOL setStringBasedObjectDataEx(IN const SOCmnString& pName, IN OTSAddressSpaceElementData* pElementData, IN unsigned short elementDataFilter = OTS_ELEMENTDATA_ALL);
	BOOL getStringBasedObjectDataEx(IN const SOCmnString& pName, OUT OTSAddressSpaceElementData **pElementData, IN unsigned short &elementDataFilter);
	BOOL cleanStringBasedObjectDataEx(IN const SOCmnString& additionalKeyToDelete = SOCmnString());

#ifdef SOFEATURE_DCOM
	// server registration
	virtual HRESULT registerServer(void);
	HRESULT registerCategories(void);
	virtual HRESULT unregisterServer(void);
	HRESULT unregisterCategories(void);

	// helpers for InProc server export functions
	virtual HRESULT getClassObject(IN REFCLSID rclsid, IN REFIID riid,
								   OUT LPVOID* ppvObj) = 0;
	virtual HRESULT canUnloadNow(void) = 0;

	// set the register information
	BOOL setRegisterResource(IN UINT rgsResource);
	BOOL addRegisterResourceReplacement(IN LPCTSTR key, IN LPCTSTR data);
	BOOL setRegisterInfo(IN LPCTSTR progID, IN LPCTSTR verIndProgID,
						 IN LPCTSTR description, OPTIONAL IN LPCTSTR ntServiceName = NULL);
	BOOL setRegisterCategories(IN BYTE catFlag);

	// do the server registartion work
	virtual HRESULT updateRegistry(BOOL doRegister);
#endif

	// root of OPC object tree
	SOSrvObjectRoot* getObjectRoot(void);

	// engine event handler
	void setEventHandler(IN SOSrvEventHandler* cbHandler);
	SOSrvEventHandler* getEventHandler(void);

	void setDefWorkerThreads(int count)
	{
		m_defWorkerThreads = count;
	}
	BOOL addWorkItem(SOCmnWorkItem* pWorkItem)
	{
		return m_pThreadPool->addWorkItem(pWorkItem);
	}

	// vendor string
	BOOL setVendorInfo(IN LPCTSTR vendorInfo);
	SOCmnString getVendorInfo(void) const;

	// version info
	BOOL setVersionInfo(IN WORD majorVersion, IN WORD minorVersion,
						IN WORD patchVersion, IN WORD buildNumber);
	void getVersionInfo(OUT WORD* majorVersion, OUT WORD* minorVersion,
						OUT WORD* patchVersion,  OUT WORD* buildNumber) const;

	// start time of OPC engine
	BOOL getStartTime(OUT SOCmnDateTime* startTime) const;

	// special release method
	virtual LONG release(void);

	// parameter for the check routines of the engine
	void setCheckParameter(IN DWORD clientInterval, OPTIONAL IN BOOL checkAllIFs = FALSE,
						   OPTIONAL IN DWORD clientInactiveTime = 180000);

#ifdef SOFEATURE_DCOM
	// send shutdown request to all connected clients
	virtual void shutdownAllClients(IN LPCTSTR reason);

	// main thread of the toolkit
	virtual DWORD mainThread(void);

	// get COM initialization mode of applications main thread
	DWORD getCoInit(void) const;

	// get class id of the OPC server
	virtual BOOL getCLSID(OUT CLSID& clsid, IN OPTIONAL BYTE serverInstance = 0);
	serverTypeCOM getServerTypeCOM(void) const
	{
		return m_type;
	}
#endif

	// entry flags
	BYTE getEntryFlags(void) const;
	void setEntryFlags(IN BYTE flags);

#ifdef SOFEATURE_DCOM
	// set shutdown event
	void setShutdownEvent(IN SOCmnEventPointer event);

	// main thread helpers
	void sendShutdownRequests(void);
	void checkClientConnections(OPTIONAL IN BOOL checkAllIFs = FALSE);
	void disconnectAllClients(void);
#endif

	BOOL isInitialized(IN BYTE mask)
	{
		return ((m_initialized & mask) == mask);
	}

	// object factory for toolkit objects
	virtual SOSrvCreator* getSrvCreator(void) = 0;

	// converter object
	SOSrvConverter* getConverter(void)
	{
		m_converter.addRef();
		return m_converter;
	}
	void setConverter(IN SOSrvConverter* conv)
	{
		m_converter = conv;
		m_converter.addRef();
	}

#ifdef SOFEATURE_DCOM
	// security
	virtual BOOL initializeSecurity(IN DWORD authLevel, IN DWORD impLevel,
									IN DWORD cbAuthLevel, IN DWORD cbImpLevel,
									IN DWORD capabilities);
	DWORD getAuthentificationLevel(void)
	{
		return m_serverAuthLevel;
	}
	DWORD getAuthentificationLevelForAdvise(void)
	{
		return m_callbackAuthLevel;
	}
	DWORD getImpersonationLevel(void)
	{
		return m_serverImpLevel;
	}
	DWORD getImpersonationLevelForAdvise(void)
	{
		return m_callbackImpLevel;
	}

	// ATL module access
	virtual HRESULT registerClassObjects(void) = 0;
	virtual HRESULT revokeClassObjects(void) = 0;
	virtual BOOL addComObjMapEntry(IN const CLSID* pClsid, IN void* pfnUpdateRegistry,
								   IN void* pfnGetClassObject, IN void* pfnCreateInstance, IN void* pfnGetObjectDescription,
								   IN void* pfnGetCategoryMap, IN void* pfnObjectMain) = 0;
#endif

	void setServiceName(LPCTSTR sN)
	{
		m_regNTServiceName = sN;
	}

	// server watch object
#ifdef SOFEATURE_WATCH
	SOCmnList<SOSrvWatch> *getWatchList(void)
	{
		return &m_watchList;
	}
	virtual SOSrvWatch* addWatchObject(void);
	virtual BOOL watchAttributeChange(IN SOCmnObject* pObj, IN LPCTSTR attrName, IN LPCTSTR attrValue);
	virtual BOOL watchAddObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj);
	virtual BOOL watchRemoveObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj);
#endif

#ifdef SOFEATURE_SOAP
	// HTTP server
	SOSrvHTTP* getHTTPServer(void)
	{
		m_srvHTTP.addRef();
		return m_srvHTTP;
	}
#endif
#ifdef SOFEATURE_TUNNEL
	// TP server
	SOSrvTP* getTPServer(void)
	{
		m_srvTP.addRef();
		return m_srvTP;
	}
#endif

protected:
	virtual ~SOSrvEntry(void);

	// add shutdown request to shutdown queue
#ifdef SOFEATURE_DCOM
	BOOL addShutdownRequest(IN SOSrvServer* server, IN LPCTSTR reason,
							IN HANDLE event);

	CLSID m_clsid;                      // class id of the server
	HINSTANCE m_appInstanceHandle;      // instance handle of the server application
#endif
	SOSrvObjectRoot* m_objectRoot;      // OPC object root
	SOSrvEventHandler* m_evtHandler;    // event handler
	SOCmnDateTime m_startTime;              // start time of server
	SOCmnList<OTSAddressSpaceElementDataEx> m_stringBasedObjectDataEx; // cache for string-based namespace against callbacks
	BOOL m_terminateLock;               // termination lock
	BYTE m_initialized;                 // initialized flags
#ifdef SOFEATURE_DCOM
	DWORD m_shutdownEventId;
	SOCmnEventPointer m_shutdownEvent;
	SOCmnThread m_mainThread;
	static DWORD _stdcall mainThreadProc(void* context);
	serverTypeCOM m_type;               // COM server type
	DWORD m_coInit;                     // COM initialization mode
	DWORD m_clientCheckInterval;        // time interval in ms for check functions
	BYTE m_clientCheckAllIFs; // check all client interfaces

	SOCmnList<SOSrvShutdownData> m_shutdownList;
	// shutdown request list
	// register / unregister data
	UINT m_regRGSResID;                 // resource id of ATL register script
	SOCmnStringList m_regRGSRepList;    // replacement list for ATL register script
	SOCmnString m_regProgID;            // progId of the server
	SOCmnString m_regVerIndProgID;      // version independent progId
	SOCmnString m_regDescription;       // server description
	BYTE m_regCategories;               // component categories (SOSRVENTRY_CATEGORY...)
#endif

	SOCmnString m_regNTServiceName;     // register server as NT Service with this name

	// server information
	SOCmnString m_vendorInfo;           // vendor string
	WORD m_majorVersion;                // version info
	WORD m_minorVersion;
	WORD m_patchVersion;
	WORD m_buildNumber;

#ifdef SOFEATURE_DCOM
	// security settings
	DWORD m_serverAuthLevel;
	DWORD m_callbackAuthLevel;
	DWORD m_serverImpLevel;
	DWORD m_callbackImpLevel;
#endif

	// ATL and thread customizing
	BYTE m_entryFlags;

	// OPC server monitoring
	SOCmnPointer<SOSrvConverter> m_converter;
	SOCmnList<SOSrvWatch> m_watchList;

	SOCmnThreadPool* m_pThreadPool;
	int m_defWorkerThreads;
	unsigned long long m_objectDataExpiryTimeout; // multiply by 10000 (filetime resolution)
	unsigned long long m_objectDataRecheckMinWait; // multiply by 10000 (filetime resolution)
	unsigned long long m_objectDataLastCleanup;
	unsigned long long m_objectDataMinCleanupElementsCount;
	double m_objectDataMaxCleanedElementsPercent;

#ifdef SOFEATURE_DCOM
	// ATL module access
	virtual void term(void) = 0;
	virtual void init(IN HINSTANCE instanceHandle) = 0;
	virtual HRESULT regServer(void) = 0;
	virtual HRESULT unregServer(void) = 0;
	virtual HRESULT updateRegistryWithRGS(IN BOOL doRegister, IN void* replacementArray) = 0;
#endif

#ifdef SOFEATURE_SOAP
	// HTTP server
	SOCmnPointer<SOSrvHTTP> m_srvHTTP;
#endif
#ifdef SOFEATURE_TUNNEL
	// Tunnel server
	SOCmnPointer<SOSrvTP> m_srvTP;
#endif

}; // SOSrvEntry


inline BOOL SOSrvEntry::setVendorInfo(IN LPCTSTR vendorInfo)
{
	m_vendorInfo = vendorInfo;
	return TRUE;
}

inline SOCmnString SOSrvEntry::getVendorInfo(void) const
{
	return m_vendorInfo;
}

inline BOOL SOSrvEntry::setVersionInfo(IN WORD majorVersion,
									   IN WORD minorVersion, IN WORD patchVersion, IN WORD buildNumber)
{
	m_majorVersion = majorVersion;
	m_minorVersion = minorVersion;
	m_patchVersion = patchVersion;
	m_buildNumber  = buildNumber;
	return TRUE;
}

inline void SOSrvEntry::getVersionInfo(OUT WORD* majorVersion,
									   OUT WORD* minorVersion, OUT WORD* patchVersion, OUT WORD* buildNumber) const
{
	if (majorVersion)
	{
		*majorVersion = m_majorVersion;
	}

	if (minorVersion)
	{
		*minorVersion = m_minorVersion;
	}

	if (patchVersion)
	{
		*patchVersion = m_patchVersion;
	}

	if (buildNumber)
	{
		*buildNumber  = m_buildNumber;
	}
}

inline BOOL SOSrvEntry::getStartTime(SOCmnDateTime* startTime) const
{
	if (!startTime)
	{
		return FALSE;
	}

	*startTime = m_startTime;
	return TRUE;
}

inline SOSrvEventHandler* SOSrvEntry::getEventHandler(void)
{
	if (m_evtHandler)
	{
		m_evtHandler->addRef();
	}

	return m_evtHandler;
}

#ifdef SOFEATURE_DCOM

inline BOOL SOSrvEntry::setRegisterResource(IN UINT rgsResource)
{
	m_regRGSResID = rgsResource;
	return TRUE;
}

inline BOOL SOSrvEntry::setRegisterInfo(IN LPCTSTR progID, IN LPCTSTR verIndProgID,
										IN LPCTSTR description, OPTIONAL IN LPCTSTR ntServiceName)
{
	if (m_regRGSResID)
	{
		return FALSE;
	}

	m_regProgID = progID;
	m_regVerIndProgID = verIndProgID;
	m_regDescription = description;

	if (ntServiceName)
	{
		m_regNTServiceName = ntServiceName;
	}

	return TRUE;
}

inline BOOL SOSrvEntry::setRegisterCategories(IN BYTE catFlag)
{
	m_regCategories = catFlag;
	return TRUE;
}

inline DWORD SOSrvEntry::getCoInit(void) const
{
	return m_coInit;
}

#endif

inline BYTE SOSrvEntry::getEntryFlags(void) const
{
	return m_entryFlags;
}

inline void SOSrvEntry::setEntryFlags(IN BYTE flags)
{
	m_entryFlags = flags;
}



//-----------------------------------------------------------------------------
// Global functions and defines                                               |
//-----------------------------------------------------------------------------

SOSRV_EXPORT void setSOSrvEntry(IN SOSrvEntry* entry);
SOSRV_EXPORT SOSrvEntry* getSOSrvEntry(void);

SOSRV_EXPORT SOSrvConverter* getSOSrvConverter(void);

#pragma pack(pop)
#endif
