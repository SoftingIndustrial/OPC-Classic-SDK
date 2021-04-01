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
//                           OPC Toolkit - SODaC                              |
//                                                                            |
//  Filename    : SODaCCallback.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : COM Callback object classes                                 |
//                - SODaCAdviseSink: OPC V1.0 sink for asynchronous advises   |
//                - SODaCDataCallback: OPC V2.0 sink for asynchronous advises |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_DCOM

#include "opcerror.h"
#include "SODaCCallback.h"
#include "SODaCGroup.h"
#include "SODaCItem.h"
#include "SODaCServer.h"

//-----------------------------------------------------------------------------
// SODaCAdviseSink                                                            |
//-----------------------------------------------------------------------------

#ifdef SOFEATURE_DA10
// OPC DA V1.0 clipboard formats
const UINT OPCSTMFORMATDATA_C = ::RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
const UINT OPCSTMFORMATDATATIME_C = ::RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
const UINT OPCSTMFORMATWRITECOMPLETE_C = ::RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));
#else
const UINT OPCSTMFORMATDATA_C = 0;
const UINT OPCSTMFORMATDATATIME_C = 0;
const UINT OPCSTMFORMATWRITECOMPLETE_C = 0;
#endif


SODaCAdviseSink::SODaCAdviseSink(
	IN SODaCComGroupImpl* group)    // group object
{
	m_group = group->getObjectDaCGroup();
	m_pComGroupImpl = group;
	m_refCount = 0;
	m_writeConnection = 0;
	m_readConnection = 0;
	m_establishedRead = FALSE;
	m_establishedWrite = FALSE;
	AddRef();
}

SODaCAdviseSink::~SODaCAdviseSink()
{
}

//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaCAdviseSink::getObjTraceId(void)
{
	SOCltComGuard gurad(this);
	return m_group->getObjTraceId();
} // getObjTraceId


//-----------------------------------------------------------------------------
// AddRef
//
// - add reference to count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SODaCAdviseSink::AddRef(void)
{
	return interlockedIncrement(&m_refCount);
} // AddRef


//-----------------------------------------------------------------------------
// Release
//
// - release one reference from count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SODaCAdviseSink::Release(void)
{
	ULONG currentCount = interlockedDecrement(&m_refCount);

	if (currentCount == 0)
	{
		delete this;
	}

	return currentCount;
} // Release


