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
//  Filename    : SODaSItem.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Item object related classes                             |
//                - SODaSItem: OPC DA item object                             |
//                - SODaSItemICyclic: Cyclic data interface of the item       |
//                - SODaSItemCyclicOne: Only one client cyclci data support   |
//                - SODaSItemCyclicFull: Full cyclic data support             |
//                - SODaSItemDefault: Default configuration for item objects  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "opcerror.h"
#include "SODaSItem.h"
#include "SODaSCache.h"
#include "SODaSItemTag.h"
#include "SODaSServer.h"
#include "SODaSEntry.h"
#include "SOCmnTrace.h"


//-----------------------------------------------------------------------------
// SODaSItem                                                                  |
//-----------------------------------------------------------------------------

SODaSItem::SODaSItem(void)
	: SOCmnElement(SOCMNOBJECT_TYPE_ITEM)
{
	m_itemTag = NULL;
	m_active = FALSE;
	m_reqDatatype = VT_EMPTY;
	m_clientHandle = 0;
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_ITEM);
#endif
}

SODaSItem::~SODaSItem()
{
	setActive(FALSE);
	// release objects
	setItemTag(NULL, FALSE);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_ITEM);
#endif
}


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
BOOL SODaSItem::setParentBranch(
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

	prevPar.release();
#endif
	return SOCmnElement::setParentBranch(parent);
} // setParentBranch


//-----------------------------------------------------------------------------
// validateName
//
// - decrement reference counter
// - free object if no reference left
//
// returns:
//		S_OK                - valid name
//		E_INVALIDARG        - no valid name
//		OPC_E_DUPLICATENAME - no unique name
//
HRESULT SODaSItem::validateName(
	IN LPCTSTR /* name */,       // name
	IN BOOL /* checkIfUnique */) // check if unique in parent
{
	return S_OK;
} // validateName


//-----------------------------------------------------------------------------
// getCache
//
// - get cache object
//
// returns:
//		pointer to cache object (maybe NULL)
//
SODaSCache* SODaSItem::getCache(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (itemTag.isNotNull())
	{
		return itemTag->getCache();
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// getServer
//
// - get server object
//
// returns:
//		pointer to cache object
//
SODaSServer* SODaSItem::getServer(void)
{
	SOCmnPointer<SODaSGroup> group = (SODaSGroup*)getParentBranch();

	if (group.isNotNull())
	{
		return (SODaSServer*)(SOCmnElement*)group->getParentBranch();
	}

	return NULL;
} // getServer


//-----------------------------------------------------------------------------
// setItemTag
//
// - set item tag object
// - release previous tag
//
// returns:
//		TRUE
//
BOOL SODaSItem::setItemTag(
	IN SODaSItemTag* tag, // tag object
	IN BOOL goingToActivate)
{
	onSetItemTag(tag);
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	SOCmnPointer<SODaSItemTag> itemTag = m_itemTag;
	m_itemTag = tag;

	if (m_itemTag)
	{
		m_itemTag->addRef();
		tag->addRef();
	}

	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);

	if (itemTag.isNotNull())
	{
		itemTag->removeConnectedObject(this);
		itemTag.release();  // release previous item tag
	}

	if (tag)
	{
		tag->addConnectedObject(this, goingToActivate);
		tag->release(); // release local addRef new item tag
	}

	setObjTraceId(NULL);
	return TRUE;
} // setItemTag


//-----------------------------------------------------------------------------
// getItemTag
//
// - get tag object
//
// returns:
//		pointer to tag object (maybe NULL)
//
SODaSItemTag* SODaSItem::getItemTag(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (m_itemTag)
	{
		m_itemTag->addRef();
	}

	return m_itemTag;
} // getItemTag


//-----------------------------------------------------------------------------
// setReqDatatype
//
// - set requested data type
//
// returns:
//		S_OK          - support data type
//		OPC_E_BADTYPE - not support data type
//
HRESULT SODaSItem::setReqDatatype(
	IN VARTYPE reqDatatype) // requested data type
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
	HRESULT res;

	if (!itemTag)
	{
		return E_FAIL;
	}

	if (FAILED(res = itemTag->supportReqDatatype(reqDatatype)))
	{
		return res;
	}

	onSetReqDatatype(reqDatatype);
	m_reqDatatype = reqDatatype;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, m_reqDatatype));
	}

#endif
	return S_OK;
} // setReqDatatype


//-----------------------------------------------------------------------------
// setAccessPath
//
// - set access path
// - the default implementation don't stores access paths
//
// returns:
//		TRUE  - access path set
//		FALSE - no access path set
//
BOOL SODaSItem::setAccessPath(
	IN LPCTSTR /* accessPath */)    // access path
{
	return FALSE;
} // setAccessPath


//-----------------------------------------------------------------------------
// getAccessPath
//
// - get access path
// - the default implementation don't stores access paths
//
// returns:
//		access path
//
SOCmnString SODaSItem::getAccessPath(void)
{
	return SOCmnString(_T(""));
} // getAccessPath


//-----------------------------------------------------------------------------
// setActive
//
// - set activation state
//
// returns:
//		TRUE
//
BOOL SODaSItem::setActive(
	IN BOOL active) // activation state
{
	SOCmnPointer<SODaSGroup> grp = (SODaSGroup*)getParentBranch();

	if (!grp)
	{
		return FALSE;
	}

	BOOL prevActive = m_active;

	if (active)
	{
		active = TRUE;    // set 'NOT FALSE(!=0)' to 'TRUE(1)'
	}

	onSetActive(active);
	m_active = (BYTE)active;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, m_active));
	}

#endif

	// check if to start polling the item value
	if ((m_active) && (!prevActive))
	{
		// item was activated
		SOCmnPointer<SODaSItemTag> itemTag = getItemTag();
		SOCmnPointer<SODaSServer> server = getServer();

		if (itemTag.isNotNull())
		{
			if (isActive())
			{
				itemTag->activateConnectedObject(this);
			}

			if ((itemTag->getAccessRights(server) & OPC_READABLE) == OPC_READABLE)
			{
				// tag of item is readable
				// -> start polling
				grp->addCyclicRequest(this);
			}

			if (itemTag->getUpdateMode() == SODaSItemTag::report)
			{
				// report updated item
				SOCmnPointer<SODaSCache> cache = itemTag->getCache();

				if (cache.isNotNull())
				{
					if ((cache->isSet()) && (server.isNotNull()))
					{
						// notify actual cache value
						SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
						SOCmnPointer<SODaSCreator> creator = engine->getCreator();
						SOCmnPointer<SODaSTransaction> tac = grp->getCyclicTransaction();
						SOCmnPointer<SODaSRequest> req;
						SOCmnVariant value;
						SOCmnDateTime timeStamp;
						WORD quality;
						req = creator->createRequest(server, this, itemTag, SODaSRequest::read, SODaSRequest::report);

						if (req.isNotNull())
						{
							req->setItem(this);
							cache->get(value, &quality, &timeStamp, req, VT_EMPTY, server->getLCID());
							req->setReportValue(value);
							req->setReportQuality(quality);
							req->setReportTimeStamp(timeStamp);
							SOCmnSingleLock<SOCmnSync> lock(tac->getRequestListLock());
							setCyclicRequest(req);
							tac->insertRequest(req);
							lock.unlock();
							req.release();
						}
					}
				}
			}
		}
	}

	// check if to stop polling the item value
	if ((!m_active) && (prevActive))
	{
		// item was deactivated
		// -> stop polling
		grp->removeCyclicRequest(this);

		if (grp->getActive())
		{
			SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

			if (itemTag.isNotNull())
			{
				itemTag->deactivateConnectedObject(this);
			}
		}
	}

	return TRUE;
} // setActive


//-----------------------------------------------------------------------------
// setClientHandle
//
// - set client handle
//
// returns:
//		TRUE
//
BOOL SODaSItem::setClientHandle(
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


//-----------------------------------------------------------------------------
// onRead
//
// - called after completed read
// - default implementation: do nothing
//
void SODaSItem::onRead(
	IN BOOL /* deviceRead */)   // device or cache read
{} // onRead


//-----------------------------------------------------------------------------
// onWrite
//
// - called after completed write
// - default implementation: do nothing
//
void SODaSItem::onWrite(void)
{} // onWrite


//-----------------------------------------------------------------------------
// onSetActive
//
// - called before activation state is set
// - default implementation: do nothing
//
void SODaSItem::onSetActive(
	IN BOOL /* active */)   // activation state
{} // onSetActive


//-----------------------------------------------------------------------------
// onSetClientHandle
//
// - called before client handle is set
// - default implementation: do nothing
//
void SODaSItem::onSetClientHandle(
	IN OPCHANDLE /* clientHandle */)    // client handle
{} // onSetClientHandle


//-----------------------------------------------------------------------------
// onSetReqDatatype
//
// - called before requested data type is set
// - default implementation: do nothing
//
void SODaSItem::onSetReqDatatype(
	IN VARTYPE /* requestedType */) // requested data type
{} // onSetReqDatatype


//-----------------------------------------------------------------------------
// onSetItemTag
//
// - called before access path is set
// - default implementation: do nothing
//
void SODaSItem::onSetItemTag(
	IN SODaSItemTag* /* tagItem */) // item tag
{} // onSetItemTag


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaSItem::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString name;
		SOCmnString id;
		SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

		if (itemTag.isNotNull())
		{
			name = itemTag->getName();
		}
		else
		{
			name = getName();
		}

		id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)name);
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// getItemID
//
// - get the item id of the item tag
//
// returns:
//		the item ID of the object
//
SOCmnString SODaSItem::getItemID(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = getItemTag();

	if (itemTag.isNotNull())
	{
		return itemTag->getItemID();
	}
	else
	{
		return SOCmnString(_T(""));
	}
} // getItemID


//-----------------------------------------------------------------------------
// getCyclicRequest
//
// - get the cyclic request of the item
//
// returns:
//		the cyclic request of the item
//
SODaSRequest* SODaSItem::getCyclicRequest(void)
{
	SOCmnPointer<SODaSGroup> group;
	SOCmnPointer<SODaSTransaction> tac;
	group = (SODaSGroup*)getParentBranch();

	if (group.isNotNull())
	{
		tac = group->getCyclicTransaction();

		if (tac.isNotNull())
		{
			return tac->getRequestByItem(this);
		}
	}

	return NULL;
} // getCyclicRequest


//-----------------------------------------------------------------------------
// setCyclicRequest
//
// - set the cyclic request of the item
//
void SODaSItem::setCyclicRequest(
	IN SODaSRequest* /* pReq */) // cyclic request
{
} // setCyclicRequest


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSItem::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_ITEM);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ITEMID_T, getItemID());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACTIVE_T, conv->bool2string(buffer, getActive()));
		buffer = getAccessPath();

		if (!buffer.isEmpty())
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACCESSPATH_T, buffer);
		}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SERVERHANDLE_T, getServerHandle());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLIENTHANDLE_T, getClientHandle());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, getReqDatatype()));
		FLOAT db;
		HRESULT dbRes;
		dbRes = getDeadBand(db);

		if (dbRes == S_OK)
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEADBAND_T, buffer.format(_T("%f"), db));
		}
		else
		{
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEADBAND_T, conv->result2string(buffer, dbRes, SOSRVSERVER_SPECID_DA));
		}

		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH


//-----------------------------------------------------------------------------
// getObjectDaSItemIAttr
//
// - get object using the implementation of the interface
//
// returns:
//      object
//      NULL - don't have the pointer of the object
//
SOCmnObject* SODaSItem::getObjectDaSItemIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectDaSItemIAttr




//-----------------------------------------------------------------------------
// SODaSItemICyclic                                                           |
//-----------------------------------------------------------------------------

SODaSItemICyclic::SODaSItemICyclic()
{}

SODaSItemICyclic::~SODaSItemICyclic()
{}


