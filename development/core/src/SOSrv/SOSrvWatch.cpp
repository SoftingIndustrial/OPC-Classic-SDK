//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvWatch.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server monitoring classes                               |
//                - SOSrvWatch: watch object                                  |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "SOCmnHandleManager.h"

#ifdef SOSRV

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// SOSrvWatch                                                                 |
//-----------------------------------------------------------------------------

SOSrvWatch::SOSrvWatch(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	m_changesList.create(SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
	m_statTime.now();
}


//-----------------------------------------------------------------------------
// getChildren
//
// - browse the available object trees
//
// returns:
//		S_OK         - information retured
//		E_INVALIDARG - invalid parent object
//    E_NOTIMPL    - watch functionality not activated
//
HRESULT SOSrvWatch::getChildren(
	IN LPCTSTR parent,      // parent object
	IN BOOL withAttributes, // add the object attributes
	OUT SOCmnStringEx& children)   // children descriptions
{
	SOCmnPointer<SOSrvWatchMethod> mth;
	USES_CONVERSION;
	LPCSTR object = T2CA(parent);
	SOCMN_ALLOCATE_OBJECT_C(mth, SOSrvWatchMethod, (SOSrvWatchMethod*))
	mth->setObjectState(SOSRVWATCHMETHOD_STATE_GETOBJECT);

	if (!mth->parse(object, strlen(object)))
	{
		return E_FAIL;
	}

	if (mth->getObjectState() == SOCMNXMLMETHOD_STATE_ERROR)
	{
		return E_FAIL;
	}

	SOCmnPointer<SOCmnObject> parentObj;
	SOCmnList<SOSrvWatchMethodData> xmlData(mth->getMethodData());
	SOSrvWatchMethodData* pMD = xmlData.getHead();
	// get parent object
	parentObj = getObject(pMD);

	if (!parentObj)
	{
		return E_INVALIDARG;
	}

	SOCmnListPosition pos;
	SOCmnObject* childObj;
	SOCmnStringEx szObjWatchData;
	szObjWatchData.getBuffer(256);

	if (parentObj->is(SOCMNOBJECT_TYPE_ELEMENT))
	{
		SOCmnList<SOCmnElement> branchList(((SOCmnElement*)(SOCmnObject*)parentObj)->getBranchList());
		SOCmnList<SOCmnElement> leafList(((SOCmnElement*)(SOCmnObject*)parentObj)->getLeafList());
		SOCmnList<SOCmnElement> addList(((SOCmnElement*)(SOCmnObject*)parentObj)->getAdditionalList());
		DWORD childrenCnt = 0;

		if (branchList.isInit())
		{
			childrenCnt += branchList.getCount();
		}

		if (leafList.isInit())
		{
			childrenCnt += leafList.getCount();
		}

		if (addList.isInit())
		{
			childrenCnt += addList.getCount();
		}

		children.getBuffer(childrenCnt * 256);
		children = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		addStartElementText(children, SOWATCH_ELEMENT_OBJECTS_T);

		if (branchList.isInit())
		{
			pos = branchList.getStartPosition();

			while (pos)
			{
				szObjWatchData.empty();
				childObj = (SOCmnObject*)branchList.getNext(pos);
				childObj->getObjWatchData(szObjWatchData, withAttributes, SOWATCH_ELEMENT_OBJECT_T);
				children += szObjWatchData;
			}
		}

		if (leafList.isInit())
		{
			pos = leafList.getStartPosition();

			while (pos)
			{
				szObjWatchData.empty();
				childObj = (SOCmnObject*)leafList.getNext(pos);
				childObj->getObjWatchData(szObjWatchData, withAttributes, SOWATCH_ELEMENT_OBJECT_T);
				children += szObjWatchData;
			}
		}

		if (addList.isInit())
		{
			pos = addList.getStartPosition();

			while (pos)
			{
				szObjWatchData.empty();
				childObj = (SOCmnObject*)addList.getNext(pos);
				childObj->getObjWatchData(szObjWatchData, withAttributes, SOWATCH_ELEMENT_OBJECT_T);
				children += szObjWatchData;
			}
		}

		addEndElementText(children, SOWATCH_ELEMENT_OBJECTS_T);
	}
	else if ((SOCmnObject*)parentObj == (SOCmnObject*)this)
	{
		// get all root objects
		SOCmnList<SOCmnObject> objList;
		objList.create(SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_ORG_LIST);

		if (m_DAHandler.isNotNull())
		{
			m_DAHandler->getRootObjects(objList);
		}

		if (m_AEHandler.isNotNull())
		{
			m_AEHandler->getRootObjects(objList);
		}

		if (m_customHandler.isNotNull())
		{
			m_customHandler->getRootObjects(objList);
		}

		children.getBuffer(objList.getCount() * 512);
		addStartElementText(children, SOWATCH_ELEMENT_OBJECTS_T);
		// generate child descriptions
		pos = objList.getStartPosition();

		while (pos)
		{
			szObjWatchData.empty();
			childObj = objList.getNext(pos);
			childObj->getObjWatchData(szObjWatchData, withAttributes, SOWATCH_ELEMENT_OBJECT_T);
			children += szObjWatchData;
		}

		addEndElementText(children, SOWATCH_ELEMENT_OBJECTS_T);
	}
	else
	{
		return E_INVALIDARG;
	}

	return S_OK;
} // getChildren


//-----------------------------------------------------------------------------
// collectChanges
//
// - enable or disable the collection of attribute changes
//
// returns:
//		S_OK         - en/disabled collection of all objects
//		S_FALSE      - at least one invalid object
//		E_INVALIDARG - invalid argument
//    E_NOTIMPL    - watch functionality not activated
//
HRESULT SOSrvWatch::collectChanges(
	IN LPCTSTR objectsT,
	IN DWORD doCollect)
{
	SOCmnPointer<SOSrvWatchMethod> mth;
	USES_CONVERSION;
	LPCSTR objects = T2CA(objectsT);
	SOCMN_ALLOCATE_OBJECT_C(mth, SOSrvWatchMethod, (SOSrvWatchMethod*))
	mth->setObjectState(SOSRVWATCHMETHOD_STATE_COLLECTCHANGES);

	if (!mth->parse(objects, strlen(objects)))
	{
		return E_FAIL;
	}

	if (mth->getObjectState() == SOCMNXMLMETHOD_STATE_ERROR)
	{
		return E_FAIL;
	}

	HRESULT res = S_OK;
	SOCmnList<SOSrvWatchMethodData> xmlData(mth->getMethodData());
	SOSrvWatchMethodData* pMD;
	SOCmnListPosition pos;
	SOCmnPointer<SOCmnObject> obj;
	SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
	SOCmnList<SOSrvWatch> watchList(entry->getWatchList());
	SOCmnList<SOSrvWatchData> changesList(m_changesList);
	pos = xmlData.getStartPosition();

	while (pos)
	{
		pMD = xmlData.getNext(pos);
		// get object
		obj = getObject(pMD);

		if (obj.isNotNull())
		{
			if (doCollect < 0x80000000)
			{
				// add
				if (!changesList.findKey(obj->getOpcHandle()))
				{
					// no entry already exists
					// add watch data to changes list
					SOSrvWatchData* watchData;
					SOCMN_ALLOCATE_OBJECT(watchData, SOSrvWatchData);
					watchData->m_object = obj;
					watchData->m_clientHandle = pMD->m_clientHandle;
					changesList.add(watchData, obj->getOpcHandle());
					changesList.autoUnlock();
					// set collect changes flag in object
					watchList.lock();   // sync all watch objects via watch list
					obj->setCollectChanges(SOCMNOBJECT_FLAG_COLLECT_CHANGES);

					if (doCollect == SOWATCH_COLLECT_ADD_ADDITIONAL)
					{
						obj->setCollectChanges(SOCMNOBJECT_FLAG_COLLECT_CHANGES | SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES);

						if ((obj->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
						{
							m_DAHandler->setCollectCacheChanges(obj, TRUE);
						}
					}

					watchList.unlock();
				}
				else
				{
					if (doCollect == SOWATCH_COLLECT_ADD_ADDITIONAL)
					{
						watchList.lock();   // sync all watch objects via watch list
						obj->setCollectChanges(SOCMNOBJECT_FLAG_COLLECT_CHANGES | SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES);

						if ((obj->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
						{
							m_DAHandler->setCollectCacheChanges(obj, TRUE);
						}

						watchList.unlock();
					}
				}
			} // add
			else
			{
				// remove
				// remove watch data from changes list
				changesList.removeKey((DWORD)(SOCmnObject*)obj);
				changesList.autoUnlock();
				// check if to reset collect changes flag in object
				watchList.lock();   // sync all watch objects via watch list
				SOSrvWatch* watchObject;
				BOOL remove = TRUE;
				SOCmnListPosition pos = watchList.getStartPosition();

				while (pos)
				{
					watchObject = watchList.getNext(pos);

					if (watchObject != this)
					{
						SOCmnList<SOSrvWatchData> wocl(watchObject->m_changesList);

						if (wocl.findKey(obj->getOpcHandle()))
						{
							remove = FALSE;
							break;
						}
					}
				}

				if (remove)
				{
					if (doCollect != SOWATCH_COLLECT_REMOVE_ADDITIONAL)
					{
						obj->setCollectChanges(0);

						if ((obj->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
						{
							m_DAHandler->setCollectCacheChanges(obj, FALSE);
						}
					}
					else
					{
						obj->setCollectChanges(SOCMNOBJECT_FLAG_COLLECT_CHANGES);

						if ((obj->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
						{
							m_DAHandler->setCollectCacheChanges(obj, FALSE);
						}
					}
				}

				watchList.unlock();
			} // remove
		}
		else
		{
			res = S_FALSE;
		}
	}

	return res;
} // collectChanges


//-----------------------------------------------------------------------------
// getChanges
//
// - get the changes of the objects
//
// returns:
//		S_OK      - returned the changes information
//    E_NOTIMPL - watch functionality not activated
//
HRESULT SOSrvWatch::getChanges(
	OUT SOCmnStringEx& changes) // changes
{
	SOCmnList<SOSrvWatchData> changesList(m_changesList);
	changesList.autoLock();
	changes.getBuffer(512 + m_addObjects.getLength() + m_removeObjects.getLength());
	addStartElementText(changes, SOWATCH_ELEMENT_CHANGES_T);

	if (!m_addObjects.isEmpty())
	{
		changes += m_addObjects;
		m_addObjects.empty();
	}

	if (!changesList.isEmpty())
	{
		SOCmnListPosition posW = changesList.getStartPosition();
		SOSrvWatchData* watchData;
		SOCmnListPosition posE;
		SOCmnStringEx szObjWD;
		szObjWD.getBuffer(128);

		while (posW)
		{
			watchData = changesList.getNext(posW);

			if (!watchData->m_elementList.isEmpty())
			{
				// changes to report
				addStartElementText(changes, SOWATCH_ELEMENT_CHANGE_T);
				szObjWD.empty();
				watchData->m_object->getObjWatchData(szObjWD, FALSE, SOWATCH_ELEMENT_OBJECT_T);
				szObjWD.replace(_T("</ob>"), _T(""));
				createElementText(szObjWD, SOWATCH_ELEMENT_CLIENTHANDLE_T, watchData->m_clientHandle);
				addStartElementText(szObjWD, SOWATCH_ELEMENT_ATTRIBUTES_T);
				posE = watchData->m_elementList.getStartPosition();

				while (posE)
				{
					szObjWD += watchData->m_elementList.getNext(posE);
				}

				watchData->m_elementList.removeAll();
				addEndElementText(szObjWD, SOWATCH_ELEMENT_ATTRIBUTES_T);
				addEndElementText(szObjWD, SOWATCH_ELEMENT_OBJECT_T);
				changes += szObjWD;
				addEndElementText(changes, SOWATCH_ELEMENT_CHANGE_T);
			}
		}
	}

	if (!m_removeObjects.isEmpty())
	{
		changes += m_removeObjects;
		m_removeObjects.empty();
	}

	addEndElementText(changes, SOWATCH_ELEMENT_CHANGES_T);
	return (changes.getLength() > 11) ? S_OK : S_FALSE;
} // getChanges


//-----------------------------------------------------------------------------
// getStatistics
//
// - get the statistics infomation of the server
//
// returns:
//		S_OK      - returned the statistic information
//    E_NOTIMPL - watch functionality not activated
//
HRESULT SOSrvWatch::getStatistics(
	OUT SOCmnStringEx& statistics) // statistic information
{
	SOCmnStringEx statDA, statAE, statCust;
	{
		SOCmnSingleLock<SOCmnSync> lock(getStatisticsLock());

		if (m_DAHandler.isNotNull())
		{
			m_DAHandler->getStatistics(statDA);
		}

		if (m_AEHandler.isNotNull())
		{
			m_AEHandler->getStatistics(statAE);
		}

		if (m_customHandler.isNotNull())
		{
			m_customHandler->getStatistics(statCust);
		}
	}
	m_statTime.now();
	SOCmnPointer<SOSrvConverter> conv = getSOSrvConverter();
	SOCmnString buffer;
	statistics.format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?><sta><StartTime>%s</StartTime>%s%s%s</sta>"),
					  m_statTime.toString(buffer, conv->getLCID()), (LPCTSTR)statDA, (LPCTSTR)statAE, (LPCTSTR)statCust);
	return S_OK;
} // getStatistics


//-----------------------------------------------------------------------------
// getObject
//
// - get object with all it's attributes
//
// returns:
//		S_OK         - information retured
//		E_INVALIDARG - invalid object description
//    E_NOTIMPL    - watch functionality not activated
//
HRESULT SOSrvWatch::getObject(
	IN LPCTSTR objectDescription, // object description
	OUT SOCmnStringEx& object)           // object
{
	SOCmnPointer<SOSrvWatchMethod> mth;
	USES_CONVERSION;
	LPCSTR objectD = T2CA(objectDescription);
	SOCMN_ALLOCATE_OBJECT_C(mth, SOSrvWatchMethod, (SOSrvWatchMethod*))
	mth->setObjectState(SOSRVWATCHMETHOD_STATE_GETOBJECT);

	if (!mth->parse(objectD, strlen(objectD)))
	{
		return E_FAIL;
	}

	if (mth->getObjectState() == SOCMNXMLMETHOD_STATE_ERROR)
	{
		return E_FAIL;
	}

	SOCmnPointer<SOCmnObject> obj;
	SOCmnList<SOSrvWatchMethodData> xmlData(mth->getMethodData());
	SOSrvWatchMethodData* pMD = xmlData.getHead();
	// get parent object
	obj = getObject(pMD);

	if (!obj)
	{
		return E_INVALIDARG;
	}

	object = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	obj->getObjWatchData(object, TRUE, SOWATCH_ELEMENT_OBJECT_T);
	return S_OK;
} // getObject


//-----------------------------------------------------------------------------
// getObject
//
// - get the toolkit object from it's XML description
//
// returns:
//		NULL - no valid object
//    pointer to the object
//
SOCmnObject* SOSrvWatch::getObject(
	IN SOSrvWatchMethodData* pMD)
{
	if (!pMD)
	{
		return NULL;
	}

	SOCmnObject* pObj = NULL;

	// find the object
	// 1) take the serverHandle
	if (pMD->m_serverHandle != 0)
	{
		SOCMN_TRY
		{
			pObj = (SOCmnObject*)GET_MEMORY_ADDRESS(pMD->m_serverHandle);

			if (pObj)
			{
				pObj->addRef();

				if (!pMD->m_objectType.isEmpty())
				{
					// object type check
					if (pObj->getObjectType() != convertObjectType(pMD->m_objectType))
					{
						pObj->release();
						pObj = NULL;
					}
				}
			}
		}
		SOCMN_CATCH(...)
		{
			pObj = NULL;
		}
	}

	if ((!pObj) && (!pMD->m_objectSpace.isEmpty()))
	{
		// 2) take space and type
		TCHAR osS = *(LPCTSTR)pMD->m_objectType;

		if ((osS == _T('D')) && (m_DAHandler.isNotNull()))
		{
			pObj = m_DAHandler->getObject(pMD->m_objectSpace, pMD->m_objectType);
		}
		else if ((osS == _T('A')) && (m_AEHandler.isNotNull()))
		{
			pObj = m_AEHandler->getObject(pMD->m_objectSpace, pMD->m_objectType);
		}
		else if (_tcscmp(pMD->m_objectSpace, SOWATCH_SPACE_ROOT) == 0)
		{
			// root object -> return watch object
			pObj = (SOCmnObject*)this;
			pObj->addRef();
		}
	}

	if ((!pObj) && (!pMD->m_objectSpace.isEmpty()))
	{
		// 3) take space and the attribute itemID
		SOCmnString itemID;

		if ((pMD->m_objectSpace == SOWATCH_SPACE_DA_NAMESPACE) && (m_DAHandler.isNotNull()))
		{
			if (pMD->m_attributes.findKey(SOWATCH_ATTRIBUTE_ITEMID_T, &itemID))
			{
				pObj = m_DAHandler->getNameSpaceObjectByItemID(itemID);
			}
		}
	}

	return pObj;
} // getObject


//-----------------------------------------------------------------------------
// attributeChange
//
// - add attribute change to changes list
//
// returns:
//    TRUE  - change added
//    FALSE - error adding change or not colleting changes for this object
//
BOOL SOSrvWatch::attributeChange(
	IN SOCmnObject* pObj,
	IN LPCTSTR attrName,
	IN LPCTSTR attrValue)
{
	SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
	SOCmnStringEx szAC;
	createElementText(szAC, attrName, attrValue);
	return entry->watchAttributeChange(pObj, attrName, szAC);
}

BOOL SOSrvWatch::changeAttribute(
	IN SOCmnObject* pObj,
	IN LPCTSTR attrName,
	IN LPCTSTR attrValue)
{
	SOCmnList<SOSrvWatchData> changesList(m_changesList);
	SOSrvWatchData* watchData = NULL;
	SOCmnListPosition pos = NULL;
	BOOL ret = FALSE;
	changesList.autoLock();

	do
	{
		if (!pObj->is(SOCMNOBJECT_TYPE_CACHE))
		{
			if ((watchData = changesList.findKey(pObj->getOpcHandle())) == NULL)
			{
				return FALSE;    // not colleting changes for this object
			}
		}
		else
		{
			// cache object
			SOSrvWatchData* wd;
			BOOL uniqueCache = FALSE;

			if (pos == NULL)
			{
				pos = changesList.getStartPosition();
			}

			while (pos)
			{
				wd = changesList.getNext(pos);

				if ((wd->m_object->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
				{
					if (m_DAHandler->hasCacheObject(wd->m_object, pObj, uniqueCache))
					{
						if (uniqueCache)
						{
							pos = NULL;
						}

						watchData = wd;
						break;
					}
				}
			}
		}

		if (!watchData)
		{
			return ret;
		}

		ret = TRUE;

		if (watchData->m_elementList.findKey(attrName, NULL))
		{
			// change already exists -> update element entry
			watchData->m_elementList.update(attrValue, attrName);
		}
		else
		{
			// first change -> create element entry
			watchData->m_elementList.add(attrValue, attrName);
		}
	}
	while (pos != NULL);

	return TRUE;
} // attributeChange


BOOL SOSrvWatch::addObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj)
{
	SOCmnList<SOSrvWatchData> changesList(m_changesList);
	SOSrvWatchData* watchData = NULL;

	if ((watchData = changesList.findKey(pParent->getOpcHandle())) == NULL)
	{
		return FALSE;    // not colleting changes for this object
	}

	SOCmnStringEx szObjWD;
	szObjWD.getBuffer(512);
	addStartElementText(m_addObjects, SOWATCH_ELEMENT_ADD_T);
	// parent
	pParent->getObjWatchData(szObjWD, FALSE, SOWATCH_ELEMENT_PARENT_T);
	szObjWD.replace(_T("</pa>"), _T(""));
	createElementText(szObjWD, SOWATCH_ELEMENT_CLIENTHANDLE_T, watchData->m_clientHandle);
	addEndElementText(szObjWD, SOWATCH_ELEMENT_PARENT_T);
	m_addObjects += szObjWD;
	// object
	szObjWD.empty();
	pObj->getObjWatchData(szObjWD, TRUE, SOWATCH_ELEMENT_OBJECT_T);
	m_addObjects += szObjWD;
	addEndElementText(m_addObjects, SOWATCH_ELEMENT_ADD_T);
	return TRUE;
}

BOOL SOSrvWatch::removeObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj)
{
	SOCmnList<SOSrvWatchData> changesList(m_changesList);
	SOSrvWatchData* watchDataParent = NULL;
	SOSrvWatchData* watchDataObj = NULL;
	watchDataParent = changesList.findKey(pParent->getOpcHandle());
	watchDataObj = changesList.findKey(pObj->getOpcHandle());

	if ((!watchDataParent) && (!watchDataObj))
	{
		return FALSE;    // not colleting changes for this object
	}

	SOCmnStringEx szObjWD;
	szObjWD.getBuffer(512);
	addStartElementText(m_removeObjects, SOWATCH_ELEMENT_REMOVE_T);
	// parent
	pParent->getObjWatchData(szObjWD, FALSE, SOWATCH_ELEMENT_PARENT_T);

	if (watchDataParent)
	{
		szObjWD.replace(_T("</pa>"), _T(""));
		createElementText(szObjWD, SOWATCH_ELEMENT_CLIENTHANDLE_T, watchDataParent->m_clientHandle);
		addEndElementText(szObjWD, SOWATCH_ELEMENT_PARENT_T);
	}

	m_removeObjects += szObjWD;
	// object
	szObjWD.empty();
	pObj->getObjWatchData(szObjWD, FALSE, SOWATCH_ELEMENT_OBJECT_T);

	if (watchDataObj)
	{
		szObjWD.replace(_T("</ob>"), _T(""));
		createElementText(szObjWD, SOWATCH_ELEMENT_CLIENTHANDLE_T, watchDataObj->m_clientHandle);
		addEndElementText(szObjWD, SOWATCH_ELEMENT_OBJECT_T);

		if ((pObj->is(SOCMNOBJECT_TYPE_ITEM_TAG)) && (m_DAHandler.isNotNull()))
		{
			m_DAHandler->setCollectCacheChanges(pObj, FALSE);
		}

		changesList.removeKey((DWORD)(SOCmnObject*)pObj);
	}

	m_removeObjects += szObjWD;
	addEndElementText(m_removeObjects, SOWATCH_ELEMENT_REMOVE_T);
	return TRUE;
}

//-----------------------------------------------------------------------------
// convertObjectType
//
// - get the value of the text node of an element node
//
// returns:
//    value of text node
//
DWORD SOSrvWatch::convertObjectType(LPCTSTR objectType)
{
	if (!objectType)
	{
		return 0;
	}

	if (objectType[0] == _T('D'))
	{
		if (_tcscmp(objectType, SOWATCH_TYPE_DA_SERVER) == 0)
		{
			return SOCMNOBJECT_TYPE_SERVER_DA;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_GROUP) == 0)
		{
			return SOCMNOBJECT_TYPE_GROUP;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_ITEM) == 0)
		{
			return SOCMNOBJECT_TYPE_ITEM;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_NODE) == 0)
		{
			return SOCMNOBJECT_TYPE_NODE;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_TAG) == 0)
		{
			return SOCMNOBJECT_TYPE_TAG;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_PROPERTY) == 0)
		{
			return SOCMNOBJECT_TYPE_PROPERTY;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_OBJECTROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_DA_NAMESPACEROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT;
		}
	}
	else if (objectType[0] == _T('A'))
	{
		if (_tcscmp(objectType, SOWATCH_TYPE_AE_SERVER) == 0)
		{
			return SOCMNOBJECT_TYPE_SERVER_AE;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_SUBSCRIPTION) == 0)
		{
			return SOCMNOBJECT_TYPE_SUBSCRIPTION;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_AREABROWSER) == 0)
		{
			return SOCMNOBJECT_TYPE_AREA_BROWSER;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_AREA) == 0)
		{
			return SOCMNOBJECT_TYPE_AREA;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_SOURCE) == 0)
		{
			return SOCMNOBJECT_TYPE_SOURCE;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_CONDITION) == 0)
		{
			return SOCMNOBJECT_TYPE_CONDITION;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_CATEGORY) == 0)
		{
			return SOCMNOBJECT_TYPE_CATEGORY;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_ATTRIBUTE) == 0)
		{
			return SOCMNOBJECT_TYPE_ATTRIBUTE;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_ATTRIBUTEVALUE) == 0)
		{
			return SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_CONDITIONCLASS) == 0)
		{
			return SOCMNOBJECT_TYPE_CONDITION_CLASS;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_SUBCONDITIONCLASS) == 0)
		{
			return SOCMNOBJECT_TYPE_SUBCONDITION_CLASS;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_OBJECTROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_AREASPACEROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_AE_EVENTSPACEROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT;
		}
	}
	else if (objectType[0] == _T('C'))
	{
		if (_tcscmp(objectType, SOWATCH_TYPE_CM_ROOT) == 0)
		{
			return SOCMNOBJECT_TYPE_ROOT | SOCMNOBJECT_TYPE_OTHER;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_CM_BRANCH) == 0)
		{
			return SOCMNOBJECT_TYPE_BRANCH | SOCMNOBJECT_TYPE_OTHER;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_CM_LEAF) == 0)
		{
			return SOCMNOBJECT_TYPE_LEAF | SOCMNOBJECT_TYPE_OTHER;
		}
		else if (_tcscmp(objectType, SOWATCH_TYPE_CM_ADDITIONAL) == 0)
		{
			return SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | SOCMNOBJECT_TYPE_OTHER;
		}
	}

	return 0;
} // convertObjectType


