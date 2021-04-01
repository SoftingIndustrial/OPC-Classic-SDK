#include "OSCompat.h"

#include "ServerDaAddressSpaceElement.h"

#include "../ServerApplication.h"
#include "ServerDaProperty.h"

#include "OTServer.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
//	DA Address Space Element default constructor
//
DaAddressSpaceElement::DaAddressSpaceElement():
	m_accessRights(EnumAccessRights_READABLE),
	m_ioMode(EnumIoMode_NONE),
	m_datatype(VT_EMPTY),
	m_active(FALSE),
	m_updateRate(-1)
{
}   //  end ctr


//-----------------------------------------------------------------------------
//	A DA Element constructor
//
DaAddressSpaceElement::DaAddressSpaceElement(
	tstring anItemId,
	tstring aName,
	unsigned long anUserData,
	unsigned long anObjectHandle,
	unsigned long aParentHandle) :
	AddressSpaceElement(aName, anUserData, anObjectHandle, aParentHandle),
	m_itemId(anItemId),
	m_accessRights(EnumAccessRights_READABLE),
	m_ioMode(EnumIoMode_NONE),
	m_datatype(VT_EMPTY),
	m_active(FALSE),
	m_updateRate(-1)
{
}   //  end ctr


//-----------------------------------------------------------------------------
//	A DA Element constructor
//
DaAddressSpaceElement::~DaAddressSpaceElement()
{
}   //  end destructor


//-----------------------------------------------------------------------------
//	Adds a child to the current Address space element
//
BOOL DaAddressSpaceElement::addChild(AddressSpaceElement* aChild)
{
	if (!m_hasChildren)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("DaAddressSpaceElement::addChild"),
									   _T("this element does not allow children"));
		return FALSE;
	}   //  end if

	DaAddressSpaceElement* pChild = (DaAddressSpaceElement*)aChild;

	if (pChild == NULL)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("DaAddressSpaceElement::addChild"), _T("Invalid child provided"));
		return FALSE;
	}   //  end if

	DaAddressSpaceRoot* daRoot = Application::Instance()->getDaAddressSpaceRoot();

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

		data.m_accessRights = (unsigned char)pChild->getAccessRights();
		data.m_ioMode = (unsigned char)pChild->getIoMode();
		data.m_datatype = (unsigned short)pChild->getDatatype();
		data.m_hasChildren = (unsigned char)pChild->getHasChildren();
		data.m_isBrowsable = (unsigned char)pChild->getIsBrowsable();
		data.m_elementType = (unsigned char)EnumAddressSpaceElementType_DA;
		data.m_userData = pChild->getUserData();

		if (daRoot->addElementToArray(pChild))
		{
			unsigned long childObjectHandle = 0;
			result = OTSAddAddressSpaceElement(m_objectHandle, &data, &childObjectHandle);

			if (SUCCEEDED(result))
			{
				pChild->setObjectHandle(childObjectHandle);
			}   //  end if
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
			//  Odd situation, just log an error
			Application::Instance()->trace(EnumTraceLevel_ERR, EnumTraceGroup_OPCSERVER,
										   _T("DaAddressSpaceElement::addChild"), _T("RemoveElementFromArray has failed"));
			return FALSE;
		}   //  end if
	}
	else
	{
		daRoot->addElementToArray(pChild);
	}   //  end if

	return AddressSpaceElement::addChild(aChild);
}   //  end addChild


//-----------------------------------------------------------------------------
// Removes Child and if there, its children too
//
// returns:
// true     - Child removed
// false    - Child was not removed
//
BOOL DaAddressSpaceElement::removeChild(AddressSpaceElement* aChild)
{
	if (!m_hasChildren)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("DaAddressSpaceElement::removeChild"),
									   _T("this element does not allow children"));
		return FALSE;
	}   //  end if

	DaAddressSpaceElement* pDaChild = (DaAddressSpaceElement*)(aChild);

	if (pDaChild == NULL)
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("DaAddressSpaceElement::removeChild"), _T("Invalid child provided"));
		return FALSE;
	}   //  end if

	if (pDaChild->getHasChildren())
	{
		//  Remove the clildren of the child
		std::vector<AddressSpaceElement*> childrenList = pDaChild->getChildren();
		std::vector<AddressSpaceElement*>::iterator pos;

		for (pos = childrenList.begin(); pos != childrenList.end(); pos++)
		{
			AddressSpaceElement* element = *pos;

			if (element != NULL)
			{
				pDaChild->removeChild(element);
			}   //  end if
		}   //  end for
	}   //  end if

	long result = S_OK;
	AddressSpaceElement::removeChild(aChild);
	unsigned long childHandle = pDaChild->getObjectHandle();
	DaAddressSpaceRoot* daRoot = Application::Instance()->getDaAddressSpaceRoot();
	result = daRoot->removeElementFromArray(pDaChild);

	if (!SUCCEEDED(result))
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER,
									   _T("DaAddressSpaceElement::removeChild"), _T("removeElementFromArray has failed"));
	}   //  end if

	delete pDaChild;
	pDaChild = NULL;

	if (childHandle != 0)
	{
		result = OTSRemoveAddressSpaceElement(childHandle);
	}   //  end if

	if (!SUCCEEDED(result))
	{
		Application::Instance()->trace(EnumTraceLevel_WRN, EnumTraceGroup_OPCSERVER, _T("DaAddressSpaceElement::removeChild"),
									   _T("OTSRemoveAddressSpaceElement has failed"));
		return FALSE;
	}   //  end if

	return TRUE;
}   //  end removeChild


