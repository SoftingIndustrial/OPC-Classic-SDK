#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>
#include <opcerror.h>
#include <opcae_er.h>

static TCHAR s_emptyString[2] = _T("");

//-----------------------------------------------------------------------------
// SOAeSConditionClass                                                        |
//-----------------------------------------------------------------------------

SOAeSConditionClass::SOAeSConditionClass(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_CONDITION_CLASS)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION_CLASS);
#endif
}

SOAeSConditionClass::SOAeSConditionClass(
	IN LPCTSTR conditionName)   // condition name
	:  SOCmnElement(SOCMNOBJECT_TYPE_CONDITION_CLASS)
{
	setName(conditionName);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION_CLASS);
#endif
}

SOAeSConditionClass::~SOAeSConditionClass(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION_CLASS);
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
SOCmnObject* SOAeSConditionClass::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a subcondition class
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSConditionClass::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// setName
//
// - set condition name
//
// returns:
//		S_OK                - set name
//		E_INVALIDARG        - no valid name or parameter
//		OPC_E_DUPLICATENAME - no unique name
//
HRESULT SOAeSConditionClass::setName(
	IN LPCTSTR name)    // condition name
{
	if (trustUniqueConditionName())
	{
		return SOCmnElement::setName(name);
	}

	// check if the name of the condition is unique
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSConditionClass> cond;
	// get event category
	cond = eventSpace->findConditionClass(name);

	if (!cond)
	{
		return SOCmnElement::setName(name);
	}

	return OPC_E_DUPLICATENAME;
} // setName


//-----------------------------------------------------------------------------
// getInactiveMessage
//
// - get the message for the inactive state of the condition
//
// returns:
//		message
//
SOCmnString SOAeSConditionClass::getInactiveMessage(void)
{
	return SOCmnString(_T("Inactive"));
} // getInactiveMessage


//-----------------------------------------------------------------------------
// getInactiveAckRequired
//
// - get the ack required for the inactive state of the condition
//
// returns:
//		message
//
BOOL SOAeSConditionClass::getInactiveAckRequired(void)
{
	return FALSE;
} // getInactiveAckRequired


//-----------------------------------------------------------------------------
// getActiveMessage
//
// - get the message for the active single state of the condition
//
// returns:
//		message
//
SOCmnString SOAeSConditionClass::getActiveMessage(void)
{
	return SOCmnString(_T("Active"));
} // getActiveMessage


//-----------------------------------------------------------------------------
// getActiveSeverity
//
// - get the severity for the active single state of the condition
//
// returns:
//		message
//
DWORD SOAeSConditionClass::getActiveSeverity(void)
{
	return 1000;
} // getActiveSeverity


//-----------------------------------------------------------------------------
// getActiveAckRequired
//
// - get the ack required for the active single state of the condition
//
// returns:
//		message
//
BOOL SOAeSConditionClass::getActiveAckRequired(void)
{
	return TRUE;
} // getActiveAckRequired


//-----------------------------------------------------------------------------
// getDefinition
//
// - get the definition for a single state condition
//
// returns:
//		definition
//
SOCmnString SOAeSConditionClass::getDefinition(void)
{
	return SOCmnString(_T(""));
} // getDefinition


//-----------------------------------------------------------------------------
// trustUniqueConditionName
//
// - trust that provided condition names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SOAeSConditionClass::trustUniqueConditionName(void)
{
	return TRUE;
} // trustUniqueConditionName


//-----------------------------------------------------------------------------
// isSingleStateCondition
//
// - check if the condition class is a single state condition
//
// returns:
//		TRUE  - single state
//		FALSE - multiple state
//
BOOL SOAeSConditionClass::isSingleStateCondition(void)
{
	SOCmnList<SOCmnElement> *pLl = getLeafList();

	if (!pLl)
	{
		return TRUE;
	}

	return (pLl->isEmpty());
} // isSingleStateCondition


//-----------------------------------------------------------------------------
// isMultipleStateCondition
//
// - check if the condition class is a multiple state condition
//
// returns:
//		TRUE  - multiple state
//		FALSE - single state
//
BOOL SOAeSConditionClass::isMultipleStateCondition(void)
{
	SOCmnList<SOCmnElement> *pLl = getLeafList();

	if (!pLl)
	{
		return FALSE;
	}

	return (!pLl->isEmpty());
} // isMultipleStateCondition


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSConditionClass::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_EVENTSPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_CONDITIONCLASS);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEFINITION_T, getDefinition());
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SOAeSConditionClassSingleStateDefault                                      |
//-----------------------------------------------------------------------------

SOAeSConditionClassSingleStateDefault::SOAeSConditionClassSingleStateDefault(void)
	:  SOAeSConditionClass()
{
	m_activeSeverity = 1;
}

SOAeSConditionClassSingleStateDefault::SOAeSConditionClassSingleStateDefault(
	IN LPCTSTR conditionName)   // condition name
	:  SOAeSConditionClass()
{
	m_activeSeverity = 1;
	setName(conditionName);
}

//-----------------------------------------------------------------------------
// setActiveMessage
//
// - set the message for the active single state of the condition
//
// returns:
//		TRUE
//
BOOL SOAeSConditionClassSingleStateDefault::setActiveMessage(
	IN LPCTSTR message) // message
{
	m_activeMessage = message;
	return TRUE;
} // setActiveMessage


//-----------------------------------------------------------------------------
// getActiveMessage
//
// - get the message for the active single state of the condition
//
// returns:
//		message
//
SOCmnString SOAeSConditionClassSingleStateDefault::getActiveMessage(void)
{
	return m_activeMessage;
} // getActiveMessage


//-----------------------------------------------------------------------------
// setActiveSeverity
//
// - set the severity for the active single state of the condition
//
// returns:
//		TRUE
//
BOOL SOAeSConditionClassSingleStateDefault::setActiveSeverity(
	IN DWORD severity)  // severity
{
	m_activeSeverity = severity;
	return TRUE;
} // setActiveSeverity


//-----------------------------------------------------------------------------
// getActiveSeverity
//
// - get the severity for the active single state of the condition
//
// returns:
//		message
//
DWORD SOAeSConditionClassSingleStateDefault::getActiveSeverity(void)
{
	return m_activeSeverity;
} // getActiveSeverity


//-----------------------------------------------------------------------------
// setDefinition
//
// - set the definition for a single state condition
//
// returns:
//		TRUE
//
BOOL SOAeSConditionClassSingleStateDefault::setDefinition(
	IN LPCTSTR definition)
{
	m_definition = definition;
	return TRUE;
} // setDefinition


//-----------------------------------------------------------------------------
// getDefinition
//
// - get the definition for a single state condition
//
// returns:
//		definition
//
SOCmnString SOAeSConditionClassSingleStateDefault::getDefinition(void)
{
	return m_definition;
} // getDefinition




//-----------------------------------------------------------------------------
// SOAeSConditionClassMultipleStateDefault                                    |
//-----------------------------------------------------------------------------

