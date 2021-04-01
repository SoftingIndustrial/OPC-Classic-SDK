#ifndef _SOAESEVENT_H_
#define _SOAESEVENT_H_

#pragma pack(push, 4)


class SOAeSAttributeValue;
class SOAeSCategory;

//-----------------------------------------------------------------------------
// SOAeSEventIAttr                                                            |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventIAttr
{
public:
	SOAeSEventIAttr(void);

	// -- simple, tracking and conditional event attributes --

	// source name
	virtual BOOL setSourceName(IN LPCTSTR sourceName);
	virtual SOCmnString getSourceName(void);

	// occurrence time
	virtual BOOL setOccurrenceTime(IN LPFILETIME time);
	virtual BOOL getOccurrenceTime(OUT LPFILETIME time);

	// message
	virtual BOOL setMessage(IN LPCTSTR message);
	virtual SOCmnString getMessage(void);

	// category
	virtual BOOL setCategory(IN SOAeSCategory* category);
	virtual BOOL setCategoryByID(IN DWORD categoryID);
	virtual SOAeSCategory* getCategory(void);

	// severity
	virtual BOOL setSeverity(IN DWORD severity);
	virtual DWORD getSeverity(void);

	// attribute values
	virtual SOCmnList<SOAeSAttributeValue> *getAttributeValueList(void);

	// -- tracking and conditional event attributes --

	// actor id
	virtual BOOL setActorID(IN LPCTSTR actorID);
	virtual SOCmnString getActorID(void);

	// -- conditional event attributes --

	// condition name
	virtual BOOL setConditionName(IN LPCTSTR conditionName);
	virtual SOCmnString getConditionName(void);

	// subcondition name
	virtual BOOL setSubConditionName(IN LPCTSTR subConditionName);
	virtual SOCmnString getSubConditionName(void);

	// change mask
	virtual BOOL setChangeMask(IN WORD changeMask);
	virtual WORD getChangeMask(void);

	// new state
	virtual BOOL setNewState(IN WORD newState);
	virtual WORD getNewState(void);

	// quality
	virtual BOOL setQuality(IN WORD quality);
	virtual WORD getQuality(void);

	// ack required
	virtual BOOL setAckRequired(IN BOOL ackRequired);
	virtual BOOL getAckRequired(void);

	// active time
	virtual BOOL setActiveTime(IN LPFILETIME time);
	virtual BOOL getActiveTime(OUT LPFILETIME time);

	// cookie
	virtual BOOL setCookie(IN DWORD cookie);
	virtual DWORD getCookie(void);

protected:
	virtual ~SOAeSEventIAttr(void);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectAeSEventIAttr(void);
}; // SOAeSEventIAttr


//-----------------------------------------------------------------------------
// SOAeSEventAttrSimple                                                       |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventAttrSimple : virtual public SOAeSEventIAttr
{
public:
	SOAeSEventAttrSimple(void);

	// source name
	virtual BOOL setSourceName(IN LPCTSTR sourceName);
	virtual SOCmnString getSourceName(void);

	// occurrence time
	virtual BOOL setOccurrenceTime(IN LPFILETIME time);
	virtual BOOL getOccurrenceTime(OUT LPFILETIME time);

	// message
	virtual BOOL setMessage(IN LPCTSTR message);
	virtual SOCmnString getMessage(void);

	// category
	virtual BOOL setCategory(IN SOAeSCategory* category);
	virtual BOOL setCategoryByID(IN DWORD categoryID);
	virtual SOAeSCategory* getCategory(void);

	// severity
	virtual BOOL setSeverity(IN DWORD severity);
	virtual DWORD getSeverity(void);

	// attribute values
	virtual SOCmnList<SOAeSAttributeValue> *getAttributeValueList(void);

protected:
	FILETIME m_occurenceTime;
	SOCmnString m_sourceName;
	SOCmnString m_message;
	SOCmnPointer<SOAeSCategory> m_category;
	DWORD m_severity;
	SOCmnList<SOAeSAttributeValue> m_attrValList;

	~SOAeSEventAttrSimple(void);
}; // SOAeSEventAttrSimple