//-----------------------------------------------------------------------------
// QueryInterface
//
// - query for an interface of the object
//
// returns:
//		S_OK          - interface pointer returned
//		E_NOINTERFACE - interface not supported
//
STDMETHODIMP SODaCAdviseSink::QueryInterface(
	IN REFIID iid,          // interface id
	IN LPVOID* ppInterface) // interface pointer
{
	SOCltComGuard gurad(this);

	if (ppInterface == NULL)
	{
		return E_INVALIDARG;
	}

	if (iid == IID_IUnknown)
	{
		*ppInterface = (IUnknown*) this;
	}
	else if (iid == IID_IAdviseSink)
	{
		*ppInterface = (IUnknown*) this;
	}
	else
	{
		*ppInterface = NULL;
	}

	if (*ppInterface == NULL)
	{
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
} // QueryInterface


//-----------------------------------------------------------------------------
// OnDataChange
//
// - OPC advise callback
//
STDMETHODIMP_(void) SODaCAdviseSink::OnDataChange(
	IN LPFORMATETC format,  // data format
	IN LPSTGMEDIUM medium)  // transport medium
{
	SOCltComGuard gurad(this);

#ifdef _WIN64
	// NOTE: OPCDA1.0 uses a memory block for transferring data between client and server.
	//       The memory contains structures with platform-dependent members.
	//       Sizeof(VARIANT) is 16 for 32-bit systems and 24 for 64-bit systems.
	//       Since OPC DA v1.0a dates back in 1997, it is a common agreement that OPCDA1.0 servers are 32-bit only.
	//       Remove this at your own risk!
	if (TRUE)
	{
		// avoid warnings
		return;
	}
#endif

	if ((!m_pComGroupImpl) || (!m_group))
	{
		return;
	}

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IDataObject::OnDataChange")));
	SOCMN_TRY
	{
		if (SOCLT_COM_CHECK_DEMO)
		{
			return;
		}

		USES_CONVERSION;

		// check format
		if ((format->cfFormat != OPCSTMFORMATDATA_C) &&
		(format->cfFormat != OPCSTMFORMATDATATIME_C) &&
		(format->cfFormat != OPCSTMFORMATWRITECOMPLETE_C))
		{
			ReleaseStgMedium(medium);
			return;
		}

		// check transport medium type
		if (format->tymed != TYMED_HGLOBAL)
		{
			ReleaseStgMedium(medium);
			return;
		}

		// check transport medium value
		if (medium->hGlobal == NULL)
		{
			return;
		}


		// handle advise
		handleAdvise(format->cfFormat, medium->hGlobal);

		ReleaseStgMedium(medium);

	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IDataObject::OnDataChange")));
		return;
	}
} // OnDataChange


//-----------------------------------------------------------------------------
// handleAdvise
//
// - handle OPC advise
//
void SODaCAdviseSink::handleAdvise(
	IN CLIPFORMAT format,   // format id
	IN HGLOBAL memory)      // memory
{
	SOCmnPointer<SODaCItem> item;
	BYTE* mem = (BYTE*)GlobalLock(memory);
	BYTE* actPtr = mem;
	DWORD i;

	if (mem == NULL)
	{
		return;
	}

	if (format != OPCSTMFORMATWRITECOMPLETE_C)
	{
		// read
		m_pComGroupImpl->onDataChangeBegin();
		OPCGROUPHEADER* groupHeader;
		OPCITEMHEADER1* itemHeader;
		DWORD valueOffset;
		DWORD groupHeaderSize;
		VARIANT* value;
		SOCmnVariant val;
		// for advise call
		HRESULT masterQuality = S_OK;
		HRESULT masterError = S_OK;
		DWORD itemCnt = 0;
		SODaCItem** ppItems = NULL;
		SOCmnVariant* pValues = NULL;
		WORD* pQualities = NULL;
		SOCmnDateTime* pTimestamps = NULL;
		HRESULT* pErrors = NULL;
		// group header
		groupHeader = (OPCGROUPHEADER*)actPtr;
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T(" IN : dwItemCount: %lu ; hClientGroup: 0x%8.8X ; hrStatus: 0x%X"), groupHeader->dwItemCount, groupHeader->hClientGroup, groupHeader->hrStatus));
		actPtr += sizeof(OPCGROUPHEADER);
		itemCnt = groupHeader->dwItemCount;
		ppItems = (SODaCItem**) malloc(itemCnt * sizeof(SODaCItem*));
		pValues = new SOCmnVariant[itemCnt];
		pQualities = new WORD[itemCnt];
		pTimestamps = new SOCmnDateTime[itemCnt];
		pErrors = new HRESULT[itemCnt];
		masterError = groupHeader->hrStatus;
		itemCnt = 0;

		if (format == OPCSTMFORMATDATA_C)
		{
			valueOffset = sizeof(OPCGROUPHEADER) + (sizeof(OPCITEMHEADER2) * groupHeader->dwItemCount);
			groupHeaderSize = sizeof(OPCITEMHEADER2);
		}
		else
		{
			valueOffset = sizeof(OPCGROUPHEADER) + (sizeof(OPCITEMHEADER1) * groupHeader->dwItemCount);
			groupHeaderSize = sizeof(OPCITEMHEADER1);
		}

		// item header and data
		for (i = 0; i < groupHeader->dwItemCount; i++)
		{
			itemHeader = (OPCITEMHEADER1*)actPtr;
			item = m_group->getItemByClientHandle(itemHeader->hClient);

			if (item.isNotNull())
			{
				item->addRef();
				ppItems[itemCnt] = (SODaCItem*)item;
				value = (VARIANT*)(mem + itemHeader->dwValueOffset);

				// correct BSTR
				if (value->vt == VT_BSTR)
				{
					value->bstrVal = (BSTR)(((DWORD*)(value + 1)) + 1);
				}

				// correct ARRAY
				if ((value->vt & VT_ARRAY) == VT_ARRAY)
				{
					value->parray = (SAFEARRAY*)(value + 1);
					value->parray->pvData = (void*)(value->parray + 1);

					if (value->vt == (VT_BSTR | VT_ARRAY))
					{
						BSTR* bstr = (BSTR*)value->parray->pvData;
						DWORD* pBstrCount;
						BYTE* pActBArray = ((BYTE*)value->parray->pvData) + (value->parray->cbElements * value->parray->rgsabound[0].cElements);
						ULONG j;
						BSTR pBstr;

						for (j = 0; j < value->parray->rgsabound[0].cElements; ++j)
						{
							pBstrCount = (DWORD*)(pActBArray);
							pBstr = (BSTR)(pBstrCount + 1);
							bstr[j] = pBstr;
							pActBArray += *pBstrCount + 6;
						}
					}
				}

				val = value;

				if (format == OPCSTMFORMATDATATIME_C)
				{
					_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C)
					{
						SOCmnString v, q, t;
						_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; %s %s %s"), i, (LPCTSTR)item->getItemID(), variantWithType2string(&v, &val), quality2string(&q, itemHeader->wQuality), filetime2string(&t, &itemHeader->ftTimeStampItem)));
					}
					SOCmnDateTime timestamp(&itemHeader->ftTimeStampItem);
					item->setRead(S_OK, &val, itemHeader->wQuality, &timestamp, groupHeader->dwTransactionID);
					pTimestamps[itemCnt] = timestamp;
				}
				else
				{
					_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C)
					{
						SOCmnString v, q, t;
						_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; %s %s"), i, (LPCTSTR)item->getItemID(), variantWithType2string(&v, &val), quality2string(&q, itemHeader->wQuality)));
					}
					item->setRead(S_OK, &val, itemHeader->wQuality, NULL, groupHeader->dwTransactionID);
					pTimestamps[itemCnt].clear();
				}

				pValues[itemCnt].initFrom(val);
				pQualities[itemCnt] = itemHeader->wQuality;
				pErrors[itemCnt] = S_OK;
				itemCnt++;
				val.clear();
			}
			else
			{
				_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, (DWORD)itemHeader->hClient));
				m_group->setLastError(OPC_E_INVALIDHANDLE, SODAC_ERR_ADVISE_DATACALLBACK);
				ppItems[i] = NULL;
			}

			actPtr += groupHeaderSize;
		}

		if (m_group)
		{
			m_group->onReadAdvise((groupHeader->dwTransactionID == 0), groupHeader->dwTransactionID, masterQuality, masterError, itemCnt, ppItems, pValues, pQualities, pTimestamps, pErrors);
		}

		if (ppItems)
		{
			for (i = 0; i < itemCnt; i++)
			{
				ppItems[i]->release();
			}

			free(ppItems);
		}

		if (pValues)
		{
			delete[] pValues;
		}

		if (pQualities)
		{
			delete[] pQualities;
		}

		if (pTimestamps)
		{
			delete[] pTimestamps;
		}

		if (pErrors)
		{
			delete[] pErrors;
		}

		m_pComGroupImpl->onReadAdvise(groupHeader->dwTransactionID, mem);
		m_pComGroupImpl->onDataChangeEnd();
	}
	else
	{
		// write
		OPCGROUPHEADERWRITE* groupHeader;
		OPCITEMHEADERWRITE* itemHeader;
		// for advise call
		DWORD itemCnt = 0;
		SODaCItem** ppItems = NULL;
		HRESULT* pErrors = NULL;
		HRESULT masterError = S_OK;
		// group header
		groupHeader = (OPCGROUPHEADERWRITE*)actPtr;
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T(" IN : dwItemCount: %lu ; hClientGroup: 0x%8.8X ; hrStatus 0x%X"), groupHeader->dwItemCount, groupHeader->hClientGroup, groupHeader->hrStatus));
		actPtr += sizeof(OPCGROUPHEADERWRITE);
		itemCnt = groupHeader->dwItemCount;
		ppItems = (SODaCItem**) malloc(itemCnt * sizeof(SODaCItem*));
		pErrors = new HRESULT[itemCnt];
		masterError = groupHeader->hrStatus;
		itemCnt = 0;

		// item header
		for (i = 0; i < groupHeader->dwItemCount; i++)
		{
			itemHeader = (OPCITEMHEADERWRITE*)actPtr;
			item = m_group->getItemByClientHandle(itemHeader->hClient);

			if (item.isNotNull())
			{
				item->addRef();
				ppItems[itemCnt] = (SODaCItem*)item;
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; 0x%8.8X"), i, (LPCTSTR)item->getItemID(), itemHeader->dwError));
				item->setWrite(itemHeader->dwError, groupHeader->dwTransactionID);
				pErrors[itemCnt] = itemHeader->dwError;
				itemCnt++;
			}
			else
			{
				_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, (DWORD)itemHeader->hClient));
				m_group->setLastError(OPC_E_INVALIDHANDLE, SODAC_ERR_ADVISE_DATACALLBACK);
				ppItems[i] = NULL;
			}

			actPtr += sizeof(OPCITEMHEADERWRITE);
		}

		if (m_group)
		{
			m_group->onWriteAdvise(groupHeader->dwTransactionID, masterError, itemCnt, ppItems, pErrors);
		}

		if (ppItems)
		{
			for (i = 0; i < itemCnt; i++)
			{
				ppItems[i]->release();
			}

			free(ppItems);
		}

		if (pErrors)
		{
			delete[] pErrors;
		}

		m_pComGroupImpl->onWriteAdvise(groupHeader->dwTransactionID, mem);
	}

	GlobalUnlock(memory);
} // handleAdvise


// not supported callbacks

STDMETHODIMP_(void) SODaCAdviseSink::OnViewChange(IN DWORD /* dwAspect */, IN LONG /* lindex */)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("AdviseSink called OnViewChange")));
}

STDMETHODIMP_(void) SODaCAdviseSink::OnSave(void)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("AdviseSink called OnSave")));
}

STDMETHODIMP_(void) SODaCAdviseSink::OnRename(IN IMoniker* /* moniker */)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("AdviseSink called OnRename")));
}

STDMETHODIMP_(void) SODaCAdviseSink::OnClose(void)
{
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("AdviseSink called OnClose")));
}


//-----------------------------------------------------------------------------
// advise
//
// - connect the advise sink of the client to the server
//
// returns:
//		S_OK                  - operation succeeded
//      E_FAIL                - operation failed
//      E_INVALIDARG          - invalid argument
//      E_OUTOFMEMORY         - not enough memory
//      E_NOINTERFACE         - interface not supported
//      CONNECT_E_ADVISELIMIT - reached maximal advise limit of server
//
HRESULT SODaCAdviseSink::advise(void)
{
	CComPtr<IDataObject> dataObject;
	FORMATETC format;
	HRESULT res = S_OK;
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = NULL;
	format.tymed = TYMED_HGLOBAL;
	format.lindex = -1;

#ifdef _WIN64
	// NOTE: OPCDA1.0 uses a memory block for transferring data between client and server.
	//       The memory contains structures with platform-dependent members.
	//       Sizeof(VARIANT) is 16 for 32-bit systems and 24 for 64-bit systems.
	//       Since OPC DA v1.0a dates back in 1997, it is a common agreement that OPCDA1.0 servers are 32-bit only.
	//       Remove this at your own risk!
	if (TRUE)
	{
		// avoid warnings
		return E_INVALIDARG;
	}
#endif

	// already advised
	if ((m_readConnection) && (m_writeConnection))
	{
		return S_OK;
	}

	if (!m_group)
	{
		return E_FAIL;
	}

	// get the COM interface
	if (FAILED(res = m_group->queryInterface(IID_IDataObject, (void**)&dataObject)))
	{
		return res;
	}

	// call functions on OPC Server
	if (!m_readConnection)
	{
		// advise read connection
		format.cfFormat = (CLIPFORMAT)OPCSTMFORMATDATATIME_C;
		m_group->beforeInterfaceCall();
		res = dataObject->DAdvise(&format, ADVF_PRIMEFIRST, (IAdviseSink*)this, &m_readConnection);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IDataObject::DAdvise (read connection) [0x%X] "), res));
		m_group->setLastError(res, SODAC_ERR_ADVISE_DATAOBJECT_READ);

		if (SUCCEEDED(res))
		{
			m_establishedRead = TRUE;
		}
	}

	if (!m_writeConnection)
	{
		// advise write connection
		format.cfFormat = (CLIPFORMAT)OPCSTMFORMATWRITECOMPLETE_C;
		m_group->beforeInterfaceCall();
		res = dataObject->DAdvise(&format, ADVF_PRIMEFIRST, (IAdviseSink*)this, &m_writeConnection);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IDataObject::DAdvise (write connection) [0x%X] "), res));
		m_group->setLastError(res, SODAC_ERR_ADVISE_DATAOBJECT_WRITE);

		if (SUCCEEDED(res))
		{
			m_establishedWrite = TRUE;
		}
	}

	return res;
} // advise


