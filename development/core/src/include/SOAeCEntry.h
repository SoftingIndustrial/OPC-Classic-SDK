#ifndef _SOAECENTRY_H_
#define _SOAECENTRY_H_

#pragma pack(push, 4)


class SOAeCEntry;
class SOAeCServer;
class SOAeCEvent;
class SOAeCSubscription;
class SOAeCArea;
class SOAeCSource;
class SOAeCCondition;
class SOAeCCategory;

//-----------------------------------------------------------------------------
// SOAeCCreator                                                               |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCCreator : public SOCltCreator
{
public:
	SOAeCCreator();

	virtual SOAeCServer* createServer(IN BYTE protocolType, IN SOAeCEntry* parent);
	virtual SOAeCSubscription* createSubscription(IN BYTE protocolType, IN SOAeCServer* parent);
	virtual SOAeCEvent* createEvent(IN SOAeCSubscription* subs, IN ONEVENTSTRUCT* pData);
	virtual SOAeCArea* createArea(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual SOAeCSource* createSource(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual SOAeCCondition* createCondition(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual SOAeCCategory* createCategory(IN SOCltBrowseObject* parent, LPCTSTR name);

protected:
	virtual ~SOAeCCreator();
}; // SOAeCCreator




//-----------------------------------------------------------------------------
// SOAeCEntry                                                                 |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCEntry :
	public SOCltEntry,
	virtual public SOCmnElementListBranch,
	virtual public SOCltElementErrorStore
{
public:
	SOAeCEntry(void);

	// server creation
	SOAeCServer* addServer(IN LPCTSTR serverName, IN LPCTSTR nodeName,
						   IN REFCLSID classID, IN DWORD clsctx = CLSCTX_ALL);
	SOAeCServer* addServer(IN LPCTSTR url);
	SOAeCServer* parseURL(IN LPCTSTR url, IN SOAeCServer* pServerIn);

	// Overrides
	virtual BOOL terminate(void);

	// object factory
	void setCreator(IN SOAeCCreator* pCreator);
	SOAeCCreator* getCreator(void);
	virtual SOCltCreator* getCltCreator(void);

	// tree management
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);
	DWORD getFileVersion(void)
	{
		return m_fileVersion;
	}

	// get child object by name
	SOAeCServer* getServer(IN LPCTSTR serverName);

	// server list
	inline SOCmnList<SOAeCServer> *getServerList(void)
	{
		return (SOCmnList<SOAeCServer> *)getBranchList();
	}

#ifdef SOFEATURE_TUNNEL
	SOCltTP* getTPClient(void)
	{
		m_tpClient.addRef();
		return m_tpClient;
	}
#endif

protected:
	SOAeCCreator* m_creator;            // creator object
	DWORD m_fileVersion;                        // version of currently loaded file
#ifdef SOFEATURE_TUNNEL
	SOCmnPointer<SOCltTP> m_tpClient;
#endif

	virtual HRESULT doStateTransition(IN BOOL deep, IN BYTE stateDeep);

	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCmnObject* getObjectCltElementIError(void);
	virtual ~SOAeCEntry(void);
}; // SOAeCEntry

inline SOAeCCreator* SOAeCEntry::getCreator()
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return m_creator;
}


//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

// get reference to SOAeCEntry object
SOAEC_EXPORT SOAeCEntry* getSOAeCEntry(void);

#pragma pack(pop)
#endif
