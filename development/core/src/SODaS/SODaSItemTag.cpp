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
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSItemTag.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Item tag classes                                            |
//                - SODaSItemTag: base class for tag and property classes     |
//                - SODaSItemTagIConnect: I/O connection data interface       |
//                - SODaSItemTagConnectQueueStore: store queued I/O mode data |
//                - SODaSItemTagConnectItemsStore: store items of the item tag|
//                    for event driven I/O                                    |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSItemTag.h"
#include "SODaSCache.h"
#include "SOCmnTrace.h"
#include "SODaSEntry.h"
#include "SODaSItem.h"
#include "SODaSNode.h"
#include "SODaSServer.h"
#include <opcerror.h>


//-----------------------------------------------------------------------------
// SODaSItemTag                                                               |
//-----------------------------------------------------------------------------


SODaSItemTag::SODaSItemTag(
	IN DWORD objType)   // generic object type
	: SOCmnElement(SOCMNOBJECT_TYPE_ITEM_TAG | objType)
{
	m_accessRightsAndDynamic = OPC_READABLE | SODASITEMTAG_MASK_DEFAULTACCESSRIGHTS;
	m_nativeDatatype = VT_EMPTY;
	m_cache = NULL;
	m_deviceIOAndUpdateMode = 0;
}

SODaSItemTag::~SODaSItemTag(void)
{
	setCache(NULL);
}


//-----------------------------------------------------------------------------
// setName
//
// - set element name
// - check if name is valid
// - add name to parents child name list
//
// returns:
//      S_OK                - set name
//      E_INVALIDARG        - no valid name or parameter
//      OPC_E_DUPLICATENAME - no unique name
//
HRESULT SODaSItemTag::setName(
	IN LPCTSTR name)    // name
{
	HRESULT res;
	res = SOCmnElement::setName(name);
	return res;
} // setName


//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//      primary tree delimiter
//
TCHAR SODaSItemTag::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root = entry->getNameSpaceRoot();

	if (root.isNotNull())
	{
		return root->getTreeDelimiter(accept, bad);
	}

	return SOCmnElement::s_delimCreate;
} // getTreeDelimiter


//-----------------------------------------------------------------------------
// getCache
//
// - create and initialize cache object if NULL
// - get cache object
//
// returns:
//      pointer to cache object
//
SODaSCache* SODaSItemTag::getCache(void)
{
	if (getDeviceIOMode(NULL) == SODaSItemTag::noDeviceIO)
	{
		return NULL;
	}

	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (!m_cache)
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSCreator> creator = engine->getCreator();
		m_cache = creator->createCache(this);

		if (m_cache)
		{
			m_cache->setDatatype(m_nativeDatatype);
			m_cache->init();
		}
	}

	if (m_cache)
	{
		m_cache->addRef();
	}

	return m_cache;
} // getCache


//-----------------------------------------------------------------------------
// setCache
//
// - set cache object
// - release previous cache
//
// returns:
//      TRUE
//
BOOL SODaSItemTag::setCache(
	IN SODaSCache* cache)   // cache object
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	SODaSCache* prevCache = m_cache;

	if (cache)
	{
		cache->addRef();
	}

	m_cache = cache;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);

	if (prevCache)
	{
		prevCache->release();
	}

	return TRUE;
} // setCache


//-----------------------------------------------------------------------------
// setAccessPaths
//
// - set access paths
// - default implementation don't support access paths
//
// returns:
//      TRUE  - set access path
//      FALSE - can't set access path
//
BOOL SODaSItemTag::setAccessPaths(SOCmnStringList* /* accessPaths */)
{
	return FALSE;
} // setAccessPaths


//-----------------------------------------------------------------------------
// getAccessPaths
//
// - get access paths
// - default implementation don't support access paths
//
void SODaSItemTag::getAccessPaths(SOCmnStringList* /* accessPaths */)
{
} // getAccessPaths


//-----------------------------------------------------------------------------
// checkAccessPath
//
// - checks if access path is OK
// - default implementation don't support access paths
//
// returns:
//      TRUE  - access path is OK
//      FALSE - access path not OK
//
BOOL SODaSItemTag::checkAccessPath(
	IN SOCmnString accessPath)  // access path
{
	return TRUE;
} // checkAccessPath


//-----------------------------------------------------------------------------
// setNativeDatatype
//
// - sets native datatype
//
// returns:
//      TRUE  - datatype set
//      FALSE - datatype not supported
//
BOOL SODaSItemTag::setNativeDatatype(
	IN VARTYPE datatype)    // new native datatype
{
	if (!supportDatatype(datatype))
	{
		return FALSE;
	}

	onSetNativeDatatype(datatype);
	m_nativeDatatype = datatype;
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();

	if (m_cache)
	{
		m_cache->setDatatype(m_nativeDatatype);
	}

	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
	return TRUE;
} // setNativeDatatype


//-----------------------------------------------------------------------------
// onSetNativeDatatype
//
// - called before native datatype is set
// - default implementation: do nothing
//
void SODaSItemTag::onSetNativeDatatype(
	IN VARTYPE /* datatype */)    // native datatype
{} // onSetNativeDatatype


//-----------------------------------------------------------------------------
// supportDatatype
//
// - checks if the data type is supported
//
// returns:
//      TRUE  - support data type
//      FALSE - datatype not supported
//
BOOL SODaSItemTag::supportDatatype(
	IN VARTYPE dataType)    // data type
{
	return SODaSCache::defaultSupportDatatype(dataType);
} // supportDatatype


//-----------------------------------------------------------------------------
// supportReqDatatype
//
// - checks if the requested data type is supported
//
// returns:
//      S_OK          - support data type
//      OPC_E_BADTYPE - not support data type
//
HRESULT SODaSItemTag::supportReqDatatype(
	IN VARTYPE reqDataType) // requested data type
{
	// supported datatype ?
	if (!supportDatatype(reqDataType))
	{
		return OPC_E_BADTYPE;
	}

	// simple checks
	if ((reqDataType == VT_EMPTY) || (reqDataType == (VT_ARRAY | VT_EMPTY)))
	{
		return S_OK;
	}

	if (m_nativeDatatype == reqDataType)
	{
		return S_OK;
	}

	if ((reqDataType & VT_ARRAY) == (m_nativeDatatype & VT_ARRAY))
	{
		return S_OK;
	}
	else
	{
		return OPC_E_BADTYPE;    // can not convert simple datatype to array
	}
} // supportReqDatatype


//-----------------------------------------------------------------------------
// setAccessRights
//
// - sets access rights
//
// returns:
//      TRUE  - access rights set
//      FALSE - not set access rights
//
BOOL SODaSItemTag::setAccessRights(
	IN DWORD rights)    // access rights
{
	onSetAccessRights(rights);
	m_accessRightsAndDynamic &= ~SODASITEMTAG_MASK_DEFAULTACCESSRIGHTS;
	m_accessRightsAndDynamic &= ~SODASITEMTAG_MASK_ACCESSRIGHTS;
	m_accessRightsAndDynamic |= (SODASITEMTAG_MASK_ACCESSRIGHTS & rights);
	return TRUE;
} // setAccessRights


//-----------------------------------------------------------------------------
// onSetAccessRights
//
// - called before access rights are set
// - default implementation: do nothing
//
void SODaSItemTag::onSetAccessRights(
	IN DWORD /* rights */) // access rights
{} // onSetAccessRights


//-----------------------------------------------------------------------------
// getAccessRights
//
// - get the access rights for one connected client instance
//
// returns:
//      access rights
//
DWORD SODaSItemTag::getAccessRights(
	IN SODaSServer* /* server */) // client instance to request access rights for
{
	return getAccessRights();
} // getAccessRights


//-----------------------------------------------------------------------------
// setDynamic
//
// - set 'dynamic created flag'
//
// returns:
//      TRUE  - flag set
//
BOOL SODaSItemTag::setDynamic(
	IN BOOL dynamic)    // dynamic or not
{
	if (dynamic)
	{
		m_accessRightsAndDynamic |= SODASITEMTAG_MASK_DYNAMIC;
	}
	else
	{
		m_accessRightsAndDynamic &= ~SODASITEMTAG_MASK_DYNAMIC;
	}

	return TRUE;
} // setDynamic


//-----------------------------------------------------------------------------
// signalQueueEvent
//
// - signal event of queued I/O
//
// returns:
//      TRUE  - event signaled
//      FALSE - no queue event
//
BOOL SODaSItemTag::signalQueueEvent(
	IN BOOL readEvent)  // read or write event
{
	SOCmnEventPointer event;

	if (readEvent)
	{
		event.set(getReadEvent());
	}
	else
	{
		event.set(getWriteEvent());
	}

	if (event.isInit())
	{
		event.signal();
		return TRUE;
	}

	return FALSE;
} // signalQueueEvent


//-----------------------------------------------------------------------------
// addToQueue
//
// - add request to list of queued I/O
//
// returns:
//      TRUE  - request added
//      FALSE - no queue
//
BOOL SODaSItemTag::addToQueue(
	IN SODaSRequest* req,   // request
	IN BOOL readQueue)      // read or write
{
	SOCmnList<SODaSRequest> queue = NULL;

	if (readQueue)
	{
		queue = getReadQueue();
	}
	else
	{
		queue = getWriteQueue();
	}

	if (!!queue)
	{
		if (queue.findObject(req))
		{
			return TRUE;
		}

		queue.add(req); // add to queue
		return TRUE;
	}

	return FALSE;
} // addToQueue


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODaSItemTag::handleRequest(
	IN SODaSRequest* request)   // request
{
	_ASSERTION(FALSE, "SODaSItemTag::handleRequest must be implemented in child class for direct I/O");
	request->complete(E_FAIL);
} // handleRequest


//-----------------------------------------------------------------------------
// setConstantValue
//
// - set a constant value for an item tag
//
// returns:
//      TRUE
//
BOOL SODaSItemTag::setConstantValue(
	IN SOCmnVariant& value) // constant value
{
	SOCmnPointer<SODaSCache> cache = getCache();
	cache->setValue(value);
	cache->setQuality(OPC_QUALITY_GOOD);
	setDeviceIOMode(SODaSItemTag::constant);
	return TRUE;
} // setConstantValue


//-----------------------------------------------------------------------------
// getDeviceIOMode
//
// - get the device I/O mode of the item tag
//
// returns:
//      device I/O mode
//
BYTE SODaSItemTag::getDeviceIOMode(
	IN SODaSRequest* req)   // request
{
	if (req)
		if ((getUpdateMode() == SODaSItemTag::report) && (req->getOperation() == SODaSRequest::read))
		{
			SOCmnPointer<SODaSCache> cache = getCache();

			if (cache.isNotNull())
				if (cache->isSet())
				{
					return SODaSItemTag::cache;    // use the cache value for event driven read operations
				}
		}

	if (m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE)
	{
		return m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE;
	}
	else
	{
		return SODaSItemTag::direct;
	}
} // getDeviceIOMode


