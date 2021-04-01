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
//                Only OPC Alarms & Events Sample                             |
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

// handle of PVLEVEL condition
SOXpSConditionHandle g_condClockLEVEL = 0;

// handle of alarm&event thread
HANDLE g_eventThread = INVALID_HANDLE_VALUE;
// handle of end event of alarm&event thread
HANDLE g_eventThreadEndEvent = INVALID_HANDLE_VALUE;
// thread routine of alarm&event thread
DWORD WINAPI eventThreadMain(IN LPVOID context);

// fire OPC AE events function pointer
SOXpSFireAEEvents g_fireEvents = NULL; 
// change OPC AE conditions function pointer
SOXpSChangeAEConditions g_changeCond = NULL;

// server initialization data
SOXpSInitData g_initData = 
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4,20), // major version, minor version
	{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
	// no OPC DA Server 
	NULL,												// ProdId of OPC DA Server
	NULL,												// version independent ProdId of OPC DA Server
	NULL,												// description of OPC DA Server
	{ 0x2e565241, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } },
	// class id of OPC AE Server {2E565241-B238-11d3-842D-0008C779D775}
	_T("Softing.OPCToolboxSmpl_XpSAECpp.1"),			// ProdId of OPC AE Server
	_T("Softing.OPCToolboxSmpl_XpSAECpp"),				// version independent ProdId of OPC AE Server
	_T("Softing OPC Toolbox XPress C++ OPC AE Server"),	// description of OPC AE Server
	3,													// major version number
	10,													// minor version number
	25,													// build number
	_T("Softing AG"),						// vendor information
	//-- GUI --
	TRUE,												// show the application window
	_T("Softing XPress OPC Server C++ Sample (onlyAE)"),// application title
	IDI_ICON,											// application icon
	//-- Service --
	_T("SOXpSCpp"),										// name of service
	_T("Softing XPress OPC Service C++ Sample"),		// description of service
	_T("RPCSS\0"),										// dependencies of service
	//-- OPC Data Access parameters --
	0, // no data access
	FALSE,
	//-- Tracing --
	NULL,												// first trace file
	NULL,												// second trace file
	0,													// toggle size of trace files
	0,									        // information trace
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
	g_fireEvents = fireEvents; 
	g_changeCond = changeCond;
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
	g_eventThreadEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_eventThread = CreateThread(NULL, 0, eventThreadMain, NULL, 0, &tid);
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
	if (g_eventThread != INVALID_HANDLE_VALUE)
	{
		if (g_eventThreadEndEvent != INVALID_HANDLE_VALUE)
		{
			SetEvent(g_eventThreadEndEvent);
			WaitForSingleObject(g_eventThread, INFINITE);
			CloseHandle(g_eventThreadEndEvent);
		}
		CloseHandle(g_eventThread);
	}
} // SOXpSStop


//-----------------------------------------------------------------------------
// SOXpSBuildAEEventSpace
//
// - build the event space of the OPC AE server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildAEEventSpace(
	IN SOXpSCreateCategory createCategory,					// create function for categories
	IN SOXpSCreateAttribute createAttribute,				// create function for attributes
	IN SOXpSCreateConditionClass createConditionClass,		// create function for condition classes
	IN SOXpSCreateSubConditionClass createSubConditionClass)// create function for subcondition classes
{
	SOXpSCategoryHandle cat;
	SOXpSConditionClassHandle cond;

	//-- time tick category --
	// simple event with 2 attributes 
	if (!(createCategory(SOXPS_EVENTTYPE_SIMPLE, 1, _T("time tick"), &cat)))
		return;	

	if (!(createAttribute(cat,	1, _T("current time"), VT_BSTR)))
		return;	

	if (!(createAttribute(cat,	2, _T("seconds since 1.1.1970"), VT_I4)))
		return;	

	//-- time level category --
	// conditional event with 4 attributes 
	if (!(createCategory(SOXPS_EVENTTYPE_CONDITION, 2, _T("time level"), &cat)))
		return;	

	if (!(createAttribute(cat,	1, _T("current second"), VT_I2)))
		return;	

	if (!(createAttribute(cat,	2, _T("low limit"), VT_I2)))
		return;	

	if (!(createAttribute(cat,	3, _T("high limit"), VT_I2)))
		return;	

	if (!(createAttribute(cat,	4, _T("current time"), VT_BSTR)))
		return;	

	// multiple state condition
	if (!(createConditionClass(cat,	_T("PVLEVEL"), _T("inactive"), FALSE, FALSE, NULL, 0, NULL, FALSE, &cond)))
		return;	
	
	if (!(createSubConditionClass(cond,	_T("HI_HI"), 510, _T("HI_HI Des"), TRUE, _T("HI_HI Def"))))
		return;	

	if (!(createSubConditionClass(cond,	_T("HI"), 205, _T("HI Des"), FALSE, _T("HI Def"))))
		return;	

	if (!(createSubConditionClass(cond,	_T("LO"), 205, _T("LO Des"), FALSE, _T("LO Def"))))
		return;	

	if (!(createSubConditionClass(cond,	_T("LO_LO"), 510, _T("LO_LO Des"), TRUE, _T("LO_LO Def"))))
		return;	

	// single state condition
	if (!(createConditionClass(cat,	_T("LO"), _T("^LO"), FALSE, TRUE, _T("LO Cond Def"), 420, _T("LO"), FALSE, NULL)))
		return;	
} // SOXpSBuildAEEventSpace


