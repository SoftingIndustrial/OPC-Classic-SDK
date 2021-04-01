#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "OTClient.h"


// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

BOOL g_gotCallback = FALSE;

void OTCAPI_CALL OnBrowseServer(IN DWORD executionContext, IN LONG result, IN LPTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData, IN DWORD serverDataCount, IN OTCServerData* pServerData);

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
	&OnBrowseServer
};

int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("OPCEnumerate [/s][/a][<Hostname or IP address>]\n"));
	_tprintf(_T("\t/s : synchonous execution of the OCT functions (default)\n"));
	_tprintf(_T("\t/a : asynchonous execution of the OCT functions\n"));
	OTActivate(OTC_FEATURE_DA_CLIENT, _T("SAMPLE"));

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbackFunctions);
		LONG res;
		LPTSTR ipAddress = NULL;
		OTCExecutionOptions executionOptions;
		executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
		executionOptions.m_executionContext = 0;

		if (argc > 1)
		{
			if (_tcsicmp(argv[1], _T("/a")) == 0)
			{
				executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
			}
			else if (_tcsicmp(argv[1], _T("/s")) == 0)
			{
				executionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			}
			else
			{
				ipAddress = argv[1];
			}

			if ((ipAddress == NULL) && (argc > 2))
			{
				ipAddress = argv[1];
			}
		}

		if (executionOptions.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS)
		{
			OTCServerData* pServerData;
			DWORD serverDataCount;
			DWORD i;
			TCHAR opcSpecs[32];
			_tprintf(_T("SYNCHRONOUS\n"));
			res = OTCBrowseServer(ipAddress, OT_OPCSPECIFICATION_ALL, OTC_SERVERDATA_ALL, &serverDataCount, &pServerData, &executionOptions);

			if (SUCCEEDED(res))
			{
				for (i = 0; i < serverDataCount; i++)
				{
					opcSpecs[0] = 0;

					if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA10)
					{
						_tcscat(opcSpecs, _T("DA1 "));
					}

					if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA20)
					{
						_tcscat(opcSpecs, _T("DA2 "));
					}

					if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA30)
					{
						_tcscat(opcSpecs, _T("DA3 "));
					}

					if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_AE10)
					{
						_tcscat(opcSpecs, _T("AE "));
					}

					_tprintf(_T("%s\n\tOPC Specifications: %s\n\tURL: %s\n\tCLSID: %s\n\tProgID: %s\n\tvProgID: %s\n\n"), pServerData[i].m_description, opcSpecs, pServerData[i].m_url, pServerData[i].m_clsid, pServerData[i].m_progId, pServerData[i].m_vProgId);
					OTFreeMemory(pServerData[i].m_clsid);
					OTFreeMemory(pServerData[i].m_progId);
					OTFreeMemory(pServerData[i].m_vProgId);
					OTFreeMemory(pServerData[i].m_description);
					OTFreeMemory(pServerData[i].m_url);
				}

				if (pServerData)
				{
					OTFreeMemory(pServerData);
				}
			}
			else
			{
				_tprintf(_T("Failed to browse server : result 0x%X"), res);
			}
		}
		else
		{
			_tprintf(_T("ASYNCHRONOUS\n"));
			res = OTCBrowseServer(ipAddress, OT_OPCSPECIFICATION_ALL, OTC_SERVERDATA_ALL, NULL, NULL, &executionOptions);

			while (!g_gotCallback)
			{
				Sleep(10);
			}
		}

		OTCTerminate();
	}

	return 0;
}


void OTCAPI_CALL OnBrowseServer(IN DWORD executionContext, IN LONG res, IN LPTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData, IN DWORD serverDataCount, IN OTCServerData* pServerData)
{
	DWORD i;
	TCHAR opcSpecs[32];

	if (SUCCEEDED(res))
	{
		for (i = 0; i < serverDataCount; i++)
		{
			opcSpecs[0] = 0;

			if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA10)
			{
				_tcscat(opcSpecs, _T("DA1 "));
			}

			if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA20)
			{
				_tcscat(opcSpecs, _T("DA2 "));
			}

			if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_DA30)
			{
				_tcscat(opcSpecs, _T("DA3 "));
			}

			if (pServerData[i].m_opcSpecifications & OT_OPCSPECIFICATION_AE10)
			{
				_tcscat(opcSpecs, _T("AE "));
			}

			_tprintf(_T("%s\n\tOPC Specifications: %s\n\tURL: %s\n\tCLSID: %s\n\tProgID: %s\n\tvProgID: %s\n\n"), pServerData[i].m_description, opcSpecs, pServerData[i].m_url, pServerData[i].m_clsid, pServerData[i].m_progId, pServerData[i].m_vProgId);
		}
	}
	else
	{
		_tprintf(_T("Failed to browse server : result 0x%X"), res);
	}

	g_gotCallback = TRUE;
}
