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
//  Filename    : SODaSEntry.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Entry object and Handler classes                            |
//                - SODaSEntry: Entry point class                             |
//                - SODaSCreator: Object factory for toolkit objects          |
//                - SODaSConfigFileHandler: Handler for configuration file    |
//                                          requests over IPersistFile        |
//                - SODaSStatisticsData: Statistics data                      |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#ifdef SOFEATURE_DCOM
#include <comcat.h>
#endif
#include "SODaSEntry.h"
#include "SODaSItemTag.h"
#include "SODaSTag.h"
#include "SODaSProperty.h"
#include "SODaSNode.h"
#include "SODaSServer.h"
#include "SODaSGroup.h"
#include "SODaSItem.h"
#include "SODaSCache.h"
#include "SODaSRequest.h"
#include "SOVersion.h"
#include "SOCmnTrace.h"
#ifdef SOFEATURE_DCOM
#include "SODaSComServer.h"
#endif
#ifdef SOFEATURE_SOAP
#include "SODaSXmlServer.h"
#endif
#ifdef SOFEATURE_TUNNEL
#include "SODaSTPServer.h"
#include "SODaSTPGroup.h"
#endif

//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

static SODaSEntry* g_engine = NULL;
static SOCmnSync g_engineSync;
static SOCmnSync g_namespaceSync;

#ifdef SOFEATURE_DCOM
#ifndef SOFEATURE_COMBINED_SERVER
CComModule _Module;
#endif

// ATL global variables
CLSID g_clsidDA0;
CLSID g_clsidDA1;
CLSID g_clsidDA2;
CLSID g_clsidDA3;
CLSID g_clsidDA4;
CLSID g_clsidDA5;
CLSID g_clsidDA6;
CLSID g_clsidDA7;
CLSID g_clsidDA8;
CLSID g_clsidDA9;

static CLSID CLSID_NO = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

BEGIN_OBJECT_MAP(g_objectMap)
OBJECT_ENTRY(g_clsidDA0, SODaSComServer0)
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL},
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL},
#endif
#else
{NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL}, {NULL, NULL, NULL, NULL, NULL, 0, NULL},
#endif
END_OBJECT_MAP()
#endif // SOFEATURE_DCOM

#ifdef SOFLAG_DEMO_VERSION
#ifndef SOFEATURE_COMBINED_SERVER
#ifndef SOFLAG_NO_DLLS
DWORD g_demoStamp = GetTickCount();
#endif
#endif
#endif


//-----------------------------------------------------------------------------
// getSODaSEntry
// getOPCEngine
//
// - get the entry object of the toolkit
//
// returns:
//		entry object
//
SODaSEntry* getOPCEngine(void)
{
	return getSODaSEntry();
}
SODaSEntry* getSODaSEntry(void)
{
	SOCmnSingleLock<SOCmnSync> lock(&g_engineSync);

	if (g_engine)
	{
		g_engine->addRef();
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(g_engine, SODaSEntry())
	}

	return g_engine;
} // getSODaSEntry




//-----------------------------------------------------------------------------
// SODaSCreator                                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// createServer
//
// - create server object
//
// returns:
//		created object
//
SODaSServer* SODaSCreator::createServer(
	IN BOOL publicGroups,   // create public groups server object
	IN BYTE serverInstance) // server instance number
{
	SODaSServer* server;
	SOCMN_ALLOCATE_OBJECT(server, SODaSServer(publicGroups, serverInstance))
	return server;
} // createServer


//-----------------------------------------------------------------------------
// createGroup
//
// - create group object
//
// returns:
//		created object
//
SODaSGroup* SODaSCreator::createGroup(
	IN SODaSServer* /* parent */)   // server
{
	SODaSGroup* group;
	SOCMN_ALLOCATE_OBJECT(group, SODaSGroupDefault())
	return group;
} // createGroup


//-----------------------------------------------------------------------------
// createItem
//
// - create item object
//
// returns:
//		created object
//
SODaSItem* SODaSCreator::createItem(
	IN SODaSGroup* /* parent */,    // group of item
	IN SODaSItemTag* /* tag */) // item tag of item
{
	SODaSItem* item;
	SOCMN_ALLOCATE_OBJECT(item, SODaSItemDefault())
	return item;
} // createItem


