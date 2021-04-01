#include "stdafx.h"

#ifdef SOCLT

#include "SOCltTask.h"
#include "SOCltThread.h"

#ifndef SOSRV_IS_RPC_ERROR
#define SOSRV_IS_RPC_ERROR(res) ((FAILED(res))&& ((res == 0x800706BA) || (res == 0x800706BE) || (res == 0x800706BF) || (res == 0x80010100)|| (res == 0x80010108) || (res == 0x80010114)))
#endif // SOSRV_IS_RPC_ERROR

//-----------------------------------------------------------------------------
// SOCltTask                                                                  |
//-----------------------------------------------------------------------------

SOCltTask::SOCltTask(
	IN SOCltElement* element) // can be NULL
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	if (element)
	{
		_ASSERT_PTR(element);
		element->addRef();
		m_element = element;
	}

	m_res = E_PENDING;
}

SOCltTask::~SOCltTask()
{
	m_element.release();
}

void SOCltTask::reportCompletion()
{
	if (m_event.isInit())
	{
		m_event.signal();
	}
}




//-----------------------------------------------------------------------------
// SOCltTaskUpdate                                                            |
//-----------------------------------------------------------------------------

SOCltTaskUpdate::SOCltTaskUpdate(
	IN SOCltElement* element,
	IN BOOL from_server,
	IN DWORD whatParameters)
	: SOCltTask(element)
{
	m_fromServer = from_server;
	m_whatParameters = whatParameters;
}

void SOCltTaskUpdate::execute()
{
	_ASSERT_PTR((SOCltElement*)m_element);
	m_res = m_element->doUpdate(m_fromServer, m_whatParameters);
}




//-----------------------------------------------------------------------------
// SOCltTaskStateTransition                                                   |
//-----------------------------------------------------------------------------

SOCltTaskStateTransition::SOCltTaskStateTransition(
	IN SOCltElement* element,
	IN BYTE targetState,
	IN BOOL deep)
	: SOCltTask(element)
{
	_ASSERTION(targetState == SOCltElement::connected ||
			   targetState == SOCltElement::disconnected ||
			   targetState == SOCltElement::started ||
			   targetState == SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE, "Invalid target state");
	m_targetState = targetState;
	m_deep = deep;
}

void SOCltTaskStateTransition::execute()
{
	_ASSERT_PTR((SOCltElement*)m_element);
	m_res = S_OK;

	if (m_targetState != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE)
	{
		if (m_element->setTargetObjectState(m_targetState))
		{
			m_res = m_element->doStateTransition(m_deep, m_targetState);
		}
	}
	else
	{
		m_res = m_element->doStateTransition(m_deep, m_targetState);
	}
}

void SOCltTaskStateTransition::reportCompletion()
{
	// It is important for the shutdown sequence
	// that at the time of signaling the event
	// we do not hold any references to the object.
	if (m_event.isInit())
	{
		m_element.release();
		m_event.signal();
		m_event.set(SOCmnEventPointer(NULL, SOCMNEVENT_INVALID_ID));
	}
}



//-----------------------------------------------------------------------------
// SOCltCyclicTask                                                            |
//-----------------------------------------------------------------------------

SOCltCyclicTask::SOCltCyclicTask(
	IN SOCltElement* element,
	IN DWORD cycle)
	: SOCltTask(element)
{
	_ASSERTION(cycle > 0 && cycle < INFINITE, "Invalid cycle value");
	m_lastCall = ::GetTickCount();
	m_cycle = cycle;
} // SOCltCyclicTask

//-----------------------------------------------------------------------------
// reportCompletion
//

void SOCltCyclicTask::reportCompletion()
{
	if (m_event.isInit())
	{
		m_event.signal();
	}
}

//-----------------------------------------------------------------------------
// getNextTimeToRun
//

DWORD SOCltCyclicTask::getNextTimeToRun(void)
{
	return m_lastCall + getCycle();
} // getNextTimeToRun




//-----------------------------------------------------------------------------
// SOCltTaskConnectionMonitor                                                 |
//-----------------------------------------------------------------------------

