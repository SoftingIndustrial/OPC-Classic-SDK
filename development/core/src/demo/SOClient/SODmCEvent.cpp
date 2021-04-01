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
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : SODmCEvent.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : AE event classes                                            |
//                - SODmCEvent: Toolkit event object class                    |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "SOClient.h"
#include "SODmCPropFrm.h"
#include "SODmCEvent.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"
#include "SODmCServerAE.h"
#include "SODmCSubscription.h"


//-----------------------------------------------------------------------------
// SODmCEvent                                                                 |
//-----------------------------------------------------------------------------

SODmCEvent::SODmCEvent(SOAeCSubscription* subs, IN ONEVENTSTRUCT* src)
	: SOAeCEvent(subs, src)
{
	m_listIdx = -1;
}

//-----------------------------------------------------------------------------
// onEventReceived
//
// - called after received an event
//
void SODmCEvent::onEventReceived(void)
{
	SODmCMainView* mainview = getMainView();
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)m_subs->getParentBranch();
	int img;
	SOCmnString conv;

	if (m_eventType != OPC_CONDITION_EVENT)
	{
		// simple and tracking related events -> log the event in the list
		CListCtrl* list = mainview->getListAE();

		if (m_eventType == OPC_SIMPLE_EVENT)
		{
			img = SODMCMAINVIEW_IMG_SIMPLEEVENT;
		}
		else
		{
			img = SODMCMAINVIEW_IMG_TRACKINGEVENT;
		}

		addRef();
		int idx = SODmCListPush(list, m_source, (void*)(SODmCEvent*)this, img);
		conv.format(_T("%lu"), m_severity);
		list->SetItemText(idx, 1, conv);
		list->SetItemText(idx, 2, m_message);
		FILETIME2STRING(&conv, &m_ftTime);
		list->SetItemText(idx, 3, conv);
		list->SetItemText(idx, 4, m_actorID);
		list->SetItemText(idx, 5, server->getURL());
#ifdef SODMC_DCOM

		if (((SOAeCSubscription*)m_subs)->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			list->SetItemText(idx, 6, ((SODmCComSubscription*)(SOAeCSubscription*)m_subs)->getLabel());
		}

#endif
#ifdef SOFEATURE_TUNNEL

		if (((SOAeCSubscription*)m_subs)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
		{
			list->SetItemText(idx, 6, ((SODmCTPSubscription*)(SOAeCSubscription*)m_subs)->getLabel());
		}

#endif
		DWORD itmCnt = list->GetItemCount();

		if (itmCnt > 70)
		{
			DWORD i;

			for (i = itmCnt - 1; i >= 70; i--)
			{
				list->DeleteItem(i);
			}
		}
	}
	else
	{
		// condition related events
		CListCtrl* list = mainview->getListAEC();
		SODmCEvent* event;
		int idx = -1;

		// try to find an entry with the source and condition
		for (idx = 0; idx < list->GetItemCount(); idx++)
		{
			event = (SODmCEvent*)(SOCltElement*)list->GetItemData(idx);

			if (event)
			{
				if (((SOAeCSubscription*)event->m_subs == (SOAeCSubscription*)m_subs) && (event->m_source == m_source) && (event->m_conditionName == m_conditionName))
				{
					// found the entry
					list->DeleteItem(idx);
					break;
				}
			}
		}

		if ((m_newState & (OPC_CONDITION_ACTIVE | OPC_CONDITION_ACKED)) != OPC_CONDITION_ACKED)
		{
			// not active and acknowledged
			addRef();

			// find the right index for the new condition
			for (idx = 0; idx < list->GetItemCount(); idx++)
			{
				event = (SODmCEvent*)(SOCltElement*)list->GetItemData(idx);

				if (event)
				{
					if ((event->m_newState & (OPC_CONDITION_ACTIVE | OPC_CONDITION_ACKED)) == (m_newState & (OPC_CONDITION_ACTIVE | OPC_CONDITION_ACKED)))
					{
						// same state check priority
						if (event->m_severity < m_severity)
						{
							break;
						}
					}

					if ((event->m_newState & OPC_CONDITION_ACKED) && (!(m_newState & OPC_CONDITION_ACKED)))
					{
						break;
					}

					if ((!(event->m_newState & OPC_CONDITION_ACKED)) && (!(m_newState & OPC_CONDITION_ACKED)))
						if ((!(event->m_newState & OPC_CONDITION_ACTIVE)) && ((m_newState & OPC_CONDITION_ACTIVE)))
						{
							break;
						}
				}
			}

			LV_ITEM lv;
			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lv.iItem = idx;
			lv.iSubItem = 0;
			lv.pszText = (LPTSTR)m_source;
			lv.lParam = (LPARAM)(void*)(SODmCEvent*)this;
			lv.iImage = SODMCMAINVIEW_IMG_CONDITIONEVENT +  m_newState;
			list->InsertItem(&lv);
			conv.format(_T("%lu"), m_severity);
			list->SetItemText(idx, 2, conv);
			list->SetItemText(idx, 3, m_message);
			FILETIME2STRING(&conv, &m_ftTime);
			list->SetItemText(idx, 4, conv);
			list->SetItemText(idx, 1, m_conditionName);
			list->SetItemText(idx, 5, m_actorID);
			list->SetItemText(idx, 6, m_subconditionName);
			list->SetItemText(idx, 7, server->getURL());
#ifdef SODMC_DCOM

			if (((SOAeCSubscription*)m_subs)->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				list->SetItemText(idx, 8, ((SODmCComSubscription*)(SOAeCSubscription*)m_subs)->getLabel());
			}

#endif
#ifdef SOFEATURE_TUNNEL

			if (((SOAeCSubscription*)m_subs)->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
			{
				list->SetItemText(idx, 8, ((SODmCTPSubscription*)(SOAeCSubscription*)m_subs)->getLabel());
			}

#endif
		}
	}
} // onEventReceived


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCEvent::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError




