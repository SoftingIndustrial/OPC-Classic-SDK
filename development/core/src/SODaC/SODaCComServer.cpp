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
//  Filename    : SODaCComServer.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Server object related classes                               |
//                - SODaCComServerImpl: proxy for OPC server object           |
//                - SODaCComServerImplDefault: default server class for creator
//                - SODaCPropertyData: item property data                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_DCOM

#include <opcda.h>
#include "SODaCComServer.h"
#include "SODaCServer.h"


//-----------------------------------------------------------------------------
// SODaCComServerImpl                                                             |
//-----------------------------------------------------------------------------

SODaCComServerImpl::SODaCComServerImpl()
{
	m_version = 0;
	m_forcedVersion = SODAC_VERSION_OPC_USEDEFAULT;
}

SODaCComServerImpl::~SODaCComServerImpl()
{
}


//-----------------------------------------------------------------------------
// doDisconnectImpl
//
// - do the disconnection of the server
//
// returns:
//      returns of unadvise
//
HRESULT SODaCComServerImpl::doDisconnectImpl(BOOL connectionMonitorStatus)
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = S_OK;

	// disconnect shutdown event sink
	if ((pCltComImpl) && (pCltComImpl->m_shutdown))
	{
		pCltComImpl->m_shutdown->unadvise(connectionMonitorStatus);
	}
	else
	{
		res = E_OUTOFMEMORY;
	}

	// release all server interfaces
	pCltComImpl->m_unknown = NULL;
	pCltComImpl->m_common = NULL;
	pCltComImpl->m_secPrivate = NULL;
	m_server = NULL;
	m_serverPub = NULL;
	m_browseSAS = NULL;
	m_properties = NULL;
	m_persistFile = NULL;
	m_itemIO = NULL;
	m_browse = NULL;
	if (pDaCServer)
	{
		pDaCServer->setObjectState(SOCltElement::disconnected);
	}
	else
	{
		res = E_OUTOFMEMORY;
	}
	return res;
} // doDisconnectImpl


//-----------------------------------------------------------------------------
// doStateTransitionImpl
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//      TRUE  - transit to new state
//      FALSE - error during state change
//
HRESULT SODaCComServerImpl::doStateTransitionImpl(void)
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = S_OK;
	BYTE targetState = pDaCServer->getTargetObjectState();

	while ((SUCCEEDED(res)) && (pDaCServer->getObjectState() != targetState))
	{
		switch (pDaCServer->getObjectState())
		{
		case SOCltElement::disconnected:
		{
			// disconnected
			//-- connect server --
			MULTI_QI mqi[10];
			COSERVERINFO sin, *sinptr;
			SOCmnString errDescr;
			USES_CONVERSION;

			// create OPC server object via COM
			if (!pDaCServer->m_nodeName.IsEmpty())
			{
				// remote server
				sinptr = &sin;
				sin.dwReserved1 = 0;
				sin.dwReserved2 = 0;
				sin.pwszName = T2W((LPTSTR)((LPCTSTR)pDaCServer->m_nodeName));
				sin.pAuthInfo = 0;
			}
			else
			{
				// local server, inproc server or inproc handler
				sinptr = 0;
			}

			// Use MultyQI to get all the available interfaces at once
			::ZeroMemory(mqi, sizeof(mqi));
			mqi[0].pIID = &IID_IUnknown;
			mqi[1].pIID = &IID_IOPCCommon;
			mqi[2].pIID = &IID_IOPCServer;
			mqi[3].pIID = &IID_IOPCServerPublicGroups;
			mqi[4].pIID = &IID_IOPCBrowseServerAddressSpace;
			mqi[5].pIID = &IID_IOPCItemProperties;
			mqi[6].pIID = &IID_IPersistFile;
			mqi[7].pIID = &IID_IOPCItemIO;
			mqi[8].pIID = &IID_IOPCBrowse;
			mqi[9].pIID = &IID_IOPCSecurityPrivate;
			pDaCServer->beforeInterfaceCall();
			res = ::CoCreateInstanceEx(pCltComImpl->m_clsid, NULL, pCltComImpl->m_clsctx, sinptr, _COUNTOF(mqi), mqi);
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("::CoCreateInstanceEx [0x%X]"), res));

			if (SUCCEEDED(res))
			{
				if (m_forcedVersion != SODAC_VERSION_OPC_USEDEFAULT)
				{
					switch (m_forcedVersion)
					{
					case SODAC_VERSION_OPC_DAV10A:
						if (mqi[1].pItf)
						{
							mqi[1].pItf->Release();
							mqi[1].pItf = NULL;
							mqi[1].hr = E_NOINTERFACE;
						}

						if (mqi[5].pItf)
						{
							mqi[5].pItf->Release();
							mqi[5].pItf = NULL;
							mqi[5].hr = E_NOINTERFACE;
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

						_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("Forced use of DA V1")));
						break;

					case SODAC_VERSION_OPC_DAV20:
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

						_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("Forced use of DA V2")));
						break;
					}
				}

				// created object in OPC server
				if (SUCCEEDED(mqi[0].hr))
				{
					pCltComImpl->m_unknown = (IUnknown*)mqi[0].pItf;
				}

				if (SUCCEEDED(mqi[1].hr))
				{
					pCltComImpl->m_common = (IOPCCommon*)mqi[1].pItf;

					if (FAILED(mqi[7].hr))
					{
						pDaCServer->setLastError(S_OK, SODAC_INF_SRV_VERSION_20);
						m_version = SODAC_VERSION_OPC_DAV20;
					}
				}
				else
				{
					pDaCServer->setLastError(res, SODAC_INF_SRV_VERSION_10A);
					m_version = SODAC_VERSION_OPC_DAV10A;
				}

				if (SUCCEEDED(mqi[2].hr))
				{
					m_server = (IOPCServer*)mqi[2].pItf;
				}

				if (SUCCEEDED(mqi[3].hr))
				{
					m_serverPub = (IOPCServerPublicGroups*)mqi[3].pItf;
				}

				if (SUCCEEDED(mqi[4].hr))
				{
					m_browseSAS = (IOPCBrowseServerAddressSpace*)mqi[4].pItf;
				}

				if (SUCCEEDED(mqi[5].hr))
				{
					m_properties = (IOPCItemProperties*)mqi[5].pItf;
				}

				if (SUCCEEDED(mqi[6].hr))
				{
					m_persistFile = (IPersistFile*)mqi[6].pItf;
				}

				if (SUCCEEDED(mqi[7].hr))
				{
					m_itemIO = (IOPCItemIO*)mqi[7].pItf;
					pDaCServer->setLastError(S_OK, SODAC_INF_SRV_VERSION_30);
					m_version = SODAC_VERSION_OPC_DAV30;
				}

				if (SUCCEEDED(mqi[8].hr))
				{
					m_browse = (IOPCBrowse*)mqi[8].pItf;
				}

				if (SUCCEEDED(mqi[9].hr))
				{
					pCltComImpl->m_secPrivate = (IOPCSecurityPrivate*)mqi[9].pItf;
				}

				// free MULTI_QI pointers
				for (int i = 0 ; i < _COUNTOF(mqi); i++)
				{
					if (SUCCEEDED(mqi[i].hr) && mqi[i].pItf)
					{
						mqi[i].pItf->Release();
					}

					mqi[i].pItf = NULL;
				}

				res = S_OK; // ignore not provided interfaces for return
				// logon
				pCltComImpl->logon();

				// additional OPC V2.0 work
				if (m_version >= SODAC_VERSION_OPC_DAV20)
				{
					// set valid object attributes
					if (pDaCServer->m_validParameter)
					{
						res = pDaCServer->doUpdate(FALSE, pDaCServer->m_validParameter);
					}

					if (SUCCEEDED(res) && pCltComImpl->m_shutdown)
					{
						// setup shutdown sink
						pCltComImpl->m_shutdown->advise();
					}
				}
			}

			pDaCServer->setLastError(res, SOCLT_ERR_CREATE_SERVER);

			if (SUCCEEDED(res))
			{
				pDaCServer->setObjectState(SOCltElement::connected);
				pDaCServer->setLastError(res, 0);
				res = S_OK;
			}
			else
			{
				// something failed -> release actions
				doDisconnectImpl(FALSE);
			}
		}
		break;

		case SOCltElement::connected:
		{
			if (targetState == SOCltElement::disconnected)
			{
				//-- disconnect server --
				res = doDisconnectImpl(FALSE);
			}
			else
			{
				// start server
				pDaCServer->setObjectState(SOCltElement::started);
				pDaCServer->setLastError(S_OK, 0);
			}
		}
		break;

		case SOCltElement::started:
		{
			// stop server
			pDaCServer->setObjectState(SOCltElement::connected);
			pDaCServer->setLastError(S_OK, 0);
		}
		break;
		}
	}

	return res;
} // doStateTransition

