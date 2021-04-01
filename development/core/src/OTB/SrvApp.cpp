#include "stdafx.h"

#ifdef OTB_SERVER

#include "SrvApp.h"
#ifdef OTB_SERVER_DA
#include "ServerDACreator.h"
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
#include "ServerAECreator.h"
#endif // OTB_SERVER_AE
#include "SOSrvHttpEx.h"
#include "SOSrvTraceEx.h"
#include "WebServer.h"

#include "SOCmnTrace.h"

// the server application object instance
ServerApp* g_pTheServerApp = NULL;

#ifdef SOFEATURE_DCOM
static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
#endif

//-----------------------------------------------------------------------------
// createServerApp
//
// - create server application object
//
void createServerApp()
{
	if (g_pTheServerApp == NULL)
	{
		SOCMN_ALLOCATE_OBJECT(g_pTheServerApp, ServerApp());
	}
} // createServerApp


//-----------------------------------------------------------------------------
// destroyServerApp
//
// - destroy server application object
//
void destroyServerApp()
{
	SOCMN_FREE_OBJECT(g_pTheServerApp);
} // destroyServerApp


//-----------------------------------------------------------------------------
// getServerApp
//
// - server application access function
//
// returns:
//   pointer to the server application object
//
ServerApp* getServerApp()
{
	return g_pTheServerApp;
} // getServerApp



//-----------------------------------------------------------------------------
// ServerApp
//-----------------------------------------------------------------------------

ServerApp::ServerApp()
{
#ifdef SOFEATURE_DCOM
	// class id of OPC DA Server
	m_clsidDA = CLSID_NO;
	// ProdId of OPC DA Server
	m_verIndProgIdDA = _T("");
	// version independent ProgId of OPC DA Server
	m_progIdDA = _T("");
	// class id of OPC AE Server
	m_clsidAE = CLSID_NO;
	// ProdId of OPC AE Server
	m_verIndProgIdAE = _T("");
	// version independent ProgId of OPC DA Server
	m_progIdAE = _T("");
#endif // SOFEATURE_DCOM
	// description of OPC DA Server
	m_serverName = _T("");
	m_ipPortHTTP = 0;
	// vendor information
	m_vendorInfo = _T("Softing Industrial Automation GmbH");
	// OPC server major version number
	m_majorVersionNumber = 0;
	// OPC server minor version number
	m_minorVersionNumber = 0;
	// OPC server patch version number
	m_patchVersionNumber = 0;
	// OPC server build number
	m_buildNumber = 0;
	m_addressSpaceDelimiter = _T('.');
	m_clientCheckPeriod = 30000;
	m_propertyDelimiter = _T('#');
	m_minUpdateRateDA = 0;
	m_supportDisableConditions = FALSE;
	m_addressSpaceType = OTS_ADDRESSSPACETYPE_OBJECT | OTS_ADDRESSSPACETYPE_STRING;
	// runtime data
	m_startedByCOM = FALSE;
	m_pCanCloseEvent = NULL;
	m_startupEvent = NULL;
#ifdef OTB_SERVER_AE
	m_alarmList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
#endif // OTB_SERVER_AE
#ifdef SOFEATURE_TUNNEL
	m_tpPort = 0;
	m_tpUserList.create();
	m_tpPwdList.create();
#endif // SOFEATURE_TUNNEL
	memset(&m_callbackFunctions, 0, sizeof(OTSCallbackFunctions));
} // ServerApp


//-----------------------------------------------------------------------------
// initialize
//
// - initialize the OPC server application
//
// returns:
//    TRUE  - application initialized
//    FALSE - error during application initialization
//
BOOL ServerApp::initialize(void)
{
#ifdef OTB_SERVER_DA
	m_itemTagChangedList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
#endif // OTB_SERVER_DA
	return TRUE;
} // initialize


