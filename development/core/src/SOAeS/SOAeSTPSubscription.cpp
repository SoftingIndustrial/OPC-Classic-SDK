#include "stdafx.h"

#ifdef SOAES

#ifdef SOFEATURE_TUNNEL

#include "SOAeSTPServer.h"
#include "SOAeSTPSubscription.h"
#include "opcerror.h"
#include <opcae_er.h>
//-----------------------------------------------------------------------------
// SOAeSTPSubscription                                                               |
//-----------------------------------------------------------------------------

SOAeSTPSubscription::SOAeSTPSubscription(void)
{
}

SOAeSTPSubscription::~SOAeSTPSubscription(void)
{
}

//-----------------------------------------------------------------------------
//	validConnection
//
BOOL SOAeSTPSubscription::validConnection(DWORD aConnection)
{
	return aConnection != NULL;
}   //  end validConnection


//-----------------------------------------------------------------------------
// handleTPRequest
//
// - processing of TP request
//
// returns:
//		TRUE  - receiver has processed or is processing the request
//		FALSE - unknown request for this receiver
//
LONG SOAeSTPSubscription::handleRequest(
	IN SOCmnTPCall* pCall,
	WORD interfaceId,
	BYTE functionId)
{
	if (interfaceId != TP_INTERFACE_AE_SUBSCRIPTION)
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

	case TP_FUNCTION_AE_SUBSCRIPTION_SELECTRETURNEDATTRIBUTES:
		selectReturnedAttributes(server, pCall);
		break;

	case TP_FUNCTION_AE_SUBSCRIPTION_GETRETURNEDATTRIBUTES:
		getReturnedAttributes(server, pCall);
		break;

	case TP_FUNCTION_AE_SUBSCRIPTION_REFRESH:
		refresh(server, pCall);
		break;

	case TP_FUNCTION_AE_SUBSCRIPTION_CANCELREFRESH:
		cancelRefresh(server, pCall);
		break;

		/* not used until now
		        case TP_FUNCTION_AE_SUBSCRIPTION_SETFILTER:
		            setFilter(server, pCall);
		        break;
		        case TP_FUNCTION_AE_SUBSCRIPTION_GETFILTER:
		            getFilter(server, pCall);
		        break;
		        case TP_FUNCTION_AE_SUBSCRIPTION_GETSTATE:
		            getState(server, pCall);
		        break;
		        case TP_FUNCTION_AE_SUBSCRIPTION_SETSTATE:
		            setState(server, pCall);
		        break;
		not used until now */
	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}   //  end handleRequest


//--------------------------------------------------------------------------
//	selectReturnedAttributes
//
void SOAeSTPSubscription::selectReturnedAttributes(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_SELECTRETURNEDATTRIBUTES,
							TPAE::Subscription::selectReturnedAttributes)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD subscriptionServerHandle;
	SOCmnPointer<SOAeSSubscription> subscription;
	BOOL ok = TRUE;
	// unmarshal input
	subscriptionServerHandle = pCall->getObjectId();
	subscription = server->getSubscriptionByHandle(subscriptionServerHandle);

	if (!subscription)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	//  Input
	DWORD eventCategory = 0;
	DWORD count = 0;
	DWORD* attributeIDs = NULL;
	DWORD index = 0;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&eventCategory);
	ok &= request->UnMarshalLong(&count);

	if (!ok || count == 0)
	{
		// check marshaling
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	attributeIDs = (DWORD*) CoTaskMemAlloc(count * sizeof(DWORD));

	if (attributeIDs == NULL)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_OUTOFMEMORY);
	}   //  end if

	memset(attributeIDs, 0, count * sizeof(DWORD));

	for (index = 0; index < count && ok; index++)
	{
		ok &= request->UnMarshalLong(&attributeIDs[index]);
	}   //  end for

	res = subscription->selectReturnedAttributes(eventCategory, count, attributeIDs);
	CoTaskMemFree(attributeIDs);
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_SELECTRETURNEDATTRIBUTES);    // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end selectReturnedAttributes


//-----------------------------------------------------------------------------
//	getReturnedAttributes
//
void SOAeSTPSubscription::getReturnedAttributes(
	IN SOAeSServer* pServer,
	IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_GETRETURNEDATTRIBUTES,
							TPAE::Subscription::getReturnedAttributes)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD subscriptionServerHandle;
	SOCmnPointer<SOAeSSubscription> subscription;
	BOOL ok = TRUE;
	// unmarshal input
	subscriptionServerHandle = pCall->getObjectId();
	subscription = server->getSubscriptionByHandle(subscriptionServerHandle);

	if (!subscription)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	//  Input
	DWORD eventCategory = 0;
	// Output
	SOCmnDWORDList attributeIDs;
	// unmarshal input
	request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
	ok = request->UnMarshalLong(&eventCategory);

	if (ok)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
	}   //  end if

	SOCmnList<SOAeSAttribute> retAttrList(subscription->getReturnedAttributeList(eventCategory));

	if (!retAttrList)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
	}   //  end if

	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) +
						   sizeof(DWORD)*retAttrList.getCount());
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_GETRETURNEDATTRIBUTES);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result
	response->MarshalLong(retAttrList.getCount());              // object id
	SOAeSAttribute* attr = NULL;
	SOCmnListPosition pos = retAttrList.GetHeadPosition();

	while (pos)
	{
		attr = (SOAeSAttribute*)retAttrList.getNext(pos);
		response->MarshalLong(attr->getNameInt());              // object id
	}   //  end while

	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end getReturnedAttributes


//-----------------------------------------------------------------------------
//	refresh
//
void SOAeSTPSubscription::refresh(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_REFRESH,
							TPAE::Subscription::refresh)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD subscriptionServerHandle;
	SOCmnPointer<SOAeSSubscription> subscription;
	// unmarshal input
	subscriptionServerHandle = pCall->getObjectId();
	subscription = server->getSubscriptionByHandle(subscriptionServerHandle);

	if (!subscription)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}   //  end if

	//  Input
	SOCmnPointer<SOAeSRefresh> refresh;
	// check if another refresh is in progress
	refresh = subscription->createRefresh(0xFFFFFFFF); // -1

	if (!refresh)
	{
		res = OPC_E_BUSY;
	}   //  end if

	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_REFRESH); // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end refresh


//-----------------------------------------------------------------------------
//	cancelRefresh
//
void SOAeSTPSubscription::cancelRefresh(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall)
{
	SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_CANCELREFRESH,
							TPAE::Subscription::CancelRefresh)
	SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
	SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
	SOAeSServer* server = pServer;
	DWORD subscriptionServerHandle;
	SOCmnPointer<SOAeSSubscription> subscription;
	// unmarshal input
	subscriptionServerHandle = pCall->getObjectId();
	subscription = server->getSubscriptionByHandle(subscriptionServerHandle);

	if (!subscription)
	{
		SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
	}

	res = subscription->cancelRefresh(0xFFFFFFFF); // -1
	// build response
	response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 4);
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
	response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
	response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
	response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_CANCELREFRESH);   // function id
	response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
	response->MarshalLong(res);                                 // result
	tpCon->reply(pCall);
	SOSRV_END_TP_FUNCTION
}   //  end cancelRefresh


