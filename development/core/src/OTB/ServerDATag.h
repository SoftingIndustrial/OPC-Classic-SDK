#ifndef _SERVERDATAG_H_
#define _SERVERDATAG_H_

#include "OTServer.h"
#include "ServerDAAddressSpaceElement.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDATag
//-----------------------------------------------------------------------------

class ServerDATag :
	public SODaSTag,
	virtual public SODaSItemTagConnectItemsStore,
	virtual public SOCmnElementListAdditional,
	public ServerDAAddressSpaceElement
{
public:
	ServerDATag(void);

	virtual BOOL hasProperty(IN DWORD propId, SODaSPropertyData* pPropData);
	virtual BOOL getProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> *pListPD);
	virtual BOOL initProperty(IN SODaSProperty* property, SODaSPropertyData* pPropData);

	virtual BOOL handlePropertyRequest(IN SODaSProperty* prop,
									   IN SODaSRequest* request);

	virtual void addConnectedObject(IN SOCmnObject* obj, IN BOOL goingToActivate);
	virtual void removeConnectedObject(IN SOCmnObject* obj);
	virtual void activateConnectedObject(IN SOCmnObject* obj);
	virtual void deactivateConnectedObject(IN SOCmnObject* obj);

	virtual BOOL isBrowsable(IN SOCmnObject* pServer);

	virtual BYTE getDeviceIOMode(IN SODaSRequest* req);

protected:
	~ServerDATag(void);
}; // ServerDATag



//-----------------------------------------------------------------------------
// ServerDATagOutOfConfig
//-----------------------------------------------------------------------------

class ServerDATagOutOfConfig :
	public SODaSTag
{
public:
	ServerDATagOutOfConfig(SODaSItemTag* pItemTag);

	// item tags item id
	virtual SOCmnString getItemID(void);
	void setItemID(LPCTSTR itemId)
	{
		m_itemID = itemId;
	}

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
	virtual BYTE getDeviceIOMode(IN SODaSRequest* req);

protected:
	SOCmnString m_itemID;
}; // ServerDATagOutOfConfig

#pragma pack(pop)
#endif
