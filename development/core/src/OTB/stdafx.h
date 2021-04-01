#ifndef _STDAFX_H_
#define _STDAFX_H_

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

// set the maximum number of parallel opened sockets. Default is 64
#define FD_SETSIZE 256


#include "OTBCfg.h"


//	Private Softing defines
#ifdef OTB_BUILD_LIB
//   OTB_PROD_TPDA_SERVER - OPC Toolkit TPDA Server Source Code
//   OTB_PROD_TPAE_SERVER - OPC Toolkit TPAE Server Source Code
//   OTB_PROD_TPDA_CLIENT - OPC Toolkit TPDA Client Source Code
//   OTB_PROD_TPAE_CLIENT - OPC Toolkit TPAE Client Source Code

#ifndef OTB_PROD_TPDA_CLIENT
#define OTB_PROD_TPDA_CLIENT
#endif


#ifndef OTB_PROD_TPDA_SERVER
#define OTB_PROD_TPDA_SERVER
#endif

#ifndef OTB_PROD_TPAE_CLIENT
#define OTB_PROD_TPAE_CLIENT
#endif

#ifndef OTB_PROD_TPAE_SERVER
#define OTB_PROD_TPAE_SERVER
#endif

#endif
// Set the defines for the installed OPC Toolkit Souce Code products

#ifdef OTB_PROD_DA_CLIENT
#ifndef SODAC
#define SODAC
#endif // SODAC
#ifndef SOCLT
#define SOCLT
#endif // SOCLT
#ifndef OTB_CLIENT_DA
#define OTB_CLIENT_DA
#endif // OTB_SERVER_DA
#ifndef OTB_CLIENT
#define OTB_CLIENT
#endif // OTB_CLIENT
#endif // OTB_PROD_DA_CLIENT

#ifdef OTB_PROD_AE_CLIENT
#ifndef SOAEC
#define SOAEC
#endif // SOAEC
#ifndef SOCLT
#define SOCLT
#endif // SOCLT
#ifndef OTB_CLIENT_AE
#ifdef SOOS_WINDOWS
#define OTB_CLIENT_AE
#endif  // SOOS_WINDOWS
#endif // OTB_SERVER_AE
#ifndef OTB_CLIENT
#define OTB_CLIENT
#endif // OTB_CLIENT
#endif // OTB_PROD_AE_CLIENT

#ifdef OTB_PROD_XMLDA_CLIENT
#ifndef SODAC
#define SODAC
#endif // SODAC
#ifndef SOCLT
#define SOCLT
#endif // SOCLT
#ifndef OTB_CLIENT_DA
#define OTB_CLIENT_DA
#endif // OTB_CLIENT_DA
#ifndef OTB_CLIENT
#define OTB_CLIENT
#endif // OTB_CLIENT
#ifndef OTB_XMLDA_CLIENT
#define OTB_XMLDA_CLIENT
#endif
#endif // OTB_PROD_XMLDA_CLIENT

#ifdef OTB_PROD_TPDA_CLIENT
#ifndef SODAC
#define SODAC
#endif // SODAC
#ifndef SOCLT
#define SOCLT
#endif // SOCLT
#ifndef OTB_CLIENT_DA
#define OTB_CLIENT_DA
#endif // OTB_CLIENT_DA
#ifndef OTB_CLIENT
#define OTB_CLIENT
#endif // OTB_CLIENT
#ifndef OTB_TPDA_CLIENT
#define OTB_TPDA_CLIENT
#endif
#endif // OTB_PROD_TPDA_CLIENT

#ifdef OTB_PROD_TPAE_CLIENT
#ifndef SOAEC
#define SOAEC
#endif // SOAEC
#ifndef SOCLT
#define SOCLT
#endif // SOCLT
#ifndef OTB_CLIENT_AE
#define OTB_CLIENT_AE
#endif // OTB_CLIENT_AE
#ifndef OTB_CLIENT
#define OTB_CLIENT
#endif // OTB_CLIENT
#ifndef OTB_TPAE_CLIENT
#define OTB_TPAE_CLIENT
#endif
#endif // OTB_PROD_TPAE_CLIENT

#ifdef OTB_PROD_DA_SERVER
#ifndef SODAS
#define SODAS
#endif // SODAS
#ifndef SOSRV
#define SOSRV
#endif // SOSRV
#ifndef OTB_SERVER_DA
#define OTB_SERVER_DA
#endif // OTB_SERVER_DA
#ifndef OTB_SERVER
#define OTB_SERVER
#endif // OTB_SERVER
#endif // OTB_PROD_DA_SERVER

#ifdef OTB_PROD_AE_SERVER
#ifndef SOAES
#define SOAES
#endif // SOAES
#ifndef SOSRV
#define SOSRV
#endif // SOSRV
#ifndef OTB_SERVER_AE
#define OTB_SERVER_AE
#endif // OTB_SERVER_AE
#ifndef OTB_SERVER
#define OTB_SERVER
#endif // OTB_SERVER
#endif // OTB_PROD_AE_SERVER

#ifdef OTB_PROD_XMLDA_SERVER
#ifndef SODAS
#define SODAS
#endif // SODAS
#ifndef SOSRV
#define SOSRV
#endif // SOSRV
#ifndef OTB_SERVER_DA
#define OTB_SERVER_DA
#endif // OTB_SERVER_DA
#ifndef OTB_SERVER
#define OTB_SERVER
#endif // OTB_SERVER
#ifndef OTB_XMLDA_SERVER
#define OTB_XMLDA_SERVER
#endif
#endif // OTB_PROD_XMLDA_SERVER

#ifdef OTB_PROD_TPDA_SERVER
#ifndef SODAS
#define SODAS
#endif // SODAS
#ifndef SOSRV
#define SOSRV
#endif // SOSRV
#ifndef OTB_SERVER_DA
#define OTB_SERVER_DA
#endif // OTB_SERVER_DA
#ifndef OTB_SERVER
#define OTB_SERVER
#endif // OTB_SERVER
#ifndef OTB_TPDA_SERVER
#define OTB_TPDA_SERVER
#endif // OTB_TPDA_SERVER 
#endif // OTB_PROD_TPDA_SERVER

#ifdef OTB_PROD_TPAE_SERVER
#ifndef SOAES
#define SOAES
#endif // SOAES
#ifndef SOSRV
#define SOSRV
#endif // SOSRV
#ifndef OTB_SERVER_AE
#define OTB_SERVER_AE
#endif // OTB_SERVER_AE
#ifndef OTB_SERVER
#define OTB_SERVER
#endif // OTB_SERVER
#ifndef OTB_TPDA_SERVER
#define OTB_TPDA_SERVER
#endif // OTB_TPDA_SERVER
#endif // OTB_PROD_TPDA_SERVER

#if defined(SODAS) && defined(SOAES)
#ifndef SOASS
#define SOASS
#endif // SOASS
#endif // defined(SODAS) && defined(SOAES)

#if defined(SOCLT) && defined(SOSRV)
#ifndef SOFLAG_CLIENT_AND_SERVER
#define SOFLAG_CLIENT_AND_SERVER
#endif // SOFLAG_CLIENT_AND_SERVER
#endif // defined(SODAS) && defined(SOAES)

#if defined(OTB_SERVER_DA) && defined(OTB_SERVER_AE)
#ifndef OTB_SERVER_DAAE
#define OTB_SERVER_DAAE
#endif // OTB_SERVER_DAAE
#endif // defined(OTB_SERVER_DA) && defined(OTB_SERVER_AE)


// SOCmn defines for the OTB

#define SOFLAG_NO_DEFAULT_FEATURES

#ifdef SOCLT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#ifdef SOSRV
#define SOFEATURE_CMN_OPCSERVER
#endif

#ifdef SOOS_WINDOWS
#if defined(OTB_PROD_DA_SERVER) || defined(OTB_PROD_AE_SERVER) || defined(OTB_PROD_DA_CLIENT) || defined(OTB_PROD_AE_CLIENT)
#ifndef SOFLAG_NO_ATL
#define SOFEATURE_DCOM
#endif
#endif
#endif // SOOS_WINDOWS

#if defined(OTB_PROD_XMLDA_CLIENT) || defined(OTB_PROD_XMLDA_SERVER)
#define SOFEATURE_SOAP
#endif

