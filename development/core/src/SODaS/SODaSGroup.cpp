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
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSGroup.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Group object related classes                            |
//                - SODaSGroup: OPC DA group object                           |
//                - SODaSDataCallback: Data of a callback advise              |
//                - SODaSGroupIAdvise: Advise interface of the group          |
//                - SODaSGroupAdviseOneOnlyV2: Only V2.0 advise support       |
//                - SODaSGroupAdviseFull: Full advise support                 |
//                - SODaSGroupDefault: Default configuration for group objects|
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSGroup.h"
#include "SODaSItemTag.h"
#include "SODaSNode.h"
#include "SODaSEntry.h"
#include "SOCmnTrace.h"
#include "SODaSServer.h"
#include "SODaSItem.h"
#include "SODaSCache.h"
#ifdef SOFEATURE_DCOM
#include "SODaSComGroup.h"
#include "SOSrvComObject.h"
#endif
#ifdef SOFEATURE_SOAP
#include "SODaSXmlServer.h"
#endif
#include <opcerror.h>

//-----------------------------------------------------------------------------
// SODaSGroup                                                                 |
//-----------------------------------------------------------------------------

SODaSGroup::SODaSGroup(void)
	: SOCmnElementListLeaf(0),
	  SOCmnElement(SOCMNOBJECT_TYPE_GROUP)
{
	// create leaf map
	if (m_leafList.isInit())
	{
		m_leafList.destroy();
	}

	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 53;
	m_leafList.create(&cfg);
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
#ifdef SOFEATURE_DCOM
#ifdef SOOS_WINDOWS
	TIME_ZONE_INFORMATION timeZone;
	DWORD timeZoneRet;
	// set system default time zone
	timeZoneRet = GetTimeZoneInformation(&timeZone);
	m_timeBias = timeZone.Bias;
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	m_timeBias = 0; // time bias only relevant for DCOM
#endif
#endif
	m_active = FALSE;
	m_updateRate = SOCMN_TIME_40DAYS;
	m_keepAliveTime = 0;
	m_pingRate = SOCMN_TIME_40DAYS;
	m_clientHandle = 0;
	m_deadBand = 0.0;
	m_lcid = MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT);
	m_me = NULL;
	m_serverHandle = (OPCHANDLE)this->getOpcHandle();
	m_cyclicTransaction = creator->createTransaction(this, SODaSTransaction::cyclic, SODaSTransaction::allConnections, 0);
	m_enable = TRUE;
	m_initCnt = 0;
	m_correctNTtimer = 0;
	m_publicTemplate = NULL;
	m_removed = FALSE;
	m_initTime = 0;
	m_notifiyAllChanges = FALSE;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_GROUP);
#endif
}

SODaSGroup::~SODaSGroup(void)
{
	m_removed = TRUE;
	m_cyclicTransaction.release();
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_GROUP);
#endif
}


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//      FALSE - no global unique name
//
BOOL SODaSGroup::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a item object
//
// returns:
//      TRUE  - added
//      FALSE - not added
//
BOOL SODaSGroup::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_ITEM))
	{
		m_leafList.doRehash(TRUE);
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key (name)
//
// returns:
//      object key
//
BOOL SODaSGroup::getKey(
	OUT DWORD& key)     // object key
{
	key = this->getOpcHandle();
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key (name) with given int
//
// returns:
//      TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SODaSGroup::hasKey(
	IN DWORD key)   // object key
{
	return (this->getOpcHandle() == key);
} // hasKey


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// getMe
//
// - get reference to COM object
//
// returns:
//      pointer of COM object
//
void* SODaSGroup::getMe(void)
{
	if (!isPublicTemplate())
	{
		m_meSync.lock();
		SOSrvComObject<SODaSComGroup> *grp = NULL;

		if (m_me)
		{
			grp = (SOSrvComObject<SODaSComGroup> *)m_me;

			if (grp)
			{
				grp->tempAddRef();
			}
		}

		m_meSync.unlock();
		return (void*)grp;
	}

	return NULL;
} // getMe


//-----------------------------------------------------------------------------
// releaseMe
//
// - release reference to COM object obtained by getMe
//
// returns:
//      temporary reference count of COM object
//
ULONG SODaSGroup::releaseMe(
	IN void* /* me */)    // pointer to COM object
{
	ULONG uRet = 0;

	if (!isPublicTemplate())
	{
		this->addRef();
		m_meSync.lock();
		SOSrvComObject<SODaSComGroup> *grp = (SOSrvComObject<SODaSComGroup> *)m_me;

		if (grp)
		{
			uRet = grp->tempRelease();
		}

		m_meSync.unlock();
		this->release();
	}

	return uRet;
} // releaseMe

#endif

//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaSGroup::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaSGroup::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// setName
//
// - set group name
// - check if name is valid
// - add name to parents child name list
//
// returns:
//      S_OK                - set name
//      E_INVALIDARG        - no valid name or parameter
//      OPC_E_DUPLICATENAME - no unique name
//
HRESULT SODaSGroup::setName(
	IN LPCTSTR name)
{
	HRESULT res;
	res = SOCmnElement::setName(name);

	if (SUCCEEDED(res))
	{
#ifdef SOFEATURE_WATCH

		if (getCollectChanges())
		{
			SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_NAME_T, getName());
		}

#endif
		m_cyclicTransaction->setObjTraceId(NULL);
#ifdef SOFEATURE_DCOM
		SODaSComGroup* grp = (SODaSComGroup*)getMe();

		if (grp)
		{
			grp->setObjTraceId(NULL);
			releaseMe(grp);
		}

#endif
	}

	return res;
} // setName


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//      TRUE  - branch set
//      FALSE - no unique name
//
BOOL SODaSGroup::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOCmnElement> prevPar = getParentBranch();

	if ((prevPar.isNotNull()) && (parent == NULL))
	{
		// remove from tree
		if ((prevPar->getCollectChanges()) || (getCollectChanges()))
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchRemoveObject(prevPar, this);
		}
	}

#endif
	return SOCmnElement::setParentBranch(parent);
} // setParentBranch


//-----------------------------------------------------------------------------
// getItemByHandle
//
// - retrieve item pointer by the server handle value
//
// returns:
//      pointer to the item
//      NULL - no item with this server handle
//
SODaSItem* SODaSGroup::getItemByHandle(IN OPCHANDLE serverHandle)  // server handle
{
	SOCmnList<SOCmnElement> list(getLeafList());
	SOCmnPointer<SODaSItem> pItem = (SODaSItem*)list.findKey(serverHandle);
	pItem.addRef();
	return (SODaSItem*)pItem;
} // getItemByHandle


//-----------------------------------------------------------------------------
// validateItem
//
// - validate item parameters
//
// returns:
//      S_OK                - validated item parameters
//      OPC_E_INVALIDITEMID - invalid item id
//      OPC_E_UNKNOWNITEMID - no namespace tag with the item id
//      OPC_E_BADTYPE       - bad requested data type
//      OPC_E_UNKNOWNPATH   - unknown access path
//
HRESULT SODaSGroup::validateItem(
	IN LPCTSTR id,                  // item id
	IN LPCTSTR accessPath,          // access path
	IN VARTYPE reqDataType,         // requested data type
	OUT VARTYPE* nativeDataType,    // native data type
	OUT DWORD* accessRights,        // access rights
	OUT OPTIONAL SODaSItemTag** ppTag)// tag object
{
	SOCmnPointer<SODaSServer> server = (SODaSServer*)getParentBranch();
	SOCmnPointer<SODaSItemTag> itemTag;
	HRESULT res = S_OK;

	if (!server)
	{
		return E_FAIL;
	}

	if (nativeDataType)
	{
		*nativeDataType = 0;
	}

	if (accessRights)
	{
		*accessRights = 0;
	}

	if (!id)
	{
		return OPC_E_INVALIDITEMID;
	}

	res = server->getItemTag(id, &itemTag);

	if (FAILED(res))
	{
		return res;
	}

	if (nativeDataType)
	{
		*nativeDataType = itemTag->getNativeDatatype();
	}

	if (accessRights)
	{
		*accessRights = itemTag->getAccessRights(server);
	}

	// check data type
	if (FAILED(res = itemTag->supportReqDatatype(reqDataType)))
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; bad requested datatype %u"), reqDataType))
		return res;
	}

	// check access path
	if (!itemTag->checkAccessPath(accessPath))
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't validate item; bad access path %s"), accessPath))
		return OPC_E_UNKNOWNPATH;
	}

	// return the found tag
	if (ppTag)
	{
		*ppTag = itemTag;
		(*ppTag)->addRef();
	}

	return S_OK;
} // validateItem


//-----------------------------------------------------------------------------
// addItem
//
// - add item to group
//
// returns:
//      S_OK                - validated item parameters
//      OPC_E_INVALIDITEMID - invalid item id
//      OPC_E_UNKNOWNITEMID - no namespace tag with the item id
//      OPC_E_BADTYPE       - bad requested data type
//      OPC_E_UNKNOWNPATH   - unknown access path
//
HRESULT SODaSGroup::addItem(
	IN LPCTSTR id,                  // item id
	IN LPCTSTR accessPath,          // access path
	IN BOOL active,                 // activation state
	IN OPCHANDLE clientHandle,      // client handle
	IN VARTYPE reqDataType,         // requested data type
	OUT SODaSItem** item,           // item object
	OUT VARTYPE* nativeDataType,    // native data type
	OUT DWORD* accessRights)        // access rights
{
	SOCmnPointer<SODaSItemTag> tag;
	SODaSItem* itm = NULL;
	HRESULT res;
	res = validateItem(id, accessPath, reqDataType, nativeDataType, accessRights, (SODaSItemTag**)&tag);

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
		SOCmnPointer<SODaSCreator> creator;
		// create OPC tree object
		creator = engine->getCreator();
		itm = creator->createItem(this, tag);

		if (itm == NULL)
		{
			return E_OUTOFMEMORY;
		}

		// add to tree
		addLeaf(itm);
		// set item parameters
		itm->setItemTag(tag, ((active) && (m_active)));
		itm->setReqDatatype(reqDataType);
		itm->setAccessPath(accessPath);
		itm->setClientHandle(clientHandle);

		if (!itm->setActive(active))
		{
			tag->removeConnectedObject(itm);
			removeLeaf(itm);
			itm->release();
			return E_FAIL;
		}

#ifdef SOFEATURE_WATCH

		if (getCollectChanges())
		{
			SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
			entry->watchAddObject(this, itm);
		}

#endif
	}

	if (item != NULL)
	{
		*item = itm;
	}
	else
	{
		if (itm)
		{
			itm->release();
		}
	}

	return res;
} // addItem


