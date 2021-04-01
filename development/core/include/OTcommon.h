#ifndef _OTCOMMON_H_
#define _OTCOMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif



#if defined(WIN32) || defined(WIN64)
#ifndef SOOS_WINDOWS
#define SOOS_WINDOWS
#endif
#endif // WIN32

#ifdef __linux__
#ifndef SOOS_LINUX
#define SOOS_LINUX
#endif
#endif // __linux__

#if !defined(SOOS_WINDOWS) && !defined(SOOS_LINUX)
#error fatal: unsupported platform!
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef SOOS_WINDOWS_CE
#include <stddef.h>
#endif
#include <stdarg.h>
#include <malloc.h>
#ifndef SOOS_WINDOWS_CE
#include <time.h>
#endif
#pragma pack(push, 4)

//-----------------------------------------------------------------------------
// Defines                                                                    |
//-----------------------------------------------------------------------------

// calling convention
#ifdef SOOS_WINDOWS
#define OTAPI_CALL __stdcall
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
#define OTAPI_CALL
#endif // SOOS_LINUX

#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((long)(void *)(Status) >= 0)
#define FAILED(Status) ((long)(void *)(Status)<0)
#define S_OK            0x00000000L
#define S_FALSE         0x00000001L
#define E_NOTIMPL       0x80004001L
#define E_FAIL          0x80004005L
#endif // SUCCEEDED

#define OT_E_OPC_INVALIDHANDLE              0xC0040001
#define OT_E_OPC_BADTYPE                    0xC0040004
#define OT_E_OPC_PUBLIC                     0xC0040005
#define OT_E_OPC_BADRIGHTS                  0xC0040006
#define OT_E_OPC_UNKNOWNITEMID              0xC0040007
#define OT_E_OPC_INVALIDITEMID              0xC0040008
#define OT_E_OPC_INVALIDFILTER              0xC0040009
#define OT_E_OPC_UNKNOWNPATH                0xC004000A
#define OT_E_OPC_RANGE                      0xC004000B
#define OT_E_OPC_DUPLICATENAME              0xC004000C
#define OT_S_OPC_UNSUPPORTEDRATE            0x0004000D
#define OT_S_OPC_CLAMP                      0x0004000E
#define OT_S_OPC_INUSE                      0x0004000F
#define OT_E_OPC_INVALIDCONFIGFILE          0xC0040010
#define OT_E_OPC_NOTFOUND                   0xC0040011
#define OT_E_OPC_INVALID_PID                0xC0040203
#define OT_E_OPC_DEADBANDNOTSET             0xC0040400
#define OT_E_OPC_DEADBANDNOTSUPPORTED       0xC0040401
#define OT_E_OPC_NOBUFFERING                0xC0040402
#define OT_E_OPC_INVALIDCONTINUATIONPOINT   0xC0040403
#define OT_S_OPC_DATAQUEUEOVERFLOW          0x00040404
#define OT_E_OPC_RATENOTSET                 0xC0040405
#define OT_E_OPC_NOTSUPPORTED               0xC0040406

// error codes
#define OT_E_WRONG_VERSION     0xCDEF0001
#define OT_E_INVALIDOBJECTTYPE 0xCDEF0002
#define OT_E_INVALIDHANDLE     0xCDEF0003
#define OT_E_INVALIDDATA       0xCDEF0004
#define OT_E_DUPLICATENAME     0xCDEF0005
#define OT_E_INVALIDLICENSE    0xCDEF0006
#define OT_E_INVALIDFEATURE    0xCDEF0007
#define OT_E_CANTEXECUTEASYNC  0xCDEF0008
#define OT_E_INVALIDADDRESSSPACEELEMENTTYPE 0xCDEF0009
#define OT_E_NOTLICENSED       0xCDEF000A
#ifdef SOOS_WINDOWS
#define OT_E_EXCEPTION         HRESULT_FROM_WIN32(ERROR_EXCEPTION_IN_SERVICE)
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
#define OT_E_EXCEPTION          0xCDEB0001
#endif // SOOS_LINUX

