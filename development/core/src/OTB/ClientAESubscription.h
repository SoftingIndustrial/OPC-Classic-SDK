#ifndef _CLIENTAESUBSCRIPTION_H_
#define _CLIENTAESUBSCRIPTION_H_

#include "OTClient.h"

#pragma pack(push, 4)


class ClientAESubscription : public SOAeCSubscription
{
public:
	ClientAESubscription();

	DWORD getOTCUserData(void)
	{
		return m_userData;
	}
	void setOTCUserData(DWORD ud)
	{
		m_userData = ud;
	}

	void storeTargetState(IN BYTE deep);
	void restoreTargetState(IN BYTE deep);

protected:
	DWORD m_userData;
	BYTE m_storedTargedState;

	virtual void onSetObjectState(void);

	virtual void onReceivedEvents(DWORD count, SOAeCEvent** pEvents, BYTE refresh, BYTE lastRefresh);
};


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// ClientAESubscriptionCom                                                            |
//-----------------------------------------------------------------------------

class ClientAESubscriptionCom :
	public ClientAESubscription,
	virtual public SOAeCComSubscriptionImpl
{
public:
	ClientAESubscriptionCom(void);
};

#endif // SOFEATURE_DCOM


#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// ClientAESubscriptionTp                                                            |
//-----------------------------------------------------------------------------

class ClientAESubscriptionTp :
	public ClientAESubscription,
	virtual public SOAeCTPSubscriptionImpl
{
public:
	ClientAESubscriptionTp(void);
};

#endif // SOFEATURE_TUNNEL

#pragma pack(pop)
#endif
