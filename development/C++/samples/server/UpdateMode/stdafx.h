//	reference to the Toolkit Operating system speciffics
#include <OSCompat.h>


#ifdef TBC_OS_WINDOWS

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#endif

#ifdef TBC_OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "PosixEvents.h"
#endif
