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
//  Filename    : SOSrvEntry.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Entry point for Server Toolkit                              |
//                - SOSrvEntry: Entry point class                             |
//                - SOSrvEventHandler: Handler for toolkit events             |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"

#ifdef SOSRV

#include "SOVersion.h"
#ifdef SOFEATURE_DCOM
#include "comcat.h"
#endif

#ifdef SOOS_WINDOWS_CE
#ifndef SOFLAG_NO_DLLS
CComModule _Module;
#endif
#endif

#ifdef SOFEATURE_DCOM
// OPC Server categories
EXTERN_C const GUID CATID_OPCDAServer10;
EXTERN_C const GUID CATID_OPCDAServer20;
EXTERN_C const IID IID_OPCEventServerCATID;
#endif

SOSrvEntry* g_srvEntry = NULL;

#ifdef SOFLAG_DEMO_VERSION
DWORD g_demoStamp = GetTickCount();
#endif

void setSOSrvEntry(
	IN SOSrvEntry* entry)
{
	g_srvEntry = entry;
}

SOSrvEntry* getSOSrvEntry(void)
{
	if (g_srvEntry)
	{
		g_srvEntry->addRef();
	}

	return g_srvEntry;
}


SOSrvConverter* getSOSrvConverter(void)
{
	if (g_srvEntry)
	{
		return g_srvEntry->getConverter();
	}

	return NULL;
}

#ifdef SOFEATURE_DCOM
static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
#endif


//-----------------------------------------------------------------------------
// SOSrvEventHandler                                                          |
//-----------------------------------------------------------------------------

SOSrvEventHandler::SOSrvEventHandler(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_CALLBACK)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("EVENT"));
	setObjTraceId(id);
}

void SOSrvEventHandler::onReleasedLastOPCObject(void)
{}

void SOSrvEventHandler::onReleasedOPCObject(void)
{}



//-----------------------------------------------------------------------------
// SOSrvCreator                                                               |
//-----------------------------------------------------------------------------

SOSrvCreator::SOSrvCreator(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_CALLBACK)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("Creator"));
	setObjTraceId(id);
}

SOSrvConverter* SOSrvCreator::createConverter(void)
{
	SOSrvConverter* conv;
	SOCMN_ALLOCATE_OBJECT(conv, SOSrvConverter()) return conv;
}

#ifdef SOFEATURE_WATCH
SOSrvWatch* SOSrvCreator::createWatch(void)
{
	SOSrvWatch* watch;
	SOCMN_ALLOCATE_OBJECT(watch, SOSrvWatch()) return watch;
}
#endif


//-----------------------------------------------------------------------------
// SOSrvEntry                                                                 |
//-----------------------------------------------------------------------------

SOSrvEntry::SOSrvEntry(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_ENTRY)
{
	m_initialized = 0;
	m_terminateLock = TRUE;
#ifdef SOFEATURE_DCOM
	m_clientCheckInterval = INFINITE;
	m_clientCheckAllIFs = FALSE;
	m_type = OutProc;
	m_appInstanceHandle = 0;
	memset(&m_clsid, 0, sizeof(CLSID));
#endif
	SOCMN_ALLOCATE_OBJECT(m_evtHandler, SOSrvEventHandler())
	SOCMN_ALLOCATE_OBJECT_C(m_converter, SOSrvConverter(), (SOSrvConverter*))
#ifdef SOFEATURE_DCOM
	m_shutdownList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_SYNCED);
#endif
	// server information
	m_vendorInfo = _T("Softing OPC Server Development Kit");
	m_majorVersion = MAJOR_VERSION_NUMBER;
	m_minorVersion = MINOR_VERSION_NUMBER;
	m_patchVersion = PATCH_VERSION_NUMBER;
	m_buildNumber  = BUILD_NUMBER;
#ifdef SOFEATURE_DCOM
	// register information
	m_regRGSResID = 0;
	m_regCategories = 0;
#endif
	// flags
	m_entryFlags = 0xFF;
#ifdef SOFEATURE_DCOM
	// security
	m_serverAuthLevel = 0;
	m_callbackAuthLevel = 0;
	m_serverImpLevel = 0;
	m_callbackImpLevel = 0;
#endif
	m_objectRoot = NULL;
	// watch objects
	m_watchList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
	m_defWorkerThreads = 16;
	m_pThreadPool = NULL;
#ifdef SOFEATURE_SOAP
	// HTTP server
	m_srvHTTP = new SOSrvHTTP();
#endif
#ifdef SOFEATURE_TUNNEL
	// TP server
	m_srvTP = new SOSrvTP();
#endif
	m_objectDataExpiryTimeout = OTS_ELEMENTDATA_DISABLED_QW; // 60 seconds
	m_objectDataRecheckMinWait = 120000 * 10000; // 120 seconds
	m_objectDataLastCleanup = 0;
	m_objectDataMinCleanupElementsCount = 1000;
	m_objectDataMaxCleanedElementsPercent = 20;
}


SOSrvEntry::~SOSrvEntry(void)
{
	// free objects
	setEventHandler(NULL);

	if (m_objectRoot)
	{
		m_objectRoot->release();
	}

	delete m_pThreadPool;
#ifdef SOFEATURE_DCOM
	// clear shutdown list
	m_shutdownList.destroy();
	m_regRGSRepList.destroy();
#endif
#ifdef SOFEATURE_TUNNEL
	m_srvTP.release();
#endif
#ifdef SOFEATURE_SOAP
	m_srvHTTP.release();
#endif
	if (m_stringBasedObjectDataEx.isInit())
	{
		m_stringBasedObjectDataEx.lock();
		SOCmnListPosition pos = m_stringBasedObjectDataEx.getStartPosition();
		OTSAddressSpaceElementDataEx* pElemData;

		while (pos)
		{
			pElemData = m_stringBasedObjectDataEx.getNext(pos);

			if (pElemData->elementData)
			{
				if (pElemData->elementData->m_itemID)
				{
					OTFreeMemory(pElemData->elementData->m_itemID);
				}

				if (pElemData->elementData->m_name)
				{
					OTFreeMemory(pElemData->elementData->m_name);
				}

				OTFreeMemory(pElemData->elementData);
			}
		}

		m_stringBasedObjectDataEx.clear();
		m_stringBasedObjectDataEx.unlock();
	}
}


//-----------------------------------------------------------------------------
// release
//
// - decrement reference counter
// - free object if no reference left and terminate lock not set
//
// returns:
//		reference count of the object
//
LONG SOSrvEntry::release(void)
{
	BOOL terminate = m_terminateLock;
	LONG refCount = interlockedDecrement(&m_refCount);
	_TRACE(TL_DEB, SOCMNTRACE_G_OBJ_REF, (_T("release %li locked:%s"), refCount, BOOL2STR(terminate)))

	if ((refCount <= 0) && (!terminate))
	{
		SOCMN_FREE_OBJECT_THIS  // last reference and terminate flag set -> free object
		return 0;
	}

	return refCount;
} // release


void SOSrvEntry::setStringBasedObjectDataExpiry(DWORD milliseconds)
{
	if (milliseconds == OTS_ELEMENTDATA_DISABLED_DW)
	{
		m_objectDataExpiryTimeout = OTS_ELEMENTDATA_DISABLED_QW;
	}
	else
	{
		m_objectDataExpiryTimeout = milliseconds * 10000; // to 100ns-filetime resolution
	}
}

DWORD SOSrvEntry::getStringBasedObjectDataExpiry() const
{
	if (m_objectDataExpiryTimeout == OTS_ELEMENTDATA_DISABLED_QW)
	{
		return OTS_ELEMENTDATA_DISABLED_DW; // disabled
	}
	else if (m_objectDataExpiryTimeout == OTS_ELEMENTDATA_INFINITE_QW)
	{
		return OTS_ELEMENTDATA_INFINITE_DW; // never expires
	}

	return static_cast<DWORD>(m_objectDataExpiryTimeout / 10000); // reduce 100-nanosecond resolution of FILETIME
}


void SOSrvEntry::setStringBasedMinRecheck(DWORD milliseconds)
{
	m_objectDataRecheckMinWait = milliseconds * 10000; // to 100ns-filetime resolution
}

void SOSrvEntry::setStringBasedMinCleanupElementsCount(DWORD count)
{
	m_objectDataMinCleanupElementsCount = count;
}

void SOSrvEntry::setStringBasedMaxCleanupElementsPercent(double percent)
{
	m_objectDataMaxCleanedElementsPercent = percent;
}

