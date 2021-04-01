#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"

#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// MyTransaction
//-----------------------------------------------------------------------------
class MyTransaction : public DaTransaction
{
public:

	MyTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey) : DaTransaction(aTransactionType, aRequestList, aSessionKey)
	{
	}   //  enc ctr

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
						// get address space element value take the Toolkit cache value
						ValueQT aValue;
						element->getCacheValue(aValue);
						request->setValue(aValue);
						request->setResult(S_OK);
					}
					else
					{
						//  for all properties return the element type
						DateTime now;
						Variant aVariant(element->getType());
						ValueQT aValue(aVariant, EnumQuality_GOOD, now);
						request->setValue(aValue);
						request->setResult(S_OK);
					}   //  end if ... else
				}   //  end if ... else
			}   //  end if
		}   //  end for

		return completeRequests();
	}   //  end HandleReadRequests


	virtual long handleWriteRequests(void)
	{
		long result = valuesChanged();

		if (FAILED(result))
		{
			return result;
		} // end if

		return completeRequests();
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif
