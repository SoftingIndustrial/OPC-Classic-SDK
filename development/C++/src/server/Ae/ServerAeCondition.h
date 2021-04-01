#ifndef _SERVERAECONDITION_H_
#define _SERVERAECONDITION_H_

#include "../../Enums.h"
#include "../../ValueQT.h"
#include "../ServerEnums.h"

#include <map>
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class AeCategory;
class AeAttribute;
class AeConditionDefinition;
class AeSubConditionDefinition;

class TBC_EXPORT AeCondition
{

	friend class OTSGlobals;
	friend class Application;

private:
	unsigned long m_handle;
	unsigned long getHandle(void)
	{
		return m_handle;
	}
	void setHandle(unsigned long aValue)
	{
		m_handle = aValue;
	}

protected:

	EnumConditionState m_stateChange;
	unsigned short m_changeMask;
	AeCategory* m_eventCategory;
	unsigned long m_severity;
	tstring m_sourcePath;
	tstring m_message;
	DateTime m_occurenceTime;
	tstring m_name;
	tstring m_activeSubConditionName;
	EnumQuality m_quality;
	BOOL m_ackRequired;
	tstring m_ackId;
	tstring m_ackComment;
	DateTime m_ackTime;

	std::vector<Variant> m_eventAttributes;
	unsigned long m_userData;

	AeConditionDefinition* m_definition;

public:
	AeCondition();
	virtual ~AeCondition();

	EnumConditionState getStateChange(void)
	{
		return m_stateChange;
	}
	void setStateChange(EnumConditionState aValue)
	{
		m_stateChange = aValue;
	}

	unsigned short getChangeMask(void)
	{
		return m_changeMask;
	}
	void setChangeMask(unsigned short aValue)
	{
		m_changeMask = aValue;
	}

	AeCategory* getCategory(void)
	{
		return m_eventCategory;
	}
	void setCategory(AeCategory* aValue)
	{
		m_eventCategory = aValue;
	}

	unsigned long getSeverity(void)
	{
		return m_severity;
	}
	void setSeverity(unsigned long aValue)
	{
		m_severity = aValue;
	}

	tstring& getSourcePath(void)
	{
		return m_sourcePath;
	}
	void setSourcePath(tstring& aValue)
	{
		m_sourcePath = aValue;
	}

	tstring& getMessage(void)
	{
		return m_message;
	}
	void setMessage(tstring& aValue)
	{
		m_message = aValue;
	}

	DateTime getOccurenceTime(void)
	{
		return m_occurenceTime;
	}
	void setOccurenceTime(DateTime aValue)
	{
		m_occurenceTime = aValue;
	}

	tstring& getName(void)
	{
		return m_name;
	}
	void setName(tstring& aValue)
	{
		m_name = aValue;
	}

	tstring& getActiveSubConditionName(void)
	{
		return m_activeSubConditionName;
	}
	void setActiveSubConditionName(tstring& aValue)
	{
		m_activeSubConditionName = aValue;
	}

	EnumQuality getQuality(void)
	{
		return m_quality;
	}
	void setQuality(EnumQuality aValue)
	{
		m_quality = aValue;
	}

	BOOL getAckRequired(void)
	{
		return m_ackRequired;
	}
	void setAckRequired(BOOL aValue)
	{
		m_ackRequired = aValue;
	}

	tstring& getAckId(void)
	{
		return m_ackId;
	}
	void setAckId(tstring& aValue)
	{
		m_ackId = aValue;
	}

	tstring& getAckComment(void)
	{
		return m_ackComment;
	}
	void setAckComment(tstring& aValue)
	{
		m_ackComment = aValue;
	}

	DateTime getAckTime(void)
	{
		return m_ackTime;
	}
	void setAckTime(DateTime aValue)
	{
		m_ackTime = aValue;
	}

	std::vector<Variant>& getEventAttributes(void);
	void setEventAttributes(std::vector<Variant>& aValue);

	unsigned long getUserData(void)
	{
		return m_userData;
	}
	void setUserData(unsigned long aValue)
	{
		m_userData = aValue;
	}

	AeConditionDefinition* getDefinition(void)
	{
		return m_definition;
	}
	void setDefinition(AeConditionDefinition* aValue)
	{
		m_definition = aValue;
	}

	virtual long activate(
		tstring& aMessage,
		unsigned long aSeverity,
		tstring& aSubConditionName,
		BOOL anAckRequired,
		DateTime anOccuranceTime);

	virtual long deactivate(
		tstring& aMessage,
		unsigned long aSeverity,
		BOOL anAckRequired,
		DateTime anOccuranceTime);

	virtual long acknowledge(
		tstring& anAckId,
		tstring& anAckComment,
		DateTime anAckTime);

	virtual long change();
	virtual long enable(tstring& anAddressSpaceElementPath);
	virtual long disable(tstring& anAddressSpaceElementPath);

};  //  end class AeCategory

class TBC_EXPORT AeSubConditionDefinition
{

protected:
	tstring m_definition;
	tstring m_description;
	unsigned long m_severity;

public:
	AeSubConditionDefinition(tstring& aDefinition);
	AeSubConditionDefinition(
		tstring& aDefinition,
		tstring& aDescription,
		unsigned long aSeverity);

	virtual ~AeSubConditionDefinition();

	const tstring& getDefinition(void)
	{
		return m_definition;
	}

	const tstring& getDescription(void)
	{
		return m_description;
	}
	void setDescription(tstring& aValue)
	{
		m_description = aValue;
	}

	unsigned long getSeverity(void)
	{
		return m_severity;
	}
	void setSeverity(unsigned long aValue)
	{
		m_severity = aValue;
	}

};  //  end AeSubConditionDefinition

class TBC_EXPORT AeConditionDefinition
{

	friend class OTSGlobals;

public:
	AeConditionDefinition(tstring& aDefinition);
	virtual ~AeConditionDefinition(void);

	tstring& getDefinition(void)
	{
		return m_definition;
	}

	std::map<tstring, AeSubConditionDefinition*>& getSubConditions(void)
	{
		return m_subconditions;
	}   //  end getSubConditions

	long addSubConditonData(
		tstring& aDefinition,
		tstring& aDescription,
		unsigned long aSeverity);

	long addSubConditonData(tstring& aDefinition);

	AeSubConditionDefinition* getSubConditionDefinition(tstring& aDefinition);

protected:
	tstring m_definition;
	std::map<tstring, AeSubConditionDefinition*> m_subconditions;

};  //  end class AeConditionDefinition


}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _AECONDITION_H_
