#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <wtypes.h>
#include <oleauto.h>
#include <list>
#include "SOXpS.h"
#include "resource.h"
#include <time.h>

using namespace std ;

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
// RequestData                                                                |
//-----------------------------------------------------------------------------

class RequestData : public SOXpSDARequestData
{
	// The RequestData class is used to store the request data in a
	// STL list for asynchronous handling.
public:
	RequestData(IN const RequestData& copy);
	RequestData(IN const SOXpSDARequestData& copy, IN DWORD ioOperation);
	~RequestData(void);

	copy(OUT SOXpSDARequestData& dest);
	DWORD getIOOperation(void);

protected:
	DWORD m_ioOperation;
}; // RequestData

inline DWORD RequestData::getIOOperation(void)
{
	return m_ioOperation;
}

RequestData::RequestData(
	IN const RequestData& copy) // data to initialize from
{
	m_itemHandle = copy.m_itemHandle;
	m_itemPath = _tcsdup(copy.m_itemPath);
	m_requestHandle = copy.m_requestHandle;
	m_requestState = copy.m_requestState;
	m_result = copy.m_result;
	::VariantInit(&m_value);
	::VariantCopy(&m_value, (LPVARIANT)&copy.m_value);
	m_quality = copy.m_quality;
	m_timeStamp = copy.m_timeStamp;
	m_itemUserData = copy.m_itemUserData;
	m_ioOperation = copy.m_ioOperation;
}

RequestData::RequestData(
	IN const SOXpSDARequestData& copy,  // data to initialize from
	IN DWORD ioOperation)               // I/O operation
{
	m_itemHandle = copy.m_itemHandle;
	m_itemPath = _tcsdup(copy.m_itemPath);
	m_requestHandle = copy.m_requestHandle;
	m_requestState = copy.m_requestState;
	m_result = copy.m_result;
	::VariantInit(&m_value);
	::VariantCopy(&m_value, (LPVARIANT)&copy.m_value);
	m_quality = copy.m_quality;
	m_timeStamp = copy.m_timeStamp;
	m_itemUserData = copy.m_itemUserData;
	m_ioOperation = ioOperation;
}

RequestData::copy(
	OUT SOXpSDARequestData& dest)   // destination to copy
{
	dest.m_itemHandle = m_itemHandle;
	dest.m_itemPath = _tcsdup(m_itemPath);
	dest.m_requestHandle = m_requestHandle;
	dest.m_requestState = m_requestState;
	dest.m_result = m_result;
	VariantInit(&dest.m_value);
	VariantCopy(&dest.m_value, &m_value);
	dest.m_quality = m_quality;
	dest.m_timeStamp = m_timeStamp;
	dest.m_itemUserData = m_itemUserData;
}

RequestData::~RequestData(void)
{
	free((void*)m_itemPath);
	::VariantClear(&m_value);
}




//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

// handle of time property
SOXpSItemHandle g_timeProperty = 0;

// completed OPC DA I/O requests function pointer
SOXpSCompletedDARequests g_completedReq = NULL;

// list with request data to work on
list<RequestData> g_requestList;
// lock object for the request list
CRITICAL_SECTION g_requestListLock;
// handle of request thread
HANDLE g_requestThread = INVALID_HANDLE_VALUE;
// handle of end event of request thread
#define WORK_EVENT 0
#define END_EVENT  1
HANDLE g_requestThreadEvents[2] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
// thread routine of request thread
DWORD WINAPI requestThreadMain(IN LPVOID context);


