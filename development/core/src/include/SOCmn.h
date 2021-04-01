#ifndef _SOCMN_H_
#define _SOCMN_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

// disable warings
#ifdef _MSC_VER
#pragma warning( disable : 4250 )   // 'xx' : inherits 'yy' via dominance
//#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients
//#pragma warning( disable : 4275 )
//#pragma warning( disable : 4786 )
//#pragma warning( disable : 4996 ) // this function or variable may be unsafe
//#pragma warning( disable : 4183 )
#elif __BORLANDC__
#pragma warn -8071 // Conversion may loose significant digits.
#pragma warn -8027 // Functions containing some return statements are not expanded inline.
#pragma warn -8026 // Functions taking class-by-value argument(s) are not expanded inline.
#pragma warn -8022 // Functions xxx  hides virtual function yyy.
#pragma warn -8057 // xxx  is never used.
#pragma warn -8074 // Structure passed by value
#pragma warn -8060 // Possibly incorect assignment
#pragma warn -8012 // Comparing signed and unsigned values
#pragma warn -8008 // Condition is always false (for DEMO)
#pragma warn -8080 // declared but never used
#endif

// special application defines
#ifdef SOXPS
#define SOFLAG_NO_DLLS
#define SOFEATURE_COMBINED_SERVER
#endif

#ifdef SOASS
#define SOFEATURE_COMBINED_SERVER
#endif

#ifdef SOFLAG_NO_DLLS
#ifndef SOFLAG_NO_FORCE_LIBS
#define SOFLAG_NO_FORCE_LIBS
#endif
#endif

#ifdef DEMO_VERSION
#define SOFLAG_DEMO_VERSION
#endif

// export or import

#ifndef SOFLAG_NO_DLLS

#ifdef SOCMN
#define SOCMN_EXPORT __declspec(dllexport)
#else
#define SOCMN_EXPORT __declspec(dllimport)
#endif

#ifdef SOSRV
#define SOSRV_EXPORT __declspec(dllexport)
#else
#define SOSRV_EXPORT __declspec(dllimport)
#endif

#ifdef SOCLT
#define SOCLT_EXPORT __declspec(dllexport)
#else
#define SOCLT_EXPORT __declspec(dllimport)
#endif

#ifdef SODAS
#define SODAS_EXPORT __declspec(dllexport)
#else
#define SODAS_EXPORT __declspec(dllimport)
#endif

#ifdef SODAC
#define SODAC_EXPORT __declspec(dllexport)
#else
#define SODAC_EXPORT __declspec(dllimport)
#endif

#ifdef SOAES
#define SOAES_EXPORT __declspec(dllexport)
#else
#define SOAES_EXPORT __declspec(dllimport)
#endif

#ifdef SOAEC
#define SOAEC_EXPORT __declspec(dllexport)
#else
#define SOAEC_EXPORT __declspec(dllimport)
#endif

#ifdef SOASS
#define SOASS_EXPORT __declspec(dllexport)
#else
#define SOASS_EXPORT __declspec(dllimport)
#endif

#else
#define SOCMN_EXPORT
#define SOSRV_EXPORT
#define SOAES_EXPORT
#define SODAS_EXPORT
#define SOCLT_EXPORT
#define SOAEC_EXPORT
#define SODAC_EXPORT
#define SOASS_EXPORT
#endif

// import libraries

#ifndef SOFLAG_NO_FORCE_LIBS
#ifndef SOCMN
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOCmnUDD.lib")
#else
#pragma comment(lib, "SOCmnADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOCmnUSD.lib")
#else
#pragma comment(lib, "SOCmnASD.lib")
#endif
#endif
#else
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOCmnUD.lib")
#else
#pragma comment(lib, "SOCmnAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOCmnUS.lib")
#else
#pragma comment(lib, "SOCmnAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SOCmnASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SOCmnD.lib")
#else
#pragma comment(lib, "SOCmn.lib")
#endif
#else
#pragma comment(lib, "SOCmnO.lib")
#endif
#endif
#endif
#endif

// SOCmn defines

#define SOCMN_EVENT_END         0
#define SOCMN_EVENT_WORK        1

#ifndef SOOS_WINDOWS_CE
#define SOCMN_ALLOCATE_OBJECT(p, creator)   \
        {                                       \
        p = new creator;                    \
        if (p == NULL)                          \
            SOCMN_THROW(SOCmnException(SOCMNEXCEPTION_ID_OUTOFMEMORY, SOCMNEXCEPTION_DESCRIPTION_OUTOFMEMORY)); \
        }
#define SOCMN_ALLOCATE_OBJECT_C(p, creator, cast)   \
        {                                       \
        p = cast new creator;                   \
        if (cast p == NULL)                         \
            SOCMN_THROW(SOCmnException(SOCMNEXCEPTION_ID_OUTOFMEMORY, SOCMNEXCEPTION_DESCRIPTION_OUTOFMEMORY)); \
        }
