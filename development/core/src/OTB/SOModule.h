#ifdef TB_OEM
#include "OemModule.h"
#else

#define FILE_DESCRIPTION  "OPC Classic Toolkit C API"
#define PRODUCT_NAME      "Softing OPC Classic Toolkit V4.47"

#ifdef _DEBUG
#ifdef UNICODE
#ifdef WIN64
#define ORIGINAL_FILENAME "OTBuDx64.dll"
#else // WIN64
#define ORIGINAL_FILENAME "OTBuD.dll"
#endif // WIN64
#else
#ifdef WIN64
#define ORIGINAL_FILENAME "OTBDx64.dll"
#else // WIN64
#define ORIGINAL_FILENAME "OTBD.dll"
#endif // WIN64
#endif //   UNICODE
#else
#ifdef UNICODE
#ifdef WIN64
#define ORIGINAL_FILENAME "OTBux64.dll"
#else // WIN64
#define ORIGINAL_FILENAME "OTBu.dll"
#endif // WIN64
#else
#ifdef WIN64
#define ORIGINAL_FILENAME "OTBx64.dll"
#else // WIN64
#define ORIGINAL_FILENAME "OTB.dll"
#endif // WIN64
#endif  //  UNICODE
#endif  //  _DEBUG

#define INTERNAL_NAME     "OTB"
#endif // TB_OEM