// server initialization data
SOXpSInitData g_initData =
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4, 20), // major version, minor version
	{ 0x2e565240, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } },
	// class id of OPC DA Server {2E565240-B238-11d3-842D-0008C779D775}
	_T("Softing.OPCToolboxSmpl_XpSDACpp.1"),            // ProdId of OPC DA Server
	_T("Softing.OPCToolboxSmpl_XpSDACpp"),              // version independent ProdId of OPC DA Server
	_T("Softing OPC Toolkit XPress C++ OPC DA Server"), // description of OPC DA Server
	{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
	// no OPC AE Server
	NULL,                                               // ProdId of OPC AE Server
	NULL,                                               // version independent ProdId of OPC AE Server
	NULL,                                               // description of OPC AE Server
	3,                                                  // major version number
	10,                                                 // minor version number
	25,                                                 // build number
	_T("Softing Industrial Automation GmbH"),           // vendor information
	//-- GUI --
	TRUE,                                               // show the application window
	_T("Softing XPress OPC Server C++ Sample (asyncIO)"),// application title
	IDI_ICON,                               // application icon
	//-- Service --
	_T("SOXpSCpp"),             // name of service
	_T("Softing XPress OPC Service C++ Sample"),// description of service
	_T("RPCSS\0"),                          // dependencies of service
	//-- OPC Data Access parameters --
	100,                        // minimal update rate for a group (ms)
	TRUE,                       // provide the current cache value on for read requests
	//-- Tracing --
	NULL,                                               // first trace file
	NULL,                                               // second trace file
	0,                                                  // toggle size of trace files
#ifdef _DEBUG
	SOXPS_TRACE_ALL,                // information trace
#else
	0,                                          // information trace
#endif
	0                                             // debug trace
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
SOXpSInitData* SOXPSCFG_CALL SOXpSInitialize(
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
	g_requestThreadEvents[WORK_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_requestThreadEvents[END_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&g_requestListLock);
	g_requestThread = CreateThread(NULL, 0, requestThreadMain, NULL, 0, &tid);
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
	if (g_requestThread != INVALID_HANDLE_VALUE)
	{
		if (g_requestThreadEvents[END_EVENT] != INVALID_HANDLE_VALUE)
		{
			SetEvent(g_requestThreadEvents[END_EVENT]);
			WaitForSingleObject(g_requestThread, INFINITE);
		}

		CloseHandle(g_requestThread);
	}

	if (g_requestThreadEvents[END_EVENT] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_requestThreadEvents[END_EVENT]);
	}

	if (g_requestThreadEvents[WORK_EVENT] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_requestThreadEvents[WORK_EVENT]);
	}

	DeleteCriticalSection(&g_requestListLock);
} // SOXpSStop


