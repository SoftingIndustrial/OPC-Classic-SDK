#ifndef _DATAEVENT_H_
#define _DATAEVENT_H_

#include "BaseEvent.h"
#include "Ae/ServerAeCategory.h"

using namespace SoftingOPCToolboxServer;

static const int DATA_EVENT_CATEGORY = 1;



//-----------------------------------------------------------------------------
// DataEvent
//-----------------------------------------------------------------------------
class DataEvent : public BaseEvent
{
public:
	DataEvent(
		const tstring& aSource,
		Variant& aCvAttribute,
		int aDataAttribute,
		const tstring& anAddressAttribute,
		const tstring& aTextAttribute)
		: BaseEvent(aSource, DATA_EVENT_CATEGORY)
	{
		std::vector<Variant> attributeValueList = getAttributeValueList();
		attributeValueList.push_back(aCvAttribute);
		attributeValueList.push_back(aDataAttribute);
		attributeValueList.push_back(anAddressAttribute.c_str());
		attributeValueList.push_back(aTextAttribute.c_str());
	}   //  end ctr

	virtual ~DataEvent(
		void)
	{
		if (m_categoryData != NULL)
		{
			delete m_categoryData;
			m_categoryData = NULL;
		} // end if
	} // end dtor


	static void buildCategory(
		void)
	{
		getApp()->addAeCategory(m_categoryData);
		m_categoryData->addAttribute(5550, tstring(_T("CV")), VT_I4);
		m_categoryData->addAttribute(5551, tstring(_T("DATA")), VT_I4);
		m_categoryData->addAttribute(5552, tstring(_T("ADDRESS")), VT_BSTR);
		m_categoryData->addAttribute(5553, tstring(_T("TEXT")), VT_BSTR);
	}   //  end BuildCategory

protected:
	static AeCategory* m_categoryData;
};  //  end DataEvent

AeCategory* DataEvent::m_categoryData = new AeCategory(EnumEventType_SIMPLE, DATA_EVENT_CATEGORY, tstring(_T("Data")));

#endif
