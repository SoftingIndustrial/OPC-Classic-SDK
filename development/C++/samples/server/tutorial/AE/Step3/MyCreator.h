#ifndef _MYCREATOR_H_
#define _MYCREATOR_H_


#include "ServerCreator.h"


#include "MyAeAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyCreator
//-----------------------------------------------------------------------------
class MyCreator : public Creator
{
protected:

	virtual AeAddressSpaceElement* createInternalAeAddressSpaceElement(
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle)
	{
		return new MyAeAddressSpaceElement(aName, anUserData, anObjectHandle, aParentHandle);
	}   //  end createInternalAeAddressSpaceElement

public:


};  //  end class MyCreator

#endif
