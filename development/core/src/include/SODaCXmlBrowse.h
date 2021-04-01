#ifndef _SODACXMLBROWSE_H_
#define _SODACXMLBROWSE_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCXmlNameSpaceBrowserImpl                                                      |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlNameSpaceBrowserImpl
	: virtual public SODaCNameSpaceBrowserICommunication
{
public:
	SODaCXmlNameSpaceBrowserImpl(SODaCServer* pServer);

	virtual BOOL expand20(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual BOOL expand30(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR itemPath, IN DWORD type, IN SOCmnString& contPoint, OUT SOCmnList<SOCltBrowseObject> &objList);
	virtual LPCTSTR retrieveItemID(SOCltBrowseObject* pObj);

	virtual void initUse(void);
	virtual void use20(void);
	virtual void use30(void);
	virtual BOOL support20(void);
	virtual BOOL support30(void);

protected:
	virtual ~SODaCXmlNameSpaceBrowserImpl();
};



class SODAC_EXPORT SODaCXmlNameSpaceBrowser :
	public SODaCNameSpaceBrowser,
	virtual public SODaCXmlNameSpaceBrowserImpl
{
public:
	SODaCXmlNameSpaceBrowser(SODaCServer* pServer);
};


#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif // _SODACBROWSE_H_
