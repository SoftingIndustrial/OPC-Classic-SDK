#include "stdafx.h"

#ifdef SOFEATURE_TUNNEL

#include "SOCmn.h"
#include "SOCmnTP.h"

//-----------------------------------------------------------------------------
// TP_Marshaler                                                               |
//-----------------------------------------------------------------------------

SOCmnTP_Marshaler::SOCmnTP_Marshaler(WORD flags)
	: msg_buf(0), msg_len(0), alloc_len(0), next_pos(0)
{
	m_flags = flags;
}

SOCmnTP_Marshaler::~SOCmnTP_Marshaler()
{
	free(msg_buf);
}

void SOCmnTP_Marshaler::BeginMarshal(u_long alloc_hint)
{
	alloc_len = alloc_hint;
	msg_len = next_pos = 0;
	free(msg_buf);
	msg_buf = (u_char*)malloc(alloc_len);
}

int SOCmnTP_Marshaler::ReserveSpace(u_long size, u_long align)
{
	if (!msg_buf)
	{
		return 0;
	}

	if (!align)
	{
		align = size;
	}

	if (align > 4)
	{
		align = 4;
	}
	else if (align == 0 || align == 3)
	{
		align = 1;
	}

	cur_pos = (next_pos + align - 1) & ~(align - 1);
	next_pos = cur_pos + size;

	if (next_pos > msg_len)
	{
		msg_len = next_pos;

		if (alloc_len < msg_len)
		{
			alloc_len = msg_len * 2;
			msg_buf = (u_char*)realloc(msg_buf, alloc_len);
		}
	}

	return msg_buf != 0;
}

void SOCmnTP_Marshaler::MarshalDate(const DATE& value)
{
	SOCmnDateTime date(&value);
	MarshalDate(date);
}

void SOCmnTP_Marshaler::MarshalDate(const SOCmnDateTime& value)
{
	if ((m_flags & TP_INIT_FLAG_USE_FULL_DATE) != 0)
	{
		// use complete date
#ifdef SOOS_WINDOWS
		ULONGLONG ftValue;
		value.get((FILETIME*)&ftValue);
		MarshalQuad(ftValue);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		ULONGLONG timeval;

		if (value.isSet())
		{
			timeval = value.getMicroseconds() * 10;
		}
		else
		{
			timeval = (ULONGLONG) - 1;
		}

		MarshalQuad(timeval);
#endif // SOS_LINUX
	}
	else
	{
		//  Old flavor datetime transport format (microseconds)
		ULONGLONG timeval;

		if (value.isSet())
		{
			timeval = value.getMicroseconds();
		}
		else
		{
			timeval = (ULONGLONG) - 1;
		}

		MarshalQuad(timeval);
	}
}

void SOCmnTP_Marshaler::MarshalString(LPCTSTR value)
{
#ifdef SOOS_WINDOWS
	USES_CONVERSION;
	LPCWSTR unicode = T2CW(value);
	u_long len = wcslen(unicode);
	LPSTR utf8 = (LPSTR)malloc(3 * len); /* note: 1 UNICODE character can be translated up to 3 UTF8 characters */

	if (!utf8)
	{
		MarshalLong(0);
		return;
	}

	if (!(m_flags & TP_INIT_FLAG_USE_UTF16_STRINGS))
	{
		// UTF-8
#ifndef SOOS_WINDOWS_CE
		len = WideCharToMultiByte(CP_UTF8, 0, unicode, len, utf8, 3 * len, NULL, NULL);
#else
		// Windows CE does not support UTF8
		len = WideCharToMultiByte(CP_ACP, 0, unicode, len, utf8, 3 * len, NULL, NULL);
#endif
	}
	else
	{
		// UTF-16
		len *= 2;
		memcpy(utf8, unicode, len);
	}

	MarshalLong(len);
	MarshalBlob(utf8, len);
	free(utf8);
#endif
#ifdef SOOS_LINUX
	u_long len = strlen(value);
	MarshalLong(len);
	MarshalBlob(value, len);
#endif
}

void SOCmnTP_Marshaler::MarshalBSTR(BSTR value)
{
#ifdef SOOS_WINDOWS
	u_long len = SysStringLen(value);
	LPSTR utf8 = (LPSTR)malloc(3 * len);

	if (!utf8)
	{
		MarshalLong(0);
		return;
	}

	if (!(m_flags & TP_INIT_FLAG_USE_UTF16_STRINGS))
	{
		// UTF-8
#ifndef SOOS_WINDOWS_CE
		len = WideCharToMultiByte(CP_UTF8, 0, value, len, utf8, 3 * len, NULL, NULL);
#else
		// Windows CE does not support UTF8
		len = WideCharToMultiByte(CP_ACP, 0, value, len, utf8, 3 * len, NULL, NULL);
#endif
	}
	else
	{
		// UTF-16
		len *= 2;
		memcpy(utf8, value, len);
	}

	MarshalLong(len);
	MarshalBlob(utf8, len);
	free(utf8);
#endif
#ifdef SOOS_LINUX
	u_long len = SysStringLen(value);
	MarshalLong(len);
	MarshalBlob(value, len);
#endif
}

void SOCmnTP_Marshaler::MarshalDWORDList(const SOCmnDWORDList* value)
{
	SOCmnDWORDList list(*value);
	SOCmnListPosition pos = list.getStartPosition();
	MarshalLong(list.getCount());

	while (pos != NULL)
	{
		MarshalLong(list.getNext(pos));
	}
}

