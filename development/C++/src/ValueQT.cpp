#include "ValueQT.h"
#include "OTcommon.h"


#ifdef TBC_OS_LINUX
#include <sys/time.h>
#endif


#define VARIANT_ERROR_BADTYPE  ((long)0xC0040004L) // = OPC_E_BADTYPE

#ifdef TBC_OS_WINDOWS
#if _MSC_VER <= 1200 // Microsoft C++ V6.0
#define SOCMNVARIANT_IMP_I64

#define LLONG_MAX     0x7fffffffffffffff  /*maximum signed __int64 value */
#define LLONG_MIN     0x8000000000000000  /*minimum signed __int64 value */
#define ULLONG_MAX    0xffffffffffffffff  /*maximum unsigned __int64 value */

#endif
#ifdef SOOS_WINDOWS_CE
#ifdef ARM
#define SOCMNVARIANT_IMP_I64
#endif
#endif
#endif

#ifdef SOCMNVARIANT_IMP_I64
__int64 _strtoi64(const char* nptr, char** endptr, int ibase);
unsigned __int64 _strtoui64(const char* nptr, char** endptr, int ibase);
__int64 _wcstoi64(const wchar_t* nptr, wchar_t** endptr, int ibase);
unsigned __int64 _wcstoui64(const wchar_t* nptr, wchar_t** endptr, int ibase);
#ifdef _UNICODE
#define _tcstoi64   _wcstoi64
#define _tcstoui64  _wcstoui64
#else
#define _tcstoi64   _strtoi64
#define _tcstoui64  _strtoui64
#endif
#endif // TBC_OS_WINDOWS

#ifdef SOOS_WINDOWS_CE
int errno;
#ifndef ERANGE
#define ERANGE -1
#endif
#endif

#ifndef MAKELONG
#define MAKELONG(a, b)      ((long)(((unsigned short)(a)) | ((unsigned long)((unsigned short)(b))) << 16))
#endif


//-----------------------------------------------------------------------------
// DateTime                                                                   |
//-----------------------------------------------------------------------------

DateTime::DateTime(
	const DateTime& dateTime)
{
#ifdef TBC_OS_WINDOWS
	m_filetime.dwLowDateTime = dateTime.m_filetime.dwLowDateTime;
	m_filetime.dwHighDateTime = dateTime.m_filetime.dwHighDateTime;
#endif
#ifdef TBC_OS_LINUX
	m_filetime.tv_sec = dateTime.m_filetime.tv_sec;
	m_filetime.tv_usec = dateTime.m_filetime.tv_usec;
#endif
}



void DateTime::init(void)
{
#ifdef TBC_OS_WINDOWS
	m_filetime.dwLowDateTime = 0L;
	m_filetime.dwHighDateTime = 0L;
#endif
#ifdef TBC_OS_LINUX
	m_filetime.tv_sec = 0L;
	m_filetime.tv_usec = 0L;
#endif
}

#ifdef TBC_OS_WINDOWS
void
DateTime::set(
	const FILETIME* pFT)
{
	m_filetime.dwLowDateTime = pFT->dwLowDateTime;
	m_filetime.dwHighDateTime = pFT->dwHighDateTime;
}



void DateTime::get(FILETIME* pFT)
{
	pFT->dwLowDateTime = m_filetime.dwLowDateTime;
	pFT->dwHighDateTime = m_filetime.dwHighDateTime;
}


void DateTime::get(DATE& pDate)
{
	SYSTEMTIME pSysT;
	FileTimeToSystemTime(&m_filetime, &pSysT);

	if (!SystemTimeToVariantTime(&pSysT, &pDate))
	{
		return ;
	}

	pDate += (double)((double)pSysT.wMilliseconds / 86400000.0);
}

#endif


#ifdef TBC_OS_LINUX
void DateTime::set(const struct timeval* pFT)
{
	m_filetime.tv_sec = pFT->tv_sec;
	m_filetime.tv_usec = pFT->tv_usec;
}

void DateTime::get(struct timeval* pFT)
{
	pFT->tv_sec = m_filetime.tv_sec;
	pFT->tv_usec = m_filetime.tv_usec;
}
#endif


void DateTime::now(void)
{
#ifdef TBC_OS_WINDOWS
	::GetSystemTimeAsFileTime(&m_filetime);
#endif
#ifdef TBC_OS_LINUX
	gettimeofday(&m_filetime, NULL);
#endif
}


int DateTime::isSet(void)
{
#ifdef TBC_OS_WINDOWS
	return (m_filetime.dwHighDateTime != 0L && m_filetime.dwLowDateTime != 0);
#endif
#ifdef TBC_OS_LINUX
	return (m_filetime.tv_sec != 0 && m_filetime.tv_usec != 0);
#endif
}



tstring DateTime::toString(void)
{
	TCHAR buffer[256] = {0};
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	_stprintf(buffer, _T("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d.%3.3d"), sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#endif
#ifdef TBC_OS_LINUX
	strftime(buffer, 252, "%d.%m.%Y  %T.", localtime(&(m_filetime.tv_sec)));
	TCHAR milisec[4] = {0};
	sprintf(milisec, _T("%3.3d"), (int)(m_filetime.tv_usec / 1000));
	strcat(buffer, milisec);
#endif
	return buffer;
}

tstring DateTime::toStringGMT(void)
{
	TCHAR buffer[256] = {0};
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	_stprintf(buffer, _T("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d.%3.3d"), sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#endif
#ifdef TBC_OS_LINUX
	strftime(buffer, 252, "%d.%m.%Y  %T.", gmtime(&(m_filetime.tv_sec)));
	TCHAR milisec[4] = {0};
	sprintf(milisec, _T("%3.3d"), (int)(m_filetime.tv_usec / 1000));
	strcat(buffer, milisec);
#endif
	return buffer;
}

unsigned long DateTime::milliSecond(void)
{
	unsigned long milliSecond = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	milliSecond = (unsigned long)sysTime.wMilliseconds;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	milliSecond = (unsigned long)(m_filetime.tv_usec / 1000);
#endif
	return milliSecond;
}

unsigned long DateTime::milliSecondGMT(void)
{
	unsigned long milliSecond = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	milliSecond = (unsigned long)sysTime.wMilliseconds;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	milliSecond = (unsigned long)(tt->tm_sec * 1000);
#endif
	return milliSecond;
}

unsigned short DateTime::second(void)
{
	unsigned short second = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	second = (unsigned short)sysTime.wSecond;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	second = (unsigned short)t.tm_sec;
#endif
	return second;
}

unsigned short DateTime::secondGMT(void)
{
	unsigned short second = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	second = (unsigned short)sysTime.wSecond;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	second = (unsigned short)tt->tm_sec;
#endif
	return second;
}

unsigned short DateTime::minute(void)
{
	unsigned short minute = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	minute = (unsigned short)sysTime.wMinute;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	minute = (unsigned short)t.tm_min;
#endif
	return minute;
}

unsigned short DateTime::minuteGMT(void)
{
	unsigned short minute = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	minute = (unsigned short)sysTime.wMinute;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	minute = (unsigned short)tt->tm_min;
#endif
	return minute;
}

unsigned short DateTime::hour(void)
{
	unsigned short hour = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	hour = (unsigned short)sysTime.wHour;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	hour = (unsigned short)t.tm_hour;
#endif
	return hour;
}

unsigned short DateTime::hourGMT(void)
{
	unsigned short hour = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	hour = (unsigned short)sysTime.wHour;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	hour = (unsigned short)tt->tm_hour;
#endif
	return hour;
}

unsigned short DateTime::day(void)
{
	unsigned short day = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	day = (unsigned short)sysTime.wDay;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	day = (unsigned short)t.tm_mday;
#endif
	return day;
}

unsigned short DateTime::dayGMT(void)
{
	unsigned short day = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	day = (unsigned short)sysTime.wDay;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	day = (unsigned short)tt->tm_mday;
#endif
	return day;
}

unsigned short DateTime::month(void)
{
	unsigned short month = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	month = (unsigned short)sysTime.wMonth;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	month = (unsigned short)t.tm_mon;
#endif
	return month;
}

unsigned short DateTime::monthGMT(void)
{
	unsigned short month = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	month = (unsigned short)sysTime.wMonth;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	month = (unsigned short)tt->tm_mon;
	month++;
#endif
	return month;
}

unsigned long DateTime::year(void)
{
	unsigned long year = 0;
#ifdef TBC_OS_WINDOWS
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_filetime, &localFileTime);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	year = (unsigned long)sysTime.wYear;
#endif
#ifdef TBC_OS_LINUX
	struct tm t;
	localtime_r(&m_filetime.tv_sec, &t);
	year = (unsigned long)(t.tm_year + 1900);
#endif
	return year;
}

unsigned long DateTime::yearGMT(void)
{
	unsigned long year = 0;
#ifdef TBC_OS_WINDOWS
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&m_filetime, &sysTime);
	year = (unsigned long)sysTime.wYear;
#endif
#ifdef TBC_OS_LINUX
	struct tm* tt;
	tt = gmtime(&m_filetime.tv_sec);
	year = (unsigned long)tt->tm_year + 1900;
#endif
	return year;
}

#ifdef TBC_OS_LINUX
#define TIME_OFFSET ((unsigned long long)(3600u*1000*1000) * (((1970-1601)*365 + (1970-1601)/4 - (1970-1601)/100) * 24u))
#endif

ULONGLONG DateTime::getMicroseconds(void) const
{
#ifdef TBC_OS_WINDOWS
	return *(ULONGLONG*)&m_filetime / 10;
#endif // SOOS_WINDOWS
#ifdef TBC_OS_LINUX
	ULONGLONG ms = ((ULONGLONG)m_filetime.tv_sec * 1000000) + (ULONGLONG)m_filetime.tv_usec;
	return ms + TIME_OFFSET;
#endif
}

void DateTime::setMicroseconds(ULONGLONG ms)
{
#ifdef TBC_OS_WINDOWS
	*(ULONGLONG*)&m_filetime = ms * 10;
#endif // SOOS_WINDOWS
#ifdef TBC_OS_LINUX
	ms -= TIME_OFFSET;

	if (ms > (ULONGLONG)0x7FFFFFFF * 1000000)
	{
		clear();
	}
	else
	{
		m_filetime.tv_sec = (time_t)(ms / 1000000);
		m_filetime.tv_usec = (long)(ms % 1000000);
	}

#endif
}

void DateTime::addMicroseconds(ULONGLONG ms)
{
#ifdef TBC_OS_WINDOWS
	*(ULONGLONG*)&m_filetime += ms * 10;  /* micro seconds (1E-6 sec) to 100 nanosec units (1E-7 sec) */
#endif // SOOS_WINDOWS
#ifdef TBC_OS_LINUX
	m_filetime.tv_sec += ms / 1000000;
	m_filetime.tv_usec += ms % 1000000;

	if (m_filetime.tv_usec >= 1000000)
	{
		m_filetime.tv_sec++;
		m_filetime.tv_usec -= 1000000;
	}
	else if (m_filetime.tv_usec < 0)
	{
		m_filetime.tv_sec--;
		m_filetime.tv_usec += 1000000;
	}

#endif
}


//-----------------------------------------------------------------------------
// Max length of variant to string                                                                  |
//-----------------------------------------------------------------------------
const int MAX_LENGTH_OF_VARIANT_TO_STRING = 256;
//----------------------------------------------------------------------------
// toString
//-----------------------------------------------------------------------------
tstring ValueData::toString(void)
{
	OTChar buffer[MAX_LENGTH_OF_VARIANT_TO_STRING] = {0};
	return OTVariantToString(ValueData::m_data, MAX_LENGTH_OF_VARIANT_TO_STRING, buffer);
}


//----------------------------------------------------------------------------
// toString
//-----------------------------------------------------------------------------
tstring ValueQT::toString(void)
{
	TCHAR bufferVQT[50 + MAX_LENGTH_OF_VARIANT_TO_STRING];
	TCHAR bufferV[MAX_LENGTH_OF_VARIANT_TO_STRING];
	bufferV[0] = 0;
	OTVariantToString(ValueData::m_data, MAX_LENGTH_OF_VARIANT_TO_STRING, bufferV);
#ifdef TBC_OS_WINDOWS
	_stprintf(bufferVQT, _T("%s (Q:%u T:%s)"), bufferV, m_quality, m_timeStamp.toString().c_str());
#endif
#ifdef TBC_OS_LINUX
	sprintf(bufferVQT, _T("%s (Q:%u T:%s)"), bufferV, m_quality, m_timeStamp.toString().c_str());
#endif
	return bufferVQT;
}

//----------------------------------------------------------------------------
// Variant                                                               |
//-----------------------------------------------------------------------------
Variant::Variant(void)
{
	OTVariantInit(this);
}



Variant::Variant(IN const Variant& cmnVar)
{
	OTVariantInit(this);
	initFrom((LPVARIANT)&cmnVar);
}


Variant::Variant(IN const VARIANT& var)
{
	OTVariantInit(this);
	initFrom((LPVARIANT)&var);
}


Variant::Variant(IN LPCVARIANT pvar)
{
	OTVariantInit(this);
	initFrom((LPVARIANT)pvar);
}


void Variant::init(IN VARTYPE type)
{
	OTVariantInit(this);

	if ((type & VT_ARRAY) == VT_ARRAY)
	{
		// array
		vt = type;
		parray = Variant::safeArrayCreateVector((VARTYPE)(type & ~VT_ARRAY), 0, 0);
	}
	else
	{
		vt = type;

		switch (type)
		{
		case VT_I1:
			cVal = 0;
			break;

		case VT_UI1:
			bVal = 0;
			break;

		case VT_I2:
			iVal = 0;
			break;

		case VT_UI2:
			uiVal = 0;
			break;

		case VT_I4:
			lVal = 0;
			break;

		case VT_UI4:
			ulVal = 0;
			break;

		case VT_I8:
			V_I8(this) = 0;
			break;

		case VT_UI8:
			V_UI8(this) = 0;
			break;

		case VT_INT:
			intVal = 0;
			break;

		case VT_UINT:
			uintVal = 0;
			break;

		case VT_CY:
			cyVal.int64 = 0;
			break;

		case VT_R4:
			fltVal = 0.0;
			break;

		case VT_R8:
			dblVal = 0.0;
			break;

		case VT_BOOL:
			boolVal = VARIANT_FALSE;
			break;

		case VT_DATE:
#ifdef TBC_OS_WINDOWS
			date = 0;
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
			date.tv_sec = 0;
			date.tv_usec = 0;
#endif
			break;

		case VT_BSTR:
		{
#ifdef TBC_OS_WINDOWS
			bstrVal = OTSysAllocString(_T(""));
#endif
#ifdef TBC_OS_LINUX
			bstrVal = OTSysAllocString(_T(""));
#endif
		}
		break;
		}
	}
}


void Variant::initArray(
	VARTYPE datatype,
	unsigned long arraySize,
	unsigned long lBound)
{
	if (datatype & VT_ARRAY)
	{
		// initialize the array values
		SAFEARRAYBOUND dim;
		void* voidArray;
		unsigned long i;
		dim.lLbound = lBound;
		dim.cElements = arraySize;
		vt = datatype;
		parray = Variant::safeArrayCreate((VARTYPE)(vt & ~VT_ARRAY), 1, &dim);
		OTSafeArrayAccessData(parray, (void**)&voidArray);

		for (i = 0; i < arraySize; i++)
		{
			switch (vt & ~VT_ARRAY)
			{
			case VT_BOOL:
			{
				VARIANT_BOOL* pVal = ((VARIANT_BOOL*)voidArray) + i;
				*pVal = VARIANT_FALSE;
			}
			break;

			case VT_I1:
			{
				char* pVal = ((char*)voidArray) + i;
				*pVal = (char)0;
			}
			break;

			case VT_UI1:
			{
				unsigned char* pVal = ((unsigned char*)voidArray) + i;
				*pVal = (unsigned char)0;
			}
			break;

			case VT_I2:
			{
				short* pVal = ((short*)voidArray) + i;
				*pVal = (short)0;
			}
			break;

			case VT_UI2:
			{
				unsigned short* pVal = ((unsigned short*)voidArray) + i;
				*pVal = (unsigned short)0;
			}
			break;

			case VT_I4:
			{
				long* pVal = ((long*)voidArray) + i;
				*pVal = (long)0;
			}
			break;

			case VT_UI4:
			{
				unsigned long* pVal = ((unsigned long*)voidArray) + i;
				*pVal = (unsigned long)0;
			}
			break;

			case VT_I8:
			{
				LONGLONG* pVal = ((LONGLONG*)voidArray) + i;
				*pVal = (LONGLONG)0;
			}
			break;

			case VT_UI8:
			{
				ULONGLONG* pVal = ((ULONGLONG*)voidArray) + i;
				*pVal = (unsigned long)0;
			}
			break;

			case VT_INT:
			{
				int* pVal = ((int*)voidArray) + i;
				*pVal = (int)0;
			}
			break;

			case VT_UINT:
			{
				unsigned int* pVal = ((unsigned int*)voidArray) + i;
				*pVal = (unsigned int)0;
			}
			break;

			case VT_R4:
			{
				float* pVal = ((float*)voidArray) + i;
				*pVal = (float)0.0;
			}
			break;

			case VT_R8:
			{
				double* pVal = ((double*)voidArray) + i;
				*pVal = (double)0.0;
			}
			break;

			case VT_DATE:
			{
				DATE* pVal = ((DATE*)voidArray) + i;
#ifdef TBC_OS_WINDOWS
				*pVal = 0;
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
				pVal->tv_sec = 0;
				pVal->tv_usec = 0;
#endif
			}
			break;

			case VT_CY:
			{
				CY* pVal = ((CY*)voidArray) + i;
				pVal->int64 = 0;
			}
			break;

			case VT_BSTR:
			{
				BSTR* pVal = ((BSTR*)voidArray) + i;
				*pVal = OTSysAllocString(_T(""));
			}
			break;

			case VT_VARIANT:
			{
				VARIANT* pVal = ((VARIANT*)voidArray) + i;
				OTVariantInit(pVal);
			}
			break;
			}
		}

		OTSafeArrayUnaccessData(parray);
	}
	else
	{
		init(datatype);
	}
}



tstring Variant::toString(void)
{
	OTChar buffer[MAX_LENGTH_OF_VARIANT_TO_STRING] = {0};
	return OTVariantToString(this, MAX_LENGTH_OF_VARIANT_TO_STRING, buffer);
}


Variant::Variant(IN const OTChar* sz)
{
	OTVariantInit(this);

	if (!sz)
	{
		return;
	}

	vt = VT_BSTR;
	bstrVal = OTSysAllocString((OTChar*)sz);
}


Variant::Variant(IN char c)
{
	OTVariantInit(this);
	vt = VT_I1;
	cVal = c;
}


Variant::Variant(IN unsigned char b)
{
	OTVariantInit(this);
	vt = VT_UI1;
	bVal = b;
}


Variant::Variant(IN short s)
{
	OTVariantInit(this);
	vt = VT_I2;
	iVal = s;
}


Variant::Variant(IN unsigned short us)
{
	OTVariantInit(this);
	vt = VT_UI2;
	uiVal = us;
}


Variant::Variant(IN long l)
{
	OTVariantInit(this);
	vt = VT_I4;
	lVal = l;
}


Variant::Variant(IN unsigned long ul)
{
	OTVariantInit(this);
	vt = VT_UI4;
	ulVal = ul;
}


Variant::Variant(IN int i)
{
	OTVariantInit(this);
	vt = VT_INT;
	intVal = i;
}


Variant::Variant(IN unsigned int ui)
{
	OTVariantInit(this);
	vt = VT_UINT;
	uintVal = ui;
}


Variant::Variant(IN float flt)
{
	OTVariantInit(this);
	vt = VT_R4;
	fltVal = flt;
}


Variant::Variant(IN double dbl)
{
	OTVariantInit(this);
	vt = VT_R8;
	dblVal = dbl;
}


Variant::Variant(IN bool bo)
{
	OTVariantInit(this);
	vt = VT_BOOL;
	boolVal = (bo) ? VARIANT_TRUE : VARIANT_FALSE;
}


Variant::Variant(IN LONGLONG i8)
{
	OTVariantInit(this);
	vt = VT_I8;
	V_I8(this) = i8;
}


Variant::Variant(IN ULONGLONG ui8)
{
	OTVariantInit(this);
	vt = VT_UI8;
	V_UI8(this) = ui8;
}


Variant::Variant(IN CURRENCY cy)
{
	OTVariantInit(this);
	vt = VT_CY;
	V_CY(this).int64 = cy.int64;
}


Variant::~Variant()
{
	clear();
}



const Variant& Variant::operator=(IN Variant& cmnVar)
{
	initFrom((LPVARIANT)&cmnVar);
	return *this;
}



const Variant& Variant::operator=(IN const VARIANT& var)
{
	initFrom((LPVARIANT)&var);
	return *this;
}


const Variant& Variant::operator=(IN LPCVARIANT pvar)
{
	initFrom((LPVARIANT)pvar);
	return *this;
}


void Variant::setBSTR(IN const TCHAR* sz)
{
	if (sz != NULL)
	{
		clear();
		vt = VT_BSTR;
		bstrVal = OTSysAllocString((OTChar*)sz);
	}
}

TCHAR* Variant::getBSTR(TCHAR*& buffer)
{
#ifdef TBC_OS_WINDOWS
	buffer = OTSysBstrToString(bstrVal);
#endif
#ifdef TBC_OS_LINUX
	buffer = bstrVal;
#endif
	return (TCHAR*)buffer;
}


void Variant::setI1(IN char c)
{
	clear();
	vt = VT_I1;
	cVal = c;
}



void Variant::setUI1(IN unsigned char b)
{
	clear();
	vt = VT_UI1;
	bVal = b;
}


void Variant::setI2(IN short s)
{
	clear();
	vt = VT_I2;
	iVal = s;
}


void Variant::setUI2(IN unsigned short us)
{
	clear();
	vt = VT_UI2;
	iVal = us;
}


void Variant::setI4(IN long l)
{
	clear();
	vt = VT_I4;
	lVal = l;
}



void Variant::setUI4(IN unsigned long ul)
{
	clear();
	vt = VT_UI4;
	ulVal = ul;
}


void Variant::setINT(IN int i)
{
	clear();
	vt = VT_INT;
	intVal = i;
}


void Variant::setUINT(IN unsigned int ui)
{
	clear();
	vt = VT_UINT;
	uintVal = ui;
}


void Variant::setR4(IN float flt)
{
	clear();
	vt = VT_R4;
	fltVal = flt;
}


void Variant::setR8(IN double dbl)
{
	clear();
	vt = VT_R8;
	dblVal = dbl;
}


void Variant::setBOOL(IN bool bo)
{
	clear();
	vt = VT_BOOL;
	boolVal = (bo) ? VARIANT_TRUE : VARIANT_FALSE;
}


void Variant::setCY(IN CURRENCY cy)
{
	clear();
	vt = VT_CY;
	V_CY(this) = cy;
}


void Variant::setI8(IN LONGLONG i8)
{
	clear();
	vt = VT_I8;
	V_I8(this) = i8;
}


void Variant::setUI8(IN ULONGLONG ui8)
{
	clear();
	vt = VT_UI8;
	V_UI8(this) = ui8;
}


void Variant::setDATE(IN DATE dt)
{
	clear();
	vt = VT_DATE;
	date = dt;
}



#ifdef TBC_OS_LINUX
// under windows DATE is a double, which creates a overload conflict!
Variant::Variant(
	IN DATE dt)
{
	OTVariantInit(this);
	vt = VT_DATE;
	date = dt;
}
#endif



//-----------------------------------------------------------------------------
// supportDatatype
//
// - check if the datatype is one of the default datatypes supported by the toolkit
//
// returns:
//		TRUE  - datatype supported
//		FALSE - datatype not supported
//
int Variant::supportDatatype(IN VARTYPE dataType)   // data type
{
	switch (dataType)
	{
	case VT_EMPTY:
	case VT_I1:
	case VT_UI1:
	case VT_I2:
	case VT_UI2:
	case VT_I4:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
	case VT_R4:
	case VT_R8:
	case VT_BOOL:
	case VT_BSTR:
	case VT_DATE:
	case VT_CY:
	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_I8):
	case (VT_ARRAY | VT_UI8):
	case (VT_ARRAY | VT_INT):
	case (VT_ARRAY | VT_UINT):
	case (VT_ARRAY | VT_R4):
	case (VT_ARRAY | VT_R8):
	case (VT_ARRAY | VT_BOOL):
	case (VT_ARRAY | VT_BSTR):
	case (VT_ARRAY | VT_DATE):
	case (VT_ARRAY | VT_CY):
	case (VT_ARRAY | VT_VARIANT):
		return 1;

	default:
		return 0;
	}
} // supportDatatype



Variant& Variant::set(
	IN void* src,
	IN VARTYPE type,
	IN unsigned short elSize)
{
	if (((type & VT_ARRAY) == 0) && (type != VT_BSTR))
	{
		// no array and no string
		Variant::variantClear(this);
		memcpy((void*)&bVal, src, elSize);
		vt = type;
	}

	return *this;
}


unsigned long Variant::getSize(void)
{
	if ((vt & VT_ARRAY) == 0)
	{
		return getSize(vt);
	}
	else
	{
		long lBound, uBound;
		OTSafeArrayGetLBound(parray, 1, &lBound);
		OTSafeArrayGetUBound(parray, 1, &uBound);
		return (getSize((VARTYPE)(vt & ~VT_ARRAY)) * (uBound - lBound + 1));
	}
}


unsigned long Variant::getSize(VARTYPE varType)
{
	unsigned long size = 0;

	if ((varType & VT_ARRAY) == 0)
	{
		// no array and no string
		switch (varType)
		{
		case VT_I1:
		case VT_UI1:
			size = 1;
			break;

		case VT_I2:
		case VT_UI2:
		case VT_BOOL:
			size = 2;
			break;

		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_R4:
		case VT_BSTR:
			size = 4;
			break;

		case VT_R8:
		case VT_CY:
		case VT_DATE:
		case VT_I8:
		case VT_UI8:
			size = 8;
			break;

		case VT_VARIANT:
			size = 16;
			break;
		}
	}

	return size;
}



void Variant::get(IN void* dest)
{
	if ((vt & VT_ARRAY) == 0)
	{
		// no array and no string
		memcpy(dest, (void*)&bVal, getSize());
	}
	else
	{
		void* pData = NULL;
		OTSafeArrayAccessData(parray, &pData);
		memcpy(dest, pData, getSize());
		OTSafeArrayUnaccessData(parray);
	}
}


//-----------------------------------------------------------------------------
// clear
//
// - clear the variant
//
void Variant::clear(void)
{
	Variant::variantClear(this);
} // clear


