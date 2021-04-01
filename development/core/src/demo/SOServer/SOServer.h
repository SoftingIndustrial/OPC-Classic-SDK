#ifndef _SOSERVER_H_
#define _SOSERVER_H_

#ifdef SOFEATURE_DCOM
//-----------------------------------------------------------------------------
// SODmSEventHandler                                                          |
//-----------------------------------------------------------------------------

class SODmSEventHandler : public SOSrvEventHandler
{
public:
	SODmSEventHandler(IN BOOL startedByCom, IN HWND wnd);

protected:
	BOOL m_startedByCom;    // application started by COM
	HWND m_wnd;             // main window

	// released last COM object of the server
	virtual void onReleasedLastOPCObject(void);
}; // SODmSEventHandler
#endif



//-----------------------------------------------------------------------------
// SODmSTraceHandler
//-----------------------------------------------------------------------------

class SODmSTraceHandler : public SOCmnTraceHandler
{
public:
	virtual HRESULT saveTraceOptions(void);
	void loadTraceOptions(void);
private:
	SOCmnString m_traceFileName;
}; // SODmSTraceHandler


#ifdef SODMS_DA

//-----------------------------------------------------------------------------
// SODmSCreatorDA                                                             |
//-----------------------------------------------------------------------------

class SODmSCreatorDA : public SODaSCreator
{
public:
	virtual SODaSServer* createServer(IN BOOL publicGroups, IN BYTE serverInstance);
	virtual SODaSGroup* createGroup(IN SODaSServer* parent);
	virtual SODaSTag* createTag(IN LPCTSTR qualItemID);
	virtual SODaSTransaction* createTransaction(IN SODaSGroup* group, IN BYTE transactionType,
												IN BYTE connectionType, IN DWORD connection);
	virtual SODaSCache* createCache(IN SODaSItemTag* tag);
}; // SODmSCreatorDA


//-----------------------------------------------------------------------------
// SODmSServerDA                                                              |
//-----------------------------------------------------------------------------

class SODmSServerDA : public SODaSServer
{
public:
	SODmSServerDA(IN BOOL publicGroups, IN BYTE serverInstance);
	~SODmSServerDA();
	static void initCache(SODaSItemTag* pItemTag, DWORD arraySize);

	virtual BOOL getVendorXMLNamespace(IN WORD forWhat, OUT SOCmnString& szVendorNS);

	void getClientSpecificString(OUT SOCmnString& cs)
	{
		cs = m_clientSpecificString;
	}
	void setClientSpecificString(IN SOCmnString& cs)
	{
		m_clientSpecificString = cs;
	}
	void getClientSpecificTimeStamp(OUT SOCmnDateTime& ts)
	{
		ts = m_clientSpecificTimeStamp;
	}
	void setClientSpecificTimeStamp(IN SOCmnDateTime& ts)
	{
		m_clientSpecificTimeStamp = ts;
	}

	// security
	virtual BOOL isPrivateSecurityAvailable(void);
	virtual BOOL logon(LPCTSTR userName, LPCTSTR password);
	virtual BOOL logoff(void);
#ifdef SOFEATURE_DCOM
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);
#endif
#ifdef SOFEATURE_SOAP
	virtual BOOL checkAuthorization(IN SODaSXmlMethod* pMth);
#endif
	SHORT getAccessLevel(void)
	{
		return m_accessLevel;
	}

protected:
	SOCmnString m_clientSpecificString;
	SOCmnDateTime m_clientSpecificTimeStamp;
	SHORT m_accessLevel;

	virtual BOOL isNodeName(IN LPCTSTR fullName);
	virtual BOOL isTagName(IN LPCTSTR fullName);
	virtual void initTag(IN SODaSTag* tag);

	virtual DWORD getPropertyIDByName(IN LPCTSTR propName);

#ifdef SOFEATURE_DCOM
	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
#endif
}; // SODmSServerDA



//-----------------------------------------------------------------------------
// SODmSTagAccept                                                             |
//-----------------------------------------------------------------------------

class SODmSTagAccept :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	SODmSTagAccept(void);
	SODmSTagAccept(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);

	// write quality and timestamp
	virtual BOOL supportWriteQT(IN BYTE flagsQT);
}; // SODmSTagAccept



//-----------------------------------------------------------------------------
// SODmSTagTime                                                               |
//-----------------------------------------------------------------------------

