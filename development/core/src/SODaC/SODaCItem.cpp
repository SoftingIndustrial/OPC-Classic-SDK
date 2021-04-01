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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCItem.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Item object related classes                                 |
//                - SODaCItem: proxy for OPC item object                      |
//                - SODaCItemIAttr: interface class for item attributes       |
//                - SODaCItemAttrFull: full item attributes support           |
//                - SODaCItemAttrMin: minimal item attributes support         |
//                - SODaCItemIData: interface class for item I/O data         |
//                - SODaCItemDataFull: full item I/O data support             |
//                - SODaCItemDefault: default item class for creator          |
//                - SODaCItemStateTransitionTask: item state transition task  |
//                - SODaCItemTaskUpdate: item attribute value update task     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#include "SODaCItem.h"
#include "SODaCGroup.h"


static TCHAR g_emptyString[2] = _T("");


//-----------------------------------------------------------------------------
// SODaCItemStateTransitionTask                                               |
//-----------------------------------------------------------------------------

SODaCItemStateTransitionTask::SODaCItemStateTransitionTask(
	IN SOCltElement* element,
	IN BYTE targetState,
	IN BOOL deep)
	: SOCltTaskStateTransition(element, targetState, deep)
{
}; // SODaCItemStateTransitionTask

//-----------------------------------------------------------------------------
// doExecute

void SODaCItemStateTransitionTask::execute()
{
	_ASSERT_PTR((SOCltElement*)m_element);
	SOCmnPointer<SODaCGroup> parent = (SODaCGroup*)m_element->getParentBranch();
	_ASSERT_PTR((SODaCGroup*)parent);

	if (!m_element->getConnectionMonitorStatus())
	{
		if (m_targetState != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE)
		{
			m_element->setTargetObjectState(m_targetState);
		}
		parent->setSingleItemTransactionHandle(m_element->getOpcHandle());
		m_res = parent->doStateTransition(TRUE, SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE);
		parent->setSingleItemTransactionHandle(0);
	}
} // doExecute




//-----------------------------------------------------------------------------
// SOCltTaskUpdate                                                            |
//-----------------------------------------------------------------------------

SODaCItemTaskUpdate::SODaCItemTaskUpdate(
	IN SOCltElement* element,
	IN BOOL from_server,
	IN DWORD whatParameters)
	: SOCltTaskUpdate(element, from_server, whatParameters)
{
}

void SODaCItemTaskUpdate::execute()
{
	_ASSERT_PTR((SOCltElement*)m_element);
	HRESULT res = S_OK;
	SOCmnPointer<SODaCGroup> parent = (SODaCGroup*)m_element->getParentBranch();
	_ASSERT_PTR((SODaCGroup*)parent);
	SOCmnList<SODaCItem> itemList;
	itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	itemList.add((SODaCItem*)(SOCltElement*)m_element);
	res = parent->updateItems(m_fromServer, &itemList, m_whatParameters);
	itemList.destroy();
	m_res = res;
}




//-----------------------------------------------------------------------------
// SODaCItem                                                                 |
//-----------------------------------------------------------------------------

SODaCItem::SODaCItem(void)
	: SOCltElement(SOCMNOBJECT_TYPE_ITEM)
{
	m_active = FALSE;
	m_serverHandle = SODAS_ITEM_SERVERHANDLE_DISCONNECTED;
	m_stateTransitionActive = FALSE;
}

SODaCItem::~SODaCItem()
{
}


//-----------------------------------------------------------------------------
// start
//
// - start the element
//
// returns:
//		S_OK       - element and children started
//		error code - problems with starting
//
LONG SODaCItem::start(
	IN OPTIONAL BOOL deep,                /* = TRUE */
	IN OPTIONAL BOOL waitopt,             /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle  /* = NULL */) // start items?
{
	LONG ok = S_OK;
	SOCmnPointer<SODaCItemStateTransitionTask> task =
		new SODaCItemStateTransitionTask(this, started, deep);
	ok = executeTask(task, waitopt, handle);
	return ok;
} // start

