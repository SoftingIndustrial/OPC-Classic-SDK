#include "OSCompat.h"

#include "ServerApplication.h"
#include "../Enums.h"

#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaRequest.h"
#include "Da/ServerDaTransaction.h"
#include "Da/ServerDaProperty.h"
#include "Da/ServerDaSession.h"

#include "Ae/ServerAeAddressSpaceElement.h"
#include "Ae/ServerAeAttribute.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeEvent.h"
#include "Ae/ServerAeCategory.h"
#include "ServerWebTemplate.h"

#include "ServerCommon.h"
#include "OTServer.h"

#ifdef TBC_OS_LINUX
#include <sys/time.h>
#endif

using namespace SoftingOPCToolboxServer;

TBC_EXPORT Application* API_CALL getApp()
{
	return Application::Instance();
}   //  end getApp


TBC_EXPORT void API_CALL releaseApp()
{
	Application::Release();
}   //  end releaseApp


namespace SoftingOPCToolboxServer
{

class OTSGlobals
{
private:

	static OTSCallbackFunctions m_OTNSCallBacks;

	static void API_CALL handleTrace(IN OTChar*  aTaceString, IN unsigned short aLevel, IN unsigned long aMask, IN OTChar*  anObjId, IN OTChar*  aText);

	static long API_CALL handleShutdown(void);
	static long API_CALL handleReadRequests(long aCount, OTSRequestData* paRequests);
	static long API_CALL handleWriteRequests(IN long aCount, IN OTSRequestData* paRequests, IN OTSValueData* pValues);
	static long API_CALL handleRequestCompletedInternally(IN unsigned long requestHandle);
	static long API_CALL handleQueryProperties(IN OTSObjectData* pObjectData, IN OTChar* anObjItemId, IN long aPropID, OUT unsigned long* pPropCount, OUT OTSPropertyData** ppPropData);
	static long API_CALL handleQueryAddressSpaceElementData(IN OTChar* aPath, IN unsigned char anElementType, OUT OTSAddressSpaceElementData* pData);
	static long API_CALL handleQueryAddressSpaceElementDataEx(IN OTChar* aPath, IN unsigned char anElementType, IN unsigned short& elementDataFilter, OUT OTSAddressSpaceElementData* pData);
	static long API_CALL handleQueryAddressSpaceElementChildren(IN OTChar* aPath, IN unsigned char anElementType, OUT unsigned long* pCount, OUT OTSAddressSpaceElementData** pElementData);
	static long API_CALL handleChangeSessionState(IN OTSObjectHandle hSession, IN OTSSessionData* pSessionStateData);
	static long API_CALL handleQueryCacheValue(IN OTSObjectHandle hClient, IN OTSObjectData objectData, OUT OTSValueData* pValue);
	static long API_CALL handleChangeItems(IN long itemCnt, IN OTSItemData* pItemData);
	static long API_CALL handleCreateAddressSpaceElement(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OTSAddressSpaceElementData* pDataIn, OUT OTSAddressSpaceElementData* pDataOut);
	static long API_CALL handleCreateAddressSpaceElementEx(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OTSAddressSpaceElementData* pDataIn, IN unsigned short elementDataFilter, OUT OTSAddressSpaceElementData* pDataOut);
	static long API_CALL handleDestroyAddressSpaceElement(IN OTSObjectData anObjectData);
	static long API_CALL handleQueryConditions(IN OTSObjectData* pObjectData, IN OTChar* aSourcePath, OUT unsigned long* pConditionCount, OUT OTChar***  pConditionNames);
	static long API_CALL handleAcknowledgeCondition(IN OTSObjectData aConditionData, IN OTChar*  anAckId, OTChar*  ackComment);
	static long API_CALL handleQueryConditionDefinition(IN OTSObjectData cConditionData, OUT OTSConditionDefinitionData* pConditionDefData);
	static long API_CALL handleEnableConditions(IN unsigned char enable, IN OTChar* addressSpaceElementName);
	static long API_CALL handleWebTemplate(IN OTChar*  templateName, IN unsigned long numArgs, IN OTChar*  *pArgs, OUT OTChar*  *pResult);

public:
	static long advise(void);
	static long unadvise(void);
	static OTChar* allocateOTBString(tstring& aValue);

	//-----------------------------------------------------------------------------
	//  fillOTSConditionDefinitionData
	//
	static long fillOTSConditionDefinitionData(
		AeConditionDefinition& aSource,
		OTSConditionDefinitionData& aDestination)
	{
		memset(&aDestination, 0, sizeof(OTSConditionDefinitionData));
		aDestination.m_definition = allocateOTBString(aSource.m_definition);
		unsigned long size = (unsigned long) aSource.m_subconditions.size();
		aDestination.m_subConditionCount = size;

		if (size > 0)
		{
			aDestination.m_subConditionDefinitions = (OTChar**)OTAllocateMemory(size * sizeof(OTChar*));
			aDestination.m_subConditionDescriptions = (OTChar**)OTAllocateMemory(size * sizeof(OTChar*));
			aDestination.m_subConditionSeverities = (unsigned long*)OTAllocateMemory(size * sizeof(unsigned long));
			AeSubConditionDefinition* element = NULL;
			std::map<tstring, AeSubConditionDefinition*>::iterator elementIterator;
			unsigned long index = 0;

			for (elementIterator = aSource.m_subconditions.begin(), index = 0;
				 elementIterator != aSource.m_subconditions.end() && index < size;
				 elementIterator++, index++)
			{
				element = elementIterator->second;

				if (element != NULL)
				{
					aDestination.m_subConditionDefinitions[index]   = allocateOTBString((tstring&)element->getDefinition());
					aDestination.m_subConditionDescriptions[index]  = allocateOTBString((tstring&)element->getDescription());
					aDestination.m_subConditionSeverities[index]    = element->getSeverity();
				}
				else
				{
					aDestination.m_subConditionDefinitions[index] = NULL;
					aDestination.m_subConditionDescriptions[index] = NULL;
					aDestination.m_subConditionSeverities[index] = 0;
				}   //  end if ... else
			}   //  end for
		}   //  end if

		return S_OK;
	}   //  end fillOTSConditionDefinitionData


	//-----------------------------------------------------------------------------
	//  releaseOTSConditionDefinitionData
	//
	static long releaseOTSConditionDefinitionData(OTSConditionDefinitionData& aData)
	{
		if (aData.m_definition != NULL)
		{
			OTFreeMemory(aData.m_definition);
			aData.m_definition = NULL;
		}   //  end if

		unsigned long size = aData.m_subConditionCount;

		if (size > 0)
		{
			for (unsigned long index = 0; index < size; index++)
			{
				if (aData.m_subConditionDefinitions[index] != NULL)
				{
					OTFreeMemory(aData.m_subConditionDefinitions[index]);
					aData.m_subConditionDefinitions[index] = 0;
				}   //  end if

				if (aData.m_subConditionDescriptions[index])
				{
					OTFreeMemory(aData.m_subConditionDescriptions[index]);
					aData.m_subConditionDefinitions[index] = 0;
				}   //  end if
			}   //  end for

			if (aData.m_subConditionDefinitions != NULL)
			{
				OTFreeMemory(aData.m_subConditionDefinitions);
			}   //  end if

			if (aData.m_subConditionDescriptions != NULL)
			{
				OTFreeMemory(aData.m_subConditionDescriptions);
			}   //  end if

			if (aData.m_subConditionSeverities != NULL)
			{
				OTFreeMemory(aData.m_subConditionSeverities);
			}   //  end if
		}   //  end if

		return S_OK;
	}   //  end releaseOTSConditionDefinitionData

	//-----------------------------------------------------------------------------
	//  fillOTSConditionData
	//
	static long fillOTSConditionData(
		AeCondition& aSource,
		OTSConditionData& aDestination)
	{
		aDestination.m_eventCategory    = aSource.m_eventCategory->getId();
		aDestination.m_name             = allocateOTBString(aSource.m_name);
		aDestination.m_sourcePath       = allocateOTBString(aSource.m_sourcePath);
		aDestination.m_quality          = (unsigned char)aSource.m_quality;
		aDestination.m_userData         = aSource.m_userData;
		aDestination.m_severity         = aSource.m_severity;
		aDestination.m_message          = allocateOTBString(aSource.m_message);
		aDestination.m_ackRequired      = (unsigned char)aSource.m_ackRequired;
		aDestination.m_activeSubConditionName = allocateOTBString(aSource.m_activeSubConditionName);
		aDestination.m_changeMask       = aSource.m_changeMask;
		aDestination.m_stateChange      = (unsigned char)aSource.m_stateChange;
		aDestination.m_occurenceTime    = aSource.m_occurenceTime.get();
		aDestination.m_ackID            = allocateOTBString(aSource.m_ackId);
		aDestination.m_ackComment       = allocateOTBString(aSource.m_ackComment);
		aDestination.m_ackTime          = aSource.m_ackTime.get();
		aDestination.m_eventAttrCount   = (unsigned long)aSource.m_eventAttributes.size();

		if (aDestination.m_eventAttrCount > 0)
		{
			aDestination.m_pEventAttrs = (OTVariant*)OTAllocateMemory((unsigned long)aSource.m_eventAttributes.size() * sizeof(OTVariant));

			if (aDestination.m_pEventAttrs != NULL)
			{
				for (unsigned long index = 0; index < aSource.m_eventAttributes.size(); index++)
				{
					Variant::variantCopy(&aDestination.m_pEventAttrs[index], &aSource.m_eventAttributes[index]);
				} // end for
			}
			else
			{
				return EnumResultCode_E_OUTOFMEMORY;
			}   //  end if ... else
		}
		else
		{
			aDestination.m_eventAttrCount = 0;
		}   //  end if ... else

		return S_OK;
	}   //  end fillOTSConditionData


