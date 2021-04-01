#include "stdafx.h"

#ifdef OTB_SERVER_AE

#include "opcae_er.h"
#include "ServerAEServer.h"
#include "ServerAEArea.h"
#include "ServerAESource.h"
#include "ServerAECondition.h"
#include "ServerAEConditionClass.h"
#include "SrvApp.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerAEServer
//-----------------------------------------------------------------------------

ServerAEServer::ServerAEServer(
	IN OPTIONAL BYTE /* serverInstance */)
{
}

ServerAEServer::~ServerAEServer()
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		OTSSessionData clientStateData;
		clientStateData.m_state = OTS_SESSIONSTATE_DESTROY;
		clientStateData.m_type = OTS_SESSIONTYPE_AE;
		clientStateData.m_password = NULL;
		clientStateData.m_userName = NULL;
		clientStateData.m_clientName = NULL;
		clientStateData.m_clientIp = NULL;
		clientStateData.m_clientRemote = 0;

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::~ServerAEServer - Exception occurred in vendor application callback: OTSChangeSessionState")));
		}
	}
}

HRESULT ServerAEServer::setFlags(BYTE flags)
{
	SOAeSServer::setFlags(flags);
	HRESULT result = S_OK;
	ServerApp* pApp = ::getServerApp();

	SOCMN_TRY
	{
		if ((flags == SOAESSERVER_FLAG_DCOM) ||
		(flags == SOAESSERVER_FLAG_TUNNEL))
		{
			if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
			{
				OTSSessionData clientStateData;
				clientStateData.m_state = OTS_SESSIONSTATE_CREATE;
				clientStateData.m_type = OTS_SESSIONTYPE_AE;
				clientStateData.m_password = NULL;
				clientStateData.m_userName = NULL;
				clientStateData.m_clientName = (LPTSTR)m_clientName;
				clientStateData.m_clientIp = (LPTSTR)m_clientIp;
				clientStateData.m_clientRemote = m_clientRemote;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::setFlags - Exception occurred in vendor application callback: OTSChangeSessionState")));
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::setFlags - Exception occurred")));	}

	return result;
}   //  end setFlags

BOOL ServerAEServer::isAreaName(IN LPCTSTR areaName)
{
	BOOL ret = FALSE;
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		bool handledEx = false;
		SOCmnString key(areaName);
		OTSAddressSpaceElementData *pExistingElementData = NULL;
		OTSAddressSpaceElementData *pElementData = NULL;
		unsigned short elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)areaName, OT_ADDRESSSPACEELEMENT_TYPE_AE,
						elementDataFilter, pElementData);
				}
					SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::isAreaName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
				}

				if (S_OK == result)
				{
					if ((elementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
					{
						ret = pElementData->m_hasChildren;
						entry->setStringBasedObjectDataEx(key, pElementData, elementDataFilter);
						handledEx = true;
					}
					else if (elementDataFilter != OTS_ELEMENTDATA_NONE)
					{
						// something was provided, cache it
						entry->setStringBasedObjectDataEx(key, pElementData, elementDataFilter);
					}
				}
			}
			else
			{
				ret = pExistingElementData->m_hasChildren;
				handledEx = true;
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)areaName, OT_ADDRESSSPACEELEMENT_TYPE_AE, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::isAreaName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (S_OK == result)
				{
					ret = pElementData->m_hasChildren;
					entry->setStringBasedObjectDataEx(key, pElementData);
				}
			}
			else
			{
				ret = pExistingElementData->m_hasChildren;
			}
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
	}
	return ret;
}

