#ifndef _SOAESCOMSUBSCRIPTION_H_
#define _SOAESCOMSUBSCRIPTION_H_

#ifdef SOFEATURE_DCOM

#include <opc_ae.h>
#include "SOSrvComObject.h"

//-----------------------------------------------------------------------------
// SOAeSComSubscription                                                       |
//-----------------------------------------------------------------------------

class SOAeSSubscription;
class SOAeSComServer;
class SOAeSServer;

class SOAeSComSubscription :
	public IOPCEventSubscriptionMgt,
	public IConnectionPointContainerImpl<SOAeSComSubscription>,
	public IConnectionPointImpl<SOAeSComSubscription, &IID_IOPCEventSink, CComDynamicUnkArray>,
	public CComObjectRoot,
	public SOSrvComBase
{
	friend SOAeSServer;
public:
	SOAeSComSubscription();
	~SOAeSComSubscription();

	BEGIN_COM_MAP(SOAeSComSubscription)
	COM_INTERFACE_ENTRY(IOPCEventSubscriptionMgt)
#if _MSC_VER >= 1200
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#else
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
#endif
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SOAeSComSubscription)
	CONNECTION_POINT_ENTRY(IID_IOPCEventSink)
	END_CONNECTION_POINT_MAP()

	DECLARE_OPCOBJECT_NOT_AGGREGATABLE(SOAeSComSubscription)

	// IOPCEventSubscriptionMgt interface
	STDMETHODIMP SetFilter(IN DWORD eventType, IN DWORD numCategories,
						   IN DWORD* eventCategoryArray, IN DWORD lowSeverity,
						   IN DWORD highSeverity, IN DWORD numAreas, IN LPWSTR* areaArray,
						   IN DWORD numSources, IN LPWSTR* sourceArray);

	STDMETHODIMP GetFilter(OUT DWORD* eventType, OUT DWORD* numCategories,
						   OUT DWORD** eventCategoryArray, OUT DWORD* lowSeverity,
						   OUT DWORD* highSeverity, OUT DWORD* numAreas,
						   OUT LPWSTR** areaArray, OUT DWORD* numSources,
						   OUT LPWSTR** sourceArray);

	STDMETHODIMP SelectReturnedAttributes(IN DWORD eventCategory,
										  IN DWORD count, IN DWORD* attributeIDs);

	STDMETHODIMP GetReturnedAttributes(IN DWORD eventCategory,
									   OUT DWORD* count, OUT DWORD** attributeIDs);

	STDMETHODIMP Refresh(IN DWORD connection);

	STDMETHODIMP CancelRefresh(IN DWORD connection);

	STDMETHODIMP GetState(OUT BOOL* active, OUT DWORD* bufferTime,
						  OUT DWORD* maxSize, OUT OPCHANDLE* clientSubscription);

	STDMETHODIMP SetState(IN BOOL* active, IN DWORD* bufferTime,
						  IN DWORD* maxSize, IN OPCHANDLE clientSubscription,
						  OUT DWORD* revisedBufferTime, OUT DWORD* revisedMaxSize);

	// IConnectionPoint
	STDMETHODIMP Advise(IN IUnknown* unknown, OUT DWORD* cookie);

	STDMETHODIMP Unadvise(IN DWORD cookie);

	// initialize subscription object
	void init(IN SOAeSComServer* server, IN SOAeSSubscription* subscr);

	// object trace id
	LPCTSTR getObjTraceId(void);

	// advise synchronisation object
	SOCmnMutex* getAdvisesSync(void);

protected:
	SOCmnString m_objectTraceId;    // object trace id string
	SOAeSSubscription* m_me;        // subscription object
	SOAeSComServer* m_server;       // server object
	SOCmnMutex m_advisesSync;       // synchronisation for advises array

	BOOL validConnection(IN DWORD connection);

	// query interface
	virtual HRESULT queryInterface(REFIID iid, void** object);

	// connection from subscription object
	virtual void lockMe(void);
	virtual void unlockMe(void);
	virtual void disuniteMe(void);

	void releaseComServer(void);
}; // SOAeSComSubscription

inline SOCmnMutex* SOAeSComSubscription::getAdvisesSync(void)
{
	return &m_advisesSync;
}

#endif // SOFEATURE_DCOM

#endif
