#define FILE_DESCRIPTION  "OPC Classic Toolkit C++ API"
#define PRODUCT_NAME      "Softing OPC Classic Toolkit V4.47"

#ifdef WIN64
#ifdef _DEBUG
#ifdef UNICODE
#define ORIGINAL_FILENAME "TBCVS20xxuDx64.dll"
#else
#define ORIGINAL_FILENAME "TBCVS20xxDx64.dll"
#endif //   UNICODE
#else
#ifdef UNICODE
#define ORIGINAL_FILENAME "TBCVS20xxux64.dll"
#else
#define ORIGINAL_FILENAME "TBCVS20xxx64.dll"
#endif  //  UNICODE
#endif  //  _DEBUG
#else
#ifdef _DEBUG
#ifdef UNICODE
#define ORIGINAL_FILENAME "TBCVS20xxuD.dll"
#else
#define ORIGINAL_FILENAME "TBCVS20xxD.dll"
#endif //   UNICODE
#else
#ifdef UNICODE
#define ORIGINAL_FILENAME "TBCVS20xxu.dll"
#else
#define ORIGINAL_FILENAME "TBCVS20xx.dll"
#endif  //  UNICODE
#endif  //  _DEBUG
#endif
#define INTERNAL_NAME     "TBC"
