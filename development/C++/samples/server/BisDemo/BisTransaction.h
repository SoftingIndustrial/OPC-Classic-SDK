#ifndef _BISTRANSACTION_H_
#define _BISTRANSACTION_H_

#include "Da/ServerDaTransaction.h"


using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// BisTransaction
//-----------------------------------------------------------------------------
class BisTransaction : public DaTransaction
{
public:
	BisTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey) :
		DaTransaction(aTransactionType, aRequestList, aSessionKey)
	{
	} // end ctr

	virtual ~BisTransaction(
		void)
	{
	} // end dtor

	virtual long handleReadRequests(
		void)
	{
		size_t count = m_requestList.size();
		DaRequest *pRequest = NULL;

		for (size_t i = 0; i < count; i++)
		{
			pRequest = m_requestList[i];

			if ((pRequest != NULL) && (pRequest->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				BaseDaElement* element = dynamic_cast<BaseDaElement*>(pRequest->getAddressSpaceElement());

				if (element == NULL)
				{
					pRequest->setResult(E_FAIL);
					pRequest->complete();
				}
				else
				{
					element->handleReadRequest(pRequest);
				} // end if .. else
			} // end if
		} // end while

		return (long)EnumResultCode_S_OK;
	} // end HandleReadRequests

	virtual long handleWriteRequests(
		void)
	{
		size_t count = m_requestList.size();
		DaRequest *pRequest = NULL;

		for (size_t i = 0; i < count; i++)
		{
			pRequest = m_requestList[i];

			if ((pRequest != NULL) && (pRequest->progressRequestState(EnumRequestState_CREATED, EnumRequestState_INPROGRESS) == true))
			{
				BaseDaElement* element = dynamic_cast<BaseDaElement*>(pRequest->getAddressSpaceElement());

				if (element == NULL)
				{
					pRequest->setResult(E_FAIL);
					pRequest->complete();
				}
				else
				{
					element->handleWriteRequest(pRequest);
				} // end if .. else
			} // end if
		} // end while

		return (long)EnumResultCode_S_OK;
	} // end HandleWriteRequests

}; // end BisTransaction

#endif
