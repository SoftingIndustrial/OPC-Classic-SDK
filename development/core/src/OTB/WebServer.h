#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#ifdef OTB_XMLDA_SERVER

#pragma pack(push, 4)

#include "SOSrvHttpEx.h"
#include "SrvApp.h"

class CWebServer : public SOSrvWeb
{
public:
	CWebServer(void);

	virtual SOSrvWebExpandData* createExpandData(void);

	virtual BOOL handleTemplate(LPCTSTR templ, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result);
	virtual SOSrvWebTrace* getWebTrace(void);

	virtual WORD checkAccess(IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, IN LPCTSTR filePath, OUT SOCmnString* httpAttributes);

	virtual LPCTSTR getTraceLevelDescription(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr);
	static LPCTSTR getTraceLevelDescriptionS(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr);
	virtual DWORD getTraceLevelFromDescription(IN LPCTSTR descr);
	static DWORD getTraceLevelFromDescriptionS(IN LPCTSTR descr);

	void getClientConnections(LPCTSTR type, LPCTSTR language, SOCmnStringEx& result);

	virtual BOOL handleLOGIN(LPCTSTR command, IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);
};

extern SOCmnPointer<CWebServer> g_webServer;

class CWebServerExpandData : public SOSrvWebExpandData
{
public:
	CWebServerExpandData(SOSrvWeb* pServer);

	virtual void init(IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, IN SOCmnString* httpAttributes);

	virtual void getQualityFormaterDefault(LPCTSTR formater, SOCmnStringEx& result);
	virtual void getQualityFormater(WORD quality, LPCTSTR formater, SOCmnStringEx& result);
	virtual void getQualityString(WORD quality, SOCmnStringEx& result);
};

#pragma pack(pop)

#endif // OTB_XMLDA_SERVER

#endif
