#include "stdafx.h"
#include "MyDaAddressSpaceRoot.h"
#include "OpcServer.h"
#include "IOManager.h"


using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyDaAddressSpaceRoot
//-----------------------------------------------------------------------------

long MyDaAddressSpaceRoot::queryAddressSpaceElementData(
	tstring& anElementID,
	AddressSpaceElement* anElement)
{
	//  TODO: add string based address space validations
	return E_NOTIMPL;
}   //  end QueryAddressSpaceElementData

void MyDaAddressSpaceRoot::activationStateUpdated(
	std::vector<DaAddressSpaceElement*>& anElementList)
{
	getOpcServer()->getIOManager()->cyclicStateChanged(anElementList);
}
