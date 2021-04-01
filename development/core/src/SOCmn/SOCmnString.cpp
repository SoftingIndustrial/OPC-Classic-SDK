#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnString.h>

#ifdef SOOS_WINDOWS_CE
#if _MSC_VER < 1400 // Microsoft Visual Studio 2005
#define _tcsdec      _wcsdec
#define _tcsinc      _wcsinc
__inline wchar_t* __cdecl _wcsdec(const wchar_t* start, const wchar_t* current)
{
	return (wchar_t*)((start >= current) ? NULL : (current - 1));
}
__inline wchar_t* __cdecl _wcsinc(const wchar_t* _pc)
{
	return (wchar_t*)(_pc + 1);
}
#endif
#endif

static TCHAR g_emptyString[2] = _T("");

//-----------------------------------------------------------------------------
// SOCmnString                                                                |
//-----------------------------------------------------------------------------

SOCmnString::SOCmnString()
{
	m_string = NULL;
}

SOCmnString::SOCmnString(
	IN LPCTSTR sz)  // string pointer
{
	m_string = NULL;

	if (sz)
	{
		m_string = _tcsdup(sz);
	}
}

SOCmnString::SOCmnString(
	IN TCHAR c) // one char
{
	TCHAR sz[2];
	sz[0] = c;
	sz[1] = _T('\0');
	m_string = _tcsdup(sz);
}

SOCmnString::SOCmnString(
	IN const SOCmnString* str)  // string object pointer
{
	m_string = NULL;

	if (str)
	{
		LPCTSTR sz = (LPCTSTR) * str;

		if (sz)
		{
			m_string = _tcsdup(sz);
		}
	}
}

SOCmnString::SOCmnString(
	IN const SOCmnString& str)  // string object
{
	LPCTSTR sz = (LPCTSTR)str;
	m_string = NULL;

	if (sz)
	{
		m_string = _tcsdup(sz);
	}
}

SOCmnString::~SOCmnString()
{
	setData(NULL);
}

SOCmnString::operator LPCTSTR() const
{
	if (m_string)
	{
		return m_string;
	}
	else
	{
		return g_emptyString;
	}
}

SOCmnString::operator LPTSTR()
{
	if (m_string)
	{
		return m_string;
	}
	else
	{
		return g_emptyString;
	}
}



//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator+=(
	IN const SOCmnString& str)
{
	LPCTSTR sz = (LPCTSTR)str;

	if (sz)
	{
		setData(addData(sz, (DWORD)_tcslen(sz)));
	}

	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator+=(
	IN LPCTSTR sz)  // string pointer
{
	if (sz)
	{
		setData(addData(sz, (DWORD)_tcslen(sz)));
	}

	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator+=(
	IN TCHAR c) // one char
{
	setData(addData((LPCTSTR)&c, 1));
	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator=(
	IN const SOCmnString& str)  // string object
{
	LPCTSTR sz = (LPCTSTR)str;

	if (sz)
	{
		sz = _tcsdup(sz);
	}

	setData((LPTSTR)sz);
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator=(
	IN LPCTSTR sz)  // string pointer
{
	LPTSTR s = NULL;

	if (sz)
	{
		s = _tcsdup(sz);
	}

	setData(s);
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnString& SOCmnString::operator=(
	IN TCHAR c) // one char
{
	TCHAR sz[2];
	sz[0] = c;
	sz[1] = _T('\0');
	setData(_tcsdup(sz));
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnString SOCmnString::operator+(
	IN const SOCmnString& str)  // string object
{
	LPCTSTR sz = (LPCTSTR)str;
	SOCmnString s;

	if (sz)
	{
		s.setData(addData(sz, (DWORD)_tcslen(sz)));
	}
	else
	{
		s.setData(addData(NULL, 0));
	}

	return s;
} // operator +


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnString SOCmnString::operator+(
	IN LPCTSTR sz)  // string pointer
{
	SOCmnString s;

	if (sz)
	{
		s.setData(addData(sz, (DWORD)_tcslen(sz)));
	}
	else
	{
		s.setData(addData(NULL, 0));
	}

	return s;
} // operator +


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnString SOCmnString::operator+(
	IN TCHAR c) // one char
{
	SOCmnString s;
	s.setData(addData(&c, 1));
	return s;
} // operator +


//-----------------------------------------------------------------------------
// getLength
//
// - get the length of the string
//
// returns:
//      number of elements of the string
//
DWORD SOCmnString::getLength(void)
const
{
	if (m_string == NULL)
	{
		return 0;
	}

	return (DWORD)_tcslen(m_string);
} // getLength


//-----------------------------------------------------------------------------
// isEmpty
//
// - check if the string is empty
//
// returns:
//      TRUE  - string is empty
//      FALSE - string is not empty
//
BOOL SOCmnString::isEmpty(void)
const
{
	if (m_string == NULL)
	{
		return TRUE;
	}

	if (_tcslen(m_string) == 0)
	{
		return TRUE;
	}

	return FALSE;
} // isEmpty


//-----------------------------------------------------------------------------
// empty
//
// - empty the string
//
void SOCmnString::empty(void)
{
	setData(NULL);
} // empty


//-----------------------------------------------------------------------------
// helper function mem_printf
// prints a string of unknown size
// and allocates memory as needed
//
static LPTSTR mem_vsprintf(LPCTSTR fmt, va_list ap)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	TCHAR* p;

	if ((p = (TCHAR*)malloc(size * sizeof(TCHAR))) == NULL)
	{
		return NULL;
	}

	while (1)
	{
		/* Try to print in the allocated space. */
		va_list cp;
#ifdef SOOS_LINUX
		va_copy(cp, ap);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		cp = ap;
#endif
		n = _vsntprintf(p, size, fmt, cp);
		va_end(cp);

		/* If that worked, return the string. */
		if (n > -1 && n < size)
		{
			return p;
		}

		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
		{
			size = n + 1;    /* precisely what is needed */
		}
		else           /* glibc 2.0 */
		{
			size *= 2;    /* twice the old size */
		}

		if ((p = (TCHAR*)realloc(p, size * sizeof(TCHAR))) == NULL)
		{
			return NULL;
		}
	}
}


//-----------------------------------------------------------------------------
// Format
//
// - set the string data with format
//
void SOCmnString::Format(
	IN LPCTSTR format,  // format
	...)                // data
{
	va_list arglist;
	va_start(arglist, format);
	setData(mem_vsprintf(format, arglist));
	va_end(arglist);
	return;
} // Format


//-----------------------------------------------------------------------------
// format
//
// - set the string data with format
//
LPCTSTR SOCmnString::format(
	IN LPCTSTR format,  // format
	...)                // data
{
	va_list arglist;
	va_start(arglist, format);
	setData(mem_vsprintf(format, arglist));
	va_end(arglist);
	return *this;
} // format

//-----------------------------------------------------------------------------
// replace
//
// - replace indicated characters with other characters
//
// returns:
//      number of replaced substrings
//
int SOCmnString::replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew)
{
	if (m_string == NULL)
	{
		return 0;
	}

	// can't work with empty or NULL pszOld
	int iSourceLen = (DWORD)_tcslen(pszOld);

	if (iSourceLen == 0)
	{
		return 0;
	}

	int iReplaceLen = (DWORD)_tcslen(pszNew);
	// loop once to figure out the size of the result string
	int nCount = 0;
	LPTSTR pszStart = m_string;
	LPTSTR pszEnd = m_string + getLength();
	LPTSTR pszTarget;

	// just count substrings
	while (pszStart < pszEnd)
	{
		while ((pszTarget = _tcsstr(pszStart, pszOld)) != NULL)
		{
			nCount++;
			pszStart = pszTarget + iSourceLen;
		}

		pszStart += lstrlen(pszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		int ibuffer = (((getLength() - (nCount * (iSourceLen - iReplaceLen))) + 1) * sizeof(TCHAR));
		TCHAR* buffer = (TCHAR*)malloc(ibuffer);
		pszStart = m_string;
		pszEnd = m_string + getLength();
		// loop again to actually do the work
		TCHAR* tmpbuffer = buffer;

		while (pszStart < pszEnd)
		{
			while ((pszTarget = _tcsstr(pszStart, pszOld)) != NULL)
			{
				// copy string until substring to buffer
				memcpy(tmpbuffer, pszStart, (pszTarget - pszStart)*sizeof(TCHAR));
				tmpbuffer += pszTarget - pszStart;
				// copy new string in buffer
				memcpy(tmpbuffer, pszNew, iReplaceLen * sizeof(TCHAR));
				tmpbuffer += iReplaceLen;
				// set new start position behind found substring
				pszStart = pszTarget + iSourceLen;
			}

			// copy missing tail to buffer
			memcpy(tmpbuffer, pszStart, (pszEnd - pszStart)*sizeof(TCHAR));
			// set final '\0' character
			memset(tmpbuffer + lstrlen(pszStart), _T('\0'), sizeof(TCHAR));
			pszStart += lstrlen(pszStart) + 1;
		}

		setData(buffer);
	}

	return nCount;
} // replace


//-----------------------------------------------------------------------------
// replace
//
// - replace indicated character with other character
//
// returns:
//      number of replaced characters
//
int SOCmnString::replace(IN TCHAR oldChar, IN TCHAR newChar, OPTIONAL IN DWORD offset)
{
	if (m_string == NULL)
	{
		return 0;
	}

	if (offset != 0)
		if (offset >= _tcslen(m_string))
		{
			return 0;
		}

	int count = 0;
	TCHAR* pOldChar = m_string + offset;

	while ((pOldChar = _tcschr(pOldChar, oldChar)) != NULL)
	{
		*pOldChar = newChar;
		pOldChar++;
		count++;
	}

	return count;
}


//-----------------------------------------------------------------------------
// replace
//
// - replace indicated character with other character
//
// returns:
//      number of replaced characters
//
int SOCmnString::replaceWithEscape(IN TCHAR oldChar, IN TCHAR newChar, OPTIONAL IN DWORD offset)
{
	if (m_string == NULL)
	{
		return 0;
	}

	if (offset != 0)
		if (offset >= _tcslen(m_string))
		{
			return 0;
		}

	int count = 0;
	TCHAR* pOldChar = m_string + offset;

	while ((pOldChar = _tcschr(pOldChar, oldChar)) != NULL)
	{
		if (*(pOldChar + 1) != oldChar)
		{
			*pOldChar = newChar;
			count++;
		}
		else
		{
			pOldChar++;    // ignore both occurances
		}

		pOldChar++;
	}

	return count;
}


//-----------------------------------------------------------------------------
// makeLower
//
// - covert string to lowercase
//
void SOCmnString::makeLower(
	IN OPTIONAL DWORD startIdx) // start index of operation
{
	DWORD curSize = getLength();
	DWORD i;

	for (i = startIdx; i < curSize; i++)
	{
		m_string[i] = (TCHAR)_totlower(m_string[i]);
	}
} // makeLower


//-----------------------------------------------------------------------------
// makeUpper
//
// - covert string to uppercase
//
void SOCmnString::makeUpper(
	IN OPTIONAL DWORD startIdx) // start index of operation
{
	DWORD curSize = getLength();
	DWORD i;

	for (i = startIdx; i < curSize; i++)
	{
		m_string[i] = (TCHAR)_totupper(m_string[i]);
	}
} // makeUpper


//-----------------------------------------------------------------------------
// getBuffer
//
// - get the data buffer of the string
//
// returns:
//      data buffer
//
LPTSTR SOCmnString::getBuffer(
	IN DWORD minBufferLen)  // minimal size of data buffer
{
	DWORD curSize = getLength();
	TCHAR* newStr;

	if (curSize < minBufferLen + 1)
	{
		newStr = (TCHAR*)malloc((minBufferLen + 1) * sizeof(TCHAR));
		memcpy(newStr, m_string, curSize * sizeof(TCHAR));
		newStr[curSize] = _T('\0');
		setData(newStr);
	}

	return m_string;
} // getBuffer


#ifdef SOOS_WINDOWS
SOCmnString::SOCmnString(LPOLESTR szOle)
{
	USES_CONVERSION;
	m_string = NULL;

	if (szOle)
	{
		m_string = _tcsdup(OLE2T(szOle));
	}
}

void SOCmnString::fromWCHAR(LPCWSTR wszSrc)
{
	USES_CONVERSION;
	setData(wszSrc ? _tcsdup(W2CT(wszSrc)) : NULL);
}

LPWSTR SOCmnString::cloneToWCHAR(void)
{
	return cloneToWCHAR(*this);
}

LPWSTR SOCmnString::cloneToWCHAR(IN const SOCmnString& str)
{
	USES_CONVERSION;
	DWORD strLen;
	LPWSTR wstr;
	strLen = str.getLength();
	wstr = (LPWSTR)CoTaskMemAlloc((strLen + 1) * 2);

	if (!wstr)
	{
		return NULL;
	}

	memset(wstr, 0, 2 * (strLen + 1));

	if (strLen)
	{
#if defined _UNICODE || defined UNICODE
		// when using UNICODE no real conversion is done
		// no stack overflow can happen
		wcscpy(wstr, T2CW((LPCTSTR)str));
#else
		// We use the follwing function since with big configurations
		// a stack overflow can appear using ATL conversion macros!
		::MultiByteToWideChar(GetACP(), 0, str, -1, wstr, strLen + 1);
#endif
	}

	return wstr;
}

#define BUFFERLENGTH    512
int SOCmnString::loadString(HINSTANCE hInstance, UINT nID)
{
	DWORD loop = 1;
	TCHAR* pszCMD = new TCHAR[BUFFERLENGTH];
	DWORD charCnt = ::LoadString(hInstance, nID, pszCMD, BUFFERLENGTH);

	while (charCnt >= (BUFFERLENGTH * loop - 1))
	{
		delete [] pszCMD;
		loop++;
		pszCMD = new TCHAR[BUFFERLENGTH * loop];
		charCnt = ::LoadString(hInstance, nID, pszCMD, BUFFERLENGTH * loop);
	}

	*this = pszCMD;
	delete [] pszCMD;
	return charCnt;
}

#endif // SOOS_WINDOWS


//-----------------------------------------------------------------------------
// areEqual
//
// - check if two strings are equal
//
// returns:
//      TRUE  - strings are equal
//      FALSE - strings are different
//
BOOL SOCmnString::areEqual(
	IN LPCTSTR sz1, // first string
	IN LPCTSTR sz2) // second string
{
	if ((sz1 == NULL) && (sz2 == NULL))
	{
		return TRUE;
	}

	if ((sz1 == NULL) || (sz2 == NULL))
	{
		if (sz1 == NULL)
		{
			return (_tcslen(sz2) == 0);
		}
		else
		{
			return (_tcslen(sz1) == 0);
		}
	}

	return (_tcscmp(sz1, sz2) == 0);
} // areEqual


//-----------------------------------------------------------------------------
// find
//
// - find a character in current string
//
// returns:
//      -1     - if the character isn't found
//      >(-1)  - 0 based index of the character in the string
//
int SOCmnString::find(
	IN TCHAR ch,
	IN OPTIONAL int start) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	int length = (DWORD)_tcslen(m_string);

	if (start > length)
	{
		return -1;
	}

	// find first single character
	LPTSTR lpsz = _tcschr(m_string + start, (_TUCHAR)ch);
	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}


//-----------------------------------------------------------------------------
// find
//
// - find a substrign in current string
//
// returns:
//      -1     - if the string isn't found
//      >(-1)  - 0 based index of the substring in the string
//
int SOCmnString::find(IN LPCTSTR pszSub, IN OPTIONAL int start) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	int length = (DWORD)_tcslen(m_string);

	if (start > length)
	{
		return -1;
	}

	// find first matching substring
	LPTSTR psz = _tcsstr(m_string + start, pszSub);
	// return -1 for not found, distance from beginning otherwise
	return (psz == NULL) ? -1 : (int)(psz - m_string);
}


//-----------------------------------------------------------------------------
// findOneOf
//
// - find one character of a collection in current string
//
// returns:
//      -1     - if no character is matching
//      >(-1)  - 0 based index of the first matching character in the string
//
int SOCmnString::findOneOf(
	IN LPCTSTR pszCharset)
{
	if ((m_string == NULL) || (pszCharset == NULL) || !(_tcslen(pszCharset) > 0))
	{
		return -1;
	}

	LPTSTR lpsz = _tcspbrk(m_string, pszCharset);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}


//-----------------------------------------------------------------------------
// ReverseFind.
//
// - find the last occurrence of character ch in the string
//
// returns:
//      index of character or -1 if not found
//
int SOCmnString::reverseFind(IN TCHAR ch) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	// find last single character
	LPTSTR lpsz = _tcsrchr(m_string, (_TUCHAR) ch);
	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}



//-----------------------------------------------------------------------------
// addData
//
// - add data to the string data
//
// returns:
//      data buffer
//
LPTSTR SOCmnString::addData(
	IN LPCTSTR data,    // data to add
	IN DWORD numChars)  // length of data
{
	DWORD curLen = 0;
	DWORD curChars = 0;
	DWORD addLen = numChars * sizeof(TCHAR);
	DWORD newLen;
	TCHAR* newStr;

	if (m_string)
	{
		curChars = (DWORD)_tcslen(m_string);
		curLen = curChars * sizeof(TCHAR);
	}

	newLen = addLen + curLen + sizeof(TCHAR);
	newStr = (TCHAR*)malloc(newLen);

	if (newStr)
	{
		if (curLen)
		{
			memcpy(newStr, m_string, curLen);
		}

		if (addLen)
		{
			memcpy(newStr + curChars, data, addLen);
		}

		newStr[curChars + numChars] = _T('\0');
	}

	return newStr;
} // addData


//-----------------------------------------------------------------------------
// setData
//
// - set the string data
//
void SOCmnString::setData(
	IN LPTSTR data)
{
	if (m_string != data)
	{
		if (m_string)
		{
			free(m_string);
		}

		m_string = data;
	}
} // setData


//-----------------------------------------------------------------------------
// left
//
// - get the first nCount characters of the string
//
// returns:
//      first nCount characters as string
//
SOCmnString SOCmnString::left(IN int nCount) const
{
	if (m_string == NULL)
	{
		return SOCmnString(_T(""));
	}

	SOCmnString dest;

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nCount >= (int)_tcslen(m_string))
	{
		dest = *this;
	}
	else
	{
		LPTSTR  buf = dest.getBuffer(nCount);
		_tcsncpy(buf, m_string, nCount);
		buf[nCount] = 0;
	}

	return dest;
} // left


//-----------------------------------------------------------------------------
// right
//
// - get the last nCount characters of the string
//
// returns:
//      last nCount characters as string
//
SOCmnString SOCmnString::right(IN int nCount) const
{
	if (m_string == NULL)
	{
		return SOCmnString(_T(""));
	}

	SOCmnString dest;
	int nLen = (DWORD)_tcslen(m_string);

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nCount >= nLen)
	{
		dest = *this;
	}
	else
	{
		LPTSTR  buf = dest.getBuffer(nCount);
		TCHAR*  bufSource = m_string + nLen - nCount;
		_tcsncpy(buf, bufSource, nCount);
		buf[nCount] = 0;
	}

	return dest;
} //  right