//-----------------------------------------------------------------------------
// prepare
//
// - prepare the OPC toolkit
//
// returns:
//    TRUE  - OPC toolkit initialized
//    FALSE - error during toolkit preparation
//
BOOL ServerApp::prepare(void)
{
#ifdef OTB_SERVER_DAAE
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
#endif
	BOOL ret = TRUE;
	// set the applications creators for OPC toolkit objects
#ifdef OTB_SERVER_DA
	SOCmnPointer<ServerDACreator> creatorDA;
	SOCMN_ALLOCATE_OBJECT_C(creatorDA, ServerDACreator(), (ServerDACreator*));
	entryDA->setCreator(creatorDA);
	creatorDA.release();
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
	SOCmnPointer<ServerAECreator> creatorAE;
	SOCMN_ALLOCATE_OBJECT_C(creatorAE, ServerAECreator(), (ServerAECreator*));
	entryAE->setCreator(creatorAE);
	creatorAE.release();
#endif // OTB_SERVER_AE
	// initialize DCOM class factory
#ifdef SOFEATURE_DCOM
#ifdef OTB_SERVER_DAAE

	if ((isDemoMode()) || ((isLicensed(OTS_FEATURE_DA_SERVER)) && (isLicensed(OTS_FEATURE_AE_SERVER))))
	{
		// DA and AE
		entry->initialize((SOSrvEntry::serverTypeCOM)m_appType, m_clsidDA, m_clsidAE, GetModuleHandle(NULL), COINIT_MULTITHREADED);
		entry->initializeSecurity(m_authLevel, m_impLevel, m_cbAuthLevel, m_cbImpLevel, EOAC_NONE);
	}
	else
	{
		if (isLicensed(OTS_FEATURE_DA_SERVER))
		{
			// DA only
			entry->initialize((SOSrvEntry::serverTypeCOM)m_appType, m_clsidDA, CLSID_NO, GetModuleHandle(NULL), COINIT_MULTITHREADED);
			entry->initializeSecurity(m_authLevel, m_impLevel, m_cbAuthLevel, m_cbImpLevel, EOAC_NONE);
		}

		if (isLicensed(OTS_FEATURE_AE_SERVER))
		{
			// AE only
			entry->initialize((SOSrvEntry::serverTypeCOM)m_appType, CLSID_NO, m_clsidAE, GetModuleHandle(NULL), COINIT_MULTITHREADED);
			entry->initializeSecurity(m_authLevel, m_impLevel, m_cbAuthLevel, m_cbImpLevel, EOAC_NONE);
		}
	}

#else
#ifdef OTB_SERVER_DA

	if ((isDemoMode()) || (isLicensed(OTS_FEATURE_DA_SERVER)))
	{
		// DA only
		entryDA->initialize((SOSrvEntry::serverTypeCOM)m_appType, m_clsidDA, GetModuleHandle(NULL), COINIT_MULTITHREADED);
		entryDA->initializeSecurity(m_authLevel, m_impLevel, m_cbAuthLevel, m_cbImpLevel, EOAC_NONE);
	}

#else
#ifdef OTB_SERVER_AE

	if ((isDemoMode()) || (isLicensed(OTS_FEATURE_AE_SERVER)))
	{
		// AE only
		entryAE->initialize((SOSrvEntry::serverTypeCOM)m_appType, m_clsidAE, GetModuleHandle(NULL), COINIT_MULTITHREADED);
		entryAE->initializeSecurity(m_authLevel, m_impLevel, m_cbAuthLevel, m_cbImpLevel, EOAC_NONE);
	}

#endif
#endif
#endif
#endif
#ifdef OTB_XMLDA_SERVER
#ifdef OTB_SERVER_DAAE

	if ((isDemoMode()) || (isLicensed(OTS_FEATURE_XMLDA_SERVER)))
	{
		if (m_ipPortHTTP != 0)
		{
			// initialize HTTP server
			if (!entry->initializeHTTP(m_ipPortHTTP))
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("SrvApp"),
						(_T("initializeHTTP failed for port#: %u"), m_ipPortHTTP));
				ret = FALSE;
			}
			else
			{
				entry->addXMLDAReceiver(m_urlDA);

				if (!m_webRootDirectory.isEmpty())
				{
					SOCmnTrace* trace = ::getSOCmnTrace();
					SOCMN_ALLOCATE_OBJECT_C(m_webTrace, SOSrvWebTrace(), (SOSrvWebTrace*));
					SOCmnList<SOCmnTraceReceiver> recList(trace->getReceiverList());
					trace->m_lock.lock();
					recList.add(m_webTrace);
					trace->m_lock.unlock();
					// add custom HTTP receiver
					SOCmnPointer<SOSrvHTTP> srvHTTP = entry->getHTTPServer();
					SOCmnPointer<SOSrvHTTPHandler> hndHTTP = srvHTTP->getHTTPHandler();

					if (hndHTTP.isNotNull())
					{
						SOCmnString szWWW, szWWWSofting;
						SOCMN_ALLOCATE_OBJECT_C(m_webServer, CWebServer(), (CWebServer*))
						hndHTTP->addHTTPReceiver(m_webServer);
						szWWW.format(_T("%s"), (LPCTSTR)m_webRootDirectory);
						szWWWSofting.format(_T("%s\\softing"), (LPCTSTR)m_webRootDirectory);
						m_webServer->setRootDirectories(szWWW, szWWWSofting);
					}
				}
			}
		}
	}

#else
#ifdef OTB_SERVER_DA

	if ((isDemoMode()) || (isLicensed(OTS_FEATURE_XMLDA_SERVER)))
	{
		if (m_ipPortHTTP != 0)
		{
			// initialize HTTP server
			if (!entryDA->initializeHTTP(m_ipPortHTTP))
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("SrvApp"),
						(_T("initializeHTTP failed for port#: %u"), m_ipPortHTTP));
				ret = FALSE;
			}
			else
			{
				entryDA->addXMLDAReceiver(m_urlDA);

				if (!m_webRootDirectory.isEmpty())
				{
					SOCmnTrace* trace = ::getSOCmnTrace();
					SOCMN_ALLOCATE_OBJECT_C(m_webTrace, SOSrvWebTrace(), (SOSrvWebTrace*));
					SOCmnList<SOCmnTraceReceiver> recList(trace->getReceiverList());
					trace->m_lock.lock();
					recList.add(m_webTrace);
					trace->m_lock.unlock();
					// add custom HTTP receiver
					SOCmnPointer<SOSrvHTTP> srvHTTP = entryDA->getHTTPServer();
					SOCmnPointer<SOSrvHTTPHandler> hndHTTP = srvHTTP->getHTTPHandler();

					if (hndHTTP.isNotNull())
					{
						SOCmnString szWWW, szWWWSofting;
						SOCMN_ALLOCATE_OBJECT_C(m_webServer, CWebServer(), (CWebServer*))
						hndHTTP->addHTTPReceiver(m_webServer);
						szWWW.format(_T("%s"), (LPCTSTR)m_webRootDirectory);
						szWWWSofting.format(_T("%s\\softing"), (LPCTSTR)m_webRootDirectory);
						m_webServer->setRootDirectories(szWWW, szWWWSofting);
					}
				}
			}
		}
	}

#endif
#endif
#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_TUNNEL
#ifdef OTB_SERVER_DAAE

	if ((isLicensed(OTS_FEATURE_TP_SERVER)) || (isDemoMode()))
	{
		// add TP Receiver
		if (m_tpPort != 0)
		{
			SOCmnList<SOCmnTPCredentials> permissibleCredentialsList;
			permissibleCredentialsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
			buildPermissibleCredentialsList(permissibleCredentialsList);

			if (!entry->initializeTP(m_tpPort, !m_tpUserList.isEmpty(), &permissibleCredentialsList))
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("SrvApp"),
						(_T("initializeTP failed for port#: %u"), m_tpPort));
				ret = FALSE;
			}
			else
			{
				entry->addTPDAReceiver();
				entry->addTPAEReceiver();
			}
		}
	}

#else
#ifdef OTB_SERVER_DA

	if ((isLicensed(OTS_FEATURE_TP_SERVER)) || (isDemoMode()))
	{
		// add TP Receiver
		if (m_tpPort != 0)
		{
			SOCmnList<SOCmnTPCredentials> permissibleCredentialsList;
			permissibleCredentialsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
			buildPermissibleCredentialsList(permissibleCredentialsList);

			if (!entryDA->initializeTP(m_tpPort, !m_tpUserList.isEmpty(), &permissibleCredentialsList))
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("SrvApp"),
						(_T("initializeTP failed for port#: %u"), m_tpPort));
				ret = FALSE;
			}
			else
			{
				entryDA->addTPDAReceiver();
			}
		}
	}

#else
#ifdef OTB_SERVER_AE

	if ((isLicensed(OTS_FEATURE_TP_SERVER)) || (isDemoMode()))
	{
		// add TP Receiver
		if (m_tpPort != 0)
		{
			SOCmnList<SOCmnTPCredentials> permissibleCredentialsList;
			permissibleCredentialsList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
			buildPermissibleCredentialsList(permissibleCredentialsList);

			if (!entry->initializeTP(m_tpPort, !m_tpUserList.isEmpty(), &permissibleCredentialsList))
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("SrvApp"),
						(_T("initializeTP failed for port#: %u"), m_tpPort));
				ret = FALSE;
			}
			else
			{
				entryAE->addTPAEReceiver();
			}
		}
	}

#endif
#endif
#endif
#endif // SOFEATURE_TUNNEL
	// set the servers identity information for registration
#ifdef OTB_SERVER_DA
#ifdef SOFEATURE_DCOM
	entryDA->setRegisterInfo(m_progIdDA, m_verIndProgIdDA, m_serverName, m_serviceName);
#endif
	entryDA->setVendorInfo(m_vendorInfo);
	entryDA->setVersionInfo(m_majorVersionNumber, m_minorVersionNumber, m_patchVersionNumber, m_buildNumber);
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
#ifdef SOFEATURE_DCOM
	entryAE->setRegisterInfo(m_progIdAE, m_verIndProgIdAE, m_serverName, m_serviceName);
#endif
	entryAE->setVendorInfo(m_vendorInfo);
	entryAE->setVersionInfo(m_majorVersionNumber, m_minorVersionNumber, m_patchVersionNumber, m_buildNumber);
	entryAE->setStringBasedObjectDataExpiry(m_stringBasedObjectDataExpiry);
#endif // OTB_SERVER_AE
#ifdef OTB_SERVER_DAAE
	// enable client watchdog and set check time to 30 seconds
	entry->setCheckParameter(m_clientCheckPeriod);
	entry->setStringBasedObjectDataExpiry(m_stringBasedObjectDataExpiry);
#endif
#ifdef OTB_SERVER_DA
	// enable client watchdog and set check time to 30 seconds
	entryDA->setCheckParameter(m_clientCheckPeriod);
	// set the minimal allowed update rate of a group (in milliseconds)
	entryDA->setMinUpdateRate(m_minUpdateRateDA);
	entryDA->setStringBasedObjectDataExpiry(m_stringBasedObjectDataExpiry);
#endif // OTB_SERVER_DA
	TCHAR accept[2];
	TCHAR bad[2];
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSNameSpaceRoot> nsRoot = entryDA->getNameSpaceRoot();
	// set delimiter signs
	accept[0] = m_addressSpaceDelimiter;
	accept[1] = _T('\0');
	bad[0] = m_propertyDelimiter;
	bad[1] = _T('\0');
	nsRoot->setTreeDelimiter(m_addressSpaceDelimiter, accept, bad);
	entryDA->setPropertyDelimiter(m_propertyDelimiter);
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSAreaSpaceRoot> asRoot = entryAE->getAreaSpaceRoot();
	asRoot->setTreeDelimiter(m_addressSpaceDelimiter, accept, bad);
#endif // OTB_SERVER_AE
	// set the OPC toolkit event handler
	SOCmnPointer<CEventHandler> eventHandler;
	SOCMN_ALLOCATE_OBJECT_C(eventHandler, CEventHandler(), (CEventHandler*))
#ifdef OTB_SERVER_DAAE
	entry->setEventHandler(eventHandler);
#else
#ifdef OTB_SERVER_DA
	entryDA->setEventHandler(eventHandler);
#endif
#ifdef OTB_SERVER_AE
	entryAE->setEventHandler(eventHandler);
#endif
#endif
	return ret;
} // prepare

