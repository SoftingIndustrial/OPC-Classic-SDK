#ifndef _MYDAADDRESSSPACEELEMENT_H_
#define _MYDAADDRESSSPACEELEMENT_H_


#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaProperty.h"

using namespace SoftingOPCToolboxServer;

extern unsigned long g_angle;



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
		unsigned long aParentHandle) : DaAddressSpaceElement(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
	{
		m_type = TYPE_UNKNOWN;
	}   //  end ctr

	MyDaAddressSpaceElement(void)
	{
		m_type = TYPE_UNKNOWN;
	}   //  end ctr

	virtual ~MyDaAddressSpaceElement(void)
	{
		size_t count = m_properties.size();

		for (size_t index = 0; index < count; index++)
		{
			DaProperty* pProperty = m_properties[index];

			if (pProperty != NULL)
			{
				delete pProperty;
			}   //  end if
		}   //  end if
	}   //  end destructor

	static const unsigned char TYPE_UNKNOWN;
	static const unsigned char TYPE_NODEMATH;
	static const unsigned char TYPE_NODERANDOM;
	static const unsigned char TYPE_RANDOM_100MS;
	static const unsigned char TYPE_RANDOM_1S;
	static const unsigned char TYPE_SINE;
	static const unsigned char TYPE_ACCEPT;

	virtual void addedToAddressSpace(void) {}

	virtual void removedFromAddressSpace(void) {}

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
		DateTime now;
		now.now();

		if (aRequest->getPropertyId() == 101)
		{
			if (getType() == TYPE_RANDOM_100MS)
			{
				Variant aVariant(_T("Random value changing every 100 ms"));
				ValueQT aValue(aVariant, EnumQuality_GOOD, now);
				aRequest->setValue(aValue);
				aRequest->setResult(S_OK);
			}
			else if (getType() == TYPE_RANDOM_1S)
			{
				Variant aVariant(_T("Random value changing every second"));
				ValueQT aValue(aVariant, EnumQuality_GOOD, now);
				aRequest->setValue(aValue);
				aRequest->setResult(S_OK);
			}
			else if (getType() == TYPE_NODERANDOM)
			{
				Variant aVariant(_T("Random values"));
				ValueQT aValue(aVariant, EnumQuality_GOOD, now);
				aRequest->setValue(aValue);
				aRequest->setResult(S_OK);
			}   //  end if ... else
		}
		else if (aRequest->getPropertyId() == 6020)
		{
			Variant aVariant(g_angle);
			ValueQT aValue(aVariant, EnumQuality_GOOD, now);
			aRequest->setValue(aValue);
			aRequest->setResult(S_OK);
		}
		else
		{
			bool found = false;
			size_t count = m_properties.size();

			for (size_t index = 0; index < count && !found; index++)
			{
				DaConstantProperty* pProperty = (DaConstantProperty*)m_properties[index];

				if (pProperty != NULL &&
					aRequest->getPropertyId() == pProperty->getId())
				{
					found = true;
					aRequest->setValue(pProperty->getValue());
					aRequest->setResult(S_OK);
				}   //  end if
			}   //  end for
		}
	}   //  end GetPropertyValue

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

	void change(
		BOOL isActive,
		int anUpdateRate)
	{
		DaAddressSpaceElement::change(isActive, anUpdateRate);		
	}   //  end change

private:
	std::vector<DaProperty*> m_properties;
	unsigned char m_type;

};  //  end class MyDaAddressSpaceElement

#endif
