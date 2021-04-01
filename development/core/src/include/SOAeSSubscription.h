#ifndef _SOAESSUBSCRIPTION_H_
#define _SOAESSUBSCRIPTION_H_

#include <opc_ae.h>
#include "SOCmnElement.h"
#include "SOAeSCategory.h"
#include "SOAeSAttribute.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeSSubscriptionReturnedAttributes                                        |
//-----------------------------------------------------------------------------

class SOAeSSubscription;

class SOAeSSubscriptionReturnedAttributes
{
	friend SOAeSSubscription;
protected:
	SOAeSSubscriptionReturnedAttributes();
	virtual ~SOAeSSubscriptionReturnedAttributes();

	SOCmnPointer<SOAeSCategory> m_category;
	SOCmnList<SOAeSAttribute> m_attributeList;
}; // SOAeSSubscriptionReturnedAttributes




//-----------------------------------------------------------------------------
// SOAeSSubscriptionIFilter                                                   |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSSubscriptionIFilter
{
public:
	virtual HRESULT setFilter(IN DWORD eventType, IN SOCmnDWORDList* eventCategoryList,
							  IN DWORD lowSeverity, IN DWORD highSeverity,
							  IN SOCmnStringList* areaList, IN SOCmnStringList* sourceList);

	virtual HRESULT getFilter(OUT DWORD* eventType, OUT SOCmnDWORDList* eventCategoryList,
							  OUT DWORD* lowSeverity, OUT DWORD* highSeverity,
							  OUT SOCmnStringList* areaList, OUT SOCmnStringList* sourceList);

	virtual BOOL filterEvent(IN SOAeSEvent* event);

protected:
	SOAeSSubscriptionIFilter();
	virtual ~SOAeSSubscriptionIFilter();

	// source wildcard filter
	virtual BOOL matchStringFilter(IN LPCTSTR string, IN LPCTSTR filter);

	virtual void onSetFilter(IN DWORD eventType, IN SOCmnDWORDList* eventCategoryList,
							 IN DWORD lowSeverity, IN DWORD highSeverity,
							 IN SOCmnStringList* areaList, IN SOCmnStringList* sourceList);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectAeSSubscriptionIFilter(void);
}; // SOAeSSubscriptionIFilter


//-----------------------------------------------------------------------------
// SOAeSSubscriptionFilterFull                                                |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSSubscriptionFilterFull : virtual public SOAeSSubscriptionIFilter
{
public:
	virtual HRESULT setFilter(IN DWORD eventType, IN SOCmnDWORDList* eventCategoryList,
							  IN DWORD lowSeverity, IN DWORD highSeverity,
							  IN SOCmnStringList* areaList, IN SOCmnStringList* sourceList);

	virtual HRESULT getFilter(OUT DWORD* eventType, OUT SOCmnDWORDList* eventCategoryList,
							  OUT DWORD* lowSeverity, OUT DWORD* highSeverity,
							  OUT SOCmnStringList* areaList, OUT SOCmnStringList* sourceList);

	virtual BOOL filterEvent(IN SOAeSEvent* event);

protected:
	SOAeSSubscriptionFilterFull();

	DWORD m_eventType;              // event types
	DWORD m_lowSeverity;            // low severity
	DWORD m_highSeverity;           // high severity
	SOCmnDWORDList m_categoryList;  // event categories list
	SOCmnStringList m_areaList;     // area list
	SOCmnStringList m_sourceList;   // source list
	SOCmnSync m_sync;               // filter synchronisation object
}; // SOAeSSubscriptionFilterFull




//-----------------------------------------------------------------------------
// SOAeSRefresh                                                               |
//-----------------------------------------------------------------------------

class SOAeSEvent;

class SOAES_EXPORT SOAeSRefresh :
	public SOCmnObject
{
public:
	// refresh state
	enum refreshState
	{
		created = 0x01,
		aggregated = 0x02,
		collected = 0x04,
		send = 0x10,
		canceled = 0x80
	};

	SOAeSRefresh(IN SOAeSSubscription* subscription, IN DWORD connection);

	// cancel the refresh
	virtual HRESULT cancel(void);

	// collected all not object based events
	void endCollection(void);

	// advise connection to refresh
	DWORD getConnection(void) const;

	// refresh event list
	SOCmnList<SOAeSEvent> *getEventList(void);

protected:
	SOCmnList<SOAeSEvent> m_eventList;
	SOCmnPointer<SOAeSSubscription> m_subscription;
	DWORD m_connection;
}; // SOAeSRefresh

inline DWORD SOAeSRefresh::getConnection(void) const
{
	return m_connection;
}

inline SOCmnList<SOAeSEvent> *SOAeSRefresh::getEventList(void)
{
	return &m_eventList;
}

inline void SOAeSRefresh::endCollection(void)
{
	setObjectState((BYTE)(getObjectState() | (BYTE)SOAeSRefresh::collected));
}




//-----------------------------------------------------------------------------
// SOAeSSubscription                                                          |
//-----------------------------------------------------------------------------

#define SOAESSUBSCRIPTION_FLAG_FORCE_ASYNC_SEND_EVENTS  0x01

#ifdef SOFEATURE_DCOM
class SOAeSComSubscription;
#endif
#ifdef SOFEATURE_TUNNEL
class SOAeSTPSubscription;
#endif
class SOAeSEvent;
class SOAeSServer;

class SOAES_EXPORT SOAeSSubscription :
	public SOCmnElement,
	virtual public SOAeSSubscriptionIFilter
{
#ifdef SOFEATURE_DCOM
	friend SOAeSComSubscription;
#endif
#ifdef SOFEATURE_TUNNEL
	friend SOAeSTPSubscription;
#endif
	friend SOAeSServer;

public:
	SOAeSSubscription(void);

	virtual LPCTSTR getObjTraceId(void);

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// activation state
	virtual BOOL setActive(IN BOOL active);
	BOOL getActive(void) const;

	// buffer time
	virtual BOOL setBufferTime(IN DWORD bufferTime);
	DWORD getBufferTime(void) const;

	// maximum number of events in a notification
	virtual BOOL setMaxSize(IN DWORD maxSize);
	DWORD getMaxSize(void) const;

	// client handle from OPC client
	virtual BOOL setClientHandle(IN OPCHANDLE clientHandle);
	OPCHANDLE getClientHandle(void) const;

	// returned attributes
	virtual BOOL selectReturnedAttributes(IN DWORD category, IN DWORD attrCount,
										  IN DWORD* attrArray);
	SOCmnList<SOAeSAttribute> *getReturnedAttributeList(IN DWORD category);

	// events to send
	SOCmnList<SOAeSEvent> *getSendEventList(void);

	// IConnectionPoint method
	BOOL createConnectedEventSinkInterfaceList(OUT SOCmnList<IOPCEventSink> *eventSinkList);

	// filter event and send it to the clients
	virtual BOOL fireEvent(IN SOAeSEvent* event);
	virtual BOOL fireEvents(IN SOCmnList<SOAeSEvent> *eventList);

	// filter event and add it to the event list of the subscription
	virtual BOOL addEvent(IN SOAeSEvent* event);

	// send all events of the event list
	virtual void sendEvents(void);

	// refresh
	SOAeSRefresh* getRefresh(void);
	BOOL isRefreshInProgress(void);

	// COM object handling
	void* getMe(void);
	ULONG releaseMe(IN void* me);

	// flags
	BYTE getFlags(void) const;
	void setFlags(IN BYTE flags);

#ifdef SOFEATURE_DCOM
	// check interface access
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#endif

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
	virtual LPCTSTR returnedAttribues2string(IN SOCmnString& buffer);
#endif

protected:
	virtual ~SOAeSSubscription();

	void* m_me;                 // COM subscription object
	SOCmnSync m_meSync;         // synchronisation of COM subscription object setting
	DWORD m_bufferTime;         // buffer time
	DWORD m_maxSize;            // maximum number of events in a notification
	OPCHANDLE m_clientHandle;   // client handle
	SOCmnList<SOAeSSubscriptionReturnedAttributes> m_retAttrList;
	// list of list of returned attributes
	SOCmnList<SOAeSEvent> m_sendEventList;
	// list of events which must be send to the client
	DWORD m_sendTime;           // can send events? handles restirction of buffer time
	SOCmnPointer<SOAeSRefresh> m_refresh;
	// current refresh operation
	BYTE m_active;              // activation state
	BYTE m_flags;               // flags

	// callbacks
	virtual void onSetActive(IN BOOL active);
	virtual void onSetBufferTime(IN DWORD bufferTime);
	virtual void onSetMaxSize(IN DWORD maxSize);
	virtual void onSetClientHandle(IN OPCHANDLE clientHandle);
	virtual void onSelectReturnedAttributes(IN SOAeSCategory* category,
											IN SOCmnList<SOAeSAttribute> *attributeList);

	// send events of the event list to the client
	virtual DWORD doSendEvents(IN BOOL refresh);
	virtual DWORD checkSendTime(IN DWORD now);
	BOOL fillEventStruct(IN SOAeSEvent* event, IN ONEVENTSTRUCT* eventStruct);
	void freeEventStructArray(IN DWORD numEvents, IN ONEVENTSTRUCT* eventData);

	// refresh handling
	virtual SOAeSRefresh* createRefresh(IN DWORD connection);
	virtual HRESULT cancelRefresh(IN DWORD connection);
	virtual void releaseRefresh(void);

	// query interface
	virtual HRESULT queryInterface(REFIID iid, void** object);

#ifdef SOFEATURE_COMIF_HOOKS
	// interface function callbacks
	virtual HRESULT onBSetFilter(IN DWORD eventType, IN DWORD numCategories, IN DWORD* eventCategoryArray, IN DWORD lowSeverity, IN DWORD highSeverity, IN DWORD numAreas, IN LPWSTR* areaArray, IN DWORD numSources, IN LPWSTR* sourceArray);
	virtual HRESULT onBGetFilter(OUT DWORD* eventType, OUT DWORD* numCategories, OUT DWORD** eventCategoryArray, OUT DWORD* lowSeverity, OUT DWORD* highSeverity, OUT DWORD* numAreas, OUT LPWSTR** areaArray, OUT DWORD* numSources, OUT LPWSTR** sourceArray);
	virtual HRESULT onBSelectReturnedAttributes(IN DWORD eventCategory, IN DWORD count, IN DWORD* attributeIDs);
	virtual HRESULT onBGetReturnedAttributes(IN DWORD eventCategory, OUT DWORD* count, OUT DWORD** attributeIDs);
	virtual HRESULT onBRefresh(IN DWORD connection);
	virtual HRESULT onBCancelRefresh(IN DWORD connection);
	virtual HRESULT onBGetState(OUT BOOL* active, OUT DWORD* bufferTime, OUT DWORD* maxSize, OUT OPCHANDLE* clientSubscription);
	virtual HRESULT onBSetState(IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE clientSubscription, OUT DWORD* revisedBufferTime, OUT DWORD* revisedMaxSize);
	virtual HRESULT onBAdvise(IN LPUNKNOWN unknown, OUT DWORD* cookie);
	virtual HRESULT onBUnadvise(IN DWORD cookie);
	virtual void onESetFilter(IN HRESULT res, IN DWORD eventType, IN DWORD numCategories, IN DWORD* eventCategoryArray, IN DWORD lowSeverity, IN DWORD highSeverity, IN DWORD numAreas, IN LPWSTR* areaArray, IN DWORD numSources, IN LPWSTR* sourceArray);
	virtual void onEGetFilter(IN HRESULT res, IN DWORD* eventType, IN DWORD* numCategories, IN DWORD** eventCategoryArray, IN DWORD* lowSeverity, IN DWORD* highSeverity, IN DWORD* numAreas, IN LPWSTR** areaArray, IN DWORD* numSources, IN LPWSTR** sourceArray);
	virtual void onESelectReturnedAttributes(IN HRESULT res, IN DWORD eventCategory, IN DWORD count, IN DWORD* attributeIDs);
	virtual void onEGetReturnedAttributes(IN HRESULT res, IN DWORD eventCategory, IN DWORD* count, IN DWORD** attributeIDs);
	virtual void onERefresh(IN HRESULT res, IN DWORD connection);
	virtual void onECancelRefresh(IN HRESULT res, IN DWORD connection);
	virtual void onEGetState(IN HRESULT res, IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE* clientSubscription);
	virtual void onESetState(IN HRESULT res, IN BOOL* active, IN DWORD* bufferTime, IN DWORD* maxSize, IN OPCHANDLE clientSubscription, IN DWORD* revisedBufferTime, IN DWORD* revisedMaxSize);
	virtual void onEAdvise(IN HRESULT res, IN LPUNKNOWN unknown, IN DWORD* cookie);
	virtual void onEUnadvise(IN HRESULT res, IN DWORD cookie);
#endif // SOFEATURE_COMIF_HOOKS
}; // SOAeSSubscription

inline BOOL SOAeSSubscription::getActive(void) const
{
	return m_active;
}

inline DWORD SOAeSSubscription::getBufferTime(void) const
{
	return m_bufferTime;
}

inline DWORD SOAeSSubscription::getMaxSize(void) const
{
	return m_maxSize;
}

inline OPCHANDLE SOAeSSubscription::getClientHandle(void) const
{
	return m_clientHandle;
}

inline SOCmnList<SOAeSEvent> *SOAeSSubscription::getSendEventList(void)
{
	return &m_sendEventList;
}

inline BOOL SOAeSSubscription::isRefreshInProgress(void)
{
	return ((SOAeSRefresh*)m_refresh != NULL);
}

inline SOAeSRefresh* SOAeSSubscription::getRefresh(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_refresh.addRef();
	return ((SOAeSRefresh*)m_refresh);
}

inline void SOAeSSubscription::releaseRefresh(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_refresh.release();
}

inline BYTE SOAeSSubscription::getFlags(void) const
{
	return m_flags;
}

inline void SOAeSSubscription::setFlags(IN BYTE flags)
{
	m_flags = flags;
}




//-----------------------------------------------------------------------------
// SOAeSSubscriptionDefault                                                   |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSSubscriptionDefault :
	public SOAeSSubscription,
	virtual public SOAeSSubscriptionFilterFull
{
protected:
	virtual SOCmnObject* getObjectAeSSubscriptionIFilter(void);
}; // SOAeSSubscriptionDefault


#pragma pack(pop)
#endif
