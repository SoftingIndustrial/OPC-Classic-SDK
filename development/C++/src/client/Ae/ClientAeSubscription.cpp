#include "OSCompat.h"
#include "ClientAeSubscription.h"

#include "ClientAeSession.h"
#include "ClientAeCondition.h"

#include "../ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

//-----------------------------------------
// AeSubscription
//-----------------------------------------

AeSubscription::AeSubscription(AeSession* parentSession)
{
	m_session = parentSession;
	long result = E_FAIL;
	result = parentSession->addAeSubscription(this);

	if (FAILED(result))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::Constructor"),
			_T(" Subscription could not be added to the session"));
	}   //  end if
} // end constructor

AeSubscription::~AeSubscription()
{
	std::vector<AeCondition*>::iterator aeConditionIterator;
	m_conditionListJanitor.lock();

	for (aeConditionIterator = m_conditionList.begin(); aeConditionIterator != m_conditionList.end(); aeConditionIterator++)
	{
		delete(*aeConditionIterator);
	}

	m_conditionList.clear();
	m_conditionListJanitor.unlock();
} // end destructor

AeSession* AeSubscription::getAeSession()
{
	return m_session;
}// end getSession

void AeSubscription::setAeSession(AeSession* aDaSession)
{
	m_session = aDaSession;
} // end setDaSession

unsigned long AeSubscription::getRequestedBufferTime()
{
	unsigned long requestedBufferTime = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getRequestedBufferTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_BUFFERTIME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestedBufferTime = objectAttributes.m_aeSubscriptionRequestedBufferTime;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getRequestedBufferTime::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestedBufferTime;
} //end getRequestedBufferTime
void AeSubscription::setRequestedBufferTime(unsigned long requestedBufferTime)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSubscriptionRequestedBufferTime = requestedBufferTime;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_BUFFERTIME,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setRequestedBufferTime::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setRequestedBufferTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setRequestedBufferTime


unsigned long AeSubscription::getRevisedBufferTime()
{
	unsigned long revisedBufferTime = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getRevisedBufferTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_BUFFERTIME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		revisedBufferTime = objectAttributes.m_aeSubscriptionRevisedBufferTime;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getRevisedBufferTime::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return revisedBufferTime;
} //end getRevisedBufferTime

unsigned long AeSubscription::getRequestedMaxSize()
{
	unsigned long requestedMaxSize = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getRequestedMaxSize::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_MAXSIZE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestedMaxSize = objectAttributes.m_aeSubscriptionRequestedMaxSize;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getRequestedMaxSize::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestedMaxSize;
} //end getRequestedMaxSize
void AeSubscription::setRequestedMaxSize(unsigned long maxSize)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSubscriptionRequestedMaxSize = maxSize;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_MAXSIZE,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setRequestedMaxSize::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setRequestedMaxSize::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setRequestedMaxSize

unsigned long AeSubscription::getRevisedMaxSize()
{
	unsigned long revisedMaxSize = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getRevisedMaxSize::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_MAXSIZE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		revisedMaxSize = objectAttributes.m_aeSubscriptionRevisedMaxSize;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getRevisedMaxSize::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return revisedMaxSize;
} //end getRevisedMaxSize

unsigned long AeSubscription::getFilterSeverityLow()
{
	unsigned long filterSeverityLow = 1;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterSeverityLow::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		filterSeverityLow = objectAttributes.m_aeSubscriptionFilterSeverityLow;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterSeverityLow::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return filterSeverityLow;
} //end getFilterSeverityLow

void AeSubscription::setFilterSeverityLow(unsigned long filterSeverityLow)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSubscriptionFilterSeverityLow = filterSeverityLow;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterSeverityLow::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterSeverityLow::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setFilterSeverityLow

unsigned long AeSubscription::getFilterSeverityHigh()
{
	unsigned long filterSeverityHigh = 1;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterSeverityHigh::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		filterSeverityHigh = objectAttributes.m_aeSubscriptionFilterSeverityHigh;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterSeverityHigh::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return filterSeverityHigh;
} //end getFilterSeverityHigh
void AeSubscription::setFilterSeverityHigh(unsigned long filterSeverityHigh)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSubscriptionFilterSeverityHigh = filterSeverityHigh;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterSeverityHigh::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterSeverityHigh::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setFilterSeverityHigh