BOOL ServerAEServer::isSourceName(IN LPCTSTR sourceName)
{
	BOOL ret = FALSE;
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		bool handledEx = false;
		SOCmnString key(sourceName);
		OTSAddressSpaceElementData* pElementData = NULL;
		unsigned short elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			if (!entry->getStringBasedObjectDataEx(key, &pElementData, elementDataFilter))
			{
				pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)sourceName, OT_ADDRESSSPACEELEMENT_TYPE_AE,
						elementDataFilter, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::isSourceName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
				}

				if (S_OK == result)
				{
					if ((elementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
					{
						ret = !pElementData->m_hasChildren;
						entry->setStringBasedObjectDataEx(key, pElementData, elementDataFilter);
						handledEx = true;
					}
					else if (elementDataFilter != OTS_ELEMENTDATA_NONE)
					{
						entry->setStringBasedObjectDataEx(key, pElementData, elementDataFilter);
					}
				}
			}
			else
			{
				ret = !pElementData->m_hasChildren;
				handledEx = true;
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

			if (!entry->getStringBasedObjectDataEx(key, &pElementData, elementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)sourceName, OT_ADDRESSSPACEELEMENT_TYPE_AE, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::isSourceName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (S_OK == result)
				{
					ret = !pElementData->m_hasChildren;
					entry->setStringBasedObjectDataEx(key, pElementData);
				}

				FreeOTSAddressSpaceElementData(pElementData);
			}
			else
			{
				ret = !pElementData->m_hasChildren;
			}
		}

		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::isSourceName - Exception occurred")));
	}

	return ret;
}

void ServerAEServer::getChildNamesOfAreaName(IN LPCTSTR areaName, BOOL areaOrSourceChilds, LPCTSTR filterCriteria, OUT SOCmnStringList& nameList)
{
	ServerApp* pApp = ::getServerApp();
	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren)
		{
			DWORD countAE;
			OTSAddressSpaceElementData* pDataAE;
			DWORD i;

			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren((LPTSTR)areaName, OT_ADDRESSSPACEELEMENT_TYPE_AE, &countAE, &pDataAE);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::getChildNamesOfAreaName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementChildren")));
			}

			for (i = 0; i < countAE; i++)
			{
				if (pDataAE[i].m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_AE)
				{
					if ((areaOrSourceChilds) && (!pDataAE[i].m_hasChildren))
					{
						continue;
					}

					if ((!areaOrSourceChilds) && (pDataAE[i].m_hasChildren))
					{
						continue;
					}

					if ((filterCriteria) && (_tcslen(filterCriteria) != 0))
					{
						if (!SOSrvServer::opcMatchStringFilter(pDataAE[i].m_name, filterCriteria, TRUE))
						{
							continue;
						}
					}

					nameList.add(pDataAE[i].m_name);
				}
			}

			for (i = 0; i < countAE; i++)
			{
				FreeOTSAddressSpaceElementData(&pDataAE[i]);
			}

			if (pDataAE)
			{
				OTFreeMemory(pDataAE);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::getChildNamesOfAreaName - Exception occurred")));
	}
}

void ServerAEServer::getConditionNamesOfSourceName(IN LPCTSTR sourceName, OUT SOCmnStringList& condNameList)
{
	ServerApp* pApp = ::getServerApp();
	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryConditions)
		{
			SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
			SOCmnPointer<SOAeSAreaSpaceRoot> asRoot = entry->getAreaSpaceRoot();
			SOCmnPointer<SOAeSSource> source;
			OTSObjectData object;
			DWORD condCount;
			LPTSTR* pCondNames;
			DWORD i;
			source = (SOAeSSource*)asRoot->findLeaf(sourceName, TRUE);

			if (source.isNotNull())
			{
				object.m_objectHandle = (DWORD)source->getOpcHandle();
				object.m_userData = ((ServerAESource*)(SOAeSSource*)source)->getOTSUserData();

				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryConditions(&object, (LPTSTR)sourceName, &condCount, &pCondNames);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::getConditionNamesOfSourceName - Exception occurred in vendor application callback: OTSQueryConditions")));
				}
			}
			else
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryConditions(NULL, (LPTSTR)sourceName, &condCount, &pCondNames);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::getConditionNamesOfSourceName - Exception occurred in vendor application callback: OTSQueryConditions")));
				}
			}

			for (i = 0; i < condCount; i++)
			{
				condNameList.add(pCondNames[i]);
				OTFreeMemory(pCondNames[i]);
			}

			if (pCondNames)
			{
				OTFreeMemory(pCondNames);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::getConditionNamesOfSourceName - Exception occurred")));
	}
}

