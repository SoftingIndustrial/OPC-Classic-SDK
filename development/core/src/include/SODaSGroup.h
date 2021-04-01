#ifndef _SODASGROUP_H_
#define _SODASGROUP_H_

#include <opcda.h>
#include "SOCmnElement.h"
#include "SODaSItemTag.h"
#include "SODaSRequest.h"
#include "SODaSTransaction.h"

#pragma pack(push, 4)


// OPC data formats
extern const UINT OPCSTMFORMATDATA;
extern const UINT OPCSTMFORMATDATATIME;
extern const UINT OPCSTMFORMATWRITECOMPLETE;

//-----------------------------------------------------------------------------
// SODaSDataCallback                                                          |
//-----------------------------------------------------------------------------

// IDataObject (OPC V1.0)
#define SODASDATACALLBACK_CT_DATAOBJECT      0x01
// IConnectionPoint (OPC V2.0)
#define SODASDATACALLBACK_CT_CONNECTIONPOINT 0x02
// OPC Tunnel
#define SODASDATACALLBACK_CT_TUNNEL          0x04

#define SODASDATACALLBACK_CT_ALL             0x07

#ifdef SOFEATURE_DCOM

class SODAS_EXPORT SODaSDataCallback : SOCmnData
{
public:
	SODaSDataCallback();
	virtual ~SODaSDataCallback();

	DWORD m_connectionId;       // connection id
	union
	{
		IUnknown* m_unknown;
		IAdviseSink* m_adviseSink;          // client's advise sink interface
		IOPCDataCallback* m_dataCallback;   // client's data callback interface
	};
	CLIPFORMAT m_format;        // data format of advise (only for OPC V1.0)
	BYTE m_connectionType;      // connection type
}; // SODaSDataCallback



//-----------------------------------------------------------------------------
// SODaSGroupIAdvise                                                          |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSGroupIAdvise
{
protected:
	SODaSGroupIAdvise();
	virtual ~SODaSGroupIAdvise();

	virtual BOOL supportConnectionType(IN BYTE connectionType);

	// advise management
	virtual HRESULT adviseDataCallback(IN BYTE connectionType, IN IUnknown* pUnk,
									   IN CLIPFORMAT format, OUT DWORD* connectionId);
	virtual HRESULT unadviseDataCallback(IN BYTE connectionType, IN DWORD connectionId);

	// callbacks
	virtual void onAdviseDataCallback(IN BYTE connectionType, IN DWORD connection,
									  IN CLIPFORMAT format);
	virtual void onUnadviseDataCallback(IN BYTE connectionType, IN DWORD connection);

	// check if connection is valid
	virtual BOOL validConnection(IN BYTE connectionType, IN DWORD connection, IN BOOL read);

	// list functions
	virtual SOCmnListPosition getAdviseStartPositionAndLock(IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL);
	virtual void getNextAdvise(IN OUT SOCmnListPosition& pos, OUT SODaSDataCallback& cb,
							   IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL) const;
	virtual void doAdviseUnlock(void);
}; // SODaSGroupIAdvise


//-----------------------------------------------------------------------------
// SODaSGroupAdviseOneOnlyV2                                                  |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSGroupAdviseOneOnlyV2 : virtual public SODaSGroupIAdvise
{
protected:
	SODaSDataCallback m_connectionPoint;
	SOCmnMutex m_advisesLock;

	virtual BOOL supportConnectionType(IN BYTE connectionType);

	virtual HRESULT adviseDataCallback(IN BYTE connectionType, IN IUnknown* pUnk,
									   IN CLIPFORMAT format, OUT DWORD* connectionId);
	virtual HRESULT unadviseDataCallback(IN BYTE connectionType, IN DWORD connectionId);

	// check if connection is valid
	virtual BOOL validConnection(IN BYTE connectionType, IN DWORD connection, IN BOOL read);

	// list functions
	virtual SOCmnListPosition getAdviseStartPositionAndLock(IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL);
	virtual void getNextAdvise(IN OUT SOCmnListPosition& pos, OUT SODaSDataCallback& cb,
							   IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL) const;
	virtual void doAdviseUnlock(void);
}; // SODaSGroupAdviseOneOnlyV2


//-----------------------------------------------------------------------------
// SODaSGroupAdviseFull                                                       |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSGroupAdviseFull : virtual public SODaSGroupIAdvise
{
protected:
	SODaSGroupAdviseFull();
	virtual ~SODaSGroupAdviseFull();

	SOCmnList<SODaSDataCallback> m_advises;
	SOCmnMutex m_advisesLock;

	virtual BOOL supportConnectionType(IN BYTE connectionType);

	virtual HRESULT adviseDataCallback(IN BYTE connectionType, IN IUnknown* pUnk,
									   IN CLIPFORMAT format, OUT DWORD* connectionId);
	virtual HRESULT unadviseDataCallback(IN BYTE connectionType, IN DWORD connectionId);

	// check if connection is valid
	virtual BOOL validConnection(IN BYTE connectionType, IN DWORD connection, IN BOOL read);

	// list functions
	virtual SOCmnListPosition getAdviseStartPositionAndLock(IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL);
	virtual void getNextAdvise(IN OUT SOCmnListPosition& pos, OUT SODaSDataCallback& cb,
							   IN OPTIONAL BYTE connectionType = SODASDATACALLBACK_CT_ALL) const;
	virtual void doAdviseUnlock(void);
}; // SODaSGroupAdviseFull

#endif // SOFEATURE_DCOM



//-----------------------------------------------------------------------------
// SODaSGroup                                                                 |
//-----------------------------------------------------------------------------

#ifdef SOFEATURE_DCOM
class SODaSComGroup;
class SODaSComServer;
#endif
class SODaSTransaction;
class SODaSEntry;
class SODaSItem;
class SODaSRequest;
class SODaSServer;
class SODaSItemData;
#ifdef SOFEATURE_SOAP
class SODaSXmlServer;
#endif
#ifdef SOFEATURE_TUNNEL
class SODaSTPGroup;
class SODaSTPServer;
#endif

class SODAS_EXPORT SODaSGroup :
	public SOCmnElement,
	virtual public SOCmnElementListLeaf,
#ifdef SOFEATURE_DCOM
	virtual public SODaSGroupIAdvise,
#endif
	virtual public SOCmnElementNameString
{
	friend class SODaSTransaction;
	friend class SODaSEntry;
	friend class SODaSRequest;
	friend class SODaSServer;
	friend class SODaSItem;
#ifdef SOFEATURE_DCOM
	friend class SODaSComGroup;
	friend class SODaSComServer;
#endif
#ifdef SOFEATURE_SOAP
	friend class SODaSXmlServer;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SODaSTPGroup;
	friend class SODaSTPServer;
#endif

public:
	SODaSGroup(void);

	// object identity
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN DWORD key);
	virtual BOOL isUniqueName(void) const;

	// tree management
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual HRESULT setName(IN LPCTSTR name);

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// activation state
	virtual BOOL setActive(IN BOOL active);
	BOOL getActive(void) const;

	// update rate in ms
	virtual BOOL setUpdateRate(IN DWORD updateRate);
	DWORD getUpdateRate(void) const;
	virtual void recalcRevUpdateRate(void);

	// client handle from OPC client
	virtual BOOL setClientHandle(IN OPCHANDLE clientHandle);
	OPCHANDLE getClientHandle(void) const;

#ifdef SOFEATURE_DCOM
	// time zone
	virtual BOOL setTimeBias(IN LONG timeBias);
	LONG getTimeBias(void) const;
#endif

	// deadband in percent
	virtual BOOL setDeadBand(IN FLOAT deadBand);
	FLOAT getDeadBand(void) const;

	// local Id
	virtual BOOL setLCID(IN DWORD lcid);
	DWORD getLCID(void) const;

	// enable flag for asynch I/O
	virtual BOOL setEnable(IN BOOL enable);
	BOOL getEnable(void) const;

#ifdef SOFEATURE_DCOM
	// keep alive time in ms
	virtual BOOL setKeepAliveTime(IN DWORD keepAlive);
	DWORD getKeepAliveTime(void) const
	{
		return m_keepAliveTime;
	};
#endif
#ifdef SOFEATURE_SOAP
	// ping rate in ms
	virtual BOOL setPingRate(IN DWORD pingRate);
	DWORD getPingRate(void) const
	{
		return m_pingRate;
	};
#endif

	// server handle
	OPCHANDLE getServerHandle(void) const;

	// item management
	virtual HRESULT validateItem(IN LPCTSTR id, IN LPCTSTR accessPath,
								 IN VARTYPE reqDataType, OUT VARTYPE* nativeDataType,
								 OUT DWORD* accessRights, OUT OPTIONAL SODaSItemTag** tag = NULL);
	virtual HRESULT addItem(IN LPCTSTR id, IN LPCTSTR accessPath,
							IN BOOL active, IN OPCHANDLE clientHandle, IN VARTYPE reqDataType,
							OUT SODaSItem** item, OUT VARTYPE* nativeDataType,
							OUT DWORD* accessRights);
	virtual HRESULT removeItem(IN OPCHANDLE serverHandle);

	SODaSItem* getItemByHandle(IN OPCHANDLE serverHandle);

	// item list
	inline SOCmnList<SODaSItem> *getItemList(void)
	{
		return (SOCmnList<SODaSItem> *)getLeafList();
	}
	inline DWORD getItemCount(void)
	{
		return getLeafList()->getCount();
	}

	// group cloning
	virtual HRESULT clone(IN SODaSServer* server, IN LPCTSTR name,
						  OUT SODaSGroup** clone);

	// public group state management
	virtual BOOL isPublic(void);

	// removed flag
	BOOL isRemoved(void);

	// get the pointer of the cyclic transaction
	SODaSTransaction* getCyclicTransaction();

	// do notify all changes ?
	virtual BOOL isNotifyAllChanges(void);

#ifdef SOFEATURE_DCOM
	// COM object handling
	void* getMe(void);
	ULONG releaseMe(IN void* me);
#endif

#ifdef SOFEATURE_DCOM
	// check interface access
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#endif

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// method callbacks
	virtual void onAddItems(void);
	virtual void onRemoveItems(void);
	virtual void onChangeStateItems(void);

	// cyclic requests
	virtual BOOL addCyclicRequest(IN SODaSItem* item);
	virtual BOOL removeCyclicRequest(IN SODaSItem* item);

	// dynamic item validation
	virtual BOOL hasPrePostValidateItems(void);
	virtual DWORD preValidateItems(BOOL addItemsOrValidateItems, SOCmnList<SODaSItemData> *pItemDataList);
	virtual void postValidateItems(DWORD preValidateItemsCookie, SOCmnList<SODaSItemData> *pItemDataList);
	virtual void preCreateItemEnumerator(void);

	virtual BOOL rejectCyclicDataBeforeInit(void);

	// copy existing cyclic transaction
	void copyCyclicTransaction(IN DWORD dwConnection, IN SODaSRequest::requestType reqType, IN DWORD maxAge,
							   IN BYTE connectionType, OUT SODaSTransaction** dest);

protected:
	virtual ~SODaSGroup();

	void* m_me;                   // COM group object
	DWORD m_updateRate;           // update rate in ms
	DWORD m_keepAliveTime;        // keep alive time in ms (DCOM)
	DWORD m_pingRate;                           // subscription ping rate in ms (SOAP)
	OPCHANDLE m_clientHandle;     // client handle from OPC client
#ifdef SOFEATURE_DCOM
	LONG m_timeBias;              // time zone
