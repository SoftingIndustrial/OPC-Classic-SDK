#ifndef _SOXPSDAOBJECTS_H_
#define _SOXPSDAOBJECTS_H_

#include "SODaSTag.h"
#include "SODaSProperty.h"
#include "SOXpSCallbacks.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


DWORD WINAPI requestThreadMain(IN LPVOID context);
extern HANDLE g_requestThreadEndEvent;
extern SOCmnList<SODaSRequest> g_requestThreadReadQueue;
extern SOCmnList<SODaSRequest> g_requestThreadWriteQueue;

void removeHandledRequests(SOCmnList<SODaSRequest> *requestQueue);
BOOL processRequestData(IN DWORD numRequests, IN SOXpSDARequestData *requestData);



//-----------------------------------------------------------------------------
// SOXpSDAServer                                                              |
//-----------------------------------------------------------------------------

class SOXpSDAServer :
	public SODaSServer
{
public:
	SOXpSDAServer(void);

protected:
	// startup synchronisation
	virtual HRESULT waitForNameSpaceStartup(void);
	virtual HRESULT waitForIOStartup(void);
}; // SOXpSDAServer

//-----------------------------------------------------------------------------
// SOXpSDARequest                                                              |
//-----------------------------------------------------------------------------

class SOXpSDARequest :
	public SODaSRequest
{
public:
	SOXpSDARequest(IN BYTE operation, IN WORD type) : SODaSRequest(operation, type) { };

	virtual BOOL hasChanged(IN DWORD connection, IN SOCmnVariant &value, IN WORD quality);
};

//-----------------------------------------------------------------------------
// SOXpSDARequest                                                              |
//-----------------------------------------------------------------------------

class SOXpSItem :
	public SODaSItemDefault
{
public:
	SOXpSItem(void) : SODaSItemDefault() { };
	virtual ~SOXpSItem();

protected:
	virtual void createCyclicData(void);
	virtual void destroyCyclicData(void);

	// last value send to the OPC client
	virtual BOOL setSendValue(IN DWORD connection, IN SOCmnVariant &value, IN WORD quality);

public:
//	virtual BOOL getSendValue(IN DWORD connection, OUT SOCmnVariant &value, OUT WORD *quality);
	virtual BOOL getSendTimeStamp(IN DWORD connection, IN SOCmnString* pTS);
	virtual void clearAllSendTimeStamps() { m_sendTimeStamps.removeAll(); };

protected:
	SOCmnStringList m_sendTimeStamps;			// previous qualities send to the OPC client
};


//-----------------------------------------------------------------------------
// SODaSItemTagConnectQueueOneItemsStore                                      |
//-----------------------------------------------------------------------------

class SODaSItemTagConnectQueueOneItemsStore : virtual public SODaSItemTagIConnect
{
friend SODaSRequest;
public:
	void createItemList(void);

protected:
	SODaSItemTagConnectQueueOneItemsStore();
	virtual ~SODaSItemTagConnectQueueOneItemsStore();

	// read queue
	virtual SOCmnList<SODaSRequest> *getReadQueue(void);
	virtual SOCmnEventPointer getReadEvent(void);

	// write queue
	virtual SOCmnList<SODaSRequest> *getWriteQueue(void);
	virtual SOCmnEventPointer getWriteEvent(void);

	// connected objects (e.g items connected to the item tag)
public:
	virtual void getAllConnectedObjects(IN DWORD objType,
					OUT SOCmnList<SOCmnObject> &objList);
protected:
	virtual void addConnectedObject(IN SOCmnObject *obj);
	virtual void removeConnectedObject(IN SOCmnObject *obj);
	virtual void activateConnectedObject(IN SOCmnObject *obj);
	virtual void deactivateConnectedObject(IN SOCmnObject *obj);

protected:
	SOCmnList<SODaSItem> m_itemList;
	DWORD m_activeItems;
}; // SODaSItemTagConnectQueueOneItemsStore




//-----------------------------------------------------------------------------
// SOXpSTag                                                                   |
//-----------------------------------------------------------------------------

class SOXpSTag :
	public SODaSTag,
	virtual public SOCmnElementListAdditional,
	virtual public SODaSItemTagConnectQueueOneItemsStore
{
public:
	SOXpSTag();

	// application specific data
	inline DWORD getXpSUserData(void) const { return m_xpsUserData; }
	inline void setXpSUserData(IN DWORD userData) { m_xpsUserData = userData; }

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

	virtual void setUpdateMode(BYTE updateMode);

protected:
	// request handling
	virtual void handleRequest(IN SODaSRequest *request);

	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
	DWORD m_xpsUserData;
}; // SOXpSTag




//-----------------------------------------------------------------------------
// SOXpSProperty                                                              |
//-----------------------------------------------------------------------------

class SOXpSProperty :
	public SODaSProperty,
	virtual public SODaSItemTagConnectQueueOneItemsStore
{
public:
	SOXpSProperty();

	// application specific data
	inline DWORD getXpSUserData(void) const { return m_xpsUserData; }
	inline void setXpSUserData(IN DWORD userData) { m_xpsUserData = userData; }

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

	virtual void setUpdateMode(BYTE updateMode);

protected:
	// request handling
	virtual void handleRequest(IN SODaSRequest *request);

	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
	DWORD m_xpsUserData;
}; // SOXpSProperty




//-----------------------------------------------------------------------------
// SOXpSNode                                                                  |
//-----------------------------------------------------------------------------

class SOXpSNode :
	public SODaSNode,
	virtual public SOCmnElementListLeafBranchAdditional,
	virtual public SODaSItemTagConnectQueueOneItemsStore
{
public:
	SOXpSNode(void);

	// application specific data
	inline DWORD getXpSUserData(void) const { return m_xpsUserData; }
	inline void setXpSUserData(IN DWORD userData) { m_xpsUserData = userData; }

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

	virtual void setUpdateMode(BYTE updateMode);

protected:
	// request handling
	virtual void handleRequest(IN SODaSRequest *request);

	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
	DWORD m_xpsUserData;
}; // SOXpSNode



//-----------------------------------------------------------------------------
// SOXpSTransaction                                                           |
//-----------------------------------------------------------------------------

class SOXpSTransaction : public SODaSTransaction
{
public:
	SOXpSTransaction(IN SODaSGroup *group, IN BYTE type,
		IN BYTE connectionType, IN OPTIONAL DWORD connection = 0);

	// actions on request list
	virtual void handleRequests(IN SOCmnList<SODaSRequest> *reqList);

protected:
}; // SOXpSTransaction

#pragma pack(pop)
#endif
