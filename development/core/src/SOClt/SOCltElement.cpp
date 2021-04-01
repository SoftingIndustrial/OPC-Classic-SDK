#include "stdafx.h"

#ifdef SOCLT

#include <opcerror.h>
#include "SOCltEntry.h"
#include "SOCltElement.h"
#include "SOCltThread.h"
#include "DllMain.h"

static TCHAR g_emptyString[2] = _T("");

//-----------------------------------------------------------------------------
// SOCltElement                                                               |
//-----------------------------------------------------------------------------

SOCltElement::SOCltElement(
	IN DWORD objType)                   // object type
	: SOCmnElement(objType)
{
	setObjectState(disconnected);
	setTargetObjectState(disconnected);
	m_persist = TRUE;
	m_connectionMonitorStatus = FALSE;
	m_singleItemTransactionHandle = 0;
} // SOCltElement

SOCltElement::~SOCltElement()
{
} // ~SOCltElement

SOCmnObject* SOCltElement::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
}

//-----------------------------------------------------------------------------
// setSingleItemTransactionHandle

void SOCltElement::setSingleItemTransactionHandle(DWORD pElementHandle)
{
	m_singleItemTransactionHandle = pElementHandle;
}

//-----------------------------------------------------------------------------
// getSingleItemTransactionHandle

DWORD SOCltElement::getSingleItemTransactionHandle()
{
	return m_singleItemTransactionHandle;
}

//-----------------------------------------------------------------------------
// setObjectState

BOOL SOCltElement::setObjectState(IN BYTE state)
{
	m_objectState = state;
	INVOKE_CALLBACK(SOCltElement, onSetObjectState);
	return TRUE;
}

//-----------------------------------------------------------------------------
// setTargetObjectState

BOOL SOCltElement::setTargetObjectState(IN BYTE state)
{
	m_objectStateTarget = state;
	INVOKE_CALLBACK(SOCltElement, onSetTargetObjectState);
	return TRUE;
}

BOOL SOCltElement::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	if (m_parent == parent)
	{
		return TRUE;    // no change to before, -> no check necessary
	}

	if (parent != NULL)
	{
		// check if the elements name is unique in parent branch
		if (parent->isUniqueChildName(getName(), this))
		{
			parent->addRef();

			if (m_parent)
			{
				m_parent->release();
			}

			m_parent = parent;
			INVOKE_CALLBACK(SOCltElement, onSetParentBranch);
			return TRUE;
		}
		else
		{
			if (m_parent)
			{
				m_parent->release();
			}

			m_parent = NULL;
			INVOKE_CALLBACK(SOCltElement, onSetParentBranch);
			return FALSE;
		}
	}
	else
	{
		if (m_parent)
		{
			m_parent->release();
		}

		m_parent = NULL;
		INVOKE_CALLBACK(SOCltElement, onSetParentBranch);
		return TRUE;
	}
} // setParentBranch

//-----------------------------------------------------------------------------
// getWorkerThread

SOCltWorkerThread* SOCltElement::getWorkerThread(void)
{
	SOCmnPointer<SOCltElement> parent = (SOCltElement*)getParentBranch();
	return !parent ? NULL : parent->getWorkerThread() ;
} // getWorkerThread

//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the element if the target state differs from the
//   actual state of the element
//
// returns:
//		S_OK
//		S_FALSE
//
HRESULT SOCltElement::doStateTransition(BOOL /* deep */, BYTE /* stateDeep */)
{
	return E_FAIL;
} // doStateTransition


BOOL SOCltElement::performStateTransitionAction(DWORD /* action */)
{
	return TRUE;
}

HRESULT SOCltElement::doRecurseChildren(SOCmnList<SOCmnElement>* plist, BYTE state)
{
	HRESULT hr = S_OK;

	if (NULL == plist)
	{
		return S_OK;
	}

	SOCmnList<SOCmnElement> list(plist);
	SOCmnPointer<SOCltElement> element;
	SOCmnListPosition pos;
	list.dontAutoLock();
	list.lock();
	list.setDirty(FALSE);
	pos = list.getStartPosition();
	list.unlock();

	while (pos)
	{
		list.lock();

		if (list.isDirty())
		{
			// check current position!
			SOCmnListPosition checkPos;
			BOOL found = FALSE;
			checkPos = list.getStartPosition();

			while (checkPos)
			{
				if (checkPos == pos)
				{
					found = TRUE;
					break;
				}

				list.getNext(checkPos);
			}

			if (!found)
			{
				pos = list.getStartPosition();
			}

			list.setDirty(FALSE);

			if (pos == NULL)
			{
				list.unlock();
				break;
			}
		}

		element = (SOCltElement*)list.getNext(pos);
		element.addRef();
		list.unlock();

		element->setConnectionMonitorStatus(m_connectionMonitorStatus);

		if (state != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE)
		{
			element->setTargetObjectState(state);
		}

		HRESULT hrChild = element->doStateTransition(TRUE, state);

		if (hrChild != S_OK)
		{
			hr = S_FALSE;
		}
	}

	return hr;
}