//-----------------------------------------------------------------------------
// mid
//
// - get right part of the string starting with nFirst.
//
// returns:
//      last characters as string starting from nFirst
//
SOCmnString SOCmnString::mid(IN int nFirst) const
{
	if (m_string == NULL)
	{
		return SOCmnString(_T(""));
	}

	return mid(nFirst, (int)_tcslen(m_string) - nFirst);
} // mid


//-----------------------------------------------------------------------------
// mid
//
// - get nCount characters of the string starting with nFirst.
//
// returns:
//      nCount characters as string starting from nFirst
//
SOCmnString SOCmnString::mid(IN int nFirst, IN int nCount) const
{
	if (m_string == NULL)
	{
		return SOCmnString(_T(""));
	}

	int nLen = (DWORD)_tcslen(m_string);

	// out-of-bounds requests return sensible things
	if (nFirst < 0)
	{
		nFirst = 0;
	}

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nFirst + nCount > nLen)
	{
		nCount = nLen - nFirst;
	}

	if (nFirst > nLen)
	{
		nCount = 0;
	}

	_ASSERTION(nFirst >= 0, "nFirst parameter is to low (negative)");
	_ASSERTION(nFirst + nCount <= nLen, "You want to read more than the string len");
	// optimize case of returning entire string
	SOCmnString dest;

	if (nFirst == 0 && nFirst + nCount == nLen)
	{
		dest = *this;
	}
	else
	{
		if (nFirst < nLen)
		{
			LPTSTR  buf = dest.getBuffer(nCount);
			TCHAR*  bufSource = m_string + nFirst;
			_tcsncpy(buf, bufSource, nCount);
			buf[nCount] = 0;
		}
	}

	return dest;
} // mid

//-----------------------------------------------------------------------------
// hashDJB2
//
// - returns a Daniel J. Bernstein hash on the contained string plus secret
//   source: http://www.cse.yorku.ca/~oz/hash.html  /  http://cr.yp.to/djb.html
//
SOCmnString SOCmnString::hashDJB2(SOCmnString secret) const
{
	unsigned long hash = 5381;
	TCHAR tch;
	LPTSTR str = m_string;
	
	while ((tch = *str++) != NULL)
	{
		hash = ((hash << 5) + hash) + tch;  /* hash * 33 + c */
	}

	str = secret.m_string;

	while ((tch = *str++) != NULL)
	{
		hash = ((hash << 5) + hash) + tch;  /* hash * 33 + c */
	}

	SOCmnString hashString;
	hashString.format(_T("%8.8X"), hash);
	return hashString;
} // hashDJB2

