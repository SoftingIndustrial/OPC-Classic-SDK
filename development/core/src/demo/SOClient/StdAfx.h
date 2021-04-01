#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>         // MFC support for Windows Common Controls

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <process.h>

#include <atlbase.h>

#define SOCLT
#ifdef SODMC_DA
#define SODAC
#endif
#ifdef SODMC_AE
#define SOAEC
#endif

#define SOFLAG_NO_DEFAULT_FEATURES
#define SOFEATURE_DCOM
#define SOFEATURE_SOAP
#ifdef SOFLAG_DEMO_VERSION
#define SOFEATURE_CMN_OPCSERVER
#endif
#define SOFEATURE_CMN_OPCCLIENT

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#define OPCHANDLE DWORD

#include "SOCmn.h"
#include "SOClt.h"
#ifdef SODMC_DA
#include "SODaC.h"
#endif
#ifdef SODMC_AE
#include "SOAeC.h"
#endif

EXTERN_C const GUID CATID_OPCDAServer10;
EXTERN_C const GUID CATID_OPCDAServer20;
EXTERN_C const IID IID_OPCEventServerCATID; // opc_ae_i.c
