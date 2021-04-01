#ifndef _SOCMNTRACE_H_
#define _SOCMNTRACE_H_

#include "SOCmn.h"
#include "SOCmnObject.h"

#ifdef SOOS_WINDOWS
#ifdef _DEBUG
#if !defined(SOOS_WINDOWS_CE) && !defined(__BORLANDC__)
#include <crtdbg.h>
#endif // !defined(SOOS_WINDOWS_CE) && !defined(__BORLANDC__)
#endif // _DEBUG

#ifdef __BORLANDC__
#include <assert.h>
#endif // __BORLANDC__
#endif // SOOS_WINDOWS

#pragma pack(push, 4)


//	TRACE time diff method
//	NOTE: The performance counter is not reliable on multicore machines therefore the default
//       implementation will use GetTickCount for time resolution
// #define TRACE_USE_PERFORMANCE_COUNTER

//-----------------------------------------------------------------------------
// Trace Groups                                                               |
//-----------------------------------------------------------------------------

#define SOCMNTRACE_G_ALL                   0xFFFF000F
#define SOCMNTRACE_G_NOTHING               0x00000000
#define SOCMNTRACE_G_USER                  0xFF000000
#define SOCMNTRACE_G_OPC                   0x0000000F
#define SOCMNTRACE_G_OPC_CLIENT            0x0000000C
#define SOCMNTRACE_G_OPC_SERVER            0x00000003

#define SOCMNTRACE_G_OPC_CALL              0x00000001
#define SOCMNTRACE_G_OPC_ADVISE            0x00000002
#define SOCMNTRACE_G_OPC_CALL_C            0x00000004
#define SOCMNTRACE_G_OPC_ADVISE_C          0x00000008

#define SOCMNTRACE_G_USER1                 0x10000000
#define SOCMNTRACE_G_USER2                 0x20000000
#define SOCMNTRACE_G_USER3                 0x40000000
#define SOCMNTRACE_G_USER4                 0x80000000
#define SOCMNTRACE_G_USER5                 0x01000000
#define SOCMNTRACE_G_USER6                 0x02000000
#define SOCMNTRACE_G_USER7                 0x04000000
#define SOCMNTRACE_G_USER8                 0x08000000

// toolkit developer traces, use with care !
#define SOCMNTRACE_G_OTB                   0x00FF0000
#define SOCMNTRACE_G_OBJ_REF               0x00000040
#define SOCMNTRACE_G_COM_OBJ_REF           0x00000020
#define SOCMNTRACE_G_OBJ_LOG               0x00000080
#define SOCMNTRACE_G_OBJECTLOG             0x00000010
#define SOCMNTRACE_G_ENTRY                 0x00000800
#define SOCMNTRACE_G_CLT_THREAD            0x00000100
#define SOCMNTRACE_G_SRV_THREAD            0x00000200
#define SOCMNTRACE_G_SUS_FILTER            0x00000400
#define SOCMNTRACE_G_SOCKET                0x00001000
#define SOCMNTRACE_G_SOCKET_DATA           0x00002000
#define SOCMNTRACE_G_SOAP_DATA             0x00004000
#define SOCMNTRACE_G_WEB_SRV               0x00008000
#define SOCMNTRACE_G_DISCOVERY_SRV         0x00010000
#define SOCMNTRACE_G_DISCOVERY_CLT         0x00020000
#define SOCMNTRACE_G_TEST                  0x80000000

// compatibility don't use this in new projects!
#define TG_USER1        0x10000000
#define TG_USER2        0x20000000
#define TG_USER3        0x40000000
#define TG_USER4        0x80000000
#define TG_USER         0xF0000000

//-----------------------------------------------------------------------------
// Trace Levels                                                               |
//-----------------------------------------------------------------------------

#define SOCMNTRACE_L_ERR 0
#define SOCMNTRACE_L_WRN 1
#define SOCMNTRACE_L_INF 2
#define SOCMNTRACE_L_DEB 3

// compatibility don't use this in new projects!
#define TL_ERR  0
#define TL_WRN  1
#define TL_INF  2
#define TL_DEB  3


//-----------------------------------------------------------------------------
// SOCmnTraceReceiver                                                         |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnTraceReceiver : public SOCmnObject
{
public:
	SOCmnTraceReceiver();
	virtual void onTrace(IN LPCTSTR traceString, IN USHORT traceStringLen, IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text);
}; // SOCmnTraceReceiver



//-----------------------------------------------------------------------------
// SOCmnTraceHandler                                                          |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnTraceHandler : public SOCmnTraceReceiver
{
public:
	SOCmnTraceHandler();
	virtual void getUserMaskString(IN DWORD mask, OUT LPTSTR string);
	virtual HRESULT saveTraceOptions(void);
}; // SOCmnTraceHandler

#define GenericTraceHandler SOCmnTraceHandler



//-----------------------------------------------------------------------------
// SOCmnTrace                                                                 |
//-----------------------------------------------------------------------------


#ifndef FORMAT_PRINTF
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR >= 7)
#define FORMAT_PRINTF(m,n) __attribute__ ((__format__ (__printf__, m, n)))
#else
#define FORMAT_PRINTF(m,n)
#endif
#endif


class SOCMN_EXPORT SOCmnTrace
{
public:
	SOCmnTrace();
	~SOCmnTrace();

	SOCmnSync m_lock;               // trace output lock
	BYTE m_dummy_forPacking[2];
	DWORD m_traceLevel[4];      // trace levels
	BYTE m_startedPeriod;

#ifdef SOFEATURE_CMN_TRACE
	// do trace string
	void trace(IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text, IN OPTIONAL BOOL traceHeader = TRUE);
	void traceASCIIString(IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN CHAR* text, IN DWORD textLen);

	// expand trance data
	LPCTSTR expand(IN LPCTSTR format, ...) FORMAT_PRINTF(2, 3);

	// time measurement
	LPCTSTR elapsedTime(SOCmnString& elapsedTime, LONGLONG startTime, LONGLONG endTime);
#endif

	// set trace levels
	void setLevels(IN DWORD error, IN DWORD warning, IN DWORD info,
				   IN DWORD debug);

	//setFile2 and updateTraceFileSettings2: new versions of functions need only one file name
	//that is used as file base name, the actual trace files are: <pszFileBase>_currentA.log and <pszFileBase>_currentB.log
	//This naming convention shall be easier to undertand in respect with the <pszFileBase>_backup#<n>.log files created during startup.
	//we want the user not to bother with two file names during configuration.
	//Also the file extension .log is forced by these new methods.
	BOOL setFile2(IN LPCTSTR pszFileBase,
				  IN DWORD   maxBackups,
				  IN DWORD   maxFileSize
				 );

#ifdef SOFEATURE_CMN_TRACE
	void setApiVersion(IN LPCTSTR apiVersion);
	void addDcomInitStatus(IN LPCTSTR apiVersion);
	void setOtbVersion(IN LPCTSTR otbVersion);
	void addLicenseInfo(IN LPCTSTR licenseInfo);
#endif

	BOOL updateTraceFileSettings2(IN const BOOL* pEnable,       //NULL: do not change
								  IN LPCTSTR      pszFileBase,    //NULL: do not change
								  IN const DWORD* pMaxSize,       //NULL: do not change
								  IN const DWORD* pMaxBackups     //NULL: do not change
								 );
	SOCmnString getFileNameBase(); //returns the currently used file name base
	LPCTSTR getActiveTraceFileName();//returns either <pszFileBase>_currentA.log or <pszFileBase>_currentB.log


	// set trace file, call this method on startup:
	BOOL setFile(IN LPCTSTR file,
				 OPTIONAL IN DWORD maxBackups = 5,
				 OPTIONAL IN LPCTSTR secondFile = NULL,
				 OPTIONAL IN DWORD maxFileSize  = 1000000
				);

	//call this method to change any setting for trace files at runtime,
	//depending on the changes eventually the trace files will be closed
	//and/or re-opened.
	BOOL updateTraceFileSettings(IN const BOOL* pEnable,    //NULL: do not change
								 IN LPCTSTR      pszFile1,   //NULL: do not change
								 IN LPCTSTR      pszFile2,   //NULL: do not change
								 IN const DWORD* pMaxSize,   //NULL: do not change
								 IN const DWORD* pMaxBackups //NULL: do not change
								);

