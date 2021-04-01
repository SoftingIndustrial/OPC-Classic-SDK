#include "stdafx.h"
#include "OpcServer.h"

#ifdef TBC_OS_WINDOWS
#include <conio.h>
#include "crtdbg.h"
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
#include "PosixEvents.h"
#include <unistd.h>
#endif

BOOL g_simulationEnabled = TRUE;
unsigned int g_simulationThreadSleepInterval = 1000;

#ifdef TBC_OS_LINUX
int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

PosixEvents g_events;
void sigint(int sig);
pthread_t g_keyListenerThread;
pthread_mutex_t g_keyListenerMutex;
bool g_listenKey = false;

void* keyListenerThread(void* context)
{
	_TCHAR pressedKey;
	int i = 0;

	while (1)
	{
		usleep(1);
		i = kbhit();

		if (i != 0)
		{
			pressedKey = fgetc(stdin);
			OpcServer* pServer = getOpcServer();
			DemoDaAddressSpaceElement* keyElement = pServer->getKeyElement();

			if (keyElement)
			{
				DateTime now;
				now.now();
				Variant aVariant((unsigned short)pressedKey);
				ValueQT val(aVariant, EnumQuality_GOOD, now);
				keyElement->valueChanged(val);
			}
		}

		bool keepListening = false;
		pthread_mutex_lock(&g_keyListenerMutex);
		keepListening = g_listenKey;
		pthread_mutex_unlock(&g_keyListenerMutex);

		if (!keepListening)
		{
			break;
		}
	}

	pthread_mutex_destroy(&g_keyListenerMutex);
	pthread_exit(NULL);
	return NULL;
}

void startKeyListenerThread()
{
	if (!g_listenKey)
	{
		g_listenKey = true;
		pthread_mutex_init(&g_keyListenerMutex, NULL);
		pthread_create(&g_keyListenerThread, NULL, keyListenerThread, NULL);
	}
}

void stopKeyListenerThread()
{
	pthread_mutex_lock(&g_keyListenerMutex);
	g_listenKey = false;
	pthread_mutex_unlock(&g_keyListenerMutex);
}

#endif

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
BOOL g_simulationEnd = FALSE;
Mutex g_simulationEndMutex;

HANDLE g_simulationChanged = 0;

//AE events simulation
BOOL g_eventsAEEnd = FALSE;
Mutex g_eventsEndMutex;

tstring COND_NAME_BETWEEN_SINGLE    = _T("between [10,50]");
tstring COND_NAME_BETWEEN_MULTIPLE  = _T("between");

const unsigned long CAT_ID_TIME_TICK            = 1;
const unsigned long CAT_ID_KEY_HIT              = 2;
const unsigned long CAT_ID_TIME_SLOT            = 3;
const unsigned long CAT_ID_DA_WRITE_SIMPLE      = 4;
const unsigned long CAT_ID_DA_WRITE_TRACKING    = 5;

AeCategory* g_keyHit            = NULL;
AeCategory* g_timeTick          = NULL;
AeCategory* g_timeSlot          = NULL;
AeCategory* g_daSimpleWrite     = NULL;
AeCategory* g_daTrackingWrite   = NULL;

//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationDAThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	g_simulationEndMutex.lock();

	while (!g_simulationEnd)
	{
		//  simulate the DA Server data changes generation
		if (g_simulationEnabled)
		{
			pServer->changeSimulationValues();
		}

		WaitForSingleObject(g_simulationChanged, g_simulationThreadSleepInterval);
	}   //  end while

	g_simulationEndMutex.unlock();
	return 0;
}   //  end simulationThread
#endif

//-----------------------------------------------------------------------------
//	startSimulationThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startDASimulationThread(void)
{
#ifdef TBC_OS_WINDOWS
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &simulationDAThread, NULL, 0, &tID);
#endif
}   //  end startThread

#ifdef TBC_OS_WINDOWS

DWORD WINAPI simulationAEThread(void* context)
{
	g_eventsEndMutex.lock();
	DateTime now;
	unsigned short second   = 0xFF;
	unsigned short minute   = 0xFF;
	AeCondition* condition1 = new AeCondition();
	AeCondition* condition2 = new AeCondition();
	tstring sourcePath;
	condition1->setCategory(g_timeSlot);
	condition1->setName(COND_NAME_BETWEEN_SINGLE);
	condition1->setDefinition(g_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_SINGLE));
	sourcePath = _T("clock.time slot 1");
	condition1->setSourcePath(sourcePath);
	condition1->setQuality(EnumQuality_GOOD);
	condition1->setUserData(1);
	condition2->setCategory(g_timeSlot);
	condition2->setDefinition(g_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_MULTIPLE));
	condition2->setName(COND_NAME_BETWEEN_MULTIPLE);
	sourcePath = _T("clock.time slot 2");
	condition2->setSourcePath(sourcePath);
	condition2->setQuality(EnumQuality_GOOD);
	condition2->setUserData(2);
	tstring anActorID(_T("opcUser"));

	while (!g_eventsAEEnd)
	{
		now.now();

		if (second != now.second())
		{
			tstring message;
			tstring name;
			std::vector<AeCondition*> conditionList;
			std::vector<Variant> eventAttributes;

			switch (now.second())
			{
			case 10:
				eventAttributes.push_back(Variant(now.second()));
				condition1->setSeverity(510);
				message = _T("between [10,50]");
				condition1->setMessage(message);
				condition1->setAckRequired(TRUE);
				condition1->setEventAttributes(eventAttributes);
				condition1->setChangeMask(
					EnumConditionChange_ACTIVE_STATE |
					EnumConditionChange_SEVERITY |
					EnumConditionChange_MESSAGE |
					EnumConditionChange_ATTRIBUTE);
				condition1->setStateChange(EnumConditionState_ACTIVE); // active and not acked
				conditionList.push_back(condition1);
				condition2->setSeverity(500);
				message = tstring(_T("between [10,30]"));
				condition2->setMessage(message);
				condition2->setAckRequired(TRUE);
				name = tstring(_T("[10,30]"));
				condition2->setActiveSubConditionName(name);
				condition2->setEventAttributes(eventAttributes);
				condition2->setChangeMask(
					EnumConditionChange_ACTIVE_STATE |
					EnumConditionChange_SEVERITY |
					EnumConditionChange_MESSAGE |
					EnumConditionChange_SUBCONDITION |
					EnumConditionChange_ATTRIBUTE);
				condition2->setStateChange(EnumConditionState_ACTIVE);
				conditionList.push_back(condition2);
				break;

			case 30:
				eventAttributes.push_back(Variant(now.second()));
				condition2->setSeverity(540);
				message = tstring(_T("between [30,50]"));
				condition2->setMessage(message);
				condition2->setAckRequired(TRUE);
				name = tstring(_T("[30,50]"));
				condition2->setActiveSubConditionName(name);
				condition2->setEventAttributes(eventAttributes);
				condition2->setChangeMask(
					EnumConditionChange_ACTIVE_STATE |
					EnumConditionChange_SEVERITY |
					EnumConditionChange_MESSAGE |
					EnumConditionChange_SUBCONDITION |
					EnumConditionChange_ATTRIBUTE);
				condition2->setStateChange(EnumConditionState_ACTIVE); // active and not acked
				conditionList.push_back(condition2);
				break;

			case 50:
				eventAttributes.push_back(Variant(now.second()));
				condition1->setSeverity(1);
				message = _T("not between [10,50]");
				condition1->setMessage(message);
				condition1->setAckRequired(FALSE);
				condition1->setEventAttributes(eventAttributes);
				condition1->setChangeMask(
					EnumConditionChange_ACTIVE_STATE |
					EnumConditionChange_SEVERITY |
					EnumConditionChange_MESSAGE |
					EnumConditionChange_ATTRIBUTE);
				condition1->setStateChange(EnumConditionState_NO); // not active
				conditionList.push_back(condition1);
				condition2->setSeverity(1);
				message = tstring(_T("not between [10,50]"));
				condition2->setMessage(message);
				condition2->setAckRequired(FALSE);
				condition2->setEventAttributes(eventAttributes);
				condition2->setChangeMask(
					EnumConditionChange_ACTIVE_STATE |
					EnumConditionChange_SEVERITY |
					EnumConditionChange_MESSAGE |
					EnumConditionChange_SUBCONDITION |
					EnumConditionChange_ATTRIBUTE);
				condition2->setStateChange(EnumConditionState_NO); // not active
				conditionList.push_back(condition2);
				break;

			default:
				break;
			}   //  end switch

			if (conditionList.size() > 0)
			{
				getApp()->changeConditions(conditionList);
			}   //  end if

			// simple events
			if (second != now.second())
			{
				AeEvent secondEvent(EnumEventType_SIMPLE, tstring(_T("time.local")), CAT_ID_TIME_TICK);
				std::vector<Variant> attributeValues(3);
				attributeValues[0] = now.hour();
				attributeValues[1] = now.minute();
				attributeValues[2] = now.second();
				second = now.second();
				secondEvent.setSeverity(50);
				secondEvent.setActorId(anActorID);
				message = tstring(_T("Second Changed"));
				secondEvent.setMessage(message);
				secondEvent.setAttributeValueList(attributeValues);
				secondEvent.fire();
			}   //  end if

			if (minute != now.minute())
			{
				AeEvent minuteEvent(EnumEventType_SIMPLE, tstring(_T("time.local")), CAT_ID_TIME_TICK);
				std::vector<Variant> attributeValues(3);
				attributeValues[0] = now.hour();
				attributeValues[1] = now.minute();
				attributeValues[2] = now.second();
				minute = now.minute();
				minuteEvent.setSeverity(900);
				minuteEvent.setActorId(anActorID);
				message = tstring(_T("Minute Changed"));
				minuteEvent.setMessage(message);
				minuteEvent.setAttributeValueList(attributeValues);
				minuteEvent.fire();
			}   //  end if
		}   //  end if

		if (_kbhit())
		{
			TCHAR key = _getch();
			std::vector<Variant> attributeValues(1);
			attributeValues[0] = (unsigned short)key;
			tstring anActorID(_T("opcUser"));
			AeEvent keyEvent(EnumEventType_TRACKING, tstring(_T("keyboard.key")), CAT_ID_KEY_HIT);
			keyEvent.setSeverity(600);
			keyEvent.setActorId(anActorID);
			tstring message(_T("key hit: "));
			message += key;
			keyEvent.setMessage(message);
			keyEvent.setAttributeValueList(attributeValues);
			keyEvent.fire();
			{
				OpcServer* pServer = getOpcServer();
				DemoDaAddressSpaceElement* keyElement = pServer->getKeyElement();

				if (keyElement)
				{
					ValueQT val(Variant((unsigned short)key), EnumQuality_GOOD, now);
					keyElement->valueChanged(val);
				}
			}
		}   //  end if

		Sleep(500);
	}   //  end while

	if (condition1 != NULL)
	{
		delete condition1;
	}   //  end if

	if (condition2 != NULL)
	{
		delete condition2;
	}   //  end if

	if (g_timeTick != NULL)
	{
		delete g_timeTick;
	}   //  end if

	if (g_keyHit != NULL)
	{
		delete g_keyHit;
	}   //  end if

	if (g_timeSlot != NULL)
	{
		delete g_timeSlot;
	}   //  end if

	if (g_daSimpleWrite != NULL)
	{
		delete g_daSimpleWrite;
	}   //  end if

	if (g_daTrackingWrite != NULL)
	{
		delete g_daTrackingWrite;
	}   //  end if

	g_eventsEndMutex.unlock();
	return 0;
}   //  end eventsAEThread

void startAESimulationThread(void)
{
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &simulationAEThread, NULL, 0, &tID);
}   //  end startThread

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
#ifdef TBC_OS_LINUX
	g_events.create(false, false);
	(void) signal(SIGINT, sigint);
#endif
#ifdef TBC_OS_WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	tstring sysCommand(_T("title Softing OPC Toolkit Demo Server C++"));
	::_tsystem(sysCommand.c_str());
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	::SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
	//  create and initialize the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	pServer->initialize();
	//  provide the server with the proper custom creator
	DemoCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
#ifdef TBC_OS_WINDOWS
		CoUninitialize();
		CloseHandle(g_endEvent);
#endif
		delete pServer;
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
		CoUninitialize();
		CloseHandle(g_endEvent);
#endif
		delete pServer;
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
	else
	{
		pServer->trace(EnumTraceLevel_WRN, EnumTraceGroup_USER1, _T("_tWinMain"),
					   _T("Server start failed"));
		delete pServer;
		return -1;
	}

	//  start the simulation thread
	if (SUCCEEDED(result))
	{
#ifdef TBC_OS_WINDOWS
		g_simulationChanged = CreateEvent(NULL, FALSE, FALSE, NULL);
		startDASimulationThread();
		startAESimulationThread();
#endif
#ifdef TBC_OS_LINUX
		g_events.create(false, false);
		startKeyListenerThread();
#endif
	}   //  end if

	_tprintf(_T("Press Ctrl-C to exit\n"));
#ifdef TBC_OS_WINDOWS
	unsigned long waitRet = 0;
	BOOL end = FALSE;

	while (!end)
	{
		DWORD waitTime = 100;
		waitRet = WaitForSingleObject(g_endEvent, waitTime);

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
		}
		else
		{
			pServer->changeSimulationValues();
		}   //  end if...else
	}   //  ens while

	//  terminate the simulation
	g_simulationEnd = TRUE;
	g_simulationEndMutex.lock();
	g_simulationEndMutex.unlock();
	//for AE events
	g_eventsAEEnd = TRUE;
	g_eventsEndMutex.lock();
	g_eventsEndMutex.unlock();
#endif
#ifdef TBC_OS_LINUX

	while (1)
	{
		bool ret = g_events.waitFor(0, g_simulationThreadSleepInterval);

		if (!ret)
		{
			if (g_simulationEnabled)
			{
				pServer->changeSimulationValues();
			}
		}
		else
		{
			//stop the loop
			break;
		}
	}

	stopKeyListenerThread();
#endif
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	CloseHandle(g_simulationChanged);
	CloseHandle(g_endEvent);
	CoUninitialize();
#endif
	delete pServer;
	return 0;
}   //  end main

#ifdef TBC_OS_LINUX
void sigint(int sig)
{
	_tprintf(_T("You just pressed Ctrl-C\n"));
	g_events.signal(0);
}
#endif
