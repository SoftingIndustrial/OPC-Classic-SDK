#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"
#include "MyDaAddressSpaceElement.h"
#include "MySession.h"

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
	{} // end ctor

	virtual ~MyTransaction()
	{} // end dtor

	virtual long handleReadRequests(void)
	{
		MySession* pSession = (MySession*)getSession();
		std::vector<DaRequest*> requests = getRequests();
		DaRequest *request = NULL;

		for (size_t i = 0; i < requests.size(); i++)
		{
			if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				ValueQT value;
				request->getAddressSpaceElement()->queryCacheValue(pSession, value);
				request->setValue(value);
				request->setResult(EnumResultCode_S_OK);
			} // end if
		} // end for

		return completeRequests();
	}   //  end HandleReadRequests

	virtual long handleWriteRequests(void)
	{
		std::vector<DaRequest*> requests = getRequests();
		size_t count = requests.size();
		MySession* pClient = (MySession*)getSession();
		DaRequest *request = NULL;

		for (size_t i = 0 ; i < count; i++)
		{
			if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				if (request->getPropertyId() == 0)
				{
					if (pClient != MySession::s_controlingSession)
					{
						request->setResult(EnumResultCode_E_ACCESSDENIED);
						request->complete();
					}
					else
					{
						ValueQT* value = request->getValue();
						request->getAddressSpaceElement()->valueChanged(*value);
						request->setResult(EnumResultCode_S_OK);
						request->complete();
					}
				} // end if
			} // end if
		} // end for

		return completeRequests();
	} // end handleWriteRequests
};  // end class MyTransaction

#endif  // _MYTRANSACTION_H_
