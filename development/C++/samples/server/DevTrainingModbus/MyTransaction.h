#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"
#include "MyDaAddressSpaceElement.h"

#include "OpcServer.h"

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

	virtual long handleReadRequests(void)
	{
		getOpcServer()->addRequests(m_requestList);
		return EnumResultCode_S_OK;
	}   //  end HandleReadRequests

	virtual long handleWriteRequests(void)
	{
		getOpcServer()->addRequests(m_requestList);
		return EnumResultCode_S_OK;
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_
