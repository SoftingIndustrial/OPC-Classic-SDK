#include "stdafx.h"

#ifdef OTB_CLIENT_AE

#include "ClientAESession.h"
#include "ClientAESubscription.h"
#include "OTC.h"

ClientAESession::ClientAESession(void)
{
	m_userData = 0;
	m_storedTargedState = disconnected;
}

void ClientAESession::storeTargetState(IN BYTE deep)
{
	m_storedTargedState = getTargetObjectState();

	if (deep)
	{
		ClientAESubscription* pGroup;
		SOCmnList<ClientAESubscription> groupList((SOCmnList<ClientAESubscription> *)getLeafList());
		SOCmnListPosition pos;
		pos = groupList.getStartPosition();

		while (pos)
		{
			pGroup = groupList.getNext(pos);
			pGroup->storeTargetState(TRUE);
		}
	}
}

void ClientAESession::restoreTargetState(IN BYTE deep)
{
	setTargetObjectState(m_storedTargedState);

	if (deep)
	{
		ClientAESubscription* pGroup;
		SOCmnList<ClientAESubscription> groupList((SOCmnList<ClientAESubscription> *)getLeafList());
		SOCmnListPosition pos;
		pos = groupList.getStartPosition();

		while (pos)
		{
			pGroup = groupList.getNext(pos);
			pGroup->restoreTargetState(TRUE);
		}
	}
}


void ClientAESession::onSetObjectState(void)
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
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientAESession::onSetObjectState - Exception occurred in vendor application callback: OTCOnStateChange")));
		}
	}
}


//-----------------------------------------------------------------------------
// shutdown
//
// - shutdown all connections to the server
//
void ClientAESession::shutdown(
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
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientAESession::shutdown - Exception occurred in vendor application callback: OTCOnServerShutdown")));
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
// ClientAESessionCom                                                         |
//-----------------------------------------------------------------------------

ClientAESessionCom::ClientAESessionCom(void)
{
}

SOCltComServerImpl* ClientAESessionCom::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// ClientAESessionTp                                                         |
//---------------------------------------------------------------------- -------

ClientAESessionTp::ClientAESessionTp(void)
{
}

ClientAESessionTp::~ClientAESessionTp(void)
{
}


#endif // SOFEATURE_TUNNEL

#endif // OTB_CLIENT_AE

