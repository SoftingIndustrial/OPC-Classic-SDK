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
//  Filename    : DABrowse.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Browse a DA Server's address space example                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

void BrowseAddressSpace(OTCObjectHandle sessionHandle, OTCObjectHandle addressSpaceElementHandle, DWORD indent);
void BrowseAddressSpaceElement(IN OTCObjectHandle sessionHandle, IN LPTSTR addessSpaceElementID);
void PrintChildren(OTCAddressSpaceElementData* pAddressSpaceElement, DWORD indent);
void OTCAPI_CALL OnBrowseAddressSpace(IN DWORD executionContext, IN OTCObjectData serverData, IN LONG result, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, IN DWORD addressSpaceElementDataCount, IN OTCAddressSpaceElementData* pAddressSpaceElement);

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
	&OnBrowseAddressSpace
};

BOOL g_gotCallback = FALSE;


int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("DABrowse <server URL> [<address space element id>]\n\n"));

	if (argc < 2)
	{
		return -1;
	}

	OTCObjectHandle sessionHandle;
	LPTSTR addressSpaceElementID = NULL;

	if (argc == 3)
	{
		addressSpaceElementID = argv[2];
	}

	OTActivate(OTC_FEATURE_DA_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAddDASession(argv[1], 0, &sessionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			_tprintf(_T("Connected to server: %s\n\n"), argv[1]);

			if (addressSpaceElementID == NULL)   // browse the whole server
			{
				BrowseAddressSpace(sessionHandle, 0, 0);
			}
			else   // browse only one node (asynchronous)
			{
				OTCAdvise(&g_callbackFunctions);
				BrowseAddressSpaceElement(sessionHandle, addressSpaceElementID);
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

void BrowseAddressSpace(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN DWORD indent)
{
	OTCAddressSpaceBrowseOptions browseOptions;
	OTCAddressSpaceElementData* pAddressSpaceElement;
	DWORD addressSpaceElementDataCount;
	DWORD i;
	// browse all child leaves
	memset(&browseOptions, 0, sizeof(OTCAddressSpaceBrowseOptions));
	browseOptions.m_type = OT_ADDRESSSPACETYPE_LEAF;
	browseOptions.m_retrieveItemID = TRUE;
	pAddressSpaceElement = NULL;

	if (SUCCEEDED(OTCBrowseAddressSpace(sessionHandle, addressSpaceElementHandle, NULL, NULL, &browseOptions,
										&addressSpaceElementDataCount, &pAddressSpaceElement, NULL)))
	{
		for (i = 0; i < addressSpaceElementDataCount; i++)
		{
			PrintChildren(&pAddressSpaceElement[i], indent);
			OTCReleaseAddressSpaceElement(pAddressSpaceElement[i].m_objectHandle, FALSE);
		}
	}

	for (i = 0; i < addressSpaceElementDataCount; i++)
	{
		OTFreeMemory(pAddressSpaceElement[i].m_name);
	}

	if (pAddressSpaceElement)
	{
		OTFreeMemory(pAddressSpaceElement);
	}

	// browse all child nodes
	browseOptions.m_type = OT_ADDRESSSPACETYPE_BRANCH;
	browseOptions.m_retrieveItemID = TRUE;
	pAddressSpaceElement = NULL;

	if (SUCCEEDED(OTCBrowseAddressSpace(sessionHandle, addressSpaceElementHandle, NULL, NULL, &browseOptions,
										&addressSpaceElementDataCount, &pAddressSpaceElement, NULL)))
	{
		for (i = 0; i < addressSpaceElementDataCount; i++)
		{
			PrintChildren(&pAddressSpaceElement[i], indent);
			BrowseAddressSpace(sessionHandle, pAddressSpaceElement[i].m_objectHandle, indent + 1);
			OTCReleaseAddressSpaceElement(pAddressSpaceElement[i].m_objectHandle, FALSE);
		}
	}

	for (i = 0; i < addressSpaceElementDataCount; i++)
	{
		OTFreeMemory(pAddressSpaceElement[i].m_name);
	}

	if (pAddressSpaceElement)
	{
		OTFreeMemory(pAddressSpaceElement);
	}
}


void BrowseAddressSpaceElement(IN OTCObjectHandle sessionHandle, IN LPTSTR addessSpaceElementID)
{
	OTCAddressSpaceBrowseOptions browseOptions;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
	executionOptions.m_executionContext = 0;
	memset(&browseOptions, 0, sizeof(OTCAddressSpaceBrowseOptions));
	browseOptions.m_type = OT_ADDRESSSPACETYPE_ALL;
	browseOptions.m_retrieveItemID = TRUE;
	_tprintf(_T("Browse address space element:  %s\n"), addessSpaceElementID);

	if (SUCCEEDED(OTCBrowseAddressSpace(sessionHandle, NULL, addessSpaceElementID, NULL, &browseOptions, NULL, NULL, &executionOptions)))
	{
		while (!g_gotCallback)
		{
			Sleep(10);
		}
	}
}

void OTCAPI_CALL OnBrowseAddressSpace(IN DWORD executionContext, IN OTCObjectData serverData, IN LONG result, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, IN DWORD addressSpaceElementDataCount, IN OTCAddressSpaceElementData* pAddressSpaceElement)
{
	DWORD i;

	if (SUCCEEDED(result))
	{
		for (i = 0; i < addressSpaceElementDataCount; i++)
		{
			PrintChildren(&pAddressSpaceElement[i], 0);
			OTCReleaseAddressSpaceElement(pAddressSpaceElement[i].m_objectHandle, FALSE);
		}
	}
	else
	{
		_tprintf(_T("Failed to browse address space element: %s 0x%X\n"), addressSpaceElementID, result);
	}

	g_gotCallback = TRUE;
}

void PrintChildren(OTCAddressSpaceElementData* pAddressSpaceElement, DWORD indent)
{
	TCHAR szNameAndIndent[41];
	LPCTSTR szType;

	switch (pAddressSpaceElement->m_type)
	{
	case OT_ADDRESSSPACETYPE_BRANCH:
		szType = _T("B ");
		break;

	case OT_ADDRESSSPACETYPE_BRANCH | OT_ADDRESSSPACETYPE_ITEM :
		szType = _T("BI");
		break;

	case OT_ADDRESSSPACETYPE_LEAF | OT_ADDRESSSPACETYPE_ITEM :
		szType = _T("LI");
		break;
	}

	memset(szNameAndIndent, _T(' '), indent * sizeof(TCHAR));
	_tcsncpy(szNameAndIndent + indent, pAddressSpaceElement->m_name, 40 - indent);
	szNameAndIndent[40] = _T('\0');
	_tprintf(_T("%-40.40s | %2.2s | %s\n"), szNameAndIndent, szType, pAddressSpaceElement->m_itemID);
}
