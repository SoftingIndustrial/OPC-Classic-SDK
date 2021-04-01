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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCXmlServer.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : SOAP Server object related classes                          |
//                - SODaCXmlServerImpl: proxy for OPC server object           |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_SOAP

#include <opcda.h>
#include "SODaCXmlServer.h"
#include "SODaCServer.h"
#include "SODaCXmlCall.h"


//-----------------------------------------------------------------------------
// SODaCXmlServerImpl                                                         |
//-----------------------------------------------------------------------------

SODaCXmlServerImpl::SODaCXmlServerImpl()
{
	SOCmnPointer<SODaCEntry> entryDaC = getSODaCEntry();
	SOCmnPointer<SOCltHTTP> httpClient = entryDaC->getHTTPClient();
	SOCMN_ALLOCATE_OBJECT_C(m_httpCon, SOCltHTTPConnection(), (SOCltHTTPConnection*));
	httpClient->addConnection(m_httpCon);
	m_timeDifference = 0;
	m_timeoutMin = 1000; // default value: 1 second
	m_timeoutMax = 10000; // default value: 10 seconds
	m_timeoutPercent = 50.0f; // default value: 50% of request timeout
}

SODaCXmlServerImpl::~SODaCXmlServerImpl()
{
	m_httpCon->disconnect();
	m_httpCon.release();
}


//-----------------------------------------------------------------------------
// doStateTransitionImpl
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//      S_OK  - transit to new state
//      error code of GetStatus - error during state change
//
HRESULT SODaCXmlServerImpl::doStateTransitionImpl(void)
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	BYTE targetState = pDaCServer->getTargetObjectState();
	BYTE currentState = pDaCServer->getObjectState();
	HRESULT res = S_OK;

	if (currentState != targetState)
	{
		switch (targetState)
		{
		case SOCltElement::connected:
		case SOCltElement::started:
		{
			if (currentState == SOCltElement::disconnected)
			{
				SOCmnString hostName = pDaCServer->getNodeName();

				if (hostName.isEmpty())
				{
					hostName = _T("localhost");
				}

				m_httpCon->setHostName(hostName);
				m_httpCon->setServer(pDaCServer);
				SODaCServerStatus status;
				res = getStatus(status);

				if (SUCCEEDED(res))
				{
					pDaCServer->setObjectState(targetState);
				}
				else
				{
					_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, m_httpCon->getHTTPClient(), (_T("connect failed")));
					m_httpCon->setServer(NULL);
					m_httpCon->disconnect();
				}
			}
			else
			{
				pDaCServer->setObjectState(targetState);
			}
		}
		break;

		case SOCltElement::disconnected:
			m_httpCon->setServer(NULL);
			m_httpCon->disconnect();
			pDaCServer->setObjectState(targetState);
			break;
		}
	}

	return res;
} // doStateTransition


//-----------------------------------------------------------------------------
// update
//
// - update the server parameter
//
// returns:
//      S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCXmlServerImpl::doUpdateImpl(
	IN BOOL /* fromServer */,                 // update direction: write to server or read from
	IN OPTIONAL DWORD /* whatParameters */)   // update what parameters
{
	// noting the get or set on server level.
	// the locale id is provided with every call
	return S_OK;
} // update