//-----------------------------------------------------------------------------
// trimLeft
//
// - remove spaces from left
//
void SOCmnString::trimLeft(void)
{
	if (m_string == NULL)
	{
		return;
	}

	TCHAR* cTmp = m_string;
	int nLen = (DWORD)_tcslen(m_string);
	int nCharSize = sizeof(TCHAR);
	int nRemoved = 0;

	while (_istspace(*cTmp))
	{
		cTmp = _tcsinc(cTmp);
		++nRemoved;
	}

	if (cTmp != m_string)
	{
		memmove(m_string, cTmp, (nLen - nRemoved + 1)*nCharSize);
	}
} // trimLeft


//-----------------------------------------------------------------------------
// trimRight
//
// - remove spaces from right
//
void SOCmnString::trimRight(void)
{
	if (m_string == NULL)
	{
		return;
	}

	// find first non-space character
	int iLen = (DWORD)_tcslen(m_string);

	if (iLen == 0)
	{
		return;
	}

	TCHAR*  cTmp = m_string + iLen - 1;

	while (cTmp != NULL && _istspace(*cTmp))
	{
		cTmp = _tcsdec(m_string, cTmp);
	}

	if (cTmp != NULL)
	{
		++cTmp;
	}
	else
	{
		cTmp = m_string;
	}

	*cTmp = 0;
} // trimRight

void SOCmnString::setCHAR(LPCSTR src, DWORD srcLen)
{
	LPSTR hlp = (LPSTR)malloc(srcLen + 1);

	if (!hlp)
	{
		return;
	}

	strncpy(hlp, src, srcLen);
	hlp[srcLen] = '\0';
	LPTSTR s = NULL;
	USES_CONVERSION;
	s = _tcsdup(A2T(hlp));
	free(hlp);
	setData(s);
}

void SOCmnString::set(LPCTSTR src, DWORD srcLen)
{
	LPTSTR s = (LPTSTR)malloc((srcLen + 1) * sizeof(TCHAR));

	if (!s)
	{
		return;
	}

	_tcsncpy(s, src, srcLen);
	s[srcLen] = _T('\0');
	setData(s);
}

void SOCmnString::setUTF8(LPCSTR src, DWORD srcLen)
{
#ifdef SOOS_LINUX
	setCHAR(src, srcLen);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	DWORD bStrLen;
	LPWSTR bStrCpy;
	bStrCpy = (LPWSTR)malloc((srcLen + 1) * 2);
#ifndef SOOS_WINDOWS_CE
	bStrLen =  MultiByteToWideChar(CP_UTF8, 0, src, srcLen, bStrCpy, srcLen + 1);
#else
	// Windows CE does not support UTF8
	bStrLen =  MultiByteToWideChar(CP_ACP, 0, src, srcLen, bStrCpy, srcLen + 1);
#endif
	bStrCpy[bStrLen] = L'\0';
	fromWCHAR(bStrCpy);
	free(bStrCpy);
#endif // SOOS_WINDOWS
}

void SOCmnString::replace(
	IN DWORD startIdx,
	IN DWORD endIdx,
	IN LPCTSTR pRepData,
	IN DWORD repLen)
{
	DWORD stringLen = (DWORD)_tcslen(m_string);

	if ((startIdx > stringLen) || (endIdx > stringLen) || (endIdx <= startIdx))
	{
		return;
	}

	DWORD curLen = (endIdx - startIdx);
	DWORD newLen = stringLen - curLen + repLen;

	if ((newLen + 1) > stringLen)
	{
		DWORD bufferLen = newLen + 1;
		m_string = (TCHAR*)realloc(m_string, bufferLen * sizeof(TCHAR));
	}

	if (m_string)
	{
		memmove(m_string + startIdx + repLen, m_string + startIdx + curLen, (stringLen - endIdx) * sizeof(TCHAR));

		if (repLen)
		{
			memcpy(m_string + startIdx, pRepData, repLen * sizeof(TCHAR));
		}

		m_string[newLen] = _T('\0');
	}
}

#ifdef SOFEATURE_CMN_XML

void SOCmnString::doXMLReplacements(SOCmnString& xmlStr)
{
	if (xmlStr.findOneOf(_T("&<>'\"")) != -1)
	{
		xmlStr.replace(_T("&"), _T("&amp;"));
		xmlStr.replace(_T("<"), _T("&lt;"));
		xmlStr.replace(_T(">"), _T("&gt;"));
		xmlStr.replace(_T("'"), _T("&apos;"));
		xmlStr.replace(_T("\""), _T("&quot;"));
	}

	if (xmlStr.findOneOf(_T("\x0c\x0b")) != -1)
	{
		xmlStr.replace(_T("\x0c"), _T("_x000c_"));
		xmlStr.replace(_T("\x0b"), _T("_x000b_"));
	}
}

#endif // SOFEATURE_CMN_XML

#ifdef SOFEATURE_CMN_STRINGEX

// special value for m_stringLen:
const DWORD STRING_LENGTH_UNKNOWN = 0xFFFFFFFF;

//-----------------------------------------------------------------------------
// SOCmnStringEx                                                                |
//-----------------------------------------------------------------------------

SOCmnStringEx::SOCmnStringEx()
{
	m_bufferLen = 0;
	m_stringLen = 0;
	m_string = NULL;
}

SOCmnStringEx::SOCmnStringEx(
	IN LPCTSTR sz)  // string pointer
{
	m_bufferLen = 0;
	m_stringLen = 0;
	m_string = NULL;

	if (sz)
	{
		setDataC(sz);
	}
}

SOCmnStringEx::SOCmnStringEx(
	IN TCHAR c) // one char
{
	m_bufferLen = 0;
	m_stringLen = 0;
	m_string = NULL;
	setDataC(&c, 1);
}

SOCmnStringEx::SOCmnStringEx(
	IN const SOCmnStringEx* str)    // string object pointer
{
	m_bufferLen = 0;
	m_stringLen = 0;
	m_string = NULL;

	if (str)
	{
		LPCTSTR sz = (LPCTSTR) * str;

		if (sz)
		{
			setDataC(sz, str->getLength());
		}
	}
}

SOCmnStringEx::SOCmnStringEx(
	IN const SOCmnStringEx& str)    // string object
{
	LPCTSTR sz = (LPCTSTR)str;
	m_stringLen = 0;
	m_bufferLen = 0;
	m_string = NULL;

	if (sz)
	{
		setDataC(sz);
	}
}

SOCmnStringEx::~SOCmnStringEx()
{
	if (m_string)
	{
		free(m_string);
	}

	m_bufferLen = 0;
	m_stringLen = 0;
	m_string = NULL;
}

SOCmnStringEx::operator LPCTSTR() const
{
	if (m_string)
	{
		return m_string;
	}
	else
	{
		return g_emptyString;
	}
}

SOCmnStringEx::operator LPTSTR()
{
	if (m_string)
	{
		return m_string;
	}
	else
	{
		return g_emptyString;
	}
}