//-----------------------------------------------------------------------------
// queryInterface
//
// - query for a COM interface of the OPC Server
//
// returns:
//      S_OK          - interface returned
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComServerImpl::queryInterfaceImpl(
	IN REFIID riid,     // interface id
	IN void** interf)   // interface pointer
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_NOINTERFACE;
	}

	if (!pCltComImpl->m_unknown)
	{
		return E_NOINTERFACE;
	}

	if (IID_IUnknown == riid)
	{
		res = !pCltComImpl->m_unknown ? E_NOINTERFACE : pCltComImpl->m_unknown.getInterface((IUnknown**)interf);
	}
	else if (IID_IOPCCommon == riid)
	{
		res = !pCltComImpl->m_common ? E_NOINTERFACE : pCltComImpl->m_common.getInterface((IOPCCommon**)interf);
	}
	else if (IID_IOPCServer == riid)
	{
		res = !m_server ? E_NOINTERFACE : m_server.getInterface((IOPCServer**)interf);
	}
	else if (IID_IOPCServerPublicGroups == riid)
	{
		res = !m_serverPub ? E_NOINTERFACE : m_serverPub.getInterface((IOPCServerPublicGroups**)interf);
	}
	else if (IID_IOPCBrowseServerAddressSpace == riid)
	{
		res = !m_browseSAS ? E_NOINTERFACE : m_browseSAS.getInterface((IOPCBrowseServerAddressSpace**)interf);
	}
	else if (IID_IOPCItemProperties == riid)
	{
		res = !m_properties ? E_NOINTERFACE : m_properties.getInterface((IOPCItemProperties**)interf);
	}
	else if (IID_IPersistFile == riid)
	{
		res = !m_persistFile ? E_NOINTERFACE : m_persistFile.getInterface((IPersistFile**)interf);
	}
	else if (IID_IOPCItemIO == riid)
	{
		res = !m_itemIO ? E_NOINTERFACE : m_itemIO.getInterface((IOPCItemIO**)interf);
	}
	else if (IID_IOPCBrowse == riid)
	{
		res = !m_browse ? E_NOINTERFACE : m_browse.getInterface((IOPCBrowse**)interf);
	}
	else if (IID_IOPCSecurityPrivate == riid)
	{
		res = !pCltComImpl->m_secPrivate ? E_NOINTERFACE : pCltComImpl->m_secPrivate.getInterface((IOPCSecurityPrivate**)interf);
	}
	else
	{
		CComPtr<IUnknown> pUnk;
		res = pCltComImpl->m_unknown.getInterface(&pUnk);

		if (SUCCEEDED(res))
		{
			pDaCServer->beforeInterfaceCall();
			res = pUnk->QueryInterface(riid, (void**)interf);
			pDaCServer->afterInterfaceCall();
		}

#ifndef _DEBUG
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IUnknown::QueryInterface [0x%X] "), res));
#else
		SOCmnString iidString;
		IID2string(&iidString, riid);
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IUnknown::QueryInterface %s [0x%X] "), (LPCTSTR)iidString, res));
#endif  //  _DEBUG
	}

	if (FAILED(res))
	{
		SOCmnString iidString;
		SOCmnString resDescr;
		IID2string(&iidString, riid);

		if (riid == IID_IOPCServer)
		{
			pDaCServer->setLastError(res, SODAC_ERR_OPCDA_QI_REQ);
		}
		else if ((riid == IID_IOPCItemIO) || (riid == IID_IOPCBrowse))
		{
			pDaCServer->setLastError(res, SODAC_ERR_OPCDA_QI_REQ_V30);
		}
		else if ((riid == IID_IOPCItemProperties) || (riid == IID_IOPCCommon))
		{
			pDaCServer->setLastError(res, SODAC_ERR_OPCDA_QI_REQ_V20);
		}
		else if ((riid == IID_IOPCServerPublicGroups) || (riid == IID_IOPCBrowseServerAddressSpace))
		{
			pDaCServer->setLastError(res, SODAC_ERR_OPCDA_QI_OPT);
		}
		else
		{
			pDaCServer->setLastError(res, SOCLT_ERR_COM_QI);
		}
	}

	return res;
} // queryInterface



