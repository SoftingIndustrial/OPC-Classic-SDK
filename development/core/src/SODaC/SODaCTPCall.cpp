//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SODAC                              |
//                                                                            |
//  Filename    : SODaCTPCall.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC XML-DA client                                           |
//                - SODaCTPCall: OPC XML-DA HTTP call class                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_TUNNEL

#include "SODaCTPCall.h"
#include "opcerror.h"

//-----------------------------------------------------------------------------
// SODaCTPCall                                                               |
//-----------------------------------------------------------------------------

SODaCTPCall::SODaCTPCall(IN SOCltTPConnection* pCon)
	: SOCmnTPCall(pCon)
{
}

SODaCTPCall::~SODaCTPCall(void)
{
}

HRESULT SODaCTPCall::getStatus(
	OUT SODaCServerStatus& status)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_GETSTATUS);       // function id
	request->MarshalChar(0);                                     // flags
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			ok &= response->UnMarshalLong(&status.m_state);
			ok &= response->UnMarshalDate(&status.m_startTime);
			ok &= response->UnMarshalDate(&status.m_currentTime);
			ok &= response->UnMarshalString(&status.m_vendorInfo);
			ok &= response->UnMarshalString(&status.m_productVersion);
			ok &= response->UnMarshalDWORDList(&status.m_supportedLCIDs);
			ok &= response->UnMarshalDate(&status.m_lastUpdateTime);
			ok &= response->UnMarshalLong(&status.m_groupCount);
			ok &= response->UnMarshalLong(&status.m_bandwidth);
			ok &= response->UnMarshalString(&status.m_statusInfo);
			status.m_supportedInterfaces.add(SODAC_VERSION_OPC_TPDA);
		}

		if (!ok)
		{
			status.clear();
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SODaCTPCall::browse(
	IN SOCltBrowseObject* pParent,
	IN LPCTSTR itemID,
	IN LPCTSTR cp,
	IN DWORD maxElements,
	IN DWORD browseFilter,
	IN LPCTSTR elementNameFilter,
	IN LPCTSTR vendorFilter,
	IN BOOL returnProperties,
	IN BOOL retrievePropertyValues,
	IN SOCmnList<SODaCPropertyData> *propteryList,
	OUT SOCmnList<SOCltBrowseObject> &objList,
	OUT SOCmnString* cpRet,
	OUT BOOL* moreElements)
{
	SOCmnPointer<SODaCEntry> entryDaC = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = entryDaC->getCreator();
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_BROWSE);          // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(itemID);
	request->MarshalString(cp);
	request->MarshalLong(maxElements);
	request->MarshalLong(browseFilter);
	request->MarshalString(elementNameFilter);
	request->MarshalString(vendorFilter);
	request->MarshalChar((BYTE)returnProperties);
	request->MarshalChar((BYTE)retrievePropertyValues);

	if (returnProperties)
	{
		// return all properties
		request->MarshalLong(0);
	}
	else
	{
		if (propteryList)
		{
			SOCmnList<SODaCPropertyData> propDList(propteryList);
			SOCmnListPosition propDpos;
			SODaCPropertyData* pPropD;
			request->MarshalLong(propDList.getCount());
			propDpos = propDList.getStartPosition();

			while (propDpos)
			{
				pPropD = propDList.getNext(propDpos);
				request->MarshalLong(pPropD->m_propertyID);
				request->MarshalString(pPropD->m_name);
			}
		}
		else
		{
			request->MarshalLong(0);
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		SOCmnString cpReturn;
		BYTE moreEl = 0;
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD elementCnt;
			DWORD propertyCnt;
			DWORD i, j;
			SOCmnString name;
			SOCmnString itemId;
			DWORD flags;
			SOCmnPointer<SOCltBrowseObject> bO;
			SOCmnList<SODaCProperty> propList;
			SOCmnPointer<SODaCProperty> prop;
			HRESULT propertiesResult;
			HRESULT propertyResult;
			WORD propertyDatatype;
			DWORD propertyId;
			SOCmnString propertyItemId;
			SOCmnString propertyDescription;
			SOCmnString propertyName;
			SOCmnVariant propertyValue;
			ok &= response->UnMarshalString(&cpReturn);
			ok &= response->UnMarshalChar(&moreEl);
			ok &= response->UnMarshalLong(&elementCnt);

			if (ok)
			{
				for (i = 0; ((i < elementCnt) && (ok)); i++)
				{
					// browse element data
					ok &= response->UnMarshalString(&name);
					ok &= response->UnMarshalString(&itemId);
					ok &= response->UnMarshalLong(&flags);

					if (!ok)
					{
						break;
					}

					if ((flags & OPC_BROWSE_HASCHILDREN) ||                            // children
						((flags & (OPC_BROWSE_HASCHILDREN | OPC_BROWSE_ISITEM)) == 0)) // no children and no item
					{
						// node
						SODaCNode* pNode;
						pNode = creator->createNode(pParent, name);
						pNode->setItemID(itemId, itemID);
						pNode->setFlags(flags);
						bO = pNode;
						propList = pNode->getPropertyList();
					}
					else
					{
						// tag
						SODaCTag* pTag;
						pTag = creator->createTag(pParent, name);
						pTag->setItemID(itemId, itemID);
						pTag->setFlags(flags);
						bO = pTag;
						propList = pTag->getPropertyList();
					}

					objList.add(bO);
					// property data
					ok &= response->UnMarshalLong(&propertiesResult);

					if (SUCCEEDED(propertiesResult))
					{
						ok &= response->UnMarshalLong(&propertyCnt);

						for (j = 0; ((j < propertyCnt) && (ok)); j++)
						{
							ok &= response->UnMarshalLong(&propertyResult);
							ok &= response->UnMarshalShort(&propertyDatatype);
							ok &= response->UnMarshalLong(&propertyId);
							ok &= response->UnMarshalString(&propertyName);
							ok &= response->UnMarshalString(&propertyItemId);
							ok &= response->UnMarshalString(&propertyDescription);
							ok &= response->UnMarshalVariant(&propertyValue);

							if (!ok)
							{
								break;
							}

							prop = creator->createProperty(bO, propertyDescription, propertyId, propertyDatatype, itemId, _T(""));
							prop->setItemID(propertyItemId, itemID);
							prop->setPropertyName(propertyName);
							prop->setValue(propertyValue);
							propList.add(prop);
						}
					}
				}
			}
		}

		if (ok)
		{
			if (cpRet)
			{
				*cpRet = cpReturn;
			}

			if (moreElements)
			{
				*moreElements = moreEl;
			}
		}
		else
		{
			objList.removeAll();
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SODaCTPCall::getProperties(
	IN LPCTSTR itemID,
	IN BOOL retrievePropertyValues,
	IN BOOL returnAllProperties,
	IN OUT SOCmnList<SODaCPropertyData> &propteryList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 256);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_GETPROPERTIES);   // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(1);
	request->MarshalString(itemID);
	request->MarshalChar((BYTE)retrievePropertyValues);

	if (returnAllProperties)
	{
		// return all properties
		request->MarshalLong(0);
		propteryList.removeAll();
	}
	else
	{
		SOCmnList<SODaCPropertyData> propDList(propteryList);
		SOCmnListPosition propDpos;
		SODaCPropertyData* pPropD;
		request->MarshalLong(propDList.getCount());
		propDpos = propDList.getStartPosition();

		while (propDpos)
		{
			pPropD = propDList.getNext(propDpos);
			request->MarshalLong(pPropD->m_propertyID);
			request->MarshalString(pPropD->m_name);
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		SOCmnListPosition posP;
		SODaCPropertyData* prop;
		BOOL ok;
		HRESULT propertyResult;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);
		ok &= response->UnMarshalLong(&propertyResult);

		if ((ok) && (SUCCEEDED(res)) && (SUCCEEDED(propertyResult)))
		{
			DWORD propertyCnt;
			DWORD j;
			ok &= response->UnMarshalLong(&propertyCnt);
			posP = propteryList.getStartPosition();

			for (j = 0; ((j < propertyCnt) && (ok)); j++)
			{
				if (returnAllProperties)
				{
					SOCMN_ALLOCATE_OBJECT(prop, SODaCPropertyData);
				}
				else
				{
					prop = propteryList.getNext(posP);

					if (!prop)
					{
						return E_FAIL;
					}
				}

				ok &= response->UnMarshalLong(&prop->m_error);
				ok &= response->UnMarshalShort(&prop->m_datatype);
				ok &= response->UnMarshalLong(&prop->m_propertyID);
				ok &= response->UnMarshalString(&prop->m_name);
				ok &= response->UnMarshalString(&prop->m_itemID);
				ok &= response->UnMarshalString(&prop->m_description);
				ok &= response->UnMarshalVariant(&prop->m_value);

				if (returnAllProperties)
				{
					propteryList.add(prop);
				}
			}
		}
		else
		{
			if (SUCCEEDED(res))
			{
				res = propertyResult;
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SODaCTPCall::read(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN DWORD maxAge,
	OUT SOCmnVariant* pValues,
	OUT WORD* pQualities,
	OUT SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 32));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_READ);            // function id
	request->MarshalChar(0);                                     // flags
	DWORD i;
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		request->MarshalString(pItemIDs[i]);
		request->MarshalLong(maxAge);
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			WORD quality;
			SOCmnDateTime timestamp;

			for (i = 0; ((i < count) && (ok)); i++)
			{
				ok = response->UnMarshalLong(&itemRes);

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}

				if (SUCCEEDED(itemRes))
				{
					ok = response->UnMarshalVariant(&pValues[i]);

					if (pQualities)
					{
						ok = response->UnMarshalShort(&pQualities[i]);
					}
					else
					{
						ok = response->UnMarshalShort(&quality);
					}

					if (pTimestamps)
					{
						ok = response->UnMarshalDate(&pTimestamps[i]);
					}
					else
					{
						ok = response->UnMarshalDate(&timestamp);
					}
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SODaCTPCall::write(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 32));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_WRITE);           // function id
	request->MarshalChar(0);                                     // flags
	DWORD i;
	BYTE qualitySpecified;
	BYTE timestampSpecified;
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		request->MarshalString(pItemIDs[i]);
		request->MarshalVariant(pValues[i]);
		qualitySpecified = FALSE;

		if (pQualities)
		{
			if (pQualities[i] != SODAC_QUALITY_NONE)
			{
				request->MarshalChar(TRUE);
				request->MarshalShort(pQualities[i]);
				qualitySpecified = TRUE;
			}
		}

		if (!qualitySpecified)
		{
			request->MarshalChar(FALSE);
		}

		timestampSpecified = FALSE;

		if (pTimestamps)
		{
			if (pTimestamps[i].isSet())
			{
				request->MarshalChar(TRUE);
				request->MarshalDate(pTimestamps[i]);
				timestampSpecified = TRUE;
			}
		}

		if (!timestampSpecified)
		{
			request->MarshalChar(FALSE);
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;

			for (i = 0; ((i < count) && (ok)); i++)
			{
				ok &= response->UnMarshalLong(&itemRes);

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SODaCTPCall::addGroup(
	IN SODaCGroup* pGroup,
	OUT DWORD& groupServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_ADDGROUP);        // function id
	request->MarshalChar(0);                                     // flags
	SOCmnString grpName;
	DWORD updateRate = 0;
	DWORD keepAliveTime = 0;
	DWORD valPars;
	valPars = pGroup->getValidParameters();
	grpName = pGroup->getName();

	if (valPars & SODAC_GRPPAR_UPDATERATE)
	{
		pGroup->getUpdateRate(&updateRate);
	}

	if (valPars & SODAC_GRPPAR_KEEPALIVETIME)
	{
		pGroup->getKeepAliveTime(&keepAliveTime);
	}

	request->MarshalLong((DWORD)pGroup->getOpcHandle());
	request->MarshalString(grpName);
	request->MarshalChar(pGroup->getTargetObjectState() == SOCltElement::started); // active
	request->MarshalChar(TRUE);                                   // enable
	request->MarshalLong(updateRate);                             // update rate
	request->MarshalLong(keepAliveTime);                          // keep alive time

	if (valPars & SODAC_GRPPAR_DEADBAND)
	{
		request->MarshalFloat(pGroup->getDeadBand());    // deadband
	}
	else
	{
		request->MarshalFloat(0.0);
	}

	if (valPars & SODAC_GRPPAR_TIMEBIAS)
	{
		request->MarshalChar(TRUE);
		request->MarshalLong(pGroup->getTimeBias());
	}
	else
	{
		request->MarshalChar(FALSE);
		request->MarshalLong(0);
	}

	if (valPars & SODAC_GRPPAR_LCID)
	{
		request->MarshalChar(TRUE);
		request->MarshalLong(pGroup->getLCID());
	}
	else
	{
		request->MarshalChar(FALSE);
		request->MarshalLong(0);
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD revUpdateRate;
			DWORD revKeepAliveTime;
			SOCmnString revGrpName;
			DWORD revTimeBias;
			DWORD revLCID;
			ok &= response->UnMarshalLong(&groupServerHandle);
			ok &= response->UnMarshalString(&revGrpName);
			ok &= response->UnMarshalLong(&revUpdateRate);
			ok &= response->UnMarshalLong(&revKeepAliveTime);
			ok &= response->UnMarshalLong(&revTimeBias);
			ok &= response->UnMarshalLong(&revLCID);

			if (ok)
			{
				pGroup->setName(revGrpName);
				pGroup->m_revUpdateRate = revUpdateRate;
				INVOKE_CALLBACK_ON_PTR(pGroup, SODaCGroup, onSetRevUpdateRate);
				pGroup->m_revKeepAliveTime = revKeepAliveTime;
				INVOKE_CALLBACK_ON_PTR(pGroup, SODaCGroup, onSetRevKeepAliveTime);

				if (!(valPars & SODAC_GRPPAR_TIMEBIAS))
				{
					pGroup->setTimeBias(revTimeBias);
				}

				if (!(valPars & SODAC_GRPPAR_LCID))
				{
					pGroup->setLCID(revLCID);
				}
			}
			else
			{
				groupServerHandle = 0;
				res = RPC_E_INVALID_DATA;
			}
		}
	}

	return res;
}


HRESULT SODaCTPCall::removeGroup(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_REMOVEGROUP);     // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalLong(groupServerHandle);
	request->MarshalChar(FALSE);                                 // forced
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SODaCTPCall::updateGroup(
	IN BOOL /* fromServer */,
	IN DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}

HRESULT SODaCTPCall::setClientName(
	IN LPCTSTR clientName)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(TP_OBJECT_ID_CONNECTION_OBJECT);        // object id
	request->MarshalShort(TP_INTERFACE_DA_SERVER);               // interface id
	request->MarshalChar(TP_FUNCTION_DA_SERVER_SETCLIENTNAME);   // function id
	request->MarshalChar(0);                                     // flags
	request->MarshalString(clientName);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}   //  end setClientName


HRESULT SODaCTPCall::addItems(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN SOCmnList<SODaCItem>* itemList,
	IN SOCmnDWORDList* targetStateList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	DWORD itemCnt = itemList->getCount();

	if (itemCnt == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + (itemCnt * 64));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_ADDITEMS);         // function id
	request->MarshalChar(0);                                     // flags
	SOCmnListPosition pos;
	SODaCItem* pItem;
	FLOAT deadband = 0;
	request->MarshalLong(itemCnt);
	pos = itemList->getStartPosition();

	while (pos)
	{
		pItem = itemList->getNext(pos);
		request->MarshalLong((DWORD)pItem->getOpcHandle());
		request->MarshalString(pItem->getItemID());
		request->MarshalChar(pItem->getTargetObjectState() == SOCltElement::started); // active
		request->MarshalShort(pItem->getReqDatatype());
		request->MarshalString(pItem->getAccessPath());
		deadband = pItem->getDeadBand();
		request->MarshalChar(deadband != SODACITEM_NO_DEADBAND);
		request->MarshalFloat(deadband);
	}

	itemList->autoUnlock();
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			DWORD serverItemHandle;
			WORD nativeDatatype;
			DWORD accessRights;
			FLOAT revDeadband;
			DWORD euType;
			SOCmnVariant euInfo;
			DWORD targetState;
			SOCmnListPosition posTS;
			pos = itemList->getStartPosition();
			posTS = targetStateList->getStartPosition();

			while ((pos) && (posTS) && (ok))
			{
				pItem = itemList->getNext(pos);
				targetState = targetStateList->getNext(posTS);
				ok &= response->UnMarshalLong(&itemRes);
				pItem->setLastError(itemRes, SODAC_ERR_ITM_ADD_ITEM);

				if ((ok) && (SUCCEEDED(itemRes)))
				{
					ok &= response->UnMarshalLong(&serverItemHandle);
					ok &= response->UnMarshalShort(&nativeDatatype);
					ok &= response->UnMarshalLong(&accessRights);
					ok &= response->UnMarshalLong(&euType);
					ok &= response->UnMarshalVariant(&euInfo);
					ok &= response->UnMarshalFloat(&revDeadband);

					if (revDeadband < 0.0)
					{
						revDeadband = SODACITEM_NO_DEADBAND;
					}

					pItem->setObjectState((BYTE)targetState);
					pItem->setServerHandle(serverItemHandle);
					pItem->setNativeDatatype(nativeDatatype);
					pItem->setAccessRights(accessRights);
					pItem->setEU((OPCEUTYPE)euType, euInfo);

					if (deadband != revDeadband)
					{
						if (revDeadband == SODACITEM_NO_DEADBAND)
						{
							pItem->setLastError(OPC_E_DEADBANDNOTSUPPORTED, SODAC_ERR_GRP_ITM_SET_DEADBAND);
						}
						else
						{
							pItem->setLastError(E_FAIL, SODAC_ERR_GRP_ITM_SET_DEADBAND);
						}
					}
				}
				else
				{
					pItem->setObjectState(SOCltElement::disconnected);
				}
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}

HRESULT SODaCTPCall::removeItems(
	IN SODaCGroup* /* pGroup */,
	DWORD groupServerHandle,
	IN SOCmnList<SODaCItem> *itemList)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	DWORD itemCnt = itemList->getCount();

	if (itemCnt == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (itemCnt * 4));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_REMOVEITEMS);      // function id
	request->MarshalChar(0);                                     // flags
	SOCmnListPosition pos;
	SODaCItem* pItem;
	request->MarshalLong(itemCnt);
	pos = itemList->getStartPosition();

	while (pos)
	{
		pItem = itemList->getNext(pos);
		pItem->setObjectState(SOCltElement::disconnected);
		request->MarshalLong(pItem->getServerHandle());
	}

	itemList->autoUnlock();
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			pos = itemList->getStartPosition();

			while ((pos) && (ok))
			{
				pItem = itemList->getNext(pos);
				ok &= response->UnMarshalLong(&itemRes);
				pItem->setLastError(itemRes, SODAC_ERR_ITM_REMOVE_ITEM);
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SODaCTPCall::validateItems(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN SOCmnList<SODaCItem> *itemList,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	DWORD itemCnt = itemList->getCount();

	if (itemCnt == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + (itemCnt * 64));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_VALIDATEITEMS);    // function id
	request->MarshalChar(0);                                     // flags
	SOCmnListPosition pos;
	SODaCItem* pItem;
	DWORD i;
	request->MarshalLong(itemCnt);
	pos = itemList->getStartPosition();

	while (pos)
	{
		pItem = itemList->getNext(pos);
		request->MarshalString(pItem->getItemID());
		request->MarshalShort(pItem->getReqDatatype());
		request->MarshalString(pItem->getAccessPath());
	}

	itemList->autoUnlock();
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			WORD nativeDatatype;
			DWORD accessRights;
			pos = itemList->getStartPosition();

			for (i = 0; ((i < itemCnt) && (ok)); i++)
			{
				pItem = itemList->getNext(pos);

				if (!pItem)
				{
					return E_FAIL;
				}

				ok &= response->UnMarshalLong(&itemRes);

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}

				pItem->setLastError(itemRes, SODAC_ERR_ITM_VALIDATE_ITEM);

				if ((ok) && (SUCCEEDED(itemRes)))
				{
					ok &= response->UnMarshalShort(&nativeDatatype);
					ok &= response->UnMarshalLong(&accessRights);
					pItem->setNativeDatatype(nativeDatatype);
					pItem->setAccessRights(accessRights);
				}

				_TRACEO_LEVEL_FROM_RESX(itemRes, SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("Validate item %s [0x%X]"), (LPCTSTR)pItem->getItemID(), (unsigned int)itemRes));
			}
		}

		if (!ok)
		{
			res = RPC_E_INVALID_DATA;
		}
	}

	return res;
}


HRESULT SODaCTPCall::setItemsState(
	IN SODaCGroup* /* pGroup */,
	IN SOCmnList<SODaCItem>* /* itemList */)
{
	return E_NOTIMPL;
}

HRESULT SODaCTPCall::updateItems(
	IN SODaCGroup* /* pGroup */,
	IN BOOL /* fromServer */,
	IN SOCmnList<SODaCItem>* /* itemList */)
{
	return E_NOTIMPL;
}

HRESULT SODaCTPCall::validateItems(
	IN SODaCGroup* /* pGroup */,
	IN DWORD /* count */,
	IN SODaCItem** /* itemArray */,
	OUT HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}


HRESULT SODaCTPCall::syncRead(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	OUT SOCmnVariant* pValues,
	OUT WORD* pQualities,
	OUT SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 8));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_SYNCREAD);         // function id
	request->MarshalChar(0);                                     // flags
	DWORD* pServerHandles;
	DWORD i;
	pServerHandles = new DWORD[count];
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		if (itemArray[i]->getObjectState() != SOCltElement::disconnected)
		{
			pServerHandles[i] = itemArray[i]->getServerHandle();
			request->MarshalLong(pServerHandles[i]);
			request->MarshalLong(maxAge);
		}
		else
		{
			pServerHandles[i] = 0;
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;

			for (i = 0; ((i < count) && (ok)); i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				ok &= response->UnMarshalLong(&itemRes);

				if (!ok)
				{
					break;
				}

				if (SUCCEEDED(itemRes))
				{
					SOCmnVariant value;
					WORD quality;
					SOCmnDateTime timestamp;
					ok &= response->UnMarshalVariant(&value);
					ok &= response->UnMarshalShort(&quality);
					ok &= response->UnMarshalDate(&timestamp);

					if (!ok)
					{
						break;
					}

					if (pValues)
					{
						value.copyTo(&pValues[i]);
					}

					if (pQualities)
					{
						pQualities[i] = quality;
					}

					if (pTimestamps)
					{
						pTimestamps[i] = timestamp;
					}

					_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C)
					{
						if (SUCCEEDED(itemRes))
						{
							SOCmnString v, q, t;
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; %s %s %s"), i, (LPCTSTR)itemArray[i]->getItemID(), variantWithType2string(&v, value), quality2string(&q, quality), timestamp.toString(t)));
						}
						else
						{
							_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; 0x%8.8X"), i, (LPCTSTR)itemArray[i]->getItemID(), (unsigned int)itemRes));
						}
					}
					itemArray[i]->setRead(itemRes, &value, quality, &timestamp, 0);
				}
				else
				{
					itemArray[i]->setRead(itemRes, NULL, OPC_QUALITY_BAD, NULL, 0);
				}

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}
			}
		}
		else
		{
			if (!ok)
			{
				res = RPC_E_INVALID_DATA;
			}

			for (i = 0; i < count; i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				itemArray[i]->setRead(res, NULL, OPC_QUALITY_BAD, NULL, 0);
			}
		}
	}

	delete[] pServerHandles;
	return res;
}