#define OT_TRACEMASK_ALL        0xFFFF000F
#define OT_TRACEMASK_NOTHING    0x00000000
#define OT_TRACEMASK_USER       0xFF000000
#define OT_TRACEMASK_COPC       0x0000000C
#define OT_TRACEMASK_COPCCALL   0x00000004
#define OT_TRACEMASK_COPCADVISE 0x00000008
#define OT_TRACEMASK_SOPC       0x00000003
#define OT_TRACEMASK_SOPCCALL   0x00000001
#define OT_TRACEMASK_SOPCADVISE 0x00000002
#define OT_TRACEMASK_TBCLIENT   0x00800000
#define OT_TRACEMASK_TBSERVER   0x00400000
#define OT_TRACEMASK_OTBLICENSE 0x00200000
#define OT_TRACEMASK_OTSIN      0x00010000
#define OT_TRACEMASK_OTSOUT     0x00020000
#define OT_TRACEMASK_OTCIN      0x00040000
#define OT_TRACEMASK_OTCOUT     0x00080000
#define OT_TRACEMASK_USER1      0x80000000
#define OT_TRACEMASK_USER2      0x40000000
#define OT_TRACEMASK_USER3      0x20000000
#define OT_TRACEMASK_USER4      0x10000000
#define OT_TRACEMASK_USER5      0x08000000
#define OT_TRACEMASK_USER6      0x04000000
#define OT_TRACEMASK_USER7      0x02000000
#define OT_TRACEMASK_USER8      0x01000000

#define OT_TRACELEVEL_ERR 0
#define OT_TRACELEVEL_WRN 1
#define OT_TRACELEVEL_INF 2
#define OT_TRACELEVEL_DEB 3

#define OT_QUALITY_NOT_SET 0xFFFF

#define OT_QUALITY_MASK_QUALITY             0xc0
#define OT_QUALITY_MASK_STATUS              0xfc
#define OT_QUALITY_MASK_LIMIT               0x03
#define OT_QUALITY_BAD                      0x00
#define OT_QUALITY_UNCERTAIN                0x40
#define OT_QUALITY_GOOD                     0xc0
#define OT_QUALITY_CONFIG_ERROR             0x04
#define OT_QUALITY_NOT_CONNECTED            0x08
#define OT_QUALITY_DEVICE_FAILURE           0x0c
#define OT_QUALITY_SENSOR_FAILURE           0x10
#define OT_QUALITY_LAST_KNOWN               0x14
#define OT_QUALITY_COMM_FAILURE             0x18
#define OT_QUALITY_OUT_OF_SERVICE           0x1c
#define OT_QUALITY_WAITING_FOR_INITIAL_DATA 0x20
#define OT_QUALITY_LAST_USABLE              0x44
#define OT_QUALITY_SENSOR_CAL               0x50
#define OT_QUALITY_EGU_EXCEEDED             0x54
#define OT_QUALITY_SUB_NORMAL               0x58
#define OT_QUALITY_LOCAL_OVERRIDE           0xd8
#define OT_QUALITY_LIMIT_OK                 0x00
#define OT_QUALITY_LIMIT_LOW                0x01
#define OT_QUALITY_LIMIT_HIGH               0x02
#define OT_QUALITY_LIMIT_CONST              0x03

#define OT_ACCESSRIGHTS_READABLE            0x01
#define OT_ACCESSRIGHTS_WRITEABLE           0x02

#define OT_OPCSPECIFICATION_DEFAULT  0x00
#define OT_OPCSPECIFICATION_DA10     0x03
#define OT_OPCSPECIFICATION_DA20     0x04
#define OT_OPCSPECIFICATION_DA30     0x10
#define OT_OPCSPECIFICATION_XMLDA10  0x20
#define OT_OPCSPECIFICATION_AE10     0x08
#define OT_OPCSPECIFICATION_ALL      0xFF

#define OT_ADDRESSSPACETYPE_ALL     0xFF
#define OT_ADDRESSSPACETYPE_BRANCH  0x01
#define OT_ADDRESSSPACETYPE_LEAF    0x02
#define OT_ADDRESSSPACETYPE_ITEM    0x04

#define OT_EVENTTYPE_SIMPLE     0x01
#define OT_EVENTTYPE_TRACKING   0x02
#define OT_EVENTTYPE_CONDITION  0x04

#define OT_CONDITIONSTATE_ENABLED 0x01
#define OT_CONDITIONSTATE_ACTIVE  0x02
#define OT_CONDITIONSTATE_ACKED   0x04

#define OT_CONDITIONCHANGE_ACTIVE_STATE 0x0001
#define OT_CONDITIONCHANGE_ACK_STATE    0x0002
#define OT_CONDITIONCHANGE_ENABLE_STATE 0x0004
#define OT_CONDITIONCHANGE_STATE        0x0007
#define OT_CONDITIONCHANGE_QUALITY      0x0008
#define OT_CONDITIONCHANGE_SEVERITY     0x0010
#define OT_CONDITIONCHANGE_SUBCONDITION 0x0020
#define OT_CONDITIONCHANGE_MESSAGE      0x0040
#define OT_CONDITIONCHANGE_ATTRIBUTE    0x0080
#define OT_CONDITIONCHANGE_ACK_DATA     0x8000

#define OT_FILTER_BY_EVENT    0x01
#define OT_FILTER_BY_CATEGORY 0x02
#define OT_FILTER_BY_SEVERITY 0x04
#define OT_FILTER_BY_AREA     0x08
#define OT_FILTER_BY_SOURCE   0x10

#define OT_ADDRESSSPACEELEMENT_TYPE_DA 0x01
#define OT_ADDRESSSPACEELEMENT_TYPE_AE 0x02

#define OT_TRACEDATA_ALL         0x000001FF
#define OT_TRACEDATA_LEVEL       0x0000000F
#define OT_TRACEDATA_LEVEL_ERR   0x00000008
#define OT_TRACEDATA_LEVEL_WRN   0x00000004
#define OT_TRACEDATA_LEVEL_INF   0x00000002
#define OT_TRACEDATA_LEVEL_DEB   0x00000001
#define OT_TRACEDATA_FILE        0x000000F0
#define OT_TRACEDATA_FILE_ENABLE 0x00000010
#define OT_TRACEDATA_FILE_NAME   0x00000020
#define OT_TRACEDATA_MAX_BACKUPS 0x00000040
#define OT_TRACEDATA_FILE_SIZE   0x00000080
#define OT_TRACEDATA_API_VERSION 0x00000100


//-----------------------------------------------------------------------------
// VARIANT                                                                    |
//-----------------------------------------------------------------------------

#ifndef SOFEATURE_CMN_VARIANT

#ifdef SOOS_LINUX

	typedef unsigned short VARTYPE;
	typedef short VARIANT_BOOL;
#define VARIANT_TRUE ((VARIANT_BOOL)-1)
#define VARIANT_FALSE ((VARIANT_BOOL)0)

	typedef union tagCY
	{
		struct
		{
#ifndef SODEVENV_CYGWIN
#if __BYTE_ORDER == __BIG_ENDIAN
			long      Hi;
			unsigned long Lo;
#else
			unsigned long Lo;
			long      Hi;
#endif
#else
			unsigned long Lo;
			long      Hi;
#endif
		};
		long long int64;
	} CY;

	typedef char* BSTR;
	typedef CY CURRENCY;
	typedef struct timeval DATE;
	typedef struct  tagSAFEARRAYBOUND
	{
		unsigned long cElements;
		long lLbound;
	}   SAFEARRAYBOUND;

	typedef struct tagSAFEARRAYBOUND* LPSAFEARRAYBOUND;

	typedef struct  tagSAFEARRAY
	{
		unsigned short cDims;
		unsigned short fFeatures;
		unsigned long cbElements;
		unsigned long cLocks;
		void* pvData;
		SAFEARRAYBOUND rgsabound[ 1 ];
	}   SAFEARRAY;
	typedef SAFEARRAY* LPSAFEARRAY;

#define FADF_BSTR       0x100
#define FADF_UNKNOWN    0x200
#define FADF_DISPATCH   0x400
#define FADF_VARIANT    0x800

	typedef struct tagVARIANT VARIANT;

	struct tagVARIANT
	{
		VARTYPE vt;
		unsigned short wReserved1;
		unsigned short wReserved2;
		unsigned short wReserved3;
		union
		{
			long lVal;
			unsigned char bVal;
			short iVal;
			float fltVal;
			double dblVal;
			VARIANT_BOOL boolVal;
			CY cyVal;
			DATE date;
			BSTR bstrVal;
			SAFEARRAY* parray;
			VARIANT* pvarVal;
			char cVal;
			unsigned short uiVal;
			unsigned long ulVal;
			long long llVal;   /* <-- NOT UNDER WINDOWS, use V_I8(&var)  */
			unsigned  long long ullVal; /* <-- NOT UNDER WINDOWS, use V_UI8(&var) */
			int intVal;
			unsigned int uintVal;
		};
	};

	typedef VARIANT* LPVARIANT;

	enum VARENUM
	{
		VT_EMPTY    = 0,
		VT_I2   = 2,
		VT_I4   = 3,
		VT_R4   = 4,
		VT_R8   = 5,
		VT_CY   = 6,
		VT_DATE = 7,
		VT_BSTR = 8,
		VT_BOOL = 11,
		VT_VARIANT  = 12,
		VT_I1   = 16,
		VT_UI1  = 17,
		VT_UI2  = 18,
		VT_UI4  = 19,
		VT_I8   = 20,
		VT_UI8  = 21,
		VT_INT  = 22,
		VT_UINT = 23,
		VT_ARRAY    = 0x2000,
		VT_RESERVED = 0x8000,
		VT_ILLEGAL  = 0xffff,
		VT_ILLEGALMASKED    = 0xfff,
		VT_TYPEMASK = 0xfff
	};

#define V_UNION(X, Y)   ((X)->Y)
#define V_VT(X)         ((X)->vt)
#define V_ISARRAY(X)     (V_VT(X)&VT_ARRAY)
#define V_UI1(X)         V_UNION(X, bVal)
#define V_I2(X)          V_UNION(X, iVal)
#define V_I4(X)          V_UNION(X, lVal)
#define V_I8(X)          V_UNION(X, llVal)
#define V_R4(X)          V_UNION(X, fltVal)
#define V_R8(X)          V_UNION(X, dblVal)
#define V_I1(X)          V_UNION(X, cVal)
#define V_UI2(X)         V_UNION(X, uiVal)
#define V_UI4(X)         V_UNION(X, ulVal)
#define V_UI8(X)         V_UNION(X, ullVal)
#define V_INT(X)         V_UNION(X, intVal)
#define V_UINT(X)        V_UNION(X, uintVal)
#define V_CY(X)          V_UNION(X, cyVal)
#define V_DATE(X)        V_UNION(X, date)
#define V_BSTR(X)        V_UNION(X, bstrVal)
#define V_BOOL(X)        V_UNION(X, boolVal)
#define V_ARRAY(X)       V_UNION(X, parray)

// Flags for VariantChangeType/VariantChangeTypeEx
#define VARIANT_NOVALUEPROP      0x01
#define VARIANT_ALPHABOOL        0x02 // For VT_BOOL to VT_BSTR conversions,
	// convert to "True"/"False" instead of
	// "-1"/"0"
#define VARIANT_NOUSEROVERRIDE   0x04 // For conversions to/from VT_BSTR,
	// passes LOCALE_NOUSEROVERRIDE
	// to core coercion routines
#define VARIANT_CALENDAR_HIJRI   0x08
#define VARIANT_LOCALBOOL        0x10 // For VT_BOOL to VT_BSTR and back,
	// convert to local language rather than
	// English
#endif // SOOS_LINUX

#ifndef V_I8
#define V_I8(X)          ((X)->cyVal.int64)
#endif
#ifndef V_UI8
#define V_UI8(X)         (*(ULONGLONG*)&V_I8(X))
#endif

#endif // SOFEATURE_CMN_VARIANT


//-----------------------------------------------------------------------------
// type definitions                                                           |
//-----------------------------------------------------------------------------

#ifdef SOOS_WINDOWS
	typedef VARIANT OTVariant;
	typedef FILETIME OTDateTime;
	typedef TCHAR OTChar;
#endif
#ifdef SOOS_LINUX
	typedef VARIANT OTVariant;
	typedef struct timeval OTDateTime;
	typedef char OTChar;
#endif

	typedef int OTBool;
	typedef unsigned long OTObjectHandle;

//-----------------------------------------------------------------------------
// Structures                                                                 |
//-----------------------------------------------------------------------------

	typedef struct _OTObjectData
	{
		OTObjectHandle m_objectHandle;
		unsigned long m_userData;
	} OTObjectData;

	typedef struct _OTValueData
	{
		OTVariant* m_pValue;
		OTDateTime m_timestamp;
		unsigned short m_quality;
	} OTValueData;

	typedef struct _OTTraceData
	{
		unsigned int m_errorLevelMask;
		unsigned int m_warningLevelMask;
		unsigned int m_infoLevelMask;
		unsigned int m_debugLevelMask;
		OTChar* m_fileName;
		OTChar* m_apiVersion;
		unsigned int m_maxFileSize;
		unsigned int m_maxBackups;
		unsigned char m_enableTraceToFile;
	} OTTraceData;

// functions
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

	long OTAPI_CALL OTActivate(IN unsigned char feature, IN OTChar* key, IN unsigned char forceDemo);

	long OTAPI_CALL OTEnableTracing(IN unsigned long whatData, IN OTTraceData* pTraceData);

	long OTAPI_CALL OTTrace(IN unsigned char traceLevel, IN unsigned long traceMask, IN OTChar* objId, IN OTChar* text);

	void* OTAPI_CALL OTAllocateMemory(unsigned long size);

	void OTAPI_CALL OTFreeMemory(void* pMemory);

	OTChar* OTAPI_CALL OTVariantToString(IN OTVariant* pValue, IN unsigned long stringLen, IN OTChar* pString);

// static library functions

	long OTAPI_CALL OTInitLib(void);

	long OTAPI_CALL OTExitLib(void);

// callbacks

	typedef void (OTAPI_CALL* OTOnTrace)(IN OTChar* traceString, IN unsigned short level, IN unsigned long mask, IN OTChar* objId, IN OTChar* text);

// VARIANT support

	BSTR OTAPI_CALL OTSysAllocString(OTChar* sz);
	BSTR OTAPI_CALL OTSysAllocStringLen(OTChar* pch, unsigned long cch);
	void OTAPI_CALL OTSysFreeString(BSTR bstr);
	OTChar* OTAPI_CALL OTSysBstrToString(BSTR bstr);
	unsigned long OTAPI_CALL OTSysStringLen(BSTR bstr);
	void OTAPI_CALL OTVariantInit(LPVARIANT pvarg);
	long OTAPI_CALL OTVariantClear(LPVARIANT pvarg);
	long OTAPI_CALL OTVariantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc);
	long OTAPI_CALL OTVariantChangeType(LPVARIANT pvargDest, LPVARIANT pvarSrc, unsigned short wFlags, VARTYPE vt);
	LPSAFEARRAY OTAPI_CALL OTSafeArrayCreate(VARTYPE vt, unsigned short cDims, LPSAFEARRAYBOUND rgsabound);
	LPSAFEARRAY OTAPI_CALL OTSafeArrayCreateVector(VARTYPE vt, long lLbound, unsigned long cElements);
	long OTAPI_CALL OTSafeArrayDestroy(LPSAFEARRAY psa);
	long OTAPI_CALL OTSafeArrayCopy(LPSAFEARRAY psa, LPSAFEARRAY* ppsaOut);
	long OTAPI_CALL OTSafeArrayRedim(SAFEARRAY* psa, SAFEARRAYBOUND* psaboundNew);
	unsigned short OTAPI_CALL OTSafeArrayGetDim(LPSAFEARRAY psa);
	unsigned long OTAPI_CALL OTSafeArrayGetElemsize(LPSAFEARRAY psa);
	long OTAPI_CALL OTSafeArrayGetUBound(LPSAFEARRAY psa, unsigned short nDim, long* plUbound);
	long OTAPI_CALL OTSafeArrayGetLBound(LPSAFEARRAY psa, unsigned short nDim, long* plLbound);
	long OTAPI_CALL OTSafeArrayAccessData(LPSAFEARRAY psa, void** ppvData);
	long OTAPI_CALL OTSafeArrayUnaccessData(LPSAFEARRAY psa);
	long OTAPI_CALL OTSafeArrayGetElement(LPSAFEARRAY psa, long* rgIndices, void* pv);
	long OTAPI_CALL OTSafeArrayPutElement(LPSAFEARRAY psa, long* rgIndices, void* pv);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
