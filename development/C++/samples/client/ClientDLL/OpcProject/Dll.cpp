#include "stdafx.h"


#include "OpcClient.h"

#include "Mutex.h"

#include <fstream>
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > tofstream;

using namespace std;

BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;
//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationThread(void* context)
{
	OpcClient* pClient = getOpcClient();
	tofstream myFile;
	myFile.open("ClientDll.txt");

	if (!myFile.is_open())
	{
		return 1;
	}

	g_simulationEndMutex.lock();

	while (!g_simulationEnd)
	{
		//  simulate the DA client read operation
		tstring value = pClient->readItem();
		//  log the read value to the file
		myFile << value.c_str() << _T("\n");
		//  simulate the AE client condition stare request
		tstring state = pClient->getConditionState();
		//  log the condition state to the file
		myFile << state.c_str() << _T("\n");
		::Sleep(1000);
	}   //  end while

	myFile.close();
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
		long result = S_OK;
		//  create the OpcClient instance
		createOpcClient();
		OpcClient* pClient = getOpcClient();

		//  initialize the client instance
		if (!SUCCEEDED(pClient->initialize()))
		{
			pClient->terminate();
			destroyOpcClient();
			return 1;
		}   //  end if

		//  initialize the DA client simulation
		result |= pClient->initializeDaObjects();
		//  initialize the AE client simulation
		result |= pClient->initializeAeObjects();

		//  start the simulation thread
		if (SUCCEEDED(result))
		{
			startSimulationThread();
		}   //  end if

		return result;
	}

	_declspec(dllexport) int __cdecl Stop(void)
	{
		OpcClient* pClient = getOpcClient();
		//  terminate the simulation
		g_simulationEnd = TRUE;
		g_simulationEndMutex.lock();
		g_simulationEndMutex.unlock();
		pClient->terminate();
		destroyOpcClient();
		return TRUE;
	}
#ifdef __cplusplus
}
#endif
