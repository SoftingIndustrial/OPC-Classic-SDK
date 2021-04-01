#ifndef _SOCLTCOMSERVER_H_
#define _SOCLTCOMSERVER_H_

#ifdef SOFEATURE_DCOM

#pragma pack(push, 4)


#include "SOCltServer.h"

class SODaCComServerImpl;
class SOAeCComServerImpl;

//-----------------------------------------------------------------------------
// SOCltComServerImpl                                                             |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltComServerImpl : virtual public SOCltServerICommunication
{
	friend class SODaCComServerImpl;
	friend class SOAeCComServerImpl;

public:
	SOCltComServerImpl(void);

	virtual void initClt(void);

	// CLSID of this Server
	virtual BOOL setClassID(IN REFCLSID clsid);
	virtual CLSID getClassID(void) const;

	// Execution context of Server
	virtual BOOL setClsctx(IN DWORD clsctx);
	virtual DWORD getClsctx(void) const;

	// IOPCCommon interface
	virtual HRESULT getErrorString(IN HRESULT error, OUT SOCmnString* string);
	virtual HRESULT queryAvailableLCIDs(SOCmnDWORDList& list);

	// IOPCSecurityPrivate interface
	virtual BOOL logon(void);
	virtual BOOL logon(IN LPCTSTR userName, IN LPCTSTR password);
	virtual BOOL logoff(void);
	virtual BOOL isPrivateSecurityAvailable(void);

	virtual void terminate(void);

protected:
	CLSID m_clsid;                      // class ID of the OPC Server
	DWORD m_clsctx;                     // execution context of the OPC server

	CComPtr<IMalloc> m_malloc;                                                  // IMalloc interface

	SOCltInterfaceAccess<IUnknown, &IID_IUnknown> m_unknown;                         // IUnknown interface of server object
	SOCltInterfaceAccess<IOPCCommon, &_ATL_IIDOF(IOPCCommon)> m_common;              // IOPCCommon interface of server object
	SOCltInterfaceAccess<IOPCSecurityPrivate, &_ATL_IIDOF(IOPCSecurityPrivate)> m_secPrivate; // IOPCSecurityPrivate interface of server object

	SOCltShutdown* m_shutdown;  // shutdown event sink

protected:
	virtual ~SOCltComServerImpl(void);
}; // SOCltComServerImpl


inline BOOL SOCltComServerImpl::setClassID(IN REFCLSID clsid)
{
	m_clsid = clsid;
	return TRUE;
}

inline CLSID SOCltComServerImpl::getClassID(void) const
{
	return m_clsid;
}

inline BOOL SOCltComServerImpl::setClsctx(IN DWORD clsctx)
{
	m_clsctx = clsctx;
	return TRUE;
}

inline DWORD SOCltComServerImpl::getClsctx(void) const
{
	return m_clsctx;
}

#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif
