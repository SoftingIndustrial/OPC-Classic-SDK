#include "stdafx.h"

#include "OpcServer.h"

#include <vector>
#include <iostream>

#ifdef TBC_OS_WINDOWS
#include <conio.h>
#include <crtdbg.h>
#endif

using namespace SoftingOPCToolboxServer;

// events
#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent;
HANDLE g_changedEvent;
#endif
#ifdef TBC_OS_LINUX
#include <signal.h>
void sigint(int sig);
PosixEvents g_events;
#endif

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
} // end controlHandler
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
#ifdef TBC_OS_WINDOWS
	g_endEvent = CreateEvent(NULL, TRUE, FALSE, _T("EndEvent"));
	g_changedEvent = CreateEvent(NULL, FALSE, FALSE, _T("ChangedEvent"));
	SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
#ifdef TBC_OS_LINUX
	(void) signal(SIGINT, sigint);
	g_events.create(false, false);
	g_events.create(false, false);
#endif
	// create and initialize the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	pServer->initialize();
	// provide the server with the proper custom creator
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
#ifdef TBC_OS_WINDOWS
		CloseHandle(g_endEvent);
#endif
		return 1;
	}

	// handle the command line arguments (register/unregister, etc)
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
			// registration operation successful
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration succeeded"));
			returnResult = 0;
		}
		else
		{
			// registration operation has failed
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration failed"));
			returnResult = 1;
		}

		// no matter what close the application if processCommandLine returned
		// something different of S_OK
		pServer->terminate();
		destroyOpcServer();
#ifdef TBC_OS_WINDOWS
		CloseHandle(g_endEvent);
#endif
		return returnResult;
	} // end if

	// Start the OPC server's I/O internal mechanism
	if (SUCCEEDED(pServer->start()))
	{
		pServer->buildAddressSpace();
		// declare the namespaces built and the server ready for clients to connect
		pServer->ready();
		pServer->showObjectTree();
	}

	unsigned long waitTime = 1000;
	BOOL end = FALSE;
#ifdef TBC_OS_WINDOWS
	unsigned long waitRet = 0;
	HANDLE eventHandles[2];
	eventHandles[0] = g_endEvent;
	eventHandles[1] = g_changedEvent;

	while (!end)
	{
		waitRet = WaitForMultipleObjects(2, eventHandles, FALSE, waitTime);

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
		}
		else
		{
			pServer->showObjectTree();
		}   //  end if ... else
	}   //  end while

#endif
#ifdef TBC_OS_LINUX

	while (!end)
	{
		DWORD ret = g_events.waitForAll(false, waitTime);

		switch (ret)
		{
		case 0: // Terminate thread
		{
			end = TRUE;
		}
		break;

		default: // Reconnect servers
		{
			pServer->showObjectTree();
		}
		break;
		}   //  end switch
	}   //  end while

#endif
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	CloseHandle(g_endEvent);
#endif
}   //  end main

#ifdef TBC_OS_LINUX
void sigint(int sig)
{
	_tprintf(_T("You just pressed Ctrl-C\n"));
	g_events.signal(0);
}
#endif