BOOL SOSrvWatch::createElementText(
	SOCmnStringEx& szXML,
	LPCTSTR element,
	LPCTSTR text)
{
	SOCmnString et;
	SOCmnStringEx ex;
	et.format(_T("<%s>%s</%s>"), element, SOCmnStringEx::getXMLString(text, ex), element);
	szXML += et;
	return TRUE;
}


BOOL SOSrvWatch::createElementText(
	SOCmnStringEx& szXML,
	LPCTSTR element,
	DWORD dw)
{
	SOCmnString et;
	et.format(_T("<%s>%lu</%s>"), element, dw, element);
	szXML += et;
	return TRUE;
}

BOOL SOSrvWatch::createElementTextWithAttributes(SOCmnStringEx& szXML, LPCTSTR element, LPCTSTR text, LPCTSTR attr)
{
	SOCmnString et;
	SOCmnStringEx ex;
	et.format(_T("<%s %s>%s</%s>"), element, attr, SOCmnStringEx::getXMLString(text, ex), element);
	szXML += et;
	return TRUE;
}

BOOL SOSrvWatch::addElementText(SOCmnStringEx& szXML, LPCTSTR parentElement, LPCTSTR element, LPCTSTR text)
{
	SOCmnString et;
	SOCmnString pet;
	SOCmnStringEx ex;
	pet.format(_T("</%s>"), parentElement);
	et.format(_T("<%s>%s</%s>%s"), element, SOCmnStringEx::getXMLString(text, ex), element, pet);
	szXML.replace(pet, et);
	return TRUE;
}

