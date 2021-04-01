#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "Da/ServerDaRequest.h"

using namespace SoftingOPCToolboxServer;

class Request : public DaRequest
{

public:

	Request(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* pAnElement,
		int aPropertyId,
		unsigned long aRequestHandle);

	virtual ~Request(void);

};  //  end class Request

#endif
