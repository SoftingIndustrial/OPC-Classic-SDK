#ifndef _SOCLTSTORAGE_H_
#define _SOCLTSTORAGE_H_

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCltStorage
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltStorage
{
public:
	enum {begin, current, end}; // offset constants

public:
	SOCltStorage();                                             // Store
	SOCltStorage(IN DWORD size);                    // Load

	virtual ~SOCltStorage();

	virtual LONG seek(IN LONG lOffset, IN UINT nFrom);
	virtual UINT read(OUT void* lpvTrg, IN UINT nCount);
	virtual void write(IN const void* lpvSrc, IN UINT nCount);
	virtual void close(void);
	virtual DWORD getPosition(void) const;

	// insertion operations
	SOCltStorage& operator<<(IN BYTE b);
	SOCltStorage& operator<<(IN WORD w);
	SOCltStorage& operator<<(IN DWORD dw);
	SOCltStorage& operator<<(IN double d);
	SOCltStorage& operator<<(IN float d);
	SOCltStorage& operator<<(IN int i);
	SOCltStorage& operator<<(IN short s);
	SOCltStorage& operator<<(IN char ch);
	SOCltStorage& operator<<(IN unsigned u);

#ifdef WIN64
	SOCltStorage& operator<<(IN ptrdiff_t u);
#endif

#ifdef SOOS_WINDOWS
	SOCltStorage& operator<<(IN GUID guid);
#endif
	SOCltStorage& operator<<(IN LPCSTR sz);
#ifdef SOOS_WINDOWS
	SOCltStorage& operator<<(IN LPCWSTR wsz);
#endif

	// extraction operations
	SOCltStorage& operator>>(OUT BYTE& b);
	SOCltStorage& operator>>(OUT WORD& w);
	SOCltStorage& operator>>(OUT DWORD& dw);
	SOCltStorage& operator>>(OUT LONG& l);
	SOCltStorage& operator>>(OUT float& f);
	SOCltStorage& operator>>(OUT double& d);

	SOCltStorage& operator>>(OUT int& i);
	SOCltStorage& operator>>(OUT short& s);
	SOCltStorage& operator>>(OUT char& ch);
	SOCltStorage& operator>>(OUT unsigned& u);

#ifdef WIN64
	SOCltStorage& operator>>(OUT ptrdiff_t& u);
#endif

#ifdef SOOS_WINDOWS
	SOCltStorage& operator>>(OUT GUID& guid);
#endif
	SOCltStorage& operator>>(OUT SOCmnString& str);

	// Memory allocated usig malloc() and must
	// be freed explicitly.
	SOCltStorage& operator>>(OUT LPSTR& sz);
#ifdef SOOS_WINDOWS
	SOCltStorage& operator>>(OUT LPWSTR& wsz);
#endif

	void* getBuffer() const;

protected:
	virtual BYTE* memAlloc(IN DWORD nBytes);
	virtual BYTE* memRealloc(IN BYTE* lpMem, IN DWORD nBytes);
	virtual void memFree(IN BYTE* lpMem);

	virtual BYTE* memCopy(OUT BYTE* lpMemTarget, const IN BYTE* lpMemSource, IN UINT nBytes);
	virtual void memGrow(IN DWORD dwNewLen);

protected:
	UINT m_nGrowBytes;      // grow by
	DWORD m_nPosition;      // current position
	DWORD m_nBufferSize;    // valid data size, <= memory size
	DWORD m_nFileSize;      // raw memory size
	BYTE* m_lpBuffer;           // pointer to raw memory
}; // SOCltStorage

inline DWORD SOCltStorage::getPosition(void) const
{
	return m_nPosition;
}

inline void* SOCltStorage::getBuffer() const
{
	return m_lpBuffer;
}

inline SOCltStorage& SOCltStorage::operator<<(IN BYTE b)
{
	write(&b, sizeof(b));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN WORD w)
{
	write(&w, sizeof(w));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN int i)
{
	write(&i, sizeof(i));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN short s)
{
	write(&s, sizeof(s));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN char ch)
{
	write(&ch, sizeof(ch));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN unsigned u)
{
	write(&u, sizeof(u));
	return *this;
}

#ifdef WIN64
inline SOCltStorage& SOCltStorage::operator<<(IN ptrdiff_t u)
{
	write(&u, sizeof(u));
	return *this;
}
#endif


inline SOCltStorage& SOCltStorage::operator<<(IN DWORD dw)
{
	write(&dw, sizeof(dw));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN double d)
{
	write(&d, sizeof(d));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator<<(IN float d)
{
	write(&d, sizeof(d));
	return *this;
}

#ifdef SOOS_WINDOWS
inline SOCltStorage& SOCltStorage::operator<<(IN GUID guid)
{
	write(&guid, sizeof(guid));
	return *this;
}
#endif

#ifndef SOOS_WINDOWS_CE
#define _SOCLTSTORAGE_SAFE_READ_(r, x) \
  {if (x != read(r,x)) SOCMN_THROW_EXCEPTION_SERIALIZATION();}
#else
#define _SOCLTSTORAGE_SAFE_READ_(r, x)
#endif

inline SOCltStorage& SOCltStorage::operator>>(OUT BYTE& b)
{
	_SOCLTSTORAGE_SAFE_READ_(&b, sizeof(b));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT WORD& w)
{
	_SOCLTSTORAGE_SAFE_READ_(&w, sizeof(w));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT DWORD& dw)
{
	_SOCLTSTORAGE_SAFE_READ_(&dw, sizeof(dw));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT LONG& l)
{
	_SOCLTSTORAGE_SAFE_READ_(&l, sizeof(l));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT float& f)
{
	_SOCLTSTORAGE_SAFE_READ_(&f, sizeof(f));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT double& d)
{
	_SOCLTSTORAGE_SAFE_READ_(&d, sizeof(d));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT int& i)
{
	_SOCLTSTORAGE_SAFE_READ_(&i, sizeof(i));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT short& s)
{
	_SOCLTSTORAGE_SAFE_READ_(&s, sizeof(s));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT char& ch)
{
	_SOCLTSTORAGE_SAFE_READ_(&ch, sizeof(ch));
	return *this;
}

inline SOCltStorage& SOCltStorage::operator>>(OUT unsigned& u)
{
	_SOCLTSTORAGE_SAFE_READ_(&u, sizeof(u));
	return *this;
}

#ifdef WIN64
inline SOCltStorage& SOCltStorage::operator>>(IN ptrdiff_t& u)
{
	write(&u, sizeof(u));
	return *this;
}
#endif

#ifdef SOOS_WINDOWS
inline SOCltStorage& SOCltStorage::operator>>(OUT GUID& guid)
{
	_SOCLTSTORAGE_SAFE_READ_(&guid, sizeof(guid));
	return *this;
}
#endif

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SOCltAutoMemory
//-----------------------------------------------------------------------------

template <class T>
class CComDeallocator
{
public:

	static void Free(T* pData)
	{
		if (pData)
		{
			::CoTaskMemFree(pData);
		}
	}
};

template <class T, class DeAllocator = CComDeallocator < T > >
class SOCltAutoMemory
{
public:
	SOCltAutoMemory()
	{
		m_pData = NULL;
	}

	SOCltAutoMemory(T* pData)
	{
		m_pData = pData;
	}

	~SOCltAutoMemory()
	{
		Attach(NULL);
	}

	void Attach(T* pData)
	{
		DeAllocator::Free(m_pData);
		m_pData = pData;
	}

	T* Detach()
	{
		T* pTemp = m_pData;
		m_pData = NULL;
		return pTemp;
	}

	const SOCltAutoMemory<T, DeAllocator>& operator =(T* pData)
	{
		Attach(pData);
		return *this;
	}

	operator T* ()
	{
		return m_pData;
	}

	T** operator &()
	{
		return &m_pData;
	}

	bool operator!() const
	{
		return (NULL == m_pData);
	}

	T* m_pData;
};

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// Translation TCHAR - WCHAR on the stack
//-----------------------------------------------------------------------------


#ifdef _UNICODE
#define USES_ALLOCA_WSTR    USES_CONVERSION; LPCWSTR _lpw2;
#define ALLOCA_WSTR(s) ((_lpw2 = s) == NULL ? \
        NULL : lstrcpyW((LPWSTR)alloca(sizeof(WCHAR) * (lstrlenW(_lpw2) + 1)), _lpw2) )
#else
#define USES_ALLOCA_WSTR    USES_CONVERSION;
#define ALLOCA_WSTR(s) A2W(s)
#endif

//-----------------------------------------------------------------------------
// Serialization helpers
//-----------------------------------------------------------------------------

inline void serialize_list(IN SOCltStorage& ar, BOOL isStoring, SOCmnStringList& list)
{
	if (isStoring)
	{
		ar << list.getCount();
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			ar << list.getNext(pos);
		}
	}
	else
	{
		DWORD dwCount = 0;
		ar >> dwCount;

		while (dwCount--)
		{
			SOCmnString str;
			ar >> str;
			list.add(str);
		}
	}
}

inline void serialize_list(IN SOCltStorage& ar, BOOL isStoring, SOCmnDWORDList& list)
{
	if (isStoring)
	{
		ar << list.getCount();
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			ar << list.getNext(pos);
		}
	}
	else
	{
		DWORD dwCount = 0;
		ar >> dwCount;

		while (dwCount--)
		{
			DWORD dw = 0;
			ar >> dw;
			list.add(dw);
		}
	}
}

inline void serialize_list(IN SOCltStorage& ar, BOOL isStoring, SOCmnVOIDList& list)
{
	if (isStoring)
	{
		ar << list.getCount();
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			ar << (ptrdiff_t)list.getNext(pos);
		}
	}
	else
	{
		DWORD dwCount = 0;
		ar >> dwCount;

		while (dwCount--)
		{
			ptrdiff_t dw = 0;
			ar >> dw;
			list.add((void*)dw);
		}
	}
}

#pragma pack(pop)
#endif //_SOCLTSTORAGE_H_
