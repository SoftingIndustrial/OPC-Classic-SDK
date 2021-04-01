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
//                            OPC Toolkit - OTS                               |
//                                                                            |
//  Filename    : OTS.cpp                                                     |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Programming interface of OPC Toolkit C Server (OTS)         |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "OTServer.h"

#ifdef OTB_SERVER


#include "SrvApp.h"
#ifdef OTB_SERVER_DA
#include "ServerDANode.h"
#include "ServerDATag.h"
#include "ServerDAProperty.h"
#include "ServerDAServer.h"
#include "ServerDAGroup.h"
#include "ServerDAItem.h"
#include "ServerDACache.h"
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
#include "ServerAEArea.h"
#include "ServerAESource.h"
#include "ServerAEServer.h"
#include "ServerAECondition.h"
#include "ServerAEConditionClass.h"
#endif // OTB_SERVER_AE
#include "OTB.h"

#include "SOVersion.h"
#include "SOModule.h"
#include "SOCmnHandleManager.h"

//-----------------------------------------------------------------------------
// OTSInitialize
//
// - initializes the library with the initialization data of the server
//   application
//
// returns:
//		S_OK                - initialized the library
//		OT_E_WRONG_VERSION  - init data are for a newer OTS version
//		OT_E_INVALIDDATA    - passed data is invalid
//		E_INVALIDARG        - passed invalid function parameter
//		E_FAIL              - internal error during the initialization
//		OT_E_EXCEPTION      - exception in function
//
LONG OTSAPI_CALL OTSInitialize(
	IN OTSInitData* pInitData) // initialization data
{
	SOCMN_TRY
	{
		if (!pInitData)
		{
			return E_INVALIDARG;
		}

		SHORT curVersion = (MAJOR_VERSION_NUMBER * 100) + MINOR_VERSION_NUMBER;

		if (pInitData->m_versionOTS > curVersion)
		{
			return OT_E_WRONG_VERSION;
		}

		if (pInitData->m_optimizeForSpeed == FALSE)
		{
			SOCmnListHeader::skipRehashing();
		}

#ifndef OTB_USER_COMPILED
		updateLicenseInfo(OTS_FEATURE_DA_SERVER);
		updateLicenseInfo(OTS_FEATURE_XMLDA_SERVER);
		updateLicenseInfo(OTS_FEATURE_AE_SERVER);
		updateLicenseInfo(OTS_FEATURE_TP_SERVER);
#endif // OTB_USER_COMPILED

		// initialize the application data
		ServerApp* pApp = ::getServerApp();
#ifdef SOOS_WINDOWS
		pApp->m_serviceName = pInitData->m_serviceName;
		pApp->m_serviceDescription = pInitData->m_serviceDescription;
#ifdef OTB_SERVER_DAAE
		SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
		entry->setServiceName(pApp->m_serviceName);
#else
#ifdef OTB_SERVER_DA
		SOCmnPointer<SOSrvEntry> entryDA = getSODaSEntry();
		entryDA->setServiceName(pApp->m_serviceName);
#endif
#ifdef OTB_SERVER_AE
		SOCmnPointer<SOSrvEntry> entryAE = getSOAeSEntry();
		entryAE->setServiceName(pApp->m_serviceName);
#endif
#endif
#endif // SOOS_WINDOWS
#ifdef SOFEATURE_DCOM
		USES_CONVERSION;

		if (pInitData->m_clsidDA != NULL && _tcslen(pInitData->m_clsidDA) > 0)
		{
			if (FAILED(CLSIDFromString((LPOLESTR)T2CW(pInitData->m_clsidDA), &pApp->m_clsidDA)))
			{
				return OT_E_INVALIDDATA;
			}
		}
		else
		{
			memset(&pApp->m_clsidDA, 0, sizeof(CLSID));
		}

		if (pInitData->m_clsidAE && _tcslen(pInitData->m_clsidAE) > 0)
		{
			if (FAILED(CLSIDFromString((LPOLESTR)T2CW(pInitData->m_clsidAE), &pApp->m_clsidAE)))
			{
				return OT_E_INVALIDDATA;
			}
		}
		else
		{
			memset(&pApp->m_clsidAE, 0, sizeof(CLSID));
		}
#endif

#ifdef SOOS_WINDOWS
		pApp->m_instance = GetModuleHandle(_T(ORIGINAL_FILENAME));
#endif // SOOS_WINDOWS

#ifdef SOFEATURE_DCOM
		pApp->m_progIdDA = pInitData->m_progIdDA;
		pApp->m_verIndProgIdDA = pInitData->m_verIndProgIdDA;
		pApp->m_progIdAE = pInitData->m_progIdAE;
		pApp->m_verIndProgIdAE = pInitData->m_verIndProgIdAE;
		pApp->m_authLevel = pInitData->m_authLevel;
		pApp->m_impLevel = pInitData->m_impLevel;
		pApp->m_cbAuthLevel = pInitData->m_cbAuthLevel;
		pApp->m_cbImpLevel = pInitData->m_cbImpLevel; 
#endif
		pApp->m_serverName = pInitData->m_description;
		pApp->m_ipPortHTTP = (WORD)pInitData->m_ipPortHTTP;
		pApp->m_urlDA = pInitData->m_urlDA;
		pApp->m_majorVersionNumber = pInitData->m_majorVersion;
		pApp->m_minorVersionNumber = pInitData->m_minorVersion;
		pApp->m_patchVersionNumber = pInitData->m_patchVersion;
		pApp->m_buildNumber = pInitData->m_buildNumber;
		pApp->m_vendorInfo = pInitData->m_vendorInfo;
		pApp->m_appType = (BYTE)((SOSrvEntry::serverTypeCOM)pInitData->m_appType);
		pApp->m_minUpdateRateDA = pInitData->m_minUpdateRateDA;
		pApp->m_addressSpaceDelimiter = pInitData->m_addressSpaceDelimiter;
		pApp->m_propertyDelimiter = pInitData->m_propertyDelimiter;
		pApp->m_clientCheckPeriod = pInitData->m_clientCheckPeriod;
		pApp->m_supportDisableConditions = pInitData->m_supportDisableConditions;
		pApp->m_webRootDirectory = pInitData->m_webRootDirectory;
		pApp->m_webAdministratorPassword = pInitData->m_webAdministratorPassword;
		pApp->m_webOperatorPassword = pInitData->m_webOperatorPassword;
		pApp->m_stringBasedObjectDataExpiry = pInitData->m_stringBasedObjectDataExpiry;
		pApp->m_optimizeForSpeed = pInitData->m_optimizeForSpeed;

#ifdef SOFEATURE_TUNNEL
		pApp->m_tpPort = pInitData->m_tpPort;

		if ((pInitData->m_tpCredentialsNumber > 0) &&
		(pInitData->m_tpUsers != NULL) &&
		(pInitData->m_tpPasswords != NULL))
		{
			int index = 0;

			for (index = 0; index < pInitData->m_tpCredentialsNumber; index++)
			{
				if (pInitData->m_tpUsers[index] != NULL)
				{
					pApp->m_tpUserList.add(pInitData->m_tpUsers[index]);
				}   //  end if

				if (pInitData->m_tpPasswords[index] != NULL)
				{
					pApp->m_tpPwdList.add(pInitData->m_tpPasswords[index]);
				}   //  end if
			}   //  end for
		}   //  end if

#endif  //  SOFEATURE_TUNNEL

		// initialize the OPC server application
		if (!pApp->initialize())
		{
#ifdef SOFEATURE_DCOM
			::CoUninitialize();
#endif
			return E_FAIL;
		}

		// prepare the OPC toolkit
		if (!pApp->prepare())
		{
			pApp->terminate();
#ifdef SOFEATURE_DCOM
			CoUninitialize();
#endif
			return E_FAIL;
		}

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSInitialize


//-----------------------------------------------------------------------------
// OTSTerminate
//
// - frees the resources used by the library
//
// returns:
//		S_OK                - terminated the library
//		OT_E_EXCEPTION     - exception in function
//
LONG OTSAPI_CALL OTSTerminate(void)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		pApp->finalize();
		pApp->terminate();

#ifdef SOFEATURE_DCOM
		CoUninitialize();
#endif

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSTerminate


//-----------------------------------------------------------------------------
// OTSProcessCommandLine
//
// - processes the command line actions of an OPC server
//
// returns:
//		S_OK                - terminated the library
//		OT_E_EXCEPTION     - exception in function
//
LONG OTSAPI_CALL OTSProcessCommandLine(
	IN LPTSTR commandLine) // command line of the server application
{
	SOCMN_TRY
	{
		if (!commandLine)
		{
			return E_INVALIDARG;
		}

		ServerApp* pApp = ::getServerApp();

		if (!pApp->processCommandLine(commandLine))
		{
			return S_OK;
		}
		else
		{ return S_FALSE; }
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSProcessCommandLine


//-----------------------------------------------------------------------------
// OTSSetServerState
//
// - sets the server specific state according to vendor application
//
// returns:
//		S_OK                - server state set successfully
//		E_FAIL              - failed to set server state
//		OT_E_EXCEPTION      - exception in function
//
LONG OTSAPI_CALL OTSSetServerState(IN DWORD daServerState, IN DWORD aeServerState)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		if (pApp != NULL)
		{
		LONG res = E_FAIL;
#ifdef OTB_SERVER_DA
			switch(daServerState)
			{
			case 0:
				res = S_OK;
				break;
			case OPC_STATUS_RUNNING:
			case OPC_STATUS_FAILED:
			case OPC_STATUS_NOCONFIG:
			case OPC_STATUS_SUSPENDED:
			case OPC_STATUS_TEST:
			case OPC_STATUS_COMM_FAULT:
				{
					OPCSERVERSTATE opcDaServerState = (OPCSERVERSTATE) daServerState;
					SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
					entryDA->setServerState(opcDaServerState);
					res = OT_E_EXCEPTION;
					break;
				}
			default:
				return E_INVALIDARG;
			}
#endif
#ifdef OTB_SERVER_AE
			switch(aeServerState)
			{
			case 0:
				res = S_OK;
				break;
			case OPCAE_STATUS_RUNNING:
			case OPCAE_STATUS_FAILED:
			case OPCAE_STATUS_NOCONFIG:
			case OPCAE_STATUS_SUSPENDED:
			case OPCAE_STATUS_TEST:
			case OPCAE_STATUS_COMM_FAULT:
				{
					OPCEVENTSERVERSTATE opcAeServerState = (OPCEVENTSERVERSTATE) aeServerState;
					SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
					entryAE->setServerState(opcAeServerState);
					res = S_OK;
					break;
				}
			default:
				return E_INVALIDARG;
			}
#endif
			return res;
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
} // OTSSetServerState


//-----------------------------------------------------------------------------
// OTSStart
//
// - starts the server features of the OTS
//
// returns:
//		S_OK                - started server features
//		E_FAIL              - failed to start server
//		OT_E_EXCEPTION      - exception in function
//
LONG OTSAPI_CALL OTSStart(void)
{
	SOCMN_TRY
	{
		// start the I/O
		ServerApp* pApp = ::getServerApp();

		if (pApp->start())
		{
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
} // OTSStart


//-----------------------------------------------------------------------------
// OTSReady
//
// - signals that the addressSpace and the I/O handling of the server application
//   are ready
//
// returns:
//		S_OK                - signaled OTS, server automatically set in ready state
//		S_FALSE             - signaled OTS, server not set in ready state (previous server state was set by vendor application)
//		OT_E_EXCEPTION      - exception in function
//
LONG OTSAPI_CALL OTSReady(void)
{
	SOCMN_TRY
	{
		LONG res = S_OK;
#ifdef OTB_SERVER_DA
		SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
		if (entryDA->setAutoServerState() == FALSE)
		{
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("OTSReady"),
				(_T("OPC DA Server state WAS NOT automatically advanced from default state (NOCONFIG) to ready state (RUNNING) as server state was previously updated by vendor.")));
			res = S_FALSE;
		}
#endif
#ifdef OTB_SERVER_AE
		SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
		if (entryAE->setAutoServerState() == FALSE)
		{
			_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("OTSReady"),
				(_T("OPC AE Server state WAS NOT automatically advanced from default state (NOCONFIG) to ready state (RUNNING) as server state was previously updated by vendor.")));
			res = S_FALSE;
		}
#endif

		ServerApp* pServerApp = getServerApp();

		if (pServerApp->m_startupEvent)
		{
			pServerApp->m_startupEvent->signal();
		}

		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSReady


//-----------------------------------------------------------------------------
// OTSStop
//
// - stops the server features of the OTS
//
// returns:
//		S_OK                - stopped server features
//		OT_E_EXCEPTION     - exception in function
//
LONG OTSAPI_CALL OTSStop(OTChar* reason)
{
	SOCMN_TRY
	{
		SOCmnListHeader::skipRehashing();
		ServerApp* pApp = ::getServerApp();
		SOCmnString shutdownReason;

		if (reason != NULL)
		{
			shutdownReason = reason;
		}

		pApp->stop(shutdownReason);
		pApp->unload();
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSStop


//-----------------------------------------------------------------------------
// OTSAdvise
//
// - advises the OTS callbacks of the server application
//
// returns:
//		S_OK                - set callbacks
//		E_INVALIDARG        - passed invalid function parameter
//		OT_E_EXCEPTION     - exception in function
//
LONG OTSAPI_CALL OTSAdvise(
	IN OTSCallbackFunctions* pCallbacks) // callback functions
{
	SOCMN_TRY
	{
		if (!pCallbacks)
		{
			return E_INVALIDARG;
		}

		ServerApp* pApp = ::getServerApp();
		pApp->m_callbackFunctions = *pCallbacks;
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
} // OTSAdvise


LONG getObject(OTSObjectHandle hObj, DWORD acceptedType, SOCmnObject** pObj)
{
	SOCMN_TRY
	{
		SOCmnElement* pElement = (SOCmnElement*)(void*)GET_MEMORY_ADDRESS(hObj);

		if (!pElement)
		{
			return OT_E_INVALIDHANDLE;
		}

		pElement->addRef();

		if (pElement->is(acceptedType))
		{
			if (pObj)
			{
				*pObj = (SOCmnObject*)pElement;
				if (!(*pObj))
				{
					pElement->release();
					return OT_E_INVALIDHANDLE;
				}
				(*pObj)->addRef();
			}

			pElement->release();
			return S_OK;
		}
		else
		{
			pElement->release();
			return OT_E_INVALIDOBJECTTYPE;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_INVALIDHANDLE;
	}
}

LONG getObjectType(OTSObjectHandle hObj, DWORD* pType)
{
	SOCmnElement* pElement = (SOCmnElement*)(void*)GET_MEMORY_ADDRESS(hObj);

	if (!pElement)
	{
		return OT_E_INVALIDHANDLE;
	}

	pElement->addRef();
	*pType = pElement->getObjectType();
	pElement->release();
	return S_OK;
}


LONG OTSAPI_CALL OTSAddAddressSpaceElement(
	IN OTSObjectHandle hParent,
	IN OTSAddressSpaceElementData* pData,
	OUT OTSObjectHandle* phObject)
{
	SOCMN_TRY
	{
		if (!pData)
		{
			return E_INVALIDARG;
		}

		SOCmnPointer<SOCmnElement> parent;
		DWORD res;

		if (pData->m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
		{
#ifdef OTB_SERVER_DA

			if (hParent != 0)
			{
				res = getObject(hParent, SOCMNOBJECT_TYPE_NODE, (SOCmnObject**)&parent);

				if (FAILED(res))
				{
					return res;
				}
			}
			else
			{
				// parent is the root object
				SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
				parent = entryDA->getNameSpaceRoot();
			}

			if (!pData->m_name)
			{
				return OT_E_INVALIDDATA;
			}

			if (pData->m_ioMode != OTS_IOMODE_NONE)
				if (((pData->m_accessRights & (OPC_READABLE | OPC_WRITEABLE)) != pData->m_accessRights) ||
					(!SOCmnVariant::supportDatatype((VARTYPE)pData->m_datatype)))
				{
					return OT_E_INVALIDDATA;
				}

			if (pData->m_hasChildren)
			{
				// add node
				SOCmnPointer<ServerDANode> node;
				SOCMN_ALLOCATE_OBJECT_C(node, ServerDANode, (ServerDANode*))
				node->setName(pData->m_name);

				if (pData->m_ioMode != OTS_IOMODE_NONE)
				{
					node->setAccessRights(pData->m_accessRights);
					node->setNativeDatatype((VARTYPE)pData->m_datatype);
				}

				node->setOTSUserData(pData->m_userData);
				node->setIsBrowsable(pData->m_isBrowsable);

				switch (pData->m_ioMode)
				{
				case OTS_IOMODE_POLL:
				case OTS_IOMODE_POLL_OWNCACHE:
					node->setDeviceIOMode(SODaSItemTag::transaction);
					node->setUpdateMode(SODaSItemTag::poll);

					if (pData->m_ioMode == OTS_IOMODE_POLL_OWNCACHE)
					{
						node->setOwnCache(TRUE);
					}

					break;

				case OTS_IOMODE_REPORT:
				case OTS_IOMODE_REPORT_CYCLIC:
					node->setDeviceIOMode(SODaSItemTag::transaction);
					node->setUpdateMode(SODaSItemTag::report);

					if (pData->m_ioMode == OTS_IOMODE_REPORT_CYCLIC)
					{
						node->setReportCyclic(TRUE);
					}

					break;

				case OTS_IOMODE_NONE:
					node->setDeviceIOMode(SODaSItemTag::noDeviceIO);
					break;

				default:
					return OT_E_INVALIDDATA;
					break;
				}

				if (parent->addBranch(node))
				{
					if (phObject)
					{
						*phObject = (DWORD)node->getOpcHandle();
					}

					node->create(parent, node);
					return S_OK;
				}
				else
				{
					// duplicate name
					return OT_E_DUPLICATENAME;
				}
			} // add node
			else
			{
				// add tag
				SOCmnPointer<ServerDATag> tag;
				SOCMN_ALLOCATE_OBJECT_C(tag, ServerDATag, (ServerDATag*))
				tag->setName(pData->m_name);
				tag->setAccessRights(pData->m_accessRights);
				tag->setNativeDatatype((VARTYPE)pData->m_datatype);
				tag->setOTSUserData(pData->m_userData);
				tag->setIsBrowsable(pData->m_isBrowsable);

				switch (pData->m_ioMode)
				{
				case OTS_IOMODE_POLL:
				case OTS_IOMODE_POLL_OWNCACHE:
					tag->setUpdateMode(SODaSItemTag::poll);

					if (pData->m_ioMode == OTS_IOMODE_POLL_OWNCACHE)
					{
						tag->setOwnCache(TRUE);
					}

					break;

				case OTS_IOMODE_REPORT:
				case OTS_IOMODE_REPORT_CYCLIC:
					tag->setUpdateMode(SODaSItemTag::report);

					if (pData->m_ioMode == OTS_IOMODE_REPORT_CYCLIC)
					{
						tag->setReportCyclic(TRUE);
					}

					break;

				default:
					return OT_E_INVALIDDATA;
					break;
				}

				if (parent->addLeaf(tag))
				{
					if (phObject)
					{
						*phObject = (DWORD)tag->getOpcHandle();
					}

					tag->create(parent, tag);
					return S_OK;
				}
				else
				{
					// duplicate name
					return OT_E_DUPLICATENAME;
				}
			} // add tag

#else // OTB_SERVER_DA
			return E_NOTIMPL;
#endif // OTB_SERVER_DA
		} // DA
		else if (pData->m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_AE)
		{
#ifdef OTB_SERVER_AE

			if (hParent != NULL)
			{
				res = getObject(hParent, SOCMNOBJECT_TYPE_AREA, (SOCmnObject**)&parent);

				if (FAILED(res))
				{
					return res;
				}
			}
			else
			{
				// parent is the root object
				SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
				parent = entryAE->getAreaSpaceRoot();
			}

			if (!pData->m_name)
			{
				return OT_E_INVALIDDATA;
			}

			if (pData->m_hasChildren)
			{
				// add area
				SOCmnPointer<ServerAEArea> area;
				SOCMN_ALLOCATE_OBJECT_C(area, ServerAEArea, (ServerAEArea*))
				area->setName(pData->m_name);
				area->setOTSUserData(pData->m_userData);
				area->setIsBrowsable(pData->m_isBrowsable);

				if (parent->addBranch(area))
				{
					if (phObject)
					{
						*phObject = (DWORD)area->getOpcHandle();
					}

					return S_OK;
				}
				else
				{
					// duplicate name
					return OT_E_DUPLICATENAME;
				}
			} // add area
			else
			{
				// add source or condition
				SOCmnPointer<ServerAESource> source;
				SOCMN_ALLOCATE_OBJECT_C(source, ServerAESource, (ServerAESource*))
				source->setName(pData->m_name);
				source->setOTSUserData(pData->m_userData);
				source->setIsBrowsable(pData->m_isBrowsable);

				if (parent->addLeaf(source))
				{
					if (phObject)
					{
						*phObject = (DWORD)source->getOpcHandle();
					}

					return S_OK;
				}
				else
				{
					// duplicate name
					return OT_E_DUPLICATENAME;
				}
			} // add source

#else // OTB_SERVER_AE
			return E_NOTIMPL;
#endif // OTB_SERVER_AE
		} // AE

		return OT_E_INVALIDADDRESSSPACEELEMENTTYPE;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


#ifdef OTB_SERVER_DA
void reconntectItemsToConfigError(SODaSItemTag* itemTag)
{
	SOCmnList<SOCmnObject> itemList;
	SODaSItem* pItem;
	SOCmnListPosition posI;
	itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	itemTag->getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, itemList);

	if (!itemList.isEmpty())
	{
		SOCmnPointer<ServerDATagOutOfConfig> cfgErrTag = new ServerDATagOutOfConfig(itemTag);
		SOCmnPointer<SODaSGroup> group;
		cfgErrTag->setItemID(itemTag->getItemID());
		posI = itemList.getStartPosition();

		while (posI)
		{
			pItem = (SODaSItem*)itemList.getNext(posI);
			group = (SODaSGroup*)pItem->getParentBranch();
			group->removeCyclicRequest(pItem);
			pItem->setItemTag(cfgErrTag, FALSE);
			group->addCyclicRequest(pItem);
		}
	}

	itemList.destroy();

	if (itemTag->is(SOCMNOBJECT_TYPE_PROPERTY))
	{
		return;
	}

	SOCmnListPosition posCIT;
	SODaSItemTag* pChlIT;
	SOCmnList<SODaSItemTag> adList((SOCmnList<SODaSItemTag> *)itemTag->getAdditionalList());
	posCIT = adList.getStartPosition();

	while (posCIT)
	{
		pChlIT = adList.getNext(posCIT);
		pChlIT->addRef();
		reconntectItemsToConfigError(pChlIT);
		pChlIT->release();
	}

	if (itemTag->is(SOCMNOBJECT_TYPE_NODE))
	{
		SOCmnList<SODaSItemTag> branchList((SOCmnList<SODaSItemTag> *)itemTag->getBranchList());
		posCIT = branchList.getStartPosition();

		while (posCIT)
		{
			pChlIT = branchList.getNext(posCIT);
			pChlIT->addRef();
			reconntectItemsToConfigError(pChlIT);
			pChlIT->release();
		}

		SOCmnList<SODaSItemTag> leafList((SOCmnList<SODaSItemTag> *)itemTag->getLeafList());
		posCIT = leafList.getStartPosition();

		while (posCIT)
		{
			pChlIT = leafList.getNext(posCIT);
			pChlIT->addRef();
			reconntectItemsToConfigError(pChlIT);
			pChlIT->release();
		}
	}
}
#endif // OTB_SERVER_DA

LONG OTSAPI_CALL OTSRemoveAddressSpaceElement(IN OTSObjectHandle hObject)
{
	SOCMN_TRY
	{
		DWORD res;
		DWORD objectType;

		res = getObjectType(hObject, &objectType);

		if (FAILED(res))
		{
			return res;
		}

		if ((objectType& SOCMNOBJECT_TYPE_ITEM_TAG) == SOCMNOBJECT_TYPE_ITEM_TAG)
		{
			// DA
#ifdef OTB_SERVER_DA
			SOCmnPointer<SODaSItemTag> itemTag;
			res = getObject(hObject, SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);

			if (FAILED(res))
			{
				return res;
			}

			// remove
			SOCmnPointer<SOCmnElement> parent = itemTag->getParentBranch();

			if (parent.isNotNull())
			{
				if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
				{
					parent->removeLeaf(itemTag);
				}
				else
				{
					parent->removeBranch(itemTag);
				}
			}

			// reconnect the items of the removed elements to config error tags
			reconntectItemsToConfigError(itemTag);
			return S_OK;
#else // OTB_SERVER_DA
			return E_NOTIMPL;
#endif // OTB_SERVER_DA
		}
		else if ((objectType& SOCMNOBJECT_TYPE_SOURCE) == SOCMNOBJECT_TYPE_SOURCE)
		{
			// AE source
#ifdef OTB_SERVER_AE
			SOCmnPointer<ServerAESource> source;
			res = getObject(hObject, SOCMNOBJECT_TYPE_SOURCE, (SOCmnObject**)&source);

			if (FAILED(res))
			{
				return res;
			}

			// remove
			SOCmnPointer<SOCmnElement> parent = source->getParentBranch();

			if (parent.isNotNull())
			{
				parent->removeLeaf(source);
			}

			return S_OK;
#else // OTB_SERVER_AE
			return E_NOTIMPL;
#endif // OTB_SERVER_AE
		}
		else if ((objectType& SOCMNOBJECT_TYPE_AREA) == SOCMNOBJECT_TYPE_AREA)
		{
			// AE area
#ifdef OTB_SERVER_AE
			SOCmnPointer<ServerAEArea> area;
			res = getObject(hObject, SOCMNOBJECT_TYPE_AREA, (SOCmnObject**)&area);

			if (FAILED(res))
			{
				return res;
			}

			// remove
			SOCmnPointer<SOCmnElement> parent = area->getParentBranch();

			if (parent.isNotNull())
			{
				parent->removeBranch(area);
			}

			return S_OK;
#else // OTB_SERVER_AE
			return E_NOTIMPL;
#endif // OTB_SERVER_AE
		}

		return OT_E_INVALIDOBJECTTYPE;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSGetAddressSpaceElementData(IN OTSObjectHandle hObject, IN OUT OTSAddressSpaceElementData* pData)
{
	SOCMN_TRY
	{
		if (!pData)
		{
			return E_INVALIDARG;
		}

		DWORD res;
		DWORD objectType;

		res = getObjectType(hObject, &objectType);

		if (FAILED(res))
		{
			return res;
		}

		SOCmnPointer<SOCmnElement> element;
		res = getObject(hObject, SOCMNOBJECT_TYPE_ELEMENT, (SOCmnObject**)&element);

		if (FAILED(res))
		{
			return res;
		}

		switch (objectType)
		{
#ifdef OTB_SERVER_DA

		case SOCMNOBJECT_TYPE_TAG:
			res = ((ServerDATag*)(SOCmnElement*)element)->getAddressSpaceElementData((SODaSItemTag*)(SOCmnElement*)element, pData);
			break;

		case SOCMNOBJECT_TYPE_NODE:
			res = ((ServerDANode*)(SOCmnElement*)element)->getAddressSpaceElementData((SODaSItemTag*)(SOCmnElement*)element, pData);
			break;
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE

		case SOCMNOBJECT_TYPE_AREA:
			res = ((ServerAEArea*)(SOCmnElement*)element)->getAddressSpaceElementData(element, pData);
			break;

		case SOCMNOBJECT_TYPE_SOURCE:
			res = ((ServerAESource*)(SOCmnElement*)element)->getAddressSpaceElementData(element, pData);
			break;
#endif // OTB_SERVER_AE

		default:
			res = OT_E_INVALIDOBJECTTYPE;
			break;
		}

		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSGetParent(IN OTSObjectHandle hObject, OUT OTSObjectData* pParent)
{
	SOCMN_TRY
	{
		if (!pParent)
		{
			return E_INVALIDARG;
		}

		LONG res;
		SOCmnPointer<SOCmnElement> element;

		res = getObject(hObject, SOCMNOBJECT_TYPE_ELEMENT, (SOCmnObject**)&element);

		if (FAILED(res))
		{
			return res;
		}

		SOCmnPointer<SOCmnElement> parent = element->getParentBranch();

		if (parent.isNotNull())
		{
			if (!parent->is(SOCMNOBJECT_TYPE_ROOT))
			{
				pParent->m_objectHandle = (DWORD)parent->getOpcHandle();
#ifdef OTB_SERVER_DA

				if (parent->is(SOCMNOBJECT_TYPE_NODE))
				{
					pParent->m_userData = ((ServerDANode*)(SOCmnElement*)parent)->getOTSUserData();
				}

#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE

				if (parent->is(SOCMNOBJECT_TYPE_AREA))
				{
					pParent->m_userData = ((ServerAEArea*)(SOCmnElement*)parent)->getOTSUserData();
				}

#endif // OTB_SERVER_AE
			}
			else
			{
				// addressSpace root
				pParent->m_objectHandle = 0;
				pParent->m_userData = 0;
			}
		}
		else
		{ res = E_FAIL; }
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTSAPI_CALL OTSGetChildren(IN OTSObjectHandle hObject, BYTE m_elementType, OUT LONG* pCount, OUT OTSObjectData** ppChildren)
{
	SOCMN_TRY
	{
		if ((!pCount) || (!ppChildren))
		{
			return E_INVALIDARG;
		}

		DWORD res;
		SOCmnPointer<SOCmnElement> parent;

		if (hObject)
		{
			res = getObject(hObject, SOCMNOBJECT_TYPE_ELEMENT, (SOCmnObject**)&parent);

			if (FAILED(res))
			{
				return res;
			}
		}
		else
		{
			if (m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
			{
#ifdef OTB_SERVER_DA
				SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
				parent = entryDA->getNameSpaceRoot();
#else // OTB_SERVER_DA
				return E_NOTIMPL;
#endif // OTB_SERVER_DA
			}
			else if (m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_AE)
			{
#ifdef OTB_SERVER_AE
				SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
				parent = entryAE->getAreaSpaceRoot();
#else // OTB_SERVER_AE
				return E_NOTIMPL;
#endif // OTB_SERVER_AE
			}
			else
			{ return OT_E_INVALIDADDRESSSPACEELEMENTTYPE; }
		}

		SOCmnList<SOCmnElement> branchList(parent->getBranchList());
		SOCmnList<SOCmnElement> leafList(parent->getLeafList());
		SOCmnElement* pElement;
		DWORD objectType;
		LONG count = 0;

		if (branchList.isInit())
		{
			count += branchList.getCount();
		}

		if (leafList.isInit())
		{
			count += leafList.getCount();
		}

		*ppChildren = (OTSObjectData*)OTAllocateMemory(count* sizeof(OTSObjectData));
		*pCount = count;
		SOCmnListPosition pos;
		LONG i = 0;

		if (branchList.isInit())
		{
			pos = branchList.getStartPosition();

			while (pos)
			{
				pElement = branchList.getNext(pos);
				objectType = pElement->getObjectType();

				switch (objectType)
				{
#ifdef OTB_SERVER_DA

				case SOCMNOBJECT_TYPE_NODE:
					(*ppChildren)[i].m_userData = ((ServerDANode*)(SOCmnElement*)pElement)->getOTSUserData();
					break;
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE

				case SOCMNOBJECT_TYPE_AREA:
					(*ppChildren)[i].m_userData = ((ServerAEArea*)(SOCmnElement*)pElement)->getOTSUserData();
					break;
#endif // OTB_SERVER_AE
				}

				(*ppChildren)[i].m_objectHandle = (DWORD)pElement->getOpcHandle();
				i++;
			}
		}

		if (leafList.isInit())
		{
			pos = leafList.getStartPosition();

			while (pos)
			{
				pElement = leafList.getNext(pos);
				objectType = pElement->getObjectType();

				switch (objectType)
				{
#ifdef OTB_SERVER_DA

				case SOCMNOBJECT_TYPE_TAG:
					(*ppChildren)[i].m_userData = ((ServerDATag*)(SOCmnElement*)pElement)->getOTSUserData();
					break;
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE

				case SOCMNOBJECT_TYPE_SOURCE:
					(*ppChildren)[i].m_userData = ((ServerAESource*)(SOCmnElement*)pElement)->getOTSUserData();
					break;
#endif // OTB_SERVER_AE
				}

				(*ppChildren)[i].m_objectHandle = (DWORD)pElement->getOpcHandle();
				i++;
			}
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTSAPI_CALL OTSInitAddressSpace(IN BYTE addressSpaceType)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		pApp->m_addressSpaceType = addressSpaceType;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

#ifdef OTB_SERVER_DA

LONG OTSAPI_CALL OTSCompleteRequests(
	IN LONG count, IN OTSRequestData* paRequests,
	IN LONG* paResult, IN OTSValueData* paValues)
{
	SOCMN_TRY
	{
		if ((!paRequests) || (!paResult))
		{
			return E_INVALIDARG;
		}

		SOCmnPointer<SODaSRequest> request;
		SOCmnPointer<SODaSCache> cache;
		SOCmnVariant value;
		SOCmnDateTime timestamp;
		DWORD res;
		LONG i;

		for (i = 0; i < count; i++)
		{
			res = getObject(paRequests[i].m_requestHandle, SOCMNOBJECT_TYPE_REQUEST, (SOCmnObject**)&request);

			if (FAILED(res))
			{
				return res;
			}

			if (SUCCEEDED(paResult[i]))
			{
				if (request->getOperation() == SODaSRequest::read)
				{
					if (!paValues)
					{
						return E_INVALIDARG;
					}

					cache = request->getCache();
#ifdef SOOS_WINDOWS

					if ((paValues[i].m_timestamp.dwHighDateTime != 0) || (paValues[i].m_timestamp.dwLowDateTime != 0))
#else
					if ((paValues[i].m_timestamp.tv_sec != 0) || (paValues[i].m_timestamp.tv_usec != 0))
#endif
						timestamp.set(&paValues[i].m_timestamp);
					else
					{
						timestamp.now();
					}

					value.initFrom(paValues[i].m_pValue);

					if (cache.isNotNull())
					{
						cache->set(value, paValues[i].m_quality, &timestamp, request);
					}
					else
					{
						_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_SERVER, _T("OTSCompleteRequests"),
								(_T("cache is NULL")));
					}

					value.clear();
				}

				request->complete(paResult[i], FALSE, OPC_QUALITY_UNCERTAIN, FALSE, FALSE);
			}
			else
			{
				request->complete(paResult[i], FALSE, OPC_QUALITY_UNCERTAIN, FALSE, FALSE);
			}

			request->release(); // release reference of OTS application
			request.release();
		}

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSAckRequestElement(
	IN OTSObjectHandle hObject)
{
	SOCmnPointer<SODaSItemTag> itemTag = NULL;
	DWORD res = getObject(hObject, SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);
	if (SUCCEEDED(res))
	{
		itemTag->addRef();
	}
	else
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTSAPI_CALL OTSNackRequestElement(
	IN OTSObjectHandle hObject)
{
	SOCmnPointer<SODaSItemTag> itemTag;
	DWORD res = getObject(hObject, SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);
	if (SUCCEEDED(res))
	{
		itemTag->release();
	}
	else
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTSAPI_CALL OTSGetCacheValue(
	IN OTSObjectHandle hObject,
	OUT OTSValueData* pValue)
{
	SOCMN_TRY
	{
		if (!pValue)
		{
			return E_INVALIDARG;
		}

		if (!pValue->m_pValue)
		{
			return E_INVALIDARG;
		}

		DWORD res;
		SOCmnPointer<SODaSItemTag> itemTag;

		res = getObject(hObject, SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);

		if (FAILED(res))
		{
			return res;
		}

		if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
		{
			if (((ServerDATag*)(SODaSItemTag*)itemTag)->getOwnCache())
			{
				return E_INVALIDARG;
			}
		}
		else
		{
			if (((ServerDANode*)(SODaSItemTag*)itemTag)->getOwnCache())
			{
				return E_INVALIDARG;
			}
		}

		SOCmnPointer<SODaSCache> cache = itemTag->getCache();
		SOCmnVariant value;
		WORD quality;
		SOCmnDateTime timestamp;

		cache->get(value, &quality, &timestamp);
		value.copyTo(pValue->m_pValue);
		pValue->m_quality = quality;
		timestamp.get(&pValue->m_timestamp);

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSSetEUInfo(
	IN OTSObjectHandle hObject,
	IN OTBool isEnumerated,
	IN unsigned long count,
	IN LPTSTR* pEnumeratedValues,
	IN double lowEU,
	IN double highEU)
{
	SOCMN_TRY
	{
		DWORD res;
		SOCmnPointer<SODaSItemTag> itemTag;
		BOOL success = FALSE;

		res = getObject(hObject, SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);

		if (FAILED(res))
		{
			return res;
		}
		if (!isEnumerated)
		{
			if (lowEU > highEU)
			{
				return E_INVALIDARG;
			}

			success = itemTag->setEUAnalog(lowEU, highEU);
		}
		else
		{
			SOCmnStringList enumValuesString;
			enumValuesString.create();

			if (pEnumeratedValues == NULL)
			{
				// return invalid param
				return E_INVALIDARG;
			}

			for (unsigned long i = 0; i < count; i++)
			{
				if (pEnumeratedValues[i] != NULL)
				{
					enumValuesString.add(pEnumeratedValues[i]);
				}
			}

			success = itemTag->setEUEnumerated(&enumValuesString);
		}

		return (success == TRUE) ? S_OK : E_FAIL;

	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSValuesChanged(
	IN LONG count,
	IN OTSObjectHandle* pahObjects,
	IN OTSValueData* paValues)
{
	SOCMN_TRY
	{
		if ((!pahObjects) || (!paValues))
		{
			return E_INVALIDARG;
		}

		LONG res = S_OK;
		LONG i;
		SOCmnPointer<SODaSItemTag> itemTag;
		SOCmnVariant value;
		SOCmnDateTime timestamp;

		for (i = 0; i < count; i++)
		{
			res = getObject(pahObjects[i], SOCMNOBJECT_TYPE_ITEM_TAG, (SOCmnObject**)&itemTag);

			if (FAILED(res))
			{
				return res;
			}

#ifdef SOOS_WINDOWS

			if ((paValues[i].m_timestamp.dwHighDateTime != 0) || (paValues[i].m_timestamp.dwLowDateTime != 0))
#else
			if ((paValues[i].m_timestamp.tv_sec != 0) || (paValues[i].m_timestamp.tv_usec != 0))
#endif
			{
				timestamp.set(&paValues[i].m_timestamp);
			}
			else
			{
				timestamp.now();
			}

			value.initFrom(paValues[i].m_pValue);

			if (value.vt == VT_EMPTY)
			{
				SOCmnPointer<SODaSCache> cache = itemTag->getCache();
				cache->getValue(value);
			}

			if (itemTag->getUpdateMode() == SODaSItemTag::report)
			{
				itemTag->notifyAllConnectedObjects(&value, paValues[i].m_quality, &timestamp);
			}
			else
			{
				SOCmnPointer<SODaSCache> cache = itemTag->getCache();

				if (cache.isNotNull())
				{
					if (!cache->set(value, paValues[i].m_quality, &timestamp))
					{
						res = DISP_E_TYPEMISMATCH;
					}
				}
				else
				{
					//  most likely this item does not support I/O
					res = E_FAIL;
				}
			}

			value.clear();
			itemTag.release();
		}

		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

#else // OTB_SERVER_DA

LONG OTSAPI_CALL OTSCompleteRequests(IN LONG count, IN OTSRequestData* paRequests, IN LONG* paResult, IN OTSValueData* paValues)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSAckRequestElement(IN OTSObjectHandle hObject)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSNackRequestElement(IN OTSObjectHandle hObject)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSGetCacheValue(IN OTSObjectHandle hObject, OUT OTSValueData* pValue)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSValuesChanged(IN LONG count, IN OTSObjectHandle* pahObjects, IN OTSValueData* paValues)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSSetEUInfo(
	IN OTSObjectHandle hObject,
	IN OTBool isEnumerated,
	IN unsigned long count,
	IN LPTSTR* pEnumeratedValues,
	IN double lowEU,
	IN double highEU)
{
	return E_NOTIMPL;
}


#endif // OTB_SERVER_DA

#ifdef OTB_SERVER_AE

LONG OTSAPI_CALL OTSFireEvents(IN DWORD eventCount, IN OTSEventData* pEventData)
{
	SOCMN_TRY
	{
		SOCmnList<SOAeSEvent> eventList;
		SOCmnPointer<SOAeSEvent> event;
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
		SOCmnPointer<SOAeSCategory> cat;
		SOCmnList<SOAeSAttributeValue> attrValList;
		SOCmnList<SOAeSAttribute> attrList;
		SOCmnPointer<SOAeSAttributeValue> attrVal;
		SOAeSAttribute* attr;
		SOCmnListPosition pos;
		BOOL ret;
		DWORD i, j;

		eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);

		for (i = 0; i < eventCount; i++)
		{
			if (pEventData[i].m_eventType == OT_EVENTTYPE_SIMPLE)
			{
				event = SOAeSEvent::createSimpleEvent(pEventData[i].m_eventCategory,
				pEventData[i].m_sourcePath, pEventData[i].m_severity,
				pEventData[i].m_message);
			}
			else if (pEventData[i].m_eventType == OT_EVENTTYPE_TRACKING)
			{
				event = SOAeSEvent::createTrackingEvent(pEventData[i].m_eventCategory,
														pEventData[i].m_sourcePath, pEventData[i].m_severity,
														pEventData[i].m_message, pEventData[i].m_actorID);
			}

			if (!!event)
			{
				cat = event->getCategory();
				attrList = (SOCmnList<SOAeSAttribute> *)cat->getAdditionalList();
				attrValList = event->getAttributeValueList();

				if ((pEventData[i].m_occurenceTime.dwLowDateTime != 0) ||
					(pEventData[i].m_occurenceTime.dwHighDateTime != 0))
				{
					event->setOccurrenceTime(&pEventData[i].m_occurenceTime);
				}

				if (pEventData[i].m_eventAttrCount == attrList.getCount())
				{
					pos = attrList.getStartPosition();
					j = 0;

					while (pos)
					{
						attr = attrList.getNext(pos);
						attrVal = creator->createAttributeValue();
						// initialize attribute value
						attrVal->setAttribute(attr);
						attrVal->setValue(&(pEventData[i].m_pEventAttrs[j]));
						// add attribute value
						attrValList.add(attrVal);
						j++;
					}

					attrValList.autoUnlock();
				}

				eventList.add((SOAeSEvent*)event);
			}
			else
			{
				SetLastError(ERROR_INVALID_DATA);
				ret = FALSE;
			}
		}

		ret = SOAeSEvent::fireEvents(&eventList, 0);

		eventList.destroy();

		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTSAPI_CALL OTSAddEventCategory(IN DWORD categoryID, IN LPTSTR description, IN DWORD eventType, OUT OTSObjectHandle* pCatHandle)
{
	SOCMN_TRY
	{
		SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
		SOCmnPointer<SOAeSEventSpaceRoot> esRoot;
		SOCmnPointer<SOAeSCategory> category;
		BOOL ret;

		esRoot = entryAE->getEventSpaceRoot();

		SOCMN_ALLOCATE_OBJECT_C(category, SOAeSCategory(categoryID, description, eventType), (SOAeSCategory*))
		ret = esRoot->addBranch(category);

		if (ret)
		{
			if (pCatHandle)
			{
				*pCatHandle = (OTObjectHandle)category->getOpcHandle();
			}

			return (ret) ? S_OK : E_FAIL;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

LONG OTSAPI_CALL OTSAddEventAttribute(IN OTSObjectHandle hCategory, IN DWORD attributeID, IN LPTSTR description, IN WORD datatype)
{
	SOCMN_TRY
	{
		SOCmnPointer<SOAeSCategory> category;
		SOCmnPointer<SOAeSAttribute> attribute;
		LONG res;
		BOOL ret;

		res = getObject(hCategory, SOCMNOBJECT_TYPE_CATEGORY, (SOCmnObject**)&category);

		if (FAILED(res))
		{
			return res;
		}

		SOCMN_ALLOCATE_OBJECT_C(attribute, SOAeSAttribute(attributeID, description, datatype), (SOAeSAttribute*))
		ret = category->addAdditionalElement(attribute);
		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSAddCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName)
{
	SOCMN_TRY
	{
		SOCmnPointer<SOAeSCategory> category;
		SOCmnPointer<SOAeSConditionClass> condClass;
		LONG res;
		BOOL ret;

		res = getObject(hCategory, SOCMNOBJECT_TYPE_CATEGORY, (SOCmnObject**)&category);

		if (FAILED(res))
		{
			return res;
		}

		SOCMN_ALLOCATE_OBJECT_C(condClass, ServerAEConditionClass(conditionName), (SOAeSConditionClass*))
		ret = category->addBranch(condClass);
		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTSAPI_CALL OTSAddSubCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName, IN LPTSTR subConditionName)
{
	SOCMN_TRY
	{
		SOCmnPointer<SOAeSCategory> category;
		SOCmnPointer<SOAeSConditionClass> condClass;
		SOCmnPointer<SOAeSSubConditionClass> subCondClass;
		LONG res;
		BOOL ret;

		res = getObject(hCategory, SOCMNOBJECT_TYPE_CATEGORY, (SOCmnObject**)&category);

		if (FAILED(res))
		{
			return res;
		}

		condClass = (SOAeSConditionClass*)category->findBranch(conditionName);

		if (!condClass)
		{
			return E_INVALIDARG;
		}

		SOCMN_ALLOCATE_OBJECT_C(subCondClass, SOAeSSubConditionClass(), (SOAeSSubConditionClass*))
		subCondClass->setName(subConditionName);
		ret = condClass->addLeaf(subCondClass);
		return (ret) ? S_OK : E_FAIL;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}

LONG OTSAPI_CALL OTSConditionsChanged(IN LONG count, IN OTSObjectHandle* pObjects, IN OTSConditionData* pData, OUT OTSObjectHandle* pObjectsOut)
{
	SOCMN_TRY
	{
		if ((!pObjects) || (!pData))
		{
			return E_INVALIDARG;
		}

		ServerApp* pApp = ::getServerApp();
		SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
		SOCmnPointer<ServerAECondition> cond;
		LONG res = S_OK;
		BOOL ret;
		LONG i;
		FILETIME* pOccTime;
		BYTE newState;
		FILETIME now;
		::GetSystemTimeAsFileTime(&now);

		for (i = 0; i < count; i++)
		{
			if (pObjects[i])
			{
				// existing condition object
				res = getObject(pObjects[i], SOCMNOBJECT_TYPE_CONDITION, (SOCmnObject**)&cond);

				if (FAILED(res))
				{
					return res;
				}
			}
			else
			{
				// newly activated condition
				SOCMN_ALLOCATE_OBJECT_C(cond, ServerAECondition(), (ServerAECondition*))

				if (!cond->setConditionName(pData[i].m_eventCategory, pData[i].m_name))
				{
					return OT_E_INVALIDDATA;
				}

				alarmList.add(cond);
				cond->setOTSUserData(pData[i].m_userData);
				cond->setSourceName(pData[i].m_sourcePath);
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_MESSAGE)
			{
				cond->setMessage(pData[i].m_message);
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_SEVERITY)
			{
				cond->setSeverity(pData[i].m_severity);
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_QUALITY)
			{
				cond->setQuality(pData[i].m_quality);
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_SUBCONDITION)
			{
				cond->setSubConditionName(pData[i].m_activeSubConditionName);
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_ATTRIBUTE)
			{
				SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
				SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
				SOCmnPointer<SOAeSCategory> cat;
				SOCmnList<SOAeSAttributeValue> attrValList;
				SOCmnList<SOAeSAttribute> attrList;
				SOCmnPointer<SOAeSAttributeValue> attrVal;
				SOAeSAttribute* attr;
				SOCmnListPosition pos;
				DWORD j;
				cat = cond->getCategory();
				attrList = (SOCmnList<SOAeSAttribute> *)cat->getAdditionalList();
				attrValList = cond->getAttributeValueList();

				if (pData[i].m_eventAttrCount == attrList.getCount())
				{
					attrValList.removeAll();
					pos = attrList.getStartPosition();
					j = 0;

					while (pos)
					{
						attr = attrList.getNext(pos);
						attrVal = creator->createAttributeValue();
						// initialize attribute value
						attrVal->setAttribute(attr);
						attrVal->setValue(&(pData[i].m_pEventAttrs[j]));
						// add attribute value
						attrValList.add(attrVal);
						++j;
					}

					attrValList.autoUnlock();
				}
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_ACK_DATA)
			{
				cond->setAckComment(pData[i].m_ackComment);
				cond->setAckID(pData[i].m_ackID);
				cond->setAckTime(&pData[i].m_ackTime);
			}

			cond->setAckRequired(pData[i].m_ackRequired);

			if ((pData[i].m_occurenceTime.dwHighDateTime == 0) && (pData[i].m_occurenceTime.dwLowDateTime == 0))
			{
				pOccTime = &now;
			}
			else
			{
				pOccTime = &pData[i].m_occurenceTime;
			}

			newState = cond->getObjectState();

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_ACTIVE_STATE)
			{
				newState &= ~OT_CONDITIONSTATE_ACTIVE;
				newState |= pData[i].m_stateChange & OT_CONDITIONSTATE_ACTIVE;

				if (newState & OT_CONDITIONSTATE_ACTIVE)
				{
					cond->setActiveTime(pOccTime);
					cond->setActSubConditionTime(pOccTime);
				}
				else
				{
					cond->setInactiveTime(pOccTime);
					cond->setActSubConditionClass(NULL);
				}
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_ACK_STATE)
			{
				newState &= ~OT_CONDITIONSTATE_ACKED;
				newState |= pData[i].m_stateChange & OT_CONDITIONSTATE_ACKED;
			}

			if (pData[i].m_changeMask & OT_CONDITIONCHANGE_ENABLE_STATE)
			{
				newState &= ~OT_CONDITIONSTATE_ENABLED;
				newState |= pData[i].m_stateChange & OT_CONDITIONSTATE_ENABLED;
			}

			ret = cond->changeState(newState, pOccTime, pData[i].m_changeMask & (~OT_CONDITIONCHANGE_STATE & 0x00FF));

			if (!ret)
			{
				res = S_FALSE;
			}

			if (cond.isNotNull())
			{
				pObjectsOut[i] = (OTObjectHandle)cond->getOpcHandle();
			}
			else   // removed condition
			{
				pObjectsOut[i] = 0;
			}
		}

		return res;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


LONG OTSAPI_CALL OTSGetConditionData(IN OTSObjectHandle hObject, OUT OTSConditionData* pData)
{
	SOCMN_TRY
	{
		if (!pData)
		{
			return E_INVALIDARG;
		}

		SOCmnPointer<ServerAECondition> cond;
		LONG res;

		res = getObject(hObject, SOCMNOBJECT_TYPE_CONDITION, (SOCmnObject**)&cond);

		if (FAILED(res))
		{
			return res;
		}

		SOCmnPointer<SOAeSCategory> cat = cond->getCategory();

		SetOutputStructMemberString(cond->getAckComment(), &pData->m_ackComment);
		SetOutputStructMemberString(cond->getAckID(), &pData->m_ackID);
		pData->m_ackRequired = (BYTE)cond->getAckRequired(0xFFFF);
		cond->getAckTime(&pData->m_ackTime);
		SetOutputStructMemberString(cond->getSubConditionName(), &pData->m_activeSubConditionName);
		pData->m_changeMask = 0;
		pData->m_eventCategory = cat->getNameInt();
		SetOutputStructMemberString(cond->getMessage(), &pData->m_message);
		SetOutputStructMemberString(cond->getName(), &pData->m_name);
		pData->m_occurenceTime.dwHighDateTime = 0;
		pData->m_occurenceTime.dwLowDateTime = 0;
		pData->m_quality = cond->getQuality();
		pData->m_severity = cond->getSeverity();
		SetOutputStructMemberString(cond->getSourceName(), &pData->m_sourcePath);
		pData->m_stateChange = cond->getObjectState();
		pData->m_userData = cond->getOTSUserData();

		SOCmnList<SOAeSAttributeValue> attrValList;
		SOAeSAttributeValue* pAttrVal;
		SOCmnListPosition pos;
		DWORD k = 0;

		attrValList = cond->getAttributeValueList();
		pData->m_eventAttrCount = attrValList.getCount();
		pData->m_pEventAttrs = (OTVariant*)OTAllocateMemory(sizeof(OTVariant) * pData->m_eventAttrCount);
		pos = attrValList.getStartPosition();

		while (pos)
		{
			pAttrVal = attrValList.getNext(pos);
			pAttrVal->getValue(&pData->m_pEventAttrs[k]);
			++k;
		}
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
	return S_OK;
}

#else // OTB_SERVER_AE

LONG OTSAPI_CALL OTSAddEventCategory(IN DWORD categoryID, IN LPTSTR description, IN DWORD eventType, OUT OTSObjectHandle* pCatHandle)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddEventAttribute(IN OTSObjectHandle hCategory, IN DWORD attributeID, IN LPTSTR description, IN WORD datatype)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSFireEvents(IN DWORD eventCount, IN OTSEventData* pEventData)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddSubCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName, IN LPTSTR subConditionName)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSConditionsChanged(IN LONG count, IN OTSObjectHandle* pObjects, IN OTSConditionData* pData, OUT OTSObjectHandle* pObjectsOut)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetConditionData(IN OTSObjectHandle hObject, OUT OTSConditionData* pData)
{
	return E_NOTIMPL;
}

#endif // OTB_SERVER_AE

#else // OTB_SERVER

LONG OTSAPI_CALL OTSInitialize(IN OTSInitData* pInitData)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSTerminate(void)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSProcessCommandLine(IN LPTSTR commandLine)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSSetServerState(IN DWORD daServerState, IN DWORD aeServerState)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSStart(void)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSReady(void)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSStop(OTChar* reason)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAdvise(IN OTSCallbackFunctions* pCallbacks)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddAddressSpaceElement(IN OTSObjectHandle hParent, IN OTSAddressSpaceElementData* pData, OUT OTSObjectHandle* phObject)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSRemoveAddressSpaceElement(IN OTSObjectHandle hObject)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSCompleteRequests(IN LONG count, IN OTSRequestData* paRequests, IN LONG* paResult, IN OTSValueData* paValues)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAckRequestElement(IN OTSObjectHandle hObject)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSNackRequestElement(IN OTSObjectHandle hObject)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetCacheValue(IN OTSObjectHandle hObject, OUT OTSValueData* pValue)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSValuesChanged(IN LONG count, IN OTSObjectHandle* pahObjects, IN OTSValueData* paValues)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetAddressSpaceElementData(IN OTSObjectHandle hObject, OUT OTSAddressSpaceElementData* pData)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetParent(IN OTSObjectHandle hObject, OUT OTSObjectData* pParent)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetChildren(IN OTSObjectHandle hObject, IN BYTE m_elementType, OUT LONG* pCount, OUT OTSObjectData** ppChildren)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSInitAddressSpace(IN BYTE addressSpaceType)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddEventCategory(IN DWORD categoryID, IN LPTSTR description, IN DWORD eventType, OUT OTSObjectHandle* pCatHandle)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddEventAttribute(IN OTSObjectHandle hCategory, IN DWORD attributeID, IN LPTSTR description, IN WORD datatype)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSFireEvents(IN DWORD eventCount, IN OTSEventData* pEventData)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSAddSubCondition(IN OTSObjectHandle hCategory, IN LPTSTR conditionName, IN LPTSTR subConditionName)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSConditionsChanged(IN LONG count, IN OTSObjectHandle* pObjects, IN OTSConditionData* pData, OUT OTSObjectHandle* pObjectsOut)
{
	return E_NOTIMPL;
}
LONG OTSAPI_CALL OTSGetConditionData(IN OTSObjectHandle hObject, OUT OTSConditionData* pData)
{
	return E_NOTIMPL;
}

LONG OTSAPI_CALL OTSSetEUInfo(
	IN OTSObjectHandle hObject,
	IN OTBool isEnumerated,
	IN unsigned long count,
	IN LPTSTR* pEnumeratedValues,
	IN double lowEU,
	IN double highEU)
{
	return E_NOTIMPL;
}

#endif // OTB_SERVER