//-----------------------------------------------------------------------------
// areEqual
//
// - compare to variants
//
// returns:
//		TRUE  - the variants are equal
//		FALSE - the variants are not equal
//
int Variant::areEqual(
	IN LPCVARIANT pvar1,    // first variant to compare
	IN LPCVARIANT pvar2)    // second variant to compare
{
	int equal = 0;

	if ((pvar1 == NULL) || (pvar2 == NULL))
	{
		return (pvar1 == pvar2);
	}

	if (pvar1 == pvar2)
	{
		return 1;
	}

	if (pvar1->vt != pvar2->vt)
	{
		return 0;    // no type conversion for compare till now!
	}

	switch (pvar1->vt)
	{
	case VT_EMPTY:
		return 1;
		break;

	case VT_I1:
		equal = (pvar1->cVal == pvar2->cVal);
		break;

	case VT_UI1:
		equal = (pvar1->bVal == pvar2->bVal);
		break;

	case VT_I2:
		equal = (pvar1->iVal == pvar2->iVal);
		break;

	case VT_UI2:
		equal = (pvar1->uiVal == pvar2->uiVal);
		break;

	case VT_I4:
		equal = (pvar1->lVal == pvar2->lVal);
		break;

	case VT_UI4:
		equal = (pvar1->ulVal == pvar2->ulVal);
		break;

	case VT_I8:
		equal = (V_I8(pvar1) == V_I8(pvar2));
		break;

	case VT_UI8:
		equal = (V_UI8(pvar1) == V_UI8(pvar2));
		break;

	case VT_INT:
		equal = (pvar1->intVal == pvar2->intVal);
		break;

	case VT_UINT:
		equal = (pvar1->uintVal == pvar2->uintVal);
		break;

	case VT_BOOL:
		equal = ((pvar1->boolVal == VARIANT_FALSE) == (pvar2->boolVal == VARIANT_FALSE));
		break;

	case VT_CY:
		equal = (pvar1->cyVal.int64 == pvar2->cyVal.int64);
		break;

	case VT_DATE:
		equal = (::memcmp(&pvar1->date, &pvar2->date, sizeof(DATE)) == 0);
		break;

	case VT_BSTR:
		equal = ((OTSysStringLen(pvar1->bstrVal) == OTSysStringLen(pvar2->bstrVal)) &&
				 (::memcmp(pvar1->bstrVal, pvar2->bstrVal, OTSysStringLen(pvar1->bstrVal)) == 0));
		break;

		// NOTE: compare float and double by bits, since the FP returns == for comparison
		//   between a valid real value and NAN/-EnumTraceLevel_INF/+EnumTraceLevel_INF
	case VT_R4:
		equal = (::memcmp(&pvar1->fltVal, &pvar2->fltVal, sizeof(float)) == 0);
		break;

	case VT_R8:
		equal = (::memcmp(&pvar1->dblVal, &pvar2->dblVal, sizeof(double)) == 0);
		break;

	default:
	{
		if ((pvar1->vt & VT_ARRAY) == VT_ARRAY)
		{
			equal = areEqualArrays(pvar1, pvar2);
		}
	}
	break;
	}

	return equal;
} // areEqual


//-----------------------------------------------------------------------------
// areEqualArrays
//
// - compare to variant arrays
//
// returns:
//		TRUE  - the variant arrays are equal
//		FALSE - the variant arrays are not equal
//
int Variant::areEqualArrays(
	IN LPCVARIANT pvar1,    // first variant array to compare
	IN LPCVARIANT pvar2)    // second variant array to compare
{
	if ((pvar1 == NULL) || (pvar2 == NULL))
	{
		return 0;
	}

	if ((pvar1->vt & VT_ARRAY) != VT_ARRAY)
	{
		return 0;
	}

	// check array datatypes
	VARTYPE vt1, vt2;
	vt1 = (VARTYPE)(pvar1->vt & ~VT_ARRAY);
	vt2 = (VARTYPE)(pvar2->vt & ~VT_ARRAY);

	if (vt1 != vt2)
	{
		return 0;
	}

	// check array pointers
	SAFEARRAY* parray1, *parray2;
	parray1 = pvar1->parray;
	parray2 = pvar2->parray;

	if ((parray1 == NULL) || (parray2 == NULL))
	{
		return (parray1 == parray2);
	}

	// check dimensions
	unsigned long dim1, dim2;
	dim1 = OTSafeArrayGetDim(parray1);
	dim2 = OTSafeArrayGetDim(parray2);

	if (dim1 != dim2)
	{
		return 0;
	}

	if (dim1 != 1)
	{
		return 0;    // OPC only supports array with one dimension
	}

	// check element size
	unsigned long elSize1, elSize2;
	elSize1 = OTSafeArrayGetElemsize(parray1);
	elSize2 = OTSafeArrayGetElemsize(parray2);

	if (dim1 != dim2)
	{
		return 0;
	}

	// check bounds
	long lBound1, lBound2, uBound1, uBound2;
	OTSafeArrayGetLBound(parray1, 1, &lBound1);
	OTSafeArrayGetLBound(parray2, 1, &lBound2);
	OTSafeArrayGetUBound(parray1, 1, &uBound1);
	OTSafeArrayGetUBound(parray2, 1, &uBound2);

	if ((lBound1 != lBound2) || (uBound1 != uBound2))
	{
		return 0;
	}

	// check data
	void* data1, *data2;
	unsigned char equal = 0;
	OTSafeArrayAccessData(parray1, &data1);
	OTSafeArrayAccessData(parray2, &data2);

	switch (vt1)
	{
	case VT_I1:
	case VT_UI1:
	case VT_I2:
	case VT_UI2:
	case VT_I4:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
	case VT_BOOL:
	case VT_DATE:
	case VT_CY:
	case VT_R4:
	case VT_R8:
		equal = (::memcmp(data1, data2, (uBound1 - lBound1 + 1) * elSize1) == 0);
		break;

	case VT_BSTR:
	{
		// check the strings
		BSTR* bstr1, *bstr2;
		long i;
		bstr1 = (BSTR*)data1;
		bstr2 = (BSTR*)data2;
		equal = 1;

		// compare the strings
		for (i = 0; i <= uBound1 - lBound1; i++)
		{
#ifdef TBC_OS_WINDOWS

			if (wcscmp(bstr1[i], bstr2[i]) != 0)
#endif // TBC_OS_WINDOWS
#ifdef TBC_OS_LINUX
				if (strcmp(bstr1[i], bstr2[i]) != 0)
#endif
				{
					equal = 0;
					break;
				}
		}
	}
	break;

	case VT_VARIANT:
	{
		LPVARIANT pv1, pv2;
		long i;
		pv1 = (LPVARIANT)data1;
		pv2 = (LPVARIANT)data2;
		equal = 1;

		for (i = 0; i <= uBound1 - lBound1; i++)
		{
			if (!areEqual(pv1, pv2))
			{
				equal = 0;
				break;
			}

			pv1++;
			pv2++;
		}
	}
	break;
	}

	OTSafeArrayUnaccessData(parray1);
	OTSafeArrayUnaccessData(parray2);
	return equal;
} // areEqualArrays


//-----------------------------------------------------------------------------
// changeDatatype
//
// - change the datatype of a value
//
// returns:
//		TRUE  - variant copied and datatype changed
//		FALSE - can't change datatype
//
int Variant::changeDatatype(
	VARIANT* value,         // source value
	VARTYPE type,           // wanted datatype
	VARIANT* changedValue,// destination value
	long* pError)   // conversion error
{
	return OTVariantChangeType(value, changedValue, 0, type);
} // changeDatatype



//-----------------------------------------------------------------------------
// changeDatatype
//
// - change the datatype of a value
//
// returns:
//		TRUE  - variant copied and datatype changed
//		FALSE - can't change datatype
//
int Variant::changeDatatype(
	VARIANT* value,         // source value
	VARTYPE type,           // wanted datatype
	unsigned long localeID,         // locale id
	VARIANT* changedValue,  // destination value
	long* pError)   // conversion error
{
	if ((!value) || (!changedValue))
	{
		if (pError)
		{
			*pError = VARIANT_ERROR_BADTYPE;
		}

		return 0; // bad input parameter
	}

	if ((value->vt & VT_ARRAY) != (type & VT_ARRAY))
	{
		if (pError)
		{
			*pError = VARIANT_ERROR_BADTYPE;
		}

		return 0; // can not convert simple type to array vice versa
	}

	int ret = 1;

	if ((type & VT_ARRAY) == 0)
	{
		// no array
		unsigned long bothTypes = MAKELONG(value->vt, type);    // from - low, to - high
		Variant to;

		switch (bothTypes)
		{
		case MAKELONG(VT_BOOL, VT_BSTR):
			if (value->boolVal == VARIANT_FALSE)
			{
				to = _T("0");
			}
			else
			{
				to = _T("-1");
			}

			Variant::variantClear(changedValue);
			to.copyTo(changedValue);
			break;

		case MAKELONG(VT_I1, VT_UI1):
			if ((value->cVal & 0x80) == 0)
			{
				to = (unsigned char)value->cVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_UI1, VT_I1):
			if ((value->bVal & 0x80) == 0)
			{
				to = (char)value->bVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_I2, VT_UI2):
			if ((value->iVal & 0x8000) == 0)
			{
				to = (unsigned short)value->iVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_UI2, VT_I2):
			if ((value->uiVal & 0x8000) == 0)
			{
				to = (short)value->uiVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_I4, VT_UI4):
		case MAKELONG(VT_I4, VT_UINT):
		case MAKELONG(VT_INT, VT_UI4):
		case MAKELONG(VT_INT, VT_UINT):
			if ((value->lVal & 0x80000000) == 0)
			{
				to = (unsigned long)value->lVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_UI4, VT_I4):
		case MAKELONG(VT_UI4, VT_INT):
		case MAKELONG(VT_UINT, VT_I4):
		case MAKELONG(VT_UINT, VT_INT):
			if ((value->ulVal & 0x80000000) == 0)
			{
				to = (long)value->ulVal;
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = EnumResultCode_DISP_E_OVERFLOW;
				}

				ret = 0;
			}

			break;

		case MAKELONG(VT_CY, VT_UI4):
		case MAKELONG(VT_CY, VT_I4):
		case MAKELONG(VT_CY, VT_UINT):
		case MAKELONG(VT_CY, VT_INT):
		{
			long vctx;
			ret = ((vctx = OTVariantChangeType(changedValue, value, 0, type)) == S_OK);

			if ((!ret) && (pError))
			{
				*pError = vctx;
			}
		}
		break;
#ifdef SOFEATURE_CMN_TIME

		case MAKELONG(VT_BSTR, VT_DATE):
		{
			SOCmnDateTime dateT;
			LPTSTR dateSZ = OTSysBstrToString(value->bstrVal);

			if (dateT.setXMLDateTime(dateSZ))
			{
				to.vt = VT_DATE;
				dateT.get(&to.date);
				Variant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				long vctx;
				ret = ((vctx = OTVariantChangeType(changedValue, value, 0, type)) == S_OK);

				if ((!ret) && (pError))
				{
					*pError = vctx;
				}
			}
		}
		break;
#endif

		default:
		{
			long vctx;
			ret = ((vctx = OTVariantChangeType(changedValue, value, 0, type)) == S_OK);

			if ((!ret) && (pError))
			{
				*pError = vctx;
			}
		}
		}
	}
	else
	{
		// array
		Variant to;
		SAFEARRAYBOUND dim;
		void* dataS, *dataD;
		unsigned long i;
		long lBound, uBound;
		Variant elS, elD;
		VARTYPE elTypeS = (VARTYPE)(value->vt & ~VT_ARRAY);
		VARTYPE elTypeD = (VARTYPE)(type & ~VT_ARRAY);
		unsigned short elSizeS, elSizeD;
		OTSafeArrayGetLBound(value->parray, 1, &lBound);
		OTSafeArrayGetUBound(value->parray, 1, &uBound);
		dim.lLbound = lBound;
		dim.cElements = uBound - lBound + 1;
		to.vt = type;
		to.parray = Variant::safeArrayCreate((VARTYPE)(type & ~VT_ARRAY), 1, &dim);
		elSizeS = (unsigned short)OTSafeArrayGetElemsize(value->parray);
		elSizeD = (unsigned short)OTSafeArrayGetElemsize(to.parray);
		OTSafeArrayAccessData(value->parray, &dataS);
		OTSafeArrayAccessData(to.parray, &dataD);
		memset(dataD, 0, dim.cElements * elSizeD);

		for (i = 0; i < dim.cElements; i++)
		{
			if (elTypeS != VT_VARIANT)
			{
				if (elTypeS != VT_BSTR)
				{
					elS.set(((unsigned char*)dataS) + (elSizeS * i), (VARTYPE)(value->vt & ~VT_ARRAY), elSizeS);
				}
				else
#ifdef TBC_OS_WINDOWS
					elS = OTSysBstrToString(*(BSTR*)(((unsigned char*)dataS) + (elSizeS * i)));

#endif
#ifdef TBC_OS_LINUX
				elS = *(BSTR*)(((unsigned char*)dataS) + (elSizeS * i));
#endif
			}
			else
			{
				elS = (LPCVARIANT)((unsigned char*)dataS + (elSizeS * i));
			}

			elD.clear();

			if (elTypeD != VT_VARIANT)
			{
				ret = changeDatatype(elS, (VARTYPE)(type & ~VT_ARRAY), localeID, elD, pError);

				if (!ret)
				{
					break;
				}

				if ((type & ~VT_ARRAY) != VT_BSTR)
				{
					memcpy(((unsigned char*)dataD) + (elSizeD * i), (const void*)&elD.bVal, elSizeD);
				}
				else
				{
#ifdef TBC_OS_WINDOWS
					*(BSTR*)(((unsigned char*)dataD) + (elSizeD * i)) = OTSysAllocString(OTSysBstrToString(elD.bstrVal));
#endif
#ifdef TBC_OS_LINUX
					*(BSTR*)(((unsigned char*)dataD) + (elSizeD * i)) = OTSysAllocString(elD.bstrVal);
#endif
				}
			}
			else
			{
				long vc;
				ret = ((vc = Variant::variantCopy((LPVARIANT)((unsigned char*)dataD + (elSizeD * i)), elS)) == S_OK);

				if (!ret)
				{
					*pError = vc;
					break;
				}
			}
		}

		OTSafeArrayUnaccessData(value->parray);
		OTSafeArrayUnaccessData(to.parray);

		if (ret)
		{
			Variant::variantClear(changedValue);
			to.copyTo(changedValue);
		}
	}

	return ret;
} // changeDatatype


long Variant::variantClear(LPVARIANT pvargDest)
{
	if ((pvargDest->vt == VT_UI8) || (pvargDest->vt == VT_I8) ||
		(pvargDest->vt == (VT_UI8 | VT_ARRAY)) || (pvargDest->vt == (VT_I8 | VT_ARRAY)))
	{
		if (pvargDest->vt & VT_ARRAY)
		{
			OTSafeArrayDestroy(pvargDest->parray);
		}

		OTVariantInit(pvargDest);
		return S_OK;
	}

	return OTVariantClear(pvargDest);
}


long Variant::variantCopy(
	LPVARIANT pvargDest,
	LPVARIANT pvargSrc)
{
	if (pvargSrc != NULL && (((pvargSrc->vt & ~VT_ARRAY) == VT_UI8) || ((pvargSrc->vt & ~VT_ARRAY) == VT_I8)))
	{
		if (pvargDest == NULL || pvargSrc == NULL)
		{
			return EnumResultCode_E_INVALIDARG;
		}

		if (pvargDest == pvargSrc)
		{
			return S_OK;
		}

		long retval = Variant::variantClear(pvargDest);

		if (FAILED(retval))
		{
			return retval;
		}

		*pvargDest = *pvargSrc;

		if (pvargSrc->vt & VT_ARRAY)
		{
			retval = OTSafeArrayCopy(pvargSrc->parray, &pvargDest->parray);
		}

		if (FAILED(retval))
		{
			pvargDest->vt = VT_EMPTY;
		}

		return retval;
	}

	if (pvargDest != NULL && ((pvargDest->vt & ~VT_ARRAY) == VT_UI8 || (pvargDest->vt & ~VT_ARRAY) == VT_I8))
	{
		long retval = Variant::variantClear(pvargDest);

		if (FAILED(retval))
		{
			return retval;
		}
	}

	return OTVariantCopy(pvargDest, pvargSrc);
}


LPSAFEARRAY Variant::safeArrayCreate(
	VARTYPE vt,
	unsigned short cDims,
	LPSAFEARRAYBOUND rgsabound)
{
	return OTSafeArrayCreate(vt, cDims, rgsabound);
}


LPSAFEARRAY Variant::safeArrayCreateVector(
	VARTYPE vt,
	long lLbound,
	unsigned long cElements)
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = lLbound;
	rgsabound[0].cElements = cElements;
	return Variant::safeArrayCreate(vt, 1, rgsabound);
}

long Variant::SafeArrayAccessData(
	LPSAFEARRAY psa,
	void** ppvData)
{
	return OTSafeArrayAccessData(psa, ppvData);
}

long Variant::SafeArrayUnaccessData(
	LPSAFEARRAY psa)
{
	return OTSafeArrayUnaccessData(psa);
}

long Variant::SafeArrayGetUBound(
	LPSAFEARRAY psa,
	unsigned short nDim,
	long* plUbound)
{
	return OTSafeArrayGetUBound(psa, nDim, plUbound);
}

long Variant::SafeArrayGetLBound(
	LPSAFEARRAY psa,
	unsigned short nDim,
	long* plLbound)
{
	return OTSafeArrayGetLBound(psa, nDim, plLbound);
}

int Variant::copyTo(OUT LPVARIANT destination) const
{
	OTVariantInit(destination);
	return (Variant::variantCopy(destination, (LPVARIANT)this) == S_OK);
}


int Variant::initFrom(IN LPCVARIANT source)
{
	Variant::variantClear(this);
	return (Variant::variantCopy((LPVARIANT)this, (LPVARIANT)source) == S_OK);
}


#ifdef SOCMNVARIANT_IMP_I64

#define FL_UNSIGNED   1       /* strtouq called */
#define FL_NEG        2       /* negative sign found */
#define FL_OVERFLOW   4       /* overflow occured */
#define FL_READDIGIT  8       /* we've read at least one correct digit */


static unsigned __int64
strtoxq(
	const char* nptr,
	char** endptr,
	int ibase,
	int flags)
{
	const char* p;
	char c;
	unsigned __int64 number;
	unsigned digval;
	unsigned __int64 maxval;
	p = nptr;            /* p is our scanning pointer */
	number = 0;            /* start with zero */
	c = *p++;            /* read char */

	while (isspace((int)(unsigned char)c))
	{
		c = *p++;    /* skip whitespace */
	}

	if (c == '-')
	{
		flags |= FL_NEG;    /* remember minus sign */
		c = *p++;
	}
	else if (c == '+')
	{
		c = *p++;    /* skip sign */
	}

	if (ibase < 0 || ibase == 1 || ibase > 36)
	{
		/* bad base! */
		if (endptr)
			/* store beginning of string in endptr */
		{
			*endptr = (char*)nptr;
		}

		return 0L;        /* return 0 */
	}
	else if (ibase == 0)
	{
		/* determine base free-lance, based on first two chars of
		   string */
		if (c != '0')
		{
			ibase = 10;
		}
		else if (*p == 'x' || *p == 'X')
		{
			ibase = 16;
		}
		else
		{
			ibase = 8;
		}
	}

	if (ibase == 16)
	{
		/* we might have 0x in front of number; remove if there */
		if (c == '0' && (*p == 'x' || *p == 'X'))
		{
			++p;
			c = *p++;    /* advance past prefix */
		}
	}

	/* if our number exceeds this, we will overflow on multiply */
	maxval = _UI64_MAX / ibase;

	for (;;)      /* exit in middle of loop */
	{
		/* convert c to value */
		if (isdigit((int)(unsigned char)c))
		{
			digval = c - '0';
		}
		else if (isalpha((int)(unsigned char)c))
		{
			digval = toupper(c) - 'A' + 10;
		}
		else
		{
			break;
		}

		if (digval >= (unsigned)ibase)
		{
			break;    /* exit loop if bad digit found */
		}

		/* record the fact we have read one digit */
		flags |= FL_READDIGIT;

		/* we now need to compute number = number * base + digval,
		   but we need to know if overflow occured.  This requires
		   a tricky pre-check. */

		if (number < maxval || (number == maxval &&
								(unsigned __int64)digval <= _UI64_MAX % ibase))
		{
			/* we won't overflow, go ahead and multiply */
			number = number * ibase + digval;
		}
		else
		{
			/* we would have overflowed -- set the overflow flag */
			flags |= FL_OVERFLOW;
		}

		c = *p++;        /* read next digit */
	}

	--p;                /* point to place that stopped scan */

	if (!(flags & FL_READDIGIT))
	{
		/* no number there; return 0 and point to beginning of
		   string */
		if (endptr)
			/* store beginning of string in endptr later on */
		{
			p = nptr;
		}

		number = 0L;        /* return 0 */
	}
	else if ((flags & FL_OVERFLOW) ||
			 (!(flags & FL_UNSIGNED) &&
			  (((flags & FL_NEG) && (number > -_I64_MIN)) ||
			   (!(flags & FL_NEG) && (number > _I64_MAX)))))
	{
		/* overflow or signed overflow occurred */
		errno = ERANGE;

		if (flags & FL_UNSIGNED)
		{
			number = _UI64_MAX;
		}
		else if (flags & FL_NEG)
		{
			number = _I64_MIN;
		}
		else
		{
			number = _I64_MAX;
		}
	}

	if (endptr != NULL)
		/* store pointer to char that stopped the scan */
	{
		*endptr = (char*)p;
	}

	if (flags & FL_NEG)
		/* negate result if there was a neg sign */
	{
		number = (unsigned __int64)(-(__int64)number);
	}

	return number;            /* done. */
}

