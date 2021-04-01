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
//  Filename    : SODaCServer.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Server object related classes                               |
//                - SODaCServer: proxy for OPC server object                  |
//                - SODaCServerDefault: default server class for creator      |
//                - SODaCPropertyData: item property data                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#include <opcda.h>
#include "SODaCServer.h"
#include "SODaCGroup.h"
#include "SODaCItem.h"
#include "SODaCEntry.h"
#include "SODaCCallback.h"
#include "SODaCBrowse.h"

//-----------------------------------------------------------------------------
// SODaCServer                                                                |
//-----------------------------------------------------------------------------

SODaCServer::SODaCServer(void)
	: SOCmnElementListBranch(0),
	  SOCltServer(SOCMNOBJECT_TYPE_SERVER_DA)
{
	// create branch map
	if (m_branchList.isInit())
	{
		m_branchList.destroy();
	}

	SOCmnListConfig cfg;
	cfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
	cfg.m_hashTableSize = 17;
	m_branchList.create(&cfg);
#ifdef SOFEATURE_SOAP
	m_xmlGetStatusTaskHandle = 0;
#endif
}

SODaCServer::~SODaCServer()
{
#ifdef SOFEATURE_SOAP
	if (m_xmlGetStatusTaskHandle != 0)
	{
		getWorkerThread()->removeCyclicTask(m_xmlGetStatusTaskHandle);
	}
#endif
}


//-----------------------------------------------------------------------------
// addLeaf
//
// - don't add leaf into server object
//
// returns:
//      FALSE - not added
//
BOOL SODaCServer::addLeaf(
	IN SOCmnElement* /* newLeaf */)
{
	return FALSE;
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a group object
//
// returns:
//      TRUE  - added
//      FALSE - not added
//
BOOL SODaCServer::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_GROUP))
	{
		m_branchList.doRehash(TRUE);
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// findBranch
//
// - find branch below the branch
//
// returns:
//		pointer to the branch - branch found
//		NULL - no branch found
//
SOCmnElement* SODaCServer::findBranch(
	IN LPCTSTR name,        // name of branch
	IN OPTIONAL BOOL deep)  // deep search or only direct child
{
	SOCmnElement* branch = NULL;
	SOCmnString path;
	LPTSTR child = NULL;
	LPTSTR remainingPath = NULL;

	if (!name)
	{
		return NULL;
	}

	path = name;
	reducePath(path, child, remainingPath, TRUE);   // slip path

	if ((!deep) && (remainingPath)) // no deep search and remaining path
	{
		return NULL;    // -> name can't be a direct child of this node
	}

	SOCmnList<SOCmnElement> branchList(getBranchList());

	if (!!branchList)
	{
		SOCmnPointer<SOCmnElement> search;
		SOCmnElement* pSearch;
		SOCmnListPosition pos;
		pos = branchList.getStartPosition();

		while (pos)
		{
			pSearch = branchList.getNext(pos);

			if (pSearch->hasKey(child))
			{
				search = pSearch;
				search.addRef();
				break;
			}
		}

		if (!!search)
		{
			if ((!deep) || (!remainingPath))
			{
				// no deep search or no remaining path
				branch = search;
				branch->addRef();
			}
			else
			{
				// deep search and remaining path
				branch = search->findBranch(remainingPath, TRUE);
			}
		}
	}

	return branch;
} // findBranch


//-----------------------------------------------------------------------------
// doStateTransition
//
// - do the state transition for the server if the target state differs from the
//   actual state of the server
//
// returns:
//      TRUE  - transit to new state
//      FALSE - error during state change
//
HRESULT SODaCServer::doStateTransition(BOOL deep, IN BYTE stateDeep)
{
	HRESULT res = S_OK;
	BYTE targetState = getTargetObjectState();

	if (disconnected == targetState)
	{
		doRecurseChildren(getBranchList(), disconnected);
		doRecurseChildren(getLeafList(), disconnected);
		doRecurseChildren(getAdditionalList(), disconnected);
	}

	res = doStateTransitionImpl();

	if (deep && SUCCEEDED(res) && disconnected != targetState)
	{
		if (S_OK != doRecurseChildren(getBranchList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getLeafList(), stateDeep))
		{
			res = S_FALSE;
		}

		if (S_OK != doRecurseChildren(getAdditionalList(), stateDeep))
		{
			res = S_FALSE;
		}
	}

	return res;
} // doStateTransition

HRESULT SODaCServer::doDisconnect(void)
{
	return doDisconnectImpl(getConnectionMonitorStatus());
}


//-----------------------------------------------------------------------------
// addGroup
//
// - insert group to the group list of the server. The group is connected by
//   calling AddGroup of the IOPCServer interface
//
// returns:
//      pointer to the new group
//      NULL - error creating the group
//
SODaCGroup* SODaCServer::addGroup(
	IN LPCTSTR groupName,       // group name
	IN DWORD reqUpdateRate,     // requested update rate
	IN OPTIONAL FLOAT* deadband,// deadband
	IN OPTIONAL LONG* timeBias, // time zone
	IN OPTIONAL DWORD LCID)     // locale id
{
	// create group object
	SOCmnPointer<SODaCEntry> session = ::getSODaCEntry();
	SOCmnPointer<SODaCCreator> creator = session->getCreator();
	SODaCGroup* pCGrp = creator->createGroup(getProtocol(), this);
	pCGrp->init(this);

	// Set attributes
	if (groupName)
	{
		pCGrp->setName(groupName);
	}
	else
	{
		SOCmnString grpName;
		grpName.format(_T("%lu_%8.8lx_%lu"), reqUpdateRate, (DWORD)pCGrp->getOpcHandle(), GetTickCount());
		pCGrp->setName(grpName);
	}

	pCGrp->setReqUpdateRate(reqUpdateRate);

	if (timeBias)
	{
		pCGrp->setTimeBias(*timeBias);
	}

	if (deadband)
	{
		pCGrp->setDeadBand(*deadband);
	}

	if (LCID)
	{
		pCGrp->setLCID(LCID);
	}

	pCGrp->setConnectionType(SODaCGroup::ctAddGroup);

	// Insert into the tree
	if (!addBranch(pCGrp))
	{
		// group couldn't be added to the server; I delete the allocated memory of the group.
		pCGrp->release();
		pCGrp = NULL;
	}

	return pCGrp;
} // addGroup


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
HRESULT SODaCServer::doUpdate(
	IN BOOL fromServer,                 // update direction: write to server or read from
	IN OPTIONAL DWORD whatParameters)   // update what parameters
{
	return doUpdateImpl(fromServer, whatParameters);
} // update


//-----------------------------------------------------------------------------
// getNamespaceBrowser
//  - creates a browser object
SODaCNameSpaceBrowser* SODaCServer::getNameSpaceBrowser()
{
	return getNameSpaceBrowserImpl();
}


SODaCGroup* SODaCServer::getGroupByClientHandle(
	IN OPCHANDLE clientHandle)  // client handle
{
	SOCmnList<SOCmnElement> branchList(getBranchList());
	return (SODaCGroup*)branchList.findKey(clientHandle);
}

//-----------------------------------------------------------------------------
// Serialization
BOOL SODaCServer::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;
	SOCmnList<SOCmnElement> list(getBranchList());
	SODaCGroup* grp;

	if (isStoring)
	{
		if (!getPersistence())
		{
			return TRUE;
		}

		// store own data
		ar << getValidParameters();
		ar << getServerName();
		ar << getNodeName();
		ar << getClientName();
		ar << getLCID();
		ar << getUserName();
		ar << getPassword();
		ar << getForcedOPCVersion();
		serializeImpl(ar, isStoring);
		// store children
		DWORD dwCount = 0;
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			grp = (SODaCGroup*)list.getNext(pos);

			if (grp->getPersistence())
			{
				dwCount++;
			}
		}

		ar << dwCount;
		pos = list.getStartPosition();

		while (pos && ok)
		{
			grp = (SODaCGroup*)list.getNext(pos);

			if (grp->getPersistence())
			{
				ok = grp->serialize(ar, isStoring);
			}
		}
	}
	else
	{
		// load own data
		SOCmnPointer<SODaCEntry> entry = getSODaCEntry();
#ifdef SOFEATURE_DCOM
		CLSID clsid = CLSID_NULL;
		DWORD clsctx = 0;
#endif
		SOCmnString nodeName;
		SOCmnString clientName;
		SOCmnString serverName;
		LCID lcid = 0;
		ar >> m_validParameter;
		ar >> serverName;
		setServerName(serverName);
		ar >> nodeName;
		setNodeName(nodeName);
#ifdef SOFEATURE_DCOM

		if (entry->getFileVersion() < MAKELONG(3, 4))
		{
			ar >> clsid;
			setClassID(clsid);
			ar >> clsctx;
			setClsctx(clsctx);
		}

#endif
		ar >> clientName;

		if (m_validParameter & SOCLT_SRVPAR_CLIENT_NAME)
		{
			setClientName(clientName);
		}

		ar >> lcid;

		if (m_validParameter & SOCLT_SRVPAR_LCID)
		{
			setLCID(lcid);
		}

		if (entry->getFileVersion() >= MAKELONG(1, 4))
		{
			SOCmnString userName;
			SOCmnString password;
			ar >> userName;
			setUserName(userName);
			ar >> password;
			setPassword(password);
		}

		if (entry->getFileVersion() >= MAKELONG(9, 4))
		{
			WORD fov = 0;
			ar >> fov;
			forceOPCVersion(fov);
		}

		if (entry->getFileVersion() >= MAKELONG(3, 4))
		{
			serializeImpl(ar, isStoring);
		}

		// load childeren
		DWORD dwCount = 0;
		ar >> dwCount;

		if (dwCount > 0)
		{
			while (dwCount-- && ok)
			{
				// create group
				grp = addGroup(NULL, 0);

				if (grp)
				{
					ok = grp->serialize(ar, isStoring);
					grp->release();
				}
			}
		}
	}

	return ok;
} // serialize

