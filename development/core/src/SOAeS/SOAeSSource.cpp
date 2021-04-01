#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>


//-----------------------------------------------------------------------------
// SOAeSSource                                                                |
//-----------------------------------------------------------------------------

SOAeSSource::SOAeSSource(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_SOURCE)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SOURCE);
#endif
}

SOAeSSource::SOAeSSource(
	IN LPCTSTR sourceName) // soruce name
	:  SOCmnElement(SOCMNOBJECT_TYPE_SOURCE)
{
	setName(sourceName);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SOURCE);
#endif
}

SOAeSSource::~SOAeSSource(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_SOURCE);
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
SOCmnObject* SOAeSSource::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// addAdditionalElement
//
// - adds element to additional  list, if element is a condition
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSSource::addAdditionalElement(
	IN SOCmnElement* newEl)
{
	if (!newEl)
	{
		return FALSE;
	}

	if (newEl->is(SOCMNOBJECT_TYPE_CONDITION))
	{
		return SOCmnElement::addAdditionalElement(newEl);
	}
	else
	{
		return FALSE;
	}
} // addAdditionalElement


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SOAeSSource::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the object
//
SOCmnString SOAeSSource::getDeviceState(void)
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
SOCmnString SOAeSSource::getDeviceStateInfo(void)
{
	return SOCmnString(_T(""));
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSSource::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_AREASPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_SOURCE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SOAeSSourceDefault                                                         |
//-----------------------------------------------------------------------------

SOAeSSourceDefault::SOAeSSourceDefault()
{
}

SOAeSSourceDefault::SOAeSSourceDefault(
	IN LPCTSTR sourceName) // source name
{
	setName(sourceName);
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSSourceDefault::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList

#endif // SOAES
