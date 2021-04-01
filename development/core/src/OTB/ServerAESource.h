#ifndef _SERVERAESOURCE_H_
#define _SERVERAESOURCE_H_

#include "OTServer.h"
#include "ServerAEAddressSpaceElement.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAESource
//-----------------------------------------------------------------------------

class ServerAESource :
	public SOAeSSource,
	public ServerAEAddressSpaceElement
{
public:
	ServerAESource(void) {}

	virtual BOOL isBrowsable(IN SOCmnObject* pServer)
	{
		return isBrowsableI(pServer);
	}

protected:
	~ServerAESource(void)
	{
		destroy(this);
	}
}; // ServerAESource

#pragma pack(pop)
#endif
