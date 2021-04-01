#include "OSCompat.h"
#include "ClientAddressSpaceElementBrowseOptions.h"

using namespace SoftingOPCToolboxClient;

//----------------------------------------------------------------------
//AddressSpaceElementBrowseOptions
//----------------------------------------------------------------------

AddressSpaceElementBrowseOptions::AddressSpaceElementBrowseOptions()
{
	m_elementTypeFilter = EnumAddressSpaceElementType_ALL;
	m_elementNameFilter = _T("");
	m_forceBrowseUp = FALSE;
} //end constructor

AddressSpaceElementBrowseOptions::~AddressSpaceElementBrowseOptions()
{
} //end destructor

EnumAddressSpaceElementType AddressSpaceElementBrowseOptions::getElementTypeFilter()
{
	return (EnumAddressSpaceElementType)m_elementTypeFilter;
} //end getElementTypeFilter

void AddressSpaceElementBrowseOptions::setElementTypeFilter(EnumAddressSpaceElementType aType)
{
	m_elementTypeFilter = aType;
} //end setElementTypeFilter

tstring& AddressSpaceElementBrowseOptions::getElementNameFilter()
{
	return m_elementNameFilter;
} //end getElementNameFilter

void AddressSpaceElementBrowseOptions::setElementNameFilter(tstring aNameFilter)
{
	m_elementNameFilter = aNameFilter;
} //end setElementNameFilter

BOOL AddressSpaceElementBrowseOptions::getForceBrowseUp()
{
	return m_forceBrowseUp;
}   // end getForceBrowseUp

void AddressSpaceElementBrowseOptions::setForceBrowseUp(BOOL forceBrowseUp)
{
	m_forceBrowseUp = forceBrowseUp;
}   // end setForceBrowseUp

//----------------------------------------------------------------------
//DaAddressSpaceElementBrowseOptions
//----------------------------------------------------------------------

DaAddressSpaceElementBrowseOptions::DaAddressSpaceElementBrowseOptions()
{
	m_maxElements = 0;
	m_returnProperties = FALSE;
	m_returnPropertyValues = FALSE;
	m_retrieveItemID = TRUE;
	m_dataTypeFilter = 0;
	m_accessRightsFilter = (EnumAccessRights)0;
} //end constructor

DaAddressSpaceElementBrowseOptions::~DaAddressSpaceElementBrowseOptions()
{
} //end destructor

unsigned long DaAddressSpaceElementBrowseOptions::getMaxElements()
{
	return m_maxElements;
} //end getMaxElements

void DaAddressSpaceElementBrowseOptions::setMaxElements(unsigned long nElements)
{
	m_maxElements = nElements;
} //end setMaxElements

tstring& DaAddressSpaceElementBrowseOptions::getVendorFilter()
{
	return m_vendorFilter;
}//end getVendorFilter

void DaAddressSpaceElementBrowseOptions::setVendorFilter(tstring aVendorFilter)
{
	m_vendorFilter = aVendorFilter;
} //end setVendorFilter

tstring& DaAddressSpaceElementBrowseOptions::getContinuationPoint()
{
	return m_continuationPoint;
}//end getContinuationPoint

void DaAddressSpaceElementBrowseOptions::setContinuationPoint(tstring aContinuationPoint)
{
	m_continuationPoint = aContinuationPoint;
} //end setContinuationPoint

EnumAccessRights DaAddressSpaceElementBrowseOptions::getAccessRightsFilter()
{
	return (EnumAccessRights)m_accessRightsFilter;
} //end getAccessRightsFilter

void DaAddressSpaceElementBrowseOptions::setAccessRightsFilter(EnumAccessRights someAccessRights)
{
	m_accessRightsFilter = someAccessRights;
} //end setAccessRightsFilter

VARTYPE DaAddressSpaceElementBrowseOptions::getDataTypeFilter()
{
	return m_dataTypeFilter;
} //end getDataTypeFilter

void DaAddressSpaceElementBrowseOptions::setDataTypeFilter(VARTYPE aDataTypeFilter)
{
	m_dataTypeFilter = aDataTypeFilter;
} //end setDataTypeFilter

BOOL DaAddressSpaceElementBrowseOptions::getReturnPropertyValues()
{
	return m_returnPropertyValues;
} //end getReturnPropertyValues

void DaAddressSpaceElementBrowseOptions::setReturnPropertyValues(BOOL returnPropertyValues)
{
	m_returnPropertyValues = returnPropertyValues;
} //end setReturnPropertyValues

BOOL DaAddressSpaceElementBrowseOptions::getReturnProperties()
{
	return m_returnProperties;
} //end getReturnProperties

void DaAddressSpaceElementBrowseOptions::setReturnProperties(BOOL returnProperties)
{
	m_returnProperties = returnProperties;
} //end setReturnProperties

BOOL DaAddressSpaceElementBrowseOptions::getRetrieveItemId()
{
	return m_retrieveItemID;
} //end getRetrieveItemId

void DaAddressSpaceElementBrowseOptions::setRetrieveItemId(BOOL retrieveItemId)
{
	m_retrieveItemID = retrieveItemId;
} //end setRetrieveItemId
