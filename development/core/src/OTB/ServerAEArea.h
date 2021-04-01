#ifndef _SERVERAEAREA_H_
#define _SERVERAEAREA_H_

#include "OTServer.h"
#include "ServerAEAddressSpaceElement.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// ServerAEArea
//-----------------------------------------------------------------------------

class ServerAEArea :
	public SOAeSArea,
	virtual public SOCmnElementListLeafBranch,
public ServerAEAddressSpaceElement
{
public:
	ServerAEArea(void) {}

	virtual BOOL isBrowsable(IN SOCmnObject* pServer)
	{
		return isBrowsableI(pServer);
	}

protected:
	~ServerAEArea(void)
	{
		destroy(this);
	}
}; // ServerAEArea

#pragma pack(pop)
#endif
