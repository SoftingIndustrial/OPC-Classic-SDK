#include "stdafx.h"
#include "OpcServer.h"
#include <conio.h>
#include <time.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace SoftingOPCToolboxServer;
using namespace std::chrono;

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
HANDLE g_threadStopEvent = 0;
HANDLE g_thread = 0;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
PosixEvents g_events;
void sigint(int sig);
#endif

unsigned int getNoOfDigits(const unsigned int& val)
{
	unsigned int ret = 0;
	unsigned int i = val;
	ret = 0;
	while (i > 0)
	{
		++ret;
		i /= 10;
	}
	return ret;
}

/**
 * TODO: Update these settings as required
 */
const TCHAR g_folderFullPath[256] = _T("objectBasedNode.test.usace.army.mil");
const TCHAR g_folderName[256] = _T("itemFolder_"); // contained below
const TCHAR g_folderNameFormat[16] = _T("%s%05d"); // itemFolder_00001 ...
const unsigned int g_foldersCount = 2000;
const TCHAR g_itemName[256] = _T("namedItem_"); // contained below
const TCHAR g_itemNameFormat[16] = _T("%s%05d"); // namedItem_00001
const unsigned int g_itemsTotalCount = 50;

const unsigned short g_minStringLength = 150;
const unsigned short g_maxStringLength = 250;

const std::string g_stringBasedNodeName = _T("stringBasedNode.test.usace.army.mil");
const unsigned int g_stringBasedItemCount = 50;
const TCHAR g_stringBasedItemName[256] = _T("stringItem_");
const TCHAR g_stringBasedItemNameFormat[16] = _T("%s%06d"); // the "d" or "x" has effects below (when querying)
const byte g_stringBasedItemNameFormatRadix = 10; // must be same as row above (e.g. 10 for %??d and 16 for %??x)
/**
 * END TODO
 */

/**
 * NOTE: These settings are computed once based on the above input data, but need to be global.
 *       DO NOT CHANGE ANY VALUES FROM THIS SECTION!!!
 */
const unsigned int g_stringBasedItemDigits = getNoOfDigits(g_stringBasedItemCount);
TCHAR g_namespaceDelimiter[2] = { 0 };
/**
 * END NOTE
 */

/**
 * NOTE: These settings are modified through console input at run time, but need to be global.
 *       You may set different values here if you desire.
 */
bool g_enableDataChanges = false;
DWORD g_itemChangeRateMilliseconds = 1000;
/**
 * END NOTE
 */

const unsigned int g_qualities[17] = {
	EnumQuality_GOOD,
	EnumQuality_GOOD_LOCAL_OVERRIDE,
	EnumQuality_BAD,
	EnumQuality_BAD_CONFIG_ERROR,
	EnumQuality_BAD_NOT_CONNECTED,
	EnumQuality_BAD_DEVICE_FAILURE,
	EnumQuality_BAD_SENSOR_FAILURE,
	EnumQuality_BAD_LAST_KNOWN,
	EnumQuality_BAD_COMM_FAILURE,
	EnumQuality_BAD_OUT_OF_SERVICE,
	EnumQuality_BAD_WAITING_FOR_INITIAL_DATA,
	EnumQuality_UNCERTAIN,
	EnumQuality_UNCERTAIN_LAST_USABLE,
	EnumQuality_UNCERTAIN_SENSOR_CAL,
	EnumQuality_UNCERTAIN_EGU_EXCEEDED,
	EnumQuality_UNCERTAIN_SUB_NORMAL,
	EnumQuality_QUALITY_NOT_SET
};

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

void usage(void)
{
	g_syncDisplay.lock();
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("\nPress \'1\' to enable data changes\n"));
	_tprintf(_T("Press \'0\' to disable data changes\n\t - data change thread is %s -\n"), g_enableDataChanges == true ? _T("started") : _T("stopped"));
	_tprintf(_T("\nPress \'/\' to manually set the data change rate interval\n"));
	_tprintf(_T("Press \'+\' to increase data change rate interval\n"));
	_tprintf(_T("Press \'-\' to decrease data change rate interval (min 1 millisecond)\n\t - data change interval is %d -\n"), g_itemChangeRateMilliseconds);
	_tprintf(_T("\nPress \'*\' to fire 1 data change\n"));
	_tprintf(_T("Press \'`\' or \'~\' to reset values to 0\n"));
	_tprintf(_T("Press \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n"));
	g_syncDisplay.unlock();
}

