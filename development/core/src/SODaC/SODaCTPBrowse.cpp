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
//  Filename    : SODaCTPBrowse.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA Browsing objects                                         |
//                - SODaCTPNameSpaceBrowserImpl: name space browsing root     |
//                  object                                                    |
//                - SODaCNode: node browser object class                      |
//                - SODaCTag: tag browser object class                        |
//                - SODaCProperty: property browser object class              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAC

#ifdef SOFEATURE_TUNNEL

#include "SODaCBrowse.h"
#include "SODaCServer.h"
#include "SODaCTPBrowse.h"
#include "SODaCTPCall.h"


//-----------------------------------------------------------------------------
// SODaCTPNameSpaceBrowserImpl                                                |
//-----------------------------------------------------------------------------

SODaCTPNameSpaceBrowserImpl::SODaCTPNameSpaceBrowserImpl(SODaCServer* /* pSrv */)
{
}

SODaCTPNameSpaceBrowserImpl::~SODaCTPNameSpaceBrowserImpl()
{
}


BOOL SODaCTPNameSpaceBrowserImpl::expand20(
	IN SOCltBrowseObject* /* pParent */,
	IN LPCTSTR /* pItemID */,
	IN DWORD /* type */,
	OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	server->setLastError(E_NOTIMPL, SOCLT_ERR_BROWSE_NOTSUPPORTED);
	return FALSE;
}



BOOL SODaCTPNameSpaceBrowserImpl::expand30(
	IN SOCltBrowseObject* pParent,
	IN LPCTSTR itemID,
	IN LPCTSTR /* itemPath */,
	IN DWORD type,
	IN SOCmnString& contPoint,
	OUT SOCmnList<SOCltBrowseObject> &objList)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	DWORD browseFilter = 0;
	SOCmnString cpSZ;

	if (!contPoint.isEmpty())
	{
		cpSZ = contPoint;
	}

	contPoint.empty();

	if (!objList.isInit())
	{
		objList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}

	// calculate the browse type
	if ((SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM) == (type & (SOCMNOBJECT_TYPE_NODE | SOCMNOBJECT_TYPE_ITEM)))
	{
		browseFilter = OPC_BROWSE_FILTER_ALL;
	}
	else
	{
		if (SOCMNOBJECT_TYPE_NODE == (type & SOCMNOBJECT_TYPE_NODE))
		{
			browseFilter = OPC_BROWSE_FILTER_BRANCHES;
		}
		else if (SOCMNOBJECT_TYPE_ITEM == (type & SOCMNOBJECT_TYPE_ITEM))
		{
			browseFilter = OPC_BROWSE_FILTER_ITEMS;
		}
	}

	if (browseFilter != 0)
	{
		SOCmnString elementNameFilter = pBrowser->getElementNameFilter();
		SOCmnString vendorFilter = pBrowser->getVendorFilter();
		SOCmnPointer<SOCltTPConnection> tpCon = server->getTPConnection();
		SOCmnPointer<SODaCTPCall> tpCall;
		HRESULT res;
		SOCMN_ALLOCATE_OBJECT_C(tpCall, SODaCTPCall(tpCon), (SODaCTPCall*));
		server->beforeInterfaceCall();
		res = tpCall->browse(pParent, itemID, cpSZ, pBrowser->getMaxElements(), browseFilter, elementNameFilter, vendorFilter, pBrowser->getReturnProperties(), pBrowser->getRetrievePropertyValues(), NULL, objList, &contPoint, NULL);
		server->afterInterfaceCall();
		_TRACEO_LEVEL_FROM_RES(res, SOCMNTRACE_G_OPC_CALL_C, server, (_T("TPDA::Server::Browse [0x%X]"), (unsigned int)res));

		if (FAILED(res))
		{
			server->setLastError(res, SODAC_ERR_SRV_BROWSE);
		}
	}

	return TRUE;
}


LPCTSTR SODaCTPNameSpaceBrowserImpl::retrieveItemID(SOCltBrowseObject* pObj)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	SOCmnString path;
	USES_CONVERSION;

	if (!pObj->is(SOCMNOBJECT_TYPE_PROPERTY))
	{
	}
	else
	{
		// property
	}

	return (LPCTSTR)path;
}


void SODaCTPNameSpaceBrowserImpl::initUse()
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	pBrowser->setObjectState(SODACNAMESPACEBROWSER_STATE_USE_IOPCBROWSE);
}

void SODaCTPNameSpaceBrowserImpl::use20(void)
{
	SODaCNameSpaceBrowser* pBrowser = getObjectSODaCNameSpaceBrowser();
	SOCmnPointer<SODaCServer> server = (SODaCServer*)pBrowser->getServer();
	server->setLastError(E_NOTIMPL, SOCLT_ERR_BROWSE_NOTSUPPORTED);
}

void SODaCTPNameSpaceBrowserImpl::use30(void)
{
}

BOOL SODaCTPNameSpaceBrowserImpl::support20(void)
{
	return FALSE;
}

BOOL SODaCTPNameSpaceBrowserImpl::support30(void)
{
	return TRUE;
}



SODaCTPNameSpaceBrowser::SODaCTPNameSpaceBrowser(SODaCServer* pServer)
	: SODaCTPNameSpaceBrowserImpl(pServer),
	  SODaCNameSpaceBrowser(pServer)
{
	initUse();
}

#endif // SOFEATURE_TUNNEL

#endif // SODAC
