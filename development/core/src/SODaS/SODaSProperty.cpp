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
//  Filename    : SODaSProperty.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Namespace Tag classes                                       |
//                - SODaSProperty: base class for namespace property objects  |
//                - SODaSPropertyData: data of a property object              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSItemTag.h"
#include "SODaSTag.h"
#include "SODaSProperty.h"
#include "SODaSCache.h"
#include "SOCmnTrace.h"
#include "SODaSEntry.h"
#include "SODaSItem.h"
#include "SODaSServer.h"
#include <opcerror.h>


//-----------------------------------------------------------------------------
// SODaSProperty                                                              |
//-----------------------------------------------------------------------------

SODaSProperty::SODaSProperty(void)
	: SODaSItemTag(SOCMNOBJECT_TYPE_PROPERTY)
{
	m_accessRightsAndDynamic = OPC_READABLE;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_PROPERTY);
#endif
}

SODaSProperty::SODaSProperty(IN DWORD propertyId, IN LPCTSTR name)
	: SODaSItemTag(SOCMNOBJECT_TYPE_PROPERTY)
{
	setPropertyId(propertyId);

	if (name)
	{
		setName(name);
	}

	m_accessRightsAndDynamic = OPC_READABLE;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_PROPERTY);
#endif
}

SODaSProperty::~SODaSProperty(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_PROPERTY);
#endif
}


//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSProperty::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SODaSProperty::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// release
//
// - check if to delete dynamic created properties
//
// returns:
//		reference count
//
LONG SODaSProperty::release(void)
{
	if (getDynamic())
	{
		SOCmnPointer<SOCmnElement> par = getParentBranch();
		int nRefs = 2;
#ifdef SOFEATURE_WATCH

		if (getCollectChanges())
		{
			nRefs++;
		}

#endif

		if ((getRefCount() == nRefs) && (!!par))
		{
			LONG ret = SOCmnObject::release();
			SOCmnList<SOCmnElement> *pList = par->getAdditionalList();

			if (pList != NULL)
			{
				SOCmnList<SODaSProperty> propList = (SOCmnList<SODaSProperty> *)pList;

				if (propList.isInit())
				{
					propList.lock();

					if (getRefCount() == nRefs - 1)
					{
						DWORD propertyId = getNameInt();
						propList.removeKey(propertyId);
					}

					propList.unlock();
				}
			}

			return ret;
		}
	}

	return (SOCmnObject::release());
} // release


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
BOOL SODaSProperty::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	BOOL ret;
#ifdef SOFEATURE_WATCH
	BOOL doCollect = FALSE;
	SOCmnPointer<SOCmnElement> prevParent;
	BOOL stdProp = isStandardTagProperty(getPropertyId());

	if (!stdProp)
	{
		prevParent = getParentBranch();

		if (prevParent.isNotNull())
		{
			doCollect = prevParent->getCollectChanges();
		}
		else if (parent)
		{
			doCollect = parent->getCollectChanges();
		}
	}

#endif
	ret = SOCmnElement::setParentBranch(parent);
#ifdef SOFEATURE_WATCH

	if ((!stdProp) && (doCollect || (getCollectChanges())) && (ret))
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();

		if (parent != NULL)
		{
			entry->watchAddObject(parent, this);
			SOCmnList<SOCmnElement> addList(parent->getAdditionalList());
			SOCmnListPosition pos;
			pos  = addList.getStartPosition();

			if (pos)
			{
				addList.getNext(pos);

				if (pos == NULL)
				{
					// the new added property is the only one
					SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
					SOCmnString buffer;
					SOSrvWatch::attributeChange(parent, SOWATCH_ATTRIBUTE_HASPROPERTIES_T, conv->bool2string(buffer, TRUE));
				}
			}
		}
		else if ((parent == NULL) && (prevParent.isNotNull()))
		{
			entry->watchRemoveObject(prevParent, this);
			SOCmnList<SOCmnElement> addList(prevParent->getAdditionalList());

			if (addList.isEmpty())
			{
				SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
				SOCmnString buffer;
				SOSrvWatch::attributeChange(prevParent, SOWATCH_ATTRIBUTE_HASPROPERTIES_T, conv->bool2string(buffer, FALSE));
			}

			prevParent.release();
		}
	}

