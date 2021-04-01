#ifndef _BISAEROOT_H_
#define _BISAEROOT_H_

#include "ServerCreator.h"
#include "Ae/ServerAeAddressSpaceElement.h"

#include "BaseDaElement.h"

#include <vector>

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// BisDaRoot
//-----------------------------------------------------------------------------
class BisAeRoot : public AeAddressSpaceRoot
{
public:
	virtual ~BisAeRoot(
		void)
	{
	} // end dtor

	virtual long queryAddressSpaceElementData(
		const tstring& anElementId,
		AddressSpaceElement*& anElement)
	{
		anElement = NULL;
		BisDaRoot* pDaRoot = (BisDaRoot*)getApp()->getDaAddressSpaceRoot();
		AddressSpaceElement* anAddressSpaceElement = NULL;
		pDaRoot->findAddressSpaceElementData(anElementId, anAddressSpaceElement);

		if (anAddressSpaceElement != NULL)
		{
			BaseDaElement* pDaElement = dynamic_cast<BaseDaElement*>(anAddressSpaceElement);

			if (pDaElement != NULL)
			{
				anElement = getApp()->getCreator()->createAeAddressSpaceElement();
				anElement->setName(pDaElement->getName());
				anElement->setHasChildren(pDaElement->getHasChildren());
				return (long)EnumResultCode_S_OK;
			} // end if
		} // end if

		return (long)EnumResultCode_E_NOTIMPL;
	} // end queryAddressSpaceElementData

	virtual long queryAddressSpaceElementChildren(
		const tstring& anElementId,
		std::vector<AddressSpaceElement*>& aChildrenList)
	{
		BisDaRoot* pDaRoot = (BisDaRoot*)getApp()->getDaAddressSpaceRoot();
		std::vector<AddressSpaceElement*> elements;

		if (anElementId == tstring(_T("")))
		{
			elements = pDaRoot->getChildren();
		}
		else
		{
			AddressSpaceElement* anAddressSpaceElement = NULL;
			pDaRoot->findAddressSpaceElementData(anElementId, anAddressSpaceElement);

			if (anAddressSpaceElement != NULL)
			{
				BaseDaElement* pDaElement = dynamic_cast<BaseDaElement*>(anAddressSpaceElement);

				if (pDaElement != NULL)
				{
					elements = pDaElement->getChildren();
				} // end if
			} // end if
		} // end if .. else

		if (elements.empty() != true)
		{
			std::vector<AddressSpaceElement*>::iterator iter;

			for (size_t index = 0; index < elements.size(); ++index)
			{
				BaseDaElement* pDaElement = dynamic_cast<BaseDaElement*>(elements.at(index));

				if (pDaElement != NULL && pDaElement->getName() != tstring(_T("CMD")))
				{
					AeAddressSpaceElement* anElement = getApp()->getCreator()->createAeAddressSpaceElement();
					anElement->setName(pDaElement->getName());
					anElement->setHasChildren(pDaElement->getHasChildren());
					aChildrenList.push_back(anElement);
				} // end if
			} // end for
		} // end if

		return (long)EnumResultCode_S_OK;
	}   //  end queryAddressSpaceElementChildren
}; // end BisAeRoot

#endif
