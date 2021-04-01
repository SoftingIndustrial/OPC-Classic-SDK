#ifndef _OPCVERPROD_H_
#define _OPCVERPROD_H_

/* Global version settings for the whole product
 * ----------------------------------------------------------------------------
 */
// Product definition
#define PROD_NAME               "XX OPC Server"
#define PROD_COMPANY        "Softing Industrial Automation GmbH"

// Version numbers
#define PROD_VER_MAJOR          1                               // Main version
#define PROD_VER_MINOR          00                          // Upgrade version
#define PROD_VER_SERVP          00                          // Service pack version
#define PROD_VER_BUILD          PRODUCT_BUILD       // Build number

// Copyright
#define PROD_COPYR              "2008"


// Not to be changed!
#define __MK_STR(x)             #x
#define _MK_STR(x)              __MK_STR(x)

#define PROD_SHORT_VERSION_STR  "V" _MK_STR(PROD_VER_MAJOR.PROD_VER_MINOR)
#define PROD_PRODUCTNAME        PROD_NAME " " PROD_SHORT_VERSION_STR
#define PROD_VERSION_STR        _MK_STR(PROD_VER_MAJOR.PROD_VER_MINOR.PROD_VER_SERVP.PROD_VER_BUILD)
#define PROD_FULL_VERSION_STR   PROD_SHORT_VERSION_STR " " _MK_STR((PROD_VER_MAJOR.PROD_VER_MINOR.PROD_VER_SERVP.PROD_VER_BUILD))

#define PROD_COMPANYNAME        PROD_COMPANY
#define PROD_COPYRIGHT          "\251 " PROD_COPYR " " PROD_COMPANY ".\r\nAll rights reserved."
#define PROD_COPYRIGHT_SHORT    "Copyright \251 " PROD_COPYR " " PROD_COMPANY "."

#define BUILD_NUMBER            PRODUCT_BUILD
#define MAJOR_VERSION_NUMBER    PROD_VER_MAJOR
#define MINOR_VERSION_NUMBER    PROD_VER_MINOR
#define PATCH_VERSION_NUMBER    PROD_VER_SERVP

#endif  // _OPCVERPROD_H_

/* ------------------------------------------------------------------------------------------------
 * $URL: http://srviad.dhcp.softing.com/svn/OPC/Products/S7S5_OPCServer/trunk/src/res/OPCVerProd.h $
 * ------------------------------------------------------------------------------------------------
 */
