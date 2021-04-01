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

	virtual long handleReadRequests(void)
	{
		size_t count = m_requestList.size();
		DaRequest *request = NULL;

		for (size_t i = 0; i < count; i++)
		{
			request = m_requestList[i];

			if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				MyDaAddressSpaceElement* element = (MyDaAddressSpaceElement*)request->getAddressSpaceElement();

				if (element == NULL)
				{
					request->setResult(E_FAIL);
				}
				else
				{
					if (request->getPropertyId() == 0)
					{
						// get address space element value take the toolkit cache value
						ValueQT cacheValue;
						element->getCacheValue(cacheValue);
						request->setValue(cacheValue);
						request->setResult(S_OK);
					}
					else
					{
						//  the element's property will handle this request
						element->getPropertyValue(m_requestList[i]);
					}   //  end if ... else
				}   //  end if ... else
			}   // end if
		}   //  end for

		return completeRequests();
	}   //  end HandleReadRequests

	virtual long handleWriteRequests(void)
	{
		size_t count = m_requestList.size();
		DaRequest* pRequest = NULL;

		for (size_t i = 0; i < count; i++)
		{
			pRequest = m_requestList[i];

			if ((pRequest != NULL) && (pRequest->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				MyDaAddressSpaceElement* pElement = (MyDaAddressSpaceElement*)pRequest->getAddressSpaceElement();

				if (pElement != NULL)
				{
					ValueQT* pValue = pRequest->getValue();
					pRequest->setResult(pElement->valueChanged(*pValue));
				} // end if
			}   // end if
		}   //  end  for

		return completeRequests();
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_