	static long releaseOTSConditionData(OTSConditionData& aData)
	{
		if (aData.m_name != NULL)
		{
			OTFreeMemory(aData.m_name);
			aData.m_name = NULL;
		}   //  end if

		if (aData.m_sourcePath != NULL)
		{
			OTFreeMemory(aData.m_sourcePath);
			aData.m_sourcePath = NULL;
		}   //  end if

		if (aData.m_message != NULL)
		{
			OTFreeMemory(aData.m_message);
			aData.m_message = NULL;
		}   //  end if

		if (aData.m_activeSubConditionName != NULL)
		{
			OTFreeMemory(aData.m_activeSubConditionName);
			aData.m_activeSubConditionName = NULL;
		}   //  end if

		if (aData.m_ackID != NULL)
		{
			OTFreeMemory(aData.m_ackID);
			aData.m_ackID = NULL;
		}   //  end if

		if (aData.m_ackComment != NULL)
		{
			OTFreeMemory(aData.m_ackComment);
			aData.m_ackComment = NULL;
		}   //  end if

		if (aData.m_eventAttrCount > 0 && aData.m_pEventAttrs != NULL)
		{
			for (unsigned int index = 0; index < aData.m_eventAttrCount; index++)
			{
				Variant::variantClear(&aData.m_pEventAttrs[index]);
			}   // end for

			OTFreeMemory(aData.m_pEventAttrs);
			aData.m_pEventAttrs = NULL;
		}   //  end if

		return S_OK;
	}   //  end releaseOTSConditionData


	static long fillOTSEventData(AeEvent& aSource, OTSEventData& aDestination)
	{
		aDestination.m_eventType        = aSource.m_type;
		aDestination.m_eventCategory    = aSource.m_categoryId;
		aDestination.m_severity         = aSource.m_severity;
		aDestination.m_sourcePath       = allocateOTBString(aSource.m_source);
		aDestination.m_message          = allocateOTBString(aSource.m_message);
		aDestination.m_occurenceTime    = aSource.m_occurenceTime.get();
		aDestination.m_actorID          = allocateOTBString(aSource.m_actorId);
		unsigned long count = (unsigned long)aSource.m_attributeValueList.size();
		aDestination.m_eventAttrCount   = count;

		if (count > 0)
		{
			aDestination.m_pEventAttrs = (OTVariant*)OTAllocateMemory(count * sizeof(OTVariant));

			for (unsigned long index = 0; index < count; index++)
			{
				Variant::variantCopy(&aDestination.m_pEventAttrs[index], &(aSource.m_attributeValueList[index]));
			}   //  end for
		}
		else
		{
			aDestination.m_pEventAttrs = NULL;
		}   //end if

		return S_OK;
	}   //  end fillOTSEventData

	static long releaseOTSEventData(OTSEventData& aData)
	{
		if (aData.m_sourcePath != NULL)
		{
			OTFreeMemory(aData.m_sourcePath);
			aData.m_sourcePath = NULL;
		}   //  end if

		if (aData.m_message != NULL)
		{
			OTFreeMemory(aData.m_message);
			aData.m_message = NULL;
		}   //  end if

		if (aData.m_actorID != NULL)
		{
			OTFreeMemory(aData.m_actorID);
			aData.m_actorID = NULL;
		}   //  end if

		if (aData.m_eventAttrCount > 0 && aData.m_pEventAttrs != NULL)
		{
			for (unsigned int index = 0; index < aData.m_eventAttrCount; index++)
			{
				Variant::variantClear(&aData.m_pEventAttrs[index]);
			}   // end for

			OTFreeMemory(aData.m_pEventAttrs);
			aData.m_pEventAttrs = NULL;
		}   //  end if

		return S_OK;
	}   //  end releaseOTSEventData

};  //  end class OTSGlobals

}   //  end namespace SoftingOPCToolboxServer

OTSCallbackFunctions OTSGlobals::m_OTNSCallBacks;

//-----------------------------------------------------------------------------
//	static members initialization region
//
Application::Application()
{
	::OTInitLib();
	m_versionOTB                  = 447;
	m_appType                     = EnumApplicationType_EXECUTABLE;
	m_clsidDA                     = _T("");
	m_progIdDA                    = _T("");
	m_verIndProgIdDA              = _T("");
	m_clsidAE                     = _T("");
	m_progIdAE                    = _T("");
	m_verIndProgIdAE              = _T("");
	m_description                 = _T("");
	m_ipPortHTTP                  = 0;
	m_urlDA                       = _T("");
	m_majorVersion                = 0;
	m_minorVersion                = 0;
	m_patchVersion                = 0;
	m_buildNumber                 = 0;
	m_vendorInfo                  = _T("");
	m_minUpdateRateDA             = 100;
	m_addressSpaceDelimiter       = _T('.');
	m_clientCheckPeriod           = 1000;
	m_propertyDelimiter           = _T('#');
	m_supportDisableConditions    = 0;
	m_tpPort                      = 0;
	m_tpCredentialsNumber         = 0;
	m_stringBasedObjectDataExpiry = 60000; // 60 seconds initial cache value
	TraceCompleted                = NULL;
	ShutdownRequest               = NULL;
	m_DaAddressSpaceRoot          = NULL;
	m_AeAddressSpaceRoot          = NULL;
	m_webTemplate                 = NULL;
	m_optimizeForSpeed            = 0; // no optimization, preserve item order
	m_authLevel                   = RPC_C_AUTHN_LEVEL_CONNECT;
	m_impLevel                    = RPC_C_IMP_LEVEL_IDENTIFY;
	m_cbAuthLevel                 = RPC_C_AUTHN_LEVEL_CONNECT;
	m_cbImpLevel                  = RPC_C_IMP_LEVEL_IDENTIFY;
	OTSGlobals::advise();
}   //  end constructor


//-----------------------------------------------------------------------------
//	destructor
//
Application::~Application()
{
	terminate();
	OTSGlobals::unadvise();
	::OTExitLib();
}   //  end destructor


//-----------------------------------------------------------------------------
//	activate
//
long Application::activate(
	EnumFeature aFeature,
	tstring aKey)
{
	int res = (int) EnumResultCode_E_FAIL;
	tstring licenseName = _T("");

	res =  OTActivate((unsigned char)aFeature, (OTChar*)aKey.c_str(), 0);
	EnumTraceLevel level = EnumTraceLevel_INF;

	if (FAILED(res))
	{
		level = EnumTraceLevel_ERR;
	}


	switch (aFeature)
	{
	case EnumFeature_DA_SERVER:
		licenseName = _T("OPC DA Server");
		break;
	case EnumFeature_XMLDA_SERVER:
		licenseName = _T("XMLDA Server");
		break;
	case EnumFeature_TP_SERVER:
		licenseName = _T("Tunnel Server");
		break;
	case EnumFeature_AE_SERVER:
		licenseName = _T("OPC AE Server");
		break;
	default:
		{
			TCHAR buffer[32];
			_stprintf(buffer, _T("unknown server feature - %u"), aFeature);
			licenseName = buffer;
		}
		break;
	}

	Application::Instance()->trace(
		level,
		EnumTraceGroup_SERVER,
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
	return OTActivate(0, NULL, 1);
}   //  end forceDemoVersion


//-----------------------------------------------------------------------------
//	initialize
//
long Application::initialize(Creator* aCreator)
{
	if (aCreator == NULL)
	{
		return EnumResultCode_E_UNEXPECTED;
	}   //  end if

	m_creator = aCreator;
	m_DaAddressSpaceRoot = m_creator->createDaAddressSpaceRoot();

	if (m_DaAddressSpaceRoot == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	m_AeAddressSpaceRoot = m_creator->createAeAddressSpaceRoot();

	if (m_AeAddressSpaceRoot == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	m_webTemplate = m_creator->createWebTemplate();
	OTSInitData initData;
	memset(&initData, 0, sizeof(OTSInitData));
	initData.m_versionOTS                  = m_versionOTB;
	initData.m_appType                     = (unsigned char)m_appType;
	initData.m_clsidDA                     = (OTChar*)m_clsidDA.c_str();
	initData.m_progIdDA                    = (OTChar*)m_progIdDA.c_str();
	initData.m_verIndProgIdDA              = (OTChar*)m_verIndProgIdDA.c_str();
	initData.m_clsidAE                     = (OTChar*)m_clsidAE.c_str();
	initData.m_progIdAE                    = (OTChar*)m_progIdAE.c_str();
	initData.m_verIndProgIdAE              = (OTChar*)m_verIndProgIdAE.c_str();
	initData.m_description                 = (OTChar*)m_description.c_str();
	initData.m_ipPortHTTP                  = m_ipPortHTTP;
	initData.m_urlDA                       = (OTChar*)m_urlDA.c_str();
	initData.m_majorVersion                = m_majorVersion;
	initData.m_minorVersion                = m_minorVersion;
	initData.m_patchVersion                = m_patchVersion;
	initData.m_buildNumber                 = m_buildNumber;
	initData.m_vendorInfo                  = (OTChar*)m_vendorInfo.c_str();
	initData.m_minUpdateRateDA             = m_minUpdateRateDA;
	initData.m_serviceName                 = (OTChar*)m_serviceName.c_str();
	initData.m_serviceDescription          = (OTChar*)m_serviceDescription.c_str();
	initData.m_addressSpaceDelimiter       = m_addressSpaceDelimiter;
	initData.m_clientCheckPeriod           = m_clientCheckPeriod;
	initData.m_propertyDelimiter           = m_propertyDelimiter;
	initData.m_supportDisableConditions    = m_supportDisableConditions;
	initData.m_webRootDirectory            = (OTChar*)m_webRootDirectory.c_str();
	initData.m_webRootFile                 = (OTChar*)m_webRootFile.c_str();
	initData.m_webAdministratorPassword    = (OTChar*)m_webAdministratorPassword.c_str();
	initData.m_webOperatorPassword         = (OTChar*)m_webOperatorPassword.c_str();
	initData.m_tpPort                      = m_tpPort;
	initData.m_stringBasedObjectDataExpiry = m_stringBasedObjectDataExpiry;
	initData.m_optimizeForSpeed            = m_optimizeForSpeed;
	initData.m_authLevel                   = m_authLevel;
	initData.m_impLevel                    = m_impLevel;
	initData.m_cbAuthLevel                 = m_cbAuthLevel;
	initData.m_cbImpLevel                  = m_cbImpLevel;

	if (m_tpCredentialsNumber > 0)
	{
		initData.m_tpCredentialsNumber = m_tpCredentialsNumber;
		initData.m_tpUsers = (OTChar**)malloc(m_tpCredentialsNumber * sizeof(OTChar*));

		if (initData.m_tpUsers == NULL)
		{
			return EnumResultCode_E_OUTOFMEMORY;
		}

		initData.m_tpPasswords = (OTChar**)malloc(m_tpCredentialsNumber * sizeof(OTChar*));

		if (initData.m_tpPasswords == NULL)
		{
			return EnumResultCode_E_OUTOFMEMORY;
		}

		int index = 0;

		for (index = 0; index < m_tpCredentialsNumber; index++)
		{
			initData.m_tpUsers[index] = (OTChar*)m_tpUsers[index].c_str();
			initData.m_tpPasswords[index] = (OTChar*)m_tpPasswords[index].c_str();
		}
	}
	else
	{
		initData.m_tpCredentialsNumber = 0;
		initData.m_tpUsers = NULL;
		initData.m_tpPasswords = NULL;
	}

	long result = OTSInitialize(&initData);

	if (m_tpCredentialsNumber > 0)
	{
		if (initData.m_tpUsers != NULL)
		{
			free(initData.m_tpUsers);
			initData.m_tpUsers = NULL;
		}

		if (initData.m_tpPasswords != NULL)
		{
			free(initData.m_tpPasswords);
			initData.m_tpPasswords = NULL;
		}
	}   //  end if

	return result;
}   //  end initialize


//-----------------------------------------------------------------------------
//	terminate
//
long Application::terminate()
{
	BOOL isInitialized = FALSE;

	if (m_DaAddressSpaceRoot != NULL)
	{
		delete m_DaAddressSpaceRoot;
		m_DaAddressSpaceRoot = NULL;
		isInitialized = TRUE;
	}   //  end if

	if (m_AeAddressSpaceRoot != NULL)
	{
		delete m_AeAddressSpaceRoot;
		m_AeAddressSpaceRoot = NULL;
		isInitialized = TRUE;
	}   //  end if

	if (m_webTemplate != NULL)
	{
		delete m_webTemplate;
		m_webTemplate = NULL;
	}   //  end if

	std::map<unsigned long, DaSession*>::iterator sessionIterator;
	m_sessionListJanitor.lock();

	for (sessionIterator = m_sessionList.begin(); sessionIterator != m_sessionList.end(); sessionIterator++)
	{
		if (sessionIterator->second != NULL)
		{
			delete sessionIterator->second;
			sessionIterator->second = NULL;
		}   //  end if
	}   //  end if

	m_sessionList.clear();
	m_sessionListJanitor.unlock();
	std::map<unsigned long, AeCondition*>::iterator conditionIterator;
	m_conditionListJanitor.lock();

	for (conditionIterator = m_conditionList.begin(); conditionIterator != m_conditionList.end(); conditionIterator++)
	{
		if (conditionIterator->second != NULL)
		{
			conditionIterator->second->setHandle(0);
			delete conditionIterator->second;
			conditionIterator->second = NULL;
		}   //  end if
	}   //  end if

	m_conditionList.clear();
	m_conditionListJanitor.unlock();

	if (isInitialized)
	{
		return OTSTerminate();
	}

	return S_OK;
}   //  end terminate


//-----------------------------------------------------------------------------
//	processCommandLine
//
long Application::processCommandLine(tstring& aCommandLine)
{
	return OTSProcessCommandLine((OTChar*) aCommandLine.c_str());
}   //  end processCommandLine


long Application::setAeServerState(EnumServerState newState)
{
	return OTSSetServerState(0, newState);
} // end setAeServerState

long Application::setDaServerState(EnumServerState newState)
{
	return OTSSetServerState(newState, 0);
} // end setDaServerState


long Application::start()
{
	return OTSStart();
}   //  end start


long Application::ready()
{
	return OTSReady();
}   //  end ready


long Application::stop(tstring reason)
{
	if (reason.length() > 0)
	{
		OTChar* otcReason = OTSGlobals::allocateOTBString(reason);
		long result = OTSStop(otcReason);

		if (otcReason != NULL)
		{
			OTFreeMemory(otcReason);
		}

		return result;
	}
	else
	{
		return OTSStop(NULL);
	}
}   //  end stop


long Application::addAeCategory(AeCategory* aCategory)
{
	unsigned long categoryHandle = 0;
	OTChar* otcName = OTSGlobals::allocateOTBString(aCategory->getName());
	long result = OTSAddEventCategory(
					  aCategory->getId(),
					  otcName,
					  aCategory->getEventType(),
					  &categoryHandle);

	if (SUCCEEDED(result))
	{
		aCategory->setHandle(categoryHandle);
		m_categoryListJanitor.lock();
		m_categoryList.insert(std::pair<unsigned long, AeCategory*>(categoryHandle, aCategory));
		m_categoryListJanitor.unlock();
	}   //  end if

	OTFreeMemory(otcName);
	return result;
}   //  end addAeCategory


std::vector<AeCondition*> Application::getAeConditionList(void)
{
	m_conditionListJanitor.lock();
	std::vector<AeCondition*> conditionVector(m_conditionList.size(), NULL);
	//  Check for a duplicate condition
	std::map<unsigned long, AeCondition*>::iterator elementIterator;
	int index = 0;

	for (elementIterator = m_conditionList.begin();
		 elementIterator != m_conditionList.end();
		 elementIterator++)
	{
		conditionVector[index++] = elementIterator->second;
	}   //  end for

	m_conditionListJanitor.unlock();
	return conditionVector;
}   //  end getAeConditionList


std::vector<AeCategory*> Application::getAeCategoryList(void)
{
	m_categoryListJanitor.lock();
	std::vector<AeCategory*> categoryVector(m_categoryList.size(), NULL);
	//  Check for a duplicate condition
	std::map<unsigned long, AeCategory*>::iterator elementIterator;
	int index = 0;

	for (elementIterator = m_categoryList.begin();
		elementIterator != m_categoryList.end();
		elementIterator++)
	{
		categoryVector[index++] = elementIterator->second;
	}   //  end for

	m_categoryListJanitor.unlock();
	return categoryVector;
}   //  end getAeCategoryList


long Application::changeCondition(AeCondition* aCondition)
{
	if (aCondition == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	std::vector<AeCondition*> conditionVector(1);
	conditionVector[0] = aCondition;
	return changeConditions(conditionVector);
}   //  end addCondition


long Application::changeConditions(std::vector<AeCondition*>& aConditionList)
{
	long result = S_OK;
	unsigned long index = 0;
	AeCondition* condition = NULL;
	unsigned long count = (unsigned long)aConditionList.size();

	if (count == 0)
	{
		return S_FALSE;
	}   //  end if

	unsigned long* inHandles = new unsigned long[count];
	OTSConditionData* conditionData = new OTSConditionData[count];
	memset(conditionData, 0, count * sizeof(OTSConditionData));
	unsigned long* outHandles = new unsigned long[count];

	for (index = 0; index < count; index++)
	{
		condition = aConditionList[index];

		if (condition != NULL)
		{
			result = OTSGlobals::fillOTSConditionData(*condition, conditionData[index]);

			if (!SUCCEEDED(result))
			{
				break;
			}   //end if

			inHandles[index] = condition->getHandle();
			outHandles[index] = 0;
		}   //  end if
	}   //  end for

	if (SUCCEEDED(result))
	{
		result = OTSConditionsChanged(count, inHandles, conditionData, outHandles);
	}   //  end if

	if (SUCCEEDED(result))
	{
		m_conditionListJanitor.lock();
		for (index = 0; index < count; index++)
		{
			condition = aConditionList[index];

			if (inHandles[index] == 0)
			{
				if (outHandles[index] == 0)
				{
					//  strange situation, new condition to add, but not accepted
					result = E_FAIL;
				}
				else
				{
					condition->setHandle(outHandles[index]);
					m_conditionList.insert(std::pair<unsigned long, AeCondition*>(outHandles[index], condition));
				}   //  end if ... else
			}
			else
			{
				if (outHandles[index] == 0)
				{
					//  remove the condition
					condition->setHandle(0);
					m_conditionList.erase(inHandles[index]);
				}
				else
				{
					//  normal situation for a update only action
				}   //  end if ... else
			}   //  end if ... else
		}   //  end for

		m_conditionListJanitor.unlock();
	}   //  end if

	for (index = 0; index < count; index++)
	{
		OTSGlobals::releaseOTSConditionData(conditionData[index]);
	}   //  end for

	delete [] conditionData;
	delete [] inHandles;
	delete [] outHandles;
	return result;
}   //  end changeConditions


void Application::releaseCondition(unsigned long aConditionHandle)
{
	m_conditionListJanitor.lock();
	m_conditionList.erase(aConditionHandle);
	m_conditionListJanitor.unlock();
}   //  end releaseCondition


long Application::fireEvents(std::vector<AeEvent*>& anEventList)
{
	unsigned long count = (unsigned long)anEventList.size();

	if (count == 0)
	{
		return S_OK;
	}   //  end if

	OTSEventData* eventsData = new OTSEventData[count];
	unsigned long index = 0;
	AeEvent* element = NULL;

	for (index = 0; index < count; index++)
	{
		element = anEventList[index];

		if (element != NULL)
		{
			OTSGlobals::fillOTSEventData(*anEventList[index], eventsData[index]);
		}   //  end if
	}   //  end foreach

	unsigned long result = OTSFireEvents(count, eventsData);

	for (index = 0; index < count; index++)
	{
		OTSGlobals::releaseOTSEventData(eventsData[index]);
	}   //  end for

	delete[] eventsData;
	return result;
}   //  end fire


//-----------------------------------------------------------------------------
// Get Client by the handle
//
DaSession* Application::getSession(unsigned long aSessionHandle)
{
	std::map<unsigned long, DaSession*>::const_iterator sessionIterator;
	m_sessionListJanitor.lock();
	sessionIterator = m_sessionList.find(aSessionHandle);

	if (sessionIterator != m_sessionList.end())
	{
		m_sessionListJanitor.unlock();
		return sessionIterator->second;
	}   //  end if

	m_sessionListJanitor.unlock();
	return NULL;
}   //  end getSession


//-----------------------------------------------------------------------------
//	addSession
//
void Application::addSession(DaSession* aSession, unsigned long aSessionHandle)
{
	m_sessionListJanitor.lock();
	m_sessionList.insert(std::pair<unsigned long, DaSession*>(aSessionHandle,  aSession));
	m_sessionListJanitor.unlock();
}   //  end addSession


//-----------------------------------------------------------------------------
//	removeSession
//
void Application::removeSession(unsigned long aSessionHandle)
{
	m_sessionListJanitor.lock();
	DaSession* session = getSession(aSessionHandle);

	if (session != NULL)
	{
		delete session;
	}   //  end if

	unsigned long result = (unsigned long)m_sessionList.erase(aSessionHandle);
	m_sessionListJanitor.unlock();

	if (result != 1)
	{
		trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER, _T("Application::removeSession"), _T("Invalid result value"));
	}   //  end if
}   //  end removeSession


//-----------------------------------------------------------------------------
//	finds a transaction in the internal active transactions list
//
DaTransaction* Application::findTransaction(unsigned long aKey)
{
	std::map<unsigned long, DaTransaction*>::const_iterator transactionIterator;
	m_transactionListJanitor.lock();
	transactionIterator = m_transactionList.find(aKey);

	if (transactionIterator == m_transactionList.end())
	{
		m_transactionListJanitor.unlock();
		return NULL;
	}   //  end if

	DaTransaction* pTransaction = transactionIterator->second;
	m_transactionListJanitor.unlock();
	return pTransaction;
}   //  end findTransaction


//-----------------------------------------------------------------------------
//	finds and releases a transaction
//
void Application::releaseTransaction(unsigned long aKey)
{
	std::map<unsigned long, DaTransaction*>::iterator transactionIterator;
	m_transactionListJanitor.lock();
	transactionIterator = m_transactionList.find(aKey);

	if (transactionIterator == m_transactionList.end())
	{
		m_transactionListJanitor.unlock();
		return;
	}   //  end if

	if (transactionIterator->second == NULL)
	{
		m_transactionListJanitor.unlock();
		return;
	}   //  end if

	DaTransaction* transaction = transactionIterator->second;

	if (transaction->isEmpty())
	{
		m_transactionList.erase(transactionIterator);
		delete transaction;
	}   //  end if

	m_transactionListJanitor.unlock();
}   //  end releaseTransaction


//-----------------------------------------------------------------------------
//	addTransaction
//
void Application::addTransaction(
	unsigned long aKey,
	DaTransaction* aTransaction)
{
	m_transactionListJanitor.lock();
	m_transactionList.insert(std::pair<unsigned long, DaTransaction*>(aKey, aTransaction));
	m_transactionListJanitor.unlock();
}   //  end addTransaction

//-----------------------------------------------------------------------------
//	markRequestCompletedInternally
//
long Application::markRequestCompletedInternally(unsigned long requestHandle)
{
	std::map<unsigned long, DaTransaction*>::iterator transactionIterator;
	DaRequest* pRequest = NULL;
	long res;
	m_transactionListJanitor.lock();
	for (transactionIterator = m_transactionList.begin(); transactionIterator != m_transactionList.end(); ++transactionIterator)
	{
		if (transactionIterator->second != NULL)
		{
			res = transactionIterator->second->findRequestByHandle(requestHandle, &pRequest);
			if (SUCCEEDED(res))
			{
				pRequest->completedInternally();
				m_transactionListJanitor.unlock();
				return res;
			}
		}
	}

	m_transactionListJanitor.unlock();
	return OT_E_OPC_NOTFOUND;
}	// end markRequestCompletedInternally

//-----------------------------------------------------------------------------
//	enableTracing
//
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


//-----------------------------------------------------------------------------
//	trace
//
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
	Trace::writeline((unsigned char)aLevel, aMask, anObjectIDStr, bufferStr);
}   //  end trace


//-----------------------------------------------------------------------------
//	handleTrace
//
void OTSGlobals::handleTrace(
	IN OTChar*  aTraceString,
	IN unsigned short aLevel,
	IN unsigned long aMask,
	IN OTChar*  anObjId,
	IN OTChar*  aText)
{
	if (Application::Instance()->TraceCompleted != NULL)
	{
		Application::Instance()->TraceCompleted(aTraceString, aLevel, aMask, anObjId, aText);
	}   //  end if
}   //  end handleTrace


//-----------------------------------------------------------------------------
//	handleShutdown
//
long OTSGlobals::handleShutdown(void)
{
	if (Application::Instance()->ShutdownRequest != NULL)
	{
		return Application::Instance()->ShutdownRequest();
	}   //  end if

	return S_OK;
}   //  end handleShutdown


//-----------------------------------------------------------------------------
//	handleReadRequests
//
long OTSGlobals::handleReadRequests(
	long aCount,
	OTSRequestData* aRequestDataList)
{
	if (aCount == 0 || aRequestDataList == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	Creator* pCreator = Application::Instance()->getCreator();
	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();

	if (pCreator == NULL || pRoot == NULL)
	{
		return E_FAIL;
	}   //  end if

	unsigned long sessionHandle = aRequestDataList[0].m_clientHandle;
	std::vector<DaRequest*> requests(aCount, NULL);
	pRoot->m_elementsJanitor.lock();

	for (int i = 0; i < aCount; i++)
	{
		DaAddressSpaceElement* element = (DaAddressSpaceElement*)pRoot->getElementFromArray(aRequestDataList[i].m_object.m_userData);
		DaRequest* request =  pCreator->createRequest(
								  EnumTransactionType_READ,
								  sessionHandle,
								  element,
								  aRequestDataList[i].m_propertyID,
								  aRequestDataList[i].m_requestHandle);
		requests[i] = request;
	}   //  end for

	pRoot->m_elementsJanitor.unlock();
	DaTransaction* pTransaction = pCreator->createTransaction(
									  EnumTransactionType_READ,
									  requests,
									  sessionHandle);

	if (pTransaction == NULL)
	{
		return EnumResultCode_E_NOTIMPL;
	}   //  end if

	Application::Instance()->addTransaction(pTransaction->getKey(), pTransaction);
	return pTransaction->handleReadRequests();
}   //  end handleReadRequests


//-----------------------------------------------------------------------------
//	handleWriteRequests
//
long OTSGlobals::handleWriteRequests(
	IN long aCount,
	IN OTSRequestData* aRequestDataList,
	IN OTSValueData* pValues)
{
	if (aCount == 0 || aRequestDataList == NULL || pValues == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	Creator* pCreator = Application::Instance()->getCreator();
	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();

	if (pCreator == NULL || pRoot == NULL)
	{
		return E_FAIL;
	}   //  end if

	unsigned long sessionHandle = aRequestDataList[0].m_clientHandle;
	std::vector<DaRequest*> requests(aCount, NULL);
	pRoot->m_elementsJanitor.lock();

	for (int i = 0; i < aCount; i++)
	{
		DaAddressSpaceElement* element = (DaAddressSpaceElement*)pRoot->getElementFromArray(aRequestDataList[i].m_object.m_userData);
		DaRequest* request =  pCreator->createRequest(
								  EnumTransactionType_WRITE,
								  sessionHandle,
								  element,
								  aRequestDataList[i].m_propertyID,
								  aRequestDataList[i].m_requestHandle);
		//  setup the write valueQT
		ValueQT writeValue(*pValues[i].m_pValue, (EnumQuality)pValues[i].m_quality, DateTime(&pValues[i].m_timestamp));
		request->setValue(writeValue);
		requests[i] = request;
	}   //  end for

	pRoot->m_elementsJanitor.unlock();
	DaTransaction* pTransaction = pCreator->createTransaction(
									  EnumTransactionType_WRITE,
									  requests,
									  sessionHandle);

	if (pTransaction == NULL)
	{
		return EnumResultCode_E_OUTOFMEMORY;
	}   //  end if

	Application::Instance()->addTransaction(pTransaction->getKey(), pTransaction);
	return pTransaction->handleWriteRequests();
}   //  end handleWriteRequests


//-----------------------------------------------------------------------------
//	handleRequestCompletedInternally
//
long OTSGlobals::handleRequestCompletedInternally(
	IN unsigned long requestHandle)
{
	return Application::Instance()->markRequestCompletedInternally(requestHandle);
}   //  end handleRequestCompletedInternally



//-----------------------------------------------------------------------------
//	handleQueryProperties
//
long OTSGlobals::handleQueryProperties(
	IN OTSObjectData* pObjectData,
	IN OTChar* anObjectItemId,
	IN long aPropertyId,
	OUT unsigned long* aPropertyCount,
	OUT OTSPropertyData** pPropertyList)
{
	//  initialise values
	*aPropertyCount = 0;
	*pPropertyList = NULL;
	long result = E_NOTIMPL;
	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
	std::vector<DaProperty*> propertyList;
	tstring objectItemId(anObjectItemId);

	pRoot->m_elementsJanitor.lock();

	if (pObjectData != NULL)
	{
		DaAddressSpaceElement* addressElement = (DaAddressSpaceElement*)pRoot->getElementFromArray(pObjectData->m_userData);

		if (addressElement != NULL)
		{
			result = addressElement->queryProperties(propertyList);
		}   //  end if
	}

	pRoot->m_elementsJanitor.unlock();

	if (propertyList.empty())
	{
		//  try to get the property list using the item ID string
		result = pRoot->queryAddressSpaceElementProperties(objectItemId, propertyList);
	}   //  end if

	if (aPropertyId > 0 && aPropertyId < 7)
	{
		return S_FALSE;
	}   //  end if

	if (SUCCEEDED(result) && !propertyList.empty())
	{
		*aPropertyCount = (unsigned long)propertyList.size();
		*pPropertyList = (OTSPropertyData*)OTAllocateMemory((unsigned long)propertyList.size() * sizeof(OTSPropertyData));

		for (unsigned long i = 0; i < *aPropertyCount; i++)
		{
			(*pPropertyList)[i].m_pid           = propertyList[i]->getId();
			(*pPropertyList)[i].m_name          = OTSGlobals::allocateOTBString(propertyList[i]->getName());
			(*pPropertyList)[i].m_itemID        = OTSGlobals::allocateOTBString(propertyList[i]->getItemId());
			(*pPropertyList)[i].m_descr         = OTSGlobals::allocateOTBString(propertyList[i]->getDescription());
			(*pPropertyList)[i].m_datatype      = (unsigned short)propertyList[i]->getDatatype();
			(*pPropertyList)[i].m_accessRights  = (unsigned char)propertyList[i]->getAccessRights();
		}   //  end foreach

		result = S_OK;
	}   //  end if

	return result;
}   //  end handleQueryProperties


//-----------------------------------------------------------------------------
//	handleQueryAddressSpaceElementData
//
long OTSGlobals::handleQueryAddressSpaceElementData(
	IN OTChar* aPath,
	IN unsigned char anElementType,
	OUT OTSAddressSpaceElementData* pData)
{
	if (pData == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType)anElementType;
	AddressSpaceElement* element = NULL;
	long result = E_FAIL;
	tstring path(aPath);

	switch (elementType)
	{
	case EnumAddressSpaceElementType_DA:
		{
			DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
			result = pRoot->queryAddressSpaceElementData(path, element);

			if (SUCCEEDED(result))
			{
				DaAddressSpaceElement* daElement = (DaAddressSpaceElement*)element;

				if (daElement != NULL)
				{
					pData->m_accessRights = (unsigned char)daElement->getAccessRights();
					pData->m_datatype = (unsigned short)daElement->getDatatype();
					pData->m_hasChildren = (unsigned char)daElement->getHasChildren();
					pData->m_isBrowsable = (unsigned char)daElement->getIsBrowsable();
					pData->m_ioMode = (unsigned char)daElement->getIoMode();
					pData->m_userData = daElement->getUserData();
					pData->m_itemID = OTSGlobals::allocateOTBString(daElement->getItemId());
					pData->m_name = OTSGlobals::allocateOTBString(daElement->getName());
				}
				else
				{
					result = EnumResultCode_E_OUTOFMEMORY;
				}   //  end if ... else
			}   //  end if
		}
		break;

	case EnumAddressSpaceElementType_AE:
		{
			AeAddressSpaceRoot* pRoot = Application::Instance()->getAeAddressSpaceRoot();
			result = pRoot->queryAddressSpaceElementData(path, element);

			if (SUCCEEDED(result))
			{
				AeAddressSpaceElement* aeElement = (AeAddressSpaceElement*)element;

				if (aeElement != NULL)
				{
					pData->m_name = OTSGlobals::allocateOTBString(aeElement->getName());
					pData->m_hasChildren = (unsigned char)aeElement->getHasChildren();
					pData->m_isBrowsable = (unsigned char)aeElement->getIsBrowsable();
					pData->m_userData = aeElement->getUserData();
				}
				else
				{
					result = EnumResultCode_E_OUTOFMEMORY;
				}   //  end if
			}   //  end if
		}
		break;

	default:
		Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
			_T("Application::handleQueryAddressSpaceElementData"), _T("Invalid element type received"));
		break;
	}   //  end switch

	//  clean the element allocated
	if (element != NULL)
	{
		delete element;
	}   //  end if

	return result;
}   //  end handleQueryAddressSpaceElementData

//-----------------------------------------------------------------------------
//	handleQueryAddressSpaceElementDataEx
//
long OTSGlobals::handleQueryAddressSpaceElementDataEx(
	IN OTChar* aPath,
	IN unsigned char anElementType,
	IN unsigned short& elementDataFilter,
	OUT OTSAddressSpaceElementData* pData)
{
	if (pData == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	if (elementDataFilter == 0)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType)anElementType;
	AddressSpaceElement* element = NULL;
	long result = E_FAIL;
	tstring path(aPath);

	switch (elementType)
	{
	case EnumAddressSpaceElementType_DA:
		{
			DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
			EnumAddressSpaceElementData elementDataFilterEx = (EnumAddressSpaceElementData)elementDataFilter;
			result = pRoot->queryAddressSpaceElementDataEx(path, elementDataFilterEx, element);

			if (SUCCEEDED(result))
			{
				elementDataFilter = (unsigned short)elementDataFilterEx;
				DaAddressSpaceElement* daElement = (DaAddressSpaceElement*)element;

				if (daElement != NULL)
				{
					pData->m_accessRights = (unsigned char)daElement->getAccessRights();
					pData->m_datatype = (unsigned short)daElement->getDatatype();
					pData->m_hasChildren = (unsigned char)daElement->getHasChildren();
					pData->m_isBrowsable = (unsigned char)daElement->getIsBrowsable();
					pData->m_ioMode = (unsigned char)daElement->getIoMode();
					pData->m_userData = daElement->getUserData();
					if ((elementDataFilter & EnumElementData_ITEMID) == EnumElementData_ITEMID)
					{
						pData->m_itemID = OTSGlobals::allocateOTBString(daElement->getItemId());
					}
					else
					{
						pData->m_itemID = NULL;
					}
					if ((elementDataFilter & EnumElementData_NAME) == EnumElementData_NAME)
					{
						pData->m_name = OTSGlobals::allocateOTBString(daElement->getName());
					}
					else
					{
						pData->m_name = NULL;
					}
				}
				else
				{
					result = EnumResultCode_E_OUTOFMEMORY;
				}   //  end if ... else
			}   //  end if
		}
		break;

	case EnumAddressSpaceElementType_AE:
		{
			AeAddressSpaceRoot* pRoot = Application::Instance()->getAeAddressSpaceRoot();
			EnumAddressSpaceElementData elementDataFilterEx = (EnumAddressSpaceElementData)elementDataFilter;
			result = pRoot->queryAddressSpaceElementDataEx(path, elementDataFilterEx, element);

			if (SUCCEEDED(result))
			{
				elementDataFilter = (unsigned short)elementDataFilterEx;
				AeAddressSpaceElement* aeElement = (AeAddressSpaceElement*)element;

				if (aeElement != NULL)
				{
					if ((elementDataFilter & EnumElementData_NAME) == EnumElementData_NAME)
					{
						pData->m_name = OTSGlobals::allocateOTBString(aeElement->getName());
					}
					else
					{
						pData->m_name = NULL;
					}
					pData->m_hasChildren = (unsigned char)aeElement->getHasChildren();
					pData->m_isBrowsable = (unsigned char)aeElement->getIsBrowsable();
					pData->m_userData = aeElement->getUserData();
				}
				else
				{
					result = EnumResultCode_E_OUTOFMEMORY;
				}   //  end if
			}   //  end if
		}
		break;

	default:
		Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
			_T("Application::handleQueryAddressSpaceElementDataEx"), _T("Invalid element type received"));
		break;
	}   //  end switch

	//  clean the element allocated
	if (element != NULL)
	{
		delete element;
	}   //  end if

	return result;
}   //  end handleQueryAddressSpaceElementDataEx


//-----------------------------------------------------------------------------
//	handleQueryAddressSpaceElementChildren
//
long OTSGlobals::handleQueryAddressSpaceElementChildren(
	IN OTChar* aPath,
	IN unsigned char anElementType,
	OUT unsigned long* pCount,
	OUT OTSAddressSpaceElementData** pChildrenList)
{
	*pCount = 0;
	*pChildrenList = NULL;
	EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType)anElementType;
	std::vector<AddressSpaceElement*> children;
	long result = E_FAIL;
	tstring path(aPath);

	switch (elementType)
	{
	case EnumAddressSpaceElementType_DA:
	{
		DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
		result = pRoot->queryAddressSpaceElementChildren(path, children);

		if (SUCCEEDED(result) && !children.empty())
		{
			*pCount = (unsigned long)children.size();
			*pChildrenList = (OTSAddressSpaceElementData*)OTAllocateMemory((*pCount) * sizeof(OTSAddressSpaceElementData));

			if (*pChildrenList != NULL)
			{
				for (unsigned long i = 0; i < *pCount; i++)
				{
					DaAddressSpaceElement* daElement = (DaAddressSpaceElement*)children[i];

					if (daElement != NULL)
					{
						(*pChildrenList)[i].m_elementType = EnumAddressSpaceElementType_DA;
						(*pChildrenList)[i].m_accessRights = (unsigned char)daElement->getAccessRights();
						(*pChildrenList)[i].m_datatype = (unsigned short)daElement->getDatatype();
						(*pChildrenList)[i].m_hasChildren = (unsigned char)daElement->getHasChildren();
						(*pChildrenList)[i].m_isBrowsable = (unsigned char)daElement->getIsBrowsable();
						(*pChildrenList)[i].m_ioMode = (unsigned char)daElement->getIoMode();
						(*pChildrenList)[i].m_itemID = OTSGlobals::allocateOTBString(daElement->getItemId());
						(*pChildrenList)[i].m_name = OTSGlobals::allocateOTBString(daElement->getName());
						(*pChildrenList)[i].m_userData = daElement->getUserData();
					}   //  end if
				}   //  end for
			}
			else
			{
				result = EnumResultCode_E_OUTOFMEMORY;
			}   //  end if ... else
		}   //  end if
	}
	break;

	case EnumAddressSpaceElementType_AE:
	{
		AeAddressSpaceRoot* pRoot = Application::Instance()->getAeAddressSpaceRoot();
		result = pRoot->queryAddressSpaceElementChildren(path, children);

		if (SUCCEEDED(result) && !children.empty())
		{
			*pCount = (unsigned long)children.size();
			*pChildrenList = (OTSAddressSpaceElementData*)OTAllocateMemory((*pCount) * sizeof(OTSAddressSpaceElementData));

			if (*pChildrenList != NULL)
			{
				for (unsigned long i = 0; i < *pCount; i++)
				{
					AeAddressSpaceElement* aeElement = (AeAddressSpaceElement*)children[i];

					if (aeElement != NULL)
					{
						(*pChildrenList)[i].m_elementType = EnumAddressSpaceElementType_AE;
						(*pChildrenList)[i].m_name = OTSGlobals::allocateOTBString(aeElement->getName());
						(*pChildrenList)[i].m_hasChildren = (unsigned char)aeElement->getHasChildren();
						(*pChildrenList)[i].m_isBrowsable = (unsigned char)aeElement->getIsBrowsable();
						(*pChildrenList)[i].m_userData = aeElement->getUserData();
					}   //  end if
				}   //  end for
			}
			else
			{
				result = EnumResultCode_E_OUTOFMEMORY;
			}   //  end if ... else
		}   //  end if ... else
	}
	break;

	default:
		Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
									   _T("Application::handleQueryAddressSpaceElementChildren"), _T("Invalid element type received"));
		break;
	}   //  end switch

	for (unsigned long index = 0; index < children.size(); index++)
	{
		if (children[index] != NULL)
		{
			delete children[index];
			children[index] = NULL;
		}   //  end if
	}   //  end for

	return result;
}   //  end handleQueryAddressSpaceElementChildren


//-----------------------------------------------------------------------------
//	handleChangeSessionState
//
long OTSGlobals::handleChangeSessionState(
	IN OTSObjectHandle aSessionHandle,
	IN OTSSessionData* aSessionStateData)
{
	if (aSessionHandle == 0 || aSessionStateData == 0)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	DaSession* pSession = NULL;
	Creator* pCreator = Application::Instance()->getCreator();

	switch (aSessionStateData->m_state)
	{
	case EnumSessionState_CREATE:
		pSession = pCreator->createSession((EnumSessionType)aSessionStateData->m_type,
										   aSessionHandle);

		if (pSession != NULL)
		{
			if (aSessionStateData->m_clientName != NULL)
			{
				pSession->m_clientName = aSessionStateData->m_clientName;
				pSession->handleClientNameChanged();
			}   //  end if

			Application::Instance()->addSession(pSession, aSessionHandle);
			pSession->connectSession();
			return S_OK;
		}   //  end if

		break;

	case EnumSessionState_MODIFY:
		Application::Instance()->m_sessionListJanitor.lock();
		pSession = Application::Instance()->getSession(aSessionHandle);

		if (pSession != NULL)
		{
			if (aSessionStateData->m_clientName != NULL)
			{
				pSession->m_clientName = aSessionStateData->m_clientName;
				pSession->handleClientNameChanged();
			}   //  end if
		}   //  end if

		Application::Instance()->m_sessionListJanitor.unlock();
		return S_OK;
		break;

	case EnumSessionState_DESTROY:
		Application::Instance()->m_sessionListJanitor.lock();
		pSession = Application::Instance()->getSession(aSessionHandle);

		if (pSession != NULL)
		{
			pSession->disconnectSession();
			Application::Instance()->removeSession(aSessionHandle);
			Application::Instance()->m_sessionListJanitor.unlock();
			return S_OK;
		}   //  end if

		Application::Instance()->m_sessionListJanitor.unlock();
		break;

	case EnumSessionState_LOGOFF:
		Application::Instance()->m_sessionListJanitor.lock();
		pSession = Application::Instance()->getSession(aSessionHandle);

		if (pSession != NULL)
		{
			long result = pSession->logoff();
			Application::Instance()->m_sessionListJanitor.unlock();
			return result;
		}   //  end if

		Application::Instance()->m_sessionListJanitor.unlock();
		break;

	case EnumSessionState_LOGON:
		Application::Instance()->m_sessionListJanitor.lock();
		pSession = Application::Instance()->getSession(aSessionHandle);

		if (pSession != NULL)
		{
			tstring userName(aSessionStateData->m_userName);
			tstring sessionStateData(aSessionStateData->m_password);
			long result = pSession->logon(userName, sessionStateData);
			Application::Instance()->m_sessionListJanitor.unlock();
			return result;
		}   //  end if

		Application::Instance()->m_sessionListJanitor.unlock();
		break;

	default:
		Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
									   _T("Application.handleChangeSessionState"), _T("Invalid Session state received"));
		break;
	}   //  end switch

	return EnumResultCode_E_FAIL;
}   //  end handleChangeSessionState


//-----------------------------------------------------------------------------
//	handleQueryCacheValue
//
long OTSGlobals::handleQueryCacheValue(
	IN OTSObjectHandle aSessionHandle,
	IN OTSObjectData anObjectData,
	OUT OTSValueData* pValue)
{
	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
	pRoot->m_elementsJanitor.lock();
	DaAddressSpaceElement* daElement = (DaAddressSpaceElement*)pRoot->getElementFromArray(anObjectData.m_userData);
	ValueQT valueData;

	if (daElement == NULL)
	{
		pRoot->m_elementsJanitor.unlock();
		return EnumResultCode_E_OPC_NOTFOUND;
	}   //  end if

	//  get the associated session
	Application::Instance()->m_sessionListJanitor.lock();
	DaSession* session = Application::Instance()->getSession(aSessionHandle);
	long result = daElement->queryCacheValue(session, valueData);

	if (SUCCEEDED(result))
	{
		if (pValue == NULL)
		{
			pValue = (OTSValueData*)OTAllocateMemory(sizeof(OTSValueData));
		}   //  end if

		pValue->m_quality = (unsigned short)valueData.getQuality();
		pValue->m_timestamp = valueData.getTimeStamp().get();

		if (pValue->m_pValue == NULL)
		{
			pValue->m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		}//  end if
		
		valueData.getData().copyTo(pValue->m_pValue);
	}   //  end if

	Application::Instance()->m_sessionListJanitor.unlock();
	pRoot->m_elementsJanitor.unlock();
	return result;
}   //  end handleQueryCacheValue


//-----------------------------------------------------------------------------
//	handleChangeItems
//
long OTSGlobals::handleChangeItems(
	IN long aCount,
	IN OTSItemData* pItemDataList)
{
	if (pItemDataList == NULL || aCount == 0)
	{
		return S_FALSE;
	}   //  end if

	BOOL errorFound = FALSE;
	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
	std::vector<DaAddressSpaceElement*> elements;

	pRoot->m_elementsJanitor.lock();
	for (long i = 0; i < aCount; i++)
	{
		DaAddressSpaceElement* element = (DaAddressSpaceElement*)pRoot->getElementFromArray(pItemDataList[i].m_object.m_userData);

		if (element != NULL)
		{
			element->change(pItemDataList[i].m_active != 0, pItemDataList[i].m_sampleRate);
			elements.push_back(element);
		}
		else
		{
			errorFound = TRUE;
		}   //  end if ... else

	}   //  end for
	pRoot->m_elementsJanitor.unlock();

	//  trigger the activation state event
	pRoot->activationStateUpdated(elements);
	return (errorFound ? E_FAIL : S_OK);
}   //  end handleChangeItems


//-----------------------------------------------------------------------------
//	handleCreateAddressSpaceElement
//
long OTSGlobals::handleCreateAddressSpaceElement(
	IN OTSObjectHandle hParent,
	IN OTSObjectHandle hObject,
	IN OTSAddressSpaceElementData* aDataIn,
	OUT OTSAddressSpaceElementData* aDataOut)
{
	if (aDataIn == NULL || aDataOut == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType)aDataIn->m_elementType;
	tstring name(aDataIn->m_name);
	Creator* pCreator = Application::Instance()->getCreator();

	switch (elementType)
	{
	case EnumAddressSpaceElementType_DA:
	{
		DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
		tstring itemId(aDataIn->m_itemID);
		DaAddressSpaceElement* daElement = NULL;
		//  Check if the user data was already added
		pRoot->m_elementsJanitor.lock();
		daElement = (DaAddressSpaceElement*)pRoot->getElementFromArray(aDataIn->m_userData);

		if (daElement != NULL)
		{
			daElement->setItemId(itemId);
			aDataOut->m_userData = daElement->getUserData();
			pRoot->m_elementsJanitor.unlock();
			return S_OK;
		}   //  end if

		daElement = pCreator->createInternalDaAddressSpaceElement(itemId, name, aDataIn->m_userData, hObject, hParent);

		if (daElement != NULL)
		{
			daElement->setHasChildren(aDataIn->m_hasChildren);
			daElement->setIsBrowsable(aDataIn->m_isBrowsable);
			daElement->setAccessRights((EnumAccessRights)aDataIn->m_accessRights);
			daElement->setDatatype((VARENUM)aDataIn->m_datatype);
			daElement->setIoMode((EnumIoMode)aDataIn->m_ioMode);
			AddressSpaceElement* daParent = pRoot->getParent(hObject);

			if (daParent != NULL)
			{
				//  Fill in the out structure
				aDataOut->m_accessRights = (unsigned char)daElement->getAccessRights();
				aDataOut->m_datatype = (unsigned short)daElement->getDatatype();
				aDataOut->m_elementType = EnumAddressSpaceElementType_DA;
				aDataOut->m_hasChildren = (unsigned char)daElement->getHasChildren();
				aDataOut->m_isBrowsable = (unsigned char)daElement->getIsBrowsable();
				aDataOut->m_ioMode = (unsigned char)daElement->getIoMode();
				aDataOut->m_itemID = NULL;
				aDataOut->m_name = NULL;
				aDataOut->m_userData = daElement->getUserData();
				aDataIn->m_userData = daElement->getUserData();
				daParent->addChild(daElement);
				pRoot->m_elementsJanitor.unlock();
				return S_OK;
			}
			else
			{
				pRoot->m_elementsJanitor.unlock();
				return E_FAIL;
			}   //  end if ... else
		}   //  end if

		pRoot->m_elementsJanitor.unlock();
		return E_FAIL;
	}
	break;

	case EnumAddressSpaceElementType_AE:
	{
		AeAddressSpaceRoot* pRoot = Application::Instance()->getAeAddressSpaceRoot();
		AeAddressSpaceElement* aeElement = NULL;
		//  Check if the user data was already added
		pRoot->m_elementsJanitor.lock();
		aeElement = (AeAddressSpaceElement*)pRoot->getElementFromArray(aDataIn->m_userData);

		if (aeElement != NULL)
		{
			aDataOut->m_userData = aDataIn->m_userData;
			pRoot->m_elementsJanitor.unlock();
			return S_OK;
		}   //  end if

		aeElement = pCreator->createInternalAeAddressSpaceElement(name, aDataIn->m_userData, hObject, hParent);

		if (aeElement != NULL)
		{
			aeElement->setHasChildren(aDataIn->m_hasChildren);
			AddressSpaceElement* aeParent = pRoot->getParent(hObject);

			if (aeParent != NULL)
			{
				//  Fill in the out structure
				aDataOut->m_accessRights = 0;
				aDataOut->m_datatype = VT_EMPTY;
				aDataOut->m_elementType = EnumAddressSpaceElementType_AE;
				aDataOut->m_hasChildren = (unsigned char)aeElement->getHasChildren();
				aDataOut->m_isBrowsable = (unsigned char)aeElement->getIsBrowsable();
				aDataOut->m_ioMode = 0;
				aDataOut->m_itemID = NULL;
				aDataOut->m_name = NULL;
				aDataOut->m_userData = aeElement->getUserData();
				pRoot->m_elementsJanitor.unlock();
				return S_OK;
			}
			else
			{
				pRoot->m_elementsJanitor.unlock();
				return E_FAIL;
			}   //  end if ... else
		}   //  end if

		pRoot->m_elementsJanitor.unlock();
		return E_FAIL;
	}
	break;

	default:
		break;
	}   //  end switch

	return EnumResultCode_E_UNEXPECTED;
}   //  end handleCreateAddressSpaceElement


