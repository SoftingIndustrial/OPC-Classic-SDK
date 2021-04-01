#ifndef _SODASSERVER_H_
#define _SODASSERVER_H_

#include <opcda.h>
#include "SOCmnElement.h"
#include "SODaSItemTag.h"
#include "SODaSTag.h"
#include "SODaSRequest.h"
#include "SODaSTransaction.h"
#include "SOSrvServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaSBrowseElementData                                                     |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSBrowseElementData :
	public SOCmnData
{
public:
	SODaSBrowseElementData();

	// data of browse element
	SOCmnString m_name;   // name
	SOCmnString m_itemID; // item id
	DWORD m_flags;        // OPC_BROWSE_xxx flags
	SOCmnPointer<SOCmnElement> m_nsElement; // namespace object
	VARTYPE m_datatype;
	DWORD m_accessRights;
}; // SODaSBrowseElementData



//-----------------------------------------------------------------------------
// SODaSServer                                                                |
//-----------------------------------------------------------------------------

#define SODASSERVER_XMLNS_ERROR     0x0001
#define SODASSERVER_XMLNS_PROPERTY  0x0002

#define SODASSERVER_FLAG_DCOM          0x01
#define SODASSERVER_FLAG_SOAP          0x02
#define SODASSERVER_FLAG_HTTP_RECEIVER 0x04
#define SODASSERVER_FLAG_TUNNEL        0x08
#define SODASSERVER_FLAG_INTERNAL      0x10
#define SODASSERVER_FLAG_PUBLIC_GROUPS 0x80

class SODaSTransaction;
class SODaSGroup;
class SODaSItem;
class SODaSTag;
class SODaSNode;
class SODaSEntry;
#ifdef SOFEATURE_DCOM
class SODaSComServer;
class SODaSComGroup;
#endif
#ifdef SOFEATURE_SOAP
class SODaSXmlServer;
#endif
#ifdef SOFEATURE_TUNNEL
class SODaSTPServer;
class SODaSTPGroup;
#endif

