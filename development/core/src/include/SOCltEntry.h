#ifndef _SOCLTENTRY_H_
#define _SOCLTENTRY_H_

#include "SOCltElement.h"
#include "SOCltThread.h"

#pragma pack(push, 4)


#define SOCLT_PROTOCOL_UNKNOWN 0x00
#define SOCLT_PROTOCOL_DCOM    0x01
#define SOCLT_PROTOCOL_SOAP    0x02
#ifdef  SOFEATURE_TUNNEL
#define SOCLT_PROTOCOL_TUNNEL  0x80
#endif

#define SOCLT_SPECIFICATION_UNKNOWN 0x00
#define SOCLT_SPECIFICATION_DA    0x01
#define SOCLT_SPECIFICATION_AE    0x02



//-----------------------------------------------------------------------------
// SOCltCreator                                                               |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltCreator : public SOCmnObject
{
public:
	SOCltCreator();

	virtual SOCltTaskConnectionMonitor* createTaskConnectionMonitor(IN SOCltElement* server, IN DWORD cycle);
	virtual SOCltTaskStateTransition* createTaskStateTransition(IN SOCltElement* pElement, IN BYTE targetState, IN BOOL deep);

protected:
	virtual ~SOCltCreator();
}; // SOCltCreator




//-----------------------------------------------------------------------------
// SOCltEntry                                                                 |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltEntry :
	public SOCltElement
{
public:
	SOCltEntry(IN DWORD objType);

	virtual BOOL initialize(IN DWORD coInit);   // initialize session
	virtual BOOL initializeSecurity(IN DWORD authLevel, IN DWORD impLevel,
									IN DWORD capabilites);
	virtual BOOL terminate(void);                           // terminate session
#ifdef SOFEATURE_DCOM
	DWORD getCoInit(void);                                      // COM init mode
#endif

	virtual LONG release(void);

#ifdef SOOS_WINDOWS
	static void dispatchCallbacks();
	static void setNotifyWindow(IN HWND hwnd);
	static DWORD s_messageCallbackNotify;
#endif

	// object factory for toolkit objects
	virtual SOCltCreator* getCltCreator(void) = 0;

	virtual SOCltWorkerThread* getWorkerThread(void);

	void setCreateWorkThread(BOOL c)
	{
		m_createWorkThread = (BYTE)c;
	}

	static BYTE getProtocolByURL(IN LPCTSTR url);
	static BYTE getSpecificationByURL(IN LPCTSTR url);

protected:
	SOCltWorkerThread* m_pWorkThread;
	BYTE m_createWorkThread;
	BOOL m_terminateLock;           // termination lock
#ifdef SOFEATURE_DCOM
	DWORD m_coInit;                 // COM init mode
	bool  m_bCoUnInit;              // Flag for CoUnInitialize, because m_coInit can be set to zero (COINIT_MULTITHREADED == 0).
#endif

// Overrides
	virtual HRESULT doStateTransition(IN BOOL deep, IN BYTE stateDeep);
	virtual HRESULT doRecurseChildren(SOCmnList<SOCmnElement>* plist, IN BYTE state);

	virtual ~SOCltEntry(void);
}; // SOCltEntry

#ifdef SOFEATURE_DCOM
inline DWORD SOCltEntry::getCoInit(void)
{
	return m_coInit;
}
#endif


//-----------------------------------------------------------------------------
// Global functions and defines                                               |
//-----------------------------------------------------------------------------

SOCLT_EXPORT void setSOCltEntry(IN SOCltEntry* entry);
SOCLT_EXPORT SOCltEntry* getSOCltEntry(void);



//-----------------------------------------------------------------------------
// Demo Version Check
//-----------------------------------------------------------------------------

#ifdef SOFLAG_DEMO_VERSION
extern DWORD SOCLT_EXPORT g_demoStamp;
inline BOOL SOCltCheckDemo()
{
	if (getTimeSpan(g_demoStamp, GetTickCount()) > 5400000)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, _T("DEMO"), (_T("Runtime (90 minutes) of Demo Version elapsed")));
		return TRUE;
	}

	return FALSE;
}
#ifndef SOCLT_COM_CHECK_DEMO
#define SOCLT_COM_CHECK_DEMO SOCltCheckDemo()
#endif
#ifndef SOCLT_COM_CHECK_DEMO_AE
#define SOCLT_COM_CHECK_DEMO_AE SOCltCheckDemo()
#endif
#ifndef SOCLT_TP_CHECK_DEMO
#define SOCLT_TP_CHECK_DEMO SOCltCheckDemo()
#endif
#ifndef SOCLT_XML_CHECK_DEMO
#define SOCLT_XML_CHECK_DEMO SOCltCheckDemo()
#endif
#else
#ifndef SOCLT_COM_CHECK_DEMO
#define SOCLT_COM_CHECK_DEMO (FALSE)
#endif
#ifndef SOCLT_COM_CHECK_DEMO_AE
#define SOCLT_COM_CHECK_DEMO_AE (FALSE)
#endif
#ifndef SOCLT_TP_CHECK_DEMO
#define SOCLT_TP_CHECK_DEMO (FALSE)
#endif
#ifndef SOCLT_XML_CHECK_DEMO
#define SOCLT_XML_CHECK_DEMO (FALSE)
#endif
#endif

#pragma pack(pop)
#endif
