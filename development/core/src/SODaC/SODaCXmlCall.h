#ifndef _SODACXMLCALL_H_
#define _SODACXMLCALL_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


#include "SOCltHttp.h"

//-----------------------------------------------------------------------------
// SODaCXmlRequestOptions                                                     |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlRequestOptions : public SOCmnData
{
public:
	SODaCXmlRequestOptions(void);

	BOOL m_returnErrorText;
	BOOL m_returnDiagnosticInfo;
	BOOL m_returnItemTime;
	BOOL m_returnItemPath;
	BOOL m_returnItemName;
	SOCmnDateTime m_requestDeadline;
	SOCmnString m_localeID;
}; // SODaCXmlRequestOptions



//-----------------------------------------------------------------------------
// SODaCXmlReplyBase                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCXmlReplyBase : public SOCmnData
{
public:
	SODaCXmlReplyBase(void);
}; // SODaCXmlReplyBase



//-----------------------------------------------------------------------------
// SODaCXmlCall                                                               |
//-----------------------------------------------------------------------------

class SODaCXmlResponse;

class SODAC_EXPORT SODaCXmlCall : public SOCltHTTPCall
{
public:
	SODaCXmlCall(void);
	SODaCXmlCall(IN SOCltHTTPConnection* pCon);

	virtual HRESULT getStatus(IN SODaCXmlRequestOptions* pOptions = NULL);
	virtual HRESULT subscribe(IN SODaCGroup* pGroup, IN SODaCXmlRequestOptions* pOptions = NULL, OUT DWORD* pItemCnt = NULL);
	virtual HRESULT subscriptionCancel(IN SODaCXmlGroupImpl* pGroup);
	virtual HRESULT subscriptionPolledRefresh(IN SODaCXmlGroupImpl* pGroup,
											  IN BOOL useGroupsUpdateRateAndKeepAliveTime = TRUE, IN SOCmnDateTime* pHoldTime = NULL,
											  DWORD waitTime = 0, BOOL returnAllItems = FALSE, IN SODaCXmlRequestOptions* pOptions = NULL);
	virtual HRESULT browse(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN LPCTSTR cp, IN DWORD maxElements, IN DWORD browseFilter, IN LPCTSTR elementNameFilter = NULL,
						   IN LPCTSTR vendorFilter = NULL, IN BOOL returnProperties = FALSE, IN BOOL retrievePropertyValues = FALSE, IN SOCmnStringList* pPropertyList = NULL, IN SODaCXmlRequestOptions* pOptions = NULL);
	virtual HRESULT getProperties(IN LPCTSTR itemID, IN LPCTSTR itemPath, IN SOCmnStringList* pItemIDList = NULL, IN SOCmnStringList* pItemPathList = NULL, IN BOOL retrievePropertyValues = FALSE, IN BOOL returnAllProperties = TRUE, IN SOCmnStringList* pPropertyList = NULL, IN SODaCXmlRequestOptions* pOptions = NULL);
	virtual HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN SOCmnVariant* pValues, IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, IN SODaCXmlRequestOptions* pOptions = NULL);
	virtual HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnString* pItemPaths, IN DWORD maxAge, IN SODaCXmlRequestOptions* pOptions = NULL);

	virtual HRESULT getStatusResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getSubscribeResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getSubscriptionCancelResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getSubscriptionPolledRefreshResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getBrowseResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getGetPropertiesResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getWriteResponse(OUT SODaCXmlResponse** ppRes);
	virtual HRESULT getReadResponse(OUT SODaCXmlResponse** ppRes);

	void setRequestOptions(SODaCXmlRequestOptions* pO)
	{
		if (m_pRequestOptions)
		{
			delete m_pRequestOptions;
		}

		m_pRequestOptions = pO;
	}
	SODaCXmlRequestOptions* getRequestOptions(void)
	{
		return m_pRequestOptions;
	}

	virtual HRESULT getErrorCodeFromXML(IN LPCTSTR errCodeSZ);
	virtual void getXMLQuality(WORD quality, IN BOOL withElement, OUT SOCmnStringEx& xmlQuality);
	static WORD getDCOMQuality(IN LPCTSTR quality, IN LPCTSTR limit, IN LPCTSTR vendor);

	SODaCGroup* getGroup(void)
	{
		m_group.addRef();
		return m_group;
	}

	BYTE getSOAPMethod(void)
	{
		return m_soapMethod;
	}

	void doTimeSynchonisation(SODaCXmlResponse* pRes);

protected:
	SOCmnPointer<SODaCGroup> m_group;
	SODaCXmlGroupImpl* m_pGroupXmlImpl;
	SODaCXmlRequestOptions* m_pRequestOptions;
	BYTE m_soapMethod;

	virtual ~SODaCXmlCall(void);

	virtual void onComplete(void);

	HRESULT getResponse(IN BYTE methodType, IN BYTE resonseType, IN LPCTSTR responseText, OUT SODaCXmlResponse** ppRes);

	void getRequestOptionsXML(IN SODaCXmlRequestOptions* pOptions, OUT SOCmnStringEx& reqOptions);

	void addDefaultHTTPAttributes(void);
}; // SODaCXmlCall