//-----------------------------------------------------------------------------
// setDeviceIOMode
//
// - set the device I/O mode of the item tag
//
void SODaSItemTag::setDeviceIOMode(
	IN BYTE deviceIOMode)   // device mode
{
	m_deviceIOAndUpdateMode &= ~SODASITEMTAG_MASK_DEVICEIOMODE;
	m_deviceIOAndUpdateMode |= (SODASITEMTAG_MASK_DEVICEIOMODE & deviceIOMode);
} // setDeviceIOMode


//-----------------------------------------------------------------------------
// getUpdateMode
//
// - get the update mode of the item tag
//
BYTE SODaSItemTag::getUpdateMode(void)
{
	return (m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_UPDATEMODE);
} // getUpdateMode


//-----------------------------------------------------------------------------
// setUpdateMode
//
// - set the update mode of the item tag
//
void SODaSItemTag::setUpdateMode(
	IN BYTE updateMode) // update mode
{
	m_deviceIOAndUpdateMode &= ~SODASITEMTAG_MASK_UPDATEMODE;
	m_deviceIOAndUpdateMode |= (SODASITEMTAG_MASK_UPDATEMODE & updateMode);
} // setDeviceIOMode


//-----------------------------------------------------------------------------
// getObjectDaSItemTagIConnect
//
// - get object using the implementation of the interface
//
// returns:
//      object
//      NULL - don't have the pointer of the object
//
SOCmnObject* SODaSItemTag::getObjectDaSItemTagIConnect(void)
{
	return (SOCmnObject*)this;
} // getObjectDaSItemTagIConnect


//-----------------------------------------------------------------------------
// getItemID
//
// - get the item id of the item tag
//
// returns:
//      the item ID of the object
//
SOCmnString SODaSItemTag::getItemID(void)
{
	return getFullName();
} // getItemID


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//      the device state of the object
//
SOCmnString SODaSItemTag::getDeviceState(void)
{
	return SOCmnString(SOWATCH_DEVSTATE_OK);
} // getDeviceState


//-----------------------------------------------------------------------------
// getDeviceStateInfo
//
// - get state info of the device represented by the object
//
// returns:
//      the device state info  of the object
//
SOCmnString SODaSItemTag::getDeviceStateInfo(void)
{
	return SOCmnString(_T(""));
} // getDeviceStateInfo


//-----------------------------------------------------------------------------
// supportWriteQT
//
// - check if writing of qualtity and/or timestamp is supported
//
// returns:
//      TRUE  - supported
//      FALSE - not supported
//
BOOL SODaSItemTag::supportWriteQT(
	IN BYTE /* flagsQT */) // SODASREQUEST_FLAG_QUALITY and SODASREQUEST_FLAG_TIMESTAMP
{
	return TRUE;
} // supportWriteQT


//-----------------------------------------------------------------------------
// getMaxAgeCyclicReq
//
// - cache age optimization for cyclic requests
//
// returns:
//      0         - no cache age optimization
//      -1        - group update rate optimization
//      all other - fixed time in ms optimization
//
DWORD SODaSItemTag::getMaxAgeCyclicReq(void)
{
	return 0xFFFFFFFF; // default: optimize within group update rate
} // getMaxAgeCyclicReq


//-----------------------------------------------------------------------------
// hasChildren
//
// - does the object has children (leaves of branches)
//
// returns:
//      TRUE  - object has children
//      FALSE - no children
//
BOOL SODaSItemTag::hasChildren(void)
{
	SOCmnList<SOCmnElement> lList(getLeafList());

	if (lList.isInit())
		if (!lList.isEmpty())
		{
			return TRUE;
		}

	SOCmnList<SOCmnElement> bList(getBranchList());

	if (bList.isInit())
		if (!bList.isEmpty())
		{
			return TRUE;
		}

	return FALSE;
} // hasChildren


//-----------------------------------------------------------------------------
// addProperty
//
// - adds property to tag
//
// returns:
//		pointer to property object
//		NULL - property not supported
//
SODaSProperty* SODaSItemTag::addProperty(
	IN DWORD propertyId,    // property id
	IN LPCTSTR propName)    // property name
{
	SOCmnList<SOCmnElement> *pList = getAdditionalList();

	if (pList == NULL)
	{
		return NULL;    // no properties supported by this tag
	}

	SODaSProperty* prop;
	SOCmnList<SODaSProperty> propList((SOCmnList<SODaSProperty> *)pList);

	if (!propList.isInit())
	{
		propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT |
						SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	}

	prop = propList.findKey(propertyId);

	if (!prop)
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSCreator> creator = engine->getCreator();
		prop = creator->createProperty(this, propertyId, propName);
		prop->setParentBranch(this);
		propList.add(prop);
	}

	return (SODaSProperty*)prop;
} // addProperty