//-----------------------------------------------------------------------------
// update
//
// - update the server parameter
//
// returns:
//      S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_INVALIDARG  - invalid argument
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - interface not supported
//
HRESULT SODaCComServerImpl::doUpdateImpl(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = S_OK;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// get the COM interfaces
	if (!pCltComImpl->m_common)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCCommon> common;

	if (FAILED(res = pCltComImpl->m_common.getInterface(&common)))
	{
		return res;
	}

	if (fromServer)
	{
		if (whatParameters & SOCLT_SRVPAR_LCID)
		{
			// get locale id from the server
			LCID lcid;
			pDaCServer->beforeInterfaceCall();
			res = common->GetLocaleID(&lcid);
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCCommon::GetLocaleID [0x%X]"), res));
			pDaCServer->setLastError(res, SOCLT_ERR_GET_LOCALE_ID);

			if (SUCCEEDED(res))
			{
				DWORD validPars = pDaCServer->m_validParameter;
				pDaCServer->setLCID(lcid);
				pDaCServer->m_validParameter = validPars;
			}
		}
	}
	else
	{
		if (whatParameters & SOCLT_SRVPAR_LCID)
		{
			// set locale id to the server
			pDaCServer->beforeInterfaceCall();
			res = common->SetLocaleID(pDaCServer->m_lcid);
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCCommon::SetLocaleID [0x%X]"), res));
			pDaCServer->setLastError(res, SOCLT_ERR_SET_LOCALE_ID);
		}

		if (whatParameters & SOCLT_SRVPAR_CLIENT_NAME)
		{
			// set client name to the server
			USES_CONVERSION;
			SOCmnString cname;
			cname = pDaCServer->getClientName();
			pDaCServer->beforeInterfaceCall();
			res = common->SetClientName(T2CW((LPCTSTR)cname));
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCCommon::SetClientName [0x%X]"), res));
			pDaCServer->setLastError(res, SOCLT_ERR_SET_CLIENT_NAME);
		}
	}

	return res;
} // update


HRESULT SODaCComServerImpl::getStatus(OUT SODaCServerStatus& status)
{
	OPCSERVERSTATUS serverStatus;
	SOCmnString vendorInfo;
	HRESULT res;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	status.clear();
	res = getStatus(&serverStatus, &vendorInfo);

	if (SUCCEEDED(res))
	{
		status.m_vendorInfo = vendorInfo;
		status.m_state = serverStatus.dwServerState;
		status.m_productVersion.format(_T("%u.%2.2u.%4.4u"), serverStatus.wMajorVersion, serverStatus.wMinorVersion, serverStatus.wBuildNumber);
		status.m_currentTime.set(&serverStatus.ftCurrentTime);
		status.m_startTime.set(&serverStatus.ftStartTime);
		status.m_lastUpdateTime.set(&serverStatus.ftLastUpdateTime);
		status.m_bandwidth = serverStatus.dwBandWidth;
		status.m_groupCount = serverStatus.dwGroupCount;
	}

	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	pCltComImpl->queryAvailableLCIDs(status.m_supportedLCIDs);

	if ((pCltComImpl->m_common.isAvailable()) &&
		(m_server.isAvailable()) &&
		(m_itemIO.isAvailable()) &&
		(m_browse.isAvailable()))
	{
		status.m_supportedInterfaces.add(SODAC_VERSION_OPC_DAV30);
	}

	if ((pCltComImpl->m_common.isAvailable()) &&
		(m_server.isAvailable()) &&
		(m_properties.isAvailable()))
	{
		status.m_supportedInterfaces.add(SODAC_VERSION_OPC_DAV20);
	}

	if (m_server.isAvailable())
	{
		status.m_supportedInterfaces.add(SODAC_VERSION_OPC_DAV10A);
	}

	return res;
}


//-----------------------------------------------------------------------------
// getPublicGroupByName
//
// - insert group to the group list of the server. The group is connected by
//   calling GetPublicGroupByName of the IOPCServerPublicGroups interface
//
// returns:
//      pointer to the new group
//      NULL - error creating the group
//
SODaCGroup* SODaCComServerImpl::getPublicGroupByName(
	IN LPCTSTR groupName)   // group name
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnPointer<SODaCEntry> session = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = session->getCreator();
	SODaCGroup* pCGrp = creator->createGroup(SOCLT_PROTOCOL_DCOM, pDaCServer);
	pCGrp->init(pDaCServer);
	pCGrp->setName(groupName);
	pCGrp->setConnectionType(SODaCGroup::ctGetPublicGroupByName);
	pDaCServer->addBranch(pCGrp);
	return pCGrp;
} // getPublicGroupByName


//-----------------------------------------------------------------------------
// getStatus
//
// - get server status
//
// returns:
//      S_OK          - operation succeeded
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::getStatus(
	OUT OPCSERVERSTATUS* serverStatus,  // server status
	OUT SOCmnString* vendorInfo)            // vendor string
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	OPCSERVERSTATUS* status;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check output parameter
	if (serverStatus == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_OUTPUT);
		return E_INVALIDARG;
	}

	if (!m_server)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = server->GetStatus(&status);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCServer::GetStatus [0x%X]"), res));
	pDaCServer->setLastError(res, SOCLT_ERR_SRV_GET_STATUS);

	if (SUCCEEDED(res) && status != NULL)
	{
		USES_CONVERSION;
		// copy status
		memcpy(serverStatus, status, sizeof(OPCSERVERSTATUS));
		serverStatus->szVendorInfo = NULL;  // vendor info only in string

		// copy vendor info
		if (vendorInfo)
		{
			*vendorInfo = W2CT(status->szVendorInfo);
		}

		pCltComImpl->m_malloc->Free(status->szVendorInfo);
		pCltComImpl->m_malloc->Free(status);
	}

	return res;
} // getStatus


//-----------------------------------------------------------------------------
// queryOrganization
//
// - get type of the namespace of the server
//
// returns:
//      S_OK          - operation succeeded
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::queryOrganization(
	OUT OPCNAMESPACETYPE* nameSpaceType)    // namespace type
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check output parameter
	if (nameSpaceType == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	if (!m_browseSAS)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCBrowseServerAddressSpace> browse;

	if (FAILED(res = m_browseSAS.getInterface(&browse)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = browse->QueryOrganization(nameSpaceType);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowseServerAddressSpace::QueryOrganization [0x%X]"), res));
	pDaCServer->setLastError(res, SODAC_ERR_SRV_QUERY_ORG);

	if (SUCCEEDED(res))
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("queryOrganization: %lu"), *nameSpaceType));
	}

	return res;
} // queryOrganization


//-----------------------------------------------------------------------------
// changeBrowsePosition
//
// - changes the current browse position
//
// returns:
//      S_OK          - operation succeeded
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::changeBrowsePosition(
	IN OPCBROWSEDIRECTION direction,    // browse direction
	IN OPTIONAL LPCTSTR node)           // node name, only needed if browsing down
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	SOCmnString resDescr;
	SOCmnString string;
	LPWSTR str = NULL;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// get the COM interface
	if (!m_browseSAS)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCBrowseServerAddressSpace> browse;

	if (FAILED(res = m_browseSAS.getInterface(&browse)))
	{
		return res;
	}

	// copy the input parameters
	if (node)
	{
		string = node;
	}
	else
	{
		string = _T("");
	}

	str = T2W((LPTSTR)((LPCTSTR)string));
	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = browse->ChangeBrowsePosition(direction, str);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowseServerAddressSpace::ChangeBrowsePosition [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("changeBrowsePosition: %X %s"), direction, (LPCTSTR)string));
	}
	else
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_CHANGE_BRS_POS);
	}

	return res;
} // changeBrowsePosition


