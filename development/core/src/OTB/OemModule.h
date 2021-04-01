#ifdef TB_OEM_ABB
#define FILE_DESCRIPTION  "OPC Classic Toolkit C API - OEM ABB"
#define SPECIAL_BUILD "ABB"
#elif TB_OEM_I4T
#define FILE_DESCRIPTION  "OPC Classic Toolkit C API - OEM I4T"
#define SPECIAL_BUILD "I4T"
#else
#define FILE_DESCRIPTION  "OPC Classic Toolkit C API - Tunnel Enabled"
#define SPECIAL_BUILD "Softing"
#endif

#define PRODUCT_NAME      "Softing OPC Classic Toolkit V4.47"

#ifdef _DEBUG
#ifdef UNICODE
#define ORIGINAL_FILENAME "OTBuD.dll"
#else
#define ORIGINAL_FILENAME "OTBD.dll"
#endif //   UNICODE
#else
#ifdef UNICODE
#define ORIGINAL_FILENAME "OTBu.dll"
#else
#define ORIGINAL_FILENAME "OTB.dll"
#endif  //  UNICODE
#endif  //  _DEBUG

#define INTERNAL_NAME     "OTB"
