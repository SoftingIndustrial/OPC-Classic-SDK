#ifndef _MYAEADDRESSSPACEELEMENT_H_
#define _MYAEADDRESSSPACEELEMENT_H_

#include "Ae/ServerAeAddressSpaceElement.h"
#include "Ae/ServerAeCategory.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeEvent.h"

using namespace SoftingOPCToolboxServer;

extern tstring COND_NAME_BETWEEN_SINGLE;
extern tstring COND_NAME_BETWEEN_MULTIPLE;

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
	{
	}   //  end ctr

	MyAeAddressSpaceElement(void)
	{
	}   //  end ctr

	long queryConditions(
		tstring& aSourcePath,
		std::vector<tstring>& aConditionNameList)
	{
		tstring source(aSourcePath);

		if (source == _T("clock.time slot 2"))
		{
			aConditionNameList.push_back(COND_NAME_BETWEEN_MULTIPLE);
		}   //  end if

		if (source == _T("clock.time slot 1"))
		{
			aConditionNameList.push_back(COND_NAME_BETWEEN_SINGLE);
		}   //  end if

		return S_OK;
	}   //  end queryConditions

};  //  end class MyAeAddressSpaceElement

#endif
