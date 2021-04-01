#ifndef _SODACXMLGROUP_H_
#define _SODACXMLGROUP_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCXmlGroupImpl                                                          |
//-----------------------------------------------------------------------------

class SODaCXmlCall;
class SODaCXmlTaskPolledRefresh;

class SODAC_EXPORT SODaCXmlGroupImpl : virtual public SODaCGroupICommunication
{
	friend class SODaCXmlCall;
	friend class SODaCXmlTaskPolledRefresh;
public:
	SODaCXmlGroupImpl(void);

	virtual void init(SODaCServer* pServer);

	virtual HRESULT updateItemsImpl(IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);

	// synchronous I/O
	virtual HRESULT syncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								 OUT OPTIONAL SOCmnVariant* pValues = NULL, OUT OPTIONAL WORD* pQualities = NULL,
								 OUT OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncWriteImpl(IN DWORD count, IN SODaCItem** itemArray, IN OPTIONAL SOCmnVariant* pValues = NULL,
								  IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);

	// asynchronous I/O
	virtual HRESULT asyncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								  IN OPTIONAL DWORD transactionID = 0, OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncWriteImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD transactionID = 0,
								   IN OPTIONAL SOCmnVariant* pValues = NULL, IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL,
								   OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncRefreshImpl(IN DWORD maxAge, IN OPTIONAL DWORD transactionID = 0,
									 OUT OPTIONAL DWORD* cancelID = NULL);
	virtual HRESULT asyncCancelImpl(IN DWORD cancelID);

	SOCltHTTPConnection* getCyclicHTTPConnection(void)
	{
		m_httpCyclicCon.addRef();
		return m_httpCyclicCon;
	}

	LPCTSTR getServerSubHandle(void)
	{
		return m_serverSubHandle;
	}
	void setServerSubHandle(LPCTSTR subH)
	{
		m_serverSubHandle = subH;
	}

	void setPolledRefreshReply(SOCltHTTPCall* pCall);
	SOCltHTTPCall* getPolledRefreshReply(void);

	virtual HRESULT validateItems(IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors = NULL);

	// group management
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_GRPPAR_ALL_MGT);

protected:
	SOCmnString m_serverSubHandle;
	SOCmnPointer<SOCltHTTPConnection> m_httpCyclicCon;
	DWORD m_taskPolledRefresh;
	SOCmnPointer<SOCltHTTPCall> m_polledRefreshReply;

	virtual ~SODaCXmlGroupImpl(void);

	// do the state transitions
	virtual HRESULT doStateTransitionImpl(BOOL deep, IN BYTE stateDeep);

	HRESULT doSubscribe(IN SOCltHTTPConnection* httpCon);
	HRESULT doSubscriptionCancel(IN SOCltHTTPConnection* httpCon, IN BYTE state);

	HRESULT cancelSubscription(IN BYTE targetState);
	HRESULT establishSubscription(void);

	HRESULT handleSubscriptionPolledRefreshResponse(IN SODaCXmlCall* httpCall);

	HRESULT doSyncRead(IN DWORD count, IN SODaCItem** itemArray, IN DWORD maxAge, IN DWORD transactionId,
					   OUT SOCmnVariant* pValues, OUT WORD* pQualities, OUT SOCmnDateTime* pTimestamps, OUT HRESULT* pErrors);
	HRESULT doSyncWrite(IN DWORD count, IN SODaCItem** itemArray, IN SOCmnVariant* pValues,
						IN WORD* pQualities, IN SOCmnDateTime* pTimestamps, IN DWORD transactionId, OUT HRESULT* pErrorsI);

	virtual HRESULT doDisconnectImpl(void);
}; // SODaCXmlGroupImpl



//-----------------------------------------------------------------------------
// SODaCXmlTaskPolledRefresh                                                  |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlTaskPolledRefresh : public SOCltCyclicTask
{
public:
	SODaCXmlTaskPolledRefresh(IN SODaCGroup* pGroup, IN SODaCXmlGroupImpl* pXmlGroupImpl);

	virtual DWORD getNextTimeToRun(void);
	virtual void execute();

protected:
	virtual ~SODaCXmlTaskPolledRefresh(void);

	SODaCXmlGroupImpl* m_pXmlGroupImpl;
}; // SODaCXmlTaskPolledRefresh



//-----------------------------------------------------------------------------
// SODaCXmlTaskGetStatus                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SOCltTaskXmlGetStatus : public SOCltCyclicTask
{
public:
	SOCltTaskXmlGetStatus(IN SOCltElement* pServer, IN const DWORD& cycle);

	virtual DWORD getNextTimeToRun(void);
	virtual void execute();

protected:
	virtual ~SOCltTaskXmlGetStatus(void);

private:
	DWORD m_period;
}; // SODaCXmlTaskPolledRefresh



//-----------------------------------------------------------------------------
// SODaCXmlGroupDefault                                                       |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlGroupDefault :
	public SODaCGroup,
	virtual public SODaCXmlGroupImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCXmlGroupDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif

