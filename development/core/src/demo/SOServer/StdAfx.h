#define SOSRV
#ifdef SODMS_DA
#define SODAS
#endif
#ifdef SODMS_AE
#define SOAES
#endif
#ifdef SODMS_ALL
#define SOASS
#endif

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"
#include "SOSAllocHelpers.h"

#ifdef SOFEATURE_DCOM
#include <statreg.h>
extern CComModule _Module;
#include <atlcom.h>
#endif

#ifdef SODMS_ALL
#include <SOAsS.h>
#else
#ifdef SODMS_DA
#include <SODaS.h>
#else
#include <SOAeS.h>
#endif
#endif

