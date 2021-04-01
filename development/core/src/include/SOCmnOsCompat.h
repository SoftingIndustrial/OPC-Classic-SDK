#ifndef _SOCMNOSCOMPAT_
#define _SOCMNOSCOMPAT_

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

#ifdef SOOS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#ifndef SOFLAG_NO_ATL
#define _ATL_STATIC_REGISTRY
#include <atlbase.h>
#endif // SOFLAG_NO_ATL

#pragma warning (disable:4100)

#ifndef LLONG_MAX
#define LLONG_MAX     0x7fffffffffffffff
#endif
#ifndef LLONG_MIN
#define LLONG_MIN     0x8000000000000000
#endif
#ifndef ULLONG_MAX
#define ULLONG_MAX    0xffffffffffffffff
#endif


#endif // SOOS_WINDOWS

#ifdef SOOS_LINUX
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>

#ifndef SODEVENV_CYGWIN
#include <endian.h>
#endif


#define SOFLAG_NO_DLLS

#ifndef SODEVENV_CYGWIN
#define _stdcall
#define __cdecl
#endif
#define IN
#define OUT
#define OPTIONAL

#define _T(text)    text
#define _tcscpy     strcpy
#define _tcscat     strcat
#define _tcsdup     strdup
#define _tcsstr     strstr
#define _tcschr     strchr
#define _tcsrchr    strrchr
#define _tcscmp     strcmp
#define _tcsicmp    strcasecmp
#define _tcslen     strlen
#define _tcspbrk    strpbrk
#define _tcsncpy    strncpy
#define _tcstoul    strtoul
#define _tcstol     strtol
#define _tcstod     strtod
#define _tcsncmp    strncmp
#define _tcstoi64   strtoll
#define _tcstoui64  strtoull
#define _tcstok     strtok
#define _tfopen     fopen

#define _tcsdec(_cpc1, _cpc2) ((_cpc1)>=(_cpc2) ? NULL : (_cpc2)-1)
#define _tcsinc(_pc)  ((_pc)+1)

#define _tprintf    printf
#define _ftprintf   fprintf
#define _stprintf   sprintf
#define _sntprintf  snprintf
#define _vstprintf  vsprintf
#define _vsntprintf vsnprintf
#define _tscanf     scanf
#define _ftscanf    fscanf
#define _stscanf    sscanf

#define lstrlen     strlen
#define lstrcpy     strcpy
#define lstrcat     strcat

/* ctype-functions */

#define _istalnum   isalnum
#define _istalpha   isalpha
#define _istdigit   isdigit
#define _istxdigit  isxdigit
#define _istgraph   isgraph
#define _istlower   islower
#define _istprint   isprint
#define _istpunct   ispunct
#define _istspace   isspace
#define _istupper   isupper

#define _totupper   toupper
#define _totlower   tolower

#define USES_CONVERSION
#define T2A(text)     text
#define T2CA(text)    text
#define A2T(text)     text
#define A2CT(text)    text
#define OLE2CT(text)  text
#define OLE2T(text)   text
#define T2OLE(text)   text
#define T2COLE(text)  text
#define OLESTR(text)  text

typedef void          VOID;
typedef VOID*         LPVOID;

typedef char          CHAR;
typedef char          TCHAR;
typedef wchar_t       WCHAR;
typedef unsigned char _TUCHAR;
//typedef WCHAR         OLECHAR;

typedef CHAR*         LPSTR;
typedef const CHAR*   LPCSTR;
typedef TCHAR*        LPTSTR;
typedef const TCHAR* LPCTSTR;
typedef WCHAR*        LPWSTR;
typedef const WCHAR* LPCWSTR;
//typedef OLECHAR*      LPOLESTR;
//typedef const OLECHAR* LPCOLESTR;

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef unsigned long long ULONGLONG;
typedef short int      SHORT;
typedef long           LONG;
typedef long long      LONGLONG;
typedef float          FLOAT;
typedef double         DOUBLE;
typedef int            INT;
typedef unsigned int   UINT;
typedef int            BOOL;
typedef unsigned short CLIPFORMAT;

#define TRUE 1
#define FALSE 0

typedef LONG            HRESULT;
#define S_OK            0x00000000L
#define S_FALSE         0x00000001L
#define E_NOTIMPL       0x80004001L
#define E_FAIL          0x80004005L
#define E_UNEXPECTED    0x8000FFFFL
#define DISP_E_TYPEMISMATCH 0x80020005L
#define DISP_E_BADVARTYPE   0x80020008L
#define DISP_E_OVERFLOW     0x8002000AL
#define DISP_E_BADINDEX     0x8002000BL
#define DISP_E_UNKNOWNLCID  0x8002000CL
#define DISP_E_ARRAYISLOCKED    0x8002000DL
#define E_ACCESSDENIED  0x80070005L
#define E_OUTOFMEMORY   0x8007000EL
#define E_INVALIDARG    0x80070057L
#define RPC_E_INVALID_DATA  0x8001000FL
#define RPC_E_SERVERFAULT   0x80010105L
#define E_PENDING           0x8000000AL
#define E_ABORT 0x80004004L

#define SUCCEEDED(Status) ((LONG)(void *)(Status) >= 0)
#define FAILED(Status) ((LONG)(void *)(Status)<0)

typedef DWORD          LCID;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif
#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX-1)
#endif
#ifndef ULLONG_MAX
#define ULLONG_MAX (LLONG_MAX * 2ULL + 1)
#endif


#ifdef SOFEATURE_CMN_VARIANT

typedef WORD VARTYPE;

typedef short int VARIANT_BOOL;
#define VARIANT_TRUE ((VARIANT_BOOL)-1)
#define VARIANT_FALSE ((VARIANT_BOOL)0)

typedef struct tagDEC
{
	USHORT wReserved;
	BYTE scale;
	BYTE sign;
	ULONG Hi32;
	ULONGLONG Lo64;
}  DECIMAL;

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
	LONGLONG int64;
} CY;

typedef LPTSTR BSTR;
typedef CY CURRENCY;
typedef struct timeval DATE;
class IUnknown;
class IDispatch;
typedef struct  tagSAFEARRAYBOUND
{
	ULONG cElements;
	LONG lLbound;
}   SAFEARRAYBOUND;

typedef struct tagSAFEARRAYBOUND* LPSAFEARRAYBOUND;

typedef struct  tagSAFEARRAY
{
	USHORT cDims;
	USHORT fFeatures;
	ULONG cbElements;
	ULONG cLocks;
	LPVOID pvData;
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
	WORD wReserved1;
	WORD wReserved2;
	WORD wReserved3;
	union
	{
		LONG lVal;
		BYTE bVal;
		SHORT iVal;
		FLOAT fltVal;
		DOUBLE dblVal;
		VARIANT_BOOL boolVal;
		CY cyVal;
		DATE date;
		BSTR bstrVal;
		IUnknown* punkVal;
		IDispatch* pdispVal;
		SAFEARRAY* parray;
		VARIANT* pvarVal;
		CHAR cVal;
		USHORT uiVal;
		ULONG ulVal;
		LONGLONG llVal;   /* <-- NOT UNDER WINDOWS, use V_I8(&var)  */
		ULONGLONG ullVal; /* <-- NOT UNDER WINDOWS, use V_UI8(&var) */
		INT intVal;
		UINT uintVal;
	};
};

typedef VARIANT* LPVARIANT;

#endif // SOFEATURE_CMN_VARIANT


//
//  Language IDs.
//
//  The following two combinations of primary language ID and
//  sublanguage ID have special semantics:
//
//    Primary Language ID   Sublanguage ID      Result
//    -------------------   ---------------     ------------------------
//    LANG_NEUTRAL          SUBLANG_NEUTRAL     Language neutral
//    LANG_NEUTRAL          SUBLANG_DEFAULT     User default language
//    LANG_NEUTRAL          SUBLANG_SYS_DEFAULT System default language
//    LANG_INVARIANT        SUBLANG_NEUTRAL     Invariant locale
//

//
//  Primary language IDs.
//

#define LANG_NEUTRAL                     0x00
#define LANG_INVARIANT                   0x7f

