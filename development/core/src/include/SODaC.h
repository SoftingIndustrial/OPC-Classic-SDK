#ifndef _SODAC_H_
#define _SODAC_H_

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
#ifndef SODAC
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SODaCUDD.lib")
#else
#pragma comment(lib, "SODaCADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SODaCUSD.lib")
#else
#pragma comment(lib, "SODaCASD.lib")
#endif
#endif
#else
#if defined(_DLL) | defined(_RTLDLL)
#if defined(_UNICODE)
#pragma comment(lib, "SODaCUD.lib")
#else
#pragma comment(lib, "SODaCAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SODaCUS.lib")
#else
#pragma comment(lib, "SODaCAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SODaCASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SODaCD.lib")
#else
#pragma comment(lib, "SODaC.lib")
#endif
#else
#pragma comment(lib, "SODaCO.lib")
#endif
#endif
#endif
#endif
#endif

#define SODAC_MAXAGE_CACHE  0xFFFFFFFF
#define SODAC_MAXAGE_DEVICE 0

#define SODAC_QUALITY_NONE 0xFFFF

// all SOCmn, SOClt and SODaC headers

#ifndef SOFLAG_NO_FORCE_HEADERS

#include <SOClt.h>
#include <SODaCCallback.h>
#include <SODaCEntry.h>
#include <SODaCGroup.h>
#include <SODaCItem.h>
#include <SODaCServer.h>
#include <SODaCBrowse.h>
#ifdef SOFEATURE_DCOM
#include <SODaCComServer.h>
#include <SODaCComGroup.h>
#include <SODaCComBrowse.h>
#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_SOAP
#include <SODaCXmlServer.h>
#include <SODaCXmlGroup.h>
#include <SODaCXmlBrowse.h>
#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_TUNNEL
#include <SODaCTPServer.h>
#include <SODaCTPGroup.h>
#include <SODaCTPBrowse.h>
#endif // SOFEATURE_TUNNEL

#endif

#endif
