#ifndef _MYCREATOR_H_
#define _MYCREATOR_H_

#include "ServerCreator.h"

#include "MyDaAddressSpaceRoot.h"
#include "MyDaAddressSpaceElement.h"
#include "MyTransaction.h"

using namespace SoftingOPCToolboxServer;


//-----------------------------------------------------------------------------
// MyCreator
//-----------------------------------------------------------------------------
class MyCreator : public Creator
{

public:
	virtual DaAddressSpaceRoot* createDaAddressSpaceRoot(void)
	{
		return (DaAddressSpaceRoot*) new MyDaAddressSpaceRoot();
	}   //  end createDaAddressSpaceRoot

	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey)
	{
		return (DaTransaction*) new MyTransaction(aTransactionType, aRequestList, aSessionKey);
	}   //  end createTransaction

	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElement(
		tstring& anItemId,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle)
	{
		return (DaAddressSpaceElement*) new MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle);
	}   //  end DaAddressSpaceElement

};  //  end class MyCreator

#endif
