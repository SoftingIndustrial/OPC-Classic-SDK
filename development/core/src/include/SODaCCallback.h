#ifndef _SODACCALLBACK_H_
#define _SODACCALLBACK_H_

#ifdef SOFEATURE_DCOM

#include "SOCltShutdown.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCAdviseSink                                                            |
//-----------------------------------------------------------------------------

class SODaCComGroupImpl;
class SODaCGroup;

class SODAC_EXPORT SODaCAdviseSink : public IAdviseSink
{
public:
	SODaCAdviseSink(IN SODaCComGroupImpl* group);
	virtual ~SODaCAdviseSink(void);

	// IUnknown interface
	STDMETHODIMP QueryInterface(IN REFIID iid, OUT LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IAdviseSink interface
	STDMETHODIMP_(void) OnDataChange(IN LPFORMATETC pFmt, IN LPSTGMEDIUM pStg);
	STDMETHODIMP_(void) OnViewChange(IN DWORD dwA, IN LONG lindex);
	STDMETHODIMP_(void) OnRename(IN IMoniker* moniker);
	STDMETHODIMP_(void) OnSave(void);
	STDMETHODIMP_(void) OnClose(void);

	// get connection identifiers
	DWORD getReadConnection(void) const;
	DWORD getWriteConnection(void) const;

	// check if the connection where established
	BOOL readConnectionEstablished(void) const;
	BOOL writeConnectionEstablished(void) const;

	// advise
	HRESULT advise(void);

	// unadvise
	HRESULT unadvise(void);

	void setGroup(SODaCComGroupImpl* pGrp);

protected:
	DWORD m_readConnection;     // id of read connection
	DWORD m_writeConnection;    // id of write connection
	BOOL m_establishedRead;     // read connection established ?
	BOOL m_establishedWrite;    // write connection established ?

	// object trace id
	LPCTSTR getObjTraceId(void);

	// handle OnDataChange advise
	void handleAdvise(IN CLIPFORMAT format, IN HGLOBAL memory);

	LONG m_refCount;        // object reference count
	SODaCGroup* m_group;    // group object
	SODaCComGroupImpl* m_pComGroupImpl; // group object
}; // SODaCAdviseSink

inline DWORD SODaCAdviseSink::getReadConnection(void) const
{
	return m_readConnection;
}

inline DWORD SODaCAdviseSink::getWriteConnection(void) const
{
	return m_writeConnection;
}

inline BOOL SODaCAdviseSink::readConnectionEstablished(void) const
{
	return m_establishedRead;
}

inline BOOL SODaCAdviseSink::writeConnectionEstablished(void) const
{
	return m_establishedWrite;
}

#define OPCAdviseSink SODaCAdviseSink




//-----------------------------------------------------------------------------
// SODaCDataCallback                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCDataCallback : public IOPCDataCallback
{
public:
	SODaCDataCallback(IN SODaCComGroupImpl* group);
	virtual ~SODaCDataCallback(void);

	// IUnknown interface
	STDMETHODIMP QueryInterface(IN REFIID iid, OUT LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IOPCDataCallback interface
	STDMETHODIMP OnDataChange(IN DWORD dwTransid, IN OPCHANDLE hGroup,
							  IN HRESULT hrMasterquality, IN HRESULT hrMastererror,
							  IN DWORD dwCount, IN OPCHANDLE* phClientItems,
							  IN VARIANT* pvValues, IN WORD* pwQualities, IN FILETIME* pftTimeStamps,
							  IN HRESULT* pErrors);

	STDMETHODIMP OnReadComplete(IN DWORD dwTransid, IN OPCHANDLE hGroup,
								IN HRESULT hrMasterquality, IN HRESULT hrMastererror,
								IN DWORD dwCount, IN OPCHANDLE* phClientItems,
								IN VARIANT* pvValues, IN WORD* pwQualities, IN FILETIME* pftTimeStamps,
								IN HRESULT* pErrors);

	STDMETHODIMP OnWriteComplete(IN DWORD dwTransid, IN OPCHANDLE hGroup,
								 IN HRESULT hrMastererr, IN DWORD dwCount,
								 IN OPCHANDLE* pClienthandles, IN HRESULT* pErrors);

	STDMETHODIMP OnCancelComplete(IN DWORD dwTransid, IN OPCHANDLE hGroup);

	// get connection identifier
	DWORD getConnection(void) const;

	// check if the connection was established
	BOOL connectionEstablished(void) const;

	// advise connection to server
	HRESULT advise(void);

	// unadvise connection to server
	HRESULT unadvise(BOOL connectionMonitorStatus);

	void setGroup(SODaCComGroupImpl* pGrp);

protected:
	DWORD m_connection;     // id of read, write and cancel connection (OPC V2.0)
	BOOL m_established;     // connection established ?

	HRESULT handleReadAdvise(IN BYTE dataChangeOrRead, IN DWORD dwTransid, IN OPCHANDLE hGroup,
							 IN HRESULT hrMasterquality, IN HRESULT hrMastererror,
							 IN DWORD dwCount, IN OPCHANDLE* phClientItems,
							 IN VARIANT* pvValues, IN WORD* pwQualities, IN FILETIME* pftTimeStamps,
							 IN HRESULT* pErrors);

	// object trace id
	LPCTSTR getObjTraceId(void);

	LONG m_refCount;        // object reference count
	SODaCGroup* m_group;    // group object
	SODaCComGroupImpl* m_pComGroupImpl; // group object
}; // SODaCDataCallback

inline DWORD SODaCDataCallback::getConnection(void) const
{
	return m_connection;
}

inline BOOL SODaCDataCallback::connectionEstablished(void) const
{
	return m_established;
}

#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif
