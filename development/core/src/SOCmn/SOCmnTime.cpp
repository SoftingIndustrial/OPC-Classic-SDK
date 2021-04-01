#include "stdafx.h"

#ifdef SOFEATURE_CMN_TIME

#include <SOCmn.h>
#include <SOCmnTime.h>

#ifdef SOOS_WINDOWS
// this function is only available under NT!
BOOL (WINAPI* s_pSystemTimeToTzSpecificLocalTime)
(LPTIME_ZONE_INFORMATION lpTimeZoneInformation,
 LPSYSTEMTIME lpUniversalTime,
 LPSYSTEMTIME lpLocalTime) = NULL;
// set date time format to UTC implicitly
BOOL SOCmnDateTime::m_dateIsUTC = SOCmnDateTime::setDateIsUtc();
#endif


//-----------------------------------------------------------------------------
// SOCmnDateTime                                                              |
//-----------------------------------------------------------------------------

const SOCmnDateTime& SOCmnDateTime::operator=(
	IN const SOCmnDateTime& cmnDT)
{
#ifdef SOOS_WINDOWS
	m_filetime.dwLowDateTime = cmnDT.m_filetime.dwLowDateTime;
	m_filetime.dwHighDateTime = cmnDT.m_filetime.dwHighDateTime;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_timeval.tv_sec = cmnDT.m_timeval.tv_sec;
	m_timeval.tv_usec = cmnDT.m_timeval.tv_usec;
#endif
	return *this;
}

ULONGLONG SOCmnDateTime::getMicroseconds(void) const
{
#ifdef SOOS_WINDOWS
	return *(ULONGLONG*)&m_filetime / 10;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
#define TIME_OFFSET ((ULONGLONG)(3600u*1000*1000) * (((1970-1601)*365 + (1970-1601)/4 - (1970-1601)/100) * 24u))
	ULONGLONG ms = ((ULONGLONG)m_timeval.tv_sec * 1000000) + (ULONGLONG)m_timeval.tv_usec;
	return ms + TIME_OFFSET;
#endif
}

void SOCmnDateTime::setMicroseconds(ULONGLONG ms)
{
#ifdef SOOS_WINDOWS
	*(ULONGLONG*)&m_filetime = ms * 10;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	ms -= TIME_OFFSET;

	if (ms > (ULONGLONG)0x7FFFFFFF * 1000000)
	{
		clear();
	}
	else
	{
		m_timeval.tv_sec = (time_t)(ms / 1000000);
		m_timeval.tv_usec = (long)(ms % 1000000);
	}

#endif
}

void SOCmnDateTime::addMicroseconds(LONGLONG ms)
{
#ifdef SOOS_WINDOWS
	*(ULONGLONG*)&m_filetime += ms * 10;  /* micro seconds (1E-6 sec) to 100 nanosec units (1E-7 sec) */
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_timeval.tv_sec += ms / 1000000;
	m_timeval.tv_usec += ms % 1000000;

	if (m_timeval.tv_usec >= 1000000)
	{
		m_timeval.tv_sec++;
		m_timeval.tv_usec -= 1000000;
	}
	else if (m_timeval.tv_usec < 0)
	{
		m_timeval.tv_sec--;
		m_timeval.tv_usec += 1000000;
	}

#endif
}


/*
 From RFC 882

 5.  DATE AND TIME SPECIFICATION

     5.1.  SYNTAX

     date-time   =  [ day "," ] date time        ; dd mm yy
                                                 ;  hh:mm:ss zzz

     day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
                 /  "Fri"  / "Sat" /  "Sun"

     date        =  1*2DIGIT month 2DIGIT        ; day month year
                                                 ;  e.g. 20 Jun 82

     month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
                 /  "May"  /  "Jun" /  "Jul"  /  "Aug"
                 /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"

     time        =  hour zone                    ; ANSI and Military

     hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
                                                 ; 00:00:00 - 23:59:59

     zone        =  "UT"  / "GMT"                ; Universal Time
                                                 ; North American : UT
                 /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
                 /  "CST" / "CDT"                ;  Central:  - 6/ - 5
                 /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
                 /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
                 /  1ALPHA                       ; Military: Z = UT;
                                                 ;  A:-1; (J not used)
                                                 ;  M:-12; N:+1; Y:+12
                 / ( ("+" / "-") 4DIGIT )        ; Local differential
                                                 ;  hours+min. (HHMM)

     5.2.  SEMANTICS

          If included, day-of-week must be the day implied by the date
     specification.

          Time zone may be indicated in several ways.  "UT" is Univer-
     sal  Time  (formerly called "Greenwich Mean Time"); "GMT" is per-
     mitted as a reference to Universal Time.  The  military  standard
     uses  a  single  character for each zone.  "Z" is Universal Time.
     "A" indicates one hour earlier, and "M" indicates 12  hours  ear-
     lier;  "N"  is  one  hour  later, and "Y" is 12 hours later.  The
     letter "J" is not used.  The other remaining two forms are  taken
     from ANSI standard X3.51-1975.  One allows explicit indication of
     the amount of offset from UT; the other uses  common  3-character
     strings for indicating time zones in North America.
*/

TCHAR s_dayOfWeek3[7][4] =
{ _T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat") };
TCHAR s_month3[12][4] =
{ _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

SOCmnString SOCmnDateTime::getHTTPDate(void) const
{
#ifdef SOOS_WINDOWS
	SYSTEMTIME sysT;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct tm sysT;
#endif
	SOCmnString sz;
#ifdef SOOS_WINDOWS
	FileTimeToSystemTime(&m_filetime, &sysT);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	gmtime_r(&m_timeval.tv_sec, &sysT);
#endif
	sz.format(_T("%s, %u %s %u %2.2u:%2.2u:%2.2u GMT"),
#ifdef SOOS_WINDOWS
			  s_dayOfWeek3[sysT.wDayOfWeek], sysT.wDay,
			  s_month3[sysT.wMonth - 1], sysT.wYear,
			  sysT.wHour, sysT.wMinute, sysT.wSecond
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			  s_dayOfWeek3[sysT.tm_wday], sysT.tm_mday,
			  s_month3[sysT.tm_mon], sysT.tm_year + 1900,
			  sysT.tm_hour, sysT.tm_min, sysT.tm_sec
#endif
			 );
	return sz;
}

/*
A single lexical representation, which is a subset of the lexical representations
allowed by [ISO 8601], is allowed for dateTime. This lexical representation is the
[ISO 8601] extended format CCYY-MM-DDThh:mm:ss where "CC" represents the century,
"YY" the year, "MM" the month and "DD" the day, preceded by an optional leading "-"
sign to indicate a negative number. If the sign is omitted, "+" is assumed. The
letter "T" is the date/time separator and "hh", "mm", "ss" represent hour, minute
 and second respectively. Additional digits can be used to increase the precision
of fractional seconds if desired i.e the format ss.ss... with any number of digits
after the decimal point is supported. The fractional seconds part is optional; other
parts of the lexical form are not optional. To accommodate year values greater than
9999 additional digits can be added to the left of this representation. Leading
zeros are required if the year value would otherwise have fewer than four digits;
otherwise they are forbidden. The year 0000 is prohibited.

The CCYY field must have at least four digits, the MM, DD, SS, hh, mm and ss fields
exactly two digits each (not counting fractional seconds); leading zeroes must be
used if the field would otherwise have too few digits.

This representation may be immediately followed by a "Z" to indicate Coordinated
Universal Time (UTC) or, to indicate the time zone, i.e. the difference between
the local time and Coordinated Universal Time, immediately followed by a sign, +
or -, followed by the difference from UTC represented as hh:mm (note: the minutes
part is required). See ISO 8601 Date and Time Formats for details about legal
values in the various fields. If the time zone is included, both hours and minutes
must be present.

For example, to indicate 1:20 pm on May the 31st, 1999 for Eastern Standard Time
which is 5 hours behind Coordinated Universal Time (UTC), one would write:
1999-05-31T13:20:00-05:00.

*/

SOCmnString SOCmnDateTime::getXMLDateTime(void) const
{
	SOCmnString sz;
	LONG timeBias;
#ifdef SOOS_WINDOWS
	FILETIME ftLocal;
	SYSTEMTIME locT;
	TIME_ZONE_INFORMATION timeZone;
	DWORD timeZoneRet;
	timeZoneRet = GetTimeZoneInformation(&timeZone);
	timeBias = timeZone.Bias;

	switch (timeZoneRet)
	{
	case TIME_ZONE_ID_STANDARD:
		timeBias += timeZone.StandardBias;
		break;

	case TIME_ZONE_ID_DAYLIGHT:
		timeBias += timeZone.DaylightBias;
		break;
	}

	timeBias = -timeBias; /* WINDOWS uses different sign */
	FileTimeToLocalFileTime(&m_filetime, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &locT);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct tm locT, sysT;
	gmtime_r(&m_timeval.tv_sec, &sysT);
	localtime_r(&m_timeval.tv_sec, &locT);
	sysT.tm_isdst = locT.tm_isdst; /* <-- very important!!! */
	timeBias = mktime(&locT) - mktime(&sysT); /* CET=3600, CEST=7200 */
	timeBias /= 60; /* convert to minutes */
#endif
	sz.format(_T("%u-%2.2u-%2.2uT%2.2u:%2.2u:%2.2u.%3.3u%c%2.2u:%2.2u"),
#ifdef SOOS_WINDOWS
			  locT.wYear, locT.wMonth, locT.wDay,
			  locT.wHour, locT.wMinute, locT.wSecond,
			  locT.wMilliseconds,
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			  locT.tm_year + 1900, locT.tm_mon + 1, locT.tm_mday,
			  locT.tm_hour, locT.tm_min, locT.tm_sec,
			  (int)(m_timeval.tv_usec / 1000),
#endif
			  (timeBias < 0) ? _T('-') : _T('+'),
			  abs(timeBias) / 60, abs(timeBias) % 60
			 );
	return sz;
}

BOOL SOCmnDateTime::setXMLDateTime(LPCTSTR xmlDateTime)
{
	if (_tcslen(xmlDateTime) < 19)
	{
		return FALSE;
	}

	if ((xmlDateTime[4] != _T('-')) || (xmlDateTime[7] != _T('-')) || (xmlDateTime[10] != _T('T')) ||
		(xmlDateTime[13] != _T(':')) || (xmlDateTime[16] != _T(':')))
	{
		return FALSE;
	}

	LPCTSTR curP;
#ifdef SOOS_WINDOWS
	FILETIME tvT;
	SYSTEMTIME sysT;
	sysT.wYear = (WORD)_tcstoul(xmlDateTime + 0, NULL, 10);
	sysT.wMonth = (WORD)_tcstoul(xmlDateTime + 5, NULL, 10);
	sysT.wDay = (WORD)_tcstoul(xmlDateTime + 8, NULL, 10);
	sysT.wHour = (WORD)_tcstoul(xmlDateTime + 11, NULL, 10);
	sysT.wMinute = (WORD)_tcstoul(xmlDateTime + 14, NULL, 10);
	sysT.wSecond = (WORD)_tcstoul(xmlDateTime + 17, NULL, 10);
	sysT.wMilliseconds = 0;
	SystemTimeToFileTime(&sysT, &tvT);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct timeval tvT;
	struct tm locT, sysT;
	locT.tm_year = (WORD)_tcstoul(xmlDateTime + 0, NULL, 10) - 1900;
	locT.tm_mon = (WORD)_tcstoul(xmlDateTime + 5, NULL, 10) - 1;
	locT.tm_mday = (WORD)_tcstoul(xmlDateTime + 8, NULL, 10);
	locT.tm_hour = (WORD)_tcstoul(xmlDateTime + 11, NULL, 10);
	locT.tm_min = (WORD)_tcstoul(xmlDateTime + 14, NULL, 10);
	locT.tm_sec = (WORD)_tcstoul(xmlDateTime + 17, NULL, 10);
	locT.tm_isdst = 0; /* assume local time w/o dst correction */
	tvT.tv_sec = mktime(&locT);
	gmtime_r(&tvT.tv_sec, &sysT);
	/* add local time zone offset */
	tvT.tv_sec += tvT.tv_sec - mktime(&sysT);
	tvT.tv_usec = 0;
#endif
	curP = xmlDateTime + 19;

	if (*curP == _T('.'))
	{
		DWORD m;
		curP++;
#ifdef SOOS_WINDOWS
		m = 1000000; // 100 nanoseconds
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		m = 100000;  // 1 microsecod
#endif

		while (TRUE)
		{
			if (_istdigit(*curP) != 0)
			{
				DWORD dg = *curP - _T('0');
#ifdef SOOS_WINDOWS
				ULARGE_INTEGER temp;
				temp.LowPart = tvT.dwLowDateTime;
				temp.HighPart = tvT.dwHighDateTime;
				temp.QuadPart += dg * m;;
				tvT.dwLowDateTime = temp.LowPart;
				tvT.dwHighDateTime = temp.HighPart;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
				tvT.tv_usec += dg * m;
#endif
				m /= 10;
				curP++;
			}
			else
			{
				break;
			}
		}
	}

	switch (*curP)
	{
	case _T('+'):
	case _T('-'):
	{
		if (_tcslen(curP) >= 5)
		{
			DWORD loc;
			loc = (_tcstoul(curP + 1, NULL, 10) * 60) + _tcstoul(curP + 4, NULL, 10);
#ifdef SOOS_WINDOWS

			if (*curP == _T('-'))
			{
				ULARGE_INTEGER temp;
				temp.LowPart = tvT.dwLowDateTime;
				temp.HighPart = tvT.dwHighDateTime;
				temp.QuadPart += ((LONGLONG)loc) * 600000000;
				tvT.dwLowDateTime = temp.LowPart;
				tvT.dwHighDateTime = temp.HighPart;
			}
			else
			{
				ULARGE_INTEGER temp;
				temp.LowPart = tvT.dwLowDateTime;
				temp.HighPart = tvT.dwHighDateTime;
				temp.QuadPart -= ((LONGLONG)loc) * 600000000;
				tvT.dwLowDateTime = temp.LowPart;
				tvT.dwHighDateTime = temp.HighPart;
			}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

			if (*curP == _T('-'))
			{
				tvT.tv_sec += loc * 60;
			}
			else
			{
				tvT.tv_sec -= loc * 60;
			}

#endif
		}
		else
		{
			return FALSE;
		}
	}
	break;

	case _T('Z'):
		break;

	case _T('\0'):  // time zone _may_ be included.
	{
		// we use the local time zone here.
#ifdef SOOS_WINDOWS
		FILETIME gmT;
		LocalFileTimeToFileTime(&tvT, &gmT);
		tvT = gmT;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		gmtime_r(&tvT.tv_sec, &locT);
		locT.tm_isdst = -1;
		tvT.tv_sec = mktime(&locT);
#endif
	}
	break;

	default:
		return FALSE;
		break;
	}

	set(&tvT);
	return TRUE;
}


//-----------------------------------------------------------------------------
// toString
//
// - convert dateTime to string
//
// returns:
//		pointer to string
//
LPCTSTR SOCmnDateTime::toString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID, OPTIONAL IN BOOL withDate,
								IN BOOL withMilliseconds, IN BOOL yearOnRight, IN LPCTSTR yearSeparator) const
{
	localeID = localeID; // inserted since waring at warning level 4
	LPCTSTR separator = yearSeparator;
#ifdef SOOS_WINDOWS
	SYSTEMTIME t;
	FILETIME f;
	FileTimeToLocalFileTime(&m_filetime, &f);
	FileTimeToSystemTime(&f, &t);

	if (withMilliseconds == TRUE)
	{
		buffer.Format(_T("%2.2d:%2.2d:%2.2d.%3.3d"),
					  t.wHour, t.wMinute, t.wSecond,
					  t.wMilliseconds);
	}
	else
	{
		buffer.Format(_T("%2.2d:%2.2d:%2.2d"),
					  t.wHour, t.wMinute, t.wSecond);
	}

	if (withDate == TRUE)
	{
		if (yearOnRight == TRUE)
		{
			buffer.Format(_T("%02.2d%s%02.2d%s%04.4d %s"), t.wDay, separator, t.wMonth, separator, t.wYear, (LPCTSTR)buffer);
		}
		else
		{
			buffer.Format(_T("%04.4d%s%02.2d%s%02.2d %s"), t.wYear, separator, t.wMonth, separator, t.wDay, (LPCTSTR)buffer);
		}
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct tm t;
	localtime_r(&m_timeval.tv_sec, &t);

	if (withMilliseconds == TRUE)
	{
		buffer.Format(_T("%2.2d:%2.2d:%2.2d.%3.3d"),
					  t.tm_hour, t.tm_min, t.tm_sec,
					  (int)(m_timeval.tv_usec / 1000));
	}
	else
	{
		buffer.Format(_T("%2.2d:%2.2d:%2.2d"),
					  t.tm_hour, t.tm_min, t.tm_sec);
	}

	if (withDate == TRUE)
	{
		if (yearOnRight == TRUE)
		{
			buffer.Format(_T("%2.2d%s%2.2d%s%4.4d %s"), t.tm_mday, separator, t.tm_mon, separator, t.tm_year + 1900, (LPCTSTR)buffer);
		}
		else
		{
			buffer.Format(_T("%4.4d%s%2.2d%s%2.2d %s"), t.tm_year + 1900, separator, t.tm_mon, separator, t.tm_mday, (LPCTSTR)buffer);
		}
	}

#endif
	return (LPCTSTR)buffer;
} // toString

//-----------------------------------------------------------------------------
// toUTCString
//
// - convert dateTime to string; UTC based
//
// returns:
//		pointer to string
//
LPCTSTR SOCmnDateTime::toUTCString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID, OPTIONAL IN BOOL withDate) const
{
	localeID = localeID; // inserted since waring at warning level 4
#ifdef SOOS_WINDOWS
	SYSTEMTIME t;
	FileTimeToSystemTime(&m_filetime, &t);
	buffer.Format(_T("%2.2d:%2.2d:%2.2d.%3.3d"),
				  t.wHour, t.wMinute, t.wSecond,
				  t.wMilliseconds);

	if (withDate)
	{
		buffer.Format(_T("%02.2d.%02.2d.%04.4d %s"), t.wDay, t.wMonth, t.wYear, (LPCTSTR)buffer);
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct tm t;
	gmtime_r(&m_timeval.tv_sec, &t);
	buffer.Format(_T("%2.2d:%2.2d:%2.2d.%3.3d"),
				  t.tm_hour, t.tm_min, t.tm_sec,
				  (int)(m_timeval.tv_usec / 1000));

	if (withDate)
	{
		buffer.Format(_T("%2.2d.%2.2d.%4.4d %s"), t.tm_mday, t.tm_mon + 1, t.tm_year + 1900, (LPCTSTR)buffer);
	}

#endif
	return (LPCTSTR)buffer;
} // toUTCString

#ifdef SOOS_WINDOWS
INT SOCmnDateTime::systemTimeToVariantTime(SYSTEMTIME* pSysT, DATE* pDate)
{
	if (!SystemTimeToVariantTime(pSysT, pDate))
	{
		return FALSE;
	}

	*pDate += (double)((double)pSysT->wMilliseconds / 86400000.0);
	return TRUE;
}


INT SOCmnDateTime::variantTimeToSystemTime(DATE date, SYSTEMTIME* pSysT)
{
	if (!VariantTimeToSystemTime(date - 499.5 / 86400000.0, pSysT))
	{
		return FALSE;
	}

	DATE noMSDate;
	LONG ms;
	SystemTimeToVariantTime(pSysT, &noMSDate);
	ms = (LONG)((date - noMSDate) * 86400000.0 + 0.5);

	if (ms < 0)
	{
		ms = 0;
	}
	else if (ms > 999)
	{
		ms = 999;
	}

	pSysT->wMilliseconds = (WORD)ms;
	return TRUE;
}
#endif

#endif // SOFEATURE_CMN_TIME

#ifdef SOTEST_CMN
#ifdef SOCMNTIME

int _tmain(int argc, _TCHAR* argv[])
{
	::initSOCmn();
	SOCmnTrace* pTrace = ::getSOCmnTrace();
	pTrace->setLevels(SOCMNTRACE_G_ALL, SOCMNTRACE_G_ALL, SOCMNTRACE_G_TEST, SOCMNTRACE_G_TEST);
	SOCmnDateTime t1;
	SOCmnString t1SZin;
	SOCmnString t1SZst;
	SOCmnString t1SZout;
	FILETIME t1FT;
	SYSTEMTIME t1ST;
	t1SZin = _T("2005-01-01T15:44:47.162+01:00");
	t1.setXMLDateTime(t1SZin);
	t1SZout = t1.getXMLDateTime();
	t1.get(&t1FT);
	FileTimeToSystemTime(&t1FT, &t1ST);
	t1SZst.Format(_T("%2.2d:%2.2d:%2.2d.%3.3d"), t1ST.wHour, t1ST.wMinute, t1ST.wSecond, t1ST.wMilliseconds);
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_TEST, _T("SOCmnTime Test"), (_T("IN XML:  %s"), t1SZin));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_TEST, _T("SOCmnTime Test"), (_T("ST:      %s"), t1SZst));
	_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_TEST, _T("SOCmnTime Test"), (_T("OUT XML: %s"), t1SZout));
	::exitSOCmn();
	return 0;
}

#endif // SOCMNTIME
#endif // SOTEST_CMN
