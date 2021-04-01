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
//  Filename    : SODaCComGroup.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Group object related classes                                |
//                - SODaCComGroupImpl: proxy for OPC group object             |
//                - SODaCComGroupImplDefault: default group class for creator |
//                - SODaCComTaskSyncRead: synchronous read task               |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_DCOM

#include <opcda.h>
#include "SODaCServer.h"
#include "SODaCComGroup.h"
#include "SODaCItem.h"
#include "SODaCEntry.h"
#include "SODaCCallback.h"
#include "SOCmnHandleManager.h"


//-----------------------------------------------------------------------------
// SODaCComGroupImpl                                                          |
//-----------------------------------------------------------------------------

SODaCComGroupImpl::SODaCComGroupImpl(void)
{
	m_active = FALSE;
	m_dataCallback = NULL;
	m_adviseSink = NULL;
	::CoGetMalloc(MEMCTX_TASK, &m_malloc);
	m_taskSyncRead = 0;
}

void SODaCComGroupImpl::init(SODaCServer* /* pServer */)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	pDaCGroup->m_dataRetrieval = SODaCGroup::drAsyncAdvise;
}

SODaCComGroupImpl::~SODaCComGroupImpl(void)
{
	if (m_dataCallback)
	{
		m_dataCallback->setGroup(NULL);
		m_dataCallback->Release();
		m_dataCallback = NULL;
	}

	if (m_adviseSink)
	{
		m_adviseSink->setGroup(NULL);
		m_adviseSink->Release();
		m_adviseSink = NULL;
	}
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//		returns of unadvise
//
HRESULT SODaCComGroupImpl::doDisconnectImpl(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> srv = (SODaCServer*)pDaCGroup->getParentBranch();

	// remove cyclic task, if any
	if (m_taskSyncRead)
	{
		BOOL bRet = pDaCGroup->getWorkerThread()->removeCyclicTask(m_taskSyncRead);

		if (bRet)
		{
			m_taskSyncRead = 0;
		}

		_ASSERTION(bRet, "Error removing sync read task.");
	}

	HRESULT res = S_OK;
	HRESULT allRes = S_OK;

	// unadvise async connection

	if (FAILED(res = unadvise()))
	{
		allRes = res;
	}

	// release all interfaces
	m_unknown = NULL;
	m_itemMgt = NULL;
	m_syncIO = NULL;
	m_asyncIO = NULL;
	m_asyncIO2 = NULL;
	m_grpState = NULL;
	m_grpStatePub = NULL;
	m_syncIO2 = NULL;
	m_asyncIO3 = NULL;
	m_grpState2 = NULL;
	if (pDaCGroup->getConnectionMonitorStatus())
	{
		m_itemDeadbandMgt.putInterface(NULL, TRUE);
	}
	else
	{
		m_itemDeadbandMgt = NULL;
	}
	m_itemSamplingMgt = NULL;

	// remove added group
	if (pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup)
	{
		if (!pDaCGroup->m_isPublic)
		{
			// check server object
			if (srv.isNotNull())
			{
				if (srv->isConnected())
				{
					// remove added group
					CComPtr<IOPCServer> server;
					res = srv->queryInterface(IID_IOPCServer, (void**)&server);

					if ((SUCCEEDED(res)) && (pDaCGroup->getConnectionMonitorStatus()))
					{
						res = 0x800706ba; // The RPC server is unavailable.
						_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup,(_T("SKIPPING IOPCServer::RemoveGroup, DCOM already marked as disconected [0x%X] "), res));
					}

					if (SUCCEEDED(res))
					{
						pDaCGroup->beforeInterfaceCall();
						res = server->RemoveGroup(m_serverHandle, FALSE);
						_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCServer::RemoveGroup [0x%X] "), res));
						pDaCGroup->afterInterfaceCall();

						if (FAILED(res))
						{
							allRes = res;
						}
					}
					else
					{
						allRes = res;
					}

					pDaCGroup->setLastError(res, SODAC_ERR_REMOVE_GROUP);
				}
				else
				{
					pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
					allRes = E_FAIL;
				}
			}
			else
			{
				pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
				allRes = E_FAIL;
			}
		}
		else
		{
			pDaCGroup->setPublic(FALSE);
		}
	}

	if (pDaCGroup->m_connectionType == SODaCGroup::ctGetPublicGroupByName)
	{
		// remove item proxies
		pDaCGroup->removeAllLeafs();
	}

	pDaCGroup->m_isPublic = FALSE;
	pDaCGroup->setObjectState(SOCltElement::disconnected);
	return allRes;
} // doDisconnectImpl

//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the group if the target state differs from the
//   actual state of the group

HRESULT SODaCComGroupImpl::doStateTransitionImpl(BOOL deep, IN BYTE stateDeep)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	SOCmnString resDescr;
	BYTE targetState = pDaCGroup->getTargetObjectState();
	USES_CONVERSION;
	SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();

	if (SOCltElement::disconnected == targetState)
	{
		pDaCGroup->setItemsTargetState(targetState);
		doItemsStateTransition();
	}

	while ((SUCCEEDED(res)) && (pDaCGroup->getObjectState() != targetState))
	{
		pDaCGroup->setStateTransitionActive(TRUE);

		switch (pDaCGroup->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			//-- connect or start group --
			SOCmnPointer<SODaCServer> srv = (SODaCServer*)pDaCGroup->getParentBranch();
			CComPtr<IOPCServer> server;
			CComPtr<IOPCServerPublicGroups> pubServer;

			// check server object
			if (!srv)
			{
				pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_NO_PARENT_OBJECT);
				pDaCGroup->setStateTransitionActive(FALSE);
				return E_FAIL;
			}

			if (!srv->isConnected())
			{
				pDaCGroup->setLastError(E_FAIL, SOCLT_ERR_PARENT_NOT_CONNECTED);
				pDaCGroup->setStateTransitionActive(FALSE);
				return E_FAIL;
			}

			// get COM interface
			if (pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup)
			{
				if (FAILED(res = srv->queryInterface(IID_IOPCServer, (void**)&server)))
				{
					pDaCGroup->setStateTransitionActive(FALSE);
					return res;
				}
			}

			if (pDaCGroup->m_connectionType == SODaCGroup::ctGetPublicGroupByName)
			{
				if (FAILED(res = srv->queryInterface(IID_IOPCServerPublicGroups, (void**)&pubServer)))
				{
					pDaCGroup->setStateTransitionActive(FALSE);
					return res;
				}
			}

			if (targetState == SOCltElement::connected)
			{
				m_active = FALSE;    // connect
			}
			else
			{
				m_active = TRUE;    // start
			}

			// call connect function
			switch (pDaCGroup->m_connectionType)
			{
			case SODaCGroup::ctAddGroup:
			{
				// create group by adding it to the server object
				DWORD revUpdateRate;
				LONG* timeBias = NULL;
				FLOAT* deadBand = NULL;
				DWORD unvalPar = 0;
				DWORD valPar = 0;

				// get parameters for AddGroup call
				if (pDaCGroup->m_validParameter & SODAC_GRPPAR_TIMEBIAS)
				{
					timeBias = &pDaCGroup->m_timeBias;
				}
				else
				{
					unvalPar |= SODAC_GRPPAR_TIMEBIAS;
				}

				if (pDaCGroup->m_validParameter & SODAC_GRPPAR_DEADBAND)
				{
					deadBand = &pDaCGroup->m_deadBand;
				}
				else
				{
					unvalPar |= SODAC_GRPPAR_DEADBAND;
				}

				if ((pDaCGroup->m_validParameter & SODAC_GRPPAR_LCID) != SODAC_GRPPAR_LCID)
				{
					pDaCGroup->m_lcid = 0;
					unvalPar |= SODAC_GRPPAR_LCID;
				}

				if (srv->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
				{
					if (pDaCGroup->m_validParameter & SODAC_GRPPAR_KEEPALIVETIME)
					{
						valPar |= SODAC_GRPPAR_KEEPALIVETIME;
					}
					else
					{
						unvalPar |= SODAC_GRPPAR_KEEPALIVETIME;
					}
				}

				CComPtr<IUnknown> pUnk;
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCServer::AddGroup >")));
				pDaCGroup->beforeInterfaceCall();
				res = server->AddGroup(T2W((LPTSTR)((LPCTSTR)pDaCGroup->getName())), m_active, pDaCGroup->m_reqUpdateRate, (OPCHANDLE)pDaCGroup->getOpcHandle(),
									   timeBias, deadBand, pDaCGroup->m_lcid, &m_serverHandle,
									   &revUpdateRate, IID_IUnknown, (LPUNKNOWN*)&pUnk);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCServer::AddGroup [0x%X] "), res));
				pDaCGroup->setLastError(res, SODAC_ERR_ADD_GROUP);

				if (SUCCEEDED(res) && pUnk != NULL)
				{
					queryAllInterfaces(pUnk);
				}

				if (SUCCEEDED(res))
				{
					pDaCGroup->setPublic(FALSE);    // added group is not public
					pDaCGroup->m_revUpdateRate = revUpdateRate;
					INVOKE_CALLBACK_ON_PTR(pDaCGroup, SODaCGroup, onSetRevUpdateRate);

					if (valPar)
					{
						res = pDaCGroup->doUpdate(FALSE, valPar);
					}

					// get all group parameter
					if (pDaCGroup->performStateTransitionAction(SODAC_GRP_ACTION_GET_GRP_PARS))
					{
						res = pDaCGroup->doUpdate(TRUE, unvalPar);
					}
				}
			}
			break;

			case SODaCGroup::ctGetPublicGroupByName:
			{
				// create group by getting a public group instance
				CComPtr<IUnknown> pUnk;
				pDaCGroup->beforeInterfaceCall();
				res = pubServer->GetPublicGroupByName(T2W((LPTSTR)((LPCTSTR)pDaCGroup->getName())), IID_IUnknown, &pUnk);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCServer::GetPublicGroupByName, Group - %s [0x%X] "), (LPCTSTR)pDaCGroup->getName(), res));
				pDaCGroup->setLastError(res, SODAC_ERR_GET_PUBGROUPBYNAME);

				if (SUCCEEDED(res))
				{
					queryAllInterfaces(pUnk);
				}

				if (SUCCEEDED(res))
				{
					pDaCGroup->setPublic(TRUE); // public group
					DWORD valPar = pDaCGroup->m_validParameter;

					// get the once we do not have....
					if (SUCCEEDED(res = pDaCGroup->doUpdate(TRUE, (~valPar) | SODAC_GRPPAR_SERVERHANDLE)))
					{
						if ((pDaCGroup->m_validParameter & SODAC_GRPPAR_UPDATERATE) == 0)
						{
							pDaCGroup->m_reqUpdateRate = pDaCGroup->m_revUpdateRate;
						}

						// ...and set the once we know.
						if (SUCCEEDED(res = pDaCGroup->doUpdate(FALSE, SODAC_GRPPAR_ACTIVE | SODAC_GRPPAR_CLIENTHANDLE | valPar)))
						{
							// create the item proxies for the group
							res = createItems();
						}
					}
				}
			}
			break;

			default:
				_ASSERTION(FALSE, "Bad group connection type.");
				res = E_FAIL;
				pDaCGroup->setLastError(res, SODAC_BAD_GROUP_CONNECTION_TYPE);
				break;
			}
			
			BOOL resetAsyncAdvise = FALSE;
			if (SUCCEEDED(res))
			{
				if (pDaCGroup->m_dataRetrieval == SODaCGroup::drAsyncAdvise)
				{
					// advise if async
					res = advise();

					if (FAILED(res))
					{
						if (!getReadConnection(NULL)) // no read notification connection
						{
							pDaCGroup->m_dataRetrieval = SODaCGroup::drSyncRead;    // automatically change to synchronous cache reads
							resetAsyncAdvise = TRUE;
						}
					}
				}

				if (pDaCGroup->m_dataRetrieval == SODaCGroup::drSyncRead)
				{
					SOCmnPointer<SODaCComTaskSyncRead> task;
					SOCMN_ALLOCATE_OBJECT_C(task, SODaCComTaskSyncRead(pDaCGroup, pDaCGroup->getUpdateRate()), (SODaCComTaskSyncRead*));
					pDaCGroup->getWorkerThread()->addCyclicTask((SODaCComTaskSyncRead*)task, &m_taskSyncRead);
				}
			}

			// undo all if failed
			if (SUCCEEDED(res))
			{
				pDaCGroup->setObjectState(targetState); // set object state to target state
			}
			else // FAILED
			{
				if (resetAsyncAdvise == TRUE)
				{
					pDaCGroup->m_dataRetrieval = SODaCGroup::drAsyncAdvise;
				}

				doDisconnectImpl();
			}
		}
		break;

		case SOCltElement::connected:
		case SOCltElement::started:
		{
			if (targetState == SOCltElement::disconnected)
			{
				//-- disconnect group --
				res = doDisconnectImpl();
			}
			else
			{
				//-- start or stop group --
				m_active = (targetState == SOCltElement::started) ? TRUE : FALSE;
				res = pDaCGroup->doUpdate(FALSE, SODAC_GRPPAR_ACTIVE | SODAC_GRPPAR_CLIENTHANDLE);

				if (SUCCEEDED(res))
				{
					pDaCGroup->setObjectState(targetState);
				}
			}
		}
		break;
		}

		pDaCGroup->setStateTransitionActive(FALSE);
	}

	if (SUCCEEDED(res) && deep && targetState != SOCltElement::disconnected)
	{
		if (stateDeep != SOCLTTASKSTATETRANSITION_NO_STATE_CHANGE)
		{
			pDaCGroup->setItemsTargetState(stateDeep);
		}

		res = doItemsStateTransition();
	}

	return res;
} // doStateTransition


