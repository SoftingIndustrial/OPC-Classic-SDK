#include "OSCompat.h"
#include "ServerAeAddressSpaceElement.h"

#include "../ServerApplication.h"
#include "OTServer.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	AeAddressSpaceElement    - default constructor
//
AeAddressSpaceElement::AeAddressSpaceElement(void)
{
}   //  end ctr


//-----------------------------------------------------------------------------
//	AeAddressSpaceElement    -  constructor
//
AeAddressSpaceElement::AeAddressSpaceElement(
	tstring aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle) :
	AddressSpaceElement(aName, anUserData, anObjectHandle, aParentHandle)
{
}   //  end ctr


//-----------------------------------------------------------------------------
//	AeAddressSpaceElement    - destructor
//
AeAddressSpaceElement::~AeAddressSpaceElement(void)
{
}   //  end destructor


//-----------------------------------------------------------------------------
//	Adds a child to the current Address space element
//
BOOL AeAddressSpaceElement::addChild(AddressSpaceElement* aChild)
{
	if (!m_hasChildren)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("AeAddressSpaceElement::addChild"),
									   _T("this element does not allow children"));
		return FALSE;
	}   //  end if

	AeAddressSpaceElement* pChild = (AeAddressSpaceElement*)aChild;

	if (pChild == NULL)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("AeAddressSpaceElement::addChild"), _T("Invalid child provided"));
		return FALSE;
	}   //  end if

	AeAddressSpaceRoot* aeRoot = Application::Instance()->getAeAddressSpaceRoot();

	//  only add the child if no handle assigned
	if (pChild->getObjectHandle() == 0)
	{
		long result = E_FAIL;
		OTSAddressSpaceElementData data;
		data.m_name = (OTChar*)OTAllocateMemory((unsigned long)(pChild->getName().size() + 1) * sizeof(OTChar));

		if (data.m_name != NULL)
		{
#ifdef TBC_OS_WINDOWS
			_tcscpy(data.m_name, pChild->getName().c_str());
#endif
#ifdef TBC_OS_LINUX
			strcpy(data.m_name, pChild->getName().c_str());
#endif
		}

		tstring itemId = m_name + Application::Instance()->getAddressSpaceDelimiter() + pChild->getName();
		data.m_itemID = (OTChar*)OTAllocateMemory((unsigned long)(itemId.size() + 1) * sizeof(OTChar));

		if (data.m_itemID != NULL)
		{
#ifdef TBC_OS_WINDOWS
			_tcscpy(data.m_itemID, itemId.c_str());
#endif
#ifdef TBC_OS_LINUX
			strcpy(data.m_itemID, itemId.c_str());
#endif
		}

		data.m_hasChildren = (unsigned char)pChild->getHasChildren();
		data.m_isBrowsable = (unsigned char)pChild->getIsBrowsable();
		data.m_elementType = (unsigned char)EnumAddressSpaceElementType_AE;
		data.m_userData = pChild->getUserData();

		if (aeRoot->addElementToArray(pChild))
		{
			unsigned long childObjectHandle = 0;
			result = OTSAddAddressSpaceElement(m_objectHandle, &data, &childObjectHandle);
			pChild->setObjectHandle(childObjectHandle);
		}   //  end if

		//  release the memory allocated
		if (data.m_name != NULL)
		{
			OTFreeMemory(data.m_name);
			data.m_name = NULL;
		}   //  end if

		if (data.m_itemID != NULL)
		{
			OTFreeMemory(data.m_itemID);
			data.m_itemID = NULL;
		}   //  end if

		if (!SUCCEEDED(result))
		{
			result = aeRoot->removeElementFromArray(aChild);

			if (!SUCCEEDED(result))
			{
				//  Odd situation, just log an error
				Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
											   _T("AeAddressSpaceElement::addChild"), _T("RemoveElementFromArray has failed"));
			}   //  end if

			delete aChild;
			return FALSE;
		}   //  end if ... else
	}
	else
	{
		aeRoot->addElementToArray(pChild);
	}   //  end if ... else

	return AddressSpaceElement::addChild(aChild);
}   //  end addChild


//-----------------------------------------------------------------------------
// Removes Child and if there, its children too
//
// returns:
// true     - Child removed
// false    - Child was not removed
BOOL AeAddressSpaceElement::removeChild(AddressSpaceElement* aChild)
{
	if (!m_hasChildren)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("AeAddressSpaceElement::removeChild"),
									   _T("this element does not allow children"));
		return FALSE;
	}   //  end if

	AeAddressSpaceElement* pAeChild = (AeAddressSpaceElement*)(aChild);

	if (pAeChild == NULL)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("AeAddressSpaceElement::removeChild"), _T("Invalid child provided"));
		return FALSE;
	}   //  end if

	if (pAeChild->getHasChildren())
	{
		//  Remove the clildren of the child
		std::vector<AddressSpaceElement*> childrenList = pAeChild->getChildren();
		std::vector<AddressSpaceElement*>::iterator pos;

		for (pos = childrenList.begin(); pos != childrenList.end(); pos++)
		{
			AddressSpaceElement* element = *pos;

			if (element != NULL)
			{
				pAeChild->removeChild(element);
			}   //  end if
		}   //  end for
	}   //  end if

	long result = S_OK;
	AddressSpaceElement::removeChild(aChild);
	unsigned long childHandle = pAeChild->getObjectHandle();
	AeAddressSpaceRoot* aeRoot = Application::Instance()->getAeAddressSpaceRoot();
	result = aeRoot->removeElementFromArray(pAeChild);

	if (!SUCCEEDED(result))
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("AeAddressSpaceElement::removeChild"), _T("removeElementFromArray has failed"));
	}   //  end if

	delete pAeChild;
	pAeChild = NULL;

	if (childHandle != 0)
	{
		result = OTSRemoveAddressSpaceElement(childHandle);
	}   //  end if

	if (!SUCCEEDED(result))
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER, _T("AeAddressSpaceElement::removeChild"),
									   _T("OTSRemoveAddressSpaceElement has failed"));
		return FALSE;
	}   //  end if

	return TRUE;
}   //  end removeChild


//-----------------------------------------------------------------------------
//	getParent
//
//		-   Returns the element`s parent
//
AddressSpaceElement* AeAddressSpaceElement::getParent(void)
{
	if (m_objectHandle == 0)
	{
		return NULL;
	}   //  end if

	OTObjectData parent;

	if (S_OK != OTSGetParent(m_objectHandle, &parent))
	{
		return NULL;
	}//end if

	AeAddressSpaceRoot* aeRoot = Application::Instance()->getAeAddressSpaceRoot();

	if (aeRoot == NULL)
	{
		return NULL;
	}   //  end if

	return aeRoot->getElementFromArray(parent.m_userData);
}   //  end getParent


//-----------------------------------------------------------------------------
//	getChildren
//
//	-    Returns a list of children.
//		this list is empty in case of an error or if no children are there
//		to be delivered
//
std::vector<AddressSpaceElement*> AeAddressSpaceElement::getChildren(void)
{
	std::vector<AddressSpaceElement*> children;
	OTSObjectData* ptrData = NULL;
	long count = 0;

	if (SUCCEEDED(OTSGetChildren(m_objectHandle, EnumAddressSpaceElementType_AE, &count, &ptrData)))
	{
		AeAddressSpaceRoot* aeRoot = Application::Instance()->getAeAddressSpaceRoot();
		AddressSpaceElement* element = NULL;

		if (aeRoot != NULL)
		{
			aeRoot->m_elementsJanitor.lock();

			for (int i = 0; i < count; i++)
			{
				element = aeRoot->getElementFromArray(ptrData[i].m_userData);
				if (element != NULL)
				{
					children.push_back(element);
				}   //  end if
			}   //  end for

			aeRoot->m_elementsJanitor.unlock();
		}

		OTFreeMemory(ptrData);
	}   //  end if

	return children;
}   //  end getChildren


//-----------------------------------------------------------------------------
//	queryConditions
//
//	-    used to return the list of connditions names of the current element for
//		for a specific path
//
long AeAddressSpaceElement::queryConditions(
	tstring& aSourcePath,
	std::vector<tstring>& aConditionNameList)
{
	return E_NOTIMPL;
}   //  end queryConditions


//-----------------------------------------------------------------------------
//	AeAddressSpaceRoot constructor
//
AeAddressSpaceRoot::AeAddressSpaceRoot(EnumAddressSpaceType anAddressSpaceType):
	AddressSpaceRoot(anAddressSpaceType,
					 new AeAddressSpaceElement(tstring(_T("AeRoot")), 0, 0, 0))
{
}   //  end constructor


//-----------------------------------------------------------------------------
//	AeAddressSpaceRoot constructor
//
AeAddressSpaceRoot::AeAddressSpaceRoot(void):
	AddressSpaceRoot(EnumAddressSpaceType_OBJECT_STRING_BASED,
					 new AeAddressSpaceElement(tstring(_T("AeRoot")),
											   0, 0, 0))
{
}   //  end constructor


//-----------------------------------------------------------------------------
//	AeAddressSpaceRoot destructor
//
AeAddressSpaceRoot::~AeAddressSpaceRoot(void)
{
	m_elementsJanitor.lock();
	if (!m_elements.empty())
	{
		Application::Instance()->trace(EnumTraceLevel_INF, EnumTraceGroup_OPCSERVER, _T("~AeAddressSpaceRoot"),
									   _T("elements vector not empty"));
	}   //  end if
	m_elementsJanitor.unlock();
}   //  destructor


//-----------------------------------------------------------------------------
// Returns the string based adderss space element
//
long AeAddressSpaceRoot::queryAddressSpaceElementData(
	const tstring& anElementId,
	AddressSpaceElement*& anElement)
{
	anElement = NULL;
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementData


//-----------------------------------------------------------------------------
// Returns the string based adderss space element (extended)
//
long AeAddressSpaceRoot::queryAddressSpaceElementDataEx(
	const tstring& anElementId,
	EnumAddressSpaceElementData& elementDataFilter,
	AddressSpaceElement*& anElement)
{
	anElement = NULL;
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementDataEx


//-----------------------------------------------------------------------------
//	queryAddressSpaceElementChildren
//
//		-   Returns the list of children the of the string based addess space
//			element. returns E_NOTIMPL - Must be overridden to support string
//			based addess space
//
long AeAddressSpaceRoot::queryAddressSpaceElementChildren(
	const tstring& anElementId,
	std::vector<AddressSpaceElement*>& aChildrenList)
{
	aChildrenList.clear();
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementChildren


//-----------------------------------------------------------------------------
// QueryConditions
//
long AeAddressSpaceRoot::queryConditions(
	unsigned long anElementUserData,
	tstring& aSourcePath,
	std::vector<tstring>& aConditionNames)
{
	m_elementsJanitor.lock();
	AeAddressSpaceElement* element = (AeAddressSpaceElement*)getElementFromArray(anElementUserData);

	if (element == NULL)
	{
		aConditionNames.clear();
		m_elementsJanitor.unlock();
		return S_FALSE;
	}   //  end if

	long result = element->queryConditions(aSourcePath, aConditionNames);
	m_elementsJanitor.unlock();
	return result;
}   //  end queryConditions
