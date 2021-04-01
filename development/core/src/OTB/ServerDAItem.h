#ifndef _SERVERDAITEM_H_
#define _SERVERDAITEM_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDAItem
//-----------------------------------------------------------------------------

class ServerDAItem :
	public SODaSItemDefault
{
	LONG addRef()
	{
		return SODaSItemDefault::addRef();
	}   //  end addRef

protected:
	// cyclic request
	virtual SODaSRequest* getCyclicRequest(void);
	virtual void setCyclicRequest(IN SODaSRequest* pReq);

	SOCmnPointer<SODaSRequest> m_cycReq;
}; // ServerDAItem

#pragma pack(pop)
#endif
