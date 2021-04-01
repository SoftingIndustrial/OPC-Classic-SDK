#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"
#include "Mutex.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// MyTransaction
//-----------------------------------------------------------------------------
class MyTransaction : public DaTransaction
{
	static Mutex m_syncTransaction;

public:

	MyTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey) : DaTransaction(aTransactionType, aRequestList, aSessionKey)
	{
	}   //  enc ctr

	long handleReadRequests(void)
	{
		m_syncTransaction.lock();
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
						// get address space element value take the Toolkit cache value
						ValueQT aValue;
						element->getCacheValue(aValue);
						request->setValue(aValue);
						request->setResult(S_OK);
					}
					else
					{
						//  the element will handle this request
						element->getPropertyValue(*request);
					}   //  end if ... else
				}   //  end if ... else
			}   // end if
		}   //  end for

		long result = completeRequests();
		m_syncTransaction.unlock();
		return result;
	}   //  end handleReadRequests


	long handleWriteRequests(void)
	{
		m_syncTransaction.lock();
		size_t count = m_requestList.size();
		DaRequest *request = NULL;

		for (size_t i = 0; i < count; i++)
		{
			request = m_requestList[i];

			if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				MyDaAddressSpaceElement* element = (MyDaAddressSpaceElement*)request->getAddressSpaceElement();

				if (element != NULL)
				{
					ValueQT* pValue = request->getValue();
					request->setResult(element->valueChanged(*pValue));
				} // end if
			} // end if
		} // end for

		long result = completeRequests();
		m_syncTransaction.unlock();
		return result;
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif
