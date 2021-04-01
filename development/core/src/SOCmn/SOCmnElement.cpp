#include "stdafx.h"

#ifdef SOFEATURE_CMN_ELEMENT

#include "SOCmn.h"
#include "SOCmnElement.h"
#include "SOCmnTrace.h"
#include "SOCmnFunctions.h"

#define SOCMNELEMENT_ERROR_DUPLICATENAME     ((HRESULT)0xC004000CL) // = OPC_E_DUPLICATENAME

static TCHAR g_delimCreate = _T('\x7f');
static TCHAR g_delimAccept[2] = _T("\x7f");
static TCHAR g_badChars[1] = _T("");

static TCHAR g_emptyString[2] = _T("");

TCHAR SOCmnElement::s_delimCreate = g_delimCreate;

//-----------------------------------------------------------------------------
// SOCmnElement                                                               |
//-----------------------------------------------------------------------------

SOCmnElement::SOCmnElement(
	IN DWORD objType)                   // object type
	: SOCmnObject(objType | SOCMNOBJECT_TYPE_ELEMENT)
{
	m_parent = NULL;
}

SOCmnElement::~SOCmnElement(void)
{
	setParentBranch(NULL);  // release parent object
}


//-----------------------------------------------------------------------------
// setName
//
// - set element name
// - check if name is valid
// - add name to parents child name list
//
// returns:
//		S_OK                - set name
//		E_INVALIDARG        - no valid name or parameter
//		OPC_E_DUPLICATENAME - no unique name
//
HRESULT SOCmnElement::setName(
	IN LPCTSTR name)    // element name
{
	HRESULT res;
	SOCmnString curName;

	if (!name)
	{
		return E_INVALIDARG;
	}

	getName(curName);

	if (curName == name) // no change in name
	{
		return S_OK;
	}

	// have a parent, check if name is ok with siblings:
	// check if name is composed of valid characters and if unique:
	if (FAILED(res = validateName(name)))
	{
		// cannot set this name, name is not valid !!
		return res;
	}
	else
	{
		// name valid
		if (setElementName(name))
		{
			SOCmnPointer<SOCmnElement> parent = getParentBranch();

			if (parent.isNotNull())
			{
				// change map entry in parent object
				res = parent->changeChildName(this, curName);

				if (FAILED(res))
				{
					setElementName(curName);
				}
			}
		}
		else
		{
			res = E_FAIL;
		}

		if (SUCCEEDED(res))
		{
			// set name
			setObjTraceId(NULL);
			res = S_OK;
		}

		return res;
	}
} // setName


//-----------------------------------------------------------------------------
// setName
//
// - set element name
// - check if name is valid
// - add name to parents child name list
//
// returns:
//		S_OK                - set name
//		E_INVALIDARG        - no valid name or parameter
//		OPC_E_DUPLICATENAME - no unique name
//
HRESULT SOCmnElement::setName(
	IN DWORD name)  // element name
{
	HRESULT res = S_OK;
	DWORD curName;

	if (!name)
	{
		return E_INVALIDARG;
	}

	getName(curName);

	if (curName == name) // no change in name
	{
		return S_OK;
	}

	// so check if new name will be unique
	SOCmnPointer<SOCmnElement> parent = getParentBranch();

	if (parent.isNotNull())
	{
		SOCmnString strName;
		strName.format(_T("%8.8lX"), name);

		if (!(parent->isUniqueChildName(strName, (SOCmnElement*)this)))
		{
			return SOCMNELEMENT_ERROR_DUPLICATENAME;
		}
	}

	// name valid
	if (setElementName(name))
	{
		if (parent.isNotNull())
		{
			// change map entry in parent object
			res = parent->changeChildName(this, curName);

			if (FAILED(res))
			{
				setElementName(curName);
			}
		}
	}
	else
	{
		res = E_FAIL;
	}

	if (SUCCEEDED(res))
	{
		// set name
		setObjTraceId(NULL);
		res = S_OK;
	}

	return res;
} // setName


//-----------------------------------------------------------------------------
// onSetParentBranch
//
void SOCmnElement::onSetParentBranch()
{} // onSetName


//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SOCmnElement::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	SOCmnPointer<SOCmnElement> parent = getParentBranch();

	if (parent.isNotNull())
	{
		while ((!!parent) && (!parent->is(SOCMNOBJECT_TYPE_ROOT)))
		{
			parent = parent->getParentBranch();
		}
	}

	if (parent.isNotNull())
	{
		return parent->getTreeDelimiter(accept, bad);
	}
	else
	{
		if (accept)
		{
			*accept = g_delimAccept;
		}

		if (bad)
		{
			*bad = g_badChars;
		}

		return s_delimCreate;
	}
} // getTreeDelimiter


//-----------------------------------------------------------------------------
// validateName
//
// - decrement reference counter
// - free object if no reference left
//
// returns:
//		S_OK                - valid name
//		E_INVALIDARG        - no valid name
//		OPC_E_DUPLICATENAME - no unique name
//
HRESULT SOCmnElement::validateName(
	IN LPCTSTR name,                // name
	IN OPTIONAL BOOL checkIfUnique) // check if unique in parent
{
	BOOL ok = TRUE;
	LPCTSTR accept = NULL;
	LPCTSTR bad = NULL;
	HRESULT res = S_OK;

	if (!name)
	{
		return E_INVALIDARG;
	}

	getTreeDelimiter(&accept, &bad);
	ok &= (_tcslen(name) != 0);
	ok &= (_tcspbrk(name, accept) == NULL);
	ok &= (_tcspbrk(name, bad) == NULL);

	if (!ok)
	{
		res = E_INVALIDARG;
	}

	// check for uniqueness, too?
	if ((ok) && (checkIfUnique))
	{
		// so check if new name will be unique
		SOCmnPointer<SOCmnElement> parent = getParentBranch();

		if (parent.isNotNull())
		{
			ok = parent->isUniqueChildName(name, (SOCmnElement*)this);

			if (!ok)
			{
				res = SOCMNELEMENT_ERROR_DUPLICATENAME;
			}
		}
	}

	return res;
} // validateName


//-----------------------------------------------------------------------------
// getFullName
//
// - composes the absolut name of the element
//
// returns:
//		path of the element
//
SOCmnString SOCmnElement::getFullName(void)
{
	SOCmnString name;
	SOCmnString path;
	SOCmnPointer<SOCmnElement> parent = getParentBranch();
	TCHAR pathSeparator;
	getKey(name);
	pathSeparator = getTreeDelimiter(NULL, NULL);

	if (!parent)
	{
		if (is(SOCMNOBJECT_TYPE_ROOT))
		{
			name.empty();
		}

		return name;    // no parent, no path before name
	}

	while ((!!parent) && (!parent->is(SOCMNOBJECT_TYPE_ROOT)))
	{
		path = name;
		parent->getKey(name);
		name += pathSeparator;
		name += path;
		parent = parent->getParentBranch();
	}

	return name;
} // getFullName


//-----------------------------------------------------------------------------
// getPath
//
// - composes the path of the element
//
// returns:
//		path of the element
//
SOCmnString SOCmnElement::getPath(void)
{
	SOCmnString path;
	SOCmnPointer<SOCmnElement> parent = getParentBranch();
	TCHAR pathSeparator;
	pathSeparator = getTreeDelimiter(NULL, NULL);

	if (!parent)
	{
		return path;    // no parent, empty path
	}

	SOCmnString name;

	while ((!!parent) && (!parent->is(SOCMNOBJECT_TYPE_ROOT)))
	{
		path = name;
		parent->getKey(name);

		if (!path.isEmpty())
		{
			name += pathSeparator;
		}

		name += path;
		parent = parent->getParentBranch();
	}

	return name;
} // getPath


//-----------------------------------------------------------------------------
// reducePath
//
// - reduce path into two parts by searching path seperator from left or right
//
void SOCmnElement::reducePath(
	IN LPCTSTR path,            // path (returned pointers point into this buffer)
	OUT LPTSTR& first,          // pointer to first path element(s) MULL if top hierarchy object
	OUT LPTSTR& second,     // pointer to second path element(s) NULL if path at end
	IN OPTIONAL BOOL fromLeft)  // where to start ?
{
	LPCTSTR accept = NULL;
	getTreeDelimiter(&accept, NULL);
	reducePath(path, first, second, fromLeft, accept);
} // reducePath


//-----------------------------------------------------------------------------
// reducePath
//
// - reduce path into two parts by searching path seperator from left or right
//
void SOCmnElement::reducePath(
	IN LPCTSTR path,            // path (returned pointers point into this buffer)
	OUT LPTSTR& first,   // pointer to first path element(s) MULL if top hierarchy object
	OUT LPTSTR& second,  // pointer to second path element(s) NULL if path at end
	IN BOOL fromLeft,       // where to start ?
	IN LPCTSTR accept)    // accepted delimiters
{
	LPTSTR delim;
	LPTSTR next;

	if (fromLeft)
	{
		// find first path element from left
		first = (LPTSTR)path;
		second = NULL;
		delim = (LPTSTR)_tcspbrk(path, accept);
	}
	else
	{
		// find first path element from right -> last element
		first = NULL;
		second = (LPTSTR)path;
		delim = NULL;
		next = (LPTSTR)path;

		while ((next = _tcspbrk(next, accept)) != NULL)
		{
			delim = next;
			next++;
		}

		if (delim)
		{
			first = (LPTSTR)path;
		}
	}

	if (delim)
	{
		*delim = _T('\0');   //split path here
		delim++;
		second = delim;
	}
} // reducePath


//-----------------------------------------------------------------------------
// getParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
SOCmnElement* SOCmnElement::getParentBranch(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);

	if (m_parent)
	{
		m_parent->addRef();
	}

	return m_parent;
} // getParentBranch


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
BOOL SOCmnElement::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	pObjectLock->lock();

	if (m_parent == parent)
	{
		pObjectLock->unlock();
		SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
		return TRUE;    // no change to before, -> no check necessary
	}

	if (parent != NULL)
	{
		// check if the elements name is unique in parent branch
		pObjectLock->unlock();

		if (parent->isUniqueChildName(getName(), this))
		{
			pObjectLock->lock();
			parent->addRef();

			if (m_parent)
			{
				m_parent->release();
			}

			m_parent = parent;
			pObjectLock->unlock();
			SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
			onSetParentBranch();
			return TRUE;
		}
		else
		{
			pObjectLock->lock();

			if (m_parent)
			{
				m_parent->release();
			}

			m_parent = NULL;
			pObjectLock->unlock();
			SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
			onSetParentBranch();
			return FALSE;
		}
	}
	else
	{
		if (m_parent)
		{
			m_parent->release();
		}

		m_parent = NULL;
		pObjectLock->unlock();
		onSetParentBranch();
		SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
		return TRUE;
	}
} // setParentBranch


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SOCmnElement::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), (LPCTSTR)getName());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key (name) with given string
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOCmnElement::hasKey(
	IN LPCTSTR key) // object key
{
	SOCmnString name;
	getName(name);
	return (name == key);
} // hasKey