//-----------------------------------------------------------------------------
// removeItem
//
// - remove item from the group
//
// returns:
//      S_OK                - removed item
//      OPC_E_INVALIDHANDLE - invalid item handle
//
HRESULT SODaSGroup::removeItem(IN OPCHANDLE serverHandle)  // server handle
{
	// get item
	SOCMN_TRY
	{
		SOCmnPointer<SODaSItem> pItem = getItemByHandle(serverHandle);

		if (!pItem)
		{
			return OPC_E_INVALIDHANDLE;
		}

		pItem->setActive(FALSE);

		SOCmnPointer<SODaSItemTag> itemTag = pItem->getItemTag();

		if (itemTag.isNotNull())
		{
			pItem->setItemTag(NULL, FALSE);
		}

		removeLeaf(pItem);

		// check the servers transaction list for outstanding requests to the item
		SOCmnPointer<SODaSServer> server = (SODaSServer*)getParentBranch();

		if (server.isNotNull())
		{
			SOCmnList<SODaSTransaction> tacList(server->getTransactionList());
			SODaSTransaction* pTac;
			SOCmnListPosition posTac;
			SOCmnList<SODaSRequest> reqList;
			SOCmnListPosition posReq;
			SODaSRequest* pReq;
			SOCmnPointer<SODaSItem> reqItem;
			SOCmnPointer<SODaSGroup> tacGrp;
			posTac = tacList.getStartPosition();

			while (posTac)
			{
				pTac = tacList.getNext(posTac);

				if (pTac->getType() == SODaSTransaction::cyclic)
				{
					continue;
				}

				tacGrp = pTac->getGroup();

				if ((SODaSGroup*)tacGrp == this)
				{
					reqList = pTac->getRequestList();
					posReq = reqList.getStartPosition();

					while (posReq)
					{
						pReq = reqList.getNext(posReq);
						reqItem = pReq->getItem();

						if ((SODaSItem*)pItem == (SODaSItem*)reqItem)
						{
							if (pReq->isStarted())
							{
								pTac->notiofyCompletedInternally(pReq);
								pReq->complete(OPC_E_INVALIDHANDLE);
							}
							else
							{
								// complete request before it was started
								pReq->complete(OPC_E_INVALIDHANDLE, FALSE, OPC_QUALITY_UNCERTAIN, TRUE);
							}
						}
					}
				}
			}
		}

		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		return OPC_E_INVALIDHANDLE;
	}
} // removeItem


//-----------------------------------------------------------------------------
// clone
//
// - creates a copy of the group
//
// returns:
//      S_OK                - created new group
//      E_INVALIDARG        - group name not valid
//      OPC_E_DUPLICATENAME - group name not unique
//
HRESULT SODaSGroup::clone(
	IN SODaSServer* server, // server object of clone
	IN LPCTSTR name,        // group name
	OUT SODaSGroup** clone) // created clone
{
	HRESULT res;
	LONG timeBias;
#ifdef SOFEATURE_SOAP
	timeBias = 0;
#endif
#ifdef SOFEATURE_DCOM
	timeBias = getTimeBias();
#endif
	FLOAT deadband = getDeadBand();
	BOOL createPubTempl;
	OPCHANDLE clientHandle = 0;

	if (!server)
	{
		return E_FAIL;
	}

	// creating a public template by moveToPublic?
	// -> set client handles to NULL
	createPubTempl = server->isPublicGroups();

	if (!createPubTempl)
	{
		clientHandle = getClientHandle();
	}

	// create tree group object
	res = server->addGroup(name, FALSE, getUpdateRate(), clientHandle,
						   &timeBias, &deadband, getLCID(), clone, NULL, FALSE);

	if (SUCCEEDED(res))
	{
		// cloning a public template?
		if (isPublicTemplate())
		{
			// -> creates a public group
			(*clone)->m_publicTemplate = this;
		}

		// clone all items
		SOCmnList<SOCmnElement> list(getLeafList());
		SOCmnPointer<SODaSItemTag> tag;
		SODaSItem* item;
		SODaSItem* clonedItem;
		SOCmnListPosition pos;
		VARTYPE canDT;      // native data type
		DWORD accessRights; // access rights
		// for all items of the group
		pos = list.getStartPosition();

		while (pos)
		{
			item = (SODaSItem*)list.getNext(pos);
			tag = item->getItemTag();

			if (!createPubTempl)
			{
				clientHandle = item->getClientHandle();
			}

			if (SUCCEEDED((*clone)->addItem(tag->getItemID(), item->getAccessPath(), item->getActive(),
											clientHandle, item->getReqDatatype(), &clonedItem,
											&canDT, &accessRights)))
			{
				clonedItem->release();
			}
		}
	}

	return res;
} // clone


//-----------------------------------------------------------------------------
// isPublicGroup
//
// - checks if the group is a public group (has public group template)
//
// returns:
//      TRUE  - public group
//      FALSE - private group
//
BOOL SODaSGroup::isPublicGroup(void) const
{
	return (NULL != m_publicTemplate);
} // isPublicGroup


//-----------------------------------------------------------------------------
// isPublicTemplate
//
// - checks if the group is a public group template (parent is public groups server)
//
// returns:
//      TRUE  - public group template
//      FALSE - no public group template
//
BOOL SODaSGroup::isPublicTemplate(void)
{
	SOCmnPointer<SODaSServer> parent = (SODaSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		return parent->isPublicGroups();
	}

	return FALSE;
} // isPublicTemplate


//-----------------------------------------------------------------------------
// isPublic
//
// - checks if the group is a public group or a public group template
//
// returns:
//      TRUE  - public group
//      FALSE - private group
//
BOOL SODaSGroup::isPublic(void)
{
	if (isPublicGroup())
	{
		return TRUE;
	}

	return isPublicTemplate();
} // isPublic


//-----------------------------------------------------------------------------
// addCyclicRequest
//
// - add a request to cyclic transaction of the items group
//
// returns:
//      TRUE  - string matches filter
//      FALSE - string don't matches filter
//
BOOL SODaSGroup::addCyclicRequest(IN SODaSItem* item) // item object
{
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)getParentBranch();

	if (!srv)
	{
		return FALSE;
	}

	if (srv->isPublicGroups())
	{
		return FALSE;
	}

	if (!item)
	{
		return FALSE;
	}

	item->createCyclicData();
	SOCmnPointer<SODaSItemTag> tag;
	BOOL ret = TRUE;
	tag = item->getItemTag();
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();

	if (tag->getUpdateMode() == SODaSItemTag::poll)
	{
		SOCmnPointer<SODaSRequest> req;
		SOCmnPointer<SODaSTransaction> tac = getCyclicTransaction();
		req = item->getCyclicRequest();

		if (req.isNotNull())
		{
			return FALSE;
		}

		// add cyclic request to the group
		req = creator->createRequest(srv, item, tag, SODaSRequest::read, SODaSRequest::cyclic);

		if (req.isNotNull())
		{
			if (srv->m_minRequestLoopTime == INFINITE)
			{
				req->setCompletedEvent(srv->getWorkEvent());
			}

			req->setItem(item);
			SOCmnSingleLock<SOCmnSync> lock(tac->getRequestListLock());
			item->setCyclicRequest(req);
			ret = tac->insertRequest(req);
		}

		if (getUpdateRate() == 0)
		{
			// not notify all changes with update rate 0
			m_notifiyAllChanges = FALSE;
		}
	}

	return ret;
} // addCyclicRequest


//-----------------------------------------------------------------------------
// removeCyclicRequest
//
// - remove a request from cyclic transaction of the items group
//
// returns:
//      TRUE  - string matches filter
//      FALSE - string don't matches filter
//
BOOL SODaSGroup::removeCyclicRequest(IN SODaSItem* item) // item
{
	SOCmnPointer<SODaSItem> pLocalItem = item;
	pLocalItem.addRef();
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)getParentBranch();

	if (!srv || srv->isPublicGroups() || !pLocalItem)
	{
		return FALSE;
	}

	BOOL ret = FALSE;
	SOCmnPointer<SODaSRequest> req = pLocalItem->getCyclicRequest();

	if (req.isNotNull())
	{
		SOCmnPointer<SODaSTransaction> tac = getCyclicTransaction();
		SOCmnSingleLock<SOCmnSync> lock(tac->getRequestListLock());
		BOOL removeSuceeded = tac->removeRequest(req);
		lock.unlock();

		if (removeSuceeded != TRUE)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Could not remove request [0x%X] from transaction [0x%X]"), (unsigned int)(SODaSRequest*)req, (unsigned int)(SODaSTransaction*)tac))
		}

		req.release();
	}

	pLocalItem->setCyclicRequest(NULL);
	pLocalItem->destroyCyclicData();
	return ret;
} // removeCyclicRequest


//-----------------------------------------------------------------------------
// isNotifyAllChanges
//
// - check if to notify all changes to the client
//
// This method can only return TRUE for zero update rate group to be compliant
// to the OPC specification. The group must include only report updated items
// if returning TRUE
//
// returns:
//      TRUE  - notify all changes
//
BOOL SODaSGroup::isNotifyAllChanges(void)
{
	return m_notifiyAllChanges;
} // isNotifyAllChanges


void SODaSGroup::calcNotifyAllChanges(void)
{
	// recalculate notifyAllChanges flag
	SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)getLeafList());
	SOCmnListPosition pos;
	SODaSItem* pItem;
	SOCmnPointer<SODaSItemTag> itemTag;
	pos = itemList.getStartPosition();

	while (pos)
	{
		pItem = itemList.getNext(pos);
		itemTag = pItem->getItemTag();

		if (itemTag->getUpdateMode() != SODaSItemTag::report)
		{
			m_notifiyAllChanges = FALSE;
			return;
		}
	}

	m_notifiyAllChanges = TRUE;
}

//-----------------------------------------------------------------------------
// setActive
//
// - set activation state
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setActive(
	IN BOOL active) // activation state
{
	BOOL prevActive = m_active;

	if (active)
	{
		active = TRUE;    // set 'NOT FALSE(!=0)' to 'TRUE(1)'
	}

	onSetActive(active);

	// check if to activate update rate mechanism
	if ((active) && (!prevActive))
	{
		// group was activated
		// -> start update rate mechanism
		m_initTime = ::GetTickCount();
		m_initCnt = 0;
	}

	m_active = (BYTE)active;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, m_active));
	}

#endif

	// check if to activate update rate mechanism
	if ((active) && (!prevActive))
	{
		// group was activated
		{
			// notify item tag
			SOCmnList<SODaSItem> itemList = getItemList();
			SOCmnListPosition pos;
			SODaSItem* pItem;
			SOCmnPointer<SODaSItemTag> itemTag;
			pos = itemList.getStartPosition();

			while (pos)
			{
				pItem = itemList.getNext(pos);

				if (pItem->getActive())
				{
					itemTag = pItem->getItemTag();

					if (itemTag.isNotNull())
					{
						itemTag->activateConnectedObject(pItem);
					}
				}
			}
		}
		SOCmnPointer<SODaSServer> parent = (SODaSServer*)getParentBranch();

		if (parent.isNotNull())
		{
			parent->signalWorkEvent();    // signal async thread
		}
	}
	else if ((!active) && (prevActive))
	{
		// notify item tag
		SOCmnList<SODaSItem> itemList = getItemList();
		SOCmnListPosition pos;
		SODaSItem* pItem;
		SOCmnPointer<SODaSItemTag> itemTag;
		pos = itemList.getStartPosition();

		while (pos)
		{
			pItem = itemList.getNext(pos);

			if (pItem->getActive())
			{
				itemTag = pItem->getItemTag();

				if (itemTag.isNotNull())
				{
					itemTag->deactivateConnectedObject(pItem);
				}
			}
		}
	}

	return TRUE;
} // setActive