//-----------------------------------------------------------------------------
// createCyclicData
//
// - create cyclic data
//
void SODaSItemICyclic::createCyclicData(void)
{} // createCyclicData


//-----------------------------------------------------------------------------
// destroyCyclicData
//
// - destroy cyclic data
//
void SODaSItemICyclic::destroyCyclicData(void)
{} // destroyCyclicData


//-----------------------------------------------------------------------------
// setSendValue
//
// - set the value send to one client
//
// returns:
//		TRUE  - value set
//		FALSE - can't set value set
//
BOOL SODaSItemICyclic::setSendValue(
	IN size_t /* connection */,      // connection id
	IN SOCmnVariant& /* value */,   // send value
	IN WORD /* quality */)          // send quality
{
	return FALSE;
} // setSendValue


//-----------------------------------------------------------------------------
// getSendValue
//
// - get the value last send to one client
//
// returns:
//		TRUE  - got value
//		FALSE - can't get value set
//
BOOL SODaSItemICyclic::getSendValue(
	IN size_t /* connection */,      // connection id
	OUT SOCmnVariant& /* value */,  // send value
	OUT WORD* /* quality */)        // send quality
{
	return FALSE;
} // getSendValue



//-----------------------------------------------------------------------------
// SODaSItemCyclicFull                                                        |
//-----------------------------------------------------------------------------

SOCmnSync s_cyclicSync;

SODaSItemCyclicFull::SODaSItemCyclicFull()
{}

SODaSItemCyclicFull::~SODaSItemCyclicFull()
{
	destroyCyclicData();
}

//-----------------------------------------------------------------------------
// createCyclicData
//
// - create cyclic data
//
void SODaSItemCyclicFull::createCyclicData(void)
{
	s_cyclicSync.lock();
	m_sendValues.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_VARIANT);
	m_sendQualities.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DWORD);
	s_cyclicSync.unlock();
} // createCyclicData


//-----------------------------------------------------------------------------
// destroyCyclicData
//
// - destroy cyclic data
//
void SODaSItemCyclicFull::destroyCyclicData(void)
{
	s_cyclicSync.lock();
	m_sendValues.destroy();
	m_sendQualities.destroy();
	s_cyclicSync.unlock();
} // destroyCyclicData


//-----------------------------------------------------------------------------
// getSendValue
//
// - get the value last send to one client
//
// returns:
//		TRUE  - got value
//		FALSE - can't get value set
//
BOOL SODaSItemCyclicFull::getSendValue(
	IN size_t connection,    // connection id
	OUT SOCmnVariant& value,    // send value
	OUT WORD* quality)      // send quality
{
	BOOL ret;
	DWORD qual;
	s_cyclicSync.lock();

	if (!m_sendValues.isInit())
	{
		s_cyclicSync.unlock();
		return FALSE;
	}

	value.clear();

	if ((ret = m_sendQualities.findKey(connection, &qual)) == TRUE)
	{
		SOCmnVariant* sendVal;
		*quality = (WORD)qual;

		if ((ret = m_sendValues.findKey(connection, &sendVal)) == TRUE)
		{
			value = *sendVal;
		}
	}
	else
	{
		*quality = OPC_QUALITY_BAD;
		ret = FALSE;
	}

	s_cyclicSync.unlock();
	return ret;
} // getSendValue


//-----------------------------------------------------------------------------
// setSendValue
//
// - set the value send to one client
//
// returns:
//		TRUE  - value set
//		FALSE - can't set value set
//
BOOL SODaSItemCyclicFull::setSendValue(
	IN size_t connection,    // connection id
	IN SOCmnVariant& value, // send value
	IN WORD quality)        // send quality
{
	s_cyclicSync.lock();

	if (!m_sendValues.isInit())
	{
		s_cyclicSync.unlock();
		return FALSE;
	}

	m_sendQualities.update(quality, connection);
	m_sendValues.update(&value, connection);
	s_cyclicSync.unlock();
	return TRUE;
} // setSendValue