//-----------------------------------------------------------------------------
// browseOPCItemIDs
//
// - browse the elements of the current branch
//
// returns:
//      S_OK                - operation succeeded
//      S_FALSE             - nothing to enumerate
//      E_INVALIDARG        - invalid argument
//      E_FAIL              - operation failed
//      E_OUTOFMEMORY       - not enough memory
//      OPC_E_INVALIDFILTER - invalid string filter
//      E_NOINTERFACE       - can't get server interface
//
HRESULT SODaCComServerImpl::browseOPCItemIDs(
	IN OPCBROWSETYPE browseType,    // what to brose?
	IN LPCTSTR filterCriteria,      // string filter
	IN VARTYPE dataTypeFilter,      // datatype filter
	IN DWORD accessRightsFilter,    // access rights filter
	OUT SOCmnStringList& itemIDs)       // found element ids
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	SOCmnString string;
	LPWSTR str = NULL;
	IEnumString* enumString = NULL;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// copy the input parameters
	if (filterCriteria)
	{
		string = filterCriteria;
	}

	str = T2W((LPTSTR)((LPCTSTR)string));

	// get the COM interface
	if (!m_browseSAS)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCBrowseServerAddressSpace> browse;

	if (FAILED(res = m_browseSAS.getInterface(&browse)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = browse->BrowseOPCItemIDs(browseType, str, dataTypeFilter, accessRightsFilter, &enumString);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowseServerAddressSpace::BrowseOPCItemIDs [0x%X]"), res));
	pDaCServer->setLastError(res, SODAC_ERR_SRV_BRS_ITEMIDS);

	if (SUCCEEDED(res))
	{
		if (!itemIDs.isInit())
		{
			itemIDs.create();
		}

		enumString2stringList(enumString, &itemIDs);
		enumString->Release();
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("browseOPCItemIDs: items:%lu"), itemIDs.GetCount()));
	}

	return res;
} // browseOPCItemIDs


//-----------------------------------------------------------------------------
// getItemID
//
// - get full path of an item id
//
// returns:
//      item id string
//
SOCmnString SODaCComServerImpl::getItemID(
	IN LPCTSTR itemID,              // relative item id
	OUT OPTIONAL HRESULT* result)   // method result
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnString fullItemID = _T("");
	HRESULT res = E_FAIL;
	HRESULT* pRes;
	LPWSTR str = NULL;
	USES_CONVERSION;

	if (result)
	{
		pRes = result;
	}
	else
	{
		pRes = &res;
	}

	// check input parameter
	if (itemID == NULL)
	{
		*pRes = E_INVALIDARG;
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return fullItemID;
	}

	// get the COM interface
	if (!m_browseSAS)
	{
		pDaCServer->setLastError(E_NOINTERFACE, SOCLT_ERR_COM_QI);
		return _T("");
	}

	CComPtr<IOPCBrowseServerAddressSpace> browse;
	*pRes = m_browseSAS.getInterface(&browse);

	// call function on OPC Server
	if (SUCCEEDED(*pRes))
	{
		pDaCServer->beforeInterfaceCall();
		*pRes = browse->GetItemID(T2W((LPTSTR)itemID), &str);
		pDaCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowseServerAddressSpace::GetItemID [0x%X]"), res));
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("getItemID: %s %s"), itemID, (LPCTSTR)fullItemID));
	}

	if (SUCCEEDED(*pRes))
	{
		fullItemID = W2CT(str);
		pCltComImpl->m_malloc->Free(str);
	}
	else
	{
		pDaCServer->setLastError(*pRes, SODAC_ERR_SRV_GET_ITEMID);
	}

	return fullItemID;
} // getItemID


//-----------------------------------------------------------------------------
// browseAccessPaths
//
// - get the access paths of an item
//
// returns:
//      S_OK          - operation succeeded
//      S_FALSE       - nothing to enumerate
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::browseAccessPaths(
	IN LPCTSTR itemID,              // item id
	OUT SOCmnStringList& accessPaths)   // access paths list
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	IEnumString* enumString = NULL;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check input parameter
	if (itemID == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	if (!m_browseSAS)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IOPCBrowseServerAddressSpace> browse;

	if (FAILED(res = m_browseSAS.getInterface(&browse)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = browse->BrowseAccessPaths(T2CW(itemID), &enumString);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowseServerAddressSpace::BrowseAccessPaths [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		if (!accessPaths.isInit())
		{
			accessPaths.create();
		}

		if (res == S_OK)
		{
			enumString2stringList(enumString, &accessPaths);
			enumString->Release();
		}
		else
		{
			if (enumString)
			{
				enumString->Release();
			}

			accessPaths.removeAll();
		}

		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("browseAccessPaths: access paths:%lu"), accessPaths.GetCount()));
	}

	pDaCServer->setLastError(res, SODAC_ERR_SRV_BRS_ACCESS_PATHS);
	return res;
} // browseAccessPaths


//-----------------------------------------------------------------------------
// queryAvailableProperties
//
// - get the available properties of an item
//
// returns:
//      S_OK                - operation succeeded
//      E_INVALIDARG        - invalid argument
//      E_FAIL              - operation failed
//      E_OUTOFMEMORY       - not enough memory
//      E_NOINTERFACE       - can't get server interface
//      OPC_E_UNKNOWNITEMID - the item id is not in the servers address space
//      OPC_E_INVALIDITEMID - the item id is syntactically not valid
//
HRESULT SODaCComServerImpl::queryAvailableProperties(
	IN LPCTSTR itemID,                      // item id
	IN LPCTSTR /* itemPath */,
	IN WORD whatData,                           // get what property data?
	OUT SOCmnList<SODaCPropertyData> &propertyList) // property list
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	SOCmnString resDescr;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check input parameter
	if (itemID == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	if (getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
	{
		// get the COM interface
		if (!m_browse)
		{
			return E_NOINTERFACE;
		}

		CComPtr<IOPCBrowse> browse;

		if (FAILED(res = m_browse.getInterface(&browse)))
		{
			return res;
		}

		// call function on OPC Server
		LPWSTR wItemID = T2W((LPTSTR)itemID);
		BOOL returnValues = ((whatData & SODAC_PROPDATA_VALUE) == SODAC_PROPDATA_VALUE) ? TRUE : FALSE;
		OPCITEMPROPERTIES* pItemProperties = NULL;
		DWORD dummy = 0;
		pDaCServer->beforeInterfaceCall();
		res = browse->GetProperties(1, &wItemID, returnValues, 0, &dummy, &pItemProperties);
		pDaCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowse::GetProperties [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			DWORD i;

			if (!propertyList.isInit())
			{
				propertyList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			if (SUCCEEDED(pItemProperties->hrErrorID))
			{
				// fill the property list
				for (i = 0; i < pItemProperties->dwNumProperties; i++)
				{
					if (SUCCEEDED(pItemProperties->pItemProperties[i].hrErrorID))
					{
						SODaCPropertyData* propData = NULL;
						SOCMN_ALLOCATE_OBJECT(propData, SODaCPropertyData);
						propData->m_propertyID = pItemProperties->pItemProperties[i].dwPropertyID;
						propData->m_datatype = pItemProperties->pItemProperties[i].vtDataType;
						propData->m_description.fromWCHAR(pItemProperties->pItemProperties[i].szDescription);
						propData->m_itemID.fromWCHAR(pItemProperties->pItemProperties[i].szItemID);
						propData->m_error = pItemProperties->pItemProperties[i].hrErrorID;
						propData->m_value = pItemProperties->pItemProperties[i].vValue;
						propertyList.add(propData);
					}
					else
					{
						_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error set on get property element [0x%X]"), pItemProperties->pItemProperties[i].hrErrorID));
						pDaCServer->setLastError(pItemProperties->pItemProperties[i].hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
					}

					::CoTaskMemFree(pItemProperties->pItemProperties[i].szDescription);
					::CoTaskMemFree(pItemProperties->pItemProperties[i].szItemID);
					::VariantClear(&pItemProperties->pItemProperties[i].vValue);
				}
			}
			else
			{
				_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error set on get property [0x%X]"), pItemProperties->hrErrorID));
				pDaCServer->setLastError(pItemProperties->hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
				res = pItemProperties->hrErrorID;
			}
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error returned on get property [0x%X]"), res));
		}

		if (pItemProperties !=  NULL)
		{
			if (pItemProperties->pItemProperties)
			{
				::CoTaskMemFree(pItemProperties->pItemProperties);
			}

			::CoTaskMemFree(pItemProperties);
		}

		pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_PROPERTIES);
	}
	else if (getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20)
	{
		if (whatData != SODAC_PROPDATA_NONE)
		{
			return E_FAIL;
		}

		// get the COM interface
		if (!m_properties)
		{
			return E_NOINTERFACE;
		}

		CComPtr<IOPCItemProperties> properties;

		if (FAILED(res = m_properties.getInterface(&properties)))
		{
			return res;
		}

		// call function on OPC Server
		DWORD count;
		SOCltAutoMemory<DWORD> propertyIDs;
		SOCltAutoMemory<LPWSTR> wszDescriptions;
		SOCltAutoMemory<VARTYPE> datatypes;
		pDaCServer->beforeInterfaceCall();
		res = properties->QueryAvailableProperties(T2W((LPTSTR)itemID), &count, &propertyIDs,
												   &wszDescriptions, &datatypes);
		pDaCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCItemProperties::QueryAvailableProperties [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			DWORD i;

			if (!propertyList.isInit())
			{
				propertyList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			// fill the property list
			for (i = 0; i < count; i++)
			{
				SODaCPropertyData* propData = NULL;
				SOCMN_ALLOCATE_OBJECT(propData, SODaCPropertyData);
				propData->m_propertyID = propertyIDs[i];
				propData->m_datatype = datatypes[i];
				propData->m_description.fromWCHAR(wszDescriptions[i]);
				::CoTaskMemFree(wszDescriptions[i]);
				propertyList.add(propData);
			}

			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("queryAvailableProperties: count :%lu"), propertyList.GetCount()));
		}
	}
	else
	{
		res = E_FAIL;
	}

	pDaCServer->setLastError(res, SODAC_ERR_SRV_QUERY_PROPERTIES);
	return res;
} // queryAvailableProperties


//-----------------------------------------------------------------------------
// getPropertiesData
//
// - get the data of properties of an item
//
// returns:
//      S_OK                - operation succeeded
//      E_INVALIDARG        - invalid argument
//      E_FAIL              - operation failed
//      E_OUTOFMEMORY       - not enough memory
//      E_NOINTERFACE       - can't get server interface
//      OPC_E_UNKNOWNITEMID - the item id is not in the servers address space
//      OPC_E_INVALIDITEMID - the item id is syntactically not valid
//
HRESULT SODaCComServerImpl::getPropertiesData(
	IN LPCTSTR itemID,                          // item id
	IN LPCTSTR /* itemPath */,
	IN WORD whatData,                           // get what property data?
	IN OUT SOCmnList<SODaCPropertyData> &propertyList)  // property list
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = S_OK;
	SOCmnString resDescr;
	USES_CONVERSION;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check input parameter
	if (itemID == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	if (propertyList.GetCount() == 0)
	{
		pDaCServer->setLastError(S_OK, 0);
		return S_OK;
	}

	DWORD count = propertyList.GetCount();
	SOCmnListPosition pos;
	SODaCPropertyData* propertyData = NULL;
	DWORD i;
	// create input data for interface calls
	DWORD* propertyIDs = (DWORD*)_alloca(count * sizeof(DWORD));

	if (propertyIDs == NULL)
	{
		pDaCServer->setLastError(E_OUTOFMEMORY, SOCLT_ERR_OUT_OF_MEMORY);
		return E_OUTOFMEMORY;
	}

	pos = propertyList.GetHeadPosition();

	for (i = 0; ((i < count) && (pos)); i++)
	{
		propertyData = propertyList.GetNext(pos);
		propertyIDs[i] = propertyData->m_propertyID;
	}

	if (getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
	{
		// get the COM interface
		if (!m_browse)
		{
			return E_NOINTERFACE;
		}

		CComPtr<IOPCBrowse> browse;

		if (FAILED(res = m_browse.getInterface(&browse)))
		{
			return res;
		}

		// call function on OPC Server
		LPWSTR wItemID = T2W((LPTSTR)itemID);
		BOOL returnValues = ((whatData & SODAC_PROPDATA_VALUE) == SODAC_PROPDATA_VALUE) ? TRUE : FALSE;
		OPCITEMPROPERTIES* pItemProperties = NULL;
		pDaCServer->beforeInterfaceCall();
		res = browse->GetProperties(1, &wItemID, returnValues, count, propertyIDs, &pItemProperties);
		pDaCServer->afterInterfaceCall();
		_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCBrowse::GetProperties [0x%X]"), res));

		if (SUCCEEDED(res))
		{
			if (!propertyList.isInit())
			{
				propertyList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_DATA);
			}

			if (SUCCEEDED(pItemProperties->hrErrorID))
			{
				// fill the property list
				pos = propertyList.GetHeadPosition();

				for (i = 0; ((i < pItemProperties->dwNumProperties) && (pos)); i++)
				{
					propertyData = propertyList.GetNext(pos);

					if (SUCCEEDED(pItemProperties->hrErrorID))
					{
						if (pItemProperties->pItemProperties + i)
						{
							if (SUCCEEDED(pItemProperties->pItemProperties[i].hrErrorID))
							{
								propertyData->m_propertyID = pItemProperties->pItemProperties[i].dwPropertyID;
								propertyData->m_datatype = pItemProperties->pItemProperties[i].vtDataType;
								propertyData->m_description.fromWCHAR(pItemProperties->pItemProperties[i].szDescription);
								propertyData->m_itemID.fromWCHAR(pItemProperties->pItemProperties[i].szItemID);
								propertyData->m_error = pItemProperties->pItemProperties[i].hrErrorID;
								propertyData->m_value = pItemProperties->pItemProperties[i].vValue;
							}
							else
							{
								propertyData->m_error = E_FAIL;
								_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error set on get property element [0x%X]"), pItemProperties->pItemProperties[i].hrErrorID));
								pDaCServer->setLastError(pItemProperties->pItemProperties[i].hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
							}

							::CoTaskMemFree(pItemProperties->pItemProperties[i].szDescription);
							::CoTaskMemFree(pItemProperties->pItemProperties[i].szItemID);
							::VariantClear(&pItemProperties->pItemProperties[i].vValue);
						}
						else
						{
							propertyData->m_error = E_FAIL;
						}
					}
					else
					{
						propertyData->m_error = E_FAIL;
						_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error set on get property [0x%X]"), pItemProperties->hrErrorID));
						pDaCServer->setLastError(pItemProperties->hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
					}
				}
			}
			else
			{
				pos = propertyList.GetHeadPosition();
				while (pos)
				{
					propertyData = propertyList.GetNext(pos);
					propertyData->m_error = E_FAIL;
				}
				_TRACEO(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("error set on get property [0x%X]"), pItemProperties->hrErrorID));
				pDaCServer->setLastError(pItemProperties->hrErrorID, SODAC_ERR_SRV_GET_PROPERTIES);
				res = pItemProperties->hrErrorID;
			}

			pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_PROPERTIES);
		}

		if (pItemProperties != NULL)
		{
			if (pItemProperties->pItemProperties != NULL)
			{
				::CoTaskMemFree(pItemProperties->pItemProperties);
			}

			::CoTaskMemFree(pItemProperties);
		}
	}
	else if (getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20)
	{
		// get the COM interface
		if (!m_properties)
		{
			return E_NOINTERFACE;
		}

		CComPtr<IOPCItemProperties> properties;

		if (FAILED(res = m_properties.getInterface(&properties)))
		{
			return res;
		}

		if ((whatData & SODAC_PROPDATA_VALUE) == SODAC_PROPDATA_VALUE)
		{
			SOCltAutoMemory<VARIANT> values;
			SOCltAutoMemory<HRESULT> errors;
			pDaCServer->beforeInterfaceCall();
			res = properties->GetItemProperties(T2W((LPTSTR)itemID), count, propertyIDs,
												&values, &errors);
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCItemProperties::GetItemProperties [0x%X]"), res));

			// fill the property list with the values
			if (SUCCEEDED(res))
			{
				pos = propertyList.GetHeadPosition();

				for (i = 0; ((i < count) && (pos)); i++)
				{
					propertyData = propertyList.GetNext(pos);

					if ((res != S_OK) && (!errors))
					{
						propertyData->m_error = E_POINTER;
						res = E_POINTER;
					}
					else
					{
						if (!values)
						{
							propertyData->m_error = E_POINTER;
							res = E_POINTER;
						}
						else
						{
							if (!!errors)
							{
								propertyData->m_error = errors[i];

								if (SUCCEEDED(errors[i]))
								{
									propertyData->m_value = values[i];
								}
							}
							else
							{
								propertyData->m_error = S_OK;
								propertyData->m_value = values[i];
							}
						}
					}

					if (!!values)
					{
						::VariantClear(&values[i]);
					}
				}
			}

			pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_PROP_VALUE);
		}

		if ((whatData & SODAC_PROPDATA_ITEMID) == SODAC_PROPDATA_ITEMID)
		{
			SOCltAutoMemory<LPWSTR> propItemIDs;
			SOCltAutoMemory<HRESULT> errors;
			pDaCServer->beforeInterfaceCall();
			res = properties->LookupItemIDs(T2W((LPTSTR)itemID), count, propertyIDs,
											&propItemIDs, &errors);
			pDaCServer->afterInterfaceCall();
			_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCItemProperties::LookupItemIDs [0x%X]"), res));

			// fill the property list with the properties item ids
			if (SUCCEEDED(res))
			{
				pos = propertyList.GetHeadPosition();

				for (i = 0; ((i < count) && (pos)); i++)
				{
					propertyData = propertyList.GetNext(pos);

					if ((res != S_OK) && (!errors))
					{
						propertyData->m_error = E_POINTER;
						res = E_POINTER;
					}
					else
					{
						if (!propItemIDs)
						{
							propertyData->m_error = E_POINTER;
							res = E_POINTER;
						}
						else
						{
							if (!!errors)
							{
								propertyData->m_error = errors[i];

								if (SUCCEEDED(errors[i]))
								{
									propertyData->m_itemID.fromWCHAR(propItemIDs[i]);
								}
							}
							else
							{
								propertyData->m_error = S_OK;
								propertyData->m_itemID.fromWCHAR(propItemIDs[i]);
							}
						}
					}

					if (!!propItemIDs)
					{
						::CoTaskMemFree(propItemIDs[i]);
					}
				}
			}

			pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_PROP_ITEMID);
		}
	}

	if (res == S_OK)
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("getPropertiesData: count :%lu"), propertyList.GetCount()));
		pDaCServer->setLastError(res, 0);
	}

	return res;
} // getPropertiesData



