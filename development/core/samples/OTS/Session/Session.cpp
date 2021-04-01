#include <Windows.h>
#include <TCHAR.h>
#include <stdio.h>
#include <math.h>
#include "OTServer.h"

// callback functions
LONG OTSAPI_CALL Shutdown(void);
LONG OTSAPI_CALL HandleReadRequests(IN LONG count, IN OTSRequestData* paRequests);
LONG OTSAPI_CALL HandleWriteRequests(IN LONG count, IN OTSRequestData* paRequests, IN OTSValueData* pValues);
LONG OTSAPI_CALL ChangeSessionState(IN OTSObjectHandle hSession, IN OTSSessionData* pSessionStateData);
LONG OTSAPI_CALL QueryCacheValue(IN OTSObjectHandle hSession, IN OTSObjectData objectData, OUT OTSValueData* pValue);
LONG OTSAPI_CALL ChangeItems(IN LONG itemCnt, IN OTSItemData* pItemData);

// local used functions
void BuildNamespace(void);
void ClearConsole(void);
void PrintSessionsAndItems(void);

// global data
OTSInitData g_initData =
{
	419,                                                  // used OTS version
	OTS_APPTYPE_OUTPROC,                                  // application type of OPC server
	NULL,                                                 // name of service; if no service NULL
	_T("{1257ABB2-52EF-4833-87EA-9836AC79D298}"),         // class id of DA server
	_T("Softing.OPCToolboxCSmpl.Session.DA"),              // description of OPC DA Server
	_T("Softing.OPCToolboxCSmpl.Session.DA.1"),            // version independent ProgId of OPC DA Server
	NULL,                                                 // class id of AE server
	NULL,                                                 // description of OPC AE Server
	NULL,                                                 // version independent ProgId of OPC AE Server
	_T("Softing OPC Toolkit C Session Server Sample"),     // server description
	8081,                                                 // IP port number of HTTP server
	_T("/OPC/DA"),                                        // URL of XML-DA server
	1, 0, 1,                                              // major version number, minor version number and build numberof the server
	_T("Softing Industrial Automation GmbH"),             // version info of the server
	_T('/'),                                              // delimiter sign for namespace levels in the item id
	30000,                                                // period in ms to check the notification connections to the clients
	_T('.'),                                              // delimiter sign for DA properties in the item id
	100,                                                  // minimal update rate for a group (ms)
	FALSE                                                 // support to enable or disable conditions
};

OTSCallbackFunctions g_callbackFunctions =
{
	NULL,
	&Shutdown,
	&HandleReadRequests,
	&HandleWriteRequests,
	NULL,
	NULL,
	NULL,
	&ChangeSessionState,
	&QueryCacheValue,
	&ChangeItems,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

HANDLE g_endEvent;

#define OBJ_COUNT                   2
#define OBJ_IDX_TAG_SESSION_SPECIFIC 0
#define OBJ_IDX_TAG_SECURED_WRITE   1

DWORD g_objHandles[OBJ_COUNT];


#define MAX_SESSION_COUNT  10

DWORD g_clientHandle[MAX_SESSION_COUNT];
BYTE g_clientType[MAX_SESSION_COUNT];
DWORD g_curSessionCnt = 0;

DWORD g_clientInControl = 0;


LONG g_objUpdateRate[OBJ_COUNT];
BOOL g_objActive[OBJ_COUNT];

CRITICAL_SECTION g_cs;

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
	DWORD i;

	for (i = 0; i < OBJ_COUNT; i++)
	{
		g_objUpdateRate[i] = -1;
		g_objActive[i] = FALSE;
		g_objHandles[i] = 0;
	}

	InitializeCriticalSection(&g_cs);
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(controlHandler, TRUE);
	OTActivate(OTS_FEATURE_DA_SERVER, _T("SAMPLE"));
	OTSAdvise(&g_callbackFunctions);

	if (SUCCEEDED(OTSInitialize(&g_initData)))
	{
		if (S_OK == OTSProcessCommandLine(GetCommandLine()))
		{
			if (SUCCEEDED(OTSStart()))
			{
				BuildNamespace();
				OTSReady();
				PrintSessionsAndItems();
				WaitForSingleObject(g_endEvent, INFINITE);
				OTSStop();
			}
		}

		for (i = 0; i < OBJ_COUNT; i++)
		{
			g_objHandles[i] = 0;
		}

		OTSTerminate();
	}

	CloseHandle(g_endEvent);
	DeleteCriticalSection(&g_cs);
	return 0;
}


void BuildNamespace(void)
{
	OTSAddressSpaceElementData data;
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("client specific");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_POLL_OWNCACHE;
	data.m_userData = OBJ_IDX_TAG_SESSION_SPECIFIC;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objHandles[OBJ_IDX_TAG_SESSION_SPECIFIC]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("secured write");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_POLL;
	data.m_userData = OBJ_IDX_TAG_SECURED_WRITE;
	data.m_hasChildren = FALSE;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objHandles[OBJ_IDX_TAG_SECURED_WRITE]);
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
		if (paRequests[i].m_object.m_userData != OBJ_IDX_TAG_SESSION_SPECIFIC)
		{
			OTSGetCacheValue(paRequests[i].m_object.m_objectHandle, &value);
			OTSCompleteRequests(1, &paRequests[i], &result, &value);
		}
		else
		{
			// client specific data
			DWORD idx;

			for (idx = 0; idx < g_curSessionCnt; idx++)
			{
				if (g_clientHandle[idx] == paRequests[i].m_clientHandle)
				{
					break;
				}
			}

			value.m_timestamp.dwHighDateTime = 0;
			value.m_timestamp.dwLowDateTime = 0;
			value.m_quality = OT_QUALITY_GOOD;
			value.m_pValue->vt = VT_UI4;
			value.m_pValue->ulVal = (DWORD)idx;
			OTSCompleteRequests(1, &paRequests[i], &result, &value);
		}

		::VariantClear(&v);
	}

	return S_OK;
}

LONG OTSAPI_CALL HandleWriteRequests(IN LONG count, IN OTSRequestData* paRequests, IN OTSValueData* pValues)
{
	LONG i;
	LONG result;

	for (i = 0; i < count; i++)
	{
		if (paRequests[i].m_object.m_userData != OBJ_IDX_TAG_SECURED_WRITE)
		{
			result = S_OK;
		}
		else
		{
			// secured write
			if (paRequests[i].m_clientHandle == g_clientInControl)
			{
				result = S_OK;
			}
			else
			{
				result = E_ACCESSDENIED;
			}
		}

		OTSCompleteRequests(1, &paRequests[i], &result, NULL);

		if (SUCCEEDED(result)) // write succeeded -> update cache
		{
			OTSValuesChanged(1, &paRequests[i].m_object.m_objectHandle, &pValues[i]);
		}
	}

	return S_OK;
}

LONG OTSAPI_CALL ChangeSessionState(IN OTSObjectHandle hSession, IN OTSSessionData* pSessionStateData)
{
	LONG res = S_OK;
	EnterCriticalSection(&g_cs);

	if (pSessionStateData->m_state == OTS_SESSIONSTATE_CREATE)
	{
		g_clientHandle[g_curSessionCnt] = hSession;
		g_clientType[g_curSessionCnt] = pSessionStateData->m_type;
		g_curSessionCnt++;
	}
	else if (pSessionStateData->m_state == OTS_SESSIONSTATE_DESTROY)
	{
		DWORD idx;

		for (idx = 0; idx < g_curSessionCnt; idx++)
		{
			if (g_clientHandle[idx] == hSession)
			{
				break;
			}
		}

		if (g_clientInControl == hSession)
		{
			g_clientInControl = NULL;
		}

		if (idx > g_curSessionCnt)
		{
			LeaveCriticalSection(&g_cs);
			return E_FAIL; // invalid handle
		}

		if (idx < g_curSessionCnt)
		{
			memmove(&g_clientHandle[idx], &g_clientHandle[idx + 1], (g_curSessionCnt - idx) * sizeof(DWORD));
		}

		g_curSessionCnt--;
	}
	else if (pSessionStateData->m_state == OTS_SESSIONSTATE_LOGON)
	{
		if ((_tcscmp(pSessionStateData->m_userName, _T("OTS")) == 0) &&
			(_tcscmp(pSessionStateData->m_password, _T("ots")) == 0))
		{
			if ((g_clientInControl == NULL) || (g_clientInControl == hSession))
			{
				g_clientInControl = hSession;
			}
			else
			{
				res = E_ACCESSDENIED;
			}
		}
		else
		{
			res = E_ACCESSDENIED;
		}
	}
	else if (pSessionStateData->m_state == OTS_SESSIONSTATE_LOGOFF)
	{
		if (g_clientInControl == hSession)
		{
			g_clientInControl = NULL;
		}
	}

	PrintSessionsAndItems();
	LeaveCriticalSection(&g_cs);
	return res;
}

