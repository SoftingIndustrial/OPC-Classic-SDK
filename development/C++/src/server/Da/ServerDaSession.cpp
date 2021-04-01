#include "OSCompat.h"
#include "ServerDaSession.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	constructor
//
DaSession::DaSession(
	EnumSessionType aType,
	unsigned long aHandle)
{
	m_type = aType;
	m_objectHandle = aHandle;
	m_clientName = _T("");
}   //  end constructor


//-----------------------------------------------------------------------------
//	destructor
//
DaSession::~DaSession()
{
}   //  end descructor


//-----------------------------------------------------------------------------
// Notify that this session is to be destroyed
//
void DaSession::connectSession(void)
{
	handleConnected();
}   //  end connectSession


//-----------------------------------------------------------------------------
// Notify that this client was created
//
void DaSession::disconnectSession(void)
{
	handleDisconnected();
}   //  end disconnectSession


//-----------------------------------------------------------------------------
// Notify that this client tried to log on
//
long DaSession::logon(tstring& aUserName, tstring& aPassword)
{
	return E_NOTIMPL;
}   //  end logon


//-----------------------------------------------------------------------------
// Notify that this client logged off
//
long DaSession::logoff(void)
{
	return E_NOTIMPL;
}   //  end logoff
