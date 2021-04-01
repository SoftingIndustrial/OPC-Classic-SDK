#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#define SOSRV
#define SODAS
#define SOAES
#define SOASS

// ATL includes
#define _ATL_STATIC_REGISTRY
#include <atlbase.h>
#include <statreg.h>
extern CComModule _Module;
#include <atlcom.h>

#define SOFLAG_NO_DEFAULT_FEATURES
#define SOFEATURE_DCOM
#define SOFEATURE_CMN_OPCSERVER

#include <SOCmnCfg.h>
#include <SOCmnOsCompat.h>

#include <SOAsS.h>
#include <SOXpS.h>

extern SOXpSInitData *g_initData;
