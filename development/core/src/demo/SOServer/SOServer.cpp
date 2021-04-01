//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : SOServer.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Main Demo Server source                                     |
//                All Functions +                                             |
//                Classes:                                                    |
//                - SODmSEventHandler: event handler class                    |
//                - SODmSTagAccept: tag accepting values                      |
//                - SODmSTagTime: tag of time namespace subtree               |
//                - SODmSTagInc: tag of increment namespace subtree           |
//                - SODmSTagMaths: tag of maths namespace subtree             |
//                - SODmSNodeWatch: node of watch subtree                     |
//                - SODmSTagWatch: tag of watch subtree                       |
//                - SODmSConditionTimeSlot: time slot condition               |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#ifdef SOOS_LINUX
#include <signal.h>
#endif
#include <math.h>
#include "SOServer.h"
#include "SOVersion.h"
#ifdef SOFEATURE_SOAP
#ifndef SOFLAG_DEMO_VERSION
#include "SODaSXmlServer.h"
#endif
#endif

#ifdef SOOS_WINDOWS_CE
//#define SODMS_HEADLESS
#endif

// window class of application window
#define WINDOW_CLASS _T("Softing OPC Classic Demo Server")

// category IDs
#define SODMS_AE_CATEGORTY_TIME_TICK    1
#define SODMS_AE_CATEGORTY_TIME_SLOT    2
#define SODMS_AE_CATEGORTY_MOUSECLICK   3

// attribute IDs
#define SODMS_AE_ATTRIBUTE_CURRENT_TIME   1
#define SODMS_AE_ATTRIBUTE_CURRENT_SECOND 2
#define SODMS_AE_ATTRIBUTE_START_SECOND   3
#define SODMS_AE_ATTRIBUTE_END_SECOND     4
#define SODMS_AE_ATTRIBUTE_POSX           5
#define SODMS_AE_ATTRIBUTE_POSY           6

// property IDs
#define SODMS_DA_PROPERTY_INC_ON            5000
#define SODMS_DA_PROPERTY_INC_BY            6010
#define SODMS_DA_PROPERTY_ANGLE             6020

// tag classes
#define SODMS_DA_TAG_ACCEPT 1
#define SODMS_DA_TAG_TIME   2
#define SODMS_DA_TAG_INC    3
#define SODMS_DA_TAG_MATHS  4
#define SODMS_DA_TAG_WATCH  5
#define SODMS_DA_TAG_ERROR  6
#define SODMS_DA_TAG_MODE   7
#define SODMS_DA_TAG_HIDDEN 8
#define SODMS_DA_TAG_CLIENT 9
#define SODMS_DA_TAG_SECURE 10


//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

#ifdef SOOS_WINDOWS
// application instance handle
static HINSTANCE g_instance = NULL;
// act as NT service
static BOOL g_service = FALSE;
#endif

#ifdef SOFEATURE_DCOM
// can close application during shutdown event
static SOCmnEvent g_canCloseEvent;
#endif

// simmulation thread
static SOCmnThread* g_simThread;

// I/O mode handling thread
#ifdef SODMS_DA
static SOCmnThread* g_ioThread;
static DWORD g_requestQueueEventId = SOCMNEVENT_INVALID_ID;
static SOCmnList<SODaSRequest> g_requestQueue; // request queue for asynchronous request
#endif

#ifdef SODMS_DA
static SOCmnPointer<SODaSNode> g_modeReportNode;
#endif

// turn of increment of increment items
static BOOL g_incrementOn = TRUE;

#ifdef SOFEATURE_DCOM
// class id of OPC DA Server {2E565242-B238-11d3-842D-0008C779D775}
static CLSID g_clsidDA = {0x2e565242, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } };
// class id of OPC AE Server {2E565243-B238-11d3-842D-0008C779D775}
static CLSID g_clsidAE = {0x2e565243, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } };
#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP
WORD g_port = 8079; // HTTP port
#endif
#ifdef SOFEATURE_TUNNEL
WORD g_tpPort = TP_DEFAULT_PORT; // TP port
#endif

#ifdef SOOS_WINDOWS
static HBITMAP g_hBitmap = NULL;
#endif

// CTT mode
static BOOL g_ctt = FALSE;

#ifdef SODMS_DA
static BOOL g_opcSpecSupport = 0x07;
#endif

//-----------------------------------------------------------------------------
// Functions                                                                  |
//-----------------------------------------------------------------------------

void buildObjectSpaces(void);
void buildPublicGroups(void);
DWORD _stdcall simThread(IN LPVOID context);
#ifdef SODMS_DA
DWORD _stdcall ioThread(IN LPVOID context);
#endif
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE
BOOL unregisterService(void);
BOOL registerService(void);
#endif
#endif

#ifdef SOOS_WINDOWS
//-----------------------------------------------------------------------------
// getInstanceHandle
//
// - get the instance handle of the server
//
// returns:
//		instance handle
//
HINSTANCE getInstanceHandle(void)
{
	return g_instance;
} // getInstanceHandle
#endif


//-----------------------------------------------------------------------------
// initializeApp
//
// - initialize the application
//
void initializeApp(void)
{
#ifdef SOFLAG_NO_DLLS
	// initialize global objects
	initSOCmn();
#endif
	g_simThread = new SOCmnThread();
#ifdef SODMS_DA
	g_ioThread = new SOCmnThread();
#endif
	SOCmnTrace* trace = ::getSOCmnTrace();
	// set the applications trace handler
	SOCmnPointer<SODmSTraceHandler> traceHandler;
	SOCMN_ALLOCATE_OBJECT_C(traceHandler, SODmSTraceHandler(), (SODmSTraceHandler*));
	trace->setTraceHandler(traceHandler);
	traceHandler->loadTraceOptions();
} // initializeApp


//-----------------------------------------------------------------------------
// initializeEntry
//
// - initialize the toolkit entry objects
//
void initializeEntry(void)
{
#ifdef SODMS_ALL
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef SODMS_AE
	SOCmnPointer<SOAeSEntry> entryAE;
#endif
#ifdef SODMS_DA
	SOCmnPointer<SODaSEntry> entryDA;
#endif
#ifdef SODMS_ALL
	entryDA = entry->getDAEntry();
	entryAE = entry->getAEEntry();
#else
#ifdef SODMS_DA
	entryDA = getSODaSEntry();
#endif
#ifdef SODMS_AE
	entryAE = getSOAeSEntry();
#endif
#endif
#ifdef SODMS_DA
	SOCmnPointer<SODmSCreatorDA> creatorDA = new SODmSCreatorDA();
	entryDA->setCreator(creatorDA);
	creatorDA.release();
#endif
#ifdef SODMS_AE
	SOCmnPointer<SODmSCreatorAE> creatorAE = new SODmSCreatorAE();
	entryAE->setCreator(creatorAE);
	creatorAE.release();
#endif
	DWORD authnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	DWORD impLevel = RPC_C_IMP_LEVEL_IDENTIFY;
	DWORD cbAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	DWORD cbImpLevel = RPC_C_IMP_LEVEL_IDENTIFY;
	DWORD capabilities = EOAC_NONE;

	// read registry settings
	{
		DWORD sizeDWORD = sizeof(DWORD);
		HKEY keyServer = NULL;

		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Server"),
			NULL, KEY_READ, &keyServer))
		{
			RegQueryValueEx(keyServer, _T("AuthLevel"), NULL, NULL, (LPBYTE)&authnLevel, &sizeDWORD);
			RegQueryValueEx(keyServer, _T("ImpLevel"), NULL, NULL, (LPBYTE)&impLevel, &sizeDWORD);
			RegQueryValueEx(keyServer, _T("CbAuthLevel"), NULL, NULL, (LPBYTE)&cbAuthnLevel, &sizeDWORD);
			RegQueryValueEx(keyServer, _T("CbImpLevel"), NULL, NULL, (LPBYTE)&cbImpLevel, &sizeDWORD);
			RegCloseKey(keyServer);
		}
	}

#ifdef SODMS_ALL
#ifdef SOFEATURE_DCOM
	entry->initialize(SOSrvEntry::OutProc, g_clsidDA, g_clsidAE, g_instance, COINIT_MULTITHREADED);
	entry->initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
#endif
#else
#ifdef SODMS_DA
#ifdef SOFEATURE_DCOM
	entryDA->initialize(SOSrvEntry::OutProc, g_clsidDA, g_instance, COINIT_MULTITHREADED);
	entryDA->initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
#endif
#endif
#ifdef SODMS_AE
#ifdef SOFEATURE_DCOM
	entryAE->initialize(SOSrvEntry::OutProc, g_clsidAE, g_instance, COINIT_MULTITHREADED);
	entryAE->initializeSecurity(authnLevel, impLevel, cbAuthnLevel, cbImpLevel, capabilities);
#endif
#endif
#endif
#ifdef SODMS_DA
#ifdef SOFEATURE_DCOM
	entryDA->setRegisterInfo(_T("Softing.OPCToolboxDemo_ServerDA.1"), // ProdId of OPC DA Server
							 _T("Softing.OPCToolboxDemo_ServerDA"),                    // version independent ProdId of OPC DA Server
							 _T("Softing OPC Classic Demo OPC DA Server"), NULL);      // description of OPC DA Server
#endif
	entryDA->setVendorInfo(_T("Softing OPC Classic Demo Server - Copyright (C) Softing Industrial Automation GmbH 1998-2019, All Rights Reserved"));
	entryDA->setVersionInfo(MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER);
#endif
#ifdef SOFEATURE_DCOM
#ifdef SODMS_AE
	entryAE->setRegisterInfo(_T("Softing.OPCToolboxDemo_ServerAE.1"), // ProdId of OPC AE Server
							 _T("Softing.OPCToolboxDemo_ServerAE"),                    // version independent ProdId of OPC AE Server
							 _T("Softing OPC Classic Demo OPC AE Server"), NULL);      // description of OPC AE Server
	entryAE->setVendorInfo(_T("Softing OPC Classic Demo Server - Copyright (C) Softing Industrial Automation GmbH 1998-2019, All Rights Reserved"));
	entryAE->setVersionInfo(MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER);
#endif
#endif
} // initializeEntry


//-----------------------------------------------------------------------------
// initializeServer
//
// - create the object spaces and start the toolkit
//
// returns:
//		TRUE  - toolkit started
//		FALSE - toolkit not started
//
BOOL initializeServer(void)
{
#ifdef SODMS_ALL
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef SODMS_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
#endif
#ifdef SODMS_AE
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
#endif
	BOOL ret;
#ifdef SODMS_ALL
#ifdef SOFEATURE_SOAP
	entry->initializeHTTP(g_port);
	entry->addXMLDAReceiver(_T("/OPC/DA"));
#endif
#ifdef SOFEATURE_TUNNEL
	entry->initializeTP(g_tpPort, FALSE, NULL);
	entry->addTPDAReceiver();
	entry->addTPAEReceiver();
#endif
#else
#ifdef SODMS_DA
#ifdef SOFEATURE_SOAP
	entryDA->initializeHTTP(g_port);
	entryDA->addXMLDAReceiver(_T("/OCP/DA"));
#endif
#ifdef SOFEATURE_TUNNEL
	entryDA->initializeTP(g_tpPort);
	entryDA->addTPDAReceiver();
#endif
#endif
#ifdef SODMS_AE
#ifdef SOFEATURE_TUNNEL
	entryAE->initializeTP(g_tpPort);
	entryAE->addTPAEReceiver();
#endif
#endif
#endif
#ifdef SODMS_ALL
	entry->setCheckParameter(30000, TRUE);
#else
#ifdef SODMS_DA
	entryDA->setCheckParameter(30000);
#endif
#ifdef SODMS_AE
	entryAE->setCheckParameter(30000);
#endif
#endif
#ifdef SODMS_DA

	if (!g_ctt)
	{
		entryDA->setMinUpdateRate(10);
	}
	else
	{
		entryDA->setMinUpdateRate(1000);
	}

#endif
	// build DA name space, AE event space and AE area space
	buildObjectSpaces();
	// create the public DA groups of the server
	buildPublicGroups();
#ifdef SODMS_AE
	// enable condition survey
	entryAE->enableConditionSurvey(TRUE);
	entryAE->setAutoServerState();
#endif
#ifdef SODMS_DA
	// request queue
	g_requestQueue.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	entryDA->setAutoServerState();
#endif
	// start simulation thread
	g_simThread->start(simThread, NULL);
#ifdef SODMS_DA
	g_requestQueueEventId = g_ioThread->createEvent();
	g_ioThread->start(ioThread, NULL);
#endif
	// start entry
#ifdef SODMS_ALL
	ret = entry->start();
#else
#ifdef SODMS_DA
	ret = entryDA->start();
#endif
#ifdef SODMS_AE
	ret = entryAE->start();
#endif
#endif
#ifdef SODMS_AE
	// connect AE attribute upate via DA
	entryAE->connectAttributeValueUpdate();
#endif
	return ret;
} // initializeServer


//-----------------------------------------------------------------------------
// processCommandLine
//
// - process the command line
//
// returns:
//		TRUE  - start server
//		FALSE - don't start server - registration
//
BOOL processCommandLine(
	IN LPCTSTR cL,      // command line string
	OUT BOOL* startedByCOM) // application started by COM?
{
	SOCmnString cmdLine(cL);
	BOOL end = FALSE;
#ifdef SOOS_WINDOWS

	if (cmdLine.isEmpty())
	{
#ifndef SOOS_WINDOWS_CE
		// set current directory to the directory of the executable
		TCHAR exePathSZ[_MAX_PATH];
		TCHAR* exePathDir;
		GetModuleFileName(getInstanceHandle(), exePathSZ, _MAX_PATH);

		if (exePathDir = _tcsrchr(exePathSZ, _T('\\')))
		{
			*(exePathDir) = _T('\0');
			SetCurrentDirectory(exePathSZ);
		}

#endif
		// open configuration file
		HANDLE hFile = CreateFile((LPCTSTR)_T("SOServerCmdLine.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			// calculate the size of the file
			DWORD fileSize;

			if ((fileSize = ::GetFileSize(hFile, NULL)) != INVALID_FILE_SIZE)
			{
				// get the contents of the file
				BYTE* pData = (BYTE*)malloc(fileSize + 1);
				DWORD lenData;

				if (ReadFile(hFile, pData, fileSize, &lenData, NULL))
				{
					USES_CONVERSION;
					pData[lenData] = _T('\0');
					cmdLine = A2T((CHAR*)pData);
				}

				free(pData);
			}

			CloseHandle(hFile);
		}
	}

#endif
	cmdLine.makeLower();
#ifdef SOFEATURE_DCOM
#ifdef SODMS_ALL
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#endif
#ifdef SODMS_AE
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
#endif
#ifdef SODMS_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
#endif
	end = FALSE;
	*startedByCOM = FALSE;

	if ((cmdLine.find(_T("-service")) != -1) || (cmdLine.find(_T("/service")) != -1))
	{
		// act as NT service
		g_service = TRUE;
#ifdef SODMS_ALL
		entry->setServiceName(_T("SOServer"));
#endif
#ifdef SODMS_AE
		entryAE->setServiceName(_T("SOServer"));
#endif
#ifdef SODMS_DA
		entryDA->setServiceName(_T("SOServer"));
#endif
	}

	if ((cmdLine.find(_T("-regserver")) != -1) || (cmdLine.find(_T("/regserver")) != -1))
	{
		// register OPC Server
#ifndef SOOS_WINDOWS_CE
		if (!g_service)
		{
			unregisterService();
		}
		else
		{
			registerService();
		}

#endif
#ifdef SODMS_ALL
		entry->registerServer();
#else
#ifdef SODMS_DA
		entryDA->registerServer();
#endif
#ifdef SODMS_AE
		entryAE->registerServer();
#endif
#endif
#ifdef SODMS_DA
		entryDA->registerCategories();
#endif
#ifdef SODMS_AE
		entryAE->registerCategories();
#endif
		end = TRUE;
	}
	else    if ((cmdLine.find(_T("-unregserver")) != -1) || (cmdLine.find(_T("/unregserver")) != -1))
	{
		// unregister OPC Server
#ifdef SODMS_ALL
		entry->unregisterServer();
#else
#ifdef SODMS_DA
		entryDA->unregisterServer();
#endif
#ifdef SODMS_AE
		entryAE->unregisterServer();
#endif
#endif
#ifdef SODMS_DA
		entryDA->unregisterCategories();
#endif
#ifdef SODMS_AE
		entryAE->unregisterCategories();
#endif
#ifndef SOOS_WINDOWS_CE
		unregisterService();
#endif
		end = TRUE;
	}
	else if ((cmdLine.find(_T("-embedding")) != -1) || (cmdLine.find(_T("/embedding")) != -1) ||
			 (cmdLine.find(_T("-embedded")) != -1) || (cmdLine.find(_T("/embedded")) != -1))
	{
		*startedByCOM = TRUE;
	}

#endif // SOFEATURE_DCOM
	int pos = 0, pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
#ifdef SOFEATURE_SOAP

	if (((pos1 = cmdLine.find(_T("-port:"))) != -1) || ((pos2 = cmdLine.find(_T("/port:"))) != -1) ||
		((pos3 = cmdLine.find(_T("-httpport:"))) != -1) || ((pos4 = cmdLine.find(_T("/httpport:"))) != -1))
	{
		if (pos1 != -1)
		{
			pos = pos1;
		}
		else if (pos2 != -1)
		{
			pos = pos2;
		}
		else if (pos3 != -1)
		{
			pos = pos3;
		}
		else if (pos4 != -1)
		{
			pos = pos4;
		}

		SOCmnString portNr = cmdLine.right(cmdLine.getLength() - pos - 6);
		g_port = (WORD)_tcstoul(portNr, NULL, 10);
	}

#endif
#ifdef SOFEATURE_TUNNEL

	if (((pos1 = cmdLine.find(_T("-tpport:"))) != -1) || ((pos2 = cmdLine.find(_T("/tpport:"))) != -1))
	{
		if (pos1 != -1)
		{
			pos = pos1;
		}
		else
		{
			pos = pos2;
		}

		SOCmnString portNr = cmdLine.right(cmdLine.getLength() - pos - 6);
		g_tpPort = (WORD)_tcstoul(portNr, NULL, 10);
	}

#endif
#ifdef SODMS_DA

	if ((cmdLine.find(_T("-ctt")) != -1) || (cmdLine.find(_T("/ctt")) != -1))
	{
		SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
		g_ctt = TRUE;
	}

	if ((cmdLine.find(_T("-da3")) != -1) || (cmdLine.find(_T("/da3")) != -1))
	{
		g_opcSpecSupport = 0x04;
	}
	else if ((cmdLine.find(_T("-da2")) != -1) || (cmdLine.find(_T("/da2")) != -1))
	{
		g_opcSpecSupport = 0x02;
	}
	else if ((cmdLine.find(_T("-da1")) != -1) || (cmdLine.find(_T("/da1")) != -1))
	{
		g_opcSpecSupport = 0x01;
	}

#endif
	return end;
} // processCommandLine


//-----------------------------------------------------------------------------
// terminateEntry
//
// - terminate the toolkit entry objects
//
void terminateEntry(void)
{
#ifdef SOFEATURE_DCOM
	DWORD waitTime = 0;
	{
#ifdef SODMS_DA
		SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
		SOCmnPointer<SOSrvObjectRoot> rootDA = entryDA->getObjectRoot();
		SOCmnList<SOCmnElement> listDA(rootDA->getBranchList());
		SOCmnListPosition posDA;
		SODaSServer* pServer;
#endif
#ifdef SODMS_AE
		SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
		SOCmnPointer<SOSrvObjectRoot> rootAE = entryAE->getObjectRoot();
		SOCmnList<SOCmnElement> listAE(rootAE->getBranchList());
#endif
#ifdef SODMS_AE

		if (listAE.getCount() > 0)
		{
			entryAE->shutdownAllClients(_T("Server shutdown"));
			waitTime = 10000;
		}

#endif
#ifdef SODMS_DA
		// check for DCOM server releated objects
		posDA = listDA.getStartPosition();

		while (posDA)
		{
			pServer = (SODaSServer*)listDA.getNext(posDA);

			if (pServer->isDCOM())
			{
				entryDA->shutdownAllClients(_T("Server shutdown"));
				waitTime = 10000;
				break;
			}
		}

#endif
	}

	if (waitTime)
	{
		g_canCloseEvent.waitFor(waitTime);    // give the clients time to disconnect
	}

#endif // SOFEATURE_DCOM
#ifdef SODMS_ALL
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#else
#ifdef SODMS_DA
	SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
#endif
#ifdef SODMS_AE
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
#endif
#endif
	entry->terminate();
	entry.release();
} // terminateEntry


//-----------------------------------------------------------------------------
// terminateApp
//
// - terminate the application
//
void terminateApp(void)
{
	delete g_simThread;
#ifdef SODMS_DA
	delete g_ioThread;
	g_requestQueue.destroy();
#endif
#ifdef SOFLAG_NO_DLLS
	// destroy global objects
	exitSOCmn();
#endif
} // terminateApp


//-----------------------------------------------------------------------------
// terminateServer
//
// - terminate server simmulation thread
//
void terminateServer(void)
{
	g_simThread->stop();
#ifdef SODMS_DA
	g_ioThread->stop();
#endif
#ifdef SODMS_DA
	g_modeReportNode.release();
#endif
} // terminateServer

#ifdef SOOS_WINDOWS

#ifndef SOOS_WINDOWS_CE
SERVICE_STATUS m_serviceStatus;// current status of the service
SERVICE_STATUS_HANDLE m_serviceStatusHandle;// status handle of the service
HANDLE m_serviceStopEvent;    // service stop event

//-----------------------------------------------------------------------------
// reportStatusToSCMgr
//
// - report status to the SCM
//
// returns:
//		TRUE  - reported
//		FALSE - error setting the service status
//
BOOL reportStatusToSCMgr(
	IN DWORD currentState, // state
	IN DWORD waitHint)     // max wait time
{
	static DWORD checkPoint = 1;
	m_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	m_serviceStatus.dwCurrentState = currentState;
	m_serviceStatus.dwWaitHint = waitHint;

	if ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED))
	{
		m_serviceStatus.dwCheckPoint = 0;
	}
	else
	{
		m_serviceStatus.dwCheckPoint = checkPoint++;
	}

	// report the status of the service to the service control manager.
	return SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);
} // reportStatusToSCMgr


//-----------------------------------------------------------------------------
// serviceControlHandler
//
// - control handler for the SCM
//
void WINAPI serviceControlHandler(
	IN DWORD ctrlCode) // requested control type
{
	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP:     // stop the service.
	case SERVICE_CONTROL_SHUTDOWN: // system shutdown
	{
		reportStatusToSCMgr(SERVICE_STOP_PENDING, 0);

		if (m_serviceStopEvent)
		{
			SetEvent(m_serviceStopEvent);    // stop the service
		}
	}
	break;
	}
} // serviceControlHandler


//-----------------------------------------------------------------------------
// ServiceMain
//
// - perform initialization of the service
// - start service
//
void WINAPI ServiceMain(
	IN DWORD argc,    // number of command line arguments
	IN LPTSTR* argv)  // array of command line arguments
{
	// register service control handler:
	m_serviceStatusHandle = RegisterServiceCtrlHandler(_T("SOServer"), serviceControlHandler);

	if (m_serviceStatusHandle)
	{
		m_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		m_serviceStatus.dwServiceSpecificExitCode = 0;
		m_serviceStatus.dwWin32ExitCode = 0;
		reportStatusToSCMgr(SERVICE_START_PENDING, 10000);
		m_serviceStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		BOOL started = FALSE;

		// load the OPC server configuration
		if (initializeServer())
		{
			started = TRUE;
			reportStatusToSCMgr(SERVICE_RUNNING, 0);
			WaitForSingleObject(m_serviceStopEvent, INFINITE);
			reportStatusToSCMgr(SERVICE_STOP_PENDING, 0);
			// unload the configuration
			terminateServer();
		}

		if (!started)
		{
			m_serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
			m_serviceStatus.dwServiceSpecificExitCode = (DWORD) - 4;
		}

		CloseHandle(m_serviceStopEvent);
	}

	// finalize OPC toolkit and terminate the application
	terminateEntry();
	reportStatusToSCMgr(SERVICE_STOPPED, 0);
	terminateApp();
} // ServiceMain

//-----------------------------------------------------------------------------
// stopService
//
// - stop the service
//
void stopService(
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


//-----------------------------------------------------------------------------
// registerService
//
// - create service entries in registry
//
// returns:
//    TRUE  - registered service
//    FALSE - error during registration
//
BOOL registerService(void)
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
		hService = CreateService(hSCM, _T("SOServer"), _T("Softing OPC Classic Demo Server"),
								 SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
								 SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

		if (hService)
		{
			// installed service
			CloseServiceHandle(hService);
			ret = TRUE;
		}
		else
		{
			// error
			if (GetLastError() == ERROR_SERVICE_EXISTS)
			{
				// this service already exsists
				hService = OpenService(hSCM, _T("SOServer"), SERVICE_ALL_ACCESS);

				if (hService)
				{
					BYTE buffer[1000];
					QUERY_SERVICE_CONFIG* pServiceConfig = (QUERY_SERVICE_CONFIG*)buffer;
					DWORD bn;

					if (QueryServiceConfig(hService, pServiceConfig, 1000, &bn))
					{
						if (_tcscmp(pServiceConfig->lpDisplayName, _T("Softing OPC Classic Demo Server")) == 0)
						{
							// same service -> change the service configuration
							stopService(hService);

							if (ChangeServiceConfig(hService, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
													SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"),
													NULL, NULL, _T("Softing OPC Classic Demo Server")))
							{
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
BOOL unregisterService(void)
{
	SC_HANDLE hService;
	SC_HANDLE hSCM;
	BOOL ret = FALSE;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		hService = OpenService(hSCM, _T("SOServer"), SERVICE_ALL_ACCESS);

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

#endif

#ifndef SODMS_HEADLESS

//-----------------------------------------------------------------------------
// wndProc
//
// - processes messages for the main window.
//
// returns:
//		0
//
LRESULT CALLBACK wndProc(
	IN HWND wnd,        // window handle
	IN UINT message,    // message id
	IN WPARAM wParam,   // 1st parameter
	IN LPARAM lParam)   // 2nd parameter
{
	switch (message)
	{
	case WM_PAINT:
	{
		// paint the main window
		PAINTSTRUCT ps;
		HDC hdc;
		HDC hdcBits;
		BITMAP bm;
		hdc = BeginPaint(wnd, &ps);
		hdcBits = CreateCompatibleDC(hdc);
		GetObject(g_hBitmap, sizeof(BITMAP), (LPSTR)&bm);
		SelectObject(hdcBits, g_hBitmap);
		BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcBits, 0, 0, SRCCOPY);
		DeleteDC(hdcBits);
#ifndef SOOS_WINDOWS_CE
		HFONT bFont = CreateFont(13, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Verdana"));
		HFONT sFont = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Verdana"));
		SelectObject(hdc, bFont);
		SetBkMode(hdc, TRANSPARENT);
		SOCmnString text;
		text = _T("Softing OPC Classic Demo Server");
		TextOut(hdc, 35, 150, text, text.getLength());
		SelectObject(hdc, sFont);
		text.format(_T("Version %s Build %s"), _T(VERSION_TEXT), _T(BUILD_NUMBER_STR));
		TextOut(hdc, 35, 176, text, text.getLength());
		SelectObject(hdc, bFont);
		text = _T("Copyright ©2020 Softing Industrial Automation GmbH");
		TextOut(hdc, 10, 293, text, text.getLength());
		text = _T("All rights reserved.");
		TextOut(hdc, 10, 306, text, text.getLength());
		SelectObject(hdc, sFont);
		text = _T("Internet: http://data-intelligence.softing.com");
		TextOut(hdc, 10, 320, text, text.getLength());
		text = _T("Support: support.automation@softing.com");
		TextOut(hdc, 10, 335, text, text.getLength());

		DeleteObject(bFont);
		DeleteObject(sFont);
#endif
		EndPaint(wnd, &ps);
	}
	break;
#ifdef SODMS_AE

	case WM_RBUTTONDOWN:
	{
		// create tracking event on right mouse click
		WORD xPos = LOWORD(lParam);
		WORD yPos = HIWORD(lParam);
		SOCmnPointer<SOAeSEvent> trackEvent;
		SOCmnString eventText;
		eventText.format(_T("clicked right mouse button at x:%d y:%d"), xPos, yPos);
		trackEvent = SOAeSEvent::createTrackingEvent(SODMS_AE_CATEGORTY_MOUSECLICK,
													 _T("computer.mouse.right button"), 300, eventText, _T("Interactive User"));
		trackEvent->addAttributeValue(SODMS_AE_ATTRIBUTE_POSX, SOCmnVariant(xPos));
		trackEvent->addAttributeValue(SODMS_AE_ATTRIBUTE_POSY, SOCmnVariant(yPos));
		trackEvent->fire();
	}
	break;

	case WM_MBUTTONDOWN:
	{
		// create tracking event on middle mouse click
		WORD xPos = LOWORD(lParam);
		WORD yPos = HIWORD(lParam);
		SOCmnPointer<SOAeSEvent> trackEvent;
		SOCmnString eventText;
		eventText.format(_T("clicked middle mouse button at x:%d y:%d"), xPos, yPos);
		trackEvent = SOAeSEvent::createTrackingEvent(SODMS_AE_CATEGORTY_MOUSECLICK,
													 _T("computer.mouse.middle button"), 700, eventText, _T("Interactive User"));
		trackEvent->addAttributeValue(SODMS_AE_ATTRIBUTE_POSX, SOCmnVariant(xPos));
		trackEvent->addAttributeValue(SODMS_AE_ATTRIBUTE_POSY, SOCmnVariant(yPos));
		trackEvent->fire();
	}
	break;
#endif

	case WM_DESTROY:
		// post a quit message and return
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(wnd, message, wParam, lParam);
	}

	return 0;
} // wndProc

#endif // SODMS_HEADLESS

//-----------------------------------------------------------------------------
// WinMain
//
// - main routine of server application
//
// returns:
//		exit value
//
int APIENTRY _tWinMain(
	HINSTANCE instance,     // instance
	HINSTANCE prevInstance, // previous instance
	LPTSTR cmdLine,         // command line
	int cmdShow)            // show command
{
	int ret = 0;
	g_instance = instance;
#ifdef SOOS_WINDOWS_CE
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
#endif
	initializeApp();
	BOOL end = FALSE;
	BOOL startedByCOM = FALSE;
#ifndef SOOS_WINDOWS_CE
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPTSTR)_T("SOServer"), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
#endif
	// initilialize entry
	initializeEntry();

	// register and unregister
	if (cmdLine)
	{
		end = processCommandLine(cmdLine, &startedByCOM);
	}

	if (!end)
	{
		if (!g_service)
		{
#ifndef SODMS_HEADLESS
			// load about image
			g_hBitmap = ::LoadBitmap(g_instance, _T("IMAGE_SOSERVER"));
			// register window class
			WNDCLASS    wc;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = (WNDPROC)wndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = instance;
			wc.hIcon = LoadIcon(g_instance, _T("ICON_SOSERVER"));
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = WINDOW_CLASS;
			RegisterClass(&wc);
			// create window
			HWND wnd;
#ifndef SOOS_WINDOWS_CE
			wnd = CreateWindow(WINDOW_CLASS, _T("Softing OPC Classic Demo Server"),
							   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_ICONIC,
							   0, 0, 405, 384, NULL, NULL, instance, NULL);
#else
			wnd = CreateWindow(WINDOW_CLASS, _T("Softing OPC Classic Demo Server"),
							   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
							   0, 0, 406, 384, NULL, NULL, instance, NULL);
#endif
#ifdef SOFEATURE_DCOM
			{
				// set event handler
				SOCmnPointer<SODmSEventHandler> eventHandler;
#ifdef SODMS_ALL
				SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
#else
#ifdef SODMS_DA
				SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
#endif
#ifdef SODMS_AE
				SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
#endif
#endif
				SOCMN_TRY
				{
					SOCMN_ALLOCATE_OBJECT_C(eventHandler, SODmSEventHandler(startedByCOM, wnd), (SODmSEventHandler*))
					entry->setEventHandler((SODmSEventHandler*)eventHandler);
				}
				SOCMN_CATCH(...)
				{
					return -1;
				}
			}
#endif // SOFEATURE_DCOM
#endif // SODMS_HEADLESS
			end = !(initializeServer());

			if (!end)
			{
#ifndef SODMS_HEADLESS

				if (wnd)
				{
#ifndef SOOS_WINDOWS_CE
					ShowWindow(wnd, SW_MINIMIZE);
#else
					ShowWindow(wnd, SW_SHOW);
					ShowWindow(wnd, SW_MINIMIZE);
#endif
					UpdateWindow(wnd);
				}

#endif
				// main message loop
				MSG msg;

				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				ret = msg.wParam;
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_USER1, _T("DMS"), (_T("Can't start the OPC server")));
			}

			terminateServer();
		}
		else
		{
			// service
#ifndef SOOS_WINDOWS_CE
			// start the service
			if (StartServiceCtrlDispatcher(dispatchTable))
			{
				return 0;
			}

#endif
		}
	}

	// terminate entry
	terminateEntry();
	terminateApp();
	return ret;
} // WinMain

#endif // SOOS_WINDOWS

#ifdef SOOS_LINUX

//-----------------------------------------------------------------------------
// main
//
// - main function of the sample
//
int main(
	IN int argc,            // number of command line arguments
	IN char** argv)         // array of command line arguments
{
	BOOL end = FALSE;
	BOOL service = FALSE;
	SOCmnStringEx cmdLine;

	for (int i = 0; i < argc; i++)
	{
		if (_tcschr(argv[i], _T(' ')))
		{
			cmdLine += _T('"');
		}

		cmdLine += argv[i];

		if (_tcschr(argv[i], _T(' ')))
		{
			cmdLine += _T('"');
		}

		if (i < argc - 1)
		{
			cmdLine += _T(' ');
		}
	}

	initializeApp();

	if (cmdLine.find(_T("-quiet")) != -1 || cmdLine.find(_T("/quiet")) != -1)
	{
		::getSOCmnTrace()->setLevels(0, 0, 0, 0);
	}

	if (cmdLine.find(_T("-service")) != -1 || cmdLine.find(_T("/service")) != -1)
	{
		signal(SIGHUP, SIG_IGN);
		_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("DMS"), (_T("Server mode: forking a child process now...")));
		pid_t child = fork();

		if (child != 0)
		{
			terminateApp();
			return 0;
		}

		service = TRUE;
	}

	// initilialize entry
	initializeEntry();

	if (!end)
	{
		end = processCommandLine(cmdLine, NULL);
	}

	if (!end)
	{
		end = !(initializeServer());

		if (!end)
		{
			_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("DMS"), (_T("Softing OPC Classic Demo Server")));
			_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("DMS"), (_T("processCommandLine(%s): port=%d ctt=%d"), (LPCTSTR)cmdLine, g_port, g_ctt));

			if (service)
			{
				Sleep(INFINITE);
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("DMS"), (_T("Press <Enter> to stop the server")));
				int c;
				c = getchar();
			}
		}
		else
		{
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_USER1, _T("DMS"), (_T("Can't start the OPC server")));
		}

		terminateServer();
	}

	// terminate entry
	terminateEntry();
	terminateApp();
	return 0;
} // main

#endif // SOOS_LINUX


//-----------------------------------------------------------------------------
// buildObjectSpaces
//
// - build all server toolkit object spaces
//   DA NameSpace, AE EventSpace and AE AreaSpace
//
void buildObjectSpaces(void)
{
	USES_CONVERSION;
#ifdef SODMS_DA
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root = entryDA->getNameSpaceRoot();
	SOCmnPointer<SODaSNode> node1stLevel;
	SOCmnPointer<SODaSNode> node2ndLevel;
	SOCmnPointer<SODaSNode> node3rdLevel;
	SOCmnPointer<SODaSTag> tagTime;
	SOCmnPointer<SODaSTag> tagInc;
	SOCmnPointer<SODaSTag> tagAccept;
	SOCmnPointer<SODaSTag> tagMaths;
	SOCmnPointer<SODaSTag> tagLocalSecond;
	SOCmnPointer<SODaSTag> tagSlot1Start;
	SOCmnPointer<SODaSTag> tagSlot1End;
	SOCmnPointer<SODaSTag> tagSlot2Start;
	SOCmnPointer<SODaSTag> tagSlot2End;
	SOCmnPointer<SODaSTag> tagWatch;
	SOCmnPointer<SODaSTag> tagError;
	SOCmnPointer<SODaSTag> tagMode;
	SOCmnPointer<SODaSTag> tagHidden;
	SOCmnPointer<SODaSTag> tagClient;
	SOCmnPointer<SODaSTag> tagSecure;
	SOCmnPointer<SODaSCache> cache;
	SOCmnPointer<SODaSProperty> prop;
	SOCmnVariant varHlp;
	SOCmnStringList stringEU;
	SOCmnPointer<SODaSTag> tagUnicode;
	//-- DA NameSpace --
	// time namespace tree
	node1stLevel = new SODaSNodeDefault(_T("time"));
	root->addBranch(node1stLevel);
	tagTime = new SODmSTagTime(_T("date"), SODmSTagTime::date, SODmSTagTime::local);
	node1stLevel->addLeaf(tagTime);
	node2ndLevel = new SODaSNodeDefault(_T("local"));
	node1stLevel->addBranch(node2ndLevel);
	tagTime = new SODmSTagTime(_T("hour"), SODmSTagTime::hour, SODmSTagTime::local);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("minute"), SODmSTagTime::minute, SODmSTagTime::local);
	node2ndLevel->addLeaf(tagTime);
	tagLocalSecond = new SODmSTagTime(_T("second"), SODmSTagTime::second, SODmSTagTime::local);
	node2ndLevel->addLeaf(tagLocalSecond);
	tagTime = new SODmSTagTime(_T("string"), SODmSTagTime::string, SODmSTagTime::local);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("array"), SODmSTagTime::array, SODmSTagTime::local);
	node2ndLevel->addLeaf(tagTime);
	node2ndLevel = new SODaSNodeDefault(_T("GMT"));
	node1stLevel->addBranch(node2ndLevel);
	tagTime = new SODmSTagTime(_T("hour"), SODmSTagTime::hour, SODmSTagTime::GMT);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("minute"), SODmSTagTime::minute, SODmSTagTime::GMT);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("second"), SODmSTagTime::second, SODmSTagTime::GMT);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("string"), SODmSTagTime::string, SODmSTagTime::GMT);
	node2ndLevel->addLeaf(tagTime);
	tagTime = new SODmSTagTime(_T("array"), SODmSTagTime::array, SODmSTagTime::GMT);
	node2ndLevel->addLeaf(tagTime);
#ifdef SODMS_ALL
	node2ndLevel = new SODaSNodeDefault(_T("time slot"));
	node1stLevel->addBranch(node2ndLevel);
	node3rdLevel = new SODaSNodeDefault(_T("slot 1"));
	node2ndLevel->addBranch(node3rdLevel);
	tagSlot1Start = new SODmSTagTime(_T("start"), SODmSTagTime::limitSecond, SODmSTagTime::none);
	node3rdLevel->addLeaf(tagSlot1Start);
	cache = tagSlot1Start->getCache();
	cache->setValue(SOCmnVariant((BYTE)5));
	tagSlot1End = new SODmSTagTime(_T("end"), SODmSTagTime::limitSecond, SODmSTagTime::none);
	node3rdLevel->addLeaf(tagSlot1End);
	cache = tagSlot1End->getCache();
	cache->setValue(SOCmnVariant((BYTE)35));

	if (!g_ctt)
	{
		node3rdLevel = new SODaSNodeDefault(_T("slot 2"));
		node2ndLevel->addBranch(node3rdLevel);
		tagSlot2Start = new SODmSTagTime(_T("start"), SODmSTagTime::limitSecond, SODmSTagTime::none);
		node3rdLevel->addLeaf(tagSlot2Start);
		cache = tagSlot2Start->getCache();
		cache->setValue(SOCmnVariant((BYTE)25));
		tagSlot2End = new SODmSTagTime(_T("end"), SODmSTagTime::limitSecond, SODmSTagTime::none);
		node3rdLevel->addLeaf(tagSlot2End);
		cache = tagSlot2End->getCache();
		cache->setValue(SOCmnVariant((BYTE)55));
	}

#endif
	// increment namespace tree
	node1stLevel = new SODmSNodeInc(_T("increment"));
	root->addBranch(node1stLevel);
	varHlp = _T("Items are incremented with every I/O request from the clients");
	prop = node1stLevel->addProperty(OPC_PROPERTY_DESCRIPTION, _T("description"));
	prop->setConstantValue(varHlp);
	prop = node1stLevel->addProperty(SODMS_DA_PROPERTY_INC_ON, _T("vsp:Increment on"));
	prop->setDescription(_T("Increment on"));
	prop->setNativeDatatype(VT_BOOL);
	prop->setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	varHlp = (BOOL)g_incrementOn;
	cache = prop->getCache();
	cache->set(varHlp , OPC_QUALITY_GOOD);
	tagInc = new SODmSTagInc(_T("UI1"), VT_UI1);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I1"), VT_I1);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI2"), VT_UI2);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I2"), VT_I2);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI4"), VT_UI4);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I4"), VT_I4);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UINT"), VT_UINT);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("INT"), VT_INT);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI8"), VT_UI8);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I8"), VT_I8);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R4"), VT_R4);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R8"), VT_R8);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BOOL"), VT_BOOL);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BSTR"), VT_BSTR);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("DATE"), VT_DATE);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("CY"), VT_CY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI1 array"), VT_UI1 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I1 array"), VT_I1 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI2 array"), VT_UI2 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I2 array"), VT_I2 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI4 array"), VT_UI4 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I4 array"), VT_I4 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UINT array"), VT_UINT | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("INT array"), VT_INT | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI8 array"), VT_UI8 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I8 array"), VT_I8 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R4 array"), VT_R4 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R8 array"), VT_R8 | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BOOL array"), VT_BOOL | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BSTR array"), VT_BSTR | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("DATE array"), VT_DATE | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("CY array"), VT_CY | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("VARIANT array"), VT_VARIANT | VT_ARRAY);
	node1stLevel->addLeaf(tagInc);
	// special namespace tree
	node1stLevel = new SODaSNodeDefault(_T("special"));
	root->addBranch(node1stLevel);
	tagAccept = new SODmSTagAccept(_T("writeonly"), VT_I4, OPC_WRITEABLE);
	node1stLevel->addLeaf(tagAccept);
	tagError = new SODmSTagError(_T("write error"), SODmSTagError::write);
	node1stLevel->addLeaf(tagError);
	tagError = new SODmSTagError(_T("read error"), SODmSTagError::read);
	node1stLevel->addLeaf(tagError);
	tagError = new SODmSTagError(_T("quality"), SODmSTagError::quality);
	node1stLevel->addLeaf(tagError);
	tagClient = new SODmSTagClient(_T("client specific"), VT_BSTR, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagClient);
	tagUnicode = new SODmSTagAccept(_T("中国的"), VT_BSTR, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagUnicode);
	varHlp = _T("中国的");
	cache = tagUnicode->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	tagUnicode = new SODmSTagAccept(_T("御国言葉"), VT_BSTR, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagUnicode);
	varHlp = _T("御国言葉");
	cache = tagUnicode->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	// maths namespace tree
	node1stLevel = new SODaSNodeDefault(_T("maths"));
	root->addBranch(node1stLevel);
	tagMaths = new SODmSTagMaths(_T("sin"), SODmSTagMaths::sin);
	tagMaths->setEUAnalog(-1.0, 1.0);
	node1stLevel->addLeaf(tagMaths);
	tagMaths = new SODmSTagMaths(_T("cos"), SODmSTagMaths::cos);
	tagMaths->setEUAnalog(-1.0, 1.0);
	node1stLevel->addLeaf(tagMaths);
	tagMaths = new SODmSTagMaths(_T("tan"), SODmSTagMaths::tan);
	node1stLevel->addLeaf(tagMaths);
	tagAccept = new SODmSTagAccept(_T("automatic angle change"), VT_BOOL, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagAccept);
	varHlp = (BOOL)TRUE;
	cache = tagAccept->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	tagAccept = new SODmSTagAccept(_T("automatic change period"), VT_UI4, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagAccept);
	varHlp = (DWORD)1000;
	cache = tagAccept->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	// watch namespace tree
	node1stLevel = new SODaSNodeDefault(_T("watch"));
	root->addBranch(node1stLevel);
	node2ndLevel = new SODmSNodeWatch(_T("device 1"), _T("controller"), 42, SOWATCH_DEVSTATE_OK, _T(""));
	node1stLevel->addBranch(node2ndLevel);
	tagWatch = new SODmSTagWatch(_T("value 1"), VT_I4, OPC_WRITEABLE | OPC_READABLE, 12);
	stringEU.create();
	stringEU.add(_T("ZERO"));
	stringEU.add(_T("ONE"));
	stringEU.add(_T("TWO"));
	tagWatch->setEUEnumerated(&stringEU);
	node2ndLevel->addLeaf(tagWatch);
	tagWatch = new SODmSTagWatch(_T("value 2"), VT_I2, OPC_WRITEABLE | OPC_READABLE, 22);
	tagWatch->setEUEnumerated(&stringEU);
	node2ndLevel->addLeaf(tagWatch);
	node2ndLevel = new SODmSNodeWatch(_T("device 2"), _T("coupler"), 11, SOWATCH_DEVSTATE_ERROR, _T("can't startup device"));
	node1stLevel->addBranch(node2ndLevel);
	tagWatch = new SODmSTagWatch(_T("value 1"), VT_R4, OPC_WRITEABLE | OPC_READABLE, 0);
	node2ndLevel->addLeaf(tagWatch);
	tagWatch = new SODmSTagWatch(_T("value 2"), VT_R8, OPC_WRITEABLE | OPC_READABLE, 40);
	node2ndLevel->addLeaf(tagWatch);
	// I/O and update mode
	node1stLevel = new SODaSNodeDefault(_T("modes"));
	root->addBranch(node1stLevel);
	node2ndLevel = new SODaSNodeDefault(_T("polling"));
	node1stLevel->addBranch(node2ndLevel);
	tagMode = new SODmSTagMode(SODaSItemTag::poll, SODaSItemTag::direct, _T("direct"), VT_I2);
	node2ndLevel->addLeaf(tagMode);
	tagMode = new SODmSTagMode(SODaSItemTag::poll, SODaSItemTag::queue, _T("queued"), VT_I2);
	node2ndLevel->addLeaf(tagMode);
	tagMode = new SODmSTagMode(SODaSItemTag::poll, SODaSItemTag::transaction, _T("transaction"), VT_I2);
	node2ndLevel->addLeaf(tagMode);
	g_modeReportNode = new SODaSNodeDefault(_T("report"));
	node1stLevel->addBranch(g_modeReportNode);
	tagMode = new SODmSTagMode(SODaSItemTag::report, SODaSItemTag::direct, _T("direct"), VT_I2);
	g_modeReportNode->addLeaf(tagMode);
	varHlp = (SHORT)0;
	cache = tagMode->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	tagMode = new SODmSTagMode(SODaSItemTag::report, SODaSItemTag::queue, _T("queued"), VT_I2);
	g_modeReportNode->addLeaf(tagMode);
	cache = tagMode->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);
	tagMode = new SODmSTagMode(SODaSItemTag::report, SODaSItemTag::transaction, _T("transaction"), VT_I2);
	g_modeReportNode->addLeaf(tagMode);
	cache = tagMode->getCache();
	cache->set(varHlp, OPC_QUALITY_GOOD);

	// dynamic namespace tree
	if (!g_ctt)
	{
		node1stLevel = new SODmSNodeDynamic(_T("dynamic"));
		root->addBranch(node1stLevel);
	}

	// hidden namespace tree
	node1stLevel = new SODmSNodeHidden(_T("hidden"));
	root->addBranch(node1stLevel);
	tagHidden = new SODmSTagHidden(_T("I2"), VT_I2, OPC_READABLE | OPC_WRITEABLE);
	node1stLevel->addLeaf(tagHidden);

	// secure namespace tree
	if (!g_ctt)
	{
		node1stLevel = new SODaSNodeDefault(_T("secure"));
		root->addBranch(node1stLevel);
		tagSecure = new SODmSTagSecure(_T("I2"), VT_I2, OPC_READABLE | OPC_WRITEABLE);
		node1stLevel->addLeaf(tagSecure);
	}

	// IOP namespace tree
	node1stLevel = new SODaSNodeDefault(_T("IOP"));
	root->addBranch(node1stLevel);
	varHlp = _T("Items for the OPC IOP and the OPC CTT");
	prop = node1stLevel->addProperty(OPC_PROPERTY_DESCRIPTION, _T("description"));
	prop->setConstantValue(varHlp);
	node2ndLevel = new SODaSNodeDefault(_T("static"));
	node1stLevel->addBranch(node2ndLevel);
	varHlp = _T("Items have static values but accept writes from the client");
	prop = node2ndLevel->addProperty(OPC_PROPERTY_DESCRIPTION, _T("description"));
	prop->setConstantValue(varHlp);
	tagInc = new SODmSTagAccept(_T("UI1"), VT_UI1, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I1"), VT_I1, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI2"), VT_UI2, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I2"), VT_I2, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI4"), VT_UI4, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I4"), VT_I4, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UINT"), VT_UINT, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("INT"), VT_INT, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI8"), VT_UI8, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I8"), VT_I8, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("R4"), VT_R4, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("R8"), VT_R8, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("BOOL"), VT_BOOL, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("BSTR"), VT_BSTR, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("DATE"), VT_DATE, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("CY"), VT_CY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI1 array"), VT_UI1 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I1 array"), VT_I1 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI2 array"), VT_UI2 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I2 array"), VT_I2 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI4 array"), VT_UI4 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I4 array"), VT_I4 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UINT array"), VT_UINT | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("INT array"), VT_INT | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("UI8 array"), VT_UI8 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("I8 array"), VT_I8 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("R4 array"), VT_R4 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("R8 array"), VT_R8 | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("BOOL array"), VT_BOOL | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("BSTR array"), VT_BSTR | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("DATE array"), VT_DATE | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("CY array"), VT_CY | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagAccept(_T("VARIANT array"), VT_VARIANT | VT_ARRAY, OPC_READABLE | OPC_WRITEABLE);
	node2ndLevel->addLeaf(tagInc);
	node2ndLevel = new SODaSNodeDefault(_T("dynamic"));
	node1stLevel->addBranch(node2ndLevel);
	varHlp = _T("Items are incremented with every I/O request from the clients");
	prop = node2ndLevel->addProperty(OPC_PROPERTY_DESCRIPTION, _T("description"));
	prop->setConstantValue(varHlp);
	tagInc = new SODmSTagInc(_T("UI1"), VT_UI1);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I1"), VT_I1);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI2"), VT_UI2);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I2"), VT_I2);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI4"), VT_UI4);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I4"), VT_I4);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UINT"), VT_UINT);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("INT"), VT_INT);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI8"), VT_UI8);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I8"), VT_I8);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R4"), VT_R4);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R8"), VT_R8);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BOOL"), VT_BOOL);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BSTR"), VT_BSTR);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("DATE"), VT_DATE);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("CY"), VT_CY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI1 array"), VT_UI1 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I1 array"), VT_I1 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI2 array"), VT_UI2 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I2 array"), VT_I2 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI4 array"), VT_UI4 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I4 array"), VT_I4 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UINT array"), VT_UINT | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("INT array"), VT_INT | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("UI8 array"), VT_UI8 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("I8 array"), VT_I8 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R4 array"), VT_R4 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("R8 array"), VT_R8 | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BOOL array"), VT_BOOL | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("BSTR array"), VT_BSTR | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("DATE array"), VT_DATE | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("CY array"), VT_CY | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
	tagInc = new SODmSTagInc(_T("VARIANT array"), VT_VARIANT | VT_ARRAY);
	node2ndLevel->addLeaf(tagInc);