SODaCXmlRequestOptions* SODaCXmlServerImpl::fillRequestOptions(SODaCXmlRequestOptions* pReqOptions)
{
	if (!pReqOptions)
	{
		return NULL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();

	if (pDaCServer->getValidParameters() & SOCLT_SRVPAR_LCID)
	{
		pDaCServer->convertLCIDtoLocaleID(pDaCServer->getLCID(), pReqOptions->m_localeID);
	}

	return pReqOptions;
}


HRESULT SODaCXmlServerImpl::getStatus(OUT SODaCServerStatus& status)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res;
	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(m_httpCon), (SODaCXmlCall*));
	SODaCXmlRequestOptions reqOptions;
	httpCall->getStatus(fillRequestOptions(&reqOptions));
	pDaCServer->beforeInterfaceCall();
	res = m_httpCon->invoke(httpCall, TRUE);
	pDaCServer->afterInterfaceCall();

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getStatusResponse(&response);

		if (SUCCEEDED(res))
		{
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			SOCmnListPosition posD, posO;
			SOCmnString key, val;
			LCID lcid;
			status.clear();
			posO = xmlOptions.getStartPosition();

			while (posO)
			{
				val = xmlOptions.getNext(posO, &key);

				if (_tcsicmp(key, _T("ServerState")) == 0)
				{
					status.m_state = getServerStateFromString(val);
				}
				else if (_tcsicmp(key, _T("StatusInfo")) == 0)
				{
					status.m_statusInfo = val;
				}
				else if (_tcsicmp(key, _T("ProductVersion")) == 0)
				{
					status.m_productVersion = val;
				}
				else if (_tcsicmp(key, _T("StartTime")) == 0)
				{
					status.m_startTime.setXMLDateTime(val);
				}
				else if (_tcsicmp(key, _T("ReplyTime")) == 0)
				{
					status.m_currentTime.setXMLDateTime(val);
				}
			}

			posD = xmlData.getStartPosition();

			while (posD)
			{
				pResData = xmlData.getNext(posD);

				switch (pResData->m_type)
				{
				case SODACXMLRESPONSEDATA_TYPE_VENDORINFO:
					status.m_vendorInfo = pResData->m_dataSZ;
					break;

				case SODACXMLRESPONSEDATA_TYPE_LOCALEID:
					if (pDaCServer->convertLocaleIDtoLCID(pResData->m_dataSZ, lcid))
					{
						status.m_supportedLCIDs.add(lcid);
					}

					break;
				}
			}

			status.m_supportedInterfaces.add(SODAC_VERSION_OPC_XMLDAV10);
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("GetStatus [0x%lX]"), res));
	return res;
}


HRESULT SODaCXmlServerImpl::queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propertyList)
{
	return getPropertiesData(itemID, itemPath, whatData, propertyList);
}

HRESULT SODaCXmlServerImpl::getPropertiesData(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData, OUT SOCmnList<SODaCPropertyData> &propList)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SOCltHTTPConnection> httpCon = getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res;
	BOOL getPropertyValues = ((whatData & SODAC_PROPDATA_VALUE) == SODAC_PROPDATA_VALUE);
	BOOL getAllProperties;
	SOCmnStringList propNameList;
	SOCmnListPosition posP;
	SODaCPropertyData* prop;
	SODaCXmlRequestOptions reqOptions;
	USES_CONVERSION;

	if (!propList.isInit())
	{
		propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	}

	getAllProperties = propList.isEmpty();

	if (!getAllProperties)
	{
		propNameList.create();
		posP = propList.getStartPosition();
		SOCmnString propertyName;

		while (posP)
		{
			prop = propList.getNext(posP);
			if (prop->m_name.getLength() > 0)
			{
				propNameList.add(prop->m_name);
			}
			else
			{
				SODaCProperty::getStandardPropertyNameByID(prop->m_propertyID, propertyName);
				propNameList.add(propertyName);
			}
		}
	}

	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	httpCall->getProperties(itemID, itemPath, NULL, NULL, getPropertyValues, getAllProperties, &propNameList, fillRequestOptions(&reqOptions));
	pDaCServer->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCServer->afterInterfaceCall();

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getGetPropertiesResponse(&response);

		if (SUCCEEDED(res))
		{
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			SOCmnListPosition pos;
			pos = xmlData.getStartPosition();
			posP = propList.getStartPosition();

			while (pos)
			{
				pResData = xmlData.getNext(pos);

				if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_PROPERTY)
				{
					SOCmnVariant propValue;

					if (getAllProperties)
					{
						SOCMN_ALLOCATE_OBJECT(prop, SODaCPropertyData);
					}
					else
					{
						prop = propList.getNext(posP);

						if (!prop)
						{
							return E_FAIL;
						}
					}

					prop->m_name = pResData->m_name;
					prop->m_propertyID = pDaCServer->getPropertyIDByName(prop->m_name);
					prop->m_itemID = pResData->m_itemName;
					prop->m_itemPath = pResData->m_itemPath;
					prop->m_description = pResData->m_description;
					SOCmnVariant::getVARTYPE(pResData->m_value.m_datatype, prop->m_datatype);

					if (propValue.initFromXMLValue(&pResData->m_value))
					{
						if ((prop->m_propertyID == 1) && (propValue.vt == VT_BSTR)) // datatype
						{
							SOCmnString propDT;
							propValue.getBSTR(propDT);

							if (response->includesNamespacePrefix(propDT))
							{
								// property name with namespace
								response->expandNamespace(propDT);
								response->removeOPCXMLDANamespace(propDT); // remove default OPC XML DA NS
								propValue.setBSTR(propDT);
							}
						}

						prop->m_value = propValue;
					}

					if (getAllProperties)
					{
						propList.add(prop);
					}
				}
				else if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ELEMENT)
				{
					// only get error code
					res = httpCall->getErrorCodeFromXML(pResData->m_resultID);
				}
			}
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("GetProperties [0x%lX]"), res));
	return res;
}

HRESULT SODaCXmlServerImpl::read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, OUT SOCmnVariant* pValues, OUT WORD* pQualities, OUT SOCmnDateTime* pTimestamps, OUT HRESULT* pErrors)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SOCltHTTPConnection> httpCon = getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res;
	SODaCXmlRequestOptions reqOptions;
	reqOptions.m_returnItemTime = TRUE;
	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	httpCall->read(count, pItemIDs, pItemPaths, maxAge, fillRequestOptions(&reqOptions));
	pDaCServer->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCServer->afterInterfaceCall();

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getReadResponse(&response);

		if (SUCCEEDED(res))
		{
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			SOCmnListPosition pos;
			SOCmnVariant itemVal;
			DWORD i = 0;
			pos = xmlData.getStartPosition();

			while (pos)
			{
				pResData = xmlData.getNext(pos);

				if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ITEM)
				{
					if (pResData->m_resultID.isEmpty())
					{
						if (pValues)
						{
							itemVal.clear();
							itemVal.initFromXMLValue(&pResData->m_value);
							itemVal.copyTo(pValues[i]);
						}

						if (pQualities)
						{
							pQualities[i] = httpCall->getDCOMQuality((LPCTSTR)pResData->m_quality, (LPCTSTR)pResData->m_qualityLimit, (LPCTSTR)pResData->m_qualityVendor);
						}

						if (pTimestamps)
						{
							pTimestamps[i].setXMLDateTime((LPCTSTR)pResData->m_timestamp);
						}

						if (pErrors)
						{
							pErrors[i] = S_OK;
						}
					}
					else
					{
						if (pErrors)
						{
							pErrors[i] = httpCall->getErrorCodeFromXML(pResData->m_resultID);
						}

						res = S_FALSE;
					}

					i++;
				}
			}
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("Read [0x%lX]"), res));
	return res;
}


HRESULT SODaCXmlServerImpl::write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues, IN WORD* pQualities, IN SOCmnDateTime* pTimestamps, OUT HRESULT* pErrors)
{
	if (SOCLT_XML_CHECK_DEMO)
	{
		return E_FAIL;
	}

	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SOCltHTTPConnection> httpCon = getHTTPConnection();
	SOCmnPointer<SODaCXmlCall> httpCall;
	HRESULT res;
	SODaCXmlRequestOptions reqOptions;
	SOCMN_ALLOCATE_OBJECT_C(httpCall, SODaCXmlCall(httpCon), (SODaCXmlCall*));
	httpCall->write(count, pItemIDs, pItemPaths, pValues, pQualities, pTimestamps, fillRequestOptions(&reqOptions));
	pDaCServer->beforeInterfaceCall();
	res = httpCon->invoke(httpCall, TRUE);
	pDaCServer->afterInterfaceCall();

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCXmlResponse> response;
		res = httpCall->getWriteResponse(&response);

		if (SUCCEEDED(res))
		{
			SOCmnStringList xmlOptions(response->getResponseOptions());
			SOCmnList<SODaCXmlResponseData> xmlData(response->getResponseData());
			SODaCXmlResponseData* pResData;
			SOCmnListPosition pos;
			DWORD i = 0;
			pos = xmlData.getStartPosition();

			while (pos)
			{
				pResData = xmlData.getNext(pos);

				if (pResData->m_type == SODACXMLRESPONSEDATA_TYPE_ITEM)
				{
					if (pResData->m_resultID.isEmpty())
					{
						if (pErrors)
						{
							pErrors[i] = S_OK;
						}
					}
					else
					{
						if (pErrors)
						{
							pErrors[i] = httpCall->getErrorCodeFromXML(pResData->m_resultID);
						}

						res = S_FALSE;
					}

					i++;
				}
			}
		}
	}

	_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("Write [0x%lX]"), res));
	return res;
}

static TCHAR s_serverStateString[6][10] =
{ _T("running"), _T("failed"), _T("noConfig"), _T("suspended"), _T("test"), _T("commFault") };

DWORD SODaCXmlServerImpl::getServerStateFromString(IN LPCTSTR stateSZ)
{
	DWORD i;

	for (i = 0; i < 6; i++)
	{
		if (_tcsicmp(s_serverStateString[i], stateSZ) == 0)
		{
			return i + 1;
		}
	}

	return 0xFFFFFFFF;
}

WORD SODaCXmlServerImpl::getSupportedOPCVersion(void)
{
	return SODAC_VERSION_OPC_XMLDAV10;
}

BOOL SODaCXmlServerImpl::forceOPCVersion(WORD opcVersion)
{
	if ((opcVersion == SODAC_VERSION_OPC_XMLDAV10) || (opcVersion == SODAC_VERSION_OPC_USEDEFAULT))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

float SODaCXmlServerImpl::getTimeoutPercent(void)
{
	return m_timeoutPercent;
}

DWORD SODaCXmlServerImpl::getTimeoutMin(void)
{
	return m_timeoutMin;
}

DWORD SODaCXmlServerImpl::getTimeoutMax(void)
{
	return m_timeoutMax;
}

BOOL SODaCXmlServerImpl::setTimeout(float timeoutPercent, DWORD timeoutMin, DWORD timeoutMax)
{
	if ((m_timeoutPercent < 0) || (m_timeoutPercent > 100))
	{
		return FALSE;
	}
	if (timeoutMin != MAXDWORD)
	{
		if (timeoutMax != MAXDWORD)
		{
			if (timeoutMax < timeoutMin)
			{
				return FALSE;
			}
			else
			{
				m_timeoutMin = timeoutMin;
				m_timeoutMax = timeoutMax;
			}
		}
		else
		{
			if (timeoutMin > m_timeoutMax)
			{
				return FALSE;
			}
			else
			{
				m_timeoutMin = timeoutMin;
			}
		}
	}
	else
	{
		if (timeoutMax != MAXDWORD)
		{
			if (timeoutMax < m_timeoutMin)
			{
				return FALSE;
			}
			else
			{
				m_timeoutMax = timeoutMax;
			}
		}
	}
	if (m_timeoutPercent != 0)
	{
		m_timeoutPercent = timeoutPercent;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//      returns of unadvise
//
HRESULT SODaCXmlServerImpl::doDisconnectImpl(BOOL /* connectionMonitorStatus */)
{
	SOCmnPointer<SODaCEntry> pEntryDaC = getSODaCEntry();
	SOCmnPointer<SOCltHTTP> pHttpClient = pEntryDaC->getHTTPClient();
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SOCmnObject> pSocket = m_httpCon->getSocket();
	m_httpCon->setServer(NULL);
	HRESULT res = S_OK;

	if (pSocket.isNotNull())
	{
		if (pHttpClient->removeClient(pSocket) != TRUE)
		{
			res = E_FAIL;
		} // end if
	}
	else
	{
		res = E_FAIL;
	} // end if

	if (pDaCServer->setObjectState(SOCltElement::disconnected) != TRUE)
	{
		res = E_FAIL;
	} // end if

	return res;
} // doDisconnectImpl


SODaCNameSpaceBrowser* SODaCXmlServerImpl::getNameSpaceBrowserImpl()
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SODaCXmlNameSpaceBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SODaCXmlNameSpaceBrowser(pDaCServer));
	return (SODaCNameSpaceBrowser*)p;
}

void SODaCXmlServerImpl::connectionFailureImpl(void)
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	m_httpCon->setServer(NULL);
	pDaCServer->setGroupsState(SOCltElement::disconnected, TRUE);
	pDaCServer->setObjectState(SOCltElement::disconnected);
	pDaCServer->performStateTransition(TRUE, SOCLT_EXEC_ASYNC);
}

void SODaCXmlServerImpl::setTimeDifference(LONGLONG timeDiffSend, LONGLONG timeDiffReply)
{
	m_timeDifference = timeDiffReply;
	SODaCServer* pDaCServer = getObjectDaCServer();
	_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("Timesync: %ld ms ; request: %ld ms - response: %ld ms"), (LONG)(m_timeDifference / 1000), (LONG)(timeDiffSend / 1000), (LONG)(timeDiffReply / 1000)));
}

LONGLONG SODaCXmlServerImpl::getTimeDifference(void)
{
	return m_timeDifference;
}


//-----------------------------------------------------------------------------
// SODaCXmlServerDefault                                                      |
//-----------------------------------------------------------------------------

SODaCXmlServerDefault::SODaCXmlServerDefault(void)
	: SODaCServer()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaCXmlServerDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


#endif // SOFEATURE_SOAP

#endif // SODAC
