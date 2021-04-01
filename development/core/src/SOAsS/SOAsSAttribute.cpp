#include "stdafx.h"

#ifdef SOASS

#include <comcat.h>

//-----------------------------------------------------------------------------
// SOAsSAttributeValue                                                        |
//-----------------------------------------------------------------------------

SOAsSAttributeValue::SOAsSAttributeValue(void)
	: SOAeSAttributeValue()
{
}

SOAsSAttributeValue::SOAsSAttributeValue(
	IN SOAeSAttribute* attribute)
	: SOAeSAttributeValue()
{
	setAttribute(attribute);
}

SOAsSAttributeValue::SOAsSAttributeValue(
	IN SOAeSAttribute* attribute,
	IN SODaSItemTag* itemTag)
	: SOAeSAttributeValue()
{
	setAttribute(attribute);
	setItemTag(itemTag);
}


SOAsSAttributeValue::~SOAsSAttributeValue(void)
{
	setItemTag(NULL);
}


//-----------------------------------------------------------------------------
// connectValueUpdate
//
// - establish connection for attribute value update
//
// returns:
//      TRUE  - created item for value update
//		FALSE - error creating item for value update
//
BOOL SOAsSAttributeValue::connectValueUpdate(
	IN SOAeSConditionGroup* condGrp)    // condition group
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (!itemTag)
	{
		return FALSE;    // no item tag set -> can't connect item to it
	}

	SOAsSConditionGroup* condGroup = (SOAsSConditionGroup*)condGrp;
	SOCmnPointer<SODaSGroup> grp = condGroup->getSurveyGroup();

	if (grp.isNotNull())
	{
		SOCmnList<SODaSItem> itemList = (SOCmnList<SODaSItem> *)grp->getLeafList();
		SOCmnListPosition pos;
		SODaSItem* item;
		SOCmnPointer<SODaSItemTag> loopItemTag;
		// try to find existing item for the item tag
		pos = itemList.getStartPosition();

		while (pos)
		{
			item = itemList.getNext(pos);
			loopItemTag = item->getItemTag();

			if (loopItemTag.isNotNull())
			{
				if ((SODaSItemTag*)loopItemTag == (SODaSItemTag*)itemTag)
				{
					return TRUE;
				}
			}
		}

		loopItemTag.release();
		// no item found -> create one
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SODaSCreator> creator = entry->getCreator();
		BOOL ret;
		item = creator->createItem(grp, itemTag);
		ret = grp->addLeaf(item);
		ret &= item->setItemTag(itemTag, TRUE);
		ret &= item->setActive(TRUE);
		item->release();
		return ret;
	}

	return FALSE;
} // connectValueUpdate


//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous tag
//
// returns:
//		TRUE
//
BOOL SOAsSAttributeValue::setItemTag(
	IN SODaSItemTag* tag) // tag object
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	SOCmnPointer<SODaSItemTag> prevItemTag;
	pObjectLock->lock();
	prevItemTag = m_itemTag;
	m_itemTag = tag;
	m_itemTag.addRef();
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
	SOCmnPointer<SOCmnElement> parent = getParentBranch();

	if (prevItemTag.isNotNull())
	{
		prevItemTag->removeConnectedObject(parent);
	}

	prevItemTag.release();

	if (tag)
	{
		tag->addConnectedObject(parent, FALSE);
	}

	return TRUE;
} // setItemTag


//-----------------------------------------------------------------------------
// updateValueFromCache
//
// - update the attribute value from the cache object of the connected item tag.
//
// returns:
//		TRUE  - value was updated from the cache
//		FALSE - no item tag connected
//
BOOL SOAsSAttributeValue::updateValueFromCache(
	OPTIONAL OUT LPVARIANT newValue)    // updated attribute value
{
	SOCmnPointer<SODaSItemTag> itemTag;
	itemTag = getItemTag();

	if (!!itemTag)
	{
		SOCmnPointer<SODaSCache> cache;
		SOCmnVariant value;
		cache = itemTag->getCache();
		cache->getValue(value);
		setValue((LPVARIANT)value);

		if (newValue)
		{
			SOCmnVariant::variantCopy(newValue, (LPVARIANT)value);
		}

		return TRUE;
	}
	else
	{
		if (newValue)
		{
			getValue(newValue);
		}

		return FALSE;
	}
} // updateValueFromCache

#endif // SOASS