//-----------------------------------------------------------------------------
// SODaCXmlResponseData                                                         |
//-----------------------------------------------------------------------------

#define SODACXMLRESPONSEDATA_TYPE_NONE             0x0000
#define SODACXMLRESPONSEDATA_TYPE_PROPERTYNAME     0x0001
#define SODACXMLRESPONSEDATA_TYPE_ITEM             0x0002
#define SODACXMLRESPONSEDATA_TYPE_VALUE            0x0003
#define SODACXMLRESPONSEDATA_TYPE_VALUEARRAY       0x0004
#define SODACXMLRESPONSEDATA_TYPE_QUALITY          0x0005
#define SODACXMLRESPONSEDATA_TYPE_DIAGNOSTICINFO   0x0006
#define SODACXMLRESPONSEDATA_TYPE_SERVERSUBHANDLE  0x0007
#define SODACXMLRESPONSEDATA_TYPE_VENDORINFO       0x0008
#define SODACXMLRESPONSEDATA_TYPE_LOCALEID         0x0009
#define SODACXMLRESPONSEDATA_TYPE_INTERFACEVERSION 0x000A
#define SODACXMLRESPONSEDATA_TYPE_ELEMENT          0x000B
#define SODACXMLRESPONSEDATA_TYPE_PROPERTY         0x000C
#define SODACXMLRESPONSEDATA_TYPE_STATUSINFO       0x000D

class SODAC_EXPORT SODaCXmlResponseData : public SOCmnData
{
public:
	SODaCXmlResponseData(void);
	virtual ~SODaCXmlResponseData(void);

	WORD m_type;

	// XML data
	SOCmnXmlValue m_value;
	SOCmnString m_quality;
	SOCmnString m_qualityLimit;
	SOCmnString m_qualityVendor;
	SOCmnStringEx m_itemName;
	SOCmnStringEx m_clientItemHandle;
	SOCmnStringEx m_itemPath;
	SOCmnString m_timestamp;
	SOCmnString m_resultID;
	SOCmnStringEx m_name;
	SOCmnStringEx m_description;
	BOOL m_isItem;
	BOOL m_hasChildren;
	SOCmnString m_dataSZ;
};

//-----------------------------------------------------------------------------
// SODaCXmlResponse                                                             |
//-----------------------------------------------------------------------------

#define SODACXMLCALL_GETSTATUS                    0x10
#define SODACXMLCALL_BROWSE                       0x20
#define SODACXMLCALL_READ                         0x30
#define SODACXMLCALL_WRITE                        0x40
#define SODACXMLCALL_GETPROPERTIES                0x50
#define SODACXMLCALL_SUBSCRIBE                    0x60
#define SODACXMLCALL_SUBSCRIPTIONPOLLEDREFRESH    0x70
#define SODACXMLCALL_SUBSCRIPTIONCANCEL           0x80

#define SODACXMLRESPONSE_STATE_GETSTATUSRESPONSE                 0x10
#define SODACXMLRESPONSE_STATE_BROWSERESPONSE                    0x20
#define SODACXMLRESPONSE_STATE_READRESPONSE                      0x30
#define SODACXMLRESPONSE_STATE_WRITERESPONSE                     0x40
#define SODACXMLRESPONSE_STATE_GETPROPERTIESRESPONSE             0x50
#define SODACXMLRESPONSE_STATE_SUBSCRIBERESPONSE                 0x60
#define SODACXMLRESPONSE_STATE_SUBSCRIPTIONPOLLEDREFRESHRESPONSE 0x70
#define SODACXMLRESPONSE_STATE_SUBSCRIPTIONCANCELRESPONSE        0x80
#define SODACXMLRESPONSE_STATE_ERROR                             0xD0
#define SODACXMLRESPONSE_STATE_ITEMLIST                          0xF1
#define SODACXMLRESPONSE_STATE_VALUEARRAY                        0xF2
#define SODACXMLRESPONSE_STATE_ELEMENTS                          0xF3
#define SODACXMLRESPONSE_STATE_PROPERTYLISTS                     0xF4

class SODAC_EXPORT SODaCXmlResponse : public SOCmnXmlMethod
{
public:
	SODaCXmlResponse(void);

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
	virtual void handleCharacterData(LPCSTR s, INT len);

	SOCmnStringList* getResponseOptions(void)
	{
		return &m_methodOptions;
	}
	SOCmnList<SODaCXmlResponseData> *getResponseData(void)
	{
		return &m_methodData;
	}

	BOOL removeOPCXMLDANamespace(OUT IN SOCmnString& szXML);

	HRESULT getFault(void)
	{
		return m_fault;
	}

protected:
	SOCmnStringList m_methodOptions;
	SOCmnList<SODaCXmlResponseData> m_methodData;
	SODaCXmlResponseData* m_pCurMethodData;
	SOCmnXmlValue* m_pCurSimpleXmlValue;
	SOCmnVOIDList m_arrayStack;
	WORD m_charDataType;
	HRESULT m_fault;

	~SODaCXmlResponse(void);
}; // SODaCXmlResponse

#pragma pack(pop)

#endif

#endif
