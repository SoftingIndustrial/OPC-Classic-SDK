#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>

//-----------------------------------------------------------------------------
// SOAeSCategory                                                              |
//-----------------------------------------------------------------------------

SOAeSCategory::SOAeSCategory(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_CATEGORY)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CATEGORY);
#endif
}

SOAeSCategory::SOAeSCategory(
	IN DWORD categoryID,            // category ID
	IN LPCTSTR categoryDescription, // category decription
	IN DWORD eventType)             // event type
	:  SOCmnElement(SOCMNOBJECT_TYPE_CATEGORY)
{
	setName(categoryID);
	setDescription(categoryDescription);
	setEventType(eventType);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CATEGORY);
#endif
}

SOAeSCategory::~SOAeSCategory(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_CATEGORY);
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
SOCmnObject* SOAeSCategory::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSCategory::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addAdditionalElement
//
// - adds element to additional list, if element is a event attribute
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSCategory::addAdditionalElement(
	IN SOCmnElement* newEl)
{
	if (!newEl)
	{
		return FALSE;
	}

	if (newEl->is(SOCMNOBJECT_TYPE_ATTRIBUTE))
	{
		return SOCmnElement::addAdditionalElement(newEl);
	}
	else
	{
		return FALSE;
	}
} // addAdditionalElement


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a condition class
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSCategory::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_CONDITION_CLASS))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// setDescription
//
// - set description
//
// returns:
//		TRUE
//
BOOL SOAeSCategory::setDescription(
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
void SOAeSCategory::onSetDescription(
	IN LPCTSTR /* descr */) // description
{} // onSetDescription


//-----------------------------------------------------------------------------
// setEventType
//
// - set event type
//
// returns:
//		TRUE
//
BOOL SOAeSCategory::setEventType(
	IN DWORD eventType) // event type
{
	onSetEventType(eventType);
	m_eventType = eventType;
	return TRUE;
} // setEventType


//-----------------------------------------------------------------------------
// onSetEventType
//
// - called before event type is set
// - default implementation: do nothing
//
void SOAeSCategory::onSetEventType(
	IN DWORD /* eventType */)   // event type
{} // onSetEventType


//-----------------------------------------------------------------------------
// getAttribute
//
// - get attribute object of the category
//
// returns:
//		attribute object
//
SOAeSAttribute* SOAeSCategory::getAttribute(
	IN DWORD attributeID)
{
	SOCmnList<SOCmnElement> attrList(getAdditionalList());
	return (SOAeSAttribute*)attrList.findKey(attributeID);
} // getAttribute


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSCategory::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_EVENTSPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_CATEGORY);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ID_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DESCRIPTION_T, getDescription());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_EVENTTYPE_T, conv->eventTypeFilter2string(buffer, getEventType()));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData
#endif // SOFEATURE_WATCH




//-----------------------------------------------------------------------------
// SOAeSEventSpaceRoot                                                        |
//-----------------------------------------------------------------------------

SOAeSEventSpaceRoot::SOAeSEventSpaceRoot(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_ROOT)
{
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSEventSpaceRoot::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIName


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSEventSpaceRoot::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a category
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSEventSpaceRoot::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_CATEGORY))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// findCategory
//
// - find category in event space
//
// returns:
//		pointer to category object
//		NULL - no cateory with the id in the event space
//
SOAeSCategory* SOAeSEventSpaceRoot::findCategory(
	IN DWORD eventCategory) // category id
{
	SOCmnList<SOCmnElement> catList(getBranchList());
	SOAeSCategory* cat;
	// get event category
	cat = (SOAeSCategory*)catList.findKey(eventCategory);
	return cat;
} // findCategory


//-----------------------------------------------------------------------------
// findConditionClass
//
// - find condition in event space
//
// returns:
//		pointer to condition object
//		NULL - no condition with the name in the event space
//
SOAeSConditionClass* SOAeSEventSpaceRoot::findConditionClass(
	IN LPCTSTR conditionName)   // condition name
{
	if (!conditionName)
	{
		return NULL;
	}

	SOCmnList<SOCmnElement> catList(getBranchList());
	SOAeSCategory* cat;
	SOCmnList<SOCmnElement> condList;
	SOAeSConditionClass* cond;
	SOCmnListPosition pos;
	pos = catList.getStartPosition();

	while (pos)
	{
		// get event category
		cat = (SOAeSCategory*)catList.getNext(pos);
		// search for condition name
		condList = cat->getBranchList();
		cond = (SOAeSConditionClass*)condList.findKey(conditionName);

		if (cond)
		{
			return cond;
		}
	}

	return NULL;
} // findConditionClass


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSEventSpaceRoot::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_EVENTSPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_EVENTSPACEROOT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#endif // SOAES
