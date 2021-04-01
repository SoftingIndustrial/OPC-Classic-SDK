#include <Windows.h>
#include <TCHAR.h>
#include <malloc.h>
#include <stdio.h>
#include "OTServer.h"

// callback functions
LONG OTSAPI_CALL Shutdown(void);
LONG OTSAPI_CALL HandleReadRequests(IN LONG count, IN OTSRequestData* paRequests);
LONG OTSAPI_CALL HandleWriteRequests(IN LONG count, IN OTSRequestData* paRequests, IN OTSValueData* pValues);
LONG OTSAPI_CALL QueryAddressSpaceElementData(IN LPTSTR itemID, IN BYTE elementType, OUT OTSAddressSpaceElementData* pData);
LONG OTSAPI_CALL QueryAddressSpaceElementChildren(IN LPTSTR itemID, IN BYTE elementType, OUT DWORD* pCount, OUT OTSAddressSpaceElementData** pElementData);
LONG OTSAPI_CALL QueryProperties(IN OTSObjectData* pObject, IN LPTSTR objItemId, IN LONG propID, OUT DWORD* pPropCount, OUT OTSPropertyData** ppPropData);
LONG OTSAPI_CALL CreateAddressSpaceElement(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OTSAddressSpaceElementData* pDataIn, OUT OTSAddressSpaceElementData* pDataOut);
LONG OTSAPI_CALL DestroyAddressSpaceElement(IN OTSObjectData objectData);

// local used functions
void BuildNamespace(void);
void BuildEventCategories(void);
void FireEvent(void);
void ClearConsole(void);
void PrintChildren(OTSObjectHandle hObj, BYTE elementType, int indent);
void Reconfigure(void);
void SetString(LPTSTR* pStrD, LPCTSTR strS);

#define CAT_ID_TIME_TICK         1

// global data
OTSInitData g_initData =
{
	419,                                                    // used OTS version
	OTS_APPTYPE_OUTPROC,                                    // application type of OPC server
	NULL,                                                   // name of service; if no service NULL
	_T("{3E0289BA-34CA-445b-9EC1-B59F3F482683}"),           // class id of DA server
	_T("Softing.OPCToolboxCSmpl.Namespace.DA"),             // description of OPC DA Server
	_T("Softing.OPCToolboxCSmpl.Namespace.DA.1"),           // version independent ProgId of OPC DA Server
	_T("{58A7AB5F-0C20-4961-9962-389C8C85273B}"),           // class id of AE server
	_T("Softing.OPCToolboxCSmpl.Namespace.AE"),             // description of OPC AE Server
	_T("Softing.OPCToolboxCSmpl.Namespace.AE.1"),           // version independent ProgId of OPC AE Server
	_T("Softing OPC Toolkit C Namespace Server Sample"),    // server description
	8081,                                                   // IP port number of HTTP server
	_T("/OPC/DA"),                                          // URL of XML-DA server
	1, 0, 1,                                                // major version number, minor version number and build numberof the server
	_T("Softing Industrial Automation GmbH"),               // version info of the server
	_T('.'),                                                // delimiter sign for namespace levels in the item id
	30000,                                                  // period in ms to check the notification connections to the clients
	_T('~'),                                                // delimiter sign for DA properties in the item id
	100,                                                    // minimal update rate for a group (ms)
	FALSE                                                   // support to enable or disable conditions
};

OTSCallbackFunctions g_callbackFunctions =
{
	NULL,
	&Shutdown,
	&HandleReadRequests,
	&HandleWriteRequests,
	&QueryProperties,
	&QueryAddressSpaceElementData,
	&QueryAddressSpaceElementChildren,
	NULL,
	NULL,
	NULL,
	CreateAddressSpaceElement,
	DestroyAddressSpaceElement,
	NULL,
	NULL,
	NULL,
	NULL
};

HANDLE g_endEvent;

OTSObjectHandle g_objBasedHandle;
OTSObjectHandle g_reconfigHandles[3];


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