SODaCServer* SODaCServer::getObjectDaCServer(void)
{
	return this;
}

#ifdef SOFEATURE_DCOM

HRESULT SODaCServer::queryInterface(IN REFIID riid, IN void** interf)
{
	return queryInterfaceImpl(riid, interf);
}

#endif // SOFEATURE_DCOM


#ifdef SOFEATURE_SOAP

DWORD SODaCServer::getPropertyIDByName(IN LPCTSTR propName)
{
	if (!propName)
	{
		return 0;
	}

	switch (*propName)
	{
	case _T('a'):
		if (_tcsicmp(propName, _T("accessRights")) == 0)
		{
			return 5;
		}

		break;

	case _T('c'):
		if (_tcsicmp(propName, _T("closeLabel")) == 0)
		{
			return 106;
		}

		break;

	case _T('d'):
		if (_tcsicmp(propName, _T("dataType")) == 0)
		{
			return 1;
		}
		else if (_tcsicmp(propName, _T("description")) == 0)
		{
			return 101;
		}

		break;

	case _T('e'):
		if (_tcsicmp(propName, _T("euType")) == 0)
		{
			return 7;
		}
		else if (_tcsicmp(propName, _T("euInfo")) == 0)
		{
			return 8;
		}
		else if (_tcsicmp(propName, _T("engineeringUnits")) == 0)
		{
			return 100;
		}

		break;

	case _T('h'):
		if (_tcsicmp(propName, _T("highEU")) == 0)
		{
			return 102;
		}
		else if (_tcsicmp(propName, _T("highIR")) == 0)
		{
			return 104;
		}

		break;

	case _T('l'):
		if (_tcsicmp(propName, _T("lowEU")) == 0)
		{
			return 103;
		}
		else if (_tcsicmp(propName, _T("lowIR")) == 0)
		{
			return 105;
		}

		break;

	case _T('o'):
		if (_tcsicmp(propName, _T("openLabel")) == 0)
		{
			return 107;
		}

		break;

	case _T('q'):
		if (_tcsicmp(propName, _T("quality")) == 0)
		{
			return 3;
		}

		break;

	case _T('s'):
		if (_tcsicmp(propName, _T("scanRate")) == 0)
		{
			return 6;
		}

		break;

	case _T('t'):
		if (_tcsicmp(propName, _T("timestamp")) == 0)
		{
			return 4;
		}
		else if (_tcsicmp(propName, _T("timeZone")) == 0)
		{
			return 108;
		}

		break;

	case _T('v'):
		if (_tcsicmp(propName, _T("value")) == 0)
		{
			return 2;
		}

		break;
	}

	return 0xFFFFFFFF;
}

void SODaCServer::setXmlStatusTask(IN SOCltTaskXmlGetStatus* pTask)
{
	getWorkerThread()->addCyclicTask(pTask);
	m_xmlGetStatusTaskHandle = pTask->getOpcHandle();
}

#endif

void SODaCServer::connectionFailure(void)
{
	connectionFailureImpl();
}


//-----------------------------------------------------------------------------
// setGroupsState
//
// - set the state of all items of the group
//
void SODaCServer::setGroupsState(
	IN BYTE state,    // group target state
	IN BOOL itemsToo)
{
	SOCmnList<SOCmnElement> list(getBranchList());
	SOCmnListPosition pos;
	SODaCGroup* group;
	pos = list.getStartPosition();

	while (pos)
	{
		group = (SODaCGroup*)list.getNext(pos);
		group->setObjectState(state);

		if (itemsToo)
		{
			group->setItemsState(state);
		}
	}
} // setGroupsState


SODaCGroup* SODaCServer::getGroup(IN LPCTSTR groupName)
{
	SOCmnList<SOCmnElement> branchList(getBranchList());
	SOCmnListPosition pos;
	SODaCGroup* pGroup;
	pos = branchList.getStartPosition();

	while (pos)
	{
		pGroup = (SODaCGroup*)branchList.getNext(pos);

		if (_tcscmp(pGroup->getName(), groupName) == 0)
		{
			pGroup->addRef();
			return pGroup;
		}
	}

	return NULL;
};


//-----------------------------------------------------------------------------
// SODaCServerStatus                                                          |
//-----------------------------------------------------------------------------

SODaCServerStatus::SODaCServerStatus(void)
{
	m_state = 0;
	m_groupCount = 0;
	m_bandwidth = 0xFFFFFFFF;
	m_supportedLCIDs.create();
	m_supportedInterfaces.create();
}

SODaCServerStatus::~SODaCServerStatus(void)
{
	m_supportedLCIDs.destroy();
	m_supportedInterfaces.destroy();
}

void SODaCServerStatus::clear(void)
{
	m_state = 0;
	m_startTime.clear();
	m_currentTime.clear();
	m_vendorInfo.empty();
	m_productVersion.empty();
	m_supportedLCIDs.removeAll();
	m_supportedInterfaces.removeAll();
	m_lastUpdateTime.clear();
	m_groupCount = 0;
	m_bandwidth = 0xFFFFFFFF;
	m_statusInfo.empty();
}



//-----------------------------------------------------------------------------
// SODaCServerICommunication                                                  |
//-----------------------------------------------------------------------------

