#include "OSCompat.h"
#include "ServerDaRequest.h"
#include "ServerDaAddressSpaceElement.h"
#include "ServerDaTransaction.h"
#include "../ServerApplication.h"

#include "OTServer.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	Constructor
//
DaRequest::DaRequest(
	EnumTransactionType aTransactionType,
	unsigned long aSessionHandle,
	DaAddressSpaceElement* anElement,
	int aPropertyId,
	unsigned long aRequestHandle):
	m_sessionHandle(aSessionHandle),
	m_requestHandle(aRequestHandle),
	m_transactionKey(0),
	m_transactionType(aTransactionType),
	m_propertyId(aPropertyId),
	m_addressSpaceElement(anElement),
	m_result(E_NOTIMPL),
	m_requestState(EnumRequestState_CREATED)
{
	if (m_addressSpaceElement)
	{
		OTSAckRequestElement(m_addressSpaceElement->getObjectHandle());
	}
}   //  end constructor


//-----------------------------------------------------------------------------
//	Destructor
//
DaRequest::~DaRequest()
{
	if (m_addressSpaceElement)
	{
		OTSNackRequestElement(m_addressSpaceElement->getObjectHandle());
	}
	if (m_requestState != EnumRequestState_COMPLETED)
	{
		//  this is not supposed to ever happen
		complete();
	}   //  end if
}   //  end destructor


//-----------------------------------------------------------------------------
// Complete this request
//
long DaRequest::complete(void)
{
	long result = S_FALSE;
	DaTransaction* transaction = Application::Instance()->findTransaction(m_transactionKey);

	if (transaction != NULL)
	{
		result = transaction->completeRequest(this);
	}   //  end if

	return result;
}   //  end complete

//-----------------------------------------------------------------------------
// The request was interanlly completed inside Toolkit
//
void DaRequest::completedInternally(void)
{
	progressRequestState(EnumRequestState_PENDING, EnumRequestState_COMPLETED);
	progressRequestState(EnumRequestState_CREATED, EnumRequestState_COMPLETED);
}   // end completedInternally