	void setTraceHandler(IN SOCmnTraceHandler* traceHandler);
	SOCmnTraceHandler* getTraceHandler(void);

	SOCmnList<SOCmnTraceReceiver> *getReceiverList(void)
	{
		return &m_recList;
	}

	LPCTSTR getFileName(void)
	{
		return m_fileName;
	}
	LPCTSTR getSecondFileName(void)
	{
		return m_fileName2;
	}
	DWORD getFileSize(void)
	{
		return m_fileSize;
	}
	DWORD getMaxBackups(void)
	{
		return m_maxBackups;
	}
	BOOL enableTraceToFile(BOOL enable);
	BOOL getTraceToFile(void)
	{
		return m_fileTraceOn;
	}

	SOCmnList<SOCmnObject> m_genObj;    // created generic object list
	SOCmnList<void> m_comObj;           // created COM object list

	SOCmnString m_variantBuffer;    // variant conversion buffer
	SOCmnString m_clipformatBuffer; // clipformat conversion buffer
	SOCmnString m_vartypeBuffer;    // vartype conversion buffer
	SOCmnString m_qualityBuffer;    // quality conversion buffer
	SOCmnString m_filetimeBuffer;   // filetime conversion buffer

protected:
	void writeTraceHeader(void);

#ifdef SOFEATURE_CMN_TRACE
#ifdef SOOS_WINDOWS
	HANDLE m_handleTrace;       // trace file handle
	HANDLE m_handleTrace2nd;    // second trace file handle
	HANDLE m_actHandle;         // current trace file handle
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	int m_handleTrace;
	int m_handleTrace2nd;
	int m_actHandle;
#endif // SOOS_LINUX
#endif
	TCHAR m_levelChar[4];       // trace level char
	TCHAR m_userMaskStr[4];     // user mask string
#ifdef SOOS_WINDOWS
	double m_pcPer1ms;
#endif // SOOS_WINDOWS
#ifdef SOFEATURE_CMN_TRACE
	TCHAR m_buffer[512];        // trace buffer
	TCHAR m_expand[512];        // expand buffer
#endif
	SOCmnString m_fileName;
	SOCmnString m_fileName2;

#ifdef SOFEATURE_CMN_TRACE
	SOCmnString m_apiVersion;
	SOCmnString m_dcomInitStatus;
	SOCmnString m_otbVersion;
	SOCmnString m_licenseInfo;
#endif

	SOCmnTraceHandler* m_trcHandler; // trace handler
	SOCmnList<SOCmnTraceReceiver> m_recList;

#ifdef SOOS_WINDOWS
	LARGE_INTEGER m_pcFrequency;
#endif

	DWORD m_fileSize;           // max trace file size
	DWORD m_maxBackups;         // 0..10: max backup copys of trace files on startup
	DWORD m_creationDisposition;//remember
#ifdef SOFEATURE_CMN_TRACE
	DWORD m_noOfTraceFiles;     // how many times the trace buffer was rotated
	DWORD m_lines;              // lines in trace file
	USHORT m_len;               // trace string lenght
#endif
	BOOL m_fileTraceOn;

	// get string for trace mask
	LPCTSTR getMaskString(DWORD mask);

	void closeTraceFiles(void);
}; // SOCmnTrace

inline SOCmnTraceHandler* SOCmnTrace::getTraceHandler(void)
{
	if (m_trcHandler)
	{
		m_trcHandler->addRef();
	}

	return m_trcHandler;
}

#define OPCTrace SOCmnTrace



//-----------------------------------------------------------------------------
// Global Functions                                                           |
//-----------------------------------------------------------------------------

// get pointer of the OPC trace object
SOCMN_EXPORT SOCmnTrace* getOPCTrace(void);
SOCMN_EXPORT SOCmnTrace* getSOCmnTrace(void);
void createTraceObject(void);
void destroyTraceObject(void);


// convert variant to string
#ifdef SOFEATURE_CMN_VARIANT
SOCMN_EXPORT LPCTSTR variant2string(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT, OPTIONAL IN BOOL readFormat = TRUE);

// convert variant to string and add vartype
SOCMN_EXPORT LPCTSTR variantWithType2string(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);

// convert vartype to string
SOCMN_EXPORT LPCTSTR vartype2string(IN SOCmnString* buffer, IN VARTYPE vt);
#endif

