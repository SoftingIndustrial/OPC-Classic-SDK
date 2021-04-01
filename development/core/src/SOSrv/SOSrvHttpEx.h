#ifndef _SOSRVHTTPEX_H_
#define _SOSRVHTTPEX_H_

#ifdef SOFEATURE_SOAP

#pragma pack(push, 4)


class SOSrvHTTPRequest : public SOCmnObject
{
public:
	SOSrvHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);

	SOCmnPointer<SOSrvHTTPHandler> m_httpHandlerForResponse; //use this pointer ONLY to send a response, its data members may be changed by receiver work thread.
	SOCmnPointer<SOSrvHTTPHandlerData> m_httpData;
	DWORD m_lenBody;
	BYTE* m_pBody;
	BYTE m_flags;

	WORD getHTTPVersion(void)
	{
		return m_httpVersion;
	}
	WORD getHTTPMethod(void)
	{
		return m_httpMethod;
	}
	LPCTSTR getURL(void)
	{
		return m_httpURL;
	}
	SOCmnStringList* getAttributeList(void)
	{
		return &m_attributeList;
	}
	BYTE getFlags(void)
	{
		return m_flags;
	}

protected:
	SOCmnString m_httpURL;
	WORD m_httpMethod;
	WORD m_httpVersion;
	SOCmnStringList m_attributeList;

	~SOSrvHTTPRequest(void);
};

class SOSrvWebExpandData;
class SOSrvWebTrace;

class SOSrvWeb : public SOSrvHTTPReceiver
{
public:
	SOSrvWeb(void);

	virtual BOOL setRootDirectories(IN LPCTSTR root1, IN LPCTSTR root2);

	virtual WORD handleHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);

	SODaSServer* getServer(void)
	{
		m_server.addRef();
		return m_server;
	}

	LCID getLCID(void)
	{
		return m_lcid;
	}

	static void forkCGIProcesses(void);

protected:
	SOCmnPointer<SODaSServer> m_server;
	SOCmnString m_rootDirectory1;
	SOCmnString m_rootDirectory2;

	SOCmnString m_rootFile;

	SOCmnString m_httpServerDescription;
	LCID m_lcid;

	SOCmnPointer<SODaSGroup> m_group;

	DWORD m_workEventId;
	SOCmnThread m_workThread;               // working thread of web server

	SOCmnList<SOSrvHTTPRequest> m_httpReqList;

	SODaSGroup* getGroup();

	virtual ~SOSrvWeb(void);

	virtual WORD get(IN SOSrvHTTPRequest* pReq);
	virtual WORD post(IN SOSrvHTTPRequest* pReq);

	virtual WORD getFile(IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, IN LPCTSTR absFilePath, OUT DWORD* length, OUT BYTE** ppData, OUT SOCmnString* type, OUT SOCmnString* filePath, OUT SOCmnString* httpAttributes);
	virtual WORD checkAccess(IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, IN LPCTSTR filePath, OUT SOCmnString* httpAttributes);

	virtual DWORD expandTemplates(SOCmnStringEx& data, SOSrvWebExpandData* pExData);
	virtual BOOL expandTemplate(SOCmnStringEx& templ, SOSrvWebExpandData* pExData, SOCmnStringEx& result);

	virtual void handleItemListValuesCommand(LPCTSTR command, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);
	virtual void handleSetPassword(LPCTSTR command, LPCTSTR user, LPCTSTR pwd, LPCTSTR pwdO, SOSrvWebExpandData* pExData);

	virtual BOOL queueHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody);

	static DWORD _stdcall workThreadMain(void* context);
	DWORD workThread(void);

	virtual void handleTRACETemplate(LPCTSTR cmd, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result);
	virtual LPCTSTR getTraceLevelDescription(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr);
	static LPCTSTR getTraceLevelDescriptionS(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr);
	virtual DWORD getTraceLevelFromDescription(IN LPCTSTR descr);
	static DWORD getTraceLevelFromDescriptionS(IN LPCTSTR descr);

	virtual void handleITEMLISTVALUESTemplate(SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result);

	virtual BOOL handlePOSTCommand(LPCTSTR command, IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, SOCmnStringList* pArgs, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);
	virtual BOOL handleLOGIN(LPCTSTR command, IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);

	virtual SOSrvWebExpandData* createExpandData(void);

	virtual void watchProcess(DWORD pid);
	virtual BOOL isProcessRunning(DWORD pid);

	virtual SOSrvWebTrace* getWebTrace(void);

	// extension methods
	virtual BOOL handleTemplate(LPCTSTR templ, SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result);
	virtual BOOL handleCommand(LPCTSTR command, SOCmnStringList* pArgs, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);
	virtual BOOL checkUser(LPCTSTR user, LPCTSTR password, LPCTSTR command, SOCmnStringList* pArgs, SOSrvWebExpandData* pExData, SOCmnStringEx& result, OUT SOCmnString* type);

	virtual void changedConfiguration(LPCTSTR command);

};

class SOSrvWebExpandData : public SOCmnObject
{
	friend class SOSrvWeb;

public:
	SOSrvWebExpandData(SOSrvWeb* pServer);

	virtual void init(IN SOSrvHTTPRequest* pReq, IN SOSrvHTTPHandlerData* pHTTPData, IN SOCmnString* httpAttributes);

	virtual void addReadRequest(IN LPCTSTR itemID);
	virtual void getReadValue(IN LPCTSTR itemID, IN SOCmnStringList* pArgs, OUT SOCmnStringEx& result);
	virtual void getReadQuality(IN LPCTSTR itemID, IN SOCmnStringList* pArgs, OUT SOCmnStringEx& result);
	virtual void execute(void);

	virtual void addWriteRequests(IN LPCTSTR command, SOCmnStringList* pArgsList);

	SOCmnStringList getReadItemList(void)
	{
		return &m_readItemList;
	}
	SOCmnStringList getWriteItemList(void)
	{
		return &m_writeItemList;
	}

	LPCTSTR getMessage(void)
	{
		return m_message;
	}
	void setMessage(LPCTSTR msg)
	{
		m_message = msg;
	}

	SOCmnStringEx m_httpResponse;

protected:
	SOCmnPointer<SOSrvWeb> m_webServer;
	SOCmnPointer<SODaSServer> m_server;
	SOCmnPointer<SODaSTransaction> m_readTransaction;
	SOCmnPointer<SODaSTransaction> m_writeTransaction;
	BYTE m_pass;
	DWORD m_readCnt;
	SOCmnVariant** m_ppReadValues;
	WORD* m_pReadQualities;
	SOCmnDateTime** m_ppReadTimeStamps;
	HRESULT* m_pReadErrors;
	BOOL m_executeRead;
	SOCmnString m_message;
	SOCmnStringList m_postArgsList;

	SOCmnStringEx m_lastReadItemID;
	SOCmnStringList m_readItemList;

	DWORD m_writeCnt;
	SOCmnVariant** m_ppWriteValues;
	HRESULT* m_pWriteErrors;
	BOOL m_executeWrite;

	SOCmnStringList m_writeItemList;

	virtual void getQualityFormaterDefault(LPCTSTR /* formater */, SOCmnStringEx& /* result */) {}
	virtual void getQualityFormater(WORD /* quality */, LPCTSTR /* formater */, SOCmnStringEx& /* result */) {}
	virtual void getQualityString(WORD quality, SOCmnStringEx& result);

	virtual ~SOSrvWebExpandData(void);
};

#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif

