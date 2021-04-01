#ifndef _SOCMNSTRING_H_
#define _SOCMNSTRING_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#pragma pack(push, 4)


#ifndef FORMAT_PRINTF
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR >= 7)
#define FORMAT_PRINTF(m,n) __attribute__ ((__format__ (__printf__, m, n)))
#else
#define FORMAT_PRINTF(m,n)
#endif
#endif

//-----------------------------------------------------------------------------
// SOCmnString                                                                |
//-----------------------------------------------------------------------------

#define FOUND_SUBSTRING(b) (b != -1)


class SOCMN_EXPORT SOCmnString
{
public:
	SOCmnString();
	SOCmnString(IN TCHAR c);
	SOCmnString(IN LPCTSTR sz);
	SOCmnString(IN const SOCmnString* str);
	SOCmnString(IN const SOCmnString& str);

	~SOCmnString();

	operator LPCTSTR() const;   // get string buffer
	operator LPTSTR();          // get string buffer

	// add string to the existing string
	const SOCmnString& operator+=(IN const SOCmnString& str);
	const SOCmnString& operator+=(IN LPCTSTR sz);
	const SOCmnString& operator+=(IN TCHAR c);

	// set string data
	const SOCmnString& operator=(IN const SOCmnString& str);
	const SOCmnString& operator=(IN LPCTSTR sz);
	const SOCmnString& operator=(IN TCHAR c);

	// add string to another string
	SOCmnString operator+(IN const SOCmnString& str);
	SOCmnString operator+(IN LPCTSTR sz);
	SOCmnString operator+(IN TCHAR c);

	// get length of the string
	DWORD GetLength(void) const;
	DWORD getLength(void) const;

	// check if string is empty
	BOOL IsEmpty(void) const;
	BOOL isEmpty(void) const;

	// empty string
	void Empty(void);
	void empty(void);

	// set string with format
	void Format(IN LPCTSTR format, ...) FORMAT_PRINTF(2, 3);
	LPCTSTR format(IN LPCTSTR format, ...) FORMAT_PRINTF(2, 3);

	// replace indicated characters with other characters
	int Replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew);
	int replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew);
	int replace(IN TCHAR oldChar, IN TCHAR newChar, OPTIONAL IN DWORD offset = 0);
	int replaceWithEscape(IN TCHAR oldChar, IN TCHAR newChar, OPTIONAL IN DWORD offset = 0);
	void replace(IN DWORD startIdx, IN DWORD endIdxT, LPCTSTR pNewData, DWORD len);

	void MakeLower(void);
	void makeLower(IN OPTIONAL DWORD startIdx = 0);
	void MakeUpper(void);
	void makeUpper(IN OPTIONAL DWORD startIdx = 0);

	// get string buffer
	LPTSTR GetBuffer(IN DWORD minBufferLen);
	LPTSTR getBuffer(IN DWORD minBufferLen);

	// check if two strings are equal
	static BOOL areEqual(IN LPCTSTR sz1, IN LPCTSTR sz2);

	// search in the string
	int Find(IN TCHAR ch, IN OPTIONAL int start = 0) const;
	int find(IN TCHAR ch, IN OPTIONAL int start = 0) const;
	int Find(IN LPCTSTR pszSub, IN OPTIONAL int start = 0) const;
	int find(IN LPCTSTR pszSub, IN OPTIONAL int start = 0) const;

	int FindOneOf(LPCTSTR pszCharset);
	int findOneOf(LPCTSTR pszCharset);

	int reverseFind(IN TCHAR ch) const;
	int ReverseFind(IN TCHAR ch) const
	{
		return reverseFind(ch);
	};

	SOCmnString left(IN int nCount) const;
	SOCmnString Left(IN int nCount) const
	{
		return left(nCount);
	};
	SOCmnString right(IN int nCount) const;
	SOCmnString Right(IN int nCount) const
	{
		return right(nCount);
	};

	SOCmnString mid(IN int nFirst) const;
	SOCmnString Mid(IN int nFirst) const
	{
		return mid(nFirst);
	};
	SOCmnString mid(IN int nFirst, int nCount) const;
	SOCmnString Mid(IN int nFirst, int nCount) const
	{
		return mid(nFirst, nCount);
	};

	// hashing function djb2
	SOCmnString hashDJB2(SOCmnString secret) const;

	void trimRight(void);
	void TrimRight(void)
	{
		trimRight();
	};
	void trimLeft(void);
	void TrimLeft(void)
	{
		trimLeft();
	};

#ifdef SOOS_WINDOWS
	SOCmnString(IN LPOLESTR szOle);

	// create wide character string
	LPWSTR cloneToWCHAR(void);
	static LPWSTR cloneToWCHAR(IN const SOCmnString& str);

	// Init from WCHAR
	void fromWCHAR(LPCWSTR wszSrc);

	// load string from resource
	int loadString(HINSTANCE hInstance, UINT nID);
	int LoadString(HINSTANCE hInstance, UINT nID)
	{
		return loadString(hInstance, nID);
	};
