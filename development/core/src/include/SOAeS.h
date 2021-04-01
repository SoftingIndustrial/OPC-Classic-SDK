#ifndef _SOAES_H_
#define _SOAES_H_

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
#ifndef SOAES
#ifndef SOOS_WINDOWS_CE
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOAeSUDD.lib")
#else
#pragma comment(lib, "SOAeSADD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOAeSUSD.lib")
#else
#pragma comment(lib, "SOAeSASD.lib")
#endif
#endif
#else
#if defined(_DLL)
#if defined(_UNICODE)
#pragma comment(lib, "SOAeSUD.lib")
#else
#pragma comment(lib, "SOAeSAD.lib")
#endif
#else
#if defined(_UNICODE)
#pragma comment(lib, "SOAeSUS.lib")
#else
#pragma comment(lib, "SOAeSAS.lib")
#endif
#endif
#endif
#else
#if !defined(_UNICODE)
#pragma comment(lib, "SOAeSASO.lib")
#endif
#endif
#else
#if !defined(SOFLAG_DEMO_VERSION)
#if defined(_DEBUG)
#pragma comment(lib, "SOAeSD.lib")
#else
#pragma comment(lib, "SOAeS.lib")
#endif
#else
#pragma comment(lib, "SOAeSO.lib")
#endif
#endif
#endif
#endif


// all SOCmn, SOSrv and SOAeS headers

#ifndef SOFLAG_NO_FORCE_HEADERS
#include <SOSrv.h>
#include <SOAeSEvent.h>
#include <SOAeSEntry.h>
#include <SOAeSServer.h>
#include "SOAeSSubscription.h"
#include "SOAeSCategory.h"
#include "SOAeSAttribute.h"
#include "SOAeSCondition.h"
#include "SOAeSSubCondition.h"
#include "SOAeSArea.h"
#include "SOAeSSource.h"
#include "SOAeSAreaBrowser.h"
#include "SOAeSWatch.h"
#endif

#endif