void SOSrvWatch::addStartElementText(SOCmnStringEx& szXML, LPCTSTR element)
{
	if (szXML.isEmpty())
	{
		szXML.format(_T("<%s>"), element);
	}
	else
	{
		SOCmnString et;
		et.format(_T("<%s>"), element);
		szXML += et;
	}
}

void SOSrvWatch::addEndElementText(SOCmnStringEx& szXML, LPCTSTR element)
{
	SOCmnString et;
	et.format(_T("</%s>"), element);
	szXML += et;
}


//-----------------------------------------------------------------------------
// SOSrvWatchData                                                             |
//-----------------------------------------------------------------------------

SOSrvWatchData::SOSrvWatchData(void)
{
	m_elementList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
}

SOSrvWatchData::~SOSrvWatchData(void)
{
	m_elementList.destroy();
}



//-----------------------------------------------------------------------------
// SOSrvWatchHandler                                                          |
//-----------------------------------------------------------------------------

SOSrvWatchHandler::SOSrvWatchHandler(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{}


//-----------------------------------------------------------------------------
// getObject
//
// - get the toolkit object from it's description elements
//
// returns:
//		NULL - no valid object
//    pointer to the object
//
SOCmnObject* SOSrvWatchHandler::getObject(
	IN LPCTSTR objSpace, // object space
	IN LPCTSTR objType)  // object type
{
	return NULL;
}


//-----------------------------------------------------------------------------
// getRootObjects
//
// - get root objects
//
void SOSrvWatchHandler::getRootObjects(
	OUT SOCmnList<SOCmnObject> objList) // object list
{
} // getRootObjects


//-----------------------------------------------------------------------------
// hasCacheObject (OPC DA)
//
// - checks if the item tag object has the cache object
//
// returns:
//		TRUE  - tag has cache object
//		FALSE - tag has not cache object
//
BOOL SOSrvWatchHandler::hasCacheObject(
	IN SOCmnObject* tag,
	IN SOCmnObject* cache,
	OUT BOOL& uniqueCache)
{
	return FALSE;
} // hasCacheObject


//-----------------------------------------------------------------------------
// setCollectCacheChanges
//
// - set if to collect changes for the cache of a item tag object
//
// returns:
//		TRUE  - set
//		FALSE - error
//
BOOL SOSrvWatchHandler::setCollectCacheChanges(
	IN SOCmnObject* tag, // item tag
	IN BOOL doCollect)   // do collect changes
{
	return FALSE;
} // setCollectCacheChanges


//-----------------------------------------------------------------------------
// getStatistics
//
// - get the statistics
//
// returns:
//		S_OK   - got statistics
//		E_FAIL - error
//
HRESULT SOSrvWatchHandler::getStatistics(
	SOCmnStringEx& szXML)
{
	return S_OK;
} // getStatistics


//-----------------------------------------------------------------------------
// getNameSpaceObjectByItemID
//
// - get a DA namespace object by it's item id
//
// returns:
//		NULL - no valid object
//    pointer to the object
//
SOCmnObject* SOSrvWatchHandler::getNameSpaceObjectByItemID(
	IN LPCTSTR itemID) // item id
{
	return NULL;
} // getNameSpaceObjectByItemID




//-----------------------------------------------------------------------------
// SOSrvWatchMethod                                                           |
//-----------------------------------------------------------------------------

SOSrvWatchMethod::SOSrvWatchMethod(void)
{
	m_charDataType = SOSRVWATCHMETHODDATA_TYPE_NONE;
	m_methodElements.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	m_methodData.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_BLOCKED);
	m_pCurMethodData = NULL;
}

