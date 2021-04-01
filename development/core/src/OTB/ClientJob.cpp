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
//                           OPC Toolkit - OTC                                |
//                                                                            |
//  Filename    : ClientJob.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description :  Client Toolkit tasks                                       |
//																			  |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT

#include "ClientJob.h"
#ifdef OTB_CLIENT_DA
#include "ClientDASession.h"
#include "ClientDASubscription.h"
#include "ClientDAItem.h"
#include "SOCmnHandleManager.h"
#endif // OTB_CLIENT_DA

//-----------------------------------------------------------------------------
// ClientJob                                                                        |
//-----------------------------------------------------------------------------

ClientJob::ClientJob(
	IN SOCltElement* pElement,
	IN OTCExecutionOptions* pExecutionOptions)
	: SOCltTask(pElement)
{
	m_jobType = CLIENTJOB_TYPE_UNKNOWN;
	m_executionOptions.m_executionType = OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE;
	m_executionOptions.m_executionContext = pExecutionOptions->m_executionContext;
	m_deep = FALSE;
	m_fromServer = TRUE;
	m_attributeCount = 0;
	m_pWhatAttributes = NULL;
#ifdef OTB_CLIENT_DA
	m_itemCount = 0;
	m_ppItems = NULL;
	m_pItemIDs = NULL;
	m_pItemPaths = NULL;
	m_maxAge = 0;
	m_pValues = NULL;
	m_pGetPropertiesOptions = NULL;
#endif // OTB_CLIENT_DA
	m_whatOPCSpec = 0;
	m_whatServerData = 0;
	m_addressSpaceElementHandle = 0;
	m_pBrowseOptions = NULL;
}

ClientJob::~ClientJob(void)
{
	if (m_pWhatAttributes)
	{
		OTFreeMemory(m_pWhatAttributes);
	}

#ifdef OTB_CLIENT_DA
	DWORD i;

	if (m_ppItems)
	{
		for (i = 0; i < m_itemCount; i++)
		{
			if (m_ppItems[i])
			{
				m_ppItems[i]->release();
			}
		}

		OTFreeMemory(m_ppItems);
	}

	if (m_pItemIDs)
	{
		for (i = 0; i < m_itemCount; i++)
		{
			OTFreeMemory(m_pItemIDs[i]);
		}

		OTFreeMemory(m_pItemIDs);
	}

	if (m_pItemPaths)
	{
		for (i = 0; i < m_itemCount; i++)
		{
			OTFreeMemory(m_pItemPaths[i]);
		}

		OTFreeMemory(m_pItemPaths);
	}

	if (m_pValues)
	{
		for (i = 0; i < m_itemCount; i++)
		{
			::VariantClear(m_pValues[i].m_pValue);
			OTFreeMemory(m_pValues[i].m_pValue);
		}

		OTFreeMemory(m_pValues);
	}

	if (m_pGetPropertiesOptions)
	{
		if (m_pGetPropertiesOptions->m_pPropertyIDs)
		{
			OTFreeMemory(m_pGetPropertiesOptions->m_pPropertyIDs);
		}

		if (m_pGetPropertiesOptions->m_pPropertyNames)
		{
			for (i = 0; i < m_pGetPropertiesOptions->m_propertyCount; i++)
			{
				if (m_pGetPropertiesOptions->m_pPropertyNames[i] != NULL)
				{
					OTFreeMemory((void*)m_pGetPropertiesOptions->m_pPropertyNames[i]);
				}
			}

			OTFreeMemory((void*)m_pGetPropertiesOptions->m_pPropertyNames);
		}

		OTFreeMemory(m_pGetPropertiesOptions);
	}

#endif // OTB_CLIENT_DA

	if (m_pBrowseOptions)
	{
		if (m_pBrowseOptions->m_elementNameFilter)
		{
			OTFreeMemory((void*)m_pBrowseOptions->m_elementNameFilter);
		}

		if (m_pBrowseOptions->m_vendorFilter)
		{
			OTFreeMemory((void*)m_pBrowseOptions->m_vendorFilter);
		}

		OTFreeMemory(m_pBrowseOptions);
	}
}

void ClientJob::execute()
{
	SOCMN_TRY
	{
		switch (m_jobType)
		{
		case CLIENTJOB_TYPE_PERFORMSTATETRANSITION:
		{
			m_res = OTCPerformStateTransition((OTCObjectHandle)m_element->getOpcHandle(), m_deep, &m_executionOptions);
		}
		break;
#ifdef OTB_CLIENT_DA

		case CLIENTJOB_TYPE_READ:
		{
			if (m_itemCount > 0)
			{
				DWORD i;
				LONG* pResults;
				OTCValueData* pValues;
				pResults = (LONG*)OTAllocateMemory(m_itemCount * sizeof(LONG));
				pValues = (OTCValueData*)OTAllocateMemory(m_itemCount * sizeof(OTCValueData));

				for (i = 0; i < m_itemCount; i++)
				{
					pValues[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
					::VariantInit(pValues[i].m_pValue);
				}

				OTCObjectHandle *p_SODaCItem_Handles = NULL;
				if (m_ppItems != NULL)
				{
					p_SODaCItem_Handles = (OTCObjectHandle*)OTAllocateMemory(m_itemCount * sizeof(OTCObjectHandle));
					for (i = 0; i < m_itemCount; ++i)
					{
						if (m_ppItems[i])
						{
							p_SODaCItem_Handles[i] = m_ppItems[i]->getOpcHandle();
						}
					}
				}
				m_res = OTCRead((OTCObjectHandle)m_element->getOpcHandle(), m_itemCount, (OTCObjectHandle*)p_SODaCItem_Handles, m_pItemIDs, m_pItemPaths, m_maxAge, pValues, pResults, &m_executionOptions);
				OTFreeMemory(pResults);
				if (p_SODaCItem_Handles != NULL)
				{
					OTFreeMemory(p_SODaCItem_Handles);
				}

				for (i = 0; i < m_itemCount; i++)
				{
					::VariantClear(pValues[i].m_pValue);
					OTFreeMemory(pValues[i].m_pValue);
				}

				OTFreeMemory(pValues);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;

		case CLIENTJOB_TYPE_WRITE:
		{
			if (m_itemCount > 0)
			{
				LONG* pResults;
				pResults = (LONG*)OTAllocateMemory(m_itemCount * sizeof(LONG));
				OTCObjectHandle *p_SODaCItem_Handles = NULL;
				if (m_ppItems != NULL)
				{
					p_SODaCItem_Handles = (OTCObjectHandle*)OTAllocateMemory(m_itemCount * sizeof(OTCObjectHandle));
					for (DWORD i = 0; i < m_itemCount; ++i)
					{
						if (m_ppItems[i])
						{
							p_SODaCItem_Handles[i] = m_ppItems[i]->getOpcHandle();
						}
					}
				}
				m_res = OTCWrite((OTCObjectHandle)m_element->getOpcHandle(), m_itemCount, (OTCObjectHandle*)p_SODaCItem_Handles, (LPTSTR*)m_pItemIDs, (LPTSTR*)m_pItemPaths, m_pValues, pResults, &m_executionOptions);
				if (p_SODaCItem_Handles != NULL)
				{
					OTFreeMemory(p_SODaCItem_Handles);
				}
				OTFreeMemory(pResults);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;
#endif // OTB_CLIENT_DA

		case CLIENTJOB_TYPE_UPDATEATTRIBUTES:
		{
			if (m_attributeCount > 0)
			{
				LONG* pResults;
				pResults = (LONG*)OTAllocateMemory(m_attributeCount * sizeof(LONG));
				m_res = OTCUpdateAttributes((OTCObjectHandle)m_element->getOpcHandle(), m_fromServer, m_attributeCount, m_pWhatAttributes, pResults, &m_executionOptions);
				OTFreeMemory(pResults);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;

		case CLIENTJOB_TYPE_BROWSEADDRESSSPACE:
		{
			DWORD addressSpaceElementDataCount;
			OTCAddressSpaceElementData* pAddressSpaceElement;
			DWORD i;
			m_res = OTCBrowseAddressSpace((OTCObjectHandle)m_element->getOpcHandle(), m_addressSpaceElementHandle, (LPTSTR)(LPCTSTR)m_addressSpaceElementID, (LPTSTR)(LPCTSTR)m_addressSpaceElementPath, m_pBrowseOptions, &addressSpaceElementDataCount, &pAddressSpaceElement, &m_executionOptions);

			if ((SUCCEEDED(m_res)) && (pAddressSpaceElement != NULL))
			{
				for (i = 0; i < addressSpaceElementDataCount; i++)
				{
					if (pAddressSpaceElement[i].m_itemID)
					{
						OTFreeMemory(pAddressSpaceElement[i].m_itemID);
					}

					if (pAddressSpaceElement[i].m_itemPath)
					{
						OTFreeMemory(pAddressSpaceElement[i].m_itemPath);
					}

					if (pAddressSpaceElement[i].m_name)
					{
						OTFreeMemory(pAddressSpaceElement[i].m_name);
					}
				}

				OTFreeMemory(pAddressSpaceElement);
			}
		}
		break;
#ifdef OTB_CLIENT_DA

		case CLIENTJOB_TYPE_UPDATEITEMATTRIBUTES:
		{
			if (m_itemCount > 0)
			{
				LONG* pResults;
				pResults = (LONG*)OTAllocateMemory(m_itemCount * sizeof(LONG));
				OTCObjectHandle *p_SODaCItem_Handles = NULL;
				if (m_ppItems != NULL)
				{
					p_SODaCItem_Handles = (OTCObjectHandle*)OTAllocateMemory(m_itemCount * sizeof(OTCObjectHandle));
					for (DWORD i = 0; i < m_itemCount; ++i)
					{
						if (m_ppItems[i])
						{
							p_SODaCItem_Handles[i] = m_ppItems[i]->getOpcHandle();
						}
					}
				}
				m_res = OTCUpdateDAItemAttributes((OTCObjectHandle)m_element->getOpcHandle(), m_itemCount, (OTCObjectHandle*)p_SODaCItem_Handles, m_fromServer, m_attributeCount, m_pWhatAttributes, pResults, &m_executionOptions);
				if (p_SODaCItem_Handles != NULL)
				{
					OTFreeMemory(p_SODaCItem_Handles);
				}
				OTFreeMemory(pResults);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;

		case CLIENTJOB_TYPE_GETPROPERTIES:
		{
			DWORD propertyDataCount;
			OTCDAPropertyData* pProperty;
			DWORD i;
			m_res = OTCGetDAProperties((OTCObjectHandle)m_element->getOpcHandle(), m_addressSpaceElementHandle, (LPTSTR)(LPCTSTR)m_addressSpaceElementID, (LPTSTR)(LPCTSTR)m_addressSpaceElementPath, m_pGetPropertiesOptions, &propertyDataCount, &pProperty, &m_executionOptions);

			if ((SUCCEEDED(m_res)) && (pProperty != NULL))
			{
				for (i = 0; i < propertyDataCount; i++)
				{
					if (pProperty[i].m_itemID)
					{
						OTFreeMemory(pProperty[i].m_itemID);
					}

					if (pProperty[i].m_itemPath)
					{
						OTFreeMemory(pProperty[i].m_itemPath);
					}

					if (pProperty[i].m_description)
					{
						OTFreeMemory(pProperty[i].m_description);
					}

					if (pProperty[i].m_name)
					{
						OTFreeMemory(pProperty[i].m_name);
					}

					::VariantClear(pProperty[i].m_value.m_pValue);
					OTFreeMemory(pProperty[i].m_value.m_pValue);
				}

				OTFreeMemory(pProperty);
			}
		}
		break;

		case CLIENTJOB_TYPE_VALIDATEITEMS:
		{
			if (m_itemCount > 0)
			{
				LONG* pResults;
				pResults = (LONG*)OTAllocateMemory(m_itemCount * sizeof(LONG));
				OTCObjectHandle *p_SODaCItem_Handles = NULL;
				if (m_ppItems != NULL)
				{
					p_SODaCItem_Handles = (OTCObjectHandle*)OTAllocateMemory(m_itemCount * sizeof(OTCObjectHandle));
					for (DWORD i = 0; i < m_itemCount; ++i)
					{
						if (m_ppItems[i])
						{
							p_SODaCItem_Handles[i] = m_ppItems[i]->getOpcHandle();
						}
					}
				}
				m_res = OTCValidateDAItems((OTCObjectHandle)m_element->getOpcHandle(), m_itemCount, (OTCObjectHandle*)p_SODaCItem_Handles, pResults, &m_executionOptions);
				if (p_SODaCItem_Handles != NULL)
				{
					OTFreeMemory(p_SODaCItem_Handles);
				}
				OTFreeMemory(pResults);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;
#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE

		case CLIENTJOB_TYPE_ACKNOWLEDGECONDITIONS:
		{
			if (m_eventCount > 0)
			{
				LONG* pResults;
				pResults = (LONG*)OTAllocateMemory(m_eventCount * sizeof(LONG));
				m_res = OTCAcknowledgeAEConditions((OTCObjectHandle)m_element->getOpcHandle(), m_ackId, m_ackComment, m_eventCount, m_pEventData, pResults, &m_executionOptions);
				OTFreeMemory(pResults);
			}
			else
			{
				m_res = E_INVALIDARG;
			}
		}
		break;

		case CLIENTJOB_TYPE_REFRESHCONDITIONS:
			m_res = OTCRefreshAEConditions((OTCObjectHandle)m_element->getOpcHandle(), m_cancelRefresh, &m_executionOptions);
			break;

		case CLIENTJOB_TYPE_QUERYSOURCECONDITIONS:
		{
			DWORD count;
			LPTSTR* pConditionNames;
			m_res = OTCQueryAESourceConditions((OTCObjectHandle)m_element->getOpcHandle(), m_sourcePath, &count, &pConditionNames, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				DWORD i;

				for (i = 0; i < count; i++)
				{
					OTFreeMemory(pConditionNames[i]);
				}

				OTFreeMemory(pConditionNames);
			}
		}
		break;

		case CLIENTJOB_TYPE_GETCONDITIONSTATE:
		{
			OTCAEConditionStateData conditionState;
			DWORD i;
			m_res = OTCGetAEConditionState((OTCObjectHandle)m_element->getOpcHandle(), m_sourcePath, m_conditionName, m_attributeCount, m_pAttributeIds, &conditionState, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				OTFreeMemory(conditionState.m_actSubConditionName);
				OTFreeMemory(conditionState.m_actSubConditionDescription);
				OTFreeMemory(conditionState.m_actSubConditionDefinition);
				OTFreeMemory(conditionState.m_ackID);
				OTFreeMemory(conditionState.m_ackComment);

				if (conditionState.m_subConditionCount > 0)
				{
					for (i = 0; i < conditionState.m_subConditionCount; i++)
					{
						OTFreeMemory(conditionState.m_pSubConditionDefinition[i]);
						OTFreeMemory(conditionState.m_pSubConditionDescription[i]);
						OTFreeMemory(conditionState.m_pSubConditionName[i]);
					}

					OTFreeMemory(conditionState.m_pSubConditionDefinition);
					OTFreeMemory(conditionState.m_pSubConditionSeverity);
					OTFreeMemory(conditionState.m_pSubConditionDescription);
					OTFreeMemory(conditionState.m_pSubConditionName);
				}

				if (conditionState.m_eventAttrCount > 0)
				{
					for (i = 0; i < conditionState.m_eventAttrCount; i++)
					{
						SOCmnVariant::variantClear(&conditionState.m_pEventAttrs[i]);
					}

					OTFreeMemory(conditionState.m_pAttrErrors);
					OTFreeMemory(conditionState.m_pEventAttrs);
				}
			}
		}
		break;

		case CLIENTJOB_TYPE_QUERYAVAILABLEFILTERS:
		{
			DWORD availableFilters;
			m_res = OTCQueryAvailableAEFilters((OTCObjectHandle)m_element->getOpcHandle(), &availableFilters, &m_executionOptions);
		}
		break;

		case CLIENTJOB_TYPE_QUERYCATEGORIES:
		{
			DWORD count;
			DWORD* pEventTypes;
			DWORD* pCategoryIds;
			LPTSTR* pCategoryDescriptions;
			m_res = OTCQueryAECategories((OTCObjectHandle)m_element->getOpcHandle(), &count, &pEventTypes, &pCategoryIds, &pCategoryDescriptions, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				DWORD i;
				OTFreeMemory(pEventTypes);
				OTFreeMemory(pCategoryIds);

				for (i = 0; i < count; i++)
				{
					OTFreeMemory(pCategoryDescriptions[i]);
				}

				OTFreeMemory(pCategoryDescriptions);
			}
		}
		break;

		case CLIENTJOB_TYPE_QUERYATTRIBUTES:
		{
			DWORD count;
			DWORD* pAttributeIds;
			LPTSTR* pAttributeDescriptions;
			WORD* pAttributeDatatypes;
			m_res = OTCQueryAEAttributes((OTCObjectHandle)m_element->getOpcHandle(), m_categoryId, &count, &pAttributeIds, &pAttributeDescriptions, &pAttributeDatatypes, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				DWORD i;
				OTFreeMemory(pAttributeIds);
				OTFreeMemory(pAttributeDatatypes);

				for (i = 0; i < count; i++)
				{
					OTFreeMemory(pAttributeDescriptions[i]);
				}

				OTFreeMemory(pAttributeDescriptions);
			}
		}
		break;

		case CLIENTJOB_TYPE_QUERYCONDITIONS:
		{
			DWORD count;
			LPTSTR* pConditionNames;
			m_res = OTCQueryAEConditions((OTCObjectHandle)m_element->getOpcHandle(), m_categoryId, &count, &pConditionNames, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				DWORD i;

				for (i = 0; i < count; i++)
				{
					OTFreeMemory(pConditionNames[i]);
				}

				OTFreeMemory(pConditionNames);
			}
		}
		break;

		case CLIENTJOB_TYPE_QUERYSUBCONDITIONS:
		{
			DWORD count;
			LPTSTR* pSubConditionNames;
			m_res = OTCQueryAESubConditions((OTCObjectHandle)m_element->getOpcHandle(), m_conditionName, &count, &pSubConditionNames, &m_executionOptions);

			if (SUCCEEDED(m_res))
			{
				DWORD i;

				for (i = 0; i < count; i++)
				{
					OTFreeMemory(pSubConditionNames[i]);
				}

				OTFreeMemory(pSubConditionNames);
			}
		}
		break;

		case CLIENTJOB_TYPE_ENABLECONDITIONS:
		{
			m_res = OTCEnableAEConditions((OTCObjectHandle)m_element->getOpcHandle(), m_enable, m_areaOrSource, m_sourcePath, &m_executionOptions);
		}
		break;
#endif // OTB_CLIENT_AE

		case CLIENTJOB_TYPE_BROWSESERVER:
		{
			DWORD serverDataCount;
			OTCServerData* pServerData = NULL;
			DWORD i;
			m_res = OTCBrowseServer(m_ipAddress, m_whatOPCSpec, m_whatServerData, &serverDataCount, &pServerData, &m_executionOptions);

			if ((SUCCEEDED(m_res)) && (pServerData != NULL))
			{
				for (i = 0; i < serverDataCount; i++)
				{
					if (pServerData[i].m_clsid)
					{
						OTFreeMemory(pServerData[i].m_clsid);
					}

					if (pServerData[i].m_progId)
					{
						OTFreeMemory(pServerData[i].m_progId);
					}

					if (pServerData[i].m_vProgId)
					{
						OTFreeMemory(pServerData[i].m_vProgId);
					}

					if (pServerData[i].m_description)
					{
						OTFreeMemory(pServerData[i].m_description);
					}

					if (pServerData[i].m_url)
					{
						OTFreeMemory(pServerData[i].m_url);
					}
				}

				OTFreeMemory(pServerData);
			}
		}
		break;

		case CLIENTJOB_TYPE_GETSERVERSTATUS:
		{
			OTCServerStatus status;
			memset(&status, 0, sizeof(OTCServerStatus));
			m_res = OTCGetServerStatus((OTCObjectHandle)m_element->getOpcHandle(), &status, &m_executionOptions);

			if (status.m_vendorInfo)
			{
				OTFreeMemory(status.m_vendorInfo);
			}

			if (status.m_productVersion)
			{
				OTFreeMemory(status.m_productVersion);
			}

			if (status.m_statusInfo)
			{
				OTFreeMemory(status.m_statusInfo);
			}

			if (&status.m_supportedLCIDs)
			{
				OTFreeMemory(status.m_supportedLCIDs);
			}
		}
		break;

		case CLIENTJOB_TYPE_GETERRORSTRING:
		{
			LPTSTR errorString = NULL;
			m_res = OTCGetErrorString((OTCObjectHandle)m_element->getOpcHandle(), m_errorCode, &errorString, &m_executionOptions);

			if (errorString)
			{
				OTFreeMemory(errorString);
			}
		}
		break;

		case CLIENTJOB_TYPE_LOGON:
		{
			m_res = OTCLogon((OTCObjectHandle)m_element->getOpcHandle(), m_userName, m_password, &m_executionOptions);
		}
		break;

		case CLIENTJOB_TYPE_LOGOFF:
		{
			m_res = OTCLogoff((OTCObjectHandle)m_element->getOpcHandle(), &m_executionOptions);
		}
		break;
		}
	}
	SOCMN_CATCH(...)
	{
	}
}

void ClientJob::performStateTransition(IN BOOL deep)
{
	m_jobType = CLIENTJOB_TYPE_PERFORMSTATETRANSITION;
	m_deep = (BYTE)deep;
}

void ClientJob::updateAttributes(BYTE fromServer, DWORD attributeCount, DWORD* pWhatAttributes)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_UPDATEATTRIBUTES;
	m_fromServer = fromServer;
	m_attributeCount = attributeCount;
	m_pWhatAttributes = (DWORD*)OTAllocateMemory(attributeCount * sizeof(DWORD));

	for (i = 0; i < attributeCount; i++)
	{
		m_pWhatAttributes[i] = pWhatAttributes[i];
	}
}

#ifdef OTB_CLIENT_DA

void ClientJob::updateItemAttributes(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, BYTE fromServer, DWORD attributeCount, DWORD* pWhatAttributes)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_UPDATEITEMATTRIBUTES;
	m_itemCount = itemCount;
	m_ppItems = (SODaCItem**)OTAllocateMemory(itemCount * sizeof(SODaCItem*));

	for (i = 0; i < itemCount; i++)
	{
		m_ppItems[i] = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
		if (m_ppItems[i])
		{
			m_ppItems[i]->addRef();
		}
	}

	m_fromServer = fromServer;
	m_attributeCount = attributeCount;
	m_pWhatAttributes = (DWORD*)OTAllocateMemory(attributeCount * sizeof(DWORD));

	for (i = 0; i < attributeCount; i++)
	{
		m_pWhatAttributes[i] = pWhatAttributes[i];
	}
}

void ClientJob::read(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, IN LPCTSTR* pItemIDs, IN LPCTSTR* pItemPaths, IN DWORD maxAge)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_READ;
	m_itemCount = itemCount;

	if (pItemHandles != NULL)
	{
		m_ppItems = (SODaCItem**)OTAllocateMemory(itemCount * sizeof(SODaCItem*));

		for (i = 0; i < itemCount; i++)
		{
			m_ppItems[i] = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
			if (m_ppItems[i])
			{
				m_ppItems[i]->addRef();
			}
		}
	}

	if (pItemIDs != NULL)
	{
		m_pItemIDs = (LPTSTR*)OTAllocateMemory(itemCount * sizeof(LPTSTR*));

		for (i = 0; i < itemCount; i++)
		{
			m_pItemIDs[i] = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pItemIDs[i]) + 1) * sizeof(TCHAR));
			_tcscpy(m_pItemIDs[i], pItemIDs[i]);
		}
	}

	if (pItemPaths != NULL)
	{
		m_pItemPaths = (LPTSTR*)OTAllocateMemory(itemCount * sizeof(LPTSTR*));

		for (i = 0; i < itemCount; i++)
		{
			m_pItemPaths[i] = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pItemPaths[i]) + 1) * sizeof(TCHAR));
			_tcscpy(m_pItemPaths[i], pItemPaths[i]);
		}
	}

	m_maxAge = maxAge;
}

void ClientJob::write(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, IN LPCTSTR* pItemIDs, IN LPCTSTR* pItemPaths, IN OTCValueData* pValues)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_WRITE;
	m_itemCount = itemCount;

	if (pItemHandles != NULL)
	{
		m_ppItems = (SODaCItem**)OTAllocateMemory(itemCount * sizeof(SODaCItem*));

		for (i = 0; i < itemCount; i++)
		{
			m_ppItems[i] = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
			if (m_ppItems[i])
			{
				m_ppItems[i]->addRef();
			}
		}
	}

	if (pItemIDs != NULL)
	{
		m_pItemIDs = (LPTSTR*)OTAllocateMemory(itemCount * sizeof(LPTSTR*));

		for (i = 0; i < itemCount; i++)
		{
			m_pItemIDs[i] = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pItemIDs[i]) + 1) * sizeof(TCHAR));
			_tcscpy(m_pItemIDs[i], pItemIDs[i]);
		}
	}

	if (pItemPaths != NULL)
	{
		m_pItemPaths = (LPTSTR*)OTAllocateMemory(itemCount * sizeof(LPTSTR*));

		for (i = 0; i < itemCount; i++)
		{
			m_pItemPaths[i] = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pItemPaths[i]) + 1) * sizeof(TCHAR));
			_tcscpy(m_pItemPaths[i], pItemPaths[i]);
		}
	}

	m_pValues = (OTCValueData*)OTAllocateMemory(m_itemCount * sizeof(OTCValueData));

	for (i = 0; i < m_itemCount; i++)
	{
		m_pValues[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		::VariantInit(m_pValues[i].m_pValue);
		SOCmnVariant::variantCopy(m_pValues[i].m_pValue, pValues[i].m_pValue);
		m_pValues[i].m_quality = pValues[i].m_quality;
#ifdef SOOS_WINDOWS
		m_pValues[i].m_timestamp.dwHighDateTime = pValues[i].m_timestamp.dwHighDateTime;
		m_pValues[i].m_timestamp.dwLowDateTime = pValues[i].m_timestamp.dwLowDateTime;
#endif
#ifdef SOOS_LINUX
		m_pValues[i].m_timestamp.tv_sec = pValues[i].m_timestamp.tv_sec;
		m_pValues[i].m_timestamp.tv_usec = pValues[i].m_timestamp.tv_usec;
#endif
	}
}

void ClientJob::validateItems(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_VALIDATEITEMS;
	m_itemCount = itemCount;

	if (pItemHandles != NULL)
	{
		m_ppItems = (SODaCItem**)OTAllocateMemory(itemCount * sizeof(SODaCItem*));

		for (i = 0; i < itemCount; i++)
		{
			m_ppItems[i] = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(pItemHandles[i]);
			if (m_ppItems[i])
			{
				m_ppItems[i]->addRef();
			}
		}
	}
}

void ClientJob::getProperties(IN OTCObjectHandle addressSpaceElementHandle, IN LPCTSTR addressSpaceElementID, IN LPCTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_GETPROPERTIES;
	m_addressSpaceElementHandle = addressSpaceElementHandle;

	if (addressSpaceElementID)
	{
		m_addressSpaceElementID = addressSpaceElementID;
	}

	if (addressSpaceElementPath)
	{
		m_addressSpaceElementPath = addressSpaceElementPath;
	}

	if (pGetPropertiesOptions)
	{
		m_pGetPropertiesOptions = (OTCGetDAPropertiesOptions*)OTAllocateMemory(sizeof(OTCGetDAPropertiesOptions));
		m_pGetPropertiesOptions->m_getLocalFromAddressSpaceElement = pGetPropertiesOptions->m_getLocalFromAddressSpaceElement;
		m_pGetPropertiesOptions->m_whatPropertyData = pGetPropertiesOptions->m_whatPropertyData;
		m_pGetPropertiesOptions->m_propertyCount = pGetPropertiesOptions->m_propertyCount;

		if (pGetPropertiesOptions->m_pPropertyIDs != NULL)
		{
			m_pGetPropertiesOptions->m_pPropertyIDs = (DWORD*)OTAllocateMemory(m_pGetPropertiesOptions->m_propertyCount * sizeof(DWORD));

			for (i = 0; i < m_pGetPropertiesOptions->m_propertyCount; i++)
			{
				m_pGetPropertiesOptions->m_pPropertyIDs[i] = pGetPropertiesOptions->m_pPropertyIDs[i];
			}
		}

		if (pGetPropertiesOptions->m_pPropertyNames != NULL)
		{
			m_pGetPropertiesOptions->m_pPropertyNames = (LPTSTR*)OTAllocateMemory(m_pGetPropertiesOptions->m_propertyCount * sizeof(LPTSTR));

			for (i = 0; i < m_pGetPropertiesOptions->m_propertyCount; i++)
			{
				m_pGetPropertiesOptions->m_pPropertyNames[i] = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pGetPropertiesOptions->m_pPropertyNames[i]) + 1) * sizeof(TCHAR));
				_tcscpy((LPTSTR)m_pGetPropertiesOptions->m_pPropertyNames[i], pGetPropertiesOptions->m_pPropertyNames[i]);
			}
		}
	}
}

