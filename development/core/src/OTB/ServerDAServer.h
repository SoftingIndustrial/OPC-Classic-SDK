#ifndef _SERVERDASERVER_H_
#define _SERVERDASERVER_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDAServer
//-----------------------------------------------------------------------------

class ServerDAServer : public SODaSServer
{
public:
	ServerDAServer(IN OPTIONAL BOOL publiServerDAGroups = FALSE,
				   IN OPTIONAL BYTE serverInstance = 0,
				   IN OPTIONAL DWORD minRequestLoopTime = INFINITE);

	static HRESULT getPropertyItemIDI(IN SOCmnElement* nsElement, IN LPCTSTR itemId, IN DWORD propId, OUT SOCmnString& propItemID);
	static LPCTSTR getOtsPropertyItemID(OTSPropertyData* pPropDataO, SODaSItemTag* pItemTag, SOCmnString& itemID);

	virtual DWORD setFlags(BYTE flags);

protected:
	virtual ~ServerDAServer(void);

	virtual DWORD getPropertyIDByItemID(IN LPCTSTR parentName, IN LPCTSTR propItemID);
	virtual HRESULT getPropertyItemID(IN SOCmnElement* nsElement, IN LPCTSTR itemId, IN DWORD propId, OUT SOCmnString& propItemID);

	// startup synchronisation
	virtual HRESULT waitForNameSpaceStartup(void);
	virtual HRESULT waitForIOStartup(void);

	// virtuals for string based namespace
	virtual BOOL isNodeName(IN LPCTSTR fullName);
	virtual BOOL isTagName(IN LPCTSTR fullName);
	virtual enum SOSrvBrowser::browserOrganisation queryOrganization(void);
	virtual void getChildNamesOfNodeName(IN LPCTSTR nodeName,
										 IN enum SOSrvBrowser::browserFilterType browseType,
										 IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
										 IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList);
	virtual BOOL browseOnlyStringBased(void);
	virtual HRESULT getChildrenData(IN LPCTSTR itemId, IN enum browserFilterType fType, IN LPCTSTR elementNameFilter,
									IN LPCTSTR vendorFilter, IN DWORD maxElementsReturned, IN OUT SOCmnString& contPoint,
									OUT BOOL& moreElements, IN OUT SOCmnList<SODaSBrowseElementData> *pListBE);
	virtual void initTag(IN SODaSTag* tag);
	virtual void initNode(IN SODaSNode* pNode);
	virtual void queryAvailablePropertiesByItemID(IN LPCTSTR itemId, IN BOOL isSOAP, IN BOOL returnPropertyValues,
												  OUT SOCmnList<SODaSPropertyData> &propertyList);
	virtual BOOL isPropertyName(IN LPCTSTR itemId, IN DWORD propId, IN LPCTSTR propIdString);
	void onSetClientName(IN LPCTSTR clientName);

	// security
	virtual BOOL isPrivateSecurityAvailable(void);
	virtual BOOL logon(LPCTSTR userName, LPCTSTR password);
	virtual BOOL logoff(void);
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#ifdef OTB_XMLDA_SERVER
	virtual BOOL checkAuthorization(IN SODaSXmlMethod* pMth);
#endif // SOFEATURE_SOAP

	// helpers
	void addStringBasedChildren(IN LPCTSTR nodeName, IN SOCmnList<SODaSBrowseElementData> *pListBE, IN BOOL deep);
	void addAEDataToBEList(DWORD countAE, OTSAddressSpaceElementData* pDataAE, SOCmnList<SODaSBrowseElementData> *pListBE);
	void initItemTag(IN SODaSItemTag* pItemTag);

#ifndef OTB_USER_COMPILED
#ifdef SOFEATURE_DCOM
	// query interface for own interfaces
	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
#endif
#endif
}; // ServerDAServer


#pragma pack(pop)
#endif
