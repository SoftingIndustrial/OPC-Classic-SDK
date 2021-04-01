#ifndef _SERVERDATRANSACTION_H_
#define _SERVERDATRANSACTION_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDATransaction
//-----------------------------------------------------------------------------

class ServerDATransaction : public SODaSTransaction
{
public:
	ServerDATransaction(IN SODaSGroup* pGroup, IN BYTE type,
						IN BYTE connectionType, IN DWORD connection);

	// handle the I/O requests
	virtual void handleRequests(IN SOCmnList<SODaSRequest> *pReqList);

	// notify the server application that the request was internally completed
	virtual void notiofyCompletedInternally(IN SODaSRequest* pRequest);

protected:
	OTSRequestData* m_paRequestData;
	DWORD m_sizeRequestData;

	virtual ~ServerDATransaction(void);
}; // ServerDATransaction

#pragma pack(pop)
#endif
