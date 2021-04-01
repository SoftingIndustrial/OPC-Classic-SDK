#include "OSCompat.h"

#ifdef TBC_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>


// TODO: reference additional headers your program requires here