#ifdef SOOS_WINDOWS
// convert clipformat to string
SOCMN_EXPORT LPCTSTR clipformat2string(IN SOCmnString* buffer, IN CLIPFORMAT format);
#endif

// convert access rights to string
SOCMN_EXPORT LPCTSTR accessRights2string(IN SOCmnString* buffer, IN DWORD accessRight, OPTIONAL IN LCID locID  = LOCALE_SYSTEM_DEFAULT);

// convert quality to string
SOCMN_EXPORT LPCTSTR quality2string(IN SOCmnString* buffer, IN WORD quality, OPTIONAL IN LCID locID  = LOCALE_SYSTEM_DEFAULT);

#ifdef SOOS_WINDOWS
// convert filetime to string
SOCMN_EXPORT LPCTSTR filetime2string(IN SOCmnString* buffer, IN LPFILETIME filetime);
#endif

#ifdef SOOS_WINDOWS
// convert interface id into interface description string
SOCMN_EXPORT LPCTSTR IID2string(IN SOCmnString* buffer, IN REFIID riid);
#endif

SOCMN_EXPORT LPCTSTR byteArrayToString(IN SOCmnString& buffer,  DWORD len, BYTE* data);


//-----------------------------------------------------------------------------
// Tracing Macros                                                             |
//-----------------------------------------------------------------------------


#define BOOL2STR(a) ((a) ? _T("TRUE") : _T("FALSE"))
#define VARIANT2STR(a) (variantWithType2string(&::getSOCmnTrace()->m_variantBuffer, a))
#define CLIPFORMAT2STR(a) (clipformat2string(&::getSOCmnTrace()->m_clipformatBuffer, a))
#define QUALITY2STR(a) (quality2string(&::getSOCmnTrace()->m_qualityBuffer, a))
#define VARTYPE2STR(a) (vartype2string(&::getSOCmnTrace()->m_vartypeBuffer, a))
#define FILETIME2STR(a) (filetime2string(&::getSOCmnTrace()->m_filetimeBuffer, a))
#define BYTEARRAY2STR(a,b) (byteArrayToString(::getSOCmnTrace()->m_clipformatBuffer, a, b))


#ifdef SOFEATURE_CMN_TRACE

#define _TRACE_CREATE_GENOBJ()                          \
{                                                       \
    SOCmnTrace *trace = ::getSOCmnTrace();                  \
    if (trace->m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG) \
    {                                                   \
        trace->m_genObj.lock();                         \
        trace->m_genObj.AddTail(this);      \
        trace->m_genObj.unlock();                           \
    }                                                   \
}

#define _TRACE_CREATE_COMOBJ()                          \
{                                                       \
    SOCmnTrace *trace = ::getSOCmnTrace();                  \
    if (trace->m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG)     \
    {                                                   \
        trace->m_comObj.lock();                         \
        trace->m_comObj.add((void *)this, this->getObjTraceId());       \
        trace->m_comObj.unlock();                           \
    }                                                   \
}

#define _TRACEC_CREATE_COMOBJ(id)                           \
{                                                       \
    SOCmnTrace *trace = ::getSOCmnTrace();                  \
    if (trace->m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG)     \
    {                                                   \
        trace->m_comObj.lock();                         \
        trace->m_comObj.add((void *)this, (LPCTSTR)id);     \
        trace->m_comObj.unlock();                           \
    }                                                   \
}

#define _TRACE_DELETE_GENOBJ()                          \
{                                                       \
    SOCmnTrace *trace = ::getSOCmnTrace();                  \
    if (trace->m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG) \
    {                                                   \
        trace->m_genObj.lock();                         \
        trace->m_genObj.removeObject((SOCmnObject *)this);          \
        trace->m_genObj.unlock();                           \
    }                                                   \
}

#define _TRACE_DELETE_COMOBJ()                          \
{                                                       \
    SOCmnTrace *trace = ::getSOCmnTrace();                  \
    if (trace->m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG)     \
    {                                                   \
        trace->m_comObj.lock();                         \
        trace->m_comObj.removeObject((void *)this);         \
        trace->m_comObj.unlock();                           \
    }                                                   \
}

#define _TRACE_IF(level, mask) if (::getSOCmnTrace()->m_traceLevel[level] & mask)

// trace with object id
#define _TRACE(level, mask, y)                          \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
    { \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

#define _TRACEASCIISTRING(level, mask, len, buffer)                         \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
    { \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->traceASCIIString(level, mask, trcID, buffer, len); \
    } \
}

// trace with object id and conversion
#define _TRACEW(level, mask, y)                         \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
    {   \
        USES_CONVERSION;                                \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    }   \
}

// trace in interface with object id
#define _TRACEI(level, mask, y)                         \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
    { \
        LPCTSTR trcID = this->getIntTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace with external object id
#define _TRACEO(level, mask, obj, y)                            \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
    { \
        LPCTSTR trcID = obj->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y);\
    } \
}

// trace without object id
#define _TRACEC(level, mask, id, y)                     \
{                                                       \
    if (::getSOCmnTrace()->m_traceLevel[level] & mask)  \
        ::getSOCmnTrace()->trace(level, mask, id, ::getSOCmnTrace()->expand y); \
}

// trace if failed res with object id
#define _TRACE_FAILED(res, level, mask, y)                          \
{                                                       \
    if ((FAILED(res)) && (::getSOCmnTrace()->m_traceLevel[level] & mask))   \
    { \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace if succeeded res with object id
#define _TRACE_SUCCEEDED(res, level, mask, y)                           \
{                                                       \
    if ((SUCCEEDED(res)) && (::getSOCmnTrace()->m_traceLevel[level] & mask))    \
    { \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace if succeeded res with object id
#define _TRACEO_SUCCEEDED(res, level, mask, obj, y)                         \
{                                                       \
    if ((SUCCEEDED(res)) && (::getSOCmnTrace()->m_traceLevel[level] & mask))    \
    { \
        LPCTSTR trcID = obj->getObjTraceId(); \
        ::getSOCmnTrace()->trace(level, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace with object id
#define _TRACEO_LEVEL_FROM_RES(res, mask, obj, y)                           \
{                                                       \
    WORD trcLevel = (res != S_OK) ? TL_ERR : TL_INF; \
    if (::getSOCmnTrace()->m_traceLevel[trcLevel] & mask)   \
    { \
        LPCTSTR trcID = obj->getObjTraceId(); \
        ::getSOCmnTrace()->trace(trcLevel, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace with object id
#define _TRACEO_LEVEL_FROM_RESX(res, level, mask, obj, y)                           \
{                                                       \
    WORD trcLevel = (res != S_OK) ? TL_ERR : level; \
    if (::getSOCmnTrace()->m_traceLevel[trcLevel] & mask)   \
    { \
        LPCTSTR trcID = obj->getObjTraceId(); \
        ::getSOCmnTrace()->trace(trcLevel, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

// trace with object id
#define _TRACE_LEVEL_FROM_RES(res, mask, y)                         \
{                                                       \
    WORD trcLevel = (res != S_OK) ? TL_ERR : TL_INF; \
    if (::getSOCmnTrace()->m_traceLevel[trcLevel] & mask)   \
    { \
        LPCTSTR trcID = this->getObjTraceId(); \
        ::getSOCmnTrace()->trace(trcLevel, mask, trcID, ::getSOCmnTrace()->expand y); \
    } \
}

#ifdef SOOS_WINDOWS

#ifdef TRACE_USE_PERFORMANCE_COUNTER
#define _TRACE_START_TIME(level, mask) ::QueryPerformanceCounter((LARGE_INTEGER *)&pcStart);
#define _TRACE_STOP_TIME(level, mask) ::QueryPerformanceCounter((LARGE_INTEGER *)&pcEnd);
#else
#define _TRACE_START_TIME(level, mask) pcStart = ::GetTickCount();;
#define _TRACE_STOP_TIME(level, mask) pcEnd = ::GetTickCount();;
#endif // TRACE_USE_PERFORMANCE_COUNTER

#define _TRACE_ELAPSED_TIME (::getSOCmnTrace()->elapsedTime(szElapsedTime, pcStart, pcEnd))
#endif // SOOS_WINDOWS

#ifdef SOOS_LINUX
#define _TRACE_START_TIME(level, mask) {SOCmnDateTime _start; _start.now(); pcStart = _start.getMicroseconds();}
#define _TRACE_STOP_TIME(level, mask) {SOCmnDateTime _stop; _stop.now(); pcEnd = _stop.getMicroseconds();}
#define _TRACE_ELAPSED_TIME (::getSOCmnTrace()->elapsedTime(szElapsedTime, pcStart, pcEnd))
#endif

#else // SOFEATURE_CMN_TRACE

#define _TRACE_CREATE_GENOBJ()
#define _TRACE_CREATE_COMOBJ()
#define _TRACEC_CREATE_COMOBJ(id)
#define _TRACE_DELETE_GENOBJ()
#define _TRACE_DELETE_COMOBJ()
#define _TRACE(level, mask, y) {}
#define _TRACEASCIISTRING(level, mask, len, buffer) {}
#define _TRACEW(level, mask, y) {}
#define _TRACEI(level, mask, y) {}
#define _TRACEO(level, mask, obj, y) {}
#define _TRACEC(level, mask, id, y) {}
#define _TRACE_START_TIME(level, mask)
#define _TRACE_STOP_TIME(level, mask)
#define _TRACE_ELAPSED_TIME _T("?");
#define _TRACE_FAILED(res, level, mask, y)
#define _TRACE_SUCCEEDED(res, level, mask, y)
#define _TRACEO_SUCCEEDED(res, level, mask, obj, y)
#define _TRACE_LEVEL_FROM_RES(res, mask, y)
#define _TRACEO_LEVEL_FROM_RES(res, mask, obj, y)
#define _TRACEO_LEVEL_FROM_RESX(res, mask, obj, y)
#define _TRACE_IF(level, mask) if (FALSE)
#endif // SOFEATURE_CMN_TRACE


#if defined __BORLANDC__ || defined SOOS_LINUX
#define _ASSERTION(expr, message) assert(expr)
#else

#ifndef SOOS_WINDOWS_CE
#ifdef  _DEBUG
#define _ASSERTION(expr, message) \
        do { if (!(expr) && \
                (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", message))) \
             _CrtDbgBreak(); } while (0)
#else // _DEBUG
#define _ASSERTION(expr, message)
#endif
#else // SOOS_WINDOWS_CE
#define _ASSERTION(expr, message)
#endif

#endif // defined __BORLANDC__ || SOOS_LINUX

//-----------------------------------------------------------------------------
// Memory validation helpers                                                  |
//-----------------------------------------------------------------------------

#ifdef SOOS_WINDOWS
inline BOOL isValidAddress(IN const void* lp, IN UINT nBytes, IN BOOL bReadWrite = FALSE)
{
	return (lp != NULL && !::IsBadReadPtr(lp, nBytes) &&
			(!bReadWrite || !::IsBadWritePtr((LPVOID)lp, nBytes)));
}

#define _IS_VALID_POINTER(p)        ( isValidAddress(p, sizeof(*p)) )
#define _IS_VALID_WRITE_POINTER(p)  ( isValidAddress(p, sizeof(*p), TRUE) )

#define _ASSERT_PTR(p)              _ASSERTION(_IS_VALID_POINTER(p), "Invalid pointer");
#define _ASSERT_WRITE_PTR(p)        _ASSERTION(_IS_VALID_WRITE_POINTER(p), "Invalid write Pointer");

#define _ASSERT_HANDLE(h)     _ASSERTION( h != INVALID_HANDLE_VALUE && h != NULL, "Invalid handle");
#else
#define _IS_VALID_POINTER(p)
#define _IS_VALID_WRITE_POINTER(p)
#define _ASSERT_PTR(p)
#define _ASSERT_WRITE_PTR(p)
#endif // SOOS_WINDOWS
#define _COUNTOF(array) (sizeof(array)/sizeof(array[0]))


#ifdef SOOS_WINDOWS
#define _TRACE_DEF_TIME(a) LONGLONG a;
#define _TRACE_GET_TIME(a) QueryPerformanceCounter((LARGE_INTEGER *)&a);
#else // SOOS_WINDOWS
#define _TRACE_DEF_TIME(a)
#define _TRACE_GET_TIME(a)
#endif // SOOS_WINDOWS

#pragma pack(pop)
#endif
