#ifndef _SOAECTPSUBSCRIPTION_H_
#define _SOAECTPSUBSCRIPTION_H_

#ifdef SOFEATURE_TUNNEL
#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOAeCTPSubscriptionImpl                                                                |
//-----------------------------------------------------------------------------

class SOAeCSubscription;

class SOAEC_EXPORT SOAeCTPSubscriptionImpl : virtual public SOAeCSubscriptionICommunication
{
	friend class SOAeCSubscription;
public:
	SOAeCTPSubscriptionImpl();

	DWORD getServerHandle(void)
	{
		return m_serverHandle;
	}

	// refresh
	virtual BOOL refresh(void);
	virtual BOOL cancelRefresh(void);

protected:
	DWORD m_serverHandle;

	virtual ~SOAeCTPSubscriptionImpl();

	// do the state transitions
	virtual HRESULT doDisconnectImpl(void);
	virtual HRESULT doStateTransitionImpl(void);
	virtual HRESULT doUpdateImpl(IN BOOL fromSubscription, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

	HRESULT doUpdateReturnedAttributes(IN BOOL fromServer);

};

//-----------------------------------------------------------------------------
// SOAeCTPSubscriptionDefault                                                         |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPSubscriptionDefault :
	public SOAeCSubscription,
	virtual public SOAeCTPSubscriptionImpl,
	virtual public SOCltElementErrorStore
{
public:
	SOAeCTPSubscriptionDefault();

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)
#endif  //  SOFEATURE_TUNNEL

#endif