SOAeSConditionClassMultipleStateDefault::SOAeSConditionClassMultipleStateDefault(void)
	:  SOAeSConditionClass()
{
}

SOAeSConditionClassMultipleStateDefault::SOAeSConditionClassMultipleStateDefault(
	IN LPCTSTR conditionName)   // condition name
	:  SOAeSConditionClass()
{
	setName(conditionName);
}

//-----------------------------------------------------------------------------
// setDefinition
//
// - set the definition for a single state condition
//
// returns:
//		TRUE
//
BOOL SOAeSConditionClassMultipleStateDefault::setDefinition(
	IN LPCTSTR definition)
{
	m_definition = definition;
	return TRUE;
} // setDefinition


//-----------------------------------------------------------------------------
// getDefinition
//
// - get the definition for a single state condition
//
// returns:
//		definition
//
SOCmnString SOAeSConditionClassMultipleStateDefault::getDefinition(void)
{
	return m_definition;
} // getDefinition

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSConditionClassMultipleStateDefault::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SOAeSCondition                                                             |
//-----------------------------------------------------------------------------

SOAeSCondition::SOAeSCondition(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_CONDITION)
{
	m_objectState = OPC_CONDITION_ENABLED | OPC_CONDITION_ACKED;
	setObjectState(OPC_CONDITION_ENABLED | OPC_CONDITION_ACKED);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION);
#endif
}

SOAeSCondition::SOAeSCondition(
	IN SOAeSConditionClass* conditionClass) // condtion class
	:  SOCmnElement(SOCMNOBJECT_TYPE_CONDITION)
{
	m_objectState = OPC_CONDITION_ENABLED | OPC_CONDITION_ACKED;
	setConditionClass(conditionClass);
	setObjectState(OPC_CONDITION_ENABLED | OPC_CONDITION_ACKED);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION);
#endif
}

SOAeSCondition::~SOAeSCondition(void)
{
	m_objectState = 0;
	setObjectState(OPC_CONDITION_ENABLED | OPC_CONDITION_ACKED);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_CONDITION);
#endif
}


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key with given string
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOAeSCondition::hasKey(
	IN LPCTSTR key) // object key
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (!conditionClass)
	{
		return FALSE;
	}

	SOCmnString myKey;
	myKey = conditionClass->getName();
	return (key == myKey);
} // hasKey


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key
//
// returns:
//		TRUE  - returned object key
//      FALSE - no condition class set
//
BOOL SOAeSCondition::getKey(OUT SOCmnString& key)
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (!conditionClass)
	{
		return FALSE;
	}

	key = conditionClass->getName();
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
SOCmnString SOAeSCondition::getName(void)
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (!conditionClass)
	{
		return SOCmnElement::getName();
	}

	return conditionClass->getName();
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
void SOAeSCondition::getName(
	OUT SOCmnString& name)
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (conditionClass.isNotNull())
	{
		name = conditionClass->getName();
	}
	else
	{
		SOCmnElement::getName(name);
	}
} // getName


//-----------------------------------------------------------------------------
// setObjectState
//
// - set the object state
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::setObjectState(
	IN BYTE state)  // object state
{
#ifdef SOFEATURE_WATCH
	BYTE prevState = getObjectState();
#endif
	BOOL ret = SOCmnObject::setObjectState(state);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->changeConditionStateStatistics(prevState, state);
#endif
	return ret;
} // setObjectState


//-----------------------------------------------------------------------------
// setConditionClass
//
// - set condition class object
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::setConditionClass(
	IN SOAeSConditionClass* conditionClass) // condition class
{
	onSetConditionClass(conditionClass);
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_conditionClass = conditionClass;
	m_conditionClass.addRef();
	return TRUE;
} // setConditionClass


//-----------------------------------------------------------------------------
// onSetConditionClass
//
// - callback before set conition class object
//
void SOAeSCondition::onSetConditionClass(
	IN SOAeSConditionClass* /* conditionClass */)   // condition class
{} // onSetConditionClass


//-----------------------------------------------------------------------------
// setActSubConditionClass
//
// - set active subcondition class object
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::setActSubConditionClass(
	IN SOAeSSubConditionClass* actSubConditionClass)    // active subcondition class
{
	onSetActSubConditionClass(actSubConditionClass);
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_actSubConditionClass = actSubConditionClass;
	m_actSubConditionClass.addRef();
	return TRUE;
} // setActSubConditionClass

//-----------------------------------------------------------------------------
// setLastActSubConditionClass
//
// - set last active subcondition class object
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::setLastActSubConditionClass(
	IN SOAeSSubConditionClass* actSubConditionClass)    // active subcondition class
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_lastActSubConditionClass = actSubConditionClass;
	m_lastActSubConditionClass.addRef();
	return TRUE;
} // setLastActSubConditionClass


//-----------------------------------------------------------------------------
// onSetActSubConditionClass
//
// - callback before set active subconition class object
//
void SOAeSCondition::onSetActSubConditionClass(
	IN SOAeSSubConditionClass* /* actSubConditionClass */)  // active subcondition class
{} // onSetActSubConditionClass


//-----------------------------------------------------------------------------
// getSeverity
//
// - get the severity for the actual state of the condition
//
// returns:
//		message
//
DWORD SOAeSCondition::getSeverity(void)
{
	if (getObjectState() & OPC_CONDITION_ACTIVE)
	{
		SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

		if (conditionClass.isNotNull())
		{
			if (conditionClass->isMultipleStateCondition())
			{
				SOCmnPointer<SOAeSSubConditionClass> actSubConditionClass = getActSubConditionClass();

				if (actSubConditionClass.isNotNull())
				{
					return actSubConditionClass->getSeverity();
				}
			}
			else
			{
				return conditionClass->getActiveSeverity();
			}
		}
	}

	return 1;
} // getSeverity


//-----------------------------------------------------------------------------
// getAckRequired
//
// - get the ack required for the actual state of the condition
//
// returns:
//		message
//
BOOL SOAeSCondition::getAckRequired(
	IN WORD changeMask)
{
	if ((changeMask & (OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_SUBCONDITION)))
	{
		SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

		if (getObjectState() & OPC_CONDITION_ACTIVE)
		{
			if (conditionClass.isNotNull())
			{
				if (conditionClass->isMultipleStateCondition())
				{
					SOCmnPointer<SOAeSSubConditionClass> actSubConditionClass = getActSubConditionClass();

					if (actSubConditionClass.isNotNull())
					{
						return actSubConditionClass->getAckRequired();
					}
				}
				else
				{
					return conditionClass->getActiveAckRequired();
				}
			}
		}
		else
		{
			if (conditionClass.isNotNull())
			{
				if (getObjectState() & OPC_CONDITION_ACKED)
				{
					return conditionClass->getInactiveAckRequired();
				}
				else
				{
					return (!conditionClass->getInactiveAckRequired());
				}
			}
		}
	}

	return FALSE;
} // getAckRequired


//-----------------------------------------------------------------------------
// getSubConditionName
//
// - get the subcondition name for the actual state of the condition
//
// returns:
//		message
//
SOCmnString SOAeSCondition::getSubConditionName(void)
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (getObjectState() & OPC_CONDITION_ACTIVE)
	{
		if (conditionClass.isNotNull())
		{
			if (conditionClass->isMultipleStateCondition())
			{
				SOCmnPointer<SOAeSSubConditionClass> actSubConditionClass = getActSubConditionClass();

				if (actSubConditionClass.isNotNull())
				{
					return actSubConditionClass->getName();
				}
			}
		}
	}
	if (m_lastActSubConditionClass.isNotNull())
	{
		return m_lastActSubConditionClass->getName();
	}
	else
	{
		return _T("");
	}
} // getSubConditionName



//-----------------------------------------------------------------------------
// getMessage
//
// - get the message for the actual state of the condition
//
// returns:
//		message
//
SOCmnString SOAeSCondition::getMessage(void)
{
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();

	if (getObjectState() & OPC_CONDITION_ACTIVE)
	{
		if (conditionClass.isNotNull())
		{
			if (conditionClass->isMultipleStateCondition())
			{
				SOCmnPointer<SOAeSSubConditionClass> actSubConditionClass = getActSubConditionClass();

				if (actSubConditionClass.isNotNull())
				{
					return actSubConditionClass->getDescription();
				}
			}
			else
			{
				return conditionClass->getActiveMessage();
			}
		}
	}
	else
	{
		if (conditionClass.isNotNull())
		{
			return conditionClass->getInactiveMessage();
		}
	}

	return SOCmnString(_T(""));
} // getMessage

//-----------------------------------------------------------------------------
// changeState
//
// - change the state of the condition
//
// returns:
//		TRUE  - event fired
//		FALSE - no event fired
//
BOOL SOAeSCondition::changeState(
	IN WORD newState,               // new state
	OPTIONAL IN LPFILETIME occTime, // occurence time
	OPTIONAL IN WORD addChangeMask) // additional change mask entries
{
	WORD state = getObjectState();
	WORD changeMask = addChangeMask;
	DWORD severity = getSeverity();
	SOCmnString message = getMessage();
	setObjectState((BYTE)newState);

	if ((!(changeMask & OPC_CHANGE_SEVERITY)) && (severity != getSeverity()))
	{
		changeMask |= OPC_CHANGE_SEVERITY;
	}

	if ((!(changeMask & OPC_CHANGE_MESSAGE)) && (message != getMessage()))
	{
		changeMask |= OPC_CHANGE_MESSAGE;
	}

	// enable and disable
	if ((state & OPC_CONDITION_ENABLED) != (newState & OPC_CONDITION_ENABLED))
	{
		// send enable notification
		changeMask |= OPC_CHANGE_ENABLE_STATE;
	}

	// active and inactive
	if ((state & OPC_CONDITION_ACTIVE) != (newState & OPC_CONDITION_ACTIVE))
	{
		// send activation notification
		changeMask |= OPC_CHANGE_ACTIVE_STATE;
	}

	// acknowledge
	if ((state & OPC_CONDITION_ACKED) != (newState & OPC_CONDITION_ACKED))
	{
		// send acknowledge notification
		changeMask |= OPC_CHANGE_ACK_STATE;
	}

	if (changeMask)
	{
		if ((newState & OPC_CONDITION_ENABLED) || (changeMask & OPC_CHANGE_ENABLE_STATE))
		{
			fireEvent(changeMask, occTime);
		}

		return TRUE;
	}

	return FALSE;
} // changeState


//-----------------------------------------------------------------------------
// enable
//
// - enable or disable condition
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::enable(
	IN BOOL enable) // en/disable
{
	WORD newState;
	onEnable(enable);
	newState = getObjectState();

	if (enable)
	{
		newState |= OPC_CONDITION_ENABLED;
	}
	else
	{
		newState &= ~OPC_CONDITION_ENABLED;
	}

	return changeState(newState);
} // enable


//-----------------------------------------------------------------------------
// onEnable
//
// - callback before en/disable condition
//
void SOAeSCondition::onEnable(
	IN BOOL /* enable */)   // en/disable
{} // onEnable


//-----------------------------------------------------------------------------
// activate
//
// - activate single state condition
//
// returns:
//		TRUE  - new condition was activated
//		FALSE - no activation
//
BOOL SOAeSCondition::activate(
	IN BOOL activate,               // activate or deactivate
	OPTIONAL IN LPFILETIME actTime, // activation time
	OPTIONAL IN WORD addChangeMask) // initial change mask
{
	WORD newState = getObjectState();
	FILETIME now;
	WORD changeMask = addChangeMask;

	if (actTime == NULL)
	{
		::GetSystemTimeAsFileTime(&now);
		actTime = &now;
	}

	if (activate != ((newState & OPC_CONDITION_ACTIVE) != 0))
	{
		DWORD severity = getSeverity();
		SOCmnString message = getMessage();
		onActivate(activate);

		// -> state change
		if (activate)
		{
			// condition becomes active
			newState |= OPC_CONDITION_ACTIVE;
			setActiveTime(actTime);
			setActSubConditionTime(actTime);
			setActSubConditionClass(NULL);
		}
		else
		{
			// condition becomes inactive
			newState &= ~OPC_CONDITION_ACTIVE;
			setInactiveTime(actTime);
			setActSubConditionClass(NULL);
		}

		if ((!(changeMask & OPC_CHANGE_SEVERITY)) && (severity != getSeverity()))
		{
			changeMask |= OPC_CHANGE_SEVERITY;
		}

		if ((!(changeMask & OPC_CHANGE_MESSAGE)) && (message != getMessage()))
		{
			changeMask |= OPC_CHANGE_MESSAGE;
		}

		return changeState(newState, actTime, changeMask);
	}
	else
	{
		// no state change
		// check if quality changed
		if (changeMask & (OPC_CHANGE_QUALITY | OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_ACK_STATE | OPC_CHANGE_ENABLE_STATE))
		{
			if (getObjectState() & OPC_CONDITION_ENABLED)
			{
				fireEvent(changeMask, actTime);
			}
		}
	}

	return FALSE;
} // activate