//-----------------------------------------------------------------------------
// connect
//
// - connect the element
//
// returns:
//		S_OK       - element and children connected
//		error code - problems with connecting
//
LONG SODaCItem::connect(
	IN OPTIONAL BOOL deep,                /* = TRUE */
	IN OPTIONAL BOOL waitopt,             /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle  /* = NULL */) // connect items?
{
	LONG ok = S_OK;
	SOCmnPointer<SODaCItemStateTransitionTask> task =
		new SODaCItemStateTransitionTask(this, connected, deep);
	ok = executeTask(task, waitopt, handle);
	return ok;
} // connect

//-----------------------------------------------------------------------------
// disconnect
//
// - disconnect the element
//
// returns:
//		S_OK       - element and children disconnected
//		error code - problems with disconnecting
//
LONG SODaCItem::disconnect(
	IN OPTIONAL BOOL waitopt,             /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle  /* = NULL */)
{
	LONG ok = S_OK;
	SOCmnPointer<SODaCItemStateTransitionTask> task =
		new SODaCItemStateTransitionTask(this, disconnected, TRUE);
	ok = executeTask(task, waitopt, handle);
	return ok;
} // disconnect

//-----------------------------------------------------------------------------
// performStateTransition
//
// - connect the element
//
// returns:
//		S_OK       - element and children performed state transition successfully
//		error code - problems with performing state transition
//
LONG SODaCItem::performStateTransition(
	IN OPTIONAL BOOL deep, /* = TRUE */
	IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */) // connect items?
{
	LONG ok = S_OK;
	SOCmnPointer<SODaCItemStateTransitionTask> task =
		new SODaCItemStateTransitionTask(this, SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE, deep);
	ok = executeTask(task, waitopt, handle);
	return ok;
} // performStateTransition


