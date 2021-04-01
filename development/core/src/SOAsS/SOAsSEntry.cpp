#include "stdafx.h"

#ifdef SOASS

#include <atlcom.h>
#include <comcat.h>
#include "SOAsS.h"
#include "SOAsSEntry.h"
#include "SODaSComServer.h"
#include "SOAeSComServer.h"
#include "SOVersion.h"
#ifdef SOFEATURE_SOAP
#include "SODaSXmlServer.h"
#endif
#ifdef SOFEATURE_TUNNEL
#include "SODaSTPServer.h"
#include "SODaSTPGroup.h"
#include "SOAeSTPServer.h"
#include "SOAeSTPSubscription.h"
#include "SOAeSTPAreaBrowser.h"
#endif

//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

CComModule _Module;

static SOAsSEntry* g_assEntry = NULL;
static SOCmnSync g_entrySync;

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

BEGIN_OBJECT_MAP(g_objectMap)
OBJECT_ENTRY(g_clsidDA0, SODaSComServer0)
OBJECT_ENTRY(g_clsidAE0, SOAeSComServer0)
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL},
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL},
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL},
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL},
#endif
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL},
{NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL},
#endif
END_OBJECT_MAP()


#ifdef SOFLAG_DEMO_VERSION
#ifndef SOFLAG_NO_DLLS
DWORD g_demoStamp = GetTickCount();
#endif
#endif


//-----------------------------------------------------------------------------
// getSOAsSEntry
//
// - get the entry object of the toolkit
//
// returns:
//		entry object
//
SOAsSEntry* getSOAsSEntry(void)
{
	SOCmnSingleLock<SOCmnSync> lock(&g_entrySync);

	if (g_assEntry)
	{
		g_assEntry->addRef();
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(g_assEntry, SOAsSEntry())
	}

	return g_assEntry;
} // getSOAsSEntry




//-----------------------------------------------------------------------------
// SOAsSEntry                                                                 |
//-----------------------------------------------------------------------------


SOAsSEntry::SOAsSEntry(void)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8X| %s]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle(), _T("AsS Entry"));
	setObjTraceId(id);
	m_vendorInfo = _T("Softing OPC Toolkit Data Access and Alarms & Events Server Connection Develompent Toolkit");
	m_majorVersion = MAJOR_VERSION_NUMBER;
	m_minorVersion = MINOR_VERSION_NUMBER;
	m_patchVersion = PATCH_VERSION_NUMBER;
	m_buildNumber = BUILD_NUMBER;
	m_objectRoot = NULL;
	SOCMN_ALLOCATE_OBJECT_C(m_creator, SOSrvCreator(), (SOSrvCreator*))
	m_entryDA = getSODaSEntry();
	m_entryDA->setEntryFlags(0);
	m_entryAE = getSOAeSEntry();
	m_entryAE->setEntryFlags(0);
#ifdef SOFEATURE_SOAP
	SOCmnPointer<SOSrvHTTPHandler> hHTTP;
	SOCMN_ALLOCATE_OBJECT_C(hHTTP, SOSrvHTTPHandler(), (SOSrvHTTPHandler*))
	m_srvHTTP->setHTTPHandler(hHTTP);
#endif
	setRegisterCategories(SOSRVENTRY_CATEGORY_DA10 | SOSRVENTRY_CATEGORY_DA20  | SOSRVENTRY_CATEGORY_DA30 | SOSRVENTRY_CATEGORY_AE10);
	::setSOSrvEntry(this);
}


SOAsSEntry::~SOAsSEntry(void)
{
	interlockedIncrement(&m_refCount);
#ifdef SOFEATURE_SOAP
	m_srvHTTP->setHTTPHandler(NULL);
	m_srvHTTP.release();
#endif
	m_surveyServer.release();
	m_entryAE.release();
	m_entryDA.release();
	g_assEntry = NULL;
	::setSOSrvEntry(NULL);
	InterlockedDecrement(&m_refCount);
}


