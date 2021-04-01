#include "stdafx.h"

#include "OpcServer.h"

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
void sigint(int sig);
PosixEvents g_events;
#endif

DateTime now;
unsigned short second   = 0xFF;
unsigned short minute   = 0xFF;

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
#ifdef TBC_OS_WINDOWS
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
#ifdef TBC_OS_LINUX
	(void) signal(SIGINT, sigint);
	g_events.create(false, false);
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
		::CoUninitialize();
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
		::CoUninitialize();
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

	_tprintf(_T("Press Ctrl-C to exit\n"));
	unsigned long waitTime = 100;
	BOOL end = FALSE;
	now.now();
	Variant initialSecond(now.second());
	pServer->changeValue(initialSecond, DAVariableType_Sec);
	Variant initialMinute(now.minute());
	pServer->changeValue(initialMinute, DAVariableType_Min);
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
			now.now();

			if (second != now.second())
			{
				Variant sec(now.second());
				pServer->changeValue(sec, DAVariableType_Sec);

				if (sec.iVal == 0)
				{
					Variant minute(now.minute());
					pServer->changeValue(minute, DAVariableType_Min);
				}
			}   //  end if
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
			now.now();

			if (second != now.second())
			{
				Variant sec(now.second());
				pServer->changeValue(sec, DAVariableType_Sec);

				if (sec.iVal == 0)
				{
					Variant minute(now.minute());
					pServer->changeValue(minute, DAVariableType_Min);
				}
			}   //  end if
		}   //  end if...else
	}   //  end while

#endif
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	::CloseHandle(g_endEvent);
	::CoUninitialize();
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
