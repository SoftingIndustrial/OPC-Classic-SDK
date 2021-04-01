#ifndef _MYREQUEST_H_
#define _MYREQUEST_H_

#include "Da/ServerDaRequest.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyRequest
//-----------------------------------------------------------------------------
class MyRequest : public DaRequest
{
public:
	MyRequest(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* anElement,
		int aPropertyId,
		unsigned long aRequestHandle):
		DaRequest(
			aTransactionType,
			aSessionHandle,
			anElement,
			aPropertyId,
			aRequestHandle)
	{}

	virtual ~MyRequest(void)
	{}

};  //  end class MyRequest

#endif