SOSrvWatchMethod::~SOSrvWatchMethod(void)
{
	m_methodElements.destroy();
	m_methodData.destroy();
}

void SOSrvWatchMethod::handleStartElement(LPCSTR nameA, LPCSTR* atts)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);
	DWORD i;
	_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)
	{
		// trace elements
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("S: %s"), name));

		for (i = 0; atts[i]; i += 2)
		{
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("   %s = %s"), atts[i], atts[i + 1]));
		}
	}

	if (getObjectState() < SOCMNXMLMETHOD_STATE_SOAP_BODY)
	{
		SOSrvXmlMethod::handleStartElement(nameA, atts);
	}
	else
	{
		switch (getObjectState())
		{
		case SOSRVWATCHMETHOD_STATE_COLLECTCHANGES:
			if (_tcsicmp(name, _T("obs")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_GETOBJECT);
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_GETOBJECT:
			if (_tcsicmp(name, _T("ob")) == 0)
			{
				SOSrvWatchMethodData* pMD;
				SOCMN_ALLOCATE_OBJECT(pMD, SOSrvWatchMethodData)
				m_pCurMethodData = pMD;
				m_methodData.add(pMD);
				setObjectState(SOSRVWATCHMETHOD_STATE_OBJECT);
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_OBJECT:
			if (_tcsicmp(name, _T("sp")) == 0)
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_OBJ_SP;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("ty")) == 0)
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_OBJ_TY;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("sh")) == 0)
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_OBJ_SH;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("ch")) == 0)
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_OBJ_CH;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("ats")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_OBJECT_ATTRIBUTES);
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_OBJ_ATS;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_OBJECT_ATTRIBUTES:
			m_charDataElementName = name;
			m_elementStack.push(name);
			break;

		case SOSRVWATCHMETHOD_STATE_SETTRACE:
			if (_tcsicmp(name, _T("op")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOPTIONS);
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_TRACEOPTIONS:
			if (_tcsicmp(name, _T("level")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOLEVEL);
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("file")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOFILE);
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("buffer")) == 0)
			{
				setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOBUFFER);
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_TRACEOLEVEL:
			if (_tcsicmp(name, _T("err")) == 0)
			{
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCLERR;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("wrn")) == 0)
			{
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCLWRN;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("inf")) == 0)
			{
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCLINF;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("dbg")) == 0)
			{
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCLDBG;
				m_elementStack.push(name);
			}

			break;

		case SOSRVWATCHMETHOD_STATE_TRACEOBUFFER:
		case SOSRVWATCHMETHOD_STATE_TRACEOFILE:
		{
			if (_tcsicmp(name, _T("enable")) == 0)
			{
				m_charDataType = getObjectState();
				m_charDataType <<= 8;
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCENABLE;
				m_elementStack.push(name);
			}
			else if (_tcsicmp(name, _T("size")) == 0)
			{
				m_charDataType = getObjectState();
				m_charDataType <<= 8;
				m_charDataType |= SOSRVWATCHMETHODDATA_TYPE_TRCSIZE;
				m_elementStack.push(name);
			}
			else if ((_tcsicmp(name, _T("name")) == 0) && (getObjectState() == SOSRVWATCHMETHOD_STATE_TRACEOFILE))
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_TRCFNAM;
				m_elementStack.push(name);
			}
			else if ((_tcsicmp(name, _T("name2")) == 0) && (getObjectState() == SOSRVWATCHMETHOD_STATE_TRACEOFILE))
			{
				m_charDataType = SOSRVWATCHMETHODDATA_TYPE_TRCFNA2;
				m_elementStack.push(name);
			}
			else
			{
				setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			}
		}
		break;

		default:
			SOSrvXmlMethod::handleStartElement(nameA, atts);
			break;
		}
	}
}

void SOSrvWatchMethod::handleEndElement(LPCSTR nameA)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("E: %s"), name));

	switch (getObjectState())
	{
	case SOSRVWATCHMETHOD_STATE_TRACEOLEVEL:
		if (_tcsicmp(name, _T("level")) == 0)
		{
			setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOPTIONS);
		}

		break;

	case SOSRVWATCHMETHOD_STATE_TRACEOFILE:
		if (_tcsicmp(name, _T("file")) == 0)
		{
			setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOPTIONS);
		}

		break;

	case SOSRVWATCHMETHOD_STATE_TRACEOBUFFER:
		if (_tcsicmp(name, _T("buffer")) == 0)
		{
			setObjectState(SOSRVWATCHMETHOD_STATE_TRACEOPTIONS);
		}

		break;

	case SOSRVWATCHMETHOD_STATE_OBJECT_ATTRIBUTES:
		if (_tcsicmp(name, _T("ats")) == 0)
		{
			setObjectState(SOSRVWATCHMETHOD_STATE_OBJECT);
		}

		break;

	case SOSRVWATCHMETHOD_STATE_OBJECT:
		if (_tcsicmp(name, _T("ob")) == 0)
		{
			setObjectState(SOSRVWATCHMETHOD_STATE_GETOBJECT);
		}

		break;
	}

	m_charDataType = SOSRVWATCHMETHODDATA_TYPE_NONE;
	SOSrvXmlMethod::handleEndElement(nameA);
}

void SOSrvWatchMethod::handleCharacterData(LPCSTR sA, INT len)
{
	USES_CONVERSION;
	SOCmnString c;
	c.setCHAR(sA, len);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("C: 0x%4.4X %s"), m_charDataType, (LPCTSTR)c));

	if (m_charDataType == SOSRVWATCHMETHODDATA_TYPE_NONE)
	{
		return;
	}

	switch (getObjectState())
	{
	case SOSRVWATCHMETHOD_STATE_TRACEOLEVEL:
	case SOSRVWATCHMETHOD_STATE_TRACEOBUFFER:
	case SOSRVWATCHMETHOD_STATE_TRACEOFILE:
		m_methodElements.add(c, m_charDataType);
		break;

	case SOSRVWATCHMETHOD_STATE_OBJECT:
		switch (m_charDataType)
		{
		case SOSRVWATCHMETHODDATA_TYPE_OBJ_SP:
			m_pCurMethodData->m_objectSpace = c;
			break;

		case SOSRVWATCHMETHODDATA_TYPE_OBJ_TY:
			m_pCurMethodData->m_objectType = c;
			break;

		case SOSRVWATCHMETHODDATA_TYPE_OBJ_SH:
			m_pCurMethodData->m_serverHandle = _tcstoul(c, NULL, 10);
			break;

		case SOSRVWATCHMETHODDATA_TYPE_OBJ_CH:
			m_pCurMethodData->m_clientHandle = _tcstoul(c, NULL, 10);
			break;
		}

		break;

	case SOSRVWATCHMETHOD_STATE_OBJECT_ATTRIBUTES:
		m_pCurMethodData->m_attributes.add(c, m_charDataElementName);
		break;
	}
}


SOSrvWatchMethodData::SOSrvWatchMethodData(void)
{
	m_clientHandle = 0;
	m_serverHandle = 0;
	m_attributes.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
}

SOSrvWatchMethodData::~SOSrvWatchMethodData(void)
{
	m_attributes.destroy();
}

#endif // SOFEATURE_WATCH

#endif // SOSRV
