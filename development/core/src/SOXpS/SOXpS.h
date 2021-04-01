#ifndef _SOXPS_H_
#define _SOXPS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push,1)

//-----------------------------------------------------------------------------
// Defines                                                                    |
//-----------------------------------------------------------------------------

// calling convention
#define SOXPSCFG_CALL __stdcall

// OPC qualities
#define SOXPS_QUALITY_BAD             0x00
#define SOXPS_QUALITY_UNCERTAIN       0x40
#define SOXPS_QUALITY_GOOD            0xC0
#define SOXPS_QUALITY_CONFIG_ERROR    0x04
#define SOXPS_QUALITY_NOT_CONNECTED   0x08
#define SOXPS_QUALITY_DEVICE_FAILURE  0x0c
#define SOXPS_QUALITY_SENSOR_FAILURE  0x10
#define SOXPS_QUALITY_LAST_KNOWN      0x14
#define SOXPS_QUALITY_COMM_FAILURE    0x18
#define SOXPS_QUALITY_OUT_OF_SERVICE  0x1C
#define SOXPS_QUALITY_WAITING_FOR_INITIAL_DATA 0x20
#define SOXPS_QUALITY_LAST_USABLE     0x44
#define SOXPS_QUALITY_SENSOR_CAL      0x50
#define SOXPS_QUALITY_EGU_EXCEEDED    0x54
#define SOXPS_QUALITY_SUB_NORMAL      0x58
#define SOXPS_QUALITY_LOCAL_OVERRIDE  0xD8
#define SOXPS_LIMIT_OK                0x00
#define SOXPS_LIMIT_LOW               0x01
#define SOXPS_LIMIT_HIGH              0x02
#define SOXPS_LIMIT_CONST             0x03

// OPC DA io modes
#define SOXPS_IOMODE_DIRECT_POLL        0x0001
#define SOXPS_IOMODE_QUEUE_POLL         0x0002
#define SOXPS_IOMODE_TRANSACTION_POLL   0x0004
#define SOXPS_IOMODE_DIRECT_REPORT      0x8001
#define SOXPS_IOMODE_QUEUE_REPORT       0x8002
#define SOXPS_IOMODE_TRANSACTION_REPORT 0x8004
#define SOXPS_IOMODE_CONSTANT           0x0005
#define SOXPS_IOMODE_NOIO               0x0007

// OPC AE event types
#define SOXPS_EVENTTYPE_SIMPLE			0x0001
#define SOXPS_EVENTTYPE_TRACKING		0x0002
#define SOXPS_EVENTTYPE_CONDITION		0x0004

// OPC AE condition data mask
#define SOXPS_CONDITIONDATA_SEVERITY	0x0001
#define SOXPS_CONDITIONDATA_MESSAGE		0x0002
#define SOXPS_CONDITIONDATA_ACKREQUIRED	0x0004

// OPC AE acknowledge states
#define SOXPS_ACKNOWLEDGE_NO_CHANGE 0x0000
#define SOXPS_ACKNOWLEDGE_ACK       0x0001
#define SOXPS_ACKNOWLEDGE_NACK      0x0002

// OPC DA access rights
#define SOXPS_ACCESSRIGHT_READABLE    1
#define SOXPS_ACCESSRIGHT_WRITEABLE   2

// OPC DA request operations
#define SOXPS_REQUEST_OPERATION_READ  0
#define SOXPS_REQUEST_OPERATION_WRITE 1

// OPC DA request states
#define SOXPS_REQUEST_STATE_NOT_HANDLED			1
#define SOXPS_REQUEST_STATE_HANDLED 			2
#define SOXPS_REQUEST_STATE_HANDLED_USE_CACHE 	3

// Trace groups
#define SOXPS_TRACE_OPC_CALL	0x00000001
#define SOXPS_TRACE_OPC_ADVISE	0x00000002
#define SOXPS_TRACE_XPRESS		0x10000000
#define SOXPS_TRACE_ALL			0x10000003




//-----------------------------------------------------------------------------
// SOXpSXXXHandle                                                             |
//-----------------------------------------------------------------------------

// object handle
struct _SOXpSItemHandle { int unused; };
typedef struct _SOXpSItemHandle *SOXpSItemHandle;
struct _SOXpSRequestHandle { int unused; };
typedef struct _SOXpSRequestHandle *SOXpSRequestHandle;
struct _SOXpSCategoryHandle { int unused; };
typedef struct _SOXpSCategoryHandle *SOXpSCategoryHandle;
struct _SOXpSConditionHandle { int unused; };
typedef struct _SOXpSConditionHandle *SOXpSConditionHandle;
struct _SOXpSConditionClassHandle { int unused; };
typedef struct _SOXpSConditionClassHandle *SOXpSConditionClassHandle;
struct _SOXpSAreaHandle { int unused; };
typedef struct _SOXpSAreaHandle *SOXpSAreaHandle;
struct _SOXpSSourceHandle { int unused; };
typedef struct _SOXpSSourceHandle *SOXpSSourceHandle;
struct _SOXpSDeviceHandle { int unused; };
typedef struct _SOXpSDeviceHandle *SOXpSDeviceHandle;

#define SOXpSNodeHandle       SOXpSItemHandle
#define SOXpSNodeOrItemHandle SOXpSItemHandle


//-----------------------------------------------------------------------------
// SOXpSInitData                                                              |
//-----------------------------------------------------------------------------

typedef struct _SOXpSInitData
{
	WORD m_xpsInitDataVersion;     // xpress version of application DLL
	CLSID m_clsidDA;               // class id of DA server
	LPCTSTR m_progIdDA;            // prog id of DA server
	LPCTSTR m_verIndProgIdDA;      // version independent prog id of DA server
	LPCTSTR m_descriptionDA;       // description of DA server
	CLSID m_clsidAE;               // class id of AE server
	LPCTSTR m_progIdAE;            // prog id of AE server
	LPCTSTR m_verIndProgIdAE;      // version independent prog id of AE server
	LPCTSTR m_descriptionAE;       // description of AE server
	WORD m_majVersion;             // major version number of the server
	WORD m_minVersion;             // minor version number of the server
	WORD m_buildNumber;            // build number of the server
	LPCTSTR m_vendorInfo;          // version info of the server
	// GUI
	DWORD m_visible;               // show the application window
	LPCTSTR m_appTitle;            // title of application
	UINT m_resIdIcon;              // resource id of Icon
	// Service
	LPCTSTR m_serviceName;         // name of service
	LPCTSTR m_serviceDescription;  // description of service
	LPCTSTR m_serviceDependencies; // dependencies of service
	// OPC Data Access parameters
	DWORD m_minUpdateRateDA;       // minimal update rate for a group (ms)
	BYTE m_provideCacheValueOnRead;// provide the current cache value on for read requests
	// Trace
	LPCTSTR m_traceFile1;          // path of first trace file
	LPCTSTR m_traceFile2;          // path of second trace file
	DWORD m_traceFileSize;         // max size of trace files
	DWORD m_traceInfoLevel;        // trace group in infomation level
	DWORD m_traceDebugLevel;       // trace group in debug level
	// RegistryEntries
	HKEY  m_hKey;				   // registry key Handle, i.e. HKEY_LOCAL_MACHINE
	LPCTSTR m_regKeyParams;		   // registry path to needed values
} SOXpSInitData;



//-----------------------------------------------------------------------------
// SOXpSDARequestData                                                         |
//-----------------------------------------------------------------------------

typedef struct _SOXpSDARequestData
{
	SOXpSItemHandle m_itemHandle;       // tag or property handle
	LPCTSTR m_itemPath;                 // fully qualified path of item
	SOXpSRequestHandle m_requestHandle; // request handle; don't change!
	DWORD m_requestState;               // handled the request?
	HRESULT m_result;                   // result of I/O request
	VARIANT m_value;                    // read value or value to write
	WORD m_quality;                     // quality of the value after the I/O
	FILETIME m_timeStamp;               // time stamp of the value
	DWORD m_itemUserData;               // user data of the item (tag or property)
} SOXpSDARequestData;