//-----------------------------------------------------------------------------
// SOXpSBuildAEAreaSpace
//
// - build the area space of the OPC AE server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildAEAreaSpace(
	IN SOXpSCreateArea createArea,				// create function for areas	
	IN SOXpSCreateSource createSource,			// create function for sources
	IN SOXpSCreateCondition createCondition)	// create function for conditions
{
	SOXpSAreaHandle area;
	SOXpSSourceHandle source;

	if (!(createArea(0, _T("computer"), &area)))
		return;	

	if (!(createSource(area, _T("clock"), &source)))
		return;	

	if (!(createCondition(source, _T("PVLEVEL"), &g_condClockLEVEL)))
		return;	

	if (!(createCondition(source, _T("LO"), NULL)))
		return;	
} // SOXpSBuildAEAreaSpace


//-----------------------------------------------------------------------------
// eventThreadMain
//
// - thread for generating alarms&events
//
DWORD WINAPI eventThreadMain(
	IN LPVOID context)	// thread data
{
	BOOL end = FALSE;					// terminate the thread?
	DWORD waitR;						// wait result
	static WORD actSec = 0xFFFF;		// current second
	static WORD actMin = 0xFFFF;		// current minute
	SYSTEMTIME now;						// current time
	WCHAR strnow[13];					// current time (as string)
	SOXpSAEEventData eventData[2];		// simple event data
	DWORD eventCnt;						// simple event count
	VARIANT attrTimeTick[2];			// time tick event attributes
	SOXpSAEConditionData condData[2];	// conditional event data
	VARIANT attrTimeLevel[4];			// pvlevel condition attributes
	VARIANT attrTimeLo[4];				// lo condition attributes

	// initialize the COM environment
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// ATTENTION !!!
	// the attribute array include must all attributes of the category in 
	// the same order used during creation of the event space

	// initialize attributes and event/conditiion data
	eventData[0].m_sourcePath = _T("computer.clock");
	eventData[0].m_eventCategory = 1;	// time tick
	eventData[0].m_severity = 15;
	eventData[0].m_message = _T("Second changed");
	eventData[0].m_eventAttrArray = attrTimeTick;
	eventData[0].m_evtType = SOXPS_EVENTTYPE_SIMPLE;
	memset(&(eventData[0].m_occurenceTime), 0, sizeof(FILETIME));
	eventData[1].m_sourcePath = _T("computer.clock");
	eventData[1].m_eventCategory = 1;	// time tick
	eventData[1].m_severity = 485;
	eventData[1].m_message = _T("Minute changed");
	eventData[1].m_eventAttrArray = attrTimeTick;
	eventData[1].m_evtType = SOXPS_EVENTTYPE_SIMPLE;
	VariantInit(&attrTimeTick[0]);
	VariantInit(&attrTimeTick[1]);
													// identify condition by:
	condData[0].m_condHandle = g_condClockLEVEL;	// condition handle
	condData[0].m_sourcePath = NULL;
	condData[0].m_condName = NULL;
	condData[0].m_condAttrArray = attrTimeLevel;
	condData[0].m_quality = SOXPS_QUALITY_GOOD;
	condData[0].m_mask = 0;
	condData[0].m_ackID = NULL;
	condData[1].m_condHandle = 0;
	condData[1].m_sourcePath = _T("computer.clock");// source path and
	condData[1].m_condName = _T("LO");				// condition name
	condData[1].m_condAttrArray = attrTimeLo;
	condData[1].m_quality = SOXPS_QUALITY_GOOD;
	condData[1].m_actSubCond = NULL;
	condData[1].m_acknowledge = SOXPS_ACKNOWLEDGE_NO_CHANGE;
	condData[1].m_ackID = NULL;
	condData[1].m_mask = SOXPS_CONDITIONDATA_SEVERITY | SOXPS_CONDITIONDATA_MESSAGE;
	VariantInit(&attrTimeLevel[0]);
	VariantInit(&attrTimeLevel[1]);
	VariantInit(&attrTimeLevel[2]);
	VariantInit(&attrTimeLevel[3]);
	attrTimeLevel[0].vt = VT_I2;
	attrTimeLevel[1].vt = VT_I2;
	attrTimeLevel[2].vt = VT_I2;
	VariantInit(&attrTimeLo[0]);
	VariantInit(&attrTimeLo[1]);
	VariantInit(&attrTimeLo[2]);
	VariantInit(&attrTimeLo[3]);
	attrTimeLo[0].vt = VT_I2;
	attrTimeLo[1].vt = VT_I2;
	attrTimeLo[1].iVal = 5;
	attrTimeLo[2].vt = VT_I2;
	attrTimeLo[2].iVal = 14;

	while (!end)
	{
		waitR = WaitForSingleObject(g_eventThreadEndEvent, 1000);
		if (waitR != WAIT_OBJECT_0)
		{
			// generate events every second
			GetLocalTime(&now);

			if (now.wSecond != actSec)
			{
				actSec = now.wSecond;
				
				// simple events
				VariantClear(&attrTimeTick[0]);
				VariantClear(&attrTimeTick[1]);
			
				swprintf(strnow, L"%02.2d:%02.2d:%02.2d.%03.3d", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);

				attrTimeTick[0].vt = VT_BSTR;
				attrTimeTick[0].bstrVal = SysAllocString(strnow);

				attrTimeTick[1].vt = VT_I4;
				attrTimeTick[1].lVal = time(NULL);

				eventCnt = 1;

				if (now.wMinute != actMin)
				{
					actMin = now.wMinute;
					CoFileTimeNow(&(eventData[1].m_occurenceTime));
					eventCnt = 2;
				}

				// fire events
				g_fireEvents(eventCnt, eventData);

				// conditions
				// PVLEVEL
				if (actSec <= 4)
				{ // LO_LO
					attrTimeLevel[1].iVal = 0;
					attrTimeLevel[2].iVal = 4;
					condData[0].m_activate = TRUE;
					condData[0].m_actSubCond = _T("LO_LO");
				}
				else if (actSec <= 14)
				{ // LO
					attrTimeLevel[1].iVal = 5;
					attrTimeLevel[2].iVal = 14;
					condData[0].m_activate = TRUE;
					condData[0].m_actSubCond = _T("LO");
				}
				else if (actSec <= 44)
				{ // normal inactive
					attrTimeLevel[1].iVal = 15;
					attrTimeLevel[2].iVal = 44;
					condData[0].m_activate = FALSE;
					condData[0].m_actSubCond = NULL;
				}
				else if (actSec <= 54)
				{ // HI
					attrTimeLevel[1].iVal = 45;
					attrTimeLevel[2].iVal = 54;
					condData[0].m_activate = TRUE;
					condData[0].m_actSubCond = _T("HI");
				}
				else 
				{ // HI_HI
					attrTimeLevel[1].iVal = 55;
					attrTimeLevel[2].iVal = 60;
					condData[0].m_activate = TRUE;
					condData[0].m_actSubCond = _T("HI_HI");
				}

				
				switch (actSec)
				{ // acknowledge from devicesimulation
					case 32: // add acknowledge from the device
						condData[0].m_acknowledge = SOXPS_ACKNOWLEDGE_ACK;
					break;
					case 37: // remove acknowledge from the device
						condData[0].m_acknowledge = SOXPS_ACKNOWLEDGE_NACK;
					break;
					default: // no acknowledge change
						condData[0].m_acknowledge = SOXPS_ACKNOWLEDGE_NO_CHANGE;
					break;
				}

				attrTimeLevel[0].iVal = actSec;
				VariantClear(&attrTimeLevel[3]);
				attrTimeLevel[3].vt = VT_BSTR;
				attrTimeLevel[3].bstrVal = SysAllocString(strnow);

				// LO
				attrTimeLo[0].iVal = actSec;
				VariantClear(&attrTimeLo[3]);
				attrTimeLo[3].vt = VT_BSTR;
				attrTimeLo[3].bstrVal = SysAllocString(strnow);

				condData[1].m_activate = ((actSec >= 5) && (actSec <= 14));
				condData[1].m_severity = ((actSec >= 5) && (actSec <= 14)) ? 600 : 53;
				condData[1].m_message = ((actSec >= 5) && (actSec <= 14)) ? _T("Active LO on computer.clock") : _T("Inactive LO on computer.clock");

				// may changed the condition state
				// if the condition state remains the same no events will be generated
				// if the condition state changes the OPC clients will be notified
				g_changeCond(2, condData);
			}
		}
		else
			end = TRUE;
	}

	// uninitialize the COM environment
	CoUninitialize();

	return 0;
} // eventThreadMain
 

