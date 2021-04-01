#include "stdafx.h"

#include "OpcServer.h"
#include "MyCreator.h"

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
void sigint(int sig);
PosixEvents g_events;
pthread_t g_simulationThread;
pthread_t g_handleRequestsThread;
#endif

BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;
BOOL g_handleRequestsEnd = FALSE;
Mutex g_handleRequestsMutex;

#ifdef TBC_OS_WINDOWS
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
		//  simulate the DA Server data changes generation
		pServer->changeSimulationValues();
		::Sleep(1000);
	}   //  end while

	g_simulationEndMutex.unlock();
	return 0;
}   //  end simulationThread
#endif
#ifdef TBC_OS_LINUX
void* simulationThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	g_simulationEndMutex.lock();

	while (!g_simulationEnd)
	{
		//  simulate the DA Server data changes generation
		pServer->changeSimulationValues();
		usleep(1000 * 1000);
	}   //  end while

	g_simulationEndMutex.unlock();
	pthread_exit(NULL);
	return NULL;
}
#endif
//-----------------------------------------------------------------------------
//	startSimulationThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startSimulationThread(void)
{
#ifdef TBC_OS_WINDOWS
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &simulationThread, NULL, 0, &tID);
#endif
#ifdef TBC_OS_LINUX

	if (!g_simulationEnd)
	{
		pthread_create(&g_simulationThread, NULL, simulationThread, NULL);
	}

#endif
}   //  end startThread

#ifdef TBC_OS_WINDOWS
//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI handleRequestsThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	g_handleRequestsMutex.lock();

	while (!g_handleRequestsEnd)
	{
		//  simulate the DA Server data changes generation
		pServer->handleRequests();
		::Sleep(100);
	}   //  end while

	g_handleRequestsMutex.unlock();
	return 0;
}   //  end handleRequestsThread
#endif
#ifdef TBC_OS_LINUX
void* handleRequestsThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	g_handleRequestsMutex.lock();

	while (!g_handleRequestsEnd)
	{
		//  simulate the DA Server data changes generation
		pServer->handleRequests();
		usleep(100 * 1000);
	}   //  end while

	g_handleRequestsMutex.unlock();
	pthread_exit(NULL);
	return NULL;
}
#endif
//-----------------------------------------------------------------------------
//	startHandleRequestsThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startHandleRequestsThread(void)
{
#ifdef TBC_OS_WINDOWS
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &handleRequestsThread, NULL, 0, &tID);
#endif
#ifdef TBC_OS_LINUX

	if (!g_simulationEnd)
	{
		pthread_create(&g_handleRequestsThread, NULL, handleRequestsThread, NULL);
	}

#endif
}   //  end startHandleRequestsThread

#ifdef TBC_OS_WINDOWS
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
#endif


#ifdef TBC_OS_LINUX
tstring GetCommandLine(int argc, char* argv[])
{
	tstringstream str;

	for (int i = 1; i <= argc; i++)
	{
		str << argv[i];
	}

	return str.str();
}
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
#ifdef TBC_OS_LINUX
	(void) signal(SIGINT, sigint);
	g_events.create(false, false);
#endif
#ifdef TBC_OS_WINDOWS
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
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
#ifdef TBC_OS_WINDOWS
		::CloseHandle(g_endEvent);
#endif
		return 1;
	}

	//  handle the command line arguments (register/unregister, etc)
#ifdef TBC_OS_WINDOWS
	tstring commandLine(GetCommandLine());
#endif
#ifdef TBC_OS_LINUX
	tstring commandLine(GetCommandLine(argc, argv));
#endif
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
#ifdef TBC_OS_WINDOWS
		::CloseHandle(g_endEvent);
#endif
		return returnResult;
	}   //  end if

	//  Start the OPC server's I/O internal mechanism
	if (SUCCEEDED(pServer->start()))
	{
		//  build the namespace
		pServer->buildAddressSpace();
		//  declare the namespaces built and the server ready for clients to connect
		pServer->ready();
	}

	//  start the simulation thread
	if (SUCCEEDED(result))
	{
		startSimulationThread();
		startHandleRequestsThread();
	}   //  end if

	_tprintf(_T("Press Ctrl-C to exit\n"));
	unsigned long waitTime = 100;
	BOOL end = FALSE;
#ifdef TBC_OS_WINDOWS

	while (!end)
	{
		unsigned long waitRet = 0;
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

#endif
#ifdef TBC_OS_LINUX

	while (!end)
	{
		DWORD ret = g_events.waitForAll(false, waitTime);

		if (ret == 0)
		{
			end = TRUE;
		}
		else
		{
			//  TODO: place your cyclic code here
		}   //  end if...else
	}   //  end while

#endif
	//  terminate the simulation
	g_simulationEnd = TRUE;
	//  give the thread time to smoothly closetdown
	g_simulationEndMutex.lock();
	g_simulationEndMutex.unlock();
	g_handleRequestsEnd = TRUE;
	//  give the thread time to smoothly closetdown
	g_handleRequestsMutex.lock();
	g_handleRequestsMutex.unlock();
	//  terminate IO
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	::CloseHandle(g_endEvent);
#endif
	return 0;
}   //  end main

#ifdef TBC_OS_LINUX
void sigint(int sig)
{
	_tprintf(_T("You just pressed Ctrl-C\n"));
	g_events.signal(0);
}
#endif