DWORD WorkerThreadEntryPoint(LPVOID param)
{
	DWORD currentWaitTime = 0;
	unsigned long long start, end;
	while (true)
	{
		if (g_enableDataChanges == true)
		{
			if (currentWaitTime >= g_itemChangeRateMilliseconds)
			{
				start = GetTickCount64();
				getOpcServer()->fireDataChanges();
				end = GetTickCount64();
				end -= start;
				if (g_syncDisplay.lock(0) == TRUE)
				{
					printf(_T("vc%d+%d "), g_itemChangeRateMilliseconds, end);
					g_syncDisplay.unlock();
				}
				currentWaitTime = 0;
			}
			else
			{
				++currentWaitTime;
				::Sleep(1);
			}
		}
		else
		{
			::Sleep(100);
		}
		if (WaitForSingleObject(g_threadStopEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//_crtBreakAlloc = 8656;
	srand((unsigned int)time(NULL));
	unsigned long result = S_OK;
#ifdef TBC_OS_WINDOWS
	g_endEvent = CreateEvent(NULL, TRUE, FALSE, _T("EndEvent"));
	g_threadStopEvent = CreateEvent(NULL, FALSE, FALSE, _T("ThreadStopEvent"));
	SetConsoleCtrlHandler(controlHandler, TRUE);
#endif
#ifdef TBC_OS_LINUX
	(void) signal(SIGINT, sigint);
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
		::CloseHandle(g_threadStopEvent);
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
			auto tStart = system_clock::now();
			std::time_t timeType;
			timeType = system_clock::to_time_t(tStart);
			std::cout << ctime(&timeType) << " - Build Namespace >>>\n";

			//  build the namespace
			pServer->buildAddressSpace();

			auto tEnd = system_clock::now();
			auto delta = tEnd - tStart;
			timeType = system_clock::to_time_t(tEnd);
			std::cout << ctime(&timeType) << " - <<< Build Namespace (" << duration_cast<milliseconds>(delta).count() << "ms)\n";


			//  declare the namespace built and the server ready for clients to connect
			pServer->ready();
			usage();

			BOOL end = FALSE;
#ifdef TBC_OS_WINDOWS
			unsigned long waitRet = 0;

			DWORD threadId;
			g_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WorkerThreadEntryPoint, NULL, 0, &threadId);

			while (!end)
			{
				waitRet = WaitForSingleObject(g_endEvent, 100);

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
						case _T('?'):
						case _T('u'):
						case _T('U'):
							g_syncDisplay.lock();
							usage();
							g_syncDisplay.unlock();
							break;
						case _T('*'):
							g_syncDisplay.lock();
							printf(_T(" * vc *\n"));
							pServer->fireDataChanges();
							g_syncDisplay.unlock();
							break;
						case _T('`'):
						case _T('~'):
							g_syncDisplay.lock();
							printf(_T(" * cv *\n"));
							pServer->emptyValues();
							g_syncDisplay.unlock();
							break;
						case _T('+'):
							g_syncDisplay.lock();
							if (g_itemChangeRateMilliseconds < MAXDWORD - 10)
							{
								g_itemChangeRateMilliseconds += 10;
								printf(_T("Data change rate interval increased to: %d\n"), g_itemChangeRateMilliseconds);
							}
							else
							{
								printf(_T("WARNING: Data change rate interval is %d, cannot increase anymore (10 units)!!!\n"), g_itemChangeRateMilliseconds);
							}
							g_syncDisplay.unlock();
							break;
						case _T('/'):
							{
								g_syncDisplay.lock();
								char intStr[256];
								long long intDw = 0;
								printf(_T("New data change rate interval in milliseconds: [1 ... 4294967295): "));
								scanf(_T("%s"), intStr);
								intDw = atoi(intStr);
								if (intDw >= 0)
								{
									if (intDw < MAXDWORD)
									{
										g_itemChangeRateMilliseconds = (DWORD)intDw;
										printf(_T("New data change rate interval (%d) successfuly set.\n"), g_itemChangeRateMilliseconds);
									}
									else
									{
										printf(_T("WARNING: Provided data change rate interval (%d) is higher than 4294967295!!!\n"), intDw);
									}
								}
								else
								{
									printf(_T("WARNING: Provided data change rate interval (%d) is lower than 0!!!\n"), intDw);
								}
								g_syncDisplay.unlock();
							}
							break;
						case _T('-'):
							g_syncDisplay.lock();
							if (g_itemChangeRateMilliseconds > 10)
							{
								g_itemChangeRateMilliseconds -= 10;
								printf(_T("Data change rate interval decreased to: %d\n"), g_itemChangeRateMilliseconds);
							}
							else
							{
								printf(_T("WARNING: Data change rate interval is %d, cannot decrease anymore!!!\n"), g_itemChangeRateMilliseconds);
							}
							g_syncDisplay.unlock();
							break;
						case _T('1'):
							g_syncDisplay.lock();
							if (g_enableDataChanges == false)
							{
								g_enableDataChanges = true;
								printf(_T("Data change thread started\n"));
							}
							else
							{
								printf(_T("WARNING: Data change thread already started, cannot start again!!!\n"));
							}
							g_syncDisplay.unlock();
							break;
						case _T('0'):
							g_syncDisplay.lock();
							if (g_enableDataChanges == true)
							{
								g_enableDataChanges = false;
								printf(_T("Data change thread stopped\n"));
							}
							else
							{
								printf(_T("WARNING: Data change thread already stopped, cannot stop again!!!\n"));
							}
							g_syncDisplay.unlock();
							break;
						case _T('E'):
						case _T('e'):
						case _T('Q'):
						case _T('q'):
							end = TRUE;
							break;
						}
					}
				}
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

	if (g_enableDataChanges == true)
	{
		g_enableDataChanges = false;
	}
	SetEvent(g_threadStopEvent);
	if (g_thread != 0)
	{
		WaitForSingleObject(g_thread, INFINITE);
		CloseHandle(g_thread);
	}

	pServer->terminate();
	destroyOpcServer();
	releaseApp();
#ifdef TBC_OS_WINDOWS
	::CloseHandle(g_endEvent);
	::CloseHandle(g_threadStopEvent);
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
