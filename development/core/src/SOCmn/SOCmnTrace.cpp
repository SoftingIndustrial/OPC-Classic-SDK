#include "stdafx.h"
#ifdef SOFEATURE_CMN_OPC
#include <opcda.h>
#endif
#include "SOCmn.h"
#include "SOCmnTrace.h"
#include "SOCmnObject.h"
#include "SOCmnFunctions.h"
#ifdef SOFEATURE_CMN_TRACE
#include "SOVersion.h"
#endif
#ifdef SOFEATURE_CMN_OPC
#endif
#ifdef SOFEATURE_CMN_TRACE_GETTID
#include <syscall.h>
#endif
#include <limits.h>
#ifdef SOOS_LINUX
#include <sys/stat.h>
#include <algorithm>
#define MAX_PATH 260
#endif


#define MAX_BACKUP_COPYS 50     //hard max limit of trace file copys
SOCmnTrace* g_trace = NULL;     // trace object

// Custom unsafe versions of _snprintf_s and strncat_s for versions of VS2003 and earlier
#if defined(_MSC_VER) && _MSC_VER < 1400

#define _snprintf_s custom_snprintf_s

// Quick and Unsafe in this case since reqLen is ignored
__inline int custom_snprintf_s(char *outBuf, size_t reqLen, const char *format, size_t size)
{
	int count;
	va_list valist;
	va_start(valist, format);
#ifdef UNICODE
	count = _vsntprintf((wchar_t*) outBuf, size, (wchar_t*)format, valist);
#else 
	count = _vsntprintf(outBuf, size, format, valist);
#endif
	va_end(valist);

	return count;
}

#define strncat_s custom_strncat

// Quick and Unsafe in this case since reqLen is ignored
__inline int custom_strncat(char *outBuf, size_t reqLen, const char *src, size_t size)
{
	char* ret = strncat(outBuf, src, size);
	if (ret == 0)
	{
		return (int)strlen(outBuf);
	}
	return (int)ret;
}

typedef size_t rsize_t;

#endif

//-----------------------------------------------------------------------------
// SOCmnTraceReceiver                                                         |
//-----------------------------------------------------------------------------

SOCmnTraceReceiver::SOCmnTraceReceiver()
	: SOCmnObject(SOCMNOBJECT_TYPE_CALLBACK)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(),
			  (DWORD)this, _T("TRACE"));
	setObjTraceId(id);
}


//-----------------------------------------------------------------------------
// onTrace
//
// - callback called on every trace call to do external things
//
void SOCmnTraceReceiver::onTrace(
	IN LPCTSTR /* traceString */,   // whole trace string (Toolkit format)
	IN USHORT /* traceStringLen */,
	IN USHORT /* level */,      // trace level
	IN DWORD /* mask */,            // trace group
	IN LPCTSTR /* objId */,     // object id string
	IN LPCTSTR /* text */)      // trace output
{} // onTrace



//-----------------------------------------------------------------------------
// SOCmnTraceHandler                                                        |
//-----------------------------------------------------------------------------

SOCmnTraceHandler::SOCmnTraceHandler()
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(),
			  (DWORD)this, _T("TRACE"));
	setObjTraceId(id);
}

//-----------------------------------------------------------------------------
// getUserMaskString
//
// - fill mask string for user groups
//
void SOCmnTraceHandler::getUserMaskString(
	IN DWORD mask,      // trace group
	OUT LPTSTR string)  // mask string
{
	switch (mask)
	{
	case TG_USER1:
		_tcscpy(string, _T("U1"));
		break;

	case TG_USER2:
		_tcscpy(string, _T("U2"));
		break;

	case TG_USER3:
		_tcscpy(string, _T("U3"));
		break;

	case TG_USER4:
		_tcscpy(string, _T("U4"));
		break;
	}
} // getUserMaskString


//-----------------------------------------------------------------------------
// saveTraceOptions
//
// - save the trace options
//
HRESULT SOCmnTraceHandler::saveTraceOptions(void)
{
	return E_NOTIMPL;
} // saveTraceOptions



//-----------------------------------------------------------------------------
// SOCmnTrace                                                                   |
//-----------------------------------------------------------------------------

SOCmnTrace* getSOCmnTrace(void)
{
	_ASSERTION((g_trace != NULL), "Before calling getSOCmnTrace the trace object must be created by createTraceObject!");
	return g_trace;
}

SOCmnTrace* getOPCTrace(void)
{
	_ASSERTION((g_trace != NULL), "Before calling getOPCTrace the trace object must be created by createTraceObject!");
	return g_trace;
}

void createTraceObject(void)
{
	if (!g_trace)
		SOCMN_ALLOCATE_OBJECT(g_trace, SOCmnTrace())
	}

void destroyTraceObject(void)
{
	if (g_trace)
	{
		SOCMN_FREE_OBJECT(g_trace)
	}
}

SOCmnTrace::SOCmnTrace()
{
#ifdef SOFEATURE_CMN_TRACE
#ifdef SOOS_WINDOWS
	m_handleTrace = INVALID_HANDLE_VALUE;
	m_handleTrace2nd = INVALID_HANDLE_VALUE;
	m_actHandle = INVALID_HANDLE_VALUE;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_handleTrace = -1;
	m_handleTrace2nd = -1;
	m_actHandle = -1;
#endif
	m_lines = 0;
#endif
	m_fileSize = ULONG_MAX;
	m_maxBackups = 5; //buildin default is 5...
#ifdef SOFEATURE_CMN_TRACE
	m_noOfTraceFiles = 0;
	m_apiVersion = _T("");
	m_dcomInitStatus = _T("");
	m_otbVersion = _T("");
	m_licenseInfo = _T("No licenses activated (Demo Mode).");
#endif
	m_fileTraceOn = TRUE;
	m_traceLevel[SOCMNTRACE_L_ERR] = SOCMNTRACE_G_ALL;
	m_traceLevel[SOCMNTRACE_L_WRN] = SOCMNTRACE_G_ALL;
#ifdef _DEBUG
	m_traceLevel[SOCMNTRACE_L_INF] = SOCMNTRACE_G_OPC | 0x00200000;
	m_traceLevel[SOCMNTRACE_L_DEB] = 0;
#else
	m_traceLevel[SOCMNTRACE_L_INF] = 0x00200000;
	m_traceLevel[SOCMNTRACE_L_DEB] = 0;
#endif
	m_levelChar[SOCMNTRACE_L_ERR] = 'E';
	m_levelChar[SOCMNTRACE_L_WRN] = 'W';
	m_levelChar[SOCMNTRACE_L_INF] = 'I';
	m_levelChar[SOCMNTRACE_L_DEB] = ' ';
	m_trcHandler = NULL;
#ifdef SOFEATURE_CMN_TRACE
#ifdef SOOS_WINDOWS
	QueryPerformanceFrequency(&m_pcFrequency);
	m_pcPer1ms = m_pcFrequency.QuadPart / 1000.0;
#endif
#endif
	m_genObj.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_SYNCED);
	m_comObj.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_SYNCED);
	m_recList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
}

SOCmnTrace::~SOCmnTrace()
{
#ifdef SOFEATURE_CMN_TRACE
	// print object trace
	SOCmnListPosition pos;

	if (m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG)
	{
		SOCmnString id;
		SOCMN_TRY
		{
			pos = m_comObj.getStartPosition();

			while (pos)
			{
				m_comObj.getNext(pos, &id);
				trace(TL_DEB, SOCMNTRACE_G_OBJ_LOG, id, _T(""));
			}

			m_comObj.removeAll();
		}
		SOCMN_CATCH(...)
		{
			trace(TL_DEB, SOCMNTRACE_G_OBJ_LOG, _T(""), _T("EXCEPTION in COM object log"));
		}
	}

	if (m_traceLevel[TL_DEB] & SOCMNTRACE_G_OBJ_LOG)
	{
		SOCmnObject* genObj;
		SOCMN_TRY
		{
			pos = m_genObj.getStartPosition();

			while (pos)
			{
				genObj = (SOCmnObject*)m_genObj.getNext(pos);

				if (genObj != m_trcHandler)
				{
					trace(TL_DEB, SOCMNTRACE_G_OBJ_LOG, genObj->getObjTraceId(),
					expand(_T("refCount %lu"), genObj->getRefCount()));
				}
			}

			m_genObj.removeAll();
		}
		SOCMN_CATCH(...)
		{
			trace(TL_DEB, SOCMNTRACE_G_OBJ_LOG, _T(""), _T("EXCEPTION in SO object log"));
		}
	}

#endif
	m_genObj.destroy();
	m_comObj.destroy();
	m_traceLevel[SOCMNTRACE_L_ERR] = 0;
	m_traceLevel[SOCMNTRACE_L_WRN] = 0;
	m_traceLevel[SOCMNTRACE_L_INF] = 0;
	m_traceLevel[SOCMNTRACE_L_DEB] = 0;
	setTraceHandler(NULL);
	m_recList.destroy();
	closeTraceFiles();
}