HRESULT SODaCTPCall::syncWrite(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 32));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_SYNCWRITE);        // function id
	request->MarshalChar(0);                                     // flags
	DWORD* pServerHandles;
	DWORD i;
	BYTE qualitySpecified;
	BYTE timestampSpecified;
	pServerHandles = new DWORD[count];
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		if (itemArray[i]->getObjectState() != SOCltElement::disconnected)
		{
			pServerHandles[i] = itemArray[i]->getServerHandle();
			request->MarshalLong(pServerHandles[i]);

			if (pValues)
			{
				request->MarshalVariant(pValues[i]);
			}
			else
			{
				SOCmnVariant value;
				itemArray[i]->getWriteValue(value);
				request->MarshalVariant(value);
			}

			qualitySpecified = FALSE;

			if (pQualities)
			{
				if (pQualities[i] != SODAC_QUALITY_NONE)
				{
					request->MarshalChar(TRUE);
					request->MarshalShort(pQualities[i]);
					qualitySpecified = TRUE;
				}
			}

			if (!qualitySpecified)
			{
				request->MarshalChar(FALSE);
			}

			timestampSpecified = FALSE;

			if (pTimestamps)
			{
				if (pTimestamps[i].isSet())
				{
					request->MarshalChar(TRUE);
					request->MarshalDate(pTimestamps[i]);
					timestampSpecified = TRUE;
				}
			}

			if (!timestampSpecified)
			{
				request->MarshalChar(FALSE);
			}
		}
		else
		{
			pServerHandles[i] = 0;
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;

			for (i = 0; ((i < count) && (ok)); i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				ok &= response->UnMarshalLong(&itemRes);

				if (FAILED(itemRes))
				{
					itemArray[i]->setWrite(itemRes, 0);
				}

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}
			}
		}
		else
		{
			if (!ok)
			{
				res = RPC_E_INVALID_DATA;
			}

			for (i = 0; i < count; i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				itemArray[i]->setWrite(res, 0);
			}
		}
	}

	delete[] pServerHandles;
	return res;
}

