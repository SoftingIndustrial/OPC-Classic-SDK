#include <windows.h>
#include <tchar.h>
#include <wtypes.h>
#include <time.h>
#include <oleauto.h>
#include <process.h>
#include <soxps.h>
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
SOXpSSetDeviceState g_setDeviceState = NULL;           // set the state of a connected device


//-----------------------------------------------------------------------------
// Server initialization data
//-----------------------------------------------------------------------------

SOXpSInitData g_initData =
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4, 20), // major version, minor version
	//-- OPC Server Identity --
	// {81806974-AD5E-42E9-922C-FBA6DFE23C3C}
	// Class id of OPC DA Server
	{0x81806974, 0xad5e, 0x42e9, {0x92, 0x2c, 0xfb, 0xa6, 0xdf, 0xe2, 0x3c, 0x3c}},
	// ProdId of OPC DA Server
	_T("Softing.OPCXPressSmpl.Shutdown.DA.1"),
	// version independent ProgId of OPC DA Server
	_T("Softing.OPCXPressSmpl.Shutdown.DA"),
	// description of OPC DA Server
	_T("Softing OPC Toolkit XPress Shutdown Server Sample"),
	// {C49A918D-FD09-46E1-9D95-F5A91375E25A}
	// Class id of OPC AE Server
	{0xc49a918d, 0xfd09, 0x46e1, {0x9d, 0x95, 0xf5, 0xa9, 0x13, 0x75, 0xe2, 0x5a}},
	// ProdId of OPC DA Server
	_T("Softing.OPCXPressSmpl.Shutdown.AE.1"),
	// version independent ProgId of OPC DA Server
	_T("Softing.OPCXPressSmpl.Shutdown.AE"),
	// description of OPC AE Server
	_T("Softing OPC Toolkit XPress Shutdown Server Sample"),
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
	_T("Softing OPC Toolkit XPress Shutdown Server Sample"),
	// application icon resource ID
	IDI_ICON,
	//-- Service --
	// name of the NT service
	_T("OPCXPressSmpl-Shutdown"),
	// description of the NT service
	_T("Softing OPCToolbox XPress Shutdown Server Sample"),
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
	IN SOXpSShutdown shutdown,                // shutdown server
	IN SOXpSSetDeviceState setDeviceState)    // set the state of a connected device (diagostic information)
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
	// SIMULATION ->
	// the simulation value let the XPress Tool take the cache value
	SOXpSItemHandle tag;
	createTag(0, _T("simulation"), VT_I4, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_DIRECT_POLL, 0, &tag);
	// SIMULATION <-
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
			// TODO: add your read I/O code here
			// ATTENTION: You must call the function ::VariantClear before
			//            setting the variant value requestArray[i].m_value
			//            with the read value. For read request it includes
			//            the current cache value at the time of the function call
			// SIMULATION ->
			// the simulation value let the XPress Tool take the cache value
			requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED_USE_CACHE;
			// SIMULATION <-
		}
		else
		{
			// write
			// TODO: add your write I/O code here
			// SIMULATION ->
			// the simulation value accepts the written value, this will update the cache
			requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
			// SIMULATION <-
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
	SOXpSSourceHandle source;
	createSource(0, _T("clock"), &source);
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
	// SIMULATION ->
	DWORD loopCnt = 0;
	// SIMULATION <-
	// SAMPLE ->
	DWORD loopCnt2 = 0;
	// SAMPLE <-

	while (!end)
	{
		waitR = ::WaitForSingleObject(g_eventStopWorkerThread, 10);

		if (waitR == WAIT_OBJECT_0)
		{
			end = TRUE;
			break;
		}

		// SIMULATION ->
		if (++loopCnt > 100)
		{
			SOXpSAEEventData eventData; // simple event data
			loopCnt = 0;
			eventData.m_sourcePath = _T("clock");
			eventData.m_eventCategory = 1;  // time tick
			eventData.m_severity = 15;
			eventData.m_message = _T("Second changed");
			eventData.m_eventAttrArray = NULL;
			CoFileTimeNow(&(eventData.m_occurenceTime));
			g_fireEvents(1, &eventData); // fire event
		}

		// SIMULATION <-
		// SAMPLE ->
//		if (++loopCnt2 > 6000)
		if (++loopCnt2 > 600)
		{
			g_shutdown(_T("One Minute Shutdown !"));
			loopCnt2 = 0;
		}

		// SAMPLE <-
		// TODO : check for events and changed conditions
		// TODO : call the g_fireEvents function for new events
		// TODO : call the g_changeCond function for changed conditions
	}
} // eventThreadMain


