#ifndef _SERVERDANODE_H_
#define _SERVERDANODE_H_

#include "OTServer.h"
#include "ServerDAAddressSpaceElement.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDANode
//-----------------------------------------------------------------------------

class ServerDANode :
	public SODaSNode,
	virtual public SODaSItemTagConnectItemsStore,
	virtual public SOCmnElementListLeafBranchAdditional,
	public ServerDAAddressSpaceElement
{
public:
	ServerDANode(void);

	virtual BOOL hasProperty(IN DWORD propId, SODaSPropertyData* pPropData);
	virtual BOOL getProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> *pListPD);
	virtual BOOL initProperty(IN SODaSProperty* property,  SODaSPropertyData* pPropData);
	virtual BOOL handlePropertyRequest(IN SODaSProperty* prop,
									   IN SODaSRequest* request);

	virtual void addConnectedObject(IN SOCmnObject* obj, IN BOOL goingToActivate);
	virtual void removeConnectedObject(IN SOCmnObject* obj);
	virtual void activateConnectedObject(IN SOCmnObject* obj);
	virtual void deactivateConnectedObject(IN SOCmnObject* obj);

	virtual BOOL isBrowsable(IN SOCmnObject* pServer);

	virtual BYTE getDeviceIOMode(IN SODaSRequest* req);

protected:
	~ServerDANode(void);
}; // ServerDANode

#pragma pack(pop)
#endif
