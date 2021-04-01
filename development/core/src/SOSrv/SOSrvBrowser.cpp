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
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvBrowser.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server object related classes                           |
//                - SOSrvBrowser: browser object                              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#include "opcerror.h"
#include "opcae_er.h"
#include "SOSrvBrowser.h"
#include "SOSrvEntry.h"

static TCHAR s_emptyString[2] = _T("");

//-----------------------------------------------------------------------------
// SOSrvBrowser                                                               |
//-----------------------------------------------------------------------------

SOSrvBrowser::SOSrvBrowser(void)
{
	m_curBrowse = NULL;
	m_curBrowsePath = _T("");
}


SOSrvBrowser::~SOSrvBrowser(void)
{
}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOSrvBrowser::getIntTraceId(void)
{
	SOCmnObject* obj;

	if ((obj = getObjectSrvBrowser()) != NULL)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return s_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectSrvBrowser
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOSrvBrowser::getObjectSrvBrowser(void)
{
	return NULL;
} // getObjectSrvBrowser


//-----------------------------------------------------------------------------
// queryOrganization
//
// - checks if the namespace tree is flat or hierarchial
//
// returns:
//		S_OK
//
enum SOSrvBrowser::browserOrganisation SOSrvBrowser::queryOrganization(void)
{
	SOCmnPointer<SOCmnElement> root = getBrowserRoot();
	SOCmnList<SOCmnElement> list(root->getBranchList());

	if (list.isEmpty())
	{
		// branch list of namespace root empty
		// -> flat namespace
		return orgFlat;
	}
	else
	{
		// branch list of namespace root not empty
		// -> hierarchial namespace
		return orgHierarchical;
	}
} // queryOrganization


//-----------------------------------------------------------------------------
// changeBrowsePosition
//
// - changes the current position in the object based space
// - changes the current position in the string based space
//
// returns:
//		S_OK                        - position changed
//		E_INVALIDARG                - called up to root node
//		OPC_E_INVALIDBRANCHNAME     - bad branch name on down or to
//
HRESULT SOSrvBrowser::changeBrowsePosition(
	IN IN enum browserDirection direction,  // direction to go in the tree
	IN LPCTSTR name)                        // name of down branch
{
	SOCmnPointer<SOCmnElement> root;
	HRESULT res = S_OK;

	if (!browseOnlyStringBased())
	{
		//-- object based browse space --
		if (((SOCmnElement*)m_curBrowse) || (direction == dirTo))
		{
			// current browse position is in object tree
			SOCmnElement* newPos = NULL;
			BOOL doStringBased = TRUE;

			switch (direction)
			{
			case dirUp:
				newPos = m_curBrowse->getParentBranch();
				doStringBased = FALSE;

				if (!newPos)
				{
					res = E_FAIL;    // called up to root node
				}

				break;

			case dirDown:
				newPos = m_curBrowse->findBranch(name);
				break;

			case dirTo:
			{
				root = getBrowserRoot();

				if (_tcslen(name) == 0)
				{
					newPos = root;
					newPos->addRef();
				}
				else
				{
					newPos = root->findBranch(name, TRUE);
				}
			}
			break;
			}

			if ((newPos) || (!doStringBased))
			{
				if (newPos)
				{
					m_curBrowse = newPos;
					m_curBrowsePath = m_curBrowse->getFullName();
				}

				return res;
			}

			// can't change to new position and direction is not up
			// -> may change to a dynamic node
		}
	}

	//-- string based browse space --
	return changeBrowsePositionStringBased(direction, name);
} // changeBrowsePosition


HRESULT SOSrvBrowser::changeBrowsePositionStringBased(
	IN IN enum browserDirection direction,  // direction to go in the tree
	IN LPCTSTR name)                        // name of down branch
{
	HRESULT res = S_OK;
	SOCmnString path((LPCTSTR)m_curBrowsePath);
	SOCmnPointer<SOCmnElement> root;

	switch (direction)
	{
	case dirUp:
	{
		if (!path.isEmpty())
		{
			LPTSTR upperLevel = NULL;
			LPTSTR name2 = NULL;
			root = getBrowserRoot();
			root->reducePath(path, upperLevel, name2, FALSE);

			if (upperLevel)
			{
				m_curBrowsePath = upperLevel;
				// check if upper level is object based
				m_curBrowse = root->findBranch(upperLevel);
			}
			else
			{
				// reached the root object
				m_curBrowse = root;
				m_curBrowsePath = _T("");
			}
		}
		else
		{
			res = E_FAIL;    // called up to root node
		}
	}
	break;

	case dirDown:
	{
		root = getBrowserRoot();
		TCHAR pathSeparator = root->getTreeDelimiter(NULL, NULL);

		if (!path.IsEmpty())
		{
			path += pathSeparator;
		}

		path += name;

		if (isBranchName(path))
		{
			// found string based child node
			m_curBrowsePath = path;
			m_curBrowse = NULL;
			res = S_OK;
		}
		else
		{
			res = OPC_E_INVALIDBRANCHNAME;
		}
	}
	break;

	case dirTo:
	{
		if (_tcslen(name) != 0)
		{
			if (isBranchName(name))
			{
				// found string based node
				m_curBrowsePath = name;
				m_curBrowse = NULL;
				res = S_OK;
			}
			else
			{
				res = OPC_E_INVALIDBRANCHNAME;
			}
		}
		else
		{
			// to the root object
			root = getBrowserRoot();
			m_curBrowse = root;
			m_curBrowsePath = _T("");
			res = S_OK;
		}
	}
	break;
	}

	return res;
}

//-----------------------------------------------------------------------------
// browseCurrentBranch
//
// - browses the current branch for branches, leafs or flat
//
// returns:
//		S_OK - filled list
//
HRESULT SOSrvBrowser::browseCurrentBranch(
	IN enum browserFilterType browseType,   // what to browse
	IN LPCTSTR filterCriteria,              // string filter
	IN VARTYPE dataTypeFilter,              // data type filter (DA only)
	IN DWORD accessRightsFilter,            // access rights filter (DA only)
	OUT SOCmnStringList& nameList,          // found elements
	IN OPTIONAL LPCTSTR path)               // path for OPC_FLAT browseType (DA only)
{
	HRESULT res = S_OK;
	BOOL addToList;
	SOCmnListPosition pos;

	if (!browseOnlyStringBased())
	{
		//-- object based --
		if ((SOCmnElement*)m_curBrowse)
		{
			// current browse position is in object space
			// get the object childs
			if ((browseType == typeBranch) && (ignoreFilterTypeOnFlatOrg()))
			{
				// ignore browse type for flat namespace (only DA)
				browserOrganisation nsOrg;
				nsOrg = queryOrganization();

				if (nsOrg == orgFlat)
				{
					browseType = typeLeaf;
				}
			}

			switch (browseType)
			{
				// return only the branches
			case typeBranch:
			{
				SOCmnList<SOCmnElement> list(m_curBrowse->getBranchList());
				SOCmnElement* branch;
				SOCmnString name;

				if (!(!list))
				{
					pos = list.getStartPosition();

					while (pos)
					{
						branch = list.getNext(pos);
						addToList = TRUE;
						name = branch->getName();

						// check browsable state of item tag
						if (!branch->isBrowsable(getObjectSrvBrowser()))
						{
							addToList = FALSE;
						}

						// string filter
						if ((addToList) && ((filterCriteria) && (_tcslen(filterCriteria) != 0)))
						{
							addToList = matchStringFilter(name, filterCriteria);
						}

						// additional filters
						if (addToList)
						{
							addToList = matchAdditionalFilters(branch, dataTypeFilter, accessRightsFilter);
						}

						// passed all filters?
						if (addToList)
						{
							nameList.add(name);
						}
					}
				}
			}
			break;

			// return only the leafs
			case typeLeaf:
			{
				SOCmnList<SOCmnElement> list(m_curBrowse->getLeafList());
				SOCmnElement* tag;
				SOCmnString name;

				if (!(!(list)))
				{
					pos = list.getStartPosition();

					while (pos)
					{
						tag = list.getNext(pos);

						if (!path)
						{
							name = tag->getName();
						}
						else
						{
							name = path;
							name += tag->getName();
						}

						addToList = TRUE;

						// check browsable state of item tag
						if (!tag->isBrowsable(getObjectSrvBrowser()))
						{
							addToList = FALSE;
						}

						// string filter
						if ((addToList) && ((filterCriteria) && (_tcslen(filterCriteria) != 0)))
						{
							addToList = matchStringFilter(name, filterCriteria);
						}

						// additional filters
						if (addToList)
						{
							addToList = matchAdditionalFilters(tag, dataTypeFilter, accessRightsFilter);
						}

						// passed all filters?
						if (addToList)
						{
							nameList.add(name);    // add to list
						}
					}
				}
			}
			break;

			// return all leafs including the leaf of the child branches
			case typeFlat:
			{
				SOCmnPointer<SOCmnElement> root = getBrowserRoot();
				TCHAR pathSeparator = root->getTreeDelimiter(NULL, NULL);
				SOCmnElement* orgBranch = NULL; /* avoid gcc warnig */
				SOCmnString orgPath;
				SOCmnString thisPath;

				if (path == NULL)
				{
					// recursion start
					orgBranch = m_curBrowse;
					orgBranch->addRef();
					orgPath = m_curBrowsePath;
					thisPath = m_curBrowsePath;

					if (!thisPath.isEmpty())
					{
						thisPath += pathSeparator;
					}
				}
				else
				{
					thisPath = path;
				}

				browseCurrentBranch(typeLeaf, filterCriteria, dataTypeFilter,
									accessRightsFilter, nameList, thisPath);
				SOCmnList<SOCmnElement> list(m_curBrowse->getBranchList());

				if (!(!list))
				{
					pos = list.getStartPosition();

					while (pos)
					{
						m_curBrowse = list.getNext(pos);
						m_curBrowse.addRef();
						m_curBrowsePath = m_curBrowse->getFullName();
						// recursion start
						browseCurrentBranch(typeFlat, filterCriteria, dataTypeFilter,
											accessRightsFilter, nameList, thisPath + m_curBrowse->getName() + pathSeparator);
					}
				}

				if (path == NULL)
				{
					// recursion start
					m_curBrowse = orgBranch;
					m_curBrowsePath = orgPath;
				}
			}
			break;

			default:
				break;
			}
		}
	}

	//-- string based --
	if (path == NULL)
	{
		// dont call string based on recursion
		if ((!browseOnlyStringBased()) && (!nameList.isEmpty()))
		{
			SOCmnStringList secNameList;
			secNameList.create();
			// get the child names from the application
			res = getChildNames(browseType, filterCriteria, dataTypeFilter, accessRightsFilter, secNameList);

			if ((SUCCEEDED(res)) && (!secNameList.isEmpty()))
			{
				// create search list
				SOCmnStringList objNameList;
				SOCmnListConfig cfg;
				SOCmnString listEntry;
				cfg.m_type = ((SOCmnListHeader::isRehashingEnabled() ? SOCMNLIST_TYPE_ORG_MAP : SOCMNLIST_TYPE_ORG_LIST) | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING);
				cfg.m_hashTableSize = 100;
				objNameList.create(&cfg);
				pos = nameList.getStartPosition();

				while (pos)
				{
					listEntry = nameList.getNext(pos);
					objNameList.add(listEntry, (LPCTSTR)listEntry);
				}

				// check for duplicates
				pos = secNameList.getStartPosition();

				while (pos)
				{
					listEntry = secNameList.getNext(pos);

					if (!objNameList.findKey(listEntry, NULL))
					{
						nameList.add(listEntry);
					}
				}
			}
		}
		else
		{
			// get the child names from the application
			res = getChildNames(browseType, filterCriteria, dataTypeFilter, accessRightsFilter, nameList);
		}
	}

	return res;
} // browseCurrentBranch


//-----------------------------------------------------------------------------
// getQualifiedElementName
//
// - get the full qualified element name from current browse position
//
// returns:
//		TRUE  - element found
//		FALSE - element not found
//
BOOL SOSrvBrowser::getQualifiedElementName(
	IN LPCTSTR name,        // element name
	OUT SOCmnString& path)  // full qualified element name
{
	BOOL found = FALSE;
	// calculate the full path
	path = m_curBrowsePath;

	if (!SOCmnString(name).isEmpty())
	{
		SOCmnPointer<SOCmnElement> root = getBrowserRoot();
		TCHAR pathSeparator = root->getTreeDelimiter(NULL, NULL);

		if (!path.IsEmpty())
		{
			path += pathSeparator;    // don't add separator on root
		}

		path += name;
	}
	else
	{
		return TRUE;    // empty element name -> current element
	}

	if (!browseOnlyStringBased())
	{
		// check if a element with the name exists
		if ((SOCmnElement*)m_curBrowse)
		{
			// serach object based space
			SOCmnPointer<SOCmnElement> el;
			el = m_curBrowse->findLeaf(name, TRUE);

			if ((SOCmnElement*)el == NULL)
			{
				el = m_curBrowse->findBranch(name, TRUE);
			}

			found = ((SOCmnElement*)el != NULL);
		}
	}

	if (!found)
	{
		found = getQualifiedElementNameStringBased(name, path);
	}

	return found;
} // getQualifiedElementName


BOOL SOSrvBrowser::getQualifiedElementNameStringBased(
	IN LPCTSTR /* name */,      // element name
	IN OUT SOCmnString& path)   // full qualified element name
{
	BOOL found = FALSE;
	// search string based space
	found = isLeafName(path);

	if (! found)
	{
		found = isBranchName(path);
	}

	return found;
}

//-----------------------------------------------------------------------------
// getQualifiedBranchName
//
// - get the full qualified branch name from current browse position
//
// returns:
//		TRUE  - element found
//		FALSE - element not found
//
BOOL SOSrvBrowser::getQualifiedBranchName(
	IN LPCTSTR name,        // branch name
	OUT SOCmnString& path)  // full qualified branch name
{
	BOOL found = FALSE;
	// calculate the full path
	path = m_curBrowsePath;

	if (!SOCmnString(name).IsEmpty())
	{
		SOCmnPointer<SOCmnElement> root = getBrowserRoot();
		TCHAR pathSeparator = root->getTreeDelimiter(NULL, NULL);

		if (!path.IsEmpty())
		{
			path += pathSeparator;    // don't add separator on root
		}

		path += name;
	}

	if (!browseOnlyStringBased())
	{
		// check if a branch with the name exists
		if ((SOCmnElement*)m_curBrowse)
		{
			// serach object based space
			SOCmnPointer<SOCmnElement> el;
			el = m_curBrowse->findBranch(name, TRUE);
			found = ((SOCmnElement*)el != NULL);
		}
	}

	if (!found)
	{
		// search string based space
		found = isBranchName(path);
	}

	return found;
} // getQualifiedBranchName


//-----------------------------------------------------------------------------
// getQualifiedLeafName
//
// - get the full qualified leaf name from current browse position
//
// returns:
//		TRUE  - element found
//		FALSE - element not found
//
BOOL SOSrvBrowser::getQualifiedLeafName(
	IN LPCTSTR name,        // leaf name
	OUT SOCmnString& path)  // full qualified branch name
{
	BOOL found = FALSE;
	// calculate the full path
	path = m_curBrowsePath;

	if (!SOCmnString(name).IsEmpty())
	{
		SOCmnPointer<SOCmnElement> root = getBrowserRoot();
		TCHAR pathSeparator = root->getTreeDelimiter(NULL, NULL);

		if (!path.IsEmpty())
		{
			path += pathSeparator;    // don't add separator on root
		}

		path += name;
	}

	if (!browseOnlyStringBased())
	{
		// check if a leaf with the name exists
		if ((SOCmnElement*)m_curBrowse)
		{
			// serach object based space
			SOCmnPointer<SOCmnElement> el;
			el = m_curBrowse->findLeaf(name, TRUE);
			found = ((SOCmnElement*)el != NULL);
		}
	}

	if (!found)
	{
		// search string based space
		found = isLeafName(path);
	}

	return found;
} // getQualifiedLeafName


//-----------------------------------------------------------------------------
// matchStringFilter
//
// - check if the string filter matches the string
// - the default implenetation uses the OPC Foundation algorithm
//
// returns:
//		TRUE  - filter matches
//		FALSE - filter don't match
//
BOOL SOSrvBrowser::matchStringFilter(
	IN LPCTSTR string,  // string
	IN LPCTSTR filter)  // filter
{
	return SOSrvServer::opcMatchStringFilter(string, filter, TRUE);
} // matchStringFilter


//-----------------------------------------------------------------------------
// matchAdditionalFilters
//
// - check if the additional filters
// - the default implenetation supports no additional filters
// - SODaS implement the additional filter check
//
// returns:
//		TRUE  - filter matches
//		FALSE - filter don't match
//
BOOL SOSrvBrowser::matchAdditionalFilters(
	IN SOCmnElement* /* obj */,         // space object
	IN VARTYPE /* dataTypeFilter */,    // datatype filter
	IN DWORD /* accessRightsFilter */)  // access rights filter
{
	return TRUE;
} // matchAdditionalFilters


//-----------------------------------------------------------------------------
// ignoreFilterTypeOnFlatOrg
//
// - ignore the filter type on flat spaces
//
// returns:
//		TRUE  - DA
//		FALSE - AE
//
BOOL SOSrvBrowser::ignoreFilterTypeOnFlatOrg(void)
{
	return FALSE;
} // ignoreFilterTypeOnFlatOrg


//-----------------------------------------------------------------------------
// getChildNames
//
// - called to receive the string based child names
// - default implementation: do nothing
//
// returns:
//		HRESULT
//
HRESULT SOSrvBrowser::getChildNames(
	IN enum browserFilterType /* browseType */,  // browse type
	IN LPCTSTR /* filterCriteria */,             // string filter
	IN VARTYPE /* dataTypeFilter */,             // data type filter
	IN DWORD /* accessRightsFilter */,           // access filter
	OUT SOCmnStringList& /* nameList */)               // list of dynamic child names
{
	return S_OK;
} // getChildNames


//-----------------------------------------------------------------------------
// isLeafName
//
// - check if the string represents a leaf in the string based space
//
// returns:
//		TRUE  - it's a leaf
//		FALSE - it's no leaf
//
BOOL SOSrvBrowser::isLeafName(
	IN LPCTSTR /* fullName */)  // fully qualified leaf name
{
	return FALSE;
} // isLeafName


//-----------------------------------------------------------------------------
// isBranchName
//
// - check if the string represents a branch in the string based space
//
// returns:
//		TRUE  - it's a branch
//		FALSE - it's no branch
//
BOOL SOSrvBrowser::isBranchName(
	IN LPCTSTR /* fullName */)  // fully qualified branch name
{
	return FALSE;
} // isBranchName


//-----------------------------------------------------------------------------
// browseOnlyStringBased
//
// - use only the string based address space for browsing
//
// returns:
//		TRUE  - only string based
//		FALSE - object based and string based
//
BOOL SOSrvBrowser::browseOnlyStringBased(void)
{
	return FALSE;
} // browseOnlyStringBased

#endif // SOSRV
