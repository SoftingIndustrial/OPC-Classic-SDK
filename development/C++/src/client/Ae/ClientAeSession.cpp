#include "OSCompat.h"
#include "ClientAeSession.h"
#include "ClientAeSubscription.h"

#include "../ClientApplication.h"
#include "../ClientServerStatus.h"
#include "../Da/ClientDaAddressSpaceElement.h"
#include "../ClientAddressSpaceElementBrowseOptions.h"

#include "ClientAeCategory.h"
#include "ClientAeCondition.h"
#include "ClientAeEvent.h"
#include "ClientAeAttribute.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

AeSession::AeSession(const tstring& url)
{
	long result = E_FAIL;
	result = Application::Instance()->addAeSession(url, this);

	if (FAILED(result))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::Constructor"),
			_T(" Session could not be added to the application"));
	}   //  end if
} // end constructor

AeSession::~AeSession()
{
	m_aeSubscriptionListJanitor.lock();
	m_aeSubscriptionList.clear();
	m_aeSubscriptionListJanitor.unlock();
	m_aeCategoryListJanitor.lock();
	m_aeCategoryList.clear();
	m_aeCategoryListJanitor.unlock();
} // end destructor

AeSubscription* AeSession::findSubscription(unsigned long position)
{
	m_aeSubscriptionListJanitor.lock();

	if (m_aeSubscriptionList.size() == 0)
	{
		m_aeSubscriptionListJanitor.unlock();
		return NULL;
	}   //  end if

	std::map<unsigned long, AeSubscription*>::iterator aeSubscriptionIterator;
	aeSubscriptionIterator = m_aeSubscriptionList.find(position);

	if (aeSubscriptionIterator != m_aeSubscriptionList.end())
	{
		m_aeSubscriptionListJanitor.unlock();
		return aeSubscriptionIterator->second;
	}   //  end if

	m_aeSubscriptionListJanitor.unlock();
	return NULL;
} //end findSubscription

tstring AeSession::getUrl(void)
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
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_URL;
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
				_T("AeSession::getUrl::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESESSION_URL,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		url = objectAttributes.m_aeSessionURL;

		if (objectAttributes.m_aeSessionURL != NULL)
		{
			OTFreeMemory(objectAttributes.m_aeSessionURL);
		} //end if
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::getUrl::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return url;
} // end getUrl

tstring AeSession::getLocaleId()
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
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_LCID;
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
				_T("AeSession::getLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESESSION_LCID,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		lcid = objectAttributes.m_aeSessionLCID;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::getLocaleId::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	std::map<unsigned long, tstring>::iterator localeIdIterator;
	localeIdIterator = Application::Instance()->m_localeIdToStringList.find(lcid);
	return localeIdIterator->second;
}// end getLocaleId

void AeSession::setLocaleId(tstring localeId)
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

	objectAttributes.m_aeSessionLCID = lcid;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESESSION_LCID,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::setLocaleId::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_LCID;
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
				_T("AeSession::setLocaleId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} // end setLocaleId
tstring AeSession::getClientName()
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
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_CLIENTNAME;
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
				_T("AeSession::getClientName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESESSION_CLIENTNAME,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		clientName = objectAttributes.m_aeSessionClientName;

		if (objectAttributes.m_aeSessionClientName != NULL)
		{
			OTFreeMemory(objectAttributes.m_aeSessionClientName);
		}
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::getClientName::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return clientName;
} // end getClientName


void AeSession::setClientName(tstring clientName)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
#ifdef TBC_OS_WINDOWS
	objectAttributes.m_aeSessionClientName = _tcsdup(clientName.c_str());
#endif
#ifdef TBC_OS_LINUX
	objectAttributes.m_aeSessionClientName = strdup(clientName.c_str());
#endif
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESESSION_CLIENTNAME,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::setClientName::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_CLIENTNAME;
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
				_T("AeSession::setclientName::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	free(objectAttributes.m_aeSessionClientName);
} // end setClientName

EnumClassContext AeSession::getClassContext()
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
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_COM_CLSCTX;
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
				_T("AeSession::getClassContext::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_AESESSION_CLASSCONTEXT,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		classContext = (EnumClassContext)objectAttributes.m_aeSessionComClsctx;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::getClassContext::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return classContext;
} //end getClassContext
void AeSession::setClassContext(EnumClassContext aClassContext)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_aeSessionComClsctx = aClassContext;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_AESESSION_CLASSCONTEXT,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::setClassContext::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_AESESSION_COM_CLSCTX;
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
				_T("AeSession::setClassContext::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setClassContext



long AeSession::addAeSubscription(
	AeSubscription* aSubscription)
{
	long result = E_FAIL;
	unsigned long groupHandle;

	if (aSubscription != NULL)
	{
		result = OTCAddAESubscription(
					 getHandle(),
					 aSubscription->getUserData(),
					 &groupHandle);

		if (SUCCEEDED(result))
		{
			aSubscription->setAeSession(this);
			aSubscription->setHandle(groupHandle);
			m_aeSubscriptionListJanitor.lock();
			m_aeSubscriptionList.insert(AeSubscriptionPair(aSubscription->getUserData(), aSubscription));
			m_aeSubscriptionListJanitor.unlock();
		} // end if
	} //end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::AddAeSubscription"),
			_T(" The subscription argument is invalid! It cannot be null!"));
	} // end else

	return result;
} // end addDaSubscription


long AeSession::removeAeSubscription(AeSubscription* aSubscription)
{
	long result = E_FAIL;

	if (aSubscription != NULL)
	{
		result = OTCRemoveAESubscription(aSubscription->getHandle());

		if (SUCCEEDED(result))
		{
			m_aeSubscriptionListJanitor.lock();
			m_aeSubscriptionList.erase(aSubscription->getUserData());
			m_aeSubscriptionListJanitor.unlock();
		} // end if
	} //end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("AeSession::removeAeSubscription"),
			_T(" The subscription argument is invalid! It cannot be null!"));
	} // end else

	return result;
} //end removeDaSubscription

long AeSession::getStatus(
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
				_T("(null)"));
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

long AeSession::browse(
	AddressSpaceElement* anAddressSpaceElement,
	AddressSpaceElementBrowseOptions* someBrowseOptions,
	std::vector<AddressSpaceElement*>& addressSpaceElements,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCAddressSpaceBrowseOptions OTCBrowseOptions;
	OTCAddressSpaceElementData* pAddressSpaceElements;
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

	OTCBrowseOptions.m_type                     = (unsigned char)someBrowseOptions->getElementTypeFilter();
	OTCBrowseOptions.m_forceBrowseUp            = (unsigned char)someBrowseOptions->getForceBrowseUp();
#ifdef TBC_OS_WINDOWS
	OTCBrowseOptions.m_elementNameFilter        = _tcsdup((someBrowseOptions->getElementNameFilter()).c_str());
#endif
#ifdef TBC_OS_LINUX
	OTCBrowseOptions.m_elementNameFilter        = strdup((someBrowseOptions->getElementNameFilter()).c_str());
#endif
	result = OTCBrowseAddressSpace(
				 getHandle(),
				 anAddressSpaceElement->getHandle(),
				 NULL,
				 NULL,
				 &OTCBrowseOptions,
				 &addressSpaceElementDataCount,
				 &pAddressSpaceElements,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			addressSpaceElements.resize(addressSpaceElementDataCount);

			for (i = 0; i < addressSpaceElementDataCount; i++)
			{
				addressSpaceElements[i] = new AddressSpaceElement(
					(EnumAddressSpaceElementType)pAddressSpaceElements[i].m_type,
					pAddressSpaceElements[i].m_name,
					pAddressSpaceElements[i].m_itemID,
					pAddressSpaceElements[i].m_objectHandle);
				OTFreeMemory(pAddressSpaceElements[i].m_name);
				OTFreeMemory(pAddressSpaceElements[i].m_itemID);
				OTFreeMemory(pAddressSpaceElements[i].m_itemPath);
			} //end for

			if (pAddressSpaceElements)
			{
				OTFreeMemory(pAddressSpaceElements);
			} //end if
		} //end if
	} //end if

	free(OTCBrowseOptions.m_elementNameFilter);
	return result;
} //end browse

long AeSession::queryAeCategories(
	std::vector<AeCategory*>& categories,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count = 0;
	unsigned long* pEventTypes = NULL;
	unsigned long* pCategoryIds = NULL;
	OTChar** pCategoryDescriptions = NULL;
	OTCExecutionOptions options;
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

	result = OTCQueryAECategories(
				 getHandle(),
				 &count,
				 &pEventTypes,
				 &pCategoryIds,
				 &pCategoryDescriptions,
				 &options);

	if (SUCCEEDED(result))
	{
		if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
		{
			categories.resize(count);

			m_aeCategoryListJanitor.lock();
			for (i = 0; i < count; i++)
			{
				categories[i] = new AeCategory(
					(EnumEventType)pEventTypes[i],
					pCategoryIds[i],
					pCategoryDescriptions[i],
					this);
				BOOL exists = FALSE;
				std::map<unsigned long, AeCategory*>::const_iterator aeCategoryIterator;

				for (aeCategoryIterator = m_aeCategoryList.begin(); aeCategoryIterator != m_aeCategoryList.end() ; aeCategoryIterator++)
				{
					if (((aeCategoryIterator->second)->getId() == pCategoryIds[i]))
					{
						exists = true;
					}
				} //end for

				if (!exists)
				{
					m_aeCategoryList.insert(AeCategoryPair(categories[i]->getCategoryCode(), categories[i]));
				} //end if
			}

			m_aeCategoryListJanitor.unlock();

			for (i = 0; i < count; i++)
			{
				if (pCategoryDescriptions[i])
				{
					OTFreeMemory(pCategoryDescriptions[i]);
				}//end if
			} //end for
		} //end if
	} //end if

	if (pEventTypes)
	{
		OTFreeMemory(pEventTypes);
	} //end if

	if (pEventTypes)
	{
		OTFreeMemory(pCategoryIds);
	} //end if

	if (pEventTypes)
	{
		OTFreeMemory(pCategoryDescriptions);
	} //end if

	return result;
} //end queryAeCategories

long AeSession::queryAvailableAeFilters(
	EnumFilterBy& availableFilters,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long otcAvailableFilters = 0;
	OTCExecutionOptions options;

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

	result = OTCQueryAvailableAEFilters(
				 getHandle(),
				 &otcAvailableFilters,
				 &options);

	if (SUCCEEDED(result))
	{
		availableFilters = (EnumFilterBy)otcAvailableFilters;
	} //end if

	return result;
} //end queryAvailableAeFilters

