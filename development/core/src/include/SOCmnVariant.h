#ifndef _SOCMNVARIANT_H_
#define _SOCMNVARIANT_H_

#ifdef SOFEATURE_CMN_VARIANT

#pragma pack(push, 4)


#ifdef SOOS_LINUX

/*
 * VARENUM usage key,
 *
 * * [V] - may appear in a VARIANT
 * * [T] - may appear in a TYPEDESC
 * * [P] - may appear in an OLE property set
 * * [S] - may appear in a Safe Array
 *
 *
 *  VT_EMPTY            [V]   [P]     nothing
 *  VT_NULL             [V]   [P]     SQL style Null
 *  VT_I2               [V][T][P][S]  2 byte signed int
 *  VT_I4               [V][T][P][S]  4 byte signed int
 *  VT_R4               [V][T][P][S]  4 byte real
 *  VT_R8               [V][T][P][S]  8 byte real
 *  VT_CY               [V][T][P][S]  currency
 *  VT_DATE             [V][T][P][S]  date
 *  VT_BSTR             [V][T][P][S]  OLE Automation string
 *  VT_DISPATCH         [V][T][P][S]  IDispatch *
 *  VT_ERROR            [V][T][P][S]  SCODE
 *  VT_BOOL             [V][T][P][S]  True=-1, False=0
 *  VT_VARIANT          [V][T][P][S]  VARIANT *
 *  VT_UNKNOWN          [V][T]   [S]  IUnknown *
 *  VT_DECIMAL          [V][T]   [S]  16 byte fixed point
 *  VT_RECORD           [V]   [P][S]  user defined type
 *  VT_I1               [V][T][P][s]  signed char
 *  VT_UI1              [V][T][P][S]  unsigned char
 *  VT_UI2              [V][T][P][S]  unsigned short
 *  VT_UI4              [V][T][P][S]  unsigned short
 *  VT_I8                  [T][P]     signed 64-bit int
 *  VT_UI8                 [T][P]     unsigned 64-bit int
 *  VT_INT              [V][T][P][S]  signed machine int
 *  VT_UINT             [V][T]   [S]  unsigned machine int
 *  VT_VOID                [T]        C style void
 *  VT_HRESULT             [T]        Standard return type
 *  VT_PTR                 [T]        pointer type
 *  VT_SAFEARRAY           [T]        (use VT_ARRAY in VARIANT)
 *  VT_CARRAY              [T]        C style array
 *  VT_USERDEFINED         [T]        user defined type
 *  VT_LPSTR               [T][P]     null terminated string
 *  VT_LPWSTR              [T][P]     wide null terminated string
 *  VT_FILETIME               [P]     FILETIME
 *  VT_BLOB                   [P]     Length prefixed bytes
 *  VT_STREAM                 [P]     Name of the stream follows
 *  VT_STORAGE                [P]     Name of the storage follows
 *  VT_STREAMED_OBJECT        [P]     Stream contains an object
 *  VT_STORED_OBJECT          [P]     Storage contains an object
 *  VT_BLOB_OBJECT            [P]     Blob contains an object
 *  VT_CF                     [P]     Clipboard format
 *  VT_CLSID                  [P]     A Class ID
 *  VT_VECTOR                 [P]     simple counted array
 *  VT_ARRAY            [V]           SAFEARRAY*
 *  VT_BYREF            [V]           void* for local use
 *  VT_BSTR_BLOB                      Reserved for system use
 */

enum VARENUM
{
	VT_EMPTY    = 0,
	VT_I2   = 2,
	VT_I4   = 3,
	VT_R4   = 4,
	VT_R8   = 5,
	VT_CY   = 6,
	VT_DATE = 7,
	VT_BSTR = 8,
	VT_BOOL = 11,
	VT_VARIANT  = 12,
	VT_I1   = 16,
	VT_UI1  = 17,
	VT_UI2  = 18,
	VT_UI4  = 19,
	VT_I8   = 20,
	VT_UI8  = 21,
	VT_INT  = 22,
	VT_UINT = 23,
	VT_ARRAY    = 0x2000,
	VT_RESERVED = 0x8000,
	VT_ILLEGAL  = 0xffff,
	VT_ILLEGALMASKED    = 0xfff,
	VT_TYPEMASK = 0xfff
};

#define V_UNION(X, Y)   ((X)->Y)
#define V_VT(X)         ((X)->vt)
#define V_ISARRAY(X)     (V_VT(X)&VT_ARRAY)
#define V_UI1(X)         V_UNION(X, bVal)
#define V_I2(X)          V_UNION(X, iVal)
#define V_I4(X)          V_UNION(X, lVal)
#define V_I8(X)          V_UNION(X, llVal)
#define V_R4(X)          V_UNION(X, fltVal)
#define V_R8(X)          V_UNION(X, dblVal)
#define V_I1(X)          V_UNION(X, cVal)
#define V_UI2(X)         V_UNION(X, uiVal)
#define V_UI4(X)         V_UNION(X, ulVal)
#define V_UI8(X)         V_UNION(X, ullVal)
#define V_INT(X)         V_UNION(X, intVal)
#define V_UINT(X)        V_UNION(X, uintVal)
#define V_CY(X)          V_UNION(X, cyVal)
#define V_DATE(X)        V_UNION(X, date)
#define V_BSTR(X)        V_UNION(X, bstrVal)
#define V_BOOL(X)        V_UNION(X, boolVal)
#define V_ARRAY(X)       V_UNION(X, parray)

BSTR SysAllocString(LPCTSTR sz);
BSTR SysAllocStringLen(LPCTSTR pch, ULONG cch);
void SysFreeString(BSTR bstr);
ULONG SysStringLen(BSTR bstr);
#define SysStringByteLen(bstr)   SysStringLen(bstr)
#define SysAllocStringByteLen(psz, len)   SysAllocStringLen(psz, len)

void VariantInit(LPVARIANT pvarg);
HRESULT VariantClear(LPVARIANT pvarg);
HRESULT VariantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc);
HRESULT VariantChangeType(LPVARIANT pvargDest,
						  LPVARIANT pvarSrc, USHORT wFlags, VARTYPE vt);
HRESULT VariantChangeTypeEx(LPVARIANT pvargDest,
							LPVARIANT pvarSrc, LCID lcid, USHORT wFlags, VARTYPE vt);

// Flags for VariantChangeType/VariantChangeTypeEx
#define VARIANT_NOVALUEPROP      0x01
#define VARIANT_ALPHABOOL        0x02 // For VT_BOOL to VT_BSTR conversions,
// convert to "True"/"False" instead of
// "-1"/"0"
#define VARIANT_NOUSEROVERRIDE   0x04 // For conversions to/from VT_BSTR,
// passes LOCALE_NOUSEROVERRIDE
// to core coercion routines
#define VARIANT_CALENDAR_HIJRI   0x08
#define VARIANT_LOCALBOOL        0x10 // For VT_BOOL to VT_BSTR and back,
// convert to local language rather than
// English

LPSAFEARRAY SafeArrayCreate(VARTYPE vt, USHORT cDims, LPSAFEARRAYBOUND rgsabound);
LPSAFEARRAY SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements);
HRESULT SafeArrayDestroy(LPSAFEARRAY psa);
HRESULT SafeArrayCopy(LPSAFEARRAY psa, LPSAFEARRAY* ppsaOut);
HRESULT SafeArrayRedim(SAFEARRAY* psa, SAFEARRAYBOUND* psaboundNew);
USHORT  SafeArrayGetDim(LPSAFEARRAY psa);
ULONG   SafeArrayGetElemsize(LPSAFEARRAY psa);
HRESULT SafeArrayGetUBound(LPSAFEARRAY psa, USHORT nDim, LONG* plUbound);
HRESULT SafeArrayGetLBound(LPSAFEARRAY psa, USHORT nDim, LONG* plLbound);
HRESULT SafeArrayAccessData(LPSAFEARRAY psa, void** ppvData);
HRESULT SafeArrayUnaccessData(LPSAFEARRAY psa);
HRESULT SafeArrayGetElement(LPSAFEARRAY psa, LONG* rgIndices, void* pv);
HRESULT SafeArrayPutElement(LPSAFEARRAY psa, LONG* rgIndices, void* pv);

#endif // SOOS_LINUX

//-----------------------------------------------------------------------------
// SOCmnVariant                                                               |
//-----------------------------------------------------------------------------

