#include "stdafx.h"

#ifdef SOCLT

#include "SOCltBrowse.h"

//-----------------------------------------------------------------------------
// SOCltBrowseObject                                                          |
//-----------------------------------------------------------------------------

SOCltBrowseObject::SOCltBrowseObject(IN DWORD objType)                  // object type
	: SOCmnElement(objType)
{
}

SOCltBrowseObject::~SOCltBrowseObject()
{
}

//-----------------------------------------------------------------------------
//
// getPath
//

SOCmnString SOCltBrowseObject::getPath(void)
{
	return _T("");
}

//-----------------------------------------------------------------------------
//
// getDisplayName
//

SOCmnString SOCltBrowseObject::getDisplayName(void)
{
	return getName();
}

//-----------------------------------------------------------------------------
//
// isExpandable
//


BOOL SOCltBrowseObject::isExpandable(void)
{
	switch (getObjectType())
	{
	default:
		return FALSE;

	case SOCMNOBJECT_TYPE_NAMESPACE_BROWSER:
	case SOCMNOBJECT_TYPE_AREA_BROWSER:
	case SOCMNOBJECT_TYPE_EVENT_BROWSER:
	case SOCMNOBJECT_TYPE_AREA:
	case SOCMNOBJECT_TYPE_SOURCE:
	case SOCMNOBJECT_TYPE_CATEGORY:
	case SOCMNOBJECT_TYPE_CONDITION_CLASS:
	case SOCMNOBJECT_TYPE_NODE:
	case SOCMNOBJECT_TYPE_TAG:
		return TRUE;
	}
}

SOCmnString SOCltBrowseObject::getItemID(IN BOOL /* retrive */)
{
	return _T("");
}

#endif // SOCLT

