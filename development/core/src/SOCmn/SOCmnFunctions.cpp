#include "stdafx.h"
#include "SOCmn.h"
#include "SOCmnFunctions.h"
#include <limits.h>
#include <math.h>

//-----------------------------------------------------------------------------
// allocateMemory
//
// - allocate memory block
// This method is only needed if using static libraries, because then 2 different
// C-Runtime libraries are used by the toolkit and the application !!!
//
void* allocateMemory(
	IN ULONG size)  // memory size
{
	return malloc(size);
} // allocateMemory


//-----------------------------------------------------------------------------
// freeMemory
//
// - free memory block
// This method is only needed if using static libraries, because then 2 different
// C-Runtime libraries are used by the toolkit and the application !!!
//
void freeMemory(
	IN  void* memptr)   // memory to free
{
	free(memptr);
} // freeMemory




//-----------------------------------------------------------------------------
// reallocateMemory
//
// - free memory block
// This method is only needed if using static libraries, because then 2 different
// C-Runtime libraries are used by the toolkit and the application !!!
//
void* reallocateMemory(
	IN   void* memptr,
	IN ULONG size)
{
	return realloc(memptr, size);
} // reallocateMemory


//-----------------------------------------------------------------------------
// getTimeSpan
//
// - calculate span between to DWORD times
//
// returns:
//		time span
//
ULONG getTimeSpan(
	IN DWORD from,  // start of span
	IN DWORD till)  // end of span
{
	ULONG span = till - from;

	if (span > 4294900000u)
	{
		span = 0;
	}

	return span;
} // getTimeSpan


#ifdef SOOS_WINDOWS
//-----------------------------------------------------------------------------
// getTimeSpan
//
// - calculate span between to __int64 times
//
// returns:
//		time span
//
__int64 getTimeSpan(
	IN __int64 from,    // start of span
	IN __int64 till)    // end of span
{
	__int64 span = till - from;
	return span;
} // getTimeSpan


//-----------------------------------------------------------------------------
// getTimeSpan
//
// - calculate span between to __int64 times
//
// returns:
//		time span
//
__int64 getTimeSpan10Micro(
	IN __int64 from,    // start of span
	IN __int64 till,  // end of span
	IN __int64 frequency)
{
	__int64 span = getTimeSpan(from, till);
	span = (span * 100000) / frequency;
	return span;
} // getTimeSpan

#endif // SOOS_WINDOWS

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// enumString2stringList
//
// - copy the entries of an IEnumString object into a MFC string list
//
void enumString2stringList(
	IN IEnumString* enumString,     // interface pointer of IEnumString object
	OUT SOCmnStringList* stringList)    // toolkit string list
{
	USES_CONVERSION;

	if ((!enumString) || (!stringList))
	{
		return;
	}

	enumString->Reset();
	LPOLESTR wzBuffer[100];
	DWORD cnt = 0;

	while (SUCCEEDED(enumString->Next(_COUNTOF(wzBuffer), wzBuffer, &cnt)))
	{
		for (DWORD i = 0 ; i < cnt; i++)
		{
			SOCmnString str;
			str.fromWCHAR(wzBuffer[i]);
			stringList->add(str);
			::CoTaskMemFree(wzBuffer[i]);
		}

		if (0 == cnt)
		{
			break;
		}
	}
} // enumString2stringList
#endif // SOFEATURE_DCOM

#ifdef SOOS_LINUX
DWORD GetTickCount(void)
{
	static DWORD ms_per_tick;
	struct tms stms;
	clock_t ticks = times(&stms);

	if (!ms_per_tick)
	{
		ms_per_tick = 1000 / sysconf(_SC_CLK_TCK);
	}

	/* linux: CLK_TCK=100 => ms_per_tick=10 */
	return ticks * ms_per_tick;
}

void Sleep(DWORD timeout)
{
	struct timespec ts;

	do
	{
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = 1000000 * (timeout % 1000);

		while (nanosleep(&ts, &ts) < 0 && errno == EINTR)
			/* nothing */;
	}
	while (timeout == INFINITE);

	return;
}

#ifdef __i386__
static inline int is_386_or_early_486(void)
{
	/* the Intel manual says EFLAGS.ID (BIT 21) is not changeable
	 * in i386 and early i486 processors. If this bit is writable
	 * this means, the CPUID instruction is implemented in this
	 * processor, and thus also the XADD instruction!
	 */
	int retval;
	__asm__("pushfl; pushfl; "
			"popl %%eax; "
			"xorl $1<<21,%%eax; "
			"pushl %%eax; "
			"popfl; "
			"pushl %%edx; "
			"pushfl; "
			"popl %%edx; "
			"xorl %%edx,%%eax; "
			"popl %%edx; "
			"shrl $21,%%eax; "
			"andl $1,%%eax; "
			"movl %%eax, %0; "
			"popfl; "
			: "=r"(retval)
			: /*no inputs*/
			: "eax" /*eax changed*/);
	return retval;
}

static inline LONG pentium_xadd(LONG* lpAddend, LONG value)
{
	/* the xadd instruction is equivalent to the
	 * following two "C" instructions:
	 *
	 * retval = *lpAddend;
	 * *lpAddend += value;
	 *
	 * the lock prefix instruction is only neccessary
	 * because this system might be a SMP
	 */
	LONG retval;
	__asm__ __volatile__("lock; xaddl %0, (%1); "
						 : "=r"(retval)
						 : "r"(lpAddend), "0"(value)
						 : "memory", "cc" /*condition flags changed*/);
	return retval;
}
#endif // __i386__

#ifndef __powerpc__
static pthread_mutex_t g_interlocked_mutex = PTHREAD_MUTEX_INITIALIZER;

LONG interlockedIncrement(LONG* lpAddend)
{
	LONG retval;
#ifdef __i386__

	if (!is_386_or_early_486())
	{
		return pentium_xadd(lpAddend, 1) + 1;
	}

#endif
	pthread_mutex_lock(&g_interlocked_mutex);
	retval = ++(*lpAddend);
	pthread_mutex_unlock(&g_interlocked_mutex);
	return retval;
}

LONG interlockedDecrement(LONG* lpAddend)
{
	LONG retval;
#ifdef __i386__

	if (!is_386_or_early_486())
	{
		return pentium_xadd(lpAddend, -1) - 1;
	}

#endif
	pthread_mutex_lock(&g_interlocked_mutex);
	retval = --(*lpAddend);
	pthread_mutex_unlock(&g_interlocked_mutex);
	return retval;
}
#endif
#endif // SOOS_LINUX

#ifdef SOOS_WINDOWS
#ifdef SOOS_WINDOWS_95

static inline int is_386_or_early_486(void)
{
	/* the Intel manual says EFLAGS.ID (BIT 21) is not changeable
	 * in i386 and early i486 processors. If this bit is writable
	 * this means, the CPUID instruction is implemented in this
	 * processor, and thus also the XADD instruction!
	 */
	int retval;
	__asm
	{
		pushf
		pushf
		pop   eax
		xor   eax, 1<<21
		push  eax
		popf
		push  edx
		pushf
		pop   edx
		xor   eax, edx
		pop   edx
		shr   eax, 21
		and   eax, 1
		mov   retval, eax
		popf
	}
	return retval;
}

static inline LONG pentium_xadd(LONG* lpAddend, LONG value)
{
	/* the xadd instruction is equivalent to the
	 * following two "C" instructions:
	 *
	 * retval = *lpAddend;
	 * *lpAddend += value;
	 */
	LONG retval;
	__asm
	{
		mov   eax, value
		mov   ebx, lpAddend
		xadd  [ebx], eax
		mov   retval, eax
	}
	return retval;
}

static CRITICAL_SECTION g_interlocked_section;

LONG interlockedIncrement(LONG* lpAddend)
{
	LONG retval;

	if (!is_386_or_early_486())
	{
		return pentium_xadd(lpAddend, 1) + 1;
	}

	EnterCriticalSection(&g_interlocked_section);
	retval = ++(*lpAddend);
	LeaveCriticalSection(&g_interlocked_section);
	return retval;
}

LONG interlockedDecrement(LONG* lpAddend)
{
	LONG retval;

	if (!is_386_or_early_486())
	{
		return pentium_xadd(lpAddend, -1) - 1;
	}

	EnterCriticalSection(&g_interlocked_section);
	retval = --(*lpAddend);
	LeaveCriticalSection(&g_interlocked_section);
	return retval;
}

#endif // SOOS_WINDOWS_95
#endif // SOOS_WINDOWS

void initSOCmn(void)
{
#ifdef SOOS_WINDOWS
#ifdef SOFEATURE_CMN_SOCKET
	WORD versionRequested;
	WSADATA wsaData;
	versionRequested = MAKEWORD(1, 0);
	WSAStartup(versionRequested, &wsaData);
#endif
#ifdef SOOS_WINDOWS_95
	InitializeCriticalSection(&g_interlocked_section);
#endif
	createEventPool();
#endif // SOOS_WINDOWS
	createTraceObject();
	createObjectLock();
}

void exitSOCmn(void)
{
	destroyObjectLock();
#ifdef SOOS_WINDOWS
	destroyEventPool();
#ifdef SOOS_WINDOWS_95
	DeleteCriticalSection(&g_interlocked_section);
#endif
#ifdef SOFEATURE_CMN_SOCKET
	WSACleanup();
#endif
	destroyTraceObject();
#endif // SOOS_WINDOWS
}


/*
  INTERNATIONAL STANDARD ISO/IEC 14882
  Second edition 2003-10-15

  4.9.1
  An rvalue of a floating point type can be converted to an rvalue of an integer type.  The conversion trun-
  cates; that is, the fractional part is discarded.  The behavior is undefined if the truncated value cannot be
  represented in the destination type.  [Note: If the destination type is bool, see 4.12.  ]
  4.9.2
  An rvalue of an integer type or of an enumeration type can be converted to an rvalue of a floating point
  type.  The result is exact if possible.  Otherwise, it is an implementation-defined choice of either the next
  lower or higher representable value.  [Note: loss of precision occurs if the integral value cannot be repre-
  sented exactly as a value of the floating type.  ] If the source type is bool, the value false is converted to
  zero and the value true is converted to one.

  round
  http://www.opengroup.org/onlinepubs/000095399/functions/round.html
  round to the nearest integer value in a floating-point format
*/