//-----------------------------------------------------------------------------
//  initialize
//
// - initialize toolkit
//
// returns:
//		TRUE  - initialized
//		FALSE - error during init
//
BOOL SOAsSEntry::initialize(
	IN enum serverTypeCOM type, // server type (inproc or outproc)
	IN REFCLSID clsidDA,        // class id of DA server
	IN REFCLSID clsidAE,        // class id of AE server
	IN HINSTANCE hInst,         // instance handle of application
	IN DWORD coInit)            // COM init mode
{
	g_clsidAE0 = clsidAE;
	g_clsidDA0 = clsidDA;
	BOOL ret = SOSrvEntry::initialize(type, CLSID_NO, hInst, coInit);
	m_entryDA->initialize(type, clsidDA, hInst, coInit);
	m_entryAE->initialize(type, clsidAE, hInst, coInit);
	return ret;
} // initialize


#ifdef SOFEATURE_SOAP
BOOL SOAsSEntry::initializeHTTP(IN WORD port)
{
	BOOL ret = SOSrvEntry::initializeHTTP(port);
	m_entryDA->initializeHTTP(0);

	if (ret)
	{
		SOCmnPointer<SOSrvHTTPHandler> xmlSrv;
		xmlSrv = (SOSrvHTTPHandler*)m_srvHTTP->getHTTPHandler();

		if (xmlSrv.isNotNull())
		{
			ret = xmlSrv->initialize();
		}
	}

	return ret;
} // initializeHTTP

BOOL SOAsSEntry::addXMLDAReceiver(IN LPCTSTR url)
{
	SOCmnPointer<SOSrvHTTPHandler> xmlSrv;
	BOOL ret = FALSE;
	xmlSrv = (SOSrvHTTPHandler*)m_srvHTTP->getHTTPHandler();

	if (xmlSrv.isNotNull())
	{
		// create XML subscription server
		SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
		entryDA->createXMLSubscriptionServer();
		SOCmnPointer<SODaSXmlServer> xmlRec;
		SOCMN_ALLOCATE_OBJECT_C(xmlRec, SODaSXmlServer(), (SODaSXmlServer*))

		if (url)
			if (_tcslen(url) > 0)
			{
				xmlRec->setURL(url);
			}

		ret = xmlSrv->addHTTPReceiver(xmlRec);
	}

	return ret;
}

#endif

#ifdef SOFEATURE_TUNNEL

BOOL SOAsSEntry::addTPDAReceiver(void)
{
	BOOL ret;
	SOCmnPointer<SODaSTPServer> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SODaSTPServer(), (SODaSTPServer*))
	ret = m_srvTP->addTPReceiver(tpRec);
	SOCmnPointer<SODaSTPGroup> tpRecG;
	SOCMN_ALLOCATE_OBJECT_C(tpRecG, SODaSTPGroup(), (SODaSTPGroup*))
	ret &= m_srvTP->addTPReceiver(tpRecG);
	return ret;
}

BOOL SOAsSEntry::addTPAEReceiver(void)
{
	BOOL ret;
	SOCmnPointer<SOAeSTPServer> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SOAeSTPServer(), (SOAeSTPServer*))
	ret = m_srvTP->addTPReceiver(tpRec);
	SOCmnPointer<SOAeSTPSubscription> tpRecS;
	SOCMN_ALLOCATE_OBJECT_C(tpRecS, SOAeSTPSubscription(), (SOAeSTPSubscription*))
	ret &= m_srvTP->addTPReceiver(tpRecS);
	SOCmnPointer<SOAeSTPAreaBrowser> tpRecA;
	SOCMN_ALLOCATE_OBJECT_C(tpRecA, SOAeSTPAreaBrowser(), (SOAeSTPAreaBrowser*))
	ret &= m_srvTP->addTPReceiver(tpRecA);
	return ret;
}

#endif

//-----------------------------------------------------------------------------
// initializeSecurity
//
// - initialize security
//
// returns:
//		TRUE  - initialization done
//		FALSE - already initialized
//
BOOL SOAsSEntry::initializeSecurity(
	IN DWORD authnLevel,
	IN DWORD impLevel,
	IN DWORD cbAuthnLevel,
	IN DWORD cbImpLevel,
	IN DWORD capabilities)
{
	BOOL ret = SOSrvEntry::initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
	m_entryDA->initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
	m_entryAE->initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
	return ret;
} // initializeSecurity


//-----------------------------------------------------------------------------
// terminate
//
// - terminate toolkit
//
// returns:
//		TRUE  - terminated
//		FALSE - error during terminate
//
BOOL SOAsSEntry::terminate(void)
{
	m_entryAE->terminate();
	m_entryDA->terminate();
	return SOSrvEntry::terminate();
} // terminate


//-----------------------------------------------------------------------------
// start
//
// - start main thread and the condition survey thread of the entry
//
// returns:
//		TRUE  - thread started
//		FALSE - error creating the thread
//
BOOL SOAsSEntry::start(void)
{
	BOOL ret;
	ret = SOSrvEntry::start();

	if (ret)
	{
		m_entryDA->setShutdownEvent(SOCmnEventPointer(m_mainThread.getEvents(), m_shutdownEventId));
		m_entryAE->setShutdownEvent(SOCmnEventPointer(m_mainThread.getEvents(), m_shutdownEventId));
	}

	if (m_entryAE->getCheckConditions())
	{
		// enabled condition survey
		// create DaS survey server object
		SOCmnPointer<SODaSObjectRoot> obRoot = (SODaSObjectRoot*)m_entryDA->getObjectRoot();
		SOCmnPointer<SODaSCreator> creator = m_entryDA->getCreator();
		m_surveyServer = creator->createServer(FALSE, 0);
		m_surveyServer->setClientName(_T("Alarms & Events Survey"));
		obRoot->addBranch(m_surveyServer);
	}

	m_entryAE->startSurvey();

	if ((m_initialized & SOSRVENTRY_INIT_COM) == SOSRVENTRY_INIT_COM)
	{
		return (ret && m_mainThread.isRunning());
	}
	else
	{
		return ret;
	}
} // start


//-----------------------------------------------------------------------------
// stop
//
// - stop the main thread and the condition survey thread of the entry
//
// returns:
//		TRUE  - thread stoped
//		FALSE - can't stop thread
//
BOOL SOAsSEntry::stop(void)
{
	if (m_surveyServer.isNotNull())
	{
		SOCmnPointer<SODaSObjectRoot> obRoot = (SODaSObjectRoot*)m_entryDA->getObjectRoot();
		m_surveyServer->removeAllAdditionalElements();
		m_surveyServer->removeAllLeafs();
		m_surveyServer->removeAllBranches();
		obRoot->removeBranch(m_surveyServer);
		m_surveyServer.release();
	}

	m_entryAE->stopSurvey();
	return SOSrvEntry::stop();
} // stop


//-----------------------------------------------------------------------------
// mainThread
//
// - main thread of the toolkit
//
// returns:
//		thread exit code
//
DWORD SOAsSEntry::mainThread(void)
{
	DWORD ret;
	BOOL end = FALSE;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("started toolkit main thread")));
	//-- initialize toolkit thread --
	HRESULT res = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(res))
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("Can't initialize COM: 0x%X"), res));
		return res;
	}

	if (m_type == OutProc)
	{
		// register class factory for EXEs
		if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
		{
			registerClassObjects();
		}
	}

	//-- wait till toolkit termiantion --
	while (!end)
	{
		SOCMN_TRY
		{
			ret = m_mainThread.waitForEvents(m_clientCheckInterval);

			if (ret ==  m_mainThread.getStopEventId())
			{
				// end event
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("received end event")));
				end = TRUE;
			}
			else if (ret == m_shutdownEventId)
			{
				// shutdown event
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("received shutdown event")));
				sendShutdownRequests();
				m_entryDA->sendShutdownRequests();
				m_entryAE->sendShutdownRequests();
			}
			else
			{
				// timeout
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("check client connections")));
				m_entryDA->checkClientConnections(m_clientCheckAllIFs);
				m_entryAE->checkClientConnections(m_clientCheckAllIFs);

				// free unused events
				SOCmnEventPool* evPool = getEventPool();

				if (evPool)
				{
					evPool->freeAllEvents();
				}

				// free expired cache "in background"
				m_entryDA->cleanStringBasedObjectDataEx();
			}
		}
		SOCMN_CATCH(SOCmnException e)
		{
#ifndef SOOS_WINDOWS_CE
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("EXCEPTION in Toolkit Main Thread %s"), (LPCTSTR)e.m_description));
			_ASSERTION(FALSE, "Exception in toolkit main thread");
