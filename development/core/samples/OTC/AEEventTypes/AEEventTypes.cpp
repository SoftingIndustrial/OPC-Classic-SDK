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
//  Filename    : AEEventTypes.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Get the event categories example                            |
//                                                                            |
//-----------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

void OTCAPI_CALL OnQueryAECategories(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD count, IN DWORD* pEventTypes, IN DWORD* pCategoryIds, IN LPTSTR* pCategoryDescriptions);
void OTCAPI_CALL OnQueryAEAttributes(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD categoryId, IN DWORD count, IN DWORD* pAttributeIds, IN LPTSTR* pAttributeDescriptions, IN WORD* pAttributeDatatypes);
void OTCAPI_CALL OnQueryAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD categoryId, IN DWORD count, IN LPTSTR* pConditionsNames);
void OTCAPI_CALL OnQueryAESubConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR conditionName, IN DWORD count, IN LPTSTR* pSubConditionsNames);
void OTCAPI_CALL OnQueryAvailableAEFilters(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD availableFilters);

void HandleCategory(OTCObjectHandle sessionHandle, DWORD eventType, DWORD catId, LPTSTR catDescr);
LONG WaitForExecution(DWORD m_executionContext);

// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

OTCCallbackFunctions g_callbackFunctions =
{
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
	&OnQueryAvailableAEFilters,
	&OnQueryAECategories,
	&OnQueryAEAttributes,
	&OnQueryAEConditions,
	&OnQueryAESubConditions
};

OTCExecutionOptions g_executionOptions;

