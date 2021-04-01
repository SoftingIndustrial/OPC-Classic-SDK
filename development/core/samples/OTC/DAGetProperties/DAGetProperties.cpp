#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

void PrintProperties(IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData);
void FreePropertiesMemory(IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData);
void OTCAPI_CALL OnGetDAProperties(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData);

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
	&OnGetDAProperties
};

BOOL g_gotCallback = FALSE;


int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("DAGetProperties [/s][/a] <server URL> <address space element id>\n\n"));

	if (argc < 3)
	{
		return -1;
	}

	OTCObjectHandle sessionHandle;
	LPTSTR url = NULL;
	LPTSTR addressSpaceElementID = NULL;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
	executionOptions.m_executionContext = 0;

	if (argc == 3)
	{
		url = argv[1];
		addressSpaceElementID = argv[2];
	}
	else if (argc > 3)
	{
		if (_tcsicmp(argv[1], _T("/a")) == 0)
		{
			executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
		}

		url = argv[2];
		addressSpaceElementID = argv[3];
	}

	OTActivate(OTC_FEATURE_DA_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		OTCAddDASession(url, 0, &sessionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			_tprintf(_T("Connected to server: %s\n\n"), url);
			OTCGetDAPropertiesOptions propOptions;
			DWORD propertyDataCount = 0;
			OTCDAPropertyData* pPropertyData = NULL;
			LONG result;
			memset(&propOptions, 0, sizeof(OTCGetDAPropertiesOptions));
			propOptions.m_whatPropertyData = OTC_DAPROPERTYDATA_ALL;
			_tprintf(_T("Get properties of address space element:  %s\n"), addressSpaceElementID);

			if (SUCCEEDED(result = OTCGetDAProperties(sessionHandle, NULL, addressSpaceElementID, NULL, &propOptions, &propertyDataCount, &pPropertyData, &executionOptions)))
			{
				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					PrintProperties(propertyDataCount, pPropertyData);
					FreePropertiesMemory(propertyDataCount, pPropertyData);
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
				_tprintf(_T("Failed to get properties of address space element: %s 0x%X\n"), addressSpaceElementID, result);
			}

			_tprintf(_T("\n\nGet properties 1 and 5 of address space element:  %s\n"), addressSpaceElementID);
			DWORD propIDs1and5[2];
			propIDs1and5[0] = 1;
			propIDs1and5[1] = 5;
			propOptions.m_propertyCount = 2;
			propOptions.m_pPropertyIDs = propIDs1and5;

			if (SUCCEEDED(result = OTCGetDAProperties(sessionHandle, NULL, addressSpaceElementID, NULL, &propOptions, &propertyDataCount, &pPropertyData, &executionOptions)))
			{
				if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
				{
					PrintProperties(propertyDataCount, pPropertyData);
					FreePropertiesMemory(propertyDataCount, pPropertyData);
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
				_tprintf(_T("Failed to get properties of address space element: %s 0x%X\n"), addressSpaceElementID, result);
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


void OTCAPI_CALL OnGetDAProperties(IN DWORD executionContext, IN OTCObjectData sessionData, IN LONG result, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData)
{
	if (SUCCEEDED(result))
	{
		PrintProperties(propertyDataCount, pPropertyData);
	}
	else
	{
		_tprintf(_T("Failed to get properties of address space element: %s 0x%X\n"), addressSpaceElementID, result);
	}

	g_gotCallback = TRUE;
}

void PrintProperties(IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData)
{
	TCHAR valueStr[36];
	DWORD i;

	for (i = 0; i < propertyDataCount; i++)
	{
		OTVariantToString(pPropertyData[i].m_value.m_pValue, 35, valueStr);
		_tprintf(_T("%5.5d | %-30.30s | %-25.25s | %-35.35s\n"), pPropertyData[i].m_ID, pPropertyData[i].m_itemID, pPropertyData[i].m_description, valueStr);
	}
}

void FreePropertiesMemory(IN DWORD propertyDataCount, IN OTCDAPropertyData* pPropertyData)
{
	DWORD i;

	for (i = 0; i < propertyDataCount; i++)
	{
		if (pPropertyData[i].m_itemID)
		{
			OTFreeMemory(pPropertyData[i].m_itemID);
		}

		if (pPropertyData[i].m_itemPath)
		{
			OTFreeMemory(pPropertyData[i].m_itemPath);
		}

		if (pPropertyData[i].m_description)
		{
			OTFreeMemory(pPropertyData[i].m_description);
		}

		if (pPropertyData[i].m_name)
		{
			OTFreeMemory(pPropertyData[i].m_name);
		}

		::VariantClear(pPropertyData[i].m_value.m_pValue);
		OTFreeMemory(pPropertyData[i].m_value.m_pValue);
	}

	if (pPropertyData)
	{
		OTFreeMemory(pPropertyData);
	}
}