//-----------------------------------------------------------------------------
//	handleCreateAddressSpaceElementEx (DA Only)
//
long OTSGlobals::handleCreateAddressSpaceElementEx(
	IN OTSObjectHandle hParent,
	IN OTSObjectHandle hObject,
	IN OTSAddressSpaceElementData* aDataIn,
	unsigned short elementDataFilter,
	OUT OTSAddressSpaceElementData* aDataOut)
{
	if (aDataIn == NULL || aDataOut == NULL)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType)aDataIn->m_elementType;

	if (elementType == EnumAddressSpaceElementType_AE)
	{
		return EnumResultCode_E_UNEXPECTED;
	}   //  end if

	tstring name(aDataIn->m_name);
	Creator* pCreator = Application::Instance()->getCreator();

	DaAddressSpaceRoot* pRoot = Application::Instance()->getDaAddressSpaceRoot();
	tstring itemId(aDataIn->m_itemID);
	DaAddressSpaceElement* daElement = NULL;
	//  Check if the user data was already added
	pRoot->m_elementsJanitor.lock();
	daElement = (DaAddressSpaceElement*)pRoot->getElementFromArray(aDataIn->m_userData);

	if (daElement != NULL)
	{
		daElement->setItemId(itemId);
		aDataOut->m_userData = daElement->getUserData();
		pRoot->m_elementsJanitor.unlock();
		return S_OK;
	}   //  end if

	daElement = pCreator->createInternalDaAddressSpaceElementEx(itemId, name, aDataIn->m_userData, hObject, hParent, (EnumAddressSpaceElementData) elementDataFilter,
		(EnumAccessRights) aDataIn->m_accessRights, (EnumIoMode) aDataIn->m_ioMode, aDataIn->m_hasChildren != 0, aDataIn->m_isBrowsable != 0, (VARENUM) aDataIn->m_datatype);

	if (daElement != NULL)
	{
		daElement->setHasChildren(aDataIn->m_hasChildren);
		daElement->setIsBrowsable(aDataIn->m_isBrowsable);
		daElement->setAccessRights((EnumAccessRights)aDataIn->m_accessRights);
		daElement->setDatatype((VARENUM)aDataIn->m_datatype);
		daElement->setIoMode((EnumIoMode)aDataIn->m_ioMode);
		AddressSpaceElement* daParent = pRoot->getParent(hObject);

		if (daParent != NULL)
		{
			//  Fill in the out structure
			aDataOut->m_accessRights = (unsigned char)daElement->getAccessRights();
			aDataOut->m_datatype = (unsigned short)daElement->getDatatype();
			aDataOut->m_elementType = EnumAddressSpaceElementType_DA;
			aDataOut->m_hasChildren = (unsigned char)daElement->getHasChildren();
			aDataOut->m_isBrowsable = (unsigned char)daElement->getIsBrowsable();
			aDataOut->m_ioMode = (unsigned char)daElement->getIoMode();
			aDataOut->m_itemID = NULL;
			aDataOut->m_name = NULL;
			aDataOut->m_userData = daElement->getUserData();
			aDataIn->m_userData = daElement->getUserData();
			daParent->addChild(daElement);
			pRoot->m_elementsJanitor.unlock();
			return S_OK;
		}
		else
		{
			pRoot->m_elementsJanitor.unlock();
			return E_FAIL;
		}   //  end if ... else
	}   //  end if

	pRoot->m_elementsJanitor.unlock();
	return E_NOTIMPL;
}   //  end handleCreateAddressSpaceElement


//-----------------------------------------------------------------------------
//	handleDestroyAddressSpaceElement
//
long OTSGlobals::handleDestroyAddressSpaceElement(IN OTSObjectData anObjectData)
{
	DaAddressSpaceRoot* pDaRoot = Application::Instance()->getDaAddressSpaceRoot();
	pDaRoot->m_elementsJanitor.lock();
	AddressSpaceElement* element = pDaRoot->getElementFromArray(anObjectData.m_userData);

	if (element != NULL)
	{
		element->setObjectHandle(0);
		pDaRoot->removeElementFromArray(element);
		element->triggerRemovedFromAddressSpace();
		delete element;
		pDaRoot->m_elementsJanitor.unlock();
		return S_OK;
	}   //  end if

	pDaRoot->m_elementsJanitor.unlock();
	//  chech in AE address space
	AeAddressSpaceRoot* pAeRoot = Application::Instance()->getAeAddressSpaceRoot();
	pAeRoot->m_elementsJanitor.lock();
	element = pAeRoot->getElementFromArray(anObjectData.m_userData);

	if (element != NULL)
	{
		element->setObjectHandle(0);
		pAeRoot->removeElementFromArray(element);
		element->triggerRemovedFromAddressSpace();
		delete element;
		pAeRoot->m_elementsJanitor.unlock();
		return S_OK;
	}   //  end if

	pAeRoot->m_elementsJanitor.unlock();

	Application::Instance()->trace(EnumTraceLevel_INF, EnumTraceGroup_OPCSERVER, _T("Application::handleDestroyAddressSpaceElement"),
								   _T("Element not found"));
	return S_FALSE;
}   //  end handleDestroyAddressSpaceElement


