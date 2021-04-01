#include "OSCompat.h"
#include "ClientDaSubscription.h"
#include "ClientDaItem.h"

#include "../../Trace.h"
#include "../ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

DaSubscription::DaSubscription(
	unsigned long updateRate,
	DaSession* parentSession)
{
	m_session = parentSession;
	long result = E_FAIL;
	result = parentSession->addDaSubscription(
				 updateRate,
				 this);

	if (FAILED(result))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::Constructor"),
			_T(" Subscription could not be added to the session"));
	}   //  end if
} // end constructor

DaSubscription::~DaSubscription()
{
	m_itemListJanitor.lock();
	m_itemList.clear();
	m_itemListJanitor.unlock();
} // end destructor

DaItem* DaSubscription::findItem(unsigned long position)
{
	m_itemListJanitor.lock();

	if (m_itemList.size() == 0)
	{
		m_itemListJanitor.unlock();
		return NULL;
	}   //  end if

	std::map<unsigned long, DaItem*>::iterator daItemIterator;
	daItemIterator = m_itemList.find(position);

	if (daItemIterator != m_itemList.end())
	{
		m_itemListJanitor.unlock();
		return daItemIterator->second;
	}   //  end if

	m_itemListJanitor.unlock();
	return NULL;
}
DaSession* DaSubscription::getDaSession()
{
	return m_session;
}// end getSession

void DaSubscription::setDaSession(DaSession* aDaSession)
{
	m_session = aDaSession;
} // end setDaSession

unsigned long DaSubscription::getRequestedUpdateRate()
{
	unsigned long requestedUpdateRate = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE;
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
				_T("DaSubscription::getRequestedUpdateRate::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_UPDATERATE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestedUpdateRate = objectAttributes.m_daSubscriptionRequestedUpdateRate;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getRequestedUpdateRate::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestedUpdateRate;
} //end getRequestedUpdateRate
void DaSubscription::setRequestedUpdateRate(unsigned long requestedUpdateRate)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSubscriptionRequestedUpdateRate = requestedUpdateRate;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_UPDATERATE,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setRequestedUpdateRate::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE;
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
				_T("DaSubscription::setRequestedUpdateRate::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setRequestedUpdateRate
unsigned long DaSubscription::getRevisedUpdateRate()
{
	unsigned long revisedUpdateRate = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE;
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
				_T("DaSubscription::getRevisedUpdateRate::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_UPDATERATE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		revisedUpdateRate = objectAttributes.m_daSubscriptionRevisedUpdateRate;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getRevisedUpdateRate::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return revisedUpdateRate;
} //end
unsigned long DaSubscription::getRequestedKeepAliveTime()
{
	unsigned long requestedKeepAliveTime = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME;
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
				_T("DaSubscription::getRequestedKeepAliveTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_KEEPALIVETIME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestedKeepAliveTime = objectAttributes.m_daSubscriptionRequestedKeepAliveTime;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getRequestedKeepAliveTime::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestedKeepAliveTime;
} //end
void DaSubscription::setRequestedKeepAliveTime(unsigned long requestedKeepAliveTime)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSubscriptionRequestedKeepAliveTime = requestedKeepAliveTime;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_KEEPALIVETIME,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setRequestedKeepAliveTime::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME;
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
				_T("DaSubscription::setRequestedKeepAliveTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end getRequestedKeepAliveTime

unsigned long DaSubscription::getRevisedKeepAliveTime()
{
	unsigned long revisedKeepAliveTime = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME;
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
				_T("DaSubscription::getRevisedKeepAliveTime::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_KEEPALIVETIME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		revisedKeepAliveTime = objectAttributes.m_daSubscriptionRevisedKeepAliveTime;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getRevisedKeepAliveTime::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return revisedKeepAliveTime;
} //end getRevisedKeepAliveTime

EnumDataRetrievalMode DaSubscription::getDataRetrievalMode()
{
	EnumDataRetrievalMode dataRetrievalMode = (EnumDataRetrievalMode)0;
	OTCObjectAttributes objectAttributes;

	unsigned long getAttrResult = OTCGetAttributes(
		getHandle(),
		EnumObjectAttribute_DASUBSCRIPTION_DATARETRIEVAL,
		&objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		dataRetrievalMode = (EnumDataRetrievalMode) objectAttributes.m_daSubscriptionDataRetreivalMode;
	}
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getDataRetrievalMode::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return dataRetrievalMode;
} //end setDataRetrievalMode

void DaSubscription::setDataRetrievalMode(EnumDataRetrievalMode dataRetrievalMode)
{
	OTCObjectAttributes objectAttributes;
	objectAttributes.m_daSubscriptionDataRetreivalMode = dataRetrievalMode;

	long setAttrsResult = OTCSetAttributes(
		getHandle(),
		EnumObjectAttribute_DASUBSCRIPTION_DATARETRIEVAL,
		&objectAttributes);

	if (!SUCCEEDED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setDataRetrievalMode::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} //end else
} //end setDataRetrievalMode

tstring DaSubscription::getName()
{
	tstring name;
	OTCObjectAttributes objectAttributes;
	unsigned long whatAttributes[1];
	long results[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_NAME;
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
				_T("DaSubscription::getName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_NAME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		name = objectAttributes.m_daSubscriptionName;
		OTFreeMemory(objectAttributes.m_daSubscriptionName);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getName::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return name;
} //end getName

void DaSubscription::setName(tstring name)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
#ifdef TBC_OS_WINDOWS
	objectAttributes.m_daSubscriptionName = _tcsdup(name.c_str());
#endif
#ifdef TBC_OS_LINUX
	objectAttributes.m_daSubscriptionName = strdup(name.c_str());
#endif
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_NAME,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setName::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_NAME;
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
				_T("DaSubscription::setName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	free(objectAttributes.m_daSubscriptionName);
} //end setName

float DaSubscription::getDeadband()
{
	float deadband = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND;
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
				_T("DaSubscription::getDeadband::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_DEADBAND,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		deadband = objectAttributes.m_daSubscriptionDeadband;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getDeadband::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return deadband;
} //end getDeadband
void DaSubscription::setDeadband(float deadband)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSubscriptionDeadband = deadband;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_DEADBAND,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setDeadband::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND;
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
				_T("DaSubscription::setDeadband::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setDeadband

long DaSubscription::getTimeBias()
{
	long timeBias = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS;
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
				_T("DaSubscription::getTimeBias::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_TIMEBIAS,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		timeBias = objectAttributes.m_daSubscriptionTimeBias;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getTimeBias::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return timeBias;
} //end getTimeBias
void DaSubscription::setTimeBias(long timeBias)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSubscriptionTimeBias = timeBias;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_TIMEBIAS,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setTimeBias::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS;
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
				_T("DaSubscription::setTimeBias::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setTimeBias
tstring DaSubscription::getLocaleId()
{
	unsigned long lcid = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_LCID;
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
				_T("DaSubscription::getLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DASUBSCRIPTION_LCID,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		lcid = objectAttributes.m_daSubscriptionLCID;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::getLocaleId::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	std::map<unsigned long, tstring>::iterator localeIdIterator;
	localeIdIterator = Application::Instance()->m_localeIdToStringList.find(lcid);
	return localeIdIterator->second;
} //end getLocaleId
void DaSubscription::setLocaleId(tstring localeId)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long lcid = 0;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	std::map<unsigned long, tstring>::const_iterator localeIdIterator;

	for (localeIdIterator = Application::Instance()->m_localeIdToStringList.begin();
		 localeIdIterator != Application::Instance()->m_localeIdToStringList.end();
		 localeIdIterator++)
	{
		if (((*localeIdIterator).second) == localeId)
		{
			lcid = localeIdIterator->first;
		} //end if
	} //end for

	objectAttributes.m_daSubscriptionLCID = lcid;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DASUBSCRIPTION_LCID,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription::setLocaleId::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASUBSCRIPTION_LCID;
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
				_T("DaSubscription::setLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setLocaleId

long DaSubscription::addDaItem(
	tstring itemId,
	DaItem* anItem)
{
	long result = E_FAIL;
	unsigned long itemHandle;
#ifdef TBC_OS_WINDOWS
	OTChar* otcItemId = _tcsdup(itemId.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcItemId = strdup(itemId.c_str());
#endif

	if (anItem)
	{
		result = OTCAddDAItem(
					 getHandle(),
					 otcItemId,
					 anItem->getUserData(),
					 &itemHandle);

		if (SUCCEEDED(result))
		{
			anItem->setDaSubscription(this);
			anItem->setHandle(itemHandle);
			m_itemListJanitor.lock();
			m_itemList.insert(DaItemPair(anItem->getUserData(), anItem));
			m_itemListJanitor.unlock();
		} //end if
	} //end
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription.AddDaItem"),
			_T(" The item argument is invalid! It cannot be null!"));
	} //end else

	free(otcItemId);
	return result;
} //end

long DaSubscription::addDaItem(
	DaItem* anItem)
{
	long result = E_FAIL;
	unsigned long itemHandle;
#ifdef TBC_OS_WINDOWS
	OTChar* otcItemId = _tcsdup((anItem->getId()).c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcItemId = strdup((anItem->getId()).c_str());
#endif

	if (&anItem)
	{
		result = OTCAddDAItem(
					 getHandle(),
					 otcItemId,
					 anItem->getUserData(),
					 &itemHandle);

		if (SUCCEEDED(result))
		{
			anItem->setDaSubscription(this);
			anItem->setHandle(itemHandle);
			m_itemListJanitor.lock();
			m_itemList.insert(DaItemPair(anItem->getUserData(), anItem));
			m_itemListJanitor.unlock();
		} //end if
	} //end
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription.AddDaItem"),
			_T(" The item argument is invalid! It cannot be null!"));
	} //end else

	free(otcItemId);
	return result;
} //end

long DaSubscription::removeDaItem(DaItem* anItem)
{
	long result = E_FAIL;

	if (anItem)
	{
		result = OTCRemoveDAItem(anItem->getHandle());

		if (SUCCEEDED(result))
		{
			m_itemListJanitor.lock();
			m_itemList.erase(anItem->getUserData());
			m_itemListJanitor.unlock();
		} //end if
	} //end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSubscription.removeDaItem"),
			_T(" The item argument is invalid! It cannot be null!"));
	} // end else

	return result;
} //end

long DaSubscription::read(
	unsigned long maxAge,
	std::vector<DaItem*> items,
	std::vector<ValueQT*>& values,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	unsigned long i;
	long result = E_FAIL;
	OTCExecutionOptions options;
	std::vector<DaItem*>::const_iterator itemsIterator;
	unsigned long count = (unsigned long) items.size();
	unsigned long* pItemHandles = new unsigned long[count];
	OTCValueData* pValues = new OTCValueData[count];
	long* pResults = new long[count];

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemsIterator = items.begin(), i = 0; itemsIterator != items.end() ; itemsIterator++, i++)
	{
		pItemHandles[i] = (*itemsIterator)->getHandle();
	} //end for

	for (i = 0; i < count; i++)
	{
		pValues[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		OTVariantInit(pValues[i].m_pValue);
	} //end for

	result = OTCRead(
				 getHandle(),
				 count,
				 pItemHandles,
				 NULL,
				 NULL,
				 maxAge,
				 pValues,
				 pResults,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			for (i = 0; i < count; i++)
			{
				values[i] = new ValueQT(
					*(pValues[i].m_pValue),
					(EnumQuality)pValues[i].m_quality,
					DateTime(&pValues[i].m_timestamp));
				OTVariantClear(pValues[i].m_pValue);
			} //end for
		} // end if
	}//end if

	if (pResults)
	{
		for (i = 0; i < count; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	for (i = 0; i < count; i++)
	{
		OTVariantClear(pValues[i].m_pValue);
		OTFreeMemory(pValues[i].m_pValue);
	} //end for

	delete[] pValues;
	delete[] pResults;
	delete[] pItemHandles;
	return result;
} //end read


long DaSubscription::write(
	std::vector<DaItem*> items,
	std::vector<ValueQT*> values,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count  = (unsigned long) items.size();
	OTCExecutionOptions options;
	std::vector<DaItem*>::const_iterator itemsIterator;
	std::vector<ValueQT*>::const_iterator valueQTIterator;
	unsigned long* pItemHandles = new unsigned long[count];
	OTCValueData* pValues = new OTCValueData[count];
	long* pResults = new long[count];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemsIterator = items.begin(), i = 0; itemsIterator != items.end() ; itemsIterator++, i++)
	{
		pItemHandles[i] = (*itemsIterator)->getHandle();
	} //end for

	for (valueQTIterator = values.begin(), i = 0 ; valueQTIterator != values.end() ; valueQTIterator++, i++)
	{
		pValues[i].m_pValue = new OTCVariant();
		OTVariantInit(pValues[i].m_pValue);
		OTVariantCopy(pValues[i].m_pValue, (LPVARIANT)&((*valueQTIterator)->getData()));
		pValues[i].m_quality = (unsigned short)(*valueQTIterator)->getQuality();
#ifdef TBC_OS_WINDOWS
		pValues[i].m_timestamp.dwHighDateTime = (*valueQTIterator)->getTimeStamp().get().dwHighDateTime;
		pValues[i].m_timestamp.dwLowDateTime = (*valueQTIterator)->getTimeStamp().get().dwLowDateTime;
#endif
#ifdef TBC_OS_LINUX
		pValues[i].m_timestamp.tv_sec = 0;
		pValues[i].m_timestamp.tv_usec = 0;
#endif
	} //end for

	result = OTCWrite(
				 getHandle(),
				 count,
				 pItemHandles,
				 NULL,
				 NULL,
				 pValues,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < count; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	for (i = 0; i < count; i++)
	{
		OTVariantClear(pValues[i].m_pValue);
		delete(pValues[i].m_pValue);
	} //end for

	delete[] pValues;
	delete[] pItemHandles;
	delete[] pResults;
	return result;
} //end write

long DaSubscription::validateDaItems(
	std::vector<DaItem*> items,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count  = (unsigned long) items.size();
	OTCExecutionOptions options;
	std::vector<DaItem*>::const_iterator itemsIterator;
	unsigned long* pItemHandles = new unsigned long[count];
	long* pResults = new long[count];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemsIterator = items.begin(), i = 0; itemsIterator != items.end() ; itemsIterator++, i++)
	{
		pItemHandles[i] = (*itemsIterator)->getHandle();
	} //end for

	result = OTCValidateDAItems(
				 getHandle(),
				 count,
				 pItemHandles,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < count; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	delete[] pItemHandles;
	delete[] pResults;
	return result;
} //end validateItems

long DaSubscription::getDaItemAttributesFromServer(
	std::vector<DaItem*> items,
	std::vector<EnumObjectAttribute> whatAttributes,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long itemsCount  = (unsigned long) items.size();
	std::vector<DaItem*>::const_iterator itemsIterator;
	unsigned long attributesCount = (unsigned long) whatAttributes.size();
	std::vector<EnumObjectAttribute>::const_iterator attributesIterator;
	unsigned long* pItemHandles     = new unsigned long[itemsCount];
	unsigned long* pWhatAttributes  = new unsigned long[attributesCount];
	long* pResults          = new long[attributesCount];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemsIterator = items.begin(), i = 0; itemsIterator != items.end() ; itemsIterator++, i++)
	{
		pItemHandles[i] = (*itemsIterator)->getHandle();
	} //end for

	for (attributesIterator = whatAttributes.begin(), i = 0; attributesIterator != whatAttributes.end() ; attributesIterator++, i++)
	{
		pWhatAttributes[i] = (*attributesIterator);
	} //end for

	result = OTCUpdateDAItemAttributes(
				 getHandle(),
				 itemsCount,
				 pItemHandles,
				 1,//the update is made from server
				 attributesCount,
				 pWhatAttributes,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < attributesCount; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	delete[] pWhatAttributes;
	delete[] pItemHandles;
	delete[] pResults;
	return result;
} //end getDaItemAttributesFromServer


long DaSubscription::setDaItemAttributesToServer(
	std::vector<DaItem*> items,
	std::vector<EnumObjectAttribute> whatAttributes,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long itemsCount  = (unsigned long) items.size();
	std::vector<DaItem*>::const_iterator itemsIterator;
	unsigned long attributesCount = (unsigned long) whatAttributes.size();
	std::vector<EnumObjectAttribute>::const_iterator attributesIterator;
	unsigned long* pItemHandles     = new unsigned long[itemsCount];
	unsigned long* pWhatAttributes  = new unsigned long[attributesCount];
	long* pResults          = new long[attributesCount];
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemsIterator = items.begin(), i = 0; itemsIterator != items.end() ; itemsIterator++, i++)
	{
		pItemHandles[i] = (*itemsIterator)->getHandle();
	} //end for

	for (attributesIterator = whatAttributes.begin(), i = 0; attributesIterator != whatAttributes.end() ; attributesIterator++, i++)
	{
		pWhatAttributes[i] = (*attributesIterator);
	} //end for

	result = OTCUpdateDAItemAttributes(
				 getHandle(),
				 itemsCount,
				 pItemHandles,
				 0,//the update is madeto the server
				 attributesCount,
				 pWhatAttributes,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < attributesCount; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	delete[] pWhatAttributes;
	delete[] pItemHandles;
	delete[] pResults;
	return result;
} //end setdaItemAttributesToServer


long DaSubscription::refresh(
	unsigned long maxAge,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCRefreshDAGroup(
				 getHandle(),
				 maxAge,
				 &options);
	return result;
} //end refresh

void DaSubscription::handleDataChanged(
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleDataChanged"),
		_T(""));
} //end handleDataChange

void DaSubscription::handleDataChangedEx(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleDataChangedEx"),
		_T(""));
} //    end handleDataChangedEx

void DaSubscription::onDataChange(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results)
{
	handleDataChanged(items, values, results);
	handleDataChangedEx(executionContext, items, values, results);

	for (unsigned long i = 0; i < items.size(); i++)
	{
		if (items[i])
		{
			items[i]->onValueChange(*values[i], results[i]);
		}   //  end if
	}   //  end for
}   //  end onDataChangeEx

void DaSubscription::handleReadCompleted(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleReadComplete"),
		_T(""));
}   //  end handleReadComplete

void DaSubscription::onReadComplete(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	handleReadCompleted(
		executionContext,
		items,
		values,
		results,
		result);
}   //  end onReadComplete

void  DaSubscription::handleWriteCompleted(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleWriteComplete"),
		_T(""));
}   //  end handleWriteComplete

void  DaSubscription::onWriteComplete(
	unsigned long executionContext,
	const std::vector<DaItem*>& items,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	handleWriteCompleted(
		executionContext,
		items,
		values,
		results,
		result);
}   //  end onWriteComplete

void  DaSubscription::handleValidateDaItemsCompleted(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<long>& results)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleValidateItems"),
		_T(""));
}   //  end handleValidateItems

void  DaSubscription::onValidateDaItems(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<long>& results)
{
	handleValidateDaItemsCompleted(
		executionContext,
		items,
		results);
}   //  end onValidateItems

void DaSubscription::handleGetDaItemAttributesFromServerCompleted(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleGetDaItemAttributesFromServer"),
		_T(""));
}   //  end handleGetDaItemAttributesFromServerCompleted

void DaSubscription::onGetDaItemAttributesFromServer(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	handleGetDaItemAttributesFromServerCompleted(
		executionContext,
		items,
		whatAttributes,
		results,
		result);
}   //  end handleGetDaItemAttributesFromServerCompleted

void DaSubscription::handleSetDaItemAttributesToServerCompleted(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSubscription::handleSetDaItemAttributesToServer"),
		_T(""));
}   //  end handleSetDaItemAttributesToServerCompleted

void DaSubscription::onSetDaItemAttributesToServer(
	unsigned long executionContext,
	std::vector<DaItem*>& items,
	std::vector<EnumObjectAttribute>& whatAttributes,
	std::vector<long>& results,
	long result)
{
	handleSetDaItemAttributesToServerCompleted(
		executionContext,
		items,
		whatAttributes,
		results,
		result);
}   //  end onSetDaItemAttributesToServer