#ifdef SOFEATURE_TUNNEL
// Iterate over configured server users (ConfigUser objects) and for each of them create a
// SOCmnTPCredentials object. The user name is meant to be clear text. What is called password
// here is meant to be the binhex string representation of the clear text password's hash.
void ServerApp::buildPermissibleCredentialsList(SOCmnList<SOCmnTPCredentials> &credentialsList)
{
	SOCmnListPosition posU, posP;
	posU = m_tpUserList.getStartPosition();
	posP = m_tpPwdList.getStartPosition();

	while ((posU) && (posP))
	{
		SOCmnString userName;
		SOCmnString pwd;
		SOCmnPointer<SOCmnTPCredentials> pCredentials;
		userName = m_tpUserList.getNext(posU);
		pwd = m_tpPwdList.getNext(posP);
		pCredentials = new SOCmnTPCredentials(userName, pwd);
		credentialsList.add(pCredentials);
	}
}
#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// start
//
// - start the OPC server I/O functionality
//
// returns:
//    TRUE  - OPC server I/O started
//    FALSE - error starting the OPC server I/O
//
BOOL ServerApp::start(void)
{
#ifdef OTB_SERVER_DAAE
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#else
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
#endif
#endif
	BOOL ret;

	if (m_startupEvent == NULL)
	{
		SOCMN_ALLOCATE_OBJECT(m_startupEvent, SOCmnEvent());
		m_startupEvent->create(TRUE);
	}

	// start the I/O via the OPC COM interfaces
	ret = entry->start();
	return ret;
} // start


//-----------------------------------------------------------------------------
// stop
//
// - stop the OPC server I/O functionality
//
void ServerApp::stop(SOCmnString reason)
{
#ifdef OTB_SERVER_DAAE
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
#endif
#ifdef SOFEATURE_DCOM
	DWORD waitTime = 0;
	SOCMN_ALLOCATE_OBJECT(m_pCanCloseEvent, SOCmnEvent());
	m_pCanCloseEvent->create();
	{
		// send a shutdown notification to all OPC clients
#ifdef OTB_SERVER_DA
		SOCmnPointer<SOSrvObjectRoot> rootDA = entryDA->getObjectRoot();
		SOCmnList<SOCmnElement> listDA(rootDA->getBranchList());
		SOCmnListPosition posDA;
		SODaSServer* pServer;
		posDA = listDA.getStartPosition();

		while (posDA)
		{
			pServer = (SODaSServer*)listDA.getNext(posDA);

			if (pServer->isDCOM())
			{
				entryDA->shutdownAllClients(reason);
				waitTime = 10000;
				break;
			}
		}

#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
		SOCmnPointer<SOSrvObjectRoot> rootAE = entryAE->getObjectRoot();
		SOCmnList<SOCmnElement> listAE(rootAE->getBranchList());

		// check for DCOM server releated objects
		if (listAE.getCount() > 0)
		{
			entryAE->shutdownAllClients(reason);
			waitTime = 10000;
		}

#endif // OTB_SERVER_AE
	} // end of block unlocks all lists!

	// wait for the clients to disconnect
	if (waitTime)
	{
		m_pCanCloseEvent->waitFor(waitTime);
	}

	delete m_pCanCloseEvent;
	m_pCanCloseEvent = NULL;
#endif // SOFEATURE_DCOM
	// stop the I/O via the OPC interfaces
#ifdef OTB_SERVER_DAAE
	entry->stop();
//#else
#ifdef OTB_SERVER_DA
	entryDA->stop();
#endif
#ifdef OTB_SERVER_AE
	entryAE->stop();
#endif
#endif

	if (m_startupEvent)
	{
		delete m_startupEvent;
		m_startupEvent = NULL;
	}
} // stop


//-----------------------------------------------------------------------------
// unload
//
// - unload the OPC server configuration
//
void ServerApp::unload(void)
{
#ifdef OTB_SERVER_AE
	m_alarmList.destroy();
#endif // OTB_SERVER_AE
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> nsRoot = entryDA->getNameSpaceRoot();
	// destroy DA name space
	nsRoot->removeAllAdditionalElements();
	nsRoot->removeAllLeaves();
	nsRoot->removeAllBranches();
	// destroy the public groups
	SOCmnPointer<SODaSServer> publicGroups;

	if ((SODaSServer*)(publicGroups = entryDA->getPublicGroups()))
	{
		publicGroups->removeAllLeafs();
		publicGroups->removeAllBranches();
	}

#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entryAeS = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> esRoot = entryAeS->getEventSpaceRoot();
	SOCmnPointer<SOAeSAreaSpaceRoot> asRoot = entryAeS->getAreaSpaceRoot();
	esRoot->removeAllAdditionalElements();
	esRoot->removeAllLeaves();
	esRoot->removeAllBranches();
	asRoot->removeAllAdditionalElements();
	asRoot->removeAllLeaves();
	asRoot->removeAllBranches();
#endif // OTB_SERVER_AE
} // unload


