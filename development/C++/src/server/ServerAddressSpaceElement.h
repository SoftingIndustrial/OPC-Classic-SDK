#ifndef _SERVERADDRESSSPACEELEMENT_H_
#define _SERVERADDRESSSPACEELEMENT_H_

#include "../Enums.h"
#include "ServerEnums.h"
#include "Mutex.h"

#include <vector>
#include <map>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxServer
{

class AddressSpaceElement;

class TBC_EXPORT IAddressSpaceElement
{

public:
	virtual ~IAddressSpaceElement() {};
	virtual BOOL addChild(AddressSpaceElement* aChild) = 0;
	virtual BOOL removeChild(AddressSpaceElement* aChild) = 0;

	virtual std::vector<AddressSpaceElement*> getChildren(void) = 0;
	virtual AddressSpaceElement* getParent(void) = 0;

};  //  class end IAddressSpaceElement

class TBC_EXPORT AddressSpaceElement : public IAddressSpaceElement
{

	friend class OTSGlobals;
	friend class DaTransaction;
	friend class DaRequest;
protected:

	AddressSpaceElement(
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

	AddressSpaceElement();

public:
	virtual ~AddressSpaceElement();

	virtual BOOL addChild(AddressSpaceElement* aChild);
	virtual BOOL removeChild(AddressSpaceElement* aChild);

	tstring& getName(void)
	{
		return m_name;
	}
	void setName(tstring& aValue)
	{
		m_name = aValue;
	}

	BOOL getHasChildren(void)
	{
		return m_hasChildren;
	}
	void setHasChildren(BOOL aValue)
	{
		m_hasChildren = aValue;
	}

	BOOL getIsBrowsable(void)
	{
		return m_isBrowsable;
	}
	void setIsBrowsable(BOOL aValue)
	{
		m_isBrowsable = aValue;
	}

	virtual unsigned long getUserData(void)
	{
		return m_userData;
	}
	virtual void setUserData(unsigned long aValue)
	{
		m_userData = aValue;
	}

	//  Events
	virtual void addedToAddressSpace() {}
	virtual void removedFromAddressSpace() {}

protected:

	tstring m_name;
	BOOL m_hasChildren;
	BOOL m_isBrowsable;
	unsigned long m_userData;

	unsigned long m_objectHandle;
	unsigned long m_parentHandle;

	void setObjectHandle(unsigned long aValue)
	{
		m_objectHandle = aValue;
	}
	void setParentHandle(unsigned long aValue)
	{
		m_parentHandle = aValue;
	}

	unsigned long getObjectHandle(void)
	{
		return m_objectHandle;
	}
	unsigned long getParentHandle(void)
	{
		return m_parentHandle;
	}

	virtual BOOL triggerAddedToAddressSpace();
	virtual BOOL triggerRemovedFromAddressSpace();

private:

	static unsigned long m_currentHash;

};  //  end class AddressSpaceElement


class TBC_EXPORT AddressSpaceRoot : public IAddressSpaceElement
{

	friend class OTSGlobals;

protected:

	EnumAddressSpaceType m_namespaceType;
	std::map<unsigned long, AddressSpaceElement*> m_elements;
	Mutex m_elementsJanitor;
	AddressSpaceElement* m_root;

	AddressSpaceRoot(AddressSpaceElement* aRoot);
	AddressSpaceRoot(
		EnumAddressSpaceType anAddressSpaceType,
		AddressSpaceElement* aRoot);

	virtual BOOL addElementToArray(AddressSpaceElement* anElement);
	virtual AddressSpaceElement* getElementFromArray(unsigned long anElementUserData);

	virtual BOOL removeElementFromArray(AddressSpaceElement* anElement);
	virtual BOOL removeElementFromArray(unsigned long anElementUserData);

	virtual AddressSpaceElement* getParent(unsigned long aHandle);

public:

	virtual ~AddressSpaceRoot(void);

	virtual BOOL addChild(AddressSpaceElement* aChild);
	virtual BOOL removeChild(AddressSpaceElement* aChild);

	Mutex getElementSyncRoot();

	virtual std::vector<AddressSpaceElement*> getChildren(void);
	virtual AddressSpaceElement* getParent()
	{
		return m_root->getParent();
	}

	virtual long queryAddressSpaceElementData(
		const tstring& anElementId,
		AddressSpaceElement*& anElementDetails) = 0;

	virtual long queryAddressSpaceElementChildren(
		const tstring& anElementId,
		std::vector<AddressSpaceElement*>& aChildrenList) = 0;

};  //  end class AddressSpaceRoot

};  //  end namespace

#pragma pack(pop)
#endif