class SODmSTagTime :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	enum tagType
	{
		second,
		minute,
		hour,
		string,
		array,
		date,
		limitSecond
	};

	enum tagTimeZone
	{
		local,
		GMT,
		none
	};

	SODmSTagTime(IN LPCTSTR tagName, IN enum tagType type, IN enum tagTimeZone timeZone);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);

protected:
	enum tagType m_tagType;
	enum tagTimeZone m_tagTimeZone;

	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SODmSTagTime



//-----------------------------------------------------------------------------
// SODmSTagInc                                                                |
//-----------------------------------------------------------------------------

class SODmSTagInc :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	SODmSTagInc(IN LPCTSTR tagName, IN VARTYPE datatype);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
	virtual BOOL handlePropertyRequest(IN SODaSProperty* prop, IN SODaSRequest* req);

protected:
	SOCmnPointer<SODaSCache> m_by;

	void doIncrement(SOCmnVariant& value, SOCmnVariant& by);
	void incrementWString(IN BSTR w, IN BOOL doinc);
	void incrementDate(IN DATE* date, IN BOOL doinc);
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SODmSTagInc



//-----------------------------------------------------------------------------
// SODmSNodeInc                                                             |
//-----------------------------------------------------------------------------

class SODmSNodeInc : public SODaSNodeDefault
{
public:
	SODmSNodeInc(IN LPCTSTR nodeName);

	virtual BOOL handlePropertyRequest(IN SODaSProperty* prop,
									   IN SODaSRequest* request);
}; // SODmSNodeInc



//-----------------------------------------------------------------------------
// SODmSTagMaths                                                              |
//-----------------------------------------------------------------------------

class SODmSTagMaths :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	enum tagType
	{
		sin,
		cos,
		tan,
	};


	SODmSTagMaths(IN LPCTSTR tagName, IN enum tagType type);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
	virtual BOOL handlePropertyRequest(IN SODaSProperty* prop, IN SODaSRequest* req);

	void incAngle(void);

protected:
	SOCmnPointer<SODaSCache> m_angle;
	enum tagType m_tagType;

	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SODmSTagMaths



//-----------------------------------------------------------------------------
// SODmSNodeWatch                                                             |
//-----------------------------------------------------------------------------

class SODmSNodeWatch : public SODaSNodeDefault
{
public:
	SODmSNodeWatch(IN LPCTSTR nodeName, IN LPCTSTR devType, IN DWORD devAddress, IN LPCTSTR devState, IN LPCTSTR devStateInfo);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif // SOFEATURE_WATCH

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

	void setDeviceState(IN LPCTSTR devState);
	void setDeviceStateInfo(IN LPCTSTR devStateInfo);

protected:
	SOCmnString m_devType;
	DWORD m_devAddress;
	SOCmnString m_devState;
	SOCmnString m_devStateInfo;
}; // SODmSNodeWatch



//-----------------------------------------------------------------------------
// SODmSTagWatch                                                              |
//-----------------------------------------------------------------------------

class SODmSTagWatch :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	SODmSTagWatch(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights, IN WORD offset);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// device state and state info for watch
	virtual SOCmnString getDeviceState(void);
	virtual SOCmnString getDeviceStateInfo(void);

protected:
	WORD m_offset;
}; // SODmSTagWatch


//-----------------------------------------------------------------------------
// SODmSTagError                                                              |
//-----------------------------------------------------------------------------

class SODmSTagError :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	enum tagType
	{
		read,
		write,
		quality,
	};
	SODmSTagError(IN LPCTSTR tagName, IN enum tagType type);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);

	enum tagType m_tagType;
}; // SODmSTagError


//-----------------------------------------------------------------------------
// SODmSTagMode                                                               |
//-----------------------------------------------------------------------------

class SODmSTagMode :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	SODmSTagMode(IN BYTE updateMode, IN BYTE ioMode, IN LPCTSTR tagName, IN VARTYPE vartype);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);

	// request queues
	virtual SOCmnList<SODaSRequest> *getReadQueue(void);
	virtual SOCmnList<SODaSRequest> *getWriteQueue(void);
	virtual SOCmnEventPointer getReadEvent(void);
	virtual SOCmnEventPointer getWriteEvent(void);

protected:
	SOCmnDateTime m_lastTime;
}; // SODmSTagMode


//-----------------------------------------------------------------------------
// SODmSNodeDynamic                                                             |
//-----------------------------------------------------------------------------

