#ifndef _SOSRV_H_
#define _SOSRV_H_

#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
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
#ifndef SOSRV
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOSrvUDD.lib")
#else
#pragma comment(lib, "SOSrvADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOSrvUSD.lib")
#else
#pragma comment(lib, "SOSrvASD.lib")
#endif
#endif
#else
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOSrvUD.lib")
#else
#pragma comment(lib, "SOSrvAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOSrvUS.lib")
#else
#pragma comment(lib, "SOSrvAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SOSrvASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SOSrvD.lib")
#else
#pragma comment(lib, "SOSrv.lib")
#endif
#else
#pragma comment(lib, "SOSrvO.lib")
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

// SOSrv defines

#define SOSRV_EVENT_SHUTDOWN    1

#define SOSRV_IS_RPC_ERROR(res) ((FAILED(res))&& ((res == 0x800706BA) || (res == 0x800706BE) || (res == 0x800706BF) || (res == 0x80010100)|| (res == 0x80010108) || (res == 0x80010114)))

#ifdef SOOS_WINDOWS

#if _MSC_VER == 1100
#ifndef __RPCNDR_H_VERSION__
#define __RPCNDR_H_VERSION__        ( 440 )
#endif // __RPCNDR_H_VERSION__
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#endif

// all SOCmn and SOSrv headers
#define _ATL_STATIC_REGISTRY

#ifdef SOOS_WINDOWS_CE
#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(IUnknown, __uuidof(IUnknown));
_COM_SMARTPTR_TYPEDEF(IDispatch, __uuidof(IDispatch));
#define _com_issue_errorex(a,b,c) { }
#define _com_dispatch_method(a, b, c, d, e, f)
inline void __stdcall _com_issue_error(HRESULT) { };
#endif

#endif // SOOS_WINDOWS

#ifndef SOFLAG_NO_FORCE_HEADERS
#include <SOCmnCfg.h>

#include <opccomn.h>
#include <opcda.h>
#undef __Constants_MODULE_DEFINED__
#include <opc_ae.h>
#include <opcSec.h>

#include <SOCmn.h>
#include <SOSrvTrace.h>
#include <SOSrvEntry.h>
#include <SOSrvServer.h>
#include <SOSrvBrowser.h>
#include <SOSrvWatch.h>

#ifdef SOFEATURE_SOAP
#include <SOSrvHttp.h>
#endif

#ifdef SOFEATURE_TUNNEL
#include <SOSrvTP.h>
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
