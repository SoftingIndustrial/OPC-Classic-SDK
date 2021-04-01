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
//  Filename    : ClientAESubscription.cpp                                    |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC AE Subscription class                                   |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT_AE

#include "ClientAESubscription.h"
#include "OTC.h"


ClientAESubscription::ClientAESubscription(void)
{
	m_userData = 0;
	m_storedTargedState = disconnected;
}

void ClientAESubscription::storeTargetState(IN BYTE /* deep */)
{
	m_storedTargedState = getTargetObjectState();
}

void ClientAESubscription::restoreTargetState(IN BYTE /* deep */)
{
	setTargetObjectState(m_storedTargedState);
}


void ClientAESubscription::onSetObjectState(void)
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
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientAESubscription::onSetObjectState - Exception occurred in vendor application callback: OTCOnStateChange")));
		}
	}
}

void ClientAESubscription::onReceivedEvents(DWORD count, SOAeCEvent** pEvents, BYTE refresh, BYTE lastRefresh)
{
	if (g_callbackFunctions.m_OTCOnReceivedEvents)
	{
		OTCObjectContext objectContext;
		OTCEventData* pEventsO;
		DWORD i, j;
		SOCmnList<SOCmnVariant> eventAttrs;
		SOCmnListPosition pos;
		SOCmnVariant* pVariant;
		getOTCObjectContext((OTCObjectHandle)this->getOpcHandle(), &objectContext);
		pEventsO = (OTCEventData*)OTAllocateMemory(sizeof(OTCEventData) * count);

		for (i = 0; i < count; i++)
		{
			pEventsO[i].m_eventType = pEvents[i]->m_eventType;
			pEventsO[i].m_eventCategory = pEvents[i]->m_eventCategory;
			pEventsO[i].m_sourcePath = pEvents[i]->m_source;
			pEventsO[i].m_severity = pEvents[i]->m_severity;
			pEventsO[i].m_message = pEvents[i]->m_message;
			pEventsO[i].m_actorID = pEvents[i]->m_actorID;
			pEventsO[i].m_occurenceTime = pEvents[i]->m_ftTime;
			pEventsO[i].m_changeMask = pEvents[i]->m_changeMask;
			pEventsO[i].m_newState = (BYTE)pEvents[i]->m_newState;
			pEventsO[i].m_ackRequired = (BYTE)pEvents[i]->m_bAckRequired;
			pEventsO[i].m_quality = pEvents[i]->m_quality;
			pEventsO[i].m_conditionName = pEvents[i]->m_conditionName;
			pEventsO[i].m_subConditionName = pEvents[i]->m_subconditionName;
			pEventsO[i].m_activeTime = pEvents[i]->m_ftActiveTime;
			pEventsO[i].m_cookie = pEvents[i]->m_dwCookie;
			eventAttrs = pEvents[i]->m_listEventAttrs;
			pEventsO[i].m_eventAttrCount = eventAttrs.getCount();

			if (pEventsO[i].m_eventAttrCount > 0)
			{
				pEventsO[i].m_pEventAttrs = (OTVariant*)OTAllocateMemory(sizeof(OTVariant) * pEventsO[i].m_eventAttrCount);
				pos = eventAttrs.getStartPosition();
				j = 0;

				while (pos)
				{
					pVariant = eventAttrs.getNext(pos);
					pVariant->copyTo(&pEventsO[i].m_pEventAttrs[j]);
					j++;
				}
			}
			else
			{
				pEventsO[i].m_pEventAttrs = NULL;
			}
		}

		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnReceivedEvents(objectContext, count, pEventsO, refresh, lastRefresh);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientAESubscription::onReceivedEvents - Exception occurred in vendor application callback: OTCOnReceivedEvents")));
		}

		for (i = 0; i < count; i++)
		{
			if (pEventsO[i].m_pEventAttrs)
			{
				for (j = 0; j < pEventsO[i].m_eventAttrCount; j++)
				{
					SOCmnVariant::variantClear(&pEventsO[i].m_pEventAttrs[j]);
				}

				OTFreeMemory(pEventsO[i].m_pEventAttrs);
			}
		}

		OTFreeMemory(pEventsO);
	}
}


#ifdef SOFEATURE_DCOM

ClientAESubscriptionCom::ClientAESubscriptionCom(void)
{}

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_TUNNEL

ClientAESubscriptionTp::ClientAESubscriptionTp(void)
{}

#endif // SOFEATURE_TUNNEL

#endif // OTB_CLIENT_AE


