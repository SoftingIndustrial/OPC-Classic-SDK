#ifndef _AE_ADDRESSSPACEELEMENT_H_
#define _AE_ADDRESSSPACEELEMENT_H_

#include "Ae/ServerAeAddressSpaceElement.h"
#include "Ae/ServerAeCategory.h"
#include "Ae/ServerAeCondition.h"
#include "Ae/ServerAeEvent.h"

using namespace SoftingOPCToolboxServer;

class DemoAeAddressSpaceElement : public AeAddressSpaceElement
{

public:
	DemoAeAddressSpaceElement(
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

	DemoAeAddressSpaceElement(void);

	long queryConditions(
		tstring& aSourcePath,
		std::vector<tstring>& aConditionNameList);

};  //  end class DemoAeAddressSpaceElement

#endif
