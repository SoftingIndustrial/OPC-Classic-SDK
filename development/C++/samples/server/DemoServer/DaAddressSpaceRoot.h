#ifndef _DEMODAADDRESSSPACEROOT_H_
#define _DEMODAADDRESSSPACEROOT_H_

#include "Da/ServerDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

class DemoDaAddressSpaceRoot : public DaAddressSpaceRoot
{

public:

	long queryAddressSpaceElementData(
		tstring& anElementID,
		AddressSpaceElement* pAnElement);

};  //  end class MyDaAddressSpaceRoot

#endif