//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator+=(
	IN const SOCmnString& str)
{
	LPCTSTR sz = (LPCTSTR)str;

	if (sz)
	{
		addData(sz, (DWORD)_tcslen(sz));
	}

	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator+=(
	IN const SOCmnStringEx& str)
{
	LPCTSTR sz = (LPCTSTR)str;

	if (sz)
	{
		addData(sz, str.getLength());
	}

	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator+=(
	IN LPCTSTR sz)  // string pointer
{
	if (sz)
	{
		addData(sz, (DWORD)_tcslen(sz));
	}

	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator +=
//
// - add a string to the string
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator+=(
	IN TCHAR c) // one char
{
	addData(&c, 1);
	return *this;
} // operator +=


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator=(
	IN const SOCmnString& str)  // string object
{
	setDataC((LPCTSTR)str);
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator=(
	IN const SOCmnStringEx& str)    // string object
{
	setDataC((LPCTSTR)str, str.getLength());
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator=(
	IN LPCTSTR sz)  // string pointer
{
	setDataC(sz);
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator =
//
// - set the string data
//
// returns:
//      the string object
//
const SOCmnStringEx& SOCmnStringEx::operator=(
	IN TCHAR c) // one char
{
	setDataC(&c, 1);
	return *this;
} // operator =


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnStringEx SOCmnStringEx::operator+(
	IN const SOCmnString& str)  // string object
{
	LPCTSTR sz = (LPCTSTR)str;
	SOCmnStringEx s(*this);

	if (sz)
	{
		s.addData(sz, (DWORD)_tcslen(sz));
	}

	return s;
} // operator +


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnStringEx SOCmnStringEx::operator+(
	IN const SOCmnStringEx& str)    // string object
{
	LPCTSTR sz = (LPCTSTR)str;
	SOCmnStringEx s(*this);

	if (sz)
	{
		s.addData(sz, str.getLength());
	}

	return s;
} // operator +


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnStringEx SOCmnStringEx::operator+(
	IN LPCTSTR sz)  // string pointer
{
	SOCmnStringEx s(*this);

	if (sz)
	{
		s.addData(sz, (DWORD)_tcslen(sz));
	}

	return s;
} // operator +


//-----------------------------------------------------------------------------
// operator +
//
// - add the string to another string
//
// returns:
//      the string object
//
SOCmnStringEx SOCmnStringEx::operator+(
	IN TCHAR c) // one char
{
	SOCmnStringEx s(*this);
	s.addData(&c, 1);
	return s;
} // operator +


//-----------------------------------------------------------------------------
// getLength
//
// - get the length of the string
//
// returns:
//      number of elements of the string
//
DWORD SOCmnStringEx::getLength(void)
const
{
	if (m_string == NULL)
	{
		return 0;
	}

	if (m_stringLen == STRING_LENGTH_UNKNOWN)
	{
		(const_cast<SOCmnStringEx*>(this))->m_stringLen = (DWORD)_tcslen(m_string);
	}

	return m_stringLen;
} // getLength


//-----------------------------------------------------------------------------
// isEmpty
//
// - check if the string is empty
//
// returns:
//      TRUE  - string is empty
//      FALSE - string is not empty
//
BOOL SOCmnStringEx::isEmpty(void)
const
{
	return getLength() == 0;
} // isEmpty


//-----------------------------------------------------------------------------
// empty
//
// - empty the string
//
void SOCmnStringEx::empty(void)
{
	if (m_string != NULL)
	{
		m_string[0] = _T('\0');
	}

	m_stringLen = 0;
} // empty


//-----------------------------------------------------------------------------
// replace
//
// - replace indicated characters with other characters
//
// returns:
//      number of replaced substrings
//
int SOCmnStringEx::replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew)
{
	if (m_string == NULL)
	{
		return 0;
	}

	// can't work with empty or NULL pszOld
	int iSourceLen = (int)_tcslen(pszOld);

	if (iSourceLen == 0)
	{
		return 0;
	}

	int iReplaceLen = (int)_tcslen(pszNew);
	// loop once to figure out the size of the result string
	int nCount = 0;
	LPTSTR pszStart = m_string;
	LPTSTR pszEnd = m_string + getLength();
	LPTSTR pszTarget;

	// just count substrings
	while (pszStart < pszEnd)
	{
		while ((pszTarget = _tcsstr(pszStart, pszOld)) != NULL)
		{
			nCount++;
			pszStart = pszTarget + iSourceLen;
		}

		pszStart += lstrlen(pszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		int ibuffer = (((getLength() - (nCount * (iSourceLen - iReplaceLen))) + 1) * sizeof(TCHAR));
		TCHAR* buffer = (TCHAR*)malloc(ibuffer);
		pszStart = m_string;
		pszEnd = m_string + getLength();
		// loop again to actually do the work
		TCHAR* tmpbuffer = buffer;

		while (pszStart < pszEnd)
		{
			while ((pszTarget = _tcsstr(pszStart, pszOld)) != NULL)
			{
				// copy string until substring to buffer
				memcpy(tmpbuffer, pszStart, (pszTarget - pszStart)*sizeof(TCHAR));
				tmpbuffer += pszTarget - pszStart;
				// copy new string in buffer
				memcpy(tmpbuffer, pszNew, iReplaceLen * sizeof(TCHAR));
				tmpbuffer += iReplaceLen;
				// set new start position behind found substring
				pszStart = pszTarget + iSourceLen;
			}

			// copy missing tail to buffer
			memcpy(tmpbuffer, pszStart, (pszEnd - pszStart)*sizeof(TCHAR));
			// set final '\0' character
			memset(tmpbuffer + lstrlen(pszStart), _T('\0'), sizeof(TCHAR));
			pszStart += lstrlen(pszStart) + 1;
		}

		setDataC(buffer);
		free(buffer);
	}

	return nCount;
} // replace


//-----------------------------------------------------------------------------
// makeLower
//
// - covert string to lowercase
//
void SOCmnStringEx::makeLower(
	IN OPTIONAL DWORD startIdx) // start index of operation
{
	DWORD curSize = getLength();
	DWORD i;

	for (i = startIdx; i < curSize; i++)
	{
		m_string[i] = (TCHAR)_totlower(m_string[i]);
	}
} // makeLower


//-----------------------------------------------------------------------------
// makeUpper
//
// - covert string to uppercase
//
void SOCmnStringEx::makeUpper(
	IN OPTIONAL DWORD startIdx) // start index of operation
{
	DWORD curSize = getLength();
	DWORD i;

	for (i = startIdx; i < curSize; i++)
	{
		m_string[i] = (TCHAR)_totupper(m_string[i]);
	}
} // makeUpper


#ifdef SOOS_WINDOWS
void SOCmnStringEx::fromWCHAR(LPCWSTR wszSrc)
{
	USES_CONVERSION;
	LPTSTR szStr = wszSrc ? W2T((LPWSTR)wszSrc) : NULL;
	setDataC(szStr);
}

LPWSTR SOCmnStringEx::cloneToWCHAR(void)
{
	return cloneToWCHAR(*this);
}

LPWSTR SOCmnStringEx::cloneToWCHAR(IN SOCmnStringEx& str)
{
	USES_CONVERSION;
	DWORD strLen;
	LPWSTR wstr;
	strLen = str.getLength();
	wstr = (LPWSTR)CoTaskMemAlloc((strLen + 1) * 2);

	if (!wstr)
	{
		return NULL;
	}

	memset(wstr, 0, 2 * (strLen + 1));

	if (strLen)
	{
#if defined _UNICODE || defined UNICODE
		// when using UNICODE no real conversion is done
		// no stack overflow can happen
		wcscpy(wstr, T2CW((LPCTSTR)str));
#else
		// We use the follwing function since with big configurations
		// a stack overflow can appear using ATL conversion macros!
		::MultiByteToWideChar(GetACP(), 0, str, -1, wstr, strLen + 1);
#endif
	}

	return wstr;
}
#endif // SOOS_WINDOWS


//-----------------------------------------------------------------------------
// areEqual
//
// - check if two strings are equal
//
// returns:
//      TRUE  - strings are equal
//      FALSE - strings are different
//
BOOL SOCmnStringEx::areEqual(
	IN LPCTSTR sz1, // first string
	IN LPCTSTR sz2) // second string
{
	if ((sz1 == NULL) && (sz2 == NULL))
	{
		return TRUE;
	}

	if ((sz1 == NULL) || (sz2 == NULL))
	{
		if (sz1 == NULL)
		{
			return (_tcslen(sz2) == 0);
		}
		else
		{
			return (_tcslen(sz1) == 0);
		}
	}

	return (_tcscmp(sz1, sz2) == 0);
} // areEqual


//-----------------------------------------------------------------------------
// find
//
// - find a character in current string
//
// returns:
//      -1     - if the character isn't found
//      >(-1)  - 0 based index of the character in the string
//
int SOCmnStringEx::find(
	IN TCHAR ch,
	IN OPTIONAL int start) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	int length = getLength();

	if (start > length)
	{
		return -1;
	}

	// find first single character
	LPTSTR lpsz = _tcschr(m_string + start, (_TUCHAR)ch);
	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}


//-----------------------------------------------------------------------------
// find
//
// - find a substrign in current string
//
// returns:
//      -1     - if the string isn't found
//      >(-1)  - 0 based index of the substring in the string
//
int SOCmnStringEx::find(IN LPCTSTR pszSub, IN OPTIONAL int start) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	int length = getLength();

	if (start > length)
	{
		return -1;
	}

	// find first matching substring
	LPTSTR psz = _tcsstr(m_string + start, pszSub);
	// return -1 for not found, distance from beginning otherwise
	return (psz == NULL) ? -1 : (int)(psz - m_string);
}


//-----------------------------------------------------------------------------
// findOneOf
//
// - find one character of a collection in current string
//
// returns:
//      -1     - if no character is matching
//      >(-1)  - 0 based index of the first matching character in the string
//
int SOCmnStringEx::findOneOf(
	IN LPCTSTR pszCharset)
{
	if ((m_string == NULL) || (pszCharset == NULL) || !(_tcslen(pszCharset) > 0))
	{
		return -1;
	}

	LPTSTR lpsz = _tcspbrk(m_string, pszCharset);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}


//-----------------------------------------------------------------------------
// ReverseFind.
//
// - find the last occurrence of character ch in the string
//
// returns:
//      index of character or -1 if not found
//
int SOCmnStringEx::reverseFind(IN TCHAR ch) const
{
	if (m_string == NULL)
	{
		return -1;
	}

	// find last single character
	LPTSTR lpsz = _tcsrchr(m_string, (_TUCHAR) ch);
	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_string);
}



//-----------------------------------------------------------------------------
// left.
//
// - get the first nCount characters of the string
//
// returns:
//      first nCount characters as string
//
SOCmnStringEx SOCmnStringEx::left(IN int nCount) const
{
	SOCmnStringEx dest(_T(""));

	if (m_string == NULL)
	{
		return dest;
	}

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nCount >= (int)getLength())
	{
		dest = *this;
	}
	else
	{
		LPTSTR  buf = dest.getBuffer(nCount);
		_tcsncpy(buf, m_string, nCount);
		buf[nCount] = 0;
	}

	return dest;
} // left


//-----------------------------------------------------------------------------
// right
//
// - get the last nCount characters of the string
//
// returns:
//      last nCount characters as string
//
SOCmnStringEx SOCmnStringEx::right(IN int nCount) const
{
	SOCmnStringEx dest(_T(""));

	if (m_string == NULL)
	{
		return dest;
	}

	int nLen = getLength();

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nCount >= nLen)
	{
		dest = *this;
	}
	else
	{
		LPTSTR  buf = dest.getBuffer(nCount);
		TCHAR*  bufSource = m_string + nLen - nCount;
		_tcsncpy(buf, bufSource, nCount);
		buf[nCount] = 0;
	}

	return dest;
} //  right


//-----------------------------------------------------------------------------
// mid
//
// - get right part of the string starting with nFirst.
//
// returns:
//      last characters as string starting from nFirst
//
SOCmnStringEx SOCmnStringEx::mid(IN int nFirst) const
{
	if (m_string == NULL)
	{
		return SOCmnStringEx(_T(""));
	}

	return mid(nFirst, (int)getLength() - nFirst);
} // mid


//-----------------------------------------------------------------------------
// mid
//
// - get nCount characters of the string starting with nFirst.
//
// returns:
//      nCount characters as string starting from nFirst
//
SOCmnStringEx SOCmnStringEx::mid(IN int nFirst, IN int nCount) const
{
	SOCmnStringEx dest(_T(""));

	if (m_string == NULL)
	{
		return dest;
	}

	int nLen = getLength();

	// out-of-bounds requests return sensible things
	if (nFirst < 0)
	{
		nFirst = 0;
	}

	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nFirst + nCount > nLen)
	{
		nCount = nLen - nFirst;
	}

	if (nFirst > nLen)
	{
		nCount = 0;
	}

	_ASSERTION(nFirst >= 0, "nFirst parameter is to low (negative)");
	_ASSERTION(nFirst + nCount <= nLen, "You want to read more than the string len");

	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == nLen)
	{
		dest = *this;
	}
	else
	{
		if (nFirst < nLen)
		{
			LPTSTR  buf = dest.getBuffer(nCount);
			TCHAR*  bufSource = m_string + nFirst;
			_tcsncpy(buf, bufSource, nCount);
			buf[nCount] = 0;
		}
	}

	return dest;
} // mid


//-----------------------------------------------------------------------------
// trimLeft
//
// - remove spaces from left
//
void SOCmnStringEx::trimLeft(void)
{
	if (m_string == NULL)
	{
		return;
	}

	TCHAR* cTmp = m_string;
	int nLen = getLength();
	int nCharSize = sizeof(TCHAR);
	int nRemoved = 0;

	while (_istspace(*cTmp))
	{
		cTmp = _tcsinc(cTmp);
		++nRemoved;
	}

	if (cTmp != m_string)
	{
		m_stringLen = nLen - nRemoved;
		memmove(m_string, cTmp, m_stringLen * nCharSize);
		m_string[m_stringLen] = 0;
	}
} // trimLeft


//-----------------------------------------------------------------------------
// trimRight
//
// - remove spaces from right
//
void SOCmnStringEx::trimRight(void)
{
	if (m_string == NULL)
	{
		return;
	}

	// find first non-space character
	int iLen = getLength();

	if (iLen == 0)
	{
		return;
	}

	TCHAR*  cTmp = m_string + iLen - 1;

	while (cTmp != NULL && _istspace(*cTmp))
	{
		cTmp = _tcsdec(m_string, cTmp);
	}

	if (cTmp != NULL)
	{
		++cTmp;
	}
	else
	{
		cTmp = m_string;
	}

	*cTmp = 0;
	m_stringLen = (DWORD)(cTmp - m_string);
} // trimRight

void SOCmnStringEx::setCHAR(LPCSTR src, DWORD srcLen)
{
	LPSTR hlp = (LPSTR)malloc(srcLen + 1);

	if (!hlp)
	{
		return;
	}

	strncpy(hlp, src, srcLen);
	hlp[srcLen] = '\0';
	LPTSTR s = NULL;
	USES_CONVERSION;
	s = A2T(hlp);
	setDataC(s);
	free(hlp);
}

void SOCmnStringEx::set(LPCTSTR src, DWORD srcLen)
{
	setDataC(src, srcLen);
}

LPTSTR SOCmnStringEx::getBuffer(DWORD minBufferLen)
{
	if (m_bufferLen < minBufferLen + 1)
	{
		DWORD curLen = 0;

		if (m_bufferLen != 0)
		{
			curLen = getLength();
		}

		m_bufferLen = minBufferLen + 1;
		m_string = (TCHAR*)realloc(m_string, (m_bufferLen) * sizeof(TCHAR));
		m_string[curLen] = _T('\0');
	}

	m_stringLen = STRING_LENGTH_UNKNOWN; // m_string can be modified...
	return m_string;
}

LPCTSTR SOCmnStringEx::format(IN LPCTSTR format, ...)
{
	va_list arglist;
	int n;

	if (m_string == NULL)
	{
		m_bufferLen = 100;

		if ((m_string = (TCHAR*)malloc(m_bufferLen * sizeof(TCHAR))) == NULL)
		{
			return NULL;
		}
	}

	while (1)
	{
		/* Try to print in the allocated space. */
		va_start(arglist, format);
		n = _vsntprintf(m_string, m_bufferLen, format, arglist);
		va_end(arglist);

		/* If that worked, return the string. */
		if (n > -1 && n < (int)m_bufferLen)
		{
			m_stringLen = n;
			return *this;
		}

		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
		{
			m_bufferLen = n + 1;    /* precisely what is needed */
		}
		else           /* glibc 2.0 */
		{
			m_bufferLen *= 2;    /* twice the old size */
		}

		if ((m_string = (TCHAR*)realloc(m_string, m_bufferLen * sizeof(TCHAR))) == NULL)
		{
			break;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// addData
//
// - add data to the string data
//
// returns:
//      data buffer
//
LPTSTR SOCmnStringEx::addData(
	IN LPCTSTR data,    // data to add
	IN DWORD numChars)  // length of data
{
	DWORD curLen = 0;
	DWORD curChars = 0;
	DWORD addLen = numChars;
	DWORD newLen;

	if (m_string)
	{
		curChars = getLength();
		curLen = curChars;
	}

	newLen = addLen + curLen + 1;

	if (newLen > m_bufferLen)
	{
		m_bufferLen = newLen * 2;
		m_string = (TCHAR*)realloc(m_string, m_bufferLen * sizeof(TCHAR));
	}

	if (m_string)
	{
		if (addLen)
		{
			memcpy(m_string + curChars, data, addLen * sizeof(TCHAR));
		}

		m_stringLen = curChars + numChars;
		m_string[m_stringLen] = _T('\0');
	}

	return m_string;
} // addData


//-----------------------------------------------------------------------------
// setDataC
//
// - set the string data
//
void SOCmnStringEx::setDataC(
	IN LPCTSTR data)
{
	DWORD len = 0;

	if (data != NULL)
	{
		len = (DWORD)_tcslen(data);
	}

	setDataC(data, len);
}


//-----------------------------------------------------------------------------
// setDataC
//
// - set the string data
//
void SOCmnStringEx::setDataC(
	IN LPCTSTR data, IN DWORD len)
{
	if (data == NULL)
	{
		if (m_string)
		{
			m_string[0] = _T('\0');
		}

		m_stringLen = 0;
		return;
	}

	if (m_string != data)
	{
		DWORD newLen;
		newLen = len + 1;

		if (newLen > m_bufferLen)
		{
			m_bufferLen = newLen;
			m_string = (TCHAR*)realloc(m_string, m_bufferLen * sizeof(TCHAR));
		}

		if (m_string)
		{
			if (len)
			{
				memcpy(m_string, data, len * sizeof(TCHAR));
			}

			m_string[len] = _T('\0');
			m_stringLen = len;
		}
	}
} // setData

//-----------------------------------------------------------------------------
// attachTo
//
// -
//
void SOCmnStringEx::attachTo(
	IN LPTSTR data, IN DWORD len)
{
	if ((data == NULL) || (len == 0))
	{
		return;
	}

	if (data[len - 1] != _T('\0'))
	{
		len++;
		data = (TCHAR*)realloc(data, len * sizeof(TCHAR));
		data[len - 1] = _T('\0');
	}

	// free existing string
	if (m_string)
	{
		free(m_string);
	}

	m_stringLen = len - 1;
	m_bufferLen = len;
	m_string = data;
} // attachTo

void SOCmnStringEx::replace(
	IN DWORD startIdx,
	IN DWORD endIdx,
	IN LPCTSTR pRepData,
	IN DWORD repLen)
{
	if ((startIdx > m_stringLen) || (endIdx > m_stringLen) || (endIdx <= startIdx))
	{
		return;
	}

	DWORD curLen = (endIdx - startIdx);
	DWORD newLen = m_stringLen - curLen + repLen;

	if ((newLen + 1) > m_bufferLen)
	{
		m_bufferLen = newLen + 1;
		m_string = (TCHAR*)realloc(m_string, m_bufferLen * sizeof(TCHAR));
	}

	if (m_string)
	{
		memmove(m_string + startIdx + repLen, m_string + startIdx + curLen, (m_stringLen - endIdx) * sizeof(TCHAR));

		if (repLen)
		{
			memcpy(m_string + startIdx, pRepData, repLen * sizeof(TCHAR));
		}

		m_string[newLen] = _T('\0');
		m_stringLen = newLen;
	}
}

void SOCmnStringEx::addCHAR(LPCSTR src, DWORD srcLen)
{
	LPSTR hlp = (LPSTR)malloc(srcLen + 1);

	if (!hlp)
	{
		return;
	}

	strncpy(hlp, src, srcLen);
	hlp[srcLen] = '\0';
	LPTSTR s = NULL;
	USES_CONVERSION;
	s = A2T(hlp);
	addData(s, srcLen);
	free(hlp);
}

#ifdef SOOS_LINUX
BYTE* SOCmnStringEx::cloneToUTF8(OUT DWORD* pLength)
{
	if (pLength)
	{
		*pLength = m_stringLen;
	}

	return (BYTE*)_tcsdup(m_string);
}
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
BYTE* SOCmnStringEx::cloneToUTF8(OUT DWORD* pLength)
{
	BYTE* pUTF8;
	DWORD utf8Len;
	WCHAR* strU;
	DWORD mallocLen = m_stringLen + 1;

	if (pLength)
	{
		*pLength = 0;
	}

	pUTF8 = (BYTE*)malloc(mallocLen);

	if (m_stringLen == 0)
	{
		*pUTF8 = 0;
		return pUTF8;
	}

	strU = cloneToWCHAR();

	do
	{
#ifndef SOOS_WINDOWS_CE
		utf8Len = WideCharToMultiByte(CP_UTF8, 0, strU, m_stringLen, (LPSTR)pUTF8, mallocLen - 1, NULL, NULL);
#else
		// Windows CE does not support UTF8
		utf8Len = WideCharToMultiByte(CP_ACP, 0, strU, m_stringLen, (LPSTR)pUTF8, mallocLen - 1, NULL, NULL);
#endif

		if (utf8Len == 0)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				mallocLen = WideCharToMultiByte(CP_UTF8, 0, strU, m_stringLen, (LPSTR)pUTF8, 0, NULL, NULL);
				mallocLen++;
				pUTF8 = (BYTE*)realloc(pUTF8, mallocLen);
			}
			else
			{
				return NULL;
			}
		}
	}
	while (utf8Len == 0);

	CoTaskMemFree(strU);

	if (pLength)
	{
		*pLength = utf8Len;
	}

	pUTF8[utf8Len] = 0;
	return pUTF8;
}
#endif // SOOS_WINDOWS

#ifdef SOOS_LINUX
CHAR* SOCmnStringEx::cloneToCHAR(OUT DWORD* pLength)
{
	if (pLength)
	{
		*pLength = m_stringLen;
	}

	return strdup(m_string);
}
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
CHAR* SOCmnStringEx::cloneToCHAR(OUT DWORD* pLength)
{
	USES_CONVERSION;

	if (pLength)
	{
		*pLength = m_stringLen;
	}

	LPSTR sA = T2A(m_string);
#ifndef SOOS_WINDOWS_CE
	return _strdup(sA);
#else
	CHAR* dupStr = (CHAR*)malloc(strlen(sA) + 1);
	memcpy(dupStr, sA, strlen(sA) + 1);
	return dupStr;
#endif
}
#endif // SOOS_WINDOWS


#ifdef SOOS_LINUX
void SOCmnStringEx::setCHARWithUTF8(BYTE** pDest, DWORD* destLen, BYTE* pSrc, DWORD srcLen)
{
	SOCMN_ALLOCATE_MEMORY_C(*pDest, srcLen + 1, (BYTE*))

	if (*pDest)
	{
		memcpy(*pDest, pSrc, srcLen);
		*destLen = srcLen;
		*pDest[srcLen] = 0;
	}
	else
	{
		*destLen = 0;
	}
}
#endif
#ifdef SOOS_WINDOWS
void SOCmnStringEx::setCHARWithUTF8(BYTE** pDest, DWORD* destLen, BYTE* pSrc, DWORD srcLen)
{
	USES_CONVERSION;
	DWORD bStrLen;
	LPWSTR bStrCpy;
	bStrCpy = (LPWSTR)malloc((srcLen + 2) * sizeof(WCHAR));
#ifndef SOOS_WINDOWS_CE
	bStrLen =  MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pSrc, srcLen, bStrCpy, srcLen + 1);
#else
	// Windows CE does not support UTF8
	bStrLen =  MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSrc, srcLen, bStrCpy, srcLen + 1);
