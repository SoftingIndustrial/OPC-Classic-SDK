#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "ServerDAItem.h"

//-----------------------------------------------------------------------------
// ServerDAItem
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getCyclicRequest
//
// - get the cyclic request of the item
//
// returns:
//		the cyclic request of the item
//
SODaSRequest* ServerDAItem::getCyclicRequest(void)
{
	m_cycReq.addRef();
	return (SODaSRequest*)m_cycReq;
} // getCyclicRequest


//-----------------------------------------------------------------------------
// setCyclicRequest
//
// - set the cyclic request of the item
//
void ServerDAItem::setCyclicRequest(
	IN SODaSRequest* pReq) // cyclic request
{
	m_cycReq = pReq;
	m_cycReq.addRef();
} // setCyclicRequest

#endif // OTB_SERVER_DA
