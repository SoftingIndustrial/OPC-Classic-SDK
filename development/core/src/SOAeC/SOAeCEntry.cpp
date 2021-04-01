#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCEntry.h"
#include "SOAeCServer.h"
#include "SOAeCComServer.h"
#include "SOAeCSubscription.h"
#include "SOAeCComSubscription.h"
#include "SOAeCEvent.h"
#include "SOVersion.h"
#ifdef SOFEATURE_TUNNEL
#include "SOAeCTPServer.h"
#include "SOAeCTPCall.h"
#include "SOCmnTP.h"
#endif


//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

static SOAeCEntry* g_session = NULL;

SOAeCEntry* getSOAeCEntry(void)
{
	if (g_session)
	{
		g_session->addRef();
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(g_session, SOAeCEntry())
	}

	return g_session;
}



//-----------------------------------------------------------------------------
// SOAeCCreator                                                              |
//-----------------------------------------------------------------------------

SOAeCCreator::SOAeCCreator(void)
{
}

SOAeCCreator::~SOAeCCreator(void)
{
}

//-----------------------------------------------------------------------------
// createServer
//
// - creates a server object
//
// returns: pointer to the created object if succeeded, NULL otherwise
//

SOAeCServer* SOAeCCreator::createServer(
	IN BYTE protocolType,
	IN SOAeCEntry* /* parent */)
{
	SOAeCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SOAeCComServerDefault(), (SOAeCServer*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SOAeCTPServerDefault(), (SOAeCServer*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

//-----------------------------------------------------------------------------
// createServer
//
// - creates a subscription object
//
// returns: pointer to the created object if succeeded, NULL otherwise
//

SOAeCSubscription* SOAeCCreator::createSubscription(
	IN BYTE protocolType,
	IN SOAeCServer* /* parent */)
{
	SOAeCSubscription* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, SOAeCComSubscriptionDefault(), (SOAeCSubscription*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, SOAeCTPSubscriptionDefault(), (SOAeCSubscription*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

//-----------------------------------------------------------------------------
// createEvent
//
// - creates an event object from the data received
//
// returns: pointer to the created object if succeeded, NULL otherwise
//

SOAeCEvent* SOAeCCreator::createEvent(
	IN SOAeCSubscription* subs,
	IN ONEVENTSTRUCT* pData)
{
	SOAeCEvent* p = NULL;
	SOCMN_ALLOCATE_OBJECT_C(p, SOAeCEvent(subs, pData), (SOAeCEvent*));
	return p;
}

SOAeCArea* SOAeCCreator::createArea(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SOAeCArea* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCArea(parent, name));
	return p;
}

SOAeCSource* SOAeCCreator::createSource(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SOAeCSource* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCSource(parent, name));
	return p;
}

SOAeCCondition* SOAeCCreator::createCondition(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SOAeCCondition* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCCondition(parent, name));
	return p;
}

SOAeCCategory* SOAeCCreator::createCategory(IN SOCltBrowseObject* parent, LPCTSTR name)
{
	SOAeCCategory* p = NULL;
	SOCMN_ALLOCATE_OBJECT(p, SOAeCCategory(parent, name));
	return p;
}



//-----------------------------------------------------------------------------
// SOAeCEntry                                                                 |
//-----------------------------------------------------------------------------

SOAeCEntry::SOAeCEntry(void)
	:  SOCltEntry(0),
	   SOCmnElementListBranch(SOCMNLIST_TYPE_ORG_LIST)
{
	m_fileVersion = MAKELONG(MINOR_VERSION_NUMBER, MAJOR_VERSION_NUMBER);
	SOCMN_ALLOCATE_OBJECT(m_creator, SOAeCCreator)
	::setSOCltEntry(this);
#ifdef SOFEATURE_TUNNEL
	SOCMN_ALLOCATE_OBJECT_C(m_tpClient, SOCltTP(), (SOCltTP*));
	SOCmnPointer<SOAeCTPReceiver> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SOAeCTPReceiver(), (SOAeCTPReceiver*));
	m_tpClient->addTPReceiver(tpRec);
#endif
}

SOAeCEntry::~SOAeCEntry()
{
#ifdef SOFEATURE_TUNNEL
	m_tpClient->stop();
	m_tpClient.release();
#endif
	g_session = NULL;
	::setSOCltEntry(NULL);
}

//-----------------------------------------------------------------------------
// getObjectElementIList
//
// - get object's implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeCEntry::getObjectCmnElementIList(void)
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
SOCmnObject* SOAeCEntry::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
} // getObjectCltElementIError


//-----------------------------------------------------------------------------
// addLeaf
//
// - don't add leaf to the session object
//
// returns:
//		FALSE - not added
//
BOOL SOAeCEntry::addLeaf(
	IN SOCmnElement* /* newLeaf */)
{
	return FALSE;
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to the branch list, if branch is a Server object
//
// returns:
//		TRUE  - added
//		FALSE - not added
//
BOOL SOAeCEntry::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_SERVER_AE))
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
void SOAeCEntry::setCreator(
	IN SOAeCCreator* creator)   // creator object
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
SOCltCreator* SOAeCEntry::getCltCreator(void)
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return (SOCltCreator*)m_creator;
} // getCltCreator