BOOL ServerAEServer::browseOnlyStringBased(void)
{
	ServerApp* pApp = ::getServerApp();
	return (pApp->m_addressSpaceType == OTS_ADDRESSSPACETYPE_STRING);
}

void ServerAEServer::refreshConditions(
	IN SOAeSRefresh* refresh)   // refresh object
{
	ServerApp* pApp = ::getServerApp();
	SOCmnList<SOAeSEvent> eventList(refresh->getEventList());
	SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
	SOAeSCondition* cond;
	SOCmnListPosition posC;
	SOCmnPointer<SOAeSEvent> event;
	WORD condState;
	posC = alarmList.getStartPosition();

	while (posC)
	{
		cond = alarmList.getNext(posC);
		condState = cond->getObjectState();

		if ((condState & OPC_CONDITION_ACTIVE) ||
			((!(condState & OPC_CONDITION_ACTIVE)) && (!(condState & OPC_CONDITION_ACKED))))
		{
			// condition active or (inactive and unacknowledged)
			event = cond->createEvent(0);
			eventList.add((SOAeSEvent*)event);
		}
	}

	refresh->setObjectState(SOAeSRefresh::aggregated | SOAeSRefresh::collected);
} // refreshConditions


HRESULT ServerAEServer::createConditionByName(
	IN LPCTSTR sourceName,          // source name
	IN LPCTSTR conditionName,       // condition name
	OUT SOAeSCondition** condition) // condition object
{
	ServerApp* pApp = ::getServerApp();
	SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
	SOCmnListPosition pos;
	ServerAECondition* cond;
	pos = alarmList.getStartPosition();

	while (pos)
	{
		cond = (ServerAECondition*)alarmList.getNext(pos);

		if (cond->getName() == conditionName)
		{
			if (cond->getSourceName() == sourceName)
			{
				if (condition)
				{
					if (pApp->m_callbackFunctions.m_OTSQueryConditionDefinition)
					{
						SOCmnPointer<ServerAEConditionClass> condClass;
						condClass = (ServerAEConditionClass*)cond->getConditionClass();

						if (condClass.isNotNull())
						{
							if (!condClass->isInit())
							{
								condClass->init();
								OTSObjectData conditionData;
								OTSConditionDefinitionData conditionDefData;
								conditionData.m_objectHandle = (OTObjectHandle)cond->getOpcHandle();
								conditionData.m_userData = cond->getOTSUserData();
								HRESULT result = E_FAIL;

								SOCMN_TRY
								{
									result = pApp->m_callbackFunctions.m_OTSQueryConditionDefinition(conditionData, &conditionDefData);
								}
								SOCMN_CATCH(...)
								{
									_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::createConditionByName - Exception occurred in vendor application callback: OTSQueryConditionDefinition")));
								}

								if (SUCCEEDED(result))
								{
									SOCmnList<SOAeSSubConditionClass> subCondList((SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList());
									SOAeSSubConditionClass* pSubCond;
									DWORD k;
									condClass->setDefinition(conditionDefData.m_definition);
									OTFreeMemory(conditionDefData.m_definition);

									if (subCondList.getCount() == conditionDefData.m_subConditionCount)
									{
										pos = subCondList.getStartPosition();

										for (k = 0; ((k < conditionDefData.m_subConditionCount) && (pos)); k++)
										{
											pSubCond = subCondList.getNext(pos);
											pSubCond->setSeverity(conditionDefData.m_subConditionSeverities[k]);

											if (conditionDefData.m_subConditionDescriptions)
											{
												pSubCond->setDescription(conditionDefData.m_subConditionDescriptions[k]);
												OTFreeMemory(conditionDefData.m_subConditionDescriptions[k]);
											}

											if (conditionDefData.m_subConditionDefinitions)
											{
												pSubCond->setDefinition(conditionDefData.m_subConditionDefinitions[k]);
												OTFreeMemory(conditionDefData.m_subConditionDefinitions[k]);
											}
										}

										if (conditionDefData.m_subConditionDefinitions)
										{
											OTFreeMemory(conditionDefData.m_subConditionDefinitions);
										}

										if (conditionDefData.m_subConditionDescriptions)
										{
											OTFreeMemory(conditionDefData.m_subConditionDescriptions);
										}

										OTFreeMemory(conditionDefData.m_subConditionSeverities);
									}
								}
							}
						}
					}

					cond->addRef();
					*condition = cond;
				}

				return S_OK;
			}
		}
	}

	// check if source is valid
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaSpace = entry->getAreaSpaceRoot();
	SOCmnPointer<SOAeSSource> source;
	source = (SOAeSSource*)areaSpace->findLeaf(sourceName, TRUE);

	if (!source)
	{
		// check source name if no object exists
		if (!isSourceName(sourceName))
		{
			return E_INVALIDARG;
		}
	}

	SOCmnStringList condNameList;
	condNameList.create();
	getConditionNamesOfSourceName(sourceName, condNameList);

	if (condNameList.findObject(conditionName) != NULL)
	{
		return OPC_E_NOINFO;
	}

	return E_INVALIDARG;
} // createConditionByName

HRESULT ServerAEServer::ackConditionByName(
	IN LPCTSTR sourceName,      // source name
	IN LPCTSTR conditionName,   // condition name
	IN LPCTSTR ackID,                   // acknowledger ID
	IN LPCTSTR ackComment,      // acknowledger comment
	LPFILETIME activeTime,      // active time
	DWORD cookie)                           // notification cookie
{
	ServerApp* pApp = ::getServerApp();

	if (!pApp->m_callbackFunctions.m_OTSAcknowledgeCondition)
	{
		return E_INVALIDARG;
	}

	SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
	SOCmnPointer<SOAeSCondition> cond;
	cond = alarmList.findKey(cookie);

	if (cond.isNotNull())
	{
		if (cond->getName() != conditionName)
		{
			return E_INVALIDARG;
		}

		if (cond->getSourceName() != sourceName)
		{
			return E_INVALIDARG;
		}

		// check active time
		FILETIME subActTime;
		cond->getActSubConditionTime(&subActTime);

		if ((activeTime->dwHighDateTime != subActTime.dwHighDateTime) ||
			(activeTime->dwLowDateTime != subActTime.dwLowDateTime))
		{
			return OPC_E_INVALIDTIME;
		}

		if (!(cond->getObjectState() & OPC_CONDITION_ACKED))
		{
			HRESULT res;
			res = cond->acknowledge(ackID, ackComment, activeTime, cookie);

			if (SUCCEEDED(res))
			{
				return S_OK;
			}
			else
			{
				return E_INVALIDARG;
			}
		}
		else
		{
			return OPC_S_ALREADYACKED;    // condition is already acknowledged
		}
	}

	return E_INVALIDARG;
} // ackConditionByName

BOOL ServerAEServer::supportTranslateToItemIDs(void)
{
	return FALSE;
}

BOOL ServerAEServer::supportEnableCondition(
	IN BOOL /* enable */,
	IN BOOL /* areaOrSource */)
{
	ServerApp* pApp = ::getServerApp();
	return pApp->m_supportDisableConditions;
}

BOOL ServerAEServer::enableConditionBySourceName(
	IN BOOL enable,                 // en/disable
	IN LPCTSTR sourceName)  // source name
{
	ServerApp* pApp = ::getServerApp();
	SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
	SOAeSCondition* cond;
	SOCmnListPosition pos;
	pos = alarmList.getStartPosition();

	while (pos)
	{
		cond = alarmList.getNext(pos);

		if (cond->getSourceName() == sourceName)
		{
			cond->enable(enable);
		}
	}

	if (pApp->m_callbackFunctions.m_OTSEnableConditions)
	{
		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSEnableConditions((BYTE)enable, (LPTSTR)sourceName);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::enableConditionBySourceName - Exception occurred in vendor application callback: OTSEnableConditions")));
		}
	}

	return TRUE;
} // enableConditionBySourceName

