#include "OSCompat.h"
#include "ClientDaSession.h"
#include "ClientDaSubscription.h"
#include "../ClientApplication.h"
#include "ClientDaAddressSpaceElement.h"
#include "../ClientAddressSpaceElementBrowseOptions.h"
#include "ClientDaGetPropertiesOptions.h"
#include "ClientDaProperty.h"
#include "../ClientServerStatus.h"

#include "OTClient.h"

#include <vector>

using namespace SoftingOPCToolboxClient;


DaSession::DaSession(const tstring& url)
{
	//initialize m_subscriptionList
	long result = E_FAIL;
	result = Application::Instance()->addDaSession(url, this);

	if (FAILED(result))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::Constructor"),
			_T(" Session could not be added to the application"));
	}   //  end if
} // end constructor

DaSession::~DaSession()
{
	m_daSubscriptionListJanitor.lock();
	m_daSubscriptionList.clear();
	m_daSubscriptionListJanitor.unlock();
} // end destructor

DaSubscription* DaSession::findSubscription(unsigned long position)
{
	m_daSubscriptionListJanitor.lock();

	if (m_daSubscriptionList.size() == 0)
	{
		m_daSubscriptionListJanitor.unlock();
		return NULL;
	}   //  end if

	std::map<unsigned long, DaSubscription*>::iterator daSubscriptionIterator;
	daSubscriptionIterator = m_daSubscriptionList.find(position);

	if (daSubscriptionIterator != m_daSubscriptionList.end())
	{
		m_daSubscriptionListJanitor.unlock();
		return daSubscriptionIterator->second;
	}   //  end if

	m_daSubscriptionListJanitor.unlock();
	return NULL;
}   //  end findSubscription