//-----------------------------------------------------------------------------
// read
//
// - read item value
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but specific item error set
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCItem::read(
	IN DWORD maxAge)    // max cache age
{
	SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)getParentBranch();
	HRESULT res;

	if ((SODaCGroup*) grp)
	{
		SODaCItem* pThis = this;
		HRESULT error;

		// call the read method
		if (grp->getDataRetrieval() == SODaCGroup::drAsyncAdvise)
		{
			res = grp->asyncRead(1, &pThis, maxAge, 0, NULL, &error);
		}
		else
		{
			res = grp->syncRead(1, &pThis, maxAge, NULL, NULL, NULL, &error);
		}

		if (res == S_FALSE)
		{
			res = error;
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // read


//-----------------------------------------------------------------------------
// write
//
// - write item value
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but specific item error set
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCItem::write(
	IN OPTIONAL SOCmnVariant* pValue,
	IN OPTIONAL WORD* pQuality,
	IN OPTIONAL SOCmnDateTime* pTimeStamp)
{
	SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)getParentBranch();
	HRESULT res;

	if ((SODaCGroup*) grp)
	{
		SODaCItem* pThis = this;
		HRESULT error;

		// call the write method
		if (grp->getDataRetrieval() == SODaCGroup::drAsyncAdvise)
		{
			res = grp->asyncWrite(1, &pThis, 0, pValue, pQuality, pTimeStamp, NULL, &error);
		}
		else
		{
			res = grp->syncWrite(1, &pThis, pValue, pQuality, pTimeStamp, &error);
		}

		if (res == S_FALSE)
		{
			res = error;
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // write


//-----------------------------------------------------------------------------
// update
//
// - update item parameters with the OPC Item object in the OPC Server
//
// returns:
//		S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCItem::update(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)getParentBranch();
	HRESULT res;

	if ((SODaCGroup*) grp)
	{
		SOCmnList<SODaCItem> itemList;
		itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
		itemList.add(this);
		res = grp->updateItems(fromServer, &itemList, whatParameters);
		itemList.destroy();
	}
	else
	{
		res = E_FAIL;
	}

	setLastError(res, 0);
	return res;
} // update


//-----------------------------------------------------------------------------
// serialization
//

BOOL SODaCItem::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;

	if (isStoring)
	{
		if (!getPersistence())
		{
			return TRUE;
		}

		// store own data
		ar << getAccessPath();
		ar << getItemID();
		ar << (DWORD)getReqDatatype();
		ar << getDeadBand();
	}
	else
	{
		// load own data
		SOCmnPointer<SODaCEntry> entry = getSODaCEntry();
		SOCmnString str;
		DWORD dw = 0;
		FLOAT f = 0.0;
		ar >> str;
		setAccessPath(str);
		ar >> str;
		setItemID(str);
		ar >> dw;
		setReqDatatype((VARTYPE)dw);

		if (entry->getFileVersion() >= MAKELONG(20, 3))
		{
			ar >> f;
			setDeadBand(f);
		}
	}

	return ok;
}




//-----------------------------------------------------------------------------
// SODaCItemIData                                                             |
//-----------------------------------------------------------------------------

SODaCItemIData::SODaCItemIData()
{}

SODaCItemIData::~SODaCItemIData()
{}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SODaCItemIData::getIntTraceId(void)
{
	SOCmnObject* obj;
	obj = getObjectDaCItemIData();

	if (obj)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return g_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectDaCItemIData
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SODaCItemIData::getObjectDaCItemIData(void)
{
	return NULL;
} // getObjectDaCItemIData


//-----------------------------------------------------------------------------
// getWriteValue
//
// - get write value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy write value
//
BOOL SODaCItemIData::getWriteValue(
	OUT SOCmnVariant& value)    // value
{
	value.clear();
	return FALSE;
} // getWriteValue
#ifdef SOOS_WINDOWS
BOOL SODaCItemIData::getWriteValue(
	OUT LPVARIANT value)    // value
{
	::VariantInit(value);
	return FALSE;
} // getWriteValue
#endif


//-----------------------------------------------------------------------------
// getReadValue
//
// - get read value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy read value
//
BOOL SODaCItemIData::getReadValue(
	OUT SOCmnVariant& value)    // value
{
	value.clear();
	return FALSE;
} // getReadValue


//-----------------------------------------------------------------------------
// getReadQuality
//
// - get read quality
//
// returns:
//		OPC_QUALITY_BAD
//
WORD SODaCItemIData::getReadQuality(void) const
{
	return OPC_QUALITY_BAD;
} // getReadQuality


//-----------------------------------------------------------------------------
// getReadTimeStamp
//
// - get read time stamp
//
// returns:
//		FALSE - can't copy read time stamp
//
BOOL SODaCItemIData::getReadTimeStamp(
	OUT SOCmnDateTime& /* timeStamp */) // time stamp
const
{
	return FALSE;
} // getReadTimeStamp


//-----------------------------------------------------------------------------
// getReadResult
//
// - get read result
//
// returns:
//		E_NOTIMPL
//
HRESULT SODaCItemIData::getReadResult(void) const
{
	return E_NOTIMPL;
} // getReadResult


//-----------------------------------------------------------------------------
// getWriteResult
//
// - get write result
//
// returns:
//		E_NOTIMPL
//
HRESULT SODaCItemIData::getWriteResult(void) const
{
	return E_NOTIMPL;
} // getWriteResult


//-----------------------------------------------------------------------------
// setRead
//
// - set read parameters
//
void SODaCItemIData::setRead(
	IN HRESULT res,
	IN SOCmnVariant* value,
	IN WORD quality,
	IN SOCmnDateTime* timeStamp,
	IN DWORD /* transactionID */)
{
	setReadResult(res);

	if (SUCCEEDED(res))
	{
		setReadQuality(quality);

		if (timeStamp)
		{
			setReadTimeStamp(timeStamp);
		}

		setReadValue(value);
	}
} // setRead


//-----------------------------------------------------------------------------
// setReadValue
//
// - set read value
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setReadValue(
	IN SOCmnVariant* /* value */)// new value
{
	return FALSE;
} // setReadValue


//-----------------------------------------------------------------------------
// setReadQuality
//
// - set read quality
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setReadQuality(
	IN WORD /* quality */)  // quality
{
	return FALSE;
} // setReadQuality


//-----------------------------------------------------------------------------
// setReadTimeStamp
//
// - set read quality
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setReadTimeStamp(
	IN  SOCmnDateTime* /* timeStamp */) // time stamp
{
	return FALSE;
} // setReadTimeStamp


//-----------------------------------------------------------------------------
// setReadResult
//
// - set read result
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setReadResult(
	IN HRESULT /* res */)   // read result
{
	return FALSE;
} // setReadResult


//-----------------------------------------------------------------------------
// setWriteValue
//
// - set write value
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setWriteValue(
	IN SOCmnVariant& /* value */)   // new value
{
	return FALSE;
} // setWriteValue


//-----------------------------------------------------------------------------
// setWrite
//
// - set write parameters
//
void SODaCItemIData::setWrite(
	IN HRESULT res,                 // write result
	IN DWORD /* transactionID */)   // transaction id
{
	setWriteResult(res);
} // setWriteResult


//-----------------------------------------------------------------------------
// setWriteResult
//
// - set write result
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIData::setWriteResult(
	IN HRESULT /* res */)   // write result
{
	return FALSE;
} // setWriteResult


//-----------------------------------------------------------------------------
// onSetReadValue
//
// - called before read value is set
//
void SODaCItemIData::onSetReadValue(void)
{} // onSetReadValue


//-----------------------------------------------------------------------------
// onSetReadQuality
//
// - called before read quality is set
//
void SODaCItemIData::onSetReadQuality(void)
{} // onSetReadQuality


//-----------------------------------------------------------------------------
// onSetReadTimeStamp
//
// - called before read time stamp is set
//
void SODaCItemIData::onSetReadTimeStamp(void)
{} // onSetReadTimeStamp


//-----------------------------------------------------------------------------
// onSetReadResult
//
// - called before read result is set
//
void SODaCItemIData::onSetReadResult(void)
{} // onSetReadResult


//-----------------------------------------------------------------------------
// onSetWriteValue
//
// - called before write value is set
//
void SODaCItemIData::onSetWriteValue(void)
{} // onSetWriteValue


//-----------------------------------------------------------------------------
// onSetWriteResult
//
// - called before write result is set
//
void SODaCItemIData::onSetWriteResult(void)
{} // onSetWriteResult




//-----------------------------------------------------------------------------
// SODaCItemDataFull                                                          |
//-----------------------------------------------------------------------------

SODaCItemDataFull::SODaCItemDataFull()
{
	m_readResult = S_OK;
	m_readQuality = OPC_QUALITY_BAD;
	m_writeResult = S_OK;
	m_readTimeStamp.now();
}


//-----------------------------------------------------------------------------
// getWriteValue
//
// - get write value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy write value
//
BOOL SODaCItemDataFull::getWriteValue(
	OUT SOCmnVariant& value)    // value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	BOOL ok = FALSE;
	value.clear();
	ok = value.initFrom(m_writeValue);
	return ok;
} // getWriteValue

#ifdef SOOS_WINDOWS
BOOL SODaCItemDataFull::getWriteValue(
	OUT LPVARIANT value)    // value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	BOOL ok;
	::VariantInit(value);
	ok = (SOCmnVariant::variantCopy(value, &m_writeValue) == S_OK);
	return ok;
}
#endif

//-----------------------------------------------------------------------------
// getReadValue
//
// - get read value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy read value
//
BOOL SODaCItemDataFull::getReadValue(
	OUT SOCmnVariant& value)    // value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	return value.initFrom(&m_readValue);
} // getReadValue