BOOL ServerAEServer::enableConditionByAreaName(
	IN BOOL enable,                 // en/disable
	IN LPCTSTR areaName)    // Area name
{
	ServerApp* pApp = ::getServerApp();
	SOCmnList<SOAeSCondition> alarmList(pApp->getAlarmList());
	SOAeSCondition* cond;
	SOCmnListPosition pos;
	DWORD lenAreaName = (DWORD)_tcslen(areaName);
	pos = alarmList.getStartPosition();

	while (pos)
	{
		cond = alarmList.getNext(pos);

		if (_tcsncmp(cond->getSourceName(), areaName, lenAreaName) == 0)
		{
			cond->enable(enable);
		}
	}

	if (pApp->m_callbackFunctions.m_OTSEnableConditions)
	{
		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSEnableConditions((BYTE)enable, (LPTSTR)areaName);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::enableConditionByAreaName - Exception occurred in vendor application callback: OTSEnableConditions")));
		}
	}

	return TRUE;
} // enableConditionByAreaName

// overrides the onSetClientName
void ServerAEServer::onSetClientName(IN LPCTSTR clientName)
{
	ServerApp* pApp = ::getServerApp();
	OTSSessionData clientStateData;
	memset(&clientStateData, 0, sizeof(OTSSessionData));

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		clientStateData.m_clientName = (LPTSTR)clientName;
		clientStateData.m_state = OTS_SESSIONSTATE_MODIFY;
		clientStateData.m_type = OTS_SESSIONTYPE_AE;

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::onSetClientName - Exception occurred in vendor application callback: OTSChangeSessionState")));
		}
	}
}

