#ifndef _SOAESCONDITION_H_
#define _SOAESCONDITION_H_

#include "SOAeSAttribute.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOAeSConditionClass                                                        |
//-----------------------------------------------------------------------------

class SOAeSSubConditionClass;

class SOAES_EXPORT SOAeSConditionClass :
	public SOCmnElement,
	virtual public SOCmnElementNameString
{
public:
	SOAeSConditionClass(void);
	SOAeSConditionClass(IN LPCTSTR conditionName);

	virtual HRESULT setName(IN LPCTSTR name);

	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);

	// inactive subcondition
	virtual SOCmnString getInactiveMessage(void);
	virtual BOOL getInactiveAckRequired(void);

	// definition of single state condition
	virtual SOCmnString getDefinition(void);

	// active single state subcondition
	virtual SOCmnString getActiveMessage(void);
	virtual DWORD getActiveSeverity(void);
	virtual BOOL getActiveAckRequired(void);

	// single or multiple state
	virtual BOOL isSingleStateCondition(void);
	virtual BOOL isMultipleStateCondition(void);

	// eventspace elements lists
	inline SOCmnList<SOAeSSubConditionClass> *getSubConditionClassList(void)
	{
		return (SOCmnList<SOAeSSubConditionClass> *)getLeafList();
	}

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

protected:
	virtual ~SOAeSConditionClass(void);

	virtual SOCmnObject* getObjectCmnElementIName(void);

	// have to check the uniqueness of the condition name?
	virtual BOOL trustUniqueConditionName(void);
}; // SOAeSConditionClass




//-----------------------------------------------------------------------------
// SOAeSConditionClassSingleStateDefault                                      |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSConditionClassSingleStateDefault :
	public SOAeSConditionClass
{
public:
	SOAeSConditionClassSingleStateDefault(void);
	SOAeSConditionClassSingleStateDefault(IN LPCTSTR conditionName);

	virtual BOOL setActiveMessage(IN LPCTSTR message);
	virtual SOCmnString getActiveMessage(void);

	virtual BOOL setActiveSeverity(IN DWORD severity);
	virtual DWORD getActiveSeverity(void);

	virtual SOCmnString getDefinition(void);
	virtual BOOL setDefinition(IN LPCTSTR definition);

protected:
	DWORD m_activeSeverity;
	SOCmnString m_activeMessage;
	SOCmnString m_definition;
}; // SOAeSConditionClassSingleStateDefault




//-----------------------------------------------------------------------------
// SOAeSConditionClassMultipleStateDefault                                    |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSConditionClassMultipleStateDefault :
	public SOAeSConditionClass,
	virtual public SOCmnElementListLeaf
{
public:
	SOAeSConditionClassMultipleStateDefault(void);
	SOAeSConditionClassMultipleStateDefault(IN LPCTSTR conditionName);

	virtual SOCmnString getDefinition(void);
	virtual BOOL setDefinition(IN LPCTSTR definition);

protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
	SOCmnString m_definition;
}; // SOAeSConditionClassMultipleStateDefault




//-----------------------------------------------------------------------------
// SOAeSConditionIAttr                                                        |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSConditionIAttr
{
public:
	SOAeSConditionIAttr(void);

	// active time
	virtual BOOL setActiveTime(IN LPFILETIME activeTime);
	virtual BOOL getActiveTime(OUT LPFILETIME activeTime);

	// inactive time
	virtual BOOL setInactiveTime(IN LPFILETIME inactiveTime);
	virtual BOOL getInactiveTime(OUT LPFILETIME inactiveTime);

	// quality
	virtual BOOL setQuality(IN WORD quality);
	virtual WORD getQuality(void);

	// acknowledge time
	virtual BOOL setAckTime(IN LPFILETIME ackTime);
	virtual BOOL getAckTime(OUT LPFILETIME ackTime);

	// acknowledger ID
	virtual BOOL setAckID(IN LPCTSTR ackID);
	virtual SOCmnString getAckID(void);

	// acknowledger comment
	virtual BOOL setAckComment(IN LPCTSTR ackComment);
	virtual SOCmnString getAckComment(void);

	// activation time
	virtual BOOL setActSubConditionTime(IN LPFILETIME actSubConditionTime);
	virtual BOOL getActSubConditionTime(OUT LPFILETIME actSubConditionTime);

protected:
	virtual ~SOAeSConditionIAttr(void);

	virtual void onSetActiveTime(IN LPFILETIME activeTime);
	virtual void onSetInactiveTime(IN LPFILETIME inactiveTime);
	virtual void onSetActSubCondtionTime(IN LPFILETIME actSubConditionTime);
	virtual void onSetAckTime(IN LPFILETIME ackTime);
	virtual void onSetQuality(IN WORD quality);
	virtual void onSetAckID(IN LPCTSTR ackID);
	virtual void onSetAckComment(IN LPCTSTR ackComment);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectAeSConditionIAttr(void);
}; // SOAeSConditionIAttr