//-----------------------------------------------------------------------------
// SODaSItemCyclicOne                                                         |
//-----------------------------------------------------------------------------

SODaSItemCyclicOne::SODaSItemCyclicOne()
{
	m_sendQuality = 0;
}


//-----------------------------------------------------------------------------
// destroyCyclicData
//
// - destroy cyclic data
//
void SODaSItemCyclicOne::destroyCyclicData(void)
{
	m_sendQuality = 0;
	m_sendValue.clear();
} // destroyCyclicData


//-----------------------------------------------------------------------------
// getSendValue
//
// - get the value last send to one client
//
// returns:
//		TRUE  - got value
//		FALSE - can't get value set
//
BOOL SODaSItemCyclicOne::getSendValue(
	IN size_t /* connection */,  // connection id
	OUT SOCmnVariant& value,    // send value
	OUT WORD* quality)          // send quality
{
	*quality = m_sendQuality;
	value = m_sendValue;
	return TRUE;
} // getSendValue


//-----------------------------------------------------------------------------
// setSendValue
//
// - set the value send to one client
//
// returns:
//		TRUE  - value set
//		FALSE - can't set value set
//
BOOL SODaSItemCyclicOne::setSendValue(
	IN size_t /* connection */,  // connection id
	IN SOCmnVariant& value,     // send value
	IN WORD quality)            // send quality
{
	m_sendQuality = quality;
	m_sendValue = value;
	return TRUE;
} // setSendValue


//-----------------------------------------------------------------------------
// SODaSItemIAttr                                                             |
//-----------------------------------------------------------------------------

SODaSItemIAttr::SODaSItemIAttr(void)
{}
SODaSItemIAttr::~SODaSItemIAttr(void)
{}

//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    OPC_E_DEADBANDNOTSET       - The item deadband has not been set for this item.
//
HRESULT SODaSItemIAttr::setDeadBand(FLOAT /* deadband */)
{
	return OPC_E_DEADBANDNOTSUPPORTED;
} // setDeadBand


//-----------------------------------------------------------------------------
// getDeadBand
//
// - get deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    OPC_E_DEADBANDNOTSET       - The item deadband has not been set for this item.
//
HRESULT SODaSItemIAttr::getDeadBand(FLOAT& /* deadband */)
{
	return OPC_E_DEADBANDNOTSUPPORTED;
} // getDeadBand


//-----------------------------------------------------------------------------
// clearDeadBand
//
// - clear deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    OPC_E_DEADBANDNOTSET       - The item deadband has not been set for this item.
//
HRESULT SODaSItemIAttr::clearDeadBand(void)
{
	return OPC_E_DEADBANDNOTSUPPORTED;
} // clearDeadBand


//-----------------------------------------------------------------------------
// getObjectDaSItemIAttr
//
// - get object using the implementation of the interface
//
// returns:
//      object
//      NULL - don't have the pointer of the object
//
SOCmnObject* SODaSItemIAttr::getObjectDaSItemIAttr(void)
{
	return NULL;
} // getObjectDaSItemIAttr


//-----------------------------------------------------------------------------
// onSetDeadBand
//
// - called before deadband is set
// - default implementation: do nothing
//
void SODaSItemIAttr::onSetDeadBand(IN FLOAT /* deadband */) // deadband
{} // onSetDeadBand

BOOL SODaSItemIAttr::setXMLClientHandle(IN LPCTSTR /* clientHandle */)
{
	return FALSE;
}

BOOL SODaSItemIAttr::getXMLClientHandle(OUT SOCmnStringEx& /* clientHandle */)
{
	return FALSE;
}


//-----------------------------------------------------------------------------
// SODaSItemAttrFull                                                          |
//-----------------------------------------------------------------------------

SODaSItemAttrFull::SODaSItemAttrFull(void)
{
	m_deadband = SODASITEM_NO_DEADBAND;
}

