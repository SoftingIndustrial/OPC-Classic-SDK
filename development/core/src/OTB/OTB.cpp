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
//                            OPC Toolkit - OTB                               |
//                                                                            |
//  Filename    : OTB.cpp                                                     |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Programming interface of OPC Toolkit(OTB)                   |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "SOVersion.h"
#include "OTcommon.h"
#include "SrvApp.h"
#include "OTB.h"
#include "OTC.h"
#include "SOCmnHandleManager.h"



#ifdef SOOS_WINDOWS

#ifdef _DEBUG
#include <crtdbg.h>
#endif

HINSTANCE g_instance = NULL;

#ifndef OTB_BUILD_LIB

//-----------------------------------------------------------------------------
// DllMain
//
// - main routine of the shared library
//
// returns:
//		TRUE
//
BOOL APIENTRY DllMain(
	IN HANDLE /* hModule */,
	IN DWORD  reasonForCall,
	IN LPVOID /* lpReserved */)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		::OTInitLib();
	}
	break;

	case DLL_PROCESS_DETACH:
	{
		::OTExitLib();
	}
	break;
	}

	return TRUE;
} // DllMain

#endif // OTB_BUILD_LIB

#endif // SOOS_WINDOWS


SOCmnMutex s_initMutex;
unsigned int m_initLibCount = 0;

LONG OTAPI_CALL OTInitLib(void)
{
	SOCMN_TRY
	{
		s_initMutex.lock();

		if (m_initLibCount == 0)
		{
			::initSOCmn();
			// set the applications trace handler
			SOCmnTrace* trace = ::getSOCmnTrace();
			SOCmnPointer<TraceHandler> traceHandler;
			SOCMN_ALLOCATE_OBJECT_C(traceHandler, TraceHandler(), (TraceHandler*));
			trace->setTraceHandler(traceHandler);
#ifdef OTB_SERVER
			createServerApp();
#endif // OTB_SERVER
		}

		m_initLibCount++;
		s_initMutex.unlock();
	}
	SOCMN_CATCH(...)
	{
		s_initMutex.unlock();
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTAPI_CALL OTExitLib(void)
{
	SOCMN_TRY
	{
		s_initMutex.lock();
		m_initLibCount--;

		if (m_initLibCount == 0)
		{
#ifdef OTB_SERVER
			destroyServerApp();
#endif // OTB_SERVER
			// release trace handler
			SOCmnTrace* trace = ::getSOCmnTrace();
			trace->setTraceHandler(NULL);
			::exitSOCmn();
			SOCmnHandleManager::destroyInstance();
		}

		s_initMutex.unlock();
	}
	SOCMN_CATCH(...)
	{
		s_initMutex.unlock();
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
// getInstanceHandle
//
// - get the instance handle of the server
//
// returns:
//		instance handle
//
HINSTANCE getInstanceHandle(void)
{
	HINSTANCE handle = NULL;
#ifdef OTB_CLIENT
	handle = g_instance;
#endif
#ifdef OTB_SERVER

	if (!handle)
	{
		handle = getServerApp()->m_instance;
	}

#endif
	return handle;
} // getInstanceHandle


#endif // SOOS_WINDOWS

LONG OTAPI_CALL OTEnableTracing(IN DWORD whatData, IN OTTraceData* pTraceData)
{
	SOCMN_TRY
	{
		if (!pTraceData)
		{
			return E_INVALIDARG;
		}

		SOCmnTrace* pTrace = ::getSOCmnTrace();


		DWORD winMajorVersion = 0;
		DWORD winMinorVersion = 0; 
		DWORD winBuildNr = 0;
		DWORD platformId = 0;
		TCHAR csdVersion[128] = {0};

		{
			OSVERSIONINFO osvi;
			ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			if (::GetVersionEx(&osvi))
			{
				winMajorVersion = osvi.dwMajorVersion;
				winMinorVersion = osvi.dwMinorVersion;
				winBuildNr = osvi.dwBuildNumber;
				platformId = osvi.dwPlatformId;
				_stprintf(csdVersion, osvi.szCSDVersion, _tcslen(osvi.szCSDVersion) > 128 ? 128 : _tcslen(osvi.szCSDVersion));
			}
			else
			{
				DWORD winVersion = GetVersion();
				winMajorVersion = (DWORD)(LOBYTE(LOWORD(winVersion)));
				winMinorVersion = (DWORD)(HIBYTE(LOWORD(winVersion)));

				if (winVersion < 0x80000000)
				{
					winBuildNr = (DWORD)(HIWORD(winVersion));
				}
			}
		}

		TCHAR otbVersion[128];
		_stprintf(otbVersion, _T("OTB%s %u.%u.%u.%u, built with VC++ %u, running on Microsoft Windows NT %u.%u.%u (%u) %s %s"),
			(sizeof(TCHAR) == 1 ? _T("") : _T("u")), MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER, _MSC_VER,
			winMajorVersion, winMinorVersion, winBuildNr, platformId, csdVersion, sizeof(ptrdiff_t) == 4 ? _T("x86") : _T("x64"));

		pTrace->setOtbVersion(otbVersion);

		if (whatData & OT_TRACEDATA_LEVEL)
		{
			if (!(whatData & OT_TRACEDATA_LEVEL_ERR))
			{
				pTraceData->m_errorLevelMask = pTrace->m_traceLevel[SOCMNTRACE_L_ERR];
			}

			if (!(whatData & OT_TRACEDATA_LEVEL_WRN))
			{
				pTraceData->m_warningLevelMask = pTrace->m_traceLevel[SOCMNTRACE_L_WRN];
			}

			if (!(whatData & OT_TRACEDATA_LEVEL_INF))
			{
				pTraceData->m_infoLevelMask = pTrace->m_traceLevel[SOCMNTRACE_L_INF];
			}

			if (!(whatData & OT_TRACEDATA_LEVEL_DEB))
			{
				pTraceData->m_debugLevelMask = pTrace->m_traceLevel[SOCMNTRACE_L_DEB];
			}

			pTrace->setLevels(pTraceData->m_errorLevelMask, pTraceData->m_warningLevelMask, pTraceData->m_infoLevelMask, pTraceData->m_debugLevelMask);
		}

		if (whatData & OT_TRACEDATA_FILE)
		{
			if ((whatData & OT_TRACEDATA_FILE_ENABLE))
			{
				if (!pTraceData->m_enableTraceToFile)
				{
					pTrace->enableTraceToFile(pTraceData->m_enableTraceToFile);
				}
			}

			if ((whatData & OT_TRACEDATA_API_VERSION))
			{
				pTrace->setApiVersion(pTraceData->m_apiVersion);
			}
			else
			{
				pTrace->setApiVersion(_T("TBN/TBC version not set."));
			}

			if ((whatData & (OT_TRACEDATA_FILE & ~OT_TRACEDATA_FILE_ENABLE)))
			{
				SOCmnString name;
				unsigned long maxBackups;
				unsigned long maxFileSize;

				if ((whatData & OT_TRACEDATA_FILE_NAME))
				{
					name = pTraceData->m_fileName;
				}
				else
				{
					name = pTrace->getFileName();
				}

				if ((whatData & OT_TRACEDATA_FILE_SIZE))
				{
					maxFileSize = pTrace->getFileSize();
				}
				else
				{
					maxFileSize = pTraceData->m_maxFileSize;
				}

				if ((whatData & OT_TRACEDATA_MAX_BACKUPS))
				{
					maxBackups = pTraceData->m_maxBackups;
				}
				else
				{
					maxBackups = pTrace->getMaxBackups();
				}

				pTrace->setFile2(name, maxBackups, pTraceData->m_maxFileSize);
			}

			if ((whatData & OT_TRACEDATA_FILE_ENABLE))
			{
				if (pTraceData->m_enableTraceToFile)
				{
					pTrace->enableTraceToFile(pTraceData->m_enableTraceToFile);
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}


//-----------------------------------------------------------------------------
// OTCTrace
//
// - writes one line to the OTC trace
//
// returns:
//		S_OK                - traced message
//		OT_E_EXCEPTION     - exception in function
//
LONG OTAPI_CALL OTTrace(
	IN BYTE traceLevel, // trace level
	IN DWORD traceMask, // trace mask
	IN LPTSTR objId,   // trace id
	IN LPTSTR text)    // trace message
{
	SOCMN_TRY
	{
		SOCmnTrace* pTrace = ::getSOCmnTrace();

		if (pTrace->m_traceLevel[traceLevel] & traceMask)
		{
			pTrace->trace(traceLevel, traceMask, objId, text);
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
} // OTCTrace


LPTSTR OTAPI_CALL OTVariantToString(IN OTVariant* pValue, IN DWORD stringLen, IN LPTSTR pString)
{
	SOCmnString str;
	::variant2string(&str, pValue);
	_tcsncpy(pString, (LPCTSTR)str, stringLen - 1);
	pString[stringLen - 1 ] = 0;
	return pString;
}

#ifdef OTB_USER_COMPILED
// the Softing version is inside the private file License.cpp !
LONG OTAPI_CALL OTActivate(IN BYTE feature, IN LPTSTR key, IN BYTE forceDemo)
{
	!! YOU HAVE TO ADD YOUR LICENSE CODES IN THIS FUNCTION, and afterwards remove this line !!
	SOCmnTrace *pTrace = ::getSOCmnTrace();

	switch (feature)
	{
	case OTS_FEATURE_DA_SERVER:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("OPC DA Server"));
			return S_OK;
		}

		break;

	case OTS_FEATURE_XMLDA_SERVER:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("XMLDA Server"));
			return S_OK;
		}

		break;

	case OTC_FEATURE_DA_CLIENT:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("OPC DA Client"));
			return S_OK;
		}

		break;

	case OTC_FEATURE_XMLDA_CLIENT:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("XMLDA Client"));
			return S_OK;
		}

		break;

	case OTS_FEATURE_AE_SERVER:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("OPC AE Server"));
			return S_OK;
		}

		break;

	case OTC_FEATURE_AE_CLIENT:
		if (_tcscmp(key, _T("!! ADD YOUR LICENSE CODE HERE !!")) == 0)
		{
			pTrace->addLicenseInfo(_T("OPC AE Client"));
			return S_OK;
		}

		break;
	}

	return OT_E_INVALIDFEATURE;
}
#else
#include "private/License.cpp"
#endif


void* OTAPI_CALL OTAllocateMemory(DWORD size)
{
	void* pMemory;
	pMemory = malloc(size);
	memset(pMemory, 0, size);
	return pMemory;
}

void OTAPI_CALL OTFreeMemory(void* pMemory)
{
	if (pMemory)
	{
		free(pMemory);
	}
}


void SetOutputStructMemberString(const SOCmnString& string, LPTSTR* pStructString)
{
	*pStructString = (LPTSTR)OTAllocateMemory((string.getLength() + 1) * sizeof(TCHAR));
	_tcscpy(*pStructString, string);
}

void SetOutputStructMemberDWORDArray(SOCmnDWORDList* pDwordList, DWORD* pArrayCount, DWORD** ppDwordArray)
{
	DWORD listCount = pDwordList->getCount();
	SOCmnListPosition pos;
	DWORD i;

	if (*pArrayCount == 0)
	{
		*pArrayCount = listCount;
		*ppDwordArray = (DWORD*)OTAllocateMemory(listCount * sizeof(DWORD));
	}

	pos = pDwordList->getStartPosition();
	i = 0;

	while (pos)
	{
		if (i < *pArrayCount)
		{
			(*ppDwordArray)[i] = pDwordList->getNext(pos);
		}
		else
		{
			break;
		}

		i++;
	}

	*pArrayCount = listCount;
}

void SetOutputStructMemberStringArray(SOCmnStringList* pStringList, DWORD* pArrayCount, LPTSTR** ppStringArray)
{
	DWORD listCount = pStringList->getCount();
	SOCmnListPosition pos;
	DWORD i;
	SOCmnString strVal;

	if (*pArrayCount == 0)
	{
		*pArrayCount = listCount;
		*ppStringArray = (LPTSTR*)OTAllocateMemory(listCount * sizeof(LPTSTR));
	}

	pos = pStringList->getStartPosition();
	i = 0;

	while (pos)
	{
		if (i < *pArrayCount)
		{
			strVal = pStringList->getNext(pos);
			(*ppStringArray)[i] = (LPTSTR)OTAllocateMemory((strVal.getLength() + 1) * sizeof(TCHAR));
			_tcscpy((*ppStringArray)[i], (LPCTSTR)strVal);
		}
		else
		{
			break;
		}

		i++;
	}

	*pArrayCount = listCount;
}



void FreeOTSAddressSpaceElementData(IN OTSAddressSpaceElementData* pData)
{
	if (pData)
	{
		if (pData->m_itemID)
		{
			OTFreeMemory(pData->m_itemID);
			pData->m_itemID = NULL;
		}

		if (pData->m_name)
		{
			OTFreeMemory(pData->m_name);
			pData->m_name = NULL;
		}
	}
}

void FreeOTSPropertyData(IN OTSPropertyData* pPropDataO)
{
	if (pPropDataO->m_itemID)
	{
		OTFreeMemory((void*)pPropDataO->m_itemID);
	}

	if (pPropDataO->m_descr)
	{
		OTFreeMemory((void*)pPropDataO->m_descr);
	}

	if (pPropDataO->m_name)
	{
		OTFreeMemory((void*)pPropDataO->m_name);
	}
}



//-----------------------------------------------------------------------------
// TraceHandler
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getUserMaskString
//
// - fill trace mask string for user groups (msx 3 characters !)
//
void TraceHandler::getUserMaskString(
	IN DWORD mask,          // trace group
	OUT LPTSTR string)  // mask string
{
	switch (mask)
	{
	case 0x80000000:
		_tcscpy(string, _T("U1 "));
		break;

	case 0x40000000:
		_tcscpy(string, _T("U2 "));
		break;

	case 0x20000000:
		_tcscpy(string, _T("U3 "));
		break;

	case 0x10000000:
		_tcscpy(string, _T("U4 "));
		break;

	case 0x08000000:
		_tcscpy(string, _T("U5 "));
		break;

	case 0x04000000:
		_tcscpy(string, _T("U6 "));
		break;

	case 0x02000000:
		_tcscpy(string, _T("U7 "));
		break;

	case 0x01000000:
		_tcscpy(string, _T("U8 "));
		break;
	}
} // getUserMaskString

void TraceHandler::onTrace(
	IN LPCTSTR traceString,
	IN USHORT /* traceStringLen */, // TODO check why unreferenced
	IN USHORT level,
	IN DWORD mask,
	IN LPCTSTR objId,
	IN LPCTSTR text)
{
	OTOnTrace serverTraceCallback = NULL;
#ifdef OTB_SERVER
	// server
	ServerApp* pServerApp = ::getServerApp();

	if (pServerApp)
	{
		if (pServerApp->m_callbackFunctions.m_OTOnTrace)
		{
			serverTraceCallback = pServerApp->m_callbackFunctions.m_OTOnTrace;

			SOCMN_TRY
			{
				pServerApp->m_callbackFunctions.m_OTOnTrace((LPTSTR)traceString, level, mask, (LPTSTR)objId, (LPTSTR)text);
			}
			SOCMN_CATCH(...)
			{
			}
		}
	}

#endif // OTB_SERVER
#ifdef OTB_CLIENT

	// client
	if (g_callbackFunctions.m_OTOnTrace)
	{
		if (g_callbackFunctions.m_OTOnTrace)
		{
			if (g_callbackFunctions.m_OTOnTrace != serverTraceCallback)
			{
				SOCMN_TRY
				{
					g_callbackFunctions.m_OTOnTrace((LPTSTR)traceString, level, mask, (LPTSTR)objId, (LPTSTR)text);
				}
				SOCMN_CATCH(...)
				{
				}
			}
		}
	}

#endif // OTB_CLIENT
}



BSTR OTAPI_CALL OTSysAllocString(OTChar* sz)
{
	USES_CONVERSION;
	return SysAllocString(T2COLE(sz));
}

BSTR OTAPI_CALL OTSysAllocStringLen(OTChar* pch, unsigned long cch)
{
	USES_CONVERSION;
	return SysAllocStringLen(T2COLE(pch), cch);
}
void OTAPI_CALL OTSysFreeString(BSTR bstr)
{
	SysFreeString(bstr);
}
unsigned long OTAPI_CALL OTSysStringLen(BSTR bstr)
{
	return SysStringLen(bstr);
}

OTChar* OTAPI_CALL OTSysBstrToString(BSTR bstr)
{
	USES_CONVERSION;
	return OLE2T(bstr);
}

void OTAPI_CALL OTVariantInit(LPVARIANT pvarg)
{
	VariantInit(pvarg);
}
long OTAPI_CALL OTVariantClear(LPVARIANT pvarg)
{
	return SOCmnVariant::variantClear(pvarg);
}
long OTAPI_CALL OTVariantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc)
{
	return SOCmnVariant::variantCopy(pvargDest, pvargSrc);
}
long OTAPI_CALL OTVariantChangeType(LPVARIANT pvargDest, LPVARIANT pvarSrc, unsigned short wFlags, VARTYPE vt)
{
	return VariantChangeType(pvargDest, pvarSrc, wFlags, vt);
}
LPSAFEARRAY OTAPI_CALL OTSafeArrayCreate(VARTYPE vt, unsigned short cDims, LPSAFEARRAYBOUND rgsabound)
{
	return SOCmnVariant::safeArrayCreate(vt, cDims, rgsabound);
}
LPSAFEARRAY OTAPI_CALL OTSafeArrayCreateVector(VARTYPE vt, long lLbound, unsigned long cElements)
{
	return SOCmnVariant::safeArrayCreateVector(vt, lLbound, cElements);
}
long OTAPI_CALL OTSafeArrayDestroy(LPSAFEARRAY psa)
{
	return SafeArrayDestroy(psa);
}
long OTAPI_CALL OTSafeArrayCopy(LPSAFEARRAY psa, LPSAFEARRAY* ppsaOut)
{
	return SafeArrayCopy(psa, ppsaOut);
}
long OTAPI_CALL OTSafeArrayRedim(SAFEARRAY* psa, SAFEARRAYBOUND* psaboundNew)
{
	return SafeArrayRedim(psa, psaboundNew);
}
unsigned short OTAPI_CALL OTSafeArrayGetDim(LPSAFEARRAY psa)
{
	return (short)SafeArrayGetDim(psa);
}
unsigned long OTAPI_CALL OTSafeArrayGetElemsize(LPSAFEARRAY psa)
{
	return SafeArrayGetElemsize(psa);
}
long OTAPI_CALL OTSafeArrayGetUBound(LPSAFEARRAY psa, unsigned short nDim, long* plUbound)
{
	return SafeArrayGetUBound(psa, nDim, plUbound);
}
long OTAPI_CALL OTSafeArrayGetLBound(LPSAFEARRAY psa, unsigned short nDim, long* plLbound)
{
	return SafeArrayGetLBound(psa, nDim, plLbound);
}
long OTAPI_CALL OTSafeArrayAccessData(LPSAFEARRAY psa, void** ppvData)
{
	return SafeArrayAccessData(psa, ppvData);
}
long OTAPI_CALL OTSafeArrayUnaccessData(LPSAFEARRAY psa)
{
	return SafeArrayUnaccessData(psa);
}
long OTAPI_CALL OTSafeArrayGetElement(LPSAFEARRAY psa, long* rgIndices, void* pv)
{
	return SafeArrayGetElement(psa, rgIndices, pv);
}
long OTAPI_CALL OTSafeArrayPutElement(LPSAFEARRAY psa, long* rgIndices, void* pv)
{
	return SafeArrayPutElement(psa, rgIndices, pv);
}

#ifdef OTB_SERVER
#ifndef OTB_USER_COMPILED
#ifdef SOFEATURE_DCOM
#include "private/SoftingServer.cpp"
#endif // SOFEATURE_DCOM
#endif // OTB_USER_COMPILED
#endif // OTB_SERVER
