#ifndef _CLIENTDACREATOR_H_
#define _CLIENTDACREATOR_H_

#include "OTClient.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ClientDACreator
//-----------------------------------------------------------------------------

class ClientDACreator : public SODaCCreator
{
public:
	virtual SODaCServer* createServer(IN BYTE protocolType, IN SODaCEntry* parent);
	virtual SODaCGroup* createGroup(IN BYTE protocolType, IN SODaCServer* parent);
	virtual SODaCItem* createItem(IN SODaCGroup* parent);
}; // ClientDACreator

#pragma pack(pop)
#endif