//-----------------------------------------------------------------------------
// finalize
//
// - finalize the OPC toolkit
//
void ServerApp::finalize(void)
{
#ifdef OTB_SERVER_DAAE
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#else
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
#endif
#endif
#ifdef OTB_XMLDA_SERVER
	m_webTrace.release();
	m_webServer.release();
#endif // SOFEATURE_SOAP
	// free all OPC toolkit objects
	entry->terminate();
	entry.release();
} // terminateEntry


//-----------------------------------------------------------------------------
// terminate
//
// - terminate the application
//
void ServerApp::terminate(void)
{
#ifdef OTB_SERVER_DA
	m_itemTagChangedList.destroy();
#endif // OTB_SERVER_DA
} // terminate


#ifdef OTB_XMLDA_SERVER

SOSrvWebTrace* ServerApp::getWebTrace(void)
{
	m_webTrace.addRef();
	return m_webTrace;
}

#endif // SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// processCommandLine
//
// - process the command line
//
// returns:
//    TRUE  - don't start server - registration/unregistration
//    FALSE - start server
//
BOOL ServerApp::processCommandLine(
	IN LPCTSTR cmd)     // command line string
{
	if (_tcslen(cmd) == 0)
	{
		return FALSE;
	}

	SOCmnString cmdLine(cmd);
	BOOL end;
	end = FALSE;
	cmdLine.makeLower();
#ifdef OTB_SERVER_DAAE
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef OTB_SERVER_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
#endif
	m_startedByCOM = FALSE;

	if ((_tcsstr(cmdLine, _T("-regserver")) != NULL) ||
		(_tcsstr(cmdLine, _T("/regserver")) != NULL))
	{
		// register OPC Server
#ifdef SOFEATURE_DCOM
#ifdef OTB_SERVER_DAAE
		entry->registerServer();
#else
#ifdef OTB_SERVER_DA
		entryDA->registerServer();
#endif
#ifdef OTB_SERVER_AE
		entryAE->registerServer();
#endif
#endif
#ifdef OTB_SERVER_DA
		entryDA->registerCategories();
#endif
#ifdef OTB_SERVER_AE
		entryAE->registerCategories();
#endif
#endif
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE

		if (m_serviceName.isEmpty())
		{
			unregisterService();
		}
		else
		{
			registerService();
		}

#endif
#endif // SOOS_WINDOWS
		end = TRUE;
	}
	else if ((_tcsstr(cmdLine, _T("-unregserver")) != NULL) ||
			 (_tcsstr(cmdLine, _T("/unregserver")) != NULL))
	{
		// unregister OPC Server
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE

		if (!m_serviceName.isEmpty())
		{
			unregisterService();
		}

#endif
#endif // SOOS_WINDOWS
#ifdef SOFEATURE_DCOM
#ifdef OTB_SERVER_DAAE
		entry->unregisterServer();
#else
#ifdef OTB_SERVER_DA
		entryDA->unregisterServer();
#endif
#ifdef OTB_SERVER_AE
		entryAE->unregisterServer();
#endif
#endif
#ifdef OTB_SERVER_DA
		entryDA->unregisterCategories();
#endif
#ifdef OTB_SERVER_AE
		entryAE->unregisterCategories();
#endif
#endif
		end = TRUE;
	}
	else if ((_tcsstr(cmdLine, _T("-embedding")) != NULL) ||
			 (_tcsstr(cmdLine, _T("/embedding")) != NULL) ||
			 (_tcsstr(cmdLine, _T("-embedded")) != NULL)  ||
			 (_tcsstr(cmdLine, _T("/embedded")) != NULL))
	{
		// started via COM
		m_startedByCOM = TRUE;
	}

	return end;
} // processCommandLine


#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE

//-----------------------------------------------------------------------------
// registerService
//
// - create service entries in registry
//
// returns:
//    TRUE  - registered service
//    FALSE - error during registration
//

