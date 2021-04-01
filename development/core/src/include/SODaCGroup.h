#ifndef _SODACGROUP_H_
#define _SODACGROUP_H_

#include "SOCltElement.h"
#include "SODaC.h"
#include "SODaCEntry.h"
#include "SODaCItem.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SODaCGroupICommunication                                                   |
//-----------------------------------------------------------------------------

#define SODAC_GRPPAR_ALL            0xFFFFFFFF

class SODaCGroup;

class SODAC_EXPORT SODaCGroupICommunication
{
	friend class SODaCGroup;
public:
	SODaCGroupICommunication(void);
	virtual ~SODaCGroupICommunication(void);

	virtual void init(SODaCServer* pServer);

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

#ifdef SOFEATURE_DCOM
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);
#endif

	virtual SODaCGroup* getObjectDaCGroup(void);

	// item validation
	virtual HRESULT validateItems(IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors = NULL);

	virtual HRESULT updateItemsImpl(IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_GRPPAR_ALL);

protected:
	virtual HRESULT doStateTransitionImpl(BOOL deep, IN BYTE stateDeep);
	virtual HRESULT doDisconnectImpl(void);
};




//-----------------------------------------------------------------------------
// SODaCGroup                                                                 |
//-----------------------------------------------------------------------------

// connection type
#define SODAC_CFG_CONNECTION_TYPE_ADDGROUP                  1
#define SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME      2

// data retrieval
#define SODAC_CFG_DATA_RETRIEVAL_ASYNCADVISE    1
#define SODAC_CFG_DATA_RETRIEVAL_SYNCREAD       2
#define SODAC_CFG_DATA_RETRIEVAL_POLLEDREFRESH  4

// group update what parameters
#define SODAC_GRPPAR_NAME           0x00000001
#define SODAC_GRPPAR_UPDATERATE     0x00000002
#define SODAC_GRPPAR_TIMEBIAS       0x00000004
#define SODAC_GRPPAR_DEADBAND       0x00000008
#define SODAC_GRPPAR_LCID           0x00000010
#define SODAC_GRPPAR_SERVERHANDLE   0x00000020
#define SODAC_GRPPAR_CLIENTHANDLE   0x00000100
#define SODAC_GRPPAR_ACTIVE         0x00000200
#define SODAC_GRPPAR_PUBLIC         0x10000000
#define SODAC_GRPPAR_KEEPALIVETIME  0x00000040
#define SODAC_GRPPAR_ALL_MGT        0x0000033F
#define SODAC_GRPPAR_ALL_MGT2   0x00000040
#define SODAC_GRPPAR_ALL_PUBMGT     0x10000000

#define SODAC_GRP_ACTION_GET_GRP_PARS 0x00000001

class SODaCItem;
#ifdef SOFEATURE_DCOM
class SODaCComGroupImpl;
class SODaCComTaskSyncRead;
class SODaCAdviseSink;
class SODaCDataCallback;
#endif
#ifdef SOFEATURE_SOAP
class SODaCXmlGroupImpl;
#endif
#ifdef SOFEATURE_TUNNEL
class SODaCTPGroupImpl;
class SODaCTPCall;
class SODaCTPReceiver;
#endif

