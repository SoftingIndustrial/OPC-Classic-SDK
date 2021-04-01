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
//  Filename    : AEObjectAttributes.cpp                                      |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Update an AE object's attributes example                    |
//                                                                            |
//-----------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

TCHAR g_objectDescription [2][64] =
{
	_T("Session      "),
	_T("Subscription ")
};

void PrintAESession(OTCObjectHandle sessionHandle);
void PrintAESubscription(OTCObjectHandle subscriptionHandle);

// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result);
void OTCAPI_CALL OnUpdateAttributes(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result, IN BYTE fromServer, IN DWORD attributesCount, IN DWORD* pWhatAttributes, IN LONG* pResults);

OTCCallbackFunctions g_callbackFunctions =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&OnPerfromStateTransition,
	NULL,
	&OnUpdateAttributes
};

#define EXECUTION_DONE     0
#define EXECUTION_WAIT_FOR 1
#define EXECUTION_ERROR    255
BYTE g_waitForExecution[7];

BYTE WaitForExecution(SHORT m_executionContext);

int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("ObjectAttributes [/s][/a]\n"));
	_tprintf(_T("\t/s : synchonous execution of the OCT functions (default)\n"));
	_tprintf(_T("\t/a : asynchonous execution of the OCT functions\n"));
	OTCExecutionOptions executionOptions;
	OTCObjectHandle sessionHandle;
	OTCObjectHandle subscriptionHandle;
	OTCObjectAttributes objectAttributes;
	DWORD whatAttributes[10];
	LONG results[10];
	LONG res;
	DWORD i;
	DWORD filterCategories[3];
	TCHAR filterAreas0[50];
	TCHAR filterAreas1[50];
	LPTSTR filterAreas[2];
	TCHAR filterSources0[50];
	LPTSTR filterSources[1];
	OTCAEReturnedAttributesData returnedAttributes[2];
	DWORD attrCat0[3];
	DWORD attrCat1[3];
	filterAreas[0] = filterAreas0;
	filterAreas[1] = filterAreas1;
	filterSources[0] = filterSources0;
	returnedAttributes[0].m_attributeIDs = attrCat0;
	returnedAttributes[1].m_attributeIDs = attrCat1;
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
	executionOptions.m_executionContext = 0;

	if (argc == 2)
	{
		if (_tcsicmp(argv[1], _T("/a")) == 0)
		{
			executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
		}
	}

	for (i = 0; i < 7; i++)
	{
		g_waitForExecution[i] = (executionOptions.m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS) ? EXECUTION_WAIT_FOR : EXECUTION_DONE;
	}

	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		// add objects
		OTCAddAESession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"), 0, &sessionHandle);
		OTCAddAESubscription(sessionHandle, 0, &subscriptionHandle);
		// connect objects to server
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, &executionOptions))
		{
			if (WaitForExecution(0) == EXECUTION_DONE)
			{
				executionOptions.m_executionContext = 1;
				// get the inital attributes, they have been retrieved from the server at the connect
				_tprintf(_T("--- Initial object attributes ---\n"));
				PrintAESession(sessionHandle);
				PrintAESubscription(subscriptionHandle);
				// change the sessions LCID and cient name
				_tprintf(_T("\n--- Change the servers LCID and cient name ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_aeSessionLCID = 1031;
				objectAttributes.m_aeSessionClientName = _T("OTB ObjectAttributes Sample");
				OTCSetAttributes(sessionHandle, OTC_ATTRIBUTE_AESESSION_CLIENTNAME | OTC_ATTRIBUTE_AESESSION_LCID, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_CLIENTNAME;
				whatAttributes[1] = OTC_ATTRIBUTE_AESESSION_LCID;
				// write changed data to the server
				res = OTCUpdateAttributes(sessionHandle, FALSE, 2, whatAttributes, results, &executionOptions);
				WaitForExecution(1);
				executionOptions.m_executionContext = 2;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate server attributes to server - 0x%X\n  result client name: 0x%X\n  result LCID: 0x%X\n"), res, results[0], results[1]);
				}

				// get it from the server to check
				res = OTCUpdateAttributes(sessionHandle, TRUE, 2, whatAttributes, results, &executionOptions);
				WaitForExecution(2);
				executionOptions.m_executionContext = 3;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate server attributes from server - 0x%X\n  result client name: 0x%X\n  result LCID: 0x%X\n"), res, results[0], results[1]);
				}

				PrintAESession(sessionHandle);
				// change the subscriptions max size, buffer time and filter
				_tprintf(_T("\n--- Change the subscriptions max size, buffer time and filter ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_aeSubscriptionRequestedBufferTime = 2000;
				objectAttributes.m_aeSubscriptionRequestedMaxSize = 5;
				objectAttributes.m_aeSubscriptionFilterSeverityHigh = 700;
				objectAttributes.m_aeSubscriptionFilterSeverityLow = 100;
				objectAttributes.m_aeSubscriptionFilterEventTypes = OT_EVENTTYPE_SIMPLE | OT_EVENTTYPE_TRACKING;
				objectAttributes.m_aeSubscriptionFilterCountCategrories = 3;
				filterCategories[0] = 1;
				filterCategories[1] = 2;
				filterCategories[2] = 3;
				objectAttributes.m_aeSubscriptionFilterCategrories = filterCategories;
				objectAttributes.m_aeSubscriptionFilterCountAreas = 2;
				_tcscpy(filterAreas0, _T("computer.mouse"));
				_tcscpy(filterAreas1, _T("computer.clock"));
				objectAttributes.m_aeSubscriptionFilterAreas = filterAreas;
				objectAttributes.m_aeSubscriptionFilterCountSources = 1;
				_tcscpy(filterSources0, _T("computer.clock.time slot 2"));
				objectAttributes.m_aeSubscriptionFilterSources = filterSources;
				OTCSetAttributes(subscriptionHandle, OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME | OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE | OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME | OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE;
				whatAttributes[1] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
				// write changed data to the server
				res = OTCUpdateAttributes(subscriptionHandle, FALSE, 2, whatAttributes, results, &executionOptions);
				WaitForExecution(3);
				executionOptions.m_executionContext = 4;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result buffer time and max size: 0x%X\n  result filter: 0x%X\n"), res, results[0], results[1]);
				}

				// get it from the server to check
				res = OTCUpdateAttributes(subscriptionHandle, TRUE, 2, whatAttributes, results, &executionOptions);
				WaitForExecution(4);
				executionOptions.m_executionContext = 5;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result buffer time and max size: 0x%X\n  result filter: 0x%X\n"), res, results[0], results[1]);
				}

				PrintAESubscription(subscriptionHandle);
				// change the subscriptions returned attributes
				_tprintf(_T("\n--- Change the subscriptions returned attributes ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_aeSubscriptionCountReturnedAttributes = 2;
				returnedAttributes[0].m_categoryID = 2;
				returnedAttributes[0].m_countAttributeIDs = 2;
				attrCat0[0] = 3;
				attrCat0[1] = 4;
				returnedAttributes[1].m_categoryID = 3;
				returnedAttributes[1].m_countAttributeIDs = 1;
				attrCat1[0] = 6;
				objectAttributes.m_aeSubscriptionReturnedAttributes = returnedAttributes;
				OTCSetAttributes(subscriptionHandle, OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME | OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES;
				// write changed data to the server
				res = OTCUpdateAttributes(subscriptionHandle, FALSE, 1, whatAttributes, results, &executionOptions);
				WaitForExecution(5);
				executionOptions.m_executionContext = 6;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result returned attributes: 0x%X\n"), res, results[0]);
				}

				// get it from the server to check
				res = OTCUpdateAttributes(subscriptionHandle, TRUE, 1, whatAttributes, results, &executionOptions);
				WaitForExecution(6);
				executionOptions.m_executionContext = 7;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result returned attributes: 0x%X\n"), res, results[0]);
				}

				PrintAESubscription(subscriptionHandle);
			}
		}

		OTCTerminate();
	}

	return 0;
}

void PrintAESession(OTCObjectHandle sessionHandle)
{
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	OTCGetAttributes(sessionHandle, OTC_ATTRIBUTE_AESESSION_ALL, &objectAttributes);
	_tprintf(_T("\n%s\n"), g_objectDescription[0]);
	_tprintf(_T("  URL: %s\n"), objectAttributes.m_aeSessionURL);
	OTFreeMemory(objectAttributes.m_aeSessionURL);
	_tprintf(_T("  Client Name: %s\n"), objectAttributes.m_aeSessionClientName);
	OTFreeMemory(objectAttributes.m_aeSessionClientName);
	_tprintf(_T("  LCID: %lu\n"), objectAttributes.m_aeSessionLCID);
}

void PrintAESubscription(OTCObjectHandle subscriptionHandle)
{
	OTCObjectAttributes objectAttributes;
	DWORD i, j;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	OTCGetAttributes(subscriptionHandle, OTC_ATTRIBUTE_AESUBSCRIPTION_ALL, &objectAttributes);
	_tprintf(_T("\n%s\n"), g_objectDescription[1]);
	_tprintf(_T("  Requested Buffer Time: %lu ms\n"), objectAttributes.m_aeSubscriptionRequestedBufferTime);
	_tprintf(_T("  Revised Buffer Time: %lu ms\n"), objectAttributes.m_aeSubscriptionRevisedBufferTime);
	_tprintf(_T("  Requested Max Size: %lu events\n"), objectAttributes.m_aeSubscriptionRequestedMaxSize);
	_tprintf(_T("  Revised Max Size: %lu events\n"), objectAttributes.m_aeSubscriptionRevisedMaxSize);
	_tprintf(_T("  Filter:\n"), objectAttributes.m_aeSubscriptionRevisedMaxSize);
	_tprintf(_T("    Low Severity: %lu\n"), objectAttributes.m_aeSubscriptionFilterSeverityLow);
	_tprintf(_T("    High Severity: %lu\n"), objectAttributes.m_aeSubscriptionFilterSeverityHigh);
	_tprintf(_T("    Event Types: %lu\n"), objectAttributes.m_aeSubscriptionFilterEventTypes);
	_tprintf(_T("    Categories: count: %lu\n"), objectAttributes.m_aeSubscriptionFilterCountCategrories);

	for (i = 0; i < objectAttributes.m_aeSubscriptionFilterCountCategrories; i++)
	{
		_tprintf(_T("      [%3.3lu] : %lu\n"), i, objectAttributes.m_aeSubscriptionFilterCategrories[i]);
	}

	if (objectAttributes.m_aeSubscriptionFilterCategrories)
	{
		OTFreeMemory(objectAttributes.m_aeSubscriptionFilterCategrories);
	}

	_tprintf(_T("    Areas: count: %lu\n"), objectAttributes.m_aeSubscriptionFilterCountAreas);

	for (i = 0; i < objectAttributes.m_aeSubscriptionFilterCountAreas; i++)
	{
		_tprintf(_T("      [%3.3lu] : %s\n"), i, objectAttributes.m_aeSubscriptionFilterAreas[i]);
		OTFreeMemory(objectAttributes.m_aeSubscriptionFilterAreas[i]);
	}

	if (objectAttributes.m_aeSubscriptionFilterAreas)
	{
		OTFreeMemory(objectAttributes.m_aeSubscriptionFilterAreas);
	}

	_tprintf(_T("    Sources: count: %lu\n"), objectAttributes.m_aeSubscriptionFilterCountSources);

	for (i = 0; i < objectAttributes.m_aeSubscriptionFilterCountSources; i++)
	{
		_tprintf(_T("      [%3.3lu] : %s\n"), i, objectAttributes.m_aeSubscriptionFilterSources[i]);
		OTFreeMemory(objectAttributes.m_aeSubscriptionFilterSources[i]);
	}

	if (objectAttributes.m_aeSubscriptionFilterSources)
	{
		OTFreeMemory(objectAttributes.m_aeSubscriptionFilterSources);
	}

	_tprintf(_T("  Returned Attributes: count: %lu\n"), objectAttributes.m_aeSubscriptionCountReturnedAttributes);

	for (i = 0; i < objectAttributes.m_aeSubscriptionCountReturnedAttributes; i++)
	{
		_tprintf(_T("    [%3.3lu] : category ID %lu : count %lu\n"), i, objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_categoryID, objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_countAttributeIDs);

		for (j = 0; j < objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_countAttributeIDs; j++)
		{
			_tprintf(_T("      [%3.3lu] : %lu\n"), j, objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_attributeIDs[j]);
		}

		OTFreeMemory(objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_attributeIDs);
	}

	if (objectAttributes.m_aeSubscriptionReturnedAttributes)
	{
		OTFreeMemory(objectAttributes.m_aeSubscriptionReturnedAttributes);
	}
}

BYTE WaitForExecution(SHORT m_executionContext)
{
	while (g_waitForExecution[m_executionContext] == EXECUTION_WAIT_FOR)
	{
		Sleep(10);
	}

	return g_waitForExecution[m_executionContext];
}

void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result)
{
	_tprintf(_T("\n--> performed state transition - 0x%X\n"), result);
	g_waitForExecution[executionContext] = (result == S_OK) ? EXECUTION_DONE : EXECUTION_ERROR;
}

void OTCAPI_CALL OnUpdateAttributes(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG res, IN BYTE fromServer, IN DWORD attributesCount, IN DWORD* pWhatAttributes, IN LONG* results)
{
	switch (executionContext)
	{
	case 1:
		_tprintf(_T("\nUpdate session attributes to server - 0x%X\n  result client name: 0x%X\n  result LCID: 0x%X\n"), res, results[0], results[1]);
		break;

	case 2:
		_tprintf(_T("\nUpdate session attributes from server - 0x%X\n  result client name: 0x%X\n  result LCID: 0x%X\n"), res, results[0], results[1]);
		break;

	case 3:
		_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result buffer time and max size: 0x%X\n  result filter: 0x%X\n"), res, results[0], results[1]);
		break;

	case 4:
		_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result buffer time and max size: 0x%X\n  result filter: 0x%X\n"), res, results[0], results[1]);
		break;

	case 5:
		_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result returned attributes: 0x%X\n"), res, results[0]);
		break;

	case 6:
		_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result returned attributes: 0x%X\n"), res, results[0]);
		break;
	}

	g_waitForExecution[executionContext] = EXECUTION_DONE;
}

