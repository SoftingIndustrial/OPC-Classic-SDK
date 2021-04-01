#ifndef _SODACCOMBROWSE_H_
#define _SODACCOMBROWSE_H_

#ifdef SOFEATURE_DCOM

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCComNameSpaceBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCComNameSpaceBrowserImpl
	: virtual public SODaCNameSpaceBrowserICommunication
{
public:
	SODaCComNameSpaceBrowserImpl(SODaCServer* pServer);

	BOOL expandFlat(OUT SOCmnList<SOCltBrowseObject> &objList);

	BOOL changeToRoot(void);

	SOCltBrowseObject* createBrowseObject(IN SOCltBrowseObject* pParent, IN OPCBROWSEELEMENT* pElement);
	SOCltBrowseObject* createBrowseObject(IN SOCltBrowseObject* pParent, IN OPCITEMPROPERTY* pProperty);

	virtual BOOL expand20(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual BOOL expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR itemPath, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual LPCTSTR retrieveItemID(SOCltBrowseObject* pObj);

	virtual void initUse(void);
	virtual void use20(void);
	virtual void use30(void);
	virtual BOOL support20(void);
	virtual BOOL support30(void);

protected:
	BYTE m_supportBrowseTo;

	virtual ~SODaCComNameSpaceBrowserImpl();
};



class SODAC_EXPORT SODaCComNameSpaceBrowser :
	public SODaCNameSpaceBrowser,
	virtual public SODaCComNameSpaceBrowserImpl
{
public:
	SODaCComNameSpaceBrowser(SODaCServer* pServer);
};


#pragma pack(pop)

#endif // SOFEATURE_DCOM

#endif // _SODACBROWSE_H_
