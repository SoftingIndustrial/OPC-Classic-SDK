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
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : SOClient.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : application related classes                                 |
//                - SODmCApp: MFC application class                           |
//                - SODmCCreatorDA: Toolkit creator for OPC DA objects        |
//                - SODmCCreatorAE: Toolkit creator for OPC AE objects        |
//                - SODmCServerBrowser: Server browser class of demo client   |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "opcda.h"
#include "afxadv.h"
#include "SOVersion.h"
#include "resource.h"
#include "SOClient.h"
#include "SODmCMainFrm.h"
#include "SODmCMainView.h"
#include "SODmCObjectView.h"
#include "SODmCServerDA.h"
#include "SODmCServerAE.h"
#include "SODmCSubscription.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCEvent.h"
#include "ConfigFile.h"

//-----------------------------------------------------------------------------
// Global functions                                                           |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getInstanceHandle
//
// - get the instance handle of the application
//
// returns:
//		instance handle
//
HINSTANCE getInstanceHandle(void)
{
	return AfxGetApp()->m_hInstance;
} // getInstanceHandle


//-----------------------------------------------------------------------------
// getComCtlVersion
//
// - get the version of the COMCTL32.DLL
//
// returns:
//		version
//
struct SODMC_DLLVERSIONINFO
{
	DWORD cbSize;
	DWORD dwMajorVersion;   // Major version
	DWORD dwMinorVersion;   // Minor version
	DWORD dwBuildNumber;    // Build number
	DWORD dwPlatformID;     // DLLVER_PLATFORM_*
};
typedef HRESULT(CALLBACK* SODMC_DLLGETVERSIONPROC)(SODMC_DLLVERSIONINFO*);
DWORD getComCtlVersion(void)
{
	DWORD s_comCtlVersion = 0;

	// return cached version if already determined...
	if (s_comCtlVersion != 0)
	{
		return s_comCtlVersion;
	}

	// otherwise determine comctl32.dll version via DllGetVersion
	HINSTANCE hInst = ::GetModuleHandleA("COMCTL32.DLL");

	if (hInst != NULL)
	{
		SODMC_DLLGETVERSIONPROC pfn;
		pfn = (SODMC_DLLGETVERSIONPROC)GetProcAddress(hInst, "DllGetVersion");

		if (pfn != NULL)
		{
			SODMC_DLLVERSIONINFO dvi;
			memset(&dvi, 0, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			HRESULT hr = (*pfn)(&dvi);

			if (SUCCEEDED(hr))
			{
				ASSERT(dvi.dwMajorVersion <= 0xFFFF);
				ASSERT(dvi.dwMinorVersion <= 0xFFFF);
				s_comCtlVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
			}
		}
	}

	return s_comCtlVersion;
} // getComCtlVersion


//-----------------------------------------------------------------------------
// SODmCTreeInsert
//
// - insert a item into a GUI tree
//
// returns:
//		tree item handle
//
HTREEITEM SODmCTreeInsert(
	IN CTreeCtrl* tree,        // tree control
	IN HTREEITEM hParent,      // tree item handle of parent
	IN LPCTSTR pszText,        // item text
	IN BOOL cChildren,         // has children ?
	IN void* lParam,           // 32bit parameter
	IN int iImage,             // item image
	IN int iSelectedImage,     // item selection image
	IN HTREEITEM hInsertAfter) // insert after this item
{
	TV_INSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.pszText = (LPTSTR)pszText;
	tvis.item.cChildren = cChildren;
	tvis.item.iImage = iImage;
	tvis.item.iSelectedImage = iSelectedImage;
	tvis.item.lParam = (long)lParam;
	return tree->InsertItem(&tvis);
} // SODmCTreeInsert


//-----------------------------------------------------------------------------
// SODmCListInsert
//
// - insert a item a the tail of a GUI list
//
// returns:
//		list item index
//
int SODmCListInsert(
	IN CListCtrl* list, // list control
	IN LPCTSTR pszText, // item text
	IN void* lParam,    // 32bit parameter
	IN int iImage)      // item image
{
	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lv.iItem = list->GetItemCount();
	lv.iSubItem = 0;
	lv.pszText = (LPTSTR)pszText;
	lv.lParam = (LPARAM)lParam;
	lv.iImage = iImage;
	return list->InsertItem(&lv);
} // SODmCListInsert


//-----------------------------------------------------------------------------
// SODmCListInsert
//
// - insert a item a the head of a GUI list
//
// returns:
//		list item index
//
int SODmCListPush(
	IN CListCtrl* list, // list control
	IN LPCTSTR pszText, // item text
	IN void* lParam,    // 32bit parameter
	IN int iImage)      // item image
{
	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lv.iItem = 0;
	lv.iSubItem = 0;
	lv.pszText = (LPTSTR)pszText;
	lv.lParam = (LPARAM)lParam;
	lv.iImage = iImage;
	return list->InsertItem(&lv);
} // SODmCListPush


//-----------------------------------------------------------------------------
// LCID2string
//
// - convert LCID to string
//
void LCID2string(
	IN LCID lcid,        // LCID to convert
	OUT CString& string) // result string
{
	switch (lcid)
	{
	case 0x0000:
		string = _T("Neutral");
		break;

	case 0x0400:
		string = _T("User Default");
		break;

	case 0x0800:
		string = _T("System Default");
		break;

	case 0x0407:
		string = _T("German");
		break;

	case 0x0807:
		string = _T("German (Swiss)");
		break;

	case 0x0C07:
		string = _T("German (Austria)");
		break;

	case 0x0409:
		string = _T("English (USA)");
		break;

	case 0x0809:
		string = _T("English (UK)");
		break;

	case 0x0019:
		string = _T("Russian");
		break;

	case 0x000E:
		string = _T("Hungarian");
		break;

	case 0x041D:
		string = _T("Swedish");
		break;

	default:
		string.Format(_T("0x%4.4X"), lcid);
		break;
	}
} // LCID2string


//-----------------------------------------------------------------------------
// string2LCID
//
// - convert string to LCID
//
LCID string2LCID(
	IN CString string) // string
{
	if (string == _T("Neutral"))
	{
		return 0x0000;
	}
	else if (string == _T("User Default"))
	{
		return 0x0400;
	}
	else if (string == _T("System Default"))
	{
		return 0x0800;
	}
	else if (string == _T("German"))
	{
		return 0x0407;
	}
	else if (string == _T("German (Swiss)"))
	{
		return 0x0807;
	}
	else if (string == _T("German (Austria)"))
	{
		return 0x0C07;
	}
	else if (string == _T("English (USA)"))
	{
		return 0x0409;
	}
	else if (string == _T("English (UK)"))
	{
		return 0x0809;
	}
	else if (string == _T("Russian"))
	{
		return 0x0019;
	}
	else if (string == _T("Hungarian"))
	{
		return 0x000E;
	}
	else if (string == _T("Swedish"))
	{
		return 0x041D;
	}
	else
	{
		return _tcstoul(string, NULL, 0);
	}
} // string2LCID


//-----------------------------------------------------------------------------
// string2vartype
//
// - convert string to vartype
//
// returns:
//		vartype
//
VARTYPE string2vartype(
	IN CString datatype)    // datatype string
{
	VARTYPE vt;

	if (datatype == _T("BSTR"))
	{
		vt = VT_BSTR;
	}
	else if (datatype == _T("BOOL"))
	{
		vt = VT_BOOL;
	}
	else if (datatype == _T("UI1"))
	{
		vt = VT_UI1;
	}
	else if (datatype == _T("I1"))
	{
		vt = VT_I1;
	}
	else if (datatype == _T("UI2"))
	{
		vt = VT_UI2;
	}
	else if (datatype == _T("I2"))
	{
		vt = VT_I2;
	}
	else if (datatype == _T("UI4"))
	{
		vt = VT_UI4;
	}
	else if (datatype == _T("I4"))
	{
		vt = VT_I4;
	}
	else if (datatype == _T("UI8"))
	{
		vt = VT_UI8;
	}
	else if (datatype == _T("I8"))
	{
		vt = VT_I8;
	}
	else if (datatype == _T("UINT"))
	{
		vt = VT_UINT;
	}
	else if (datatype == _T("INT"))
	{
		vt = VT_INT;
	}
	else if (datatype == _T("R4"))
	{
		vt = VT_R4;
	}
	else if (datatype == _T("R8"))
	{
		vt = VT_R8;
	}
	else if (datatype == _T("DATE"))
	{
		vt = VT_DATE;
	}
	else if (datatype == _T("CY"))
	{
		vt = VT_CY;
	}
	else if (datatype == _T("BSTR ARRAY"))
	{
		vt = VT_BSTR | VT_ARRAY;
	}
	else if (datatype == _T("BOOL ARRAY"))
	{
		vt = VT_BOOL | VT_ARRAY;
	}
	else if (datatype == _T("UI1 ARRAY"))
	{
		vt = VT_UI1 | VT_ARRAY;
	}
	else if (datatype == _T("I1 ARRAY"))
	{
		vt = VT_I1 | VT_ARRAY;
	}
	else if (datatype == _T("UI2 ARRAY"))
	{
		vt = VT_UI2 | VT_ARRAY;
	}
	else if (datatype == _T("I2 ARRAY"))
	{
		vt = VT_I2 | VT_ARRAY;
	}
	else if (datatype == _T("UI4 ARRAY"))
	{
		vt = VT_UI4 | VT_ARRAY;
	}
	else if (datatype == _T("I4 ARRAY"))
	{
		vt = VT_I4 | VT_ARRAY;
	}
	else if (datatype == _T("UI8 ARRAY"))
	{
		vt = VT_UI8 | VT_ARRAY;
	}
	else if (datatype == _T("I8 ARRAY"))
	{
		vt = VT_I8 | VT_ARRAY;
	}
	else if (datatype == _T("R4 ARRAY"))
	{
		vt = VT_R4 | VT_ARRAY;
	}
	else if (datatype == _T("R8 ARRAY"))
	{
		vt = VT_R8 | VT_ARRAY;
	}
	else if (datatype == _T("DATE ARRAY"))
	{
		vt = VT_DATE | VT_ARRAY;
	}
	else if (datatype == _T("CY ARRAY"))
	{
		vt = VT_CY | VT_ARRAY;
	}
	else if (datatype == _T("VARIANT ARRAY"))
	{
		vt = VT_VARIANT | VT_ARRAY;
	}
	else
	{
		vt = VT_EMPTY;
	}

	return vt;
} // string2vartype


//-----------------------------------------------------------------------------
// string2accessRights
//
// - convert string to OPC access rights
//
// returns:
//		OPC access rights
//
WORD string2accessRights(
	IN CString rights)  // access rights
{
	WORD ar;

	if (rights == _T("WRITE"))
	{
		ar = OPC_WRITEABLE;
	}
	else if (rights == _T("READ"))
	{
		ar = OPC_READABLE;
	}
	else if (rights == _T("READ and WRITE"))
	{
		ar = OPC_READABLE | OPC_WRITEABLE;
	}
	else
	{
		ar = 0;
	}

	return ar;
} // string2accessRights


// variant conversion
LPCTSTR VARIANT2STRING(IN SOCmnString* buffer, IN LPCVARIANT var, IN BOOL readFormat)
{
	if (getApp()->getOptions()->valueDec)
	{
		if ((getApp()->getOptions()->valuePure) || (!readFormat))
		{
			return ::variant2string(buffer, var, LOCALE_SYSTEM_DEFAULT, readFormat);
		}
		else
		{
			return ::variantWithType2string(buffer, var, LOCALE_SYSTEM_DEFAULT);
		}
	}
	else
	{
		if ((getApp()->getOptions()->valuePure) || (!readFormat))
		{
			return variant2stringHex(buffer, var, LOCALE_SYSTEM_DEFAULT, readFormat);
		}
		else
		{
			return variantWithType2stringHex(buffer, var, LOCALE_SYSTEM_DEFAULT);
		}
	}
}


//-----------------------------------------------------------------------------
// variant2stringHex
//
// - convert variant to string
//
// returns:7
//		pointer to string
//
LPCTSTR variant2stringHex(
	IN SOCmnString* buffer,     // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID,
	OPTIONAL IN BOOL readFormat)    // locale id
{
	switch (var->vt)
	{
	case VT_EMPTY:
		*buffer = _T("");
		break;

	case VT_I1:
	{
		unsigned long ulVal = (unsigned long)var->bVal & 0x7F;

		if (var->bVal & 0x80)
		{
			buffer->format(_T("-0x%2.2X"), (~ulVal & 0X7F) + 1);
		}
		else
		{
			buffer->format(_T("0x%2.2X"), ulVal);
		}
	}
	break;

	case VT_UI1:
	{
		unsigned long ulVal = (unsigned long)var->bVal & 0xFF;
		buffer->format(_T("0x%2.2X"), ulVal);
	}
	break;

	case VT_I2:
	{
		unsigned long ulVal = (unsigned long)var->iVal & 0x7FFF;

		if (var->iVal & 0x8000)
		{
			buffer->format(_T("-0x%4.4X"), (~ulVal & 0X7FFF) + 1);
		}
		else
		{
			buffer->format(_T("0x%4.4X"), ulVal);
		}
	}
	break;

	case VT_UI2:
	{
		unsigned long ulVal = (unsigned long)var->uiVal & 0xFFFF;
		buffer->format(_T("0x%4.4X"), ulVal);
	}
	break;

	case VT_INT:
	case VT_I4:
	{
		unsigned long ulVal = (unsigned long)var->lVal & 0x7FFFFFFF;

		if (var->lVal & 0x80000000)
		{
			buffer->format(_T("-0x%8.8X"), (~ulVal & 0X7FFFFFFF) + 1);
		}
		else
		{
			buffer->format(_T("0x%8.8X"), ulVal);
		}
	}
	break;

	case VT_UI4:
	case VT_UINT:
		buffer->format(_T("0x%8.8X"), var->ulVal);
		break;

	case VT_I8:
	{
		ULONGLONG ull = V_I8(var) & 0x7FFFFFFFFFFFFFFF;
		unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
		unsigned long hi = (unsigned long)(ull >> 32);

		if (V_I8(var) & 0x8000000000000000)
		{
			hi = (~hi & 0x7FFFFFFF);
			lo = (~lo & 0xFFFFFFFF);
			lo++;

			if (lo == 0)
			{
				hi++;
			}

			buffer->format(_T("-0x%8.8X%8.8X"), hi, lo);
		}
		else
		{
			buffer->format(_T("0x%8.8X%8.8X"), hi, lo);
		}
	}
	break;

	case VT_UI8:
	{
		ULONGLONG ull = V_UI8(var);
		unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
		unsigned long hi = (unsigned long)(ull >> 32);
		buffer->format(_T("0x%8.8X%8.8X"), hi, lo);
	}
	break;

	case VT_BOOL:
		buffer->format(_T("%s"), (var->boolVal) ? _T("-1") : _T("0"));
		break;

	case VT_BSTR:
	{
		USES_CONVERSION;
		buffer->format(_T("%-.90s"), OLE2CT(var->bstrVal));
	}
	break;

	case VT_DATE:
	{
#ifdef SOOS_WINDOWS
		SYSTEMTIME t;
		::VariantTimeToSystemTime(var->date, &t);
		buffer->format(_T("%2.2d:%2.2d:%2.2d"), t.wHour, t.wMinute, t.wSecond);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		struct tm t;
		localtime_r(&var->date.tv_sec, &t);
		buffer->format(_T("%2.2d:%2.2d:%2.2d"),
					   t.tm_hour, t.tm_min, t.tm_sec);
#endif
	}
	break;

	case VT_R4:
	case VT_R8:
	case VT_CY:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_INT):
	case (VT_ARRAY | VT_UINT):
	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I8):
	case (VT_ARRAY | VT_UI8):
	case (VT_ARRAY | VT_R4):
	case (VT_ARRAY | VT_R8):
	case (VT_ARRAY | VT_BOOL):
	case (VT_ARRAY | VT_BSTR):
	case (VT_ARRAY | VT_DATE):
	case (VT_ARRAY | VT_CY):
	case (VT_ARRAY | VT_VARIANT):
	{
		LONG lBound, uBound;
		LONG i;
		SOCmnString valStr;
		SafeArrayGetLBound(var->parray, 1, &lBound);
		SafeArrayGetUBound(var->parray, 1, &uBound);

		if (readFormat)
		{
			buffer->format(_T("[%ld,%ld] ("), lBound, uBound);
		}
		else
		{
			buffer->empty();
		}

		for (i = lBound; i <= uBound; i++)
		{
			if (buffer->getLength() > 250)
			{
				*buffer += _T("...");
				break;
			}

			switch (var->vt & ~VT_ARRAY)
			{
			case VT_I2:
			{
				short iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				unsigned long ulVal = (unsigned long)iVal & 0x7FFF;

				if (iVal & 0x8000)
				{
					valStr.format(_T("-0x%4.4X"), (~ulVal & 0X7FFF) + 1);
				}
				else
				{
					valStr.format(_T("0x%4.4X"), ulVal);
				}
			}
			break;

			case VT_UI2:
			{
				unsigned short uiVal;
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				ulVal = (unsigned long)uiVal & 0xFFFF;
				valStr.format(_T("0x%4.4X"), ulVal);
			}
			break;

			case VT_I4:
			case VT_INT:
			{
				long lVal;
				SafeArrayGetElement(var->parray, &i, &lVal);
				unsigned long ulVal = (unsigned long)var->lVal & 0x7FFFFFFF;

				if (lVal & 0x80000000)
				{
					valStr.format(_T("-0x%8.8X"), (~ulVal & 0X7FFFFFFF) + 1);
				}
				else
				{
					valStr.format(_T("0x%8.8X"), ulVal);
				}
			}

			case VT_UI4:
			case VT_UINT:
			{
				unsigned int uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("0x%8.8X"), uiVal);
			}
			break;

			case VT_I1:
			{
				char bVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				unsigned long ulVal = (unsigned long)(bVal & 0x7F);

				if (bVal & 0x80)
				{
					valStr.format(_T("-0x%2.2X"), (~ulVal & 0X7F) + 1);
				}
				else
				{
					valStr.format(_T("0x%2.2X"), ulVal);
				}
			}
			break;

			case VT_UI1:
			{
				unsigned char cVal;
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &cVal);
				ulVal = (unsigned long)cVal & 0xFF;
				valStr.format(_T("0x%2.2X"), ulVal);
			}
			break;

			case VT_I8:
			{
				LONGLONG llVal;
				SafeArrayGetElement(var->parray, &i, &llVal);
				ULONGLONG ull = llVal & 0x7FFFFFFFFFFFFFFF;
				unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
				unsigned long hi = (unsigned long)(ull >> 32);

				if (llVal & 0x8000000000000000)
				{
					hi = (~hi & 0x7FFFFFFF);
					lo = (~lo & 0xFFFFFFFF);
					lo++;

					if (lo == 0)
					{
						hi++;
					}

					valStr.format(_T("-0x%8.8X%8.8X"), hi, lo);
				}
				else
				{
					valStr.format(_T("0x%8.8X%8.8X"), hi, lo);
				}
			}
			break;

			case VT_UI8:
			{
				ULONGLONG ullVal;
				SafeArrayGetElement(var->parray, &i, &ullVal);
				ULONGLONG ull = ullVal;
				unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
				unsigned long hi = (unsigned long)(ull >> 32);
				valStr.format(_T("0x%8.8X%8.8X"), hi, lo);
			}
			break;

			case VT_R4:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.fltVal);
				bstr.vt = VT_R4;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_R8:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_R8;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BOOL:
			{
				VARIANT_BOOL boolVal;
				SafeArrayGetElement(var->parray, &i, &boolVal);
				valStr.format(_T("%s"), (boolVal) ? _T("-1") : _T("0"));
			}
			break;

			case VT_CY:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.cyVal);
				bstr.vt = VT_CY;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BSTR:
			{
				USES_CONVERSION;
				BSTR bstrVal = NULL;
				::SafeArrayGetElement(var->parray, &i, &bstrVal);
				valStr.format(_T("%-.90s"), OLE2CT(bstrVal));
				::SysFreeString(bstrVal);
			}
			break;

			case VT_DATE:
			{
#ifdef SOOS_WINDOWS
				double dblVal;
				SYSTEMTIME t;
				SafeArrayGetElement(var->parray, &i, &dblVal);
				::VariantTimeToSystemTime(dblVal, &t);
				valStr.format(_T("%2.2d:%2.2d:%2.2d"), t.wHour, t.wMinute, t.wSecond);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
				DATE dateVal;
				struct tm t;
				SafeArrayGetElement(var->parray, &i, &dateVal);
				localtime_r(&dateVal.tv_sec, &t);
				buffer->format(_T("%2.2d:%2.2d:%2.2d"),
							   t.tm_hour, t.tm_min, t.tm_sec);
#endif
			}
			break;

			case VT_VARIANT:
			{
				SOCmnVariant varV;
				SafeArrayGetElement(var->parray, &i, (void*)(LPCVARIANT)varV);
				variant2string(&valStr, &varV, localeID);
			}
			break;
			}

			*buffer += valStr;

			if (readFormat)
			{
				if (i + 1 <= uBound)
				{
					*buffer += _T(", ");
				}
				else
				{
					*buffer +=  _T(")");
				}
			}
			else
			{
				if (i + 1 <= uBound)
				{
					*buffer += _T("; ");
				}
			}
		}
	}
	break;

	default:
		buffer->format(_T("VARIANT vt; 0x%8.8X"), var->vt);
		break;
	}

	return (LPCTSTR) * buffer;
} // variant2stringHex


//-----------------------------------------------------------------------------
// variantWithType2stringHex
//
// - convert variant with vartype to string
//
// returns:
//		pointer to string
//
LPCTSTR variantWithType2stringHex(
	IN SOCmnString* buffer,     // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID)  // locale id
{
	SOCmnString type;

	switch (var->vt)
	{
	case VT_EMPTY:
		*buffer = _T("EMPTY");
		break;

	case VT_I1:
	{
		unsigned long ulVal = (unsigned long)var->bVal & 0x7F;

		if (var->bVal & 0x80)
		{
			buffer->format(_T("I1(-0x%2.2X)"), (~ulVal & 0X7F) + 1);
		}
		else
		{
			buffer->format(_T("I1(0x%2.2X)"), ulVal);
		}
	}
	break;

	case VT_UI1:
	{
		unsigned long ulVal = (unsigned long)var->bVal & 0xFF;
		buffer->format(_T("UI1(0x%2.2X)"), ulVal);
	}
	break;

	case VT_I2:
	{
		unsigned long ulVal = (unsigned long)var->iVal & 0x7FFF;

		if (var->iVal & 0x8000)
		{
			buffer->format(_T("I2(-0x%4.4X)"), (~ulVal & 0X7FFF) + 1);
		}
		else
		{
			buffer->format(_T("I2(0x%4.4X)"), ulVal);
		}
	}
	break;

	case VT_UI2:
	{
		unsigned long ulVal = (unsigned long)var->uiVal & 0xFFFF;
		buffer->format(_T("UI2(0x%4.4X)"), ulVal);
	}
	break;

	case VT_I4:
	{
		unsigned long ulVal = (unsigned long)var->lVal & 0x7FFFFFFF;

		if (var->lVal & 0x80000000)
		{
			buffer->format(_T("I4(-0x%8.8X)"), (~ulVal & 0X7FFFFFFF) + 1);
		}
		else
		{
			buffer->format(_T("I4(0x%8.8X)"), ulVal);
		}
	}
	break;

	case VT_UI4:
		buffer->format(_T("UI4(0x%8.8X)"), var->ulVal);
		break;

	case VT_INT:
	{
		unsigned long ulVal = (unsigned long)var->lVal & 0x7FFFFFFF;

		if (var->lVal & 0x80000000)
		{
			buffer->format(_T("INT(-0x%8.8X)"), (~ulVal & 0X7FFFFFFF) + 1);
		}
		else
		{
			buffer->format(_T("INT(0x%8.8X)"), ulVal);
		}
	}
	break;

	case VT_UINT:
		buffer->format(_T("UINT(0x%8.8X)"), var->ulVal);
		break;

	case VT_I8:
	{
		ULONGLONG ull = V_I8(var) & 0x7FFFFFFFFFFFFFFF;
		unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
		unsigned long hi = (unsigned long)(ull >> 32);

		if (V_I8(var) & 0x8000000000000000)
		{
			hi = (~hi & 0x7FFFFFFF);
			lo = (~lo & 0xFFFFFFFF);
			lo++;

			if (lo == 0)
			{
				hi++;
			}

			buffer->format(_T("I8(-0x%8.8X%8.8X)"), hi, lo);
		}
		else
		{
			buffer->format(_T("I8(0x%8.8X%8.8X)"), hi, lo);
		}
	}
	break;

	case VT_UI8:
	{
		ULONGLONG ull = V_UI8(var);
		unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
		unsigned long hi = (unsigned long)(ull >> 32);
		buffer->format(_T("UI8(0x%8.8X%8.8X)"), hi, lo);
	}
	break;

	case VT_R4:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("R4(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_R8:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("R8(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_BOOL:
		buffer->format(_T("BOOL(%s)"), (var->boolVal) ? _T("-1") : _T("0"));
		break;

	case VT_CY:
	{
		USES_CONVERSION;
		VARIANT bstr;
		::VariantInit(&bstr);
		SOCmnVariant::variantChangeTypeEx(&bstr, (LPVARIANT)var, localeID, 0, VT_BSTR);
		buffer->format(_T("CY(%-.90s)"), OLE2CT(bstr.bstrVal));
		SOCmnVariant::variantClear(&bstr);
	}
	break;

	case VT_BSTR:
	{
		USES_CONVERSION;
		buffer->format(_T("BSTR(%-.90s)"), OLE2CT(var->bstrVal));
	}
	break;

	case VT_DATE:
	{
#ifdef SOOS_WINDOWS
		SYSTEMTIME t;
		::VariantTimeToSystemTime(var->date, &t);
		buffer->format(_T("DATE(%2.2d:%2.2d:%2.2d)"), t.wHour, t.wMinute, t.wSecond);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		struct tm t;
		localtime_r(&var->date.tv_sec, &t);
		buffer->format(_T("DATE(%2.2d:%2.2d:%2.2d)"),
					   t.tm_hour, t.tm_min, t.tm_sec);
#endif
	}
	break;

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
	{
		LONG lBound, uBound;
		LONG i;
		SOCmnString valStr;
		SafeArrayGetLBound(var->parray, 1, &lBound);
		SafeArrayGetUBound(var->parray, 1, &uBound);
		buffer->format(_T("%s[%ld,%ld] ("), vartype2string(&type, var->vt & ~VT_ARRAY), lBound, uBound);

		for (i = lBound; i <= uBound; i++)
		{
			if (buffer->getLength() > 250)
			{
				*buffer += _T("...");
				break;
			}

			switch (var->vt & ~VT_ARRAY)
			{
			case VT_I2:
			{
				short iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				unsigned long ulVal = (unsigned long)iVal & 0x7FFF;

				if (iVal & 0x8000)
				{
					valStr.format(_T("-0x%4.4X"), (~ulVal & 0X7FFF) + 1);
				}
				else
				{
					valStr.format(_T("0x%4.4X"), ulVal);
				}
			}
			break;

			case VT_UI2:
			{
				unsigned short uiVal;
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				ulVal = (unsigned long)uiVal & 0xFFFF;
				valStr.format(_T("0x%4.4X"), ulVal);
			}
			break;

			case VT_I4:
			case VT_INT:
			{
				long lVal;
				SafeArrayGetElement(var->parray, &i, &lVal);
				unsigned long ulVal = (unsigned long)var->lVal & 0x7FFFFFFF;

				if (lVal & 0x80000000)
				{
					valStr.format(_T("-0x%8.8X"), (~ulVal & 0X7FFFFFFF) + 1);
				}
				else
				{
					valStr.format(_T("0x%8.8X"), ulVal);
				}
			}

			case VT_UI4:
			case VT_UINT:
			{
				unsigned int uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("0x%8.8X"), uiVal);
			}
			break;

			case VT_I1:
			{
				char bVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				unsigned long ulVal = (unsigned long)bVal & 0x7F;

				if (bVal & 0x80)
				{
					valStr.format(_T("-0x%2.2X"), (~ulVal & 0X7F) + 1);
				}
				else
				{
					valStr.format(_T("0x%2.2X"), ulVal);
				}
			}
			break;

			case VT_UI1:
			{
				char bVal;
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				ulVal = (unsigned long)bVal & 0xFF;
				valStr.format(_T("0x%2.2X"), ulVal);
			}
			break;

			case VT_I8:
			{
				LONGLONG llVal;
				SafeArrayGetElement(var->parray, &i, &llVal);
				ULONGLONG ull = llVal & 0x7FFFFFFFFFFFFFFF;
				unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
				unsigned long hi = (unsigned long)(ull >> 32);

				if (llVal & 0x8000000000000000)
				{
					hi = (~hi & 0x7FFFFFFF);
					lo = (~lo & 0xFFFFFFFF);
					lo++;

					if (lo == 0)
					{
						hi++;
					}

					valStr.format(_T("-0x%8.8X%8.8X"), hi, lo);
				}
				else
				{
					valStr.format(_T("0x%8.8X%8.8X"), hi, lo);
				}
			}
			break;

			case VT_UI8:
			{
				ULONGLONG ullVal;
				SafeArrayGetElement(var->parray, &i, &ullVal);
				ULONGLONG ull = ullVal;
				unsigned long lo = (unsigned long)(ull & 0xFFFFFFFF);
				unsigned long hi = (unsigned long)(ull >> 32);
				valStr.format(_T("0x%8.8X%8.8X"), hi, lo);
			}
			break;

			case VT_R4:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.fltVal);
				bstr.vt = VT_R4;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_R8:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_R8;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BOOL:
			{
				VARIANT_BOOL boolVal;
				SafeArrayGetElement(var->parray, &i, &boolVal);
				valStr.format(_T("%s"), (boolVal) ? _T("-1") : _T("0"));
			}
			break;

			case VT_CY:
			{
				USES_CONVERSION;
				VARIANT bstr;
				::VariantInit(&bstr);
				::SafeArrayGetElement(var->parray, &i, &bstr.dblVal);
				bstr.vt = VT_CY;
				SOCmnVariant::variantChangeTypeEx(&bstr, &bstr, localeID, 0, VT_BSTR);
				valStr.format(_T("%-.90s"), OLE2CT(bstr.bstrVal));
				SOCmnVariant::variantClear(&bstr);
			}
			break;

			case VT_BSTR:
			{
				USES_CONVERSION;
				BSTR bstrVal = NULL;
				::SafeArrayGetElement(var->parray, &i, &bstrVal);
				valStr.format(_T("%-.80s"), OLE2CT(bstrVal));
				::SysFreeString(bstrVal);
			}
			break;

			case VT_DATE:
			{
#ifdef SOOS_WINDOWS
				double dblVal;
				SYSTEMTIME t;
				SafeArrayGetElement(var->parray, &i, &dblVal);
				::VariantTimeToSystemTime(dblVal, &t);
				valStr.format(_T("%2.2d:%2.2d:%2.2d"), t.wHour, t.wMinute, t.wSecond);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
				DATE dateVal;
				struct tm t;
				SafeArrayGetElement(var->parray, &i, &dateVal);
				localtime_r(&dateVal.tv_sec, &t);
				buffer->format(_T("%2.2d:%2.2d:%2.2d"),
							   t.tm_hour, t.tm_min, t.tm_sec);
#endif
			}
			break;

			case VT_VARIANT:
			{
				SOCmnVariant varV;
				SafeArrayGetElement(var->parray, &i, (void*)(LPCVARIANT)varV);
				variantWithType2string(&valStr, &varV, localeID);
			}
			break;
			}

			*buffer += valStr;

			if (i + 1 <= uBound)
			{
				*buffer += _T(", ");
			}
			else
			{
				*buffer +=  _T(")");
			}
		}
	}
	break;

	default:
		buffer->format(_T("VARIANT vt; 0x%8.8X"), var->vt);
		break;
	}

	return (LPCTSTR) * buffer;
} // variantWithType2stringHex


//-----------------------------------------------------------------------------
// putArrayElement
//
// - convert a string into a safe array element
//
BOOL putArrayElement(
	IN LPVARIANT array, // variant containing the safe array
	IN LONG idx,                        // element index
	IN LPCTSTR strVal)  // string
{
	SOCmnVariant value;
	VARTYPE datatype = array->vt & (~VT_ARRAY);

	if ((datatype == VT_UI1) || (datatype == VT_UI2) || (datatype == VT_UI4) || (datatype == VT_UINT))
	{
		value.setUI4(_tcstoul(strVal, NULL, 0));
	}
	else if ((datatype == VT_I1) || (datatype == VT_I2) || (datatype == VT_I4) || (datatype == VT_INT))
	{
		value.setI4(_tcstol(strVal, NULL, 0));
	}
	else if ((datatype == VT_R4) || (datatype == VT_R8))
	{
		value.setR8(_tcstod(strVal, NULL));
	}

#if _MSC_VER > 1200 // Microsoft C++ V6.0
	else if (datatype == VT_I8)
	{
		value.setI8(_tcstoi64(strVal, NULL, 0));
	}
	else if (datatype == VT_UI8)
	{
		value.setUI8(_tcstoui64(strVal, NULL, 0));
	}

#endif
	else
	{
		value = strVal;
	}

	// convert the data
	if (!value.changeDatatype(array->vt & (~VT_ARRAY)))
	{
		return FALSE;
	}

	if (array->vt == (VT_BSTR | VT_ARRAY))
	{
		::SafeArrayPutElement(array->parray, &idx, (void*)::SysAllocString(value.bstrVal));
	}
	else if (array->vt == (VT_DECIMAL | VT_ARRAY))
	{
		::SafeArrayPutElement(array->parray, &idx, (void*)&value.decVal);
	}
	else
	{
		::SafeArrayPutElement(array->parray, &idx, (void*)&value.bVal);
	}

	return TRUE;
} // putArrayElement


//-----------------------------------------------------------------------------
// convertWriteStringToVariant
//
// - convert write string
//
BOOL convertWriteStringToVariant(
	IN SOCltElement* obj,
	IN LPCTSTR valueString,
	IN VARTYPE datatype,
	IN OPTIONAL DWORD startIdx,
	OUT SOCmnVariant& value)
{
	BOOL ret = TRUE;

	if (!(datatype & VT_ARRAY))
	{
		// simple datatype
		if ((datatype == VT_UI1) || (datatype == VT_UI2) || (datatype == VT_UI4) || (datatype == VT_UINT))
		{
			value.setUI4(_tcstoul(valueString, NULL, 0));
		}
		else if ((datatype == VT_I1) || (datatype == VT_I2) || (datatype == VT_I4) || (datatype == VT_INT))
		{
			value.setI4(_tcstol(valueString, NULL, 0));
		}
		else if ((datatype == VT_R4) || (datatype == VT_R8))
		{
			value.setR8(_tcstod(valueString, NULL));
		}

#if _MSC_VER > 1200 // Microsoft C++ V6.0
		else if (datatype == VT_I8)
		{
			value.setI8(_tcstoi64(valueString, NULL, 0));
		}
		else if (datatype == VT_UI8)
		{
			value.setUI8(_tcstoui64(valueString, NULL, 0));
		}

#endif
		else
		{
			value = valueString;
		}

		// convert the data
		if (!value.changeDatatype(datatype))
		{
			getMainView()->insertError(obj, HRESULT_FROM_WIN32(GetLastError()),
									   SODMC_ERR_CONVERT_DA_WRITE_VALUE, _T("Can't convert DA write value to selected or native datatype of the item"));
			ret = FALSE;
		}
	}
	else
	{
		// array
		SOCmnString valueStr(valueString);
		SAFEARRAYBOUND dim;
		LPTSTR tokenEnd, tokenBegin;
		DWORD arraySize = 0;
		LONG valueStrSize = (long)valueStr.getLength();
		LONG i;
		BOOL putOK = TRUE;
		tokenBegin = valueStr;

		while ((tokenEnd = _tcspbrk(tokenBegin, _T(";"))) != NULL)
		{
			tokenBegin = tokenEnd + 1;
			arraySize++;
		}

		if (valueStrSize > tokenBegin - (LPCTSTR)valueStr)
		{
			arraySize++;
		}

		dim.lLbound = startIdx;
		dim.cElements = arraySize;
		value.vt = datatype;
		value.parray = SOCmnVariant::safeArrayCreate(datatype & ~VT_ARRAY, 1, &dim);
		tokenBegin = valueStr;
		i = (LONG)startIdx;

		while ((tokenEnd = _tcspbrk(tokenBegin, _T(";"))) != NULL)
		{
			*tokenEnd = _T('\0');

			if (!(putOK = putArrayElement(value, i, tokenBegin)))
			{
				break;
			}

			tokenBegin = tokenEnd + 1;
			i++;
		}

		if ((putOK) && (valueStrSize > tokenBegin - (LPCTSTR)valueStr))
		{
			putOK = putArrayElement(value, i, tokenBegin);
		}

		if (!putOK)
		{
			getMainView()->insertError(obj, HRESULT_FROM_WIN32(GetLastError()),
									   SODMC_ERR_CONVERT_DA_WRITE_VALUE, _T("Can't convert DA write value to selected or native datatype of the item"));
			ret = FALSE;
		}
	}

	return ret;
} // convertWriteStringToVariant




//-----------------------------------------------------------------------------
// SODmCApp                                                                   |
//-----------------------------------------------------------------------------

SODmCApp theApp;

SODmCApp* getApp(void)
{
	return (SODmCApp*)&theApp;
}

BEGIN_MESSAGE_MAP(SODmCApp, CWinApp)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
//>>>SJ 04.09.2008 help added
	ON_COMMAND(ID_HELP, OnHelpFinder)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
//<<<SJ 04.09.2008 help added
END_MESSAGE_MAP()

SODmCApp::SODmCApp()
{
	m_dirtyFlag = FALSE;
	m_fileVersion = 0;
	m_options.viewIOErrors = TRUE;
	m_options.cmOn = TRUE;
	m_options.timeOnly = TRUE;
	m_options.valuePure = TRUE;
	m_options.trcOn = FALSE;
	m_options.alphaItemOn = TRUE;
	m_options.alphaAEOn = TRUE;
	m_options.valueDec = TRUE;
//>>>SJ 04.09.2008 help added
	EnableHtmlHelp();
//<<<SJ 04.09.2008 help added
}

//-----------------------------------------------------------------------------
// InitInstance
//
// - initialize application
//
// returns:
//		TRUE
//
BOOL SODmCApp::InitInstance()
{
#ifdef SOFLAG_NO_DLLS
	// toolkit initialization !only needed if not using toolkit DLLs!
	initSOCmn();
#endif
	// standard initialization
	//Enable3dControlsStatic();
	SetRegistryKey(_T("Softing"));
	LoadStdProfileSettings();
	// command line
	SOCmnString cmdLine = GetCommandLine();
	cmdLine.makeLower();

	if ((_tcsstr(cmdLine, _T("-regserver")) != NULL) ||
		(_tcsstr(cmdLine, _T("/regserver")) != NULL))
	{
		doRegister();
		return FALSE;
	}

	if ((_tcsstr(cmdLine, _T("-unregserver")) != NULL) ||
		(_tcsstr(cmdLine, _T("/unregserver")) != NULL))
	{
		doUnregister();
		return FALSE;
	}

	m_options.viewIOErrors = GetProfileInt(_T("Options"), _T("viewIOErrors"), TRUE);
	m_options.cmOn = GetProfileInt(_T("Options"), _T("cmOn"), TRUE);
	m_options.timeOnly = GetProfileInt(_T("Options"), _T("timeOnly"), TRUE);
	m_options.valuePure = GetProfileInt(_T("Options"), _T("valuePure"), TRUE);
	m_options.trcOn = GetProfileInt(_T("Options"), _T("TraceFileEnable"), FALSE);
	m_options.valueDec = GetProfileInt(_T("Options"), _T("valueDec"), TRUE);
	m_options.alphaItemOn = GetProfileInt(_T("Options"), _T("alphaSortItems"), FALSE);
	m_options.alphaAEOn = GetProfileInt(_T("Options"), _T("alphaSortAE"), FALSE);
	enableTrace(m_options.trcOn);
	// create and load the frame with its resources
	SODmCMainFrame* pFrame = new SODmCMainFrame;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
	// show main frame
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// allow everyone to connect the callbacks to the client
	// if this is not here, about dialog will crash demo client!!!!!
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	DWORD authnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	DWORD impLevel = RPC_C_IMP_LEVEL_IDENTIFY;
	DWORD capabilities = EOAC_NONE;

	// read registry settings
	{
		DWORD sizeDWORD = sizeof(DWORD);
		HKEY keyServer = NULL;

		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Client"),
			NULL, KEY_READ, &keyServer))
		{
			RegQueryValueEx(keyServer, _T("AuthLevel"), NULL, NULL, (LPBYTE)&authnLevel, &sizeDWORD);
			RegQueryValueEx(keyServer, _T("ImpLevel"), NULL, NULL, (LPBYTE)&impLevel, &sizeDWORD);
			RegCloseKey(keyServer);
		}
	}

	// initialize toolkit
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creatorDA = new SODmCCreatorDA();
	entryDA->initialize(COINIT_MULTITHREADED);
	entryDA->initializeSecurity(authnLevel, impLevel, capabilities);
	entryDA->setNotifyWindow(pFrame->m_hWnd);
	entryDA->setCreator(creatorDA);
	entryDA->start();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	SOCmnPointer<SOAeCCreator> creatorAE = new SODmCCreatorAE();
	entryAE->initialize(COINIT_MULTITHREADED);