__int64
_strtoi64(
	const char* nptr,
	char** endptr,
	int ibase)
{
	return (__int64) strtoxq(nptr, endptr, ibase, 0);
}



unsigned __int64
_strtoui64(
	const char* nptr,
	char** endptr,
	int ibase)
{
	return strtoxq(nptr, endptr, ibase, FL_UNSIGNED);
}



int
_wchartodigit(
	wchar_t ch)
{
#define DIGIT_RANGE_TEST(zero)  \
    if (ch < zero)              \
        return -1;              \
    if (ch < zero + 10)         \
    {                           \
        return ch - zero;       \
    }
	DIGIT_RANGE_TEST(0x0030)        // 0030;DIGIT ZERO

	if (ch < 0xFF10)                // FF10;FULLWIDTH DIGIT ZERO
	{
		DIGIT_RANGE_TEST(0x0660)    // 0660;ARABIC-INDIC DIGIT ZERO
		DIGIT_RANGE_TEST(0x06F0)    // 06F0;EXTENDED ARABIC-INDIC DIGIT ZERO
		DIGIT_RANGE_TEST(0x0966)    // 0966;DEVANAGARI DIGIT ZERO
		DIGIT_RANGE_TEST(0x09E6)    // 09E6;BENGALI DIGIT ZERO
		DIGIT_RANGE_TEST(0x0A66)    // 0A66;GURMUKHI DIGIT ZERO
		DIGIT_RANGE_TEST(0x0AE6)    // 0AE6;GUJARATI DIGIT ZERO
		DIGIT_RANGE_TEST(0x0B66)    // 0B66;ORIYA DIGIT ZERO
		DIGIT_RANGE_TEST(0x0C66)    // 0C66;TELUGU DIGIT ZERO
		DIGIT_RANGE_TEST(0x0CE6)    // 0CE6;KANNADA DIGIT ZERO
		DIGIT_RANGE_TEST(0x0D66)    // 0D66;MALAYALAM DIGIT ZERO
		DIGIT_RANGE_TEST(0x0E50)    // 0E50;THAI DIGIT ZERO
		DIGIT_RANGE_TEST(0x0ED0)    // 0ED0;LAO DIGIT ZERO
		DIGIT_RANGE_TEST(0x0F20)    // 0F20;TIBETAN DIGIT ZERO
		DIGIT_RANGE_TEST(0x1040)    // 1040;MYANMAR DIGIT ZERO
		DIGIT_RANGE_TEST(0x17E0)    // 17E0;KHMER DIGIT ZERO
		DIGIT_RANGE_TEST(0x1810)    // 1810;MONGOLIAN DIGIT ZERO
		return -1;
	}

#undef DIGIT_RANGE_TEST

	// FF10;FULLWIDTH DIGIT ZERO
	if (ch < 0xFF10 + 10)
	{
		return ch - 0xFF10;
	}

	return -1;
}



static unsigned __int64
wcstoxq(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase,
	int flags)
{
	const wchar_t* p;
	wchar_t c;
	unsigned __int64 number;
	unsigned digval;
	unsigned __int64 maxval;
	p = nptr;                       /* p is our scanning pointer */
	number = 0;                     /* start with zero */
	c = *p++;                       /* read wchar_t */

	while (iswspace(c))
	{
		c = *p++;    /* skip whitespace */
	}

	if (c == '-')
	{
		flags |= FL_NEG;        /* remember minus sign */
		c = *p++;
	}
	else if (c == '+')
	{
		c = *p++;    /* skip sign */
	}

	if (ibase < 0 || ibase == 1 || ibase > 36)
	{
		/* bad base! */
		if (endptr)
			/* store beginning of string in endptr */
		{
			*endptr = (wchar_t*)nptr;
		}

		return 0L;              /* return 0 */
	}
	else if (ibase == 0)
	{
		/* determine base free-lance, based on first two chars of
		   string */
		if (_wchartodigit(c) != 0)
		{
			ibase = 10;
		}
		else if (*p == 'x' || *p == 'X')
		{
			ibase = 16;
		}
		else
		{
			ibase = 8;
		}
	}

	if (ibase == 16)
	{
		/* we might have 0x in front of number; remove if there */
		if (_wchartodigit(c) == 0 && (*p == L'x' || *p == L'X'))
		{
			++p;
			c = *p++;       /* advance past prefix */
		}
	}

	/* if our number exceeds this, we will overflow on multiply */
	maxval = _UI64_MAX / ibase;

	for (;;)        /* exit in middle of loop */
	{
		/* convert c to value */
		if ((digval = _wchartodigit(c)) != -1)
			;
		else if ((('A' <= (c) && (c) <= 'Z') || ('a' <= (c) && (c) <= 'z')))
		{
			digval = toupper(c) - 'A' + 10;
		}
		else
		{
			break;
		}

		if (digval >= (unsigned)ibase)
		{
			break;    /* exit loop if bad digit found */
		}

		/* record the fact we have read one digit */
		flags |= FL_READDIGIT;

		/* we now need to compute number = number * base + digval,
		   but we need to know if overflow occured.  This requires
		   a tricky pre-check. */

		if (number < maxval || (number == maxval &&
								(unsigned __int64)digval <= _UI64_MAX % ibase))
		{
			/* we won't overflow, go ahead and multiply */
			number = number * ibase + digval;
		}
		else
		{
			/* we would have overflowed -- set the overflow flag */
			flags |= FL_OVERFLOW;
		}

		c = *p++;               /* read next digit */
	}

	--p;                            /* point to place that stopped scan */

	if (!(flags & FL_READDIGIT))
	{
		/* no number there; return 0 and point to beginning of
		   string */
		if (endptr)
			/* store beginning of string in endptr later on */
		{
			p = nptr;
		}

		number = 0L;            /* return 0 */
	}
	else if ((flags & FL_OVERFLOW) ||
			 (!(flags & FL_UNSIGNED) &&
			  (((flags & FL_NEG) && (number > -_I64_MIN)) ||
			   (!(flags & FL_NEG) && (number > _I64_MAX)))))
	{
		/* overflow or signed overflow occurred */
		errno = ERANGE;

		if (flags & FL_UNSIGNED)
		{
			number = _UI64_MAX;
		}
		else if (flags & FL_NEG)
		{
			number = (_I64_MIN);
		}
		else
		{
			number = _I64_MAX;
		}
	}

	if (endptr != NULL)
		/* store pointer to wchar_t that stopped the scan */
	{
		*endptr = (wchar_t*)p;
	}

	if (flags & FL_NEG)
		/* negate result if there was a neg sign */
	{
		number = (unsigned __int64)(-(__int64)number);
	}

	return number;                  /* done. */
}



__int64
_wcstoi64(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase)
{
	return (__int64) wcstoxq(nptr, endptr, ibase, 0);
}



unsigned __int64
_wcstoui64(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase)
{
	return wcstoxq(nptr, endptr, ibase, FL_UNSIGNED);
}

#endif // SOCMNVARIANT_IMP_I64