BOOL SOSrvEntry::setStringBasedObjectDataEx(IN const SOCmnString& pName, IN OTSAddressSpaceElementData* pElementData, IN unsigned short elementDataFilter /* = OTS_ELEMENTDATA_ALL */)
{
	if ((m_objectDataExpiryTimeout == OTS_ELEMENTDATA_DISABLED_QW) || (elementDataFilter == OTS_ELEMENTDATA_NONE))
	{
		return FALSE;
	}

	if (!m_stringBasedObjectDataEx.isInit())
	{
		SOCmnListConfig cfg;
		cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_VOID | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		m_stringBasedObjectDataEx.create(&cfg);
	}

	unsigned long long now;
	GetSystemTimeAsFileTime((FILETIME*)&(now));
	m_stringBasedObjectDataEx.lock();
	OTSAddressSpaceElementDataEx *pExistingElementDataEx = NULL;

	if (m_stringBasedObjectDataEx.findKey(pName, &pExistingElementDataEx))
	{
		// if found, operate on found object
		if (now > pExistingElementDataEx->timeStamp + m_objectDataExpiryTimeout)
		{
			// if expired, clear everything
			if (pExistingElementDataEx->elementData->m_name)
			{
				OTFreeMemory(pExistingElementDataEx->elementData->m_name);
			}

			if (pExistingElementDataEx->elementData->m_itemID)
			{
				OTFreeMemory(pExistingElementDataEx->elementData->m_itemID);
			}

			ZeroMemory(pExistingElementDataEx->elementData, sizeof(OTSAddressSpaceElementData));
			pExistingElementDataEx->elementDataFilter = OTS_ELEMENTDATA_NONE;
		}

		// reset timestamp
		pExistingElementDataEx->timeStamp = now;

		if (((elementDataFilter & OTS_ELEMENTDATA_NAME) == OTS_ELEMENTDATA_NAME) && (pElementData->m_name))
		{
			if (pExistingElementDataEx->elementData->m_name)
			{
				OTFreeMemory(pExistingElementDataEx->elementData->m_name);
			}

			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_NAME;
			pExistingElementDataEx->elementData->m_name = pElementData->m_name;
			pElementData->m_name = NULL; // mark null to prevent deletion in caller (memory ownership belongs now to this object
		}

		if (((elementDataFilter & OTS_ELEMENTDATA_ITEMID) == OTS_ELEMENTDATA_ITEMID) && (pElementData->m_itemID))
		{
			if (pExistingElementDataEx->elementData->m_itemID)
			{
				OTFreeMemory(pExistingElementDataEx->elementData->m_itemID);
			}

			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_ITEMID;
			pExistingElementDataEx->elementData->m_itemID = pElementData->m_itemID;
			pElementData->m_itemID = NULL; // mark null to prevent deletion in caller (memory ownership belongs now to this object)
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_ACCESSRIGHTS) == OTS_ELEMENTDATA_ACCESSRIGHTS)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_ACCESSRIGHTS;
			pExistingElementDataEx->elementData->m_accessRights = pElementData->m_accessRights;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_IOMODE) == OTS_ELEMENTDATA_IOMODE)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_IOMODE;
			pExistingElementDataEx->elementData->m_ioMode = pElementData->m_ioMode;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_HASCHILDREN) == OTS_ELEMENTDATA_HASCHILDREN)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_HASCHILDREN;
			pExistingElementDataEx->elementData->m_hasChildren = pElementData->m_hasChildren;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_ISBROWSABLE) == OTS_ELEMENTDATA_ISBROWSABLE)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_ISBROWSABLE;
			pExistingElementDataEx->elementData->m_isBrowsable = pElementData->m_isBrowsable;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_USERDATA) == OTS_ELEMENTDATA_USERDATA)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_USERDATA;
			pExistingElementDataEx->elementData->m_userData = pElementData->m_userData;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_DATATYPE) == OTS_ELEMENTDATA_DATATYPE)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_DATATYPE;
			pExistingElementDataEx->elementData->m_datatype = pElementData->m_datatype;
		}

		if ((elementDataFilter & OTS_ELEMENTDATA_ELEMENTTYPE) == OTS_ELEMENTDATA_ELEMENTTYPE)
		{
			pExistingElementDataEx->elementDataFilter |= OTS_ELEMENTDATA_ELEMENTTYPE;
			pExistingElementDataEx->elementData->m_elementType = pElementData->m_elementType;
		}
	}
	else
	{
		// if not found, create object from scratch
		pExistingElementDataEx = (OTSAddressSpaceElementDataEx*)OTAllocateMemory(sizeof(OTSAddressSpaceElementDataEx));
		pExistingElementDataEx->elementData = (OTSAddressSpaceElementData*)OTAllocateMemory(sizeof(OTSAddressSpaceElementData));
		memcpy(pExistingElementDataEx->elementData, pElementData, sizeof(OTSAddressSpaceElementData));

		pElementData->m_name = NULL; // mark null to prevent deletion in caller (memory ownership belongs now to this object)
		pElementData->m_itemID = NULL; // mark null to prevent deletion in caller (memory ownership belongs now to this object)

		pExistingElementDataEx->timeStamp = now;
		pExistingElementDataEx->elementDataFilter = elementDataFilter;
		m_stringBasedObjectDataEx.add(pExistingElementDataEx, pName); // socmnlist "add" allocates memory for object and performs a shallow copy
		OTFreeMemory(pExistingElementDataEx); // therefore we will perform a shallow delete (deep delete at removal/shutdown)
	}

	m_stringBasedObjectDataEx.doRehash();
	m_stringBasedObjectDataEx.unlock();
	return TRUE;
}

BOOL SOSrvEntry::getStringBasedObjectDataEx(IN const SOCmnString &pName, OUT OTSAddressSpaceElementData **pElementData, IN unsigned short &elementDataFilter)
{
	elementDataFilter = OTS_ELEMENTDATA_NONE;
	*pElementData = NULL;

	if (m_objectDataExpiryTimeout == OTS_ELEMENTDATA_DISABLED_QW)
	{
		return FALSE;
	}

	if (!m_stringBasedObjectDataEx.isInit())
	{
		return FALSE;
	}

	m_stringBasedObjectDataEx.lock();

	OTSAddressSpaceElementDataEx *pExistingElementDataEx = NULL;
	unsigned long long now;
	GetSystemTimeAsFileTime((FILETIME*)&now);

	if (m_stringBasedObjectDataEx.findKey(pName, &pExistingElementDataEx))
	{
		if ((m_objectDataExpiryTimeout == OTS_ELEMENTDATA_INFINITE_QW) || 
			(now < pExistingElementDataEx->timeStamp + m_objectDataExpiryTimeout))
		{
			// not expired yet
			if (((elementDataFilter & pExistingElementDataEx->elementDataFilter) == elementDataFilter) ||
				(elementDataFilter == OTS_ELEMENTDATA_ANY))
			{
				// all required data? (or.. any of the required data?)
				elementDataFilter = pExistingElementDataEx->elementDataFilter;
				*pElementData = pExistingElementDataEx->elementData;
				m_stringBasedObjectDataEx.unlock();
				return TRUE;
			}
		}
		else
		{
			// expired
			cleanStringBasedObjectDataEx(pName);
		}
	}

	*pElementData = NULL;
	elementDataFilter = 0;
	m_stringBasedObjectDataEx.unlock();
	return FALSE;
}

BOOL SOSrvEntry::cleanStringBasedObjectDataEx(IN const SOCmnString &additionalKeyToDelete)
{
	if (m_objectDataExpiryTimeout == OTS_ELEMENTDATA_DISABLED_QW)
	{
		return FALSE;
	}

	if (!m_stringBasedObjectDataEx.isInit())
	{
		return FALSE;
	}
	
	m_stringBasedObjectDataEx.lock();
	BOOL ret = FALSE;
	unsigned long long now;
	::GetSystemTimeAsFileTime((FILETIME*)&now);
	OTSAddressSpaceElementDataEx* pElemData;

	if (!additionalKeyToDelete.isEmpty())
	{
		if (m_stringBasedObjectDataEx.findKey(additionalKeyToDelete, &pElemData))
		{
			if (pElemData->elementData->m_name)
			{
				OTFreeMemory(pElemData->elementData->m_name);
			}

			if (pElemData->elementData->m_itemID)
			{
				OTFreeMemory(pElemData->elementData->m_itemID);
			}

			OTFreeMemory(pElemData->elementData);

			m_stringBasedObjectDataEx.removeKey(additionalKeyToDelete);
			ret = TRUE;
		}
	}

	if (m_objectDataExpiryTimeout == OTS_ELEMENTDATA_INFINITE_QW)
	{
		m_stringBasedObjectDataEx.unlock();
		return ret; // never expire, only remove items on demand.
	}

	if (now > m_objectDataLastCleanup + m_objectDataRecheckMinWait)
	{
		// only remove the old data every now and then (configurable)
		m_objectDataLastCleanup = now;
		SOCmnStringList keysToDelete;
		keysToDelete.create();

		SOCmnString key;
		SOCmnListPosition pos = m_stringBasedObjectDataEx.getStartPosition();
		DWORD totalSize = m_stringBasedObjectDataEx.GetCount();
		DWORD currentSize = 0;

		while (pos)
		{
			pElemData = m_stringBasedObjectDataEx.getNext(pos, &key);

			if (now < pElemData->timeStamp + m_objectDataExpiryTimeout)
			{
				if (pElemData->elementData->m_itemID)
				{
					OTFreeMemory(pElemData->elementData->m_itemID);
				}

				if (pElemData->elementData->m_name)
				{
					OTFreeMemory(pElemData->elementData->m_name);
				}

				OTFreeMemory(pElemData->elementData);

				keysToDelete.add(key);
				++currentSize;

				if ((currentSize > m_objectDataMinCleanupElementsCount) &&						// if there are at least minimum count elements
					(currentSize * 100 > m_objectDataMaxCleanedElementsPercent * totalSize))	// and maximum percent is exceeded
				{
					break; // don`t know whether it`s idle thread or call, therefore we don`t waste much time here
				}
			}
		}

		pos = keysToDelete.getStartPosition();

		while (pos)
		{
			key = keysToDelete.GetNext(pos);
			m_stringBasedObjectDataEx.removeKey(key);
			ret = TRUE;
		}
	}

	m_stringBasedObjectDataEx.unlock();
	return ret;
}

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// initialize
//
// - initialize the toolkit
//
// returns:
//		TRUE  - initialization done
//		FALSE - error during initialization
//
BOOL SOSrvEntry::initialize(
	IN enum serverTypeCOM type,     // COM server type
	IN REFCLSID clsid,              // server class id
	IN HINSTANCE hInst,             // instance handle of the application
	IN DWORD coInit)                // COM init mode
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("initialize; clsid:%8.8X-%4.4X-%4.4X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X"), clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7]));
	HRESULT res = S_OK;
	// set data
	m_type = type;
	m_clsid = clsid;
	m_appInstanceHandle = hInst;
	m_coInit = coInit;
	m_initialized |= SOSRVENTRY_INIT_COM;

	// initialize COM (only OutProc servers)
	if (type != InProc)
	{
		if (SUCCEEDED(res = ::CoInitializeEx(NULL, coInit)))
		{
			m_initialized |= SOSRVENTRY_INIT_HAVETO_UNINIT_COM;
		}
		else
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("Can't initialize COM: coInit:0x%X [0x%X]"), coInit, res));
		}
	}

	if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
	{
		init(hInst);    // init ATL
	}

	m_startTime.now();  // set start time
	return SUCCEEDED(res);
} // initialize

