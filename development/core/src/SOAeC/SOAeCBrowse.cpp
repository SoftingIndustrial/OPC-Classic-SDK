#include "stdafx.h"

#ifdef SOAEC

#include "SOAeCBrowse.h"
#include "SOAeCServer.h"


//-----------------------------------------------------------------------------
// SOAeCAreaBrowser                                                           |
//-----------------------------------------------------------------------------

SOAeCAreaBrowser::SOAeCAreaBrowser(SOAeCServer* pServer)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_AREA_BROWSER)
{
	m_server = pServer;
	m_server.addRef();
	m_delimNScreate = _T('\x7f');
	m_delimNSaccept = _T("\x7f");
	m_badNSchars = _T("");
}

SOAeCAreaBrowser::~SOAeCAreaBrowser()
{
}

BOOL SOAeCAreaBrowser::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	return expandImpl(this, type, objList);
}

//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SOAeCAreaBrowser::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	if (accept)
	{
		*accept = m_delimNSaccept;
	}

	if (bad)
	{
		*bad = m_badNSchars;
	}

	return m_delimNScreate;
} // getTreeDelimiter


//-----------------------------------------------------------------------------
//
// getDisplayName
//

SOCmnString SOAeCAreaBrowser::getDisplayName()
{
	_ASSERTION(!!m_server, "No server object");
	return getServer()->getServerName();
}

SOAeCAreaBrowser* SOAeCAreaBrowser::getObjectSOAeCAreaBrowser(void)
{
	return this;
}


SOAeCAreaBrowserICommunication::SOAeCAreaBrowserICommunication(void) {}
SOAeCAreaBrowserICommunication::~SOAeCAreaBrowserICommunication(void) {}
BOOL SOAeCAreaBrowserICommunication::expandImpl(
	IN SOCltBrowseObject* /* pParent */,
	IN DWORD /* type */,
	OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}

SOAeCAreaBrowser* SOAeCAreaBrowserICommunication::getObjectSOAeCAreaBrowser(void)
{
	return NULL;
}
SOCmnString SOAeCAreaBrowserICommunication::getPathImpl(IN SOCltBrowseObject* /* pParent */)
{
	return _T("");
}


//-----------------------------------------------------------------------------
// SOAeCArea                                                                  |
//-----------------------------------------------------------------------------

SOAeCArea::SOAeCArea(SOAeCAreaBrowser* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_AREA)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_browser = parent;
	m_browser.addRef();
	m_name = W2CT(wszName);
	m_path.create();
}

SOAeCArea::SOAeCArea(SOAeCArea* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_AREA)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_browser = parent->getAreaSpaceBrowser();
	m_name = W2CT(wszName);
	m_path.clone(parent->m_path);
	m_path.add(parent->m_name);
}

SOAeCArea::SOAeCArea(IN SOCltBrowseObject* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_AREA)
{
	if (parent->is(SOCMNOBJECT_TYPE_AREA))
	{
		m_browser = ((SOAeCArea*)parent)->getAreaSpaceBrowser();
		m_path.clone(&((SOAeCArea*)parent)->m_path);
		m_path.add(parent->getName());
	}
	else if (parent->is(SOCMNOBJECT_TYPE_AREA_BROWSER))
	{
		m_browser = (SOAeCAreaBrowser*)parent;
		m_browser.addRef();
		m_path.create();
	}
	else
	{
		return;
	}

	m_name = wszName;
}


SOAeCArea::~SOAeCArea()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCArea::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	return m_browser->expandImpl(this, type, objList);
}

//-----------------------------------------------------------------------------
//
// getPath
//	Returns the fully qualified name

SOCmnString SOAeCArea::getPath()
{
	return m_browser->getPathImpl(this);
}



//-----------------------------------------------------------------------------
// SOAeCSource                                                                |
//-----------------------------------------------------------------------------

SOAeCSource::SOAeCSource(SOAeCAreaBrowser* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_SOURCE)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_browser = parent;
	m_browser.addRef();
	m_name = W2CT(wszName);
	m_path.create();
}

SOAeCSource::SOAeCSource(SOAeCArea* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_SOURCE)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_browser = parent->getAreaSpaceBrowser();
	m_path.clone(parent->getPathList());
	m_path.add(parent->getName());
	m_name = W2CT(wszName);
}

SOAeCSource::SOAeCSource(IN SOCltBrowseObject* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_SOURCE)
{
	if (parent->is(SOCMNOBJECT_TYPE_AREA))
	{
		m_browser = ((SOAeCArea*)parent)->getAreaSpaceBrowser();
		m_path.clone(((SOAeCArea*)parent)->getPathList());
		m_path.add(parent->getName());
	}
	else if (parent->is(SOCMNOBJECT_TYPE_AREA_BROWSER))
	{
		m_browser = (SOAeCAreaBrowser*)parent;
		m_browser.addRef();
		m_path.create();
	}
	else
	{
		return;
	}

	m_name = wszName;
}

SOAeCSource::~SOAeCSource()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCSource::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject>& objList)
{
	return m_browser->expandImpl(this, type, objList);
}

//-----------------------------------------------------------------------------
//
// getPath
//	Returns the fully qualified name

SOCmnString SOAeCSource::getPath()
{
	if (m_fullPath.isEmpty())
	{
		return m_browser->getPathImpl(this);
	}
	else
	{
		return m_fullPath;
	}
}




//-----------------------------------------------------------------------------
// SOAeCCondition                                                             |
//-----------------------------------------------------------------------------

SOAeCCondition::SOAeCCondition(SOAeCSource* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CONDITION)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_name = W2CT(wszName);
	m_browser = parent->getAreaSpaceBrowser();
}

SOAeCCondition::SOAeCCondition(IN SOCltBrowseObject* parent, LPCTSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CONDITION)
{
	if (parent->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		m_name = wszName;
		m_browser = ((SOAeCSource*)parent)->getAreaSpaceBrowser();
	}
}

SOAeCCondition::~SOAeCCondition()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCCondition::expand(
	IN DWORD /* type */, OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}




//-----------------------------------------------------------------------------
// SOAeCEventBrowser                                                          |
//-----------------------------------------------------------------------------

SOAeCEventBrowser::SOAeCEventBrowser(SOAeCServer* pServer)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_EVENT_BROWSER)
{
	m_filterCategories = OPC_ALL_EVENTS;
	m_server = pServer;
	m_server.addRef();
}

SOAeCEventBrowser::~SOAeCEventBrowser()
{
}


//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCEventBrowser::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	return expandImpl(this, type, objList);
}

//-----------------------------------------------------------------------------
//
// getDisplayName
//

SOCmnString SOAeCEventBrowser::getDisplayName()
{
	_ASSERTION(!!m_server, "No server object");
	return getServer()->getServerName();
}

SOAeCEventBrowser* SOAeCEventBrowser::getObjectSOAeCEventBrowser(void)
{
	return this;
}


SOAeCEventBrowserICommunication::SOAeCEventBrowserICommunication(void) {}
SOAeCEventBrowserICommunication::~SOAeCEventBrowserICommunication(void) {}
BOOL SOAeCEventBrowserICommunication::expandImpl(
	IN SOCltBrowseObject* /* pParent */,
	IN DWORD /* type */,
	OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}

SOAeCEventBrowser* SOAeCEventBrowserICommunication::getObjectSOAeCEventBrowser(void)
{
	return NULL;
}


//-----------------------------------------------------------------------------
// SOAeCCategory                                                              |
//-----------------------------------------------------------------------------

SOAeCCategory::SOAeCCategory(SOAeCEventBrowser* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CATEGORY)
{
	USES_CONVERSION;
	m_name = W2CT(wszName);
	m_browser = parent;
	m_browser.addRef();
	m_eventType = 0;
	m_id = 0;
}

SOAeCCategory::SOAeCCategory(IN SOCltBrowseObject* parent, LPCTSTR name)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CATEGORY)
{
	if (parent->is(SOCMNOBJECT_TYPE_EVENT_BROWSER))
	{
		m_browser = (SOAeCEventBrowser*)parent;
		m_browser.addRef();
	}
	else
	{
		return;
	}

	m_name = name;
	m_eventType = 0;
	m_id = 0;
}

SOAeCCategory::~SOAeCCategory()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCCategory::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	return m_browser->expandImpl(this, type, objList);
}




//-----------------------------------------------------------------------------
// SOAeCAttribute                                                             |
//-----------------------------------------------------------------------------

SOAeCAttribute::SOAeCAttribute(SOAeCCategory* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_ATTRIBUTE)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_name = W2CT(wszName);
	m_browser = parent->getEventSpaceBrowser();
}

SOAeCAttribute::SOAeCAttribute(IN SOCltBrowseObject* parent, LPCTSTR name)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_ATTRIBUTE)
{
	if (parent->is(SOCMNOBJECT_TYPE_CATEGORY))
	{
		m_browser = ((SOAeCCategory*)parent)->getEventSpaceBrowser();
	}
	else
	{
		return;
	}

	m_name = name;
}

SOAeCAttribute::~SOAeCAttribute()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCAttribute::expand(
	IN DWORD /* type */,
	OUT SOCmnList<SOCltBrowseObject>& /* objList */)
{
	return FALSE;
}


//-----------------------------------------------------------------------------
// SOAeCConditionClass                                                        |
//-----------------------------------------------------------------------------

SOAeCConditionClass::SOAeCConditionClass(SOAeCCategory* parent, LPCWSTR wszName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CONDITION_CLASS)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_name = W2CT(wszName);
	m_browser = parent->getEventSpaceBrowser();
}

SOAeCConditionClass::SOAeCConditionClass(IN SOCltBrowseObject* parent, LPCTSTR name)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_CONDITION_CLASS)
{
	if (parent->is(SOCMNOBJECT_TYPE_CATEGORY))
	{
		m_browser = ((SOAeCCategory*)parent)->getEventSpaceBrowser();
	}
	else
	{
		return;
	}

	m_name = name;
}

SOAeCConditionClass::~SOAeCConditionClass()
{
}

//-----------------------------------------------------------------------------
//
// expand
//

BOOL SOAeCConditionClass::expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList)
{
	return m_browser->expandImpl(this, type, objList);
}




//-----------------------------------------------------------------------------
// SOAeCSubConditionClass                                                     |
//-----------------------------------------------------------------------------

SOAeCSubConditionClass::SOAeCSubConditionClass(SOAeCConditionClass* parent, LPCWSTR szName)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS)
{
	USES_CONVERSION;
	_ASSERTION(!!parent, "No parent object");
	m_name = W2CT(szName);
	m_browser = parent->getEventSpaceBrowser();
}

SOAeCSubConditionClass::SOAeCSubConditionClass(IN SOCltBrowseObject* parent, LPCTSTR name)
	: SOCltBrowseObject(SOCMNOBJECT_TYPE_SUBCONDITION_CLASS)
{
	if (parent->is(SOCMNOBJECT_TYPE_CONDITION_CLASS))
	{
		m_browser = ((SOAeCConditionClass*)parent)->getEventSpaceBrowser();
	}
	else
	{
		return;
	}

	m_name = name;
}

SOAeCSubConditionClass::~SOAeCSubConditionClass()
{
}

BOOL SOAeCSubConditionClass::expand(IN DWORD /*type*/, OUT SOCmnList<SOCltBrowseObject>& /*objList*/)
{
	return FALSE;
}

#endif // SOAEC
