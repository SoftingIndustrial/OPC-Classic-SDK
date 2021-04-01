#include "OSCompat.h"
#include "ServerAddressSpaceElement.h"

#include "OTServer.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	static members initialization
//
unsigned long AddressSpaceElement::m_currentHash = 1;

//-----------------------------------------------------------------------------
//	Constructor
//
AddressSpaceElement::AddressSpaceElement(
	tstring& aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle)
{
	m_name = aName;
	m_objectHandle = anObjectHandle;
	m_parentHandle = aParentHandle;
	m_hasChildren = false;
	m_isBrowsable = true;

	if (anUserData != 0)
	{
		m_userData = anUserData;
	}
	else
	{
		m_userData = m_currentHash++;
	}   //  end if ... else
}   //  end ctr


//-----------------------------------------------------------------------------
//	Constructor
//
AddressSpaceElement::AddressSpaceElement()
{
	m_objectHandle = 0;
	m_parentHandle = 0;
	m_hasChildren = FALSE;
	m_userData = m_currentHash++;
	m_isBrowsable = true;
}   //  end ctr


//-----------------------------------------------------------------------------
//	Destructor
//
AddressSpaceElement::~AddressSpaceElement()
{
}   //  end destructor


//-----------------------------------------------------------------------------
//	addChild
//
BOOL AddressSpaceElement::addChild(AddressSpaceElement* aChild)
{
	//  just trigger the added event
	if (aChild != NULL)
	{
		aChild->triggerAddedToAddressSpace();
	}   //  end if

	return TRUE;
}   //  end addChild


//-----------------------------------------------------------------------------
//	removeChild
//
BOOL AddressSpaceElement::removeChild(AddressSpaceElement* aChild)
{
	//  just trigger the removed event
	if (aChild != NULL)
	{
		aChild->triggerRemovedFromAddressSpace();
	}   //  end RemoveChild

	return TRUE;
}   //  end removeChild


//-----------------------------------------------------------------------------
//	removeChild
//
BOOL AddressSpaceElement::triggerAddedToAddressSpace()
{
	addedToAddressSpace();
	return TRUE;
}   //  end triggerAddedToAddressSpace


//-----------------------------------------------------------------------------
//	removeChild
//
BOOL AddressSpaceElement::triggerRemovedFromAddressSpace()
{
	removedFromAddressSpace();
	return TRUE;
}   //  end triggerRemovedFromAddressSpace


//-----------------------------------------------------------------------------
//	Constructor
//
AddressSpaceRoot::AddressSpaceRoot(AddressSpaceElement* aRoot) :
	m_namespaceType(EnumAddressSpaceType_STRING_BASED),
	m_root(aRoot)
{
	m_root->setHasChildren(TRUE);
	OTSInitAddressSpace((unsigned char)m_namespaceType);
}   //  end Constructor


//-----------------------------------------------------------------------------
//	Constructor
//
AddressSpaceRoot::AddressSpaceRoot(
	EnumAddressSpaceType anAddressSpaceType,
	AddressSpaceElement* aRoot) :
	m_namespaceType(anAddressSpaceType),
	m_root(aRoot)
{
	m_root->setHasChildren(TRUE);
	OTSInitAddressSpace((unsigned char)m_namespaceType);
}   //  end Constructor


//-----------------------------------------------------------------------------
//	Destructor
//
AddressSpaceRoot::~AddressSpaceRoot(void)
{
	if (m_root != NULL)
	{
		delete m_root;
	}   //  end if

	std::map<unsigned long, AddressSpaceElement*>::iterator elementIterator;
	m_elementsJanitor.lock();

	for (elementIterator = m_elements.begin(); elementIterator != m_elements.end(); elementIterator++)	{
		if (elementIterator->second != NULL)
		{
			delete elementIterator->second;
			elementIterator->second = NULL;
		}   //  end if
	}   //  end if

	m_elements.clear();
	m_elementsJanitor.unlock();
}   //  end destructor


//-----------------------------------------------------------------------------
//	addChild
//
BOOL AddressSpaceRoot::addChild(AddressSpaceElement* aChild)
{
	return m_root->addChild(aChild);
}   //  end addChild


//-----------------------------------------------------------------------------
//	removeChild
//
BOOL AddressSpaceRoot::removeChild(AddressSpaceElement* aChild)
{
	return m_root->removeChild(aChild);
}   //  end removeChild

Mutex AddressSpaceRoot::getElementSyncRoot()
{
	return m_elementsJanitor;
}

//-----------------------------------------------------------------------------
//	getParent
//
AddressSpaceElement* AddressSpaceRoot::getParent(unsigned long aHandle)
{
	if (aHandle == 0)
	{
		return m_root;
	}   //  end if

	OTObjectData parent;

	if (S_OK != OTSGetParent(aHandle, &parent))
	{
		return NULL;
	}   //  end if

	AddressSpaceElement* elementParent = getElementFromArray(parent.m_userData);

	if (elementParent == NULL)
	{
		return m_root;
	}   //  end if

	return elementParent;
}   //  end getParent


//-----------------------------------------------------------------------------
//	getChildren
//
std::vector<AddressSpaceElement*> AddressSpaceRoot::getChildren(void)
{
	return m_root->getChildren();
}   //  end getChildren


//-----------------------------------------------------------------------------
//	addElementToArray
//
BOOL AddressSpaceRoot::addElementToArray(AddressSpaceElement* anElement)
{
	if (anElement == NULL)
	{
		return FALSE;
	}   //  end if

	//  check for a duplicate element
	std::map<unsigned long, AddressSpaceElement*>::const_iterator elementIterator;
	m_elementsJanitor.lock();
	elementIterator = m_elements.find(anElement->getUserData());

	if (elementIterator != m_elements.end())
	{
		//  a duplicate found; drop it
		m_elementsJanitor.unlock();
		return FALSE;
	}   //  end if

	m_elements.insert(std::pair<unsigned long, AddressSpaceElement*>(anElement->getUserData(), anElement));
	m_elementsJanitor.unlock();
	return TRUE;
}   //  end addElementToArray


//-----------------------------------------------------------------------------
//	getElementFromArray
//
AddressSpaceElement* AddressSpaceRoot::getElementFromArray(unsigned long anElementUserData)
{
	std::map<unsigned long, AddressSpaceElement*>::iterator elementIterator;
	m_elementsJanitor.lock();
	elementIterator = m_elements.find(anElementUserData);

	if (elementIterator != m_elements.end())
	{
		AddressSpaceElement* foundElement = elementIterator->second;
		m_elementsJanitor.unlock();
		return foundElement;
	}   //  end if

	m_elementsJanitor.unlock();
	return NULL;
}   //  end getElementFromArray


//-----------------------------------------------------------------------------
//	RemoveElementFromArray
//
BOOL AddressSpaceRoot::removeElementFromArray(AddressSpaceElement* anElement)
{
	return removeElementFromArray(anElement->getUserData());
}   //  end removeElementFromArray


//-----------------------------------------------------------------------------
//	removeElementFromArray
//
BOOL AddressSpaceRoot::removeElementFromArray(unsigned long anElementUserData)
{
	std::map<unsigned long, AddressSpaceElement*>::iterator elementIterator;
	m_elementsJanitor.lock();
	elementIterator = m_elements.find(anElementUserData);

	if (elementIterator != m_elements.end())
	{
		m_elements.erase(elementIterator);
		m_elementsJanitor.unlock();
		return TRUE;
	}   //  end if
	m_elementsJanitor.unlock();
	return FALSE;
}   //  end removeElementFromArray