//-----------------------------------------------------------------------------
// setUpdateRate
//
// - set update rate
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setUpdateRate(
	IN DWORD updateRate)    // update rate
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();

	if (updateRate < engine->getMinUpdateRate())
	{
		updateRate = engine->getMinUpdateRate();
	}

	if (updateRate > SOCMN_TIME_40DAYS)
	{
		updateRate = SOCMN_TIME_40DAYS; // max update rate 40 days
	}

	if (updateRate == 0)
	{
		calcNotifyAllChanges();
	}
	else if ((updateRate != 0) && (m_updateRate == 0))
	{
		m_notifiyAllChanges = FALSE;
	}

	onSetUpdateRate(updateRate);
	m_updateRate = updateRate;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_UPDATERATE_T, buffer.format(_T("%lu"), m_updateRate));
	}

#endif
	// signal work event
	SOCmnPointer<SODaSServer> parent = (SODaSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		parent->signalWorkEvent();
	}

	return TRUE;
} // setUpdateRate


void SODaSGroup::recalcRevUpdateRate(void)
{
}

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setKeepAliveTime
//
// - set keep alive time
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setKeepAliveTime(
	IN DWORD keepAlive) // keep alive time
{
	if (keepAlive != 0)
	{
		SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();

		if (keepAlive < engine->getMinKeepAliveTime())
		{
			keepAlive = engine->getMinKeepAliveTime();
		}

		if (keepAlive > SOCMN_TIME_40DAYS)
		{
			keepAlive = SOCMN_TIME_40DAYS;  // max keep alive time 40 days
		}
	}

	onSetKeepAliveTime(keepAlive);
	m_keepAliveTime = keepAlive;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_KEEPALIVETIME_T, buffer.format(_T("%lu"), m_keepAliveTime));
	}

#endif
	// signal work event
	SOCmnPointer<SODaSServer> parent = (SODaSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		parent->signalWorkEvent();
	}

	return TRUE;
} // setKeepAliveTime

#endif
#ifdef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// setPingRate
//
// - set keep alive time
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setPingRate(
	IN DWORD pingRate)  // keep alive time
{
	if (pingRate == 0)
	{
		if (m_updateRate > 10)
		{
			pingRate = m_updateRate * 10;
		}
		else
		{
			pingRate = 100;
		}
	}

	if (pingRate > SOCMN_TIME_40DAYS)
	{
		pingRate = SOCMN_TIME_40DAYS;   // max ping rate 40 days
	}

	onSetPingRate(pingRate);
	m_pingRate = pingRate;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_PINGRATE_T, buffer.format(_T("%lu"), m_pingRate));
	}

#endif
	// signal work event
	SOCmnPointer<SODaSServer> parent = (SODaSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		parent->signalWorkEvent();
	}

	return TRUE;
} // setPingRate

#endif

//-----------------------------------------------------------------------------
// setClientHandle
//
// - set client handle
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setClientHandle(
	IN OPCHANDLE clientHandle)  // client handle
{
	onSetClientHandle(clientHandle);
	m_clientHandle = clientHandle;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_CLIENTHANDLE_T, buffer.format(_T("%lu"), m_clientHandle));
	}

#endif
	return TRUE;
} // setClientHandle


#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// setTimeBias
//
// - set time zone
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setTimeBias(
	IN LONG timeBias)   // time zone
{
	onSetTimeBias(timeBias);
	m_timeBias = timeBias;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_TIMEBIAS_T, buffer.format(_T("%ld"), m_timeBias));
	}

#endif
	return TRUE;
} // setTimeBias
#endif

//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setDeadBand(
	IN FLOAT deadBand)  // deadband
{
	onSetDeadBand(deadBand);
	m_deadBand = deadBand;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_DEADBAND_T, buffer.format(_T("%f"), m_deadBand));
	}

#endif
	return TRUE;
} // setDeadBand


//-----------------------------------------------------------------------------
// setLCID
//
// - set local Id
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setLCID(
	IN DWORD lcid)  // local Id
{
	onSetLCID(lcid);
	m_lcid = lcid;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_LCID_T, conv->LCID2string(buffer, m_lcid));
	}

#endif
	return TRUE;
} // setLCID


//-----------------------------------------------------------------------------
// setEnable
//
// - set enable
//
// returns:
//      TRUE
//
BOOL SODaSGroup::setEnable(
	IN BOOL enable) // enable
{
	onSetEnable(enable);
	m_enable = (BYTE)enable;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ENABLE_T, conv->bool2string(buffer, m_enable));
	}

#endif
	return TRUE;
} // setEnable


//-----------------------------------------------------------------------------
// onSetActive
//
// - called before activation state is set
// - default implementation: do nothing
//
void SODaSGroup::onSetActive(
	IN BOOL /* active */)  // activation state
{} // onSetActive


//-----------------------------------------------------------------------------
// onSetUpdateRate
//
// - called before update rate is set
// - default implementation: do nothing
//
void SODaSGroup::onSetUpdateRate(
	IN DWORD /* updateRate */)    // update rate
{} // onSetUpdateRate


//-----------------------------------------------------------------------------
// onSetKeepAliveTime
//
// - called before keep alive time is set
// - default implementation: do nothing
//
void SODaSGroup::onSetKeepAliveTime(
	IN DWORD /* keepAlive */) // keep alive time
{} // onSetKeepAliveTime

//-----------------------------------------------------------------------------
// onSetPingRate
//
// - called before ping rate is set
// - default implementation: do nothing
//
void SODaSGroup::onSetPingRate(
	IN DWORD /* pingRate */)  // ping rate
{} // onSetPingRate

//-----------------------------------------------------------------------------
// onSetClientHandle
//
// - called before client handle is set
// - default implementation: do nothing
//
void SODaSGroup::onSetClientHandle(
	IN OPCHANDLE /* clientHandle */)  // client handle
{} // onSetClientHandle


#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// onSetTimeBias
//
// - called before time zone is set
// - default implementation: do nothing
//
void SODaSGroup::onSetTimeBias(
	IN LONG /* timeBias */)   // time zone
{} // onSetTimeBias
#endif

//-----------------------------------------------------------------------------
// onSetDeadBand
//
// - called before deadband is set
// - default implementation: do nothing
//
void SODaSGroup::onSetDeadBand(
	IN FLOAT /* percentDeadBand */)   // deadband
{} // onSetDeadBand


//-----------------------------------------------------------------------------
// onSetLCID
//
// - called before local id is set
// - default implementation: do nothing
//
void SODaSGroup::onSetLCID(
	IN DWORD /* lcid */)
{} // onSetLCID


//-----------------------------------------------------------------------------
// onSetEnable
//
// - called before enable is set
// - default implementation: do nothing
//
void SODaSGroup::onSetEnable(
	IN BOOL /* enable */)
{} // onSetEnable


//-----------------------------------------------------------------------------
// initSpan
//
// - initialize time span till next update is needed
//
DWORD SODaSGroup::initSpan(
	IN DWORD toLate)    // to late called
{
	if (m_updateRate < 200000)
	{
		// don't correct timer if update rate is bigger than 3 minutes
		DWORD initCnt = m_initCnt;
		// correct bad NT Timer (~11 ms interval)
		initCnt += ((m_updateRate * 150) >> 10);

		if (initCnt < 1000)
		{
			m_correctNTtimer = 0;
		}
		else
		{
			DWORD correctNTtimer = m_correctNTtimer;
			correctNTtimer = 10 * (initCnt >> 10);

			if (correctNTtimer < 0xFF)
			{
				m_correctNTtimer = (BYTE)correctNTtimer;
			}
			else
			{
				m_correctNTtimer = 0xFF;
			}

			initCnt = initCnt & 0x3ff;
		}

		m_initCnt = (WORD)initCnt;
		toLate += m_correctNTtimer;
	}

	m_initTime = ::GetTickCount();

	if (toLate < m_updateRate)
	{
		return m_updateRate - toLate;
	}
	else
	{
		return m_updateRate;
	}
} // initSpan


//-----------------------------------------------------------------------------
// updateSpan
//
// - update time span till next request start
//
// returns:
//      TRUE  - time span ellapsed
//      FALSE - time span not ellapsed
//
BOOL SODaSGroup::updateSpan(
	IN DWORD now,   // current time
	IN DWORD* span) // time till next group update
{
	DWORD newSpan;

	if (!m_active)
	{
		if (span)
		{
			*span = INFINITE;
		}

		return FALSE;
	}

	// check for immediate update rate
	if (m_updateRate < 10)
	{
		if (span)
		{
			*span = INFINITE;
		}

		if (isNotifyAllChanges())
		{
			return TRUE;
		}
		else
		{
			SOCmnPointer<SODaSTransaction> cycTrans;
			cycTrans = getCyclicTransaction();
			return !cycTrans->isStarted();
		}
	}

	DWORD ellapsed = getTimeSpan(m_initTime, now);
	DWORD updateRate = m_updateRate - m_correctNTtimer;
	BOOL fire = FALSE;

	if (ellapsed < updateRate)
	{
		newSpan = updateRate - ellapsed;
	}
	else    // span ellapsed
	{
		DWORD toLate = ellapsed - updateRate;
		newSpan = initSpan(toLate);
		fire = TRUE;
	}

	if (span)
	{
		*span = newSpan;
	}

	return fire;
} // updateSpan


//-----------------------------------------------------------------------------
// isRemoved
//
// - check if group is already removed
//
// returns:
//      TRUE  - group already removed
//      FALSE - group not removed
//
BOOL SODaSGroup::isRemoved(void)
{
	// object marked as removed
	if (m_removed)
	{
		return TRUE;
	}

	if (isPublicGroup())
	{
		// public group check if template of group is removed
		return m_publicTemplate->isRemoved();
	}

	return FALSE;
} // isRemoved


//-----------------------------------------------------------------------------
// getServerHandle
//
// - get server handle of group
//
// returns:
//      server handle of group
//
OPCHANDLE SODaSGroup::getServerHandle(void) const
{
	if (!isPublicGroup())
	{
		return m_serverHandle;
	}
	else
	{
		return m_publicTemplate->m_serverHandle;
	}
} // getServerHandle


//-----------------------------------------------------------------------------
// trustClientItemHandles
//
// - check client item handles on COM calls
//
// returns:
//      TRUE  - not check
//      FALSE - check
//
BOOL SODaSGroup::trustClientItemHandles(void)
{
	return TRUE;    // trustClientItemHandles
}


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//      E_NOTIMPL     - no query interface implementation for this interface
//      S_OK          - interface delivered
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaSGroup::queryInterface(
	IN REFIID /* iid */,      // interface id
	IN void** /* object */)   // interface pointer
{
	return E_NOTIMPL;    // queryInterface
}

#endif

void SODaSGroup::resetSendValues(void)
{
	SOCmnListPosition posI;
	SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)getLeafList());
	SODaSItem* pItem;
	SOCmnVariant emptyValue;
	size_t connection = 0;