class SODAC_EXPORT SODaCGroup :
	public SOCltElement,
	virtual public SOCmnElementNameString,
	virtual public SOCmnElementListLeaf,
	virtual public SODaCGroupICommunication
{
	friend class SODaCServer;
	friend class SODaCItem;
	friend class SODaCItemTaskUpdate;
	friend class SODaCItemStateTransitionTask;
#ifdef SOFEATURE_DCOM
	friend class SODaCComGroupImpl;
	friend class SODaCComTaskSyncRead;
	friend class SODaCAdviseSink;
	friend class SODaCDataCallback;
#endif
#ifdef SOFEATURE_SOAP
	friend class SODaCXmlGroupImpl;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SODaCTPGroupImpl;
	friend class SODaCTPCall;
	friend class SODaCTPReceiver;
#endif

public:
	// connection type
	enum connectionType
	{
		ctNone                 = 0,
		ctAddGroup             = SODAC_CFG_CONNECTION_TYPE_ADDGROUP,
		ctGetPublicGroupByName = SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME
	};

	// data retrieval
	enum dataRetrieval
	{
		drAsyncAdvise     = SODAC_CFG_DATA_RETRIEVAL_ASYNCADVISE,
		drSyncRead        = SODAC_CFG_DATA_RETRIEVAL_SYNCREAD,
		drPolledRefresh = SODAC_CFG_DATA_RETRIEVAL_POLLEDREFRESH
	};

	SODaCGroup(void);

	// object identity
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN DWORD key);
	virtual BOOL isUniqueName(void) const;

	// update rate in ms
	BOOL setReqUpdateRate(IN DWORD updateRate);
	DWORD getUpdateRate(OUT OPTIONAL DWORD* reqUpdateRate = NULL);

	// keep alive time in ms
	BOOL setReqKeepAliveTime(IN DWORD keepAliveTime);
	DWORD getKeepAliveTime(OUT OPTIONAL DWORD* reqKeepAliveTime = NULL);

	// time zone
	BOOL setTimeBias(IN LONG timeBias);
	LONG getTimeBias(void) const;

	// deadband in percent
	BOOL setDeadBand(IN FLOAT deadBand);
	FLOAT getDeadBand(void) const;

	// local Id
	BOOL setLCID(IN DWORD lcid);
	DWORD getLCID(void) const;

	// public flag
	BOOL setPublic(IN BOOL pub);
	BOOL getPublic(void) const;

	// connection type
	connectionType getConnectionType(void);
	BOOL setConnectionType(IN connectionType conType);

	// data retrieval method
	dataRetrieval getDataRetrieval(void);
	BOOL setDataRetrieval(IN dataRetrieval dataRet);

	// flags for valid parameter
	DWORD getValidParameters(void) const;
	void setValidParameters(DWORD vp);
	void resetValidParameters(void);

	// get item object
	SODaCItem* getItemByClientHandle(IN OPCHANDLE clientHandle);
	virtual SODaCItem* getItemByServerHandle(IN OPCHANDLE serverHandle);

	virtual HRESULT updateItems(IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);

	// item creation
	SODaCItem* addItem(IN LPCTSTR itemID, IN OPTIONAL VARTYPE reqDatatype = VT_EMPTY,
					   IN OPTIONAL LPCTSTR itemOrAccessPath = _T(""));

	// tree management
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	// don't check the server provided item handles
	virtual BOOL trustServerItemHandles(void);

	// synchronous I/O
	virtual HRESULT syncRead(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
							 OUT OPTIONAL SOCmnVariant* pValues = NULL, OUT OPTIONAL WORD* pQualities = NULL,
							 OUT OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncWrite(IN DWORD count, IN SODaCItem** itemArray, IN OPTIONAL SOCmnVariant* pValues = NULL,
							  IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncRead(IN OPTIONAL SOCmnList<SODaCItem> *itemList = NULL, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
							 OUT OPTIONAL SOCmnVariant* pValues = NULL, OUT OPTIONAL WORD* pQualities = NULL,
							 OUT OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncWrite(IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL SOCmnVariant* pValues = NULL,
							  IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);

	// asynchronous I/O
	virtual HRESULT asyncRead(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
							  IN OPTIONAL DWORD transactionID = 0, OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncWrite(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD transactionID = 0,
							   IN OPTIONAL SOCmnVariant* pValues = NULL, IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL,
							   OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncRefresh(IN DWORD maxAge, IN OPTIONAL DWORD transactionID = 0,
								 OUT OPTIONAL DWORD* cancelID = NULL);
	virtual HRESULT asyncCancel(IN DWORD cancelID);
	virtual HRESULT asyncRead(IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
							  IN OPTIONAL DWORD transactionID = 0, OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncWrite(IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD transactionID = 0,
							   IN OPTIONAL SOCmnVariant* pValues = NULL, IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL,
							   OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	// get child object by name
	SODaCItem* getItem(IN LPCTSTR itemID);

	// item list
	inline SOCmnList<SODaCItem> *getItemList(void)
	{
		return (SOCmnList<SODaCItem> *)getLeafList();
	}

	virtual BYTE getProtocol(void);

	// set the target state of the items of the group
	void setItemsTargetState(IN BYTE targetState);
	void setItemsState(IN BYTE state);

	virtual DWORD getCyclicTimeOut(void);

#ifdef SOFEATURE_DCOM
	// query COM interface
	virtual HRESULT queryInterface(IN REFIID riid, IN void** interf);
#endif // SOFEATURE_DCOM

	BOOL getStateTransitionActive(void)
	{
		return m_stateTransitionActive;
	};
	virtual void setStateTransitionActive(BOOL st)
	{
		m_stateTransitionActive = (BYTE)st;
	};

	virtual SODaCGroup* getObjectDaCGroup(void);

protected:
	DWORD m_reqUpdateRate;          // requested update rate in ms
	DWORD m_revUpdateRate;          // revised update rate in ms
	DWORD m_reqKeepAliveTime;   // requested keep alive time in ms
	DWORD m_revKeepAliveTime;     // revised keep alive time in ms
	LONG m_timeBias;                // time zone
	FLOAT m_deadBand;               // deadband in percent
	DWORD m_lcid;                   // local Id
	DWORD m_validParameter;         // valid parameters
	BYTE m_public;                  // public
	BYTE m_isPublic;                // is public group?
	BYTE m_connectionType;          // connection type of the group
	BYTE m_dataRetrieval;           // data retrival method of the group
	BYTE m_asyncDirty;              // asynchronous data to copy
	BYTE m_correctNTtimer;          // NT timer correction value
	BYTE m_stateTransitionActive; // state transition currently active?
	ULONG m_span;                   // time in ms untill the start of the next cyclic read
	DWORD m_initTime;               // init time of the cyclic read in ms since system start
	WORD m_initCnt;                 // init cyclic transaction counter

	virtual ~SODaCGroup(void);

	// group management
	virtual HRESULT doUpdate(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_GRPPAR_ALL_MGT);

	// do the state transitions
	virtual HRESULT doStateTransition(BOOL deep, IN BYTE stateDeep);

	// Force the object to disconnect
	virtual HRESULT doDisconnect(void);

	virtual void onReadAdvise(IN BYTE dataChangeOrRead, IN DWORD transactionId, IN HRESULT masterQuality, IN HRESULT masterError,
							  IN DWORD itemCnt, IN SODaCItem** ppItems, IN SOCmnVariant* pValues, IN WORD* pQualities,
							  IN SOCmnDateTime* pTimestamps, IN HRESULT* pErrors);
	virtual void onWriteAdvise(IN DWORD transactionId, IN HRESULT masterError, IN DWORD itemCount,
							   IN SODaCItem** ppItems, IN HRESULT* pErrors);

	// data member callbacks
	virtual void onSetReqUpdateRate(void);
	virtual void onSetRevUpdateRate(void);
	virtual void onSetTimeBias(void);
	virtual void onSetDeadBand(void);
	virtual void onSetLCID(void);
	virtual void onSetPublic(void);
	virtual void onSetReqKeepAliveTime(void);
	virtual void onSetRevKeepAliveTime(void);

	virtual SOCmnObject* getObjectCmnElementIName(void);
	virtual SOCmnObject* getObjectCmnElementIList(void);

	DWORD itemListToArray(IN SOCmnList<SODaCItem> *itemList, IN BOOL onlyStarted, OUT SODaCItem** &itemArray);
	void freeItemArray(IN DWORD count, IN SODaCItem** itemArray);
}; // SODaCGroup

inline LONG SODaCGroup::getTimeBias(void) const
{
	return m_timeBias;
}

inline FLOAT SODaCGroup::getDeadBand(void) const
{
	return m_deadBand;
}

inline DWORD SODaCGroup::getLCID(void) const
{
	return m_lcid;
}

inline BOOL SODaCGroup::getPublic(void) const
{
	return m_public;
}

inline SODaCGroup::connectionType SODaCGroup::getConnectionType(void)
{
	return (SODaCGroup::connectionType)m_connectionType;
}

inline BOOL SODaCGroup::setConnectionType(IN SODaCGroup::connectionType conType)
{
	m_connectionType = (BYTE)conType;
	return TRUE;
}

inline SODaCGroup::dataRetrieval SODaCGroup::getDataRetrieval(void)
{
	return (SODaCGroup::dataRetrieval)m_dataRetrieval;
}

inline BOOL SODaCGroup::setDataRetrieval(IN SODaCGroup::dataRetrieval dataRet)
{
	m_dataRetrieval = (BYTE)dataRet;
	return TRUE;
}

inline DWORD SODaCGroup::getValidParameters(void) const
{
	return m_validParameter;
}

inline void SODaCGroup::resetValidParameters(void)
{
	m_validParameter = 0;
}

inline void SODaCGroup::setValidParameters(DWORD vp)
{
	m_validParameter = vp;
}


#pragma pack(pop)
#endif