#if defined(OTB_PROD_TPDA_CLIENT) || defined(OTB_PROD_TPDA_SERVER) || defined(OTB_PROD_TPAE_CLIENT) || defined(OTB_PROD_TPAE_SERVER)
#define SOFEATURE_TUNNEL
#endif


// OPC Toolkit includes
#include <SOCmnCfg.h>
#undef SOFEATURE_WATCH
#include <SOCmnOsCompat.h>
#ifdef SODAC
#include <SODaC.h>
#endif // SODAC
#ifdef SOAEC
#include <SOAeC.h>
#endif // SOAEC
#ifdef SODAS
#include <SODaS.h>
#endif // SODAS
#ifdef SOAES
#include <SOAeS.h>
#endif // SOAES
#ifdef SOASS
#include <SOAsS.h>
#endif // SOASS

#include <stdio.h>
#include <stdarg.h>

#ifdef SOOS_WINDOWS
#include <tchar.h>

#ifndef SOFLAG_NO_ATL
// ATL includes
#define _ATL_STATIC_REGISTRY
#include <atlbase.h>
#include <statreg.h>
extern CComModule _Module;
#include <atlcom.h>
#endif // SOFLAG_NO_ATL
#endif // SOOS_WINDOWS

#include "OTClient.h"
#include "OTServer.h"

#ifndef OTB_USER_COMPILED

// Server Demo Mode
// define own demo version checks
extern void updateLicenseInfo(BYTE feature);
extern BOOL isDemoMode(void);
extern BOOL isLicensed(BYTE feature, LONG reserved = 0);
extern BOOL demoElapsed(BYTE feature, LONG reserved = 0);

#define SOFLAG_DEMO_VERSION
#define SOSRV_CALL_CHECK_DEMO_DA      \
    if (demoElapsed(OTS_FEATURE_DA_SERVER))               \
    {                                \
        return E_ACCESSDENIED;         \
    }
#define SOSRV_CALL_CHECK_DEMO_AE      \
    if (demoElapsed(OTS_FEATURE_AE_SERVER))               \
    {                                \
        return E_ACCESSDENIED;         \
    }
#define SOSRV_ADVISE_CHECK_DEMO_DA  (!demoElapsed(OTS_FEATURE_DA_SERVER))
#define SOSRV_ADVISE_CHECK_DEMO_AE  (!demoElapsed(OTS_FEATURE_AE_SERVER))

#define SOSRV_HTTP_CHECK_DEMO(a)                                                \
    if (demoElapsed(OTS_FEATURE_XMLDA_SERVER))               \
    {                                                                       \
        a =  SOCMNHTTP_CODE_FORBIDDEN;                                              \
        sndHTTPErrorResponse(srvData, srvData->m_flags, a); \
    } \
    else \
    { \
        a = SOCMNHTTP_CODEI_NOT_HANDLED; \
    }

#undef SOSRV_TP_CHECK_DEMO
#define SOSRV_TP_CHECK_DEMO                 \
    if (demoElapsed(OTS_FEATURE_TP_SERVER)) \
    {                                       \
        return E_ACCESSDENIED;              \
    }

#undef SOSRV_TP_ADVISE_CHECK_DEMO
#define SOSRV_TP_ADVISE_CHECK_DEMO  (!demoElapsed(OTS_FEATURE_TP_SERVER))


#undef SOCLT_COM_CHECK_DEMO
#define SOCLT_COM_CHECK_DEMO demoElapsed(OTC_FEATURE_DA_CLIENT)
#undef SOCLT_COM_CHECK_DEMO_AE
#define SOCLT_COM_CHECK_DEMO_AE demoElapsed(OTC_FEATURE_AE_CLIENT)
#undef SOCLT_XML_CHECK_DEMO
#define SOCLT_XML_CHECK_DEMO demoElapsed(OTC_FEATURE_XMLDA_CLIENT)
#undef SOCLT_TP_CHECK_DEMO
#define SOCLT_TP_CHECK_DEMO demoElapsed(OTC_FEATURE_TP_CLIENT)

#else // OTB_USER_COMPILED

#define isDemoMode() TRUE
#define isLicensed(a) TRUE

#endif  // OTB_USER_COMPILED

#if defined(WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#endif