void SOCmnTP_Marshaler::MarshalVariant(LPCVARIANT value)
{
	MarshalShort(V_VT(value));

	if (V_ISARRAY(value))
	{
		MarshalSafeArray(V_ARRAY(value), (VARTYPE)(V_VT(value) & VT_TYPEMASK));
	}
	else switch (V_VT(value))
		{
		case VT_BOOL:
			MarshalShort(V_BOOL(value));
			break;

		case VT_UI1:
			MarshalChar(V_UI1(value));
			break;

		case VT_I1:
			MarshalChar(V_I1(value));
			break;

		case VT_I2:
			MarshalShort(V_I2(value));
			break;

		case VT_UI2:
			MarshalShort(V_UI2(value));
			break;

		case VT_I4:
			MarshalLong(V_I4(value));
			break;

		case VT_UI4:
			MarshalLong(V_UI4(value));
			break;

		case VT_INT:
			MarshalLong(V_INT(value));
			break;

		case VT_UINT:
			MarshalLong(V_UINT(value));
			break;

		case VT_I8:
			MarshalQuad(V_I8(value));
			break;

		case VT_UI8:
			MarshalQuad(V_UI8(value));
			break;

		case VT_CY:
			MarshalQuad(V_CY(value).int64);
			break;

		case VT_R4:
			MarshalFloat(V_R4(value));
			break;

		case VT_R8:
			MarshalDouble(V_R8(value));
			break;

		case VT_DATE:
			MarshalDate(V_DATE(value));
			break;

		case VT_BSTR:
			MarshalBSTR(V_BSTR(value));
			break;
		}
}

