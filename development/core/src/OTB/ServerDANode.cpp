#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDAAddressSpaceElement.h"
#include "ServerDANode.h"

//-----------------------------------------------------------------------------
// ServerDANode
//-----------------------------------------------------------------------------

ServerDANode::ServerDANode(void)
	:SOCmnElementListLeafBranchAdditional(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
										  SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
										  SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING)
{
	m_userDataOTS = 0;
	m_deviceIOAndUpdateMode = SODaSItemTag::noDeviceIO;
}

ServerDANode::~ServerDANode(void)
{
	destroy(this);
}

BOOL ServerDANode::hasProperty(IN DWORD propId, SODaSPropertyData* pPropData)
{
	return hasPropertyI(this, propId, pPropData);
}

BOOL ServerDANode::getProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> *pListPD)
{
	getPropertiesI(this, isSOAP, pListPD);
	return TRUE;
}

BOOL ServerDANode::handlePropertyRequest(IN SODaSProperty* prop, IN SODaSRequest* pRequest)
{
	return handlePropertyRequestI(this, prop, pRequest);
}

BOOL ServerDANode::initProperty(IN SODaSProperty* property, SODaSPropertyData* pPropData)
{
	return initPropertyI(this, property, pPropData);
}

void ServerDANode::addConnectedObject(IN SOCmnObject* obj, IN BOOL goingToActivate)
{
	SODaSItemTagConnectItemsStore::addConnectedObject(obj, goingToActivate);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDANode::removeConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::removeConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDANode::activateConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::activateConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

void ServerDANode::deactivateConnectedObject(IN SOCmnObject* obj)
{
	SODaSItemTagConnectItemsStore::deactivateConnectedObject(obj);
	connectedObjectChanged((SODaSItemTag*)this);
}

BOOL ServerDANode::isBrowsable(IN SOCmnObject* pServer)
{
	return isBrowsableI(pServer);
}

BYTE ServerDANode::getDeviceIOMode(IN SODaSRequest* req)
{
	return getDeviceIOModeI((SODaSItemTag*)this, m_deviceIOAndUpdateMode, req);
}

#endif // OTB_SERVER_DA