unsigned long AeSubscription::getFilterEventTypes()
{
	unsigned long eventType = 7;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterEventTypes::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		eventType = objectAttributes.m_aeSubscriptionFilterEventTypes;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterEventTypes::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return eventType;
} //end getFilterEventTypes

void AeSubscription::setFilterEventTypes(unsigned long filterEventTypes)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSubscriptionFilterEventTypes = filterEventTypes;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterEventTypes::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterEventTypes::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setFilterEventTypes
std::vector<unsigned long> AeSubscription::getFilterCategories()
{
	std::vector<unsigned long> filterCategories;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long i;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterCategories::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		unsigned long categoriesCount = objectAttributes.m_aeSubscriptionFilterCountCategrories;
		filterCategories.resize(categoriesCount);

		if (objectAttributes.m_aeSubscriptionFilterCategrories)
		{
			for (i = 0; i < categoriesCount; i++)
			{
				filterCategories[i] = objectAttributes.m_aeSubscriptionFilterCategrories[i];
			} //end for

			OTFreeMemory(objectAttributes.m_aeSubscriptionFilterCategrories);
		} //end if
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterCategories::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return filterCategories;
} //end getFilterCategories

void AeSubscription::setFilterCategories(std::vector<unsigned long> filterCategories)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long categoriesCount = 0;
	unsigned long i = 0;
	unsigned long* pFilterCategories = NULL;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;

	if (!filterCategories.empty())
	{
		categoriesCount = (unsigned long) filterCategories.size();
		pFilterCategories = new unsigned long [categoriesCount];
		objectAttributes.m_aeSubscriptionFilterCountCategrories = categoriesCount;

		for (i = 0; i < categoriesCount; i++)
		{
			pFilterCategories[i] = filterCategories[i];
		} //end for

		objectAttributes.m_aeSubscriptionFilterCategrories = pFilterCategories;
	} //end if

	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterCategories::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterCategories::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	delete[] pFilterCategories;
} //end setFilterCategories

std::vector<tstring> AeSubscription::getFilterAreas()
{
	std::vector<tstring> filterAreas(0, _T(""));
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long i;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterAreas::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		unsigned long areasCount = objectAttributes.m_aeSubscriptionFilterCountAreas;
		filterAreas.resize(areasCount);

		if (objectAttributes.m_aeSubscriptionFilterAreas)
		{
			for (i = 0; i < areasCount; i++)
			{
				filterAreas[i] = (objectAttributes.m_aeSubscriptionFilterAreas[i]);
				OTFreeMemory(objectAttributes.m_aeSubscriptionFilterAreas[i]);
			} //end for

			OTFreeMemory(objectAttributes.m_aeSubscriptionFilterAreas);
		} //end if
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterAreas::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return filterAreas;
} //end

void AeSubscription::setFilterAreas(std::vector<tstring> filterAreas)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long areasCount = 0;
	unsigned long i;
	OTChar** pFilterAreas = NULL;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;

	if (!filterAreas.empty())
	{
		areasCount = (unsigned long) filterAreas.size();
		objectAttributes.m_aeSubscriptionFilterCountAreas = areasCount;
		pFilterAreas = new OTChar*[areasCount];

		for (i = 0; i < areasCount; i++)
		{
#ifdef TBC_OS_WINDOWS
			pFilterAreas[i] = _tcsdup(filterAreas[i].c_str());
#endif
#ifdef TBC_OS_LINUX
			pFilterAreas[i] = strdup(filterAreas[i].c_str());
#endif
		} //end for
	} //end if

	objectAttributes.m_aeSubscriptionFilterAreas = pFilterAreas;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterAreas::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterAreas::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	for (i = 0; i < areasCount; i++)
	{
		free(objectAttributes.m_aeSubscriptionFilterAreas[i]);
	} //end for

	delete[] pFilterAreas;
} //end setFilterAreas

