//-----------------------------------------------------------------------------
// Supported defines:
//
//  OPC Toolkit Source Code Products:
//   OTB_PROD_DA_CLIENT    - OPC Toolkit DA Client Source Code
//   OTB_PROD_AE_CLIENT    - OPC Toolkit AE Client Source Code
//   OTB_PROD_XMLDA_CLIENT - OPC Toolkit XMLDA Client Source Code
//   OTB_PROD_DA_SERVER    - OPC Toolkit DA Server Source Code
//   OTB_PROD_AE_SERVER    - OPC Toolkit AE Server Source Code
//   OTB_PROD_XMLDA_SERVER - OPC Toolkit XMLDA Server Source Code

//-----------------------------------------------------------------------------

//	TODO SourceCode License: Only enable the products you have license code for.
//		Comment the rest of defines
//		E.g.: For an XML-DA Client and Server license key the defines looks like:
//
//		#ifndef OTB_PROD_DA_CLIENT
//		//#define OTB_PROD_DA_CLIENT // user commented because no license key
//		#endif
//		#ifndef OTB_PROD_DA_SERVER
//		//#define OTB_PROD_DA_SERVER // user commented because no license key
//		#endif
//		#ifndef OTB_PROD_XMLDA_SERVER
//		#define OTB_PROD_XMLDA_SERVER // uncommented
//		#endif
//		#ifndef OTB_PROD_XMLDA_CLIENT
//		#define OTB_PROD_XMLDA_CLIENT // uncommented
//		#endif

//	COM-DA and COM-AE for Windows only
#ifdef SOOS_WINDOWS

#ifndef OTB_PROD_DA_CLIENT
#define OTB_PROD_DA_CLIENT
#endif

#ifndef OTB_PROD_DA_SERVER
#define OTB_PROD_DA_SERVER
#endif

#ifndef OTB_PROD_AE_CLIENT
#define OTB_PROD_AE_CLIENT
#endif

#ifndef OTB_PROD_AE_SERVER
#define OTB_PROD_AE_SERVER
#endif

#endif // SOOS_WINDOWS

//	XML-DA works for both Windows and Linux
#ifndef OTB_PROD_XMLDA_SERVER
#define OTB_PROD_XMLDA_SERVER
#endif

#ifndef OTB_PROD_XMLDA_CLIENT
#define OTB_PROD_XMLDA_CLIENT
#endif

// TODO SourceCode License: remove the comment
//#define OTB_USER_COMPILED