//-----------------------------------------------------------------------------
// addProperty
//
// - adds property to tag
//
// returns:
//		TRUE  - property added
//		FALSE - can't add property
//
BOOL SODaSItemTag::addProperty(
	IN SODaSProperty* prop) // property
{
	SOCmnList<SOCmnElement> *pList = getAdditionalList();

	if (pList == NULL)
	{
		return FALSE;    // no properties supported by this tag
	}

	if (!pList->isInit())
	{
		SOCmnListConfig cfg;
		cfg.m_type = SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT |
					 SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		createAdditionalList(&cfg);
	}

	return addAdditionalElement(prop);
} // addProperty


BOOL SODaSItemTag::hasProperty(IN DWORD /* propId */, SODaSPropertyData* /* pPropData */)
{
	return FALSE;
}

BOOL SODaSItemTag::getProperties(
	IN BOOL /* isSOAP */,
	OUT SOCmnList<SODaSPropertyData>* /* pListPD */)
{
	return FALSE;
}

BOOL SODaSItemTag::initProperty(
	IN SODaSProperty* /* property */,
	SODaSPropertyData* /* pPropData */)
{
	return FALSE;
}


//-----------------------------------------------------------------------------
// handlePropertyRequest
//
// - handle I/O request for child property
//
// returns:
//		TRUE  - request handled
//		FALSE - can't handle request
//
BOOL SODaSItemTag::handlePropertyRequest(
	IN SODaSProperty* /* prop */,       // property
	IN SODaSRequest* /* request */) // request
{
	return FALSE;
} // handlePropertyRequest


//-----------------------------------------------------------------------------
// getEUType
//
// - get the type of the EU information of the tag
//
// returns:
//		type of the EU information
//
OPCEUTYPE SODaSItemTag::getEUType(void)
{
	SOCmnList<SOCmnElement> *pList = getAdditionalList();

	if (pList == NULL)
	{
		return OPC_NOENUM;  // no properties supported by this tag
	}   //  end if

	SOCmnList<SODaSProperty> propList((SOCmnList<SODaSProperty> *)pList);

	if (!!propList)
	{
		SOCmnPointer<SODaSProperty> prop7;
		prop7 = propList.findKey(7);

		if (!prop7)
		{
			prop7 = propList.findKey(2507);
		}

		if (!!prop7)
		{
			SOCmnPointer<SODaSProperty> prop102;
			prop102 = propList.findKey(102);

			if (!prop102)
			{
				return OPC_ENUMERATED;
			}
			else
			{
				return OPC_ANALOG;
			}
		}
	}
	else
	{
		SODaSPropertyData propDataEuType;

		if (hasProperty(7, &propDataEuType))
		{
			addProperty(7, NULL);
			SODaSPropertyData propDataEuInfo;

			if (hasProperty(102, &propDataEuInfo))
			{
				addProperty(102, NULL);
				return OPC_ANALOG;
			}
			else
			{
				return OPC_ENUMERATED;
			}
		}
		else if (hasProperty(2507, &propDataEuType))
		{
			addProperty(2507, NULL);
			SODaSPropertyData propDataEuInfo;

			if (hasProperty(102, &propDataEuInfo))
			{
				addProperty(102, NULL);
				return OPC_ANALOG;
			}
			else
			{
				return OPC_ENUMERATED;
			}
		}
	}

	return OPC_NOENUM;
} // getEUType


//-----------------------------------------------------------------------------
// getEUInfo
//
// - get the EU information of the tag
//
// returns:
//		TRUE
//
BOOL SODaSItemTag::getEUInfo(
	OUT SOCmnVariant& info)     // EU information
{
	OPCEUTYPE type = getEUType();

	if (type == OPC_NOENUM)
	{
		::VariantInit(info);    // return VT_EMPTY
		return TRUE;
	}
	else
	{
		SOCmnList<SODaSProperty> propList((SOCmnList<SODaSProperty> *)getAdditionalList());
		info.clear();

		if (type == OPC_ANALOG)
		{
			SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
			SOCmnPointer<SODaSCreator> creator = engine->getCreator();
			SOCmnVariant value;
			SOCmnPointer<SODaSProperty> prop102;
			SOCmnPointer<SODaSProperty> prop103;
			SOCmnPointer<SODaSCache> cache;
			SAFEARRAYBOUND dim;
			LONG idx;
			HRESULT error;
			SOCmnPointer<SODaSRequest> req;
			prop102 = propList.findKey(102);
			prop103 = propList.findKey(103);

			if (!prop102)
			{
				prop102 = addProperty(102, _T("highEU"));

				if (prop102.isNotNull())
				{
					initProperty(prop102, NULL);
					prop102->setDynamic(TRUE);
					req = creator->createRequest(NULL, NULL, prop102, SODaSRequest::read, SODaSRequest::serverDevice);

					if (req.isNotNull())
					{
						req->setItemTag(prop102);
						req->setResultAddress(&error);
						req->setResult(S_OK);
						handlePropertyRequest(prop102, req);
					}
				}
			}

			if (!prop103)
			{
				prop103 = addProperty(103, _T("lowEU"));

				if (prop103.isNotNull())
				{
					initProperty(prop103, NULL);
					prop103->setDynamic(TRUE);
					req = creator->createRequest(NULL, NULL, prop103, SODaSRequest::read, SODaSRequest::serverDevice);

					if (req.isNotNull())
					{
						req->setItemTag(prop103);
						req->setResultAddress(&error);
						req->setResult(S_OK);
						handlePropertyRequest(prop103, req);
					}
				}
			}

			dim.lLbound = 0;
			dim.cElements = 2;
			info.vt = (VT_ARRAY | VT_R8);
			info.parray = SOCmnVariant::safeArrayCreate(VT_R8, 1, &dim);
			idx = 0;
			cache = prop103->getCache();
			cache->getValue(VT_R8, value);
			::SafeArrayPutElement(info.parray, &idx, &value.dblVal);
			idx = 1;
			cache = prop102->getCache();
			cache->getValue(VT_R8, value);
			::SafeArrayPutElement(info.parray, &idx, &value.dblVal);
		}
		else
		{
			SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
			SOCmnPointer<SODaSCreator> creator = engine->getCreator();
			SOCmnPointer<SODaSProperty> propEnum;
			SOCmnPointer<SODaSCache> cache;
			HRESULT error;
			SOCmnPointer<SODaSRequest> req;
			propEnum = propList.findKey(getEUEnumeratedPropertyId());

			if (!propEnum)
			{
				propEnum = addProperty(getEUEnumeratedPropertyId(), _T("euInfo"));

				if (propEnum.isNotNull())
				{
					initProperty(propEnum, NULL);
					propEnum->setDynamic(TRUE);
					req = creator->createRequest(NULL, NULL, propEnum, SODaSRequest::read, SODaSRequest::serverDevice);

					if (req.isNotNull())
					{
						req->setItemTag(propEnum);
						req->setResultAddress(&error);
						req->setResult(S_OK);
						handlePropertyRequest(propEnum, req);
					}
				}
			}

			cache = propEnum->getCache();
			cache->getValue(info);
		}

		return TRUE;
	}
} // getEUInfo


//-----------------------------------------------------------------------------
// setEUAnalog
//
// - set the analog EU information of the tag
//
// returns:
//		TRUE  - EU information set
//		FALSE - can't set EU information
//
BOOL SODaSItemTag::setEUAnalog(
	IN double low,  // low value
	IN double high) // high value
{
	SOCmnPointer<SODaSProperty> tagProp;
	SOCmnVariant value;
	BOOL ret = TRUE;
#ifdef SOFEATURE_DCOM
	tagProp = addProperty(7, NULL);

	if (!!tagProp)
	{
		tagProp->setDeviceIOMode(SODaSItemTag::direct);
		tagProp->setNativeDatatype(VT_I4);
		tagProp->setAccessRights(OPC_READABLE);
	}
	else
	{
		ret = FALSE;
	}

#endif
#ifdef SOFEATURE_SOAP
	tagProp = addProperty(2507, NULL);

	if (!!tagProp)
	{
		tagProp->setDeviceIOMode(SODaSItemTag::direct);
		tagProp->setNativeDatatype(VT_BSTR);
		tagProp->setAccessRights(OPC_READABLE);
	}
	else
	{
		ret = FALSE;
	}

#endif
	tagProp = addProperty(102, NULL);

	if (!!tagProp)
	{
		value = high;
		tagProp->setConstantValue(value);
	}
	else
	{
		ret = FALSE;
	}

	tagProp = addProperty(103, NULL);

	if (!!tagProp)
	{
		value = low;
		tagProp->setConstantValue(value);
	}
	else
	{
		ret = FALSE;
	}

	return ret;
} // setEUAnalog


//-----------------------------------------------------------------------------
// setEUEnumerated
//
// - set the enumeration EU information of the tag
//
// returns:
//		TRUE  - EU information set
//		FALSE - can't set EU information
//
BOOL SODaSItemTag::setEUEnumerated(
	IN SOCmnStringList* enumList,   // enumeration strings
	IN OPTIONAL LONG startIndex)    // start index of enum
{
	SOCmnPointer<SODaSProperty> tagProp;
	SAFEARRAYBOUND dim;
	SOCmnListPosition pos;
	LONG idx;
	BSTR bstr;
	SOCmnString enumStr;
	SOCmnVariant value;
	USES_CONVERSION;

	if (!enumList)
	{
		return FALSE;
	}

	dim.lLbound = startIndex;
	dim.cElements = enumList->getCount();
	value.vt = (VT_ARRAY | VT_BSTR);
	value.parray = SOCmnVariant::safeArrayCreate(VT_BSTR, 1, &dim);
	pos = enumList->getStartPosition();
	idx = dim.lLbound;

	while (pos)
	{
		enumStr = enumList->getNext(pos);
		bstr = ::SysAllocString(T2COLE((LPCTSTR)enumStr));
		::SafeArrayPutElement(value.parray, &idx, bstr);
		::SysFreeString(bstr);
		idx++;
	}

#ifdef SOFEATURE_DCOM
	tagProp = addProperty(7, NULL);

	if (!!tagProp)
	{
		tagProp->setDeviceIOMode(SODaSItemTag::direct);
		tagProp->setNativeDatatype(VT_I4);
		tagProp->setAccessRights(OPC_READABLE);
	}
	else
	{
		return FALSE;
	}

#endif
#ifdef SOFEATURE_SOAP
	tagProp = addProperty(2507, NULL);

	if (!!tagProp)
	{
		tagProp->setDeviceIOMode(SODaSItemTag::direct);
		tagProp->setNativeDatatype(VT_BSTR);
		tagProp->setAccessRights(OPC_READABLE);
	}
	else
	{
		return FALSE;
	}

#endif
	tagProp = addProperty(getEUEnumeratedPropertyId(), NULL);

	if (!!tagProp)
	{
		tagProp->setDescription(_T("Enumerated EU"));
		tagProp->setNativeDatatype(VT_ARRAY | VT_BSTR);
		tagProp->setConstantValue(value);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // setEUEnumerated


//-----------------------------------------------------------------------------
// getEUEnumeratedPropertyId
//
// - get the property Id of the enumeration EU information
//
// returns:
//		property id
//
DWORD SODaSItemTag::getEUEnumeratedPropertyId(void)
{
	return 8;
} // getEUEnumeratedPropertyId



//-----------------------------------------------------------------------------
// SODaSItemTagIConnect                                                       |
//-----------------------------------------------------------------------------

SODaSItemTagIConnect::SODaSItemTagIConnect()
{}

SODaSItemTagIConnect::~SODaSItemTagIConnect()
{}

//-----------------------------------------------------------------------------
// getReadEvent
//
// - get event for read request queue
//
// returns:
//      handle of read event
//
SOCmnEventPointer SODaSItemTagIConnect::getReadEvent(void)
{
	return SOCmnEventPointer();
} // getReadEvent


//-----------------------------------------------------------------------------
// getWriteEvent
//
// - get event for write request queue
//
// returns:
//      handle of write event
//
SOCmnEventPointer SODaSItemTagIConnect::getWriteEvent(void)
{
	return SOCmnEventPointer();
} // getWriteEvent


//-----------------------------------------------------------------------------
// getReadQueue
//
// - get read request queue
//
// returns:
//      pointer to read request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagIConnect::getReadQueue(void)
{
	return NULL;
} // getReadQueue


//-----------------------------------------------------------------------------
// getWriteQueue
//
// - get write request queue
//
// returns:
//      pointer to write request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagIConnect::getWriteQueue(void)
{
	return NULL;
} // getWriteQueue


//-----------------------------------------------------------------------------
// getAllConnectedObjects
//
// - get all items connected to the item tag
//
void SODaSItemTagIConnect::getAllConnectedObjects(
	IN DWORD objType,                       // object type
	OUT SOCmnList<SOCmnObject> &objList)    // object list
{
	if (objType != SOCMNOBJECT_TYPE_ITEM)
	{
		return;
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> opcRoot = engine->getObjectRoot();
	SOCmnList<SOCmnElement> serverList(opcRoot->getBranchList());
	SOCmnListPosition posSrv;
	SODaSServer* server;
	SOCmnList<SOCmnElement> groupList;
	SOCmnListPosition posGrp;
	SODaSGroup* group;
	SOCmnList<SOCmnElement> itemList;
	SOCmnListPosition posItm;
	SODaSItem* item;
	SOCmnPointer<SODaSItemTag> itemTag;
	_ASSERTION((getObjectDaSItemTagIConnect() != NULL), "The method getObjectDaSItemTagIConnect must be implemeted!");
	posSrv = serverList.getStartPosition();

	while (posSrv)
	{
		server = (SODaSServer*)serverList.getNext(posSrv);
		groupList = server->getBranchList();
		posGrp = groupList.getStartPosition();

		while (posGrp)
		{
			group = (SODaSGroup*)groupList.getNext(posGrp);
			itemList = group->getLeafList();
			posItm = itemList.getStartPosition();

			while (posItm)
			{
				item = (SODaSItem*)itemList.getNext(posItm);
				itemTag = item->getItemTag();

				if ((SODaSItemTag*)itemTag == getObjectDaSItemTagIConnect())
				{
					objList.add((SOCmnObject*)item);
				}
			}

			itemList.autoUnlock();
		}

		groupList.autoUnlock();
	}
} // getAllConnectedObjects


//-----------------------------------------------------------------------------
// addConnectedObject
//
// - add connected to the list of the item tag
//
void SODaSItemTagIConnect::addConnectedObject(
	IN SOCmnObject* /* obj */,           // connected object
	IN BOOL /* goingToActivate */)
{
} // addConnectedObject


//-----------------------------------------------------------------------------
// removeConnectedObject
//
// - remove connected from the list of the item tag
//
void SODaSItemTagIConnect::removeConnectedObject(
	IN SOCmnObject* /* obj */)    // connected object
{
} // removeConnectedObject


//-----------------------------------------------------------------------------
// activateConnectedObject
//
// - activate connected object
//
void SODaSItemTagIConnect::activateConnectedObject(
	IN SOCmnObject* /* obj */)    // connected object
{
} // activateConnectedObject


//-----------------------------------------------------------------------------
// deactivateConnectedObject
//
// - deactivate connected object
//
void SODaSItemTagIConnect::deactivateConnectedObject(
	IN SOCmnObject* /* obj */)    // connected object
{
} // deactivateConnectedObject


//-----------------------------------------------------------------------------
// getObjectDaSItemTagIConnect
//
// - get object using the implementation of the interface
//
// returns:
//      object
//      NULL - don't have the pointer of the object
//
SOCmnObject* SODaSItemTagIConnect::getObjectDaSItemTagIConnect(void)
{
	return NULL;
} // getObjectDaSItemTagIConnect


//-----------------------------------------------------------------------------
// notifyAllConnectedObjects
//
// - notify all items of the item tag that the value changed (report update mode)
//
// returns:
//      number of items notified
//
DWORD SODaSItemTagIConnect::notifyAllConnectedObjects(
	IN OPTIONAL SOCmnVariant* value,          // value
	IN OPTIONAL WORD quality,                 // quality
	IN OPTIONAL SOCmnDateTime* timeStamp,     // time stamp
	IN OPTIONAL HRESULT result)               // result
{
	_ASSERTION((getObjectDaSItemTagIConnect() != NULL), "The method getObjectDaSItemTagIConnect must be implemeted!");
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnList<SOCmnObject> itemList;
	DWORD itemCnt = 0;
	SODaSItemTag* itemTag = (SODaSItemTag*)getObjectDaSItemTagIConnect();
	SOCmnVariant cacheValue;
	SOCmnDateTime genTimeStamp;

	if (timeStamp == NULL)
	{
		genTimeStamp.now();
		timeStamp = &genTimeStamp;
	}

	// check value input
	if (value == NULL)
	{
		// no value supplied -> take the current cache value
		SOCmnPointer<SODaSCache> cache = itemTag->getCache();

		if (cache.isNotNull())
		{
			cache->getValue(cacheValue);
			// no value -> no valid quality
			quality = cache->getQuality();
			value = &cacheValue;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		// value supplied -> update the cache
		if (SUCCEEDED(result))
		{
			SOCmnPointer<SODaSCache> cache = itemTag->getCache();

			if (cache.isNotNull())
			{
				cache->set(*value, quality, timeStamp);
			}
		}
	}

	itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);
	getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, itemList);

	if (!itemList.isEmpty())
	{
		itemCnt += notifyItems((SOCmnList<SODaSItem> *) &itemList, *value, quality, *timeStamp, result);
	}

	itemList.removeAll();

	// more tags on one cache ?
	for (BYTE i = 0; i < SODAS_SERVER_INSTANCE_COUNT; i++)
	{
		SOCmnPointer<SODaSNameSpaceRoot> nsRoot = engine->getNameSpaceRoot(i);

		if (nsRoot->getUseTagLinks())
		{
			SOCmnList<SODaSItemTag> itemTagList;
			SOCmnListPosition itPos;
			SOCmnPointer<SODaSCache> cache = ((SODaSItemTag*)getObjectDaSItemTagIConnect())->getCache();
			itemTagList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);
			cache->getAllItemTags(itemTagList);

			if (!itemTagList.isEmpty())
			{
				itPos = itemTagList.GetHeadPosition();

				while (itPos)
				{
					itemTag = itemTagList.GetNext(itPos);

					if (itemTag != getObjectDaSItemTagIConnect())
					{
						itemTag->getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, itemList);

						if (!itemList.isEmpty())
						{
							itemCnt += notifyItems((SOCmnList<SODaSItem> *) &itemList, *value, quality, *timeStamp, result);
							itemList.removeAll();
						}
					}
				}
			}

			itemTagList.destroy();
		}
	}

	itemList.destroy();
	return itemCnt;
} // notifyAllConnectedObjects


