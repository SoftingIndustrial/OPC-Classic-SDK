#ifndef _SOASS_H_
#define _SOASS_H_

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

#ifndef SOFLAG_NO_FORCE_LIBS
#ifndef SOASS
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOAsSUDD.lib")
#else
#pragma comment(lib, "SOAsSADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOAsSUSD.lib")
#else
#pragma comment(lib, "SOAsSASD.lib")
#endif
#endif
#else
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOAsSUD.lib")
#else
#pragma comment(lib, "SOAsSAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOAsSUS.lib")
#else
#pragma comment(lib, "SOAsSAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SOAsSASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SOAsSD.lib")
#else
#pragma comment(lib, "SOAsS.lib")
#endif
#else
#pragma comment(lib, "SOAsSO.lib")
#endif
#endif
#endif
#endif


// all SOCmn, SOSrv, SOAeS, SODaS and SOAsS headers

#ifndef SOFLAG_NO_FORCE_HEADERS
#include <SOAeS.h>
#include <SODaS.h>
#include <SOAsSEntry.h>
#include <SOAsSAttribute.h>
#include <SOAsSCondition.h>
#include <SOAsSItemTag.h>
#endif

#endif
