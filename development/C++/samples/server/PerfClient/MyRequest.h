#ifndef _MYREQUEST_H_
#define _MYREQUEST_H_

#include "Da/ServerDaRequest.h"

using namespace SoftingOPCToolboxServer;

class MyRequest : public DaRequest
{

public:
	MyRequest(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* anElement,
		int aPropertyId,
		unsigned long aRequestHandle):
		DaRequest(aTransactionType,
				  aSessionHandle,
				  anElement,
				  aPropertyId,
				  aRequestHandle)
	{} // end ctor

	virtual ~MyRequest(void)
	{} // end dtor

};  // end class MyRequest

#endif