tstring DaSession::getUrl(void)
{
	tstring url;
	OTCObjectAttributes objectAttributes;
	unsigned long whatAttributes[1];
	long results[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_URL;
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
				_T("DaSession::getUrl::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_URL,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		url = objectAttributes.m_daSessionURL;
		OTFreeMemory(objectAttributes.m_daSessionURL);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getUrl::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return url;
} // end getUrl


tstring DaSession::getLocaleId()
{
	unsigned long lcid = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	TCHAR arcBuf[128];
	memset(arcBuf, 0, sizeof(arcBuf));
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_LCID;
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
				_T("DaSession::getLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_LCID,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		lcid = objectAttributes.m_daSessionLCID;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getLocaleId::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	std::map<unsigned long, tstring>::iterator localeIdIterator;
	localeIdIterator = Application::Instance()->m_localeIdToStringList.find(lcid);
	return localeIdIterator->second;
}// end getLocaleId

void DaSession::setLocaleId(tstring localeId)
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

	objectAttributes.m_daSessionLCID = lcid;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  (unsigned int)EnumObjectAttribute_DASESSION_LCID,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setLocaleId::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_LCID;
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
				_T("DaSession::setLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} // end setLocaleId


tstring DaSession::getClientName()
{
	tstring clientName;
	OTCObjectAttributes objectAttributes;
	unsigned long whatAttributes[1];
	long results[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_CLIENTNAME;
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
				_T("DaSession::getClientName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_CLIENTNAME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		clientName = objectAttributes.m_daSessionClientName;
		OTFreeMemory(objectAttributes.m_daSessionClientName);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getClientName::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return clientName;
} // end getClientName


void DaSession::setClientName(tstring clientName)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
#ifdef TBC_OS_WINDOWS
	objectAttributes.m_daSessionClientName = _tcsdup(clientName.c_str());
#endif
#ifdef TBC_OS_LINUX
	objectAttributes.m_daSessionClientName = strdup(clientName.c_str());
#endif
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  (unsigned int)EnumObjectAttribute_DASESSION_CLIENTNAME,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setClientName::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_CLIENTNAME;
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
				_T("DaSession::setclientName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	free(objectAttributes.m_daSessionClientName);
} // end setClientName


unsigned long DaSession::getRequestTimeout()
{
	unsigned long requestTimeout = 0;
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_REQUESTTIMEOUT,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestTimeout = objectAttributes.m_daSessionRequestTimeout;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getRequestTimeout::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestTimeout;
} // end getRequestTimeout


void DaSession::setRequestTimeout(unsigned long requestTimeout)
{
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	objectAttributes.m_daSessionRequestTimeout = requestTimeout;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  (unsigned int)EnumObjectAttribute_DASESSION_REQUESTTIMEOUT,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setRequestTimeout::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if
} // end setRequestTimeout


EnumOPCSpecification DaSession::getSupportedOpcSpecification()
{
	unsigned char specification = 0;
	tstring cultureName;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION;
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
				_T("DaSession::getSupportedOpcSpecification::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_SUPPORTEDOPCSPECIFICATION,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		specification = objectAttributes.m_daSessionSupportedOPCSpecification;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getSupportedOpcSpecification::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return (EnumOPCSpecification)specification;
} // end getSupportedOpcSpecification

BOOL DaSession::getPerformInitialRead()
{
	BOOL performInitialRead = FALSE;
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	unsigned long getAttrResult = OTCGetAttributes(
		getHandle(),
		(unsigned int)EnumObjectAttribute_DASESSION_PERFORMINITIALREAD,
		&objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		performInitialRead = objectAttributes.m_daSessionPerformInitialRead;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getPerformInitialRead::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return performInitialRead;
} //end getPerformInitialRead

void DaSession::setPerformInitialRead(BOOL performInitialRead)
{
	OTCObjectAttributes objectAttributes;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	objectAttributes.m_daSessionPerformInitialRead = (unsigned char)performInitialRead;
	long setAttrsResult = OTCSetAttributes(
		getHandle(),
		(unsigned int)EnumObjectAttribute_DASESSION_PERFORMINITIALREAD,
		&objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setPerformInitialRead::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

} // end setPerformInitialRead

EnumOPCSpecification DaSession::getForcedOpcSpecification()
{
	unsigned char specification = 0;
	tstring cultureName;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION;
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
				_T("DaSession::getForcedOpcSpecification::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_FORCEDOPCSPECIFICATION,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		specification = objectAttributes.m_daSessionForcedOPCSpecification;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getForcedOpcSpecification::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return (EnumOPCSpecification)specification;
} // end getForcedOpcSpecification


void DaSession::setForcedOpcSpecification(EnumOPCSpecification forcedSpecification)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSessionForcedOPCSpecification = (unsigned char)forcedSpecification;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  (unsigned int)EnumObjectAttribute_DASESSION_FORCEDOPCSPECIFICATION,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setForcedOpcSpecification::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION;
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
				_T("DaSession::seForcedOpcSpecification::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} // end setForcedOpcSpecification


EnumClassContext DaSession::getClassContext()
{
	EnumClassContext classContext = EnumClassContext_NOT_COM;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_COM_CLSCTX;
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
				_T("DaSession::getClassContext::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  (unsigned int)EnumObjectAttribute_DASESSION_CLASSCONTEXT,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		classContext = (EnumClassContext)objectAttributes.m_daSessionComClsctx;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::getClassContext::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return classContext;
} //end getClassContext


void DaSession::setClassContext(EnumClassContext aClassContext)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daSessionComClsctx = aClassContext;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  (unsigned int)EnumObjectAttribute_DASESSION_CLASSCONTEXT,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession::setClassContext::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DASESSION_COM_CLSCTX;
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
				_T("DaSession::setClassContext::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setClassContext


long DaSession::addDaSubscription(
	unsigned long updateRate,
	DaSubscription* aSubscription)
{
	long result = E_FAIL;
	unsigned long groupHandle;

	if (aSubscription != NULL)
	{
		result = OTCAddDASubscription(
					 getHandle(),
					 updateRate,
					 aSubscription->getUserData(),
					 &groupHandle);

		if (SUCCEEDED(result))
		{
			aSubscription->setDaSession(this);
			aSubscription->setHandle(groupHandle);
			m_daSubscriptionListJanitor.lock();
			m_daSubscriptionList.insert(DaSubscriptionPair(aSubscription->getUserData(), aSubscription));
			m_daSubscriptionListJanitor.unlock();
		} // end if
	} //end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession.AddDaSubscription"),
			_T(" The subscription argument is invalid! It cannot be null!"));
	} // end else

	return result;
} // end addDaSubscription


long DaSession::removeDaSubscription(DaSubscription* aSubscription)
{
	long result = E_FAIL;

	if (aSubscription != NULL)
	{
		result = OTCRemoveDASubscription(aSubscription->getHandle());

		if (SUCCEEDED(result))
		{
			m_daSubscriptionListJanitor.lock();
			m_daSubscriptionList.erase(aSubscription->getUserData());
			m_daSubscriptionListJanitor.unlock();
		} // end if
	} // end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaSession.removeDaSubscription"),
			_T(" The subscription argument is invalid! It cannot be null!"));
	} // end else

	return result;
} //end removeDaSubscription


long DaSession ::read(
	unsigned long maxAge,
	std::vector<tstring> itemIds,
	std::vector<tstring> itemPaths,
	std::vector<ValueQT*>& values,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	unsigned long i;
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long idsCount = (unsigned long) itemIds.size();
	unsigned long pathsCount = (unsigned long) itemPaths.size();
	OTChar** pItemIds = new OTChar*[idsCount];
	OTChar** pItemPaths = new OTChar*[pathsCount];
	std::vector<tstring>::const_iterator itemIdsIterator;
	std::vector<tstring>::const_iterator itemPathsIterator;
	OTCValueData* pValues = new OTCValueData[idsCount];
	long* pResults = new long[idsCount];

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

	for (itemIdsIterator = itemIds.begin(), i = 0; itemIdsIterator != itemIds.end() ; itemIdsIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pItemIds[i] = _tcsdup((*itemIdsIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pItemIds[i] = strdup((*itemIdsIterator).c_str());
#endif
	} //end for

	for (itemPathsIterator = itemPaths.begin(), i = 0 ; itemPathsIterator != itemPaths.end() ; itemPathsIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pItemPaths[i] = _tcsdup((*itemPathsIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pItemPaths[i] = strdup((*itemPathsIterator).c_str());
#endif
	} //end for

	for (i = 0; i < idsCount; i++)
	{
		pValues[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		OTVariantInit(pValues[i].m_pValue);
	} //end for

	result = OTCRead(
				 this->getHandle(),
				 idsCount,
				 NULL,
				 pItemIds,
				 pItemPaths,
				 maxAge,
				 pValues,
				 pResults,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			for (i = 0; i < idsCount; i++)
			{
				values[i] = new ValueQT();
				values[i]->setData(
					*(pValues[i].m_pValue),
					(EnumQuality)pValues[i].m_quality,
					DateTime(&pValues[i].m_timestamp));
				OTVariantClear(pValues[i].m_pValue);
			} //end for
		} // end if
	}//end if

	if (pResults)
	{
		for (i = 0; i < idsCount; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	for (i = 0; i < idsCount; i++)
	{
		free(pItemIds[i]);
	}//end for

	for (i = 0; i < pathsCount; i++)
	{
		free(pItemPaths[i]);
	}//end for

	delete[] pItemIds;
	delete[] pItemPaths;
	delete[] pResults;

	for (i = 0; i < idsCount; i++)
	{
		OTVariantClear(pValues[i].m_pValue);
		OTFreeMemory(pValues[i].m_pValue);
	} //end for

	delete[] pValues;
	return result;
} // end read


long DaSession::write(
	std::vector<tstring> itemIds,
	std::vector<tstring> itemPaths,
	std::vector<ValueQT*>& values,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long idsCount = (unsigned long) itemIds.size();
	unsigned long pathsCount = (unsigned long) itemPaths.size();
	unsigned long i;
	OTCExecutionOptions options;
	OTChar** pItemIds = new OTChar*[idsCount];
	OTChar** pItemPaths = new OTChar*[pathsCount];
	OTCValueData* pValues = new OTCValueData[idsCount];
	long* pResults = new long[idsCount];
	std::vector<tstring>::const_iterator itemIdsIterator;
	std::vector<tstring>::const_iterator itemPathsIterator;
	std::vector<ValueQT*>::const_iterator valueQTIterator;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType = EnumExecutionType_SYNCHRONOUS;
	} //end else

	for (itemIdsIterator = itemIds.begin(), i = 0; itemIdsIterator != itemIds.end() ; itemIdsIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pItemIds[i] = _tcsdup((*itemIdsIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pItemIds[i] = strdup((*itemIdsIterator).c_str());
#endif
	} //end for

	for (itemPathsIterator = itemPaths.begin(), i = 0 ; itemPathsIterator != itemPaths.end() ; itemPathsIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pItemPaths[i] = _tcsdup((*itemPathsIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pItemPaths[i] = strdup((*itemPathsIterator).c_str());
#endif
	} //end for

	for (valueQTIterator = values.begin(), i = 0 ; valueQTIterator != values.end() ; valueQTIterator++, i++)
	{
		pValues[i].m_pValue = new OTVariant();
		OTVariantInit(pValues[i].m_pValue);
		OTVariantCopy(pValues[i].m_pValue, (LPVARIANT)&((*valueQTIterator)->getData()));
		pValues[i].m_quality = pValues[i].m_quality = (unsigned short)(*valueQTIterator)->getQuality();
#ifdef TBC_OS_WINDOWS
		pValues[i].m_timestamp.dwHighDateTime = (*valueQTIterator)->getTimeStamp().get().dwHighDateTime;
		pValues[i].m_timestamp.dwLowDateTime  = (*valueQTIterator)->getTimeStamp().get().dwLowDateTime;
#endif
#ifdef TBC_OS_LINUX
		pValues[i].m_timestamp.tv_sec = 0;
		pValues[i].m_timestamp.tv_usec = 0;
#endif
	} //end for

	result = OTCWrite(
				 this->getHandle(),
				 idsCount,
				 NULL,
				 pItemIds,
				 pItemPaths,
				 pValues,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < idsCount; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	for (i = 0; i < idsCount; i++)
	{
		OTVariantClear(pValues[i].m_pValue);
		delete(pValues[i].m_pValue);
	} //end for

	for (i = 0; i < idsCount; i++)
	{
		free(pItemIds[i]);
	}//end for

	for (i = 0; i < pathsCount; i++)
	{
		free(pItemPaths[i]);
	}//end for

	delete[] pValues;
	delete[] pItemIds;
	delete[] pItemPaths;
	delete[] pResults;
	return result;
}

long DaSession::browse(
	tstring rootId,
	tstring rootPath,
	DaAddressSpaceElementBrowseOptions* someBrowseOptions,
	std::vector<DaAddressSpaceElement*>& addressSpaceElements,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCAddressSpaceBrowseOptions OTCBrowseOptions;
	OTCBrowseOptions.m_accessRightsFilter = 0;
	OTCAddressSpaceElementData* pAddressSpaceElement;
	OTCExecutionOptions options;
	unsigned long addressSpaceElementDataCount = 0;
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType     = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	if (someBrowseOptions != NULL)
	{
		OTCBrowseOptions.m_type                     = (unsigned char)someBrowseOptions->getElementTypeFilter();
#ifdef TBC_OS_WINDOWS
		OTCBrowseOptions.m_elementNameFilter        = _tcsdup((someBrowseOptions->getElementNameFilter()).c_str());
		OTCBrowseOptions.m_vendorFilter             = _tcsdup((someBrowseOptions->getVendorFilter()).c_str());
#endif
#ifdef TBC_OS_LINUX
		OTCBrowseOptions.m_elementNameFilter        = strdup((someBrowseOptions->getElementNameFilter()).c_str());
		OTCBrowseOptions.m_vendorFilter             = strdup((someBrowseOptions->getVendorFilter()).c_str());
#endif
		OTCBrowseOptions.m_continuationPoint        = (OTChar*) OTAllocateMemory((unsigned long)((someBrowseOptions->getContinuationPoint().length() + 1) * sizeof(OTChar)));
		memcpy(OTCBrowseOptions.m_continuationPoint, someBrowseOptions->getContinuationPoint().c_str(), someBrowseOptions->getContinuationPoint().length());
		OTCBrowseOptions.m_accessRightsFilter       = someBrowseOptions->getAccessRightsFilter();
		OTCBrowseOptions.m_datatypeFilter           = someBrowseOptions->getDataTypeFilter();
		OTCBrowseOptions.m_maxElements              = someBrowseOptions->getMaxElements();
		OTCBrowseOptions.m_retrieveItemID           = (unsigned char)someBrowseOptions->getRetrieveItemId();
		OTCBrowseOptions.m_retrieveProperties       = (unsigned char)someBrowseOptions->getReturnProperties();
		OTCBrowseOptions.m_retrievePropertyValues   = (unsigned char)someBrowseOptions->getReturnPropertyValues();
		OTCBrowseOptions.m_forceBrowseUp            = (unsigned char)someBrowseOptions->getForceBrowseUp();
	}

#ifdef TBC_OS_WINDOWS
	OTChar* otcRootId = _tcsdup(rootId.c_str());
	OTChar* otcRootPath = _tcsdup(rootPath.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcRootId = strdup(rootId.c_str());
	OTChar* otcRootPath = strdup(rootPath.c_str());
#endif
	result = OTCBrowseAddressSpace(
				 this->getHandle(),
				 0,
				 otcRootId,
				 otcRootPath,
				 &OTCBrowseOptions,
				 &addressSpaceElementDataCount,
				 &pAddressSpaceElement,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			for (i = 0; i < addressSpaceElementDataCount; i++)
			{
				DaAddressSpaceElement* element = new DaAddressSpaceElement(
					(EnumAddressSpaceElementType)pAddressSpaceElement[i].m_type,
					pAddressSpaceElement[i].m_name,
					pAddressSpaceElement[i].m_itemID,
					pAddressSpaceElement[i].m_objectHandle,
					pAddressSpaceElement[i].m_itemPath,
					this);
				addressSpaceElements.push_back(element);
				OTFreeMemory(pAddressSpaceElement[i].m_name);
				OTFreeMemory(pAddressSpaceElement[i].m_itemID);
				OTFreeMemory(pAddressSpaceElement[i].m_itemPath);
			} //end for

			if (pAddressSpaceElement)
			{
				OTFreeMemory(pAddressSpaceElement);
			} //end if

			someBrowseOptions->setContinuationPoint(OTCBrowseOptions.m_continuationPoint);
		} //end if
	} //end if

	free(OTCBrowseOptions.m_vendorFilter);//it was allocated with tcsdup
	free(OTCBrowseOptions.m_elementNameFilter);// it was allocated with tcsdup
	OTFreeMemory(OTCBrowseOptions.m_continuationPoint);// it was allocated with tcsdup
	free(otcRootId);
	free(otcRootPath);
	return result;
} //end browse


long DaSession::getDaProperties(
	tstring anAddressSpaceElementId,
	tstring anAddressSpaceElementPath,
	DaGetPropertiesOptions* aGetPropertyOptions,
	std::vector<DaProperty*>& someDaProperties,
	ExecutionOptions* someExecutionOptions)
{
	return getDaProperties(
			   NULL,
			   anAddressSpaceElementId,
			   anAddressSpaceElementPath,
			   aGetPropertyOptions,
			   someDaProperties,
			   someExecutionOptions);
}   //  end GetProperties

long DaSession::getDaProperties(
	DaAddressSpaceElement* addressSpaceElement,
	tstring anAddressSpaceElementId,
	tstring anAddressSpaceElementPath,
	DaGetPropertiesOptions* aGetPropertyOptions,
	std::vector<DaProperty*>& someDaProperties,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long propertyDataCount = 0;
	OTCGetDAPropertiesOptions otcPropertyOptions;
	std::vector<unsigned long>::const_iterator propertyIdsIterator;
	std::vector<tstring>::const_iterator propertyNamesIterator;
	OTCDAPropertyData* pPropertyData = NULL;
	BOOL propertiesIdsSet = FALSE;
	BOOL propertiesNamesSet = FALSE;
	unsigned long i;
	memset(&otcPropertyOptions, 0, sizeof(OTCGetDAPropertiesOptions));

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType     = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	unsigned long aeHandle = 0;

	if (addressSpaceElement != NULL)
	{
		aeHandle = addressSpaceElement->getHandle();
	}   //  end if

	if (aGetPropertyOptions != NULL)
	{
		unsigned long propertyIdsCount   = (unsigned long) aGetPropertyOptions->getPropertyIds().size();
		unsigned long propertyNamesCount = (unsigned long) aGetPropertyOptions->getPropertyNames().size();

		if (propertyIdsCount != 0)
		{
			otcPropertyOptions.m_propertyCount = propertyIdsCount;
			otcPropertyOptions.m_pPropertyIDs = new unsigned long[propertyIdsCount];

			for (propertyIdsIterator = aGetPropertyOptions->getPropertyIds().begin(), i = 0 ; propertyIdsIterator != aGetPropertyOptions->getPropertyIds().end() ; propertyIdsIterator++, i++)
			{
				otcPropertyOptions.m_pPropertyIDs[i] = *propertyIdsIterator;
			} //end for

			propertiesIdsSet = true;
		} //end if

		if (propertyNamesCount != 0)
		{
			otcPropertyOptions.m_propertyCount = propertyNamesCount;
			otcPropertyOptions.m_pPropertyNames = new OTChar*[propertyNamesCount];

			for (propertyNamesIterator = aGetPropertyOptions->getPropertyNames().begin() , i = 0; propertyNamesIterator != aGetPropertyOptions->getPropertyNames().end(); propertyNamesIterator++, i++)
			{
#ifdef TBC_OS_WINDOWS
				otcPropertyOptions.m_pPropertyNames[i] = _tcsdup((*propertyNamesIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
				otcPropertyOptions.m_pPropertyNames[i] = strdup((*propertyNamesIterator).c_str());
#endif
			} //end for

			propertiesNamesSet = true;
		} //end if

		otcPropertyOptions.m_getLocalFromAddressSpaceElement = 0;
		otcPropertyOptions.m_whatPropertyData = (unsigned char)aGetPropertyOptions->getWhatPropertyData();
	} //end if
	else
	{
		otcPropertyOptions.m_getLocalFromAddressSpaceElement = 1;
	} //end else

#ifdef TBC_OS_WINDOWS
	OTChar* otcAddressSpaceElementId = _tcsdup(anAddressSpaceElementId.c_str());
	OTChar* otcAddressSpaceElementPath = _tcsdup(anAddressSpaceElementPath.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcAddressSpaceElementId = strdup(anAddressSpaceElementId.c_str());
	OTChar* otcAddressSpaceElementPath = strdup(anAddressSpaceElementPath.c_str());
#endif
	result = OTCGetDAProperties(
				 getHandle(),
				 aeHandle,
				 otcAddressSpaceElementId,
				 otcAddressSpaceElementPath,
				 &otcPropertyOptions,
				 &propertyDataCount,
				 &pPropertyData,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			if (propertyDataCount > 0)
			{
				for (i = 0; i < propertyDataCount; i++)
				{
					ValueQT* aValue = new ValueQT(
						*(pPropertyData[i].m_value.m_pValue),
						(EnumQuality)pPropertyData[i].m_value.m_quality,
						DateTime(&pPropertyData[i].m_value.m_timestamp));
					DaProperty* property = new DaProperty(
						pPropertyData[i].m_ID,
						pPropertyData[i].m_name,
						pPropertyData[i].m_itemID,
						pPropertyData[i].m_itemPath,
						pPropertyData[i].m_description,
						pPropertyData[i].m_datatype,
						aValue,
						pPropertyData[i].m_result);
					someDaProperties.push_back(property);
				} //end for

				for (i = 0; i < propertyDataCount; i++)
				{
					if (pPropertyData[i].m_itemID)
					{
						OTFreeMemory(pPropertyData[i].m_itemID);
					} //end if

					if (pPropertyData[i].m_itemPath)
					{
						OTFreeMemory(pPropertyData[i].m_itemPath);
					}//end if

					if (pPropertyData[i].m_description)
					{
						OTFreeMemory(pPropertyData[i].m_description);
					} //end if

					if (pPropertyData[i].m_name)
					{
						OTFreeMemory(pPropertyData[i].m_name);
					} //end if

					if (pPropertyData[i].m_value.m_pValue)
					{
						OTVariantClear(pPropertyData[i].m_value.m_pValue);
						OTFreeMemory(pPropertyData[i].m_value.m_pValue);
					}
				} //end for

				if (pPropertyData)
				{
					OTFreeMemory(pPropertyData);
				} //end if
			} //end if
		} //end if
	} //end if

	if (propertiesNamesSet)
	{
		for (i = 0; i < otcPropertyOptions.m_propertyCount; i++)
		{
			free(otcPropertyOptions.m_pPropertyNames[i]);
		} //end for

		delete[] otcPropertyOptions.m_pPropertyNames;
	} //end if

	if (propertiesIdsSet)
	{
		delete[] otcPropertyOptions.m_pPropertyIDs;
	} //end if

	free(otcAddressSpaceElementId);
	free(otcAddressSpaceElementPath);
	return result;
} //end getDaProperties


long DaSession::getStatus(
	ServerStatus& aServerStatus,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCServerStatus serverStatus;
	OTCExecutionOptions options;
	memset(&serverStatus, 0, sizeof(OTCServerStatus));

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType     = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCGetServerStatus(
				 getHandle(),
				 &serverStatus,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			std::vector<tstring> lcIds(serverStatus.m_countLCIDs, _T(""));
			std::map<unsigned long, tstring>::iterator localeIdIterator;

			for (unsigned long i = 0; i < serverStatus.m_countLCIDs; i++)
			{
				localeIdIterator = Application::Instance()->m_localeIdToStringList.find(serverStatus.m_supportedLCIDs[i]);

				if (localeIdIterator != Application::Instance()->m_localeIdToStringList.end())
				{
					lcIds[i] = localeIdIterator->second;
				}//end if
			} //end for

			ServerStatus* aStatus = new ServerStatus(
				serverStatus.m_state,
				DateTime(&serverStatus.m_startTime),
				DateTime(&serverStatus.m_currentTime),
				serverStatus.m_vendorInfo,
				serverStatus.m_productVersion,
				lcIds,
				DateTime(&serverStatus.m_lastUpdateTime),
				serverStatus.m_groupCount,
				serverStatus.m_bandwidth,
				serverStatus.m_statusInfo);
			aServerStatus = *aStatus;
			delete aStatus;
		} //end if
	} //end if

	if (serverStatus.m_vendorInfo != NULL)
	{
		OTFreeMemory(serverStatus.m_vendorInfo);
	} //end if

	if (serverStatus.m_productVersion != NULL)
	{
		OTFreeMemory(serverStatus.m_productVersion);
	} //end if

	if (serverStatus.m_statusInfo != NULL)
	{
		OTFreeMemory(serverStatus.m_statusInfo);
	} //end if

	if (serverStatus.m_supportedLCIDs != NULL)
	{
		OTFreeMemory(serverStatus.m_supportedLCIDs);
	} //end if

	return result;
} //end getStatus


long DaSession::activateConnectionMonitor(
	BOOL activate,
	unsigned long checkCycle,
	unsigned long connect1Attempts,
	unsigned long connect1Cycle,
	unsigned long connect2Cycle)
{
	long result = E_FAIL;
	result = OTCActivateConnectionMonitor(
				 getHandle(),
				 (unsigned char)activate,
				 checkCycle,
				 connect1Attempts,
				 connect1Cycle,
				 connect2Cycle);
	return result;
} //end activateConnectionMonitor

long DaSession::getErrorString(
	long errorCode,
	tstring& errorText,
	ExecutionOptions* someExecutionOptions)
{
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

	long result = E_FAIL;
	OTChar* errorString = NULL;
	result = OTCGetErrorString(
				 getHandle(),
				 errorCode,
				 &errorString,
				 &options);

	if (SUCCEEDED(result))
	{
		if (errorString)
		{
			errorText = errorString;
			OTFreeMemory(errorString);
		}//end if
	} //end if

	return result;
} // end getErrorString



long DaSession::logon(
	tstring userName,
	tstring password,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	OTChar* aUserName;
	OTChar* aPassword;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext = someExecutionOptions->getExecutionContext();
		options.m_executionType    = (unsigned char)someExecutionOptions->getExecutionType();
	}
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

#ifdef TBC_OS_WINDOWS
	aUserName = _tcsdup(userName.c_str());
	aPassword = _tcsdup(password.c_str());
#endif
#ifdef TBC_OS_LINUX
	aUserName = strdup(userName.c_str());
	aPassword = strdup(password.c_str());
#endif
	result = OTCLogon(
				 getHandle(),
				 aUserName,
				 aPassword,
				 &options);
	free(aUserName);
	free(aPassword);
	return result;
} //end if

long DaSession::logoff(ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext = someExecutionOptions->getExecutionContext();
		options.m_executionType    = (unsigned char)someExecutionOptions->getExecutionType();
	}
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	result = OTCLogoff(
				 getHandle(),
				 &options);
	return result;
} //end logoff
unsigned char DaSession::handleShutdownRequest(const tstring& reason)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleShutdown"),
		_T(""));
	return 1;
}//end handleShutdown

unsigned char DaSession::onShutdown(const tstring& reason)
{
	return handleShutdownRequest(reason);
} // end onShutdown


void DaSession::handleReadCompleted(
	unsigned long executionContext,
	const std::vector<tstring>& itemIds,
	const std::vector<tstring>& itemPaths,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleReadCompleted"),
		_T(""));
} // end handleReadCompleted

void DaSession::onReadComplete(
	unsigned long executionContext,
	const std::vector<tstring>& itemIds,
	const std::vector<tstring>& itemPaths,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	handleReadCompleted(
		executionContext,
		itemIds,
		itemPaths,
		values,
		results,
		result);
} //end onReadCompleted

void DaSession::handleWriteCompleted(
	unsigned long executionContext,
	const std::vector<tstring>& itemIds,
	const std::vector<tstring>& itemPaths,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleWriteCompleted"),
		_T(""));
} //end handleWriteCompleted

void DaSession::onWriteComplete(
	unsigned long executionContext,
	const std::vector<tstring>& itemIds,
	const std::vector<tstring>& itemPaths,
	const std::vector<ValueQT*>& values,
	const std::vector<long>& results,
	long result)
{
	handleWriteCompleted(
		executionContext,
		itemIds,
		itemPaths,
		values,
		results,
		result);
} //end onWriteCompleted

void DaSession::handleBrowseCompleted(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	tstring& addressSpaceElementId,
	tstring& addressSpaceElementPath,
	std::vector<DaAddressSpaceElement*>& addressSpaceElements,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleBrowseCompleted"),
		_T(""));
} //end handleBrowseCompleted

void DaSession::onBrowse(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	tstring& addressSpaceElementId,
	tstring& addressSpaceElementPath,
	std::vector<DaAddressSpaceElement*>& addressSpaceElements,
	long result)
{
	handleBrowseCompleted(
		executionContext,
		addressSpaceElementHandle,
		addressSpaceElementId,
		addressSpaceElementPath,
		addressSpaceElements,
		result);
}//end onBrowseCompleted
void DaSession::handleGetDaPropertiesCompleted(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	tstring& addressSpaceElementId,
	tstring& addressSpaceElementPath,
	std::vector<DaProperty*>& someDaProperties,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleGetDaPropertiesCompleted"),
		_T(""));
} //end handleGetDaPropertiesCompleted

void DaSession::onGetDaProperties(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	tstring& addressSpaceElementId,
	tstring& addressSpaceElementPath,
	std::vector<DaProperty*>& someDaProperties,
	long result)
{
	handleGetDaPropertiesCompleted(
		executionContext,
		addressSpaceElementHandle,
		addressSpaceElementId,
		addressSpaceElementPath,
		someDaProperties,
		result);
} //end OnGetDaProperties

void DaSession::handleGetStatusCompleted(
	unsigned long executionContext,
	ServerStatus& aServerStatus,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleGetStatusCompleted"),
		_T(""));
} //end handleGetStatusCompleted
void DaSession::onGetServerStatus(
	unsigned long executionContext,
	ServerStatus& aServerStatus,
	long result)
{
	handleGetStatusCompleted(
		executionContext,
		aServerStatus,
		result);
} //end onGetStatus
void DaSession::handleGetErrorStringCompleted(
	unsigned long executionContext,
	long errorCode,
	tstring& errorString,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleGetErrorStringCompleted"),
		_T(""));
} //end handleGetErrorStringCompleted
void DaSession::onGetErrorString(
	unsigned long executionContext,
	long errorCode,
	tstring& errorString,
	long result)
{
	handleGetErrorStringCompleted(
		executionContext,
		errorCode,
		errorString,
		result);
} //end onGetErrorString
void DaSession::handleLogonCompleted(
	unsigned long executionContext,
	tstring& userName,
	tstring& password,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleLogonCompleted"),
		_T(""));
} //end handleLogonCompleted
void DaSession::onLogon(
	unsigned long executionContext,
	tstring& userName,
	tstring& password,
	long result)
{
	handleLogonCompleted(
		executionContext,
		userName,
		password,
		result);
} //end onLogon
void DaSession::handleLogoffCompleted(
	unsigned long executionContext,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaSession::handleLogoffCompleted"),
		_T(""));
} //end handleLogoffCompleted
void DaSession::onLogoff(
	unsigned long executionContext,
	long result)
{
	handleLogoffCompleted(
		executionContext,
		result);
} //end onHandleLogoff