#endif
		}
		SOCMN_CATCH(...)
		{
#ifndef SOOS_WINDOWS_CE
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("EXCEPTION in Toolkit Main Thread")));
			_ASSERTION(FALSE, "Exception in toolkit main thread");
#endif
		}
	}

	//-- terminated engine --
	if (m_type == OutProc)
	{
		// revoke class factory for EXEs
		if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
		{
			revokeClassObjects();
		}
	}

	m_entryDA->disconnectAllClients();
	m_entryAE->disconnectAllClients();
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("COM uninitialize")));
	::CoUninitialize();
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("terminated toolkit main thread")));
	return S_OK;
} // mainThread


//-----------------------------------------------------------------------------
// shutdownAllClients
//
// - add shutdown requests for all clients to the shutdown list
//
void SOAsSEntry::shutdownAllClients(
	IN LPCTSTR reason)  // shutdown reason
{
	m_entryAE->shutdownAllClients(reason);
	m_entryDA->shutdownAllClients(reason);
} // shutdownAllClients


//-----------------------------------------------------------------------------
// addWatchObject
//
// - create and add watch object to watch list
//
// returns:
//		pointer to watch object
//
SOSrvWatch* SOAsSEntry::addWatchObject(void)
{
#ifdef SOFEATURE_WATCH
	SOSrvWatch* watch = SOSrvEntry::addWatchObject();

	if (watch)
	{
		SOCmnPointer<SODaSWatchHandler> whDA = new SODaSWatchHandler();
		watch->setDAHandler(whDA);
		SOCmnPointer<SOAeSWatchHandler> whAE = new SOAeSWatchHandler();
		watch->setAEHandler(whAE);
	}

	return watch;
#else
	return NULL;
#endif // SOFEATURE_WATCH
} // addWatchObject


//-----------------------------------------------------------------------------
// getSrvCreator
//
// - get toolkit object factory
//
// returns:
//		toolkit object factory
//
SOSrvCreator* SOAsSEntry::getSrvCreator(void)
{
	m_creator.addRef();
	return (SOSrvCreator*)m_creator;
} // getSrvCreator


//-----------------------------------------------------------------------------
// ATL COM Helpers for _Module access
//
void SOAsSEntry::term(void)
{
	_Module.Term();
}

