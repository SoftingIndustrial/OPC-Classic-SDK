//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                             OPC Toolkit - OTC                              |
//                                                                            |
//  Filename    : AECondition.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : How to use the AECondition list example                     |
//                                                                            |
//-----------------------------------------------------------------------------
// Console.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "OTClient.h"

HANDLE g_endEvent;

#define MAX_CONDITIONS 10
OTCEventData g_conditionList[MAX_CONDITIONS];

TCHAR g_objectDescription [2][64] =
{
	_T("Session       "),
	_T("Subscription  "),
};

TCHAR g_stateDescription [4][16] =
{
	_T("disconnected"),
	_T("connected"),
	_T(""),
	_T("active"),
};


//-----------------------------------------------------------------------------
// controlHandler
//
// - handle console control events
//
// returns:
//		TRUE  - handled event
//		FALSE - event not handled
//
BOOL WINAPI controlHandler(
	IN DWORD crtlType)  // control type
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
} // controlHandler


// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

// helper functions
LPTSTR GetDateTimeSZ(IN OTCDateTime dateTime);
void ClearConsole(void);
void ChangeCondition(IN DWORD idx, IN OTCEventData* pEvent);
void ClearCondition(IN DWORD idx);
void PrintConditionState(LPTSTR sourcePath, LPTSTR conditionName, OTCAEConditionStateData* pConditionState);

// callback functions
void OTCAPI_CALL OnStateChange(IN OTCObjectContext objectContext, IN BYTE state);
void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result);
void OTCAPI_CALL OnReceivedEvents(IN OTCObjectContext objectContect, IN DWORD count, IN OTCEventData* pEvents, BYTE refresh, BYTE lastRefresh);
void OTCAPI_CALL OnRefreshAEConditions(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result, IN BYTE cancelRefresh);
void OTCAPI_CALL OnAcknowledgeAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents, IN LONG* pResults);
void OTCAPI_CALL OnGetAEConditionState(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds, IN OTCAEConditionStateData* pConditionState);
void OTCAPI_CALL OnEnableAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path);

OTCCallbackFunctions g_callbackFunctions =
{
	NULL,
	&OnStateChange,
	NULL,
	NULL,
	NULL,
	&OnPerfromStateTransition,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&OnReceivedEvents,
	&OnRefreshAEConditions,
	&OnAcknowledgeAEConditions,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&OnGetAEConditionState,
	&OnEnableAEConditions,
};

void usage(void)
{
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("AECondition [<server URL>]\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to acknowledge all conditions\n"));
	_tprintf(_T("Press \'r\' or \'R\' to refresh the condition list\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get condition state of first condition\n"));
	_tprintf(_T("Press \'f\' or \'F\' to enable \"computer.clock.time slot 1\"\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disable \"computer.clock.time slot 1\"\n"));
	_tprintf(_T("Press \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n\n"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(controlHandler, TRUE);
	OTCExecutionOptions executionOptions;
	OTCObjectHandle sessionHandle;
	OTCObjectHandle subscriptionHandle;
	BOOL end = FALSE;
	DWORD waitRet;
	OTCObjectAttributes objectAttributes;
	DWORD i;
	LONG res;
	memset(&g_conditionList, 0, sizeof(OTCEventData) * MAX_CONDITIONS);
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
	executionOptions.m_executionContext = 0;
	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);

		if (argc == 1)
		{
			OTCAddAESession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"), 0, &sessionHandle);
		}
		else
		{
			OTCAddAESession(argv[1], 0, &sessionHandle);
		}

		OTCAddAESubscription(sessionHandle, 1, &subscriptionHandle);
		// filter simple and tracking events
		memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
		objectAttributes.m_aeSubscriptionFilterEventTypes = OT_EVENTTYPE_CONDITION;
		OTCSetAttributes(subscriptionHandle, OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER, &objectAttributes);
		OTCChangeTargetState(sessionHandle, OTC_STATE_ACTIVE, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			usage();
			OTCRefreshAEConditions(subscriptionHandle, FALSE, NULL);

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
						int c;
						c = _getch();

						switch (c)
						{
						case _T('A'):
						case _T('a'):
						{
							OTCEventData ackList[MAX_CONDITIONS];
							LONG results[MAX_CONDITIONS];
							DWORD ackCount = 0;

							for (i = 0; i < MAX_CONDITIONS; i++)
							{
								if (g_conditionList[i].m_eventType != 0)
								{
									if (!(g_conditionList[i].m_newState & OT_CONDITIONSTATE_ACKED))
									{
										ackList[ackCount].m_sourcePath = _tcsdup(g_conditionList[i].m_sourcePath);
										ackList[ackCount].m_conditionName = _tcsdup(g_conditionList[i].m_conditionName);
										ackList[ackCount].m_activeTime = g_conditionList[i].m_activeTime;
										ackList[ackCount].m_cookie = g_conditionList[i].m_cookie;
										ackCount++;
									}
								}
							}

							if (c == _T('a'))
							{
								res = OTCAcknowledgeAEConditions(sessionHandle, _T("AECondition"),  _T("a"), ackCount, ackList, results, NULL);
							}
							else
							{
								res = OTCAcknowledgeAEConditions(sessionHandle, _T("AECondition"),  _T("a"), ackCount, ackList, results, &executionOptions);
								executionOptions.m_executionContext++;
							}

							for (i = 0; i < ackCount; i++)
							{
								free(ackList[i].m_sourcePath);
								free(ackList[i].m_conditionName);
							}
						}
						break;

						case _T('R'):
							res = OTCRefreshAEConditions(subscriptionHandle, FALSE, &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('r'):
							res = OTCRefreshAEConditions(subscriptionHandle, FALSE, NULL);

							if (FAILED(res))
							{
								_tprintf(_T("Refresh failed [0x%8.8X]\n"), res);
							}

							break;

						case _T('S'):
							for (i = 0; i < MAX_CONDITIONS; i++)
							{
								if (g_conditionList[i].m_eventType != 0)
								{
									break;
								}
							}

							if (i < MAX_CONDITIONS)
							{
								res = OTCGetAEConditionState(sessionHandle, g_conditionList[i].m_sourcePath, g_conditionList[i].m_conditionName, 0, NULL, NULL, &executionOptions);
								executionOptions.m_executionContext++;
							}

							break;

						case _T('s'):
							for (i = 0; i < MAX_CONDITIONS; i++)
							{
								if (g_conditionList[i].m_eventType != 0)
								{
									break;
								}
							}

							if (i < MAX_CONDITIONS)
							{
								OTCAEConditionStateData conditionState;
								res = OTCGetAEConditionState(sessionHandle, g_conditionList[i].m_sourcePath, g_conditionList[i].m_conditionName, 0, NULL, &conditionState, NULL);

								if (SUCCEEDED(res))
								{
									PrintConditionState(g_conditionList[i].m_sourcePath, g_conditionList[i].m_conditionName, &conditionState);
									OTFreeMemory(conditionState.m_actSubConditionName);
									OTFreeMemory(conditionState.m_actSubConditionDescription);
									OTFreeMemory(conditionState.m_actSubConditionDefinition);
									OTFreeMemory(conditionState.m_ackID);
									OTFreeMemory(conditionState.m_ackComment);

									if (conditionState.m_subConditionCount > 0)
									{
										for (i = 0; i < conditionState.m_subConditionCount; i++)
										{
											OTFreeMemory(conditionState.m_pSubConditionDefinition[i]);
											OTFreeMemory(conditionState.m_pSubConditionDescription[i]);
											OTFreeMemory(conditionState.m_pSubConditionName[i]);
										}

										OTFreeMemory(conditionState.m_pSubConditionDefinition);
										OTFreeMemory(conditionState.m_pSubConditionSeverity);
										OTFreeMemory(conditionState.m_pSubConditionDescription);
										OTFreeMemory(conditionState.m_pSubConditionName);
									}

									if (conditionState.m_eventAttrCount > 0)
									{
										for (i = 0; i < conditionState.m_eventAttrCount; i++)
										{
											VariantClear(&conditionState.m_pEventAttrs[i]);
										}

										OTFreeMemory(conditionState.m_pAttrErrors);
										OTFreeMemory(conditionState.m_pEventAttrs);
									}
								}
								else
								{
									_tprintf(_T("Refresh failed [0x%8.8X]\n"), res);
								}
							}

							break;

						case _T('F'):
							res = OTCEnableAEConditions(sessionHandle, TRUE, FALSE, _T("computer.clock.time slot 1"), &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('f'):
							res = OTCEnableAEConditions(sessionHandle, TRUE, FALSE, _T("computer.clock.time slot 1"), NULL);

							if (FAILED(res))
							{
								_tprintf(_T("Enable failed [0x%8.8X]\n"), res);
							}

							break;

						case _T('D'):
							res = OTCEnableAEConditions(sessionHandle, FALSE, FALSE, _T("computer.clock.time slot 1"), &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('d'):
							res = OTCEnableAEConditions(sessionHandle, FALSE, FALSE, _T("computer.clock.time slot 1"), NULL);

							if (FAILED(res))
							{
								_tprintf(_T("Disable failed [0x%8.8X]\n"), res);
							}

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
							break;
						}
					}
				}
			}
		}

		OTCTerminate();
	}

	for (i = 0; i < MAX_CONDITIONS; i++)
	{
		ClearCondition(i);
	}

	return 0;
}

