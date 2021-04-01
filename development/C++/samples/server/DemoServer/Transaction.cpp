#include "stdafx.h"
#include "Transaction.h"
#include "Function.h"

Transaction::Transaction(
	EnumTransactionType aTransactionType,
	std::vector<DaRequest*>& aRequestList,
	unsigned long aSessionKey) :
	DaTransaction(
		aTransactionType,
		aRequestList,
		aSessionKey)
{
}   //  end constructor

Transaction::~Transaction()
{
}   //  end destructor

long Transaction::handleReadRequests(void)
{
	size_t count = m_requestList.size();
	DaRequest *request = NULL;

	for (size_t i = 0; i < count; i++)
	{
		request = m_requestList[i];

		if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
		{
			DemoDaAddressSpaceElement* pElement = (DemoDaAddressSpaceElement*)request->getAddressSpaceElement();

			if (pElement == NULL)
			{
				request->setResult(E_FAIL);
			}
			else
			{
				pElement->handleReadRequest(request);
			}   //  end if ... else
		}   //  end if
	}   //  end for

	return completeRequests();
}   //  end HandleReadRequests

long Transaction::handleWriteRequests(void)
{
	size_t count = m_requestList.size();
	DaRequest *request = NULL;

	for (size_t i = 0; i < count; i++)
	{
		request = m_requestList[i];

		if ((request != NULL) && (request->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
		{
			DemoDaAddressSpaceElement* pElement = (DemoDaAddressSpaceElement*)request->getAddressSpaceElement();

			if (pElement == NULL)
			{
				m_requestList[i]->setResult(E_FAIL);
			}
			else
			{
				pElement->handleWriteRequest(m_requestList[i]);
			}   //  end if ... else
		}   //  end if
	}   //  end  for

	return completeRequests();
}   //  end handleWriteRequests
