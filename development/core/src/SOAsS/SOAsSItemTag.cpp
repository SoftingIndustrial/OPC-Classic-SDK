#include "stdafx.h"

#ifdef SOASS

#include "SODaSItemTag.h"
#include "SODaSItem.h"


//-----------------------------------------------------------------------------
// SOAsSItemTagConnectObjectStore                                             |
//-----------------------------------------------------------------------------

SOAsSItemTagConnectObjectStore::SOAsSItemTagConnectObjectStore(void)
{
	m_itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
	m_conditionList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_SYNCED);
}

SOAsSItemTagConnectObjectStore::~SOAsSItemTagConnectObjectStore()
{
	m_itemList.destroy();
	m_conditionList.destroy();
}


//-----------------------------------------------------------------------------
// getAllConnectedObjects
//
// - get all items connected to the item tag
//
void SOAsSItemTagConnectObjectStore::getAllConnectedObjects(
	IN DWORD objType,                       // object type
	OUT SOCmnList<SOCmnObject> &objList)    // object list
{
	if (objType == SOCMNOBJECT_TYPE_ITEM)
	{
		SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
		objList.clone((SOCmnList<SOCmnObject> *)&m_itemList);
	}
	else if (objType == SOCMNOBJECT_TYPE_CONDITION)
	{
		SOCmnSingleLock<SOCmnSync> lock(m_conditionList.getSyncObject());
		objList.clone((SOCmnList<SOCmnObject> *)&m_conditionList);
	}
} // getAllConnectedObjects


//-----------------------------------------------------------------------------
// notifyAllConnectedObjects
//
// - notify all items of the item tag that the value changed (report update mode)
//
// returns:
//		number of items notified
//
DWORD SOAsSItemTagConnectObjectStore::notifyAllConnectedObjects(
	IN OPTIONAL SOCmnVariant* value,     // value
	IN OPTIONAL WORD quality,         // quality
	IN OPTIONAL SOCmnDateTime* timeStamp, // time stamp
	IN OPTIONAL HRESULT result)       // result
{
	_ASSERTION((getObjectDaSItemTagIConnect() != NULL), "The method getObjectDaSItemTagIConnect must be implemeted!");
	DWORD objCnt;
	objCnt = SODaSItemTagIConnect::notifyAllConnectedObjects(value, quality, timeStamp, result);
	SOCmnList<SOCmnObject> condList;
	condList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_KEY_NO);
	getAllConnectedObjects(SOCMNOBJECT_TYPE_CONDITION, condList);

	if (!condList.isEmpty())
	{
		objCnt += checkConditions((SOCmnList<SOAeSCondition> *) &condList);
	}

	condList.destroy();
	return objCnt;
} // notifyAllConnectedObjects


//-----------------------------------------------------------------------------
// addConnectedObject
//
// - add connected to the list of the item tag
//
void SOAsSItemTagConnectObjectStore::addConnectedObject(
	IN SOCmnObject* obj,          // connected object
	IN BOOL /* goingToActivate */)
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
			m_itemList.add((SODaSItem*) obj);
		}

		if (obj->is(SOCMNOBJECT_TYPE_CONDITION))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_conditionList.getSyncObject());
			m_conditionList.add((SOAeSCondition*)obj);
		}
	}
} // addItem


//-----------------------------------------------------------------------------
// removeConnectedObject
//
// - remove connected objects from the list of the item tag
//
void SOAsSItemTagConnectObjectStore::removeConnectedObject(
	IN SOCmnObject* obj)    // connected object
{
	if (obj)
	{
		if (obj->is(SOCMNOBJECT_TYPE_ITEM))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_itemList.getSyncObject());
			m_itemList.removeObject((SODaSItem*)obj);
		}

		if (obj->is(SOCMNOBJECT_TYPE_CONDITION))
		{
			SOCmnSingleLock<SOCmnSync> lock(m_conditionList.getSyncObject());
			m_conditionList.removeObject((SOAeSCondition*)obj);
		}
	}
} // removeItem


//-----------------------------------------------------------------------------
// checkConditions
//
// - check all connected conditions
//
DWORD SOAsSItemTagConnectObjectStore::checkConditions(
	IN SOCmnList<SOAeSCondition> *conditionList)
{
	SOCmnList<SOAeSCondition> condList(conditionList);
	SOCmnListPosition pos;
	SOAeSCondition* cond;
	DWORD condCnt = 0;
	pos = condList.getStartPosition();

	while (pos)
	{
		condCnt++;
		cond = condList.getNext(pos);
		cond->checkCondition();
	}

	return condCnt;
} // checkConditions

#endif // SOASS