#ifndef SODMC_DA
	// only initialize DCOM security once, not twice
	entryAE->initializeSecurity(authnLevel, impLevel, capabilities);
#endif
	entryAE->setNotifyWindow(pFrame->m_hWnd);
	entryAE->setCreator(creatorAE);
	entryAE->start();
#endif
	LPTSTR cmdLineP, cmdLineP1, cmdLineP2;

	if (((cmdLineP1 = _tcsstr(cmdLine, _T("-load"))) != NULL) ||
		((cmdLineP2 = _tcsstr(cmdLine, _T("/load"))) != NULL))
	{
		if (cmdLineP1)
		{
			cmdLineP = cmdLineP1;
		}
		else
		{
			cmdLineP = cmdLineP2;
		}

		LPTSTR filePath = cmdLineP + _tcslen(_T("-load")) + 1;
		if ((filePath[0] == '\"') && (filePath[_tcslen(filePath) - 1] == '\"'))
		{
			filePath = filePath + 1;
			filePath[_tcslen(filePath) - 1] = '\0';
		}

		doOpen(filePath);
	}

	if (((cmdLineP1 = _tcsstr(cmdLine, _T("-import"))) != NULL) ||
		((cmdLineP2 = _tcsstr(cmdLine, _T("/import"))) != NULL))
	{
		if (cmdLineP1)
		{
			cmdLineP = cmdLineP1;
		}
		else
		{
			cmdLineP = cmdLineP2;
		}

		doImport(cmdLineP + 8);
	}

	return TRUE;
} // InitInstance


