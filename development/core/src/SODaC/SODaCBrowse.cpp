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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCBrowse.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA Browsing objects                                         |
//                - SODaCNameSpaceBrowser: name space browsing root object    |
//                - SODaCNode: node browser object class                      |
//                - SODaCTag: tag browser object class                        |
//                - SODaCProperty: property browser object class              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#include "SODaCBrowse.h"
#include "SODaCServer.h"


//-----------------------------------------------------------------------------
// SODaCNameSpaceBrowser                                                      |
//-----------------------------------------------------------------------------

SODaCNameSpaceBrowser::SODaCNameSpaceBrowser(SODaCServer* pServer)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER)
{
	m_datatypeFilter = VT_EMPTY;
	m_accessRightsFilter = 0;
	m_elementNameFilter = _T("");
	m_vendorFilter = _T("");
	m_maxElements = 0;
	m_returnProps = TRUE;
	m_retrievePropValues = FALSE;
	m_retrieveItemID = FALSE;
	m_server = pServer;
	m_server.addRef();
	m_delimNScreate = _T('\x7f');
	m_delimNSaccept = _T("\x7f");
	m_badNSchars = _T("");
}

SODaCNameSpaceBrowser::~SODaCNameSpaceBrowser()
{
}


void SODaCNameSpaceBrowser::resetFilter(void)
{
	m_datatypeFilter = VT_EMPTY;
	m_accessRightsFilter = 0;
	m_elementNameFilter = _T("");
	m_vendorFilter = _T("");
	initUse();
}

BOOL SODaCNameSpaceBrowser::setFilter(LPCTSTR elementNameFilter, DWORD accessRightsFilter, VARTYPE datatypeFilter)
{
	if (!support20())
	{
		return FALSE;
	}

	if (elementNameFilter)
	{
		m_elementNameFilter = elementNameFilter;
	}
	else
	{
		m_elementNameFilter = _T("");
	}

	m_datatypeFilter = datatypeFilter;
	m_accessRightsFilter = accessRightsFilter;
	use20();
	return TRUE;
}

BOOL SODaCNameSpaceBrowser::setFilter(LPCTSTR elementNameFilter, LPCTSTR vendorFilter)
{
	if (!support30())
	{
		return FALSE;
	}

	if (elementNameFilter)
	{
		m_elementNameFilter = elementNameFilter;
	}
	else
	{
		m_elementNameFilter = _T("");
	}

	if (vendorFilter)
	{
		m_vendorFilter = vendorFilter;
	}
	else
	{
		m_vendorFilter = _T("");
	}

	use30();
	return TRUE;
}


//-----------------------------------------------------------------------------
// expand
//
BOOL SODaCNameSpaceBrowser::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	BYTE os = getObjectState();
	BOOL result = FALSE;
	switch (os)
	{
	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES:
	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE:
		result = expand20(this , _T(""), type, objList);
		break;

	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE:
		{
			SOCmnString contPoint = getContinuationPoint();
			result = expand30(this, _T(""), _T(""), type, contPoint, objList);
			setContinuationPoint(contPoint);
		}
		break;
	default:
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("There is no interface to perform the browse operation.")));
		break;
	}

	return result;
}


SOCmnString SODaCNameSpaceBrowser::getDisplayName()
{
	_ASSERTION(!!m_server, "No server object");
	return m_server->getServerName();
}

//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SODaCNameSpaceBrowser::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	if (accept)
	{
		*accept = m_delimNSaccept;
	}

	if (bad)
	{
		*bad = m_badNSchars;
	}

	return m_delimNScreate;
} // getTreeDelimiter

SODaCNameSpaceBrowser* SODaCNameSpaceBrowser::getObjectSODaCNameSpaceBrowser(void)
{
	return this;
}


SODaCNameSpaceBrowserICommunication::SODaCNameSpaceBrowserICommunication(void) {}
SODaCNameSpaceBrowserICommunication::~SODaCNameSpaceBrowserICommunication(void) {}
BOOL SODaCNameSpaceBrowserICommunication::expand20(IN SOCltBrowseObject* /* pParent */, IN LPCTSTR /* itemID */, IN DWORD /* type */, OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}
BOOL SODaCNameSpaceBrowserICommunication::expand30(IN SOCltBrowseObject* /* pParent */, IN LPCTSTR /* itemID */, IN LPCTSTR /* itemPath */, IN DWORD /* type */, IN SOCmnString& /* contPoint */, OUT SOCmnList<SOCltBrowseObject>& /*objList */)
{
	return FALSE;
}
LPCTSTR SODaCNameSpaceBrowserICommunication::retrieveItemID(SOCltBrowseObject* /* pObj */)
{
	return NULL;
}
void SODaCNameSpaceBrowserICommunication::initUse(void) {}
void SODaCNameSpaceBrowserICommunication::use20(void) {}
void SODaCNameSpaceBrowserICommunication::use30(void) {}
BOOL SODaCNameSpaceBrowserICommunication::support20(void)
{
	return FALSE;
}
BOOL SODaCNameSpaceBrowserICommunication::support30(void)
{
	return FALSE;
}
SODaCNameSpaceBrowser* SODaCNameSpaceBrowserICommunication::getObjectSODaCNameSpaceBrowser(void)
{
	return NULL;
}


//-----------------------------------------------------------------------------
// SODaCNode                                                                  |
//-----------------------------------------------------------------------------

SODaCNode::SODaCNode(IN SODaCNameSpaceBrowser* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_NODE)
{
	m_browser = parent;
	m_browser.addRef();
	m_name = wszName;
	m_path.create();
	m_flags = 0;
}

SODaCNode::SODaCNode(IN SODaCNode* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_NODE)
{
	m_browser = parent->getNameSpaceBrowser();
	m_name = wszName;
	// child path = path + name
	m_path.clone(&parent->m_path);
	m_path.add(parent->m_name);
	m_flags = 0;
}

SODaCNode::SODaCNode(IN SOCltBrowseObject* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_NODE)
{
	if (parent->is(SOCMNOBJECT_TYPE_NODE))
	{
		m_browser = ((SODaCNode*)parent)->getNameSpaceBrowser();
		m_path.clone(&((SODaCNode*)parent)->m_path);
		m_path.add(parent->getName());
	}
	else if (parent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))
	{
		m_browser = (SODaCNameSpaceBrowser*)parent;
		m_browser.addRef();
		m_path.create();
	}
	else
	{
		return;
	}

	m_name = wszName;
	m_flags = 0;
}

SODaCNode::~SODaCNode()
{
}

SOCmnList<SODaCProperty> *SODaCNode::getPropertyList(void)
{
	if (!m_propList.isInit())
	{
		m_propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	return &m_propList;
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SODaCNode::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	BYTE os = m_browser->getObjectState();
	BOOL result = FALSE;

	switch (os)
	{
	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES:
	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE:
		result = m_browser->expand20(this, getItemID(FALSE), type, objList);
		break;

	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE:
		{
			SOCmnString contPoint = m_browser->getContinuationPoint();
			result = m_browser->expand30(this, getItemID(), getItemPath(), type, contPoint, objList);
			m_browser->setContinuationPoint(contPoint);
		}
		break;
	default:
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("There is no interface to perform the browse operation.")));
		break;
	}

	return result;
}


//-----------------------------------------------------------------------------
//
// getPath
//	Returns the fully qualified name

SOCmnString SODaCNode::getPath()
{
	if (m_itemID.isEmpty())
	{
		m_browser->retrieveItemID(this);
	}

	return m_itemID;
}



//-----------------------------------------------------------------------------
// SODaCTag                                                                   |
//-----------------------------------------------------------------------------

SODaCTag::SODaCTag(IN SODaCNameSpaceBrowser* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_TAG)
{
	m_browser = parent;
	m_browser.addRef();
	m_name = wszName;
	m_path.create();
	m_flags = 0;
}


SODaCTag::SODaCTag(IN SODaCNode* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_TAG)
{
	m_browser = parent->getNameSpaceBrowser();
	m_name = wszName;
	// child path = path + name
	m_path.clone(parent->getPathList());
	m_path.add(parent->getName());
	m_flags = 0;
}

SODaCTag::SODaCTag(IN SOCltBrowseObject* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_TAG)
{
	if (parent->is(SOCMNOBJECT_TYPE_NODE))
	{
		m_browser = ((SODaCNode*)parent)->getNameSpaceBrowser();
		m_path.clone(((SODaCNode*)parent)->getPathList());
		m_path.add(parent->getName());
	}
	else if (parent->is(SOCMNOBJECT_TYPE_NAMESPACE_BROWSER))
	{
		m_browser = (SODaCNameSpaceBrowser*)parent;
		m_browser.addRef();
		m_path.create();
	}
	else if (parent->is(SOCMNOBJECT_TYPE_TAG))
	{
		m_browser = ((SODaCTag*)parent)->getNameSpaceBrowser();
		m_path.clone(((SODaCTag*)parent)->getPathList());
		m_path.add(parent->getName());
	}
	else
	{
		_ASSERTION(FALSE, _T("SODaCTag without m_browser"));
	}

	m_name = wszName;
	m_flags = 0;
}

SODaCTag::~SODaCTag()
{
}

SOCmnList<SODaCProperty> *SODaCTag::getPropertyList(void)
{
	if (!m_propList.isInit())
	{
		m_propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	return &m_propList;
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SODaCTag::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	if (!m_browser)
	{
		return FALSE;
	}

	BYTE os = m_browser->getObjectState();
	BOOL result = FALSE;

	switch (os)
	{
	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSESERVERADDRESSSPACE_AND_IOPCITEMPROPERTIES:
		result = m_browser->expand20(this, getItemID(FALSE), type, objList);
		break;

	case SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE:
		{
			SOCmnString contPoint = m_browser->getContinuationPoint();
			result = m_browser->expand30(this, getItemID(), getItemPath(), type, contPoint, objList);
			m_browser->setContinuationPoint(contPoint);
		}
		break;
	default:
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("There is no interface to perform the browse operation.")));
		break;
	}

	return result;
}


