#ifndef _SOSRVCOMENUM_H_
#define _SOSRVCOMENUM_H_

#ifdef SOFEATURE_DCOM

#include "SOSrvComObject.h"

//-----------------------------------------------------------------------------
// SOSrvComEnumString                                                         |
//-----------------------------------------------------------------------------

class SOSrvComEnumString :
	public IEnumString,
	public CComObjectRoot,
	public SOSrvComBase
{
public:
	SOSrvComEnumString();
	~SOSrvComEnumString();

	BEGIN_COM_MAP(SOSrvComEnumString)
	COM_INTERFACE_ENTRY(IEnumString)
	END_COM_MAP()

	STDMETHODIMP Next(IN ULONG celt, OUT LPOLESTR* rgelt,
					  OUT ULONG* pceltFetched);

	STDMETHODIMP Skip(IN ULONG celt);

	STDMETHODIMP Reset(void);

	STDMETHODIMP Clone(OUT IEnumString** ppEnumString);

	SOCmnStringList m_list;

	LPCTSTR getObjTraceId(void);

protected:
	SOCmnListPosition m_curPos;
	SOCmnString m_objectTraceId;    // object trace id string
}; // IEnumString

inline LPCTSTR SOSrvComEnumString::getObjTraceId(void)
{
	return m_objectTraceId;
}

typedef SOSrvComObject<SOSrvComEnumString> SOSrvComEnumStringObject;




//-----------------------------------------------------------------------------
// SOSrvComEnumUnknown                                                        |
//-----------------------------------------------------------------------------

class SOSrvComEnumUnknown :
	public IEnumUnknown,
	public CComObjectRoot,
	public SOSrvComBase
{
public:
	SOSrvComEnumUnknown();
	~SOSrvComEnumUnknown();

	BEGIN_COM_MAP(SOSrvComEnumUnknown)
	COM_INTERFACE_ENTRY(IEnumUnknown)
	END_COM_MAP()

	STDMETHODIMP Next(IN ULONG celt, OUT IUnknown** rgelt,
					  OUT ULONG* pceltFetched);

	STDMETHODIMP Skip(IN ULONG celt);

	STDMETHODIMP Reset(void);

	STDMETHODIMP Clone(OUT IEnumUnknown** ppEnumUnknown);

	SOCmnList<IUnknown> m_list;

	LPCTSTR getObjTraceId(void);

protected:
	SOCmnListPosition m_curPos;
	SOCmnString m_objectTraceId;    // object trace id string
}; // SOSrvComEnumUnknown

inline LPCTSTR SOSrvComEnumUnknown::getObjTraceId(void)
{
	return m_objectTraceId;
}

typedef SOSrvComObject<SOSrvComEnumUnknown> SOSrvComEnumUnknownObject;

#endif // SOFEATURE_DCOM

#endif
