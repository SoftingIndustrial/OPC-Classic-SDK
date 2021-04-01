#ifndef _CLIENTADDRESSSPACEELEMENTBROWSEOPTIONS_
#define _CLIENTADDRESSSPACEELEMENTBROWSEOPTIONS_

#include "../Enums.h"
#include "ClientEnums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class TBC_EXPORT AddressSpaceElementBrowseOptions
{

protected:

	EnumAddressSpaceElementType m_elementTypeFilter;
	tstring m_elementNameFilter;
	BOOL m_forceBrowseUp;

public:

	AddressSpaceElementBrowseOptions();
	virtual ~AddressSpaceElementBrowseOptions();

	EnumAddressSpaceElementType getElementTypeFilter();
	void setElementTypeFilter(EnumAddressSpaceElementType);

	tstring& getElementNameFilter();
	void setElementNameFilter(tstring aNameFilter);

	BOOL getForceBrowseUp();
	void setForceBrowseUp(BOOL forceBrowseUp);
};

class TBC_EXPORT DaAddressSpaceElementBrowseOptions: public AddressSpaceElementBrowseOptions
{

private:

	unsigned long m_maxElements;
	BOOL m_returnProperties;
	BOOL m_returnPropertyValues;
	BOOL m_retrieveItemID;
	tstring m_vendorFilter;
	tstring m_continuationPoint;
	VARTYPE m_dataTypeFilter;
	EnumAccessRights m_accessRightsFilter;


protected:



public:

	DaAddressSpaceElementBrowseOptions();
	virtual ~DaAddressSpaceElementBrowseOptions();

	unsigned long getMaxElements();
	void setMaxElements(unsigned long nElements);

	tstring& getVendorFilter();
	void setVendorFilter(tstring aVendorFilter);

	tstring& getContinuationPoint();
	void setContinuationPoint(tstring aContinuationPoint);

	EnumAccessRights getAccessRightsFilter();
	void setAccessRightsFilter(EnumAccessRights someAccessRights);

	VARTYPE getDataTypeFilter();
	void setDataTypeFilter(VARTYPE aDataTypeFilter);

	BOOL getReturnPropertyValues();
	void setReturnPropertyValues(BOOL returnPropertyValues);

	BOOL getReturnProperties();
	void setReturnProperties(BOOL returnProperties);

	BOOL getRetrieveItemId();
	void setRetrieveItemId(BOOL retrieveItemId);


}; //end DaAddressSpaceElementBrowseOptions

} //end SoftingOPCToolboxClient

#pragma pack(pop)
#endif
