#ifndef _SOCMNTIME_H_
#define _SOCMNTIME_H_

#ifdef SOFEATURE_CMN_TIME

#include "SOCmn.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCmnDateTime                                                              |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnDateTime
{
public:
	SOCmnDateTime()
	{
		init();
	}

#ifdef SOOS_WINDOWS
	SOCmnDateTime(const FILETIME* pFT)
	{
		set(pFT);
	}
	SOCmnDateTime(const DATE* pDate)
	{
		set(pDate);
	}
	const SOCmnDateTime& operator= (const FILETIME& pFT)
	{
		set(&pFT);
		return *this;
	}
	const SOCmnDateTime& operator= (const DATE& pDate)
	{
		set(&pDate);
		return *this;
	}

	void init(void)
	{
		m_filetime.dwLowDateTime = 0L;
		m_filetime.dwHighDateTime = 0L;
	}

	void now(void)
	{
		::GetSystemTimeAsFileTime(&m_filetime);
	}
	void set(const FILETIME* pFT)
	{
		m_filetime.dwLowDateTime = pFT->dwLowDateTime;
		m_filetime.dwHighDateTime = pFT->dwHighDateTime;
	}
	void get(FILETIME* pFT) const
	{
		pFT->dwLowDateTime = m_filetime.dwLowDateTime;
		pFT->dwHighDateTime = m_filetime.dwHighDateTime;
	}


	BOOL set(const DATE* pDate)
	{
		BOOL ok = TRUE;

		if ((double)*pDate > -109205) // 1601
		{
			SYSTEMTIME sysT;
			ok &= variantTimeToSystemTime(*pDate, &sysT);

			if (m_dateIsUTC != FALSE)
			{
				ok &= SystemTimeToFileTime(&sysT, &m_filetime);
			}
			else
			{
				FILETIME locT;
				ok &= SystemTimeToFileTime(&sysT, &locT);
				ok &= LocalFileTimeToFileTime(&locT, &m_filetime);
			} // end if
		}
		else
		{
			clear();
			ok = FALSE;
		}

		return ok;
	}
	BOOL get(DATE* pDate) const
	{
		BOOL ok = TRUE;
		SYSTEMTIME sysT;

		if (m_dateIsUTC != FALSE)
		{
			ok &= FileTimeToSystemTime(&m_filetime, &sysT);
			ok &= systemTimeToVariantTime(&sysT, pDate);
		}
		else
		{
			FILETIME locT;
			ok &= FileTimeToLocalFileTime(&m_filetime, &locT);
			ok &= FileTimeToSystemTime(&locT, &sysT);
			ok &= systemTimeToVariantTime(&sysT, pDate);
		}

		return ok;
	}

	static INT systemTimeToVariantTime(SYSTEMTIME* pSysT, DATE* pDate);
	static INT variantTimeToSystemTime(DATE date, SYSTEMTIME* pSysT);

	BOOL isSet(void) const
	{
		return ((m_filetime.dwLowDateTime != 0) || (m_filetime.dwHighDateTime != 0));
	}

	static BOOL setDateIsUtc(BOOL flag = FALSE)
	{
		m_dateIsUTC = flag;
		return m_dateIsUTC;
	};

	static BOOL m_dateIsUTC;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	SOCmnDateTime(const struct timeval* pFT)
	{
		set(pFT);
	}
	SOCmnDateTime(const struct timespec* pFT)
	{
		set(pFT);
	}
	const SOCmnDateTime& operator=(const struct timeval& pFT)
	{
		set(&pFT);
		return *this;
	}
	const SOCmnDateTime& operator=(const struct timespec& pFT)
	{
		set(&pFT);
		return *this;
	}

	void init(void)
	{
		m_timeval.tv_sec = 0L;
		m_timeval.tv_usec = 0L;
	}

	void now(void)
	{
		gettimeofday(&m_timeval, NULL);
	}
	void set(const struct timeval* pFT)
	{
		m_timeval.tv_sec = pFT->tv_sec;
		m_timeval.tv_usec = pFT->tv_usec;
	}
	void get(struct timeval* pFT) const
	{
		pFT->tv_sec = m_timeval.tv_sec;
		pFT->tv_usec = m_timeval.tv_usec;
	}

	void set(const struct timespec* pFT)
	{
		m_timeval.tv_sec = pFT->tv_sec;
		m_timeval.tv_usec = pFT->tv_nsec / 1000;
	}
	void get(struct timespec* pFT) const
	{
		pFT->tv_sec = m_timeval.tv_sec;
		pFT->tv_nsec = m_timeval.tv_usec * 1000;
	}

	BOOL isSet(void) const
	{
		return ((m_timeval.tv_sec != 0) || (m_timeval.tv_usec != 0));
	}
#endif

	void clear(void)
	{
		init();
	}

	/**
	  * getMicroseconds returns the number of microseconds
	  * since 1.1.1970 00:00 UTC (unit 10^-6 sec)
	  */
	ULONGLONG getMicroseconds(void) const;
	/**
	  * setMicroseconds expects the number of microseconds
	  * since 1.1.1970 00:00 UTC (unit: 10^-6 sec)
	  */
	void setMicroseconds(ULONGLONG ms);
	/**
	  * addMicroseconds adds the given microseconds
	  * to the current time value (unit: 10^-6 sec)
	  */
	void addMicroseconds(LONGLONG ms);

	const SOCmnDateTime& operator=(IN const SOCmnDateTime& cmnDT);

	BOOL setXMLDateTime(LPCTSTR xmlDateTime);

	SOCmnString getHTTPDate(void) const;
	SOCmnString getXMLDateTime(void) const;
	LPCTSTR toString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID = LOCALE_NEUTRAL, OPTIONAL IN BOOL withDate = FALSE,
					 IN BOOL withMilliseconds = TRUE, IN BOOL yearOnRight = TRUE, IN LPCTSTR dateSeparator = _T(".")) const;

	LPCTSTR toUTCString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID = LOCALE_NEUTRAL, OPTIONAL IN BOOL withDate = FALSE) const;
protected:
#ifdef SOOS_WINDOWS
	FILETIME m_filetime;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	struct timeval m_timeval;
#endif
}; // SOCmnDateTime

#pragma pack(pop)

#endif // SOFEATURE_CMN_TIME

#endif