#ifdef SOFEATURE_CMN_TRACE

//-----------------------------------------------------------------------------
// expand
//
// - expand the string specified with format and ... arguments
//
// returns:
//		pointer to expanded string (m_expand)
//
LPCTSTR SOCmnTrace::expand(
	IN LPCTSTR format,  // format string
	IN ...)             // arguments
{
	SOCMN_TRY
	{
		m_lock.lock();      // lock must be freed outside (normaly its done in trace())
		va_list arglist;
		va_start(arglist, format);
		_vsntprintf(m_expand, 400, format, arglist);
	}
	SOCMN_CATCH(...)
	{
		_tcscpy(m_expand, _T("EXCEPTION in SOCmnTrace::expand"));
	}
	return m_expand;
} // expand


void SOCmnTrace::traceASCIIString(
	IN USHORT level,    // trace level
	IN DWORD mask,      // trace group
	IN LPCTSTR objId,   // object identification string
	IN CHAR* text,  // text to trace
	IN DWORD textLen)
{
#ifdef SOFEATURE_CMN_STRINGEX
	SOCmnStringEx szPart;
	szPart.getBuffer(410);
#else
	SOCmnString szPart;
#endif
	DWORD i;
	DWORD remText;
	DWORD lenText;
	CHAR requiresEscape = 0;
	DWORD realTextLen = textLen;
	CHAR* realText = text;
	m_lock.lock(); // keep this block together...

	for (i = 0; i < textLen; ++i)
	{
		if (text[i] < 32)
		{
			requiresEscape = 1;
			break;
		}
	}

	if (requiresEscape)
	{
		CHAR buff[10] = { 0 };
		size_t reqLen = (textLen + 1) * 2 + 1;
		realText = (CHAR*)malloc(reqLen * sizeof(CHAR));
		memset(realText, 0, (reqLen * sizeof(CHAR)));

		for (i = 0; i < textLen; ++i)
		{
			_snprintf_s(buff, 10, "%02X", (unsigned char)text[i]);
			strncat_s(realText, (rsize_t)reqLen, buff, (rsize_t)2);
		}

		realTextLen = 2 * textLen;
	}

	for (i = 0; i < realTextLen; i += 398)
	{
		m_lock.lock();      // lock must be freed outside (normaly its done in trace())
		remText = realTextLen - i;
		lenText = (remText > 398) ? 398 : remText;
		szPart.setCHAR(realText + i, lenText);

		if (remText <= 398)
		{
#ifdef SOOS_WINDOWS
			szPart += _T("\r\n");
			lenText++;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			szPart += _T('\n');
			lenText++;
#endif
		}

		trace(level, mask, objId, szPart, FALSE);
	}

	if (requiresEscape)
	{
		free(realText);
	}

	m_lock.unlock(); // ...keep this block together
}


//-----------------------------------------------------------------------------
// trace
//
// - trace text to trace window and file
//
void SOCmnTrace::trace(
	IN USHORT level,    // trace level
	IN DWORD mask,      // trace group
	IN LPCTSTR objId,   // object identification string
	IN LPCTSTR text,    // text to trace
	IN OPTIONAL BOOL traceHeader)
{
	SOCMN_TRY
	{
		if (traceHeader)
		{
#ifdef SOOS_WINDOWS
			SYSTEMTIME now;
			::GetLocalTime(&now);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			struct tm locT;
			struct timeval now;
			gettimeofday(&now, NULL);
			localtime_r(&now.tv_sec, &locT);
#endif
#ifdef SOOS_WINDOWS
#ifdef SOFEATURE_TUNNEL
#ifndef OCT_OEM_LENZEDIGITEC
			_stprintf(m_buffer, _T("%c %s %4.4d %2.2d-%2.2d:%2.2d:%2.2d.%3.3d %-60.60s %-1.400s\r\n"),
			m_levelChar[level], getMaskString(mask), GetCurrentThreadId() % 10000,
			now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
			objId, text);
#else
			_stprintf(m_buffer, _T("%c %s %4.4d %2.2d-%2.2d:%2.2d:%2.2d.%3.3d %-80.80s %-1.400s\r\n"),
			m_levelChar[level], getMaskString(mask), GetCurrentThreadId() % 10000,
			now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
			objId, text);
#endif // OCT_OEM_LENZEDIGITEC
#else
			_stprintf(m_buffer, _T("%c %s %4.4d %2.2d-%2.2d:%2.2d:%2.2d.%3.3d %-80.80s %-1.400s\r\n"),
			m_levelChar[level], getMaskString(mask), GetCurrentThreadId() % 10000,
			now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
			objId, text);
#endif
#endif
#ifdef SOOS_LINUX
			_stprintf(m_buffer, _T("%c %s %4.4d %2.2d-%2.2d:%2.2d:%2.2d.%3.3d %-35.35s %-1.400s\n"),
			m_levelChar[level], getMaskString(mask),
#ifdef SOFEATURE_CMN_TRACE_GETTID
			(int)syscall(SYS_gettid) % 10000,
#else
#ifdef SOFEATURE_CMN_TRACE_GETPID
			(int)getpid() % 10000,
#else
			(int)pthread_self() % 10000,
#endif
#endif
			locT.tm_mday, locT.tm_hour, locT.tm_min,
			locT.tm_sec, (int)(now.tv_usec / 1000),
			objId, text);
#endif
		}
		else
		{
			_stprintf(m_buffer, _T("%-1.400s"), text);
		}

		m_len = (USHORT)_tcslen(m_buffer);

#ifdef SOOS_WINDOWS
		OutputDebugString(m_buffer);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		write(STDERR_FILENO, m_buffer, m_len);
#endif

		if (m_trcHandler)
		{
			m_trcHandler->onTrace(m_buffer, m_len, level, mask, objId, text);
		}

		SOCmnListPosition recPos;
		SOCmnTraceReceiver* pRec;
		recPos = m_recList.getStartPosition();

		while (recPos)
		{
			pRec = m_recList.getNext(recPos);
			pRec->onTrace(m_buffer, m_len, level, mask, objId, text);
		}

#ifdef SOOS_WINDOWS

		if (m_actHandle != INVALID_HANDLE_VALUE)
		{
			USES_CONVERSION;
			DWORD len;
			// unicode to ansi
			LPSTR ansiBuf = T2A(m_buffer);
			::WriteFile(m_actHandle, ansiBuf, m_len, &len, NULL);
			m_lines++;

			if ((m_lines & 0xff) == 0)
			{
				DWORD sizeLow;
				DWORD sizeHigh;
				sizeLow = GetFileSize(m_actHandle, &sizeHigh);

				if (sizeLow != 0xFFFFFFFF)
				{
					if (((m_fileSize != 0xFFFFFFFF) && (sizeLow > m_fileSize))
					|| (sizeHigh > 0x00000000))
					{
						if (m_handleTrace2nd != INVALID_HANDLE_VALUE)
						{
							// 2 trace files -> change files
							if (m_actHandle == m_handleTrace)
							{
								m_actHandle = m_handleTrace2nd;
							}
							else
							{
								m_actHandle = m_handleTrace;
							}

							SetFilePointer(m_actHandle, 0, NULL, FILE_BEGIN);
							SetEndOfFile(m_actHandle);
						}
						else
						{
							// 1 trace file -> overwrite the current trace
							SetFilePointer(m_actHandle, 0, NULL, FILE_BEGIN);
							SetEndOfFile(m_actHandle);
						}

#ifdef SOFEATURE_CMN_TRACE
						writeTraceHeader();
#endif
					}
				}
			}
		}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

		if (m_actHandle >= 0)
		{
			write(m_actHandle, m_buffer, m_len);
			m_lines++;

			if ((m_lines & 0xff) == 0)
			{
				off_t sizeLow = lseek(m_actHandle, 0, SEEK_CUR);

				if (sizeLow != (off_t) - 1)
				{
					if ((DWORD)sizeLow > m_fileSize)
					{
						if (m_handleTrace2nd >= 0)
						{
							// 2 trace files -> change files
							if (m_actHandle == m_handleTrace)
							{
								m_actHandle = m_handleTrace2nd;
							}
							else
							{
								m_actHandle = m_handleTrace;
							}
						}

						// overwrite the current trace
						lseek(m_actHandle, 0, SEEK_SET);
						ftruncate(m_actHandle, 0);
#ifdef SOFEATURE_CMN_TRACE
						writeTraceHeader();
#endif
					}
				}
			}
		}

#endif

	}
	SOCMN_CATCH(...) { }
	m_lock.unlock();
} // trace