//-----------------------------------------------------------------------------
// createRequest
//
// - create request object
//
// returns:
//		created object
//
SODaSRequest* SODaSCreator::createRequest(
	IN SODaSServer* /* server */,   // server object
	IN SODaSItem* /* item */,       // item object (can be NULL)
	IN SODaSItemTag* /* tag */,     // tag object
	IN BYTE operation,              // request operation (read or write)
	IN WORD type)                   // request type
{
	SODaSRequest* req;
	SOCMN_ALLOCATE_OBJECT(req, SODaSRequest(operation, type))
	return req;
} // createRequest


//-----------------------------------------------------------------------------
// createCache
//
// - create cache object
//
// returns:
//		created object
//
SODaSCache* SODaSCreator::createCache(
	IN SODaSItemTag* /* tag */) // tag object of cache
{
	SODaSCache* cache;
	SOCMN_ALLOCATE_OBJECT(cache, SODaSCache())
	return cache;
} // createCache


//-----------------------------------------------------------------------------
// createTag
//
// - create tag object
//
// returns:
//		created object
//
SODaSTag* SODaSCreator::createTag(
	IN LPCTSTR /* qualItemID */)    // fully qualified id of the tag
{
	SODaSTag* tag;
	SOCMN_ALLOCATE_OBJECT(tag, SODaSTagDefault())
	return tag;
} // createTag


//-----------------------------------------------------------------------------
// createNode
//
// - create node object
// - used for object creation of initial string based nodes
//
// returns:
//		created object
//
SODaSNode* SODaSCreator::createNode(
	IN LPCTSTR /* nodePath */)  // full namespace path of the node
{
	SODaSNode* node;
	SOCMN_ALLOCATE_OBJECT(node, SODaSNodeDefault())
	return node;
} // createNode


//-----------------------------------------------------------------------------
//  createProperty
//
// - create property object
//
// returns:
//		created object
//
SODaSProperty* SODaSCreator::createProperty(
	IN SOCmnElement* /* namespaceElement */,    // namespace element to create property for
	IN DWORD propertyId,                                // property id
	IN LPCTSTR propertyName)                        // property id string
{
	SODaSProperty* prop;
	SOCMN_ALLOCATE_OBJECT(prop, SODaSProperty(propertyId, propertyName))
	return prop;
} // createProperty


//-----------------------------------------------------------------------------
//  createTransaction
//
// - create transaction object
//
// returns:
//		created object
//
SODaSTransaction* SODaSCreator::createTransaction(
	IN SODaSGroup* group,    // group object
	IN BYTE transactionType, // type of transaction
	IN BYTE connectionType,  // connection type
	IN DWORD connection)     // connection id
{
	SODaSTransaction* tac;
	SOCMN_ALLOCATE_OBJECT(tac, SODaSTransaction(group, transactionType, connectionType, connection))
	return tac;
} // createTransaction


//-----------------------------------------------------------------------------
//  createNameSpaceRoot
//
// - create namespace root object
//
// returns:
//		created object
//
SODaSNameSpaceRoot* SODaSCreator::createNameSpaceRoot(
	IN BYTE /* serverInstance */)  // server ínstance
{
	SODaSNameSpaceRoot* root;
	SOCMN_ALLOCATE_OBJECT(root, SODaSNameSpaceRoot())
	return root;
} // createNameSpaceRoot


//-----------------------------------------------------------------------------
//  createObjectRoot
//
// - create COM object root object
//
// returns:
//		created object
//
SODaSObjectRoot* SODaSCreator::createObjectRoot(void)
{
	SODaSObjectRoot* root;
	SOCMN_ALLOCATE_OBJECT(root, SODaSObjectRoot())
	return root;
} // createObjectRoot




//-----------------------------------------------------------------------------
// SODaSConfigFileHandler                                                     |
//-----------------------------------------------------------------------------

SODaSConfigFileHandler::SODaSConfigFileHandler(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_CALLBACK)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("ConfigFile"));
	setObjTraceId(id);
}




//-----------------------------------------------------------------------------
// SODaSEntry                                                                 |
//-----------------------------------------------------------------------------

SODaSEntry::SODaSEntry(void)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("DaS Entry"));
	setObjTraceId(id);
	m_delimProp = _T('#');
	m_vendorInfo = _T("Softing OPC Data Access Server Development Kit");
	m_majorVersion = MAJOR_VERSION_NUMBER;
	m_minorVersion = MINOR_VERSION_NUMBER;
	m_patchVersion = PATCH_VERSION_NUMBER;
	m_buildNumber = BUILD_NUMBER;
	m_minUpdateRate = 20;   // minimal NT timer is 10 msecs !!
	m_minKeepAliveTime = 20;
	memset(&m_statistics, 0, sizeof(SODaSEntryStatisticsData));
#ifdef SOFEATURE_DCOM
	g_clsidDA0 = CLSID_NO;
	g_clsidDA1 = CLSID_NO;
	g_clsidDA2 = CLSID_NO;
	g_clsidDA3 = CLSID_NO;
	g_clsidDA4 = CLSID_NO;
	g_clsidDA5 = CLSID_NO;
	g_clsidDA6 = CLSID_NO;
	g_clsidDA7 = CLSID_NO;
	g_clsidDA8 = CLSID_NO;
	g_clsidDA9 = CLSID_NO;
#endif

	for (BYTE i = 0; i < SODAS_SERVER_INSTANCE_COUNT; i++)
	{
		m_nsRoot[i] = NULL;
	}

	m_cfgHandler = NULL;
	SOCMN_ALLOCATE_OBJECT(m_creator, SODaSCreator())
	m_serverState = OPC_STATUS_NOCONFIG;
	m_serverStateManuallySet = FALSE;
#ifdef SOFEATURE_SOAP
	SOCmnPointer<SOSrvHTTPHandler> hHTTP;
	SOCMN_ALLOCATE_OBJECT_C(hHTTP, SOSrvHTTPHandler(), (SOSrvHTTPHandler*))
	m_srvHTTP->setHTTPHandler(hHTTP);
#endif
#ifdef SOFEATURE_DCOM
#ifndef SOOS_WINDOWS_CE
	setRegisterCategories(SOSRVENTRY_CATEGORY_DA10 | SOSRVENTRY_CATEGORY_DA20 | SOSRVENTRY_CATEGORY_DA30);
#else
	setRegisterCategories(SOSRVENTRY_CATEGORY_DA20 | SOSRVENTRY_CATEGORY_DA30);
#endif
#endif
	::setSOSrvEntry(this);
}


SODaSEntry::~SODaSEntry(void)
{
	interlockedIncrement(&m_refCount);

	// destroy name space
	for (BYTE i = 0; i < SODAS_SERVER_INSTANCE_COUNT; i++)
	{
		if (m_nsRoot[i])
		{
			m_nsRoot[i]->removeAllAdditionalElements();
			m_nsRoot[i]->removeAllLeafs();
			m_nsRoot[i]->removeAllBranches();
			m_nsRoot[i]->release();
		}
	}

#ifdef SOFEATURE_SOAP
	m_srvHTTP->setHTTPHandler(NULL);
	m_srvHTTP.release();
#endif
	setConfigurationFileHandler(NULL);
	m_creator->release();
	g_engine = NULL;
	::setSOSrvEntry(NULL);
	interlockedDecrement(&m_refCount);
}


#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
//  initialize
//
// - initialize toolkit
//
// returns:
//		TRUE  - initialized
//		FALSE - error during init
//
BOOL SODaSEntry::initialize(
	IN enum serverTypeCOM type, // server type (inproc or outproc)
	IN REFCLSID clsid,          // class id of server
	IN HINSTANCE hInst,         // instance handle of application
	IN DWORD coInit)            // COM init mode
{
	BOOL ret = SOSrvEntry::initialize(type, clsid, hInst, coInit);

	// create object root
	if (!m_objectRoot)
	{
		m_objectRoot = m_creator->createObjectRoot();
	}

	// create public group server
	SOCmnPointer<SODaSServer> pg = m_creator->createServer(TRUE, 0);
	pg->setName(_T("PublicGroups"));
	m_objectRoot->addBranch(pg);
	return ret;
} // initialize
#endif

