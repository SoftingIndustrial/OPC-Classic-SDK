#ifndef _DEVICEEVENT_H_
#define _DEVICEEVENT_H_

#include "BaseEvent.h"

using namespace SoftingOPCToolboxServer;

static const int DEVICE_EVENT_CATEGORY = 2;



//-----------------------------------------------------------------------------
// DeviceEvent
//-----------------------------------------------------------------------------
class DeviceEvent : public BaseEvent
{
public:
	DeviceEvent(
		tstring& aSource,
		Variant& aCvAttribute) :
		BaseEvent(aSource, DEVICE_EVENT_CATEGORY)
	{
		std::vector<Variant> attributeValueList = getAttributeValueList();
		attributeValueList.push_back(aCvAttribute);
	}   //  end ctr

	virtual ~DeviceEvent(
		void)
	{
		if (m_categoryDevice != NULL)
		{
			delete m_categoryDevice;
			m_categoryDevice = NULL;
		} // end if
	} // end dtor


	static void buildCategory(
		void)
	{
		getApp()->addAeCategory(m_categoryDevice);
		m_categoryDevice->addAttribute(5550, tstring(_T("CV")), VT_UI4);
	}   //  end BuildCategory

protected:
	static AeCategory* m_categoryDevice;
};  //  end DataEvent

AeCategory* DeviceEvent::m_categoryDevice = new AeCategory(EnumEventType_SIMPLE, DEVICE_EVENT_CATEGORY, tstring(_T("Device")));

#endif
