#ifndef _MYDAADDRESSSPACEELEMENT_H_
#define _MYDAADDRESSSPACEELEMENT_H_

#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaProperty.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyDaAddressSpaceElement
//-----------------------------------------------------------------------------
class MyDaAddressSpaceElement : public DaAddressSpaceElement
{
public:
	MyDaAddressSpaceElement(
		tstring& anItemID,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle) :
		DaAddressSpaceElement(
			anItemID, aName, anUserData, anObjectHandle, aParentHandle)
	{
		m_isChanging = false;
	} // end ctr

	MyDaAddressSpaceElement(void)
	{
		m_isChanging = false;
	} // end ctr

	virtual ~MyDaAddressSpaceElement(void)
	{
		size_t count = m_properties.size();

		for (size_t index = 0; index < count; index++)
		{
			DaProperty* property = m_properties[index];

			if (property != NULL)
			{
				delete property;
			} // end if
		} // end if
	} // end destructor


	virtual void addedToAddressSpace(void)
	{
		// add your notification code here
	} // end addedToAddressSpace

	virtual void removedFromAddressSpace(void)
	{
		// add your notification code here
	} // end removedFromAddressSpace

	void getPropertyValue(DaRequest* aRequest)
	{
		if (aRequest->getPropertyId() == EnumPropertyId_ITEM_DESCRIPTION)
		{
			DateTime now;
			now.now();
			Variant aVariant(_T("description"));
			ValueQT aValue(aVariant, EnumQuality_GOOD, now);
			aRequest->setValue(aValue);
			aRequest->setResult(S_OK);
		} // end if ... else
	} // end getPropertyValue

	long queryProperties(std::vector<DaProperty*>& aPropertyList)
	{
		aPropertyList = m_properties;
		return S_OK;
	} // end QueryProperties

	long addProperty(DaProperty* aProperty)
	{
		if (aProperty == NULL)
		{
			return S_FALSE;
		} // end if

		m_properties.push_back(aProperty);
		return S_OK;
	} // end AddProperty

	void setIsChanging(bool isChanging)
	{
		m_isChanging = isChanging;
	}
	bool getIsChanging(void)
	{
		return m_isChanging;
	}

private:
	std::vector<DaProperty*> m_properties;
	bool m_isChanging;

};  // end class MyDaAddressSpaceElement

#endif
