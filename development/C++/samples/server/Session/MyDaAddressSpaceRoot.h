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
	long queryAddressSpaceElementData(
		tstring& anElementID,
		AddressSpaceElement* anElement)
	{
		// TODO: add string based address space validations
		return E_NOTIMPL;
	} // end QueryAddressSpaceElementData

};  // end class MyDaAddressSpaceRoot

#endif
