#include "stdafx.h"
#include "MyDaAddressSpaceElement.h"
#include "OpcServer.h"


MyDaAddressSpaceElement::MyDaAddressSpaceElement(void)
	: m_dwIOIdentifier(OpcServer::IOBYTE_TAG_NOTDEFINED)
{
}   //  end ctr

MyDaAddressSpaceElement::MyDaAddressSpaceElement(DWORD dwIOIdentifier)
	: m_dwIOIdentifier(dwIOIdentifier)
{
}   //  end ctr

MyDaAddressSpaceElement::MyDaAddressSpaceElement(
	tstring& anItemID,
	tstring& aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle) :
	DaAddressSpaceElement(
		anItemID, aName, anUserData, anObjectHandle, aParentHandle),
	m_dwIOIdentifier(OpcServer::IOBYTE_TAG_NOTDEFINED)
{
}   //  end ctr

MyDaAddressSpaceElement::MyDaAddressSpaceElement(
	tstring& anItemID,
	tstring& aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle,
	DWORD dwIOIdentifier):
	DaAddressSpaceElement(
		anItemID, aName, anUserData, anObjectHandle, aParentHandle),
	m_dwIOIdentifier(dwIOIdentifier)
{
}   //  end ctr