//-----------------------------------------------------------------------------
// hasKey
//
// - compare objects unique key (name) with given DWORD
//
// returns:
//		TRUE  - object has the key
//      FALSE - object has not the key
//
BOOL SOCmnElement::hasKey(
	IN DWORD key)   // object key
{
	return (getNameInt() == key);
} // hasKey


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key (name)
//
// returns:
//		object key
//
BOOL SOCmnElement::getKey(
	OUT SOCmnString& key)
{
	getName(key);
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// getKey
//
// - get objects unique key (name)
//
// returns:
//		object key
//
BOOL SOCmnElement::getKey(
	OUT DWORD& key)
{
	getName(key);
	return TRUE;
} // getKey


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOCmnElement::addLeaf(
	IN SOCmnElement* newLeaf)       // leaf
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (!newLeaf->is(SOCMNOBJECT_TYPE_LEAF))
	{
		return FALSE;
	}

	getLeafList()->doRehash();
	SOCmnList<SOCmnElement> leafList(getLeafList());

	if (!leafList)
	{
		return FALSE;
	}

	if (!isUniqueChildName(newLeaf->getName(), newLeaf))
	{
		return FALSE;
	}

	// add leaf
	if (leafList.add(newLeaf))
	{
		leafList.autoUnlock();
	}

	return newLeaf->setParentBranch(this);
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOCmnElement::addBranch(
	IN SOCmnElement* newBranch) // branch
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (!newBranch->is(SOCMNOBJECT_TYPE_BRANCH))
	{
		return FALSE;
	}

	getBranchList()->doRehash();
	SOCmnList<SOCmnElement> branchList(getBranchList());

	if (!branchList)
	{
		return FALSE;
	}

	if (!isUniqueChildName(newBranch->getName(), newBranch))
	{
		return FALSE;
	}

	// add branch
	if (branchList.add(newBranch))
	{
		branchList.autoUnlock();
	}

	return newBranch->setParentBranch(this);
} // addBranch


//-----------------------------------------------------------------------------
// addAdditionalElement
//
// - adds Additional element to list
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOCmnElement::addAdditionalElement(
	IN SOCmnElement* newEl)     // leaf
{
	if (!newEl)
	{
		return FALSE;
	}

	if (!newEl->is(SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT))
	{
		return FALSE;
	}

	getAdditionalList()->doRehash();
	SOCmnList<SOCmnElement> addList(getAdditionalList());

	if (!addList)
	{
		return FALSE;
	}

	if (!isUniqueChildName(newEl->getName(), newEl))
	{
		return FALSE;
	}

	// add leaf
	if (addList.add(newEl))
	{
		addList.autoUnlock();
	}

	return newEl->setParentBranch(this);
} // addAdditionalElement


//-----------------------------------------------------------------------------
// removeLeaf
//
// - removes leaf from leaf list
//
// returns:
//		TRUE  - removed
//		FALSE - not in list
//
BOOL SOCmnElement::removeLeaf(
	IN SOCmnElement* leaf)      // leaf
{
	if (!leaf)
	{
		return FALSE;
	}

	if (!leaf->is(SOCMNOBJECT_TYPE_LEAF))
	{
		return FALSE;
	}

	SOCmnList<SOCmnElement> leafList(getLeafList());

	if (!leafList)
	{
		return FALSE;
	}

	BOOL ret;
	leaf->addRef();
	ret = leafList.removeObject(leaf);

	if (ret)
	{
		leaf->setParentBranch(NULL);
	}

	getLeafList()->doRehash();

	leaf->release();
	return ret;
} // removeLeaf


//-----------------------------------------------------------------------------
// removeBranch
//
// - removes branch from branch list
//
// returns:
//		TRUE  - removed
//		FALSE - not in list
//
BOOL SOCmnElement::removeBranch(
	IN SOCmnElement* branch)    // branch
{
	if (!branch)
	{
		return FALSE;
	}

	if (!branch->is(SOCMNOBJECT_TYPE_BRANCH))
	{
		return FALSE;
	}

	SOCmnList<SOCmnElement> branchList(getBranchList());

	if (!branchList)
	{
		return FALSE;
	}

	BOOL ret;
	branch->addRef();
	ret = branchList.removeObject(branch);
	branchList.autoUnlock();

	if (ret)
	{
		branch->setParentBranch(NULL);
	}

	getBranchList()->doRehash();

	branch->release();
	return ret;
} // removeBranch


