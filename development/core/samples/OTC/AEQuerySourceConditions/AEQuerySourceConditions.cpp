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
//  Filename    : AEQuerySourceConditions.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Query the conditions associated with a source example       |
//                                                                            |
//-----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

void OTCAPI_CALL OnQueryAESourceConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR sourcePath, IN DWORD count, IN LPTSTR* pConditionsNames);

// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

OTCCallbackFunctions g_callbackFunctions =
{
	NULL,
	NULL,
	NULL, +
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&OnQueryAESourceConditions
};

BOOL g_gotCallback = FALSE;


int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("AEQuerySourceConditions [/s][/a] <server URL> <source path>\n\n"));

	if (argc < 3)
	{
		return -1;
	}

	OTCObjectHandle sessionHandle;
	LPTSTR url = NULL;
	LPTSTR sourcePath = NULL;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
	executionOptions.m_executionContext = 0;

	if (argc == 3)
	{
		url = argv[1];
		sourcePath = argv[2];
	}
	else if (argc > 3)
	{
		if (_tcsicmp(argv[1], _T("/a")) == 0)
		{
			executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
		}

		url = argv[2];
		sourcePath = argv[3];
	}

	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		OTCAddAESession(url, 0, &sessionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			_tprintf(_T("Connected to server: %s\n\n"), url);
			LONG result;
			DWORD conditionCount;
			LPTSTR* pConditionNames;
			DWORD i;
			_tprintf(_T("Query source conditions of:  %s\n"), sourcePath);

			if (SUCCEEDED(result = OTCQueryAESourceConditions(sessionHandle, sourcePath, &conditionCount, &pConditionNames, &executionOptions)))
			{
				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					for (i = 0; i < conditionCount; i++)
					{
						printf(_T("[%3.3d] %s"), i, pConditionNames[i]);
						OTFreeMemory(pConditionNames[i]);
					}
				}
				else
				{
					while (!g_gotCallback)
					{
						Sleep(10);
					}
				}
			}
			else
			{
				_tprintf(_T("Failed to query the conditions of source: %s 0x%X\n"), sourcePath, result);
			}
		}
		else
		{
			_tprintf(_T("Can't connect to server: %s\n"), argv[1]);
		}

		OTCTerminate();
	}

	return 0;
}


void OTCAPI_CALL OnQueryAESourceConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR sourcePath, IN DWORD conditionCount, IN LPTSTR* pConditionNames)
{
	if (SUCCEEDED(result))
	{
		DWORD i;

		for (i = 0; i < conditionCount; i++)
		{
			printf(_T("[%3.3d] %s"), i, pConditionNames[i]);
		}
	}
	else
	{
		_tprintf(_T("Failed to query the conditions of source: %s 0x%X\n"), sourcePath, result);
	}

	g_gotCallback = TRUE;
}