//-----------------------------------------------------------------------------
// setReadValue
//
// - set read value
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setReadValue(
	IN SOCmnVariant* value) // new value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	m_readValue = *value;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetReadValue);
	return TRUE;
} // setReadValue


//-----------------------------------------------------------------------------
// setReadQuality
//
// - set read quality
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setReadQuality(
	IN WORD quality)    // quality
{
	m_readQuality = quality;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetReadQuality);
	return TRUE;
} // setReadQuality


//-----------------------------------------------------------------------------
// setReadTimeStamp
//
// - set read quality
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setReadTimeStamp(
	IN SOCmnDateTime* timeStamp)    // time stamp
{
	m_readTimeStamp = *timeStamp;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetReadTimeStamp);
	return TRUE;
} // setReadTimeStamp


//-----------------------------------------------------------------------------
// setReadResult
//
// - set read result
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setReadResult(
	IN HRESULT res) // read result
{
	m_readResult = res;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetReadResult);
	return TRUE;
} // setReadResult


//-----------------------------------------------------------------------------
// setWriteValue
//
// - set write value
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setWriteValue(
	IN SOCmnVariant& value) // new value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(SOCmnObjectLock::getObjectLockV(this), this);
	m_writeValue = value;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetWriteValue);
	return TRUE;
} // setWriteValue


