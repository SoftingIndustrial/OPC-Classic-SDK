#ifndef _OSCOMPAT_H_
#define _OSCOMPAT_H_

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <oaidl.h>
#include <tchar.h>

#ifdef _MSC_VER
#pragma warning( disable : 4251 )
#pragma warning( disable : 4786 )
#pragma warning (disable : 4100 )
#pragma warning (disable : 4702 )
#endif  //  _MSC_VER

#endif // _WINDOWS

#if defined(WIN32) || defined(WIN64)
#ifndef TBC_OS_WINDOWS
#define TBC_OS_WINDOWS
#endif

#else

#ifdef __linux__
#ifndef TBC_OS_LINUX
#define TBC_OS_LINUX
#endif
#endif // __linux__

#endif // WIN32


#if !defined(TBC_OS_WINDOWS) && !defined(TBC_OS_LINUX)
#error fatal: unsupported platform!
#endif


#include <string>
#include <sstream>

#ifdef TBC_OS_LINUX
#include "OTcommon.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#endif

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

#ifdef TBC_OS_WINDOWS
#ifndef TBC_USING_LIB
#define TBC_EXPORT __declspec(dllexport)
#else
#define TBC_EXPORT
#endif
#endif

#ifdef TBC_OS_LINUX
#define TBC_EXPORT
#endif

#ifdef TBC_OS_LINUX
typedef char TCHAR;
typedef char CHAR;
typedef unsigned char BYTE;
typedef short SHORT;
typedef unsigned short USHORT;
typedef long long LONGLONG;
typedef unsigned long ULONG;
typedef unsigned long DWORD;
typedef long LONG;
typedef float FLOAT;
typedef unsigned long long ULONGLONG;
typedef long BOOL;
const static long TRUE = 1L;
const static long FALSE = 0L;
#define _TCHAR char
#define _tprintf printf
#define _tcsncmp strncmp
#define _tcscmp strcmp
#define _tmain main
#endif

#ifndef _T
#if defined _UNICODE || defined UNICODE
#define _T(x)  L ## x
#else
#define _T(x)  x
#endif
#endif

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringstream;

#pragma pack(pop)

#endif // _OSCOMPAT_H_