#ifdef SOFEATURE_SOAP
BOOL SODaSEntry::initializeHTTP(IN WORD port)
{
	BOOL ret = TRUE;

	if (port != 0)
	{
		ret = SOSrvEntry::initializeHTTP(port);
	}

	// create object root
	if (!m_objectRoot)
	{
		m_objectRoot = m_creator->createObjectRoot();
	}

	if ((ret) && (port != 0))
	{
		SOCmnPointer<SOSrvHTTPHandler> xmlSrv;
		xmlSrv = (SOSrvHTTPHandler*)m_srvHTTP->getHTTPHandler();

		if (xmlSrv.isNotNull())
		{
			ret = xmlSrv->initialize();
		}
	}

	return ret;
} // initializeHTTP

void SODaSEntry::createXMLSubscriptionServer(void)
{
	// create XML subscription server
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();
	m_xmlSubsServer = m_creator->createServer(FALSE, 0);
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
	m_xmlSubsServer->setClientName(_T("XML Subscriptions"));
	m_xmlSubsServer->setFlags(SODASSERVER_FLAG_SOAP);
	m_objectRoot->addBranch(m_xmlSubsServer);
}

SODaSServer* SODaSEntry::getXMLSubscriptionServer(void)
{
	if (!m_xmlSubsServer)
	{
		return NULL;
	}

	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(m_xmlSubsServer);
	SODaSServer* pXmlSubsServer;
	pObjectLock->lock();
	m_xmlSubsServer.addRef();
	pXmlSubsServer = m_xmlSubsServer;
	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(m_xmlSubsServer, pObjectLock);
	return pXmlSubsServer;
}

BOOL SODaSEntry::addXMLDAReceiver(IN LPCTSTR url)
{
	SOCmnPointer<SOSrvHTTPHandler> xmlSrv;
	BOOL ret = FALSE;
	xmlSrv = (SOSrvHTTPHandler*)m_srvHTTP->getHTTPHandler();

	if (xmlSrv.isNotNull())
	{
		createXMLSubscriptionServer();
		SOCmnPointer<SODaSXmlServer> xmlRec;
		SOCMN_ALLOCATE_OBJECT_C(xmlRec, SODaSXmlServer(), (SODaSXmlServer*))

		if (url)
			if (_tcslen(url) > 0)
			{
				xmlRec->setURL(url);
			}

		ret = xmlSrv->addHTTPReceiver(xmlRec);
	}

	return ret;
}

#endif

#ifdef SOFEATURE_TUNNEL

BOOL SODaSEntry::addTPDAReceiver(void)
{
	BOOL ret;
	SOCmnPointer<SODaSTPServer> tpRec;
	SOCMN_ALLOCATE_OBJECT_C(tpRec, SODaSTPServer(), (SODaSTPServer*))
	ret = m_srvTP->addTPReceiver(tpRec);
	SOCmnPointer<SODaSTPGroup> tpRecG;
	SOCMN_ALLOCATE_OBJECT_C(tpRecG, SODaSTPGroup(), (SODaSTPGroup*))
	ret &= m_srvTP->addTPReceiver(tpRecG);
	return ret;
}

#endif

//-----------------------------------------------------------------------------
// terminate
//
// - terminate toolkit
//
// returns:
//		TRUE  - terminated
//		FALSE - error during terminate
//
BOOL SODaSEntry::terminate(void)
{
	SOCmnPointer<SODaSServer> pg;

	if ((SODaSServer*)(pg = getPublicGroups()))
	{
		m_objectRoot->removeBranch(pg);
		pg->removeAllLeafs();
		pg->removeAllBranches();
	}

#ifdef SOFEATURE_SOAP

	if (m_xmlSubsServer.isNotNull())
	{
		m_xmlSubsServer->stopWorkThread();
	}

	if (m_objectRoot != NULL)
	{
		m_objectRoot->removeBranch(m_xmlSubsServer);
	}

	m_xmlSubsServer.release();
#endif
	return SOSrvEntry::terminate();
} // terminate


