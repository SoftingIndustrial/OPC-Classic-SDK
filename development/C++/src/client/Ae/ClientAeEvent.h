#ifndef _CLIENTAEEVENT_H_
#define _CLIENTAEEVENT_H_

#include "../ClientEnums.h"
#include "../../Enums.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class AeAttribute;
class TBC_EXPORT AeEvent
{

	friend class Application;

private:

protected:

	EnumEventType m_eventType;
	unsigned long m_eventCategory;
	unsigned long m_severity;
	tstring m_sourcePath;
	tstring m_message;
	DateTime m_occurenceTime;
	std::vector<Variant*> m_eventAttributes;
	tstring m_actorId;

public:

	AeEvent()
	{
		m_eventType         = (EnumEventType)7;
		m_eventCategory     = 0;
		m_severity          = 0;
		m_sourcePath        = _T("");
		m_message           = _T("");
		m_actorId           = _T("");
	} //end constructor

	AeEvent(
		EnumEventType anEventType,
		unsigned long eventCategory,
		unsigned long severity,
		tstring sourcePath,
		tstring message,
		DateTime occurenceTime,
		std::vector<Variant*> someEventAttributes,
		tstring actorId)
	{
		m_eventType         = anEventType;
		m_eventCategory     = eventCategory;
		m_severity          = severity;
		m_sourcePath        = sourcePath;
		m_message           = message;
		m_occurenceTime     = occurenceTime;
		m_eventAttributes   = someEventAttributes;
		m_actorId           = actorId;
	} //end constructor

	virtual ~AeEvent()
	{
		std::vector<Variant*>::iterator variantIterator;

		for (variantIterator = m_eventAttributes.begin(); variantIterator != m_eventAttributes.end(); variantIterator++)
		{
			if (*variantIterator)
			{
				delete(*variantIterator);
				(*variantIterator) = NULL;
			} //end if
		} //end for
	}//end destructor


	EnumEventType getEventType()
	{
		return m_eventType;
	} //end getEventType

	void setEventType(EnumEventType anEventType)
	{
		m_eventType = anEventType;
	} //end setEventType


	unsigned long getCategory()
	{
		return m_eventCategory;
	} //end getCategory

	void setCategory(unsigned long categoryId)
	{
		m_eventCategory = categoryId;
	} //end setCategory

	unsigned long getSeverity()
	{
		return m_severity;
	} //end getSeverity
	void setSeverity(unsigned long severity)
	{
		m_severity = severity;
	}

	tstring& getSourcePath()
	{
		return m_sourcePath;
	} //end getSourcePath

	void setSourcePath(tstring sourcePath)
	{
		m_sourcePath = sourcePath;
	} //end setSourcePath

	tstring& getMessage()
	{
		return m_message;
	} //end getMessage

	void setMessage(tstring message)
	{
		m_message = message;
	} //end setMessage

	DateTime getOcurrenceTime()
	{
		return m_occurenceTime;
	} //end getOcurrenceTime

	void setOccurenceTime(DateTime occurenceTime)
	{
		m_occurenceTime = occurenceTime;
	} //end setOccurenceTime

	std::vector<Variant*>& getAttributes()
	{
		return m_eventAttributes;
	} //end getAttributes

	void setAttributes(std::vector<Variant*> someAttributes)
	{
		if (someAttributes == m_eventAttributes)
		{
			return;
		}

		m_eventAttributes.clear();

		for (std::vector<Variant*>::iterator it = someAttributes.begin(); it != someAttributes.end(); ++it)
		{
			Variant* var = new Variant(*it);
			m_eventAttributes.push_back(var);
		}
	} //end setAttributes

	tstring& getActorId()
	{
		return m_actorId;
	} //end getActorId
	void setActorId(tstring actorId)
	{
		m_actorId = actorId;
	} //end setActorId


}; // end class AeAttribute

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
