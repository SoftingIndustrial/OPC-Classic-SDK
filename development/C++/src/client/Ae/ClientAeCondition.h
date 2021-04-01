#ifndef _CLIENTAECONDITION_H_
#define _CLIENTAECONDITION_H_

#include "../ClientEnums.h"
#include "../../ValueQT.h"
#include "../../Enums.h"

#include "ClientAeEvent.h"
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class AeAttribute;
class AeSession;
class AeSubscription;

class TBC_EXPORT AeCondition : public AeEvent
{

	//friend class Application;
	friend class OTCGlobals;

private:

	AeCondition(
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
		unsigned long cookie);

protected:

	EnumConditionChange m_changeMask;
	EnumConditionState m_newState;
	BOOL m_ackRequired ;
	EnumQuality m_quality ;
	tstring m_conditionName ;
	tstring m_subConditionName ;
	DateTime m_activeTime ;
	unsigned long m_cookie ;
	AeSession* m_aeSession ;
	//the subscription to whose condition list the subscription belongs
	AeSubscription* m_aeSubscription;
	//this is necessary for keeping the reference to the subscription in whose condition list the condition exists
	//when deleting the condition, it will be automatically deleted from the list
	AeSubscription* getAeSubscription();
	void setAeSubscription(AeSubscription* aSubscription);

public:

	AeCondition();

	virtual ~AeCondition();

	AeSession* getAeSession();
	void setAeSession(AeSession* anAeSession);

	EnumConditionChange getChangeMask();
	void setChangeMask(EnumConditionChange mask);

	EnumConditionState getState();
	void setState(EnumConditionState state);

	BOOL getAckRequired();
	void setAckRequired(BOOL ackRequired);

	EnumQuality getQuality();
	void setQuality(EnumQuality quality);

	tstring& getConditionName();
	void setConditionName(tstring conditionName);

	tstring& getSubConditionName();
	void setSubConditionName(tstring subConditionName);

	DateTime getActiveTime();
	void setActiveTime(DateTime activeTime);

	unsigned long getCookie();
	void setCookie(unsigned long cookie);

	virtual long acknowledge(
		tstring ackId,
		tstring ackComment,
		ExecutionOptions* someExecutionOptions);

	virtual long updateConditionState(
		std::vector<AeAttribute*> attributes,
		ExecutionOptions* someExecutionOptions);

}; // end class AeCondition

class TBC_EXPORT AeConditionState
{

private:

	unsigned long m_subConditionCount;
	unsigned long m_eventAttrCount;

protected:

	EnumConditionState m_state ;
	tstring  m_actSubConditionName;
	tstring  m_actSubConditionDefinition ;
	unsigned long m_actSubConditionSeverity ;
	tstring m_actSubConditionDescription ;
	DateTime m_conditionActiveTime;
	DateTime m_conditionAckTime ;
	DateTime m_subConditionActiveTime;
	DateTime m_conditionInactiveTime;
	EnumQuality m_quality ;
	tstring m_ackId;
	tstring m_ackComment ;
	std::vector<tstring> m_pSubConditionDefinition;
	std::vector<unsigned long> m_pSubConditionSeverity;
	std::vector<tstring> m_pSubConditionDescription ;
	std::vector<tstring> m_pSubConditionName ;
	std::vector<Variant*> m_pEventAttrs ;  // array with all event attributes
	std::vector<long> m_pAttrErrors ;

public:

	AeConditionState();

	~AeConditionState();

	EnumConditionState getState();
	void setState(EnumConditionState aState);

	tstring& getActiveSubConditionName();
	void setActiveSubConditionName(tstring subConditionName);

	tstring& getActiveSubConditionDefinition();
	void setActiveSubConditionDefinition(tstring subConditionDefinition);

	unsigned long getActiveSubConditionSeverity();
	void setActiveSubConditionSeverity(unsigned long subConditionSeverity);

	tstring& getActiveSubConditionDescription();
	void setActiveSubConditionDescription(tstring subConditionDescription);

	DateTime getConditionActiveTime();
	void setConditionActiveTime(DateTime conditionActiveTime);

	DateTime getConditionAckTime();
	void setConditionAckTime(DateTime ackTime);

	DateTime getSubConditionActiveTime();
	void setSubConditionActiveTime(DateTime activeTime);

	DateTime getConditionInactiveTime();
	void setConditionInactiveTime(DateTime inactiveTime);

	EnumQuality getQuality();
	void setQuality(EnumQuality quality);

	tstring& getAcknowledgerId();
	void setAcknowledgerId(tstring acknowledgerId);

	tstring& getAcknowledgerComment();
	void setAcknowledgerComment(tstring acknowledgerComment);

	std::vector<tstring>& getSubConditionsDefinitions();
	void setSubConditionsDefinitions(std::vector<tstring> subConditionsDefinition);


	std::vector<unsigned long>& getSubConditionsSeverities();
	void setSubConditionsSeverities(std::vector<unsigned long> subConditionsSeverities);


	std::vector<tstring>& getSubConditionsDescriptions();
	void setSubConditionsDescriptions(std::vector<tstring> subConditionsDescription);


	std::vector<tstring>& getSubConditionsNames();
	void setSubConditionsNames(std::vector<tstring> subConditionsName);

	std::vector<Variant*>& getEventAttributes();
	void setEventAttributes(std::vector<Variant*> eventAttributes);

	std::vector<long>& getAttributesErrors();
	void setAttributesErrors(std::vector<long> eventErrors);

}; //end class AeConditionState

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