#define SOCMN_ALLOCATE_MEMORY_C(p, size, cast)  \
        {                                       \
        p = cast malloc(size);                  \
        if (p == NULL)                          \
            SOCMN_THROW(SOCmnException(SOCMNEXCEPTION_ID_OUTOFMEMORY, SOCMNEXCEPTION_DESCRIPTION_OUTOFMEMORY)); \
        }
#else
#define SOCMN_ALLOCATE_OBJECT(p, creator)           p = new creator;
#define SOCMN_ALLOCATE_OBJECT_C(p, creator, cast)   p = cast new creator;
#define SOCMN_ALLOCATE_MEMORY_C(p, size, cast)  p = cast malloc(size);
#endif
#define SOCMN_FREE_OBJECT(p) { if (p) { delete p; p = NULL; } }
#define SOCMN_FREE_OBJECT_C(p, cast) { if (cast p) { delete cast p; p = NULL; } }
#define SOCMN_FREE_OBJECT_THIS { delete this; }
#define SOCMN_FREE_MEMORY(p) { if (p) { free(p); p = NULL; } }


// SOCmn functions

SOCMN_EXPORT void initSOCmn(void);
SOCMN_EXPORT void exitSOCmn(void);

// XML DA errorcodes
#define OPC_E_INVALIDHOLDTIME   0xC0045600L
#define OPC_E_INVALIDITEMNAME   0xC0045601L
#define OPC_E_INVALIDITEMPATH   0xC0045602L
#define OPC_E_NOSUBSCRIPTION    0xC0045603L
#define OPC_E_READONLY          0xC0045604L
#define OPC_E_WRITEONLY         0xC0045605L
#define OPC_E_UNKNOWNITEMNAME   0xC0045606L
#define OPC_E_UNKNOWNITEMPATH   0xC0045607L
#define OPC_E_TIMEDOUT          0xC0045608L
#define OPC_E_SERVERSTATE       0xC0045609L
#define OPC_E_ACCESS_DENIED     0xC0045610L
#define OPC_E_BUSY_XMLDA        0xC0045611L

#define OPC_E_HTTP_STATUS_BASE  0xC0050000L

#define OPC_E_HTTP_DISCONNECT   0xC0051388L
#define OPC_E_HTTP_TIMEOUT      0xC0051389L
#define OPC_E_HTTP_CANTSEND     0xC005138AL
#define OPC_E_HTTP_CANCELED     0xC005138BL

#define SOCMN_TIME_40DAYS 0xCDFE6000L
#define SOCMN_TIME_20DAYS 0x66FF3000L
#define SOCMN_TIME_4DAYS  0x14997000L
#define SOCMN_TIME_1DAY     0x05265C00L

// all SOCmn headers

#ifndef SOFLAG_NO_FORCE_HEADERS
#include <SOCmnString.h>
#include <SOCmnException.h>
#include <SOCmnSync.h>
#ifdef SOFEATURE_CMN_VARIANT
#include <SOCmnVariant.h>
#endif //SOFEATURE_CMN_VARIANT
#include <SOCmnList.h>
#include <SOCmnTrace.h>
#include <SOCmnEventPool.h>
#include <SOCmnData.h>
#include <SOCmnObject.h>
#include <SOCmnFunctions.h>
#ifdef SOFEATURE_CMN_ELEMENT
#include <SOCmnElement.h>
#endif //SOFEATURE_CMN_ELEMENT
#include <SOCmnEvent.h>
#include <SOCmnThread.h>
#ifdef SOFEATURE_CMN_TIME
#include <SOCmnTime.h>
#endif //SOFEATURE_CMN_TIME
#ifdef SOFEATURE_CMN_HTTP
#include <SOCmnHttp.h>
#endif //SOFEATURE_CMN_HTTP
#ifdef SOFEATURE_CMN_SOCKET
#include <SOCmnSocket.h>
#endif //SOFEATURE_CMN_SOCKET
#ifdef SOFEATURE_CMN_XML
#include <SOCmnXML.h>
#endif //SOFEATURE_CMN_XML
#ifdef SOFEATURE_TUNNEL
#include <SOCmnTP.h>
#endif // SOFEATURE_TUNNEL
#endif //SOFLAG_NO_FORCE_HEADERS

#ifdef SOFEATURE_CMN_SOCKET
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE
#pragma comment(lib, "ws2_32.lib")
#else
#if _WIN32_WCE > 300
#pragma comment(lib, "ws2.lib")
#else
#pragma comment(lib, "winsock.lib")
#endif
#endif //SOOS_WINDOWS_CE
#endif //SOOS_WINDOWS
#endif //SOFEATURE_CMN_SOCKET

#endif //_SOCMN_H_
