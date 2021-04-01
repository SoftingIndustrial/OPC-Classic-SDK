#ifndef _SODASXMLSERVER_H_
#define _SODASXMLSERVER_H_

#ifdef SOFEATURE_SOAP

#define SODASXMLSERVER_WAIT_FOR_NAMESPACE_STARTUP \
    res = pServer->waitForNameSpaceStartup(); \
 
#define SODASXMLSERVER_WAIT_FOR_IO_STARTUP \
    res = pServer->waitForIOStartup(); \
 
#pragma pack(push, 4)


#include "SOSrvHttp.h"
#include "SOSrvXML.h"

//-----------------------------------------------------------------------------
// SODaSXmlServer                                                             |
//-----------------------------------------------------------------------------

class SODaSXmlMethod;

class SODAS_EXPORT SODaSXmlServer : public SOSrvHTTPReceiver
{
public:
	SODaSXmlServer(void);

	HRESULT getStatus(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT browse(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT read(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT write(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT getProperties(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT subscribe(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT subscriptionPolledRefresh(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
	HRESULT subscriptionCancel(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);

	void sndSOAPFault(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pSrvData, IN BYTE flags,
					  IN SODaSServer* pServer, IN DWORD errorCode);

	virtual WORD handleHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);
	DWORD handleXmlMethod(IN SODaSServer* pServer, IN SODaSXmlMethod* pMth, IN SOCmnDateTime& now);

	virtual void removeClient(IN SOSrvHTTPHandlerData* pHTTPData);

	void sendPolledRefreshResponse(IN SODaSServer* pServer, IN SODaSXmlMethod* mth, IN SODaSTransaction* pTac);

protected:
	SOCmnStringEx m_resHeader;
	SOCmnStringEx m_resBody;
	SOCmnStringEx m_replyBase;
	SOCmnList<SOSrvHTTPHandlerData> m_groupIdToSessionObjectMap;

	LPCTSTR getReplyBase(SOCmnStringEx& dest, LPCTSTR rbName, IN SOCmnDateTime* pNow, IN SODaSServer* pServer, IN SODaSXmlMethod* pXmlMth);
	void setReqLocaleID(IN SODaSServer* pServer, IN SODaSXmlMethod* pXmlMth);
	void addAbnormalResult(IN SOCmnDWORDList* pAbnormalResultList, IN DWORD propError);
	void getErrorTexts(IN SODaSServer* pServer, IN SOCmnDWORDList* pAbnormalResultList, OUT SOCmnStringEx& resErrors);
	void sendResponse(IN SODaSServer* pServer, IN SODaSXmlMethod* mth, SOCmnDateTime* pNow,
					  SOCmnStringEx* pResHeader, SOCmnStringEx* pResBody);
	void getXMLStart(IN SODaSServer* pServer, IN BYTE methodID, OUT SOCmnStringEx& resBody);

	void doXMLDATypeConversion(VARTYPE nativeDT, VARTYPE reqDT, SOCmnVariant& value);
	BOOL checkXMLDATypeConversion(VARTYPE nativeDT, VARTYPE reqDT);

	void traceInput(IN SODaSServer* pServer, IN SODaSXmlMethod* mth);
}; // SODaSXmlServer



//-----------------------------------------------------------------------------
// SODaSXmlMethodData                                                         |
//-----------------------------------------------------------------------------

#define SODASXMLMETHODDATA_TYPE_NONE            0x0000
#define SODASXMLMETHODDATA_TYPE_PROPERTYNAME    0x0001
#define SODASXMLMETHODDATA_TYPE_ITEM            0x0002
#define SODASXMLMETHODDATA_TYPE_VALUE           0x0003
#define SODASXMLMETHODDATA_TYPE_VALUEARRAY      0x0004
#define SODASXMLMETHODDATA_TYPE_QUALITY         0x0005
#define SODASXMLMETHODDATA_TYPE_DIAGNOSTICINFO  0x0006
#define SODASXMLMETHODDATA_TYPE_SERVERSUBHANDLE 0x0007

class SODAS_EXPORT SODaSXmlMethodData : public SOCmnData
{
public:
	SODaSXmlMethodData(void);
	virtual ~SODaSXmlMethodData(void);

	WORD m_type;

	// XML data
	SOCmnStringEx m_propertyName;
	SOCmnXmlValue m_value;
	SOCmnString m_quality;
	SOCmnString m_qualityLimit;
	SOCmnString m_qualityVendor;
	SOCmnStringEx m_itemName;
	SOCmnStringEx m_clientItemHandle;
	SOCmnStringEx m_itemPath;
	SOCmnString m_maxAge;
	SOCmnString m_reqType;
	SOCmnString m_valueTypeQualifier;
	SOCmnString m_timestamp;
	SOCmnString m_deadband;
	SOCmnString m_reqSamplingRate;
	SOCmnString m_enableBuffering;
	SOCmnString m_serverSubHandle;

	// processing data
	HRESULT m_result;
};


//-----------------------------------------------------------------------------
// SODaSXmlMethod                                                             |
//-----------------------------------------------------------------------------

#define SODASXMLMETHOD_GETSTATUS                 0x10
#define SODASXMLMETHOD_BROWSE                    0x20
#define SODASXMLMETHOD_READ                      0x30
#define SODASXMLMETHOD_WRITE                     0x40
#define SODASXMLMETHOD_GETPROPERTIES             0x50
#define SODASXMLMETHOD_SUBSCRIBE                 0x60
#define SODASXMLMETHOD_SUBSCRIPTIONPOLLEDREFRESH 0x70
#define SODASXMLMETHOD_SUBSCRIPTIONCANCEL        0x80

#define SODASXMLMETHOD_STATE_GETSTATUS                 0x10
#define SODASXMLMETHOD_STATE_BROWSE                    0x20
#define SODASXMLMETHOD_STATE_READ                      0x30
#define SODASXMLMETHOD_STATE_WRITE                     0x40
#define SODASXMLMETHOD_STATE_GETPROPERTIES             0x50
#define SODASXMLMETHOD_STATE_SUBSCRIBE                 0x60
#define SODASXMLMETHOD_STATE_SUBSCRIPTIONPOLLEDREFRESH 0x70
#define SODASXMLMETHOD_STATE_SUBSCRIPTIONCANCEL        0x80
#define SODASXMLMETHOD_STATE_ITEMLIST                  0xF1
#define SODASXMLMETHOD_STATE_VALUEARRAY                0xF2

class SODaSServer;
class SODaSGroup;

class SODAS_EXPORT SODaSXmlMethod : public SOSrvXmlMethod
{
	friend class SODaSXmlServer;
	friend class SODaSServer;
	friend class SODaSGroup;
public:
	SODaSXmlMethod(void);
	SODaSXmlMethod(IN BYTE xmlMethodType, IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
	virtual void handleCharacterData(LPCSTR s, INT len);

	SOCmnStringList* getMethodOptions(void)
	{
		return &m_methodOptions;
	}
	SOCmnList<SODaSXmlMethodData> *getMethodData(void)
	{
		return &m_methodData;
	}

	BOOL removeOPCXMLDANamespace(OUT IN SOCmnString& szXML);

protected:
	SOCmnStringList m_methodOptions;
	SOCmnList<SODaSXmlMethodData> m_methodData;
	SODaSXmlMethodData* m_pCurMethodData;
	SOCmnXmlValue* m_pCurSimpleXmlValue;
	SOCmnVOIDList m_arrayStack;
	WORD m_charDataType;
	SOCmnStringEx m_methodResults;

	~SODaSXmlMethod(void);
}; // SODaSXmlMethod



#ifdef SOFEATURE_CMN_TRACE
#define SODASXML_TRACE_INPUT(s,m)                               \
    {                                                                       \
        _TRACE_IF(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL)  \
        { \
            traceInput(s,m); \
        } \
    }
#else
#define SODASXML_TRACE_INPUT(s,m)
#endif

#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif
