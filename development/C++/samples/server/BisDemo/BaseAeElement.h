#ifndef _BASEAEELEMENT_H_
#define _BASEAEELEMENT_H_

#include "Ae/ServerAeAddressSpaceElement.h"
#include "Ae/ServerAeCategory.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeEvent.h"

#include <vector>

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// BaseAeElement
//-----------------------------------------------------------------------------
class BaseAeElement : public AeAddressSpaceElement
{
public:
	BaseAeElement(
		tstring& aName,
		unsigned int anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle) :
		AeAddressSpaceElement(aName, anUserData, anObjectHandle, aParentHandle)
	{
	} // end ctr

	BaseAeElement(
		tstring& aName,
		bool hasChildren) :
		AeAddressSpaceElement()
	{
		m_name = aName;
		m_hasChildren = hasChildren;
	} // end ctr

	BaseAeElement(
		void) :
		AeAddressSpaceElement()
	{
	} // end ctr

	virtual ~BaseAeElement(
		void)
	{
	} // end dtor

	virtual long queryConditions(
		tstring& aSourcePath,
		std::vector<tstring>& aConditionNames)
	{
		return (long)EnumResultCode_S_OK;
	} // end QueryConditions
}; // end BaseAeElemenet

#endif
