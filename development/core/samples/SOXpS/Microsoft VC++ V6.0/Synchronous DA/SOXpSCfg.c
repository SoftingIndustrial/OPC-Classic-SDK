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
//                Only OPC Data Access Sample                                 |
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

// handle of time property
SOXpSItemHandle g_timeProperty = 0;

// completed OPC DA I/O requests function pointer
SOXpSCompletedDARequests g_completedReq = NULL;

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
	_T("Softing XPress OPC Server C++ Sample (onlyDA)"),// application title
	IDI_ICON,											// application icon
	//-- Service --
	_T("SOXpSCpp"),										// name of service
	_T("Softing XPress OPC Service C++ Sample"),		// description of service
	_T("RPCSS\0"),										// dependencies of service
	//-- OPC Data Access parameters --
	100,                        // minimal update rate for a group (ms)
	TRUE,                       // provide the current cache value on for read requests 
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
	SOXpSItemHandle tag;
	SOXpSItemHandle property;
	VARIANT descrPropVal;

	//-- increment branch --
	// the cache values of the tags are incremented by 10 or 1

	if (!(createNode(0, _T("increment"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node1)))
		return;	

	// description of increment node
	descrPropVal.vt = VT_BSTR;
	descrPropVal.bstrVal = SysAllocString(L"increment the cache values");
	if (!(createProperty((SOXpSNodeOrItemHandle)node1, 101, NULL, VT_BSTR, SOXPS_ACCESSRIGHT_READABLE,
				SOXPS_IOMODE_CONSTANT, &descrPropVal, 0x110, &property)))		
		return;	
	VariantClear(&descrPropVal);

	if (!(createNode(node1, _T("by10"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
		return;	

	if (!(createTag(node2, _T("long"), VT_I4, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_DIRECT_POLL, 0x100, &tag)))
		return;	

	if (!(createTag(node2, _T("short"), VT_I2, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_DIRECT_POLL, 0x101, &tag)))
		return;	

	if (!(createNode(node1, _T("by1"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
		return;	
	
	if (!(createTag(node2, _T("long"), VT_I4, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_DIRECT_POLL, 0x102, &tag)))
		return;	

	if (!(createTag(node2, _T("short"), VT_I2, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_DIRECT_POLL, 0x103, &tag)))
		return;	


	//-- time branch --
	// the tags include the current time (local or GMT)

	if (!(createNode(0, _T("time"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node1)))
		return;	

	// description of time node
	descrPropVal.vt = VT_BSTR;
	descrPropVal.bstrVal = SysAllocString(L"the current time");
	if (!(createProperty((SOXpSNodeOrItemHandle)node1, 101, NULL, VT_BSTR, SOXPS_ACCESSRIGHT_READABLE,
				SOXPS_IOMODE_CONSTANT, &descrPropVal, 0x210, &property)))	
		return;	
	VariantClear(&descrPropVal);

	if (!(createNode(node1, _T("local"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
		return;	

	if (!(createTag(node2, _T("sec"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_POLL, 0x201, &tag)))
		return;	

	if (!(createTag(node2, _T("min"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_POLL, 0x202, &tag)))
		return;	

	if (!(createTag(node2, _T("hour"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_POLL, 0x203, &tag)))
		return;	

	if (!(createNode(node1, _T("GMT"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
		return;	

	if (!(createTag(node2, _T("sec"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_POLL, 0x204, &tag)))
		return;	

	// server specific property of tag time.GMT.sec
	if (!(createProperty((SOXpSNodeOrItemHandle)tag, 8000, _T("number of seconds elapsed since midnight (00:00:00), January 1, 1970"), VT_I4, SOXPS_ACCESSRIGHT_READABLE,
				SOXPS_IOMODE_DIRECT_POLL, NULL, 0x211, &g_timeProperty)))
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
		if (ioOperation == SOXPS_REQUEST_OPERATION_READ)
		{ // read
			// do the differen simmulations

			if (requestArray[i].m_itemHandle == g_timeProperty)
			{ // check the saved item handles (tag or property)
				// saved the hanlde for the time property (seconds since 1970)
				VariantClear(&requestArray[i].m_value);
				requestArray[i].m_value.lVal = time(NULL);
				requestArray[i].m_value.vt = VT_I4;
				requestArray[i].m_quality = SOXPS_QUALITY_GOOD;
				requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
				memset(&requestArray[i].m_timeStamp, 0, sizeof(FILETIME));
			}
			else if ((requestArray[i].m_itemUserData >= 0x0100) && (requestArray[i].m_itemUserData <= 0x0103))
			{ // check the item user data
				// increment branch
				switch (requestArray[i].m_itemUserData)
				{
					case 0x0100:
							requestArray[i].m_value.lVal += (LONG)10;
					break;				
					case 0x0101:
							requestArray[i].m_value.iVal += (SHORT)10;
					break;				
					case 0x0102:
							requestArray[i].m_value.lVal++;
					break;				
					case 0x0103:
							requestArray[i].m_value.iVal++;
					break;				

				}

				// set the quality to good
				requestArray[i].m_quality = SOXPS_QUALITY_GOOD;
				// set the request state to handled
				requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
				// let the XPress Server calculate the current time
				memset(&requestArray[i].m_timeStamp, 0, sizeof(FILETIME));
			}
			else if (requestArray[i].m_itemPath[0] == _T('t'))
			{ // check the item path
				// time branch
				SYSTEMTIME time;
				TCHAR type = _T(' ');
				WCHAR wideString[10];

				// set the current time
				CoFileTimeNow(&requestArray[i].m_timeStamp);
				if (_tcsncmp(requestArray[i].m_itemPath, _T("time.local."), 11) == 0)
				{
					GetLocalTime(&time);
					type = requestArray[i].m_itemPath[11];
				}
				else if (_tcsncmp(requestArray[i].m_itemPath, _T("time.GMT."), 9) == 0)
				{
					GetSystemTime(&time);
					type = requestArray[i].m_itemPath[9];
				}

				// allways clear the request value!!! 
				// it includes the current cache value
				VariantClear(&requestArray[i].m_value);	
				VariantInit(&requestArray[i].m_value);
				requestArray[i].m_value.vt = VT_BSTR;

				switch (type)
				{
					case _T('s'):
						swprintf(wideString, L"%2.2d:%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
						requestArray[i].m_value.bstrVal = SysAllocString(wideString);
					break;
					case _T('m'):
						swprintf(wideString, L"%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
						requestArray[i].m_value.bstrVal = SysAllocString(wideString);
					break;
					case _T('h'):
						swprintf(wideString, L"%2.2d", time.wHour, time.wMinute, time.wSecond);
						requestArray[i].m_value.bstrVal = SysAllocString(wideString);
					break;
				}
				// set the quality to good
				requestArray[i].m_quality = SOXPS_QUALITY_GOOD;
				// set the request state to handled
				requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
			}
		}
		else
		{ // write
			// in the simmulation here is nothing to do
			// setting the request state to handled will update the cache
			requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
		}
	}
} // SOXpSHandleDARequests


