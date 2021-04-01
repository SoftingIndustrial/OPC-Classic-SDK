#include "OSCompat.h"
#include "ServerAeCondition.h"
#include "ServerAeCategory.h"
#include "../ServerApplication.h"

using namespace SoftingOPCToolboxServer;


//-----------------------------------------------------------------------------
//	AeCondition
//
AeCondition::AeCondition(void):
	m_handle(0),
	m_stateChange(EnumConditionState_ENABLED),
	m_changeMask(0),
	m_eventCategory(NULL),
	m_severity(0),
	m_quality(EnumQuality_BAD_WAITING_FOR_INITIAL_DATA),
	m_ackRequired(FALSE),
	m_userData(0),
	m_definition(NULL)
{
	m_ackTime.init();
}   //  end Constructor


//-----------------------------------------------------------------------------
//	destructor
//
AeCondition::~AeCondition()
{
	if (m_handle != 0)
	{
		//  This should never happen!
		Application::Instance()->releaseCondition(m_handle);
	}   //  end if
}   //  end destruictor


//-----------------------------------------------------------------------------
//	getEventAttributes
//
std::vector<Variant>& AeCondition::getEventAttributes(void)
{
	return m_eventAttributes;
}   //  end getEventAttributes


//-----------------------------------------------------------------------------
//	setEventAttributes
//
void AeCondition::setEventAttributes(std::vector<Variant>& aValue)
{
	m_eventAttributes.clear();

	for (unsigned long index = 0; index < aValue.size(); index++)
	{
		m_eventAttributes.push_back(aValue[index]);
	}   //  end for
}   //  end setEventAttributes


//-----------------------------------------------------------------------------
// activate
//
long AeCondition::activate(
	tstring& aMessage,
	unsigned long aSeverity,
	tstring& aSubConditionName,
	BOOL anAckRequired,
	DateTime anOccuranceTime)
{
	m_message = aMessage;
	m_severity = aSeverity;
	m_activeSubConditionName = aSubConditionName;
	m_ackRequired = anAckRequired;
	m_occurenceTime = anOccuranceTime;
	m_changeMask = (unsigned short)(
					   EnumConditionChange_ACTIVE_STATE |
					   EnumConditionChange_SEVERITY |
					   EnumConditionChange_ACK_STATE |
					   EnumConditionChange_MESSAGE |
					   EnumConditionChange_SUBCONDITION |
					   EnumConditionChange_ATTRIBUTE);
	m_stateChange = EnumConditionState_ACTIVE;
	return EnumResultCode_S_OK;
}   //  end activate


//-----------------------------------------------------------------------------
// deactivate
//
long AeCondition::deactivate(
	tstring& aMessage,
	unsigned long aSeverity,
	BOOL anAckRequired,
	DateTime anOccuranceTime)
{
	m_message = aMessage;
	m_severity = aSeverity;
	m_ackRequired = anAckRequired;
	m_occurenceTime = anOccuranceTime;
	m_changeMask = (unsigned short)(
					   EnumConditionChange_ACTIVE_STATE |
					   EnumConditionChange_SEVERITY |
					   EnumConditionChange_MESSAGE |
					   EnumConditionChange_ATTRIBUTE);
	m_stateChange = EnumConditionState_NO;
	return EnumResultCode_S_OK;
}   //  end deactivate


//-----------------------------------------------------------------------------
// acknowledge
//
long AeCondition::acknowledge(
	tstring& anAckId,
	tstring& anAckComment,
	DateTime anAckTime)
{
	m_ackId = anAckId;
	m_ackComment = anAckComment;
	m_ackTime = anAckTime;
	m_changeMask = (unsigned short)(
					   EnumConditionChange_ACK_DATA |
					   EnumConditionChange_ACK_STATE);
	m_stateChange = EnumConditionState_ACKED;
	return EnumResultCode_S_OK;
}   //  end acknowledge


//-----------------------------------------------------------------------------
// changed
//
long AeCondition::change()
{
	std::vector<AeCondition*> conditions(1);
	conditions[0] = this;
	return Application::Instance()->changeConditions(conditions);
}   //  end changed


//-----------------------------------------------------------------------------
// enable
//
long AeCondition::enable(tstring& anAddressSpaceElementPath)
{
	return EnumResultCode_E_NOTIMPL;
}   //  end enable


//-----------------------------------------------------------------------------
// disable
//
long AeCondition::disable(tstring& anAddressSpaceElementPath)
{
	return EnumResultCode_E_NOTIMPL;
}   //  end disable


//-----------------------------------------------------------------------------
//	Public constructor
//
AeConditionDefinition::AeConditionDefinition(tstring& aDefinition)
{
	m_definition = aDefinition;
}   //  end Constructor


//-----------------------------------------------------------------------------
//	Public destructor
//
AeConditionDefinition::~AeConditionDefinition()
{
	//  Check for a duplicate condition
	std::map<tstring, AeSubConditionDefinition*>::iterator elementIterator;

	for (elementIterator = m_subconditions.begin();
		 elementIterator != m_subconditions.end();
		 elementIterator++)
	{
		if (elementIterator->second != NULL)
		{
			delete elementIterator->second;
		}   //  end if
	}   //  end if

	m_subconditions.clear();
}   //  end destructor


//-----------------------------------------------------------------------------
//	addSubConditonData
//
long AeConditionDefinition::addSubConditonData(
	tstring& aDefinition,
	tstring& aDescription,
	unsigned long aSeverity)
{
	AeSubConditionDefinition* newSubCondition = new AeSubConditionDefinition(aDefinition, aDescription, aSeverity);

	if (newSubCondition == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	m_subconditions.insert(std::pair<tstring, AeSubConditionDefinition*>(aDefinition, newSubCondition));
	return  EnumResultCode_S_OK;
}   //  end addSubConditonData


//-----------------------------------------------------------------------------
//	addSubConditonData
//
long AeConditionDefinition::addSubConditonData(tstring& aDefinition)
{
	AeSubConditionDefinition* newSubCondition = new AeSubConditionDefinition(aDefinition);

	if (newSubCondition == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	m_subconditions.insert(std::pair<tstring, AeSubConditionDefinition*>(aDefinition, newSubCondition));
	return EnumResultCode_S_OK;
}   //  end addSubConditonData


//-----------------------------------------------------------------------------
// SubConditionDefinition
//
AeSubConditionDefinition* AeConditionDefinition::getSubConditionDefinition(
	tstring& aDefinition)
{
	//  Check for a duplicate condition
	std::map<tstring, AeSubConditionDefinition*>::const_iterator elementIterator;
	elementIterator = m_subconditions.find(aDefinition);

	if (elementIterator == m_subconditions.end())
	{
		return NULL;
	}   //  end if

	return elementIterator->second;
}   //  end getSubConditionDefinition


//-----------------------------------------------------------------------------
// Public constructor
//
AeSubConditionDefinition::AeSubConditionDefinition(tstring& aDefinition):
	m_definition(aDefinition),
	m_description(aDefinition),
	m_severity(0)
{
}   //  end constructor


//-----------------------------------------------------------------------------
// Public constructor
//
AeSubConditionDefinition::AeSubConditionDefinition(
	tstring& aDefinition,
	tstring& aDescription,
	unsigned long aSeverity) :
	m_definition(aDefinition),
	m_description(aDescription),
	m_severity(aSeverity)
{
}   //  end constructor


//-----------------------------------------------------------------------------
// Public destructor
//
AeSubConditionDefinition::~AeSubConditionDefinition(void)
{
}   //  end destructor
