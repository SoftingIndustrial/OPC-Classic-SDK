#include "stdafx.h"

#ifdef SOFEATURE_CMN_XML

// eXpat defines
#ifndef SOFLAG_USE_EXPAT_SHARED_LIB
#define XML_STATIC
#ifdef SOOS_WINDOWS
#pragma warning (push, 3)
#include "expat/winconfig.h"
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
#include "expat/linuxconfig.h"
#endif

// eXpat implementation
#include "expat/xmlrole.c"
#include "expat/xmltok.c"
#include "expat/xmlparse.c"
#ifdef SOOS_WINDOWS
#pragma warning (pop)
#endif // SOOS_WINDOWS
#endif // SOFLAG_USE_EXPAT_SHARED_LIB

#endif
