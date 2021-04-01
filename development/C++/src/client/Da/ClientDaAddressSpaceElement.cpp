#include "OSCompat.h"

#include "ClientDaAddressSpaceElement.h"
#include "ClientDaSession.h"
#include "../ClientAddressSpaceElementBrowseOptions.h"
#include "../ClientApplication.h"
#include "ClientDaGetPropertiesOptions.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

//----------------------------------------------------------------------
//DaAddressSpaceElement
//----------------------------------------------------------------------
DaAddressSpaceElement::DaAddressSpaceElement(
	EnumAddressSpaceElementType aType,
	tstring aName,
	tstring anItemId,
	unsigned long anObjectHandle,
	tstring anItemPath,
	DaSession* aSession):
	AddressSpaceElement(aType, aName, anItemId, anObjectHandle)
{
	m_itemPath = anItemPath;
	m_session = aSession;
}   //  end constructor


DaAddressSpaceElement::~DaAddressSpaceElement()
{
}   //  end destructor

tstring& DaAddressSpaceElement::getItemId()
{
	return AddressSpaceElement::getQualifiedName();
}   //  end getItemId

tstring& DaAddressSpaceElement::getItemPath()
{
	return m_itemPath;
}   //  end getItemPath

DaSession* DaAddressSpaceElement::getSession()
{
	return m_session;
}   //  end getSession

long DaAddressSpaceElement::browse(
	DaAddressSpaceElementBrowseOptions* browseOptions,
	std::vector<DaAddressSpaceElement*>& addressSpaceElements,
	ExecutionOptions* someExecutionOptions)
{
	OTCAddressSpaceBrowseOptions OTCBrowseOptions;
	OTCBrowseOptions.m_accessRightsFilter = 0;
	long result = E_FAIL;
	OTCExecutionOptions options;
	OTCAddressSpaceElementData* pAddressSpaceElement;
	unsigned long addressSpaceElementDataCount = 0;
	unsigned long i;
	DaSession* aSession = getSession();

	if (aSession == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaAddressSpaceElement::browse"),
			_T("The session attribute of the AddressSpaceElement cannot be null! Use setSession before calling browse!"));
		return result;
	}   //  end if

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext  = someExecutionOptions->getExecutionContext();
		options.m_executionType     = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	OTCBrowseOptions.m_type                     = (unsigned char)browseOptions->getElementTypeFilter();
#ifdef TBC_OS_WINDOWS
	OTCBrowseOptions.m_elementNameFilter        = _tcsdup((browseOptions->getElementNameFilter()).c_str());
	OTCBrowseOptions.m_vendorFilter             = _tcsdup((browseOptions->getVendorFilter()).c_str());
#endif
#ifdef TBC_OS_LINUX
	OTCBrowseOptions.m_elementNameFilter        = strdup((browseOptions->getElementNameFilter()).c_str());
	OTCBrowseOptions.m_vendorFilter             = strdup((browseOptions->getVendorFilter()).c_str());
#endif
	OTCBrowseOptions.m_accessRightsFilter       = browseOptions->getAccessRightsFilter();
	OTCBrowseOptions.m_datatypeFilter           = browseOptions->getDataTypeFilter();
	OTCBrowseOptions.m_maxElements              = browseOptions->getMaxElements();
	OTCBrowseOptions.m_retrieveItemID           = (unsigned char)browseOptions->getRetrieveItemId();
	OTCBrowseOptions.m_retrieveProperties       = (unsigned char)browseOptions->getReturnProperties();
	OTCBrowseOptions.m_retrievePropertyValues   = (unsigned char)browseOptions->getReturnPropertyValues();
	OTCBrowseOptions.m_forceBrowseUp            = (unsigned char)browseOptions->getForceBrowseUp();
	OTCBrowseOptions.m_continuationPoint        = (OTChar*) OTAllocateMemory((unsigned long)((browseOptions->getContinuationPoint().length() + 1 ) * sizeof(OTChar)));
	memcpy(OTCBrowseOptions.m_continuationPoint, browseOptions->getContinuationPoint().c_str(), browseOptions->getContinuationPoint().length());
#ifdef TBC_OS_WINDOWS
	OTChar* otcItemId = _tcsdup(this->getItemId().c_str());
	OTChar* otcItemPath = _tcsdup(this->getItemPath().c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcItemId = strdup(this->getItemId().c_str());
	OTChar* otcItemPath = strdup(this->getItemPath().c_str());
#endif
	result = OTCBrowseAddressSpace(
				 aSession->getHandle(),
				 m_objectElementHandle,
				 otcItemId,
				 otcItemPath,
				 &OTCBrowseOptions,
				 &addressSpaceElementDataCount,
				 &pAddressSpaceElement,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			for (i = 0; i < addressSpaceElementDataCount; i++)
			{
				DaAddressSpaceElement* element = new DaAddressSpaceElement(
					(EnumAddressSpaceElementType)pAddressSpaceElement[i].m_type,
					pAddressSpaceElement[i].m_name,
					pAddressSpaceElement[i].m_itemID,
					pAddressSpaceElement[i].m_objectHandle,
					pAddressSpaceElement[i].m_itemPath,
					aSession);
				addressSpaceElements.push_back(element);
				OTFreeMemory(pAddressSpaceElement[i].m_name);
				OTFreeMemory(pAddressSpaceElement[i].m_itemID);
				OTFreeMemory(pAddressSpaceElement[i].m_itemPath);
			} //end for

			if (pAddressSpaceElement)
			{
				OTFreeMemory(pAddressSpaceElement);
			}   //  end if
		browseOptions->setContinuationPoint(OTCBrowseOptions.m_continuationPoint);
		}   //  end if
	}   //  end if

	free(OTCBrowseOptions.m_vendorFilter);
	free(OTCBrowseOptions.m_elementNameFilter);
	OTFreeMemory(OTCBrowseOptions.m_continuationPoint);
	free(otcItemId);
	free(otcItemPath);
	return result;
} //end browse


long DaAddressSpaceElement::getDaProperties(
	DaGetPropertiesOptions* aGetPropertiesOptions,
	std::vector<DaProperty*>& someDaProperty,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	result = getSession()->getDaProperties(
				 getItemId(),
				 getItemPath(),
				 aGetPropertiesOptions,
				 someDaProperty,
				 someExecutionOptions);
	return result;
} //end getDaProperties


long DaAddressSpaceElement::getDaProperties(
	std::vector<DaProperty*>& someDaProperty,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	result = getSession()->getDaProperties(
				 this,
				 getItemId(),
				 getItemPath(),
				 NULL,
				 someDaProperty,
				 someExecutionOptions);
	return result;
} //end getDaProperties


bool DaAddressSpaceElement::isItem(void)
{
	return (m_type & EnumAddressSpaceElementType_DA_ITEM) != 0;
}   //  end isItem
