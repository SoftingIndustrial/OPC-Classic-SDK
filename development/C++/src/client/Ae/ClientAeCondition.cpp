#include "OSCompat.h"
#include "ClientAeCondition.h"
#include "ClientAeSession.h"
#include "ClientAeAttribute.h"
#include "ClientAeSubscription.h"
#include "../ClientApplication.h"

using namespace SoftingOPCToolboxClient;

//-----------------------------------------------------
// AeCondition
//-----------------------------------------------------

AeCondition::AeCondition()
{
	m_changeMask        = (EnumConditionChange)0;
	m_newState          = (EnumConditionState)0;
	m_ackRequired       = FALSE;
	m_quality           = EnumQuality_BAD_WAITING_FOR_INITIAL_DATA;
	m_conditionName     = _T("");
	m_subConditionName  = _T("");
	m_cookie            = 0;
	m_aeSession         = NULL;
	m_aeSubscription    = NULL;
} // end constructor

AeCondition::AeCondition(
	EnumEventType anEventType,
	unsigned long eventCategory,
	unsigned long severity,
	tstring sourcePath,
	tstring message,
	DateTime occurenceTime,
	std::vector<Variant*> someEventAttributes,
	tstring actorId,
	EnumConditionChange changeMask,
	EnumConditionState newState,
	BOOL ackRequired,
	EnumQuality quality,
	tstring conditionName,
	tstring subConditionName,
	DateTime activeTime,
	unsigned long cookie)
	: AeEvent(anEventType,
			  eventCategory,
			  severity,
			  sourcePath,
			  message,
			  occurenceTime,
			  someEventAttributes,
			  actorId)
{
	m_changeMask = changeMask;
	m_newState = newState;
	m_ackRequired = ackRequired;
	m_quality = quality;
	m_conditionName = conditionName;
	m_subConditionName = subConditionName;
	m_activeTime = activeTime;
	m_cookie = cookie;
} //end constructor

AeCondition::~AeCondition()
{
} // end destructor

AeSession* AeCondition::getAeSession()
{
	return m_aeSession;
} //end getSession
void AeCondition::setAeSession(AeSession* anAeSession)
{
	m_aeSession = anAeSession;
} //end setSession

AeSubscription* AeCondition::getAeSubscription()
{
	return m_aeSubscription;
} //end getAeSubscription

void AeCondition::setAeSubscription(AeSubscription* aSubscription)
{
	m_aeSubscription = aSubscription;
} //end setAeSubscription

EnumConditionChange AeCondition::getChangeMask()
{
	return m_changeMask;
} //end getChangeMask
void AeCondition::setChangeMask(EnumConditionChange mask)
{
	m_changeMask = mask;
} //end setChangeMask
EnumConditionState AeCondition::getState()
{
	return m_newState;
} //end getState
void AeCondition::setState(EnumConditionState state)
{
	m_newState = state;
} //end setState
BOOL AeCondition::getAckRequired()
{
	return m_ackRequired;
} //end getAckRe
void AeCondition::setAckRequired(BOOL ackRequired)
{
	m_ackRequired = ackRequired;
} //end setAckRequired
EnumQuality AeCondition::getQuality()
{
	return m_quality;
} //end getQuality
void AeCondition::setQuality(EnumQuality quality)
{
	m_quality = quality;
} //end setQuality
tstring& AeCondition::getConditionName()
{
	return m_conditionName;
} //end getConditionName
void AeCondition::setConditionName(tstring conditionName)
{
	m_conditionName = conditionName;
} //end setConditionName
tstring& AeCondition::getSubConditionName()
{
	return m_subConditionName;
} //end getSubConditionName
void AeCondition::setSubConditionName(tstring subConditionName)
{
	m_subConditionName = subConditionName;
} //end subConditionName
DateTime AeCondition::getActiveTime()
{
	return m_activeTime;
} //end getActiveTime
void AeCondition::setActiveTime(DateTime activeTime)
{
	m_activeTime = activeTime;
} //end setActiveTime
unsigned long AeCondition::getCookie()
{
	return m_cookie;
} //end getCookie
void AeCondition::setCookie(unsigned long cookie)
{
	m_cookie = cookie;
} //end setCookie

