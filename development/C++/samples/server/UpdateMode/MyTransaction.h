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
						now.now();

						if (element->getType() == MyDaAddressSpaceElement::TYPE_REPORT_SEC)
						{
							// get address space element value
							Variant aVariant(now.second());
							ValueQT cacheValue(aVariant, EnumQuality_GOOD, now);
							request->setValue(cacheValue);
							request->setResult(S_OK);
						}
						else if (element->getType() == MyDaAddressSpaceElement::TYPE_POLL_SEC)
						{
							// get address space element value
							Variant aVariant(now.second());
							ValueQT cacheValue(aVariant, EnumQuality_GOOD, now);
							request->setValue(cacheValue);
							request->setResult(S_OK);
						}
						else if (element->getType() == MyDaAddressSpaceElement::TYPE_REPORT_MIN)
						{
							//  this is a device read
							// get address space element value take the toolkit cache value
							Variant aVariant(now.minute());
							ValueQT cacheValue(aVariant, EnumQuality_GOOD, now);
							request->setValue(cacheValue);
							request->setResult(S_OK);
						}
						else if (element->getType() == MyDaAddressSpaceElement::TYPE_POLL_MIN)
						{
							// get address space element value take the toolkit cache value
							Variant aVariant(now.minute());
							ValueQT cacheValue(aVariant, EnumQuality_GOOD, now);
							request->setValue(cacheValue);
							request->setResult(S_OK);
						}
						else
						{
							// get address space element value take the toolkit cache value
							ValueQT cacheValue;
							element->getCacheValue(cacheValue);
							request->setValue(cacheValue);
							request->setResult(S_OK);
						}   //  end if ... else
					}
					else
					{
						//  the element will handle this request
						element->getPropertyValue(request);
					}   //  end if ... else
				}   //  end if ... else
			}   // end if
		}   //  end for

		return completeRequests();
	}   //  end HandleReadRequests

};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_
