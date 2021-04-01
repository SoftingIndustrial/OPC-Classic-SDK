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
//  Filename    : ClientDASEssion.cpp                                         |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Session class                                           |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT_DA

#include "ClientDASession.h"
#include "ClientDASubscription.h"
#include "ClientJob.h"
#include "OTC.h"

ClientDASession::ClientDASession(void)
{
	m_userData = 0;
	m_storedTargedState = disconnected;
}

void ClientDASession::storeTargetState(IN BYTE deep)
{
	m_storedTargedState = getTargetObjectState();

	if (deep)
	{
		ClientDASubscription* pGroup;
		SOCmnList<ClientDASubscription> groupList((SOCmnList<ClientDASubscription> *)getBranchList());
		SOCmnListPosition pos;
		pos = groupList.getStartPosition();

		while (pos)
		{
			pGroup = groupList.getNext(pos);
			pGroup->storeTargetState(TRUE);
		}
	}
}

void ClientDASession::restoreTargetState(IN BYTE deep)
{
	setTargetObjectState(m_storedTargedState);

	if (deep)
	{
		ClientDASubscription* pGroup;
		SOCmnList<ClientDASubscription> groupList((SOCmnList<ClientDASubscription> *)getBranchList());
		SOCmnListPosition pos;
		pos = groupList.getStartPosition();

		while (pos)
		{
			pGroup = groupList.getNext(pos);
			pGroup->restoreTargetState(TRUE);
		}
	}
}


void ClientDASession::onSetObjectState(void)
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
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASession::onSetObjectState - Exception occurred in vendor application callback: OTCOnStateChange")));
		}
	}
}


//-----------------------------------------------------------------------------
// shutdown
//
// - shutdown all connections to the server
//
void ClientDASession::shutdown(
	IN LPCTSTR reason)  // shutdown reason
{
	storeTargetState(TRUE); // save target object state over disconnect
	disconnect();
	BOOL cbRet = TRUE;

	if (g_callbackFunctions.m_OTCOnServerShutdown)
	{
		OTCObjectData objectData;
		objectData.m_objectHandle = (OTCObjectHandle)this->getOpcHandle();
		objectData.m_userData = getOTCUserData();

		SOCMN_TRY
		{
			cbRet = g_callbackFunctions.m_OTCOnServerShutdown(objectData, (LPTSTR)reason);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDASession::shutdown - Exception occurred in vendor application callback: OTCOnServerShutdown")));
		}
		
	}

	if (cbRet)
	{
		// reconnect on next connection monitor loop
		restoreTargetState(TRUE);
	}
}

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// ClientDASessionCom                                                         |
//-----------------------------------------------------------------------------

ClientDASessionCom::ClientDASessionCom(void)
{
}

SOCltComServerImpl* ClientDASessionCom::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// ClientDASessionXml                                                         |
//-----------------------------------------------------------------------------

ClientDASessionXml::ClientDASessionXml(void)
{
}

#endif // SOFEATURE_SOAP

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// ClientDASessionTp                                                          |
//-----------------------------------------------------------------------------

ClientDASessionTp::ClientDASessionTp(void)
{
}

ClientDASessionTp::~ClientDASessionTp(void)
{
}

#endif // SOFEATURE_TUNNEL

#endif // OTB_CLIENT_DA

