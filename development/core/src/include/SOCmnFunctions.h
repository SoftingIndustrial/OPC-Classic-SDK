#ifndef _SOCMNFUNCTIONS_H_
#define _SOCMNFUNCTIONS_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

// allocate and free memory
SOCMN_EXPORT void freeMemory(IN void* memptr);
SOCMN_EXPORT void* allocateMemory(IN ULONG size);
SOCMN_EXPORT void* reallocateMemory(IN void* memptr, IN ULONG size);

// get the time span between two times
SOCMN_EXPORT ULONG getTimeSpan(IN DWORD from, IN DWORD till);

#ifdef SOOS_WINDOWS
SOCMN_EXPORT __int64 getTimeSpan(IN __int64 from, IN __int64 till);
SOCMN_EXPORT __int64 getTimeSpan10Micro(IN __int64 from, IN __int64 till, IN __int64 frequency);

#ifndef SOOS_WINDOWS_95
#define interlockedIncrement(a) InterlockedIncrement(a)
#define interlockedDecrement(a) InterlockedDecrement(a)
#else
SOCMN_EXPORT LONG interlockedIncrement(LONG* lpAddend);
SOCMN_EXPORT LONG interlockedDecrement(LONG* lpAddend);
#endif
#endif // SOOS_WINDOWS

#ifdef SOFEATURE_DCOM
// covert COM enum to string list
SOCMN_EXPORT void enumString2stringList(IN IEnumString* enumString,
										OUT SOCmnStringList* stringList);
#endif

#ifdef SOOS_LINUX
SOCMN_EXPORT DWORD GetTickCount(void);
SOCMN_EXPORT void Sleep(DWORD timeout);
SOCMN_EXPORT LONG interlockedIncrement(LONG* lpAddend);
SOCMN_EXPORT LONG interlockedDecrement(LONG* lpAddend);
#ifdef __powerpc__
inline LONG interlockedIncrement(LONG* lpAddend)
{
	LONG retval;
	__asm__("1:\tlwarx %1,0,%2\n"
			"\taddi %1,%1,1\n"
			"\tstwcx. %1,0,%2\n"
			"\tbne- 1b\n"
			: "+m"(*lpAddend), "=&b"(retval)
			: "r"(lpAddend) : "cc");
	return retval;
}
inline LONG interlockedDecrement(LONG* lpAddend)
{
	LONG retval;
	__asm__("1:\tlwarx %1,0,%2\n"
			"\tsubi %1,%1,1\n"
			"\tstwcx. %1,0,%2\n"
			"\tbne- 1b\n"
			: "+m"(*lpAddend), "=&b"(retval)
			: "r"(lpAddend) : "cc");
	return retval;
}
#endif
#endif

// old macros - don't use in new code
#define EMPTYSTRING(a)
#define INITSTRING(a)
#define SETSTRING(a, b) { a = b; }
#define ADDSTRING(a, b) (a).add(b);
#define EMPTYSTRINGLIST(a) { (a).removeAll();}
#define ADDPOINTER(a, b) (a).add(b);
#define EMPTYPTRLIST(a) { (a).removeAll();}
#define FREEMEMORY(a) ::freeMemory(a);
#define ALLOCATEMEMORY(a) ::allocateMemory(a);

// Returns the long value x such that the
// absolute value of the double value d-x
// has minimum value.
LONGLONG SignedRound(double d) throw() ;
ULONGLONG UnsignedRound(double d) throw() ;

#endif