long AeSession::enableAeConditionsByArea(
	std::vector<tstring>& areas,
	BOOL enable,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count = (unsigned long) areas.size();
	std::vector<tstring>::const_iterator areasIterator;
	OTChar** pAreas = new OTChar*[count];
	OTCExecutionOptions options;
	unsigned long i;

	for (areasIterator = areas.begin(), i = 0; areasIterator != areas.end() ; areasIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pAreas[i] = _tcsdup((*areasIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pAreas[i] = strdup((*areasIterator).c_str());
#endif
	} //end for

	for (i = 0; i < count; i++)
	{
		result = OTCEnableAEConditions(
					 getHandle(),
					 (unsigned char)enable,
					 1,//for true the area is taking into account
					 pAreas[i],
					 &options);
	} //end for

	for (i = 0; i < count ; i++)
	{
		free(pAreas[i]);
	} //end for

	delete[] pAreas;
	return result;
}
long AeSession::enableAeConditionsBySource(
	std::vector<tstring>& sources,
	BOOL enable,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count = (unsigned long) sources.size();
	std::vector<tstring>::const_iterator sourcesIterator;
	OTChar** pSources = new OTChar*[count];
	OTCExecutionOptions options;
	unsigned long i;

	for (sourcesIterator = sources.begin(), i = 0; sourcesIterator != sources.end() ; sourcesIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pSources[i] = _tcsdup((*sourcesIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pSources[i] = strdup((*sourcesIterator).c_str());
#endif
	} //end for

	for (i = 0; i < count; i++)
	{
		result = OTCEnableAEConditions(
					 getHandle(),
					 (unsigned char)enable,
					 0,//for false the source is taking into account
					 pSources[i],
					 &options);
	} //end for

	for (i = 0; i < count ; i++)
	{
		free(pSources[i]);
	} //end for

	delete[] pSources;
	return result;
}
long AeSession::acknowledgeAeConditions(
	const tstring& ackId,
	const tstring& ackComment,
	std::vector<AeCondition*>& someAeConditions,
	std::vector<long>& results,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	unsigned long count = (unsigned long) someAeConditions.size();
	long* pResults = new long[count];
	OTCEventData* pNewConditions = new OTCEventData[count];
	OTCExecutionOptions options;
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

	for (i = 0 ; i < count ; i++)
	{
#ifdef TBC_OS_WINDOWS
		pNewConditions[i].m_sourcePath    = _tcsdup(someAeConditions[i]->getSourcePath().c_str());
		pNewConditions[i].m_conditionName = _tcsdup(someAeConditions[i]->getConditionName().c_str());
#endif
#ifdef TBC_OS_LINUX
		pNewConditions[i].m_sourcePath    = strdup(someAeConditions[i]->getSourcePath().c_str());
		pNewConditions[i].m_conditionName = strdup(someAeConditions[i]->getConditionName().c_str());
#endif
		pNewConditions[i].m_activeTime    = someAeConditions[i]->getActiveTime().get();
		pNewConditions[i].m_cookie        = someAeConditions[i]->getCookie();
	} //end for

#ifdef TBC_OS_WINDOWS
	OTChar* otcAckId         = _tcsdup(ackId.c_str());
	OTChar* otcAckComment = _tcsdup(ackComment.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcAckId         = strdup(ackId.c_str());
	OTChar* otcAckComment = strdup(ackComment.c_str());
#endif
	result = OTCAcknowledgeAEConditions(
				 getHandle(),
				 otcAckId,
				 otcAckComment,
				 count,
				 pNewConditions,
				 pResults,
				 &options);

	if (pResults)
	{
		for (i = 0; i < count; i++)
		{
			results[i] = pResults[i];
		}//end for
	} //end if

	free(otcAckId);
	free(otcAckComment);

	for (i = 0 ; i < count ; i++)
	{
		free(pNewConditions[i].m_sourcePath);
		free(pNewConditions[i].m_conditionName);
	} //end for

	delete[] pNewConditions;
	delete[] pResults;
	return result;
} //end acknowledgeConditions


long AeSession::queryAeSourceConditions(
	tstring& sourceName,
	std::vector<tstring>& conditionNames,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTChar** pConditionNames;
	unsigned long conditionCount;
	OTCExecutionOptions options;
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

#ifdef TBC_OS_WINDOWS
	OTChar* otcSourceName = _tcsdup(sourceName.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcSourceName = strdup(sourceName.c_str());
#endif
	result = OTCQueryAESourceConditions(
				 getHandle(),
				 otcSourceName,
				 &conditionCount,
				 &pConditionNames,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			conditionNames.resize(conditionCount);

			for (i = 0; i < conditionCount; i++)
			{
				conditionNames[i] = pConditionNames[i];
				OTFreeMemory(pConditionNames[i]);
			}
		} //end if
	} //end if

	OTFreeMemory(pConditionNames);
	free(otcSourceName);
	return result;
} //end querySourceConditions

long AeSession::getAeConditionState(
	tstring& sourcePath,
	tstring& conditionName,
	std::vector<AeAttribute*>& attributes,
	AeConditionState& conditionState,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	OTCAEConditionStateData otcConditionState;
	unsigned long count = (unsigned long) attributes.size();
	unsigned long* pAttributeIds = new unsigned long[count];
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

	for (i = 0; i < count ; i++)
	{
		pAttributeIds[i] = attributes[i]->getId();
	} //end for

#ifdef TBC_OS_WINDOWS
	OTChar* otcSourcePath   = _tcsdup(sourcePath.c_str());
	OTChar* otcConditionName = _tcsdup(conditionName.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcSourcePath   = strdup(sourcePath.c_str());
	OTChar* otcConditionName = strdup(conditionName.c_str());
#endif
	result = OTCGetAEConditionState(
				 getHandle(),
				 otcSourcePath,
				 otcConditionName,
				 count,
				 pAttributeIds,
				 &otcConditionState,
				 &options);
	free(otcSourcePath);
	free(otcConditionName);

	if (SUCCEEDED(result))
	{
		if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
		{
			conditionState.setAcknowledgerComment(otcConditionState.m_ackComment);
			conditionState.setAcknowledgerId(otcConditionState.m_ackID);
			conditionState.setConditionAckTime(DateTime(&otcConditionState.m_conditionAckTime));
			conditionState.setConditionActiveTime(DateTime(&otcConditionState.m_conditionActiveTime));
			conditionState.setConditionInactiveTime(DateTime(&otcConditionState.m_conditionInactiveTime));
			conditionState.setQuality((EnumQuality)otcConditionState.m_quality);
			conditionState.setState((EnumConditionState)otcConditionState.m_state);
			conditionState.setSubConditionActiveTime(DateTime(&otcConditionState.m_subConditionActiveTime));
			conditionState.setActiveSubConditionDefinition(otcConditionState.m_actSubConditionDefinition);
			conditionState.setActiveSubConditionDescription(otcConditionState.m_actSubConditionDescription);
			conditionState.setActiveSubConditionName(otcConditionState.m_actSubConditionName);
			conditionState.setActiveSubConditionSeverity(otcConditionState.m_actSubConditionSeverity);
			conditionState.setSubConditionActiveTime(DateTime(&otcConditionState.m_subConditionActiveTime));
			unsigned long subConditionsCount = otcConditionState.m_subConditionCount;
			std::vector<unsigned long>   subConditionsSeverities(subConditionsCount, 0);
			std::vector<tstring> subConditionsDefinitions(subConditionsCount, _T(""));
			std::vector<tstring> subConditionsDescriptions(subConditionsCount, _T(""));
			std::vector<tstring> subConditionsNames(subConditionsCount, _T(""));

			for (i = 0; i < subConditionsCount; i++)
			{
				subConditionsSeverities[i]   = otcConditionState.m_pSubConditionSeverity[i];
				subConditionsDefinitions [i] = otcConditionState.m_pSubConditionDefinition[i];
				subConditionsDescriptions[i] = otcConditionState.m_pSubConditionDescription[i];
				subConditionsNames[i]        = otcConditionState.m_pSubConditionName[i];
				OTFreeMemory(otcConditionState.m_pSubConditionDefinition[i]);
				OTFreeMemory(otcConditionState.m_pSubConditionDescription[i]);
				OTFreeMemory(otcConditionState.m_pSubConditionName[i]);
			} //end for

			conditionState.setSubConditionsSeverities(subConditionsSeverities);
			conditionState.setSubConditionsDefinitions(subConditionsDefinitions);
			conditionState.setSubConditionsDescriptions(subConditionsDescriptions);
			conditionState.setSubConditionsNames(subConditionsNames);
			unsigned long countEventAttributes = otcConditionState.m_eventAttrCount;
			std::vector<long> attributeErrors(countEventAttributes, 0);
			std::vector<Variant*> eventAttributes(countEventAttributes, NULL);

			for (i = 0; i < countEventAttributes; i++)
			{
				eventAttributes[i] = new Variant(otcConditionState.m_pEventAttrs[i]);
				attributeErrors[i] = otcConditionState.m_pAttrErrors[i];
			} //end for

			for (i = 0; i < otcConditionState.m_eventAttrCount; i++)
			{
				OTVariantClear(&otcConditionState.m_pEventAttrs[i]);
			} //end for

			conditionState.setEventAttributes(eventAttributes);
			conditionState.setAttributesErrors(attributeErrors);
			OTFreeMemory(otcConditionState.m_ackComment);
			OTFreeMemory(otcConditionState.m_ackID);
			OTFreeMemory(otcConditionState.m_actSubConditionDefinition);
			OTFreeMemory(otcConditionState.m_actSubConditionDescription);
			OTFreeMemory(otcConditionState.m_actSubConditionName);
			OTFreeMemory(otcConditionState.m_pSubConditionDefinition);
			OTFreeMemory(otcConditionState.m_pSubConditionDescription);
			OTFreeMemory(otcConditionState.m_pSubConditionName);
			OTFreeMemory(otcConditionState.m_pSubConditionSeverity);
			OTFreeMemory(otcConditionState.m_pAttrErrors);
			OTFreeMemory(otcConditionState.m_pEventAttrs);
		} //end if
	} //end if

	delete[] pAttributeIds;
	return result;
} //end getAeConditionState
long AeSession::activateConnectionMonitor(
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

long AeSession::getErrorString(
	long errorCode,
	tstring& errorText,
	ExecutionOptions* someExecutionOptions)
{
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



long AeSession::logon(
	tstring& userName,
	tstring& password,
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
	} // end if
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

long AeSession::logoff(ExecutionOptions* someExecutionOptions)
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

	result = OTCLogoff(
				 getHandle(),
				 &options);
	return result;
} //end logoff

unsigned char AeSession::handleShutdownRequest(const tstring& reason)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleShutdownRequest"),
		_T(""));
	return 1;
} //end handleShutdownRequest


unsigned char AeSession::onShutdown(const tstring& reason)
{
	return handleShutdownRequest(reason);
} //end onShutdown

void AeSession::handleGetStatusCompleted(
	unsigned long executionContext,
	ServerStatus& aServerStatus,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleGetServerStatus"),
		_T(""));
} //end handleGetServerStatus

void AeSession::onGetServerStatus(
	unsigned long executionContext,
	ServerStatus& aServerStatus,
	long result)
{
	handleGetStatusCompleted(
		executionContext,
		aServerStatus,
		result);
} //end onGetServerStatus

void AeSession::handleBrowseCompleted(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	std::vector<AddressSpaceElement*>& addressSpaceElements,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleBrowseAeServer"),
		_T(""));
} //end handleBrowseAeServer


void AeSession::onBrowse(
	unsigned long executionContext,
	unsigned long addressSpaceElementHandle,
	std::vector<AddressSpaceElement*>& addressSpaceElements,
	long result)
{
	handleBrowseCompleted(
		executionContext,
		addressSpaceElementHandle,
		addressSpaceElements,
		result);
} //end onBrowseAeServer

void AeSession::handleQueryAeCategoriesCompleted(
	unsigned long executionContext,
	std::vector<AeCategory*>& categories,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleQueryAeCategories"),
		_T(""));
} //end handleQueryAeCategories


void AeSession::onQueryAeCategories(
	unsigned long executionContext,
	std::vector<AeCategory*>& categories,
	long result)
{
	handleQueryAeCategoriesCompleted(
		executionContext,
		categories,
		result);
}//end onQueryAeCategories

void AeSession::handleQueryAvailableAeFiltersCompleted(
	unsigned long executionContext,
	EnumFilterBy availableFilters,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleQueryAvailableAeFilters"),
		_T(""));
} //end handleQueryAvailableAeFilters


void AeSession::onQueryAvailableAeFilters(
	unsigned long executionContext,
	EnumFilterBy availableFilters,
	long result)
{
	handleQueryAvailableAeFiltersCompleted(
		executionContext,
		availableFilters,
		result);
} //end onQueryAvailableAeFilters

void AeSession::handleEnableAeConditionsByAreaCompleted(
	unsigned long executionContext,
	BOOL enable,
	tstring& area,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleEnableConditionsByArea"),
		_T(""));
} //end handleEnableConditionsByArea


void AeSession::onEnableAeConditionsByArea(
	unsigned long executionContext,
	BOOL enable,
	tstring& area,
	long result)
{
	handleEnableAeConditionsByAreaCompleted(
		executionContext,
		enable,
		area,
		result);
} //end onEnableConditionsByArea

void AeSession::handleEnableAeConditionsBySourceCompleted(
	unsigned long executionContext,
	BOOL enable,
	tstring& source,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleEnableConditionsBySource"),
		_T(""));
} //end handleEnableConditionsBySource


void AeSession::onEnableAeConditionsBySource(
	unsigned long executionContext,
	BOOL enable,
	tstring& source,
	long result)
{
	handleEnableAeConditionsBySourceCompleted(
		executionContext,
		enable,
		source,
		result);
} //end onEnableConditionsBySource

void AeSession::handleAcknowledgeAeConditionsCompleted(
	unsigned long executionContext,
	tstring& ackId,
	tstring& ackComment,
	std::vector<AeCondition*>& someAeConditions,
	std::vector<long>& results,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleAcknowledgeConditions"),
		_T(""));
} //end handleAcknowledgeConditions


void AeSession::onAcknowledgeAeConditions(
	unsigned long executionContext,
	tstring& ackId,
	tstring& ackComment,
	std::vector<AeCondition*>& someAeConditions,
	std::vector<long>& results,
	long result)
{
	handleAcknowledgeAeConditionsCompleted(
		executionContext,
		ackId,
		ackComment,
		someAeConditions,
		results,
		result);
} //end onAcknowledgeConditions

