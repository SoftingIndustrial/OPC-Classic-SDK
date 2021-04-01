//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                            OPC Toolkit - OTC                               |
//                                                                            |
//  Filename    : OTC.cpp                                                     |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Programming interface of OPC Toolkit C Client (OTC)         |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifndef SOFEATURE_DCOM
#ifndef COINIT_MULTITHREADED
#define COINIT_MULTITHREADED 0
#endif
#endif

#ifdef OTB_CLIENT

#include "OTClient.h"
#include "ClientJob.h"
#ifdef OTB_CLIENT_DA
#include "ClientDACreator.h"
#include "ClientDASession.h"
#include "ClientDASubscription.h"
#include "ClientDAItem.h"
#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE
#include "ClientAECreator.h"
#include "ClientAESession.h"
#include "ClientAESubscription.h"
#endif // OTB_CLIENT_AE
#include "OTB.h"
#include "OTC.h"

#include "SOVersion.h"
#include "SOModule.h"
#include "SOCmnHandleManager.h"


void getOTCObjectData(IN OTCObjectHandle objectHandle, OUT OTCObjectData* pObjectData)
{
	SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

	if (!pObject)
	{
		pObjectData->m_objectHandle = 0;
		return;
	}

	pObject->addRef();
	pObjectData->m_objectHandle = objectHandle;
	pObjectData->m_userData = 0;
#ifdef OTB_CLIENT_DA

	if (pObject->is(SOCMNOBJECT_TYPE_ITEM))
	{
		pObjectData->m_userData = ((ClientDAItem*)pObject)->getOTCUserData();
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
	{
		pObjectData->m_userData = ((ClientDASubscription*)pObject)->getOTCUserData();
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA))
	{
		pObjectData->m_userData = ((ClientDASession*)pObject)->getOTCUserData();
	}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

	if (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
	{
		pObjectData->m_userData = ((ClientAESubscription*)pObject)->getOTCUserData();
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))
	{
		pObjectData->m_userData = ((ClientAESession*)pObject)->getOTCUserData();
	}
#endif // OTB_CLIENT_AE

	pObject->release();
}


void getOTCObjectContext(IN OTCObjectHandle objectHandle, OUT OTCObjectContext* pObjectContext)
{
	SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

	if (!pObject)
	{
		pObjectContext->m_objectData.m_objectHandle = 0;
		return;
	}

	pObject->addRef();
	memset(pObjectContext, 0, sizeof(OTCObjectContext));
	pObjectContext->m_objectData.m_objectHandle = objectHandle;
#ifdef OTB_CLIENT_DA

	if (pObject->is(SOCMNOBJECT_TYPE_ITEM))
	{
		pObjectContext->m_objectType = OTC_OBJECTTYPE_DAITEM;
		pObjectContext->m_objectData.m_userData = ((ClientDAItem*)pObject)->getOTCUserData();
		SOCmnPointer<ClientDASubscription> daSubscription = (ClientDASubscription*)((ClientDAItem*)pObject)->getParentBranch();

		if (daSubscription.isNotNull())
		{
			pObjectContext->m_subscriptionData.m_objectHandle = (DWORD)daSubscription->getOpcHandle();
			pObjectContext->m_subscriptionData.m_userData = daSubscription->getOTCUserData();
			SOCmnPointer<ClientDASession> daSession = (ClientDASession*)daSubscription->getParentBranch();

			if (daSession.isNotNull())
			{
				pObjectContext->m_sessionData.m_objectHandle = (DWORD)daSession->getOpcHandle();
				pObjectContext->m_sessionData.m_userData = daSession->getOTCUserData();
			}
		}
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
	{
		pObjectContext->m_objectType = OTC_OBJECTTYPE_DASUBSCRIPTION;
		pObjectContext->m_objectData.m_userData = ((ClientDASubscription*)pObject)->getOTCUserData();
		SOCmnPointer<ClientDASession> daSession = (ClientDASession*)((ClientDASubscription*)pObject)->getParentBranch();

		if (daSession.isNotNull())
		{
			pObjectContext->m_sessionData.m_objectHandle = (DWORD)daSession->getOpcHandle();
			pObjectContext->m_sessionData.m_userData = daSession->getOTCUserData();
		}
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA))
	{
		pObjectContext->m_objectType = OTC_OBJECTTYPE_DASESSION;
		pObjectContext->m_objectData.m_userData = ((ClientDASession*)pObject)->getOTCUserData();
	}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

	if (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))
	{
		pObjectContext->m_objectType = OTC_OBJECTTYPE_AESESSION;
		pObjectContext->m_objectData.m_userData = ((ClientAESession*)pObject)->getOTCUserData();
	}
	else if (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
	{
		pObjectContext->m_objectType = OTC_OBJECTTYPE_AESUBSCRIPTION;
		pObjectContext->m_objectData.m_userData = ((ClientAESubscription*)pObject)->getOTCUserData();
		SOCmnPointer<ClientAESession> aeSession = (ClientAESession*)((ClientAESubscription*)pObject)->getParentBranch();

		if (aeSession.isNotNull())
		{
			pObjectContext->m_sessionData.m_objectHandle = (DWORD)aeSession->getOpcHandle();
			pObjectContext->m_sessionData.m_userData = aeSession->getOTCUserData();
		}
	}
#endif // OTB_CLIENT_AE

	pObject->release();
}


void getOTCObjectsData(IN DWORD objectCount, IN OTCObjectHandle* pObjectHandles, OUT OTCObjectData* pObjectData)
{
	DWORD i;

	for (i = 0; i < objectCount; i++)
	{
		getOTCObjectData(pObjectHandles[i], &pObjectData[i]);
	}
}

#ifdef OTB_CLIENT_DA

BYTE DaCOPCVersionToOTC(WORD dacSpec)
{
	BYTE otcSpec = OT_OPCSPECIFICATION_DEFAULT;

	switch (dacSpec)
	{
	case SODAC_VERSION_OPC_DAV10A:
		otcSpec = OT_OPCSPECIFICATION_DA10;
		break;

	case SODAC_VERSION_OPC_DAV20:
		otcSpec = OT_OPCSPECIFICATION_DA20;
		break;

	case SODAC_VERSION_OPC_DAV30:
		otcSpec = OT_OPCSPECIFICATION_DA30;
		break;

	case SODAC_VERSION_OPC_XMLDAV10:
		otcSpec = OT_OPCSPECIFICATION_XMLDA10;
		break;

	case SODAC_VERSION_OPC_USEDEFAULT:
		otcSpec = OT_OPCSPECIFICATION_DEFAULT;
		break;
	}

	return otcSpec;
}

WORD OTCOPCVersionToDaC(BYTE otcSpec)
{
	WORD dacSpec = SODAC_VERSION_OPC_USEDEFAULT;

	switch (otcSpec)
	{
	case OT_OPCSPECIFICATION_DA10:
		dacSpec = SODAC_VERSION_OPC_DAV10A;
		break;

	case OT_OPCSPECIFICATION_DA20 :
		dacSpec = SODAC_VERSION_OPC_DAV20;
		break;

	case OT_OPCSPECIFICATION_DA30 :
		dacSpec = SODAC_VERSION_OPC_DAV30;
		break;

	case OT_OPCSPECIFICATION_XMLDA10 :
		dacSpec = SODAC_VERSION_OPC_XMLDAV10;
		break;

	case OT_OPCSPECIFICATION_DEFAULT :
		dacSpec = SODAC_VERSION_OPC_USEDEFAULT;
		break;
	}

	return dacSpec;
}

#endif // OTB_CLIENT_DA

#ifdef SOOS_WINDOWS
extern HINSTANCE g_instance;
#endif
OTCCallbackFunctions g_callbackFunctions;

#ifdef SOOS_WINDOWS
SOCmnString g_serviceName;
SOCmnString g_serviceDescription;
#endif


#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE


//-----------------------------------------------------------------------------
// stopService
//
// - stop the service
//
void stopServiceC(
	IN SC_HANDLE hService) // service handle
{
	SERVICE_STATUS serviceStatus;// current status of the service

	// try to stop the service
	if (ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		while (QueryServiceStatus(hService, &serviceStatus))
		{
			if (serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				Sleep(100);
			}
			else
			{
				break;
			}
		}
	}
} // stopServiceC

//-----------------------------------------------------------------------------
// registerService
//
// - create service entries in registry
//
// returns:
//    TRUE  - registered service
//    FALSE - error during registration
//

BOOL registerServiceC(void)
{
	SC_HANDLE hService;
	SC_HANDLE hSCM;
	TCHAR path[_MAX_PATH];
	BOOL ret = FALSE;

	if (GetModuleFileName(NULL, path, _MAX_PATH) == 0)
	{
		return FALSE;
	}

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		hService = CreateService(hSCM, g_serviceName, g_serviceName,
								 SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
								 SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

		if (hService)
		{
			// installed service
			if (!g_serviceDescription.IsEmpty())
			{
				// set service description
				SERVICE_DESCRIPTION description;
				description.lpDescription = g_serviceDescription;
				ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &description);
			}
			CloseServiceHandle(hService);
			ret = TRUE;
		}
		else
		{
			// error
			if (GetLastError() == ERROR_SERVICE_EXISTS)
			{
				// this service already exsists
				hService = OpenService(hSCM, g_serviceName, SERVICE_ALL_ACCESS);

				if (hService)
				{
					BYTE buffer[1000];
					QUERY_SERVICE_CONFIG* pServiceConfig = (QUERY_SERVICE_CONFIG*)buffer;
					DWORD bn;

					if (QueryServiceConfig(hService, pServiceConfig, 1000, &bn))
					{
						if (_tcscmp(pServiceConfig->lpDisplayName, g_serviceName) == 0)
						{
							// same service -> change the service configuration
							stopServiceC(hService);

							if (ChangeServiceConfig(hService, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
													SERVICE_ERROR_NORMAL, path, NULL, NULL, _T("RPCSS\0"),
													NULL, NULL, g_serviceName))
							{
								if (!g_serviceDescription.IsEmpty())
								{
									// set service description
									SERVICE_DESCRIPTION description;
									description.lpDescription = g_serviceDescription;
									ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &description);
								}
								ret = TRUE;
							}
						}
					}

					CloseServiceHandle(hService);
				}
			}
		}

		CloseServiceHandle(hSCM);
	}

	return ret;
} // registerServiceC


//-----------------------------------------------------------------------------
// unregisterService
//
// - stops service
// - remove service entries in registry
//
// returns:
//    TRUE  - unregistered service
//    FALSE - error during unregistration
//
BOOL unregisterServiceC(void)
{
	SC_HANDLE hService;
	SC_HANDLE hSCM;
	BOOL ret = FALSE;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		hService = OpenService(hSCM, g_serviceName, SERVICE_ALL_ACCESS);

		if (hService)
		{
			stopServiceC(hService);

			// now remove the service
			if (DeleteService(hService))
			{
				ret = TRUE;
			}

			CloseServiceHandle(hService);
		}

		CloseServiceHandle(hSCM);
	}

	return ret;
} // unregisterServiceC

#endif // SOOS_WINDOWS_CE
#endif // SOOS_WINDOWS


//-----------------------------------------------------------------------------
// OTCInitialize
//
// - initializes the library with the initialization data of the server
//   application
//
// returns:
//		S_OK                - initialised the library
//		OT_E_WRONG_VERSION - init data are for a newer OTC version
//		OT_E_INVALIDDATA   - passed data is invalid
//		E_INVALIDARG        - passed invalid function parameter
//		E_FAIL              - internal error during the initialization
//		OT_E_EXCEPTION     - exception in function
//
LONG OTCAPI_CALL OTCInitialize(
	IN OTCInitData* pInitData) // initialization data
{
#ifdef SOOS_WINDOWS
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // DEBUG
#endif
	SOCMN_TRY
	{
		memset(&g_callbackFunctions, 0, sizeof(OTCCallbackFunctions));

		if (!pInitData)
		{
			return E_INVALIDARG;
		}

		SHORT curVersion = (MAJOR_VERSION_NUMBER * 100) + MINOR_VERSION_NUMBER;

		if (pInitData->m_versionOTC > curVersion)
		{
			return OT_E_WRONG_VERSION;
		}

#ifdef SOOS_WINDOWS
		g_serviceName = pInitData->m_serviceName;
		g_serviceDescription = pInitData->m_serviceDescription;
		g_instance = GetModuleHandle(_T(ORIGINAL_FILENAME));
#endif // SOOS_WINDOWS

#ifndef OTB_USER_COMPILED
		updateLicenseInfo(OTC_FEATURE_DA_CLIENT);
		updateLicenseInfo(OTC_FEATURE_XMLDA_CLIENT);
		updateLicenseInfo(OTC_FEATURE_AE_CLIENT);
		updateLicenseInfo(OTC_FEATURE_TP_CLIENT);
#endif // OTB_USER_COMPILED

#ifdef OTB_CLIENT_DA
		SOCmnPointer<SODaCEntry> entryDA = getSODaCEntry();
		SOCmnPointer<SODaCCreator> creatorDA = new ClientDACreator();

		entryDA->initialize(COINIT_MULTITHREADED);
#ifdef SOFEATURE_DCOM
		entryDA->initializeSecurity(pInitData->m_authLevel, pInitData->m_impLevel, EOAC_NONE);
#endif // SOFEATURE_DCOM
		entryDA->setCreator(creatorDA);

#ifdef SOFEATURE_SOAP
		creatorDA->setXmlGetStatusInterval(pInitData->m_xmldaGetStatusInterval);
#endif // SOFEATURE_SOAP


		entryDA->start();
#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE
		SOCmnPointer<SOAeCEntry> entryAE = getSOAeCEntry();
		SOCmnPointer<SOAeCCreator> creatorAE = new ClientAECreator();

		entryAE->initialize(COINIT_MULTITHREADED);
		entryAE->initializeSecurity(pInitData->m_authLevel, pInitData->m_impLevel, EOAC_NONE);
		entryAE->setCreator(creatorAE);

		entryAE->start();
#endif // OTB_CLIENT_AE

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCInitialize


//-----------------------------------------------------------------------------
// OTCTerminate
//
// - frees the resources used by the library
//
// returns:
//		S_OK                - terminated the library
//		OT_E_EXCEPTION     - exception in function
//
LONG OTCAPI_CALL OTCTerminate(void)
{
	SOCMN_TRY
	{
#ifdef OTB_CLIENT_DA
		SOCmnPointer<SODaCEntry> entryDA = getSODaCEntry();
		entryDA->disconnect();
		entryDA->removeAllAdditionalElements();
		entryDA->removeAllLeaves();
		entryDA->removeAllBranches();
		entryDA->terminate();
#endif // OTB_CLIENT_DA

#ifdef OTB_CLIENT_AE
		SOCmnPointer<SOAeCEntry> entryAE = getSOAeCEntry();
		entryAE->disconnect();
		entryAE->removeAllAdditionalElements();
		entryAE->removeAllLeaves();
		entryAE->removeAllBranches();
		entryAE->terminate();
#endif // OTB_CLIENT_AE

#ifdef SOFEATURE_DCOM
		CoUninitialize();
#endif

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCTerminate


//-----------------------------------------------------------------------------
// OTCProcessCommandLine
//
// - processes the command line actions of an OPC server
//
// returns:
//		S_OK                - terminated the library
//		OT_E_EXCEPTION     - exception in function
//
LONG OTCAPI_CALL OTCProcessCommandLine(
	IN LPTSTR commandLine) // command line of the server application
{
	SOCMN_TRY
	{
		if (!commandLine)
		{
			return E_INVALIDARG;
		}

		if (_tcslen(commandLine) == 0)
		{
			return S_FALSE;
		}

		SOCmnString cmdLine(commandLine);
		BOOL end;

		end = FALSE;
		cmdLine.makeLower();

		if ((_tcsstr(cmdLine, _T("-regserver")) != NULL) ||
		(_tcsstr(cmdLine, _T("/regserver")) != NULL))
		{
			// register OPC Server
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE

			if (!g_serviceName.isEmpty())
			{
				registerServiceC();
			}

#endif
#endif // SOOS_WINDOWS
			end = TRUE;
		}
		else if ((_tcsstr(cmdLine, _T("-unregserver")) != NULL) ||
				 (_tcsstr(cmdLine, _T("/unregserver")) != NULL))
		{
			// unregister OPC Server
#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE

			if (!g_serviceName.isEmpty())
			{
				unregisterServiceC();
			}

#endif
#endif // SOOS_WINDOWS
			end = TRUE;
		}

		return (!end) ? S_OK : S_FALSE;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCProcessCommandLine


//-----------------------------------------------------------------------------
// OTCAdvise
//
// - advises the OTC callbacks of the client application
//
// returns:
//		S_OK                - set callbacks
//		E_INVALIDARG        - passed invalid function parameter
//		OTS_E_EXCEPTION     - exception in function
//
LONG OTCAPI_CALL OTCAdvise(
	IN OTCCallbackFunctions* pCallbacks) // callback functions
{
	if (!pCallbacks)
	{
		return E_INVALIDARG;
	}

	g_callbackFunctions.m_OTOnTrace = (g_callbackFunctions.m_OTOnTrace != NULL) ? g_callbackFunctions.m_OTOnTrace : pCallbacks->m_OTOnTrace;
	g_callbackFunctions.m_OTCOnStateChange = (g_callbackFunctions.m_OTCOnStateChange != NULL) ? g_callbackFunctions.m_OTCOnStateChange : pCallbacks->m_OTCOnStateChange;
	g_callbackFunctions.m_OTCOnDataChange = (g_callbackFunctions.m_OTCOnDataChange != NULL) ? g_callbackFunctions.m_OTCOnDataChange : pCallbacks->m_OTCOnDataChange;
	g_callbackFunctions.m_OTCOnReadComplete = (g_callbackFunctions.m_OTCOnReadComplete != NULL) ? g_callbackFunctions.m_OTCOnReadComplete : pCallbacks->m_OTCOnReadComplete;
	g_callbackFunctions.m_OTCOnWriteComplete = (g_callbackFunctions.m_OTCOnWriteComplete != NULL) ? g_callbackFunctions.m_OTCOnWriteComplete : pCallbacks->m_OTCOnWriteComplete;
	g_callbackFunctions.m_OTCOnPerfromStateTransition = (g_callbackFunctions.m_OTCOnPerfromStateTransition != NULL) ? g_callbackFunctions.m_OTCOnPerfromStateTransition : pCallbacks->m_OTCOnPerfromStateTransition;
	g_callbackFunctions.m_OTCOnServerShutdown = (g_callbackFunctions.m_OTCOnServerShutdown != NULL) ? g_callbackFunctions.m_OTCOnServerShutdown : pCallbacks->m_OTCOnServerShutdown;
	g_callbackFunctions.m_OTCOnUpdateAttributes = (g_callbackFunctions.m_OTCOnUpdateAttributes != NULL) ? g_callbackFunctions.m_OTCOnUpdateAttributes : pCallbacks->m_OTCOnUpdateAttributes;
	g_callbackFunctions.m_OTCOnUpdateDAItemAttributes = (g_callbackFunctions.m_OTCOnUpdateDAItemAttributes != NULL) ? g_callbackFunctions.m_OTCOnUpdateDAItemAttributes : pCallbacks->m_OTCOnUpdateDAItemAttributes;
	g_callbackFunctions.m_OTCOnValidateDAItems = (g_callbackFunctions.m_OTCOnValidateDAItems != NULL) ? g_callbackFunctions.m_OTCOnValidateDAItems : pCallbacks->m_OTCOnValidateDAItems;
	g_callbackFunctions.m_OTCOnGetServerStatus = (g_callbackFunctions.m_OTCOnGetServerStatus != NULL) ? g_callbackFunctions.m_OTCOnGetServerStatus : pCallbacks->m_OTCOnGetServerStatus;
	g_callbackFunctions.m_OTCOnBrowseAddressSpace = (g_callbackFunctions.m_OTCOnBrowseAddressSpace != NULL) ? g_callbackFunctions.m_OTCOnBrowseAddressSpace : pCallbacks->m_OTCOnBrowseAddressSpace;
	g_callbackFunctions.m_OTCOnGetDAProperties = (g_callbackFunctions.m_OTCOnGetDAProperties != NULL) ? g_callbackFunctions.m_OTCOnGetDAProperties : pCallbacks->m_OTCOnGetDAProperties;
	g_callbackFunctions.m_OTCOnBrowseServer = (g_callbackFunctions.m_OTCOnBrowseServer != NULL) ? g_callbackFunctions.m_OTCOnBrowseServer : pCallbacks->m_OTCOnBrowseServer;
	g_callbackFunctions.m_OTCOnReceivedEvents = (g_callbackFunctions.m_OTCOnReceivedEvents != NULL) ? g_callbackFunctions.m_OTCOnReceivedEvents : pCallbacks->m_OTCOnReceivedEvents;
	g_callbackFunctions.m_OTCOnRefreshAEConditions = (g_callbackFunctions.m_OTCOnRefreshAEConditions != NULL) ? g_callbackFunctions.m_OTCOnRefreshAEConditions : pCallbacks->m_OTCOnRefreshAEConditions;
	g_callbackFunctions.m_OTCOnAcknowledgeAEConditions = (g_callbackFunctions.m_OTCOnAcknowledgeAEConditions != NULL) ? g_callbackFunctions.m_OTCOnAcknowledgeAEConditions : pCallbacks->m_OTCOnAcknowledgeAEConditions;
	g_callbackFunctions.m_OTCOnQueryAvailableAEFilters = (g_callbackFunctions.m_OTCOnQueryAvailableAEFilters != NULL) ? g_callbackFunctions.m_OTCOnQueryAvailableAEFilters : pCallbacks->m_OTCOnQueryAvailableAEFilters;
	g_callbackFunctions.m_OTCOnQueryAECategories = (g_callbackFunctions.m_OTCOnQueryAECategories != NULL) ? g_callbackFunctions.m_OTCOnQueryAECategories : pCallbacks->m_OTCOnQueryAECategories;
	g_callbackFunctions.m_OTCOnQueryAEAttributes = (g_callbackFunctions.m_OTCOnQueryAEAttributes != NULL) ? g_callbackFunctions.m_OTCOnQueryAEAttributes : pCallbacks->m_OTCOnQueryAEAttributes;
	g_callbackFunctions.m_OTCOnQueryAEConditions = (g_callbackFunctions.m_OTCOnQueryAEConditions != NULL) ? g_callbackFunctions.m_OTCOnQueryAEConditions : pCallbacks->m_OTCOnQueryAEConditions;
	g_callbackFunctions.m_OTCOnQueryAESubConditions = (g_callbackFunctions.m_OTCOnQueryAESubConditions != NULL) ? g_callbackFunctions.m_OTCOnQueryAESubConditions : pCallbacks->m_OTCOnQueryAESubConditions;
	g_callbackFunctions.m_OTCOnQueryAESourceConditions = (g_callbackFunctions.m_OTCOnQueryAESourceConditions != NULL) ? g_callbackFunctions.m_OTCOnQueryAESourceConditions : pCallbacks->m_OTCOnQueryAESourceConditions;
	g_callbackFunctions.m_OTCOnGetAEConditionState = (g_callbackFunctions.m_OTCOnGetAEConditionState != NULL) ? g_callbackFunctions.m_OTCOnGetAEConditionState : pCallbacks->m_OTCOnGetAEConditionState;
	g_callbackFunctions.m_OTCOnEnableAEConditions = (g_callbackFunctions.m_OTCOnEnableAEConditions != NULL) ? g_callbackFunctions.m_OTCOnEnableAEConditions : pCallbacks->m_OTCOnEnableAEConditions;
	g_callbackFunctions.m_OTCOnGetErrorString = (g_callbackFunctions.m_OTCOnGetErrorString !=  NULL) ? g_callbackFunctions.m_OTCOnGetErrorString : pCallbacks->m_OTCOnGetErrorString;
	g_callbackFunctions.m_OTCOnLogon = (g_callbackFunctions.m_OTCOnLogon != NULL) ? g_callbackFunctions.m_OTCOnLogon : pCallbacks->m_OTCOnLogon;
	g_callbackFunctions.m_OTCOnLogoff = (g_callbackFunctions.m_OTCOnLogoff != NULL) ? g_callbackFunctions.m_OTCOnLogoff : pCallbacks->m_OTCOnLogoff;
	return S_OK;
} // OTCAdvise


#ifdef OTB_CLIENT_DA

LONG OTCAPI_CALL OTCAddDASession(IN LPTSTR url, IN DWORD serverUserData, OUT OTCObjectHandle* pObjectHandle)
{
	SOCMN_TRY
	{
		if (!((isDemoMode())
		|| ((isLicensed(OTC_FEATURE_DA_CLIENT)) && (SOCltEntry::getProtocolByURL(url) == SOCLT_PROTOCOL_DCOM))
		|| ((isLicensed(OTC_FEATURE_XMLDA_CLIENT)) && (SOCltEntry::getProtocolByURL(url) == SOCLT_PROTOCOL_SOAP))
#ifdef SOFEATURE_TUNNEL
		|| ((isLicensed(OTC_FEATURE_TP_CLIENT, 1)) && (SOCltEntry::getProtocolByURL(url) == SOCLT_PROTOCOL_TUNNEL))
#endif
			 ))
		{
			// no license
			return OT_E_NOTLICENSED;
		}

		SOCmnPointer<SODaCEntry> entryDA = getSODaCEntry();
		SOCmnPointer<ClientDASession> server;
		server = (ClientDASession*)entryDA->addServer(url);

		if (server.isNotNull())
		{
			server->setOTCUserData(serverUserData);
			*pObjectHandle = (LONG)server->getOpcHandle();

			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCAddDAServer


LONG OTCAPI_CALL OTCRemoveDASession(IN OTCObjectHandle objectHandle)
{
	SOCMN_TRY
	{
		SODaCServer* pServer = (SODaCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pServer)
		{
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		SOCmnPointer<SODaCEntry> entryDA = getSODaCEntry();
		BOOL ret;

#ifdef SOFEATURE_TUNNEL

		if (pServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			//  unregister this session
			isLicensed(OTC_FEATURE_TP_CLIENT, -1);
		}

#endif

		pServer->disconnect();
		ret = entryDA->removeBranch(pServer);
		pServer->release();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCAddDASubscription(IN OTCObjectHandle sessionHandle, IN DWORD updateRate, IN DWORD groupUserData, OUT OTCObjectHandle* pGroupHandle)
{
	SOCMN_TRY
	{
		SODaCServer* pServer = (SODaCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pServer)
		{
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		SOCmnPointer<ClientDASubscription> group;
		group = (ClientDASubscription*)pServer->addGroup(NULL, updateRate);
		pServer->release();

		if (group.isNotNull())
		{
			group->setOTCUserData(groupUserData);
			*pGroupHandle = (LONG)group->getOpcHandle();

			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCAddDASubscription

LONG OTCAPI_CALL OTCRemoveDASubscription(IN OTCObjectHandle objectHandle)
{
	SOCMN_TRY
	{
		SODaCGroup* pGroup = (SODaCGroup*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pGroup)
		{
			return OT_E_INVALIDHANDLE;
		}

		pGroup->addRef();
		SOCmnPointer<SODaCServer> server = (SODaCServer*)pGroup->getParentBranch();
		BOOL ret;
		pGroup->disconnect();
		ret = server->removeBranch(pGroup);
		pGroup->release();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCAddDAItem(IN OTCObjectHandle groupHandle, IN LPTSTR itemID, IN DWORD itemUserData, OUT OTCObjectHandle* pItemHandle)
{
	SOCMN_TRY
	{
		SODaCGroup* pGroup = (SODaCGroup*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(groupHandle);

		if (!pGroup)
		{
			return OT_E_INVALIDHANDLE;
		}

		pGroup->addRef();
		SOCmnPointer<ClientDAItem> item;
		item = (ClientDAItem*)pGroup->addItem(itemID);
		pGroup->release();

		if (item.isNotNull())
		{
			item->setOTCUserData(itemUserData);
			*pItemHandle = (LONG)item->getOpcHandle();

			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCRemoveDAItem(IN OTCObjectHandle objectHandle)
{
	SOCMN_TRY
	{
		SODaCItem* pItem = (SODaCItem*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pItem)
		{
			return OT_E_INVALIDHANDLE;
		}

		pItem->addRef();
		SOCmnPointer<SODaCGroup> group = (SODaCGroup*)pItem->getParentBranch();
		BOOL ret;
		pItem->disconnect();
		ret = group->removeLeaf(pItem);
		pItem->release();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCRead(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN DWORD maxAge, OUT OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
		SODaCGroup* pGroup = NULL;
		SODaCServer* pServer = NULL;
		HRESULT res;
		DWORD i;
		DWORD correctCount = count;
		SOCmnString traceId;

		if (pObject->getObjectState() == SOCltElement::disconnected)
		{
			pObject->release();
			return OT_E_OPC_NOTFOUND;
		}

		if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
		{
			pGroup = (SODaCGroup*)pObject;
			traceId = pGroup->getObjTraceId();
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER))
		{
			pServer = (SODaCServer*)pObject;
			traceId = pServer->getObjTraceId();
		}
		else
		{
			pObject->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			// synchronous
			SOCmnVariant* pValueArray;
			WORD* pQualityArray;
			SOCmnDateTime* pTimestampArray;
			HRESULT* pErrorArray;
			pValueArray = new SOCmnVariant[count];
			pQualityArray = new WORD[count];
			pTimestampArray = new SOCmnDateTime[count];
			pErrorArray = new HRESULT[count];
			DWORD *pPlaceArray = new DWORD[count];

			if (pGroup)
			{
				// read on group object
				SODaCItem **pSODaCItemArray = (SODaCItem**)OTAllocateMemory(count * sizeof(SODaCItem*));
				SODaCItem *pSODaCItem = NULL;
				correctCount = 0;

				for (i = 0; i < count; ++i)
				{
					pSODaCItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);

					if (pSODaCItem)
					{
						pSODaCItem->addRef();
						pSODaCItemArray[correctCount] = pSODaCItem;
						pPlaceArray[correctCount] = i;
						++correctCount;
					}
					else
					{
						pResults[i] = OT_E_INVALIDHANDLE;
					}
				}

				res = pGroup->syncRead(correctCount, pSODaCItemArray, maxAge, pValueArray, pQualityArray, pTimestampArray, pErrorArray);

				for (i = 0; i < correctCount; ++i)
				{
					pSODaCItemArray[i]->release();
				}

				OTFreeMemory(pSODaCItemArray);
			}
			else // pServer)
			{
				// read on server object
				SOCmnString* pItemIDArray;
				SOCmnString* pItemPathArray;
				pItemIDArray = new SOCmnString[count];
				pItemPathArray = new SOCmnString[count];

				for (i = 0; i < count; i++)
				{
					pItemIDArray[i] = pItemIDs[i];
					pPlaceArray[i] = i;

					if (pItemPaths)
					{
						pItemPathArray[i] = pItemPaths[i];
					}
				}

				res = pServer->read(count, pItemIDArray, pItemPathArray, maxAge, pValueArray, pQualityArray, pTimestampArray, pErrorArray);
				delete[] pItemIDArray;
				delete[] pItemPathArray;
			}

			if (SUCCEEDED(res))
			{
				for (i = 0; i < correctCount; ++i)
				{
					if (SUCCEEDED(pErrorArray[i]))
					{
						if (pValues)
						{
							pValueArray[i].copyTo(pValues[pPlaceArray[i]].m_pValue);
							pValues[pPlaceArray[i]].m_quality = pQualityArray[i];
							pTimestampArray[i].get(&pValues[pPlaceArray[i]].m_timestamp);
						}
					}

					if (pResults)
					{
						pResults[pPlaceArray[i]] = pErrorArray[i];
					}
				}
			}

			delete[] pValueArray;
			delete[] pQualityArray;
			delete[] pTimestampArray;
			delete[] pErrorArray;
			delete[] pPlaceArray;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnReadComplete)
				{
					OTCObjectContext objectContext;
					OTCObjectData* pItemData = NULL;
					getOTCObjectContext(objectHandle, &objectContext);

					if (pItemHandles)
					{
						pItemData = (OTCObjectData*)OTAllocateMemory(correctCount * sizeof(OTCObjectData));
						getOTCObjectsData(correctCount, pItemHandles, pItemData);
					}

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnReadComplete(pExecutionOptions->m_executionContext, objectContext, res, correctCount, pItemData, (LPTSTR*)pItemIDs, (LPTSTR*)pItemPaths, pValues, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, traceId, (_T("OTCRead - Exception occurred in vendor application callback: OTCOnReadComplete")));
					}

					if (pItemData)
					{
						OTFreeMemory(pItemData);
					}
				}
			}

			pObject->release();
			return res;
		}
		else // pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			// asynchronous
			BOOL doAsyncRead = FALSE;

#ifdef SOFEATURE_DCOM

			// check if read connection is established
			if (pGroup)
			{
				if (pGroup->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					doAsyncRead = ((ClientDASubscriptionCom*)pGroup)->getReadConnection(NULL);
				}
			}

#endif // SOFEATURE_DCOM

			if (doAsyncRead)
			{
				// read on group object
				HRESULT* pErrorArray;
				pErrorArray = new HRESULT[count];
				DWORD *pPlaceArray = new DWORD[count];

				SODaCItem **pSODaCItemArray = (SODaCItem**)OTAllocateMemory(count * sizeof(SODaCItem*));
				SODaCItem *pSODaCItem = NULL;
				correctCount = 0;

				for (i = 0; i < count; ++i)
				{
					pSODaCItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
					if (pSODaCItem)
					{
						pSODaCItem->addRef();
						pSODaCItemArray[correctCount] = pSODaCItem;
						pPlaceArray[correctCount] = i;
						++correctCount;
					}
					else
					{
						pResults[i] = OT_E_INVALIDHANDLE;
					}
				}

				res = pGroup->asyncRead(correctCount, pSODaCItemArray, maxAge, pExecutionOptions->m_executionContext, NULL, pErrorArray);

				for (i = 0; i < correctCount; ++i)
				{
					pSODaCItemArray[i]->release();
				}

				OTFreeMemory(pSODaCItemArray);

				if (SUCCEEDED(res))
				{
					if (pResults)
					{
						for (i = 0; i < correctCount; ++i)
						{
							pResults[pPlaceArray[i]] = pErrorArray[i];
						}
					}
				}

				delete[] pErrorArray;
				delete[] pPlaceArray;
				pObject->release();
				return res;
			}
			else
			{
				// read on server object
				SOCmnPointer<ClientJob> job;
				SOCMN_ALLOCATE_OBJECT_C(job, ClientJob((SOCltElement*)pObject, pExecutionOptions), (ClientJob*));
				job->read(count, pItemHandles, (LPCTSTR*)pItemIDs, (LPCTSTR*)pItemPaths, maxAge);

				if (S_OK != ((SOCltElement*)pObject)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
				{
					pObject->release();
					return OT_E_CANTEXECUTEASYNC;
				}

				pObject->release();
				return S_OK;
			}
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCWrite(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
		SODaCGroup* pGroup = NULL;
		SODaCServer* pServer = NULL;
		HRESULT res;
		SOCmnString traceId;

		if (pObject->getObjectState() == SOCltElement::disconnected)
		{
			pObject->release();
			return OT_E_OPC_NOTFOUND;
		}

		if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
		{
			pGroup = (SODaCGroup*)pObject;
			traceId = pGroup->getObjTraceId();
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER))
		{
			pServer = (SODaCServer*)pObject;
			traceId = pServer->getObjTraceId();
		}
		else
		{
			pObject->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		SOCmnVariant *pValueArray = NULL;
		WORD *pQualityArray = NULL;
		SOCmnDateTime *pTimestampArray = NULL;
		HRESULT *pErrorArray = NULL;
		DWORD i;
		BOOL doAsyncWrite = FALSE;
		DWORD correctCount = count;
		DWORD *pPlaceArray = NULL;

		if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
#ifdef SOFEATURE_DCOM

			// check if write connection is established
			if (pGroup)
			{
				if (pGroup->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					doAsyncWrite = ((ClientDASubscriptionCom*)pGroup)->getWriteConnection(NULL);
				}
			}

#endif // SOFEATURE_DCOM
		}

		if ((pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS) || (doAsyncWrite))
		{
			pValueArray = new SOCmnVariant[count];
			pQualityArray = new WORD[count];
			pTimestampArray = new SOCmnDateTime[count];
			pErrorArray = new HRESULT[count];
			pPlaceArray = new DWORD[count];
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			// synchronous
			if (pGroup)
			{
				SODaCItem **pSODaCItemArray = (SODaCItem**)OTAllocateMemory(count * sizeof(SODaCItem*));
				SODaCItem *pSODaCItem = NULL;
				correctCount = 0;

				for (i = 0; i < count; ++i)
				{
					pSODaCItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);

					if (pSODaCItem)
					{
						pSODaCItem->addRef();
						pSODaCItemArray[correctCount] = pSODaCItem;
						pErrorArray[correctCount] = E_FAIL;
						pValueArray[correctCount].initFrom(pValues[i].m_pValue);
						pQualityArray[correctCount] = pValues[i].m_quality;
						pTimestampArray[correctCount].set(&pValues[i].m_timestamp);
						pPlaceArray[correctCount] = i;
						++correctCount;
					}
					else
					{
						pResults[i] = OT_E_INVALIDHANDLE;
					}
				}

				res = pGroup->syncWrite(correctCount, pSODaCItemArray, pValueArray, pQualityArray, pTimestampArray, pErrorArray);

				for (i = 0; i < correctCount; ++i)
				{
					pSODaCItemArray[i]->release();
				}

				OTFreeMemory(pSODaCItemArray);

				if (SUCCEEDED(res))
				{
					if (pResults)
					{
						for (i = 0; i < correctCount; ++i)
						{
							pResults[pPlaceArray[i]] = pErrorArray[i];
						}
					}
				}
			}
			else
			{
				// write on server object
				SOCmnString* pItemIDArray;
				SOCmnString* pItemPathArray;
				pItemIDArray = new SOCmnString[count];
				pItemPathArray = new SOCmnString[count];

				for (i = 0; i < count; i++)
				{
					pItemIDArray[i] = pItemIDs[i];
					pErrorArray[i] = E_FAIL;
					pValueArray[i].initFrom(pValues[i].m_pValue);
					pQualityArray[i] = pValues[i].m_quality;
					pTimestampArray[i].set(&pValues[i].m_timestamp);
					pPlaceArray[i] = i;

					if (pItemPaths)
					{
						pItemPathArray[i] = pItemPaths[i];
					}
				}

				res = pServer->write(count, pItemIDArray, pItemPathArray, pValueArray, pQualityArray, pTimestampArray, pErrorArray);

				if (SUCCEEDED(res))
				{
					if (pResults)
					{
						for (i = 0; i < count; i++)
						{
							pResults[i] = pErrorArray[i];
						}
					}
				}

				delete[] pItemIDArray;
				delete[] pItemPathArray;
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnWriteComplete)
				{
					OTCObjectContext objectContext;
					OTCObjectData* pItemData = NULL;
					getOTCObjectContext(objectHandle, &objectContext);

					if (pItemHandles)
					{
						pItemData = (OTCObjectData*)OTAllocateMemory(count * sizeof(OTCObjectData));
						getOTCObjectsData(count, pItemHandles, pItemData);
					}

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnWriteComplete(pExecutionOptions->m_executionContext, objectContext, res, count, pItemData, (LPTSTR*)pItemIDs, (LPTSTR*)pItemPaths, pValues, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, traceId ,(_T("OTCWrite - Exception occurred in vendor application callback: OTCOnWriteComplete")));
					}

					if (pItemData)
					{
						OTFreeMemory(pItemData);
					}
				}
			}
		}
		else // pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			// asynchronous
			if (doAsyncWrite)
			{
				// write on group object
				SODaCItem **pSODaCItemArray = (SODaCItem**)OTAllocateMemory(count * sizeof(SODaCItem*));
				SODaCItem *pSODaCItem = NULL;
				correctCount = 0;

				for (i = 0; i < count; ++i)
				{
					pSODaCItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);

					if (pSODaCItem)
					{
						pSODaCItem->addRef();
						pSODaCItemArray[correctCount] = pSODaCItem;
						pErrorArray[correctCount] = E_FAIL;
						pValueArray[correctCount].initFrom(pValues[i].m_pValue);
						pQualityArray[correctCount] = pValues[i].m_quality;
						pTimestampArray[correctCount].set(&pValues[i].m_timestamp);
						pPlaceArray[correctCount] = i;
						++correctCount;
					}
					else
					{
						pResults[i] = OT_E_INVALIDHANDLE;
					}
				}

				res = pGroup->asyncWrite(correctCount, pSODaCItemArray, pExecutionOptions->m_executionContext, pValueArray, pQualityArray, pTimestampArray, NULL, pErrorArray);

				for (i = 0; i < correctCount; ++i)
				{
					pSODaCItemArray[i]->release();
				}

				OTFreeMemory(pSODaCItemArray);

				if (SUCCEEDED(res))
				{
					if (pResults)
					{
						for (i = 0; i < correctCount; ++i)
						{
							pResults[pPlaceArray[i]] = pErrorArray[i];
						}
					}
				}
			}
			else
			{
				SOCmnPointer<ClientJob> job;
				SOCMN_ALLOCATE_OBJECT_C(job, ClientJob((SOCltElement*)pObject, pExecutionOptions), (ClientJob*));
				job->write(count, pItemHandles, (LPCTSTR*)pItemIDs, (LPCTSTR*)pItemPaths, pValues);

				if (S_OK != ((SOCltElement*)pObject)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
				{
					pObject->release();
					return OT_E_CANTEXECUTEASYNC;
				}

				pObject->release();
				return S_OK;
			}
		}

		if (pValueArray)
		{
			delete[] pValueArray;
		}

		if (pQualityArray)
		{
			delete[] pQualityArray;
		}

		if (pTimestampArray)
		{
			delete[] pTimestampArray;
		}

		if (pErrorArray)
		{
			delete[] pErrorArray;
		}

		if (pPlaceArray)
		{
			delete[] pPlaceArray;
		}

		pObject->release();
		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCRefreshDAGroup(IN OTCObjectHandle subscriptionHandle, IN DWORD maxAge, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		DWORD transID = 0;

		if (pExecutionOptions)
		{
			transID = pExecutionOptions->m_executionContext;
		}

		SODaCGroup* pGroup = (SODaCGroup*)(void*)GET_MEMORY_ADDRESS(subscriptionHandle);

		if (!pGroup)
		{
			return OT_E_INVALIDHANDLE;
		}

		pGroup->addRef();
		HRESULT res;

		if (pGroup->getObjectState() == SOCltElement::disconnected)
		{
			pGroup->release();
			return OT_E_OPC_NOTFOUND;
		}

		if (!pGroup->is(SOCMNOBJECT_TYPE_GROUP))
		{
			pGroup->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		res = pGroup->asyncRefresh(maxAge, transID);
		pGroup->release();
		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}



LONG OTCAPI_CALL OTCValidateDAItems(IN OTCObjectHandle subscriptionHandle, IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SODaCGroup* pGroup = (SODaCGroup*)(void*)GET_MEMORY_ADDRESS(subscriptionHandle);

		if (!pGroup)
		{
			return OT_E_INVALIDHANDLE;
		}

		pGroup->addRef();
		SOCmnList<SODaCItem> itemList;
		HRESULT res = S_OK;
		DWORD i;
		DWORD correctItemCount = itemCount;

		if (!pGroup->is(SOCMNOBJECT_TYPE_GROUP))
		{
			pGroup->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pGroup->getObjectState() == SOCltElement::disconnected)
		{
			pGroup->release();
			return OT_E_OPC_NOTFOUND;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			if (itemCount > 0)
			{
				// prepare item list
				itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
				HRESULT* pErrorArray = new HRESULT[itemCount];
				DWORD *pPlaceArray = new DWORD[itemCount];
				correctItemCount = 0;
				SODaCItem* pItem;

				for (i = 0; i < itemCount; i++)
				{
					pItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);

					if (pItem)
					{
						pItem->addRef(); // add is time consuming, not risking the item to vanish
						itemList.add(pItem);
						pItem->release();
						pPlaceArray[correctItemCount] = i;
						++correctItemCount;
					}
					else
					{
						pResults[i] = OT_E_INVALIDHANDLE;
					}
				}

				// now have all to validate
				res = pGroup->validateItems(&itemList, pErrorArray);

				if (SUCCEEDED(res))
				{
					if (pResults)
					{
						for (i = 0; i < correctItemCount; ++i)
						{
							pResults[pPlaceArray[i]] = pErrorArray[i];
						}
					}
				}

				delete[] pErrorArray;
			}
			else
			{
				res = E_INVALIDARG;
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnValidateDAItems)
				{
					OTCObjectContext objectContext;
					OTCObjectData* pItemData;
					getOTCObjectContext((OTCObjectHandle)subscriptionHandle, &objectContext);
					pItemData = (OTCObjectData*)OTAllocateMemory(itemCount * sizeof(OTCObjectData));
					getOTCObjectsData(itemCount, pItemHandles, pItemData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnValidateDAItems(pExecutionOptions->m_executionContext, objectContext, res, itemCount, pItemData, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pGroup->getObjTraceId(), (_T("OTCValidateDAItems - Exception occurred in vendor application callback: OTCOnValidateDAItems")));
					}

					OTFreeMemory(pItemData);
				}
			}

			pGroup->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pGroup, pExecutionOptions), (ClientJob*));
			job->validateItems(itemCount, pItemHandles);

			if (S_OK != ((SOCltElement*)pGroup)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pGroup->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pGroup->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCGetDAProperties(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, OUT DWORD* pPropertiesDataCount, OUT OTCDAPropertyData** ppProperty, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltBrowseObject* pBO = (SOCltBrowseObject*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(addressSpaceElementHandle);

		if (pBO)
		{
			pBO->addRef();
		}

		SODaCServer* pServer = (SODaCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pServer)
		{
			if (pBO)
			{
				pBO->release();
			}
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		DWORD numElements = 0;
		LONG res = S_OK;
		DWORD i;

		if (pServer->getObjectState() == SOCltElement::disconnected)
		{
			if (pBO)
			{
				pBO->release();
			}
			pServer->release();
			return OT_E_OPC_NOTFOUND;
		}

		// initialize output parameter
		if (ppProperty != NULL)
		{
			*ppProperty = NULL;
		}

		if (pPropertiesDataCount)
		{
			*pPropertiesDataCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnList<SODaCProperty> propertyList;
			SOCmnList<SODaCPropertyData> propertyDataList;
			BOOL getLocal = FALSE;
			BOOL usePropData = TRUE;

			if (pGetPropertiesOptions)
			{
				getLocal = pGetPropertiesOptions->m_getLocalFromAddressSpaceElement;
			}

			if (!getLocal)
			{
				SOCmnString itemID;
				SOCmnString itemPath;
				WORD whatData = 0;
				DWORD propertyCount = 0;
				DWORD* pPropertyIDs = NULL;
				LPCTSTR* pPropertyNames = NULL;
				BOOL getData = FALSE;

				if (pBO)
				{
					itemID = pBO->getItemID();
					itemPath = pBO->getItemPath();
				}
				else
				{
					itemID = addressSpaceElementID;
					itemPath = addressSpaceElementPath;
				}

				if (pGetPropertiesOptions)
				{
					whatData = pGetPropertiesOptions->m_whatPropertyData;
					propertyCount = pGetPropertiesOptions->m_propertyCount;
					pPropertyIDs = pGetPropertiesOptions->m_pPropertyIDs;
					pPropertyNames = (LPCTSTR*)pGetPropertiesOptions->m_pPropertyNames;
				}

				propertyDataList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);

				if (propertyCount == 0)
				{
					res = pServer->queryAvailableProperties(itemID, itemPath, whatData, propertyDataList);
					getData = ((whatData != 0) && (pServer->getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20));
				}
				else
				{
					SODaCPropertyData* pPropData;
					getData = TRUE;

					for (i = 0; i < propertyCount; i++)
					{
						pPropData = new SODaCPropertyData();

						if (pPropertyIDs)
						{
							pPropData->m_propertyID = pPropertyIDs[i];
						}

						if (pPropertyNames)
						{
							pPropData->m_name = pPropertyNames[i];
						}

						propertyDataList.add(pPropData);
					}
				}

				if ((SUCCEEDED(res)) && (getData))
				{
					res = pServer->getPropertiesData(itemID, itemPath, whatData, propertyDataList);
				}

				numElements = propertyDataList.getCount();
			}
			else
			{
				// getLocal
				if (pBO)
				{
					if ((pBO->getObjectType() & SOCMNOBJECT_TYPE_NODE) == SOCMNOBJECT_TYPE_NODE)
					{
						propertyList = (SOCmnList<SODaCProperty> *)((SODaCNode*)pBO)->getPropertyList();
					}

					if ((pBO->getObjectType() & SOCMNOBJECT_TYPE_TAG) == SOCMNOBJECT_TYPE_TAG)
					{
						propertyList = (SOCmnList<SODaCProperty> *)((SODaCTag*)pBO)->getPropertyList();
					}

					if (propertyList.isInit())
					{
						numElements = propertyList.getCount();
					}
					else
					{
						numElements = 0;
					}

					usePropData = FALSE;
				}
				else
				{
					res = E_INVALIDARG;
				}
			}

			if (SUCCEEDED(res))
			{
				if (pPropertiesDataCount)
				{
					*pPropertiesDataCount = numElements;
				}

				if (numElements > 0)
				{
					// allocate memory for array
					if (ppProperty != NULL)
					{
						*ppProperty = (OTCDAPropertyData*)OTAllocateMemory(numElements * sizeof(OTCDAPropertyData));

						for (i = 0; i < numElements; i++)
						{
							(*ppProperty)[i].m_value.m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
							::VariantInit((*ppProperty)[i].m_value.m_pValue);
						}
					}
					else
					{
						res = E_INVALIDARG;
					}

					if (SUCCEEDED(res))
					{
						// copy properties data to output parameters
						SOCmnString propName;
						SOCmnString propItemID;
						SOCmnString propItemPath;
						DWORD propID;
						SOCmnString propDescription;
						WORD propDatatype;
						SOCmnVariant propValue;
						HRESULT propResult;
						SOCmnListPosition posP;
						SODaCProperty* pProp;
						SODaCPropertyData* pPropData;

						if (usePropData)
						{
							posP = propertyDataList.getStartPosition();
						}
						else
						{
							if (propertyList.isInit())
							{
								posP = propertyList.getStartPosition();
							}
							else
							{
								posP = NULL;
							}
						}

						i = 0;

						while (posP)
						{
							if (usePropData)
							{
								pPropData = propertyDataList.getNext(posP);
								propID = pPropData->m_propertyID;
								propName = pPropData->m_name;
								propItemID = pPropData->m_itemID;
								propItemPath = pPropData->m_itemPath;
								propDescription = pPropData->m_description;
								propDatatype = pPropData->m_datatype;
								propValue = pPropData->m_value;
								propResult = pPropData->m_error;
							}
							else
							{
								pProp = propertyList.getNext(posP);
								propID = pProp->getPropertyID();
								propName = pProp->getPropertyName();
								propItemID = pProp->getItemID();
								propItemPath = pProp->getItemPath();
								propDescription = pProp->getName();
								propDatatype = pProp->getDatatype();
								pProp->getValue(propValue);
								propResult = S_OK;
							}

							(*ppProperty)[i].m_ID = propID;
							SetOutputStructMemberString(propName, &(*ppProperty)[i].m_name);
							SetOutputStructMemberString(propItemID, &(*ppProperty)[i].m_itemID);
							SetOutputStructMemberString(propItemPath, &(*ppProperty)[i].m_itemPath);
							SetOutputStructMemberString(propDescription, &(*ppProperty)[i].m_description);
							(*ppProperty)[i].m_datatype = propDatatype;
							(*ppProperty)[i].m_result = propResult;
							(*ppProperty)[i].m_value.m_quality = OPC_QUALITY_GOOD;
#ifdef SOOS_WINDOWS
							(*ppProperty)[i].m_value.m_timestamp.dwHighDateTime = 0;
							(*ppProperty)[i].m_value.m_timestamp.dwLowDateTime = 0;
#endif
#ifdef SOOS_LINUX
							(*ppProperty)[i].m_value.m_timestamp.tv_sec = 0;
							(*ppProperty)[i].m_value.m_timestamp.tv_usec = 0;
#endif
							propValue.copyTo((*ppProperty)[i].m_value.m_pValue);
							i++;
						}
					}
				}
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnGetDAProperties)
				{
					OTCObjectData objectData;
					getOTCObjectData(sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnGetDAProperties(pExecutionOptions->m_executionContext, objectData, res, addressSpaceElementHandle, (LPTSTR)addressSpaceElementID, (LPTSTR)addressSpaceElementPath, pGetPropertiesOptions, *pPropertiesDataCount, *ppProperty);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pServer->getObjTraceId(), (_T("OTCGetDAProperties - Exception occurred in vendor application callback: OTCOnGetDAProperties")));
					}
				}
			}

			if (pBO)
			{
				pBO->release();
			}
			pServer->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pServer, pExecutionOptions), (ClientJob*));
			job->getProperties(addressSpaceElementHandle, addressSpaceElementID, addressSpaceElementPath, pGetPropertiesOptions);

			if (S_OK != (pServer->executeTask(job, SOCLT_EXEC_ASYNC, NULL)))
			{
				if (pBO)
				{
					pBO->release();
				}
				pServer->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			if (pBO)
			{
				pBO->release();
			}
			pServer->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#else // OTB_CLIENT_DA
LONG OTCAPI_CALL OTCAddDASession(IN LPTSTR url, IN DWORD sessionUserData, OUT OTCObjectHandle* pObjectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddDASubscription(IN OTCObjectHandle sessionHandle, IN DWORD updateRate, IN DWORD groupUserData, OUT OTCObjectHandle* pGroupHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddDAItem(IN OTCObjectHandle groupHandle, IN LPTSTR itemID, IN DWORD itemUserData, OUT OTCObjectHandle* pItemHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDASession(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDASubscription(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDAItem(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetDAProperties(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, OUT DWORD* pPropertiesDataCount, OUT OTCDAPropertyData** ppProperty, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRefreshDAGroup(IN OTCObjectHandle subscriptionHandle, IN DWORD maxAge, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCValidateDAItems(IN OTCObjectHandle subscriptionHandle, IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRead(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN DWORD maxAge, OUT OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCWrite(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
#endif // OTB_CLIENT_DA



LONG OTCAPI_CALL OTCGetState(IN OTCObjectHandle objectHandle, OUT BYTE* pCurrentState, OUT BYTE* pTargetState)
{
	SOCMN_TRY
	{
		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
		*pCurrentState = pObject->getObjectState();
		*pTargetState = pObject->getTargetObjectState();
		pObject->release();
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}



LONG OTCAPI_CALL OTCChangeTargetState(IN OTCObjectHandle objectHandle, IN BYTE state, IN BYTE deep)
{
	SOCMN_TRY
	{
		SOCltElement* pElement = (SOCltElement*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pElement)
		{
			return OT_E_INVALIDHANDLE;
		}

		pElement->addRef();
		pElement->setTargetObjectState(state);

		if (deep)
		{
			SOCmnList<SOCmnElement> subscriptionList;
			SOCmnListPosition pos = NULL;
#ifdef OTB_CLIENT_DA

			if (pElement->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				SODaCGroup* pGroup = NULL;
				subscriptionList = pElement->getBranchList();
				pos = subscriptionList.getStartPosition();

				while (pos)
				{
					pGroup = (SODaCGroup*)subscriptionList.getNext(pos);
					pGroup->setTargetObjectState(state);
					pGroup->setItemsTargetState(state);
				}
			}
			else if (pElement->is(SOCMNOBJECT_TYPE_GROUP))
			{
				SODaCGroup* pGroup = NULL;
				pGroup = (SODaCGroup*)pElement;
				pGroup->setItemsTargetState(state);
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pElement->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				SOAeCSubscription* pSubscription = NULL;
				subscriptionList = pElement->getLeafList();
				pos = subscriptionList.getStartPosition();

				while (pos)
				{
					pSubscription = (SOAeCSubscription*)subscriptionList.getNext(pos);
					pSubscription->setTargetObjectState(state);
				}
			}

#endif // OTB_CLIENT_AE
		}

		pElement->release();
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCChangeTargetState

LONG OTCAPI_CALL OTCPerformStateTransition(IN OTCObjectHandle objectHandle, IN BYTE deep, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltElement* pElement = (SOCltElement*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pElement)
		{
			return OT_E_INVALIDHANDLE;
		}

		pElement->addRef();

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			LONG res;
			// perform state transition
			res = pElement->performStateTransition(deep);

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnPerfromStateTransition)
				{
					OTCObjectContext objectContext;
					getOTCObjectContext(objectHandle, &objectContext);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnPerfromStateTransition(pExecutionOptions->m_executionContext, objectContext, res);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pElement->getObjTraceId(),  (_T("OTCPerformStateTransition - Exception occurred in vendor application callback: OTCOnPerfromStateTransition")));
					}
				}
			}

			pElement->release();
			return res;
		}
		else
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pElement, pExecutionOptions), (ClientJob*));
			job->performStateTransition(deep);

			if (S_OK != pElement->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pElement->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pElement->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCPerformStateTransition


LONG OTCAPI_CALL OTCGetAttributes(IN OTCObjectHandle objectHandle, IN DWORD whatAttributes, OUT OTCObjectAttributes* pObjectAttributes)
{
	SOCMN_TRY
	{
		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
#ifdef OTB_CLIENT_DA

		if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
		{
			SODaCGroup* pGroup;
			pGroup = (SODaCGroup*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_NAME) == OTC_ATTRIBUTE_DASUBSCRIPTION_NAME)
			{
				SOCmnString name = pGroup->getName();
				SetOutputStructMemberString(name, &pObjectAttributes->m_daSubscriptionName);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE) == OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE)
			{
				pObjectAttributes->m_daSubscriptionRevisedUpdateRate = pGroup->getUpdateRate(&pObjectAttributes->m_daSubscriptionRequestedUpdateRate);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND) == OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND)
			{
				pObjectAttributes->m_daSubscriptionDeadband = pGroup->getDeadBand();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS) == OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS)
			{
				pObjectAttributes->m_daSubscriptionTimeBias = pGroup->getTimeBias();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_LCID) == OTC_ATTRIBUTE_DASUBSCRIPTION_LCID)
			{
				pObjectAttributes->m_daSubscriptionLCID = pGroup->getLCID();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME) == OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME)
			{
				pObjectAttributes->m_daSubscriptionRevisedKeepAliveTime = pGroup->getKeepAliveTime(&pObjectAttributes->m_daSubscriptionRequestedKeepAliveTime);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_DATARETRIEVAL) == OTC_ATTRIBUTE_DASUBSCRIPTION_DATARETRIEVAL)
			{
				pObjectAttributes->m_daSubscriptionDataRetreivalMode = pGroup->getDataRetrieval();
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SODaCItem* pItem;
			pItem = (SODaCItem*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_ID) == OTC_ATTRIBUTE_DAITEM_ID)
			{
				SOCmnString itemID = pItem->getItemID();
				SetOutputStructMemberString(itemID, &pObjectAttributes->m_daItemID);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_PATH) == OTC_ATTRIBUTE_DAITEM_PATH)
			{
				SOCmnString itemPath = pItem->getItemPath();
				SetOutputStructMemberString(itemPath, &pObjectAttributes->m_daItemPath);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE) == OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE)
			{
				pObjectAttributes->m_daItemNativeDatatype = pItem->getNativeDatatype();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS) == OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS)
			{
				pObjectAttributes->m_daItemAccessRights = pItem->getAccessRights();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE) == OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE)
			{
				pObjectAttributes->m_daItemRequestedDatatype = pItem->getReqDatatype();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_DEADBAND) == OTC_ATTRIBUTE_DAITEM_DEADBAND)
			{
				pObjectAttributes->m_daItemDeadband = pItem->getDeadBand();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_DEADBAND) == OTC_ATTRIBUTE_DAITEM_DEADBAND)
			{
				pObjectAttributes->m_daItemDeadband = pItem->getDeadBand();
			}

			if (((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUTYPE) == OTC_ATTRIBUTE_DAITEM_EUTYPE) ||
				((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUINFO) == OTC_ATTRIBUTE_DAITEM_EUINFO))
			{
				OPCEUTYPE euType;
				SOCmnVariant euInfo;
				pItem->getEU(&euType, euInfo);

				if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUTYPE) == OTC_ATTRIBUTE_DAITEM_EUTYPE)
				{
					pObjectAttributes->m_daItemEuType = (BYTE)euType;
				}

				if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_EUINFO) == OTC_ATTRIBUTE_DAITEM_EUINFO)
				{
					if (pObjectAttributes->m_daItemEuInfo)
					{
						euInfo.copyTo(pObjectAttributes->m_daItemEuInfo);
					}
				}
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA))
		{
			SODaCServer* pServer;
			pServer = (SODaCServer*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_LCID) == OTC_ATTRIBUTE_DASESSION_LCID)
			{
				pObjectAttributes->m_daSessionLCID = pServer->getLCID();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_CLIENTNAME) == OTC_ATTRIBUTE_DASESSION_CLIENTNAME)
			{
				SOCmnString clientName = pServer->getClientName();
				SetOutputStructMemberString(clientName, &pObjectAttributes->m_daSessionClientName);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_URL) == OTC_ATTRIBUTE_DASESSION_URL)
			{
				SOCmnString url = pServer->getURL();
				SetOutputStructMemberString(url, &pObjectAttributes->m_daSessionURL);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_REQUESTTIMEOUT) == OTC_ATTRIBUTE_DASESSION_REQUESTTIMEOUT)
			{
				pObjectAttributes->m_daSessionRequestTimeout = pServer->getRequestTimeout();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION) == OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION)
			{
				pObjectAttributes->m_daSessionForcedOPCSpecification = DaCOPCVersionToOTC(pServer->getForcedOPCVersion());
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION) == OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION)
			{
				pObjectAttributes->m_daSessionSupportedOPCSpecification = DaCOPCVersionToOTC(pServer->getSupportedOPCVersion());
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_PERFORMINITIALREAD) == OTC_ATTRIBUTE_DASESSION_PERFORMINITIALREAD)
			{
				pObjectAttributes->m_daSessionPerformInitialRead = pServer->getPerformInitialRead();
			}

#ifdef SOFEATURE_DCOM

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_COM_CLSCTX) == OTC_ATTRIBUTE_DASESSION_COM_CLSCTX)
			{
				if (pServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pObjectAttributes->m_daSessionComClsctx = ((ClientDASessionCom*)pServer)->getClsctx();
				}
				else
				{
					pObjectAttributes->m_daSessionComClsctx = OTC_COM_CLSCTX_NOT_COM;
				}
			}

#endif // SOFEATURE_DCOM
		}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

		if (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
		{
			SOAeCSubscription* pSubscription;
			pSubscription = (SOAeCSubscription*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME) == OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME)
			{
				pObjectAttributes->m_aeSubscriptionRequestedBufferTime = pSubscription->getBufferTime();
				pObjectAttributes->m_aeSubscriptionRevisedBufferTime = pSubscription->getRevisedBufferTime();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE) == OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE)
			{
				pObjectAttributes->m_aeSubscriptionRequestedMaxSize = pSubscription->getMaxSize();
				pObjectAttributes->m_aeSubscriptionRevisedMaxSize = pSubscription->getRevisedMaxSize();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER) == OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER)
			{
				pObjectAttributes->m_aeSubscriptionFilterSeverityLow = pSubscription->getLowSeverity();
				pObjectAttributes->m_aeSubscriptionFilterSeverityHigh = pSubscription->getHighSeverity();
				pObjectAttributes->m_aeSubscriptionFilterEventTypes = pSubscription->getEventTypes();
				SetOutputStructMemberDWORDArray(pSubscription->getCategoryList(), &pObjectAttributes->m_aeSubscriptionFilterCountCategrories, &pObjectAttributes->m_aeSubscriptionFilterCategrories);
				SetOutputStructMemberStringArray(pSubscription->getAreaList(), &pObjectAttributes->m_aeSubscriptionFilterCountAreas, &pObjectAttributes->m_aeSubscriptionFilterAreas);
				SetOutputStructMemberStringArray(pSubscription->getSourceList(), &pObjectAttributes->m_aeSubscriptionFilterCountSources, &pObjectAttributes->m_aeSubscriptionFilterSources);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES) == OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES)
			{
				SOAeCReturnedAttrListList retAttrListList(*pSubscription->getReturnedAttrListList());
				SOCmnListPosition posRALL;
				DWORD countRALL;
				size_t categoryID;
				SOCmnDWORDList retAttrList;
				DWORD k = 0;
				countRALL = retAttrListList.getCount();
				pObjectAttributes->m_aeSubscriptionCountReturnedAttributes = countRALL;

				if (countRALL > 0)
				{
					pObjectAttributes->m_aeSubscriptionReturnedAttributes = (OTCAEReturnedAttributesData*)OTAllocateMemory(sizeof(OTCAEReturnedAttributesData) * countRALL);
					posRALL = retAttrListList.getStartPosition();

					while (posRALL)
					{
						retAttrList = retAttrListList.getNext(posRALL, &categoryID);
						pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_categoryID = (DWORD)categoryID;
						SetOutputStructMemberDWORDArray(&retAttrList, &pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_countAttributeIDs, &pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_attributeIDs);
						k++;
					}
				}
				else
				{
					pObjectAttributes->m_aeSubscriptionReturnedAttributes = NULL;
				}
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			SOAeCServer* pServer;
			pServer = (SOAeCServer*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_LCID) == OTC_ATTRIBUTE_AESESSION_LCID)
			{
				pObjectAttributes->m_aeSessionLCID = pServer->getLCID();
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_CLIENTNAME) == OTC_ATTRIBUTE_AESESSION_CLIENTNAME)
			{
				SOCmnString clientName = pServer->getClientName();
				SetOutputStructMemberString(clientName, &pObjectAttributes->m_aeSessionClientName);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_URL) == OTC_ATTRIBUTE_AESESSION_URL)
			{
				SOCmnString url = pServer->getURL();
				SetOutputStructMemberString(url, &pObjectAttributes->m_aeSessionURL);
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_COM_CLSCTX) == OTC_ATTRIBUTE_AESESSION_COM_CLSCTX)
			{
				if (pServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					pObjectAttributes->m_aeSessionComClsctx = ((ClientAESessionCom*)pServer)->getClsctx();
				}
				else
				{
					pObjectAttributes->m_aeSessionComClsctx = OTC_COM_CLSCTX_NOT_COM;
				}
			}
		}

#endif // OTB_CLIENT_AE
		else if ((!(pObject->is(SOCMNOBJECT_TYPE_GROUP)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_ITEM)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_DA)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))))

		{
			pObject->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		pObject->release();
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCSetAttributes(IN OTCObjectHandle objectHandle, IN DWORD whatAttributes, IN OTCObjectAttributes* pObjectAttributes)
{
	SOCMN_TRY
	{
		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
		HRESULT res = S_OK;

#ifdef OTB_CLIENT_DA

		if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
		{
			SODaCGroup* pGroup;
			pGroup = (SODaCGroup*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_NAME) == OTC_ATTRIBUTE_DASUBSCRIPTION_NAME)
			{
				if (!pGroup->setName(pObjectAttributes->m_daSubscriptionName))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE) == OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE)
			{
				if (!pGroup->setReqUpdateRate(pObjectAttributes->m_daSubscriptionRequestedUpdateRate))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND) == OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND)
			{
				if (!pGroup->setDeadBand(pObjectAttributes->m_daSubscriptionDeadband))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS) == OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS)
			{
				if (!pGroup->setTimeBias(pObjectAttributes->m_daSubscriptionTimeBias))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_LCID) == OTC_ATTRIBUTE_DASUBSCRIPTION_LCID)
			{
				if (!pGroup->setLCID(pObjectAttributes->m_daSubscriptionLCID))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME) == OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME)
			{
				if (!pGroup->setReqKeepAliveTime(pObjectAttributes->m_daSubscriptionRequestedKeepAliveTime))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASUBSCRIPTION_DATARETRIEVAL) == OTC_ATTRIBUTE_DASUBSCRIPTION_DATARETRIEVAL)
			{
				BYTE newMode = 0;

				switch (pObjectAttributes->m_daSubscriptionDataRetreivalMode)
				{
				case OTC_DASUBSCRIPTION_DATARETRIEVAL_ASYNCADVISE:
					newMode = SODaCGroup::drAsyncAdvise;
					break;
				case OTC_DASUBSCRIPTION_DATARETRIEVAL_SYNCREAD:
					newMode = SODaCGroup::drSyncRead;
					break;
				case OTC_DASUBSCRIPTION_DATARETRIEVAL_POLLEDREFRESH:
					newMode = SODaCGroup::drPolledRefresh;
					break;
				default:
					res = S_FALSE;
				}

				if (newMode != 0)
				{
					if (!pGroup->setDataRetrieval((SODaCGroup::dataRetrieval)newMode))
					{
						res = S_FALSE;
					}
				}
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SODaCItem* pItem;
			pItem = (SODaCItem*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_ID) == OTC_ATTRIBUTE_DAITEM_ID)
			{
				if (!pItem->setItemID(pObjectAttributes->m_daItemID))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_PATH) == OTC_ATTRIBUTE_DAITEM_PATH)
			{
				if (!pItem->setItemPath(pObjectAttributes->m_daItemPath))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE) == OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE)
			{
				if (!pItem->setReqDatatype(pObjectAttributes->m_daItemRequestedDatatype))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DAITEM_DEADBAND) == OTC_ATTRIBUTE_DAITEM_DEADBAND)
			{
				if (!pItem->setDeadBand(pObjectAttributes->m_daItemDeadband))
				{
					res = S_FALSE;
				}
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA))
		{
			SODaCServer* pServer;
			pServer = (SODaCServer*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_LCID) == OTC_ATTRIBUTE_DASESSION_LCID)
			{
				if (!pServer->setLCID(pObjectAttributes->m_daSessionLCID))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_CLIENTNAME) == OTC_ATTRIBUTE_DASESSION_CLIENTNAME)
			{
				if (!pServer->setClientName(pObjectAttributes->m_daSessionClientName))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_REQUESTTIMEOUT) == OTC_ATTRIBUTE_DASESSION_REQUESTTIMEOUT)
			{
				if (!pServer->setRequestTimeout(pObjectAttributes->m_daSessionRequestTimeout))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_PERFORMINITIALREAD) == OTC_ATTRIBUTE_DASESSION_PERFORMINITIALREAD)
			{
				if (!pServer->setPerformInitialRead(pObjectAttributes->m_daSessionPerformInitialRead))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION) == OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION)
			{
				if (!pServer->forceOPCVersion(OTCOPCVersionToDaC(pObjectAttributes->m_daSessionForcedOPCSpecification)))
				{
					res = S_FALSE;
				}
			}

#ifdef SOFEATURE_DCOM

			if ((whatAttributes & OTC_ATTRIBUTE_DASESSION_COM_CLSCTX) == OTC_ATTRIBUTE_DASESSION_COM_CLSCTX)
			{
				if (pServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					if (!((ClientDASessionCom*)pServer)->setClsctx(pObjectAttributes->m_daSessionComClsctx))
					{
						res = S_FALSE;
					}
				}
				else
				{
					res = S_FALSE;
				}
			}

#endif // SOFEATURE_DCOM
		}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

		if (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
		{
			SOAeCSubscription* pSubscription;
			pSubscription = (SOAeCSubscription*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME) == OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME)
			{
				if (!pSubscription->setBufferTime(pObjectAttributes->m_aeSubscriptionRequestedBufferTime))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE) == OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE)
			{
				if (!pSubscription->setMaxSize(pObjectAttributes->m_aeSubscriptionRequestedMaxSize))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER) == OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER)
			{
				if (pObjectAttributes->m_aeSubscriptionFilterSeverityLow != 0)
				{
					if (!pSubscription->setLowSeverity(pObjectAttributes->m_aeSubscriptionFilterSeverityLow))
					{
						res = S_FALSE;
					}
				}

				if (pObjectAttributes->m_aeSubscriptionFilterSeverityHigh != 0)
				{
					if (!pSubscription->setHighSeverity(pObjectAttributes->m_aeSubscriptionFilterSeverityHigh))
					{
						res = S_FALSE;
					}
				}

				if (pObjectAttributes->m_aeSubscriptionFilterEventTypes != 0)
				{
					if (!pSubscription->setEventTypes(pObjectAttributes->m_aeSubscriptionFilterEventTypes))
					{
						res = S_FALSE;
					}
				}

				DWORD k;
				SOCmnDWORDList catList(pSubscription->getCategoryList());
				catList.clear();

				for (k = 0; k < pObjectAttributes->m_aeSubscriptionFilterCountCategrories; k++)
				{
					if (!catList.add(pObjectAttributes->m_aeSubscriptionFilterCategrories[k]))
					{
						res = S_FALSE;
					}
				}

				SOCmnStringList areaList(pSubscription->getAreaList());
				areaList.clear();

				for (k = 0; k < pObjectAttributes->m_aeSubscriptionFilterCountAreas; k++)
				{
					if (!areaList.add(pObjectAttributes->m_aeSubscriptionFilterAreas[k]))
					{
						res = S_FALSE;
					}
				}

				SOCmnStringList sourceList(pSubscription->getSourceList());
				sourceList.clear();

				for (k = 0; k < pObjectAttributes->m_aeSubscriptionFilterCountSources; k++)
				{
					if (!sourceList.add(pObjectAttributes->m_aeSubscriptionFilterSources[k]))
					{
						res = S_FALSE;
					}
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES) == OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES)
			{
				// clear all returned attributes specified
				SOAeCReturnedAttrListList retAttrListList(*pSubscription->getReturnedAttrListList());
				retAttrListList.removeAll();
				retAttrListList.autoUnlock();
				SOCmnDWORDList retAttrList;
				DWORD k, l;

				for (k = 0; k < pObjectAttributes->m_aeSubscriptionCountReturnedAttributes; k++)
				{
					retAttrList = pSubscription->getReturnedAttributes(pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_categoryID);

					for (l = 0; l < pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_countAttributeIDs; l++)
					{
						if (!retAttrList.add(pObjectAttributes->m_aeSubscriptionReturnedAttributes[k].m_attributeIDs[l]))
						{
							res = S_FALSE;
						}
					}
				}
			}
		}
		else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			SOAeCServer* pServer;
			pServer = (SOAeCServer*)pObject;

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_LCID) == OTC_ATTRIBUTE_AESESSION_LCID)
			{
				if (!pServer->setLCID(pObjectAttributes->m_aeSessionLCID))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_CLIENTNAME) == OTC_ATTRIBUTE_AESESSION_CLIENTNAME)
			{
				if (!pServer->setClientName(pObjectAttributes->m_aeSessionClientName))
				{
					res = S_FALSE;
				}
			}

			if ((whatAttributes & OTC_ATTRIBUTE_AESESSION_COM_CLSCTX) == OTC_ATTRIBUTE_AESESSION_COM_CLSCTX)
			{
				if (pServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					if (!((ClientAESessionCom*)pServer)->setClsctx(pObjectAttributes->m_aeSessionComClsctx))
					{
						res = S_FALSE;
					}
				}
				else
				{
					res = S_FALSE;
				}
			}
		}

