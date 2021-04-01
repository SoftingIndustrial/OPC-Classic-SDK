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
//  Filename    : SODmCNameSpaceDA.cpp                                        |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA name space classes                                       |
//                - SODmCPPProperties: Name space element properties          |
//                                     property page                          |
//                - SODmCPPProperty: Property property page                   |
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
#include "SODmCPropFrm.h"
#include "SODmCNameSpaceDA.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"
#include "SODmCServerDA.h"


//-----------------------------------------------------------------------------
// SODmCPPTagWrite                                                            |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPTagWrite, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPTagWrite, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPTagWrite::SODmCPPTagWrite(IN SODmCPSheet* sheet)
	: SODmCPPage(sheet, IDD_PP_TAG_WRITE)
{
	m_obj = NULL;
	m_datatype = VT_EMPTY;
	m_accessRights = 0;
	m_edReadValue.m_firstControl = TRUE;
	m_edWriteValue.m_lastControl = TRUE;
}

SODmCPPTagWrite::~SODmCPPTagWrite()
{
}

//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPTagWrite::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TAG_READ_VALUE, m_readValue);
	DDX_Control(pDX, IDC_TAG_READ_VALUE, m_edReadValue);
	DDX_Text(pDX, IDC_TAG_READ_QUALITY, m_readQuality);
	DDX_Control(pDX, IDC_TAG_READ_QUALITY, m_edReadQuality);
	DDX_Text(pDX, IDC_TAG_READ_TIMESTAMP, m_readTimestamp);
	DDX_Control(pDX, IDC_TAG_READ_TIMESTAMP, m_edReadTimestamp);
	DDX_Text(pDX, IDC_TAG_WRITE_VALUE, m_writeValue);
	DDX_Control(pDX, IDC_TAG_WRITE_VALUE, m_edWriteValue);
} // DoDataExchange


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPTagWrite::loadAttributes(void)
{
	if (m_obj)
	{
		SOCmnPointer<SODaCServer> server = connectServerDA((SOCmnElement*)m_obj);
		HRESULT res;

		if ((SODaCServer*)server)
		{
			if (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
			{
				SOCmnVariant value;
				WORD quality;
				SOCmnDateTime timestamp;
				SOCmnString itemID = m_obj->getPath();
				SOCmnString itemPath = m_obj->getItemPath();
				res = server->read(1, &itemID, &itemPath, SODAC_MAXAGE_DEVICE, &value, &quality, &timestamp);

				if (res == S_OK)
				{
					SOCmnString str;
					m_datatype = value.vt;
					m_readValue = VARIANT2STRING(&str, value);
					m_readQuality = quality2string(&str, quality);
					m_readTimestamp = timestamp.toString(str);
				}

				m_accessRights = 0;
				SODaCPropertyData* propData = NULL;
				SOCmnList<SODaCPropertyData> propertyList;
				propertyList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
				SOCMN_ALLOCATE_OBJECT(propData, SODaCPropertyData);
				propData->m_propertyID = 5;
				propData->m_name = _T("accessRights");
				propertyList.add(propData);

				if (SUCCEEDED(server->getPropertiesData(itemID, (LPCTSTR)m_obj->getItemPath(), SODAC_PROPDATA_VALUE, propertyList)))
				{
					if (server->getProtocol() != SOCLT_PROTOCOL_SOAP)
					{
						if (propData->m_value.changeDatatype(VT_I4))
						{
							if (propData->m_value.lVal & OPC_WRITEABLE)
							{
								m_edWriteValue.SetReadOnly(FALSE);
							}

							m_accessRights = propData->m_value.lVal;
						}
					}
					else if (propData->m_value.vt == VT_BSTR)
					{
						SOCmnString val(propData->m_value.bstrVal);

						if (val == _T("readWritable"))
						{
							m_edWriteValue.SetReadOnly(FALSE);
							m_accessRights = OPC_READABLE | OPC_WRITEABLE;
						}
						else if (val == _T("readable"))
						{
							m_accessRights = OPC_READABLE;
						}
						else if (val == _T("writable"))
						{
							m_edWriteValue.SetReadOnly(FALSE);
							m_accessRights = OPC_WRITEABLE;
						}
					}
				}

				UpdateData(FALSE);
				return (res == S_OK);
			}
		}
	}

	return FALSE;
} // loadAttributes


//-----------------------------------------------------------------------------
// onOK
//
// - write the value to the OPC server
//
void SODmCPPTagWrite::onOK(void)
{
	if (m_obj)
	{
		SOCmnPointer<SODaCServer> server = connectServerDA((SOCmnElement*)m_obj);
		HRESULT res = S_OK;

		if ((SODaCServer*)server)
		{
			if (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
			{
				UpdateData(TRUE);   // get the value string form the edit box

				if (!m_writeValue.IsEmpty())
				{
					SOCmnVariant value;
					SOCmnString itemID = m_obj->getPath();
					SOCmnString itemPath = m_obj->getItemPath();

					if (convertWriteStringToVariant(server, m_writeValue, m_datatype, 0, value))
					{
						res = server->write(1, &itemID, &itemPath, &value);
					}
				}

				if (res == S_OK)
				{
					loadAttributes();
				}
			}
		}
	}
} // onOK


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPTagWrite::enableControls(IN BOOL enable)    // enable or disable
{
	BOOL connected = FALSE;
	BOOL array = FALSE;
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, FALSE);

	if (m_obj)
	{
		SOCmnPointer<SODaCServer> server = connectServerDA((SOCmnElement*)m_obj);

		if ((SODaCServer*)server)
		{
			if (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
			{
				enableButton(ID_PROPERTY_APPLY, TRUE);
			}
		}
	}
} // enableControls




//-----------------------------------------------------------------------------
// SODmCPPProperties                                                          |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPProperties, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPProperties, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPProperties::SODmCPPProperties(IN SODmCPSheet* sheet) // property sheet
	: SODmCPPage(sheet, IDD_PP_TAG)
{
	m_obj = NULL;
	m_liProperties.m_firstControl = TRUE;
	m_liProperties.m_lastControl = TRUE;
}

SODmCPPProperties::~SODmCPPProperties()
{
}


//-----------------------------------------------------------------------------
// OnInitDialog
//
// - Initialize list control
//
// returns:
//		return of base class
//
BOOL SODmCPPProperties::OnInitDialog(void)
{
	BOOL ret = SODmCPPage::OnInitDialog();
	CRect rect;
	m_liProperties.GetClientRect(&rect);
	m_liProperties.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 30);
	m_liProperties.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 70);
	m_liProperties.InsertColumn(2, _T("Value"), LVCFMT_LEFT, 150);
	m_liProperties.InsertColumn(3, _T("Datatype"), LVCFMT_LEFT, 70);
	m_liProperties.InsertColumn(4, _T("Description"), LVCFMT_LEFT, rect.Width() - 268);
	m_liProperties.SetBkColor(0x00CED3D6);
	m_liProperties.SetTextBkColor(0x00CED3D6);
	return ret;
} // OnInitDialog


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPProperties::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROPERTY_LIST, m_liProperties);
} // DoDataExchange


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPProperties::loadAttributes(void)
{
	if (m_obj)
	{
		getProperties();
		UpdateData(FALSE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // loadAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPProperties::clearAttributes(void)
{
	m_liProperties.DeleteAllItems();
} // clearAttributes


//-----------------------------------------------------------------------------
// onOK
//
// - reload attributes on OK
//
void SODmCPPProperties::onOK(void)
{
	loadAttributes();
} // onOK


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPProperties::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// getProperties
//
// - get the item properties from the OPC server
//
void SODmCPPProperties::getProperties(void)
{
	SOCmnPointer<SODaCServer> server = connectServerDA((SOCmnElement*)m_obj);
	SOCmnList<SODaCPropertyData> propertyList;
	HRESULT res = E_FAIL;
	m_liProperties.DeleteAllItems();

	if ((SODaCServer*)server)
	{
		if (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
		{
			res = server->queryAvailableProperties((LPCTSTR)m_obj->getPath(), (LPCTSTR)m_obj->getItemPath(), SODAC_PROPDATA_ALL, propertyList);
		}
		else if (server->getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20)
		{
			// get all available properties
			if (SUCCEEDED(server->queryAvailableProperties((LPCTSTR)m_obj->getPath(), _T(""), SODAC_PROPDATA_NONE, propertyList)))
			{
				if (propertyList.GetCount() > 0)
				{
					// get the properties data (value)
					res = server->getPropertiesData((LPCTSTR)m_obj->getPath(), _T(""), SODAC_PROPDATA_VALUE, propertyList);
				}
			}
		}

		if (SUCCEEDED(res))
		{
			BOOL isSOAP = server->getSupportedOPCVersion() == SODAC_VERSION_OPC_XMLDAV10;
			SODaCPropertyData* propertyData;
			SOCmnListPosition pos;
			int index, j;
			CString conv;
			SOCmnString dllStr;
			DWORD propertyID;
			pos = propertyList.GetHeadPosition();
			index = 0;

			while (pos)
			{
				propertyData = propertyList.GetNext(pos);

				for (j = -1; ((j = m_liProperties.GetNextItem(j, LVNI_ALL)) > -1);)
				{
					propertyID = _tcstoul(m_liProperties.GetItemText(j, 0), NULL, 0);

					if (propertyID > propertyData->m_propertyID)
					{
						index = j;
						break;
					}
				}

				if (j <= -1)
				{
					index = m_liProperties.GetItemCount();
				}

				if (propertyData->m_propertyID != 0xFFFFFFFF)
				{
					conv.Format(_T("%lu"), propertyData->m_propertyID);
				}
				else
				{
					conv.Empty();
				}

				m_liProperties.InsertItem(index, (LPCTSTR)conv);

				if (!propertyData->m_name.isEmpty())
				{
					m_liProperties.SetItemText(index, 1, (LPCTSTR)propertyData->m_name);
				}

				if (SUCCEEDED(propertyData->m_error))
				{
					switch (propertyData->m_propertyID)
					{
					case 1:
						if (!isSOAP)
						{
							::vartype2string(&dllStr, propertyData->m_value.iVal);
						}
						else
						{
							VARIANT2STRING(&dllStr, &propertyData->m_value);
						}

						break;

					case 3:
						::quality2string(&dllStr, propertyData->m_value.iVal);
						break;

					case 5:
						if (!isSOAP)
						{
							::accessRights2string(&dllStr, propertyData->m_value.lVal);
						}
						else
						{
							VARIANT2STRING(&dllStr, &propertyData->m_value);
						}

						break;

					default:
						VARIANT2STRING(&dllStr, &propertyData->m_value);
						break;
					}

					m_liProperties.SetItemText(index, 2, dllStr);
				}

				m_liProperties.SetItemText(index, 3, ::vartype2string(&dllStr, propertyData->m_datatype));

				if (!propertyData->m_description.isEmpty())
				{
					m_liProperties.SetItemText(index, 4, (LPCTSTR)propertyData->m_description);
				}

				index++;
			}
		}
	}
} // getProperties




//-----------------------------------------------------------------------------
// SODmCPPProperty                                                            |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPProperty, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPProperty, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPProperty::SODmCPPProperty(IN SODmCPSheet* sheet) // property sheet
	: SODmCPPage(sheet, IDD_PP_PROPERTY)
{
	m_obj = NULL;
	m_edItemID.m_firstControl = TRUE;
	m_edDatatype.m_lastControl = TRUE;
}

SODmCPPProperty::~SODmCPPProperty()
{
}


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPProperty::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROPERTY_ITEM_ID, m_itemID);
	DDX_Control(pDX, IDC_PROPERTY_ITEM_ID, m_edItemID);
	DDX_Text(pDX, IDC_PROPERTY_PROP_ID, m_propID);
	DDX_Control(pDX, IDC_PROPERTY_PROP_ID, m_edPropID);
	DDX_Text(pDX, IDC_PROPERTY_PROP_NAME, m_propName);
	DDX_Control(pDX, IDC_PROPERTY_PROP_NAME, m_edPropName);
	DDX_Text(pDX, IDC_PROPERTY_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_PROPERTY_DESCRIPTION, m_edDescription);
	DDX_Text(pDX, IDC_PROPERTY_VALUE, m_value);
	DDX_Control(pDX, IDC_PROPERTY_VALUE, m_edValue);
	DDX_Text(pDX, IDC_PROPERTY_DATATYPE, m_datatype);
	DDX_Control(pDX, IDC_PROPERTY_DATATYPE, m_edDatatype);
} // DoDataExchange


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPProperty::loadAttributes(void)
{
	if (m_obj)
	{
		getProperty();
		UpdateData(FALSE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // loadAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPProperty::clearAttributes(void)
{
	m_itemID.Empty();
	m_propID = 0;
	m_description.Empty();
	m_value.Empty();
	m_datatype.Empty();
	m_propName.Empty();
} // clearAttributes


//-----------------------------------------------------------------------------
// onOK
//
// - reload attributes on OK
//
void SODmCPPProperty::onOK(void)
{
	loadAttributes();
} // onOK


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPProperty::enableControls(IN BOOL enable)    // enable or disable
{
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// getProperties
//
// - get the item properties from the OPC server
//
void SODmCPPProperty::getProperty(void)
{
	SOCmnPointer<SODaCServer> server = connectServerDA((SOCmnElement*)m_obj);

	if ((SODaCServer*)server)
	{
		if (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
		{
			SOCmnList<SODaCPropertyData> propertyList;
			SODaCPropertyData* propData = NULL;
			propertyList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			SOCMN_ALLOCATE_OBJECT(propData, SODaCPropertyData);
			propData->m_propertyID = m_obj->getPropertyID();
			propData->m_name = m_obj->getPropertyName();
			propertyList.add(propData);

			if (SUCCEEDED(server->getPropertiesData((LPCTSTR)m_obj->getParentItemID(), (LPCTSTR)m_obj->getParentItemPath(), SODAC_PROPDATA_ALL, propertyList)))
			{
				SOCmnString str;

				if (propData->m_propertyID != 0xFFFFFFFF)
				{
					m_propID = propData->m_propertyID;
				}
				else
				{
					m_propID = 0;
				}

				m_itemID = propData->m_itemID;
				m_propName = propData->m_name;
				m_description = propData->m_description;
				m_datatype = vartype2string(&str, propData->m_datatype);
				m_value = VARIANT2STRING(&str, propData->m_value);
			}
		}
		else if (server->getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20)
		{
			// get properties only from V2.0 pComSrvs
			SOCmnList<SODaCPropertyData> propertyList;

			// get all available properties
			if (SUCCEEDED(server->queryAvailableProperties((LPCTSTR)m_obj->getParentItemID(), _T(""), SODAC_PROPDATA_NONE, propertyList)))
			{
				SODaCPropertyData* propertyData;
				SOCmnListPosition pos, curPos;
				pos = propertyList.getStartPosition();

				while (pos)
				{
					curPos = pos;
					propertyData = propertyList.getNext(pos);

					if (m_obj->getPropertyID() != propertyData->m_propertyID)
					{
						propertyList.removeAt(curPos);
					}
				}

				if (!propertyList.isEmpty())
				{
					server->getPropertiesData((LPCTSTR)m_obj->getParentItemID(), _T(""), SODAC_PROPDATA_VALUE, propertyList);
					server->getPropertiesData((LPCTSTR)m_obj->getParentItemID(), _T(""), SODAC_PROPDATA_ITEMID, propertyList);
					SOCmnString str;
					propertyData = propertyList.getHead();
					m_itemID = propertyData->m_itemID;
					m_propID = propertyData->m_propertyID;
					m_description = propertyData->m_description;
					m_datatype = vartype2string(&str, propertyData->m_datatype);
					m_value = VARIANT2STRING(&str, propertyData->m_value);
				}
			}
		}
	}
} // getProperties