#endif

//-----------------------------------------------------------------------------
// setLevels
//
// - set trace levels
//
void SOCmnTrace::setLevels(
	IN DWORD error,     // error level
	IN DWORD warning,   // warning level
	IN DWORD info,      // info level
	IN DWORD debug)     // debug level
{
	m_traceLevel[TL_ERR] = error;
	m_traceLevel[TL_WRN] = warning;
	m_traceLevel[TL_INF] = info;
	m_traceLevel[TL_DEB] = debug;
} // setLevels


BOOL SOCmnTrace::enableTraceToFile(BOOL enable)
{
	if (!enable)
	{
		//stop tracing to file, close the files:
		closeTraceFiles();
		m_fileTraceOn = FALSE;
		return TRUE;
	}

	//enable file tracing:
	if (!m_fileTraceOn)
	{
		m_fileTraceOn = TRUE;
		return setFile(m_fileName, m_maxBackups, m_fileName2, m_fileSize);
	}//else: is already active, do not call setFile again!

	return TRUE;
}


//local helper to get a pointer to ".<fileext>" or NULL if no extension
static LPCTSTR OSGetFileExtension(LPCTSTR pszFile)
{
	LPCTSTR pszExt = _tcsrchr(pszFile, '.');
#ifdef SOOS_WINDOWS

	if (pszExt && pszExt > _tcsrchr(pszFile, '/') && pszExt > _tcsrchr(pszFile, '\\'))
	{
		return pszExt;
	}

#endif
#ifdef SOOS_LINUX

	if (pszExt && pszExt > _tcsrchr(pszFile, '/'))
	{
		return pszExt;
	}

#endif
	return NULL;
}
static void OSRemoveFileExt(SOCmnString& sFile)
{
	LPCTSTR pszFile = sFile;
	LPCTSTR pszExt = OSGetFileExtension(pszFile);

	if (pszExt)
	{
		sFile = sFile.left((int)(pszExt - pszFile));
	}
}

#ifdef SOOS_WINDOWS
static LONGLONG OSGetFileLastWriteTime(IN LPCTSTR pszFile, OUT OPTIONAL LONGLONG* pByteSize)
//Returns 0 if any error, else the best (finest) file time of the last write access.
//Exact meaning of the int64 file time depends on OS.
//If pByteSize is not NULL it receives the file size in bytes.
{
	WIN32_FILE_ATTRIBUTE_DATA   fileAttr;
	LARGE_INTEGER               lint;

	if (GetFileAttributesEx(pszFile, GetFileExInfoStandard, &fileAttr))
	{
		lint.LowPart = fileAttr.ftLastWriteTime.dwLowDateTime;
		lint.HighPart = fileAttr.ftLastWriteTime.dwHighDateTime;

		if (pByteSize)
		{
			LARGE_INTEGER lint2;
			lint2.LowPart = fileAttr.nFileSizeLow;
			lint2.HighPart = fileAttr.nFileSizeHigh;
			*pByteSize = lint2.QuadPart;
		}
	}
	else
	{
		//any error:
		lint.QuadPart = 0;

		if (pByteSize)
		{
			*pByteSize = 0;
		}
	}

	return lint.QuadPart;
}
#endif

static BOOL OSDeleteFile(IN LPCTSTR pszFile)
{
#ifdef SOOS_WINDOWS
	return DeleteFile(pszFile);
#endif
#ifdef SOOS_LINUX
	int status = remove(pszFile);

	if (status)
	{
		return FALSE;
	}

	return TRUE;
#endif
}

static BOOL OSMoveFile(IN LPCTSTR pszSrc, IN LPCTSTR pszDest)
{
#ifdef SOOS_WINDOWS
	return MoveFile(pszSrc, pszDest);
#endif
#ifdef SOOS_LINUX
	SOCmnString command = _T("mv ");
	command += pszSrc;
	command += _T(" ");
	command += pszDest;

	if (system(command))
	{
		return FALSE;
	}

	return TRUE;
#endif
}

static SOCmnString GetFileNameBaseFromFirstFile(IN LPCTSTR pszFirstFile)
{
	SOCmnString fnamebase = pszFirstFile;
	//special consideration: we do not know if the new function setFile2 has been
	//used to create the trace file names. But if it was used we must remove the "_currentA.log"
	//to get the original base name:
	int nStart = fnamebase.getLength() - (DWORD)_tcslen(_T("_currentA.log"));

	if (nStart > 0 && !_tcsicmp(((LPCTSTR)fnamebase) + nStart, _T("_currentA.log")))
	{
		fnamebase = fnamebase.left(nStart);
	}

	return fnamebase;
}

//create backup of trace files by rename sequence:
//Trace_currentA.log (Trace_currentB.log)   <--- newst trace file pairs
//Trace_backup#1.log                        <--- newest backup (contains a concatenation of the pair files)
//Trace_backup#2.log
//Trace_backup#3.log
//Trace_backup#<maxBackups>.log             <--- oldest backup
//before copy to backup file concat the two trace files (newest at end) to have a single one.
static void MakeBackupCopys(IN int maxBackups, IN LPCTSTR pszFile1, IN LPCTSTR pszFile2)
{
	SOCmnString fname;
	SOCmnString fname2;
	SOCmnString formatA;
	SOCmnString formatB;
	formatA = GetFileNameBaseFromFirstFile(pszFile1);
	formatB = formatA;
	formatA += _T("_backup#%dA.log");
	formatB += _T("_backup#%dB.log");

	if (maxBackups <= 0)
	{
		return;    //nothing to do, leave old backups on disk if any...
	}

	fname.Format(formatA, maxBackups);
	OSDeleteFile(fname);
	fname.Format(formatB, maxBackups);
	OSDeleteFile(fname);

	//rename backups to higher numbers:
	for (int i = maxBackups - 1; i > 0; i--)
	{
		fname.Format(formatA, i);
		fname2.Format(formatA, i + 1);
		OSMoveFile(fname, fname2);//by design: ignore return code
		fname.Format(formatB, i);
		fname2.Format(formatB, i + 1);
		OSMoveFile(fname, fname2);//by design: ignore return code
	}

	//finally safe pszFile as backup#1
	fname.Format(formatA, 1);
	OSMoveFile(pszFile1, fname);//by design: ignore return code
	fname.Format(formatB, 1);
	OSMoveFile(pszFile2, fname);//by design: ignore return code
}

#ifdef SOFEATURE_CMN_TRACE
void SOCmnTrace::setApiVersion(IN LPCTSTR apiVersion)
{
	m_apiVersion = apiVersion;
}

void SOCmnTrace::addDcomInitStatus(IN LPCTSTR dcomInitStatus)
{
	if (m_dcomInitStatus == _T("DCOM initialization not called."))
	{
		m_dcomInitStatus = dcomInitStatus;
		return;
	}

	if (!m_dcomInitStatus.isEmpty())
	{
		m_dcomInitStatus += _T(" ");
	}

	m_dcomInitStatus += dcomInitStatus;
}

void SOCmnTrace::setOtbVersion(IN LPCTSTR otbVersion)
{
	m_otbVersion = otbVersion;
}

void SOCmnTrace::addLicenseInfo(IN LPCTSTR licenseInfo)
{
	if ((m_licenseInfo == _T("No licenses activated (Demo Mode).")) ||
		(m_licenseInfo.isEmpty()))
	{
		m_licenseInfo = _T("Activated licenses: ");
	}
	else
	{
		m_licenseInfo += _T(", ");
	}

	m_licenseInfo += licenseInfo;
}
#endif

BOOL SOCmnTrace::setFile2(IN LPCTSTR pszFileBase,
						  IN DWORD   maxBackups,
						  IN DWORD   maxFileSize
						 )
{
	SOCmnString file1;
	SOCmnString file2;

	if (pszFileBase)
	{
		file1 = pszFileBase;
		file1.trimLeft();
		file1.trimRight();

		if (!file1.isEmpty()) //pszFileBase may be NULL or empty
		{
			OSRemoveFileExt(file1);
			file2 = file1;
			file1 += _T("_currentA.log");
			file2 += _T("_currentB.log");
		}
	}

	return setFile(file1, maxBackups, file2, maxFileSize);
}

BOOL SOCmnTrace::updateTraceFileSettings2(IN const BOOL* pEnable,       //NULL: do not change
										  IN LPCTSTR      pszFileBase,    //NULL: do not change
										  IN const DWORD* pMaxSize,       //NULL: do not change
										  IN const DWORD* pMaxBackups     //NULL: do not change
										 )
{
	if (pszFileBase)
	{
		SOCmnString file1;
		SOCmnString file2;
		file1 = pszFileBase;
		file1.trimLeft();
		file1.trimRight();

		if (!file1.isEmpty()) //pszFileBase may be NULL or empty
		{
			OSRemoveFileExt(file1);
			file2 = file1;
			file1 += _T("_currentA.log");
			file2 += _T("_currentB.log");
		}

		return updateTraceFileSettings(pEnable, file1, file2, pMaxSize, pMaxBackups);
	}
	else
	{
		return updateTraceFileSettings(pEnable, NULL, NULL, pMaxSize, pMaxBackups);
	}
}

#ifdef SOFEATURE_CMN_TRACE
void SOCmnTrace::writeTraceHeader(void)
{
	m_lock.lock();      // keep this block together...

	if (m_licenseInfo.isEmpty())
	{
		m_licenseInfo = _T("No licenses activated (Demo Mode).");
	}

	if (m_dcomInitStatus.isEmpty())
	{
		m_dcomInitStatus = _T("DCOM initialization not called.");
	}

	++m_noOfTraceFiles;
	trace(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, _T("Header row 1/5"), expand(_T("Softing OPC Classic Toolkit V%u.%u, built %s. Process ID %u .Trace file no. %u"), MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, _T(__DATE__), GetCurrentProcessId(), m_noOfTraceFiles));
	m_lock.lock();      // lock must be freed outside (normaly its done in trace(), as it is usually first locked by expand())
	trace(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, _T("Header row 2/5"), m_apiVersion);
	m_lock.lock();      // lock must be freed outside (normaly its done in trace(), as it is usually first locked by expand())
	trace(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, _T("Header row 3/5"), m_otbVersion);
	m_lock.lock();      // lock must be freed outside (normaly its done in trace(), as it is usually first locked by expand())
	trace(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, _T("Header row 4/5"), m_licenseInfo);
	m_lock.lock();      // lock must be freed outside (normaly its done in trace(), as it is usually first locked by expand())
	trace(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, _T("Header row 5/5"), m_dcomInitStatus);
	m_lock.unlock();      // ...keep this block together
}
#endif

SOCmnString SOCmnTrace::getFileNameBase()
{
	return GetFileNameBaseFromFirstFile(m_fileName);
}

#ifdef SOFEATURE_CMN_TRACE
LPCTSTR SOCmnTrace::getActiveTraceFileName()
{
	if (m_actHandle == m_handleTrace)
	{
		return m_fileName;
	}
	else
	{
		return m_fileName2;
	}
}
#endif


//-----------------------------------------------------------------------------
// setFile
//
// - set new trace file
// - opens the trace file
//
// returns:
//		TRUE -  new trace file opened
//		FALSE - error open trace file
//
BOOL SOCmnTrace::setFile(
	IN LPCTSTR rawFile,                                // file name
	IN OPTIONAL DWORD maxBackups,                   // number of backup copys
	IN OPTIONAL LPCTSTR secondRawFile,                 // second file name
	IN OPTIONAL DWORD maxFileSize                   // max file size
)
{
	BOOL ret = FALSE;
	int result;
	char exePath [MAX_PATH];
	SOCmnString fullNewFileName;
	SOCmnString newFileName;
	SOCmnString newSecondFileName;
	SOCmnString rawFilePath = rawFile;
	SOCmnString rawFileString = rawFile;
	SOCmnString secondRawFileString = secondRawFile;
	LPCTSTR file = rawFile;
	LPCTSTR secondFile = secondRawFile;
	TCHAR delimiter;
#ifdef SOOS_WINDOWS
	delimiter = '\\';
#endif
#ifdef SOOS_LINUX
	delimiter = '/';
#endif

	if (!rawFile)
	{
		rawFile = _T("");
	}

	if (!secondRawFile)
	{
		secondRawFile = _T("");
	}

	if (maxBackups > MAX_BACKUP_COPYS) //limit to reasonable value
	{
		maxBackups = MAX_BACKUP_COPYS;
	}

	m_maxBackups = maxBackups;

	if (!m_fileTraceOn)
	{
		m_fileName = rawFile;
		m_fileName2 = secondRawFile;
		m_fileSize = maxFileSize;
		return TRUE;
	}

#ifdef SOFEATURE_CMN_TRACE
	//check if there are changes made that
	closeTraceFiles();

	if (_tcslen(rawFile) == 0)
	{
		return TRUE;
	}

//--------------------------------------------------------
// Validate the user given path to the trace files.
// If not successful, use the application execution path
// and append the given filename
//--------------------------------------------------------
	//cut of filename of raw path and validate path
	int found = rawFileString.reverseFind(delimiter);

	if (found != (int) - 1)
	{
		rawFilePath = rawFileString.left(found);
	}

#ifdef SOOS_WINDOWS
	DWORD fileAtt = GetFileAttributes(rawFilePath);

	if (fileAtt == INVALID_FILE_ATTRIBUTES)
	{
#endif
#ifdef SOOS_LINUX
	struct stat stFileInfo;
	int intStat = stat(rawFilePath, &stFileInfo);

	if (intStat != 0)
	{
#endif
		//not valid, get executed file
#ifdef SOOS_WINDOWS
		result = GetModuleFileNameA(NULL, exePath, MAX_PATH);
#endif
#ifdef SOOS_LINUX
		char szTmp[32];
		sprintf(szTmp, "/proc/%d/exe", getpid());
		result = std::min(readlink(szTmp, exePath, MAX_PATH), MAX_PATH - 1);
		exePath[result] = '\0';
#endif

		if (result != 0)
		{
			int j = 0;

			while (exePath[j] != ('\0'))
			{
				newFileName = newFileName + (TCHAR) exePath[j];
				j++;
			}

			//cut of filename of exe path
			found = newFileName.reverseFind(delimiter);

			if (found != (size_t) - 1)
			{
				newFileName[found] = (TCHAR) '\0';
			}

			// validate again with user-given path appended
			fullNewFileName = newFileName + rawFilePath;
#ifdef SOOS_WINDOWS
			fileAtt = GetFileAttributes(fullNewFileName);

			if (fileAtt == INVALID_FILE_ATTRIBUTES)
#endif
#ifdef SOOS_LINUX
				intStat = stat(fullNewFileName, &stFileInfo);

			if (intStat != 0)
#endif
			{
				found = rawFileString.reverseFind(delimiter);
				int secondFound = secondRawFileString.reverseFind(delimiter);

				//not valid, append only filename
				if (found != (int) - 1)
				{
					rawFileString = rawFileString.right(rawFileString.getLength() - found - 1);
				}

				if (secondFound != (int) - 1)
				{
					secondRawFileString = secondRawFileString.right(secondRawFileString.getLength() - secondFound - 1);
				}
			}

			newSecondFileName = newFileName + (SOCmnString) delimiter + secondRawFileString;
			newFileName = newFileName + (SOCmnString) delimiter + rawFileString;
			file = (LPCTSTR) newFileName;
			secondFile = (LPCTSTR) newSecondFileName;
		}
	}

//------------------------
// End of path validation
//------------------------
	SOCmnSingleLock<SOCmnSync> lock(&m_lock);
	MakeBackupCopys(m_maxBackups, file, secondFile);
#ifdef SOOS_WINDOWS
	m_handleTrace = ::CreateFile(file, GENERIC_WRITE, FILE_SHARE_READ, NULL,
								 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_handleTrace = open(file, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0666);
#endif
	m_fileSize = maxFileSize;

	if (_tcslen(secondFile) > 0)
	{
#ifdef SOOS_WINDOWS
		m_handleTrace2nd = ::CreateFile(secondFile, GENERIC_WRITE , FILE_SHARE_READ, NULL,
										CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		m_handleTrace2nd = open(secondFile, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0666);
#endif
#ifdef SOOS_WINDOWS
		ret = ((m_handleTrace != INVALID_HANDLE_VALUE) &&
			   (m_handleTrace2nd != INVALID_HANDLE_VALUE));
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		ret = ((m_handleTrace >= 0) &&
			   (m_handleTrace2nd >= 0));
#endif
	}
	else
	{
#ifdef SOOS_WINDOWS
		m_handleTrace2nd = INVALID_HANDLE_VALUE;
		ret = (m_handleTrace != INVALID_HANDLE_VALUE);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		m_handleTrace2nd = -1;
		ret = (m_handleTrace >= 0);
#endif
	}

	if (ret)
	{
		m_actHandle = m_handleTrace;
#ifdef SOOS_WINDOWS

		if (m_handleTrace != INVALID_HANDLE_VALUE)
		{
			m_fileName = file;
		}

		if (m_handleTrace2nd != INVALID_HANDLE_VALUE)
		{
			m_fileName2 = secondFile;
		}
		else
		{
			m_fileName2.empty();
		}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

		if (m_handleTrace >= 0)
		{
			m_fileName = file;
		}

		if (m_handleTrace2nd >= 0)
		{
			m_fileName2 = secondFile;
		}
		else
		{
			m_fileName2.empty();
		}

#endif
#ifdef SOFEATURE_CMN_TRACE
		writeTraceHeader();
#endif
	}
	else
	{
		closeTraceFiles();
	}

#endif // SOFEATURE_CMN_TRACE
	return ret;
} // setFile

//-----------------------------------------------------------------------------
// updateTraceFileSettings
//- call this method to change any setting for trace files at runtime,
//  depending on the changes eventually the trace files will be closed
//  and/or re-opened.
BOOL SOCmnTrace::updateTraceFileSettings(
	IN const BOOL *  pEnable,   //NULL: do not change
	IN LPCTSTR      pszFile1,   //NULL: do not change
	IN LPCTSTR      pszFile2,   //NULL: do not change
	IN const DWORD * pMaxSize,  //NULL: do not change
	IN const DWORD * pMaxBackups //NULL: do not change
)
{
	BOOL    bEnable     = pEnable  ? *pEnable != 0 : m_fileTraceOn != 0;
	LPCTSTR file1       = pszFile1 ? pszFile1 : (LPCTSTR)m_fileName;
	LPCTSTR file2       = (pszFile1 && pszFile2) ? pszFile2 : (LPCTSTR)m_fileName2;
	DWORD   maxSize     = pMaxSize ? *pMaxSize : m_fileSize;

	//if pMaxBackups is given, simply change the member m_maxBackups
	//never do a close/open files:
	if (pMaxBackups)
	{
		m_maxBackups = *pMaxBackups;

		if (m_maxBackups > MAX_BACKUP_COPYS)
		{
			m_maxBackups = MAX_BACKUP_COPYS;
		}
	}

	//enable state changed ?
	if (bEnable != m_fileTraceOn)
	{
		m_fileName  = file1;
		m_fileName2 = file2;
		m_fileSize = maxSize;
		return enableTraceToFile(bEnable);
	}

	//file names or max file size changed => reopen
	if (maxSize != m_fileSize ||
		_tcscmp(file1, m_fileName) != 0 ||
		_tcscmp(file2, m_fileName2) != 0
	   )
	{
		return setFile(file1, m_maxBackups, file2, maxSize);
	}

	return TRUE;
} // updateTraceFileSettings
//-----------------------------------------------------------------------------
// closeTraceFiles
//
// - close the trace files
//
void SOCmnTrace::closeTraceFiles(void)
{
#ifdef SOFEATURE_CMN_TRACE
	SOCmnSingleLock<SOCmnSync> lock(&m_lock);
#ifdef SOOS_WINDOWS

	if (m_handleTrace != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handleTrace);
		m_handleTrace = INVALID_HANDLE_VALUE;
	}

	if (m_handleTrace2nd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handleTrace2nd);
		m_handleTrace2nd = INVALID_HANDLE_VALUE;
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

	if (m_handleTrace >= 0)
	{
		close(m_handleTrace);
		m_handleTrace = -1;
	}

	if (m_handleTrace2nd >= 0)
	{
		close(m_handleTrace2nd);
		m_handleTrace2nd = -1;
	}

#endif
#endif // SOFEATURE_CMN_TRACE
} // closeTraceFiles
#ifdef SOFEATURE_CMN_TRACE
//-----------------------------------------------------------------------------
// elapsedTime
//
// - calculate elapsed time and convert it to a string
//
// returns:
//		pointer to string
//
LPCTSTR SOCmnTrace::elapsedTime(SOCmnString & elapsedTime, LONGLONG startTime, LONGLONG endTime)
{
	double timeDif;
#ifdef SOOS_WINDOWS
#ifdef TRACE_USE_PERFORMANCE_COUNTER
	timeDif = (double)(endTime - startTime) / m_pcPer1ms;
#else

	if (startTime > endTime)
	{
		//  the time wrapped arround 0; 49.7 days has passed
		timeDif  = (double)(ULONG_MAX - startTime + endTime);
	}
	else
	{
		timeDif = (double)(endTime - startTime);
	}

#endif // TRACE_USE_PERFORMANCE_COUNTER
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	timeDif = (double)(endTime - startTime) / 1000;
#endif
	elapsedTime.format(_T("%.1lf"),  timeDif);
	return (LPCTSTR)elapsedTime;
} // elapsedTime
#endif // SOFEATURE_CMN_TRACE
//-----------------------------------------------------------------------------
// setTraceHandler
//
// - set trace handler object
//
void SOCmnTrace::setTraceHandler(
	IN SOCmnTraceHandler * trace)   // trace handler object
{
	if (m_trcHandler)
	{
		SOCmnTraceHandler* traceHOld = m_trcHandler;
		m_trcHandler = NULL;
		traceHOld->release();
	}

	m_trcHandler = trace;

	if (m_trcHandler)
	{
		m_trcHandler->addRef();
	}
} // setTraceHandler
//-----------------------------------------------------------------------------
// getMaskString
//
// - get string of tracing group
//
// returns:
//		string of tracing group
//
LPCTSTR SOCmnTrace::getMaskString(
	IN DWORD mask)  // trace group
{
	switch (mask)
	{
	case SOCMNTRACE_G_ALL:
		return _T("ALL");
		break;

	case SOCMNTRACE_G_OPC_CALL:
		return _T("OPC");
		break;

	case SOCMNTRACE_G_OPC_ADVISE:
		return _T("OPA");
		break;

	case SOCMNTRACE_G_OPC_CALL_C:
		return _T("OpC");
		break;

	case SOCMNTRACE_G_OPC_ADVISE_C:
		return _T("OpA");
		break;

	case SOCMNTRACE_G_OTB:
		return _T("OTB");
		break;

	case SOCMNTRACE_G_OBJ_REF:
		return _T("REF");
		break;

	case SOCMNTRACE_G_OBJ_LOG:
		return _T("OLG");
		break;

	case SOCMNTRACE_G_ENTRY:
		return _T("ENT");
		break;

	case SOCMNTRACE_G_SRV_THREAD:
		return _T("SRV");
		break;

	case SOCMNTRACE_G_USER1:
	case SOCMNTRACE_G_USER2:
	case SOCMNTRACE_G_USER3:
	case SOCMNTRACE_G_USER4:
	case SOCMNTRACE_G_USER5:
	case SOCMNTRACE_G_USER6:
	case SOCMNTRACE_G_USER7:
	case SOCMNTRACE_G_USER8:
	{
		if (m_trcHandler)
		{
			m_trcHandler->getUserMaskString(mask, m_userMaskStr);
			return m_userMaskStr;
		}
	}
	break;
	}

	return _T("   ");
} // getMaskString
#ifdef SOFEATURE_CMN_VARIANT
//-----------------------------------------------------------------------------
// variant2string
//
// - convert variant to string
//
// returns:
//		pointer to string
//
LPCTSTR variant2string(
	IN SOCmnString * buffer,    // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID,  // locale id
	OPTIONAL IN BOOL readFormat)
{
	switch (var->vt)
	{
	case VT_EMPTY:
		*buffer = _T("");
		break;

	case VT_I1:
	{
		short sVal = (char)var->cVal;
		buffer->format(_T("%i"), sVal);
	}
	break;

	case VT_UI1:
	{
		unsigned short usVal = (unsigned short)var->bVal & 0xFF;
		buffer->format(_T("%u"), usVal);
	}
	break;

	case VT_I2:
		buffer->format(_T("%i"), var->iVal);
		break;

	case VT_UI2:
		buffer->format(_T("%u"), var->uiVal);
		break;

	case VT_I4:
		buffer->format(_T("%li"), var->lVal);
		break;

	case VT_UI4:
		buffer->format(_T("%lu"), var->ulVal);
		break;

	case VT_INT:
		buffer->format(_T("%i"), var->intVal);
		break;

	case VT_UINT:
		buffer->format(_T("%u"), var->uintVal);
		break;

	case VT_I8:
#ifdef SOOS_LINUX
		buffer->format(_T("%lld"), V_I8(var));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		buffer->format(_T("%I64d"), V_I8(var));
#endif
		break;

	case VT_UI8:
#ifdef SOOS_LINUX
		buffer->format(_T("%llu"), V_UI8(var));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		buffer->format(_T("%I64u"), V_UI8(var));
#endif
		break;

	case VT_BOOL:
		buffer->format(_T("%s"), (var->boolVal) ? _T("-1") : _T("0"));
		break;

	case VT_BSTR:
	{
		USES_CONVERSION;
		buffer->format(_T("%-.90s"), OLE2CT(var->bstrVal));
	}
	break;

	case VT_DATE:
	{
		SOCmnDateTime dt(&var->date);
		buffer->format(_T("%s"), (LPCTSTR)dt.getXMLDateTime());
	}
	break;

	case VT_R4:
	case VT_R8:
	case VT_CY:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_INT):
	case (VT_ARRAY | VT_UINT):
	case (VT_ARRAY | VT_I8):
	case (VT_ARRAY | VT_UI8):
	case (VT_ARRAY | VT_R4):
	case (VT_ARRAY | VT_R8):
	case (VT_ARRAY | VT_BOOL):
	case (VT_ARRAY | VT_BSTR):
	case (VT_ARRAY | VT_DATE):
	case (VT_ARRAY | VT_CY):
	case (VT_ARRAY | VT_VARIANT):
	{
		LONG lBound, uBound;
		LONG i;
		SOCmnString valStr;
		SafeArrayGetLBound(var->parray, 1, &lBound);
		SafeArrayGetUBound(var->parray, 1, &uBound);

		if (readFormat)
		{
			buffer->format(_T("[%ld,%ld] ("), lBound, uBound);
		}
		else
		{
			buffer->empty();
		}

		for (i = lBound; i <= uBound; i++)
		{
			if (buffer->getLength() > 250)
			{
				*buffer += _T("...");
				break;
			}

			switch (var->vt & ~VT_ARRAY)
			{
			case VT_I2:
			{
				short iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UI2:
			{
				unsigned short uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I4:
			{
				long lVal;
				SafeArrayGetElement(var->parray, &i, &lVal);
				valStr.format(_T("%li"), lVal);
			}
			break;

			case VT_UI4:
			{
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &ulVal);
				valStr.format(_T("%lu"), ulVal);
			}
			break;

			case VT_INT:
			{
				int iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UINT:
			{
				unsigned int uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I8:
			{
				LONGLONG llVal;
				SafeArrayGetElement(var->parray, &i, &llVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%lld"), llVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64d"), llVal);
#endif
			}
			break;

			case VT_UI8:
			{
				ULONGLONG ullVal;
				SafeArrayGetElement(var->parray, &i, &ullVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%llu"), ullVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64u"), ullVal);
#endif
			}
			break;

			case VT_UI1:
			{
				char bVal;
				unsigned short usVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				usVal = (unsigned short)bVal & 0xFF;
				valStr.format(_T("%u"), usVal);
			}
			break;

			case VT_I1:
			{
				char cVal;
				short sVal;
				SafeArrayGetElement(var->parray, &i, &cVal);
				sVal = (char)cVal;
				valStr.format(_T("%i"), sVal);
			}
			break;

			case VT_R4:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.fltVal);
				bstr.vt = VT_R4;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_R8:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_R8;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BOOL:
			{
				VARIANT_BOOL boolVal;
				SafeArrayGetElement(var->parray, &i, &boolVal);
				valStr.format(_T("%s"), (boolVal) ? _T("-1") : _T("0"));
			}
			break;

			case VT_CY:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.cyVal);
				bstr.vt = VT_CY;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BSTR:
			{
				USES_CONVERSION;
				BSTR bstrVal = NULL;
				::SafeArrayGetElement(var->parray, &i, &bstrVal);
				valStr.format(_T("%-.90s"), OLE2CT(bstrVal));
				::SysFreeString(bstrVal);
			}
			break;

			case VT_DATE:
			{
				double dblVal;
				SafeArrayGetElement(var->parray, &i, &dblVal);
				SOCmnDateTime dt(&dblVal);
				valStr = dt.getXMLDateTime();
			}
			break;

			case VT_VARIANT:
			{
				SOCmnVariant varV;
				SafeArrayGetElement(var->parray, &i, (void*)(LPCVARIANT)varV);
				variant2string(&valStr, &varV, localeID);
			}
			break;
			}

			*buffer += valStr;

			if (readFormat)
			{
				if (i + 1 <= uBound)
				{
					*buffer += _T(", ");
				}
				else
				{
					*buffer +=  _T(")");
				}
			}
			else
			{
				if (i + 1 <= uBound)
				{
					*buffer += _T("; ");
				}
			}
		}
	}
	break;

	default:
		buffer->format(_T("VARIANT vt; 0x%8.8X"), var->vt);
		break;
	}

	return (LPCTSTR) * buffer;
} // variant2string
//-----------------------------------------------------------------------------
// variantWithType2string
//
// - convert variant with vartype to string
//
// returns:
//		pointer to string
//
LPCTSTR variantWithType2string(
	IN SOCmnString * buffer,    // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID)  // locale id
{
	SOCmnString type;

	switch (var->vt)
	{
	case VT_EMPTY:
		*buffer = _T("EMPTY");
		break;

	case VT_I1:
	{
		short sVal = (char)var->cVal;
		buffer->format(_T("I1(%i)"), sVal);
	}
	break;

	case VT_UI1:
	{
		unsigned short usVal = (unsigned short)var->bVal & 0xFF;
		buffer->format(_T("UI1(%u)"), usVal);
	}
	break;

	case VT_I2:
		buffer->format(_T("I2(%i)"), var->iVal);
		break;

	case VT_UI2:
		buffer->format(_T("UI2(%u)"), var->uiVal);
		break;

	case VT_I4:
		buffer->format(_T("I4(%li)"), var->lVal);
		break;

	case VT_UI4:
		buffer->format(_T("UI4(%lu)"), var->ulVal);
		break;

	case VT_INT:
		buffer->format(_T("INT(%i)"), var->intVal);
		break;

	case VT_UINT:
		buffer->format(_T("UINT(%u)"), var->uintVal);
		break;

	case VT_I8:
#ifdef SOOS_LINUX
		buffer->format(_T("I8(%lld)"), V_I8(var));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		buffer->format(_T("I8(%I64d)"), V_I8(var));
#endif
		break;

	case VT_UI8:
#ifdef SOOS_LINUX
		buffer->format(_T("UI8(%llu)"), V_UI8(var));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		buffer->format(_T("UI8(%I64u)"), V_UI8(var));
#endif
		break;

	case VT_R4:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("R4(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_R8:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("R8(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_BOOL:
		buffer->format(_T("BOOL(%s)"), (var->boolVal) ? _T("-1") : _T("0"));
		break;

	case VT_CY:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("CY(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_BSTR:
	{
		USES_CONVERSION;
		buffer->format(_T("BSTR(%-.90s)"), OLE2CT(var->bstrVal));
	}
	break;

	case VT_DATE:
	{
		SOCmnDateTime dt(&var->date);
		buffer->format(_T("DATE(%s)"), (LPCTSTR)dt.getXMLDateTime());
	}
	break;

	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_INT):
	case (VT_ARRAY | VT_UINT):
	case (VT_ARRAY | VT_I8):
	case (VT_ARRAY | VT_UI8):
	case (VT_ARRAY | VT_R4):
	case (VT_ARRAY | VT_R8):
	case (VT_ARRAY | VT_BOOL):
	case (VT_ARRAY | VT_BSTR):
	case (VT_ARRAY | VT_DATE):
	case (VT_ARRAY | VT_CY):
	case (VT_ARRAY | VT_VARIANT):
	{
		LONG lBound, uBound;
		LONG i;
		SOCmnString valStr;
		SafeArrayGetLBound(var->parray, 1, &lBound);
		SafeArrayGetUBound(var->parray, 1, &uBound);
		buffer->format(_T("%s[%ld,%ld] ("), vartype2string(&type, (VARTYPE)(var->vt & ~VT_ARRAY)), lBound, uBound);

		for (i = lBound; i <= uBound; i++)
		{
			if (buffer->getLength() > 250)
			{
				*buffer += _T("...");
				break;
			}

			switch (var->vt & ~VT_ARRAY)
			{
			case VT_I2:
			{
				short iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UI2:
			{
				unsigned short uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I4:
			{
				long lVal;
				SafeArrayGetElement(var->parray, &i, &lVal);
				valStr.format(_T("%li"), lVal);
			}
			break;

			case VT_UI4:
			{
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &ulVal);
				valStr.format(_T("%lu"), ulVal);
			}
			break;

			case VT_INT:
			{
				int iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UINT:
			{
				unsigned int uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I8:
			{
				LONGLONG llVal;
				SafeArrayGetElement(var->parray, &i, &llVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%lld"), llVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64d"), llVal);
#endif
			}
			break;

			case VT_UI8:
			{
				ULONGLONG ullVal;
				SafeArrayGetElement(var->parray, &i, &ullVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%llu"), ullVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64u"), ullVal);
#endif
			}
			break;

			case VT_I1:
			{
				char cVal;
				short sVal;
				SafeArrayGetElement(var->parray, &i, &cVal);
				sVal = (char)cVal;
				valStr.format(_T("%i"), sVal);
			}
			break;

			case VT_UI1:
			{
				BYTE bVal;
				unsigned short usVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				usVal = (unsigned short)(bVal & 0xFF);
				valStr.format(_T("%u"), usVal);
			}
			break;

			case VT_R4:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.fltVal);
				bstr.vt = VT_R4;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_R8:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_R8;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BOOL:
			{
				VARIANT_BOOL boolVal;
				SafeArrayGetElement(var->parray, &i, &boolVal);
				valStr.format(_T("%s"), (boolVal) ? _T("-1") : _T("0"));
			}
			break;

			case VT_CY:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_CY;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BSTR:
			{
				USES_CONVERSION;
				BSTR bstrVal = NULL;
				::SafeArrayGetElement(var->parray, &i, &bstrVal);
				valStr.format(_T("%-.80s"), OLE2CT(bstrVal));
				::SysFreeString(bstrVal);
			}
			break;

			case VT_DATE:
			{
				double dblVal;
				SafeArrayGetElement(var->parray, &i, &dblVal);
				SOCmnDateTime dt(&dblVal);
				valStr = dt.getXMLDateTime();
			}
			break;

			case VT_VARIANT:
			{
				SOCmnVariant varV;
				SafeArrayGetElement(var->parray, &i, (void*)(LPCVARIANT)varV);
				variantWithType2string(&valStr, &varV, localeID);
			}
			break;
			}

			*buffer += valStr;

			if (i + 1 <= uBound)
			{
				*buffer += _T(", ");
			}
			else
			{
				*buffer +=  _T(")");
			}
		}
	}
	break;

	default:
		buffer->format(_T("VARIANT vt; 0x%8.8X"), var->vt);
		break;
	}

	return (LPCTSTR) * buffer;
} // variantWithType2string
//-----------------------------------------------------------------------------
// vartype2string
//
// - convert vartype to string
//
// returns:
//		pointer to string
//
LPCTSTR vartype2string(
	IN SOCmnString * buffer, // string buffer
	IN VARTYPE vt)      // vartype
{
	switch (vt)
	{
	case VT_I1:
		*buffer = _T("I1");
		break;

	case VT_UI1:
		*buffer = _T("UI1");
		break;

	case VT_I2:
		*buffer = _T("I2");
		break;

	case VT_UI2:
		*buffer = _T("UI2");
		break;

	case VT_I4:
		*buffer = _T("I4");
		break;

	case VT_UI4:
		*buffer = _T("UI4");
		break;

	case VT_INT:
		*buffer = _T("INT");
		break;

	case VT_UINT:
		*buffer = _T("UINT");
		break;

	case VT_I8:
		*buffer = _T("I8");
		break;

	case VT_UI8:
		*buffer = _T("UI8");
		break;

	case VT_R4:
		*buffer = _T("R4");
		break;

	case VT_R8:
		*buffer = _T("R8");
		break;

	case VT_BOOL:
		*buffer = _T("BOOL");
		break;

	case VT_CY:
		*buffer = _T("CY");
		break;

	case VT_BSTR:
		*buffer = _T("BSTR");
		break;

	case VT_DATE:
		*buffer = _T("DATE");
		break;

	case VT_EMPTY:
		*buffer = _T("EMPTY");
		break;

	case VT_VARIANT:
		*buffer = _T("VARIANT");
		break;

	case VT_I1 | VT_ARRAY:
		*buffer = _T("I1 ARRAY");
		break;

	case VT_UI1 | VT_ARRAY:
		*buffer = _T("UI1 ARRAY");
		break;

	case VT_I2 | VT_ARRAY:
		*buffer = _T("I2 ARRAY");
		break;

	case VT_UI2 | VT_ARRAY:
		*buffer = _T("UI2 ARRAY");
		break;

	case VT_I4 | VT_ARRAY:
		*buffer = _T("I4 ARRAY");
		break;

	case VT_UI4 | VT_ARRAY:
		*buffer = _T("UI4 ARRAY");
		break;

	case VT_INT | VT_ARRAY:
		*buffer = _T("INT ARRAY");
		break;

	case VT_UINT | VT_ARRAY:
		*buffer = _T("UINT ARRAY");
		break;

	case VT_I8 | VT_ARRAY:
		*buffer = _T("I8 ARRAY");
		break;

	case VT_UI8 | VT_ARRAY:
		*buffer = _T("UI8 ARRAY");
		break;

	case VT_R4 | VT_ARRAY:
		*buffer = _T("R4 ARRAY");
		break;

	case VT_R8 | VT_ARRAY:
		*buffer = _T("R8 ARRAY");
		break;

	case VT_BOOL | VT_ARRAY:
		*buffer = _T("BOOL ARRAY");
		break;

	case VT_CY | VT_ARRAY:
		*buffer = _T("CY ARRAY");
		break;

	case VT_BSTR | VT_ARRAY:
		*buffer = _T("BSTR ARRAY");
		break;

	case VT_DATE | VT_ARRAY:
		*buffer = _T("DATE ARRAY");
		break;

	case VT_VARIANT | VT_ARRAY:
		*buffer = _T("VARIANT ARRAY");
		break;

	default:
		buffer->format(_T("VARTYPE 0x%8.8X"), vt);
		break;
	}

	return (LPCTSTR) * buffer;
} // vartype2string
#endif // SOFEATURE_CMN_VARIANT
#ifdef SOOS_WINDOWS
//-----------------------------------------------------------------------------
// clipformat2string
//
// - convert clipformat to string
//
// returns:
//		pointer to string
//
LPCTSTR clipformat2string(
	IN SOCmnString * buffer,    // string buffer
	IN CLIPFORMAT format)   // clipformat
{
#ifdef SOFEATURE_DA10
	LPTSTR buf = buffer->GetBuffer(100);
	GetClipboardFormatName(format, buf, 99);
	return (LPCTSTR) * buffer;
#else
	return NULL;
#endif
} // clipformat2string
#endif // SOOS_LINUX
#ifdef SOFEATURE_CMN_OPC
//-----------------------------------------------------------------------------
// quality2string
//
// - convert quality to string
//
// returns:
//		pointer to string
//
LPCTSTR quality2string(
	IN SOCmnString * buffer,   // string buffer
	IN WORD quality,           // quality
	OPTIONAL IN LCID locID) // locale id
{
	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (quality & OPC_QUALITY_MASK)
		{
		case OPC_QUALITY_BAD:
			*buffer = _T("BAD ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_CONFIG_ERROR:
				*buffer += _T("(config error)");
				break;

			case OPC_QUALITY_NOT_CONNECTED:
				*buffer += _T("(not connected)");
				break;

			case OPC_QUALITY_DEVICE_FAILURE:
				*buffer += _T("(device failure)");
				break;

			case OPC_QUALITY_SENSOR_FAILURE:
				*buffer += _T("(sensor failure)");
				break;

			case OPC_QUALITY_LAST_KNOWN:
				*buffer += _T("(last known)");
				break;

			case OPC_QUALITY_COMM_FAILURE:
				*buffer += _T("(comm failure)");
				break;

			case OPC_QUALITY_OUT_OF_SERVICE:
				*buffer += _T("(out of service)");
				break;

			case OPC_QUALITY_WAITING_FOR_INITIAL_DATA:
				*buffer += _T("(waiting for initial data)");
				break;
			}

			break;

		case OPC_QUALITY_UNCERTAIN:
			*buffer = _T("UNCERTAIN ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_LAST_USABLE:
				*buffer += _T("(last usable)");
				break;

			case OPC_QUALITY_SENSOR_CAL:
				*buffer += _T("(sensor cal)");
				break;

			case OPC_QUALITY_EGU_EXCEEDED:
				*buffer += _T("(egu exceeded)");
				break;

			case OPC_QUALITY_SUB_NORMAL:
				*buffer += _T("(sub normal)");
				break;
			}

			break;

		case OPC_QUALITY_GOOD:
			*buffer = _T("GOOD ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_LOCAL_OVERRIDE:
				*buffer += _T("(local override)");
				break;
			}

			break;

		default:
			buffer->format(_T("QUALITY 0x%4.4X"), quality);
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (quality & OPC_QUALITY_MASK)
		{
		case OPC_QUALITY_BAD:
			*buffer = _T("SCHLECHT ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_CONFIG_ERROR:
				*buffer += _T("(Konfigurationsfehler)");
				break;

			case OPC_QUALITY_NOT_CONNECTED:
				*buffer += _T("(Nicht verbunden)");
				break;

			case OPC_QUALITY_DEVICE_FAILURE:
				*buffer += _T("(Gerätefehler)");
				break;

			case OPC_QUALITY_SENSOR_FAILURE:
				*buffer += _T("(Sensorfehler)");
				break;

			case OPC_QUALITY_LAST_KNOWN:
				*buffer += _T("(Letzter Wert)");
				break;

			case OPC_QUALITY_COMM_FAILURE:
				*buffer += _T("(Kommunikationsfehler)");
				break;

			case OPC_QUALITY_OUT_OF_SERVICE:
				*buffer += _T("(Nicht aktualisiert)");
				break;

			case OPC_QUALITY_WAITING_FOR_INITIAL_DATA:
				*buffer += _T("(Warte auf Initialisierungsdaten)");
				break;
			}

			break;

		case OPC_QUALITY_UNCERTAIN:
			*buffer = _T("UNSICHER ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_LAST_USABLE:
				*buffer += _T("(Zuletzt nutzbar)");
				break;

			case OPC_QUALITY_SENSOR_CAL:
				*buffer += _T("(Sensor-Kalibrierung)");
				break;

			case OPC_QUALITY_EGU_EXCEEDED:
				*buffer += _T("(EGU-Überschritten)");
				break;

			case OPC_QUALITY_SUB_NORMAL:
				*buffer += _T("(Sub-Normal)");
				break;
			}

			break;

		case OPC_QUALITY_GOOD:
			*buffer = _T("GUT ");

			switch (quality & OPC_STATUS_MASK)
			{
			case OPC_QUALITY_LOCAL_OVERRIDE:
				*buffer += _T("(Lokal überschrieben)");
				break;
			}

			break;

		default:
			buffer->format(_T("QUALITÄT 0x%4.4X"), quality);
			break;
		}

		break;
		break;

	default:
		buffer->format(_T("0x%4.4X"), quality);
		break;
	}

	return (LPCTSTR) * buffer;
} // quality2string
//-----------------------------------------------------------------------------
// accessRights2string
//
// - convert access rights to string
//
// returns:
//		pointer to string
//
LPCTSTR accessRights2string(
	IN SOCmnString * buffer,    // string buffer
	IN DWORD accessRight,      // access rights
	OPTIONAL IN LCID locID) // locale id
{
	LPCTSTR read = NULL;
	LPCTSTR write = NULL;
	LPCTSTR andd = NULL;
	*buffer = _T("");

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		read = _T("read");
		write = _T("write");
		andd = _T(" and ");
		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		read = _T("lesbar");
		write = _T("schreibbar");
		andd = _T(" und ");
		break;
	}

	if ((!read) || (!write) || (!andd))
	{
		return (LPCTSTR) * buffer;
	}

	if ((accessRight & OPC_READABLE) == OPC_READABLE)
	{
		*buffer += read;
	}

	if ((accessRight & OPC_WRITEABLE) == OPC_WRITEABLE)
	{
		if (buffer->getLength() != 0)
		{
			*buffer += andd;
		}

		*buffer += write;
	}

	return (LPCTSTR) * buffer;
} // accessRights2string
#endif // SOFEATURE_CMN_OPC
#ifdef SOOS_WINDOWS
//-----------------------------------------------------------------------------
// filetime2string
//
// - convert filetime to string
//
// returns:
//		pointer to string
//
LPCTSTR filetime2string(
	IN SOCmnString * buffer,    // string buffer
	IN LPFILETIME filetime) // filetime
{
	SYSTEMTIME t;
	FileTimeToSystemTime(filetime, &t);
	buffer->format(_T("%2.2d:%2.2d:%2.2d.%3.3d"), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return (LPCTSTR) * buffer;
} // filetime2string
#endif // SOOS_WINDOWS
#ifdef SOOS_WINDOWS
//-----------------------------------------------------------------------------
// IID2string
//
// - convert interface id into interface description string
//
// returns:
//		pointer to string
//
LPCTSTR IID2string(
	IN SOCmnString * buffer,    // string buffer
	IN REFIID riid)         // interface id
{
	LPWSTR wIIDstring = NULL;
	SOCmnString iidString;
	StringFromIID(riid, &wIIDstring);

	if (wIIDstring)
	{
		USES_CONVERSION;
		iidString = OLE2T(wIIDstring);
	}

	if (!iidString.IsEmpty())
	{
		TCHAR iidPath[60];
		TCHAR iidDescr[100];
		ULONG size = 100;
		DWORD type = REG_SZ;
		iidDescr[0] = '\0';
		_stprintf(iidPath, _T("Interface\\%-.38s"), (LPCTSTR)iidString);
		RegQueryValueEx(HKEY_CLASSES_ROOT, iidPath, NULL, &type, (LPBYTE)iidDescr, &size);

		if (_tcslen(iidDescr) > 0)
		{
			iidString = iidDescr;
		}
	}

	*buffer = iidString;

	if (wIIDstring)
	{
		CoTaskMemFree(wIIDstring);
	}

	return (LPCTSTR) * buffer;
} // IID2string
#endif // SOOS_WINDOWS
LPCTSTR byteArrayToString(IN SOCmnString & buffer,      // string buffer
						  DWORD len, BYTE * data)
{
	SOCmnString elStr;
	DWORD i;
	buffer.empty();
	LPTSTR bufP = buffer.getBuffer(len * 3);

	for (i = 0; i < len; i ++)
	{
		if (i < len - 1)
		{
			elStr.format(_T("%2.2X "), data[i]);
		}
		else
		{
			elStr.format(_T("%2.2X"), data[i]);
		}

		_tcscat(bufP, elStr);
	}

	return buffer;
}