/* compatibility with old windows.h,
 * where there was no V_I8 and V_UI8.
 */

#ifndef V_I8
#define V_I8(X)          ((X)->cyVal.int64)
#endif
#ifndef V_UI8
#define V_UI8(X)         (*(ULONGLONG*)&V_I8(X))
#endif

#ifndef LPCVARIANT
typedef const VARIANT* LPCVARIANT;
#endif

class SOCmnXmlValue;

class SOCMN_EXPORT SOCmnVariant : public tagVARIANT
{
public:
	SOCmnVariant();
	SOCmnVariant(IN const SOCmnVariant& cmnVar);
	SOCmnVariant(IN const VARIANT& var);
	SOCmnVariant(IN LPCVARIANT pvar);
	SOCmnVariant(IN const LPCTSTR sz);
	SOCmnVariant(IN char c);
	SOCmnVariant(IN BYTE b);
	SOCmnVariant(IN short s);
	SOCmnVariant(IN unsigned short us);
	SOCmnVariant(IN long l);
	SOCmnVariant(IN unsigned long ul);
	SOCmnVariant(IN int i);
	SOCmnVariant(IN unsigned int ui);
	SOCmnVariant(IN float flt);
	SOCmnVariant(IN double dbl);
	SOCmnVariant(IN bool bo);
	SOCmnVariant(IN CURRENCY cy);
	SOCmnVariant(IN LONGLONG i8);
	SOCmnVariant(IN ULONGLONG ui8);
	~SOCmnVariant();

	const SOCmnVariant& operator=(IN SOCmnVariant& cmnVar);
	const SOCmnVariant& operator=(IN const VARIANT& var);
	const SOCmnVariant& operator=(IN LPCVARIANT pvar);
	const SOCmnVariant& operator=(IN const LPCTSTR sz)
	{
		setBSTR(sz);
		return *this;
	}
	const SOCmnVariant& operator=(IN char c)
	{
		setI1(c);
		return *this;
	}
	const SOCmnVariant& operator=(IN BYTE b)
	{
		setUI1(b);
		return *this;
	}
	const SOCmnVariant& operator=(IN short s)
	{
		setI2(s);
		return *this;
	}
	const SOCmnVariant& operator=(IN unsigned short us)
	{
		setUI2(us);
		return *this;
	}
	const SOCmnVariant& operator=(IN long l)
	{
		setI4(l);
		return *this;
	}
	const SOCmnVariant& operator=(IN unsigned long ul)
	{
		setUI4(ul);
		return *this;
	}
	const SOCmnVariant& operator=(IN int i)
	{
		setINT(i);
		return *this;
	}
	const SOCmnVariant& operator=(IN unsigned int ui)
	{
		setUINT(ui);
		return *this;
	}
	const SOCmnVariant& operator=(IN float flt)
	{
		setR4(flt);
		return *this;
	}
	const SOCmnVariant& operator=(IN double dbl)
	{
		setR8(dbl);
		return *this;
	}
	const SOCmnVariant& operator=(IN bool bo)
	{
		setBOOL(bo);
		return *this;
	}
	const SOCmnVariant& operator=(IN CURRENCY cy)
	{
		setCY(cy.int64);
		return *this;
	}
	const SOCmnVariant& operator=(IN LONGLONG i8)
	{
		setI8(i8);
		return *this;
	}
	const SOCmnVariant& operator=(IN ULONGLONG ui8)
	{
		setUI8(ui8);
		return *this;
	}

	void setBSTR(IN const LPCTSTR sz);
	void setI1(IN char c);
	void setUI1(IN BYTE b);
	void setI2(IN short s);
	void setUI2(IN unsigned short us);
	void setI4(IN long l);
	void setUI4(IN unsigned long ul);
	void setINT(IN int i);
	void setUINT(IN unsigned int ui);
	void setR4(IN float flt);
	void setR8(IN double dbl);
	void setBOOL(IN bool bo);
	void setCY(IN LONGLONG cy);
	void setI8(IN LONGLONG i8);
	void setUI8(IN ULONGLONG ui8);
	void setDATE(IN DATE date);

	LPCTSTR getBSTR(SOCmnString& buffer);

#ifdef SOOS_LINUX
	SOCmnVariant(IN DATE dt);
	const SOCmnVariant& operator=(IN DATE dt)
	{
		setDATE(dt);
		return *this;
	}
#endif