//-----------------------------------------------------------------------------
// SOAeSConditionAttrFull                                                     |
//-----------------------------------------------------------------------------

class SOAeSAttributeValue;

class SOAES_EXPORT SOAeSConditionAttrFull : virtual public SOAeSConditionIAttr
{
public:
	SOAeSConditionAttrFull(void);

	// active time
	virtual BOOL setActiveTime(IN LPFILETIME activeTime);
	virtual BOOL getActiveTime(OUT LPFILETIME activeTime);

	// inactive time
	virtual BOOL setInactiveTime(IN LPFILETIME inactiveTime);
	virtual BOOL getInactiveTime(OUT LPFILETIME inactiveTime);

	// quality
	virtual BOOL setQuality(IN WORD quality);
	virtual WORD getQuality(void);

	// acknowledge time
	virtual BOOL setAckTime(IN LPFILETIME ackTime);
	virtual BOOL getAckTime(OUT LPFILETIME ackTime);

	// acknowledger ID
	virtual BOOL setAckID(IN LPCTSTR ackID);
	virtual SOCmnString getAckID(void);

	// acknowledger comment
	virtual BOOL setAckComment(IN LPCTSTR ackComment);
	virtual SOCmnString getAckComment(void);

	// activation time
	virtual BOOL setActSubConditionTime(IN LPFILETIME actSubConditionTime);
	virtual BOOL getActSubConditionTime(OUT LPFILETIME actSubConditionTime);

protected:
	FILETIME m_activeTime;
	FILETIME m_inactiveTime;
	FILETIME m_ackTime;
	FILETIME m_actSubConditionTime;
	SOCmnString m_ackID;
	SOCmnString m_ackComment;
	WORD m_quality;

	~SOAeSConditionAttrFull(void);
}; // SOAeSConditionAttrFull




//-----------------------------------------------------------------------------
// SOAeSCondition                                                             |
//-----------------------------------------------------------------------------

class SOAeSSubConditionClass;

class SOAES_EXPORT SOAeSCondition :
	public SOCmnElement,
	virtual public SOAeSConditionIAttr
{
public:
	SOAeSCondition(void);
	SOAeSCondition(IN SOAeSConditionClass* conditionClass);

	// object identity
	virtual BOOL getKey(OUT SOCmnString& key);
	virtual BOOL hasKey(IN LPCTSTR key);
	virtual SOCmnString getName(void);
	virtual void getName(OUT SOCmnString& name);

	// condition state
	virtual BOOL setObjectState(IN BYTE state);

	// condition class
	virtual BOOL setConditionClass(IN SOAeSConditionClass* conditionClass);
	SOAeSConditionClass* getConditionClass(void);

	// active subcondition class
	virtual BOOL setActSubConditionClass(IN SOAeSSubConditionClass* actSubCondition);
	SOAeSSubConditionClass* getActSubConditionClass(void);

	// last active subcondition class
	virtual BOOL setLastActSubConditionClass(IN SOAeSSubConditionClass* actSubCondition);

	// category of the condition
	SOAeSCategory* getCategory(void);

	// name of condition source
	virtual SOCmnString getSourceName(void);

	// notification cookie
	virtual DWORD getCookie(void);

	// subcondtion data
	virtual DWORD getSeverity(void);
	virtual BOOL getAckRequired(IN WORD changeMask);
	virtual SOCmnString getSubConditionName(void);
	virtual SOCmnString getMessage(void);

	// condition state
	virtual BOOL changeState(IN WORD newState, OPTIONAL IN LPFILETIME occTime = NULL,
							 OPTIONAL IN WORD addChangeMask = 0);

	// enabling
	virtual BOOL enable(IN BOOL enable);

	// activation
	virtual BOOL activate(IN BOOL activate,
						  OPTIONAL IN LPFILETIME actTime = NULL,
						  OPTIONAL IN WORD addChangeMask = OPC_CHANGE_ATTRIBUTE);
	virtual BOOL activate(IN SOAeSSubConditionClass* newSubCondition,
						  OPTIONAL IN LPFILETIME actTime = NULL,
						  OPTIONAL IN WORD addChangeMask = OPC_CHANGE_ATTRIBUTE);
	virtual BOOL activate(IN LPCTSTR newSubConditionName,
						  OPTIONAL IN LPFILETIME actTime = NULL,
						  OPTIONAL IN WORD addChangeMask = OPC_CHANGE_ATTRIBUTE);

	// acknowledge
	virtual HRESULT acknowledge(IN LPCTSTR ackID, IN LPCTSTR ackComment,
								LPFILETIME activeTime, DWORD cookie);

	// change quality
	virtual BOOL changeQuality(IN WORD quality);

	// update attribute value to the list
	virtual BOOL updateAttributeValue(IN DWORD attributeID, IN LPVARIANT value);

	// fire event
	virtual BOOL fireEvent(IN WORD changeMask, OPTIONAL IN LPFILETIME occTime = NULL);

	// create event
	virtual SOAeSEvent* createEvent(IN WORD changeMask, OPTIONAL IN LPFILETIME occTime = NULL);

	// translate attribute values to DA item ids
	virtual HRESULT translateToItemIDs(IN LPCTSTR subConditionName,
									   OUT SOCmnList<SOAeSAttributeValueDADescription> *descrDAList);

	// update attributes and check conditions (called from survey thread of entry)
	virtual void checkCondition(void);

	// attribute value list
	SOCmnList<SOAeSAttributeValue> *getAttributeValueList(void);

	virtual BYTE getServerInstance(void);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

protected:
	SOCmnPointer<SOAeSConditionClass> m_conditionClass;
	SOCmnPointer<SOAeSSubConditionClass> m_actSubConditionClass;
	SOCmnPointer<SOAeSSubConditionClass> m_lastActSubConditionClass;

	virtual ~SOAeSCondition(void);

	// external handling of client acknowledge
	virtual HRESULT handleAcknowledge(IN LPCTSTR ackID, IN LPCTSTR ackComment,
									  IN LPFILETIME activeTime);

	// copy the attribute values to an event
	virtual void copyAttributeValueList(OUT SOCmnList<SOAeSAttributeValue> *attrValueList);

	// callbacks
	virtual void onEnable(IN BOOL enable);
	virtual void onActivate(IN BOOL activate);
	virtual void onActivate(IN SOAeSSubConditionClass* newSubCondition);
	virtual void onAcknowledge(IN LPCTSTR ackID, IN LPCTSTR ackComment,
							   IN LPFILETIME activeTime, IN LPFILETIME ackTime);
	virtual void onSetConditionClass(IN SOAeSConditionClass* conditionClass);
	virtual void onSetActSubConditionClass(IN SOAeSSubConditionClass* actSucconditionClass);
}; // SOAeSCondition

