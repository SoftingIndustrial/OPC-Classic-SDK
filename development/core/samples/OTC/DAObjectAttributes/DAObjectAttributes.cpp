#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

TCHAR g_objectDescription [5][64] =
{
	_T("Session                "),
	_T("Subscription           "),
	_T("Item: maths.sin        "),
	_T("Item: time.local.second"),
	_T("Item: increment.UI1    ")
};

void PrintDASession(OTCObjectHandle sessionHandle);
void PrintDASubscription(OTCObjectHandle subscriptionHandle);
void PrintDAItems(OTCObjectHandle* pItemHandles);
void PrintDAItemAttributes(DWORD userData, DWORD whatAttributes, OTCObjectAttributes* pobjectAttributes);

// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

void OTCAPI_CALL OnPerfromStateTransition(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result);
void OTCAPI_CALL OnUpdateAttributes(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result, IN BYTE fromServer, IN DWORD attributesCount, IN DWORD* pWhatAttributes, IN LONG* pResults);
void OTCAPI_CALL OnUpdateItemAttributes(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG result, IN DWORD itemCount, IN OTCObjectData* pItemData, IN BYTE fromServer, IN DWORD attributesCount, IN DWORD* pWhatAttributes, IN LONG* pResults);

OTCCallbackFunctions g_callbackFunctions =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&OnPerfromStateTransition,
	NULL,
	&OnUpdateAttributes,
	&OnUpdateItemAttributes
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
	OTCObjectHandle groupHandle;
	OTCObjectHandle itemHandles[3];
	OTCObjectAttributes objectAttributes;
	DWORD whatAttributes[10];
	LONG results[10];
	LONG res;
	DWORD i;
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

	OTActivate(OTC_FEATURE_DA_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		// add objects
		OTCAddDASession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"), 0, &sessionHandle);
		OTCAddDASubscription(sessionHandle, 500, 1, &groupHandle);
		OTCAddDAItem(groupHandle, _T("maths.sin"), 2, &itemHandles[0]);
		OTCAddDAItem(groupHandle, _T("time.local.second"), 3, &itemHandles[1]);
		OTCAddDAItem(groupHandle, _T("increment.UI1"), 4, &itemHandles[2]);
		// connect objects to server
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, &executionOptions))
		{
			if (WaitForExecution(0) == EXECUTION_DONE)
			{
				executionOptions.m_executionContext = 1;
				// get the inital attributes, they have been retrieved from the server at the connect
				_tprintf(_T("--- Initial object attributes ---\n"));
				PrintDASession(sessionHandle);
				PrintDASubscription(groupHandle);
				PrintDAItems(itemHandles);
				// change the servers LCID and cient name
				_tprintf(_T("\n--- Change the sessions LCID and cient name ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_daSessionLCID = 1031;
				objectAttributes.m_daSessionClientName = _T("OTB ObjectAttributes Sample");
				OTCSetAttributes(sessionHandle, OTC_ATTRIBUTE_DASESSION_CLIENTNAME | OTC_ATTRIBUTE_DASESSION_LCID, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_CLIENTNAME;
				whatAttributes[1] = OTC_ATTRIBUTE_DASESSION_LCID;
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

				PrintDASession(sessionHandle);
				// change the subscriptions name, update rate and keep alive time
				_tprintf(_T("\n--- Change the subscriptions name, update rate, deadband and keep alive time ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_daSubscriptionName = _T("OTB ObjectAttributes Subscription");
				objectAttributes.m_daSubscriptionRequestedUpdateRate = 1000;
				objectAttributes.m_daSubscriptionRequestedKeepAliveTime = 4000;
				objectAttributes.m_daSubscriptionDeadband = 10.0;
				OTCSetAttributes(groupHandle, OTC_ATTRIBUTE_DASUBSCRIPTION_NAME | OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE | OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME | OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_NAME;
				whatAttributes[1] = OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE | OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND;
				whatAttributes[2] = OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME;
				// write changed data to the server
				res = OTCUpdateAttributes(groupHandle, FALSE, 3, whatAttributes, results, &executionOptions);
				WaitForExecution(3);
				executionOptions.m_executionContext = 4;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result name: 0x%X\n  result update rate and deadband: 0x%X\n  result keep alive time: 0x%X\n"), res, results[0], results[1], results[2]);
				}

				// get it from the server to check
				res = OTCUpdateAttributes(groupHandle, TRUE, 3, whatAttributes, results, &executionOptions);
				WaitForExecution(4);
				executionOptions.m_executionContext = 5;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result name: 0x%X\n  result update rate and deadband: 0x%X\n  result keep alive time: 0x%X\n"), res, results[0], results[1], results[2]);
				}

				PrintDASubscription(groupHandle);
				// change the requested datatype of the items
				_tprintf(_T("\n--- Change requested datatype of the items ---\n"));
				memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
				objectAttributes.m_daItemRequestedDatatype = VT_BSTR;
				OTCSetAttributes(itemHandles[0], OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE, &objectAttributes);
				OTCSetAttributes(itemHandles[1], OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE, &objectAttributes);
				OTCSetAttributes(itemHandles[2], OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE, &objectAttributes);
				whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE;
				// write requested datatypes to the server
				res = OTCUpdateDAItemAttributes(groupHandle, 3, itemHandles, FALSE, 1, whatAttributes, results, &executionOptions);
				WaitForExecution(5);
				executionOptions.m_executionContext = 6;

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate item attributes to server - 0x%X\n  result datatype: 0x%X\n"), res, results[0]);
				}

				// get it from the server to check
				res = OTCUpdateDAItemAttributes(groupHandle, 3, itemHandles, TRUE, 1, whatAttributes, results, &executionOptions);
				WaitForExecution(6);

				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					_tprintf(_T("\nUpdate item attributes from server - 0x%X\n  result datatype: 0x%X\n"), res, results[0]);
				}

				PrintDAItems(itemHandles);
			}
		}

		OTCTerminate();
	}

	return 0;
}

TCHAR g_opcSpec[5][8] =
{
	_T("default"), _T("DA1"), _T("DA2"), _T("DA3"), _T("XML-DA")
};

LPTSTR GetOPCSpecificationString(BYTE opcSpec)
{
	LPTSTR spec = NULL;

	if (opcSpec == OT_OPCSPECIFICATION_DEFAULT)
	{
		spec = g_opcSpec[0];
	}
	else if (opcSpec == OT_OPCSPECIFICATION_DA10)
	{
		spec = g_opcSpec[1];
	}
	else if (opcSpec == OT_OPCSPECIFICATION_DA20)
	{
		spec = g_opcSpec[2];
	}
	else if (opcSpec == OT_OPCSPECIFICATION_DA30)
	{
		spec = g_opcSpec[3];
	}
	else if (opcSpec == OT_OPCSPECIFICATION_XMLDA10)
	{
		spec = g_opcSpec[4];
	}

	return spec;
}

void PrintDASession(OTCObjectHandle sessionHandle)
{
	OTCObjectAttributes objectAttributes;
	LPTSTR supportedSpec;
	LPTSTR forcedSpec;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	OTCGetAttributes(sessionHandle, OTC_ATTRIBUTE_DASESSION_ALL, &objectAttributes);
	_tprintf(_T("\n%s\n"), g_objectDescription[0]);
	_tprintf(_T("  URL: %s\n"), objectAttributes.m_daSessionURL);
	OTFreeMemory(objectAttributes.m_daSessionURL);
	_tprintf(_T("  Client Name: %s\n"), objectAttributes.m_daSessionClientName);
	OTFreeMemory(objectAttributes.m_daSessionClientName);
	_tprintf(_T("  LCID: %lu\n"), objectAttributes.m_daSessionLCID);
	supportedSpec = GetOPCSpecificationString(objectAttributes.m_daSessionSupportedOPCSpecification);
	forcedSpec = GetOPCSpecificationString(objectAttributes.m_daSessionForcedOPCSpecification);
	_tprintf(_T("  Supported OPC Specification: %s\n"), supportedSpec);
	_tprintf(_T("  Forced OPC Specification: %s\n"), forcedSpec);
}

void PrintDASubscription(OTCObjectHandle subscriptionHandle)
{
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	OTCGetAttributes(subscriptionHandle, OTC_ATTRIBUTE_DASUBSCRIPTION_ALL, &objectAttributes);
	_tprintf(_T("\n%s\n"), g_objectDescription[1]);
	_tprintf(_T("  Name: %s\n"), objectAttributes.m_daSubscriptionName);
	OTFreeMemory(objectAttributes.m_daSubscriptionName);
	_tprintf(_T("  Requested Update Rate: %lu ms\n"), objectAttributes.m_daSubscriptionRequestedUpdateRate);
	_tprintf(_T("  Revised Update Rate: %lu ms\n"), objectAttributes.m_daSubscriptionRevisedUpdateRate);
	_tprintf(_T("  Deadband: %g\n"), objectAttributes.m_daSubscriptionDeadband);
	_tprintf(_T("  Time Bias: %ld\n"), objectAttributes.m_daSubscriptionTimeBias);
	_tprintf(_T("  Requested Keep Alive Time: %lu ms\n"), objectAttributes.m_daSubscriptionRequestedKeepAliveTime);
	_tprintf(_T("  Revised Keep Alive Time: %lu ms\n"), objectAttributes.m_daSubscriptionRevisedKeepAliveTime);
	_tprintf(_T("  LCID: %lu\n"), objectAttributes.m_daSubscriptionLCID);
}

