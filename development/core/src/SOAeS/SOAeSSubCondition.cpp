#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>


//-----------------------------------------------------------------------------
// SOAeSSubConditionClass                                                     |
//-----------------------------------------------------------------------------

SOAeSSubConditionClass::SOAeSSubConditionClass(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS)
{
	m_severity = 1;
	m_ackRequired = FALSE;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS);
#endif
}

SOAeSSubConditionClass::SOAeSSubConditionClass(
	IN LPCTSTR name,        // subcondition name
	IN DWORD severity,      // severity
	IN LPCTSTR description, // description
	IN BOOL ackRequired,    // acknowledge required
	IN LPCTSTR definition)  // definition
	:  SOCmnElement(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS)
{
	setName(name);
	setSeverity(severity);
	setAckRequired(ackRequired);
	setDescription(description);
	setDefinition(definition);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS);
#endif
}

SOAeSSubConditionClass::~SOAeSSubConditionClass(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS);
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
SOCmnObject* SOAeSSubConditionClass::getObjectCmnElementIName(void)
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
BOOL SOAeSSubConditionClass::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setSeverity
//
// - set severity
//
// returns:
//		TRUE
//
BOOL SOAeSSubConditionClass::setSeverity(
	IN DWORD severity)  // severity
{
	onSetSeverity(severity);
	m_severity = severity;
	return TRUE;
} // setSeverity


//-----------------------------------------------------------------------------
// onSetSeverity
//
// - called before severity is set
// - default implementation: do nothing
//
void SOAeSSubConditionClass::onSetSeverity(
	IN DWORD /* severity */)    // severity
{} // onSetSeverity


//-----------------------------------------------------------------------------
// setDescription
//
// - set description
//
// returns:
//		TRUE
//
BOOL SOAeSSubConditionClass::setDescription(
	IN LPCTSTR description) // description
{
	onSetDescription(description);
	m_description = description;
	return TRUE;
} // setDescription


//-----------------------------------------------------------------------------
// onSetDescription
//
// - called before description is set
// - default implementation: do nothing
//
void SOAeSSubConditionClass::onSetDescription(
	IN LPCTSTR /* description */)   // description
{} // onSetDescription


//-----------------------------------------------------------------------------
// setDefinition
//
// - set definition
//
// returns:
//		TRUE
//
BOOL SOAeSSubConditionClass::setDefinition(
	IN LPCTSTR definition)  // definition
{
	onSetDefinition(definition);
	m_definition = definition;
	return TRUE;
} // setDefinition


//-----------------------------------------------------------------------------
// onSetDefinition
//
// - called before definition is set
// - default implementation: do nothing
//
void SOAeSSubConditionClass::onSetDefinition(
	IN LPCTSTR /* definition */)    // definition
{} // onSetDefinition


//-----------------------------------------------------------------------------
// setAckRequired
//
// - set acknowledge required
//
// returns:
//		TRUE
//
BOOL SOAeSSubConditionClass::setAckRequired(
	IN BOOL ackRequired)    // acknowledge required
{
	onSetAckRequired(ackRequired);
	m_ackRequired = (BYTE)ackRequired;
	return TRUE;
} // setAckRequired


//-----------------------------------------------------------------------------
// onSetAckRequired
//
// - called before ack required is set
// - default implementation: do nothing
//
void SOAeSSubConditionClass::onSetAckRequired(
	IN BOOL /* ackRequired */)  // acknowledge required
{} // onSetAckRequired


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSSubConditionClass::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_EVENTSPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_SUBCONDITIONCLASS);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DESCRIPTION_T, getDescription());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEFINITION_T, getDefinition());
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#endif // SOAES
