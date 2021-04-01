#include "stdafx.h"

#include <conio.h>
#include <stdio.h>

#include "OpcClient.h"

HANDLE g_endEvent = 0;

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

void usage(void)
{
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to activate the objects\n"));
	_tprintf(_T("Press \'c\' or \'C\' to connect the objects\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get the server status\n"));
	_tprintf(_T("Press \'m\' to activate the connection monitor\n"));
	_tprintf(_T("Press \'n\' to deactivate the connection monitor\n"));
	_tprintf(_T("Press \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n\n"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	// create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
		CloseHandle(g_endEvent);
		return 1;
	}

	// initialize the AE client simulation
	result |= pClient->initializeAeObjects();
	usage();
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
			if (_kbhit())
			{
				int input;
				input = _getch();

				switch (input)
				{
				case _T('A'):
					pClient->activateAsyncObjects();
					break;

				case _T('a'):
					pClient->activateSyncObjects();
					break;

				case _T('C'):
					pClient->connectAsyncObjects();
					break;

				case _T('c'):
					pClient->connectSyncObjects();
					break;

				case _T('D'):
					pClient->disconnectAsyncObjects();
					break;

				case _T('d'):
					pClient->disconnectSyncObjects();
					break;

				case _T('S'):
					pClient->getAsyncServerStatus();
					break;

				case _T('s'):
					pClient->getSyncServerStatus();
					break;

				case _T('M'):
				case _T('m'):
					pClient->activateConnectionMonitor();
					break;

				case _T('N'):
				case _T('n'):
					pClient->deactivateConnectionMonitor();
					break;

				case _T('?'):
				case _T('U'):
				case _T('u'):
					usage();
					break;

				case _T('E'):
				case _T('e'):
				case _T('Q'):
				case _T('q'):
					end = TRUE;
					CloseHandle(g_endEvent);
					break;
				} // end switch
			} // end if
		} // end else
	} // end while

	pClient->terminate();
	destroyOpcClient();
	return 0;
} // end main