//-----------------------------------------------------------------------------
// serialize
//

BOOL SOAeCEntry::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	BOOL ok = TRUE;
	SOCmnList<SOCmnElement> list(getBranchList());
	SOAeCServer* server = NULL;

	if (isStoring)
	{
		DWORD fileVersion;
		fileVersion = MAKELONG(MINOR_VERSION_NUMBER, MAJOR_VERSION_NUMBER);
		ar << fileVersion;
		DWORD dwCount = 0;
		SOCmnListPosition pos = list.getStartPosition();

		while (pos)
		{
			server = (SOAeCServer*)list.getNext(pos);

			if (server->getPersistence())
			{
				dwCount++;
			}
		}

		ar << dwCount;
		pos = list.getStartPosition();

		while (pos && ok)
		{
			server = (SOAeCServer*)list.getNext(pos);
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
			dwCount = m_fileVersion;
			m_fileVersion = MAKELONG(0, 3);
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
					server = m_creator->createServer(SOCLT_PROTOCOL_DCOM, this);

					if (server)
					{
						server->initClt();
						server->setProtocol(protocol);
						addBranch((SOAeCServer*)server);
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


//-----------------------------------------------------------------------------
// addServer
//
// - create server object and insert it into the entry branch list
// - initialize the server with the parameter data
//
// returns:
//      pointer to the newly created server object
//
SOAeCServer* SOAeCEntry::addServer(
	IN LPCTSTR serverName,  // server name
	IN LPCTSTR nodeName,      // node name
	IN REFCLSID classID,      // class Id
	IN DWORD clsctx)            // execution context
{
	// create server
	SOAeCServer* server = m_creator->createServer(SOCLT_PROTOCOL_DCOM, this);
	server->initClt();

	if (server)
	{
		// initialize cfgServer
		server->setServerName(serverName);
		server->setNodeName(nodeName);
		server->setClassID(classID);
		server->setClsctx(clsctx);
		// add server to the server list
		addBranch(server);
	}

	return server;
} // addServer


//-----------------------------------------------------------------------------
// addServer
//
// - create server object and insert it into the session branch list
// - initialize the server with the parameter data
//
// returns:
//      pointer to server object
//
SOAeCServer* SOAeCEntry::addServer(IN LPCTSTR url)
{
	return parseURL(url, NULL);
}

SOAeCServer* SOAeCEntry::parseURL(IN LPCTSTR url, IN SOAeCServer* pServerIn)
{
	SOCmnPointer<SOAeCServer> server;
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

	if (protocol == _T("opcae"))
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
			server->setProtocol(SOCLT_PROTOCOL_DCOM);
			server->setURL(url);
			server->setNodeName(host);
			server->setClassID(clsid);
		}
	}

#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_TUNNEL

	if (protocol == _T("tpae"))
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
			addBranch((SOAeCServer*)server);
		}

		server.addRef();
	}

	return server;
} // addServer


HRESULT SOAeCEntry::doStateTransition(IN BOOL deep, IN BYTE stateDeep)
{
	HRESULT res = S_OK;

	if (getTargetObjectState() != disconnected)
	{
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
SOAeCServer* SOAeCEntry::getServer(
	IN LPCTSTR serverName) // server name
{
	SOCmnList<SOAeCServer> list = (SOCmnList<SOAeCServer> *)getBranchList();
	SOCmnListPosition pos;
	SOAeCServer* pServer;
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
BOOL SOAeCEntry::terminate(void)
{
	BOOL bRet = SOCltEntry::terminate();
	setCreator(NULL);
	return bRet;
}

#endif // SOAEC
