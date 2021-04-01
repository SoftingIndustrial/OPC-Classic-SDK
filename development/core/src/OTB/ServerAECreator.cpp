#include "stdafx.h"

#ifdef OTB_SERVER_AE

#include "ServerAECreator.h"
#include "ServerAEServer.h"

//-----------------------------------------------------------------------------
// CreatorAE                                                               |
//-----------------------------------------------------------------------------

SOAeSServer* ServerAECreator::createServer(IN BYTE serverInstance)
{
	ServerAEServer* server;
	SOCMN_ALLOCATE_OBJECT(server, ServerAEServer(serverInstance))
	return server;
}

#endif // OTB_SERVER_AE
