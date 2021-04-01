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
//  Filename    : SOSrvServer.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server object related classes                           |
//                - SOSrvServer: OPC server object                            |
//                - SOSrvObjectRoot: root object of OPC runtime object tree   |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#include <opcerror.h>
#include "SOSrvServer.h"
#include "SOSrvEntry.h"
#ifdef SOFEATURE_DCOM
#include "SOSrvComObject.h"
#endif
#ifdef SOOS_WINDOWS
#include "dllmain.h"
#endif

//-----------------------------------------------------------------------------
// SOSrvServer                                                                |
//-----------------------------------------------------------------------------

SOSrvServer::SOSrvServer(
	IN DWORD objType,
	IN OPTIONAL BYTE serverInstance)
	:  SOCmnElement(SOCMNOBJECT_TYPE_SERVER | objType)
{
	m_serverInstance = serverInstance;
	m_clientName = _T("");
	m_clientIp = _T("");
	m_clientRemote = 0;
	m_lcid = MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT);
	m_workEventId = 0;
	m_me = NULL;
	m_minRequestLoopTime = INFINITE;
#ifdef SOFEATURE_SOAP
	m_xmlMethodList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED);
#endif
	m_isDisconnectingClients = FALSE;
}


SOSrvServer::~SOSrvServer(void)
{
#ifdef SOFEATURE_WATCH

	if (m_watchObject.isNotNull())
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
		SOCmnList<SOSrvWatch> watchList = entry->getWatchList();
		watchList.removeObject(m_watchObject);
	}

#endif
#ifdef SOFEATURE_SOAP
	m_xmlMethodList.destroy();
	m_httpReceiver.release();
#endif
}


//-----------------------------------------------------------------------------
// release
//
// - decrement reference counter
// - free object if no reference left
//
// returns:
//		reference count
//
LONG SOSrvServer::release(void)
{
	LONG refCount = interlockedDecrement(&m_refCount);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OBJ_REF, (_T("release %li"), refCount))

	if (refCount <= 0)
	{
		// terminate request thread
		if (!m_workThread.isOwnThread())
		{
			m_workThread.stop();
		}
		else
		{
			m_workThread.detach();
		}

		SOCMN_FREE_OBJECT_THIS; // last reference -> free object
		return 0;
	}

	return refCount;
} // release


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
BOOL SOSrvServer::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	BOOL ret;
#ifdef SOFEATURE_WATCH
	BOOL doCollect = FALSE;
	SOCmnPointer<SOCmnElement> prevParent;

	if (m_parent)
	{
		if ((doCollect = m_parent->getCollectChanges()) != 0)
		{
			prevParent = m_parent;
			prevParent.addRef();
		}
	}
	else if (parent)
	{
		doCollect = parent->getCollectChanges();
	}

#endif
	ret = SOCmnElement::setParentBranch(parent);
#ifdef SOFEATURE_WATCH

	if ((doCollect || (getCollectChanges())) && (ret))
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();

		if (parent != NULL)
		{
			entry->watchAddObject(parent, this);
		}
		else if ((parent == NULL) && (prevParent.isNotNull()))
		{
			entry->watchRemoveObject(prevParent, this);
			prevParent.release();
		}
	}

#endif
	return ret;
}


//-----------------------------------------------------------------------------
// workThreadProc
//
// - thread main routine of working thread
//
// returns:
//		thread end state
//
DWORD _stdcall SOSrvServer::workThreadProc(void* context)
{
	SOSrvServer* srv = (SOSrvServer*)context;
	return srv->workThread();
} // workThreadProc


//-----------------------------------------------------------------------------
// createWorkThread
//
// - create working thread
//
// returns:
//		handle of working thread
//
BOOL SOSrvServer::createWorkThread(void)
{
	BOOL ret;
	// create events
	m_workEventId = m_workThread.createEvent();
	// create thread
	ret = m_workThread.start(workThreadProc, (void*)this);

	if (ret)
	{
		m_workThread.setPriority(THREAD_PRIORITY_HIGHEST);
	}

	return ret;
} // createWorkThread


//-----------------------------------------------------------------------------
// workThread
//
// - thread routine of working thread
//
// returns:
//		thread end state
//
DWORD SOSrvServer::workThread(void)
{
	BOOL end = FALSE;
	DWORD ret;
	DWORD waitTime = SOCMN_TIME_40DAYS;
	DWORD loopStart;
	DWORD loopTime = 0;
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("started work thread")));

	while (!end)
	{
		if (waitTime != 0)
		{
			ret = m_workThread.waitForEvents(waitTime);
		}
		else
		{
			ret = SOCMNEVENT_INVALID_ID;
		}

		if (ret == m_workThread.getStopEventId())
		{
			// end
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("terminate work thread")));
			end = TRUE;
		}
		else
		{
			// work
			// addRef()
			LONG refCount = interlockedIncrement(&m_refCount);

			if (refCount <= 1)
			{
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("terminate work thread (refCount==0)")));
				// only start the work loop if not in the release phase
				interlockedDecrement(&m_refCount);
				return 0;
			}

			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SRV_THREAD, (_T("loop >>>")));
			loopStart = ::GetTickCount();
			// do work
			waitTime = doWork(loopStart);

			// max wait time is 40 days
			if (SOCMN_TIME_40DAYS < waitTime)
			{
				waitTime = SOCMN_TIME_40DAYS;
			}
			else
			{
				loopTime = getTimeSpan(loopStart, ::GetTickCount());

				if (loopTime != 0)
				{
					if (waitTime >= loopTime)
					{
						waitTime -= loopTime;
					}
					else
					{
						waitTime = 0;
					}
				}

				if (m_minRequestLoopTime < waitTime)
				{
					waitTime = m_minRequestLoopTime;
				}
			}

			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SRV_THREAD, (_T("loop next:%lu needed:%lu <<<"), waitTime, loopTime));
			refCount = interlockedDecrement(&m_refCount);

			if (refCount <= 0)
			{
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SRV_THREAD, (_T("terminate work thread & free object (refCount==0)")));
				// terminate own thread & free object
				m_workThread.detach();
				SOCMN_FREE_OBJECT_THIS; // last reference -> free object
				return 0;
			}
		}
	}

	return 0;
} // workThread