#ifdef SOFEATURE_DCOM
	SOCmnListPosition posA;
	posA = getAdviseStartPositionAndLock(SODASDATACALLBACK_CT_ALL);
	{
		SODaSDataCallback advise;

		while (posA)
		{
			getNextAdvise(posA, advise, SODASDATACALLBACK_CT_ALL);
#endif
			posI = itemList.getStartPosition();

			while (posI)
			{
				pItem = itemList.getNext(posI);
#ifdef SOFEATURE_DCOM

				if (advise.m_connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT) // V 2.0
				{
					connection = (size_t)advise.m_dataCallback;
				}
				else // V 1.0
				{
					connection = advise.m_connectionId;
				}

#endif
				pItem->setSendValue(connection, emptyValue, 0);
			}

#ifdef SOFEATURE_DCOM
		}
	}
	doAdviseUnlock();
#endif
}



//-----------------------------------------------------------------------------
// copyCyclicTransaction
//
// - creates an transaction and copies the requests of the cyclic transaction
//
void SODaSGroup::copyCyclicTransaction(
	IN DWORD dwConnection,       // connection id
	IN SODaSRequest::requestType reqType,   // request type
	IN DWORD maxAge,             // maximum cache age
	IN BYTE connectionType,      // connection type
	OUT SODaSTransaction** dest) // copied transaction
{
	SOCmnPointer<SODaSTransaction> source = getCyclicTransaction();
	SOCmnList<SODaSRequest> destList;
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)getParentBranch();
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SODaSRequest* destReq;
	SOCmnPointer<SODaSItemTag> tag;
	SOCmnListPosition pos;
	*dest = creator->createTransaction(this, SODaSTransaction::refresh, connectionType, dwConnection);
	destList = ((*dest)->getRequestList());
	SOCmnList<SOCmnElement> itemList(getLeafList());
	SODaSItem* item;
	destList.allocateBlock(itemList.getCount());
	// create requests for all active items
	pos = itemList.getStartPosition();

	while (pos)
	{
		item = (SODaSItem*)itemList.getNext(pos);

		if (item->getActive())
		{
			tag = item->getItemTag();
			destReq = creator->createRequest(srv, item, tag, SODaSRequest::read, (WORD)reqType);

			if (destReq != NULL)
			{
				if (reqType == SODaSRequest::asyncMaxAge)
				{
					destReq->setMaxAge(maxAge);
				}

				destReq->setItem(item);
				destReq->setCompletedEvent(srv->getWorkEvent());
				destList.add(destReq);
				destReq->release();
			}
		}
	}
} // copyCyclicTransaction


//-----------------------------------------------------------------------------
// onDataChange
//
// - called after data change callback is called on the advise
//
void SODaSGroup::onDataChange(
	IN SODaSTransaction* /* transaction */,   // transaction
	IN SODaSDataCallback* /* advise */)       // advise
{} // onDataChange


//-----------------------------------------------------------------------------
// onReadComplete
//
// - called after read complete callback is called on the advise
//
void SODaSGroup::onReadComplete(
	IN SODaSTransaction* /* transaction */,   // transaction
	IN SODaSDataCallback* /* advise */)       // advise
{} // onReadComplete


//-----------------------------------------------------------------------------
// onWriteComplete
//
// - called after write complete callback is called on the advise
//
void SODaSGroup::onWriteComplete(
	IN SODaSTransaction* /* transaction */,   // transaction
	IN SODaSDataCallback* /* advise */)       // advise
{} // onWriteComplete


//-----------------------------------------------------------------------------
// onCancelComplete
//
// - called after cancel complete callback is called on the advise
//
void SODaSGroup::onCancelComplete(
	IN SODaSTransaction* /* transaction */,   // transaction
	IN SODaSDataCallback* /* advise */)       // advise
{} // onCancelComplete

void SODaSGroup::onAddItems(void)
{}

void SODaSGroup::onRemoveItems(void)
{}

void SODaSGroup::onChangeStateItems(void)
{}

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSGroup::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_GROUP);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, getActive()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_UPDATERATE_T, getUpdateRate());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEADBAND_T, conv->variant2string(buffer, SOCmnVariant(getDeadBand())));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_LCID_T, conv->LCID2string(buffer, getLCID()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SERVERHANDLE_T, getServerHandle());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLIENTHANDLE_T, getClientHandle());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ENABLE_T, conv->bool2string(buffer, getEnable()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PUBLIC_T, conv->publicState2string(buffer, isPublic(), isPublicGroup()));
#ifdef SOFEATURE_DCOM
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_TIMEBIAS_T, buffer.format(_T("%li"), getTimeBias()));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_KEEPALIVETIME_T, buffer.format(_T("%lu"), getKeepAliveTime()));
#endif
#ifdef SOFEATURE_SOAP
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PINGRATE_T, buffer.format(_T("%lu"), getPingRate()));
#endif
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#ifdef SOFEATURE_SOAP

