#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDAAddressSpaceElement.h"
#include "ServerDATag.h"


//-----------------------------------------------------------------------------
// ServerDATag
//-----------------------------------------------------------------------------

ServerDATag::ServerDATag(void)
{
	setDeviceIOMode(transaction);
}

ServerDATag::~ServerDATag(void)
{
	destroy(this);
}

BOOL ServerDATag::hasProperty(IN DWORD propId, SODaSPropertyData* pPropData)
{
	return hasPropertyI(this, propId, pPropData);
}

BOOL ServerDATag::getProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> *pListPD)
{
	getPropertiesI(this, isSOAP, pListPD);
	return FALSE;
}

BOOL ServerDATag::handlePropertyRequest(IN SODaSProperty* prop, IN SODaSRequest* pRequest)
{
	return handlePropertyRequestI(this, prop, pRequest);
}

BOOL ServerDATag::initProperty(IN SODaSProperty* property, SODaSPropertyData* pPropData)
{
	return initPropertyI(this, property, pPropData);
}

void ServerDATag::addConnectedObject(IN SOCmnObject* obj, IN BOOL goingToActivate)
{
	SODaSItemTagConnectItemsStore::addConnectedObject(obj, goingToActivate);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDATag::removeConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::removeConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDATag::activateConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::activateConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDATag::deactivateConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::deactivateConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

BOOL ServerDATag::isBrowsable(IN SOCmnObject* pServer)
{
	return isBrowsableI(pServer);
}

BYTE ServerDATag::getDeviceIOMode(IN SODaSRequest* req)
{
	return getDeviceIOModeI((SODaSItemTag*)this, m_deviceIOAndUpdateMode, req);
}

//-----------------------------------------------------------------------------
// ServerDATagOutOfConfig
//-----------------------------------------------------------------------------

ServerDATagOutOfConfig::ServerDATagOutOfConfig(SODaSItemTag* itemTag)
	: SODaSTag()
{
	SOCmnPointer<SODaSCache> cache(getCache());
	SOCmnPointer<SODaSCache> cacheIT(itemTag->getCache());
	SOCmnVariant value;
	cacheIT->getValue(value);
	cache->setValue(value);
	cache->setQuality(OPC_QUALITY_CONFIG_ERROR);
}

SOCmnString ServerDATagOutOfConfig::getItemID(void)
{
	return m_itemID;
}

void ServerDATagOutOfConfig::handleRequest(IN SODaSRequest* request)
{
	request->complete(E_FAIL);
}

BYTE ServerDATagOutOfConfig::getDeviceIOMode(IN SODaSRequest* req)
{
	if (!req)
	{
		return cache;
	}

	if (req->getOperation() == SODaSRequest::read)
	{
		return cache;
	}
	else
	{
		return direct;
	}
}

#endif // OTB_SERVER_DA