#define LANG_AFRIKAANS                   0x36
#define LANG_ALBANIAN                    0x1c
#define LANG_ARABIC                      0x01
#define LANG_ARMENIAN                    0x2b
#define LANG_ASSAMESE                    0x4d
#define LANG_AZERI                       0x2c
#define LANG_BASQUE                      0x2d
#define LANG_BELARUSIAN                  0x23
#define LANG_BENGALI                     0x45
#define LANG_BULGARIAN                   0x02
#define LANG_CATALAN                     0x03
#define LANG_CHINESE                     0x04
#define LANG_CROATIAN                    0x1a
#define LANG_CZECH                       0x05
#define LANG_DANISH                      0x06
#define LANG_DIVEHI                      0x65
#define LANG_DUTCH                       0x13
#define LANG_ENGLISH                     0x09
#define LANG_ESTONIAN                    0x25
#define LANG_FAEROESE                    0x38
#define LANG_FARSI                       0x29
#define LANG_FINNISH                     0x0b
#define LANG_FRENCH                      0x0c
#define LANG_GALICIAN                    0x56
#define LANG_GEORGIAN                    0x37
#define LANG_GERMAN                      0x07
#define LANG_GREEK                       0x08
#define LANG_GUJARATI                    0x47
#define LANG_HEBREW                      0x0d
#define LANG_HINDI                       0x39
#define LANG_HUNGARIAN                   0x0e
#define LANG_ICELANDIC                   0x0f
#define LANG_INDONESIAN                  0x21
#define LANG_ITALIAN                     0x10
#define LANG_JAPANESE                    0x11
#define LANG_KANNADA                     0x4b
#define LANG_KASHMIRI                    0x60
#define LANG_KAZAK                       0x3f
#define LANG_KONKANI                     0x57
#define LANG_KOREAN                      0x12
#define LANG_KYRGYZ                      0x40
#define LANG_LATVIAN                     0x26
#define LANG_LITHUANIAN                  0x27
#define LANG_MACEDONIAN                  0x2f   // the Former Yugoslav Republic of Macedonia
#define LANG_MALAY                       0x3e
#define LANG_MALAYALAM                   0x4c
#define LANG_MANIPURI                    0x58
#define LANG_MARATHI                     0x4e
#define LANG_MONGOLIAN                   0x50
#define LANG_NEPALI                      0x61
#define LANG_NORWEGIAN                   0x14
#define LANG_ORIYA                       0x48
#define LANG_POLISH                      0x15
#define LANG_PORTUGUESE                  0x16
#define LANG_PUNJABI                     0x46
#define LANG_ROMANIAN                    0x18
#define LANG_RUSSIAN                     0x19
#define LANG_SANSKRIT                    0x4f
#define LANG_SERBIAN                     0x1a
#define LANG_SINDHI                      0x59
#define LANG_SLOVAK                      0x1b
#define LANG_SLOVENIAN                   0x24
#define LANG_SPANISH                     0x0a
#define LANG_SWAHILI                     0x41
#define LANG_SWEDISH                     0x1d
#define LANG_SYRIAC                      0x5a
#define LANG_TAMIL                       0x49
#define LANG_TATAR                       0x44
#define LANG_TELUGU                      0x4a
#define LANG_THAI                        0x1e
#define LANG_TURKISH                     0x1f
#define LANG_UKRAINIAN                   0x22
#define LANG_URDU                        0x20
#define LANG_UZBEK                       0x43
#define LANG_VIETNAMESE                  0x2a

//
//  Sublanguage IDs.
//
//  The name immediately following SUBLANG_ dictates which primary
//  language ID that sublanguage ID can be combined with to form a
//  valid language ID.
//

#define SUBLANG_NEUTRAL                  0x00    // language neutral
#define SUBLANG_DEFAULT                  0x01    // user default
#define SUBLANG_SYS_DEFAULT              0x02    // system default

#define SUBLANG_ARABIC_SAUDI_ARABIA      0x01    // Arabic (Saudi Arabia)
#define SUBLANG_ARABIC_IRAQ              0x02    // Arabic (Iraq)
#define SUBLANG_ARABIC_EGYPT             0x03    // Arabic (Egypt)
#define SUBLANG_ARABIC_LIBYA             0x04    // Arabic (Libya)
#define SUBLANG_ARABIC_ALGERIA           0x05    // Arabic (Algeria)
#define SUBLANG_ARABIC_MOROCCO           0x06    // Arabic (Morocco)
#define SUBLANG_ARABIC_TUNISIA           0x07    // Arabic (Tunisia)
#define SUBLANG_ARABIC_OMAN              0x08    // Arabic (Oman)
#define SUBLANG_ARABIC_YEMEN             0x09    // Arabic (Yemen)
#define SUBLANG_ARABIC_SYRIA             0x0a    // Arabic (Syria)
#define SUBLANG_ARABIC_JORDAN            0x0b    // Arabic (Jordan)
#define SUBLANG_ARABIC_LEBANON           0x0c    // Arabic (Lebanon)
#define SUBLANG_ARABIC_KUWAIT            0x0d    // Arabic (Kuwait)
#define SUBLANG_ARABIC_UAE               0x0e    // Arabic (U.A.E)
#define SUBLANG_ARABIC_BAHRAIN           0x0f    // Arabic (Bahrain)
#define SUBLANG_ARABIC_QATAR             0x10    // Arabic (Qatar)
#define SUBLANG_AZERI_LATIN              0x01    // Azeri (Latin)
#define SUBLANG_AZERI_CYRILLIC           0x02    // Azeri (Cyrillic)
#define SUBLANG_CHINESE_TRADITIONAL      0x01    // Chinese (Taiwan)
#define SUBLANG_CHINESE_SIMPLIFIED       0x02    // Chinese (PR China)
#define SUBLANG_CHINESE_HONGKONG         0x03    // Chinese (Hong Kong S.A.R., P.R.C.)
#define SUBLANG_CHINESE_SINGAPORE        0x04    // Chinese (Singapore)
#define SUBLANG_CHINESE_MACAU            0x05    // Chinese (Macau S.A.R.)
#define SUBLANG_DUTCH                    0x01    // Dutch
#define SUBLANG_DUTCH_BELGIAN            0x02    // Dutch (Belgian)
#define SUBLANG_ENGLISH_US               0x01    // English (USA)
#define SUBLANG_ENGLISH_UK               0x02    // English (UK)
#define SUBLANG_ENGLISH_AUS              0x03    // English (Australian)
#define SUBLANG_ENGLISH_CAN              0x04    // English (Canadian)
#define SUBLANG_ENGLISH_NZ               0x05    // English (New Zealand)
#define SUBLANG_ENGLISH_EIRE             0x06    // English (Irish)
#define SUBLANG_ENGLISH_SOUTH_AFRICA     0x07    // English (South Africa)
#define SUBLANG_ENGLISH_JAMAICA          0x08    // English (Jamaica)
#define SUBLANG_ENGLISH_CARIBBEAN        0x09    // English (Caribbean)
#define SUBLANG_ENGLISH_BELIZE           0x0a    // English (Belize)
#define SUBLANG_ENGLISH_TRINIDAD         0x0b    // English (Trinidad)
#define SUBLANG_ENGLISH_ZIMBABWE         0x0c    // English (Zimbabwe)
#define SUBLANG_ENGLISH_PHILIPPINES      0x0d    // English (Philippines)
#define SUBLANG_FRENCH                   0x01    // French
#define SUBLANG_FRENCH_BELGIAN           0x02    // French (Belgian)
#define SUBLANG_FRENCH_CANADIAN          0x03    // French (Canadian)
#define SUBLANG_FRENCH_SWISS             0x04    // French (Swiss)
#define SUBLANG_FRENCH_LUXEMBOURG        0x05    // French (Luxembourg)
#define SUBLANG_FRENCH_MONACO            0x06    // French (Monaco)
#define SUBLANG_GERMAN                   0x01    // German
#define SUBLANG_GERMAN_SWISS             0x02    // German (Swiss)
#define SUBLANG_GERMAN_AUSTRIAN          0x03    // German (Austrian)
#define SUBLANG_GERMAN_LUXEMBOURG        0x04    // German (Luxembourg)
#define SUBLANG_GERMAN_LIECHTENSTEIN     0x05    // German (Liechtenstein)
#define SUBLANG_ITALIAN                  0x01    // Italian
#define SUBLANG_ITALIAN_SWISS            0x02    // Italian (Swiss)
#define SUBLANG_KASHMIRI_SASIA           0x02    // Kashmiri (South Asia)
#define SUBLANG_KASHMIRI_INDIA           0x02    // For app compatibility only
#define SUBLANG_KOREAN                   0x01    // Korean (Extended Wansung)
#define SUBLANG_LITHUANIAN               0x01    // Lithuanian
#define SUBLANG_MALAY_MALAYSIA           0x01    // Malay (Malaysia)
#define SUBLANG_MALAY_BRUNEI_DARUSSALAM  0x02    // Malay (Brunei Darussalam)
#define SUBLANG_NEPALI_INDIA             0x02    // Nepali (India)
#define SUBLANG_NORWEGIAN_BOKMAL         0x01    // Norwegian (Bokmal)
#define SUBLANG_NORWEGIAN_NYNORSK        0x02    // Norwegian (Nynorsk)
#define SUBLANG_PORTUGUESE               0x02    // Portuguese
#define SUBLANG_PORTUGUESE_BRAZILIAN     0x01    // Portuguese (Brazilian)
#define SUBLANG_SERBIAN_LATIN            0x02    // Serbian (Latin)
#define SUBLANG_SERBIAN_CYRILLIC         0x03    // Serbian (Cyrillic)
#define SUBLANG_SPANISH                  0x01    // Spanish (Castilian)
#define SUBLANG_SPANISH_MEXICAN          0x02    // Spanish (Mexican)
#define SUBLANG_SPANISH_MODERN           0x03    // Spanish (Spain)
#define SUBLANG_SPANISH_GUATEMALA        0x04    // Spanish (Guatemala)
#define SUBLANG_SPANISH_COSTA_RICA       0x05    // Spanish (Costa Rica)
#define SUBLANG_SPANISH_PANAMA           0x06    // Spanish (Panama)
#define SUBLANG_SPANISH_DOMINICAN_REPUBLIC 0x07  // Spanish (Dominican Republic)
#define SUBLANG_SPANISH_VENEZUELA        0x08    // Spanish (Venezuela)
#define SUBLANG_SPANISH_COLOMBIA         0x09    // Spanish (Colombia)
#define SUBLANG_SPANISH_PERU             0x0a    // Spanish (Peru)
#define SUBLANG_SPANISH_ARGENTINA        0x0b    // Spanish (Argentina)
#define SUBLANG_SPANISH_ECUADOR          0x0c    // Spanish (Ecuador)
#define SUBLANG_SPANISH_CHILE            0x0d    // Spanish (Chile)
#define SUBLANG_SPANISH_URUGUAY          0x0e    // Spanish (Uruguay)
#define SUBLANG_SPANISH_PARAGUAY         0x0f    // Spanish (Paraguay)
#define SUBLANG_SPANISH_BOLIVIA          0x10    // Spanish (Bolivia)
#define SUBLANG_SPANISH_EL_SALVADOR      0x11    // Spanish (El Salvador)
#define SUBLANG_SPANISH_HONDURAS         0x12    // Spanish (Honduras)
#define SUBLANG_SPANISH_NICARAGUA        0x13    // Spanish (Nicaragua)
#define SUBLANG_SPANISH_PUERTO_RICO      0x14    // Spanish (Puerto Rico)
#define SUBLANG_SWEDISH                  0x01    // Swedish
#define SUBLANG_SWEDISH_FINLAND          0x02    // Swedish (Finland)
#define SUBLANG_URDU_PAKISTAN            0x01    // Urdu (Pakistan)
#define SUBLANG_URDU_INDIA               0x02    // Urdu (India)
#define SUBLANG_UZBEK_LATIN              0x01    // Uzbek (Latin)
#define SUBLANG_UZBEK_CYRILLIC           0x02    // Uzbek (Cyrillic)

