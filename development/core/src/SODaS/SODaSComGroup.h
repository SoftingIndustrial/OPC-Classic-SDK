#ifndef _SODASCOMGROUP_H_
#define _SODASCOMGROUP_H_

#ifdef SOFEATURE_DCOM

#include <opcda.h>
#include "SOSrvComObject.h"

//-----------------------------------------------------------------------------
// SODaSComGroup                                                              |
//-----------------------------------------------------------------------------

class SODaSComServer;
class SODaSServer;
class SODaSGroup;

class SODaSComGroup :
	public IOPCGroupStateMgt2,
	public IOPCPublicGroupStateMgt,
	public IOPCSyncIO2,
	public IOPCAsyncIO,
	public IOPCAsyncIO3,
	public IOPCItemMgt,
	public IOPCItemDeadbandMgt,
	public IDataObject,
	public IConnectionPointContainerImpl<SODaSComGroup>,
	public IConnectionPointImpl<SODaSComGroup, &IID_IOPCDataCallback, CComDynamicUnkArray>,
	public CComObjectRoot,
	public SOSrvComBase
{
	friend SODaSComServer;
	friend SODaSServer;
public:
	SODaSComGroup();
	~SODaSComGroup();

	BEGIN_COM_MAP(SODaSComGroup)
	COM_INTERFACE_ENTRY(IOPCGroupStateMgt)
	COM_INTERFACE_ENTRY(IOPCGroupStateMgt2)
	COM_INTERFACE_ENTRY(IOPCPublicGroupStateMgt)
	COM_INTERFACE_ENTRY(IOPCSyncIO)
	COM_INTERFACE_ENTRY(IOPCSyncIO2)
	COM_INTERFACE_ENTRY(IOPCAsyncIO)
	COM_INTERFACE_ENTRY(IOPCAsyncIO2)
	COM_INTERFACE_ENTRY(IOPCAsyncIO3)
	COM_INTERFACE_ENTRY(IOPCItemMgt)
	COM_INTERFACE_ENTRY(IOPCItemDeadbandMgt)
	COM_INTERFACE_ENTRY(IDataObject)
#if _MSC_VER >= 1200
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#else
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
#endif
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SODaSComGroup)
	CONNECTION_POINT_ENTRY(IID_IOPCDataCallback)
	END_CONNECTION_POINT_MAP()

	// IOPCGroupStateMgt interface
	STDMETHODIMP GetState(OUT DWORD* pUpdateRate, OUT BOOL* pActive,
						  OUT LPWSTR* ppName, OUT LONG* pTimeBias, OUT FLOAT* pDeadband,
						  OUT DWORD* pLCID, OUT OPCHANDLE* phClientGroup,
						  OUT OPCHANDLE* phServerGroup);

	STDMETHODIMP SetState(IN DWORD* pRequestedUpdateRate,
						  OUT DWORD* pRevisedUpdateRate, IN BOOL* pActive, IN LONG* pTimeBias,
						  IN FLOAT* pPercentDeadband, IN DWORD* pLCID,
						  IN OPCHANDLE* phClientGroup);

	STDMETHODIMP SetName(IN LPCWSTR szName);

	STDMETHODIMP CloneGroup(IN LPCWSTR szName, IN REFIID riid,
							OUT LPUNKNOWN* ppUnk);

	// IOPCGroupStateMgt2 interface
	STDMETHODIMP SetKeepAlive(IN DWORD keepAliveTime, OUT DWORD* pRevisedKeepAliveTime);

	STDMETHODIMP GetKeepAlive(OUT DWORD* pKeepAliveTime);

	// IOPCPublicGroupStateMgt interface
	STDMETHODIMP GetState(OUT BOOL* pPublic);

	STDMETHODIMP MoveToPublic(void);

	// IOPCItemMgt interface
	STDMETHODIMP AddItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray,
						  OUT OPCITEMRESULT** ppAddResults, OUT HRESULT** ppErrors);

	STDMETHODIMP ValidateItems(IN DWORD dwNumItems, IN OPCITEMDEF* pItemArray,
							   IN BOOL bBlobUpdate, OUT OPCITEMRESULT** ppValidationResults,
							   OUT HRESULT** ppErrors);

	STDMETHODIMP RemoveItems(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
							 OUT HRESULT** ppErrors);

	STDMETHODIMP SetActiveState(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
								IN BOOL bActive, OUT HRESULT** ppErrors);

	STDMETHODIMP SetClientHandles(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
								  IN OPCHANDLE* phClient, OUT HRESULT** ppErrors);

	STDMETHODIMP SetDatatypes(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
							  IN VARTYPE* pRequestedDatatypes, OUT HRESULT** ppErrors);

	STDMETHODIMP CreateEnumerator(IN REFIID riid, OUT LPUNKNOWN* ppUnk);

	// IOPCItemDeadbandMgt interface
	STDMETHODIMP SetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer,
								 IN FLOAT* pPercentDeadband, OUT HRESULT** ppErrors);

	STDMETHODIMP GetItemDeadband(IN DWORD count, IN OPCHANDLE* phServer,
								 OUT FLOAT** ppPercentDeadband, OUT HRESULT** ppErrors);

	STDMETHODIMP ClearItemDeadband(IN DWORD count, IN OPCHANDLE* phServer,
								   OUT HRESULT** ppErrors);

	// IOPCSyncIO interface
	STDMETHODIMP Read(IN OPCDATASOURCE dwSource, IN DWORD dwNumItems,
					  IN OPCHANDLE* phServer, OUT OPCITEMSTATE** ppItemValues,
					  OUT HRESULT** ppErrors);

	STDMETHODIMP Write(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
					   IN VARIANT* pItemValues, OUT HRESULT** ppErrors);

	// IOPCSyncIO2 interface
	STDMETHODIMP ReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer, IN DWORD* pMaxAge,
							OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps,
							OUT HRESULT** ppErrors);

	STDMETHODIMP WriteVQT(IN DWORD count, IN OPCHANDLE* phServer,
						  IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);

	// IOPCAsyncIO interface
	STDMETHODIMP Read(IN DWORD dwConnection, IN OPCDATASOURCE dwSource,
					  IN DWORD dwNumItems, IN OPCHANDLE* phServer,
					  OUT DWORD* pTransactionID, OUT HRESULT** ppErrors);

	STDMETHODIMP Write(IN DWORD dwConnection, IN DWORD dwNumItems,
					   IN OPCHANDLE* phServer, IN VARIANT* pItemValues,
					   IN DWORD* pTransactionID, OUT HRESULT** ppErrors);

	STDMETHODIMP Refresh(IN DWORD dwConnection, IN OPCDATASOURCE dwSource,
						 OUT DWORD* pTransactionID);

	STDMETHODIMP Cancel(IN DWORD dwTransactionID);

	// IOPCAsyncIO2 interface
	STDMETHODIMP Read(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
					  IN DWORD dwTransactionID, OUT DWORD* pdwCancelID,
					  OUT HRESULT** ppErrors);

	STDMETHODIMP Write(IN DWORD dwNumItems, IN OPCHANDLE* phServer,
					   IN VARIANT* pItemValues, IN DWORD dwTransactionID,
					   OUT DWORD* pdwCancelID, OUT HRESULT** ppErrors);

	STDMETHODIMP Refresh2(IN OPCDATASOURCE dwSource, IN DWORD dwTransactionID,
						  OUT DWORD* pdwCancelID);

	STDMETHODIMP Cancel2(IN DWORD dwCancelID);

	STDMETHODIMP SetEnable(IN BOOL bEnable);

	STDMETHODIMP GetEnable(OUT BOOL* pbEnable);

	// IOPCAsyncIO3 interface
	STDMETHODIMP ReadMaxAge(IN DWORD count, IN OPCHANDLE* phServer,
							IN DWORD* pMaxAge, IN DWORD transactionID, OUT DWORD* pCancelID,
							OUT HRESULT** ppErrors);

	STDMETHODIMP WriteVQT(IN DWORD count, IN OPCHANDLE* phServer,
						  IN OPCITEMVQT* pItemVQT, IN DWORD transactionID,
						  OUT DWORD* pCancelID, OUT HRESULT** ppErrors);

	STDMETHODIMP RefreshMaxAge(IN DWORD maxAge, IN DWORD transactionID,
							   OUT DWORD* pCancelID);

	// IDataObject interface
	STDMETHODIMP DAdvise(IN FORMATETC* format, IN DWORD advf,
						 IN IAdviseSink* pAdvSink, OUT DWORD* pdwConnection);

	STDMETHODIMP DUnadvise(IN DWORD dwConnection);

	STDMETHODIMP EnumDAdvise(OUT IEnumSTATDATA** ppenumAdvise);

	STDMETHODIMP GetData(IN FORMATETC* formatIn, OUT STGMEDIUM* medium);

	STDMETHODIMP QueryGetData(IN FORMATETC* format);

	STDMETHODIMP GetDataHere(IN FORMATETC* format, OUT STGMEDIUM* medium);

	STDMETHODIMP GetCanonicalFormatEtc(IN FORMATETC* formatIn,
									   OUT FORMATETC* formatOut);

	STDMETHODIMP SetData(IN FORMATETC* format, IN STGMEDIUM* medium,
						 OUT BOOL fRelease);

	STDMETHODIMP EnumFormatEtc(IN DWORD dwDirection,
							   OUT IEnumFORMATETC** ppenumFormatEtc);

	// IConnectionPoint
	STDMETHODIMP Advise(IN IUnknown* pUnk, OUT DWORD* pdwCookie);
	STDMETHODIMP Unadvise(IN DWORD dwCookie);
	STDMETHODIMP EnumConnections(IN IEnumConnections** ppEnum);

	// initialize group object
	void init(SODaSComServer* server, SODaSGroup* group);

	// object trace id
	LPCTSTR getObjTraceId(void);
	BOOL setObjTraceId(IN LPCTSTR traceId);

protected:
	SOCmnString m_objectTraceId;    // object trace id string
	SODaSComServer* m_server;       // server object
	SODaSGroup* m_me;           // generic group object

	// handle common interface function parameters
	HRESULT itemArrayCheckAndAlloc(IN DWORD dwNumItems, IN void* p1, IN void* p2,
								   OUT void** p3, OUT HRESULT** ppErrors);

	virtual HRESULT queryInterface(REFIID iid, void** object);
	virtual BOOL lastExternalReleaseWithInternalRefs(void);

	virtual void lockMe(void);
	virtual void unlockMe(void);
	virtual void disuniteMe(void);

	void releaseComServer(void);
}; // SODaSComGroup

typedef SOSrvComObject<SODaSComGroup> OPCGroupObj;

#endif // SOFEATURE_DCOM

#endif