//-----------------------------------------------------------------------------
// SOXpSDAItemData                                                            |
//-----------------------------------------------------------------------------

typedef struct _SOXpSDAItemData
{
	SOXpSItemHandle m_itemHandle; // tag or property handle
	LPCTSTR m_itemPath;           // fully qualified path of item
	DWORD m_itemUserData;         // user data of the item (tag or property)
} SOXpSDAItemData;



//-----------------------------------------------------------------------------
// SOXpSAEEventData                                                           |
//-----------------------------------------------------------------------------

typedef struct _SOXpSAEEventData
{
	LPCTSTR m_sourcePath;       // fully qualified path of event source
	FILETIME m_occurenceTime;   // time of event occurance
	LPCTSTR m_message;          // event message
	DWORD m_eventCategory;      // event category id
	DWORD m_severity;           // severity
	LPVARIANT m_eventAttrArray; // array with all event attributes
	LPCTSTR m_actorID;			// actorID for tracking event
	DWORD m_evtType;			// specifies the event type
} SOXpSAEEventData;



//-----------------------------------------------------------------------------
// SOXpSAECondtionData                                                        |
//-----------------------------------------------------------------------------

typedef struct _SOXpSAEConditionData
{
	SOXpSConditionHandle m_condHandle;	// condition handle
	LPCTSTR m_sourcePath;				// fully qualified path of the source
	LPCTSTR m_condName;					// name of the condition classs
	DWORD m_activate;					// activate or deactivate
	LPCTSTR m_actSubCond;				// name of active sub condition
	DWORD m_acknowledge;				// condition acknowlegded
	LPVARIANT m_condAttrArray;			// array with all condition attributes
	WORD m_quality;						// quality of condition
	DWORD m_mask;						// instance specific data supplied
	LPCTSTR m_message;					// message of condition instance
	DWORD m_severity;					// severity of condition instance
	DWORD m_ackRequired;				// ack required of condition instance
	LPCTSTR m_ackID;					// actorID, isn't really used with cond-rel. events
} SOXpSAEConditionData;



//-----------------------------------------------------------------------------
// Server function pointers                                                   |
//-----------------------------------------------------------------------------

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateNode)(IN SOXpSItemHandle parentNodeHandle,
		IN LPCTSTR name, IN VARTYPE datatype, IN DWORD accessRights, IN WORD ioMode,
		IN DWORD userData, OUT SOXpSItemHandle *nodeHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateTag)(IN SOXpSItemHandle parentNodeHandle,
		IN LPCTSTR name, IN VARTYPE datatype, IN DWORD accessRights, IN WORD ioMode,
		IN DWORD userData, OUT SOXpSItemHandle *tagHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateProperty)(IN SOXpSItemHandle parentHandle,
		IN DWORD propertyID, IN LPCTSTR description, IN VARTYPE datatype,
		IN DWORD accessRights, IN WORD ioMode, IN LPVARIANT constantValue, IN DWORD userData,
		OUT SOXpSItemHandle *propertyHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateArea)(IN SOXpSAreaHandle parentAreaHandle,
		IN LPCTSTR name, OUT SOXpSAreaHandle *areaHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateSource)(IN SOXpSAreaHandle parentAreaHandle,
		IN LPCTSTR name, OUT SOXpSSourceHandle *sourceHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateCondition)(IN SOXpSSourceHandle parentSourceHandle,
		IN LPCTSTR conditionClassName, OUT SOXpSConditionHandle *condHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateCategory)(IN DWORD eventType,	IN DWORD categoryID,
		IN LPCTSTR description, OUT SOXpSCategoryHandle *categoryHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateAttribute)(IN SOXpSCategoryHandle categoryHandle,
		IN DWORD attributeID, IN LPCTSTR description, IN VARTYPE datatype);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateConditionClass)(IN SOXpSCategoryHandle categoryHandle,
		IN LPCTSTR conditionClassName, IN LPCTSTR inactiveMessage,
		IN DWORD inactiveAckRequired, IN DWORD singleStateConditionClass,
		IN LPCTSTR definition, IN DWORD activeSeverity, IN LPCTSTR activeMessage,
		IN DWORD activeAckRequired, OUT SOXpSConditionClassHandle *conditionClassHandle);

