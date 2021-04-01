#ifndef _MYDAADDRESSSPACEROOT_H_
#define _MYDAADDRESSSPACEROOT_H_

#include "Da/ServerDaAddressSpaceElement.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

extern const std::string g_stringBasedNodeName;
extern const unsigned int g_stringBasedItemCount;
extern const TCHAR g_stringBasedItemFullPath[256];
extern const TCHAR g_stringBasedItemNameFormat[16];
extern const TCHAR g_stringBasedItemName[256];
extern const byte g_stringBasedItemNameFormatRadix;
extern const unsigned int g_stringBasedItemDigits;
extern TCHAR g_namespaceDelimiter[2];


//-----------------------------------------------------------------------------
// MyDaAddressSpaceRoot
//-----------------------------------------------------------------------------
class MyDaAddressSpaceRoot : public DaAddressSpaceRoot
{
public:
	MyDaAddressSpaceRoot()
	{
	}

	virtual ~MyDaAddressSpaceRoot()
	{
	}

	long queryAddressSpaceElementData(
		const tstring& anElementID,
		AddressSpaceElement*& anElement)
	{
		static MyDaAddressSpaceElement* currentElement;
		currentElement = new MyDaAddressSpaceElement();
		static tstring path = tstring(g_stringBasedItemFullPath);
		if (path.find(anElementID) != tstring::npos)
		{
			currentElement->setName(tstring(getCurrentNodeName(anElementID.c_str())));
			currentElement->setIoMode(EnumIoMode_NONE);
			currentElement->setHasChildren(TRUE);
		}
		else if ((anElementID.find(g_stringBasedItemFullPath) == 0) && (anElementID.find(g_stringBasedItemName) != tstring::npos))
		{
			unsigned int itemNo = (unsigned int) _tcstoul(anElementID.c_str() + anElementID.length() - g_stringBasedItemDigits, NULL, g_stringBasedItemNameFormatRadix);
			currentElement->setName(tstring(getCurrentNodeName(anElementID.c_str())));
			currentElement->setIoMode(EnumIoMode_POLL);
			currentElement->setHasChildren(FALSE);
			currentElement->setRandomDatatype(itemNo);
			currentElement->setAccessRights(EnumAccessRights_READWRITEABLE);
		}
		if (currentElement->getName().length() > 0)
		{
			anElement = currentElement;
			return EnumResultCode_S_OK;
		}
		delete currentElement;
		currentElement = NULL;
		anElement = NULL;
		return EnumResultCode_E_OPC_INVALIDITEMID;
	}   //  end QueryAddressSpaceElementData

	long queryAddressSpaceElementChildren(
		const tstring& anElementId,
		std::vector<AddressSpaceElement*>& aChildrenList)
	{
		static MyDaAddressSpaceElement* currentChild;
		static TCHAR buffer[256] = {0};
		static TCHAR l_format[256] = {0};
		_stprintf_s(l_format, _T("%s%s%s"), _T("%s"), g_namespaceDelimiter, g_stringBasedItemNameFormat);
		if (g_stringBasedNodeName.length() > 0)
		{
			if (anElementId.compare(g_stringBasedNodeName) == 0)
			{
				_tprintf(_T("Creating string-based children...\n"));
				for (unsigned int i = 0; i < g_stringBasedItemCount; ++i)
				{
					if (i % 100 == 0)
					{
						_tprintf(_T("\r"));
						_tprintf(_T("%3.2f%%"), ((double)(i * 100) / g_stringBasedItemCount));
					}
					currentChild = new MyDaAddressSpaceElement();
					_stprintf_s(buffer, g_stringBasedItemNameFormat, g_stringBasedItemName, i);
					currentChild->setName(tstring(buffer));
					_stprintf_s(buffer, l_format, g_stringBasedItemFullPath, g_stringBasedItemName, i);
					currentChild->setItemId(tstring(buffer));
					currentChild->setAccessRights(EnumAccessRights_READWRITEABLE);
					currentChild->setRandomDatatype(i);
					currentChild->setIoMode(EnumIoMode_REPORT);
					currentChild->setHasChildren(FALSE);
					aChildrenList.push_back(currentChild);
				}
				_tprintf(_T("\r 100%%"));
				_tprintf(_T("\n...string-based children created.\n"));
			}
		}
		return EnumResultCode_S_OK;
	} // end queryAddressSpaceElementChildren


	long queryAddressSpaceElementProperties(
		const tstring& anElementID,
		std::vector<DaProperty*>& aPropertyList)
	{
		return EnumResultCode_S_OK;
	} // end queryAddressSpaceElementProperties

private:
	const TCHAR* getCurrentNodeName(const TCHAR* fullName)
	{
		static TCHAR ret[256] = {0};
		static unsigned int i = 0;
		static const char* pos = 0;
		unsigned int offset = 0;
		pos = _tcsstr(fullName, g_namespaceDelimiter);
		if (pos == NULL)
		{
			return fullName;
		}
		while (pos)
		{
			offset = pos - fullName + 1;
			pos = _tcsstr(fullName + offset, g_namespaceDelimiter);
		}
		_tcscpy_s(ret, fullName + offset);
		return ret;
	}

};  //  end MyDaAddressSpaceRoot

#endif