//-----------------------------------------------------------------------------
// unadvise
//
// - disconnect the advise sink of the client to the server
//
// returns:
//		S_OK                  - operation succeeded
//      E_FAIL                - operation failed
//      E_INVALIDARG          - invalid argument
//      E_OUTOFMEMORY         - not enough memory
//      E_NOINTERFACE         - interface not supported
//
HRESULT SODaCAdviseSink::unadvise(void)
{
	CComPtr<IDataObject> dataObject;
	HRESULT res = S_OK;

	// already unadvised
	if ((!m_establishedRead) && (!m_establishedWrite))
	{
		return S_OK;
	}

	if (!m_group)
	{
		return E_FAIL;
	}

	// get the COM interface
	if (FAILED(res = m_group->queryInterface(IID_IDataObject, (void**)&dataObject)))
	{
		return res;
	}

	// call functions on OPC Server
	if (m_establishedRead)
	{
		// unadvise read connection
		m_group->beforeInterfaceCall();
		res = dataObject->DUnadvise(m_readConnection);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IDataObject::DUnadvise (read connection) [0x%X] "), res));
	}

	if (m_establishedWrite)
	{
		// unadvise write connection
		m_group->beforeInterfaceCall();
		res = dataObject->DUnadvise(m_writeConnection);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IDataObject::DUnadvise (write connection) [0x%X] "), res));
	}

	if (FAILED(res))
	{
		res = ::CoDisconnectObject(static_cast<IUnknown*>(this), 0);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoDisconnectObject() [0x%X] "), res));
	}

	m_readConnection = 0;
	m_establishedRead = FALSE;
	m_writeConnection = 0;
	m_establishedWrite = FALSE;
	m_group->setLastError(res, SODAC_ERR_UNADVISE_DATAOBJECT);
	return res;
} // unadvise

void SODaCAdviseSink::setGroup(SODaCComGroupImpl* pGrp)
{
	m_pComGroupImpl = pGrp;

	if (pGrp)
	{
		m_group = pGrp->getObjectDaCGroup();
	}
	else
	{
		m_group = NULL;
	}
}




//-----------------------------------------------------------------------------
// SODaCDataCallback                                                          |
//-----------------------------------------------------------------------------

SODaCDataCallback::SODaCDataCallback(
	IN SODaCComGroupImpl* group)    // group object
{
	m_group = group->getObjectDaCGroup();

	if (m_group)
	{
		m_group->addRef();
	}

	m_pComGroupImpl = group;
	m_refCount = 0;
	m_connection = 0;
	m_established = FALSE;
	AddRef();
}

SODaCDataCallback::~SODaCDataCallback()
{
	if (m_group)
	{
		m_group->release();
	}
}


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaCDataCallback::getObjTraceId(void)
{
	return m_group->getObjTraceId();
} // getObjTraceId


//-----------------------------------------------------------------------------
// AddRef
//
// - add reference to count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SODaCDataCallback::AddRef(void)
{
	return interlockedIncrement(&m_refCount);
} // AddRef


//-----------------------------------------------------------------------------
// Release
//
// - release one reference from count
//
// returns:
//		reference count
//
STDMETHODIMP_(ULONG) SODaCDataCallback::Release(void)
{
	ULONG currentCount = interlockedDecrement(&m_refCount);

	if (currentCount == 0)
	{
		delete this;
	}

	return currentCount;
} // Release


//-----------------------------------------------------------------------------
// QueryInterface
//
// - query for an interface of the object
//
// returns:
//		S_OK          - interface pointer returned
//		E_NOINTERFACE - interface not supported
//
STDMETHODIMP SODaCDataCallback::QueryInterface(
	IN REFIID iid,          // interface id
	IN LPVOID* ppInterface) // interface pointer
{
	if (ppInterface == NULL)
	{
		return E_INVALIDARG;
	}

	if (iid == IID_IUnknown)
	{
		*ppInterface = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_IOPCDataCallback)
	{
		*ppInterface = static_cast<IOPCDataCallback*>(this);
	}
	else
	{
		*ppInterface = NULL;
	}

	if (*ppInterface == NULL)
	{
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
} // QueryInterface


//-----------------------------------------------------------------------------
// OnDataChange
//
// - cyclic data retrieval changed server data
//
// returns:
//		S_OK
//
STDMETHODIMP SODaCDataCallback::OnDataChange(
	IN DWORD dwTransid,         // transaction id
	IN OPCHANDLE hGroup,        // group handle
	IN HRESULT hrMasterquality, // quality
	IN HRESULT hrMastererror,   // error
	IN DWORD dwCount,           // item count
	IN OPCHANDLE* phClientItems,// item handles
	IN VARIANT* pvValues,       // item values
	IN WORD* pwQualities,       // item qualities
	IN FILETIME* pftTimeStamps, // item timestamps
	IN HRESULT* pErrors)        // item errors
{
	SOCltComGuard gurad(this);

	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IOPCDataCallback::OnDataChange ; dwCount = %lu ; dwTransid = 0x%X"), dwCount, dwTransid));


	SOCMN_TRY
	{
		if (!m_established)
		{
			return S_OK;
		}

		if ((!m_pComGroupImpl) || (!m_group))
		{
			return S_OK;
		}

		if (SOCLT_COM_CHECK_DEMO)
		{
			return S_OK;
		}

		if (hGroup != m_group->getOpcHandle())
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("invalid group handle 0x%X in IOPCDataCallback::OnDataChange"), hGroup));
			m_group->setLastError(E_INVALIDARG, SODAC_ERR_READANDDATACHANGE_CALLBACK);
		}

		return handleReadAdvise(TRUE, dwTransid, hGroup, hrMasterquality, hrMastererror, dwCount,
		phClientItems, pvValues, pwQualities, pftTimeStamps, pErrors);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IOPCDataCallback::OnDataChange")));
		return S_OK;
	}
} // OnDataChange


//-----------------------------------------------------------------------------
// OnReadComplete
//
// - advise of  read call
//
// returns:
//		S_OK
//
STDMETHODIMP SODaCDataCallback::OnReadComplete(
	IN DWORD dwTransid,         // transaction id
	IN OPCHANDLE hGroup,        // group handle
	IN HRESULT hrMasterquality, // quality
	IN HRESULT hrMastererror,   // error
	IN DWORD dwCount,           // item count
	IN OPCHANDLE* phClientItems,// item handles
	IN VARIANT* pvValues,       // item values
	IN WORD* pwQualities,       // item qualities
	IN FILETIME* pftTimeStamps, // item timestamps
	IN HRESULT* pErrors)        // item errors
{
	SOCltComGuard gurad(this);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IOPCDataCallback::OnReadComplete ; dwCount = %lu ; dwTransid = 0x%X"), dwCount, dwTransid));
	SOCMN_TRY
	{
		if (!m_established)
		{
			return S_OK;
		}

		if ((!m_pComGroupImpl) || (!m_group))
		{
			return S_OK;
		}
		
		if (SOCLT_COM_CHECK_DEMO)
		{
			return S_OK;
		}

		if (hGroup != m_group->getOpcHandle())
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("invalid group handle 0x%X in IOPCDataCallback::OnReadComplete"), hGroup));
			m_group->setLastError(E_INVALIDARG, SODAC_ERR_READANDDATACHANGE_CALLBACK);
		}

		return handleReadAdvise(FALSE, dwTransid, hGroup, hrMasterquality, hrMastererror, dwCount,
		phClientItems, pvValues, pwQualities, pftTimeStamps, pErrors);
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IOPCDataCallback::OnReadComplete, TransactionId = 0x%8.8X"), dwTransid));
		return S_OK;
	}
} // OnReadComplete


//-----------------------------------------------------------------------------
// OnWriteComplete
//
// - advise of async write call
//
// returns:
//		S_OK
//
STDMETHODIMP SODaCDataCallback::OnWriteComplete(
	IN DWORD dwTransid,             // transaction id
	IN OPCHANDLE hGroup,            // group handle
	IN HRESULT hrMastererr,         // error
	IN DWORD dwCount,               // item count
	IN OPCHANDLE* pClienthandles,   // item handles
	IN HRESULT* pErrors)            // item errors
{
	SOCltComGuard gurad(this);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IOPCDataCallback::OnWriteComplete ; dwCount = %lu ; dwTransid = 0x%X"), dwCount, dwTransid));

	SOCMN_TRY
	{
		if (!m_established)
		{
			return S_OK;
		}

		if ((!m_pComGroupImpl) || (!m_group))
		{
			return S_OK;
		}
		
		if (SOCLT_COM_CHECK_DEMO)
		{
			return S_OK;
		}

		if (hGroup != m_group->getOpcHandle())
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("invalid group handle 0x%X in IOPCDataCallback::OnWriteComplete"), hGroup));
			m_group->setLastError(E_INVALIDARG, SODAC_ERR_WRITECOMPLETE_CALLBACK);
		}

		ptrdiff_t* pItems;
		SOCmnPointer<SODaCItem> item;
		DWORD i;
		DWORD rellevantItemCount = 0;
		HRESULT *pRellevantErrors = (HRESULT*) malloc(dwCount * sizeof(HRESULT));

		pItems = new ptrdiff_t[dwCount];
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T(" IN : hrMastererr: 0x%X ; hGroup: 0x%8.8X"), dwCount, hrMastererr, hGroup));

		for (i = 0; i < dwCount; i++)
		{
			item = m_group->getItemByClientHandle(pClienthandles[i]);

			if (item.isNotNull())
			{
				item.addRef();
				pItems[rellevantItemCount] = (ptrdiff_t)(void*)(SODaCItem*)item;
				_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s ; 0x%8.8X"), i, (LPCTSTR)item->getItemID(), pErrors[i]));
				item->setWrite(pErrors[rellevantItemCount], dwTransid);
				pRellevantErrors[rellevantItemCount] = pErrors[i];
				++rellevantItemCount;
			}
			else
			{
				_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, (DWORD)pClienthandles[i]));
				m_group->setLastError(OPC_E_INVALIDHANDLE, SODAC_ERR_ADVISE_DATACALLBACK);
			}
		}

		m_group->onWriteAdvise(dwTransid, hrMastererr, rellevantItemCount, (SODaCItem**)pItems, pRellevantErrors);

		for (i = 0; i < rellevantItemCount; i++)
		{
			((SODaCItem*)pItems[i])->release();
		}

		delete[] pItems;
		free(pRellevantErrors);
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IOPCDataCallback::OnWriteComplete, TransactionId = 0x%8.8X"), dwTransid));
		return S_OK;
	}
} // OnWriteComplete


//-----------------------------------------------------------------------------
// OnCancelComplete
//
// - advise of async cancel call
//
// returns:
//		S_OK
//
STDMETHODIMP SODaCDataCallback::OnCancelComplete(
	IN DWORD dwTransid,     // transaction id
	IN OPCHANDLE hGroup)    // group handle
{
	SOCltComGuard gurad(this);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_ADVISE_C, (_T("IOPCDataCallback::OnCancelComplete ; dwTransid = 0x%X; hGroup = 0x%X"), dwTransid, hGroup));
	SOCMN_TRY
	{
		if (!m_established)
		{
			return S_OK;
		}

		if ((!m_pComGroupImpl) || (!m_group))
		{
			return S_OK;
		}
		
		if (hGroup != m_group->getOpcHandle())
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("invalid group handle 0x%X in IOPCDataCallback::OnCancelComplete"), hGroup));
			m_group->setLastError(E_INVALIDARG, SODAC_ERR_CANCELCOMPLETE_CALLBACK);
		}

		m_pComGroupImpl->onCancelAdvise(dwTransid, hGroup);
		return S_OK;
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_ADVISE_C, (_T("EXCEPTION in IOPCDataCallback::OnCancelComplete clientHandle 0x%8.8X  transactionId 0x%8.8X"), hGroup, dwTransid));
		return S_OK;
	}
} // OnCancelComplete


//-----------------------------------------------------------------------------
// handleReadAdvise
//
// - handle advise of async read data
//
// returns:
//		S_OK
//
HRESULT SODaCDataCallback::handleReadAdvise(IN BYTE dataChangeOrRead,
											IN DWORD dwTransid,         // transaction id
											IN OPCHANDLE hGroup,        // group handle
											IN HRESULT hrMasterquality, // quality
											IN HRESULT hrMastererror,   // error
											IN DWORD dwCount,           // item count
											IN OPCHANDLE* phClientItems,// item handles
											IN VARIANT* pvValues,       // item values
											IN WORD* pwQualities,       // item qualities
											IN FILETIME* pftTimeStamps, // item timestamps
											IN HRESULT* pErrors)        // item errors
{
	SOCltComGuard gurad(this);
	SOCmnPointer<SODaCItem> item;
	ptrdiff_t* pItems;
	SOCmnVariant* pValues;
	SOCmnDateTime* pTimestamps;
	DWORD i;
	DWORD rellevantItemCount = 0;
	pItems = new ptrdiff_t[dwCount];
	pValues = new SOCmnVariant[dwCount];
	WORD *pQualities = new WORD[dwCount];
	pTimestamps = new SOCmnDateTime[dwCount];
	m_pComGroupImpl->onDataChangeBegin();
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T(" IN : TransID=0x%X; count: 0x%X ; hrMastererror: 0x%X ; hrMasterquality: 0x%X ; hGroup: 0x%8.8X"), dwTransid, dwCount, hrMastererror, hrMasterquality, hGroup));

	for (i = 0; i < dwCount; i++)
	{
		item = m_group->getItemByClientHandle(phClientItems[i]);

		if (item.isNotNull())
		{
			item.addRef();
			pItems[rellevantItemCount] = (ptrdiff_t)(void*)(SODaCItem*)item;
			_TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C)
			{
				if (SUCCEEDED(pErrors[i]))
				{
					SOCmnString v, q, t;
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s (0x%8.8X); %s %s %s"), i, (LPCTSTR)item->getItemID(), (DWORD)phClientItems[i], variantWithType2string(&v, &pvValues[i]), quality2string(&q, pwQualities[i]), filetime2string(&t, &pftTimeStamps[i])));
				}
				else
				{
					_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] %s (0x%8.8X) [0x%8.8X]"), i, (LPCTSTR)item->getItemID(), (DWORD)phClientItems[i], pErrors[i]));
				}
			}
			pValues[rellevantItemCount] = &pvValues[i];
			pTimestamps[rellevantItemCount] = &pftTimeStamps[i];
			pQualities[rellevantItemCount] = pwQualities[i];
			item->setRead(pErrors[i], &pValues[rellevantItemCount], pQualities[rellevantItemCount], &pTimestamps[rellevantItemCount], dwTransid);
			++rellevantItemCount;
		}
		else
		{
			_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_OPC_ADVISE_C, (_T("      [%lu] Client handle 0x%8.8X: no item found"), i, (DWORD)phClientItems[i]));
			m_group->setLastError(OPC_E_INVALIDHANDLE, SODAC_ERR_READANDDATACHANGE_CALLBACK);
			pItems[i] = NULL;
		}
	}

	m_group->onReadAdvise(dataChangeOrRead, dwTransid, hrMasterquality, hrMastererror, rellevantItemCount,
						  (SODaCItem**)pItems, pValues, pQualities, pTimestamps, pErrors);

	for (i = 0; i < rellevantItemCount; i++)
	{
		((SODaCItem*)pItems[i])->release();
	}

	m_pComGroupImpl->onDataChangeEnd();
	delete[] pItems;
	delete[] pValues;
	delete[] pTimestamps;
	delete[] pQualities;
	return S_OK;
} // handleReadAdvise


//-----------------------------------------------------------------------------
// advise
//
// - connect the advise sink of the client to the server
//
// returns:
//		S_OK                  - operation succeeded
//      E_FAIL                - operation failed
//      E_INVALIDARG          - invalid argument
//      E_OUTOFMEMORY         - not enough memory
//      E_NOINTERFACE         - interface not supported
//      CONNECT_E_ADVISELIMIT - reached maximal advise limit of server
//
HRESULT SODaCDataCallback::advise(void)
{
	CComPtr<IConnectionPointContainer> cpc;
	CComPtr<IConnectionPoint> cp;
	HRESULT res;

	if (!m_group)
	{
		return E_FAIL;
	}

	SOCmnPointer<SODaCServer> server = (SODaCServer*)m_group->getParentBranch();

	if (server.isNotNull())
	{
		if (server->getForcedOPCVersion() == SODAC_VERSION_OPC_DAV10A)
		{
			return E_NOINTERFACE;    // forced DA V1
		}
	}

	res = m_group->queryInterface(IID_IConnectionPointContainer, (void**)&cpc);

	if (SUCCEEDED(res))
	{
		m_group->beforeInterfaceCall();
		res = cpc->FindConnectionPoint(_ATL_IIDOF(IOPCDataCallback), &cp);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_group->beforeInterfaceCall();
		res = cp->Advise((IUnknown*)this, &m_connection);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPoint::Advise [0x%X] "), res));

		if (SUCCEEDED(res))
		{
			m_established = TRUE;
		}
	}

	m_group->setLastError(res, SODAC_ERR_ADVISE_DATACALLBACK);
	return res;
} // advise


//-----------------------------------------------------------------------------
// unadvise
//
// - unadvise the shutdown event sink
//
// returns:
//		S_OK                   - connection unadvised or no connection was advised
//		E_NOINTERFACE          - IConnectionPointContainer interface not supported by server -> OPC V1.0A
//		CONNECT_E_NOCONNECTION - IOPCShutdown is not supported by connection point container
//		CONNECT_E_ADVISELIMIT  - the connection point has already reached its limit of connections and cannot accept any more
//		E_FAIL
//
HRESULT SODaCDataCallback::unadvise(BOOL connectionMonitorStatus)
{
	CComPtr<IConnectionPointContainer> cpc;
	CComPtr<IConnectionPoint> cp;
	HRESULT res;

	if (!m_established)
	{
		return S_OK;    // no connection was advised
	}

	if (!m_group)
	{
		return E_FAIL;
	}

	m_established = FALSE;

	// Get connection point container of the OPC group object
	res = m_group->queryInterface(IID_IConnectionPointContainer, (void**)&cpc);

	if ((SUCCEEDED(res)) && (connectionMonitorStatus))
	{
		res = 0x800706ba; // The RPC server is unavailable.
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint skipped, ConnectionMonitor is marked disconnected: [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_group->beforeInterfaceCall();
		res = cpc->FindConnectionPoint(_ATL_IIDOF(IOPCDataCallback), &cp);
		m_group->afterInterfaceCall();
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IConnectionPointContainer::FindConnectionPoint [0x%X] "), res));
	}

	if (SUCCEEDED(res))
	{
		m_group->beforeInterfaceCall();
		res = cp->Unadvise(m_connection);
		m_group->afterInterfaceCall();
	}

	if (FAILED(res))
	{
		res = ::CoDisconnectObject(static_cast<IUnknown*>(this), 0);
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoDisconnectObject() [0x%X] "), res));
	}

	m_connection = 0;
	m_group->setLastError(res, SODAC_ERR_UNADVISE_DATACALLBACK);
	return res;
} // unadvise

void SODaCDataCallback::setGroup(SODaCComGroupImpl* pGrp)
{
	m_pComGroupImpl = pGrp;

	if (pGrp)
	{
		m_group = pGrp->getObjectDaCGroup();
	}
	else
	{
		m_group = NULL;
	}
}
#endif // SOFEATURE_DCOM

#endif // SODAC

