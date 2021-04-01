#ifndef _MYCREATOR_H_
#define _MYCREATOR_H_

#include "ServerCreator.h"

#include "MyTransaction.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyCreator
//-----------------------------------------------------------------------------
class MyCreator : public Creator
{

public:
	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey)
	{
		return (DaTransaction*) new MyTransaction(aTransactionType, aRequestList, aSessionKey);
	}   //  end createTransaction

	virtual DaAddressSpaceElement* createDaAddressSpaceElement(void)
	{
		return (DaAddressSpaceElement*) new MyDaAddressSpaceElement();
	}   //  end DaAddressSpaceElement

};  //  end class MyCreator

#endif
