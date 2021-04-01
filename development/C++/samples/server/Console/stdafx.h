#include <OSCompat.h>

#ifdef TBC_OS_WINDOWS

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// Windows Header Files:
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