SODaCServerICommunication::SODaCServerICommunication(void)
{}
SODaCServerICommunication::~SODaCServerICommunication(void)
{}
void SODaCServerICommunication::init(void)
{}
SODaCServer* SODaCServerICommunication::getObjectDaCServer(void)
{
	return NULL;
}
HRESULT SODaCServerICommunication::doStateTransitionImpl(void)
{
	return E_NOTIMPL;
}
HRESULT SODaCServerICommunication::doUpdateImpl(IN BOOL /* fromServer */, IN OPTIONAL DWORD /* whatParameters */)
{
	return E_NOTIMPL;
}
HRESULT SODaCServerICommunication::doDisconnectImpl(BOOL /* connectionMonitorStatus */)
{
	return E_NOTIMPL;
}
BOOL SODaCServerICommunication::serializeImpl(IN OUT SOCltStorage& /* ar */, IN BOOL /* isStoring */)
{
	return FALSE;
}
HRESULT SODaCServerICommunication::getStatus(OUT SODaCServerStatus& /* status */)
{
	return E_NOTIMPL;
}
HRESULT SODaCServerICommunication::queryAvailableProperties(IN LPCTSTR /* itemID */, IN LPCTSTR /* itemPath */, IN WORD /* whatData */, OUT SOCmnList<SODaCPropertyData>& /* propteryList */)
{
	return E_NOTIMPL;
}
HRESULT SODaCServerICommunication::getPropertiesData(IN LPCTSTR /* itemID */, IN LPCTSTR /* itemPath */, IN WORD /* whatData */, IN OUT SOCmnList<SODaCPropertyData>& /* propteryList */)
{
	return E_NOTIMPL;
}
WORD SODaCServerICommunication::getSupportedOPCVersion(void)
{
	return 0;
}
BOOL SODaCServerICommunication::forceOPCVersion(WORD /* opcVersion */)
{
	return FALSE;
}
WORD SODaCServerICommunication::getForcedOPCVersion(void)
{
	return SODAC_VERSION_OPC_USEDEFAULT;
}
BOOL SODaCServerICommunication::setTimeout(float /* timeoutPercent */, DWORD /* timeoutMin */,DWORD /* timeoutMax */)
{
	return FALSE;
}
float SODaCServerICommunication::getTimeoutPercent(void)
{
	return 0.0f;
}
DWORD SODaCServerICommunication::getTimeoutMin(void)
{
	return 0;
}
DWORD SODaCServerICommunication::getTimeoutMax(void)
{
	return 0;
}
SODaCNameSpaceBrowser* SODaCServerICommunication::getNameSpaceBrowserImpl(void)
{
	return NULL;
}
HRESULT SODaCServerICommunication::read(
	IN DWORD /* count */,
	IN SOCmnString* /* pItemIDs */,
	IN SOCmnString* /* pItemPaths */,
	IN DWORD /* maxAge */,
	OUT SOCmnVariant* /* pValues */,
	OUT WORD* /* pQualities */,
	OUT SOCmnDateTime* /* pTimestamps */,
	OUT HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

HRESULT SODaCServerICommunication::write(
	IN DWORD /* count */,
	IN SOCmnString* /* pItemIDs */,
	IN SOCmnString* /* pItemPaths */,
	IN SOCmnVariant* /* pValues */,
	IN WORD* /* pQualities */,
	IN SOCmnDateTime* /* pTimestamps */,
	OUT HRESULT* /* pErrors */)
{
	return E_NOTIMPL;
}

void SODaCServerICommunication::connectionFailureImpl(void)
{ }

#ifdef SOFEATURE_DCOM
HRESULT SODaCServerICommunication::queryInterfaceImpl(
	IN REFIID /* riid */,
	IN void** /* interf */)
{
	return E_NOTIMPL;
}
#endif

#ifdef SOFEATURE_SOAP
// XML-DA specific
void SODaCServerICommunication::setIPPort(WORD /* port */)
{}
WORD SODaCServerICommunication::getIPPort(void)
{
	return 0;
}
void SODaCServerICommunication::setHTTPPath(LPCTSTR /* path */)
{}
LPCTSTR SODaCServerICommunication::getHTTPPath(void)
{
	return NULL;
}
SOCltHTTPConnection* SODaCServerICommunication::getHTTPConnection(void)
{
	return NULL;
}
SODaCXmlRequestOptions* SODaCServerICommunication::fillRequestOptions(SODaCXmlRequestOptions* pReqOptions)
{
	return pReqOptions;
}
void SODaCServerICommunication::setTimeDifference(LONGLONG /* timeDiffSend */, LONGLONG /* timeDiffReply */)
{}
LONGLONG SODaCServerICommunication::getTimeDifference(void)
{
	return 0;
}
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
// XML-DA specific
void SODaCServerICommunication::setIPPort(WORD /* port */)
{}
WORD SODaCServerICommunication::getIPPort(void)
{
	return 0;
}
#endif // SOFEATURE_SOAP
// Softing OPC Tunnel specific
SOCltTPConnection* SODaCServerICommunication::getTPConnection(void)
{
	return NULL;
}
#endif

#endif // SODAC