//-----------------------------------------------------------------------------
// Gets the last value in the element`s cache
//
long DaAddressSpaceElement::getCacheValue(ValueQT& aCacheValue)
{
	long result = E_FAIL;

	if (m_objectHandle != 0 && m_ioMode != EnumIoMode_NONE)
	{
		OTValueData valueData = {0};
		valueData.m_pValue = new VARIANT();
		result = OTSGetCacheValue(m_objectHandle, &valueData);

		if (SUCCEEDED(result))
		{
			aCacheValue.setData(*valueData.m_pValue, (EnumQuality)valueData.m_quality, DateTime(&valueData.m_timestamp));
		}   //  end if

		Variant::variantClear(valueData.m_pValue);
		delete valueData.m_pValue;
	}   //  end if

	return result;
}   //  end getCacheValue


//-----------------------------------------------------------------------------
// queryCacheValue
//
long DaAddressSpaceElement::queryCacheValue(DaSession* aSession, ValueQT& aCacheValue)
{
	return getCacheValue(aCacheValue);
}   //  end queryCacheValue

//-----------------------------------------------------------------------------
//	Returns namespace element`s properties
//	returns E_NOTIMPL - Must be overridden
//
long DaAddressSpaceElement::queryProperties(std::vector<DaProperty*>& aPropertyList)
{
	aPropertyList.clear();
	return E_NOTIMPL;
}   //  end queryProperties


//-----------------------------------------------------------------------------
// Changes value of namespace element
// - Invalid valueQT was passed
//
long DaAddressSpaceElement::valueChanged(ValueQT& aNewValue)
{
	OTValueData valueData;
	valueData.m_quality = (unsigned short)aNewValue.getQuality();
	valueData.m_timestamp = aNewValue.getTimeStamp().get();
	valueData.m_pValue = new VARIANT();
	aNewValue.getData().copyTo(valueData.m_pValue);
	long result = OTSValuesChanged(1, &m_objectHandle, &valueData);
	Variant::variantClear(valueData.m_pValue);
	delete valueData.m_pValue;
	return result;
}   //  end valueChanged


//-----------------------------------------------------------------------------
//	getParent
//	Returns the element`s parent
//
AddressSpaceElement* DaAddressSpaceElement::getParent(void)
{
	if (m_objectHandle == 0)
	{
		return NULL;
	}   //  end if

	OTObjectData parent;

	if (S_OK != OTSGetParent(m_objectHandle, &parent))
	{
		return NULL;
	}   //  end if

	DaAddressSpaceRoot* daRoot = Application::Instance()->getDaAddressSpaceRoot();

	if (daRoot == NULL)
	{
		return NULL;
	}   //  end if

	return daRoot->getElementFromArray(parent.m_userData);
}   //  end getParent


//-----------------------------------------------------------------------------
//	getChildren
//
//	-    Returns a list of children.
//		this list is empty in case of an error or if no children are there
//		to be delivered
//
std::vector<AddressSpaceElement*> DaAddressSpaceElement::getChildren(void)
{
	std::vector<AddressSpaceElement*>  children;
	OTSObjectData* ptrData = NULL;
	long count = 0;

	if (SUCCEEDED(OTSGetChildren(m_objectHandle, EnumAddressSpaceElementType_DA, &count, &ptrData)))
	{
		DaAddressSpaceRoot* daRoot = Application::Instance()->getDaAddressSpaceRoot();
		AddressSpaceElement* element = NULL;

		if (daRoot != NULL)
		{
			daRoot->m_elementsJanitor.lock();

			for (int i = 0; i < count; i++)
			{
				element = daRoot->getElementFromArray(ptrData[i].m_userData);
				if (element != NULL)
				{
					children.push_back(element);
				}   //  end if
			}   //  end for

			daRoot->m_elementsJanitor.unlock();
		}

		OTFreeMemory(ptrData);
	}   //  end if

	return children;
}   //  end getChildren


//-----------------------------------------------------------------------------
//	DaAddressSpaceRoot constructor
//
DaAddressSpaceRoot::DaAddressSpaceRoot(EnumAddressSpaceType anAddressSpaceType):
	AddressSpaceRoot(anAddressSpaceType,
					 new DaAddressSpaceElement(tstring(_T("DaRoot")),
											   tstring(_T("DaRoot")), 0, 0, 0))
{
}   //  end constructor


//-----------------------------------------------------------------------------
//	DaAddressSpaceRoot constructor
//
DaAddressSpaceRoot::DaAddressSpaceRoot():
	AddressSpaceRoot(EnumAddressSpaceType_OBJECT_STRING_BASED,
					 new DaAddressSpaceElement(tstring(_T("DaRoot")),
											   tstring(_T("DaRoot")), 0, 0, 0))
{
}   //  end constructor


