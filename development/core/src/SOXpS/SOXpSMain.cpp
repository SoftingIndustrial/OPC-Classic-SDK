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
//                           OPC Toolkit - SOXpS                              |
//                                                                            |
//  Filename    : SOXpSMain.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Main routines of the Server                                 |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <shellapi.h>
#include "SOXpS.h"
#include "SOXpSEntry.h"
#include "SOXpSCallbacks.h"
#include "SOXpSDAObjects.h"
#include "SOXpSmc.h"

static HINSTANCE g_instance = NULL;
BOOL g_isService = FALSE;
SOXpSInitData *g_initData = NULL;

// can close application during shutdown event
HANDLE g_canCloseEvent = NULL;

#ifdef SOXPS_SERVICE
static SERVICE_STATUS g_status;       // current status of the service
static SERVICE_STATUS_HANDLE g_statusHandle;
static HANDLE g_serviceStopEvent = INVALID_HANDLE_VALUE;
#endif
#ifdef SOXPS_SERVER
HWND g_hWnd = NULL;
#endif

SOCmnString *g_pShutdownReason = NULL;
HANDLE g_startupEvent = NULL;

static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

// additional values for request handling and condition related events
BOOL g_tsChangeSendsValue = FALSE;
BOOL g_condRelEventsSendsActorID = FALSE;



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
	return GetModuleHandle(NULL);
} // getInstanceHandle


//-----------------------------------------------------------------------------
// addToMessageLog
//
// - write message to error log
//
void addToMessageLog(
	IN DWORD id,
	IN LPCTSTR par)
{
	HANDLE eventSource;
	LPCTSTR strings[2];

	eventSource = RegisterEventSource(NULL, _T("SOXpS"));
	if (eventSource != NULL)
	{
	    TCHAR path[512];
		path[0] = 0;
	    GetModuleFileName(NULL, path, 512);

		strings[0] = (LPCTSTR)path;
		strings[1] = par;
		if (par)
			ReportEvent(eventSource, EVENTLOG_ERROR_TYPE, 0, id, NULL, 2, 0, (LPCTSTR *)strings, NULL);
		else
			ReportEvent(eventSource, EVENTLOG_ERROR_TYPE, 0, id, NULL, 1, 0, (LPCTSTR *)strings, NULL);

		DeregisterEventSource(eventSource);
	}
} // addToMessageLog


BOOL initializeApp(void)
{
	// initialize global objects
	initSOCmn();

	if (!loadCfgDLL())
	{
		// destroy global objects
		destroyTraceObject();
		destroyEventPool();
		return FALSE;
	}

	// initialize configuration dll
#ifdef SOXPS_ALL
	g_initData = g_fpSOXpSInitialize(&_SOXpSCompletedDARequests, &_SOXpSFireAEEvents, &_SOXpSChangeAEConditions, &_SOXpSShutdown, &_SOXpSSetDeviceState);
#else
#ifdef SOXPS_DA
	g_initData = g_fpSOXpSInitialize(&_SOXpSCompletedDARequests, NULL, NULL, &_SOXpSShutdown, &_SOXpSSetDeviceState);
#endif
#endif

#ifndef SOXPS_DA
	g_initData->m_clsidDA = CLSID_NO;        // class id of DA server
	g_initData->m_progIdDA = NULL;           // prog id of DA server
	g_initData->m_verIndProgIdDA = NULL;     // version independent prog id of DA server
	g_initData->m_descriptionDA = NULL;      // description of DA server
#endif
#ifndef SOXPS_AE
	g_initData->m_clsidAE = CLSID_NO;        // class id of AE server
	g_initData->m_progIdAE = NULL;           // prog id of AE server
	g_initData->m_verIndProgIdAE = NULL;     // version independent prog id of AE server
	g_initData->m_descriptionAE = NULL;      // description of AE server
#endif

	// initialize trace
	SOCmnTrace *trace = ::getSOCmnTrace();
	if (g_initData->m_traceFile1)
	{
		if (g_initData->m_traceFileSize > 0)
			trace->setFile(g_initData->m_traceFile1, 5, g_initData->m_traceFile2, g_initData->m_traceFileSize);
		else
			trace->setFile(g_initData->m_traceFile1);
	}
	trace->setLevels(SOCMNTRACE_G_ALL, SOCMNTRACE_G_ALL, g_initData->m_traceInfoLevel, g_initData->m_traceDebugLevel);

	{
	SOCmnString clsid;
#ifdef SOXPS_ALL
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Softing OPC Toolkit - XPress OPC Tool")));
#else
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Softing OPC Toolkit - XPress OPC Tool Data Access")));
#endif
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Build for XPress Version:  %u.%u"), LOBYTE(g_initData->m_xpsInitDataVersion), HIBYTE(g_initData->m_xpsInitDataVersion)));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Version:         %u.%u.%u"), g_initData->m_majVersion, g_initData->m_minVersion, g_initData->m_buildNumber));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Vendor Info:     %s"), g_initData->m_vendorInfo));
#ifdef SOXPS_DA
	IID2string(&clsid, g_initData->m_clsidDA);
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("DA CLSID:        %s"), (LPCTSTR)clsid));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("DA ProdId:       %s"), g_initData->m_progIdDA));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("DA VerIndProgId: %s"), g_initData->m_verIndProgIdDA));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("DA Description:  %s"), g_initData->m_descriptionDA));
#endif
#ifdef SOXPS_AE
	IID2string(&clsid, g_initData->m_clsidAE);
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("AE CLSID:        %s"), (LPCTSTR)clsid));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("AE ProdId:       %s"), g_initData->m_progIdAE));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("AE VerIndProgId: %s"), g_initData->m_verIndProgIdAE));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("AE Description:  %s"), g_initData->m_descriptionAE));
#endif
	}

	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Exported functions of SOXpSCfg.DLL:")));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSInitialize:            0x%8.8X"), g_fpSOXpSInitialize));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSStart:                 0x%8.8X"), g_fpSOXpSStart));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSStop:                  0x%8.8X"), g_fpSOXpSStop));
#ifdef SOXPS_DA
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSBuildDANameSpace:      0x%8.8X"), g_fpSOXpSBuildDANameSpace));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSHandleDARequests:      0x%8.8X"), g_fpSOXpSHandleDARequests));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSActivateDAItems:       0x%8.8X"), g_fpSOXpSActivateDAItems));
#endif
#ifdef SOXPS_AE
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSBuildAEAreaSpace:      0x%8.8X"), g_fpSOXpSBuildAEAreaSpace));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSBuildAEEventSpace:     0x%8.8X"), g_fpSOXpSBuildAEEventSpace));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSActivateAESubscription:0x%8.8X"), g_fpSOXpSActivateAESubscription));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSReceivedAEAcknowledge: 0x%8.8X"), g_fpSOXpSReceivedAEAcknowledge));
	_TRACEC(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("  SOXpSTranslateToItemIDs:    0x%8.8X"), g_fpSOXpSTranslateToItemIDs));
