#ifndef _SERVERDAPROPERTY_H_
#define _SERVERDAPROPERTY_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDAProperty
//-----------------------------------------------------------------------------

class ServerDAProperty : public SODaSProperty
{
public:
	ServerDAProperty(DWORD propertyID);

	virtual SOCmnString getItemID(void);
}; // ServerDAProperty

#pragma pack(pop)
#endif
