#include "stdafx.h"
#include "OpcServer.h"

#include <vector>
#include <iostream>

#ifdef TBC_OS_WINDOWS
#include <conio.h>
#include <crtdbg.h>
#endif

using namespace SoftingOPCToolboxServer;

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
#include <signal.h>
PosixEvents g_events;
void sigint(int sig);
#endif

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
		if (g_endEvent)
		{
			SetEvent(g_endEvent);
		}   //  end if

		return TRUE;
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	g_endEvent = CreateEvent(NULL, TRUE, FALSE, _T("EndEvent"));
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
			pServer->buildDaAddressSpace();
#ifdef TBC_OS_WINDOWS
			pServer->buildAeAddressSpace();
			pServer->buildEventCategories();
#endif
			pServer->ready();
			_tprintf(_T("Press Ctrl-C to exit!\n"));
			//  loop speciffic variables
			unsigned long waitTime = 100;
			unsigned long waitRet = 0;
			BOOL end = FALSE;
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
					//nothing to do
				}
			} // end while

#endif
#ifdef TBC_OS_WINDOWS
			DateTime now;
			unsigned short second   = 0xFF;
			unsigned short minute   = 0xFF;
			pServer->initializeConditions();
			tstring anActorID(_T("opcUser"));

			while (!end)
			{
				waitRet = WaitForSingleObject(g_endEvent, waitTime);

				if (waitRet == WAIT_OBJECT_0)
				{
					end = TRUE;
				}
				else
				{
					now.now();
					tstring message = _T("");
					tstring name = _T("");
					std::vector<Variant> eventAttributes;

					if (second != now.second())
					{
						second = now.second();

						switch (now.second())
						{
						case 10:
							message = _T("between [10,50]");
							name = tstring(_T(""));
							eventAttributes.push_back(Variant(now.second()));
							pServer->updateAECondition(
								510,
								message,
								TRUE,
								name,
								eventAttributes,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE, // active and not acked
								FirstCondition);
							message = tstring(_T("between [10,30]"));
							name = tstring(_T("[10,30]"));
							pServer->updateAECondition(
								500,
								message,
								TRUE,
								name,
								eventAttributes,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_SUBCONDITION |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE, // active and not acked
								SecondCondition);
							break;

						case 30:
							message = tstring(_T("between [30,50]"));
							name = tstring(_T("[30,50]"));
							eventAttributes.push_back(Variant(now.second()));
							pServer->updateAECondition(
								540,
								message,
								TRUE,
								name,
								eventAttributes,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_SUBCONDITION |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_ACTIVE, // active and not acked
								SecondCondition);
							break;

						case 50:
							message = _T("not between [10,50]");
							name = _T("");
							eventAttributes.push_back(Variant(now.second()));
							pServer->updateAECondition(
								1,
								message,
								FALSE,
								name,
								eventAttributes,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_NO, // not active
								FirstCondition);
							message = tstring(_T("not between [10,50]"));
							name = _T("");
							pServer->updateAECondition(
								1,
								message,
								FALSE,
								name,
								eventAttributes,
								EnumConditionChange_ACTIVE_STATE |
								EnumConditionChange_SEVERITY |
								EnumConditionChange_MESSAGE |
								EnumConditionChange_SUBCONDITION |
								EnumConditionChange_ATTRIBUTE,
								EnumConditionState_NO, // not active
								SecondCondition);
							break;

						default:
							break;
						}   //  end switch

						// change conditions
						pServer->changeConditions();
						// simple events
						pServer->handleSimpleEvents(second, minute, now);
					}

					if (_kbhit())
					{
						TCHAR key = _getch();
						pServer->handleKeyEvent(key);
					}   //  end if
				}   //  end if ... else
			}   //  end while

#endif
			pServer->stop();
		}
	}
	else
	{
		if (result == S_FALSE)
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
	} // end if .. else

	pServer->terminate();
	destroyOpcServer();
#ifdef TBC_OS_WINDOWS
	::CloseHandle(g_endEvent);
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