#endif

	// check required exported functions
	if (memcmp(&CLSID_NO, &g_initData->m_clsidDA, sizeof(CLSID)) != 0)
	{
		// implements DA server -> check for the required
		if ((g_fpSOXpSBuildDANameSpace == NULL) || (g_fpSOXpSHandleDARequests == NULL))
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Not implemented all required DA functions")));
			if (!g_isService)
				MessageBox(NULL, _T("The DLL exports not all required OPC DA functions"), _T("XPress OPC Server Tool"), MB_ICONSTOP | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
			addToMessageLog(SOXPS_ERROR_REQUIRED_DA_FUNCTIONS, NULL);
			return FALSE;
		}
	}

	if (memcmp(&CLSID_NO, &g_initData->m_clsidAE, sizeof(CLSID)) != 0)
	{
		// implements AE server -> check for the required
		if ((g_fpSOXpSBuildAEAreaSpace == NULL) || (g_fpSOXpSBuildAEEventSpace == NULL))
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Not implemented all required AE functions")));
			if (!g_isService)
				MessageBox(NULL, _T("The DLL exports not all required OPC AE functions"), _T("XPress OPC Server Tool"), MB_ICONSTOP | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
			addToMessageLog(SOXPS_ERROR_REQUIRED_AE_FUNCTIONS, NULL);
			return FALSE;
		}
	}

	if (MAKEWORD(HIBYTE(g_initData->m_xpsInitDataVersion), LOBYTE(g_initData->m_xpsInitDataVersion)) < MAKEWORD(1,4)) // V4.01
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("The DLL is build for an older XPress version")));
		if (!g_isService)
			MessageBox(NULL, _T("The DLL is build for an older XPress version"), _T("XPress OPC Server Tool"), MB_ICONSTOP | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
		addToMessageLog(SOXPS_ERROR_WRONG_XPS_VERSION, NULL);
		return FALSE;
	}

	if (g_initData->m_hKey && g_initData->m_regKeyParams)	{
		HKEY key = NULL;

		if (ERROR_SUCCESS == RegOpenKeyEx(g_initData->m_hKey, g_initData->m_regKeyParams, NULL, KEY_READ, &key))
		{
			DWORD dwLen = sizeof(DWORD), dwType = REG_DWORD, dw;

			if (ERROR_SUCCESS == RegQueryValueEx(key, _T("TimestampChangeSendsValue"), NULL, NULL, (LPBYTE)&dw, &dwLen))
				g_tsChangeSendsValue = ((dw!=0)? TRUE : FALSE);

			if (ERROR_SUCCESS == RegQueryValueEx(key, _T("CondRelEventsSendsActorID"), NULL, NULL, (LPBYTE)&dw, &dwLen))
				g_condRelEventsSendsActorID = ((dw!=0)? TRUE : FALSE);

			RegCloseKey(key);
		}
	}


	return TRUE;
}


