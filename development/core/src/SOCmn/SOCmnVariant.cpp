//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOCmn                              |
//                                                                            |
//  Filename    : SOCmnVariant.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : VARIANT object class                                        |
//                - SOCmnVariant: VARIANT object class                        |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOFEATURE_CMN_VARIANT

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <SOCmn.h>
#include <SOCmnVariant.h>
#include <SOCmnXML.h>

#define SOCMNVARIANT_ERROR_BADTYPE  ((HRESULT)0xC0040004L) // = OPC_E_BADTYPE

#ifdef SOOS_WINDOWS
#if _MSC_VER <= 1200 // Microsoft C++ V6.0
#define SOCMNVARIANT_IMP_I64
#endif
#ifdef SOOS_WINDOWS_CE
#ifdef ARM
#define SOCMNVARIANT_IMP_I64
#endif
#if _MSC_VER >= 1400 // Microsoft VS2005
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
#endif // SOOS_WINDOWS

#ifdef SOOS_WINDOWS_CE
int errno;
#ifndef ERANGE
#define ERANGE -1
#endif
#endif


// vs2003 performs run-time checks when casting
// these checks give dialog boxes to the user
// if the value to be casted cannot be represented
// as a cast type value.
//
// To avoid this values feed to cast operation
// are first masked to make sure they are representable
// as a cast type value.
//
# define MASK_BYTE  0xff
# define MASK_CHAR  0xff
# define MASK_SHORT 0xffff
# define MASK_LONG  0xffffffff
# define MASK_LONGLONG 0xffffffffffffffffLL

#define FLOAT_INFINITE_PLUS 0x7F800000
#define FLOAT_INFINITE_MINUS 0xFF800000

#ifdef SOOS_WINDOWS

BOOL g_isOsXpOrLater = TRUE;
BOOL g_getOsVersionEnabled = TRUE;

BOOL isOsXpOrLater(void)
{
	if (g_getOsVersionEnabled)
	{
		OSVERSIONINFO osVersionInfo;
		memset(&osVersionInfo, 0, sizeof(OSVERSIONINFO));
		osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&osVersionInfo);
		g_isOsXpOrLater = ((osVersionInfo.dwMajorVersion > 5) ||
						   ((osVersionInfo.dwMajorVersion == 5) && (osVersionInfo.dwMinorVersion >= 1)));

		if (!g_isOsXpOrLater)
		{
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("Variant"),
					(_T("Is Windows 2000 or earlyer VT_I8 array types are forced to VT_R8")));
		}   //  end if

		g_getOsVersionEnabled = FALSE;
	} // end if

	return g_isOsXpOrLater;
} // end isXpOrLater

#endif  //  SOOS_WINDOWS


//-----------------------------------------------------------------------------
// SOCmnVariant                                                               |
//-----------------------------------------------------------------------------

SOCmnVariant::SOCmnVariant()
{
	::VariantInit(this);
}

SOCmnVariant::SOCmnVariant(
	IN const SOCmnVariant& cmnVar)
{
	::VariantInit(this);
	initFrom((LPVARIANT)&cmnVar);
}

SOCmnVariant::SOCmnVariant(
	IN const VARIANT& var)
{
	::VariantInit(this);
	initFrom((LPVARIANT)&var);
}

SOCmnVariant::SOCmnVariant(
	IN LPCVARIANT pvar)
{
	::VariantInit(this);
	initFrom((LPVARIANT)pvar);
}

void SOCmnVariant::init(
	IN VARTYPE type)
{
	::VariantInit(this);

	if ((type & VT_ARRAY) == VT_ARRAY)
	{
		// array
		vt = type;
		parray = SOCmnVariant::safeArrayCreateVector((VARTYPE)(type & ~VT_ARRAY), 0, 0);
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
#ifdef SOOS_WINDOWS
			date = 0;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			date.tv_sec = 0;
			date.tv_usec = 0;
#endif
			break;

		case VT_BSTR:
		{
			USES_CONVERSION;
			bstrVal = ::SysAllocString(T2OLE(_T("")));
		}
		break;

		case VT_VARIANT:
			::VariantInit(this);
			ulVal = 0;
			break;

		default:
			break;
		}
	}
}

void SOCmnVariant::initArray(
	VARTYPE datatype,
	DWORD arraySize,
	DWORD lBound)
{
	if (datatype & VT_ARRAY)
	{
		// initialize the array values
		SAFEARRAYBOUND dim;
		void* voidArray;
		DWORD i;
		dim.lLbound = lBound;
		dim.cElements = arraySize;
		vt = datatype;
		parray = SOCmnVariant::safeArrayCreate((VARTYPE)(vt & ~VT_ARRAY), 1, &dim);
		SafeArrayAccessData(parray, (void**)&voidArray);

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
				SHORT* pVal = ((SHORT*)voidArray) + i;
				*pVal = (SHORT)0;
			}
			break;

			case VT_UI2:
			{
				WORD* pVal = ((WORD*)voidArray) + i;
				*pVal = (WORD)0;
			}
			break;

			case VT_I4:
			{
				LONG* pVal = ((LONG*)voidArray) + i;
				*pVal = (LONG)0;
			}
			break;

			case VT_UI4:
			{
				DWORD* pVal = ((DWORD*)voidArray) + i;
				*pVal = (DWORD)0;
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
				*pVal = (ULONGLONG)0;
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
#ifdef SOOS_WINDOWS
				*pVal = 0;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
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
				USES_CONVERSION;
				BSTR* pVal = ((BSTR*)voidArray) + i;
				*pVal = ::SysAllocString(T2OLE(_T("")));
			}
			break;

			case VT_VARIANT:
			{
				VARIANT* pVal = ((VARIANT*)voidArray) + i;
				::VariantInit(pVal);
			}
			break;
			}
		}

		SafeArrayUnaccessData(parray);
	}
	else
	{
		init(datatype);
	}
}


SOCmnVariant::SOCmnVariant(
	IN const LPCTSTR sz)
{
	::VariantInit(this);

	if (!sz)
	{
		return;
	}

	USES_CONVERSION;
	vt = VT_BSTR;
	bstrVal = ::SysAllocString(T2OLE((LPTSTR)sz));
}

SOCmnVariant::SOCmnVariant(
	IN char c)
{
	::VariantInit(this);
	vt = VT_I1;
	cVal = c;
}


SOCmnVariant::SOCmnVariant(
	IN BYTE b)
{
	::VariantInit(this);
	vt = VT_UI1;
	bVal = b;
}

SOCmnVariant::SOCmnVariant(
	IN short s)
{
	::VariantInit(this);
	vt = VT_I2;
	iVal = s;
}

SOCmnVariant::SOCmnVariant(
	IN unsigned short us)
{
	::VariantInit(this);
	vt = VT_UI2;
	uiVal = us;
}

SOCmnVariant::SOCmnVariant(
	IN long l)
{
	::VariantInit(this);
	vt = VT_I4;
	lVal = l;
}

SOCmnVariant::SOCmnVariant(
	IN unsigned long ul)
{
	::VariantInit(this);
	vt = VT_UI4;
	ulVal = ul;
}

SOCmnVariant::SOCmnVariant(
	IN int i)
{
	::VariantInit(this);
	vt = VT_INT;
	intVal = i;
}

SOCmnVariant::SOCmnVariant(
	IN unsigned int ui)
{
	::VariantInit(this);
	vt = VT_UINT;
	uintVal = ui;
}

SOCmnVariant::SOCmnVariant(
	IN float flt)
{
	::VariantInit(this);
	vt = VT_R4;
	fltVal = flt;
}

SOCmnVariant::SOCmnVariant(
	IN double dbl)
{
	::VariantInit(this);
	vt = VT_R8;
	dblVal = dbl;
}

SOCmnVariant::SOCmnVariant(
	IN bool bo)
{
	::VariantInit(this);
	vt = VT_BOOL;
	boolVal = (bo) ? VARIANT_TRUE : VARIANT_FALSE;
}

SOCmnVariant::SOCmnVariant(
	IN LONGLONG i8)
{
	::VariantInit(this);
	vt = VT_I8;
	V_I8(this) = i8;
}

SOCmnVariant::SOCmnVariant(
	IN ULONGLONG ui8)
{
	::VariantInit(this);
	vt = VT_UI8;
	V_UI8(this) = ui8;
}

SOCmnVariant::SOCmnVariant(
	IN CURRENCY cy)
{
	::VariantInit(this);
	vt = VT_CY;
	V_CY(this).int64 = cy.int64;
}

SOCmnVariant::~SOCmnVariant()
{
	clear();
}

const SOCmnVariant& SOCmnVariant::operator=(
	IN SOCmnVariant& cmnVar)
{
	initFrom((LPVARIANT)&cmnVar);
	return *this;
}

const SOCmnVariant& SOCmnVariant::operator=(
	IN const VARIANT& var)
{
	initFrom((LPVARIANT)&var);
	return *this;
}

const SOCmnVariant& SOCmnVariant::operator=(
	IN LPCVARIANT pvar)
{
	initFrom((LPVARIANT)pvar);
	return *this;
}

void SOCmnVariant::setBSTR(IN const LPCTSTR sz)
{
	if (sz != NULL)
	{
		USES_CONVERSION;
		clear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(T2OLE((LPTSTR)sz));
	}
}

LPCTSTR SOCmnVariant::getBSTR(SOCmnString& buffer)
{
	USES_CONVERSION;
	buffer = OLE2T(bstrVal);
	return (LPCTSTR)buffer;
}


void SOCmnVariant::setI1(IN char c)
{
	clear();
	vt = VT_I1;
	cVal = c;
}

void SOCmnVariant::setUI1(IN BYTE b)
{
	clear();
	vt = VT_UI1;
	bVal = b;
}

void SOCmnVariant::setI2(IN short s)
{
	clear();
	vt = VT_I2;
	iVal = s;
}

void SOCmnVariant::setUI2(IN unsigned short us)
{
	clear();
	vt = VT_UI2;
	iVal = us;
}

void SOCmnVariant::setI4(IN long l)
{
	clear();
	vt = VT_I4;
	lVal = l;
}

void SOCmnVariant::setUI4(IN unsigned long ul)
{
	clear();
	vt = VT_UI4;
	ulVal = ul;
}

void SOCmnVariant::setINT(IN int i)
{
	clear();
	vt = VT_INT;
	intVal = i;
}

void SOCmnVariant::setUINT(IN unsigned int ui)
{
	clear();
	vt = VT_UINT;
	uintVal = ui;
}

void SOCmnVariant::setR4(IN float flt)
{
	clear();
	vt = VT_R4;
	fltVal = flt;
}

void SOCmnVariant::setR8(IN double dbl)
{
	clear();
	vt = VT_R8;
	dblVal = dbl;
}

void SOCmnVariant::setBOOL(IN bool bo)
{
	clear();
	vt = VT_BOOL;
	boolVal = (bo) ? VARIANT_TRUE : VARIANT_FALSE;
}

void SOCmnVariant::setCY(IN LONGLONG cy)
{
	clear();
	vt = VT_CY;
	V_CY(this).int64 = cy;
}

void SOCmnVariant::setI8(IN LONGLONG i8)
{
	clear();
	vt = VT_I8;
	V_I8(this) = i8;
}

void SOCmnVariant::setUI8(IN ULONGLONG ui8)
{
	clear();
	vt = VT_UI8;
	V_UI8(this) = ui8;
}

void SOCmnVariant::setDATE(IN DATE dt)
{
	clear();
	vt = VT_DATE;
	date = dt;
}

#ifdef SOOS_LINUX
// under windows DATE is a double, which creates a overload conflict!
SOCmnVariant::SOCmnVariant(IN DATE dt)
{
	::VariantInit(this);
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
BOOL SOCmnVariant::supportDatatype(
	IN VARTYPE dataType)    // data type
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
		return TRUE;

	default:
		return FALSE;
	}
} // supportDatatype


SOCmnVariant& SOCmnVariant::set(IN void* src, IN VARTYPE type, IN WORD elSize)
{
	if (((type & VT_ARRAY) == 0) && (type != VT_BSTR))
	{
		// no array and no string
		SOCmnVariant::variantClear(this);
		memcpy((void*)&bVal, src, elSize);
		vt = type;
	}

	return *this;
}

DWORD SOCmnVariant::getSize(void)
{
	if ((vt & VT_ARRAY) == 0)
	{
		return getSize(vt);
	}
	else
	{
		LONG lBound, uBound;
		::SafeArrayGetLBound(parray, 1, &lBound);
		::SafeArrayGetUBound(parray, 1, &uBound);
		return (getSize((VARTYPE)(vt & ~VT_ARRAY)) * (uBound - lBound + 1));
	}
}

