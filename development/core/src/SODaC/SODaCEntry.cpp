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
//  Filename    : SODaCEntry.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Entry point for OPC Data Access Client Toolkit              |
//                - SODaCEntry: Entry point class                             |
//                - SODaCCreator: Object factory for toolkit objects          |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#include "SODaCEntry.h"
#include "SODaCServer.h"
#include "SODaCGroup.h"
#include "SODaCItem.h"
#include "SOVersion.h"
#ifdef SOFEATURE_TUNNEL
#include "SODaCTPCall.h"
#include "SOCmnTP.h"
#endif

//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

static SODaCEntry* g_session = NULL;

SODaCEntry* getOPCSession(void)
{
	return getSODaCEntry();
}

SODaCEntry* getSODaCEntry(void)
{
	if (g_session)
	{
		g_session->addRef();
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(g_session, SODaCEntry());
	}

	return g_session;
}


//-----------------------------------------------------------------------------
// SODaCCreator                                                               |
//-----------------------------------------------------------------------------

SODaCCreator::SODaCCreator(void)
#ifdef SOFEATURE_SOAP
	:m_xmlGetStatusInterval(0)
#endif
{
}

SODaCCreator::~SODaCCreator(void)
{
}

SODaCServer* SODaCCreator::createServer(
	IN BYTE protocolType,
	IN SODaCEntry* /* parent */)
{
	SODaCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCComServerDefault(), (SODaCServer*));
		break;
#endif
#ifdef SOFEATURE_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCXmlServerDefault(), (SODaCServer*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCTPServerDefault(), (SODaCServer*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

SODaCGroup* SODaCCreator::createGroup(
	IN BYTE protocolType,
	IN SODaCServer* /* parent */)
{
	SODaCGroup* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCComGroupDefault(), (SODaCGroup*));
		break;
#endif
#ifdef SOFEATURE_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCXmlGroupDefault(), (SODaCGroup*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SODaCTPGroupDefault(), (SODaCGroup*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

SODaCItem* SODaCCreator::createItem(
	IN SODaCGroup* /* parent */)
{
	SODaCItemDefault* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SODaCItemDefault());
	return p;
}

SODaCNode* SODaCCreator::createNode(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SODaCNode* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SODaCNode(parent, name));
	return p;
}

SODaCTag* SODaCCreator::createTag(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SODaCTag* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SODaCTag(parent, name));
	return p;
}

SODaCProperty* SODaCCreator::createProperty(IN SOCltBrowseObject* parent, LPCTSTR name, DWORD propID, VARTYPE datatype, LPCTSTR parentID, LPCTSTR parentPath)
{
	SODaCProperty* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SODaCProperty(parent, name, propID, datatype, parentID, parentPath));
	return p;
}

#ifdef SOFEATURE_SOAP
SOCltTaskXmlGetStatus* SODaCCreator::createTaskXmlGetStatus(IN SOCltElement* pServer, IN DWORD cycle)
{
	SOCltTaskXmlGetStatus* task = NULL;
	SOCMN_ALLOCATE_OBJECT_C(task, SOCltTaskXmlGetStatus(pServer, cycle), (SOCltTaskXmlGetStatus*));
	return task;
}

void SODaCCreator::setXmlGetStatusInterval(IN const DWORD& getStatusInterval)
{
	m_xmlGetStatusInterval = getStatusInterval;

	if ((m_xmlGetStatusInterval < 1000) && (m_xmlGetStatusInterval > 0))
	{
		// should not be less than 1 second!
		m_xmlGetStatusInterval = 1000;
	}
}
#endif

//-----------------------------------------------------------------------------
// SODaCEntry                                                                 |
//-----------------------------------------------------------------------------

SODaCEntry::SODaCEntry(void)
	: SOCmnElementListBranch(SOCMNLIST_TYPE_ORG_LIST),
	  SOCltEntry(0)
{
	m_fileVersion = MAKELONG(MINOR_VERSION_NUMBER, MAJOR_VERSION_NUMBER);
	SOCMN_ALLOCATE_OBJECT(m_creator, SODaCCreator);
#ifdef SOFEATURE_SOAP
	SOCMN_ALLOCATE_OBJECT_C(m_httpClient, SOCltHTTP(), (SOCltHTTP*));
#endif
#ifdef SOFEATURE_TUNNEL
	SOCMN_ALLOCATE_OBJECT_C(m_tpClient, SOCltTP(), (SOCltTP*));
	SOCmnPointer<SODaCTPReceiver> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SODaCTPReceiver(), (SODaCTPReceiver*));
	m_tpClient->addTPReceiver(tpRec);
#endif
	::setSOCltEntry(this);
}

SODaCEntry::~SODaCEntry()
{
#ifdef SOFEATURE_TUNNEL
	m_tpClient->stop();
	m_tpClient.release();
#endif
#ifdef SOFEATURE_SOAP
	m_httpClient->stop();
	m_httpClient.release();
#endif
	g_session = NULL;
	::setSOCltEntry(NULL);
}


//-----------------------------------------------------------------------------
// getObjectElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCEntry::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIList


//-----------------------------------------------------------------------------
// getObjectCltElementIError
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaCEntry::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


//-----------------------------------------------------------------------------
// addLeaf
//
// - don't add leaf into session object
//
// returns:
//		FALSE - not added
//
BOOL SODaCEntry::addLeaf(
	IN SOCmnElement* /* newLeaf */)
{
	return FALSE;
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a server object
//
// returns:
//		TRUE  - added
//		FALSE - not added
//
BOOL SODaCEntry::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_SERVER_DA))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// setCreator
//
// - set creator object
//
void SODaCEntry::setCreator(
	IN SODaCCreator* creator)   // creator object
{
	if (m_creator)
	{
		m_creator->release();
	}

	m_creator = creator;

	if (m_creator)
	{
		m_creator->addRef();
	}
} // setCreator


//-----------------------------------------------------------------------------
// getCltCreator
//
// - get toolkit object factory
//
// returns:
//		toolkit object factory
//
SOCltCreator* SODaCEntry::getCltCreator(void)
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return (SOCltCreator*)m_creator;
} // getCltCreator


//-----------------------------------------------------------------------------
// addServer
//
// - create server object and insert it into the session branch list
// - initialize the server with the parameter data
//
// returns:
//      pointer to server object
//
SODaCServer* SODaCEntry::addServer(IN LPCTSTR url)
{
	return parseURL(url, NULL);
}