	SOCmnVariant& set(IN void* src, IN VARTYPE type, IN WORD elSize);
	DWORD getSize(void);
	static DWORD getSize(VARTYPE varType);

	void get(IN void* dest);

	operator LPVARIANT();
	operator LPCVARIANT() const;

	BOOL operator==(IN const VARIANT& var) const;
	BOOL operator==(IN LPCVARIANT pvar) const;
	BOOL operator==(IN const SOCmnVariant& cmnVar) const;

	BOOL operator!=(IN const VARIANT& var) const;
	BOOL operator!=(IN LPCVARIANT pvar) const;
	BOOL operator!=(IN const SOCmnVariant& cmnVar) const;

	// clear variant
	void Clear(void);
	void clear(void);

	BOOL copyTo(OUT LPVARIANT destination);
	BOOL initFrom(IN LPCVARIANT source);

	LPCTSTR toString(OUT SOCmnString* string) const;

	void init(IN VARTYPE datatype);
	void initArray(VARTYPE datatype, DWORD arraySize, DWORD lBound = 0);

	static BOOL supportDatatype(IN VARTYPE dataType);

	// datatype conversion
	BOOL changeDatatype(IN VARTYPE type);
	static BOOL changeDatatype(IN VARIANT* value, IN VARTYPE type, OUT VARIANT* changedValue,
							   OPTIONAL OUT HRESULT* pError = NULL);
	static BOOL changeDatatype(IN VARIANT* value, IN VARTYPE type, IN LCID localeID,
							   OUT VARIANT* changedValue, OPTIONAL OUT HRESULT* pError = NULL);

	// compare variants
	static BOOL areEqual(IN LPCVARIANT pvar1, IN LPCVARIANT pvar2);
	static BOOL areEqualArrays(IN LPCVARIANT pvar1, IN LPCVARIANT pvar2);

#ifdef SOFEATURE_CMN_XML
	// XML support
	static void getXMLValue(IN LPCVARIANT pVar, OUT SOCmnStringEx& xmlType, OUT SOCmnStringEx& xmlVal);
	static LPCTSTR getXMLDataType(IN VARTYPE dataType, OUT SOCmnString& xmlType);
	BOOL initFromXMLValue(IN SOCmnXmlValue* pXmlValue);
	static BOOL getVARTYPE(IN LPCTSTR xmlType, OUT VARTYPE& vt);
#endif // SOFEATURE_CMN_XML

	static HRESULT variantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc);
	static HRESULT variantChangeTypeEx(LPVARIANT pvargDest, LPVARIANT pvarSrc, LCID lcid, USHORT wFlags, VARTYPE vt);
	static HRESULT variantClear(LPVARIANT pvarg);

	static LPSAFEARRAY safeArrayCreate(VARTYPE vt, USHORT cDims, LPSAFEARRAYBOUND rgsabound);
	static LPSAFEARRAY safeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements);
}; // SOCmnVariant

inline SOCmnVariant::operator LPVARIANT()
{
	return this;
}

inline SOCmnVariant::operator LPCVARIANT() const
{
	return this;
}

inline void SOCmnVariant::Clear(void)
{
	clear();
}

inline BOOL SOCmnVariant::changeDatatype(IN VARTYPE type)
{
	return changeDatatype(this, type, this);
}

inline BOOL SOCmnVariant::operator==(IN const VARIANT& var) const
{
	return areEqual(this, &var);
}

inline BOOL SOCmnVariant::operator==(IN LPCVARIANT pvar) const
{
	return areEqual(this, pvar);
}

inline BOOL SOCmnVariant::operator==(IN const SOCmnVariant& cmnVar) const
{
	return areEqual(this, &cmnVar);
}


inline BOOL SOCmnVariant::operator!=(IN const VARIANT& var) const
{
	return !areEqual(this, &var);
}

inline BOOL SOCmnVariant::operator!=(IN LPCVARIANT pvar) const
{
	return !areEqual(this, pvar);
}

inline BOOL SOCmnVariant::operator!=(IN const SOCmnVariant& cmnVar) const
{
	return !areEqual(this, &cmnVar);
}

#pragma pack(pop)

#endif // SOFEATURE_CMN_VARIANT

#endif
