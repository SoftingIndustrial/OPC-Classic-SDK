#ifndef _SERVERAEATTRIBUTE_H_
#define _SERVERAEATTRIBUTE_H_

#include "../../Enums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class TBC_EXPORT AeAttribute
{

private:
	unsigned long m_id;
	tstring m_name;
	VARENUM m_dataType;

public:
	AeAttribute(
		unsigned long anId,
		tstring& aName,
		VARENUM aDataType);

	unsigned long getId(void)
	{
		return m_id;
	}
	void setId(unsigned long aValue)
	{
		m_id = aValue;
	}

	tstring& getName(void)
	{
		return m_name;
	}
	void setName(tstring& aValue)
	{
		m_name = aValue;
	}

	VARENUM getDataType(void)
	{
		return m_dataType;
	}
	void setDataType(VARENUM  aValue)
	{
		m_dataType = aValue;
	}

};  //  end class Attribute

}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _AEATTRIBUTE_H_