//-----------------------------------------------------------------------------
// supportLCID
//
// - check if the local Id is supported by the server
//
// returns:
//		TRUE  - lcid supported
//		FALSE - lcid not supported
//
BOOL SOSrvServer::supportLCID(
	IN DWORD lcid)  // local Id
{
	SOCmnDWORDList lcidList;
	SOCmnListPosition pos;
	lcidList.create();
	queryAvailableLCIDs(lcidList);
	pos = lcidList.getStartPosition();

	while (pos)
	{
		if (lcidList.getNext(pos) == lcid)
		{
			return TRUE;
		}
	}

	return FALSE;
} // supportLCID


//-----------------------------------------------------------------------------
// setLCID
//
// - set local Id
//
// returns:
//		TRUE
//
BOOL SOSrvServer::setLCID(
	IN DWORD lcid)  // local Id
{
	if (supportLCID(lcid))
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

		if (m_watchObject.isNotNull())
		{
			SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
			conv->setLCID(lcid);
		}

#endif
		return TRUE;
	}

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL, (_T("can't set locale id to 0x%lX (LCID not supported)"), lcid));
	return FALSE;
} // setLCID


//-----------------------------------------------------------------------------
// onSetLCID
//
// - called before local id is set
// - default implementation: do nothing
//
void SOSrvServer::onSetLCID(
	IN DWORD /* lcid */)
{} // onSetLCID


//-----------------------------------------------------------------------------
// setClientName
//
// - set client name
//
// returns:
//		TRUE
//
BOOL SOSrvServer::setClientName(
	IN LPCTSTR clientName)  // client name
{
	onSetClientName(clientName);
	m_clientNameSync.lock();
	m_clientName = clientName;
	m_clientNameSync.unlock();
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		SOSrvWatch::attributeChange(this, SOWATCH_ATTRIBUTE_CLIENTNAME_T, m_clientName);
	}

#endif
	return TRUE;
} // setClientName

//-----------------------------------------------------------------------------
// setClientIp
//
// - set client ip
//
// returns:
//		TRUE
//
BOOL SOSrvServer::setClientIp(
	IN LPCTSTR clientIp)    // client ip
{
	m_clientIp = clientIp;
	return TRUE;
}

//-----------------------------------------------------------------------------
// setClientRemote
//
// - set client remote
//
// returns:
//		TRUE
//
BOOL SOSrvServer::setClientRemote(
	IN unsigned char clientRemote)  // client remote
{
	m_clientRemote = clientRemote;
	return TRUE;
}

//-----------------------------------------------------------------------------
// onSetClientName
//
// - called before client name is set
// - default implementation: do nothing
//
void SOSrvServer::onSetClientName(
	IN LPCTSTR /* name */)  // client name
{} // onSetClientName


//-----------------------------------------------------------------------------
// queryAvailableLCIDs
//
// - return all available locale Ids
//
// returns:
//		number of elements of the array
//
void SOSrvServer::queryAvailableLCIDs(
	OUT SOCmnDWORDList& lcidList)
{
	lcidList.add(MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT));
	lcidList.add(MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT));
	lcidList.add(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT));
	lcidList.add(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
	lcidList.add(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT));
	lcidList.add(MAKELCID(MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), SORT_DEFAULT));
#ifdef SOOS_WINDOWS
	LCID lcid = GetSystemDefaultLCID();

	if (lcidList.findObject(lcid) == NULL)
	{
		lcidList.add(lcid);
	}

	lcid = GetUserDefaultLCID();

	if (lcidList.findObject(lcid) == NULL)
	{
		lcidList.add(lcid);
	}

#endif
} // queryAvailableLCIDs


//-----------------------------------------------------------------------------
// getErrorString
//
// - get error string for an error code
//
// returns:
//		S_OK          - got error string
//		E_INVALIDARG  - no string for the error code
//
HRESULT SOSrvServer::getErrorString(
	IN HRESULT error,       // error code
	IN DWORD lcid,          // locale for string
	OUT SOCmnString& string)    // error string
{
	return doGetErrorString(error, getSpecID(), lcid, string);
}

HRESULT SOSrvServer::doGetErrorString(
	IN HRESULT error,       // error code
	IN DWORD specID,
	IN DWORD lcid,          // locale for string
	OUT SOCmnString& string)    // error string
{
#ifdef SOOS_WINDOWS
	LPVOID msgBuf = NULL;
	DWORD formatResult = 0;
	WORD langID = LANGIDFROMLCID(lcid); // locale identifier
	DWORD sysError;

	if (((error & 0xC0000000) == 0xC0000000) || // custom errors or
		((error & 0xC0000000) == 0x00000000))       // success codes
	{
		// search for OPC errorcodes
		HMODULE module = ::getInstanceHandle();
		DWORD modError = error & 0xF000FFFF;
		// mask OPC error mask
		modError |= specID;
		formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
									 module, modError, langID, (LPTSTR) &msgBuf, 0, NULL);

		if (formatResult == 0)
		{
			sysError = GetLastError();

			if (sysError == ERROR_RESOURCE_LANG_NOT_FOUND)
				formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
											 module, modError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR) &msgBuf, 0, NULL);
		}
	}

	if (formatResult == 0)
	{
		// search for system errorcodes
		formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
									 NULL, error, langID, (LPTSTR) &msgBuf, 0, NULL);

		if (formatResult == 0)
		{
			sysError = GetLastError();

			if (sysError == ERROR_RESOURCE_LANG_NOT_FOUND)
				formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
											 NULL, error, LANG_SYSTEM_DEFAULT, (LPTSTR) &msgBuf, 0, NULL);
		}
	}

	if (formatResult != 0)
	{
		string = (LPTSTR)msgBuf;
		LocalFree(msgBuf);
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}

#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
	static LCID lcidGER = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);

	switch (error)
	{
	case OPC_E_BADTYPE:
		if (lcid != lcidGER)
		{
			string = _T("The server cannot convert the data between the specified format and/or requested data type and the canonical data type.");
		}
		else
		{
			string = _T("Der Server kann den Wert nicht zwischen dem geforderten und dem ursprünglichen Datentyp konvertieren.");
		}

		break;

	case OPC_E_BADRIGHTS:
		if (lcid != lcidGER)
		{
			string = _T("The item's access rights do not allow the operation.");
		}
		else
		{
			string = _T("Die Zugriffsrechte des Items erlauben die Operation nicht.");
		}

		break;

	case OPC_E_UNKNOWNITEMID:
	case OPC_E_UNKNOWNITEMNAME:
		if (lcid != lcidGER)
		{
			string = _T("The item ID is not defined in the server address space or no longer exists in the server address space.");
		}
		else
		{
			string = _T("Das Item ist nicht definiert oder nicht länger im Adressraum des Servers verfügbar.");
		}

		break;

	case OPC_E_INVALIDHANDLE:
	case OPC_E_INVALIDITEMNAME:
	case OPC_E_INVALIDITEMID:
		if (lcid != lcidGER)
		{
			string = _T("The item definition does not conform to the server syntax.");
		}
		else
		{
			string = _T("Die Item Definition entspricht nicht der Syntax des Servers.");
		}

		break;

	case OPC_E_INVALIDFILTER:
		if (lcid != lcidGER)
		{
			string = _T("The filter string was not valid.");
		}
		else
		{
			string = _T("Der Filter String war nicht gültig.");
		}

		break;

	case OPC_E_RANGE:
		if (lcid != lcidGER)
		{
			string = _T("The value was out of range.");
		}
		else
		{
			string = _T("Der Wert war ausserhalb des Wertebereiches.");
		}

		break;

	case OPC_S_UNSUPPORTEDRATE:
		if (lcid != lcidGER)
		{
			string = _T("The server does not support the requested data rate but will use the closest available rate.");
		}
		else
		{
			string = _T("Der Server unterstützt die angeforderte Datenrate nicht. Er wird aber die näheste verfügbare Rate benutzen.");
		}

		break;

	case OPC_S_CLAMP:
		if (lcid != lcidGER)
		{
			string = _T("A value passed to write was accepted but the output was clamped.");
		}
		else
		{
			string = _T("Der an write übergebene Wert wurde akzeptiert, aber die Ausgabe wurde gehalten.");
		}

		break;

	case OPC_E_INVALID_PID:
		if (lcid != lcidGER)
		{
			string = _T("The specified property ID is not valid for the item.");
		}
		else
		{
			string = _T("Die übergebene Eigenschaftskennung ist nicht gültig für das Item.");
		}

		break;

	case OPC_E_INVALIDCONTINUATIONPOINT:
		if (lcid != lcidGER)
		{
			string = _T("The continuation point is not valid.");
		}
		else
		{
			string = _T("Der Fortsetzungspunkt ist nicht gültig.");
		}

		break;

	case OPC_S_DATAQUEUEOVERFLOW:
		if (lcid != lcidGER)
		{
			string = _T("Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.");
		}
		else
		{
			string = _T("Nicht jede Datenänderung wurde geliefert, da der Speicher des Servers seine Obergrenze erreicht hat und die ältesten Werte verworfen wurden.");
		}

		break;

	case OPC_E_NOTSUPPORTED:
		if (lcid != lcidGER)
		{
			string = _T("The server does not support writing of quality and/or timestamp.");
		}
		else
		{
			string = _T("Der Server unterstützt nicht das Schreiben der Qualität und/oder des Zeitstempels.");
		}

		break;

	case OPC_E_INVALIDHOLDTIME:
		if (lcid != lcidGER)
		{
			string = _T("The hold time is too long (determined by server).");
		}
		else
		{
			string = _T("Die Haltezeit ist zu lang (vom Server festgestellt).");
		}

		break;

	case OPC_E_INVALIDITEMPATH:
		if (lcid != lcidGER)
		{
			string = _T("The item path does not conform the server’s syntax.");
		}
		else
		{
			string = _T("Der Item Pfad entspricht nicht der Syntax des Servers.");
		}

		break;

	case OPC_E_UNKNOWNPATH:
	case OPC_E_UNKNOWNITEMPATH:
		if (lcid != lcidGER)
		{
			string = _T("The item path is no longer available in the server address space.");
		}
		else
		{
			string = _T("Der Item Pfad nicht definiert oder nicht länger im Adressraum des Servers verfügbar.");
		}

		break;

	case OPC_E_NOSUBSCRIPTION:
		if (lcid != lcidGER)
		{
			string = _T("An invalid set of subscription handles was passed to the request.");
		}
		else
		{
			string = _T("Ein ungülter Satz von Subscription Handles wurde mit der Anfrage übergeben.");
		}

		break;

	case OPC_E_READONLY:
		if (lcid != lcidGER)
		{
			string = _T("The value is read only and may not be written to.");
		}
		else
		{
			string = _T("Der Wert ist nur lesbar und kann nicht geschrieben werden.");
		}

		break;

	case OPC_E_WRITEONLY:
		if (lcid != lcidGER)
		{
			string = _T("The value is write-only and may not be read from or returned as part of a write response.");
		}
		else
		{
			string = _T("Der Wert ist nur schreibar und kann nicht gelesen oder als Teil einer Schreibantwort zurückgegeben werden.");
		}

		break;

	case OPC_E_TIMEDOUT:
		if (lcid != lcidGER)
		{
			string = _T("The operation took too long to complete (determined by server).");
		}
		else
		{
			string = _T("Die Anfrage hat zu viel Zeit benötigt (vom Server festgestellt).");
		}

		break;

	case OPC_E_SERVERSTATE:
		if (lcid != lcidGER)
		{
			string = _T("The operation could not complete due to an abnormal server state.");
		}
		else
		{
			string = _T("Die Anfrage kann wegen eines nicht normalen Serverzustandes nicht bearbeitet werden.");
		}

		break;

	case OPC_E_ACCESS_DENIED:
		if (lcid != lcidGER)
		{
			string = _T("The server denies access (read and/or write) to the specified item. This error is typically caused by Web Service security (e.g. globally disabled write capabilities).");
		}
		else
		{
			string = _T("Der Server verbietet den Zugriff (lesen und/oder schreiben) auf das spezifizierte Item (read and/or write) to the specified item.");
		}

		break;

	case OPC_E_BUSY_XMLDA:
		if (lcid != lcidGER)
		{
			string = _T("The server is currenly processing another polled refresh for one or more of the subscriptions.");
		}
		else
		{
			string = _T("Der Server bearbeitet gerade einen anderen polled Refresh für eine oder mehrere Subscriptions.");
		}

		break;

	default:
		return E_INVALIDARG;
		break;
	}

	return S_OK;
#endif
} // getErrorString


LPCTSTR SOSrvServer::getErrorCodeSZ(IN DWORD error, OUT SOCmnString& errCodeSZ)
{
	switch (error)
	{
	case S_OK:
		errCodeSZ = _T("S_OK");
		break;

	case OPC_S_CLAMP:
		errCodeSZ = _T("S_CLAMP");
		break;

	case OPC_S_DATAQUEUEOVERFLOW:
		errCodeSZ = _T("S_DATAQUEUEOVERFLOW");
		break;

	case E_ACCESSDENIED:
	case OPC_E_ACCESS_DENIED:
	case OPC_E_BADRIGHTS:
		errCodeSZ = _T("E_ACCESS_DENIED");
		break;

	case OPC_E_BUSY_XMLDA:
		errCodeSZ = _T("E_BUSY");
		break;

	case E_FAIL:
		errCodeSZ = _T("E_FAIL");
		break;

	case E_INVALIDARG:
		errCodeSZ = _T("E_INVALIDARG");
		break;

	case OPC_E_INVALIDCONTINUATIONPOINT:
		errCodeSZ = _T("E_INVALIDCONTINUATIONPOINT");
		break;

	case OPC_E_INVALIDFILTER:
		errCodeSZ = _T("E_INVALIDFILTER");
		break;

	case OPC_E_INVALIDHOLDTIME:
		errCodeSZ = _T("E_INVALIDHOLDTIME");
		break;

	case OPC_E_INVALIDITEMNAME:
	case OPC_E_INVALIDITEMID:
	case OPC_E_INVALIDHANDLE:
		errCodeSZ = _T("E_INVALIDITEMNAME");
		break;

	case OPC_E_INVALIDITEMPATH:
		errCodeSZ = _T("E_INVALIDITEMPATH");
		break;

	case OPC_E_INVALID_PID:
		errCodeSZ = _T("E_INVALIDPID");
		break;

	case OPC_E_NOSUBSCRIPTION:
		errCodeSZ = _T("E_NOSUBSCRIPTION");
		break;

	case OPC_E_NOTSUPPORTED:
		errCodeSZ = _T("E_NOTSUPPORTED");
		break;

	case E_OUTOFMEMORY:
		errCodeSZ = _T("E_OUTOFMEMORY");
		break;

	case DISP_E_OVERFLOW:
	case OPC_E_RANGE:
		errCodeSZ = _T("E_RANGE");
		break;

	case OPC_E_READONLY:
		errCodeSZ = _T("E_READONLY");
		break;

	case OPC_E_SERVERSTATE:
		errCodeSZ = _T("E_SERVERSTATE");
		break;

	case OPC_E_TIMEDOUT:
		errCodeSZ = _T("E_TIMEDOUT");
		break;

	case OPC_E_UNKNOWNITEMNAME:
	case OPC_E_UNKNOWNITEMID:
		errCodeSZ = _T("E_UNKNOWNITEMNAME");
		break;

	case OPC_E_UNKNOWNITEMPATH:
	case OPC_E_UNKNOWNPATH:
		errCodeSZ = _T("E_UNKNOWNITEMPATH");
		break;

	case OPC_E_WRITEONLY:
		errCodeSZ = _T("E_WRITEONLY");
		break;

	case OPC_S_UNSUPPORTEDRATE:
		errCodeSZ = _T("S_UNSUPPORTEDRATE");
		break;

	case OPC_E_BADTYPE:
		errCodeSZ = _T("E_BADTYPE");
		break;

	case OPC_E_PUBLIC:
		errCodeSZ = _T("E_PUBLIC");
		break;

	case OPC_E_DEADBANDNOTSET:
		errCodeSZ = _T("E_DEADBANDNOTSET");
		break;

	case OPC_E_DEADBANDNOTSUPPORTED:
		errCodeSZ = _T("E_DEADBANDNOTSUPPORTED");
		break;

	case OPC_E_NOBUFFERING:
		errCodeSZ = _T("E_NOBUFFERING");
		break;

	case OPC_E_RATENOTSET:
		errCodeSZ = _T("E_RATENOTSET");
		break;

	default:
		errCodeSZ.format(_T("ERROR 0x%8.8lX"), error);
		break;
	}

	return errCodeSZ;
}