void SOCmnTP_Marshaler::MarshalSafeArray(LPSAFEARRAY value, VARTYPE vt)
{
	ULONG  nElements;
	USHORT i;

	/*
	 * If VARTYPE was given, check if it's consistent with the SAFEARRAY,
	 * if no VARTYPE was given, guess a likely VARTYPE from the SAFEARRAY.
	 * We need the VARTYPE for proper Marshalling of DATEs.
	 */

	if (value == NULL || value->cDims == 0)
	{
		vt = VT_EMPTY;
		value = NULL;
	}

	switch (vt)
	{
	case VT_I1:
	case VT_UI1:
		if (value->cbElements != sizeof(BYTE))
		{
			vt = VT_EMPTY;
		}

		break;

	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		if (value->cbElements != sizeof(WORD))
		{
			vt = VT_EMPTY;
		}

		break;

	case VT_BSTR:
		if (!(value->fFeatures & FADF_BSTR))
		{
			vt = VT_EMPTY;
		}

		/* fall thru */
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
		if (value->cbElements != sizeof(LONG))
		{
			vt = VT_EMPTY;
		}

		break;

	case VT_R8:
		if (value->cbElements != sizeof(DOUBLE))
		{
			vt = VT_EMPTY;
		}

		break;

	case VT_CY:
	case VT_DATE:
	case VT_I8:
	case VT_UI8:
		if (value->cbElements != sizeof(ULONGLONG))
		{
			vt = VT_EMPTY;
		}

		break;

	case VT_VARIANT:
		if (!(value->fFeatures & FADF_VARIANT))
		{
			vt = VT_EMPTY;
		}

		if (value->cbElements != sizeof(VARIANT))
		{
			vt = VT_EMPTY;
		}

		break;

	default:
		vt = VT_EMPTY;
	}

	if (vt == VT_EMPTY && value != NULL)
		switch (value->cbElements)
		{
		case sizeof(BYTE):
			vt = VT_UI1;
			break;

		case sizeof(WORD):
			vt = VT_I2;
			break;

		case sizeof(LONG):
			if (value->fFeatures & FADF_BSTR)
			{
				vt = VT_BSTR;
			}
			else
			{
				vt = VT_I4;
			}

			break;

		case sizeof(DOUBLE):
			vt = VT_R8;
			break;

		case sizeof(VARIANT):
			if (value->fFeatures & FADF_VARIANT)
			{
				vt = VT_VARIANT;
			}

			break;
		}

	MarshalShort(vt);

	if (vt == VT_EMPTY)
	{
		return;
	}

	MarshalShort(value->cDims);

	for (i = 0, nElements = 1; i < value->cDims; i++)
	{
		MarshalLong(value->rgsabound[i].cElements);
		MarshalLong(value->rgsabound[i].lLbound);
		nElements *= value->rgsabound[i].cElements;
	}

	switch (vt)
	{
	case VT_UI1:
	case VT_I1:
	{
		BYTE* data;
		SafeArrayAccessData(value, (void**)&data);
		MarshalBlob(data, nElements);
		SafeArrayUnaccessData(value);
	}
	break;

	case VT_BOOL:
	case VT_I2:
	case VT_UI2:
	{
		WORD* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalShort(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_I4:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
	{
		DWORD* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalLong(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_I8:
	case VT_UI8:
	case VT_CY:
	{
		ULONGLONG* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalQuad(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_R4:
	{
		FLOAT* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalFloat(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_R8:
	{
		DOUBLE* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalDouble(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_DATE:
	{
		DATE* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalDate(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_BSTR:
	{
		BSTR* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalBSTR(data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;

	case VT_VARIANT:
	{
		VARIANT* data;
		SafeArrayAccessData(value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			MarshalVariant(&data[k]);
		}

		SafeArrayUnaccessData(value);
	}
	break;
	}
}

//-----------------------------------------------------------------------------
// TP_UnMarshaler                                                             |
//-----------------------------------------------------------------------------

SOCmnTP_UnMarshaler::SOCmnTP_UnMarshaler(WORD flags)
	: msg_buf(0), msg_len(0), next_pos(0)
{
	m_flags = flags;
}

SOCmnTP_UnMarshaler::~SOCmnTP_UnMarshaler()
{
	free(msg_buf);
}

int SOCmnTP_UnMarshaler::SetMarshalData(u_char* data, u_long size)
{
	msg_len = 0;
	free(msg_buf);
	msg_buf = 0;
	return AppendMarshalData(data, size);
}

int SOCmnTP_UnMarshaler::AppendMarshalData(u_char* data, u_long size)
{
	if (size > 0x10000000)
	{
		return 0;
	}

	msg_buf = (u_char*)realloc(msg_buf, msg_len + size);

	if (!msg_buf)
	{
		return 0;
	}

	memcpy(msg_buf + msg_len, data, size);
	msg_len += size;
	next_pos = 0;
	return 1;
}

int SOCmnTP_UnMarshaler::CheckSpace(u_long size, u_long align)
{
	if (!msg_buf)
	{
		return 0;
	}

	if (!align)
	{
		align = size;
	}

	if (align > 4)
	{
		align = 4;
	}
	else if (align == 0 || align == 3)
	{
		align = 1;
	}

	cur_pos = (next_pos + align - 1) & ~(align - 1);
	next_pos = cur_pos + size;
	return next_pos <= msg_len;
}

int SOCmnTP_UnMarshaler::UnMarshalArrayLen(u_long* value)
{
	if (!UnMarshalLong(value))
	{
		*value = 0;
		return 0;
	}

	if (*value > GetMarshalDataLen())
	{
		*value = 0;
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalDate(DATE* value)
{
	SOCmnDateTime date;

	if (!UnMarshalDate(&date))
	{
		return 0;
	}

#ifdef SOOS_WINDOWS

	if (!date.get(value))
	{
		date.clear();
		date.get(value);
	}

#endif
#ifdef SOOS_LINUX
	date.get(value);
#endif
	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalDate(SOCmnDateTime* value)
{
	ULONGLONG timeval;

	if (!UnMarshalQuad(&timeval))
	{
		return 0;
	}

	if ((m_flags & TP_INIT_FLAG_USE_FULL_DATE) != 0)
	{
#ifdef SOOS_WINDOWS
		value->set((const FILETIME*)&timeval);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		value->setMicroseconds(timeval / 10);
#endif // SOS_LINUX
	}
	else
	{
		value->setMicroseconds(timeval);
	}

	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalString(SOCmnString* value)
{
	USES_CONVERSION;
	u_long len;
	LPSTR utf8;

	if (!UnMarshalArrayLen(&len))
	{
		return 0;
	}

	utf8 = (LPSTR)malloc(len);

	if (!utf8)
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!UnMarshalBlob(utf8, len))
	{
		free(utf8);
		return 0;
	}

#ifdef SOOS_WINDOWS
	LPWSTR unicode = (LPWSTR)malloc((len + 1) * sizeof(WCHAR));

	if (!unicode)
	{
		free(utf8);
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!(m_flags & TP_INIT_FLAG_USE_UTF16_STRINGS))
	{
		// UTF-8
#ifndef SOOS_WINDOWS_CE
		len = MultiByteToWideChar(CP_UTF8, 0, utf8, len, unicode, len);
#else
		// Windows CE does not support UTF8
		len = MultiByteToWideChar(CP_ACP, 0, utf8, len, unicode, len);
#endif
	}
	else
	{
		// UTF-16
		memcpy(unicode, utf8, len);
		len /= 2;
	}

	unicode[len] = 0;
	LPCTSTR str = W2CT(unicode);
	len = _tcslen(str);
	value->set(str, len);
	free(unicode);
#endif
#ifdef SOOS_LINUX
	value->set(utf8, len);
#endif
	free(utf8);
	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalString(SOCmnStringEx* value)
{
	USES_CONVERSION;
	u_long len;
	LPSTR utf8;

	if (!UnMarshalArrayLen(&len))
	{
		return 0;
	}

	utf8 = (LPSTR)malloc(len);

	if (!utf8)
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!UnMarshalBlob(utf8, len))
	{
		free(utf8);
		return 0;
	}

#ifdef SOOS_WINDOWS
	LPWSTR unicode = (LPWSTR)malloc((len + 1) * sizeof(WCHAR));

	if (!unicode)
	{
		free(utf8);
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!(m_flags & TP_INIT_FLAG_USE_UTF16_STRINGS))
	{
		// UTF-8
#ifndef SOOS_WINDOWS_CE
		len = MultiByteToWideChar(CP_UTF8, 0, utf8, len, unicode, len);
#else
		// Windows CE does not support UTF8
		len = MultiByteToWideChar(CP_ACP, 0, utf8, len, unicode, len);
#endif
	}
	else
	{
		// UTF-16
		memcpy(unicode, utf8, len);
		len /= 2;
	}

	unicode[len] = 0;
	LPCTSTR str = W2CT(unicode);
	len = _tcslen(str);
	value->set(str, len);
	free(unicode);
#endif
#ifdef SOOS_LINUX
	value->set(utf8, len);
#endif
	free(utf8);
	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalBSTR(BSTR* value)
{
	USES_CONVERSION;
	u_long len;
	LPSTR utf8;
	*value = NULL;

	if (!UnMarshalArrayLen(&len))
	{
		return 0;
	}

	utf8 = (LPSTR)malloc(len);

	if (!utf8)
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!UnMarshalBlob(utf8, len))
	{
		free(utf8);
		return 0;
	}

#ifdef SOOS_WINDOWS
	LPWSTR unicode = (LPWSTR)malloc(len * sizeof(WCHAR));

	if (!unicode)
	{
		free(utf8);
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	if (!(m_flags & TP_INIT_FLAG_USE_UTF16_STRINGS))
	{
		// UTF-8
#ifndef SOOS_WINDOWS_CE
		len = MultiByteToWideChar(CP_UTF8, 0, utf8, len, unicode, len);
#else
		// Windows CE does not support UTF8
		len = MultiByteToWideChar(CP_ACP, 0, utf8, len, unicode, len);
#endif
	}
	else
	{
		// UTF-16
		memcpy(unicode, utf8, len);
		len /= 2;
	}

	*value = SysAllocStringLen(unicode, len);
	free(unicode);
#endif
#ifdef SOOS_LINUX
	*value = SysAllocStringLen(utf8, len);
#endif
	free(utf8);
	return 1;
}

int SOCmnTP_UnMarshaler::UnMarshalDWORDList(SOCmnDWORDList* value)
{
	SOCmnDWORDList list(*value);
	u_long count, n, elem;
	int ok = 1;

	if (!UnMarshalArrayLen(&count))
	{
		return 0;
	}

	list.removeAll();

	for (n = 0; n < count; n++)
	{
		ok &= UnMarshalLong(&elem);

		if (ok)
		{
			list.add(elem);
		}
	}

	return ok;
}

int SOCmnTP_UnMarshaler::UnMarshalVariant(LPVARIANT value)
{
	SOCmnVariant::variantClear(value);

	if (!UnMarshalShort(&V_VT(value)))
	{
		return 0;
	}

	if (V_ISARRAY(value))
	{
		return UnMarshalSafeArray(&V_ARRAY(value));
	}
	else switch (V_VT(value))
		{
		case VT_EMPTY:
			return 1;

		case VT_NULL:
			return 1;

		case VT_BOOL:
			return UnMarshalShort(&V_BOOL(value));

		case VT_UI1:
			return UnMarshalChar(&V_UI1(value));

		case VT_I1:
			return UnMarshalChar(&V_I1(value));

		case VT_I2:
			return UnMarshalShort(&V_I2(value));

		case VT_UI2:
			return UnMarshalShort(&V_UI2(value));

		case VT_I4:
			return UnMarshalLong(&V_I4(value));

		case VT_UI4:
			return UnMarshalLong(&V_UI4(value));

		case VT_INT:
			return UnMarshalLong(&V_INT(value));

		case VT_UINT:
			return UnMarshalLong(&V_UINT(value));

		case VT_I8:
			return UnMarshalQuad(&V_I8(value));

		case VT_UI8:
			return UnMarshalQuad(&V_UI8(value));

		case VT_CY:
			return UnMarshalQuad(&V_CY(value).int64);

		case VT_R4:
			return UnMarshalFloat(&V_R4(value));

		case VT_R8:
			return UnMarshalDouble(&V_R8(value));

		case VT_DATE:
			return UnMarshalDate(&V_DATE(value));

		case VT_BSTR:
			return UnMarshalBSTR(&V_BSTR(value));
		}

	return 0;
}

int SOCmnTP_UnMarshaler::UnMarshalSafeArray(LPSAFEARRAY* value)
{
	int         ok;
	USHORT      cDims, i;
	VARTYPE     vt;
	ULONG       nElements;
	SAFEARRAYBOUND* rgsabound;
	*value = NULL;

	if (!UnMarshalShort(&vt))
	{
		return 0;
	}

	if (vt == VT_EMPTY)
	{
		return 1;
	}

	ok = UnMarshalShort(&cDims);

	if (!ok || cDims == 0 || cDims > GetMarshalDataLen())
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	rgsabound = (SAFEARRAYBOUND*)malloc(sizeof(SAFEARRAYBOUND) * cDims);

	if (!rgsabound)
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	for (i = 0, nElements = 1; i < cDims; i++)
	{
		ok &= UnMarshalLong(&rgsabound[i].cElements);
		ok &= UnMarshalLong(&rgsabound[i].lLbound);
		nElements *= rgsabound[i].cElements;
	}

	if (!ok || nElements > GetMarshalDataLen())
	{
		free(rgsabound);
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	*value = SOCmnVariant::safeArrayCreate(vt, cDims, rgsabound);
	free(rgsabound);

	if (!*value)
	{
		SetReadPos(GetMarshalDataLen());
		return 0;
	}

	switch (vt)
	{
	case VT_UI1:
	case VT_I1:
	{
		BYTE* data;
		SafeArrayAccessData(*value, (void**)&data);
		ok &= UnMarshalBlob(data, nElements);
		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_BOOL:
	case VT_I2:
	case VT_UI2:
	{
		WORD* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalShort(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_I4:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
	{
		DWORD* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalLong(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_I8:
	case VT_UI8:
	case VT_CY:
	{
		ULONGLONG* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalQuad(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_R4:
	{
		FLOAT* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalFloat(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_R8:
	{
		DOUBLE* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalDouble(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_DATE:
	{
		DATE* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalDate(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_BSTR:
	{
		BSTR* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalBSTR(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	case VT_VARIANT:
	{
		VARIANT* data;
		SafeArrayAccessData(*value, (void**)&data);

		for (ULONG k = 0; k < nElements; k++)
		{
			ok &= UnMarshalVariant(&data[k]);
		}

		SafeArrayUnaccessData(*value);
	}
	break;

	default:
		return 0;
	}

	return ok;
}


//-----------------------------------------------------------------------------
// SOCmnTPCredentials
//-----------------------------------------------------------------------------

SOCmnTPCredentials::SOCmnTPCredentials(SOCmnString strUserName, const BYTE* pbyPasswordData, u_short unPasswordLength)
	: m_unType(TP_CREDENTIALS_TYPE_USERNAME_PWDHASH),
	  m_strUserName(strUserName),
	  m_pbyPasswordData(NULL),
	  m_unPasswordLength(unPasswordLength)
{
	m_pbyPasswordData = new BYTE[unPasswordLength];
	memcpy(m_pbyPasswordData, pbyPasswordData, unPasswordLength);
}

SOCmnTPCredentials::SOCmnTPCredentials(SOCmnString strUserName, SOCmnString strPasswordBinHex)
	: m_unType(TP_CREDENTIALS_TYPE_NONE),
	  m_strUserName(strUserName),
	  m_pbyPasswordData(NULL),
	  m_unPasswordLength(0)
{
	m_pbyPasswordData = binHexToBinary(strPasswordBinHex, m_unPasswordLength);

	if (m_pbyPasswordData != NULL)
	{
		m_unType = TP_CREDENTIALS_TYPE_USERNAME_PWDHASH;
	}
}

SOCmnTPCredentials::~SOCmnTPCredentials()
{
	delete[] m_pbyPasswordData;
}

u_short SOCmnTPCredentials::getType()
{
	return (m_unType);
}

SOCmnString SOCmnTPCredentials::getUserName()
{
	return (m_strUserName);
}

u_short SOCmnTPCredentials::getPasswordLength()
{
	return (m_unPasswordLength);
}

BYTE* SOCmnTPCredentials::getPasswordData()
{
	return (m_pbyPasswordData);
}

bool SOCmnTPCredentials::check(SOCmnTPCredentials* pCredentials)
{
	if (pCredentials == NULL)
	{
		return (false);
	}

	if (m_unType != pCredentials->getType())
	{
		return (false);
	}

	if (m_strUserName != pCredentials->m_strUserName)
	{
		return (false);
	}

	if (m_unPasswordLength != pCredentials->m_unPasswordLength)
	{
		return (false);
	}

	if (memcmp(m_pbyPasswordData, pCredentials->m_pbyPasswordData, m_unPasswordLength) != 0)
	{
		return (false);
	}

	return (true);
}

BYTE* SOCmnTPCredentials::binHexToBinary(SOCmnString strBinHex, u_short& unCount)
{
	DWORD dwLength;
	BYTE* pbyData;
	u_short i;
	dwLength = strBinHex.getLength();

	if (dwLength % 2 != 0)
	{
		return (NULL);
	}

	if (dwLength / 2 > USHRT_MAX)
	{
		return (NULL);
	}

	unCount = (u_short)(dwLength / 2);
	pbyData = new BYTE[unCount];

	for (i = 0; i < unCount; ++i)
	{
		SOCmnString strByte;
		LPTSTR pszEnd = NULL;
		strByte = strBinHex.mid(2 * i, 2);
		pbyData[i] = (BYTE)_tcstoul(strByte, &pszEnd, 16);

		if (pszEnd != NULL && *pszEnd != _T('\0'))
		{
			delete[] pbyData;
			return (NULL);
		};
	};

	return (pbyData);
}


//-----------------------------------------------------------------------------
// SOCmnTPConnection                                                          |
//-----------------------------------------------------------------------------

SOCmnTPConnection::SOCmnTPConnection(void)
{
	m_init = false;
	m_invokeIdLoop = 0;
	m_flags = 0;
	m_callArray.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD
					   | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED, SOCMNTPCONNECTION_MAX_CALLS);
}

SOCmnTPConnection::~SOCmnTPConnection()
{
}

bool SOCmnTPConnection::init(SOCmnSocket* pSrv, SOCmnObject* pSocket)
{
	m_init = false;
	m_unmarshal.release();
	m_srv = pSrv;
	m_srv.addRef();
	m_sock = pSocket;
	m_sock.addRef();
	return true;
}

void SOCmnTPConnection::handleClose(void)
{
	m_init = false;
	m_sync.lock();
	m_sock.release();
	m_sync.unlock();
	SOCmnList<SOCmnTPReceiver> recList(getReceiverList());

	if (recList.isInit())
	{
		SOCmnListPosition pos;
		SOCmnTPReceiver* pRec = NULL;
		pos = recList.getStartPosition();

		while (pos)
		{
			pRec = recList.getNext(pos);

			if (pRec != NULL)
			{
				pRec->addRef();
				recList.setDirty(FALSE);
				recList.autoUnlock();
				pRec->closeConnection(this);
				pRec->release();
				recList.autoLock();

				if (recList.isDirty())
				{
					pos = recList.getStartPosition();
				}
			}
		}
		recList.autoUnlock();
	}

	m_callArray.lock();
	SOCmnListPosition pos = m_callArray.getStartPosition();

	while (pos != NULL)
	{
		SOCmnTPCall* call = m_callArray.getNext(pos);
		call->setResult(E_FAIL);
		call->getCompleteEvent()->signal();
	}

	m_callArray.removeAll();
	m_callArray.unlock();
}

void SOCmnTPConnection::rcvData(IN LONG size, IN BYTE* pData)
{
	u_long msg_len, rcv_len;
	long rest;
	SOCmnPointer<SOCmnObject> sock = getSocket();
more_follows:

	if (!m_unmarshal)
	{
		m_unmarshal = new SOCmnTP_UnMarshaler(m_flags);
	}

	if (!m_unmarshal)
	{
		goto out_of_memory;
	}

	/* first try to receive only the
	 * first 4 bytes message_len.
	 * if the message len is known,
	 * receive the rest of the message.
	 * if there is more than one message
	 * concatenated, handle the next message
	 * afterwards.
	 */
	m_unmarshal->SetReadPos(0);

	if (!m_unmarshal->UnMarshalLong(&msg_len) || msg_len < TP_IDLE_MESSAGE_LEN)
	{
		msg_len = TP_IDLE_MESSAGE_LEN;
	}

	rcv_len = msg_len - m_unmarshal->GetMarshalDataLen();
	rest = size - rcv_len;

	if (rest < 0)
	{
		rcv_len = size;
	}

	if (!m_unmarshal->AppendMarshalData(pData, rcv_len))
	{
		goto out_of_memory;
	}

	if (msg_len == TP_IDLE_MESSAGE_LEN)
	{
		m_unmarshal->SetReadPos(0);

		if (m_unmarshal->UnMarshalLong(&msg_len) && msg_len > TP_IDLE_MESSAGE_LEN)
		{
			if (rest > 0)
			{
				pData += rcv_len;
				size = rest;
				goto more_follows;
			}

			return;
		}
	}

	if (rest < 0)
	{
		// check some magic values immediately if possible
		if (!m_init)
		{
			u_long magic_value;
			m_unmarshal->SetReadPos(0);

			if (m_unmarshal->UnMarshalLong(&magic_value) && magic_value > 0xFFFF)
			{
				goto protocol_error;
			}

			if (m_unmarshal->UnMarshalLong(&magic_value) && magic_value != TP_MAGIC_VALUE_TPV1 && magic_value != TP_MAGIC_VALUE_ADD_ACCEPT)
			{
				goto protocol_error;
			}
		}

		return;
	}

	if (!m_init)
	{
		// handle init message
		if (!handleInitMessage())
		{
			goto protocol_error;
		}
	}
	else
	{
		if (msg_len < sizeof(TP_BASIC_MESSAGE_HEADER))
		{
			// handle idle message
			if (msg_len != TP_IDLE_MESSAGE_LEN)
			{
				goto protocol_error;
			}

			// pong: send this back to the client!
			if (!handlePingMessage())
			{
				goto protocol_error;
			}
		}
		else
		{
			// handle basic message
			if (!handleBasicMessage())
			{
				goto protocol_error;
			}
		}
	}

	m_unmarshal.release();

	if (rest > 0)
	{
		pData += rcv_len;
		size = rest;
		goto more_follows;
	}

	return;
out_of_memory:
protocol_error:
	m_srv->removeClient(sock, SOCMNSOCKET_REQ_ABORTIVE_CLOSE);
	return;
}

bool SOCmnTPConnection::handlePingMessage(void)
{
	return false;
}

bool SOCmnTPConnection::handleBasicMessage(void)
{
	TP_BASIC_MESSAGE_HEADER basic;
	BOOL ok;
	m_unmarshal->SetReadPos(0);
	ok = m_unmarshal->UnMarshalLong(&basic.message_len);
	ok &= m_unmarshal->UnMarshalLong(&basic.invoke_id);
	ok &= m_unmarshal->UnMarshalLong(&basic.object_id);
	ok &= m_unmarshal->UnMarshalShort(&basic.interface_id);
	ok &= m_unmarshal->UnMarshalChar(&basic.function_id);
	ok &= m_unmarshal->UnMarshalChar(&basic.flags);

	if (!ok)
	{
		return false;
	}

	if (basic.flags & TP_MSG_FLAG_FUNCTION_RETURN)
	{
		// received response
		// get request of response
		SOCmnPointer<SOCmnTPCall> call;
		m_callArray.lock();
		m_callArray.findKey(basic.invoke_id, &call);
		m_callArray.unlock();

		// check invoke id
		if (!call || call->getInvokeId() != basic.invoke_id)
		{
			// received response for already completed call -> ignore it
			return true;
		}

		// copy response and signal call
		call->setRcvMsg(m_unmarshal);
		m_unmarshal.release();
		call->getCompleteEvent()->signal();
		return true;
	}
	else
	{
		// received request
		SOCmnPointer<SOCmnTPCall> call;
		SOCmnPointer<SOCmnTP_UnMarshaler> request;
		SOCMN_ALLOCATE_OBJECT_C(call, SOCmnTPCall(this), (SOCmnTPCall*));
		// copy request
		call->setInvokeId(basic.invoke_id);
		call->setObjectId(basic.object_id);
		call->setRcvMsg(m_unmarshal);
		m_unmarshal.release();
		addRequestToWorkQueue(call, basic.interface_id, basic.function_id);
		return true;
	}
}


bool SOCmnTPConnection::handleRequest(SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
{
	SOCmnList<SOCmnTPReceiver> recList(getReceiverList());
	LONG result = E_NOTIMPL;

	if (recList.isInit())
	{
		SOCmnListPosition pos;
		SOCmnTPReceiver* pRec;
		pos = recList.getStartPosition();

		while (pos)
		{
			pRec = recList.getNext(pos);
			pRec->addRef();
			recList.setDirty(FALSE);
			recList.autoUnlock();
			result = pRec->handleRequest(pCall, interfaceId, functionId);

			if (S_OK == result)
			{
				pRec->release();
				return true;
			}

			pRec->release();
			recList.autoLock();

			if (recList.isDirty())
			{
				pos = recList.getStartPosition();
			}
		}

		recList.autoUnlock();
	}

	// build neg. response
	sendReply(pCall, interfaceId, functionId, result);
	return false;
}

SOCmnList<SOCmnTPReceiver> *SOCmnTPConnection::getReceiverList(void)
{
	return NULL;
}

HRESULT SOCmnTPConnection::invoke(SOCmnTPCall* pCall, BOOL waitForReply)
{
	SOCmnPointer<SOCmnTP_Marshaler> request = pCall->getSndMsg();
	u_long msg_len = request->GetMarshalDataLen();
	u_char* pReqData;
	DWORD invokeId;
	HRESULT res;
	_ASSERTION(msg_len >= sizeof(TP_BASIC_MESSAGE_HEADER), "message too short");
	request->SetWritePos(0);
	request->MarshalLong(msg_len);
	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (!sock || !m_srv)
	{
		return E_FAIL;
	}

	// call management
	m_callArray.lock();
	invokeId = getNewInvokeId();
	pCall->setInvokeId(invokeId);
	m_callArray.add(pCall, invokeId);
	m_callArray.unlock();
	request->MarshalLong(invokeId);
	// send request
	pReqData = request->GetMarshalData();

	if (!pReqData || m_srv->sndData(sock, msg_len, pReqData) == SOCKET_ERROR)
	{
		res = E_FAIL;
	}
	else if (!waitForReply)
	{
		res = S_OK;
	}
	else

		// wait for response
		if (!pCall->getCompleteEvent()->waitFor(pCall->getTimeout()))
		{
			res = OPC_E_TIMEDOUT;
		}
		else
		{
			res = pCall->getResult();
		}

	// remove call from array
	m_callArray.lock();
	m_callArray.removeKey(invokeId);
	m_callArray.unlock();

	//  TODO POC : check how the pCall->getResult is generated;
	if (FAILED(res))
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE, (_T("invoke failed with: [0x%X]"), res))
	} // end if

	return res;
}

DWORD SOCmnTPConnection::getNewInvokeId(void)
{
	while (TRUE)
	{
		SOCmnTPCall* pCall;
		DWORD nextInvokeId = m_invokeIdLoop++;

		if ((pCall = m_callArray.findKey(nextInvokeId)) == NULL)
		{
			return nextInvokeId;
		}

		pCall->release();
	}
}

void SOCmnTPConnection::reply(SOCmnTPCall* pCall)
{
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	u_long msg_len = response->GetMarshalDataLen();
	_ASSERTION(msg_len >= sizeof(TP_BASIC_MESSAGE_HEADER), "message too short");
	response->SetWritePos(0);
	response->MarshalLong(msg_len);
	response->MarshalLong(pCall->getInvokeId());
	response->MarshalLong(pCall->getObjectId());
	// send response
	u_char* pResData = response->GetMarshalData();
	SOCmnPointer<SOCmnObject> sock = getSocket();

	if (!pResData || !m_srv || !sock)
	{
		return;
	}

	if (m_srv->sndData(sock, msg_len, pResData) == SOCKET_ERROR)
	{
		return;
	}
}

void SOCmnTPConnection::sendReply(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId,
	IN HRESULT res)
{
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTPConnection> tpCon = (SOCmnTPConnection*)pCall->getTPConnection();
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + sizeof(HRESULT));
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // object id
	response->MarshalShort(interfaceId);                          // interface id
	response->MarshalChar(functionId);                            // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);           // flags
	response->MarshalLong(res);                                   // result
	tpCon->reply(pCall);
}

void SOCmnTPConnection::marshalInitialMessageHeader(SOCmnTP_Marshaler* pMarshaler, const TP_INITIAL_MESSAGE_HEADER& header)
{
	pMarshaler->MarshalLong(header.message_len);
	pMarshaler->MarshalLong(header.magic_value);
	pMarshaler->MarshalShort(header.protocol_version);
	pMarshaler->MarshalShort(header.flags);
	pMarshaler->MarshalLong(header.reserved);
}

bool SOCmnTPConnection::unmarshalInitialMessageHeader(SOCmnTP_UnMarshaler* pUnmarshaler, TP_INITIAL_MESSAGE_HEADER& header)
{
	if (!pUnmarshaler->UnMarshalLong(&header.message_len))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&header.magic_value))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalShort(&header.protocol_version))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalShort(&header.flags))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&header.reserved))
	{
		return (false);
	}

	return (true);
}

void SOCmnTPConnection::marshalSessionId(SOCmnTP_Marshaler* pMarshaler, const TP_SESSION_ID& sessionId)
{
	pMarshaler->MarshalLong(sessionId.server_id[0]);
	pMarshaler->MarshalLong(sessionId.server_id[1]);
	pMarshaler->MarshalLong(sessionId.server_id[2]);
	pMarshaler->MarshalLong(sessionId.server_id[3]);
}

bool SOCmnTPConnection::unmarshalSessionId(SOCmnTP_UnMarshaler* pUnmarshaler, TP_SESSION_ID& sessionId)
{
	if (!pUnmarshaler->UnMarshalLong(&sessionId.server_id[0]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionId.server_id[1]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionId.server_id[2]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionId.server_id[3]))
	{
		return (false);
	}

	return (true);
}

void SOCmnTPConnection::marshalSessionContext(SOCmnTP_Marshaler* pMarshaler, const TP_SESSION_CONTEXT& sessionContext)
{
	pMarshaler->MarshalLong(sessionContext.session_id.server_id[0]);
	pMarshaler->MarshalLong(sessionContext.session_id.server_id[1]);
	pMarshaler->MarshalLong(sessionContext.session_id.server_id[2]);
	pMarshaler->MarshalLong(sessionContext.session_id.server_id[3]);
}

bool SOCmnTPConnection::unmarshalSessionContext(SOCmnTP_UnMarshaler* pUnmarshaler, TP_SESSION_CONTEXT& sessionContext)
{
	if (!pUnmarshaler->UnMarshalLong(&sessionContext.session_id.server_id[0]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionContext.session_id.server_id[1]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionContext.session_id.server_id[2]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionContext.session_id.server_id[3]))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalLong(&sessionContext.context_id))
	{
		return (false);
	}

	return (true);
}

void SOCmnTPConnection::marshalCredentials(SOCmnTP_Marshaler* pMarshaler, SOCmnTPCredentials* pCredentials)
{
	u_short unType;

	if (pCredentials == NULL)
	{
		pMarshaler->MarshalShort(TP_CREDENTIALS_TYPE_NONE);
		return;
	};

	unType = pCredentials->getType();

	pMarshaler->MarshalShort(unType);

	if (unType == TP_CREDENTIALS_TYPE_USERNAME_PWDHASH)
	{
		u_short unPasswordLength;
		pMarshaler->MarshalString((LPCTSTR)pCredentials->getUserName());
		unPasswordLength = pCredentials->getPasswordLength();
		pMarshaler->MarshalShort(unPasswordLength);
		pMarshaler->MarshalBlob(pCredentials->getPasswordData(), unPasswordLength);
		return;
	};

	_ASSERTION(false, _T("Invalid credentials type"));
}

bool SOCmnTPConnection::unmarshalCredentials(SOCmnTP_UnMarshaler* pUnmarshaler, SOCmnPointer<SOCmnTPCredentials> &pCredentials)
{
	u_short unType;
	SOCmnString strUserName;
	u_short unPasswordLength;
	BYTE* pbyPasswordData;

	if (!pUnmarshaler->UnMarshalShort(&unType))
	{
		return (false);
	}

	if (unType == TP_CREDENTIALS_TYPE_NONE)
	{
		pCredentials = NULL;
		return (true);
	};

	if (unType != TP_CREDENTIALS_TYPE_USERNAME_PWDHASH)
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalString(&strUserName))
	{
		return (false);
	}

	if (!pUnmarshaler->UnMarshalShort(&unPasswordLength))
	{
		return (false);
	}

	pbyPasswordData = new BYTE[unPasswordLength];

	if (!pUnmarshaler->UnMarshalBlob(pbyPasswordData, unPasswordLength))
	{
		delete[] pbyPasswordData;
		return (false);
	};

	pCredentials = new SOCmnTPCredentials(strUserName, pbyPasswordData, unPasswordLength);

	delete[] pbyPasswordData;

	return (true);
}



//-----------------------------------------------------------------------------
// SOCmnTPCall                                                                |
//-----------------------------------------------------------------------------

SOCmnTPCall::SOCmnTPCall(SOCmnTPConnection* pCon)
{
	SOCMN_ALLOCATE_OBJECT_C(m_sndMsg, SOCmnTP_Marshaler(pCon->getFlags()), (SOCmnTP_Marshaler*));
	SOCMN_ALLOCATE_OBJECT_C(m_rcvMsg, SOCmnTP_UnMarshaler(pCon->getFlags()), (SOCmnTP_UnMarshaler*));
	m_timeout = SOCMN_TIME_40DAYS;
	m_activationTime = 0;
	m_completeEvent.create();
	m_result = S_OK;
	m_tpConnection = pCon;
	m_tpConnection.addRef();
}

SOCmnTPCall::~SOCmnTPCall(void)
{
	m_sndMsg.release();
	m_rcvMsg.release();
	m_tpConnection.release();
}



//-----------------------------------------------------------------------------
// SOCmnTPReceiver                                                            |
//-----------------------------------------------------------------------------

SOCmnTPReceiver::SOCmnTPReceiver(void)
{
}

SOCmnTPReceiver::~SOCmnTPReceiver(void)
{
}

//-----------------------------------------------------------------------------
// handleRequest
//
// - processing of TP request
//
// returns:
//      TRUE  - receiver has processed or is processing the request
//      FALSE - unknown request for this receiver
//
LONG SOCmnTPReceiver::handleRequest(
	IN SOCmnTPCall* /* pCall */,
	WORD /* interfaceId */,
	BYTE /* functionId */)
{
	return E_NOTIMPL;
}

void SOCmnTPReceiver::closeConnection(IN SOCmnTPConnection* /* pCon */)
{}

#endif // SOFEATURE_TUNNEL