HRESULT SODaCComGroupImpl::queryAllInterfaces(IUnknown* punknown)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	_ASSERTION(punknown, "NULL interface pointer.");
	HRESULT res = S_OK;
	m_unknown = punknown;
	MULTI_QI mqi[11];
	::ZeroMemory(mqi, sizeof(mqi));
	mqi[0].pIID = &IID_IOPCItemMgt;
	mqi[1].pIID = &IID_IOPCSyncIO;
	mqi[2].pIID = &IID_IOPCAsyncIO;
	mqi[3].pIID = &IID_IOPCAsyncIO2;
	mqi[4].pIID = &IID_IOPCGroupStateMgt;
	mqi[5].pIID = &IID_IOPCPublicGroupStateMgt;
	mqi[6].pIID = &IID_IOPCSyncIO2;
	mqi[7].pIID = &IID_IOPCAsyncIO3;
	mqi[8].pIID = &IID_IOPCGroupStateMgt2;
	mqi[9].pIID = &IID_IOPCItemDeadbandMgt;
	mqi[10].pIID = &IID_IOPCItemSamplingMgt;
	CComPtr<IMultiQI> pMQI;
	res = punknown->QueryInterface(_ATL_IIDOF(IMultiQI), (void**)&pMQI);

	if (SUCCEEDED(res))
	{
		pDaCGroup->beforeInterfaceCall();
		res = pMQI->QueryMultipleInterfaces(_COUNTOF(mqi), mqi);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IMultiQI::QueryMultipleInterfaces [0x%X] "), res));
	}
	else
	{
		for (int i = 0; i < _COUNTOF(mqi); i++)
		{
			mqi[i].hr = punknown->QueryInterface(*mqi[i].pIID, (void**)&mqi[i].pItf);
#ifndef _DEBUG
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IUnknown::QueryInterface [0x%X] "), mqi[i].hr));
#else
			SOCmnString iidString;
			IID2string(&iidString, *mqi[i].pIID);
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IUnknown::QueryInterface %s [0x%X] "), (LPCTSTR)iidString, mqi[i].hr));
#endif  //  _DEBUG
		}

		res = S_OK;
	}

	if (SUCCEEDED(res))
	{
		SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();

		if (server.isNotNull())
		{
			if (server->getForcedOPCVersion() != SODAC_VERSION_OPC_USEDEFAULT)
			{
				switch (server->getForcedOPCVersion())
				{
				case SODAC_VERSION_OPC_DAV10A:
					if (mqi[3].pItf)
					{
						mqi[3].pItf->Release();
						mqi[3].pItf = NULL;
						mqi[3].hr = E_NOINTERFACE;
					}

					if (mqi[6].pItf)
					{
						mqi[6].pItf->Release();
						mqi[6].pItf = NULL;
						mqi[6].hr = E_NOINTERFACE;
					}

					if (mqi[7].pItf)
					{
						mqi[7].pItf->Release();
						mqi[7].pItf = NULL;
						mqi[7].hr = E_NOINTERFACE;
					}

					if (mqi[8].pItf)
					{
						mqi[8].pItf->Release();
						mqi[8].pItf = NULL;
						mqi[8].hr = E_NOINTERFACE;
					}

					if (mqi[9].pItf)
					{
						mqi[9].pItf->Release();
						mqi[9].pItf = NULL;
						mqi[9].hr = E_NOINTERFACE;
					}

					if (mqi[10].pItf)
					{
						mqi[10].pItf->Release();
						mqi[10].pItf = NULL;
						mqi[10].hr = E_NOINTERFACE;
					}

					break;

				case SODAC_VERSION_OPC_DAV20:
					if (mqi[6].pItf)
					{
						mqi[6].pItf->Release();
						mqi[6].pItf = NULL;
						mqi[6].hr = E_NOINTERFACE;
					}

					if (mqi[7].pItf)
					{
						mqi[7].pItf->Release();
						mqi[7].pItf = NULL;
						mqi[7].hr = E_NOINTERFACE;
					}

					if (mqi[8].pItf)
					{
						mqi[8].pItf->Release();
						mqi[8].pItf = NULL;
						mqi[8].hr = E_NOINTERFACE;
					}

					if (mqi[9].pItf)
					{
						mqi[9].pItf->Release();
						mqi[9].pItf = NULL;
						mqi[9].hr = E_NOINTERFACE;
					}

					if (mqi[10].pItf)
					{
						mqi[10].pItf->Release();
						mqi[10].pItf = NULL;
						mqi[10].hr = E_NOINTERFACE;
					}

					break;
				}
			}
		}

		m_itemMgt = (IOPCItemMgt*) mqi[0].pItf;
		m_syncIO = (IOPCSyncIO*) mqi[1].pItf;
		m_asyncIO = (IOPCAsyncIO*) mqi[2].pItf;
		m_asyncIO2 = (IOPCAsyncIO2*) mqi[3].pItf;
		m_grpState = (IOPCGroupStateMgt*) mqi[4].pItf;
		m_grpStatePub = (IOPCPublicGroupStateMgt*) mqi[5].pItf;
		m_syncIO2 = (IOPCSyncIO2*) mqi[6].pItf;
		m_asyncIO3 = (IOPCAsyncIO3*) mqi[7].pItf;
		m_grpState2 = (IOPCGroupStateMgt2*) mqi[8].pItf;
		if (pDaCGroup->getConnectionMonitorStatus())
		{
			m_itemDeadbandMgt.putInterface((IOPCItemDeadbandMgt*) mqi[9].pItf, TRUE);
		}
		else
		{
			m_itemDeadbandMgt = (IOPCItemDeadbandMgt*) mqi[9].pItf;
		}
		m_itemSamplingMgt = (IOPCItemSamplingMgt*) mqi[10].pItf;

		for (int i = 0; i < _COUNTOF(mqi); i++)
		{
			if (SUCCEEDED(mqi[i].hr))
			{
				mqi[i].pItf->Release();
			}
		}
	}

	return res;
}

