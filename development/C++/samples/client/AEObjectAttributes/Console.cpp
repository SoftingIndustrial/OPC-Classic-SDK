#include "stdafx.h"

#include "OpcClient.h"

#include <conio.h>

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

	// initialize the AE client simulation
	result |= pClient->initializeAeObjects();
	pClient->printInitialObjectAttributes();
	pClient->setSessionAttributes();
	pClient->setSubscriptionAttributes();
	pClient->terminate();
	destroyOpcClient();
	_tprintf(_T("\nPress any key to finish..."));
	_getch();
	return 0;
} // end main
