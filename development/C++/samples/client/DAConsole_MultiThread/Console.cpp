#include "stdafx.h"
#include "Mutex.h"
#include "OSCompat.h"

#include "OpcClient.h"

#include <iostream>
#ifdef TBC_OS_WINDOWS
#include <conio.h>
#include <time.h>
#endif

#include <stdio.h>

#ifdef TBC_OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#endif

/**
 * TODO: update these settings as required
 */
char g_initialThreadCount = 10;
bool g_tracesOn = true;
bool g_threadsStarted = false;
const unsigned int g_subscriptionCount = 2; // >0
const unsigned int g_itemsCountPerSubscription = 5; // >0
const unsigned int g_itemsTotalCount = g_subscriptionCount * g_itemsCountPerSubscription;
DWORD g_updateRate = 1000;
DWORD g_operationInterval = 1000;
const TCHAR g_itemName[256] = _T("addressSpaceSample.itemsLoadTest.namedItem_");
const TCHAR g_itemNameFormat[16] = _T("%s%05d");
const unsigned short g_minStringLength = 150;
const unsigned short g_maxStringLength = 250;
int g_random = 12345;
const unsigned int g_silentDisplayCount = 100;
const bool g_connectItemsFirst = false;
bool g_allowItemRemoval = false;
/**
 * END TODO
 */

