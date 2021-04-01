#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stddef.h>
#include <crtdbg.h>
#include <malloc.h>
#include <oleauto.h>
#include <string>

#include "SOXpS.h"
#include "resource.h"

using namespace std ;

class CDataRequest
{
public:
	CDataRequest(DWORD dwSize, const SOXpSDARequestData& src)
		: m_dwSize(dwSize)
	{
		::ZeroMemory(&m_ov, sizeof(m_ov));
		::ZeroMemory(&m_rq, sizeof(m_rq));
		::VariantInit(&m_rq.m_value);
		m_rq.m_itemHandle = src.m_itemHandle;
		m_rq.m_itemPath = _tcsdup(src.m_itemPath);
		m_rq.m_requestHandle = src.m_requestHandle;
		m_lpBuffer = ::VirtualAlloc(NULL,  dwSize, MEM_COMMIT, PAGE_READWRITE);

		if (!m_lpBuffer)
		{
			throw E_OUTOFMEMORY;
		}
	}

	~CDataRequest()
	{
		::VariantClear(&m_rq.m_value);
		free((LPVOID)m_rq.m_itemPath);

		if (m_lpBuffer)
		{
			::VirtualFree(m_lpBuffer , 0, MEM_RELEASE);
		}
	}

	OVERLAPPED m_ov;
	LPVOID m_lpBuffer;
	SOXpSDARequestData m_rq;
	DWORD m_dwSize;
};

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
// Globals                                                                    |
//-----------------------------------------------------------------------------

// Completed OPC DA I/O requests function pointer
SOXpSCompletedDARequests pfnRequestCompleted = NULL;
SOXpSItemHandle tag = NULL;

LPCTSTR szFileName =  _T("SOXPS.TXT");
HANDLE hFile = NULL;
DWORD g_dwBytesPerSector = 0;

#define IS_VALID_HANDLE(h) ((NULL != (h)) && ((h) != INVALID_HANDLE_VALUE))
#define _countof(array) (sizeof(array)/sizeof(array[0]))

