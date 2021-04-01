#ifndef _SOCLT_H_
#define _SOCLT_H_

#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif

#ifdef UNDER_CE
#ifndef SOOS_WINDOWS_CE
#define SOOS_WINDOWS_CE
#endif
#endif

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

#if defined(WIN32) || defined(WIN64)
// import libraries

#ifndef SOFLAG_NO_FORCE_LIBS
#ifndef SOCLT
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOCltUDD.lib")
#else
#pragma comment(lib, "SOCltADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOCltUSD.lib")
#else
#pragma comment(lib, "SOCltASD.lib")
#endif
#endif
#else
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOCltUD.lib")
#else
#pragma comment(lib, "SOCltAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOCltUS.lib")
#else
#pragma comment(lib, "SOCltAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SOCltASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SOCltD.lib")
#else
#pragma comment(lib, "SOClt.lib")
#endif
#else
#pragma comment(lib, "SOCltO.lib")
#endif
#endif
#endif

// the default SOCmn library implements both
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif

#endif // !SOFLAG_NO_FORCE_LIBS

#endif // WIN32

#if _MSC_VER == 1100
#ifndef __RPCNDR_H_VERSION__
#define __RPCNDR_H_VERSION__        ( 440 )
#endif // __RPCNDR_H_VERSION__
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#endif

#ifndef SOFLAG_NO_FORCE_HEADERS
#include <SOCmnCfg.h>

// all SOCmn and SOClt headers
#ifdef SOOS_WINDOWS
#include <opccomn.h>
#include <opcenum.h>
#include <opcda.h>
#undef __Constants_MODULE_DEFINED__
#include <opc_ae.h>
#include <opcSec.h>

#endif // SOOS_WINDOWS
#include <SOCmn.h>

#include <SOCltElement.h>
#include <SOCltEntry.h>
#include <SOCltStorage.h>
#include <SOCltTask.h>
#include <SOCltThread.h>
#include <SOCltShutdown.h>
#include <SOCltServerBrowser.h>
#include <SOCltServer.h>
#include <SOCltError.h>
#include <SOCltBrowse.h>

#ifdef SOFEATURE_DCOM
#include <SOCltComServer.h>
#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP
#include <SOCltHttp.h>
#endif // SOFEATURE_SOAP

#ifdef SOFEATURE_TUNNEL
#include <SOCltTP.h>
#endif // SOFEATURE_TUNNEL
#endif

#ifdef SOFEATURE_DCOM
#ifndef SOFLAG_SUPPORT_DCOM
#define SOFLAG_SUPPORT_DCOM
#endif
#endif

#ifdef SOFEATURE_SOAP
#ifndef SOFLAG_SUPPORT_SOAP
#define SOFLAG_SUPPORT_SOAP
#endif
#endif

#endif

