#ifndef _SODMCEVENT_H_
#define _SODMCEVENT_H_

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// SODmCEvent                                                                 |
//-----------------------------------------------------------------------------

class SODmCEvent : public SOAeCEvent
{
public:
	SODmCEvent(SOAeCSubscription* subs, IN ONEVENTSTRUCT* src);

	virtual void onEventReceived(void);

protected:
	int m_listIdx;

	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCEvent

#endif

#endif
