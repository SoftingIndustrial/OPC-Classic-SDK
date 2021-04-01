#ifndef _SOAESATTRIBUTE_H_
#define _SOAESATTRIBUTE_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeSAttribute                                                             |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSAttribute :
	public SOCmnElement,
	virtual public SOCmnElementNameInt
{
public:
	SOAeSAttribute(void);
	SOAeSAttribute(IN DWORD attributeID, IN LPCTSTR attributeDescription,
				   IN VARTYPE datatype);

	// object identity
	virtual BOOL isUniqueName(void) const;

	// datatype of the attribute
	virtual BOOL setDatatype(IN VARTYPE datatype);
	VARTYPE getDatatype(void) const;

	// description
	virtual BOOL setDescription(IN LPCTSTR descr);
	SOCmnString getDescription(void) const;

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

protected:
	SOCmnString m_description;
	VARTYPE m_datatype;

	virtual ~SOAeSAttribute(void);

	virtual void onSetDatatype(IN VARTYPE datatype);
	virtual void onSetDescription(IN LPCTSTR descr);

	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SOAeSAttribute

inline VARTYPE SOAeSAttribute::getDatatype(void) const
{
	return m_datatype;
}

inline SOCmnString SOAeSAttribute::getDescription(void) const
{
	return m_description;
}




//-----------------------------------------------------------------------------
// SOAeSAttributeValue                                                        |
//-----------------------------------------------------------------------------

class SOAeSConditionGroup;

class SOAES_EXPORT SOAeSAttributeValue :
	public SOCmnElement
{
public:
	SOAeSAttributeValue(void);
	SOAeSAttributeValue(IN SOAeSAttribute* attribute);

	// attribute description
	virtual BOOL setAttribute(IN SOAeSAttribute* attribute);
	SOAeSAttribute* getAttribute(void);

	// object identity
	virtual BOOL getKey(OUT SOCmnString& key);
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN LPCTSTR key);
	virtual BOOL hasKey(IN DWORD key);

	// value
	virtual BOOL setValue(IN LPVARIANT value);
	virtual BOOL getValue(OUT LPVARIANT value);
	virtual BOOL getValue(IN VARTYPE typeWanted, OUT LPVARIANT value);

	// datatype conversion
	virtual BOOL changeDatatype(IN VARIANT* value, IN VARTYPE type,
								OUT VARIANT* changedValue);

	// establish connection for attribute value update
	virtual BOOL connectValueUpdate(IN SOAeSConditionGroup* condGrp);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

protected:
	SOCmnPointer<SOAeSAttribute> m_attribute;
	SOCmnVariant m_value;

	~SOAeSAttributeValue(void);

	virtual void onSetAttribute(IN SOAeSAttribute* attribute);
}; // SOAeSAttributeValue

inline SOAeSAttribute* SOAeSAttributeValue::getAttribute(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_attribute.addRef();
	return (SOAeSAttribute*)m_attribute;
}




//-----------------------------------------------------------------------------
// SOAeSAttributeValueDADescription                                           |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSAttributeValueDADescription :
	public SOCmnData
{
public:
	SOAeSAttributeValueDADescription();
	virtual ~SOAeSAttributeValueDADescription();

	SOCmnString m_itemID;
	SOCmnString m_nodeName;
	CLSID m_clsid;
	DWORD m_attrID;
}; // SOAeSAttributeValueDADescription


#pragma pack(pop)
#endif
