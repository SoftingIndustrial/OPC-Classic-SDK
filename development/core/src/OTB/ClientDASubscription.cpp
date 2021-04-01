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
//  Filename    : ClientDASubscription.cpp                                    |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Subscription class                                      |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT_DA

#include "ClientDASubscription.h"
#include "ClientDAItem.h"
#include "ClientJob.h"
#include "OTC.h"

ClientDASubscription::ClientDASubscription(void)
{
	m_userData = 0;
	m_storedTargedState = disconnected;
}

void ClientDASubscription::storeTargetState(IN BYTE deep)
{
	m_storedTargedState = getTargetObjectState();

	if (deep)
	{
		ClientDAItem* pItem;
		SOCmnList<ClientDAItem> itemList((SOCmnList<ClientDAItem> *)getLeafList());
		SOCmnListPosition pos;
		pos = itemList.getStartPosition();

		while (pos)
		{
			pItem = itemList.getNext(pos);
			pItem->storeTargetState();
		}
	}
}

void ClientDASubscription::restoreTargetState(IN BYTE deep)
{
	setTargetObjectState(m_storedTargedState);

	if (deep)
	{
		ClientDAItem* pItem;
		SOCmnList<ClientDAItem> itemList((SOCmnList<ClientDAItem> *)getLeafList());
		SOCmnListPosition pos;
		pos = itemList.getStartPosition();

		while (pos)
		{
			pItem = itemList.getNext(pos);
			pItem->restoreTargetState();
		}
	}
}

void ClientDASubscription::onSetObjectState(void)
{
	if (g_callbackFunctions.m_OTCOnStateChange)
	{
		OTCObjectContext objectContext;
		getOTCObjectContext((OTCObjectHandle)this->getOpcHandle(), &objectContext);

		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnStateChange(objectContext, getObjectState());
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASubscription::onSetObjectState - Exception occurred in vendor application callback: OTCOnStateChange")));
		}
	}
}

void ClientDASubscription::onReadAdvise(
	IN BYTE dataChangeOrRead,
	IN DWORD transactionId,
	IN HRESULT /* masterQuality */,
	IN HRESULT masterError,
	IN DWORD itemCnt,
	IN SODaCItem** ppItems,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	IN HRESULT* pErrors)
{
	OTCObjectContext objectContext;
	OTCObjectData* pItemData;
	OTCValueData* pValueData;
	LPCTSTR* pItemIDs = NULL;
	LPCTSTR* pItemPaths = NULL;
	DWORD i;
	getOTCObjectContext((OTCObjectHandle)this->getOpcHandle(), &objectContext);
	pItemData = (OTCObjectData*) malloc(itemCnt * sizeof(OTCObjectData));
	pValueData = (OTCValueData*)malloc(itemCnt * sizeof(OTCValueData));

	if (transactionId != 0)
	{
		pItemIDs = (LPCTSTR*) malloc(itemCnt * sizeof(LPCTSTR));
		pItemPaths = (LPCTSTR*) malloc(itemCnt * sizeof(LPCTSTR));
	}

	for (i = 0; i < itemCnt; i++)
	{
		pItemData[i].m_objectHandle = 0;
		pItemData[i].m_userData = (unsigned long)NULL;

		if (ppItems[i])
		{
			pItemData[i].m_objectHandle = (OTCObjectHandle)ppItems[i]->getOpcHandle();
			pItemData[i].m_userData = ((ClientDAItem*)ppItems[i])->getOTCUserData();
			pValueData[i].m_pValue = pValues[i];
			pValueData[i].m_quality = pQualities[i];
			pTimestamps[i].get(&pValueData[i].m_timestamp);

			if (transactionId != 0)
			{
				pItemIDs[i] = _tcsdup((LPCTSTR)ppItems[i]->getItemID());
				pItemPaths[i] = _tcsdup((LPCTSTR)ppItems[i]->getItemPath());
			}
		}
	}

	if ((dataChangeOrRead) && (g_callbackFunctions.m_OTCOnDataChange))
	{
		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnDataChange(transactionId, objectContext, itemCnt, pItemData, pValueData, (LONG*)pErrors);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASubscription::onReadAdvise - Exception occurred in vendor application callback: OTCOnDataChange")));
		}
	}
	else if ((!dataChangeOrRead) && (g_callbackFunctions.m_OTCOnReadComplete))
	{
		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnReadComplete(transactionId, objectContext, masterError, itemCnt, pItemData, (LPTSTR*)pItemIDs, (LPTSTR*)pItemPaths, pValueData, (LONG*)pErrors);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASubscription::onReadAdvise - Exception occurred in vendor application callback: OTCOnReadComplete")));
		}
	}

	if (transactionId != 0)
	{
		for (i = 0; i < itemCnt; i++)
		{
			free((void*)pItemIDs[i]);
			free((void*)pItemPaths[i]);
		}

		free(pItemIDs);
		free(pItemPaths);
	}

	free(pItemData);
	free(pValueData);
}

void ClientDASubscription::onWriteAdvise(IN DWORD transactionId, IN HRESULT masterError, IN DWORD itemCnt,
										 IN SODaCItem** ppItems, IN HRESULT* pErrors)
{
	OTCObjectContext objectContext;
	OTCObjectData* pItemData;
	LPCTSTR* pItemIDs = NULL;
	LPCTSTR* pItemPaths = NULL;
	DWORD i;
	getOTCObjectContext((OTCObjectHandle)this->getOpcHandle(), &objectContext);
	pItemData = (OTCObjectData*) malloc(itemCnt * sizeof(OTCObjectData));
	pItemIDs = (LPCTSTR*) malloc(itemCnt * sizeof(LPCTSTR));
	pItemPaths = (LPCTSTR*) malloc(itemCnt * sizeof(LPCTSTR));

	for (i = 0; i < itemCnt; i++)
	{
		pItemData[i].m_objectHandle = 0;
		pItemData[i].m_userData = 0;

		if (ppItems[i])
		{
			pItemData[i].m_objectHandle = (OTCObjectHandle)ppItems[i]->getOpcHandle();
			pItemData[i].m_userData = ((ClientDAItem*)ppItems[i])->getOTCUserData();
			pItemIDs[i] = _tcsdup((LPCTSTR)ppItems[i]->getItemID());
			pItemPaths[i] = _tcsdup((LPCTSTR)ppItems[i]->getItemPath());
		}
	}

	if (g_callbackFunctions.m_OTCOnWriteComplete)
	{
		// check why values are set to null instead of a valid value
		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnWriteComplete(transactionId, objectContext, masterError, itemCnt, pItemData, (LPTSTR*)pItemIDs, (LPTSTR*)pItemPaths, NULL, (LONG*)pErrors);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASubscription::onWriteAdvise - Exception occurred in vendor application callback: OTCOnWriteComplete")));
		}		
	}

	for (i = 0; i < itemCnt; i++)
	{
		free((void*)pItemIDs[i]);
		free((void*)pItemPaths[i]);
	}

	free(pItemIDs);
	free(pItemPaths);
	free(pItemData);
}

#ifdef SOFEATURE_DCOM

ClientDASubscriptionCom::ClientDASubscriptionCom(void)
{}

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP

ClientDASubscriptionXml::ClientDASubscriptionXml(void)
{}

#endif // SOFEATURE_SOAP

#ifdef SOFEATURE_TUNNEL

ClientDASubscriptionTp::ClientDASubscriptionTp(void)
{}

#endif // SOFEATURE_TUNNEL

#endif // OTB_CLIENT_DA
