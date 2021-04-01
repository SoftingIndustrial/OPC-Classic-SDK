#ifndef _SOSRVSERVER_H_
#define _SOSRVSERVER_H_

#include "SOSrvXML.h"

#pragma pack(push, 4)


#define SOSRVSERVER_SPECID_DA 0x01FF0000
#define SOSRVSERVER_SPECID_AE 0x02FF0000

//-----------------------------------------------------------------------------
// SOSrvServer                                                                |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvServer :
	public SOCmnElement
{
public:
	SOSrvServer(IN DWORD objType, IN OPTIONAL BYTE serverInstance = 0);

	// special release method
	virtual LONG release(void);

	// locale Id
	DWORD getLCID(void) const;
#ifdef SOFEATURE_SOAP
	BOOL setReqLocaleID(LPCTSTR reqLocId);
#endif

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// client name
	SOCmnString getClientName(void) const;

	// client ip
	SOCmnString getClientIp(void) const;

	// client remote
	unsigned char getClientRemote(void) const;

	// IOPCCommon methods
	virtual void queryAvailableLCIDs(OUT SOCmnDWORDList& lcidList);

	virtual HRESULT getErrorString(IN HRESULT error, OUT SOCmnString& string);
	static HRESULT doGetErrorString(IN HRESULT error, IN DWORD specID,
									IN DWORD lcid, OUT SOCmnString& string);
	virtual HRESULT getErrorString(IN HRESULT error, IN DWORD lcid,
								   OUT SOCmnString& string);
	virtual LPCTSTR getErrorCodeSZ(IN DWORD error, OUT SOCmnString& errCodeSZ);

	// start the sending of the shutdown request to the OPC client
#ifdef SOFEATURE_DCOM
	virtual HRESULT startShutdownRequest(IN LPCTSTR reason,
										 IN OPTIONAL HANDLE event = INVALID_HANDLE_VALUE) = 0;

	// send a shutdown request to the OPC client
	virtual HRESULT sendShutdownRequest(IN LPCTSTR reason) = 0;

	// check if connection to OPC client is OK
	virtual BOOL checkClientConnection(OPTIONAL IN BOOL checkAllIFs = FALSE) = 0;
	// disconnect the OPC client belonging to the server object -> deletes the server object
	virtual void disconnectClient(void) = 0;
#endif

	// standard string filter implementations
	static BOOL stdMatchStringFilter(IN LPCTSTR string, IN LPCTSTR filter);
	static BOOL opcMatchStringFilter(IN LPCTSTR string, IN LPCTSTR filter, IN BOOL caseSensitive);
	virtual BOOL includesFilterChars(IN LPCTSTR string);

	// signal work event of working thread
	BOOL signalWorkEvent(void);

	SOCmnEventPointer getWorkEvent(void);
	// end event
	SOCmnEventPointer getEndEvent(void);
	void stopWorkThread(void)
	{
		m_workThread.stop();
	}

	// locale Id
	virtual BOOL supportLCID(IN DWORD lcid);

	// working thread main routine
	DWORD workThread(void);

	// server instance
	BYTE getServerInstance(void)
	{
		return m_serverInstance;
	}

#ifdef SOOS_FEATURE_DCOM
	virtual void getCLSID(OUT CLSID& classID) = 0;
#endif

	// locale Id
	virtual BOOL setLCID(IN DWORD lcid);

	// client name
	virtual BOOL setClientName(IN LPCTSTR clientName);

	// client ip
	virtual BOOL setClientIp(IN LPCTSTR clientIp);

	// client remote
	virtual BOOL setClientRemote(IN unsigned char clientRemote);

#ifdef SOFEATURE_WATCH
	// server watch object
	virtual SOSrvWatch* getWatchObject(void);
	// server trace object
	virtual SOSrvTrace* getTraceObject(void);
#endif

#ifdef SOFEATURE_SOAP
	SOCmnList<SOSrvXmlMethod> *getXmlMethodList(void)
	{
		return &m_xmlMethodList;
	}
	SOSrvHTTPReceiver* getHTTPReceiver(void)
	{
		m_httpReceiver.addRef();
		return m_httpReceiver;
	}
	virtual DWORD insertXMLMethod(BYTE xmlMethodType, IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);
#endif

	BOOL isDCOM(void)
	{
		return(m_me != NULL);
	}

	// security
	virtual BOOL isPrivateSecurityAvailable(void);
	virtual BOOL logon(LPCTSTR userName, LPCTSTR password);
	virtual BOOL logoff(void);
#ifdef SOFEATURE_DCOM
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#endif

	SOCmnSync* getWorkThreadLock(void)
	{
		return &m_workThreadLock;
	}

protected:
	virtual ~SOSrvServer();

	void* m_me;                             // me pointer (to COM object)
	SOCmnSync m_meSync;                     // synchronisation of me pointer
	DWORD m_lcid;                           // locale Id (Windows LCID)
	SOCmnString m_clientName;               // client name
	SOCmnString m_clientIp;                 // client IP (and port)
	unsigned char m_clientRemote;           // flag to specify if the client is remote or not
	mutable SOCmnSync m_clientNameSync;     // lock for client name string, as it may be accessed from multiple threads at once

	DWORD m_workEventId;
	SOCmnThread m_workThread;               // server working thread
	static DWORD _stdcall workThreadProc(void* context);
	SOCmnSync m_workThreadLock;             // server working thread lock
	DWORD m_minRequestLoopTime;             // minimal loop difference of the working thread
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOSrvWatch> m_watchObject; // watch object of the server object
	SOCmnPointer<SOSrvTrace> m_traceObject; // trace object of the server object
#endif
	BYTE m_serverInstance;                  // server instance number
	BYTE m_isDisconnectingClients;          // currently disconnecting clients
#ifdef SOFEATURE_SOAP
	SOCmnList<SOSrvXmlMethod> m_xmlMethodList;
	SOCmnPointer<SOSrvHTTPReceiver> m_httpReceiver;
	SOCmnStringEx m_xmlResHeader;
	SOCmnStringEx m_xmlReplyBase;
	SOCmnStringEx m_xmlResBody;
#endif

#ifdef SOFEATURE_SOAP
	virtual BOOL convertLCIDtoLocaleID(LCID lcid, SOCmnString& localeId);
	virtual BOOL convertLocaleIDtoLCID(LPCTSTR localeId, LCID& lcid);

	void getXMLBuffers(SOCmnStringEx** pHeader, SOCmnStringEx** pReplyBase, SOCmnStringEx** pBody)
	{
		*pHeader = &m_xmlResHeader;
		*pReplyBase = &m_xmlReplyBase;
		*pBody = &m_xmlResBody;
	}
#endif

	// create working thread
	BOOL createWorkThread(void);

	// what type of server object
	virtual DWORD getSpecID(void) = 0;

	// working thread helper
	virtual DWORD doWork(IN DWORD startTime) = 0;

	// callbacks
	virtual void onSetLCID(IN DWORD lcid);
	virtual void onSetClientName(IN LPCTSTR clientName);

#ifdef SOFEATURE_SOAP
	virtual void getXMLQuality(WORD quality, IN BOOL withElement, OUT SOCmnStringEx& xmlQuality);
	virtual WORD getDCOMQuality(IN LPCTSTR quality, IN LPCTSTR limit, IN LPCTSTR vendor);
#endif
}; // SOSrvServer


inline DWORD SOSrvServer::getLCID(void) const
{
	return m_lcid;
}

inline SOCmnString SOSrvServer::getClientName(void) const
{
	m_clientNameSync.lock();
	SOCmnString clientName = m_clientName;
	m_clientNameSync.unlock();
	return clientName;
}

inline SOCmnString SOSrvServer::getClientIp(void) const
{
	return m_clientIp;
}

inline unsigned char SOSrvServer::getClientRemote(void) const
{
	return m_clientRemote;
}

inline BOOL SOSrvServer::signalWorkEvent(void)
{
	return m_workThread.getEvents()->signal(m_workEventId);
}

inline SOCmnEventPointer SOSrvServer::getWorkEvent(void)
{
	return SOCmnEventPointer(m_workThread.getEvents(), m_workEventId);
}

inline SOCmnEventPointer  SOSrvServer::getEndEvent(void)
{
	return SOCmnEventPointer(m_workThread.getEvents(),  m_workThread.getStopEventId());
}

inline HRESULT SOSrvServer::getErrorString(IN HRESULT error, OUT SOCmnString& string)
{
	return getErrorString(error, m_lcid, string);
}




//-----------------------------------------------------------------------------
// SOSrvObjectRoot                                                            |
//-----------------------------------------------------------------------------

class SOSrvEntry;

class SOSRV_EXPORT SOSrvObjectRoot :
	public SOCmnElement,
	virtual public SOCmnElementListBranch
{
public:
	SOSrvObjectRoot();

	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	virtual SOCmnObject* getObjectCmnElementIList(void);

protected:
#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_WATCH
	void addDCOMCnfgWatchData(IN SOSrvEntry* entry, IN SOCmnStringEx& szXML);
#endif
#endif
}; // SOSrvObjectRoot

#pragma pack(pop)
#endif
