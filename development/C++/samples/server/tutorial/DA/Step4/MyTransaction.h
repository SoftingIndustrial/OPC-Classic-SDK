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
						DateTime now;
						Variant aVariant(::rand());
						ValueQT cacheValue(aVariant, EnumQuality_GOOD, now);
						request->setValue(cacheValue);
						request->setResult(S_OK);
					}
					else
					{
						//  the element's property will handle this request
						element->getPropertyValue(request);
					}   //  end if ... else
				}   //  end if ... else
			}   //  end if
		}   //  end for

		return completeRequests();
	}   //  end HandleReadRequests

};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_
