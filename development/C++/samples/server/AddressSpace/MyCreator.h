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

	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElementEx(
		tstring& anItemId,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle,
		EnumAddressSpaceElementData elementDataFilter,
		EnumAccessRights accessRights,
		EnumIoMode ioMode,
		BOOL hasChildren,
		BOOL isBrowsable,
		VARENUM dataType)
	{
		// This is the extended version of the method createInternalDaAddressSpaceElement. The Toolkit CORE will call this method
		// (createInternalDaAddressSpaceElementEx) prior to calling the non-extended method (createInternalDaAddressSpaceElement)
		// and will only call the non-extended method if this method fails (if this method returns a null pointer).

		// This method allows the vendor application to avoid collecting all data about the AddressSpaceElement to be created
		// by requesting the vendor application only the data the Toolkit CORE does not have yet (data that was either never
		// requested by the Toolkit CORE or data that was requested through DaAddressSpaceRoot::queryAddressSpaceElementDataEx
		// and cached but the cache age has expired).

		// Based on the additional information (access rights, i/o mode, has children, is browsable, data type) provided
		// by the Toolkit CORE only if the corresponding flag for each information is NOT set in the elementDataFilter,
		// the vendor application may save time required for querying the underlaying system for this data.
		// Flags which ARE set require the vendor application to read their corresponding data from the underlying layer.

		// NOTE: The EnumElementData_ELEMENTTYPE flag is deprecated.
		//       Please ignore the EnumElementData_ELEMENTTYPE flag, if set.

		return (DaAddressSpaceElement*) new MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle,
			elementDataFilter, accessRights, ioMode, hasChildren, isBrowsable, dataType);
	}   //  end DaAddressSpaceElement

};  //  end class MyCreator

#endif
