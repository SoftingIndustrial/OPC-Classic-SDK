#ifndef _MYAEADDRESSSPACEELEMENT_H_
#define _MYAEADDRESSSPACEELEMENT_H_

#include "Ae/ServerAeAddressSpaceElement.h"
#include "Ae/ServerAeCategory.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeEvent.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyAeAddressSpaceElement
//-----------------------------------------------------------------------------
class MyAeAddressSpaceElement : public AeAddressSpaceElement
{

public:
	MyAeAddressSpaceElement(
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle) :
		AeAddressSpaceElement(
			aName,
			anUserData,
			anObjectHandle,
			aParentHandle)
	{}  //  end cotr

	MyAeAddressSpaceElement(void)
	{}  //  end cotr

	virtual ~MyAeAddressSpaceElement(void)
	{}  //  end dtor

};  //  end class MyAeAddressSpaceElement

#endif
