/* $Id: SLMbase.h 2391 2011-02-28 11:15:01Z poc $
 * ----------------------------------------------------------------------------
 *
 * PROJECT:         License Manager
 *
 * COMPONENT:       SLMbase
 *
 * REVISION:        $Revision: 2391 $   $Author: poc $
 *                  $Date: 2011-02-28 13:15:01 +0200 (Mon, 28 Feb 2011) $
 *
 * ----------------------------------------------------------------------------
 *  (c) 2005 - 2020 Softing Industrial Automation GmbH - All rights reserved.
 * ----------------------------------------------------------------------------
 */

#ifndef _SLMBASE_H_
#define _SLMBASE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


	/* ----  Status Codes:   --------------------------------------------------- */

	/* LM-X */
	/* Success */
#define SLM_STATUS_SUCCESS 0
	/* Unable to use network license as a local license */
#define SLM_STATUS_VENDOR_DENY 34
	/* Unable to save file */
#define SLM_STATUS_FILE_SAVE_ERROR 22
	/* Unexpected network-related error occurred */
#define SLM_STATUS_NETWORK_ERROR 14
	/* Infinite token loop detected */
#define SLM_STATUS_TOKEN_LOOP 32
	/* Software expired */
#define SLM_STATUS_TOO_LATE_DATE 12
	/* Software not allowed to run on a virtual machine */
#define SLM_STATUS_VIRTUAL_DENY 20
	/* Borrow functionality not enabled on client side */
#define SLM_STATUS_BORROW_NOT_ENABLED 26
	/* Request exceeds the number of softlimit licenses available */
#define SLM_STATUS_SOFTLIMIT 29
	/* No more available memory */
#define SLM_STATUS_NO_MEMORY 5
	/* Rejected from license server */
#define SLM_STATUS_NETWORK_DENY 16
	/* Feature is blacklisted */
#define SLM_STATUS_BLACKLIST 33
	/* System clock has been set back */
#define SLM_STATUS_BAD_SYSTEMCLOCK 18
	/* Dongle is not attached or does not function correctly */
#define SLM_STATUS_DONGLE_ERROR 28
	/* Unable to return borrowed feature */
#define SLM_STATUS_BORROW_RETURN_ERROR 24
	/* Internal */
#define SLM_STATUS_QUEUE_ALREADY 129
	/* License server is not currently in use */
#define SLM_STATUS_SERVER_NOT_IN_USE 37
	/* Internal */
#define SLM_STATUS_QUEUE_INSERT 128
	/* Functionality not implemented */
#define SLM_STATUS_NOT_IMPLEMENTED 40
	/* Unable to initialize network subsystem */
#define SLM_STATUS_NO_NETWORK 3
	/* Feature already borrowed */
#define SLM_STATUS_ALREADY_BORROWED 23
	/* Software not allowed to run on terminal server clients */
#define SLM_STATUS_TS_DENY 19
	/* Unable to read file */
#define SLM_STATUS_FILE_READ_ERROR 6
	/* License server returned borrow error */
#define SLM_STATUS_SERVER_BORROW_ERROR 25
	/* Unable to use license server */
#define SLM_STATUS_NO_NETWORK_HOST 15
	/* The specified borrow period is too long */
#define SLM_STATUS_BORROW_TOO_LONG 21
	/* Software version does not match license */
#define SLM_STATUS_BAD_VERSION 13
	/* Hostid does not match license */
#define SLM_STATUS_BAD_HOSTID 10
	/* Software activation date is not yet reached */
#define SLM_STATUS_TOO_EARLY_DATE 11
	/* Unknown error occurred */
#define SLM_STATUS_UNKNOWN_ERROR 1
	/* Invalid input parameter */
#define SLM_STATUS_INVALID_PARAMETER 2
	/* Unable to use network license as a local license */
#define SLM_STATUS_NOT_NETWORK_FEATURE 35
	/* Number of reset system clock attempts exceeded */
#define SLM_STATUS_RESET_SYSTEMCLOCK_EXCEEDED 31
	/* Request for more licenses than available on license server */
#define SLM_STATUS_NOT_ENOUGH_LICENSES 17
	/* Invalid license key */
#define SLM_STATUS_BAD_KEY 8
	/* Platform not permitted by license */
#define SLM_STATUS_BAD_PLATFORM 30
	/* User do not have superuser privilege */
#define SLM_STATUS_USER_NOT_ROOT 39
	/* Problem with License Distribution Service */
#define SLM_STATUS_LICSERVICE_ERROR 38
	/* Feature not found */
#define SLM_STATUS_FEATURE_NOT_FOUND 9
	/* Invalid date */
#define SLM_STATUS_BAD_DATE 7
	/* Internal */
#define SLM_STATUS_LIMIT 131
	/* Internal */
#define SLM_STATUS_RESERVE 130
	/* Checkout is not permitted in the client time zone */
#define SLM_STATUS_BAD_TIMEZONE 36
	/* License file is using unknown/invalid syntax */
#define SLM_STATUS_BAD_LICFILE  4
	/* Feature was not borrowed */
#define SLM_STATUS_NOT_BORROWED 27
	/* License server limitation are exceeded */
#define SLM_STATUS_BORROW_LIMIT_EXCEEDED 41
	/* License server function error occurred */
#define SLM_STATUS_SERVER_FUNC_ERROR 42
	/* License is lost due to heartbeat failure */
