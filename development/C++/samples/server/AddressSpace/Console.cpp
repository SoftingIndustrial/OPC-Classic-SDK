#include "stdafx.h"
#include "OpcServer.h"

using namespace SoftingOPCToolboxServer;

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
HANDLE g_changedEvent = 0;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
PosixEvents g_events;
void sigint(int sig);
#endif

BOOL g_holdSignalChange = FALSE;
Mutex g_syncDisplay;


#ifdef TBC_OS_WINDOWS
//-----------------------------------------------------------------------------
// controlHandler
//
// - handle console control events
//
// returns:
//		TRUE  - handled event
//		FALSE - event not handled
//
BOOL WINAPI controlHandler(IN unsigned long crtlType)   // control type
{
	switch (crtlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to end server
	case CTRL_C_EVENT:
	{
		if (g_endEvent)
		{
			::SetEvent(g_endEvent);
		}

		return TRUE;
	}
	break;
	}

	return FALSE;
}   //  controlHandler
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
	unsigned long result = S_OK;
#ifdef TBC_OS_WINDOWS
	g_endEvent = CreateEvent(NULL, TRUE, FALSE, _T("EndEvent"));
	g_changedEvent = CreateEvent(NULL, FALSE, FALSE, _T("ChangedEvent"));
	SetConsoleCtrlHandler(controlHandler, TRUE);
#endif
#ifdef TBC_OS_LINUX
	(void) signal(SIGINT, sigint);
	g_events.create(false, false);
	g_events.create(false, false);
#endif
	//  create and initialize the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	pServer->initialize();
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
#ifdef TBC_OS_WINDOWS
		::CloseHandle(g_endEvent);
		::CloseHandle(g_changedEvent);
#endif
		return 1;
	}

#ifdef TBC_OS_WINDOWS
	tstring commandLine(GetCommandLine());
#endif
#ifdef TBC_OS_LINUX
	tstring commandLine(GetCommandLine(argc, argv));
#endif
	result = pServer->processCommandLine(commandLine);

	if (result == S_OK)
	{
		if (SUCCEEDED(pServer->start()))
		{
			//  build the namespace
			pServer->buildAddressSpace();
			//  declare the namespaces built and the server ready for clients to connect
			pServer->ready();
			_tprintf(_T("Press Ctrl-C to exit\n"));
			g_syncDisplay.lock();
			pServer->showObjectTree();
			g_syncDisplay.unlock();
			//  loop specific variables
			unsigned long waitTime = 1000;
			BOOL end = FALSE;
			int count = 0;
#ifdef TBC_OS_WINDOWS
			unsigned long waitRet = 0;
			HANDLE eventHandles[2];
			eventHandles[0] = g_endEvent;
			eventHandles[1] = g_changedEvent;

			while (!end)
			{
				waitRet = WaitForMultipleObjects(2, eventHandles, FALSE, waitTime);

				switch (waitRet)
				{
				case WAIT_OBJECT_0:
					end = TRUE;
					break;

				case WAIT_OBJECT_0 + 1:
					g_syncDisplay.lock();
					pServer->showObjectTree();
					g_syncDisplay.unlock();
					break;

				default:
					if (count > 30)
					{
						g_holdSignalChange = TRUE;
						pServer->updateAddressSpaceElements();
						pServer->rebuildRandomNamespace();
						g_holdSignalChange = FALSE;
						::SetEvent(g_changedEvent);
						count = 0;
					}
					else
					{
						count++;
					} // end if .. else
				}   // end switch
			}   //  end while

#endif
#ifdef TBC_OS_LINUX

			while (!end)
			{
				DWORD ret = g_events.waitForAll(false, waitTime);

				switch (ret)
				{
				case 0: // Terminate thread
					end = TRUE;
					break;

				case 1:
					g_syncDisplay.lock();
					pServer->showObjectTree();
					g_syncDisplay.unlock();
					break;

				default:
					if (count > 30)
					{
						g_holdSignalChange = TRUE;
						pServer->updateAddressSpaceElements();
						pServer->rebuildRandomNamespace();
						g_holdSignalChange = FALSE;
						g_events.signal(1);
						count = 0;
					}
					else
					{
						count++;
					} // end if .. else
				}   //  end switch
			}   //  end while

#endif
			pServer->stop();
		}   //  end if
	}
	else if (result == S_FALSE)
	{
		_tprintf(_T("Registration operation %s"), commandLine.c_str());
	}
	else
	{
		_tprintf(_T("processCommandLine has failed"));
	}   //  end if ... else

	pServer->terminate();
	destroyOpcServer();
	releaseApp();
#ifdef TBC_OS_WINDOWS
	::CloseHandle(g_endEvent);
	::CloseHandle(g_changedEvent);
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