//-----------------------------------------------------------------------------
// activate
//
// - activate subcondition
//
// returns:
//		TRUE  - new subcondition was activated
//		FALSE - no activation (already in the given subcondition)
//
BOOL SOAeSCondition::activate(
	IN SOAeSSubConditionClass* newSubCondition, // new subcondtion object
	OPTIONAL IN LPFILETIME actTime,             // activation time
	OPTIONAL IN WORD addChangeMask)             // initial change mask
{
	FILETIME now;
	WORD changeMask = addChangeMask;

	if (actTime == NULL)
	{
		::GetSystemTimeAsFileTime(&now);
		actTime = &now;
	}

	SOCmnPointer<SOAeSSubConditionClass> actSubConditionClass = getActSubConditionClass();

	if (newSubCondition != actSubConditionClass)
	{
		DWORD severity = getSeverity();
		SOCmnString message = getMessage();
		onActivate(newSubCondition);

		if ((!newSubCondition) || (!actSubConditionClass))
		{
			// one of the subcondition is NULL
			// -> state change
			WORD newState;
			newState = getObjectState();

			if (newSubCondition)
			{
				// condition becomes active
				newState |= OPC_CONDITION_ACTIVE;
				setActiveTime(actTime);
				setActSubConditionClass(newSubCondition);
				setActSubConditionTime(actTime);
			}
			else
			{
				// condition becomes inactive
				newState &= ~OPC_CONDITION_ACTIVE;
				setInactiveTime(actTime);
				setActSubConditionClass(NULL);
			}

			if ((!(changeMask & OPC_CHANGE_SEVERITY)) && (severity != getSeverity()))
			{
				changeMask |= OPC_CHANGE_SEVERITY;
			}

			if ((!(changeMask & OPC_CHANGE_MESSAGE)) && (message != getMessage()))
			{
				changeMask |= OPC_CHANGE_MESSAGE;
			}

			return changeState(newState, actTime, changeMask);
		}
		else
		{
			setActSubConditionClass(newSubCondition);
			setActSubConditionTime(actTime);
			changeMask |= OPC_CHANGE_SUBCONDITION;

			if ((!(changeMask & OPC_CHANGE_SEVERITY)) && (severity != getSeverity()))
			{
				changeMask |= OPC_CHANGE_SEVERITY;
			}

			if ((!(changeMask & OPC_CHANGE_MESSAGE)) && (message != getMessage()))
			{
				changeMask |= OPC_CHANGE_MESSAGE;
			}

			if (getObjectState() & OPC_CONDITION_ENABLED)
			{
				fireEvent(changeMask, actTime);
			}

			return TRUE;
		}
	}
	else
	{
		// no state change
		// check if quality changed
		if (changeMask & (OPC_CHANGE_QUALITY | OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_ACK_STATE | OPC_CHANGE_ENABLE_STATE))
		{
			if (getObjectState() & OPC_CONDITION_ENABLED)
			{
				fireEvent(changeMask, actTime);
			}
		}
	}

	return FALSE;
} // activate

BOOL SOAeSCondition::activate(
	IN LPCTSTR newSubConditionName, // name of new subcondition
	OPTIONAL IN LPFILETIME actTime, // activation time
	OPTIONAL IN WORD addChangeMask) // initial change mask
{
	SOCmnPointer<SOAeSSubConditionClass> subCondClass;

	if (newSubConditionName)
	{
		SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();
		_ASSERTION(conditionClass.isNotNull(), "Before activation of a subcondition the condition class must be set!");
		subCondClass = (SOAeSSubConditionClass*)conditionClass->findLeaf(newSubConditionName);

		if (!subCondClass)
		{
			return FALSE;
		}
	}

	return activate(subCondClass, actTime, addChangeMask);
} // activate


//-----------------------------------------------------------------------------
// onActivate
//
// - callback before the activation of a single state condition
//
void SOAeSCondition::onActivate(
	IN BOOL /* activate */) // activate or deactivate
{} // onActivate


//-----------------------------------------------------------------------------
// onActivate
//
// - callback before the activation of a subcondition
//
void SOAeSCondition::onActivate(
	IN SOAeSSubConditionClass* /* newSubCondition */)   // new subcondition
{} // onActivate


//-----------------------------------------------------------------------------
// acknowledge
//
// - acknowledge condition
//
// returns:
//		S_OK               - acknowledge succeeded
//		OPC_S_ALREADYACKED - condition has already been acked
//		E_INVALIDARG       - bad parameter
//		OPC_E_INVALIDTIME  - time does not match latest activation time
//
HRESULT SOAeSCondition::acknowledge(
	IN LPCTSTR ackID,           // acknowledger ID
	IN LPCTSTR ackComment,      // acknowledger comment
	LPFILETIME activeTime,      // active time
	DWORD cookie)               // notification cookie
{
	// check cookie
	if (cookie != (DWORD)this->getOpcHandle())
	{
		return E_INVALIDARG;
	}

	// check active time
	FILETIME subActTime;
	getActSubConditionTime(&subActTime);

	if ((activeTime->dwHighDateTime != subActTime.dwHighDateTime) ||
		(activeTime->dwLowDateTime != subActTime.dwLowDateTime))
	{
		return OPC_E_INVALIDTIME;
	}

	// check state of condition
	if (getObjectState() & OPC_CONDITION_ACKED)
	{
		return OPC_S_ALREADYACKED;
	}

	// it's an valid acknowledge -> handle it
	return handleAcknowledge(ackID, ackComment, activeTime);
} // acknowledge


//-----------------------------------------------------------------------------
// onAcknowledge
//
// - callback if an valid acknowledge was received
//
void SOAeSCondition::onAcknowledge(
	IN LPCTSTR /* ackID */,          // acknowledger ID
	IN LPCTSTR /* ackComment */,     // acknowledger comment
	IN LPFILETIME /* activeTime */,  // active time
	IN LPFILETIME /* ackTime */)     // acknowledge time
{} // onAcknowledge


//-----------------------------------------------------------------------------
// handleAcknowledge
//
// - handle the valid acknowledge from the client
//
// returns:
//		S_OK  - accept the acknowledge
//		any other HRESULT error to deny the acknowledge
//
HRESULT SOAeSCondition::handleAcknowledge(
	IN LPCTSTR ackID,
	IN LPCTSTR ackComment,
	IN LPFILETIME activeTime)
{
	FILETIME ackTime;
	::GetSystemTimeAsFileTime(&ackTime);
	onAcknowledge(ackID, ackComment, activeTime, &ackTime);
	// set the acknowledge values and change the state
	setAckTime(&ackTime);
	setAckID(ackID);
	setAckComment(ackComment);
	changeState((WORD)(getObjectState() | OPC_CONDITION_ACKED));
	return S_OK;
} // handleAcknowledge


//-----------------------------------------------------------------------------
// changeQuality
//
// - change the quality of the condition
//
// returns:
//		TRUE  - quality changed and notification send to the clients
//		FALSE - no quality change
//
BOOL SOAeSCondition::changeQuality(
	IN WORD quality)
{
	BOOL ret = FALSE;
	WORD actQuality = getQuality();

	if (quality != actQuality)
	{
		DWORD severity = getSeverity();
		SOCmnString message = getMessage();
		WORD changeMask = OPC_CHANGE_QUALITY;
		setQuality(quality);

		if ((!(changeMask & OPC_CHANGE_SEVERITY)) && (severity != getSeverity()))
		{
			changeMask |= OPC_CHANGE_SEVERITY;
		}

		if ((!(changeMask & OPC_CHANGE_MESSAGE)) && (message != getMessage()))
		{
			changeMask |= OPC_CHANGE_MESSAGE;
		}

		if (getObjectState() & OPC_CONDITION_ENABLED)
		{
			ret = fireEvent(changeMask);
		}
	}

	return ret;
} // changeQuality


//-----------------------------------------------------------------------------
// fireEvent
//
// - create an event notification and send it to all clients
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::fireEvent(
	IN WORD changeMask,             // event change mask
	OPTIONAL IN LPFILETIME occTime) // occurence time
{
	SOCmnPointer<SOAeSEvent> event;
	FILETIME now;
	BOOL ret;

	if (occTime == NULL)
	{
		::GetSystemTimeAsFileTime(&now);
		occTime = &now;
	}

	// create event object
	event = createEvent(changeMask, occTime);
	// send event to the clients
	ret = event->fire(getServerInstance());
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		WORD changeMask = event->getChangeMask();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		FILETIME ft;

		if (changeMask & (OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_ACK_STATE | OPC_CHANGE_ENABLE_STATE))
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			SOCmnStringEx szRawAttr, szXML;
			szRawAttr.format(_T("%s = \"%lu\""), SOWATCH_ATTR_RAW_T, (DWORD)getObjectState());
			SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_STATE_T, conv->conditionState2string(buffer, getObjectState()), szRawAttr);
			entry->watchAttributeChange(this, SOWATCH_ATTRIBUTE_STATE_T, szXML);

			if (changeMask & OPC_CHANGE_ACTIVE_STATE)
				if (event->getNewState() & OPC_CONDITION_ACTIVE)
				{
					if (getActiveTime(&ft))
					{
						SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACTIVETIME_T, conv->filetime2string(buffer, &ft));
					}
				}
				else
				{
					if (getInactiveTime(&ft))
					{
						SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_INACTIVETIME_T, conv->filetime2string(buffer, &ft));
					}
				}

			if ((changeMask & OPC_CHANGE_ACK_STATE) && (event->getNewState() & OPC_CONDITION_ACKED))
			{
				SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACKID_T, getAckID());
				SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACKCOMMENT_T, getAckComment());

				if (getAckTime(&ft))
				{
					SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACKTIME_T, conv->filetime2string(buffer, &ft));
				}
			}
		}

		if (changeMask & OPC_CHANGE_SUBCONDITION)
		{
			SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_SUBCONDITION_T, event->getSubConditionName());

			if (getActSubConditionTime(&ft))
			{
				SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACTSUBCONDITIONTIME_T, conv->filetime2string(buffer, &ft));
			}
		}

		if (changeMask & OPC_CHANGE_MESSAGE)
		{
			SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_MESSAGE_T, event->getMessage());
		}

		if (changeMask & OPC_CHANGE_SEVERITY)
		{
			SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_SEVERITY_T, buffer.format(_T("%lu"), event->getSeverity()));
		}

		if (changeMask & OPC_CHANGE_QUALITY)
		{
			SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_QUALITY_T, conv->quality2string(buffer, event->getQuality()));
		}
	}

#endif // SOFEATURE_WATCH
	return ret;
} // fireEvent


//-----------------------------------------------------------------------------
// createEvent
//
// - create and initalize a event
//
// returns:
//		pointer to the event object
//
SOAeSEvent* SOAeSCondition::createEvent(
	IN WORD changeMask,             // event change mask
	OPTIONAL IN LPFILETIME occTime) // occurence time
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOCmnPointer<SOAeSEvent> event = creator->createConditionEvent(this);
	SOCmnPointer<SOAeSCategory> cat;
	FILETIME latestSavedTime;
	FILETIME activeTime;
	BOOL ackReq = FALSE;

	if (occTime == NULL)
	{
		// calculate the time of the saved times - for refresh
		FILETIME tmpTime;
		latestSavedTime.dwHighDateTime = 0;
		latestSavedTime.dwLowDateTime = 0;

		if (getActiveTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
				(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
			{
				latestSavedTime = tmpTime;
			}

		if (getInactiveTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
				(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
			{
				latestSavedTime = tmpTime;
			}

		if (getAckTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
				(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
			{
				latestSavedTime = tmpTime;
			}

		if (getActSubConditionTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
				(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
			{
				latestSavedTime = tmpTime;
			}

		occTime = &latestSavedTime;
	}

	event->setOccurrenceTime(occTime);
	event->setCookie(getCookie());
	event->setSourceName(getSourceName());
	// ASSERT: before firing an event the condition class must be set!
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();
	_ASSERTION((conditionClass.isNotNull()), "Before firing an event the condition class must be set!");
	cat = (SOAeSCategory*)conditionClass->getParentBranch();
	event->setCategory((SOAeSCategory*)cat);
	event->setConditionName(conditionClass->getName());
	// subcondition attributes
	ackReq = getAckRequired(changeMask);

	if (ackReq)
	{
		WORD state = getObjectState();

		if ((state & OPC_CONDITION_ACKED) != 0)
		{
			state &= ~OPC_CONDITION_ACKED;
			setObjectState((BYTE)state);
			changeMask |= OPC_CHANGE_ACK_STATE;
		}
	}

	event->setAckRequired(ackReq);
	event->setSubConditionName(getSubConditionName());
	event->setMessage(getMessage());
	event->setSeverity(getSeverity());
	getActSubConditionTime(&activeTime);
	event->setActiveTime(&activeTime);
	event->setChangeMask(changeMask);
	event->setNewState(getObjectState());
	event->setQuality(getQuality());

	if (getObjectState() & OPC_CONDITION_ACKED)
	{
		event->setActorID(getAckID());
	}

	copyAttributeValueList(event->getAttributeValueList());
	event.addRef();
	return (SOAeSEvent*)event;
} // createEvent


//-----------------------------------------------------------------------------
// updateAttributeValue
//
// - update a attribute value of the condition
//
// returns:
//		TRUE
//
BOOL SOAeSCondition::updateAttributeValue(
	IN DWORD attributeID,   // attribute id
	IN LPVARIANT value)     // new value
{
	SOCmnList<SOAeSAttributeValue> attrValList(getAttributeValueList());
	SOCmnPointer<SOAeSAttributeValue> attrVal;
	attrVal = attrValList.findKey(attributeID);

	if (!attrVal)
	{
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
		SOCmnPointer<SOAeSCategory> cat = getCategory();
		// ASSERT: before updating attributes to the event the category must be set!
		_ASSERTION((SOAeSCategory*)cat, "Before updating attribute values to the event the category must be set!");

		if (!cat)
		{
			return FALSE;
		}

		SOCmnPointer<SOAeSAttribute> attr = cat->getAttribute(attributeID);
		attrVal = creator->createAttributeValue(this);
		// initialize attribute value
		attrVal->setAttribute(attr);
		attrVal->setValue(value);
		// add attribute value
		attrValList.add(attrVal);
	}
	else
	{
		attrVal->setValue(value);
	}

	return TRUE;
} // updateAttributeValue


//-----------------------------------------------------------------------------
// copyAttributeValueList
//
// - copy the attribute values of the condition
//
void SOAeSCondition::copyAttributeValueList(
	OUT SOCmnList<SOAeSAttributeValue> *attrValueList)  // attribute value list
{
	SOCmnList<SOAeSAttributeValue> avlS(getAttributeValueList());
	SOCmnList<SOAeSAttributeValue> avlD(attrValueList);
	SOCmnListPosition pos;
	SOAeSAttributeValue* avS;
	SOCmnPointer<SOAeSAttributeValue> avD;
	SOCmnPointer<SOAeSAttribute> attr;
	SOCmnVariant varS;
	pos = avlS.getStartPosition();

	while (pos)
	{
		avS = avlS.getNext(pos);
		SOCMN_ALLOCATE_OBJECT_C(avD, SOAeSAttributeValue(), (SOAeSAttributeValue*))
		avD->setAttribute((SOAeSAttribute*)(attr = avS->getAttribute()));
		varS.clear();
		avS->getValue(&varS);
		avD->setValue(&varS);
		avlD.add(avD);
	}
} // copyAttributeValueList


//-----------------------------------------------------------------------------
// getCategory
//
// - get the category object of the condition
//
SOAeSCategory* SOAeSCondition::getCategory(void)
{
	// ASSERT: Before requesting the category the condition class must be set!
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();
	_ASSERTION(conditionClass.isNotNull(), "Before requesting the category the condition class must be set!");
	return (SOAeSCategory*)conditionClass->getParentBranch();
} // getCategory


//-----------------------------------------------------------------------------
// getCookie
//
// - get the event cookie for the conditional events
//
DWORD SOAeSCondition::getCookie(void)
{
	return (DWORD)this->getOpcHandle();
} // getCookie


//-----------------------------------------------------------------------------
// getSourceName
//
// - get the name of the condition source
//
SOCmnString SOAeSCondition::getSourceName(void)
{
	SOCmnPointer<SOAeSSource> src = (SOAeSSource*)getParentBranch();
	// ASSERT: before firing an event the condition must be in the area space!
	_ASSERTION(((SOAeSSource*)src), "Before firing an event the condition must be in the area space!");
	return src->getFullName();
} // getSourceName


//-----------------------------------------------------------------------------
// translateToItemIDs
//
// - get the OPC DA item IDs for the attributes of a (sub)condition
//
// returns:
//		S_OK         - no DA item ID available
//    E_INVALIDARG - invalid subcondition
//
HRESULT SOAeSCondition::translateToItemIDs(
	IN LPCTSTR subConditionName,        // subcondition name
	OUT SOCmnList<SOAeSAttributeValueDADescription>* /* descrDAList */) // DA description list
{
	SOCmnPointer<SOAeSConditionClass> condClass = getConditionClass();
	SOCmnList<SOAeSSubConditionClass> *pSubCondList = (SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList();

	if (pSubCondList != NULL)
	{
		// multiple state condition
		SOCmnList<SOAeSSubConditionClass> subCondList = pSubCondList;
		SOAeSSubConditionClass* subCondClass;
		SOCmnListPosition pos;
		pos = subCondList.getStartPosition();

		while (pos)
		{
			subCondClass = subCondList.getNext(pos);

			if (subCondClass->getName() == subConditionName)
			{
				return S_OK;
			}
		}
	}
	else
	{
		// single state condition
		if ((subConditionName == NULL) || (_tcslen(subConditionName) == 0))
		{
			return S_OK;
		}

		if (subConditionName == getName())
		{
			return S_OK;
		}
	}

	return E_INVALIDARG;
} // translateToItemIDs


//-----------------------------------------------------------------------------
// checkCondition
//
// - check the condition state
//
void SOAeSCondition::checkCondition(void)
{
} // checkConditions


//-----------------------------------------------------------------------------
// getServerInstance
//
// - get the server instance of the condition
//
// returns:
//		server instance (between 0 and 9)
//
BYTE SOAeSCondition::getServerInstance(void)
{
	return 0;
} // getServerInstance


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the object
//
SOCmnString SOAeSCondition::getDeviceState(void)
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
SOCmnString SOAeSCondition::getDeviceStateInfo(void)
{
	return SOCmnString(_T(""));
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSCondition::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_AREASPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_CONDITION);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();
		SOCmnString buffer;
		FILETIME ft;
		SOCmnStringEx szStateAtts;

		if (conditionClass.isNotNull())
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, conditionClass->getName());
		}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
		szStateAtts.format(_T("%s = \"%u\""), SOWATCH_ATTR_RAW_T, getObjectState());
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_STATE_T, conv->conditionState2string(buffer, getObjectState()), szStateAtts);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SUBCONDITION_T, getSubConditionName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_MESSAGE_T, getMessage());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SEVERITY_T, buffer.format(_T("%lu"), getSeverity()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_QUALITY_T, conv->quality2string(buffer, getQuality()));

		if (getActiveTime(&ft))
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACTIVETIME_T, conv->filetime2string(buffer, &ft));
		}

		if (getInactiveTime(&ft))
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_INACTIVETIME_T, conv->filetime2string(buffer, &ft));
		}

		if (getActSubConditionTime(&ft))
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACTSUBCONDITIONTIME_T, conv->filetime2string(buffer, &ft));
		}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACKID_T, getAckID());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACKCOMMENT_T, getAckComment());

		if (getAckTime(&ft))
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACKTIME_T, conv->filetime2string(buffer, &ft));
		}

		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SOAeSConditionIAttr                                                        |
//-----------------------------------------------------------------------------

SOAeSConditionIAttr::SOAeSConditionIAttr(void)
{}

SOAeSConditionIAttr::~SOAeSConditionIAttr(void)
{}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOAeSConditionIAttr::getIntTraceId(void)
{
	SOCmnObject* obj = getObjectAeSConditionIAttr();

	if (obj != NULL)
	{
		return obj->getObjTraceId();
	}

	return s_emptyString;
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectAeSConditionIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSConditionIAttr::getObjectAeSConditionIAttr(void)
{
	return NULL;
} // getObjectAeSConditionIAttr


//-----------------------------------------------------------------------------
// setActiveTime
//
// - set active time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setActiveTime(
	IN LPFILETIME /* activeTime */) // active time
{
	return FALSE;    // setActiveTime
}


//-----------------------------------------------------------------------------
// getActiveTime
//
// - get active time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::getActiveTime(
	OUT LPFILETIME /* activeTime */)    // active time
{
	return FALSE;    // getActiveTime
}


//-----------------------------------------------------------------------------
// setInactiveTime
//
// - set inactive time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setInactiveTime(
	IN LPFILETIME /* inactiveTime */)   // inactive time
{
	return FALSE;    // setInactiveTime
}


//-----------------------------------------------------------------------------
// getInactiveTime
//
// - get inactive time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::getInactiveTime(
	OUT LPFILETIME /* inactiveTime */)  // inactive time
{
	return FALSE;    // getInactiveTime
}


