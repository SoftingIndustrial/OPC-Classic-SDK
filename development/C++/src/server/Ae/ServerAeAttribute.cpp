#include "OSCompat.h"
#include "ServerAeAttribute.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// Constructor
//
AeAttribute::AeAttribute(
	unsigned long anId,
	tstring& aName,
	VARENUM aDataType):
	m_id(anId),
	m_name(aName),
	m_dataType(aDataType)
{
}   //  end ctr
