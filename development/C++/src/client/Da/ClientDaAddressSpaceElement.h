#ifndef _CLIENTDAADDRESSSPACEELEMENT_H_
#define _CLIENTDAADDRESSSPACEELEMENT_H_

#include "../../Enums.h"
#include "../ClientAddressSpaceElement.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class DaSession;
class DaAddressSpaceElementBrowseOptions;
class DaGetPropertiesOptions;
class DaProperty;

class TBC_EXPORT DaAddressSpaceElement : public AddressSpaceElement
{

	friend class DaSession;
	friend class OTCGlobals;

protected:
	DaSession* m_session;
	tstring m_itemPath;

public:
	DaAddressSpaceElement(
		EnumAddressSpaceElementType aType,
		tstring aName,
		tstring anItemId,
		unsigned long anObjectHandle,
		tstring anItemPath,
		DaSession* aSession);

	virtual ~DaAddressSpaceElement();

	tstring& getItemId();
	tstring& getItemPath();
	DaSession* getSession();
	bool isItem(void);

	virtual long browse(
		DaAddressSpaceElementBrowseOptions* browseOptions,
		std::vector<DaAddressSpaceElement*>& addressSpaceElements,
		ExecutionOptions* someExecutionOptions);

	virtual long getDaProperties(
		DaGetPropertiesOptions* aGetPropertiesOptions,
		std::vector<DaProperty*>& someDaProperty,
		ExecutionOptions* someExecutionOptions);

	virtual long getDaProperties(
		std::vector<DaProperty*>& someDaProperty,
		ExecutionOptions* someExecutionOptions);

}; // end DaAddressSpaceElement
} // end SoftingOPCToolboxClient

#pragma pack(pop)
#endif