//-----------------------------------------------------------------------------
// ExitInstance
//
// - terminate application
//
// returns:
//		return of base class
//
int SODmCApp::ExitInstance()
{
	// terminate entries
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	entryDA->disconnect();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->disconnect();
#endif
#ifdef SODMC_DA
	entryDA->terminate();
	entryDA.release();
#endif
#ifdef SODMC_AE
	entryAE->terminate();
	entryAE.release();
#endif
#ifdef SOFLAG_NO_DLLS
	// free global toolkit resources !only needed if not using toolkit DLLs!
	exitSOCmn();
#endif
	// save application settings
	SaveStdProfileSettings();
	// write application options to the registry
	WriteProfileInt(_T("Options"), _T("viewIOErrors"), m_options.viewIOErrors);
	WriteProfileInt(_T("Options"), _T("timeOnly"), m_options.timeOnly);
	WriteProfileInt(_T("Options"), _T("cmOn"), m_options.cmOn);
	WriteProfileInt(_T("Options"), _T("valuePure"), m_options.valuePure);
	WriteProfileInt(_T("Options"), _T("TraceFileEnable"), m_options.trcOn);
	WriteProfileInt(_T("Options"), _T("valueDec"), m_options.valueDec);
	WriteProfileInt(_T("Options"), _T("alphaSortItems"), m_options.alphaItemOn);
	WriteProfileInt(_T("Options"), _T("alphaSortAE"), m_options.alphaAEOn);
	return CWinApp::ExitInstance();
} // ExitInstance


