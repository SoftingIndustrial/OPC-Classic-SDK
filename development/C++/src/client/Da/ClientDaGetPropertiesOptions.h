#ifndef _CLIENTDAGETPROPERTIESOPTIONS_H_
#define _CLIENTDAGETPROPERTIESOPTIONS_H_

#include "../../Enums.h"
#include "../ClientEnums.h"

#include <vector>
#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class TBC_EXPORT  DaGetPropertiesOptions
{

private:
protected:

	EnumPropertyData m_whatPropertyData;
	std::vector<unsigned long> m_pPropertyIds;
	std::vector<tstring> m_pPropertyNames;

public:

	DaGetPropertiesOptions();
	virtual ~DaGetPropertiesOptions();

	EnumPropertyData getWhatPropertyData();
	void setWhatPropertyData(EnumPropertyData somePropertyData);

	std::vector<unsigned long>& getPropertyIds();
	void setPropertyIds(std::vector<unsigned long>& propertyIds);

	std::vector<tstring>& getPropertyNames();
	void setPropertyNames(std::vector<tstring>& propertyNames);

};  //end class DaGetPropertiesOptions
};// end SoftingOPCToolboxClient

#pragma pack(pop)
#endif
