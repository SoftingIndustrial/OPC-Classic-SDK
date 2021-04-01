#include "stdafx.h"
#include "OpcServer.h"
#include <iostream>

using namespace SoftingOPCToolboxServer;

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
PosixEvents g_events;
void sigint(int sig);
#endif

unsigned long g_angle = 0;


//-----------------------------------------------------------------------------
// controlHandler
//
// - handle console control events
//
// returns:
//		TRUE  - handled event
//		FALSE - event not handled
//
#ifdef TBC_OS_WINDOWS
BOOL WINAPI ControlHandler(IN unsigned long crtlType)   // control type
{
	switch (crtlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to end server
	case CTRL_C_EVENT:
	{
		if (g_endEvent)
		{
			SetEvent(g_endEvent);
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
			pServer->buildDaNameSpace();
#ifdef TBC_OS_WINDOWS
			pServer->buildAeNameSpace();
			pServer->buildEventCategories();
#endif
			//  declare the namespaces built and the server ready for clients to connect
			pServer->ready();
			DateTime now;
			unsigned short second   = 0xFF;
#ifdef TBC_OS_WINDOWS
			unsigned short minute   = 0xFF;
			unsigned short hour = 0xFF;
			bool firstLoop = true;
			pServer->initializeConditions();
#endif
			_tprintf(_T("Press Ctrl-C to exit\n"));
			//  loop speciffic variables
			unsigned long waitTime = 100;
			BOOL end = FALSE;
			unsigned long waitRet = 0;
#ifdef TBC_OS_WINDOWS

			while (!end)
			{
				waitRet = WaitForSingleObject(g_endEvent, waitTime);

				if (waitRet == WAIT_OBJECT_0)
				{
					end = TRUE;
				}
				else
				{
#endif
#ifdef TBC_OS_LINUX

			while (!end)
			{
				waitRet = g_events.waitForAll(false, waitTime);

				if (waitRet == 0)
				{
					end = TRUE;
				}
				else
				{
#endif
					now.now();
					Variant aVariant(rand());
					pServer->changeValue(aVariant, Rand100);

					if (second != now.second())
					{
						pServer->changeValue(aVariant, Rand1);
						g_angle++;
						g_angle = (g_angle % 360);
						aVariant = Variant(sin(g_angle / 180.0 * 3.14));
						pServer->changeValue(aVariant, Sin);
					}   //  end if

#ifdef TBC_OS_WINDOWS
					tstring aMessage = _T("");
					tstring aName = _T("");

					if (!firstLoop)
					{
						if (second != now.second())
						{
							switch (now.second())
							{
							case 10:
								aMessage = tstring(_T("between [10,30]"));
								aName = tstring(_T("[10,30]"));
								pServer->updateAECondition(
									500,
									aMessage,
									TRUE,
									aName,
									EnumConditionChange_ACTIVE_STATE |
									EnumConditionChange_SEVERITY |
									EnumConditionChange_MESSAGE |
									EnumConditionChange_SUBCONDITION |
									EnumConditionChange_ATTRIBUTE,
									EnumConditionState_ACTIVE, // active and not acked
									FirstCondition);
								aMessage = tstring(_T("between [10,50]"));
								aName = tstring(_T(""));
								pServer->updateAECondition(
									510,
									aMessage,
									TRUE,
									aName,
									EnumConditionChange_ACTIVE_STATE |
									EnumConditionChange_SEVERITY |
									EnumConditionChange_MESSAGE |
									EnumConditionChange_ATTRIBUTE,
									EnumConditionState_ACTIVE, // active and not acked
									SecondCondition);
								break;

							case 30:
								aMessage = tstring(_T("between [30,50]"));
								aName = tstring(_T("[30,50]"));
								pServer->updateAECondition(
									540,
									aMessage,
									TRUE,
									aName,
									EnumConditionChange_ACTIVE_STATE |
									EnumConditionChange_SEVERITY |
									EnumConditionChange_MESSAGE |
									EnumConditionChange_SUBCONDITION |
									EnumConditionChange_ATTRIBUTE,
									EnumConditionState_ACTIVE, // active and not acked
									FirstCondition);
								break;

							case 50:
								aMessage = tstring(_T("not between [10,50]"));
								aName = tstring(_T(""));
								pServer->updateAECondition(
									1,
									aMessage,
									FALSE,
									aName,
									EnumConditionChange_ACTIVE_STATE |
									EnumConditionChange_SEVERITY |
									EnumConditionChange_MESSAGE |
									EnumConditionChange_SUBCONDITION |
									EnumConditionChange_ATTRIBUTE,
									EnumConditionState_NO,  // not active
									FirstCondition);
								aMessage = tstring(_T("not between [10,50]"));
								pServer->updateAECondition(
									1,
									aMessage,
									FALSE,
									aName,
									EnumConditionChange_ACTIVE_STATE |
									EnumConditionChange_SEVERITY |
									EnumConditionChange_MESSAGE |
									EnumConditionChange_ATTRIBUTE,
									EnumConditionState_NO,  // not active
									SecondCondition);
								break;
							}   //  end switch
						}
					}
					else
					{
						firstLoop = false;

						if ((now.second() >= 10) && (now.second() < 30))
						{
							aMessage = tstring(_T("between [30,50]"));
							aName = tstring(_T("[30,50]"));
							pServer->updateAECondition(
								540,
								aMessage,
								TRUE,
								aName,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_SUBCONDITION |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE,  // active and not acked
								FirstCondition);
							aMessage = tstring(_T("between [10,50]"));
							aName = tstring(_T(""));
							pServer->updateAECondition(
								510,
								aMessage,
								TRUE,
								aName,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE,  // active and not acked
								SecondCondition);
						}   //  end if

						if ((now.second() >= 30) && (now.second() < 50))
						{
							aMessage = tstring(_T("between [30,50]"));
							aName = tstring(_T(""));
							pServer->updateAECondition(
								540,
								aMessage,
								TRUE,
								aName,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_SUBCONDITION |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE,  // active and not acked
								FirstCondition);
							aMessage = tstring(_T("between [10,50]"));
							pServer->updateAECondition(
								510,
								aMessage,
								TRUE,
								aName,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE,  // active and not acked
								SecondCondition);
						}   //  end if
					}   //  end if ... else

					pServer->changeConditions();
					pServer->clearConditionList();
					// simple events
					pServer->handleSimpleEvents(
						second,
						minute,
						hour,
						now);
#endif
				}   //  end if ... else
			}   //  end while

			pServer->stop();
		}   //  end if
	}
	else if (result == S_FALSE)
	{
#ifdef TBC_OS_WINDOWS
		_tprintf(_T("Registration operation %s"), (LPCTSTR)GetCommandLine());
#endif
#ifdef TBC_OS_LINUX
		_tprintf(_T("Registration operation %s"), commandLine.c_str());
#endif
	}
	else
	{
		_tprintf(_T("processCommandLine has failed"));
	}   //  end if ... else

	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	CloseHandle(g_endEvent);
#endif
	releaseApp();
	return 0;
}   //  end main

#ifdef TBC_OS_LINUX
void sigint(int sig)
{
	_tprintf(_T("You just pressed Ctrl-C\n"));
	g_events.signal(0);
}
#endif