//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    E_INVALIDARG               - Requested deadband was not in the range of 0.0 to 100.0.
//
HRESULT SODaSItemAttrFull::setDeadBand(
	IN FLOAT deadband)  // deadband
{
	SODaSItem* pItem = (SODaSItem*)getObjectDaSItemIAttr();
	SOCmnPointer<SODaSItemTag> itemTag = pItem->getItemTag();

	if (!itemTag)
	{
		return E_FAIL;
	}

	if (itemTag->getEUType() == OPC_ANALOG)
	{
		if ((deadband >= 0.0) && (deadband <= 100.0))
		{
			onSetDeadBand(deadband);
			m_deadband = deadband;
#ifdef SOFEATURE_WATCH

			if (pItem->getCollectChanges())
			{
				SOCmnString buffer;
				SOSrvWatch::attributeChange(pItem, SOWATCH_ATTRIBUTE_DEADBAND_T, buffer.format(_T("%f"), m_deadband));
			}

#endif
			return S_OK;
		}
		else
		{
			return E_INVALIDARG;
		}
	}
	else
	{
		return OPC_E_DEADBANDNOTSUPPORTED;
	}
} // setDeadBand


//-----------------------------------------------------------------------------
// getDeadBand
//
// - get deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    OPC_E_DEADBANDNOTSET       - The item deadband has not been set for this item.
//
HRESULT SODaSItemAttrFull::getDeadBand(
	OUT FLOAT& deadband) // deadband
{
	SODaSItem* pItem = (SODaSItem*)getObjectDaSItemIAttr();
	SOCmnPointer<SODaSItemTag> itemTag = pItem->getItemTag();

	if (!itemTag)
	{
		return E_FAIL;
	}

	if (itemTag->getEUType() == OPC_ANALOG)
	{
		if (m_deadband != SODASITEM_NO_DEADBAND)
		{
			deadband = m_deadband;
			return S_OK;
		}
		else
		{
			return OPC_E_DEADBANDNOTSET;
		}
	}
	else
	{
		return OPC_E_DEADBANDNOTSUPPORTED;
	}
} // getDeadBand


//-----------------------------------------------------------------------------
// clearDeadBand
//
// - clear deadband
//
// returns:
//    S_OK                       - The function was successful.
//    OPC_E_DEADBANDNOTSUPPORTED - The item does not support deadband.
//    OPC_E_DEADBANDNOTSET       - The item deadband has not been set for this item.
//
HRESULT SODaSItemAttrFull::clearDeadBand(void)
{
	SODaSItem* pItem = (SODaSItem*)getObjectDaSItemIAttr();
	SOCmnPointer<SODaSItemTag> itemTag = pItem->getItemTag();

	if (!itemTag)
	{
		return E_FAIL;
	}

	if (itemTag->getEUType() == OPC_ANALOG)
	{
		if (m_deadband != SODASITEM_NO_DEADBAND)
		{
			onSetDeadBand(SODASITEM_NO_DEADBAND);
			m_deadband = SODASITEM_NO_DEADBAND;
#ifdef SOFEATURE_WATCH

			if (pItem->getCollectChanges())
			{
				SOCmnString buffer;
				SOSrvWatch::attributeChange(pItem, SOWATCH_ATTRIBUTE_DEADBAND_T, buffer.format(_T("%f"), m_deadband));
			}

#endif
			return S_OK;
		}
		else
		{
			return OPC_E_DEADBANDNOTSET;
		}
	}
	else
	{
		return OPC_E_DEADBANDNOTSUPPORTED;
	}
} // clearDeadBand


#ifdef SOFEATURE_SOAP

BOOL SODaSItemAttrFull::setXMLClientHandle(IN LPCTSTR clientHandle)
{
	m_clientHandle = clientHandle;
	return TRUE;
}

BOOL SODaSItemAttrFull::getXMLClientHandle(OUT SOCmnStringEx& clientHandle)
{
	clientHandle = (LPCTSTR)m_clientHandle;
	return TRUE;
}

#endif

//-----------------------------------------------------------------------------
// SODaSItemDefault                                                           |
//-----------------------------------------------------------------------------

SODaSItemDefault::SODaSItemDefault(void)
{
}

#endif // SODAS
