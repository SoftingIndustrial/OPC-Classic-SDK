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
//  Filename    : SODmCItem.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA item classes                                             |
//                - SODmCItem: Toolkit item object class                      |
//                - SODmCPPItem: Base class for all item property pages       |
//                - SODmCPPItemAttributes: Item attribute property page       |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "SOClient.h"
#include "SODmCServerDA.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCPropFrm.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"

LPCTSTR arrayvariant2string(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT, OPTIONAL IN BOOL readFormat = TRUE);

//-----------------------------------------------------------------------------
// variant2string
//
// - convert variant to string
//
// returns:
//		pointer to string
//
LPCTSTR arrayvariant2string(
	IN SOCmnString* buffer,     // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID,  // locale id
	OPTIONAL IN BOOL readFormat)
{
	switch (var->vt)
	{
	case (VT_ARRAY | VT_I1):
	case (VT_ARRAY | VT_UI1):
	case (VT_ARRAY | VT_I2):
	case (VT_ARRAY | VT_UI2):
	case (VT_ARRAY | VT_I4):
	case (VT_ARRAY | VT_UI4):
	case (VT_ARRAY | VT_INT):
	case (VT_ARRAY | VT_UINT):
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
			switch (var->vt & ~VT_ARRAY)
			{
			case VT_I2:
			{
				short iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UI2:
			{
				unsigned short uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I4:
			{
				long lVal;
				SafeArrayGetElement(var->parray, &i, &lVal);
				valStr.format(_T("%li"), lVal);
			}
			break;

			case VT_UI4:
			{
				unsigned long ulVal;
				SafeArrayGetElement(var->parray, &i, &ulVal);
				valStr.format(_T("%lu"), ulVal);
			}
			break;

			case VT_INT:
			{
				int iVal;
				SafeArrayGetElement(var->parray, &i, &iVal);
				valStr.format(_T("%i"), iVal);
			}
			break;

			case VT_UINT:
			{
				unsigned int uiVal;
				SafeArrayGetElement(var->parray, &i, &uiVal);
				valStr.format(_T("%u"), uiVal);
			}
			break;

			case VT_I8:
			{
				LONGLONG llVal;
				SafeArrayGetElement(var->parray, &i, &llVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%lld"), llVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64d"), llVal);
#endif
			}
			break;

			case VT_UI8:
			{
				ULONGLONG ullVal;
				SafeArrayGetElement(var->parray, &i, &ullVal);
#ifdef SOOS_LINUX
				valStr.format(_T("%llu"), ullVal);
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
				valStr.format(_T("%I64u"), ullVal);
#endif
			}
			break;

			case VT_UI1:
			{
				char bVal;
				unsigned short usVal;
				SafeArrayGetElement(var->parray, &i, &bVal);
				usVal = (unsigned short)bVal & 0xFF;
				valStr.format(_T("%u"), usVal);
			}
			break;

			case VT_I1:
			{
				char cVal;
				short sVal;
				SafeArrayGetElement(var->parray, &i, &cVal);
				sVal = (char)cVal;
				valStr.format(_T("%i"), sVal);
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
} // variant2string



//-----------------------------------------------------------------------------
// SODmCItem                                                                  |
//-----------------------------------------------------------------------------

SODmCItem::SODmCItem()
{
	m_treeHandle = 0;
	m_listIdx = -1;
	m_logToFile = FALSE;
	m_logFile = _T("");
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCItem::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();
	SODmCMainView* mainview = getMainView();

	if ((!objview) || (!mainview))
	{
		return;
	}

	if (!IsWindow(objview->GetTreeCtrl().m_hWnd))
	{
		return;
	}

	if (!IsWindow(mainview->getListDA()->m_hWnd))
	{
		return;
	}

	SOCmnPointer<SODaCGroup> group = (SODaCGroup*)getParentBranch();

	if (group.isNotNull())
	{
		HTREEITEM hTree = NULL;
#ifdef SODMC_DCOM

		if (group->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			hTree = ((SODmCComGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
#ifdef SODMC_SOAP

		if (group->getProtocol() == SOCLT_PROTOCOL_SOAP)
		{
			hTree = ((SODmCXmlGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
#ifdef SOFEATURE_TUNNEL

		if (group->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			hTree = ((SODmCTPGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), hTree, getItemID(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);
		SOCmnPointer<SODaCServer> server = (SODaCServer*)group->getParentBranch();
		int idx = SODmCListInsert(mainview->getListDA(), getItemID(), (void*)(SODaCItem*)this, SODMCMAINVIEW_IMG_OBJ_DEFAULT);
		setListIndex(idx);
		mainview->getListDA()->SetItemText(idx, 6, group->getName());
		mainview->getListDA()->SetItemText(idx, 5, server->getURL());
	}
	else
	{
		if (m_treeHandle)
		{
			HTREEITEM hParent = objview->GetTreeCtrl().GetParentItem(m_treeHandle);

			if (hParent)
			{
				group = (SODaCGroup*)objview->GetTreeCtrl().GetItemData(hParent);
				group.addRef();
			}

			objview->GetTreeCtrl().DeleteItem(m_treeHandle);
			m_treeHandle = 0;
			getPropertyFrame()->removedObject(this);
		}

		if (m_listIdx != -1)
		{
			mainview->getListDA()->DeleteItem(m_listIdx);
			mainview->updateListDAIndex(m_listIdx);
			m_listIdx = -1;
		}
	}

	if (group.isNotNull())
	{
		SOCmnList<SOCmnElement> itemList = group->getLeafList();
		TVITEM  item;
		HTREEITEM hTree = NULL;
#ifdef SODMC_DCOM

		if (group->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			hTree = ((SODmCComGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
#ifdef SODMC_SOAP

		if (group->getProtocol() == SOCLT_PROTOCOL_SOAP)
		{
			hTree = ((SODmCXmlGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
#ifdef SOFEATURE_TUNNEL

		if (group->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			hTree = ((SODmCXmlGroup*)(SODaCGroup*)group)->getTreeHandle();
		}

#endif
		item.hItem = hTree;
		item.mask = TVIF_CHILDREN | TVIF_HANDLE;
		item.cChildren = !itemList.isEmpty();
		objview->GetTreeCtrl().SetItem(&item);

		if (item.cChildren)
		{
			objview->GetTreeCtrl().Expand(hTree, TVE_EXPAND);
		}
	}
} // onSetParentBranch


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCItem::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, m_listIdx, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCItem::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, m_listIdx, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCItem::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// onSetItemID
//
// - called after the item id was set
//
void SODmCItem::onSetItemID(void)
{
	if (m_treeHandle)
	{
		SODmCObjectView* objview = getObjectView();
		objview->GetTreeCtrl().SetItemText(m_treeHandle, getItemID());
	}

	if (m_listIdx != -1)
	{
		SODmCMainView* mainview = getMainView();
		mainview->getListDA()->SetItemText(getListIndex(), 0, getItemID());
	}

	getPropertyFrame()->getPropertySheet()->setItemID(this, getItemID());
} // onSetItemID


//-----------------------------------------------------------------------------
// onSetReadValue
//
// - show read value
//
void SODmCItem::onSetReadValue()    // read value
{
	SODmCMainView* mainview = getMainView();
	SOCmnString valueText;
	VARIANT2STRING(&valueText, m_readValue);
	mainview->getListDA()->SetItemText(getListIndex(), 1, valueText);
} // onSetReadValue


//-----------------------------------------------------------------------------
// onSetReadQuality
//
// - show read quality
//
void SODmCItem::onSetReadQuality()  // read quality
{
	SODmCMainView* mainview = getMainView();
	SOCmnString qualityText;
	::quality2string(&qualityText, m_readQuality);
	mainview->getListDA()->SetItemText(getListIndex(), 2, qualityText);
} // onSetReadQuality


//-----------------------------------------------------------------------------
// onSetReadTimeStamp
//
// - show read time stamp
//
void SODmCItem::onSetReadTimeStamp()
{
	SODmCMainView* mainview = getMainView();
	SOCmnString tsText;
	DATETIME2STRING(tsText, m_readTimeStamp);
	mainview->getListDA()->SetItemText(getListIndex(), 3, tsText);
} // onSetReadTimeStamp


//-----------------------------------------------------------------------------
// onSetReadResult
//
// - add error to the error list if read failed
//
void SODmCItem::onSetReadResult()
{
	SODmCMainView* mainview = getMainView();
	SOCmnString resText;

	if (FAILED(getReadResult()))
	{
		SOCmnString tsText;
		FILETIME errorTimeStamp;

		if (getApp()->getOptions()->viewIOErrors)
		{
			mainview->insertError(this, getReadResult(), SODMC_ERR_BAD_READ_RESULT, _T("The read I/O operation failed"));
		}

		resText.format(_T("READ ERROR 0x%X"), getReadResult());
		mainview->getListDA()->SetItemText(getListIndex(), 1, _T(""));
		mainview->getListDA()->SetItemText(getListIndex(), 2, _T(""));
		CoFileTimeNow(&errorTimeStamp);
		FILETIME2STRING(&tsText, &errorTimeStamp);
		mainview->getListDA()->SetItemText(getListIndex(), 3, tsText);
	}
	else
	{
		resText = _T("");
	}

	mainview->getListDA()->SetItemText(getListIndex(), 4, resText);
} // onSetReadResult

//-----------------------------------------------------------------------------
// setRead
//
// - set read parameters
//
void SODmCItem::setRead(
	IN HRESULT res,
	IN SOCmnVariant* value,
	IN WORD quality,
	IN SOCmnDateTime* timeStamp,
	IN DWORD transactionID)
{
	SODaCItem::setRead(res, value, quality, timeStamp, transactionID);
	USES_CONVERSION;

	if (!getLogToFile())
	{
		return;
	}

	HANDLE hFile =  CreateFile(m_logFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, INVALID_HANDLE_VALUE);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		SOCmnString errDescr;
		errDescr.format(_T("Can't open file %s to read item value to file"), m_logFile);
		this->setLastError(HRESULT_FROM_WIN32(GetLastError()), SODMC_ERR_OPEN_FILE);
		return; // file open error
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);
	SOCmnString tab(_T("\t"));
	SOCmnString buffer;
	SOCmnString outputString;
	outputString = getItemID() + tab;

	if (value->vt & VT_ARRAY)
	{
		// array
		outputString += SOCmnString(arrayvariant2string(&buffer, value)) + tab;
	}
	else if (value->vt == VT_BSTR) // BSTR
	{
		outputString += SOCmnString(W2T(value->bstrVal)) + tab;
	}
	else    // must be a normal datatype
	{
		outputString += SOCmnString(variant2string(&buffer, value)) + tab;
	}

	outputString += SOCmnString(quality2string(&buffer, quality)) + tab;
	outputString += timeStamp->getXMLDateTime() + SOCmnString(_T("\r\n"));
	DWORD writtenBytes = 0;
	WriteFile(hFile, T2A((LPTSTR)outputString), outputString.getLength(), &writtenBytes, NULL);
	CloseHandle(hFile);
} // setRead


//-----------------------------------------------------------------------------
// onSetWriteResult
//
// - add error to the error list if write failed
//
void SODmCItem::onSetWriteResult()
{
	SODmCMainView* mainview = getMainView();
	SOCmnString resText;

	if (FAILED(getWriteResult()))
	{
		if (getApp()->getOptions()->viewIOErrors)
		{
			mainview->insertError(this, getWriteResult(), SODMC_ERR_BAD_WRITE_RESULT, _T("The write I/O operation failed"));
		}

		resText.format(_T("WRITE ERROR 0x%X"), getWriteResult());
	}
	else
	{
		resText = _T("");
	}

	mainview->getListDA()->SetItemText(getListIndex(), 4, resText);
} // onSetWriteResult


//-----------------------------------------------------------------------------
// writeStringValue
//
// - write item value
//
void SODmCItem::writeStringValue(
	IN LPCTSTR valueString,
	IN VARTYPE datatype,
	IN OPTIONAL DWORD startIdx)
{
	SOCmnVariant value;

	if (datatype == VT_EMPTY)
	{
		datatype = getNativeDatatype();
	}

	if (convertWriteStringToVariant(this, valueString, datatype, startIdx, value))
	{
		// write the item data
		write(&value);
	}
} // writeStringValue




//-----------------------------------------------------------------------------
// SODmCPPItem                                                                |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPItem, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPItem, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPItem::SODmCPPItem(
	IN SODmCPSheet* sheet,  // property sheet
	IN UINT resID)      // resource id of property page
	: SODmCPPage(sheet, resID)
{
	m_obj = NULL;
}

SODmCPPItem::~SODmCPPItem()
{
}




//-----------------------------------------------------------------------------
// SODmCPPItemAttributes                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPItemAttributes, SODmCPPItem)

BEGIN_MESSAGE_MAP(SODmCPPItemAttributes, SODmCPPItem)
	ON_NOTIFY(NM_DBLCLK, IDC_ITEM_ACCESS_PATH_LIST, onDblclkAccessPath)
	ON_EN_CHANGE(IDC_ITEM_ID, onChangeID)
	ON_BN_CLICKED(IDC_ITEM_VALID_DEADBAND, onClickDeadband)
END_MESSAGE_MAP()

SODmCPPItemAttributes::SODmCPPItemAttributes(IN SODmCPSheet* sheet)
	: SODmCPPItem(sheet, IDD_PP_ITEM_ATTRIBUTES)
{
	m_obj = NULL;
	m_edItemID.m_firstControl = TRUE;
	m_edClientHandle.m_lastControl = TRUE;
	m_validDeadband = FALSE;
}

SODmCPPItemAttributes::~SODmCPPItemAttributes()
{
}


//-----------------------------------------------------------------------------
// OnInitDialog
//
// - Initialize list controls
//
// returns:
//		return of base class
//
BOOL SODmCPPItemAttributes::OnInitDialog(void)
{
	BOOL ret = SODmCPPItem::OnInitDialog();
	CRect rect;
	m_liAccessPath.GetClientRect(&rect);
	m_liAccessPath.InsertColumn(0, _T(""), LVCFMT_LEFT, rect.Width());
	m_liAccessPath.SetBkColor(0x00CED3D6);
	m_liAccessPath.SetTextBkColor(0x00CED3D6);
	m_liEUInfo.SetBkColor(0x00CED3D6);
	m_liEUInfo.SetTextBkColor(0x00CED3D6);
	return ret;
} // OnInitDialog


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPItemAttributes::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPItem::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_ITEM_DATATYPE, m_datatype);
	DDX_Control(pDX, IDC_ITEM_DATATYPE, m_cbDatatype);
	DDX_Text(pDX, IDC_ITEM_ID, m_itemID);
	DDX_Control(pDX, IDC_ITEM_ID, m_edItemID);
	DDX_Text(pDX, IDC_ITEM_ACCESS_PATH, m_accessPath);
	DDX_Control(pDX, IDC_ITEM_ACCESS_PATH, m_edAccessPath);
	DDX_Control(pDX, IDC_ITEM_ACCESS_PATH_LIST, m_liAccessPath);
	DDX_Text(pDX, IDC_ITEM_SERVER_HANDLE, m_serverHandle);
	DDX_Control(pDX, IDC_ITEM_SERVER_HANDLE, m_edServerHandle);
	DDX_Text(pDX, IDC_ITEM_CLIENT_HANDLE, m_clientHandle);
	DDX_Control(pDX, IDC_ITEM_CLIENT_HANDLE, m_edClientHandle);
	DDX_Control(pDX, IDC_TAG_EU_INFO, m_liEUInfo);
	DDX_Text(pDX, IDC_TAG_NATIVE_DATATYPE, m_nativeDatatype);
	DDX_Control(pDX, IDC_TAG_NATIVE_DATATYPE, m_edNativeDatatype);
	DDX_Text(pDX, IDC_TAG_ACCESS_RIGHTS, m_accessRights);
	DDX_Control(pDX, IDC_TAG_ACCESS_RIGHTS, m_edAccessRights);
	DDX_Text(pDX, IDC_TAG_EU_TYPE, m_EUType);
	DDX_Control(pDX, IDC_TAG_EU_TYPE, m_edEUType);

	if (m_validDeadband)
	{
		DDX_Text(pDX, IDC_ITEM_DEADBAND, m_deadband);
	}
	else
	{
		CString dummy;
		DDX_Text(pDX, IDC_ITEM_DEADBAND, dummy);
	}

	DDX_Control(pDX, IDC_ITEM_DEADBAND, m_edDeadband);
	DDX_Check(pDX, IDC_ITEM_VALID_DEADBAND, m_validDeadband);
	DDX_Control(pDX, IDC_ITEM_VALID_DEADBAND, m_buValidDeadband);
} // DoDataExchange


//-----------------------------------------------------------------------------
// loadServerGivenAttributes
//
// - load the attributes provided by the OPC server from the object
//
void SODmCPPItemAttributes::loadServerGivenAttributes(void)
{
	if (m_obj)
	{
		OPCEUTYPE euType;
		SOCmnVariant euInfo;
		m_serverHandle.Format(_T("0x%8.8X"), m_obj->getServerHandle());
		VARTYPE natDT = m_obj->getNativeDatatype();
		SOCmnString datatype;
		::vartype2string(&datatype, natDT);
		m_nativeDatatype = (LPCTSTR)datatype;
		DWORD ar = m_obj->getAccessRights();
		SOCmnString sAr;
		::accessRights2string(&sAr, ar);
		m_accessRights = (LPCTSTR)sAr;
		m_obj->getEU(&euType, euInfo);
		m_liEUInfo.DeleteAllItems();
		m_liEUInfo.DeleteColumn(1);
		m_liEUInfo.DeleteColumn(0);

		if (euType == OPC_NOENUM)
		{
			m_EUType = _T("no");
		}
		else
		{
			// EU information
			if (euType == OPC_ANALOG)
			{
				m_EUType = _T("analog");
			}
			else
			{
				m_EUType = _T("enumerated");
			}

			CRect rect;
			m_liEUInfo.GetClientRect(&rect);
			m_liEUInfo.InsertColumn(0, _T("A"), LVCFMT_LEFT, 35);
			m_liEUInfo.InsertColumn(1, _T("B"), LVCFMT_LEFT, rect.Width() - 52);

			if ((euInfo.vt & VT_ARRAY) == VT_ARRAY)
			{
				LONG lBound, uBound;
				LONG i;
				CString conStr;
				int idx;
				SafeArrayGetLBound(euInfo.parray, 1, &lBound);
				SafeArrayGetUBound(euInfo.parray, 1, &uBound);

				for (i = lBound, idx = 0; i <= uBound; i++, idx++)
				{
					switch (euInfo.vt & ~VT_ARRAY)
					{
					case VT_R8:
					{
						double dblVal;
						SafeArrayGetElement(euInfo.parray, &i, &dblVal);

						if (i == lBound)
						{
							conStr = _T("low");
						}
						else
						{
							conStr = _T("high");
						}

						m_liEUInfo.InsertItem(idx, (LPCTSTR)conStr);
						conStr.Format(_T("%.2lf"), dblVal);
						m_liEUInfo.SetItemText(idx, 1, (LPCTSTR)conStr);
					}
					break;

					case VT_BSTR:
					{
						USES_CONVERSION;
						BSTR bstrVal = NULL;
						::SafeArrayGetElement(euInfo.parray, &i, &bstrVal);
						conStr.Format(_T("%d"), i);
						m_liEUInfo.InsertItem(idx, (LPCTSTR)conStr);
						conStr.Format(_T("%-.80s"), OLE2CT(bstrVal));
						m_liEUInfo.SetItemText(idx, 1, conStr);
						::SysFreeString(bstrVal);
					}
					break;
					}
				}
			}
		}
	}
} // loadServerGivenAttributes


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPItemAttributes::loadAttributes(void)
{
	if (m_obj)
	{
		m_itemID = (LPCTSTR)m_obj->getItemID();
		m_accessPath = (LPCTSTR)m_obj->getAccessPath();
		VARTYPE reqDT = m_obj->getReqDatatype();
		SOCmnString datatype;
		::vartype2string(&datatype, reqDT);
		m_datatype = (LPCTSTR)datatype;
		m_clientHandle.Format(_T("0x%8.8X"), m_obj->getClientHandle());
		m_deadband = m_obj->getDeadBand();
		m_validDeadband = (m_deadband != SODACITEM_NO_DEADBAND);
		loadServerGivenAttributes();
		UpdateData(FALSE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // loadAttributes


//-----------------------------------------------------------------------------
// storeAttributes
//
// - store the current attributes to the object
//
// returns:
//		TRUE  - changed attributes stored
//		FALSE - no object set
//
BOOL SODmCPPItemAttributes::storeAttributes(void)
{
	if (m_obj)
	{
		DWORD updateWhat = 0;
		BOOL reconnect = FALSE;
		UpdateData(TRUE);

		if (m_oItemID != m_itemID)
		{
			m_obj->setItemID(m_itemID);
			reconnect = TRUE;
			getApp()->setDirtyFlag();
		}

		if (m_oAccessPath != m_accessPath)
		{
			m_obj->setAccessPath(m_accessPath);
			reconnect = TRUE;
			getApp()->setDirtyFlag();
		}

		if (m_datatype != m_oDatatype)
		{
			m_obj->setReqDatatype(string2vartype(m_datatype));

			if (m_obj->isConnected())
			{
				updateWhat |= SODAC_ITMPAR_REQDATATYPE;
			}
			else
			{
				reconnect = TRUE;
			}

			getApp()->setDirtyFlag();
		}

		if (m_validDeadband)
		{
			if ((m_oDeadband != m_deadband) ||
				(m_oValidDeadband != m_validDeadband))
			{
				updateWhat |= SODAC_ITMPAR_DEADBAND;
				getApp()->setDirtyFlag();
			}

			m_obj->setDeadBand(m_deadband);
		}
		else
		{
			if (m_oValidDeadband != m_validDeadband)
			{
				m_obj->setDeadBand(SODACITEM_NO_DEADBAND);
				updateWhat |= SODAC_ITMPAR_DEADBAND;
				getApp()->setDirtyFlag();
			}
		}

		if (reconnect)
		{
			BYTE targetState = m_obj->getTargetObjectState();
			BOOL started, connected;
			connected = (targetState == ClientItem::connected);
			started = (targetState == ClientItem::started);

			if ((connected) || (started))
			{
				m_obj->disconnect();

				if (started)
				{
					m_obj->start();
				}
				else
				{
					m_obj->connect();
				}
			}
		}
		else
		{
			// write changed parameters to server
			if (updateWhat)
			{
				m_obj->update(FALSE, updateWhat);
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // storeAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPItemAttributes::clearAttributes(void)
{
	m_itemID.Empty();
	m_accessPath.Empty();
	m_datatype.Empty();
	m_serverHandle.Empty();
	m_clientHandle.Empty();
	m_nativeDatatype.Empty();
	m_accessRights.Empty();
	m_EUType.Empty();
	m_deadband = 0.0;
	m_validDeadband = FALSE;
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPItemAttributes::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oDatatype = m_datatype;
	m_oItemID = m_itemID;
	m_oAccessPath = m_accessPath;
	m_oDeadband = m_deadband;
	m_oValidDeadband = m_validDeadband;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPItemAttributes::resetCurrentAttributes(void)
{
	m_datatype = m_oDatatype;
	m_itemID = m_oItemID;
	m_accessPath = m_oAccessPath;
	m_deadband = m_oDeadband;
	m_validDeadband = m_oValidDeadband;
	UpdateData(FALSE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPItemAttributes::enableControls(IN BOOL enable)  // enable or disable
{
	BOOL privateGrp = FALSE;

	if ((m_obj) && (enable))
	{
		SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)m_obj->getParentBranch();

		if (!grp)
		{
			return;
		}

		privateGrp = !grp->getPublic();
	}

	enableButton(ID_PROPERTY_APPLY, enable);
	enableButton(ID_PROPERTY_RESET, enable);
	m_edItemID.SetReadOnly(!privateGrp);
	m_edAccessPath.SetReadOnly(!privateGrp);
	m_cbDatatype.EnableWindow(enable);
	SOCmnPointer<SODaCServer> server = getServerDA(m_obj);
	BOOL enableDB = FALSE;

	if (server.isNotNull())
	{
		enableDB = (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30);
	}

	if (!enableDB)
	{
		m_buValidDeadband.EnableWindow(FALSE);
	}

	m_edDeadband.SetReadOnly(!m_buValidDeadband.GetCheck());

	if (privateGrp)
	{
		m_liAccessPath.SetBkColor(0x00FFFFFF);
		m_liAccessPath.SetTextBkColor(0x00FFFFFF);
	}
	else
	{
		m_liAccessPath.SetBkColor(0x00CED3D6);
		m_liAccessPath.SetTextBkColor(0x00CED3D6);
	}

	m_liAccessPath.Invalidate();

	if ((privateGrp) || ((m_obj) && m_obj->isConnected()))
	{
		SOCmnPointer<SODaCServer> server = connectServerDA(m_obj);
		insertAccessPaths();
	}
} // enableControls


//-----------------------------------------------------------------------------
// onChangeID
//
// - changed item id in edit field
// - load server provided attributes for new item (if connected)
//
void SODmCPPItemAttributes::onChangeID()
{
	UpdateData(TRUE);
	loadServerGivenAttributes();
	insertAccessPaths();
	UpdateData(FALSE);
} // onChangeID


//-----------------------------------------------------------------------------
// onClickDeadband
//
// - changed deadband check button
//
void SODmCPPItemAttributes::onClickDeadband()
{
	UpdateData(TRUE);
	m_edDeadband.SetReadOnly(!m_buValidDeadband.GetCheck());

	if (m_buValidDeadband.GetCheck())
	{
		m_deadband = 0.0;
	}
	else
	{
		m_deadband = SODACITEM_NO_DEADBAND;
	}

	UpdateData(FALSE);
} // onClickDeadband


//-----------------------------------------------------------------------------
// insertAccessPaths
//
// - insert the access path list into the list control
//
void SODmCPPItemAttributes::insertAccessPaths(void)
{
	m_liAccessPath.DeleteAllItems();
	SOCmnPointer<SODaCServer> server = getServerDA(m_obj);

	if (!((SODaCServer*)server))
	{
		return;
	}

	if (!server->isConnected())
	{
		return;
	}

#ifdef SODMC_DCOM
	SODmCComServerDA* pComSrv = NULL;

	if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComSrv = (SODmCComServerDA*)(SODaCServer*)server;
	}

	if (pComSrv)
	{
		SOCmnStringList accessPaths;

		if (SUCCEEDED(pComSrv->browseAccessPaths(m_itemID, accessPaths)))
		{
			SOCmnListPosition pos;
			int index;
			pos = accessPaths.GetHeadPosition();
			index = 0;

			while (pos)
			{
				SOCmnString path = accessPaths.GetNext(pos);
				m_liAccessPath.InsertItem(index, (LPCTSTR)path);
			}
		}
	}

#endif // SODMC_DCOM
} // insertAccessPaths


//-----------------------------------------------------------------------------
// onDblclkAccessPath
//
// - double click to access path list control
// - change access path attribute of the item
//
void SODmCPPItemAttributes::onDblclkAccessPath(NMHDR* pNMHDR, LRESULT* pResult)
{
	COLORREF ref;
	ref = m_liAccessPath.GetBkColor();

	if (0x00FFFFFF == ref)
	{
		int index = -1;
		UINT state;

		while ((index = m_liAccessPath.GetNextItem(index, LVNI_ALL)) > -1)
		{
			state = m_liAccessPath.GetItemState(index, LVIS_SELECTED);

			if (state & LVIS_SELECTED)
			{
				// item is selected
				m_accessPath = m_liAccessPath.GetItemText(index, 0);
				UpdateData(FALSE);
				break;
			}
		}
	}

	*pResult = 0;
} // onDblclkAccessPath


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - object state has changed
//
void SODmCPPItemAttributes::onSetObjectState(IN BYTE state) // new object state
{
	if (state != SOCltElement::disconnected)
	{
		loadServerGivenAttributes();
		insertAccessPaths();
	}
	else
	{
		m_serverHandle.Empty();
		m_clientHandle.Empty();
		m_nativeDatatype.Empty();
		m_accessRights.Empty();
		m_EUType.Empty();
		m_liEUInfo.DeleteAllItems();
		m_liEUInfo.DeleteColumn(1);
		m_liEUInfo.DeleteColumn(0);
	}

	UpdateData(FALSE);
} // onSetObjectState




