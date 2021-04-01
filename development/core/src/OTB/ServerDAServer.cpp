#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDAServer.h"
#include "ServerDANode.h"
#include "ServerDATag.h"
#include "ServerDAAddressSpaceElement.h"
#include "SrvApp.h"
#include "SODaSXmlServer.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerDAServer
//-----------------------------------------------------------------------------

ServerDAServer::ServerDAServer(
	IN OPTIONAL BOOL publiServerDAGroups,
	IN OPTIONAL BYTE serverInstance,
	IN OPTIONAL DWORD /* minRequestLoopTime */) :
	SODaSServer(publiServerDAGroups, serverInstance)
{
}

ServerDAServer::~ServerDAServer()
{
	ServerApp* pApp = ::getServerApp();
	BYTE flags = getFlags();

	SOCMN_TRY
	{
		if ((flags == SODASSERVER_FLAG_DCOM) ||
		(flags == SODASSERVER_FLAG_SOAP) ||
		(flags == (SODASSERVER_FLAG_SOAP | SODASSERVER_FLAG_HTTP_RECEIVER)))
		{
			if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
			{
				OTSSessionData clientStateData;
				clientStateData.m_state = OTS_SESSIONSTATE_DESTROY;
				clientStateData.m_type = flags;
				clientStateData.m_password = NULL;
				clientStateData.m_userName = NULL;
				clientStateData.m_clientName = NULL;
				clientStateData.m_clientIp = NULL;
				clientStateData.m_clientRemote = 0;

				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::~ServerDAServer - Exception occurred in vendor application callback: OTSChangeSessionState")));
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::~ServerDAServer - Exception occurred")));
	}
}

DWORD ServerDAServer::setFlags(BYTE flags)
{
	SODaSServer::setFlags(flags);
	HRESULT result = S_OK;
	ServerApp* pApp = ::getServerApp();

	SOCMN_TRY
	{
		if ((flags == SODASSERVER_FLAG_DCOM) ||
		(flags == SODASSERVER_FLAG_SOAP) ||
		(flags == (SODASSERVER_FLAG_SOAP | SODASSERVER_FLAG_HTTP_RECEIVER)))
		{
			if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
			{
				OTSSessionData clientStateData;
				clientStateData.m_state = OTS_SESSIONSTATE_CREATE;
				clientStateData.m_type = flags;
				clientStateData.m_password = NULL;
				clientStateData.m_userName = NULL;
				clientStateData.m_clientName = (LPTSTR)m_clientName;
				clientStateData.m_clientIp = (LPTSTR)m_clientIp;
				clientStateData.m_clientRemote = m_clientRemote;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::setFlags - Exception occurred in vendor application callback: OTSChangeSessionState")));
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::setFlags - Exception occurred")));
	}

	return result;
}

LPCTSTR ServerDAServer::getOtsPropertyItemID(OTSPropertyData* pPropDataO, SODaSItemTag* pItemTag, SOCmnString& itemID)
{
	itemID.empty();

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
					itemID.format(_T("%s%c%s"), (LPCTSTR)parItemId, propDelim, (LPCTSTR)pPropDataO->m_itemID);
				}
				else
				{
					itemID = pPropDataO->m_itemID;
				}
			}
			else
			{
				itemID = pPropDataO->m_itemID;
			}
		}
	}

	return itemID;
}

DWORD ServerDAServer::getPropertyIDByItemID(IN LPCTSTR parentName, IN LPCTSTR propItemID)   // property part of the item id
{
	DWORD propID = 0;
	ServerApp* pApp = ::getServerApp();

	SOCMN_TRY
	{
		SOCmnPointer<SODaSItemTag> itemTag;

		itemTag = (SODaSItemTag*)SODaSNameSpaceRoot::getElement(parentName);

		if (pApp->m_callbackFunctions.m_OTSQueryProperties)
		{
			SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
			TCHAR propDelim = entryDA->getPropertyDelimiter();
			SOCmnString itemID;
			OTSObjectData object;
			DWORD propCountO = 0;
			OTSPropertyData* pPropDataO = NULL;
			DWORD i;
			SOCmnString itemIdO;

			if (itemTag.isNotNull())
			{
				object.m_objectHandle = (DWORD)itemTag->getOpcHandle();

				if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
				{
					object.m_userData = ((ServerDATag*)(SODaSItemTag*)itemTag)->getOTSUserData();
				}
				else if (itemTag->is(SOCMNOBJECT_TYPE_NODE))
				{
					object.m_userData = ((ServerDANode*)(SODaSItemTag*)itemTag)->getOTSUserData();
				}

				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryProperties(&object, (LPTSTR)parentName, propID, &propCountO, &pPropDataO);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::getPropertyIDByItemID - Exception occurred in vendor application callback: OTSQueryProperties")));
				}
			}
			else
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryProperties(NULL, (LPTSTR)parentName, propID, &propCountO, &pPropDataO);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::getPropertyIDByItemID - Exception occurred in vendor application callback: OTSQueryProperties")));
				}
			}

			itemID.format(_T("%s%c%s"), parentName, propDelim, propItemID);

			for (i = 0; i < propCountO; i++)
			{
				if (_tcscmp(getOtsPropertyItemID(&pPropDataO[i], itemTag, itemIdO), itemID) == 0)
				{
					propID = pPropDataO[i].m_pid;
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

			if (propID == 0)
			{
				propID = SODaSServer::getPropertyIDByItemID(parentName, propItemID);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::getPropertyIDByItemID - Exception occurred")));
	}

	return propID;
} // getPropertyIdByName

//-----------------------------------------------------------------------------
// getPropertyItemID
//
// - get the item id for a property
//
// returns:
//      S_OK              - is property of tag
//      OPC_E_INVALID_PID - not property of tag
//
HRESULT ServerDAServer::getPropertyItemID(
	IN SOCmnElement* nsElement, // namespace element
	IN LPCTSTR itemId,                  // parents itemId
	IN DWORD propId,                        // property id
	OUT SOCmnString& propItemID)// property item id
{
	return getPropertyItemIDI(nsElement, itemId, propId, propItemID);
}

HRESULT ServerDAServer::getPropertyItemIDI(
	IN SOCmnElement* nsElement, // namespace element
	IN LPCTSTR itemId,                  // parents itemId
	IN DWORD propId,                        // property id
	OUT SOCmnString& propItemID)// property item id
{
	ServerApp* pApp = ::getServerApp();
	HRESULT res = OPC_E_INVALID_PID;

	SOCMN_TRY
	{
		SOCmnPointer<SODaSItemTag> itemTag;

		if (nsElement)
		{
			itemTag = (SODaSItemTag*)nsElement;
			itemTag.addRef();
		}
		else
		{
			itemTag = (SODaSItemTag*)SODaSNameSpaceRoot::getElement(itemId);
		}
		if (pApp->m_callbackFunctions.m_OTSQueryProperties)
		{
			SOCmnPointer<SODaSEntry> entryDA = getSODaSEntry();
			SOCmnString itemID;
			OTSObjectData object;
			DWORD propCountO = 0;
			OTSPropertyData* pPropDataO = NULL;
			DWORD i;

			if (itemTag.isNotNull())
			{
				object.m_objectHandle = (DWORD)itemTag->getOpcHandle();

				if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
				{
					object.m_userData = ((ServerDATag*)(SODaSItemTag*)itemTag)->getOTSUserData();
				}
				else if (itemTag->is(SOCMNOBJECT_TYPE_NODE))
				{
					object.m_userData = ((ServerDANode*)(SODaSItemTag*)itemTag)->getOTSUserData();
				}

				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryProperties(&object, (LPTSTR)itemId, propId, &propCountO, &pPropDataO);
				}
				SOCMN_CATCH(...)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, itemTag->getObjTraceId(), (_T("ServerDAServer::getPropertyItemIDI - Exception occurred in vendor application callback: OTSQueryProperties")));
				}
			}
			else
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryProperties(NULL, (LPTSTR)itemId, propId, &propCountO, &pPropDataO);
				}
				SOCMN_CATCH(...)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, itemID, (_T("ServerDAServer::getPropertyItemIDI - Exception occurred in vendor application callback: OTSQueryProperties")));
				}
			}

			for (i = 0; i < propCountO; i++)
			{
				if (pPropDataO[i].m_pid == propId)
				{
					getOtsPropertyItemID(&pPropDataO[i], itemTag, propItemID);
					res = S_OK;
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
	}
	SOCMN_CATCH(...)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, itemId, (_T("ServerDAServer::getPropertyItemIDI - Exception occurred")));
	}

	return res;
} // getPropertyItemID


BOOL ServerDAServer::isNodeName(IN LPCTSTR fullName)
{
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnString key(fullName);
		bool handledEx = false;
		OTSAddressSpaceElementData *pElementData = NULL;
		OTSAddressSpaceElementData *pExistingElementData = NULL;
		unsigned short elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)fullName, OT_ADDRESSSPACEELEMENT_TYPE_DA,
						elementDataFilter, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isNodeName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
				}

				if (S_OK == result)
				{
					if ((elementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
					{
						// vendor app callback provided required data
						entry->setStringBasedObjectDataEx(fullName, pElementData, elementDataFilter);
						ret = pElementData->m_hasChildren;
						handledEx = true;
					}
					else if (elementDataFilter != OTS_ELEMENTDATA_NONE)
					{
						// vendor app callback provided something (although not required data), store to cache
						entry->setStringBasedObjectDataEx(fullName, pElementData, elementDataFilter);
					}
				}
			}
			else
			{
				ret = pExistingElementData->m_hasChildren;
				handledEx = true;
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)fullName, OT_ADDRESSSPACEELEMENT_TYPE_DA, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isNodeName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (S_OK == result)
				{
					ret = pElementData->m_hasChildren;
					entry->setStringBasedObjectDataEx(key, pElementData);
				}
			}
			else
			{
				ret = pExistingElementData->m_hasChildren;
			}
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isNodeName - Exception occurred")));
	}

	return ret;
}