#endif
	FLOAT m_deadBand;             // deadband in percent
	DWORD m_lcid;                 // local Id
	OPCHANDLE m_serverHandle;     // server handle
	SOCmnSync m_meSync;           // synchronisation of server handle setting
	SOCmnPointer<SODaSTransaction> m_cyclicTransaction; // cyclic transaction
	SODaSGroup* m_publicTemplate; // template object of public group
	DWORD m_initTime;             // init time of the cyclic transaction in ms since system start
	WORD m_initCnt;               // init cyclic transaction counter
	BYTE m_correctNTtimer;        // NT timer correction value
	BYTE m_active;                // activation state
	BYTE m_removed;               // removed flag
	BYTE m_enable;                // enable flag for async I/O
	BYTE m_notifiyAllChanges;     // notifiy all changes flag

	// don't check the client provided item handles
	virtual BOOL trustClientItemHandles(void);

#ifdef SOFEATURE_SOAP
	virtual BOOL hasPolledRefreshTransactions(void);
	virtual DWORD cancelPolledRefreshTransactions(void);
#endif

	// attribute callbacks
	virtual void onSetActive(IN BOOL active);
	virtual void onSetUpdateRate(IN DWORD updateRate);
	virtual void onSetKeepAliveTime(IN DWORD keepAlive);
	virtual void onSetPingRate(IN DWORD pingRate);
	virtual void onSetClientHandle(IN OPCHANDLE clientHandle);
#ifdef SOFEATURE_DCOM
	virtual void onSetTimeBias(IN LONG timeBias);
#endif
	virtual void onSetDeadBand(IN FLOAT percentDeadBand);
	virtual void onSetLCID(IN DWORD lcid);
	virtual void onSetEnable(IN BOOL enable);

	// advise callbacks
	virtual void onDataChange(IN SODaSTransaction* transaction, IN SODaSDataCallback* advise);
	virtual void onReadComplete(IN SODaSTransaction* transaction, IN SODaSDataCallback* advise);
	virtual void onWriteComplete(IN SODaSTransaction* transaction, IN SODaSDataCallback* advise);
	virtual void onCancelComplete(IN SODaSTransaction* transaction, IN SODaSDataCallback* advise);

	// cyclic transaction helpers
	virtual BOOL updateSpan(IN DWORD now, IN DWORD* span);
	virtual DWORD initSpan(IN DWORD toLate);

	// public group
	BOOL isPublicGroup(void) const;
	BOOL isPublicTemplate(void);

	// removed flag
	void setRemoved(void);

	void resetSendValues(void);

#ifdef SOFEATURE_DCOM
	// query interface for own interfaces
	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
#endif

	void calcNotifyAllChanges(void);

	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCmnObject* getObjectCmnElementIName(void);

#ifdef SOFEATURE_DCOM
	// create transaction from interface parameters
	HRESULT createTransaction(IN BOOL read, IN BYTE connectionType,
							  IN DWORD connection, IN DWORD source, IN SOCmnEventPointer reqCompEvent, IN DWORD numItems,
							  IN OPCHANDLE* serverHandles, IN VARIANT* itemValues,
							  IN OPCITEMSTATE** readItemValues, IN DWORD* pMaxAge, IN VARIANT** ppValues,
							  IN WORD** ppQualities, IN FILETIME** ppTimeStamps, IN OPCITEMVQT* pItemVQT,
							  OUT SODaSTransaction** transaction, OUT DWORD* transactionID, OUT HRESULT** errors);

	// fill request list from interface parameters
	HRESULT fillRequestList(IN SODaSRequest::requestType reqType,
							IN SODaSRequest::requestOperation reqOp, IN DWORD accessRight, IN SOCmnEventPointer reqCompEvent,
							IN DWORD numItems, IN OPCHANDLE* serverHandles,
							IN VARIANT* writeItemValues, IN BOOL checkAccessRights,
							IN OPCITEMSTATE** readItemValues, IN DWORD* pMaxAge, IN VARIANT** ppValues,
							IN WORD** ppQualities, IN FILETIME** ppTimeStamps, IN OPCITEMVQT* pItemVQT,
							OUT SOCmnList<SODaSRequest> *reqList, OUT HRESULT** errors);
