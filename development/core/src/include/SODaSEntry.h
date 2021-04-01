#ifndef _SODASENTRY_H_
#define _SODASENTRY_H_

#include <opcda.h>
#include "SOCmnObject.h"
#include "SOSrvEntry.h"
#include "SODaSRequest.h"
#include "SODaSServer.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SODaSCreator                                                               |
//-----------------------------------------------------------------------------

class SODaSServer;
class SODaSGroup;
class SODaSItem;
class SODaSCache;
class SODaSItemTag;
class SODaSTag;
class SODaSNode;
class SODaSProperty;
class SODaSNameSpaceRoot;
class SODaSObjectRoot;
class SOCmnElement;

class SODAS_EXPORT SODaSCreator : public SOSrvCreator
{
public:
	virtual SODaSServer* createServer(IN BOOL publicGroups, IN BYTE serverInstance);
	virtual SODaSGroup* createGroup(IN SODaSServer* parent);
	virtual SODaSItem* createItem(IN SODaSGroup* parent, IN SODaSItemTag* tag);
	virtual SODaSRequest* createRequest(IN SODaSServer* server, IN SODaSItem* item,
										IN SODaSItemTag* tag, IN BYTE operation, IN WORD type);
	virtual SODaSCache* createCache(IN SODaSItemTag* tag);
	virtual SODaSTag* createTag(IN LPCTSTR qualItemID);
	virtual SODaSNode* createNode(IN LPCTSTR nodePath);
	virtual SODaSProperty* createProperty(IN SOCmnElement* namespaceElement, IN DWORD propertyId,
										  IN LPCTSTR propertyName);
	virtual SODaSTransaction* createTransaction(IN SODaSGroup* group, IN BYTE transactionType,
												IN BYTE connectionType, IN DWORD connection);
	virtual SODaSNameSpaceRoot* createNameSpaceRoot(IN BYTE serverInstance);
	virtual SODaSObjectRoot* createObjectRoot(void);
}; // SODaSCreator

#define GenericCreator SODaSCreator




//-----------------------------------------------------------------------------
// SODaSConfigFileHandler                                                     |
//-----------------------------------------------------------------------------

struct SODAS_EXPORT SODaSConfigFileHandler : public SOCmnObject
{
public:
	SODaSConfigFileHandler();
	virtual BOOL isDirty(IN SODaSServer* server) = 0;
	virtual HRESULT load(IN SODaSServer* server, IN LPCTSTR name, IN DWORD mode) = 0;
	virtual HRESULT save(IN SODaSServer* server, IN LPCTSTR name, IN BOOL remember) = 0;
	virtual HRESULT getCurFile(IN SODaSServer* server, OUT SOCmnString& name) = 0;
}; // SODaSConfigFileHandler

#define GenericConfigFileHandler SODaSConfigFileHandler




//-----------------------------------------------------------------------------
// SODaSEntry                                                                 |
//-----------------------------------------------------------------------------

#define SODAS_SERVER_INSTANCE_COUNT 1

class SODaSNameSpaceRoot;
class SODaSObjectRoot;
class SOCmnTrace;

class SODAS_EXPORT SODaSEntry : public SOSrvEntry
{
	friend class SOCmnTrace;
	friend class SODaSRequest;
	friend class SODaSServer;

public:
	SODaSEntry(void);

	// initialize engine
#ifdef SOFEATURE_DCOM
	virtual BOOL initialize(IN enum serverTypeCOM type, IN REFCLSID clsid,
							IN HINSTANCE hInst, OPTIONAL IN DWORD coInit = COINIT_APARTMENTTHREADED);
#endif
	virtual BOOL terminate(void);
#ifdef SOFEATURE_SOAP
	virtual BOOL initializeHTTP(IN WORD port);
	BOOL addXMLDAReceiver(IN LPCTSTR url);
#endif
#ifdef SOFEATURE_TUNNEL
	BOOL addTPDAReceiver(void);
#endif

	// namespace root
	SODaSNameSpaceRoot* getNameSpaceRoot(IN OPTIONAL BYTE serverInstance = 0);
	void createNewNameSpaceRoot(IN OPTIONAL BYTE serverInstance = 0);

	// root of OPC object tree
	SODaSObjectRoot* getOPCRoot(void);

	// get the public groups server object
	SODaSServer* getPublicGroups(void);

	// object factory for OPC objects
	void setCreator(IN SODaSCreator* creator);
	SODaSCreator* getCreator(void);
	virtual SOSrvCreator* getSrvCreator(void);

	// handler object for configuration file calls (IPersistFile)
	void setConfigurationFileHandler(IN SODaSConfigFileHandler* cfgHandler);
	SODaSConfigFileHandler* getConfigurationFileHandler(void);

#ifdef SOFEATURE_DCOM
	// COM helper methods
	virtual HRESULT getClassObject(IN REFCLSID rclsid, IN REFIID riid,
								   OUT LPVOID* ppvObj);
	virtual HRESULT canUnloadNow(void);
#endif

	// delimiter between tag path and property id (is a not allowed char of tree)
	void setPropertyDelimiter(IN TCHAR propDel);
	TCHAR getPropertyDelimiter(void) const;

	// OPC server state
	BOOL setServerState(IN OPCSERVERSTATE serverState);
	BOOL setAutoServerState();
	OPCSERVERSTATE getServerState(void) const;
	LPCTSTR getServerStateString(void) const;

	// minimal update rate
	void setMinUpdateRate(DWORD minUpdateRate);
	DWORD getMinUpdateRate(void);

	// minimal keep alive time
	void setMinKeepAliveTime(DWORD minKeepAliveTime)
	{
		m_minKeepAliveTime = minKeepAliveTime;
	}
	DWORD getMinKeepAliveTime(void)
	{
		return m_minKeepAliveTime;
	}

#ifdef SOFEATURE_WATCH
	// server watch object
	virtual SOSrvWatch* addWatchObject(void);
	void getObjectCountStatistics(IN SOCmnStringEx& szXML);
	void incObjectCountStatistics(IN DWORD objType);
	void decObjectCountStatistics(IN DWORD objType);
#endif

#ifdef SOFEATURE_SOAP
	void createXMLSubscriptionServer(void);
	SODaSServer* getXMLSubscriptionServer(void);
#endif

#ifdef SOFEATURE_DCOM
	// ATL COM helper methods
	virtual HRESULT revokeClassObjects(void);
	virtual BOOL addComObjMapEntry(IN const CLSID* pClsid, IN void* pfnUpdateRegistry,
								   IN void* pfnGetClassObject, IN void* pfnCreateInstance, IN void* pfnGetObjectDescription,
								   IN void* pfnGetCategoryMap, IN void* pfnObjectMain);
	virtual BOOL addComObjMapEntry(IN const CLSID* pClsid, IN DWORD cnt);
	virtual BOOL getCLSID(OUT CLSID& clsid, IN OPTIONAL BYTE serverInstance = 0);
#endif

protected:
	virtual ~SODaSEntry(void);

	OPCSERVERSTATE m_serverState;           // OPC server state
	SODaSNameSpaceRoot* m_nsRoot[SODAS_SERVER_INSTANCE_COUNT];          // namespace root
	SODaSConfigFileHandler* m_cfgHandler;   // config handler
	SODaSCreator* m_creator;                // creator
	DWORD m_minUpdateRate;                  // minimal update rate (maximal update rate is 40 days)
	DWORD m_minKeepAliveTime;       // minimal keep alive time (maximal keep alive time is 40 days)
#ifdef SOFEATURE_SOAP
	SOCmnPointer<SODaSServer> m_xmlSubsServer;
#endif
	BOOL m_serverStateManuallySet;
	struct SODaSEntryStatisticsData
	{
		ULONG serverObjs;
		ULONG groupObjs;
		ULONG itemObjs;
		ULONG nodeObjs;
		ULONG tagObjs;
		ULONG propertyObjs;
	} m_statistics;
	TCHAR m_delimProp;                      // tag property delimiter

#ifdef SOFEATURE_DCOM
	// ATL COM helper methods
	virtual void term(void);
	virtual void init(HINSTANCE instanceHandle);
	virtual HRESULT registerClassObjects(void);
	virtual HRESULT regServer(void);
	virtual HRESULT unregServer(void);
	virtual HRESULT updateRegistryWithRGS(IN BOOL doRegister, IN void* replacementArray);
#endif
}; // SODaSEntry

inline void SODaSEntry::setPropertyDelimiter(IN TCHAR propDel)
{
	m_delimProp = propDel;
}

inline TCHAR SODaSEntry::getPropertyDelimiter(void) const
{
	return m_delimProp;
}

inline BOOL SODaSEntry::setServerState(IN OPCSERVERSTATE serverState)
{
	m_serverState = serverState;
	m_serverStateManuallySet = TRUE;
	return TRUE;
}

inline BOOL SODaSEntry::setAutoServerState()
{
	if (m_serverStateManuallySet == FALSE)
	{
		m_serverState = OPC_STATUS_RUNNING;
		return TRUE;
	}
	return FALSE;
}

inline OPCSERVERSTATE SODaSEntry::getServerState(void) const
{
	return m_serverState;
}

inline SODaSCreator* SODaSEntry::getCreator(void)
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return m_creator;
}

inline SODaSConfigFileHandler* SODaSEntry::getConfigurationFileHandler(void)
{
	if (m_cfgHandler)
	{
		m_cfgHandler->addRef();
	}

	return m_cfgHandler;
}

inline void SODaSEntry::setMinUpdateRate(DWORD minUpdateRate)
{
	m_minUpdateRate = minUpdateRate;
}

inline DWORD SODaSEntry::getMinUpdateRate(void)
{
	return m_minUpdateRate;
}

inline SODaSObjectRoot* SODaSEntry::getOPCRoot(void)
{
	return (SODaSObjectRoot*)getObjectRoot();
}

#define OPCEngine SODaSEntry




//-----------------------------------------------------------------------------
// Global Functions                                                           |
//-----------------------------------------------------------------------------

// get pointer of the OPC engine object
SODAS_EXPORT SODaSEntry* getSODaSEntry(void);
SODAS_EXPORT SODaSEntry* getOPCEngine(void);

#pragma pack(pop)
#endif
