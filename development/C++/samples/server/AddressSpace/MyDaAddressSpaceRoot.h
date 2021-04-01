#ifndef _MYDAADDRESSSPACEROOT_H_
#define _MYDAADDRESSSPACEROOT_H_

#include "Da/ServerDaAddressSpaceElement.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

//-----------------------------------------------------------------------------
// MyDaAddressSpaceRoot
//-----------------------------------------------------------------------------
class MyDaAddressSpaceRoot : public DaAddressSpaceRoot
{

private:
	DaProperty propType1;
	DaProperty propType2;

public:

	MyDaAddressSpaceRoot()
	{
		propType1.setId(5555);
		propType1.setAccessRights(EnumAccessRights_READABLE);
		propType1.setDatatype(VT_I2);
		propType1.setName(tstring(_T("Type")));
		propType1.setDescription(tstring(_T("Object Type")));

		propType2.setId(5556);
		propType2.setAccessRights(EnumAccessRights_READABLE);
		propType2.setDatatype(VT_I2);
		propType2.setName(tstring(_T("Type2")));
		propType2.setDescription(tstring(_T("Object Type2")));
	}

	virtual ~MyDaAddressSpaceRoot()
	{
	}

	long queryAddressSpaceElementDataEx(
		const tstring& anElementID,
		EnumAddressSpaceElementData& elementDataFilter,
		AddressSpaceElement*& anElement)
	{
		MyDaAddressSpaceElement* element = new MyDaAddressSpaceElement();
		tstring name;
		long result = EnumResultCode_S_OK;

		// This is the extended version of the method queryAddressSpaceElementData. The Toolkit CORE will call this
		// method (queryAddressSpaceElementDataEx) prior to calling the non-extended method (queryAddressSpaceElementData)
		// and will only call the non-extended method if this method returns a bad EnumResultCode.
		// This method allows the vendor application to avoid collecting and providing to the Toolkit CORE all data for
		// an Address Space Element by indicating what data is requested through the additional elementDataFilter parameter.
		// Additional data may be reported but in order for the Toolkit CORE to cache the additionally reported data, the
		// corresponding additional flags must be set into elementDataFilter.

		// NOTE: This is an easy sample and will not provide all the possible EnumAddressSpaceElementData the
		//       Toolkit CORE requests, but only the minimum amount of data needed for creating the AddressSpaceElement.

		if (anElementID.find(_T("stringBased")) == 0)
		{
			//starts with stringBased
			if (anElementID == _T("stringBased"))
			{
				//equals with stringBased

				// check what data is requested by Toolkit CORE returned.
				// more than 1 data may be requested at once.

				// NOTE: only the requested data will be returned here.
				if (elementDataFilter & EnumElementData_NAME)
				{
					element->setName(tstring(_T("stringBased")));
				}

				if (elementDataFilter & EnumElementData_HASCHILDREN)
				{
					element->setHasChildren(TRUE);
				}

				if (elementDataFilter & EnumElementData_IOMODE)
				{
					element->setIoMode(EnumIoMode_NONE);
				}
			} // end if
			else if (anElementID == _T("stringBased.N1"))
			{
				//equals with stringBased.N1

				// check what data is requested by Toolkit CORE returned.

				// NOTE: only the requested data will be returned here as well.
				if (elementDataFilter & EnumElementData_NAME)
				{
					element->setName(tstring(_T("N1")));
				}

				if (elementDataFilter & EnumElementData_ACCESSRIGHTS)
				{
					element->setAccessRights(EnumAccessRights_READWRITEABLE);
				}

				if (elementDataFilter & EnumElementData_DATATYPE)
				{
					element->setDatatype(VT_BSTR);
				}

				if (elementDataFilter & EnumElementData_HASCHILDREN)
				{
					element->setHasChildren(TRUE);
				}

				if (elementDataFilter & EnumElementData_IOMODE)
				{
					element->setIoMode(EnumIoMode_REPORT);
				}
			} // end if
			else if (anElementID == _T("stringBased.N2"))
			{
				//equals with stringBased.N2

				// check what data is requested by Toolkit CORE returned.

				// NOTE: besides the requested data, additional data will be provided
				//       (e.g. underlaying system provides required data in groups)

				if (elementDataFilter & EnumElementData_NAME)
				{
					element->setName(tstring(_T("N2")));
				}

				if (elementDataFilter & EnumElementData_ACCESSRIGHTS)
				{
					element->setAccessRights(EnumAccessRights_READWRITEABLE);

					// e.g. underlaying system also provides element name when reading access rights
					element->setName(tstring(_T("N2")));
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME);
				}

				if ((elementDataFilter & EnumElementData_DATATYPE) || 
					(elementDataFilter & EnumElementData_HASCHILDREN) || 
					(elementDataFilter & EnumElementData_IOMODE))
				{
					// e.g. underlaying system provides all the dataType, hasChildren and ioMode at once, can't read them separately
					element->setDatatype(VT_BSTR);
					element->setHasChildren(TRUE);
					element->setIoMode(EnumIoMode_REPORT);
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_DATATYPE | EnumElementData_HASCHILDREN | EnumElementData_IOMODE);
				}
			} // end if
			else if (anElementID == _T("stringBased.T1"))
			{
				//equals with stringBased.T1

				// check what data is requested by Toolkit CORE returned.

				// NOTE: besides the requested data, additional data will be provided
				//       (e.g. underlaying system provides required data in groups)

				if ((elementDataFilter & EnumElementData_NAME) ||
					(elementDataFilter & EnumElementData_HASCHILDREN))
				{
					// e.g. underlying system provides both element name and whether the element is a node or not
					element->setName(tstring(_T("T1")));
					element->setHasChildren(false);
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME | EnumElementData_HASCHILDREN);
				}