#endif // SOOS_WINDOWS

	void setCHAR(LPCSTR src, DWORD srcLen);
	void set(LPCTSTR src, DWORD srcLen);
	void setUTF8(LPCSTR src, DWORD srcLen);

#ifdef SOFEATURE_CMN_XML
	static void doXMLReplacements(SOCmnString& xmlStr);
#endif // SOFEATURE_CMN_XML

protected:
	TCHAR* m_string;    // string data

	void setData(IN LPTSTR data);
	LPTSTR addData(IN LPCTSTR data, IN DWORD numChars);
}; // SOCmnString


bool operator==(const SOCmnString& s1, const SOCmnString& s2);
bool operator==(const SOCmnString& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const SOCmnString& s2);
bool operator!=(const SOCmnString& s1, const SOCmnString& s2);
bool operator!=(const SOCmnString& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const SOCmnString& s2);

inline bool operator==(const SOCmnString& s1, const SOCmnString& s2)
{
	return  !!SOCmnString::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator==(const SOCmnString& s1, LPCTSTR s2)
{
	return !!SOCmnString::areEqual((LPCTSTR)s1, s2);
}

inline bool operator==(LPCTSTR s1, const SOCmnString& s2)
{
	return !!SOCmnString::areEqual(s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnString& s1, const SOCmnString& s2)
{
	return !SOCmnString::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnString& s1, LPCTSTR s2)
{
	return !SOCmnString::areEqual((LPCTSTR)s1, s2);
}

inline bool operator!=(LPCTSTR s1, const SOCmnString& s2)
{
	return !SOCmnString::areEqual(s1, (LPCTSTR)s2);
}

inline DWORD SOCmnString::GetLength(void) const
{
	return getLength();
}

inline BOOL SOCmnString::IsEmpty(void) const
{
	return isEmpty();
}

inline int SOCmnString::Replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew)
{
	return replace(pszOld, pszNew);
}

inline void SOCmnString::MakeLower(void)
{
	makeLower();
}

inline void SOCmnString::MakeUpper(void)
{
	makeUpper();
}

inline void SOCmnString::Empty(void)
{
	empty();
}

inline LPTSTR SOCmnString::GetBuffer(IN DWORD minBufferLen)
{
	return getBuffer(minBufferLen);
}

inline int SOCmnString::Find(TCHAR ch, int start) const
{
	return find(ch, start);
};

// find a sub-string (like strstr)
inline int SOCmnString::Find(LPCTSTR pszSub, int start) const
{
	return find(pszSub, start);
};

inline int SOCmnString::FindOneOf(LPCTSTR pszCharset)
{
	return findOneOf(pszCharset);
}


#ifdef SOFEATURE_CMN_STRINGEX

//-----------------------------------------------------------------------------
// SOCmnStringEx                                                              |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnStringEx
{
public:
	SOCmnStringEx();
	SOCmnStringEx(IN TCHAR c);
	SOCmnStringEx(IN LPCTSTR sz);
	SOCmnStringEx(IN const SOCmnStringEx* str);
	SOCmnStringEx(IN const SOCmnStringEx& str);

	~SOCmnStringEx();

	operator LPCTSTR() const;   // get string buffer
	operator LPTSTR();          // get string buffer

	// add string to the existing string
	const SOCmnStringEx& operator+=(IN const SOCmnStringEx& str);
	const SOCmnStringEx& operator+=(IN const SOCmnString& str);
	const SOCmnStringEx& operator+=(IN LPCTSTR sz);
	const SOCmnStringEx& operator+=(IN TCHAR c);

	// set string data
	const SOCmnStringEx& operator=(IN const SOCmnStringEx& str);
	const SOCmnStringEx& operator=(IN const SOCmnString& str);
	const SOCmnStringEx& operator=(IN LPCTSTR sz);
	const SOCmnStringEx& operator=(IN TCHAR c);

	// add string to another string
	SOCmnStringEx operator+(IN const SOCmnStringEx& str);
	SOCmnStringEx operator+(IN const SOCmnString& str);
	SOCmnStringEx operator+(IN LPCTSTR sz);
	SOCmnStringEx operator+(IN TCHAR c);

	// get length of the string
	DWORD getLength(void) const;

	// check if string is empty
	BOOL isEmpty(void) const;

	// empty string
	void empty(void);

	// set string with format
	LPCTSTR format(IN LPCTSTR format, ...) FORMAT_PRINTF(2, 3);

	// replace indicated characters with other characters
	int replace(IN LPCTSTR pszOld, IN LPCTSTR pszNew);

	void makeLower(IN OPTIONAL DWORD startIdx = 0);
	void makeUpper(IN OPTIONAL DWORD startIdx = 0);

	// get string buffer
	LPTSTR getBuffer(IN DWORD minBufferLen);

	// check if two strings are equal
	static BOOL areEqual(IN LPCTSTR sz1, IN LPCTSTR sz2);

	// search in the string
	int find(IN TCHAR ch, IN OPTIONAL int start = 0) const;
	int find(IN LPCTSTR pszSub, IN OPTIONAL int start = 0) const;

	int findOneOf(LPCTSTR pszCharset);

	int reverseFind(IN TCHAR ch) const;

	SOCmnStringEx left(IN int nCount) const;
	SOCmnStringEx right(IN int nCount) const;

	SOCmnStringEx mid(IN int nFirst) const;
	SOCmnStringEx mid(IN int nFirst, int nCount) const;

	void trimRight(void);
	void trimLeft(void);

#ifdef SOOS_WINDOWS
	// create wide character string
	LPWSTR cloneToWCHAR(void);
	static LPWSTR cloneToWCHAR(IN SOCmnStringEx& str);

	// Init from WCHAR
	void fromWCHAR(LPCWSTR wszSrc);
#endif // SOOS_WINDOWS

	BYTE* cloneToUTF8(OUT DWORD* pLength);
	static void setCHARWithUTF8(BYTE** pDest, DWORD* destLen, BYTE* pSrc, DWORD srcLen);

	CHAR* cloneToCHAR(OUT DWORD* pLength);

	void setCHAR(LPCSTR src, DWORD srcLen);
	void set(LPCTSTR src, DWORD srcLen);
	void addCHAR(LPCSTR src, DWORD srcLen);

	void setUTF8(LPCSTR src, DWORD srcLen);

	void attachTo(IN LPTSTR data, IN DWORD len);
	void replace(IN DWORD startIdx, IN DWORD endIdxT, LPCTSTR pNewData, DWORD len);

#ifdef SOFEATURE_CMN_XML
	static void doXMLReplacements(SOCmnStringEx& xmlStr);
#ifdef SOOS_LINUX
	static LPCTSTR getXMLString(LPCTSTR str, SOCmnStringEx& xmlStr);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	static LPCTSTR getXMLString(LPCWSTR bStr, SOCmnStringEx& xmlStr);
	static LPCTSTR getXMLString(LPCSTR str, SOCmnStringEx& xmlStr);
#endif
	static LPCTSTR setXMLString(LPCTSTR xmlStr, SOCmnStringEx& outStr);
#endif // SOFEATURE_CMN_XML

protected:
	TCHAR* m_string;    // string data
	DWORD m_bufferLen; // length of the string buffer
	DWORD m_stringLen; // length of the string

	void setDataC(IN LPCTSTR data);
	void setDataC(IN LPCTSTR data, IN DWORD len);
	LPTSTR addData(IN LPCTSTR data, IN DWORD numChars);
}; // SOCmnStringEx


bool operator==(const SOCmnStringEx& s1, const SOCmnStringEx& s2);
bool operator==(const SOCmnStringEx& s1, const SOCmnString& s2);
bool operator==(const SOCmnString& s1, const SOCmnString& s2);
bool operator==(const SOCmnStringEx& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const SOCmnStringEx& s2);
bool operator!=(const SOCmnStringEx& s1, const SOCmnStringEx& s2);
bool operator!=(const SOCmnString& s1, const SOCmnStringEx& s2);
bool operator!=(const SOCmnStringEx& s1, const SOCmnString& s2);
bool operator!=(const SOCmnStringEx& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const SOCmnStringEx& s2);

inline bool operator==(const SOCmnStringEx& s1, const SOCmnStringEx& s2)
{
	return !!SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator==(const SOCmnString& s1, const SOCmnStringEx& s2)
{
	return !!SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator==(const SOCmnStringEx& s1, const SOCmnString& s2)
{
	return !!SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator==(const SOCmnStringEx& s1, LPCTSTR s2)
{
	return !!SOCmnStringEx::areEqual((LPCTSTR)s1, s2);
}

inline bool operator==(LPCTSTR s1, const SOCmnStringEx& s2)
{
	return !!SOCmnStringEx::areEqual(s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnStringEx& s1, const SOCmnStringEx& s2)
{
	return !SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnStringEx& s1, const SOCmnString& s2)
{
	return !SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnString& s1, const SOCmnStringEx& s2)
{
	return !SOCmnStringEx::areEqual((LPCTSTR)s1, (LPCTSTR)s2);
}

inline bool operator!=(const SOCmnStringEx& s1, LPCTSTR s2)
{
	return !SOCmnStringEx::areEqual((LPCTSTR)s1, s2);
}

inline bool operator!=(LPCTSTR s1, const SOCmnStringEx& s2)
{
	return !SOCmnStringEx::areEqual(s1, (LPCTSTR)s2);
}

#endif // SOFEATURE_CMN_STRINGEX

#pragma pack(pop)
#endif