//-----------------------------------------------------------------------------
// doDisconnect
//
// Force the object to disconnect
//

HRESULT SOCltElement::doDisconnect()
{
	setObjectState(disconnected);
	return S_OK;
} // doDisconnect


//-----------------------------------------------------------------------------
// doUpdate
//

HRESULT SOCltElement::doUpdate(IN BOOL /* fromServer */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_FAIL;
} // doUpdate

//-----------------------------------------------------------------------------
// start
//
// - start the element
//
// returns:
//		S_OK        - element and children started
//		error code  - problems with starting
//
LONG SOCltElement::start(
	IN OPTIONAL BOOL deep, /* = TRUE */
	IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */) // start items?
{
	LONG ok = S_OK;
	SOCmnPointer<SOCltTaskStateTransition> task;
	SOCmnPointer<SOCltEntry> entry = ::getSOCltEntry();
	SOCmnPointer<SOCltCreator> creator = entry->getCltCreator();
	task = creator->createTaskStateTransition(this, started, deep);
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
LONG SOCltElement::connect(
	IN OPTIONAL BOOL deep, /* = TRUE*/
	IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */) // connect items?
{
	LONG ok = S_OK;
	SOCmnPointer<SOCltTaskStateTransition> task;
	SOCmnPointer<SOCltEntry> entry = ::getSOCltEntry();
	SOCmnPointer<SOCltCreator> creator = entry->getCltCreator();
	task = creator->createTaskStateTransition(this, connected, deep);
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
LONG SOCltElement::disconnect(
	IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */)
{
	LONG ok = S_OK;
	SOCmnPointer<SOCltTaskStateTransition> task;
	SOCmnPointer<SOCltEntry> entry = ::getSOCltEntry();

	if (!entry)
	{
		return S_FALSE;
	}

	SOCmnPointer<SOCltCreator> creator = entry->getCltCreator();
	task = creator->createTaskStateTransition(this, disconnected, TRUE);
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
LONG SOCltElement::performStateTransition(
	IN OPTIONAL BOOL deep, /* = TRUE */
	IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */) // connect items?
{
	LONG ok = S_OK;
	SOCmnPointer<SOCltTaskStateTransition> task;
	SOCmnPointer<SOCltEntry> entry = ::getSOCltEntry();
	SOCmnPointer<SOCltCreator> creator = entry->getCltCreator();
	task = creator->createTaskStateTransition(this, SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE, deep);
	ok = executeTask(task, waitopt, handle);
	return ok;
} // performStateTransition


//-----------------------------------------------------------------------------
// executeTask

LONG SOCltElement::executeTask(IN SOCltTask* task,
							   IN OPTIONAL BOOL waitopt, /* = SOCLT_EXEC_AUTO_SYNC */
							   IN OPTIONAL SOCmnEventPointer handle /* = NULL */)
{
	_ASSERT_PTR(task);
	LONG ok = S_OK;
	SOCltWorkerThread* thread = getWorkerThread();

	if (thread && !thread->isOwnThread())
	{
		SOCmnEventPointer wait;
		SOCmnEvent hWait;

		if ((!handle.isInit()) && (waitopt == SOCLT_EXEC_AUTO_SYNC))
		{
			// create completion event for synchronous operation
			hWait.create();
			wait.set(SOCmnEventPointer(&hWait));
			_ASSERTION(wait.isInit(), "Failed to create synchronisation event");
		}
		else if (handle.isInit())
		{
			wait = handle;
		}

		if (wait.isInit())
		{
			task->setCompletionEvent(wait);
		}

		ok = thread->addTask(task);

		if ((ok) && (waitopt == SOCLT_EXEC_AUTO_SYNC))
		{
			wait.waitFor();
			ok = task->getResult();
		}
		else if ((ok) && (waitopt == SOCLT_EXEC_ASYNC))
		{
			ok = S_OK;
		}
		else
		{
			ok = S_FALSE;
		}
	}
	else
	{
		_ASSERTION(!handle.isInit(), "Potential deadlock. EventHandle ignored.");
		task->execute();

		ok = task->getResult();
	}

	return ok;
} // executeTask

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the element
//
// returns:
//		S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SOCltElement::queryInterface(
	IN REFIID /* riid */,       // interface id
	OUT void** /* interf */)    // interface pointer
{
	return E_NOINTERFACE;
} // queryInterface

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// update
//
// - update the element parameter
//
// returns:
//		S_OK          - operation succeeded
//
LONG SOCltElement::update(
	IN BOOL fromServer,                 /* update direction: write to server or read from */
	IN OPTIONAL DWORD whatParameters,   /* = 0xFFFFFFFF*/
	IN OPTIONAL BOOL waitopt,           /* = SOCLT_EXEC_AUTO_SYNC */
	IN OPTIONAL SOCmnEventPointer handle /* = NULL */) // update what parameters
{
	LONG ok = S_OK;
	SOCmnPointer<SOCltTaskUpdate> task;
	SOCMN_ALLOCATE_OBJECT_C(task, SOCltTaskUpdate(this, fromServer, whatParameters), (SOCltTaskUpdate*));
	ok = executeTask(task, waitopt, handle);
	return ok;
} // update


//-----------------------------------------------------------------------------
// serialize
//
// - store/retrieve element's data to/from
//   an archive
//
// returns:
//		TRUE         - operation succeeded
//		FALSE        - otherwise
//
BOOL SOCltElement::serialize(
	IN OUT SOCltStorage& /* ar */,
	IN BOOL /* isStoring */)
{
	return TRUE;
} // serialize

void SOCltElement::beforeInterfaceCall(void)
{}

void SOCltElement::afterInterfaceCall(void)
{}

void SOCltElement::connectionFailure(void)
{}



//-----------------------------------------------------------------------------
// SOCltElementIError                                                         |
//-----------------------------------------------------------------------------

SOCltElementIError::SOCltElementIError()
{
} // SOCltElementIError

SOCltElementIError::~SOCltElementIError()
{
} // ~SOCltElementIError


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOCltElementIError::getIntTraceId(void)
{
	SOCmnObject* obj;
	obj = getObjectCltElementIError();

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
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOCltElementIError::getObjectCltElementIError(void)
{
	return NULL;
} // getObjectCltElementIError


//-----------------------------------------------------------------------------
// getLastError
//
// - get the last error
//
// returns:
//		result code for last error
//
HRESULT SOCltElementIError::getLastError(
	OUT OPTIONAL DWORD* /* errorcode */)        // Toolkit error code
{
	return E_NOTIMPL;
} // getLastError


//-----------------------------------------------------------------------------
// setLastError
//
// - set the last error
//
// returns:
//		TRUE
//
BOOL SOCltElementIError::setLastError(
	IN HRESULT res,                           // system error code
	IN OPTIONAL DWORD errorcode)        // Toolkit error code
{
	if (FAILED(res))
	{
		if (getObjectCltElementIError())
		{
#ifdef SOOS_WINDOWS
			SOCltCallback clb;
			clb.type = 1;
			clb.f1.ferr = reinterpret_cast<SOCLTELEMENT_CALLBACKTYPE_ERROR>(&SOCltElementIError::onError);
			clb.f1.dw1 = res;
			clb.f1.dw2 = errorcode;
			clb.vtlb = (SOCLTELEMENT_VTBL)(SOCltElementIError*)this;
			(clb.e = getObjectCltElementIError())->addRef();
			SOCltInvokeCallback(clb);
#else
			onError(res, errorcode);
#endif
		}
	}

	return FALSE;
} // setLastError


BOOL SOCltElementIError::getErrorDescription(
	DWORD errorcode,
	SOCmnString& str,
	LCID lcid)
{
#ifdef SOOS_WINDOWS
	//WORD langID = LANGIDFROMLCID(lcid); // locale identifier
	LPVOID lpMsgBuf = NULL;

	if (errorcode & 0x20000000)
	{
		::FormatMessage(
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			getInstanceHandle(),
			errorcode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);

		if (NULL != lpMsgBuf)
		{
			str  = (LPCTSTR)lpMsgBuf;
			::LocalFree(lpMsgBuf);
		}
	}

	return (lpMsgBuf != NULL);
#else
	static LCID lcidGER = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT);

	switch (errorcode)
	{
	case OPC_E_BADTYPE:
		if (lcid != lcidGER)
		{
			str = _T("The server cannot convert the data between the specified format and/or requested data type and the canonical data type.");
		}
		else
		{
			str = _T("Der Server kann den Wert nicht zwischen dem geforderten und dem ursprnglichen Datentyp konvertieren.");
		}

		break;

	case OPC_E_BADRIGHTS:
		if (lcid != lcidGER)
		{
			str = _T("The item's access rights do not allow the operation.");
		}
		else
		{
			str = _T("Die Zugriffsrechte des Items erlauben die Operation nicht.");
		}

		break;

	case OPC_E_UNKNOWNITEMID:
	case OPC_E_UNKNOWNITEMNAME:
		if (lcid != lcidGER)
		{
			str = _T("The item ID is not defined in the server address space or no longer exists in the server address space.");
		}
		else
		{
			str = _T("Das Item ist nicht definiert oder nicht l�ger im Adressraum des Servers verfgbar.");
		}

		break;

	case OPC_E_INVALIDHANDLE:
	case OPC_E_INVALIDITEMNAME:
	case OPC_E_INVALIDITEMID:
		if (lcid != lcidGER)
		{
			str = _T("The item definition does not conform to the server syntax.");
		}
		else
		{
			str = _T("Die Item Definition entspricht nicht der Syntax des Servers.");
		}

		break;

	case OPC_E_INVALIDFILTER:
		if (lcid != lcidGER)
		{
			str = _T("The filter string was not valid.");
		}
		else
		{
			str = _T("Der Filter String war nicht gltig.");
		}

		break;

	case OPC_E_RANGE:
		if (lcid != lcidGER)
		{
			str = _T("The value was out of range.");
		}
		else
		{
			str = _T("Der Wert war ausserhalb des Wertebereiches.");
		}

		break;

	case OPC_S_UNSUPPORTEDRATE:
		if (lcid != lcidGER)
		{
			str = _T("The server does not support the requested data rate but will use the closest available rate.");
		}
		else
		{
			str = _T("Der Server untersttzt die angeforderte Datenrate nicht. Er wird aber die n�este verfgbare Rate benutzen.");
		}

		break;

	case OPC_S_CLAMP:
		if (lcid != lcidGER)
		{
			str = _T("A value passed to write was accepted but the output was clamped.");
		}
		else
		{
			str = _T("Der an write bergebene Wert wurde akzeptiert, aber die Ausgabe wurde gehalten.");
		}

		break;

	case OPC_E_INVALID_PID:
		if (lcid != lcidGER)
		{
			str = _T("The specified property ID is not valid for the item.");
		}
		else
		{
			str = _T("Die bergebene Eigenschaftskennung ist nicht gltig fr das Item.");
		}

		break;

	case OPC_E_INVALIDCONTINUATIONPOINT:
		if (lcid != lcidGER)
		{
			str = _T("The continuation point is not valid.");
		}
		else
		{
			str = _T("Der Fortsetzungspunkt ist nicht gltig.");
		}

		break;

	case OPC_S_DATAQUEUEOVERFLOW:
		if (lcid != lcidGER)
		{
			str = _T("Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.");
		}
		else
		{
			str = _T("Nicht jede Daten�derung wurde geliefert, da der Speicher des Servers seine Obergrenze erreicht hat und die �testen Werte verworfen wurden.");
		}

		break;

	case OPC_E_NOTSUPPORTED:
		if (lcid != lcidGER)
		{
			str = _T("The server does not support writing of quality and/or timestamp.");
		}
		else
		{
			str = _T("Der Server untersttzt nicht das Schreiben der Qualit� und/oder des Zeitstempels.");
		}

		break;

	case OPC_E_INVALIDHOLDTIME:
		if (lcid != lcidGER)
		{
			str = _T("The hold time is too long (determined by server).");
		}
		else
		{
			str = _T("Die Haltezeit ist zu lang (vom Server festgestellt).");
		}

		break;

	case OPC_E_INVALIDITEMPATH:
		if (lcid != lcidGER)
		{
			str = _T("The item path does not conform the servers syntax.");
		}
		else
		{
			str = _T("Der Item Pfad entspricht nicht der Syntax des Servers.");
		}

		break;

	case OPC_E_UNKNOWNPATH:
	case OPC_E_UNKNOWNITEMPATH:
		if (lcid != lcidGER)
		{
			str = _T("The item path is no longer available in the server address space.");
		}
		else
		{
			str = _T("Der Item Pfad nicht definiert oder nicht l�ger im Adressraum des Servers verfgbar.");
		}

		break;

	case OPC_E_NOSUBSCRIPTION:
		if (lcid != lcidGER)
		{
			str = _T("An invalid set of subscription handles was passed to the request.");
		}
		else
		{
			str = _T("Ein unglter Satz von Subscription Handles wurde mit der Anfrage bergeben.");
		}

		break;

	case OPC_E_READONLY:
		if (lcid != lcidGER)
		{
			str = _T("The value is read only and may not be written to.");
		}
		else
		{
			str = _T("Der Wert ist nur lesbar und kann nicht geschrieben werden.");
		}

		break;

	case OPC_E_WRITEONLY:
		if (lcid != lcidGER)
		{
			str = _T("The value is write-only and may not be read from or returned as part of a write response.");
		}
		else
		{
			str = _T("Der Wert ist nur schreibar und kann nicht gelesen oder als Teil einer Schreibantwort zurckgegeben werden.");
		}

		break;

	case OPC_E_TIMEDOUT:
		if (lcid != lcidGER)
		{
			str = _T("The operation took too long to complete (determined by server).");
		}
		else
		{
			str = _T("Die Anfrage hat zu viel Zeit ben�igt (vom Server festgestellt).");
		}

		break;

	case OPC_E_SERVERSTATE:
		if (lcid != lcidGER)
		{
			str = _T("The operation could not complete due to an abnormal server state.");
		}
		else
		{
			str = _T("Die Anfrage kann wegen eines nicht normalen Serverzustandes nicht bearbeitet werden.");
		}

		break;

	case OPC_E_ACCESS_DENIED:
		if (lcid != lcidGER)
		{
			str = _T("The server denies access (read and/or write) to the specified item. This error is typically caused by Web Service security (e.g. globally disabled write capabilities).");
		}
		else
		{
			str = _T("Der Server verbietet den Zugriff (lesen und/oder schreiben) auf das spezifizierte Item (read and/or write) to the specified item.");
		}

		break;

	case OPC_E_BUSY_XMLDA:
		if (lcid != lcidGER)
		{
			str = _T("The server is currenly processing another polled refresh for one or more of the subscriptions.");
		}
		else
		{
			str = _T("Der Server bearbeitet gerade einen anderen polled Refresh fr eine oder mehrere Subscriptions.");
		}

		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
#endif // SOOS_WINDOWS
}

//-----------------------------------------------------------------------------
// onError
//
// - called if error occurred
//
void SOCltElementIError::onError(
	IN HRESULT /* res */,         // error
	IN DWORD /* errorcode */)
{ } // onError




//-----------------------------------------------------------------------------
// SOCltElementErrorStore                                                     |
//-----------------------------------------------------------------------------

SOCltElementErrorStore::SOCltElementErrorStore()
{
	m_lastError = 0;
	m_lastErrorCode = 0;
} // SOCltElementErrorStore

//-----------------------------------------------------------------------------
// getLastError
//
// - get the last occurred error
//
// returns:
//		result code for last error
//
HRESULT SOCltElementErrorStore::getLastError(
	OUT OPTIONAL DWORD* errorcode)      // Toolkit error code
{
	if (errorcode)
	{
		*errorcode = m_lastErrorCode;
	}

	return m_lastError;
} // getLastError


//-----------------------------------------------------------------------------
// setLastError
//
// - set the last occurred error
//
// returns:
//		TRUE
//

BOOL SOCltElementErrorStore::setLastError(
	IN HRESULT res,
	IN OPTIONAL DWORD errorcode)
{
	m_lastError = res;
	m_lastErrorCode = errorcode;
	SOCltElementIError::setLastError(res, errorcode);
	return TRUE;
} // setLastError

#endif // SOCLT

