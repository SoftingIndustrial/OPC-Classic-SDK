#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

class MyTransaction : public DaTransaction
{

public:

	MyTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey) :
		DaTransaction(
			aTransactionType,
			aRequestList,
			aSessionKey)
	{}  //  end ctor

	virtual ~MyTransaction()
	{}  //  end dtor

	virtual long handleReadRequests(void);
	virtual long handleWriteRequests(void);

};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_
