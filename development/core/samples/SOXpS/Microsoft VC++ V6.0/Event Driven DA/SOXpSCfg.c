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
//                          OPC TOOLBOX - Samples                             |
//                                                                            |
//  Filename    : SOXpSCfg.c                                                  |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Implementation of the SOXpSCfg DLL                          |
//                Report I/O OPC Data Access Sample                           |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <wtypes.h>
#include <oleauto.h>
#include "SOXpS.h"
#include "resource.h"
#include <time.h>

//-----------------------------------------------------------------------------
// DllMain
//
// - main function of the dll
//
// returns:
//		TRUE  - init ok
//		FALSE - init failed
//
BOOL APIENTRY DllMain(
	IN HANDLE module,	// module handle 
	IN DWORD reason,	// calling reason
	IN LPVOID reserved)
{
    switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
} // DllMain




//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

// completed OPC DA I/O requests function pointer
SOXpSCompletedDARequests g_completedReq = NULL;

SOXpSItemHandle g_tagSec = 0;
SOXpSItemHandle g_tagMin = 0;
SOXpSItemHandle g_tagHour = 0;


// handle of report thread
HANDLE g_reportThread = INVALID_HANDLE_VALUE;
// handle of end event of report thread
HANDLE g_reportThreadEndEvent = INVALID_HANDLE_VALUE;
// thread routine of report thread
DWORD WINAPI reportThreadMain(IN LPVOID context);

// server initialization data
SOXpSInitData g_initData = 
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4,20), // major version, minor version
	{ 0x2e565240, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } },
	// class id of OPC DA Server {2E565240-B238-11d3-842D-0008C779D775}
	_T("Softing.OPCToolboxSmpl_XpSDACpp.1"),			// ProdId of OPC DA Server
	_T("Softing.OPCToolboxSmpl_XpSDACpp"),				// version independent ProdId of OPC DA Server
	_T("Softing OPC Toolbox XPress C++ OPC DA Server"),	// description of OPC DA Server
	{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
	// no OPC AE Server 
	NULL,												// ProdId of OPC AE Server
	NULL,												// version independent ProdId of OPC AE Server
	NULL,												// description of OPC AE Server
	3,													// major version number
	10,													// minor version number
	25,													// build number
	_T("Softing AG"),						// vendor information
	//-- GUI --
	TRUE,												// show the application window
	_T("Softing XPress OPC Server C++ Sample (reportIO)"),// application title
	IDI_ICON,											// application icon
	//-- Service --
	_T("SOXpSCpp"),										// name of service
	_T("Softing XPress OPC Service C++ Sample"),		// description of service
	_T("RPCSS\0"),										// dependencies of service
	//-- OPC Data Access parameters --
	100,                        // minimal update rate for a group (ms)
	FALSE,                      // provide the current cache value on for read requests 
	//-- Tracing --
	NULL,												// first trace file
	NULL,												// second trace file
	0,													// toggle size of trace files
#ifdef _DEBUG
	SOXPS_TRACE_ALL,		        // information trace
#else
	0,									        // information trace
#endif
	0									          // debug trace
};




//-----------------------------------------------------------------------------
// Configuration DLL exports                                                  |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ATTENTION !!!
// You must add all exported functions to the SOXpSCfg.def file 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SOXpSInitialize
//
// - provides the server specific data of the OPC server
// - stores the global function pointers
// - this function is the first called during the initialization of the server
//
// returns:
//		initialization data
//
SOXpSInitData * SOXPSCFG_CALL SOXpSInitialize(
	IN SOXpSCompletedDARequests completedReq, // completed OPC DA I/O requests function pointer
	IN SOXpSFireAEEvents fireEvents,          // fire OPC AE events function pointer
	IN SOXpSChangeAEConditions changeCond,    // change OPC AE conditions function pointer
	IN SOXpSShutdown shutdown,                // send shutdown request to the clients
	IN SOXpSSetDeviceState setDeviceState)    // set the state of a connected device (diagostic information)
{
	g_completedReq = completedReq;
	return &g_initData;
} // SOXpSInitialize


//-----------------------------------------------------------------------------
// SOXpSStart
//
// - starts additional thread of the DLL
// - this function is called after building all spaces (name, event, area)
//
// returns:
//		TRUE  - DLL startup OK, continue starting the OPC server
//		FALSE - error during startup, end the OPC server
//
DWORD SOXPSCFG_CALL SOXpSStart(void)
{
	DWORD tid;
	g_reportThreadEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_reportThread = CreateThread(NULL, 0, reportThreadMain, NULL, 0, &tid);
	return TRUE;
} // SOXpSStart


//-----------------------------------------------------------------------------
// SOXpSStop
//
// - stops additional thread of the DLL
// - this function is called before termination of the OPC server
//
void SOXPSCFG_CALL SOXpSStop(void)
{
	if (g_reportThread != INVALID_HANDLE_VALUE)
	{
		if (g_reportThreadEndEvent != INVALID_HANDLE_VALUE)
		{
			SetEvent(g_reportThreadEndEvent);
			WaitForSingleObject(g_reportThread, INFINITE);
		}
		CloseHandle(g_reportThread);
	}

	if (g_reportThreadEndEvent != INVALID_HANDLE_VALUE)
		CloseHandle(g_reportThreadEndEvent);
		
} // SOXpSStop


//-----------------------------------------------------------------------------
// SOXpSBuildDANameSpace
//
// - build the name(address) space of the OPC DA server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildDANameSpace(
	IN SOXpSCreateNode createNode,			// create function for nodes
	IN SOXpSCreateTag createTag,			// create function for tags
	IN SOXpSCreateProperty createProperty)	// create function for properties
{
	SOXpSNodeHandle node1;
	SOXpSNodeHandle node2;
	SOXpSItemHandle property;
	VARIANT descrPropVal;

	//-- time branch --
	// the tags include the current time (local or GMT)

	if (!(createNode(0, _T("time"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node1)))
		return;	

	// description of time node
	descrPropVal.vt = VT_BSTR;
	descrPropVal.bstrVal = SysAllocString(L"the current time");
	if (!(createProperty((SOXpSNodeOrItemHandle)node1, 101, NULL, VT_BSTR, SOXPS_ACCESSRIGHT_READABLE,
				SOXPS_IOMODE_CONSTANT, &descrPropVal, 0, &property)))	
		return;	
	VariantClear(&descrPropVal);

	if (!(createNode(node1, _T("local"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
		return;	

	if (!(createTag(node2, _T("sec"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_REPORT, 1, &g_tagSec)))
		return;	

	if (!(createTag(node2, _T("min"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_REPORT, 2, &g_tagMin)))
		return;	

	if (!(createTag(node2, _T("hour"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_REPORT, 3, &g_tagHour)))
		return;	
} // SOXpSBuildDANameSpace


//-----------------------------------------------------------------------------
// SOXpSHandleDARequests
//
// - handle the OPC DA I/O requests
// - this function is called for every device read or write of a OPC client
//
void SOXPSCFG_CALL SOXpSHandleDARequests(
	IN DWORD ioOperation,					// type of I/O operation
	IN DWORD numRequests,					// number of requests
	IN OUT SOXpSDARequestData *requestArray)// request data
{
	DWORD i;

	for (i = 0; i < numRequests; i++)	// handle all requests
	{
		// SOXpSHandleDARequests is only called once to get the initial data
		SYSTEMTIME time;
		TCHAR type = _T(' ');
		WCHAR wideString[10];

		GetLocalTime(&time);

		VariantInit(&requestArray[i].m_value);
		requestArray[i].m_value.vt = VT_BSTR;

		switch (requestArray[i].m_itemUserData)
		{
			case 1:
				swprintf(wideString, L"%2.2d:%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestArray[i].m_value.bstrVal = SysAllocString(wideString);
			break;
			case 2:
				swprintf(wideString, L"%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestArray[i].m_value.bstrVal = SysAllocString(wideString);
			break;
			case 3:
				swprintf(wideString, L"%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestArray[i].m_value.bstrVal = SysAllocString(wideString);
			break;
		}
		// set the quality to good
		requestArray[i].m_quality = SOXPS_QUALITY_GOOD;
		// set the request state to handled
		requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
		
		// if you don't want to handle the initial read, you can return use cache to XPress
		// then the client will receive the quality "waiting for initial data"
		// in this case the function will be called until the initial data was set
//		requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED_USE_CACHE;
	}

} // SOXpSHandleDARequests



//-----------------------------------------------------------------------------
// reportThreadMain
//
// - thread for reporting requests
//
DWORD WINAPI reportThreadMain(
	IN LPVOID context)	// thread data
{
	BOOL end = FALSE;
	DWORD waitR;
	static WORD actSec = 0xFFFF;		// current second
	static WORD actMin = 0xFFFF;		// current minute
	static WORD actHour = 0xFFFF;		// current hour
	SYSTEMTIME now;						// current time
	WCHAR strnow[10];					// current time (as string)
	SOXpSDARequestData requestArray[3];
	DWORD reqCnt;

	requestArray[0].m_itemHandle = g_tagSec;
	requestArray[0].m_itemPath = _T("time.local.sec");
	requestArray[0].m_requestHandle = NULL;
	requestArray[0].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
	requestArray[0].m_result = S_OK;
	requestArray[0].m_quality = SOXPS_QUALITY_GOOD;
	VariantInit(&requestArray[0].m_value);
	requestArray[1].m_itemHandle = g_tagMin;
	requestArray[1].m_itemPath = _T("time.local.min");
	requestArray[1].m_requestHandle = NULL;
	requestArray[1].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
	requestArray[1].m_result = S_OK;
	requestArray[1].m_quality = SOXPS_QUALITY_GOOD;
	VariantInit(&requestArray[1].m_value);
	requestArray[2].m_itemHandle = g_tagHour;
	requestArray[2].m_itemPath = _T("time.local.hour");
	requestArray[2].m_requestHandle = NULL;
	requestArray[2].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
	requestArray[2].m_result = S_OK;
	requestArray[2].m_quality = SOXPS_QUALITY_GOOD;
	VariantInit(&requestArray[2].m_value);

	while (!end)
	{
		// wait for next second or the end
		waitR = WaitForSingleObject(g_reportThreadEndEvent, 1000);
		if (waitR == WAIT_TIMEOUT) 
		{
			GetLocalTime(&now);

			if (now.wSecond != actSec)
			{
				actSec = now.wSecond;

				VariantClear(&requestArray[0].m_value);
				swprintf(strnow, L"%02.2d:%02.2d:%02.2d", now.wHour, now.wMinute, now.wSecond);
				requestArray[0].m_value.vt = VT_BSTR;
				requestArray[0].m_value.bstrVal = SysAllocString(strnow);
				memset(&requestArray[0].m_timeStamp, 0, sizeof(FILETIME));
				reqCnt = 1;

				if (now.wMinute != actMin)
				{
					actMin = now.wMinute;

					VariantClear(&requestArray[1].m_value);
					swprintf(strnow, L"%02.2d:%02.2d", now.wHour, now.wMinute);
					requestArray[1].m_value.vt = VT_BSTR;
					requestArray[1].m_value.bstrVal = SysAllocString(strnow);
					memset(&requestArray[1].m_timeStamp, 0, sizeof(FILETIME));
					reqCnt = 2;

					if (now.wHour != actHour)
					{
						actHour = now.wHour;

						VariantClear(&requestArray[2].m_value);
						swprintf(strnow, L"%02.2d", now.wHour);
						requestArray[2].m_value.vt = VT_BSTR;
						requestArray[2].m_value.bstrVal = SysAllocString(strnow);
						memset(&requestArray[2].m_timeStamp, 0, sizeof(FILETIME));
						reqCnt = 3;
					}
				}

				g_completedReq(reqCnt, requestArray);
			}
		}
		else
			end = TRUE;
	}

	return 0;
}