//-----------------------------------------------------------------------------
//	DaAddressSpaceRoot destructor
//
DaAddressSpaceRoot::~DaAddressSpaceRoot()
{
	m_elementsJanitor.lock();
	if (!m_elements.empty())
	{
		Application::Instance()->trace(EnumTraceLevel_INF, EnumTraceGroup_OPCSERVER, _T("~DaAddressSpaceRoot"),
									   _T("elements not empty"));
	}   //  end if
	m_elementsJanitor.unlock();
}   //  destructor


//-----------------------------------------------------------------------------
//	Returns namespace element`s properties
//
long DaAddressSpaceRoot::queryAddressSpaceElementProperties(
	const tstring& anElementID,
	std::vector<DaProperty*>& aPropertyList)
{
	aPropertyList.clear();
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementProperties


//-----------------------------------------------------------------------------
// Returns the string based address space element
//
long DaAddressSpaceRoot::queryAddressSpaceElementData(
	const tstring& anElementId,
	AddressSpaceElement*& anElement)
{
	anElement = NULL;
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementData


//-----------------------------------------------------------------------------
// Returns the string based address space element (extended)
//
long DaAddressSpaceRoot::queryAddressSpaceElementDataEx(
	const tstring& anElementId,
	EnumAddressSpaceElementData& elementDataFilter,
	AddressSpaceElement*& anElement)
{
	anElement = NULL;
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementDataEx


//-----------------------------------------------------------------------------
//	Returns the list of children the of the string based addess space element.
//	returns E_NOTIMPL - Must be overridden to support string based addess
//	space
//
long DaAddressSpaceRoot::queryAddressSpaceElementChildren(
	const tstring& anElementId,
	std::vector<AddressSpaceElement*>& aChildrenList)
{
	aChildrenList.clear();
	return E_NOTIMPL;
}   //  end queryAddressSpaceElementChildren


//-----------------------------------------------------------------------------
//	If the values of some addressspace elements have been changed this
//	function notifies the clients
//
long DaAddressSpaceRoot::valuesChanged(
	std::vector<DaAddressSpaceElement*>& anElementList,
	std::vector<ValueQT*>& aValueList)
{
	if (anElementList.size() != aValueList.size())
	{
		return EnumResultCode_E_INVALIDARG;
	}   //  end if

	size_t count = anElementList.size();

	if (count == 0)
	{
		return S_FALSE;
	}   //  end if

	OTValueData* values = new OTValueData[count];
	unsigned long* objects = new unsigned long[count];

	for (size_t i = 0; i < count; i++)
	{
		objects[i] = anElementList[i]->getObjectHandle();
		values[i].m_quality     = (unsigned char)aValueList[i]->getQuality();
		values[i].m_timestamp   = aValueList[i]->getTimeStamp().get();
		values[i].m_pValue = new VARIANT();
		aValueList[i]->getData().copyTo(values[i].m_pValue);
	}   //  end for

	long result = OTSValuesChanged((long)count, objects, values);

	if (values != NULL)
	{
		for (size_t i = 0; i < count; i++)
		{
			if (values[i].m_pValue != NULL)
			{
				Variant::variantClear(values[i].m_pValue);
				delete values[i].m_pValue;
				values[i].m_pValue = NULL;
			}   //  end if
		}   //  end for

		delete[] values;
	}   //  end if

	if (objects != NULL)
	{
		delete[] objects;
	}   //  end if

	return result;
}   //  end valuesChanged


long SoftingOPCToolboxServer::DaAddressSpaceElement::setEUInfoAnalog(
	double lowEULimit,
	double highEULimit)
{
	long result =  OTSSetEUInfo(m_objectHandle, FALSE, 0, NULL, lowEULimit, highEULimit);
	return result;
}

long SoftingOPCToolboxServer::DaAddressSpaceElement::setEUInfoEnumerated(
	std::vector<tstring> enumeratedValues)
{
	unsigned long enumValuesCount = (unsigned long)enumeratedValues.size();
	OTChar** pEnumeratedValues = new OTChar*[enumValuesCount];
	std::vector<tstring>::const_iterator enumValuesIterator;
	unsigned long i;

	for (enumValuesIterator = enumeratedValues.begin(), i = 0; enumValuesIterator != enumeratedValues.end() ; enumValuesIterator++, i++)
	{
#ifdef TBC_OS_WINDOWS
		pEnumeratedValues[i] = _tcsdup((*enumValuesIterator).c_str());
#endif
#ifdef TBC_OS_LINUX
		pEnumeratedValues[i] = strdup((*enumValuesIterator).c_str());
#endif
	} //end for

	long result = OTSSetEUInfo(m_objectHandle, TRUE, enumValuesCount, pEnumeratedValues, 0, 0);
	for (i = 0; i < enumValuesCount; ++i)
	{
		delete pEnumeratedValues[i];
	}
	delete[] pEnumeratedValues;
	return result;
}