BOOL ServerDAServer::isTagName(IN LPCTSTR fullName)
{
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnString key(fullName);
		bool handledEx = false;
		OTSAddressSpaceElementData *pElementData = NULL;
		OTSAddressSpaceElementData *pExistingElementData = NULL;
		unsigned short elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)fullName, OT_ADDRESSSPACEELEMENT_TYPE_DA,
						elementDataFilter, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isTagName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
				}

				if (S_OK == result)
				{
					if ((elementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
					{
						entry->setStringBasedObjectDataEx(fullName, pElementData, elementDataFilter);
						ret = !pElementData->m_hasChildren;
						handledEx = true;
					}
					else if (elementDataFilter != 0)
					{
						// vendor app callback provided something (although not required data), store to cache
						entry->setStringBasedObjectDataEx(fullName, pElementData, elementDataFilter);
					}
				}
			}
			else
			{
				ret = !pExistingElementData->m_hasChildren;
				handledEx = true;
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			elementDataFilter = OTS_ELEMENTDATA_HASCHILDREN;

			if (!entry->getStringBasedObjectDataEx(key, &pExistingElementData, elementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)fullName, OT_ADDRESSSPACEELEMENT_TYPE_DA, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isTagName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (S_OK == result)
				{
					ret = !pElementData->m_hasChildren;
					entry->setStringBasedObjectDataEx(key, pElementData);
				}
			}
			else
			{
				ret = !pExistingElementData->m_hasChildren;
			}
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isTagName - Exception occurred")));
	}

	return ret;
}

void ServerDAServer::initTag(IN SODaSTag* tag)
{
	initItemTag(tag);
	SOCmnPointer<SOCmnElement> parent = tag->getParentBranch();
	((ServerDATag*)tag)->create((SOCmnElement*)parent, tag);
}

void ServerDAServer::initNode(IN SODaSNode* node)
{
	initItemTag(node);
	SOCmnPointer<SOCmnElement> parent = node->getParentBranch();
	((ServerDANode*)node)->create((SOCmnElement*)parent, node);
}

void ServerDAServer::initItemTag(IN SODaSItemTag* itemTag)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		bool handledEx = false;
		SOCmnString itemID = itemTag->getItemID();
		unsigned short cachedElementDataFilter = OTS_ELEMENTDATA_ANY;
		unsigned short requiredElementDataFilter = OTS_ELEMENTDATA_ACCESSRIGHTS | OTS_ELEMENTDATA_IOMODE | OTS_ELEMENTDATA_ISBROWSABLE | OTS_ELEMENTDATA_USERDATA | OTS_ELEMENTDATA_DATATYPE;
		OTSAddressSpaceElementData *pElementData = NULL;
		OTSAddressSpaceElementData *pExistingElementData = NULL;
		long result = S_FALSE;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			// get any existing, cached element data
			entry->getStringBasedObjectDataEx(itemID, &pExistingElementData, cachedElementDataFilter);

			if ((requiredElementDataFilter & cachedElementDataFilter) == requiredElementDataFilter)
			{
				handledEx = true;
			}
			else
			{
				unsigned char pass = 0;

				while ((pass < 2) && (!handledEx))
				{
					if (!pElementData)
					{
						pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
					}

					++pass;
					// negate current cache value with required mask and see what needs to be asked for in the callback
					unsigned short requestedElementDataFilter = requiredElementDataFilter & ~cachedElementDataFilter;

					SOCMN_TRY
					{
						result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx(itemID, OT_ADDRESSSPACEELEMENT_TYPE_DA,
							requestedElementDataFilter, pElementData);
					}
						SOCMN_CATCH(...)
					{
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::initItemTag - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
					}

					if (S_OK == result)
					{
						if (((cachedElementDataFilter | requestedElementDataFilter) & requiredElementDataFilter) != requiredElementDataFilter)
						{
							_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (
								_T("ServerDAServer::initItemTag/QueryAddressSpaceElementDataEx - Vendor application callback did not provide all data (requested 0x%03X, provided 0x%03X) for tag %s"),
								requiredElementDataFilter & ~cachedElementDataFilter, requestedElementDataFilter, (LPCTSTR)itemID));
							// vendor callback was not able to provide required data once, don`t call again
							++pass;

							if (requestedElementDataFilter != OTS_ELEMENTDATA_NONE)
							{
								// if vendor app callback did provide something however, store in cache
								entry->setStringBasedObjectDataEx(itemID, pElementData, requestedElementDataFilter);
							}
						}
						else
						{
							// merge provided data with existing data
							if (entry->setStringBasedObjectDataEx(itemID, pElementData, requestedElementDataFilter))
							{
								// if cache is enabled
								if (entry->getStringBasedObjectDataEx(itemID, &pExistingElementData, requiredElementDataFilter))
								{
									// if the part from cache did not expire until now (e.g. callback might had taken a while)..
									handledEx = true;
								}
								// otherwise try once more with the *Ex callback (see pass + while), request only data that expired
							}
							else
							{
								// surely cache was disabled to begin with, so don`t try again, no need
								++pass;
								pExistingElementData = pElementData;
							}
						}
					}
					else
					{
						// callback failed, don`t call again
						++pass;
					}
				}
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			cachedElementDataFilter = requiredElementDataFilter;

			if (!entry->getStringBasedObjectDataEx(itemID, &pExistingElementData, cachedElementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData(itemID, OT_ADDRESSSPACEELEMENT_TYPE_DA, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::initItemTag - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (result == S_OK)
				{
					// save data into cache in order to release previously-allocated memory
					if (entry->setStringBasedObjectDataEx(itemID, pElementData))
					{
						// if cache is enabled
						entry->getStringBasedObjectDataEx(itemID, &pExistingElementData, requiredElementDataFilter);
					}
					else
					{
						pExistingElementData = pElementData;
					}
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
					OTFreeMemory(pElementData);
					return;
				}
			}
		}

		if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
		{
			((ServerDATag*)itemTag)->setOTSUserData(pExistingElementData->m_userData);
			((ServerDATag*)itemTag)->setIsBrowsable(pExistingElementData->m_isBrowsable);
		}
		else
		{
			((ServerDANode*)itemTag)->setOTSUserData(pExistingElementData->m_userData);
			((ServerDANode*)itemTag)->setIsBrowsable(pExistingElementData->m_isBrowsable);
		}

		itemTag->setDeviceIOMode(SODaSItemTag::transaction);
		itemTag->setAccessRights(pExistingElementData->m_accessRights);
		itemTag->setNativeDatatype((VARTYPE)pExistingElementData->m_datatype);

		switch (pExistingElementData->m_ioMode)
		{
		case OTS_IOMODE_POLL_OWNCACHE:
			((ServerDATag*)itemTag)->setOwnCache(TRUE);

		case OTS_IOMODE_POLL:
			itemTag->setUpdateMode(SODaSItemTag::poll);
			break;

		case OTS_IOMODE_REPORT_CYCLIC:
			((ServerDATag*)itemTag)->setReportCyclic(TRUE);

		case OTS_IOMODE_REPORT:
			itemTag->setUpdateMode(SODaSItemTag::report);
			break;

		default:
			break;
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::initItemTag - Exception occurred")));
	}
}

enum SOSrvBrowser::browserOrganisation ServerDAServer::queryOrganization(void)
{
	return SOSrvBrowser::orgHierarchical;
}

BOOL ServerDAServer::browseOnlyStringBased(void)
{
	ServerApp* pApp = ::getServerApp();
	return (pApp->m_addressSpaceType == OTS_ADDRESSSPACETYPE_STRING);
}

void ServerDAServer::getChildNamesOfNodeName(
	IN LPCTSTR nodeName,
	IN enum SOSrvBrowser::browserFilterType browseType,
	IN LPCTSTR filterCriteria,
	IN VARTYPE dataTypeFilter,
	IN DWORD accessRightsFilter,
	OUT SOCmnStringList& nameList)
{
	ServerApp* pApp = ::getServerApp();

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren)
		{
			SOCmnList<SODaSBrowseElementData> *pListBE = new SOCmnList<SODaSBrowseElementData>();
			SOCmnListPosition pos;
			SODaSBrowseElementData* pBE;
			pListBE->create((SOCmnListHeader::isRehashingEnabled() ? SOCMNLIST_TYPE_ORG_MAP : SOCMNLIST_TYPE_ORG_LIST) | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_DATA);
			addStringBasedChildren(nodeName, pListBE, browseType == SOSrvBrowser::typeFlat);
			pos = pListBE->getStartPosition();

			while (pos)
			{
				pBE = pListBE->getNext(pos);

				// apply filters
				if (browseType != SOSrvBrowser::typeAll)
				{
					if (((browseType == SOSrvBrowser::typeLeaf) || (browseType == SOSrvBrowser::typeFlat)) && (pBE->m_flags & OPC_BROWSE_HASCHILDREN))
					{
						continue;
					}

					if ((browseType == SOSrvBrowser::typeBranch) && (!(pBE->m_flags & OPC_BROWSE_HASCHILDREN)))
					{
						continue;
					}
				}

				if ((filterCriteria) && (_tcslen(filterCriteria) != 0))
				{
					if (browseType == SOSrvBrowser::typeFlat)
					{
						if (!matchStringFilter(pBE->m_itemID, filterCriteria))
						{
							continue;
						}
					}
					else
					{
						if (!matchStringFilter(pBE->m_name, filterCriteria))
						{
							continue;
						}
					}
				}

				if (dataTypeFilter != VT_EMPTY)
				{
					if (pBE->m_datatype != dataTypeFilter)
					{
						continue;
					}
				}

				if (accessRightsFilter != 0)
				{
					if ((accessRightsFilter & pBE->m_accessRights) == 0)
					{
						continue;
					}
				}

				// passed all filters
				if (browseType != SOSrvBrowser::typeFlat)
				{
					nameList.add(pBE->m_name);
				}
				else
				{
					nameList.add(pBE->m_itemID);
				}
			}

			pListBE->removeAll();
			delete pListBE;
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::getChildNamesOfNodeName - Exception occurred")));
	}
}

