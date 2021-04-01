#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDAAddressSpaceElement.h"
#include "ServerDATag.h"
#include "ServerDANode.h"
#include "ServerDAProperty.h"
#include "ServerDAServer.h"
#include "SrvApp.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerDAAddressSpaceElement
//-----------------------------------------------------------------------------

ServerDAAddressSpaceElement::ServerDAAddressSpaceElement(void)
{
	m_userDataOTS = 0;
	m_ownCache = FALSE;
	m_isBrowsable = TRUE;
	m_reportCyclic = FALSE;
}

void ServerDAAddressSpaceElement::clonePropertyData(
	IN SODaSItemTag* pItemTag,
	IN OTSPropertyData* pPropDataO,
	OUT SODaSPropertyData* pPropData)
{
	if (!pPropData)
	{
		return;
	}

	pPropData->m_pid = pPropDataO->m_pid;
	pPropData->m_name = pPropDataO->m_name;
	pPropData->m_descr = pPropDataO->m_descr;
	pPropData->m_accessRights = pPropDataO->m_accessRights;

	if (pPropDataO->m_itemID)
	{
		if (_tcslen(pPropDataO->m_itemID) != 0)
		{
			if (pItemTag)
			{
				SOCmnString parItemId = pItemTag->getItemID();

				if (_tcsncmp(parItemId, pPropDataO->m_itemID, _tcslen(parItemId)) != 0)
				{
					SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
					TCHAR propDelim = entryDA->getPropertyDelimiter();
					pPropData->m_itemID.format(_T("%s%c%s"), (LPCTSTR)parItemId, propDelim, (LPCTSTR)pPropDataO->m_itemID);
				}
				else
				{
					pPropData->m_itemID = pPropDataO->m_itemID;
				}
			}
			else
			{
				pPropData->m_itemID = pPropDataO->m_itemID;
			}
		}
	}

	pPropData->m_datatype = pPropDataO->m_datatype;
}


BOOL ServerDAAddressSpaceElement::getPropertyData(IN SODaSItemTag* pItemTag, IN DWORD propId, SODaSPropertyData* pPropData)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	if (pApp->m_callbackFunctions.m_OTSQueryProperties)
	{
		OTSObjectData object;
		DWORD propCountO = 0;
		OTSPropertyData* pPropDataO = NULL;
		DWORD i;
		object.m_objectHandle = (DWORD)pItemTag->getOpcHandle();
		object.m_userData = getOTSUserData();

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSQueryProperties(&object, pItemTag->getItemID(), (LONG)propId, &propCountO, &pPropDataO);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::getPropertyData - Exception occurred in vendor application callback: OTSQueryProperties")));
		}

		for (i = 0; i < propCountO; i++)
		{
			if (pPropDataO[i].m_pid == propId)
			{
				clonePropertyData(pItemTag, &pPropDataO[i], pPropData);
				ret = TRUE;
				break;
			}
		}

		for (i = 0; i < propCountO; i++)
		{
			FreeOTSPropertyData(&pPropDataO[i]);
		}

		if (pPropDataO)
		{
			OTFreeMemory(pPropDataO);
		}
	}

	return ret;
}

BOOL ServerDAAddressSpaceElement::hasPropertyI(IN SODaSItemTag* pItemTag, IN DWORD propId,  SODaSPropertyData* pPropData)
{
	SOCMN_TRY
	{
		return getPropertyData(pItemTag, propId, pPropData);
	}
	SOCMN_CATCH(...)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::hasPropertyI - Exception occurred")));
	}
	return FALSE;
}

void ServerDAAddressSpaceElement::getPropertiesI(
	IN SODaSItemTag* pItemTag,
	IN BOOL /* isSOAP */,
	OUT SOCmnList<SODaSPropertyData>* pListPD)
{
	ServerApp* pApp = ::getServerApp();
	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryProperties)
		{
			OTSObjectData object;
			SODaSPropertyData* pPropData;
			DWORD propCountO = 0;
			OTSPropertyData* pPropDataO = NULL;
			DWORD i;
			object.m_objectHandle = (DWORD)pItemTag->getOpcHandle();
			object.m_userData = getOTSUserData();

			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSQueryProperties(&object, pItemTag->getItemID(), 0, &propCountO, &pPropDataO);
			}
			SOCMN_CATCH(...)
			{
				_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::getPropertiesI - Exception occurred in vendor application callback: OTSQueryProperties")));
			}

			for (i = 0; i < propCountO; i++)
			{
				SOCMN_ALLOCATE_OBJECT(pPropData, SODaSPropertyData());
				clonePropertyData(pItemTag, &pPropDataO[i], pPropData);
				pListPD->add(pPropData);
			}

			for (i = 0; i < propCountO; i++)
			{
				FreeOTSPropertyData(&pPropDataO[i]);
			}

			if (pPropDataO)
			{
				OTFreeMemory(pPropDataO);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::getPropertiesI - Exception occurred")));
	}
}

