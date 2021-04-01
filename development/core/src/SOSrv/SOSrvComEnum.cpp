//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvComEnum.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : COM OPC Server object                                       |
//                - SOSrvComEnumString: string enumerator                     |
//                - SOSrvComEnumUnknown: IUnknown enumerator                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_DCOM

#include "SOSrvComEnum.h"
#include "SOCmnTrace.h"


//-----------------------------------------------------------------------------
// SOSrvComEnumString                                                         |
//-----------------------------------------------------------------------------

SOSrvComEnumString::SOSrvComEnumString()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_objectTraceId.Format(_T("(enumS| %8.8X)"), this->getOpcHandle());
	_TRACE_CREATE_COMOBJ();
	m_list.create();
	m_curPos = m_list.getStartPosition();
}

SOSrvComEnumString::~SOSrvComEnumString()
{
	lastCOMObjectActions();
	_TRACE_DELETE_COMOBJ();
}


//-----------------------------------------------------------------------------
// Next (IEnumString)
//
// - get next entries of the enum
//
// returns:
//		S_OK          - entries returned
//		S_FALSE       - entries returned
//		E_INVALIDARG  - invalid pointer
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - out of memory
//
STDMETHODIMP SOSrvComEnumString::Next(
	IN ULONG celt,                  // number of entries to get
	OUT LPOLESTR* rgelt,            // entries
	OUT ULONG* pceltFetched)        // number of returned entries
{
	ULONG i = 0;
	SOCmnString name;
	USES_CONVERSION;

	if (rgelt == NULL)
	{
		return E_POINTER;
	}

	if ((pceltFetched == NULL) && (celt != 1))
	{
		return E_INVALIDARG;
	}

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		name = m_list.getNext(m_curPos);
		rgelt[i] = (LPWSTR) ::CoTaskMemAlloc((name.GetLength() + 1) * sizeof(WCHAR));
		::wcscpy(rgelt[i], T2COLE(name));
	}

	if (pceltFetched)
	{
		*pceltFetched = i;
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Next


//-----------------------------------------------------------------------------
// Skip (IEnumString)
//
// - skip next entries of the enum
//
// returns:
//		S_OK    - number of entries skiped
//		S_FALSE - reached end of list before skipped all requested entries
//
STDMETHODIMP SOSrvComEnumString::Skip(
	IN ULONG celt) // number of entries to skip
{
	ULONG i = 0;

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		m_list.getNext(m_curPos);
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Skip


//-----------------------------------------------------------------------------
// Reset (IEnumString)
//
// - reset current list position to the head of the list
//
// returns:
//		S_OK
//
STDMETHODIMP SOSrvComEnumString::Reset(void)
{
	m_curPos = m_list.getStartPosition();
	return S_OK;
} // Reset


//-----------------------------------------------------------------------------
// Clone (IEnumString)
//
// - clone the enumerator
//
// returns:
//		S_OK         - enum cloned
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOSrvComEnumString::Clone(
	OUT IEnumString** ppEnumString)     // cloned enum
{
	if (!ppEnumString)
	{
		return E_INVALIDARG;
	}

	SOSrvComEnumStringObject* clone;
	SOCmnListPosition pos;
	SOCmnListPosition clonePos;
	BOOL isCurPos;
	SOCMN_ALLOCATE_OBJECT(clone, SOSrvComEnumStringObject)
	pos = m_list.getStartPosition();

	while (pos)
	{
		if (pos != m_curPos)
		{
			isCurPos = FALSE;
		}
		else
		{
			isCurPos = TRUE;
		}

		clonePos = clone->m_list.AddTail(m_list.getNext(pos));

		if (isCurPos)
		{
			clone->m_curPos = clonePos;
		}
	}

	clone->QueryInterface(IID_IEnumString, (void**)ppEnumString);
	return S_OK;
} // Clone




//-----------------------------------------------------------------------------
// SOSrvComEnumUnknown                                                        |
//-----------------------------------------------------------------------------

SOSrvComEnumUnknown::SOSrvComEnumUnknown()
{
	m_objectTraceId.Format(_T("(enumU| %8.8X)"), this->getOpcHandle());
	_TRACE_CREATE_COMOBJ();
	m_list.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_INTERFACE);
	m_curPos = m_list.GetHeadPosition();
}

SOSrvComEnumUnknown::~SOSrvComEnumUnknown()
{
	lastCOMObjectActions();
	_TRACE_DELETE_COMOBJ();
}


//-----------------------------------------------------------------------------
// Next (IEnumUnknown)
//
// - get next entries of the enum
//
// returns:
//		S_OK          - entries returned
//		S_FALSE       - entries returned
//		E_INVALIDARG  - invalid pointer
//		E_INVALIDARG  - invalid argument
//		E_OUTOFMEMORY - out of memory
//
STDMETHODIMP SOSrvComEnumUnknown::Next(
	IN ULONG celt,                  // number of entries to get
	OUT IUnknown** rgelt,           // entries
	OUT ULONG* pceltFetched)        // number of returned entries
{
	ULONG i = 0;

	if (rgelt == NULL)
	{
		return E_POINTER;
	}

	if ((pceltFetched == NULL) && (celt != 1))
	{
		return E_INVALIDARG;
	}

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		rgelt[i] = m_list.GetNext(m_curPos);
		rgelt[i]->AddRef();
	}

	if (pceltFetched)
	{
		*pceltFetched = i;
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Next


//-----------------------------------------------------------------------------
// Skip (IEnumUnknown)
//
// - skip next entries of the enum
//
// returns:
//		S_OK    - number of entries skiped
//		S_FALSE - reached end of list before skipped all requested entries
//
STDMETHODIMP SOSrvComEnumUnknown::Skip(
	IN ULONG celt)  // number of entries to skip
{
	ULONG i = 0;

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		m_list.GetNext(m_curPos);
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Skip


//-----------------------------------------------------------------------------
// Reset (IEnumUnknown)
//
// - reset current list position to the head of the list
//
// returns:
//		S_OK
//
STDMETHODIMP SOSrvComEnumUnknown::Reset(void)
{
	m_curPos = m_list.GetHeadPosition();
	return S_OK;
} // Reset


//-----------------------------------------------------------------------------
// Clone (IEnumString)
//
// - clone the enumerator
//
// returns:
//		S_OK         - enum cloned
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SOSrvComEnumUnknown::Clone(
	OUT IEnumUnknown** ppEnumUnknown)   // cloned enum
{
	IUnknown* unknown;

	if (!ppEnumUnknown)
	{
		return E_INVALIDARG;
	}

	SOSrvComEnumUnknownObject* clone;
	SOCmnListPosition pos;
	SOCmnListPosition clonePos;
	BOOL isCurPos;
	SOCMN_ALLOCATE_OBJECT(clone, SOSrvComEnumUnknownObject)
	pos = m_list.GetHeadPosition();

	while (pos)
	{
		if (pos != m_curPos)
		{
			isCurPos = FALSE;
		}
		else
		{
			isCurPos = TRUE;
		}

		unknown = m_list.GetNext(pos);
		clonePos = clone->m_list.AddTail(unknown);

		if (isCurPos)
		{
			clone->m_curPos = clonePos;
		}
	}

	clone->QueryInterface(IID_IEnumUnknown, (void**)ppEnumUnknown);
	return S_OK;
} // Clone

#endif // SOFEATURE_DCOM

#endif // SOSRV