DWORD SOCmnVariant::getSize(VARTYPE varType)
{
	DWORD size = 0;

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

void SOCmnVariant::get(IN void* dest)
{
	if ((vt & VT_ARRAY) == 0)
	{
		// no array and no string
		memcpy(dest, (void*)&bVal, getSize());
	}
	else
	{
		void* pData = NULL;
		SafeArrayAccessData(parray, &pData);
		memcpy(dest, pData, getSize());
		SafeArrayUnaccessData(parray);
	}
}


//-----------------------------------------------------------------------------
// clear
//
// - clear the variant
//
void SOCmnVariant::clear(void)
{
	SOCmnVariant::variantClear(this);
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
BOOL SOCmnVariant::areEqual(
	IN LPCVARIANT pvar1,    // first variant to compare
	IN LPCVARIANT pvar2)    // second variant to compare
{
	BOOL equal = FALSE;

	if ((pvar1 == NULL) || (pvar2 == NULL))
	{
		return (pvar1 == pvar2);
	}

	if (pvar1 == pvar2)
	{
		return TRUE;
	}

	if (pvar1->vt != pvar2->vt)
	{
		return FALSE;    // no type conversion for compare till now!
	}

	switch (pvar1->vt)
	{
	case VT_EMPTY:
		return TRUE;
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
		equal = ((::SysStringByteLen(pvar1->bstrVal) == SysStringByteLen(pvar2->bstrVal)) &&
				 (::memcmp(pvar1->bstrVal, pvar2->bstrVal, SysStringByteLen(pvar1->bstrVal)) == 0));
		break;

		// NOTE: compare float and double by bits, since the FP returns == for comparison
		//   between a valid real value and NAN/-INF/+INF
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
BOOL SOCmnVariant::areEqualArrays(
	IN LPCVARIANT pvar1,    // first variant array to compare
	IN LPCVARIANT pvar2)    // second variant array to compare
{
	if ((pvar1 == NULL) || (pvar2 == NULL))
	{
		return FALSE;
	}

	if ((pvar1->vt & VT_ARRAY) != VT_ARRAY)
	{
		return FALSE;
	}

	// check array datatypes
	VARTYPE vt1, vt2;
	vt1 = (VARTYPE)(pvar1->vt & ~VT_ARRAY);
	vt2 = (VARTYPE)(pvar2->vt & ~VT_ARRAY);

	if (vt1 != vt2)
	{
		return FALSE;
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
	DWORD dim1, dim2;
	dim1 = ::SafeArrayGetDim(parray1);
	dim2 = ::SafeArrayGetDim(parray2);

	if (dim1 != dim2)
	{
		return FALSE;
	}

	if (dim1 != 1)
	{
		return FALSE;    // OPC only supports array with one dimension
	}

	// check element size
	DWORD elSize1, elSize2;
	elSize1 = ::SafeArrayGetElemsize(parray1);
	elSize2 = ::SafeArrayGetElemsize(parray2);

	if (dim1 != dim2)
	{
		return FALSE;
	}

	// check bounds
	LONG lBound1, lBound2, uBound1, uBound2;
	::SafeArrayGetLBound(parray1, 1, &lBound1);
	::SafeArrayGetLBound(parray2, 1, &lBound2);
	::SafeArrayGetUBound(parray1, 1, &uBound1);
	::SafeArrayGetUBound(parray2, 1, &uBound2);

	if ((lBound1 != lBound2) || (uBound1 != uBound2))
	{
		return FALSE;
	}

	// check data
	void* data1, *data2;
	BYTE equal = FALSE;
	::SafeArrayAccessData(parray1, &data1);
	::SafeArrayAccessData(parray2, &data2);

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

		BSTR* pv1;
		BSTR* pv2;
		LONG i;
		pv1 = (BSTR*)data1;
		pv2 = (BSTR*)data2;
		equal = TRUE;

		for (i = 0; i <= uBound1 - lBound1; i++)
		{

			VARIANT v1temp, v2temp; 
			v1temp.vt = v2temp.vt = VT_BSTR;
			v1temp.bstrVal = pv1[i];
			v2temp.bstrVal = pv2[i];
			if (!((::SysStringByteLen(v1temp.bstrVal) == SysStringByteLen(v2temp.bstrVal)) &&
				(::memcmp(v1temp.bstrVal, v2temp.bstrVal, SysStringByteLen(v1temp.bstrVal)) == 0)))
			{
				equal = FALSE;
				break;
			}		
		}
	}
	break;
	case VT_VARIANT:
	{
		LPVARIANT pv1, pv2;
		LONG i;
		pv1 = (LPVARIANT)data1;
		pv2 = (LPVARIANT)data2;
		equal = TRUE;

		for (i = 0; i <= uBound1 - lBound1; i++)
		{
			if (!areEqual(pv1, pv2))
			{
				equal = FALSE;
				break;
			}

			pv1++;
			pv2++;
		}
	}
	break;
	}

	::SafeArrayUnaccessData(parray1);
	::SafeArrayUnaccessData(parray2);
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
BOOL SOCmnVariant::changeDatatype(
	IN VARIANT* value,          // source value
	IN VARTYPE type,            // wanted datatype
	OUT VARIANT* changedValue,// destination value
	OPTIONAL OUT HRESULT* pError)   // conversion error
{
	return changeDatatype(value, type, LOCALE_SYSTEM_DEFAULT, changedValue, pError);
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
BOOL SOCmnVariant::changeDatatype(
	IN VARIANT* value,          // source value
	IN VARTYPE type,            // wanted datatype
	IN LCID localeID,           // locale id
	OUT VARIANT* changedValue,  // destination value
	OPTIONAL OUT HRESULT* pError)   // conversion error
{
	if ((!value) || (!changedValue))
	{
		if (pError)
		{
			*pError = SOCMNVARIANT_ERROR_BADTYPE;
		}

		return FALSE; // bad input parameter
	}

	if ((value->vt & VT_ARRAY) != (type & VT_ARRAY))
	{
		if (pError)
		{
			*pError = SOCMNVARIANT_ERROR_BADTYPE;
		}

		return FALSE; // can not convert simple type to array vice versa
	}

	BOOL ret = TRUE;

	if ((type & VT_ARRAY) == 0)
	{
		// no array
		DWORD bothTypes = MAKELONG(value->vt, type);    // from - low, to - high
		SOCmnVariant to;

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

			SOCmnVariant::variantClear(changedValue);
			to.copyTo(changedValue);
			break;

		case MAKELONG(VT_I1, VT_UI1):
			if ((value->cVal & 0x80) == 0)
			{
				to = (BYTE)value->cVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_UI1, VT_I1):
			if ((value->bVal & 0x80) == 0)
			{
				to = (char)value->bVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_I2, VT_UI2):
			if ((value->iVal & 0x8000) == 0)
			{
				to = (WORD)value->iVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_UI2, VT_I2):
			if ((value->uiVal & 0x8000) == 0)
			{
				to = (SHORT)value->uiVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_I4, VT_UI4):
		case MAKELONG(VT_I4, VT_UINT):
		case MAKELONG(VT_INT, VT_UI4):
		case MAKELONG(VT_INT, VT_UINT):
			if ((value->lVal & 0x80000000) == 0)
			{
				to = (DWORD)value->lVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_UI4, VT_I4):
		case MAKELONG(VT_UI4, VT_INT):
		case MAKELONG(VT_UINT, VT_I4):
		case MAKELONG(VT_UINT, VT_INT):
			if ((value->ulVal & 0x80000000) == 0)
			{
				to = (LONG)value->ulVal;
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				if (pError)
				{
					*pError = DISP_E_OVERFLOW;
				}

				ret = FALSE;
			}

			break;

		case MAKELONG(VT_CY, VT_UI4):
		case MAKELONG(VT_CY, VT_I4):
		case MAKELONG(VT_CY, VT_UINT):
		case MAKELONG(VT_CY, VT_INT):
		{
			HRESULT vctx;
			ret = ((vctx = SOCmnVariant::variantChangeTypeEx(changedValue, value, localeID, 0, type)) == S_OK);

			if ((!ret) && (pError))
			{
				*pError = vctx;
			}
		}
		break;
#ifdef SOFEATURE_CMN_TIME

		case MAKELONG(VT_BSTR, VT_DATE):
		{
			USES_CONVERSION;
			SOCmnDateTime dateT;
			LPTSTR dateSZ = OLE2T(value->bstrVal);

			if (dateT.setXMLDateTime(dateSZ))
			{
				to.vt = VT_DATE;
				dateT.get(&to.date);
				SOCmnVariant::variantClear(changedValue);
				to.copyTo(changedValue);
			}
			else
			{
				HRESULT vctx;
				ret = ((vctx = SOCmnVariant::variantChangeTypeEx(changedValue, value, localeID, 0, type)) == S_OK);

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
			HRESULT vctx;
			ret = ((vctx = SOCmnVariant::variantChangeTypeEx(changedValue, value, localeID, 0, type)) == S_OK);

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
		SOCmnVariant to;
		SAFEARRAYBOUND dim;
		void* dataS, *dataD;
		DWORD i;
		LONG lBound, uBound;
		SOCmnVariant elS, elD;
		VARTYPE elTypeS = (VARTYPE)(value->vt & ~VT_ARRAY);
		VARTYPE elTypeD = (VARTYPE)(type & ~VT_ARRAY);
		WORD elSizeS, elSizeD;
		USES_CONVERSION;
		::SafeArrayGetLBound(value->parray, 1, &lBound);
		::SafeArrayGetUBound(value->parray, 1, &uBound);
		dim.lLbound = lBound;
		dim.cElements = uBound - lBound + 1;
		to.vt = type;
		to.parray = SOCmnVariant::safeArrayCreate((VARTYPE)(type & ~VT_ARRAY), 1, &dim);
		elSizeS = (WORD)::SafeArrayGetElemsize(value->parray);
		elSizeD = (WORD)::SafeArrayGetElemsize(to.parray);
		::SafeArrayAccessData(value->parray, &dataS);
		::SafeArrayAccessData(to.parray, &dataD);
		memset(dataD, 0, dim.cElements * elSizeD);

		for (i = 0; i < dim.cElements; i++)
		{
			if (elTypeS != VT_VARIANT)
			{
				if (elTypeS != VT_BSTR)
				{
					elS.set(((BYTE*)dataS) + (elSizeS * i), (VARTYPE)(value->vt & ~VT_ARRAY), elSizeS);
				}
				else
				{
					elS = OLE2T(*(BSTR*)(((BYTE*)dataS) + (elSizeS * i)));
				}
			}
			else
			{
				elS = (LPCVARIANT)((BYTE*)dataS + (elSizeS * i));
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
					memcpy(((BYTE*)dataD) + (elSizeD * i), (const void*)&elD.bVal, elSizeD);
				}
				else
				{
					*(BSTR*)(((BYTE*)dataD) + (elSizeD * i)) = ::SysAllocString(elD.bstrVal);
				}
			}
			else
			{
				HRESULT vc;
				ret = ((vc = SOCmnVariant::variantCopy((LPVARIANT)((BYTE*)dataD + (elSizeD * i)), elS)) == S_OK);

				if (!ret)
				{
					*pError = vc;
					break;
				}
			}
		}

		::SafeArrayUnaccessData(value->parray);
		::SafeArrayUnaccessData(to.parray);

		if (ret)
		{
			SOCmnVariant::variantClear(changedValue);
			to.copyTo(changedValue);
		}
	}

	return ret;
} // changeDatatype


HRESULT SOCmnVariant::variantClear(LPVARIANT pvargDest)
{
#ifdef SOOS_WINDOWS

	if ((pvargDest->vt == VT_UI8) || (pvargDest->vt == VT_I8) ||
		(pvargDest->vt == (VT_UI8 | VT_ARRAY)) || (pvargDest->vt == (VT_I8 | VT_ARRAY)))
	{
		if (pvargDest->vt & VT_ARRAY)
		{
			::SafeArrayDestroy(pvargDest->parray);
		}

		::VariantInit(pvargDest);
		return S_OK;
	}

	return ::VariantClear(pvargDest);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::VariantClear(pvargDest);
#endif
}

HRESULT SOCmnVariant::variantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc)
{
#ifdef SOOS_WINDOWS

	if (pvargSrc != NULL && (((pvargSrc->vt & ~VT_ARRAY) == VT_UI8) || ((pvargSrc->vt & ~VT_ARRAY) == VT_I8)))
	{
		if (pvargDest == NULL || pvargSrc == NULL)
		{
			return E_INVALIDARG;
		}

		if (pvargDest == pvargSrc)
		{
			return S_OK;
		}

		HRESULT retval = SOCmnVariant::variantClear(pvargDest);

		if (FAILED(retval))
		{
			return retval;
		}

		*pvargDest = *pvargSrc;

		if (pvargSrc->vt & VT_ARRAY)
		{
			retval = ::SafeArrayCopy(pvargSrc->parray, &pvargDest->parray);
		}

		if (FAILED(retval))
		{
			pvargDest->vt = VT_EMPTY;
		}

		return retval;
	}

	if (pvargDest != NULL && ((pvargDest->vt & ~VT_ARRAY) == VT_UI8 || (pvargDest->vt & ~VT_ARRAY) == VT_I8))
	{
		HRESULT retval = SOCmnVariant::variantClear(pvargDest);

		if (FAILED(retval))
		{
			return retval;
		}
	}

	return ::VariantCopy(pvargDest, pvargSrc);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::VariantCopy(pvargDest, pvargSrc);
#endif
}

HRESULT SOCmnVariant::variantChangeTypeEx(LPVARIANT pvargDest, LPVARIANT pvargSrc, LCID lcid, USHORT wFlags, VARTYPE vt)
{
#ifdef SOOS_WINDOWS

	if ((vt == VT_UI8) || (vt == VT_I8) || (pvargSrc->vt == VT_UI8) || (pvargSrc->vt == VT_I8))
	{
		WCHAR tmpval[64];
		BSTR  tmpstr = NULL;

		if (pvargDest == NULL || pvargSrc == NULL)
		{
			return E_INVALIDARG;
		}

		if (pvargDest != pvargSrc)
		{
			HRESULT hr = SOCmnVariant::variantClear(pvargDest);

			if (FAILED(hr))
			{
				return hr;
			}
		}

		if (vt == pvargSrc->vt)
		{
			return SOCmnVariant::variantCopy(pvargDest, pvargSrc);
		}
		else switch (MAKELONG(pvargSrc->vt, vt))      // from - low, to - high
			{
				// Numeric to I8
			case MAKELONG(VT_UI1, VT_I8):
				V_I8(pvargDest) = pvargSrc->bVal;
				break;

			case MAKELONG(VT_I1, VT_I8):
				V_I8(pvargDest) = pvargSrc->cVal;
				break;

			case MAKELONG(VT_I2, VT_I8):
				V_I8(pvargDest) = pvargSrc->iVal;
				break;

			case MAKELONG(VT_UI2, VT_I8):
				V_I8(pvargDest) = pvargSrc->uiVal;
				break;

			case MAKELONG(VT_I4, VT_I8):
			case MAKELONG(VT_INT, VT_I8):
				V_I8(pvargDest) = pvargSrc->lVal;
				break;

			case MAKELONG(VT_UI4, VT_I8):
			case MAKELONG(VT_UINT, VT_I8):
				V_I8(pvargDest) = pvargSrc->ulVal;
				break;

			case MAKELONG(VT_CY, VT_I8):
				V_I8(pvargDest) = pvargSrc->cyVal.int64 / 10000;
				break;

			case MAKELONG(VT_R4, VT_I8):
				if (pvargSrc->fltVal > LLONG_MAX || pvargSrc->fltVal < LLONG_MIN)
				{
					return DISP_E_OVERFLOW;
				}

				V_I8(pvargDest) = (LONGLONG)(pvargSrc->fltVal);
				break;

			case MAKELONG(VT_R8, VT_I8):
			{
				if (pvargSrc->dblVal > LLONG_MAX || (-pvargSrc->dblVal) > LLONG_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				V_I8(pvargDest) = (LONGLONG)(pvargSrc->dblVal);
			}
			break;

			// Numeric to UI8
			case MAKELONG(VT_UI1, VT_UI8):
				V_UI8(pvargDest) = pvargSrc->bVal;
				break;

			case MAKELONG(VT_I1, VT_UI8):
				if (pvargSrc->cVal < 0)
				{
					return DISP_E_OVERFLOW;
				}

				V_UI8(pvargDest) = pvargSrc->cVal;
				break;

			case MAKELONG(VT_I2, VT_UI8):
				if (pvargSrc->iVal < 0)
				{
					return DISP_E_OVERFLOW;
				}

				V_UI8(pvargDest) = pvargSrc->iVal;
				break;

			case MAKELONG(VT_UI2, VT_UI8):
				V_UI8(pvargDest) = pvargSrc->uiVal;
				break;

			case MAKELONG(VT_I4, VT_UI8):
			case MAKELONG(VT_INT, VT_UI8):
				if (pvargSrc->lVal < 0)
				{
					return DISP_E_OVERFLOW;
				}

				V_UI8(pvargDest) = pvargSrc->lVal;
				break;

			case MAKELONG(VT_UI4, VT_UI8):
			case MAKELONG(VT_UINT, VT_UI8):
				V_UI8(pvargDest) = pvargSrc->ulVal;
				break;

			case MAKELONG(VT_CY, VT_UI8):
				V_UI8(pvargDest) = pvargSrc->cyVal.int64 / 10000;
				break;

			case MAKELONG(VT_R4, VT_UI8):
				if (pvargSrc->fltVal > ULLONG_MAX || pvargSrc->fltVal < 0)
				{
					return DISP_E_OVERFLOW;
				}

				V_UI8(pvargDest) = (ULONGLONG)(pvargSrc->fltVal);
				break;

			case MAKELONG(VT_R8, VT_UI8):
				if (pvargSrc->dblVal > ULLONG_MAX || pvargSrc->dblVal < 0)
				{
					return DISP_E_OVERFLOW;
				}

				V_UI8(pvargDest) = (ULONGLONG)(pvargSrc->dblVal);
				break;

				// signed <-> unsigned conversions
			case MAKELONG(VT_I8, VT_UI8):
				if (V_I8(pvargSrc) < 0)
				{
					return DISP_E_OVERFLOW;
				}

				VariantCopy(pvargDest, pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_I8):
				if (V_UI8(pvargSrc) > LLONG_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				VariantCopy(pvargDest, pvargSrc);
				break;

				// Numeric to BOOL
			case MAKELONG(VT_UI8, VT_BOOL):
			case MAKELONG(VT_I8, VT_BOOL):
				pvargDest->boolVal = V_I8(pvargSrc) ? VARIANT_TRUE : VARIANT_FALSE;
				break;

				// BOOL to Numeric
			case MAKELONG(VT_BOOL, VT_UI8):
			case MAKELONG(VT_BOOL, VT_I8):
				V_I8(pvargDest) = (LONGLONG)pvargSrc->boolVal;
				break;

				// Numeric to UI1
			case MAKELONG(VT_I8, VT_UI1):
			case MAKELONG(VT_UI8, VT_UI1):
				if (V_UI8(pvargSrc) > UCHAR_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->bVal = (BYTE)V_UI8(pvargSrc);
				break;

				// Numeric to I1
			case MAKELONG(VT_I8, VT_I1):
				if (V_I8(pvargSrc) > CHAR_MAX || V_I8(pvargSrc) < CHAR_MIN)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->cVal = (CHAR)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_I1):
				if (V_UI8(pvargSrc) > CHAR_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->cVal = (CHAR)V_UI8(pvargSrc);
				break;

				// Numeric to I2
			case MAKELONG(VT_I8, VT_I2):
				if (V_I8(pvargSrc) > SHRT_MAX || V_I8(pvargSrc) < SHRT_MIN)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->iVal = (SHORT)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_I2):
				if (V_UI8(pvargSrc) > SHRT_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->iVal = (SHORT)V_UI8(pvargSrc);
				break;

				// Numeric to UI2
			case MAKELONG(VT_I8, VT_UI2):
				if (V_I8(pvargSrc) > USHRT_MAX || V_I8(pvargSrc) < 0)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->uiVal = (USHORT)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_UI2):
				if (V_UI8(pvargSrc) > USHRT_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->uiVal = (USHORT)V_UI8(pvargSrc);
				break;

				// Numeric to I4
			case MAKELONG(VT_I8, VT_I4):
			case MAKELONG(VT_I8, VT_INT):
				if (V_I8(pvargSrc) > LONG_MAX || V_I8(pvargSrc) < LONG_MIN)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->lVal = (LONG)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_I4):
			case MAKELONG(VT_UI8, VT_INT):
				if (V_UI8(pvargSrc) > LONG_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->lVal = (LONG)V_UI8(pvargSrc);
				break;

				// Numeric to UI4
			case MAKELONG(VT_I8, VT_UI4):
			case MAKELONG(VT_I8, VT_UINT):
				if (V_I8(pvargSrc) > ULONG_MAX || V_I8(pvargSrc) < 0)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->ulVal = (ULONG)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_UI4):
			case MAKELONG(VT_UI8, VT_UINT):
				if (V_UI8(pvargSrc) > ULONG_MAX)
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->ulVal = (ULONG)V_UI8(pvargSrc);
				break;

				// Numeric to CY
			case MAKELONG(VT_I8, VT_CY):
				if ((V_I8(pvargSrc) > (LLONG_MAX / 10000)) || (V_I8(pvargSrc) < (LLONG_MIN / 10000)))
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->cyVal.int64 = 10000 * V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_CY):
				if (V_UI8(pvargSrc) > (ULLONG_MAX / 10000))
				{
					return DISP_E_OVERFLOW;
				}

				pvargDest->cyVal.int64 = 10000 * V_UI8(pvargSrc);
				break;

				// Numeric to R4
			case MAKELONG(VT_I8, VT_R4):
				pvargDest->fltVal = (FLOAT)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_R4):
#if _MSC_VER >= 1300 // Microsoft Visual Studio .NET
				pvargDest->fltVal = (FLOAT)V_UI8(pvargSrc);
#else
				pvargDest->fltVal = (FLOAT)(LONGLONG)V_I8(pvargSrc);
#endif
				break;

				// Numeric to R8
			case MAKELONG(VT_I8, VT_R8):
				pvargDest->dblVal = (DOUBLE)V_I8(pvargSrc);
				break;

			case MAKELONG(VT_UI8, VT_R8):
#if _MSC_VER >= 1300 // Microsoft Visual Studio .NET
				pvargDest->dblVal = (DOUBLE)V_UI8(pvargSrc);
#else
				pvargDest->dblVal = (DOUBLE)(LONGLONG)V_I8(pvargSrc);
#endif
				break;

				// Numeric to String
			case MAKELONG(VT_I8, VT_BSTR):
				_snwprintf(tmpval, sizeof(tmpval), L"%I64i", V_I8(pvargSrc));
				pvargDest->bstrVal = SysAllocString(tmpval);
				break;

			case MAKELONG(VT_UI8, VT_BSTR):
				_snwprintf(tmpval, sizeof(tmpval), L"%I64u", V_UI8(pvargSrc));
				pvargDest->bstrVal = SysAllocString(tmpval);
				break;

				// String to Numeric
			case MAKELONG(VT_BSTR, VT_I8):
				tmpstr = pvargSrc->bstrVal;

				if (tmpstr == NULL)
				{
					return E_INVALIDARG;
				}

				V_I8(pvargDest) = _wcstoi64(tmpstr, NULL, 0);
				break;

			case MAKELONG(VT_BSTR, VT_UI8):
				tmpstr = pvargSrc->bstrVal;

				if (tmpstr == NULL)
				{
					return E_INVALIDARG;
				}

				V_UI8(pvargDest) = _wcstoui64(tmpstr, NULL, 0);
				break;

			default:
				return DISP_E_TYPEMISMATCH;
			}

		pvargDest->vt = vt;

		if (tmpstr != NULL && pvargSrc == pvargDest)
		{
			SysFreeString(tmpstr);
		}

		if (vt == VT_BSTR && pvargDest->bstrVal == NULL)
		{
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}
	else
	{
		return ::VariantChangeTypeEx(pvargDest, pvargSrc, lcid, wFlags, vt);
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::VariantChangeTypeEx(pvargDest, pvargSrc, lcid, wFlags, vt);
#endif
}

LPSAFEARRAY SOCmnVariant::safeArrayCreate(VARTYPE vt, USHORT cDims, LPSAFEARRAYBOUND rgsabound)
{
#ifdef SOOS_WINDOWS

	if ((vt == VT_UI8) || (vt == VT_I8))
	{
#ifdef SOOS_WINDOWS_CE
		vt = VT_R8;
#else

		if (!isOsXpOrLater())
		{
			vt = VT_R8;
		}

#endif // SOOS_WINDOWS_CE
	}

	return ::SafeArrayCreate(vt, cDims, rgsabound);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	return ::SafeArrayCreate(vt, cDims, rgsabound);
#endif
}

LPSAFEARRAY SOCmnVariant::safeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements)
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = lLbound;
	rgsabound[0].cElements = cElements;
	return SOCmnVariant::safeArrayCreate(vt, 1, rgsabound);
}

BOOL SOCmnVariant::copyTo(OUT LPVARIANT destination)
{
	::VariantInit(destination);
	return (SOCmnVariant::variantCopy(destination, (LPVARIANT)this) == S_OK);
}

BOOL SOCmnVariant::initFrom(IN LPCVARIANT source)
{
	SOCmnVariant::variantClear(this);
	return (SOCmnVariant::variantCopy((LPVARIANT)this, (LPVARIANT)source) == S_OK);
}


LPCTSTR SOCmnVariant::toString(
	IN SOCmnString* string)
const
{
	return ::variant2string(string, (LPCVARIANT)this);
}


#ifdef SOFEATURE_CMN_XML

//-----------------------------------------------------------------------------
// getXMLValue
//
// - convert the variant to a XML value and datatype string
//
void SOCmnVariant::getXMLValue(
	IN LPCVARIANT pVar,       // VARIANT value
	OUT SOCmnStringEx& xmlType, // XML datatype
	OUT SOCmnStringEx& xmlVal)  // XML value
{
	USES_CONVERSION;

	switch (pVar->vt)
	{
	case VT_BOOL:
		xmlVal.format(_T("%s"), BOOL2XMLSTR(pVar->boolVal != VARIANT_FALSE));
		xmlType = _T("xsd:boolean");
		break;

	case VT_I1:
		xmlVal.format(_T("%i"), pVar->cVal);
		xmlType = _T("xsd:byte");
		break;

	case VT_UI1:
		xmlVal.format(_T("%u"), pVar->bVal);
		xmlType = _T("xsd:unsignedByte");
		break;

	case VT_I2:
		xmlVal.format(_T("%i"), pVar->iVal);
		xmlType = _T("xsd:short");
		break;

	case VT_UI2:
		xmlVal.format(_T("%u"), pVar->uiVal);
		xmlType = _T("xsd:unsignedShort");
		break;

	case VT_I4:
	case VT_INT:
		xmlVal.format(_T("%li"), pVar->lVal);
		xmlType = _T("xsd:int");
		break;

	case VT_UI4:
	case VT_UINT:
		xmlVal.format(_T("%lu"), pVar->ulVal);
		xmlType = _T("xsd:unsignedInt");
		break;

	case VT_I8:
#ifdef SOOS_LINUX
		xmlVal.format(_T("%lld"), V_I8(pVar));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		xmlVal.format(_T("%I64d"), V_I8(pVar));
#endif
		xmlType = _T("xsd:long");
		break;

	case VT_UI8:
#ifdef SOOS_LINUX
		xmlVal.format(_T("%llu"), V_UI8(pVar));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
		xmlVal.format(_T("%I64u"), V_UI8(pVar));
#endif
		xmlType = _T("xsd:unsignedLong");
		break;

	case VT_R4:
		xmlVal.format(_T("%g"), pVar->fltVal);
		xmlType = _T("xsd:float");
		break;

	case VT_R8:
		xmlVal.format(_T("%.15g"), pVar->dblVal);
		xmlType = _T("xsd:double");
		break;

	case VT_CY:
	{
		SHORT decimalDigits = (SHORT)(pVar->cyVal.int64 % 10000);

		if (decimalDigits < 0)
		{
			decimalDigits = -decimalDigits;
		}

#ifdef SOOS_LINUX

		if (decimalDigits != 0)
		{
			xmlVal.format(_T("%lld.%4.4u"), (pVar->cyVal.int64 / 10000) , decimalDigits);
		}
		else
		{
			xmlVal.format(_T("%lld"), (pVar->cyVal.int64 / 10000));
		}

#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS

		if (decimalDigits != 0)
		{
			xmlVal.format(_T("%I64d.%4.4u"), (pVar->cyVal.int64 / 10000) , decimalDigits);
		}
		else
		{
			xmlVal.format(_T("%I64d"), (pVar->cyVal.int64 / 10000));
		}

#endif
		xmlType = _T("xsd:decimal");
	}
	break;

	case VT_DATE:
	{
		// 1999-05-31T13:20:00-05:00
		SOCmnDateTime date(&pVar->date);
		xmlVal  = date.getXMLDateTime();
		xmlType = _T("xsd:dateTime");
	}
	break;

	case VT_BSTR:
		SOCmnStringEx::getXMLString(pVar->bstrVal, xmlVal);
		xmlType = _T("xsd:string");
		break;

	case VT_ARRAY | VT_UI1:
	{
		SOCmnVariant val(pVar);
		xmlVal  = SOCmnXmlValue::encodeBytes2base64(val);
		xmlType = _T("xsd:base64Binary");
	}
	break;

	default:
		if (pVar->vt & VT_ARRAY)
		{
			void*   pData;
			LONG    lBound, uBound;

			if (FAILED(SafeArrayAccessData(V_ARRAY(pVar), &pData))
				||  FAILED(SafeArrayGetLBound(V_ARRAY(pVar), 1, &lBound))
				||  FAILED(SafeArrayGetUBound(V_ARRAY(pVar), 1, &uBound)))
			{
				return;
			}

			LONG lElements = uBound - lBound + 1;
			SOCmnStringEx xmlHelper;
			xmlHelper.getBuffer(48);
			SOCmnStringEx xmlArrayString;
			xmlArrayString.getBuffer(48 * lElements);
			LONG i;

			// create array string of booleans
			if ((pVar->vt & ~ VT_ARRAY) == VT_BOOL)
			{
				VARIANT_BOOL* pBool = (VARIANT_BOOL*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<boolean>%s</boolean>"), BOOL2XMLSTR(pBool[i] != VARIANT_FALSE));
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfBoolean;
			}
			// create array string of signed char (8Bit)
			else if ((pVar->vt & ~ VT_ARRAY) == VT_I1)
			{
				xmlArrayString.getBuffer(lElements * 17);
				char* pChar = (char*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<byte>%d</byte>"), pChar[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfByte;
			}
			// create array string of signed short (16Bit)
			else if ((pVar->vt & ~VT_ARRAY) == VT_I2)
			{
				xmlArrayString.getBuffer(lElements * 21);
				short* pShort = (short*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<short>%d</short>"), pShort[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfShort;
			}
			// create array string of unsigned short (16Bit)
			else if ((pVar->vt & ~VT_ARRAY) == VT_UI2)
			{
				xmlArrayString.getBuffer(lElements * 38);
				unsigned short* pUshort = (unsigned short*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<unsignedShort>%u</unsignedShort>"), pUshort[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfUnsignedShort;
			}
			// create array string of signed integer (32Bit)
			else if (((pVar->vt & ~VT_ARRAY) == VT_I4) || ((pVar->vt & ~VT_ARRAY) == VT_INT))
			{
				xmlArrayString.getBuffer(lElements * 22);
				long* pInt = (long*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<int>%ld</int>"), pInt[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfInt;
			}
			// create array string of unsigned integer (32Bit)
			else if (((pVar->vt & ~VT_ARRAY) == VT_UI4) || ((pVar->vt & ~VT_ARRAY) == VT_UINT))
			{
				xmlArrayString.getBuffer(lElements * 37);
				unsigned long* pUint = (unsigned long*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<unsignedInt>%lu</unsignedInt>"), pUint[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfUnsignedInt;
			}
			// create array string of float
			else if ((pVar->vt & ~VT_ARRAY) == VT_R4)
			{
				xmlArrayString.getBuffer(lElements * 30);
				float* pFloat = (float*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<float>%g</float>"), pFloat[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfFloat;
			}
			// create array string of double
			else if ((pVar->vt & ~VT_ARRAY) == VT_R8)
			{
				xmlArrayString.getBuffer(lElements * 40);
				double* pDouble = (double*)pData;

				for (i = 0; i < lElements; i++)
				{
					xmlHelper.format(_T("<double>%.15g</double>"), pDouble[i]);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfDouble;
			}
			// create array string of long
			else if ((pVar->vt & ~VT_ARRAY) == VT_I8)
			{
				xmlArrayString.getBuffer(lElements * 40);
				LONGLONG* pLong = (LONGLONG*)pData;

				for (i = 0; i < lElements; i++)
				{
#ifdef SOOS_LINUX
					xmlHelper.format(_T("<long>%lld</long>"), pLong[i]);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
					xmlHelper.format(_T("<long>%I64d</long>"), pLong[i]);
#endif
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfLong;
			}
			// create array string of unsigned long
			else if ((pVar->vt & ~VT_ARRAY) == VT_UI8)
			{
				xmlArrayString.getBuffer(lElements * 50);
				ULONGLONG* pULong = (ULONGLONG*)pData;

				for (i = 0; i < lElements; i++)
				{
#ifdef SOOS_LINUX
					xmlHelper.format(_T("<unsignedLong>%lld</unsignedLong>"), pULong[i]);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
					xmlHelper.format(_T("<unsignedLong>%I64d</unsignedLong>"), pULong[i]);
#endif
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfUnsignedLong;
			}
			// create array string of currency
			else if ((pVar->vt & ~VT_ARRAY) == VT_CY)
			{
				xmlArrayString.getBuffer(lElements * 40);
				CY* pCy = (CY*)pData;

				for (i = 0; i < lElements; i++)
				{
					SHORT decimalDigits = (SHORT)(pCy[i].int64 % 10000);

					if (decimalDigits < 0)
					{
						decimalDigits = -decimalDigits;
					}

#ifdef SOOS_LINUX
					xmlHelper.format(_T("<decimal>%lld.%4.4u</decimal>"), (pCy[i].int64 / 10000) , decimalDigits);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
					xmlHelper.format(_T("<decimal>%I64d.%4.4u</decimal>"), (pCy[i].int64 / 10000) , decimalDigits);
#endif
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfDecimal;
			}
			// create array string of dateTime
			else if ((pVar->vt & ~VT_ARRAY) == VT_DATE)
			{
				xmlArrayString.getBuffer(lElements * 41);
				DATE* pDate = (DATE*)pData;

				for (i = 0; i < lElements; i++)
				{
					SOCmnDateTime date(&pDate[i]);
					xmlHelper.format(_T("<dateTime>%s</dateTime>"),
									 (LPCTSTR)date.getXMLDateTime());
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfDateTime;
			}
			else if ((pVar->vt & ~VT_ARRAY) == VT_BSTR)
			{
				xmlArrayString.getBuffer(lElements * 50);
				BSTR* pBstr = (BSTR*)pData;

				for (i = 0; i < lElements; i++)
				{
					SOCmnStringEx str;
					SOCmnStringEx::getXMLString(pBstr[i], str);
					xmlHelper.format(_T("<string>%s</string>"), (LPCTSTR)str);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfString;
			}
			else if ((pVar->vt & ~VT_ARRAY) == VT_VARIANT)
			{
				VARIANT* pVariant = (VARIANT*)pData;

				for (i = 0; i < lElements; i++)
				{
					SOCmnStringEx elementXMLVal, elementXMLType;
					getXMLValue(&(pVariant[i]), elementXMLType, elementXMLVal);
					xmlHelper.format(_T("<anyType xsi:type=\"%s\">%s</anyType>"), (LPCTSTR)elementXMLType, (LPCTSTR)elementXMLVal);
					xmlArrayString += xmlHelper;
				}

				xmlType = g_szArrayOfAnyType;
			}

			SafeArrayUnaccessData(V_ARRAY(pVar));
			xmlVal = xmlArrayString;
		}
		else
		{
			// should never reach this !
			SOCmnString xV;
			::variant2string(&xV, (LPCVARIANT)pVar);
			xmlVal = xV;
			xmlType = _T("xsd:string");
		}

		break;
	}
} // getXMLValue

BOOL SOCmnVariant::getVARTYPE(IN LPCTSTR xmlType, OUT VARTYPE& vt)
{
	BOOL ret = TRUE;
	int idx;
	SOCmnString szXmlType(xmlType);
	SOCmnString szTypeOnly = szXmlType.mid((((idx = szXmlType.reverseFind(_T('|'))) == -1) ? idx : idx + 1));

	if (szXmlType.find(_T("http://www.w3.org/2001/XMLSchema")) != -1)
	{
		if (szTypeOnly == _T("boolean"))
		{
			vt = VT_BOOL;
		}
		else if (szTypeOnly == _T("byte"))
		{
			vt = VT_I1;
		}
		else if (szTypeOnly == _T("unsignedByte"))
		{
			vt = VT_UI1;
		}
		else if (szTypeOnly == _T("short"))
		{
			vt = VT_I2;
		}
		else if (szTypeOnly == _T("unsignedShort"))
		{
			vt = VT_UI2;
		}
		else if (szTypeOnly == _T("int"))
		{
			vt = VT_I4;
		}
		else if (szTypeOnly == _T("unsignedInt"))
		{
			vt = VT_UI4;
		}
		else if (szTypeOnly == _T("long"))
		{
			vt = VT_I8;
		}
		else if (szTypeOnly == _T("unsignedLong"))
		{
			vt = VT_UI8;
		}
		else if (szTypeOnly == _T("float"))
		{
			vt = VT_R4;
		}
		else if (szTypeOnly == _T("double"))
		{
			vt = VT_R8;
		}
		else if (szTypeOnly == _T("decimal"))
		{
			vt = VT_CY;
		}
		else if (szTypeOnly == _T("dateTime"))
		{
			vt = VT_DATE;
		}
		else if (szTypeOnly == _T("string"))
		{
			vt = VT_BSTR;
		}
		else if (szTypeOnly == _T("base64Binary"))
		{
			vt = VT_UI1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("any"))
		{
			vt = VT_EMPTY;
		}
		else if (szTypeOnly == _T("QName"))
		{
			vt = VT_BSTR;
		}
		else if (szTypeOnly == _T("OPCQuality"))
		{
			vt = VT_UI2;
		}
		else if (szTypeOnly == _T("ArrayOfBoolean"))
		{
			vt = VT_BOOL | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfByte"))
		{
			vt = VT_I1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfShort"))
		{
			vt = VT_I2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedShort"))
		{
			vt = VT_UI2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfInt"))
		{
			vt = VT_I4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedInt"))
		{
			vt = VT_UI4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfLong"))
		{
			vt = VT_I8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedLong"))
		{
			vt = VT_UI8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfFloat"))
		{
			vt = VT_R4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDouble"))
		{
			vt = VT_R8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDecimal"))
		{
			vt = VT_CY | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDateTime"))
		{
			vt = VT_DATE | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfString"))
		{
			vt = VT_BSTR | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfAnyType"))
		{
			vt = VT_VARIANT | VT_ARRAY;
		}
		else
		{
			ret = FALSE;
		}
	}
	else if (szXmlType.find(_T("http://opcfoundation.org/webservices/XMLDA/1.0/")) != -1)
	{
		if (szTypeOnly == _T("ArrayOfBoolean"))
		{
			vt = VT_BOOL | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfByte"))
		{
			vt = VT_I1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfShort"))
		{
			vt = VT_I2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedShort"))
		{
			vt = VT_UI2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfInt"))
		{
			vt = VT_I4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedInt"))
		{
			vt = VT_UI4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfLong"))
		{
			vt = VT_I8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedLong"))
		{
			vt = VT_UI8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfFloat"))
		{
			vt = VT_R4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDouble"))
		{
			vt = VT_R8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDecimal"))
		{
			vt = VT_CY | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDateTime"))
		{
			vt = VT_DATE | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfString"))
		{
			vt = VT_BSTR | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfAnyType"))
		{
			vt = VT_VARIANT | VT_ARRAY;
		}
		else if (szTypeOnly == _T("boolean"))
		{
			vt = VT_BOOL;
		}
		else if (szTypeOnly == _T("byte"))
		{
			vt = VT_I1;
		}
		else if (szTypeOnly == _T("unsignedByte"))
		{
			vt = VT_UI1;
		}
		else if (szTypeOnly == _T("short"))
		{
			vt = VT_I2;
		}
		else if (szTypeOnly == _T("unsignedShort"))
		{
			vt = VT_UI2;
		}
		else if (szTypeOnly == _T("int"))
		{
			vt = VT_I4;
		}
		else if (szTypeOnly == _T("unsignedInt"))
		{
			vt = VT_UI4;
		}
		else if (szTypeOnly == _T("long"))
		{
			vt = VT_I8;
		}
		else if (szTypeOnly == _T("unsignedLong"))
		{
			vt = VT_UI8;
		}
		else if (szTypeOnly == _T("float"))
		{
			vt = VT_R4;
		}
		else if (szTypeOnly == _T("double"))
		{
			vt = VT_R8;
		}
		else if (szTypeOnly == _T("decimal"))
		{
			vt = VT_CY;
		}
		else if (szTypeOnly == _T("dateTime"))
		{
			vt = VT_DATE;
		}
		else if (szTypeOnly == _T("string"))
		{
			vt = VT_BSTR;
		}
		else if (szTypeOnly == _T("base64Binary"))
		{
			vt = VT_UI1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("any"))
		{
			vt = VT_EMPTY;
		}
		else if (szTypeOnly == _T("QName"))
		{
			vt = VT_BSTR;
		}
		else if (szTypeOnly == _T("OPCQuality"))
		{
			vt = VT_UI2;
		}
		else
		{
			ret = FALSE;
		}
	}
	else
	{
		if (szTypeOnly == _T("ArrayOfBoolean"))
		{
			vt = VT_BOOL | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfByte"))
		{
			vt = VT_I1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfShort"))
		{
			vt = VT_I2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedShort"))
		{
			vt = VT_UI2 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfInt"))
		{
			vt = VT_I4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedInt"))
		{
			vt = VT_UI4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfLong"))
		{
			vt = VT_I8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfUnsignedLong"))
		{
			vt = VT_UI8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfFloat"))
		{
			vt = VT_R4 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDouble"))
		{
			vt = VT_R8 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDecimal"))
		{
			vt = VT_CY | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfDateTime"))
		{
			vt = VT_DATE | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfString"))
		{
			vt = VT_BSTR | VT_ARRAY;
		}
		else if (szTypeOnly == _T("ArrayOfAnyType"))
		{
			vt = VT_VARIANT | VT_ARRAY;
		}
		else if (szTypeOnly == _T("boolean"))
		{
			vt = VT_BOOL;
		}
		else if (szTypeOnly == _T("byte"))
		{
			vt = VT_I1;
		}
		else if (szTypeOnly == _T("unsignedByte"))
		{
			vt = VT_UI1;
		}
		else if (szTypeOnly == _T("short"))
		{
			vt = VT_I2;
		}
		else if (szTypeOnly == _T("unsignedShort"))
		{
			vt = VT_UI2;
		}
		else if (szTypeOnly == _T("int"))
		{
			vt = VT_I4;
		}
		else if (szTypeOnly == _T("unsignedInt"))
		{
			vt = VT_UI4;
		}
		else if (szTypeOnly == _T("long"))
		{
			vt = VT_I8;
		}
		else if (szTypeOnly == _T("unsignedLong"))
		{
			vt = VT_UI8;
		}
		else if (szTypeOnly == _T("float"))
		{
			vt = VT_R4;
		}
		else if (szTypeOnly == _T("double"))
		{
			vt = VT_R8;
		}
		else if (szTypeOnly == _T("decimal"))
		{
			vt = VT_CY;
		}
		else if (szTypeOnly == _T("dateTime"))
		{
			vt = VT_DATE;
		}
		else if (szTypeOnly == _T("string"))
		{
			vt = VT_BSTR;
		}
		else if (szTypeOnly == _T("base64Binary"))
		{
			vt = VT_UI1 | VT_ARRAY;
		}
		else if (szTypeOnly == _T("any"))
		{
			vt = VT_EMPTY;
		}
		else if (szTypeOnly == _T("QName"))
		{
			vt = VT_BSTR;
		}
		else
		{
			ret = FALSE;
		}
	}

	return ret;
}


//-----------------------------------------------------------------------------
// getXMLDataType
//
// - convert the vartype to a XML datatype string
//
// returns:
//		pointer to the datatype string
//
LPCTSTR SOCmnVariant::getXMLDataType(
	IN VARTYPE dataType,      // VARIANT datatype
	OUT SOCmnString& xmlType) // XML datatype
{
	switch (dataType)
	{
	case VT_BOOL:
		xmlType = _T("xsd:boolean");
		break;

	case VT_I1:
		xmlType = _T("xsd:byte");
		break;

	case VT_UI1:
		xmlType = _T("xsd:unsignedByte");
		break;

	case VT_I2:
		xmlType = _T("xsd:short");
		break;

	case VT_UI2:
		xmlType = _T("xsd:unsignedShort");
		break;

	case VT_I4:
	case VT_INT:
		xmlType = _T("xsd:int");
		break;

	case VT_UI4:
	case VT_UINT:
		xmlType = _T("xsd:unsignedInt");
		break;

	case VT_I8:
		xmlType = _T("xsd:long");
		break;

	case VT_UI8:
		xmlType = _T("xsd:unsignedLong");
		break;

	case VT_R4:
		xmlType = _T("xsd:float");
		break;

	case VT_R8:
		xmlType = _T("xsd:double");
		break;

	case VT_CY:
		xmlType = _T("xsd:decimal");
		break;

	case VT_DATE:
		xmlType = _T("xsd:dateTime");
		break;

	case VT_BSTR:
		xmlType = _T("xsd:string");
		break;

	case VT_BOOL | VT_ARRAY:
		xmlType = _T("ArrayOfBoolean");
		break;

	case VT_I1 | VT_ARRAY:
		xmlType = _T("ArrayOfByte");
		break;

	case VT_UI1 | VT_ARRAY:
		xmlType = _T("xsd:base64Binary");
		break;

	case VT_I2 | VT_ARRAY:
		xmlType = _T("ArrayOfShort");
		break;

	case VT_UI2 | VT_ARRAY:
		xmlType = _T("ArrayOfUnsignedShort");
		break;

	case VT_I4 | VT_ARRAY:
	case VT_INT | VT_ARRAY:
		xmlType = _T("ArrayOfInt");
		break;

	case VT_UI4 | VT_ARRAY:
	case VT_UINT | VT_ARRAY:
		xmlType = _T("ArrayOfUnsignedInt");
		break;

	case VT_I8 | VT_ARRAY:
		xmlType = _T("ArrayOfLong");
		break;

	case VT_UI8 | VT_ARRAY:
		xmlType = _T("ArrayOfUnsignedLong");
		break;

	case VT_R4 | VT_ARRAY:
		xmlType = _T("ArrayOfFloat");
		break;

	case VT_R8 | VT_ARRAY:
		xmlType = _T("ArrayOfDouble");
		break;

	case VT_CY | VT_ARRAY:
		xmlType = _T("ArrayOfDecimal");
		break;

	case VT_DATE | VT_ARRAY:
		xmlType = _T("ArrayOfDateTime");
		break;

	case VT_BSTR | VT_ARRAY:
		xmlType = _T("ArrayOfString");
		break;

	case VT_VARIANT | VT_ARRAY:
		xmlType = _T("ArrayOfAnyType");
		break;

	default: // should never reach this !
		xmlType = _T("xsd:anyType");
		break;
	}

	return xmlType;
} // getXMLDataType


//-----------------------------------------------------------------------------
// initFromXMLValue
//
// - set the value of the variant from the xml value and datatype string
//
// returns:
//		TRUE  - variant value set
//		FALSE - invalid input data
//
BOOL SOCmnVariant::initFromXMLValue(
	IN SOCmnXmlValue* pXmlValue)
{
	USES_CONVERSION;
	BOOL bRet = FALSE;
	SOCmnString szXmlType(pXmlValue->m_datatype);
	LONG idx = -1;
	SOCmnString szTypeOnly = szXmlType.mid((((idx = szXmlType.reverseFind(_T('|'))) == -1) ? idx : idx + 1));
	clear();

	if (!pXmlValue->m_simpleType)
	{
		// I have to handle an array of values
		if ((szXmlType.find(_T("http://opcfoundation.org/webservices/XMLDA/1.0")) == -1) &&
			(szXmlType.find(_T("http://www.w3.org/2001/XMLSchema")) == -1))
		{
			return bRet;
		}

		idx = 0;
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = pXmlValue->m_array.getCount();

		if (szTypeOnly != g_szArrayOfAnyType)
		{
			VARTYPE vtType = pXmlValue->XmlArrayElementType2Vartype();

			if (vtType != VT_EMPTY)
			{
				// I found a valid datatype...
				vt = (VARTYPE)(VT_ARRAY | vtType);
				parray = SOCmnVariant::safeArrayCreate(vtType, 1, &dim);
				bRet = TRUE; // empty array is OK, isnt it?
				SOCmnListPosition pos = pXmlValue->m_array.getStartPosition();

				while (pos)
				{
					SOCmnXmlValue* pXmlVal = pXmlValue->m_array.getNext(pos);
					SOCmnVariant arrayElem;

					if (arrayElem.initFromXMLValue(pXmlVal)
						&& vtType == arrayElem.vt)
					{
						if (vtType == VT_BSTR)
						{
							::SafeArrayPutElement(parray, &idx, arrayElem.bstrVal);
						}
						else
						{
							// I use the dblVal only to give the SafeArrayPutElement an adress for the value.
							::SafeArrayPutElement(parray, &idx, &arrayElem.dblVal);
						}

						idx++;
					}
					else
					{
						bRet = FALSE;
						clear();
						break;
					}
				}// end while
			}
		}
		else
		{
			// I have to handle the array of anyType here!!!
			vt = (VT_ARRAY | VT_VARIANT);
			parray = SOCmnVariant::safeArrayCreate(VT_VARIANT, 1, &dim);
			bRet = TRUE; // empty array is OK, isnt it?
			SOCmnListPosition pos = pXmlValue->m_array.getStartPosition();

			while (pos)
			{
				SOCmnXmlValue* pXmlVal = pXmlValue->m_array.getNext(pos);
				SOCmnVariant arrayElem;
				bRet = arrayElem.initFromXMLValue(pXmlVal);

				if (bRet)
				{
					::SafeArrayPutElement(parray, &idx, &arrayElem);
					idx++;
				}
				else
				{
					clear();
					break;
				}
			}// end while
		}

		return bRet;
	}
	else // it is a simple XML type; just convert it
	{
		if ((szXmlType.find(_T("http://opcfoundation.org/webservices/XMLDA/1.0")) == -1) &&
			(szXmlType.find(_T("http://www.w3.org/2001/XMLSchema")) == -1))
		{
			return bRet;
		}

		SOCmnString szXmlVal(pXmlValue->m_value);

		if (szTypeOnly == _T("boolean"))
		{
			szXmlVal.makeUpper();
			vt = VT_BOOL;
			bRet = TRUE;

			if ((szXmlVal == _T("TRUE")) || (szXmlVal == _T("1")))
			{
				boolVal = VARIANT_TRUE;
			}
			else if ((szXmlVal == _T("FALSE")) || (szXmlVal == _T("0")))
			{
				boolVal = VARIANT_FALSE;
			}
			else
			{
				bRet = FALSE;
			}
		}
		else if (szTypeOnly == _T("byte"))
		{
			vt = VT_I1;
			errno = 0;
			cVal = (char)_tcstol(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("unsignedByte"))
		{
			vt = VT_UI1;
			errno = 0;
			bVal = (unsigned char)_tcstoul(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("short"))
		{
			vt = VT_I2;
			errno = 0;
			iVal = (SHORT)_tcstol(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("unsignedShort"))
		{
			vt = VT_UI2;
			errno = 0;
			uiVal = (USHORT)_tcstoul(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("int"))
		{
			vt = VT_I4;
			errno = 0;
			lVal = _tcstol(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("unsignedInt"))
		{
			vt = VT_UI4;
			errno = 0;
			ulVal = _tcstoul(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("long"))
		{
			V_VT(this) = VT_I8;
			errno = 0;
			V_I8(this) = _tcstoi64(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("unsignedLong"))
		{
			V_VT(this) = VT_UI8;
			errno = 0;
			V_UI8(this) = _tcstoui64(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("float"))
		{
			vt = VT_R4;
			errno = 0;
			fltVal = (FLOAT)_tcstod(szXmlVal, NULL);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("double"))
		{
			vt = VT_R8;
			errno = 0;
			dblVal = _tcstod(szXmlVal, NULL);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("decimal"))
		{
			ULONGLONG dec;
			ULONGLONG subDec = 0;
			LPTSTR endP = NULL;
			LPTSTR endSubP = NULL;
			vt = VT_CY;
			errno = 0;
			dec = _tcstoui64(szXmlVal, &endP, 10);
			dec *= 10000;

			if (*endP == _T('.'))
			{
				DWORD i, cC;
				subDec = (_tcstoui64(endP + 1, &endSubP, 10));
				cC = (DWORD)(endSubP - endP - 1);

				for (i = 4; i > cC; i--)
				{
					subDec *= 10;
				}

				for (i = 4; i < cC; i++)
				{
					subDec /= 10;
				}
			}

			cyVal.int64 = dec + subDec;
			bRet = TRUE;
		}
		else if (szTypeOnly == _T("dateTime"))
		{
			SOCmnDateTime dt;
			bRet = dt.setXMLDateTime(szXmlVal);

			if (bRet)
			{
				vt = VT_DATE;
				dt.get(&date);
			}
		}
		else if ((szTypeOnly == _T("string")) || (szTypeOnly == _T("QName")))
		{
			V_VT(this) = VT_BSTR;

			if (szXmlVal.find(_T("_x")) != -1)
			{
				idx = 0;
				DWORD hexNumber;
				TCHAR hexSign[8];
				TCHAR newSign;

				do
				{
					idx = szXmlVal.find(_T("_x"), idx);

					if (idx != -1)
					{
						memset(hexSign, 0, 8 * sizeof(TCHAR));
						_tcsncpy(hexSign, (LPCTSTR)szXmlVal + idx, 7);

						if (hexSign[6] != _T('_'))
						{
							idx = idx + 1;
							continue;
						}

						if ((_istxdigit(hexSign[2])) && (_istxdigit(hexSign[3])) && (_istxdigit(hexSign[4])) && (_istxdigit(hexSign[5])))
						{
							hexNumber = _tcstoul(&hexSign[2], NULL, 16);
							newSign = (TCHAR)hexNumber;
							szXmlVal.replace(idx, idx + 7, &newSign, 1);
						}
						else
						{
							idx = idx + 1;
						}
					}
				}
				while (idx != -1);
			}

#ifdef SOOS_LINUX
			bstrVal = SysAllocString(T2OLE(szXmlVal));
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
			DWORD bStrLen;
			DWORD xmlStrLen = szXmlVal.getLength();
			LPCSTR xmlStrP;
			LPWSTR bStrCpy;
			xmlStrP = T2CA((LPCTSTR)szXmlVal);
			bStrCpy = (LPWSTR)malloc((xmlStrLen + 1) * 2);
#ifndef SOOS_WINDOWS_CE
			bStrLen =  MultiByteToWideChar(CP_UTF8, 0, xmlStrP, xmlStrLen, bStrCpy, xmlStrLen + 1);
#else
			// Windows CE does not support UTF8
			bStrLen =  MultiByteToWideChar(CP_ACP, 0, xmlStrP, xmlStrLen, bStrCpy, xmlStrLen + 1);
#endif
			bStrCpy[bStrLen] = L'\0';
			bstrVal = SysAllocString(bStrCpy);
			free(bStrCpy);
#endif
			bRet = bstrVal != NULL;
		}
		else if (szTypeOnly == _T("OPCQuality"))
		{
			V_VT(this) = VT_UI2;

			errno = 0;
			uiVal = (WORD) _tcstoul(szXmlVal, NULL, 10);
			bRet = (errno != ERANGE);
		}
		else if (szTypeOnly == _T("base64Binary"))
		{
			bRet = SOCmnXmlValue::decodeBase64String2Variant(szXmlVal, *this) >= 0;
		}
		else
		{
			// I should never reach this section
		}
	}

	return bRet;
}

#endif // SOFEATURE_CMN_XML


#ifdef SOOS_LINUX
BSTR SysAllocString(LPCTSTR sz)
{
	if (sz == NULL)
	{
		return NULL;
	}

	return SysAllocStringLen(sz, _tcslen(sz));
}

BSTR SysAllocStringLen(LPCTSTR pch, ULONG cch)
{
	ULONG* pmem = (ULONG*)malloc(sizeof(ULONG) + sizeof(TCHAR) * (cch + 1));
	BSTR bstr = NULL;

	if (pmem != NULL)
	{
		*pmem = cch;
		bstr = (BSTR)(pmem + 1);

		if (pch != NULL)
		{
			memcpy(bstr, pch, cch * sizeof(TCHAR));
		}

		bstr[cch] = _T('\0');
	}

	return bstr;
}

void SysFreeString(BSTR bstr)
{
	if (bstr != NULL)
	{
		free((ULONG*)bstr - 1);
	}
}

ULONG SysStringLen(BSTR bstr)
{
	ULONG len = 0;

	if (bstr != NULL)
	{
		len = *((ULONG*)bstr - 1);
	}

	return len;
}

void VariantInit(LPVARIANT pvarg)
{
	pvarg->vt = VT_EMPTY;
}

HRESULT VariantClear(LPVARIANT pvarg)
{
	HRESULT retval = S_OK;

	if (pvarg == NULL)
	{
		return E_INVALIDARG;
	}

	if (pvarg->vt & VT_ARRAY)
	{
		retval = SafeArrayDestroy(pvarg->parray);
	}
	else if (pvarg->vt == VT_BSTR)
	{
		SysFreeString(pvarg->bstrVal);
	}

	pvarg->vt = VT_EMPTY;
	return retval;
}

HRESULT VariantCopy(LPVARIANT pvargDest, LPVARIANT pvargSrc)
{
	if (pvargDest == NULL || pvargSrc == NULL)
	{
		return E_INVALIDARG;
	}

	if (pvargDest == pvargSrc)
	{
		return S_OK;
	}

	HRESULT retval = VariantClear(pvargDest);

	if (FAILED(retval))
	{
		return retval;
	}

	*pvargDest = *pvargSrc;

	if (pvargSrc->vt & VT_ARRAY)
	{
		retval = SafeArrayCopy(pvargSrc->parray, &pvargDest->parray);
	}
	else if (pvargSrc->vt == VT_BSTR)
	{
		pvargDest->bstrVal = SysAllocStringLen(pvargSrc->bstrVal,
											   SysStringLen(pvargSrc->bstrVal));

		if (pvargDest->bstrVal == NULL)
		{
			retval = E_OUTOFMEMORY;
		}
	}

	if (FAILED(retval))
	{
		pvargDest->vt = VT_EMPTY;
	}

	return retval;
}

HRESULT VariantChangeType(LPVARIANT pvargDest,
						  LPVARIANT pvargSrc, USHORT wFlags, VARTYPE vt)
{
	return VariantChangeTypeEx(pvargDest, pvargSrc, 0, wFlags, vt);
}

HRESULT VariantChangeTypeEx(LPVARIANT pvargDest,
							LPVARIANT pvargSrc, LCID lcid, USHORT wFlags, VARTYPE vt)
{
	TCHAR tmpval[64];
	BSTR  tmpstr = NULL;

	if (pvargDest == NULL || pvargSrc == NULL)
	{
		return E_INVALIDARG;
	}

	if (pvargDest != pvargSrc)
	{
		HRESULT hr = VariantClear(pvargDest);

		if (FAILED(hr))
		{
			return hr;
		}
	}

	if (vt == pvargSrc->vt)
	{
		return VariantCopy(pvargDest, pvargSrc);
	}
	else switch (MAKELONG(pvargSrc->vt, vt))      // from - low, to - high
		{
			// signed <-> unsigned conversions
		case MAKELONG(VT_I1, VT_UI1):
			if (pvargSrc->cVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_UI1, VT_I1):
			if (pvargSrc->bVal > CHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_I2, VT_UI2):
			if (pvargSrc->iVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_UI2, VT_I2):
			if (pvargSrc->uiVal > SHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_INT, VT_I4):
		case MAKELONG(VT_UINT, VT_I4):
		case MAKELONG(VT_UINT, VT_UI4):
		case MAKELONG(VT_I4, VT_INT):
			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_I4, VT_UI4):
		case MAKELONG(VT_INT, VT_UI4):
		case MAKELONG(VT_I4, VT_UINT):
			if (pvargSrc->lVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_UI4, VT_UINT):
		case MAKELONG(VT_UI4, VT_I4):
		case MAKELONG(VT_UI4, VT_INT):
			if (pvargSrc->ulVal > LONG_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_I8, VT_UI8):
			if (pvargSrc->llVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

		case MAKELONG(VT_UI8, VT_I8):
			if (pvargSrc->ullVal > ULLONG_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			VariantCopy(pvargDest, pvargSrc);
			break;

			// Numeric to BOOL
		case MAKELONG(VT_UI1, VT_BOOL):
		case MAKELONG(VT_I1, VT_BOOL):
			pvargDest->boolVal = pvargSrc->bVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

		case MAKELONG(VT_UI2, VT_BOOL):
		case MAKELONG(VT_I2, VT_BOOL):
			pvargDest->boolVal = pvargSrc->iVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

		case MAKELONG(VT_UI4, VT_BOOL):
		case MAKELONG(VT_I4, VT_BOOL):
		case MAKELONG(VT_UINT, VT_BOOL):
		case MAKELONG(VT_INT, VT_BOOL):
			pvargDest->boolVal = pvargSrc->lVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

		case MAKELONG(VT_UI8, VT_BOOL):
		case MAKELONG(VT_I8, VT_BOOL):
			pvargDest->boolVal = pvargSrc->llVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

		case MAKELONG(VT_R4, VT_BOOL):
			pvargDest->boolVal = pvargSrc->fltVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

		case MAKELONG(VT_R8, VT_BOOL):
			pvargDest->boolVal = pvargSrc->dblVal ? VARIANT_TRUE : VARIANT_FALSE;
			break;

			// BOOL to Numeric
		case MAKELONG(VT_BOOL, VT_UI1):
		case MAKELONG(VT_BOOL, VT_I1):
			pvargDest->cVal = (CHAR)pvargSrc->boolVal;
			break;

		case MAKELONG(VT_BOOL, VT_UI2):
		case MAKELONG(VT_BOOL, VT_I2):
			pvargDest->iVal = (SHORT)pvargSrc->boolVal;
			break;

		case MAKELONG(VT_BOOL, VT_UI4):
		case MAKELONG(VT_BOOL, VT_I4):
		case MAKELONG(VT_BOOL, VT_UINT):
		case MAKELONG(VT_BOOL, VT_INT):
			pvargDest->lVal = (LONG)pvargSrc->boolVal;
			break;

		case MAKELONG(VT_BOOL, VT_R4):
			pvargDest->fltVal = (float)pvargSrc->boolVal;
			break;

		case MAKELONG(VT_BOOL, VT_R8):
			pvargDest->dblVal = (double)pvargSrc->boolVal;
			break;

		case MAKELONG(VT_BOOL, VT_UI8):
		case MAKELONG(VT_BOOL, VT_I8):
			pvargDest->llVal = (LONGLONG)pvargSrc->boolVal;
			break;

			// Numeric to UI1
		case MAKELONG(VT_I2, VT_UI1):
		case MAKELONG(VT_UI2, VT_UI1):
			if (pvargSrc->uiVal > UCHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_UI1):
		case MAKELONG(VT_INT, VT_UI1):
		case MAKELONG(VT_UI4, VT_UI1):
		case MAKELONG(VT_UINT, VT_UI1):
			if (pvargSrc->ulVal > UCHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_UI1):
		case MAKELONG(VT_UI8, VT_UI1):
			if (pvargSrc->ullVal > UCHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_UI1):
			if ((pvargSrc->cyVal.int64 / 10000) > UCHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_UI1):
			if (pvargSrc->fltVal >= UCHAR_MAX + 0.5 || pvargSrc->fltVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = (BYTE)(MASK_BYTE & UnsignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_UI1):
			if (pvargSrc->dblVal >= UCHAR_MAX + 0.5 || pvargSrc->dblVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->bVal = (BYTE)(MASK_BYTE & UnsignedRound(pvargSrc->dblVal));
			break;

			// Numeric to I1
		case MAKELONG(VT_I2, VT_I1):
			if (pvargSrc->iVal > CHAR_MAX || pvargSrc->iVal < CHAR_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_I1):
			if (pvargSrc->uiVal > CHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_I1):
		case MAKELONG(VT_INT, VT_I1):
			if (pvargSrc->lVal > CHAR_MAX || pvargSrc->lVal < CHAR_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_I1):
		case MAKELONG(VT_UINT, VT_I1):
			if (pvargSrc->ulVal > CHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_I1):
			if (pvargSrc->llVal > CHAR_MAX || pvargSrc->llVal < CHAR_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_I1):
			if (pvargSrc->ullVal > CHAR_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_I1):
			if ((pvargSrc->cyVal.int64 / 10000) > CHAR_MAX || (pvargSrc->cyVal.int64 / 10000) < CHAR_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_I1):
			if (pvargSrc->fltVal >= CHAR_MAX + 0.5 || pvargSrc->fltVal <= CHAR_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = (CHAR)(MASK_CHAR & SignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_I1):
			if (pvargSrc->dblVal >= CHAR_MAX + 0.5 || pvargSrc->dblVal <= CHAR_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cVal = (CHAR)(MASK_CHAR & SignedRound(pvargSrc->dblVal));
			break;

			// Numeric to I2
		case MAKELONG(VT_UI1, VT_I2):
			pvargDest->iVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_I2):
			pvargDest->iVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I4, VT_I2):
		case MAKELONG(VT_INT, VT_I2):
			if (pvargSrc->lVal > SHRT_MAX || pvargSrc->lVal < SHRT_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_I2):
		case MAKELONG(VT_UINT, VT_I2):
			if (pvargSrc->ulVal > SHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_I2):
			if (pvargSrc->llVal > SHRT_MAX || pvargSrc->llVal < SHRT_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_I2):
			if (pvargSrc->ullVal > SHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_I2):
			if ((pvargSrc->cyVal.int64 / 10000) > SHRT_MAX || (pvargSrc->cyVal.int64 / 10000) < SHRT_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_I2):
			if (pvargSrc->fltVal >= SHRT_MAX + 0.5 || pvargSrc->fltVal <= SHRT_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = (SHORT)(MASK_SHORT & SignedRound(pvargSrc->fltVal)) ;
			break;

		case MAKELONG(VT_R8, VT_I2):
			if (pvargSrc->dblVal >= SHRT_MAX + 0.5 || pvargSrc->dblVal <= SHRT_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->iVal = (SHORT)(MASK_SHORT & SignedRound(pvargSrc->dblVal)) ;
			break;

			// Numeric to UI2
		case MAKELONG(VT_UI1, VT_UI2):
			pvargDest->uiVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_UI2):
			pvargDest->uiVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I4, VT_UI2):
		case MAKELONG(VT_INT, VT_UI2):
			if (pvargSrc->lVal > USHRT_MAX || pvargSrc->lVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_UI2):
		case MAKELONG(VT_UINT, VT_UI2):
			if (pvargSrc->ulVal > USHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_UI2):
			if (pvargSrc->llVal > USHRT_MAX || pvargSrc->llVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_UI2):
			if (pvargSrc->ullVal > USHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_UI2):
			if ((pvargSrc->cyVal.int64 / 10000) > USHRT_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_UI2):
			if (pvargSrc->fltVal >= USHRT_MAX + 0.5 || pvargSrc->fltVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = (USHORT)(MASK_SHORT & UnsignedRound(pvargSrc->fltVal)) ;
			break;

		case MAKELONG(VT_R8, VT_UI2):
			if (pvargSrc->dblVal >= USHRT_MAX + 0.5 || pvargSrc->dblVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->uiVal = (USHORT)(MASK_SHORT & UnsignedRound(pvargSrc->dblVal));
			break;

			// Numeric to I4
		case MAKELONG(VT_UI1, VT_I4):
		case MAKELONG(VT_UI1, VT_INT):
			pvargDest->lVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_I4):
		case MAKELONG(VT_I1, VT_INT):
			pvargDest->lVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_I4):
		case MAKELONG(VT_I2, VT_INT):
			pvargDest->lVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_I4):
		case MAKELONG(VT_UI2, VT_INT):
			pvargDest->lVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I8, VT_I4):
		case MAKELONG(VT_I8, VT_INT):
			if (pvargSrc->llVal > LONG_MAX || pvargSrc->llVal < LONG_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->lVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_I4):
		case MAKELONG(VT_UI8, VT_INT):
			if (pvargSrc->ullVal > LONG_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->lVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_I4):
		case MAKELONG(VT_CY, VT_INT):
			if ((pvargSrc->cyVal.int64 / 10000) > LONG_MAX || (pvargSrc->cyVal.int64 / 10000) < LONG_MIN)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->lVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_I4):
		case MAKELONG(VT_R4, VT_INT):
			if (pvargSrc->fltVal >= LONG_MAX + 0.5 || pvargSrc->fltVal <= LONG_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->lVal = (LONG)(MASK_LONG & SignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_I4):
		case MAKELONG(VT_R8, VT_INT):
			if (pvargSrc->dblVal >= LONG_MAX + 0.5 || pvargSrc->dblVal <= LONG_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->lVal = (LONG)(MASK_LONG & SignedRound(pvargSrc->dblVal));
			break;

			// Numeric to UI4
		case MAKELONG(VT_UI1, VT_UI4):
		case MAKELONG(VT_UI1, VT_UINT):
			pvargDest->ulVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_UI4):
		case MAKELONG(VT_I1, VT_UINT):
			if (pvargSrc->cVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_UI4):
		case MAKELONG(VT_I2, VT_UINT):
			if (pvargSrc->iVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_UI4):
		case MAKELONG(VT_UI2, VT_UINT):
			pvargDest->ulVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I8, VT_UI4):
		case MAKELONG(VT_I8, VT_UINT):
			if (pvargSrc->llVal > ULONG_MAX || pvargSrc->llVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_UI4):
		case MAKELONG(VT_UI8, VT_UINT):
			if (pvargSrc->ullVal > ULONG_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_UI4):
		case MAKELONG(VT_CY, VT_UINT):
			if ((pvargSrc->cyVal.int64 / 10000) > ULONG_MAX)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_UI4):
		case MAKELONG(VT_R4, VT_UINT):
			if (pvargSrc->fltVal >= ULONG_MAX + 0.5 || pvargSrc->fltVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = (ULONG)(MASK_LONG & UnsignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_UI4):
		case MAKELONG(VT_R8, VT_UINT):
			if (pvargSrc->dblVal >= ULONG_MAX + 0.5 || pvargSrc->dblVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ulVal = (ULONG)(MASK_LONG & UnsignedRound(pvargSrc->dblVal));
			break;

			// Numeric to I8
		case MAKELONG(VT_UI1, VT_I8):
			pvargDest->llVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_I8):
			pvargDest->llVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_I8):
			pvargDest->llVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_I8):
			pvargDest->llVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_I8):
		case MAKELONG(VT_INT, VT_I8):
			pvargDest->llVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_I8):
		case MAKELONG(VT_UINT, VT_I8):
			pvargDest->llVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_CY, VT_I8):
			pvargDest->llVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_I8):
			if (pvargSrc->fltVal >= LLONG_MAX + 0.5 || pvargSrc->fltVal <= LLONG_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->llVal = (LONGLONG)(MASK_LONGLONG & SignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_I8):
			if (pvargSrc->dblVal >= LLONG_MAX + 0.5 || pvargSrc->dblVal <= LLONG_MIN - 0.5)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->llVal = (LONGLONG)(MASK_LONGLONG & SignedRound(pvargSrc->dblVal));
			break;

			// Numeric to UI8
		case MAKELONG(VT_UI1, VT_UI8):
			pvargDest->ullVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_UI8):
			if (pvargSrc->cVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ullVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_UI8):
			if (pvargSrc->iVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ullVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_UI8):
			pvargDest->ullVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_UI8):
		case MAKELONG(VT_INT, VT_UI8):
			if (pvargSrc->lVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ullVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_UI8):
		case MAKELONG(VT_UINT, VT_UI8):
			pvargDest->ullVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_CY, VT_UI8):
			pvargDest->ullVal = pvargSrc->cyVal.int64 / 10000;
			break;

		case MAKELONG(VT_R4, VT_UI8):
			if (pvargSrc->fltVal >= ULLONG_MAX + 0.5 || pvargSrc->fltVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ullVal = (ULONGLONG)(MASK_LONGLONG & UnsignedRound(pvargSrc->fltVal));
			break;

		case MAKELONG(VT_R8, VT_UI8):
			if (pvargSrc->dblVal >= ULLONG_MAX + 0.5 || pvargSrc->dblVal < 0)
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->ullVal = (ULONGLONG)(MASK_LONGLONG & UnsignedRound(pvargSrc->dblVal));
			break;

			// Numeric to CY
		case MAKELONG(VT_UI1, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_CY):
		case MAKELONG(VT_INT, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_CY):
		case MAKELONG(VT_UINT, VT_CY):
			pvargDest->cyVal.int64 = 10000 * (LONGLONG)pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_CY):
			if ((pvargSrc->llVal > (LLONG_MAX / 10000)) || (pvargSrc->llVal < (LLONG_MIN / 10000)))
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cyVal.int64 = 10000 * pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_CY):
			if (pvargSrc->ullVal > (ULLONG_MAX / 10000))
			{
				return DISP_E_OVERFLOW;
			}

			pvargDest->cyVal.int64 = 10000 * pvargSrc->ullVal;
			break;

		case MAKELONG(VT_R4, VT_CY):
			pvargDest->cyVal.int64 = (LONGLONG)(pvargSrc->fltVal * 10000.0);
			break;

		case MAKELONG(VT_R8, VT_CY):
			pvargDest->cyVal.int64 = (LONGLONG)(pvargSrc->dblVal * 10000.0);
			break;

			// Numeric to R4
		case MAKELONG(VT_UI1, VT_R4):
			pvargDest->fltVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_R4):
			pvargDest->fltVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_R4):
			pvargDest->fltVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_R4):
			pvargDest->fltVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_R4):
		case MAKELONG(VT_INT, VT_R4):
			pvargDest->fltVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_R4):
		case MAKELONG(VT_UINT, VT_R4):
			pvargDest->fltVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_R4):
			pvargDest->fltVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_R4):
			pvargDest->fltVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_R4):
			pvargDest->fltVal = pvargSrc->cyVal.int64 / 10000.0;
			break;

		case MAKELONG(VT_R8, VT_R4):
			if ((pvargSrc->dblVal < FLOAT_INFINITE_PLUS) && (pvargSrc->dblVal > FLOAT_INFINITE_MINUS))
			{
				pvargDest->fltVal = pvargSrc->dblVal;
			}
			else
			{
				return DISP_E_OVERFLOW;
			}

			break;

			// Numeric to R8
		case MAKELONG(VT_UI1, VT_R8):
			pvargDest->dblVal = pvargSrc->bVal;
			break;

		case MAKELONG(VT_I1, VT_R8):
			pvargDest->dblVal = pvargSrc->cVal;
			break;

		case MAKELONG(VT_I2, VT_R8):
			pvargDest->dblVal = pvargSrc->iVal;
			break;

		case MAKELONG(VT_UI2, VT_R8):
			pvargDest->dblVal = pvargSrc->uiVal;
			break;

		case MAKELONG(VT_I4, VT_R8):
		case MAKELONG(VT_INT, VT_R8):
			pvargDest->dblVal = pvargSrc->lVal;
			break;

		case MAKELONG(VT_UI4, VT_R8):
		case MAKELONG(VT_UINT, VT_R8):
			pvargDest->dblVal = pvargSrc->ulVal;
			break;

		case MAKELONG(VT_I8, VT_R8):
			pvargDest->dblVal = pvargSrc->llVal;
			break;

		case MAKELONG(VT_UI8, VT_R8):
			pvargDest->dblVal = pvargSrc->ullVal;
			break;

		case MAKELONG(VT_CY, VT_R8):
			pvargDest->dblVal = pvargSrc->cyVal.int64 / 10000.0;
			break;

		case MAKELONG(VT_R4, VT_R8):
			pvargDest->dblVal = pvargSrc->fltVal;
			break;

			// Numeric to String
		case MAKELONG(VT_I1, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%i", pvargSrc->cVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_I2, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%i", pvargSrc->iVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_I4, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%li", pvargSrc->lVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_I8, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%lli", pvargSrc->llVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_UI1, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%u", pvargSrc->bVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_UI2, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%u", pvargSrc->uiVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_UI4, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%lu", pvargSrc->ulVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_UI8, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%llu", pvargSrc->ullVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_CY, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%.15g", pvargSrc->cyVal.int64 / 10000.0);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_R4, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%g", pvargSrc->fltVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

		case MAKELONG(VT_R8, VT_BSTR):
			_sntprintf(tmpval, sizeof(tmpval), "%.15g", pvargSrc->dblVal);
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

			// String to Numeric
		case MAKELONG(VT_BSTR, VT_I1):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->cVal = strtol(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_UI1):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->cVal = strtoul(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_I2):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->iVal = strtol(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_UI2):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->uiVal = strtoul(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_I4):
		case MAKELONG(VT_BSTR, VT_INT):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->lVal = strtol(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_UI4):
		case MAKELONG(VT_BSTR, VT_UINT):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->ulVal = strtoul(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_I8):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->llVal = strtoll(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_UI8):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->ullVal = strtoull(tmpstr, NULL, 0);
			break;

		case MAKELONG(VT_BSTR, VT_CY):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->cyVal.int64 = (LONGLONG)(strtod(tmpstr, NULL) * 10000.0);
			break;

		case MAKELONG(VT_BSTR, VT_R4):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->fltVal = strtod(tmpstr, NULL);
			break;

		case MAKELONG(VT_BSTR, VT_R8):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->dblVal = strtod(tmpstr, NULL);
			break;

			// VT_BSTR <-> VT_BOOL
		case MAKELONG(VT_BOOL, VT_BSTR):
			if (wFlags & VARIANT_ALPHABOOL)
				pvargDest->bstrVal = SysAllocString(pvargSrc->boolVal == VARIANT_FALSE
													? _T("False") : _T("True"));
			else
				pvargDest->bstrVal = SysAllocString(pvargSrc->boolVal == VARIANT_FALSE
													? _T("0") : _T("-1"));

			break;

		case MAKELONG(VT_BSTR, VT_BOOL):
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			pvargDest->boolVal = (*tmpstr != _T('\0') && *tmpstr != _T('0') &&
								  *tmpstr != _T('F') && *tmpstr != _T('f'));
			break;

			// VT_DATE -> VT_BSTR
		case MAKELONG(VT_DATE, VT_BSTR):
			ctime_r(&pvargSrc->date.tv_sec, tmpval);
			/* tmpval = "Wed Jun 30 21:49:08 1993\n" */
			tmpval[24] = _T('\0'); /* delete NL char from end */
			pvargDest->bstrVal = SysAllocString(tmpval);
			break;

			// VT_DATE -> VT_BOOL
		case MAKELONG(VT_DATE, VT_BOOL):
			printf("\n\n\n-----------Alla\n\n\n");
			pvargDest->boolVal = (pvargSrc->date.tv_sec == 0 && pvargSrc->date.tv_usec == 0) ? VARIANT_FALSE : VARIANT_TRUE;
			break;

			// VT_BOOL -> VT_DATE
		case MAKELONG(VT_BOOL, VT_DATE):
			printf("\n\n\n-----------Balla\n\n\n");
			pvargDest->date.tv_sec = 0;
			pvargDest->date.tv_usec = (pvargSrc->boolVal == VARIANT_TRUE) ? 1 : 0;
			break;

			// VT_BSTR <-> VT_DATE
		case MAKELONG(VT_BSTR, VT_DATE):
		{
			tmpstr = pvargSrc->bstrVal;

			if (tmpstr == NULL)
			{
				return E_INVALIDARG;
			}

			struct tm stm;

			memset(&stm, 0, sizeof(stm));

			stm.tm_isdst = -1; /* <-- make mktime ignore this */

			strptime(tmpstr, "%c", &stm);

			pvargDest->date.tv_sec = mktime(&stm);

			pvargDest->date.tv_usec = 0;

			break;
		}

		default:
			return DISP_E_TYPEMISMATCH;
		}

	pvargDest->vt = vt;

	if (tmpstr != NULL && pvargSrc == pvargDest)
	{
		SysFreeString(tmpstr);
	}

	if (vt == VT_BSTR && pvargDest->bstrVal == NULL)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}

LPSAFEARRAY SafeArrayCreate(VARTYPE vt, USHORT cDims, LPSAFEARRAYBOUND rgsabound)
{
	if (cDims == 0)
	{
		return NULL;    /* invalid arguments */
	}

	USHORT fFeatures = 0;
	ULONG  cbElements = 0;
	ULONG  nElements = 1;

	for (USHORT i = 0; i < cDims; i++)
	{
		nElements *= rgsabound[i].cElements;
	}

	switch (vt)
	{
	case VT_I1:
	case VT_UI1:
		cbElements = sizeof(BYTE);
		break;

	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbElements = sizeof(SHORT);
		break;

	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
		cbElements = sizeof(LONG);
		break;

	case VT_R8:
	case VT_CY:
	case VT_DATE:
	case VT_I8:
	case VT_UI8:
		cbElements = sizeof(DOUBLE);
		break;

	case VT_VARIANT:
		cbElements = sizeof(VARIANT);
		fFeatures = FADF_VARIANT;
		break;

	case VT_BSTR:
		cbElements = sizeof(BSTR);
		fFeatures = FADF_BSTR;
		break;

	default:
		return NULL; /* can't handle this */
	}

	ULONG descr_size  = sizeof(SAFEARRAY) + (cDims - 1) * sizeof(SAFEARRAYBOUND);
	ULONG data_size   = cbElements * nElements;
	SAFEARRAY* retval = (SAFEARRAY*)malloc(descr_size);

	if (retval != NULL)
	{
		retval->cDims = cDims;
		retval->fFeatures = fFeatures;
		retval->cbElements = cbElements;
		retval->cLocks = 0;
		retval->pvData = malloc(data_size);
		memcpy(retval->rgsabound, rgsabound, cDims * sizeof(SAFEARRAYBOUND));

		if (retval->pvData == NULL)
		{
			free(retval);
			return NULL;
		}

		if (fFeatures & FADF_VARIANT)
		{
			for (ULONG i = 0; i < nElements; i++)
			{
				VariantInit(& ((VARIANT*)retval->pvData) [i]);
			}
		}
		else
		{
			/* VT_BSTR: set elements = NULL */
			memset(retval->pvData, 0, data_size);
		}
	}

	return retval;
}

LPSAFEARRAY SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements)
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = lLbound;
	rgsabound[0].cElements = cElements;
	return SafeArrayCreate(vt, 1, rgsabound);
}

HRESULT SafeArrayDestroy(LPSAFEARRAY psa)
{
	if (psa == NULL)
	{
		return E_INVALIDARG;
	}

	if (psa->cLocks != 0)
	{
		return DISP_E_ARRAYISLOCKED;
	}

	ULONG  nElements = 1;

	for (USHORT i = 0; i < psa->cDims; i++)
	{
		nElements *= psa->rgsabound[i].cElements;
	}

	if (psa->fFeatures & FADF_BSTR)
	{
		while (nElements != 0)
		{
			SysFreeString(((BSTR*)psa->pvData) [--nElements]);
		}
	}
	else if (psa->fFeatures & FADF_VARIANT)
	{
		while (nElements != 0)
		{
			VariantClear(& ((VARIANT*)psa->pvData) [--nElements]);
		}
	}

	free(psa->pvData);
	free(psa);
	return S_OK;
}

HRESULT SafeArrayCopy(LPSAFEARRAY psa, LPSAFEARRAY* ppsaOut)
{
	if (ppsaOut == NULL)
	{
		return E_INVALIDARG;
	}

	*ppsaOut = NULL;

	if (psa == NULL)
	{
		return E_INVALIDARG;
	}

	if (psa->cLocks != 0)
	{
		return DISP_E_ARRAYISLOCKED;
	}

	ULONG   nElements = 1;

	for (USHORT i = 0; i < psa->cDims; i++)
	{
		nElements *= psa->rgsabound[i].cElements;
	}

	ULONG descr_size = sizeof(SAFEARRAY) + (psa->cDims - 1) * sizeof(SAFEARRAYBOUND);
	ULONG data_size  = psa->cbElements * nElements;
	SAFEARRAY* psaout = (SAFEARRAY*)malloc(descr_size);

	if (psaout == NULL)
	{
		return E_OUTOFMEMORY;
	}

	memcpy(psaout, psa, descr_size);
	psaout->pvData = malloc(data_size);

	if (psaout->pvData == NULL)
	{
		free(psaout);
		return E_OUTOFMEMORY;
	}

	HRESULT retval = S_OK;

	if (psa->fFeatures & FADF_BSTR)
	{
		BSTR* pvOutData = (BSTR*)psaout->pvData;
		BSTR* pvInData  = (BSTR*)psa->pvData;

		for (ULONG i = 0; i < nElements; i++)
		{
			pvOutData[i] = SysAllocStringLen(pvInData[i], SysStringLen(pvInData[i]));

			if (pvOutData[i] == NULL)
			{
				retval = E_OUTOFMEMORY;
			}
		}
	}
	else if (psa->fFeatures & FADF_VARIANT)
	{
		LPVARIANT pvOutData = (VARIANT*)psaout->pvData;
		LPVARIANT pvInData  = (VARIANT*)psa->pvData;

		for (ULONG i = 0; i < nElements; i++)
		{
			VariantInit(&pvOutData[i]);
			HRESULT hr = VariantCopy(&pvOutData[i], &pvInData[i]);

			if (FAILED(hr))
			{
				retval = hr;
			}
		}
	}
	else
	{
		memcpy(psaout->pvData, psa->pvData, data_size);
	}

	*ppsaOut = psaout;
	return retval;
}

HRESULT SafeArrayRedim(SAFEARRAY* psa, SAFEARRAYBOUND* psaboundNew)
{
	/* If you reduce the bound of an array, SafeArrayRedim
	 * deallocates the array elements outside the new array boundary.
	 * If the bound of an array is increased, SafeArrayRedim allocates
	 * and initializes the new array elements. The data is preserved
	 * for elements that exist in both the old and new array.
	 */
	if (psa == NULL || psaboundNew == NULL)
	{
		return E_INVALIDARG;
	}

	if (psa->cLocks != 0)
	{
		return DISP_E_ARRAYISLOCKED;
	}

	ULONG nElements = 1;
	assert(psa->cDims > 0);

	for (USHORT n = 0; n < psa->cDims - 1; n++)
	{
		nElements *= psa->rgsabound[n].cElements;
	}

	ULONG i;
	ULONG new_nElements = nElements * psaboundNew->cElements;
	ULONG old_nElements = nElements * psa->rgsabound[psa->cDims - 1].cElements;

	// allocate memory first
	if (new_nElements > old_nElements)
	{
		LPVOID pv = realloc(psa->pvData, psa->cbElements * new_nElements);

		if (pv == NULL)
		{
			return E_OUTOFMEMORY;
		}

		psa->pvData = pv;

		if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = old_nElements; i < new_nElements; i++)
			{
				VariantInit(& ((VARIANT*)psa->pvData) [i]);
			}
		}
		else
		{
			/* VT_BSTR: set elements = NULL */
			memset((BYTE*)psa->pvData + psa->cbElements * old_nElements, 0,
				   psa->cbElements * (new_nElements - old_nElements));
		}
	}

#if 0 /* WINDOWS does not move elements ... */
	LONG offset = nElements * (psa->rgsabound[psa->cDims - 1].lLbound
							   - psaboundNew->lLbound);

	// move data to lower addresses
	if (offset < 0 && (ULONG) - offset < old_nElements)
	{
		if (psa->fFeatures & FADF_BSTR)
		{
			for (i = 0; i < (ULONG) - offset; i++)
			{
				SysFreeString(((BSTR*)psa->pvData)[i]);
			}
		}
		else if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = 0; i < (ULONG) - offset; i++)
			{
				VariantClear(& ((VARIANT*)psa->pvData)[i]);
			}
		}

		memmove((BYTE*)psa->pvData,
				(BYTE*)psa->pvData + psa->cbElements * (ULONG) - offset,
				psa->cbElements * (old_nElements + offset));

		if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = old_nElements + offset; i < old_nElements; i++)
			{
				VariantInit(& ((VARIANT*)psa->pvData) [i]);
			}
		}
		else
		{
			/* VT_BSTR: set elements = NULL */
			memset((BYTE*)psa->pvData + psa->cbElements *
				   (old_nElements + offset), 0,
				   psa->cbElements * (ULONG) - offset);
		}
	} /* if (offset < 0 && ... */

#endif /* WINDOWS does not move elements ... */

	// free memory
	if (new_nElements < old_nElements)
	{
		if (psa->fFeatures & FADF_BSTR)
		{
			for (i = new_nElements; i < old_nElements; i++)
			{
				SysFreeString(((BSTR*)psa->pvData)[i]);
			}
		}
		else if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = new_nElements; i < old_nElements; i++)
			{
				VariantClear(& ((VARIANT*)psa->pvData)[i]);
			}
		}

		psa->pvData = realloc(psa->pvData, psa->cbElements * new_nElements);
		assert(psa->pvData != NULL || new_nElements == 0);
	}

#if 0 /* WINDOWS does not move elements ... */

	// move data to higher addresses
	if (offset > 0 && (ULONG)offset < new_nElements)
	{
		if (psa->fFeatures & FADF_BSTR)
		{
			for (i = new_nElements - offset; i < new_nElements; i++)
			{
				SysFreeString(((BSTR*)psa->pvData)[i]);
			}
		}
		else if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = new_nElements - offset; i < new_nElements; i++)
			{
				VariantClear(& ((VARIANT*)psa->pvData)[i]);
			}
		}

		memmove((BYTE*)psa->pvData + psa->cbElements * offset,
				(BYTE*)psa->pvData,
				psa->cbElements * (new_nElements - offset));

		if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = 0; i < (ULONG)offset; i++)
			{
				VariantInit(& ((VARIANT*)psa->pvData) [i]);
			}
		}
		else
		{
			/* VT_BSTR: set elements = NULL */
			memset((BYTE*)psa->pvData, 0,
				   psa->cbElements * offset);
		}
	} /* if (offset > 0 && ... */

	// clear all memory iff offset out of range
	if (offset > 0 && (ULONG)offset >= new_nElements ||
		offset < 0 && (ULONG) - offset >= old_nElements)
	{
		if (psa->fFeatures & FADF_VARIANT)
		{
			for (i = 0; i < new_nElements; i++)
			{
				VariantClear(& ((VARIANT*)psa->pvData) [i]);
			}
		}
		else
		{
			if (psa->fFeatures & FADF_BSTR)
			{
				for (i = 0; i < new_nElements; i++)
				{
					SysFreeString(((BSTR*)psa->pvData) [i]);
				}
			}

			memset((BYTE*)psa->pvData, 0,
				   psa->cbElements * new_nElements);
		}
	}

#endif /* WINDOWS does not move elements ... */
	psa->rgsabound[psa->cDims - 1] = *psaboundNew;
	return S_OK;
}

USHORT SafeArrayGetDim(LPSAFEARRAY psa)
{
	return psa->cDims;
}

ULONG  SafeArrayGetElemsize(LPSAFEARRAY psa)
{
	return psa->cbElements;
}

HRESULT SafeArrayGetUBound(LPSAFEARRAY psa, USHORT nDim, LONG* plUbound)
{
	if (psa == NULL || plUbound == NULL)
	{
		return E_INVALIDARG;
	}

	if (nDim == 0   || nDim > psa->cDims)
	{
		return DISP_E_BADINDEX;
	}

	*plUbound = psa->rgsabound[nDim - 1].lLbound +
				psa->rgsabound[nDim - 1].cElements - 1;
	return S_OK;
}

HRESULT SafeArrayGetLBound(LPSAFEARRAY psa, USHORT nDim, LONG* plLbound)
{
	if (psa == NULL || plLbound == NULL)
	{
		return E_INVALIDARG;
	}

	if (nDim == 0   || nDim > psa->cDims)
	{
		return DISP_E_BADINDEX;
	}

	*plLbound = psa->rgsabound[nDim - 1].lLbound;
	return S_OK;
}

HRESULT SafeArrayAccessData(LPSAFEARRAY psa, void** ppvData)
{
	if (psa == NULL || ppvData == NULL)
	{
		return E_INVALIDARG;
	}

	if (psa->cLocks >= USHRT_MAX)
	{
		return E_UNEXPECTED;
	}

	psa->cLocks++;
	*ppvData = psa->pvData;
	return S_OK;
}

HRESULT SafeArrayUnaccessData(LPSAFEARRAY psa)
{
	if (psa == NULL)
	{
		return E_INVALIDARG;
	}

	if (psa->cLocks == 0)
	{
		return E_UNEXPECTED;
	}

	psa->cLocks--;
	return S_OK;
}

HRESULT SafeArrayGetElement(LPSAFEARRAY psa, LONG* rgIndices, void* pv)
{
	HRESULT retval = S_OK;
	ULONG index = 0;

	if (psa == NULL || rgIndices == NULL || pv == NULL)
	{
		return E_INVALIDARG;
	}

	assert(psa->cDims > 0);

	for (int i = psa->cDims - 1; i >= 0; i--)
	{
		if (rgIndices[i] <  psa->rgsabound[i].lLbound ||
			rgIndices[i] >= psa->rgsabound[i].lLbound +
			(LONG)psa->rgsabound[i].cElements)
		{
			return DISP_E_BADINDEX;
		}

		index *= psa->rgsabound[i].cElements;
		index += rgIndices[i] - psa->rgsabound[i].lLbound;
	}

	if (psa->fFeatures & FADF_BSTR)
	{
		BSTR* p_elem = (BSTR*)psa->pvData;

		if ((*(BSTR*)pv = SysAllocStringLen(p_elem[index],
											SysStringLen(p_elem[index]))) == NULL)
		{
			retval = E_OUTOFMEMORY;
		}
	}
	else if (psa->fFeatures & FADF_VARIANT)
	{
		retval = VariantCopy((VARIANT*)pv, (VARIANT*)psa->pvData + index);
	}
	else
	{
		memcpy(pv, (BYTE*)psa->pvData + psa->cbElements * index, psa->cbElements);
	}

	return retval;
}

HRESULT SafeArrayPutElement(LPSAFEARRAY psa, LONG* rgIndices, void* pv)
{
	HRESULT retval = S_OK;
	ULONG index = 0;

	if (psa == NULL || rgIndices == NULL /* || pv == NULL */)
	{
		return E_INVALIDARG;
	}

	assert(psa->cDims > 0);

	for (int i = psa->cDims - 1; i >= 0; i--)
	{
		if (rgIndices[i] <  psa->rgsabound[i].lLbound ||
			rgIndices[i] >= psa->rgsabound[i].lLbound +
			(LONG)psa->rgsabound[i].cElements)
		{
			return DISP_E_BADINDEX;
		}

		index *= psa->rgsabound[i].cElements;
		index += rgIndices[i] - psa->rgsabound[i].lLbound;
	}

	if (psa->fFeatures & FADF_BSTR)
	{
		/* The variant types VT_DISPATCH, VT_UNKNOWN, and VT_BSTR are pointers,
		 * and do not require another level of indirection.
		 */
		BSTR* p_elem = (BSTR*)psa->pvData;
		SysFreeString(p_elem[index]);
		p_elem[index] = SysAllocStringLen((BSTR)pv, SysStringLen((BSTR)pv));

		if (p_elem[index] == NULL)
		{
			retval = E_OUTOFMEMORY;
		}
	}
	else if (psa->fFeatures & FADF_VARIANT)
	{
		retval = VariantCopy((VARIANT*)psa->pvData + index, (VARIANT*)pv);
	}
	else
	{
		memcpy((BYTE*)psa->pvData + psa->cbElements * index, pv, psa->cbElements);
	}

	return retval;
}

#endif // SOOS_LINUX


#ifdef SOCMNVARIANT_IMP_I64

#define FL_UNSIGNED   1       /* strtouq called */
#define FL_NEG        2       /* negative sign found */
#define FL_OVERFLOW   4       /* overflow occured */
#define FL_READDIGIT  8       /* we've read at least one correct digit */

static unsigned __int64  strtoxq(
	const char* nptr,
	char** endptr,
	int ibase,
	int flags
)
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

__int64   _strtoi64(
	const char* nptr,
	char** endptr,
	int ibase
)
{
	return (__int64) strtoxq(nptr, endptr, ibase, 0);
}
unsigned __int64   _strtoui64(
	const char* nptr,
	char** endptr,
	int ibase
)
{
	return strtoxq(nptr, endptr, ibase, FL_UNSIGNED);
}

int _wchartodigit(wchar_t ch)
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

static unsigned __int64  wcstoxq(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase,
	int flags
)
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


__int64   _wcstoi64(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase
)
{
	return (__int64) wcstoxq(nptr, endptr, ibase, 0);
}
unsigned __int64   _wcstoui64(
	const wchar_t* nptr,
	wchar_t** endptr,
	int ibase
)
{
	return wcstoxq(nptr, endptr, ibase, FL_UNSIGNED);
}

#endif // SOCMNVARIANT_IMP_I64

#endif // SOFEATURE_CMN_VARIANT