//-----------------------------------------------------------------------------
// OnFileNew
//
// - menu handler of menu file/new
//
void SODmCApp::OnFileNew()
{
	doFileNew();
}

int SODmCApp::doFileNew()
{
	CWaitCursor waitCursor;

	if (IDCANCEL == doSaveChanges())
	{
		return IDCANCEL;
	}

	m_fileName.empty();
	resetDirtyFlag();
	// deactivate and remove all toolkit objects
#ifdef SODMC_DA
	SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
	entryDA->disconnect();
	entryDA->removeAllAdditionalElements();
	entryDA->removeAllLeaves();
	entryDA->removeAllBranches();
#endif
#ifdef SODMC_AE
	SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
	entryAE->disconnect();
	entryAE->removeAllAdditionalElements();
	entryAE->removeAllLeaves();
	entryAE->removeAllBranches();
#endif
	// delete the root object of the GUI tree
	SODmCObjectView* objview = getObjectView();
	objview->deleteDATree();
	objview->deleteAETree();
	return IDYES;
} // OnFileNew


//-----------------------------------------------------------------------------
// OnFileOpen
//
// - menu handler of menu file/open
//
void SODmCApp::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T("soc"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
					_T("Softing OPC Client Files (*.soc)|*.soc|All Files (*.*)|*.*||"));
	// show file open dialog
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
	dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

	if (IDOK == dlg.DoModal())
	{
		doOpen(dlg.GetPathName());
	}
} // OnFileOpen


