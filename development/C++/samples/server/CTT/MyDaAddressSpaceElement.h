#ifndef _MYDAADDRESSSPACEELEMENT_H_
#define _MYDAADDRESSSPACEELEMENT_H_

#include "Da/ServerDaProperty.h"
#include "Da/ServerDaRequest.h"
#include "Da/ServerDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;


//-----------------------------------------------------------------------------
// MyDaAddressSpaceElement
//-----------------------------------------------------------------------------
class MyDaAddressSpaceElement : public DaAddressSpaceElement
{

public:
	static const unsigned char TYP_UNKNOWN;
	static const unsigned char TYP_NODE;
	static const unsigned char TYP_TAG;

	MyDaAddressSpaceElement(void)
	{
		createProperty();
	}   //  end ctr

	~MyDaAddressSpaceElement(void)
	{
		if (m_pProperty != NULL)
		{
			delete m_pProperty;
			m_pProperty = NULL;
		} // end if
	}   //  end destructor

	virtual unsigned char getType(void)
	{
		return m_Type;
	} // end getType

	virtual void setType(unsigned char aValue)
	{
		m_Type = aValue;
	} // end setType

	long queryProperties(std::vector<DaProperty*>& aPropertyList)
	{
		if (m_pProperty == NULL)
		{
			return EnumResultCode_E_OPC_INVALIDHANDLE;
		} // end if

		aPropertyList.push_back(m_pProperty);
		return EnumResultCode_S_OK;
	}   //  end QueryProperties

	void getPropertyValue(DaRequest& aRequest)
	{
		if (aRequest.getPropertyId() == 101)
		{
			DateTime time;
			time.now();
			Variant propVariant;
			propVariant.setBSTR(_T("item description"));
			ValueQT aValueQT(propVariant, EnumQuality_GOOD, time);
			aRequest.setValue(aValueQT);
			aRequest.setResult(EnumResultCode_S_OK);
		} // end if
	} // end getPropertyValues

protected:

	void createProperty(void)
	{
		tstring propertyName(_T("Description"));
		tstring propertyDescription(_T("Item Description"));
		m_pProperty = new DaProperty();
		m_pProperty->setId(101);
		m_pProperty->setAccessRights(EnumAccessRights_READABLE);
		m_pProperty->setDatatype(VT_BSTR);
		m_pProperty->setName(propertyName);

		if (getItemId().empty())
		{
			m_pProperty->setItemId(propertyName);
		}
		else
		{
			tstring itemId = getItemId() + getApp()->getPropertyDelimiter() +  _T("Description");
			m_pProperty->setItemId(itemId);
		}   //  end if .. else

		m_pProperty->setDescription(propertyDescription);
	}   // end createProperty

private:
	unsigned char m_Type;
	DaProperty* m_pProperty;

};  //  end class MyAddressSpaceElement

#endif
