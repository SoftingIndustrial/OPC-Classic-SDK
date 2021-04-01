#include "stdafx.h"

#include "OTcommon.h"

#include "OpcServer.h"


//	TODO change the name
const tstring s_serviceName = _T("XXOPCsvc");

SERVICE_STATUS s_serviceStatus;
SERVICE_STATUS_HANDLE s_serviceStatusHandle =   0;
HANDLE s_serviceStopEvent   =   0;

// instance handle of application
HINSTANCE s_instance;

// TODO: remove simulation which starts here >>>

BOOL s_simulationEnd = FALSE;
Mutex s_simulationEndMutex;
//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	s_simulationEndMutex.lock();

	while (!s_simulationEnd)
	{
		//  simulate the AE Server events generation
		pServer->fireSimulationEvents();
		//  simulate the DA Server data changes generation
		pServer->changeSimulationValues();
		::Sleep(1000);
	}   //  end while

	s_simulationEndMutex.unlock();
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

// TODO: remove simulation which ends here <<<

//-----------------------------------------------------------------------------
//	reportStatusToSCMgr  - service manager
//
//-----------------------------------------------------------------------------
long reportStatusToSCMgr(
	IN DWORD currentState,
	IN DWORD waitHint)
{
	static DWORD checkPoint = 1;
	s_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	s_serviceStatus.dwCurrentState = currentState;
	s_serviceStatus.dwWaitHint = waitHint;

	if ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED))
	{
		s_serviceStatus.dwCheckPoint = 0;
	}
	else
	{
		s_serviceStatus.dwCheckPoint = checkPoint++;
	}   //  end if...else

	// report the status of the service to the service control manager.
	return SetServiceStatus(s_serviceStatusHandle, &s_serviceStatus);
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

		if (s_serviceStopEvent)
		{
			::SetEvent(s_serviceStopEvent);
		}   //  end if
	}   //  end case
	break;
	}   //  end switch
}   //  end serviceControlHandler

void WINAPI ServiceMain(
	IN DWORD argc,
	IN LPTSTR* argv)
{
	s_serviceStatusHandle = ::RegisterServiceCtrlHandler(s_serviceName.c_str(), serviceControlHandler);

	if (s_serviceStatusHandle)
	{
		s_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		s_serviceStatus.dwServiceSpecificExitCode = 0;
		s_serviceStatus.dwWin32ExitCode = 0;
		reportStatusToSCMgr(SERVICE_START_PENDING, 1000);
		s_serviceStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		BOOL started = FALSE;
		long result = S_OK;
		OpcServer* pServer = getOpcServer();

		//  Start the OPC server's I/O internal mechanism
		if (SUCCEEDED(pServer->start()))
		{
			started = TRUE;
			pServer->buildAddressSpace();
			pServer->buildEventCategories();
			pServer->ready();
		}

		// TODO: remove simulation which starts here >>>
		//  start the simulation thread
		if (SUCCEEDED(result))
		{
			startSimulationThread();
		}

		// TODO: remove simulation which ends here <<<
		reportStatusToSCMgr(SERVICE_RUNNING, 0);
		WaitForSingleObject(s_serviceStopEvent, INFINITE);
		//  terminate the simulation
		s_simulationEnd = TRUE;
		s_simulationEndMutex.lock();
		s_simulationEndMutex.unlock();
		pServer->stop();

		if (!started)
		{
			s_serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
			s_serviceStatus.dwServiceSpecificExitCode = -4;
		}   //  end if

		CloseHandle(s_serviceStopEvent);
		reportStatusToSCMgr(SERVICE_STOPPED, 0);
	}   //  end if
}   //  end ServiceMain


int APIENTRY _tWinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPTSTR cmdLine,
	int cmdShow)
{
	OTInitLib();
	s_instance = instance;
	//  create the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	//  set the service name to the OpcServer instance
	pServer->setServiceName(s_serviceName);
	//  Initialize the OpcServer instance
	pServer->initialize();
	BOOL commandLineProcessed = FALSE;
	tstring commandLine(GetCommandLine());
	BOOL end = FALSE;
	int ret = 0;
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPTSTR)s_serviceName.c_str(), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	//  provide the server with the proper custom creator
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
		::CoUninitialize();
		OTExitLib();
		return 1;
	}

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
		::CoUninitialize();
		OTExitLib();
		return returnResult;
	}   //  end if

	// start the service
	::StartServiceCtrlDispatcher(dispatchTable);
	pServer->terminate();
	destroyOpcServer();
	::CoUninitialize();
	OTExitLib();
	return 0;
}   //  end _tWinMain

