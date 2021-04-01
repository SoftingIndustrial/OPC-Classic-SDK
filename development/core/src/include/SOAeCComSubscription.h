#ifndef _SOAECCOMSUBSCRIPTION_H_
#define _SOAECCOMSUBSCRIPTION_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeCComSubscriptionImpl                                                   |
//-----------------------------------------------------------------------------

class SOAeCSubscription;

class SOAEC_EXPORT SOAeCComSubscriptionImpl : virtual public SOAeCSubscriptionICommunication
{
	friend class SOAeCSubscription;
public:
	SOAeCComSubscriptionImpl();

	// refresh
	virtual BOOL refresh(void);
	virtual BOOL cancelRefresh(void);

	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);

protected:
	SOAeCEventSink* m_sink;
	SOCltInterfaceAccess<IOPCEventSubscriptionMgt, &_ATL_IIDOF(IOPCEventSubscriptionMgt)> m_submgt;

	virtual ~SOAeCComSubscriptionImpl();

	virtual HRESULT doStateTransitionImpl(void);
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters);
	virtual HRESULT doDisconnectImpl(void);

	HRESULT doUpdateFilter(IN BOOL fromServer);
	HRESULT doUpdateReturnedAttributes(IN BOOL fromServer);
	HRESULT doUpdateSubscriptionState(IN BOOL fromServer,   IN DWORD whatParameters);

	virtual void receivedEvents(IN OPCHANDLE hClientSubscription, IN BOOL bRefresh, IN BOOL bLastRefresh, IN DWORD dwCount, IN ONEVENTSTRUCT* pEvents);
};



//-----------------------------------------------------------------------------
// SOAeCComSubscriptionDefault                                                |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCComSubscriptionDefault :
	public SOAeCSubscription,
	virtual public SOAeCComSubscriptionImpl,
	virtual public SOCltElementErrorStore
{
public:
	SOAeCComSubscriptionDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)
#endif // _SOAECSUBSCRIPTION_H_