#endif
#ifdef SODMS_AE
	//-- AE EventSpace --
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> esRoot;
	SOCmnPointer<SOAeSCategory> category;
	SOCmnPointer<SOAeSAttribute> attribute;
#ifdef SODMS_ALL
	SOCmnPointer<SOAeSAttribute> attributeC;
	SOCmnPointer<SOAeSAttribute> attributeS;
	SOCmnPointer<SOAeSAttribute> attributeE;
	SOCmnPointer<SOAeSConditionClassSingleStateDefault> betweenConditionClass;
#endif
	esRoot = entryAE->getEventSpaceRoot();
	category = new SOAeSCategory(SODMS_AE_CATEGORTY_TIME_TICK, _T("time tick"), OPC_SIMPLE_EVENT);
	esRoot->addBranch(category);
	attribute = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_CURRENT_TIME, _T("current time"), VT_DATE);
	category->addAdditionalElement(attribute);
	category = new SOAeSCategory(SODMS_AE_CATEGORTY_MOUSECLICK, _T("mouse click"), OPC_TRACKING_EVENT);
	esRoot->addBranch(category);
	attribute = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_POSX, _T("x position"), VT_UI2);
	category->addAdditionalElement(attribute);
	attribute = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_POSY, _T("y position"), VT_UI2);
	category->addAdditionalElement(attribute);
#ifdef SODMS_ALL
	category = new SOAeSCategory(SODMS_AE_CATEGORTY_TIME_SLOT, _T("time slot"), OPC_CONDITION_EVENT);
	esRoot->addBranch(category);
	attributeC = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_CURRENT_SECOND, _T("current second"), VT_UI1);
	category->addAdditionalElement(attributeC);
	attributeS = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_START_SECOND, _T("start second"), VT_UI1);
	category->addAdditionalElement(attributeS);
	attributeE = new SOAeSAttribute(SODMS_AE_ATTRIBUTE_END_SECOND, _T("end second"), VT_UI1);
	category->addAdditionalElement(attributeE);
	betweenConditionClass = new SOAeSConditionClassSingleStateDefault(_T("between"));
	category->addBranch(betweenConditionClass);
	betweenConditionClass->setActiveSeverity(345);
	betweenConditionClass->setActiveMessage(_T("The current second is between the start and the end limit values"));
	betweenConditionClass->setDefinition(_T("This condition checks if the current second is between the start and the end limit values"));
#endif
	//-- AE AreaSpace --
	SOCmnPointer<SOAeSAreaSpaceRoot> areaRoot;
	SOCmnPointer<SOAeSArea> area1stLevel;
	SOCmnPointer<SOAeSArea> area2ndLevel;
	SOCmnPointer<SOAeSSource> src;
#ifdef SODMS_ALL
	SOCmnPointer<SOAeSCondition> cond;
	SOCmnPointer<SOAsSAttributeValue> av;
	SOCmnList<SOAeSConditionGroup> condGroupList(entryAE->getConditionGroupList());
	SOCmnPointer<SOAsSConditionGroup> condGroup;
	SOCmnList<SOAeSCondition> condList;
#endif
	areaRoot = entryAE->getAreaSpaceRoot();
	area1stLevel = new SOAeSAreaDefault(_T("computer"));
	areaRoot->addBranch(area1stLevel);
	area2ndLevel = new SOAeSAreaDefault(_T("mouse"));
	area1stLevel->addBranch(area2ndLevel);
	src = new SOAeSSourceDefault(_T("right button"));
	area2ndLevel->addLeaf(src);
	src = new SOAeSSourceDefault(_T("middle button"));
	area2ndLevel->addLeaf(src);
	area2ndLevel = new SOAeSAreaDefault(_T("clock"));
	area1stLevel->addBranch(area2ndLevel);
	src = new SOAeSSourceDefault(_T("timer"));
	area2ndLevel->addLeaf(src);
#ifdef SODMS_ALL
	condGroup = new SOAsSConditionGroup(250);
	condList = condGroup->getConditionList();
	src = new SOAeSSourceDefault(_T("time slot 1"));
	area2ndLevel->addLeaf(src);
	cond = new SODmSConditionTimeSlot();
	cond->setConditionClass(betweenConditionClass);
	src->addAdditionalElement(cond);
	condList.add(cond);
	av = new SOAsSAttributeValue(attributeC);
	cond->addAdditionalElement(av);
	av->setItemTag(tagLocalSecond);
	av = new SOAsSAttributeValue(attributeS);
	cond->addAdditionalElement(av);
	av->setItemTag(tagSlot1Start);
	av = new SOAsSAttributeValue(attributeE);
	cond->addAdditionalElement(av);
	av->setItemTag(tagSlot1End);

	if (!g_ctt)
	{
		src = new SOAeSSourceDefault(_T("time slot 2"));
		area2ndLevel->addLeaf(src);
		cond = new SODmSConditionTimeSlot();
		cond->setConditionClass(betweenConditionClass);
		src->addAdditionalElement(cond);
		condList.add(cond);
		av = new SOAsSAttributeValue(attributeC);
		cond->addAdditionalElement(av);
		av->setItemTag(tagLocalSecond);
		av = new SOAsSAttributeValue(attributeS);
		cond->addAdditionalElement(av);
		av->setItemTag(tagSlot2Start);
		av = new SOAsSAttributeValue(attributeE);
		cond->addAdditionalElement(av);
		av->setItemTag(tagSlot2End);
	}

	condGroupList.add(condGroup);
#endif
#endif
} // buildObjectSpaces


