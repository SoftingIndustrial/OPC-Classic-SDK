#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_TUNNEL

#include "SOAeSTPAreaBrowser.h"

//-----------------------------------------------------------------------------
//
//
SOAeSTPAreaBrowser::SOAeSTPAreaBrowser(void)
{
}   //  end constructor


SOAeSTPAreaBrowser::~SOAeSTPAreaBrowser(void)
{
}   //  end destructor


//-----------------------------------------------------------------------------
// handleTPRequest
//
// - processing of TP request
//
// returns:
//		TRUE  - receiver has processed or is processing the request
//		FALSE - unknown request for this receiver
//
LONG SOAeSTPAreaBrowser::handleRequest(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId)
{
	if (interfaceId != TP_INTERFACE_AE_AREA_BROWSER)
	{
		return E_FAIL;
	}

	SOSRV_TP_CHECK_DEMO
	SOCmnPointer<SOSrvTPConnection> tpCon = (SOSrvTPConnection*)pCall->getTPConnection();
	SOCmnPointer<SOAeSServer> server = (SOAeSServer*)tpCon->getServer();

	if (!server)
	{
		return E_FAIL;
	}

	switch (functionId)
	{
	case TP_FUNCTION_UNKNOWN_QUERY_SUPPORT:
		tpCon->sendReply(pCall, interfaceId, functionId, S_OK);
		break;

	case TP_FUNCTION_AE_AREA_BROWSER_CHANGEBROWSEPOSITION:
		changeBrowsePosition(server, pCall);
		break;

	case TP_FUNCTION_AE_AREA_BROWSER_BROWSEOPCAREAS:
		browseOPCAreas(server, pCall);
		break;

	case TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDAREANAME:
		getQualifiedAreaName(server, pCall);
		break;

	case TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDSOURCENAME:
		getQualifiedSourceName(server, pCall);
		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}   //  end handleRequest


//-----------------------------------------------------------------------------
//	changeBrowsePosition
//
void SOAeSTPAreaBrowser::changeBrowsePosition(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_AREA_BROWSER, TP_FUNCTION_AE_AREA_BROWSER_CHANGEBROWSEPOSITION,
							TPAE::AreaBrowser::ChangeBrowsePosition)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD areaBrowserServerHandle;
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	BOOL ok = TRUE;
	// unmarshal input
	areaBrowserServerHandle = pCall->getObjectId();
	areaBrowser = server->getAreaBrowserByHandle(areaBrowserServerHandle);

	if (!areaBrowser)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	//  Input
	DWORD browseDirection = 0xFFFFFFFF; // -1
	SOCmnString browseString;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&browseDirection);
	ok &= request->UnMarshalString(&browseString);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	res = areaBrowser->changeBrowsePosition((SOSrvBrowser::browserDirection)browseDirection, browseString);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_CHANGEBROWSEPOSITION);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end changeBrowsePosition


//-----------------------------------------------------------------------------
//	browseOPCAreas
//
void SOAeSTPAreaBrowser::browseOPCAreas(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_AREA_BROWSER, TP_FUNCTION_AE_AREA_BROWSER_BROWSEOPCAREAS,
							TPAE::AreaBrowser::BrowseOPCAreas)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD areaBrowserServerHandle;
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	BOOL ok = TRUE;
	// unmarshal input
	areaBrowserServerHandle = pCall->getObjectId();
	areaBrowser = server->getAreaBrowserByHandle(areaBrowserServerHandle);

	if (!areaBrowser)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	DWORD browseFilterTypeOpc = 0xFFFFFFFF; //-1 - areas or sources
	SOSrvBrowser::browserFilterType fType = SOSrvBrowser::typeBranch;
	SOCmnString filterCriteria;     // string filter
	SOCmnStringList enumString;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&browseFilterTypeOpc);
	ok &= request->UnMarshalString(&filterCriteria);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	switch (browseFilterTypeOpc)
	{
	case OPC_SOURCE:
		fType = SOSrvBrowser::typeLeaf;
		break;

	case OPC_AREA:
		fType = SOSrvBrowser::typeBranch;
		break;
	}

	enumString.create();
	res = areaBrowser->browseCurrentBranch(fType, filterCriteria, VT_EMPTY, 0, enumString);
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64 * enumString.getCount());
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_BROWSEOPCAREAS);  // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

	if (SUCCEEDED(res))
	{
		// no list entry -> return OK but nothing found
		if (enumString.isEmpty())
		{
			res = S_FALSE;
		}   //  end if
	}   //  end if

	response->MarshalLong(res);

	if (SUCCEEDED(res))
	{
		response->MarshalLong(enumString.getCount());
		SOCmnListPosition pos = enumString.GetHeadPosition();
		SOCmnString currentString;

		while (pos)
		{
			currentString = enumString.getNext(pos);
			response->MarshalString(currentString);
		}   //  end while
	}

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end browseOPCAreas


//-----------------------------------------------------------------------------
//	getQualifiedAreaName
//
void SOAeSTPAreaBrowser::getQualifiedAreaName(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_AREA_BROWSER, TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDAREANAME,
							TPAE::AreaBrowser::getQualifiedAreaName)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD areaBrowserServerHandle;
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	BOOL ok = TRUE;
	// unmarshal input
	areaBrowserServerHandle = pCall->getObjectId();
	areaBrowser = server->getAreaBrowserByHandle(areaBrowserServerHandle);

	if (!areaBrowser)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	SOCmnString areaName;           // area name
	SOCmnString qualifiedAreaName;  // fully qualified area name
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&areaName);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDAREANAME);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

	if (areaBrowser->getQualifiedBranchName(areaName, qualifiedAreaName))
	{
		response->MarshalLong(S_OK);
		response->MarshalString(qualifiedAreaName);
	}
	else
	{
		response->MarshalLong((unsigned long)E_INVALIDARG);
	}   //  end if .. else

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end getQualifiedAreaName


//-----------------------------------------------------------------------------
//	getQualifiedSourceName
//
void SOAeSTPAreaBrowser::getQualifiedSourceName(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_AREA_BROWSER, TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDSOURCENAME,
							TPAE::AreaBrowser::getQualifiedSourceName)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD areaBrowserServerHandle;
	SOCmnPointer<SOAeSAreaBrowser> areaBrowser;
	BOOL ok = TRUE;
	// unmarshal input
	areaBrowserServerHandle = pCall->getObjectId();
	areaBrowser = server->getAreaBrowserByHandle(areaBrowserServerHandle);

	if (!areaBrowser)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	SOCmnString sourceName;             // source name
	SOCmnString qualifiedSourceName;    // fully qualified source name
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalString(&sourceName);

	if (!ok)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_AREA_BROWSER);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDSOURCENAME);  // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

	// get the path and check if valid element
	if (areaBrowser->getQualifiedLeafName(sourceName, qualifiedSourceName))
	{
		response->MarshalLong(S_OK);
		response->MarshalString(qualifiedSourceName);
	}
	else
	{
		response->MarshalLong((unsigned long)E_INVALIDARG);
	}   //  end if ... else

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end getQualifiedSourceName


#endif // SOFEATURE_TUNNEL

#endif // SOAES
