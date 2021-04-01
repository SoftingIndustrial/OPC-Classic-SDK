#ifndef _SOAECCALLBACK_H_
#define _SOAECCALLBACK_H_

#include "SOCltShutdown.h"

#pragma pack(push, 4)


// callback errors

//-----------------------------------------------------------------------------
// SOAeCEventSink                                                             |
//-----------------------------------------------------------------------------

class SOAeCSubscription;

class SOAEC_EXPORT SOAeCEventSink : public IOPCEventSink
{
public:
	SOAeCEventSink(IN SOAeCSubscription* subs);

	// IUnknown interface
	STDMETHOD(QueryInterface)(IN REFIID iid, OUT LPVOID* ppInterface);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	// IOPCEventSink interface
	STDMETHOD(OnEvent)(IN OPCHANDLE hClientSubscription,
					   IN BOOL bRefresh,
					   IN BOOL bLastRefresh,
					   IN DWORD dwCount,
					   IN ONEVENTSTRUCT* pEvents);

	// get connection identifiers
	// advise
	HRESULT advise(void);

	// unadvise
	HRESULT unadvise(BOOL connectionMonitorStatus);

	BOOL isAdvised(void);

	DWORD m_dwCookie;

	void setSubscription(SOAeCSubscription* pSub)
	{
		m_subs = pSub;
	}

protected:
	virtual ~SOAeCEventSink();

	BOOL m_established;     // connection established ?

	// object trace id
	LPCTSTR getObjTraceId(void);

	LONG m_refCount;                // object reference count
	SOAeCSubscription* m_subs;  // subscription object
}; // SOAeCEventSink

inline BOOL SOAeCEventSink::isAdvised(void)
{
	return m_established;
}

#pragma pack(pop)
#endif
