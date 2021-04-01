#include "stdafx.h"
#include "DaAddressSpaceElement.h"
#include "Function.h"
#include "EUEngineeringUnits.h"

DemoDaAddressSpaceElement::DemoDaAddressSpaceElement
(
	tstring& anItemID,
	tstring& aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle) :
	DaAddressSpaceElement(
		anItemID, aName, anUserData, anObjectHandle, aParentHandle)
{
}   //  end ctr

DemoDaAddressSpaceElement::DemoDaAddressSpaceElement(void)
{
}   //  end ctr

DemoDaAddressSpaceElement::~DemoDaAddressSpaceElement(void)
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


void DemoDaAddressSpaceElement::addedToAddressSpace(void)
{
}   //  end addedToAddressSpace

void DemoDaAddressSpaceElement::removedFromAddressSpace(void)
{
} // end removedFromAddressSpace


void DemoDaAddressSpaceElement::getPropertyValue(DaRequest* pRequest)
{
	DateTime now;
	now.now();

	if (pRequest->getPropertyId() == 101)
	{
		Variant aVariant(_T("description"));
		ValueQT aValue(aVariant, EnumQuality_GOOD, now);
		pRequest->setValue(aValue);
		pRequest->setResult(S_OK);
	}
	else if (pRequest->getPropertyId() == 6020)
	{
		Function* element = dynamic_cast<Function*>(this);
		Variant aVariant(element->m_angle);
		ValueQT aValue(aVariant, EnumQuality_GOOD, now);
		pRequest->setValue(aValue);
		pRequest->setResult(S_OK);
	}
	else if (pRequest->getPropertyId() == 102)
	{
		Variant aVariant(1.0);
		ValueQT aValue(aVariant, EnumQuality_GOOD, now);
		pRequest->setValue(aValue);
		pRequest->setResult(S_OK);
	}
	else if (pRequest->getPropertyId() == 103)
	{
		Variant aVariant(-1.0);
		ValueQT aValue(aVariant, EnumQuality_GOOD, now);
		pRequest->setValue(aValue);
		pRequest->setResult(S_OK);
	}
	else
	{
		bool found = false;
		size_t count = m_properties.size();

		for (size_t index = 0; index < count && !found; index++)
		{
			DaConstantProperty* pProperty = (DaConstantProperty*)m_properties[index];

			if (pProperty != NULL && pRequest->getPropertyId() == pProperty->getId())
			{
				found = true;
				pRequest->setValue(pProperty->getValue());
				pRequest->setResult(S_OK);
			}   //  end if
		}   //  end for
	}
}   //  end getPropertyValue

long DemoDaAddressSpaceElement::queryProperties(std::vector<DaProperty*>& aPropertyList)
{
	aPropertyList = m_properties;
	return S_OK;
}   //  end QueryProperties

long DemoDaAddressSpaceElement::addProperty(DaProperty* pAProperty)
{
	if (pAProperty == NULL)
	{
		return S_FALSE;
	}   //  end if

	m_properties.push_back(pAProperty);
	return S_OK;
}   //  end AddProperty

void DemoDaAddressSpaceElement::simulation(void)
{
}

void DemoDaAddressSpaceElement::init()
{
}