typedef DWORD (SOXPSCFG_CALL *SOXpSCreateSubConditionClass)(IN SOXpSConditionClassHandle conditionClassHandle,
		IN LPCTSTR name, IN DWORD severity, IN LPCTSTR description,
		IN DWORD ackRequired, IN LPCTSTR definition);

typedef DWORD (SOXPSCFG_CALL *SOXpSCompletedDARequests)(IN DWORD numRequests, IN SOXpSDARequestData *requestArray);

typedef DWORD (SOXPSCFG_CALL *SOXpSFireAEEvents)(IN DWORD numEvents, IN SOXpSAEEventData *eventArray);

typedef DWORD (SOXPSCFG_CALL *SOXpSChangeAEConditions)(IN DWORD numConditions, IN SOXpSAEConditionData *condArray);

typedef DWORD (SOXPSCFG_CALL *SOXpSTranslatedItemID)(IN DWORD attributeID, IN LPCTSTR itemID,
		IN LPCTSTR nodeName, IN CLSID *classID);

typedef DWORD (SOXPSCFG_CALL *SOXpSShutdown)(IN LPCTSTR reason);

typedef DWORD (SOXPSCFG_CALL *SOXpSSetDeviceState)(IN SOXpSDeviceHandle deviceHandle, IN LPCTSTR devState, IN LPCTSTR devStateInfo);


//-----------------------------------------------------------------------------
// Configuration DLL exports                                                  |
//-----------------------------------------------------------------------------

SOXpSInitData * SOXPSCFG_CALL SOXpSInitialize(IN SOXpSCompletedDARequests completedReq,
		IN SOXpSFireAEEvents fireEvents, IN SOXpSChangeAEConditions changeCond, IN SOXpSShutdown shutdown,
		IN SOXpSSetDeviceState setDeviceState);

DWORD SOXPSCFG_CALL SOXpSStart(void);

void SOXPSCFG_CALL SOXpSStop(void);

void SOXPSCFG_CALL SOXpSBuildDANameSpace(IN SOXpSCreateNode createNode,
		IN SOXpSCreateTag createTag, IN SOXpSCreateProperty createProperty);

void SOXPSCFG_CALL SOXpSBuildAEAreaSpace(IN SOXpSCreateArea createArea,
		IN SOXpSCreateSource createSource, IN SOXpSCreateCondition createCondition);

void SOXPSCFG_CALL SOXpSBuildAEEventSpace(IN SOXpSCreateCategory createCategory,
		IN SOXpSCreateAttribute createAttribute, IN SOXpSCreateConditionClass createConditionClass,
		IN SOXpSCreateSubConditionClass createSubConditionClass);

void SOXPSCFG_CALL SOXpSHandleDARequests(IN DWORD ioOperation,
		IN DWORD numRequests, IN OUT SOXpSDARequestData *requestArray);

void SOXPSCFG_CALL SOXpSActivateDAItems(IN BYTE activate, IN DWORD numItems, IN SOXpSDAItemData *itemDataArray);

void SOXPSCFG_CALL SOXpSActivateAESubscription(IN BYTE activate);

DWORD SOXPSCFG_CALL SOXpSReceivedAEAcknowledge(IN SOXpSConditionHandle condHandle,IN LPCTSTR sourcePath,
		IN LPCTSTR condName, IN LPCTSTR ackID, IN LPCTSTR ackComment, IN FILETIME *activeTime);

void SOXPSCFG_CALL SOXpSTranslateToItemIDs(IN SOXpSConditionHandle condHandle, IN LPCTSTR sourcePath,
		IN LPCTSTR condName, IN LPCTSTR subCondName, IN DWORD numAttributes, IN DWORD *attributesArray,
		IN SOXpSTranslatedItemID translatedItemID);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
