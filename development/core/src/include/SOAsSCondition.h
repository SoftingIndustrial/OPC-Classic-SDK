#ifndef _SOASSCONDITION_H_
#define _SOASSCONDITION_H_

#include "SOAeSCondition.h"
#include "SODaSGroup.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAsSCondition                                                             |
//-----------------------------------------------------------------------------

class SOASS_EXPORT SOAsSCondition :
	public SOAeSCondition
{
public:
	SOAsSCondition(void);
	SOAsSCondition(IN SOAeSConditionClass* conditionClass);

	// translate attribute values to DA item ids
	virtual HRESULT translateToItemIDs(IN LPCTSTR subConditionName,
									   OUT SOCmnList<SOAeSAttributeValueDADescription> *descrDAList);
}; // SOAsSCondition




//-----------------------------------------------------------------------------
// SOAsSConditionDefault                                                      |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAsSConditionDefault :
	public SOAsSCondition,
	virtual public SOCmnElementListAdditional,
	virtual public SOAeSConditionAttrFull
{
public:
	SOAsSConditionDefault(void);
	SOAsSConditionDefault(IN SOAeSConditionClass* conditionClass);

protected:
	virtual SOCmnObject* getObjectAeSConditionIAttr(void);
}; // SOAsSConditionDefault




//-----------------------------------------------------------------------------
// SOAsSConditionGroup                                                        |
//-----------------------------------------------------------------------------

class SOAeSEntry;

class SOASS_EXPORT SOAsSConditionGroup :
	public SOAeSConditionGroup
{
	friend SOAeSEntry;
public:
	SOAsSConditionGroup(IN DWORD updateRate);

	// establish connection for attribute value update
	virtual void connectAttributeValueUpdate(void);

	SODaSGroup* getSurveyGroup(void);

protected:
	SOCmnPointer<SODaSGroup> m_surveyGroup;

}; // SOAsSConditionGroup

inline SODaSGroup* SOAsSConditionGroup::getSurveyGroup(void)
{
	m_surveyGroup.addRef();
	return (SODaSGroup*)m_surveyGroup;
}


#pragma pack(pop)
#endif