//-----------------------------------------------------------------------------
// setWriteResult
//
// - get write result
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemDataFull::setWriteResult(
	IN HRESULT res) // write result
{
	m_writeResult = res;
	INVOKE_CALLBACK_ON(getObjectDaCItemIData(), SODaCItemIData, onSetWriteResult);
	return TRUE;
} // setWriteResult


//-----------------------------------------------------------------------------
// getReadQuality
//
// - get read quality
//
// returns:
//		read quality
//
WORD SODaCItemDataFull::getReadQuality(void) const
{
	return m_readQuality;
} // getReadQuality


//-----------------------------------------------------------------------------
// getReadTimeStamp
//
// - get read time stamp
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy read time stamp
//
BOOL SODaCItemDataFull::getReadTimeStamp(
	OUT SOCmnDateTime& timeStamp)   // time stamp
const
{
	timeStamp = m_readTimeStamp;
	return TRUE;
} // getReadTimeStamp


//-----------------------------------------------------------------------------
// getReadResult
//
// - get read result
//
// returns:
//		read result
//
HRESULT SODaCItemDataFull::getReadResult(void) const
{
	return m_readResult;
} // getReadResult


//-----------------------------------------------------------------------------
// getWriteResult
//
// - get write result
//
// returns:
//		write result
//
HRESULT SODaCItemDataFull::getWriteResult(void) const
{
	return m_writeResult;
} // getWriteResult




//-----------------------------------------------------------------------------
// SODaCItemIAttr                                                             |
//-----------------------------------------------------------------------------

SODaCItemIAttr::SODaCItemIAttr()
{}

SODaCItemIAttr::~SODaCItemIAttr()
{}