//-----------------------------------------------------------------------------
// stdMatchStringFilter
//
// - string filter function of OPC Toolkit V1.x
//
// returns:
//		TRUE  - string matches filter
//		FALSE - string is filtered out
//
BOOL SOSrvServer::stdMatchStringFilter(
	IN LPCTSTR string,  // string
	IN LPCTSTR filter)  // filter
{
	BOOL match;
	int actS, actF;
	int prev;

	if (!filter)
	{
		return TRUE;
	}

	if (_tcslen(filter) == 0)
	{
		return TRUE;
	}

	if (!string)
	{
		return FALSE;
	}

	actS = 0;
	actF = 0;
	prev = -1;

	while (((string[actS] == filter[actF]) ||
			(filter[actF] == _T('*')) ||
			(prev != -1))
		   && (string[actS] != _T('\0')))
	{
		if (filter[actF] != _T('*'))
		{
			if (string[actS] == filter[actF])
			{
				actF++;
			}
			else
			{
				actF = prev;

				if (string[actS] == filter[actF + 1])
				{
					// match Puchheim with *heim
					actF += 2;
				}
			}
		}

		if (filter[actF] == _T('*'))
		{
			prev = actF;

			if (filter[actF + 1] != _T('\0'))
			{
				if (string[actS + 1] == filter[actF + 1])
				{
					actF++;
				}
			}
		}

		actS++;
	}

	if (filter[actF] == _T('*'))
	{
		actF++;
	}

	match = ((string[actS] == _T('\0')) && (filter[actF] == _T('\0')));
	return match;
} // stdMatchStringFilter


//-----------------------------------------------------------------------------
// opcMatchStringFilter
//
// - string filter function of the OPC Foundation
//
// returns:
//		TRUE  - string matches filter
//		FALSE - string is filtered out
//
BOOL SOSrvServer::opcMatchStringFilter(
	IN LPCTSTR String,      // string
	IN LPCTSTR Pattern,     // filter
	IN BOOL bCaseSensitive) // case sensitive or not
{
	if (!String)
	{
		return FALSE;
	}

	if (!Pattern)
	{
		return TRUE;
	}

	TCHAR c, p, l;

	for (; ;)
	{
		switch (p = (TCHAR)((bCaseSensitive) ? *Pattern++ : toupper(*Pattern++)))
		{
		case 0:                             // end of pattern
			return *String ? FALSE : TRUE;  // if end of string TRUE

		case _T('*'):
			while (*String)
			{
				// match zero or more char
				if (opcMatchStringFilter(String++, Pattern, bCaseSensitive))
				{
					return TRUE;
				}
			}

			return opcMatchStringFilter(String, Pattern, bCaseSensitive);

		case _T('?'):
			if (*String++ == 0)             // match any one char
			{
				return FALSE;    // not end of string
			}

			break;

		case _T('['):
			if ((c = (TCHAR)((bCaseSensitive) ? *String++ : toupper(*String++))) == 0)  // match char set
			{
				return FALSE;    // syntax
			}

			l = 0;

			if (*Pattern == _T('!'))   // match a char if NOT in set []
			{
				++Pattern;

				while ((p = (TCHAR)((bCaseSensitive) ? *Pattern++ : toupper(*Pattern++))) != _T('\0'))
				{
					if (p == _T(']'))               // if end of char set, then
					{
						break;    // no match found
					}

					if (p == _T('-'))
					{
						// check a range of chars?
						p = (TCHAR)((bCaseSensitive) ? *Pattern++ : toupper(*Pattern++));   // get high limit of range

						if (p == 0  ||  p == _T(']'))
						{
							return FALSE;    // syntax
						}

						if (c >= l  &&  c <= p)
						{
							return FALSE;    // if in range, return FALSE
						}
					}

					l = p;

					if (c == p)                 // if char matches this element
					{
						return FALSE;    // return false
					}
				}
			}
			else    // match if char is in set []
			{
				while ((p = (TCHAR)((bCaseSensitive) ? *Pattern++ : toupper(*Pattern++))) != _T('\0'))
				{
					if (p == _T(']'))               // if end of char set, then
					{
						return FALSE;    // no match found
					}

					if (p == _T('-'))
					{
						// check a range of chars?
						p = (TCHAR)((bCaseSensitive) ? *Pattern++ : toupper(*Pattern++));   // get high limit of range

						if (p == 0  ||  p == _T(']'))
						{
							return FALSE;    // syntax
						}

						if (c >= l  &&  c <= p)
						{
							break;    // if in range, move on
						}
					}

					l = p;

					if (c == p)                 // if char matches this element
					{
						break;    // move on
					}
				}

				while (p  &&  p != _T(']'))         // got a match in char set
				{
					p = *Pattern++;    // skip to end of set
				}
			}

			break;

		case _T('#'):
			c = *String++;

			if (!_istdigit(c))
			{
				return FALSE;    // not a digit
			}

			break;

		default:
			c = (TCHAR)((bCaseSensitive) ? *String++ : toupper(*String++));

			if (c != p)             // check for exact char
			{
				return FALSE;    // not a match
			}

			break;
		}
	}
} // opcMatchStringFilter


