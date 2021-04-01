#include "stdafx.h"

#ifdef SOCLT

#include "SOCltStorage.h"

//-----------------------------------------------------------------------------
// SOCltStorage
//-----------------------------------------------------------------------------

SOCltStorage::SOCltStorage()
{
	m_nGrowBytes = 1024;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	m_lpBuffer = NULL;
}

SOCltStorage::SOCltStorage(IN   DWORD size)
{
	m_nFileSize = size;
	m_nGrowBytes = 1024;
	m_nPosition = 0;
	m_nBufferSize = size;
	m_lpBuffer = memAlloc(size);
}


SOCltStorage::~SOCltStorage()
{
	close();
}

BYTE* SOCltStorage::memAlloc(IN DWORD nBytes)
{
	void* p = ::malloc((UINT)nBytes);

	if (p == NULL)
	{
		SOCMN_THROW_EXCEPTION_OUTOPFMEMORY();
	}

	return (BYTE*)p;
}

BYTE* SOCltStorage::memRealloc(IN BYTE* lpMem, IN DWORD nBytes)
{
	void* p = ::realloc(lpMem, (UINT)nBytes);

	if (p == NULL)
	{
		SOCMN_THROW_EXCEPTION_OUTOPFMEMORY();
	}

	return (BYTE*)p;
}

void SOCltStorage::memFree(BYTE* lpMem)
{
	_ASSERTION(lpMem != NULL, "Invalid pointer");
	::free(lpMem);
}

BYTE* SOCltStorage::memCopy(
	OUT BYTE* lpMemTarget,
	IN const BYTE* lpMemSource,
	IN UINT nBytes)
{
	_ASSERTION(lpMemTarget != NULL, "NULL pointer passed");
	_ASSERTION(lpMemSource != NULL, "NULL pointer passed");
	return (BYTE*)::memcpy(lpMemTarget, lpMemSource, nBytes);
}



void SOCltStorage::memGrow(IN DWORD dwNewLen)
{
	if (dwNewLen > m_nBufferSize)
	{
		// grow the buffer
		DWORD dwNewBufferSize = (DWORD)m_nBufferSize;
		// watch out for buffers which cannot be grown!
		_ASSERTION(m_nGrowBytes != 0, "m_nGrowBytes cannot be 0");

		if (0 == m_nGrowBytes)
		{
			SOCMN_THROW_EXCEPTION_OUTOPFMEMORY();
		}

		// determine new buffer size
		while (dwNewBufferSize < dwNewLen)
		{
			dwNewBufferSize += m_nGrowBytes;
		}

		// allocate new buffer
		BYTE* lpNew;

		if (m_lpBuffer == NULL)
		{
			lpNew = memAlloc(dwNewBufferSize);
		}
		else
		{
			lpNew = memRealloc(m_lpBuffer, dwNewBufferSize);
		}

		m_lpBuffer = lpNew;
		m_nBufferSize = dwNewBufferSize;
	}
}


UINT SOCltStorage::read(OUT void* lpvTrg, IN UINT nCount)
{
	if (nCount == 0)
	{
		return 0;
	}

	if (m_nPosition > m_nFileSize)
	{
		return 0;
	}

	UINT nRead;

	if (m_nPosition + nCount > m_nFileSize)
	{
		nRead = (UINT)(m_nFileSize - m_nPosition);
	}
	else
	{
		nRead = nCount;
	}

	memCopy((BYTE*)lpvTrg, (BYTE*)(m_lpBuffer + m_nPosition), nRead);
	m_nPosition += nRead;
	return nRead;
}

void SOCltStorage::write(IN const void* lpvSrc, IN UINT nCount)
{
	if (nCount == 0)
	{
		return;
	}

	if (m_nPosition + nCount > m_nBufferSize)
	{
		memGrow(m_nPosition + nCount);
	}

	_ASSERTION(m_nPosition + nCount <= m_nBufferSize, "Out of buffer");
	memCopy((BYTE*)m_lpBuffer + m_nPosition, (BYTE*)lpvSrc, nCount);
	m_nPosition += nCount;

	if (m_nPosition > m_nFileSize)
	{
		m_nFileSize = m_nPosition;
	}
}

LONG SOCltStorage::seek(IN LONG lOffset, IN UINT nFrom)
{
	LONG lNewPos = m_nPosition;

	switch (nFrom)
	{
	default:
	{
		// invalid arg
		_ASSERTION(FALSE, "Invalid enum value");
		return -1;
	}
	break;

	case begin:
		lNewPos = lOffset;
		break;

	case current:
		lNewPos += lOffset;
		break;

	case end:
		lNewPos = m_nFileSize + lOffset;
		break;
	}

	if (lNewPos < 0)
	{
		// bad seek
		_ASSERTION(FALSE, "Bad seek");
		return -1;
	}

	m_nPosition = lNewPos;
	return m_nPosition;
}


void SOCltStorage::close(void)
{
	if (NULL == m_lpBuffer)
	{
		return;
	}

	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	memFree(m_lpBuffer);
	m_lpBuffer = NULL;
}

// Strings are strored as follows:
// 0   1       5
// +---+--------+-----------......------+
// |s/w| length | <actual string  data> |
// +---+--------+-----------......------+

SOCltStorage& SOCltStorage::operator<<(IN LPCSTR sz)
{
	DWORD dwLen = sz ? (DWORD)strlen(sz) : 0;
	*this << 's' << dwLen;
	write(sz, dwLen);
	return *this;
}

#ifdef SOOS_WINDOWS
SOCltStorage& SOCltStorage::operator<<(IN LPCWSTR wsz)
{
	_ASSERTION(wsz ? (!::IsBadStringPtrW(wsz, 0xFFFFFFFF)) : 1, "Invalid string");
	DWORD dwLen = wsz ? (DWORD)wcslen(wsz) : 0;
	*this << 'w' << dwLen;
	write(wsz, dwLen * sizeof(WCHAR));
	return *this;
}
#endif

SOCltStorage& SOCltStorage::operator>>(OUT LPSTR& sz)
{
	sz = NULL;
	char flag = 0;
	DWORD dwLen = 0;
	*this >> flag;
	*this >> dwLen;

	if ('s' != flag)
	{
		// Not a valid string
		_ASSERTION(FALSE, "Invalid archive string");
		SOCMN_THROW_EXCEPTION_SERIALIZATION();
	}
	else
	{
		sz = (LPSTR)memAlloc(dwLen + 1);

		if (sz)
		{
			read(sz, dwLen);
			sz[dwLen] = 0;
		}
	}

	return *this;
}

#ifdef SOOS_WINDOWS
SOCltStorage& SOCltStorage::operator>>(OUT LPWSTR& wsz)
{
	wsz = NULL;
	char flag = 0;
	DWORD dwLen = 0;
	*this >> flag;
	*this >> dwLen;

	if ('w' != flag)
	{
		// Not a valid string
		_ASSERTION(FALSE, "Invalid archive string");
		SOCMN_THROW_EXCEPTION_SERIALIZATION();
	}
	else
	{
		wsz = (LPWSTR)memAlloc(sizeof(WCHAR) * (dwLen + 1));

		if (wsz)
		{
			read(wsz, dwLen * sizeof(WCHAR));
			wsz[dwLen] = 0;
		}
	}

	return *this;
}
#endif

SOCltStorage& SOCltStorage::operator>>(OUT SOCmnString& str)
{
	char flag = 0;
	DWORD dwLen = 0;
	DWORD dwSize = 0;
	LPTSTR lpstr = NULL;
	*this >> flag;
	*this >> dwLen;

	if (flag != 's' && flag != 'w')
	{
		// Not a valid string
		_ASSERTION(FALSE, "Invalid string");
		return *this;
	}

	dwSize = dwLen;

	if ('w' == flag)
	{
		dwSize *= sizeof(WCHAR);
	}

	if (m_nPosition + dwSize > m_nFileSize)
	{
		_ASSERTION(FALSE, "Invalid archive string");
		SOCMN_THROW_EXCEPTION_SERIALIZATION();
		//return *this;
	}

	if (NULL == (lpstr = str.getBuffer(dwLen + 1)))
	{
		_ASSERTION(FALSE, "Invalid archive string");
		return *this;
	}

#ifdef SOOS_WINDOWS
#ifdef _UNICODE

	if ('w' == flag)
	{
		read(lpstr, dwSize);
	}
	else
	{
		::MultiByteToWideChar(::GetACP(), 0, (char*)(m_lpBuffer + m_nPosition),
							  dwLen, lpstr, dwLen);
		m_nPosition += dwSize;
	}

	lpstr[dwLen] = 0;
#else // !_UNICODE

	if ('s' == flag)
	{
		read(lpstr, dwSize);
	}
	else
	{
		::WideCharToMultiByte(::GetACP(), 0, (WCHAR*)(m_lpBuffer + m_nPosition),
							  dwLen, lpstr, dwLen, NULL, NULL);
		m_nPosition += dwSize;
	}

	lpstr[dwLen] = 0;
#endif //_UNICODE
#else
	read(lpstr, dwSize);
#endif // SOOS_WINDOWS
	return *this;
}

#endif // SOCLT
