#ifndef _SOAECTPSERVER_H_
#define _SOAECTPSERVER_H_

#ifdef SOFEATURE_TUNNEL
#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeCTPServerImpl                                                                |
//-----------------------------------------------------------------------------

class SOAeCServer;

class SOAEC_EXPORT SOAeCTPServerImpl : virtual public SOAeCServerICommunication
{
	friend class SOAeCServer;
public:
	SOAeCTPServerImpl();

	virtual void connectionFailureImpl(void);

	void setIPPort(WORD port)
	{
		m_ipPort = port;
	}
	WORD getIPPort(void)
	{
		return m_ipPort;
	}

	virtual SOCltTPConnection* getTPConnection(void)
	{
		m_tpCon.addRef();
		return m_tpCon;
	}

	virtual HRESULT getStatus(OUT SOAeCServerStatus& status);

	virtual BOOL logon(IN LPCTSTR userName, IN LPCTSTR password);
	virtual BOOL logoff(void);

protected:
	SOCmnPointer<SOCltTPConnection> m_tpCon;
	WORD m_ipPort;

	virtual ~SOAeCTPServerImpl();

	// do the state transitions
	virtual HRESULT doDisconnectImpl(void);
	virtual HRESULT doStateTransitionImpl(void);

	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

	virtual SOAeCAreaBrowser* getAreaBrowserImpl(void);
	virtual SOAeCEventBrowser* getEventBrowserImpl(void);
	virtual SOCltTask* createAckTask(IN SOCmnList<SOAeCEvent> *events, IN LPCTSTR acknowledgerID, IN LPCTSTR comment);

};

//-----------------------------------------------------------------------------
// SOAeCTPServerDefault                                                         |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPServerDefault :
	public SOAeCServer,
	virtual public SOAeCTPServerImpl,
	virtual public SOCltElementErrorStore
{
public:
	SOAeCTPServerDefault();

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

//-----------------------------------------------------------------------------
// SOAeCTPAckTask                                                          |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPAckTask : public SOCltTask
{
public:
	SOAeCTPAckTask(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events, LPCTSTR acknowledgerID, LPCTSTR comment);
	virtual void execute(void);

	SOCmnString m_ackID;
	SOCmnString m_comment;
	SOCmnList<SOAeCEvent> m_listEvents;

	virtual ~SOAeCTPAckTask();
};


#pragma pack(pop)

#endif  //  SOFEATURE_TUNNEL
#endif
