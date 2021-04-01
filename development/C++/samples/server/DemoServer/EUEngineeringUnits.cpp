#include "stdafx.h"
#include "EUEngineeringUnits.h"

EUEngineeringUnits::EUEngineeringUnits(
	int anId,
	tstring& aName,
	tstring& anItemId,
	ValueQT& aValue) : DaProperty()
{
	m_id        = anId;
	m_name  = aName;
	m_itemId    = anItemId;
	m_value = aValue;
}

EUEngineeringUnits::~EUEngineeringUnits()
{
}

ValueQT& EUEngineeringUnits::getValue()
{
	return m_value;
}

void EUEngineeringUnits::setValue(ValueQT value)
{
	m_value = value;
}
