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
//                             OPC Toolkit C++                                |
//                                                                            |
//  Filename    : Service.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Service appication template main functions                  |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "OpcServer.h"

//	TODO change the name
const tstring g_serviceName = _T("Softing.OPCToolbox.C++.AddressSpace");

SERVICE_STATUS g_serviceStatus;
SERVICE_STATUS_HANDLE g_serviceStatusHandle =   0;
HANDLE g_endEvent = 0;
HANDLE g_changedEvent = 0;

int g_count = 0;
// instance handle of application
HINSTANCE g_instance;

BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;

BOOL g_holdSignalChange = FALSE;
Mutex g_syncDisplay;

//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	HANDLE eventHandles[1];
	eventHandles[0] = g_changedEvent;
	g_simulationEndMutex.lock();

	while (!g_simulationEnd)
	{
		if (g_count > 30)
		{
			pServer->updateAddressSpaceElements();
			pServer->rebuildRandomNamespace();
			SetEvent(g_changedEvent);
			g_count = 0;
		}
		else
		{
			g_count++;
		} // end if .. else

		::Sleep(1000);
	}   //  end while

	g_simulationEndMutex.unlock();
	return 0;
}   //  end simulationThread


//-----------------------------------------------------------------------------
//	startSimulationThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startSimulationThread(void)
{
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &simulationThread, NULL, 0, &tID);
}   //  end startThread

//-----------------------------------------------------------------------------
//	reportStatusToSCMgr  - service manager
//
//-----------------------------------------------------------------------------
long reportStatusToSCMgr(
	IN DWORD currentState,
	IN DWORD waitHint)
{
	static DWORD checkPoint = 1;
	g_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	g_serviceStatus.dwCurrentState = currentState;
	g_serviceStatus.dwWaitHint = waitHint;

	if ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED))
	{
		g_serviceStatus.dwCheckPoint = 0;
	}
	else
	{
		g_serviceStatus.dwCheckPoint = checkPoint++;
	}   //  end if...else

	// report the status of the service to the service control manager.
	return SetServiceStatus(g_serviceStatusHandle, &g_serviceStatus);
}   //  end reportStatusToSCMgr


//-----------------------------------------------------------------------------
//	serviceControlHandler    - service manager
//
//-----------------------------------------------------------------------------
void WINAPI serviceControlHandler(IN DWORD ctrlCode)
{
	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		reportStatusToSCMgr(SERVICE_STOP_PENDING, 0);

		if (g_endEvent)
		{
			::SetEvent(g_endEvent);
		}   //  end if
	}   //  end case
	break;
	}   //  end switch
}   //  end serviceControlHandler

void WINAPI ServiceMain(
	IN DWORD argc,
	IN LPTSTR* argv)
{
	g_serviceStatusHandle = ::RegisterServiceCtrlHandler(g_serviceName.c_str(), serviceControlHandler);

	if (g_serviceStatusHandle)
	{
		g_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		g_serviceStatus.dwServiceSpecificExitCode = 0;
		g_serviceStatus.dwWin32ExitCode = 0;
		reportStatusToSCMgr(SERVICE_START_PENDING, 1000);
		g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		BOOL started = FALSE;
		long result = S_OK;
		OpcServer* pServer = getOpcServer();

		//  Start the OPC server's I/O internal mechanism
		if (SUCCEEDED(pServer->start()))
		{
			started = TRUE;
			pServer->buildAddressSpace();
			pServer->ready();
		}

		//  start the simulation thread
		if (SUCCEEDED(result))
		{
			startSimulationThread();
		}

		reportStatusToSCMgr(SERVICE_RUNNING, 0);
		::WaitForSingleObject(g_endEvent, INFINITE);
		//  terminate the simulation
		g_simulationEnd = TRUE;
		g_simulationEndMutex.lock();
		g_simulationEndMutex.unlock();
		pServer->stop();

		if (!started)
		{
			g_serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
			g_serviceStatus.dwServiceSpecificExitCode = -4;
		}   //  end if

		::CloseHandle(g_endEvent);
		reportStatusToSCMgr(SERVICE_STOPPED, 0);
	}   //  end if
}   //  end ServiceMain


int APIENTRY _tWinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPTSTR cmdLine,
	int cmdShow)
{
	g_instance = instance;
	g_changedEvent = ::CreateEvent(NULL, FALSE, FALSE, _T("ChangedEvent"));
	//  create the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	//  Initialize the OpcServer instance
	pServer->initialize();
	//  set the service name to the OpcServer instance
	pServer->setServiceName(g_serviceName);
	BOOL commandLineProcessed = FALSE;
	BOOL end = FALSE;
	int ret = 0;
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPTSTR)g_serviceName.c_str(), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	//  provide the server with the proper custom creator
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
		::CloseHandle(g_changedEvent);
		return 1;
	}

	tstring commandLine(GetCommandLine());
	//  handle the command line arguments (register/unregister, etc)
	long result = pServer->processCommandLine(commandLine);
	commandLineProcessed = TRUE;

	if (result != S_OK)
	{
		long returnResult = 0;

		if (result == S_FALSE)
		{
			//  registration operation successful
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration succeeded"));
			returnResult = 0;
		}
		else
		{
			//  registration operation has failed
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration failed"));
			returnResult = 1;
		}

		//  no matter what colse the application if processCommandLine returned
		//  something different of S_OK
		pServer->terminate();
		destroyOpcServer();
		::CloseHandle(g_changedEvent);
		return returnResult;
	}   //  end if

	// start the service
	::StartServiceCtrlDispatcher(dispatchTable);
	pServer->terminate();
	destroyOpcServer();
	::CloseHandle(g_changedEvent);
	return 0;
}   //  end _tWinMain