//-----------------------------------------------------------------------------
// doOpen
//
// - file open helper
//
void SODmCApp::doOpen(
	IN LPCTSTR fileName)    // file name
{
	if (IDCANCEL == doFileNew())    // clear configuration
	{
		return;
	}

	CWaitCursor waitCursor;
	CFile file;

	if (file.Open(fileName, CFile::modeRead))
	{
		m_fileName = fileName;
		m_pRecentFileList->Add(m_fileName);
#ifdef SODMC_DA
		SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
#endif
#ifdef SODMC_AE
		SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
#endif
		int size = (int)file.GetLength();

		if (size)
		{
			SOCltStorage stg(size);
			file.Read((void*)stg.getBuffer(), size);
			stg >> m_fileVersion;

			if ((HIWORD(m_fileVersion) < 3) || (HIWORD(m_fileVersion) > 5))
			{
				AfxMessageBox(_T("Invalid file format!"), MB_OK | MB_ICONSTOP);
			}
			else
			{
				// serialize objects
#ifdef SODMC_DA
				entryDA->serialize(stg, FALSE);
#endif
#ifdef SODMC_AE
				entryAE->serialize(stg, FALSE);
#endif
				// activate all objects
#ifdef SODMC_DA
				entryDA->start(TRUE, SOCLT_EXEC_ASYNC);
#endif
#ifdef SODMC_AE
				entryAE->start(TRUE, SOCLT_EXEC_ASYNC);
#endif
			}
		}

		file.Close();
	}
} // doOpen