void PrintDAItems(OTCObjectHandle* pItemHandles)
{
	OTCObjectAttributes objectAttributes[3];
	memset(&objectAttributes[0], 0, sizeof(OTCObjectAttributes));
	memset(&objectAttributes[1], 0, sizeof(OTCObjectAttributes));
	memset(&objectAttributes[2], 0, sizeof(OTCObjectAttributes));
	objectAttributes[0].m_daItemEuInfo = (OTCVariant*)OTAllocateMemory(sizeof(OTCVariant));
	objectAttributes[1].m_daItemEuInfo = (OTCVariant*)OTAllocateMemory(sizeof(OTCVariant));
	objectAttributes[2].m_daItemEuInfo = (OTCVariant*)OTAllocateMemory(sizeof(OTCVariant));
	OTCGetAttributes(pItemHandles[0], OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[0]);
	PrintDAItemAttributes(2, OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[0]);
	OTCGetAttributes(pItemHandles[1], OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[1]);
	PrintDAItemAttributes(3, OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[1]);
	OTCGetAttributes(pItemHandles[2], OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[2]);
	PrintDAItemAttributes(4, OTC_ATTRIBUTE_DAITEM_ALL, &objectAttributes[2]);
	::VariantClear(objectAttributes[0].m_daItemEuInfo);
	::VariantClear(objectAttributes[1].m_daItemEuInfo);
	::VariantClear(objectAttributes[2].m_daItemEuInfo);
	OTFreeMemory(objectAttributes[0].m_daItemEuInfo);
	OTFreeMemory(objectAttributes[1].m_daItemEuInfo);
	OTFreeMemory(objectAttributes[2].m_daItemEuInfo);
}

void PrintDAItemAttributes(DWORD userData, DWORD whatAttributes, OTCObjectAttributes* pObjectAttributes)
{
	_tprintf(_T("\n%s\n"), g_objectDescription[userData]);

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_ID) == OTC_ATTRIBUTE_DAITEM_ID)
	{
		_tprintf(_T("  ID: %s\n"), pObjectAttributes->m_daItemID);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_PATH) == OTC_ATTRIBUTE_DAITEM_PATH)
	{
		_tprintf(_T("  Path: %s\n"), pObjectAttributes->m_daItemPath);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE) == OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE)
	{
		_tprintf(_T("  Native Datatype: %u\n"), pObjectAttributes->m_daItemNativeDatatype);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS) == OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS)
	{
		_tprintf(_T("  Access Rights: %lu\n"), pObjectAttributes->m_daItemAccessRights);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE) == OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE)
	{
		_tprintf(_T("  Requested Datatype: %u\n"), pObjectAttributes->m_daItemRequestedDatatype);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_DEADBAND) == OTC_ATTRIBUTE_DAITEM_DEADBAND)
	{
		_tprintf(_T("  Deadband: %g\n"), pObjectAttributes->m_daItemDeadband);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUTYPE) == OTC_ATTRIBUTE_DAITEM_EUTYPE)
	{
		_tprintf(_T("  EU Type: %u\n"), (WORD)pObjectAttributes->m_daItemEuType);
	}

	if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUINFO) == OTC_ATTRIBUTE_DAITEM_EUINFO)
	{
		TCHAR euInfo[100];
		_tprintf(_T("  EU Info: %s\n"), OTVariantToString(pObjectAttributes->m_daItemEuInfo, 99, euInfo));
	}

	OTFreeMemory(pObjectAttributes->m_daItemID);
	OTFreeMemory(pObjectAttributes->m_daItemPath);
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
		_tprintf(_T("\nUpdate subscription attributes to server - 0x%X\n  result name: 0x%X\n  result update rate and deadband: 0x%X\n  result keep alive time: 0x%X\n"), res, results[0], results[1], results[2]);
		break;

	case 4:
		_tprintf(_T("\nUpdate subscription attributes from server - 0x%X\n  result name: 0x%X\n  result update rate and deadband: 0x%X\n  result keep alive time: 0x%X\n"), res, results[0], results[1], results[2]);
		break;
	}

	g_waitForExecution[executionContext] = EXECUTION_DONE;
}

void OTCAPI_CALL OnUpdateItemAttributes(IN DWORD executionContext, IN OTCObjectContext objectContext, IN LONG res, IN DWORD itemCount, IN OTCObjectData* pItemData, IN BYTE fromServer, IN DWORD attributesCount, IN DWORD* pWhatAttributes, IN LONG* results)
{
	switch (executionContext)
	{
	case 5:
		_tprintf(_T("\nUpdate item attributes to server - 0x%X\n  result datatype: 0x%X\n"), res, results[0]);
		break;

	case 6:
		_tprintf(_T("\nUpdate item attributes from server - 0x%X\n  result datatype: 0x%X\n"), res, results[0]);
		break;
	}

	g_waitForExecution[executionContext] = EXECUTION_DONE;
}