BOOL ServerApp::registerService(void)
{
	SC_HANDLE hService;
	SC_HANDLE hSCM;
	TCHAR path[_MAX_PATH];
	BOOL ret = FALSE;

	if (GetModuleFileName(NULL, path, _MAX_PATH) == 0)
	{
		return FALSE;
	}

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		hService = CreateService(hSCM, m_serviceName, m_serviceName,
								 SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
								 SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

		if (hService)
		{
			// installed service
			if (!m_serviceDescription.IsEmpty())
			{
				// set service description
				SERVICE_DESCRIPTION description;
				description.lpDescription = m_serviceDescription;
				ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &description);
			}
			CloseServiceHandle(hService);
			ret = TRUE;
		}
		else
		{
			// error
			if (GetLastError() == ERROR_SERVICE_EXISTS)
			{
				// this service already exsists
				hService = OpenService(hSCM, m_serviceName, SERVICE_ALL_ACCESS);

				if (hService)
				{
					BYTE buffer[1000];
					QUERY_SERVICE_CONFIG* pServiceConfig = (QUERY_SERVICE_CONFIG*)buffer;
					DWORD bn;

					if (QueryServiceConfig(hService, pServiceConfig, 1000, &bn))
					{
						if (_tcscmp(pServiceConfig->lpDisplayName, m_serviceName) == 0)
						{
							// same service -> change the service configuration
							stopService(hService);

							if (ChangeServiceConfig(hService, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
													SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"),
													NULL, NULL, m_serviceName))
							{
								if (!m_serviceDescription.IsEmpty())
								{
									// set service description
									SERVICE_DESCRIPTION description;
									description.lpDescription = m_serviceDescription;
									ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &description);
								}
								ret = TRUE;
							}
						}
					}

					CloseServiceHandle(hService);
				}
			}
		}

		CloseServiceHandle(hSCM);
	}

	return ret;
} // registerService


//-----------------------------------------------------------------------------
// unregisterService
//
// - stops service
// - remove service entries in registry
//
// returns:
//    TRUE  - unregistered service
//    FALSE - error during unregistration
//
BOOL ServerApp::unregisterService(void)

{
	SC_HANDLE hService;
	SC_HANDLE hSCM;
	BOOL ret = FALSE;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		hService = OpenService(hSCM, m_serviceName, SERVICE_ALL_ACCESS);

		if (hService)
		{
			stopService(hService);

			// now remove the service
			if (DeleteService(hService))
			{
				ret = TRUE;
			}

			CloseServiceHandle(hService);
		}

		CloseServiceHandle(hSCM);
	}

	return ret;
} // unregisterService

//-----------------------------------------------------------------------------
// stopService
//
// - stop the service
//
void ServerApp::stopService(
	IN SC_HANDLE hService) // service handle
{
	// try to stop the service
	if (ControlService(hService, SERVICE_CONTROL_STOP, &m_serviceStatus))
	{
		while (QueryServiceStatus(hService, &m_serviceStatus))
		{
			if (m_serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				Sleep(100);
			}
			else
			{
				break;
			}
		}
	}
} // stopService

#endif // SOOS_WINDOWS_CE
#endif // SOOS_WINDOWS

//-----------------------------------------------------------------------------
// CEventHandler
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// onReleasedLastOPCObject
//
// - called after last OPC object was released
//
void CEventHandler::onReleasedLastOPCObject(void)
{
	ServerApp* pServerApp = getServerApp();

	if (pServerApp->m_pCanCloseEvent)
	{
		// set shutdown close event - event only available during server shutdown
		pServerApp->m_pCanCloseEvent->signal();
	}

#ifdef SOFEATURE_DCOM

	if (pServerApp->m_startedByCOM)
	{
		// started via COM
#ifdef OTB_SERVER_DAAE
		SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#else
#ifdef OTB_SERVER_DA
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
#endif
#ifdef OTB_SERVER_AE
		SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
#endif
#endif

		// -> try to close application if last object was released
		if (S_OK == entry->canUnloadNow())
		{
			if (pServerApp->m_appType == SOSrvEntry::OutProc)
			{
				// OutProc
				entry->revokeClassObjects();

				if (pServerApp->m_callbackFunctions.m_OTSShutdown)
				{
					SOCMN_TRY
					{
						pServerApp->m_callbackFunctions.m_OTSShutdown();
					}
					SOCMN_CATCH(...)
					{
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("CEventHandler::onReleasedLastOPCObject - Exception occurred in vendor application callback: OTSShutdown")));
					}
				}
			}
		}
	}

#endif
} // onReleasedLastOPCObject

#endif // OTB_SERVER

