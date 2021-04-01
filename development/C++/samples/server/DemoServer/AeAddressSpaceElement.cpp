#include "stdafx.h"
#include "AeAddressSpaceElement.h"

DemoAeAddressSpaceElement::DemoAeAddressSpaceElement(
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
} // end constructor

DemoAeAddressSpaceElement::DemoAeAddressSpaceElement(void) : AeAddressSpaceElement()
{
} // end constructor

long DemoAeAddressSpaceElement::queryConditions(
	tstring& aSourcePath,
	std::vector<tstring>& aConditionNameList)
{
	/*
	tstring source(aSourcePath);

	if (source == _T("clock.time slot 2")){
	    aConditionNameList.push_back(COND_NAME_BETWEEN_MULTIPLE);
	}   //  end if
	if (source == _T("clock.time slot 1")){
	    aConditionNameList.push_back(COND_NAME_BETWEEN_SINGLE);
	}   //  end if
	*/
	return S_OK;
}   //  end queryConditions