std::vector<tstring> AeSubscription::getFilterSources()
{
	std::vector<tstring> filterSources(0, _T(""));
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long i;
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getFilterSources::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		unsigned long sourcesCount = objectAttributes.m_aeSubscriptionFilterCountSources;
		filterSources.resize(sourcesCount);

		if (objectAttributes.m_aeSubscriptionFilterSources)
		{
			for (i = 0; i < sourcesCount; i++)
			{
				filterSources[i] = (objectAttributes.m_aeSubscriptionFilterSources[i]);
				OTFreeMemory(objectAttributes.m_aeSubscriptionFilterSources[i]);
			} //end for

			OTFreeMemory(objectAttributes.m_aeSubscriptionFilterSources);
		} //end if
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getFilterSources::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return filterSources;
} //end getFilterSources
void AeSubscription::setFilterSources(std::vector<tstring> filterSources)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long sourcesCount = 0;
	unsigned long i;
	OTChar** pFilterSources = NULL;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;

	if (!filterSources.empty())
	{
		sourcesCount = (unsigned long) filterSources.size();
		objectAttributes.m_aeSubscriptionFilterCountSources = sourcesCount;
		pFilterSources = new OTChar*[sourcesCount];

		for (i = 0; i < sourcesCount; i++)
		{
#ifdef TBC_OS_WINDOWS
			pFilterSources[i] = _tcsdup(filterSources[i].c_str());
#endif
#ifdef TBC_OS_LINUX
			pFilterSources[i] = strdup(filterSources[i].c_str());
#endif
		} //end for
	} //end if

	objectAttributes.m_aeSubscriptionFilterSources = pFilterSources;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESUBSCRIPTION_FILTER,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::setFilterSources::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setFilterSources::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	for (i = 0; i < sourcesCount; i++)
	{
		free(objectAttributes.m_aeSubscriptionFilterSources[i]);
	} //end for

	delete[] pFilterSources;
} //end setFilterSources
std::vector<AeReturnedAttributes*> AeSubscription::getReturnedAttributes()
{
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long i, j;
	unsigned long count = 0;
	std::vector<AeReturnedAttributes*> returnedAttributes(count, NULL);
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES;
		long updateResult = OTCUpdateAttributes(
								getHandle(),
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::getReturnedAttributes::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESUBSCRIPTION_RETURNED_ATTRIBUTES,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		unsigned long aeAttributesCount = objectAttributes.m_aeSubscriptionCountReturnedAttributes;
		returnedAttributes.resize(aeAttributesCount);

		if (objectAttributes.m_aeSubscriptionReturnedAttributes)
		{
			for (i = 0; i < aeAttributesCount; i++)
			{
				returnedAttributes[i] = new AeReturnedAttributes();
				returnedAttributes[i]->setCategoryId(objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_categoryID);
				unsigned long attributesCount = objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_countAttributeIDs;
				std::vector<unsigned long> attributesIds(attributesCount, 0);

				for (j = 0; j < attributesCount; j++)
				{
					attributesIds[j] = objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_attributeIDs[j];
				} //end for

				returnedAttributes[i]->setAttributesIds(attributesIds);
				OTFreeMemory(objectAttributes.m_aeSubscriptionReturnedAttributes[i].m_attributeIDs);
			} //end for

			OTFreeMemory(objectAttributes.m_aeSubscriptionReturnedAttributes);
		} //end if
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSubscription::getReturnedAttributes::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return returnedAttributes;
} //end getReturnedAttributes

void AeSubscription::setReturnedAttributes(std::vector<AeReturnedAttributes*> someAeReturnedAttributes)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long aeReturnedAttributesCount = 0;
	OTCAEReturnedAttributesData* returnedAttrs = NULL;
	unsigned long i, j;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;

	if (!someAeReturnedAttributes.empty())
	{
		aeReturnedAttributesCount = (unsigned long) someAeReturnedAttributes.size();
		std::vector<unsigned long*> allAttributesIds(aeReturnedAttributesCount, NULL);
		objectAttributes.m_aeSubscriptionCountReturnedAttributes = aeReturnedAttributesCount;
		returnedAttrs = new OTCAEReturnedAttributesData[aeReturnedAttributesCount];

		for (i = 0; i < aeReturnedAttributesCount; i++)
		{
			returnedAttrs[i].m_categoryID = someAeReturnedAttributes[i]->getCategoryId();
			unsigned long attributesIdsCount = (unsigned long) someAeReturnedAttributes[i]->getAttributesIds().size();
			//unsigned long* attributesIds = new unsigned long[attributesIdsCount];
			allAttributesIds[i] = new unsigned long[attributesIdsCount];

			for (j = 0; j < attributesIdsCount; j++)
			{
				//attributesIds[j] = (someAeReturnedAttributes[i]->getAttributesIds())[j];
				allAttributesIds[i][j] = (someAeReturnedAttributes[i]->getAttributesIds())[j];
			} //end for

			returnedAttrs[i].m_countAttributeIDs = attributesIdsCount;
			//returnedAttrs[i].m_attributeIDs = attributesIds;
			returnedAttrs[i].m_attributeIDs = allAttributesIds[i];
		} //end for

		objectAttributes.m_aeSubscriptionReturnedAttributes = returnedAttrs;
		long setAttrsResult = OTCSetAttributes(
								  getHandle(),
								  EnumObjectAttribute_AESUBSCRIPTION_RETURNED_ATTRIBUTES,
								  &objectAttributes);

		if (FAILED(setAttrsResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("AeSubscription::setReturnedAttributes::OTCSetAttributes"),
				_T(" - code is %x"), setAttrsResult);
		} // end if

		if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
		{
			whatAttributes[0] = EnumObjectAttribute_AESUBSCRIPTION_RETURNED_ATTRIBUTES;
			long updateResult = OTCUpdateAttributes(
									getHandle(),
									0,
									1,
									whatAttributes,
									results,
									&executionOptions);

			if (FAILED(updateResult))
			{
				Application::Instance()->trace(
					EnumTraceLevel_ERR,
					EnumTraceGroup_CLIENT,
					_T("AeSubscription::setReturnedAttributes::OTCUpdateAttributes"),
					_T(" - code is %x"), updateResult);
			} // end if
		} // end if

		std::vector<unsigned long*>::iterator attributesIterator;

		for (attributesIterator = allAttributesIds.begin(); attributesIterator != allAttributesIds.end(); attributesIterator++)
		{
			if ((*attributesIterator) != NULL)
			{
				delete *attributesIterator;
				*attributesIterator = NULL;
			} //end if
		} //end for
	} //end if

	delete [] returnedAttrs;
} //end setReturnedAttributes

