#ifndef _BISCREATOR_H_
#define _BISCREATOR_H_

#include "ServerCreator.h"

#include "BisTransaction.h"
#include "BisDaRoot.h"
#include "BisAeRoot.h"



using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// BisCreator
//-----------------------------------------------------------------------------
class BisCreator : public Creator
{
public:
	virtual ~BisCreator(
		void)
	{
	} // end dtor

	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey)
	{
		return (DaTransaction*)new BisTransaction(aTransactionType, aRequestList, aSessionKey);
	}   //  end createTransaction

	virtual AeAddressSpaceElement* createAeAddressSpaceElement(
		void)
	{
		return new BaseAeElement();
	}   //  end createAeAddressSpaceElement

	virtual DaAddressSpaceElement* createDaAddressSpaceElement(
		void)
	{
		return new BaseDaElement();
	}   //  end createAeAddressSpaceElement

	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElement(
		tstring& anItemID,
		tstring& aName,
		unsigned int anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle)
	{
		return new BaseDaElement(anItemID, aName, anUserData, anObjectHandle, aParentHandle);
	}   //  end createAeAddressSpaceElement

	virtual AeAddressSpaceRoot* createAeAddressSpaceRoot(
		void)
	{
		return new BisAeRoot();
	}   //  end CreateDaAddressSpaceRoot

	virtual DaAddressSpaceRoot* createDaAddressSpaceRoot(
		void)
	{
		return new BisDaRoot();
	}   //  end CreateDaAddressSpaceRoot
};  //  end class BisCreator

#endif
