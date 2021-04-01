#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>
#include <SOAeSComAreaBrowser.h>


//-----------------------------------------------------------------------------
// SOAeSAreaBrowser                                                           |
//-----------------------------------------------------------------------------

SOAeSAreaBrowser::SOAeSAreaBrowser(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_AREA_BROWSER)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_AREA_BROWSER);
#endif
}

SOAeSAreaBrowser::~SOAeSAreaBrowser(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_AREA_BROWSER);
#endif
}

//-----------------------------------------------------------------------------
// getMe
//
// - get reference to COM object
//
// returns:
//		pointer of COM object
//
void* SOAeSAreaBrowser::getMe(void)
{
	m_meSync.lock();
	SOSrvComObject<SOAeSComAreaBrowser> *ab = (SOSrvComObject<SOAeSComAreaBrowser> *)m_me;

	if (ab)
	{
		ab->tempAddRef();
	}

	m_meSync.unlock();
	return m_me;
} // getMe


//-----------------------------------------------------------------------------
// releaseMe
//
// - release reference to COM object obtained by getMe
//
// returns:
//		temporary reference count of COM object
//
ULONG SOAeSAreaBrowser::releaseMe(
	IN void* /* me */)  // pointer to COM object
{
	SOSrvComObject<SOAeSComAreaBrowser> *ab = (SOSrvComObject<SOAeSComAreaBrowser> *)m_me;

	if (ab)
	{
		return ab->tempRelease();
	}

	return 0;
} // releaseMe


//-----------------------------------------------------------------------------
// isLeafName
//
// - check if the string represents a leaf in the string based space
// - calls the method of the server object
//
// returns:
//		TRUE  - it's a leaf
//		FALSE - it's no leaf
//
BOOL SOAeSAreaBrowser::isLeafName(
	IN LPCTSTR fullName)    // fully qualified leaf name
{
	SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

	if (!parent)
	{
		return FALSE;
	}

	return parent->isSourceName(fullName);
} // isLeafName


//-----------------------------------------------------------------------------
// isBranchName
//
// - check if the string represents a branch in the string based space
// - calls the method of the server object
//
// returns:
//		TRUE  - it's a branch
//		FALSE - it's no branch
//
BOOL SOAeSAreaBrowser::isBranchName(
	IN LPCTSTR fullName)    // fully qualified branch name
{
	SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

	if (!parent)
	{
		return FALSE;
	}

	return parent->isAreaName(fullName);
} // isBranchName


//-----------------------------------------------------------------------------
// getChildNames
//
// - called to receive the string based child names
// - calls the method of the server object
//
// returns:
//		S_OK
//
HRESULT SOAeSAreaBrowser::getChildNames(
	IN enum browserFilterType browseType,   // browse type
	IN LPCTSTR filterCriteria,              // string filter
	IN VARTYPE /* dataTypeFilter */,        // data type filter
	IN DWORD /* accessRightsFilter */,      // access filter
	OUT SOCmnStringList& nameList)          // list of dynamic child names
{
	SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		parent->getChildNamesOfAreaName(m_curBrowsePath,
										(browseType == typeLeaf) ? FALSE : TRUE, filterCriteria, nameList);
	}

	return S_OK;
} // getChildNames


//-----------------------------------------------------------------------------
// browseOnlyStringBased
//
// - use only the string based address space for browsing
//
// returns:
//		TRUE  - only string based
//		FALSE - object based and string based
//
BOOL SOAeSAreaBrowser::browseOnlyStringBased(void)
{
	SOCmnPointer<SOAeSServer> parent = (SOAeSServer*)getParentBranch();

	if (parent.isNotNull())
	{
		parent->browseOnlyStringBased();
	}

	return FALSE;
} // browseOnlyStringBased


//-----------------------------------------------------------------------------
// getBrowserRoot
//
// - get the root object of the browsing tree
//
// returns:
//		root object
//
SOCmnElement* SOAeSAreaBrowser::getBrowserRoot(void)
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSServer> server = (SOAeSServer*)getParentBranch();

	if (server.isNotNull())
	{
		return entry->getAreaSpaceRoot(server->getServerInstance());
	}
	else
	{
		return NULL;
	}
} // getBrowserRoot


//-----------------------------------------------------------------------------
// getObjectSrvBrowser
//
// - get object using the SOSrvBrowser class via multiple inheritance
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOAeSAreaBrowser::getObjectSrvBrowser(void)
{
	return (SOCmnObject*)this;
} // getObjectSrvBrowser


//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//		E_NOTIMPL     - no query interface implementation for this interface
//		S_OK          - interface delivered
//		E_NOINTERFACE - interface not supported
//
HRESULT SOAeSAreaBrowser::queryInterface(
	IN REFIID /* iid */,        // interface id
	IN void** /* object */)     // interface pointer
{
	return E_NOTIMPL;    // queryInterface
}


#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// checkAuthorizationDCOM
//
// - check if the call to the COM interface function is authorizated
//
// returns:
//   S_OK           - allowed access
//   E_ACCESSDENIED - denied access
//
HRESULT SOAeSAreaBrowser::checkAuthorizationDCOM(
	IN LPCTSTR /* ifFunction */)
{
	return S_OK;
} // checkAuthorizationDCOM
#endif


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSAreaBrowser::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_OBJECT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_AREABROWSER);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif

#ifdef SOFEATURE_COMIF_HOOKS
// interface function callbacks
HRESULT SOAeSAreaBrowser::onBChangeBrowsePosition(IN OPCAEBROWSEDIRECTION browseDirection, IN LPCWSTR string)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  dir:%d string:%s"), browseDirection, W2CT(string)));
	return E_NOTIMPL;
}
HRESULT SOAeSAreaBrowser::onBBrowseOPCAreas(IN OPCAEBROWSETYPE browseFilterType, IN LPCWSTR filterCriteria, OUT LPENUMSTRING* enumString)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  type:%d filter:%s"), browseFilterType, W2CT(filterCriteria)));
	return E_NOTIMPL;
}
HRESULT SOAeSAreaBrowser::onBGetQualifiedAreaName(IN LPCWSTR areaName, OUT LPWSTR* qualifiedAreaName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  areaName:%s"), W2CT(areaName)));
	return E_NOTIMPL;
}
HRESULT SOAeSAreaBrowser::onBGetQualifiedSourceName(IN LPCWSTR sourceName, OUT LPWSTR* qualifiedSourceName)
{
	_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  sourceName:%s"), W2CT(sourceName)));
	return E_NOTIMPL;
}
void SOAeSAreaBrowser::onEChangeBrowsePosition(IN HRESULT res, IN OPCAEBROWSEDIRECTION browseDirection, IN LPCWSTR string) {}
void SOAeSAreaBrowser::onEBrowseOPCAreas(IN HRESULT res, IN OPCAEBROWSETYPE browseFilterType, IN LPCWSTR filterCriteria, OUT LPENUMSTRING* enumString) {}
void SOAeSAreaBrowser::onEGetQualifiedAreaName(IN HRESULT res, IN LPCWSTR areaName, OUT LPWSTR* qualifiedAreaName)
{
	if (SUCCEEDED(res))
	{
		_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  qualifiedAreaName:%s"), W2CT(*qualifiedAreaName)));
	}
}
void SOAeSAreaBrowser::onEGetQualifiedSourceName(IN HRESULT res, IN LPCWSTR sourceName, OUT LPWSTR* qualifiedSourceName)
{
	if (SUCCEEDED(res))
	{
		_TRACEW(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL, (_T("  qualifiedSourceName:%s"), W2CT(*qualifiedSourceName)));
	}
}
#endif // SOFEATURE_COMIF_HOOKS

#endif // SOAES