#endif
	return ret;
} // setParentBranch


//-----------------------------------------------------------------------------
// setDescription
//
// - set description
//
// returns:
//		TRUE
//
BOOL SODaSProperty::setDescription(
	IN LPCTSTR descr)   // description
{
	onSetDescription(descr);
	m_description = descr;
	return TRUE;
} // setDescription


//-----------------------------------------------------------------------------
// onSetDescription
//
// - called before description is set
// - default implementation: do nothing
//
void SODaSProperty::onSetDescription(
	IN LPCTSTR /* descr */) // description
{} // onSetDescription


//-----------------------------------------------------------------------------
// handleRequest
//
// - handle request for direct I/O
//
void SODaSProperty::handleRequest(
	IN SODaSRequest* request)   // request
{
	if (!handleStandardProperties(request))
	{
		SOCmnPointer<SOCmnElement> parent = getParentBranch();
		BOOL handled = FALSE;

		if (parent.isNotNull())
		{
			handled = ((SODaSItemTag*)(SOCmnElement*)parent)->handlePropertyRequest(this, request);
		}

		if (!handled)
		{
			_ASSERTION(FALSE, ("SODaSProperty::handleRequest must be implemented in child class for not standard properties"));
			SOCmnPointer<SODaSCache> cache = getCache();
			cache->setQuality(OPC_QUALITY_BAD, request);
			request->complete(E_FAIL);
		}
	}
} // handleRequest


//-----------------------------------------------------------------------------
// getDeviceIOMode
//
// - get the device I/O mode of the property
//
// returns:
//		device I/O mode
//
BYTE SODaSProperty::getDeviceIOMode(
	IN SODaSRequest* req)   // request
{
	// check standard properties
	if (!(m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE))
	{
		DWORD propertyId = getNameInt();

		if (((propertyId >= 2) && (propertyId <= 4)) || (propertyId == 2503))
		{
			return SODaSItemTag::delegate;
		}
	}

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

	if (!(m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE))
	{
		return SODaSItemTag::direct;
	}
	else
	{
		return m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE;
	}
} // getDeviceIOMode


