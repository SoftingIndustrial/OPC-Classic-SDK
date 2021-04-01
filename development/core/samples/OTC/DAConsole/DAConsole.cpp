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
//  Filename    : DAConsole.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Use DASession,DASubscription and DAItem objects example     |
//                                                                            |
//-----------------------------------------------------------------------------

// Console.cpp : Defines the entry point for the console application.
//
#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#include <TChar.h>
#include <conio.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <memory.h>
#include "OTClient.h"

#ifndef _T
#define _T(a)          a
#define _tcsncmp    strncmp
#define _tcscmp     strcmp
#define _tcslen     strlen
#define _tcscpy     strcpy
#define _stprintf   sprintf
#endif

#if defined(WIN32) || defined(WIN64)
#define OUTPUT _tprintf
#else
#define OUTPUT printf
#endif

OTChar g_objectDescription [5][64] =
{
	_T("Session                "),
	_T("Subscription           "),
	_T("Item: maths.sin        "),
	_T("Item: time.local.second"),
	_T("Item: increment.UI1    ")
};

OTChar g_stateDescription [4][16] =
{
	_T("disconnected"),
	_T("connected"),
	_T(""),
	_T("active"),
};


// global data
OTCInitData g_initData =
{
	410                    // used OTC version
};

// helper functions
OTChar* GetItemValueSZ(IN OTCValueData* pValueData);
OTChar* GetDateTimeSZ(IN OTCDateTime dateTime);
void PrintServerStatus(OTCServerStatus* pServerStatus);

// callback functions
void OTCAPI_CALL OnStateChange(IN OTCObjectContext objectContext, IN unsigned char state);
void OTCAPI_CALL OnDataChange(IN OTCObjectContext objectContext, IN unsigned long count, IN OTCObjectData* pItemData, IN OTCValueData* pValues, IN long* pResults);
void OTCAPI_CALL OnReadComplete(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long count, IN OTCObjectData* pItemData, IN OTChar * *pItemIDs, IN OTChar * *pItemPaths, IN OTCValueData* pValues, IN long* pResults);
void OTCAPI_CALL OnWriteComplete(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long count, IN OTCObjectData* pItemData, IN OTChar * *pItemIDs, IN OTChar * *pItemPaths, IN OTCValueData* pValues, IN long* pResults);
unsigned char OTCAPI_CALL OnServerShutdown(IN OTCObjectData serverData, IN OTChar* reason);
void OTCAPI_CALL OnPerfromStateTransition(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result);
void OTCAPI_CALL OnGetServerStatus(IN unsigned long executionContext, IN OTCObjectData serverData, IN long result, IN OTCServerStatus* pServerStatus);

OTCCallbackFunctions g_callbacks =
{
	NULL,
	&OnStateChange,
	&OnDataChange,
	&OnReadComplete,
	&OnWriteComplete,
	&OnPerfromStateTransition,
	&OnServerShutdown,
	NULL,
	NULL,
	NULL,
	&OnGetServerStatus
};

OTTraceData g_traceSettings =
{
	OT_TRACEMASK_ALL,                      // error level trace mask
	OT_TRACEMASK_ALL,                      // warning level trace mask
	OT_TRACEMASK_USER | OT_TRACEMASK_COPC, // info level trace mask
	OT_TRACEMASK_NOTHING,                  // error level trace mask
	false,                                 // enable trace to file
	_T("trace1.txt"),                      // first trace file
	_T("trace2.txt"),                      // second trace file
	1000000                                // maximum size of trace files
};


void usage(void)
{
	OUTPUT(_T("\nUsage:\n"));
	OUTPUT(_T("Use lower case letters for synchronous execution\n"));
	OUTPUT(_T("Use upper case letters for asynchronous execution\n"));
	OUTPUT(_T("Press \'a\' or \'A\' to activate the objects\n"));
	OUTPUT(_T("Press \'c\' or \'C\' to connect the objects\n"));
	OUTPUT(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	OUTPUT(_T("Press \'r\' or \'R\' to read the items\n"));
	OUTPUT(_T("Press \'w\' or \'W\' to write the items\n"));
	OUTPUT(_T("Press \'f\' or \'F\' to refresh the items\n"));
	OUTPUT(_T("Press \'z\' or \'Z\' to read using server object\n"));
	OUTPUT(_T("Press \'t\' or \'T\' to write using the server object\n"));
	OUTPUT(_T("Press \'s\' or \'S\' to get the server status\n"));
	OUTPUT(_T("Press \'m\' to activate the connection monitor\n"));
	OUTPUT(_T("Press \'n\' to deactivate the connection monitor\n"));
	OUTPUT(_T("Press \'?\' or \'u\' to display this usage information\n"));
	OUTPUT(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n\n"));
}

#if defined(WIN32) || defined(WIN64)
int _tmain(int argc, OTChar* argv[])
#else
int main(int argc, OTChar* argv[])
#endif
{
#ifdef OTB_USE_STATIC_LIB
	OTInitLib();
#endif
	OTCExecutionOptions executionOptions;
	OTCObjectHandle sessionHandle;
	OTCObjectHandle groupHandle;
	OTCObjectHandle itemHandles[3];
	OTCValueData values[3];
	long results[3];
	OTChar* itemIDs[2];
	bool end = false;
	unsigned long i;
	long res;

	for (i = 0; i < 3; i++)
	{
		values[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		OTVariantInit(values[i].m_pValue);
	}

	itemIDs[0] = _T("IOP.static.R8");
	itemIDs[1] = _T("IOP.static.BSTR");
	executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS;
	executionOptions.m_executionContext = 0;
	OTActivate(OTC_FEATURE_DA_CLIENT, _T("SAMPLE"));
	OTActivate(OTC_FEATURE_XMLDA_CLIENT, _T("SAMPLE"));
	OTEnableTracing(OT_TRACEDATA_ALL, &g_traceSettings);

	if (SUCCEEDED(OTCInitialize(&g_initData)))
	{
		OTCAdvise(&g_callbacks);
#if defined(WIN32) || defined(WIN64)
		OTCAddDASession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"), 0, &sessionHandle);
#else
		// TODO replace the IP address with the one of a Windows machine running the Softing OPC Demo Server
		OTCAddDASession(_T("http://192.168.42.2:8079/DA"), 0, &sessionHandle);
#endif
		OTCAddDASubscription(sessionHandle, 1000, 1, &groupHandle);
		OTCAddDAItem(groupHandle, _T("maths.sin"), 2, &itemHandles[0]);
		OTCAddDAItem(groupHandle, _T("time.local.second"), 3, &itemHandles[1]);
		OTCAddDAItem(groupHandle, _T("increment.UI1"), 4, &itemHandles[2]);
		OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, true);

		if (S_OK == OTCPerformStateTransition(sessionHandle, true, NULL))
		{
			usage();

			while (!end)
			{
				int c;
#if defined(WIN32) || defined(WIN64)
#ifndef UNDER_CE
				c = _getche();
#else
				c = getchar();
#endif
#else
				c = getchar();
#endif

				switch (c)
				{
				case _T('A'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_ACTIVE, true);
					OTCPerformStateTransition(sessionHandle, true, &executionOptions);
					executionOptions.m_executionContext++;
					break;

				case _T('a'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_ACTIVE, true);
					OTCPerformStateTransition(sessionHandle, true, NULL);
					break;

				case _T('C'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, true);
					OTCPerformStateTransition(sessionHandle, true, &executionOptions);
					executionOptions.m_executionContext++;
					break;

				case _T('c'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_CONNECTED, true);
					OTCPerformStateTransition(sessionHandle, true, NULL);
					break;

				case _T('D'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_DISCONNECTED, true);
					OTCPerformStateTransition(sessionHandle, true, &executionOptions);
					executionOptions.m_executionContext++;
					break;

				case _T('d'):
					OTCChangeTargetState(sessionHandle, OTC_STATE_DISCONNECTED, true);
					OTCPerformStateTransition(sessionHandle, true, NULL);
					break;

				case _T('R'):
					if (FAILED(res = OTCRead(groupHandle, 3, itemHandles, NULL, NULL, 0, values, results, &executionOptions)))
					{
						OUTPUT(_T("Read failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('r'):
					if (SUCCEEDED(res = OTCRead(groupHandle, 3, itemHandles, NULL, NULL, 0, values, results, NULL)))
					{
						OUTPUT(_T("Read\n"));

						for (i = 0; i < 3; i++)
						{
							if (SUCCEEDED(results[i]))
							{
								OUTPUT(_T("%-23.23s - %s\n"), g_objectDescription[i + 2], GetItemValueSZ(&values[i]));
								OTVariantClear(values[i].m_pValue);
							}
							else
							{
								OUTPUT(_T("Read failed - %s [0x%8.8lX]\n"), g_objectDescription[i + 2], results[i]);
							}
						}
					}
					else
					{
						OUTPUT(_T("Read failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('F'):
				case _T('f'):
					if (FAILED(res = OTCRefreshDAGroup(groupHandle, 0, &executionOptions)))
					{
						OUTPUT(_T("Refresh failed [0x%8.8lX]\n"), res);
					}

					executionOptions.m_executionContext++;
					break;

				case _T('Z'):
					if (FAILED(res = OTCRead(sessionHandle, 2, NULL, itemIDs, NULL, 0, NULL, NULL, &executionOptions)))
					{
						OUTPUT(_T("Read failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('z'):
					if (SUCCEEDED(res = OTCRead(sessionHandle, 2, NULL, itemIDs, NULL, 0, values, results, NULL)))
					{
						OUTPUT(_T("Read\n"));

						for (i = 0; i < 2; i++)
						{
							if (SUCCEEDED(results[i]))
							{
								OUTPUT(_T("%-23.23s - %s\n"), itemIDs[i], GetItemValueSZ(&values[i]));
								OTVariantClear(values[i].m_pValue);
							}
							else
							{
								OUTPUT(_T("Read failed - %s [0x%8.8lX]\n"), itemIDs[i], results[i]);
							}
						}
					}
					else
					{
						OUTPUT(_T("Read failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('W'):
					values[2].m_pValue->vt = VT_UI1;
					values[2].m_pValue->bVal = 19;
					values[2].m_quality = OT_QUALITY_NOT_SET;
					memset(&values[2].m_timestamp, 0, sizeof(OTDateTime));

					if (FAILED(res = OTCWrite(groupHandle, 1, &itemHandles[2], NULL, NULL, &values[2], &results[2], &executionOptions)))
					{
						OUTPUT(_T("Write failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('w'):
					values[2].m_pValue->vt = VT_UI1;
					values[2].m_pValue->bVal = 42;
					values[2].m_quality = OT_QUALITY_NOT_SET;
					memset(&values[2].m_timestamp, 0, sizeof(OTDateTime));

					if (SUCCEEDED(res = OTCWrite(groupHandle, 1, &itemHandles[2], NULL, NULL, &values[2], &results[2], NULL)))
					{
						OUTPUT(_T("Write\n"));

						if (SUCCEEDED(results[2]))
						{
							OUTPUT(_T("%s - %s\n"), g_objectDescription[4], GetItemValueSZ(&values[2]));
							OTVariantClear(values[2].m_pValue);
						}
						else
						{
							OUTPUT(_T("Write failed - %s [0x%8.8lX]\n"), g_objectDescription[4], results[2]);
						}
					}
					else
					{
						OUTPUT(_T("Write failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('T'):
					values[0].m_pValue->vt = VT_BSTR;
					values[0].m_pValue->bstrVal = OTSysAllocString(_T("OPC Toolkit"));
					values[0].m_quality = OT_QUALITY_NOT_SET;
					memset(&values[0].m_timestamp, 0, sizeof(OTDateTime));

					if (FAILED(res = OTCWrite(sessionHandle, 1, NULL, &itemIDs[1], NULL, &values[0], &results[0], &executionOptions)))
					{
						OUTPUT(_T("Write failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('t'):
					values[0].m_pValue->vt = VT_BSTR;
					values[0].m_pValue->bstrVal = OTSysAllocString(_T("Softing"));
					values[0].m_quality = OT_QUALITY_NOT_SET;
					memset(&values[0].m_timestamp, 0, sizeof(OTDateTime));

					if (SUCCEEDED(res = OTCWrite(sessionHandle, 1, NULL, &itemIDs[1], NULL, &values[0], &results[0], NULL)))
					{
						OUTPUT(_T("Write\n"));

						if (SUCCEEDED(results[0]))
						{
							OUTPUT(_T("%-23.23s - %s\n"), itemIDs[1], GetItemValueSZ(&values[0]));
							OTVariantClear(values[0].m_pValue);
						}
						else
						{
							OUTPUT(_T("Write failed - %s [0x%8.8lX]\n"), itemIDs[1], results[0]);
						}
					}
					else
					{
						OUTPUT(_T("Write failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('S'):
					OTCGetServerStatus(sessionHandle, NULL, &executionOptions);
					break;

				case _T('s'):
				{
					OTCServerStatus serverStatus;
					memset(&serverStatus, 0, sizeof(OTCServerStatus));

					if (SUCCEEDED(res = OTCGetServerStatus(sessionHandle, &serverStatus, NULL)))
					{
						PrintServerStatus(&serverStatus);
					}
					else
					{
						OUTPUT(_T("Get Status failed [0x%8.8lX]\n"), res);
					}

					if (serverStatus.m_vendorInfo != NULL)
					{
						OTFreeMemory(serverStatus.m_vendorInfo);
					}

					if (serverStatus.m_productVersion != NULL)
					{
						OTFreeMemory(serverStatus.m_productVersion);
					}

					if (serverStatus.m_statusInfo != NULL)
					{
						OTFreeMemory(serverStatus.m_statusInfo);
					}

					if (serverStatus.m_supportedLCIDs != NULL)
					{
						OTFreeMemory(serverStatus.m_supportedLCIDs);
					}
				}
				break;

				case _T('M'):
				case _T('m'):
					if (SUCCEEDED(res = OTCActivateConnectionMonitor(sessionHandle, true, 5000, 10, 10000, 300000)))
					{
						OUTPUT(_T("Activated connection monitor\n"));
					}
					else
					{
						OUTPUT(_T("Activate connection monitor failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('N'):
				case _T('n'):
					if (SUCCEEDED(res = OTCActivateConnectionMonitor(sessionHandle, false, 0, 0, 0, 0)))
					{
						OUTPUT(_T("Deactivated connection monitor\n"));
					}
					else
					{
						OUTPUT(_T("Deactivate connection monitor failed [0x%8.8lX]\n"), res);
					}

					break;

				case _T('?'):
				case _T('U'):
				case _T('u'):
					usage();
					break;

				case _T('E'):
				case _T('e'):
				case _T('Q'):
				case _T('q'):
					end = true;
					break;
				}
			}
		}

		OTCTerminate();
	}

	for (i = 0; i < 3; i++)
	{
		OTFreeMemory(values[i].m_pValue);
	}

#ifdef OTB_USE_STATIC_LIB
	OTExitLib();
#endif
	return 0;
}

void OTCAPI_CALL OnStateChange(IN OTCObjectContext objectContext, IN unsigned char state)
{
	OUTPUT(_T("%s state changed - %s\n"), g_objectDescription[objectContext.m_objectData.m_userData], g_stateDescription[state]);
}


void OTCAPI_CALL OnDataChange(IN OTCObjectContext objectContext, IN unsigned long count, IN OTCObjectData* pItemData, IN OTCValueData* pValueData, IN long* pResults)
{
	unsigned long i;
	OUTPUT(_T("%s data changed\n"), g_objectDescription[objectContext.m_objectData.m_userData]);

	for (i = 0; i < count; i++)
	{
		OUTPUT(_T("%s - %s\n"), g_objectDescription[pItemData[i].m_userData], GetItemValueSZ(&pValueData[i]));
	}
}


void OTCAPI_CALL OnReadComplete(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long itemCount, IN OTCObjectData* pItemData, IN OTChar * *pItemIDs, IN OTChar * *pItemPaths, IN OTCValueData* pValues, IN long* pResults)
{
	OUTPUT(_T("Read complete\n"));

	if (SUCCEEDED(result))
	{
		OTChar* pItemID;
		unsigned long i;

		for (i = 0; i < itemCount; i++)
		{
			if (pItemData)
			{
				pItemID = g_objectDescription[pItemData[i].m_userData];
			}
			else
			{
				pItemID = (OTChar*)pItemIDs[i];
			}

			if (SUCCEEDED(pResults[i]))
			{
				OUTPUT(_T("%-23.23s - %s\n"), pItemID, GetItemValueSZ(&pValues[i]));
				OTVariantClear(pValues[i].m_pValue);
			}
			else
			{
				OUTPUT(_T("Read failed - %s [0x%8.8lX]\n"), pItemID, pResults[i]);
			}
		}
	}
	else
	{
		OUTPUT(_T("Read failed [0x%8.8lX]\n"), result);
	}
}

void OTCAPI_CALL OnWriteComplete(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long itemCount, IN OTCObjectData* pItemData, IN OTChar * *pItemIDs, IN OTChar * *pItemPaths, IN OTCValueData* pValues, IN long* pResults)
{
	OUTPUT(_T("Write complete\n"));

	if (SUCCEEDED(result))
	{
		OTChar* pItemID;
		unsigned long i;

		for (i = 0; i < itemCount; i++)
		{
			if (pItemData)
			{
				pItemID = g_objectDescription[pItemData[i].m_userData];
			}
			else
			{
				pItemID = (OTChar*)pItemIDs[i];
			}

			if (SUCCEEDED(pResults[i]))
			{
				if (pValues)
				{
					OUTPUT(_T("%-23.23s - %s\n"), pItemID, GetItemValueSZ(&pValues[i]));
					OTVariantClear(pValues[i].m_pValue);
				}
				else
				{
					OUTPUT(_T("%-23.23s - write succeeded\n"), pItemID);
				}
			}
			else
			{
				OUTPUT(_T("Write failed - %s [0x%8.8lX]\n"), pItemID, pResults[i]);
			}
		}
	}
	else
	{
		OUTPUT(_T("Write failed [0x%8.8lX]\n"), result);
	}
}



void OTCAPI_CALL OnPerfromStateTransition(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result)
{
	OUTPUT(_T("%s performed state transition - result 0x%8.8lX - context: %lu\n"), g_objectDescription[objectContext.m_objectData.m_userData], result, executionContext);
}

unsigned char OTCAPI_CALL OnServerShutdown(IN OTCObjectData serverData, IN OTChar* reason)
{
	OUTPUT(_T("%s shutdown - reason: %s\n"), g_objectDescription[serverData.m_userData], reason);
	return true; // reconnect automatically
}

void OTCAPI_CALL OnGetServerStatus(IN unsigned long executionContext, IN OTCObjectData serverData, IN long result, IN OTCServerStatus* pServerStatus)
{
	if (SUCCEEDED(result))
	{
		PrintServerStatus(pServerStatus);
	}
	else
	{
		OUTPUT(_T("Get Status failed [0x%8.8lX]\n"), result);
	}
}

OTChar g_itemValueSZ[201];
OTChar g_itemValueQTSZ[256];

OTChar* GetItemValueSZ(IN OTCValueData* pValueData)
{
	OTVariantToString(pValueData->m_pValue, 200, g_itemValueSZ);
	_stprintf(g_itemValueQTSZ, _T("%s (Q:%u T:%s)"), g_itemValueSZ, pValueData->m_quality, GetDateTimeSZ(pValueData->m_timestamp));
	return g_itemValueQTSZ;
}


OTChar g_dateTimeSZ[256];

OTChar* GetDateTimeSZ(IN OTCDateTime dateTime)
{
#if defined(WIN32) || defined(WIN64)
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&dateTime, &sysTime);
	_stprintf(g_dateTimeSZ, _T("%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%3.3d"), sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#else
	struct tm t;
	localtime_r(&dateTime.tv_sec, &t);
	sprintf(g_dateTimeSZ, "%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%3.3d", t.tm_mday, t.tm_mon, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec, (int)(dateTime.tv_usec / 1000));
#endif
	return g_dateTimeSZ;
}

void PrintServerStatus(OTCServerStatus* pServerStatus)
{
	unsigned long i;
	OUTPUT(_T("Server Status\n"));
	OUTPUT(_T("   Vendor info: %s\n"), pServerStatus->m_vendorInfo);
	OUTPUT(_T("   Product version: %s\n"), pServerStatus->m_productVersion);
	OUTPUT(_T("   Start time: %s\n"), GetDateTimeSZ(pServerStatus->m_startTime));
	OUTPUT(_T("   Last update time: %s\n"), GetDateTimeSZ(pServerStatus->m_lastUpdateTime));
	OUTPUT(_T("   Current time: %s\n"), GetDateTimeSZ(pServerStatus->m_currentTime));
	OUTPUT(_T("   Group count: %ld\n"), pServerStatus->m_groupCount);
	OUTPUT(_T("   Bandwidth: %ld\n"), pServerStatus->m_bandwidth);

	for (i = 0; i < pServerStatus->m_countLCIDs; i++)
	{
		OUTPUT(_T("   Supported LCID: 0x%4.4X\n"), (unsigned short)pServerStatus->m_supportedLCIDs[i]);
	}

	OUTPUT(_T("   Status info: %s\n"), pServerStatus->m_statusInfo);
}
