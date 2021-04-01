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

	//-----------------------------------------------------------------------------
	//  MyDaAddressSpaceElement - constructors
	//
	//-----------------------------------------------------------------------------
	MyDaAddressSpaceElement(
		tstring& anItemID,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

	MyDaAddressSpaceElement(
		tstring& anItemID,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle,
		DWORD dwIOIdentifier);

	MyDaAddressSpaceElement(void);
	MyDaAddressSpaceElement(DWORD dwIOIdentifier);

	//-----------------------------------------------------------------------------
	//  ~MyDaAddressSpaceElement    - destructor
	//
	//-----------------------------------------------------------------------------
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

	//-----------------------------------------------------------------------------
	//  getPropertyValue    - deals with a get property request
	//
	//-----------------------------------------------------------------------------
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

	//-----------------------------------------------------------------------------
	//  queryProperties - returns the list of properties
	//
	//-----------------------------------------------------------------------------
	long queryProperties(std::vector<DaProperty*>& aPropertyList)
	{
		aPropertyList = m_properties;
		return S_OK;
	}   //  end QueryProperties

	//-----------------------------------------------------------------------------
	//  addProperty - adds a property to the end of the property list
	//
	//-----------------------------------------------------------------------------
	long addProperty(DaProperty* aProperty)
	{
		if (aProperty == NULL)
		{
			return S_FALSE;
		}   //  end if

		m_properties.push_back(aProperty);
		return S_OK;
	}   //  end AddProperty

	//-----------------------------------------------------------------------------
	//  getIOIdentifier - returns the IO identifier of the element
	//
	//-----------------------------------------------------------------------------
	DWORD getIOIdentifier()
	{
		return m_dwIOIdentifier;
	};

private:
	std::vector<DaProperty*> m_properties;

	// the IO identifier assigned to each element
	// the values should match the enum ioIdentifier values
	DWORD m_dwIOIdentifier;

};  //  end class MyDaAddressSpaceElement

#endif