//-----------------------------------------------------------------------------
// setConfigurationFileHandler
//
// - set configuration file handler object
//
void SODaSEntry::setConfigurationFileHandler(
	IN SODaSConfigFileHandler* cfgHandler)  // configuration handler object
{
	if (m_cfgHandler)
	{
		m_cfgHandler->release();
	}

	m_cfgHandler = cfgHandler;

	if (m_cfgHandler)
	{
		m_cfgHandler->addRef();
	}
} // setConfigurationFileHandler


//-----------------------------------------------------------------------------
// setCreator
//
// - set OPC object factory
//
void SODaSEntry::setCreator(
	IN SODaSCreator* creator)   // OPC object factory
{
	if (creator)
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
	}
} // setCreator


//-----------------------------------------------------------------------------
// getNameSpaceRoot
//
// - get namespace root object
//
// returns:
//		namespace root
//
SODaSNameSpaceRoot* SODaSEntry::getNameSpaceRoot(
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SODAS_SERVER_INSTANCE_COUNT)
	{
		return NULL;
	}

	SOCmnSingleLock<SOCmnSync> lock(&g_namespaceSync);

	if (m_nsRoot[serverInstance] != NULL)
	{
		m_nsRoot[serverInstance]->addRef();
	}
	else
	{
		m_nsRoot[serverInstance] = m_creator->createNameSpaceRoot(serverInstance);

		if (m_nsRoot[serverInstance] != NULL)
		{
			m_nsRoot[serverInstance]->addRef();
		}
	}

	return m_nsRoot[serverInstance];
} // getNameSpaceRoot


//-----------------------------------------------------------------------------
// createNewNameSpaceRoot
//
// - release the old namespace object -> a new one is created via the next access
//
void SODaSEntry::createNewNameSpaceRoot(
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SODAS_SERVER_INSTANCE_COUNT)
	{
		return;
	}

	if (m_nsRoot[serverInstance] != NULL)
	{
		m_nsRoot[serverInstance]->release();
		m_nsRoot[serverInstance] = NULL;
	}

	// reset the browse position of all servers to the new namespace root
	SOCmnList<SODaSServer> serverList((SOCmnList<SODaSServer> *)m_objectRoot->getBranchList());
	SOCmnListPosition posSrv;
	SODaSServer* server;
	posSrv = serverList.getStartPosition();

	while (posSrv)
	{
		server = serverList.getNext(posSrv);
		server->changeBrowsePosition(SOSrvBrowser::dirTo, _T(""));
	}

	serverList.autoUnlock();
} // createNewNameSpaceRoot


//-----------------------------------------------------------------------------
// getPublicGroups
//
// - get public group server object
//
// returns:
//		public group server object
//
SODaSServer* SODaSEntry::getPublicGroups(void)
{
	if (!m_objectRoot)
	{
		return NULL;
	}

	SODaSServer* server;
	SOCmnList<SOCmnElement> list(m_objectRoot->getBranchList());
	SOCmnListPosition pos;
	pos = list.getStartPosition();

	while (pos)
	{
		server = (SODaSServer*)list.getNext(pos);

		if (server->isPublicGroups())
		{
			// public groups server found
			server->addRef();
			return (SODaSServer*)server;
		}
	}

	return NULL;
} // getPublicGroups


//-----------------------------------------------------------------------------
// getSrvCreator
//
// - get toolkit object factory
//
// returns:
//		toolkit object factory
//
SOSrvCreator* SODaSEntry::getSrvCreator(void)
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return (SOSrvCreator*)m_creator;
} // getSrvCreator

static TCHAR s_serverStateString[6][10] =
{ _T("running"), _T("failed"), _T("noConfig"), _T("suspended"), _T("test"), _T("commFault") };

LPCTSTR SODaSEntry::getServerStateString(void) const
{
	return s_serverStateString[m_serverState - 1];
}

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// addWatchObject
//
// - create and add watch object to watch list
//
// returns:
//		pointer to watch object
//
SOSrvWatch* SODaSEntry::addWatchObject(void)
{
	SOSrvWatch* watch = SOSrvEntry::addWatchObject();

	if (watch)
	{
		SOCmnPointer<SODaSWatchHandler> wh = new SODaSWatchHandler();
		watch->setDAHandler(wh);
	}

	return watch;
} // addWatchObject


