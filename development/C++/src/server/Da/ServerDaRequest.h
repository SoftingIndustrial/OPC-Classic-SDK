#ifndef _SERVERDAREQUEST_H_
#define _SERVERDAREQUEST_H_

#include "../../Enums.h"
#include "../ServerEnums.h"
#include "../../ValueQT.h"
#include "Mutex.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class DaAddressSpaceElement;
class DaTransaction;

// Stores data concerning a read- or write-request.
class TBC_EXPORT DaRequest
{

	friend class OTSGlobals;
	friend class DaTransaction;

private:

	unsigned long m_sessionHandle;
	unsigned long m_requestHandle;
	unsigned long m_transactionKey;
	EnumTransactionType m_transactionType;

	int m_propertyId;

	DaAddressSpaceElement* m_addressSpaceElement;

	long m_result;
	EnumRequestState m_requestState;
	ValueQT m_value;
	Mutex m_requestStateJanitor;

protected:

	unsigned long getRequestHandle(void)
	{
		return m_requestHandle;
	}
	unsigned long getSessionHandle(void)
	{
		return m_sessionHandle;
	}

public:

	DaRequest(EnumTransactionType aTransactionType,
			  unsigned long aSessionHandle,
			  DaAddressSpaceElement* anElement,
			  int aPropertyId,
			  unsigned long aRequestHandle);

	virtual ~DaRequest();

	// The property ID the request is about (0 means no property request)
	int getPropertyId(void)
	{
		return m_propertyId;
	}

	// The namespace element associated to the request
	DaAddressSpaceElement* getAddressSpaceElement(void)
	{
		return m_addressSpaceElement;
	}

	//  the getTransactionType
	EnumTransactionType getTransactionType(void)
	{
		return m_transactionType;
	}

	// The result of the request
	long getResult(void)
	{
		return m_result;
	}
	void setResult(long aValue)
	{
		m_result = aValue;
	}

	// the state of the request
	EnumRequestState getRequestState(void)
	{
		EnumRequestState ret;
		m_requestStateJanitor.lock();
		ret = m_requestState;
		m_requestStateJanitor.unlock();
		return ret;
	}
	void setRequestState(EnumRequestState aValue)
	{
		m_requestStateJanitor.lock();
		m_requestState = aValue;
		m_requestStateJanitor.unlock();
	}
	bool progressRequestState(EnumRequestState valueFrom, EnumRequestState valueTo)
	{
		m_requestStateJanitor.lock();
		if (m_requestState == valueFrom)
		{
			m_requestState = valueTo;
			m_requestStateJanitor.unlock();
			return true;
		}
		m_requestStateJanitor.unlock();
		return false;
	}

	// the transaction the request is contained in
	unsigned long getTransactionKey(void)
	{
		return m_transactionKey;
	}
	void setTransactionKey(unsigned long aValue)
	{
		m_transactionKey = aValue;
	}

	// This requests value with quality and timestamp.
	ValueQT* getValue(void)
	{
		return &m_value;
	}
	void setValue(ValueQT& aValue)
	{
		m_value = aValue;
	}

	// Complete only this request
	long complete(void);
	virtual void completedInternally();

};  //  end DaRequest

}   //  end namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _DAREQUEST_H_

