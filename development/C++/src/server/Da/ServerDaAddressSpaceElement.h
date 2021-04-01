#ifndef _SERVERDAADDRESSSPACEELEMENT_H_
#define _SERVERDAADDRESSSPACEELEMENT_H_

#include "../../Enums.h"
#include "../ServerEnums.h"

#include "../../ValueQT.h"
#include "../ServerAddressSpaceElement.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class DaProperty;
class DaSession;

class TBC_EXPORT DaAddressSpaceElement : public AddressSpaceElement
{

	friend class Creator;
	friend class DaAddressSpaceRoot;

private:
	tstring m_itemId;
	EnumAccessRights m_accessRights;
	EnumIoMode m_ioMode;
	VARENUM m_datatype;
	BOOL m_active;
	int m_updateRate;

protected:
	DaAddressSpaceElement();

	DaAddressSpaceElement(
		tstring anItemId,
		tstring aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

public:

	virtual ~DaAddressSpaceElement();

	virtual BOOL addChild(AddressSpaceElement* aChild);
	virtual BOOL removeChild(AddressSpaceElement* aChild);

	virtual long getCacheValue(ValueQT& aCacheValue);
	virtual long queryCacheValue(DaSession* aSession, ValueQT& aCacheValue);
	virtual long queryProperties(std::vector<DaProperty*>& aPropertyList);
	virtual long valueChanged(ValueQT& aNewValue);

	virtual AddressSpaceElement* getParent(void);
	virtual std::vector<AddressSpaceElement*> getChildren(void);

	virtual void change(
		BOOL isActive,
		int anUpdateRate)
	{
		m_active        = isActive;
		m_updateRate    = anUpdateRate;
	}   //  end change

	EnumAccessRights getAccessRights(void)
	{
		return m_accessRights;
	}
	void setAccessRights(EnumAccessRights aValue)
	{
		m_accessRights = aValue;
	}

	EnumIoMode getIoMode(void)
	{
		return m_ioMode;
	}
	void setIoMode(EnumIoMode aValue)
	{
		m_ioMode = aValue;
	}

	// Determines the datatype of this namespace element
	VARENUM getDatatype(void)
	{
		return m_datatype;
	}
	void setDatatype(VARENUM aValue)
	{
		m_datatype = aValue;
	}

	// Determines the update rate for the namespace element
	int getUpdateRate(void)
	{
		return m_updateRate;
	}

	// Determines wheter the namespace element is active(true) or not(false)
	BOOL getActive(void)
	{
		return m_active;
	}

	// Returns the item ID (namespace path) of this namespace element
	tstring& getItemId(void)
	{
		return m_itemId;
	}
	void setItemId(tstring& aValue)
	{
		m_itemId = aValue;
	}

	long setEUInfoAnalog(double lowEULimit, double highEULimit);
	long setEUInfoEnumerated(std::vector<tstring> enumeratedValues);
};  //  end class DaAddressSpaceElement


/// <summary>
/// AddressSpaceRoot: Represents the namespaces root
/// </summary>
class TBC_EXPORT DaAddressSpaceRoot : public AddressSpaceRoot
{

	friend class DaAddressSpaceElement;
	friend class OTSGlobals;

public:

	DaAddressSpaceRoot(EnumAddressSpaceType anAddressSpaceType);
	DaAddressSpaceRoot();

	virtual ~DaAddressSpaceRoot();

	virtual long queryAddressSpaceElementProperties(
		const tstring& anElementID,
		std::vector<DaProperty*>& aPropertyList);

	virtual long queryAddressSpaceElementData(
		const tstring& anElementId,
		AddressSpaceElement*& anElement);

	virtual long queryAddressSpaceElementDataEx(
		const tstring& anElementId,
		EnumAddressSpaceElementData& elementDataFilter,
		AddressSpaceElement*& anElement);

	virtual long queryAddressSpaceElementChildren(
		const tstring& anElementId,
		std::vector<AddressSpaceElement*>& aChildrenList);

	virtual long valuesChanged(
		std::vector<DaAddressSpaceElement*>& anElementList,
		std::vector<ValueQT*>& aValueList);

	//  Activation state updated event
	virtual void activationStateUpdated(std::vector<DaAddressSpaceElement*>& anElementList) {}

};  //  end DaAddressSpaceRoot

}   //  end namespace

#pragma pack(pop)
#endif  //  _DAADDRESSSPACEELEMENT_H_