//-----------------------------------------------------------------------------
// SOXpSBuildDANameSpace
//
// - build the name(address) space of the OPC DA server
// - this function is called once during the initialization of the OPC server
//
void SOXPSCFG_CALL SOXpSBuildDANameSpace(
	IN SOXpSCreateNode createNode,          // create function for nodes
	IN SOXpSCreateTag createTag,            // create function for tags
	IN SOXpSCreateProperty createProperty)  // create function for properties
{
	SOXpSNodeHandle node1;
	SOXpSNodeHandle node2;
	SOXpSItemHandle tag;
	SOXpSItemHandle property;
	VARIANT descrPropVal;

	//-- increment branch --
	// the cache values of the tags are incremented by 10 or 1

	if (!(createNode(0, _T("increment"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node1)))
	{
		return;
	}

	// description of increment node
	descrPropVal.vt = VT_BSTR;
	descrPropVal.bstrVal = SysAllocString(L"increment the cache values");

	if (!(createProperty((SOXpSNodeOrItemHandle)node1, 101, NULL, VT_BSTR, SOXPS_ACCESSRIGHT_READABLE,
						 SOXPS_IOMODE_CONSTANT, &descrPropVal, 0x110, &property)))
	{
		return;
	}

	VariantClear(&descrPropVal);

	if (!(createNode(node1, _T("by10"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
	{
		return;
	}

	if (!(createTag(node2, _T("long"), VT_I4, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_QUEUE_POLL, 0x100, &tag)))
	{
		return;
	}

	if (!(createTag(node2, _T("short"), VT_I2, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_QUEUE_POLL, 0x101, &tag)))
	{
		return;
	}

	if (!(createNode(node1, _T("by1"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
	{
		return;
	}

	if (!(createTag(node2, _T("long"), VT_I4, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_QUEUE_POLL, 0x102, &tag)))
	{
		return;
	}

	if (!(createTag(node2, _T("short"), VT_I2, SOXPS_ACCESSRIGHT_READABLE | SOXPS_ACCESSRIGHT_WRITEABLE, SOXPS_IOMODE_QUEUE_POLL, 0x103, &tag)))
	{
		return;
	}

	//-- time branch --
	// the tags include the current time (local or GMT)

	if (!(createNode(0, _T("time"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node1)))
	{
		return;
	}

	// description of time node
	descrPropVal.vt = VT_BSTR;
	descrPropVal.bstrVal = SysAllocString(L"the current time");

	if (!(createProperty((SOXpSNodeOrItemHandle)node1, 101, NULL, VT_BSTR, SOXPS_ACCESSRIGHT_READABLE,
						 SOXPS_IOMODE_CONSTANT, &descrPropVal, 0x210, &property)))
	{
		return;
	}

	VariantClear(&descrPropVal);

	if (!(createNode(node1, _T("local"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
	{
		return;
	}

	if (!(createTag(node2, _T("sec"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_QUEUE_POLL, 0x201, &tag)))
	{
		return;
	}

	if (!(createTag(node2, _T("min"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_QUEUE_POLL, 0x202, &tag)))
	{
		return;
	}

	if (!(createTag(node2, _T("hour"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_QUEUE_POLL, 0x203, &tag)))
	{
		return;
	}

	if (!(createNode(node1, _T("GMT"), VT_EMPTY, 0, SOXPS_IOMODE_NOIO, 0, &node2)))
	{
		return;
	}

	if (!(createTag(node2, _T("sec"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_QUEUE_POLL, 0x204, &tag)))
	{
		return;
	}

	// server specific property of tag time.GMT.sec
	if (!(createProperty((SOXpSNodeOrItemHandle)tag, 8000, _T("number of seconds elapsed since midnight (00:00:00), January 1, 1970"), VT_I4, SOXPS_ACCESSRIGHT_READABLE,
						 SOXPS_IOMODE_QUEUE_POLL, NULL, 0x211, &g_timeProperty)))
	{
		return;
	}
} // SOXpSBuildDANameSpace


//-----------------------------------------------------------------------------
// SOXpSHandleDARequests
//
// - store the OPC DA I/O requests in the list for async I/O
// - this function is called for every device read or write of a OPC client
//
void SOXPSCFG_CALL SOXpSHandleDARequests(
	IN DWORD ioOperation,                   // type of I/O operation
	IN DWORD numRequests,                   // number of requests
	IN OUT SOXpSDARequestData* requestArray)// request data
{
	DWORD i;
	EnterCriticalSection(&g_requestListLock);

	for (i = 0; i < numRequests; i++)   // handle all requests
	{
		// add the request data to the list
		g_requestList.insert(g_requestList.end(), RequestData(requestArray[i], ioOperation));
	}

	LeaveCriticalSection(&g_requestListLock);
	// signal request thread
	SetEvent(g_requestThreadEvents[WORK_EVENT]);
} // SOXpSHandleDARequests


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle a OPC DA I/O request
//
void handleRequest(DWORD ioOperation,
				   IN OUT SOXpSDARequestData* requestData)// request data
{
	if (ioOperation == SOXPS_REQUEST_OPERATION_READ)
	{
		// read
		// do the differen simmulations
		if (requestData->m_itemHandle == g_timeProperty)
		{
			// check the saved item handles (tag or property)
			// saved the hanlde for the time property (seconds since 1970)
			VariantClear(&requestData->m_value);
			requestData->m_value.lVal = time(NULL);
			requestData->m_value.vt = VT_I4;
			requestData->m_quality = SOXPS_QUALITY_GOOD;
			requestData->m_requestState = SOXPS_REQUEST_STATE_HANDLED;
			memset(&requestData->m_timeStamp, 0, sizeof(FILETIME));
		}
		else if ((requestData->m_itemUserData >= 0x0100) && (requestData->m_itemUserData <= 0x0103))
		{
			// check the item user data
			// increment branch
			switch (requestData->m_itemUserData)
			{
			case 0x0100:
				requestData->m_value.lVal += (LONG)10;
				break;

			case 0x0101:
				requestData->m_value.iVal += (SHORT)10;
				break;

			case 0x0102:
				requestData->m_value.lVal++;
				break;

			case 0x0103:
				requestData->m_value.iVal++;
				break;
			}

			// set the quality to good
			requestData->m_quality = SOXPS_QUALITY_GOOD;
			// set the request state to handled
			requestData->m_requestState = SOXPS_REQUEST_STATE_HANDLED;
			// let the XPress Server calculate the current time
			memset(&requestData->m_timeStamp, 0, sizeof(FILETIME));
		}
		else if (requestData->m_itemPath[0] == _T('t'))
		{
			// check the item path
			// time branch
			SYSTEMTIME time;
			TCHAR type = _T(' ');
			WCHAR wideString[10];
			// set the current time
			CoFileTimeNow(&requestData->m_timeStamp);

			if (_tcsncmp(requestData->m_itemPath, _T("time.local."), 11) == 0)
			{
				GetLocalTime(&time);
				type = requestData->m_itemPath[11];
			}
			else if (_tcsncmp(requestData->m_itemPath, _T("time.GMT."), 9) == 0)
			{
				GetSystemTime(&time);
				type = requestData->m_itemPath[9];
			}

			// allways clear the request value!!!
			// it includes the current cache value
			VariantClear(&requestData->m_value);
			VariantInit(&requestData->m_value);
			requestData->m_value.vt = VT_BSTR;

			switch (type)
			{
			case _T('s'):
				swprintf(wideString, L"%2.2d:%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestData->m_value.bstrVal = SysAllocString(wideString);
				break;

			case _T('m'):
				swprintf(wideString, L"%2.2d:%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestData->m_value.bstrVal = SysAllocString(wideString);
				break;

			case _T('h'):
				swprintf(wideString, L"%2.2d", time.wHour, time.wMinute, time.wSecond);
				requestData->m_value.bstrVal = SysAllocString(wideString);
				break;
			}

			// set the quality to good
			requestData->m_quality = SOXPS_QUALITY_GOOD;
			// set the request state to handled
			requestData->m_requestState = SOXPS_REQUEST_STATE_HANDLED;
		}
	}
	else
	{
		// write
		// in the simmulation here is nothing to do
		// setting the request state to handled will update the cache
		requestData->m_requestState = SOXPS_REQUEST_STATE_HANDLED;
	}
} // handleRequest


//-----------------------------------------------------------------------------
// requestThreadMain
//
// - thread for handling async requests
//
DWORD WINAPI requestThreadMain(
	IN LPVOID context)  // thread data
{
	BOOL end = FALSE;
	DWORD waitR;
	DWORD i, j;
	list<RequestData>::iterator requestData;
	DWORD size;
	SOXpSDARequestData* requestArray;

	while (!end)
	{
		// wait for request data or the end
		waitR = WaitForMultipleObjects(2, g_requestThreadEvents, FALSE, INFINITE);

		switch (waitR - WAIT_OBJECT_0)
		{
		case END_EVENT:
			end = TRUE;
			break;

		default:
		{
			// request data in list
			EnterCriticalSection(&g_requestListLock);

			if ((size = g_requestList.size()) > 0)
			{
				// alloc array
				requestArray = (SOXpSDARequestData*)malloc(sizeof(SOXpSDARequestData) * size);
				i = 0;

				for (requestData = g_requestList.begin(); requestData != g_requestList.end(); ++requestData)
				{
					(*requestData).copy(requestArray[i]);
					// handle the request
					handleRequest((*requestData).getIOOperation(), &requestArray[i]);
					i++;
				}

				// call request completion function of XPress Tool
				g_completedReq(i, requestArray);
				// free all memory
				g_requestList.clear();

				for (j = 0; j < i; j++)
				{
					free((void*)requestArray[j].m_itemPath);
					VariantClear(&requestArray[j].m_value);
				}

				free((void*)requestArray);
			}

			LeaveCriticalSection(&g_requestListLock);
		}
		break;
		}
	}

	return 0;
} // requestThreadMain
