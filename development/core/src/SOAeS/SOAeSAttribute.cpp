#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>

static TCHAR s_emptyString[2] = _T("");

//-----------------------------------------------------------------------------
// SOAeSAttribute                                                             |
//-----------------------------------------------------------------------------

SOAeSAttribute::SOAeSAttribute(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_ATTRIBUTE)
{
	m_datatype = VT_EMPTY;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE);
#endif
}

SOAeSAttribute::SOAeSAttribute(
	IN DWORD attributeID,               // attribute ID
	IN LPCTSTR attributeDescription,    // attribute decription
	IN VARTYPE datatype)                // event type
	:  SOCmnElement(SOCMNOBJECT_TYPE_ATTRIBUTE)
{
	setName(attributeID);
	setDescription(attributeDescription);
	setDatatype(datatype);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE);
#endif
}

SOAeSAttribute::~SOAeSAttribute(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE);
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
SOCmnObject* SOAeSAttribute::getObjectCmnElementIName(void)
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
BOOL SOAeSAttribute::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setDescription
//
// - set description
//
// returns:
//		TRUE
//
BOOL SOAeSAttribute::setDescription(
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
void SOAeSAttribute::onSetDescription(
	IN LPCTSTR /* descr */) // description
{} // onSetDescription


//-----------------------------------------------------------------------------
// setDatatype
//
// - set datatype
//
// returns:
//		TRUE
//
BOOL SOAeSAttribute::setDatatype(
	IN VARTYPE datatype)    // datatype
{
	onSetDatatype(datatype);
	m_datatype = datatype;
	return TRUE;
} // setDatatype


//-----------------------------------------------------------------------------
// onSetDatatype
//
// - called before datatype is set
// - default implementation: do nothing
//
void SOAeSAttribute::onSetDatatype(
	IN VARTYPE /* datatype */)  // datatype
{} // onSetDatatype


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSAttribute::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_EVENTSPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_ATTRIBUTE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ID_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DESCRIPTION_T, getDescription());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, getDatatype()));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SOAeSAttributeValue                                                        |
//-----------------------------------------------------------------------------

SOAeSAttributeValue::SOAeSAttributeValue(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE);
#endif
}

SOAeSAttributeValue::SOAeSAttributeValue(
	IN SOAeSAttribute* attribute)
	:  SOCmnElement(SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE)
{
	setAttribute(attribute);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE);
#endif
}

SOAeSAttributeValue::~SOAeSAttributeValue(void)
{
	setAttribute(NULL);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE);
#endif
}



//-----------------------------------------------------------------------------
// setAttribute
//
// - set attribute
//
// returns:
//		TRUE
//
BOOL SOAeSAttributeValue::setAttribute(
	IN SOAeSAttribute* attribute)
{
	onSetAttribute(attribute);
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_attribute = attribute;
	m_attribute.addRef();
	return TRUE;
} // setAttribute


//-----------------------------------------------------------------------------
// onSetSeverity
//
// - called before severity is set
// - default implementation: do nothing
//
void SOAeSAttributeValue::onSetAttribute(
	IN SOAeSAttribute* /* attribute */) // attribute
{} // onSetAttribute


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key with given string
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOAeSAttributeValue::hasKey(
	IN LPCTSTR key) // object key
{
	SOCmnPointer<SOAeSAttribute> attribute = getAttribute();

	if (!attribute)
	{
		return FALSE;
	}

	SOCmnString myKey;
	myKey.format(_T("%lu"), attribute->getNameInt());
	return (key == myKey);
} // hasKey


BOOL SOAeSAttributeValue::hasKey(
	IN DWORD key)   // object key
{
	SOCmnPointer<SOAeSAttribute> attribute = getAttribute();

	if (!attribute)
	{
		return FALSE;
	}

	return (key == attribute->getNameInt());
} // hasKey


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key
//
// returns:
//		TRUE  - returned object key
//      FALSE - no attribute set
//
BOOL SOAeSAttributeValue::getKey(OUT SOCmnString& key)
{
	SOCmnPointer<SOAeSAttribute> attribute = getAttribute();

	if (!attribute)
	{
		return FALSE;
	}

	key.format(_T("%lu"), attribute->getNameInt());
	return TRUE;
} // getKey

BOOL SOAeSAttributeValue::getKey(OUT DWORD& key)
{
	SOCmnPointer<SOAeSAttribute> attribute = getAttribute();

	if (!attribute)
	{
		return FALSE;
	}

	key = attribute->getNameInt();
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// getValue
//
// - get attribute value
//
// returns:
//		TRUE  - copied value
//		FALSE - error copy value
//
BOOL SOAeSAttributeValue::getValue(
	OUT LPVARIANT value)    // value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	return m_value.copyTo(value);
} // getValue


//-----------------------------------------------------------------------------
// getValue
//
// - get attribute value in a given datatype
//
// returns:
//		FALSE
//
BOOL SOAeSAttributeValue::getValue(
	IN VARTYPE typeWanted,  // datatype
	OUT LPVARIANT value)    // value
{
	BOOL ok = getValue(value);

	if ((typeWanted != VT_EMPTY) && (typeWanted != value->vt))
	{
		ok |= changeDatatype(value, typeWanted, value);
	}

	return ok;
} // getValue


//-----------------------------------------------------------------------------
// changeDatatype
//
// - change the datatype of a value
//
// returns:
//		FALSE
//
BOOL SOAeSAttributeValue::changeDatatype(
	IN VARIANT* value,          // source value
	IN VARTYPE type,            // wanted datatype
	OUT VARIANT* changedValue)  // destination value
{
	return SOCmnVariant::changeDatatype(value, type, changedValue);
} // changeDatatype


//-----------------------------------------------------------------------------
// setValue
//
// - set attribute value
//
// returns:
//		TRUE
//
BOOL SOAeSAttributeValue::setValue(
	IN LPVARIANT value) // value
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	m_value = value;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
#ifdef SOFEATURE_WATCH

	if (getCollectChanges() & SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES)
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_VALUE_T, conv->variant2string(buffer, value));
	}

#endif
	return TRUE;
} // setValue


//-----------------------------------------------------------------------------
// connectValueUpdate
//
// - establish connection for attribute value update
//
// returns:
//		FALSE - the default attribute values don't support value update
//
BOOL SOAeSAttributeValue::connectValueUpdate(
	IN SOAeSConditionGroup* /* condGrp */)  // condition group
{
	return FALSE;
} // connectValueUpdate


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the object
//
SOCmnString SOAeSAttributeValue::getDeviceState(void)
{
	return SOCmnString(SOWATCH_DEVSTATE_OK);
} // getDeviceState


//-----------------------------------------------------------------------------
// getDeviceStateInfo
//
// - get state info of the device represented by the object
//
// returns:
//		the device state info  of the object
//
SOCmnString SOAeSAttributeValue::getDeviceStateInfo(void)
{
	return SOCmnString(_T(""));
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSAttributeValue::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_AREASPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_ATTRIBUTEVALUE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnPointer<SOAeSAttribute> attribute = getAttribute();
		SOCmnString buffer;

		if (attribute.isNotNull())
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ID_T, attribute->getName());
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DESCRIPTION_T, attribute->getDescription());
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
			SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
			pObjectLock->lock();
			conv->variant2string(buffer, m_value);
			pObjectLock->unlock();
			SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VALUE_T, buffer);
			SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, attribute->getDatatype()));
		}

		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SOAeSAttributeValueDADescription                                           |
//-----------------------------------------------------------------------------

SOAeSAttributeValueDADescription::SOAeSAttributeValueDADescription(void)
{
	memset(&m_clsid, 0, sizeof(CLSID));
}

SOAeSAttributeValueDADescription::~SOAeSAttributeValueDADescription(void)
{}

#endif // SOAES