//-----------------------------------------------------------------------------
// doItemsStateTransition
//
// - try to transit the item states to the target item state
//
// returns:
//		TRUE  - transit all items to target state
//		FALSE - error during state change
//
HRESULT SODaCComGroupImpl::doItemsStateTransition(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	SOCmnString errDescr;
	DWORD errCode = S_OK;
	SOCmnString itemErrDescr;
	DWORD itemErrCode = S_OK;
	SOCmnList<SOCmnElement> list(pDaCGroup->getLeafList());
	SOCmnListPosition pos;
	SODaCItem* item;
	DWORD* targetStates = NULL;
	DWORD* addItemsParTargetStates = NULL;
	SODaCItem** itemsStart;
	DWORD itemState;
	DWORD numItems = list.getCount();
	OPCITEMDEF* addItemsPar = NULL;
	OPCHANDLE* removeItemsPar = NULL;
	OPCHANDLE* setActivePar = NULL;
	OPCHANDLE* setInactivePar = NULL;
	OPCHANDLE* setClientHandleC = NULL;
	OPCHANDLE* setClientHandleS = NULL;
	OPCITEMRESULT* results = NULL;
	HRESULT* errors = NULL;
	DWORD iA, iR, iSI, iSA, iSC, iTS;
	DWORD i;
	int function;
	BOOL called, isCalledItem;
	BOOL ok = TRUE;
	BOOL performInitialRead = FALSE;
	SOCmnList<SODaCItem> readList;
	SOCmnList<SODaCItem> updateList;
	SOCmnList<SODaCItem> setDBList;
	setDBList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

	if (numItems == 0)
	{
		return S_OK;
	}

	SOCmnPointer<SODaCServer> server = (SODaCServer*)pDaCGroup->getParentBranch();
	if (server.isNotNull())
	{
		if (pDaCGroup->getSingleItemTransactionHandle() != 0)
		{
			SODaCItem* pSingleItem = (SODaCItem*)GET_MEMORY_ADDRESS(pDaCGroup->getSingleItemTransactionHandle());

			if (pSingleItem)
			{
				numItems = 1;
				list.detach();
				list.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
				list.add(pSingleItem);
			}
		}

		performInitialRead = server->getPerformInitialRead();

		if (performInitialRead)
		{
			addItemsParTargetStates = (DWORD*) malloc(numItems * sizeof(DWORD));

			if (!addItemsParTargetStates)
			{
				pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
				return E_OUTOFMEMORY;
			}
		}
	}

	targetStates = (DWORD*) malloc(numItems * sizeof(DWORD));
	itemsStart = (SODaCItem**) malloc(numItems * sizeof(SODaCItem*));
	//-- allocate the parameter structures --
	setInactivePar = (OPCHANDLE*) malloc(numItems * sizeof(OPCHANDLE));

	if (!setInactivePar)
	{
		pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	setActivePar = (OPCHANDLE*) malloc(numItems * sizeof(OPCHANDLE));

	if (!setActivePar)
	{
		if (addItemsParTargetStates)
		{
			free(addItemsParTargetStates);
		}

		if (setInactivePar)
		{
			free(setInactivePar);
		}

		pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	if (pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup)
	{
		addItemsPar = (OPCITEMDEF*) malloc(numItems * sizeof(OPCITEMDEF));

		if (!addItemsPar)
		{
			if (addItemsParTargetStates)
			{
				free(addItemsParTargetStates);
			}

			if (setActivePar)
			{
				free(setActivePar);
			}

			if (setInactivePar)
			{
				free(setInactivePar);
			}

			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		removeItemsPar = (OPCHANDLE*) malloc(numItems * sizeof(OPCHANDLE));

		if (!removeItemsPar)
		{
			if (addItemsParTargetStates)
			{
				free(addItemsParTargetStates);
			}

			if (setActivePar)
			{
				free(setActivePar);
			}

			if (setInactivePar)
			{
				free(setInactivePar);
			}

			if (addItemsPar)
			{
				free(addItemsPar);
			}

			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		setClientHandleC = (OPCHANDLE*) malloc(numItems * sizeof(OPCHANDLE));
		setClientHandleS = (OPCHANDLE*) malloc(numItems * sizeof(OPCHANDLE));

		if ((!setClientHandleC) || (!setClientHandleS))
		{
			if (addItemsParTargetStates)
			{
				free(addItemsParTargetStates);
			}

			if (setActivePar)
			{
				free(setActivePar);
			}

			if (setInactivePar)
			{
				free(setInactivePar);
			}

			if (setClientHandleC)
			{
				free(setClientHandleC);
			}

			if (setClientHandleS)
			{
				free(setClientHandleS);
			}

			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}
	}

	//-- fill the parameter structures --
	iA = 0;
	iSI = 0;
	iSA = 0;
	iSC = 0;
	iR = 0;
	iTS = 0;
	pos = list.getStartPosition();

	while (pos)
	{
		item = (SODaCItem*)list.getNext(pos);
		itemsStart[iTS] = item;
		itemsStart[iTS]->addRef();
		targetStates[iTS] = item->getTargetObjectState();
		itemState = item->getObjectState();

		if (targetStates[iTS] != itemState)
		{
			itemsStart[iTS]->setStateTransitionActive(TRUE);

			// item has the same target state and hasn't reached it.
			switch (itemState)
			{
			case SODaCItem::disconnected:
			{
				if (pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup)
				{
					// must call AddItems to connect or start
					SOCmnString strAccessPath = item->getAccessPath();
					SOCmnString strItemID = item->getItemID();
					addItemsPar[iA].szAccessPath = strAccessPath.cloneToWCHAR(); // uses CoTaskMemAlloc, use CoTaskMemFree to release memory
					addItemsPar[iA].szItemID = strItemID.cloneToWCHAR();         // uses CoTaskMemAlloc, use CoTaskMemFree to release memory
					if (performInitialRead == TRUE)
					{
						addItemsPar[iA].bActive = FALSE;
						addItemsParTargetStates[iA] = (targetStates[iTS] == SODaCItem::started) ? TRUE : FALSE;
					}
					else
					{
						addItemsPar[iA].bActive = (targetStates[iTS] == SODaCItem::started) ? TRUE : FALSE;
					}
					addItemsPar[iA].hClient = (OPCHANDLE)item->getOpcHandle();
					addItemsPar[iA].dwBlobSize = 0;
					addItemsPar[iA].pBlob = NULL;
					addItemsPar[iA].vtRequestedDataType = item->getReqDatatype();
					iA++;
				}
				else
				{
					setClientHandleC[iSC] = (OPCHANDLE)item->getOpcHandle();
					setClientHandleS[iSC] = item->getServerHandle();
					iSC++;

					if (targetStates[iTS] == SODaCItem::started)
					{
						setActivePar[iSA] = item->getServerHandle();
						iSA++;
					}
					else
					{
						setInactivePar[iSI] = item->getServerHandle();
						iSI++;
					}
				}

				if (item->getDeadBand() != SODACITEM_NO_DEADBAND)
				{
					setDBList.add(item);
				}
			}
			break;

			case SODaCItem::connected:
			{
				if (targetStates[iTS] == SODaCItem::disconnected)
				{
					if ((pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup) && (!pDaCGroup->m_isPublic))
					{
						removeItemsPar[iR] = item->getServerHandle();
						iR++;
					}
					else
					{
						setInactivePar[iSI] = item->getServerHandle();
						iSI++;
					}
				}
				else
				{
					setActivePar[iSA] = item->getServerHandle();
					iSA++;
				}
			}
			break;

			case SODaCItem::started:
			{
				if (targetStates[iTS] == SODaCItem::disconnected)
				{
					if ((pDaCGroup->m_connectionType == SODaCGroup::ctAddGroup) && (!pDaCGroup->m_isPublic))
					{
						removeItemsPar[iR] = item->getServerHandle();
						iR++;
					}
					else
					{
						setInactivePar[iSI] = item->getServerHandle();
						iSI++;
					}
				}
				else
				{
					setInactivePar[iSI] = item->getServerHandle();
					iSI++;
				}
			}
			break;
			}
		}

		iTS++;
	}

	list.autoUnlock();
	// Get interface pointer
	CComPtr<IOPCItemMgt> itemMgt;
	res = m_itemMgt.getInterface(&itemMgt);

	// Something to do ?

	if (SUCCEEDED(res) && ((iA) || (iSC) || (iR) || (iSA) || (iSI)))
	{
		readList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
		updateList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

		//-- call the functions --
		/*
		 * NOTE: Function numbers:
		 *			0 - AddItems
		 *			1 - SetClientHandles
		 *			2 - RemoveItems
		 *			3 - ActivateItems
		 *			4 - DeactivateItems
		 */
		for (function = 0; function < 5 ; function++)
		{
			called = FALSE;
			results = NULL;
			errors = NULL;

			if ((function == 0) && (iA))
			{
				pDaCGroup->beforeInterfaceCall();
				res = itemMgt->AddItems(iA, addItemsPar, &results, &errors);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::AddItems %lu [0x%X]"), iA, res));
				errCode = SODAC_ERR_GRP_ADD_ITEMS;
				itemErrCode = SODAC_ERR_ITM_ADD_ITEM;
				called = TRUE;
			}

			if ((function == 1) && (iSC))
			{
				pDaCGroup->beforeInterfaceCall();
				res = itemMgt->SetClientHandles(iSC, setClientHandleS, setClientHandleC, &errors);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::SetClientHandles %lu [0x%X]"), iSC, res));
				errCode = SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS;
				itemErrCode = SODAC_ERR_ITM_SET_CLIENTHANDLE;
				called = TRUE;
			}

			if ((function == 2) && (iR))
			{
				pDaCGroup->beforeInterfaceCall();
				res = itemMgt->RemoveItems(iR, removeItemsPar, &errors);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::RemoveItems %lu [0x%X]"), iR, res));
				errCode = SODAC_ERR_GRP_REMOVE_ITEMS;
				itemErrCode = SODAC_ERR_ITM_REMOVE_ITEM;
				called = TRUE;
			}

			if ((function == 3) && (iSA))
			{
				if (performInitialRead == TRUE)
				{
					iTS = 0;
					for (i = 0; i < numItems; ++i)
					{
						item = itemsStart[i];
						if (item->getServerHandle() == setActivePar[iTS])
						{
							readList.add(item);
							++iTS;
						}
					}

					//-- read data of activated items --
					if ((readList.getCount() > 0) && (pDaCGroup->performStateTransitionAction(SODAC_ITEM_ACTION_INITIAL_READ)))
					{
						if (pDaCGroup->getDataRetrieval() == SODaCGroup::drAsyncAdvise)
						{
							res = pDaCGroup->asyncRead(&readList, SODAC_MAXAGE_CACHE);
							ok &= SUCCEEDED(res);
						}
						else
						{
							res = pDaCGroup->syncRead(&readList, SODAC_MAXAGE_CACHE);
							ok &= SUCCEEDED(res);
						}
					}
					readList.destroy();
				}

				pDaCGroup->beforeInterfaceCall();
				res = itemMgt->SetActiveState(iSA, setActivePar, TRUE, &errors);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::SetActive %lu [0x%X]"), iSA, res));
				errCode = SODAC_ERR_GRP_ACTIVATE_ITEMS;
				itemErrCode = SODAC_ERR_ITM_ACTIVATE_ITEM;
				called = TRUE;
			}

			if ((function == 4) && (iSI))
			{
				pDaCGroup->beforeInterfaceCall();
				res = itemMgt->SetActiveState(iSI, setInactivePar, FALSE, &errors);
				pDaCGroup->afterInterfaceCall();
				_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::SetActive(FALSE) %lu [0x%X]"), iSA, res));
				errCode = SODAC_ERR_GRP_DEACTIVATE_ITEMS;
				itemErrCode = SODAC_ERR_ITM_DEACTIVATE_ITEM;
				called = TRUE;
			}

			if (called)
			{
				pDaCGroup->setLastError(res, errCode);

				if (SUCCEEDED(res))
				{
					i = 0;

					for (iTS = 0; iTS < numItems; iTS++)
					{
						isCalledItem = FALSE;
						item = itemsStart[iTS];

						if ((function == 0) && (i >= iA))
						{
							break;
						}

						if ((function == 1) && (i >= iSC))
						{
							break;
						}

						if ((function == 2) && (i >= iR))
						{
							break;
						}

						if ((function == 3) && (i >= iSA))
						{
							break;
						}

						if ((function == 4) && (i >= iSI))
						{
							break;
						}

						if ((function == 0) && ((OPCHANDLE)item->getOpcHandle() == addItemsPar[i].hClient))
						{
							isCalledItem = TRUE;
						}

						if ((function == 1) && ((OPCHANDLE)item->getOpcHandle() == setClientHandleC[i]))
						{
							isCalledItem = TRUE;
						}

						if ((function == 2) && (item->getServerHandle() == removeItemsPar[i]))
						{
							isCalledItem = TRUE;
						}

						if ((function == 3) && (item->getServerHandle() == setActivePar[i]))
						{
							isCalledItem = TRUE;
						}

						if ((function == 4) && (item->getServerHandle() == setInactivePar[i]))
						{
							isCalledItem = TRUE;
						}

						if (isCalledItem)
						{
							item->setLastError(errors[i], itemErrCode);

							if (SUCCEEDED(errors[i]))
							{
								if (function != 1)
								{
									if (item->getObjectState() == SODaCItem::disconnected)
									{
										updateList.add(item);
									}

									item->setObjectState((BYTE)targetStates[iTS]);
								}

								if (function == 0)
								{
									item->setServerHandle(results[i].hServer);
									item->setNativeDatatype(results[i].vtCanonicalDataType);
									item->setAccessRights(results[i].dwAccessRights);
									if (performInitialRead == TRUE)
									{
										if (addItemsParTargetStates[i] == TRUE)
										{
											setActivePar[iSA++] = item->getServerHandle();
										}
									}
								}
							}
							else
							{
								if (targetStates[iTS] == SODaCItem::disconnected)
								{
									item->setObjectState(SODaCItem::disconnected);
								}

								ok = FALSE;
							}

							switch (function)
							{
							case 0:
							{
								_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  AI  [%lu] %s (0x%8.8X): ServerHandle 0x%8.8X; Active:%d [0x%X]"), i, (LPCTSTR)item->getItemID(), (ptrdiff_t)item, results[i].hServer, addItemsPar[i].bActive, errors[i]));
								break;
							}

							case 1:
							{
								_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  SC  [%lu] %s (0x%8.8X): ServerHandle 0x%8.8X; [0x%X]"), i, (LPCTSTR)item->getItemID(), (ptrdiff_t)item, setClientHandleS[i], errors[i]));
								break;
							}

							case 2:
							{
								_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  RI  [%lu] %s (0x%8.8X): ServerHandle 0x%8.8X [0x%X]"), i, (LPCTSTR)item->getItemID(), (ptrdiff_t)item, removeItemsPar[i], errors[i]));
								item->setServerHandle(SODAS_ITEM_SERVERHANDLE_DISCONNECTED);
								break;
							}

							case 3:
							{
								_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  SA  [%lu] %s (0x%8.8X): ServerHandle 0x%8.8X; [0x%X]"), i, (LPCTSTR)item->getItemID(), (ptrdiff_t)item, setActivePar[i], errors[i]));
								break;
							}

							case 4:
							{
								_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("  SI  [%lu] %s (0x%8.8X): ServerHandle 0x%8.8X; [0x%X]"), i, (LPCTSTR)item->getItemID(), (ptrdiff_t)item, setInactivePar[i], errors[i]));
								break;
							}
							}

							i++;
						}
					}

					if (results)
					{
						m_malloc->Free(results);
					}

					if (errors)
					{
						m_malloc->Free(errors);
					}
				}
				else  // SUCCEEDED(res)
				{
					// set to 'disconnected' anyway
					for (iTS = 0; iTS < numItems; iTS++)
					{
						if (targetStates[iTS] == SODaCItem::disconnected)
						{
							itemsStart[iTS]->setObjectState(SODaCItem::disconnected);
						}
					}

					ok = FALSE;
				}
			}
		}

		list.autoUnlock();

		//-- set item deadband --
		if (setDBList.getCount() > 0)
		{
			res = pDaCGroup->updateItems(FALSE, &setDBList, SODAC_ITMPAR_DEADBAND);
			ok &= SUCCEEDED(res);
		}

		//-- update parameters of items --
		if ((updateList.getCount() > 0) && (pDaCGroup->performStateTransitionAction(SODAC_ITEM_ACTION_GET_ITEM_PARS)))
		{
			if (!m_itemDeadbandMgt)
			{
				if (pDaCGroup->getSingleItemTransactionHandle() != 0)
				{
					// No need to read requested data type, item just created...
					// Should there be an invalid requested data type,. the server would return error message OPC_E_BADTYPE
					// and item would not have been added!
					// Instead, we simply request EU only.
					res = updateItem(pDaCGroup->getSingleItemTransactionHandle(), SODAC_ITMPAR_ALL_MGT);
				}
				else
				{
					res = pDaCGroup->updateItems(TRUE, &updateList, SODAC_ITMPAR_ALL_MGT);
				}
			}
			else
			{
				if (pDaCGroup->getSingleItemTransactionHandle() != 0)
				{
					// No need to read requested data type, item just created...
					// Should there be an invalid requested data type,. the server would return error message OPC_E_BADTYPE
					// and item would not have been added!
					// Instead, we simply request EU and DeadBand.
					res = updateItem(pDaCGroup->getSingleItemTransactionHandle(), SODAC_ITMPAR_ALL);
					res = pDaCGroup->updateItems(TRUE, &updateList, SODAC_ITMPAR_ALL_DEADBAND);
				}
				else
				{
					res = pDaCGroup->updateItems(TRUE, &updateList, SODAC_ITMPAR_ALL);
				}
			}

			ok &= SUCCEEDED(res);
		}

		updateList.destroy();
	}

	//-- free all --
	if (addItemsPar)
	{
		for (i = 0; i < iA; i++)
		{
			CoTaskMemFree(addItemsPar[i].szAccessPath);
			CoTaskMemFree(addItemsPar[i].szItemID);
		}

		free(addItemsPar);
	}

	for (iTS = 0; iTS < numItems; iTS++)
	{
		itemsStart[iTS]->setStateTransitionActive(FALSE); // reset all even if not all have been set
		itemsStart[iTS]->release();
	}

	if (itemsStart)
	{
		free(itemsStart);
	}

	if (targetStates)
	{
		free(targetStates);
	}

	if (removeItemsPar)
	{
		free(removeItemsPar);
	}

	if (setActivePar)
	{
		free(setActivePar);
	}

	if (setInactivePar)
	{
		free(setInactivePar);
	}

	if (setClientHandleC)
	{
		free(setClientHandleC);
	}

	if (setClientHandleS)
	{
		free(setClientHandleS);
	}

	if (addItemsParTargetStates)
	{
		free(addItemsParTargetStates);
	}

	return ok ? S_OK : S_FALSE;
} // doItemsStateTransition


//-----------------------------------------------------------------------------
// createItems
//
// - create item objects for all items of the OPC Group object. This method
//   is used to create the item objects for groups created by getPublicGroupByName
//   and getGroupByName
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but no items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::createItems(void)
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	CComPtr<IEnumOPCItemAttributes> enumItemAttr;
	HRESULT res = S_OK;
	//-- get interface pointer --
	CComPtr<IOPCItemMgt> itemMgt;

	if (FAILED(res = m_itemMgt.getInterface(&itemMgt)))
	{
		return res;
	}

	//-- get the attributes of all items and create the item objects --
	pDaCGroup->beforeInterfaceCall();
	res = itemMgt->CreateEnumerator(IID_IEnumOPCItemAttributes, (LPUNKNOWN*)&enumItemAttr);
	pDaCGroup->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::CreateEnumerator [0x%X]"), res));
	pDaCGroup->setLastError(res, SODAC_ERR_GRP_CREATE_ITEMENUM);

	if (SUCCEEDED(res))
	{
		if (res == S_OK)
		{
			// there are items to enumerate
			SOCmnPointer<SODaCItem> item;
			OPCITEMATTRIBUTES* itemAttr = NULL;
			ULONG number;
			BOOL end = FALSE;
			DWORD i = 0;
			USES_CONVERSION;
			enumItemAttr->Reset();

			while ((!end) && (SUCCEEDED(enumItemAttr->Next(1, &itemAttr, &number))))
			{
				if (number == 0)
				{
					end = TRUE;

					if (itemAttr != NULL)
					{
						::CoTaskMemFree(itemAttr);
						itemAttr = NULL;
					}

					break;
				}

				item = insertItem(itemAttr);

				// free memory
				if (itemAttr->szItemID)
				{
					::CoTaskMemFree(itemAttr->szItemID);
				}

				if (itemAttr->szAccessPath)
				{
					::CoTaskMemFree(itemAttr->szAccessPath);
				}

				if (itemAttr->pBlob)
				{
					::CoTaskMemFree(itemAttr->pBlob);
				}

				::VariantClear(&itemAttr->vEUInfo);
				::CoTaskMemFree(itemAttr);
				itemAttr = NULL;
				i++;
			}
		}
	}

	return res;
} // createItems


//-----------------------------------------------------------------------------
// insertItem
//
// - create item object and insert it into the group leaf list
// - initialize the item with the parameter data
//
// returns:
//      pointer to item object
//
SODaCItem* SODaCComGroupImpl::insertItem(
	IN OPCITEMATTRIBUTES* itemAttr) // item attributes
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	USES_CONVERSION;
	_ASSERT_PTR(itemAttr);
	// create item
	SOCmnPointer<SODaCEntry> session = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = session->getCreator();
	SODaCItem* item = creator->createItem(pDaCGroup);
	// initialize item
	item->setItemID(W2CT((LPCWSTR)itemAttr->szItemID));
	item->setReqDatatype(itemAttr->vtRequestedDataType);
	item->setAccessPath(W2CT((LPCWSTR)itemAttr->szAccessPath));
	item->setServerHandle(itemAttr->hServer);
	item->setNativeDatatype(itemAttr->vtCanonicalDataType);
	item->setAccessRights(itemAttr->dwAccessRights);
	SOCmnVariant euInfo(&itemAttr->vEUInfo);
	item->setEU(itemAttr->dwEUType, euInfo);
	// add item to group
	pDaCGroup->addLeaf(item);
	return item;
} // insertItem


