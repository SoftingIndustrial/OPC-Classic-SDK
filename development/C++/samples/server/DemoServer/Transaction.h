#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "DaAddressSpaceElement.h"
#include "DaAddressSpaceRoot.h"
#include "Da/ServerDaTransaction.h"


using namespace SoftingOPCToolboxServer;

class Transaction : public DaTransaction
{

public:

	Transaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey);

	virtual ~Transaction();

	virtual long handleReadRequests(void);

	virtual long handleWriteRequests(void);

};  //  end class Transaction

#endif  //  _TRANSACTION_H_