//-----------------------------------------------------------------------------
// incObjectCountStatistics
//
// - increment one object statistics count
//
void SODaSEntry::incObjectCountStatistics(
	IN DWORD objType)  // object type
{
	switch (objType)
	{
	case SOCMNOBJECT_TYPE_SERVER_DA:
		m_statistics.serverObjs++;
		break;

	case SOCMNOBJECT_TYPE_GROUP:
		m_statistics.groupObjs++;
		break;

	case SOCMNOBJECT_TYPE_ITEM:
		m_statistics.itemObjs++;
		break;

	case SOCMNOBJECT_TYPE_NODE:
		m_statistics.nodeObjs++;
		break;

	case SOCMNOBJECT_TYPE_TAG:
		m_statistics.tagObjs++;
		break;

	case SOCMNOBJECT_TYPE_PROPERTY:
		m_statistics.propertyObjs++;
		break;
	}
} // incObjectCountStatistics


//-----------------------------------------------------------------------------
// decObjectCountStatistics
//
// - decrement one object statistics count
//
void SODaSEntry::decObjectCountStatistics(
	IN DWORD objType) // object type
{
	switch (objType)
	{
	case SOCMNOBJECT_TYPE_SERVER_DA:
		m_statistics.serverObjs--;
		break;

	case SOCMNOBJECT_TYPE_GROUP:
		m_statistics.groupObjs--;
		break;

	case SOCMNOBJECT_TYPE_ITEM:
		m_statistics.itemObjs--;
		break;

	case SOCMNOBJECT_TYPE_NODE:
		m_statistics.nodeObjs--;
		break;

	case SOCMNOBJECT_TYPE_TAG:
		m_statistics.tagObjs--;
		break;

	case SOCMNOBJECT_TYPE_PROPERTY:
		m_statistics.propertyObjs--;
		break;
	}
} // decObjectCountStatistics


//-----------------------------------------------------------------------------
// getObjectCountStatistics
//
// - get the statistics
//
// returns:
//		S_OK   - got statistics
//		E_FAIL - error
//
void SODaSEntry::getObjectCountStatistics(
	IN SOCmnStringEx& szXML)
{
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRSERVEROBJ, m_statistics.serverObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRGROUPOBJ, m_statistics.groupObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRITEMOBJ, m_statistics.itemObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRNODEOBJ, m_statistics.nodeObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRTAGOBJ, m_statistics.tagObjs);
	SOSrvWatch::createElementText(szXML, SOWATCH_STAT_DA_NRPROPERTYOBJ, m_statistics.propertyObjs);
} // getObjectCountStatistics

#endif // SOFEATURE_WATCH

#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// ATL COM Helpers for _Module access
//
void SODaSEntry::term(void)
{
	_Module.Term();
}
void SODaSEntry::init(
	IN HINSTANCE instanceHandle)
{
	g_clsidDA0 = m_clsid;
	_Module.Init(g_objectMap, instanceHandle);
}
HRESULT SODaSEntry::updateRegistryWithRGS(IN BOOL bRegister, IN void* replacementArray)
{
#ifndef SOOS_WINDOWS_CE
#if (_MSC_VER > 1200)
	HINSTANCE hInstSave = _AtlBaseModule.m_hInst;
	_AtlBaseModule.m_hInst = m_appInstanceHandle;
#endif
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, bRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#if (_MSC_VER > 1200)
	_AtlBaseModule.m_hInst = hInstSave;
#endif
#else
	HRESULT res;
	res = _Module.UpdateRegistryFromResource(m_regRGSResID, bRegister, (_ATL_REGMAP_ENTRY*)replacementArray);
#endif
	return res;
}