BOOL ServerAEServer::isPrivateSecurityAvailable(void)
{
	return TRUE; // allways provide the security interface
}

BOOL ServerAEServer::logon(LPCTSTR userName, LPCTSTR password)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		OTSSessionData clientStateData;
		clientStateData.m_state = OTS_SESSIONSTATE_LOGON;
		clientStateData.m_type = OTS_SESSIONTYPE_AE;
		clientStateData.m_userName = (LPTSTR)userName;
		clientStateData.m_password = (LPTSTR)password;
		clientStateData.m_clientName = NULL;
		clientStateData.m_clientIp = NULL;
		clientStateData.m_clientRemote = 0;

		SOCMN_TRY
		{
			ret = (pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData) == S_OK);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::logon - Exception occurred in vendor application callback: OTSChangeSessionState")));
		}
	}

	return ret;
}

BOOL ServerAEServer::logoff(void)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		OTSSessionData clientStateData;
		clientStateData.m_state = OTS_SESSIONSTATE_LOGOFF;
		clientStateData.m_type = OTS_SESSIONTYPE_AE;
		clientStateData.m_password = NULL;
		clientStateData.m_userName = NULL;
		clientStateData.m_clientName = NULL;
		clientStateData.m_clientIp = NULL;
		clientStateData.m_clientRemote = 0;

		SOCMN_TRY
		{
			ret = (pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData) == S_OK);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAEServer::logoff - Exception occurred in vendor application callback: OTSChangeSessionState")));
		}
	}

	return ret;
}

HRESULT ServerAEServer::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
}

//-----------------------------------------------------------------------------
// waitForAreaSpaceStartup
//
// - wait for the area space creation to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT ServerAEServer::waitForAreaSpaceStartup(void)
{
	ServerApp *pServerApp = getServerApp();
	if (pServerApp->m_startupEvent)
	{
		pServerApp->m_startupEvent->waitFor(INFINITE);
	}
	return S_OK;
} // waitForAreaSpaceStartup

HRESULT ServerAEServer::waitForEventSpaceStartup(void)
{
	ServerApp *pServerApp = getServerApp();
	if (pServerApp->m_startupEvent)
	{
		pServerApp->m_startupEvent->waitFor(INFINITE);
	}
	return S_OK;
} // waitForEventSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT ServerAEServer::waitForIOStartup(void)
{
	ServerApp *pServerApp = getServerApp();
	if (pServerApp->m_startupEvent)
	{
		pServerApp->m_startupEvent->waitFor(INFINITE);
	}
	return S_OK;
} // waitForIOStartup


#endif // OTB_SERVER_AE


