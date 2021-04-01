#include "stdafx.h"

#ifdef SOAES

#include <comcat.h>


//-----------------------------------------------------------------------------
// SOAeSArea                                                                  |
//-----------------------------------------------------------------------------

SOAeSArea::SOAeSArea(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_AREA)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_AREA);
#endif
}

SOAeSArea::SOAeSArea(
	IN LPCTSTR areaName) // area name
	:  SOCmnElement(SOCMNOBJECT_TYPE_AREA)
{
	setName(areaName);
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_AREA);
#endif
}

SOAeSArea::~SOAeSArea(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_AREA);
#endif
}

//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSArea::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SOAeSArea::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a source
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSArea::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a area
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSArea::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_AREA))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// trustUniqueChilds
//
// - trust that provided child names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SOAeSArea::trustUniqueChilds(void)
{
	return TRUE;
} // trustUniqueChilds


//-----------------------------------------------------------------------------
// getDeviceState
//
// - get state of the device represented by the object
//
// returns:
//		the device state of the object
//
SOCmnString SOAeSArea::getDeviceState(void)
{
	return SOCmnString(SOWATCH_DEVSTATE_OK);
} // getDeviceState


//-----------------------------------------------------------------------------
// getDeviceStateInfo
//
// - get state info of the device represented by the object
//
// returns:
//		the device state info  of the object
//
SOCmnString SOAeSArea::getDeviceStateInfo(void)
{
	return SOCmnString(_T(""));
} // getDeviceStateInfo


#ifdef SOFEATURE_WATCH
//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSArea::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_AREASPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_AREA);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif


//-----------------------------------------------------------------------------
// SOAeSAreaDefault                                                           |
//-----------------------------------------------------------------------------

SOAeSAreaDefault::SOAeSAreaDefault()
	:   SOCmnElementListLeafBranch(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
								   SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING)
{
	m_branchList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	m_leafList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
}

SOAeSAreaDefault::SOAeSAreaDefault(
	IN LPCTSTR areaName) // area name
	:   SOCmnElementListLeafBranch(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
								   SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING)
{
	m_branchList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	m_leafList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	setName(areaName);
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSAreaDefault::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SOAeSAreaSpaceRoot                                                         |
//-----------------------------------------------------------------------------

SOAeSAreaSpaceRoot::SOAeSAreaSpaceRoot(void)
	:  SOCmnElement(SOCMNOBJECT_TYPE_ROOT)
{
	// tree delimiters
	m_delimAScreate = _T('.');
	m_delimASaccept = _T(".,/\\!|");
	m_badASchars = _T("\"`´'#");
	m_trustUniqueChilds = TRUE;
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SOAeSAreaSpaceRoot::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a source
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSAreaSpaceRoot::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a area
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOAeSAreaSpaceRoot::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_AREA))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SOAeSAreaSpaceRoot::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	if (accept)
	{
		*accept = m_delimASaccept;
	}

	if (bad)
	{
		*bad = m_badASchars;
	}

	return m_delimAScreate;
} // getTreeDelimiter


//-----------------------------------------------------------------------------
// getConditions
//
// - get all conditions of an area or source object
//
void SOAeSAreaSpaceRoot::getConditions(
	IN SOCmnElement* parent,                    // parent element
	OUT SOCmnList<SOAeSCondition> &condList)    // cond list
{
	SOCmnList<SOAeSArea> areaList;
	SOAeSArea* area;
	SOCmnListPosition posA;
	SOCmnList<SOAeSSource> sourceList;
	SOAeSSource* source;
	SOCmnListPosition posS;
	SOCmnList<SOAeSCondition> conditions;
	SOAeSCondition* cond;
	SOCmnListPosition posC;
	conditions = (SOCmnList<SOAeSCondition> *)parent->getAdditionalList();

	if (!!conditions)
	{
		posC = conditions.getStartPosition();

		while (posC)
		{
			cond = conditions.getNext(posC);
			condList.add(cond);
		}
	}

	sourceList = (SOCmnList<SOAeSSource> *)parent->getLeafList();

	if (!!sourceList)
	{
		posS = sourceList.getStartPosition();

		while (posS)
		{
			source = sourceList.getNext(posS);
			SOAeSAreaSpaceRoot::getConditions(source, condList);
		}
	}

	areaList = (SOCmnList<SOAeSArea> *)parent->getBranchList();

	if (!!areaList)
	{
		posA = areaList.getStartPosition();

		while (posA)
		{
			area = areaList.getNext(posA);
			SOAeSAreaSpaceRoot::getConditions(area, condList);
		}
	}
} // getConditions


//-----------------------------------------------------------------------------
// trustUniqueChilds
//
// - trust that provided child names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SOAeSAreaSpaceRoot::trustUniqueChilds(void)
{
	return m_trustUniqueChilds;
} // trustUniqueChilds


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SOAeSAreaSpaceRoot::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_AE_AREASPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_AE_AREASPACEROOT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
#ifdef SOFEATURE_DCOM
		SOCmnPointer<SOAeSAreaSpaceRoot> asRoot;
		BYTE i;

		for (i = 0; i < SOAES_SERVER_INSTANCE_COUNT; i++)
		{
			asRoot = entry->getAreaSpaceRoot(i);

			if ((SOAeSAreaSpaceRoot*)asRoot == this)
			{
				SOCmnString buffer;
				CLSID clsid;
				entry->getCLSID(clsid, i);
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLSID_T, conv->CLSID2string(buffer, clsid));
				break;
			}
		}

#endif
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATHDELIMITER_T, SOCmnString(getTreeDelimiter(NULL, NULL)));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#endif // SOAES
