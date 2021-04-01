#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>
#include <opcerror.h>

static TCHAR s_emptyString[2] = _T("");


//-----------------------------------------------------------------------------
// SOAeSEvent                                                                 |
//-----------------------------------------------------------------------------

SOAeSEvent::SOAeSEvent(void)
	:  SOCmnObject(SOCMNOBJECT_TYPE_EVENT)
{
}


//-----------------------------------------------------------------------------
// fire
//
// - fire the event. Give it to the subscriptions to send it to the clients
//
// returns:
//		TRUE
//
BOOL SOAeSEvent::fire(
	IN OPTIONAL BYTE serverInstance)
{
	SOCmnList<SOAeSEvent> eventList;
	BOOL ret;
	eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);
	eventList.add(this);
	ret = fireEvents(&eventList, serverInstance);
	eventList.destroy();
	return ret;
} // fire


//-----------------------------------------------------------------------------
// fireEvents
//
// - fire the event of the event list.
//   Give it to the subscriptions to send it to the clients
//
// returns:
//		TRUE
//
BOOL SOAeSEvent::fireEvents(
	IN SOCmnList<SOAeSEvent> *eventList,    // events
	IN OPTIONAL BYTE serverInstance)
{
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOSrvObjectRoot> objectTree = entry->getObjectRoot();
	SOCmnList<SOCmnElement> serverList(objectTree->getBranchList());
	SOCmnPointer<SOAeSServer> server;
	SOCmnListPosition serverPos;
	SOCmnList<SOCmnElement> subList;
	SOCmnPointer<SOAeSSubscription> sub;
	SOCmnListPosition subPos;
#ifdef SOFEATURE_WATCH
	SOCmnListPosition statPos;
	SOAeSEvent* statEvent;
	statPos = eventList->getStartPosition();

	while (statPos)
	{
		statEvent = eventList->getNext(statPos);
		SOAeSWatchHandler::doEventStatistics(statEvent);
	}

#endif
	serverList.dontAutoLock();
	serverList.lock();
	serverList.setDirty(FALSE);
	serverPos = serverList.getStartPosition();
	serverList.unlock();

	while (serverPos)
	{
		serverList.lock();

		if (serverList.isDirty())
		{
			// check current position!
			SOCmnListPosition checkPos;
			BOOL found = FALSE;
			checkPos = serverList.getStartPosition();

			while (checkPos)
			{
				if (checkPos == serverPos)
				{
					found = TRUE;
					break;
				}

				serverList.getNext(checkPos);
			}

			if (!found)
			{
				serverPos = serverList.getStartPosition();
			}

			serverList.setDirty(FALSE);

			if (serverPos == NULL)
			{
				serverList.unlock();
				break;
			}
		}

		server = (SOAeSServer*)serverList.getNext(serverPos);
		server.addRef();
		serverList.unlock();

		if (server->getServerInstance() == serverInstance)
		{
			subList = server->getLeafList();
			subList.dontAutoLock();
			subList.lock();
			subList.setDirty(FALSE);
			subPos = subList.getStartPosition();
			subList.unlock();

			while (subPos)
			{
				subList.lock();

				if (subList.isDirty())
				{
					// check current position!
					SOCmnListPosition checkPos;
					BOOL found = FALSE;
					checkPos = subList.getStartPosition();

					while (checkPos)
					{
						if (checkPos == subPos)
						{
							found = TRUE;
							break;
						}

						subList.getNext(checkPos);
					}

					if (!found)
					{
						subPos = subList.getStartPosition();
					}

					subList.setDirty(FALSE);

					if (subPos == NULL)
					{
						subList.unlock();
						break;
					}
				}

				sub = (SOAeSSubscription*)subList.getNext(subPos);
				sub.addRef();
				subList.unlock();
				sub->fireEvents(eventList);
			}

			subList.autoUnlock();
		}
	}

	return TRUE;
} // fireEvents


//-----------------------------------------------------------------------------
// addAttributeValue
//
// - add an attribute value to the event
//
// returns:
//		TRUE
//
BOOL SOAeSEvent::addAttributeValue(
	IN DWORD attributeID,   // attribute id
	IN LPVARIANT value)     // value
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOCmnList<SOAeSAttributeValue> attrValList(getAttributeValueList());
	SOCmnPointer<SOAeSCategory> cat = getCategory();
	// ASSERT: before adding attributes to the event the category must be set!
	_ASSERTION((SOAeSCategory*)cat, "Before adding attribute values to the event the category must be set!");

	if (!cat)
	{
		return FALSE;
	}

	SOCmnPointer<SOAeSAttribute> attr = cat->getAttribute(attributeID);

	if (!attr)
	{
		return FALSE;    // invalid attribute for this category
	}

	// initialize attribute value
	SOCmnPointer<SOAeSAttributeValue> attrVal = creator->createAttributeValue();
	attrVal->setAttribute(attr);
	attrVal->setValue(value);
	// add attribute value
	attrValList.add(attrVal);
	return TRUE;
} // addAttributeValue


//-----------------------------------------------------------------------------
// createSimpleEvent
//
// - create a simple event and initialize it
//
// returns:
//		new created event
//
SOAeSEvent* SOAeSEvent::createSimpleEvent(
	IN DWORD categoryID,            // category id
	IN LPCTSTR sourceName,          // source name
	IN DWORD severity,              // serverity
	IN LPCTSTR message,             // event message
	OPTIONAL IN LPFILETIME ocTime)  // occurrence time
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOAeSEvent* event = creator->createSimpleEvent();
	FILETIME now;

	if (ocTime == NULL)
	{
		::GetSystemTimeAsFileTime(&now);
		ocTime = &now;
	}

	event->setSourceName(sourceName);
	event->setOccurrenceTime(ocTime);
	event->setMessage(message);

	if (!event->setCategoryByID(categoryID))
	{
		event->release();
		return NULL;
	}

	event->setSeverity(severity);
	return event;
} // createSimpleEvent


//-----------------------------------------------------------------------------
// createTrackingEvent
//
// - create a tracking event and initialize it
//
// returns:
//		new created event
//
SOAeSEvent* SOAeSEvent::createTrackingEvent(
	IN DWORD categoryID,            // category id
	IN LPCTSTR sourceName,          // source name
	IN DWORD severity,              // serverity
	IN LPCTSTR message,             // event message
	IN LPCTSTR actorID,             // actor id
	OPTIONAL IN LPFILETIME ocTime)  // occurrence time
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOAeSEvent* event = creator->createTrackingEvent();
	FILETIME now;

	if (ocTime == NULL)
	{
		::GetSystemTimeAsFileTime(&now);
		ocTime = &now;
	}

	event->setSourceName(sourceName);
	event->setOccurrenceTime(ocTime);
	event->setMessage(message);

	if (!event->setCategoryByID(categoryID))
	{
		event->release();
		return NULL;
	}

	event->setSeverity(severity);
	event->setActorID(actorID);
	return event;
} // createTrackingEvent




//-----------------------------------------------------------------------------
// SOAeSEventIAttr                                                            |
//-----------------------------------------------------------------------------

SOAeSEventIAttr::SOAeSEventIAttr(void)
{}

SOAeSEventIAttr::~SOAeSEventIAttr(void)
{}