void OTCAPI_CALL OnStateChange(IN OTCObjectContext objectContext, IN BYTE state)
{
	_tprintf(_T("%s state changed - %s\n"), g_objectDescription[objectContext.m_objectData.m_userData], g_stateDescription[state]);
}

void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result)
{
	_tprintf(_T("%s performed state transition - result 0x%8.8X - context: %lu\n"), g_objectDescription[objectContext.m_objectData.m_userData], result, executionContext);
}

void OTCAPI_CALL OnRefreshAEConditions(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result, IN BYTE cancelRefresh)
{
	_tprintf(_T("%s refreshed AE conditions - result 0x%8.8X - context: %lu\n"), g_objectDescription[objectContext.m_objectData.m_userData], result, executionContext);
}

void OTCAPI_CALL OnAcknowledgeAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents, IN LONG* pResults)
{
	_tprintf(_T("%s acknowledged AE conditions - result 0x%8.8X - context: %lu\n"), g_objectDescription[sessionData.m_userData], result, executionContext);
}

void OTCAPI_CALL OnGetAEConditionState(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds, IN OTCAEConditionStateData* pConditionState)
{
	if (SUCCEEDED(result))
	{
		PrintConditionState(sourcePath, conditionName, pConditionState);
	}
	else
	{
		_tprintf(_T("Refresh failed [0x%8.8X]\n"), result);
	}
}

void OTCAPI_CALL OnEnableAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path)
{
	if (FAILED(result))
	{
		if (enable)
		{
			_tprintf(_T("Enable failed [0x%8.8X]\n"), result);
		}
		else
		{
			_tprintf(_T("Disable failed [0x%8.8X]\n"), result);
		}
	}
}


void ClearCondition(IN DWORD idx)
{
	free(g_conditionList[idx].m_sourcePath);
	free(g_conditionList[idx].m_conditionName);
	free(g_conditionList[idx].m_subConditionName);
	free(g_conditionList[idx].m_message);
	memset(&g_conditionList[idx], 0, sizeof(OTCEventData));
}

