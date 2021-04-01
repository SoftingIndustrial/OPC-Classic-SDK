#ifndef _CLIENTAECREATOR_H_
#define _CLIENTAECREATOR_H_

#include "OTClient.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ClientAECreator
//-----------------------------------------------------------------------------

class ClientAECreator : public SOAeCCreator
{
public:
	virtual SOAeCServer* createServer(IN BYTE protocolType, IN SOAeCEntry* parent);
	virtual SOAeCSubscription* createSubscription(IN BYTE protocolType, IN SOAeCServer* parent);
}; // ClientAECreator

#pragma pack(pop)
#endif