class SODmSNodeDynamic : public SODaSNodeDefault
{
public:
	SODmSNodeDynamic(IN LPCTSTR nodeName);
}; // SODmSNodeDynamic


//-----------------------------------------------------------------------------
// SODmSTagHidden                                                             |
//-----------------------------------------------------------------------------

class SODmSTagHidden : public SODmSTagAccept
{
public:
	SODmSTagHidden(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights);

	// is visible during browsing
	virtual BOOL isBrowsable(IN SOCmnObject* pServer);
}; // SODmSTagHidden


//-----------------------------------------------------------------------------
// SODmSNodeHidden                                                            |
//-----------------------------------------------------------------------------

class SODmSNodeHidden : public SODaSNodeDefault
{
public:
	SODmSNodeHidden(IN LPCTSTR nodeName);

	// is visible during browsing
	virtual BOOL isBrowsable(IN SOCmnObject* pServer);
}; // SODmSNodeHidden


//-----------------------------------------------------------------------------
// SODmSTagClient                                                             |
//-----------------------------------------------------------------------------

class SODmSTagClient :
	public SODaSTag,
	virtual public SOCmnElementListAdditional
{
public:
	SODmSTagClient(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
}; // SODmSTagClient


//-----------------------------------------------------------------------------
// SODmSTagSecure                                                             |
//-----------------------------------------------------------------------------

class SODmSTagSecure : public SODmSTagAccept
{
public:
	SODmSTagSecure(IN LPCTSTR tagName, IN VARTYPE datatype, IN DWORD accessRights);

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
}; // SODmSTagSecure


//-----------------------------------------------------------------------------
// SODmSCache                                                                 |
//-----------------------------------------------------------------------------

class SODmSCache :
	public SODaSCache
{
public:
	SODmSCache(void);

	virtual WORD getQuality(IN SODaSRequest* req);
	virtual BOOL getTimeStamp(IN SODaSRequest* req, OUT SOCmnDateTime& timeStamp);
protected:
	~SODmSCache(void);
	virtual BOOL getValue(IN SODaSRequest* req, IN VARTYPE typeWanted, IN LCID localeID, OUT SOCmnVariant& value, OPTIONAL OUT HRESULT* pConvRes = NULL);
};

//-----------------------------------------------------------------------------
// SODmSTransaction                                                           |
//-----------------------------------------------------------------------------

class SODmSTransaction :
	public SODaSTransaction
{
public:
	SODmSTransaction(IN SODaSGroup* group, IN BYTE type,
					 IN BYTE connectionType, IN OPTIONAL DWORD connection = 0);

	// actions on request list
	virtual void handleRequests(IN SOCmnList<SODaSRequest> *reqList);
}; // SODmSTransaction

class SODmSGroup : public SODaSGroupDefault
{
protected:
#ifdef SOFEATURE_DCOM
	virtual HRESULT queryInterface(IN REFIID iid, IN void** object);
#endif
};

#endif

#ifdef SODMS_AE

//-----------------------------------------------------------------------------
// SODmSCreatorAE                                                             |
//-----------------------------------------------------------------------------

class SODmSCreatorAE : public SOAeSCreator
{
public:
	virtual SOAeSServer* createServer(IN BYTE serverInstance);
}; // SODmSCreatorAE


//-----------------------------------------------------------------------------
// SODmSServerAE                                                              |
//-----------------------------------------------------------------------------

class SODmSServerAE : public SOAeSServer
{
public:
	SODmSServerAE(IN BYTE serverInstance);

	// security
	virtual BOOL isPrivateSecurityAvailable(void);
	virtual BOOL logon(LPCTSTR userName, LPCTSTR password);
	virtual BOOL logoff(void);
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);

	SHORT getAccessLevel(void)
	{
		return m_accessLevel;
	}

protected:
	SHORT m_accessLevel;
}; // SODmSServerAE

#endif

#ifdef SODMS_ALL

//-----------------------------------------------------------------------------
// SODmSConditionTimeSlot                                                     |
//-----------------------------------------------------------------------------

class SODmSConditionTimeSlot :
	public SOAeSCondition,
	virtual public SOCmnElementListAdditional,
	virtual public SOAeSConditionAttrFull
{
public:

	// update attributes and check conditions (called from survey thread of entry)
	virtual void checkCondition(void);

protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
	virtual SOCmnObject* getObjectAeSConditionIAttr(void);
}; // SODmSConditionTimeSlot

#endif

#endif
