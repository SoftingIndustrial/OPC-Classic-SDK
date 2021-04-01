#ifndef _SOAECCOMSERVER_H_
#define _SOAECCOMSERVER_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeCComServerImpl                                                                |
//-----------------------------------------------------------------------------

class SOAeCServer;

class SOAEC_EXPORT SOAeCComServerImpl : virtual public SOAeCServerICommunication
{
	friend class SOAeCServer;
public:
	SOAeCComServerImpl();

	// IOPCEventServer  interface
	virtual HRESULT getStatus(OUT SOAeCServerStatus& status);
	HRESULT getStatus(OUT OPCEVENTSERVERSTATUS* serverStatus, OUT SOCmnString* vendorInfo);
	virtual HRESULT enableConditionByArea(IN SOCmnStringList* areas, IN BOOL enable);
	virtual HRESULT enableConditionByArea(IN LPCTSTR area, IN BOOL enable);
	virtual HRESULT enableConditionBySource(IN SOCmnStringList* sources, IN BOOL enable);
	virtual HRESULT enableConditionBySource(IN LPCTSTR source, IN BOOL enable);
	virtual HRESULT translateToItemIDs(
		IN LPCTSTR source,
		IN DWORD eventCategory,
		IN LPCTSTR conditionName,
		IN LPCTSTR subconditionName,
		IN SOCmnDWORDList* assocAttrIDs,
		OUT SOCmnList<SOAeCTranslateResult>& translateResults);
	virtual HRESULT getConditionState(IN LPCTSTR source, IN LPCTSTR condName,
									  IN OPTIONAL SOCmnDWORDList* eventAttrIDs,
									  OUT SOAeCConditionState& state);
	virtual HRESULT queryAvailableFilters(OUT DWORD* pFilterMask);

	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);

protected:
	SOCltInterfaceAccess<IOPCEventServer, &_ATL_IIDOF(IOPCEventServer)> m_server;         // IOPCServer interface of server object

	virtual ~SOAeCComServerImpl();

	virtual HRESULT doDisconnectImpl(void);

	// do the state transitions
	virtual HRESULT doStateTransitionImpl(void);
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

	virtual SOAeCAreaBrowser* getAreaBrowserImpl(void);
	virtual SOAeCEventBrowser* getEventBrowserImpl(void);

	virtual SOCltTask* createAckTask(IN SOCmnList<SOAeCEvent> *events, IN LPCTSTR acknowledgerID, IN LPCTSTR comment);

	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
};

//-----------------------------------------------------------------------------
// SOAeCComServerDefault                                                         |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCComServerDefault :
	public SOAeCServer,
	virtual public SOCltComServerImpl,
	virtual public SOAeCComServerImpl,
	virtual public SOCltElementErrorStore
{
public:
	SOAeCComServerDefault();

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
};


class SOAeCEvent;

//-----------------------------------------------------------------------------
// SOAeCComAckTask                                                          |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCComAckTask : public SOCltTask
{
public:
	SOAeCComAckTask(SOAeCServer* pServer, IN SOCmnList<SOAeCEvent> *events, LPCTSTR acknowledgerID, LPCTSTR comment);
	virtual void execute(void);

	SOCmnString m_ackID;
	SOCmnString m_comment;
	SOCmnList<SOAeCEvent> m_listEvents;

	virtual ~SOAeCComAckTask();
};


#pragma pack(pop)
#endif