HRESULT SODaCTPCall::asyncRead(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	IN DWORD transactionID,
	OUT DWORD* cancelID,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 8));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_ASYNCREAD);        // function id
	request->MarshalChar(0);                                     // flags
	DWORD* pServerHandles;
	DWORD i;
	pServerHandles = new DWORD[count];

	if (transactionID == 0)
	{
		transactionID = ::GetTickCount();

		if ((transactionID == 0) || (transactionID >= SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED))
		{
			transactionID = 1;
		}
	}

	request->MarshalLong(transactionID);
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		if (itemArray[i]->getObjectState() != SOCltElement::disconnected)
		{
			pServerHandles[i] = itemArray[i]->getServerHandle();
			request->MarshalLong(pServerHandles[i]);
			request->MarshalLong(maxAge);
		}
		else
		{
			pServerHandles[i] = 0;
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			DWORD cID;
			ok = response->UnMarshalLong(&cID);

			if (cancelID)
			{
				*cancelID = cID;
			}

			for (i = 0; ((i < count) && (ok)); i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				ok &= response->UnMarshalLong(&itemRes);

				if (FAILED(itemRes))
				{
					itemArray[i]->setRead(itemRes, NULL, OPC_QUALITY_BAD, NULL, transactionID);
				}

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}
			}
		}
		else
		{
			if (!ok)
			{
				res = RPC_E_INVALID_DATA;
			}

			for (i = 0; i < count; i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				itemArray[i]->setRead(res, NULL, OPC_QUALITY_BAD, NULL, transactionID);
			}
		}
	}

	delete[] pServerHandles;
	return res;
}