//
//  Sorting IDs.
//

#define SORT_DEFAULT                     0x0     // sorting default

#define SORT_JAPANESE_XJIS               0x0     // Japanese XJIS order
#define SORT_JAPANESE_UNICODE            0x1     // Japanese Unicode order

#define SORT_CHINESE_BIG5                0x0     // Chinese BIG5 order
#define SORT_CHINESE_PRCP                0x0     // PRC Chinese Phonetic order
#define SORT_CHINESE_UNICODE             0x1     // Chinese Unicode order
#define SORT_CHINESE_PRC                 0x2     // PRC Chinese Stroke Count order
#define SORT_CHINESE_BOPOMOFO            0x3     // Traditional Chinese Bopomofo order

#define SORT_KOREAN_KSC                  0x0     // Korean KSC order
#define SORT_KOREAN_UNICODE              0x1     // Korean Unicode order

#define SORT_GERMAN_PHONE_BOOK           0x1     // German Phone Book order

#define SORT_HUNGARIAN_DEFAULT           0x0     // Hungarian Default order
#define SORT_HUNGARIAN_TECHNICAL         0x1     // Hungarian Technical order

#define SORT_GEORGIAN_TRADITIONAL        0x0     // Georgian Traditional order
#define SORT_GEORGIAN_MODERN             0x1     // Georgian Modern order

// end_r_winnt

//
//  A language ID is a 16 bit value which is the combination of a
//  primary language ID and a secondary language ID.  The bits are
//  allocated as follows:
//
//       +-----------------------+-------------------------+
//       |     Sublanguage ID    |   Primary Language ID   |
//       +-----------------------+-------------------------+
//        15                   10 9                       0   bit
//
//
//  Language ID creation/extraction macros:
//
//    MAKELANGID    - construct language id from a primary language id and
//                    a sublanguage id.
//    PRIMARYLANGID - extract primary language id from a language id.
//    SUBLANGID     - extract sublanguage id from a language id.
//

#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define PRIMARYLANGID(lgid)    ((WORD  )(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((WORD  )(lgid) >> 10)


//
//  A locale ID is a 32 bit value which is the combination of a
//  language ID, a sort ID, and a reserved area.  The bits are
//  allocated as follows:
//
//       +-------------+---------+-------------------------+
//       |   Reserved  | Sort ID |      Language ID        |
//       +-------------+---------+-------------------------+
//        31         20 19     16 15                      0   bit
//
//
//  Locale ID creation/extraction macros:
//
//    MAKELCID            - construct the locale id from a language id and a sort id.
//    MAKESORTLCID        - construct the locale id from a language id, sort id, and sort version.
//    LANGIDFROMLCID      - extract the language id from a locale id.
//    SORTIDFROMLCID      - extract the sort id from a locale id.
//    SORTVERSIONFROMLCID - extract the sort version from a locale id.
//

#define NLS_VALID_LOCALE_MASK  0x000fffff

#define MAKELCID(lgid, srtid)  ((DWORD)((((DWORD)((WORD  )(srtid))) << 16) |  \
                                         ((DWORD)((WORD  )(lgid)))))
#define MAKESORTLCID(lgid, srtid, ver)                                            \
                               ((DWORD)((MAKELCID(lgid, srtid)) |             \
                                    (((DWORD)((WORD  )(ver))) << 20)))
#define LANGIDFROMLCID(lcid)   ((WORD  )(lcid))
#define SORTIDFROMLCID(lcid)   ((WORD  )((((DWORD)(lcid)) >> 16) & 0xf))
#define SORTVERSIONFROMLCID(lcid)  ((WORD  )((((DWORD)(lcid)) >> 20) & 0xf))


//
//  Default System and User IDs for language and locale.
//

#define LANG_SYSTEM_DEFAULT    (MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))

#define LOCALE_SYSTEM_DEFAULT  (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))

#define LOCALE_NEUTRAL                                                        \
          (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT))

#define LOCALE_INVARIANT                                                      \
          (MAKELCID(MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL), SORT_DEFAULT))

#endif /* SOOS_LINUX */

#ifdef SOFLAG_NO_ATL

extern void GetSystemTimeAsFileTime(FILETIME* pFiletime);

#define USES_CONVERSION
#define T2A(text)     (LPSTR)text
#define T2CA(text)    (LPCSTR)text
#define A2T(text)     (LPTSTR)text
#define A2CT(text)    (LPCTSTR)text
#define OLE2CT(text)  text
#define OLE2T(text)   text
#define T2OLE(text)   text
#define T2COLE(text)  text

#define W2CT(text)    text
#define TC2W(text)    text
#define T2CW(text)    text
#define W2T(text)     text

#endif

#endif /* _SOCMNOSCOMAT_ */
