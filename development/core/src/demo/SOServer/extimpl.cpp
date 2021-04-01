//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : Extimpl.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : External implementations                                    |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <opccomn.h>
#include <opcda.h>
#include <opc_ae.h>
#include <SOProxy.h>
EXTERN_C const IID IID_OPCEventServerCATID; // opc_ae_i.c

// ATL code
#if _MSC_VER < 1300
#include <atlimpl.cpp>
#include <statreg.cpp>
#endif

// OPC IIDs
#include "initguid.h"
#define __IID_DEFINED__
#define __GUID_DEFINED__
#define CLSID_DEFINED
#include "opccomn_i.c"
#include "opcda_i.c"
#include "opc_ae_i.c"
#include "opcSec_i.c"
#include "SOProxy_i.c"