//-----------------------------------------------------------------------------
// notifyItems
//
// - notify items the item tag value changed (report update mode)
//
DWORD SODaSItemTagIConnect::notifyItems(
	IN SOCmnList<SODaSItem> *itmList, // item list
	IN SOCmnVariant& value,           // value
	IN WORD quality,                  // quality
	IN SOCmnDateTime& timeStamp,      // time stamp
	IN HRESULT result)                // result
{
	_ASSERTION((getObjectDaSItemTagIConnect() != NULL), "The method getObjectDaSItemTagIConnect must be implemeted!");
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SODaSItem* item;
	SOCmnListPosition pos;
	SOCmnPointer<SODaSGroup> group;
	SOCmnPointer<SODaSServer> srv;
	SOCmnPointer<SODaSRequest> req;
	SOCmnPointer<SODaSTransaction> tac;
	SOCmnPointer<SODaSServer> signalServer;
	SOCmnList<SODaSItem> itemList(itmList);
	SODaSItemTag* itemTag = (SODaSItemTag*)getObjectDaSItemTagIConnect();
	BOOL addRequest;
	DWORD itemCnt = 0;
	BOOL insertRequestSucceeded = TRUE;
	pos = itemList.getStartPosition();

	while (insertRequestSucceeded && pos)
	{
		item = (SODaSItem*)itemList.getNext(pos);
		item->addRef();
		group = (SODaSGroup*)item->getParentBranch();

		if ((SODaSGroup*)group)
		{
			if (item->isActive())
			{
				srv = (SODaSServer*)group->getParentBranch();

				if ((SODaSServer*)srv)
				{
					if (!srv->isPublicGroups())
					{
						// not the public groups server
						addRequest = TRUE;
						tac = group->getCyclicTransaction();
						SOCmnSingleLock<SOCmnSync> lock(tac->getRequestListLock());

						if ((!group->isNotifyAllChanges()) && (!tac->notifyAllChanges()))
						{
							SOCmnPointer<SODaSRequest> cycRepReq = item->getCyclicRequest();

							if (cycRepReq.isNotNull())
							{
								if (!(cycRepReq->getObjectState() & (SODASREQUEST_FLAG_PROCESSED | SODASREQUEST_FLAG_TOPROCESS)))
								{
									cycRepReq->setReportValue(value);
									cycRepReq->setReportQuality(quality);
									cycRepReq->setReportTimeStamp(timeStamp);
									cycRepReq->setResult(result);
									addRequest = FALSE;
									itemCnt++;
								}
							}
						}

						if (addRequest)
						{
							if ((SODaSServer*)srv != (SODaSServer*)signalServer)
							{
								if (signalServer.isNotNull())
								{
									signalServer->signalWorkEvent();    // signal async thread
								}

								signalServer = srv;
							}

							req = creator->createRequest(srv, item, itemTag, SODaSRequest::read, SODaSRequest::report);

							if (req.isNotNull())
							{
								req->setItem(item);
								req->setReportValue(value);
								req->setReportQuality(quality);
								req->setReportTimeStamp(timeStamp);
								req->setResult(result);
								insertRequestSucceeded = tac->insertRequest(req);

								if (insertRequestSucceeded == TRUE)
								{
									item->setCyclicRequest(req);
									itemCnt++;
								}
							}
						}
					}
				}
			}
		}

		item->release();
	}

	if (signalServer.isNotNull())
	{
		signalServer->signalWorkEvent();    // signal async thread
	}

	return itemCnt;
} // notifyItems




//-----------------------------------------------------------------------------
// SODaSItemTagConnectQueueStore                                              |
//-----------------------------------------------------------------------------

SODaSItemTagConnectQueueStore::SODaSItemTagConnectQueueStore()
{
}

SODaSItemTagConnectQueueStore::~SODaSItemTagConnectQueueStore()
{
}


//-----------------------------------------------------------------------------
// setReadEvent
//
// - set event for read request queue
//
// returns:
//      TRUE
//
BOOL SODaSItemTagConnectQueueStore::setReadEvent(
	IN SOCmnEventPointer event)    // read event
{
	m_readEvent = event;
	return TRUE;
} // setReadEvent


//-----------------------------------------------------------------------------
// getReadEvent
//
// - get event for read request queue
//
// returns:
//      handle of read event
//
SOCmnEventPointer SODaSItemTagConnectQueueStore::getReadEvent(void)
{
	return m_readEvent;
} // getReadEvent


//-----------------------------------------------------------------------------
// setWriteEvent
//
// - set event for write request queue
//
// returns:
//      TRUE
//
BOOL SODaSItemTagConnectQueueStore::setWriteEvent(
	IN SOCmnEventPointer event)    // write event
{
	m_writeEvent = event;
	return TRUE;
} // setWriteEvent


//-----------------------------------------------------------------------------
// getWriteEvent
//
// - get event for write request queue
//
// returns:
//      handle of write event
//
SOCmnEventPointer SODaSItemTagConnectQueueStore::getWriteEvent(void)
{
	return m_writeEvent;
} // getWriteEvent


//-----------------------------------------------------------------------------
// setReadQueue
//
// - set read request queue
//
// returns:
//      TRUE
//
BOOL SODaSItemTagConnectQueueStore::setReadQueue(
	IN SOCmnList<SODaSRequest> *queue)  // read request queue
{
	m_readQueue = queue;
	m_readQueue.dontAutoLock();
	return TRUE;
} // setReadQueue


//-----------------------------------------------------------------------------
// getReadQueue
//
// - get read request queue
//
// returns:
//      pointer to read request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagConnectQueueStore::getReadQueue(void)
{
	return &m_readQueue;
} // getReadQueue


//-----------------------------------------------------------------------------
// setWriteQueue
//
// - set write request queue
//
// returns:
//      TRUE
//
BOOL SODaSItemTagConnectQueueStore::setWriteQueue(
	IN SOCmnList<SODaSRequest> *queue)  // write request queue
{
	m_writeQueue = queue;
	m_writeQueue.dontAutoLock();
	return TRUE;
} // setWriteQueue


//-----------------------------------------------------------------------------
// getWriteQueue
//
// - get write request queue
//
// returns:
//      pointer to write request queue
//
SOCmnList<SODaSRequest> *SODaSItemTagConnectQueueStore::getWriteQueue(void)
{
	return &m_writeQueue;
} // getWriteQueue




//-----------------------------------------------------------------------------
// SODaSItemTagConnectItemsStore                                               |
//-----------------------------------------------------------------------------

SODaSItemTagConnectItemsStore::SODaSItemTagConnectItemsStore(void)
{
	m_itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
}

SODaSItemTagConnectItemsStore::~SODaSItemTagConnectItemsStore()
{
	m_itemList.destroy();
}


//-----------------------------------------------------------------------------
// getAllConnectedObjects
//
// - get all items connected to the item tag
//
void SODaSItemTagConnectItemsStore::getAllConnectedObjects(
	IN DWORD objType,                       // object type
	OUT SOCmnList<SOCmnObject> &objList)    // object list
{
	if (objType != SOCMNOBJECT_TYPE_ITEM)
	{
		return;
	}

	SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
	objList.clone((SOCmnList<SOCmnObject> *)&m_itemList);
} // getAllConnectedObjects


//-----------------------------------------------------------------------------
// addConnectedObject
//
// - add connected to the list of the item tag
//
void SODaSItemTagConnectItemsStore::addConnectedObject(
	IN SOCmnObject* obj,
	IN BOOL /* goingToActivate */)    // connected object
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
			m_itemList.add((SODaSItem*) obj);
		}
	}
} // addItem


//-----------------------------------------------------------------------------
// removeConnectedObject
//
// - remove connected from the list of the item tag
//
void SODaSItemTagConnectItemsStore::removeConnectedObject(
	IN SOCmnObject* obj)    // connected object
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
			m_itemList.removeObject((SODaSItem*)obj);
		}
	}
} // removeItem

#endif // SODAS