//-----------------------------------------------------------------------------
// initializeSecurity
//
// - initialize security
//
// returns:
//		TRUE  - initialization done
//		FALSE - already initialized
//
BOOL SOSrvEntry::initializeSecurity(
	IN DWORD authLevel,
	IN DWORD impLevel,
	IN DWORD cbAuthLevel,
	IN DWORD cbImpLevel,
	IN DWORD capabilities)
{
	static bool s_dcomInitialized = false;
	m_serverAuthLevel = authLevel;
	m_callbackAuthLevel = cbAuthLevel;
	m_serverImpLevel = impLevel;
	m_callbackImpLevel = cbImpLevel;
	m_initialized |= SOSRVENTRY_INIT_SECURITY;
	
	if (!s_dcomInitialized)
	{
		s_dcomInitialized = true;
		SOCmnString buffer;

		if ((authLevel != RPC_C_AUTHN_LEVEL_DEFAULT) && (impLevel != RPC_C_IMP_LEVEL_DEFAULT))
		{
			HRESULT res = CoInitializeSecurity(0, -1, 0, 0, authLevel,
				impLevel, 0, capabilities, 0);
			_TRACE(FAILED(res) ? SOCMNTRACE_L_ERR : SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, (_T("DCOM server CoInitializeSecurity (a:%d, i:%d) result: 0x%X"), authLevel, impLevel, res));
			buffer.format(_T("DCOM server initialized: a=%d, i=%d, res=0x%08X."), authLevel, impLevel, res);
			getSOCmnTrace()->addDcomInitStatus((LPCTSTR)buffer);
			return SUCCEEDED(res);
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_ALL, this, (_T("DCOM server CoInitializeSecurity skipped")));
			buffer.format(_T("DCOM server initialization skipped: a=%d, i=%d."), authLevel, impLevel);
			getSOCmnTrace()->addDcomInitStatus((LPCTSTR)buffer);
		}
	}

	return TRUE;
} // initializeSecurity


#endif

#ifdef SOFEATURE_SOAP
BOOL SOSrvEntry::initializeHTTP(IN WORD port)
{
	m_initialized |= SOSRVENTRY_INIT_HTTP;
	m_startTime.now();    // set start time
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("initializeHTTP; port:%u"), port));

	if (m_srvHTTP->create(port))
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ENTRY, (_T("initializeHTTP; port:%u has failed"), port));
		return FALSE;
	}   //  end if

	return TRUE;
} // initializeHTTP
#endif

#ifdef SOFEATURE_TUNNEL
BOOL SOSrvEntry::initializeTP(IN WORD port, IN BOOL bEnableClientAuthentication, IN SOCmnList<SOCmnTPCredentials> *pPermissibleCredentialsList)
{
	m_initialized |= SOSRVENTRY_INIT_TP;
	m_startTime.now();    // set start time
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("initializeTP; port:%u"), port));
	m_srvTP->enableClientAuthentication(bEnableClientAuthentication);
	m_srvTP->setPermissibleCredentialsList(pPermissibleCredentialsList);
	return (0 == m_srvTP->create(port));
} // initializeHTTP
#endif

//-----------------------------------------------------------------------------
// terminate
//
// - reset terminate lock
// - terminate toolkit
//
// returns:
//		TRUE
//
BOOL SOSrvEntry::terminate(void)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("terminate")));
	m_terminateLock = FALSE;
	stop();     // stop toolkit main thread
#ifdef SOFEATURE_DCOM

	if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
	{
		term();    // termiante ATL
	}

	// uninitalize COM
	if (m_initialized & SOSRVENTRY_INIT_HAVETO_UNINIT_COM)
	{
		::CoUninitialize();
	}

#endif
	return TRUE;
} // terminate


void SOSrvEntry::setCheckParameter(
	IN DWORD clientInterval,
	OPTIONAL IN BOOL checkAllIFs,
	OPTIONAL IN DWORD clientInactiveTime)
{
#ifdef SOFEATURE_DCOM
	m_clientCheckInterval = clientInterval;
	m_clientCheckAllIFs = (BYTE)checkAllIFs;
#endif
#ifdef SOFEATURE_SOAP

	if (m_srvHTTP.isNotNull())
	{
		SOCmnPointer<SOSrvHTTPHandler> httpH = m_srvHTTP->getHTTPHandler();

		if (httpH.isNotNull())
		{
			httpH->setCheckParameter(clientInterval, clientInactiveTime);
		}
	}

#endif
}


//-----------------------------------------------------------------------------
// getObjectRoot
//
// - get OPC tree root object
//
// returns:
//		OPC tree root
//
SOSrvObjectRoot* SOSrvEntry::getObjectRoot(void)
{
	if (m_objectRoot)
	{
		m_objectRoot->addRef();
	}

	return m_objectRoot;
} // getObjectRoot


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setShutdownEvent
//
// - set shutdown event
//
void SOSrvEntry::setShutdownEvent(
	IN SOCmnEventPointer event) // shutdown event
{
	m_shutdownEvent = event;
} // setShutdownEvent


//-----------------------------------------------------------------------------
// mainThreadProc
//
// - main thread of the toolkit
//
// returns:
//		thread exit code
//
DWORD _stdcall SOSrvEntry::mainThreadProc(void* context)
{
	SOSrvEntry* eng = (SOSrvEntry*)context;
	return eng->mainThread();
} // mainThreadProc


//-----------------------------------------------------------------------------
// mainThread
//
// - main thread of the toolkit
//
// returns:
//		thread exit code
//
DWORD SOSrvEntry::mainThread(void)
{
	DWORD ret;
	BOOL end = FALSE;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("started toolkit main thread")));
	//-- initialize toolkit thread --
	HRESULT res = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(res))
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ENTRY, (_T("Can't initialize COM: 0x%X"), res));
		return res;
	}

	if (m_type == OutProc)
	{
		// register class factory for EXEs
		if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
		{
			registerClassObjects();
		}
	}

	//-- wait till toolkit termiantion --
	while (!end)
	{
		ret = m_mainThread.waitForEvents(m_clientCheckInterval);

		if (ret == m_mainThread.getStopEventId())
		{
			// end event
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("received end event")));
			end = TRUE;
		}
		else if (ret == m_shutdownEventId)
		{
			// shutdown event
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("received shutdown event")));
			sendShutdownRequests();
		}
		else
		{
			// timeout
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("check client connections")));
			checkClientConnections(m_clientCheckAllIFs);

			// free unused events
			SOCmnEventPool* evPool = getEventPool();

			if (evPool)
			{
				evPool->freeAllEvents();
			}

			// free expired cache "in background"
			cleanStringBasedObjectDataEx();
		}
	}

	//-- terminated engine --
	if (m_type == OutProc)
	{
		// revoke class factory for EXEs
		if (m_entryFlags & SOSRVENTRY_FLAG_ATL_MODULE)
		{
			revokeClassObjects();
		}
	}

	disconnectAllClients();
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_ENTRY, (_T("COM uninitialize")));
	::CoUninitialize();
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("terminated toolkit main thread")));
	return S_OK;
} // mainThread

#endif


//-----------------------------------------------------------------------------
// start
//
// - start the main thread of the toolkit
//
// returns:
//		TRUE  - thread started
//		FALSE - error creating the thread
//
BOOL SOSrvEntry::start(void)
{
	BOOL ret = TRUE;

	if ((m_entryFlags & SOSRVENTRY_FLAG_MAIN_THREAD))
	{
		if (m_pThreadPool == NULL)
		{
			m_pThreadPool = new SOCmnThreadPool(m_defWorkerThreads);
			m_pThreadPool->start();
		}

#ifdef SOFEATURE_DCOM

		if ((m_initialized & SOSRVENTRY_INIT_COM) == SOSRVENTRY_INIT_COM)
			if (!m_mainThread.isRunning())
			{
				// create events
				m_shutdownEventId = m_mainThread.createEvent();
				setShutdownEvent(SOCmnEventPointer(m_mainThread.getEvents(), m_shutdownEventId));
				// create thread
				ret = m_mainThread.start(mainThreadProc, (void*)this);

				if (ret)
				{
					m_mainThread.setPriority(THREAD_PRIORITY_NORMAL);
				}
			}

		if (m_type == OutProc)
		{
#endif
#ifdef SOFEATURE_SOAP

			if ((m_initialized & SOSRVENTRY_INIT_HTTP) == SOSRVENTRY_INIT_HTTP)
				if (ret)
				{
					ret = m_srvHTTP->start();
				}

#endif
#ifdef SOFEATURE_TUNNEL

			if ((m_initialized & SOSRVENTRY_INIT_TP) == SOSRVENTRY_INIT_TP)
				if (ret)
				{
					ret = m_srvTP->start();
				}

#endif
#ifdef SOFEATURE_DCOM
		}

#endif

		if (!ret)
		{
			stop();
		}
	}
	else
	{
		ret = FALSE;
	}

	return ret;
} // start


