#include "stdafx.h"
#include "SOCmn.h"
#include "SOCmnObject.h"
#include "SOCmnFunctions.h"
#include "SOCmnTrace.h"
#include "SOCmnHandleManager.h"

//-----------------------------------------------------------------------------
// SOCmnObject                                                                |
//-----------------------------------------------------------------------------

SOCmnObject::SOCmnObject(
	IN DWORD objType)   // object type
{
	m_opcHandle = PROVIDE_OPC_HANDLE(this);
	m_refCount = 0;
	m_objectType = objType;
	m_objectState = 0;
	m_objectStateTarget = 0;
	m_userData = 0;
	m_flags = 0;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OBJ_REF, (_T("create")));
	_TRACE_CREATE_GENOBJ();
	addRef();
}

DWORD SOCmnObject::getOpcHandle(void)
const
{
	return m_opcHandle;
} // getOpcHandle

SOCmnObject::~SOCmnObject(void)
{
	RELEASE_OPC_HANDLE(m_opcHandle);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OBJ_REF, (_T("free")))
	_TRACE_DELETE_GENOBJ();
	// The reference count (m_refCount) of an object MUST be 0 in the
	// destuctor of the object. With a reference count greater than 0 here
	// you will have some references pointing to an invalid object.
	// Check for:
	// - delete calls on objects
	// - local created objects
	_ASSERTION((m_refCount == 0), "The reference count of an object must be 0 in the destuctor of the object");
	m_refCount = -1;
	m_objectType = 0;
}


//-----------------------------------------------------------------------------
// addRef
//
// - increment reference counter
//
LONG SOCmnObject::addRef(void)
{
	LONG refCount = interlockedIncrement(&m_refCount);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("addRef %li"), refCount))
	return refCount;
} // addRef


//-----------------------------------------------------------------------------
// release
//
// - decrement reference counter
// - free object if no reference left
//
// returns:
//		reference count
//
LONG SOCmnObject::release(void)
{
	LONG refCount = interlockedDecrement(&m_refCount);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("release %li"), refCount))

	if (refCount <= 0)
	{
		SOCMN_FREE_OBJECT_THIS  // last reference -> free object
		return 0;
	}

	return refCount;
} // release


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key with given string
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOCmnObject::hasKey(
	IN LPCTSTR key) // object key
{
	SOCmnString myKey;
	myKey.format(_T("%8.8lX"), (DWORD)this->getOpcHandle());
	return (key == myKey);
} // hasKey

BOOL SOCmnObject::hasKey(
	IN DWORD key)   // object key
{
	return (key == (DWORD)this->getOpcHandle());
} // hasKey

//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key
//
// returns:
//		object key
//
BOOL SOCmnObject::getKey(OUT SOCmnString& key)
{
	key.format(_T("%8.8lX"), (DWORD)this->getOpcHandle());
	return TRUE;
} // getKey

BOOL SOCmnObject::getKey(OUT DWORD& key)
{
	key = (DWORD)this->getOpcHandle();
	return TRUE;
} // getKey

BOOL SOCmnObject::hasKeyType(IN DWORD keyType)
{
	if (keyType == SOCMNOBJECT_KEYTYPE_DWORD)
	{
		return TRUE;
	}

	if (keyType == SOCMNOBJECT_KEYTYPE_STRING)
	{
		return TRUE;
	}

	return FALSE;
} // getKey


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get and creates objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOCmnObject::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[%s|%8.8lX|]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// setObjectState
//
// - set the object state
//
// returns:
//		TRUE
//
BOOL SOCmnObject::setObjectState(
	IN BYTE state)  // object state
{
	m_objectState = state;
	onSetObjectState();
	return TRUE;
} // setObjectState


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called before the new object state is set
//
void SOCmnObject::onSetObjectState()
{ } // onSetObjectState


//-----------------------------------------------------------------------------
// getObjectState
//
// - get the object state
//
// returns:
//		object state
//
BYTE SOCmnObject::getObjectState(void) const
{
	return m_objectState;
} // getObjectState


//-----------------------------------------------------------------------------
// setTargetObjectState
//
// - set the target state of the object
//
// returns:
//		TRUE
//
BOOL SOCmnObject::setTargetObjectState(
	IN BYTE state)  // object state
{
	m_objectStateTarget = state;
	onSetTargetObjectState();
	return TRUE;
} // setTargetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called before the new object state is set
//
void SOCmnObject::onSetTargetObjectState()
{} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// getTargetObjectState
//
// - get the target object state
//
// returns:
//		object state
//
BYTE SOCmnObject::getTargetObjectState(void) const
{
	return m_objectStateTarget;
} // getTargetObjectState


//-----------------------------------------------------------------------------
// getObjTypeString
//
// - get objects type string
//
// returns:
//		type string
//
SOCmnString SOCmnObject::getObjTypeString(void)
{
	SOCmnString objectTypeString;           // object type string
#ifdef SOFEATURE_CMN_OPC

	switch (m_objectType)
	{
	case SOCMNOBJECT_TYPE_CALLBACK:
		objectTypeString = _T("CBK");
		break;

	case SOCMNOBJECT_TYPE_ENTRY:
		objectTypeString = _T("ENT");
		break;

	case SOCMNOBJECT_TYPE_REQUEST:
		objectTypeString = _T("REQ");
		break;

	case SOCMNOBJECT_TYPE_CACHE:
		objectTypeString = _T("CAH");
		break;

	case SOCMNOBJECT_TYPE_ELEMENT:
		objectTypeString = _T("ELT");
		break;

	case SOCMNOBJECT_TYPE_TRANSACTION:
		objectTypeString = _T("TAC");
		break;

	case SOCMNOBJECT_TYPE_LEAF:
		objectTypeString = _T("LEF");
		break;

	case SOCMNOBJECT_TYPE_BRANCH:
		objectTypeString = _T("BRA");
		break;

	case SOCMNOBJECT_TYPE_SERVER_DA:
		objectTypeString = _T("SRD");
		break;

	case SOCMNOBJECT_TYPE_SERVER_AE:
		objectTypeString = _T("SRA");
		break;

	case SOCMNOBJECT_TYPE_GROUP:
		objectTypeString = _T("GRP");
		break;

	case SOCMNOBJECT_TYPE_NODE:
		objectTypeString = _T("NOD");
		break;

	case SOCMNOBJECT_TYPE_TAG:
		objectTypeString = _T("TAG");
		break;

	case SOCMNOBJECT_TYPE_ITEM:
		objectTypeString = _T("ITM");
		break;

	case SOCMNOBJECT_TYPE_ENTRY_DAC:
		objectTypeString = _T("ENC");
		break;

	case SOCMNOBJECT_TYPE_OTHER:
		objectTypeString = _T("OTH");
		break;

	case SOCMNOBJECT_TYPE_PROPERTY:
		objectTypeString = _T("PRO");
		break;

	case SOCMNOBJECT_TYPE_ITEM_TAG:
		objectTypeString = _T("ITG");
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE:
		objectTypeString = _T("ATV");
		break;

	case SOCMNOBJECT_TYPE_EVENT:
		objectTypeString = _T("EVT");
		break;

	case SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT:
		objectTypeString = _T("AEL");
		break;

	case SOCMNOBJECT_TYPE_ROOT:
		objectTypeString = _T("ROT");
		break;

	case SOCMNOBJECT_TYPE_SUBSCRIPTION:
		objectTypeString = _T("SUS");
		break;

	case SOCMNOBJECT_TYPE_CATEGORY:
		objectTypeString = _T("CAT");
		break;

	case SOCMNOBJECT_TYPE_ATTRIBUTE:
		objectTypeString = _T("ATR");
		break;

	case SOCMNOBJECT_TYPE_CONDITION_CLASS:
		objectTypeString = _T("COC");
		break;

	case SOCMNOBJECT_TYPE_CONDITION:
		objectTypeString = _T("CON");
		break;

	case SOCMNOBJECT_TYPE_SUBCONDITION_CLASS:
		objectTypeString = _T("SUC");
		break;

	case SOCMNOBJECT_TYPE_SOURCE:
		objectTypeString = _T("SRC");
		break;

	case SOCMNOBJECT_TYPE_AREA:
		objectTypeString = _T("ARE");
		break;

	case SOCMNOBJECT_TYPE_AREA_BROWSER:
		objectTypeString = _T("BAR");
		break;

	case SOCMNOBJECT_TYPE_EVENT_BROWSER:
		objectTypeString = _T("BEV");
		break;

	case SOCMNOBJECT_TYPE_NAMESPACE_BROWSER:
		objectTypeString = _T("BNA");
		break;

	default:
		objectTypeString = _T("???");
		break;
	}

#endif // SOFEATURE_CMN_OPC
	return objectTypeString;
} // getObjTypeString


//-----------------------------------------------------------------------------
// getActObjTraceIdString
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOCmnObject::getActObjTraceIdString(void)
{
#ifdef SOFEATURE_CMN_TRACE
	return m_objectTraceId;
#else
	return _T("");
#endif
} // getActObjTraceIdString


//-----------------------------------------------------------------------------
// setObjTraceId
//
// - set objects trace string id
//
// returns:
//		TRUE
//
BOOL SOCmnObject::setObjTraceId(
	IN LPCTSTR traceId)
{
#ifdef SOFEATURE_CMN_TRACE

	if (traceId)
	{
		m_objectTraceId = traceId;
	}
	else
	{
		m_objectTraceId.empty();
	}

	return TRUE;
#else
	return FALSE;
#endif
} // setObjTraceId


//-----------------------------------------------------------------------------
// isInitObjTraceId
//
// - check if the trace string id is initialized
//
// returns:
//		TRUE
//
BOOL SOCmnObject::isInitObjTraceId(void)
{
#ifdef SOFEATURE_CMN_TRACE
	return (!m_objectTraceId.isEmpty());
#else
	return FALSE;
#endif
} // isInitObjTraceId


//-----------------------------------------------------------------------------
// getUserData
//
// - get user data
//
// returns:
//		user data
//
BYTE SOCmnObject::getUserData(void) const
{
	return m_userData;
} // getUserData


//-----------------------------------------------------------------------------
// setUserData
//
// - set user data
//
void SOCmnObject::setUserData(
	IN BYTE userData)   // user data
{
	m_userData = userData;
} // setUserData


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOCmnObject::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{} // getObjWatchData


//-----------------------------------------------------------------------------
// getCollectChanges
//
// - get if to collect changes
//
// returns:
//		TRUE  - collect changes
//		FALSE - don't collect changes
//
BYTE SOCmnObject::getCollectChanges(void)
const
{
	return (m_flags & SOCMNOBJECT_FLAG_COLLECT_MASK);
} // getCollectChanges


//-----------------------------------------------------------------------------
// setCollectChanges
//
// - set if to collect changes
//
// returns:
//		TRUE
//
BOOL SOCmnObject::setCollectChanges(
	IN BYTE doCollect) // collect changes or not
{
	m_flags &= ~SOCMNOBJECT_FLAG_COLLECT_MASK;
	m_flags |= (doCollect & SOCMNOBJECT_FLAG_COLLECT_MASK);
	return TRUE;
} // setCollectChanges

#endif // SOFEATURE_WATCH

