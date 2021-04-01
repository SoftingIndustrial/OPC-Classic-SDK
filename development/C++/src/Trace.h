#ifndef _TRACE_H_
#define _TRACE_H_

#include "Enums.h"

typedef enum tagEnumTraceGroup
{

	EnumTraceGroup_ALL = 0xFFFF000F,

	EnumTraceGroup_NOTHING  = 0x00000000,

	EnumTraceGroup_USER = 0xFF000000,

	EnumTraceGroup_OPCCLIENT = 0x0000000C,

	EnumTraceGroup_OPCCLIENTCALL = 0x00000004,

	EnumTraceGroup_OPCCLIENTADVISE = 0x00000008,

	EnumTraceGroup_OPCSERVER = 0x00000003,

	EnumTraceGroup_OPCSERVERCALL = 0x00000001,

	EnumTraceGroup_OPCSERVERADVISE = 0x00000002,

	EnumTraceGroup_CLIENT = 0x00800000,

	EnumTraceGroup_SERVER = 0x00400000,

	EnumTraceGroup_OTSIN = 0x00010000,

	EnumTraceGroup_OTSOUT = 0x00020000,

	EnumTraceGroup_OTCIN = 0x00040000,

	EnumTraceGroup_OTCOUT = 0x00080000,

	EnumTraceGroup_LICENSE = 0x00200000,

	EnumTraceGroup_USER1 = 0x80000000,

	EnumTraceGroup_USER2 = 0x40000000,

	EnumTraceGroup_USER3 = 0x20000000,

	EnumTraceGroup_USER4 = 0x10000000,

	EnumTraceGroup_USER5 = 0x08000000,

	EnumTraceGroup_USER6 = 0x04000000,

	EnumTraceGroup_USER7 = 0x02000000,

	EnumTraceGroup_USER8 = 0x01000000

} EnumTraceGroup;

typedef enum tagEnumTraceLevel
{

	EnumTraceLevel_ERR = 0,

	EnumTraceLevel_WRN = 1,

	EnumTraceLevel_INF = 2,

	EnumTraceLevel_DEB = 3

} EnumTraceLevel;

// Trace handler class: Traces either and to debug out and to one or two files
class Trace
{

private:

	static unsigned long m_errorLevelMask;
	static unsigned long m_warningLevelMask;
	static unsigned long m_infoLevelMask;
	static unsigned long m_debugLevelMask;
	static bool m_enableTraceToFile;
	static tstring m_fileName;
	static unsigned long m_maxFileSize;
	static unsigned long m_maxBackups;

	static void setTraceOptions(unsigned long aWhatDataFlag);

public:


	static unsigned long getDebugLevelMask();
	static void setDebugLevelMask(unsigned long aMask);

	static unsigned long getInfoLevelMask();
	static void setInfoLevelMask(unsigned long aMask);

	static unsigned long getWarningLevelMask();
	static void setWarningLevelMask(unsigned long aMask);

	static unsigned long getErrorLevelMask();
	static void setErrorLevelMask(unsigned long aMask);

	static bool getEnableTraceToFile();
	static void setEnableTraceToFile(bool isEnabled);

	static unsigned long getFileMaxSize();
	static void setFileMaxSize(unsigned long aMaxSize);

	static tstring& getFileName();
	static void setFileName(tstring& aFileName);

	static unsigned long getMaxBackups();
	static void setMaxBackups(unsigned long maxBackups);

	static void writeline(unsigned char aLevel, unsigned long aMask, tstring& anObjectID, tstring& aMessage);

};  //end Trace

#endif
