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
//  Filename    : AEBrowse.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Browse an AE Server's address space example                  |
//                                                                            |
//-----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"

void BrowseAddressSpace(OTCObjectHandle sessionHandle, OTCObjectHandle addressSpaceElementHandle, DWORD indent);
void PrintChildren(OTCAddressSpaceElementData* pAddressSpaceElement, DWORD indent);

// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};


int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("AEBrowse <server URL>\n\n"));

	if (argc < 2)
	{
		return -1;
	}

	OTCObjectHandle sessionHandle;
	OTActivate(OTC_FEATURE_AE_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAddAESession(argv[1], 0, &sessionHandle);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, TRUE);

		if (S_OK == OTCPerformStateTransition(sessionHandle, TRUE, NULL))
		{
			_tprintf(_T("Connected to server: %s\n\n"), argv[1]);
			BrowseAddressSpace(sessionHandle, 0, 0);
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

void PrintChildren(OTCAddressSpaceElementData* pAddressSpaceElement, DWORD indent)
{
	TCHAR szNameAndIndent[41];
	LPCTSTR szType;

	switch (pAddressSpaceElement->m_type)
	{
	case OT_ADDRESSSPACETYPE_BRANCH:
		szType = _T("B ");
		break;

	case OT_ADDRESSSPACETYPE_LEAF:
		szType = _T("L");
		break;
	}

	memset(szNameAndIndent, _T(' '), indent * sizeof(TCHAR));
	_tcsncpy(szNameAndIndent + indent, pAddressSpaceElement->m_name, 40 - indent);
	szNameAndIndent[40] = _T('\0');
	_tprintf(_T("%-40.40s | %1.1s\n"), szNameAndIndent, szType);
}