int _tmain(int argc, _TCHAR* argv[])
{
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(controlHandler, TRUE);
	OTActivate(OTS_FEATURE_DA_SERVER, _T("SAMPLE"));
	OTActivate(OTS_FEATURE_AE_SERVER, _T("SAMPLE"));
	OTSAdvise(&g_callbackFunctions);

	if (SUCCEEDED(OTSInitialize(&g_initData)))
	{
		if (S_OK == OTSProcessCommandLine(GetCommandLine()))
		{
			if (SUCCEEDED(OTSStart()))
			{
				BuildEventCategories();
				BuildNamespace();
				OTSReady();
				BOOL end = FALSE;
				DWORD waitRes;
				DWORD loopCnt = 0;

				while (!end)
				{
					// print the namespace
					ClearConsole();

					if (loopCnt == 30)
					{
						Reconfigure();
						loopCnt = 0;
					}
					else
					{
						loopCnt++;
					}

					_tprintf("CURRENT NAMESPACE\n\n");
					_tprintf("DA\n\n");
					PrintChildren(0, OT_ADDRESSSPACEELEMENT_TYPE_DA, 0);
					_tprintf("\n\n\nAE\n\n");
					PrintChildren(0, OT_ADDRESSSPACEELEMENT_TYPE_AE, 0);
					FireEvent();
					_tprintf(_T("\n\nPress Ctrl-C to exit\n"));
					waitRes = WaitForSingleObject(g_endEvent, 1000);

					if (waitRes == WAIT_OBJECT_0)
					{
						end = TRUE;
					}
				}

				OTSStop();
			}
		}

		OTSTerminate();
	}

	CloseHandle(g_endEvent);
	return 0;
}


void FireEvent(void)
{
	SYSTEMTIME now;
	OTSEventData event;
	OTSVariant time;
	WCHAR timeSZ[20];
	GetLocalTime(&now);
	time.vt = VT_BSTR;
	swprintf(timeSZ, L"%2.2d:%2.2d:%2.2d", now.wHour, now.wMinute, now.wSecond);
	time.bstrVal = SysAllocString(timeSZ);
	event.m_eventType = OT_EVENTTYPE_SIMPLE;
	event.m_eventCategory = CAT_ID_TIME_TICK;
	event.m_severity = 50;
	event.m_sourcePath = _T("objectBased.time");
	event.m_message = _T("Time tick");
	memset(&event.m_occurenceTime, 0, sizeof(OTSDateTime));
	event.m_eventAttrCount = 1;
	event.m_pEventAttrs = &time;
	OTSFireEvents(1, &event);
	::VariantClear(&time);
}

void BuildEventCategories(void)
{
	DWORD hCategory;
	OTSAddEventCategory(CAT_ID_TIME_TICK, _T("time tick"), OT_EVENTTYPE_SIMPLE, &hCategory);
	OTSAddEventAttribute(hCategory, 1, _T("time"), VT_BSTR);
}

void BuildNamespace(void)
{
	OTSAddressSpaceElementData data;
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("objectBased");
	data.m_ioMode = OTS_IOMODE_NONE;
	data.m_userData = 1;
	data.m_hasChildren = TRUE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objBasedHandle);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("T1");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_I1;
	data.m_ioMode = OTS_IOMODE_POLL;
	data.m_userData = 2;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, NULL);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("T2");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI1;
	data.m_ioMode = OTS_IOMODE_POLL;
	data.m_userData = 2;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, NULL);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("T3");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI1 | VT_ARRAY;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 2;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, NULL);
	TCHAR name[20];
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 3;
	data.m_hasChildren = TRUE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, &g_reconfigHandles[0]);
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 4;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, &g_reconfigHandles[1]);
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 5;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_reconfigHandles[0], &data, &g_reconfigHandles[2]);
	OTSObjectHandle objBasedAEHandle;
	OTSObjectHandle objBasedTimeAEHandle;
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	data.m_name = _T("objectBased");
	data.m_userData = 101;
	data.m_hasChildren = TRUE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &objBasedAEHandle);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	data.m_name = _T("clock");
	data.m_userData = 102;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(objBasedAEHandle, &data, &objBasedTimeAEHandle);
}