class SODAS_EXPORT SODaSServer :
	public SOSrvServer,
	public SOSrvBrowser,
	virtual public SOCmnElementListBranch
{
	friend class SODaSTransaction;
	friend class SODaSGroup;
	friend class SODaSItem;
	friend class SODaSRequest;
	friend class SODaSEntry;
#ifdef SOFEATURE_DCOM
	friend class SODaSComServer;
	friend class SODaSComGroup;
#endif
#ifdef SOFEATURE_SOAP
	friend class SODaSXmlServer;
#endif
#ifdef SOFEATURE_TUNNEL
	friend class SODaSTPServer;
	friend class SODaSTPGroup;
#endif

public:
	SODaSServer(IN OPTIONAL BOOL publicGroups = FALSE,
				IN OPTIONAL BYTE serverInstance = 0,
				IN OPTIONAL DWORD minRequestLoopTime = INFINITE);

	virtual BOOL addBranch(IN SOCmnElement* newBranch);
	virtual SOCmnElement* findBranch(IN LPCTSTR name, IN OPTIONAL BOOL deep = FALSE);

	virtual LPCTSTR getObjTraceId(void);

	// timeout of requests
	BOOL setDuetimeRead(IN DWORD time);
	DWORD getDuetimeRead(void) const;
	BOOL setDuetimeWrite(IN DWORD time);
	DWORD getDuetimeWrite(void) const;

	// time of last client update
	BOOL getUpdateTime(OUT SOCmnDateTime& time) const;

	// bandwidth
	virtual DWORD getBandwidth(void);
	virtual LONG forceDisconnect(LPCTSTR reason)
	{
		return S_OK;
	};

#ifdef SOFEATURE_DCOM
	// IOPCServer methods
	virtual HRESULT getStatus(OUT OPCSERVERSTATUS* status);
#endif

	virtual HRESULT addGroup(IN LPCTSTR name, IN BOOL active,
							 IN DWORD reqUpdateRate, IN OPCHANDLE clientHandle, IN LONG* pTimeBias,
							 IN FLOAT* pPercentDeadband, IN DWORD lcid, OUT SODaSGroup** group,
							 OUT DWORD* revUpdateRate, IN OPTIONAL BOOL addWatchChange = TRUE);
	virtual HRESULT removeGroup(IN OPCHANDLE serverHandle,
								IN OPTIONAL BOOL remPublicGrp = FALSE);

	// group retrival
	virtual SODaSGroup* getGroupByName(IN LPCTSTR name);
	virtual SODaSGroup* getGroupByHandle(IN OPCHANDLE serverHandle);
	virtual SODaSGroup** allocGroupsAsArray(OUT int* pnGroups);
	virtual void        freeGroupsAsArray(SODaSGroup** pGroups);

	// is public groups server
	BOOL isPublicGroups(void) const;

#ifdef SOFEATURE_DCOM
	// start the sending of the shutdown request to the OPC client
	HRESULT startShutdownRequest(IN LPCTSTR reason,
								 IN OPTIONAL HANDLE event = INVALID_HANDLE_VALUE);
	// send a shutdown request to the OPC client
	virtual HRESULT sendShutdownRequest(IN LPCTSTR reason);
#endif

	// transaction list
	SOCmnList<SODaSTransaction> *getTransactionList(void)
	{
		return &m_transactionList;
	}

	// group list
	inline SOCmnList<SODaSGroup> *getGroupList(void)
	{
		return (SOCmnList<SODaSGroup> *)getBranchList();
	}

#ifdef SOFEATURE_DCOM
	// check if connection to OPC client is OK
	virtual BOOL checkClientConnection(OPTIONAL IN BOOL checkAllIFs = FALSE);
	// disconnect the OPC client belonging to the server object -> deletes the server object
	virtual void disconnectClient(void);
#endif

#ifdef SOFEATURE_DCOM
	// COM object handling
	void* getMe(void);
	ULONG releaseMe(IN void* me);

	virtual void getCLSID(OUT CLSID& classID);
#endif

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// get item tag object
	virtual HRESULT getItemTag(IN LPCTSTR itemID, OUT SODaSItemTag** tag);
	virtual SOCmnElement* getNameSpaceElement(IN LPCTSTR itemID);
	virtual BOOL isNameSpaceElement(IN LPCTSTR itemID, OPTIONAL OUT SOCmnElement** nsElement = NULL);
	virtual DWORD getItemID(IN LPCTSTR itemPath, IN LPCTSTR itemName, OUT SOCmnString& itemID);

#ifdef SOFEATURE_SOAP
	virtual DWORD insertXMLMethod(BYTE xmlMethodType, IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);

	virtual BOOL checkHoldTime(LONGLONG holdTime, DWORD waitTime, DWORD updateRate, DWORD pingRate, SODaSTransaction* pTac);

	virtual BOOL checkAuthorization(IN SODaSXmlMethod* pMth);

	virtual BOOL getVendorXMLNamespace(IN WORD forWhat, OUT SOCmnString& szVendorNS);
#endif

	BYTE getFlags(void)
	{
		return m_flags;
	}
	virtual DWORD setFlags(BYTE flags);

#ifdef SOFEATURE_TUNNEL
	virtual SOSrvTPConnection* getTPConnection(void)
	{
		m_tpCon.addRef();
		return m_tpCon;
	}
	virtual void setTPConnection(SOSrvTPConnection* tpCon)
	{
		m_tpCon = tpCon;
		m_tpCon.addRef();
	}
#endif

protected:
	virtual ~SODaSServer();

	DWORD m_dueTimeRead;            // timeout of synchronous read request
	DWORD m_dueTimeWrite;           // timeout of synchronous write request
	SOCmnDateTime m_lastUpdateTime;     // time of last update to OPC client
	DWORD m_bandwidth;              // bandwidth of server
	SOCmnList<SODaSTransaction> m_transactionList;  // transaction list
	// lock for transaction list
	BYTE m_publicGroupTemplates;    // is public groups server
	BYTE m_flags;
	SOCmnString m_secretString;		// secret string to hash the continuation point with

#ifdef SOFEATURE_TUNNEL
	SOCmnPointer<SOSrvTPConnection> m_tpCon;
#endif

	// IOPCBrowse
	virtual HRESULT browse(IN LPCTSTR itemId, IN SOCmnElement* pElement, IN enum browserFilterType fType,
						   IN LPCTSTR elementNameFilter, IN LPCTSTR vendorFilter, IN DWORD maxElementsReturned,
						   IN OUT SOCmnString& contPoint, OUT BOOL& moreElements, OUT SOCmnList<SODaSBrowseElementData> *pListBE);
	virtual HRESULT getProperties(IN LPCTSTR itemId, IN SOCmnElement* pElement,
								  IN DWORD propCnt, IN DWORD* pPropIDs, IN BOOL isSOAP, IN BOOL returnPropertyValues, OUT SOCmnList<SODaSPropertyData> *pListPD);

	// IOPCItemProperties
	virtual void queryAvailablePropertiesByItemID(IN LPCTSTR itemId, IN BOOL isSOAP, IN BOOL returnPropertyValues,
												  OUT SOCmnList<SODaSPropertyData> &propertyList);
	virtual BOOL isPropertyName(IN LPCTSTR itemId, IN DWORD propId, IN LPCTSTR propIdString, SODaSPropertyData* pPropData);

	virtual HRESULT readProperties(IN LPCTSTR itemId, IN SOCmnElement* pElement, IN DWORD accessRights,
								   IN SOCmnList<SODaSPropertyData> *pListPD, IN DWORD propCnt, IN DWORD* pPropIDs, IN BOOL isSOAP,
								   OUT OPCITEMPROPERTIES* pItemProperties, OUT LPVARIANT pVariants, OUT SOCmnVariant** ppVar, OUT HRESULT* pErrors);

	// IOPCBrowseServerAddressSpace
	// callbacks for string-based namespace
	virtual BOOL isNodeName(IN LPCTSTR fullName);
	virtual BOOL isTagName(IN LPCTSTR fullName);
	virtual BOOL isBranchName(IN LPCTSTR fullName);
	virtual BOOL isLeafName(IN LPCTSTR fullName);
	virtual VARTYPE getTagDatatypeByItemID(IN LPCTSTR fullName);
	virtual VARTYPE getNodeDatatypeByItemID(IN LPCTSTR fullName);
	virtual HRESULT getAccessPathsByItemID(IN LPCTSTR name,
										   OUT SOCmnStringList& pathList);
	virtual DWORD getAccessRightsByItemID(IN LPCTSTR fullName);
	virtual void getChildNamesOfNodeName(IN LPCTSTR nodeName, IN enum browserFilterType browseType,
										 IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
										 IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList);

	virtual HRESULT getChildNames(IN enum browserFilterType browseType,
								  IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
								  IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList);
	virtual HRESULT getChildrenData(IN LPCTSTR itemId, IN enum browserFilterType fType, IN LPCTSTR elementNameFilter,
									IN LPCTSTR vendorFilter, IN DWORD maxElementsReturned, IN OUT SOCmnString& contPoint,
									OUT BOOL& moreElements, IN OUT SOCmnList<SODaSBrowseElementData> *pListBE);
	virtual SOCmnElement* getBrowserRoot(void);
	virtual SOCmnObject* getObjectSrvBrowser(void);
	virtual BOOL ignoreFilterTypeOnFlatOrg(void);
	virtual BOOL matchAdditionalFilters(IN SOCmnElement* obj,
										IN VARTYPE dataTypeFilter, IN DWORD accessRightsFilter);
	virtual BOOL matchVendorFilter(IN SOCmnElement* obj,
								   IN LPCTSTR vendorFilter);
	virtual BOOL isInvalidOrUnknownItemID(LPCTSTR itemID);

	// string based creation of namespace tag and node objects
	virtual SODaSTag* createTag(IN LPCTSTR fullName);
	virtual void initTag(IN SODaSTag* tag);
	virtual SODaSNode* createNode(IN LPCTSTR fullName);
	virtual void initNode(IN SODaSNode* pNode);
	virtual SODaSTag* createStringBasedTag(LPCTSTR itemId);
	virtual SODaSNode* createStringBasedNode(LPCTSTR itemId);

	// string based property object creation
	virtual SODaSProperty* createProperty(IN SOCmnElement* element, IN LPCTSTR itemId, IN DWORD propId, IN LPCTSTR propName);
	virtual void initProperty(IN SODaSProperty* property, SODaSPropertyData* pPropData);
	virtual HRESULT canCreateProperty(IN SOCmnElement* element, IN LPCTSTR itemId, IN DWORD propId, IN SODaSPropertyData* pPropData);

	// property item ids
	virtual DWORD getPropertyIDByItemID(IN LPCTSTR parentName, IN LPCTSTR propItemID);
	virtual HRESULT getPropertyItemID(IN SOCmnElement* element, IN LPCTSTR itemId, IN DWORD propId,
									  OUT SOCmnString& propItemID);
	virtual DWORD getPropertyIDByName(IN LPCTSTR propName);
	virtual LPCTSTR getPropertyNameByID(IN DWORD propID, OUT SOCmnString& propName);

	// string filter used for browsing
	virtual BOOL matchStringFilter(IN LPCTSTR string, IN LPCTSTR filter);

	// transaction list management
	virtual void addTransaction(IN SODaSTransaction* trans);
	virtual BOOL removeTransaction(IN SODaSTransaction* trans);
	virtual HRESULT cancelTransaction(IN DWORD transId);

	// transaction actions
	virtual DWORD doWork(IN DWORD startTime);
	virtual DWORD updateTransactions(IN DWORD now, IN DWORD minSpan);
	virtual DWORD startCyclicTransactions(IN DWORD now, IN DWORD minSpan);
#ifdef SOFEATURE_SOAP
	virtual DWORD handleXMLMethods(IN SOCmnDateTime& now, IN DWORD nowTick, IN DWORD minSpan);
	virtual DWORD checkSubscriptions(IN SOCmnDateTime& now, IN DWORD nowTick, IN DWORD minSpan);
#endif
#ifdef SOFEATURE_DCOM
	virtual DWORD sendKeepAlive(IN SOCmnDateTime& now, IN DWORD nowTick, IN DWORD minSpan);
#endif

	// public groups server methods
	virtual BOOL canRemovePublicGroup(IN SODaSGroup* group);

#ifdef SOFEATURE_DCOM
	// handle the last external release of the COM object
	BOOL handleLastExternalReleaseWithInternalRefs(IN void* object, IN void* comServer);

	// query interface for own interfaces
	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
#endif

	virtual SOCmnObject* getObjectCmnElementIList(void);

	// what type of server object
	virtual DWORD getSpecID(void);

	// standard properties
	virtual BOOL isStandardTagProperty(IN DWORD propId);
	virtual BOOL isStandardNodeProperty(IN DWORD propId);
	virtual void getStandardTagProperties(IN VARTYPE tagNativeDatatype, IN BOOL isSOAP,
										  OUT SOCmnList<SODaSPropertyData> &propertyList);
	virtual void getStandardNodeProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> &propertyList);

	// startup synchronisation
	virtual HRESULT waitForNameSpaceStartup(void);
	virtual HRESULT waitForIOStartup(void);

