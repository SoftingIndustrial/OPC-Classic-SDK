#ifndef _SOXPSAEOBJECTS_H_
#define _SOXPSAEOBJECTS_H_

#include "SOAeSCondition.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif



//-----------------------------------------------------------------------------
// SOXpSAEServer                                                              |
//-----------------------------------------------------------------------------

class SOXpSAEServer :
	public SOAeSServer
{
public:
	SOXpSAEServer(void);

protected:
	virtual BOOL supportEnableCondition(IN BOOL enable, IN BOOL areaOrSource);

	// startup synchronisation
	virtual HRESULT waitForNameSpaceStartup(void);
	virtual HRESULT waitForIOStartup(void);
}; // SOXpSAEServer



//-----------------------------------------------------------------------------
// SOXpSSubscription                                                          |
//-----------------------------------------------------------------------------

class SOXpSSubscription :
	public SOAeSSubscriptionDefault
{
public:
	virtual BOOL setActive(IN BOOL active);

protected:
	~SOXpSSubscription(void);
}; // SOXpSAESubscription



//-----------------------------------------------------------------------------
// SOXpSConditionClassSingleState                                             |
//-----------------------------------------------------------------------------

class SOXpSConditionClassSingleState :
	public SOAeSConditionClassSingleStateDefault
{
public:
	SOXpSConditionClassSingleState(void);

	// inactive subcondition
	virtual SOCmnString getInactiveMessage(void);
	BOOL setInactiveMessage(IN LPCTSTR message);

	virtual BOOL getInactiveAckRequired(void);
	BOOL setInactiveAckRequired(IN BOOL ackReq);

	// active single state subcondition
	virtual BOOL getActiveAckRequired(void);
	BOOL setActiveAckRequired(IN BOOL ackReq);

protected:
	BOOL m_activeAckReq;
	BOOL m_inactiveAckReq;
	SOCmnString m_inactiveMessage;
}; // SOXpSConditionClassSingleState

inline BOOL SOXpSConditionClassSingleState::setInactiveMessage(IN LPCTSTR message)
{ m_inactiveMessage = message; return TRUE; }

inline BOOL SOXpSConditionClassSingleState::setInactiveAckRequired(IN BOOL ackReq)
{ m_inactiveAckReq = ackReq; return TRUE; }

inline BOOL SOXpSConditionClassSingleState::setActiveAckRequired(IN BOOL ackReq)
{ m_activeAckReq = ackReq; return TRUE; }




//-----------------------------------------------------------------------------
// SOXpSConditionClassMultipleState                                           |
//-----------------------------------------------------------------------------

class SOXpSConditionClassMultipleState :
	public SOAeSConditionClassMultipleStateDefault
{
public:
	SOXpSConditionClassMultipleState(void);

	// inactive subcondition
	virtual SOCmnString getInactiveMessage(void);
	BOOL setInactiveMessage(IN LPCTSTR message);

	virtual BOOL getInactiveAckRequired(void);
	BOOL setInactiveAckRequired(IN BOOL ackReq);

protected:
	BOOL m_inactiveAckReq;
	SOCmnString m_inactiveMessage;
}; // SOXpSConditionClassMultipleState

inline BOOL SOXpSConditionClassMultipleState::setInactiveMessage(IN LPCTSTR message)
{ m_inactiveMessage = message; return TRUE; }

inline BOOL SOXpSConditionClassMultipleState::setInactiveAckRequired(IN BOOL ackReq)
{ m_inactiveAckReq = ackReq; return TRUE; }




//-----------------------------------------------------------------------------
// SOXpSCondition                                                             |
//-----------------------------------------------------------------------------

class SOXpSCondition :
	public SOAeSCondition,
	virtual public SOAeSConditionAttrFull,
	virtual public SOCmnElementListAdditional
{
public:
	SOXpSCondition(void);

	// translate attribute values to DA item ids
	virtual HRESULT translateToItemIDs(IN LPCTSTR subConditionName,
			OUT SOCmnList<SOAeSAttributeValueDADescription> *descrDAList);

	// subcondtion data
	virtual DWORD getSeverity(void);
	virtual BOOL getAckRequired(IN WORD changeMask);
	virtual SOCmnString getMessage(void);

	// instance data
	void enableInstanceData(IN BOOL enable);
	void setSeverity(IN DWORD severity);
	void setAckRequired(IN BOOL ackReq);
	void setMessage(IN LPCTSTR message);

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

	// create event
	virtual SOAeSEvent *createEvent(IN WORD changeMask, OPTIONAL IN LPFILETIME occTime = NULL);

protected:
	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
	BYTE m_instanceData;
	BYTE m_ackRequired;
	DWORD m_severity;
	SOCmnString m_message;

	virtual SOCmnObject *getObjectAeSConditionIAttr(void);

	virtual HRESULT handleAcknowledge(IN LPCTSTR ackID, IN LPCTSTR ackComment,
			LPFILETIME activeTime);
}; // SOXpSCondition

inline void SOXpSCondition::enableInstanceData(IN BOOL enable)
{ m_instanceData = enable; }

inline void SOXpSCondition::setSeverity(IN DWORD severity)
{ m_severity = severity; }

inline void SOXpSCondition::setAckRequired(IN BOOL ackReq)
{ m_ackRequired = ackReq; }

inline void SOXpSCondition::setMessage(IN LPCTSTR message)
{ m_message = message; }




//-----------------------------------------------------------------------------
// SOXpSArea                                                                  |
//-----------------------------------------------------------------------------

class SOXpSArea : public SOAeSAreaDefault
{
public:
	SOXpSArea(IN LPCTSTR name);

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

protected:
	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
}; // SOXpSArea




//-----------------------------------------------------------------------------
// SOXpSSource                                                                |
//-----------------------------------------------------------------------------

class SOXpSSource : public SOAeSSourceDefault
{
public:
	SOXpSSource(IN LPCTSTR name);

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);
	void setDeviceState(IN LPCTSTR deviceState, IN LPCTSTR deviceStateInfo) { m_deviceState = deviceState; m_deviceStateInfo = deviceStateInfo; }

protected:
	SOCmnString m_deviceState;
	SOCmnString m_deviceStateInfo;
}; // SOXpSSource


#pragma pack(pop)
#endif
