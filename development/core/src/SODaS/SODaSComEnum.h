#ifndef _SODASCOMENUM_H_
#define _SODASCOMENUM_H_

#ifdef SOFEATURE_DCOM

#include "SOSrvComObject.h"
#include "SODaSItem.h"
#include "SODaSTag.h"
#include <opcda.h>

//-----------------------------------------------------------------------------
// SODaSComEnumItem                                                           |
//-----------------------------------------------------------------------------

class SODaSComEnumItem :
	public IEnumOPCItemAttributes,
	public CComObjectRoot,
	public SOSrvComBase
{
public:
	SODaSComEnumItem();
	~SODaSComEnumItem();

	BEGIN_COM_MAP(SODaSComEnumItem)
	COM_INTERFACE_ENTRY(IEnumOPCItemAttributes)
	END_COM_MAP()

	// IEnumOPCItemAttributes interface
	STDMETHODIMP Next(IN ULONG celt, OUT OPCITEMATTRIBUTES** rgelt,
					  OUT ULONG* pceltFetched);

	STDMETHODIMP Skip(IN ULONG celt);

	STDMETHODIMP Reset(void);

	STDMETHODIMP Clone(OUT IEnumOPCItemAttributes** ppEnumOPCItemAttr);

	LPCTSTR getObjTraceId(void);

	SOCmnList<SODaSItem> m_list;
protected:
	SOCmnListPosition m_curPos;     // current position in list
	SOCmnString m_objectTraceId;    // object trace id string
}; // SODaSComEnumItem

inline LPCTSTR SODaSComEnumItem::getObjTraceId(void)
{
	return m_objectTraceId;
}

typedef SOSrvComObject<SODaSComEnumItem> OPCItemEnumObject;

#endif // SOFEATURE_DCOM

#endif
