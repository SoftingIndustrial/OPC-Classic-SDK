#ifndef _SOAECEVENT_H_
#define _SOAECEVENT_H_

#pragma pack(push, 4)


class SOAeCEvent;

//-----------------------------------------------------------------------------
// SOAeCTaskAckEvent                                                          |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTaskAckEvent : public SOCltTask
{
public:
	SOAeCTaskAckEvent(SOAeCServer* pServer, SOAeCEvent* pEvent);
	SOAeCTaskAckEvent(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events);
	virtual void execute(void);

	void executeSingle(void);
	void executeMultiple(void);

	SOCmnString m_ackID;
	SOCmnString m_comment;
	SOCmnList<SOAeCEvent> m_listEvents;
	SOCmnPointer<SOAeCEvent> m_eventAE;

	virtual ~SOAeCTaskAckEvent();
};

//-----------------------------------------------------------------------------
// SOAeCEvent                                                                 |
//-----------------------------------------------------------------------------

class SOAeCCreator;
class SOAeCSubscription;
#ifdef SOFEATURE_TUNNEL
class SOAeCTPReceiver;
#endif

class SOAEC_EXPORT SOAeCEvent : public SOCltElement
{
	friend class SOAeCCreator;
	friend class SOAeCSubscription;
#ifdef SOFEATURE_TUNNEL
	friend class SOAeCTPReceiver;
#endif

public:
	SOAeCEvent(void);
	SOAeCEvent(SOAeCSubscription* subs, IN ONEVENTSTRUCT* pData);

	SOCltWorkerThread* getWorkerThread(void);

	virtual const SOAeCEvent& operator=(IN const ONEVENTSTRUCT& src);
	BOOL isSimple(void) const;
	BOOL isTracking(void) const;
	BOOL isConditional(void) const;
	BOOL ackRequired(void) const;

	BOOL ackCondition(IN OPTIONAL LPCTSTR acknowledgerID = NULL,
					  IN OPTIONAL LPCTSTR comment = NULL,
					  IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC,
					  IN OPTIONAL SOCmnEventPointer handle = NULL);

	HRESULT getAcknowledgmentResult(void) const;

	// callbacks
	virtual void onEventReceived(void);
	virtual void onEventAcknowledged(void);

	// valid for all event types
	SOCmnString m_source;
	FILETIME m_ftTime;
	SOCmnString m_message;
	DWORD m_eventType;
	DWORD m_eventCategory;
	DWORD m_severity;
	DWORD m_dwNumEventAttrs;
	SOCmnList<SOCmnVariant> m_listEventAttrs;

	// valid conditional events only
	WORD m_changeMask;
	WORD m_newState;
	SOCmnString  m_conditionName;
	SOCmnString  m_subconditionName;
	WORD m_quality;
	WORD m_wReserved;
	BOOL m_bAckRequired;
	FILETIME m_ftActiveTime;
	DWORD m_dwCookie;

	// Tracking events & acknowledgment notifications
	SOCmnString  m_actorID;

	// Misc
	HRESULT m_ackRes;                   // acknowledge result
	SOCmnPointer<SOAeCSubscription> m_subs;

	virtual void report();

protected:
	void initialize(void);
	void copyData(IN const ONEVENTSTRUCT& src);
	virtual ~SOAeCEvent();
};

inline const SOAeCEvent& SOAeCEvent::operator=(IN const ONEVENTSTRUCT& src)
{
	copyData(src);
	return *this;
}

inline BOOL SOAeCEvent::isSimple(void) const
{
	return OPC_SIMPLE_EVENT == m_eventType ;
}

inline BOOL SOAeCEvent::isTracking(void) const
{
	return OPC_TRACKING_EVENT == m_eventType ;
}

inline BOOL SOAeCEvent::isConditional(void) const
{
	return OPC_CONDITION_EVENT == m_eventType ;
}

inline BOOL SOAeCEvent::ackRequired(void) const
{
	return isConditional() && m_bAckRequired;
}

inline HRESULT SOAeCEvent::getAcknowledgmentResult(void) const
{
	return m_ackRes;
}

#pragma pack(pop)

#endif // _SOAEEVENT_H_
