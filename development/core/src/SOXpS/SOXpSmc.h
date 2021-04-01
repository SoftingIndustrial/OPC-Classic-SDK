#ifndef __SOXPSMC_H
#define __SOXPSMC_H

// XPS
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: SOXPS_ERROR_NO_DLL
//
// MessageText:
//
//  %1: 
//  Can't load the SOXpSCfg.DLL or the %2
//
#define SOXPS_ERROR_NO_DLL               ((HRESULT)0x00000000L)

//
// MessageId: SOXPS_ERROR_NO_INITIALIZE
//
// MessageText:
//
//  %1: 
//  The DLL exports no function SOXpSInitialize
//
#define SOXPS_ERROR_NO_INITIALIZE        ((HRESULT)0x00000001L)

//
// MessageId: SOXPS_ERROR_REQUIRED_AE_FUNCTIONS
//
// MessageText:
//
//  %1: 
//  The DLL exports not all required OPC AE functions
//
#define SOXPS_ERROR_REQUIRED_AE_FUNCTIONS ((HRESULT)0x00000002L)

//
// MessageId: SOXPS_ERROR_REQUIRED_DA_FUNCTIONS
//
// MessageText:
//
//  %1: 
//  The DLL exports not all required OPC DA functions
//
#define SOXPS_ERROR_REQUIRED_DA_FUNCTIONS ((HRESULT)0x00000003L)

//
// MessageId: SOXPS_ERROR_WRONG_XPS_VERSION
//
// MessageText:
//
//  %1: 
//  The DLL is build for an older XPress version
//
#define SOXPS_ERROR_WRONG_XPS_VERSION    ((HRESULT)0x00000004L)

#endif
