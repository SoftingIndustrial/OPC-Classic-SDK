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
//  Filename    : AEConsole.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : How to use the AeSession and AeSubscription objects example |
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
LPTSTR GetValueSZ(IN OTCVariant* pVariant);
LPTSTR GetDateTimeSZ(IN OTCDateTime dateTime);
void PrintServerStatus(OTCServerStatus* pServerStatus);

// callback functions
void OTCAPI_CALL OnStateChange(IN OTCObjectContext objectContext, IN BYTE state);
BYTE OTCAPI_CALL OnServerShutdown(IN OTCObjectData serverData, IN LPTSTR reason);
void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result);
void OTCAPI_CALL OnGetServerStatus(IN DWORD executionContext, IN OTCObjectData serverData, IN LONG result, IN OTCServerStatus* pServerStatus);
void OTCAPI_CALL OnReceivedEvents(IN OTCObjectContext objectContect, IN DWORD count, IN OTCEventData* pEvents, BYTE refresh, BYTE lastRefresh);

OTCCallbackFunctions g_callbackFunctions =
{
	NULL,
	&OnStateChange,
	NULL,
	NULL,
	NULL,
	&OnPerfromStateTransition,
	&OnServerShutdown,
	NULL,
	NULL,
	NULL,
	&OnGetServerStatus,
	NULL,
	NULL,
	NULL,
	&OnReceivedEvents
};

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
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(controlHandler, TRUE);
	OTCExecutionOptions executionOptions;
	OTCObjectHandle sessionHandle;
	OTCObjectHandle subscriptionHandle;
	BOOL end = FALSE;
	DWORD waitRet;
	LONG res;
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
	executionOptions.m_executionContext = 0;
	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		OTCAddAESession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"), 0, &sessionHandle);
		OTCAddAESubscription(sessionHandle, 1, &subscriptionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			usage();

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
							OTCChangeTargetState(sessionHandle, OTC_STATE_ACTIVE, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('a'):
							OTCChangeTargetState(sessionHandle, OTC_STATE_ACTIVE, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, NULL);
							break;

						case _T('C'):
							OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('c'):
							OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, NULL);
							break;

						case _T('D'):
							OTCChangeTargetState(sessionHandle, OTC_STATE_DISCONNECTED, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, &executionOptions);
							executionOptions.m_executionContext++;
							break;

						case _T('d'):
							OTCChangeTargetState(sessionHandle, OTC_STATE_DISCONNECTED, TRUE);
							OTCPerformStateTransition(sessionHandle, TRUE, NULL);
							break;

						case _T('S'):
							OTCGetServerStatus(sessionHandle, NULL, &executionOptions);
							break;

						case _T('s'):
						{
							OTCServerStatus serverStatus;
							memset(&serverStatus, 0, sizeof(OTCServerStatus));

							if (SUCCEEDED(res = OTCGetServerStatus(sessionHandle, &serverStatus, NULL)))
							{
								PrintServerStatus(&serverStatus);
							}
							else
							{
								_tprintf(_T("Get Status failed [0x%8.8X]\n"), res);
							}

							if (serverStatus.m_vendorInfo != NULL)
							{
								OTFreeMemory(serverStatus.m_vendorInfo);
							}

							if (serverStatus.m_productVersion != NULL)
							{
								OTFreeMemory(serverStatus.m_productVersion);
							}

							if (serverStatus.m_statusInfo != NULL)
							{
								OTFreeMemory(serverStatus.m_statusInfo);
							}

							if (serverStatus.m_supportedLCIDs != NULL)
							{
								OTFreeMemory(serverStatus.m_supportedLCIDs);
							}
						}
						break;

						case _T('M'):
						case _T('m'):
							if (SUCCEEDED(res = OTCActivateConnectionMonitor(sessionHandle, TRUE, 5000, 10, 10000, 300000)))
							{
								_tprintf(_T("Activated connection monitor\n"));
							}
							else
							{
								_tprintf(_T("Activate connection monitor failed [0x%8.8X]\n"), res);
							}

							break;

						case _T('N'):
						case _T('n'):
							if (SUCCEEDED(res = OTCActivateConnectionMonitor(sessionHandle, FALSE, 0, 0, 0, 0)))
							{
								_tprintf(_T("Deactivated connection monitor\n"));
							}
							else
							{
								_tprintf(_T("Deactivate connection monitor failed [0x%8.8X]\n"), res);
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

BYTE OTCAPI_CALL OnServerShutdown(IN OTCObjectData serverData, IN LPTSTR reason)
{
	_tprintf(_T("%s shutdown - reason: %s\n"), g_objectDescription[serverData.m_userData], reason);
	return TRUE; // reconnect automatically
}

void OTCAPI_CALL OnGetServerStatus(IN DWORD executionContext, IN OTCObjectData serverData, IN LONG result, IN OTCServerStatus* pServerStatus)
{
	if (SUCCEEDED(result))
	{
		PrintServerStatus(pServerStatus);
	}
	else
	{
		_tprintf(_T("Get Status failed [0x%8.8X]\n"), result);
	}
}

void OTCAPI_CALL OnReceivedEvents(IN OTCObjectContext objectContext, IN DWORD count, IN OTCEventData* pEvents, BYTE refresh, BYTE lastRefresh)
{
	_tprintf(_T("%s received events - %lu\n"), g_objectDescription[objectContext.m_objectData.m_userData], count);
	DWORD i, j;

	for (i = 0; i < count; i++)
	{
		_tprintf(_T("[%3.3lu]  Event type: %lu  Event category: %lu\n"), i, pEvents[i].m_eventType, pEvents[i].m_eventCategory);
		_tprintf(_T("       Source path: %s\n"), pEvents[i].m_sourcePath);
		_tprintf(_T("       Message: %s\n"), pEvents[i].m_message);
		_tprintf(_T("       Occurence time: %s\n"), GetDateTimeSZ(pEvents[i].m_occurenceTime));
		_tprintf(_T("       Severity: %lu\n"), pEvents[i].m_severity);
		_tprintf(_T("       Actor id: %s\n"), pEvents[i].m_actorID);
		_tprintf(_T("       Change mask: %u\n"), pEvents[i].m_changeMask);
		_tprintf(_T("       New state: %u\n"), (WORD)pEvents[i].m_newState);
		_tprintf(_T("       Ack Required: %u\n"), (WORD)pEvents[i].m_ackRequired);
		_tprintf(_T("       Quality: %u\n"), pEvents[i].m_quality);
		_tprintf(_T("       Condition name: %s\n"), pEvents[i].m_conditionName);
		_tprintf(_T("       Sub condition name: %s\n"), pEvents[i].m_subConditionName);
		_tprintf(_T("       Active time: %s\n"), GetDateTimeSZ(pEvents[i].m_activeTime));
		_tprintf(_T("       Attributes: %lu\n"), pEvents[i].m_eventAttrCount);

		for (j = 0; j < pEvents[i].m_eventAttrCount; j++)
		{
			_tprintf(_T("         [%3.3lu] %s"), j, GetValueSZ(&pEvents[i].m_pEventAttrs[j]));
		}
	}
}


TCHAR g_itemValueSZ[201];

LPTSTR GetValueSZ(IN OTCVariant* pVariant)
{
	OTVariantToString(pVariant, 200, g_itemValueSZ);
	return g_itemValueSZ;
}


TCHAR g_dateTimeSZ[256];

LPTSTR GetDateTimeSZ(IN OTCDateTime dateTime)
{
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&dateTime, &sysTime);
	_stprintf(g_dateTimeSZ, _T("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d.%3.3d"), sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	return g_dateTimeSZ;
}

void PrintServerStatus(OTCServerStatus* pServerStatus)
{
	DWORD i;
	_tprintf(_T("Server Status\n"));
	_tprintf(_T("   Vendor info: %s\n"), pServerStatus->m_vendorInfo);
	_tprintf(_T("   Product version: %s\n"), pServerStatus->m_productVersion);
	_tprintf(_T("   Start time: %s\n"), GetDateTimeSZ(pServerStatus->m_startTime));
	_tprintf(_T("   Last update time: %s\n"), GetDateTimeSZ(pServerStatus->m_lastUpdateTime));
	_tprintf(_T("   Current time: %s\n"), GetDateTimeSZ(pServerStatus->m_currentTime));
	_tprintf(_T("   Group count: %ld\n"), pServerStatus->m_groupCount);
	_tprintf(_T("   Bandwidth: %ld\n"), pServerStatus->m_bandwidth);

	for (i = 0; i < pServerStatus->m_countLCIDs; i++)
	{
		_tprintf(_T("   Supported LCID: 0x%4.4X\n"), pServerStatus->m_supportedLCIDs[i]);
	}

	_tprintf(_T("   Status info: %s\n"), pServerStatus->m_statusInfo);
}
