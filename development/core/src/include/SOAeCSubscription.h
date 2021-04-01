#ifndef _SOAECSUBSCRIPTION_H_
#define _SOAECSUBSCRIPTION_H_

#pragma pack(push, 4)



class SOAEC_EXPORT SOAeCReturnedAttrListList : public SOCmnList<SOCmnDWORDList>
{
public:
	BOOL create()
	{
		return SOCmnList<SOCmnDWORDList>::create(
				   SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_SYNCED);
	}

	void destroy()
	{
		SOCmnListPosition pos = getStartPosition();

		while (pos)
		{
			SOCmnDWORDList* list = getNext(pos);
			delete list;
		}

		SOCmnList<SOCmnDWORDList>::destroy();
	}
};


class SOAEC_EXPORT SOAeCSubscriptionICommunication
{
	friend class SOAeCSubscription;
public:
	SOAeCSubscriptionICommunication(void);
	virtual ~SOAeCSubscriptionICommunication(void);

#ifdef SOFEATURE_DCOM
	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);
#endif

	// refresh
	virtual BOOL refresh(void);
	virtual BOOL cancelRefresh(void);

	virtual SOAeCSubscription* getObjectAeCSubscription(void);

protected:
	virtual HRESULT doStateTransitionImpl(void);
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters);
	virtual HRESULT doDisconnectImpl(void);

#ifdef SOFEATURE_DCOM
	virtual void receivedEvents(IN OPCHANDLE hClientSubscription, IN BOOL bRefresh, IN BOOL bLastRefresh, IN DWORD dwCount, IN ONEVENTSTRUCT* pEvents);
#endif
};

class SOAeCServer;

typedef SOCmnDWORDList SOCmnDWORDList;

// valid flags
#define SOAEC_SUBSPAR_BUFFERTIME            0x000000001
#define SOAEC_SUBSPAR_MAXSIZE             0x000000002
#define SOAEC_SUBSPAR_RETURNED_ATTRIBUTES 0x000000010
#define SOAEC_SUBSPAR_ACTIVE              0x000000020
#define SOAEC_SUBSPAR_FILTER              0x000000040
#define SOAEC_SUBSPAR_STATE               (SOAEC_SUBSPAR_MAXSIZE | SOAEC_SUBSPAR_BUFFERTIME | SOAEC_SUBSPAR_ACTIVE)


#ifdef SOFEATURE_DCOM
class SOAeCComSubscriptionImpl;
class SOAeCEventSink;
#endif
#ifdef SOFEATURE_TUNNEL
class SOAeCTPSubscriptionImpl;
class SOAeCTPCall;
class SOAeCTPReceiver;
#endif

//-----------------------------------------------------------------------------
// SOAeCSubscription                                                          |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCSubscription :
	virtual public SOAeCSubscriptionICommunication,
public SOCltElement
{
#ifdef SOFEATURE_DCOM
	friend class SOAeCComSubscriptionImpl;
	friend class SOAeCEventSink;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SOAeCTPSubscriptionImpl;
	friend class SOAeCTPCall;
	friend class SOAeCTPReceiver;
#endif

public:
	SOAeCSubscription();

	// object identity
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN DWORD key);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	SOAeCServer* getServer(void);

#ifdef SOFEATURE_DCOM
	// query COM interface
	virtual HRESULT queryInterface(IN REFIID riid, OUT void** interf);
#endif

	SOCmnStringList* getSourceList(void);
	SOCmnStringList* getAreaList(void);
	SOCmnDWORDList* getCategoryList(void);
	SOCmnDWORDList* getReturnedAttributes(IN DWORD category);

	BOOL setBufferTime(IN DWORD bufferTime);
	BOOL setMaxSize(IN DWORD maxSize);
	BOOL setLowSeverity(IN DWORD lowSeverity);
	BOOL setHighSeverity(IN DWORD highSeverity);
	BOOL setEventTypes(IN DWORD eventTypes);
	BOOL setActive(IN BOOL active);

	DWORD getBufferTime(void) const;
	DWORD getMaxSize(void) const;
	DWORD getLowSeverity(void) const;
	DWORD getHighSeverity(void) const;
	DWORD getEventTypes(void) const;
	BOOL    getActive(void) const;
	DWORD getRevisedBufferTime(void) const;
	DWORD getRevisedMaxSize(void) const;

	SOAeCReturnedAttrListList* getReturnedAttrListList(void)
	{
		return &m_retAttrList;
	}

	virtual SOAeCSubscription* getObjectAeCSubscription(void);

	virtual HRESULT doUpdate(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters);

	virtual BYTE getProtocol(void);

protected:
	virtual void onSetBufferTime(void);
	virtual void onSetMaxSize(void);
	virtual void onSetLowSeverity(void);
	virtual void onSetHighSeverity(void);
	virtual void onSetEventTypes(void);
	virtual void onSetActive(void);
	virtual void onSetRevisedBufferTime(void);
	virtual void onSetRevisedMaxSize(void);

	virtual void onReceivedEvents(DWORD count, SOAeCEvent** pEvents, BYTE refresh, BYTE lastRefresh);

	DWORD m_dwBufferTime;                     // Buffer time
	DWORD m_dwMaxSize;                          // Max time
	DWORD m_dwLowSeverity;                          // Low severity, 0...1000
	DWORD m_dwHighSeverity;                         // High severity, 0...1000
	DWORD m_dwEventTypes;                               // Event types. See 'opcaedef.h' for the list of valid types.

	SOCmnDWORDList m_categoryList;          // List of event categories of interest. An empty list
	// indicates all categories should be included in the filter.
	SOCmnStringList m_areaList;                 // List of process area of interest. An empty list
	// indicates all areas should be included in the filter.
	SOCmnStringList m_sourceList;               // List of event sources. An empty list indicates all
	// sourcesshould be included in the filter.

	DWORD m_dwRevisedBufferTime;        // Revised buffer time
	DWORD m_dwRevisedMaxSize;           // Revised max size
	DWORD m_dwAvailableFilters;         // available filters
	BOOL m_bActive;                     // active

	// List with keys of returned attributes for a category
	// Key is CategoryID.

	SOAeCReturnedAttrListList m_retAttrList;

	virtual HRESULT doStateTransition(BOOL deep, BYTE stateDeep);
	virtual HRESULT doDisconnect(void);


	virtual ~SOAeCSubscription();
};

inline SOAeCServer* SOAeCSubscription::getServer(void)
{
	SOCmnElement* p = getParentBranch();
	_ASSERTION(SOCMNOBJECT_TYPE_SERVER_AE  == (p->getObjectType() & SOCMNOBJECT_TYPE_SERVER_AE), "Parent isn't an AE Server object");
	return (SOAeCServer*)p;
}

inline SOCmnStringList* SOAeCSubscription::getSourceList(void)
{
	return &m_sourceList;
}

inline SOCmnStringList* SOAeCSubscription::getAreaList(void)
{
	return &m_areaList;
}

inline SOCmnDWORDList* SOAeCSubscription::getCategoryList(void)
{
	return &m_categoryList;
}

inline DWORD SOAeCSubscription::getBufferTime(void)  const
{
	return m_dwBufferTime;
}

inline DWORD SOAeCSubscription::getMaxSize(void) const
{
	return m_dwMaxSize;
}

inline DWORD SOAeCSubscription::getLowSeverity(void) const
{
	return m_dwLowSeverity;
}

inline DWORD SOAeCSubscription::getHighSeverity(void) const
{
	return m_dwHighSeverity;
}

inline DWORD SOAeCSubscription::getEventTypes(void) const
{
	return m_dwEventTypes;
}

inline BOOL SOAeCSubscription::getActive(void) const
{
	return m_bActive;
}

inline DWORD SOAeCSubscription::getRevisedBufferTime(void) const
{
	return m_dwRevisedBufferTime;
}

inline DWORD SOAeCSubscription::getRevisedMaxSize(void) const
{
	return m_dwRevisedMaxSize;
}

#pragma pack(pop)
#endif // _SOAECSUBSCRIPTION_H_
