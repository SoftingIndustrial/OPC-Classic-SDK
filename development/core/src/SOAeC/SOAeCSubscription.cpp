#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCEntry.h"
#include "SOAeCServer.h"
#include "SOAeCSubscription.h"
#include "SOAeCCallback.h"
#include "SOAeCEvent.h"



//-----------------------------------------------------------------------------
// SOAeCSubscription                                                          |
//-----------------------------------------------------------------------------


SOAeCSubscription::SOAeCSubscription()
	: SOCltElement(SOCMNOBJECT_TYPE_SUBSCRIPTION)
{
	m_dwBufferTime = 0;
	m_dwMaxSize = 0;
	m_dwEventTypes = OPC_ALL_EVENTS;
	m_dwLowSeverity = 1;
	m_dwHighSeverity = 1000;
	m_areaList.create();
	m_sourceList.create();
	m_categoryList.create();
	m_dwRevisedBufferTime = 0;
	m_dwRevisedMaxSize = 0;
	m_bActive = FALSE;
	m_retAttrList.create();
}

SOAeCSubscription::~SOAeCSubscription()
{
	m_retAttrList.destroy();
}


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key (name)
//
// returns:
//		object key
//
BOOL SOAeCSubscription::getKey(
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
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOAeCSubscription::hasKey(
	IN DWORD key)   // object key
{
	return (this->getOpcHandle() == key);
} // hasKey


//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the group if the target state differs from the
//   actual state of the group

HRESULT SOAeCSubscription::doStateTransition(BOOL /* deep */, IN BYTE /* stateDeep */)
{
	return doStateTransitionImpl();
} // doStateTransition

HRESULT SOAeCSubscription::doDisconnect(void)
{
	return doDisconnectImpl();
}

HRESULT SOAeCSubscription::doUpdate(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	return doUpdateImpl(fromServer, whatParameters);
} // update

#ifdef SOFEATURE_DCOM

HRESULT SOAeCSubscription::queryInterface(IN REFIID riid, IN void** interf)
{
	return queryInterfaceImpl(riid, interf);
}

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// getReturnedAttributes
//
// - get the returned attributes list for a event category
//
// returns:
//		returned attributes list
//
SOCmnDWORDList* SOAeCSubscription::getReturnedAttributes(
	IN DWORD category)  // category
{
	BOOL ok = TRUE;
	// find existing list entry
	SOAeCReturnedAttrListList retAttrList(m_retAttrList);
	SOCmnDWORDList* retAttr = retAttrList.findKey(category);

	if (!retAttr)
	{
		SOCMN_ALLOCATE_OBJECT(retAttr, SOCmnDWORDList());

		if (ok)
		{
			ok &= retAttr->create(SOCMNLIST_TYPE_FLAGS | SOCMNLIST_TYPE_ORG_LIST |
								  SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DWORD);
		}

		if (ok)
		{
			ok &= retAttrList.add(retAttr, category);
		}

		if (!ok && retAttr)
		{
			delete retAttr;
			retAttr = NULL;
		}
	}

	return retAttr ;
} // getReturnedAttributes



//-----------------------------------------------------------------------------
// onReceivedEvents
//

void SOAeCSubscription::onReceivedEvents(
	DWORD /* count */,
	SOAeCEvent** /* pEvents */,
	BYTE /* refresh */,
	BYTE /* lastRefresh */)
{
}

//-----------------------------------------------------------------------------
// onSetBufferTime
//

void SOAeCSubscription::onSetBufferTime(void)
{
} //onSetBufferTime


//-----------------------------------------------------------------------------
// onSetMaxSize
//

void SOAeCSubscription::onSetMaxSize(void)
{
} //onSetMaxSize

//-----------------------------------------------------------------------------
// onSetLowSeverity
//

void SOAeCSubscription::onSetLowSeverity(void)
{
} //onSetLowSeverity

//-----------------------------------------------------------------------------
// onSetHighSeverity
//

void SOAeCSubscription::onSetHighSeverity(void)
{
} //onSetHighSeverity

//-----------------------------------------------------------------------------
// onSetEventTypes
//

void SOAeCSubscription::onSetEventTypes(void)
{
} //onSetEventTypes

//-----------------------------------------------------------------------------
// onSetActive
//

void SOAeCSubscription::onSetActive(void)
{
} //onSetActive

//-----------------------------------------------------------------------------
// onSetBufferTime
//

void SOAeCSubscription::onSetRevisedBufferTime(void)
{
} //onSetBufferTime


//-----------------------------------------------------------------------------
// onSetMaxSize
//

void SOAeCSubscription::onSetRevisedMaxSize(void)
{
} //onSetMaxSize


//-----------------------------------------------------------------------------
// setBufferTime
//

BOOL SOAeCSubscription::setBufferTime(
	IN DWORD bufferTime)
{
	m_dwBufferTime  = bufferTime;
	INVOKE_CALLBACK(SOAeCSubscription, onSetBufferTime);
	return TRUE;
} // setBufferTime

//-----------------------------------------------------------------------------
// setMaxSize
//

BOOL SOAeCSubscription::setMaxSize(
	IN DWORD MaxSize)
{
	m_dwMaxSize  = MaxSize;
	INVOKE_CALLBACK(SOAeCSubscription, onSetMaxSize);
	return TRUE;
} // setMaxSize

//-----------------------------------------------------------------------------
// setLowSeverity
//

BOOL SOAeCSubscription::setLowSeverity(
	IN DWORD LowSeverity)
{
	m_dwLowSeverity  = LowSeverity;
	INVOKE_CALLBACK(SOAeCSubscription, onSetLowSeverity);
	return TRUE;
} // setLowSeverity

//-----------------------------------------------------------------------------
// setHighSeverity
//

BOOL SOAeCSubscription::setHighSeverity(
	IN DWORD HighSeverity)
{
	m_dwHighSeverity  = HighSeverity;
	INVOKE_CALLBACK(SOAeCSubscription, onSetHighSeverity);
	return TRUE;
} // setHighSeverity

//-----------------------------------------------------------------------------
// setEventTypes
//

BOOL SOAeCSubscription::setEventTypes(
	IN DWORD EventTypes)
{
	m_dwEventTypes  = EventTypes;
	INVOKE_CALLBACK(SOAeCSubscription, onSetEventTypes);
	return TRUE;
} // setEventTypes

//-----------------------------------------------------------------------------
// setEventTypes
//

BOOL SOAeCSubscription::setActive(
	IN BOOL active)
{
	m_bActive = active;
	INVOKE_CALLBACK(SOAeCSubscription, onSetActive);
	return TRUE;
} // setEventTypes

BYTE SOAeCSubscription::getProtocol(void)
{
	SOCmnPointer<SOAeCServer> server = (SOAeCServer*)getParentBranch();

	if (server.isNotNull())
	{
		return server->getProtocol();
	}

	return 0;
}

SOAeCSubscription* SOAeCSubscription::getObjectAeCSubscription(void)
{
	return this;
}

BOOL SOAeCSubscription::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;

	if (!getPersistence())
	{
		return TRUE;
	}

	if (isStoring)
	{
		ar << m_dwBufferTime;
		ar << m_dwMaxSize;
		ar << m_dwLowSeverity;
		ar << m_dwHighSeverity;
		ar << m_dwEventTypes;
	}
	else
	{
		SOCmnPointer<SOAeCEntry> entryAE = ::getSOAeCEntry();

		if (entryAE->getFileVersion() >= MAKELONG(4, 3))    // Version 3.04
		{
			ar >> m_dwBufferTime;
			ar >> m_dwMaxSize;
			ar >> m_dwLowSeverity;

			if (m_dwLowSeverity == 0)
			{
				m_dwLowSeverity = 1;    // OPC AE 1.03 change !
			}

			ar >> m_dwHighSeverity;
			ar >> m_dwEventTypes;
		}
	}

	SOCmnStringList listSrc(getSourceList());
	SOCmnStringList listArea(getAreaList());
	SOCmnDWORDList  listCategory(getCategoryList());
	serialize_list(ar, isStoring, listSrc);
	serialize_list(ar, isStoring, listArea);
	serialize_list(ar, isStoring, listCategory);
	// Returned attribute list
	SOAeCReturnedAttrListList retAttrList(m_retAttrList);
	SOCmnListPosition pos = retAttrList.getStartPosition();

	if (isStoring)
	{
		size_t category = 0;
		ar << retAttrList.getCount();

		while (pos)
		{
			SOCmnDWORDList list(retAttrList.getNext(pos, &category));
			ar << (DWORD)category;
			serialize_list(ar, TRUE, list);
		}
	}
	else
	{
		DWORD dwCount = 0;
		DWORD category = 0;
		ar >> dwCount;

		while (dwCount--)
		{
			category = 0;
			ar >> category;
			SOCmnDWORDList list(getReturnedAttributes(category));
			serialize_list(ar, FALSE, list);
		}
	}

	return ok;
}



//-----------------------------------------------------------------------------
// SOAeCSubscriptionICommunication                                            |
//-----------------------------------------------------------------------------

SOAeCSubscriptionICommunication::SOAeCSubscriptionICommunication(void)
{}
SOAeCSubscriptionICommunication::~SOAeCSubscriptionICommunication(void)
{}
SOAeCSubscription* SOAeCSubscriptionICommunication::getObjectAeCSubscription(void)
{
	return NULL;
}
BOOL SOAeCSubscriptionICommunication::refresh(void)
{
	return FALSE;
}
BOOL SOAeCSubscriptionICommunication::cancelRefresh(void)
{
	return FALSE;
}
HRESULT SOAeCSubscriptionICommunication::doStateTransitionImpl(void)
{
	return E_NOTIMPL;
}
HRESULT SOAeCSubscriptionICommunication::doUpdateImpl(IN BOOL /* fromServer */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}
HRESULT SOAeCSubscriptionICommunication::doDisconnectImpl(void)
{
	return E_NOTIMPL;
}

#ifdef SOFEATURE_DCOM
HRESULT SOAeCSubscriptionICommunication::queryInterfaceImpl(IN REFIID /* riid */, IN void** /* interf */)
{
	return E_NOTIMPL;
}
void SOAeCSubscriptionICommunication::receivedEvents(IN OPCHANDLE /*hClientSubscription */, IN BOOL /* bRefresh */, IN BOOL /* bLastRefresh */, IN DWORD /* dwCount */, IN ONEVENTSTRUCT* /* pEvents */)
{}
#endif

#endif // SOAEC
