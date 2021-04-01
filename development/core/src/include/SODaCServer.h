#ifndef _SODACSERVER_H_
#define _SODACSERVER_H_

#include "SOCltElement.h"
#include "SODaCEntry.h"


#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SODaCPropertyData                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCPropertyData : public SOCmnData
{
public:
	SODaCPropertyData()
	{
		m_propertyID = 0;
		m_datatype = VT_EMPTY;
		m_error = S_OK;
	}

	SODaCPropertyData(SODaCPropertyData* pProp)
	{
		m_propertyID = pProp->m_propertyID;   // property id
		m_description = pProp->m_description; // description
		m_datatype = pProp->m_datatype;       // data type
		m_value = pProp->m_value;                 // property value
		m_itemID = pProp->m_itemID;             // item ID of property
		m_itemPath = pProp->m_itemPath;       // item Path of property
		m_error = pProp->m_error;                   // error
		m_name = pProp->m_name;               // XML-DA property name
	}

	virtual ~SODaCPropertyData()    {}

	DWORD m_propertyID;                 // property id
	SOCmnString m_description;  // description
	VARTYPE m_datatype;                 // data type
	SOCmnVariant m_value;           // property value
	SOCmnString m_itemID;           // item ID of property
	SOCmnString m_itemPath;     // item Path of property
	HRESULT m_error;                        // error
	SOCmnString m_name;         // XML-DA property name
};


class SODAC_EXPORT SODaCServerStatus : SOCmnData
{
public:
	SODaCServerStatus(void);
	~SODaCServerStatus(void);

	void clear(void);

	DWORD m_state;                   // COM + XML
	SOCmnDateTime m_startTime;       // COM + XML
	SOCmnDateTime m_currentTime;     // COM + XML
	SOCmnString m_vendorInfo;        // COM + XML
	SOCmnString m_productVersion;    // COM + XML
	SOCmnDWORDList m_supportedLCIDs; // COM + XML
	SOCmnDWORDList m_supportedInterfaces; // COM + XML
	SOCmnDateTime m_lastUpdateTime;  // COM
	DWORD m_groupCount;              // COM
	DWORD m_bandwidth;               // COM
	SOCmnString m_statusInfo;        //       XML
};



class SODaCServer;
class SODaCNameSpaceBrowser;
#ifdef SOFEATURE_SOAP
class SODaCXmlRequestOptions;
#endif

class SODAC_EXPORT SODaCServerICommunication
{
	friend class SODaCServer;
public:
	SODaCServerICommunication(void);
	virtual ~SODaCServerICommunication(void);

	virtual void init(void);

	virtual HRESULT getStatus(OUT SODaCServerStatus& status);
	virtual HRESULT queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData,
											 OUT SOCmnList<SODaCPropertyData> &propertyList);
	virtual HRESULT getPropertiesData(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData,
									  IN OUT SOCmnList<SODaCPropertyData> &propertyList);

	virtual HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, OUT SOCmnVariant* pValues,
						 OUT WORD* pQualities = NULL, OUT SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	virtual HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues,
						  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	virtual WORD getSupportedOPCVersion(void);
	virtual BOOL forceOPCVersion(WORD opcVersion);
	virtual WORD getForcedOPCVersion(void);

	virtual BOOL setTimeout(float timeoutPercent, DWORD timeoutMin, DWORD timeoutMax);
	virtual float getTimeoutPercent(void);
	virtual DWORD getTimeoutMin(void);
	virtual DWORD getTimeoutMax(void);

	virtual void connectionFailureImpl(void);

#ifdef SOFEATURE_DCOM
	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);
#endif

#ifdef SOFEATURE_SOAP
	// XML-DA specific
	virtual void setIPPort(WORD port);
	virtual WORD getIPPort(void);
	virtual void setHTTPPath(LPCTSTR path);
	virtual LPCTSTR getHTTPPath(void);
	virtual SOCltHTTPConnection* getHTTPConnection(void);

	virtual SODaCXmlRequestOptions* fillRequestOptions(SODaCXmlRequestOptions* pReqOptions);

	virtual void setTimeDifference(LONGLONG timeDiffSend, LONGLONG timeDiffReply);
	virtual LONGLONG getTimeDifference(void);
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	//  only if XML-DA disable
	virtual void setIPPort(WORD port);
	virtual WORD getIPPort(void);
#endif // SOFEATURE_SOAP
	// Softing OPC Tunnel specific
	virtual SOCltTPConnection* getTPConnection(void);
#endif

	virtual SODaCServer* getObjectDaCServer(void);

	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

protected:
	virtual HRESULT doStateTransitionImpl(void);
	virtual SODaCNameSpaceBrowser* getNameSpaceBrowserImpl(void);
	virtual BOOL serializeImpl(IN OUT SOCltStorage& ar, IN BOOL isStoring);
	virtual HRESULT doDisconnectImpl(BOOL connectionMonitorStatus);
}; // SODaCServerICommunication


//-----------------------------------------------------------------------------
// SODaCServer                                                                |
//-----------------------------------------------------------------------------

// server errors

// server property get what parameters
#define SODAC_PROPDATA_NONE     0x0000
#define SODAC_PROPDATA_VALUE    0x0001
#define SODAC_PROPDATA_ITEMID   0x0002
#define SODAC_PROPDATA_ALL      0xFFFF

// supported opc version
#define SODAC_VERSION_OPC_USEDEFAULT 0x0000
#define SODAC_VERSION_OPC_DAV10A   0x0101
#define SODAC_VERSION_OPC_DAV20    0x0200
#define SODAC_VERSION_OPC_DAV30    0x0300
#define SODAC_VERSION_OPC_XMLDAV10 0x8100
#define SODAC_VERSION_OPC_TPDA     0x8200

class SODaCGroup;
class SODaCNameSpaceBrowser;
#ifdef SOFEATURE_DCOM
class SODaCComServerImpl;
#endif // SOFEATURE_DCOM
#ifdef SOFEATURE_TUNNEL
class SODaCTPServerImpl;
#endif

class SODAC_EXPORT SODaCServer :
	public SOCltServer,
	virtual public SODaCServerICommunication,
	virtual public SOCmnElementListBranch
{
	friend class SODaCGroup;
#ifdef SOFEATURE_DCOM
	friend class SODaCComServerImpl;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SODaCTPServerImpl;
#endif
public:
	SODaCServer(void);

	// namespace browsing
	virtual SODaCNameSpaceBrowser* getNameSpaceBrowser();

	// group creation
	SODaCGroup* addGroup(IN LPCTSTR groupName = NULL,   IN DWORD reqUpdateRate = 1000,
						 IN OPTIONAL FLOAT* deadband = NULL, IN OPTIONAL LONG* timeBias = NULL,
						 IN OPTIONAL DWORD LCID = 0);

	// tree management
	virtual BOOL addLeaf(IN SOCmnElement* newLeaf);
	virtual BOOL addBranch(IN SOCmnElement* newBranch);
	virtual SOCmnElement* findBranch(IN LPCTSTR name, IN OPTIONAL BOOL deep = FALSE);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	// get child object by name
	inline SODaCGroup* getGroup(IN LPCTSTR groupName);

	SODaCGroup* getGroupByClientHandle(IN OPCHANDLE clientHandle);

	// group list
	inline SOCmnList<SODaCGroup> *getGroupList(void)
	{
		return (SOCmnList<SODaCGroup> *)getBranchList();
	}

#ifdef SOFEATURE_SOAP
	virtual DWORD getPropertyIDByName(IN LPCTSTR propName);
	virtual void setXmlStatusTask(IN SOCltTaskXmlGetStatus* pTask);
#endif

	virtual SODaCServer* getObjectDaCServer(void);

#ifdef SOFEATURE_DCOM
	virtual HRESULT queryInterface(IN REFIID riid, IN void** interf);
#endif

	virtual void connectionFailure(void);

	void setGroupsState(IN BYTE state, IN BOOL itemsToo);

protected:
	// do the state transitions
	virtual HRESULT doStateTransition(BOOL deep, IN BYTE stateDeep);
	virtual HRESULT doUpdate(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

	// Force the object to disconnect
	virtual HRESULT doDisconnect(void);

	virtual ~SODaCServer();

#ifdef SOFEATURE_SOAP
	DWORD m_xmlGetStatusTaskHandle;
#endif
}; // SODaCServer

#define OPCItemPropertyData SODaCPropertyData

#define SODAC_SRVPAR_CLIENT_NAME  SOCLT_SRVPAR_CLIENT_NAME
#define SODAC_SRVPAR_LCID         SOCLT_SRVPAR_LCID
#define SODAC_SRVPAR_ALL          SOCLT_SRVPAR_ALL

#pragma pack(pop)
#endif
