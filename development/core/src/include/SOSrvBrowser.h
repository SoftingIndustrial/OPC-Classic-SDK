#ifndef _SOSRVBROWSER_H_
#define _SOSRVBROWSER_H_

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOSrvBrowser                                                               |
//-----------------------------------------------------------------------------

#define SOSRVBROWSER_TYPE_BRANCH 0x00000001
#define SOSRVBROWSER_TYPE_ITEM   0x00000002

class SOSRV_EXPORT SOSrvBrowser
{
public:
	enum browserFilterType
	{
		typeBranch = SOSRVBROWSER_TYPE_BRANCH,  // DA: OPC_BRANCH, AE: OPC_AREA, DA3: OPC_BROWSE_FILTER_BRANCHES
		typeLeaf   = 0x00000010,                // DA: OPC_LEAF, AE: OPC_SOURCE
		typeFlat   = 0x10000000,                // DA: OPC_FLAT
		typeItem   = SOSRVBROWSER_TYPE_ITEM,    // DA3: OPC_BROWSE_FILTER_ITEMS
		typeAll    = SOSRVBROWSER_TYPE_BRANCH | SOSRVBROWSER_TYPE_ITEM  // DA3: OPC_BROWSE_FILTER_ALL
	};

	enum browserDirection
	{
		dirUp = 1, // DA: OPC_BROWSE_UP, AE:OPCAE_BROWSE_UP
		dirDown,   // DA: OPC_BROWSE_DOWN, AE:OPCAE_BROWSE_DOWN
		dirTo      // DA: OPC_BROWSE_TO, AE:OPCAE_BROWSE_TO
	};

	enum browserOrganisation
	{
		orgHierarchical = 1,
		orgFlat
	};

protected:
	SOSrvBrowser(void);
	virtual ~SOSrvBrowser();

	SOCmnPointer<SOCmnElement> m_curBrowse;
	// current browse node
	SOCmnString m_curBrowsePath;        // current browse path

	// get root of browse object space
	virtual SOCmnElement* getBrowserRoot(void) = 0;

	// object and string based
	virtual enum SOSrvBrowser::browserOrganisation queryOrganization(void);
	virtual HRESULT changeBrowsePosition(IN enum browserDirection direction,
										 IN LPCTSTR name);
	virtual HRESULT browseCurrentBranch(IN enum browserFilterType browseType,
										IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
										IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList,
										IN OPTIONAL LPCTSTR path = NULL);
	virtual BOOL getQualifiedElementName(IN LPCTSTR name, OUT SOCmnString& path);
	virtual BOOL getQualifiedBranchName(IN LPCTSTR name, OUT SOCmnString& path);
	virtual BOOL getQualifiedLeafName(IN LPCTSTR name, OUT SOCmnString& path);

	// string based overwirtables
	virtual BOOL isLeafName(IN LPCTSTR fullName);
	virtual BOOL isBranchName(IN LPCTSTR fullName);
	virtual HRESULT getChildNames(IN enum browserFilterType browseType,
								  IN LPCTSTR filterCriteria, IN VARTYPE dataTypeFilter,
								  IN DWORD accessRightsFilter, OUT SOCmnStringList& nameList);
	virtual BOOL browseOnlyStringBased(void);
	virtual HRESULT changeBrowsePositionStringBased(
		IN IN enum browserDirection direction, IN LPCTSTR name);
	virtual BOOL getQualifiedElementNameStringBased(IN LPCTSTR name, OUT SOCmnString& path);

	// helpers for differences between DA and AE
	virtual BOOL ignoreFilterTypeOnFlatOrg(void);

	// string filter used for browsing
	virtual BOOL matchStringFilter(IN LPCTSTR string, IN LPCTSTR filter);
	virtual BOOL matchAdditionalFilters(IN SOCmnElement* obj,
										IN VARTYPE dataTypeFilter, IN DWORD accessRightsFilter);

	// tracing
	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectSrvBrowser(void);
}; // SOSrvBrowser

#pragma pack(pop)
#endif
