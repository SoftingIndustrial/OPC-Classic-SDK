#ifndef _SODACTPBROWSE_H_
#define _SODACTPBROWSE_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push,4)

//-----------------------------------------------------------------------------
// SODaCTPNameSpaceBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPNameSpaceBrowserImpl
	: virtual public SODaCNameSpaceBrowserICommunication
{
public:
	SODaCTPNameSpaceBrowserImpl(SODaCServer* pServer);

	virtual BOOL expand20(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual BOOL expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR itemPath, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual LPCTSTR retrieveItemID(SOCltBrowseObject* pObj);

	virtual void initUse(void);
	virtual void use20(void);
	virtual void use30(void);
	virtual BOOL support20(void);
	virtual BOOL support30(void);

protected:
	virtual ~SODaCTPNameSpaceBrowserImpl();
};



class SODAC_EXPORT SODaCTPNameSpaceBrowser :
	public SODaCNameSpaceBrowser,
	virtual public SODaCTPNameSpaceBrowserImpl
{
public:
	SODaCTPNameSpaceBrowser(SODaCServer* pServer);
};


#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SODACBROWSE_H_