void SOAsSEntry::init(
	IN HINSTANCE instanceHandle)
{
	_ATL_OBJMAP_ENTRY* objectMap = g_objectMap;

	if (CLSID_NO == g_clsidAE0)
	{
		g_objectMap[1].pclsid = g_objectMap[0].pclsid;
		g_objectMap[1].pfnUpdateRegistry = g_objectMap[0].pfnUpdateRegistry;
		g_objectMap[1].pfnGetClassObject = g_objectMap[0].pfnGetClassObject;
		g_objectMap[1].pfnCreateInstance = g_objectMap[0].pfnCreateInstance;
		g_objectMap[1].pCF = g_objectMap[0].pCF;
		g_objectMap[1].dwRegister = g_objectMap[0].dwRegister;
		g_objectMap[1].pfnGetObjectDescription = g_objectMap[0].pfnGetObjectDescription;
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
		g_objectMap[1].pfnGetCategoryMap = g_objectMap[0].pfnGetCategoryMap;
#endif
		g_objectMap[1].pfnObjectMain = g_objectMap[0].pfnObjectMain;
#endif
		objectMap = &g_objectMap[1];
	}

	if (CLSID_NO == g_clsidDA0)
	{
		objectMap = &g_objectMap[1];
	}

	_Module.Init(objectMap, instanceHandle);
}
HRESULT SOAsSEntry::updateRegistryWithRGS(IN BOOL doRegister, IN void* replacementArray)
{
#ifndef SOOS_WINDOWS_CE
#if (_MSC_VER > 1200)
	HINSTANCE hInstSave = _AtlBaseModule.m_hInst;
	_AtlBaseModule.m_hInst = m_appInstanceHandle;
#endif
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, doRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#if (_MSC_VER > 1200)
	_AtlBaseModule.m_hInst = hInstSave;
#endif
#else
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, doRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#endif
	return res;
}
HRESULT SOAsSEntry::getClassObject(
	IN REFCLSID rclsid,
	IN REFIID riid,
	OUT LPVOID* ppvObj)
{
	return _Module.GetClassObject(rclsid, riid, ppvObj);
}
HRESULT SOAsSEntry::canUnloadNow(void)
{
	LONG lockCount = _Module.GetLockCount();
	return (lockCount == 0) ? S_OK : S_FALSE;
}
HRESULT SOAsSEntry::registerClassObjects(void)
{
	return _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
}
HRESULT SOAsSEntry::revokeClassObjects(void)
{
	return _Module.RevokeClassObjects();
}
HRESULT SOAsSEntry::regServer(void)
{
	return _Module.RegisterServer(FALSE);
}
HRESULT SOAsSEntry::unregServer(void)
{
	return _Module.UnregisterServer();
}
BOOL SOAsSEntry::addComObjMapEntry(
	IN const CLSID* pClsid,
	IN void* pfnUpdateRegistry,
	IN void* pfnGetClassObject,
	IN void* pfnCreateInstance,
	IN void* pfnGetObjectDescription,
	IN void* pfnGetCategoryMap,
	IN void* pfnObjectMain)
{
	DWORD i;

	for (i = 2; i < 22; i++)
	{
		if (NULL == g_objectMap[i].pclsid)
		{
			g_objectMap[i].pclsid = pClsid;
			g_objectMap[i].pfnUpdateRegistry = (HRESULT(WINAPI*)(BOOL))pfnUpdateRegistry;
			g_objectMap[i].pfnGetClassObject = (_ATL_CREATORFUNC*)pfnGetClassObject;
			g_objectMap[i].pfnCreateInstance = (_ATL_CREATORFUNC*)pfnCreateInstance;
			g_objectMap[i].pCF = NULL;
			g_objectMap[i].dwRegister = 0;
			g_objectMap[i].pfnGetObjectDescription = (_ATL_DESCRIPTIONFUNC*)pfnGetObjectDescription;
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
			g_objectMap[i].pfnGetCategoryMap = (_ATL_CATMAPFUNC*)pfnGetCategoryMap;
#endif
			g_objectMap[i].pfnObjectMain = (void (WINAPI*)(bool))pfnObjectMain;
#endif
			return TRUE;
		}
	}

	return FALSE;
}


BOOL SOAsSEntry::addComObjMapEntry(IN const CLSID* clsid, IN DWORD cnt, IN DWORD specID)
{
	BOOL ret = FALSE;

	if (specID == SOSRVSERVER_SPECID_DA)
	{
		switch (cnt)
		{
		case 1:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer1));
			g_clsidDA1 = *clsid;
			break;

		case 2:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer2));
			g_clsidDA2 = *clsid;
			break;

		case 3:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer3));
			g_clsidDA3 = *clsid;
			break;

		case 4:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer4));
			g_clsidDA4 = *clsid;
			break;

		case 5:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer5));
			g_clsidDA5 = *clsid;
			break;

		case 6:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer6));
			g_clsidDA6 = *clsid;
			break;

		case 7:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer7));
			g_clsidDA7 = *clsid;
			break;

		case 8:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer8));
			g_clsidDA8 = *clsid;
			break;

		case 9:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer9));
			g_clsidDA9 = *clsid;
			break;
		}
	}
	else if (specID == SOSRVSERVER_SPECID_AE)
	{
		switch (cnt)
		{
		case 1:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer1));
			g_clsidAE1 = *clsid;
			break;

		case 2:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer2));
			g_clsidAE2 = *clsid;
			break;

		case 3:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer3));
			g_clsidAE3 = *clsid;
			break;

		case 4:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer4));
			g_clsidAE4 = *clsid;
			break;

		case 5:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer5));
			g_clsidAE5 = *clsid;
			break;

		case 6:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer6));
			g_clsidAE6 = *clsid;
			break;

		case 7:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer7));
			g_clsidAE7 = *clsid;
			break;

		case 8:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer8));
			g_clsidAE8 = *clsid;
			break;

		case 9:
			ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SOAeSComServer9));
			g_clsidAE9 = *clsid;
			break;
		}
	}

	return ret;
}

#endif // SOASS