BOOL ServerDAAddressSpaceElement::initPropertyI(IN SODaSItemTag* pItemTag, IN SODaSProperty* property, SODaSPropertyData* pPropData)
{
	BOOL ret = FALSE;
	SOCMN_TRY
	{
		SODaSPropertyData locPropData;

		if (!pPropData)
		{
			ret = getPropertyData(pItemTag, property->getPropertyId(), &locPropData);

			if (ret)
			{
				pPropData = &locPropData;
			}
		}

		if (pPropData)
		{
			if (pPropData->m_pid >= 5000)
			{
				// server specific properties
				property->setDescription(pPropData->m_descr);
				property->setNativeDatatype(pPropData->m_datatype);
				property->setPropertyId(pPropData->m_pid);
				property->setName(pPropData->m_name);
				property->setAccessRights(pPropData->m_accessRights);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::initPropertyI - Exception occurred")));
	}
	return ret;
}

BOOL ServerDAAddressSpaceElement::handlePropertyRequestI(IN SODaSItemTag* pItemTag, IN SODaSProperty* prop, IN SODaSRequest* pRequest)
{
	ServerApp* pApp = ::getServerApp();
	SOCmnPointer<ServerDAServer> server = (ServerDAServer*)pRequest->getServer();
	OTSRequestData requestData;
	OTSValueData writeValue;
	SOCmnVariant val;
	BOOL handled = FALSE;
	requestData.m_object.m_objectHandle = (DWORD)pItemTag->getOpcHandle();
	requestData.m_propertyID = prop->getPropertyId();
	pRequest->addRef();
	requestData.m_requestHandle = (DWORD)pRequest->getOpcHandle();
	requestData.m_object.m_userData = getOTSUserData();
	requestData.m_clientHandle = (DWORD)server->getOpcHandle();

	if (pRequest->getOperation() == SODaSRequest::write)
	{
		pRequest->getWriteValue(&val);
	}

	writeValue.m_pValue = val;
#ifdef SOOS_WINDOWS
	writeValue.m_timestamp.dwHighDateTime = 0;
	writeValue.m_timestamp.dwLowDateTime = 0;
#else
	writeValue.m_timestamp.tv_sec = 0;
	writeValue.m_timestamp.tv_usec = 0;
#endif
	writeValue.m_quality = OPC_QUALITY_GOOD;
	SOCMN_TRY
	{
		if (pRequest->getOperation() == SODaSRequest::read)
		{
			if (pApp->m_callbackFunctions.m_OTSHandleReadRequests)
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSHandleReadRequests(1, &requestData);
				}
				SOCMN_CATCH(...)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pRequest->getObjTraceId(), (_T("ServerDAAddressSpaceElement::handlePropertyRequestI - Exception occurred in vendor application callback: OTSHandleReadRequests")));
				}
				handled = TRUE;
			}
		}
		else
		{
			if (pApp->m_callbackFunctions.m_OTSHandleWriteRequests)
			{
				pApp->m_callbackFunctions.m_OTSHandleWriteRequests(1, &requestData, &writeValue);
				handled = TRUE;
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pRequest->getObjTraceId(), (_T("ServerDAAddressSpaceElement::handlePropertyRequestI - Exception occurred")));
	}
	return handled;
}


//-----------------------------------------------------------------------------
// getDeviceIOMode
//
// - get the device I/O mode of the item tag
//
// returns:
//      device I/O mode
//
BYTE ServerDAAddressSpaceElement::getDeviceIOModeI(
	SODaSItemTag* pItemTag,
	BYTE deviceIOAndUpdateMode,
	SODaSRequest* req)   // request
{
	if (req)
	{
		if ((pItemTag->getUpdateMode() == SODaSItemTag::report) && (req->getOperation() == SODaSRequest::read) && (!m_reportCyclic))
		{
			SOCmnPointer<SODaSCache> cache = pItemTag->getCache();

			if (cache.isNotNull())
				if (cache->isSet())
				{
					return SODaSItemTag::cache;    // use the cache value for event driven read operations
				}
		}
	}

	if (deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE)
	{
		return deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE;
	}
	else
	{
		return SODaSItemTag::transaction;
	}
} // getDeviceIOMode

void ServerDAAddressSpaceElement::connectedObjectChanged(SODaSItemTag* pItemTag)
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSChangeItems)
	{
		SOCmnList<SODaSItemTag> itemTagList(pApp->m_itemTagChangedList);

		if (!itemTagList.findKey(pItemTag->getOpcHandle(), NULL))
		{
			itemTagList.add(pItemTag, pItemTag->getOpcHandle());
		}
	}
}

LONG ServerDAAddressSpaceElement::getAddressSpaceElementData(SODaSItemTag* pItemTag, OTSAddressSpaceElementData* pData)
{
	if (!pData)
	{
		return E_INVALIDARG;
	}

	SOCmnString name = pItemTag->getName();
	SOCmnString itemID = pItemTag->getItemID();
	BOOL ownCache;
	pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	SetOutputStructMemberString(name, &pData->m_name);
	SetOutputStructMemberString(itemID, &pData->m_itemID);

	if (pItemTag->is(SOCMNOBJECT_TYPE_TAG))
	{
		pData->m_userData = ((ServerDATag*)(SODaSItemTag*)pItemTag)->getOTSUserData();
		pData->m_hasChildren = FALSE;
		ownCache = ((ServerDATag*)(SODaSItemTag*)pItemTag)->getOwnCache();
	}
	else
	{
		pData->m_userData = ((ServerDANode*)(SODaSItemTag*)pItemTag)->getOTSUserData();
		pData->m_hasChildren = TRUE;
		ownCache = ((ServerDANode*)(SODaSItemTag*)pItemTag)->getOwnCache();
	}

	pData->m_accessRights = (BYTE)pItemTag->getAccessRights();
	pData->m_datatype = pItemTag->getNativeDatatype();
	pData->m_isBrowsable = (BYTE)pItemTag->isBrowsable(NULL);

	if (pItemTag->getDeviceIOMode(NULL) == SODaSItemTag::transaction)
	{
		if (pItemTag->getUpdateMode() == SODaSItemTag::poll)
		{
			if (ownCache)
			{
				pData->m_ioMode = OTS_IOMODE_POLL_OWNCACHE;
			}
			else
			{
				pData->m_ioMode = OTS_IOMODE_POLL;
			}
		}
		else
		{
			pData->m_ioMode = OTS_IOMODE_REPORT;
		}
	}
	else
	{
		pData->m_ioMode = OTS_IOMODE_NONE;
	}

	return S_OK;
}