BOOL SODaSGroup::hasPolledRefreshTransactions(void)
{
	SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)getLeafList());
	SODaSItem* pItem;
	pItem = itemList.getHead();

	if (pItem != NULL)
	{
		if (pItem->getRefCount() > 3)
		{
			return TRUE;
		}
		else
		{
			// group + cyclic request + polledRefresh requests
			// items of the group are used in a polledRefresh transaction
			SOCmnPointer<SODaSRequest> pRequest = pItem->getCyclicRequest();

			if (pRequest.isNotNull() == TRUE)
			{
				return FALSE;
			}

			//not cyclic
			if (pItem->getRefCount() > 2)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

DWORD SODaSGroup::cancelPolledRefreshTransactions(void)
{
	// clean up polledRefresh transactions to this subscription
	DWORD tacCnt = 0;

	if (hasPolledRefreshTransactions())
	{
		// signal all server objects with polledRefresh transactions -> maybe the end of some
		SOCmnPointer<SODaSEntry> entryDA = ::getSODaSEntry();
		SOCmnPointer<SOSrvObjectRoot> oRoot = entryDA->getObjectRoot();
		SOCmnList<SODaSServer> serverList((SOCmnList<SODaSServer> *)oRoot->getBranchList());
		SOCmnListPosition posS;
		SODaSServer* pS;
		SOCmnList<SODaSTransaction> tacList;
		SOCmnListPosition posT;
		SODaSTransaction* pT;
		SOCmnList<SODaSRequest> reqList;
		SOCmnListPosition posR, posRC, posRP;
		SODaSRequest* pR;
		SOCmnPointer<SODaSItem> itemR;
		SOCmnPointer<SODaSGroup> grpR;
		BOOL signalSWE;
		BOOL insertedISH;
		posS = serverList.getStartPosition();

		while (posS)
		{
			signalSWE = FALSE;
			pS = serverList.getNext(posS);
			tacList = pS->getTransactionList();
			posT = tacList.getStartPosition();

			while (posT)
			{
				insertedISH = FALSE;
				pT = tacList.getNext(posT);

				if (pT->getType() == SODaSTransaction::polledRefresh)
				{
					reqList = pT->getRequestList();
					posRC = NULL;
					posR = reqList.getStartPosition();

					while (posR)
					{
						posRP = posRC;
						posRC = posR;
						pR = reqList.getNext(posR);
						itemR = pR->getItem();

						if (itemR.isNotNull())
						{
							grpR = (SODaSGroup*)itemR->getParentBranch();

							if ((SODaSGroup*)grpR == (SODaSGroup*)this)
							{
								// request of the removed group
								signalSWE = TRUE;

								if (!insertedISH)
								{
									// insert invalid server sub handle to transaction
									SOCmnPointer<SODaSXmlMethod> mthT = pT->getXMLMethod();
									SOCmnStringEx szInvalSubH;
									szInvalSubH.format(_T("<InvalidServerSubHandles>0x%8.8lX</InvalidServerSubHandles>"), (DWORD)this->getOpcHandle());
									mthT->m_methodResults += szInvalSubH;
									insertedISH = TRUE;
									tacCnt++;
								}

								// remove request
								pR->cancel(S_OK);
								reqList.removeAt(posRC, posRP);
								posRC = posRP;
							}
						}
					}

					if (reqList.getCount() == 0)
					{
						// removed all requests
						// -> end polledRefresh
						pT->setWaitTime(GetTickCount());
						pT->setHoldTime(GetTickCount());
					}

					reqList.autoUnlock();
				}
			}

			tacList.autoUnlock();

			if (signalSWE)
			{
				pS->signalWorkEvent();
			}
		}
	}

	return tacCnt;
}
#endif

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// checkAuthorizationDCOM
//
// - check if the call to the COM interface function is authorizated
//
// returns:
//   S_OK           - allowed access
//   E_ACCESSDENIED - denied access
//
HRESULT SODaSGroup::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
} // checkAuthorizationDCOM
#endif

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// createTransaction
//
// - create transaction object, used for the asynchronous calls
//
// returns:
//      S_OK    - the transaction includes all items
//      S_FALSE - at least one item is not in the transaction
//
HRESULT SODaSGroup::createTransaction(
	IN BOOL read,                       // read or write transaction?
	IN BYTE connectionType,             // connection type (DA 1.0 or DA 2.0)
	IN DWORD connection,                // connection id
	IN DWORD source,                    // data source: cache or device or 0 (maxAge)
	IN SOCmnEventPointer reqCompEvent,  // completion event for requests
	IN DWORD numItems,                  // number of items
	IN OPCHANDLE* serverHandles,        // item handles
	IN VARIANT* itemValues,             // item write values
	IN OPCITEMSTATE** readItemValues,   // read values (DA 2.0)
	IN DWORD* pMaxAge,                  // max age of cache
	IN VARIANT** ppValues,              // read values (DA 3.0)
	IN WORD** ppQualities,              // read qualities
	IN FILETIME** ppTimeStamps,         // read timestamps
	IN OPCITEMVQT* pItemVQT,            // write values, qualities and timestamp (DA 3.0)
	OUT SODaSTransaction** transaction, // created transaction
	OUT DWORD* transactionID,           // transaction id
	OUT HRESULT** errors)               // errors
{
	SODaSTransaction::transactionType type;
	SODaSRequest::requestOperation reqOp;
	SODaSRequest::requestType reqType;
	HRESULT res;
	DWORD accessRight;

	if (source == OPC_DS_CACHE)
	{
		// cache
		if (connectionType == SODaSTransaction::synchronous)
		{
			reqType = SODaSRequest::syncCache;
		}
		else
		{
			reqType = SODaSRequest::asyncCache;
		}
	}
	else
	{
		// device or maxage
		if (!pMaxAge)
		{
			if (connectionType == SODaSTransaction::synchronous)
			{
				reqType = SODaSRequest::syncDevice;
			}
			else
			{
				reqType = SODaSRequest::asyncDevice;
			}
		}
		else
		{
			if (connectionType == SODaSTransaction::synchronous)
			{
				reqType = SODaSRequest::syncMaxAge;
			}
			else
			{
				reqType = SODaSRequest::asyncMaxAge;
			}
		}
	}

	if (read)
	{
		if (connectionType == SODaSTransaction::synchronous)
		{
			type = SODaSTransaction::syncRead;
		}
		else
		{
			type = SODaSTransaction::asyncRead;
		}

		reqOp = SODaSRequest::read;
		accessRight = OPC_READABLE;
	}
	else
	{
		if (connectionType == SODaSTransaction::synchronous)
		{
			type = SODaSTransaction::syncWrite;
		}
		else
		{
			type = SODaSTransaction::asyncWrite;
		}

		reqOp = SODaSRequest::write;
		accessRight = OPC_WRITEABLE;
	}

	SODaSTransaction* tac;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	tac = creator->createTransaction(this, (BYTE)type, connectionType, connection);

	if (tac != NULL)
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(numItems);
		// create request list
		res = fillRequestList(reqType, reqOp, accessRight, reqCompEvent, numItems,
							  serverHandles, itemValues, (connectionType != SODaSTransaction::dataObject),
							  readItemValues, pMaxAge, ppValues, ppQualities, ppTimeStamps, pItemVQT,
							  tac->getRequestList(), errors);

		if (SUCCEEDED(res))
		{
			if (transactionID)
			{
				*transactionID = tac->getTransactionId();
			}

			*transaction = tac;
		}
		else
		{
			if (transactionID)
			{
				*transactionID = 0;
			}

			*transaction = NULL;
			tac->release(); // free transaction
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // createTransaction


//-----------------------------------------------------------------------------
// fillRequestList
//
// - fill request list, used for all I/O calls
//
// returns:
//      S_OK    - the request list includes all items
//      S_FALSE - at least one item is not in the request list
//
HRESULT SODaSGroup::fillRequestList(
	IN SODaSRequest::requestType reqType,   // request type
	IN SODaSRequest::requestOperation reqOp, // request operation
	IN DWORD accessRight,                 // needed access rights
	IN SOCmnEventPointer reqCompEvent,    // completion event for requests
	IN DWORD numItems,                    // number of items
	IN OPCHANDLE* serverHandles,          // item handles
	IN VARIANT* writeItemValues,          // write values (write only)
	IN BOOL checkForErrors,               // check for errors when filling the list
	IN OPCITEMSTATE** readItemValues,     // read values (DA 2.0)
	IN DWORD* pMaxAge,                    // max age of cache
	IN VARIANT** ppValues,                // read values (DA 3.0)
	IN WORD** ppQualities,                // read qualities
	IN FILETIME** ppTimeStamps,           // read timestamps
	IN OPCITEMVQT* pItemVQT,              // write values, qualities and timestamp (DA 3.0)
	OUT SOCmnList<SODaSRequest> *reqList, // request list
	OUT HRESULT** errors)                 // errors
{
	HRESULT res = S_OK;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSItem> item;
	SOCmnPointer<SODaSItemTag> tag;
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)getParentBranch();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	DWORD i;
	SOCmnPointer<SODaSRequest> req;
	BOOL addReqToList;

	// create request list
	for (i = 0; i < numItems; i++)
	{
		// get item
		SOCMN_TRY
		{
			item = getItemByHandle(serverHandles[i]);
		} 
		SOCMN_CATCH(...)
		{
			item = NULL;
		}

		if ((SODaSItem*)item)
		{
			// check access rights
			tag = item->getItemTag();

			if ((!checkForErrors) || ((tag->getAccessRights(srv) & accessRight) == accessRight))
			{
				// valid access rights or do not check the access rights
				// create request
				(*errors)[i] = S_OK;
				req = creator->createRequest(srv, item, tag, (BYTE)reqOp, (WORD)reqType);

				if (req.isNotNull())
				{
					req->setItem(item);
					addReqToList = TRUE;

					switch (reqType)
					{
					case SODaSRequest::syncCache:
					case SODaSRequest::syncDevice:
					case SODaSRequest::syncMaxAge:
					{
						req->setCompletedEvent(reqCompEvent);
						req->setResultAddress((*errors) + i);
						req->setResult(S_OK);

						if (reqOp == SODaSRequest::read)
						{
							if (reqType != SODaSRequest::syncMaxAge)
							{
								req->setItemStateAddress((*readItemValues) + i);    // cache and device
							}
							else
							{
								// maxage
								req->setMaxAge(pMaxAge[i]);
								req->setReadValueAddress((*ppValues) + i);
								req->setReadQualityAddress((*ppQualities) + i);
								req->setReadTimeStampAddress((*ppTimeStamps) + i);
							}
						}
						else
						{
							if (writeItemValues)
							{
								// DA 1.0 and DA 2.0
								if (!req->setWriteValue(&writeItemValues[i]))
								{
									addReqToList = FALSE;
									res = S_FALSE;
								}
							}
							else
							{
								// DA 3.0
								BYTE writeQT = 0;

								if (pItemVQT[i].bQualitySpecified)
								{
									writeQT |= SODASREQUEST_FLAG_QUALITY;
								}

								if (pItemVQT[i].bTimeStampSpecified)
								{
									writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
								}

								if (writeQT == 0)
								{
									req->setWriteValue(&(pItemVQT[i].vDataValue));
								}
								else
								{
									if (tag->supportWriteQT(writeQT))
									{
										if (req->setWriteValue(&(pItemVQT[i].vDataValue)))
										{
											if (pItemVQT[i].bQualitySpecified)
											{
												req->setWriteQuality(pItemVQT[i].wQuality);
											}

											if (pItemVQT[i].bTimeStampSpecified)
											{
												req->setWriteTimeStamp(SOCmnDateTime(&pItemVQT[i].ftTimeStamp));
											}
										}
									}
									else
									{
										req->setResult(OPC_E_NOTSUPPORTED);
									}
								}

								if (FAILED(req->getResult()))
								{
									addReqToList = FALSE;
									res = S_FALSE;
								}
							}
						}
					}
					break;

					case SODaSRequest::asyncCache:
					case SODaSRequest::asyncDevice:
					case SODaSRequest::asyncMaxAge:
					{
						req->setCompletedEvent(reqCompEvent);

						if ((tag->getAccessRights(srv) & accessRight) != accessRight)   // valid access rights
						{
							req->setResult(OPC_E_BADRIGHTS);

							if (checkForErrors)
							{
								addReqToList = FALSE;
								(*errors)[i] = req->getResult();
								res = S_FALSE;
								break;
							}
						}

						if (reqOp == SODaSRequest::read)
						{
							if (reqType == SODaSRequest::asyncMaxAge)
							{
								req->setMaxAge(pMaxAge[i]);
							}
						}
						else // write
						{
							if (writeItemValues)
							{
								// DA 1.0 and DA 2.0
								req->setWriteValue(&writeItemValues[i]);
							}
							else
							{
								// DA 3.0
								BYTE writeQT = 0;

								if (pItemVQT[i].bQualitySpecified)
								{
									writeQT |= SODASREQUEST_FLAG_QUALITY;
								}

								if (pItemVQT[i].bTimeStampSpecified)
								{
									writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
								}

								if (writeQT == 0)
								{
									req->setWriteValue(&(pItemVQT[i].vDataValue));
								}
								else
								{
									if (tag->supportWriteQT(writeQT))
									{
										if (req->setWriteValue(&(pItemVQT[i].vDataValue)))
										{
											if (pItemVQT[i].bQualitySpecified)
											{
												req->setWriteQuality(pItemVQT[i].wQuality);
											}

											if (pItemVQT[i].bTimeStampSpecified)
											{
												req->setWriteTimeStamp(SOCmnDateTime(&pItemVQT[i].ftTimeStamp));
											}
										}
									}
									else
									{
										req->setResult(OPC_E_NOTSUPPORTED);
									}
								}
							}

							if (FAILED(req->getResult()))
							{
								(*errors)[i] = req->getResult();
								addReqToList = FALSE;
								res = S_FALSE;
							}
						}
					}
					break;
					}

					if (addReqToList)
					{
						reqList->AddTail((SODaSRequest*)req);
					}

					req.release();
				}
			}
			else
			{
				(*errors)[i] = OPC_E_BADRIGHTS;
				res = S_FALSE;
			}
		}
		else
		{
			(*errors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
	}

	return res;
} // fillRequestList
#endif // SOFEATURE_DCOM


#ifdef SOFEATURE_TUNNEL
//-----------------------------------------------------------------------------
// createTransaction
//
// - create transaction object, used for the asynchronous calls
//
// returns:
//      S_OK    - the transaction includes all items
//      S_FALSE - at least one item is not in the transaction
//
HRESULT SODaSGroup::createTransaction(IN BOOL read, IN BYTE connectionType,
									  IN DWORD connection, IN DWORD source, IN SOCmnEventPointer reqCompEvent,
									  IN DWORD numItems, IN OPCHANDLE* serverHandles,
									  IN DWORD* pMaxAge, IN OUT SOCmnVariant* pValues,
									  IN OUT WORD* pQualities, IN BOOL* pQualitySpecified,
									  IN OUT SOCmnDateTime* pTimeStamps, IN BOOL* pTimeStampSpecified,
									  OUT SODaSTransaction** transaction, OUT DWORD* transactionID, OUT HRESULT* errors)
{
	SODaSTransaction::transactionType type;
	SODaSRequest::requestOperation reqOp;
	SODaSRequest::requestType reqType;
	HRESULT res;
	DWORD accessRight;

	if (source == OPC_DS_CACHE)
	{
		// cache
		if (connectionType == SODaSTransaction::synchronous)
		{
			reqType = SODaSRequest::syncCache;
		}
		else
		{
			reqType = SODaSRequest::asyncCache;
		}
	}
	else
	{
		// device or maxage
		if (!pMaxAge)
		{
			if (connectionType == SODaSTransaction::synchronous)
			{
				reqType = SODaSRequest::syncDevice;
			}
			else
			{
				reqType = SODaSRequest::asyncDevice;
			}
		}
		else
		{
			if (connectionType == SODaSTransaction::synchronous)
			{
				reqType = SODaSRequest::syncMaxAge;
			}
			else
			{
				reqType = SODaSRequest::asyncMaxAge;
			}
		}
	}

	if (read)
	{
		if (connectionType == SODaSTransaction::synchronous)
		{
			type = SODaSTransaction::syncRead;
		}
		else
		{
			type = SODaSTransaction::asyncRead;
		}

		reqOp = SODaSRequest::read;
		accessRight = OPC_READABLE;
	}
	else
	{
		if (connectionType == SODaSTransaction::synchronous)
		{
			type = SODaSTransaction::syncWrite;
		}
		else
		{
			type = SODaSTransaction::asyncWrite;
		}

		reqOp = SODaSRequest::write;
		accessRight = OPC_WRITEABLE;
	}

	SODaSTransaction* tac;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	tac = creator->createTransaction(this, (BYTE)type, connectionType, connection);
	{
		SOCmnList<SODaSRequest> reqList(tac->getRequestList());
		reqList.allocateBlock(numItems);
	}
	// create request list
	res = fillRequestList(reqType, reqOp, accessRight,
						  (connectionType != SODaSTransaction::dataObject), reqCompEvent,
						  numItems, serverHandles,
						  pMaxAge, pValues, pQualities, pQualitySpecified, pTimeStamps, pTimeStampSpecified,
						  tac->getRequestList(), errors);

	if (SUCCEEDED(res))
	{
		if (transactionID)
		{
			*transactionID = tac->getTransactionId();
		}

		*transaction = tac;
	}
	else
	{
		if (transactionID)
		{
			*transactionID = 0;
		}

		*transaction = NULL;
		tac->release(); // free transaction
	}

	return res;
} // createTransaction


// fill request list from interface parameters
HRESULT SODaSGroup::fillRequestList(IN SODaSRequest::requestType reqType,
									IN SODaSRequest::requestOperation reqOp, IN DWORD accessRight, IN BOOL checkForErrors, IN SOCmnEventPointer reqCompEvent,
									IN DWORD numItems, IN OPCHANDLE* serverHandles,
									IN DWORD* pMaxAge, IN OUT SOCmnVariant* pValues,
									IN OUT WORD* pQualities, IN BOOL* pQualitySpecified,
									IN OUT SOCmnDateTime* pTimeStamps, IN BOOL* pTimeStampSpecified,
									OUT SOCmnList<SODaSRequest> *reqList, OUT HRESULT* errors)
{
	HRESULT res = S_OK;
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSItem> item;
	SOCmnPointer<SODaSItemTag> tag;
	SOCmnPointer<SODaSServer> srv = (SODaSServer*)getParentBranch();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	DWORD i = 0;
	SOCmnPointer<SODaSRequest> req;
	BOOL addReqToList;

	// create request list
	for (i = 0; i < numItems; i++)
	{
		// get item
		SOCMN_TRY
		{
			item = getItemByHandle(serverHandles[i]);
		} 
		SOCMN_CATCH(...)
		{
			item = NULL;
		}

		if ((SODaSItem*)item)
		{
			// check access rights
			tag = item->getItemTag();

			if ((!checkForErrors) || ((tag->getAccessRights(srv) & accessRight) == accessRight))
			{
				// valid access rights or do not check the access rights
				// create request
				errors[i] = S_OK;
				req = creator->createRequest(srv, item, tag, (BYTE)reqOp, (WORD)reqType);

				if (req.isNotNull())
				{
					req->setItem(item);
					addReqToList = TRUE;

					switch (reqType)
					{
					case SODaSRequest::syncCache:
					case SODaSRequest::syncDevice:
					case SODaSRequest::syncMaxAge:
					{
						req->setCompletedEvent(reqCompEvent);
						req->setResultAddress(&errors[i]);
						req->setResult(S_OK);

						if (reqOp == SODaSRequest::read)
						{
							if (reqType == SODaSRequest::syncMaxAge)
							{
								req->setMaxAge(pMaxAge[i]);
							}

							req->setReadValueAddress(&pValues[i]);

							if (pQualities)
							{
								req->setReadQualityAddress(&pQualities[i]);
							}

							if (pTimeStamps)
							{
								req->setReadTimeStampAddress(&pTimeStamps[i]);
							}
						}
						else
						{
							BYTE writeQT = 0;

							if (pQualities && (!pQualitySpecified || pQualitySpecified[i]))
							{
								writeQT |= SODASREQUEST_FLAG_QUALITY;
							}

							if (pTimeStamps && (!pTimeStampSpecified || pTimeStampSpecified[i]))
							{
								writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
							}

							if (writeQT == 0)
							{
								req->setWriteValue(&pValues[i]);
							}
							else
							{
								if (tag->supportWriteQT(writeQT))
								{
									if (req->setWriteValue(&pValues[i]))
									{
										if (writeQT & SODASREQUEST_FLAG_QUALITY)
										{
											req->setWriteQuality(pQualities[i]);
										}

										if (writeQT & SODASREQUEST_FLAG_TIMESTAMP)
										{
											req->setWriteTimeStamp(pTimeStamps[i]);
										}
									}
								}
								else
								{
									req->setResult(OPC_E_NOTSUPPORTED);
								}
							}

							if (FAILED(req->getResult()))
							{
								addReqToList = FALSE;
								res = S_FALSE;
							}
						}
					}
					break;

					case SODaSRequest::asyncCache:
					case SODaSRequest::asyncDevice:
					case SODaSRequest::asyncMaxAge:
					{
						req->setCompletedEvent(reqCompEvent);

						if ((tag->getAccessRights(srv) & accessRight) != accessRight)   // valid access rights
						{
							req->setResult(OPC_E_BADRIGHTS);

							if (checkForErrors)
							{
								addReqToList = FALSE;
								errors[i] = req->getResult();
								res = S_FALSE;
								break;
							}
						}

						if (reqOp == SODaSRequest::read)
						{
							if (reqType == SODaSRequest::asyncMaxAge)
							{
								req->setMaxAge(pMaxAge[i]);
							}
						}
						else // write
						{
							BYTE writeQT = 0;

							if (pQualities && (!pQualitySpecified || pQualitySpecified[i]))
							{
								writeQT |= SODASREQUEST_FLAG_QUALITY;
							}

							if (pTimeStamps && (!pTimeStampSpecified || pTimeStampSpecified[i]))
							{
								writeQT |= SODASREQUEST_FLAG_TIMESTAMP;
							}

							if (writeQT == 0)
							{
								req->setWriteValue(&pValues[i]);
							}
							else
							{
								if (tag->supportWriteQT(writeQT))
								{
									if (req->setWriteValue(&pValues[i]))
									{
										if (writeQT & SODASREQUEST_FLAG_QUALITY)
										{
											req->setWriteQuality(pQualities[i]);
										}

										if (writeQT & SODASREQUEST_FLAG_TIMESTAMP)
										{
											req->setWriteTimeStamp(pTimeStamps[i]);
										}
									}
								}
								else
								{
									req->setResult(OPC_E_NOTSUPPORTED);
								}
							}

							if (FAILED(req->getResult()))
							{
								errors[i] = req->getResult();
								addReqToList = FALSE;
								res = S_FALSE;
							}
						}
					}
					break;

					default:
						break;
					}

					if (addReqToList)
					{
						reqList->AddTail((SODaSRequest*)req);
					}

					req.release();
				}
			}
			else
			{
				errors[i] = OPC_E_BADRIGHTS;
				res = S_FALSE;
			}
		}
		else
		{
			errors[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
	}

	return res;
} // fillRequestList
#endif // SOFEATURE_TUNNEL

BOOL SODaSGroup::hasPrePostValidateItems(void)
{
	return FALSE;
}

DWORD SODaSGroup::preValidateItems(BOOL /* addItemsOrValidateItems */, SOCmnList<SODaSItemData>* /* pItemDataList */)
{
	return 0;
}

void SODaSGroup::postValidateItems(DWORD /* preValidateItemsCookie */, SOCmnList<SODaSItemData>* /* pItemDataList */)
{
}

void SODaSGroup::preCreateItemEnumerator(void)
{
}

BOOL SODaSGroup::rejectCyclicDataBeforeInit(void)
{
	return TRUE;
}

#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_COMIF_HOOKS
HRESULT SODaSGroup::onBGetState(OUT DWORD* pUpdateRate, OUT BOOL* pActive, OUT LPWSTR* ppName, OUT LONG* pTimeBias, OUT FLOAT* pDeadband, OUT DWORD* pLCID, OUT OPCHANDLE* phClientGroup, OUT OPCHANDLE* phServerGroup)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetState(IN DWORD* pRequestedUpdateRate, OUT DWORD* pRevisedUpdateRate, IN BOOL* pActive, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetName(IN LPCWSTR szName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBCloneGroup(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  name:%s"), W2CT(szName)));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBGetState(OUT BOOL* pPublic)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBMoveToPublic(void)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBAddItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, OUT OPCITEMRESULT** ppAddResults, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBValidateItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, IN BOOL bBlobUpdate, OUT OPCITEMRESULT** ppValidationResults, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRemoveItems(IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetActiveState(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN BOOL bActive, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetClientHandles(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCHANDLE* phClient, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetDatatypes(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARTYPE* pRequestedDatatypes, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBCreateEnumerator(IN REFIID riid, OUT LPUNKNOWN* ppUnk)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRead(IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT OPCITEMSTATE** ppItemValues, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu dsrv:%lu"), dwNumItems, dwSource));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBWrite(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu"), dwNumItems));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRead(IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT DWORD* pTransactionID, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu con:%lu dsrv:%lu"), dwNumItems, dwConnection, dwSource));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBWrite(IN DWORD dwConnection, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD* pTransactionID, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu con:%lu"), dwNumItems, dwConnection));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRefresh(IN DWORD dwConnection, IN OPCDATASOURCE dwSource, OUT DWORD* pTransactionID)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  con:%lu"), dwConnection));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBCancel(IN DWORD dwTransactionID)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cId:%lu"), dwTransactionID));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRead(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu tId:%lu"), dwNumItems, dwTransactionID));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBWrite(IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID, OUT HRESULT** ppErrors)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cnt:%lu tId:%lu"), dwNumItems, dwTransactionID));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRefresh2(IN OPCDATASOURCE dwSource, IN DWORD dwTransactionID, OUT DWORD* pdwCancelID)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  dsrv:%lu tId:%lu"), dwSource, dwTransactionID));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBCancel2(IN DWORD dwCancelID)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cId:%lu"), dwCancelID));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetEnable(IN BOOL bEnable)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  enable:%u"), bEnable));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBGetEnable(OUT BOOL* pbEnable)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBAdvise(IN IUnknown* pUnk, OUT DWORD* pdwCookie)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBUnadvise(IN DWORD dwCookie)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), dwCookie));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBDAdvise(IN FORMATETC* format, IN DWORD advf, IN IAdviseSink* pAdvSink, OUT DWORD* pdwConnection)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBDUnadvise(IN DWORD dwConnection)
{
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  con:0x%X"), dwConnection));
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBWriteVQT(IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBWriteVQT(IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBRefreshMaxAge(IN DWORD maxAge, IN DWORD transactionID, OUT DWORD* pCancelID)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetKeepAlive(IN DWORD keepAliveTime, OUT DWORD* pRevisedKeepAliveTime)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBGetKeepAlive(OUT DWORD* pKeepAliveTime)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBSetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, IN FLOAT* pPercentDeadband, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBGetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, OUT FLOAT** ppPercentDeadband, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
HRESULT SODaSGroup::onBClearItemDeadband(IN DWORD count, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors)
{
	return E_NOTIMPL;
}
void SODaSGroup::onEGetState(IN HRESULT res, IN DWORD* pUpdateRate, IN BOOL* pActive, IN LPWSTR* ppName, IN LONG* pTimeBias, IN FLOAT* pDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup, IN OPCHANDLE* phServerGroup) {}
void SODaSGroup::onESetState(IN HRESULT res, IN DWORD* pRequestedUpdateRate, IN DWORD* pRevisedUpdateRate, IN BOOL* pActive, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD* pLCID, IN OPCHANDLE* phClientGroup) {}
void SODaSGroup::onESetName(IN HRESULT res, IN LPCWSTR szName) {}
void SODaSGroup::onECloneGroup(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk) {}
void SODaSGroup::onEGetState(IN HRESULT res, IN BOOL* pPublic) {}
void SODaSGroup::onEMoveToPublic(IN HRESULT res) {}
void SODaSGroup::onEAddItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, OUT OPCITEMRESULT** ppAddResults, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEValidateItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray, IN BOOL bBlobUpdate, OUT OPCITEMRESULT** ppValidationResults, OUT HRESULT** ppErrors) {}
void SODaSGroup::onERemoveItems(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors) {}
void SODaSGroup::onESetActiveState(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN BOOL bActive, OUT HRESULT** ppErrors) {}
void SODaSGroup::onESetClientHandles(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCHANDLE* phClient, OUT HRESULT** ppErrors) {}
void SODaSGroup::onESetDatatypes(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARTYPE* pRequestedDatatypes, OUT HRESULT** ppErrors) {}
void SODaSGroup::onECreateEnumerator(IN HRESULT res, IN REFIID riid, OUT LPUNKNOWN* ppUnk) {}
void SODaSGroup::onERead(IN HRESULT res, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN OPCITEMSTATE** ppItemValues, IN HRESULT** ppErrors) {}
void SODaSGroup::onEWrite(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN HRESULT** ppErrors) {}
void SODaSGroup::onERead(IN HRESULT res, IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD* pTransactionID, IN HRESULT** ppErrors) {}
void SODaSGroup::onEWrite(IN HRESULT res, IN DWORD dwConnection, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD* pTransactionID, IN HRESULT** ppErrors) {}
void SODaSGroup::onERefresh(IN HRESULT res, IN DWORD dwConnection, IN OPCDATASOURCE dwSource, IN DWORD* pTransactionID) {}
void SODaSGroup::onECancel(IN HRESULT res, IN DWORD dwTransactionID) {}
void SODaSGroup::onERead(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN DWORD dwTransactionID, IN DWORD* pdwCancelID, IN HRESULT** ppErrors) {}
void SODaSGroup::onEWrite(IN HRESULT res, IN DWORD dwNumItems, IN OPCHANDLE* phServer, IN VARIANT* pItemValues, IN DWORD dwTransactionID, IN DWORD* pdwCancelID, IN HRESULT** ppErrors) {}
void SODaSGroup::onERefresh2(IN HRESULT res, IN OPCDATASOURCE dwSource, IN DWORD dwTransactionID, IN DWORD* pdwCancelID) {}
void SODaSGroup::onECancel2(IN HRESULT res, IN DWORD dwCancelID) {}
void SODaSGroup::onESetEnable(IN HRESULT res, IN BOOL bEnable) {}
void SODaSGroup::onEGetEnable(IN HRESULT res, IN BOOL* pbEnable)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  enable:%u"), *pbEnable));
	}
}
void SODaSGroup::onEAdvise(IN HRESULT res, IN IUnknown* pUnk, IN DWORD* pdwCookie)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  cookie:0x%X"), *pdwCookie));
	}
}
void SODaSGroup::onEUnadvise(IN HRESULT res, IN DWORD dwCookie) {}
void SODaSGroup::onEDAdvise(IN HRESULT res, IN FORMATETC* format, IN DWORD advf, IN IAdviseSink* pAdvSink, IN DWORD* pdwConnection)
{
	if (SUCCEEDED(res))
	{
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  con:0x%X"), *pdwConnection));
	}
}
void SODaSGroup::onEDUnadvise(IN HRESULT res, IN DWORD dwConnection) {}
void SODaSGroup::onEReadMaxAge(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEWriteVQT(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEReadMaxAge(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEWriteVQT(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN OPCITEMVQT* pItemVQT, IN DWORD transactionID, OUT DWORD* pCancelID, OUT HRESULT** ppErrors) {}
void SODaSGroup::onERefreshMaxAge(IN HRESULT res, IN DWORD maxAge, IN DWORD transactionID, OUT DWORD* pCancelID) {}
void SODaSGroup::onESetKeepAlive(IN HRESULT res, IN DWORD keepAliveTime, OUT DWORD* pRevisedKeepAliveTime) {}
void SODaSGroup::onEGetKeepAlive(IN HRESULT res, OUT DWORD* pKeepAliveTime) {}
void SODaSGroup::onESetItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, IN FLOAT* pPercentDeadband, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEGetItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, OUT FLOAT** ppPercentDeadband, OUT HRESULT** ppErrors) {}
void SODaSGroup::onEClearItemDeadband(IN HRESULT res, IN DWORD count, IN OPCHANDLE* phServer, OUT HRESULT** ppErrors) {}
#endif // SOFEATURE_COMIF_HOOKS
#endif // SOFEATURE_DCOM


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SODaSGroupIAdvise                                                          |
//-----------------------------------------------------------------------------

SODaSGroupIAdvise::SODaSGroupIAdvise()
{}

SODaSGroupIAdvise::~SODaSGroupIAdvise()
{}


//-----------------------------------------------------------------------------
// supportConnectionType
//
// - check if connection type is supported
//
// returns:
//      TRUE  - connection type supported
//      FALSE - connection type not supported
//
BOOL SODaSGroupIAdvise::supportConnectionType(
	IN BYTE /* connectionType */) // connection type
{
	return FALSE;
} // supportConnectionType


//-----------------------------------------------------------------------------
// adviseDataCallback
//
// - advise callback connection
//
// returns:
//      S_OK                  - connection advised
//      CONNECT_E_ADVISELIMIT - advise limit reached
//      E_NOTIMPL             - not implemented
//
HRESULT SODaSGroupIAdvise::adviseDataCallback(
	IN BYTE /* connectionType */,     // connection type
	IN IUnknown* /* pUnk */,          // advise interface
	IN CLIPFORMAT /* format */,       // data format (only v1.0)
	OUT DWORD* /* connectionId */)    // connection id
{
	return E_NOTIMPL;
} // adviseDataCallback


//-----------------------------------------------------------------------------
// unadviseDataCallback
//
// - unadvise callback connection
//
// returns:
//      S_OK                   - connection unadvised
//      CONNECT_E_NOCONNECTION - no connection advised with this id
//      E_NOTIMPL              - not implemented
//
HRESULT SODaSGroupIAdvise::unadviseDataCallback(
	IN BYTE /* connectionType */, // connection type
	IN DWORD /* connectionId */)  // connection id
{
	return E_NOTIMPL;
} // unadviseDataCallback


//-----------------------------------------------------------------------------
// onAdviseDataCallback
//
// - called after advised callback connection
//
void SODaSGroupIAdvise::onAdviseDataCallback(
	IN BYTE /* connectionType */, // connection type
	IN DWORD /* connection */,    // connection id
	IN CLIPFORMAT /* format */)   // data format (only v1.0)
{} // onAdviseDataCallback


//-----------------------------------------------------------------------------
// onUnadviseDataCallback
//
// - called after unadvised callback connection
//
void SODaSGroupIAdvise::onUnadviseDataCallback(
	IN BYTE /* connectionType */, // connection type
	IN DWORD /* connection */)    // connection id
{} // onUnadviseDataCallback


//-----------------------------------------------------------------------------
// validConnection
//
// - check if the connection is advised
//
// returns:
//      TRUE  - connection is advised
//      FALSE - no connection advised with this id
//
BOOL SODaSGroupIAdvise::validConnection(
	IN BYTE /* connectionType */, // connection type
	IN DWORD /* connection */,    // connection id
	IN BOOL /* read */)           // read or write connection (only v1.0)
{
	return FALSE;
} // validConnection


//-----------------------------------------------------------------------------
// getAdviseStartPositionAndLock
//
// - get start position of 'advise list'
//
// returns:
//      position of first advise
//      NULL - no callback advised
//
SOCmnListPosition SODaSGroupIAdvise::getAdviseStartPositionAndLock(
	IN BYTE /* connectionType */)    // connection type
{
	return NULL;
} // getAdviseStartPositionAndLock


//-----------------------------------------------------------------------------
// getNextAdvise
//
// - get next advise of 'advise list'
//
void SODaSGroupIAdvise::getNextAdvise(
	IN OUT SOCmnListPosition& pos,      // position in 'advise list'
	OUT SODaSDataCallback& cb,          // advise
	IN OPTIONAL BYTE /* connectionType */)    // connection type
const
{
	if (cb.m_unknown)
	{
		cb.m_unknown->Release();
	}

	cb.m_format = 0;
	cb.m_connectionId = 0;
	cb.m_connectionType = 0;
	cb.m_unknown = NULL;
	pos = NULL;
} // getNextAdvise


//-----------------------------------------------------------------------------
// doAdviseUnlock
//
// - unlock the 'advise list'
//
void SODaSGroupIAdvise::doAdviseUnlock(void)
{} // doAdviseUnlock



//-----------------------------------------------------------------------------
// SODaSGroupAdviseOneOnlyV2                                                  |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// supportConnectionType
//
// - check if connection type is supported
//
// returns:
//      TRUE  - connection type supported
//      FALSE - connection type not supported
//
BOOL SODaSGroupAdviseOneOnlyV2::supportConnectionType(
	IN BYTE connectionType) // connection type
{
	if (connectionType == SODASDATACALLBACK_CT_DATAOBJECT)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
} // supportConnectionType


//-----------------------------------------------------------------------------
// adviseDataCallback
//
// - advise callback connection
//
// returns:
//      S_OK                  - connection advised
//      CONNECT_E_ADVISELIMIT - advise limit reached
//      E_NOTIMPL             - not implemented
//
HRESULT SODaSGroupAdviseOneOnlyV2::adviseDataCallback(
	IN BYTE connectionType,     // connection type
	IN IUnknown* pUnk,          // advise interface
	IN CLIPFORMAT /* format */,       // data format (only v1.0)
	OUT DWORD* connectionId)    // connection id
{
	if (!(m_advisesLock.lock(2000)))
	{
		return E_PENDING;
	}

	HRESULT res;

	if (connectionType == SODASDATACALLBACK_CT_DATAOBJECT)
	{
		res = CONNECT_E_ADVISELIMIT;
	}
	else if (m_connectionPoint.m_connectionId != 0)
	{
		res =  CONNECT_E_ADVISELIMIT;
	}
	else
	{
		if (pUnk)
		{
			pUnk->AddRef();
		}

		m_connectionPoint.m_unknown = pUnk;
		m_connectionPoint.m_format = 0;
		m_connectionPoint.m_connectionId = 1;
		m_connectionPoint.m_connectionType = connectionType;

		if (connectionId)
		{
			*connectionId = 1;
		}

		res = S_OK;
	}

	m_advisesLock.unlock();

	if (res == S_OK)
	{
		onAdviseDataCallback(m_connectionPoint.m_connectionType, m_connectionPoint.m_connectionId, m_connectionPoint.m_format);
	}

	return res;
} // adviseDataCallback


//-----------------------------------------------------------------------------
// unadviseDataCallback
//
// - unadvise callback connection
//
// returns:
//      S_OK                   - connection unadvised
//      CONNECT_E_NOCONNECTION - no connection advised with this id
//      E_NOTIMPL              - not implemented
//
HRESULT SODaSGroupAdviseOneOnlyV2::unadviseDataCallback(
	IN BYTE connectionType, // connection type
	IN DWORD connectionId)  // connection id
{
	if (!(m_advisesLock.lock(2000)))
	{
		return E_PENDING;
	}

	HRESULT res;

	if (m_connectionPoint.m_connectionId == connectionId)
	{
		if (m_connectionPoint.m_unknown)
		{
			m_connectionPoint.m_unknown->Release();
		}

		m_connectionPoint.m_unknown = NULL;
		m_connectionPoint.m_format = 0;
		m_connectionPoint.m_connectionId = 0;
		m_connectionPoint.m_connectionType = 0;
		res = S_OK;
	}
	else
	{
		res = CONNECT_E_NOCONNECTION;
	}

	m_advisesLock.unlock();

	if (res == S_OK)
	{
		onUnadviseDataCallback(connectionType, connectionId);
	}

	return res;
} // unadviseDataCallback


//-----------------------------------------------------------------------------
// getAdviseStartPositionAndLock
//
// - get start position of 'advise list'
//
// returns:
//      position of first advise
//      NULL - no callback advised
//
SOCmnListPosition SODaSGroupAdviseOneOnlyV2::getAdviseStartPositionAndLock(
	IN OPTIONAL BYTE connectionType)    // connection type
{
	m_advisesLock.lock();

	switch (connectionType)
	{
	case SODASDATACALLBACK_CT_CONNECTIONPOINT:
	case SODASDATACALLBACK_CT_TUNNEL:
	case SODASDATACALLBACK_CT_ALL:
		return ((m_connectionPoint.m_connectionId != 1) || (m_connectionPoint.m_connectionType != connectionType)) ? NULL : (SOCmnListPosition)1;
		break;
	}

	return NULL;
} // SODaSGroupAdviseOneOnlyV2


//-----------------------------------------------------------------------------
// getNextAdvise
//
// - get next advise of 'advise list'
//
void SODaSGroupAdviseOneOnlyV2::getNextAdvise(
	IN OUT SOCmnListPosition& pos,      // position in 'advise list'
	OUT SODaSDataCallback& cb,          // advise
	IN OPTIONAL BYTE connectionType)    // connection type
const
{
	if (cb.m_unknown)
	{
		cb.m_unknown->Release();
	}

	if (((DWORD)pos == 1) && (m_connectionPoint.m_connectionId == 1) && (m_connectionPoint.m_connectionType == connectionType))
	{
		cb.m_format = 0;
		cb.m_connectionId = m_connectionPoint.m_connectionId;
		cb.m_connectionType = m_connectionPoint.m_connectionType;
		cb.m_unknown = m_connectionPoint.m_unknown;

		if (cb.m_unknown)
		{
			cb.m_unknown->AddRef();
		}
	}
	else
	{
		cb.m_format = 0;
		cb.m_connectionId = 0;
		cb.m_connectionType = 0;
		cb.m_unknown = NULL;
	}

	pos = NULL;
} // getNextAdvise


//-----------------------------------------------------------------------------
// doAdviseUnlock
//
// - unlock the 'advise list'
//
void SODaSGroupAdviseOneOnlyV2::doAdviseUnlock(void)
{
	m_advisesLock.unlock();
} // doAdviseUnlock


//-----------------------------------------------------------------------------
// validConnection
//
// - check if the connection is advised
//
// returns:
//      TRUE  - connection is advised
//      FALSE - no connection advised with this id
//
BOOL SODaSGroupAdviseOneOnlyV2::validConnection(
	IN BYTE connectionType, // connection type
	IN DWORD /* connection */,    // connection id
	IN BOOL /* read */)           // read or write connection (only v1.0)
{
	if ((m_connectionPoint.m_connectionId != 0) &&
		(m_connectionPoint.m_connectionType == connectionType))
	{
		return TRUE;
	}

	return FALSE;
} // validConnection



//-----------------------------------------------------------------------------
// SODaSGroupAdviseFull                                                       |
//-----------------------------------------------------------------------------

SODaSGroupAdviseFull::SODaSGroupAdviseFull()
{
	m_advises.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DATA);
}

SODaSGroupAdviseFull::~SODaSGroupAdviseFull()
{
	m_advises.destroy();
}


//-----------------------------------------------------------------------------
// supportConnectionType
//
// - check if connection type is supported
//
// returns:
//      TRUE  - connection type supported
//      FALSE - connection type not supported
//
BOOL SODaSGroupAdviseFull::supportConnectionType(
	IN BYTE /* connectionType */) // connection type
{
	return TRUE;
} // supportConnectionType


//-----------------------------------------------------------------------------
// adviseDataCallback
//
// - advise callback connection
//
// returns:
//      S_OK                  - connection advised
//      CONNECT_E_ADVISELIMIT - advise limit reached
//      E_NOTIMPL             - not implemented
//
HRESULT SODaSGroupAdviseFull::adviseDataCallback(
	IN BYTE connectionType,     // connection type
	IN IUnknown* pUnk,          // advise interface
	IN CLIPFORMAT format,       // data format (only v1.0)
	OUT DWORD* connectionId)    // connection id
{
	if (!(m_advisesLock.lock(2000)))
	{
		return E_PENDING;
	}

	SODaSDataCallback* advise = new SODaSDataCallback;
	DWORD idx;

	for (idx = 1; m_advises.findKey(idx, NULL); idx++)
		;

	if (connectionId)
	{
		*connectionId = idx;
	}

	advise->m_connectionId = idx;
	advise->m_connectionType = connectionType;
	advise->m_format = format;
	advise->m_unknown = pUnk;

	if (pUnk)
	{
		pUnk->AddRef();
	}

	m_advises.add(advise, idx);
	m_advisesLock.unlock();
	onAdviseDataCallback(connectionType, advise->m_connectionId, advise->m_format);
	return S_OK;
} // adviseDataCallback


//-----------------------------------------------------------------------------
// unadviseDataCallback
//
// - unadvise callback connection
//
// returns:
//      S_OK                   - connection unadvised
//      CONNECT_E_NOCONNECTION - no connection advised with this id
//      E_NOTIMPL              - not implemented
//
HRESULT SODaSGroupAdviseFull::unadviseDataCallback(
	IN BYTE connectionType, // connection type
	IN DWORD connectionId)  // connection id
{
	HRESULT res;
	BOOL removed;

	if (!(m_advisesLock.lock(2000)))
	{
		return E_PENDING;
	}

	removed = m_advises.removeKey(connectionId);
	m_advisesLock.unlock();

	if (removed)
	{
		onUnadviseDataCallback(connectionType, connectionId);
		res = S_OK;
	}
	else
	{
		if (connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
		{
			res = CONNECT_E_NOCONNECTION;
		}
		else
		{
			res = OLE_E_NOCONNECTION;
		}
	}

	return res;
} // unadviseDataCallback


//-----------------------------------------------------------------------------
// getAdviseStartPositionAndLock
//
// - get start position of 'advise list'
//
// returns:
//      position of first advise
//      NULL - no callback advised
//
SOCmnListPosition SODaSGroupAdviseFull::getAdviseStartPositionAndLock(
	IN OPTIONAL BYTE connectionType)    // connection type
{
	SOCmnListPosition pos, curPos;
	SODaSDataCallback* advise;
	m_advisesLock.lock();
	pos = m_advises.getStartPosition();

	while (pos)
	{
		curPos = pos;
		advise = m_advises.getNext(pos);

		if (connectionType & advise->m_connectionType)
		{
			return curPos;
		}
	}

	return NULL;
} // getAdviseStartPositionAndLock


//-----------------------------------------------------------------------------
// doAdviseUnlock
//
// - unlock the 'advise list'
//
void SODaSGroupAdviseFull::doAdviseUnlock(void)
{
	m_advisesLock.unlock();
} // doAdviseUnlock


//-----------------------------------------------------------------------------
// getNextAdvise
//
// - get next advise of 'advise list'
//
void SODaSGroupAdviseFull::getNextAdvise(
	IN OUT SOCmnListPosition& pos,      // position in 'advise list'
	OUT SODaSDataCallback& cb,          // advise
	IN OPTIONAL BYTE connectionType)    // connection type
const
{
	SODaSDataCallback* advise;

	if (cb.m_unknown)
	{
		cb.m_unknown->Release();
	}

	while (pos)
	{
		advise = m_advises.getNext(pos);

		if (connectionType & advise->m_connectionType)
		{
			SOCmnListPosition nextPos;
			cb.m_format = advise->m_format;
			cb.m_connectionId = advise->m_connectionId;
			cb.m_connectionType = advise->m_connectionType;
			cb.m_unknown = advise->m_unknown;

			if (cb.m_unknown)
			{
				cb.m_unknown->AddRef();
			}

			while (pos)
			{
				nextPos = pos;
				advise = m_advises.getNext(nextPos);

				if (connectionType & advise->m_connectionType)
				{
					return;
				}

				pos = nextPos;
			}

			return;
		}
	}

	cb.m_format = 0;
	cb.m_connectionId = 0;
	cb.m_unknown = NULL;
	cb.m_connectionType = 0;
} // getNextAdvise


//-----------------------------------------------------------------------------
// validConnection
//
// - check if the connection is advised
//
// returns:
//      TRUE  - connection is advised
//      FALSE - no connection advised with this id
//
BOOL SODaSGroupAdviseFull::validConnection(
	IN BYTE connectionType, // connection type
	IN DWORD connection,    // connection id
	IN BOOL read)           // read or write connection (only v1.0)
{
	BOOL valid = FALSE;
	SODaSDataCallback* advise;

	if (!(m_advisesLock.lock(0)))
	{
		return TRUE;    // do not get the lock -> ignore initial check! It's checked again before sending the data
	}

	if (connectionType == SODASDATACALLBACK_CT_CONNECTIONPOINT)
	{
		valid = !m_advises.isEmpty();
	}
	else
	{
		advise = m_advises.findKey(connection);

		if (advise)
		{
			if (read)
			{
				if ((advise->m_format == OPCSTMFORMATDATA) ||
					(advise->m_format == OPCSTMFORMATDATATIME))
				{
					valid = TRUE;
				}
			}
			else
			{
				if (advise->m_format == OPCSTMFORMATWRITECOMPLETE)
				{
					valid = TRUE;
				}
			}
		}
	}

	m_advisesLock.unlock();
	return valid;
} // validConnection

#endif // SOFEATURE_DCOM



//-----------------------------------------------------------------------------
// SODaSGroupDefault                                                          |
//-----------------------------------------------------------------------------

SODaSGroupDefault::SODaSGroupDefault(void)
	: SODaSGroup()
{}



#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SODaSDataCallback                                                          |
//-----------------------------------------------------------------------------

SODaSDataCallback::SODaSDataCallback()
{
	m_format = 0;
	m_connectionId = 0;
	m_unknown = NULL;
}

SODaSDataCallback::~SODaSDataCallback()
{
	if (m_unknown)
	{
		m_unknown->Release();
		m_unknown = NULL;
	}
}

#endif // SOFEATURE_DCOM

#endif // SODAS
