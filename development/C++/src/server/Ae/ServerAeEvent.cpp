#include "OSCompat.h"
#include "ServerAeEvent.h"
#include "../ServerApplication.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	AeEvent - constructor
//
AeEvent::AeEvent(
	EnumEventType anEventType,
	tstring& aSource,
	unsigned long aCategoryId):
	m_type(anEventType),
	m_source(aSource),
	m_categoryId(aCategoryId),
	m_severity(0)
{
	m_occurenceTime.now();
}   //  enc ctr


//-----------------------------------------------------------------------------
//	AeEvent - destructor
//
AeEvent::~AeEvent()
{
	for (unsigned long index = 0; index < m_attributeValueList.size(); index++)
	{
		m_attributeValueList[index].clear();
	}   //  end for

	m_attributeValueList.clear();
}   //  end destructor


//-----------------------------------------------------------------------------
//	AeEvent - constructor
//
long AeEvent::fire(void)
{
	std::vector<AeEvent*> events(1);
	events[0] = this;
	return Application::Instance()->fireEvents(events);
}   //  end fire
