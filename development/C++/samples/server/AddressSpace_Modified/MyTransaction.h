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
		m_requestListJanitor.lock();
		size_t count = m_requestList.size();
		int rnd = rand() * 33;
		DateTime dt;
		dt.now();

		for (size_t i = 0; i < count; i++)
		{
			MyDaAddressSpaceElement* element = (MyDaAddressSpaceElement*)m_requestList[i]->getAddressSpaceElement();

			if (element == NULL)
			{
				m_requestList[i]->setResult(E_FAIL);
			}
			else
			{
				if (m_requestList[i]->getPropertyId() == 0)
				{
					// get address space element value take the Toolkit cache value
					Variant aData;
					ValueQT aValue;
					if (element->getIoMode() == EnumIoMode_POLL_OWNCACHE)
					{
						aData = element->getCachedValue().getData();
					}
					else
					{
						aData = element->getRandomValue(rnd);
					}
					aValue = ValueQT(aData, (EnumQuality)g_qualities[0], dt);
					element->setCachedValue(aValue);
					_tprintf(_T("%s element \"%s\", newValue: %s\n"), dt.toString().c_str(), element->getName().c_str(), aData.toString().c_str());

					m_requestList[i]->setValue(aValue);
					m_requestList[i]->setResult(EnumResultCode_S_OK);
				}
			}
		}
		m_requestListJanitor.unlock();
		return completeRequests();
	}   //  end HandleReadRequests


	virtual long handleWriteRequests(void)
	{
		m_requestListJanitor.lock();
		size_t count = m_requestList.size();
		int rnd = rand() * 33;
		DateTime dt;
		dt.now();

		for (size_t i = 0; i < count; i++)
		{
			MyDaAddressSpaceElement* element = (MyDaAddressSpaceElement*)m_requestList[i]->getAddressSpaceElement();

			if (element == NULL)
			{
				m_requestList[i]->setResult(E_FAIL);
			}
			else
			{
				if (m_requestList[i]->getPropertyId() == 0)
				{
					// get address space element value take the Toolkit cache value
					element->setCachedValue(*(m_requestList[i]->getValue()));
					element->valueChanged(*(m_requestList[i]->getValue()));
					m_requestList[i]->setResult(EnumResultCode_S_OK);
				}
			}
		}
		m_requestListJanitor.unlock();
		return completeRequests();
	}   //  end handleWriteRequests

};  //  end class MyTransaction

#endif