//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOAeSEventIAttr::getIntTraceId(void)
{
	SOCmnObject* obj = getObjectAeSEventIAttr();

	if (obj != NULL)
	{
		return obj->getObjTraceId();
	}

	return s_emptyString;
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectAeSEventIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSEventIAttr::getObjectAeSEventIAttr(void)
{
	return NULL;
} // getObjectAeSEventIAttr


//-----------------------------------------------------------------------------
// setSourceName
//
// - set source name
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setSourceName(
	IN LPCTSTR /* sourceName */)    // source name
{
	return FALSE;    // setSourceName
}


//-----------------------------------------------------------------------------
// getSourceName
//
// - get source name
//
// returns:
//		empty string
//
SOCmnString SOAeSEventIAttr::getSourceName(void)
{
	return SOCmnString(_T(""));    // getSourceName
}


//-----------------------------------------------------------------------------
// setOccurrenceTime
//
// - set occurrence time
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setOccurrenceTime(
	IN LPFILETIME /* time */)   // occurrence time
{
	return FALSE;    // setOccurrenceTime
}


//-----------------------------------------------------------------------------
// getOccurrenceTime
//
// - get occurrence time
//
// returns:
//		TRUE  - time returned
//      FALSE - bad argument
//
BOOL SOAeSEventIAttr::getOccurrenceTime(
	OUT LPFILETIME time)    // occurrence time
{
	if (time)
	{
		time->dwHighDateTime = 0;
		time->dwLowDateTime = 0;
		return TRUE;
	}

	return FALSE;
} // getOccurrenceTime


//-----------------------------------------------------------------------------
// setMessage
//
// - set message
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setMessage(
	IN LPCTSTR /* message */)   // message
{
	return FALSE;    // setMessage
}


//-----------------------------------------------------------------------------
// getMessage
//
// - get message
//
// returns:
//		empty string
//
SOCmnString SOAeSEventIAttr::getMessage(void)
{
	return SOCmnString(_T(""));    // getMessage
}


//-----------------------------------------------------------------------------
// setCategory
//
// - set category
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setCategory(
	IN SOAeSCategory* /* category */)   // category
{
	return FALSE;    // setCategory
}


//-----------------------------------------------------------------------------
// setCategoryByID
//
// - set category by the id of the category
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setCategoryByID(
	IN DWORD /* categoryID */)  // category id
{
	return FALSE;    // setCategoryByID
}


//-----------------------------------------------------------------------------
// getCategory
//
// - get category
//
// returns:
//		NULL
//
SOAeSCategory* SOAeSEventIAttr::getCategory(void)
{
	return NULL;    // getCategory
}


//-----------------------------------------------------------------------------
// setSeverity
//
// - set severity
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setSeverity(
	IN DWORD /* severity */)    // severity
{
	return FALSE;    // setSeverity
}


//-----------------------------------------------------------------------------
// getSeverity
//
// - get severity
//
// returns:
//		1
//
DWORD SOAeSEventIAttr::getSeverity(void)
{
	return 1;
}


//-----------------------------------------------------------------------------
// getAttributeValueList
//
// - get attribute values
//
// returns:
//		NULL
//
SOCmnList<SOAeSAttributeValue>* SOAeSEventIAttr::getAttributeValueList(void)
{
	return NULL;    //  getAttributeValueList
}


//-----------------------------------------------------------------------------
// setActorID
//
// - set actor id
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setActorID(
	IN LPCTSTR /* actorID */)   // actor id
{
	return FALSE;    // setActorID
}


//-----------------------------------------------------------------------------
// getActorID
//
// - get actor id
//
// returns:
//		empty string
//
SOCmnString SOAeSEventIAttr::getActorID(void)
{
	return SOCmnString(_T(""));    // getActorID
}


//-----------------------------------------------------------------------------
// setConditionName
//
// - set condition name
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setConditionName(
	IN LPCTSTR /* conditionName */) // condition name
{
	return FALSE;    // setConditionName
}


//-----------------------------------------------------------------------------
// getConditionName
//
// - get condition name
//
// returns:
//		empty string
//
SOCmnString SOAeSEventIAttr::getConditionName(void)
{
	return SOCmnString(_T(""));    // getConditionName
}


//-----------------------------------------------------------------------------
// setSubConditionName
//
// - set subcondition name
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setSubConditionName(
	IN LPCTSTR /* subConditionName */)  // subcondition name
{
	return FALSE;    // setSubConditionName
}


//-----------------------------------------------------------------------------
// getSubConditionName
//
// - get subcondition name
//
// returns:
//		empty string
//
SOCmnString SOAeSEventIAttr::getSubConditionName(void)
{
	return SOCmnString(_T(""));    // getSubConditionName
}


//-----------------------------------------------------------------------------
// setChangeMask
//
// - set change mask
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setChangeMask(
	IN WORD /* changeMask */)   // change mask
{
	return FALSE;    // setChangeMask
}


//-----------------------------------------------------------------------------
// getChangeMask
//
// - get change mask
//
// returns:
//		0
//
WORD SOAeSEventIAttr::getChangeMask(void)
{
	return 0;    // getChangeMask
}


//-----------------------------------------------------------------------------
// setNewState
//
// - set new state
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setNewState(
	IN WORD /* newState */) // new state
{
	return FALSE;    // setNewState
}


//-----------------------------------------------------------------------------
// getNewState
//
// - get new state
//
// returns:
//		0
//
WORD SOAeSEventIAttr::getNewState(void)
{
	return 0;    // getNewState
}


//-----------------------------------------------------------------------------
// setQuality
//
// - set quality
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setQuality(
	IN WORD /* quality */)  // quality
{
	return FALSE;    // setQuality
}


//-----------------------------------------------------------------------------
// getQuality
//
// - get quality
//
// returns:
//		0
//
WORD SOAeSEventIAttr::getQuality(void)
{
	return OPC_QUALITY_GOOD;    // getQuality
}


//-----------------------------------------------------------------------------
// setAckRequired
//
// - set ack required
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setAckRequired(
	IN BOOL /* ackRequired */)  // ack required
{
	return FALSE;    // setAckRequired
}


//-----------------------------------------------------------------------------
// getAckRequired
//
// - get ack required
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::getAckRequired(void)
{
	return FALSE;    // getAckRequired
}


//-----------------------------------------------------------------------------
// setActiveTime
//
// - set active time
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setActiveTime(
	IN LPFILETIME /* time */)   // active time
{
	return FALSE;    // setActiveTime
}


//-----------------------------------------------------------------------------
// getActiveTime
//
// - get active time
//
// returns:
//		empty string
//
BOOL SOAeSEventIAttr::getActiveTime(
	OUT LPFILETIME time)    // active time
{
	if (time)
	{
		time->dwHighDateTime = 0;
		time->dwLowDateTime = 0;
		return TRUE;
	}

	return FALSE;
} // getActiveTime


//-----------------------------------------------------------------------------
// setCookie
//
// - set cookie
//
// returns:
//		FALSE
//
BOOL SOAeSEventIAttr::setCookie(
	IN DWORD /* cookie */)  // cookie
{
	return FALSE;    // setCookie
}


//-----------------------------------------------------------------------------
// getCookie
//
// - get cookie
//
// returns:
//		event object address
//
DWORD SOAeSEventIAttr::getCookie(void)
{
	return (DWORD)getObjectAeSEventIAttr()->getOpcHandle();
} // getCookie




//-----------------------------------------------------------------------------
// SOAeSEventAttrSimple                                                       |
//-----------------------------------------------------------------------------

SOAeSEventAttrSimple::SOAeSEventAttrSimple(void)
{
	m_occurenceTime.dwHighDateTime = 0;
	m_occurenceTime.dwLowDateTime = 0;
	m_severity = 1;
	m_attrValList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
}

SOAeSEventAttrSimple::~SOAeSEventAttrSimple(void)
{
	setCategory(NULL);
	m_attrValList.destroy();
}

//-----------------------------------------------------------------------------
// setSourceName
//
// - set source name
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrSimple::setSourceName(
	IN LPCTSTR sourceName)  // source name
{
	m_sourceName = sourceName;
	return TRUE;
} // setSourceName


//-----------------------------------------------------------------------------
// getSourceName
//
// - get source name
//
// returns:
//		source name string
//
SOCmnString SOAeSEventAttrSimple::getSourceName(void)
{
	return m_sourceName;
} // getSourceName


//-----------------------------------------------------------------------------
// setOccurrenceTime
//
// - set occurrence time
//
// returns:
//		TRUE  - time set
//      FALSE - bad argument
//
BOOL SOAeSEventAttrSimple::setOccurrenceTime(
	IN LPFILETIME time) // occurrence time
{
	if (time)
	{
		m_occurenceTime.dwHighDateTime = time->dwHighDateTime;
		m_occurenceTime.dwLowDateTime = time->dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // setOccurrenceTime


//-----------------------------------------------------------------------------
// getOccurrenceTime
//
// - get occurrence time
//
// returns:
//		TRUE  - time returned
//      FALSE - bad argument
//
BOOL SOAeSEventAttrSimple::getOccurrenceTime(
	OUT LPFILETIME time)    // occurrence time
{
	if (time)
	{
		time->dwHighDateTime = m_occurenceTime.dwHighDateTime;
		time->dwLowDateTime = m_occurenceTime.dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // getOccurrenceTime


//-----------------------------------------------------------------------------
// setMessage
//
// - set message
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrSimple::setMessage(
	IN LPCTSTR message) // message
{
	m_message = message;
	return TRUE;
} // setMessage


//-----------------------------------------------------------------------------
// getMessage
//
// - get message
//
// returns:
//		message
//
SOCmnString SOAeSEventAttrSimple::getMessage(void)
{
	return m_message;
} // getMessage


//-----------------------------------------------------------------------------
// setCategory
//
// - set category
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrSimple::setCategory(
	IN SOAeSCategory* category) // category
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	m_category = category;
	m_category.addRef();
	return TRUE;
} // setCategory


//-----------------------------------------------------------------------------
// setCategoryByID
//
// - set category by the id of the category
//
// returns:
//		FALSE
//
BOOL SOAeSEventAttrSimple::setCategoryByID(
	IN DWORD categoryID)    // category id
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> esRoot = entry->getEventSpaceRoot();
	SOAeSCategory* category = esRoot->findCategory(categoryID);
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	m_category = category;
	return m_category.isNotNull();
} // setCategoryByID


//-----------------------------------------------------------------------------
// getCategory
//
// - get category
//
// returns:
//      category object
//		NULL - no category object available
//
SOAeSCategory* SOAeSEventAttrSimple::getCategory(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	m_category.addRef();
	return (SOAeSCategory*)m_category;
} // getCategory


//-----------------------------------------------------------------------------
// setSeverity
//
// - set severity
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrSimple::setSeverity(
	IN DWORD severity)  // severity
{
	m_severity = severity;
	return TRUE;
} // setSeverity


//-----------------------------------------------------------------------------
// getSeverity
//
// - get severity
//
// returns:
//		severity
//
DWORD SOAeSEventAttrSimple::getSeverity(void)
{
	return m_severity;
}


//-----------------------------------------------------------------------------
// getAttributeValueList
//
// - get attribute values
//
// returns:
//		list
//
SOCmnList<SOAeSAttributeValue> *SOAeSEventAttrSimple::getAttributeValueList(void)
{
	return &m_attrValList;
} //  getAttributeValueList




//-----------------------------------------------------------------------------
// SOAeSEventAttrTracking                                                     |
//-----------------------------------------------------------------------------

SOAeSEventAttrTracking::SOAeSEventAttrTracking(void)
{
}

SOAeSEventAttrTracking::~SOAeSEventAttrTracking(void)
{
}

//-----------------------------------------------------------------------------
// setActorID
//
// - set actor ID
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrTracking::setActorID(
	IN LPCTSTR actorID) // actor ID
{
	m_actorID = actorID;
	return TRUE;
} // setActorID


//-----------------------------------------------------------------------------
// getActorID
//
// - get actor ID
//
// returns:
//		actor ID
//
SOCmnString SOAeSEventAttrTracking::getActorID(void)
{
	return m_actorID;
} // getActorID




//-----------------------------------------------------------------------------
// SOAeSEventAttrCondition                                                    |
//-----------------------------------------------------------------------------

SOAeSEventAttrCondition::SOAeSEventAttrCondition(void)
{
	m_activeTime.dwHighDateTime = 0;
	m_activeTime.dwLowDateTime = 0;
	m_cookie = 0;
	m_changeMask = 0;
	m_newState = 0;
	m_quality = OPC_QUALITY_GOOD;
	m_ackRequired = FALSE;
}

SOAeSEventAttrCondition::~SOAeSEventAttrCondition(void)
{
}

//-----------------------------------------------------------------------------
// setConditionName
//
// - set condition name
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setConditionName(
	IN LPCTSTR conditionName)   // condition name
{
	m_conditionName = conditionName;
	return TRUE;
} // setConditionName


//-----------------------------------------------------------------------------
// getConditionName
//
// - get condition name
//
// returns:
//		condition name
//
SOCmnString SOAeSEventAttrCondition::getConditionName(void)
{
	return m_conditionName;
} // getConditionName


//-----------------------------------------------------------------------------
// setSubConditionName
//
// - set subcondition name
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setSubConditionName(
	IN LPCTSTR subConditionName)    // subcondition name
{
	m_subConditionName = subConditionName;
	return TRUE;
} // setSubConditionName


//-----------------------------------------------------------------------------
// getSubConditionName
//
// - get subcondition name
//
// returns:
//		subcondition name
//
SOCmnString SOAeSEventAttrCondition::getSubConditionName(void)
{
	return m_subConditionName;
} // getSubConditionName


//-----------------------------------------------------------------------------
// setChangeMask
//
// - set change mask
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setChangeMask(
	IN WORD changeMask) // change mask
{
	m_changeMask = changeMask;
	return TRUE;
} // setChangeMask


//-----------------------------------------------------------------------------
// getChangeMask
//
// - get change mask
//
// returns:
//		change mask
//
WORD SOAeSEventAttrCondition::getChangeMask(void)
{
	return m_changeMask;
} // getChangeMask


//-----------------------------------------------------------------------------
// setNewState
//
// - set new state
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setNewState(
	IN WORD newState)   // new state
{
	m_newState = newState;
	return TRUE;
} // setNewState


//-----------------------------------------------------------------------------
// getNewState
//
// - get new state
//
// returns:
//		new state
//
WORD SOAeSEventAttrCondition::getNewState(void)
{
	return m_newState;
} // getNewState


//-----------------------------------------------------------------------------
// setQuality
//
// - set quality
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setQuality(
	IN WORD quality)    // quality
{
	m_quality = quality;
	return TRUE;
} // setQuality


//-----------------------------------------------------------------------------
// getQuality
//
// - get quality
//
// returns:
//		quality
//
WORD SOAeSEventAttrCondition::getQuality(void)
{
	return m_quality;
} // getQuality


//-----------------------------------------------------------------------------
// setCookie
//
// - set cookie
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setCookie(
	IN DWORD cookie)    // cookie
{
	m_cookie = cookie;
	return TRUE;
} // setCookie


//-----------------------------------------------------------------------------
// getCookie
//
// - get cookie
//
// returns:
//		cookie
//
DWORD SOAeSEventAttrCondition::getCookie(void)
{
	return m_cookie;
} // getCookie


//-----------------------------------------------------------------------------
// setAckRequired
//
// - set ack required
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setAckRequired(
	IN BOOL ackRequired)    // ack required
{
	m_ackRequired = (BYTE)ackRequired;
	return TRUE;
} // setAckRequired


//-----------------------------------------------------------------------------
// getAckRequired
//
// - get ack required
//
// returns:
//		ack required
//
BOOL SOAeSEventAttrCondition::getAckRequired(void)
{
	return m_ackRequired;
} // getAckRequired


//-----------------------------------------------------------------------------
// setActiveTime
//
// - set active time
//
// returns:
//		TRUE
//
BOOL SOAeSEventAttrCondition::setActiveTime(
	IN LPFILETIME activeTime)   // active time
{
	m_activeTime.dwHighDateTime = activeTime->dwHighDateTime;
	m_activeTime.dwLowDateTime = activeTime->dwLowDateTime;
	return TRUE;
} // setActiveTime


//-----------------------------------------------------------------------------
// getActiveTime
//
// - get active time
//
// returns:
//		TRUE  - return time
//		FALSE - bad parameter
//
BOOL SOAeSEventAttrCondition::getActiveTime(
	OUT LPFILETIME activeTime)  // active time
{
	if (activeTime)
	{
		activeTime->dwHighDateTime = m_activeTime.dwHighDateTime;
		activeTime->dwLowDateTime = m_activeTime.dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // getActiveTime




//-----------------------------------------------------------------------------
// SOAeSEventSimpleDefault                                                    |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getObjectAeSEventIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSEventSimpleDefault::getObjectAeSEventIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSEventIAttr


//-----------------------------------------------------------------------------
// SOAeSEventTrackingDefault                                                  |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getObjectAeSEventIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSEventTrackingDefault::getObjectAeSEventIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSEventIAttr


//-----------------------------------------------------------------------------
// SOAeSEventConditionDefault                                                 |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getObjectAeSEventIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSEventConditionDefault::getObjectAeSEventIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSEventIAttr

#endif // SOAES
