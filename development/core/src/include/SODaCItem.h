#ifndef _SODACITEM_H_
#define _SODACITEM_H_

#include "SOCltElement.h"
#include "SODaCEntry.h"
#include "opcda.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCItemIAttr                                                             |
//-----------------------------------------------------------------------------

#define SODACITEM_NO_DEADBAND -1.0

class SODaCGroup;
#ifdef SOFEATURE_DCOM
class SODaCComGroupImpl;
#endif
#ifdef SOFEATURE_SOAP
class SODaCXmlGroupImpl;
#endif

class SODAC_EXPORT SODaCItemIAttr
{
	friend class SODaCGroup;
#ifdef SOFEATURE_DCOM
	friend class SODaCComGroupImpl;
#endif
#ifdef SOFEATURE_SOAP
	friend class SODaCXmlGroupImpl;
#endif
public:
	SODaCItemIAttr();

	// item data
	virtual BOOL setAccessPath(LPCTSTR accessPath);
	virtual SOCmnString getAccessPath(void);

	virtual BOOL setItemID(LPCTSTR itemID);
	virtual SOCmnString getItemID(void);

	virtual BOOL setItemPath(LPCTSTR itemPath);
	virtual SOCmnString getItemPath(void);

	virtual VARTYPE getReqDatatype(void);
	virtual BOOL setReqDatatype(IN VARTYPE datatype);

	virtual VARTYPE getNativeDatatype(void);
	virtual DWORD getAccessRights(void);
	virtual BOOL getEU(OUT OPCEUTYPE* type, OUT SOCmnVariant& info);
	virtual OPCHANDLE getClientHandle(void);

	virtual BOOL setDeadBand(IN FLOAT deadBand);
	virtual FLOAT getDeadBand(void);

	// data member callbacks
	virtual void onSetReqDatatype(void);
	virtual void onSetAccessPath(void);
	virtual void onSetItemID(void);
	virtual void onSetItemPath(void);
	virtual void onSetNativeDatatype(void);
	virtual void onSetAccessRights(void);
	virtual void onSetEU(void);
	virtual void onSetDeadBand(void);

protected:
	virtual ~SODaCItemIAttr();

	// server provided item data
	virtual BOOL setNativeDatatype(IN VARTYPE datatype);
	virtual BOOL setAccessRights(IN DWORD accessRights);
	virtual BOOL setEU(IN OPCEUTYPE type, IN SOCmnVariant& info);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectDaCItemIAttr(void);
}; // SODaCItemIAttr


//-----------------------------------------------------------------------------
// SODaCItemAttrFull                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemAttrFull : virtual public SODaCItemIAttr
{
public:
	SODaCItemAttrFull();

	// item data
	virtual BOOL setAccessPath(LPCTSTR accessPath);
	virtual SOCmnString getAccessPath(void);

	virtual BOOL setItemID(LPCTSTR itemID);
	virtual SOCmnString getItemID(void);

	virtual BOOL setItemPath(LPCTSTR itemPath);
	virtual SOCmnString getItemPath(void);

	virtual VARTYPE getReqDatatype(void);
	virtual BOOL setReqDatatype(IN VARTYPE datatype);

	virtual VARTYPE getNativeDatatype(void);
	virtual DWORD getAccessRights(void);
	virtual BOOL getEU(OUT OPCEUTYPE* type, OUT SOCmnVariant& info);

	virtual BOOL setDeadBand(IN FLOAT deadBand);
	virtual FLOAT getDeadBand(void);

protected:
	SOCmnString m_itemID;           // fully quallified item ID
	SOCmnString m_itemAndAccessPath;    // item path (XML) or access path (DCOM)
	VARTYPE m_reqDatatype;          // requested datatype
	VARTYPE m_nativeDatatype;       // native datatype of the namespace tag
	DWORD m_accessRights;           // access rights
	OPCEUTYPE m_EUType;             // engineering unit type
	SOCmnVariant m_EUInfo;          // engineering units
	FLOAT m_deadBand;         // item deadand

	// server provided item data
	virtual BOOL setNativeDatatype(IN VARTYPE datatype);
	virtual BOOL setAccessRights(IN DWORD accessRights);
	virtual BOOL setEU(IN OPCEUTYPE type, IN SOCmnVariant& info);
}; // SODaCItemAttrFull


//-----------------------------------------------------------------------------
// SODaCItemAttrMin                                                           |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemAttrMin : virtual public SODaCItemIAttr
{
public:
	virtual BOOL setItemID(LPCTSTR itemID);
	virtual SOCmnString getItemID(void);
protected:
	SOCmnString m_itemID;           // fully quallified item ID
}; // SODaCItemAttrMin




