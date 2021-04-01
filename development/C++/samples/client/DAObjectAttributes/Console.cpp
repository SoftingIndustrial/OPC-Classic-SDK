#include "OSCompat.h"
#include "stdafx.h"

#include "OpcClient.h"

#include <iostream>

#ifdef TBC_OS_WINDOWS
#include <conio.h>
#endif

#include <stdio.h>

#ifdef TBC_OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#endif

using namespace std;
using namespace SoftingOPCToolboxClient;
#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
#endif
#ifdef TBC_OS_LINUX
sem_t g_endSemaphor;
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
	// create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
		return 1;
	}

	// initialize the DA client simulation
	result |= pClient->initializeDaObjects();
	pClient->printObjectAttributes();
	pClient->changeSessionAttributes();
	pClient->changeSubscriptionAttributes();
	pClient->changeItemAttributes();
	pClient->terminate();
	destroyOpcClient();
	_tprintf(_T("\nPress any key to finish..."));
#ifdef TBC_OS_WINDOWS
	_getch();
#endif
#ifdef TBC_OS_LINUX
	char input;
	std::cin.get(input);
#endif
	return 0;
} // end main