#endif

#ifdef SOFEATURE_TUNNEL
	// create transaction from interface parameters
	HRESULT createTransaction(IN BOOL read, IN BYTE connectionType,
							  IN DWORD connection, IN DWORD source, IN SOCmnEventPointer reqCompEvent,
							  IN DWORD numItems, IN OPCHANDLE* serverHandles,
							  IN DWORD* pMaxAge, IN OUT SOCmnVariant* pValues,
							  IN OUT WORD* pQualities, IN BOOL* pQualitySpecified,
							  IN OUT SOCmnDateTime* pTimeStamps, IN BOOL* pTimeStampSpecified,
							  OUT SODaSTransaction** transaction, OUT DWORD* transactionID, OUT HRESULT* errors);

	// fill request list from interface parameters
	HRESULT fillRequestList(IN SODaSRequest::requestType reqType,
							IN SODaSRequest::requestOperation reqOp, IN DWORD accessRight, IN BOOL checkAccessRights, IN SOCmnEventPointer reqCompEvent,
							IN DWORD numItems, IN OPCHANDLE* serverHandles,
							IN DWORD* pMaxAge, IN OUT SOCmnVariant* pValues,
							IN OUT WORD* pQualities, IN BOOL* pQualitySpecified,
							IN OUT SOCmnDateTime* pTimeStamps, IN BOOL* pTimeStampSpecified,
							OUT SOCmnList<SODaSRequest> *reqList, OUT HRESULT* errors);
#endif

#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_COMIF_HOOKS
	// interface function callbacks
	virtual HRESULT onBGetState(OUT DWORD* pUpdateRate, OUT BOOL* pActive, OUT LPWSTR* ppName, OUT LONG* pTimeBias, OUT FLOAT* pDeadband, OUT DWORD* pLCID, OUT OPCHANDLE* phClientGroup, OUT OPCHANDLE* phServerGroup);
	virtual HRESULT onBSetState(IN DWORD* pRequestedUpdateRate, OUT DWORD* pRevisedUpdateRate, IN BOOL* pActive, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup);
	virtual HRESULT onBSetName(IN LPCWSTR szName);
	virtual HRESULT onBCloneGroup(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBGetState(OUT BOOL* pPublic);
	virtual HRESULT onBMoveToPublic(void);
	virtual HRESULT onBAddItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, OUT OPCITEMRESULT** ppAddResults, OUT HRESULT** ppErrors);
	virtual HRESULT onBValidateItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, IN BOOL bBlobUpdate, OUT OPCITEMRESULT** ppValidationResults, OUT HRESULT** ppErrors);
	virtual HRESULT onBRemoveItems(IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors);
	virtual HRESULT onBSetActiveState(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN BOOL bActive, OUT HRESULT** ppErrors);
	virtual HRESULT onBSetClientHandles(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCHANDLE* phClient, OUT HRESULT** ppErrors);
	virtual HRESULT onBSetDatatypes(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARTYPE* pRequestedDatatypes, OUT HRESULT** ppErrors);
	virtual HRESULT onBCreateEnumerator(IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBRead(IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT OPCITEMSTATE** ppItemValues, OUT HRESULT** ppErrors);
	virtual HRESULT onBWrite(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, OUT HRESULT** ppErrors);
	virtual HRESULT onBRead(IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT DWORD* pTransactionID, OUT HRESULT** ppErrors);
	virtual HRESULT onBWrite(IN DWORD dwConnection, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD* pTransactionID, OUT HRESULT** ppErrors);
	virtual HRESULT onBRefresh(IN DWORD dwConnection, IN OPCDATASOURCE dwSource, OUT DWORD* pTransactionID);
	virtual HRESULT onBCancel(IN DWORD dwTransactionID);
	virtual HRESULT onBRead(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID, OUT HRESULT** ppErrors);
	virtual HRESULT onBWrite(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID, OUT HRESULT** ppErrors);
	virtual HRESULT onBRefresh2(IN OPCDATASOURCE dwSource, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID);
	virtual HRESULT onBCancel2(IN DWORD dwCancelID);
	virtual HRESULT onBSetEnable(IN BOOL bEnable);
	virtual HRESULT onBGetEnable(OUT BOOL* pbEnable);
	virtual HRESULT onBAdvise(IN IUnknown* pUnk, OUT DWORD* pdwCookie);
	virtual HRESULT onBUnadvise(IN DWORD dwCookie);
	virtual HRESULT onBDAdvise(IN FORMATETC* format, IN DWORD advf, IN IAdviseSink* pAdvSink, OUT DWORD* pdwConnection);
	virtual HRESULT onBDUnadvise(IN DWORD dwConnection);
	virtual HRESULT onBReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors);
	virtual HRESULT onBWriteVQT(IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);
	virtual HRESULT onBReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors);
	virtual HRESULT onBWriteVQT(IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors);
	virtual HRESULT onBRefreshMaxAge(IN DWORD maxAge, IN DWORD transactionID, OUT DWORD* pCancelID);
	virtual HRESULT onBSetKeepAlive(IN DWORD keepAliveTime, OUT DWORD* pRevisedKeepAliveTime);
	virtual HRESULT onBGetKeepAlive(OUT DWORD* pKeepAliveTime);
	virtual HRESULT onBSetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, IN FLOAT* pPercentDeadband, OUT HRESULT** ppErrors);
	virtual HRESULT onBGetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, OUT FLOAT** ppPercentDeadband, OUT HRESULT** ppErrors);
	virtual HRESULT onBClearItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors);
	virtual void onEGetState(IN HRESULT res, IN DWORD* pUpdateRate, IN BOOL* pActive, IN LPWSTR* ppName, IN LONG* pTimeBias, IN FLOAT* pDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup, IN OPCHANDLE* phServerGroup);
	virtual void onESetState(IN HRESULT res, IN DWORD* pRequestedUpdateRate, IN DWORD* pRevisedUpdateRate, IN BOOL* pActive, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup);
	virtual void onESetName(IN HRESULT res, IN LPCWSTR szName);
	virtual void onECloneGroup(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onEGetState(IN HRESULT res, IN BOOL* pPublic);
	virtual void onEMoveToPublic(IN HRESULT res);
	virtual void onEAddItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, IN OPCITEMRESULT** ppAddResults, IN HRESULT** ppErrors);
	virtual void onEValidateItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, IN BOOL bBlobUpdate, IN OPCITEMRESULT** ppValidationResults, IN HRESULT** ppErrors);
	virtual void onERemoveItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN HRESULT** ppErrors);
	virtual void onESetActiveState(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN BOOL bActive, IN HRESULT** ppErrors);
	virtual void onESetClientHandles(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCHANDLE* phClient, IN HRESULT** ppErrors);
	virtual void onESetDatatypes(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARTYPE* pRequestedDatatypes, IN HRESULT** ppErrors);
	virtual void onECreateEnumerator(IN HRESULT res, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onERead(IN HRESULT res, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCITEMSTATE** ppItemValues, IN HRESULT** ppErrors);
	virtual void onEWrite(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN HRESULT** ppErrors);
	virtual void onERead(IN HRESULT res, IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD* pTransactionID, IN HRESULT** ppErrors);
	virtual void onEWrite(IN HRESULT res, IN DWORD dwConnection, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD* pTransactionID, IN HRESULT** ppErrors);
	virtual void onERefresh(IN HRESULT res, IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD* pTransactionID);
	virtual void onECancel(IN HRESULT res, IN DWORD dwTransactionID);
	virtual void onERead(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD dwTransactionID, IN DWORD* pdwCancelID, IN HRESULT** ppErrors);
	virtual void onEWrite(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD dwTransactionID, IN DWORD* pdwCancelID, IN HRESULT** ppErrors);
	virtual void onERefresh2(IN HRESULT res, IN OPCDATASOURCE dwSource, IN DWORD dwTransactionID, IN DWORD* pdwCancelID);
	virtual void onECancel2(IN HRESULT res, IN DWORD dwCancelID);
	virtual void onESetEnable(IN HRESULT res, IN BOOL bEnable);
	virtual void onEGetEnable(IN HRESULT res, IN BOOL* pbEnable);
	virtual void onEAdvise(IN HRESULT res, IN IUnknown* pUnk, IN DWORD* pdwCookie);
	virtual void onEUnadvise(IN HRESULT res, IN DWORD dwCookie);
	virtual void onEDAdvise(IN HRESULT res, IN FORMATETC* format, IN DWORD advf, IN IAdviseSink* pAdvSink, IN DWORD* pdwConnection);
	virtual void onEDUnadvise(IN HRESULT res, IN DWORD dwConnection);
	virtual void onEReadMaxAge(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors);
	virtual void onEWriteVQT(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);
	virtual void onEReadMaxAge(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors);
	virtual void onEWriteVQT(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors);
	virtual void onERefreshMaxAge(IN HRESULT res, IN DWORD maxAge, IN DWORD transactionID, OUT DWORD* pCancelID);
	virtual void onESetKeepAlive(IN HRESULT res, IN DWORD keepAliveTime, OUT DWORD* pRevisedKeepAliveTime);
	virtual void onEGetKeepAlive(IN HRESULT res, OUT DWORD* pKeepAliveTime);
	virtual void onESetItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN FLOAT* pPercentDeadband, OUT HRESULT** ppErrors);
	virtual void onEGetItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, OUT FLOAT** ppPercentDeadband, OUT HRESULT** ppErrors);
	virtual void onEClearItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors);
