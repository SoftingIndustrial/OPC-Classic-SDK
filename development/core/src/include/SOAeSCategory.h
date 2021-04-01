#ifndef _SOAESCATEGORY_H_
#define _SOAESCATEGORY_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeSCategory                                                              |
//-----------------------------------------------------------------------------

class SOAeSServer;
class SOAeSAttribute;
class SOAeSConditionClass;

class SOAES_EXPORT SOAeSCategory :
	public SOCmnElement,
	virtual public SOCmnElementNameInt,
	virtual public SOCmnElementListBranchAdditional
{
public:
	SOAeSCategory(void);
	SOAeSCategory(IN DWORD categoryID, IN LPCTSTR categoryDescription, IN DWORD eventType);

	virtual BOOL addAdditionalElement(IN SOCmnElement* newEl);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	// event type
	virtual BOOL setEventType(IN DWORD eventType);
	DWORD getEventType(void) const;

	// description
	virtual BOOL setDescription(IN LPCTSTR descr);
	SOCmnString getDescription(void) const;

	// objects of additional list (SOAeSAttribute)
	SOAeSAttribute* getAttribute(IN DWORD attributeID);

	// eventspace elements lists
	inline SOCmnList<SOAeSConditionClass> *getConditionClassList(void)
	{
		return (SOCmnList<SOAeSConditionClass> *)getBranchList();
	}
	inline SOCmnList<SOAeSAttribute> *getAttributeList(void)
	{
		return (SOCmnList<SOAeSAttribute> *)getAdditionalList();
	}

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

protected:
	DWORD m_eventType;
	SOCmnString m_description;

	virtual ~SOAeSCategory(void);

	virtual void onSetEventType(IN DWORD eventType);
	virtual void onSetDescription(IN LPCTSTR descr);

	virtual SOCmnObject* getObjectCmnElementIName(void);
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SOAeSCategory

inline DWORD SOAeSCategory::getEventType(void) const
{
	return m_eventType;
}

inline SOCmnString SOAeSCategory::getDescription(void) const
{
	return m_description;
}




//-----------------------------------------------------------------------------
// SOAeSEventSpaceRoot                                                        |
//-----------------------------------------------------------------------------

class SOAeSConditionClass;

class SOAES_EXPORT SOAeSEventSpaceRoot :
	public SOCmnElement,
	virtual public SOCmnElementListBranch
{
public:
	SOAeSEventSpaceRoot();

	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	SOAeSCategory* findCategory(IN DWORD eventCategory);
	SOAeSConditionClass* findConditionClass(IN LPCTSTR conditionName);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// eventspace elements lists
	inline SOCmnList<SOAeSCategory> *getCategoryList(void)
	{
		return (SOCmnList<SOAeSCategory> *)getBranchList();
	}

protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SOAeSEventSpaceRoot

#pragma pack(pop)
#endif