long AeCondition::acknowledge(
	tstring ackId,
	tstring ackComment,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	std::vector<long> results;
	std::vector<AeCondition*> conditions(1, this);
	AeSession* aSession = getAeSession();

	if (aSession == NULL)
	{
		getApplication()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeCondition::acknowledge"),
			_T("The Session property of the Condition cannot be null! Set the property to a value before calling Acknowledge!"));
		return result;
	} //end if

	result = aSession->acknowledgeAeConditions(
				 ackId,
				 ackComment,
				 conditions,
				 results,
				 someExecutionOptions);
	return result;
} //end acknowledge

long AeCondition::updateConditionState(
	std::vector<AeAttribute*> attributes,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	std::vector<long> results;
	std::vector<AeCondition*> conditions;
	conditions.push_back(this);
	AeConditionState conditionState;
	AeSession* aSession = getAeSession();

	if (aSession == NULL)
	{
		getApplication()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeCondition::updateConditionState"),
			_T("The Session property of the Condition cannot be null! Set the property to a value before calling UpdateConditionState!"));
		return result;
	} //end if

	result = aSession->getAeConditionState(
				 getSourcePath(),
				 getConditionName(),
				 attributes,
				 conditionState,
				 someExecutionOptions);

	if (SUCCEEDED(result))
	{
		setActiveTime(conditionState.getConditionActiveTime());
		setActorId(conditionState.getAcknowledgerId());
		setAttributes(conditionState.getEventAttributes());
	} //end if

	return result;
} //end updateConditionState

//--------------------------------------------------------
//AeConditionState
//--------------------------------------------------------

AeConditionState::AeConditionState()
{
	m_state = EnumConditionState_NO;
	m_actSubConditionName = _T("");
	m_actSubConditionDefinition = _T("");
	m_actSubConditionSeverity  = 0;
	m_actSubConditionDescription = _T("");
	m_quality = EnumQuality_BAD_WAITING_FOR_INITIAL_DATA;
	m_ackId = _T("");
	m_ackComment = _T("");
} //end constructor

AeConditionState::~AeConditionState()
{
} //end destructor


EnumConditionState AeConditionState::getState()
{
	return m_state;
} //end getState
void AeConditionState::setState(EnumConditionState aState)
{
	m_state = aState;
} //end setState

tstring& AeConditionState::getActiveSubConditionName()
{
	return m_actSubConditionName;
} //end
void AeConditionState::setActiveSubConditionName(tstring subConditionName)
{
	m_actSubConditionName = subConditionName;
} //end

tstring& AeConditionState::getActiveSubConditionDefinition()
{
	return m_actSubConditionDefinition;
} //end
void AeConditionState::setActiveSubConditionDefinition(tstring subConditionDefinition)
{
	m_actSubConditionDefinition = subConditionDefinition;
} //end

unsigned long AeConditionState::getActiveSubConditionSeverity()
{
	return m_actSubConditionSeverity;
} //end
void AeConditionState::setActiveSubConditionSeverity(unsigned long subConditionSeverity)
{
	m_actSubConditionSeverity = subConditionSeverity;
} //end

tstring& AeConditionState::getActiveSubConditionDescription()
{
	return m_actSubConditionDescription;
} //end
void AeConditionState::setActiveSubConditionDescription(tstring subConditionDescription)
{
	m_actSubConditionDescription = subConditionDescription;
} //end

DateTime AeConditionState::getConditionActiveTime()
{
	return m_conditionActiveTime;
} //end getConditionActiveTime
void AeConditionState::setConditionActiveTime(DateTime conditionActiveTime)
{
	m_conditionActiveTime = conditionActiveTime;
} //end setConditionActiveTime

DateTime AeConditionState::getConditionAckTime()
{
	return m_conditionAckTime;
} //end getConditionAckTime
void AeConditionState::setConditionAckTime(DateTime ackTime)
{
	m_conditionAckTime = ackTime;
} //end getConditionAckTime

DateTime AeConditionState::getSubConditionActiveTime()
{
	return m_subConditionActiveTime;
} //end getSubConditionActiveTime
void AeConditionState::setSubConditionActiveTime(DateTime activeTime)
{
	m_subConditionActiveTime = activeTime;
} //end setSubConditionActiveTime

