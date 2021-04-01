#include "OSCompat.h"
#include "ServerDaTransaction.h"

#include "ServerDaAddressSpaceElement.h"
#include "ServerDaRequest.h"
#include "ServerDaSession.h"

#include "../ServerApplication.h"
#include "OTServer.h"



using namespace SoftingOPCToolboxServer;

unsigned long DaTransaction::KeyBuilder = 1;
Mutex DaTransaction::KeyBuilderJanitor = Mutex();

//-----------------------------------------------------------------------------
// Constructor
//
DaTransaction::DaTransaction(
	EnumTransactionType aTransactionType,
	std::vector<DaRequest*>& aRequestList,
	unsigned long aSessionHandle):
	m_type(aTransactionType),
	m_sessionHandle(aSessionHandle),
	m_requestList(aRequestList)
{
	KeyBuilderJanitor.lock();
	m_key = KeyBuilder++;
	KeyBuilderJanitor.unlock();

	unsigned long count = (unsigned long)aRequestList.size();

	for (unsigned long i = 0; i < count; i++)
	{
		aRequestList[i]->setTransactionKey(m_key);
	}   //  end for
}   //  end constructor


//-----------------------------------------------------------------------------
// Destructor
//
DaTransaction::~DaTransaction()
{
	m_requestListJanitor.lock();
	unsigned long count = (unsigned long)m_requestList.size();
	DaRequest* current = NULL;

	for (unsigned long i = 0; i < count; i++)
	{
		current = m_requestList[i];

		if (current != NULL)
		{
			delete current;
		}   //  end if
	}   //  end for

	m_requestListJanitor.unlock();
}   //  end destructor


//----------------------------------------------------------------------------
// Function to get the client from which  the Transaction came from
//
//	returns - null Some error occurred
//	returns - a Session referring to the client the transaction came from
//
DaSession* DaTransaction::getSession(void)
{
	if (m_sessionHandle != 0)
	{
		return Application::Instance()->getSession(m_sessionHandle);
	}   //  end if
	else
	{
		return NULL;
	}   //  end if ... else
}   //  end getSession


//----------------------------------------------------------------------------
// Completes all requests must be called while or after "handling" requests
//
// S_OK - Everything was OK
// S_FALSE - not everything ok, but usable results
// The Result should be checked with ResultCode.SUCCEEDED
// or with ResultCode.FAILED
//
long DaTransaction::completeRequests(void)
{
	long result = E_FAIL;
	m_requestListJanitor.lock();
	size_t count = m_requestList.size();

	if (count == 0)
	{
		m_requestListJanitor.unlock();
		return EnumResultCode_S_FALSE;
	}   //  end if

	OTValueData* values = new OTValueData[count];
	OTSRequestData* requests = new OTSRequestData[count];
	long* results = new long[count];

	for (size_t i = 0; i < count; i++)
	{
		if (m_requestList[i] != NULL)
		{
			requests[i].m_clientHandle  = m_requestList[i]->getSessionHandle();
			requests[i].m_propertyID    = m_requestList[i]->getPropertyId();
			requests[i].m_requestHandle = m_requestList[i]->getRequestHandle();
			DaAddressSpaceElement* element = m_requestList[i]->getAddressSpaceElement();

			if (element != NULL)
			{
				requests[i].m_object.m_userData     = element->getUserData();
				requests[i].m_object.m_objectHandle = element->getObjectHandle();
			}
			else
			{
				requests[i].m_object.m_userData     = 0;
				requests[i].m_object.m_objectHandle = 0;
			}   //  end if ... else

			//  fill in the result
			ValueQT* value = m_requestList[i]->getValue();

			if (value != NULL)
			{
				values[i].m_timestamp   = value->getTimeStamp().get();
				values[i].m_quality     = (unsigned char)value->getQuality();
				values[i].m_pValue      = new VARIANT();
				value->getData().copyTo(values[i].m_pValue);
				results[i] = m_requestList[i]->getResult();
			}
			else
			{
				values[i].m_quality     = EnumQuality_BAD;
				values[i].m_pValue      = new VARIANT();
				values[i].m_pValue->vt  = VT_EMPTY;
				results[i]              = EnumResultCode_E_UNEXPECTED;
			}   //  end if ... else
		}
		else
		{
			requests[i].m_clientHandle          = 0;
			requests[i].m_propertyID            = 0;
			requests[i].m_requestHandle         = 0;
			requests[i].m_object.m_userData     = 0;
			requests[i].m_object.m_objectHandle = 0;
			values[i].m_quality     = EnumQuality_BAD;
			values[i].m_pValue      = new VARIANT();
			values[i].m_pValue->vt  = VT_EMPTY;
			results[i] = EnumResultCode_E_UNEXPECTED;
		}   //  end if ... else

		m_requestList[i]->setRequestState(EnumRequestState_COMPLETED);
	}   //  end for

	// empty request list
	for (size_t k = 0; k < m_requestList.size(); k++)
	{
		if (m_requestList[k] != NULL)
		{
			delete m_requestList[k];
			m_requestList[k] = NULL;
		}   //  end if
	}   //  end for

	m_requestList.clear();

	m_requestListJanitor.unlock();

	result = OTSCompleteRequests((long)count, requests, results, values);

	for (size_t j = 0; j < count; j++)
	{
		if (values[j].m_pValue != NULL)
		{
			Variant::variantClear(values[j].m_pValue);
			delete values[j].m_pValue;
			values[j].m_pValue = NULL;
		}   //  end if
	}   //  end for

	//  release the allocated memory
	if (values != NULL)
	{
		delete[] values;
	}   //  end if

	if (results != NULL)
	{
		delete[] results;
	}   //  end if

	if (requests != NULL)
	{
		delete[] requests;
	}   //  end if

	Application::Instance()->releaseTransaction(m_key);
	return result;
}   //  end completeRequests


//----------------------------------------------------------------------------
// completeRequest
//
long DaTransaction::completeRequest(DaRequest* aRequest)
{
	long result = E_FAIL;
	OTSRequestData requestData;
	memset(&requestData, 0, sizeof(OTSRequestData));
	OTValueData valueData;
	memset(&valueData, 0, sizeof(OTValueData));
	long resultData = 0;
	requestData.m_clientHandle  = aRequest->getSessionHandle();
	requestData.m_propertyID    = aRequest->getPropertyId();
	requestData.m_requestHandle = aRequest->getRequestHandle();
	DaAddressSpaceElement* element = aRequest->getAddressSpaceElement();

	if (element != NULL)
	{
		requestData.m_object.m_userData     = element->getUserData();
		requestData.m_object.m_objectHandle = element->getObjectHandle();
	}
	else
	{
		requestData.m_object.m_userData     = 0;
		requestData.m_object.m_objectHandle = 0;
	}   //  end if ... else

	//  fill in the result
	resultData = aRequest->getResult();
	ValueQT* value = aRequest->getValue();

	if (value != NULL)
	{
		valueData.m_timestamp   = value->getTimeStamp().get();
		valueData.m_quality     = (unsigned char)value->getQuality();
		valueData.m_pValue      = new VARIANT();
		value->getData().copyTo(valueData.m_pValue);
	}
	else
	{
		valueData.m_quality     = EnumQuality_BAD;
		valueData.m_pValue      = new VARIANT();
		valueData.m_pValue->vt  = VT_EMPTY;
		resultData              = EnumResultCode_E_UNEXPECTED;
	}   //  end if ... else

	aRequest->setRequestState(EnumRequestState_COMPLETED);
	removeRequest(aRequest);
	result = OTSCompleteRequests(1, &requestData, &resultData, &valueData);

	if (valueData.m_pValue != NULL)
	{
		Variant::variantClear(valueData.m_pValue);
		delete valueData.m_pValue;
		valueData.m_pValue = NULL;
	}   //  end if

	if (this->isEmpty())
	{
		Application::Instance()->releaseTransaction(m_key);
	}   //  end if

	return result;
}   //  end completeRequest

//-----------------------------------------------------------------------------
// requestCompletedInternally
//
long DaTransaction::findRequestByHandle(unsigned long requestHandle, DaRequest** aRequest)
{
	long ret = OT_E_OPC_NOTFOUND;
	(*aRequest) = NULL;
	std::vector<DaRequest*>::iterator requestIterator;
	m_requestListJanitor.lock();
	for (requestIterator = m_requestList.begin(); requestIterator != m_requestList.end(); ++requestIterator)
	{
		if (((*requestIterator) != NULL) && ((*requestIterator)->getRequestHandle() == requestHandle))
		{
			(*aRequest) = (*requestIterator);
			ret = S_OK;
			break;
		}
	}
	m_requestListJanitor.unlock();
	return ret;
}   //  end requestCompletedInternally

//-----------------------------------------------------------------------------
// ValuesChanged
//
long DaTransaction::valuesChanged(void)
{
	m_requestListJanitor.lock();
	size_t count = m_requestList.size();

	if (count == 0)
	{
		m_requestListJanitor.unlock();
		return EnumResultCode_S_FALSE;
	}   //  end if

	OTValueData* values = new OTValueData[count];
	unsigned long* handles = new unsigned long[count];

	for (size_t i = 0; i < count; i++)
	{
		DaRequest* request = m_requestList[i];
		BOOL setOk = FALSE;

		if (request != NULL)
		{
			ValueQT* value = request->getValue();

			if (value != NULL)
			{
				values[i].m_quality = (unsigned char)value->getQuality();
				values[i].m_timestamp = value->getTimeStamp().get();
				values[i].m_pValue = new VARIANT();
				value->getData().copyTo(values[i].m_pValue);
				handles[i] = request->getAddressSpaceElement()->getObjectHandle();
				setOk = TRUE;
			}   //  end if
		}   //  end if

		if (!setOk)
		{
			values[i].m_quality     = EnumQuality_BAD;
			values[i].m_pValue      = new VARIANT();
			values[i].m_pValue->vt  = VT_EMPTY;
			handles[i] = 0;
		}   //  end if ... else

		request->setResult(EnumResultCode_S_OK);
	}   //  end for

	long result = OTSValuesChanged((long)count, handles, values);
	m_requestListJanitor.unlock();

	for (size_t j = 0; j < count; j++)
	{
		if (values[j].m_pValue != NULL)
		{
			Variant::variantClear(values[j].m_pValue);
			delete values[j].m_pValue;
			values[j].m_pValue = NULL;
		}   //  end if
	}   //  end for

	//  release the allocated memory
	if (values != NULL)
	{
		delete[] values;
	}   //  end if

	if (handles != NULL)
	{
		delete [] handles;
	}   //  end if

	return result;
}   //  end ValuesChanged


//----------------------------------------------------------------------------
// removeRequest
//
void DaTransaction::removeRequest(DaRequest* aRequest)
{
	if (aRequest == NULL)
	{
		return;
	}   //  end if

	m_requestListJanitor.lock();

	std::vector<DaRequest*>::iterator requestIterator;
	for (requestIterator = m_requestList.begin(); requestIterator != m_requestList.end(); requestIterator++)
	{
		if (*requestIterator == aRequest)
		{
			delete *requestIterator;
			m_requestList.erase(requestIterator);
			break;
		}   //  end if
	}   //  end if

	m_requestListJanitor.unlock();
}   //  end removeRequest