HRESULT SODaCComServerImpl::read(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN SOCmnString* /* pItemPaths */,
	IN DWORD maxAge,
	OUT SOCmnVariant* pValues,
	OUT WORD* pQualities,
	OUT SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SODaCServer* pDaCServer = getObjectDaCServer();

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check parameters
	if ((!pItemIDs) || (!pValues))
	{
		return E_FAIL;
	}

	HRESULT res;
	CComPtr<IOPCItemIO> itemIO;

	// get the COM interface
	if (!m_itemIO)
	{
		return E_NOINTERFACE;
	}

	if (FAILED(res = m_itemIO.getInterface(&itemIO)))
	{
		return res;
	}

	// call function on OPC Server
	LPCWSTR* iItemIDs;
	DWORD* iMaxAge;
	VARIANT* iValues;
	WORD* iQualities;
	FILETIME* iTimeStamps;
	HRESULT* iErrors;
	DWORD i;
	iMaxAge = (DWORD*)malloc(count * sizeof(DWORD));
	iItemIDs = (LPCWSTR*)malloc(count * sizeof(LPCWSTR));

	if ((!iMaxAge) || (!iItemIDs))
	{
		if (iMaxAge)
		{
			free(iMaxAge);
		}

		if (iItemIDs)
		{
			free(iItemIDs);
		}

		return E_OUTOFMEMORY;
	}

	for (i = 0; i < count; i++)
	{
		iItemIDs[i] = pItemIDs[i].cloneToWCHAR();
		iMaxAge[i] = maxAge;
	}

	pDaCServer->beforeInterfaceCall();
	res = itemIO->Read(count, iItemIDs, iMaxAge, &iValues, &iQualities, &iTimeStamps, &iErrors);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCItemIO::Read [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		for (i = 0; i < count; i++)
		{
			pValues[i] = iValues[i];

			if (pQualities)
			{
				pQualities[i] = iQualities[i];
			}

			if (pTimestamps)
			{
				pTimestamps[i] = iTimeStamps[i];
			}

			if (pErrors)
			{
				pErrors[i] = iErrors[i];
			}

			::VariantClear(&iValues[i]);
		}

		CoTaskMemFree(iValues);
		CoTaskMemFree(iQualities);
		CoTaskMemFree(iTimeStamps);
		CoTaskMemFree(iErrors);
	}
	else
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_READ_ITEM_VALUE);
	}

	for (i = 0; i < count; i++)
	{
		CoTaskMemFree((void*)iItemIDs[i]);
	}

	if (iMaxAge)
	{
		free(iMaxAge);
	}

	if (iItemIDs)
	{
		free(iItemIDs);
	}

	return res;
} // read


HRESULT SODaCComServerImpl::write(
	IN DWORD count,
	IN SOCmnString* pItemIDs,
	IN SOCmnString* /*pItemPaths */,
	IN SOCmnVariant* pValues,
	IN WORD* pQualities,
	IN SOCmnDateTime* pTimestamps,
	OUT HRESULT* pErrors)
{
	SODaCServer* pDaCServer = getObjectDaCServer();

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check parameters
	if ((!pItemIDs) || (!pValues))
	{
		return E_FAIL;
	}

	HRESULT res;
	CComPtr<IOPCItemIO> itemIO;

	// get the COM interface
	if (!m_itemIO)
	{
		return E_NOINTERFACE;
	}

	if (FAILED(res = m_itemIO.getInterface(&itemIO)))
	{
		return res;
	}

	// call function on OPC Server
	LPCWSTR* iItemIDs;
	OPCITEMVQT* iVQTs;
	HRESULT* iErrors;
	DWORD i;
	iItemIDs = (LPCWSTR*)malloc(count * sizeof(LPCWSTR));
	iVQTs = (OPCITEMVQT*)malloc(count * sizeof(OPCITEMVQT));

	if ((!iVQTs) || (!iItemIDs))
	{
		if (iVQTs)
		{
			free(iVQTs);
		}

		if (iItemIDs)
		{
			free(iItemIDs);
		}

		return E_OUTOFMEMORY;
	}

	memset(iVQTs, 0, (count * sizeof(OPCITEMVQT)));

	for (i = 0; i < count; i++)
	{
		iItemIDs[i] = pItemIDs[i].cloneToWCHAR();
		pValues[i].copyTo(&iVQTs[i].vDataValue);

		if (pQualities)
		{
			if (pQualities[i] != SODAC_QUALITY_NONE)
			{
				iVQTs[i].bQualitySpecified = TRUE;
				iVQTs[i].wQuality = pQualities[i];
			}
		}

		if (pTimestamps)
		{
			if (pTimestamps[i].isSet())
			{
				iVQTs[i].bTimeStampSpecified = TRUE;
				pTimestamps[i].get(&iVQTs[i].ftTimeStamp);
			}
		}
	}

	pDaCServer->beforeInterfaceCall();
	res = itemIO->WriteVQT(count, iItemIDs, iVQTs, &iErrors);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCItemIO::Write [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		for (i = 0; i < count; i++)
			if (pErrors)
			{
				pErrors[i] = iErrors[i];
			}

		CoTaskMemFree(iErrors);
	}
	else
	{
		pDaCServer->setLastError(res, SODAC_ERR_SRV_WRITE_ITEM_VALUE);
	}

	for (i = 0; i < count; i++)
	{
		CoTaskMemFree((void*)iItemIDs[i]);
		::VariantClear(&iVQTs[i].vDataValue);
	}

	if (iVQTs)
	{
		free(iVQTs);
	}

	if (iItemIDs)
	{
		free(iItemIDs);
	}

	return res;
} // write


//-----------------------------------------------------------------------------
// isDirty
//
// - checks if the configuration has changed since last save
//
// returns:
//      TRUE  - configuration has changed since last save
//      FALSE - configuration has not changed since last save
//
BOOL SODaCComServerImpl::isDirty(void)
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	BOOL ret;

	// get the COM interface
	if (!m_persistFile)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IPersistFile> persistFile;

	if (FAILED(res = m_persistFile.getInterface(&persistFile)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = persistFile->IsDirty();
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IPersistFile::IsDirty [0x%X]"), res));
	ret = (res == S_OK);
	_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("isDirty: %s"), BOOL2STR(ret)));
	return ret;
} // isDirty


//-----------------------------------------------------------------------------
// load
//
// - load the configuration of the OPC Server
//
// returns:
//      S_OK                    - operation succeeded
//      E_INVALIDARG            - invalid argument
//      E_FAIL                  - operation failed
//      E_OUTOFMEMORY           - not enough memory
//      E_NOINTERFACE           - can't get server interface
//      OPC_E_INVALIDCONFIGFILE - invalid file format
//
HRESULT SODaCComServerImpl::load(
	IN LPCTSTR fileName,    // configuration file name
	IN DWORD mode)          // access mode
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnString resDescr;
	HRESULT res;
	USES_CONVERSION;

	// check input parameter
	if (fileName == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	if (!m_persistFile)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IPersistFile> persistFile;

	if (FAILED(res = m_persistFile.getInterface(&persistFile)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = persistFile->Load(T2CW(fileName), mode);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IPersistFile::Load [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("load: %s"), fileName));
	}
	else
	{
		resDescr.Format(_T("Can't load configuration file %s on server"), fileName);
	}

	pDaCServer->setLastError(res, SODAC_ERR_SRV_LOAD);
	return res;
} // load


