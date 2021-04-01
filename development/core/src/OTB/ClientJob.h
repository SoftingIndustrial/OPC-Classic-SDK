#ifndef _JOB_H_
#define _JOB_H_

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ClientJob                                                                  |
//-----------------------------------------------------------------------------

#define OTC_EXECUTIONTYPE_ASYCHRONOUS_EXECUTE 0x80

#define CLIENTJOB_TYPE_UNKNOWN                 0
#define CLIENTJOB_TYPE_PERFORMSTATETRANSITION  1
#define CLIENTJOB_TYPE_UPDATEATTRIBUTES        2
#define CLIENTJOB_TYPE_UPDATEITEMATTRIBUTES    3
#define CLIENTJOB_TYPE_GETSERVERSTATUS         4
#define CLIENTJOB_TYPE_READ                    5
#define CLIENTJOB_TYPE_WRITE                   6
#define CLIENTJOB_TYPE_VALIDATEITEMS           7
#define CLIENTJOB_TYPE_BROWSESERVER            8
#define CLIENTJOB_TYPE_BROWSEADDRESSSPACE      9
#define CLIENTJOB_TYPE_GETPROPERTIES           10
#define CLIENTJOB_TYPE_REFRESHCONDITIONS       11
#define CLIENTJOB_TYPE_ACKNOWLEDGECONDITIONS   12
#define CLIENTJOB_TYPE_QUERYAVAILABLEFILTERS   13
#define CLIENTJOB_TYPE_QUERYCATEGORIES         14
#define CLIENTJOB_TYPE_QUERYATTRIBUTES         15
#define CLIENTJOB_TYPE_QUERYCONDITIONS         16
#define CLIENTJOB_TYPE_QUERYSUBCONDITIONS      17
#define CLIENTJOB_TYPE_QUERYSOURCECONDITIONS   18
#define CLIENTJOB_TYPE_GETCONDITIONSTATE       19
#define CLIENTJOB_TYPE_ENABLECONDITIONS        20
#define CLIENTJOB_TYPE_GETERRORSTRING          21
#define CLIENTJOB_TYPE_LOGON                   22
#define CLIENTJOB_TYPE_LOGOFF                  23

class ClientJob : public SOCltTask
{
public:
	ClientJob(IN SOCltElement* pElement, IN OTCExecutionOptions* pExecutionOptions);
	virtual void execute();

	void performStateTransition(IN BOOL deep);
	void updateAttributes(BYTE fromServer, DWORD attributeCount, DWORD* pWhatAttributes);
	void updateItemAttributes(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles, BYTE fromServer, DWORD attributeCount, DWORD* pWhatAttributes);
	void getServerStatus(void)
	{
		m_jobType = CLIENTJOB_TYPE_GETSERVERSTATUS;
	}
	void read(IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPCTSTR* pItemIDs, IN LPCTSTR* pItemPaths, IN DWORD maxAge);
	void write(IN DWORD count, IN OTCObjectHandle* pItemHandles, IN LPCTSTR* pItemIDs, IN LPCTSTR* pItemPaths, IN OTCValueData* pValues);
	void validateItems(IN DWORD itemCount, IN OTCObjectHandle* pItemHandles);
	void browseServer(IN LPCTSTR ipAddress, IN BYTE whatOPCSpec, IN BYTE whatServerData);
	void browseAddressSpace(IN OTCObjectHandle addressSpaceElementHandle, IN LPCTSTR addressSpaceElementID, IN LPCTSTR addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions);
	void getProperties(IN OTCObjectHandle addressSpaceElementHandle, IN LPCTSTR addressSpaceElementID, IN LPCTSTR addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions);
	void refreshConditions(IN BYTE cancelRefresh);
	void acknowledgeConditions(IN LPTSTR ackID, IN LPTSTR ackComment, IN DWORD count, IN OTCEventData* pEvents);
	void queryAvailableFilters(void);
	void queryCategories(void);
	void queryAttributes(IN DWORD categoryId);
	void queryConditions(IN DWORD categoryId);
	void querySubConditions(IN LPTSTR conditionName);
	void querySourceConditions(IN LPTSTR sourcePath);
	void getConditionState(IN LPTSTR sourcePath, IN LPTSTR conditionName, IN DWORD attributeCount, IN DWORD* pAttributeIds);
	void enableConditions(IN BYTE enable, IN BYTE areaOrSource, IN LPTSTR path);
	void getErrorString(IN LONG errorCode);
	void logon(IN LPTSTR userName, IN LPTSTR password);
	void logoff(void);

protected:
	virtual ~ClientJob(void);

	OTCExecutionOptions m_executionOptions;
	BYTE m_jobType;
	BYTE m_deep;               // performStateTransition
	BYTE m_fromServer;         // updateAttributes + updateItemAttributes
	DWORD m_attributeCount;    // updateAttributes + updateItemAttributes + getConditionState
	DWORD* m_pWhatAttributes;  // updateAttributes + updateItemAttributes
#ifdef OTB_CLIENT_DA
	DWORD m_itemCount;         // updateItemAttributes + read + write + validateItems
	SODaCItem** m_ppItems;     // updateItemAttributes + read + write + validateItems
	LPTSTR* m_pItemIDs;        // read + write
	LPTSTR* m_pItemPaths;      // read + write
	DWORD m_maxAge;            // read
	OTCValueData* m_pValues;   // write
#endif // OTB_CLIENT_DA
	SOCmnString m_ipAddress;   // browseServer
	BYTE m_whatOPCSpec;        // browseServer
	BYTE m_whatServerData;     // browseServer
	OTCObjectHandle m_addressSpaceElementHandle;        // browseAddressSpace + getProperties
	SOCmnString m_addressSpaceElementID;                // browseAddressSpace + getProperties
	SOCmnString m_addressSpaceElementPath;              // browseAddressSpace + getProperties
	OTCAddressSpaceBrowseOptions* m_pBrowseOptions;     // browseAddressSpace
#ifdef OTB_CLIENT_DA
	OTCGetDAPropertiesOptions* m_pGetPropertiesOptions; // getProperties
#endif // OTB_CLIENT_DA
#ifdef OTB_CLIENT_AE
	BYTE m_cancelRefresh;       // refreshConditions
	SOCmnString m_ackId;        // acknowledgeConditions
	SOCmnString m_ackComment;     // acknowledgeConditions
	DWORD m_eventCount;         // acknowledgeConditions
	OTCEventData* m_pEventData; // acknowledgeConditions
	DWORD m_categoryId;         // queryAEAttributes + queryAEConditions
	SOCmnString m_conditionName;// queryAESubConditions + getConditionState
	SOCmnString m_sourcePath;   // querySourceConditions + getConditionState + enableConditions
	DWORD* m_pAttributeIds;     // getConditionState
	BYTE m_enable;              // enableConditions
	BYTE m_areaOrSource;        // enableConditions
#endif // OTB_CLIENT_AE
	LONG m_errorCode;           // getErrorString
	SOCmnString m_userName;     // logon
	SOCmnString m_password;     // logon
};  // ClientJob

#pragma pack(pop)

#endif // _SOCLTTASK_H_