HRESULT SODaSEntry::getClassObject(
	IN REFCLSID rclsid,
	IN REFIID riid,
	OUT LPVOID* ppvObj)
{
	return _Module.GetClassObject(rclsid, riid, ppvObj);
}
HRESULT SODaSEntry::canUnloadNow(void)
{
	LONG lockCount = _Module.GetLockCount();
	return (lockCount == 0) ? S_OK : S_FALSE;
}
HRESULT SODaSEntry::registerClassObjects(void)
{
	return _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
}
HRESULT SODaSEntry::revokeClassObjects(void)
{
	return _Module.RevokeClassObjects();
}
HRESULT SODaSEntry::regServer(void)
{
	return _Module.RegisterServer(FALSE);
}
HRESULT SODaSEntry::unregServer(void)
{
	return _Module.UnregisterServer();
}
BOOL SODaSEntry::addComObjMapEntry(
	IN const CLSID* pClsid,
	IN void* pfnUpdateRegistry,
	IN void* pfnGetClassObject,
	IN void* pfnCreateInstance,
	IN void* pfnGetObjectDescription,
	IN void* pfnGetCategoryMap,
	IN void* pfnObjectMain)
{
	DWORD i;

	for (i = 1; i < 11; i++)
	{
		if (NULL == g_objectMap[i].pclsid)
		{
			g_objectMap[i].pclsid = pClsid;
			g_objectMap[i].pfnUpdateRegistry = (HRESULT(WINAPI*)(BOOL))pfnUpdateRegistry;
			g_objectMap[i].pfnGetClassObject = (_ATL_CREATORFUNC*)pfnGetClassObject;
			g_objectMap[i].pfnCreateInstance = (_ATL_CREATORFUNC*)pfnCreateInstance;
			g_objectMap[i].pCF = NULL;
			g_objectMap[i].dwRegister = 0;
			g_objectMap[i].pfnGetObjectDescription = (_ATL_DESCRIPTIONFUNC*)pfnGetObjectDescription;
#if (_ATL_VER >= 0x0300)
#ifndef SOOS_WINDOWS_CE
			g_objectMap[i].pfnGetCategoryMap = (_ATL_CATMAPFUNC*)pfnGetCategoryMap;
#endif
			g_objectMap[i].pfnObjectMain = (void (WINAPI*)(bool))pfnObjectMain;
#endif
			return TRUE;
		}
	}

	return FALSE;
}


BOOL SODaSEntry::addComObjMapEntry(IN const CLSID* clsid, IN DWORD cnt)
{
	BOOL ret = FALSE;

	switch (cnt)
	{
	case 1:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer1));
		g_clsidDA1 = *clsid;
		break;

	case 2:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer2));
		g_clsidDA2 = *clsid;
		break;

	case 3:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer3));
		g_clsidDA3 = *clsid;
		break;

	case 4:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer4));
		g_clsidDA4 = *clsid;
		break;

	case 5:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer5));
		g_clsidDA5 = *clsid;
		break;

	case 6:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer6));
		g_clsidDA6 = *clsid;
		break;

	case 7:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer7));
		g_clsidDA7 = *clsid;
		break;

	case 8:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer8));
		g_clsidDA8 = *clsid;
		break;

	case 9:
		ret = addComObjMapEntry(SOSRVENTRY_COM_OBJMAP_ENTRY(*clsid, SODaSComServer9));
		g_clsidDA9 = *clsid;
		break;
	}

	return ret;
}


BOOL SODaSEntry::getCLSID(OUT CLSID& clsid, IN OPTIONAL BYTE serverInstance)
{
	BOOL ret = TRUE;

	switch (serverInstance)
	{
	case 0:
		clsid = g_clsidDA0;
		break;

	case 1:
		clsid = g_clsidDA1;
		break;

	case 2:
		clsid = g_clsidDA2;
		break;

	case 3:
		clsid = g_clsidDA3;
		break;

	case 4:
		clsid = g_clsidDA4;
		break;

	case 5:
		clsid = g_clsidDA5;
		break;

	case 6:
		clsid = g_clsidDA6;
		break;

	case 7:
		clsid = g_clsidDA7;
		break;

	case 8:
		clsid = g_clsidDA8;
		break;

	case 9:
		clsid = g_clsidDA9;
		break;

	default:
		clsid = CLSID_NO;
		ret = FALSE;
		break;
	}

	return ret;
}

#endif // SOFEATURE_DCOM

#endif // SODAS