//-----------------------------------------------------------------------------
// setQuality
//
// - set quality
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setQuality(
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
//		OPC_QUALITY_BAD
//
WORD SOAeSConditionIAttr::getQuality(void)
{
	return OPC_QUALITY_BAD;    // getQuality
}


//-----------------------------------------------------------------------------
// setAckTime
//
// - set acknowledge time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setAckTime(
	IN LPFILETIME /* ackTime */)    // acknowledge time
{
	return FALSE;    // setAckTime
}


//-----------------------------------------------------------------------------
// getAckTime
//
// - get acknowledge time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::getAckTime(
	OUT LPFILETIME /* ackTime */)   // acknowledge time
{
	return FALSE;    // getAckTime
}


//-----------------------------------------------------------------------------
// setAckID
//
// - set acknowledger ID
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setAckID(
	IN LPCTSTR /* ackID */) // acknowledger ID
{
	return FALSE;    // setAckID
}


//-----------------------------------------------------------------------------
// getAckID
//
// - get acknowledger ID
//
// returns:
//		empty string
//
SOCmnString SOAeSConditionIAttr::getAckID(void)
{
	return SOCmnString(_T(""));    // getAckID
}


//-----------------------------------------------------------------------------
// setAckComment
//
// - set acknowledger comment
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setAckComment(
	IN LPCTSTR /* ackComment */)    // acknowledger comment
{
	return FALSE;    // setAckComment
}


//-----------------------------------------------------------------------------
// getAckComment
//
// - get acknowledger comment
//
// returns:
//		empty string
//
SOCmnString SOAeSConditionIAttr::getAckComment(void)
{
	return SOCmnString(_T(""));    // getAckComment
}


//-----------------------------------------------------------------------------
// setActSubConditionTime
//
// - set active subcondition time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::setActSubConditionTime(
	IN LPFILETIME /* actSubConditionTime */)    // active subcondition time
{
	return FALSE;    // setActSubConditionTime
}


//-----------------------------------------------------------------------------
// getActSubConditionTime
//
// - get active subcondition time
//
// returns:
//		FALSE
//
BOOL SOAeSConditionIAttr::getActSubConditionTime(
	OUT LPFILETIME /* actSubConditionTime */)   // active subcondition time
{
	return FALSE;    // getActSubConditionTime
}


//-----------------------------------------------------------------------------
// onSetActiveTime
//
// - callback before set active time
//
void SOAeSConditionIAttr::onSetActiveTime(
	IN LPFILETIME /* activeTime */) // active time
{} // onSetActiveTime


//-----------------------------------------------------------------------------
// onSetInactiveTime
//
// - callback before set inactive time
//
void SOAeSConditionIAttr::onSetInactiveTime(
	IN LPFILETIME /* inactiveTime */)   // inactive time
{} // onSetInactiveTime


//-----------------------------------------------------------------------------
// onSetActSubCondtionTime
//
// - callback before set active subcondition time time
//
void SOAeSConditionIAttr::onSetActSubCondtionTime(
	IN LPFILETIME /* actSubConditionTime */)    // active subcondition time
{} // onSetActSubCondtionTime


//-----------------------------------------------------------------------------
// onSetAckTime
//
// - callback before set ack time
//
void SOAeSConditionIAttr::onSetAckTime(
	IN LPFILETIME /* ackTime */)    // ack time
{} // onSetAckTime


//-----------------------------------------------------------------------------
// onSetQuality
//
// - callback before set quality
//
void SOAeSConditionIAttr::onSetQuality(
	IN WORD /* quality */)  // quality
{} // onSetQuality


//-----------------------------------------------------------------------------
// onSetAckID
//
// - callback before set acknowledger id
//
void SOAeSConditionIAttr::onSetAckID(
	IN LPCTSTR /* ackID */) // acknowledger id
{} // onSetAckID


//-----------------------------------------------------------------------------
// onSetAckComment
//
// - callback before set acknowledger comment
//
void SOAeSConditionIAttr::onSetAckComment(
	IN LPCTSTR /* ackComment */)    // acknowledger comment
{} // onSetAckComment




//-----------------------------------------------------------------------------
// SOAeSConditionAttrFull                                                     |
//-----------------------------------------------------------------------------

SOAeSConditionAttrFull::SOAeSConditionAttrFull(void)
{
	m_activeTime.dwHighDateTime = 0;
	m_activeTime.dwLowDateTime = 0;
	m_inactiveTime.dwHighDateTime = 0;
	m_inactiveTime.dwLowDateTime = 0;
	m_ackTime.dwHighDateTime = 0;
	m_ackTime.dwLowDateTime = 0;
	m_actSubConditionTime.dwHighDateTime = 0;
	m_actSubConditionTime.dwLowDateTime = 0;
	m_quality = OPC_QUALITY_GOOD;
}

SOAeSConditionAttrFull::~SOAeSConditionAttrFull(void)
{
}


//-----------------------------------------------------------------------------
// setActiveTime
//
// - set active time
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setActiveTime(
	IN LPFILETIME activeTime)   // active time
{
	onSetActiveTime(activeTime);
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
BOOL SOAeSConditionAttrFull::getActiveTime(
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
// setInactiveTime
//
// - set inactive time
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setInactiveTime(
	IN LPFILETIME inactiveTime) // inactive time
{
	onSetInactiveTime(inactiveTime);
	m_inactiveTime.dwHighDateTime = inactiveTime->dwHighDateTime;
	m_inactiveTime.dwLowDateTime = inactiveTime->dwLowDateTime;
	return TRUE;
} // setInactiveTime


//-----------------------------------------------------------------------------
// getInactiveTime
//
// - get active time
//
// returns:
//		TRUE  - return time
//		FALSE - bad parameter
//
BOOL SOAeSConditionAttrFull::getInactiveTime(
	OUT LPFILETIME inactiveTime)    // inactive time
{
	if (inactiveTime)
	{
		inactiveTime->dwHighDateTime = m_inactiveTime.dwHighDateTime;
		inactiveTime->dwLowDateTime = m_inactiveTime.dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // getInactiveTime


//-----------------------------------------------------------------------------
// setQuality
//
// - set quality
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setQuality(
	IN WORD quality)    // quality
{
	onSetQuality(quality);
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
WORD SOAeSConditionAttrFull::getQuality(void)
{
	return m_quality;
} // getQuality


//-----------------------------------------------------------------------------
// setAckTime
//
// - set acknowledge time
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setAckTime(
	IN LPFILETIME ackTime)  // acknowledge time
{
	onSetAckTime(ackTime);
	m_ackTime.dwHighDateTime = ackTime->dwHighDateTime;
	m_ackTime.dwLowDateTime = ackTime->dwLowDateTime;
	return TRUE;
} // setInactiveTime


//-----------------------------------------------------------------------------
// getAckTime
//
// - get acknowledge time
//
// returns:
//		TRUE  - return time
//		FALSE - bad parameter
//
BOOL SOAeSConditionAttrFull::getAckTime(
	OUT LPFILETIME ackTime) // acknowledge time
{
	if (ackTime)
	{
		ackTime->dwHighDateTime = m_ackTime.dwHighDateTime;
		ackTime->dwLowDateTime = m_ackTime.dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // getInactiveTime


//-----------------------------------------------------------------------------
// setAckID
//
// - set acknowledger ID
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setAckID(
	IN LPCTSTR ackID)   // acknowledger ID
{
	onSetAckID(ackID);
	m_ackID = ackID;
	return TRUE;
} // setAckID


//-----------------------------------------------------------------------------
// getAckID
//
// - get acknowledger ID
//
// returns:
//		acknowledger ID
//
SOCmnString SOAeSConditionAttrFull::getAckID(void)
{
	return m_ackID;
} // getAckID


//-----------------------------------------------------------------------------
// setAckComment
//
// - set acknowledger comment
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setAckComment(
	IN LPCTSTR ackComment)  // acknowledger comment
{
	onSetAckComment(ackComment);
	m_ackComment = ackComment;
	return TRUE;
} // setAckComment


//-----------------------------------------------------------------------------
// getAckComment
//
// - get acknowledger comment
//
// returns:
//		acknowledger comment
//
SOCmnString SOAeSConditionAttrFull::getAckComment(void)
{
	return m_ackComment;
} // getAckComment


//-----------------------------------------------------------------------------
// setActSubConditionTime
//
// - set active subcondition time
//
// returns:
//		TRUE
//
BOOL SOAeSConditionAttrFull::setActSubConditionTime(
	IN LPFILETIME actSubConditionTime)  // active subcondition time
{
	onSetInactiveTime(actSubConditionTime);
	m_actSubConditionTime.dwHighDateTime = actSubConditionTime->dwHighDateTime;
	m_actSubConditionTime.dwLowDateTime = actSubConditionTime->dwLowDateTime;
	return TRUE;
} // setActSubConditionTime


//-----------------------------------------------------------------------------
// getActSubConditionTime
//
// - get active subcondition time
//
// returns:
//		TRUE  - return time
//		FALSE - bad parameter
//
BOOL SOAeSConditionAttrFull::getActSubConditionTime(
	OUT LPFILETIME actSubConditionTime) // active subcondition time
{
	if (actSubConditionTime)
	{
		actSubConditionTime->dwHighDateTime = m_actSubConditionTime.dwHighDateTime;
		actSubConditionTime->dwLowDateTime = m_actSubConditionTime.dwLowDateTime;
		return TRUE;
	}

	return FALSE;
} // getActSubConditionTime




//-----------------------------------------------------------------------------
// SOAeSConditionDefault                                                      |
//-----------------------------------------------------------------------------

SOAeSConditionDefault::SOAeSConditionDefault(void)
	:  SOAeSCondition()
{}

SOAeSConditionDefault::SOAeSConditionDefault(
	IN SOAeSConditionClass* conditionClass) // condition class
	:  SOAeSCondition()
{
	setConditionClass(conditionClass);
}

//-----------------------------------------------------------------------------
// getObjectAeSConditionIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSConditionDefault::getObjectAeSConditionIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSConditionIAttr




//-----------------------------------------------------------------------------
// SOAeSConditionGroup                                                        |
//-----------------------------------------------------------------------------

SOAeSConditionGroup::SOAeSConditionGroup(
	IN DWORD updateRate)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	m_updateRate = updateRate;
	m_condList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_initCnt = 0;
	m_correctNTtimer = 0;
	m_initTime = 0;
	initSpan(0);
}


void SOAeSConditionGroup::connectAttributeValueUpdate(void)
{
	SOCmnList<SOAeSCondition> condList(getConditionList());
	SOAeSCondition* cond;
	SOCmnListPosition posC;
	SOAeSAttributeValue* attrVal;
	SOCmnListPosition posV;

	if (!!condList)
	{
		posC = condList.getStartPosition();

		while (posC)
		{
			SOCmnList<SOAeSAttributeValue> attrValList;
			cond = condList.getNext(posC);
			attrValList = cond->getAttributeValueList();

			if (attrValList.isInit())
			{
				posV = attrValList.getStartPosition();

				while (posV)
				{
					attrVal = attrValList.getNext(posV);
					attrVal->connectValueUpdate(this);
				}
			}
		}
	}
} // checkConditions



//-----------------------------------------------------------------------------
// checkConditions
//
// - check all conditions of the condition list
//
void SOAeSConditionGroup::checkConditions(void)
{
	SOCmnList<SOAeSCondition> condList(getConditionList());
	SOAeSCondition* cond;
	SOCmnListPosition posC;

	if (!!condList)
	{
		posC = condList.getStartPosition();

		while (posC)
		{
			cond = condList.getNext(posC);
			cond->checkCondition();
		}
	}
} // checkConditions


//-----------------------------------------------------------------------------
// initSpan
//
// - initialize time span till next update is needed
//
DWORD SOAeSConditionGroup::initSpan(
	IN DWORD toLate)    // to late called
{
	if (m_updateRate < 200000)
	{
		// don't correct timer if update rate is bigger than 3 minutes
		DWORD initCnt = m_initCnt;
		// correct bad NT Timer (~11 ms interval)
		initCnt += ((m_updateRate * 150) >> 10);

		if (initCnt < 1000)
		{
			m_correctNTtimer = 0;
		}
		else
		{
			DWORD correctNTtimer = m_correctNTtimer;
			correctNTtimer = 10 * (initCnt >> 10);

			if (correctNTtimer < 0xFF)
			{
				m_correctNTtimer = (BYTE)correctNTtimer;
			}
			else
			{
				m_correctNTtimer = 0xFF;
			}

			initCnt = initCnt & 0x3ff;
		}

		m_initCnt = (WORD)initCnt;
		toLate += m_correctNTtimer;
	}

	m_initTime = ::GetTickCount();

	if (toLate < m_updateRate)
	{
		return m_updateRate - toLate;
	}
	else
	{
		return m_updateRate;
	}
} // initSpan


//-----------------------------------------------------------------------------
// updateSpan
//
// - update time span till next request start
//
// returns:
//		TRUE  - time span ellapsed
//		FALSE - time span not ellapsed
//
BOOL SOAeSConditionGroup::updateSpan(
	IN DWORD now,   // current time
	IN DWORD* span) // time till next group update
{
	DWORD newSpan;

	// check for immediate update rate
	if (m_updateRate < 10)
	{
		if (span)
		{
			*span = INFINITE;
		}

		return TRUE;
	}

	DWORD ellapsed = getTimeSpan(m_initTime, now);
	DWORD updateRate = m_updateRate - m_correctNTtimer;
	BOOL fire = FALSE;

	if (ellapsed < updateRate)
	{
		newSpan = updateRate - ellapsed;
	}
	else    // span ellapsed
	{
		DWORD toLate = ellapsed - updateRate;
		newSpan = initSpan(toLate);
		fire = TRUE;
	}

	if (span)
	{
		*span = newSpan;
	}

	return fire;
} // updateSpan

#endif // SOAES
