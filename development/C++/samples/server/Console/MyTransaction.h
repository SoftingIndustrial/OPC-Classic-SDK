#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"

#include <math.h>

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
						// get address space element value take the toolkit cache value
						ValueQT cacheValue;
						element->getCacheValue(cacheValue);
						request->setValue(cacheValue);
						request->setResult(S_OK);
					}
					else
					{
						//  the element will handle this request
						element->getPropertyValue(request);
					}   //  end if ... else
				}   //  end if ... else
			}   //  end if
		}   //  end for

		return completeRequests();
	}   //  end HandleReadRequests


	virtual long handleWriteRequests(void)
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
				else if (element->getType() == MyDaAddressSpaceElement::TYPE_ACCEPT)
				{
					// accept the written value
					ValueQT* writeValue = request->getValue();

					if (request->getValue() != NULL)
					{
						long result = element->valueChanged(*writeValue);
						request->setResult(result);
					}
				}
				else if ((request->getPropertyId() == 6020) &&
					(element->getType() == MyDaAddressSpaceElement::TYPE_SINE))
				{
					// change the angle of the sin
					ValueQT* writeValue = request->getValue();

					if (writeValue != NULL && writeValue->getData().iVal < 359)
					{
						g_angle = writeValue->getData().iVal;
						double pi = 3.1415926535;
						double radianAngle = (2 * pi) * ((double)g_angle / 360.0);
						Variant var;
						var.setR8(sin(radianAngle));
						DateTime now;
						now.now();
						ValueQT elementValue(var, EnumQuality_GOOD, now);
						long result = element->valueChanged(elementValue);
						request->setResult(result);
					}
					else
					{
						request->setResult(EnumResultCode_E_OPC_RANGE);
					}   //  end if ... else
				}
				else
				{
					// unknown write item - should never get here !
					request->setResult(E_FAIL);
				}   //  end if ... else
			}   //  end if
		}   //  end  for

		return completeRequests();
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif
