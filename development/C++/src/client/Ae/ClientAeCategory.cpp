#include "OSCompat.h"
#include "ClientAeCategory.h"
#include "ClientAeSession.h"
#include "ClientAeAttribute.h"
#include "../ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

unsigned long AeCategory::CategoryIndex = 0;

AeCategory::AeCategory()
{
	m_index = ++(AeCategory::CategoryIndex);
	m_eventType   = (EnumEventType)7;
	m_id          = 0;
	m_description = _T("");
	m_aeSession   = NULL;
} // end constructor

AeCategory::AeCategory(
	EnumEventType anEventType,
	unsigned long id,
	tstring description,
	AeSession* aSession)
{
	m_index = ++(AeCategory::CategoryIndex);
	m_eventType   = anEventType;
	m_id          = id;
	m_description = description;
	m_aeSession   = aSession;
} //end constructor

AeCategory::~AeCategory()
{
} // end destructor

void AeCategory::onQueryAeConditionNames(
	unsigned long executionContext,
	unsigned long categoryId,
	std::vector<tstring>& conditionNames,
	long result)
{
	handleQueryAeConditionNamesCompleted(
		executionContext,
		categoryId,
		conditionNames,
		result);
} //end onQueryAeConditionNames

void AeCategory::onQueryAeSubConditionNames(
	unsigned long executionContext,
	tstring& conditionName,
	std::vector<tstring>& subConditionNames,
	long result)
{
	handleQueryAeSubConditionNamesCompleted(
		executionContext,
		conditionName,
		subConditionNames,
		result);
} //end onQueryAeSubConditionNames

void AeCategory::onQueryAeAttributes(
	unsigned long executionContext,
	unsigned long categoryId,
	std::vector<AeAttribute*>& attributes,
	long result)
{
	handleQueryAeAttributesCompleted(
		executionContext,
		categoryId,
		attributes,
		result);
} //end onQueryAeAttributes

unsigned long AeCategory::getCategoryCode()
{
	return m_index;
} //end getCategoryCode

EnumEventType AeCategory::getEventType()
{
	return m_eventType;
} //end getEventType
void AeCategory::setEventType(EnumEventType anEventType)
{
	m_eventType = anEventType;
} //end setEventType
unsigned long AeCategory::getId()
{
	return m_id;
} //end getId
void AeCategory::setId(unsigned long id)
{
	m_id = id;
} //end setId
tstring& AeCategory::getDescription()
{
	return m_description;
} //end getDescription
void AeCategory::setDescription(tstring description)
{
	m_description = description;
} //end setDescription
AeSession* AeCategory::getSession()
{
	return m_aeSession;
} //end getDaSession
void AeCategory::setSession(AeSession* anAeSession)
{
	m_aeSession = anAeSession;
} //end getDaSession
long AeCategory::queryAeAttributes(
	std::vector<AeAttribute*>& attributes,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long attributesCount = 0;
	unsigned long* pAttributeIds;
	OTChar** pAttributeDescriptions;
	unsigned short* pAttributeDataTypes;
	OTCExecutionOptions options;
	unsigned long i;
	AeSession* aSession = getSession();

	if (aSession == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_CLIENT,
			_T("AeCategory::queryAeAttributes"),
			_T("The Session property of the Category cannot be null! Set the property to a value before calling QueryAEAttributes!"));
		return result;
	} //end if

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext  = 0;
		options.m_executionType     = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCQueryAEAttributes(
				 aSession->getHandle(),
				 this->getId(),
				 &attributesCount,
				 &pAttributeIds,
				 &pAttributeDescriptions,
				 &pAttributeDataTypes,
				 &options);

	if (SUCCEEDED(result))
	{
		if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
		{
			attributes.resize(attributesCount);

			for (i = 0; i < attributesCount; i++)
			{
				attributes[i] = new AeAttribute(
					pAttributeIds[i],
					pAttributeDescriptions[i],
					pAttributeDataTypes[i]);
				OTFreeMemory(pAttributeDescriptions[i]);
			} //end for

			OTFreeMemory(pAttributeIds);
			OTFreeMemory(pAttributeDescriptions);
			OTFreeMemory(pAttributeDataTypes);
		}//end if
	} //end if

	return result;
} //end queryAeAttributes

long AeCategory::queryAeConditionNames(
	std::vector<tstring>& conditionNames,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long conditionNamesCount = 0;
	OTChar** pConditionNames;
	unsigned long i;
	AeSession* aSession = getSession();

	if (aSession == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_CLIENT,
			_T("AeCategory::queryAeConditionNames"),
			_T("The Session property of the Category cannot be null! Set the property to a value before calling queryAeConditionNames!"));
		return result;
	} //end if

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext  = 0;
		options.m_executionType     = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCQueryAEConditions(
				 aSession->getHandle(),
				 this->getId(),
				 &conditionNamesCount,
				 &pConditionNames,
				 &options);

	if (SUCCEEDED(result))
	{
		if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
		{
			conditionNames.resize(conditionNamesCount);

			for (i = 0; i < conditionNamesCount; i++)
			{
				conditionNames[i] = pConditionNames[i];
				OTFreeMemory(pConditionNames[i]);
			} //end for

			OTFreeMemory(pConditionNames);
		} //end if
	} //end if

	return result;
} //end queryAeConditionNames

long AeCategory::queryAeSubConditionsNames(
	tstring conditionName,
	std::vector<tstring>& subConditionNames,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long subConditionNamesCount = 0;
	OTChar** pSubConditionNames;
#ifdef TBC_OS_WINDOWS
	OTChar* otcConditionName = _tcsdup(conditionName.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcConditionName = strdup(conditionName.c_str());
#endif
	unsigned long i;
	AeSession* aSession = getSession();

	if (aSession == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_CLIENT,
			_T("AeCategory::queryAeSubConditionsNames"),
			_T("The Session property of the Category cannot be null! Set the property to a value before calling queryAeSubConditionsNames!"));
		return result;
	} //end if

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext  = 0;
		options.m_executionType     = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCQueryAESubConditions(
				 aSession->getHandle(),
				 otcConditionName,
				 &subConditionNamesCount,
				 &pSubConditionNames,
				 &options);

	if (SUCCEEDED(result))
	{
		if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
		{
			subConditionNames.resize(subConditionNamesCount);

			for (i = 0; i < subConditionNamesCount; i++)
			{
				subConditionNames[i] = pSubConditionNames[i];
				OTFreeMemory(pSubConditionNames[i]);
			} //end for

			OTFreeMemory(pSubConditionNames);
		} //end if
	} //end if

	free(otcConditionName);
	return result;
} //end queryAeSubConditionsNames

void AeCategory::handleQueryAeConditionNamesCompleted(
	unsigned long executionContext,
	unsigned long categoryId,
	std::vector<tstring>& conditionNames,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeCategory::handleQueryAeConditionNames"),
		_T(""));
} //end handleQueryAeConditionNames

void AeCategory::handleQueryAeSubConditionNamesCompleted(
	unsigned long executionContext,
	tstring& conditionName,
	std::vector<tstring>& subConditionNames,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeCategory::handleQueryAeSubConditionNames"),
		_T(""));
} //end handleQueryAeSubConditionNames

void AeCategory::handleQueryAeAttributesCompleted(
	unsigned long executionContext,
	unsigned long categoryId,
	std::vector<AeAttribute*>& attributes,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeCategory::handleQueryAeAttributes"),
		_T(""));
} //end handleQueryAeAttributes