#endif // OTB_CLIENT_DA

void ClientJob::browseServer(IN LPCTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData)
{
	m_jobType = CLIENTJOB_TYPE_BROWSESERVER;
	m_ipAddress = ipAddress;
	m_whatOPCSpec = whatOPCSpec;
	m_whatServerData = whatServerData;
}

void ClientJob::browseAddressSpace(IN OTCObjectHandle addressSpaceElementHandle, IN LPCTSTR addressSpaceElementID, IN LPCTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions)
{
	m_jobType = CLIENTJOB_TYPE_BROWSEADDRESSSPACE;
	m_addressSpaceElementHandle = addressSpaceElementHandle;

	if (addressSpaceElementID)
	{
		m_addressSpaceElementID = addressSpaceElementID;
	}

	if (addressSpaceElementPath)
	{
		m_addressSpaceElementPath = addressSpaceElementPath;
	}

	if (pBrowseOptions)
	{
		m_pBrowseOptions = (OTCAddressSpaceBrowseOptions*)OTAllocateMemory(sizeof(OTCAddressSpaceBrowseOptions));
		m_pBrowseOptions->m_accessRightsFilter = pBrowseOptions->m_accessRightsFilter;
		m_pBrowseOptions->m_datatypeFilter = pBrowseOptions->m_datatypeFilter;
		m_pBrowseOptions->m_maxElements = pBrowseOptions->m_maxElements;
		m_pBrowseOptions->m_retrieveItemID = pBrowseOptions->m_retrieveItemID;
		m_pBrowseOptions->m_retrieveProperties = pBrowseOptions->m_retrieveProperties;
		m_pBrowseOptions->m_retrievePropertyValues = pBrowseOptions->m_retrievePropertyValues;
		m_pBrowseOptions->m_type = pBrowseOptions->m_type;
		m_pBrowseOptions->m_forceBrowseUp = pBrowseOptions->m_forceBrowseUp;

		if (pBrowseOptions->m_elementNameFilter)
		{
			m_pBrowseOptions->m_elementNameFilter = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pBrowseOptions->m_elementNameFilter) + 1) * sizeof(TCHAR));
			_tcscpy((LPTSTR)m_pBrowseOptions->m_elementNameFilter, pBrowseOptions->m_elementNameFilter);
		}

		if (pBrowseOptions->m_vendorFilter)
		{
			m_pBrowseOptions->m_vendorFilter = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pBrowseOptions->m_vendorFilter) + 1) * sizeof(TCHAR));
			_tcscpy((LPTSTR)m_pBrowseOptions->m_vendorFilter, pBrowseOptions->m_vendorFilter);
		}
	}
}

#ifdef OTB_CLIENT_AE

void ClientJob::refreshConditions(IN BYTE cancelRefresh)
{
	m_jobType = CLIENTJOB_TYPE_REFRESHCONDITIONS;
	m_cancelRefresh = cancelRefresh;
}

void ClientJob::acknowledgeConditions(IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_ACKNOWLEDGECONDITIONS;
	m_ackId = ackID;
	m_ackComment = ackComment;
	m_eventCount = count;
	m_pEventData = (OTCEventData*)OTAllocateMemory(count * sizeof(OTCEventData));

	for (i = 0; i < count; i++)
	{
		m_pEventData[i].m_cookie = pEvents[i].m_cookie;
		m_pEventData[i].m_activeTime = pEvents[i].m_activeTime;
		m_pEventData[i].m_conditionName = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pEvents[i].m_conditionName) + 1) * sizeof(TCHAR));
		_tcscpy(m_pEventData[i].m_conditionName, pEvents[i].m_conditionName);
		m_pEventData[i].m_sourcePath = (LPTSTR)OTAllocateMemory((DWORD)(_tcslen(pEvents[i].m_sourcePath) + 1) * sizeof(TCHAR));
		_tcscpy(m_pEventData[i].m_sourcePath, pEvents[i].m_sourcePath);
	}
}

void ClientJob::queryAvailableFilters(void)
{
	m_jobType = CLIENTJOB_TYPE_QUERYAVAILABLEFILTERS;
}

void ClientJob::queryCategories(void)
{
	m_jobType = CLIENTJOB_TYPE_QUERYCATEGORIES;
}

void ClientJob::queryAttributes(IN DWORD categoryId)
{
	m_jobType = CLIENTJOB_TYPE_QUERYATTRIBUTES;
	m_categoryId = categoryId;
}

void ClientJob::queryConditions(IN DWORD categoryId)
{
	m_jobType = CLIENTJOB_TYPE_QUERYCONDITIONS;
	m_categoryId = categoryId;
}

void ClientJob::querySubConditions(IN LPTSTR conditionName)
{
	m_jobType = CLIENTJOB_TYPE_QUERYSUBCONDITIONS;
	m_conditionName = conditionName;
}

void ClientJob::querySourceConditions(IN LPTSTR sourcePath)
{
	m_jobType = CLIENTJOB_TYPE_QUERYSOURCECONDITIONS;
	m_sourcePath = sourcePath;
}

void ClientJob::getConditionState(IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds)
{
	DWORD i;
	m_jobType = CLIENTJOB_TYPE_GETCONDITIONSTATE;
	m_conditionName = conditionName;
	m_sourcePath = sourcePath;
	m_attributeCount = attributeCount;

	if ((attributeCount > 0) && (m_pAttributeIds != NULL))
	{
		m_pAttributeIds = (DWORD*)OTAllocateMemory(attributeCount * sizeof(DWORD));

		for (i = 0; i < attributeCount; i++)
		{
			m_pAttributeIds[i] = pAttributeIds[i];
		}
	}
}

void ClientJob::enableConditions(IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path)
{
	m_jobType = CLIENTJOB_TYPE_ENABLECONDITIONS;
	m_sourcePath = path;
	m_enable = enable;
	m_areaOrSource = areaOrSource;
}

#endif // OTB_CLIENT_AE

void ClientJob::getErrorString(IN LONG errorCode)
{
	m_jobType = CLIENTJOB_TYPE_GETERRORSTRING;
	m_errorCode = errorCode;
}

void ClientJob::logon(IN LPTSTR userName, IN LPTSTR password)
{
	m_jobType = CLIENTJOB_TYPE_LOGON;
	m_userName = userName;
	m_password = password;
}

void ClientJob::logoff(void)
{
	m_jobType = CLIENTJOB_TYPE_LOGOFF;
}

#endif // OTB_CLIENT