void ServerDAAddressSpaceElement::create(SOCmnElement* pParent, SODaSItemTag* pItemTag)
{
	ServerApp* pApp = ::getServerApp();

	long result = S_FALSE;
	bool handledEx = false;
	OTSObjectHandle hObject, hParent;
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnString key(pItemTag->getItemID());
	unsigned short cachedElementDataFilter = OTS_ELEMENTDATA_ANY;
	unsigned short requiredElementDataFilter = OTS_ELEMENTDATA_ACCESSRIGHTS | OTS_ELEMENTDATA_IOMODE | OTS_ELEMENTDATA_HASCHILDREN |
		OTS_ELEMENTDATA_ISBROWSABLE | OTS_ELEMENTDATA_DATATYPE | OTS_ELEMENTDATA_ELEMENTTYPE;
	OTSAddressSpaceElementData *pElementData = NULL;
	OTSAddressSpaceElementData *pExistingElementData = NULL;
	OTSAddressSpaceElementData dataIn;

	hParent = NULL;

	if ((pParent) && (!pParent->is(SOCMNOBJECT_TYPE_ROOT)))
	{
		hParent = pParent->getOpcHandle();
	}

	hObject = pItemTag->getOpcHandle();
	memset(&dataIn, 0, sizeof(OTSAddressSpaceElementData));
	getAddressSpaceElementData(pItemTag, &dataIn);

	if (pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElementEx)
	{
		unsigned short requestedElementDataFilter = OTS_ELEMENTDATA_NONE;

		if (entry->getStringBasedObjectDataEx(key, &pExistingElementData, cachedElementDataFilter))
		{
			if ((requiredElementDataFilter & cachedElementDataFilter) == requiredElementDataFilter)
			{
				dataIn.m_accessRights = pExistingElementData->m_accessRights;
				dataIn.m_ioMode = pExistingElementData->m_ioMode;
				dataIn.m_hasChildren = pExistingElementData->m_hasChildren;
				dataIn.m_isBrowsable = pExistingElementData->m_isBrowsable;
				dataIn.m_userData = pExistingElementData->m_userData;
				dataIn.m_datatype = pExistingElementData->m_datatype;
				dataIn.m_elementType = pExistingElementData->m_elementType;
			}
			else
			{
				if ((cachedElementDataFilter & OTS_ELEMENTDATA_ACCESSRIGHTS) == OTS_ELEMENTDATA_ACCESSRIGHTS)
				{
					dataIn.m_accessRights = pExistingElementData->m_accessRights;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_ACCESSRIGHTS;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_IOMODE) == OTS_ELEMENTDATA_IOMODE)
				{
					dataIn.m_ioMode = pExistingElementData->m_ioMode;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_IOMODE;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
				{
					dataIn.m_hasChildren = pExistingElementData->m_hasChildren;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_HASCHILDREN;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_ISBROWSABLE) == OTS_ELEMENTDATA_ISBROWSABLE)
				{
					dataIn.m_isBrowsable = pExistingElementData->m_isBrowsable;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_ISBROWSABLE;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_USERDATA) == OTS_ELEMENTDATA_USERDATA)
				{
					dataIn.m_userData = pExistingElementData->m_userData;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_USERDATA;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_DATATYPE) == OTS_ELEMENTDATA_DATATYPE)
				{
					dataIn.m_datatype = pExistingElementData->m_datatype;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_DATATYPE;
				}

				if ((cachedElementDataFilter & OTS_ELEMENTDATA_ELEMENTTYPE) == OTS_ELEMENTDATA_ELEMENTTYPE)
				{
					dataIn.m_elementType = pExistingElementData->m_elementType;
				}
				else
				{
					requestedElementDataFilter |= OTS_ELEMENTDATA_ELEMENTTYPE;
				}
			}
		}

		pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));

		SOCMN_TRY
		{
			result = pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElementEx(hParent, hObject, &dataIn, requestedElementDataFilter, pElementData);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::create - Exception occurred in vendor application callback: OTSCreateAddressSpaceElementEx")));
		}

		if (S_OK == result)
		{
			if (pItemTag->is(SOCMNOBJECT_TYPE_TAG))
			{
				((ServerDATag*)(SODaSItemTag*)pItemTag)->setOTSUserData(pElementData->m_userData);
				handledEx = true;
			}
			else if (pItemTag->is(SOCMNOBJECT_TYPE_NODE))
			{
				((ServerDANode*)(SODaSItemTag*)pItemTag)->setOTSUserData(pElementData->m_userData);
				handledEx = true;
			}

			entry->setStringBasedObjectDataEx(key, pElementData, requestedElementDataFilter);
		}

		FreeOTSAddressSpaceElementData(&dataIn);
	}

	if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElement))
	{
		cachedElementDataFilter = OTS_ELEMENTDATA_ALL;
		hParent = NULL;

		if (entry->getStringBasedObjectDataEx(key, &pExistingElementData, cachedElementDataFilter))
		{
			dataIn.m_accessRights = pExistingElementData->m_accessRights;
			dataIn.m_ioMode = pExistingElementData->m_ioMode;
			dataIn.m_hasChildren = pExistingElementData->m_hasChildren;
			dataIn.m_isBrowsable = pExistingElementData->m_isBrowsable;
			dataIn.m_userData = pExistingElementData->m_userData;
			dataIn.m_datatype = pExistingElementData->m_datatype;
			dataIn.m_elementType = pExistingElementData->m_elementType;
		}

		if (!pElementData)
		{
			pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
		}
		else
		{
			FreeOTSAddressSpaceElementData(pElementData);
		}

		SOCMN_TRY
		{
			result = pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElement(hParent, hObject, &dataIn, pElementData);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pItemTag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::create - Exception occurred in vendor application callback: OTSCreateAddressSpaceElement")));
		}

		if (pItemTag->is(SOCMNOBJECT_TYPE_TAG))
		{
			((ServerDATag*)(SODaSItemTag*)pItemTag)->setOTSUserData(pElementData->m_userData);
		}
		else if (pItemTag->is(SOCMNOBJECT_TYPE_NODE))
		{
			((ServerDANode*)(SODaSItemTag*)pItemTag)->setOTSUserData(pElementData->m_userData);
		}

		if (S_OK == result)
		{
			entry->setStringBasedObjectDataEx(key, pElementData);
		}

		FreeOTSAddressSpaceElementData(&dataIn);
		FreeOTSAddressSpaceElementData(pElementData);
	}

	OTFreeMemory(pElementData);
}

void ServerDAAddressSpaceElement::destroy(SODaSItemTag* tag)
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSDestroyAddressSpaceElement)
	{
		OTSObjectData object;
		object.m_objectHandle = (DWORD)tag->getOpcHandle();
		object.m_userData = 0;

		if (tag->is(SOCMNOBJECT_TYPE_TAG))
		{
			object.m_userData = ((ServerDATag*)(SODaSItemTag*)tag)->getOTSUserData();
		}
		else if (tag->is(SOCMNOBJECT_TYPE_NODE))
		{
			object.m_userData = ((ServerDANode*)(SODaSItemTag*)tag)->getOTSUserData();
		}

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSDestroyAddressSpaceElement(object);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, tag->getObjTraceId(), (_T("ServerDAAddressSpaceElement::destroy - Exception occurred in vendor application callback: OTSDestroyAddressSpaceElement")));
		}
	}
}

BOOL ServerDAAddressSpaceElement::isBrowsableI(IN SOCmnObject* /* pServer */)
{
	return (BOOL)m_isBrowsable;
}

#endif // OTB_SERVER_DA