/* not used until now

//-----------------------------------------------------------------------------
//  setFilter
//
void SOAeSTPSubscription::setFilter(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall)
{

    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_SETFILTER,
        TPAE::Subscription::setFilter)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
    SOCmnPointer<SOAeSEventSpaceRoot> eventSpace = entry->getEventSpaceRoot();
    SOCmnPointer<SOAeSAreaSpaceRoot> areaSpace = entry->getAreaSpaceRoot();

    SOAeSServer *server = pServer;
    DWORD subscriptionServerHandle;
    SOCmnPointer<SOAeSSubscription> subscription;
    BOOL ok = TRUE;

    // unmarshal input
    subscriptionServerHandle = pCall->getObjectId();
    subscription = server->getSubscriptionByHandle(subscriptionServerHandle);
    if (!subscription)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
    }   //  end if

    //  Input
    DWORD eventType;                // event type
    DWORD numCategories;            // number of categories
    SOCmnDWORDList eventCategories; // categories List
    DWORD lowSeverity;              // low severity
    DWORD highSeverity;             // high severity
    DWORD numAreas;                 // number of areas
    SOCmnStringList areas;          // area array
    DWORD numSources;               // number of sources
    SOCmnStringList sources;        // sources list

    DWORD index = 0;
    SOCmnString inputString;
    DWORD inputDWORD;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalLong(&eventType);
    ok &= request->UnMarshalLong(&numCategories);
    index = 0;
    SOCmnPointer<SOAeSCategory> cat;
    while (ok && index < numCategories)
    {
        ok &= request->UnMarshalLong(&inputDWORD);
        // check if valid category
        cat = eventSpace->findCategory(inputDWORD);
        if (!cat)
        {
            SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
        }
        eventCategories.add(inputDWORD);
    }   //  end while

    ok &= request->UnMarshalLong(&lowSeverity);
    ok &= request->UnMarshalLong(&highSeverity);

    ok &= request->UnMarshalLong(&numAreas);
    index = 0;
    SOCmnPointer<SOAeSArea> area;

    while (ok && index < numAreas)
    {
        ok &= request->UnMarshalString(&inputString);

        if (!server->includesFilterChars(inputString))
        {
            // check if valid area
            area = (SOAeSArea*)areaSpace->findBranch(inputString, TRUE);
            if (!area)
            {
                if (!server->isAreaName(inputString))
                {
                    SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
                }   //  end if
            }   //  end if
        }   //  end if

        areas.add(inputString);
    }   //  end while

    ok &= request->UnMarshalLong(&numSources);
    index = 0;
    SOCmnPointer<SOAeSSource> source;
    while (ok && index < numSources)
    {
        ok &= request->UnMarshalString(&inputString);
        if (!server->includesFilterChars(inputString))
        {
            source = (SOAeSSource *)areaSpace->findLeaf(inputString, TRUE);
            if (!source)
            {
                if (!server->isSourceName(inputString))
                {
                    SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG)
                }   //  end if
            }   //  end if
        }   //  end if
        sources.add(inputString);
    }   //  end while

    if (!ok)
    {
        // check marshaling
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    if (subscription->isRefreshInProgress())
    {
        // a refresh is currently in progress
        SOSRV_RETURN_TP_FUNCTION_RES(OPC_E_BUSY)
    }   //  end if

    // set filter
    res = subscription->setFilter(eventType, &eventCategories, lowSeverity, highSeverity, &areas, &sources);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 16);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
    response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_SETFILTER);   // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);                                 // result

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end setFilter


//-----------------------------------------------------------------------------
//  getFilter
//
void SOAeSTPSubscription::getFilter(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall)
{
    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_GETFILTER,
        TPAE::Subscription::getFilter)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();

    SOAeSServer *server = pServer;
    DWORD subscriptionServerHandle;
    SOCmnPointer<SOAeSSubscription> subscription;
    BOOL ok = TRUE;

    // unmarshal input
    subscriptionServerHandle = pCall->getObjectId();
    subscription = server->getSubscriptionByHandle(subscriptionServerHandle);
    if (!subscription)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
    }   //  end if

    //  Output
    DWORD eventType = OPC_ALL_EVENTS;   // event type
    SOCmnDWORDList eventCategories;     // categories List
    DWORD lowSeverity = 1;              // low severity
    DWORD highSeverity = 1000;          // high severity
    SOCmnStringList areas;              // area array
    SOCmnStringList sources;            // sources list

    // get filter
    res = subscription->getFilter(&eventType, &eventCategories, &lowSeverity, &highSeverity, &areas, &sources);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64 +
        eventCategories.getCount()*sizeof(DWORD) +
        64 * areas.getCount() +
        64 * sources.getCount());
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
    response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_GETFILTER);   // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);                                 // result

    if (SUCCEEDED(res))
    {
        DWORD outputDWORD = 0;
        SOCmnString outputString;

        response->MarshalLong(eventType);
        response->MarshalLong(eventCategories.getCount());
        SOCmnListPosition pos = eventCategories.GetHeadPosition();
        while (pos)
        {
            outputDWORD = eventCategories.getNext(pos);
            response->MarshalLong(outputDWORD);
        }   //  end while

        response->MarshalLong(lowSeverity);
        response->MarshalLong(highSeverity);

        response->MarshalLong(areas.getCount());
        pos = areas.GetHeadPosition();
        while (pos)
        {
            outputString = areas.getNext(pos);
            response->MarshalString(outputString);
        }   //  end while

        response->MarshalLong(sources.getCount());
        pos = sources.GetHeadPosition();
        while (pos)
        {
            outputString = sources.getNext(pos);
            response->MarshalString(outputString);
        }   //  end while
    }
    else
    {
        SOSRV_RETURN_TP_FUNCTION_RES(res);
    }   //  end if ... else

    tpCon->reply(pCall);
    SOSRV_END_TP_FUNCTION

}   //  end getFilter


//-----------------------------------------------------------------------------
//  getState
//
void SOAeSTPSubscription::getState(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{

    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_GETSTATE,
        TPAE::Subscription::getState)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;
    DWORD subscriptionServerHandle;
    SOCmnPointer<SOAeSSubscription> subscription;
    BOOL ok = TRUE;

    // unmarshal input
    subscriptionServerHandle = pCall->getObjectId();
    subscription = server->getSubscriptionByHandle(subscriptionServerHandle);
    if (!subscription)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
    }   //  end if

    res = S_OK;

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
    response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_GETSTATE);    // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags
    response->MarshalLong(res);                                 // result

    response->MarshalChar((BYTE)subscription->getActive());
    response->MarshalLong(subscription->getBufferTime());
    response->MarshalLong(subscription->getMaxSize());
    response->MarshalLong(subscription->getClientHandle());

    tpCon->reply(pCall);

    SOSRV_END_TP_FUNCTION

}   //  end getState


//-----------------------------------------------------------------------------
//  getState
//
void SOAeSTPSubscription::setState(
    IN SOAeSServer *pServer,
    IN SOCmnTPCall *pCall)
{

    SOSRV_BEGIN_TP_FUNCTION(TP_INTERFACE_AE_SUBSCRIPTION, TP_FUNCTION_AE_SUBSCRIPTION_SETSTATE,
        TPAE::Subscription::setState)
    SOCmnPointer<SOCmnTP_Marshaler> response = pCall->getSndMsg();
    SOCmnPointer<SOCmnTP_UnMarshaler> request = pCall->getRcvMsg();
    SOAeSServer *server = pServer;
    DWORD subscriptionServerHandle;
    SOCmnPointer<SOAeSSubscription> subscription;
    BOOL ok = TRUE;

    // unmarshal input
    subscriptionServerHandle = pCall->getObjectId();
    subscription = server->getSubscriptionByHandle(subscriptionServerHandle);
    if (!subscription)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(E_INVALIDARG);
    }   //  end if

    BYTE active;
    DWORD bufferTime;
    DWORD maxSize;
    DWORD clientSubscription;
    DWORD revisedBufferTime;
    DWORD revisedMaxSize;

    // unmarshal input
    request->SetReadPos(sizeof(TP_BASIC_MESSAGE_HEADER));
    ok = request->UnMarshalChar(&active);
    ok &= request->UnMarshalLong(&bufferTime);
    ok &= request->UnMarshalLong(&maxSize);
    ok &= request->UnMarshalLong(&clientSubscription);

    if (!ok)
    {
        SOSRV_RETURN_TP_FUNCTION_RES(RPC_E_INVALID_DATA);
    }   //  end if

    // set the parameter
    subscription->setActive((active != 0));
    subscription->setBufferTime(bufferTime);
    subscription->setMaxSize(maxSize);
    subscription->setClientHandle(clientSubscription);

    // build response
    response->BeginMarshal(sizeof(TP_BASIC_MESSAGE_HEADER) + 64);
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // message length
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // invoke id
    response->MarshalLong(TP_DUMMY_VALUE_FILLED_IN_LATER);      // object id
    response->MarshalShort(TP_INTERFACE_AE_SUBSCRIPTION);       // interface id
    response->MarshalChar(TP_FUNCTION_AE_SUBSCRIPTION_SETSTATE);    // function id
    response->MarshalChar(TP_MSG_FLAG_FUNCTION_RETURN);         // flags

    res = S_OK;

    // fill the output parameter
    revisedBufferTime = subscription->getBufferTime();
    if (revisedBufferTime != bufferTime)
    {
        res = OPC_S_INVALIDBUFFERTIME;
    }   //  end if

    revisedMaxSize = subscription->getMaxSize();
    if (revisedMaxSize != maxSize)
    {
        res = OPC_S_INVALIDMAXSIZE;
    }   //  end if

    response->MarshalLong(res);
    response->MarshalLong(revisedBufferTime);
    response->MarshalLong(revisedMaxSize);

    tpCon->reply(pCall);
    SOSRV_END_TP_FUNCTION

}   //  end setState

 not used until now */



#endif // SOFEATURE_TUNNEL

#endif // SOAES