// **************************
// The basic monitoring shema
// **************************
//
// |<- connection okay ->|<--------- phase 1 ------->|<------- phase 2 --...
// |                     |                           |
// |--|--|--|--|...|--|--|-----|-----|---...---|-----|---------------|---...
// |                     | # attempts to reconnect   |
//

SOCltTaskConnectionMonitor::SOCltTaskConnectionMonitor(
	IN SOCltElement* element,
	IN DWORD cycle)
	: SOCltCyclicTask(element, cycle)
{
	m_attempts = 10;
	m_cnt_attempts = 0;
	m_cycle1 = 10000;    // 10 sec
	m_cycle2 = 300000;   // 5 min
	m_state = is_idle;
} // SOCltTaskConnectionMonitor

SOCltTaskConnectionMonitor::~SOCltTaskConnectionMonitor()
{
}

#ifdef SOFEATURE_DCOM
static IID _iid_not_there_ = {0x51169E41, 0xE209, 0x11d2, {0x98, 0xF5, 0x00, 0x60, 0x08, 0x19, 0x8F, 0x00}};
#endif

//-----------------------------------------------------------------------------
// execute
//
void SOCltTaskConnectionMonitor::execute(void)
{
	_ASSERT_PTR((SOCltElement*)m_element);

	if (SOCltElement::disconnected != m_element->getObjectState())
	{
		if (m_element->is(SOCMNOBJECT_TYPE_SERVER))
		{
#ifdef SOFEATURE_DCOM
			{
				SOCltServer* pServer = (SOCltServer*)(SOCltElement*)m_element;

				if (pServer->getProtocol() == SOCLT_PROTOCOL_DCOM)
				{
					// Ping the real COM object, to see if it is still alive.
					// We simply ask for an non-existing interface.
					// Asking for an existing one would not work since COM cashes
					// interface pointers.
					CComPtr<IUnknown> pUnknown;
					HRESULT res = m_element->queryInterface(IID_IUnknown, (void**)&pUnknown);

					if (!!pUnknown)
					{
						void* pv = NULL;
						m_element->beforeInterfaceCall();
						res = pUnknown->QueryInterface(_iid_not_there_, &pv);
						m_element->afterInterfaceCall();
					}

					// Since we know the interface does not exist, the only valid
					// return code is E_NOINTERFACE. Anything else means connection
					// failure or server crash.

					if (res == E_NOINTERFACE)
					{
						m_res = S_OK;
						m_state = is_idle;
						m_lastCall = ::GetTickCount();
						return; // everything okay
					}

					m_res = res;
					m_state = reconnect1;
					m_cnt_attempts = 0;
					// Set element and it's children status to 'disconnected'
					doDisconnectElement(m_element);
				}
			}
#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_SOAP
			{
				SOCltServer* pServer = (SOCltServer*)(SOCltElement*)m_element;

				if (pServer->getProtocol() == SOCLT_PROTOCOL_SOAP)
				{
					SODaCServer* pDaCServer = (SODaCServer*)(SOCltElement*)m_element;
					SOCmnPointer<SOCltHTTPConnection> pHttpCon = pDaCServer->getHTTPConnection();

					if (pHttpCon.isNotNull() == TRUE)
					{
						if (pHttpCon->isConnected() != TRUE)
						{
							doDisconnectElement(m_element);
						} // end if
					} // end if
				} // end if
			}
#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_TUNNEL
			{
				SOCltServer* pServer = (SOCltServer*)(SOCltElement*)m_element;

				if (pServer->getProtocol() == SOCLT_PROTOCOL_TUNNEL)
				{
					if (pServer->is(SOCMNOBJECT_TYPE_SERVER_DA))
					{
						SODaCServer* pDaCServer = (SODaCServer*)(SOCltElement*)m_element;
						SOCmnPointer<SOCltTPConnection> tpCon = pDaCServer->getTPConnection();

						if (tpCon.isNotNull())
						{
							SOCmnPointer<SOCltServer> tpServer;
							tpServer = tpCon->getServer();

							if (!tpServer)
							{
								doDisconnectElement(m_element);
							}
						}
					}

#ifdef SOAEC
					else if (pServer->is(SOCMNOBJECT_TYPE_SERVER_AE))
					{
#ifdef SOOS_WINDOWS
						SOAeCServer* pAeCServer = (SOAeCServer*)(SOCltElement*)m_element;
						SOCmnPointer<SOCltTPConnection> tpCon = pAeCServer->getTPConnection();

						if (tpCon.isNotNull())
						{
							SOCmnPointer<SOCltServer> tpServer;
							tpServer = tpCon->getServer();

							if (!tpServer)
							{
								doDisconnectElement(m_element);
							}
						}

#endif  //  SOOS_WINDOWS
#ifdef SOOS_LINUX
						//TODO  Verify AE tunnel
#endif  //  SOOS_LINUX
					}

#endif //   SOAEC
				}
			}
#endif // SOFEATURE_TUNNEL
		}
	}

	doReconnectElement(m_element);

	if (reconnect1 == m_state)
	{
		m_cnt_attempts++;

		if (m_cnt_attempts > m_attempts)
		{
			m_state = reconnect2;
			m_cnt_attempts = 0;
		}
	}

	m_lastCall = ::GetTickCount();
} // execute