//-----------------------------------------------------------------------------
// stop
//
// - stop the main thread of the toolkit
//
// returns:
//		TRUE  - thread stopped
//		FALSE - can't stop thread
//
BOOL SOSrvEntry::stop(void)
{
	BOOL ret = TRUE;

	if ((m_entryFlags & SOSRVENTRY_FLAG_MAIN_THREAD))
	{
#ifdef SOFEATURE_TUNNEL

		if ((m_initialized & SOSRVENTRY_INIT_TP) == SOSRVENTRY_INIT_TP)
		{
			ret = m_srvTP->stop();
		}

#endif
#ifdef SOFEATURE_SOAP

		if ((m_initialized & SOSRVENTRY_INIT_HTTP) == SOSRVENTRY_INIT_HTTP)
		{
			ret = m_srvHTTP->stop();
		}

#endif
#ifdef SOFEATURE_DCOM

		if ((m_initialized & SOSRVENTRY_INIT_COM) == SOSRVENTRY_INIT_COM)
		{
			ret |= m_mainThread.stop(10000);
		}

#endif
		delete m_pThreadPool;
		m_pThreadPool = NULL;
	}

	return ret;
} // stop


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// registerServer
//
// - register the server
//
// returns:
//		S_OK  - server registered
//		other HRESULTs
//
HRESULT SOSrvEntry::registerServer(void)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("register server")));
	HRESULT res;
	// register COM Server object
	res = regServer();

	if (SUCCEEDED(res))
	{
		res = registerCategories();
	}

	return res;
} // registerServer


//-----------------------------------------------------------------------------
// registerCategories
//
// - register the categories of server
//
// returns:
//		S_OK  - server registered
//		other HRESULTs
//
HRESULT SOSrvEntry::registerCategories(void)
{
	if (memcmp(&CLSID_NO, &m_clsid, sizeof(CLSID)) == 0)
	{
		return S_OK;
	}

	// register COM Categories
	HRESULT res;
	ICatRegister* cr;

	if (SUCCEEDED(res = ::CoCreateInstance(CLSID_StdComponentCategoriesMgr,
										   NULL, CLSCTX_ALL, IID_ICatRegister, (void**)&cr)))
	{
		CATEGORYINFO catInfo[8];
		CATID catIDs[4];
		int i = 0;

		if (m_regCategories & SOSRVENTRY_CATEGORY_DA10)
		{
			catInfo[(i * 2)].catid = CATID_OPCDAServer10;
			catInfo[(i * 2) + 1].catid = CATID_OPCDAServer10;
			catInfo[(i * 2)].lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) , SORT_DEFAULT);
			catInfo[(i * 2) + 1].lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),  SORT_DEFAULT);
			wcscpy(catInfo[(i * 2)].szDescription, L"OPC Data Access Servers Version 1.0");
			wcscpy(catInfo[(i * 2) + 1].szDescription, L"OPC Daten Server Version 1.0");
			catIDs[i] = CATID_OPCDAServer10;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_DA20)
		{
			catInfo[(i * 2)].catid = CATID_OPCDAServer20;
			catInfo[(i * 2) + 1].catid = CATID_OPCDAServer20;
			catInfo[(i * 2)].lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) , SORT_DEFAULT);
			catInfo[(i * 2) + 1].lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),  SORT_DEFAULT);
			wcscpy(catInfo[(i * 2)].szDescription, L"OPC Data Access Servers Version 2.0");
			wcscpy(catInfo[(i * 2) + 1].szDescription, L"OPC Daten Server Version 2.0");
			catIDs[i] = CATID_OPCDAServer20;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_DA30)
		{
			catInfo[(i * 2)].catid = CATID_OPCDAServer30;
			catInfo[(i * 2) + 1].catid = CATID_OPCDAServer30;
			catInfo[(i * 2)].lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) , SORT_DEFAULT);
			catInfo[(i * 2) + 1].lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),  SORT_DEFAULT);
			wcscpy(catInfo[(i * 2)].szDescription, L"OPC Data Access Servers Version 3.0");
			wcscpy(catInfo[(i * 2) + 1].szDescription, L"OPC Daten Server Version 3.0");
			catIDs[i] = CATID_OPCDAServer30;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_AE10)
		{
			catInfo[(i * 2)].catid = IID_OPCEventServerCATID;
			catInfo[(i * 2) + 1].catid = IID_OPCEventServerCATID;
			catInfo[(i * 2)].lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) , SORT_DEFAULT);
			catInfo[(i * 2) + 1].lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),  SORT_DEFAULT);
			wcscpy(catInfo[(i * 2)].szDescription, L"OPC Alarms and Event Servers Version 1.0");
			wcscpy(catInfo[(i * 2) + 1].szDescription, L"OPC Alarm und Event Server Version 1.0");
			catIDs[i] = IID_OPCEventServerCATID;
			i++;
		}

		// register category
		res = cr->RegisterCategories(i * 2, catInfo);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("register component categories [0x%X]"), res));
		// register server in categories
		res = cr->RegisterClassImplCategories(m_clsid, i, catIDs);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("register component implementations [0x%X]"), res));
		cr->Release();
	}
	else
	{
		// No StdComponentCategoriesMgr like in CE 3.0
		// Add the implemented categories to the CLSID key
		// The custom OPCENUM under CE will look for this
		CRegKey key, keyCLSID, keyImpCat, keyCat;
		DWORD ret;
		ret = keyCLSID.Open(HKEY_CLASSES_ROOT, _T("CLSID"));

		if (ret == ERROR_SUCCESS)
		{
			USES_CONVERSION;
			LPOLESTR wszClsid;
			LPTSTR szClsid;
			StringFromCLSID(m_clsid, &wszClsid);
			szClsid = W2T(wszClsid);

			if ((ret = key.Open(keyCLSID, szClsid)) == ERROR_SUCCESS)
			{
				if (ERROR_SUCCESS == (ret = keyImpCat.Create(key, _T("Implemented Categories"))))
				{
					LPOLESTR wszCatid;

					if (m_regCategories & SOSRVENTRY_CATEGORY_DA10)
					{
						StringFromCLSID(CATID_OPCDAServer10, &wszCatid);

						if (ERROR_SUCCESS == (ret = keyCat.Create(keyImpCat, W2T(wszCatid))))
						{
							keyCat.Close();
						}

						CoTaskMemFree(wszCatid);
					}

					if (m_regCategories & SOSRVENTRY_CATEGORY_DA20)
					{
						StringFromCLSID(CATID_OPCDAServer20, &wszCatid);

						if (ERROR_SUCCESS == (ret = keyCat.Create(keyImpCat, W2T(wszCatid))))
						{
							keyCat.Close();
						}

						CoTaskMemFree(wszCatid);
					}

					if (m_regCategories & SOSRVENTRY_CATEGORY_DA30)
					{
						StringFromCLSID(CATID_OPCDAServer30, &wszCatid);

						if (ERROR_SUCCESS == (ret = keyCat.Create(keyImpCat, W2T(wszCatid))))
						{
							keyCat.Close();
						}

						CoTaskMemFree(wszCatid);
					}

					if (m_regCategories & SOSRVENTRY_CATEGORY_AE10)
					{
						StringFromCLSID(IID_OPCEventServerCATID, &wszCatid);

						if (ERROR_SUCCESS == (ret = keyCat.Create(keyImpCat, W2T(wszCatid))))
						{
							keyCat.Close();
						}

						CoTaskMemFree(wszCatid);
					}

					keyImpCat.Close();
				}

				key.Close();
			}

			CoTaskMemFree(wszClsid);
			keyCLSID.Close();
		}

		if (ret != ERROR_SUCCESS)
		{
			res = HRESULT_FROM_WIN32(ret);
		}
	}

	return res;
} // registerCategories


//-----------------------------------------------------------------------------
// unregisterServer
//
// - unregister the server
//
// returns:
//		S_OK  - server registered
//		other HRESULTs
//
HRESULT SOSrvEntry::unregisterServer(void)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("unregister server")));
	HRESULT ret, res;
	// unregister COM Categories
	ret = unregisterCategories();
	// unregister COM Server object
	res = unregServer();

	if (FAILED(res))
	{
		ret = res;
	}

	return ret;
} // unregisterServer


//-----------------------------------------------------------------------------
// unregisterCategories
//
// - unregister the categories of the server
//
// returns:
//		S_OK  - server registered
//		other HRESULTs
//
HRESULT SOSrvEntry::unregisterCategories(void)
{
	if (memcmp(&CLSID_NO, &m_clsid, sizeof(CLSID)) == 0)
	{
		return S_OK;
	}

	HRESULT res;
	ICatRegister* cr;
	ICatInformation* ci = NULL ;
	IEnumCLSID* enumCLSID = NULL ;
	CATID catIDs[4];
	CLSID clsid ;
	int i = 0, j;

	if (SUCCEEDED(res = ::CoCreateInstance(CLSID_StdComponentCategoriesMgr,
										   NULL, CLSCTX_ALL, IID_ICatRegister, (void**)&cr)))
	{
		// unregister server in categories
		if (m_regCategories & SOSRVENTRY_CATEGORY_DA10)
		{
			catIDs[i] = CATID_OPCDAServer10;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_DA20)
		{
			catIDs[i] = CATID_OPCDAServer20;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_DA30)
		{
			catIDs[i] = CATID_OPCDAServer30;
			i++;
		}

		if (m_regCategories & SOSRVENTRY_CATEGORY_AE10)
		{
			catIDs[i] = IID_OPCEventServerCATID;
			i++;
		}

		res = cr->UnRegisterClassImplCategories(m_clsid, i, catIDs);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("unregister component implementations [0x%X]"), res));

		// check if this server was the last in the category
		if (SUCCEEDED(res = cr->QueryInterface(IID_ICatInformation, (void**)&ci)))
		{
			for (j = 0; j < i; j++)
			{
				if (SUCCEEDED(res = ci->EnumClassesOfCategories(1, &catIDs[j], 0, NULL, &enumCLSID)))
				{
					res = enumCLSID->Next(1, &clsid, NULL) ;

					// if result is S_FALSE, there no components are in this category, so remove it.
					if (res == S_FALSE)
					{
						// unregister the component category
						res = cr->UnRegisterCategories(1, &catIDs[j]) ;
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_ENTRY, (_T("unregister component category [0x%X]"), res));
					}

					enumCLSID->Release();
				}
			}

			ci->Release();
		}

		cr->Release();
	}

	// remove "Implemented Categories" Key form the CLSID
	CRegKey key, keyCLSID;
	DWORD ret;
	ret = keyCLSID.Open(HKEY_CLASSES_ROOT, _T("CLSID"));

	if (ret == ERROR_SUCCESS)
	{
		USES_CONVERSION;
		LPOLESTR wszClsid;
		LPTSTR szClsid;
		StringFromCLSID(m_clsid, &wszClsid);
		szClsid = W2T(wszClsid);
		ret = key.Open(keyCLSID, szClsid);

		if (ret == ERROR_SUCCESS)
		{
			key.RecurseDeleteKey(_T("Implemented Categories"));
			key.Close();
		}
		else
		{
			res = HRESULT_FROM_WIN32(ret);
		}

		CoTaskMemFree(wszClsid);
		keyCLSID.Close();
	}
	else
	{
		res = HRESULT_FROM_WIN32(ret);
	}

	return res;
} // unregisterCategories


//-----------------------------------------------------------------------------
// addRegisterResourceReplacement
//
// - add replacment string pair for register the server via rgs
//
// returns:
//		TRUE  - added replacement
//		FALSE - can't add replacement
//
BOOL SOSrvEntry::addRegisterResourceReplacement(
	IN LPCTSTR key,
	IN LPCTSTR data)
{
	if (!m_regRGSRepList)
	{
		// create list
		m_regRGSRepList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING);
	}

	SOCmnString dat(data);
	return m_regRGSRepList.add(dat, key);
} // addRegisterResourceReplacement


//-----------------------------------------------------------------------------
// updateRegistry
//
// - update the registry with the register information of the server
//
// returns:
//		S_OK  - server registered
//		other HRESULTs
//
HRESULT SOSrvEntry::updateRegistry(
	IN BOOL doRegister) // register or unregister
{
	if (m_regRGSResID)
	{
		// have a registar script
		// let the ATL do the work
		_ATL_REGMAP_ENTRY* rmEntry = NULL;
		BOOL ret;
		DWORD cnt = 0;

		if (m_regRGSRepList.isInit())
		{
			cnt = m_regRGSRepList.getCount();

			if (cnt > 0)
			{
				USES_CONVERSION;
				SOCmnListPosition pos;
				SOCmnString data;
				SOCmnString key;
				DWORD i = 0;
				rmEntry = (_ATL_REGMAP_ENTRY*)malloc((cnt + 1) * sizeof(_ATL_REGMAP_ENTRY));

				if (!rmEntry)
				{
					return FALSE;
				}

				pos = m_regRGSRepList.getStartPosition();

				while (pos)
				{
					data = m_regRGSRepList.getNext(pos, &key);
					rmEntry[i].szData = _wcsdup(T2CW(data));
					rmEntry[i].szKey = _wcsdup(T2CW(key));
					i++;
				}

				rmEntry[cnt].szData = NULL;
				rmEntry[cnt].szKey = NULL;
			}
		}

		ret = updateRegistryWithRGS(doRegister, rmEntry);

		if ((rmEntry) && (cnt > 0))
		{
			DWORD i;

			for (i = 0; i < cnt; i++)
			{
				free((void*)rmEntry[i].szData);
				free((void*)rmEntry[i].szKey);
			}

			free(rmEntry);
		}

		return ret;
	}
	else
	{
		// only the 3 register strings
		// do the standard registry settings
		USES_CONVERSION;
		HRESULT res = S_OK;
		LPOLESTR wszClsid;
		LPTSTR clsid;
		LONG ret;

		if (memcmp(&CLSID_NO, &m_clsid, sizeof(CLSID)) == 0)
		{
			return S_OK;
		}

		_ASSERTION((!m_regDescription.isEmpty()) && (!m_regProgID.isEmpty()) &&
				   (!m_regVerIndProgID.isEmpty()), "Not called setRegisterResource or setRegisterInfo before registerServer or unregisterServer");
		StringFromCLSID(m_clsid, &wszClsid);
		clsid = W2T(wszClsid);

		if (doRegister)
		{
			// register
			CRegKey keyProgID;
			CRegKey keyVerIndProgID;
			CRegKey keyCLSID;
			CRegKey keyAppID;
			CRegKey keyClass;
			TCHAR module[_MAX_PATH + 1];
			TCHAR moduleShort[_MAX_PATH + 1];
			TCHAR moduleComplete[_MAX_PATH + 3];
			
			GetModuleFileName(m_appInstanceHandle, module, _MAX_PATH);
			_stprintf(moduleComplete, _T("\"%s\""), module);
			
#ifndef SOOS_WINDOWS_CE
			GetShortPathName(module, moduleShort, _MAX_PATH);
#else
			_tcscpy(moduleShort, module);
#endif
			// progId
			ret = keyProgID.Create(HKEY_CLASSES_ROOT, m_regProgID);

			if (ret == ERROR_SUCCESS)
			{
				keyProgID.SetStringValue(NULL, m_regDescription);
				keyProgID.SetKeyValue(_T("CLSID"), clsid);
			}
			else if (res == S_OK)
			{
				res = HRESULT_FROM_WIN32(ret);
			}

			// verIndProgId
			ret = keyVerIndProgID.Create(HKEY_CLASSES_ROOT, m_regVerIndProgID);

			if (ret == ERROR_SUCCESS)
			{
				keyVerIndProgID.SetStringValue(NULL, m_regDescription);
				keyVerIndProgID.SetKeyValue(_T("CLSID"), clsid);

				if (m_regCategories & SOSRVENTRY_CATEGORY_DA10)
				{
					CRegKey keyOPC;

					if (ERROR_SUCCESS == keyOPC.Create(keyVerIndProgID, _T("OPC")))
					{
						keyOPC.SetKeyValue(_T("Vendor"), m_vendorInfo);
					}
				}
			}
			else if (res == S_OK)
			{
				res = HRESULT_FROM_WIN32(ret);
			}

			// class id
			ret = keyCLSID.Open(HKEY_CLASSES_ROOT, _T("CLSID"));

			if (ret == ERROR_SUCCESS)
			{
				ret = keyClass.Create(keyCLSID, clsid);

				if (ret == ERROR_SUCCESS)
				{
					keyClass.SetStringValue(NULL, m_regDescription);
					keyClass.SetStringValue(_T("AppID"), clsid);
					keyClass.SetKeyValue(_T("ProgID"), m_regProgID);
					keyClass.SetKeyValue(_T("VersionIndependentProgID"), m_regVerIndProgID);

					if (m_type == OutProc)
					{
						//keyClass.SetKeyValue(_T("LocalServer32"), moduleShort);
						keyClass.SetKeyValue(_T("LocalServer32"), moduleComplete);
						keyClass.DeleteSubKey(_T("InprocServer32"));
					}
					else
					{
						keyClass.SetKeyValue(_T("InprocServer32"), module);
						keyClass.SetKeyValue(_T("InprocServer32"), _T("Both"), _T("ThreadingModel"));
						keyClass.DeleteSubKey(_T("LocalServer32"));
					}
				}
			}

			if ((ret != ERROR_SUCCESS) && (res == S_OK))
			{
				res = HRESULT_FROM_WIN32(ret);
			}

			// application id
			ret = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"));

			if (ret == ERROR_SUCCESS)
			{
				ret = keyClass.Create(keyAppID, clsid);

				if (ret == ERROR_SUCCESS)
				{
					keyClass.SetStringValue(NULL, m_regDescription);

					if ((m_type == OutProc) && (!m_regNTServiceName.isEmpty()))
					{
						keyClass.SetStringValue(_T("LocalService"), m_regNTServiceName);
					}
					else
					{
						keyClass.DeleteValue(_T("LocalService"));
					}
				}
			}

			if ((ret != ERROR_SUCCESS) && (res == S_OK))
			{
				res = HRESULT_FROM_WIN32(ret);
			}
		}
		else
		{
			// unregister
			CRegKey key;
			key.Attach(HKEY_CLASSES_ROOT);

			if (!m_regProgID.isEmpty())
			{
				key.RecurseDeleteKey(m_regProgID);
			}

			if (!m_regVerIndProgID.isEmpty())
			{
				key.RecurseDeleteKey(m_regVerIndProgID);
			}

			key.Detach();
			ret = key.Open(HKEY_CLASSES_ROOT, _T("CLSID"));

			if (ret == ERROR_SUCCESS)
			{
				if ((clsid) && (_tcslen(clsid) != 0))
				{
					key.RecurseDeleteKey(clsid);
				}
				else
				{
					res = E_FAIL;
				}
			}
			else
			{
				res = HRESULT_FROM_WIN32(ret);
			}

			key.Close();
			ret = key.Open(HKEY_CLASSES_ROOT, _T("AppID"));

			if (ret == ERROR_SUCCESS)
			{
				if ((clsid) && (_tcslen(clsid) != 0))
				{
					key.RecurseDeleteKey(clsid);
				}
				else
				{
					res = E_FAIL;
				}
			}
			else
			{
				res = HRESULT_FROM_WIN32(ret);
			}

			key.Close();
		}

		CoTaskMemFree(wszClsid);
		return res;
	}
} // updateRegistry

