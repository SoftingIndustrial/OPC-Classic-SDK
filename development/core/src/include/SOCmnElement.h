#ifndef _SOCMNELEMENT_H_
#define _SOCMNELEMENT_H_

#ifdef SOFEATURE_CMN_ELEMENT

#include "SOCmnObject.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCmnElementIName                                                          |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementIName
{
public:
	SOCmnElementIName();

	virtual SOCmnString getName(void);
	virtual DWORD getNameInt(void);
	virtual void getName(OUT DWORD& name);
	virtual void getName(OUT SOCmnString& name);

	virtual BOOL isUniqueName(void) const;

protected:
	virtual ~SOCmnElementIName();

	// callbacks
	virtual void onSetName();

	virtual BOOL setElementName(IN LPCTSTR name);
	virtual BOOL setElementName(IN DWORD name);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SOCmnElementIName


//-----------------------------------------------------------------------------
// SOCmnElementNameString                                                     |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementNameString : virtual public SOCmnElementIName
{
public:
	SOCmnElementNameString();
	virtual SOCmnString getName(void);
	virtual void getName(OUT SOCmnString& name);

	virtual BOOL isUniqueName(void) const;

protected:
	SOCmnString m_name;             // element name

	virtual BOOL setElementName(IN LPCTSTR name);
}; // SOCmnElementNameString


//-----------------------------------------------------------------------------
// SOCmnElementNameInt                                                        |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementNameInt : virtual public SOCmnElementIName
{
public:
	SOCmnElementNameInt();
	virtual DWORD getNameInt(void);
	virtual void getName(OUT DWORD& name);
	virtual SOCmnString getName(void);
	virtual void getName(OUT SOCmnString& name);

	virtual BOOL isUniqueName(void) const;

protected:
	DWORD m_name;               // element name

	virtual BOOL setElementName(IN DWORD name);
}; // SOCmnElementNameInt




//-----------------------------------------------------------------------------
// SOCmnElementNameIntString                                                  |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementNameIntString : virtual public SOCmnElementIName
{
public:
	SOCmnElementNameIntString();
	virtual DWORD getNameInt(void);
	virtual void getName(OUT DWORD& name);
	virtual SOCmnString getName(void);
	virtual void getName(OUT SOCmnString& name);

	virtual BOOL isUniqueName(void) const;

protected:
	DWORD m_name;                   // element id
	SOCmnString m_szName;   // element name

	virtual BOOL setElementName(IN DWORD name);
	virtual BOOL setElementName(IN LPCTSTR name);
}; // SOCmnElementNameInt


//-----------------------------------------------------------------------------
// SOCmnElementIList                                                          |
//-----------------------------------------------------------------------------

class SOCmnElement;

class SOCMN_EXPORT SOCmnElementIList
{
public:
	SOCmnElementIList();

	virtual SOCmnList<SOCmnElement> *getLeafList(void);
	virtual SOCmnList<SOCmnElement> *getBranchList(void);
	virtual SOCmnList<SOCmnElement> *getAdditionalList(void);

	virtual BOOL createAdditionalList(SOCmnListConfig* cfg);
protected:
	virtual ~SOCmnElementIList();

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SOCmnElementIList


//-----------------------------------------------------------------------------
// SOCmnElementListLeaf                                                       |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListLeaf : virtual public SOCmnElementIList
{
public:
	SOCmnElementListLeaf(IN OPTIONAL WORD leafListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getLeafList(void);

protected:
	virtual ~SOCmnElementListLeaf();

	SOCmnList<SOCmnElement> m_leafList;     // list of child leafs
}; // SOCmnElementListLeaf


//-----------------------------------------------------------------------------
// SOCmnElementListBranch                                                     |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListBranch : virtual public SOCmnElementIList
{
public:
	SOCmnElementListBranch(IN OPTIONAL WORD branchListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getBranchList(void);

protected:
	virtual ~SOCmnElementListBranch();

	SOCmnList<SOCmnElement> m_branchList;   // list of child branches
}; // SOCmnElementListBranch


//-----------------------------------------------------------------------------
// SOCmnElementListLeafBranch                                                 |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListLeafBranch : virtual public SOCmnElementIList
{
public:
	SOCmnElementListLeafBranch(IN OPTIONAL WORD leafListType = SOCMNLIST_TYPE_ORG_LIST,
							   IN OPTIONAL WORD branchListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getLeafList(void);
	virtual SOCmnList<SOCmnElement> *getBranchList(void);

protected:
	virtual ~SOCmnElementListLeafBranch();

	SOCmnList<SOCmnElement> m_leafList;     // list of child leafs
	SOCmnList<SOCmnElement> m_branchList;   // list of child branches
}; // SOCmnElementListLeafBranch


//-----------------------------------------------------------------------------
// SOCmnElementListAdditional                                                 |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListAdditional : virtual public SOCmnElementIList
{
public:
	SOCmnElementListAdditional(IN OPTIONAL WORD additionalListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getAdditionalList(void);
	virtual BOOL createAdditionalList(SOCmnListConfig* cfg);

protected:
	virtual ~SOCmnElementListAdditional();

	SOCmnList<SOCmnElement> m_additionalList;   // list of additional child elements
}; // SOCmnElementListLeafAdditional


//-----------------------------------------------------------------------------
// SOCmnElementListBranchAdditional                                           |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListBranchAdditional : virtual public SOCmnElementIList
{
public:
	SOCmnElementListBranchAdditional(IN OPTIONAL WORD branchListType = SOCMNLIST_TYPE_ORG_LIST,
									 IN OPTIONAL WORD additionalListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getBranchList(void);
	virtual SOCmnList<SOCmnElement> *getAdditionalList(void);
	virtual BOOL createAdditionalList(SOCmnListConfig* cfg);

protected:
	virtual ~SOCmnElementListBranchAdditional();

	SOCmnList<SOCmnElement> m_branchList;       // list of child branches
	SOCmnList<SOCmnElement> m_additionalList;   // list of additional child elements
}; // SOCmnElementListBranchAdditional


//-----------------------------------------------------------------------------
// SOCmnElementListLeafAdditional                                             |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListLeafAdditional : virtual public SOCmnElementIList
{
public:
	SOCmnElementListLeafAdditional(IN OPTIONAL WORD leafListType = SOCMNLIST_TYPE_ORG_LIST,
								   IN OPTIONAL WORD additionalListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getLeafList(void);
	virtual SOCmnList<SOCmnElement> *getAdditionalList(void);
	virtual BOOL createAdditionalList(SOCmnListConfig* cfg);

protected:
	virtual ~SOCmnElementListLeafAdditional();

	SOCmnList<SOCmnElement> m_leafList;         // list of child leafs
	SOCmnList<SOCmnElement> m_additionalList;   // list of additional child elements
}; // SOCmnElementListLeafAdditional


//-----------------------------------------------------------------------------
// SOCmnElementListLeafBranchAdditional                                       |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnElementListLeafBranchAdditional : virtual public SOCmnElementIList
{
public:
	SOCmnElementListLeafBranchAdditional(IN OPTIONAL WORD leafListType = SOCMNLIST_TYPE_ORG_LIST,
										 IN OPTIONAL WORD branchListType = SOCMNLIST_TYPE_ORG_LIST,
										 IN OPTIONAL WORD additionalListType = SOCMNLIST_TYPE_ORG_LIST);

	// get child lists
	virtual SOCmnList<SOCmnElement> *getBranchList(void);
	virtual SOCmnList<SOCmnElement> *getLeafList(void);
	virtual SOCmnList<SOCmnElement> *getAdditionalList(void);
	virtual BOOL createAdditionalList(SOCmnListConfig* cfg);

protected:
	virtual ~SOCmnElementListLeafBranchAdditional();

	SOCmnList<SOCmnElement> m_leafList;         // list of child leafs
	SOCmnList<SOCmnElement> m_branchList;       // list of child branches
	SOCmnList<SOCmnElement> m_additionalList;   // list of additional child elements
}; // SOCmnElementListLeafBranchAdditional




//-----------------------------------------------------------------------------
// SOCmnElement                                                               |
//-----------------------------------------------------------------------------

#define GenericElement SOCmnElement
#define GenericLeaf SOCmnElement
#define GenericBranch SOCmnElement

class SOCMN_EXPORT SOCmnElement :
	public SOCmnObject,
	virtual public SOCmnElementIName,
	virtual public SOCmnElementIList
{
public:
	SOCmnElement(IN DWORD objType);

	// element name
	virtual HRESULT setName(IN LPCTSTR name);
	virtual HRESULT setName(IN DWORD name);

	// validate name
	virtual HRESULT validateName(IN LPCTSTR name, IN OPTIONAL BOOL checkIfUnique = TRUE);

	// element path
	virtual SOCmnString getFullName(void);
	virtual SOCmnString getPath(void);

	// parent branch
	virtual BOOL setParentBranch(IN SOCmnElement* parent);
	SOCmnElement* getParentBranch(void);

	// object trace id
	virtual LPCTSTR getObjTraceId(void);

	// object identity
	virtual BOOL getKey(OUT SOCmnString& key);
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN LPCTSTR key);
	virtual BOOL hasKey(IN DWORD key);

	// path splitting
	void reducePath(IN LPCTSTR path, OUT LPTSTR& first,
					OUT LPTSTR& second, IN OPTIONAL BOOL fromLeft = TRUE);
	static void reducePath(IN LPCTSTR path, OUT LPTSTR& first,
						   OUT LPTSTR& second, IN BOOL fromLeft, IN LPCTSTR accept);

	// add child
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);
	virtual BOOL addAdditionalElement(IN SOCmnElement* newEl);

	// remove children
	virtual BOOL removeLeaf(IN SOCmnElement* leaf);
	virtual BOOL removeBranch(IN SOCmnElement* branch);
	virtual BOOL removeAdditionalElement(IN SOCmnElement* addEl);
	virtual void removeAllLeafs(IN OPTIONAL BOOL deep = TRUE);
	virtual void removeAllLeaves(IN OPTIONAL BOOL deep = TRUE);
	virtual void removeAllBranches(IN OPTIONAL BOOL deep = TRUE);
	virtual void removeAllAdditionalElements(IN OPTIONAL BOOL deep = TRUE);

	// find child
	virtual SOCmnElement* findLeaf(IN LPCTSTR name, IN OPTIONAL BOOL deep = FALSE);
	virtual SOCmnElement* findBranch(IN LPCTSTR name, IN OPTIONAL BOOL deep = FALSE);
	virtual SOCmnElement* findAdditionalElement(IN LPCTSTR name);

	// check if a name is a unique child name
	BOOL isUniqueChildName(IN LPCTSTR name,
						   IN OPTIONAL SOCmnElement* child = NULL);

	virtual TCHAR getTreeDelimiter(OUT LPCTSTR* accept, OUT LPCTSTR* bad);

	// is visible during browsing
	virtual BOOL isBrowsable(IN SOCmnObject* pServer);

protected:
	SOCmnElement*	m_parent;     // parent branch

	virtual ~SOCmnElement(void);

	// have to check the uniqueness of the children?
	virtual BOOL trustUniqueChilds(void);

	// callbacks
	virtual void onSetParentBranch();

	// change the name of a child object in lists
	HRESULT changeChildName(IN SOCmnElement* child, IN LPCTSTR name);
	HRESULT changeChildName(IN SOCmnElement* child, IN DWORD name);

	virtual SOCmnObject* getObjectCmnElementIName(void);

	static TCHAR s_delimCreate;// = _T('\x7f');

}; // SOCmnElement

#pragma pack(pop)

#endif // SOFEATURE_CMN_ELEMENT

#endif
