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
//                           OPC Toolkit - OTB                                |
//                                                                            |
//  Filename    : Extimpl.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description :  External implementation (ATL, OPC, ...)                    |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <opccomn.h>
#include <opcda.h>
#include <opc_ae.h>
#include <SOProxy.h>
EXTERN_C const GUID CATID_OPCDAServer10;    // oppcda_cats.c
EXTERN_C const GUID CATID_OPCDAServer20;    // oppcda_cats.c
EXTERN_C const IID IID_OPCEventServerCATID; // opc_ae_i.c


// OPC IIDs
#include "initguid.h"
#define __IID_DEFINED__
#define __GUID_DEFINED__
#define CLSID_DEFINED
#include "opccomn_i.c"
#include "opcda_i.c"
#include "opc_ae_i.c"
#include "opcenum_i.c"
#include "SOProxy_i.c"
#include "opcSec_i.c"