unsigned long g_dataChangeSetsReceived = 0;
unsigned long g_totalGoodDataChanges = 0;
unsigned long g_totalBadDataChanges = 0;
unsigned long g_legitGoodDataChanges = 0;
unsigned long g_legitBadDataChanges = 0;
bool g_subscriptionsActive = false;
HANDLE g_threadNewLine;
Mutex g_syncDisplay;
using namespace std;
using namespace SoftingOPCToolboxClient;
#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
HANDLE g_threadsStopEvent = 0;
HANDLE g_threads[50] = {0};
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
	g_syncDisplay.lock();
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to activate the objects\n"));
	_tprintf(_T("Press \'c\' or \'C\' to connect the objects\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	_tprintf(_T("Press \',\' or \'>\' to connect session (deep)\n"));
	_tprintf(_T("Press \'.\' or \'<\' to disconnect session (deep)\n"));
	_tprintf(_T("Press \'r\' or \'R\' to read the items\n"));
	_tprintf(_T("Press \'w\' or \'W\' to write the items\n"));
	_tprintf(_T("Press \'f\' or \'F\' to refresh the items\n"));
	_tprintf(_T("Press \'z\' or \'Z\' to read using server object\n"));
	_tprintf(_T("Press \'t\' or \'T\' to write using the server object\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get the server status\n"));
	_tprintf(_T("Press \'m\' to activate the connection monitor\n"));
	_tprintf(_T("Press \'n\' to deactivate the connection monitor\n"));
	_tprintf(_T("Press \'l\' or \'L\' to logon for secure communication\n"));
	_tprintf(_T("Press \'o\' or \'O\' to logoff for secure communication\n"));
	_tprintf(_T("\nPress \'&\' to remove random items\n"));
	_tprintf(_T("\nPress \'(\' to re-add random items\n"));
	_tprintf(_T("\nPress \'1\' to start the threads\n"));
	_tprintf(_T("Press \'0\' to stop the threads\n\t - threads are %s -\n"), g_threadsStarted == true ? _T("started") : _T("stopped"));
	_tprintf(_T("\nPress \'7\' to allow item removal/addition\n"));
	_tprintf(_T("Press \'9\' to disallow item removal/addition\n\t - item removal/addition is %s -\n"), g_allowItemRemoval == true ? _T("allowed") : _T("disallowed"));
	_tprintf(_T("\nPress \'+\' to increase the thread count (max 50 threads)\n"));
	_tprintf(_T("Press \'-\' to decrease the thread count (min 1 thread)\n\t - number of threads is %d -\n"), g_initialThreadCount);
	_tprintf(_T("\nPress \'\\\' to manually specify the update rate\n"));
	_tprintf(_T("Press \'[\' to decrease the update rate\n"));
	_tprintf(_T("Press \']\' to increase the update rate\n\t - current update rate is %d -\n"), g_updateRate);
	_tprintf(_T("\nPress \'5\' to manually specify the thread operation interval\n"));
	_tprintf(_T("Press \'4\' to decrease the thread operation interval\n"));
	_tprintf(_T("Press \'6\' to increase the thread operation interval\n\t - current thread operation interval is %dms (%.2f op/sec) -\n"), g_operationInterval, ((float)1000)/g_operationInterval);
	_tprintf(_T("\nPress \'/\' to disable output\n"));
	_tprintf(_T("Press \'*\' to enable output\n\t - output is %s -\n"), g_tracesOn == true ? _T("enabled") : _T("disabled"));
	_tprintf(_T("\nPress \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n"));
	g_syncDisplay.unlock();
}

DWORD NewLineThreadEntryPoint(LPVOID param)
{
	DWORD currentWaitTime = 0;
	while (true)
	{
		if (g_subscriptionsActive == true)
		{
			if (currentWaitTime >= g_updateRate)
			{
				currentWaitTime = 0;
				if (g_syncDisplay.lock(0))
				{
					char buffer[256];
					_stprintf_s(buffer, _T("-------- s:%d, t:%d, g:%d b:%d (tl: %d, lg:%d, lb:%d) --------"), g_dataChangeSetsReceived, g_totalGoodDataChanges + g_totalBadDataChanges,
						g_totalGoodDataChanges, g_totalBadDataChanges, g_legitGoodDataChanges + g_legitBadDataChanges, g_legitGoodDataChanges, g_legitBadDataChanges);
					_tprintf(_T("\n%s\n\n"), buffer);
					getApplication()->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, "NewLineThreadEntryPoint", buffer);
					g_dataChangeSetsReceived = 0;
					g_totalGoodDataChanges = 0;
					g_totalBadDataChanges = 0;
					g_legitGoodDataChanges= 0;
					g_legitBadDataChanges= 0;
					g_syncDisplay.unlock();
				}
			}
			else
			{
				++currentWaitTime;
				::Sleep(1);
			}
		}
		else
		{
			::Sleep(500);
		}
		if (WaitForSingleObject(g_threadsStopEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

DWORD WorkerThreadEntryPoint(LPVOID param)
{
	DWORD currentWaitTime = 0;
	int threadId = g_initialThreadCount;
	OpcClient* pClient = getOpcClient();
	byte reminder;
	while (true)
	{
		if (g_threadsStarted == true)
		{
			if (currentWaitTime >= g_operationInterval)
			{
				currentWaitTime = 0;
				reminder = 12;
				if (g_allowItemRemoval == true)
				{
					reminder += 2;
				}
				switch(g_random % reminder)
				{
				case 1:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					pClient->readAsyncSubscription();
					break;
				case 2:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->readSyncItem();
					break;
				case 3:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->readSyncSubscription();
					break;
				case 4:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					pClient->refreshSubscription();
					break;
				case 5:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->readAsyncSession();
					break;
				case 6:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->readSyncSession();
					break;
				case 7:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					pClient->writeAsyncSubscription();
					break;
				case 8:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->writeSyncSubscription();
					break;
				case 9:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->writeAsyncSession();
					break;
				case 10:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					//pClient->writeSyncSession();
					break;
				case 11:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					pClient->getAsyncServerStatus();
					break;
				case 12:
					{
						if (g_syncDisplay.lock(0))
						{
							_tprintf(_T(" %d:"), threadId);
							g_syncDisplay.unlock();
						}
						pClient->removeRandomItems();
						break;
					}
				case 13:
					{
						if (g_syncDisplay.lock(0))
						{
							_tprintf(_T(" %d:"), threadId);
							g_syncDisplay.unlock();
						}
						pClient->addRandomItems();
						break;
					}
				default:
					if (g_syncDisplay.lock(0))
					{
						_tprintf(_T(" %d:"), threadId);
						g_syncDisplay.unlock();
					}
					pClient->getSyncServerStatus();
					break;
				}
				g_random = (g_random * 19) / 7 + 1;
			}
			else
			{
				++currentWaitTime;
				::Sleep(1);
			}
		}
		else
		{
			::Sleep(500);
		}
		if (WaitForSingleObject(g_threadsStopEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand((unsigned int)time(NULL));
	g_random = rand();
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
	g_threadsStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
#ifdef TBC_OS_WINDOWS
		CloseHandle(g_endEvent);
		CloseHandle(g_threadsStopEvent);
#endif
		return 1;
	}

	// initialize the DA client simulation
	result |= pClient->initializeDaObjects();
	usage();
#ifdef TBC_OS_WINDOWS
	BOOL end = FALSE;

	DWORD threadId;
	int n = g_initialThreadCount;
	g_initialThreadCount = 0;
	for (int i = 0; i < n; ++i)
	{
		g_threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WorkerThreadEntryPoint, NULL, 0, &threadId);
		++g_initialThreadCount;
	}

	g_threadNewLine = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&NewLineThreadEntryPoint, NULL, 0, &threadId);

	while (!end)
	{
		waitRet = WaitForSingleObject(g_endEvent, 100);

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
					g_subscriptionsActive = true;
					break;

				case _T('a'):
					pClient->activateSyncSubscription();
					g_subscriptionsActive = true;
					break;

				case _T('C'):
					pClient->connectAsyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('c'):
					pClient->connectSyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('D'):
					pClient->disconnectAsyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('d'):
					pClient->disconnectSyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('R'):
					pClient->readAsyncSubscription();
					break;

				case _T('r'):
					//pClient->readSyncItem();
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
				case _T('.'):
				case _T('>'):
					pClient->connectSession();
					g_subscriptionsActive = false;
					break;
				case _T(','):
				case _T('<'):
					pClient->disconnectSession();
					g_subscriptionsActive = false;
					break;

				/*
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
				*/

				case _T('?'):
				case _T('U'):
				case _T('u'):
					usage();
					break;

				case _T('+'):
					g_syncDisplay.lock();
					if (g_initialThreadCount < 50)
					{
						g_threads[g_initialThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WorkerThreadEntryPoint, NULL, 0, &threadId);
						++g_initialThreadCount;
						_tprintf(_T("\nNumber of threads increased to: %d\n"), g_initialThreadCount);
					}
					else
					{
						_tprintf(_T("\nWARNING: Number of threads is already %d, cannot increase anymore!!!\n"), g_initialThreadCount);
					}
					g_syncDisplay.unlock();
					break;
				case _T(']'):
					g_syncDisplay.lock();
					if (g_updateRate < 0xfffffff5)
					{
						g_updateRate += 10;
						pClient->modifyUpdateRate();
						_tprintf(_T("\nUpdate rate increased to: %dms\n"), g_updateRate);
					}
					else
					{
						_tprintf(_T("\nWARNING: Update rate is already %dms, cannot increase anymore!!!\n"), g_updateRate);
					}
					g_syncDisplay.unlock();
					break;
				case _T('['):
					g_syncDisplay.lock();
					if (g_updateRate > 11)
					{
						g_updateRate -= 10;
						pClient->modifyUpdateRate();
						_tprintf(_T("\nUpdate rate decreased to: %dms\n"), g_updateRate);
					}
					else
					{
						_tprintf(_T("\nWARNING: Update rate is already %dms, cannot decrease anymore!!!\n"), g_updateRate);
					}
					g_syncDisplay.unlock();
					break;
				case _T('5'):
					{
						g_syncDisplay.lock();
						char intStr[256];
						long long intDw = 0;
						_tprintf(_T("\nNew thread operation interval: [0 ... 4294967295): "));
						scanf(_T("%s"), intStr);
						intDw = atoi(intStr);
						if (intDw > 0)
						{
							if (intDw < MAXDWORD)
							{
								g_operationInterval = (DWORD)intDw;
								_tprintf(_T("\nNew thread operation interval (%dms, %.2f op/sec) successfuly set.\n"), g_operationInterval, ((float)1000)/g_operationInterval);
							}
							else
							{
								_tprintf(_T("\nWARNING: Provided thread operation interval (%d) is higher than 4294967295!!!\n"), intDw);
							}
						}
						else
						{
							_tprintf(_T("\nWARNING: Provided update rate (%d) is lower than 0!!!\n"), intDw);
						}
						g_syncDisplay.unlock();
						break;
					}
				case _T('4'):
					g_syncDisplay.lock();
					if (g_operationInterval < 0xfffffff5)
					{
						g_operationInterval += 10;
						_tprintf(_T("\nThread operation interval increased to: %dms (%.2f op/sec)\n"), g_operationInterval, ((float)1000)/g_operationInterval);
					}
					else
					{
						_tprintf(_T("\nWARNING: Thread operation interval is already %dms (%.2f op/sec), cannot increase anymore!!!\n"), g_operationInterval, ((float)1000)/g_operationInterval);
					}
					g_syncDisplay.unlock();
					break;
				case _T('6'):
					g_syncDisplay.lock();
					if (g_operationInterval > 10)
					{
						g_operationInterval -= 10;
						_tprintf(_T("\nThread operation interval decreased to: %dms (%.2f op/sec)\n"), g_operationInterval, ((float)g_operationInterval)/1000);
					}
					else
					{
						_tprintf(_T("\nWARNING: Thread operation interval is already %dms (%.2f op/sec), cannot decrease anymore!!!\n"), g_operationInterval, ((float)1000)/g_operationInterval);
					}
					g_syncDisplay.unlock();
					break;
				case _T('\\'):
					{
						g_syncDisplay.lock();
						char intStr[256];
						long long intDw = 0;
						_tprintf(_T("\nNew update rate: [1 ... 4294967295): "));
						scanf(_T("%s"), intStr);
						intDw = atoi(intStr);
						if (intDw > 0)
						{
							if (intDw < MAXDWORD)
							{
								g_updateRate = (DWORD)intDw;
								pClient->modifyUpdateRate();
								_tprintf(_T("\nNew update rate (%dms) successfuly set.\n"), g_updateRate);
							}
							else
							{
								_tprintf(_T("\nWARNING: Provided update rate (%d) is higher than 4294967295!!!\n"), intDw);
							}
						}
						else
						{
							_tprintf(_T("\nWARNING: Provided update rate (%d) is lower than 1!!!\n"), intDw);
						}
						g_syncDisplay.unlock();
						break;
					}
				case _T('-'):
					g_syncDisplay.lock();
					if (g_initialThreadCount > 1)
					{
						--g_initialThreadCount;
						TerminateThread(g_threads[g_initialThreadCount], 12345678);
						CloseHandle(g_threads[g_initialThreadCount]);
						g_threads[g_initialThreadCount] = 0;
						_tprintf(_T("\nNumber of threads decreased to: %d\n"), g_initialThreadCount);
					}
					else
					{
						_tprintf(_T("\nWARNING: Number of threads is already %d, cannot decrease anymore!!!\n"), g_initialThreadCount);
					}
					g_syncDisplay.unlock();
					break;
				case _T('1'):
					g_syncDisplay.lock();
					if (g_threadsStarted == false)
					{
						g_threadsStarted = true;
						_tprintf(_T("\nThreads started, R/W operations in progress\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Threads already started, cannot start again!!!\n"));
					}
					g_syncDisplay.unlock();
					break;
				case _T('0'):
					g_syncDisplay.lock();
					if (g_threadsStarted == true)
					{
						g_threadsStarted = false;
						_tprintf(_T("\nThreads stopped, R/W operations suspended\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Threads already stopped, cannot stop again!!!\n"));
					}
					g_syncDisplay.unlock();
					break;
				case _T('7'):
					g_syncDisplay.lock();
					if (g_allowItemRemoval == false)
					{
						g_allowItemRemoval = true;
						_tprintf(_T("\nItem removal/addition is now allowed.\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Item removal/addition already allowed, cannot allow again!!!\n"));
					}
					g_syncDisplay.unlock();
					break;
				case _T('9'):
					g_syncDisplay.lock();
					if (g_allowItemRemoval == true)
					{
						g_allowItemRemoval = false;
						_tprintf(_T("\nItem removal/addition is now DISallowed\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Item removal/addition already DISallowed, cannot DISallow again!!!\n"));
					}
					g_syncDisplay.unlock();
					break;
				case _T('&'):
					pClient->removeRandomItems();
					break;
				case _T('('):
					pClient->addRandomItems();
					break;
				case _T('/'):
					g_syncDisplay.lock();
					if (g_tracesOn == true)
					{
						g_tracesOn = false;
						_tprintf(_T("\nTraces disabled\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Traces already disabled, cannot mute again!!!\n"));
					}
					g_syncDisplay.unlock();
					break;
				case _T('*'):
					g_syncDisplay.lock();
					if (g_tracesOn == false)
					{
						g_tracesOn = true;
						_tprintf(_T("\nTraces enabled\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Traces already enabled, cannot unmute again!!!\n"));
					}
					g_syncDisplay.unlock();
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
	} // end while

	if (g_threadsStarted == true)
	{
		g_threadsStarted = false;
	}
	SetEvent(g_threadsStopEvent);
	if (g_threads[0] != 0)
	{
		WaitForMultipleObjects(g_initialThreadCount, g_threads, TRUE, INFINITE);
		for (int i = 0; i < g_initialThreadCount; ++i)
		{
			CloseHandle(g_threads[i]);
		}
	}

	WaitForSingleObject(g_threadNewLine, INFINITE);

	CloseHandle(g_threadsStopEvent);

	pClient->terminate();
	destroyOpcClient();
	return 0;
} // end main
