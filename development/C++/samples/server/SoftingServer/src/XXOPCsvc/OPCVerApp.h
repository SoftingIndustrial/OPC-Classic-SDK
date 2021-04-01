#ifndef _OPCVERAPP_H_
#define _OPCVERAPP_H_

/* Application/component specific version settings
 * ----------------------------------------------------------------------------
 */
// Application name
#define APP_INTERNALNAME        "XXOPCsrv"
#define APP_FILEDESCRIPTION     PROD_PRODUCTNAME " - " APP_INTERNALNAME
#define APP_FILETYPE            VFT_APP

// Trade marks
#undef APP_TRADEMARKS


// Not to be changed!
#if APP_FILETYPE == VFT_APP
#define APP_ORIGINALFILENAME    APP_INTERNALNAME ".exe"
#elif APP_FILETYPE == VFT_DLL
#define APP_ORIGINALFILENAME    APP_INTERNALNAME ".dll"
#else
#define APP_ORIGINALFILENAME    APP_INTERNALNAME
#endif

#define APP_VER_MAJOR            PROD_VER_MAJOR
#define APP_VER_MINOR            PROD_VER_MINOR
#define APP_VER_SERVP            PROD_VER_SERVP
#define APP_VER_BUILD            PROD_VER_BUILD

#define APP_VERSION_STR          PROD_VERSION_STR

#endif  // _OPCVERAPP_H_

/* ------------------------------------------------------------------------------------------------
 * $URL: http://srviad.dhcp.softing.com/svn/OPC/Products/S7S5_OPCServer/trunk/src/S7OPCsrv/OPCVerApp.h $
 * ------------------------------------------------------------------------------------------------
 */
