#ifndef _SOCLTSHUTDOWN_H_
#define _SOCLTSHUTDOWN_H_

#ifdef SOFEATURE_DCOM

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCltShutdown                                                              |
//-----------------------------------------------------------------------------

class SOCltServer;

class SOCLT_EXPORT SOCltShutdown : public IOPCShutdown
{
public:
	SOCltShutdown(SOCltServer* server);
	virtual ~SOCltShutdown(void);

	// IUnknown interface
	STDMETHODIMP QueryInterface(IN REFIID iid, OUT LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IOPCShutdown interface
	STDMETHODIMP ShutdownRequest(IN LPCWSTR reason);

	HRESULT advise(void);       // advise shutdown event sink
	HRESULT unadvise(BOOL connectionMonitorStatus);     // unadvise shutdown event sink

	void setServer(SOCltServer* pServer);

protected:
	LONG m_refCount;                     // object reference count
	SOCmnPointer<SOCltServer> m_server;  // server object
	DWORD m_cookie;                      // advise token
	BOOL m_established;                  // connection established ?

	// object trace id
	LPCTSTR getObjTraceId(void);
	virtual void onShutdown(IN LPCTSTR reason);
}; // SOCltShutdown


//-----------------------------------------------------------------------------
// SOCltComGuard                                                              |
//-----------------------------------------------------------------------------

class SOCltComGuard
{
public:
	SOCltComGuard(IUnknown* p);

	virtual ~SOCltComGuard();
private:
	IUnknown* m_iUnknownInterface;
}; // SOCltComGuard

#define OPCShutdown SOCltShutdown

#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif
