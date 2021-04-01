#include "OSCompat.h"
#include "Trace.h"
#include "OTcommon.h"
#include "SOVersion.h"

typedef enum tagTraceData
{

	TRACEDATA_ALL           = 0x000001FF,
	TRACEDATA_LEVEL         = 0x0000000F,
	TRACEDATA_LEVEL_ERR     = 0x00000008,
	TRACEDATA_LEVEL_WRN     = 0x00000004,
	TRACEDATA_LEVEL_INF     = 0x00000002,
	TRACEDATA_LEVEL_DEB     = 0x00000001,
	TRACEDATA_FILE          = 0x000000F0,
	TRACEDATA_FILE_ENABLE   = 0x00000010,
	TRACEDATA_FILE_NAME     = 0x00000020,
	TRACEDATA_MAX_BACKUPS   = 0x00000040,
	TRACEDATA_FILE_SIZE     = 0x00000080,
	TRACEDATA_API_VERSION   = 0x00000100

}   EnumTraceData;

unsigned long Trace::m_errorLevelMask = 0;
unsigned long Trace::m_warningLevelMask = 0;
unsigned long Trace::m_infoLevelMask = 0;
unsigned long Trace::m_debugLevelMask = 0;
bool Trace::m_enableTraceToFile = false;
tstring Trace::m_fileName;
unsigned long Trace::m_maxFileSize = 0;
unsigned long Trace::m_maxBackups = 0;

unsigned long Trace::getDebugLevelMask(void)
{
	return m_debugLevelMask;
}// end GetDebugLevelMask

void Trace::setDebugLevelMask(unsigned long aMask)
{
	m_debugLevelMask = aMask;
}// end SetDebugLevelMask

unsigned long Trace::getInfoLevelMask(void)
{
	return m_infoLevelMask;
}// end GetInfoLevelMask

void Trace::setInfoLevelMask(unsigned long aMask)
{
	m_infoLevelMask = aMask;
}// end SetInfoLevelMask

// Level of warnings traced
unsigned long Trace::getWarningLevelMask(void)
{
	return m_warningLevelMask;
}// end GetWarningLevelMask

void Trace::setWarningLevelMask(unsigned long aMask)
{
	m_warningLevelMask = aMask;
}// end SetWarningLevelMask

unsigned long Trace::getErrorLevelMask(void)
{
	return m_errorLevelMask;
}// end GetErrorLevelMask

void Trace::setErrorLevelMask(unsigned long aMask)
{
	m_errorLevelMask = aMask;
}// end SetErrorLevelMask

bool Trace::getEnableTraceToFile(void)
{
	return m_enableTraceToFile;
}// end GetEnableTraceToFile

void Trace::setEnableTraceToFile(bool isEnabled)
{
	m_enableTraceToFile = isEnabled;
	setTraceOptions(TRACEDATA_ALL);
}// end setEnableTraceToFile

unsigned long Trace::getFileMaxSize(void)
{
	return m_maxFileSize;
}// end getFileMaxSize

void Trace::setFileMaxSize(unsigned long aMaxSize)
{
	m_maxFileSize = aMaxSize;
}// end setFileMaxSize

tstring& Trace::getFileName(void)
{
	return m_fileName;
}   //  end getFileName

void Trace::setFileName(tstring& aFileName)
{
	m_fileName = aFileName;
}   //  end setFileName

unsigned long Trace::getMaxBackups()
{
	return m_maxBackups;
}   //  end getMaxBackups

void Trace::setMaxBackups(unsigned long maxBackups)
{
	m_maxBackups = maxBackups;
}   //  end setMaxBackups

void Trace::setTraceOptions(unsigned long aWhatDataFlag)
{
	OTTraceData traceData;
	traceData.m_debugLevelMask = m_debugLevelMask;
	traceData.m_infoLevelMask = m_infoLevelMask;
	traceData.m_warningLevelMask = m_warningLevelMask;
	traceData.m_errorLevelMask = m_errorLevelMask;
	traceData.m_enableTraceToFile = (unsigned char)m_enableTraceToFile;
	traceData.m_maxFileSize = m_maxFileSize;
	traceData.m_maxBackups  =   m_maxBackups;

	if (m_fileName.empty())
	{
		traceData.m_fileName = NULL;
	}
	else
	{
		traceData.m_fileName = (OTChar*) m_fileName.c_str();
	}   //  end if ... else

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

	TCHAR apiVersion[128];
	_stprintf(apiVersion, _T("TBC%s %u.%u.%u.%u, built with VC++ %u, running on Microsoft Windows NT %u.%u.%u (%u) %s %s"),
		(sizeof(TCHAR) == 1 ? _T("") : _T("u")), MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER, _MSC_VER,
		winMajorVersion, winMinorVersion, winBuildNr, platformId, csdVersion, sizeof(ptrdiff_t) == 4 ? _T("x86") : _T("x64"));

	traceData.m_apiVersion = apiVersion;

	OTEnableTracing(aWhatDataFlag, &traceData);
}   //  end setTraceOptions


void Trace::writeline(
	unsigned char aLevel,
	unsigned long aMask,
	tstring& anObjectID,
	tstring& aMessage)
{
	OTTrace(aLevel, aMask, (OTChar*)anObjectID.c_str(), (OTChar*)aMessage.c_str());
}   //  end writeline