//-----------------------------------------------------------------------------
//
// getPath
//	Returns the fully qualified name

SOCmnString SODaCTag::getPath()
{
	if (m_itemID.isEmpty())
	{
		m_browser->retrieveItemID(this);
	}

	return m_itemID;
}




//-----------------------------------------------------------------------------
// SODaCProperty                                                              |
//-----------------------------------------------------------------------------


SODaCProperty::SODaCProperty(IN SODaCNode* parent, LPCTSTR name, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_PROPERTY)
{
	m_browser = parent->getNameSpaceBrowser();
	_ASSERTION(m_browser.isNotNull(), _T("parent browser illegal"));
	setElementName(propID);
	setElementName(name);
	m_dataType = datatype;
	m_parentID = szParentID;
	m_parentPath = szParentPath;
}

SODaCProperty::SODaCProperty(IN SODaCTag* parent, LPCTSTR name, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_PROPERTY)
{
	m_browser = parent->getNameSpaceBrowser();
	_ASSERTION(m_browser.isNotNull(), _T("parent browser illegal"));
	setElementName(propID);
	setElementName(name);
	m_dataType = datatype;
	m_parentID = szParentID;
	m_parentPath = szParentPath;
}

SODaCProperty::SODaCProperty(IN SOCltBrowseObject* parent, LPCTSTR name, DWORD propID, VARTYPE datatype, LPCTSTR szParentID, LPCTSTR szParentPath)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_PROPERTY)
{
	m_browser = (SODaCNameSpaceBrowser*)parent->getBrowser();
	_ASSERTION(m_browser.isNotNull(), _T("parent browser illegal"));
	setElementName(propID);
	setElementName(name);
	m_dataType = datatype;
	m_parentID = szParentID;
	m_parentPath = szParentPath;
}

SODaCProperty::~SODaCProperty()
{
}


//-----------------------------------------------------------------------------
//
// expand
//

BOOL SODaCProperty::expand(IN DWORD /* type */, OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}

//-----------------------------------------------------------------------------
//
// getPath
//

SOCmnString SODaCProperty::getPath()
{
	if (m_itemID.isEmpty())
	{
		m_browser->retrieveItemID(this);
	}

	return m_itemID;
}

LPCTSTR SODaCProperty::getStandardPropertyNameByID(IN DWORD propID, OUT SOCmnString& propName)
{
	switch (propID)
	{
	case 1:
		propName = _T("dataType");
		break;

	case 2:
		propName = _T("value");
		break;

	case 3:
		propName = _T("quality");
		break;

	case 4:
		propName = _T("timestamp");
		break;

	case 5:
		propName = _T("accessRights");
		break;

	case 6:
		propName = _T("scanRate");
		break;

	case 7:
		propName = _T("euType");
		break;

	case 8:
		propName = _T("euInfo");
		break;

	case 100:
		propName = _T("engineeringUnits");
		break;

	case 101:
		propName = _T("description");
		break;

	case 102:
		propName = _T("highEU");
		break;

	case 103:
		propName = _T("lowEU");
		break;

	case 104:
		propName = _T("highIR");
		break;

	case 105:
		propName = _T("lowIR");
		break;

	case 106:
		propName = _T("closeLabel");
		break;

	case 107:
		propName = _T("openLabel");
		break;

	case 108:
		propName = _T("timeZone");
		break;

	default:
		propName.format(_T("%lu"), propID);
		break;
	}

	return (LPCTSTR)propName;
}

void SODaCProperty::convertPropertyValueFromSOAPtoDCOM(IN DWORD propID, IN OUT SOCmnVariant& value)
{
	switch (propID)
	{
	case 1: // dataType
	{
		WORD datatype = VT_EMPTY;
		SOCmnString dtSZ;
		value.getBSTR(dtSZ);
		SOCmnVariant::getVARTYPE(dtSZ, datatype);
		value.setUI2(datatype);
		break;
	}

	case 5: // accessRights
	{
		DWORD accessRights = OPC_READABLE | OPC_WRITEABLE;
		SOCmnString arSZ;
		value.getBSTR(arSZ);

		if (arSZ.find(_T("ead")) == -1)
		{
			accessRights &= ~OPC_READABLE;
		}

		if (arSZ.find(_T("rit")) == -1)
		{
			accessRights &= ~OPC_WRITEABLE;
		}

		value.setUI4(accessRights);
		break;
	}

	case 7: //euType
	{
		DWORD euType = OPC_NOENUM;
		SOCmnString euSZ;
		value.getBSTR(euSZ);

		if (euSZ == _T("analog"))
		{
			euType = OPC_ANALOG;
		}
		else if (euSZ == _T("enumerated"))
		{
			euType = OPC_ENUMERATED;
		}

		value.setI4(euType);
		break;
	}
	}
}

#endif // SODAC