HRESULT ServerDAServer::getChildrenData(
	IN LPCTSTR itemId,
	IN enum browserFilterType fType,
	IN LPCTSTR elementNameFilter,
	IN LPCTSTR /* vendorFilter */,
	IN DWORD maxElementsReturned,
	IN OUT SOCmnString& contPoint,
	OUT BOOL& moreElements,
	IN OUT SOCmnList<SODaSBrowseElementData> *pListBE)
{
	ServerApp* pApp = ::getServerApp();
	HRESULT res = S_OK;
	DWORD inCnt = pListBE->getCount();
	moreElements = FALSE;

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren)
		{
			SOCmnListPosition posC, posP, posN, posSBstart;
			SODaSBrowseElementData* pBE = NULL;
			BOOL removeFromList = FALSE;
			DWORD cnt = 0;
			BOOL continuationPointFound = FALSE;
			addStringBasedChildren(itemId, pListBE, FALSE);
			posN = pListBE->getStartPosition();
			posC = NULL;
			posSBstart = NULL;

			while (posN)
			{
				posP = posC;
				posC = posN;
				pBE = pListBE->getNext(posN);

				if (cnt < inCnt)
				{
					// skip the object based entries
					cnt++;
					continue;
				}

				if (!posSBstart)
				{
					posSBstart = posC;    // save start of string based entries
				}

				removeFromList = FALSE;

				// apply filters
				if (cnt < maxElementsReturned + inCnt)
				{
					if ((!contPoint.isEmpty()) && (!moreElements) && (!continuationPointFound))
					{
						// search for continuation point
						if (_tcscmp(contPoint, pBE->m_itemID) != 0)
						{
							removeFromList = TRUE;
						}
						else
						{
							continuationPointFound = TRUE;
						}
					}

					if (!removeFromList)
					{
						if ((fType == SOSrvBrowser::typeAll) ||
							((fType == SOSrvBrowser::typeItem) && (pBE->m_flags & OPC_BROWSE_ISITEM)) ||
							((fType == SOSrvBrowser::typeBranch) && (pBE->m_flags & OPC_BROWSE_HASCHILDREN)))
						{
							if ((elementNameFilter) && (_tcslen(elementNameFilter) != 0))
							{
								if (!matchStringFilter(pBE->m_name, elementNameFilter))
								{
									removeFromList = TRUE;
								}
							}
						}
						else
						{
							removeFromList = TRUE;
						}
					}
				}
				else
				{
					removeFromList = TRUE;

					if (!moreElements)
					{
						// set continuation point
						contPoint = pBE->m_itemID;
						moreElements = TRUE;
					}
				}

				if (removeFromList)
				{
					pListBE->removeAt(posC, posP);

					if (posSBstart == posC)
					{
						posSBstart = posP;
					}

					posC = posP;
				}
				else
				{
					cnt++;
				}
			}

			if (posSBstart != NULL)
			{
				// remove duplicates from list
				SOCmnListPosition posL;
				SODaSBrowseElementData* pBEL;
				posC = NULL;
				posN = pListBE->getStartPosition();

				while (posN != posSBstart)
				{
					// check if element which was already in the list was added again
					posP = posC;
					posC = posN;
					pBE = pListBE->getNext(posN);
					posL = posSBstart;

					while (posL)
					{
						pBEL = pListBE->getNext(posL);

						if (_tcscmp(pBE->m_itemID, pBEL->m_itemID) == 0)
						{
							pListBE->removeAt(posC, posP);
							posP = posC;
							break;
						}
					}
				}
			}

			if (!moreElements)
			{
				if (!contPoint.isEmpty())
				{
					if (continuationPointFound == FALSE)
					{
						res = OPC_E_INVALIDCONTINUATIONPOINT;
					}
					else
					{
						contPoint.empty();
					}
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::getChildrenData - Exception occurred")));
	}

	return res;
}

void ServerDAServer::addAEDataToBEList(DWORD countAE, OTSAddressSpaceElementData* pDataAE, SOCmnList<SODaSBrowseElementData> *pListBE)
{
	SOCmnList<SODaSBrowseElementData> listBE(pListBE);
	DWORD i;
	SODaSBrowseElementData* pRegisteredElementData = NULL;

	for (i = 0; i < countAE; i++)
	{
		if (pDataAE[i].m_elementType == OT_ADDRESSSPACEELEMENT_TYPE_DA)
		{
			pRegisteredElementData = listBE.findKey(pDataAE[i].m_itemID);

			if (!pRegisteredElementData)
			{
				SODaSBrowseElementData* pBE = new SODaSBrowseElementData();
				pBE->m_name = pDataAE[i].m_name;
				pBE->m_itemID = pDataAE[i].m_itemID;
				pBE->m_datatype = (VARTYPE)pDataAE[i].m_datatype;
				pBE->m_accessRights = pDataAE[i].m_accessRights;
				pBE->m_nsElement = NULL;
				pBE->m_flags = 0;

				if (pDataAE[i].m_hasChildren)
				{
					pBE->m_flags |= OPC_BROWSE_HASCHILDREN;
				}

				if (pDataAE[i].m_ioMode != OTS_IOMODE_NONE)
				{
					pBE->m_flags |= OPC_BROWSE_ISITEM;
				}

				listBE.add(pBE, (LPCTSTR)pDataAE[i].m_itemID);
			}
		}
	}
}

void ServerDAServer::addStringBasedChildren(
	IN LPCTSTR nodeName,
	IN SOCmnList<SODaSBrowseElementData> *pListBE,
	IN BOOL deep)
{
	ServerApp* pApp = ::getServerApp();
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	DWORD countAE;
	OTSAddressSpaceElementData* pDataAE;
	DWORD i;

	SOCMN_TRY
	{
		pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren((LPTSTR)nodeName, OT_ADDRESSSPACEELEMENT_TYPE_DA, &countAE, &pDataAE);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::addStringBasedChildren - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementChildren")));
	}
	
	addAEDataToBEList(countAE, pDataAE, pListBE);

	for (i = 0; i < countAE; i++)
	{
		entry->setStringBasedObjectDataEx(pDataAE[i].m_itemID, &pDataAE[i]);
		FreeOTSAddressSpaceElementData(&pDataAE[i]);
	}

	OTFreeMemory(pDataAE);

	if (deep)
	{
		// call query for all children
		SOCmnListPosition pos, prevPos;
		SODaSBrowseElementData* pBE;
		pos = pListBE->getStartPosition();

		while (pos)
		{
			prevPos = pos;
			pBE = pListBE->getNext(pos);

			if (pBE->m_flags & OPC_BROWSE_HASCHILDREN)
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementChildren(pBE->m_itemID, OT_ADDRESSSPACEELEMENT_TYPE_DA, &countAE, &pDataAE);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::addStringBasedChildren - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementChildren")));
				}

				addAEDataToBEList(countAE, pDataAE, pListBE);

				for (i = 0; i < countAE; i++)
				{
					entry->setStringBasedObjectDataEx(pDataAE[i].m_itemID, &pDataAE[i]);
					FreeOTSAddressSpaceElementData(&pDataAE[i]);
				}

				OTFreeMemory(pDataAE);

				if (pos == NULL)
				{
					pListBE->getNext(prevPos);
					pos = prevPos;
				}
			}
		}
	}
}

void ServerDAServer::queryAvailablePropertiesByItemID(
	IN LPCTSTR itemId,
	IN BOOL isSOAP,
	IN BOOL returnPropertyValues,
	OUT SOCmnList<SODaSPropertyData> &propertyList)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnString key(itemId);
		bool handledEx = false;
		OTSAddressSpaceElementData *pElementData = NULL;
		OTSAddressSpaceElementData *pExistingElementData = NULL;
		unsigned short requiredElementDataFilter = OTS_ELEMENTDATA_IOMODE;
		unsigned short cachedElementDataFilter = OTS_ELEMENTDATA_ANY;

		if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
		{
			if (returnPropertyValues)
			{
				requiredElementDataFilter |= OTS_ELEMENTDATA_DATATYPE;
			}

			// get any existing, cached element data
			entry->getStringBasedObjectDataEx(key, &pExistingElementData, cachedElementDataFilter);

			if ((requiredElementDataFilter & cachedElementDataFilter) == requiredElementDataFilter)
			{
				// cache has everything
				handledEx = true;
			}
			else
			{
				unsigned char pass = 0;

				while ((pass < 2) & (!handledEx))
				{
					if (!pElementData)
					{
						pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
					}

					++pass;
					// negate current cache value with required mask and see what needs to be asked for in the callback
					unsigned short requestedElementDataFilter = requiredElementDataFilter & ~cachedElementDataFilter;
					long result = S_FALSE;

					SOCMN_TRY
					{
						result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)itemId, OT_ADDRESSSPACEELEMENT_TYPE_DA,
							requestedElementDataFilter, pElementData);
					}
						SOCMN_CATCH(...)
					{
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::queryAvailablePropertiesByItemID - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
					}

					if (S_OK == result)
					{
						if (((cachedElementDataFilter | requestedElementDataFilter) & requiredElementDataFilter) != requiredElementDataFilter)
						{
							_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (
								_T("ServerDAServer::queryAvailablePropertiesByItemID/QueryAddressSpaceElementDataEx - Vendor application callback did not provide all data (requested 0x%03X, provided 0x%03X) for tag %s"),
								requiredElementDataFilter & ~cachedElementDataFilter, requestedElementDataFilter, (LPCTSTR)itemId));
							// skip requesting once more, vendor callback was not able to provide data once, won`t stress again
							++pass;

							if (requestedElementDataFilter != 0)
							{
								entry->setStringBasedObjectDataEx(itemId, pElementData, requestedElementDataFilter);
							}
						}
						else
						{
							if (entry->setStringBasedObjectDataEx(itemId, pElementData, requestedElementDataFilter))
							{
								// if cache is enabled
								if (entry->getStringBasedObjectDataEx(itemId, &pExistingElementData, requiredElementDataFilter))
								{
									// if the part from cache did not expire until now (e.g. callback might had taken a while)..
									handledEx = true;
								}
								// otherwise give it one more try with the *Ex callback (see pass + while), request only data that expired
							}
							else
							{
								// if cache is disabled, all data was requested (none was found in cache). skip any remaining while cycles
								++pass;
								pExistingElementData = pElementData;
							}
						}
					}
					else
					{
						// callback failed, don`t call again
						++pass;
					}
				}
			}
		}

		if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
		{
			requiredElementDataFilter = OTS_ELEMENTDATA_IOMODE;

			if (returnPropertyValues)
			{
				requiredElementDataFilter |= OTS_ELEMENTDATA_DATATYPE;
			}

			if (!entry->getStringBasedObjectDataEx(key, &pElementData, requiredElementDataFilter))
			{
				if (!pElementData)
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
				}
				else
				{
					FreeOTSAddressSpaceElementData(pElementData);
				}

				long result = S_FALSE;

				SOCMN_TRY
				{
					result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)itemId, OT_ADDRESSSPACEELEMENT_TYPE_DA, pElementData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::queryAvailablePropertiesByItemID - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
				}

				if (S_OK == result)
				{
					if (entry->setStringBasedObjectDataEx(key, pElementData))
					{
						// cache enabled?
						entry->getStringBasedObjectDataEx(key, &pExistingElementData, requiredElementDataFilter);
					}
					else
					{
						pExistingElementData = pElementData;
					}

					handledEx = TRUE; // mark success in order to continue
				}
			}
		}

		if (handledEx)
		{
			if (pExistingElementData->m_ioMode != OTS_IOMODE_NONE)
			{
				getStandardTagProperties((VARTYPE)pExistingElementData->m_datatype, isSOAP, propertyList);
			}
		}
		
		if (pApp->m_callbackFunctions.m_OTSQueryProperties)
		{
			DWORD propCountO = 0;
			OTSPropertyData* pPropDataO = NULL;
			DWORD i;
			SODaSPropertyData* pPropData;

			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSQueryProperties(NULL, (LPTSTR)itemId, 0, &propCountO, &pPropDataO);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::queryAvailablePropertiesByItemID - Exception occurred in vendor application callback: OTSQueryProperties")));
			}

			for (i = 0; i < propCountO; i++)
			{
				SOCMN_ALLOCATE_OBJECT(pPropData, SODaSPropertyData());
				ServerDAAddressSpaceElement::clonePropertyData(NULL, &pPropDataO[i], pPropData);
				propertyList.add(pPropData);
			}

			for (i = 0; i < propCountO; i++)
			{
				FreeOTSPropertyData(&pPropDataO[i]);
			}

			OTFreeMemory(pPropDataO);
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::queryAvailablePropertiesByItemID - Exception occurred")));
	}
}

BOOL ServerDAServer::isPropertyName(
	IN LPCTSTR itemId,
	IN DWORD propId,
	IN LPCTSTR /* propIdString */)
{
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		bool handledEx = false;
		OTSAddressSpaceElementData* pElementData = NULL;
		OTSAddressSpaceElementData* pExistingElementData = NULL;
		unsigned short elementDataFilter = OTS_ELEMENTDATA_IOMODE;

		if (isStandardTagProperty(propId))
		{
			if (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx)
			{
				if (!entry->getStringBasedObjectDataEx(itemId, &pExistingElementData, elementDataFilter))
				{
					pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
					elementDataFilter = OTS_ELEMENTDATA_IOMODE;
					long result = S_FALSE;

					SOCMN_TRY
					{
						result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementDataEx((LPTSTR)itemId, OT_ADDRESSSPACEELEMENT_TYPE_DA,
							elementDataFilter, pElementData);
					}
					SOCMN_CATCH(...)
					{
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isPropertyName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementDataEx")));
					}

					if (S_OK == result)
					{
						entry->setStringBasedObjectDataEx(itemId, pElementData, elementDataFilter);
						ret = (pElementData->m_ioMode != OTS_IOMODE_NONE);
						handledEx = true;
					}
				}
				else
				{
					ret = (pExistingElementData->m_ioMode != OTS_IOMODE_NONE);
					handledEx = true;
				}
			}

			if ((handledEx == false) && (pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData))
			{
				elementDataFilter = OTS_ELEMENTDATA_IOMODE;

				if (!entry->getStringBasedObjectDataEx(itemId, &pExistingElementData, elementDataFilter))
				{
					if (!pElementData)
					{
						pElementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
					}
					else
					{
						FreeOTSAddressSpaceElementData(pElementData);
					}

					long result = S_FALSE;

					SOCMN_TRY
					{
						result = pApp->m_callbackFunctions.m_OTSQueryAddressSpaceElementData((LPTSTR)itemId, OT_ADDRESSSPACEELEMENT_TYPE_DA, pElementData);
					}
					SOCMN_CATCH(...)
					{
						_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isPropertyName - Exception occurred in vendor application callback: OTSQueryAddressSpaceElementData")));
					}

					if (S_OK == result)
					{
						ret = (pElementData->m_ioMode != OTS_IOMODE_NONE);
						entry->setStringBasedObjectDataEx(itemId, pElementData);
					}
				}
				else
				{
					ret = (pExistingElementData->m_ioMode != OTS_IOMODE_NONE);
				}
			}

			return ret;
		}

		FreeOTSAddressSpaceElementData(pElementData);
		OTFreeMemory(pElementData);

		if (pApp->m_callbackFunctions.m_OTSQueryProperties)
		{
			DWORD propCountO = 0;
			OTSPropertyData* pPropDataO = NULL;
			DWORD i;

			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSQueryProperties(NULL, (LPTSTR)itemId, propId, &propCountO, &pPropDataO);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isPropertyName - Exception occurred in vendor application callback: OTSQueryProperties")));
			}

			for (i = 0; i < propCountO; i++)
			{
				if (pPropDataO[i].m_pid == propId)
				{
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
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::isPropertyName - Exception occurred")));
	}

	return ret;
}

// overrides the onSetClientName
void ServerDAServer::onSetClientName(IN LPCTSTR clientName)
{
	ServerApp* pApp = ::getServerApp();
	OTSSessionData clientStateData;
	memset(&clientStateData, 0, sizeof(OTSSessionData));

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		clientStateData.m_clientName = (LPTSTR)clientName;
		clientStateData.m_state = OTS_SESSIONSTATE_MODIFY;
		clientStateData.m_type = getFlags();

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::onSetClientName - Exception occurred in vendor application callback: OTSChangeSessionState")));
		}
	}
}

//-----------------------------------------------------------------------------
// waitForNameSpaceStartup
//
// - wait for the name space creation to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT ServerDAServer::waitForNameSpaceStartup(void)
{
	ServerApp* pServerApp = getServerApp();

	if (pServerApp->m_startupEvent)
	{
		pServerApp->m_startupEvent->waitFor(INFINITE);
	}

	return S_OK;
} // waitForNameSpaceStartup


//-----------------------------------------------------------------------------
// waitForIOStartup
//
// - wait for the I/O system to start up
//
// returns:
//		S_OK - continue COM interface function call
//		any error returns COM interface with this error
//
HRESULT ServerDAServer::waitForIOStartup(void)
{
	ServerApp* pServerApp = getServerApp();

	if (pServerApp->m_startupEvent)
	{
		pServerApp->m_startupEvent->waitFor(INFINITE);
	}

	return S_OK;
} // waitForIOStartup

BOOL ServerDAServer::isPrivateSecurityAvailable(void)
{
	return TRUE; // allways provide the security interface
}

BOOL ServerDAServer::logon(LPCTSTR userName, LPCTSTR password)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
		{
			OTSSessionData clientStateData;
			clientStateData.m_state = OTS_SESSIONSTATE_LOGON;
			clientStateData.m_type = getFlags();
			clientStateData.m_userName = (LPTSTR)userName;
			clientStateData.m_password = (LPTSTR)password;
			clientStateData.m_clientName = NULL;
			clientStateData.m_clientIp = NULL;
			clientStateData.m_clientRemote = 0;

			SOCMN_TRY
			{
				ret = (pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData) == S_OK);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::logon - Exception occurred in vendor application callback: OTSChangeSessionState")));
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::logon - Exception occurred")));
	}

	return ret;
}

BOOL ServerDAServer::logoff(void)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
		{
			OTSSessionData clientStateData;
			clientStateData.m_state = OTS_SESSIONSTATE_LOGOFF;
			clientStateData.m_type = getFlags();
			clientStateData.m_password = NULL;
			clientStateData.m_userName = NULL;
			clientStateData.m_clientName = NULL;
			clientStateData.m_clientIp = NULL;
			clientStateData.m_clientRemote = 0;

			SOCMN_TRY
			{
				ret = (pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData) == S_OK);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::logoff - Exception occurred in vendor application callback: OTSChangeSessionState")));
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::logon - Exception occurred")));
	}

	return ret;
}

HRESULT ServerDAServer::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
}

#ifdef OTB_XMLDA_SERVER

BOOL ServerDAServer::checkAuthorization(IN SODaSXmlMethod* pMth)
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSChangeSessionState)
	{
		SOCmnPointer<SOSrvHTTPHandler> httpH = pMth->getHTTPHandler();
		SOCmnStringList attributeList(httpH->getAttributeList());
		SOCmnString authorization;
		OTSSessionData clientStateData;
		memset(&clientStateData, 0, sizeof(OTSSessionData));
		BOOL calledLogon = FALSE;
		clientStateData.m_type = getFlags();

		if (attributeList.findKey(_T("authorization"), &authorization))
		{
			if (_tcsncmp(_T("Basic "), authorization, 6) == 0)
			{
				// authorization provided
				SOCmnString userPwd64;
				SOCmnString userPwd;
				LPTSTR userName, password;
				userPwd64 = authorization.right(authorization.getLength() - 6);
				SOCmnXmlValue::decodeBase64String2String(userPwd64, userPwd);
				SOCmnElement::reducePath(userPwd, userName, password, TRUE, _T(":"));
				clientStateData.m_state = OTS_SESSIONSTATE_LOGON;
				clientStateData.m_userName = userName;
				clientStateData.m_password = password;

				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::checkAuthorization - Exception occurred in vendor application callback: OTSChangeSessionState")));
				}

				calledLogon = TRUE;
			}
		}

		if (!calledLogon)
		{
			clientStateData.m_state = OTS_SESSIONSTATE_LOGOFF;
			clientStateData.m_userName = NULL;
			clientStateData.m_password = NULL;

			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSChangeSessionState((DWORD)this->getOpcHandle(), &clientStateData);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAServer::checkAuthorization - Exception occurred in vendor application callback: OTSChangeSessionState")));
			}
		}
	}

	return TRUE;
}
#endif // SOFEATURE_SOAP

#endif // OTB_SERVER_DA
