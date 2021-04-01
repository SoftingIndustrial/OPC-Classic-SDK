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
//                           OPC Toolkit - SOXpS                              |
//                                                                            |
//  Filename    : Extimpl.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : External implementations                                    |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "SOProxy.h"

// ATL code
#include <atlimpl.cpp>
#include <statreg.cpp>

// OPC IIDs
#include "initguid.h"
#define __IID_DEFINED__
#define __GUID_DEFINED__
#define CLSID_DEFINED
#include "opccomn_i.c"
#include "opcda_i.c"
#include "opc_ae_i.c"
#include "SOProxy_i.c"