#endif // SOFEATURE_COMIF_HOOKS
#endif // SOFEATURE_DCOM
}; // SODaSGroup


inline BOOL SODaSGroup::getActive(void) const
{
	return m_active;
}

inline DWORD SODaSGroup::getUpdateRate(void) const
{
	return m_updateRate;
}

inline OPCHANDLE SODaSGroup::getClientHandle(void) const
{
	return m_clientHandle;
}

#ifdef SOFEATURE_DCOM
inline LONG SODaSGroup::getTimeBias(void) const
{
	return m_timeBias;
}
#endif

inline FLOAT SODaSGroup::getDeadBand(void) const
{
	return m_deadBand;
}

inline DWORD SODaSGroup::getLCID(void) const
{
	return m_lcid;
}

inline SODaSTransaction* SODaSGroup::getCyclicTransaction()
{
	m_cyclicTransaction.addRef();
	return m_cyclicTransaction;
}

inline void SODaSGroup::setRemoved(void)
{
	m_removed = TRUE;
}

inline BOOL SODaSGroup::getEnable(void) const
{
	return m_enable;
}

#define GenericGroup SODaSGroupDefault




//-----------------------------------------------------------------------------
// SODaSGroupDefault                                                          |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSGroupDefault :
	public SODaSGroup
#ifdef SOFEATURE_DCOM
#ifndef SOOS_WINDOWS_CE
	, virtual public SODaSGroupAdviseFull
#else
	, virtual public SODaSGroupAdviseOneOnlyV2
#endif
#endif
{
public:
	SODaSGroupDefault(void);
	virtual ~SODaSGroupDefault(void) {}
}; // SODaSGroupDefault

#pragma pack(pop)
#endif