//-----------------------------------------------------------------------------
//	handleQueryConditions
//
long OTSGlobals::handleQueryConditions(
	IN OTSObjectData* pObjectData,
	IN OTChar* aSourcePath,
	OUT unsigned long* pConditionCount,
	OUT OTChar***  pConditionNames)
{
	tstring sourcePath(aSourcePath);
	*pConditionCount = 0;
	*pConditionNames = NULL;
	long result = E_FAIL;

	if (pObjectData != NULL)
	{
		std::vector<tstring> conditionNames;
		result = Application::Instance()->m_AeAddressSpaceRoot->queryConditions(
					 pObjectData->m_userData,
					 sourcePath,
					 conditionNames);

		if (SUCCEEDED(result))
		{
			*pConditionCount = (unsigned long)conditionNames.size();

			if (*pConditionCount != 0)
			{
				*pConditionNames = (OTChar**)OTAllocateMemory((*pConditionCount) * sizeof(OTChar*));

				for (unsigned long index = 0; index < (*pConditionCount); index++)
				{
					//  Allocate the string
					(*pConditionNames)[index] = OTSGlobals::allocateOTBString(conditionNames[index]);
				}   //  end for
			}   //  end if
		}   //  end if
	}   //  end if

	return result;
}   //  end handleQueryConditions


//-----------------------------------------------------------------------------
//	handleAcknowledgeCondition
//
long OTSGlobals::handleAcknowledgeCondition(
	IN OTSObjectData aConditionData,
	IN OTChar*  anAckId,
	OTChar*  anAckComment)
{
	std::map<unsigned long, AeCondition*>::const_iterator elementIterator;
	Application::Instance()->m_conditionListJanitor.lock();
	elementIterator = Application::Instance()->m_conditionList.find(aConditionData.m_objectHandle);

	if (elementIterator == Application::Instance()-> m_conditionList.end())
	{
		Application::Instance()->m_conditionListJanitor.unlock();
		return E_FAIL;
	}   //  end if

	AeCondition* condition = elementIterator->second;

	if (condition == NULL)
	{
		Application::Instance()->m_conditionListJanitor.unlock();
		return E_FAIL;
	}   //  end if

	DateTime now;
	now.now();
	tstring anAckIdStr = tstring(anAckId);
	tstring anAckCommentStr = tstring(anAckComment);
	condition->acknowledge(anAckIdStr, anAckCommentStr, now);
	long result = condition->change();
	Application::Instance()->m_conditionListJanitor.unlock();
	return result;
}   //  end handleAcknowledgeCondition


//-----------------------------------------------------------------------------
//	handleQueryConditionDefinition
//
long OTSGlobals::handleQueryConditionDefinition(
	OTSObjectData aConditionData,
	OTSConditionDefinitionData* pConditionDefData)
{
	std::map<unsigned long, AeCondition*>::const_iterator elementIterator;
	Application::Instance()->m_conditionListJanitor.lock();
	elementIterator = Application::Instance()->m_conditionList.find(aConditionData.m_objectHandle);

	if (elementIterator == Application::Instance()->m_conditionList.end())
	{
		Application::Instance()->m_conditionListJanitor.unlock();
		return E_FAIL;
	}   //  end if

	AeCondition* condition = elementIterator->second;

	if (condition == NULL)
	{
		Application::Instance()->m_conditionListJanitor.unlock();
		return E_FAIL;
	}   //  end if

	AeConditionDefinition* conditionDefinition = condition->getDefinition();

	//  check if condition definition found
	if (conditionDefinition == NULL)
	{
		Application::Instance()->m_conditionListJanitor.unlock();
		return S_FALSE;
	}   //  end if

	if (pConditionDefData == NULL)
	{
		pConditionDefData = (OTSConditionDefinitionData*)OTAllocateMemory(sizeof(OTSConditionDefinitionData));

		if (pConditionDefData == NULL)
		{
			Application::Instance()->m_conditionListJanitor.unlock();
			return EnumResultCode_E_OUTOFMEMORY;
		}   //  end if
	}   //  end if

	long result = fillOTSConditionDefinitionData(*conditionDefinition, *pConditionDefData);
	Application::Instance()->m_conditionListJanitor.unlock();
	return result;
}   //  end handleQueryConditionDefinition


//-----------------------------------------------------------------------------
//	handleEnableConditions
//
long OTSGlobals::handleEnableConditions(
	unsigned char enable,
	OTChar* addressSpaceElementName)
{
	tstring path(addressSpaceElementName);
	AeCondition* condition = NULL;
	std::map<unsigned long, AeCondition*>::iterator elementIterator;
	unsigned long result = S_OK;
	bool found = false;
	Application::Instance()->m_conditionListJanitor.lock();

	for (elementIterator =  Application::Instance()->m_conditionList.begin();
		 elementIterator !=  Application::Instance()->m_conditionList.end();
		 elementIterator++)
	{
		condition = elementIterator->second;

		if (condition != NULL)
		{
			if (path == condition->getSourcePath())
			{
				found = true;

				if (enable != 0)
				{
					result |= condition->enable(path);
				}
				else
				{
					result |= condition->disable(path);
				}   //  end if ... else
			}   //  end if
		}   //  end if
	}   //  end for

	Application::Instance()->m_conditionListJanitor.unlock();
	if (!found)
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	return result;
}   //  end handleEnableConditions


//-----------------------------------------------------------------------------
//	handleWebTemplate
//
long OTSGlobals::handleWebTemplate(
	IN OTChar*  templateName,
	IN unsigned long numArgs,
	IN OTChar*  *pArgs,
	OUT OTChar*  *pResult)
{
	WebTemplate* webTemplate = Application::Instance()->getWebTemplate();

	if (webTemplate == NULL)
	{
		return E_NOTIMPL;
	}   //  end if

	tstring resultString;
	long result = webTemplate->handleWebTemplate(templateName, numArgs, pArgs, resultString);

	if (SUCCEEDED(result))
	{
		(*pResult) = allocateOTBString(resultString);
	}   //  end if

	return result;
}   //  end handleWebTemplate


//-----------------------------------------------------------------------------
//	allocateOTBString
//
OTChar* OTSGlobals::allocateOTBString(tstring& aValue)
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


//-----------------------------------------------------------------------------
//	advise
//
long OTSGlobals::advise()
{
	m_OTNSCallBacks.m_OTOnTrace = &handleTrace;
	m_OTNSCallBacks.m_OTSShutdown = &handleShutdown;
	m_OTNSCallBacks.m_OTSHandleReadRequests = &handleReadRequests;
	m_OTNSCallBacks.m_OTSHandleWriteRequests = &handleWriteRequests;
	m_OTNSCallBacks.m_OTSRequestCompletedInternally = &handleRequestCompletedInternally;
	m_OTNSCallBacks.m_OTSQueryProperties = &handleQueryProperties;
	m_OTNSCallBacks.m_OTSCreateAddressSpaceElement = &handleCreateAddressSpaceElement;
	m_OTNSCallBacks.m_OTSCreateAddressSpaceElementEx = &handleCreateAddressSpaceElementEx;
	m_OTNSCallBacks.m_OTSDestroyAddressSpaceElement = &handleDestroyAddressSpaceElement;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementChildren = &handleQueryAddressSpaceElementChildren;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementData = &handleQueryAddressSpaceElementData;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementDataEx = &handleQueryAddressSpaceElementDataEx;
	m_OTNSCallBacks.m_OTSChangeSessionState = &handleChangeSessionState;
	m_OTNSCallBacks.m_OTSChangeItems = &handleChangeItems;
	m_OTNSCallBacks.m_OTSQueryCacheValue = &handleQueryCacheValue;
	m_OTNSCallBacks.m_OTSQueryConditions = &handleQueryConditions;
	m_OTNSCallBacks.m_OTSAcknowledgeCondition = &handleAcknowledgeCondition;
	m_OTNSCallBacks.m_OTSQueryConditionDefinition = &handleQueryConditionDefinition;
	m_OTNSCallBacks.m_OTSEnableConditions = &handleEnableConditions;
	m_OTNSCallBacks.m_OTSWebHandleTemplate = &handleWebTemplate;
	return OTSAdvise(&m_OTNSCallBacks);
}   //  end advise

//-----------------------------------------------------------------------------
//	unadvise
//
long OTSGlobals::unadvise()
{
	m_OTNSCallBacks.m_OTOnTrace = NULL;
	m_OTNSCallBacks.m_OTSShutdown = NULL;
	m_OTNSCallBacks.m_OTSHandleReadRequests = NULL;
	m_OTNSCallBacks.m_OTSHandleWriteRequests = NULL;
	m_OTNSCallBacks.m_OTSRequestCompletedInternally = NULL;
	m_OTNSCallBacks.m_OTSQueryProperties = NULL;
	m_OTNSCallBacks.m_OTSCreateAddressSpaceElement = NULL;
	m_OTNSCallBacks.m_OTSDestroyAddressSpaceElement = NULL;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementChildren = NULL;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementData = NULL;
	m_OTNSCallBacks.m_OTSQueryAddressSpaceElementDataEx = NULL;
	m_OTNSCallBacks.m_OTSChangeSessionState = NULL;
	m_OTNSCallBacks.m_OTSChangeItems = NULL;
	m_OTNSCallBacks.m_OTSQueryCacheValue = NULL;
	m_OTNSCallBacks.m_OTSQueryConditions = NULL;
	m_OTNSCallBacks.m_OTSAcknowledgeCondition = NULL;
	m_OTNSCallBacks.m_OTSQueryConditionDefinition = NULL;
	m_OTNSCallBacks.m_OTSEnableConditions = NULL;
	m_OTNSCallBacks.m_OTSWebHandleTemplate = NULL;
	return OTSAdvise(&m_OTNSCallBacks);
}   //  end unadvise
