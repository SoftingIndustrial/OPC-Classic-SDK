#include <windows.h>
#include <tchar.h>
#include <wtypes.h>
#include <time.h>
#include <oleauto.h>
#include <process.h>
#include <soxps.h>
// SAMPLE ->
#include <soproxydef.h>
// SAMPLE <-
#include "resource.h"


//-----------------------------------------------------------------------------
// DllMain
//
// - main function of the dll
//
// returns:
//        TRUE  - init ok
//        FALSE - init failed
//
BOOL APIENTRY DllMain(
	IN HANDLE module,   // module handle
	IN DWORD reason,    // calling reason
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
// Globals
//-----------------------------------------------------------------------------

HANDLE g_workerThread    = INVALID_HANDLE_VALUE;       // worker thread handle
HANDLE g_eventStopWorkerThread = INVALID_HANDLE_VALUE; // termination event of the worker thread
void workerThreadMain(LPVOID);                         // worker thread routine
SOXpSCompletedDARequests g_completedReq = NULL;        // DA request completion function
SOXpSFireAEEvents g_fireEvents = NULL;                 // AE event notification function
SOXpSChangeAEConditions g_changeCond = NULL;           // AE condition change notification function
SOXpSShutdown g_shutdown = NULL;                       // shutdown function
SOXpSSetDeviceState g_setDeviceState = NULL;           // set the state of a connected device function

// object handles
SOXpSNodeHandle g_hNode = NULL;
SOXpSItemHandle g_hTag = NULL;
SOXpSAreaHandle g_hArea = NULL;
SOXpSSourceHandle g_hSource = NULL;


//-----------------------------------------------------------------------------
// Server initialization data
//-----------------------------------------------------------------------------

SOXpSInitData g_initData =
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4, 20), // major version, minor version
	//-- OPC Server Identity --
	// {371108A3-8D31-488B-A297-7C8AA8A402DA}
	// Class id of OPC DA Server
	{0x371108a3, 0x8d31, 0x488b, {0xa2, 0x97, 0x7c, 0x8a, 0xa8, 0xa4, 0x02, 0xda}},
	// ProdId of OPC DA Server
	_T("Softing.OPCXPressSmpl.DevState.DA.1"),
	// version independent ProgId of OPC DA Server
	_T("Softing.OPCXPressSmpl.DevState.DA"),
	// description of OPC DA Server
	_T("Softing OPC Toolkit XPress Device State Server Sample"),
	// {50004B01-04B9-447D-A0B3-A7221967B4F7}
	// Class id of OPC AE Server
	{0x50004b01, 0x04b9, 0x447d, {0xa0, 0xb3, 0xa7, 0x22, 0x19, 0x67, 0xb4, 0xf7}},
	// ProdId of OPC DA Server
	_T("Softing.OPCXPressSmpl.DevState.AE.1"),
	// version independent ProgId of OPC DA Server
	_T("Softing.OPCXPressSmpl.DevState.AE"),
	// description of OPC AE Server
	_T("Softing OPC Toolkit XPress Device State Server Sample"),
	// OPC server major version number
	3,
	// OPC server minor version number
	20,
	// OPC server build number
	400,
	// vendor information
	_T("Softing Industrial Automation GmbH"),
	//-- GUI --
	// show the application window
#ifdef _DEBUG
	TRUE,
#else
	FALSE,
#endif
	// application title
	_T("Softing OPC Toolkit XPress Device State Server Sample"),
	// application icon resource ID
	IDI_ICON,
	//-- Service --
	// name of the NT service
	_T("OPCXPressSmpl-DevState"),
	// description of the NT service
	_T("Softing OPC Toolkit XPress Device State Server Sample"),
	// dependencies of NT service
	_T("RPCSS\0"),
	//-- OPC Data Access parameters --
	// minimal update rate for a group (ms)
	// TODO: change this value suitable to your I/O system
	100,
	FALSE,
	//-- Tracing --
	// first trace file
	NULL,
	// second trace file
	NULL,
	// toggle trace files
	0,
	// information trace
	0,
	// debug trace
	0
};


//-----------------------------------------------------------------------------
// DLL exports
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SOXpSInitialize
//
// - provides the server specific data of the OPC server
// - stores the global function pointers
// - this function is called as first during the initialization of the server
//
// returns:
//        pointer to the persistent initialization data
//
SOXpSInitData* SOXPSCFG_CALL SOXpSInitialize(
	IN SOXpSCompletedDARequests completedReq, // completed OPC DA I/O requests function pointer
	IN SOXpSFireAEEvents fireEvents,          // fire OPC AE events function pointer
	IN SOXpSChangeAEConditions changeCond,    // change OPC AE conditions function pointer
	IN SOXpSShutdown shutdown,                // shutdown server function pointer
	IN SOXpSSetDeviceState setDeviceState)    // set the state of a connected device function pointer
{
	// store the function pointers for later use
	g_completedReq = completedReq;
	g_fireEvents = fireEvents;
	g_changeCond = changeCond;
	g_shutdown = shutdown;
	g_setDeviceState = setDeviceState;
	return &g_initData;
} // SOXpSInitialize


//-----------------------------------------------------------------------------
// SOXpSStart
//
// - starts additional thread of the DLL
// - this function is called after building all spaces (name, event, area)
//
// returns:
//        TRUE  - DLL startup OK, continue starting the OPC server
//        FALSE - error during startup, shut down the OPC server
//
DWORD SOXPSCFG_CALL SOXpSStart(void)
{
	// create end event for worker thread
	g_eventStopWorkerThread = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (NULL == g_eventStopWorkerThread)
	{
		return FALSE;
	}

	// start worker thread
	g_workerThread = (HANDLE)_beginthread(workerThreadMain, 0, NULL);

	if (INVALID_HANDLE_VALUE == g_workerThread)
	{
		return FALSE;
	}

	return TRUE; // return FALSE to terminate the app
} // SOXpSStart


//-----------------------------------------------------------------------------
// SOXpSStop
//
// - stops worker thread of the DLL
// - this function is called when the OPC server is about to shut down
//
void SOXPSCFG_CALL SOXpSStop(void)
{
	// singnal the worker thread to terminate
	if (g_eventStopWorkerThread)
	{
		if (g_workerThread != INVALID_HANDLE_VALUE)
		{
			::SetEvent(g_eventStopWorkerThread);
			::WaitForSingleObject(g_workerThread, INFINITE);
			g_workerThread = INVALID_HANDLE_VALUE;
		}

		::CloseHandle(g_eventStopWorkerThread);
		g_eventStopWorkerThread =  NULL;
	}
} // SOXpSStop


//-----------------------------------------------------------------------------
// SOXpSBuildDANameSpace
//
// - build the name(address) space of the OPC DA server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildDANameSpace(
	IN SOXpSCreateNode createNode,         // create function for nodes
	IN SOXpSCreateTag createTag,           // create function for tags
	IN SOXpSCreateProperty createProperty) // create function for properties
{
	// SAMPLE ->
	createNode(0, _T("node"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &g_hNode);
	createTag(g_hNode, _T("tag"), VT_I4, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_DIRECT_POLL, 0, &g_hTag);
	// SAMPLE <-
	// TODO: add your code to build the DA name space here
} // SOXpSBuildDANameSpace


//-----------------------------------------------------------------------------
// SOXpSHandleDARequests
//
// - handle the OPC DA I/O requests
// - this function is called for every device read or write of a OPC client
//
void SOXPSCFG_CALL SOXpSHandleDARequests(
	IN DWORD ioOperation,                    // type of I/O operation
	IN DWORD numRequests,                    // number of requests
	IN OUT SOXpSDARequestData* requestArray) // request data
{
	DWORD i;

	for (i = 0; i < numRequests; i++)   // handle all requests
	{
		if (ioOperation == SOXPS_REQUEST_OPERATION_READ)
		{
			// read
			// the current tag value is stored in the cache of the XPress Tool
			requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED_USE_CACHE;
		}
	}
} // SOXpSHandleDARequests


//-----------------------------------------------------------------------------
// SOXpSBuildAEEventSpace
//
// - build the event space of the OPC AE server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildAEEventSpace(
	IN SOXpSCreateCategory createCategory,                   // create function for categories
	IN SOXpSCreateAttribute createAttribute,                 // create function for attributes
	IN SOXpSCreateConditionClass createConditionClass,       // create function for condition classes
	IN SOXpSCreateSubConditionClass createSubConditionClass) // create function for subcondition classes
{
	// SIMULATION ->
	// the simulation creates a simple event
	SOXpSCategoryHandle cat;

	if (!(createCategory(SOXPS_EVENTTYPE_SIMPLE, 1, _T("time tick"), &cat)))
	{
		return;
	}

	// SIMULATION <-
	// TODO: add your code to build the AE event space here
} // SOXpSBuildAEEventSpace


//-----------------------------------------------------------------------------
// SOXpSBuildAEAreaSpace
//
// - build the area space of the OPC AE server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildAEAreaSpace(
	IN SOXpSCreateArea createArea,           // create function for areas
	IN SOXpSCreateSource createSource,       // create function for sources
	IN SOXpSCreateCondition createCondition) // create function for conditions
{
	// SIMULATION ->
	// the simulation creates one event source
	createArea(0, _T("area"), &g_hArea);
	createSource(g_hArea, _T("source"), &g_hSource);
	// SIMULATION <-
	// TODO: add your code to build the AE area space here
} // SOXpSBuildAEAreaSpace


//-----------------------------------------------------------------------------
// I/O worker thread main routine
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// workerThreadMain
//
// - main routine of worker thread
//
void workerThreadMain(LPVOID)
{
	DWORD waitR;
	BOOL end = FALSE;
	// SAMPLE ->
	DWORD loopCnt = 0;
	SOXpSDARequestData requestArray;
	requestArray.m_itemHandle = g_hTag;
	requestArray.m_itemPath = _T("node.tag");
	requestArray.m_requestHandle = NULL;
	requestArray.m_requestState = SOXPS_REQUEST_STATE_HANDLED;
	requestArray.m_result = S_OK;
	memset(&requestArray.m_timeStamp, 0, sizeof(FILETIME));
	VariantInit(&requestArray.m_value);
	// SAMPLE <-

	while (!end)
	{
		waitR = ::WaitForSingleObject(g_eventStopWorkerThread, 10);

		if (waitR == WAIT_OBJECT_0)
		{
			end = TRUE;
			break;
		}

		// TODO : check for I/O value changes
		// TODO : call the g_completedReq function for changed I/O values
		// SAMPLE ->
		if (++loopCnt <= 1000)
		{
			if ((loopCnt % 100) == 50)
			{
				SOXpSAEEventData eventData; // simple event data
				eventData.m_sourcePath = _T("area.source");
				eventData.m_eventCategory = 1;  // time tick
				eventData.m_severity = 15;
				eventData.m_message = _T("Second changed");
				eventData.m_eventAttrArray = NULL;
				eventData.m_evtType = SOXPS_EVENTTYPE_SIMPLE;
				CoFileTimeNow(&(eventData.m_occurenceTime));
				g_fireEvents(1, &eventData); // fire event
				requestArray.m_quality = SOXPS_QUALITY_GOOD;
				requestArray.m_value.vt = VT_I4;
				requestArray.m_value.lVal = loopCnt;
				g_completedReq(1, &requestArray);
			}
		}
		else
		{
			// simulate device error
			if (loopCnt == 1001)
			{
				g_setDeviceState((SOXpSDeviceHandle)g_hNode, SOWATCH_DEVSTATE_ERROR, _T("communication failure"));
				g_setDeviceState((SOXpSDeviceHandle)g_hTag, SOWATCH_DEVSTATE_ERROR, _T("communication failure"));
				g_setDeviceState((SOXpSDeviceHandle)g_hArea, SOWATCH_DEVSTATE_ERROR, _T("communication failure"));
				g_setDeviceState((SOXpSDeviceHandle)g_hSource, SOWATCH_DEVSTATE_ERROR, _T("communication failure"));
				requestArray.m_quality = SOXPS_QUALITY_COMM_FAILURE;
				g_completedReq(1, &requestArray);
			}

			if (loopCnt >= 2000)
			{
				g_setDeviceState((SOXpSDeviceHandle)g_hNode, SOWATCH_DEVSTATE_OK, _T(""));
				g_setDeviceState((SOXpSDeviceHandle)g_hTag, SOWATCH_DEVSTATE_OK, _T(""));
				g_setDeviceState((SOXpSDeviceHandle)g_hArea, SOWATCH_DEVSTATE_OK, _T(""));
				g_setDeviceState((SOXpSDeviceHandle)g_hSource, SOWATCH_DEVSTATE_OK, _T(""));
				requestArray.m_quality = SOXPS_QUALITY_GOOD;
				requestArray.m_value.vt = VT_I4;
				requestArray.m_value.lVal = -50;
				g_completedReq(1, &requestArray);
				loopCnt = 0;
			}
		}

		// SAMPLE <-
		// TODO : check for events and changed conditions
		// TODO : call the g_fireEvents function for new events
		// TODO : call the g_changeCond function for changed conditions
	}
} // eventThreadMain


