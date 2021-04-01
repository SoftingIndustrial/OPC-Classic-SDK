#ifndef _SERVERAECATEGORY_H_
#define _SERVERAECATEGORY_H_

#include "../../Enums.h"
#include "../ServerEnums.h"

#include "ServerAeAttribute.h"
#include "ServerAeCondition.h"

#include <map>
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class TBC_EXPORT AeCategory
{

	friend class Application;
protected:

	EnumEventType m_eventType;
	tstring m_name;
	unsigned long m_id;
	unsigned long m_handle;

	std::map<unsigned long, AeAttribute*> m_attributes;
	std::map<tstring, AeConditionDefinition*> m_conditions;

	AeCategory(EnumEventType anEventType, unsigned long anId, tstring& aName, unsigned long aHandle);

	unsigned long getHandle(void)
	{
		return m_handle;
	}
	void setHandle(unsigned long aValue)
	{
		m_handle = aValue;
	}

	virtual long addAttribute(AeAttribute* anAttribute);

public:

	AeCategory(
		EnumEventType anEventType,
		unsigned long anId,
		tstring& aName);

	virtual ~AeCategory();

	//  Properties
	EnumEventType getEventType(void)
	{
		return m_eventType;
	}
	void getEventType(EnumEventType aValue)
	{
		m_eventType = aValue;
	}

	tstring& getName(void)
	{
		return m_name;
	}
	void setName(tstring& aValue)
	{
		m_name = aValue;
	}

	unsigned long getId(void)
	{
		return m_id;
	}
	void setId(unsigned long aValue)
	{
		m_id = aValue;
	}

	virtual std::map<unsigned long, AeAttribute*>& getAeAttributeList(void);

	//  methods
	virtual long addAttribute(
		unsigned long anId,
		tstring& aName,
		VARENUM aDataType);

	virtual long addConditionDefinition(tstring& aConditionName);

	virtual AeConditionDefinition* getConditionDefinition(tstring& aConditionName);

	virtual long addSubConditionDefinition(
		tstring& aConditionName,
		tstring& aSubconditionName);

	virtual AeSubConditionDefinition* getSubConditionDefinition(
		tstring& aConditionName,
		tstring& aSubconditionName);

};  //  end class AeCategory

}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _AECATEGORY_H_
