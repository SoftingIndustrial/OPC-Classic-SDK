#include "stdafx.h"

#include "OpcServer.h"


HANDLE g_endEvent = 0;

BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;
//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	g_simulationEndMutex.lock();

	while (!g_simulationEnd)
	{
		//  simulate the AE Server events generation
		pServer->fireSimulationEvents();
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

BOOL WINAPI ControlHandler(IN unsigned long crtlType)   // control type
{
	switch (crtlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to end server
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		if (g_endEvent)
		{
			SetEvent(g_endEvent);
		}

		return TRUE;
	}

	return FALSE;
}   //  end controlHandler


int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	//  create and initialize the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	pServer->initialize();
	//  provide the server with the proper custom creator
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
		CoUninitialize();
		CloseHandle(g_endEvent);
		return 1;
	}

	//  handle the command line arguments (register/unregister, etc)
	tstring commandLine(GetCommandLine());
	result = pServer->processCommandLine(commandLine);

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

		//  no matter what close the application if processCommandLine returned
		//  something different of S_OK
		pServer->terminate();
		destroyOpcServer();
		CoUninitialize();
		CloseHandle(g_endEvent);
		return returnResult;
	}   //  end if

	//  Start the OPC server's I/O internal mechanism
	if (SUCCEEDED(pServer->start()))
	{
		//  build the namespace
		pServer->buildAddressSpace();
		pServer->buildEventCategories();
		//  declare the namespaces built and the server ready for clients to connect
		pServer->ready();
	}

	//  start the simulation thread
	if (SUCCEEDED(result))
	{
		startSimulationThread();
	}   //  end if

	_tprintf(_T("Press Ctrl-C to exit\n"));
	unsigned long waitTime = 100;
	BOOL end = FALSE;
	unsigned long waitRet = 0;

	while (!end)
	{
		waitRet = WaitForSingleObject(g_endEvent, waitTime);
		waitTime = 100;

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
		}
		else
		{
			//  TODO: place your cyclic code here
		}   //  end if...else
	}   //  ens while

	//  terminate the simulation
	g_simulationEnd = TRUE;
	g_simulationEndMutex.lock();
	g_simulationEndMutex.unlock();
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
	CloseHandle(g_endEvent);
	CoUninitialize();
	return 0;
}   //  end main
