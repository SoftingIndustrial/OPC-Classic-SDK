#ifndef _MYCREATOR_H_
#define _MYCREATOR_H_


#include "ServerCreator.h"


#include "MyDaAddressSpaceRoot.h"
#include "MyDaAddressSpaceElement.h"
#include "MyRequest.h"
#include "MyTransaction.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyCreator
//-----------------------------------------------------------------------------
class MyCreator : public Creator
{
protected:


	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElement(
		tstring& anItemId,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle)
	{
		return new MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle);
	}   //  end createInternalDaAddressSpaceElement

public:

	virtual DaAddressSpaceRoot* createDaAddressSpaceRoot(void)
	{
		return new MyDaAddressSpaceRoot();
	}   //  end createDaAddressSpaceRoot

	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey)
	{
		return new MyTransaction(aTransactionType, aRequestList, aSessionKey);
	}   //  end createTransaction

	virtual DaRequest* createRequest(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* anElement,
		int aPropertyId,
		unsigned long aRequestHandle)
	{
		return new MyRequest(aTransactionType, aSessionHandle, anElement, aPropertyId, aRequestHandle);
	}   //  end createRequest

	virtual DaAddressSpaceElement* createMyDaAddressSpaceElement(void)
	{
		return new MyDaAddressSpaceElement();
	}   //  end createMyDaAddressSpaceElement


};  //  end class MyCreator

#endif