//-----------------------------------------------------------------------------
// fillServerHandles
//
// - filles server handles array with connected items of item list
//
// returns:
//		S_OK          - operation succeeded
//      E_OUTOFMEMORY - not enough memory
//
HRESULT SODaCComGroupImpl::fillServerHandles(
	IN SOCmnList<SODaCItem> *itemList,  // item list
	OUT DWORD* numItems,            // number of handles
	OUT OPCHANDLE** handles)        // handle array
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SODaCItem* item;
	SOCmnListPosition pos;
	DWORD i;
	*numItems = 0;
	*handles = NULL;
	// get number of items
	i = itemList->GetCount();

	if (i == 0)
	{
		return S_OK;
	}

	// allocate handle array
	*handles = (OPCHANDLE*) malloc((i) * sizeof(OPCHANDLE));

	if (!(*handles))
	{
		pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	itemList->lock();
	// fill handle array
	i = 0;
	pos = itemList->GetHeadPosition();

	while (pos)
	{
		item = itemList->GetNext(pos);

		if (item->isConnected())
		{
			(*handles)[i] = item->getServerHandle();
			i++;
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, item, (_T(" [%3.3lu] not connected"), i));
		}
	}

	itemList->unlock();
	// check handle array
	*numItems = i;

	if (i == 0)
	{
		free(*handles);
		*handles = NULL;
	}

	return S_OK;
} // fillServerHandles


//-----------------------------------------------------------------------------
// fillServerHandles
//
// - filles server handles array with connected items of item list
//
// returns:
//		S_OK          - operation succeeded
//      E_OUTOFMEMORY - not enough memory
//
HRESULT SODaCComGroupImpl::fillServerHandles(
	IN DWORD count,
	IN SODaCItem** itemArray, // item list
	OUT DWORD* numItems,            // number of handles
	OUT OPCHANDLE** handles) // handle array
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	*numItems = 0;

	if (count == 0)
	{
		*handles = NULL;
		return S_OK;
	}

	DWORD i;
	// allocate handle array
	*handles = (OPCHANDLE*) malloc((count) * sizeof(OPCHANDLE));

	if (!(*handles))
	{
		pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	// fill handle array
	for (i = 0; i < count; i++)
	{
		if (itemArray[i]->isConnected())
		{
			(*handles)[(*numItems)] = itemArray[i]->getServerHandle();
			(*numItems)++;
		}
	}

	// check handle array
	if (*numItems == 0)
	{
		free(*handles);
		*handles = NULL;
	}

	return S_OK;
} // fillServerHandles


//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the OPC Server
//
// returns:
//		S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::queryInterfaceImpl(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_NOINTERFACE;
	}

	if (!m_unknown)
	{
		return E_NOINTERFACE;
	}

	if (IID_IUnknown == riid)
	{
		res = m_unknown.getInterface((IUnknown**)interf);
	}
	else if (IID_IOPCItemMgt == riid)
	{
		res = m_itemMgt.getInterface((IOPCItemMgt**)interf);
	}
	else if (IID_IOPCSyncIO == riid)
	{
		res = m_syncIO.getInterface((IOPCSyncIO**)interf);
	}
	else if (IID_IOPCAsyncIO == riid)
	{
		res = m_asyncIO.getInterface((IOPCAsyncIO**)interf);
	}
	else if (IID_IOPCAsyncIO2 == riid)
	{
		res = m_asyncIO2.getInterface((IOPCAsyncIO2**)interf);
	}
	else if (IID_IOPCGroupStateMgt == riid)
	{
		res = m_grpState.getInterface((IOPCGroupStateMgt**)interf);
	}
	else if (IID_IOPCPublicGroupStateMgt == riid)
	{
		res = m_grpStatePub.getInterface((IOPCPublicGroupStateMgt**)interf);
	}
	else if (IID_IOPCSyncIO2 == riid)
	{
		res = m_syncIO2.getInterface((IOPCSyncIO2**)interf);
	}
	else if (IID_IOPCAsyncIO3 == riid)
	{
		res = m_asyncIO3.getInterface((IOPCAsyncIO3**)interf);
	}
	else if (IID_IOPCGroupStateMgt2 == riid)
	{
		res = m_grpState2.getInterface((IOPCGroupStateMgt2**)interf);
	}
	else if (IID_IOPCItemDeadbandMgt == riid)
	{
		res = m_itemDeadbandMgt.getInterface((IOPCItemDeadbandMgt**)interf);
	}
	else if (IID_IOPCItemSamplingMgt == riid)
	{
		res = m_itemSamplingMgt.getInterface((IOPCItemSamplingMgt**)interf);
	}
	else
	{
		CComPtr<IUnknown> pUnk;
		res = m_unknown.getInterface(&pUnk);

		if (SUCCEEDED(res))
		{
			pDaCGroup->beforeInterfaceCall();
			res = pUnk->QueryInterface(riid, (void**)interf);
			pDaCGroup->afterInterfaceCall();
		}

#ifndef _DEBUG
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IUnknown::QueryInterface [0x%X] "), res));
#else
		SOCmnString iidString;
		IID2string(&iidString, riid);
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IUnknown::QueryInterface %s [0x%X] "), (LPCTSTR)iidString, res));
#endif  //  _DEBUG
	}

	if (FAILED(res))
	{
		SOCmnString iidString;
		SOCmnString resDescr;
		IID2string(&iidString, riid);

		if ((riid == IID_IOPCItemMgt) || (riid == IID_IOPCGroupStateMgt) || (riid == IID_IOPCSyncIO))
		{
			pDaCGroup->setLastError(res, SODAC_ERR_OPCDA_QI_REQ);
		}
		else if ((riid == IID_IOPCAsyncIO3) || (riid == IID_IOPCSyncIO2) || (riid == IID_IOPCGroupStateMgt2) || (riid == IID_IOPCItemDeadbandMgt))
		{
			pDaCGroup->setLastError(res, SODAC_ERR_OPCDA_QI_REQ_V30);
		}
		else if ((riid == IID_IOPCAsyncIO2) || (riid == IID_IConnectionPointContainer))
		{
			pDaCGroup->setLastError(res, SODAC_ERR_OPCDA_QI_REQ_V20);
		}
		else if ((riid == IID_IOPCAsyncIO) || (riid == IID_IDataObject))
		{
			pDaCGroup->setLastError(res, SODAC_ERR_OPCDA_QI_REQ_V10);
		}
		else if ((riid == IID_IOPCPublicGroupStateMgt) || (riid == IID_IOPCItemSamplingMgt))
		{
			pDaCGroup->setLastError(res, SODAC_ERR_OPCDA_QI_OPT);
		}
		else
		{
			pDaCGroup->setLastError(res, SOCLT_ERR_COM_QI);
		}
	}

	return res;
} // queryInterface


//-----------------------------------------------------------------------------
// update
//
// - update the group parameter
//
// returns:
//		S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::doUpdateImpl(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	HRESULT allRes = S_OK;
	DWORD updateRate;
	DWORD* pUpdateRate = NULL;
	LONG timeBias;
	LONG* pTimeBias = NULL;
	FLOAT deadband;
	FLOAT* pDeadband = NULL;
	DWORD lcid;
	DWORD* pLcid = NULL;
	OPCHANDLE serverHandle;
	OPCHANDLE* pServerHandle = NULL;
	OPCHANDLE clientHandle;
	OPCHANDLE* pClientHandle = NULL;
	BOOL active;
	BOOL* pActive = NULL;
	SOCmnString errDescr;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	CComPtr<IOPCGroupStateMgt> groupMgt;
	CComPtr<IOPCPublicGroupStateMgt> pubGroupMgt;
	CComPtr<IOPCGroupStateMgt2> grpState2;

	// get the COM interfaces
	if (whatParameters & SODAC_GRPPAR_ALL_MGT)
	{
		res = m_grpState.getInterface(&groupMgt);

		if (FAILED(res))
		{
			return res;
		}
	}

	if (whatParameters & SODAC_GRPPAR_ALL_PUBMGT)
	{
		res = m_grpStatePub.getInterface(&pubGroupMgt);

		if (FAILED(res))
		{
			return res;
		}
	}

	if (whatParameters & SODAC_GRPPAR_ALL_MGT2)
	{
		res = m_grpState2.getInterface(&grpState2);

		if (FAILED(res))
		{
			return res;
		}
	}

	// call function on OPC Server
	if (fromServer)
	{
		// read parameter from server
		if (whatParameters & SODAC_GRPPAR_ALL_MGT)
		{
			// group parameter
			LPWSTR name;
			LPWSTR* pName = NULL;
			// no update rate -> access violation in OPCProxy !!!
			// if (whatParameters & SODAC_GRPPAR_UPDATERATE)
			pUpdateRate = &updateRate;
			// no lcid -> access violation in OPCProxy !!!
			//if (whatParameters & SODAC_GRPPAR_LCID)
			pLcid = &lcid;
			// no time bias -> access violation in OPCProxy !!!
			//if (whatParameters & SODAC_GRPPAR_TIMEBIAS)
			pTimeBias = &timeBias;
			// no deadband -> access violation in OPCProxy !!!
			//if (whatParameters & SODAC_GRPPAR_DEADBAND)
			pDeadband = &deadband;
			// no name -> access violation in OPCProxy !!!
			// if (whatParameters & SODAC_GRPPAR_NAME)
			pName = &name;
			// no server handle -> access violation in OPCProxy !!!
			// if (whatParameters & SODAC_GRPPAR_SERVERHANDLE)
			pServerHandle = &serverHandle;
			pDaCGroup->beforeInterfaceCall();
			res = groupMgt->GetState(pUpdateRate, &active, pName, pTimeBias, pDeadband,
									 pLcid, &clientHandle, pServerHandle);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt::GetState [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_GET_STATE);

			if (SUCCEEDED(res))
			{
				DWORD validPars = pDaCGroup->m_validParameter;

				// set the read parameters
				if (pName)
				{
					USES_CONVERSION;
					pDaCGroup->setName(W2CT((LPCWSTR)name));
					m_malloc->Free(name);
				}

				if (pUpdateRate)
				{
					pDaCGroup->m_reqUpdateRate = *pUpdateRate;
					INVOKE_CALLBACK_ON_PTR(pDaCGroup,  SODaCGroup, onSetReqUpdateRate);
				}

				if (pTimeBias)
				{
					pDaCGroup->setTimeBias(*pTimeBias);
				}

				if (pDeadband)
				{
					if (pDaCGroup->setDeadBand(*pDeadband) == FALSE)
					{
						res = S_FALSE;
						_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt::GetState -> [0x%X] error while parsing received deadband: \"%f\""), res, *pDeadband));
						pDaCGroup->setLastError(res, SODAC_ERR_GRP_GET_DEADBAND);
					}
				}

				if (pLcid)
				{
					pDaCGroup->setLCID(*pLcid);
				}

				if (pServerHandle)
				{
					m_serverHandle = *pServerHandle;
				}

				pDaCGroup->m_validParameter = validPars;    // restore valid state

				if (pDaCGroup->getObjectState() == pDaCGroup->getTargetObjectState())
				{
					// state check
					if (clientHandle != (OPCHANDLE)pDaCGroup->getOpcHandle())
					{
						pDaCGroup->setLastError(res, SODAC_ERR_GRP_BAD_CLIENTHANDLE);
					}

					if (active != pDaCGroup->isStarted())
					{
						pDaCGroup->setLastError(res = E_FAIL, SODAC_ERR_GRP_WRONG_ACTMODE);
					}
				}
			}

			if (FAILED(res))
			{
				allRes = res;
			}
		}

		if (whatParameters & SODAC_GRPPAR_KEEPALIVETIME)
		{
			DWORD revKeepAliveTime;
			pDaCGroup->beforeInterfaceCall();
			res = grpState2->GetKeepAlive(&revKeepAliveTime);
			pDaCGroup->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt2::GetKeepAliveTime [0x%X] "), res));
			_TRACEO_SUCCEEDED(res, SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T(" OUT: *pdwKeepAliveTime: %lu"), revKeepAliveTime));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_GET_KEEPALIVE);

			if (SUCCEEDED(res))
			{
				pDaCGroup->m_revKeepAliveTime = revKeepAliveTime;
				INVOKE_CALLBACK_ON_PTR(pDaCGroup, SODaCGroup, onSetRevKeepAliveTime);
			}
		}

		if (whatParameters & SODAC_GRPPAR_ALL_PUBMGT)
		{
			// public group parameter
			BOOL pub;
			pDaCGroup->beforeInterfaceCall();
			res = pubGroupMgt->GetState(&pub);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCPublicGroupStateMgt::GetState [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_GET_STATE_PUB);

			if (SUCCEEDED(res))
			{
				// set the read parameters
				pDaCGroup->m_isPublic = (BYTE)pub;
				pDaCGroup->setPublic(pub);
			}
			else
			{
				allRes = res;
			}
		}
	}
	else  /////////////// to server //////////////////////////
	{
		// write parameter to server
		if (whatParameters & (SODAC_GRPPAR_ALL_MGT & ~SODAC_GRPPAR_NAME))
		{
			DWORD revUpdateRate;

			if (whatParameters & SODAC_GRPPAR_UPDATERATE)
			{
				pDaCGroup->getUpdateRate(&updateRate);
				pUpdateRate = &updateRate;
			}

			if (whatParameters & SODAC_GRPPAR_LCID)
			{
				lcid = pDaCGroup->getLCID();
				pLcid = &lcid;
			}

			if (whatParameters & SODAC_GRPPAR_TIMEBIAS)
			{
				timeBias = pDaCGroup->getTimeBias();
				pTimeBias = &timeBias;
			}

			if (whatParameters & SODAC_GRPPAR_DEADBAND)
			{
				deadband = pDaCGroup->getDeadBand();
				pDeadband = &deadband;
			}

			if (whatParameters & SODAC_GRPPAR_ACTIVE)
			{
				active = m_active;
				pActive = &active;
			}

			if (whatParameters & SODAC_GRPPAR_CLIENTHANDLE)
			{
				clientHandle = (OPCHANDLE)pDaCGroup->getOpcHandle();
				pClientHandle = &clientHandle;
			}

			pDaCGroup->beforeInterfaceCall();
			res = groupMgt->SetState(pUpdateRate, &revUpdateRate, pActive, pTimeBias, pDeadband, pLcid, pClientHandle);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt::SetState [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_SET_STATE);

			if (SUCCEEDED(res))
			{
				if (pUpdateRate)
				{
					pDaCGroup->m_revUpdateRate = revUpdateRate;
					INVOKE_CALLBACK_ON_PTR(pDaCGroup,  SODaCGroup, onSetRevUpdateRate);
				}

				if (m_taskSyncRead)
				{
					SOCmnPointer<SODaCComTaskSyncRead> task =
						(SODaCComTaskSyncRead*)pDaCGroup->getWorkerThread()->getCyclicTask(m_taskSyncRead);

					if (!!task)
					{
						task->setCycle(pDaCGroup->m_revUpdateRate);
					}
				}
			}
			else
			{
				allRes = res;
			}
		}

		if (whatParameters & SODAC_GRPPAR_NAME)
		{
			// group name
			USES_CONVERSION;
			SOCmnString cname;
			cname = pDaCGroup->getName();
			pDaCGroup->beforeInterfaceCall();
			res = groupMgt->SetName(T2CW((LPCTSTR)cname));
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt::SetName [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_SET_NAME);

			if (FAILED(res))
			{
				allRes = res;
			}
		}

		if (whatParameters & SODAC_GRPPAR_KEEPALIVETIME)
		{
			DWORD revKeepAliveTime = 0;
			DWORD keepAlive;
			pDaCGroup->getKeepAliveTime(&keepAlive);
			pDaCGroup->beforeInterfaceCall();
			res = grpState2->SetKeepAlive(keepAlive, &revKeepAliveTime);
			pDaCGroup->afterInterfaceCall();
			_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCGroupStateMgt2::SetKeepAliveTime [0x%X] "), res));
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T(" IN : dwKeepAliveTime: %lu"), keepAlive));
			_TRACEO_SUCCEEDED(res, SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T(" OUT: *pdwRevisedKeepAliveTime: %lu"), revKeepAliveTime));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_SET_KEEPALIVE);

			if (SUCCEEDED(res))
			{
				pDaCGroup->m_revKeepAliveTime = revKeepAliveTime;
				INVOKE_CALLBACK_ON_PTR(pDaCGroup, SODaCGroup, onSetRevKeepAliveTime);
			}
		}

		if ((whatParameters & SODAC_GRPPAR_PUBLIC) &&
			(pDaCGroup->getPublic()) && (!pDaCGroup->m_isPublic))
		{
			// public group parameter
			pDaCGroup->beforeInterfaceCall();
			res = pubGroupMgt->MoveToPublic();
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCPublicGroupStateMgt::MoveToPublic [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_MOVE_TO_PUB);

			if (FAILED(res))
			{
				pDaCGroup->m_isPublic = TRUE;
			}
			else
			{
				allRes = res;
			}
		}
	}

	return allRes;
} // update


//-----------------------------------------------------------------------------
// syncRead
//
// - read item values synchronous
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors for specific
//                      items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::syncReadImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN DWORD maxAge,
	OUT OPTIONAL SOCmnVariant* pValues,
	OUT OPTIONAL WORD* pQualities,
	OUT OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	OPCHANDLE* itemHandles;
	HRESULT* errors;
	DWORD numItems;
	OPCITEMSTATE* iItemState;
	DWORD i, j;
	SODaCItem* pItem;
	HRESULT resFH = S_OK;
	SOCmnString trcHelpSZ;
	SOCmnString trcHelpSZ2;
	// get the COM interface
	CComPtr<IOPCSyncIO> syncIO;
	CComPtr<IOPCSyncIO2> syncIO2;
	res = m_syncIO2.getInterface(&syncIO2);

	if (FAILED(res))
	{
		res = m_syncIO.getInterface(&syncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	// get the server handles of the items
	if (FAILED(res = fillServerHandles(count, itemArray, &numItems, &itemHandles)))
	{
		return res;
	}

	if (numItems != count)
	{
		resFH =  S_FALSE;

		if (numItems == 0)
		{
			for (j = 0; j < count; j++)
			{
				itemArray[j]->setRead(E_FAIL, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}

			return S_FALSE;
		}

		for (j = 0, i = 0; j < count; j++, i++)
		{
			if (i < numItems)
			{
				if (itemArray[j]->getServerHandle() != itemHandles[i])
				{
					itemArray[j]->setRead(E_FAIL, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC); // not connected

					if (pErrors)
					{
						pErrors[j] = E_FAIL;
					}

					i--;
				}
			}
			else
			{
				itemArray[j]->setRead(E_FAIL, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}
		}
	}

	// call function on OPC Server
	if (!!m_syncIO2)
	{
		DWORD* iMaxAge = (DWORD*)malloc(numItems * sizeof(DWORD));
		VARIANT* iValues;
		WORD* iQualities;
		FILETIME* iTimeStamps;

		if (!iMaxAge)
		{
			free(itemHandles);
			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		for (i = 0; i < numItems; i++)
		{
			iMaxAge[i] = maxAge;
		}

		pDaCGroup->beforeInterfaceCall();
		res = syncIO2->ReadMaxAge(numItems, itemHandles, iMaxAge, &iValues, &iQualities, &iTimeStamps, &errors);
		pDaCGroup->afterInterfaceCall();
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_SYNC_READ);

		if (SUCCEEDED(res))
		{
			// set the read item iItemState
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				if (SUCCEEDED(errors[i]))
				{
					SOCmnVariant value(&iValues[i]);
					SOCmnDateTime timestamp(&iTimeStamps[i]);
					pItem->setRead(errors[i], &value, iQualities[i], &timestamp, SODACITEM_TRANSACTIONID_SYNC);
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T(" [%3.3lu] value:%s; quality:0x%X; timestamp:%s"), j, variantWithType2string(&trcHelpSZ, &iValues[i]), iQualities[i], filetime2string(&trcHelpSZ2, &iTimeStamps[i])));

					if (pValues)
					{
						pValues[j] = iValues[i];
					}

					if (pQualities)
					{
						pQualities[j] = iQualities[i];
					}

					if (pTimestamps)
					{
						pTimestamps[j] = iTimeStamps[i];
					}

					if (pErrors)
					{
						pErrors[j] = errors[i];
					}

					::VariantClear(&iValues[i]);
				}
				else
				{
					pItem->setRead(errors[i], NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC);
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T(" [%3.3lu] [0x%8.8X]"), j, errors[i]));

					if (pErrors)
					{
						pErrors[j] = errors[i];
					}
				}
			}

			m_malloc->Free(iValues);
			m_malloc->Free(iQualities);
			m_malloc->Free(iTimeStamps);
			m_malloc->Free(errors);
		}
		else
		{
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setRead(res, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC);
			}
		}

		_TRACEO((res == S_OK) ? SOCMNTRACE_L_INF : SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCSyncIO2::ReadMaxAge [0x%X]"), res));
		free(iMaxAge);
	}
	else
	{
		pDaCGroup->beforeInterfaceCall();
		res = syncIO->Read((maxAge == SODAC_MAXAGE_CACHE) ? OPC_DS_CACHE : OPC_DS_DEVICE, numItems, itemHandles, &iItemState, &errors);
		pDaCGroup->afterInterfaceCall();
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_SYNC_READ);

		if (SUCCEEDED(res))
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("syncRead on %lu items"), numItems));

			// set the read item iItemState
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				if (SUCCEEDED(errors[i]))
				{
					SOCmnVariant value(&iItemState[i].vDataValue);
					SOCmnDateTime timestamp(&iItemState[i].ftTimeStamp);
					pItem->setRead(errors[i], &value, iItemState[i].wQuality, &timestamp, SODACITEM_TRANSACTIONID_SYNC);
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T(" [%3.3lu] value:%s; quality:0x%X; timestamp:%s"), j, variantWithType2string(&trcHelpSZ, &iItemState[i].vDataValue), iItemState[i].wQuality, filetime2string(&trcHelpSZ2, &iItemState[i].ftTimeStamp)));

					if (pValues)
					{
						pValues[j] = iItemState[i].vDataValue;
					}

					if (pQualities)
					{
						pQualities[j] = iItemState[i].wQuality;
					}

					if (pTimestamps)
					{
						pTimestamps[j] = iItemState[i].ftTimeStamp;
					}

					if (pErrors)
					{
						pErrors[j] = errors[i];
					}

					::VariantClear(&iItemState[i].vDataValue);
				}
				else
				{
					pItem->setRead(errors[i], NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC);
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T(" [%3.3lu] [0x%8.8X]"), j, errors[i]));

					if (pErrors)
					{
						pErrors[j] = errors[i];
					}
				}
			}

			m_malloc->Free(iItemState);
			m_malloc->Free(errors);
		}
		else
		{
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setRead(res, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_SYNC);
			}
		}

		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCSyncIO::Read [0x%X]"), res));
	}

	onSyncRead(count, itemArray, maxAge, res);
	free(itemHandles);

	if ((res == S_OK) && (resFH != S_OK))
	{
		res = resFH;
	}

	return res;
} // syncRead



//-----------------------------------------------------------------------------
// syncWrite
//
// - write item values synchronous
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors for specific
//                      items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::syncWriteImpl(
	IN DWORD count,
	IN SODaCItem** itemArray,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	OPCHANDLE* itemHandles;
	HRESULT* errors;
	DWORD numItems;
	DWORD i, j;
	SODaCItem* pItem;
	HRESULT resFH = S_OK;
	// get the COM interface
	CComPtr<IOPCSyncIO> syncIO;
	CComPtr<IOPCSyncIO2> syncIO2;
	res = m_syncIO2.getInterface(&syncIO2);

	if (FAILED(res))
	{
		res = m_syncIO.getInterface(&syncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	// get the server itemHandles of the items
	if (FAILED(res = fillServerHandles(count, itemArray, &numItems, &itemHandles)))
	{
		return res;
	}

	if (numItems != count)
	{
		resFH =  S_FALSE;

		if (numItems == 0)
		{
			for (j = 0; j < count; j++)
			{
				itemArray[j]->setWrite(E_FAIL, SODACITEM_TRANSACTIONID_SYNC); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}

			return S_FALSE;
		}

		for (j = 0, i = 0; j < count; j++, i++)
		{
			if (i < numItems)
			{
				if (itemArray[j]->getServerHandle() != itemHandles[i])
				{
					itemArray[j]->setWrite(E_FAIL, SODACITEM_TRANSACTIONID_SYNC); // not connected

					if (pErrors)
					{
						pErrors[j] = E_FAIL;
					}

					i--;
				}
			}
			else
			{
				itemArray[j]->setWrite(E_FAIL, SODACITEM_TRANSACTIONID_SYNC); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}
		}
	}

	// call function on OPC Server
	if (!!m_syncIO2)
	{
		// get the values to write
		OPCITEMVQT* iVQTs;
		iVQTs = (OPCITEMVQT*)malloc(numItems * sizeof(OPCITEMVQT));

		if (!(iVQTs))
		{
			free(itemHandles);
			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		memset(iVQTs, 0, (numItems * sizeof(OPCITEMVQT)));

		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if (pValues)
			{
				pValues[j].copyTo(&iVQTs[i].vDataValue);
			}
			else
			{
				pItem->getWriteValue(&iVQTs[i].vDataValue);
			}

			if (pQualities)
			{
				if (pQualities[j] != SODAC_QUALITY_NONE)
				{
					iVQTs[i].bQualitySpecified = TRUE;
					iVQTs[i].wQuality = pQualities[j];
				}
			}

			if (pTimestamps)
			{
				if (pTimestamps[j].isSet())
				{
					iVQTs[i].bTimeStampSpecified = TRUE;
					pTimestamps[j].get(&iVQTs[i].ftTimeStamp);
				}
			}
		}

		// call function on OPC Server
		pDaCGroup->beforeInterfaceCall();
		res = syncIO2->WriteVQT(numItems, itemHandles, iVQTs, &errors);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCSyncIO2::WriteVQT [0x%X]"), res));
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_SYNC_WRITE);

		if (SUCCEEDED(res))
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("syncWrite on %lu items"), numItems));

			// set the write item results
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setWrite(errors[i], SODACITEM_TRANSACTIONID_SYNC);

				if (pErrors)
				{
					pErrors[j] = errors[i];
				}
			}

			m_malloc->Free(errors);
		}
		else
		{
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setWrite(res, SODACITEM_TRANSACTIONID_SYNC);
			}
		}

		// free allocated memory
		for (i = 0; i < numItems; i++)
		{
			::VariantClear(&iVQTs[i].vDataValue);
		}

		free(iVQTs);
	}
	else
	{
		VARIANT* values;
		// get the values to write
		values = (VARIANT*) malloc((numItems) * sizeof(VARIANT));

		if (!(values))
		{
			free(itemHandles);
			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if (pValues)
			{
				pValues[j].copyTo(&values[i]);
			}
			else
			{
				pItem->getWriteValue(&values[i]);
			}
		}

		// call function on OPC Server
		pDaCGroup->beforeInterfaceCall();
		res = syncIO->Write(numItems, itemHandles, values, &errors);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCSyncIO::Write [0x%X]"), res));
		pDaCGroup->setLastError(res, SODAC_ERR_GRP_SYNC_WRITE);

		if (SUCCEEDED(res))
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("syncWrite on %lu items"), numItems));

			// set the write item results
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setWrite(errors[i], SODACITEM_TRANSACTIONID_SYNC);

				if (pErrors)
				{
					pErrors[j] = errors[i];
				}
			}

			m_malloc->Free(errors);
		}
		else
		{
			for (i = 0, j = 0; i < numItems; i++, j++)
			{
				pItem = itemArray[j];

				while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
				{
					j++;

					if (j < count)
					{
						pItem = itemArray[j];    // not connected item are not sent to the server
					}
				}

				if (j >= count)
				{
					break;
				}

				pItem->setWrite(res, SODACITEM_TRANSACTIONID_SYNC);
			}
		}

		// free allocated memory
		for (i = 0; i < numItems; i++)
		{
			::VariantClear(&values[i]);
		}

		free(values);
	}

	free(itemHandles);

	if ((res == S_OK) && (resFH != S_OK))
	{
		res = resFH;
	}

	return res;
} // syncWrite


