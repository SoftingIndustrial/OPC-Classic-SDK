#include "stdafx.h"


#include "OpcClient.h"

#include "Mutex.h"

#include <fstream>

typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > tofstream;

using namespace std;

// The following constant holds the name and the description of the Windows NT service
// the OPC application runs under.
//    TODO : change your service name and description here
const tstring g_defaultServiceName = _T("DaConsole_Service OpcService");
const tstring g_defaultServiceDescription = _T("Softing OPC Toolkit - OPC DA Client. \r\n\r\nSample name: \"C++ DaConsole_Service\".");

SERVICE_STATUS g_serviceStatus = {0};
SERVICE_STATUS_HANDLE g_serviceStatusHandle =   0;
HANDLE g_serviceStopEvent   =   0;

// instance handle of application
HINSTANCE g_instance = 0;


//-----------------------------------------------------------------------------
// reportStatusToSCMgr  - service manager
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
	} // end if...else

	// report the status of the service to the service control manager.
	return SetServiceStatus(g_serviceStatusHandle, &g_serviceStatus);
} // end reportStatusToSCMgr


//-----------------------------------------------------------------------------
// serviceControlHandler    - service manager
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

		if (g_serviceStopEvent)
		{
			::SetEvent(g_serviceStopEvent);
		} // end if
	} // end case
	break;
	} // end switch
} // end serviceControlHandler

void WINAPI ServiceMain(
	IN DWORD argc,
	IN LPTSTR* argv)
{
	g_serviceStatusHandle = ::RegisterServiceCtrlHandler(g_defaultServiceName.c_str(), serviceControlHandler);

	if (g_serviceStatusHandle)
	{
		g_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		g_serviceStatus.dwServiceSpecificExitCode = 0;
		g_serviceStatus.dwWin32ExitCode = 0;
		reportStatusToSCMgr(SERVICE_START_PENDING, 1000);
		g_serviceStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		BOOL started = FALSE;
		long result = S_OK;
		OpcClient* pClient = getOpcClient();
		// initialize the DA client simulation
		result |= pClient->initializeDaObjects();
		reportStatusToSCMgr(SERVICE_RUNNING, 0);
		WaitForSingleObject(g_serviceStopEvent, INFINITE);
		CloseHandle(g_serviceStopEvent);
		reportStatusToSCMgr(SERVICE_STOPPED, 0);
	} // end if
} // end ServiceMain


int APIENTRY _tWinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPTSTR cmdLine,
	int cmdShow)
{
	long result = S_OK;
	g_instance = instance;
	// create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();
	// set the service name and description to the OpcClient instance
	pClient->setServiceName(g_defaultServiceName);
	pClient->setServiceDescription(g_defaultServiceDescription);
	BOOL commandLineProcessed = FALSE;
	tstring commandLine(GetCommandLine());
	BOOL end = FALSE;
	int ret = 0;
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPTSTR)g_defaultServiceName.c_str(), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
		::CoUninitialize();
		return 1;
	}

	// handle the command line arguments (register/unregister)
	if (!commandLineProcessed)
	{
		long result = pClient->processCommandLine(commandLine);
		commandLineProcessed = TRUE;

		if (result != S_OK)
		{
			long returnResult = 0;

			if (result == S_FALSE)
			{
				// registration operation successful
				pClient->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
							   _T("Registration succeeded"));
				returnResult = 0;
			}
			else
			{
				// registration operation has failed
				pClient->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
							   _T("Registration failed"));
				returnResult = 1;
			}

			// no matter what close the application if processCommandLine returned
			// something different of S_OK
			pClient->terminate();
			destroyOpcClient();
			::CoUninitialize();
			return returnResult;
		} // end if
	}

	// start the service
	::StartServiceCtrlDispatcher(dispatchTable);
	pClient->terminate();
	destroyOpcClient();
	::CoUninitialize();
	return 0;
} // end _tWinMain

