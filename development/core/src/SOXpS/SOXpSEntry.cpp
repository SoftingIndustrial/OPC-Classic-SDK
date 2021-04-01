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
//  Filename    : SOXpSEntry.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Entry object                                                |
//                - SOXpSEntry: Entry point class                             |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <comcat.h>
#include "SOXpS.h"
#include "SOXpSEntry.h"
#include "SOXpSAEObjects.h"
#include "SOXpSDAObjects.h"
#include "SOXpSCallbacks.h"
#include "SOXpSDAObjects.h"
#include "SODaSComServer.h"
#include "SOAeSComServer.h"
#include "SOVersion.h"

#ifdef SOFTING
#include "Softing/SoftingOPCTB.h"
#endif


// can close application during shutdown event
extern HANDLE g_canCloseEvent;

//-----------------------------------------------------------------------------
// SOXpSEventHandler                                                          |
//-----------------------------------------------------------------------------

SOXpSEventHandler::SOXpSEventHandler(
	IN BOOL startedByCom,
	IN HWND wnd)
 : SOSrvEventHandler()
{
	m_startedByCom = startedByCom;
	m_wnd = wnd;
}

//-----------------------------------------------------------------------------
//  onReleasedLastOPCObject
//
// - handle release of last COM object
//
void SOXpSEventHandler::onReleasedLastOPCObject(void)
{
	if (g_canCloseEvent)
	{
		SetEvent(g_canCloseEvent);
	}

#ifndef SOXPS_SERVICE
	if (m_startedByCom)
	{ // started via COM
		SOCmnPointer<SOAsSEntry> entry = ::getSOAsSEntry();
		// -> try to close application if last object was released
		if (S_OK == entry->canUnloadNow())
		{
			entry->revokeClassObjects();
			if (m_wnd)
			{
				PostMessage(m_wnd, WM_CLOSE, 0, 0);
			}
		}
	}
#endif
} // onReleasedLastOPCObject




//-----------------------------------------------------------------------------
// SOXpSAECreator                                                             |
//-----------------------------------------------------------------------------

SOAeSServer *SOXpSAECreator::createServer(
	IN BYTE serverInstance)
{
	SOXpSAEServer *server;
	SOCMN_ALLOCATE_OBJECT(server, SOXpSAEServer())
	return server;
}

SOAeSSubscription *SOXpSAECreator::createSubscription(IN SOAeSServer *server)
{
	SOXpSSubscription *sub;
	SOCMN_ALLOCATE_OBJECT(sub, SOXpSSubscription())
	return sub;
}



//-----------------------------------------------------------------------------
// SOXpSDACreator                                                             |
//-----------------------------------------------------------------------------

SODaSServer *SOXpSDACreator::createServer(
	IN BOOL publicGroups,
	IN BYTE serverInstance)
{
	SOXpSDAServer *server;
	SOCMN_ALLOCATE_OBJECT(server, SOXpSDAServer())
	return server;
}

//-----------------------------------------------------------------------------
//  createTransaction
//
// - create transaction object
//
// returns:
//		created object
//
SODaSTransaction *SOXpSDACreator::createTransaction(
	IN SODaSGroup *group,    // group object
	IN BYTE transactionType, // type of transaction
	IN BYTE connectionType,  // connection type
	IN DWORD connection)     // connection id
{
	SODaSTransaction *tac;
	SOCMN_ALLOCATE_OBJECT(tac, SOXpSTransaction(group, transactionType, connectionType, connection))
	return tac;
} // createTransaction


//-----------------------------------------------------------------------------
// createRequest
//
// - create request object
//
// returns:
//		created object
//
SODaSRequest* SOXpSDACreator::createRequest(
	IN SODaSServer *server,	// server object
	IN SODaSItem *item,		// item object (can be NULL)
	IN SODaSItemTag *tag,	// tag object
	IN BYTE operation,		// request operation (read or write)
	IN WORD type)			// request type
{
	SODaSRequest* req;
	SOCMN_ALLOCATE_OBJECT(req, SOXpSDARequest(operation, type))
	return req;
} // createRequest

//-----------------------------------------------------------------------------
// createItem
//
// - create item object
//
// returns:
//		created object
//
SODaSItem *SOXpSDACreator::createItem(
	IN SODaSGroup *parent,	// group of item
	IN SODaSItemTag *tag)	// item tag of item
{
	SODaSItem *item;
	SOCMN_ALLOCATE_OBJECT(item, SOXpSItem())
	return item;
} // createItem