//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SODaCItemIAttr::getIntTraceId(void)
{
	SOCmnObject* obj;
	obj = getObjectDaCItemIAttr();

	if (obj)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return g_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectDaCItemIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SODaCItemIAttr::getObjectDaCItemIAttr(void)
{
	return NULL;
} // getObjectDaCItemIAttr


//-----------------------------------------------------------------------------
// getReqDatatype
//
// - get requested datatype of the item
//
// returns:
//		requested datatype
//
VARTYPE SODaCItemIAttr::getReqDatatype(void)
{
	return VT_EMPTY;
} // getReqDatatype


//-----------------------------------------------------------------------------
// getNativeDatatype
//
// - get native datatype of the item
//
// returns:
//		VT_EMPTY
//
VARTYPE SODaCItemIAttr::getNativeDatatype(void)
{
	return VT_EMPTY;
} // getNativeDatatype


//-----------------------------------------------------------------------------
// getAccessRights
//
// - get access rights
//
// returns:
//		OPC_READABLE
//
DWORD SODaCItemIAttr::getAccessRights(void)
{
	return OPC_READABLE;
} // getAccessRights


//-----------------------------------------------------------------------------
// getClientHandle
//
// - get client handle
//
// returns:
//		client handle
//
OPCHANDLE SODaCItemIAttr::getClientHandle(void)
{
	return (OPCHANDLE)getObjectDaCItemIAttr()->getOpcHandle();
} // getClientHandle


//-----------------------------------------------------------------------------
// getAccessPath
//
// - get access path
//
// returns:
//		empty string
//
SOCmnString SODaCItemIAttr::getAccessPath(void)
{
	return _T("");
} // getAccessPath


//-----------------------------------------------------------------------------
// getItemPath
//
// - get item path
//
// returns:
//		empty string
//
SOCmnString SODaCItemIAttr::getItemPath(void)
{
	return _T("");
} // getItemPath


//-----------------------------------------------------------------------------
// getItemID
//
// - get item id
//
// returns:
//		empty string
//
SOCmnString SODaCItemIAttr::getItemID(void)
{
	return _T("");
} // getItemID


//-----------------------------------------------------------------------------
// getDeadband
//
// - get deadband
//
// returns:
//		SODACITEM_NO_DEADBAND
//
FLOAT SODaCItemIAttr::getDeadBand(void)
{
	return SODACITEM_NO_DEADBAND;
} // getDeadband


//-----------------------------------------------------------------------------
// getEU
//
// - get EU info and type
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy EU info value
//
BOOL SODaCItemIAttr::getEU(
	OUT OPCEUTYPE* type,    // EU type
	OUT SOCmnVariant& info)     // EU info
{
	if (type)
	{
		*type = OPC_NOENUM;
	}

	info.clear();
	return TRUE;
} // getEU


//-----------------------------------------------------------------------------
// setEU
//
// - set EU info and type
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setEU(
	IN OPCEUTYPE /* type */,    // EU type
	IN SOCmnVariant& /* info */)    // EU info
{
	return FALSE;    // setEU
}


//-----------------------------------------------------------------------------
// setReqDatatype
//
// - set requested datatype
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setReqDatatype(
	IN VARTYPE /* datatype */)  // requested datatype
{
	return FALSE;    // setReqDatatype
}


//-----------------------------------------------------------------------------
// setAccessPath
//
// - set access path
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setAccessPath(
	IN LPCTSTR /* accessPath */)    // access path
{
	return FALSE;    // setAccessPath
}


//-----------------------------------------------------------------------------
// setItemPath
//
// - set item path
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setItemPath(
	IN LPCTSTR /* itemPath */)  // item path
{
	return FALSE;    // setItemPath
}


//-----------------------------------------------------------------------------
// setNativeDatatype
//
// - set native datatype
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setNativeDatatype(
	IN VARTYPE /* datatype */)  // native datatype
{
	return FALSE;    // setNativeDatatype
}


//-----------------------------------------------------------------------------
// setAccessRights
//
// - set access rights
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setAccessRights(
	IN DWORD /* accessRights */)    // access rights
{
	return FALSE;    // setAccessRights
}


//-----------------------------------------------------------------------------
// setItemID
//
// - set item id
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemIAttr::setItemID(
	IN LPCTSTR /* itemID */)    // item id
{
	return FALSE;    // setItemID
}


//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//		TRUE
//
BOOL SODaCItemIAttr::setDeadBand(
	IN FLOAT /* deadBand */)    // deadband
{
	return FALSE;    // setDeadBand
}


//-----------------------------------------------------------------------------
// onSetItemID
//
// - called before item id is set
// - sets the name to the item id
//
void SODaCItemIAttr::onSetItemID(void)
{
} // onSetItemID


//-----------------------------------------------------------------------------
// onSetReqDatatype
//
// - called before requested datatype is set
//
void SODaCItemIAttr::onSetReqDatatype(void)
{} // onSetReqDatatype


//-----------------------------------------------------------------------------
// onSetAccessPath
//
// - called before access path is set
//
void SODaCItemIAttr::onSetAccessPath(void)
{} // onSetAccessPath


//-----------------------------------------------------------------------------
// onSetItemPath
//
// - called before item path is set
//
void SODaCItemIAttr::onSetItemPath(void)
{} // onSetItemPath


//-----------------------------------------------------------------------------
// onSetNativeDatatype
//
// - called before native datatype is set
//
void SODaCItemIAttr::onSetNativeDatatype(void)
{} // onSetNativeDatatype


//-----------------------------------------------------------------------------
// onSetAccessRights
//
// - called before access rights are set
//
void SODaCItemIAttr::onSetAccessRights(void)    // access rights
{} // onSetAccessRights


//-----------------------------------------------------------------------------
// onSetEU
//
// - called before EU type and information is set
//
void SODaCItemIAttr::onSetEU(void)
{} // onSetEU


//-----------------------------------------------------------------------------
// onSetDeadBand
//
// - called after deadband is set
// - default implementation: do nothing
//
void SODaCItemIAttr::onSetDeadBand(void)
{} // onSetDeadBand


//-----------------------------------------------------------------------------
// SODaCItemAttrMin                                                           |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getItemID
//
// - get item id
//
// returns:
//		item id
//
SOCmnString SODaCItemAttrMin::getItemID(void)
{
	return m_itemID;
} // getItemID


//-----------------------------------------------------------------------------
// setItemID
//
// - set item id
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrMin::setItemID(
	IN LPCTSTR itemID)  // item id
{
	m_itemID = itemID;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetItemID)
	return TRUE;
} // setItemID




//-----------------------------------------------------------------------------
// SODaCItemAttrFull                                                          |
//-----------------------------------------------------------------------------

SODaCItemAttrFull::SODaCItemAttrFull()
{
	m_reqDatatype = VT_EMPTY;
	m_nativeDatatype = VT_EMPTY;
	m_accessRights = 0;
	m_EUType = OPC_NOENUM;
	m_deadBand = SODACITEM_NO_DEADBAND;
}


//-----------------------------------------------------------------------------
// getReqDatatype
//
// - get requested datatype of the item
//
// returns:
//		requested datatype
//
VARTYPE SODaCItemAttrFull::getReqDatatype(void)
{
	return m_reqDatatype;
} // getReqDatatype


//-----------------------------------------------------------------------------
// getNativeDatatype
//
// - get native datatype of the item
//
// returns:
//		native datatype
//
VARTYPE SODaCItemAttrFull::getNativeDatatype(void)
{
	return m_nativeDatatype;
} // getNativeDatatype


//-----------------------------------------------------------------------------
// getAccessRights
//
// - get access rights
//
// returns:
//		access rights
//
DWORD SODaCItemAttrFull::getAccessRights(void)
{
	return m_accessRights;
} // getAccessRights


//-----------------------------------------------------------------------------
// getAccessPath
//
// - get access path
//
// returns:
//		access path
//
SOCmnString SODaCItemAttrFull::getAccessPath(void)
{
	return m_itemAndAccessPath;
} // getAccessPath


//-----------------------------------------------------------------------------
// getItemPath
//
// - get access path
//
// returns:
//		item path
//
SOCmnString SODaCItemAttrFull::getItemPath(void)
{
	return m_itemAndAccessPath;
} // getItemPath


//-----------------------------------------------------------------------------
// getItemID
//
// - get item id
//
// returns:
//		item id
//
SOCmnString SODaCItemAttrFull::getItemID(void)
{
	return m_itemID;
} // getItemID


//-----------------------------------------------------------------------------
// getDeadband
//
// - get deadband
//
// returns:
//		SODACITEM_NO_DEADBAND
//
FLOAT SODaCItemAttrFull::getDeadBand(void)
{
	return m_deadBand;
} // getDeadband