//-----------------------------------------------------------------------------
// buildPublicGroups
//
// - create the public groups of the server
//
void buildPublicGroups(void)
{
#ifdef SODMS_DA
#ifdef SOFEATURE_DCOM
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnPointer<SODaSServer> pg;
	SOCmnPointer<SODaSGroup> grp;
	pg = entry->getPublicGroups();

	if (!pg)
	{
		return;
	}

	if (SUCCEEDED(pg->addGroup(_T("Public Increment Group"), FALSE, 200, 0, NULL, NULL, 0, (SODaSGroup**)grp, NULL)))
	{
		grp->addItem(_T("increment.I2"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
		grp->addItem(_T("increment.BOOL"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
		grp->addItem(_T("increment.R8"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
		grp->addItem(_T("increment.UI1 array"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
	}

	grp.release();

	if (SUCCEEDED(pg->addGroup(_T("Public Maths Group"), FALSE, 1000, 0, NULL, NULL, 0, (SODaSGroup**)grp, NULL)))
	{
		grp->addItem(_T("maths.sin"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
		grp->addItem(_T("maths.cos"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
		grp->addItem(_T("maths.tan"), _T(""), TRUE, 0, VT_EMPTY, NULL, NULL, NULL);
	}

	grp.release();
#endif
#endif
} // buildPublicGroups


//-----------------------------------------------------------------------------
// fireTimeTick
//
// - fire "Time Tick" event
//
void fireTimeTick(void)
{
#ifdef SODMS_AE
	static WORD actSec = 0xFFFF;
	static WORD actMin = 0xFFFF;
	SYSTEMTIME now;
	SOCmnPointer<SOAeSEvent> eventSec;
	SOCmnPointer<SOAeSEvent> eventMin;
	GetLocalTime(&now);

	if (now.wSecond != actSec)
	{
		// second changed -> create event
		SOCmnString strnow;
		actSec = now.wSecond;
		eventSec = SOAeSEvent::createSimpleEvent(SODMS_AE_CATEGORTY_TIME_TICK,
												 _T("computer.clock.timer"), 10, _T("The second changed"));
		strnow.format(_T("%02.2d:%02.2d:%02.2d.%03.3d"),
					  now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
		eventSec->addAttributeValue(SODMS_AE_ATTRIBUTE_CURRENT_TIME, SOCmnVariant(strnow));
	}

	if (now.wMinute != actMin)
	{
		// minute changed -> create event
		SOCmnString strnow;
		actMin = now.wMinute;
		eventMin = SOAeSEvent::createSimpleEvent(SODMS_AE_CATEGORTY_TIME_TICK,
												 _T("computer.clock.timer"), 490, _T("The minute changed"));
		strnow.format(_T("%02.2d:%02.2d:%02.2d.%03.3d"),
					  now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
		eventMin->addAttributeValue(SODMS_AE_ATTRIBUTE_CURRENT_TIME, SOCmnVariant(strnow));
	}

	if (((SOAeSEvent*)eventSec) || ((SOAeSEvent*)eventMin))
	{
		// created some events
		// -> send them to the clients
		SOCmnList<SOAeSEvent> eventList;
		eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

		if ((SOAeSEvent*)eventSec)
		{
			eventList.add((SOAeSEvent*)eventSec);
		}

		if ((SOAeSEvent*)eventMin)
		{
			eventList.add((SOAeSEvent*)eventMin);
		}

		SOAeSEvent::fireEvents(&eventList);
	}

#endif
} // fireTimeTick

#ifdef SODMS_DA
//-----------------------------------------------------------------------------
// removeCompletedRequestsFromQueue
//
// - remove the completed requests from the request queue
//
void removeCompletedRequestsFromQueue(void)
{
	SOCmnList<SODaSRequest> requestQueue(g_requestQueue);
	SOCmnListPosition pos, curPos, prevPos;
	SODaSRequest* pRequest;
	curPos = NULL;
	pos = requestQueue.getStartPosition();

	while (pos)
	{
		prevPos = curPos;
		curPos = pos;
		pRequest = requestQueue.getNext(pos);

		if (pRequest->isCompleted())
		{
			requestQueue.removeAt(curPos);
			curPos = prevPos;
		}
	}
} // removeCompletedRequestsFromQueue


//-----------------------------------------------------------------------------
// ioThread
//
// - main routine of I/O worker thread
//
DWORD _stdcall ioThread(void* context)
{
	DWORD waitR;
	BOOL end = FALSE;

	while (!end)
	{
		// wait for new requests or the end
		waitR = g_ioThread->waitForEvents();

		if (waitR == g_ioThread->getStopEventId())
		{
			end = TRUE;
		}
		else if (waitR == g_requestQueueEventId)
		{
			SOCmnList<SODaSRequest> requestQueue(g_requestQueue);
			SOCmnListPosition pos;
			SODaSRequest* pRequest;
			pos = requestQueue.getStartPosition();

			while (pos)
			{
				// for all request in the queue
				pRequest = requestQueue.getNext(pos);

				if (!pRequest->isCompleted())
				{
					SOCmnPointer<SODaSItemTag> tag = pRequest->getItemTag();

					if (tag.isNotNull())
					{
						tag->handleRequest(pRequest);
					}
				}
			}

			removeCompletedRequestsFromQueue();
		}
	}

	return 0;
} // ioThread
#endif

//-----------------------------------------------------------------------------
// simThread
//
// - event and I/O simmulation thread
//
// returns:
//		0
//
DWORD _stdcall simThread(
	IN LPVOID context)
{
	DWORD changePeriod = 1000;  // every second
	DWORD lastDevStateChange = GetTickCount();
	DWORD lastMathsChange = GetTickCount() - 1100;

	while (g_simThread->waitForEvents(10) != g_simThread->getStopEventId())
	{
		fireTimeTick();
#ifdef SODMS_DA

		if (getTimeSpan(lastMathsChange, GetTickCount()) > changePeriod)
		{
			// every changePeriod
			lastMathsChange = GetTickCount();
			SOCmnListPosition pos;
			SODaSTag* tag;
			// change maths values
			SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
			SOCmnPointer<SODaSNameSpaceRoot> nsRoot = entry->getNameSpaceRoot();
			SOCmnPointer<SOCmnElement> mathsNode = nsRoot->findBranch(_T("maths"));
			SOCmnList<SODaSTag> mathsList = (SOCmnList<SODaSTag> *)mathsNode->getLeafList();
			SOCmnPointer<SODmSTagAccept> autoAngleChange = (SODmSTagAccept*)mathsList.findKey(_T("automatic angle change"));
			SOCmnPointer<SODaSCache> autoAngleChangeCache = autoAngleChange->getCache();
			SOCmnVariant aaC;
			SOCmnPointer<SODmSTagAccept> autoChangePeriod = (SODmSTagAccept*)mathsList.findKey(_T("automatic change period"));
			SOCmnPointer<SODaSCache> autoChangePeriodCache = autoChangePeriod->getCache();
			SOCmnVariant acP;
			autoAngleChangeCache->getValue(aaC);

			if (aaC.boolVal)
			{
				pos = mathsList.getStartPosition();

				while (pos)
				{
					tag = mathsList.getNext(pos);

					if (tag->getUserData() == SODMS_DA_TAG_MATHS)
					{
						((SODmSTagMaths*)tag)->incAngle();
					}
				}
			}

			autoChangePeriodCache->getValue(acP);
			changePeriod = acP.ulVal;
			// simulate event-driven (report) I/O
			SOCmnList<SODaSItemTag> reportTagList = (SOCmnList<SODaSItemTag> *)g_modeReportNode->getLeafList();
			SOCmnPointer<SODaSCache> cache;
			SOCmnVariant nvalue, cvalue;
			pos = reportTagList.getStartPosition();

			while (pos)
			{
				tag = (SODaSTag*)reportTagList.getNext(pos);
				cache = tag->getCache();
				cache->getValue(cvalue);
				nvalue = (SHORT)(cvalue.iVal + 1);
				cache->set(nvalue, OPC_QUALITY_GOOD);
				tag->notifyAllConnectedObjects(&nvalue, OPC_QUALITY_GOOD);
			}
		}

		if (getTimeSpan(lastDevStateChange, GetTickCount()) > 15000)
		{
			// every 15 seconds
			lastDevStateChange = GetTickCount();
			SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
			SOCmnPointer<SODaSNameSpaceRoot> nsRoot = entry->getNameSpaceRoot();
			SOCmnPointer<SOCmnElement> watchNode = nsRoot->findBranch(_T("watch"));
			SOCmnList<SODmSNodeWatch> devList = (SOCmnList<SODmSNodeWatch> *)watchNode->getBranchList();
			SOCmnListPosition devPos;
			SODmSNodeWatch* dev;
			WORD newQuality = OPC_QUALITY_GOOD;
			// for all devices
			devPos = devList.getStartPosition();

			while (devPos)
			{
				dev = devList.getNext(devPos);

				if (dev->getDeviceState() == SOWATCH_DEVSTATE_OK)
				{
					dev->setDeviceState(SOWATCH_DEVSTATE_ERROR);
					dev->setDeviceStateInfo(_T("communication to device failed!"));
					newQuality = OPC_QUALITY_COMM_FAILURE;
				}
				else if (dev->getDeviceState() == SOWATCH_DEVSTATE_ERROR)
				{
					dev->setDeviceState(SOWATCH_DEVSTATE_OK);
					dev->setDeviceStateInfo(_T(""));
					newQuality = OPC_QUALITY_GOOD;
				}

				SOCmnList<SODmSTagWatch> tagList = (SOCmnList<SODmSTagWatch> *)dev->getLeafList();
				SOCmnListPosition tagPos;
				SODmSTagWatch* tag;
				SOCmnPointer<SODaSCache> cache;
				SOCmnDateTime now;
				now.now();
				// for all tags
				tagPos = tagList.getStartPosition();

				while (tagPos)
				{
					tag = tagList.getNext(tagPos);
					cache = tag->getCache();
					cache->setQuality(newQuality);
					cache->setTimeStamp(now);
					// add device state notification to all tags
#ifdef SOFEATURE_WATCH

					if (tag->getCollectChanges())
					{
						SOSrvWatch::attributeChange(tag, SOWATCH_ATTRIBUTE_DEVSTATE_T, tag->getDeviceState());
						SOSrvWatch::attributeChange(tag, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, tag->getDeviceStateInfo());
					}

#endif
				}
			}
		}

#endif
	}

	return 0;
} // simThread



#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SODmSEventHandler                                                          |
//-----------------------------------------------------------------------------

SODmSEventHandler::SODmSEventHandler(
	IN BOOL startedByCom,
	IN HWND wnd)
	: SOSrvEventHandler()
{
	m_startedByCom = startedByCom;
	m_wnd = wnd;
}

//-----------------------------------------------------------------------------
//  onReleasedLastOPCObject
//
// - handle release of last COM object
//
void SODmSEventHandler::onReleasedLastOPCObject(void)
{
	g_canCloseEvent.signal();

	if (m_startedByCom)
	{
		// started via COM
#ifdef SODMS_ALL
		SOCmnPointer<SOAsSEntry> entry = ::getSOAsSEntry();
#else
#ifdef SODMS_DA
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
#endif
#ifdef SODMS_AE
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
#endif
#endif

		// -> try to close application if last object was released
		if (S_OK == entry->canUnloadNow())
		{
			entry->revokeClassObjects();

			if (m_wnd)
			{
				PostMessage(m_wnd, WM_CLOSE, 0, 0);
			}
		}
	}
} // onReleasedLastOPCObject

#endif // SOFEATURE_DCOM



//-----------------------------------------------------------------------------
// SODmSTraceHandler                                                          |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// saveTraceOptions
//
// - save the trace options
//
HRESULT SODmSTraceHandler::saveTraceOptions(void)
{
#ifdef SOOS_WINDOWS
	SOCmnTrace* trace = ::getSOCmnTrace();
	HKEY keyServer = NULL;
	DWORD ret;
	DWORD dw;

	if (ERROR_SUCCESS == (ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Server"),
											 NULL, KEY_WRITE, &keyServer)))
	{
		RegSetValueEx(keyServer, _T("TraceError"), NULL, REG_DWORD, (LPBYTE)&trace->m_traceLevel[SOCMNTRACE_L_ERR], sizeof(DWORD));
		RegSetValueEx(keyServer, _T("TraceWarning"), NULL, REG_DWORD, (LPBYTE)&trace->m_traceLevel[SOCMNTRACE_L_WRN], sizeof(DWORD));
		RegSetValueEx(keyServer, _T("TraceInfo"), NULL, REG_DWORD, (LPBYTE)&trace->m_traceLevel[SOCMNTRACE_L_INF], sizeof(DWORD));
		RegSetValueEx(keyServer, _T("TraceDebug"), NULL, REG_DWORD, (LPBYTE)&trace->m_traceLevel[SOCMNTRACE_L_DEB], sizeof(DWORD));
		dw = trace->getFileSize();
		RegSetValueEx(keyServer, _T("TraceFileMaxSize"), NULL, REG_DWORD, (LPBYTE)&dw, sizeof(DWORD));
		dw = trace->getTraceToFile();
		RegSetValueEx(keyServer, _T("TraceFileEnable"), NULL, REG_DWORD, (LPBYTE)&dw, sizeof(DWORD));
		RegSetValueEx(keyServer, _T("TraceFile"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)m_traceFileName, m_traceFileName.getLength());
		RegCloseKey(keyServer);
		return S_OK;
	}
	else
	{
		return HRESULT_FROM_WIN32(ret);
	}

#else
	return E_FAIL;
#endif // SOOS_WINDOWS
} // saveTraceOptions


//-----------------------------------------------------------------------------
// loadTraceOptions
//
// - load the trace options
//
void SODmSTraceHandler::loadTraceOptions(void)
{
	SOCmnTrace* trace = ::getSOCmnTrace();
#ifdef SOOS_WINDOWS
	HKEY keyServer = NULL;
	DWORD sizeDWORD = sizeof(DWORD);
	DWORD sizeBuffer = MAX_PATH;
	TCHAR stringBuffer[MAX_PATH];
	BOOL bSetFile = FALSE;
	SOCmnString traceFile2;      // path of trace file
	DWORD traceFileMaxSize;     // max size of trace file
	DWORD traceFileOn = FALSE;
#endif
	DWORD traceErrorLevel;      // trace group in error level
	DWORD traceWarningLevel;    // trace group in warning level
	DWORD traceInfoLevel;       // trace group in information level
	DWORD traceDebugLevel;      // trace group in debug level
	DWORD traceMaxBackups = 5;
	traceErrorLevel = SOCMNTRACE_G_ALL;
	traceWarningLevel = SOCMNTRACE_G_ALL;
#ifdef _DEBUG
	traceInfoLevel = SOCMNTRACE_G_OPC | SOCMNTRACE_G_USER;
	traceDebugLevel = SOCMNTRACE_G_OPC | SOCMNTRACE_G_USER;
#else
	traceInfoLevel = SOCMNTRACE_G_USER;
	traceDebugLevel = 0;
#endif
#ifdef SOOS_WINDOWS
	traceFileMaxSize = 1000000;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Server"),
									  NULL, KEY_READ, &keyServer))
	{
		RegQueryValueEx(keyServer, _T("TraceError"), NULL, NULL, (LPBYTE)&traceErrorLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceWarning"), NULL, NULL, (LPBYTE)&traceWarningLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceInfo"), NULL, NULL, (LPBYTE)&traceInfoLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceDebug"), NULL, NULL, (LPBYTE)&traceDebugLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceFileMaxSize"), NULL, NULL, (LPBYTE)&traceFileMaxSize, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceFileMaxBackups"), NULL, NULL, (LPBYTE)&traceMaxBackups, &sizeDWORD);
		stringBuffer[0] = _T('\0');
		RegQueryValueEx(keyServer, _T("TraceFile"), NULL, NULL, (LPBYTE)stringBuffer, &sizeBuffer);

		if ((sizeBuffer != 0) && (_tcslen(stringBuffer) != 0))
		{
			m_traceFileName = stringBuffer;
			bSetFile = TRUE;
		}

		traceFileOn = bSetFile;

		RegQueryValueEx(keyServer, _T("TraceFileEnable"), NULL, NULL, (LPBYTE)&traceFileOn, &sizeDWORD);
		RegCloseKey(keyServer);
	}

	// initialize trace
	trace->enableTraceToFile(traceFileOn);

	if (bSetFile)
	{
		trace->setFile2(m_traceFileName, traceMaxBackups, traceFileMaxSize);
	}

#endif // SOOS_WINDOWS
	trace->setLevels(traceErrorLevel, traceWarningLevel, traceInfoLevel, traceDebugLevel);
} // loadTraceOptions


#ifdef SODMS_DA


//-----------------------------------------------------------------------------
// SODmSCreatorDA                                                             |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// createServer
//
// - create server object
//
// returns:
//		created object
//
SODaSServer* SODmSCreatorDA::createServer(
	IN BOOL publicGroups,   // create public groups server object
	IN BYTE serverInstance) // server instance number
{
	SODaSServer* server;
	SOCMN_ALLOCATE_OBJECT(server, SODmSServerDA(publicGroups, serverInstance))
	return server;
} // createServer

SODaSGroup* SODmSCreatorDA::createGroup(IN SODaSServer* parent)
{
	SODaSGroup* group;
	SOCMN_ALLOCATE_OBJECT(group, SODmSGroup())
	return group;
} // createGroup


//-----------------------------------------------------------------------------
// createTag
//
// - create tag object
//
// returns:
//		created object
//
SODaSTag* SODmSCreatorDA::createTag(
	IN LPCTSTR qualItemID)  // fully qualified id of the tag
{
	SODaSTag* tag;
	SOCMN_ALLOCATE_OBJECT(tag, SODmSTagAccept())
	return tag;
} // createTag


//-----------------------------------------------------------------------------
// createCache
//
// - create cache object
//
// returns:
//		created object
//
SODaSCache* SODmSCreatorDA::createCache(
	IN SODaSItemTag* tag)   // tag object of cache
{
	SODaSCache* cache;

	if (tag->getUserData() != SODMS_DA_TAG_CLIENT)
	{
		SOCMN_ALLOCATE_OBJECT(cache, SODaSCache())
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(cache, SODmSCache())
	}

	return cache;
} // createCache


//-----------------------------------------------------------------------------
//  createTransaction
//
// - create transaction object
//
// returns:
//		created object
//
SODaSTransaction* SODmSCreatorDA::createTransaction(
	IN SODaSGroup* group,    // group object
	IN BYTE transactionType, // type of transaction
	IN BYTE connectionType,  // connection type
	IN DWORD connection)     // connection id
{
	SODaSTransaction* tac;
	SOCMN_ALLOCATE_OBJECT(tac, SODmSTransaction(group, transactionType, connectionType, connection))
	return tac;
} // createTransaction




//-----------------------------------------------------------------------------
// SODmSServerDA                                                              |
//-----------------------------------------------------------------------------

SODmSServerDA::SODmSServerDA(IN BOOL publicGroups, IN BYTE serverInstance)
	: SODaSServer(publicGroups, serverInstance)
{
	m_clientSpecificString.format(_T("0x%8.8lX"), (DWORD)this->getOpcHandle());
	m_clientSpecificTimeStamp.now();
	m_accessLevel = 0;
}

SODmSServerDA::~SODmSServerDA()
{
}

//-----------------------------------------------------------------------------
// isNodeName
//
// - check if the string represents a valid node
//
// returns:
//		TRUE  - valid node name
//		FALSE - no node with this name | 0x0000f000
//
BOOL SODmSServerDA::isNodeName(
	IN LPCTSTR fullName)    // node name
{
	SOCmnPointer<SOCmnElement> el = SODaSNameSpaceRoot::getElement(fullName);

	if (el.isNotNull())
	{
		return FALSE;
	}

	if (_tcsncmp(_T("dynamic."), fullName, 8) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // isNodeName


//-----------------------------------------------------------------------------
// isTagName
//
// - check if the string represents a valid tag
//
// returns:
//		TRUE  - valid tag name
//		FALSE - no tag with this name
//
BOOL SODmSServerDA::isTagName(
	IN LPCTSTR fullName)    // tag name
{
	SOCmnPointer<SOCmnElement> el = SODaSNameSpaceRoot::getElement(fullName);

	if (el.isNotNull())
	{
		return FALSE;
	}

	if (_tcsncmp(_T("dynamic."), fullName, 8) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // isTagName


//-----------------------------------------------------------------------------
// initTag
//
// - initialize created tag
//
void SODmSServerDA::initTag(IN SODaSTag* tag)
{
	SOCmnString fullName = tag->getItemID();
	DWORD arraySize = 0;
	VARTYPE dt = VT_UI4;
	LPTSTR datatypeStr;

	if ((datatypeStr = _tcsstr(fullName, _T("-VT_")))   != NULL)
	{
		LPTSTR datatypeStrEnd;

		if ((datatypeStrEnd = _tcschr(datatypeStr + 1, _T('-'))) != NULL)
		{
			SOCmnString datatype;
			LPTSTR datatypeBuf = datatype.getBuffer(datatypeStrEnd - datatypeStr);
			_tcsncpy(datatypeBuf, datatypeStr + 1, (datatypeStrEnd - datatypeStr) - 1);
			datatypeBuf[(datatypeStrEnd - datatypeStr) - 1] = _T('\0');

			if (datatype == _T("VT_BSTR"))
			{
				dt = VT_BSTR;
			}
			else if (datatype == _T("VT_UI1"))
			{
				dt = VT_UI1;
			}
			else if (datatype == _T("VT_I1"))
			{
				dt = VT_I1;
			}
			else if (datatype == _T("VT_UI2"))
			{
				dt = VT_UI2;
			}
			else if (datatype == _T("VT_I2"))
			{
				dt = VT_I2;
			}
			else if (datatype == _T("VT_UI4"))
			{
				dt = VT_UI4;
			}
			else if (datatype == _T("VT_I4"))
			{
				dt = VT_I4;
			}
			else if (datatype == _T("VT_UI8"))
			{
				dt = VT_UI8;
			}
			else if (datatype == _T("VT_I8"))
			{
				dt = VT_I8;
			}
			else if (datatype == _T("VT_UINT"))
			{
				dt = VT_UINT;
			}
			else if (datatype == _T("VT_INT"))
			{
				dt = VT_INT;
			}
			else if (datatype == _T("VT_R4"))
			{
				dt = VT_R4;
			}
			else if (datatype == _T("VT_R8"))
			{
				dt = VT_R8;
			}
			else if (datatype == _T("VT_BOOL"))
			{
				dt = VT_BOOL;
			}
			else if (datatype == _T("VT_DATE"))
			{
				dt = VT_DATE;
			}
			else if (datatype == _T("VT_CY"))
			{
				dt = VT_CY;
			}
			else if (datatype == _T("VT_BSTR_ARRAY"))
			{
				dt = VT_BSTR | VT_ARRAY;
			}
			else if (datatype == _T("VT_UI1_ARRAY"))
			{
				dt = VT_UI1 | VT_ARRAY;
			}
			else if (datatype == _T("VT_I1_ARRAY"))
			{
				dt = VT_I1 | VT_ARRAY;
			}
			else if (datatype == _T("VT_UI2_ARRAY"))
			{
				dt = VT_UI2 | VT_ARRAY;
			}
			else if (datatype == _T("VT_I2_ARRAY"))
			{
				dt = VT_I2 | VT_ARRAY;
			}
			else if (datatype == _T("VT_UI4_ARRAY"))
			{
				dt = VT_UI4 | VT_ARRAY;
			}
			else if (datatype == _T("VT_I4_ARRAY"))
			{
				dt = VT_I4 | VT_ARRAY;
			}
			else if (datatype == _T("VT_UI8_ARRAY"))
			{
				dt = VT_UI8 | VT_ARRAY;
			}
			else if (datatype == _T("VT_I8_ARRAY"))
			{
				dt = VT_I8 | VT_ARRAY;
			}
			else if (datatype == _T("VT_UINT_ARRAY"))
			{
				dt = VT_UINT | VT_ARRAY;
			}
			else if (datatype == _T("VT_INT_ARRAY"))
			{
				dt = VT_INT | VT_ARRAY;
			}
			else if (datatype == _T("VT_R4_ARRAY"))
			{
				dt = VT_R4 | VT_ARRAY;
			}
			else if (datatype == _T("VT_R8_ARRAY"))
			{
				dt = VT_R8 | VT_ARRAY;
			}
			else if (datatype == _T("VT_BOOL_ARRAY"))
			{
				dt = VT_BOOL | VT_ARRAY;
			}
			else if (datatype == _T("VT_DATE_ARRAY"))
			{
				dt = VT_DATE | VT_ARRAY;
			}
			else if (datatype == _T("VT_CY_ARRAY"))
			{
				dt = VT_CY | VT_ARRAY;
			}
			else if (datatype == _T("VT_VARIANT_ARRAY"))
			{
				dt = VT_VARIANT | VT_ARRAY;
			}

			if ((dt & VT_ARRAY) == VT_ARRAY)
			{
				arraySize = _tcstoul(datatypeStrEnd + 1, NULL, 10);
			}
		}
	}

	tag->setNativeDatatype(dt);
	tag->setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	SODmSServerDA::initCache(tag, arraySize);
} // initTag


void SODmSServerDA::initCache(SODaSItemTag* pItemTag, DWORD arraySize)
{
	VARTYPE datatype = pItemTag->getNativeDatatype();

	if (datatype & VT_ARRAY)
	{
		// initialize the array values
		SOCmnPointer<SODaSCache> cache = pItemTag->getCache();
		SOCmnVariant value;
		SAFEARRAYBOUND dim;
		void* voidArray;
		DWORD i;
		dim.lLbound = 0;
		dim.cElements = arraySize;
		value.vt = datatype;
		value.parray = SOCmnVariant::safeArrayCreate(value.vt & ~VT_ARRAY, 1, &dim);
		SafeArrayAccessData(value.parray, (void**)&voidArray);

		for (i = 0; i < arraySize; i++)
		{
			switch (value.vt & ~VT_ARRAY)
			{
			case VT_BOOL:
			{
				VARIANT_BOOL* pVal = ((VARIANT_BOOL*)voidArray) + i;
				*pVal = (VARIANT_BOOL)(i % 2);
			}
			break;

			case VT_I1:
			{
				char* pVal = ((char*)voidArray) + i;
				*pVal = (char)i;
			}
			break;

			case VT_UI1:
			{
				unsigned char* pVal = ((unsigned char*)voidArray) + i;
				*pVal = (unsigned char)i;
			}
			break;

			case VT_I2:
			{
				SHORT* pVal = ((SHORT*)voidArray) + i;
				*pVal = (SHORT)i;
			}
			break;

			case VT_UI2:
			{
				WORD* pVal = ((WORD*)voidArray) + i;
				*pVal = (WORD)i;
			}
			break;

			case VT_I4:
			{
				LONG* pVal = ((LONG*)voidArray) + i;
				*pVal = (LONG)i;
			}
			break;

			case VT_UI4:
			{
				DWORD* pVal = ((DWORD*)voidArray) + i;
				*pVal = (DWORD)i;
			}
			break;

			case VT_I8:
			{
				LONGLONG* pVal = ((LONGLONG*)voidArray) + i;
				*pVal = (LONGLONG)i;
			}
			break;

			case VT_UI8:
			{
				ULONGLONG* pVal = ((ULONGLONG*)voidArray) + i;
				*pVal = (ULONGLONG)i;
			}
			break;

			case VT_INT:
			{
				int* pVal = ((int*)voidArray) + i;
				*pVal = (int)i;
			}
			break;

			case VT_UINT:
			{
				unsigned int* pVal = ((unsigned int*)voidArray) + i;
				*pVal = (unsigned int)i;
			}
			break;

			case VT_R4:
			{
				float* pVal = ((float*)voidArray) + i;
				*pVal = (float)i;
			}
			break;

			case VT_R8:
			{
				double* pVal = ((double*)voidArray) + i;
				*pVal = (double)i;
			}
			break;

			case VT_DATE:
			{
				DATE* pVal = ((DATE*)voidArray) + i;
				SOCmnDateTime sysTime;
				sysTime.now();
				sysTime.get(pVal);
			}
			break;

			case VT_CY:
			{
				CY* pVal = ((CY*)voidArray) + i;
				SOCmnVariant cy((int)i);
				cy.changeDatatype(VT_CY);
				pVal->int64 = cy.cyVal.int64;
			}
			break;

			case VT_BSTR:
			{
				DWORD j = i % 26;
#ifdef SOOS_WINDOWS
				BSTR* pVal = ((BSTR*)voidArray) + i;
				unsigned short wstr[3];
				wstr[0] = L'A' + (unsigned short)j;
				wstr[1] = L'a' + (unsigned short)(j + 1);
				wstr[2] = 0;
				*pVal = SysAllocString((WCHAR*)wstr);
#else // SOOS_LINUX
				BSTR* pVal = ((BSTR*)voidArray) + i;
				char wstr[3];
				wstr[0] = (char)('A' + j);
				wstr[1] = (char)('a' + (j + 1));
				wstr[2] = 0;
				*pVal = SysAllocString(wstr);
#endif
			}
			break;

			case VT_VARIANT:
			{
				VARIANT* pVal = ((VARIANT*)voidArray) + i;
				pVal->vt = VT_I2;
				pVal->iVal = (SHORT)i;
			}
			break;
			}
		}

		SafeArrayUnaccessData(value.parray);
		cache->set(value, OPC_QUALITY_GOOD);
	}
	else if (datatype == VT_BSTR)
	{
		SOCmnPointer<SODaSCache> cache = pItemTag->getCache();
		SOCmnVariant value(_T("abc ZYX"));
		cache->set(value, OPC_QUALITY_GOOD);
	}
	else if (datatype == VT_DATE)
	{
		SOCmnPointer<SODaSCache> cache = pItemTag->getCache();
		DATE date;
		SOCmnDateTime sysTime;
		sysTime.now();
		sysTime.get(&date);
		SOCmnVariant value(date);
		cache->set(value, OPC_QUALITY_GOOD);
	}
	else
	{
		SOCmnPointer<SODaSCache> cache = pItemTag->getCache();
		SOCmnVariant value((BYTE)0);
		cache->set(value, OPC_QUALITY_GOOD);
	}
}

BOOL SODmSServerDA::getVendorXMLNamespace(IN WORD forWhat, OUT  SOCmnString& szVendorNS)
{
	switch (forWhat)
	{
	case SODASSERVER_XMLNS_PROPERTY:
		szVendorNS = _T("http://softing.com/OPCToolboxDemo"); // vendor specific properties
		break;

	case SODASSERVER_XMLNS_ERROR:
	default:
		szVendorNS.empty(); // no vendor specific error codes
		break;
	}

	return (!szVendorNS.isEmpty());
}


DWORD SODmSServerDA::getPropertyIDByName(IN LPCTSTR propName)
{
	DWORD id;
	id = SODaSServer::getPropertyIDByName(propName);

	if (id == 0)
	{
		// no default property
		if (_tcscmp(propName, _T("http://softing.com/OPCToolboxDemo|Increment by")) == 0)
		{
			id = SODMS_DA_PROPERTY_INC_BY;
		}
		else if (_tcscmp(propName, _T("http://softing.com/OPCToolboxDemo|Angle")) == 0)
		{
			id = SODMS_DA_PROPERTY_ANGLE;
		}
		else if (_tcscmp(propName, _T("http://softing.com/OPCToolboxDemo|Increment on")) == 0)
		{
			id = SODMS_DA_PROPERTY_INC_ON;
		}
	}

	return id;
}

BOOL SODmSServerDA::isPrivateSecurityAvailable(void)
{
	return TRUE;
}

BOOL SODmSServerDA::logon(LPCTSTR userName, LPCTSTR password)
{
	if ((_tcscmp(userName, _T("Softing")) == 0) && (_tcscmp(password, _T("Softing")) == 0))
	{
		m_accessLevel = 1;
	}
	else if (_tcscmp(userName, _T("NoRights")) == 0)
	{
		m_accessLevel = -1;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SODmSServerDA::logoff(void)
{
	m_accessLevel = 0;
	return TRUE;
}

#ifdef SOFEATURE_DCOM
HRESULT SODmSServerDA::checkAuthorizationDCOM(IN LPCTSTR ifFunction)
{
	HRESULT res = S_OK;

	if (m_accessLevel < 0)
	{
		res = E_ACCESSDENIED;
	}

	return res;
}
#endif

#ifdef SOFEATURE_SOAP
BOOL SODmSServerDA::checkAuthorization(IN SODaSXmlMethod* pMth)
{
#ifndef SOFLAG_DEMO_VERSION
	SOCmnPointer<SOSrvHTTPHandler> httpH = pMth->getHTTPHandler();
	SOCmnStringList attributeList(httpH->getAttributeList());
	SOCmnString authorization;

	if (attributeList.findKey(_T("authorization"), &authorization))
	{
		if (_tcsncmp(_T("Basic "), authorization, 6) == 0)
		{
			SOCmnString userPwd64;
			SOCmnString userPwd;
			userPwd64 = authorization.right(authorization.getLength() - 6);
			SOCmnXmlValue::decodeBase64String2String(userPwd64, userPwd);

			if (userPwd == _T("Softing:Softing"))
			{
				m_accessLevel = 1;
			}
			else if (_tcsncmp(userPwd, _T("NoRights:"), 9) == 0)
			{
				m_accessLevel = -1;
			}
		}
	}
	else
	{
		m_accessLevel = 0;
	}

	if (m_accessLevel < 0)
	{
		SOCmnPointer<SOSrvHTTPHandlerData> httpD = pMth->getHTTPHandlerData();
		httpH->sndHTTPErrorResponse(httpD, pMth->getFlags(), SOCMNHTTP_CODE_UNAUTHORIZED);
	}

#endif
	return (m_accessLevel >= 0);
}
#endif


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//      E_NOTIMPL     - no query interface implementation for this interface
//      S_OK          - interface delivered
//      E_NOINTERFACE - interface not supported
//
HRESULT SODmSServerDA::queryInterface(IN REFIID iid, IN void** object)
{
	if (g_opcSpecSupport == 0x07)
	{
		return E_NOTIMPL;    // support all interfaces
	}

	if (g_opcSpecSupport == 0x04) // DA3
	{
		if ((iid == IID_IOPCBrowseServerAddressSpace) ||
			(iid == IID_IOPCServerPublicGroups) ||
			(iid == IID_IOPCItemProperties) ||
			(iid == IID_IPersistFile))
		{
			return E_NOINTERFACE;
		}
	}

	if (g_opcSpecSupport == 0x02) // DA2
	{
		if ((iid == IID_IOPCBrowse) ||
			(iid == IID_IOPCItemIO))
		{
			return E_NOINTERFACE;
		}
	}

	if (g_opcSpecSupport == 0x01) // DA1
	{
		if ((iid == IID_IOPCBrowse) ||
			(iid == IID_IOPCCommon) ||
			(iid == IID_IOPCItemProperties) ||
			(iid == IID_IConnectionPointContainer) ||
			(iid == IID_IOPCItemIO))
		{
			return E_NOINTERFACE;
		}
	}

	return E_NOTIMPL;
}

#endif


//-----------------------------------------------------------------------------
// SODmSGroup                                                                 |
//-----------------------------------------------------------------------------

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//      E_NOTIMPL     - no query interface implementation for this interface
//      S_OK          - interface delivered
//      E_NOINTERFACE - interface not supported
//
HRESULT SODmSGroup::queryInterface(IN REFIID iid, IN void** object)
{
	if (g_opcSpecSupport == 0x07)
	{
		return E_NOTIMPL;    // support all interfaces
	}

	if (g_opcSpecSupport == 0x04) // DA3
	{
		if ((iid == IID_IOPCGroupStateMgt) ||
			(iid == IID_IOPCPublicGroupStateMgt) ||
			(iid == IID_IOPCAsyncIO) ||
			(iid == IID_IDataObject))
		{
			return E_NOINTERFACE;
		}
	}

	if (g_opcSpecSupport == 0x02) // DA2
	{
		if ((iid == IID_IOPCGroupStateMgt2) ||
			(iid == IID_IOPCAsyncIO) ||
			(iid == IID_IOPCAsyncIO3) ||
			(iid == IID_IOPCSyncIO2) ||
			(iid == IID_IOPCItemDeadbandMgt) ||
			(iid == IID_IDataObject))
		{
			return E_NOINTERFACE;
		}
	}

	if (g_opcSpecSupport == 0x01) // DA1
	{
		if ((iid == IID_IOPCGroupStateMgt2) ||
			(iid == IID_IOPCAsyncIO2) ||
			(iid == IID_IOPCAsyncIO3) ||
			(iid == IID_IOPCSyncIO2) ||
			(iid == IID_IOPCItemDeadbandMgt) ||
			(iid == IID_IConnectionPointContainer))
		{
			return E_NOINTERFACE;
		}
	}

	return E_NOTIMPL;
}

#endif


//-----------------------------------------------------------------------------
// SODmSTagAccept                                                             |
//-----------------------------------------------------------------------------

SODmSTagAccept::SODmSTagAccept(void)
{
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_ACCEPT);
	SODmSServerDA::initCache(this, 2);
}


SODmSTagAccept::SODmSTagAccept(
	IN LPCTSTR tagName,     // name
	IN VARTYPE datatype,    // datatype
	IN DWORD accessRights)// access rights
{
	setName(tagName);
	setAccessRights(accessRights);
	setNativeDatatype(datatype);
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_ACCEPT);
	SODmSServerDA::initCache(this, 2);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagAccept::handleRequest(
	IN SODaSRequest* req)   // request
{
	req->complete();
} // handleRequest


//-----------------------------------------------------------------------------
// supportWriteQT
//
// - check if writing of qualtity and/or timestamp is supported
//
// returns:
//      TRUE  - supported
//      FALSE - not supported
//
BOOL SODmSTagAccept::supportWriteQT(
	IN BYTE flagsQT) // SODASREQUEST_FLAG_QUALITY and SODASREQUEST_FLAG_TIMESTAMP
{
	return TRUE;
} // supportWriteQT



//-----------------------------------------------------------------------------
// SODmSTagTime                                                               |
//-----------------------------------------------------------------------------

SODmSTagTime::SODmSTagTime(
	IN LPCTSTR tagName,             // name
	IN enum tagType type,           // type
	IN enum tagTimeZone timeZone)   // time zone
{
	setName(tagName);
	setAccessRights(OPC_READABLE);

	switch (type)
	{
	case hour:
	case second:
	case minute:
		setNativeDatatype(VT_UI1);
		break;

	case string:
		setNativeDatatype(VT_BSTR);
		break;

	case array:
		setNativeDatatype(VT_UI1 | VT_ARRAY);
		break;

	case date:
		setNativeDatatype(VT_DATE);
		break;

	case limitSecond:
		setAccessRights(OPC_READABLE | OPC_WRITEABLE);
		setNativeDatatype(VT_UI1);
		break;
	}

	m_tagType = type;
	m_tagTimeZone = timeZone;
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_TIME);
}


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagTime::handleRequest(
	IN SODaSRequest* req)   // request
{
	if (req->getOperation() == SODaSRequest::read)
	{
		// read
		SOCmnPointer<SODaSCache> cache = req->getCache();
		SOCmnVariant value;

		if (m_tagType != date)
		{
#ifdef SOOS_WINDOWS
			SYSTEMTIME now;

			switch (m_tagTimeZone)
			{
			case local:
				GetLocalTime(&now);
				break;

			case GMT:
				GetSystemTime(&now);
				break;
			}

			switch (m_tagType)
			{
			case hour:
				value = (unsigned char)now.wHour;
				break;

			case second:
				value = (unsigned char)now.wSecond;
				break;

			case minute:
				value = (unsigned char)now.wMinute;
				break;

			case date:
				value.vt = VT_DATE;
				SystemTimeToVariantTime(&now, &value.date);
				break;

			case array:
			{
				SAFEARRAYBOUND dim;
				unsigned char* timeArray;
				dim.lLbound = 0;
				dim.cElements = 3;
				value.vt = VT_UI1 | VT_ARRAY;
				value.parray = SOCmnVariant::safeArrayCreate(VT_UI1, 1, &dim);
				SafeArrayAccessData(value.parray, (void**)&timeArray);
				timeArray[0] = (unsigned char)now.wHour;
				timeArray[1] = (unsigned char)now.wMinute;
				timeArray[2] = (unsigned char)now.wSecond;
				SafeArrayUnaccessData(value.parray);
			}
			break;

			case string:
			{
				SOCmnString stringTime;
				stringTime.format(_T("%2.2d:%2.2d:%2.2d"), now.wHour, now.wMinute, now.wSecond);
				value = stringTime;
			}
			break;

			case limitSecond:
			{
				cache->getValue(value);
			}
			break;
			}

#else // SOOS_LINUX
			struct timeval tv;
			struct tm now;
			gettimeofday(&tv, NULL);

			switch (m_tagTimeZone)
			{
			case local:
				localtime_r(&tv.tv_sec, &now);
				break;

			case GMT:
				gmtime_r(&tv.tv_sec, &now);
				break;

			default:
				break;
			}

			switch (m_tagType)
			{
			case hour:
				value = (unsigned char)now.tm_hour;
				break;

			case second:
				value = (unsigned char)now.tm_sec;
				break;

			case minute:
				value = (unsigned char)now.tm_min;
				break;

			case date:
				break;

			case array:
			{
				SAFEARRAYBOUND dim;
				unsigned char* timeArray;
				dim.lLbound = 0;
				dim.cElements = 3;
				value.vt = VT_UI1 | VT_ARRAY;
				value.parray = SOCmnVariant::safeArrayCreate(VT_UI1, 1, &dim);
				SafeArrayAccessData(value.parray, (void**)&timeArray);
				timeArray[0] = (unsigned char)now.tm_hour;
				timeArray[1] = (unsigned char)now.tm_min;
				timeArray[2] = (unsigned char)now.tm_sec;
				SafeArrayUnaccessData(value.parray);
			}
			break;

			case string:
			{
				SOCmnString stringTime;
				stringTime.format(_T("%2.2d:%2.2d:%2.2d"), now.tm_hour, now.tm_min, now.tm_sec);
				value = stringTime;
			}
			break;

			case limitSecond:
			{
				cache->getValue(value);
			}
			break;
			}

#endif // SOOS_LINUX
		}
		else
		{
			// date
			SOCmnDateTime now;
			value.vt = VT_DATE;
			now.now();
			now.get(&value.date);
		}

		cache->set(value, OPC_QUALITY_GOOD);
		req->complete();
	}
	else
	{
		// write
		HRESULT res = E_FAIL;

		if (m_tagType == limitSecond)
		{
			SOCmnVariant writeValue;
			req->getWriteValue(writeValue);

			if (writeValue.changeDatatype(VT_UI1))
			{
				if (writeValue.bVal <= 59)
				{
					res = S_OK; // OK, take the value
				}
			}
		}

		req->complete(res);
	}
} // handleRequest


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODmSTagTime::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SODmSTagInc                                                                |
//-----------------------------------------------------------------------------

SODmSTagInc::SODmSTagInc(
	IN LPCTSTR tagName,     // name
	IN VARTYPE datatype)    // datatype
{
	setName(tagName);
	setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	setNativeDatatype(datatype);
	setDeviceIOMode(direct);
	// create by property
	SOCmnPointer<SODaSProperty> propBy = new SODaSProperty(SODMS_DA_PROPERTY_INC_BY, _T("vsp:Increment by"));
	propBy->setDescription(_T("Increment by ..."));

	if (((datatype & ~VT_ARRAY) != VT_BSTR) && ((datatype & ~VT_ARRAY) != VT_DATE) && (datatype != (VT_ARRAY | VT_VARIANT)))
	{
		propBy->setNativeDatatype(datatype & ~VT_ARRAY);
	}
	else
	{
		propBy->setNativeDatatype(VT_BOOL);
	}

	propBy->setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	addProperty(propBy);
	m_by = propBy->getCache();
	SOCmnVariant varHlp;

	if (g_incrementOn)
	{
		varHlp = (BYTE)1;
		m_by->set(varHlp, OPC_QUALITY_GOOD);
	}
	else
	{
		varHlp = (BYTE)0;
		m_by->set(varHlp, OPC_QUALITY_GOOD);
	}

	SODmSServerDA::initCache(this, 5);
	setUserData(SODMS_DA_TAG_INC);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagInc::handleRequest(
	IN SODaSRequest* req)   // request
{
	if (req->getOperation() == SODaSRequest::read)
	{
		// read
		if (g_incrementOn)
		{
			SOCmnPointer<SODaSCache> cache = req->getCache();
			SOCmnVariant value;
			SOCmnVariant by;
			cache->getValue(value);
			m_by->getValue(by);
			doIncrement(value, by);
			cache->set(value, OPC_QUALITY_GOOD);
		}

		req->complete();
	}
	else
	{
		// write
		req->complete();
	}
} // handleRequest

void SODmSTagInc::doIncrement(SOCmnVariant& value, SOCmnVariant& by)
{
	switch (value.vt)
	{
	case VT_BOOL:
		value.boolVal = VARIANT_BOOL(((int)value.boolVal + (int)by.boolVal) % 2);
		break;

	case VT_I1:
		value.cVal += by.cVal;
		break;

	case VT_UI1:
		value.bVal += by.bVal;
		break;

	case VT_I2:
		value.iVal += by.iVal;
		break;

	case VT_UI2:
		value.uiVal += by.uiVal;
		break;

	case VT_I4:
		value.lVal += by.lVal;
		break;

	case VT_UI4:
		value.ulVal += by.ulVal;
		break;

	case VT_INT:
		value.intVal += by.intVal;
		break;

	case VT_UINT:
		value.uintVal += by.uintVal;
		break;

	case VT_I8:
		V_I8(&value) += V_I8(&by);
		break;

	case VT_UI8:
		V_UI8(&value) += V_UI8(&by);
		break;

	case VT_R4:
		value.fltVal += by.fltVal;
		break;

	case VT_R8:
		value.dblVal += by.dblVal;
		break;

	case VT_DATE:
		incrementDate(&value.date, (BOOL)by.boolVal);
		break;

	case VT_CY:
		value.cyVal.int64 += by.cyVal.int64;
		break;

	case VT_BSTR:
		incrementWString(value.bstrVal, (BOOL)by.boolVal);
		break;

	case (VT_I1 | VT_ARRAY):
	case (VT_UI1 | VT_ARRAY):
	case (VT_I2 | VT_ARRAY):
	case (VT_UI2 | VT_ARRAY):
	case (VT_I4 | VT_ARRAY):
	case (VT_UI4 | VT_ARRAY):
	case (VT_I8 | VT_ARRAY):
	case (VT_UI8 | VT_ARRAY):
	case (VT_INT | VT_ARRAY):
	case (VT_UINT | VT_ARRAY):
	case (VT_R4 | VT_ARRAY):
	case (VT_R8 | VT_ARRAY):
	case (VT_BOOL | VT_ARRAY):
	case (VT_BSTR | VT_ARRAY):
	case (VT_CY | VT_ARRAY):
	case (VT_DATE | VT_ARRAY):
	case (VT_VARIANT | VT_ARRAY):
	{
		LONG lBound, uBound;
		LONG idx;
		void* voidArray;
		::SafeArrayGetLBound(value.parray, 1, &lBound);
		::SafeArrayGetUBound(value.parray, 1, &uBound);
		::SafeArrayAccessData(value.parray, (void**)&voidArray);

		for (idx = lBound; idx <= uBound; idx++)
		{
			switch (value.vt & ~VT_ARRAY)
			{
			case VT_BOOL:
			{
				VARIANT_BOOL* boolVal = ((VARIANT_BOOL*)voidArray) + idx;
				*boolVal = VARIANT_BOOL(((int) * boolVal + (int)by.boolVal) % 2);
			}
			break;

			case VT_I1:
			{
				char* cVal = ((char*)voidArray) + idx;
				*cVal += by.cVal;
			}
			break;

			case VT_UI1:
			{
				unsigned char* bVal = ((unsigned char*)voidArray) + idx;
				*bVal += by.bVal;
			}
			break;

			case VT_I2:
			{
				SHORT* iVal = ((SHORT*)voidArray) + idx;
				*iVal += by.iVal;
			}
			break;

			case VT_UI2:
			{
				WORD* uiVal = ((WORD*)voidArray) + idx;
				*uiVal += by.uiVal;
			}
			break;

			case VT_I4:
			{
				LONG* lVal = ((LONG*)voidArray) + idx;
				*lVal += by.lVal;
			}
			break;

			case VT_UI4:
			{
				DWORD* ulVal = ((DWORD*)voidArray) + idx;
				*ulVal += by.ulVal;
			}
			break;

			case VT_I8:
			{
				LONGLONG* llVal = ((LONGLONG*)voidArray) + idx;
				*llVal += V_I8(&by);
			}
			break;

			case VT_UI8:
			{
				ULONGLONG* ullVal = ((ULONGLONG*)voidArray) + idx;
				*ullVal += V_UI8(&by);
			}
			break;

			case VT_INT:
			{
				int* intVal = ((int*)voidArray) + idx;
				*intVal += by.intVal;
			}
			break;

			case VT_UINT:
			{
				unsigned int* uintVal = ((unsigned int*)voidArray) + idx;
				*uintVal += by.uintVal;
			}
			break;

			case VT_R4:
			{
				float* fltVal = ((float*)voidArray) + idx;
				*fltVal += by.fltVal;
			}
			break;

			case VT_R8:
			{
				double* dblVal = ((double*)voidArray) + idx;
				*dblVal += by.dblVal;
			}
			break;

			case VT_BSTR:
			{
				BSTR* bstrVal = ((BSTR*)voidArray) + idx;
				incrementWString(*bstrVal, (BOOL)by.boolVal);
			}
			break;

			case VT_DATE:
			{
				DATE* dateVal = ((DATE*)voidArray) + idx;
				incrementDate(dateVal, (BOOL)by.boolVal);
			}
			break;

			case VT_CY:
			{
				CY* cyVal = ((CY*)voidArray) + idx;
				cyVal->int64 += by.cyVal.int64;
			}
			break;

			case VT_VARIANT:
			{
				SOCmnVariant val(((VARIANT*)voidArray) + idx);
				doIncrement(val, by);
				SOCmnVariant::variantClear(((VARIANT*)voidArray) + idx);
				SOCmnVariant::variantCopy((((VARIANT*)voidArray) + idx), val);
			}
			break;
			}
		}

		::SafeArrayUnaccessData(value.parray);
	}
	break;
	}
} // doIncrement


//-----------------------------------------------------------------------------
// handlePropertyRequest
//
// - handle request for properties direct I/O
//
BOOL SODmSTagInc::handlePropertyRequest(
	IN SODaSProperty* prop, // property
	IN SODaSRequest* req)   // request
{
	if (prop->getNameInt() == SODMS_DA_PROPERTY_INC_BY)
	{
		// by property
		HRESULT res = S_OK;

		if (req->getOperation() == SODaSRequest::write)
		{
			SOCmnVariant writeValue;
			res = E_FAIL;
			req->getWriteValue(writeValue);

			if (writeValue.vt == VT_BOOL)
			{
				res = S_OK;
			}
			else if (writeValue.changeDatatype(VT_UI2))
			{
				if (writeValue.uiVal <= 359)
				{
					res = S_OK; // OK, take the value
				}
			}
		}

		req->complete(res);
		return TRUE;
	}

	return SODaSTag::handlePropertyRequest(prop, req);
} // handlePropertyRequest

//-----------------------------------------------------------------------------
// incrementWString
//
// - increment unicode string
//
void SODmSTagInc::incrementWString(
	IN BSTR w,  // unicode string
	IN BOOL doinc)  // do increment
{
	if ((w) && (doinc))
	{
		while (*w != 0)
		{
			(*w)++;

			if (*w == OLESTR('Z') + 1)
			{
				*w = OLESTR('A');
			}

			if (*w == OLESTR('z') + 1)
			{
				*w = OLESTR('a');
			}

			if (*w == OLESTR('9') + 1)
			{
				*w = OLESTR('0');
			}

			if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
				  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
				  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
			{
				*w = L' ';
			}

			w++;
		}
	}
} // incrementWString



//-----------------------------------------------------------------------------
// incrementDate
//
// - increment date
//
void SODmSTagInc::incrementDate(
	IN DATE* date, // date
	IN BOOL doinc) // do increment
{
	if ((date) && (doinc))
	{
#ifdef SOOS_WINDOWS
		*date += 1.0 / (24 * 3600);
#else // SOOS_LINUX
		++date->tv_sec;
#endif
	}
} // incrementDate


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODmSTagInc::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SODmSNodeInc                                                             |
//-----------------------------------------------------------------------------

SODmSNodeInc::SODmSNodeInc(IN LPCTSTR nodeName)
{
	setName(nodeName);
}


//-----------------------------------------------------------------------------
// handlePropertyRequest
//
// - handle I/O request for child property
//
// returns:
//		TRUE  - request handled
//		FALSE - can't handle request
//
BOOL SODmSNodeInc::handlePropertyRequest(
	IN SODaSProperty* prop,
	IN SODaSRequest* request)
{
	if (prop->getPropertyId() == 5000)
	{
		if (request->getOperation() == SODaSRequest::write)
		{
			SOCmnVariant wV;
			request->getWriteValue(wV);
			g_incrementOn = (wV.boolVal == VARIANT_TRUE);
			request->complete(); // accept written value
		}
		else
		{
			request->complete();    // return cache value
		}

		return TRUE;
	}

	return FALSE;
} // handlePropertyRequest



//-----------------------------------------------------------------------------
// SODmSTagMaths                                                              |
//-----------------------------------------------------------------------------

SODmSTagMaths::SODmSTagMaths(
	IN LPCTSTR tagName,     // name
	IN enum tagType type)   // type
{
	setName(tagName);
	setDeviceIOMode(direct);
	m_tagType = type;
	setAccessRights(OPC_READABLE);
	setNativeDatatype(VT_R8);
	setUserData(SODMS_DA_TAG_MATHS);
	// create angle property
	SOCmnPointer<SODaSProperty> propAngle = new SODaSProperty(SODMS_DA_PROPERTY_ANGLE, _T("vsp:Angle"));
	propAngle->setDescription(_T("Angle"));
	propAngle->setNativeDatatype(VT_UI2);
	propAngle->setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	addProperty(propAngle);
	m_angle = propAngle->getCache();
	m_angle->setQuality(OPC_QUALITY_GOOD);
}


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagMaths::handleRequest(
	IN SODaSRequest* req)   // request
{
	if (req->getOperation() == SODaSRequest::read)
	{
		// read
		SOCmnPointer<SODaSCache> cache = req->getCache();
		SOCmnVariant value;
		double radianAngle = 0.0;
		// calc radian angle
		SOCmnVariant angle;
		static double pi = 3.1415926535;
		m_angle->getValue(angle);
		radianAngle = (2 * pi) * ((double)angle.uiVal / 360.0);

		switch (m_tagType)
		{
		case sin:
			value = ::sin(radianAngle);
			break;

		case cos:
			value = ::cos(radianAngle);
			break;

		case tan:
			value = ::tan(radianAngle);
			break;
		}

		cache->set(value, OPC_QUALITY_GOOD);
		req->complete();
	}
	else
	{
		// write
		req->complete(E_FAIL);
	}
} // handleRequest


//-----------------------------------------------------------------------------
// handlePropertyRequest
//
// - handle request for properties direct I/O
//
BOOL SODmSTagMaths::handlePropertyRequest(
	IN SODaSProperty* prop, // property
	IN SODaSRequest* req)   // request
{
	if (prop->getNameInt() == SODMS_DA_PROPERTY_ANGLE)
	{
		// by property
		req->complete();    // take the write value, the read value is in cache
		return TRUE;
	}

	return SODaSTag::handlePropertyRequest(prop, req);
} // handlePropertyRequest


//-----------------------------------------------------------------------------
// incAngle
//
// - increment the angle property
//
void SODmSTagMaths::incAngle(void)
{
	SOCmnVariant angle;
	m_angle->getValue(angle);
	angle.uiVal++;

	if (angle.uiVal >= 360)
	{
		angle.uiVal = 0;
	}

	m_angle->setValue(angle);
} // incAngle


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODmSTagMaths::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SODmSNodeWatch                                                             |
//-----------------------------------------------------------------------------

SODmSNodeWatch::SODmSNodeWatch(
	IN LPCTSTR nodeName, // name
	IN LPCTSTR devType,  // device type
	IN DWORD devAddress, // device address
	IN LPCTSTR devState, // device state
	IN LPCTSTR devStateInfo) // device state information
{
	setName(nodeName);
	m_devType = devType;
	m_devAddress = devAddress;
	m_devState = devState;
	m_devStateInfo = devStateInfo;
}

//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the object
//
SOCmnString SODmSNodeWatch::getDeviceState(void)
{
	return m_devState;
} // getDeviceState


//-----------------------------------------------------------------------------
// getDeviceStateInfo
//
// - get state info of the device represented by the object
//
// returns:
//		the device state info  of the object
//
SOCmnString SODmSNodeWatch::getDeviceStateInfo(void)
{
	return m_devStateInfo;
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODmSNodeWatch::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	if (withAttributes)
	{
		SODaSNode::getObjWatchData(szXML, withAttributes, objNodeName);
		// add the server specific tag attributes
		SOCmnString buffer;
		SOSrvWatch::addElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T, _T("deviceAddress"), buffer.format(_T("%lu"), m_devAddress));
		SOSrvWatch::addElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T, _T("deviceType"), m_devType);
	}
	else
	{
		SODaSNode::getObjWatchData(szXML, withAttributes, objNodeName);
	}
}

#endif // SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// setDeviceState
//
// - set the device state of the object
//
void SODmSNodeWatch::setDeviceState(
	IN LPCTSTR devState) // device state
{
	m_devState = devState;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_DEVSTATE_T, m_devState);
	}

#endif
} // setDeviceState


//-----------------------------------------------------------------------------
// setDeviceStateInfo
//
// - set the device state info of the object
//
void SODmSNodeWatch::setDeviceStateInfo(
	IN LPCTSTR devStateInfo) // device state info
{
	m_devStateInfo = devStateInfo;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, m_devStateInfo);
	}

#endif
} // setDeviceStateInfo




//-----------------------------------------------------------------------------
// SODmSTagWatch                                                              |
//-----------------------------------------------------------------------------

SODmSTagWatch::SODmSTagWatch(
	IN LPCTSTR tagName,      // name
	IN VARTYPE datatype,     // datatype
	IN DWORD accessRights, // access rights
	IN WORD offset)        // process image offset
{
	setName(tagName);
	setAccessRights(accessRights);
	setNativeDatatype(datatype);
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_WATCH);
	m_offset = offset;
	SOCmnPointer<SODaSCache> cache;
	cache = getCache();
	SOCmnVariant b0((BYTE)0);
	cache->set(b0, OPC_QUALITY_GOOD);
}


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagWatch::handleRequest(
	IN SODaSRequest* req)   // request
{
	SOCmnPointer<SODmSNodeWatch> parentNode = (SODmSNodeWatch*)getParentBranch();
	SOCmnPointer<SODaSCache> cache;
	cache = getCache();
	cache->setQuality((parentNode->getDeviceState() == SOWATCH_DEVSTATE_OK) ? OPC_QUALITY_GOOD : OPC_QUALITY_COMM_FAILURE);
	req->complete();
} // handleRequest


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the parent object
//
SOCmnString SODmSTagWatch::getDeviceState(void)
{
	SOCmnPointer<SODmSNodeWatch> parentNode = (SODmSNodeWatch*)getParentBranch();
	return parentNode->getDeviceState();
} // getDeviceState


//-----------------------------------------------------------------------------
// getDeviceStateInfo
//
// - get state info of the device represented by the object
//
// returns:
//		the device state info of the parent object
//
SOCmnString SODmSTagWatch::getDeviceStateInfo(void)
{
	SOCmnPointer<SODmSNodeWatch> parentNode = (SODmSNodeWatch*)getParentBranch();
	return parentNode->getDeviceStateInfo();
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODmSTagWatch::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	if (withAttributes)
	{
		SODaSTag::getObjWatchData(szXML, withAttributes, objNodeName);
		// add the server specific tag attributes
		SOCmnString buffer;
		SOSrvWatch::addElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T, _T("processImageOffset"), buffer.format(_T("%u"), m_offset));
	}
	else
	{
		SODaSTag::getObjWatchData(szXML, withAttributes, objNodeName);
	}
}
#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SODmSTagError                                                              |
//-----------------------------------------------------------------------------

SODmSTagError::SODmSTagError(
	IN LPCTSTR tagName, // name
	IN enum SODmSTagError::tagType type)    // access rights
{
	SOCmnPointer<SODaSCache> cache = getCache();
	SOCmnVariant v;
	setName(tagName);
	setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	m_tagType = type;

	switch (type)
	{
	case read:
	case write:
		setNativeDatatype(VT_UI4);
		v = (DWORD)0;
		break;

	case quality:
	{
		setNativeDatatype(VT_UI2);
		v = (WORD)OPC_QUALITY_GOOD;
	}
	break;
	}

	cache->set(v, OPC_QUALITY_GOOD);
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_ERROR);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagError::handleRequest(
	IN SODaSRequest* req)   // request
{
	SOCmnPointer<SODaSCache> cache = getCache();
	SOCmnVariant value;

	if (req->getOperation() == SODaSRequest::read)
	{
		if (m_tagType == read)
		{
			cache->getValue(value);

			if (value.ulVal >= (DWORD)0x80000000)
			{
				cache->setQuality(OPC_QUALITY_BAD);    // bad read result set quality to bad
			}

			req->complete(value.ulVal);
		}
		else
		{
			req->complete();
		}
	}
	else
	{
		switch (m_tagType)
		{
		case read:
			req->complete();
			break;

		case write:
		{
			req->getWriteValue(&value);
			cache->setValue(value);
			req->complete(value.ulVal, 0, OPC_QUALITY_GOOD, 0, FALSE);
		}
		break;

		case quality:
			req->getWriteValue(&value);
			req->complete(S_OK, TRUE, value.uiVal);
			break;
		}
	}
} // handleRequest




//-----------------------------------------------------------------------------
// SODmSTagMode                                                               |
//-----------------------------------------------------------------------------

SODmSTagMode::SODmSTagMode(
	IN BYTE updateMode,   // update mode
	IN BYTE ioMode,       // I/O mode
	IN LPCTSTR tagName,     // name
	IN VARTYPE datatype)    // datatype
{
	setName(tagName);
	setAccessRights(OPC_READABLE | OPC_WRITEABLE);
	setNativeDatatype(datatype);
	setDeviceIOMode(ioMode);
	setUpdateMode(updateMode);
	setUserData(SODMS_DA_TAG_MODE);
	m_lastTime.now();
	SODmSServerDA::initCache(this, 2);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagMode::handleRequest(
	IN SODaSRequest* req)   // request
{
	if (req->getOperation() == SODaSRequest::read)
	{
		// read
		SOCmnPointer<SODaSCache> cache = getCache();
		SOCmnDateTime now;
		SHORT secondsToAdd = 0;
		now.now();
		secondsToAdd = (SHORT)((now.getMicroseconds() - m_lastTime.getMicroseconds()) / 1000000);
		SOCmnVariant cvalue;
		SOCmnVariant nvalue;
		cache->getValue(cvalue);
		nvalue = (SHORT)(cvalue.iVal + secondsToAdd);
		cache->set(nvalue, OPC_QUALITY_GOOD);

		if (secondsToAdd > 0)
		{
			m_lastTime.now();
		}
	}

	req->complete();
} // handleRequest


//-----------------------------------------------------------------------------
// getReadEvent
//
// - get event for read request queue
//
// returns:
//		handle of read event
//
SOCmnEventPointer SODmSTagMode::getReadEvent(void)
{
	return SOCmnEventPointer(g_ioThread->getEvents(), g_requestQueueEventId);
} // getReadEvent


//-----------------------------------------------------------------------------
// getWriteEvent
//
// - get event for write request queue
//
// returns:
//		handle of write event
//
SOCmnEventPointer SODmSTagMode::getWriteEvent(void)
{
	return SOCmnEventPointer(g_ioThread->getEvents(), g_requestQueueEventId);
} // getWriteEvent


//-----------------------------------------------------------------------------
// getReadQueue
//
// - get read request queue
//
// returns:
//		pointer to read request queue
//
SOCmnList<SODaSRequest> *SODmSTagMode::getReadQueue(void)
{
	return &g_requestQueue;
} // getReadQueue


//-----------------------------------------------------------------------------
// getWriteQueue
//
// - get write request queue
//
// returns:
//		pointer to write request queue
//
SOCmnList<SODaSRequest> *SODmSTagMode::getWriteQueue(void)
{
	return &g_requestQueue;
} // getWriteQueue



//-----------------------------------------------------------------------------
// SODmSNodeDynamic                                                           |
//-----------------------------------------------------------------------------

SODmSNodeDynamic::SODmSNodeDynamic(
	IN LPCTSTR nodeName) // name
{
	setName(nodeName);
	SOCmnListConfig cfg;
	m_leafList.destroy();
	cfg.m_hashTableSize = 1000;
	cfg.m_type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED | SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING;
	m_leafList.create(&cfg);
}



//-----------------------------------------------------------------------------
// SODmSNodeHidden                                                            |
//-----------------------------------------------------------------------------

SODmSNodeHidden::SODmSNodeHidden(
	IN LPCTSTR nodeName) // name
{
	setName(nodeName);
	setUserData(SODMS_DA_TAG_HIDDEN);
}

BOOL SODmSNodeHidden::isBrowsable(IN SOCmnObject* pServer)
{
	return FALSE;
}


//-----------------------------------------------------------------------------
// SODmSTagHidden                                                             |
//-----------------------------------------------------------------------------

SODmSTagHidden::SODmSTagHidden(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights)
	: SODmSTagAccept(tagName, datatype, accessRights)
{
}

BOOL SODmSTagHidden::isBrowsable(IN SOCmnObject* pServer)
{
	return FALSE;
}



//-----------------------------------------------------------------------------
// SODmSTagClient                                                             |
//-----------------------------------------------------------------------------

SODmSTagClient::SODmSTagClient(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights)
{
	setName(tagName);
	setAccessRights(accessRights);
	setNativeDatatype(datatype);
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_CLIENT);
	SOCmnPointer<SODaSCache> cache = getCache();
	SOCmnVariant assert;
	assert = _T("ERROR you should not see this!");
	cache->set(assert, OPC_QUALITY_BAD);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagClient::handleRequest(
	IN SODaSRequest* req)   // request
{
	if (req->getOperation() == SODaSRequest::read)
	{
		req->complete();
	}
	else
	{
		SOCmnPointer<SODmSServerDA> server = (SODmSServerDA*)req->getServer();
		SOCmnDateTime now;
		SOCmnVariant writeValue;
		SOCmnString csString;
		USES_CONVERSION;
		req->getWriteValue(writeValue);
		csString = OLE2T(writeValue.bstrVal);
		now.now();
		server->setClientSpecificTimeStamp(now);
		server->setClientSpecificString(csString);
		req->complete(S_OK, FALSE, OPC_QUALITY_BAD, FALSE, FALSE);
	}
} // handleRequest


//-----------------------------------------------------------------------------
// SODmSTagSecure                                                             |
//-----------------------------------------------------------------------------

SODmSTagSecure::SODmSTagSecure(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights)
{
	setName(tagName);
	setAccessRights(accessRights);
	setNativeDatatype(datatype);
	setDeviceIOMode(direct);
	setUserData(SODMS_DA_TAG_SECURE);
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODmSTagSecure::handleRequest(
	IN SODaSRequest* req)   // request
{
	SOCmnPointer<SODmSServerDA> server = (SODmSServerDA*)req->getServer();

	if (server->getAccessLevel() < 1)
	{
		req->complete(E_ACCESSDENIED);
	}
	else
	{
		SODmSTagAccept::handleRequest(req);
	}
} // handleRequest


//-----------------------------------------------------------------------------
// SODmSCache                                                                 |
//-----------------------------------------------------------------------------

SODmSCache::SODmSCache(void) {}
SODmSCache::~SODmSCache(void) {}

BOOL SODmSCache::getTimeStamp(IN SODaSRequest* req, OUT SOCmnDateTime& timeStamp)
{
	SOCmnPointer<SODmSServerDA> server = (SODmSServerDA*)req->getServer();
	server->getClientSpecificTimeStamp(timeStamp);
	return TRUE;
}

BOOL SODmSCache::getValue(IN SODaSRequest* req, IN VARTYPE typeWanted, IN LCID localeID, OUT SOCmnVariant& value, OPTIONAL OUT HRESULT* pConvRes)
{
	SOCmnPointer<SODmSServerDA> server = (SODmSServerDA*)req->getServer();
	SOCmnString csString;
	BOOL ok = TRUE;
	server->getClientSpecificString(csString);
	value = (LPCTSTR)csString;

	if ((ok) && ((typeWanted != VT_EMPTY) && (typeWanted != (VT_EMPTY | VT_ARRAY)) &&
				 (typeWanted != value.vt)))
	{
		ok = changeDatatype(value, typeWanted, value, localeID, NULL, pConvRes);

		if (!ok)
		{
			SOCmnVariant::variantClear(value);
		}
	}

	return ok;
}

WORD SODmSCache::getQuality(IN SODaSRequest* req)
{
	return OPC_QUALITY_GOOD;
}




//-----------------------------------------------------------------------------
// SODmSTransaction                                                           |
//-----------------------------------------------------------------------------

SODmSTransaction::SODmSTransaction(
	IN SODaSGroup* group,         // group
	IN BYTE type,                 // transaction type
	IN BYTE connectionType,       // connection type
	IN OPTIONAL DWORD connection) // connection id
	: SODaSTransaction(group, type, connectionType, connection)
{
}


//-----------------------------------------------------------------------------
// handleRequests
//
// - handle all transaction requests of the transaction
//
void SODmSTransaction::handleRequests(
	IN SOCmnList<SODaSRequest> *reqList)
{
	SOCmnList<SODaSRequest> list(reqList);
	SOCmnListPosition pos;
	SODaSRequest* pReq;
	SOCmnPointer<SODaSItemTag> itemTag;
	pos = list.getStartPosition();

	while (pos)
	{
		pReq = list.getNext(pos);

		if (!pReq->isCompleted())
		{
			itemTag = pReq->getItemTag();

			if (!itemTag)
			{
				//continue;
			}

			if (itemTag->getDeviceIOMode(pReq) == SODaSItemTag::transaction)
			{
				itemTag->handleRequest(pReq);
			}
		}
	}
} // handleRequests


#endif

#ifdef SODMS_AE

//-----------------------------------------------------------------------------
// SODmSCreatorAE                                                             |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// createServer
//
// - create server object
//
// returns:
//		created object
//
SOAeSServer* SODmSCreatorAE::createServer(
	IN BYTE serverInstance) // server instance number
{
	SOAeSServer* server;
	SOCMN_ALLOCATE_OBJECT(server, SODmSServerAE(serverInstance))
	return server;
} // createServer



//-----------------------------------------------------------------------------
// SODmSServerAE                                                              |
//-----------------------------------------------------------------------------

SODmSServerAE::SODmSServerAE(IN BYTE serverInstance)
	: SOAeSServer(serverInstance)
{
	m_accessLevel = 0;
}

BOOL SODmSServerAE::isPrivateSecurityAvailable(void)
{
	return TRUE;
}

BOOL SODmSServerAE::logon(LPCTSTR userName, LPCTSTR password)
{
	if ((_tcscmp(userName, _T("Softing")) == 0) && (_tcscmp(password, _T("Softing")) == 0))
	{
		m_accessLevel = 1;
	}
	else if (_tcscmp(userName, _T("NoRights")) == 0)
	{
		m_accessLevel = -1;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SODmSServerAE::logoff(void)
{
	m_accessLevel = 0;
	return TRUE;
}

HRESULT SODmSServerAE::checkAuthorizationDCOM(IN LPCTSTR ifFunction)
{
	HRESULT res = S_OK;

	if (m_accessLevel < 0)
	{
		if (_tcsncmp(ifFunction, _T("IOPCSecurityPrivate::"), 21) == 0)
		{
			res = S_OK;
		}
		else
		{
			res = E_ACCESSDENIED;
		}
	}

	return res;
}

#endif

#ifdef SODMS_ALL

//-----------------------------------------------------------------------------
// SODmSConditionTimeSlot                                                     |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// checkCondition
//
// - check the condition state
//
void SODmSConditionTimeSlot::checkCondition(void)
{
	SOCmnList<SOAeSAttributeValue> attrValList(getAttributeValueList());
	SOCmnPointer<SOAsSAttributeValue> c = (SOAsSAttributeValue*)attrValList.findKey(SODMS_AE_ATTRIBUTE_CURRENT_SECOND);
	SOCmnPointer<SOAsSAttributeValue> s = (SOAsSAttributeValue*)attrValList.findKey(SODMS_AE_ATTRIBUTE_START_SECOND);
	SOCmnPointer<SOAsSAttributeValue> e = (SOAsSAttributeValue*)attrValList.findKey(SODMS_AE_ATTRIBUTE_END_SECOND);
	attrValList.autoUnlock();

	if ((!!c) && (!!s) && (!!e))
	{
		SOCmnVariant cVal;
		SOCmnVariant sVal;
		SOCmnVariant eVal;
		// update all attribute values from the DA caches
		c->updateValueFromCache((LPVARIANT)cVal);
		s->updateValueFromCache((LPVARIANT)sVal);
		e->updateValueFromCache((LPVARIANT)eVal);

		if ((cVal.bVal >= sVal.bVal) && (cVal.bVal <= eVal.bVal) && (sVal.bVal != eVal.bVal))
		{
			// active
			activate(TRUE);
		}
		else
		{
			// inactive
			activate(FALSE);
		}
	}
} // checkCondition


//-----------------------------------------------------------------------------
// getObjectAeSConditionIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODmSConditionTimeSlot::getObjectAeSConditionIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSConditionIAttr


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODmSConditionTimeSlot::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList

#endif