SODaCServer* SODaCEntry::parseURL(IN LPCTSTR url, IN SODaCServer* pServerIn)
{
	SOCmnPointer<SODaCServer> server;
	SOCmnString i1(url);
	SOCmnString i2;
	SOCmnString protocol;
	SOCmnString host;
	SOCmnString path;
	int cnt1 = i1.getLength();
	int cnt2 = 0;
	int ppos;
	int hpos = 0;
	ppos = i1.find(_T("://"));

	if (ppos != -1)
	{
		protocol = (LPCTSTR)i1.left(ppos);       // the left of "://"
		i2 = (LPCTSTR)i1.right(cnt1 - ppos - 3); // the right of "://"
		cnt2 = i2.getLength();
		hpos = i2.find(_T("/"));

		if (hpos != -1)
		{
			host = (LPCTSTR)i2.left(hpos);         // the left of "/"
			path = (LPCTSTR)i2.right(cnt2 - hpos - 1); // the right of "/"
		}
		else
		{
			// no / as path seperator -> i2 is only the host name
			host = i2;
		}
	}

#ifdef SOFEATURE_DCOM

	if (protocol == _T("opcda"))
	{
		if (pServerIn)
			if (pServerIn->getProtocol() != SOCLT_PROTOCOL_DCOM)
			{
				return NULL;
			}

		USES_CONVERSION;
		CLSID clsid;
		SOCmnString progID, classID;
		int pos3, cnt3;
		BOOL clsidOK = FALSE;

		if (host == _T("localhost"))
		{
			host.empty();
		}

		// get CLSID
		pos3 = path.find(_T("/"));

		if (pos3 != -1)
		{
			cnt3 = path.getLength();
			progID = (LPCTSTR)path.left(pos3);
			classID = (LPCTSTR)path.right(cnt3 - pos3 - 1);
		}
		else
		{
			progID = path;
		}

		if (classID.isEmpty())
		{
			// no clsid specified -> use progID to the the clsid
			if (host.isEmpty())
			{
				// local connection -> use the registry
				if (SUCCEEDED(::CLSIDFromProgID(T2W(progID), &clsid)))
				{
					clsidOK = TRUE;
				}
			}
			else
			{
				// remote connection -> use OPCEnum
				SOCmnPointer<SOCltServerBrowser> sb;
				sb = new SOCltServerBrowser();
				sb->setNodeName(host);

				if (sb->getCLSIDFromProgID(progID, &clsid))
				{
					clsidOK = TRUE;
				}
			}
		}
		else
		{
			if (SUCCEEDED(::CLSIDFromString(T2W(classID), &clsid)))
			{
				clsidOK = TRUE;
			}
		}

		if (clsidOK)
		{
			// create Server
			if (!pServerIn)
			{
				server = m_creator->createServer(SOCLT_PROTOCOL_DCOM, this);
			}
			else
			{
				server = pServerIn;
				server.addRef();
			}

			server->initClt();
			server->init();
			server->setProtocol(SOCLT_PROTOCOL_DCOM);
			server->setURL(url);
			server->setNodeName(host);
			server->setClassID(clsid);
		}
	}

#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_SOAP

	if (protocol == _T("http"))
	{
		if (pServerIn)
			if (pServerIn->getProtocol() != SOCLT_PROTOCOL_SOAP)
			{
				return NULL;
			}

		WORD port = 80;
		int pos3, cnt3;
		// get port
		pos3 = host.find(_T(":"));

		if (pos3 != -1)
		{
			cnt3 = host.getLength();
			port = (WORD)_tcstoul((LPCTSTR)host + pos3 + 1, NULL, 10);
			host = (LPCTSTR)host.left(pos3);
		}

		if (host == _T("localhost"))
		{
			host.empty();
		}

		path = (LPCTSTR)i2.right(cnt2 - hpos); // path with leading /

		if (!pServerIn)
		{
			server = m_creator->createServer(SOCLT_PROTOCOL_SOAP, this);
		}
		else
		{
			server = pServerIn;
			server.addRef();
		}

		server->initClt();
		server->init();
		server->setProtocol(SOCLT_PROTOCOL_SOAP);
		server->setURL(url);
		server->setNodeName(host);
		server->setIPPort(port);
		server->setHTTPPath(path);
	}

#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_TUNNEL

	if (protocol == _T("tpda"))
	{
		if (pServerIn)
			if (pServerIn->getProtocol() != SOCLT_PROTOCOL_TUNNEL)
			{
				return NULL;
			}

		WORD port = TP_DEFAULT_PORT;
		int pos3, cnt3;
		// get port
		pos3 = host.find(_T(":"));

		if (pos3 != -1)
		{
			cnt3 = host.getLength();
			port = (WORD)_tcstoul((LPCTSTR)host + pos3 + 1, NULL, 10);
			host = (LPCTSTR)host.left(pos3);
		}

		if (host == _T("localhost"))
		{
			host.empty();
		}

		if (!pServerIn)
		{
			server = m_creator->createServer(SOCLT_PROTOCOL_TUNNEL, this);
		}
		else
		{
			server = pServerIn;
			server.addRef();
		}

		server->initClt();
		server->init();
		server->setProtocol(SOCLT_PROTOCOL_TUNNEL);
		server->setURL(url);
		server->setNodeName(host);
		server->setIPPort(port);
	}

#endif // SOFEATURE_TUNNEL

	if (!pServerIn)
	{
		if (server.isNotNull())
		{
			// add server to session
			addBranch((SODaCServer*)server);
		}

		server.addRef();
	}

	return server;
} // addServer


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// addServer
//
// - create server object and insert it into the session branch list
// - initialize the server with the parameter data
//
// returns:
//      pointer to server object
//
SODaCServer* SODaCEntry::addServer(
	IN LPCTSTR serverName,  // server name
	IN LPCTSTR nodeName,    // node name
	IN REFCLSID clsid,  // class Id
	IN DWORD clsctx)        // execution context
{
	// create Server
	SOCmnPointer<SODaCServer> server = m_creator->createServer(SOCLT_PROTOCOL_DCOM, this);
	server->initClt();
	server->init();
	// initialize server
	server->setProtocol(SOCLT_PROTOCOL_DCOM);
	server->setServerName(serverName);
	server->setNodeName(nodeName);
	server->setClassID(clsid);
	server->setClsctx(clsctx);
	SOCmnString classID;
	SOCmnString url;
	classID.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7]);
	url.format(_T("opcda://%s//%s"), nodeName, (LPCTSTR)classID);
	server->setURL(url);
	// add server to session
	addBranch((SODaCServer*)server);
	server.addRef();
	return server;
} // addServer

#endif // SOFEATURE_DCOM


HRESULT SODaCEntry::doStateTransition(IN BOOL deep, IN BYTE stateDeep)
{
	HRESULT res = S_OK;

	if (getTargetObjectState() != disconnected)
	{
#ifdef SOFEATURE_SOAP

		if (!m_httpClient->start())
		{
			res = E_FAIL;
		}

#endif // SOFEATURE_SOAP
#ifdef SOFEATURE_TUNNEL

		if (!m_tpClient->start())
		{
			res = E_FAIL;
		}

#endif // SOFEATURE_TUNNEL
	}

	if (SUCCEEDED(res))
	{
		res = SOCltEntry::doStateTransition(deep, stateDeep);
	}

	if (getTargetObjectState() == disconnected)
	{
#ifdef SOFEATURE_TUNNEL
		m_tpClient->stop();
#endif // SOFEATURE_TUNNEL
#ifdef SOFEATURE_SOAP
		m_httpClient->stop();
#endif // SOFEATURE_SOAP
	}

	return res;
}


//-----------------------------------------------------------------------------
// getServer
//
// - get server object by the server name
//
// returns:
//      NULL - server not found
//      pointer to server object
//
SODaCServer* SODaCEntry::getServer(
	IN LPCTSTR serverName) // server name
{
	SOCmnList<SODaCServer> list = (SOCmnList<SODaCServer> *)getBranchList();
	SOCmnListPosition pos;
	SODaCServer* pServer;
	pos = list.getStartPosition();

	while (pos)
	{
		pServer = list.getNext(pos);

		if (pServer->getServerName() == serverName)
		{
			pServer->addRef();
			return pServer;
		}
	}

	return NULL;
} // getServer


//-----------------------------------------------------------------------------
// terminate
//
BOOL SODaCEntry::terminate(void)
{
	BOOL bRet = SOCltEntry::terminate();
	setCreator(NULL);
	return bRet;
}

//-----------------------------------------------------------------------------
// serialize
//
BOOL SODaCEntry::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;
	SOCmnList<SOCmnElement> list(getBranchList());
	SODaCServer* server = NULL;

	if (isStoring)
	{
		DWORD fileVersion;
		fileVersion = MAKELONG(MINOR_VERSION_NUMBER, MAJOR_VERSION_NUMBER);
		ar << fileVersion;
		DWORD dwCount = 0;
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			server = (SODaCServer*)list.getNext(pos);

			if (server->getPersistence())
			{
				dwCount++;
			}
		}

		ar << dwCount;
		pos = list.getStartPosition();

		while (pos && ok)
		{
			server = (SODaCServer*)list.getNext(pos);
			ar << server->getProtocol();
			ar << server->getURL();

			if (server->getPersistence())
			{
				ok = server->serialize(ar, isStoring);
			}
		}
	}
	else
	{
		// stop communication with the servers
		disconnect();
		// destroy object tree
		removeAllLeafs(TRUE);
		removeAllBranches(TRUE);
		DWORD dwCount = 0;
		ar >> m_fileVersion;

		if ((HIWORD(m_fileVersion) < 3) || (HIWORD(m_fileVersion) > 5))
		{
			return FALSE;
		}
		else
		{
			ar >> dwCount;
		}

		if (dwCount > 0)
		{
			while (dwCount-- && ok)
			{
				BYTE protocol;

				if (m_fileVersion < MAKELONG(03, 4))
				{
					protocol = SOCLT_PROTOCOL_DCOM;
					// create server
					server = m_creator->createServer(protocol, this);

					if (server)
					{
						server->initClt();
						server->init();
						server->setProtocol(protocol);
						addBranch((SODaCServer*)server);
					}
				}
				else
				{
					SOCmnString url;
					ar >> protocol;
					ar >> url;
					server = addServer(url);

					if (server)
					{
						server->setProtocol(protocol);
					}
				}

				if (server)
				{
					ok = server->serialize(ar, isStoring);
					server->release();
				}
			}
		}
	}

	return ok;
}

#endif // SODAC
