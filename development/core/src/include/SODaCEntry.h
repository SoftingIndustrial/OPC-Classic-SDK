#ifndef _SODACENTRY_H_
#define _SODACENTRY_H_

#pragma pack(push, 4)


// object state flags for server, group and item objects
#define SODAC_STATE_FLAG_CONNECTED      0x01
#define SODAC_STATE_FLAG_STARTED        0x02

//-----------------------------------------------------------------------------
// SODaCCreator                                                               |
//-----------------------------------------------------------------------------

class SODaCItem;
class SODaCGroup;
class SODaCServer;
class SODaCNode;
class SODaCTag;
class SODaCProperty;
class SODaCEntry;
#ifdef SOFEATURE_SOAP
class SOCltTaskXmlGetStatus;
#endif // SOFEATURE_SOAP


class SODAC_EXPORT SODaCCreator : public SOCltCreator
{
public:
	SODaCCreator();
	virtual SODaCServer* createServer(IN BYTE protocolType, IN SODaCEntry* parent);
	virtual SODaCGroup* createGroup(IN BYTE protocolType, IN SODaCServer* parent);
	virtual SODaCItem* createItem(IN SODaCGroup* parent);
	virtual SODaCNode* createNode(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual SODaCTag* createTag(IN SOCltBrowseObject* parent, LPCTSTR name);
	virtual SODaCProperty* createProperty(IN SOCltBrowseObject* parent, LPCTSTR name, DWORD propID, VARTYPE datatype, LPCTSTR parentID, LPCTSTR parentPath);

#ifdef SOFEATURE_SOAP
	virtual SOCltTaskXmlGetStatus* createTaskXmlGetStatus(IN SOCltElement* server, IN DWORD cycle);
	virtual void setXmlGetStatusInterval(IN const DWORD& getStatusInterval);
#endif

protected:
	virtual ~SODaCCreator();

protected:
#ifdef SOFEATURE_SOAP
	DWORD m_xmlGetStatusInterval;
#endif
}; // SODaCCreator

#define ClientCreator SODaCCreator




//-----------------------------------------------------------------------------
// SODaCEntry                                                                 |
//-----------------------------------------------------------------------------

class SODaCCreator;
class SODaCConfig;

class SODAC_EXPORT SODaCEntry :
	public SOCltEntry,
	virtual public SOCmnElementListBranch,
	virtual public SOCltElementErrorStore
{
public:
	SODaCEntry(void);
	virtual ~SODaCEntry(void);

	// Overrides
	virtual BOOL terminate(void);

	// server creation
	SODaCServer* addServer(IN LPCTSTR url);
	SODaCServer* parseURL(IN LPCTSTR url, IN SODaCServer* pServerIn);
#ifdef SOFEATURE_DCOM
	SODaCServer* addServer(IN LPCTSTR serverName, IN LPCTSTR nodeName,
						   IN REFCLSID classID, IN DWORD clsctx = CLSCTX_ALL); // DCOM only
#endif

	// object factory
	void setCreator(IN SODaCCreator* pCreator);
	SODaCCreator* getCreator(void);
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
	SODaCServer* getServer(IN LPCTSTR serverName);

	// server list
	inline SOCmnList<SODaCServer> *getServerList(void)
	{
		return (SOCmnList<SODaCServer> *)getBranchList();
	}

#ifdef SOFEATURE_SOAP
	SOCltHTTP* getHTTPClient(void)
	{
		m_httpClient.addRef();
		return m_httpClient;
	}
#endif
#ifdef SOFEATURE_TUNNEL
	SOCltTP* getTPClient(void)
	{
		m_tpClient.addRef();
		return m_tpClient;
	}
#endif

protected:
	SODaCCreator* m_creator;        // creator object
	DWORD m_fileVersion;                // version of currently loaded file
#ifdef SOFEATURE_SOAP
	SOCmnPointer<SOCltHTTP> m_httpClient;
#endif
#ifdef SOFEATURE_TUNNEL
	SOCmnPointer<SOCltTP> m_tpClient;
#endif

	virtual HRESULT doStateTransition(IN BOOL deep, IN BYTE stateDeep);

	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCmnObject* getObjectCltElementIError(void);

}; // SODaCEntry

inline SODaCCreator* SODaCEntry::getCreator()
{
	if (m_creator)
	{
		m_creator->addRef();
	}

	return (SODaCCreator*)m_creator;
}

#define OPCSession SODaCEntry




//-----------------------------------------------------------------------------
// Globals                                                                    |
//-----------------------------------------------------------------------------

// get reference to SODaC Entry object
SODAC_EXPORT SODaCEntry* getOPCSession(void);
SODAC_EXPORT SODaCEntry* getSODaCEntry(void);

#pragma pack(pop)
#endif