//-----------------------------------------------------------------------------
// SOAeSEventAttrTracking                                                     |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventAttrTracking : public SOAeSEventAttrSimple
{
public:
	SOAeSEventAttrTracking(void);
	// -- tracking and conditional event attributes --

	// actor id
	virtual BOOL setActorID(IN LPCTSTR actorID);
	virtual SOCmnString getActorID(void);

protected:
	SOCmnString m_actorID;

	~SOAeSEventAttrTracking(void);
}; // SOAeSEventAttrTracking



//-----------------------------------------------------------------------------
// SOAeSEventAttrCondition                                                    |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventAttrCondition : public SOAeSEventAttrTracking
{
public:
	SOAeSEventAttrCondition(void);

	// -- conditional event attributes --

	// condition name
	virtual BOOL setConditionName(IN LPCTSTR conditionName);
	virtual SOCmnString getConditionName(void);

	// subcondition name
	virtual BOOL setSubConditionName(IN LPCTSTR subConditionName);
	virtual SOCmnString getSubConditionName(void);

	// change mask
	virtual BOOL setChangeMask(IN WORD changeMask);
	virtual WORD getChangeMask(void);

	// new state
	virtual BOOL setNewState(IN WORD newState);
	virtual WORD getNewState(void);

	// quality
	virtual BOOL setQuality(IN WORD quality);
	virtual WORD getQuality(void);

	// ack required
	virtual BOOL setAckRequired(IN BOOL ackRequired);
	virtual BOOL getAckRequired(void);

	// active time
	virtual BOOL setActiveTime(IN LPFILETIME time);
	virtual BOOL getActiveTime(OUT LPFILETIME time);

	// cookie
	virtual BOOL setCookie(IN DWORD cookie);
	virtual DWORD getCookie(void);

protected:
	FILETIME m_activeTime;
	SOCmnString m_conditionName;
	SOCmnString m_subConditionName;
	DWORD m_cookie;
	WORD m_changeMask;
	WORD m_newState;
	WORD m_quality;
	BYTE m_ackRequired;

	~SOAeSEventAttrCondition(void);
}; // SOAeSEventAttrCondition




//-----------------------------------------------------------------------------
// SOAeSEvent                                                                 |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEvent :
	public SOCmnObject,
	virtual public SOAeSEventIAttr
{
public:
	SOAeSEvent(void);

	// fire this event
	virtual BOOL fire(IN OPTIONAL BYTE serverInstance = 0);

	// add attribute value to the list of the event
	virtual BOOL addAttributeValue(IN DWORD attributeID, IN LPVARIANT value);

	// fire an event list
	static BOOL fireEvents(IN SOCmnList<SOAeSEvent> *eventList,
						   IN OPTIONAL BYTE serverInstance = 0);

	// creation helpers
	static SOAeSEvent* createSimpleEvent(IN DWORD categoryID,
										 IN LPCTSTR sourceName, IN DWORD severity, IN LPCTSTR message,
										 OPTIONAL IN LPFILETIME ocTime = NULL);
	static SOAeSEvent* createTrackingEvent(IN DWORD categoryID,
										   IN LPCTSTR sourceName, IN DWORD severity, IN LPCTSTR message,
										   IN LPCTSTR actorID, OPTIONAL IN LPFILETIME ocTime = NULL);

}; // SOAeSEvent




//-----------------------------------------------------------------------------
// SOAeSEventSimpleDefault                                                    |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventSimpleDefault :
	public SOAeSEvent,
	virtual public SOAeSEventAttrSimple
{
protected:
	virtual SOCmnObject* getObjectAeSEventIAttr(void);
}; // SOAeSEventSimpleDefault


//-----------------------------------------------------------------------------
// SOAeSEventTrackingDefault                                                  |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventTrackingDefault :
	public SOAeSEvent,
	virtual public SOAeSEventAttrTracking
{
protected:
	virtual SOCmnObject* getObjectAeSEventIAttr(void);
}; // SOAeSEventTrackingDefault


//-----------------------------------------------------------------------------
// SOAeSEventConditionDefault                                                 |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSEventConditionDefault :
	public SOAeSEvent,
	virtual public SOAeSEventAttrCondition
{
protected:
	virtual SOCmnObject* getObjectAeSEventIAttr(void);
}; // SOAeSEventConditionDefault

#pragma pack(pop)
#endif