#endif // SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// setEventHandler
//
// - set event handler object
//
void SOSrvEntry::setEventHandler(
	IN SOSrvEventHandler* events)   // event handler object
{
	if (m_evtHandler)
	{
		m_evtHandler->release();
	}

	m_evtHandler = events;

	if (m_evtHandler)
	{
		m_evtHandler->addRef();
	}
} // setEventHandler

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// shutdownAllClients
//
// - add shutdown requests for all clients to the shutdown list
//
void SOSrvEntry::shutdownAllClients(
	IN LPCTSTR reason)  // shutdown reason
{
	if (m_objectRoot)
	{
		SOCmnList<SOCmnElement> list(m_objectRoot->getBranchList());
		SOCmnListPosition posSrv;
		SOSrvServer* server;
		posSrv = list.getStartPosition();

		while (posSrv)
		{
			server = (SOSrvServer*)list.getNext(posSrv);
			SOSrvShutdownData* sreq = new SOSrvShutdownData();
			sreq->server = server;
			sreq->server.addRef();
			sreq->reason = reason;
			sreq->event = INVALID_HANDLE_VALUE;
			m_shutdownList.lock();
			m_shutdownList.add(sreq);
			m_shutdownList.unlock();
		}

		list.autoUnlock();
		m_shutdownEvent.signal();
	}
} // shutdownAllClients


//-----------------------------------------------------------------------------
// addShutdownRequest
//
// - add a shutdown request one client to the shutdown list
//
// returns:
//		TRUE  - request added
//		FALSE - can't add request
//
BOOL SOSrvEntry::addShutdownRequest(
	IN SOSrvServer* server, // server object (-> client)
	IN LPCTSTR reason,      // shutdown reason
	IN HANDLE event)        // shutdown completed event
{
	if (!server)
	{
		return FALSE;
	}

	SOSrvShutdownData* sreq = new SOSrvShutdownData();
	sreq->server = server;
	sreq->server.addRef();
	sreq->reason = reason;
	sreq->event = event;
	m_shutdownList.lock();
	m_shutdownList.add(sreq);
	m_shutdownList.unlock();
	m_shutdownEvent.signal();
	return TRUE;
} // addShutdownRequest


//-----------------------------------------------------------------------------
// sendShutdownRequests
//
// - send all shutdown requests of the shutdown list
//
void SOSrvEntry::sendShutdownRequests(void)
{
	SOSrvShutdownData* sreq;
	m_shutdownList.lock();

	while (!m_shutdownList.isEmpty())
	{
		sreq = m_shutdownList.getHead();
		m_shutdownList.unlock();

		if (sreq->server.isNotNull())
		{
			// send shutdown requests to the clients
			((SOSrvServer*)sreq->server)->sendShutdownRequest(sreq->reason);

			if (sreq->event != INVALID_HANDLE_VALUE)
			{
				::SetEvent(sreq->event);
			}

			sreq->server.release();
		}

		m_shutdownList.lock();
		m_shutdownList.removeHead();
	}

	m_shutdownList.unlock();
} // sendShutdownRequests


//-----------------------------------------------------------------------------
// checkClientConnections
//
// - check all client connections
//
void SOSrvEntry::checkClientConnections(IN BOOL checkAllIFs)
{
	if (m_objectRoot)
	{
		//-- check client connections --
		SOCmnListPosition posSrv;
		SOSrvServer* server = NULL;
		BOOL conOK = FALSE;

 		SOCmnList<SOCmnElement> serverList(m_objectRoot->getBranchList());

		serverList.dontAutoLock();
		serverList.lock();
		serverList.setDirty(FALSE);
		posSrv = serverList.getStartPosition();
		serverList.unlock();

		while(posSrv)
		{

			serverList.lock();
			if (serverList.isDirty())
			{
				// check current position!
				SOCmnListPosition checkPos;
				BOOL found = FALSE;

				checkPos = serverList.getStartPosition();
				while (checkPos)
				{
					if (checkPos == posSrv)
					{
						found = TRUE;
						break;
					}
					serverList.getNext(checkPos);
				}

				if (!found)
				{
					posSrv = serverList.getStartPosition();
				}

				serverList.setDirty(FALSE);
				if (posSrv == NULL)
				{
					serverList.unlock();
					break;
				}
			}

			server = (SOSrvServer *)serverList.getNext(posSrv);
			server->addRef();
			serverList.unlock();

			conOK = server->checkClientConnection(checkAllIFs);

			if (!conOK)
			{
				server->disconnectClient();
			}
			
			server->release();
		}
		
	}
} // checkClientConnections


//-----------------------------------------------------------------------------
// disconnectAllClients
//
// - disconnect all client connections
//
void SOSrvEntry::disconnectAllClients(void)
{
	//-- disconnect all clients --
	SOCmnList<SOCmnElement> serverList(m_objectRoot->getBranchList());
	SOCmnListPosition posSrv;
	SOSrvServer* server;
	posSrv = serverList.getStartPosition();

	while (posSrv)
	{
		server = (SOSrvServer*)serverList.getNext(posSrv);
		server->disconnectClient();
	}

	serverList.autoUnlock();
} // disconnectAllClients


BOOL SOSrvEntry::getCLSID(
	OUT CLSID& clsid,
	IN OPTIONAL BYTE /* serverInstance */)
{
	clsid = m_clsid;
	return TRUE;
}

#endif

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// addWatchObject
//
// - create and add watch object to watch list
//
// returns:
//		pointer to watch object
//
SOSrvWatch* SOSrvEntry::addWatchObject(void)
{
	SOCmnPointer<SOSrvCreator> creator = getSrvCreator();
	SOSrvWatch* watch = creator->createWatch();
	SOCmnList<SOSrvWatch> watchList(getWatchList());

	if (watch)
	{
		watchList.add(watch);
	}

	return watch;
} // addWatchObject


//-----------------------------------------------------------------------------
// watchAttributeChange
//
// - add an attribute change message to the watch objects
//
// returns:
//		TRUE  - added to at least one watch object
//    FALSE - added to none
//
BOOL SOSrvEntry::watchAttributeChange(
	IN SOCmnObject* pObj,                // object
	IN LPCTSTR attrName, IN LPCTSTR attrValue) // change
{
	SOCmnList<SOSrvWatch> watchList(getWatchList());
	SOCmnListPosition pos = watchList.getStartPosition();
	SOSrvWatch* watch;
	BOOL ret = FALSE;

	while (pos)
	{
		watch = watchList.getNext(pos);

		if (watch->changeAttribute(pObj, attrName, attrValue))
		{
			ret = TRUE;
		}
	}

	return ret;
} // watchAttributeChange


//-----------------------------------------------------------------------------
// watchAddObject
//
// - add an object add message to the watch objects
//
// returns:
//		TRUE  - added to at least one watch object
//    FALSE - added to none
//
BOOL SOSrvEntry::watchAddObject(
	IN SOCmnObject* pParent,
	IN SOCmnObject* pObj)
{
	SOCmnList<SOSrvWatch> watchList(getWatchList());
	SOCmnListPosition pos = watchList.getStartPosition();
	SOSrvWatch* watch;
	BOOL ret = FALSE;
	BOOL sret;

	while (pos)
	{
		watch = watchList.getNext(pos);

		if ((sret = watch->addObject(pParent, pObj)) == TRUE)
		{
			ret = TRUE;
		}
	}

	return ret;
} // watchAddObject


//-----------------------------------------------------------------------------
// watchRemoveObject
//
// - add an object remove message to the watch objects
//
// returns:
//		TRUE  - added to at least one watch object
//    FALSE - added to none
//
BOOL SOSrvEntry::watchRemoveObject(
	IN SOCmnObject* pParent,
	IN SOCmnObject* pObj)
{
	SOCmnList<SOSrvWatch> watchList(getWatchList());
	SOCmnListPosition pos = watchList.getStartPosition();
	SOSrvWatch* watch;
	BOOL ret = FALSE;
	BOOL sret;

	while (pos)
	{
		watch = watchList.getNext(pos);

		if ((sret = watch->removeObject(pParent, pObj)) == TRUE)
		{
			ret = TRUE;
		}
	}

	return ret;
} // watchRemoveObject

#endif // SOFEATURE_WATCH




//-----------------------------------------------------------------------------
// SOSrvConverter                                                             |
//-----------------------------------------------------------------------------

SOSrvConverter::SOSrvConverter()
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	m_lcid = MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT);
}


//-----------------------------------------------------------------------------
// setLCID
//
// - set language of converter
//
// returns:
//		TRUE  - language set
//    FALSE - unsupported language
//
BOOL SOSrvConverter::setLCID(
	IN LCID lcid) // new LCID
{
	switch (lcid)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		m_lcid = lcid;
		return TRUE;
		break;

	default:
		return FALSE;
		break;
	}
} // setLCID


//-----------------------------------------------------------------------------
// variant2string
//
// - convert variant to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::variant2string(
	IN SOCmnString& buffer,     // string buffer
	IN LPCVARIANT var,          // variant
	OPTIONAL IN LCID localeID)  // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;
	return ::variant2string(&buffer, var, locID);
} // variant2string


//-----------------------------------------------------------------------------
// vartype2string
//
// - convert vartype to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::vartype2string(
	IN SOCmnString& buffer,    // string buffer
	IN VARTYPE vt,             // vartype
	OPTIONAL IN LCID /* localeID */) // locale id
{
	return ::vartype2string(&buffer, vt);
} // vartype2string


//-----------------------------------------------------------------------------
// quality2string
//
// - convert quality to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::quality2string(
	IN SOCmnString& buffer,    // string buffer
	IN WORD quality,           // quality
	OPTIONAL IN LCID localeID) // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;
	return ::quality2string(&buffer, quality, locID);
} // quality2string


//-----------------------------------------------------------------------------
// accessRights2string
//
// - convert access rights to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::accessRights2string(
	IN SOCmnString& buffer,    // string buffer
	IN DWORD accessRight,      // access rights
	OPTIONAL IN LCID localeID) // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;
	return ::accessRights2string(&buffer, accessRight, locID);
} // accessRights2string

#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
// filetime2string
//
// - convert filetime to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::filetime2string(
	IN SOCmnString& buffer,          // string buffer
	IN LPFILETIME filetime,          // filetime
	OPTIONAL IN LCID /* localeID */) // locale id
{
	SYSTEMTIME t;
	FILETIME f;
	FileTimeToLocalFileTime(filetime, &f);
	FileTimeToSystemTime(&f, &t);
	buffer.Format(_T("%02.2d:%02.2d:%02.2d.%03.3d"), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return (LPCTSTR)buffer;
} // filetime2string

#endif

//-----------------------------------------------------------------------------
// datetime2string
//
// - convert filetime to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::datetime2string(
	IN SOCmnString& buffer,    // string buffer
	IN IN SOCmnDateTime& filetime,    // filetime
	OPTIONAL IN LCID localeID) // locale id
{
	return filetime.toString(buffer, localeID);
} // datetime2string


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// IID2string
//
// - convert interface id into interface description string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::IID2string(
	IN SOCmnString& buffer, // string buffer
	IN REFIID riid)         // interface id
{
	LPWSTR wIIDstring = NULL;
	SOCmnString iidString;
	StringFromIID(riid, &wIIDstring);

	if (wIIDstring)
	{
		USES_CONVERSION;
		iidString = OLE2T(wIIDstring);
	}

	if (!iidString.IsEmpty())
	{
		TCHAR iidPath[48];
		TCHAR iidDescr[100];
		ULONG size = 100;
		DWORD type = REG_SZ;
		iidDescr[0] = '\0';
		_stprintf(iidPath, _T("Interface\\%-.38s"), (LPCTSTR)iidString);
		RegQueryValueEx(HKEY_CLASSES_ROOT, iidPath, NULL, &type, (LPBYTE)iidDescr, &size);

		if (_tcslen(iidDescr) > 0)
		{
			iidString = iidDescr;
		}
	}

	buffer = iidString;

	if (wIIDstring)
	{
		CoTaskMemFree(wIIDstring);
	}

	return (LPCTSTR)buffer;
} // IID2string


//-----------------------------------------------------------------------------
// CLSID2string
//
// - convert class id into class description string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::CLSID2string(
	IN SOCmnString& buffer, // string buffer
	IN REFCLSID rclsid)     // class id
{
	LPWSTR wCLSIDstring = NULL;
	SOCmnString clsidString;
	StringFromCLSID(rclsid, &wCLSIDstring);

	if (wCLSIDstring)
	{
		USES_CONVERSION;
		clsidString = OLE2T(wCLSIDstring);
	}

	if (!clsidString.IsEmpty())
	{
		clsidString.Format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), rclsid.Data1, rclsid.Data2, rclsid.Data3, rclsid.Data4[0], rclsid.Data4[1], rclsid.Data4[2], rclsid.Data4[3], rclsid.Data4[4], rclsid.Data4[5], rclsid.Data4[6], rclsid.Data4[7]);
	}

	buffer = clsidString;

	if (wCLSIDstring)
	{
		CoTaskMemFree(wCLSIDstring);
	}

	return (LPCTSTR)buffer;
} // CLSID2string

#endif

//-----------------------------------------------------------------------------
// LCID2string
//
// - convert LCID to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::LCID2string(
	OUT SOCmnString& string, // result string
	IN LCID lcid,            // LCID to convert
	OPTIONAL IN LCID localeID)  // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (lcid)
		{
		case 0x0000:
			string = _T("Neutral");
			break;

		case 0x0400:
			string = _T("User Default");
			break;

		case 0x0800:
			string = _T("System Default");
			break;

		case 0x0407:
			string = _T("German");
			break;

		case 0x0807:
			string = _T("German (Swiss)");
			break;

		case 0x0C07:
			string = _T("German (Austria)");
			break;

		case 0x0409:
			string = _T("English (USA)");
			break;

		case 0x0809:
			string = _T("English (UK)");
			break;

		case 0x0019:
			string = _T("Russian");
			break;

		case 0x000E:
			string = _T("Hungarian");
			break;

		case 0x041D:
			string = _T("Swedish");
			break;

		default:
			string.format(_T("0x%4.4lX"), lcid);
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (lcid)
		{
		case 0x0000:
			string = _T("Neutral");
			break;

		case 0x0400:
			string = _T("Benutzer Default");
			break;

		case 0x0800:
			string = _T("System Default");
			break;

		case 0x0407:
			string = _T("Deutsch");
			break;

		case 0x0807:
			string = _T("Deutsch (Schweiz)");
			break;

		case 0x0C07:
			string = _T("Deutsch (Österreich)");
			break;

		case 0x0409:
			string = _T("Englisch (USA)");
			break;

		case 0x0809:
			string = _T("Englisch (UK)");
			break;

		case 0x0019:
			string = _T("Rusisch");
			break;

		case 0x000E:
			string = _T("Ungarisch");
			break;

		case 0x041D:
			string = _T("Schwedisch");
			break;

		default:
			string.format(_T("0x%4.4lX"), lcid);
			break;
		}

		break;

	default:
		string.format(_T("0x%4.4lX"), lcid);
		break;
	}

	return (LPCTSTR)string;
} // LCID2string


//-----------------------------------------------------------------------------
// serverState2string
//
// - convert server state to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::serverState2string(
	OUT SOCmnString& string, // result string
	IN DWORD serverStatus,   // server status
	OPTIONAL IN LCID localeID)  // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (serverStatus)
		{
		case OPC_STATUS_RUNNING:
			string = _T("running");
			break;

		case OPC_STATUS_FAILED:
			string = _T("failed");
			break;

		case OPC_STATUS_NOCONFIG:
			string = _T("no configuration");
			break;

		case OPC_STATUS_SUSPENDED:
			string = _T("suspended");
			break;

		case OPC_STATUS_TEST:
			string = _T("test");
			break;

		default:
			string.format(_T("0x%8.8lX"), serverStatus);
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (serverStatus)
		{
		case OPC_STATUS_RUNNING:
			string = _T("Läuft");
			break;

		case OPC_STATUS_FAILED:
			string = _T("Fehler");
			break;

		case OPC_STATUS_NOCONFIG:
			string = _T("Keine Konfiguration");
			break;

		case OPC_STATUS_SUSPENDED:
			string = _T("Angehalten");
			break;

		case OPC_STATUS_TEST:
			string = _T("Test");
			break;

		default:
			string.format(_T("0x%8.8lX"), serverStatus);
			break;
		}

		break;

	default:
		string.format(_T("0x%8.8lX"), serverStatus);
		break;
	}

	return (LPCTSTR)string;
} // serverState2string

LPCTSTR SOSrvConverter::bool2string(IN SOCmnString& buffer, IN BOOL b, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		buffer = (b) ? _T("true") : _T("false");
		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		buffer = (b) ? _T("richtig") : _T("falsch");
		break;

	default:
		buffer = (b) ? _T("true") : _T("false");
		break;
	}

	return (LPCTSTR)buffer;
}

LPCTSTR SOSrvConverter::publicState2string(IN SOCmnString& buffer, IN BOOL pub, IN BOOL noTempl, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		if (!pub)
		{
			buffer = _T("private");
		}
		else if (noTempl)
		{
			buffer = _T("public");
		}
		else
		{
			buffer = _T("public template");
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		if (!pub)
		{
			buffer = _T("privat");
		}
		else if (noTempl)
		{
			buffer = _T("öffentlich");
		}
		else
		{
			buffer = _T("öffentliche Vorlage");
		}

		break;

	default:
		if (!pub)
		{
			buffer = _T("private");
		}
		else if (noTempl)
		{
			buffer = _T("public");
		}
		else
		{
			buffer = _T("public template");
		}

		break;
	}

	return (LPCTSTR)buffer;
}

LPCTSTR SOSrvConverter::euType2string(IN SOCmnString& buffer, IN OPCEUTYPE euType, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		if (euType == OPC_ANALOG)
		{
			buffer = _T("analog");
		}
		else
		{
			buffer = _T("enumeration");
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		if (euType == OPC_ANALOG)
		{
			buffer = _T("Analog");
		}
		else
		{
			buffer = _T("Aufzählung");
		}

		break;

	default:
		if (euType == OPC_ANALOG)
		{
			buffer = _T("analog");
		}
		else
		{
			buffer = _T("enumeration");
		}

		break;
	}

	return (LPCTSTR)buffer;
}


//-----------------------------------------------------------------------------
// filters2string
//
// - convert filters to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::filters2string(
	OUT SOCmnString& string,   // result string
	IN DWORD avFilter,         // available filters
	OPTIONAL IN LCID localeID) // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;
	DWORD i;
	DWORD filter = 0x00000001;
	BOOL addComma = FALSE;
	string.empty();

	for (i = 0; i < 5; i++)
	{
		if (avFilter & filter)
		{
			if (addComma)
			{
				string += _T(", ");
			}

			switch (locID)
			{
			case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
			case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
			case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
			case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
				switch (filter)
				{
				case OPC_FILTER_BY_EVENT:
					string += _T("event");
					break;

				case OPC_FILTER_BY_CATEGORY:
					string += _T("category");
					break;

				case OPC_FILTER_BY_SEVERITY:
					string += _T("serverity");
					break;

				case OPC_FILTER_BY_AREA:
					string += _T("area");
					break;

				case OPC_FILTER_BY_SOURCE:
					string += _T("source");
					break;
				}

				break;

			case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
				switch (filter)
				{
				case OPC_FILTER_BY_EVENT:
					string += _T("Ereignis");
					break;

				case OPC_FILTER_BY_CATEGORY:
					string += _T("Kategorie");
					break;

				case OPC_FILTER_BY_SEVERITY:
					string += _T("Bedeutung");
					break;

				case OPC_FILTER_BY_AREA:
					string += _T("Bereich");
					break;

				case OPC_FILTER_BY_SOURCE:
					string += _T("Quelle");
					break;
				}

				break;

			default:
				string.format(_T("0x%8.8lX"), avFilter);
				i = 5;
				break;
			}

			addComma = TRUE;
		}

		filter <<= 1;
	}

	return (LPCTSTR)string;
} // filters2string


//-----------------------------------------------------------------------------
// eventTypeFilter2string
//
// - convert filters to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::eventTypeFilter2string(
	OUT SOCmnString& string,   // result string
	IN DWORD eventType,        // available filters
	OPTIONAL IN LCID localeID) // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (eventType)
		{
		case 0:
			string = _T("no filter");
			break;

		case OPC_SIMPLE_EVENT:
			string = _T("simple");
			break;

		case OPC_TRACKING_EVENT:
			string = _T("tracking");
			break;

		case OPC_CONDITION_EVENT:
			string = _T("condition");
			break;

		case (OPC_SIMPLE_EVENT|OPC_TRACKING_EVENT):
			string = _T("simple,tracking");
			break;

		case (OPC_SIMPLE_EVENT|OPC_CONDITION_EVENT):
			string = _T("simple,condition");
			break;

		case (OPC_TRACKING_EVENT|OPC_CONDITION_EVENT):
			string = _T("tracking,condition");
			break;

		case (OPC_SIMPLE_EVENT|OPC_TRACKING_EVENT|OPC_CONDITION_EVENT):
			string = _T("simple,tracking,condition");
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (eventType)
		{
		case 0:
			string = _T("Kein Filter");
			break;

		case OPC_SIMPLE_EVENT:
			string = _T("simple");
			break;

		case OPC_TRACKING_EVENT:
			string = _T("tracking");
			break;

		case OPC_CONDITION_EVENT:
			string = _T("condition");
			break;

		case (OPC_SIMPLE_EVENT|OPC_TRACKING_EVENT):
			string = _T("simple,tracking");
			break;

		case (OPC_SIMPLE_EVENT|OPC_CONDITION_EVENT):
			string = _T("simple,condition");
			break;

		case (OPC_TRACKING_EVENT|OPC_CONDITION_EVENT):
			string = _T("tracking,condition");
			break;

		case (OPC_SIMPLE_EVENT|OPC_TRACKING_EVENT|OPC_CONDITION_EVENT):
			string = _T("simple,tracking,condition");
			break;
		}

		break;

	default:
		string.format(_T("0x%8.8lX"), eventType);
		break;
	}

	return (LPCTSTR)string;
} // eventTypeFilter2string


//-----------------------------------------------------------------------------
// conditionState2string
//
// - condition state to string
//
// returns:
//		pointer to string
//
LPCTSTR SOSrvConverter::conditionState2string(
	OUT SOCmnString& string,   // result string
	IN BYTE conditionState,    // condition state
	OPTIONAL IN LCID localeID) // locale id
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (conditionState)
		{
		case 0:
			string = _T("not active, not acked, not enabled");
			break;

		case OPC_CONDITION_ENABLED:
			string = _T("not active, not acked, enabled");
			break;

		case OPC_CONDITION_ACTIVE:
			string = _T("active, not acked, not enabled");
			break;

		case OPC_CONDITION_ACKED:
			string = _T("not active, acked, not enabled");
			break;

		case (OPC_CONDITION_ACTIVE|OPC_CONDITION_ENABLED):
			string = _T("active, not acked, enabled");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ENABLED):
			string = _T("not active, acked, enabled");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ACTIVE):
			string = _T("active, acked, not enabled");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ACTIVE|OPC_CONDITION_ENABLED):
			string = _T("active, acked, enabled");
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (conditionState)
		{
		case 0:
			string = _T("nicht activ, nicht quitiert, nicht ausgewertet");
			break;

		case OPC_CONDITION_ENABLED:
			string = _T("nicht activ, nicht quitiert, ausgewertet");
			break;

		case OPC_CONDITION_ACTIVE:
			string = _T("activ, nicht quitiert, nicht ausgewertet");
			break;

		case OPC_CONDITION_ACKED:
			string = _T("nicht activ, quitiert, nicht ausgewertet");
			break;

		case (OPC_CONDITION_ACTIVE|OPC_CONDITION_ENABLED):
			string = _T("activ, nicht quitiert, ausgewertet");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ENABLED):
			string = _T("nicht activ, quitiert, ausgewertet");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ACTIVE):
			string = _T("activ, quitiert, nicht ausgewertet");
			break;

		case (OPC_CONDITION_ACKED|OPC_CONDITION_ACTIVE|OPC_CONDITION_ENABLED):
			string = _T("activ, quitiert, ausgewertet");
			break;
		}

		break;

	default:
		string.format(_T("0x%8.8X"), conditionState);
		break;
	}

	return (LPCTSTR)string;
} // conditionState2string

#ifdef SOFEATURE_DCOM

LPCTSTR SOSrvConverter::authentificationLevel2string(IN SOCmnString& string, IN DWORD authLevel, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (authLevel)
		{
		case RPC_C_AUTHN_LEVEL_DEFAULT:
			string = _T("default");
			break;

		case RPC_C_AUTHN_LEVEL_NONE:
			string = _T("none");
			break;

		case RPC_C_AUTHN_LEVEL_CONNECT:
			string = _T("connect");
			break;

		case RPC_C_AUTHN_LEVEL_CALL:
			string = _T("call");
			break;

		case RPC_C_AUTHN_LEVEL_PKT:
			string = _T("packet");
			break;

		case RPC_C_AUTHN_LEVEL_PKT_INTEGRITY:
			string = _T("packet integrity");
			break;

		case RPC_C_AUTHN_LEVEL_PKT_PRIVACY:
			string = _T("packet privacy");
			break;

		default:
			string.format(_T("0x%8.8X"), authLevel);
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (authLevel)
		{
		case RPC_C_AUTHN_LEVEL_DEFAULT:
			string = _T("Standard");
			break;

		case RPC_C_AUTHN_LEVEL_NONE:
			string = _T("Kein");
			break;

		case RPC_C_AUTHN_LEVEL_CONNECT:
			string = _T("Verbinden");
			break;

		case RPC_C_AUTHN_LEVEL_CALL:
			string = _T("Anrufen");
			break;

		case RPC_C_AUTHN_LEVEL_PKT:
			string = _T("Paket");
			break;

		case RPC_C_AUTHN_LEVEL_PKT_INTEGRITY:
			string = _T("Paketintegrität");
			break;

		case RPC_C_AUTHN_LEVEL_PKT_PRIVACY:
			string = _T("Paketvertraulichkeit");
			break;

		default:
			string.format(_T("0x%8.8X"), authLevel);
			break;
		}

		break;

	default:
		string.format(_T("0x%8.8X"), authLevel);
		break;
	}

	return (LPCTSTR)string;
} // authentificationLevel2string

LPCTSTR SOSrvConverter::impersonationLevel2string(IN SOCmnString& string, IN DWORD impLevel, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		switch (impLevel)
		{
#ifdef RPC_C_IMP_LEVEL_DEFAULT

		case RPC_C_IMP_LEVEL_DEFAULT:
			string = _T("default");
			break;
#endif

		case RPC_C_IMP_LEVEL_ANONYMOUS:
			string = _T("anonymous");
			break;

		case RPC_C_IMP_LEVEL_IDENTIFY:
			string = _T("identify");
			break;

		case RPC_C_IMP_LEVEL_IMPERSONATE:
			string = _T("impersonate");
			break;

		case RPC_C_IMP_LEVEL_DELEGATE:
			string = _T("delegate");
			break;

		default:
			string.format(_T("0x%8.8X"), impLevel);
			break;
		}

		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		switch (impLevel)
		{
#ifdef RPC_C_IMP_LEVEL_DEFAULT

		case RPC_C_IMP_LEVEL_DEFAULT:
			string = _T("Standard");
			break;
#endif

		case RPC_C_IMP_LEVEL_ANONYMOUS:
			string = _T("Anonym");
			break;

		case RPC_C_IMP_LEVEL_IDENTIFY:
			string = _T("Identifizieren");
			break;

		case RPC_C_IMP_LEVEL_IMPERSONATE:
			string = _T("Darstellen");
			break;

		case RPC_C_IMP_LEVEL_DELEGATE:
			string = _T("Delegieren");
			break;

		default:
			string.format(_T("0x%8.8X"), impLevel);
			break;
		}

		break;

	default:
		string.format(_T("0x%8.8X"), impLevel);
		break;
	}

	return (LPCTSTR)string;
} // impersonationLevel2string

LPCTSTR SOSrvConverter::account2string(IN SOCmnString& string, IN LPCTSTR identity, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;

	switch (locID)
	{
	case MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT):
	case MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT):
		if (_tcsicmp(identity, _T("LocalSystem")))
		{
			string = _T("Local System");
		}

		string = identity;
		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		if (_tcsicmp(identity, _T("Interactive User")))
		{
			string = _T("Interaktiver Benutzer");
		}
		else if (_tcsicmp(identity, _T("Launching User")))
		{
			string = _T("Benutzer, der die Anwendung startet");
		}
		else if (_tcsicmp(identity, _T("LocalSystem")))
		{
			string = _T("Lokales System");
		}

		break;

	default:
		string = identity;
		break;
	}

	return (LPCTSTR)string;
} // account2string

#endif

LPCTSTR SOSrvConverter::result2string(IN SOCmnString& buffer, IN HRESULT res, IN DWORD specId, OPTIONAL IN LCID localeID)
{
	LCID locID = (localeID == SOSRVCONVERTER_LCID_DEFAULT) ? m_lcid : localeID;
	SOSrvServer::doGetErrorString(res, specId, locID, buffer);
	return (LPCTSTR)buffer;
}

#endif // SOSRV