void SODmCApp::doImport(LPCTSTR fileName)
{
#ifdef SODMC_DA
	SOCmnPointer<SOCmnXmlTree> cfgXmlTree;
	SOCMN_ALLOCATE_OBJECT_C(cfgXmlTree, SOCmnXmlTree(fileName), (SOCmnXmlTree*));

	if (cfgXmlTree->readContents())
	{
		cfgXmlTree->parse();
		cfgXmlTree->freeContents();
	}
	else
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("Import file"), (_T("Can't read import file %s"), (LPCTSTR)fileName));
	}

	SOCmnPointer<SOCmnXmlTreeElement> xmlRoot = cfgXmlTree->getXmlRoot();
	SOCmnStringList* pAttributes;
	SOCmnPointer<SOCmnXmlTreeElement> sessions;
	SOCmnList<SOCmnXmlTreeElement> sessionList;
	SOCmnXmlTreeElement* pSession;
	SOCmnListPosition posS;
	SOCmnPointer<SOCmnXmlTreeElement> subscriptions;
	SOCmnList<SOCmnXmlTreeElement> subscriptionList;
	SOCmnXmlTreeElement* pSubscription;
	SOCmnListPosition posU;
	SOCmnPointer<SOCmnXmlTreeElement> items;
	SOCmnList<SOCmnXmlTreeElement> itemList;
	SOCmnXmlTreeElement* pItem;
	SOCmnListPosition posI;
	SOCmnString value;
	SOCmnString url;
	DWORD samplingRate;
	SOCmnString itemID;
	SOCmnPointer<SODaCEntry> daEntry = ::getSODaCEntry();
	SOCmnPointer<SODaCServer> daSession;
	SOCmnPointer<SODaCGroup> daSubscription;
	SOCmnPointer<SODaCItem> daItem;
	sessions = (SOCmnXmlTreeElement*)xmlRoot->findBranch(_T("OPCToolboxDemoClient\x7fSessions"), TRUE);

	if (sessions.isNotNull())
	{
		sessionList = (SOCmnList<SOCmnXmlTreeElement> *)sessions->getBranchList();
		posS = sessionList.getStartPosition();

		while (posS)
		{
			pSession = sessionList.getNext(posS);

			if (pSession->getName() == _T("Session"))
			{
				pAttributes = pSession->getXmlAttributes();
				url.empty();
				pAttributes->findKey(_T("URL"), &url);
				daSession = daEntry->addServer(url);
				subscriptions = (SOCmnXmlTreeElement*)pSession->findBranch(_T("Subscriptions"), FALSE);

				if (subscriptions.isNotNull())
				{
					subscriptionList = (SOCmnList<SOCmnXmlTreeElement> *)subscriptions->getBranchList();
					posU = subscriptionList.getStartPosition();

					while (posU)
					{
						pSubscription = subscriptionList.getNext(posU);

						if (pSubscription->getName() == _T("Subscription"))
						{
							pAttributes = pSubscription->getXmlAttributes();
							samplingRate = 1000;

							if (pAttributes->findKey(_T("SamplingRate"), &value))
							{
								samplingRate = (DWORD)_tcstoul(value, NULL, 10);
							}

							daSubscription = daSession->addGroup(NULL, samplingRate);
							items = (SOCmnXmlTreeElement*)pSubscription->findBranch(_T("Items"), FALSE);

							if (items.isNotNull())
							{
								itemList = (SOCmnList<SOCmnXmlTreeElement> *)items->getBranchList();
								posI = itemList.getStartPosition();

								while (posI)
								{
									pItem = itemList.getNext(posI);

									if (pItem->getName() == _T("Item"))
									{
										pAttributes = pItem->getXmlAttributes();
										itemID.empty();
										pAttributes->findKey(_T("ID"), &itemID);
										daItem = daSubscription->addItem(itemID);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	daEntry->start();
	xmlRoot->removeAllBranches();
	xmlRoot.release();
	SODmCMainView* pMainView = getMainView();
	pMainView->selectDAItemTab();
#endif //SODMC_DA
}

//-----------------------------------------------------------------------------
// OnOpenRecentFile
//
// - menu handler of menu file/'recent files'
//
BOOL SODmCApp::OnOpenRecentFile(
	IN UINT nID)    // resource id
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);
	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);
	doOpen((*m_pRecentFileList)[nIndex]);
	return TRUE;
} // OnOpenRecentFile


//-----------------------------------------------------------------------------
// OnFileSaveAs
//
// - menu handler of menu file/save as
//
void SODmCApp::OnFileSaveAs()
{
	CFileDialog dlg(FALSE, _T("soc"), _T("soclient"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
					_T("Softing OPC Client Files (*.soc)|*.soc|All Files (*.*)|*.*||"));
	// show save file dialog
#if (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
	dlg.GetOFN().lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif

	if (IDOK == dlg.DoModal())
	{
		m_fileName = dlg.GetPathName();
		m_pRecentFileList->Add(m_fileName);
		OnFileSave();
	}
} // OnFileSaveAs


//-----------------------------------------------------------------------------
// OnFileSave
//
// - menu handler of menu file/save
//
void SODmCApp::OnFileSave()
{
	if (!m_fileName.isEmpty())
	{
		CFile file;
		CWaitCursor waitCursor;

		// open file
		if (file.Open(m_fileName, CFile::modeCreate | CFile::modeWrite))
		{
			SOCltStorage stg;
			DWORD fileVersion;
			resetDirtyFlag();
			fileVersion = MAKELONG(MINOR_VERSION_NUMBER, MAJOR_VERSION_NUMBER);
			stg << fileVersion;
			// serialize objects to storage
#ifdef SODMC_DA
			SOCmnPointer<SODaCEntry> entryDA = ::getSODaCEntry();
			entryDA->serialize(stg, TRUE);
#endif
#ifdef SODMC_AE
			SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();
			entryAE->serialize(stg, TRUE);
#endif
			int size = stg.getPosition();

			if (size)
			{
				// copy object data to the file
				file.Write(stg.getBuffer(), size);
			}

			file.Close();
		}
	}
	else    // no current file name
	{
		OnFileSaveAs();    // -> call save as
	}
} // OnFileSave


//-----------------------------------------------------------------------------
// doSaveChanges
//
// - check dirty flag and ask for saving
//
int SODmCApp::doSaveChanges(void)
{
	int ret = IDYES;

	if (getDirtyFlag())
	{
		CString message;

		if (!m_fileName.isEmpty())
		{
			message.Format(_T("Save changes to %s?"), m_fileName);
		}
		else
		{
			message = _T("Save changes?");
		}

		if (IDYES == (ret = AfxMessageBox(message, MB_YESNOCANCEL | MB_ICONQUESTION)))
		{
			OnFileSave();
		}
	}

	return ret;
} // doSaveChanges


void SODmCApp::doRegister(void)
{
	SOCmnString cmd;
	TCHAR module[_MAX_PATH];
	TCHAR moduleShort[_MAX_PATH];
	GetModuleFileName(m_hInstance, module, _MAX_PATH);
	GetShortPathName(module, moduleShort, _MAX_PATH);
	cmd.format(_T("%s -Load %%1"), moduleShort);
	CRegKey::SetValue(HKEY_CLASSES_ROOT, _T(".soc"), _T("Softing.OPCToolboxDemo.Client"));
	CRegKey::SetValue(HKEY_CLASSES_ROOT, _T("Softing.OPCToolboxDemo.Client\\shell\\open\\command"), cmd);
	CRegKey::SetValue(HKEY_CLASSES_ROOT, _T("Applications\\SOClient.exe\\shell\\open\\command"), cmd);
}


void SODmCApp::doUnregister(void)
{
	CRegKey keyHKCR;
	keyHKCR.Attach(HKEY_CLASSES_ROOT);
	keyHKCR.RecurseDeleteKey(_T(".soc"));
	keyHKCR.RecurseDeleteKey(_T("Softing.OPCToolboxDemo.Client"));
	keyHKCR.RecurseDeleteKey(_T("Applications\\SOClient.exe"));
}

void SODmCApp::enableTrace(BOOL traceFileOn)
{
	SOCmnTrace* trace = ::getSOCmnTrace();
	HKEY keyServer = NULL;
	DWORD sizeDWORD = sizeof(DWORD);
	DWORD sizeBuffer = MAX_PATH;
	TCHAR stringBuffer[MAX_PATH];
	SOCmnString traceFile = _T("SOClient.txt");  // path of trace file
	DWORD traceFileMaxSize;     // max size of trace file
	DWORD traceErrorLevel;      // trace group in error level
	DWORD traceWarningLevel;    // trace group in warning level
	DWORD traceInfoLevel;       // trace group in infomation level
	DWORD traceDebugLevel;      // trace group in debug level
	DWORD traceMaxBackups = 0;
	traceFileMaxSize = 1000000;
	traceErrorLevel = SOCMNTRACE_G_ALL;
	traceWarningLevel = SOCMNTRACE_G_ALL;
#ifdef _DEBUG
	traceInfoLevel = SOCMNTRACE_G_OPC;
	traceDebugLevel = SOCMNTRACE_G_OPC;
#else

	if (traceFileOn)
	{
		traceInfoLevel = SOCMNTRACE_G_OPC;
		traceDebugLevel = SOCMNTRACE_G_OPC;
	}
	else
	{
		traceInfoLevel = 0;
		traceDebugLevel = 0;
	}

#endif

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Softing\\OPC Classic Demo Client"),
									  NULL, KEY_READ, &keyServer))
	{
		RegQueryValueEx(keyServer, _T("TraceError"), NULL, NULL, (LPBYTE)&traceErrorLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceWarning"), NULL, NULL, (LPBYTE)&traceWarningLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceInfo"), NULL, NULL, (LPBYTE)&traceInfoLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceDebug"), NULL, NULL, (LPBYTE)&traceDebugLevel, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceFileMaxSize"), NULL, NULL, (LPBYTE)&traceFileMaxSize, &sizeDWORD);
		RegQueryValueEx(keyServer, _T("TraceFileMaxBackups"), NULL, NULL, (LPBYTE)&traceMaxBackups, &sizeDWORD);
		stringBuffer[0] = _T('\0');
		RegQueryValueEx(keyServer, _T("TraceFile"), NULL, NULL, (LPBYTE)stringBuffer, &sizeBuffer);

		if ((sizeBuffer != 0) && (_tcslen(stringBuffer) != 0))
		{
			traceFile = stringBuffer;
		}
		RegCloseKey(keyServer);
	}

	// initialize trace
	trace->enableTraceToFile(traceFileOn);
	trace->setFile2(traceFile, traceMaxBackups, traceFileMaxSize);
	trace->setLevels(traceErrorLevel, traceWarningLevel, traceInfoLevel, traceDebugLevel);
}


//>>> SJ 04.09.2008 help functionality added
//------------------------------------------------------------------
void SODmCApp::OnHelpFinder()
{
	ShowHelp(0, HELP_FINDER);
	return;
}

//------------------------------------------------------------------
void SODmCApp::WinHelp(DWORD dwData, UINT nCmd)
{
	ShowHelp(dwData, nCmd);
	return;
}

//------------------------------------------------------------------
void SODmCApp::ShowHelp(DWORD dwData, UINT nCmd)
{
	ASSERT(m_pMainWnd->m_hWnd != NULL);
	ASSERT(m_pszHelpFilePath != NULL);
	CWaitCursor wait;
	DWORD dwRawID = dwData;

	switch (nCmd)
	{
	case HELP_CONTEXT:
		if (dwData >= 0x30000)
		{
			dwData -= 0x30000;
		}
		else if (dwData >= 0x20000)
		{
			dwData -= 0x20000;
		}
		else if (dwData >= 0x10000)
		{
			dwData -= 0x10000;
		}

		TRACE(_T("SODmCApp::WinHelp: pszHelpFile = '%s', ContextID: %d (raw:0x%08X), fuCommand: %d.\n"),
			  m_pszHelpFilePath, dwData, dwRawID, nCmd);
//      if (!HtmlHelp(m_pMainWnd->m_hWnd, m_pszHelpFilePath, HH_HELP_CONTEXT, dwData))
//          TRACE(_T("Help context ID %d (raw:0x%08X) NOT DEFINED!!\n"), dwData, dwRawID);
		HtmlHelp(dwData, HH_HELP_CONTEXT);
		break;

	case HELP_INDEX:
		TRACE(_T("SODmCApp::WinHelp: pszHelpFile = '%s', HELP_INDEX.\n"), m_pszHelpFilePath);
		break;

	case HELP_FINDER:
		TRACE(_T("SODmCApp::WinHelp: pszHelpFile = '%s', HELP_FINDER.\n"), m_pszHelpFilePath);
//      if (!HtmlHelp(m_pMainWnd->m_hWnd, m_pszHelpFilePath, HH_DISPLAY_TOPIC, NULL))
//          TRACE(_T("Help finder/topic NOT DEFINED!!\n"));
		HtmlHelp(NULL, HH_DISPLAY_TOPIC);
		break;
	} // end of switch(nCmd)

	return;
}
//<<< SJ 04.09.2008 help functionality added

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// SODmCCreatorDA                                                             |
//-----------------------------------------------------------------------------

SODaCServer* SODmCCreatorDA::createServer(IN BYTE protocolType, IN SODaCEntry* parent)
{
	SODaCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SODMC_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCComServerDA(), (SODaCServer*));
		break;
#endif
#ifdef SODMC_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCXmlServerDA(), (SODaCServer*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCTPServerDA(), (SODaCServer*));
		break;
#endif

	default:
		return SODaCCreator::createServer(protocolType, parent);
		break;
	}

	return p;
}

