#include "stdafx.h"
#include "DefaultElement.h"

BasicElement::BasicElement(tstring name) : DemoDaAddressSpaceElement()
{
	setName(name);
}

BasicElement::BasicElement(VARENUM varType)
{
	m_varType = varType;
	setAccessRights(EnumAccessRights_READWRITEABLE);
	setIoMode(EnumIoMode_POLL);
	DateTime time;
	time.now();
	Variant theValue;
	Variant dateVariant;
	unsigned long arraySize = 5;
	SAFEARRAYBOUND dim;
	dim.lLbound = 0;
	dim.cElements = arraySize;
	tstring name;

	switch (varType)
	{
	case VT_I1:
		setDatatype(VT_I1);
		name = _T("I1");
		setName(name);
		break;

	case VT_UI1:
		setDatatype(VT_UI1);
		name = _T("UI1");
		setName(name);
		break;

	case VT_I2:
		setDatatype(VT_I2);
		name = _T("I2");
		setName(name);
		break;

	case VT_UI2:
		setDatatype(VT_UI2);
		name = _T("UI2");
		setName(name);
		break;

	case VT_I4:
		setDatatype(VT_I4);
		name = _T("I4");
		setName(name);
		break;

	case VT_UI4:
		setDatatype(VT_UI4);
		name = _T("UI4");
		setName(name);
		break;

	case VT_I8:
		setDatatype(VT_I8);
		name = _T("I8");
		setName(name);
		break;

	case VT_UI8:
		setDatatype(VT_UI8);
		name = _T("UI8");
		setName(name);
		break;

	case VT_R4:
		setDatatype(VT_R4);
		name = _T("R4");
		setName(name);
		break;

	case VT_BOOL:
		setDatatype(VT_BOOL);
		name = _T("Bool");
		setName(name);
		break;

	case VT_BSTR:
		setDatatype(VT_BSTR);
		name = _T("BSTR");
		setName(name);
		break;

	case VT_DATE:
		setDatatype(VT_DATE);
		name = _T("Date");
		setName(name);
		break;

	case VT_ARRAY | VT_I1:
		setDatatype((VARENUM)(VT_ARRAY | VT_I1));
		name = _T("I1 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_UI1:
		setDatatype((VARENUM)(VT_ARRAY | VT_UI1));
		name = _T("UI1 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_I2:
		setDatatype((VARENUM)(VT_ARRAY | VT_I2));
		name = _T("I2 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_UI2:
		setDatatype((VARENUM)(VT_ARRAY | VT_UI2));
		name = _T("UI2 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_I4:
		setDatatype((VARENUM)(VT_ARRAY | VT_I4));
		name = _T("I4 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_UI4:
		setDatatype((VARENUM)(VT_ARRAY | VT_UI4));
		name = _T("UI4 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_R4:
		setDatatype((VARENUM)(VT_ARRAY | VT_R4));
		name = _T("R4 Array");
		setName(name);
		break;

	case VT_ARRAY | VT_BOOL:
		setDatatype((VARENUM)(VT_ARRAY | VT_BOOL));
		name = _T("Bool Array");
		setName(name);
		break;

	case VT_ARRAY | VT_BSTR:
		setDatatype((VARENUM)(VT_ARRAY | VT_BSTR));
		name = _T("BSTR Array");
		setName(name);
		break;

	case VT_ARRAY | VT_DATE:
		setDatatype((VARENUM)(VT_ARRAY | VT_DATE));
		name = _T("Date Array");
		setName(name);
		break;

	default:
		//nothing to do
		break;
	}
}

void BasicElement::init()
{
	DateTime time;
	time.now();
	Variant theValue;
	Variant dateVariant;
	unsigned long arraySize = 5;
	SAFEARRAYBOUND dim;
	dim.lLbound = 0;
	dim.cElements = arraySize;

	switch (m_varType)
	{
	case VT_I1:
	{
		theValue.setI1(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_UI1:
	{
		theValue.setUI1(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_I2:
	{
		theValue.setI2(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_UI2:
	{
		theValue.setUI2(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_I4:
	{
		theValue.setI4(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_UI4:
	{
		theValue.setUI4(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_I8:
	{
		theValue.setI8(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_UI8:
	{
		theValue.setUI8(0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_R4:
	{
		theValue.setR4(0.0);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_BOOL:
	{
		theValue.setBOOL(TRUE);
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_BSTR:
	{
		theValue.setBSTR(_T("DEMO"));
		ValueQT aValueQT(theValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_DATE:
	{
#ifdef TBC_OS_WINDOWS
		DATE pDate;
		time.get(pDate);
		dateVariant.setDATE(pDate);
		ValueQT aValueQT(dateVariant, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
#endif
#ifdef TBC_OS_LINUX
		timeval tv;
		time.get(&tv);
		Variant dateVariantLinux(tv);
		ValueQT valueQTDate(dateVariantLinux, EnumQuality_GOOD, time);
		valueChanged(valueQTDate);
#endif
	}
	break;

	case VT_ARRAY | VT_I1:
	{
		void* byteArray;
		Variant byteArrayValue;
		byteArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I1);
		byteArrayValue.parray = Variant::safeArrayCreate(byteArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(byteArrayValue.parray, (void**)&byteArray);

		for (unsigned long i = 0; i < arraySize; i++)
		{
			char* pVal = ((char*)byteArray) + i;
			*pVal = (char)i;
		} // end for

		Variant::SafeArrayUnaccessData(byteArrayValue.parray);
		ValueQT aValueQT(byteArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_UI1:
	{
		void* uByteArray;
		Variant uByteArrayValue;
		uByteArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI1);
		uByteArrayValue.parray = Variant::safeArrayCreate(uByteArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uByteArrayValue.parray, (void**)&uByteArray);

		for (unsigned long j = 0; j < arraySize; j++)
		{
			unsigned char* pVal = ((unsigned char*)uByteArray) + j;
			*pVal = (unsigned char)j;
		} // end for

		Variant::SafeArrayUnaccessData(uByteArrayValue.parray);
		ValueQT aValueQT(uByteArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_I2:
	{
		void* shortArray;
		Variant shortArrayValue;
		shortArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I2);
		shortArrayValue.parray = Variant::safeArrayCreate(shortArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(shortArrayValue.parray, (void**)&shortArray);

		for (unsigned long k = 0; k < arraySize; k++)
		{
			short* pVal = ((short*)shortArray) + k;
			*pVal = (short)k;
		} // end for

		Variant::SafeArrayUnaccessData(shortArrayValue.parray);
		ValueQT aValueQT(shortArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_UI2:
	{
		void* uShortArray;
		Variant uShortArrayValue;
		uShortArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI2);
		uShortArrayValue.parray = Variant::safeArrayCreate(uShortArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uShortArrayValue.parray, (void**)&uShortArray);

		for (unsigned long l = 0; l < arraySize; l++)
		{
			unsigned short* pVal = ((unsigned short*)uShortArray) + l;
			*pVal = (unsigned short)l;
		} // end for

		Variant::SafeArrayUnaccessData(uShortArrayValue.parray);
		ValueQT aValueQT(uShortArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_I4:
	{
		void* longArray;
		Variant longArrayValue;
		longArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I4);
		longArrayValue.parray = Variant::safeArrayCreate(longArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(longArrayValue.parray, (void**)&longArray);

		for (unsigned long m = 0; m < arraySize; m++)
		{
			long* pVal = ((long*)longArray) + m;
			*pVal = (long)m;
		} // end for

		Variant::SafeArrayUnaccessData(longArrayValue.parray);
		ValueQT aValueQT(longArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_UI4:
	{
		void* uLongArray;
		Variant uLongArrayValue;
		uLongArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI4);
		uLongArrayValue.parray = Variant::safeArrayCreate(uLongArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uLongArrayValue.parray, (void**)&uLongArray);

		for (unsigned long n = 0; n < arraySize; n++)
		{
			unsigned long* pVal = ((unsigned long*)uLongArray) + n;
			*pVal = n;
		} // end for

		Variant::SafeArrayUnaccessData(uLongArrayValue.parray);
		ValueQT aValueQT(uLongArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_R4:
	{
		void* floatArray;
		Variant floatArrayValue;
		floatArrayValue.vt = (VARENUM)(VT_ARRAY | VT_R4);
		floatArrayValue.parray = Variant::safeArrayCreate(floatArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(floatArrayValue.parray, (void**)&floatArray);

		for (unsigned long o = 0; o < arraySize; o++)
		{
			float* pVal = ((float*)floatArray) + o;
			*pVal = (float)o;
		} // end for

		Variant::SafeArrayUnaccessData(floatArrayValue.parray);
		ValueQT aValueQT(floatArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_BOOL:
	{
		void* boolArray = NULL;
		Variant boolArrayValue;
		boolArrayValue.vt = (VARENUM)(VT_ARRAY | VT_BOOL);
		boolArrayValue.parray = Variant::safeArrayCreate(boolArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(boolArrayValue.parray, (void**)&boolArray);

		for (unsigned long q = 0; q < arraySize; q++)
		{
			bool* pVal = ((bool*)boolArray) + q;

			if (q % 2 == 0)
			{
				*pVal = true;
			}
			else
			{
				*pVal = false;
			} // end if .. else
		} // end for

		Variant::SafeArrayUnaccessData(boolArrayValue.parray);
		ValueQT aValueQT(boolArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_BSTR:
	{
		void* strArray;
		Variant strArrayValue;
		strArrayValue.vt = (VARENUM)(VT_ARRAY | VT_BSTR);
		strArrayValue.parray = Variant::safeArrayCreate(strArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(strArrayValue.parray, (void**)&strArray);

		for (unsigned long r = 0; r < arraySize; r++)
		{
#ifdef TBC_OS_WINDOWS
			DWORD j = r % 26;
			BSTR* pVal = ((BSTR*)strArray) + r;
			unsigned short wstr[3];
			wstr[0] = L'A' + (unsigned short)j;
			wstr[1] = L'a' + (unsigned short)(j + 1);
			wstr[2] = 0;
			*pVal = SysAllocString((WCHAR*)wstr);
#endif
#ifdef TBC_OS_LINUX
			DWORD j = r % 26;
			TCHAR** pVal = ((TCHAR**)strArray) + r;
			TCHAR str[3];
			str[0] = _T('A') + (unsigned short)j;
			str[1] = _T('a') + (unsigned short)(j + 1);
			str[2] = 0;
			*pVal = OTSysAllocString((TCHAR*)str);
#endif
		} // end for

		Variant::SafeArrayUnaccessData(strArrayValue.parray);
		ValueQT aValueQT(strArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	case VT_ARRAY | VT_DATE:
	{
		void* dateArray;
		Variant dateArrayValue;
		dateArrayValue.vt = (VARENUM)(VT_ARRAY | VT_DATE);
		dateArrayValue.parray = Variant::safeArrayCreate(dateArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(dateArrayValue.parray, (void**)&dateArray);

		for (unsigned long t = 0; t < arraySize; t++)
		{
#ifdef TBC_OS_WINDOWS
			DATE* pVal = ((DATE*)dateArray) + t;
			time.get(*pVal);
#endif
#ifdef TBC_OS_LINUX
			timeval* pVal = ((timeval*)dateArray) + t;
			time.get(pVal);
#endif
		} // end for

		Variant::SafeArrayUnaccessData(dateArrayValue.parray);
		ValueQT aValueQT(dateArrayValue, EnumQuality_GOOD, time);
		valueChanged(aValueQT);
	}
	break;

	default:
	{
		//nothing to do
	}
	break;
	}
}

void BasicElement::handleReadRequest(IN DaRequest* pRequest)
{
	if (pRequest)
	{
		DaAddressSpaceElement* pElement = pRequest->getAddressSpaceElement();

		if (pElement)
		{
			// get address space element value take the Toolkit cache value
			ValueQT cacheValue;
			pElement->getCacheValue(cacheValue);
			pRequest->setValue(cacheValue);
			pRequest->setResult(S_OK);
		}
		else
		{
			// should never get here
			pRequest->setResult(E_FAIL);
		}
	}
}

void BasicElement::handleWriteRequest(IN DaRequest* pRequest)
{
	if (pRequest)
	{
		DaAddressSpaceElement* pElement = pRequest->getAddressSpaceElement();

		if (pElement)
		{
			ValueQT* pValue = pRequest->getValue();
			pRequest->setResult(pElement->valueChanged(*pValue));
		}
		else
		{
			// should never get here
			pRequest->setResult(E_FAIL);
		}
	}
}

BasicStaticElement::BasicStaticElement(tstring name) : BasicElement(name)
{
}

BasicStaticElement::BasicStaticElement(VARENUM varType) : BasicElement(varType)
{
}


BasicDymanicElement::BasicDymanicElement(tstring name) : BasicElement(name)
{
}

BasicDymanicElement::BasicDymanicElement(VARENUM varType) : BasicElement(varType)
{
}

void BasicDymanicElement::simulation(void)
{
	ValueQT value;
	DateTime now;
	now.now();

	switch (m_varType)
	{
	case VT_I1:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.cVal++;

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_UI1:
	{
		getCacheValue(value);
		Variant valueVariant = value.getData();
		valueVariant.bVal++;

		ValueQT val(valueVariant, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_I2:
	{
		getCacheValue(value);
		Variant valueVariant = value.getData();
		valueVariant.iVal++;

		ValueQT val(valueVariant, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_UI2:
	{
		getCacheValue(value);
		Variant valueVariant = value.getData();
		valueVariant.uiVal++;

		ValueQT val(valueVariant, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_I4:
	{
		getCacheValue(value);
		Variant valueVariant = value.getData();
		valueVariant.lVal++;

		ValueQT val(valueVariant, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_UI4:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.ulVal++;

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_I8:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.llVal++;

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_UI8:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.ullVal++;

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_R4:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.fltVal++;

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_BOOL:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		variantValue.boolVal = !(variantValue.boolVal);

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_BSTR:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		incrementWString(variantValue.bstrVal);

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case VT_DATE:
	{
		getCacheValue(value);
		Variant variantValue = value.getData();
		incrementDate(&(variantValue.date));

		ValueQT val(variantValue, EnumQuality_GOOD, now);
		valueChanged(val);
	}
	break;

	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_R4):
	case (VT_ARRAY | VT_BOOL):
	case (VT_ARRAY | VT_BSTR):
	case (VT_ARRAY | VT_DATE):
	{
		getCacheValue(value);
		LONG lBound = 0;
		LONG uBound = 0;
		LONG idx;
		void* voidArray = 0;
		Variant variantValue = value.getData();
		Variant::SafeArrayGetLBound(variantValue.parray, 1, &lBound);
		Variant::SafeArrayGetUBound(variantValue.parray, 1, &uBound);
		Variant::SafeArrayAccessData(variantValue.parray, (void**)&voidArray);

		for (idx = lBound; idx <= uBound; idx++)
		{
			VARENUM type = (VARENUM)(variantValue.vt & ~VT_ARRAY);

			switch (type)
			{
			case VT_I1:
			{
				CHAR* cVal = ((CHAR*)voidArray) + idx;
				(*cVal)++;
			}
			break;

			case VT_UI1:
			{
				BYTE* bVal = ((BYTE*)voidArray) + idx;
				(*bVal)++;
			}
			break;

			case VT_I2:
			{
				SHORT* iVal = ((SHORT*)voidArray) + idx;
				(*iVal)++;
			}
			break;

			case VT_UI2:
			{
				USHORT* uiVal = ((USHORT*)voidArray) + idx;
				(*uiVal)++;
			}
			break;

			case VT_I4:
			{
				LONG* lVal = ((LONG*)voidArray) + idx;
				(*lVal)++;
			}
			break;

			case VT_UI4:
			{
				ULONG* ulVal = ((ULONG*)voidArray) + idx;
				(*ulVal)++;
			}
			break;

			case VT_R4:
			{
				FLOAT* fltVal = ((FLOAT*)voidArray) + idx;
				(*fltVal)++;
			}
			break;

			case VT_BOOL:
			{
				VARIANT_BOOL* boolVal = ((VARIANT_BOOL*)voidArray) + idx;
				(*boolVal) = !(*boolVal);
			}
			break;

			case VT_BSTR:
			{
				BSTR* bstrVal = ((BSTR*)voidArray) + idx;
				incrementWString(*bstrVal);
			}
			break;

			case VT_DATE:
			{
				DATE* dateVal = ((DATE*)voidArray) + idx;
				incrementDate(dateVal);
			}
			break;

			default:
			{
				// nothing to do
			}
			break;
			}
		}

		Variant::SafeArrayUnaccessData(variantValue.parray);
		ValueQT aValueQT(variantValue, EnumQuality_GOOD, now);
		valueChanged(aValueQT);
	}
	break;

	default:
	{
		//nothing to do
	}
	break;
	}
}

void BasicDymanicElement::incrementWString(IN BSTR w)
{
#ifdef TBC_OS_WINDOWS

	if (w)
	{
		while (*w != 0)
		{
			(*w)++;

			if (*w == OLESTR('Z') + 1)
			{
				*w = OLESTR('A');
			}

			if (*w == OLESTR('z') + 1)
			{
				*w = OLESTR('a');
			}

			if (*w == OLESTR('9') + 1)
			{
				*w = OLESTR('0');
			}

			if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
				  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
				  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
			{
				*w = L' ';
			}

			w++;
		}
	}

#endif
#ifdef TBC_OS_LINUX

	if (w)
	{
		while (*w != 0)
		{
			(*w)++;

			if (*w == ('Z') + 1)
			{
				*w = ('A');
			}

			if (*w == ('z') + 1)
			{
				*w = ('a');
			}

			if (*w == ('9') + 1)
			{
				*w = ('0');
			}

			if (!(((*w >= ('A')) && (*w <= ('Z'))) ||
				  ((*w >= ('a')) && (*w <= ('z'))) ||
				  ((*w >= ('0')) && (*w <= ('9')))))
			{
				*w = _TCHAR(' ');
			}

			w++;
		}
	}

#endif
}

void BasicDymanicElement::incrementDate(IN DATE* pDate)
{
	if (pDate)
	{
#ifdef TBC_OS_WINDOWS
		*pDate += 1.0 / (24 * 3600);
#else // TBC_OS_WINDOWS
		++pDate->tv_sec;
#endif
	}
}