LONG OTSAPI_CALL QueryCacheValue(IN OTSObjectHandle hSession, IN OTSObjectData objectData, OUT OTSValueData* pValue)
{
	if (objectData.m_userData == OBJ_IDX_TAG_SESSION_SPECIFIC)
	{
		// client specific data
		DWORD idx;

		for (idx = 0; idx < g_curSessionCnt; idx++)
		{
			if (g_clientHandle[idx] == hSession)
			{
				break;
			}
		}

		CoFileTimeNow(&pValue->m_timestamp);
		pValue->m_quality = OT_QUALITY_GOOD;
		pValue->m_pValue->vt = VT_UI4;
		pValue->m_pValue->ulVal = (DWORD)idx;
		return S_OK;
	}

	return E_FAIL;
}

LONG OTSAPI_CALL ChangeItems(IN LONG itemCnt, IN OTSItemData* pItemData)
{
	LONG i;

	for (i = 0; i < itemCnt; i++)
	{
		if (pItemData[i].m_active)
		{
			g_objUpdateRate[pItemData[i].m_object.m_userData] = pItemData[i].m_sampleRate;
			g_objActive[pItemData[i].m_object.m_userData] = TRUE;
		}
		else
		{
			g_objUpdateRate[pItemData[i].m_object.m_userData] = -1;
			g_objActive[pItemData[i].m_object.m_userData] = FALSE;
		}
	}

	PrintSessionsAndItems();
	return S_OK;
}

void PrintSessionsAndItems(void)
{
	DWORD i;
	LPCTSTR type;
	LPCTSTR control;
	ClearConsole();
	_tprintf(_T("SESSIONS:\n\n"));

	for (i = 0; i < g_curSessionCnt; i++)
	{
		if (g_clientInControl != g_clientHandle[i])
		{
			control = _T("");
		}
		else
		{
			control = _T("<IN CONTROL>");
		}

		switch (g_clientType[i])
		{
		case OTS_SESSIONTYPE_DA:
			type = _T("DA");
			break;

		case OTS_SESSIONTYPE_XMLDA:
			type = _T("XML-DA");
			break;

		case OTS_SESSIONTYPE_XMLSUBSCRIPTIONS:
			type = _T("INTERNAL XML-DA");
			break;

		case OTS_SESSIONTYPE_AE:
			type = _T("AE");
			break;
		}

		_tprintf(_T("%8.8X %s  %s\n"), g_clientHandle[i], type, control);
	}

	_tprintf(_T("\n\nITEMS:\n\n"));
	OTSAddressSpaceElementData data;

	for (i = 0; i < OBJ_COUNT; i++)
	{
		if (g_objHandles[i] != 0)
		{
			if (SUCCEEDED(OTSGetAddressSpaceElementData(g_objHandles[i], &data)))
			{
				_tprintf(_T("%-40.40s  %d  %ld\n"), data.m_itemID, g_objActive[i], g_objUpdateRate[i]);

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
	}

	_tprintf(_T("\n\nPress Ctrl-C to exit\n"));
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