SODaCGroup* SODmCCreatorDA::createGroup(IN BYTE protocolType, IN SODaCServer* parent)
{
	SODaCGroup* p = NULL;

	switch (protocolType)
	{
#ifdef SODMC_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCComGroup(), (SODaCGroup*));
		break;
#endif
#ifdef SODMC_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCXmlGroup(), (SODaCGroup*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCTPGroup(), (SODaCGroup*));
		break;
#endif

	default:
		return SODaCCreator::createGroup(protocolType, parent);
		break;
	}

	return p;
}

SODaCItem* SODmCCreatorDA::createItem(IN SODaCGroup* parent)
{
	return new SODmCItem();
}

#endif

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// SODmCCreatorAE                                                             |
//-----------------------------------------------------------------------------

SOAeCServer* SODmCCreatorAE::createServer(IN BYTE protocolType, IN SOAeCEntry* parent)
{
	SOAeCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SODMC_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCComServerAE(), (SOAeCServer*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCTPServerAE(), (SOAeCServer*));
		break;
#endif

	default:
		return SOAeCCreator::createServer(protocolType, parent);
		break;
	}

	return p;
}

SOAeCSubscription* SODmCCreatorAE::createSubscription(IN BYTE protocolType, IN SOAeCServer* parent)
{
	SOAeCSubscription* p = NULL;

	switch (protocolType)
	{
#ifdef SODMC_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCComSubscription(), (SOAeCSubscription*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODmCTPSubscription(), (SOAeCSubscription*));
		break;
#endif

	default:
		return SOAeCCreator::createSubscription(protocolType, parent);
		break;
	}

	return p;
}

SOAeCEvent* SODmCCreatorAE::createEvent(IN SOAeCSubscription* subs, IN ONEVENTSTRUCT* pData)
{
	return new SODmCEvent(subs, pData);
}

#endif

#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCServerBrowser                                                          |
//-----------------------------------------------------------------------------

void SODmCServerBrowser::onError(IN HRESULT res, IN DWORD errorcode)
{
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	getMainView()->insertError((SOCltElement*)this, res, errorcode, et);
}

#endif // SODMC_DCOM