inline SOAeSConditionClass* SOAeSCondition::getConditionClass(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_conditionClass.addRef();
	return (SOAeSConditionClass*)m_conditionClass;
}

inline SOAeSSubConditionClass* SOAeSCondition::getActSubConditionClass(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_actSubConditionClass.addRef();
	return (SOAeSSubConditionClass*)m_actSubConditionClass;
}

inline SOCmnList<SOAeSAttributeValue> *SOAeSCondition::getAttributeValueList(void)
{
	return (SOCmnList<SOAeSAttributeValue> *)getAdditionalList();
}




//-----------------------------------------------------------------------------
// SOAeSConditionDefault                                                      |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSConditionDefault :
	public SOAeSCondition,
	virtual public SOCmnElementListAdditional,
	virtual public SOAeSConditionAttrFull
{
public:
	SOAeSConditionDefault(void);
	SOAeSConditionDefault(IN SOAeSConditionClass* conditionClass);

protected:
	virtual SOCmnObject* getObjectAeSConditionIAttr(void);
}; // SOAeSConditionDefault




//-----------------------------------------------------------------------------
// SOAeSConditionGroup                                                        |
//-----------------------------------------------------------------------------

class SOAeSEntry;

class SOAES_EXPORT SOAeSConditionGroup :
	public SOCmnObject
{
	friend SOAeSEntry;
public:
	SOAeSConditionGroup(IN DWORD updateRate);

	DWORD getUpdateRate(void);
	SOCmnList<SOAeSCondition> *getConditionList(void);

	virtual void checkConditions(void);

	// establish connection for attribute value update
	virtual void connectAttributeValueUpdate(void);

protected:
	DWORD m_updateRate;         // update rate of the group
	SOCmnList<SOAeSCondition> m_condList;
	// condition list
	DWORD m_initTime;           // init time of the condition group in ms since system start
	WORD m_initCnt;             // init cyclic counter
	BYTE m_correctNTtimer;      // NT timer correction value

	// helpers
	virtual BOOL updateSpan(IN DWORD now, IN DWORD* span);
	virtual DWORD initSpan(IN DWORD toLate);
}; // SOAeSConditionGroup

inline DWORD SOAeSConditionGroup::getUpdateRate(void)
{
	return m_updateRate;
}

inline SOCmnList<SOAeSCondition> *SOAeSConditionGroup::getConditionList(void)
{
	return &m_condList;
}

#pragma pack(pop)
#endif