//-----------------------------------------------------------------------------
// includesFilterChars
//
// - check if a string includes filter chars
//
// returns:
//     TRUE  - includes filter chars
//     FALSE - doesn't include filter chars
//
BOOL SOSrvServer::includesFilterChars(
	IN LPCTSTR string)
{
	return (NULL != _tcspbrk(string, _T("*?#[]")));
} // includesFilterChars


//-----------------------------------------------------------------------------
// isPrivateSecurityAvailable
//
// - implements the server private security?
//
// returns:
//     TRUE  - server implements security
//     FALSE - server does not implements security
//
BOOL SOSrvServer::isPrivateSecurityAvailable(void)
{
	return FALSE;
} // isPrivateSecurityAvailable


//-----------------------------------------------------------------------------
// logon
//
// - logon user
//
// returns:
//     TRUE  - login accepted
//     FALSE - login denied
//
BOOL SOSrvServer::logon(
	IN LPCTSTR /* userName */, // user name
	IN LPCTSTR /* password */) // password
{
	return FALSE;
} // logon


//-----------------------------------------------------------------------------
// logoff
//
// - logoff user
//
BOOL SOSrvServer::logoff(void)
{
	return FALSE;
} // logoff


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
HRESULT SOSrvServer::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
} // checkAuthorizationDCOM
#endif

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getWatchObject
//
// - get the watch object of the server
//
// returns:
//		pointer to the watch object
//		NULL - no watch supported
//
SOSrvWatch* SOSrvServer::getWatchObject(void)
{
	if (!m_watchObject)
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
		m_watchObject = entry->addWatchObject();
	}

	m_watchObject.addRef();
	return m_watchObject;
} // getWatchObject


//-----------------------------------------------------------------------------
// getTraceObject
//
// - get the trace object of the server
//
// returns:
//		pointer to the trace object
//		NULL - no remote trace supported
//
SOSrvTrace* SOSrvServer::getTraceObject(void)
{
	if (!m_traceObject)
	{
		SOCMN_ALLOCATE_OBJECT_C(m_traceObject, SOSrvTrace(), (SOSrvTrace*))
	}

	m_traceObject.addRef();
	return m_traceObject;
} // getTraceObject

#endif // SOFEATURE_WATCH

#ifdef SOFEATURE_SOAP
DWORD SOSrvServer::insertXMLMethod(
	IN BYTE /* xmlMethodType */,
	IN SOSrvHTTPHandler* /* pHandler */,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN DWORD /* lenBody */,
	IN BYTE* /* pBody */)
{
	return (DWORD)E_FAIL;
}

BOOL SOSrvServer::convertLCIDtoLocaleID(LCID lcid, SOCmnString& localeId)
{
	BOOL ret = TRUE;
#ifdef SOOS_WINDOWS

	// handle windows default lcids
	if (lcid == MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
	{
		lcid = GetSystemDefaultLCID();
	}

	if (lcid == MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
	{
		lcid = GetUserDefaultLCID();
	}

#endif

	switch (lcid)
	{
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
		localeId = _T("en-US");
		break;

	case MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT):
		localeId = _T("de-DE");
		break;

	case MAKELCID(MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), SORT_DEFAULT):
		localeId = _T("sv-SE");
		break;

	default:
	{
		WORD lang = LANGIDFROMLCID(lcid);

		switch (PRIMARYLANGID(lang))
		{
		case LANG_ENGLISH:
			localeId = _T("en");
			break;

		case LANG_GERMAN:
			localeId = _T("de");
			break;

		case LANG_SWEDISH:
			localeId = _T("sv");
			break;

		default:
			ret = FALSE;
			localeId = _T("en-US"); // default is enlish US
			break;
		}
	}
	break;
	}

	return ret;
}

