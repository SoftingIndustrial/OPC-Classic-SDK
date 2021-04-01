#include "stdafx.h"
#include "IOManager.h"

using namespace SoftingOPCToolboxServer;

long IOManager::handleReadRequests(MyTransaction& aTransaction)
{
	std::vector<DaRequest*> requestList = aTransaction.getRequests();
	size_t count = requestList.size();

	for (size_t i = 0; i < count; i++)
	{
		MyDaAddressSpaceElement* element =
			(MyDaAddressSpaceElement*)requestList[i]->getAddressSpaceElement();

		if (element == NULL)
		{
			requestList[i]->setResult(E_FAIL);
		}
		else
		{
			if (requestList[i]->getPropertyId() == 0)
			{
				// get address space element value take the toolkit cache value
				ValueQT cacheValue;
				element->getCacheValue(cacheValue);
				requestList[i]->setValue(cacheValue);
				requestList[i]->setResult(S_OK);
			}
			else
			{
				//  the element's property will handle this request
				element->getPropertyValue(requestList[i]);
			}   //  end if ... else
		}   //  end if ... else
	}   //  end for

	return aTransaction.completeRequests();
}   //  end HandleReadRequests

long IOManager::handleWriteRequests(MyTransaction& aTransaction)
{
	std::vector<DaRequest*> requestList = aTransaction.getRequests();
	size_t count = requestList.size();

	for (size_t i = 0; i < count; i++)
	{
		DaRequest* pRequest = requestList[i];

		if (pRequest != NULL)
		{
			MyDaAddressSpaceElement* pElement =
				(MyDaAddressSpaceElement*)pRequest->getAddressSpaceElement();

			if (pElement != NULL)
			{
				ValueQT* pValue = pRequest->getValue();
				pRequest->setResult(pElement->valueChanged(*pValue));
			} // end if
		}   // end if
	}   //  end  for

	return aTransaction.completeRequests();
}   //  end handleWriteRequests


void IOManager::cyclicStateChanged(std::vector<DaAddressSpaceElement*>& anElementList)
{
}

