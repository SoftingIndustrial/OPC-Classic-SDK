#include "stdafx.h"

#include "OpcServer.h"


BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;

//	the Creator must exist as long as the application exists!
static MyCreator creator;


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
		//  simulate the DA Server data changes generation
		pServer->changeSimulationValues();
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


#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif

	_declspec(dllexport) int __cdecl Start(void)
	{
		long result = 0;
		//  create and initialize the OpcServer instance
		createOpcServer();
		OpcServer* pServer = getOpcServer();
		long initializeResult = pServer->initialize();

		//  provide the server with the proper custom creator
		if (!SUCCEEDED(pServer->prepare(&creator)))
		{
			pServer->terminate();
			destroyOpcServer();
			return 1;
		}

		//  handle the command line arguments (register/unregister, etc)
		tstring commandLine(GetCommandLine());
		result = pServer->processCommandLine(commandLine);

		if (result != S_OK)
		{
			if (result == S_FALSE)
			{
				//  registration operation successful
				pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("Start"),
							   _T("Registration succeeded"));
			}
			else
			{
				//  registration operation has failed
				pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("Start"),
							   _T("Registration failed"));
			}   //  end if...else

			//  no matter what colse the application if processCommandLine returned
			//  something different of S_OK
			pServer->terminate();
			destroyOpcServer();
			return result;
		}   //  end if

		long startResult = pServer->start();

		//  Start the OPC server's I/O internal mechanism
		if (SUCCEEDED(startResult))
		{
			//  build the namespace
			pServer->buildAddressSpace();
			pServer->buildEventCategories();
			//  declare the namespaces built and the server ready for clients to connect
			pServer->ready();
		}   //  end if

		//  start the simulation thread
		if (SUCCEEDED(result))
		{
			startSimulationThread();
		}   //  end if

		return result;
	}

	_declspec(dllexport) int __cdecl Stop(void)
	{
		OpcServer* pServer = getOpcServer();
		//  terminate the simulation
		g_simulationEnd = TRUE;
		g_simulationEndMutex.lock();
		g_simulationEndMutex.unlock();
		pServer->stop();
		pServer->terminate();
		destroyOpcServer();
		return TRUE;
	}
#ifdef __cplusplus
}
#endif
