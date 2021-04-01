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
		m_type = TYPE_NODE_NO_IO;
	}   //  end ctr

	MyDaAddressSpaceElement(void)
	{
		m_type = TYPE_NODE_NO_IO;
	}   //  end ctr

	virtual ~MyDaAddressSpaceElement(void)
	{
		size_t count = m_properties.size();

		for (size_t index = 0; index < count; index++)
		{
			DaProperty* property = m_properties[index];

			if (property != NULL)
			{
				delete property;
			}   //  end if
		}   //  end if
	}   //  end destructor

	static const unsigned char TYPE_NODE_NO_IO;
	static const unsigned char TYPE_REPORT_SEC;
	static const unsigned char TYPE_REPORT_MIN;
	static const unsigned char TYPE_POLL_SEC;
	static const unsigned char TYPE_POLL_MIN;

	virtual void addedToAddressSpace(void) {} // end addedToAddressSpace

	virtual void removedFromAddressSpace(void) {} // end removedFromAddressSpace

	virtual unsigned char getType(void)
	{
		return m_type;
	}
	virtual void setType(unsigned char aValue)
	{
		m_type = aValue;
	}

	void getPropertyValue(DaRequest* aRequest)
	{
		if (aRequest->getPropertyId() == 101)
		{
			DateTime now;
			now.now();
			Variant aVariant(_T("description"));
			ValueQT aValue(aVariant, EnumQuality_GOOD, now);
			aRequest->setValue(aValue);
			aRequest->setResult(S_OK);
		}   //  end if ... else
	}   //  end getPropertyValue

	long queryProperties(std::vector<DaProperty*>& aPropertyList)
	{
		aPropertyList = m_properties;
		return S_OK;
	}   //  end QueryProperties

	long addProperty(DaProperty* aProperty)
	{
		if (aProperty == NULL)
		{
			return S_FALSE;
		}   //  end if

		m_properties.push_back(aProperty);
		return S_OK;
	}   //  end AddProperty

private:
	std::vector<DaProperty*> m_properties;
	unsigned char m_type;
};  //  end class MyDaAddressSpaceElement

#endif