//-----------------------------------------------------------------------------
// asyncRead
//
// - read item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but one or more errors
//                               for specific items
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCComGroupImpl::asyncReadImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD maxAge,
	IN OPTIONAL DWORD transactionID,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	OPCHANDLE* itemHandles;
	HRESULT* errors;
	DWORD numItems;
	DWORD i, j;
	DWORD canId;
	DWORD connectionId;
	SODaCItem* pItem;
	HRESULT resFH = S_OK;

	// check if advised
	if (!getReadConnection(&connectionId))
	{
		pDaCGroup->setLastError(CONNECT_E_NOCONNECTION, SODAC_ERR_GRP_NO_ASYNC_CON);
		return CONNECT_E_NOCONNECTION;
	}

	// get the COM interface
	CComPtr<IOPCAsyncIO> asyncIO;
	CComPtr<IOPCAsyncIO2> asyncIO2;
	CComPtr<IOPCAsyncIO3> asyncIO3;

	if (m_dataCallback)
	{
		res = m_asyncIO3.getInterface(&asyncIO3);

		if (FAILED(res))
		{
			res = m_asyncIO2.getInterface(&asyncIO2);
		}
	}
	else
	{
		res = m_asyncIO.getInterface(&asyncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	// get the server itemHandles of the items
	if (FAILED(res = fillServerHandles(count, itemArray, &numItems, &itemHandles)))
	{
		return res;
	}

	if (numItems != count)
	{
		resFH =  S_FALSE;

		if (numItems == 0)
		{
			for (j = 0; j < count; j++)
			{
				itemArray[j]->setRead(E_FAIL, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}

			return S_FALSE;
		}

		//for each item that doesn't have a server handle the result and the quality are set accordingly
		//the itemHandles array contains only the valid items from the itemArray
		i = 0;
		for (j = 0; j < count; j++)
		{
			if (itemArray[j]->getServerHandle() != itemHandles[i])
			{
				itemArray[j]->setRead(E_FAIL, NULL, OPC_QUALITY_BAD, NULL, SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}

			}
			else
			{
				i++;
			}
		}
	}

	if (!cancelID)
	{
		cancelID = &canId;
	}

	// call function on OPC Server
	if (m_dataCallback)
	{
		if (transactionID == 0)
		{
			transactionID = ::GetTickCount();

			if ((transactionID == 0) || (transactionID >= SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED))
			{
				transactionID = 1;
			}
		}

		if (!!asyncIO3)
		{
			DWORD* iMaxAge = (DWORD*)malloc(numItems * sizeof(DWORD));

			if (!iMaxAge)
			{
				free(itemHandles);
				pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
				return E_OUTOFMEMORY;
			}

			for (i = 0; i < numItems; i++)
			{
				iMaxAge[i] = maxAge;
			}

			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO3::ReadMaxAge ; tID: 0x%8.8X >>"), transactionID));
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO3->ReadMaxAge(numItems, itemHandles, iMaxAge, transactionID, cancelID, &errors);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO3::ReadMaxAge ; tID: 0x%8.8X [0x%X]"), transactionID, res));
			free(iMaxAge);
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Read ; tID: 0x%8.8X >>"), transactionID));
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO2->Read(numItems, itemHandles, transactionID, cancelID, &errors);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Read ; tID: 0x%8.8X [0x%X]"), transactionID, res));
		}
	}
	else
	{
		pDaCGroup->beforeInterfaceCall();
		res = asyncIO->Read(connectionId, (maxAge == SODAC_MAXAGE_CACHE) ? OPC_DS_CACHE : OPC_DS_DEVICE, numItems, itemHandles, cancelID, &errors);
		pDaCGroup->afterInterfaceCall();
		transactionID = *cancelID;
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO::Read [0x%X]"), res));
	}

	pDaCGroup->setLastError(res, SODAC_ERR_GRP_ASYNC_READ);

	if (SUCCEEDED(res))
	{
		// set the read result
		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if ((errors != NULL) && FAILED(errors[i]))
			{
				pItem->setRead(errors[i], NULL, OPC_QUALITY_BAD, NULL, transactionID);
				_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("asyncRead itemError [0x%x] hServer: %lu"), errors[j], itemHandles[j]));
			}

			if (pErrors != NULL && errors != NULL)
			{
				pErrors[j] = errors[i];
			}
		}
		if (errors != NULL)
		{
			m_malloc->Free(errors);
		}
	}
	else
	{
		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			pItem->setRead(res, NULL, OPC_QUALITY_BAD, NULL, transactionID);
		}
	}

	free(itemHandles);

	if ((res == S_OK) && (resFH != S_OK))
	{
		res = resFH;
	}

	return res;
} // asyncRead


//-----------------------------------------------------------------------------
// asyncWrite
//
// - write item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//		S_FALSE                - operation succeeded, but one or more errors
//                               for specific items
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no write advise connection
//
HRESULT SODaCComGroupImpl::asyncWriteImpl(
	IN DWORD count,
	IN OPTIONAL SODaCItem** itemArray,
	IN OPTIONAL DWORD transactionID,
	IN OPTIONAL SOCmnVariant* pValues,
	IN OPTIONAL WORD* pQualities,
	IN OPTIONAL SOCmnDateTime* pTimestamps,
	OUT OPTIONAL DWORD* cancelID,
	OUT OPTIONAL HRESULT* pErrors)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	OPCHANDLE* itemHandles;
	HRESULT* errors;
	DWORD numItems;
	DWORD i, j;
	DWORD canId;
	DWORD connectionId;
	SODaCItem* pItem;
	HRESULT resFH = S_OK;

	// check if advised
	if (!getWriteConnection(&connectionId))
	{
		pDaCGroup->setLastError(CONNECT_E_NOCONNECTION, SODAC_ERR_GRP_NO_ASYNC_CON);
		return CONNECT_E_NOCONNECTION;
	}

	// get the COM interface
	CComPtr<IOPCAsyncIO> asyncIO;
	CComPtr<IOPCAsyncIO2> asyncIO2;
	CComPtr<IOPCAsyncIO3> asyncIO3;

	if (m_dataCallback)
	{
		res = m_asyncIO3.getInterface(&asyncIO3);

		if (FAILED(res))
		{
			res = m_asyncIO2.getInterface(&asyncIO2);
		}
	}
	else
	{
		res = m_asyncIO.getInterface(&asyncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	// get the server itemHandles of the items
	if (FAILED(res = fillServerHandles(count, itemArray, &numItems, &itemHandles)))
	{
		return res;
	}

	if (numItems != count)
	{
		resFH =  S_FALSE;

		if (numItems == 0)
		{
			for (j = 0; j < count; j++)
			{
				itemArray[j]->setWrite(E_FAIL, SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}
			}

			return S_FALSE;
		}

		//for each item that doesn't have a server handle the result and the quality are set accordingly
		//the itemHandles array contains only the valid items from the itemArray
		i = 0;
		for (j = 0; j < count; j++)
		{
			if (itemArray[j]->getServerHandle() != itemHandles[i])
			{
				itemArray[j]->setWrite(E_FAIL, SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED); // not connected

				if (pErrors)
				{
					pErrors[j] = E_FAIL;
				}

			}
			else
			{
				i++;
			}
		}
	}

	// get the values to write
	OPCITEMVQT* iVQTs = NULL;
	VARIANT* values = NULL;

	if (!!asyncIO3)
	{
		// get the values to write
		iVQTs = (OPCITEMVQT*)malloc(numItems * sizeof(OPCITEMVQT));

		if (!(iVQTs))
		{
			free(itemHandles);
			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		memset(iVQTs, 0, (numItems * sizeof(OPCITEMVQT)));

		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if (pValues)
			{
				pValues[j].copyTo(&iVQTs[i].vDataValue);
			}
			else
			{
				pItem->getWriteValue(&iVQTs[i].vDataValue);
			}

			if (pQualities)
			{
				if (pQualities[j] != SODAC_QUALITY_NONE)
				{
					iVQTs[i].bQualitySpecified = TRUE;
					iVQTs[i].wQuality = pQualities[j];
				}
			}

			if (pTimestamps)
			{
				if (pTimestamps[j].isSet())
				{
					iVQTs[i].bTimeStampSpecified = TRUE;
					pTimestamps[j].get(&iVQTs[i].ftTimeStamp);
				}
			}
		}
	}
	else
	{
		values = (VARIANT*) malloc((numItems) * sizeof(VARIANT));

		if (!(values))
		{
			free(itemHandles);
			pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
			return E_OUTOFMEMORY;
		}

		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if (pValues)
			{
				pValues[j].copyTo(&values[i]);
			}
			else
			{
				pItem->getWriteValue(&values[i]);
			}
		}
	}

	if (!cancelID)
	{
		cancelID = &canId;
	}

	// call function on OPC Server
	if (m_dataCallback)
	{
		if (transactionID == 0)
		{
			transactionID = ::GetTickCount();

			if ((transactionID == 0) || (transactionID >= SODACITEM_TRANSACTIONID_ASYNC_NOT_STARTED))
			{
				transactionID = 1;
			}
		}

		if (!!asyncIO3)
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO3::WriteVQT ; tID: 0x%8.8X >>"), transactionID));
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO3->WriteVQT(numItems, itemHandles, iVQTs, transactionID, cancelID, &errors);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO3::WriteVQT ; tID: 0x%8.8X [0x%X]"), transactionID, res));
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Write ; tID: 0x%8.8X >>"), transactionID));
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO2->Write(numItems, itemHandles, values, transactionID, cancelID, &errors);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Write ; tID: 0x%8.8X [0x%X]"), transactionID, res));
		}
	}
	else
	{
		pDaCGroup->beforeInterfaceCall();
		res = asyncIO->Write(connectionId, numItems, itemHandles, values, cancelID, &errors);
		pDaCGroup->afterInterfaceCall();
		transactionID = *cancelID;
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO::Write[0x%X]"), res));
	}

	pDaCGroup->setLastError(res, SODAC_ERR_GRP_ASYNC_WRITE);

	if (SUCCEEDED(res))
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("asyncWrite on %lu items; cancelId: 0x%8.8X"), numItems, *cancelID));

		HRESULT errorHolder;
		// set the write result
		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			if (errors != NULL && FAILED(errors[i]))
			{
				pItem->setWrite(errors[i], transactionID);

				if (values)
				{
					_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("      [%lu] %s = %s ; itemHandle: 0x%8.8X [0x%8.8X]"), i, (LPCTSTR)pItem->getItemID(), VARIANT2STR(&values[i]), itemHandles[i], errors[i]));
				}
				else if (iVQTs)
				{
					_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("      [%lu] %s = %s ; itemHandle: 0x%8.8X [0x%8.8X]"), i, (LPCTSTR)pItem->getItemID(), VARIANT2STR(&iVQTs[i].vDataValue), itemHandles[i], errors[i]));
				}
			}
			else
			{
				if (errors != NULL)
				{
					errorHolder = errors[i];
				}
				else
				{
					if (res == S_OK)
					{
						pItem->setWrite(S_OK, transactionID);
						errorHolder = S_OK;
					}
					else
					{
						pItem->setWrite(E_POINTER, transactionID);
						errorHolder = E_POINTER;
					}
				}

				if (values)
				{
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("      [%lu] %s = %s ; itemHandle: 0x%8.8X [0x%8.8X]"), i, (LPCTSTR)pItem->getItemID(), VARIANT2STR(&values[i]), itemHandles[i], errorHolder));
				}
				else if (iVQTs)
				{
					_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pItem, (_T("      [%lu] %s = %s ; itemHandle: 0x%8.8X [0x%8.8X]"), i, (LPCTSTR)pItem->getItemID(), VARIANT2STR(&iVQTs[i].vDataValue), itemHandles[i], errorHolder));
				}
			}

			if (errors != NULL && pErrors != NULL)
			{
				pErrors[j] = errors[i];
			}
		}

		if (errors != NULL)
		{
			m_malloc->Free(errors);
		}
	}
	else
	{
		for (i = 0, j = 0; i < numItems; i++, j++)
		{
			pItem = itemArray[j];

			while ((j < count) && (pItem->getServerHandle() != itemHandles[i]))
			{
				j++;

				if (j < count)
				{
					pItem = itemArray[j];    // not connected item are not sent to the server
				}
			}

			if (j >= count)
			{
				break;
			}

			pItem->setWrite(res, transactionID);
		}
	}

	// free allocated memory
	if (iVQTs)
	{
		for (i = 0; i < numItems; i++)
		{
			::VariantClear(&iVQTs[i].vDataValue);
		}

		free(iVQTs);
	}

	if (values)
	{
		for (i = 0; i < numItems; i++)
		{
			::VariantClear(&values[i]);
		}

		free(values);
	}

	free(itemHandles);

	if ((res == S_OK) && (resFH != S_OK))
	{
		res = resFH;
	}

	return res;
} // asyncWrite


//-----------------------------------------------------------------------------
// asyncRefresh
//
// - refresh item values asynchronous
//
// returns:
//		S_OK                   - operation succeeded
//      E_FAIL                 - operation failed
//      E_INVALIDARG           - invalid argument
//      E_OUTOFMEMORY          - not enough memory
//      E_NOINTERFACE          - interface not supported
//		CONNECT_E_NOCONNECTION - no read advise connection
//
HRESULT SODaCComGroupImpl::asyncRefreshImpl(
	IN DWORD maxAge,            // data source
	IN OPTIONAL DWORD transactionID,    // transaction id (provided by client)
	OUT OPTIONAL DWORD* cancelID)       // cancel id (provided by server)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	DWORD canId;
	DWORD connectionId;

	// check if advised
	if (!getReadConnection(&connectionId))
	{
		pDaCGroup->setLastError(CONNECT_E_NOCONNECTION, SODAC_ERR_GRP_NO_ASYNC_CON);
		return CONNECT_E_NOCONNECTION;
	}

	// get the COM interface
	CComPtr<IOPCAsyncIO> asyncIO;
	CComPtr<IOPCAsyncIO2> asyncIO2;
	CComPtr<IOPCAsyncIO3> asyncIO3;

	if (m_dataCallback)
	{
		res = m_asyncIO3.getInterface(&asyncIO3);

		if (FAILED(res))
		{
			res = m_asyncIO2.getInterface(&asyncIO2);
		}
	}
	else
	{
		res = m_asyncIO.getInterface(&asyncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	if (!cancelID)
	{
		cancelID = &canId;
	}

	// call function on OPC Server
	if (m_dataCallback)
	{
		if (transactionID == 0)
		{
			transactionID = ::GetTickCount();

			if (transactionID == 0)
			{
				transactionID = 1;
			}
		}

		if (!!asyncIO3)
		{
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO3->RefreshMaxAge(maxAge, transactionID, cancelID);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO3::RefreshMaxAge[0x%X]"), res));
		}
		else
		{
			pDaCGroup->beforeInterfaceCall();
			res = asyncIO2->Refresh2((maxAge == SODAC_MAXAGE_CACHE) ? OPC_DS_CACHE : OPC_DS_DEVICE, transactionID, cancelID);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Refresh2[0x%X]"), res));
		}
	}
	else
	{
		pDaCGroup->beforeInterfaceCall();
		res = asyncIO->Refresh(connectionId, (maxAge == SODAC_MAXAGE_CACHE) ? OPC_DS_CACHE : OPC_DS_DEVICE, cancelID);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO::Refresh[0x%X]"), res));
	}

	pDaCGroup->setLastError(res, SODAC_ERR_GRP_ASYNC_REFRESH);
	return res;
} // asyncRefresh


//-----------------------------------------------------------------------------
// asyncCancel
//
// - cancel asynchronous request
//
// returns:
//		S_OK                   - operation succeeded
//      E_FAIL                 - operation failed
//      E_NOINTERFACE          - interface not supported
//
HRESULT SODaCComGroupImpl::asyncCancelImpl(
	IN DWORD cancelID)  // cancel id (provided by server)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;

	// check if advised
	if ((!getReadConnection(NULL)) && (!getWriteConnection(NULL)))
	{
		pDaCGroup->setLastError(CONNECT_E_NOCONNECTION, SODAC_ERR_GRP_NO_ASYNC_CON);
		return CONNECT_E_NOCONNECTION;
	}

	// get the COM interface
	CComPtr<IOPCAsyncIO> asyncIO;
	CComPtr<IOPCAsyncIO2> asyncIO2;
	CComPtr<IOPCAsyncIO3> asyncIO3;

	if (m_dataCallback)
	{
		res = m_asyncIO3.getInterface(&asyncIO3);

		if (FAILED(res))
		{
			res = m_asyncIO2.getInterface(&asyncIO2);
		}
	}
	else
	{
		res = m_asyncIO.getInterface(&asyncIO);
	}

	if (FAILED(res))
	{
		return res;
	}

	// call function on OPC Server
	if (m_dataCallback)
	{
		pDaCGroup->beforeInterfaceCall();
		res = asyncIO2->Cancel2(cancelID);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO2::Cancel2 [0x%X]"), res));
	}
	else
	{
		pDaCGroup->beforeInterfaceCall();
		res = asyncIO->Cancel(cancelID);
		pDaCGroup->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCAsyncIO::Cancel [0x%X]"), res));
	}

	pDaCGroup->setLastError(res, SODAC_ERR_GRP_ASYNC_CANCEL);
	return res;
} // asyncCancel


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
HRESULT SODaCComGroupImpl::advise(void)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	HRESULT res = E_FAIL;

	if ((!m_dataCallback) && (!m_adviseSink))
	{
		m_dataCallback = new SODaCDataCallback(this);
		res = m_dataCallback->advise();

		if (FAILED(res))
		{
			m_dataCallback->Release();
			m_dataCallback = NULL;
		}

		if ((res == E_NOINTERFACE) || (res == CONNECT_E_NOCONNECTION) ||
			(res == CONNECT_E_ADVISELIMIT) || (res == CONNECT_E_CANNOTCONNECT))
		{
			// only try DA1 on results which are generated if DA2 is not supported
			// IDataObject->DAdvise returns no access denied on DCOM configuration problems!
			m_adviseSink = new SODaCAdviseSink(this);

			if (FAILED(res = m_adviseSink->advise()))
			{
				m_adviseSink->Release();
				m_adviseSink = NULL;
			}
		}
	}
	else
	{
		if (m_dataCallback)
		{
			res = m_dataCallback->advise();
		}
		else
		{
			res = m_adviseSink->advise();
		}
	}

	return res;
} // advise


//-----------------------------------------------------------------------------
// unadvise
//
// - unadvise the shutdown event sink
//
// returns:
//		S_OK                   - connection unadvised or no connection was advised
//		E_NOINTERFACE          - IConnectionPointContainer interface not supported by server
//		CONNECT_E_NOCONNECTION - IOPCDataCallback is not supported by connection point container
//		CONNECT_E_ADVISELIMIT  - the connection point has already reached its limit of connections and cannot accept any more
//      E_INVALIDARG           - invalid argument
//		E_FAIL
//
HRESULT SODaCComGroupImpl::unadvise(void)
{
	HRESULT res = S_OK;

	if (m_dataCallback)
	{
		SODaCGroup* pDaCGroup = getObjectDaCGroup();
		res = m_dataCallback->unadvise(pDaCGroup->getConnectionMonitorStatus());
		m_dataCallback->Release();
		m_dataCallback = NULL;
	}

	if (m_adviseSink)
	{
		res = m_adviseSink->unadvise();
		m_adviseSink->Release();
		m_adviseSink = NULL;
	}

	return res;
} // unadvise


//-----------------------------------------------------------------------------
// getReadConnection
//
// - get the read connection advise handle
//
// returns:
//		TRUE  - read connection established
//		FALSE - no read connection established
//
BOOL SODaCComGroupImpl::getReadConnection(
	OUT DWORD* connection)  // connection id
{
	if (m_dataCallback)
	{
		if (connection)
		{
			*connection = m_dataCallback->getConnection();
		}

		return m_dataCallback->connectionEstablished();
	}

	if (m_adviseSink)
	{
		if (connection)
		{
			*connection = m_adviseSink->getReadConnection();
		}

		return m_adviseSink->readConnectionEstablished();
	}

	return 0;
} // getReadConnection


//-----------------------------------------------------------------------------
// getWriteConnection
//
// - get the write connection advise handle
//
// returns:
//		TRUE  - write connection established
//		FALSE - no write connection established
//
BOOL SODaCComGroupImpl::getWriteConnection(
	OUT DWORD* connection)  // connection id
{
	if (m_dataCallback)
	{
		if (connection)
		{
			*connection = m_dataCallback->getConnection();
		}

		return m_dataCallback->connectionEstablished();
	}

	if (m_adviseSink)
	{
		if (connection)
		{
			*connection = m_adviseSink->getWriteConnection();
		}

		return m_adviseSink->writeConnectionEstablished();
	}

	return 0;
} // getWriteConnection

//-----------------------------------------------------------------------------
// updateItem
//
// - send the requested datatypes of the items to the server
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors
//                      for specific items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::updateItem(
	IN DWORD singleItemHandle,
	IN DWORD whatParameters)
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	HRESULT res = S_OK;
	SODaCItem* pItem = (SODaCItem*)GET_MEMORY_ADDRESS(singleItemHandle);

	if (pItem == NULL)
	{
		return E_INVALIDARG;
	}
	
	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	SOCmnPointer<SODaCServer> srv = (SODaCServer*)pDaCGroup->getParentBranch();
	SOCmnList<SODaCPropertyData> propList;
	propList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
	WORD whatData = SODAC_PROPDATA_VALUE;

	if (whatParameters & SODAC_ITMPAR_EU)
	{
		SODaCPropertyData* pPropData = new SODaCPropertyData();
		pPropData->m_propertyID = 7; // EU_TYPE
		propList.add(pPropData);
		pPropData = new SODaCPropertyData();
		pPropData->m_propertyID = 8; // EU_INFO
		propList.add(pPropData);
		res = srv->getPropertiesData(pItem->getItemID(), pItem->getItemPath(), whatData, propList);
		if (res == S_OK)
		{
			OPCEUTYPE euType = OPC_NOENUM;
			SOCmnVariant euData;
			SOCmnListPosition pos = propList.getStartPosition();
			SODaCPropertyData *prop = NULL;
			while (pos)
			{
				prop = propList.getNext(pos);
				if (prop->m_propertyID == 7)
				{
					euType = (OPCEUTYPE)prop->m_value.iVal; // EU_TYPE is VT_I4
				}
				else
				{
					euData = prop->m_value; // EU_INFO is VT_BSTR | VT_ARRAY if EU_TYPE is "enumerated"
				}
			}
			pItem->setEU(euType, euData);
		}
	}
	return res;
} // updateItem

//-----------------------------------------------------------------------------
// updateItems
//
// - send the requested datatypes of the items to the server
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors
//                      for specific items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::updateItemsImpl(
	IN BOOL fromServer,                 // update data from server or to server
	IN SOCmnList<SODaCItem> *itemList,      // item list
	IN OPTIONAL DWORD whatParameters)   // update what item properties
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res = S_OK;
	SOCmnPointer<SODaCItem> item;
	SODaCItem* pItem;
	DWORD i;
	SOCmnListPosition pos;

	// check input parameter
	if (itemList == NULL)
	{
		pDaCGroup->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	CComPtr<IOPCItemMgt> itemMgt;
	CComPtr<IOPCItemDeadbandMgt> itemDeadband;

	if (whatParameters & SODAC_ITMPAR_ALL_MGT)
	{
		if (FAILED(res = m_itemMgt.getInterface(&itemMgt)))
		{
			return res;
		}
	}

	if (whatParameters & SODAC_ITMPAR_ALL_DEADBAND)
	{
		if (FAILED(res = m_itemDeadbandMgt.getInterface(&itemDeadband)))
		{
			return res;
		}
	}

	if (fromServer)
	{
		if (whatParameters & SODAC_ITMPAR_ALL_MGT)
		{
			CComPtr<IEnumOPCItemAttributes> enumItemAttr;
			//-- get the attributes of all items and create the item objects --
			pDaCGroup->beforeInterfaceCall();
			res = itemMgt->CreateEnumerator(IID_IEnumOPCItemAttributes, (LPUNKNOWN*)&enumItemAttr);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::CreateEnumerator [0x%X]"), res));

			if (SUCCEEDED(res))
			{
				if (res == S_OK)
				{
					// there are items to enumerate
					OPCITEMATTRIBUTES* itemAttr = NULL;
					ULONG number;
					BOOL end = FALSE;
					USES_CONVERSION;
					i = 0;
					enumItemAttr->Reset();

					while ((!end) && (SUCCEEDED(enumItemAttr->Next(1, &itemAttr, &number))))
					{
						if (number == 0)
						{
							end = TRUE;

							if (itemAttr != NULL)
							{
								::CoTaskMemFree(itemAttr);
								itemAttr = NULL;
							}

							break;
						}

						item = pDaCGroup->getItemByClientHandle(itemAttr->hClient);

						if (item.isNotNull())
						{
							if (whatParameters & SODAC_ITMPAR_REQDATATYPE)
							{
								item->setReqDatatype(itemAttr->vtRequestedDataType);
							}

							if (whatParameters & SODAC_ITMPAR_EU)
							{
								SOCmnVariant euInfo(itemAttr->vEUInfo);
								item->setEU(itemAttr->dwEUType, euInfo);
							}
						}

						// free memory
						if (itemAttr->szItemID)
						{
							CoTaskMemFree(itemAttr->szItemID);
						}

						if (itemAttr->szAccessPath)
						{
							CoTaskMemFree(itemAttr->szAccessPath);
						}

						if (itemAttr->pBlob)
						{
							CoTaskMemFree(itemAttr->pBlob);
						}

						::VariantClear(&itemAttr->vEUInfo);
						CoTaskMemFree(itemAttr);
						itemAttr = NULL;
						i++;
					}
				}
			}

			pDaCGroup->setLastError(res, SODAC_ERR_GRP_CREATE_ITEMENUM);
		}

		if (whatParameters & SODAC_ITMPAR_DEADBAND)
		{
			OPCHANDLE* handles;
			HRESULT* pErrors;
			DWORD numItems;
			FLOAT* pDeadband;

			// get the server handles of the items
			if (FAILED(res = fillServerHandles(itemList, &numItems, &handles)))
			{
				return res;
			}

			if (numItems == 0)
			{
				return S_OK;
			}

			pDaCGroup->beforeInterfaceCall();
			res = itemDeadband->GetItemDeadband(numItems, handles, &pDeadband, &pErrors);
			pDaCGroup->afterInterfaceCall();
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_ITM_GET_DEADBAND);

			if (SUCCEEDED(res))
			{
				itemList->lock();

				for (i = 0, pos = itemList->getStartPosition(), pItem = itemList->getNext(pos); i < numItems; i++, pItem = itemList->getNext(pos))
				{
					while ((pItem) && (pItem->getServerHandle() != handles[i]))
					{
						pItem = itemList->getNext(pos);    // not connected item are not sent to the server
					}

					if (!pItem)
					{
						break;
					}

					pItem->setLastError(pErrors[i], SODAC_ERR_GRP_ITM_GET_DEADBAND);

					if (SUCCEEDED(pErrors[i]))
					{
						pItem->setDeadBand(pDeadband[i]);
					}
				}

				itemList->unlock();
				m_malloc->Free(pDeadband);
				m_malloc->Free(pErrors);
			}

			free(handles);
		}
	}
	else
	{
		if (whatParameters & SODAC_ITMPAR_REQDATATYPE)
		{
			OPCHANDLE* handles;
			HRESULT* errors;
			DWORD numItems;
			VARTYPE* datatypes;

			// get the server handles of the items
			if (FAILED(res = fillServerHandles(itemList, &numItems, &handles)))
			{
				return res;
			}

			if (numItems == 0)
			{
				return S_OK;
			}

			// get the datatypes
			datatypes = (VARTYPE*) malloc((numItems) * sizeof(VARTYPE));

			if (!(datatypes))
			{
				free(handles);
				pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
				return E_OUTOFMEMORY;
			}

			itemList->lock();

			for (i = 0, pos = itemList->getStartPosition(), pItem = itemList->getNext(pos); i < numItems; i++, pItem = itemList->getNext(pos))
			{
				while ((pItem) && (pItem->getServerHandle() != handles[i]))
				{
					pItem = itemList->getNext(pos);    // not connected item are not sent to the server
				}

				if (!pItem)
				{
					break;
				}

				datatypes[i] = pItem->getReqDatatype();
			}

			itemList->unlock();
			// call function on OPC Server
			pDaCGroup->beforeInterfaceCall();
			res = itemMgt->SetDatatypes(numItems, handles, datatypes, &errors);
			pDaCGroup->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::SetDatatypes [0x%X]"), res));
			pDaCGroup->setLastError(res, SODAC_ERR_GRP_SET_ITEM_DATATYPES);

			if (SUCCEEDED(res))
			{
				_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("setDatatypes on %lu items"), numItems));

				for (i = 0, pos = itemList->getStartPosition(), pItem = itemList->getNext(pos); i < numItems; i++, pItem = itemList->getNext(pos))
				{
					while ((pItem) && (pItem->getServerHandle() != handles[i]))
					{
						pItem = itemList->getNext(pos);    // not connected item are not sent to the server
					}

					if (!pItem)
					{
						break;
					}

					pItem->setLastError(errors[i], SODAC_ERR_ITM_SET_DATATYPE);
				}

				itemList->unlock();
				m_malloc->Free(errors);
			}

			free(datatypes);
			free(handles);
		}

		if (whatParameters & SODAC_ITMPAR_DEADBAND)
		{
			OPCHANDLE* handles;
			HRESULT* pErrors;
			DWORD numItems;
			FLOAT* pDeadband;
			SOCmnList<SODaCItem> itemListClear;
			itemListClear.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
			SOCmnListPosition nextPos;
			SOCmnListPosition curPos = NULL;
			SOCmnListPosition prevPos = NULL;
			nextPos = itemList->getStartPosition();

			while (nextPos)
			{
				prevPos = curPos;
				curPos = nextPos;
				pItem = itemList->getNext(nextPos);

				if (pItem->getDeadBand() == SODACITEM_NO_DEADBAND)
				{
					itemListClear.add(pItem);
					itemList->removeAt(curPos, prevPos);
					curPos = prevPos;
				}
			}

			if (itemList->getCount() > 0)
			{
				// set deadband
				if (FAILED(res = fillServerHandles(itemList, &numItems, &handles)))
				{
					return res;
				}

				// get the deadband
				pDeadband = (FLOAT*) malloc((numItems) * sizeof(FLOAT));

				if (!(pDeadband))
				{
					free(handles);
					pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
					return E_OUTOFMEMORY;
				}

				itemList->lock();

				for (i = 0, pos = itemList->getStartPosition(), pItem = itemList->getNext(pos); i < numItems; i++, pItem = itemList->getNext(pos))
				{
					while ((pItem) && (pItem->getServerHandle() != handles[i]))
					{
						pItem = itemList->getNext(pos);    // not connected item are not sent to the server
					}

					if (!pItem)
					{
						break;
					}

					pDeadband[i] = pItem->getDeadBand();
				}

				itemList->unlock();
				pDaCGroup->beforeInterfaceCall();
				res = itemDeadband->SetItemDeadband(numItems, handles, pDeadband, &pErrors);
				pDaCGroup->afterInterfaceCall();
				pDaCGroup->setLastError(res, SODAC_ERR_GRP_ITM_SET_DEADBAND);

				if (SUCCEEDED(res))
				{
					itemList->lock();

					for (i = 0, pos = itemList->getStartPosition(), pItem = itemList->getNext(pos); i < numItems; i++, pItem = itemList->getNext(pos))
					{
						while ((pItem) && (pItem->getServerHandle() != handles[i]))
						{
							pItem = itemList->getNext(pos);    // not connected item are not sent to the server
						}

						if (!pItem)
						{
							break;
						}

						pItem->setLastError(pErrors[i], SODAC_ERR_GRP_ITM_SET_DEADBAND);
					}

					itemList->unlock();
					m_malloc->Free(pErrors);
				}

				free(handles);
			}

			if (itemListClear.getCount() > 0)
			{
				// clear deadband
				if (FAILED(res = fillServerHandles(&itemListClear, &numItems, &handles)))
				{
					return res;
				}

				pDaCGroup->beforeInterfaceCall();
				res = itemDeadband->ClearItemDeadband(numItems, handles, &pErrors);
				pDaCGroup->afterInterfaceCall();
				pDaCGroup->setLastError(res, SODAC_ERR_GRP_ITM_CLEAR_DEADBAND);

				if (SUCCEEDED(res))
				{
					itemListClear.lock();

					for (i = 0, pos = itemListClear.getStartPosition(), pItem = itemListClear.getNext(pos); i < numItems; i++, pItem = itemListClear.getNext(pos))
					{
						while ((pItem) && (pItem->getServerHandle() != handles[i]))
						{
							pItem = itemListClear.getNext(pos);    // not connected item are not sent to the server
						}

						if (!pItem)
						{
							break;
						}

						pItem->setLastError(pErrors[i], SODAC_ERR_GRP_ITM_CLEAR_DEADBAND);
					}

					itemListClear.unlock();
					m_malloc->Free(pErrors);
				}

				free(handles);
			}

			itemListClear.destroy();
		}
	}

	return res;
} // updateItems


//-----------------------------------------------------------------------------
// validateItems
//
// - let the OPC Server check the item parameters
//
// returns:
//		S_OK          - operation succeeded
//		S_FALSE       - operation succeeded, but one or more errors
//                      for specific items
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComGroupImpl::validateItems(
	IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors)    // item list
{
	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	SODaCGroup* pDaCGroup = getObjectDaCGroup();
	HRESULT res;
	HRESULT* errors;
	DWORD numItems;
	OPCITEMDEF* def;
	OPCITEMRESULT* result;
	SODaCItem* item;
	DWORD i;
	SOCmnListPosition pos;

	// check input parameter
	if (itemList == NULL)
	{
		pDaCGroup->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	CComPtr<IOPCItemMgt> itemMgt;

	if (FAILED(res = m_itemMgt.getInterface(&itemMgt)))
	{
		return res;
	}

	// allocate memory for input pararmeter
	numItems = itemList->GetCount();

	if (numItems == 0)
	{
		return S_OK;
	}

	def = (OPCITEMDEF*) malloc((numItems) * sizeof(OPCITEMDEF));

	if (!(def))
	{
		pDaCGroup->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	// get the item data
	i = 0;
	pos = itemList->GetHeadPosition();

	while (pos)
	{
		item = itemList->GetNext(pos);
		SOCmnString strAccessPath = item->getAccessPath();
		SOCmnString strItemID = item->getItemID();
		def[i].szAccessPath = strAccessPath.cloneToWCHAR(); // uses CoTaskMemAlloc, use CoTaskMemFree to release memory
		def[i].szItemID = strItemID.cloneToWCHAR();      // uses CoTaskMemAlloc, use CoTaskMemFree to release memory
		def[i].bActive = FALSE;
		def[i].hClient = (OPCHANDLE)item->getOpcHandle();
		def[i].dwBlobSize = 0;
		def[i].pBlob = NULL;
		def[i].vtRequestedDataType = item->getReqDatatype();
		i++;
	}

	// call function on OPC Server
	pDaCGroup->beforeInterfaceCall();
	res = itemMgt->ValidateItems(numItems, def, FALSE, &result, &errors);
	pDaCGroup->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("IOPCItemMgt::ValidateItems [0x%X]"), res));
	pDaCGroup->setLastError(res, SODAC_ERR_GRP_VALIDATE_ITEMS);

	if (SUCCEEDED(res))
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCGroup, (_T("validateItems on %lu items"), numItems));

		for (i = 0; i < numItems; i++)
		{
			if (pErrors)
			{
				pErrors[i] = errors[i];
			}

			item = (SODaCItem*)(void*)GET_MEMORY_ADDRESS(def[i].hClient);
			if (item)
			{
				item->setLastError(errors[i], SODAC_ERR_ITM_VALIDATE_ITEM);

				if (SUCCEEDED(errors[i]))
				{
					item->setNativeDatatype(result[i].vtCanonicalDataType);
					item->setAccessRights(result[i].dwAccessRights);
				}
			}
		}

		m_malloc->Free(errors);
		m_malloc->Free(result);
	}

	if (def)
	{
		for (i = 0; i < numItems; i++)
		{
			CoTaskMemFree(def[i].szAccessPath);
			CoTaskMemFree(def[i].szItemID);
		}

		free(def);
	}

	return res;
} // validateItems


//-----------------------------------------------------------------------------
// onSyncRead
//
// - called after sync read
// - default implementation: do nothing
//
void SODaCComGroupImpl::onSyncRead(
	IN DWORD /* count */,
	IN SODaCItem** /* itemArray */,
	IN DWORD /* maxAge */,
	IN HRESULT /* res */)                   // read result
{} // onSyncRead


//-----------------------------------------------------------------------------
// onReadAdvise
//
// - called after got read advise (OPC V1.0)
// - default implementation: do nothing
//
void SODaCComGroupImpl::onReadAdvise(
	IN DWORD /* transactionID */,
	IN BYTE* /* data */)    // advise data
{} // onReadAdvise


//-----------------------------------------------------------------------------
// onWriteAdvise
//
// - called after got write advise (OPC V1.0)
// - default implementation: do nothing
//
void SODaCComGroupImpl::onWriteAdvise(
	IN DWORD /* transactionID */,
	IN BYTE* /* data */)    // advise data
{} // onWriteAdvise


//-----------------------------------------------------------------------------
// onCancelAdvise
//
// - called after got cancel advise (OPC V2.0)
// - default implementation: do nothing
//
void SODaCComGroupImpl::onCancelAdvise(
	IN DWORD /* dwTransid */,       // transaction id
	IN OPCHANDLE /* hGroup */)  // group handle
{} // onCancelAdvise


//-----------------------------------------------------------------------------
// onDataChangeBegin
//
void SODaCComGroupImpl::onDataChangeBegin(void)
{
}

//-----------------------------------------------------------------------------
// onDataChangeEnd
//
void SODaCComGroupImpl::onDataChangeEnd(void)
{
}



//-----------------------------------------------------------------------------
// SODaCComTaskSyncRead                                                       |
//-----------------------------------------------------------------------------

SODaCComTaskSyncRead::SODaCComTaskSyncRead(
	IN SOCltElement* element, IN DWORD cycle)
	: SOCltCyclicTask(element, cycle)
{
}

//-----------------------------------------------------------------------------
//
// execute
void SODaCComTaskSyncRead::execute(void)
{
	SODaCGroup* pDaCGroup = (SODaCGroup*)(SOCltElement*)m_element;
	m_lastCall = ::GetTickCount();

	if (pDaCGroup->isStarted())
	{
		SOCmnList<SODaCItem> itemList;
		HRESULT res = S_OK;
		itemList = (SOCmnList<SODaCItem>*)pDaCGroup->getLeafList();
		SODaCItem** itemArray;
		DWORD count = pDaCGroup->itemListToArray(&itemList, TRUE, itemArray);

		if (count > 0)
		{

			SOCmnVariant* pValues = new SOCmnVariant[count];
			WORD* pQualities = new WORD[count];
			SOCmnDateTime* pTimestamps = new SOCmnDateTime[count];
			HRESULT* pErrors = new HRESULT[count];

			res = pDaCGroup->syncRead(count, itemArray, SODAC_MAXAGE_CACHE, pValues, pQualities, pTimestamps, pErrors);
			
			pDaCGroup->onReadAdvise(1, 0, res, res, count, itemArray, pValues, pQualities, pTimestamps, pErrors);
			
			delete [] pValues;
			delete [] pQualities;
			delete [] pTimestamps;
			delete [] pErrors;
		}

		pDaCGroup->freeItemArray(count, itemArray);
	}
}


//-----------------------------------------------------------------------------
// SODaCComGroupDefault                                                       |
//-----------------------------------------------------------------------------
SODaCComGroupDefault::SODaCComGroupDefault(void)
	: SODaCGroup()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCComGroupDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError

#endif // SOFEATURE_DCOM

#endif // SODAC
