#include "stdafx.h"

#ifdef SOASS


//-----------------------------------------------------------------------------
// SOAsSCondition                                                             |
//-----------------------------------------------------------------------------

SOAsSCondition::SOAsSCondition(void)
	:  SOAeSCondition()
{}

SOAsSCondition::SOAsSCondition(
	IN SOAeSConditionClass* conditionClass) // condition class
	:  SOAeSCondition()
{
	setConditionClass(conditionClass);
}

//-----------------------------------------------------------------------------
// translateToItemIDs
//
// - get the OPC DA item IDs for the attributes of a (sub)condition
//
// returns:
//		S_OK
//
HRESULT SOAsSCondition::translateToItemIDs(
	IN LPCTSTR /* subConditionName */,                                  // subcondition name
	OUT SOCmnList<SOAeSAttributeValueDADescription> *descrDAList)   // DA description list
{
	if (descrDAList)
	{
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnListPosition pos;
		SOAeSAttributeValueDADescription* descr;
		SOCmnList<SOAeSAttributeValue> attrValList(getAttributeValueList());
		SOCmnPointer<SOAsSAttributeValue> attrVal;
		SOCmnPointer<SODaSItemTag> itemTag;
		pos = descrDAList->getStartPosition();

		while (pos)
		{
			descr = descrDAList->getNext(pos);
			// get the attribute value object for each attribute id
			attrVal = (SOAsSAttributeValue*)attrValList.findKey(descr->m_attrID);

			if (attrVal.isNotNull())
			{
				itemTag = attrVal->getItemTag();

				if (itemTag.isNotNull())
				{
					// DA item tag set for attribute
					descr->m_itemID = itemTag->getFullName();
					entry->getCLSID(descr->m_clsid);
					descr->m_nodeName = _T("");
				}
			}
		}
	}

	return S_OK;
} // translateToItemIDs




//-----------------------------------------------------------------------------
// SOAsSConditionDefault                                                      |
//-----------------------------------------------------------------------------

SOAsSConditionDefault::SOAsSConditionDefault(void)
	:  SOAsSCondition()
{}

SOAsSConditionDefault::SOAsSConditionDefault(
	IN SOAeSConditionClass* conditionClass) // condition class
	:  SOAsSCondition()
{
	setConditionClass(conditionClass);
}

//-----------------------------------------------------------------------------
// getObjectAeSConditionIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAsSConditionDefault::getObjectAeSConditionIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectAeSConditionIAttr


//-----------------------------------------------------------------------------
// SOAsSConditionGroup                                                        |
//-----------------------------------------------------------------------------

SOAsSConditionGroup::SOAsSConditionGroup(
	IN DWORD updateRate)
	: SOAeSConditionGroup(updateRate)
{
}


void SOAsSConditionGroup::connectAttributeValueUpdate(void)
{
	if (!m_surveyGroup)
	{
		// no DA group exists for this condition survey group
		// create DaS survey server object
		SOCmnPointer<SOAsSEntry> entryAS = ::getSOAsSEntry();
		SOCmnPointer<SODaSServer> server = entryAS->getSurveyServer();

		if (server.isNotNull())
		{
			server->addGroup(NULL, TRUE, m_updateRate, 0, NULL, NULL, 0,
							 (SODaSGroup**)m_surveyGroup, NULL);
		}
	}

	SOAeSConditionGroup::connectAttributeValueUpdate();
} // connectAttributeValueUpdate

#endif // SOASS

