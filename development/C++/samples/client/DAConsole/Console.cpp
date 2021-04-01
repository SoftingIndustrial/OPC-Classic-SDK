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

void usage(void)
{
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to activate the objects\n"));
	_tprintf(_T("Press \'c\' or \'C\' to connect the objects\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	_tprintf(_T("Press \'r\' or \'R\' to read the items\n"));
	_tprintf(_T("Press \'w\' or \'W\' to write the items\n"));
	_tprintf(_T("Press \'F\' to refresh the items\n"));
	_tprintf(_T("Press \'z\' or \'Z\' to read using server object\n"));
	_tprintf(_T("Press \'t\' or \'T\' to write using the server object\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get the server status\n"));
	_tprintf(_T("Press \'h\' to change the subscription`s data retrieval mode\n"));
	_tprintf(_T("Press \'m\' to activate the connection monitor\n"));
	_tprintf(_T("Press \'n\' to deactivate the connection monitor\n"));
	_tprintf(_T("Press \'l\' or \'L\' to logon for secure communication\n"));
	_tprintf(_T("Press \'o\' or \'O\' to logoff for secure communication\n"));
	_tprintf(_T("Press \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
#ifdef TBC_OS_WINDOWS
	SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
	// create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();
#ifdef TBC_OS_WINDOWS
	unsigned long waitRet;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
#ifdef TBC_OS_WINDOWS
		CloseHandle(g_endEvent);
#endif
		return 1;
	}

	// initialize the DA client simulation
	result |= pClient->initializeDaObjects();
	usage();
	unsigned long waitTime = 100;
#ifdef TBC_OS_WINDOWS
	BOOL end = FALSE;

	while (!end)
	{
		waitRet = WaitForSingleObject(g_endEvent, waitTime);
		waitTime = 100;

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
			CloseHandle(g_endEvent);
		}
		else
		{
#endif
#ifdef TBC_OS_LINUX
	unsigned long timeout = 0;

	while (timeout < waitTime)
	{
		timespec delay;
		delay.tv_sec = 0;
		delay.tv_nsec = 100000000;  /* 100 milli sec */
		// Wait for the event be signaled
		int retCode = sem_trywait(&g_endSemaphor); // event semaphore handle

		if (!retCode)
		{
			/* Event is signaled */
			break;

		}
		else
		{
			/* check whether somebody else has the mutex */
			if (retCode == EPERM)
			{
				/* sleep for delay time */
				nanosleep(&delay, NULL);
				timeout++ ;
			}

#endif
#ifdef TBC_OS_WINDOWS

			if (_kbhit())
			{
				int input;
				input = _getch();
#endif
#ifdef TBC_OS_LINUX
			char input;

			if (1)
			{
				std::cin.get(input);
#endif

				switch (input)
				{
				case _T('A'):
					pClient->activateAsyncSubscription();
					break;

				case _T('a'):
					pClient->activateSyncSubscription();
					break;

				case _T('C'):
					pClient->connectAsyncSubscription();
					break;

				case _T('c'):
					pClient->connectSyncSubscription();
					break;

				case _T('D'):
					pClient->disconnectAsyncSubscription();
					break;

				case _T('d'):
					pClient->disconnectSyncSubscription();
					break;

				case _T('R'):
					pClient->readAsyncSubscription();
					break;

				case _T('r'):
					pClient->readSyncItem();
					pClient->readSyncSubscription();
					break;

				case _T('F'):
				case _T('f'):
					//the refresh can be done only asynchronously!
					pClient->refreshSubscription();
					break;

				case _T('Z'):
					pClient->readAsyncSession();
					break;

				case _T('z'):
					pClient->readSyncSession();
					break;

				case _T('W'):
					pClient->writeAsyncSubscription();
					break;

				case _T('w'):
					pClient->writeSyncSubscription();
					break;

				case _T('T'):
					pClient->writeAsyncSession();
					break;

				case _T('t'):
					pClient->writeSyncSession();
					break;

				case _T('S'):
					pClient->getAsyncServerStatus();
					break;

				case _T('s'):
					pClient->getSyncServerStatus();
					break;

				case _T('h'):
				case _T('H'):
					pClient->changeDataRetrievalMode();
					break;

				case _T('M'):
				case _T('m'):
					pClient->activateConnectionMonitor();
					break;

				case _T('N'):
				case _T('n'):
					pClient->deactivateConnectionMonitor();
					break;

				case _T('l'):
					pClient->setCredentials();
					pClient->logOnSyncForSecureCommunication();
					break;

				case _T('L'):
					pClient->setCredentials();
					pClient->logOnAsyncForSecureCommunication();
					break;

				case _T('o'):
					pClient->setCredentials(_T("Guest"), _T("nopass"));
					pClient->logOffSyncForSecureCommunication();
					break;

				case _T('O'):
					pClient->logOffAsyncForSecureCommunication();
					pClient->setCredentials(_T("Guest"), _T("nopass"));
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
#ifdef TBC_OS_WINDOWS
					end = TRUE;
					CloseHandle(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
					sem_post(&g_endSemaphor);
#endif
					break;
				} // end switch
			} // end if
		} // end else

#ifdef TBC_OS_WINDOWS
		Sleep(10);
#endif
	} // end while

	pClient->terminate();
	destroyOpcClient();
	return 0;
} // end main