//-----------------------------------------------------------------------------
// setEU
//
// - set EU info and type
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setEU(
	IN OPCEUTYPE type,  // EU type
	IN SOCmnVariant& info)  // EU info
{
	m_EUType = type;
	m_EUInfo = info;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetEU)
	return TRUE;
} // setEU


//-----------------------------------------------------------------------------
// getEU
//
// - get EU info and type
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy EU info value
//
BOOL SODaCItemAttrFull::getEU(
	OUT OPCEUTYPE* type,    // EU type
	OUT SOCmnVariant& info)     // EU info
{
	BOOL ok;

	if (type)
	{
		*type = m_EUType;
	}

	info.clear();
	ok = info.initFrom(m_EUInfo);
	return ok;
} // getEU


//-----------------------------------------------------------------------------
// setReqDatatype
//
// - set requested datatype
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setReqDatatype(
	IN VARTYPE datatype)    // requested datatype
{
	m_reqDatatype = datatype;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetReqDatatype)
	return TRUE;
} // setReqDatatype


//-----------------------------------------------------------------------------
// setAccessPath
//
// - set access path
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setAccessPath(
	IN LPCTSTR accessPath)  // access path
{
	m_itemAndAccessPath = accessPath;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetAccessPath)
	return TRUE;
} // setAccessPath


//-----------------------------------------------------------------------------
// setItemPath
//
// - set item path
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setItemPath(
	IN LPCTSTR itemPath)    // item path
{
	m_itemAndAccessPath = itemPath;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetItemPath)
	return TRUE;
} // setItemPath


//-----------------------------------------------------------------------------
// setNativeDatatype
//
// - set native datatype
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setNativeDatatype(
	IN VARTYPE datatype)    // native datatype
{
	m_nativeDatatype = datatype;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetNativeDatatype)
	return TRUE;
} // setNativeDatatype


//-----------------------------------------------------------------------------
// setAccessRights
//
// - set access rights
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setAccessRights(
	IN DWORD accessRights)  // access rights
{
	m_accessRights = accessRights;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetAccessRights)
	return TRUE;
} // setAccessRights


//-----------------------------------------------------------------------------
// setItemID
//
// - set item id
//
// returns:
//		TRUE  - all ok
//
BOOL SODaCItemAttrFull::setItemID(
	IN LPCTSTR itemID)  // item id
{
	m_itemID = itemID;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetItemID)
	return TRUE;
} // setItemID

//-----------------------------------------------------------------------------
// setDeadBand
//
// - set deadband
//
// returns:
//		TRUE  - set deadband
//    FALSE - invalid deadband
//
BOOL SODaCItemAttrFull::setDeadBand(
	IN FLOAT deadBand)  // deadband
{
	if (((deadBand < 0.0) || (deadBand > 100.0)) && (deadBand != SODACITEM_NO_DEADBAND))
	{
		return FALSE;
	}

	m_deadBand = deadBand;
	INVOKE_CALLBACK_ON(getObjectDaCItemIAttr(), SODaCItemIAttr, onSetDeadBand)
	return TRUE;
} // setDeadBand



//-----------------------------------------------------------------------------
// SODaCItemDefault                                                           |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCItemDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


//-----------------------------------------------------------------------------
// getObjectDaCItemIData
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCItemDefault::getObjectDaCItemIData(void)
{
	return (SOCmnObject*)this;
} // getObjectDaCItemIData


//-----------------------------------------------------------------------------
// getObjectDaCItemIAttr
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCItemDefault::getObjectDaCItemIAttr(void)
{
	return (SOCmnObject*)this;
} // getObjectDaCItemIAttr


//-----------------------------------------------------------------------------
// getClientHandle
//
// - get client handle (overwrites SODaCItemIAttr::getClientHandle)
//
// returns:
//		client handle
//
OPCHANDLE SODaCItemDefault::getClientHandle(void)
{
	return (OPCHANDLE)this->getOpcHandle();
} // getClientHandle

#endif // SODAC
