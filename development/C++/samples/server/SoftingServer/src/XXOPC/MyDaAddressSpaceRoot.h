#ifndef _MYDAADDRESSSPACEROOT_H_
#define _MYDAADDRESSSPACEROOT_H_

#include "Da/ServerDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyDaAddressSpaceRoot
//-----------------------------------------------------------------------------
class MyDaAddressSpaceRoot : public DaAddressSpaceRoot
{

public:
	virtual long queryAddressSpaceElementData(
		tstring& anElementID,
		AddressSpaceElement* anElement);

	virtual void activationStateUpdated(
		std::vector<DaAddressSpaceElement*>& anElementList);

};  //  end class MyDaAddressSpaceRoot

#endif
