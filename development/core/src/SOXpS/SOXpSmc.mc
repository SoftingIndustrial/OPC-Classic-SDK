;//-----------------------------------------------------------------------------
;//                                                                            |
;//                               Softing AG                                   |
;//                        Richard-Reitzner-Allee 6                            |
;//                           85540 Haar, Germany                              |
;//                                                                            |
;//                 This is a part of the Softing OPC Toolkit                  |
;//        Copyright (c) 1998-2020 Softing Industrial Automation GmbH          |
;//                           All Rights Reserved                              |
;//                                                                            |
;//-----------------------------------------------------------------------------
;//-----------------------------------------------------------------------------
;//                           OPC Toolkit - SOXpS                              |
;//                                                                            |
;//  Filename    : SOXpSmc.mc                                                  |
;//  Version     : 4.47.1                                                      |
;//  Date        : 27-July-2020                                                |
;//                                                                            |
;//  Description : XPress Tool error texts                                     |
;//                                                                            |
;//-----------------------------------------------------------------------------

;#ifndef __SOXPSMC_H
;#define __SOXPSMC_H
;

MessageIdTypedef=HRESULT
SeverityNames=(
               Success=0x0
               Informational=0x1
               Warning=0x2
               Error=0x3
              )
FacilityNames=(
               XPS=0x0;
              )
LanguageNames=(
              Neutral=0x0:SOXpS0
              )
OutputBase=16

;// XPS

MessageID=0x0000
Severity=Success
Facility=XPS
SymbolicName=SOXPS_ERROR_NO_DLL
Language=Neutral
%1: 
Can't load the SOXpSCfg.DLL or the %2
.

MessageID=
SymbolicName=SOXPS_ERROR_NO_INITIALIZE
Language=Neutral
%1: 
The DLL exports no function SOXpSInitialize
.

MessageID=
SymbolicName=SOXPS_ERROR_REQUIRED_AE_FUNCTIONS
Language=Neutral
%1: 
The DLL exports not all required OPC AE functions
.

MessageID=
SymbolicName=SOXPS_ERROR_REQUIRED_DA_FUNCTIONS
Language=Neutral
%1: 
The DLL exports not all required OPC DA functions
.

MessageID=
SymbolicName=SOXPS_ERROR_WRONG_XPS_VERSION
Language=Neutral
%1: 
The DLL is build for an older XPress version
.

;#endif
