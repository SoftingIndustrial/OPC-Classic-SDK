#include "OSCompat.h"
#include "ClientDaProperty.h"

using namespace SoftingOPCToolboxClient;

/*
DaProperty::DaProperty(){
    m_id            = 0;
    m_name          = _T("");
    m_itemId        =_T("");
    m_itemPath      =_T("");
    m_description   =_T("");
    m_dataType      = 0;
    m_result        = -1;

} //end constructor
*/

DaProperty::DaProperty(
	unsigned long anId,
	tstring aName,
	tstring anItemId,
	tstring anItemPath,
	tstring aDescription,
	unsigned short aDataType,
	ValueQT* aValue,
	long aResult)
{
	m_id            = anId;
	m_name          = aName;
	m_itemId        = anItemId;
	m_itemPath      = anItemPath;
	m_description   = aDescription;
	m_dataType      = aDataType;
	m_value         = aValue;
	m_result        = aResult;
} //end constructor

DaProperty::~DaProperty()
{
	delete m_value;
}//end destructor

unsigned long DaProperty::getId()
{
	return m_id;
} //end getId

tstring& DaProperty::getName()
{
	return m_name;
} //end getName

tstring& DaProperty::getItemId()
{
	return m_itemId;
} //end getItemId


tstring& DaProperty::getItemPath()
{
	return m_itemPath;
} //end getItemPath


tstring& DaProperty::getDescription()
{
	return m_description;
} //end getDescription

VARTYPE DaProperty::getDataType()
{
	return m_dataType;
} //end getDataType

ValueQT& DaProperty::getValueQT()
{
	return *m_value;
} //end getValueQT


long DaProperty::getResult()
{
	return m_result;
} //end getResult
