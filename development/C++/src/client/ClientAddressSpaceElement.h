#ifndef _CLIENTADDRESSSPACEELEMENT_H_
#define _CLIENTADDRESSSPACEELEMENT_H_

#include "../Enums.h"

#include "ClientEnums.h"
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class TBC_EXPORT AddressSpaceElement
{
	friend class AeSession;
	friend class OTCGlobals;

protected:
	EnumAddressSpaceElementType m_type;
	tstring m_name;
	tstring m_qName;
	unsigned long m_objectElementHandle;

public:
	AddressSpaceElement(
		EnumAddressSpaceElementType aType,
		tstring aName,
		tstring aQName,
		unsigned long anObjectHandle);

public:
	virtual ~AddressSpaceElement();
	unsigned long getHandle();
	tstring& getName();
	tstring& getQualifiedName();
	bool isBranch(void);
	bool isLeaf(void);

};  //  end class AddressSpaceElement

}   // end SoftingOPCToolboxClient

#pragma pack(pop)
#endif  //  _CLIENTADDRESSSPACEELEMENT_H_