// server initialization data
SOXpSInitData g_initData =
{
	//-- XPress OPC Server Tool version --
	MAKEWORD(4, 20), // major version, minor version
	{ 0x2e565240, 0xb238, 0x11d3, { 0x84, 0x2d, 0x0, 0x8, 0xc7, 0x79, 0xd7, 0x75 } },
	// class id of OPC DA Server {2E565240-B238-11d3-842D-0008C779D775}
	_T("Softing.OPCToolboxSmpl_XpSDACpp.1"),            // ProdId of OPC DA Server
	_T("Softing.OPCToolboxSmpl_XpSDACpp"),              // version independent ProdId of OPC DA Server
	_T("Softing OPC Toolkit XPress C++ OPC DA Server (asynchIO-2)"),    // description of OPC DA Server
	{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
	// no OPC AE Server
	NULL,                                               // ProdId of OPC AE Server
	NULL,                                               // version independent ProdId of OPC AE Server
	NULL,                                               // description of OPC AE Server
	3,                                                  // major version number
	20,                                                 // minor version number
	400,                                                // build number
	_T("Softing Industrial Automation GmbH"),           // vendor information
	//-- GUI --
	TRUE,                                               // show the application window
	_T("Softing XPress OPC Server C++ Sample (asyncIO- 2)"),// application title
	IDI_ICON,                                       // application icon

	// Service
	NULL,
	NULL,
	NULL,
	//-- OPC Data Access parameters --
	100, // minimal update rate for a group (ms)
	FALSE,
	// Trace
	NULL,                                               // first trace file
	NULL,                                               // second trace file
	0,                                                  // toggle size of trace files
	0,                                          // information trace
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
	pfnRequestCompleted = completedReq;
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
	DWORD dwSectorsPerCluster = 0;
	DWORD dwBytesPerSector = 0;
	DWORD dwNumberOfFreeClusters = 0;
	DWORD dwTotalNumberOfClusters = 0;
// Use the flags FILE_FLAG_OVERLAPPED and FILE_FLAG_NO_BUFFERING
// to instruct the operating system that asynchronous i/o with no buffering
// should be used for this file
	hFile = ::CreateFile(szFileName,
						 GENERIC_READ | GENERIC_WRITE,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL,
						 OPEN_ALWAYS,
						 FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
						 NULL);
// An application must meet certain requirements when working with files opened
// with FILE_FLAG_NO_BUFFERING:
//
// File access must begin at byte offsets within the file that are integer multiples
// of the volume's sector size. To determine a volume's sector size, we call the
// GetDiskFreeSpace function.
//
// File access must be for numbers of bytes that are integer multiples
// of the volume's sector size. For example, if the sector size is 512 bytes,
// an application can request reads and writes of 512, 1024, or 2048 bytes, but
// not of 335, 981, or 7171 bytes.
//
// Buffer addresses for read and write operations must be sector
// aligned (aligned on addresses in memory that are integer multiples of
// the volume's sector size). One way to sector align buffers is to use
// the VirtualAlloc function to allocate the buffers.
// This function allocates memory that is aligned on addresses that are
// integer multiples of the system's page size. Because both page and
// volume sector sizes are powers of 2, memory aligned by multiples of the
// system's page size is also aligned by multiples of the volume's sector size.
	::GetDiskFreeSpace(NULL,  // use the root of the current directory
					   &dwSectorsPerCluster,
					   &dwBytesPerSector,
					   &dwNumberOfFreeClusters,
					   &dwTotalNumberOfClusters);
	g_dwBytesPerSector = dwBytesPerSector;
	return (hFile != INVALID_HANDLE_VALUE);
} // SOXpSStart


//-----------------------------------------------------------------------------
// SOXpSStop
//
// - stops additional thread of the DLL
// - this function is called before termination of the OPC server
//
void SOXPSCFG_CALL SOXpSStop(void)
{
	if (IS_VALID_HANDLE(hFile))
	{
		::CloseHandle(hFile);
	}

	hFile = NULL;
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
	createTag(NULL, _T("FileContent"), VT_BSTR, SOXPS_ACCESSRIGHT_READABLE, SOXPS_IOMODE_QUEUE_POLL, 0, &tag);
} // SOXpSBuildDANameSpace


//-----------------------------------------------------------------------------
// fileIOCompletionRoutine
//
// - completion routine of ReadFileEx
//
VOID CALLBACK fileIOCompletionRoutine(
	DWORD dwErrorCode,                // completion code
	DWORD dwNumberOfBytesTransfered,  // number of bytes transferred
	LPOVERLAPPED lpOverlapped)        // I/O information buffer
{
	// Calculate the pointer to the CDataRequest from
	//the pointer to it's m_ov member
	CDataRequest* pdrq = (CDataRequest*)(((LPBYTE)lpOverlapped) - offsetof(CDataRequest, m_ov));
	pdrq->m_rq.m_requestState = SOXPS_REQUEST_STATE_HANDLED;

	if (0 == dwErrorCode)
	{
		LPWSTR lpwszBuff = (LPWSTR)_alloca(sizeof(WCHAR) * (dwNumberOfBytesTransfered + 1));
		pdrq->m_rq.m_result = S_OK;
		pdrq->m_rq.m_quality = SOXPS_QUALITY_GOOD;
		LPVARIANT pv = &pdrq->m_rq.m_value;
		// Convert the buffer to UNICODE
		DWORD dwChars = ::MultiByteToWideChar(CP_ACP, 0, (LPSTR)pdrq->m_lpBuffer,
											  (int)dwNumberOfBytesTransfered,
											  lpwszBuff, (int)dwNumberOfBytesTransfered);
		lpwszBuff[dwChars] = 0;
		V_VT(pv) = VT_BSTR;
		V_BSTR(pv) = ::SysAllocString(lpwszBuff);
	}
	else
	{
		pdrq->m_rq.m_result = HRESULT_FROM_WIN32(dwErrorCode);
		pdrq->m_rq.m_quality = SOXPS_QUALITY_BAD;
	}

	pfnRequestCompleted(1, &pdrq->m_rq);
	delete pdrq;
} // fileIOCompletionRoutine


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
	for (DWORD i = 0; i < numRequests; i++)  // handle all requests
	{
		CDataRequest* pdrq = NULL;

		try
		{
			if (tag == requestArray[i].m_itemHandle)
			{
				_ASSERTE(IS_VALID_HANDLE(hFile));
				_ASSERTE(SOXPS_REQUEST_OPERATION_READ == ioOperation);
				// Read the first file sector
				pdrq =  new CDataRequest(g_dwBytesPerSector, requestArray[i]);

				if (NULL == pdrq)
				{
					throw E_OUTOFMEMORY;
				}

				LPOVERLAPPED lpOverlapped = &pdrq->m_ov;
				lpOverlapped.hEvent = CreateEvent(
										  NULL,    // default security attribute
										  TRUE,    // manual reset event
										  TRUE,    // initial state = signaled
										  NULL);   // unnamed event object
				// Start overlapped i/o. The result of the operation will be reported
				// in the fileIOCompletionRoutine. It will also free the corresponding
				// CDataRequest instance
				BOOL ret = ::ReadFileEx(hFile, pdrq->m_lpBuffer, pdrq->m_dwSize,
										lpOverlapped,
										(LPOVERLAPPED_COMPLETION_ROUTINE) fileIOCompletionRoutine);

				if (!ret)
				{
					throw E_FAIL;
				}
			}
			else
			{
				_ASSERTE(false); // this should never be reached
			}
		}
		catch (HRESULT hr)
		{
			requestArray[i].m_requestState = SOXPS_REQUEST_STATE_HANDLED;
			requestArray[i].m_result = hr;

			if (pdrq)
			{
				delete pdrq;
			}

			continue;
		}
	}
} // SOXpSHandleDARequests


