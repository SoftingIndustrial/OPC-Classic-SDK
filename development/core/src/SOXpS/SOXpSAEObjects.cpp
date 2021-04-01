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
//                           OPC Toolkit - SOXpS                              |
//                                                                            |
//  Filename    : SOXpSAEObjects.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : AE object classes                                           |
//                - SOXpSConditionClassSingleState: single state condition    |
//                - SOXpSConditionClassMultipleState: multiple state condition|
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "SOXpS.h"
#include "SOXpSCallbacks.h"
#include "SOXpSAEObjects.h"
#include "DllMain.h"


//-----------------------------------------------------------------------------
// SOXpSAEServer                                                              |
//-----------------------------------------------------------------------------

extern HANDLE g_startupEvent;

SOXpSAEServer::SOXpSAEServer(void)
{}

//-----------------------------------------------------------------------------
// supportEnableCondition
//
// - support en/disabling of conditions by area or source
//
// returns:
//		FALSE - not supported
//
BOOL SOXpSAEServer::supportEnableCondition(
	IN BOOL enable,			// en/disable
	IN BOOL areaOrSource)	// area or source
{
	return FALSE;
} // supportEnableCondition


//-----------------------------------------------------------------------------
// waitForNameSpaceStartup
//
// - wait for the name space creation to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOXpSAEServer::waitForNameSpaceStartup(void)
{
	WaitForSingleObject(g_startupEvent, INFINITE);
	return S_OK;
} // waitForNameSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT SOXpSAEServer::waitForIOStartup(void)
{
	WaitForSingleObject(g_startupEvent, INFINITE);
	return S_OK;
} // waitForIOStartup




//-----------------------------------------------------------------------------
// SOXpSSubscription                                                          |
//-----------------------------------------------------------------------------

DWORD s_activeSubscriptionCnt = 0;

SOXpSSubscription::~SOXpSSubscription(void)
{
	setActive(FALSE);
}

BOOL SOXpSSubscription::setActive(IN BOOL active)
{
	BOOL curState = m_active;
	if (SOAeSSubscriptionDefault::setActive(active))
	{
		if (m_active != curState)
		{
			BOOL callFct = FALSE;

			if (active)
			{
				callFct = (s_activeSubscriptionCnt == 0);
				s_activeSubscriptionCnt++;
			}
			else
			{
				s_activeSubscriptionCnt--;
				callFct = (s_activeSubscriptionCnt == 0);
			}

			if ((callFct) && (g_fpSOXpSActivateAESubscription))
				g_fpSOXpSActivateAESubscription(active);
		}

		return TRUE;
	}
	else
		return FALSE;
}



//-----------------------------------------------------------------------------
// SOXpSConditionClassSingleState                                             |
//-----------------------------------------------------------------------------

SOXpSConditionClassSingleState::SOXpSConditionClassSingleState(void)
{}

//-----------------------------------------------------------------------------
// getInactiveMessage
//
// - get inactive message of the condition class
//
// returns:
//		message
//
SOCmnString SOXpSConditionClassSingleState::getInactiveMessage(void)
{
	return m_inactiveMessage;
} // getInactiveMessage


//-----------------------------------------------------------------------------
// getInactiveAckRequired
//
// - get inactive ack required of the condition class
//
// returns:
//		ack required
//
BOOL SOXpSConditionClassSingleState::getInactiveAckRequired(void)
{
	return m_inactiveAckReq;
} // getInactiveAckRequired


//-----------------------------------------------------------------------------
// getActiveAckRequired
//
// - get active ack required of the condition class
//
// returns:
//		ack required
//
BOOL SOXpSConditionClassSingleState::getActiveAckRequired(void)
{
	return m_activeAckReq;
} // getActiveAckRequired




//-----------------------------------------------------------------------------
// SOXpSConditionClassMultipleState                                           |
//-----------------------------------------------------------------------------


SOXpSConditionClassMultipleState::SOXpSConditionClassMultipleState(void)
{}


//-----------------------------------------------------------------------------
// getInactiveMessage
//
// - get inactive message of the condition class
//
// returns:
//		message
//
SOCmnString SOXpSConditionClassMultipleState::getInactiveMessage(void)
{
	return m_inactiveMessage;
} // getInactiveMessage


//-----------------------------------------------------------------------------
// getInactiveAckRequired
//
// - get inactive ack required of the condition class
//
// returns:
//		ack required
//
BOOL SOXpSConditionClassMultipleState::getInactiveAckRequired(void)
{
	return m_inactiveAckReq;
} // getInactiveAckRequired




//-----------------------------------------------------------------------------
// SOXpSCondition                                                             |
//-----------------------------------------------------------------------------

extern BOOL g_condRelEventsSendsActorID;

SOXpSCondition::SOXpSCondition(void)
 :	SOAeSCondition()
{
	m_instanceData = FALSE;
	m_ackRequired = FALSE;
	m_severity = 0;
	m_deviceState = SOWATCH_DEVSTATE_OK;
}

//-----------------------------------------------------------------------------
// getObjectAeSConditionIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject *SOXpSCondition::getObjectAeSConditionIAttr(void)
{
	return (SOCmnObject *)this;
} // getObjectAeSConditionIAttr


//-----------------------------------------------------------------------------
// getSeverity
//
// - get the severity for the actual state of the condition
//
// returns:
//		message
//
DWORD SOXpSCondition::getSeverity(void)
{
	if (m_instanceData & SOXPS_CONDITIONDATA_SEVERITY)
		return m_severity;
	else
		return SOAeSCondition::getSeverity();
} // getSeverity


//-----------------------------------------------------------------------------
// getAckRequired
//
// - get the ack required for the actual state of the condition
//
// returns:
//		message
//
BOOL SOXpSCondition::getAckRequired(
	IN WORD changeMask)
{
	if ((changeMask & (OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_SUBCONDITION)))
	{
		if (m_instanceData & SOXPS_CONDITIONDATA_ACKREQUIRED)
			return m_ackRequired;
	}

	return SOAeSCondition::getAckRequired(changeMask);
} // getAckRequired


//-----------------------------------------------------------------------------
// getMessage
//
// - get the message for the actual state of the condition
//
// returns:
//		message
//
SOCmnString SOXpSCondition::getMessage(void)
{
	if (m_instanceData & SOXPS_CONDITIONDATA_MESSAGE)
		return m_message;
	else
		return SOAeSCondition::getMessage();
} // getMessage


//-----------------------------------------------------------------------------
// translateToItemIDs
//
// - get the OPC DA item IDs for the attributes of a (sub)condition
//
// returns:
//		S_OK
//
HRESULT SOXpSCondition::translateToItemIDs(
	IN LPCTSTR subConditionName,
	OUT SOCmnList<SOAeSAttributeValueDADescription> *descrDAList)
{
	if (g_fpSOXpSTranslateToItemIDs)
	{
		SOCmnPointer<SOAeSSource> source = (SOAeSSource *)getParentBranch();
		SOCmnPointer<SOAeSCategory> cat = (SOAeSCategory *)m_conditionClass->getParentBranch();
		SOCmnString sourcePath;
		SOCmnString condName;
		DWORD *attrIdArray;
		DWORD numAttr;

		condName = m_conditionClass->getName();
		sourcePath = source->getFullName();

		numAttr = descrDAList->getCount();
		attrIdArray = (DWORD *)malloc(sizeof(DWORD) * numAttr);
		if (attrIdArray)
		{
			SOAeSAttributeValueDADescription *attrValDescr;
			SOCmnListPosition pos;
			DWORD i = 0;
			SOCmnSingleLock<SOCmnSync> lock(&g_lockTranslateToItemID);

			pos = descrDAList->getStartPosition();
			while (pos)
			{
				attrValDescr = descrDAList->getNext(pos);
				attrIdArray[i] = attrValDescr->m_attrID;
				i++;
			}

			g_translateToItemIDList = descrDAList;
			g_fpSOXpSTranslateToItemIDs((SOXpSConditionHandle)((SOAeSCondition *)this), sourcePath,
				condName, subConditionName, numAttr, attrIdArray, &_SOXpSTranslatedItemID);
		}
		else
			return E_OUTOFMEMORY;
		return S_OK;
	}
	return E_NOTIMPL;
} // translateToItemIDs


//-----------------------------------------------------------------------------
// handleAcknowledge
//
// - handle the valid acknowledge from the client
//
// returns:
//		S_OK  - accept the acknowledge
//		any other HRESULT error to deny the acknowledge
//
HRESULT SOXpSCondition::handleAcknowledge(
	IN LPCTSTR ackID,
	IN LPCTSTR ackComment,
	IN LPFILETIME activeTime)
{
	FILETIME ackTime;
	HRESULT res = S_OK;

	::CoFileTimeNow(&ackTime);
	if (g_fpSOXpSReceivedAEAcknowledge)
	{
		SOCmnPointer<SOAeSSource> source = (SOAeSSource *)getParentBranch();
		SOCmnPointer<SOAeSCategory> cat = (SOAeSCategory *)m_conditionClass->getParentBranch();
		SOCmnString sourcePath;
		SOCmnString condName;

		condName = m_conditionClass->getName();
		sourcePath = source->getFullName();

		SOCmnSingleLock<SOCmnSync> lock(&g_lockReceivedAEAcknowledge);
		if (!g_fpSOXpSReceivedAEAcknowledge((SOXpSConditionHandle)((SOAeSCondition *)this), sourcePath,
				condName, ackID, ackComment, activeTime))
			res = E_FAIL;
	}

	if (SUCCEEDED(res))
	{
		// set the acknowledge values and change the state
		setAckTime(&ackTime);
		setAckID(ackID);
		setAckComment(ackComment);

		changeState((WORD)(getObjectState() | OPC_CONDITION_ACKED));
	}

	return res;
} // handleAcknowledge

SOCmnString SOXpSCondition::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSCondition::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }


//-----------------------------------------------------------------------------
// createEvent
//
// - create and initalize a event
//
// returns:
//		pointer to the event object
//
SOAeSEvent *SOXpSCondition::createEvent(
	IN WORD changeMask,				// event change mask
	OPTIONAL IN LPFILETIME occTime)	// occurence time
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOCmnPointer<SOAeSEvent> event = creator->createConditionEvent(this);
	SOCmnPointer<SOAeSCategory> cat;
	FILETIME latestSavedTime;
	FILETIME activeTime;
	WORD state = getObjectState();
	WORD newState = 0;
	SOCmnString sAckID;
	BOOL ackReq;

	if (occTime == NULL)
	{	// calculate the time of the saved times - for refresh
		FILETIME tmpTime;
		latestSavedTime.dwHighDateTime = 0;
		latestSavedTime.dwLowDateTime = 0;

		if (getActiveTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
					(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
				latestSavedTime = tmpTime;
		if (getInactiveTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
					(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
				latestSavedTime = tmpTime;
		if (getAckTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
					(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
				latestSavedTime = tmpTime;
		if (getActSubConditionTime(&tmpTime))
			if ((latestSavedTime.dwHighDateTime < tmpTime.dwHighDateTime) &&
					(latestSavedTime.dwLowDateTime < tmpTime.dwLowDateTime))
				latestSavedTime = tmpTime;
		occTime = &latestSavedTime;
	}
	event->setOccurrenceTime(occTime);
	event->setCookie(getCookie());

	event->setSourceName(getSourceName());

	// ASSERT: before firing an event the condition class must be set!
	SOCmnPointer<SOAeSConditionClass> conditionClass = getConditionClass();
	_ASSERTION((conditionClass.isNotNull()), "Before firing an event the condition class must be set!");
	cat = (SOAeSCategory *)conditionClass->getParentBranch();
	event->setCategory((SOAeSCategory *)cat);
	event->setConditionName(conditionClass->getName());

	// subcondition attributes
	ackReq = getAckRequired(changeMask);
	if (ackReq)
	{
		WORD state = getObjectState();
		if ((state & OPC_CONDITION_ACKED) != 0)
		{
			state &= ~OPC_CONDITION_ACKED;
			setObjectState((BYTE)state);
			changeMask |= OPC_CHANGE_ACK_STATE;
		}
	}
	event->setAckRequired(ackReq);
	event->setSubConditionName(getSubConditionName());
	event->setMessage(getMessage());
	event->setSeverity(getSeverity());
	getActSubConditionTime(&activeTime);
	event->setActiveTime(&activeTime);

	event->setNewState(getObjectState());
	event->setQuality(getQuality());
	sAckID = getAckID();

	if (g_condRelEventsSendsActorID)
	{
		event->setActorID(sAckID);
		if (getObjectState() & OPC_CONDITION_ACKED)
		{
			if(!sAckID.isEmpty() && (sAckID.find(_T("NULL")) < 0) && (sAckID.find(_T("null")) < 0))
				changeMask |= OPC_CHANGE_ACK_STATE;
		}
	}
	else
	{
		if (getObjectState() & OPC_CONDITION_ACKED)
		{
			event->setActorID(sAckID);
			if(!sAckID.isEmpty() && (sAckID.find(_T("NULL")) < 0) && (sAckID.find(_T("null")) < 0))
				changeMask |= OPC_CHANGE_ACK_STATE;

		}
	}
/*
	if (getObjectState() & OPC_CONDITION_ACKED)
	{
		changeMask |= OPC_CHANGE_ACK_STATE;
	}
*/
	event->setChangeMask(changeMask);

	copyAttributeValueList(event->getAttributeValueList());

	event.addRef();
	return (SOAeSEvent *)event;
} // createEvent


SOXpSArea::SOXpSArea(IN LPCTSTR name)
{
	setName(name);
	m_deviceState = SOWATCH_DEVSTATE_OK;
}
SOCmnString SOXpSArea::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSArea::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }



SOXpSSource::SOXpSSource(IN LPCTSTR name)
{
	setName(name);
	m_deviceState = SOWATCH_DEVSTATE_OK;
}
SOCmnString SOXpSSource::getDeviceState(void)
{ return m_deviceState; }
SOCmnString SOXpSSource::getDeviceStateInfo(void)
{ return m_deviceStateInfo; }
