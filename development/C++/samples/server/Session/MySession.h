#ifndef _MYSESSION_H_
#define _MYSESSION_H_

#include "Da/ServerDaSession.h"
#include "MyDaAddressSpaceElement.h"

#include <algorithm>
#include <iostream>

#ifdef TBC_OS_LINUX
#include "PosixEvents.h"
#endif

using namespace SoftingOPCToolboxServer;

class MySession;

extern std::vector<MySession*> g_sessionList;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_changedEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

//-----------------------------------------------------------------------------
// MySession
//-----------------------------------------------------------------------------
class MySession : public DaSession
{
public:

	MySession(EnumSessionType aType,
			  unsigned long aHandle):
		DaSession(aType, aHandle)
	{
		DateTime time;
		time.now();
		Variant aVariant(s_clientCount);
		m_clientSpecValue = new ValueQT(aVariant, EnumQuality_GOOD, time);
		s_clientCount++;
	}

	virtual ~MySession(void)
	{
		if (m_clientSpecValue != NULL)
		{
			delete m_clientSpecValue;
			m_clientSpecValue = NULL;
		} // end if
	}

	void connectSession(void)
	{
		g_sessionList.push_back(this);
#ifdef TBC_OS_WINDOWS
		SetEvent(g_changedEvent);
#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
	} // end connectSession

	void disconnectSession(void)
	{
		std::vector<MySession*>::iterator result = std::find(g_sessionList.begin(), g_sessionList.end(), this);

		if (result != g_sessionList.end())
		{
			g_sessionList.erase(result);
		} // end if

#ifdef TBC_OS_WINDOWS
		SetEvent(g_changedEvent);
#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
	} // end disconnectSession

	unsigned long getHandle()
	{
		return getObjectHandle();
	}   //  end getHandle

	//  Events
	virtual void handleConnected()
	{
		_tprintf(_T("Connected session: [%d]\n"), (int)getHandle());
	}

	virtual void handleDisconnected()
	{
		_tprintf(_T("Disconnected session: [%d]\n"), (int)getHandle());
	}

	virtual void handleClientNameChanged()
	{
		_tprintf(_T("Session [%d] client name changed to [%s]\n"), (int)getHandle(), getClientName().c_str());
	}

	virtual long logoff(void)
	{
		if (this == s_controlingSession)
		{
			s_controlingSession = NULL;
		} // end if

#ifdef TBC_OS_WINDOWS
		SetEvent(g_changedEvent);
#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
		return EnumResultCode_S_OK;
	} // end logoff

	virtual long logon(
		tstring& UserName,
		tstring& Password)
	{
		long ret = EnumResultCode_E_ACCESSDENIED;

		if (UserName == _T("OPC") && Password == _T("opc"))
		{
			if ((this == s_controlingSession) || (s_controlingSession == NULL))
			{
				s_controlingSession = this;
				ret = EnumResultCode_S_OK;
			} // end if
		} // end if

#ifdef TBC_OS_WINDOWS
		SetEvent(g_changedEvent);
#endif
#ifdef TBC_OS_LINUX
		g_events.signal(1);
#endif
		return ret;
	} // end logon

	virtual long getCacheValue(ValueQT& pValue)
	{
		pValue = *m_clientSpecValue;
		return EnumResultCode_S_OK;
	}   //  end getCacheValue

public:
	static MySession* s_controlingSession;

private:
	static  unsigned long s_clientCount;
	ValueQT* m_clientSpecValue;

};  // end MySession

//MySession* MySession::s_controlingSession = NULL;
//unsigned long MySession::s_clientCount = 0;

#endif
