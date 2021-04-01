#ifndef _SOAESSOURCE_H_
#define _SOAESSOURCE_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeSSource                                                                |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSSource :
	public SOCmnElement,
	virtual public SOCmnElementNameString
{
public:
	SOAeSSource(void);
	SOAeSSource(IN LPCTSTR sourceName);

	virtual BOOL addAdditionalElement(IN SOCmnElement* newEl);
	virtual BOOL isUniqueName(void) const;

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

	// areaspace elements lists
	inline SOCmnList<SOAeSCondition> *getConditionList(void)
	{
		return (SOCmnList<SOAeSCondition> *)getAdditionalList();
	}

protected:
	virtual ~SOAeSSource(void);

	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SOAeSSource




//-----------------------------------------------------------------------------
// SOAeSSourceDefault                                                         |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSSourceDefault :
	public SOAeSSource,
	virtual public SOCmnElementListAdditional
{
public:
	SOAeSSourceDefault();
	SOAeSSourceDefault(IN LPCTSTR sourceName);
protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SOAeSSourceDefault

#pragma pack(pop)
#endif