BOOL SOSrvServer::convertLocaleIDtoLCID(LPCTSTR localeId, LCID& lcid)
{
	BOOL ret = TRUE;

	if (_tcsncmp(localeId, _T("en"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	}
	else if (_tcsncmp(localeId, _T("de"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);
	}
	else if (_tcsncmp(localeId, _T("sv"), 2) == 0)
	{
		lcid = MAKELCID(MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), SORT_DEFAULT);
	}
	else
	{
		ret = FALSE;
		lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	}

	return ret;
}

BOOL SOSrvServer::setReqLocaleID(LPCTSTR reqLocId)
{
	LCID lcid;
	BOOL ret;
	ret = convertLocaleIDtoLCID(reqLocId, lcid);
	setLCID(lcid);
	return ret;
}

void SOSrvServer::getXMLQuality(WORD quality, IN BOOL withElement, OUT SOCmnStringEx& xmlQuality)
{
	WORD q = quality & OPC_STATUS_MASK;
	WORD l = quality & OPC_LIMIT_MASK;

	if (withElement)
	{
		xmlQuality = _T("<Quality");
	}

	if (q != OPC_QUALITY_GOOD)
	{
		switch (q)
		{
		case OPC_QUALITY_BAD:
			xmlQuality += _T(" QualityField=\"bad\"");
			break;

		case OPC_QUALITY_CONFIG_ERROR:
			xmlQuality += _T(" QualityField=\"badConfigurationError\"");
			break;

		case OPC_QUALITY_NOT_CONNECTED:
			xmlQuality += _T(" QualityField=\"badNotConnected\"");
			break;

		case OPC_QUALITY_DEVICE_FAILURE:
			xmlQuality += _T(" QualityField=\"badDeviceFailure\"");
			break;

		case OPC_QUALITY_SENSOR_FAILURE:
			xmlQuality += _T(" QualityField=\"badSensorFailure\"");
			break;

		case OPC_QUALITY_LAST_KNOWN:
			xmlQuality += _T(" QualityField=\"badLastKnownValue\"");
			break;

		case OPC_QUALITY_COMM_FAILURE:
			xmlQuality += _T(" QualityField=\"badCommFailure\"");
			break;

		case OPC_QUALITY_OUT_OF_SERVICE:
			xmlQuality += _T(" QualityField=\"badOutOfService\"");
			break;

		case OPC_QUALITY_WAITING_FOR_INITIAL_DATA:
			xmlQuality += _T(" QualityField=\"badWaitingForInitialData\"");
			break;

		case OPC_QUALITY_UNCERTAIN:
			xmlQuality += _T(" QualityField=\"uncertain\"");
			break;

		case OPC_QUALITY_LAST_USABLE:
			xmlQuality += _T(" QualityField=\"uncertainLastUsableValue\"");
			break;

		case OPC_QUALITY_SENSOR_CAL:
			xmlQuality += _T(" QualityField=\"uncertainSensorNotAccurate\"");
			break;

		case OPC_QUALITY_EGU_EXCEEDED:
			xmlQuality += _T(" QualityField=\"uncertainEUExceeded\"");
			break;

		case OPC_QUALITY_SUB_NORMAL:
			xmlQuality += _T(" QualityField=\"uncertainSubNormal\"");
			break;

		case OPC_QUALITY_LOCAL_OVERRIDE:
			xmlQuality += _T(" QualityField=\"goodLocalOverride\"");
			break;
		}
	}

	if (l != OPC_LIMIT_OK)
	{
		switch (l)
		{
		case OPC_LIMIT_LOW:
			xmlQuality += _T(" LimitField=\"low\"");
			break;

		case OPC_LIMIT_HIGH:
			xmlQuality += _T(" LimitField=\"high\"");
			break;

		case OPC_LIMIT_CONST:
			xmlQuality += _T(" LimitField=\"constant\"");
			break;
		}
	}

	if (withElement)
	{
		xmlQuality += _T("/>");
	}
}

WORD SOSrvServer::getDCOMQuality(IN LPCTSTR quality, IN LPCTSTR limit, IN LPCTSTR /* vendor */)
{
	WORD q = 0;

	if (_tcslen(quality) != 0)
	{
		if ((quality[0] == _T('b')) || (quality[0] == _T('B')))
		{
			if (_tcsicmp(quality, _T("bad")) == 0)
			{
				q = OPC_QUALITY_BAD;
			}
			else if (_tcsicmp(quality, _T("badConfigurationError")) == 0)
			{
				q = OPC_QUALITY_CONFIG_ERROR;
			}
			else if (_tcsicmp(quality, _T("badNotConnected")) == 0)
			{
				q = OPC_QUALITY_NOT_CONNECTED;
			}
			else if (_tcsicmp(quality, _T("badDeviceFailure")) == 0)
			{
				q = OPC_QUALITY_DEVICE_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badSensorFailure")) == 0)
			{
				q = OPC_QUALITY_SENSOR_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badLastKnownValue")) == 0)
			{
				q = OPC_QUALITY_LAST_KNOWN;
			}
			else if (_tcsicmp(quality, _T("badCommFailure")) == 0)
			{
				q = OPC_QUALITY_COMM_FAILURE;
			}
			else if (_tcsicmp(quality, _T("badOutOfService")) == 0)
			{
				q = OPC_QUALITY_OUT_OF_SERVICE;
			}
			else if (_tcsicmp(quality, _T("badWaitingForInitialData")) == 0)
			{
				q = OPC_QUALITY_WAITING_FOR_INITIAL_DATA;
			}
		}
		else if ((quality[0] == _T('u')) || (quality[0] == _T('U')))
		{
			if (_tcsicmp(quality, _T("uncertain")) == 0)
			{
				q = OPC_QUALITY_UNCERTAIN;
			}
			else if (_tcsicmp(quality, _T("uncertainLastUsableValue")) == 0)
			{
				q = OPC_QUALITY_LAST_USABLE;
			}
			else if (_tcsicmp(quality, _T("uncertainSensorNotAccurate")) == 0)
			{
				q = OPC_QUALITY_SENSOR_CAL;
			}
			else if (_tcsicmp(quality, _T("uncertainEUExceeded")) == 0)
			{
				q = OPC_QUALITY_EGU_EXCEEDED;
			}
			else if (_tcsicmp(quality, _T("uncertainSubNormal")) == 0)
			{
				q = OPC_QUALITY_SUB_NORMAL;
			}
			else
			{
				TCHAR shortQ[10] = {0};
				DWORD len = (DWORD) _tcslen(quality);
				if (len > 9)
				{
					memcpy((void*)shortQ, quality, 9 * sizeof(char));
					if (_tcsicmp(shortQ, _T("uncertain")) == 0)
					{
						q = OPC_QUALITY_UNCERTAIN;
					}
				}
			}		}
		else if ((quality[0] == _T('g')) || (quality[0] == _T('G')))
		{
			if (_tcsicmp(quality, _T("good")) == 0)
			{
				q = OPC_QUALITY_GOOD;
			}
			else if (_tcsicmp(quality, _T("goodLocalOverride")) == 0)
			{
				q = OPC_QUALITY_LOCAL_OVERRIDE;
			}
			else
			{
				TCHAR shortQuality[5] = {0};
				DWORD len = (DWORD) _tcslen(quality);
				if (len > 4)
				{
					memcpy((void*)shortQuality, quality, 4 * sizeof(char));
					if (_tcsicmp(shortQuality, _T("good")) == 0)
					{
						q = OPC_QUALITY_GOOD;
					}
				}
			}
		}
	}
	else
	{
		q = OPC_QUALITY_GOOD;
	}

	if (_tcslen(limit) != 0)
	{
		if (_tcsicmp(limit, _T("low")) == 0)
		{
			q |= OPC_LIMIT_LOW;
		}
		else if (_tcsicmp(limit, _T("high")) == 0)
		{
			q |= OPC_LIMIT_HIGH;
		}
		else if (_tcsicmp(limit, _T("constant")) == 0)
		{
			q |= OPC_LIMIT_CONST;
		}
	}

	return q;
}

#endif


//-----------------------------------------------------------------------------
// SOSrvObjectRoot                                                            |
//-----------------------------------------------------------------------------

SOSrvObjectRoot::SOSrvObjectRoot(void)
	: SOCmnElementListBranch(SOCMNLIST_TYPE_ORG_MAP),
	  SOCmnElement(SOCMNOBJECT_TYPE_ROOT)
{
}


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOSrvObjectRoot::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addLeaf
//
// - don't add leaf into OPC root object
//
// returns:
//		FALSE - not added
//
BOOL SOSrvObjectRoot::addLeaf(
	IN SOCmnElement* /* newLeaf */)
{
	return FALSE;
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a server object
//
// returns:
//		TRUE  - added
//		FALSE - not added
//
BOOL SOSrvObjectRoot::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_SERVER))
	{
		// branch is a server object -> add
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch

#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// addDCOMCnfgWatchData
//
// - adds DCOMCNFG watch data to the attributes of the object roots watch data
//
void SOSrvObjectRoot::addDCOMCnfgWatchData(
	IN SOSrvEntry* entry,
	IN SOCmnStringEx& szXML)
{
	SOCmnPointer<SOSrvConverter> conv = getSOSrvConverter();
	SOCmnString buffer;
	DWORD authLevel;
	DWORD impLevel;
	SOCmnString identity;
	SOCmnString serviceName;
	BYTE gotSecLevels = 0;
	CRegKey keyOLE, keyApp;
	TCHAR szBuffer[200];
	DWORD szSize;
	CLSID classid;

	if (entry->isInitialized(SOSRVENTRY_INIT_SECURITY))
	{
		DWORD authLevelCB;
		DWORD impLevelCB;
		authLevel = entry->getAuthentificationLevel();
		authLevelCB = entry->getAuthentificationLevelForAdvise();
		impLevel = entry->getImpersonationLevel();
		impLevelCB = entry->getImpersonationLevelForAdvise();
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVEL_T, conv->authentificationLevel2string(buffer, authLevel));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_IMPERSONATIONLEVEL_T, conv->impersonationLevel2string(buffer, impLevel));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVELCB_T, conv->authentificationLevel2string(buffer, authLevelCB));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_IMPERSONATIONLEVELCB_T, conv->impersonationLevel2string(buffer, impLevelCB));
		gotSecLevels = 0xFF;
	}

	if (entry->getCLSID(classid))
	{
		USES_CONVERSION;
		LPOLESTR wszClsid;
		SOCmnString appPath;
		StringFromCLSID(classid, &wszClsid);
		appPath.format(_T("AppID\\%s"), W2T(wszClsid));

		if (keyApp.Open(HKEY_CLASSES_ROOT, appPath, KEY_READ) == ERROR_SUCCESS)
		{
			if ((gotSecLevels & 0x01) == 0)
				if (keyApp.QueryDWORDValue(_T("AuthenticationLevel"), authLevel) == ERROR_SUCCESS)
				{
					SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVEL_T, conv->authentificationLevel2string(buffer, authLevel));
					gotSecLevels |= 0x01;
				}

			szSize = 200;

			if (keyApp.QueryStringValue(_T("LocalService"), szBuffer, &szSize) == ERROR_SUCCESS)
			{
				serviceName = szBuffer;
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_SERVICENAME_T, serviceName);
			}

			szSize = 200;

			if (keyApp.QueryStringValue(_T("RunAs"), szBuffer, &szSize) == ERROR_SUCCESS)
			{
				identity = szBuffer;
			}
			else
			{
				if (serviceName.isEmpty())
				{
					identity = _T("Launching User");
				}
				else
				{
					SOCmnString svcPath;
					CRegKey keySvc;
					svcPath.format(_T("System\\CurrentControlSet\\Services\\%s"), serviceName);

					if (keySvc.Open(HKEY_LOCAL_MACHINE, svcPath, KEY_READ) == ERROR_SUCCESS)
					{
						szSize = 200;

						if (keySvc.QueryStringValue(_T("ObjectName"), szBuffer, &szSize) == ERROR_SUCCESS)
						{
							identity = szBuffer;
						}
					}
				}
			}

			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_IDENTITY_T, conv->account2string(buffer, identity));
		}

		CoTaskMemFree(wszClsid);
	}

	if (keyOLE.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\OLE"), KEY_READ) == ERROR_SUCCESS)
	{
		szSize = 200;

		if (keyOLE.QueryStringValue(_T("EnableDCOM"), szBuffer, &szSize) == ERROR_SUCCESS)
		{
			BOOL enableDCOM = (_tcsicmp(_T("Y"), szBuffer) == 0) ? TRUE : FALSE;
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ENABLEDDCOM_T, conv->bool2string(buffer, enableDCOM));
		}

		if ((gotSecLevels & 0x01) == 0)
			if (keyOLE.QueryDWORDValue(_T("LegacyAuthenticationLevel"), authLevel) == ERROR_SUCCESS)
			{
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVEL_T, conv->authentificationLevel2string(buffer, authLevel));
			}

		if ((gotSecLevels & 0x02) == 0)
			if (keyOLE.QueryDWORDValue(_T("LegacyImpersonationLevel"), impLevel) == ERROR_SUCCESS)
			{
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_IMPERSONATIONLEVEL_T, conv->impersonationLevel2string(buffer, impLevel));
			}
	}
} // addDCOMCnfgWatchData

#endif // SOFEATURE_WATCH
#endif

#endif // SOSRV
