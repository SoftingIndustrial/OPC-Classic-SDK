#include "stdafx.h"
#include "Request.h"

Request::Request(
	EnumTransactionType aTransactionType,
	unsigned long aSessionHandle,
	DaAddressSpaceElement* pAnElement,
	int aPropertyId,
	unsigned long aRequestHandle):
	DaRequest(aTransactionType,
			  aSessionHandle,
			  pAnElement,
			  aPropertyId,
			  aRequestHandle)
{
}   //  end ctor

Request::~Request(void)
{
}   //  end dtor
