#ifndef _SERVERAEEVENT_H_
#define _SERVERAEEVENT_H_

#include "../../Enums.h"
#include "../../ValueQT.h"
#include "../ServerEnums.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class TBC_EXPORT AeEvent
{

	friend class OTSGlobals;

protected:

	EnumEventType m_type;
	tstring m_source;
	DateTime m_occurenceTime;
	unsigned long m_categoryId;
	unsigned long m_severity;
	tstring m_actorId;
	tstring m_message;
	std::vector<Variant> m_attributeValueList;

public:

	AeEvent(
		EnumEventType anEventType,
		tstring& aSource,
		unsigned long aCategoryId);

	virtual ~AeEvent();

	EnumEventType getType(void)
	{
		return m_type;
	}
	void setType(EnumEventType aValue)
	{
		m_type = aValue;
	}

	tstring& getSource(void)
	{
		return m_source;
	}
	void setSource(tstring& aValue)
	{
		m_source = aValue;
	}

	unsigned long getCategoryId(void)
	{
		return m_categoryId;
	}
	void setCategoryId(unsigned long aValue)
	{
		m_categoryId = aValue;
	}

	unsigned long getSeverity(void)
	{
		return m_severity;
	}
	void setSeverity(unsigned long aValue)
	{
		m_severity = aValue;
	}

	tstring& getActorId(void)
	{
		return m_actorId;
	}
	void setActorId(tstring& aValue)
	{
		m_actorId = aValue;
	}

	tstring& getMessage(void)
	{
		return m_message;
	}
	void setMessage(tstring& aValue)
	{
		m_message = aValue;
	}

	const std::vector<Variant>& getAttributeValueList(void)
	{
		return m_attributeValueList;
	}
	void setAttributeValueList(std::vector<Variant>& aValue)
	{
		for (unsigned long i = 0; i < m_attributeValueList.size(); i++)
		{
			m_attributeValueList[i].clear();
		}   //  end for

		m_attributeValueList.clear();
		m_attributeValueList.resize(aValue.size());

		for (unsigned long j = 0; j < aValue.size(); j++)
		{
			m_attributeValueList[j] = aValue[j];
		}   //  end for
	}   //  end setAttributeValueList

	const DateTime& getOccurenceTime(void)
	{
		return m_occurenceTime;
	}

	void setOccurenceTime(DateTime& aValue)
	{
		m_occurenceTime = aValue;
	}

	long fire(void);

};  //  end class AeEvent

}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _AEEVENT_H_