#ifdef SOFEATURE_DCOM
#ifdef SOFEATURE_COMIF_HOOKS
	// interface function callbacks
	virtual HRESULT onBSetLocaleID(IN LCID dwLcid);
	virtual HRESULT onBGetLocaleID(OUT LCID* pdwLcid);
	virtual HRESULT onBQueryAvailableLocaleIDs(OUT DWORD* pdwCount, OUT LCID** pdwLcid);
	virtual HRESULT onBGetErrorString(IN HRESULT dwError, OUT LPWSTR* ppString);
	virtual HRESULT onBSetClientName(IN LPCWSTR szName);
	virtual HRESULT onBAdvise(IN LPUNKNOWN unknown, OUT DWORD* cookie);
	virtual HRESULT onBUnadvise(IN DWORD cookie);
	virtual HRESULT onBAddGroup(IN LPCWSTR szName, IN BOOL bActive, IN DWORD dwRequestedUpdateRate, IN OPCHANDLE hClientGroup, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD dwLCID, OUT OPCHANDLE* phServerGroup, OUT DWORD* pRevisedUpdateRate, IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBGetErrorString(IN HRESULT dwError, IN LCID dwLocale, OUT LPWSTR* ppString);
	virtual HRESULT onBGetGroupByName(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBGetStatus(OUT OPCSERVERSTATUS** ppServerStatus);
	virtual HRESULT onBRemoveGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce);
	virtual HRESULT onBCreateGroupEnumerator(IN OPCENUMSCOPE dwScope, IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBGetPublicGroupByName(IN LPCWSTR szName, IN REFIID riid, OUT LPUNKNOWN* ppUnk);
	virtual HRESULT onBRemovePublicGroup(IN OPCHANDLE hServerGroup, IN BOOL bForce);
	virtual HRESULT onBQueryAvailableProperties(IN LPWSTR szItemID, OUT DWORD* pdwCount, OUT DWORD** ppPropertyIDs, OUT LPWSTR** ppDescriptions, OUT VARTYPE** ppvtDataTypes);
	virtual HRESULT onBGetItemProperties(IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, OUT VARIANT** ppvData, OUT HRESULT** ppErrors);
	virtual HRESULT onBLookupItemIDs(IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, OUT LPWSTR** ppszNewItemIDs, OUT HRESULT** ppErrors);
	virtual HRESULT onBQueryOrganization(OUT OPCNAMESPACETYPE* pNameSpaceType);
	virtual HRESULT onBChangeBrowsePosition(IN OPCBROWSEDIRECTION dwBrowseDirection, IN LPCWSTR szString);
	virtual HRESULT onBBrowseOPCItemIDs(IN OPCBROWSETYPE dwBrowseFilterType, IN LPCWSTR szFilterCriteria, IN VARTYPE vtDataTypeFilter, IN DWORD dwAccessRightsFilter, OUT LPENUMSTRING* ppIEnumString);
	virtual HRESULT onBGetItemID(IN LPWSTR szItemDataID, OUT LPWSTR* szItemID);
	virtual HRESULT onBBrowseAccessPaths(IN LPCWSTR szItemID, OUT LPENUMSTRING* ppIEnumString);
	virtual HRESULT onBGetClassID(OUT CLSID* pClassID);
	virtual HRESULT onBIsDirty(void);
	virtual HRESULT onBLoad(IN LPCOLESTR pszFileName, IN DWORD dwMode);
	virtual HRESULT onBSave(IN LPCOLESTR pszFileName, IN BOOL fRemember);
	virtual HRESULT onBSaveCompleted(IN LPCOLESTR pszFileName);
	virtual HRESULT onBGetCurFile(OUT LPOLESTR* ppszFileName);
	virtual HRESULT onBGetChildren(IN LPCWSTR parent, IN BOOL withAttributes, OUT LPWSTR* children);
	virtual HRESULT onBCollectChanges(IN LPCWSTR objects, IN DWORD doCollect);
	virtual HRESULT onBGetChanges(OUT LPWSTR* changes);
	virtual HRESULT onBGetStatistics(OUT LPWSTR* statistics);
	virtual HRESULT onBGetObject(IN LPCWSTR objectDescription, OUT LPWSTR* object);
	virtual HRESULT onBSetTraceOptions(IN LPCWSTR options);
	virtual HRESULT onBGetTraceOptions(OUT LPWSTR* options);
	virtual HRESULT onBSaveTraceOptions(void);
	virtual HRESULT onBGetTrace(OUT LPWSTR* trace);
	virtual HRESULT onBRead(IN DWORD count, IN LPCWSTR* pItemIDs, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors);
	virtual HRESULT onBWriteVQT(IN DWORD count, IN LPCWSTR* pItemIDs, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);
	virtual HRESULT onBGetProperties(IN DWORD itemCount, IN LPWSTR* pItemIDs, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT OPCITEMPROPERTIES** ppItemProperties);
	virtual HRESULT onBBrowse(IN LPWSTR itemID, IN OUT LPWSTR* pContinuationPoint, IN DWORD maxElementsReturned, IN OPCBROWSEFILTER browseFilter, IN LPWSTR elementNameFilter, IN LPWSTR vendorFilter, IN BOOL returnAllProperties, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT BOOL* pMoreElements, OUT DWORD* pCount, OUT OPCBROWSEELEMENT** ppBrowseElements);
	virtual HRESULT onBIsAvailablePriv(OUT BOOL* pbAvailable);
	virtual HRESULT onBLogon(IN LPCWSTR szUserID, IN LPCWSTR szPassword);
	virtual HRESULT onBLogoff(void);
	virtual void onESetLocaleID(IN HRESULT res, IN LCID dwLcid);
	virtual void onEGetLocaleID(HRESULT res, IN LCID* pdwLcid);
	virtual void onEQueryAvailableLocaleIDs(IN HRESULT res, IN DWORD* pdwCount, IN LCID** pdwLcid);
	virtual void onEGetErrorString(IN HRESULT res, IN HRESULT dwError, IN LPWSTR* ppString);
	virtual void onESetClientName(IN HRESULT res, IN LPCWSTR szName);
	virtual void onEAdvise(IN HRESULT res, IN LPUNKNOWN unknown, IN DWORD* cookie);
	virtual void onEUnadvise(IN HRESULT res, IN DWORD cookie);
	virtual void onEAddGroup(IN HRESULT res, IN LPCWSTR szName, IN BOOL bActive, IN DWORD dwRequestedUpdateRate, IN OPCHANDLE hClientGroup, IN LONG* pTimeBias, IN FLOAT* pPercentDeadband, IN DWORD dwLCID, OUT OPCHANDLE* phServerGroup, IN DWORD* pRevisedUpdateRate, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onEGetErrorString(IN HRESULT res, IN HRESULT dwError, IN LCID dwLocale, IN LPWSTR* ppString);
	virtual void onEGetGroupByName(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onEGetStatus(IN HRESULT res, IN OPCSERVERSTATUS** ppServerStatus);
	virtual void onERemoveGroup(IN HRESULT res, IN OPCHANDLE hServerGroup, IN BOOL bForce);
	virtual void onECreateGroupEnumerator(IN HRESULT res, IN OPCENUMSCOPE dwScope, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onEGetPublicGroupByName(IN HRESULT res, IN LPCWSTR szName, IN REFIID riid, IN LPUNKNOWN* ppUnk);
	virtual void onERemovePublicGroup(IN HRESULT res, IN OPCHANDLE hServerGroup, IN BOOL bForce);
	virtual void onEQueryAvailableProperties(IN HRESULT res, IN LPWSTR szItemID, IN DWORD* pdwCount, IN DWORD** ppPropertyIDs, IN LPWSTR** ppDescriptions, IN VARTYPE** ppvtDataTypes);
	virtual void onEGetItemProperties(IN HRESULT res, IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, IN VARIANT** ppvData, IN HRESULT** ppErrors);
	virtual void onELookupItemIDs(IN HRESULT res, IN LPWSTR szItemID, IN DWORD dwCount, IN DWORD* pdwPropertyIDs, IN LPWSTR** ppszNewItemIDs, IN HRESULT** ppErrors);
	virtual void onEQueryOrganization(IN HRESULT res, IN OPCNAMESPACETYPE* pNameSpaceType);
	virtual void onEChangeBrowsePosition(IN HRESULT res, IN OPCBROWSEDIRECTION dwBrowseDirection, IN LPCWSTR szString);
	virtual void onEBrowseOPCItemIDs(IN HRESULT res, IN OPCBROWSETYPE dwBrowseFilterType, IN LPCWSTR szFilterCriteria, IN VARTYPE vtDataTypeFilter, IN DWORD dwAccessRightsFilter, IN LPENUMSTRING* ppIEnumString);
	virtual void onEGetItemID(IN HRESULT res, IN LPWSTR szItemDataID, IN LPWSTR* szItemID);
	virtual void onEBrowseAccessPaths(IN HRESULT res, IN LPCWSTR szItemID, IN LPENUMSTRING* ppIEnumString);
	virtual void onEGetClassID(IN HRESULT res, IN CLSID* pClassID);
	virtual void onEIsDirty(IN HRESULT res);
	virtual void onELoad(IN HRESULT res, IN LPCOLESTR pszFileName, IN DWORD dwMode);
	virtual void onESave(IN HRESULT res, IN LPCOLESTR pszFileName, IN BOOL fRemember);
	virtual void onESaveCompleted(IN HRESULT res, IN LPCOLESTR pszFileName);
	virtual void onEGetCurFile(IN HRESULT res, IN LPOLESTR* ppszFileName);
	virtual void onEGetChildren(IN HRESULT res, IN LPCWSTR parent, IN BOOL withAttributes, IN LPWSTR* children);
	virtual void onECollectChanges(IN HRESULT res, IN LPCWSTR objects, IN DWORD doCollect);
	virtual void onEGetChanges(IN HRESULT res, IN LPWSTR* changes);
	virtual void onEGetStatistics(IN HRESULT res, IN LPWSTR* statistics);
	virtual void onEGetObject(IN HRESULT res, IN LPCWSTR objectDescription, OUT LPWSTR* object);
	virtual void onESetTraceOptions(IN HRESULT res, IN LPCWSTR options);
	virtual void onEGetTraceOptions(IN HRESULT res, OUT LPWSTR* options);
	virtual void onESaveTraceOptions(IN HRESULT res);
	virtual void onEGetTrace(IN HRESULT res, OUT LPWSTR* trace);
	virtual void onERead(IN HRESULT res, IN DWORD count, IN LPCWSTR* pItemIDs, IN DWORD* pMaxAge, OUT VARIANT** ppValues, OUT WORD** ppQualities, OUT FILETIME** ppTimeStamps, OUT HRESULT** ppErrors);
	virtual void onEWriteVQT(IN HRESULT res, IN DWORD count, IN LPCWSTR* pItemIDs, IN OPCITEMVQT* pItemVQT, OUT HRESULT** ppErrors);
	virtual void onEGetProperties(IN HRESULT res, IN DWORD itemCount, IN LPWSTR* pItemIDs, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT OPCITEMPROPERTIES** ppItemProperties);
	virtual void onEBrowse(IN HRESULT res, IN LPWSTR itemID, IN OUT LPWSTR* pContinuationPoint, IN DWORD maxElementsReturned, IN OPCBROWSEFILTER browseFilter, IN LPWSTR elementNameFilter, IN LPWSTR vendorFilter, IN BOOL returnAllProperties, IN BOOL returnPropertyValues, IN DWORD propertyCount, IN DWORD* pPropertyIDs, OUT BOOL* pMoreElements, OUT DWORD* pCount, OUT OPCBROWSEELEMENT** ppBrowseElements);
	virtual void onEIsAvailablePriv(IN HRESULT res, OUT BOOL* pbAvailable);
	virtual void onELogon(IN HRESULT res, IN LPCWSTR szUserID, IN LPCWSTR szPassword);
	virtual void onELogoff(IN HRESULT res);
#endif // SOFEATURE_COMIF_HOOKS
#endif // SOFEATURE_DCOM
}; // SODaSServer


inline BOOL SODaSServer::setDuetimeRead(IN DWORD time)
{
	m_dueTimeRead = time;
	return TRUE;
}

inline DWORD SODaSServer::getDuetimeRead(void) const
{
	return m_dueTimeRead;
}

inline BOOL SODaSServer::setDuetimeWrite(IN DWORD time)
{
	m_dueTimeWrite = time;
	return TRUE;
}

inline DWORD SODaSServer::getDuetimeWrite(void) const
{
	return m_dueTimeWrite;
}

inline BOOL SODaSServer::getUpdateTime(OUT SOCmnDateTime& time) const
{
	time = m_lastUpdateTime;
	return TRUE;
}

inline BOOL SODaSServer::isPublicGroups(void) const
{
	return m_publicGroupTemplates;
}

#define GenericServer SODaSServer




//-----------------------------------------------------------------------------
// SODaSObjectRoot                                                            |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSObjectRoot :
	public SOSrvObjectRoot
{
public:
	SODaSObjectRoot(void);

	virtual BOOL addBranch(IN SOCmnElement* newBranch);

	// server list
	inline SOCmnList<SODaSServer> *getServerList(void)
	{
		return (SOCmnList<SODaSServer> *)getBranchList();
	}

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif
}; // SODaSObjectRoot

#define OPCRootOPC SODaSObjectRoot

#pragma pack(pop)
#endif
