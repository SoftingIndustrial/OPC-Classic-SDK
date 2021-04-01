#ifndef _CLIENTAEATTRIBUTE_H_
#define _CLIENTAEATTRIBUTE_H_

#include "../ClientEnums.h"
#include "../../Enums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


namespace SoftingOPCToolboxClient
{

class TBC_EXPORT AeAttribute
{

private:

protected:

	unsigned long m_id;
	tstring m_description;
	VARTYPE m_dataType;

public:

	AeAttribute()
	{
		m_id          = 0;
		m_description = _T("");
		m_dataType    = 0;
	} //end constructor

	AeAttribute(
		unsigned long id,
		tstring description,
		unsigned short type)
	{
		m_id = id;
		m_description = description;
		m_dataType = type;
	} //end constructor

	virtual ~AeAttribute() {} //end destructor

	unsigned long getId()
	{
		return m_id;
	} //end getId
	void setId(unsigned long id)
	{
		m_id = id;
	} //end setId

	tstring& getDescription()
	{
		return m_description;
	} //end getDescription
	void setDescription(tstring description)
	{
		m_description = description;
	} //end setDescription

	VARTYPE getDataType()
	{
		return m_dataType;
	} //end getDataType
	void setDataType(VARTYPE dataType)
	{
		m_dataType = dataType;
	} //end setDataType

}; // end class AeAttribute

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