//-----------------------------------------------------------------------------
// SODaCItemIData                                                             |
//-----------------------------------------------------------------------------

#define SODACITEM_TRANSACTIONID_SUBSCRIPTION      0x0
#define SODACITEM_TRANSACTIONID_SYNC              0xFFFFFFFF
#define SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED 0xFFFFFFFE

class SODaCAdviseSink;
class SODaCDataCallback;
class SODaCGroup;

class SODAC_EXPORT SODaCItemIData
{
	friend class SODaCAdviseSink;
	friend class SODaCDataCallback;
	friend class SODaCGroup;
#ifdef SOFEATURE_SOAP
	friend class SODaCXmlGroupImpl;
#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_DCOM
	friend class SODaCComGroupImpl;
#endif

public:
	SODaCItemIData();

	// read value
	virtual HRESULT getReadResult(void) const;
	virtual WORD getReadQuality(void) const;
	virtual BOOL getReadTimeStamp(OUT SOCmnDateTime& timeStamp) const;
	virtual BOOL getReadValue(OUT SOCmnVariant& value);

	// write value
	virtual HRESULT getWriteResult(void) const;
	virtual BOOL setWriteValue(IN SOCmnVariant& value);
	virtual BOOL getWriteValue(OUT SOCmnVariant& value);
#ifdef SOOS_WINDOWS
	virtual BOOL getWriteValue(OUT LPVARIANT value);
#endif

	// I/O callbacks
	virtual void onSetReadResult(void);
	virtual void onSetReadValue(void);
	virtual void onSetReadQuality(void);
	virtual void onSetReadTimeStamp(void);
	virtual void onSetWriteResult(void);
	virtual void onSetWriteValue(void);

protected:
	virtual ~SODaCItemIData();

	// read value
	virtual void setRead(IN HRESULT res, IN SOCmnVariant* value, IN WORD quality, IN SOCmnDateTime* timeStamp, IN DWORD transactionID);
	virtual BOOL setReadResult(IN HRESULT res);
	virtual BOOL setReadQuality(IN WORD quality);
	virtual BOOL setReadTimeStamp(IN SOCmnDateTime* timeStamp);
	virtual BOOL setReadValue(IN SOCmnVariant* value);

	// write value
	virtual void setWrite(IN HRESULT res, IN DWORD transactionID);
	virtual BOOL setWriteResult(IN HRESULT res);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectDaCItemIData(void);
};


//-----------------------------------------------------------------------------
// SODaCItemDataFull                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemDataFull : virtual public SODaCItemIData
{
public:
	SODaCItemDataFull();

	// read value
	virtual HRESULT getReadResult(void) const;
	virtual WORD getReadQuality(void) const;
	virtual BOOL getReadTimeStamp(OUT SOCmnDateTime& timeStamp) const;
	virtual BOOL getReadValue(OUT SOCmnVariant& value);

	// write value
	virtual HRESULT getWriteResult(void) const;
	virtual BOOL setWriteValue(IN SOCmnVariant& value);
	virtual BOOL getWriteValue(OUT SOCmnVariant& value);
#ifdef SOOS_WINDOWS
	virtual BOOL getWriteValue(OUT LPVARIANT value);
#endif

protected:
	WORD m_readQuality;             // read quality
	HRESULT m_readResult;           // read result
	SOCmnDateTime m_readTimeStamp;      // read time stamp
	SOCmnVariant m_readValue;       // read value
	HRESULT m_writeResult;          // write result
	SOCmnVariant m_writeValue;      // write value

	// read value
	virtual BOOL setReadResult(IN HRESULT res);
	virtual BOOL setReadQuality(IN WORD quality);
	virtual BOOL setReadTimeStamp(IN SOCmnDateTime* timeStamp);
	virtual BOOL setReadValue(IN SOCmnVariant* value);

	// write value
	virtual BOOL setWriteResult(IN HRESULT res);
}; // SODaCItemDataFull


//-----------------------------------------------------------------------------
// SODaCItemStateTransitionTask                                               |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemStateTransitionTask : public SOCltTaskStateTransition
{
public:
	SODaCItemStateTransitionTask(IN SOCltElement* element, IN BYTE targetState, IN BOOL deep);
	virtual void execute();
}; // SODaCItemStateTransitionTask


//-----------------------------------------------------------------------------
// SODaCItemTaskUpdate                                                        |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemTaskUpdate : public SOCltTaskUpdate
{
public:
	SODaCItemTaskUpdate(IN SOCltElement* element, IN BOOL from_server, IN DWORD whatParameters);
	virtual void execute();
};  // SODaCItemTaskUpdate



//-----------------------------------------------------------------------------
// SODaCItem                                                                  |
//-----------------------------------------------------------------------------

// item update what parameters
#define SODAC_ITMPAR_REQDATATYPE    0x00000001
#define SODAC_ITMPAR_EU               0x00000002
#define SODAC_ITMPAR_DEADBAND     0x00000004
#define SODAC_ITMPAR_ALL                0x00000007
#define SODAC_ITMPAR_ALL_MGT      0x00000003
#define SODAC_ITMPAR_ALL_DEADBAND 0x00000004

// state transition actions
#define SODAC_ITEM_ACTION_GET_ITEM_PARS 0x00000001
#define SODAC_ITEM_ACTION_INITIAL_READ  0x00000002

// flag for asyncDirty flag (async received data)
#define SODAC_ITEM_ASYNCDIRTY_NONE          0x00
#define SODAC_ITEM_ASYNCDIRTY_READVALUE     0x01
#define SODAC_ITEM_ASYNCDIRTY_READQUALITY   0x02
#define SODAC_ITEM_ASYNCDIRTY_READTIMESTAMP 0x04
#define SODAC_ITEM_ASYNCDIRTY_READRESULT    0x08
#define SODAC_ITEM_ASYNCDIRTY_WRITERESULT   0x10

// item not connected server handle
#define SODAS_ITEM_SERVERHANDLE_DISCONNECTED 0xFFFFABBE

class SODaCAdviseSink;
class SODaCDataCallback;
class SODaCGroup;
#ifdef SOFEATURE_DCOM
class SODaCComGroupImpl;
#endif
#ifdef SOFEATURE_SOAP
class SODaCXmlGroupImpl;
#endif
#ifdef SOFEATURE_TUNNEL
class SODaCTPCall;
class SODaCTPReceiver;
#endif

class SODAC_EXPORT SODaCItem :
	public SOCltElement,
	virtual public SODaCItemIAttr,
	virtual public SODaCItemIData
{
	friend class SODaCGroup;
	friend class SODaCAdviseSink;
	friend class SODaCDataCallback;
#ifdef SOFEATURE_DCOM
	friend class SODaCComGroupImpl;
#endif
#ifdef SOFEATURE_SOAP
	friend class SODaCXmlGroupImpl;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SODaCTPCall;
	friend class SODaCTPReceiver;
#endif

public:
	SODaCItem();

	// start the communication for all items of the group
	virtual LONG start(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	// connect group and create all items
	virtual LONG connect(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	// disconnect group and remove all items
	virtual LONG disconnect(IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	virtual LONG performStateTransition(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	// item server handle
	OPCHANDLE getServerHandle(void);

	// I/O
	HRESULT read(IN DWORD maxAge);
	HRESULT write(IN OPTIONAL SOCmnVariant* pValue = NULL, IN OPTIONAL WORD* pQuality = NULL,
				  IN OPTIONAL SOCmnDateTime* pTimeStamp = NULL);

	// item management
	virtual HRESULT update(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	BOOL getStateTransitionActive(void)
	{
		return m_stateTransitionActive;
	};
	virtual void setStateTransitionActive(BOOL st)
	{
		m_stateTransitionActive = (BYTE)st;
	};
protected:
	OPCHANDLE m_serverHandle;       // server handle of the item
	BYTE m_active;                  // activation state
	BYTE m_stateTransitionActive; // state transition currently active?

	virtual ~SODaCItem();

	virtual BOOL setServerHandle(IN OPCHANDLE serverHandle);
}; // SODaCItem

inline BOOL SODaCItem::setServerHandle(OPCHANDLE serverHandle)
{
	m_serverHandle = serverHandle;
	return TRUE;
}

inline OPCHANDLE SODaCItem::getServerHandle(void)
{
	return m_serverHandle;
}



#define ClientItem SODaCItemDefault


//-----------------------------------------------------------------------------
// SODaCItemDefault                                                           |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCItemDefault :
	public SODaCItem,
	virtual public SODaCItemAttrFull,
	virtual public SODaCItemDataFull,
	virtual public SOCltElementErrorStore
{
public:
	virtual OPCHANDLE getClientHandle(void);
protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
	virtual SOCmnObject* getObjectDaCItemIData(void);
	virtual SOCmnObject* getObjectDaCItemIAttr(void);
};

#pragma pack(pop)
#endif