LONG OTSAPI_CALL Shutdown(void)
{
	SetEvent(g_endEvent);
	return S_OK;
}

LONG OTSAPI_CALL HandleReadRequests(IN LONG count, IN OTSRequestData* paRequests)
{
	LONG i;
	OTSValueData value;
	VARIANT v;
	LONG result = S_OK;
	::VariantInit(&v);
	value.m_pValue = &v;

	for (i = 0; i < count; i++)
	{
		if (paRequests[i].m_propertyID == 0)
		{
			// value
			OTSGetCacheValue(paRequests[i].m_object.m_objectHandle, &value);
			OTSCompleteRequests(1, &paRequests[i], &result, &value);
		}
		else
		{
			// property
			if (paRequests[i].m_propertyID == 5555)
			{
				value.m_pValue->vt = VT_UI2;
				value.m_pValue->uiVal = (SHORT)paRequests[i].m_object.m_userData;
				value.m_timestamp.dwHighDateTime = 0;
				value.m_timestamp.dwLowDateTime = 0;
				value.m_quality = OT_QUALITY_GOOD;
				OTSCompleteRequests(1, &paRequests[i], &result, &value);
			}
		}

		::VariantClear(&v);
	}

	return S_OK;
}


LONG OTSAPI_CALL HandleWriteRequests(IN LONG count, IN OTSRequestData* paRequests, IN OTSValueData* pValues)
{
	LONG i;
	LONG result = S_OK;

	for (i = 0; i < count; i++)
	{
		OTSCompleteRequests(1, &paRequests[i], &result, NULL);
		// write succeeded -> update cache
		OTSValuesChanged(1, &paRequests[i].m_object.m_objectHandle, &pValues[i]);
	}

	return S_OK;
}

LONG OTSAPI_CALL QueryAddressSpaceElementData(IN LPTSTR itemID, IN BYTE elementType, OUT OTSAddressSpaceElementData* pData)
{
	memset(pData, 0, sizeof(OTSAddressSpaceElementData));

	if (itemID == NULL)
	{
		return E_INVALIDARG;
	}

	if (elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
	{
		if (_tcsncmp(itemID, _T("stringBased"), 11) == 0)
		{
			// string based
			if (_tcslen(itemID) == 11)
			{
				SetString(&pData->m_name, _T("stringBased"));
				pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				pData->m_hasChildren = TRUE;
				pData->m_ioMode = OTS_IOMODE_NONE;
				pData->m_userData = 20;
				return S_OK;
			}
			else
			{
				LPCTSTR str = itemID + 12;
				LONG strLen = (LONG)_tcslen(str);

				if (strLen == 2)
				{
					if ((_tcscmp(str, _T("N1")) == 0) ||
						(_tcscmp(str, _T("N2")) == 0) ||
						(_tcscmp(str, _T("T1")) == 0))
					{
						pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
						SetString(&pData->m_name, str);
						pData->m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
						pData->m_ioMode = OTS_IOMODE_REPORT;

						if (str[0] == _T('N'))
						{
							pData->m_datatype = VT_UI1 | VT_ARRAY;
							pData->m_hasChildren = TRUE;
							pData->m_userData = 21;
						}
						else
						{
							pData->m_datatype = VT_UI1;
							pData->m_hasChildren = FALSE;
							pData->m_userData = 22;
						}

						return S_OK;
					}
				}
				else if (strLen == 5)
				{
					if ((_tcscmp(str, _T("N1.T1")) == 0) ||
						(_tcscmp(str, _T("N2.T1")) == 0) ||
						(_tcscmp(str, _T("N2.T2")) == 0))
					{
						pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
						SetString(&pData->m_name, str + 3);
						pData->m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
						pData->m_ioMode = OTS_IOMODE_REPORT;
						pData->m_datatype = VT_UI1;
						pData->m_hasChildren = FALSE;
						pData->m_userData = 23;
					}
				}

				return S_OK;
			}
		}
		else
		{
			if (_tcscmp(itemID, _T("syntaxBased")) == 0)
			{
				SetString(&pData->m_name, _T("syntaxBased"));
				pData->m_hasChildren = TRUE;
				pData->m_ioMode = OTS_IOMODE_NONE;
				pData->m_userData = 40;
				return S_OK;
			}

			if (_tcsncmp(itemID, _T("syntaxBased."), 12) == 0)
			{
				// syntax based
				LPCTSTR syn = itemID + 12;
				LONG synLen = (LONG)_tcslen(syn);
				LONG i;

				if ((synLen % 3) == 2)
				{
					for (i = 0; i < (synLen - 3); i += 3)
					{
						if (syn[i] != _T('N'))
						{
							return S_FALSE;
						}
					}

					switch (syn[i])
					{
					case _T('N'):
						pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
						SetString(&pData->m_name, syn + i);
						pData->m_hasChildren = TRUE;
						pData->m_ioMode = OTS_IOMODE_NONE;
						pData->m_userData = 41;
						break;

					case _T('T'):
						pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
						SetString(&pData->m_name, syn + i);
						pData->m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
						pData->m_datatype = VT_I4;
						pData->m_hasChildren = FALSE;
						pData->m_ioMode = OTS_IOMODE_REPORT;
						pData->m_userData = 42;
						break;

					default:
						return S_FALSE;
						break;
					}

					return S_OK;
				}
			}
		}
	}

	if (elementType == OT_ADDRESSSPACEELEMENT_TYPE_AE)
	{
		if (_tcsncmp(itemID, _T("stringBased"), 11) == 0)
		{
			// string based
			if (_tcslen(itemID) == 11)
			{
				SetString(&pData->m_name, _T("stringBased"));
				pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
				pData->m_hasChildren = TRUE;
				pData->m_userData = 120;
				return S_OK;
			}
			else
			{
				LPCTSTR str = itemID + 12;
				LONG strLen = (LONG)_tcslen(str);

				if (strLen == 5)
				{
					if (_tcscmp(str, _T("clock")) == 0)
					{
						pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
						SetString(&pData->m_name, _T("clock"));
						pData->m_hasChildren = TRUE;
						pData->m_userData = 121;
						return S_OK;
					}
				}
			}
		}
	}

	return S_FALSE;
}

LONG OTSAPI_CALL QueryAddressSpaceElementChildren(IN LPTSTR itemID, IN BYTE elementType, OUT DWORD* pCount, OUT OTSAddressSpaceElementData** pElementData)
{
	*pElementData = NULL;
	*pCount = 0;

	if (elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
	{
		if (_tcslen(itemID) == 0)
		{
			// root
			*pCount = 1;
			*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
			(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
			SetString(&((*pElementData)[0].m_name), _T("stringBased"));
			SetString(&((*pElementData)[0].m_itemID), _T("stringBased"));
			(*pElementData)[0].m_ioMode = OTS_IOMODE_NONE;
			(*pElementData)[0].m_userData = 20;
			(*pElementData)[0].m_hasChildren = TRUE;
			return S_OK;
		}
		else if (_tcsncmp(itemID, _T("stringBased"), 11) == 0)
		{
			// string based
			if (_tcscmp(itemID, _T("stringBased")) == 0)
			{
				*pCount = 3;
				*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
				(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[0].m_name), _T("N1"));
				SetString(&((*pElementData)[0].m_itemID), _T("stringBased.N1"));
				(*pElementData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[0].m_datatype = VT_UI1 | VT_ARRAY;
				(*pElementData)[0].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[0].m_userData = 21;
				(*pElementData)[0].m_hasChildren = TRUE;
				(*pElementData)[1].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[1].m_name), _T("N2"));
				SetString(&((*pElementData)[1].m_itemID), _T("stringBased.N2"));
				(*pElementData)[1].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[1].m_datatype = VT_UI1 | VT_ARRAY;
				(*pElementData)[1].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[1].m_userData = 21;
				(*pElementData)[1].m_hasChildren = TRUE;
				(*pElementData)[2].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[2].m_name), _T("T1"));
				SetString(&((*pElementData)[2].m_itemID), _T("stringBased.T1"));
				(*pElementData)[2].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[2].m_datatype = VT_UI1;
				(*pElementData)[2].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[2].m_userData = 22;
				(*pElementData)[2].m_hasChildren = FALSE;
				return S_OK;
			}
			else if (_tcscmp(itemID, _T("stringBased.N1")) == 0)
			{
				*pCount = 1;
				*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
				(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[0].m_name), _T("T1"));
				SetString(&((*pElementData)[0].m_itemID), _T("stringBased.N1.T1"));
				(*pElementData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[0].m_datatype = VT_UI1;
				(*pElementData)[0].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[0].m_userData = 23;
				(*pElementData)[0].m_hasChildren = FALSE;
				return S_OK;
			}
			else if (_tcscmp(itemID, _T("stringBased.N2")) == 0)
			{
				*pCount = 2;
				*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
				(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[0].m_name), _T("T1"));
				SetString(&((*pElementData)[0].m_itemID), _T("stringBased.N2.T1"));
				(*pElementData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[0].m_datatype = VT_UI1;
				(*pElementData)[0].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[0].m_userData = 23;
				(*pElementData)[0].m_hasChildren = FALSE;
				(*pElementData)[1].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
				SetString(&((*pElementData)[1].m_name), _T("T2"));
				SetString(&((*pElementData)[1].m_itemID), _T("stringBased.N2.T2"));
				(*pElementData)[1].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				(*pElementData)[1].m_datatype = VT_UI1;
				(*pElementData)[1].m_ioMode = OTS_IOMODE_REPORT;
				(*pElementData)[1].m_userData = 23;
				(*pElementData)[1].m_hasChildren = FALSE;
				return S_OK;
			}
		}
	}
	else if (elementType == OT_ADDRESSSPACEELEMENT_TYPE_AE)
	{
		if (_tcslen(itemID) == 0)
		{
			// root
			*pCount = 1;
			*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
			(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
			SetString(&((*pElementData)[0].m_name), _T("stringBased"));
			(*pElementData)[0].m_userData = 120;
			(*pElementData)[0].m_hasChildren = TRUE;
			return S_OK;
		}
		else if (_tcsncmp(itemID, _T("stringBased"), 11) == 0)
		{
			// string based
			if (_tcscmp(itemID, _T("stringBased")) == 0)
			{
				*pCount = 1;
				*pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData) * (*pCount));
				(*pElementData)[0].m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
				SetString(&((*pElementData)[0].m_name), _T("clock"));
				(*pElementData)[0].m_userData = 121;
				(*pElementData)[0].m_hasChildren = FALSE;
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

LONG OTSAPI_CALL QueryProperties(
	IN OTSObjectData* pObject,
	IN LPTSTR objItemId,
	IN LONG propID,
	OUT DWORD* pPropCount, OUT OTSPropertyData** ppPropData)
{
	LPTSTR szItemID;
	LONG ret = S_FALSE;
	*ppPropData = NULL;
	*pPropCount = 0;

	if ((propID == 0) || (propID == 5555))
	{
		*ppPropData = (OTSPropertyData*)OTAllocateMemory(sizeof(OTSPropertyData) * 1);
		*pPropCount = 1;
		(*ppPropData)[0].m_pid = 101;
		szItemID = (TCHAR*)malloc((_tcslen(objItemId) + 11 + 1) * sizeof(TCHAR));
		(*ppPropData)[0].m_pid = 5555;
		_stprintf(szItemID, _T("%s~objectType"), objItemId);
		SetString(&((*ppPropData)[0].m_itemID), szItemID);
		SetString(&((*ppPropData)[0].m_name), _T("type"));
		SetString(&((*ppPropData)[0].m_descr), _T("Object Type"));
		(*ppPropData)[0].m_datatype = VT_UI2;
		(*ppPropData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE;
		ret = S_OK;
		free(szItemID);
	}

	return ret;
}

LONG OTSAPI_CALL CreateAddressSpaceElement(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OUT OTSAddressSpaceElementData* pData, OUT OTSAddressSpaceElementData* pDataOut)
{
	ClearConsole();
	_tprintf("CURRENT NAMESPACE\n\n");
	PrintChildren(0, pData->m_elementType, 0);
	_tprintf(_T("\n\nPress Ctrl-C to exit\n"));
	return S_OK;
}

LONG OTSAPI_CALL DestroyAddressSpaceElement(IN OTSObjectData objectData)
{
	return S_OK;
}

void PrintChildren(OTSObjectHandle hObj, BYTE elementType, int indent)
{
	OTSObjectData* pChildrenData;
	OTSAddressSpaceElementData data;
	LONG cnt = 0;
	LONG i;
	TCHAR indentS[41];
	OTSObjectData parent;
	memset(indentS, 32, indent * sizeof(TCHAR));
	indentS[indent] = _T('\0');

	if (SUCCEEDED(OTSGetChildren(hObj, elementType, &cnt, &pChildrenData)))
	{
		for (i = 0; i < cnt; i++)
		{
			memset(&data, 0, sizeof(OTSAddressSpaceElementData));

			if (SUCCEEDED(OTSGetAddressSpaceElementData(pChildrenData[i].m_objectHandle, &data)))
			{
				OTSGetParent(pChildrenData[i].m_objectHandle, &parent);
				_tcsncpy(indentS + indent, data.m_name, 40 - indent);

				if (elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
				{
					_tprintf(_T("%-40.40s[%4.4ld|%d|%d|%8.8X](%8.8X|%8.8X)\n"), indentS, data.m_datatype, (int)data.m_accessRights, (int)data.m_ioMode, data.m_userData, pChildrenData[i].m_objectHandle, parent.m_objectHandle);
				}
				else
				{
					_tprintf(_T("%-40.40s[%8.8X](%8.8X|%8.8X)\n"), indentS, data.m_userData, pChildrenData[i].m_objectHandle, parent.m_objectHandle);
				}

				PrintChildren(pChildrenData[i].m_objectHandle, elementType, indent + 2);

				if (data.m_name)
				{
					OTFreeMemory(data.m_name);
				}

				if (data.m_itemID)
				{
					OTFreeMemory(data.m_itemID);
				}
			}
		}

		OTFreeMemory(pChildrenData);
	}
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

void Reconfigure(void)
{
	LONG i;

	for (i = 0; i < 3; i++)
	{
		OTSRemoveAddressSpaceElement(g_reconfigHandles[i]);
	}

	OTSAddressSpaceElementData data;
	TCHAR name[20];
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 3;
	data.m_hasChildren = TRUE;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, &g_reconfigHandles[0]);
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 4;
	data.m_hasChildren = FALSE;
	OTSAddAddressSpaceElement(g_objBasedHandle, &data, &g_reconfigHandles[1]);
	_stprintf(name, _T("R%d"), rand());
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = name;
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = 5;
	data.m_hasChildren = FALSE;
	OTSAddAddressSpaceElement(g_reconfigHandles[0], &data, &g_reconfigHandles[2]);
}

void SetString(LPTSTR* pStrD, LPCTSTR strS)
{
	*pStrD = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(strS) + sizeof(TCHAR)));
	_tcscpy(*pStrD, strS);
}
