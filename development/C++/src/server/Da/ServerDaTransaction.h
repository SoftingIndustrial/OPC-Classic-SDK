#ifndef _SERVERDATRANSACTION_H_
#define _SERVERDATRANSACTION_H_

#include "../../Enums.h"
#include "../ServerEnums.h"
#include "Mutex.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class DaRequest;
class DaSession;

// Transaction class: Used for managing the transaction requests
class TBC_EXPORT DaTransaction
{

private:
	EnumTransactionType m_type;
	unsigned long m_sessionHandle;
	static unsigned long KeyBuilder;
	static Mutex KeyBuilderJanitor;
	unsigned long m_key;

public:
	DaTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionHandle);

	virtual ~DaTransaction();

protected:
	std::vector<DaRequest*> m_requestList;
	Mutex m_requestListJanitor;

public:

	virtual long handleReadRequests(void)
	{
		return E_NOTIMPL;
	}   //  end handleReadRequests

	virtual long handleWriteRequests(void)
	{
		return E_NOTIMPL;
	}   //  end handleWriteRequests

	//  Determines whether this is a read or a write transaction
	EnumTransactionType getType(void)
	{
		return m_type;
	}

	//  retrieves the key
	unsigned long getKey()
	{
		return m_key;
	}

	// Requests contained in this transaction
	std::vector<DaRequest*>& getRequests(void)
	{
		return m_requestList;
	}

	//  no requests pending
	BOOL isEmpty(void)
	{
		m_requestListJanitor.lock();
		BOOL ret = m_requestList.empty();
		m_requestListJanitor.unlock();
		return ret;
	}

	// Function to get the session from which  the Transaction came from
	DaSession* getSession(void);

	long completeRequests(void);
	long completeRequest(DaRequest* aRequest);
	long findRequestByHandle(unsigned long requestHandle, DaRequest** aRequest);
	long valuesChanged(void);

	void removeRequest(DaRequest* aRequest);

};  //  end class Transaction

}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _DATRANSACTION_H_

