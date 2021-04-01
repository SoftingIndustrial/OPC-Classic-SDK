#ifndef _SERVERAECREATOR_H_
#define _SERVERAECREATOR_H_

#ifdef OTB_SERVER_AE

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAECreator
//-----------------------------------------------------------------------------

class ServerAECreator : public SOAeSCreator
{
	virtual SOAeSServer* createServer(IN BYTE serverInstance);
}; // ServerAECreator

#pragma pack(pop)

#endif // OTB_SERVER_AE

#endif