//-----------------------------------------------------------------------------
// getNextTimeToRun
//
DWORD SOCltTaskConnectionMonitor::getNextTimeToRun(void)
{
	DWORD nextTime = 0;

	switch (m_state)
	{
	default:
		_ASSERTION(FALSE, "Invalid enum value");

		// fall through
	case is_idle:
		nextTime = m_lastCall + getCycle();
		break;

	case reconnect1:
		nextTime = m_lastCall + m_cycle1;
		break;

	case reconnect2:
		nextTime = m_lastCall + m_cycle2;
		break;
	}

	return nextTime;
}

//
//-----------------------------------------------------------------------------
// doDisconnectElement
//

void SOCltTaskConnectionMonitor::doDisconnectElement(IN SOCltElement* element)
{
	_ASSERT_PTR(element);
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, element, (_T("disconnect by connection monitor")));
	doDisconnectElementsInList(element->getBranchList());
	doDisconnectElementsInList(element->getLeafList());
	doDisconnectElementsInList(element->getAdditionalList());
	element->setConnectionMonitorStatus(TRUE);
	element->doDisconnect();
}

//-----------------------------------------------------------------------------
// doDisconnectElementsInList
//
void SOCltTaskConnectionMonitor::doDisconnectElementsInList(IN SOCmnList<SOCmnElement>* plist)
{
	if (NULL == plist)
	{
		return;
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
		doDisconnectElement(element);
	}
} // doDisconnectElementsInList

//-----------------------------------------------------------------------------
// doReconnectElement
//
void SOCltTaskConnectionMonitor::doReconnectElement(IN SOCltElement* element)
{
	_ASSERT_PTR(element);

	if (element->getObjectState() != element->getTargetObjectState())
	{
		element->setConnectionMonitorStatus(FALSE);
		HRESULT res = element->doStateTransition(TRUE, SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE);

		if (SUCCEEDED(res))
		{
			doReconnectElementsInList(element->getBranchList());
			doReconnectElementsInList(element->getLeafList());
			doReconnectElementsInList(element->getAdditionalList());
		}
		else
		{
			if (SOSRV_IS_RPC_ERROR(res)) // The RPC server is unavailable.
			{
				element->setConnectionMonitorStatus(TRUE);
			}
		}
	}
} // doReconnectElement

//-----------------------------------------------------------------------------
// doReconnectElementsInList
//
void SOCltTaskConnectionMonitor::doReconnectElementsInList(IN SOCmnList<SOCmnElement>* plist)
{
	if (NULL == plist)
	{
		return;
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

				list.getNext(pos);
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
		doReconnectElement(element);
	}
} // doReconnectElementsInList

#endif // SOCLT