void initializeEntry(void)
{
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
	SOCmnPointer<SOAeSEntry> entryAE;
	SOCmnPointer<SODaSEntry> entryDA;
	SOCmnPointer<SOXpSAECreator> creatorAE;
	SOCmnPointer<SOXpSDACreator> creatorDA;
	LPCTSTR serviceName = NULL;

	if (g_isService)
		serviceName = g_initData->m_serviceName;

	entry->initialize(SOSrvEntry::OutProc, g_initData->m_clsidDA, g_initData->m_clsidAE, g_instance, COINIT_MULTITHREADED);
	entryDA = entry->getDAEntry();
	entryAE = entry->getAEEntry();

	entryDA->setRegisterInfo(g_initData->m_progIdDA, g_initData->m_verIndProgIdDA, g_initData->m_descriptionDA, serviceName);
	entryDA->setVendorInfo(g_initData->m_vendorInfo);
	entryDA->setVersionInfo(g_initData->m_majVersion, g_initData->m_minVersion, 0, g_initData->m_buildNumber);
	entryAE->setRegisterInfo(g_initData->m_progIdAE, g_initData->m_verIndProgIdAE, g_initData->m_descriptionAE, serviceName);
	entryAE->setVendorInfo(g_initData->m_vendorInfo);
	entryAE->setVersionInfo(g_initData->m_majVersion, g_initData->m_minVersion, 0, g_initData->m_buildNumber);

	// set creators
	SOCMN_ALLOCATE_OBJECT_C(creatorAE, SOXpSAECreator(), (SOXpSAECreator *));
	entryAE->setCreator(creatorAE);
	creatorAE.release();
	SOCMN_ALLOCATE_OBJECT_C(creatorDA, SOXpSDACreator(), (SOXpSDACreator *));
	entryDA->setCreator(creatorDA);

	entry->setCheckParameter(30000);

	entryDA->setMinUpdateRate(g_initData->m_minUpdateRateDA);

	g_startupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

BOOL initializeServer(HANDLE *deviceThread)
{
	BOOL end = FALSE;

	// start entry
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
	entry->start();

	// create DA request thread
	if (g_fpSOXpSHandleDARequests)
	{
		DWORD tid;
		*deviceThread = ::CreateThread(NULL, 0, requestThreadMain, NULL, 0, &tid);
	}

	// build DA name space
	if (g_fpSOXpSBuildDANameSpace)
	{
		g_fpSOXpSBuildDANameSpace(&_SOXpSCreateNode, &_SOXpSCreateTag, &_SOXpSCreateProperty);
	}

	// build AE event space
	if (g_fpSOXpSBuildAEEventSpace)
	{
		g_fpSOXpSBuildAEEventSpace(&_SOXpSCreateCategory, &_SOXpSCreateAttribute, &_SOXpSCreateConditionClass, &_SOXpSCreateSubConditionClass);
	}

	// build AE area space
	if (g_fpSOXpSBuildAEAreaSpace)
	{
		g_fpSOXpSBuildAEAreaSpace(&_SOXpSCreateArea, &_SOXpSCreateSource, &_SOXpSCreateCondition);
	}

	SetEvent(g_startupEvent);

	if (g_fpSOXpSStart)
		end = !(g_fpSOXpSStart());

	return end;
}

void terminateServer(HANDLE deviceThread)
{
	SOCmnString shutdownReason;
	DWORD waitTime = 0;
	g_canCloseEvent = getEventPool()->popEvent();

	if (g_pShutdownReason)
	{
		shutdownReason = *g_pShutdownReason;
		delete g_pShutdownReason;
		g_pShutdownReason = NULL;
	}
	else
		shutdownReason = _T("Server shutdown");

	{
	SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> rootDA = entryDA->getObjectRoot();
 	SOCmnList<SOCmnElement> listDA(rootDA->getBranchList());
	SOCmnListPosition posDA;
	SODaSServer *pServer;
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
	SOCmnPointer<SOSrvObjectRoot> rootAE = entryAE->getObjectRoot();
 	SOCmnList<SOCmnElement> listAE(rootAE->getBranchList());

	if (listAE.getCount() > 0)
	{
		entryAE->shutdownAllClients(shutdownReason);
		waitTime = 10000;
	}
	// check for DCOM server releated objects
	posDA = listDA.getStartPosition();
	while (posDA)
	{
		pServer = (SODaSServer *)listDA.getNext(posDA);
		if (pServer->isDCOM())
		{
			entryDA->shutdownAllClients(shutdownReason);
			waitTime = 10000;
			break;
		}
	}
	}
	if (waitTime)
		WaitForSingleObject(g_canCloseEvent, waitTime);	// give the clients time to disconnect

	getEventPool()->pushEvent(g_canCloseEvent);
	g_canCloseEvent = NULL;


	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
	entry->stop();

	// stop DA request thread
	if ((deviceThread != INVALID_HANDLE_VALUE) && (g_requestThreadEndEvent != INVALID_HANDLE_VALUE))
	{
		::SetEvent(g_requestThreadEndEvent);
		::WaitForSingleObject(deviceThread, INFINITE);
		::CloseHandle(deviceThread);
	}

	if (g_fpSOXpSStop)
		g_fpSOXpSStop();
}

void terminateEntry(void)
{
	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
	entry->terminate();
	entry.release();

	CloseHandle(g_startupEvent);
}

void terminateApp(void)
{
	// destroy global objects
	exitSOCmn();
}

#ifdef SOXPS_SERVICE

void stopService(SC_HANDLE service)
{
	// try to stop the service
	if (ControlService(service, SERVICE_CONTROL_STOP, &g_status))
	{
		int i = 1;
		_tprintf(_T("Stopping %s."), g_initData->m_serviceDescription);
		Sleep(100);

		while (QueryServiceStatus(service, &g_status))
		{
			if (g_status.dwCurrentState == SERVICE_STOP_PENDING)
			{
				i++;
				if (i >= 10)
				{
					_tprintf(_T("."));
					i = 0;
				}
				Sleep(100);
			}
			else
				break;
		}

		if (g_status.dwCurrentState == SERVICE_STOPPED)
			_tprintf(TEXT("\n%s stopped.\n"), g_initData->m_serviceDescription);
	}
}

//-----------------------------------------------------------------------------
// cmdRegisterService
//
// - create service entries in registry
// - create OPC entries in registry
//
BOOL registerService()
{
	SC_HANDLE service;
	SC_HANDLE SCManager;
	TCHAR path[512];
	BOOL ret = FALSE;

	if (GetModuleFileName(NULL, path, 512) == 0)
	{
		return FALSE;
	}

	SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (SCManager)
	{
		service = CreateService(SCManager, g_initData->m_serviceName, g_initData->m_serviceDescription,
						SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
						SERVICE_ERROR_NORMAL, path, NULL, NULL, g_initData->m_serviceDependencies, NULL, NULL);
		if (service)
		{
			_tprintf(_T("%s installed.\n"), g_initData->m_serviceDescription);
			CloseServiceHandle(service);
			ret = TRUE;
		}
		else
		{
			DWORD lastE = GetLastError();
			if (lastE == ERROR_SERVICE_EXISTS)
			{
				service = OpenService(SCManager, g_initData->m_serviceName, SERVICE_ALL_ACCESS);
				if (service)
				{
					BYTE buffer[1000];
					QUERY_SERVICE_CONFIG *pServiceConfig = (QUERY_SERVICE_CONFIG *)buffer;
					DWORD bn;
					if (QueryServiceConfig(service, pServiceConfig, 1000, &bn))
					{
						if (_tcscmp(pServiceConfig->lpDisplayName, g_initData->m_serviceDescription) == 0)
						{ // same service
							stopService(service);
							if (ChangeServiceConfig(service, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
							        SERVICE_ERROR_NORMAL, path, NULL, NULL, g_initData->m_serviceDependencies,
							        NULL, NULL, g_initData->m_serviceDescription))
							{
								_tprintf(_T("%s reinstalled.\n"), g_initData->m_serviceDescription);
								ret = TRUE;
							}
						}
					}
					CloseServiceHandle(service);
				}
			}
		}
		CloseServiceHandle(SCManager);
	}

	return ret;
} // cmdRegisterService


//-----------------------------------------------------------------------------
// cmdUnregisterService
//
// - stops service
// - remove service entries in registry
// - remove OPC entries in registry
//
BOOL unregisterService()
{
	SC_HANDLE service;
	SC_HANDLE SCManager;
	BOOL ret = FALSE;

	SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (SCManager)
	{
		service = OpenService(SCManager, g_initData->m_serviceName, SERVICE_ALL_ACCESS);

		if (service)
		{
			stopService(service);

			// now remove the service
			if(DeleteService(service))
			{
				_tprintf(_T("%s removed.\n"), g_initData->m_serviceDescription);
				ret = TRUE;
			}

			CloseServiceHandle(service);
		}

		CloseServiceHandle(SCManager);
	}

	return ret;
} // cmdUnregisterService
#endif


BOOL processCommandLine(IN LPCTSTR cmdLine, OUT BOOL *startedByCOM)
{
	BOOL end;

	SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
	SOCmnPointer<SOAeSEntry> entryAE = entry->getAEEntry();
	SOCmnPointer<SODaSEntry> entryDA = entry->getDAEntry();
	end = FALSE;
	*startedByCOM = FALSE;

	if ( ( _tcsicmp( _T("-RegServer"), cmdLine ) == 0 ) ||
		 ( _tcsicmp( _T("/RegServer"), cmdLine ) == 0 ) )
	{ // register OPC Server
#ifdef SOXPS_SERVICE
		registerService();
#endif
		entry->registerServer();
		entryDA->registerCategories();
		entryAE->registerCategories();

		// register event viewer source
		CRegKey keyEvS;
		DWORD ret;
		ret = keyEvS.Create(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Services\\EventLog\\Application\\SOXpS"));
		if (ret == ERROR_SUCCESS)
		{
		    TCHAR path[512];
			path[0] = _T('\0');
		    GetModuleFileName(NULL, path, 512);

			keyEvS.SetValue(path, _T("EventMessageFile"));
			keyEvS.SetValue(7, _T("TypesSupported"));
		}

		end = TRUE;
	}
	else if ( ( _tcsicmp( _T("-UnregServer"), cmdLine ) == 0 ) ||
			  ( _tcsicmp( _T("/UnregServer"), cmdLine ) == 0 ) )
	{ // unregister OPC Server
		entry->unregisterServer();
		entryDA->unregisterCategories();
		entryAE->unregisterCategories();
#ifdef SOXPS_SERVICE
		unregisterService();
#endif

		// unregister event viewer source
		CRegKey keyEvS;
		DWORD ret;
		ret = keyEvS.Open(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Services\\EventLog\\Application"));
		if (ret == ERROR_SUCCESS)
		{
			keyEvS.RecurseDeleteKey(_T("SOXpS"));
		}

		end = TRUE;
	}
	else if ( ( _tcsicmp( _T("-Embedding"), cmdLine ) == 0 ) ||
			  ( _tcsicmp( _T("/Embedding"), cmdLine ) == 0 ) ||
			  ( _tcsicmp( _T("-Embedded"), cmdLine ) == 0 )  ||
			  ( _tcsicmp( _T("/Embedded"), cmdLine ) == 0 )	)
	{
		*startedByCOM = TRUE;
	}

	return end;
}

#ifdef SOXPS_SERVER

#define WINDOW_CLASS _T("Softing XPress OPC Server")

//-----------------------------------------------------------------------------
// wndProc
//
// - processes messages for the main window.
//
// returns:
//		0
//
LRESULT CALLBACK wndProc(
	IN HWND wnd,		// window handle
	IN UINT message,	// message id
	IN WPARAM wParam,	// 1st parameter
	IN LPARAM lParam)	// 2nd parameter
{
	switch (message)
	{
		case WM_PAINT:
		{ // paint the main window
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(wnd, &ps);
			EndPaint(wnd, &ps);
			break;
		}
		case WM_DESTROY:
			// post a quit message and return
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(wnd, message, wParam, lParam);
   }
   return 0;
} // wndProc


//-----------------------------------------------------------------------------
// WinMain
//
// - main routine of server application
//
// returns:
//		exit value
//
int APIENTRY WinMain(
	HINSTANCE instance,		// instance
	HINSTANCE prevInstance,	// previous intance
	LPSTR cmdLine,			// command line
	int cmdShow)			// show command
{
USES_CONVERSION;

	g_isService = FALSE;
	g_instance = instance;

	if (!initializeApp())
		return FALSE;

	BOOL end = FALSE;
	BOOL startedByCOM = FALSE;
	HANDLE deviceThread = INVALID_HANDLE_VALUE;
	int ret = 0;

	// register window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(g_dllInstance, MAKEINTRESOURCE(g_initData->m_resIdIcon));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);

	// create window
	g_hWnd = CreateWindow(WINDOW_CLASS, g_initData->m_appTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_ICONIC,
		0, 0, 400, 0, NULL, NULL, instance, NULL);

	// initilialize entry
	initializeEntry();

	// register and unregister
	if (cmdLine)
		end = processCommandLine(A2T(cmdLine), &startedByCOM);

	if (!end)
	{
		{
		// set event handler
		SOCmnPointer<SOXpSEventHandler> eventHandler;
		SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
		SOCMN_ALLOCATE_OBJECT_C(eventHandler, SOXpSEventHandler(startedByCOM, g_hWnd), (SOXpSEventHandler *))
		entry->setEventHandler((SOXpSEventHandler *)eventHandler);
		}

		end = initializeServer(&deviceThread);

		if (!end)
		{
			if ((g_hWnd) && (g_initData->m_visible))
			{
				ShowWindow(g_hWnd, SW_MINIMIZE);
				UpdateWindow(g_hWnd);
			}

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
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Can't start the OPC server")));
		}

		terminateServer(deviceThread);
	}

	// terminate entry
	terminateEntry();

	terminateApp();

	return ret;
} // WinMain


#endif // SOXPS_SERVER

#ifdef SOXPS_SERVICE



//-----------------------------------------------------------------------------
// reportStatusToSCMgr
//
// - set status
// - report status to the SCM
//
// returns:
//		TRUE  - reported
//		FALSE - error
//
BOOL reportStatusToSCMgr(
	IN DWORD currentState,	// state
	IN DWORD exitCode,		// exit code
	IN DWORD waitHint)		// max wait time
{
	static DWORD checkPoint = 1;
	BOOL ret = TRUE;

	if (currentState == SERVICE_START_PENDING)
		g_status.dwControlsAccepted = 0;
	else
		g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	g_status.dwCurrentState = currentState;
	g_status.dwWin32ExitCode = exitCode;
	g_status.dwWaitHint = waitHint;

	if ( (currentState == SERVICE_RUNNING ) ||
		 (currentState == SERVICE_STOPPED ) )
		g_status.dwCheckPoint = 0;
	else
		g_status.dwCheckPoint = checkPoint++;

    // report the status of the service to the service control manager.
	ret = SetServiceStatus(g_statusHandle, &g_status);

	return ret;
} // reportStatusToSCMgr


//-----------------------------------------------------------------------------
// serviceControlHandler
//
// - control handler for the SCM
//
void WINAPI serviceControlHandler(
	IN DWORD ctrlCode)	// requested control type
{
	switch(ctrlCode)
	{
		// stop the service.
		case SERVICE_CONTROL_STOP:
		{
			reportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
			if (g_serviceStopEvent)
				SetEvent(g_serviceStopEvent);
		}
		break;
		default:
		break;
	}

	reportStatusToSCMgr(g_status.dwCurrentState, NO_ERROR, 0);
}


//-----------------------------------------------------------------------------
// service_main
//
// - perform initialization of the service
// - start service
//
void WINAPI service_main(
	IN DWORD argc,		// number of command line arguments
	IN LPTSTR *argv)	// array of command line arguments
{
	BOOL end = FALSE;
	HANDLE deviceThread = INVALID_HANDLE_VALUE;

	// register service control handler:
	g_statusHandle = RegisterServiceCtrlHandler(g_initData->m_serviceName, serviceControlHandler);

	if (g_statusHandle)
	{
		g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		g_status.dwServiceSpecificExitCode = 0;

		// report the status to the service control manager.
		if (reportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))
		{
			g_serviceStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			end = initializeServer(&deviceThread);

			if (!end)	{
				if (reportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0))
					end = FALSE;

				WaitForSingleObject(g_serviceStopEvent, INFINITE);
			}

			terminateServer(deviceThread);
		}
	}

	terminateEntry();

	terminateApp();

	// report the stopped status to the service control manager.
	if (g_statusHandle)
	{
		reportStatusToSCMgr(SERVICE_STOPPED, 0, 0);
	}

	return;
} // service_main


//-----------------------------------------------------------------------------
// main
//
// - parse command line for options:
//		-RegServer   install the service
//		-UnregServer remove the service
// - start service if no options
//
int main(
	IN DWORD argc,		// number of command line arguments
	IN LPTSTR *argv)	// array of command line arguments
{
	BOOL end = FALSE;
	BOOL startedByCOM = FALSE;

	g_isService = TRUE;
	g_instance = GetModuleHandle(NULL);
	if (!initializeApp())
	{
		return 1;
	}

	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPTSTR)g_initData->m_serviceName, (LPSERVICE_MAIN_FUNCTION)service_main },
		{ NULL, NULL }
	};

	// initilialize entry
	initializeEntry();

#ifdef UNICODE
	argv = CommandLineToArgvW(GetCommandLineW(), (int *)&(argc));
#endif

	// register and unregister
	if (argc > 1)
		end = processCommandLine(argv[1], &startedByCOM);

	if (!end)
	{
		{
		// set event handler
		SOCmnPointer<SOXpSEventHandler> eventHandler;
		SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
		SOCMN_ALLOCATE_OBJECT_C(eventHandler, SOXpSEventHandler(startedByCOM, NULL), (SOXpSEventHandler *))
		entry->setEventHandler((SOXpSEventHandler *)eventHandler);
		}

		StartServiceCtrlDispatcher(dispatchTable);
	}

	return 0;
} // main

#endif // SOXPS_SERVICE