void ChangeCondition(IN DWORD idx, IN OTCEventData* pEvent)
{
	if (g_conditionList[idx].m_eventType == 0)
	{
		if ((!(pEvent->m_newState & OT_CONDITIONSTATE_ACTIVE)) &&  // not active and
			(pEvent->m_newState & OT_CONDITIONSTATE_ACKED))       // acknowledged
		{
			return;
		}

		// new condition
		g_conditionList[idx].m_eventType = pEvent->m_eventType;
		g_conditionList[idx].m_newState = pEvent->m_newState;
		g_conditionList[idx].m_sourcePath = _tcsdup(pEvent->m_sourcePath);
		g_conditionList[idx].m_conditionName = _tcsdup(pEvent->m_conditionName);
		g_conditionList[idx].m_subConditionName = _tcsdup(pEvent->m_subConditionName);
		g_conditionList[idx].m_occurenceTime = pEvent->m_occurenceTime;
		g_conditionList[idx].m_severity = pEvent->m_severity;
		g_conditionList[idx].m_message = _tcsdup(pEvent->m_message);
		g_conditionList[idx].m_activeTime = pEvent->m_activeTime;
		g_conditionList[idx].m_cookie = pEvent->m_cookie;
	}
	else
	{
		if ((!(pEvent->m_newState & OT_CONDITIONSTATE_ACTIVE)) &&  // not active and
			(pEvent->m_newState & OT_CONDITIONSTATE_ACKED))       // acknowledged
		{
			ClearCondition(idx); // remove
		}
		else
		{
			// set the changed values
			free(g_conditionList[idx].m_subConditionName);
			free(g_conditionList[idx].m_message);
			g_conditionList[idx].m_newState = pEvent->m_newState;
			g_conditionList[idx].m_subConditionName = _tcsdup(pEvent->m_subConditionName);
			g_conditionList[idx].m_occurenceTime = pEvent->m_occurenceTime;
			g_conditionList[idx].m_severity = pEvent->m_severity;
			g_conditionList[idx].m_message = _tcsdup(pEvent->m_message);
			g_conditionList[idx].m_activeTime = pEvent->m_activeTime;
			g_conditionList[idx].m_cookie = pEvent->m_cookie;
		}
	}
}

void OTCAPI_CALL OnReceivedEvents(IN OTCObjectContext objectContext, IN DWORD count, IN OTCEventData* pEvents, BYTE refresh, BYTE lastRefresh)
{
	ClearConsole();
	DWORD i, j;
	DWORD freeCondListIdx;

	for (i = 0; i < count; i++)
	{
		freeCondListIdx = MAX_CONDITIONS;

		// find condition in condition list
		for (j = 0; j < MAX_CONDITIONS; j++)
		{
			if (g_conditionList[j].m_eventType != 0)
			{
				if ((_tcscmp(g_conditionList[j].m_conditionName, pEvents[i].m_conditionName) == 0) &&
					(_tcscmp(g_conditionList[j].m_sourcePath, pEvents[i].m_sourcePath) == 0))
				{
					// found condition
					ChangeCondition(j, &pEvents[i]);
					break;
				}
			}
			else
			{
				if (freeCondListIdx == MAX_CONDITIONS)
				{
					freeCondListIdx = j;
				}
			}
		}

		if ((j == MAX_CONDITIONS) && (freeCondListIdx != MAX_CONDITIONS))
		{
			// not found condition -> add it
			ChangeCondition(freeCondListIdx, &pEvents[i]);
		}
	}

	TCHAR state[12];
	_tprintf(_T("\nACTIVE and NOT ACKNOWLEDGED CONDITIONS\n\n"));

	for (j = 0; j < MAX_CONDITIONS; j++)
	{
		if (g_conditionList[j].m_eventType != 0)
		{
			_tcscpy(state, _T("           "));

			if (g_conditionList[j].m_newState & OT_CONDITIONSTATE_ACTIVE)
			{
				_tcsncpy(state + 0, _T("ACT"), 3);
			}

			if (g_conditionList[j].m_newState & OT_CONDITIONSTATE_ACKED)
			{
				_tcsncpy(state + 4, _T("ACK"), 3);
			}

			if (g_conditionList[j].m_newState & OT_CONDITIONSTATE_ENABLED)
			{
				_tcsncpy(state + 8, _T("ENA"), 3);
			}

			_tprintf(_T("%s # %-30.30s # %-15.15s # %-15.15s # %s # %-4.4u # %s\n"), state, g_conditionList[j].m_sourcePath, g_conditionList[j].m_conditionName, g_conditionList[j].m_subConditionName, GetDateTimeSZ(g_conditionList[j].m_occurenceTime), g_conditionList[j].m_severity, g_conditionList[j].m_message);
		}
	}

	_tprintf(_T("\n\n"));
	usage();
}

void PrintConditionState(LPTSTR sourcePath, LPTSTR conditionName, OTCAEConditionStateData* pConditionState)
{
	DWORD i;
	_tprintf(_T("CONDITION STATE\n"));
	_tprintf(_T("Source Path: %s  Condition Name: %s\n"), sourcePath, conditionName);
	_tprintf(_T("  State: %u\n"), (WORD)pConditionState->m_state);
	_tprintf(_T("  Quality: %u\n"), pConditionState->m_quality);
	_tprintf(_T("  Active time: %s\n"), GetDateTimeSZ(pConditionState->m_conditionActiveTime));
	_tprintf(_T("  Inactive time: %s\n"), GetDateTimeSZ(pConditionState->m_conditionInactiveTime));
	_tprintf(_T("  Ack time: %s\n"), GetDateTimeSZ(pConditionState->m_conditionAckTime));
	_tprintf(_T("  Ack id: %s\n"), pConditionState->m_ackID);
	_tprintf(_T("  Ack comment: %s\n"), pConditionState->m_ackComment);
	_tprintf(_T("  Active subcondition time: %s\n"), GetDateTimeSZ(pConditionState->m_subConditionActiveTime));
	_tprintf(_T("  Active subcondition name: %s\n"), pConditionState->m_actSubConditionName);
	_tprintf(_T("  Active subcondition definition: %s\n"), pConditionState->m_actSubConditionDefinition);
	_tprintf(_T("  Active subcondition description: %s\n"), pConditionState->m_actSubConditionDescription);
	_tprintf(_T("  Active subcondition severity: %lu\n"), pConditionState->m_actSubConditionSeverity);
	_tprintf(_T("  Number of subconditions: %lu\n"), pConditionState->m_subConditionCount);

	for (i = 0; i < pConditionState->m_subConditionCount; i++)
	{
		_tprintf(_T("    [%3.3d] subcondition name: %s\n"), i, pConditionState->m_pSubConditionName[i]);
		_tprintf(_T("         subcondition definition: %s\n"), pConditionState->m_pSubConditionDefinition[i]);
		_tprintf(_T("         subcondition description: %s\n"), pConditionState->m_pSubConditionDescription[i]);
		_tprintf(_T("         subcondition severity: %lu\n"), pConditionState->m_pSubConditionSeverity[i]);
	}
}

TCHAR g_dateTimeSZ[256];

LPTSTR GetDateTimeSZ(IN OTCDateTime dateTime)
{
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&dateTime, &sysTime);
	_stprintf(g_dateTimeSZ, _T("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d.%3.3d"), sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	return g_dateTimeSZ;
}


void ClearConsole(void)
{
	HANDLE hConsoleHandle;
	DWORD hWrittenChars = 0;
	CONSOLE_SCREEN_BUFFER_INFO strConsoleInfo;
	COORD Home;
	hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	Home.X = 0;
	Home.Y = 0;
	GetConsoleScreenBufferInfo(hConsoleHandle, &strConsoleInfo);
	FillConsoleOutputCharacter(hConsoleHandle, 32, strConsoleInfo.dwSize.X * strConsoleInfo.dwSize.Y, Home, &hWrittenChars);
	SetConsoleCursorPosition(hConsoleHandle, Home);
}