#endif // OTB_CLIENT_AE
		else if ((!(pObject->is(SOCMNOBJECT_TYPE_GROUP)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_ITEM)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_DA)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION)))
				 &&
				 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))))
		{
			pObject->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		pObject->release();
		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCUpdateAttributes(IN OTCObjectHandle objectHandle, IN BYTE fromServer, IN DWORD attributeCount, IN DWORD* pWhatAttributes, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCmnObject* pObject = (SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pObject)
		{
			return OT_E_INVALIDHANDLE;
		}

		pObject->addRef();
		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			HRESULT res = S_OK;
			DWORD internalWhatAttributesOne = 0;
			DWORD* pInternalWhatAttributes;
			DWORD i;
			pInternalWhatAttributes = (DWORD*)OTAllocateMemory(sizeof(DWORD) * attributeCount);
			SOCmnString traceId;
#ifdef OTB_CLIENT_DA

			if (pObject->is(SOCMNOBJECT_TYPE_GROUP))
			{
				SODaCGroup* pGroup;
				LONG resMgt = E_FAIL;
				LONG resName = E_FAIL;
				LONG resKeepAlive = E_FAIL;
				pGroup = (SODaCGroup*)pObject;
				traceId = pGroup->getObjTraceId();

				// prepare whatAttributes
				for (i = 0; i < attributeCount; i++)
				{
					pInternalWhatAttributes[i] = 0;

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_NAME) == OTC_ATTRIBUTE_DASUBSCRIPTION_NAME)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_NAME;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE) == OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_UPDATERATE;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND) == OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_DEADBAND;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS) == OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_TIMEBIAS;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_LCID) == OTC_ATTRIBUTE_DASUBSCRIPTION_LCID)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_LCID;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME) == OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME)
					{
						pInternalWhatAttributes[i] |= SODAC_GRPPAR_KEEPALIVETIME;
					}

					internalWhatAttributesOne |= pInternalWhatAttributes[i];
				}

				if (internalWhatAttributesOne & SODAC_GRPPAR_ALL_MGT)
				{
					if (fromServer)
					{
						resMgt = pGroup->doUpdateImpl(fromServer, internalWhatAttributesOne & SODAC_GRPPAR_ALL_MGT);

						if (FAILED(resMgt))
						{
							res = S_FALSE;
						}
					}
					else
					{
						if (internalWhatAttributesOne & SODAC_GRPPAR_NAME)
						{
							resName = pGroup->doUpdateImpl(fromServer, internalWhatAttributesOne & SODAC_GRPPAR_NAME);

							if (FAILED(resName))
							{
								res = S_FALSE;
							}
						}

						if (internalWhatAttributesOne & (SODAC_GRPPAR_ALL_MGT & ~SODAC_GRPPAR_NAME))
						{
							resMgt = pGroup->doUpdateImpl(fromServer, internalWhatAttributesOne & (SODAC_GRPPAR_ALL_MGT & ~SODAC_GRPPAR_NAME));

							if (FAILED(resMgt))
							{
								res = S_FALSE;
							}
						}
					}
				}

				if (internalWhatAttributesOne & SODAC_GRPPAR_KEEPALIVETIME)
				{
					resKeepAlive = pGroup->doUpdateImpl(fromServer, internalWhatAttributesOne & SODAC_GRPPAR_KEEPALIVETIME);

					if (FAILED(resKeepAlive))
					{
						res = S_FALSE;
					}
				}

				// set the results
				if (pResults)
				{
					for (i = 0; i < attributeCount; i++)
					{
						if (pInternalWhatAttributes[i] & SODAC_GRPPAR_ALL_MGT)
						{
							if (fromServer)
							{
								if (pInternalWhatAttributes[i] & ~SODAC_GRPPAR_ALL_MGT)
								{
									pResults[i] = res;
								}
								else
								{
									pResults[i] = resMgt;
								}
							}
							else
							{
								if (pInternalWhatAttributes[i] & ~SODAC_GRPPAR_NAME)
								{
									pResults[i] = res;
								}
								else
								{
									pResults[i] = resName;
								}

								if (pInternalWhatAttributes[i] & ~(SODAC_GRPPAR_ALL_MGT & ~SODAC_GRPPAR_NAME))
								{
									pResults[i] = res;
								}
								else
								{
									pResults[i] = resMgt;
								}
							}
						}
						else if (pInternalWhatAttributes[i] & SODAC_GRPPAR_KEEPALIVETIME)
						{
							if (pInternalWhatAttributes[i] & ~SODAC_GRPPAR_KEEPALIVETIME)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resKeepAlive;
							}
						}
					}
				}
			}
			else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				SODaCServer* pServer;
				LONG resLCID = E_FAIL;
				LONG resClientName = E_FAIL;
				pServer = (SODaCServer*)pObject;
				traceId = pServer->getObjTraceId();

				// prepare whatAttributes
				for (i = 0; i < attributeCount; i++)
				{
					pInternalWhatAttributes[i] = 0;

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASESSION_LCID) == OTC_ATTRIBUTE_DASESSION_LCID)
					{
						pInternalWhatAttributes[i] |= SOCLT_SRVPAR_LCID;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DASESSION_CLIENTNAME) == OTC_ATTRIBUTE_DASESSION_CLIENTNAME)
					{
						pInternalWhatAttributes[i] |= SOCLT_SRVPAR_CLIENT_NAME;
					}

					internalWhatAttributesOne |= pInternalWhatAttributes[i];
				}

				if (internalWhatAttributesOne & SOCLT_SRVPAR_LCID)
				{
					resLCID = pServer->doUpdateImpl(fromServer, internalWhatAttributesOne & SOCLT_SRVPAR_LCID);

					if (FAILED(resLCID))
					{
						res = S_FALSE;
					}
				}

				if (internalWhatAttributesOne & SOCLT_SRVPAR_CLIENT_NAME)
				{
					resClientName = pServer->doUpdateImpl(fromServer, internalWhatAttributesOne & SOCLT_SRVPAR_CLIENT_NAME);

					if (FAILED(resClientName))
					{
						res = S_FALSE;
					}
				}

				// set the results
				if (pResults)
				{
					for (i = 0; i < attributeCount; i++)
					{
						if (pInternalWhatAttributes[i] & SOCLT_SRVPAR_LCID)
						{
							if (pInternalWhatAttributes[i] & ~SOCLT_SRVPAR_LCID)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resLCID;
							}
						}

						if (pInternalWhatAttributes[i] & SOCLT_SRVPAR_CLIENT_NAME)
						{
							if (pInternalWhatAttributes[i] & ~SOCLT_SRVPAR_CLIENT_NAME)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resClientName;
							}
						}
					}
				}
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
			{
				SOAeCSubscription* pSubscription;
				LONG resState = E_FAIL;
				LONG resFilter = E_FAIL;
				LONG resReturnedAttributes = E_FAIL;
				pSubscription = (SOAeCSubscription*)pObject;
				traceId = pSubscription->getObjTraceId();

				// prepare whatAttributes
				for (i = 0; i < attributeCount; i++)
				{
					pInternalWhatAttributes[i] = 0;

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME) == OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME)
					{
						pInternalWhatAttributes[i] |= SOAEC_SUBSPAR_BUFFERTIME;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE) == OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE)
					{
						pInternalWhatAttributes[i] |= SOAEC_SUBSPAR_MAXSIZE;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER) == OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER)
					{
						pInternalWhatAttributes[i] |= SOAEC_SUBSPAR_FILTER;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES) == OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES)
					{
						pInternalWhatAttributes[i] |= SOAEC_SUBSPAR_RETURNED_ATTRIBUTES;
					}

					internalWhatAttributesOne |= pInternalWhatAttributes[i];
				}

				if (internalWhatAttributesOne & SOAEC_SUBSPAR_STATE)
				{
					resState = pSubscription->doUpdate(fromServer, internalWhatAttributesOne & SOAEC_SUBSPAR_STATE);

					if (FAILED(resState))
					{
						res = S_FALSE;
					}
				}

				if (internalWhatAttributesOne & SOAEC_SUBSPAR_FILTER)
				{
					resFilter = pSubscription->doUpdate(fromServer, internalWhatAttributesOne & SOAEC_SUBSPAR_FILTER);

					if (FAILED(resFilter))
					{
						res = S_FALSE;
					}
				}

				if (internalWhatAttributesOne & SOAEC_SUBSPAR_RETURNED_ATTRIBUTES)
				{
					resReturnedAttributes = pSubscription->doUpdate(fromServer, internalWhatAttributesOne & SOAEC_SUBSPAR_RETURNED_ATTRIBUTES);

					if (FAILED(resReturnedAttributes))
					{
						res = S_FALSE;
					}
				}

				// set the results
				if (pResults)
				{
					for (i = 0; i < attributeCount; i++)
					{
						if (pInternalWhatAttributes[i] & SOAEC_SUBSPAR_STATE)
						{
							if (pInternalWhatAttributes[i] & ~SOAEC_SUBSPAR_STATE)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resState;
							}
						}

						if (pInternalWhatAttributes[i] & SOAEC_SUBSPAR_FILTER)
						{
							if (pInternalWhatAttributes[i] & ~SOAEC_SUBSPAR_FILTER)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resFilter;
							}
						}

						if (pInternalWhatAttributes[i] & SOAEC_SUBSPAR_RETURNED_ATTRIBUTES)
						{
							if (pInternalWhatAttributes[i] & ~SOAEC_SUBSPAR_RETURNED_ATTRIBUTES)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resReturnedAttributes;
							}
						}
					}
				}
			}
			else if (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				SOAeCServer* pServer = NULL;
				LONG resLCID = E_FAIL;
				LONG resClientName = E_FAIL;
				pServer = (SOAeCServer*)pObject;
				traceId = pServer->getObjTraceId();

				// prepare whatAttributes
				for (i = 0; i < attributeCount; i++)
				{
					pInternalWhatAttributes[i] = 0;

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESESSION_LCID) == OTC_ATTRIBUTE_AESESSION_LCID)
					{
						pInternalWhatAttributes[i] |= SOCLT_SRVPAR_LCID;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_AESESSION_CLIENTNAME) == OTC_ATTRIBUTE_AESESSION_CLIENTNAME)
					{
						pInternalWhatAttributes[i] |= SOCLT_SRVPAR_CLIENT_NAME;
					}

					internalWhatAttributesOne |= pInternalWhatAttributes[i];
				}

				if (internalWhatAttributesOne & SOCLT_SRVPAR_LCID)
				{
					resLCID = pServer->doUpdate(fromServer, internalWhatAttributesOne & SOCLT_SRVPAR_LCID);

					if (FAILED(resLCID))
					{
						res = S_FALSE;
					}
				}

				if (internalWhatAttributesOne & SOCLT_SRVPAR_CLIENT_NAME)
				{
					resClientName = pServer->doUpdate(fromServer, internalWhatAttributesOne & SOCLT_SRVPAR_CLIENT_NAME);

					if (FAILED(resClientName))
					{
						res = S_FALSE;
					}
				}

				// set the results
				if (pResults)
				{
					for (i = 0; i < attributeCount; i++)
					{
						if (pInternalWhatAttributes[i] & SOCLT_SRVPAR_LCID)
						{
							if (pInternalWhatAttributes[i] & ~SOCLT_SRVPAR_LCID)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resLCID;
							}
						}

						if (pInternalWhatAttributes[i] & SOCLT_SRVPAR_CLIENT_NAME)
						{
							if (pInternalWhatAttributes[i] & ~SOCLT_SRVPAR_CLIENT_NAME)
							{
								pResults[i] = res;
							}
							else
							{
								pResults[i] = resClientName;
							}
						}
					}
				}
			}

#endif // OTB_CLIENT_AE
			else if ((!(pObject->is(SOCMNOBJECT_TYPE_GROUP)))
					 &&
					 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_DA)))
					 &&
					 (!(pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION)))
					 &&
					 (!(pObject->is(SOCMNOBJECT_TYPE_SERVER_AE))))
			{
				res = OT_E_INVALIDOBJECTTYPE;
				traceId = pObject->getObjTraceId();
			}

			OTFreeMemory(pInternalWhatAttributes);

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnUpdateAttributes)
				{
					OTCObjectContext objectContext;
					getOTCObjectContext((OTCObjectHandle)objectHandle, &objectContext);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnUpdateAttributes(pExecutionOptions->m_executionContext, objectContext, res, fromServer, attributeCount, pWhatAttributes, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, traceId, (_T("OTCUpdateAttributes - Exception occurred in vendor application callback: OTCOnUpdateAttributes")));
					}
				}
			}

			pObject->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			if ((pObject->is(SOCMNOBJECT_TYPE_GROUP)) || (pObject->is(SOCMNOBJECT_TYPE_SERVER_DA)) || (pObject->is(SOCMNOBJECT_TYPE_SERVER_AE)) || (pObject->is(SOCMNOBJECT_TYPE_SUBSCRIPTION)))
			{
				SOCmnPointer<ClientJob> job;
				SOCMN_ALLOCATE_OBJECT_C(job, ClientJob((SOCltElement*)pObject, pExecutionOptions), (ClientJob*));
				job->updateAttributes(fromServer, attributeCount, pWhatAttributes);

				if (S_OK != ((SOCltElement*)pObject)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
				{
					pObject->release();
					return OT_E_CANTEXECUTEASYNC;
				}

				pObject->release();
				return S_OK;
			}
			else
			{
				pObject->release();
				return OT_E_INVALIDOBJECTTYPE;
			}
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#ifdef OTB_CLIENT_DA

LONG OTCAPI_CALL OTCUpdateDAItemAttributes(
	IN OTCObjectHandle subscriptionHandle,
	IN DWORD itemCount,
	IN OTCObjectHandle* pItemHandles,
	IN BYTE fromServer,
	IN DWORD attributeCount,
	IN DWORD* pWhatAttributes,
	OUT LONG* pResults,
	IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SODaCGroup* pGroup = (SODaCGroup*)(void*)GET_MEMORY_ADDRESS(subscriptionHandle);

		if (!pGroup)
		{
			return OT_E_INVALIDHANDLE;
		}

		pGroup->addRef();

		if (!pGroup->is(SOCMNOBJECT_TYPE_GROUP))
		{
			pGroup->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnList<SODaCItem> itemList;
			HRESULT res = S_OK;
			DWORD internalWhatAttributesOne = 0;
			DWORD* pInternalWhatAttributes;
			DWORD i;
			HRESULT resMgt = E_FAIL;
			HRESULT resDeadband = E_FAIL;
			SODaCItem* pItem = NULL;
			// prepare item list
			itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

			for (i = 0; i < itemCount; i++)
			{
				pItem = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
				if (pItem)
				{
					pItem->addRef(); // add is time consuming, not risking the item to vanish
					itemList.add(pItem);
					pItem->release();
				}
			}

			// prepare whatAttributes
			pInternalWhatAttributes = (DWORD*)OTAllocateMemory(sizeof(DWORD) * attributeCount);

			for (i = 0; i < attributeCount; i++)
			{
				pInternalWhatAttributes[i] = 0;

				if (fromServer)
				{
					// read only attributes
					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE) == OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE)
					{
						pInternalWhatAttributes[i] |= SODAC_ITMPAR_ALL_MGT;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS) == OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS)
					{
						pInternalWhatAttributes[i] |= SODAC_ITMPAR_ALL_MGT;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_EUTYPE) == OTC_ATTRIBUTE_DAITEM_EUTYPE)
					{
						pInternalWhatAttributes[i] |= SODAC_ITMPAR_EU;
					}

					if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_EUINFO) == OTC_ATTRIBUTE_DAITEM_EUINFO)
					{
						pInternalWhatAttributes[i] |= SODAC_ITMPAR_EU;
					}
				}

				if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE) == OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE)
				{
					pInternalWhatAttributes[i] |= SODAC_ITMPAR_REQDATATYPE;
				}

				if ((pWhatAttributes[i] & OTC_ATTRIBUTE_DAITEM_DEADBAND) == OTC_ATTRIBUTE_DAITEM_DEADBAND)
				{
					pInternalWhatAttributes[i] |= SODAC_ITMPAR_DEADBAND;
				}

				internalWhatAttributesOne |= pInternalWhatAttributes[i];
			}

			// execute the update
			if (internalWhatAttributesOne & SODAC_ITMPAR_ALL_MGT)
			{
				resMgt = pGroup->updateItemsImpl(fromServer, &itemList, internalWhatAttributesOne & SODAC_ITMPAR_ALL_MGT);

				if (FAILED(resMgt))
				{
					res = S_FALSE;
				}
			}

			if (internalWhatAttributesOne & SODAC_ITMPAR_ALL_DEADBAND)
			{
				resDeadband = pGroup->updateItemsImpl(fromServer, &itemList, internalWhatAttributesOne & SODAC_ITMPAR_ALL_DEADBAND);

				if (FAILED(resDeadband))
				{
					res = S_FALSE;
				}
			}

			// set the results
			if (pResults)
			{
				for (i = 0; i < attributeCount; i++)
				{
					if (pInternalWhatAttributes[i] & SODAC_ITMPAR_ALL_MGT)
					{
						if (pInternalWhatAttributes[i] & ~SODAC_ITMPAR_ALL_MGT)
						{
							pResults[i] = res;
						}
						else
						{
							pResults[i] = resMgt;
						}
					}

					if (pInternalWhatAttributes[i] & SODAC_ITMPAR_ALL_DEADBAND)
					{
						if (pInternalWhatAttributes[i] & ~SODAC_ITMPAR_ALL_DEADBAND)
						{
							pResults[i] = res;
						}
						else
						{
							pResults[i] = resDeadband;
						}
					}
				}
			}

			OTFreeMemory(pInternalWhatAttributes);
			itemList.destroy();

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnUpdateDAItemAttributes)
				{
					OTCObjectContext objectContext;
					OTCObjectData* pItemData;
					getOTCObjectContext((OTCObjectHandle)subscriptionHandle, &objectContext);
					pItemData = (OTCObjectData*)OTAllocateMemory(itemCount * sizeof(OTCObjectData));
					getOTCObjectsData(itemCount, pItemHandles, pItemData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnUpdateDAItemAttributes(pExecutionOptions->m_executionContext, objectContext, res, itemCount, pItemData, fromServer, attributeCount, pWhatAttributes, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pGroup->getObjTraceId(), (_T("OTCUpdateDAItemAttributes - Exception occurred in vendor application callback: OTCOnUpdateDAItemAttributes")));
					}

					OTFreeMemory(pItemData);
				}
			}

			pGroup->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pGroup, pExecutionOptions), (ClientJob*));
			job->updateItemAttributes(itemCount, pItemHandles, fromServer, attributeCount, pWhatAttributes);

			if (S_OK != ((SOCltElement*)pGroup)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pGroup->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pGroup->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}
#else // OTB_CLIENT_DA
LONG OTCAPI_CALL OTCUpdateDAItemAttributes(IN OTCObjectHandle subscriptionHandle, IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, IN BYTE fromServer, IN DWORD attributeCount, IN DWORD* pWhatAttributes, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
#endif // OTB_CLIENT_DA


LONG OTCAPI_CALL OTCActivateConnectionMonitor(IN OTCObjectHandle sessionHandle, BYTE activate, DWORD checkCycle, DWORD connect1Attempts, DWORD connect1Cycle, DWORD connect2Cycle)
{
	SOCMN_TRY
	{
		SOCltServer* pServer = (SOCltServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pServer)
		{
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		BOOL ret;

		if (activate)
		{
			ret = pServer->addConnectionMonitor(checkCycle, connect1Attempts, connect1Cycle, connect2Cycle);
		}
		else
		{
			ret = pServer->removeConnectionMonitor();
		}

		pServer->release();
		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCGetServerStatus(IN OTCObjectHandle sessionHandle, OTCServerStatus* pServerStatus, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltElement* pEl = (SOCltElement*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pEl)
		{
			return OT_E_INVALIDHANDLE;
		}

		pEl->addRef();
		HRESULT res = E_FAIL;

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
#ifdef OTB_CLIENT_DA

			if (pEl->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				SODaCServer* pServer = (SODaCServer*)pEl;
				SODaCServerStatus status;
				res = pServer->getStatus(status);

				if (SUCCEEDED(res))
				{
					pServerStatus->m_state = status.m_state;
					status.m_startTime.get(&pServerStatus->m_startTime);
					status.m_currentTime.get(&pServerStatus->m_currentTime);
					status.m_lastUpdateTime.get(&pServerStatus->m_lastUpdateTime);
					SetOutputStructMemberString(status.m_vendorInfo, &pServerStatus->m_vendorInfo);
					SetOutputStructMemberString(status.m_productVersion, &pServerStatus->m_productVersion);
					SetOutputStructMemberString(status.m_statusInfo, &pServerStatus->m_statusInfo);
					pServerStatus->m_bandwidth = status.m_bandwidth;
					pServerStatus->m_groupCount = status.m_groupCount;
					SetOutputStructMemberDWORDArray(&status.m_supportedLCIDs, &pServerStatus->m_countLCIDs, &pServerStatus->m_supportedLCIDs);
				}
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pEl->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				SOAeCServer* pServer = (SOAeCServer*)pEl;
				SOAeCServerStatus status;
				res = pServer->getStatus(status);

				if (SUCCEEDED(res))
				{
					pServerStatus->m_state = status.m_state;
					status.m_startTime.get(&pServerStatus->m_startTime);
					status.m_currentTime.get(&pServerStatus->m_currentTime);
					status.m_lastUpdateTime.get(&pServerStatus->m_lastUpdateTime);
					SetOutputStructMemberString(status.m_vendorInfo, &pServerStatus->m_vendorInfo);
					SetOutputStructMemberString(status.m_productVersion, &pServerStatus->m_productVersion);
					pServerStatus->m_statusInfo = NULL;
					pServerStatus->m_bandwidth = 0xFFFFFFFF;
					pServerStatus->m_groupCount = 0;
					SetOutputStructMemberDWORDArray(&status.m_supportedLCIDs, &pServerStatus->m_countLCIDs, &pServerStatus->m_supportedLCIDs);
				}
			}

#endif // OTB_CLIENT_AE

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnGetServerStatus)
				{
					OTCObjectData objectData;
					getOTCObjectData(sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnGetServerStatus(pExecutionOptions->m_executionContext, objectData, res, pServerStatus);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pEl->getObjTraceId(), (_T("OTCGetServerStatus - Exception occurred in vendor application callback: OTCOnGetServerStatus")));
					}
				}
			}

			pEl->release();
			return res;
		}
		else
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pEl, pExecutionOptions), (ClientJob*));
			job->getServerStatus();

			if (S_OK != pEl->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pEl->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pEl->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#ifdef SOFEATURE_DCOM

LONG OTCAPI_CALL OTCBrowseServer(IN LPTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData, OUT DWORD* pServerDataCount, OUT OTCServerData** ppServerData, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		BOOL ret;
		BOOL allOK = TRUE;
		SOCmnPointer<SOCltServerBrowser> sb;
		DWORD whatServer = 0;
		SOCmnList<CLSID> clsidList;
		SOCmnDWORDList whatServersList;
		SOCmnString ipA;
		DWORD numServers;
		LONG res = S_OK;

		// initialize output parameter
		if (ppServerData != NULL)
		{
			*ppServerData = NULL;
		}

		if (pServerDataCount)
		{
			*pServerDataCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			if (ipAddress)
			{
				ipA = ipAddress;
			}

			sb = new SOCltServerBrowser();
			sb->setNodeName(ipA);

			if (whatOPCSpec & OT_OPCSPECIFICATION_DA10)
			{
				whatServer |= SOCLT_BROWSE_SERVER_DA10A;
			}

			if (whatOPCSpec & OT_OPCSPECIFICATION_DA20)
			{
				whatServer |= SOCLT_BROWSE_SERVER_DA20;
			}

			if (whatOPCSpec & OT_OPCSPECIFICATION_DA30)
			{
				whatServer |= SOCLT_BROWSE_SERVER_DA30;
			}

			if (whatOPCSpec & OT_OPCSPECIFICATION_AE10)
			{
				whatServer |= SOCLT_BROWSE_SERVER_AE10;
			}

			ret = sb->browseServer(whatServer, clsidList, &whatServersList);
			allOK &= ret;

			if (ret)
			{
				numServers = clsidList.getCount();

				if (pServerDataCount)
				{
					*pServerDataCount = numServers;
				}

				if (numServers > 0)
				{
					// allocate memory for array
					if (ppServerData != NULL)
					{
						*ppServerData = (OTCServerData*)OTAllocateMemory(numServers * sizeof(OTCServerData));
					}
					else
					{
						res = E_INVALIDARG;
					}

					if (SUCCEEDED(res))
					{
						SOCmnString classID;
						SOCmnString descr;
						SOCmnString progId;
						SOCmnString vProgId;
						SOCmnString url;
						SOCmnListPosition pos;
						CLSID* clsid = NULL;
						DWORD i;
						pos = clsidList.getStartPosition();
						i = 0;

						while (pos)
						{
							clsid = clsidList.getNext(pos);
							classID.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), clsid->Data1, clsid->Data2, clsid->Data3, clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3], clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);

							if (whatServerData != OTC_SERVERDATA_CLSID)
							{
								// want more than the CLSID
								ret = sb->getClassDetail(*clsid, descr, progId, vProgId);
								allOK &= ret;

								if (!ret)
								{
									continue;
								}
							}

							if (whatServerData & OTC_SERVERDATA_CLSID)
							{
								SetOutputStructMemberString(classID, &(*ppServerData)[i].m_clsid);
							}

							if (whatServerData & OTC_SERVERDATA_PROGID)
							{
								SetOutputStructMemberString(progId, &(*ppServerData)[i].m_progId);
							}

							if (whatServerData & OTC_SERVERDATA_VERSIONINDEPENDENTPROGID)
							{
								SetOutputStructMemberString(vProgId, &(*ppServerData)[i].m_vProgId);
							}

							if (whatServerData & OTC_SERVERDATA_DESCRIPTION)
							{
								SetOutputStructMemberString(descr, &(*ppServerData)[i].m_description);
							}

							if (whatServerData & OTC_SERVERDATA_URL)
							{
								DWORD opcSpec = 0;
								opcSpec = whatServersList.findKey(i);

								if ((opcSpec & (SOCLT_BROWSE_SERVER_DA10A | SOCLT_BROWSE_SERVER_DA20 | SOCLT_BROWSE_SERVER_DA30)) != 0)
								{
									// DA
									url.format(_T("opcda://%s/%s/%s"), (LPCTSTR)ipA, (LPCTSTR)progId, (LPCTSTR)classID);
								}
								else
								{
									// AE
									url.format(_T("opcae://%s/%s/%s"), (LPCTSTR)ipA, (LPCTSTR)progId, (LPCTSTR)classID);
								}

								SetOutputStructMemberString(url, &(*ppServerData)[i].m_url);
							}

							if (whatServerData & OTC_SERVERDATA_OPCSPECIFICATIONS)
							{
								DWORD opcSpec = 0;
								(*ppServerData)[i].m_opcSpecifications = 0;
								opcSpec = whatServersList.findKey(i);

								if (opcSpec & SOCLT_BROWSE_SERVER_DA10A)
								{
									(*ppServerData)[i].m_opcSpecifications |= OT_OPCSPECIFICATION_DA10;
								}

								if (opcSpec & SOCLT_BROWSE_SERVER_DA20)
								{
									(*ppServerData)[i].m_opcSpecifications |= OT_OPCSPECIFICATION_DA20;
								}

								if (opcSpec & SOCLT_BROWSE_SERVER_DA30)
								{
									(*ppServerData)[i].m_opcSpecifications |= OT_OPCSPECIFICATION_DA30;
								}

								if (opcSpec & SOCLT_BROWSE_SERVER_AE10)
								{
									(*ppServerData)[i].m_opcSpecifications |= OT_OPCSPECIFICATION_AE10;
								}
							}

							i++;
						}
					}
				}
			}

			if (res == S_OK)
			{
				res = (allOK == TRUE) ? S_OK : E_FAIL;
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnBrowseServer)
				{
					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnBrowseServer(pExecutionOptions->m_executionContext, res, (LPTSTR)ipAddress, whatOPCSpec, whatServerData, *pServerDataCount, *ppServerData);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, sb->getObjTraceId(), (_T("OTCBrowseServer - Exception occurred in vendor application callback: OTCOnBrowseServer")));
					}
				}
			}

			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
#ifdef OTB_CLIENT_DA
			SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
#else
			SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
#endif
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(entry, pExecutionOptions), (ClientJob*));
			job->browseServer(ipAddress, whatOPCSpec, whatServerData);

			if (S_OK != (entry->executeTask(job, SOCLT_EXEC_ASYNC, NULL)))
			{
				return OT_E_CANTEXECUTEASYNC;
			}

			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}
#else // SOFEATURE_DCOM
LONG OTCAPI_CALL OTCBrowseServer(IN LPTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData, OUT DWORD* pServerDataCount, OUT OTCServerData** ppServerData, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
#endif // SOFEATURE_DCOM


LONG OTCAPI_CALL OTCBrowseAddressSpace(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, OUT DWORD* pAddressSpaceElementDataCount, OUT OTCAddressSpaceElementData** ppAddressSpaceElement, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltBrowseObject* pBO = (SOCltBrowseObject*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(addressSpaceElementHandle);
		
		if (pBO)
		{
			pBO->addRef();
		}

		SOCltElement* pEl = (SOCltElement*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pEl) // pBO is checked against null during the function, if it`s null everything is fine, no need to panic
		{
			if (pBO)
			{
				pBO->release();
			}
			return OT_E_INVALIDHANDLE;
		}

		pEl->addRef();
		SOCmnList<SOCltBrowseObject> childList;
		SOCmnListPosition posC;
		SOCltBrowseObject* pBOC;
		DWORD expandType = 0;
		DWORD i;
		BOOL ret = FALSE;
		DWORD type;
		SOCmnString name;
		DWORD numElements = 0;
		LONG res = S_OK;
		BOOL browseFlat = FALSE;

		// initialize output parameter
		if (ppAddressSpaceElement != NULL)
		{
			*ppAddressSpaceElement = NULL;
		}

		if (pAddressSpaceElementDataCount)
		{
			*pAddressSpaceElementDataCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
#ifdef OTB_CLIENT_DA

			if (pEl->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				SODaCServer* pServer = (SODaCServer*)pEl;
				SOCmnPointer<SODaCNameSpaceBrowser> browser;
				SOCmnString itemID;
				SOCmnString itemPath;
				browser = pServer->getNameSpaceBrowser();
				SOCmnString contPoint;

#ifdef SOFEATURE_DCOM
				OPCNAMESPACETYPE nameSpaceType = OPC_NS_HIERARCHIAL;
				ClientDASessionCom* pComServer = dynamic_cast<ClientDASessionCom*>(pServer);
				if (pComServer != NULL)
				{
					pComServer->queryOrganization(&nameSpaceType);
					if (nameSpaceType == OPC_NS_FLAT)
					{
						browseFlat = TRUE;
					}
				}
#endif //	SOFEATURE_DCOM
				if (pBrowseOptions)
				{
					if ((pBrowseOptions->m_type & OT_ADDRESSSPACETYPE_BRANCH) == OT_ADDRESSSPACETYPE_BRANCH)
					{
						expandType |= SOCMNOBJECT_TYPE_NODE;
					}

					if (((pBrowseOptions->m_type & OT_ADDRESSSPACETYPE_LEAF) == OT_ADDRESSSPACETYPE_LEAF) ||
						((pBrowseOptions->m_type & OT_ADDRESSSPACETYPE_ITEM) == OT_ADDRESSSPACETYPE_ITEM))
					{
						if (browser->support30())
						{
							expandType |= SOCMNOBJECT_TYPE_ITEM;
						}
						else
						{
							expandType |= SOCMNOBJECT_TYPE_TAG;
						}
					}
					
					browser->setRetrieveItemID(pBrowseOptions->m_retrieveItemID);
					browser->setForceBrowseUp(pBrowseOptions->m_forceBrowseUp);

					if (browser->support30())
					{
						browser->setReturnProperties(pBrowseOptions->m_retrieveProperties);
						browser->setRetrievePropertyValues(pBrowseOptions->m_retrievePropertyValues);
						browser->setMaxElements(pBrowseOptions->m_maxElements);
						browser->setFilter(pBrowseOptions->m_elementNameFilter, pBrowseOptions->m_vendorFilter);
						browser->setContinuationPoint(pBrowseOptions->m_continuationPoint);
						contPoint = pBrowseOptions->m_continuationPoint;
					}
					else
					{
						if (!pBO)
						{
							browser->setFilter(pBrowseOptions->m_elementNameFilter, pBrowseOptions->m_accessRightsFilter, pBrowseOptions->m_datatypeFilter);
						}
						else
						{
							SOCmnPointer<SODaCNameSpaceBrowser> pBrowser = (SODaCNameSpaceBrowser*)pBO->getBrowser();
							pBrowser->setFilter(pBrowseOptions->m_elementNameFilter, pBrowseOptions->m_accessRightsFilter, pBrowseOptions->m_datatypeFilter);
						}
					}
				}
				else
				{
					if (browser->support30())
					{
						expandType = SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM;
					}
					else
					{
						expandType = SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_LEAF;
					}

					browser->setRetrieveItemID(TRUE);
				}
				
				if (browseFlat == TRUE)
				{
					if (pBrowseOptions->m_type != OT_ADDRESSSPACETYPE_BRANCH)
					{
#ifdef SOFEATURE_DCOM
						// only do the flat browse if no branches are to be returned
						ret = ((SODaCComNameSpaceBrowser*)(SODaCNameSpaceBrowser*)browser)->expandFlat(childList);
#endif // SOFEATURE_DCOM
					}
				}
				else
				{
					if (!pBO)
					{
						if (((!addressSpaceElementID) || (_tcslen(addressSpaceElementID) == 0)) &&
							((!addressSpaceElementPath) || (_tcslen(addressSpaceElementPath) == 0)))
						{
							ret = browser->expand(expandType, childList); // browse from the root
							contPoint.empty();
						}
						else   // browse from id string - works only with DA3 or XML-DA
						{
							ret = browser->expand30(browser, addressSpaceElementID, addressSpaceElementPath, expandType, contPoint, childList); // browse from the root
							browser->setContinuationPoint(_T(""));
						}
					}
					else
					{
						ret = pBO->expand(expandType, childList);
						contPoint.empty();
					}

				}

				if (ret)
				{
					numElements = childList.getCount();

					if (pAddressSpaceElementDataCount)
					{
						*pAddressSpaceElementDataCount = numElements;
					}

					if (numElements > 0)
					{
						// allocate memory for array
						if (ppAddressSpaceElement != NULL)
						{
							*ppAddressSpaceElement = (OTCAddressSpaceElementData*)OTAllocateMemory(numElements * sizeof(OTCAddressSpaceElementData));
						}
						else
						{
							res = E_INVALIDARG;
						}

						if (SUCCEEDED(res))
						{
							posC = childList.getStartPosition();
							i = 0;

							while (posC)
							{
								pBOC = childList.getNext(posC);
								name = pBOC->getName();
								type = pBOC->getObjectType();
								itemID = pBOC->getItemID();
								itemPath = pBOC->getItemPath();
								(*ppAddressSpaceElement)[i].m_objectHandle = (OTObjectHandle)pBOC->getOpcHandle();
								pBOC->addRef();

								if ((type & SOCMNOBJECT_TYPE_NODE) == SOCMNOBJECT_TYPE_NODE)
								{
									(*ppAddressSpaceElement)[i].m_type = OT_ADDRESSSPACETYPE_BRANCH;

									if (((SODaCNode*)pBOC)->getFlags() & OPC_BROWSE_ISITEM)
									{
										(*ppAddressSpaceElement)[i].m_type |= OT_ADDRESSSPACETYPE_ITEM;
									}
								}

								if ((type & SOCMNOBJECT_TYPE_TAG) == SOCMNOBJECT_TYPE_TAG)
								{
									(*ppAddressSpaceElement)[i].m_type = OT_ADDRESSSPACETYPE_LEAF;
									(*ppAddressSpaceElement)[i].m_type |= OT_ADDRESSSPACETYPE_ITEM;
								}

								SetOutputStructMemberString(name, &(*ppAddressSpaceElement)[i].m_name);
								SetOutputStructMemberString(itemID, &(*ppAddressSpaceElement)[i].m_itemID);
								SetOutputStructMemberString(itemPath, &(*ppAddressSpaceElement)[i].m_itemPath);
								i++;
							}
						}
					}
				}
				if (pBrowseOptions)
				{
					if (pBrowseOptions->m_continuationPoint)
					{
						free(pBrowseOptions->m_continuationPoint);
						pBrowseOptions->m_continuationPoint = NULL;
					}
					if (!contPoint.isEmpty())
					{
						pBrowseOptions->m_continuationPoint = _tcsdup(contPoint);
					}
					else
					{
						pBrowseOptions->m_continuationPoint = _tcsdup(browser->getContinuationPoint());
					}
				}
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pEl->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				SOAeCServer* pServer = (SOAeCServer*)pEl;
				SOCmnPointer<SOAeCAreaBrowser> browser;
				SOCmnString itemID;
				browser = pServer->getAreaBrowser();

				if (pBrowseOptions)
				{
					if ((pBrowseOptions->m_type & OT_ADDRESSSPACETYPE_BRANCH) == OT_ADDRESSSPACETYPE_BRANCH)
					{
						expandType |= SOCMNOBJECT_TYPE_AREA;
					}

					if ((pBrowseOptions->m_type & OT_ADDRESSSPACETYPE_LEAF) == OT_ADDRESSSPACETYPE_LEAF)
					{
						expandType |= SOCMNOBJECT_TYPE_SOURCE;
					}
				}
				else
				{
					expandType = SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_LEAF;
				}

				if (!pBO)
				{
					if (((!addressSpaceElementID) || (_tcslen(addressSpaceElementID) == 0)) &&
						((!addressSpaceElementPath) || (_tcslen(addressSpaceElementPath) == 0)))
					{
						ret = browser->expand(expandType, childList); // browse from the root
					}
					else
					{
						ret = FALSE;
					}
				}
				else
				{
					ret = pBO->expand(expandType, childList);
				}

				if (ret)
				{
					numElements = childList.getCount();

					if (pAddressSpaceElementDataCount)
					{
						*pAddressSpaceElementDataCount = numElements;
					}

					if (numElements > 0)
					{
						// allocate memory for array
						if (ppAddressSpaceElement != NULL)
						{
							*ppAddressSpaceElement = (OTCAddressSpaceElementData*)OTAllocateMemory(numElements * sizeof(OTCAddressSpaceElementData));
						}
						else
						{
							res = E_INVALIDARG;
						}

						if (SUCCEEDED(res))
						{
							posC = childList.getStartPosition();
							i = 0;

							while (posC)
							{
								pBOC = childList.getNext(posC);
								name = pBOC->getName();
								type = pBOC->getObjectType();
								itemID = pBOC->getPath();
								(*ppAddressSpaceElement)[i].m_objectHandle = (OTObjectHandle)pBOC->getOpcHandle();
								pBOC->addRef();

								if ((type & SOCMNOBJECT_TYPE_AREA) == SOCMNOBJECT_TYPE_AREA)
								{
									(*ppAddressSpaceElement)[i].m_type = OT_ADDRESSSPACETYPE_BRANCH;
								}

								if ((type & SOCMNOBJECT_TYPE_SOURCE) == SOCMNOBJECT_TYPE_SOURCE)
								{
									(*ppAddressSpaceElement)[i].m_type = OT_ADDRESSSPACETYPE_LEAF;
								}

								SetOutputStructMemberString(name, &(*ppAddressSpaceElement)[i].m_name);
								SetOutputStructMemberString(itemID, &(*ppAddressSpaceElement)[i].m_itemID);
								(*ppAddressSpaceElement)[i].m_itemPath = NULL;
								i++;
							}
						}
					}

					childList.removeAll();
				}
			}

#endif // OTB_CLIENT_AE

			if (res == S_OK)
			{
				res = (ret == TRUE) ? S_OK : E_FAIL;
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnBrowseAddressSpace)
				{
					OTCObjectData objectData;
					getOTCObjectData(sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnBrowseAddressSpace(pExecutionOptions->m_executionContext, objectData, res, addressSpaceElementHandle, (LPTSTR)addressSpaceElementID, (LPTSTR)addressSpaceElementPath, pBrowseOptions, *pAddressSpaceElementDataCount, *ppAddressSpaceElement);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pEl->getObjTraceId(), (_T("OTCBrowseAddressSpace - Exception occurred in vendor application callback: OTCOnBrowseAddressSpace")));
					}
				}
			}

			if (pBO)
			{
				pBO->release();
			}
			pEl->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pEl, pExecutionOptions), (ClientJob*));
			job->browseAddressSpace(addressSpaceElementHandle, addressSpaceElementID, addressSpaceElementPath, pBrowseOptions);

			if (S_OK != (pEl->executeTask(job, SOCLT_EXEC_ASYNC, NULL)))
			{
				if (pBO)
				{
					pBO->release();
				}
				pEl->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			if (pBO)
			{
				pBO->release();
			}
			pEl->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCReleaseAddressSpaceElement(IN OTCObjectHandle addressSpaceElementHandle, IN BOOL deep)
{
	SOCMN_TRY
	{
		SOCltBrowseObject* pBO = (SOCltBrowseObject*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(addressSpaceElementHandle);

		if (!pBO)
		{
			return OT_E_INVALIDHANDLE;
		}

		pBO->addRef();

		if (deep)
		{
			pBO->removeAllLeaves();
			pBO->removeAllBranches();
		}

		pBO->release();
		pBO->release();
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


#ifdef OTB_CLIENT_AE

LONG OTCAPI_CALL OTCAddAESession(IN LPTSTR url, IN DWORD serverUserData, OUT OTCObjectHandle* pObjectHandle)
{
	SOCMN_TRY
	{
		if (!((isDemoMode())
		|| ((isLicensed(OTC_FEATURE_AE_CLIENT)) && (SOCltEntry::getProtocolByURL(url) == SOCLT_PROTOCOL_DCOM))
#ifdef SOFEATURE_TUNNEL
		|| ((isLicensed(OTC_FEATURE_TP_CLIENT, 1)) && (SOCltEntry::getProtocolByURL(url) == SOCLT_PROTOCOL_TUNNEL))
#endif
			 ))
		{
			// no license
			return OT_E_NOTLICENSED;
		}

		SOCmnPointer<SOAeCEntry> entryAE = getSOAeCEntry();
		SOCmnPointer<ClientAESession> server;

		server = (ClientAESession*)entryAE->addServer(url);

		if (server.isNotNull())
		{
			server->setOTCUserData(serverUserData);
			*pObjectHandle = (LONG)server->getOpcHandle();
			return S_OK;
		}
		else
		{ return E_FAIL; }
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCAddAEServer


LONG OTCAPI_CALL OTCRemoveAESession(IN OTCObjectHandle objectHandle)
{
	SOCMN_TRY
	{
		SOAeCServer* pServer = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pServer)
		{
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		SOCmnPointer<SOAeCEntry> entryAE = getSOAeCEntry();
		BOOL ret;

#ifdef SOFEATURE_TUNNEL

		if (pServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			//  unregister this session
			isLicensed(OTC_FEATURE_TP_CLIENT, -1);
		}

#endif

		pServer->disconnect();
		ret = entryAE->removeBranch(pServer);
		pServer->release();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCAddAESubscription(IN OTCObjectHandle sessionHandle, IN DWORD subscriptionUserData, OUT OTCObjectHandle* pSubscriptionHandle)
{
	SOCMN_TRY
	{
		SOAeCServer* pServer = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pServer)
		{
			return OT_E_INVALIDHANDLE;
		}

		pServer->addRef();
		SOCmnPointer<ClientAESubscription> subscription;
		subscription = (ClientAESubscription*)pServer->addSubscription();
		pServer->release();

		if (subscription.isNotNull())
		{
			subscription->setOTCUserData(subscriptionUserData);
			*pSubscriptionHandle = (LONG)subscription->getOpcHandle();
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTCAddAESubscription


LONG OTCAPI_CALL OTCRemoveAESubscription(IN OTCObjectHandle objectHandle)
{
	SOCMN_TRY
	{
		SOAeCSubscription* pSubscription = (SOAeCSubscription*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(objectHandle);

		if (!pSubscription)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSubscription->addRef();
		SOCmnPointer<SOAeCServer> server = (SOAeCServer*)pSubscription->getParentBranch();
		BOOL ret;
		pSubscription->disconnect();
		ret = server->removeLeaf(pSubscription);
		pSubscription->release();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCRefreshAEConditions(IN OTCObjectHandle subscriptionHandle, IN BYTE cancelRefresh, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCSubscription* pSubscription = (SOAeCSubscription*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(subscriptionHandle);

		if (!pSubscription)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSubscription->addRef();

		if (!pSubscription->is(SOCMNOBJECT_TYPE_SUBSCRIPTION))
		{
			pSubscription->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			BOOL ret;
			LONG res;

			if (!cancelRefresh)
			{
				ret = pSubscription->refresh();
			}
			else
			{
				ret = pSubscription->cancelRefresh();
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnRefreshAEConditions)
				{
					OTCObjectContext objectContext;
					getOTCObjectContext((OTCObjectHandle)subscriptionHandle, &objectContext);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnRefreshAEConditions(pExecutionOptions->m_executionContext, objectContext, res, cancelRefresh);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSubscription->getObjTraceId(), (_T("OTCRefreshAEConditions - Exception occurred in vendor application callback: OTCOnRefreshAEConditions")));
					}
				}
			}

			pSubscription->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSubscription, pExecutionOptions), (ClientJob*));
			job->refreshConditions(cancelRefresh);

			if (S_OK != ((SOCltElement*)pSubscription)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSubscription->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSubscription->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCAcknowledgeAEConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnList<SOAeCEvent> eventList;
			SOCmnPointer<SOAeCEvent> event;
			SOCmnListPosition pos;
			SOAeCEvent* pEv;
			DWORD i;
			BOOL ret;
			LONG res;
			eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED);

			for (i = 0; i < count; i++)
			{
				SOCMN_ALLOCATE_OBJECT_C(event, SOAeCEvent(), (SOAeCEvent*));
				event->m_conditionName = pEvents[i].m_conditionName;
				event->m_source = pEvents[i].m_sourcePath;
				event->m_dwCookie = pEvents[i].m_cookie;
				event->m_ftActiveTime = pEvents[i].m_activeTime;
				eventList.add(event);
			}

			if (count > 0)
			{
				ret = pSession->ackConditions(&eventList, ackID, ackComment);

				if (pResults)
				{
					pos = eventList.getStartPosition();
					i = 0;

					while (pos)
					{
						pEv = eventList.getNext(pos);
						pResults[i] = pEv->m_ackRes;
						i++;
					}
				}
			}
			else
			{
				ret = TRUE;
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnAcknowledgeAEConditions)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnAcknowledgeAEConditions(pExecutionOptions->m_executionContext, objectData, res, ackID, ackComment, count, pEvents, pResults);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCAcknowledgeAEConditions - Exception occurred in vendor application callback: OTCOnAcknowledgeAEConditions")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->acknowledgeConditions(ackID, ackComment, count, pEvents);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAvailableAEFilters(IN OTCObjectHandle sessionHandle, OUT DWORD* pAvailableFilters, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			LONG res;
			res = pSession->queryAvailableFilters(pAvailableFilters);

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAvailableAEFilters)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAvailableAEFilters(pExecutionOptions->m_executionContext, objectData, res, *pAvailableFilters);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAvailableAEFilters - Exception occurred in vendor application callback: OTCOnQueryAvailableAEFilters")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->queryAvailableFilters();

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAECategories(IN OTCObjectHandle sessionHandle, OUT DWORD* pCount, OUT DWORD** ppEventTypes, OUT DWORD** ppCategoryIds, OUT LPTSTR** ppCategoryDescriptions, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (ppEventTypes != NULL)
		{
			*ppEventTypes = NULL;
		}

		if (ppCategoryIds != NULL)
		{
			*ppCategoryIds = NULL;
		}

		if (ppCategoryDescriptions != NULL)
		{
			*ppCategoryDescriptions = NULL;
		}

		if (pCount)
		{
			*pCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SOAeCEventBrowser> evB = pSession->getEventBrowser();
			SOCmnList<SOCltBrowseObject> boList;
			LONG res;
			BOOL ret;
			ret = evB->expand(SOCMNOBJECT_TYPE_CATEGORY, boList);

			if (ret)
			{
				DWORD count = boList.getCount();
				SOCmnListPosition pos;
				SOAeCCategory* pCat;
				SOCmnString catDescr;
				DWORD i;

				if (ppEventTypes)
				{
					*ppEventTypes = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
				}

				if (ppCategoryIds)
				{
					*ppCategoryIds = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
				}

				if (ppCategoryDescriptions)
				{
					*ppCategoryDescriptions = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));
				}

				pos = boList.getStartPosition();
				i = 0;

				while (pos)
				{
					pCat = (SOAeCCategory*)boList.getNext(pos);

					if (ppEventTypes)
					{
						(*ppEventTypes)[i] = pCat->m_eventType;
					}

					if (ppCategoryIds)
					{
						(*ppCategoryIds)[i] = pCat->m_id;
					}

					if (ppCategoryDescriptions)
					{
						catDescr = pCat->getName();
						(*ppCategoryDescriptions)[i] = (LPTSTR)OTAllocateMemory((catDescr.getLength() + 1) * sizeof(TCHAR));
						_tcscpy((*ppCategoryDescriptions)[i], catDescr);
					}

					i++;
				}

				if (pCount)
				{
					*pCount = count;
				}
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAECategories)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAECategories(pExecutionOptions->m_executionContext, objectData, res, *pCount, *ppEventTypes, *ppCategoryIds, *ppCategoryDescriptions);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAECategories - Exception occurred in vendor application callback: OTCOnQueryAECategories")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->queryCategories();

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAEAttributes(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT DWORD** ppAttributeIds, OUT LPTSTR** ppAttributeDescriptions, OUT WORD** ppAttributeDatatypes, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (ppAttributeDatatypes != NULL)
		{
			*ppAttributeDatatypes = NULL;
		}

		if (ppAttributeIds != NULL)
		{
			*ppAttributeIds = NULL;
		}

		if (ppAttributeDescriptions != NULL)
		{
			*ppAttributeDescriptions = NULL;
		}

		if (pCount)
		{
			*pCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SOAeCEventBrowser> evB = pSession->getEventBrowser();
			SOCmnPointer<SOAeCCategory> cat;
			SOCmnList<SOCltBrowseObject> boList;
			LONG res;
			BOOL ret;
			SOCMN_ALLOCATE_OBJECT_C(cat, SOAeCCategory(evB, L""), (SOAeCCategory*));
			cat->m_id = categoryId;
			ret = cat->expand(SOCMNOBJECT_TYPE_ATTRIBUTE, boList);

			if (ret)
			{
				DWORD count = boList.getCount();
				SOCmnListPosition pos;
				SOAeCAttribute* pAtt;
				SOCmnString attDescr;
				DWORD i;

				if (ppAttributeDatatypes)
				{
					*ppAttributeDatatypes = (WORD*)OTAllocateMemory(count * sizeof(WORD));
				}

				if (ppAttributeIds)
				{
					*ppAttributeIds = (DWORD*)OTAllocateMemory(count * sizeof(DWORD));
				}

				if (ppAttributeDescriptions)
				{
					*ppAttributeDescriptions = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));
				}

				pos = boList.getStartPosition();
				i = 0;

				while (pos)
				{
					pAtt = (SOAeCAttribute*)boList.getNext(pos);

					if (ppAttributeDatatypes)
					{
						(*ppAttributeDatatypes)[i] = pAtt->m_type;
					}

					if (ppAttributeIds)
					{
						(*ppAttributeIds)[i] = pAtt->m_id;
					}

					if (ppAttributeDescriptions)
					{
						attDescr = pAtt->getName();
						(*ppAttributeDescriptions)[i] = (LPTSTR)OTAllocateMemory((attDescr.getLength() + 1) * sizeof(TCHAR));
						_tcscpy((*ppAttributeDescriptions)[i], attDescr);
					}

					i++;
				}

				if (pCount)
				{
					*pCount = count;
				}
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAEAttributes)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAEAttributes(pExecutionOptions->m_executionContext, objectData, res, categoryId, *pCount, *ppAttributeIds, *ppAttributeDescriptions, *ppAttributeDatatypes);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAEAttributes - Exception occurred in vendor application callback: OTCOnQueryAEAttributes")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->queryAttributes(categoryId);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAEConditions(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (ppConditionsNames != NULL)
		{
			*ppConditionsNames = NULL;
		}

		if (pCount)
		{
			*pCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SOAeCEventBrowser> evB = pSession->getEventBrowser();
			SOCmnPointer<SOAeCCategory> cat;
			SOCmnList<SOCltBrowseObject> boList;
			LONG res;
			BOOL ret;
			SOCMN_ALLOCATE_OBJECT_C(cat, SOAeCCategory(evB, L""), (SOAeCCategory*));
			cat->m_id = categoryId;
			cat->m_eventType = OPC_CONDITION_EVENT;
			ret = cat->expand(SOCMNOBJECT_TYPE_CONDITION_CLASS, boList);

			if (ret)
			{
				DWORD count = boList.getCount();
				SOCmnListPosition pos;
				SOAeCConditionClass* pCond;
				SOCmnString condName;
				DWORD i;

				if (ppConditionsNames)
				{
					*ppConditionsNames = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));
				}

				pos = boList.getStartPosition();
				i = 0;

				while (pos)
				{
					pCond = (SOAeCConditionClass*)boList.getNext(pos);

					if (ppConditionsNames)
					{
						condName = pCond->getName();
						(*ppConditionsNames)[i] = (LPTSTR)OTAllocateMemory((condName.getLength() + 1) * sizeof(TCHAR));
						_tcscpy((*ppConditionsNames)[i], condName);
					}

					i++;
				}

				if (pCount)
				{
					*pCount = count;
				}
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAEConditions)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAEConditions(pExecutionOptions->m_executionContext, objectData, res, categoryId, *pCount, *ppConditionsNames);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAEConditions - Exception occurred in vendor application callback: OTCOnQueryAEConditions")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->queryConditions(categoryId);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAESubConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR conditionName, OUT DWORD* pCount, OUT LPTSTR** ppSubConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (ppSubConditionsNames != NULL)
		{
			*ppSubConditionsNames = NULL;
		}

		if (pCount)
		{
			*pCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SOAeCEventBrowser> evB = pSession->getEventBrowser();
			SOCmnPointer<SOAeCCategory> cat;
			SOCmnPointer<SOAeCConditionClass> cond;
			SOCmnList<SOCltBrowseObject> boList;
			LONG res;
			BOOL ret;
			SOCMN_ALLOCATE_OBJECT_C(cat, SOAeCCategory(evB, L""), (SOAeCCategory*));
			cat->m_eventType = OPC_CONDITION_EVENT;
			SOCMN_ALLOCATE_OBJECT_C(cond, SOAeCConditionClass(cat, L""), (SOAeCConditionClass*));
			cond->setName(conditionName);
			ret = cond->expand(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS, boList);

			if (ret)
			{
				DWORD count = boList.getCount();
				SOCmnListPosition pos;
				SOAeCSubConditionClass* pSubCond;
				SOCmnString subCondName;
				DWORD i;

				if (ppSubConditionsNames)
				{
					*ppSubConditionsNames = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));
				}

				pos = boList.getStartPosition();
				i = 0;

				while (pos)
				{
					pSubCond = (SOAeCSubConditionClass*)boList.getNext(pos);

					if (ppSubConditionsNames)
					{
						subCondName = pSubCond->getName();
						(*ppSubConditionsNames)[i] = (LPTSTR)OTAllocateMemory((subCondName.getLength() + 1) * sizeof(TCHAR));
						_tcscpy((*ppSubConditionsNames)[i], subCondName);
					}

					i++;
				}

				if (pCount)
				{
					*pCount = count;
				}
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAESubConditions)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAESubConditions(pExecutionOptions->m_executionContext, objectData, res, conditionName, *pCount, *ppSubConditionsNames);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAESubConditions - Exception occurred in vendor application callback: OTCOnQueryAESubConditions")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->querySubConditions(conditionName);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCQueryAESourceConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (ppConditionsNames != NULL)
		{
			*ppConditionsNames = NULL;
		}

		if (pCount)
		{
			*pCount = 0;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<SOAeCAreaBrowser> asB = pSession->getAreaBrowser();
			SOCmnPointer<SOAeCSource> source;
			SOCmnList<SOCltBrowseObject> boList;
			LONG res;
			BOOL ret;
			SOCMN_ALLOCATE_OBJECT_C(source, SOAeCSource(asB, L""), (SOAeCSource*));
			source->setPath(sourcePath);
			ret = source->expand(SOCMNOBJECT_TYPE_CONDITION, boList);

			if (ret)
			{
				DWORD count = boList.getCount();
				SOCmnListPosition pos;
				SOAeCCondition* pCond;
				SOCmnString condName;
				DWORD i;

				if (ppConditionsNames)
				{
					*ppConditionsNames = (LPTSTR*)OTAllocateMemory(count * sizeof(LPTSTR));
				}

				pos = boList.getStartPosition();
				i = 0;

				while (pos)
				{
					pCond = (SOAeCCondition*)boList.getNext(pos);

					if (ppConditionsNames)
					{
						condName = pCond->getName();
						(*ppConditionsNames)[i] = (LPTSTR)OTAllocateMemory((condName.getLength() + 1) * sizeof(TCHAR));
						_tcscpy((*ppConditionsNames)[i], condName);
					}

					i++;
				}

				if (pCount)
				{
					*pCount = count;
				}
			}

			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnQueryAESourceConditions)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnQueryAESourceConditions(pExecutionOptions->m_executionContext, objectData, res, sourcePath, *pCount, *ppConditionsNames);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCQueryAESourceConditions - Exception occurred in vendor application callback: OTCOnQueryAESourceConditions")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->querySourceConditions(sourcePath);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCGetAEConditionState(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds, OUT OTCAEConditionStateData* pConditionState, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			if (!pConditionState)
			{
				pSession->release();
				return E_INVALIDARG;
			}

			SOAeCConditionState condState;
			SOCmnDWORDList attributeList;
			SOCmnDWORDList* pAttributeList = NULL;
			SOAeCSubConditionState* pSubCondState;
			SOCmnListPosition pos;
			SOCmnVariant* pVariant;
			DWORD i;
			LONG res;

			if (attributeCount > 0)
			{
				attributeList.create();

				for (i = 0; i < attributeCount; i++)
				{
					attributeList.add(pAttributeIds[i]);
				}

				pAttributeList = &attributeList;
			}

			res = pSession->getConditionState(sourcePath, conditionName, pAttributeList, condState);

			if (SUCCEEDED(res))
			{
				SetOutputStructMemberString(condState.m_comment, &pConditionState->m_ackComment);
				SetOutputStructMemberString(condState.m_acknowledgerID, &pConditionState->m_ackID);
				SetOutputStructMemberString(condState.m_asc.m_definition, &pConditionState->m_actSubConditionDefinition);
				SetOutputStructMemberString(condState.m_asc.m_description, &pConditionState->m_actSubConditionDescription);
				SetOutputStructMemberString(condState.m_asc.m_name, &pConditionState->m_actSubConditionName);
				pConditionState->m_actSubConditionSeverity = condState.m_asc.m_severity;
				pConditionState->m_conditionAckTime = condState.m_lastAckTime;
				pConditionState->m_conditionActiveTime = condState.m_condLastActive;
				pConditionState->m_conditionInactiveTime = condState.m_condLastInactive;
				pConditionState->m_subConditionActiveTime = condState.m_subCondLastActive;
				pConditionState->m_quality = condState.m_quality;
				pConditionState->m_state = (BYTE)condState.m_state;
				pConditionState->m_subConditionCount = condState.m_subconds.getCount();

				if (pConditionState->m_subConditionCount)
				{
					pConditionState->m_pSubConditionDefinition = (LPTSTR*)OTAllocateMemory(pConditionState->m_subConditionCount * sizeof(LPTSTR));
					pConditionState->m_pSubConditionDescription = (LPTSTR*)OTAllocateMemory(pConditionState->m_subConditionCount * sizeof(LPTSTR));
					pConditionState->m_pSubConditionName = (LPTSTR*)OTAllocateMemory(pConditionState->m_subConditionCount * sizeof(LPTSTR));
					pConditionState->m_pSubConditionSeverity = (DWORD*)OTAllocateMemory(pConditionState->m_subConditionCount * sizeof(DWORD));
					pos = condState.m_subconds.getStartPosition();

					for (i = 0; pos; i++)
					{
						pSubCondState = condState.m_subconds.getNext(pos);
						SetOutputStructMemberString(pSubCondState->m_definition, &pConditionState->m_pSubConditionDefinition[i]);
						SetOutputStructMemberString(pSubCondState->m_description, &pConditionState->m_pSubConditionDescription[i]);
						SetOutputStructMemberString(pSubCondState->m_name, &pConditionState->m_pSubConditionName[i]);
						pConditionState->m_pSubConditionSeverity[i] = pSubCondState->m_severity;
					}
				}
				else
				{
					pConditionState->m_pSubConditionDefinition = NULL;
					pConditionState->m_pSubConditionDescription = NULL;
					pConditionState->m_pSubConditionName = NULL;
					pConditionState->m_pSubConditionSeverity = NULL;
				}

				pConditionState->m_eventAttrCount = condState.m_eventAttrs.getCount();

				if (pConditionState->m_eventAttrCount)
				{
					pConditionState->m_pAttrErrors = (LONG*)OTAllocateMemory(pConditionState->m_eventAttrCount * sizeof(LONG));
					pConditionState->m_pEventAttrs = (OTVariant*)OTAllocateMemory(sizeof(OTVariant) * pConditionState->m_eventAttrCount);
					pos = condState.m_eventAttrs.getStartPosition();

					for (i = 0; pos; i++)
					{
						pVariant = condState.m_eventAttrs.getNext(pos);
						pVariant->copyTo(&pConditionState->m_pEventAttrs[i]);
						pConditionState->m_pAttrErrors[i] = S_OK;
					}
				}
				else
				{
					pConditionState->m_pAttrErrors = NULL;
					pConditionState->m_pEventAttrs = NULL;
				}
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnGetAEConditionState)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnGetAEConditionState(pExecutionOptions->m_executionContext, objectData, res, sourcePath, conditionName, attributeCount, pAttributeIds, pConditionState);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCGetAEConditionState - Exception occurred in vendor application callback: OTCOnGetAEConditionState")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->getConditionState(sourcePath, conditionName, attributeCount, pAttributeIds);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTCAPI_CALL OTCEnableAEConditions(IN OTCObjectHandle sessionHandle, IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOAeCServer* pSession = (SOAeCServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if (!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			LONG res;

			if (areaOrSource)
			{
				res = pSession->enableConditionByArea(path, enable);
			}
			else
			{
				res = pSession->enableConditionBySource(path, enable);
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnEnableAEConditions)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnEnableAEConditions(pExecutionOptions->m_executionContext, objectData, res, enable, areaOrSource, path);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCEnableAEConditions - Exception occurred in vendor application callback: OTCOnEnableAEConditions")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->enableConditions(enable, areaOrSource, path);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#else // OTB_CLIENT_AE
LONG OTCAPI_CALL OTCAddAESession(IN LPTSTR url, IN DWORD sessionUserData, OUT OTCObjectHandle* pObjectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddAESubscription(IN OTCObjectHandle sessionHandle, IN DWORD subscriptionUserData, OUT OTCObjectHandle* pSubscriptionHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveAESession(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveAESubscription(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRefreshAEConditions(IN OTCObjectHandle subscriptionHandle, IN BYTE cancelRefresh, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAcknowledgeAEConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAvailableAEFilters(IN OTCObjectHandle sessionHandle, OUT DWORD* pAvailableFilters, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAECategories(IN OTCObjectHandle sessionHandle, OUT DWORD* pCount, OUT DWORD** ppEventTypes, OUT DWORD** ppCategoryIds, OUT LPTSTR** ppCategoryDescriptions, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAEAttributes(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT DWORD** ppAttributeIds, OUT LPTSTR** ppAttributeDescriptions, OUT WORD** ppAttributeDatatypes, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAEConditions(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAESubConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR conditionName, OUT DWORD* pCount, OUT LPTSTR** ppSubConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAESourceConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetAEConditionState(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds, OUT OTCAEConditionStateData* pConditionState, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCEnableAEConditions(IN OTCObjectHandle sessionHandle, IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
#endif // OTB_CLIENT_AE


#ifdef SOFEATURE_DCOM
LONG OTCAPI_CALL OTCGetErrorString(IN OTCObjectHandle sessionHandle, IN LONG errorCode, OUT LPTSTR* pErrorString, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltServer* pSession = (SOCltServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		pSession->addRef();

		if ((!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE)) && (!pSession->is(SOCMNOBJECT_TYPE_SERVER_DA)))
		{
			pSession->release();
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			if (!pErrorString)
			{
				pSession->release();
				return E_INVALIDARG;
			}

			SOCmnString string;
			LONG res;
			res = pSession->getErrorString(errorCode, &string);

			if (SUCCEEDED(res))
			{
				*pErrorString = (LPTSTR)OTAllocateMemory((string.getLength() + 1) * sizeof(TCHAR));
				_tcscpy(*pErrorString, (LPCTSTR)string);
			}
			else
			{
				pErrorString = NULL;
			}

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
			{
				// call callback
				if (g_callbackFunctions.m_OTCOnGetErrorString)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnGetErrorString(pExecutionOptions->m_executionContext, objectData, res, errorCode, *pErrorString);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCGetErrorString - Exception occurred in vendor application callback: OTCOnGetErrorString")));
					}
				}
			}

			pSession->release();
			return res;
		}
		else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
		{
			SOCmnPointer<ClientJob> job;
			SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
			job->getErrorString(errorCode);

			if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
			{
				pSession->release();
				return OT_E_CANTEXECUTEASYNC;
			}

			pSession->release();
			return S_OK;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCLogon(IN OTCObjectHandle sessionHandle, IN LPTSTR userName, IN LPTSTR password, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltServer* pSession = (SOCltServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		if ((!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE)) && (!pSession->is(SOCMNOBJECT_TYPE_SERVER_DA)))
		{
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pSession->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				BOOL ret = FALSE;
				LONG res = E_FAIL;
#ifdef OTB_CLIENT_DA

				if (pSession->is(SOCMNOBJECT_TYPE_SERVER_DA))
				{
					ret = ((ClientDASessionCom*)pSession)->logon(userName, password);
				}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

				if (pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
				{
					ret = ((ClientAESessionCom*)pSession)->logon(userName, password);
				}

#endif // OTB_CLIENT_AE
				res = (ret) ? S_OK : E_FAIL;

				if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
				{
					// call callback
					if (g_callbackFunctions.m_OTCOnLogon)
					{
						OTCObjectData objectData;
						getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

						SOCMN_TRY
						{
							g_callbackFunctions.m_OTCOnLogon(pExecutionOptions->m_executionContext, objectData, res, userName, password);
						}
						SOCMN_CATCH(...)
						{
							_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogon - Exception occurred in vendor application callback: OTCOnLogon")));
						}
					}
				}

				return res;
			}
			else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				SOCmnPointer<ClientJob> job;
				SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
				job->logon(userName, password);

				if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
				{
					return OT_E_CANTEXECUTEASYNC;
				}

				return S_OK;
			}
		}

#ifdef SOFEATURE_TUNNEL
		else if (pSession->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			BOOL ret = FALSE;
			LONG res;
			pSession->setUserName(userName);
			pSession->setPassword(password);
#ifdef OTB_CLIENT_DA

			if (pSession->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				ret = ((ClientDASessionTp*)pSession)->logon(userName, password);
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				ret = ((ClientAESessionTp*)pSession)->logon(userName, password);
			}

#endif // OTB_CLIENT_AE
			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				if (g_callbackFunctions.m_OTCOnLogon)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnLogon(pExecutionOptions->m_executionContext, objectData, res, userName, password);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogon - Exception occurred in vendor application callback: OTCOnLogon")));
					}
				}
			}

			return res;
		}

#endif // SOFEATURE_TUNNEL
		else{

			pSession->setUserName(userName);
			pSession->setPassword(password);

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				if (g_callbackFunctions.m_OTCOnLogon)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnLogon(pExecutionOptions->m_executionContext, objectData, S_OK, userName, password);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogon - Exception occurred in vendor application callback: OTCOnLogon")));
					}
				}
			}

			return S_OK;

		}   //  end if ... else

	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTCAPI_CALL OTCLogoff(IN OTCObjectHandle sessionHandle, IN OTCExecutionOptions* pExecutionOptions)
{
	SOCMN_TRY
	{
		OTCExecutionOptions locExecutionOptions;

		if (!pExecutionOptions)
		{
			locExecutionOptions.m_executionType = OTC_EXECUTIONTYPE_SYCHRONOUS;
			pExecutionOptions = &locExecutionOptions;
		}

		SOCltServer* pSession = (SOCltServer*)(SOCmnObject*)(void*)GET_MEMORY_ADDRESS(sessionHandle);

		if (!pSession)
		{
			return OT_E_INVALIDHANDLE;
		}

		if ((!pSession->is(SOCMNOBJECT_TYPE_SERVER_AE)) && (!pSession->is(SOCMNOBJECT_TYPE_SERVER_DA)))
		{
			return OT_E_INVALIDOBJECTTYPE;
		}

		if (pSession->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			if (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				BOOL ret = FALSE;
				LONG res = E_FAIL;
#ifdef OTB_CLIENT_DA

				if (pSession->is(SOCMNOBJECT_TYPE_SERVER_DA))
				{
					ret = ((ClientDASessionCom*)pSession)->logoff();
				}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

				if (pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
				{
					ret = ((ClientAESessionCom*)pSession)->logoff();
				}

#endif // OTB_CLIENT_AE
				res = (ret) ? S_OK : E_FAIL;

				if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE)
				{
					// call callback
					if (g_callbackFunctions.m_OTCOnLogoff)
					{
						OTCObjectData objectData;
						getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

						SOCMN_TRY
						{
							g_callbackFunctions.m_OTCOnLogoff(pExecutionOptions->m_executionContext, objectData, res);
						}
						SOCMN_CATCH(...)
						{
							_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogoff - Exception occurred in vendor application callback: OTCOnLogoff")));
						}
					}
				}

				return res;
			}
			else // (pExecutionOptions->m_executionType != OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				SOCmnPointer<ClientJob> job;
				SOCMN_ALLOCATE_OBJECT_C(job, ClientJob(pSession, pExecutionOptions), (ClientJob*));
				job->logoff();

				if (S_OK != ((SOCltElement*)pSession)->executeTask(job, SOCLT_EXEC_ASYNC, NULL))
				{
					return OT_E_CANTEXECUTEASYNC;
				}

				return S_OK;
			}
		}

#ifdef SOFEATURE_TUNNEL
		else if (pSession->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			BOOL ret = FALSE;
			LONG res;
			pSession->setUserName(NULL);
			pSession->setPassword(NULL);
#ifdef OTB_CLIENT_DA

			if (pSession->is(SOCMNOBJECT_TYPE_SERVER_DA))
			{
				ret = ((ClientDASessionTp*)pSession)->logoff();
			}

#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

			if (pSession->is(SOCMNOBJECT_TYPE_SERVER_AE))
			{
				ret = ((ClientAESessionTp*)pSession)->logoff();
			}

#endif // OTB_CLIENT_AE
			res = (ret) ? S_OK : E_FAIL;

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				if (g_callbackFunctions.m_OTCOnLogoff)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnLogoff(pExecutionOptions->m_executionContext, objectData, res);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogoff - Exception occurred in vendor application callback: OTCOnLogoff")));
					}
				}
			}

			return res;
		}

#endif // SOFEATURE_TUNNEL
		else{

			pSession->setUserName(NULL);
			pSession->setPassword(NULL);

			if (pExecutionOptions->m_executionType == OTC_EXECUTIONTYPE_ASYCHRONOUS)
			{
				if (g_callbackFunctions.m_OTCOnLogoff)
				{
					OTCObjectData objectData;
					getOTCObjectData((OTCObjectHandle)sessionHandle, &objectData);

					SOCMN_TRY
					{
						g_callbackFunctions.m_OTCOnLogoff(pExecutionOptions->m_executionContext, objectData, S_OK);
					}
					SOCMN_CATCH(...)
					{
						_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, pSession->getObjTraceId(), (_T("OTCLogoff - Exception occurred in vendor application callback: OTCOnLogoff")));
					}
				}
			}

			return S_OK;

		}   //  end if ... else

	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#else // SOFEATURE_DCOM
LONG OTCAPI_CALL OTCGetErrorString(IN OTCObjectHandle sessionHandle, IN LONG errorCode, OUT LPTSTR* pErrorString, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCLogon(IN OTCObjectHandle sessionHandle, IN LPTSTR userName, IN LPTSTR password, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCLogoff(IN OTCObjectHandle sessionHandle, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
#endif // SOFEATURE_DCOM

#else // OTB_CLIENT

LONG OTCAPI_CALL OTCInitialize(IN OTCInitData* pInitData)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCTerminate(void)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAdvise(IN OTCCallbackFunctions* pCallbacks)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddDASession(IN LPTSTR url, IN DWORD sessionUserData, OUT OTCObjectHandle* pObjectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddDASubscription(IN OTCObjectHandle sessionHandle, IN DWORD updateRate, IN DWORD groupUserData, OUT OTCObjectHandle* pGroupHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddDAItem(IN OTCObjectHandle groupHandle, IN LPTSTR itemID, IN DWORD itemUserData, OUT OTCObjectHandle* pItemHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDASession(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDASubscription(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveDAItem(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetState(IN OTCObjectHandle objectHandle, OUT BYTE* pCurrentState, OUT BYTE* pTargetState)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCChangeTargetState(IN OTCObjectHandle objectHandle, IN BYTE state, IN BYTE deep)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCPerformStateTransition(IN OTCObjectHandle objectHandle, IN BYTE deep, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRead(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN DWORD maxAge, OUT OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCWrite(IN OTCObjectHandle objectHandle, IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPTSTR* pItemIDs, IN LPTSTR* pItemPaths, IN OTCValueData* pValues, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetAttributes(IN OTCObjectHandle objectHandle, IN DWORD whatAttributes, OUT OTCObjectAttributes* pObjectAttributes)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCSetAttributes(IN OTCObjectHandle objectHandle, IN DWORD whatAttributes, IN OTCObjectAttributes* pObjectAttributes)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCUpdateAttributes(IN OTCObjectHandle objectHandle, IN BYTE fromServer, IN DWORD attributeCount, IN DWORD* pWhatAttributes, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCUpdateDAItemAttributes(IN OTCObjectHandle subscriptionHandle, IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, IN BYTE fromServer, IN DWORD attributeCount, IN DWORD* pWhatAttributes, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetServerStatus(IN OTCObjectHandle sessionHandle, OUT OTCServerStatus* pServerStatus, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCValidateDAItems(IN OTCObjectHandle subscriptionHandle, IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCActivateConnectionMonitor(IN OTCObjectHandle sessionHandle, BYTE activate, DWORD checkCycle, DWORD connect1Attempts, DWORD connect1Cycle, DWORD connect2Cycle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCBrowseServer(IN LPTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData, OUT DWORD* pServerDataCount, OUT OTCServerData** ppServerData, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCBrowseAddressSpace(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, OUT DWORD* pAddressSpaceElementDataCount, OUT OTCAddressSpaceElementData** ppAddressSpaceElement, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCReleaseAddressSpaceElement(IN OTCObjectHandle addressSpaceElementHandle, IN BOOL deep)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetDAProperties(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN LPTSTR addressSpaceElementID, IN LPTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, OUT DWORD* pPropertiesDataCount, OUT OTCDAPropertyData** ppProperty, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRefreshDAGroup(IN OTCObjectHandle subscriptionHandle, IN DWORD maxAge, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddAESession(IN LPTSTR url, IN DWORD sessionUserData, OUT OTCObjectHandle* pObjectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAddAESubscription(IN OTCObjectHandle sessionHandle, IN DWORD subscriptionUserData, OUT OTCObjectHandle* pSubscriptionHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveAESession(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRemoveAESubscription(IN OTCObjectHandle objectHandle)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCRefreshAEConditions(IN OTCObjectHandle subscriptionHandle, IN BYTE cancelRefresh, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCAcknowledgeAEConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents, OUT LONG* pResults, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAvailableAEFilters(IN OTCObjectHandle sessionHandle, OUT DWORD* pAvailableFilters, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAECategories(IN OTCObjectHandle sessionHandle, OUT DWORD* pCount, OUT DWORD** ppEventTypes, OUT DWORD** ppCategoryIds, OUT LPTSTR** ppCategoryDescriptions, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAEAttributes(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT DWORD** ppAttributeIds, OUT LPTSTR** ppAttributeDescriptions, OUT WORD** ppAttributeDatatypes, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAEConditions(IN OTCObjectHandle sessionHandle, IN DWORD categoryId, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAESubConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR conditionName, OUT DWORD* pCount, OUT LPTSTR** ppSubConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCQueryAESourceConditions(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, OUT DWORD* pCount, OUT LPTSTR** ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetAEConditionState(IN OTCObjectHandle sessionHandle, IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds, OUT OTCAEConditionStateData* pConditionState, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCEnableAEConditions(IN OTCObjectHandle sessionHandle, IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCGetErrorString(IN OTCObjectHandle sessionHandle, IN LONG errorCode, OUT LPTSTR* pErrorString, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCLogon(IN OTCObjectHandle sessionHandle, IN LPTSTR userName, IN LPTSTR password, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCLogoff(IN OTCObjectHandle sessionHandle, IN OTCExecutionOptions* pExecutionOptions)
{
	return E_NOTIMPL;
}
LONG OTCAPI_CALL OTCProcessCommandLine(IN LPTSTR commandLine)
{
	return E_NOTIMPL;
}

#endif // OTB_CLIENT