//-----------------------------------------------------------------------------
// setPropertyId
//
// - set the property id
// - sets description and datatype for standard properties
//
// returns:
//		TRUE  - id set
//		FALSE - can't set id
//
BOOL SODaSProperty::setPropertyId(
	IN DWORD propertyId)    // property id
{
	BOOL ret = setElementName(propertyId);
	SOCmnString propName;
	setElementName(getStandardPropertyNameByID(propertyId, propName));

	// ID set 1 - OPC specific properties
	if ((propertyId >= 1) && (propertyId <= 8))
	{
		switch (propertyId)
		{
		case 1:
			setDescription(_T("Item Canonical DataType"));
			setNativeDatatype(VT_I2);
			break;

		case 2:
			setDescription(_T("Item Value"));
			break;

		case 3:
			setDescription(_T("Item Quality"));
			setNativeDatatype(VT_I2);
			break;

		case 4:
			setDescription(_T("Item Timestamp"));
			setNativeDatatype(VT_DATE);
			break;

		case 5:
			setDescription(_T("Item Access Rights"));
			setNativeDatatype(VT_I4);
			break;

		case 6:
			setDescription(_T("Server Scan Rate"));
			setNativeDatatype(VT_R4);
			break;

		case 7:
			setDescription(_T("Item EU Type"));
			setNativeDatatype(VT_I4);
			break;

		case 8:
			setDescription(_T("Item EU Info"));
			setNativeDatatype(VT_ARRAY | VT_BSTR);
			break;
		}
	}

	if ((propertyId >= 2501) && (propertyId <= 2508))
	{
		switch (propertyId)
		{
		case 2501:
			setDescription(_T("Item Canonical DataType"));
			setNativeDatatype(VT_BSTR);
			break;

		case 2503:
			setDescription(_T("Item Quality"));
			setNativeDatatype(VT_BSTR);
			break;

		case 2505:
			setDescription(_T("Item Access Rights"));
			setNativeDatatype(VT_BSTR);
			break;

		case 2507:
			setDescription(_T("Item EU Type"));
			setNativeDatatype(VT_BSTR);
			break;
		}
	}

	// ID set 2 - recommended properties
	if ((propertyId >= 100) && (propertyId <= 108))
	{
		switch (propertyId)
		{
		case 100:
			setDescription(_T("EU Units"));
			setNativeDatatype(VT_BSTR);
			break;

		case 101:
			setDescription(_T("Item Description"));
			setNativeDatatype(VT_BSTR);
			break;

		case 102:
			setDescription(_T("High EU"));
			setNativeDatatype(VT_R8);
			break;

		case 103:
			setDescription(_T("Low EU"));
			setNativeDatatype(VT_R8);
			break;

		case 104:
			setDescription(_T("High Instrument Range"));
			setNativeDatatype(VT_R8);
			break;

		case 105:
			setDescription(_T("Low Instrument Range"));
			setNativeDatatype(VT_R8);
			break;

		case 106:
			setDescription(_T("Contact Close Label"));
			setNativeDatatype(VT_BSTR);
			break;

		case 107:
			setDescription(_T("Contact Open Label"));
			setNativeDatatype(VT_BSTR);
			break;

		case 108:
			setDescription(_T("Item Timezone"));
			setNativeDatatype(VT_I4);
			break;
		}
	}

	// ID set 3 - operator display related properties
	if ((propertyId >= 200) && (propertyId <= 207))
	{
		switch (propertyId)
		{
		case 200:
			setDescription(_T("Default Display"));
			setNativeDatatype(VT_BSTR);
			break;

		case 201:
			setDescription(_T("Current Foreground Color"));
			setNativeDatatype(VT_I4);
			break;

		case 202:
			setDescription(_T("Current Background Color"));
			setNativeDatatype(VT_I4);
			break;

		case 203:
			setDescription(_T("Current Blink"));
			setNativeDatatype(VT_BOOL);
			break;

		case 204:
			setDescription(_T("BMP File"));
			setNativeDatatype(VT_BSTR);
			break;

		case 205:
			setDescription(_T("Sound File"));
			setNativeDatatype(VT_BSTR);
			break;

		case 206:
			setDescription(_T("HTML File"));
			setNativeDatatype(VT_BSTR);
			break;

		case 207:
			setDescription(_T("AVI File"));
			setNativeDatatype(VT_BSTR);
			break;
		}
	}

	return ret;
} // setPropertyId


//-----------------------------------------------------------------------------
// getPropertyData
//
// - get the data of the property
//
void SODaSProperty::getPropertyData(
	OUT SODaSPropertyData* pD)  // property data
{
	if (pD)
	{
		SOCmnString name = getDescription();
		pD->m_datatype = getNativeDatatype();

		if (!name.isEmpty())
		{
			pD->m_descr = name;
		}
		else
		{
			pD->m_descr = _T("");
		}

		pD->m_pid = getNameInt();
		pD->m_itemID = getItemID();
		pD->m_name = getName();
	}
} // getPropertyData


//-----------------------------------------------------------------------------
// isStandardTagProperty
//
// - check if the property id is a standard tag property id
//
// returns:
//		TRUE  - standard propery
//		FALSE - no standard propery
//
BOOL SODaSProperty::isStandardTagProperty(
	IN DWORD propId)    // property id
{
	return (((propId >= 1) && (propId <= 6)) || ((propId >= 2501) && (propId <= 2506)));
} // isStandardTagProperty


//-----------------------------------------------------------------------------
// isStandardNodeProperty
//
// - check if the property id is a standard node property id
//
// returns:
//		TRUE  - standard propery
//		FALSE - no standard propery
//
BOOL SODaSProperty::isStandardNodeProperty(
	IN DWORD /* propId */)  // property id
{
	return FALSE;
} // isStandardNodeProperty