//-----------------------------------------------------------------------------
// save
//
// - save the configuration of the OPC Server
//
// returns:
//      S_OK          - operation succeeded
//      E_FAIL        - operation failed
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::save(
	IN LPCTSTR fileName,    // new configuration file name
	IN BOOL remember)       // change configuration file name to saved
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCmnString resDescr;
	HRESULT res;
	LPWSTR str;
	USES_CONVERSION;

	// get the COM interface
	if (!m_persistFile)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IPersistFile> persistFile;

	if (FAILED(res = m_persistFile.getInterface(&persistFile)))
	{
		return res;
	}

	if (fileName == NULL)
	{
		str = NULL;
	}
	else
	{
		str = T2W((LPTSTR)fileName);
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = persistFile->Save(str, remember);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IPersistFile::Save [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		if (fileName)
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("save: %s %s"), fileName, BOOL2STR(remember)));
		}
		else
		{
			_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("save: %s"), BOOL2STR(remember)));
		}
	}

	pDaCServer->setLastError(res, SODAC_ERR_SRV_SAVE);
	return res;
} // save


//-----------------------------------------------------------------------------
// saveCompleted
//
// - complete saving of the configuration of the OPC Server
//
// returns:
//      S_OK          - operation succeeded
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::saveCompleted(
	IN LPCTSTR fileName)    // new configuration file name
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res;
	LPWSTR str;
	USES_CONVERSION;

	// get the COM interface
	if (!m_persistFile)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IPersistFile> persistFile;

	if (FAILED(res = m_persistFile.getInterface(&persistFile)))
	{
		return res;
	}

	if (fileName == NULL)
	{
		str = NULL;
	}
	else
	{
		str = T2W((LPTSTR)fileName);
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = persistFile->SaveCompleted(str);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IPersistFile::SaveCompleted [0x%X]"), res));
	pDaCServer->setLastError(res, SODAC_ERR_SRV_SAVE_COMPLETED);
	return res;
} // saveCompleted


//-----------------------------------------------------------------------------
// getCurFile
//
// - get the path of the current configuration file of the OPC Server
//
// returns:
//      S_OK          - operation succeeded
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_NOINTERFACE - can't get server interface
//      E_OUTOFMEMORY - not enough memory
//
HRESULT SODaCComServerImpl::getCurFile(
	OUT SOCmnString* fileName)  // file name
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();
	HRESULT res;
	LPWSTR str;

	// check output parameter
	if (fileName == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	if (!m_persistFile)
	{
		return E_NOINTERFACE;
	}

	CComPtr<IPersistFile> persistFile;

	if (FAILED(res = m_persistFile.getInterface(&persistFile)))
	{
		return res;
	}

	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = persistFile->GetCurFile(&str);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IPersistFile::GetCurFile [0x%X]"), res));

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		*fileName = W2CT(str);
		pCltComImpl->m_malloc->Free(str);
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("getCurFile: %s"), (LPCTSTR)*fileName));
	}

	pDaCServer->setLastError(res, SODAC_ERR_SRV_GET_CUR_FILE);
	return res;
} // getCurFile

//-----------------------------------------------------------------------------
// getErrorString
//
// - get error description from server
//
// returns:
//      S_OK          - operation succeeded
//      E_INVALIDARG  - invalid argument
//      E_FAIL        - operation failed
//      E_OUTOFMEMORY - not enough memory
//      E_NOINTERFACE - can't get server interface
//
HRESULT SODaCComServerImpl::getErrorString(
	IN HRESULT error,       // error code
	IN LCID locale,         // locale
	OUT SOCmnString* string)    // error text
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	HRESULT res = E_FAIL;

	if (SOCLT_COM_CHECK_DEMO)
	{
		return E_ACCESSDENIED;
	}

	// check output parameter
	if (string == NULL)
	{
		pDaCServer->setLastError(E_INVALIDARG, SOCLT_ERR_PAR_INPUT);
		return E_INVALIDARG;
	}

	// get the COM interface
	CComPtr<IOPCServer> server;

	if (FAILED(res = m_server.getInterface(&server)))
	{
		return res;
	}

	SOCltAutoMemory<WCHAR> wsz;
	// call function on OPC Server
	pDaCServer->beforeInterfaceCall();
	res = server->GetErrorString(error, locale, &wsz);
	pDaCServer->afterInterfaceCall();
	_TRACEO(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("IOPCServer::GetErrorString [0x%X]"), res));
	pDaCServer->setLastError(res, SOCLT_ERR_GET_ERROR_STRING);

	if (SUCCEEDED(res))
	{
		USES_CONVERSION;
		*string = W2CT(wsz);
		_TRACEO(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, pDaCServer, (_T("getErrorText: 0x%8.8X %s"), error, (LPCTSTR)*string));
	}

	return res;
} // getErrorString

BOOL SODaCComServerImpl::serializeImpl(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	SOCltComServerImpl* pCltComImpl = getObjectCltComServerImpl();

	if (isStoring)
	{
		ar << pCltComImpl->getClassID();
		ar << pCltComImpl->getClsctx();
	}
	else
	{
		CLSID clsid = CLSID_NULL;
		DWORD clsctx = 0;
		ar >> clsid;
		pCltComImpl->setClassID(clsid);
		ar >> clsctx;
		pCltComImpl->setClsctx(clsctx);
	}

	return TRUE;
}

SOCltComServerImpl* SODaCComServerImpl::getObjectCltComServerImpl(void)
{
	return NULL;
}

//-----------------------------------------------------------------------------
// getNamespaceBrowser
//  - creates a browser object
SODaCNameSpaceBrowser* SODaCComServerImpl::getNameSpaceBrowserImpl()
{
	SODaCServer* pDaCServer = getObjectDaCServer();
	SODaCComNameSpaceBrowser* p = 0;
	SOCMN_ALLOCATE_OBJECT(p, SODaCComNameSpaceBrowser(pDaCServer));
	return (SODaCNameSpaceBrowser*)p;
}

BOOL SODaCComServerImpl::forceOPCVersion(WORD opcVersion)
{
	switch (opcVersion)
	{
	case SODAC_VERSION_OPC_USEDEFAULT:
	case SODAC_VERSION_OPC_DAV10A:
	case SODAC_VERSION_OPC_DAV20:
	case SODAC_VERSION_OPC_DAV30:
		m_forcedVersion = opcVersion;
		return TRUE;
		break;

	default:
		return FALSE;
		break;
	}
}




//-----------------------------------------------------------------------------
// SODaCComServerDefault                                                      |
//-----------------------------------------------------------------------------

SODaCComServerDefault::SODaCComServerDefault(void)
	: SODaCServer()
{}


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//      object
//
SOCmnObject* SODaCComServerDefault::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


SOCltComServerImpl* SODaCComServerDefault::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}

#endif // SOFEATURE_DCOM

#endif // SODAC