HRESULT SODaCTPCall::asyncWrite(
	IN SODaCGroup* /* pGroup */,
	IN DWORD groupServerHandle,
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD transactionID,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	OUT DWORD* cancelID,
	OUT HRESULT* pErrors)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;

	if (count == 0)
	{
		return S_OK;
	}

	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4 + (count * 32));
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_ASYNCWRITE);       // function id
	request->MarshalChar(0);                                     // flags
	DWORD* pServerHandles;
	DWORD i;
	BYTE qualitySpecified;
	BYTE timestampSpecified;
	pServerHandles = new DWORD[count];

	if (transactionID == 0)
	{
		transactionID = ::GetTickCount();

		if ((transactionID == 0) || (transactionID >= SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED))
		{
			transactionID = 1;
		}
	}

	request->MarshalLong(transactionID);
	request->MarshalLong(count);

	for (i = 0; i < count; i++)
	{
		if (itemArray[i]->getObjectState() != SOCltElement::disconnected)
		{
			pServerHandles[i] = itemArray[i]->getServerHandle();
			request->MarshalLong(pServerHandles[i]);

			if (pValues)
			{
				request->MarshalVariant(pValues[i]);
			}
			else
			{
				SOCmnVariant value;
				itemArray[i]->getWriteValue(value);
				request->MarshalVariant(value);
			}

			qualitySpecified = FALSE;

			if (pQualities)
			{
				if (pQualities[i] != SODAC_QUALITY_NONE)
				{
					request->MarshalChar(TRUE);
					request->MarshalShort(pQualities[i]);
					qualitySpecified = TRUE;
				}
			}

			if (!qualitySpecified)
			{
				request->MarshalChar(FALSE);
			}

			timestampSpecified = FALSE;

			if (pTimestamps)
			{
				if (pTimestamps[i].isSet())
				{
					request->MarshalChar(TRUE);
					request->MarshalDate(pTimestamps[i]);
					timestampSpecified = TRUE;
				}
			}

			if (!timestampSpecified)
			{
				request->MarshalChar(FALSE);
			}
		}
		else
		{
			pServerHandles[i] = 0;
		}
	}

	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			HRESULT itemRes;
			DWORD cID;
			ok = response->UnMarshalLong(&cID);

			if (cancelID)
			{
				*cancelID = cID;
			}

			for (i = 0; ((i < count) && (ok)); i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				ok &= response->UnMarshalLong(&itemRes);

				if (FAILED(itemRes))
				{
					itemArray[i]->setWrite(itemRes, transactionID);
				}

				if (pErrors)
				{
					pErrors[i] = itemRes;
				}
			}
		}
		else
		{
			if (!ok)
			{
				res = RPC_E_INVALID_DATA;
			}

			for (i = 0; i < count; i++)
			{
				if (pServerHandles[i] == 0)
				{
					continue;
				}

				itemArray[i]->setWrite(res, transactionID);
			}
		}
	}

	delete[] pServerHandles;
	return res;
}


HRESULT SODaCTPCall::refresh(
	IN SODaCGroup* pGroup,
	IN DWORD groupServerHandle,
	IN DWORD maxAge,
	IN DWORD transactionID,
	OUT DWORD* cancelID)
{
	SOCmnPointer<SOCmnTPConnection> tpCon = getTPConnection();
	SOCmnPointer<SOCmnTP_Marshaler> request = getSndMsg();
	HRESULT res;
	// build request
	request->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // message length
	request->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);        // invoke id
	request->MarshalLong(groupServerHandle);                     // object id
	request->MarshalShort(TP_INTERFACE_DA_GROUP);                // interface id
	request->MarshalChar(TP_FUNCTION_DA_GROUP_REFRESH);          // function id
	request->MarshalChar(0);                                     // flags

	if (transactionID == 0)
	{
		transactionID = ::GetTickCount();

		if ((transactionID == 0) || (transactionID >= SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED))
		{
			transactionID = 1;
		}
	}

	request->MarshalLong(transactionID);
	request->MarshalLong(maxAge);
	// send request and wait for response
	res = tpCon->invoke(this);

	if (SUCCEEDED(res))
	{
		// handle response
		SOCmnPointer<SOCmnTP_UnMarshaler> response = getRcvMsg();
		BOOL ok;
		response->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
		ok = response->UnMarshalLong(&res);

		if ((ok) && (SUCCEEDED(res)))
		{
			DWORD cID;
			ok = response->UnMarshalLong(&cID);

			if (cancelID)
			{
				*cancelID = cID;
			}
		}
		else
		{
			if (!ok)
			{
				res = RPC_E_INVALID_DATA;
			}
		}
	}

	return res;
} // refresh


//-----------------------------------------------------------------------------
// SODaCTPReceiver                                                            |
//-----------------------------------------------------------------------------

SODaCTPReceiver::SODaCTPReceiver(void)
{
}

void SODaCTPReceiver::closeConnection(IN SOCmnTPConnection* pCon)
{
	SOCmnPointer<SODaCServer> server = (SODaCServer*)((SOCltTPConnection*)pCon)->getServer();

	if (server.isNotNull())
	{
		server->connectionFailure();
	}
}

LONG SODaCTPReceiver::handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
{
	if (SOCLT_TP_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	if (interfaceId != TP_INTERFACE_DA_CALLBACK)
	{
		return E_FAIL;
	}

	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)tpCon->getServer();

	if (!server)
	{
		return E_FAIL;
	}

	switch (functionId)
	{
	case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
		tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
		break;

	case TP_FUNCTION_DA_CALLBACK_ONDATACHANGE:
	case TP_FUNCTION_DA_CALLBACK_ONREADCOMPLETE:
		onDataChange(server, pCall, functionId);
		break;

	case TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE:
		onWriteComplete(server, pCall);
		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}

void SODaCTPReceiver::onDataChange(IN SODaCServer* pServer, IN SOCmnTPCall* pCall, BYTE functionId)
{
	SOCmnPointer<SOCmnTP_UnMarshaler> notification = pCall->getRcvMsg();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SODaCGroup> group;
	BOOL ok;
	DWORD groupClientHandle;
	// unmarshal input
	groupClientHandle = pCall->getObjectId();
	group = pServer->getGroupByClientHandle(groupClientHandle);

	if (!group)
	{
		tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, functionId, E_INVALIDARG);
		return;
	}

	SOCmnPointer<SODaCItem> item;
	size_t* pItems;
	SOCmnVariant* pValues;
	WORD* pQualities;
	SOCmnDateTime* pTimestamps;
	HRESULT* pErrors;
	DWORD i;
	DWORD transactionId;
	HRESULT masterError;
	HRESULT masterQuality;
	DWORD itemCnt;
	DWORD rellevantItemCount = 0;
	DWORD itemClientHandle;
	notification->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = notification->UnMarshalLong(&transactionId);
	ok &= notification->UnMarshalLong(&masterError);
	ok &= notification->UnMarshalLong(&masterQuality);
	ok &= notification->UnMarshalArrayLen(&itemCnt);
	pItems = new size_t[itemCnt];
	pValues = new SOCmnVariant[itemCnt];
	pQualities = new WORD[itemCnt];
	pTimestamps = new SOCmnDateTime[itemCnt];
	pErrors = new HRESULT[itemCnt];

	if (!pItems || !pValues || !pQualities || !pTimestamps || !pErrors )
	{
		delete[] pItems;
		delete[] pValues;
		delete[] pQualities;
		delete[] pTimestamps;
		delete[] pErrors;
		tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, functionId, E_OUTOFMEMORY);
		return;
	}

	for (i = 0; i < itemCnt; i++)
	{
		pItems[i] = 0;
	}

	if (functionId == TP_FUNCTION_DA_CALLBACK_ONDATACHANGE)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("TPDA::Group::OnDataChange %lu"), itemCnt));
	}
	else
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("TPDA::Group::OnReadComplete %lu ; transaction id = %lu"), itemCnt, transactionId));
	}

	for (i = 0; ((ok) && (i < itemCnt)); i++)
	{
		ok &= notification->UnMarshalLong(&itemClientHandle);
		ok &= notification->UnMarshalLong(&pErrors[rellevantItemCount]);
		ok &= notification->UnMarshalVariant(&pValues[rellevantItemCount]);
		ok &= notification->UnMarshalShort(&pQualities[rellevantItemCount]);
		ok &= notification->UnMarshalDate(&pTimestamps[rellevantItemCount]);

		if (!ok)
		{
			break;
		}

		item = group->getItemByClientHandle(itemClientHandle);

		if (item.isNotNull())
		{
			item.addRef();
			pItems[rellevantItemCount] = (size_t)(void*)(SODaCItem*)item;
			_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C)
			{
				if (SUCCEEDED(pErrors[i]))
				{
					SOCmnString v, q, t;
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; %s %s %s"), i, (LPCTSTR)item->getItemID(), variantWithType2string(&v, pValues[i]), quality2string(&q, pQualities[i]), pTimestamps[i].toString(t)));
				}
				else
				{
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; 0x%8.8X"), i, (LPCTSTR)item->getItemID(), (unsigned int)pErrors[i]));
				}
			}
			item->setRead(pErrors[rellevantItemCount], &pValues[rellevantItemCount], pQualities[rellevantItemCount], &pTimestamps[rellevantItemCount], transactionId);
			++rellevantItemCount;
		}
		else
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, itemClientHandle));
		}
	}

	if (ok)
	{
		group->onReadAdvise((functionId == TP_FUNCTION_DA_CALLBACK_ONDATACHANGE), transactionId, masterQuality, masterError, rellevantItemCount,
							(SODaCItem**)pItems, pValues, pQualities, pTimestamps, pErrors);
	}

	for (i = 0; i < rellevantItemCount; i++)
	{
		((SODaCItem*)pItems[i])->release();
	}

	delete[] pItems;
	delete[] pValues;
	delete[] pQualities;
	delete[] pTimestamps;
	delete[] pErrors;
	tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, functionId, ok ? S_OK : RPC_E_INVALID_DATA);
	return;
}


void SODaCTPReceiver::onWriteComplete(IN SODaCServer* pServer, IN SOCmnTPCall* pCall)
{
	SOCmnPointer<SOCmnTP_UnMarshaler> notification = pCall->getRcvMsg();
	SOCmnPointer<SOCltTPConnection> tpCon = (SOCltTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SODaCGroup> group;
	BOOL ok;
	DWORD groupClientHandle;
	// unmarshal input
	groupClientHandle = pCall->getObjectId();
	group = pServer->getGroupByClientHandle(groupClientHandle);

	if (!group)
	{
		tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE, E_INVALIDARG);
		return;
	}

	SOCmnPointer<SODaCItem> item;
	size_t* pItems;
	HRESULT* pErrors;
	DWORD i;
	DWORD transactionId;
	HRESULT masterError;
	DWORD itemCnt;
	DWORD rellevantItemCount = 0;
	DWORD itemClientHandle;
	notification->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = notification->UnMarshalLong(&transactionId);
	ok &= notification->UnMarshalLong(&masterError);
	ok &= notification->UnMarshalArrayLen(&itemCnt);
	pItems = new size_t[itemCnt];
	pErrors = new HRESULT[itemCnt];

	if (!pItems || !pErrors)
	{
		delete[] pItems;
		delete[] pErrors;
		tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE, E_OUTOFMEMORY);
		return;
	}

	for (i = 0; i < itemCnt; i++)
	{
		pItems[i] = 0;
	}

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("TPDA::Group::OnWriteComplete %lu ; transaction id = 0x%X"), itemCnt, (unsigned int)transactionId));

	for (i = 0; ((ok) && (i < itemCnt)); i++)
	{
		ok &= notification->UnMarshalLong(&itemClientHandle);
		ok &= notification->UnMarshalLong(&pErrors[rellevantItemCount]);

		if (!ok)
		{
			break;
		}

		item = group->getItemByClientHandle(itemClientHandle);

		if (item.isNotNull())
		{
			item.addRef();
			pItems[rellevantItemCount] = (size_t)(void*)(SODaCItem*)item;
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; 0x%8.8X"), i, (LPCTSTR)item->getItemID(), (unsigned int)pErrors[i]));
			item->setWrite(pErrors[rellevantItemCount], transactionId);
			++rellevantItemCount;
		}
		else
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, itemClientHandle));
		}
	}

	if (ok)
	{
		group->onWriteAdvise(transactionId, masterError, rellevantItemCount, (SODaCItem**)pItems, pErrors);
	}

	for (i = 0; i < rellevantItemCount; i++)
	{
		((SODaCItem*)pItems[i])->release();
	}

	delete[] pItems;
	delete[] pErrors;
	tpCon->sendReply(pCall, TP_INTERFACE_DA_CALLBACK, TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE, ok ? S_OK : RPC_E_INVALID_DATA);
	return;
}

#endif // SOFEATURE_TUNNEL

#endif // SODAC