std::vector<AeCondition*>& AeSubscription::getAeConditionList()
{
	return m_conditionList;
} //end getAeConditionList

long AeSubscription::refreshAeConditions(ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext = someExecutionOptions->getExecutionContext();
		options.m_executionType    = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCRefreshAEConditions(
				 getHandle(),
				 0,
				 &options);
	return result;
} //end refreshConditions

long AeSubscription::cancelRefreshAeConditions(ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType       = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCRefreshAEConditions(
				 getHandle(),
				 1,
				 &options);
	return result;
} //end cancelRefreshConditions

void AeSubscription::handleAeEventsReceived(
	BOOL isRefresh,
	BOOL lastRefresh,
	const std::vector<AeEvent*>& events)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSubscription::handleAeEventsReceived"),
		_T(""));
} //end handleAeEventsReceived

void AeSubscription::onAeEventsReceived(
	BOOL isRefresh,
	BOOL lastRefresh,
	const std::vector<AeEvent*>& events)
{
	handleAeEventsReceived(
		isRefresh,
		lastRefresh,
		events);
} //end onAeEventsReceived

void AeSubscription::handleAeConditionsChanged(const std::vector<AeCondition*>& someAeConditions)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSubscription::handleAeConditionsChanged"),
		_T(""));
} //end handleAeConditionsChanged

void AeSubscription::onAeConditionsChanged(const std::vector<AeCondition*>& someAeConditions)
{
	handleAeConditionsChanged(someAeConditions);
} //end onAeConditionsChanged

void AeSubscription::handleRefreshAeConditionsCompleted(
	unsigned long executionContext,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSubscription::handleRefreshAeConditions"),
		_T(""));
} //end handleRefreshAeConditions

void AeSubscription::onRefreshAeConditions(
	unsigned long executionContext,
	long result)
{
	handleRefreshAeConditionsCompleted(
		executionContext,
		result);
} //end onRefreshAeConditions

void AeSubscription::handleCancelRefreshAeConditionsCompleted(
	unsigned long executionContext,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSubscription::handleCancelRefreshAeConditions"),
		_T(""));
} //end handleCancelRefreshAeConditions

void AeSubscription::onCancelRefreshAeConditions(
	unsigned long executionContext,
	long result)
{
	handleCancelRefreshAeConditionsCompleted(
		executionContext,
		result);
} //end onCancelRefreshAeConditions
//-----------------------------------------------
// AeReturnedAttributes
//----------------------------------------------

AeReturnedAttributes::~AeReturnedAttributes()
{
} //end destructor
