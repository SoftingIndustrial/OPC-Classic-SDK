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
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSComEnum.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DCOM enumerator classes                                     |
//                - SODaSComEnumItem: OPCITEMATTRIBUTES enumerator            |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_DCOM

#include "SODaSComEnum.h"
#include "SOCmnTrace.h"


//-----------------------------------------------------------------------------
// SODaSComEnumItem                                                           |
//-----------------------------------------------------------------------------

SODaSComEnumItem::SODaSComEnumItem()
{
#if _MSC_VER >= 1400 // Microsoft Visual Studio 2005
	_AtlInitialConstruct();
#endif
	m_objectTraceId.Format(_T("(enumI| %X)"), this->getOpcHandle());
	_TRACE_CREATE_COMOBJ();
	m_list.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	m_curPos = m_list.GetHeadPosition();
}

SODaSComEnumItem::~SODaSComEnumItem()
{
	SODaSItem* item;
	SOCmnListPosition pos;
	pos = m_list.GetHeadPosition();

	while (pos)
	{
		item = m_list.GetNext(pos);
		item->release();
	}

	lastCOMObjectActions();
	_TRACE_DELETE_COMOBJ();
}


//-----------------------------------------------------------------------------
// Next (IEnumOPCItemAttributes)
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
STDMETHODIMP SODaSComEnumItem::Next(
	IN ULONG celt,                  // number of entries to get
	OUT OPCITEMATTRIBUTES** rgelt,  // entries
	OUT ULONG* pceltFetched)        // number of returned entries
{
	ULONG i = 0;
	SODaSItem* item;
	SOCmnPointer<SODaSItemTag> tag;
	SOCmnPointer<SODaSServer> server;
	SOCmnString str;
	OPCITEMATTRIBUTES* itemArray;
	HRESULT res = S_OK;
	USES_CONVERSION;
	SOCmnVariant euInfo;

	if (rgelt == NULL)
	{
		return E_POINTER;
	}

	*rgelt = NULL;

	if ((pceltFetched == NULL) && (celt != 1))
	{
		return E_INVALIDARG;
	}

	if (pceltFetched)
	{
		*pceltFetched = 0;
	}

	itemArray = (OPCITEMATTRIBUTES*)CoTaskMemAlloc(celt * sizeof(OPCITEMATTRIBUTES));

	if (itemArray == NULL)
	{
		return E_OUTOFMEMORY;
	}

	memset(itemArray, 0, sizeof(OPCITEMATTRIBUTES) * celt);

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		item = m_list.GetNext(m_curPos);
		tag = item->getItemTag();
		server = item->getServer();

		if (tag.isNotNull())
		{
			itemArray[i].szItemID = SOCmnString::cloneToWCHAR(tag->getItemID());

			if (!(itemArray[i].szItemID))
			{
				res = E_OUTOFMEMORY;
			}

			itemArray[i].dwAccessRights = tag->getAccessRights(server);
			itemArray[i].vtCanonicalDataType = tag->getNativeDatatype();
			itemArray[i].dwEUType = tag->getEUType();
			tag->getEUInfo(euInfo);
			SOCmnVariant::variantCopy(&itemArray[i].vEUInfo, euInfo);
		}
		else
		{
			itemArray[i].szItemID = SOCmnString::cloneToWCHAR(SOCmnString(_T("")));
			itemArray[i].dwAccessRights = 0;
			itemArray[i].vtCanonicalDataType = VT_EMPTY;
			itemArray[i].dwEUType = OPC_NOENUM;
			VariantInit(&itemArray[i].vEUInfo);
		}

		itemArray[i].szAccessPath = SOCmnString::cloneToWCHAR(item->getAccessPath());

		if (!(itemArray[i].szAccessPath))
		{
			res = E_OUTOFMEMORY;
		}

		itemArray[i].bActive = item->getActive();
		itemArray[i].hClient = item->getClientHandle();
		itemArray[i].hServer = item->getServerHandle();
		itemArray[i].vtRequestedDataType = item->getReqDatatype();
		itemArray[i].pBlob = NULL;
		itemArray[i].dwBlobSize = 0;

		if (res == E_OUTOFMEMORY)
		{
			ULONG j;

			for (j = 0; j <= i; j++)
			{
				if (itemArray[j].szItemID)
				{
					CoTaskMemFree(itemArray[j].szItemID);
				}

				if (itemArray[j].szAccessPath)
				{
					CoTaskMemFree(itemArray[j].szAccessPath);
				}

				SOCmnVariant::variantClear(&itemArray[j].vEUInfo);
			}

			CoTaskMemFree(itemArray);
		}
	}

	if (pceltFetched)
	{
		*pceltFetched = i;
	}

	if (i != 0)
	{
		*rgelt = (OPCITEMATTRIBUTES*)CoTaskMemRealloc(itemArray, i * sizeof(OPCITEMATTRIBUTES));
	}
	else
	{
		if (celt != 0)
		{
			CoTaskMemFree(itemArray);
		}

		*rgelt = NULL;
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Next


//-----------------------------------------------------------------------------
// Skip (IEnumOPCItemAttributes)
//
// - skip next entries of the enum
//
// returns:
//		S_OK    - number of entries skiped
//		S_FALSE - reached end of list before skipped all requested entries
//
STDMETHODIMP SODaSComEnumItem::Skip(
	IN ULONG celt)
{
	ULONG i = 0;

	for (i = 0; (i < celt) && (m_curPos); i++)
	{
		m_list.GetNext(m_curPos);
	}

	return (celt == i) ? S_OK : S_FALSE;
} // Skip


//-----------------------------------------------------------------------------
// Reset (IEnumOPCItemAttributes)
//
// - reset current list position to the head of the list
//
// returns:
//		S_OK
//
STDMETHODIMP SODaSComEnumItem::Reset(void)
{
	m_curPos = m_list.GetHeadPosition();
	return S_OK;
} // Reset


//-----------------------------------------------------------------------------
// Clone (IEnumOPCItemAttributes)
//
// - clone the enumerator
//
// returns:
//		S_OK         - enum cloned
//		E_INVALIDARG - invalid argument
//
STDMETHODIMP SODaSComEnumItem::Clone(
	OUT IEnumOPCItemAttributes** ppEnumOPCItemAttr)     // cloned enum
{
	SODaSItem* item;

	if (!ppEnumOPCItemAttr)
	{
		return E_INVALIDARG;
	}

	OPCItemEnumObject* clone;
	SOCMN_ALLOCATE_OBJECT(clone, OPCItemEnumObject)
	SOCmnListPosition pos;
	SOCmnListPosition clonePos;
	BOOL isCurPos;
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

		item = m_list.GetNext(pos);
		item->addRef();
		clonePos = clone->m_list.AddTail(item);

		if (isCurPos)
		{
			clone->m_curPos = clonePos;
		}
	}

	clone->QueryInterface(IID_IEnumOPCItemAttributes, (void**)ppEnumOPCItemAttr);
	return S_OK;
} // Clone

#endif // SOFEATURE_DCOM

#endif // SODAS