DateTime AeConditionState::getConditionInactiveTime()
{
	return m_conditionInactiveTime;
} //end getConditionInactiveTime
void AeConditionState::setConditionInactiveTime(DateTime inactiveTime)
{
	m_conditionInactiveTime = inactiveTime;
} //end setConditionInactiveTime

EnumQuality AeConditionState::getQuality()
{
	return m_quality;
} //end getQuality
void AeConditionState::setQuality(EnumQuality quality)
{
	m_quality = quality;
} //end setQuality

tstring& AeConditionState::getAcknowledgerId()
{
	return m_ackId;
} //end getAcknowledgerId

void AeConditionState::setAcknowledgerId(tstring acknowledgerId)
{
	m_ackId = acknowledgerId;
} //end setAcknowledgerId

tstring& AeConditionState::getAcknowledgerComment()
{
	return m_ackComment;
} //end getAcknowledgerId
void AeConditionState::setAcknowledgerComment(tstring acknowledgerComment)
{
	m_ackComment = acknowledgerComment;
} //end setAcknowledgerId

std::vector<tstring>& AeConditionState::getSubConditionsDefinitions()
{
	return m_pSubConditionDefinition;
} //end getSubConditionsDefinitions

void AeConditionState::setSubConditionsDefinitions(std::vector<tstring> subConditionsDefinition)
{
	m_pSubConditionDefinition.clear();
	unsigned long count = (unsigned long) subConditionsDefinition.size();
	m_pSubConditionDefinition.resize(count);
	m_pSubConditionDefinition = subConditionsDefinition;
} //end setSubConditionsDefinitions


std::vector<unsigned long>& AeConditionState::getSubConditionsSeverities()
{
	return m_pSubConditionSeverity;
} //end getSubConditionsDefinitions

void AeConditionState::setSubConditionsSeverities(std::vector<unsigned long> subConditionsSeverities)
{
	m_pSubConditionSeverity.clear();
	unsigned long count = (unsigned long) subConditionsSeverities.size();
	m_pSubConditionSeverity.resize(count);
	m_pSubConditionSeverity = subConditionsSeverities;
} //end setSubConditionsDefinitions


std::vector<tstring>& AeConditionState::getSubConditionsDescriptions()
{
	return m_pSubConditionDescription;
} //end getSubConditionsDefinitions
void AeConditionState::setSubConditionsDescriptions(std::vector<tstring> subConditionsDescription)
{
	m_pSubConditionDescription.clear();
	unsigned long count = (unsigned long) subConditionsDescription.size();
	m_pSubConditionDescription.resize(count);
	m_pSubConditionDescription = subConditionsDescription;
} //end setSubConditionsDefinitions


std::vector<tstring>& AeConditionState::getSubConditionsNames()
{
	return m_pSubConditionName;
} //end getSubConditionsDefinitions

void AeConditionState::setSubConditionsNames(std::vector<tstring> subConditionsName)
{
	m_pSubConditionName.clear();
	unsigned long count = (unsigned long) subConditionsName.size();
	m_pSubConditionName.resize(count);
	m_pSubConditionName = subConditionsName;
} //end setSubConditionsDefinitions

std::vector<Variant*>& AeConditionState::getEventAttributes()
{
	return m_pEventAttrs;
} //end getEventAttributes
void AeConditionState::setEventAttributes(std::vector<Variant*> eventAttributes)
{
	if (m_pEventAttrs.size() > 0)
	{
		std::vector<Variant*>::iterator attributesIterator;

		for (attributesIterator = m_pEventAttrs.begin(); attributesIterator != m_pEventAttrs.end(); attributesIterator++)
		{
			if (*attributesIterator)
			{
				delete(*attributesIterator);
			} //end if
		} //end for
	} //end if

	m_pEventAttrs.clear();
	unsigned long count = (unsigned long) eventAttributes.size();
	m_pEventAttrs.resize(count);
	m_pEventAttrs = eventAttributes;
} //end setEventAttributes

std::vector<long>& AeConditionState::getAttributesErrors()
{
	return m_pAttrErrors;
} //end getEventAttributes
void AeConditionState::setAttributesErrors(std::vector<long> eventErrors)
{
	m_pAttrErrors.clear();
	unsigned long count = (unsigned long) eventErrors.size();
	m_pAttrErrors.resize(count);
	m_pAttrErrors = eventErrors;
} //end setEventAttributes