//-----------------------------------------------------------------------------
// getStandardTagProperties
//
// - get the standard properties of a tag
//
void SODaSProperty::getStandardTagProperties(
	IN VARTYPE tagNativeDatatype,                   // native datatype of the tag
	IN BOOL isSOAP,
	OUT SOCmnList<SODaSPropertyData> &propertyList) // property data list
{
	SODaSPropertyData* propData;

	if (isSOAP)
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 2501;
		propData->m_name = _T("dataType");
		propData->m_datatype = VT_BSTR;
		propData->m_descr = _T("Item Canonical DataType");
		propertyList.add(propData);
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 1;
		propData->m_name = _T("dataType");
		propData->m_datatype = VT_I2;
		propData->m_descr = _T("Item Canonical DataType");
		propertyList.add(propData);
	}

	SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
	propData->m_pid = 2;
	propData->m_name = _T("value");
	propData->m_datatype = tagNativeDatatype;
	propData->m_descr = _T("Item Value");
	propertyList.add(propData);

	if (isSOAP)
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 2503;
		propData->m_name = _T("quality");
		propData->m_datatype = VT_BSTR;
		propData->m_descr = _T("Item Quality");
		propertyList.add(propData);
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 3;
		propData->m_name = _T("quality");
		propData->m_datatype = VT_I2;
		propData->m_descr = _T("Item Quality");
		propertyList.add(propData);
	}

	SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
	propData->m_pid = 4;
	propData->m_name = _T("timestamp");
	propData->m_datatype = VT_DATE;
	propData->m_descr = _T("Item Timestamp");
	propertyList.add(propData);

	if (isSOAP)
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 2505;
		propData->m_name = _T("accessRights");
		propData->m_datatype = VT_BSTR;
		propData->m_descr = _T("Item Access Rights");
		propertyList.add(propData);
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
		propData->m_pid = 5;
		propData->m_name = _T("accessRights");
		propData->m_datatype = VT_I4;
		propData->m_descr = _T("Item Access Rights");
		propertyList.add(propData);
	}

	SOCMN_ALLOCATE_OBJECT_C(propData, SODaSPropertyData, (SODaSPropertyData*))
	propData->m_pid = 6;
	propData->m_name = _T("scanRate");
	propData->m_datatype = VT_R4;
	propData->m_descr = _T("Server Scan Rate");
	propertyList.add(propData);
} // getStandardTagProperties


//-----------------------------------------------------------------------------
// getStandardNodeProperties
//
// - get the standard properties of a node
//
void SODaSProperty::getStandardNodeProperties(
	IN BOOL /* isSOAP */,
	OUT SOCmnList<SODaSPropertyData>& /* propertyList */)   // property data list
{
} // getStandardNodeProperties


//-----------------------------------------------------------------------------
// handleStandardProperties
//
// - handle requests for standard properties
//
// returns:
//		TRUE  - request handled
//		FALSE - request not handled
//
BOOL SODaSProperty::handleStandardProperties(
	IN SODaSRequest* request)   // request
{
	DWORD propertyId = getNameInt();

	if ((propertyId == 1) || (propertyId == 5) || (propertyId == 6) || (propertyId == 7) ||
		(propertyId == 2501) || (propertyId == 2505) || (propertyId == 2507) || (propertyId == 108))
	{
		SOCmnPointer<SODaSCache> cache = getCache();
		SOCmnVariant value;

		switch (propertyId)
		{
		case 1: // native datatype
		case 2501:
		{
			SOCmnPointer<SODaSTag> tag = (SODaSTag*)getParentBranch();

			if (!tag)
			{
				return FALSE;
			}

			VARTYPE vt = tag->getNativeDatatype();

			if (request->isDCOMRequest())
			{
				value = (SHORT)vt;
			}
			else
			{
				SOCmnString buffer;
				value = SOCmnVariant::getXMLDataType(vt, buffer);
			}

			cache->clearValue();
			cache->setDatatype(value.vt);
			cache->setValue(value, request);
		}
		break;

		case 5: // access rights
		case 2505:
		{
			SOCmnPointer<SODaSTag> tag = (SODaSTag*)getParentBranch();

			if (!tag)
			{
				return FALSE;
			}

			SOCmnPointer<SODaSServer> server = request->getServer();

			if (request->isDCOMRequest())
			{
				value = (LONG)tag->getAccessRights(server);
			}
			else
			{
				switch (tag->getAccessRights(server))
				{
				case OPC_READABLE:
					value = _T("readable");
					break;

				case OPC_WRITEABLE:
					value = _T("writable");
					break;

				case OPC_WRITEABLE | OPC_READABLE:
					value = _T("readWritable");
					break;

				default:
					value = _T("unknown");
					break;
				}
			}

			cache->clearValue();
			cache->setDatatype(value.vt);
			cache->setValue(value, request);
		}
		break;

		case 6: // server scan rate
		{
			SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
			value = (float)engine->getMinUpdateRate();
			cache->setValue(value, request);
		}
		break;

		case 7: // eu type
		case 2507:
		{
			SOCmnPointer<SODaSTag> tag = (SODaSTag*)getParentBranch();

			if (!tag)
			{
				return FALSE;
			}

			OPCEUTYPE euType = tag->getEUType();

			if (request->isDCOMRequest())
			{
				value = (LONG)euType;
			}
			else
			{
				switch (euType)
				{
				case OPC_ANALOG:
					value = _T("analog");
					break;

				case OPC_ENUMERATED:
					value = _T("enumerated");
					break;

				default:
					value = _T("noEnum");
					break;
				}
			}

			cache->clearValue();
			cache->setDatatype(value.vt);
			cache->setValue(value, request);
		}
		break;

		case 108: // item timezone
		{
			LONG timeBias = 0;
#ifdef SOOS_WINDOWS
			TIME_ZONE_INFORMATION timeZone;
			DWORD timeZoneRet;
			// set system default time zone
			timeZoneRet = GetTimeZoneInformation(&timeZone);

			if (timeZoneRet == TIME_ZONE_ID_DAYLIGHT)
			{
				timeBias = timeZone.Bias + timeZone.StandardBias + timeZone.DaylightBias;
			}
			else
			{
				timeBias = timeZone.Bias + timeZone.StandardBias;
			}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			time_t t = time(&t);
			struct tm locT, sysT;
			gmtime_r(&t, &sysT);
			localtime_r(&t, &locT);
			sysT.tm_isdst = locT.tm_isdst; /* <-- very important!!! */
			timeBias = mktime(&locT) - mktime(&sysT); /* CET=3600, CEST=7200 */
			timeBias /= 60; /* convert to minutes */
#endif
			value = (LONG)timeBias;
			cache->setValue(value, request);
		}
		break;
		}

		cache->setQuality(OPC_QUALITY_GOOD, request);
		request->complete();
		return TRUE;
	}

	return FALSE;
} // handleStandardProperties


//-----------------------------------------------------------------------------
// getItemID
//
// - get the item id of the item tag
//
// returns:
//		the item ID of the object
//
SOCmnString SODaSProperty::getItemID(void)
{
	SOCmnPointer<SOCmnElement> parent = getParentBranch();

	if ((parent.isNotNull()) && (getPropertyId() > 99) && (!((getPropertyId() > 2500) && (getPropertyId() < 2599))))
	{
		SOCmnPointer<SODaSEntry> entry;
		TCHAR propDelim;
		SOCmnString propItemID;
		entry = ::getSODaSEntry();
		propDelim = entry->getPropertyDelimiter();
		propItemID.format(_T("%s%c%lu"), (LPCTSTR)parent->getFullName(), propDelim, getPropertyId());
		return propItemID;
	}

	return SOCmnString(_T(""));
} // getItemID


DWORD SODaSProperty::getStandardPropertyIDByName(IN LPCTSTR propName)
{
	if (!propName)
	{
		return 0;
	}

	switch (*propName)
	{
	case _T('a'):
		if (_tcsicmp(propName, _T("accessRights")) == 0)
		{
			return 2505;
		}

		break;

	case _T('c'):
		if (_tcsicmp(propName, _T("closeLabel")) == 0)
		{
			return 106;
		}

		break;

	case _T('d'):
		if (_tcsicmp(propName, _T("dataType")) == 0)
		{
			return 2501;
		}
		else if (_tcsicmp(propName, _T("description")) == 0)
		{
			return 101;
		}

		break;

	case _T('e'):
		if (_tcsicmp(propName, _T("euType")) == 0)
		{
			return 2507;
		}
		else if (_tcsicmp(propName, _T("euInfo")) == 0)
		{
			return 8;
		}
		else if (_tcsicmp(propName, _T("engineeringUnits")) == 0)
		{
			return 100;
		}

		break;

	case _T('h'):
		if (_tcsicmp(propName, _T("highEU")) == 0)
		{
			return 102;
		}
		else if (_tcsicmp(propName, _T("highIR")) == 0)
		{
			return 104;
		}

		break;

	case _T('l'):
		if (_tcsicmp(propName, _T("lowEU")) == 0)
		{
			return 103;
		}
		else if (_tcsicmp(propName, _T("lowIR")) == 0)
		{
			return 105;
		}

		break;

	case _T('o'):
		if (_tcsicmp(propName, _T("openLabel")) == 0)
		{
			return 107;
		}

		break;

	case _T('q'):
		if (_tcsicmp(propName, _T("quality")) == 0)
		{
			return 2503;
		}

		break;

	case _T('s'):
		if (_tcsicmp(propName, _T("scanRate")) == 0)
		{
			return 6;
		}

		break;

	case _T('t'):
		if (_tcsicmp(propName, _T("timestamp")) == 0)
		{
			return 4;
		}
		else if (_tcsicmp(propName, _T("timeZone")) == 0)
		{
			return 108;
		}

		break;

	case _T('v'):
		if (_tcsicmp(propName, _T("value")) == 0)
		{
			return 2;
		}

		break;
	}

	return 0;
}

LPCTSTR SODaSProperty::getStandardPropertyNameByID(IN DWORD propID, OUT SOCmnString& propName)
{
	switch (propID)
	{
	case 1:
	case 2501:
		propName = _T("dataType");
		break;

	case 2:
		propName = _T("value");
		break;

	case 3:
	case 2503:
		propName = _T("quality");
		break;

	case 4:
		propName = _T("timestamp");
		break;

	case 5:
	case 2505:
		propName = _T("accessRights");
		break;

	case 6:
		propName = _T("scanRate");
		break;

	case 7:
		propName = _T("euType");
		break;

	case 2507:
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


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSProperty::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOCmnPointer<SOCmnElement> parent = getParentBranch();
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_NAMESPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_PROPERTY);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOCmnString buffer;
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnPointer<SODaSCache> cache = getCache();
		WORD quality;
		SOCmnDateTime timeStamp;
		SOCmnStringEx szRawAttr;
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());

		if (parent.isNotNull())
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, parent->getFullName());
		}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PROPERTYID_T, getPropertyId());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ITEMID_T, getItemID());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VALUE_T, cache->getValueString(buffer));
		quality = cache->getQuality();
		szRawAttr.format(_T("%s = \"%u\""), SOWATCH_ATTR_RAW_T, quality);
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_QUALITY_T, conv->quality2string(buffer, quality), szRawAttr);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, getNativeDatatype()));
		cache->getTimeStamp(timeStamp);
		szRawAttr.format(_T("%s = \"%lu\""), SOWATCH_ATTR_RAW_T, DWORD(timeStamp.getMicroseconds() / 1000));
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_TIMESTAMP_T, conv->datetime2string(buffer, timeStamp), szRawAttr);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACCESSRIGHTS_T, conv->accessRights2string(buffer, getAccessRights()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DESCRIPTION_T, getDescription());

		if (getEUType() != OPC_NOENUM)
		{
			SOCmnVariant euInfo;
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_EUTYPE_T, conv->euType2string(buffer, getEUType()));
			getEUInfo(euInfo);
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_EUINFO_T, variant2string(&buffer, euInfo));
		}

		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SODaSPropertyData                                                          |
//-----------------------------------------------------------------------------

SODaSPropertyData::SODaSPropertyData()
	: SOCmnData()
{
	m_result = S_OK;
	m_pid = 0;
	m_datatype = VT_EMPTY;
	m_accessRights = OPC_READABLE;
}

const SODaSPropertyData& SODaSPropertyData::operator=(
	IN SODaSPropertyData& propData)
{
	m_datatype = propData.m_datatype;
	m_descr = propData.m_descr;
	m_pid = propData.m_pid;
	m_itemID = propData.m_itemID;
	m_name = propData.m_name;
	m_result = propData.m_result;
	m_value = propData.m_value;
	m_accessRights = propData.m_accessRights;
	return *this;
}

#endif // SODAS