LONGLONG
SignedRound(double d) throw()
{
	// If d is smaller than the smallest long value,
	// the nearest integer value is the minimum integer
	// value
	if (LLONG_MIN > d)
	{
		return LLONG_MIN ;
	}

	// If d is greater than the greatest long value,
	// the nearest integer value is the maximum integer
	// value.
	if (LLONG_MAX < d)
	{
		return LLONG_MAX ;
	}

# if defined(SODEVENV_VXWORKS) || defined(SOOS_WINDOWS)
	// Discard fractional digits, probe nearest long value, return it.
	LONGLONG b = (LONGLONG) d ;  // The nearest integer can only be one of the following
	LONGLONG a = b - 1 ;                 // three values : b, b-1, b+1. We use the name a for the
	LONGLONG c = b + 1 ;                 // value b-1, and the name c for the name b+1.
	double dB = (d > b) ? (d - b) : (b - d) ;    // absalute value of the double value d-b
	double dA = (d > a) ? (d - a) : (a - b) ;    // absolute value of the double value d-a
	double dC = (d > c) ? (d - c) : (c - d) ;    // absolute value of the double value d-c

	if (dB < dA) //dB<dA
	{
		if (dB < dC) // dB<dA, dB<dC
		{
			return b ;
		}
		else     // dC < dB < dA
		{
			return c ;
		}
	}
	else     // dA<dB
	{
		if (dA < dC) // dA<dB, dA<dC
		{
			return a ;
		}
		else     // dC<dA<dB
		{
			return c ;
		}
	}

	/*
	  Example 1                |   Example 2                 |   Example 3                |   Example 4                |
	  d=1.2                    |   d=1.8                     |   d=-1.2                   |   d=-1.8                   |
	  b=1 a=0 c=2              |   b=1 a=0 c=2               |   b=-1 a=-2 c=0            |   b=-1 a=-2 c=0            |
	  dB=0.2 dA=1.2 dC=0.8     |   dB=0.8 dA=1.8 dC=0.2      |   dB=0.2 dA=0.8 dC=1.2     |   dB=0.8 dA=0.2 dC=1.8     |
	  --> b                    |   --> c                     |   --> b                    |   --> a                    |
	*/
# else
	double roundA = round(d) ;
	LONGLONG roundB = (LONGLONG) roundA ;
	return roundB ;
# endif
} // SignedRound

ULONGLONG
UnsignedRound(double d) throw()
{
	// If d is smaller than the smallest long value,
	// the nearest integer value is the minimum integer
	// value
	if (0 > d)
	{
		return 0 ;
	}

	// If d is greater than the greatest long value,
	// the nearest integer value is the maximum integer
	// value.
	if (ULLONG_MAX < d)
	{
		return ULLONG_MAX ;
	}

# if defined(SODEVENV_VXWORKS) || defined(SOOS_WINDOWS)
	// Discard fractional digits, probe nearest long value, return it.
	ULONGLONG b = (ULONGLONG) d ;  // The nearest integer can only be one of the following
	ULONGLONG a = b - 1 ;                 // three values : b, b-1, b+1. We use the name a for the
	ULONGLONG c = b + 1 ;                 // value b-1, and the name c for the name b+1.
	double dB = (d > b) ? (d - b) : (b - d) ;      // absalute value of the double value d-b
	double dA = (d > a) ? (d - a) : (a - b) ;      // absolute value of the double value d-a
	double dC = (d > c) ? (d - c) : (c - d) ;      // absolute value of the double value d-c

	if (dB < dA) //dB<dA
	{
		if (dB < dC) // dB<dA, dB<dC
		{
			return b ;
		}
		else     // dC < dB < dA
		{
			return c ;
		}
	}
	else     // dA<dB
	{
		if (dA < dC) // dA<dB, dA<dC
		{
			return a ;
		}
		else     // dC<dA<dB
		{
			return c ;
		}
	}

	/*
	  Example 1                |   Example 2                 |   Example 3                |   Example 4                |
	  d=1.2                    |   d=1.8                     |   d=-1.2                   |   d=-1.8                   |
	  b=1 a=0 c=2              |   b=1 a=0 c=2               |   b=-1 a=-2 c=0            |   b=-1 a=-2 c=0            |
	  dB=0.2 dA=1.2 dC=0.8     |   dB=0.8 dA=1.8 dC=0.2      |   dB=0.2 dA=0.8 dC=1.2     |   dB=0.8 dA=0.2 dC=1.8     |
	  --> b                    |   --> c                     |   --> b                    |   --> a                    |
	*/
# else
	double roundA = round(d) ;
	ULONGLONG roundB = (ULONGLONG) roundA ;
	return roundB ;
# endif
} // UnsignedRound