//-----------------------------------------------------------------------------
// removeAdditionalElement
//
// - removes branch from branch list
//
// returns:
//		TRUE  - removed
//		FALSE - not in list
//
BOOL SOCmnElement::removeAdditionalElement(
	IN SOCmnElement* addEl) // additional element
{
	if (!addEl)
	{
		return FALSE;
	}

	if (!addEl->is(SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT))
	{
		return FALSE;
	}

	SOCmnList<SOCmnElement> addList(getAdditionalList());

	if (!addList)
	{
		return FALSE;
	}

	BOOL ret;
	addEl->addRef();
	ret = addList.removeObject(addEl);

	if (ret)
	{
		addEl->setParentBranch(NULL);
	}

	getAdditionalList()->doRehash();

	addEl->release();
	return ret;
} // removeAdditionalElement


//-----------------------------------------------------------------------------
// removeAllLeaves
//
// - remove all leafs from the leaf list
//
void SOCmnElement::removeAllLeaves(
	IN OPTIONAL BOOL deep)  // remove all sub leafs too
{
	SOCMN_TRY
	{
		SOCmnListPosition pos;
		SOCmnList<SOCmnElement> leafList(getLeafList());

		if (!!leafList)
		{
			SOCmnElement* leaf;
			pos = leafList.getStartPosition();

			while (pos)
			{
				leaf = leafList.getNext(pos);
				leaf->setParentBranch(NULL);
			}

			leafList.removeAll();
			leafList.autoUnlock();
		}

		if (deep)
		{
			SOCmnList<SOCmnElement> branchList(getBranchList());

			if (!!branchList)
			{
				SOCmnElement* branch;
				pos = branchList.getStartPosition();

				while (pos)
				{
					branch = branchList.getNext(pos);
					branch->removeAllLeafs(TRUE);
				}

				branchList.autoUnlock();
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OBJ_REF, (_T("EXCEPTION in removeAllLeaves")));
	}
} // removeAllLeaves


//-----------------------------------------------------------------------------
// removeAllLeafs
//
// - remove all leafs from the leaf list
//
void SOCmnElement::removeAllLeafs(
	IN OPTIONAL BOOL deep)  // remove all sub leafs too
{
	removeAllLeaves(deep);
} // removeAllLeafs


//-----------------------------------------------------------------------------
// removeAllBranches
//
// - remove all branches from the branches list
//
void SOCmnElement::removeAllBranches(
	IN OPTIONAL BOOL deep)  // remove all sub branches too
{
	SOCMN_TRY
	{
		SOCmnPointer<SOCmnElement> preventSelfdestruction;
		preventSelfdestruction.setWithAddRef(this);
		SOCmnList<SOCmnElement> branchList(getBranchList());

		if (!!branchList)
		{
			SOCmnElement* branch;
			SOCmnListPosition pos;
			pos = branchList.getStartPosition();

			while (pos)
			{
				branch = branchList.getNext(pos);
				branch->addRef();

				if (deep)
				{
					branch->removeAllBranches(TRUE);
				}

				branch->setParentBranch(NULL);
				branch->release();
			}

			branchList.removeAll();
			branchList.autoUnlock();
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OBJ_REF, (_T("EXCEPTION in removeAllBranches")));
	}
} // removeAllBranches


//-----------------------------------------------------------------------------
//  removeAllAdditionalElements
//
// - remove all elements from the additional elements list
//
void SOCmnElement::removeAllAdditionalElements(
	IN OPTIONAL BOOL deep)  // remove all sub branches too
{
	SOCMN_TRY
	{
		SOCmnListPosition pos;
		SOCmnList<SOCmnElement> addList(getAdditionalList());
		SOCmnElement* addEl;

		if (deep)
		{
			if (!!addList)
			{
				pos = addList.getStartPosition();

				while (pos)
				{
					addEl = addList.getNext(pos);
					addEl->removeAllAdditionalElements(TRUE);
				}

				addList.autoUnlock();
			}

			SOCmnList<SOCmnElement> leafList(getLeafList());

			if (!!leafList)
			{
				SOCmnElement* leaf;
				pos = leafList.getStartPosition();

				while (pos)
				{
					leaf = leafList.getNext(pos);
					leaf->removeAllAdditionalElements(TRUE);
				}

				leafList.autoUnlock();
			}

			SOCmnList<SOCmnElement> branchList(getBranchList());

			if (!!branchList)
			{
				SOCmnElement* branch;
				pos = branchList.getStartPosition();

				while (pos)
				{
					branch = branchList.getNext(pos);
					branch->removeAllAdditionalElements(TRUE);
				}

				branchList.autoUnlock();
			}
		}

		if (!!addList)
		{
			pos = addList.getStartPosition();

			while (pos)
			{
				addEl = addList.getNext(pos);
				addEl->setParentBranch(NULL);
			}

			addList.removeAll();
			addList.autoUnlock();
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OBJ_REF, (_T("EXCEPTION in removeAllAdditionalElements")));
	}
} // removeAllAdditionalElements


//-----------------------------------------------------------------------------
// findLeaf
//
// - find leaf below the branch
//
// returns:
//		pointer to the leaf - leaf found
//		NULL - no leaf found
//
SOCmnElement* SOCmnElement::findLeaf(
	IN LPCTSTR name,        // name of leaf
	IN OPTIONAL BOOL deep)  // deep search or only direct child
{
	SOCmnElement* leaf = NULL;
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

	if ((!deep) || (!remainingPath))    // no deep search or no remaining path
	{
		// -> lookup name in leaf list
		SOCmnList<SOCmnElement> leafList(getLeafList());

		if (!!leafList)
		{
			leaf = leafList.findKey(name);
		}
	}
	else                            // deep search and remaining path
	{
		// -> search for child branch and find inside of this branch
		SOCmnList<SOCmnElement> branchList(getBranchList());

		if (!!branchList)
		{
			SOCmnPointer<SOCmnElement> branch;
			branch = branchList.findKey(child);
			branchList.autoUnlock();

			if (!(!branch))
			{
				leaf = branch->findLeaf(remainingPath, TRUE);
			}
		}
	}

	return leaf;
} // findLeaf


//-----------------------------------------------------------------------------
// findBranch
//
// - find branch below the branch
//
// returns:
//		pointer to the branch - branch found
//		NULL - no branch found
//
SOCmnElement* SOCmnElement::findBranch(
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
		search = branchList.findKey(child);

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
// findAdditionalElement
//
// - find additional element
//
// returns:
//		pointer to the additional element - additional element found
//		NULL - no additional element found
//
SOCmnElement* SOCmnElement::findAdditionalElement(
	IN LPCTSTR name)    // name of additional element
{
	SOCmnList<SOCmnElement> addElList(getAdditionalList());

	if (addElList.isInit())
	{
		return addElList.findKey(name);
	}
	else
	{
		return NULL;
	}
} // findAdditionalElement


//-----------------------------------------------------------------------------
// isUniqueChildName
//
// - check if the name is unique among all children
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SOCmnElement::isUniqueChildName(
	IN LPCTSTR name,                    // name
	IN OPTIONAL SOCmnElement* child)    // child to ignore for check
{
	if (!name)
	{
		return FALSE;
	}

	if (FAILED(validateName(name, FALSE)))
	{
		return FALSE;    // no valid name
	}

	if (child)
	{
		if (child->isUniqueName())
		{
			return TRUE;
		}
	}

	if (trustUniqueChilds() == TRUE)
	{
		return TRUE;
	}

	if ((!child) || (child->is(SOCMNOBJECT_TYPE_LEAF)) || (child->is(SOCMNOBJECT_TYPE_BRANCH)))
	{
		SOCmnPointer<SOCmnElement> leaf;
		SOCmnPointer<SOCmnElement> branch;

		// check for leaf with this name
		if ((SOCmnElement*)(leaf = findLeaf(name, FALSE)))
		{
			if ((SOCmnElement*)leaf == (SOCmnElement*)child)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		// check for branch with this name
		if ((SOCmnElement*)(branch = findBranch(name, FALSE)))
		{
			if ((SOCmnElement*)branch == (SOCmnElement*)child)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else
	{
		if (child->is(SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT))
		{
			SOCmnList<SOCmnElement> addList(getAdditionalList());

			if (!!addList)
			{
				SOCmnPointer<SOCmnElement> addEl;

				if ((SOCmnElement*)(addEl = addList.findKey(name)))
				{
					if ((SOCmnElement*)addEl == (SOCmnElement*)child)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
} // isUniqueChildName


//-----------------------------------------------------------------------------
// changeChildName
//
// - change the name of a child object in the map
//
// returns:
//		S_OK         - map entry changed
//		E_FAIL       - can't remove object with previous name
//		E_INVALIDARG - invalid argument
//
HRESULT SOCmnElement::changeChildName(
	IN SOCmnElement* child, // child element
	IN LPCTSTR name)            // previous child name
{
	if ((!child) || (!name))
	{
		return E_INVALIDARG;
	}

	if (child->is(SOCMNOBJECT_TYPE_LEAF))
	{
		SOCmnList<SOCmnElement> leafList(getLeafList());
		SOCmnElement* leaf = (SOCmnElement*)child;

		if (!leafList.isMap())
		{
			return S_OK;
		}

		if (leafList.findKey(name, &leaf))
		{
			leafList.removeKey(name);
			leafList.add(leaf);
		}
	}
	else
	{
		if (child->is(SOCMNOBJECT_TYPE_BRANCH))
		{
			SOCmnList<SOCmnElement> branchList(getBranchList());
			SOCmnElement* branch = (SOCmnElement*)child;

			if (!branchList.isMap())
			{
				return S_OK;
			}

			if (branchList.findKey(name, &branch))
			{
				branchList.removeKey(name);
				branchList.add(branch);
			}
		}
		else
		{
			if (child->is(SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT))
			{
				SOCmnList<SOCmnElement> addList(getAdditionalList());
				SOCmnElement* addEl = (SOCmnElement*)child;

				if (!addList.isMap())
				{
					return S_OK;
				}

				if (addList.findKey(name, &addEl))
				{
					addList.removeKey(name);
					addList.add(addEl);
				}
			}
		}
	}

	return S_OK;
} // changeChildName


//-----------------------------------------------------------------------------
// changeChildName
//
// - change the name of a child object in the map
//
// returns:
//		S_OK         - map entry changed
//		E_FAIL       - can't remove object with previous name
//		E_INVALIDARG - invalid argument
//
HRESULT SOCmnElement::changeChildName(
	IN SOCmnElement* child, // child element
	IN DWORD name)          // previous child name
{
	if ((!child) || (!name))
	{
		return E_INVALIDARG;
	}

	if (child->is(SOCMNOBJECT_TYPE_LEAF))
	{
		SOCmnList<SOCmnElement> leafList(getLeafList());
		SOCmnElement* leaf = (SOCmnElement*)child;

		if (!leafList.isMap())
		{
			return S_OK;
		}

		if (leafList.findKey(name, &leaf))
		{
			leafList.removeKey(name);
			leafList.add(leaf);
		}
	}
	else
	{
		if (child->is(SOCMNOBJECT_TYPE_BRANCH))
		{
			SOCmnList<SOCmnElement> branchList(getBranchList());
			SOCmnElement* branch = (SOCmnElement*)child;

			if (!branchList.isMap())
			{
				return S_OK;
			}

			if (branchList.findKey(name, &branch))
			{
				branchList.removeKey(name);
				branchList.add(branch);
			}
		}
		else
		{
			if (child->is(SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT))
			{
				SOCmnList<SOCmnElement> addList(getAdditionalList());
				SOCmnElement* addEl = (SOCmnElement*)child;

				if (!addList.isMap())
				{
					return S_OK;
				}

				if (addList.findKey(name, &addEl))
				{
					addList.removeKey(name);
					addList.add(addEl);
				}
			}
		}
	}

	return S_OK;
} // changeChildName


//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOCmnElement::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// trustUniqueChilds
//
// - trust that provided child names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SOCmnElement::trustUniqueChilds(void)
{
	return FALSE;
} // trustUniqueChilds


//-----------------------------------------------------------------------------
// isBrowsable
//
// - is visible during browsing
//
// returns:
//		TRUE  - item tag is returned to the client as browse result
//    FALSE - item tag is not visible over the browse interfaces
//
BOOL SOCmnElement::isBrowsable(IN SOCmnObject* /* pServer */)
{
	return TRUE;
} // isBrowsable



//-----------------------------------------------------------------------------
// SOCmnElementIName                                                          |
//-----------------------------------------------------------------------------

SOCmnElementIName::SOCmnElementIName()
{}

SOCmnElementIName::~SOCmnElementIName()
{}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOCmnElementIName::getIntTraceId(void)
{
	SOCmnObject* obj;

	if ((obj = getObjectCmnElementIName()) != NULL)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return g_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOCmnElementIName::getObjectCmnElementIName(void)
{
	return NULL;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// getNameInt
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
DWORD SOCmnElementIName::getNameInt(void)
{
	return getObjectCmnElementIName()->getOpcHandle();
} // getNameInt


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
void SOCmnElementIName::getName(
	OUT DWORD& name)
{
	name = getObjectCmnElementIName()->getOpcHandle();
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
SOCmnString SOCmnElementIName::getName(void)
{
	SOCmnString name;
	name.format(_T("%8.8lX"), getObjectCmnElementIName()->getOpcHandle());
	return name;
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
void SOCmnElementIName::getName(
	OUT SOCmnString& name)
{
	name.format(_T("%8.8lX"), getObjectCmnElementIName()->getOpcHandle());
} // getName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		TRUE
//
BOOL SOCmnElementIName::isUniqueName(void)
const
{
	return TRUE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (string)
//
// returns:
//		FALSE
//
BOOL SOCmnElementIName::setElementName(
	IN LPCTSTR /* name */)
{
	return FALSE;
} // setElementName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (DWORD)
//
// returns:
//		FALSE
//
BOOL SOCmnElementIName::setElementName(
	IN DWORD /* name */)
{
	return FALSE;
} // setElementName


//-----------------------------------------------------------------------------
// onSetName
//
void SOCmnElementIName::onSetName()
{} // onSetName

//-----------------------------------------------------------------------------
// SOCmnElementNameString                                                     |
//-----------------------------------------------------------------------------

SOCmnElementNameString::SOCmnElementNameString()
{
}

//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
SOCmnString SOCmnElementNameString::getName(void)
{
	return m_name;
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
void SOCmnElementNameString::getName(
	OUT SOCmnString& name)
{
	name = m_name;
} // getName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE
//
BOOL SOCmnElementNameString::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (string)
//
// returns:
//		TRUE
//
BOOL SOCmnElementNameString::setElementName(
	IN LPCTSTR name)
{
	m_name = name;
	onSetName();
	return TRUE;
} // setElementName


//-----------------------------------------------------------------------------
// SOCmnElementNameInt                                                        |
//-----------------------------------------------------------------------------

SOCmnElementNameInt::SOCmnElementNameInt()
{
	m_name = 0;
}

//-----------------------------------------------------------------------------
// getNameInt
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
DWORD SOCmnElementNameInt::getNameInt(void)
{
	return m_name;
} // getNameInt


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
void SOCmnElementNameInt::getName(
	OUT DWORD& name)
{
	name = m_name;
} // getName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE
//
BOOL SOCmnElementNameInt::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (DWORD)
//
// returns:
//		TRUE
//
BOOL SOCmnElementNameInt::setElementName(
	IN DWORD name)
{
	m_name = name;
	onSetName();
	return TRUE;
} // setElementName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
SOCmnString SOCmnElementNameInt::getName(void)
{
	SOCmnString name;
	name.format(_T("%lu"), m_name);
	return name;
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
void SOCmnElementNameInt::getName(
	OUT SOCmnString& name)
{
	name.format(_T("%lu"), m_name);
} // getName


//-----------------------------------------------------------------------------
// SOCmnElementNameIntString                                                        |
//-----------------------------------------------------------------------------

SOCmnElementNameIntString::SOCmnElementNameIntString()
{
	m_name = 0;
}

//-----------------------------------------------------------------------------
// getNameInt
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
DWORD SOCmnElementNameIntString::getNameInt(void)
{
	return m_name;
} // getNameInt


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (DWORD)
//
void SOCmnElementNameIntString::getName(
	OUT DWORD& name)
{
	name = m_name;
} // getName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE
//
BOOL SOCmnElementNameIntString::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (DWORD)
//
// returns:
//		TRUE
//
BOOL SOCmnElementNameIntString::setElementName(
	IN DWORD name)
{
	m_name = name;

	if (m_szName.isEmpty())
	{
		m_szName.format(_T("%lu"), m_name);
	}

	onSetName();
	return TRUE;
} // setElementName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
SOCmnString SOCmnElementNameIntString::getName(void)
{
	return m_szName;
} // getName


//-----------------------------------------------------------------------------
// getName
//
// - get name of element
//
// returns:
//		name of element (string)
//
void SOCmnElementNameIntString::getName(
	OUT SOCmnString& name)
{
	name =  m_szName;
} // getName


//-----------------------------------------------------------------------------
// setElementName
//
// - set name of element (string)
//
// returns:
//		TRUE
//
BOOL SOCmnElementNameIntString::setElementName(
	IN LPCTSTR name)
{
	m_szName = name;
	onSetName();
	return TRUE;
} // setElementName


//-----------------------------------------------------------------------------
// SOCmnElementIList                                                          |
//-----------------------------------------------------------------------------

SOCmnElementIList::SOCmnElementIList()
{}

SOCmnElementIList::~SOCmnElementIList()
{}


//-----------------------------------------------------------------------------
// getIntTraceId
//
// - get trace id for the traces in the interface
//
// returns:
//		trace id
//
LPCTSTR SOCmnElementIList::getIntTraceId(void)
{
	SOCmnObject* obj;

	if ((obj = getObjectCmnElementIList()) != NULL)
	{
		return obj->getObjTraceId();
	}
	else
	{
		return g_emptyString;
	}
} // getIntTraceId


//-----------------------------------------------------------------------------
// getObjectElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//		NULL - don't have the pointer of the object
//
SOCmnObject* SOCmnElementIList::getObjectCmnElementIList(void)
{
	return NULL;
} // getObjectElementIList


//-----------------------------------------------------------------------------
// getLeafList
//
// - get leaf list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementIList::getLeafList(void)
{
	return NULL;
} // getLeafList


//-----------------------------------------------------------------------------
// getBranchList
//
// - get branch list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementIList::getBranchList(void)
{
	return NULL;
} // getBranchList


//-----------------------------------------------------------------------------
// getAdditionalList
//
// - get additional list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementIList::getAdditionalList(void)
{
	return NULL;
} // getAdditionalList

BOOL SOCmnElementIList::createAdditionalList(SOCmnListConfig* /* cfg */)
{
	return FALSE;
}

//-----------------------------------------------------------------------------
// SOCmnElementListLeaf                                                       |
//-----------------------------------------------------------------------------

SOCmnElementListLeaf::SOCmnElementListLeaf(IN OPTIONAL WORD leafListType)
{
	if (leafListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (leafListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_leafList.create(type);
	}
}

SOCmnElementListLeaf::~SOCmnElementListLeaf()
{
	m_leafList.destroy();
}


//-----------------------------------------------------------------------------
// getLeafList
//
// - get leaf list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListLeaf::getLeafList(void)
{
	return &m_leafList;
} // getLeafList


//-----------------------------------------------------------------------------
// SOCmnElementListBranch                                                     |
//-----------------------------------------------------------------------------

SOCmnElementListBranch::SOCmnElementListBranch(IN OPTIONAL WORD branchListType)
{
	if (branchListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (branchListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_branchList.create(type);
	}
}

SOCmnElementListBranch::~SOCmnElementListBranch()
{
	m_branchList.destroy();
}

//-----------------------------------------------------------------------------
// getBranchList
//
// - get branch list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListBranch::getBranchList(void)
{
	return &m_branchList;
} // getBranchList


//-----------------------------------------------------------------------------
// SOCmnElementListLeafBranch                                                 |
//-----------------------------------------------------------------------------

SOCmnElementListLeafBranch::SOCmnElementListLeafBranch(IN OPTIONAL WORD leafListType, IN OPTIONAL WORD branchListType)
{
	if (leafListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (leafListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_leafList.create(type);
	}

	if (branchListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (branchListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_branchList.create(type);
	}
}

SOCmnElementListLeafBranch::~SOCmnElementListLeafBranch()
{
	m_leafList.destroy();
	m_branchList.destroy();
}

//-----------------------------------------------------------------------------
// getLeafList
//
// - get leaf list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafBranch::getLeafList(void)
{
	return &m_leafList;
} // getLeafList


//-----------------------------------------------------------------------------
// getBranchList
//
// - get branch list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafBranch::getBranchList(void)
{
	return &m_branchList;
} // getBranchList


//-----------------------------------------------------------------------------
// SOCmnElementListAdditional                                                 |
//-----------------------------------------------------------------------------

SOCmnElementListAdditional::SOCmnElementListAdditional(IN OPTIONAL WORD additionalListType)
{
	if (additionalListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (additionalListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_additionalList.create(type);
	}
}

SOCmnElementListAdditional::~SOCmnElementListAdditional()
{
	m_additionalList.destroy();
}

//-----------------------------------------------------------------------------
// getAdditionalList
//
// - get additional list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListAdditional::getAdditionalList(void)
{
	return &m_additionalList;
} // getAdditionalList


BOOL SOCmnElementListAdditional::createAdditionalList(SOCmnListConfig* cfg)
{
	return m_additionalList.create(cfg);
} // getAdditionalList



//-----------------------------------------------------------------------------
// SOCmnElementListBranchAdditional                                           |
//-----------------------------------------------------------------------------

SOCmnElementListBranchAdditional::SOCmnElementListBranchAdditional(IN OPTIONAL WORD branchListType, IN OPTIONAL WORD additionalListType)
{
	if (branchListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (branchListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_branchList.create(type);
	}

	if (additionalListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (additionalListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_additionalList.create(type);
	}
}

SOCmnElementListBranchAdditional::~SOCmnElementListBranchAdditional()
{
	m_additionalList.destroy();
	m_branchList.destroy();
}

//-----------------------------------------------------------------------------
// getBranchList
//
// - get branch list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListBranchAdditional::getBranchList(void)
{
	return &m_branchList;
} // getBranchList


//-----------------------------------------------------------------------------
// getAdditionalList
//
// - get additional list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListBranchAdditional::getAdditionalList(void)
{
	return &m_additionalList;
} // getAdditionalList


BOOL SOCmnElementListBranchAdditional::createAdditionalList(SOCmnListConfig* cfg)
{
	return m_additionalList.create(cfg);
} // getAdditionalList




//-----------------------------------------------------------------------------
// SOCmnElementListLeafAdditional                                             |
//-----------------------------------------------------------------------------

SOCmnElementListLeafAdditional::SOCmnElementListLeafAdditional(IN OPTIONAL WORD leafListType, IN OPTIONAL WORD additionalListType)
{
	if (leafListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (leafListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_leafList.create(type);
	}

	if (additionalListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (additionalListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_additionalList.create(type);
	}
}

SOCmnElementListLeafAdditional::~SOCmnElementListLeafAdditional()
{
	m_additionalList.destroy();
	m_leafList.destroy();
}

//-----------------------------------------------------------------------------
// getLeafList
//
// - get branch list
//
// returns:
//		leaf list
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafAdditional::getLeafList(void)
{
	return &m_leafList;
} // getLeafList


//-----------------------------------------------------------------------------
// getAdditionalList
//
// - get additional list
//
// returns:
//		additional element list
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafAdditional::getAdditionalList(void)
{
	return &m_additionalList;
} // getAdditionalList


BOOL SOCmnElementListLeafAdditional::createAdditionalList(SOCmnListConfig* cfg)
{
	return m_additionalList.create(cfg);
} // getAdditionalList



//-----------------------------------------------------------------------------
// SOCmnElementListLeafBranchAdditional                                       |
//-----------------------------------------------------------------------------

SOCmnElementListLeafBranchAdditional::SOCmnElementListLeafBranchAdditional(
	IN OPTIONAL WORD leafListType,
	IN OPTIONAL WORD branchListType,
	IN OPTIONAL WORD additionalListType)
{
	if (leafListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (leafListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_leafList.create(type);
	}

	if (branchListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (branchListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_branchList.create(type);
	}

	if (additionalListType)
	{
		WORD type = SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED;
		type |= (additionalListType & (SOCMNLIST_TYPE_ORG_MASK | SOCMNLIST_TYPE_OBJECT_KEY_STRING));
		m_additionalList.create(type);
	}
}

SOCmnElementListLeafBranchAdditional::~SOCmnElementListLeafBranchAdditional()
{
	m_additionalList.destroy();
	m_leafList.destroy();
	m_branchList.destroy();
}

//-----------------------------------------------------------------------------
// getLeafList
//
// - get branch list
//
// returns:
//		leaf list
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafBranchAdditional::getLeafList(void)
{
	return &m_leafList;
} // getLeafList


//-----------------------------------------------------------------------------
// getBranchList
//
// - get branch list
//
// returns:
//		NULL
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafBranchAdditional::getBranchList(void)
{
	return &m_branchList;
} // getBranchList


//-----------------------------------------------------------------------------
// getAdditionalList
//
// - get additional list
//
// returns:
//		additional element list
//
SOCmnList<SOCmnElement> *SOCmnElementListLeafBranchAdditional::getAdditionalList(void)
{
	return &m_additionalList;
} // getAdditionalList


BOOL SOCmnElementListLeafBranchAdditional::createAdditionalList(SOCmnListConfig* cfg)
{
	return m_additionalList.create(cfg);
} // getAdditionalList

#endif // SOFEATURE_CMN_ELEMENT
