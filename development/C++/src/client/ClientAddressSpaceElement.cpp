#include "OSCompat.h"

#include "ClientAddressSpaceElement.h"
#include "ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

AddressSpaceElement::AddressSpaceElement(
	EnumAddressSpaceElementType aType,
	tstring aName,
	tstring aQName,
	unsigned long aHandle)
{
	m_type = aType;
	m_name = aName;
	m_qName = aQName;
	m_objectElementHandle = aHandle;
} //end constructor

AddressSpaceElement::~AddressSpaceElement()
{
	if (m_objectElementHandle != 0)
	{
		OTCReleaseAddressSpaceElement(m_objectElementHandle, 0);
	} //end if
} //end destructor


unsigned long AddressSpaceElement::getHandle()
{
	return m_objectElementHandle;
}   //  end getHandle

tstring& AddressSpaceElement::getName()
{
	return m_name;
} // end getName

tstring& AddressSpaceElement::getQualifiedName()
{
	return m_qName;
} // end getQualifiedName


bool AddressSpaceElement::isBranch(void)
{
	return (m_type & EnumAddressSpaceElementType_BRANCH) != 0;
} //    end isBranch

bool AddressSpaceElement::isLeaf(void)
{
	return (m_type & EnumAddressSpaceElementType_LEAF) != 0;
} // isLeaf
