#ifndef _SODACCOMSERVER_H_
#define _SODACCOMSERVER_H_

#ifdef SOFEATURE_DCOM

#include "SODaCServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCComServerImpl                                                                |
//-----------------------------------------------------------------------------

class SODaCServer;

class SODAC_EXPORT SODaCComServerImpl : virtual public SODaCServerICommunication
{
	friend class SODaCServer;
public:
	SODaCComServerImpl();

	virtual WORD getSupportedOPCVersion(void);
	virtual BOOL forceOPCVersion(WORD opcVersion);
	virtual WORD getForcedOPCVersion(void)
	{
		return m_forcedVersion;
	}

	virtual HRESULT getStatus(OUT SODaCServerStatus& status);
	virtual HRESULT queryAvailableProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData,
											 OUT SOCmnList<SODaCPropertyData> &propertyList);
	virtual HRESULT getPropertiesData(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN WORD whatData,
									  IN OUT SOCmnList<SODaCPropertyData> &propertyList);

	SODaCGroup* getPublicGroupByName(IN LPCTSTR groupName);

	// query COM interface
	virtual HRESULT queryInterfaceImpl(IN REFIID riid, IN void** interf);

	// IOPCServer interface
	HRESULT getErrorString(IN HRESULT error, IN LCID locale, OUT SOCmnString* string);
	HRESULT getStatus(OUT OPCSERVERSTATUS* serverStatus, OUT SOCmnString* vendorInfo);

	// IOPCBrowseServerAddressSpace interface
	HRESULT queryOrganization(OUT OPCNAMESPACETYPE* nameSpaceType);
	HRESULT changeBrowsePosition(IN OPCBROWSEDIRECTION direction,
								 IN OPTIONAL LPCTSTR node = NULL);
	HRESULT browseOPCItemIDs(IN OPCBROWSETYPE browseType,
							 IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
							 IN DWORD accessRightsFilter, OUT SOCmnStringList& itemIDs);
	SOCmnString getItemID(IN LPCTSTR itemID, OUT OPTIONAL HRESULT* result = NULL);
	HRESULT browseAccessPaths(IN LPCTSTR itemID, OUT SOCmnStringList& accessPaths);

	//  IPersistFile
	BOOL isDirty(void);
	HRESULT load(IN LPCTSTR fileName, IN DWORD mode);
	HRESULT save(IN LPCTSTR fileName, IN BOOL remember);
	HRESULT saveCompleted(IN LPCTSTR fileName);
	HRESULT getCurFile(OUT SOCmnString* fileName);

	// item I/O
	virtual HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, OUT SOCmnVariant* pValues,
						 OUT WORD* pQualities = NULL, OUT SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	virtual HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues,
						  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	virtual BOOL serializeImpl(IN OUT SOCltStorage& ar, IN BOOL isStoring);

protected:
	WORD m_version;                      // supported OPC version of the server
	WORD m_forcedVersion;        // forced OPC version to use

	// interfaces of the server object
	SOCltInterfaceAccess<IOPCServer, &_ATL_IIDOF(IOPCServer)> m_server;
	SOCltInterfaceAccess<IOPCServerPublicGroups, &_ATL_IIDOF(IOPCServerPublicGroups)> m_serverPub;
	SOCltInterfaceAccess<IOPCBrowseServerAddressSpace, &_ATL_IIDOF(IOPCBrowseServerAddressSpace)> m_browseSAS;
	SOCltInterfaceAccess<IOPCItemProperties, &_ATL_IIDOF(IOPCItemProperties)> m_properties;
	SOCltInterfaceAccess<IPersistFile, &IID_IPersistFile> m_persistFile;
	SOCltInterfaceAccess<IOPCItemIO, &IID_IOPCItemIO> m_itemIO;
	SOCltInterfaceAccess<IOPCBrowse, &IID_IOPCBrowse> m_browse;

	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SOCLT_SRVPAR_ALL);

protected:
	virtual ~SODaCComServerImpl();

	virtual HRESULT doDisconnectImpl(BOOL connectionMonitorStatus);

	// do the state transitions
	virtual HRESULT doStateTransitionImpl(void);

	virtual SODaCNameSpaceBrowser* getNameSpaceBrowserImpl(void);

	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
}; // SODaCComServerImpl


inline WORD SODaCComServerImpl::getSupportedOPCVersion(void)
{
	return m_version;
}



//-----------------------------------------------------------------------------
// SODaCServerDefault                                                         |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCComServerDefault :
	public SODaCServer,
	virtual public SOCltComServerImpl,
	virtual public SODaCComServerImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCComServerDefault();

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
};

#define ClientServer SODaCComServerDefault


#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif
