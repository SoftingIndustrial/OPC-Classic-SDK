#include "OSCompat.h"
#include "ClientDaGetPropertiesOptions.h"

using namespace SoftingOPCToolboxClient;

DaGetPropertiesOptions::DaGetPropertiesOptions()
{
	m_whatPropertyData = (EnumPropertyData)0;
} //end constructor

DaGetPropertiesOptions::~DaGetPropertiesOptions()
{
}//end destructor

EnumPropertyData DaGetPropertiesOptions::getWhatPropertyData()
{
	return m_whatPropertyData;
} //end getWhatPropertyData
void DaGetPropertiesOptions::setWhatPropertyData(EnumPropertyData somePropertyData)
{
	m_whatPropertyData = somePropertyData;
} //end
std::vector<unsigned long>& DaGetPropertiesOptions::getPropertyIds()
{
	return m_pPropertyIds;
} //end
void DaGetPropertiesOptions::setPropertyIds(std::vector<unsigned long>& propertyIds)
{
	m_pPropertyIds = propertyIds;
} //end
std::vector<tstring>& DaGetPropertiesOptions::getPropertyNames()
{
	return m_pPropertyNames;
} //end
void DaGetPropertiesOptions::setPropertyNames(std::vector<tstring>& propertyNames)
{
	m_pPropertyNames = propertyNames;
} //end
