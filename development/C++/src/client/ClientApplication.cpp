#include "OSCompat.h"
#include "ClientApplication.h"

#include "../Enums.h"
#include "Da/ClientDaSubscription.h"
#include "Ae/ClientAeSubscription.h"
#include "Da/ClientDaItem.h"

#include "ClientObjectSpaceElement.h"
#include "ClientEnums.h"

#include "Da/ClientDaAddressSpaceElement.h"
#include "Da/ClientDaProperty.h"
#include "Ae/ClientAeCondition.h"
#include "Ae/ClientAeEvent.h"
#include "Ae/ClientAeCategory.h"
#include "Ae/ClientAeAttribute.h"

#include "ClientServerBrowser.h"
#include "ClientServerStatus.h"
#include <vector>

#include "ClientCommon.h"
#include "OTClient.h"


#ifdef TBC_OS_LINUX
#include <sys/time.h>
#endif

using namespace SoftingOPCToolboxClient;


TBC_EXPORT Application* API_CALL getApplication()
{
	return Application::Instance();
} //end getApplication


TBC_EXPORT void API_CALL releaseApplication()
{
	Application::Release();
} //end releaseApplication


namespace SoftingOPCToolboxClient
{

class OTCGlobals
{

	friend class Application;

private:

	static long advise();
	static OTChar* allocateOTBString(tstring& aValue);

	static BOOL m_isAdvised;

	static OTCCallbackFunctions m_OTNCCallbacks;

	static void API_CALL handleTrace(
		OTChar* aTraceString,
		unsigned short aTraceLevel,
		unsigned long aTraceGroup,
		OTChar* anObjectId,
		OTChar* aText);

	static void API_CALL handleStateChange(
		IN OTCObjectContext objectContext,
		IN unsigned char state);

	static void API_CALL handleDataChange(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN unsigned long count,
		IN OTCObjectData* pItemData,
		IN OTCValueData* pValues,
		IN long* pResults);

	static void API_CALL handleReadComplete(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned long count,
		IN OTCObjectData* pItemData,
		IN OTChar** pItemIDs,
		IN OTChar** pItemPaths,
		IN OTCValueData* pValues,
		IN long* pResults);

	static void API_CALL handleWriteComplete(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned long count,
		IN OTCObjectData* pItemData,
		IN OTChar** pItemIDs,
		IN OTChar** pItemPaths,
		IN OTCValueData* pValues,
		IN long* pResults);

	static void API_CALL handlePerformStateTransition(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result);

	static unsigned char API_CALL handleServerShutdown(
		IN OTCObjectData sessionData,
		IN OTChar* reason);

	static void API_CALL handleUpdateAttributes(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned char fromServer,
		IN unsigned long attributesCount,
		IN unsigned long* pWhatAttributes,
		IN long* pResults);

	static void API_CALL handleUpdateDaItemAttributes(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned long itemCount,
		IN OTCObjectData* pItemData,
		IN unsigned char fromServer,
		IN unsigned long attributesCount,
		IN unsigned long* pWhatAttributes,
		IN long* pResults);

	static void API_CALL handleValidateDaItems(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned long itemCount,
		IN OTCObjectData* pItemData,
		IN long* pResults);

	static void API_CALL handleGetServerStatus(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTCServerStatus* pServerStatus);

	static void API_CALL handleBrowseServers(
		IN unsigned long executionContext,
		IN long result,
		IN OTChar* ipAddress,
		IN unsigned char whatOPCSpec,
		IN unsigned char whatServerData,
		IN unsigned long serverDataCount,
		IN OTCServerData* pServerData);

	static void API_CALL handleBrowseAddressSpace(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTCObjectHandle addressSpaceElementHandle,
		IN OTChar* addressSpaceElementID,
		IN OTChar* addressSpaceElementPath,
		IN OTCAddressSpaceBrowseOptions* pBrowseOptions,
		IN unsigned long addressSpaceElementDataCount,
		IN OTCAddressSpaceElementData* pAddressSpaceElement);

	static void API_CALL handleGetDaProperties(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTCObjectHandle addressSpaceElementHandle,
		IN OTChar* addressSpaceElementID,
		IN OTChar* addressSpaceElementPath,
		IN OTCGetDAPropertiesOptions* pGetPropertiesOptions,
		IN unsigned long propertyDataCount,
		IN OTCDAPropertyData* pPropertyData);

	static void API_CALL handleAeEventsReceived(
		IN OTCObjectContext objectContext,
		IN unsigned long count,
		IN OTCEventData* pEvents,
		IN unsigned char refresh,
		IN unsigned char lastRefresh);

	static void API_CALL handleRefreshAeConditions(
		IN unsigned long executionContext,
		IN OTCObjectContext objectContext,
		IN long result,
		IN unsigned char cancelRefresh);

	static void API_CALL handleAcknowledgeAeConditions(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTChar* ackID,
		IN OTChar* ackComment,
		IN unsigned long count,
		IN OTCEventData* pEvents,
		IN long* pResults);

	static void API_CALL handleQueryAvailableAeFilters(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN unsigned long availableFilters);

	static void API_CALL handleQueryAeCategories(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN unsigned long count,
		IN unsigned long* pEventTypes,
		IN unsigned long* pCategoryIds,
		IN OTChar* *pCategoryDescriptions);

	static void API_CALL handleQueryAeAttributes(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN unsigned long categoryId,
		IN unsigned long count,
		IN unsigned long* pAttributeIds,
		IN OTChar* *pAttributeDescriptions,
		IN unsigned short* pAttributeDatatypes);

	static void API_CALL handleQueryAeConditions(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN unsigned long categoryId,
		IN unsigned long count,
		IN OTChar* *pConditionsNames);

	static void API_CALL handleQueryAeSubConditions(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTChar* conditionName,
		IN unsigned long count,
		IN OTChar* *pSubConditionsNames);

	static void API_CALL handleQueryAeSourceConditions(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTChar* sourcePath,
		IN unsigned long count,
		IN OTChar* *pConditionsNames);

	static void API_CALL handleGetAeConditionState(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTChar* sourcePath,
		IN OTChar* conditionName,
		IN unsigned long attributeCount,
		IN unsigned long* pAttributeIds,
		IN OTCAEConditionStateData* pConditionState);

	static void API_CALL handleEnableAeConditions(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN unsigned char enable,
		IN unsigned char areaOrSource,
		IN OTChar* path);

	static void API_CALL handleGetErrorString(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN long errorCode,
		IN OTChar* errorString);

	static void API_CALL handleLogon(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result,
		IN OTChar* userName,
		IN OTChar* password);

	static void API_CALL handleLogoff(
		IN unsigned long executionContext,
		IN OTCObjectData sessionData,
		IN long result);

	static int changeCondition(
		BOOL conditionAlreadyExists,
		AeSubscription* anAeSubscription,
		AeCondition* anAeCondition,
		AeCondition* newAeCondition);

}; //end class OTCGlobals
} //end namespace

Application::Application()
{
	::OTInitLib();
	m_isInit      = FALSE;
	m_versionOTB  = 447;
	m_updateAttributesBehavior = EnumUpdateAttributesBehavior_IMPLICIT_UPDATE;
	TraceOutput = NULL;
	//load the localeIds array
	m_localeIdToStringList.insert(LcidToStringPair(2048, _T("System Default")));
	m_localeIdToStringList.insert(LcidToStringPair(1024, _T("User Default")));
	m_localeIdToStringList.insert(LcidToStringPair(0, _T("Neutral")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0C09, _T("en-au")));
	m_localeIdToStringList.insert(LcidToStringPair(0x2809, _T("en-bz")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1009, _T("en-ca")));
	m_localeIdToStringList.insert(LcidToStringPair(0x2409, _T("en-cb")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1809, _T("en-ie")));
	m_localeIdToStringList.insert(LcidToStringPair(0x2009, _T("en-jm")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1409, _T("en-nz")));
	m_localeIdToStringList.insert(LcidToStringPair(0x3409, _T("en-ph")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1C09, _T("en-za")));
	m_localeIdToStringList.insert(LcidToStringPair(0x2C09, _T("en-tt")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0809, _T("en-gb")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0409, _T("en-us")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0407, _T("de-de")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0C07, _T("de-at")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1407, _T("de-li")));
	m_localeIdToStringList.insert(LcidToStringPair(0x1007, _T("de-lu")));
	m_localeIdToStringList.insert(LcidToStringPair(0x0807, _T("de-ch")));
	m_localeIdToStringList.insert(LcidToStringPair(0x041D, _T("sv-se")));
	m_localeIdToStringList.insert(LcidToStringPair(0x081D, _T("sv-fi")));
	m_authLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	m_impLevel = RPC_C_IMP_LEVEL_IDENTIFY;
	m_xmldaGetStatusInterval = 100000; // every 100 seconds
} // end constructor

Application::~Application()
{
	m_daSessionListJanitor.lock();
	m_daSessionList.clear();
	m_daSessionListJanitor.unlock();
	m_aeSessionListJanitor.lock();
	m_aeSessionList.clear();
	m_aeSessionListJanitor.unlock();
	m_localeIdToStringList.clear();
	::OTExitLib();
} // end destructor


long Application::activate(
	EnumFeature aProduct,
	tstring aKey)
{
	int res = (int) EnumResultCode_E_FAIL;
	tstring licenseName = _T("");
	
	res = OTActivate((unsigned char)aProduct, (OTChar*)aKey.c_str(), 0);
	EnumTraceLevel level = EnumTraceLevel_INF;

	if (FAILED(res))
	{
		level = EnumTraceLevel_ERR;
	}
	switch (aProduct)
	{
	case EnumFeature_DA_CLIENT:
		licenseName = _T("OPC DA Client");
		break;
	case EnumFeature_XMLDA_CLIENT:
		licenseName = _T("XMLDA Client");
		break;
	case EnumFeature_TP_CLIENT:
		licenseName = _T("Tunnel Client");
		break;
	case EnumFeature_AE_CLIENT:
		licenseName = _T("OPC AE Client");
		break;
	default:
		{
			TCHAR buffer[32];
			_stprintf(buffer, _T("unknown client feature - %u"), aProduct);
			licenseName = buffer;
		}
		break;
	}

	Application::Instance()->trace(
		level,
		EnumTraceGroup_CLIENT,
		_T("Application::Activate"),
		_T("Result of activating %s license: 0x%0X"), licenseName.c_str(), res);

	return res;
}   //  end activate


//-----------------------------------------------------------------------------
//	forceDemoVersion
//		- force the target application to run as demo
//
long Application::forceDemoVersion()
{
	return OTActivate(0, NULL, 0);
}   //  end forceDemoVersion


long Application::initialize()
{
	long result = E_FAIL;

	if (!m_isInit)
	{
		OTCInitData initData;
		memset(&initData, 0, sizeof(OTCInitData));
		initData.m_versionOTC = m_versionOTB;
		initData.m_serviceName = OTCGlobals::allocateOTBString(m_serviceName);
		initData.m_serviceDescription = OTCGlobals::allocateOTBString(m_serviceDescription);
		initData.m_authLevel = m_authLevel;
		initData.m_impLevel = m_impLevel;
		initData.m_xmldaGetStatusInterval = m_xmldaGetStatusInterval;

		result = OTCInitialize(&initData);

		if (SUCCEEDED(result))
		{
			m_isInit = TRUE;
			OTCGlobals::advise();
		}   //  end if

		if (initData.m_serviceName != NULL)
		{
			OTFreeMemory(initData.m_serviceName);
			OTFreeMemory(initData.m_serviceDescription);
			initData.m_serviceName = NULL;
			initData.m_serviceDescription = NULL;
		}   //  end if
	}   //  end if

	return result;
} // end initialize


//-----------------------------------------------------------------------------
//	processCommandLine
//
long Application::processCommandLine(tstring& aCommandLine)
{
	return OTCProcessCommandLine((OTChar*) aCommandLine.c_str());
}   //  end processCommandLine


long Application::addDaSession(const tstring& url, DaSession* aDaSession)
{
	long result;
	unsigned long sessionHandle = 0;
#ifdef TBC_OS_WINDOWS
	OTChar* anUrl = _tcsdup(url.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* anUrl = strdup(url.c_str());
#endif

	if (aDaSession != NULL)
	{
		result = OTCAddDASession(
					 anUrl,
					 aDaSession->getUserData(),
					 &sessionHandle);

		if (SUCCEEDED(result))
		{
			aDaSession->setHandle(sessionHandle);
			m_daSessionListJanitor.lock();
			m_daSessionList.insert(DaSessionPair(aDaSession->getUserData(), aDaSession));
			m_daSessionListJanitor.unlock();
		} // end if
	} //end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("Application::addDaSession"),
			_T(" The session argument is invalid! It cannot be null!"));
	} // end else

	free(anUrl);
	return result;
} // end AddDaSession

long Application::removeDaSession(DaSession* aDaSession)
{
	long result = E_FAIL;

	if (aDaSession != NULL)
	{
		m_daSessionListJanitor.lock();
		result = OTCRemoveDASession(aDaSession->getHandle());

		if (SUCCEEDED(result))
		{
			m_daSessionList.erase(aDaSession->getUserData());
		} //end if

		m_daSessionListJanitor.unlock();
	}//end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("Application::removeDaSession"),
			_T("The argument is invalid! The session cannot be null!"));
	} //end else

	return result;
} // end removeDaSession

long Application::addAeSession(const tstring& url, AeSession* anAeSession)
{
	long result;
	unsigned long sessionHandle = 0;
#ifdef TBC_OS_WINDOWS
	OTChar* otcUrl = _tcsdup(url.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcUrl = strdup(url.c_str());
#endif

	if (anAeSession != NULL)
	{
		result = OTCAddAESession(
					 otcUrl,
					 anAeSession->getUserData(),
					 &sessionHandle);

		if (SUCCEEDED(result))
		{
			anAeSession->setHandle(sessionHandle);
			m_aeSessionListJanitor.lock();
			m_aeSessionList.insert(AeSessionPair(anAeSession->getUserData(), anAeSession));
			m_aeSessionListJanitor.unlock();
		} // end if
	} // end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("Application::addAeSession"),
			_T("The session argument is invalid! It cannot be null!"));
	} // end else

	free(otcUrl);
	return result;
} // end AddDaSession

long Application::removeAeSession(AeSession* anAeSession)
{
	long result = E_FAIL;

	if (anAeSession != NULL)
	{
		m_aeSessionListJanitor.lock();
		result = OTCRemoveAESession(anAeSession->getHandle());

		if (SUCCEEDED(result))
		{
			m_aeSessionList.erase(anAeSession->getUserData());
		} //end if

		m_aeSessionListJanitor.unlock();
	}//end if
	else
	{
		result = EnumResultCode_E_INVALIDARG;
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("Application::removeAeSession"),
			_T("The argument is invalid! The session cannot be null!"));
	} //end else

	return result;
} // end removeDaSession

DaSession* Application::findDaSession(unsigned long position)
{
	m_daSessionListJanitor.lock();

	if (m_daSessionList.size() == 0)
	{
		m_daSessionListJanitor.unlock();
		return NULL;
	}   //  end if

	std::map<unsigned long, DaSession*>::iterator daSessionIterator;
	daSessionIterator = m_daSessionList.find(position);

	if (daSessionIterator != m_daSessionList.end())
	{
		m_daSessionListJanitor.unlock();
		return daSessionIterator->second;
	}   //  end if

	m_daSessionListJanitor.unlock();
	return NULL;
} //end findDaSession
AeSession* Application::findAeSession(unsigned long position)
{
	m_aeSessionListJanitor.lock();

	if (m_aeSessionList.size() == 0)
	{
		m_aeSessionListJanitor.unlock();
		return NULL;
	}   //  end if

	std::map<unsigned long, AeSession*>::iterator aeSessionIterator;
	aeSessionIterator = m_aeSessionList.find(position);

	if (aeSessionIterator != m_aeSessionList.end())
	{
		m_aeSessionListJanitor.unlock();
		return aeSessionIterator->second;
	}   //  end if

	m_aeSessionListJanitor.unlock();
	return NULL;
} //end findAeSession
long Application::terminate()
{
	long result = E_FAIL;
	result = OTCTerminate();

	if (m_isInit)
	{
		m_isInit = FALSE;
	} // end if

	if (OTCGlobals::m_isAdvised)
	{
		OTCGlobals::m_isAdvised = FALSE;
	}//end if

	//}// end if
	return result;
} // end terminate


void Application::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	const TCHAR* anObjectID,
	const TCHAR* aMessage,
	...)
{
	const unsigned long LENGTH = 400;
	TCHAR buffer[LENGTH + 1] = {0};
	va_list arglist;
	va_start(arglist, aMessage);
#ifdef TBC_OS_WINDOWS
	_vsntprintf(buffer, LENGTH, aMessage, arglist);
#endif
#ifdef TBC_OS_LINUX
	vsnprintf(buffer, LENGTH, aMessage, arglist);
#endif
	tstring anObjectIDStr(anObjectID);
	tstring bufferStr(buffer);
	Trace::writeline((unsigned char)aLevel, (unsigned long)aMask, anObjectIDStr, bufferStr);
}   // end Trace


void Application::enableTracing(
	EnumTraceGroup errorLevelMask,
	EnumTraceGroup warningLevelMask,
	EnumTraceGroup infoLevelMask,
	EnumTraceGroup debugLevelMask,
	tstring fileName,
	unsigned long fileMaxSize,
	unsigned long maxBackups)
{
	Trace::setErrorLevelMask(errorLevelMask);
	Trace::setWarningLevelMask(warningLevelMask);
	Trace::setDebugLevelMask(debugLevelMask);
	Trace::setInfoLevelMask(infoLevelMask);

	//  Note the Trace::setEnableTraceToFile() must be the very last setting of the Trace
	//  this will trigger the configuration definition within the core
	if (fileName.empty())
	{
		Trace::setEnableTraceToFile(false);
	}
	else
	{
		Trace::setFileName(fileName);
		Trace::setMaxBackups(maxBackups);
		Trace::setFileMaxSize(fileMaxSize);
		Trace::setEnableTraceToFile(true);
	}   //  end if ... else
}   // end enableTracing

//-----------------------------------------------------------------
// OTCGlobals class
//-----------------------------------------------------------------
OTCCallbackFunctions OTCGlobals::m_OTNCCallbacks;
BOOL OTCGlobals::m_isAdvised = FALSE;
void OTCGlobals::handleTrace(
	OTChar* aTraceString,
	unsigned short aTraceLevel,
	unsigned long aTraceGroup,
	OTChar* anObjectId,
	OTChar* aText)
{
	if (Application::Instance()->TraceOutput != NULL)
	{
		Application::Instance()->TraceOutput(
			aTraceString,
			aTraceLevel,
			aTraceGroup,
			anObjectId,
			aText);
	} // end if
} // end handleTrace


void OTCGlobals::handleStateChange(
	IN OTCObjectContext objectContext,
	IN unsigned char state)
{
	DaSession* daSession = NULL;
	DaSubscription* daSubscription = NULL;
	AeSession* aeSession = NULL;
	AeSubscription* aeSubscription = NULL;
	DaItem* daItem;

	switch (objectContext.m_objectType)
	{
	case EnumObjectType_DASESSION:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_objectData.m_userData);

		if (daSession != NULL)
		{
			daSession->onStateChange((EnumObjectState)state);
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;

	case EnumObjectType_AESESSION:
		Application::Instance()->m_aeSessionListJanitor.lock();
		aeSession = Application::Instance()->findAeSession(objectContext.m_objectData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->onStateChange((EnumObjectState)state);
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
		break;

	case EnumObjectType_DASUBSCRIPTION:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->m_daSubscriptionListJanitor.lock();
			daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

			if (daSubscription != NULL)
			{
				daSubscription->onStateChange((EnumObjectState)state);
			} //end if

			daSession->m_daSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;

	case EnumObjectType_AESUBSCRIPTION:
		Application::Instance()->m_aeSessionListJanitor.lock();
		aeSession = Application::Instance()->findAeSession(objectContext.m_sessionData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->m_aeSubscriptionListJanitor.lock();
			aeSubscription = aeSession->findSubscription(objectContext.m_objectData.m_userData);

			if (aeSubscription != NULL)
			{
				aeSubscription->onStateChange((EnumObjectState)state);
			} //end if

			aeSession->m_aeSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
		break;

	case EnumObjectType_DAITEM:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->m_daSubscriptionListJanitor.lock();
			daSubscription = daSession->findSubscription(objectContext.m_subscriptionData.m_userData);

			if (daSubscription != NULL)
			{
				daSubscription->m_itemListJanitor.lock();
				daItem = daSubscription->findItem(objectContext.m_objectData.m_userData);

				if (daItem != NULL)
				{
					daItem->onStateChange((EnumObjectState)state);
				} //end if

				daSubscription->m_itemListJanitor.unlock();
			} //end if

			daSession->m_daSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;
	} //end switch
} // end handleStateChange

void OTCGlobals::handlePerformStateTransition(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result)
{
	DaSession* daSession = NULL;
	DaSubscription* daSubscription = NULL;
	AeSession* aeSession = NULL;
	AeSubscription* aeSubscription = NULL;
	DaItem* daItem = NULL;

	switch (objectContext.m_objectType)
	{
	case EnumObjectType_DASESSION:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_objectData.m_userData);

		if (daSession != NULL)
		{
			daSession->onPerformStateTransition(
				executionContext,
				result);
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;

	case EnumObjectType_AESESSION:
		Application::Instance()->m_aeSessionListJanitor.lock();
		aeSession = Application::Instance()->findAeSession(objectContext.m_objectData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->onPerformStateTransition(
				executionContext,
				result);
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
		break;

	case EnumObjectType_DASUBSCRIPTION:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->m_daSubscriptionListJanitor.lock();
			daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

			if (daSubscription != NULL)
			{
				daSubscription->onPerformStateTransition(
					executionContext,
					result);
			} //end if

			daSession->m_daSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;

	case EnumObjectType_AESUBSCRIPTION:
		Application::Instance()->m_aeSessionListJanitor.lock();
		aeSession = Application::Instance()->findAeSession(objectContext.m_sessionData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->m_aeSubscriptionListJanitor.lock();
			aeSubscription = aeSession->findSubscription(objectContext.m_objectData.m_userData);

			if (aeSubscription != NULL)
			{
				aeSubscription->onPerformStateTransition(
					executionContext,
					result);
			} //end if

			aeSession->m_aeSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
		break;

	case EnumObjectType_DAITEM:
		Application::Instance()->m_daSessionListJanitor.lock();
		daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->m_daSubscriptionListJanitor.lock();
			daSubscription = daSession->findSubscription(objectContext.m_subscriptionData.m_userData);

			if (daSubscription != NULL)
			{
				daSubscription->m_itemListJanitor.lock();
				daItem = daSubscription->findItem(objectContext.m_objectData.m_userData);

				if (daItem != NULL)
				{
					daItem->onPerformStateTransition(
						executionContext,
						result);
				} //end if

				daSubscription->m_itemListJanitor.unlock();
			} //end if

			daSession->m_daSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		break;
	} //end switch
} // end handlePerformStateTransition


void OTCGlobals::handleDataChange(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN unsigned long count,
	IN OTCObjectData* pItemData,
	IN OTCValueData* pValues,
	IN long* pResults)
{
	unsigned long i = 0;
	unsigned long correctCount = 0;
	std::vector<DaItem*> items(count, NULL);
	std::vector<ValueQT*> values(count, NULL);
	std::vector<long> results(count, E_FAIL);
	DaItem* pItem = NULL;

	if (objectContext.m_objectType == EnumObjectType_DASUBSCRIPTION)
	{
		Application::Instance()->m_daSessionListJanitor.lock();
		DaSession* daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->m_daSubscriptionListJanitor.lock();
			DaSubscription* daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

			if (daSubscription != NULL)
			{
				daSubscription->m_itemListJanitor.lock();
				if (pItemData)
				{
					for (i = 0; i < count; i++)
					{
						pItem = daSubscription->findItem(pItemData[i].m_userData);
						if (pItem != NULL)
						{
							items[correctCount] = pItem;

							if (pValues)
							{
								values[correctCount] = new ValueQT(
									*(pValues[i].m_pValue),
									(EnumQuality)pValues[i].m_quality,
									DateTime(&(pValues[i].m_timestamp)));
							}

							if (pResults)
							{
								results[correctCount] = pResults[i];
							}

							++correctCount;
						}
					}   //  end for
				}   //  end if

				items.resize(correctCount);
				values.resize(correctCount);
				results.resize(correctCount);
				daSubscription->onDataChange(executionContext, items, values, results);
				daSubscription->m_itemListJanitor.unlock();
			}   //  end if

			daSession->m_daSubscriptionListJanitor.unlock();
		}   //  end if

		Application::Instance()->m_daSessionListJanitor.unlock();
	}   //  end if

	std::vector<ValueQT*>::iterator valueQTIterator;

	for (valueQTIterator = values.begin(); valueQTIterator != values.end(); valueQTIterator++)
	{
		if (*valueQTIterator != NULL)
		{
			delete(*valueQTIterator);
		} //end if
	} //end for
} // end handleDataChange

void OTCGlobals::handleReadComplete(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned long count,
	IN OTCObjectData* pItemData,
	IN OTChar** pItemIDs,
	IN OTChar** pItemPaths,
	IN OTCValueData* pValues,
	IN long* pResults)
{
	if (count > 0)
	{
		unsigned long i;
		std::vector<tstring> itemIds(count, _T(""));
		std::vector<tstring> itemPaths(count, _T(""));
		std::vector<ValueQT*> values(count, NULL);
		std::vector<long> results(count, E_FAIL);
		std::vector<DaItem*> items(count, NULL);
		unsigned long correctCount = 0;

		DaSession* daSession = NULL;
		DaSubscription* daSubscription = NULL;
		DaItem* daItem = NULL;

		switch (objectContext.m_objectType)
		{
		case EnumObjectType_DASESSION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_objectData.m_userData);

			if (daSession != NULL)
			{
				for (i = 0; i < count; i++)
				{
					if (pValues)
					{
						values[i] = new ValueQT(
							*(pValues[i].m_pValue),
							(EnumQuality)pValues[i].m_quality,
							DateTime(&(pValues[i].m_timestamp)));
					}

					if (pResults)
					{
						results[i] = pResults[i];
					} //end if

					if (pItemIDs)
					{
						itemIds[i] = pItemIDs[i];
					} //end if

					if (pItemPaths)
					{
						itemPaths[i] = pItemPaths[i];
					} //end if
				} //end for
				
				daSession->onReadComplete(
					executionContext,
					itemIds,
					itemPaths,
					values,
					results,
					result);
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;

		case EnumObjectType_DASUBSCRIPTION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

			if (daSession != NULL)
			{
				daSession->m_daSubscriptionListJanitor.lock();
				daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

				if (daSubscription != NULL)
				{
					daSubscription->m_itemListJanitor.lock();
					if (pItemData)
					{
						for (i = 0; i < count; i++)
						{
							daItem = daSubscription->findItem(pItemData[i].m_userData);

							if (daItem)
							{
								items[correctCount] = daItem;

								if (pValues)
								{
									values[correctCount] = new ValueQT(
										*(pValues[i].m_pValue),
										(EnumQuality)pValues[i].m_quality,
										DateTime(&(pValues[i].m_timestamp)));
								} //end if

								if (pResults)
								{
									results[correctCount] = pResults[i];
								} //end if

								++correctCount;
							} //end if
						} //end for
					}//end if

					items.resize(correctCount);
					values.resize(correctCount);
					results.resize(correctCount);
					daSubscription->onReadComplete(
						executionContext,
						items,
						values,
						results,
						result);
					daSubscription->m_itemListJanitor.unlock();
				} //end if

				daSession->m_daSubscriptionListJanitor.unlock();
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;
		} //end switch

		std::vector<ValueQT*>::iterator valueQTIterator;

		for (valueQTIterator = values.begin(); valueQTIterator != values.end(); valueQTIterator++)
		{
			if (*valueQTIterator != NULL)
			{
				delete(*valueQTIterator);
			} //end if
		} //end for
	} //end if
} //end handleReadComplete

void OTCGlobals::handleWriteComplete(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned long count,
	IN OTCObjectData* pItemData,
	IN OTChar** pItemIDs,
	IN OTChar** pItemPaths,
	IN OTCValueData* pValues,
	IN long* pResults)
{
	if (count > 0)
	{
		unsigned long i;
		unsigned long correctCount = 0;
		std::vector<tstring> itemIds(count, _T(""));
		std::vector<tstring> itemPaths(count, _T(""));
		std::vector<ValueQT*> values(count, NULL);
		std::vector<long> results(count, E_FAIL);
		std::vector<DaItem*> items(count, NULL);

		DaSession* daSession = NULL;
		DaSubscription* daSubscription = NULL;
		DaItem* daItem = NULL;

		switch (objectContext.m_objectType)
		{
		case EnumObjectType_DASESSION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_objectData.m_userData);

			for (i = 0; i < count; i++)
			{
				if (pValues)
				{
					values[i] = new ValueQT(*(pValues[i].m_pValue),
						(EnumQuality)pValues[i].m_quality,
						DateTime(&(pValues[i].m_timestamp)));
				} //end if

				if (pResults)
				{
					results[i] = pResults[i];
				} //end if

				if (pItemIDs)
				{
					itemIds[i] = pItemIDs[i];
				} //end if

				if (pItemPaths)
				{
					itemPaths[i] = pItemPaths[i];
				} //end if
			} //end for
			if (daSession != NULL)
			{
				daSession->onWriteComplete(
					executionContext,
					itemIds,
					itemPaths,
					values,
					results,
					result);
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;

		case EnumObjectType_DASUBSCRIPTION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

			if (daSession != NULL)
			{
				daSession->m_daSubscriptionListJanitor.lock();
				daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

				if (daSubscription != NULL)
				{
					daSubscription->m_itemListJanitor.lock();
					if (pItemData)
					{
						for (i = 0; i < count; i++)
						{
							daItem = daSubscription->findItem(pItemData[i].m_userData);
							if (daItem)
							{
								items[correctCount] = daItem;

								if (pValues)
								{
									values[correctCount] = new ValueQT(*(pValues[i].m_pValue),
										(EnumQuality)pValues[i].m_quality,
										DateTime(&(pValues[i].m_timestamp)));
								} //end if

								if (pResults)
								{
									results[correctCount] = pResults[i];
								} //end if

								++correctCount;
							} //end if
						} //end for
					}//end if

					items.resize(correctCount);
					values.resize(correctCount);
					results.resize(correctCount);
					daSubscription->onWriteComplete(
						executionContext,
						items,
						values,
						results,
						result);
					daSubscription->m_itemListJanitor.unlock();
				} //end if

				daSession->m_daSubscriptionListJanitor.unlock();
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;
		} //end switch

		std::vector<ValueQT*>::iterator valueQTIterator;

		for (valueQTIterator = values.begin(); valueQTIterator != values.end(); valueQTIterator++)
		{
			if (*valueQTIterator != NULL)
			{
				delete(*valueQTIterator);
			} //end if
		} //end for
	} //end if
} // end handleWriteComplete


unsigned char OTCGlobals::handleServerShutdown(
	IN OTCObjectData sessionData,
	IN OTChar* reason)
{
	Application::Instance()->m_daSessionListJanitor.lock();
	DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

	if (daSession != NULL)
	{
		unsigned char shutdownResult = daSession->onShutdown(tstring(reason));
		Application::Instance()->m_daSessionListJanitor.unlock();
		return shutdownResult;
	} //end if

	Application::Instance()->m_daSessionListJanitor.unlock();
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

	if (aeSession != NULL)
	{
		unsigned char shutdownResult = aeSession->onShutdown(tstring(reason));
		Application::Instance()->m_aeSessionListJanitor.unlock();
		return shutdownResult;
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
	return 0;
} // end handleServerShutdown


void OTCGlobals::handleUpdateAttributes(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned char fromServer,
	IN unsigned long attributesCount,
	IN unsigned long* pWhatAttributes,
	IN long* pResults)
{
	if (attributesCount > 0)
	{
		std::vector<EnumObjectAttribute> whatAttributes(attributesCount, (EnumObjectAttribute)0);
		std::vector<long> results(attributesCount, E_FAIL);
		unsigned long i;

		if (pWhatAttributes)
		{
			for (i = 0; i < attributesCount; i++)
			{
				whatAttributes[i] = (EnumObjectAttribute)pWhatAttributes[i];
			} //end for
		} //end if

		if (pResults)
		{
			for (i = 0; i < attributesCount; i++)
			{
				results[i] = pResults[i];
			} //end for
		} //end if

		DaSession* daSession = NULL;
		DaSubscription* daSubscription = NULL;

		switch (objectContext.m_objectType)
		{
		case EnumObjectType_DASESSION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_objectData.m_userData);

			if (daSession != NULL)
			{
				if (fromServer == 0)
				{
					daSession->onSetAttributesToServer(
						executionContext,
						whatAttributes,
						results,
						result);
				}
				else
				{
					daSession->onGetAttributesFromServer(
						executionContext,
						whatAttributes,
						results,
						result);
				} //end if ... else
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;

		case EnumObjectType_DASUBSCRIPTION:
			Application::Instance()->m_daSessionListJanitor.lock();
			daSession = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

			if (daSession != NULL)
			{
				daSession->m_daSubscriptionListJanitor.lock();
				daSubscription = daSession->findSubscription(objectContext.m_objectData.m_userData);

				if (daSubscription != NULL)
				{
					if (fromServer == 0)
					{
						daSubscription->onSetAttributesToServer(
							executionContext,
							whatAttributes,
							results,
							result);
					}
					else
					{
						daSubscription->onGetAttributesFromServer(
							executionContext,
							whatAttributes,
							results,
							result);
					} //end if ... else
				} //end if

				daSession->m_daSubscriptionListJanitor.unlock();
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
			break;
		} //end switch
	} //end if
} // end handleUpdateAttribute

void OTCGlobals::handleUpdateDaItemAttributes(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned long itemCount,
	IN OTCObjectData* pItemData,
	IN unsigned char fromServer,
	IN unsigned long attributesCount,
	IN unsigned long* pWhatAttributes,
	IN long* pResults)
{
	if ((attributesCount > 0) && (itemCount > 0))
	{
		std::vector<DaItem*> items(itemCount, NULL);
		std::vector<EnumObjectAttribute> whatAttributes(attributesCount, (EnumObjectAttribute)0);
		std::vector<long> results(attributesCount, -1);
		unsigned long i;
		unsigned long correctCount = 0;

		if (objectContext.m_objectType == EnumObjectType_DASUBSCRIPTION)
		{
			Application::Instance()->m_daSessionListJanitor.lock();
			DaSession* session = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

			if (session != NULL)
			{
				session->m_daSubscriptionListJanitor.lock();
				DaSubscription* daSubscription = session->findSubscription(objectContext.m_objectData.m_userData);

				if (daSubscription != NULL)
				{
					daSubscription->m_itemListJanitor.lock();
					if (pItemData)
					{
						DaItem* daItem = NULL;

						for (i = 0; i < itemCount; i++)
						{
							daItem = daSubscription->findItem(pItemData[i].m_userData);

							if (daItem)
							{
								items[correctCount] = daItem;

								if (pWhatAttributes)
								{
									whatAttributes[correctCount] = (EnumObjectAttribute)pWhatAttributes[i];
								} //end if

								if (pResults)
								{
									results[correctCount] = pResults[i];
								} //end if

								++correctCount;
							} //end if
						} //end for
					}//end if

					items.resize(correctCount);
					whatAttributes.resize(correctCount);
					results.resize(correctCount);

					if (fromServer == 0)
					{
						daSubscription->onSetDaItemAttributesToServer(
							executionContext,
							items,
							whatAttributes,
							results,
							result);
					}
					else
					{
						daSubscription->onGetDaItemAttributesFromServer(
							executionContext,
							items,
							whatAttributes,
							results,
							result);
					} //end if ... else

					daSubscription->m_itemListJanitor.unlock();
				} //end if

				session->m_daSubscriptionListJanitor.unlock();
			} //end if

			Application::Instance()->m_daSessionListJanitor.unlock();
		} //end if
	} //end if
} // end handleUpdateDaItemAttribute

void OTCGlobals::handleValidateDaItems(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned long itemCount,
	IN OTCObjectData* pItemData,
	IN long* pResults)
{
	if (itemCount > 0)
	{
		std::vector<DaItem*> items(itemCount, NULL);
		std::vector<long> results(itemCount, 0);
		unsigned long i;
		unsigned long correctCount = 0;

		Application::Instance()->m_daSessionListJanitor.lock();
		DaSession* session = Application::Instance()->findDaSession(objectContext.m_sessionData.m_userData);

		if (session != NULL)
		{
			session->m_daSubscriptionListJanitor.lock();
			DaSubscription* subscription = session->findSubscription(objectContext.m_objectData.m_userData);

			if (subscription != NULL)
			{
				subscription->m_itemListJanitor.lock();
				if (pItemData)
				{
					DaItem* daItem = NULL;
					for (i = 0; i < itemCount; i++)
					{
						daItem = subscription->findItem(pItemData[i].m_userData);

						if (daItem)
						{
							items[correctCount] = daItem;

							if (pResults)
							{
								results[correctCount] = pResults[i];
							} //end if

							++correctCount;
						}
					} //end for
				}//end if

				items.resize(correctCount);
				results.resize(correctCount);

				subscription->onValidateDaItems(
					executionContext,
					items,
					results);
				subscription->m_itemListJanitor.unlock();
			} //end if

			session->m_daSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
	} //end if
} // end handleValidateDaItems


void OTCGlobals::handleGetServerStatus(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTCServerStatus* pServerStatus)
{
	std::vector<tstring> lcIds(pServerStatus->m_countLCIDs, _T(""));
	std::map<unsigned long, tstring>::iterator localeIdIterator;
	tstring vendorInfo;
	tstring statusInfo;
	tstring productVersion;

	if (pServerStatus->m_statusInfo != NULL)
	{
		statusInfo = pServerStatus->m_statusInfo;
	}
	else
	{
		statusInfo = _T("(null)");
	}//end if...else

	if (pServerStatus->m_vendorInfo != NULL)
	{
		vendorInfo = pServerStatus->m_vendorInfo;
	}
	else
	{
		vendorInfo = _T("(null)");
	}//end if...else

	if (pServerStatus->m_productVersion != NULL)
	{
		productVersion = pServerStatus->m_productVersion;
	}
	else
	{
		productVersion = _T("(null)");
	}//end if...else

	if (pServerStatus)
	{
		for (unsigned long i = 0; i < pServerStatus->m_countLCIDs; i++)
		{
			localeIdIterator = Application::Instance()->m_localeIdToStringList.find(pServerStatus->m_supportedLCIDs[i]);

			if (localeIdIterator != Application::Instance()->m_localeIdToStringList.end())
			{
				lcIds[i] = localeIdIterator->second;
			}//end if
		} //end for

		ServerStatus serverStatus(
			pServerStatus->m_state,
			DateTime(&pServerStatus->m_startTime),
			DateTime(&pServerStatus->m_currentTime),
			vendorInfo,
			productVersion,
			lcIds,
			DateTime(&pServerStatus->m_lastUpdateTime),
			pServerStatus->m_groupCount,
			pServerStatus->m_bandwidth,
			statusInfo);
		Application::Instance()->m_daSessionListJanitor.lock();
		DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->onGetServerStatus(
				executionContext,
				serverStatus,
				result);
			Application::Instance()->m_daSessionListJanitor.unlock();
			return;
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		Application::Instance()->m_aeSessionListJanitor.lock();
		AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->onGetServerStatus(
				executionContext,
				serverStatus,
				result);
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
	}
	else
	{
		ServerStatus serverStatus;
		Application::Instance()->m_daSessionListJanitor.lock();
		DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

		if (daSession != NULL)
		{
			daSession->onGetServerStatus(
				executionContext,
				serverStatus,
				result);
			Application::Instance()->m_daSessionListJanitor.unlock();
			return;
		} //end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		Application::Instance()->m_aeSessionListJanitor.lock();
		AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->onGetServerStatus(
				executionContext,
				serverStatus,
				result);
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
	}//end if...else
} //end handleGetServerStatus

void OTCGlobals::handleBrowseServers(
	IN unsigned long executionContext,
	IN long result,
	IN OTChar* ipAddress,
	IN unsigned char whatOPCSpec,
	IN unsigned char whatServerData,
	IN unsigned long serverDataCount,
	IN OTCServerData* pServerData)
{
	if (serverDataCount > 0)
	{
		unsigned long i;
		std::vector<ServerBrowserData*> serverData(serverDataCount, NULL);

		for (i = 0; i < serverDataCount; i++)
		{
			serverData[i] = new ServerBrowserData(
				pServerData[i].m_clsid,
				pServerData[i].m_progId,
				pServerData[i].m_description,
				pServerData[i].m_vProgId,
				(EnumOPCSpecification)pServerData[i].m_opcSpecifications,
				pServerData[i].m_url);
		} //end for

		std::map<unsigned long, ServerBrowser*>::const_iterator serverBrowserIterator;
		serverBrowserIterator = (Application::Instance()->getServerBrowserList()).find(executionContext);

		if (serverBrowserIterator != (Application::Instance()->getServerBrowserList()).end())
		{
			tstring ipAddressString = tstring(ipAddress);
			(serverBrowserIterator->second)->onBrowseServers(
				executionContext,
				ipAddressString,
				serverData,
				result);
		} //end if

		std::vector<ServerBrowserData*>::iterator serverDataIterator;

		for (serverDataIterator = serverData.begin(); serverDataIterator != serverData.end(); serverDataIterator++)
		{
			if (*serverDataIterator != NULL)
			{
				delete(*serverDataIterator);
			} //end if
		} //end for
	} //end if
} // end handleBrowseServers

void OTCGlobals::handleBrowseAddressSpace(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTCObjectHandle addressSpaceElementHandle,
	IN OTChar* addressSpaceElementID,
	IN OTChar* addressSpaceElementPath,
	IN OTCAddressSpaceBrowseOptions* pBrowseOptions,
	IN unsigned long addressSpaceElementDataCount,
	IN OTCAddressSpaceElementData* pAddressSpaceElements)
{
	if (addressSpaceElementDataCount > 0)
	{
		unsigned long i = 0;
		Application::Instance()->m_daSessionListJanitor.lock();
		DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

		if (daSession != NULL)
		{
			std::vector<DaAddressSpaceElement*> daAddressSpaceElements(addressSpaceElementDataCount, NULL);

			for (i = 0; i < addressSpaceElementDataCount; i++)
			{
				daAddressSpaceElements[i] = new DaAddressSpaceElement(
					(EnumAddressSpaceElementType)pAddressSpaceElements[i].m_type,
					pAddressSpaceElements[i].m_name,
					pAddressSpaceElements[i].m_itemID,
					pAddressSpaceElements[i].m_objectHandle,
					pAddressSpaceElements[i].m_itemPath,
					daSession);
			}   //  end for

			tstring addressSpaceElementIDStr = tstring(addressSpaceElementID);
			tstring addressSpaceElementPathStr = tstring(addressSpaceElementPath);
			daSession->onBrowse(
				executionContext,
				(unsigned long)addressSpaceElementHandle,
				addressSpaceElementIDStr,
				addressSpaceElementPathStr,
				daAddressSpaceElements,
				result);
			std::vector<DaAddressSpaceElement*>::iterator daAddressSpaceElementsIterator;

			for (daAddressSpaceElementsIterator = daAddressSpaceElements.begin(); daAddressSpaceElementsIterator != daAddressSpaceElements.end(); daAddressSpaceElementsIterator++)
			{
				if (*daAddressSpaceElementsIterator != NULL)
				{
					delete(*daAddressSpaceElementsIterator);
				}   //  end if
			}   //  end for

			Application::Instance()->m_daSessionListJanitor.unlock();
			return;
		}   //  end if

		Application::Instance()->m_daSessionListJanitor.unlock();
		Application::Instance()->m_aeSessionListJanitor.lock();
		AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

		if (aeSession != NULL)
		{
			std::vector<AddressSpaceElement*> addressSpaceElements(addressSpaceElementDataCount, NULL);

			for (i = 0; i < addressSpaceElementDataCount; i++)
			{
				addressSpaceElements[i] = new AddressSpaceElement(
					(EnumAddressSpaceElementType)pAddressSpaceElements[i].m_type,
					pAddressSpaceElements[i].m_name,
					pAddressSpaceElements[i].m_itemID,
					pAddressSpaceElements[i].m_objectHandle);
			}   //  end for

			aeSession->onBrowse(
				executionContext,
				(unsigned long)addressSpaceElementHandle,
				addressSpaceElements,
				result);
			std::vector<AddressSpaceElement*>::iterator addressSpaceElementsIterator;

			for (addressSpaceElementsIterator = addressSpaceElements.begin(); addressSpaceElementsIterator != addressSpaceElements.end(); addressSpaceElementsIterator++)
			{
				if (*addressSpaceElementsIterator != NULL)
				{
					delete(*addressSpaceElementsIterator);
				}   //  end if
			}   //  end for
		}   //  end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
	}   //  end if
} // end handleBrowseAddressSpace

void OTCGlobals::handleGetDaProperties(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTCObjectHandle addressSpaceElementHandle,
	IN OTChar* addressSpaceElementID,
	IN OTChar* addressSpaceElementPath,
	IN OTCGetDAPropertiesOptions* pGetPropertiesOptions,
	IN unsigned long propertyDataCount,
	IN OTCDAPropertyData* pPropertyData)
{
	std::vector<DaProperty*> someDaProperties(propertyDataCount, NULL);
	unsigned long i;

	for (i = 0; i < propertyDataCount; i++)
	{
		ValueQT* aValue = new ValueQT(
			*(pPropertyData[i].m_value.m_pValue),
			(EnumQuality)pPropertyData[i].m_value.m_quality,
			DateTime(&pPropertyData[i].m_value.m_timestamp));
		someDaProperties[i] = new DaProperty(
			pPropertyData[i].m_ID,
			pPropertyData[i].m_name,
			pPropertyData[i].m_itemID,
			pPropertyData[i].m_itemPath,
			pPropertyData[i].m_description,
			pPropertyData[i].m_datatype,
			aValue,
			pPropertyData[i].m_result);
	} //end for

	Application::Instance()->m_daSessionListJanitor.lock();
	DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

	if (daSession != NULL)
	{
		tstring addressSpaceElementIDStr = tstring(addressSpaceElementID);
		tstring addressSpaceElementPathStr = tstring(addressSpaceElementPath);
		daSession->onGetDaProperties(
			executionContext,
			(unsigned long)addressSpaceElementHandle,
			addressSpaceElementIDStr,
			addressSpaceElementPathStr,
			someDaProperties,
			result);
	} //end if

	Application::Instance()->m_daSessionListJanitor.unlock();
	std::vector<DaProperty*>::iterator daPropertyIterator;

	for (daPropertyIterator = someDaProperties.begin(); daPropertyIterator != someDaProperties.end(); daPropertyIterator++)
	{
		if (*daPropertyIterator != NULL)
		{
			delete(*daPropertyIterator);
		} //end if
	} //end for
} // end handleGetDaProperties

int OTCGlobals::changeCondition(
	BOOL conditionAlreadyExists,
	AeSubscription* anAeSubscription,
	AeCondition* anAeCondition,
	AeCondition* newAeCondition)
{
	if (!conditionAlreadyExists)
	{
		if (((newAeCondition->getState() & EnumConditionState_ACTIVE) != EnumConditionState_ACTIVE)
			&&
			((newAeCondition->getState() & EnumConditionState_ACKED) == EnumConditionState_ACKED))
		{
			return -1;
		}//end if

		//new condition
		anAeCondition->setAckRequired(newAeCondition->getAckRequired());
		anAeCondition->setCategory(newAeCondition->getCategory());
		anAeCondition->setAttributes(newAeCondition->getAttributes());
		anAeCondition->setChangeMask(newAeCondition->getChangeMask());
		anAeCondition->setQuality(newAeCondition->getQuality());
		anAeCondition->setEventType(newAeCondition->getEventType());
		anAeCondition->setState(newAeCondition->getState());
		anAeCondition->setSourcePath(newAeCondition->getSourcePath());
		anAeCondition->setConditionName(newAeCondition->getConditionName());
		anAeCondition->setSubConditionName(newAeCondition->getSubConditionName());
		anAeCondition->setOccurenceTime(newAeCondition->getOcurrenceTime());
		anAeCondition->setSeverity(newAeCondition->getSeverity());
		anAeCondition->setMessage(newAeCondition->getMessage());
		anAeCondition->setActiveTime(newAeCondition->getActiveTime());
		anAeCondition->setCookie(newAeCondition->getCookie());
		anAeCondition->setActorId(newAeCondition->getActorId());
		anAeCondition->setAeSession(newAeCondition->getAeSession());
		anAeCondition->setAeSubscription(anAeSubscription);
		//add condition to conditionList
		anAeSubscription->m_conditionListJanitor.lock();
		(anAeSubscription->getAeConditionList()).push_back(anAeCondition);
		anAeSubscription->onAeConditionsChanged(anAeSubscription->getAeConditionList());
		anAeSubscription->m_conditionListJanitor.unlock();
	} //end if
	else
	{
		if (((newAeCondition->getState() & EnumConditionState_ACTIVE) != EnumConditionState_ACTIVE)
			&&
			((newAeCondition->getState() & EnumConditionState_ACKED) == EnumConditionState_ACKED))
		{
			std::vector<AeCondition*>::iterator aeConditionIterator;
			anAeSubscription->m_conditionListJanitor.lock();
			aeConditionIterator = anAeSubscription->getAeConditionList().begin();

			while (anAeCondition != *aeConditionIterator)
			{
				aeConditionIterator++;
			} //end while

			AeCondition* position = *aeConditionIterator;
			anAeSubscription->getAeConditionList().erase(aeConditionIterator);
			delete position;
			anAeSubscription->onAeConditionsChanged(anAeSubscription->getAeConditionList());
			anAeSubscription->m_conditionListJanitor.unlock();
		} //end if
		else
		{
			//set the changed values
			//new condition
			anAeCondition->setAckRequired(newAeCondition->getAckRequired());
			anAeCondition->setCategory(newAeCondition->getCategory());
			anAeCondition->setAttributes(newAeCondition->getAttributes());
			anAeCondition->setChangeMask(newAeCondition->getChangeMask());
			anAeCondition->setQuality(newAeCondition->getQuality());
			anAeCondition->setEventType(newAeCondition->getEventType());
			anAeCondition->setState(newAeCondition->getState());
			anAeCondition->setSubConditionName(newAeCondition->getSubConditionName());
			anAeCondition->setOccurenceTime(newAeCondition->getOcurrenceTime());
			anAeCondition->setSeverity(newAeCondition->getSeverity());
			anAeCondition->setMessage(newAeCondition->getMessage());
			anAeCondition->setActiveTime(newAeCondition->getActiveTime());
			anAeCondition->setCookie(newAeCondition->getCookie());
			anAeCondition->setActorId(newAeCondition->getActorId());
			anAeCondition->setAeSession(newAeCondition->getAeSession());
			anAeCondition->setAeSubscription(anAeSubscription);
			anAeSubscription->m_conditionListJanitor.lock();
			anAeSubscription->onAeConditionsChanged(anAeSubscription->getAeConditionList());
			anAeSubscription->m_conditionListJanitor.unlock();
		} //end else
	} //end else

	return 0;
} //end changeCondition
void OTCGlobals::handleAeEventsReceived(
	IN OTCObjectContext objectContext,
	IN unsigned long count,
	IN OTCEventData* pEvents,
	IN unsigned char refresh,
	IN unsigned char lastRefresh)
{
	if (count > 0)
	{
		AeSubscription* anAeSubscription;
		std::vector<AeEvent*> events(count, NULL);
		unsigned long i, j;

		if (objectContext.m_objectType == EnumObjectType_AESUBSCRIPTION)
		{
			Application::Instance()->m_aeSessionListJanitor.lock();
			AeSession* aeSession = Application::Instance()->findAeSession(objectContext.m_sessionData.m_userData);

			if (aeSession != NULL)
			{
				aeSession->m_aeSubscriptionListJanitor.lock();
				AeSubscription* aeSubscription = aeSession->findSubscription(objectContext.m_objectData.m_userData);

				if (aeSubscription != NULL)
				{
					anAeSubscription = aeSubscription;

					if (pEvents)
					{
						for (i = 0; i < count; i++)
						{
							BOOL ackRequired = (pEvents[i].m_ackRequired == 1);
							unsigned long attributesCount = pEvents[i].m_eventAttrCount;
							std::vector<Variant*> attributes(attributesCount, NULL);

							for (j = 0; j < attributesCount; j++)
							{
								attributes[j] = new Variant(pEvents[i].m_pEventAttrs[j]);
							} //end for

							if (((pEvents[i].m_eventType) & EnumEventType_CONDITION) != EnumEventType_CONDITION)
							{
								events[i] = new AeEvent(
									(EnumEventType)pEvents[i].m_eventType,
									pEvents[i].m_eventCategory,
									pEvents[i].m_severity,
									pEvents[i].m_sourcePath,
									pEvents[i].m_message,
									DateTime(&pEvents[i].m_occurenceTime),
									attributes,
									pEvents[i].m_actorID);
							} //end if
							else
							{
								//maintains the subscription's conditions list
								//the conditions list contains all active and/or not acknowledged condition-related events
								events[i] = new AeCondition(
									(EnumEventType)pEvents[i].m_eventType,
									pEvents[i].m_eventCategory,
									pEvents[i].m_severity,
									pEvents[i].m_sourcePath,
									pEvents[i].m_message,
									DateTime(&pEvents[i].m_occurenceTime),
									attributes,
									pEvents[i].m_actorID,
									(EnumConditionChange)pEvents[i].m_changeMask,
									(EnumConditionState)pEvents[i].m_newState,
									ackRequired,
									(EnumQuality)pEvents[i].m_quality,
									pEvents[i].m_conditionName,
									pEvents[i].m_subConditionName,
									DateTime(&pEvents[i].m_activeTime),
									pEvents[i].m_cookie);
								bool conditionExists = FALSE;
								anAeSubscription->m_conditionListJanitor.lock();
								std::vector<AeCondition*>conditionList = anAeSubscription->getAeConditionList();
								std::vector<AeCondition*>::const_iterator conditionIterator;

								for (conditionIterator = conditionList.begin(); conditionIterator != conditionList.end(); conditionIterator++)
								{
									if (((*conditionIterator)->getConditionName() == ((AeCondition*)events[i])->getConditionName())
										&&
										((*conditionIterator)->getSourcePath() == ((AeCondition*)events[i])->getSourcePath()))
									{
										conditionExists = TRUE;
										changeCondition(
											TRUE,
											anAeSubscription,
											*conditionIterator,
											(AeCondition*)events[i]);
									} //end if
								} //end for

								anAeSubscription->m_conditionListJanitor.unlock();

								// not found condition -> add it
								if (!conditionExists)
								{
									AeCondition* newCondition = new AeCondition();
									changeCondition(
										FALSE,
										anAeSubscription,
										newCondition,
										(AeCondition*)events[i]);
								} //end for
							} //end else
						} //end for
					} //end if

					bool bRefresh = (refresh == 1) ? TRUE : FALSE;
					bool bLastRefresh = (lastRefresh == 1) ? TRUE : FALSE;
					anAeSubscription->onAeEventsReceived(
						bRefresh,
						bLastRefresh,
						events);
				} //end if

				aeSession->m_aeSubscriptionListJanitor.unlock();
			} //end if

			Application::Instance()->m_aeSessionListJanitor.unlock();
		} //end if

		std::vector<AeEvent*>::iterator aeEventIterator;

		for (aeEventIterator = events.begin(); aeEventIterator != events.end(); aeEventIterator++)
		{
			if (*aeEventIterator != NULL)
			{
				delete(*aeEventIterator);
			} //end if
		} //end for
	} //end if
} // end handleReceivedEvents

void OTCGlobals::handleRefreshAeConditions(
	IN unsigned long executionContext,
	IN OTCObjectContext objectContext,
	IN long result,
	IN unsigned char cancelRefresh)
{
	if (objectContext.m_objectType == EnumObjectType_AESUBSCRIPTION)
	{
		Application::Instance()->m_aeSessionListJanitor.lock();
		AeSession* aeSession = Application::Instance()->findAeSession(objectContext.m_sessionData.m_userData);

		if (aeSession != NULL)
		{
			aeSession->m_aeSubscriptionListJanitor.lock();
			AeSubscription* aeSubscription = aeSession->findSubscription(objectContext.m_objectData.m_userData);

			if (aeSubscription != NULL)
			{
				if (cancelRefresh == 0)
				{
					aeSubscription->onRefreshAeConditions(
						executionContext,
						result);
				}
				else
				{
					aeSubscription->onCancelRefreshAeConditions(
						executionContext,
						result);
				} //end if ... else
			} //end if

			aeSession->m_aeSubscriptionListJanitor.unlock();
		} //end if

		Application::Instance()->m_aeSessionListJanitor.unlock();
	} //end if
} // end handleRefreshAeConditions

void OTCGlobals::handleAcknowledgeAeConditions(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTChar* ackID,
	IN OTChar* ackComment,
	IN unsigned long count,
	IN OTCEventData* pEvents,
	IN long* pResults)
{
	if (count > 0)
	{
		unsigned long i, j;
		std::vector<AeCondition*> conditions(count, NULL);
		std::vector<long> results(count, 0);
		Application::Instance()->m_aeSessionListJanitor.lock();
		AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

		//the searched session was found
		if (aeSession != NULL)
		{
			for (i = 0; i < count; i++)
			{
				conditions[i] = new AeCondition();
				bool ackRequired = (pEvents[i].m_ackRequired == 1) ? TRUE : FALSE;
				conditions[i]->setAckRequired(ackRequired);
				conditions[i]->setActiveTime(DateTime(&pEvents[i].m_activeTime));
				conditions[i]->setActorId(pEvents[i].m_actorID);
				unsigned long attributesCount = pEvents[i].m_eventAttrCount;
				std::vector<Variant*> attributes(attributesCount, NULL);

				if (attributesCount > 0)
				{
					for (j = 0; j <  attributesCount; j++)
					{
						attributes[j] = new Variant(pEvents[i].m_pEventAttrs[j]);
					}//end for
				} //end if

				conditions[i]->setAttributes(attributes);
				conditions[i]->setCategory(pEvents[i].m_eventCategory);
				conditions[i]->setChangeMask((EnumConditionChange)pEvents[i].m_changeMask);
				conditions[i]->setConditionName(pEvents[i].m_conditionName);
				conditions[i]->setCookie(pEvents[i].m_cookie);
				conditions[i]->setMessage(pEvents[i].m_message);
				conditions[i]->setState((EnumConditionState)pEvents[i].m_newState);
				conditions[i]->setOccurenceTime(DateTime(&pEvents[i].m_occurenceTime));
				conditions[i]->setQuality((EnumQuality)pEvents[i].m_quality);
				conditions[i]->setSeverity(pEvents[i].m_severity);
				conditions[i]->setSourcePath(pEvents[i].m_sourcePath);
				conditions[i]->setSubConditionName(pEvents[i].m_subConditionName);
				conditions[i]->setEventType((EnumEventType)pEvents[i].m_eventType);

				if (pResults)
				{
					results[i] = pResults[i];
				} //end if
			}//end for

			tstring ackIDStr = tstring(ackID);
			tstring ackCommentStr = tstring(ackComment);
			aeSession->onAcknowledgeAeConditions(
				executionContext,
				ackIDStr,
				ackCommentStr,
				conditions,
				results,
				result);
		} //end if

		std::vector<AeCondition*>::iterator aeConditionIterator;

		for (aeConditionIterator = conditions.begin(); aeConditionIterator != conditions.end(); aeConditionIterator++)
		{
			if (*aeConditionIterator != NULL)
			{
				std::vector<Variant*>::iterator variantIterator;

				for (variantIterator = ((*aeConditionIterator)->getAttributes()).begin(); variantIterator != ((*aeConditionIterator)->getAttributes()).end(); variantIterator++)
				{
					if (*variantIterator != NULL)
					{
						delete(*variantIterator);
					} //end if
				} //end for

				delete(*aeConditionIterator);
			} //end if
		} //end for

		Application::Instance()->m_aeSessionListJanitor.unlock();
	} //end if
} // end handleAcknowledgeAeConditions

void OTCGlobals::handleQueryAvailableAeFilters(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN unsigned long availableFilters)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

	//the searched session was found
	if (aeSession != NULL)
	{
		aeSession->onQueryAvailableAeFilters(
			executionContext,
			(EnumFilterBy)availableFilters,
			result);
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleQueryAvailableAeFilters

void OTCGlobals::handleQueryAeCategories(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN unsigned long count,
	IN unsigned long* pEventTypes,
	IN unsigned long* pCategoryIds,
	IN OTChar* *pCategoryDescriptions)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	unsigned long i;

	//the searched session was found
	if (aeSession != NULL)
	{
		if (count > 0)
		{
			std::vector<AeCategory*> categories(count, NULL);

			for (i = 0; i < count; i++)
			{
				categories[i] = new AeCategory(
					(EnumEventType)pEventTypes[i],
					pCategoryIds[i],
					pCategoryDescriptions[i],
					aeSession);
			} //end for

			aeSession->onQueryAeCategories(
				executionContext,
				categories,
				result);
			//destroy the categories vector
			std::vector<AeCategory*>::iterator aeCategoryIterator;

			for (aeCategoryIterator = categories.begin(); aeCategoryIterator != categories.end(); aeCategoryIterator++)
			{
				if ((*aeCategoryIterator) != NULL)
				{
					delete(*aeCategoryIterator);
				} //end if
			} //end for
		} //end if
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
}//end handleQueryAeCategories


void OTCGlobals::handleQueryAeAttributes(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN unsigned long categoryId,
	IN unsigned long count,
	IN unsigned long* pAttributeIds,
	IN OTChar* *pAttributeDescriptions,
	IN unsigned short* pAttributeDatatypes)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	unsigned long i;

	//the searched session was found
	if (aeSession != NULL)
	{
		if (count > 0)
		{
			std::vector<AeAttribute*> attributes(count, NULL);

			for (i = 0; i < count; i++)
			{
				attributes[i] = new AeAttribute(
					pAttributeIds[i],
					pAttributeDescriptions[i],
					pAttributeDatatypes[i]);
			} //end for

			std::map<unsigned long, AeCategory*>::const_iterator aeCategoryIterator;
			aeSession->m_aeCategoryListJanitor.lock();

			for (aeCategoryIterator = aeSession->m_aeCategoryList.begin();
				 aeCategoryIterator != aeSession->m_aeCategoryList.end();
				 aeCategoryIterator++)
			{
				if ((aeCategoryIterator->second)->getId() == categoryId)
				{
					(aeCategoryIterator->second)->onQueryAeAttributes(
						executionContext,
						categoryId,
						attributes,
						result);
				} //end if
			} //end for

			aeSession->m_aeCategoryListJanitor.unlock();
			std::vector<AeAttribute*>::iterator aeAttributesIterator;

			for (aeAttributesIterator = attributes.begin(); aeAttributesIterator != attributes.end(); aeAttributesIterator++)
			{
				if ((*aeAttributesIterator) != NULL)
				{
					delete(*aeAttributesIterator);
				} //end if
			} //end for
		} //end if
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleQueryAeAttributes

void OTCGlobals::handleQueryAeConditions(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN unsigned long categoryId,
	IN unsigned long count,
	IN OTChar* *pConditionsNames)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	unsigned long i;

	//the searched session was found
	if (aeSession != NULL)
	{
		if (count > 0)
		{
			std::map<unsigned long, AeCategory*>::const_iterator aeCategoryIterator;
			std::vector<tstring> conditionNames(count, NULL);

			for (i = 0; i < count; i++)
			{
				conditionNames[i] = pConditionsNames[i];
			} //end for

			aeSession->m_aeCategoryListJanitor.lock();

			for (aeCategoryIterator = aeSession->m_aeCategoryList.begin(); aeCategoryIterator != aeSession->m_aeCategoryList.end(); aeCategoryIterator++)
			{
				if ((aeCategoryIterator->second)->getId() == categoryId)
				{
					(aeCategoryIterator->second)->onQueryAeConditionNames(
						executionContext,
						categoryId,
						conditionNames,
						result);
				} //end if
			} //end for

			aeSession->m_aeCategoryListJanitor.unlock();
		} //end if
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleQueryAeConditions

void OTCGlobals::handleQueryAeSubConditions(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTChar* conditionName,
	IN unsigned long count,
	IN OTChar* *pSubConditionsNames)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	unsigned long i;

	//the searched session was found
	if (aeSession != NULL)
	{
		if (count > 0)
		{
			std::map<unsigned long, AeCategory*>::const_iterator aeCategoryIterator;
			std::vector<tstring> subConditionNames(count, _T(""));

			for (i = 0; i < count; i++)
			{
				subConditionNames[i] = pSubConditionsNames[i];
			} //end for

			aeSession->m_aeCategoryListJanitor.lock();

			for (aeCategoryIterator = aeSession->m_aeCategoryList.begin(); aeCategoryIterator != aeSession->m_aeCategoryList.end(); aeCategoryIterator++)
			{
				for (i = 0; i < (aeCategoryIterator->second)->m_conditionNameList.size(); i++)
				{
					if ((aeCategoryIterator->second)->m_conditionNameList[i] == conditionName)
					{
						tstring conditionNameStr = tstring(conditionName);
						(aeCategoryIterator->second)->onQueryAeSubConditionNames(
							executionContext,
							conditionNameStr,
							subConditionNames,
							result);
					} //end if
				} //end for
			} //end for

			aeSession->m_aeCategoryListJanitor.unlock();
		} //end if
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleQueryAeSubConditions

void OTCGlobals::handleQueryAeSourceConditions(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTChar* sourcePath,
	IN unsigned long count,
	IN OTChar* *pConditionsNames)
{
	if (count > 0)
	{
		std::vector<tstring> conditionNames(count, _T(""));
		unsigned long i;
		AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

		//the searched session was found
		if (aeSession != NULL)
		{
			for (i = 0; i < count; i++)
			{
				conditionNames[i] = pConditionsNames[i];
			} //end for

			tstring sourcePathStr = tstring(sourcePath);
			aeSession->onQueryAeSourceConditions(
				executionContext,
				sourcePathStr,
				conditionNames,
				result);
		} //end if
	} //end if
} // end handleQueryAeSourceConditions

void OTCGlobals::handleGetAeConditionState(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTChar* sourcePath,
	IN OTChar* conditionName,
	IN unsigned long attributeCount,
	IN unsigned long* pAttributeIds,
	IN OTCAEConditionStateData* pConditionState)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	unsigned long i;

	//the searched session was found
	if (aeSession != NULL)
	{
		AeConditionState conditionState;
		conditionState.setAcknowledgerComment(pConditionState->m_ackComment);
		conditionState.setAcknowledgerId(pConditionState->m_ackID);
		conditionState.setConditionAckTime(DateTime(&pConditionState->m_conditionAckTime));
		conditionState.setConditionActiveTime(DateTime(&pConditionState->m_conditionActiveTime));
		conditionState.setConditionInactiveTime(DateTime(&pConditionState->m_conditionInactiveTime));
		conditionState.setQuality((EnumQuality)pConditionState->m_quality);
		conditionState.setState((EnumConditionState)pConditionState->m_state);
		conditionState.setSubConditionActiveTime(DateTime(&pConditionState->m_subConditionActiveTime));
		conditionState.setActiveSubConditionDefinition(pConditionState->m_actSubConditionDefinition);
		conditionState.setActiveSubConditionDescription(pConditionState->m_actSubConditionDescription);
		conditionState.setActiveSubConditionName(pConditionState->m_actSubConditionName);
		conditionState.setActiveSubConditionSeverity(pConditionState->m_actSubConditionSeverity);
		conditionState.setSubConditionActiveTime(DateTime(&pConditionState->m_subConditionActiveTime));
		unsigned long subConditionsCount = pConditionState->m_subConditionCount;
		std::vector<unsigned long>   subConditionsSeverities(subConditionsCount, 0);
		std::vector<tstring> subConditionsDefinitions(subConditionsCount, NULL);
		std::vector<tstring> subConditionsDescriptions(subConditionsCount, NULL);
		std::vector<tstring> subConditionsNames(subConditionsCount, NULL);

		for (i = 0; i < subConditionsCount; i++)
		{
			subConditionsSeverities[i]   = pConditionState->m_pSubConditionSeverity[i];
			subConditionsDefinitions [i] = pConditionState->m_pSubConditionDefinition[i];
			subConditionsDescriptions[i] = pConditionState->m_pSubConditionDescription[i];
			subConditionsNames[i]        = pConditionState->m_pSubConditionName[i];
		} //end for

		conditionState.setSubConditionsSeverities(subConditionsSeverities);
		conditionState.setSubConditionsDefinitions(subConditionsDefinitions);
		conditionState.setSubConditionsDescriptions(subConditionsDescriptions);
		conditionState.setSubConditionsNames(subConditionsNames);
		unsigned long countEventAttributes = pConditionState->m_eventAttrCount;
		std::vector<long> attributeErrors(countEventAttributes, 0);
		std::vector<Variant*> eventAttributes(countEventAttributes, 0);
		std::vector<unsigned long> attributeIds(attributeCount);

		if (pAttributeIds)
		{
			for (i = 0; i < countEventAttributes; i++)
			{
				eventAttributes[i] = new Variant(pConditionState->m_pEventAttrs[i]);
				attributeErrors[i] = pConditionState->m_pAttrErrors[i];
			} //end for

			for (i = 0; i < attributeCount; i++)
			{
				attributeIds[i] = pAttributeIds[i];
			}
		} //end if

		conditionState.setEventAttributes(eventAttributes);
		conditionState.setAttributesErrors(attributeErrors);
		tstring sourcePathStr = tstring(sourcePath);
		tstring conditionNameStr = tstring(conditionName);
		aeSession->onGetAeConditionState(
			executionContext,
			sourcePathStr,
			conditionNameStr,
			attributeIds,
			conditionState,
			result);
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleGetAeConditionState

void OTCGlobals::handleEnableAeConditions(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN unsigned char enable,
	IN unsigned char areaOrSource,
	IN OTChar* path)
{
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);
	bool enabled = (enable == 0) ? FALSE : TRUE;

	//the searched session was found
	if (aeSession != NULL)
	{
		tstring pathStr = tstring(path);

		if (areaOrSource == 0)
		{
			aeSession->onEnableAeConditionsBySource(
				executionContext,
				enabled,
				pathStr,
				result);
		}
		else
		{
			aeSession->onEnableAeConditionsByArea(
				executionContext,
				enabled,
				pathStr,
				result);
		} //end if
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleEnableAeConditions

void OTCGlobals::handleGetErrorString(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN long errorCode,
	IN OTChar* errorString)
{
	Application::Instance()->m_daSessionListJanitor.lock();
	DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

	if (daSession != NULL)
	{
		tstring errorStringStr = tstring(errorString);
		daSession->onGetErrorString(
			executionContext,
			errorCode,
			errorStringStr,
			result);
		Application::Instance()->m_daSessionListJanitor.unlock();
		return;
	} //end if

	Application::Instance()->m_daSessionListJanitor.unlock();
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

	if (aeSession != NULL)
	{
		tstring errorStringStr = tstring(errorString);
		aeSession->onGetErrorString(
			executionContext,
			errorCode,
			errorStringStr,
			result);
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleGetErrorString

void OTCGlobals::handleLogon(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result,
	IN OTChar* userName,
	IN OTChar* password)
{
	Application::Instance()->m_daSessionListJanitor.lock();
	DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

	if (daSession != NULL)
	{
		tstring userNameStr = tstring(userName);
		tstring passwordStr = tstring(password);
		daSession->onLogon(
			executionContext,
			userNameStr,
			passwordStr,
			result);
		Application::Instance()->m_daSessionListJanitor.unlock();
		return;
	} //end if

	Application::Instance()->m_daSessionListJanitor.unlock();
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

	if (aeSession != NULL)
	{
		tstring userNameStr = tstring(userName);
		tstring passwordStr = tstring(password);
		aeSession->onLogon(
			executionContext,
			userNameStr,
			passwordStr,
			result);
	} //end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleLogon

void OTCGlobals::handleLogoff(
	IN unsigned long executionContext,
	IN OTCObjectData sessionData,
	IN long result)
{
	Application::Instance()->m_daSessionListJanitor.lock();
	DaSession* daSession = Application::Instance()->findDaSession(sessionData.m_userData);

	if (daSession != NULL)
	{
		daSession->onLogoff(executionContext, result);
		Application::Instance()->m_daSessionListJanitor.unlock();
		return;
	}   //  end if

	Application::Instance()->m_daSessionListJanitor.unlock();
	Application::Instance()->m_aeSessionListJanitor.lock();
	AeSession* aeSession = Application::Instance()->findAeSession(sessionData.m_userData);

	if (aeSession != NULL)
	{
		aeSession->onLogoff(executionContext, result);
	}   //  end if

	Application::Instance()->m_aeSessionListJanitor.unlock();
} // end handleLogoff


//-----------------------------------------------------------------------------
//	allocateOTBString
//
OTChar* OTCGlobals::allocateOTBString(tstring& aValue)
{
	if (aValue.empty())
	{
		return NULL;
	}   //  end if

	OTChar*  result = (OTChar*)OTAllocateMemory((unsigned long)(aValue.size() + 1) * sizeof(OTChar));

	if (result != NULL)
	{
#ifdef TBC_OS_WINDOWS
		_tcscpy(result, aValue.c_str());
#endif
#ifdef TBC_OS_LINUX
		strcpy(result, aValue.c_str());
#endif
	}   //  end if

	return result;
}   //  end allocateOTBString


long OTCGlobals::advise()
{
	long result = E_FAIL;
	m_OTNCCallbacks.m_OTOnTrace = &handleTrace;
	m_OTNCCallbacks.m_OTCOnStateChange = &handleStateChange;
	m_OTNCCallbacks.m_OTCOnDataChange = &handleDataChange;
	m_OTNCCallbacks.m_OTCOnReadComplete = &handleReadComplete;
	m_OTNCCallbacks.m_OTCOnWriteComplete = &handleWriteComplete;
	m_OTNCCallbacks.m_OTCOnPerfromStateTransition = &handlePerformStateTransition;
	m_OTNCCallbacks.m_OTCOnServerShutdown = &handleServerShutdown;
	m_OTNCCallbacks.m_OTCOnUpdateAttributes = &handleUpdateAttributes;
	m_OTNCCallbacks.m_OTCOnUpdateDAItemAttributes = &handleUpdateDaItemAttributes;
	m_OTNCCallbacks.m_OTCOnValidateDAItems = &handleValidateDaItems;
	m_OTNCCallbacks.m_OTCOnGetServerStatus = &handleGetServerStatus;
	m_OTNCCallbacks.m_OTCOnBrowseAddressSpace = &handleBrowseAddressSpace;
	m_OTNCCallbacks.m_OTCOnGetDAProperties = &handleGetDaProperties;
	m_OTNCCallbacks.m_OTCOnBrowseServer = &handleBrowseServers;
	m_OTNCCallbacks.m_OTCOnReceivedEvents = &handleAeEventsReceived;
	m_OTNCCallbacks.m_OTCOnRefreshAEConditions = &handleRefreshAeConditions;
	m_OTNCCallbacks.m_OTCOnAcknowledgeAEConditions = &handleAcknowledgeAeConditions;
	m_OTNCCallbacks.m_OTCOnQueryAvailableAEFilters = &handleQueryAvailableAeFilters;
	m_OTNCCallbacks.m_OTCOnQueryAECategories = &handleQueryAeCategories;
	m_OTNCCallbacks.m_OTCOnQueryAEAttributes = &handleQueryAeAttributes;
	m_OTNCCallbacks.m_OTCOnQueryAEConditions = &handleQueryAeConditions;
	m_OTNCCallbacks.m_OTCOnQueryAESubConditions = &handleQueryAeSubConditions;
	m_OTNCCallbacks.m_OTCOnQueryAESourceConditions = &handleQueryAeSourceConditions;
	m_OTNCCallbacks.m_OTCOnGetAEConditionState = &handleGetAeConditionState;
	m_OTNCCallbacks.m_OTCOnEnableAEConditions = &handleEnableAeConditions;
	m_OTNCCallbacks.m_OTCOnGetErrorString = &handleGetErrorString;
	m_OTNCCallbacks.m_OTCOnLogon = &handleLogon;
	m_OTNCCallbacks.m_OTCOnLogoff = &handleLogoff;

	if (!m_isAdvised)
	{
		result = OTCAdvise(&m_OTNCCallbacks);

		if (SUCCEEDED(result))
		{
			m_isAdvised = TRUE;
		} // end if
	} // end if

	return result;
}
