#ifndef _SODACCOMGROUP_H_
#define _SODACCOMGROUP_H_

#ifdef SOFEATURE_DCOM

#include "SODaC.h"


#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCComGroupImpl                                                          |
//-----------------------------------------------------------------------------
class SODaCAdviseSink;
class SODaCDataCallback;

class SODAC_EXPORT SODaCComGroupImpl : virtual public SODaCGroupICommunication
{
	friend class SODaCAdviseSink;
	friend class SODaCDataCallback;
public:
	SODaCComGroupImpl(void);

	virtual void init(SODaCServer* pServer);

	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);

	SODaCItem* insertItem(IN OPCITEMATTRIBUTES* itemAttr);

	// item management
	virtual HRESULT validateItems(IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors = NULL);
	virtual HRESULT updateItemsImpl(IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);

	// synchronous I/O
	virtual HRESULT syncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								 OUT OPTIONAL SOCmnVariant* pValues = NULL, OUT OPTIONAL WORD* pQualities = NULL,
								 OUT OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncWriteImpl(IN DWORD count, IN SODaCItem** itemArray, IN OPTIONAL SOCmnVariant* pValues = NULL,
								  IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);

	// asynchronous I/O
	virtual HRESULT asyncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								  IN OPTIONAL DWORD transactionID = 0, OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncWriteImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD transactionID = 0,
								   IN OPTIONAL SOCmnVariant* pValues = NULL, IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL,
								   OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncRefreshImpl(IN DWORD maxAge, IN OPTIONAL DWORD transactionID = 0,
									 OUT OPTIONAL DWORD* cancelID = NULL);
	virtual HRESULT asyncCancelImpl(IN DWORD cancelID);

	// advising
	virtual HRESULT advise(void);
	virtual HRESULT unadvise(void);
	virtual BOOL getReadConnection(OUT DWORD* connection);
	virtual BOOL getWriteConnection(OUT DWORD* connection);

	OPCHANDLE getServerHandle(void) const
	{
		return m_serverHandle;
	}
	OPCHANDLE getClientHandle(void)
	{
		return (OPCHANDLE)getObjectDaCGroup()->getOpcHandle();
	}

	// group management
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_GRPPAR_ALL_MGT);

protected:
	SODaCAdviseSink* m_adviseSink;  // advise sink for asynchronous results OPC V1.0A
	SODaCDataCallback* m_dataCallback;// advise sink for asynchronous results OPC V2.0
	OPCHANDLE m_serverHandle;       // server handle
	BYTE m_active;                  // activation state of the group
	DWORD m_taskSyncRead;       // task for cyclic reads

	SOCltInterfaceAccess<IUnknown, &IID_IUnknown> m_unknown;
	SOCltInterfaceAccess<IOPCItemMgt, &_ATL_IIDOF(IOPCItemMgt)> m_itemMgt;
	SOCltInterfaceAccess<IOPCSyncIO, &_ATL_IIDOF(IOPCSyncIO)> m_syncIO;
	SOCltInterfaceAccess<IOPCSyncIO2, &_ATL_IIDOF(IOPCSyncIO2)> m_syncIO2;
	SOCltInterfaceAccess<IOPCAsyncIO, &_ATL_IIDOF(IOPCAsyncIO)> m_asyncIO;
	SOCltInterfaceAccess<IOPCAsyncIO2, &_ATL_IIDOF(IOPCAsyncIO2)> m_asyncIO2;
	SOCltInterfaceAccess<IOPCAsyncIO3, &_ATL_IIDOF(IOPCAsyncIO3)> m_asyncIO3;
	SOCltInterfaceAccess<IOPCGroupStateMgt, &_ATL_IIDOF(IOPCGroupStateMgt)> m_grpState;
	SOCltInterfaceAccess<IOPCGroupStateMgt2, &_ATL_IIDOF(IOPCGroupStateMgt2)> m_grpState2;
	SOCltInterfaceAccess<IOPCPublicGroupStateMgt, &_ATL_IIDOF(IOPCPublicGroupStateMgt)> m_grpStatePub;
	SOCltInterfaceAccess<IOPCItemDeadbandMgt, &_ATL_IIDOF(IOPCItemDeadbandMgt)> m_itemDeadbandMgt;
	SOCltInterfaceAccess<IOPCItemSamplingMgt, &_ATL_IIDOF(IOPCItemSamplingMgt)> m_itemSamplingMgt;

	CComPtr<IMalloc> m_malloc;      // IMalloc interface

	virtual ~SODaCComGroupImpl(void);

	// I/O callbacks
	virtual void onDataChangeBegin(void);
	virtual void onDataChangeEnd(void);

	virtual void onSyncRead(IN DWORD count, IN SODaCItem** itemArray, IN DWORD maxAge, IN HRESULT res);
	virtual void onReadAdvise(IN DWORD transactionID, IN BYTE* data);
	virtual void onWriteAdvise(IN DWORD transactionID, IN BYTE* data);
	virtual void onCancelAdvise(IN DWORD dwTransid, IN OPCHANDLE hGroup);

	// do the state transitions for the items of the group
	virtual HRESULT doItemsStateTransition(void);

	// do the state transitions
	virtual HRESULT doStateTransitionImpl(BOOL deep, IN BYTE stateDeep);

	// do the disconnection of the group (helper of doStateTransition)
	virtual HRESULT doDisconnectImpl(void);

	// create item proxies for existing group (Get...GroupByName)
	HRESULT createItems(void);

	// query all interfaces
	HRESULT queryAllInterfaces(IUnknown* punknown);

	// item list helper
	HRESULT fillServerHandles(IN SOCmnList<SODaCItem> *itemList, OUT DWORD* numItems,
							  OUT OPCHANDLE** handles);
	HRESULT fillServerHandles(IN DWORD count, IN SODaCItem** itemArray, OUT DWORD* numItems,
							  OUT OPCHANDLE** handles);
	HRESULT updateItem(IN DWORD singleItemHandle, IN DWORD whatParameters);
}; // SODaCComGroupImpl



//-----------------------------------------------------------------------------
// SODaCComTaskSyncRead                                                       |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCComTaskSyncRead : public SOCltCyclicTask
{
public:
	SODaCComTaskSyncRead(IN SOCltElement* element, IN DWORD cycle);
	virtual void execute();
}; // SODaCComTaskSyncRead



//-----------------------------------------------------------------------------
// SODaCComGroupDefault                                                       |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCComGroupDefault :
	public SODaCGroup,
	virtual public SODaCComGroupImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCComGroupDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#define ClientGroup SODaCComGroupDefault

#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif

