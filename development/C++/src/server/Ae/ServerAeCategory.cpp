#include "OSCompat.h"
#include "ServerAeCategory.h"
#include "../ServerApplication.h"

#include "OTServer.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// AeCategory - constructor
//
AeCategory::AeCategory(
	EnumEventType anEventType,
	unsigned long anId,
	tstring& aName):
	m_eventType(anEventType),
	m_name(aName),
	m_id(anId),
	m_handle(0)
{
}   //  end constructor


//-----------------------------------------------------------------------------
// AeCategory - constructor
//
AeCategory::AeCategory(
	EnumEventType anEventType,
	unsigned long anId,
	tstring& aName,
	unsigned long aHandle):
	m_eventType(anEventType),
	m_name(aName),
	m_id(anId),
	m_handle(aHandle)
{
}   //  end ctr


//-----------------------------------------------------------------------------
// AeCategory - destructor
//
AeCategory::~AeCategory()
{
	std::map<unsigned long, AeAttribute*>::iterator atributeIterator;

	for (atributeIterator = m_attributes.begin(); atributeIterator != m_attributes.end(); atributeIterator++)
	{
		if (atributeIterator->second != NULL)
		{
			delete atributeIterator->second;
			atributeIterator->second = NULL;
		}   //  end if
	}   //  end for

	m_attributes.clear();
	std::map<tstring, AeConditionDefinition*>::iterator conditionIterator;

	for (conditionIterator = m_conditions.begin(); conditionIterator != m_conditions.end(); conditionIterator++)
	{
		if (conditionIterator->second != NULL)
		{
			delete conditionIterator->second;
			conditionIterator->second = NULL;
		}   //  end if
	}   //  end for

	m_conditions.clear();
}   //  end destructor


//-----------------------------------------------------------------------------
//	getAeAttributeList
//
std::map<unsigned long, AeAttribute*>& AeCategory::getAeAttributeList(void)
{
	return m_attributes;
}   //  end getAeAttributeList


//-----------------------------------------------------------------------------
//	getAeAttributeList
//
long AeCategory::addAttribute(unsigned long anId, tstring& aName, VARENUM aDataType)
{
	AeAttribute* attribute = new AeAttribute(anId, aName, aDataType);

	if (attribute == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	return addAttribute(attribute);
}   //  end addAttribute


//-----------------------------------------------------------------------------
//	getAeAttributeList
//
long AeCategory::addAttribute(AeAttribute* anAttribute)
{
	if (anAttribute == NULL)
	{
		return E_FAIL;
	}   //  end if

	if (m_handle == 0)
	{
		return E_FAIL;
	}   //  end if

	OTChar* otcName = (OTChar*)OTAllocateMemory((unsigned long)(anAttribute->getName().size() + 1) * sizeof(OTChar));

	if (otcName != NULL)
	{
#ifdef TBC_OS_WINDOWS
		_tcscpy(otcName, anAttribute->getName().c_str());
#endif
#ifdef TBC_OS_LINUX
		strcpy(otcName, anAttribute->getName().c_str());
#endif
	}   //  end if

	long result = OTSAddEventAttribute(
					  m_handle,
					  anAttribute->getId(),
					  otcName,
					  (unsigned short)anAttribute->getDataType());

	if (SUCCEEDED(result))
	{
		m_attributes.insert(std::pair<unsigned long, AeAttribute*>(anAttribute->getId(), anAttribute));
	}   //  end if

	if (otcName != NULL)
	{
		OTFreeMemory(otcName);
	}   //  end if

	return result;
}   //  end addAttribute


//-----------------------------------------------------------------------------
//	addConditionDefinition
//
long AeCategory::addConditionDefinition(tstring& aConditionName)
{
	if (aConditionName.length() == 0)
	{
		return E_FAIL;
	}   //  end if

	if (m_handle == 0)
	{
		return E_FAIL;
	}   //  end if

	OTChar* otcConditionName = (OTChar*)OTAllocateMemory((unsigned long)(aConditionName.size() + 1) * sizeof(OTChar));

	if (otcConditionName != NULL)
	{
#ifdef TBC_OS_WINDOWS
		_tcscpy(otcConditionName, aConditionName.c_str());
#endif
#ifdef TBC_OS_LINUX
		strcpy(otcConditionName, aConditionName.c_str());
#endif
	}   //  end if

	long result = OTSAddCondition(m_handle, otcConditionName);

	if (SUCCEEDED(result))
	{
		//  Check for a duplicate condition
		std::map<tstring, AeConditionDefinition*>::const_iterator conditionIterator;
		conditionIterator = m_conditions.find(aConditionName);

		if (conditionIterator == m_conditions.end())
		{
			AeConditionDefinition* condition = new AeConditionDefinition(aConditionName);

			if (condition == NULL)
			{
				return EnumResultCode_E_OUTOFMEMORY;
			}   //  end if

			m_conditions.insert(std::pair<tstring, AeConditionDefinition*>(aConditionName, condition));
		}
		else
		{
			Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
										   _T("AeCategory::addConditionDefinition"),
										   _T("a duplicate condition was detected; add fails"));
			result = E_FAIL;
		}   //  end if
	}   //  end if

	if (otcConditionName != NULL)
	{
		OTFreeMemory(otcConditionName);
	}   //  end if

	return result;
}   //  end addConditionDefinition


//-----------------------------------------------------------------------------
// GetConditionDefinition
//
AeConditionDefinition* AeCategory::getConditionDefinition(tstring& aConditionName)
{
	//  Check for a duplicate condition
	std::map<tstring, AeConditionDefinition*>::const_iterator elementIterator;
	elementIterator = m_conditions.find(aConditionName);

	if (elementIterator == m_conditions.end())
	{
		return NULL;
	}   //  end if

	return elementIterator->second;
}   //  end getConditionDefinition


//-----------------------------------------------------------------------------
// GetConditionDefinition
//
long AeCategory::addSubConditionDefinition(
	tstring& aConditionName,
	tstring& aSubconditionName)
{
	if (aConditionName.length() == 0 || aSubconditionName.length() == 0)
	{
		return E_FAIL;
	}   //  end if

	if (m_handle == 0)
	{
		return E_FAIL;
	}   //  end if

	long result = S_OK;
	AeConditionDefinition* condition = getConditionDefinition(aConditionName);

	if (condition == NULL)
	{
		result = addConditionDefinition(aConditionName);

		if (!SUCCEEDED(result))
		{
			return result;
		}   //  end if

		condition = getConditionDefinition(aConditionName);

		//  if still no condition, something is seriously wrong
		if (condition == NULL)
		{
			Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
										   _T("AeCategory::addSubConditionDefinition"),
										   _T("no such condition found"));
			return E_FAIL;
		}   //  end if
	}   //  end if

	OTChar* otsConditionName = (OTChar*)OTAllocateMemory((unsigned long)(aConditionName.size() + 1) * sizeof(OTChar));

	if (otsConditionName != NULL)
	{
#ifdef TBC_OS_WINDOWS
		_tcscpy(otsConditionName, aConditionName.c_str());
#endif
#ifdef TBC_OS_LINUX
		strcpy(otsConditionName, aConditionName.c_str());
#endif
	}   //  end if

	OTChar* otsSubConditionName = (OTChar*)OTAllocateMemory((unsigned long)(aSubconditionName.size() + 1) * sizeof(OTChar));

	if (otsSubConditionName != NULL)
	{
#ifdef TBC_OS_WINDOWS
		_tcscpy(otsSubConditionName, aSubconditionName.c_str());
#endif
#ifdef TBC_OS_LINUX
		strcpy(otsSubConditionName, aSubconditionName.c_str());
#endif
	}   //  end if

	//  at this time the condition should have been be registered
	result = OTSAddSubCondition(m_handle, otsConditionName, otsSubConditionName);

	if (SUCCEEDED(result))
	{
		result = condition->addSubConditonData(aSubconditionName);
	}   //  end if

	if (otsConditionName != NULL)
	{
		OTFreeMemory(otsConditionName);
	}   //  end if

	if (otsSubConditionName != NULL)
	{
		OTFreeMemory(otsSubConditionName);
	}   //  end if

	return result;
}   //  end addSubConditionDefinition


//-----------------------------------------------------------------------------
//	getConditionDefinition
//
AeSubConditionDefinition* AeCategory::getSubConditionDefinition(
	tstring& aConditionName,
	tstring& aSubconditionName)
{
	AeConditionDefinition* condition = getConditionDefinition(aConditionName);

	if (condition == NULL)
	{
		return NULL;
	}   //  end if

	return condition->getSubConditionDefinition(aSubconditionName);
}   //  end getConditionDefinition