#define SLM_STATUS_HEARTBEAT_LOST_LICENSE 43


	/* Own */
	/* Base library not initalized SLMbaseInit not called */
#define SLM_STATUS_NOT_INITIALIZED -100
	/* Maximum number of used features reached */
#define SLM_STATUS_MAX_USED_FEATURES -101
	/* Can't create license thread */
#define SLM_STATUS_NO_THREAD -103
	/* Softing License Manager V3 is not available */
#define SLM_STATUS_NO_SLMV3 -104
	/* Softing PB Interface Licence DLL is not available */
#define SLM_STATUS_NO_PBIFLIC -105
	/* Can not write license file */
#define SLM_STATUS_ERROR_WRITE_FILE -106
	/* Can not read license file */
#define SLM_STATUS_ERROR_READ_FILE -107
	/* Can not set license path environment variable */
#define SLM_STATUS_ERROR_SET_LICPATH_ENV -108
	/* Can not get license directory */
#define SLM_STATUS_ERROR_GET_LICDIR -109
	/* Softing PB Interface with old boot block which does not support licensing */
#define SLM_STATUS_PBIFLIC_OLDBOOTBLOCK -110
	/* Softing PB Interface error writing the license to the board */
#define SLM_STATUS_PBIFLIC_ERROR_SET_LICENSE -111
	/* Softing BC Interface Licence DLL is not available */
#define SLM_STATUS_NO_BCIFLIC -112
	/* Softing BC Interface with old boot block which does not support licensing */
#define SLM_STATUS_BCIFLIC_OLDBOOTBLOCK -113
	/* Softing BC Interface error writing the license to the board */
#define SLM_STATUS_BCIFLIC_ERROR_SET_LICENSE -114


	/* ----  Defines:    --------------------------------------------------------- */

	/* Maximum length of names (e.g. feature name) */
#define SLM_MAX_NAME_LENGTH 64
	/* Maximum field length (e.g. options)  */
#define SLM_MAX_FIELD_LENGTH 8196

	/* Product number for PB Interface licenses 0x8000<Application License Byte><Application License Bit> */
#define SLM_IFLIC_PB_IF  0x80000000

	/* License types */
	/*  Not licensed */
#define SLM_LICTYPE_NO_LICENSE   0x00
	/*  LM-X licensed */
#define SLM_LICTYPE_LMX          0x0F
	/*       Normal local license */
#define SLM_LICTYPE_LMX_LOCAL    0x01
	/*       Networked license */
#define SLM_LICTYPE_LMX_NETWORK  0x02
	/*       Borrowed local license */
#define SLM_LICTYPE_LMX_BORROW   0x03
	/*       Grace local license */
#define SLM_LICTYPE_LMX_GRACE    0x04
	/*       Trial local license */
#define SLM_LICTYPE_LMX_TRIAL    0x05
	/*       Feature licensed with different options */
#define SLM_LICTYPE_LMX_DIFFERENT_OPTIONS 0x08
	/*  SLMV3 licensed */
#define SLM_LICTYPE_SLMV3        0xC0
	/*       Soft license */
#define SLM_LICTYPE_SLMV3_SOFT   0x40
	/*       Hardlock Dongle license */
#define SLM_LICTYPE_SLMV3_DONGLE 0x80
	/*       PB Interface license */
#define SLM_LICTYPE_SLMV3_IF     0xC0
	/*  PBLicence licensed */
#define SLM_LICTYPE_IF_PB        0x20

#define SLM_RECHECKPERIOD_INFINITE 0xFFFFu

	/* ----  Types:  ----------------------------------------------------------- */

	/* Status Code */
	typedef int SLM_STATUS;


	/* ----  Callback functions:     --------------------------------------------- */
	typedef void (*SLMbaseFeatureLost)(const char* featureName);
	typedef void (*SLMbaseFeatureRegained)(const char* featureName, const char* options);


	/* ----  Functions:  ------------------------------------------------------- */

	/* Node locked and floating licenses */
	SLM_STATUS SLMbaseInit(unsigned short recheckPeriod, SLMbaseFeatureLost pSLMbaseFeatureLost, SLMbaseFeatureRegained pSLMbaseFeatureRegained);
	void SLMbaseExit(void);
	SLM_STATUS SLMbaseCheckout(const char* featureName, int majorVersion, int minorVersion, int trialPeriod, char* options);
	SLM_STATUS SLMbaseCheckoutAdvanced(const char* featureName, int majorVersion, int minorVersion, unsigned long iterfaceLicNumber, long productNumber, int trialPeriod, char* options);
	void SLMbaseCheckin(const char* featureName);
	const char* SLMbaseGetStatusString(SLM_STATUS status);
	const char* SLMbaseGetLicensePath(void);
	SLM_STATUS SLMbaseSetLicensePath(const char* path);
	SLM_STATUS SLMbaseRefresh(void);

	/* Design time licenses */
	SLM_STATUS SLMbaseCheckDesigntimeLicense(short productNumber, char* licenseKey, unsigned short* pOptionBits);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // _SLMBASE_H_

/* ------------------------------------------------------------------------------------------------
 * $URL: http://srviad.dhcp.softing.com/svn/OPCToolboxV42/branches/V4-22-xx/dev/core/src/OTB/private/SLMbase.h $
 * ------------------------------------------------------------------------------------------------
 */