DWORD g_waitForExecution = 0xFFFFFFFF;
LONG g_result = E_FAIL;
DWORD g_catCount = 0;
DWORD* g_pEventTypes = NULL;
DWORD* g_pCategoryIds = NULL;
LPTSTR* g_pCategoryDescriptions = NULL;
DWORD g_attCount = 0;
DWORD* g_pAttributeIds = NULL;
LPTSTR* g_pAttributeDescriptions = NULL;
WORD* g_pAttributeDatatypes = NULL;
DWORD g_conCount = 0;
LPTSTR* g_pConditionNames = NULL;
DWORD g_subConCount = 0;
LPTSTR* g_pSubConditionNames = NULL;
DWORD g_availableFilter = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("AEEventTypes [/a] <server URL>\n"));
	_tprintf(_T("\t/a : asynchonous execution of the OCT functions\n\n"));
	LPTSTR url = NULL;
	g_executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
	g_executionOptions.m_executionContext = 0;

	if (argc == 2)
	{
		url = argv[1];
	}
	else if (argc > 2)
	{
		if (_tcsicmp(argv[1], _T("/a")) == 0)
		{
			g_executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
		}

		url = argv[2];
	}

	OTCObjectHandle sessionHandle;
	LONG res;
	DWORD i;
	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		OTCAddAESession(url, 0, &sessionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			_tprintf(_T("Connected to server: %s\n\n"), url);
			// query categories
			res = OTCQueryAECategories(sessionHandle, &g_catCount, &g_pEventTypes, &g_pCategoryIds, &g_pCategoryDescriptions, &g_executionOptions);

			if (g_executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				res = WaitForExecution(g_executionOptions.m_executionContext);
				g_executionOptions.m_executionContext++;
			}

			if (SUCCEEDED(res))
			{
				for (i = 0; i < g_catCount; i++)
				{
					HandleCategory(sessionHandle, g_pEventTypes[i], g_pCategoryIds[i], g_pCategoryDescriptions[i]);
					OTFreeMemory(g_pCategoryDescriptions[i]);
				}

				OTFreeMemory(g_pEventTypes);
				OTFreeMemory(g_pCategoryIds);
				OTFreeMemory(g_pCategoryDescriptions);
			}

			// query available filters
			res = OTCQueryAvailableAEFilters(sessionHandle, &g_availableFilter, &g_executionOptions);

			if (g_executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				res = WaitForExecution(g_executionOptions.m_executionContext);
				g_executionOptions.m_executionContext++;
			}

			if (SUCCEEDED(res))
			{
				_tprintf(_T("Available filter: %li\n"), g_availableFilter);
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

TCHAR g_eventTypes[4][10] =
{
	_T("Simple   "), _T("Tracking "), _T(""), _T("Condition")
};

void HandleCategory(OTCObjectHandle sessionHandle, DWORD eventType, DWORD catId, LPTSTR catDescr)
{
	_tprintf(_T("Category: Event type: %s  Id: %lu  Description: %s\n"), g_eventTypes[eventType - 1], catId, catDescr);
	LONG res;
	DWORD i, j;
	// query attributes
	res = OTCQueryAEAttributes(sessionHandle, catId, &g_attCount, &g_pAttributeIds, &g_pAttributeDescriptions, &g_pAttributeDatatypes, &g_executionOptions);

	if (g_executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
	{
		res = WaitForExecution(g_executionOptions.m_executionContext);
		g_executionOptions.m_executionContext++;
	}

	if (SUCCEEDED(res))
	{
		for (i = 0; i < g_attCount; i++)
		{
			_tprintf(_T("  Attribute: Id: %lu  Description: %s  Datatype: %u\n"), g_pAttributeIds[i], g_pAttributeDescriptions[i], g_pAttributeDatatypes[i]);
			OTFreeMemory(g_pAttributeDescriptions[i]);
		}

		OTFreeMemory(g_pAttributeDatatypes);
		OTFreeMemory(g_pAttributeIds);
		OTFreeMemory(g_pAttributeDescriptions);
	}

	if (eventType == OT_EVENTTYPE_CONDITION)
	{
		// query conditions
		res = OTCQueryAEConditions(sessionHandle, catId, &g_conCount, &g_pConditionNames, &g_executionOptions);

		if (g_executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			res = WaitForExecution(g_executionOptions.m_executionContext);
			g_executionOptions.m_executionContext++;
		}

		if (SUCCEEDED(res))
		{
			for (i = 0; i < g_conCount; i++)
			{
				_tprintf(_T("  Condition: Name: %s\n"), g_pConditionNames[i]);
				// query sub conditions
				res = OTCQueryAESubConditions(sessionHandle, g_pConditionNames[i], &g_subConCount, &g_pSubConditionNames, &g_executionOptions);

				if (g_executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
				{
					res = WaitForExecution(g_executionOptions.m_executionContext);
					g_executionOptions.m_executionContext++;
				}

				if (SUCCEEDED(res))
				{
					for (j = 0; j < g_subConCount; j++)
					{
						_tprintf(_T("  Sub condition: Name: %s\n"), g_pSubConditionNames[j]);
						OTFreeMemory(g_pSubConditionNames[j]);
					}

					OTFreeMemory(g_pSubConditionNames);
				}

				OTFreeMemory(g_pConditionNames[i]);
			}

			OTFreeMemory(g_pConditionNames);
		}
	}
}

LONG WaitForExecution(DWORD m_executionContext)
{
	while (g_waitForExecution != m_executionContext)
	{
		Sleep(10);
	}

	return g_result;
}

void OTCAPI_CALL OnQueryAECategories(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD count, IN DWORD* pEventTypes, IN DWORD* pCategoryIds, IN LPTSTR* pCategoryDescriptions)
{
	DWORD i;
	g_result = result;
	g_catCount = count;
	g_pEventTypes = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
	g_pCategoryIds = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
	g_pCategoryDescriptions = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));

	for (i = 0; i < count; i++)
	{
		g_pEventTypes[i] = pEventTypes[i];
		g_pCategoryIds[i] = pCategoryIds[i];
		g_pCategoryDescriptions[i] = (LPTSTR)OTAllocateMemory(((DWORD)_tcslen(pCategoryDescriptions[i]) + 1) * sizeof(TCHAR));
		_tcscpy(g_pCategoryDescriptions[i], pCategoryDescriptions[i]);
	}

	g_waitForExecution = executionContext;
}

void OTCAPI_CALL OnQueryAEAttributes(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD categoryId, IN DWORD count, IN DWORD* pAttributeIds, IN LPTSTR* pAttributeDescriptions, IN WORD* pAttributeDatatypes)
{
	DWORD i;
	g_result = result;
	g_attCount = count;
	g_pAttributeDatatypes = (WORD*)OTAllocateMemory(count * sizeof(WORD));
	g_pAttributeIds = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
	g_pAttributeDescriptions = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));

	for (i = 0; i < count; i++)
	{
		g_pAttributeDatatypes[i] = pAttributeDatatypes[i];
		g_pAttributeIds[i] = pAttributeIds[i];
		g_pAttributeDescriptions[i] = (LPTSTR)OTAllocateMemory(((DWORD)_tcslen(pAttributeDescriptions[i]) + 1) * sizeof(TCHAR));
		_tcscpy(g_pAttributeDescriptions[i], pAttributeDescriptions[i]);
	}

	g_waitForExecution = executionContext;
}

void OTCAPI_CALL OnQueryAEConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD categoryId, IN DWORD count, IN LPTSTR* pConditionsNames)
{
	DWORD i;
	g_result = result;
	g_conCount = count;
	g_pConditionNames = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));

	for (i = 0; i < count; i++)
	{
		g_pConditionNames[i] = (LPTSTR)OTAllocateMemory(((DWORD)_tcslen(pConditionsNames[i]) + 1) * sizeof(TCHAR));
		_tcscpy(g_pConditionNames[i], pConditionsNames[i]);
	}

	g_waitForExecution = executionContext;
}

void OTCAPI_CALL OnQueryAESubConditions(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN LPTSTR conditionName, IN DWORD count, IN LPTSTR* pSubConditionsNames)
{
	DWORD i;
	g_result = result;
	g_subConCount = count;
	g_pSubConditionNames = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));

	for (i = 0; i < count; i++)
	{
		g_pSubConditionNames[i] = (LPTSTR)OTAllocateMemory(((DWORD)_tcslen(pSubConditionsNames[i]) + 1) * sizeof(TCHAR));
		_tcscpy(g_pSubConditionNames[i], pSubConditionsNames[i]);
	}

	g_waitForExecution = executionContext;
}

void OTCAPI_CALL OnQueryAvailableAEFilters(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN DWORD availableFilters)
{
	g_result = result;
	g_availableFilter = availableFilters;
	g_waitForExecution = executionContext;
}