void AeSession::handleQueryAeSourceConditionsCompleted(
	unsigned long executioncontext,
	tstring& sourceName,
	std::vector<tstring>& conditionNames,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleQuerySourceConditions"),
		_T(""));
} //end handleQuerySourceConditions


void AeSession::onQueryAeSourceConditions(
	unsigned long executioncontext,
	tstring& sourceName,
	std::vector<tstring>& conditionNames,
	long result)
{
	handleQueryAeSourceConditionsCompleted(
		executioncontext,
		sourceName,
		conditionNames,
		result);
} //end onQuerySourceConditions

void AeSession::handleGetAeConditionStateCompleted(
	unsigned long executionContext,
	tstring& sourcePath,
	tstring& conditionName,
	std::vector<unsigned long>& attributesIds,
	AeConditionState& conditionState,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleGetAeConditionState"),
		_T(""));
} //end handleGetAeConditionState


void AeSession::onGetAeConditionState(
	unsigned long executionContext,
	tstring& sourcePath,
	tstring& conditionName,
	std::vector<unsigned long>& attributesIds,
	AeConditionState& conditionState,
	long result)
{
	handleGetAeConditionStateCompleted(
		executionContext,
		sourcePath,
		conditionName,
		attributesIds,
		conditionState,
		result);
} //end onGetAeConditionState

void AeSession::handleGetErrorStringCompleted(
	unsigned long executionContext,
	long errorCode,
	tstring& errorText,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleGetErrorString"),
		_T(""));
} //end handleGetErrorString


void AeSession::onGetErrorString(
	unsigned long executionContext,
	long errorCode,
	tstring& errorText,
	long result)
{
	handleGetErrorStringCompleted(
		executionContext,
		errorCode,
		errorText,
		result);
} //end onGetErrorString

void AeSession::handleLogonCompleted(
	unsigned long executionContext,
	tstring& userName,
	tstring& password,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleLogon"),
		_T(""));
} //end handleLogon


void AeSession::onLogon(
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

void AeSession::handleLogoffCompleted(
	unsigned long executionContext,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("AeSession::handleLogoff"),
		_T(""));
} //end handleLogoff


void AeSession::onLogoff(
	unsigned long executionContext,
	long result)
{
	handleLogoffCompleted(
		executionContext,
		result);
} //end onLogoff