				if (elementDataFilter & EnumElementData_ACCESSRIGHTS)
				{
					element->setAccessRights(EnumAccessRights_READWRITEABLE);
				}

				if (elementDataFilter & EnumElementData_DATATYPE)
				{
					element->setDatatype(VT_I1);
				}

				if (elementDataFilter & EnumElementData_IOMODE)
				{
					element->setIoMode(EnumIoMode_REPORT);
				}
			} // end if
			else if (anElementID == _T("stringBased.N1.T1"))
			{
				//equals with stringBased.N1.T1

				// check what data is requested by Toolkit CORE returned.

				// NOTE: besides the requested data, additional data will be provided
				//       (e.g. underlaying system provides required data in groups)

				if (elementDataFilter & EnumElementData_NAME)
				{
					element->setName(tstring(_T("T1")));
				}

				if (elementDataFilter & EnumElementData_ACCESSRIGHTS)
				{
					element->setAccessRights(EnumAccessRights_READWRITEABLE);
				}

				if (elementDataFilter & EnumElementData_DATATYPE)
				{
					// e.g. underlaying system also reports the access rights when reading data type
					element->setDatatype(VT_I1);
					element->setAccessRights(EnumAccessRights_READWRITEABLE);
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_ACCESSRIGHTS);
				}

				if (elementDataFilter & EnumElementData_HASCHILDREN)
				{
					// e.g. underlaying system also reports the item name when identifying if the elements is a branch or a leaf
					element->setHasChildren(FALSE);
					element->setName(tstring(_T("T1")));
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME);
				}

				if (elementDataFilter & EnumElementData_IOMODE)
				{
					// e.g. underlaying system also reports the item data type and access rights when reading io mode
					element->setIoMode(EnumIoMode_REPORT);
					element->setAccessRights(EnumAccessRights_READWRITEABLE);
					element->setDatatype(VT_I1);
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_ACCESSRIGHTS | EnumElementData_DATATYPE);
				}
			} // end if
			else if (anElementID == _T("stringBased.N2.T1"))
			{
				//equals with stringBased.N2.T1

				// NOTE: all data will be provided for this node by the underlying system, no need to check what is asked
				element->setName(tstring(_T("T1")));
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(FALSE);
				element->setIoMode(EnumIoMode_REPORT);
				elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME | EnumElementData_ACCESSRIGHTS
					| EnumElementData_DATATYPE | EnumElementData_HASCHILDREN | EnumElementData_IOMODE);
			} // end if
			else if (anElementID == _T("stringBased.N2.T2"))
			{
				//equals with stringBased.N2.T2

				// e.g. let's have this element data be filled by the queryAddressSpaceElementData method
				result = EnumResultCode_E_OPC_BADTYPE;

				/*
				// e.g. or this block may be uncommented and therefore the data handled here
				//      (please first comment out the "result = ..." above)
				element->setName(tstring(_T("T2")));
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(FALSE);
				element->setIoMode(EnumIoMode_REPORT);
				*/
			} // end if
			else
			{
				result = EnumResultCode_E_OPC_BADTYPE;
			}
		}
		else
		{
			// e.g. above section provides a good example for how to use this method.
			//      below all data will be provided at once
			if (anElementID.find(_T("syntaxBased")) == 0)
			{
				//starts with syntaxBased
				if (anElementID == _T("syntaxBased"))
				{
					element->setName(tstring(_T("stringBased")));
					element->setIoMode(EnumIoMode_NONE);
					element->setHasChildren(true);
					elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME | EnumElementData_HASCHILDREN | EnumElementData_IOMODE);
				} // end if
				else
				{
					size_t index = anElementID.rfind(_T(".T"));

					if (index != tstring::npos)
					{
						element->setName(tstring(anElementID.substr(index + 1)));

						if (element->getName().length() == 2)
						{
							element->setHasChildren(FALSE);
							element->setIoMode(EnumIoMode_POLL);
							element->setAccessRights(EnumAccessRights_READWRITEABLE);
							element->setDatatype(VT_I4);
							elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME | EnumElementData_ACCESSRIGHTS
								| EnumElementData_DATATYPE | EnumElementData_HASCHILDREN | EnumElementData_IOMODE);
						}
						else
						{
							result = EnumResultCode_E_OPC_BADTYPE;
						} // end if .. else
					}
					else
					{
						index = anElementID.rfind(_T(".N"));

						if (index != tstring::npos)
						{
							element->setName(tstring(anElementID.substr(index + 1)));

							if (element->getName().length() == 2)
							{
								element->setHasChildren(TRUE);
								element->setIoMode(EnumIoMode_NONE);
								elementDataFilter = (EnumAddressSpaceElementData)(elementDataFilter | EnumElementData_NAME | EnumElementData_HASCHILDREN | EnumElementData_IOMODE);
							}
							else
							{
								result = EnumResultCode_E_OPC_BADTYPE;
							} // end if .. else
						} // end if
						else
						{
							result = EnumResultCode_E_OPC_BADTYPE;
						} // end if .. else
					} // end if .. else
				} // end if .. else
			} // end if
		} // end if .. else

		//  check if an element was succeeded
		if (result == EnumResultCode_E_OPC_BADTYPE)
		{
			delete element;
			element = NULL;
		}   //  end if

		anElement = element;
		return result;
	}   //  end QueryAddressSpaceElementDataEx

	long queryAddressSpaceElementData(
		const tstring& anElementID,
		AddressSpaceElement*& anElement)
	{
		MyDaAddressSpaceElement* element = new MyDaAddressSpaceElement();
		tstring name;

		if (anElementID.find(_T("stringBased")) == 0)
		{
			//starts with stringBased
			if (anElementID == _T("stringBased"))
			{
				//equals with stringBased
				name = _T("stringBased");
				element->setName(name);
				element->setIoMode(EnumIoMode_NONE);
				element->setHasChildren(TRUE);
			} // end if

			if (anElementID == _T("stringBased.N1"))
			{
				//equals with stringBased.N1
				name = _T("N1");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_BSTR);
				element->setHasChildren(TRUE);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if

			if (anElementID == _T("stringBased.N2"))
			{
				//equals with stringBased.N2
				name = _T("N2");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_BSTR);
				element->setHasChildren(TRUE);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if

			if (anElementID == _T("stringBased.T1"))
			{
				//equals with stringBased.T1
				name = _T("T1");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(false);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if

			if (anElementID == _T("stringBased.N1.T1"))
			{
				//equals with stringBased.N1.T1
				name = _T("T1");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(FALSE);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if

			if (anElementID == _T("stringBased.N2.T1"))
			{
				//equals with stringBased.N2.T1
				name = _T("T1");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(FALSE);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if

			if (anElementID == _T("stringBased.N2.T2"))
			{
				//equals with stringBased.N2.T2
				name = _T("T2");
				element->setName(name);
				element->setAccessRights(EnumAccessRights_READWRITEABLE);
				element->setDatatype(VT_I1);
				element->setHasChildren(FALSE);
				element->setIoMode(EnumIoMode_REPORT);
			} // end if
		}
		else
		{
			if (anElementID.find(_T("syntaxBased")) == 0)
			{
				//starts with syntaxBased
				if (anElementID == _T("syntaxBased"))
				{
					name = _T("stringBased");
					element->setName(name);
					element->setIoMode(EnumIoMode_NONE);
					element->setHasChildren(true);
				} // end if

				size_t index = anElementID.rfind(_T(".T"));

				if (index != tstring::npos)
				{
					name = anElementID.substr(index + 1);
					element->setName(name);

					if (element->getName().length() == 2)
					{
						element->setHasChildren(FALSE);
						element->setIoMode(EnumIoMode_POLL);
						element->setAccessRights(EnumAccessRights_READWRITEABLE);
						element->setDatatype(VT_I4);
					}
					else
					{
						name = _T("");
						element->setName(name);
					} // end if .. else
				}
				else
				{
					index = anElementID.rfind(_T(".N"));

					if (index != tstring::npos)
					{
						name = anElementID.substr(index + 1);
						element->setName(name);

						if (element->getName().length() == 2)
						{
							element->setHasChildren(TRUE);
							element->setIoMode(EnumIoMode_NONE);
						}
						else
						{
							name = _T("");
							element->setName(name);
						} // end if .. else
					} // end if
				} // end if .. else
			} // end if
		} // end if .. else

		//  check if an element was succeeded
		if (element->getName() == _T(""))
		{
			delete element;
			element = NULL;
			anElement = NULL;
			return EnumResultCode_E_OPC_BADTYPE;
		}   //  end if

		anElement = element;
		return EnumResultCode_S_OK;
	}   //  end QueryAddressSpaceElementData

	long queryAddressSpaceElementChildren(
		const tstring& anElementId,
		std::vector<AddressSpaceElement*>& aChildrenList)
	{
		MyDaAddressSpaceElement* child = NULL;
		tstring name;

		if (anElementId.length() == 0)
		{
			child = new MyDaAddressSpaceElement();
			name = _T("stringBased");
			child->setName(name);
			child->setIoMode(EnumIoMode_NONE);
			child->setItemId(name);
			child->setHasChildren(TRUE);
			aChildrenList.push_back(child);
		}
		else
		{
			if (anElementId == _T("stringBased"))
			{
				child = new MyDaAddressSpaceElement();
				name = _T("N1");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_BSTR);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(TRUE);
				aChildrenList.push_back(child);
				child = new MyDaAddressSpaceElement();
				name = _T("N2");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_BSTR);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(TRUE);
				aChildrenList.push_back(child);
				child = new MyDaAddressSpaceElement();
				name = _T("T1");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_I1);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(FALSE);
				aChildrenList.push_back(child);
			} // end if

			if (anElementId == _T("stringBased.N1"))
			{
				child = new MyDaAddressSpaceElement();
				name = _T("T1");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_I1);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(FALSE);
				aChildrenList.push_back(child);
			} // end if

			if (anElementId == _T("stringBased.N2"))
			{
				child = new MyDaAddressSpaceElement();
				name = _T("T1");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_I1);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(FALSE);
				aChildrenList.push_back(child);
				child = new MyDaAddressSpaceElement();
				name = _T("T2");
				child->setName(name);
				name = anElementId + getApp()->getAddressSpaceDelimiter() + child->getName();
				child->setItemId(name);
				child->setAccessRights(EnumAccessRights_READWRITEABLE);
				child->setDatatype(VT_I1);
				child->setIoMode(EnumIoMode_REPORT);
				child->setHasChildren(FALSE);
				aChildrenList.push_back(child);
			} // end if
		} // end if .. else

		return EnumResultCode_S_OK;
	} // end queryAddressSpaceElementChildren


	long queryAddressSpaceElementProperties(
		const tstring& anElementID,
		std::vector<DaProperty*>& aPropertyList)
	{
		if (anElementID.find(_T("stringBased")) == 0)   // starts with stringBased
		{
			tstring propertyId;
			propertyId = anElementID + getApp()->getPropertyDelimiter() + _T("objectType");
			propType1.setItemId(propertyId);
			aPropertyList.push_back(&propType1);
			propertyId = anElementID + getApp()->getPropertyDelimiter() + _T("objectType2");
			propType2.setItemId(propertyId);
			aPropertyList.push_back(&propType2);
		} // end if

		return EnumResultCode_S_OK;
	} // end queryAddressSpaceElementProperties

	BOOL lockElementsMutex(unsigned long timeout = INFINITE)
	{
		return m_elementsJanitor.lock(timeout);
	}

	BOOL unlockElementsMutex()
	{
		return m_elementsJanitor.unlock();
	}

};  //  end MyDaAddressSpaceRoot

#endif