#endif
	bStrCpy[bStrLen] = _T('\0');
	SOCMN_ALLOCATE_MEMORY_C(*pDest, bStrLen + 1, (BYTE*))
	*destLen = WideCharToMultiByte(CP_ACP, 0, bStrCpy, bStrLen, (LPSTR) * pDest, bStrLen + 1, NULL, NULL);
	free(bStrCpy);
}

#endif

void SOCmnStringEx::setUTF8(LPCSTR src, DWORD srcLen)
{
#ifdef SOOS_LINUX
	setCHAR(src, srcLen);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	DWORD bStrLen;
	LPWSTR bStrCpy;
	bStrCpy = (LPWSTR)malloc((srcLen + 1) * 2);
#ifndef SOOS_WINDOWS_CE
	bStrLen =  MultiByteToWideChar(CP_UTF8, 0, src, srcLen, bStrCpy, srcLen + 1);
#else
	// Windows CE does not support UTF8
	bStrLen =  MultiByteToWideChar(CP_ACP, 0, src, srcLen, bStrCpy, srcLen + 1);
#endif
	bStrCpy[bStrLen] = L'\0';
	fromWCHAR(bStrCpy);
	free(bStrCpy);
#endif // SOOS_WINDOWS
}

#ifdef SOFEATURE_CMN_XML

void SOCmnStringEx::doXMLReplacements(SOCmnStringEx& xmlStr)
{
	if (xmlStr.findOneOf(_T("&<>'\"")) != -1)
	{
		xmlStr.replace(_T("&"), _T("&amp;"));
		xmlStr.replace(_T("<"), _T("&lt;"));
		xmlStr.replace(_T(">"), _T("&gt;"));
		xmlStr.replace(_T("'"), _T("&apos;"));
		xmlStr.replace(_T("\""), _T("&quot;"));
	}

	if (xmlStr.findOneOf(_T("\x0c\x0b")) != -1)
	{
		xmlStr.replace(_T("\x0c"), _T("_x000c_"));
		xmlStr.replace(_T("\x0b"), _T("_x000b_"));
	}
}

#ifdef SOOS_LINUX
LPCTSTR SOCmnStringEx::getXMLString(LPCTSTR str, SOCmnStringEx& xmlStr)
{
	xmlStr = str;
	doXMLReplacements(xmlStr);
	return xmlStr;
}
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
LPCTSTR SOCmnStringEx::getXMLString(LPCSTR str, SOCmnStringEx& xmlStr)
{
	xmlStr.setCHAR(str, (DWORD)strlen(str));
	doXMLReplacements(xmlStr);
	return xmlStr;
}

LPCTSTR SOCmnStringEx::getXMLString(LPCWSTR bStr, SOCmnStringEx& xmlStr)
{
	xmlStr.fromWCHAR(bStr);
	doXMLReplacements(xmlStr);
	return xmlStr;
}
#endif // SOOS_WINDOWS

LPCTSTR SOCmnStringEx::setXMLString(LPCTSTR xmlStra, SOCmnStringEx& outStr)
{
	SOCmnStringEx xmlStr(xmlStra);

	// Is it a string at least of 7 characters than replace all 
	//	patterns "_x[0-f][0-f][0-f][0-f]_" like _x000a_ or _xa00c_
	//  with their corresponding hex number 
	if (xmlStr.getLength() >= 7)
	{
		
		if (xmlStr.find(_T("_x")) != -1)
		{
			int idx = 0;
			DWORD hexNumber;
			TCHAR hexSign[8];
			TCHAR newSign;

			do
			{
				idx = xmlStr.find(_T("_x"), idx);

				if ( (idx != -1) && ( (DWORD)idx + 7 <= xmlStr.getLength()) )
				{
					memset(hexSign, 0, 8 * sizeof(TCHAR));
					_tcsncpy(hexSign, (LPCTSTR)xmlStr + idx, 7);
					// pattern is not ending with _ on the last position (ex _x00afg_ )
					if (hexSign[6] != _T('_'))
					{
						idx = idx + 1;
						continue;
					}
					//is it a valid hex number pattern (ex:_x0fab_ ?
					if ((_istxdigit(hexSign[2])) && (_istxdigit(hexSign[3])) && (_istxdigit(hexSign[4])) && (_istxdigit(hexSign[5])))
					{
						hexNumber = _tcstoul(&hexSign[2], NULL, 16);
						newSign = (TCHAR)hexNumber;
						xmlStr.replace(idx, idx + 7, &newSign, 1);
						idx = idx + 1;
						continue;
					}
					// try onwards to identify and replace
					idx = idx + 7;
				}
				else 
				{
					break;
				}
			}
			while (idx != -1);
		}
	}

	outStr = xmlStr;
	return outStr;
}

#endif // SOFEATURE_CMN_XML


#endif // SOFEATURE_CMN_STRINGEX

