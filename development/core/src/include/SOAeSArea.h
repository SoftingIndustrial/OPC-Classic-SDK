#ifndef _SOAESAREA_H_
#define _SOAESAREA_H_

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOAeSArea                                                                  |
//-----------------------------------------------------------------------------

class SOAeSSource;

class SOAES_EXPORT SOAeSArea :
	public SOCmnElement,
	virtual public SOCmnElementNameString
{
public:
	SOAeSArea(void);
	SOAeSArea(IN LPCTSTR areaName);

	virtual BOOL isUniqueName(void) const;
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

	// areaspace elements lists
	inline SOCmnList<SOAeSArea> *getAreaList(void)
	{
		return (SOCmnList<SOAeSArea> *)getBranchList();
	}
	inline SOCmnList<SOAeSSource> *getSourceList(void)
	{
		return (SOCmnList<SOAeSSource> *)getLeafList();
	}

protected:
	virtual ~SOAeSArea(void);

	// have to check the uniqueness of the children?
	virtual BOOL trustUniqueChilds(void);

	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SOAeSArea




//-----------------------------------------------------------------------------
// SOAeSAreaDefault                                                           |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSAreaDefault :
	public SOAeSArea,
	virtual public SOCmnElementListLeafBranch
{
public:
	SOAeSAreaDefault();
	SOAeSAreaDefault(IN LPCTSTR areaName);
protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SOAeSAreaDefault




//-----------------------------------------------------------------------------
// SOAeSAreaSpaceRoot                                                         |
//-----------------------------------------------------------------------------

class SOAES_EXPORT SOAeSAreaSpaceRoot :
	public SOCmnElement,
	virtual public SOCmnElementListLeafBranch
{
public:
	SOAeSAreaSpaceRoot();

	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	// tree hierarchy level delimiter and not allowed chars
	void setTreeDelimiter(IN TCHAR create, IN LPCTSTR accept,
						  IN LPCTSTR bad);
	virtual TCHAR getTreeDelimiter(OUT LPCTSTR* accept, OUT LPCTSTR* bad);

	void setTrustUniqueChilds(IN BOOL trust);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	static void getConditions(IN SOCmnElement* parent, OUT SOCmnList<SOAeSCondition> &condList);

	// areaspace elements lists
	inline SOCmnList<SOAeSArea> *getAreaList(void)
	{
		return (SOCmnList<SOAeSArea> *)getBranchList();
	}
	inline SOCmnList<SOAeSSource> *getSourceList(void)
	{
		return (SOCmnList<SOAeSSource> *)getLeafList();
	}

protected:
	// tree delimiters
	TCHAR m_delimAScreate;              // creation tree delimiter
	SOCmnString m_delimASaccept;        // accepted tree delimiter
	SOCmnString m_badASchars;           // bad tree characters
	BYTE m_trustUniqueChilds;           // trust unique childs

	// have to check the uniqueness of the children?
	virtual BOOL trustUniqueChilds(void);

	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SOAeSAreaSpaceRoot

inline void SOAeSAreaSpaceRoot::setTreeDelimiter(IN TCHAR create,
												 IN LPCTSTR accept, IN LPCTSTR bad)
{
	m_delimAScreate = create;
	m_delimASaccept = accept;
	m_badASchars = bad;
}

inline void SOAeSAreaSpaceRoot::setTrustUniqueChilds(IN BOOL trust)
{
	m_trustUniqueChilds = (BYTE)trust;
}

#pragma pack(pop)
#endif
