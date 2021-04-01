#ifndef _BISDAROOT_H_
#define _BISDAROOT_H_

#include "ServerCreator.h"
#include "Da/ServerDaAddressSpaceElement.h"

#include <vector>

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// BisDaRoot
//-----------------------------------------------------------------------------
class BisDaRoot : public DaAddressSpaceRoot
{
public:
	virtual ~BisDaRoot(
		void)
	{
	} // end dtor

	virtual long findAddressSpaceElementData(
		const tstring& anElementId,
		AddressSpaceElement*& anElement)
	{
		anElement = NULL;
		std::map<unsigned long, AddressSpaceElement*>::iterator iter;

		for (iter = m_elements.begin(); iter != m_elements.end(); ++iter)
		{
			if (iter->second != NULL)
			{
				BaseDaElement* daElement = dynamic_cast<BaseDaElement*>(iter->second);

				if (daElement != NULL)
				{
					if (daElement->getItemId() == anElementId)
					{
						anElement = daElement;
						return (long)EnumResultCode_S_OK;
					} // end if
				} // end fi
			} // end if
		} // end for

		return (long)EnumResultCode_E_NOTIMPL;
	} // end queryAddressSpaceElementData
	/*
	    virtual long queryAddressSpaceElementData(
	        const tstring& anElementId,
	        AddressSpaceElement*& anElement)
	    {
	        anElement = NULL;
	        std::map<unsigned long,AddressSpaceElement*>::iterator iter;
	        for(iter = m_elements.begin(); iter != m_elements.end(); ++iter ) {
	            if (iter->second != NULL) {
	                DaAddressSpaceElement* pDaElement = dynamic_cast<DaAddressSpaceElement*>(iter->second);
	                if (pDaElement != NULL) {
	                    if (pDaElement->getItemId() == anElementId) {
	                        *anElement = *pDaElement;

	                        return (long)EnumResultCode_S_OK;
	                    } // end if
	                } // end if
	            } // end if
	        } // end for

	        return (long)EnumResultCode_E_NOTIMPL;
	    } // end queryAddressSpaceElementData

	    virtual long queryAddressSpaceElementChildren(
	        const tstring& anElementId,
	        std::vector<AddressSpaceElement*>& aChildrenList)
	    {
	        std::map<unsigned long,AddressSpaceElement*>::iterator iter;
	        for(iter = m_elements.begin(); iter != m_elements.end(); ++iter ) {
	            if (iter->second != NULL) {
	                DaAddressSpaceElement* daElement = dynamic_cast<DaAddressSpaceElement*>(iter->second);
	                if (daElement->getItemId() == anElementId) {
	                    std::vector<AddressSpaceElement*> children = daElement->getChildren();
	                    aChildrenList.insert(aChildrenList.end(),
	                                         children.begin(),
	                                         children.end());
	                    return (long)EnumResultCode_S_OK;
	                } // end if
	            } // end if
	        } // end for

	        return (long)EnumResultCode_E_NOTIMPL;
	    }   //  end queryAddressSpaceElementChildren

	    virtual long queryAddressSpaceElementProperties(
	        const tstring& anElementID,
	        std::vector<DaProperty*>& aPropertyList)
	    {
	        std::map<unsigned long,AddressSpaceElement*>::iterator iter;
	        for(iter = m_elements.begin(); iter != m_elements.end(); ++iter ) {
	            if (iter->second != NULL) {
	                DaAddressSpaceElement* daElement = dynamic_cast<DaAddressSpaceElement*>(iter->second);
	                if (daElement->getItemId() == anElementID) {
	                    daElement->queryProperties(aPropertyList);
	                    return (long)EnumResultCode_S_OK;
	                } // end if
	            } // end if
	        } // end for

	        return (long)EnumResultCode_E_NOTIMPL;
	    } // end queryAddressSpaceElementProperties
	*/
}; // end BisDaRoot

#endif
