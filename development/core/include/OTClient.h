#ifndef _OTCLIENT_H_
#define _OTCLIENT_H_

#include "OTcommon.h"

#ifdef __cplusplus
extern "C"
{

#endif
#pragma pack(push, 4)

//-----------------------------------------------------------------------------
// Defines                                                                    |
//-----------------------------------------------------------------------------

#define OTC_FEATURE_DA_CLIENT           0x10
#define OTC_FEATURE_XMLDA_CLIENT        0x20
#define OTC_FEATURE_AE_CLIENT           0x40
#define OTC_FEATURE_TP_CLIENT           0x80

#define OTC_STATE_DISCONNECTED 0x00
#define OTC_STATE_CONNECTED    0x01
#define OTC_STATE_ACTIVE       0x03

#define OTC_EXECUTIONTYPE_SYCHRONOUS  0x01
#define OTC_EXECUTIONTYPE_ASYCHRONOUS 0x02

#define OTC_SERVERDATA_CLSID                     0x01
#define OTC_SERVERDATA_PROGID                    0x02
#define OTC_SERVERDATA_VERSIONINDEPENDENTPROGID  0x04
#define OTC_SERVERDATA_DESCRIPTION               0x08
#define OTC_SERVERDATA_URL                       0x10
#define OTC_SERVERDATA_OPCSPECIFICATIONS         0x20
#define OTC_SERVERDATA_ALL                       0xFF

#define OTC_DAPROPERTYDATA_NONE    0x00
#define OTC_DAPROPERTYDATA_VALUE   0x01
#define OTC_DAPROPERTYDATA_ITEMID  0x02
#define OTC_DAPROPERTYDATA_ALL     0xFF

#define OTC_DAITEM_EUTYPE_NO         0x00
#define OTC_DAITEM_EUTYPE_ANALOG     0x01
#define OTC_DAITEM_EUTYPE_ENUMERATED 0x02

#define OTC_DASUBSCRIPTION_DATARETRIEVAL_ASYNCADVISE   0x01
#define OTC_DASUBSCRIPTION_DATARETRIEVAL_SYNCREAD      0x02
#define OTC_DASUBSCRIPTION_DATARETRIEVAL_POLLEDREFRESH 0x04

#define OTC_OBJECTTYPE_DASESSION                    0x80000000
#define OTC_OBJECTTYPE_DASUBSCRIPTION               0x40000000
#define OTC_OBJECTTYPE_DAITEM                       0x20000000
#define OTC_OBJECTTYPE_AESESSION                    0x08000000
#define OTC_OBJECTTYPE_AESUBSCRIPTION               0x04000000

#define OTC_COM_CLSCTX_NOT_COM       0x00000000
#define OTC_COM_CLSCTX_INPROC_SERVER 0x00000001
#define OTC_COM_CLSCTX_LOCAL_SERVER  0x00000004
#define OTC_COM_CLSCTX_REMOTE_SERVER 0x00000010
#define OTC_COM_CLSCTX_SERVER        (OTC_COM_CLSCTX_INPROC_SERVER | OTC_COM_CLSCTX_LOCAL_SERVER | OTC_COM_CLSCTX_REMOTE_SERVER)

#define OTC_ATTRIBUTE_DASESSION_ALL                (OTC_OBJECTTYPE_DASESSION | 0xFFFF)
#define OTC_ATTRIBUTE_DASESSION_CLIENTNAME         (OTC_OBJECTTYPE_DASESSION | 0x0001)
#define OTC_ATTRIBUTE_DASESSION_LCID               (OTC_OBJECTTYPE_DASESSION | 0x0002)
#define OTC_ATTRIBUTE_DASESSION_URL                (OTC_OBJECTTYPE_DASESSION | 0x0004)
#define OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION    (OTC_OBJECTTYPE_DASESSION | 0x0008)
#define OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION (OTC_OBJECTTYPE_DASESSION | 0x0010)
#define OTC_ATTRIBUTE_DASESSION_REQUESTTIMEOUT     (OTC_OBJECTTYPE_DASESSION | 0x0020)
#define OTC_ATTRIBUTE_DASESSION_PERFORMINITIALREAD (OTC_OBJECTTYPE_DASESSION | 0x0040)
#define OTC_ATTRIBUTE_DASESSION_COM_CLSCTX         (OTC_OBJECTTYPE_DASESSION | 0x8000)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_ALL           (OTC_OBJECTTYPE_DASUBSCRIPTION | 0xFFFF)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_NAME          (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0001)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE    (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0002)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND      (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0004)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS      (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0008)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_LCID          (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0010)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0020)
#define OTC_ATTRIBUTE_DASUBSCRIPTION_DATARETRIEVAL (OTC_OBJECTTYPE_DASUBSCRIPTION | 0x0040)
#define OTC_ATTRIBUTE_DAITEM_ALL                   (OTC_OBJECTTYPE_DAITEM | 0xFFFF)
#define OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE    (OTC_OBJECTTYPE_DAITEM | 0x0001)
#define OTC_ATTRIBUTE_DAITEM_DEADBAND              (OTC_OBJECTTYPE_DAITEM | 0x0002)
#define OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE       (OTC_OBJECTTYPE_DAITEM | 0x0004)
#define OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS          (OTC_OBJECTTYPE_DAITEM | 0x0008)
#define OTC_ATTRIBUTE_DAITEM_EUTYPE                (OTC_OBJECTTYPE_DAITEM | 0x0010)
#define OTC_ATTRIBUTE_DAITEM_EUINFO                (OTC_OBJECTTYPE_DAITEM | 0x0020)
#define OTC_ATTRIBUTE_DAITEM_ID                    (OTC_OBJECTTYPE_DAITEM | 0x0040)
#define OTC_ATTRIBUTE_DAITEM_PATH                  (OTC_OBJECTTYPE_DAITEM | 0x0080)

#define OTC_ATTRIBUTE_AESESSION_ALL                (OTC_OBJECTTYPE_AESESSION | 0xFFFF)
#define OTC_ATTRIBUTE_AESESSION_CLIENTNAME         (OTC_OBJECTTYPE_AESESSION | 0x0001)
#define OTC_ATTRIBUTE_AESESSION_LCID               (OTC_OBJECTTYPE_AESESSION | 0x0002)
#define OTC_ATTRIBUTE_AESESSION_URL                (OTC_OBJECTTYPE_AESESSION | 0x0004)
#define OTC_ATTRIBUTE_AESESSION_COM_CLSCTX         (OTC_OBJECTTYPE_AESESSION | 0x8000)
#define OTC_ATTRIBUTE_AESUBSCRIPTION_ALL           (OTC_OBJECTTYPE_AESUBSCRIPTION | 0xFFFF)
#define OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME    (OTC_OBJECTTYPE_AESUBSCRIPTION | 0x0001)
#define OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE       (OTC_OBJECTTYPE_AESUBSCRIPTION | 0x0002)
#define OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER        (OTC_OBJECTTYPE_AESUBSCRIPTION | 0x0004)
#define OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES (OTC_OBJECTTYPE_AESUBSCRIPTION | 0x0008)


//-----------------------------------------------------------------------------
// Structures                                                                 |
//-----------------------------------------------------------------------------

#define OTCAPI_CALL OTAPI_CALL
#define OTCObjectHandle OTObjectHandle
#define OTCVariant OTVariant
#define OTCDateTime OTDateTime
#define OTCObjectData OTObjectData
#define OTCValueData OTValueData

	typedef struct _OTCInitData
	{
		// server identity
		OTChar* m_serviceName;         // name of the windows service; leave empty if not a service
		OTChar* m_serviceDescription;  // description of the windows service; optional
		unsigned long m_authLevel;             // DCOM call authentication level
		unsigned long m_impLevel;              // DCOM call impersonation level
		short m_versionOTC;            // used OTC version
		unsigned long m_xmldaGetStatusInterval;         //  configurable interval for GetStatus cyclic call
	} OTCInitData;

	typedef struct _OTCExecutionOptions
	{
		unsigned long m_executionContext;
		unsigned char m_executionType;
	} OTCExecutionOptions;

	typedef struct _OTCObjectContext
	{
		OTCObjectData m_objectData;
		OTCObjectData m_sessionData;
		OTCObjectData m_subscriptionData;
		unsigned long m_objectType;
	} OTCObjectContext;

	typedef struct _OTCAEReturnedAttributesData
	{
		unsigned long m_categoryID;
		unsigned long m_countAttributeIDs;
		unsigned long* m_attributeIDs;
	} OTCAEReturnedAttributesData;

	typedef struct _OTCObjectAttributes
	{
		// DA Session
		OTChar* m_daSessionClientName;
		OTChar* m_daSessionURL;
		unsigned long m_daSessionLCID;
		unsigned long m_daSessionRequestTimeout;
		unsigned long m_daSessionComClsctx;

		// DA Subscription
		unsigned long m_daSubscriptionLCID;
		OTChar* m_daSubscriptionName;
		unsigned long m_daSubscriptionRequestedUpdateRate;
		unsigned long m_daSubscriptionRevisedUpdateRate;
		long m_daSubscriptionTimeBias;
		unsigned long m_daSubscriptionRequestedKeepAliveTime;
		unsigned long m_daSubscriptionRevisedKeepAliveTime;
		unsigned long m_daSubscriptionDataRetreivalMode;

		// DA Item
		OTChar* m_daItemID;
		OTChar* m_daItemPath;
		unsigned short m_daItemNativeDatatype;
		unsigned short m_daItemRequestedDatatype;
		unsigned long m_daItemAccessRights;
		OTCVariant* m_daItemEuInfo;
		float m_daItemDeadband;

		// AE Session
		unsigned long m_aeSessionLCID;
		OTChar* m_aeSessionClientName;
		OTChar* m_aeSessionURL;
		unsigned long m_aeSessionComClsctx;

		// AE Subscription
		unsigned long m_aeSubscriptionRequestedBufferTime;
		unsigned long m_aeSubscriptionRevisedBufferTime;
		unsigned long m_aeSubscriptionRequestedMaxSize;
		unsigned long m_aeSubscriptionRevisedMaxSize;
		unsigned long m_aeSubscriptionFilterSeverityLow;
		unsigned long m_aeSubscriptionFilterSeverityHigh;
		unsigned long m_aeSubscriptionFilterEventTypes;
		unsigned long m_aeSubscriptionFilterCountCategrories;
		unsigned long m_aeSubscriptionFilterCountAreas;
		unsigned long* m_aeSubscriptionFilterCategrories;
		OTChar** m_aeSubscriptionFilterAreas;
		unsigned long m_aeSubscriptionFilterCountSources;
		unsigned long m_aeSubscriptionCountReturnedAttributes;
		OTChar** m_aeSubscriptionFilterSources;
		OTCAEReturnedAttributesData* m_aeSubscriptionReturnedAttributes;

		// DA Subscription
		float m_daSubscriptionDeadband;

		// DA Session
		unsigned char m_daSessionSupportedOPCSpecification;
		unsigned char m_daSessionForcedOPCSpecification;
		unsigned char m_daSessionPerformInitialRead;

		// DA Item
		unsigned char m_daItemEuType;
	} OTCObjectAttributes;

	typedef struct _OTCServerStatus
	{
		OTCDateTime m_startTime;      // COM + XML
		OTCDateTime m_currentTime;    // COM + XML
		OTChar* m_vendorInfo;          // COM + XML
		OTChar* m_productVersion;      // COM + XML
		unsigned long m_state;                // COM + XML
		unsigned long m_countLCIDs;
		unsigned long* m_supportedLCIDs;      // COM + XML
		OTCDateTime m_lastUpdateTime; // COM
		unsigned long m_groupCount;           // COM
		unsigned long m_bandwidth;            // COM
		OTChar* m_statusInfo;          //       XML
	} OTCServerStatus;

	typedef struct _OTCServerData
	{
		OTChar* m_clsid;
		OTChar* m_progId;
		OTChar* m_vProgId;
		OTChar* m_description;
		OTChar* m_url;
		unsigned char m_opcSpecifications;
	} OTCServerData;

	typedef struct _OTCAddressSpaceBrowseOptions
	{
		// filter
		OTChar* m_elementNameFilter;
		OTChar* m_vendorFilter;                   // DA V3 and XML-DA
		OTChar* m_continuationPoint;              // DA V3 and XML-DA
		unsigned long m_accessRightsFilter;       // DA V1 and V2

		// retrieve options
		unsigned long m_maxElements;              // DA V3 and XML-DA

		unsigned short m_datatypeFilter;          // DA V1 and V2

		// retrieve what data
		unsigned char m_retrieveItemID;
		unsigned char m_retrieveProperties;       // DA V3 and XML-DA
		unsigned char m_retrievePropertyValues;   // DA V3 and XML-DA

		unsigned char m_forceBrowseUp;            // DA V2

		unsigned char m_type;                     // OT_ADDRESSSPACETYPE
	} OTCAddressSpaceBrowseOptions;

	typedef struct _OTCAddressSpaceElementData
	{
		OTChar* m_name;

		// DA
		OTChar* m_itemID;
		OTChar* m_itemPath;

		OTCObjectHandle m_objectHandle;

		unsigned char m_type;                                       // OT_ADDRESSSPACETYPE
	} OTCAddressSpaceElementData;


	typedef struct _OTCGetDAPropertiesOptions
	{
		unsigned long m_propertyCount;
		unsigned long* m_pPropertyIDs;
		OTChar** m_pPropertyNames;
		unsigned char m_getLocalFromAddressSpaceElement;  // get properties stored in the address space element
		// only used if !m_getLocalFromAddressSpaceElement
		unsigned char m_whatPropertyData;
	} OTCGetDAPropertiesOptions;


	typedef struct _OTCDAPropertyData
	{
		OTChar* m_name;
		OTChar* m_itemID;
		OTChar* m_itemPath;
		OTChar* m_description;
		OTCValueData m_value;
		unsigned long m_ID;
		long m_result;
		unsigned short m_datatype;
	} OTCDAPropertyData;

	typedef struct _OTCEventData
	{
		// simple event data
		unsigned long m_eventType;
		unsigned long m_eventCategory;       // event category id
		unsigned long m_severity;            // severity
		OTChar* m_sourcePath;         // fully qualified path of event source
		OTChar* m_message;            // event message
		OTCDateTime m_occurenceTime; // time of event occurance
		unsigned long m_eventAttrCount;
		OTCVariant* m_pEventAttrs;  // array with all event attributes

		// tracking and condition related event data
		OTChar* m_actorID;

		// condition related event data
		OTChar* m_conditionName;
		OTChar* m_subConditionName;
		OTCDateTime m_activeTime;
		unsigned long m_cookie;
		unsigned short m_quality;
		unsigned short m_changeMask;
		unsigned char m_newState;
		unsigned char m_ackRequired;
	} OTCEventData;

	typedef struct _OTCAEConditionStateData
	{
		// condition state data
		OTChar* m_actSubConditionName;
		OTChar* m_actSubConditionDefinition;
		OTChar* m_actSubConditionDescription;
		OTCDateTime m_conditionActiveTime;
		OTCDateTime m_conditionAckTime;
		OTCDateTime m_subConditionActiveTime;
		OTCDateTime m_conditionInactiveTime;
		OTChar* m_ackID;
		OTChar* m_ackComment;
		unsigned long m_actSubConditionSeverity;
		unsigned long m_subConditionCount;
		OTChar** m_pSubConditionDefinition;
		unsigned long* m_pSubConditionSeverity;
		OTChar** m_pSubConditionDescription;
		OTChar** m_pSubConditionName;
		OTCVariant* m_pEventAttrs;  // array with all event attributes
		long* m_pAttrErrors;
		unsigned long m_eventAttrCount;
		unsigned short m_quality;
		unsigned char m_state;
	} OTCAEConditionStateData;



//-----------------------------------------------------------------------------
// OPC Toolkit C Client DLL callbacks                                         |
//-----------------------------------------------------------------------------

	typedef void (OTCAPI_CALL* OTCOnStateChange)(IN OTCObjectContext objectContext, IN unsigned char state);
	typedef void (OTCAPI_CALL* OTCOnDataChange)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN unsigned long count, IN OTCObjectData* pItemData, IN OTCValueData* pValues, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnReadComplete)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long count, IN OTCObjectData* pItemData, IN OTChar** pItemIDs, IN OTChar** pItemPaths, IN OTCValueData* pValues, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnWriteComplete)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long count, IN OTCObjectData* pItemData, IN OTChar** pItemIDs, IN OTChar** pItemPaths, IN OTCValueData* pValues, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnPerfromStateTransition)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result);
	typedef unsigned char(OTCAPI_CALL* OTCOnServerShutdown)(IN OTCObjectData sessionData, IN OTChar* reason);
	typedef void (OTCAPI_CALL* OTCOnUpdateAttributes)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned char fromServer, IN unsigned long attributesCount, IN unsigned long* pWhatAttributes, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnUpdateDAItemAttributes)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long itemCount, IN OTCObjectData* pItemData, IN unsigned char fromServer, IN unsigned long attributesCount, IN unsigned long* pWhatAttributes, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnValidateDAItems)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned long itemCount, IN OTCObjectData* pItemData, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnGetServerStatus)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTCServerStatus* pServerStatus);
	typedef void (OTCAPI_CALL* OTCOnBrowseServer)(IN unsigned long executionContext, IN long result, IN OTChar* ipAddress, IN unsigned char whatOPCSpec, IN unsigned char whatServerData, IN unsigned long serverDataCount, IN OTCServerData* pServerData);
	typedef void (OTCAPI_CALL* OTCOnBrowseAddressSpace)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTCObjectHandle addressSpaceElementHandle, IN OTChar* addressSpaceElementID, IN OTChar* addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, IN unsigned long addressSpaceElementDataCount, IN OTCAddressSpaceElementData* pAddressSpaceElement);
	typedef void (OTCAPI_CALL* OTCOnGetDAProperties)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTCObjectHandle addressSpaceElementHandle, IN OTChar* addressSpaceElementID, IN OTChar* addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, IN unsigned long propertyDataCount, IN OTCDAPropertyData* pPropertyData);
	typedef void (OTCAPI_CALL* OTCOnReceivedEvents)(IN OTCObjectContext objectContext, IN unsigned long count, IN OTCEventData* pEvents, IN unsigned char refresh, IN unsigned char lastRefresh);
	typedef void (OTCAPI_CALL* OTCOnRefreshAEConditions)(IN unsigned long executionContext, IN OTCObjectContext objectContext, IN long result, IN unsigned char cancelRefresh);
	typedef void (OTCAPI_CALL* OTCOnAcknowledgeAEConditions)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTChar* ackID, IN OTChar* ackComment, IN unsigned long count, IN OTCEventData* pEvents, IN long* pResults);
	typedef void (OTCAPI_CALL* OTCOnQueryAvailableAEFilters)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN unsigned long availableFilters);
	typedef void (OTCAPI_CALL* OTCOnQueryAECategories)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN unsigned long count, IN unsigned long* pEventTypes, IN unsigned long* pCategoryIds, IN OTChar** pCategoryDescriptions);
	typedef void (OTCAPI_CALL* OTCOnQueryAEAttributes)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN unsigned long categoryId, IN unsigned long count, IN unsigned long* pAttributeIds, IN OTChar** pAttributeDescriptions, IN unsigned short* pAttributeDatatypes);
	typedef void (OTCAPI_CALL* OTCOnQueryAEConditions)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN unsigned long categoryId, IN unsigned long count, IN OTChar** pConditionsNames);
	typedef void (OTCAPI_CALL* OTCOnQueryAESubConditions)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTChar* conditionName, IN unsigned long count, IN OTChar** pSubConditionsNames);
	typedef void (OTCAPI_CALL* OTCOnQueryAESourceConditions)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTChar* sourcePath, IN unsigned long count, IN OTChar** pConditionsNames);
	typedef void (OTCAPI_CALL* OTCOnGetAEConditionState)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTChar* sourcePath, IN OTChar* conditionName, IN unsigned long attributeCount, IN unsigned long* pAttributeIds, IN OTCAEConditionStateData* pConditionState);
	typedef void (OTCAPI_CALL* OTCOnEnableAEConditions)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN unsigned char enable, IN unsigned char areaOrSource, IN OTChar* path);
	typedef void (OTCAPI_CALL* OTCOnGetErrorString)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN long errorCode, IN OTChar* errorString);
	typedef void (OTCAPI_CALL* OTCOnLogon)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result, IN OTChar* userName, IN OTChar* password);
	typedef void (OTCAPI_CALL* OTCOnLogoff)(IN unsigned long executionContext, IN OTCObjectData sessionData, IN long result);

	typedef struct _OTCCallbackFunctions
	{
		// callbacks
		OTOnTrace m_OTOnTrace;
		OTCOnStateChange m_OTCOnStateChange;
		OTCOnDataChange m_OTCOnDataChange;
		OTCOnReadComplete m_OTCOnReadComplete;
		OTCOnWriteComplete m_OTCOnWriteComplete;
		OTCOnPerfromStateTransition m_OTCOnPerfromStateTransition;
		OTCOnServerShutdown m_OTCOnServerShutdown;
		OTCOnUpdateAttributes m_OTCOnUpdateAttributes;
		OTCOnUpdateDAItemAttributes m_OTCOnUpdateDAItemAttributes;
		OTCOnValidateDAItems m_OTCOnValidateDAItems;
		OTCOnGetServerStatus m_OTCOnGetServerStatus;
		OTCOnBrowseAddressSpace m_OTCOnBrowseAddressSpace;
		OTCOnGetDAProperties m_OTCOnGetDAProperties;
		OTCOnBrowseServer m_OTCOnBrowseServer;
		OTCOnReceivedEvents m_OTCOnReceivedEvents;
		OTCOnRefreshAEConditions m_OTCOnRefreshAEConditions;
		OTCOnAcknowledgeAEConditions m_OTCOnAcknowledgeAEConditions;
		OTCOnQueryAvailableAEFilters m_OTCOnQueryAvailableAEFilters;
		OTCOnQueryAECategories m_OTCOnQueryAECategories;
		OTCOnQueryAEAttributes m_OTCOnQueryAEAttributes;
		OTCOnQueryAEConditions m_OTCOnQueryAEConditions;
		OTCOnQueryAESubConditions m_OTCOnQueryAESubConditions;
		OTCOnQueryAESourceConditions m_OTCOnQueryAESourceConditions;
		OTCOnGetAEConditionState m_OTCOnGetAEConditionState;
		OTCOnEnableAEConditions m_OTCOnEnableAEConditions;
		OTCOnGetErrorString m_OTCOnGetErrorString;
		OTCOnLogon m_OTCOnLogon;
		OTCOnLogoff m_OTCOnLogoff;
	} OTCCallbackFunctions;


//-----------------------------------------------------------------------------
// OPC Toolkit C Client DLL functions                                         |
//-----------------------------------------------------------------------------

	long OTCAPI_CALL OTCInitialize(IN OTCInitData* pInitData);

	long OTCAPI_CALL OTCTerminate(void);

	long OTCAPI_CALL OTCProcessCommandLine(IN OTChar* commandLine);

	long OTCAPI_CALL OTCAdvise(IN OTCCallbackFunctions* pCallbacks);

	long OTCAPI_CALL OTCAddDASession(IN OTChar* url, IN unsigned long sessionUserData, OUT OTCObjectHandle* pObjectHandle);

	long OTCAPI_CALL OTCAddDASubscription(IN OTCObjectHandle sessionHandle, IN unsigned long updateRate, IN unsigned long groupUserData, OUT OTCObjectHandle* pGroupHandle);

	long OTCAPI_CALL OTCAddDAItem(IN OTCObjectHandle groupHandle, IN OTChar* itemID, IN unsigned long itemUserData, OUT OTCObjectHandle* pItemHandle);

	long OTCAPI_CALL OTCRemoveDASession(IN OTCObjectHandle objectHandle);

	long OTCAPI_CALL OTCRemoveDASubscription(IN OTCObjectHandle objectHandle);

	long OTCAPI_CALL OTCRemoveDAItem(IN OTCObjectHandle objectHandle);

	long OTCAPI_CALL OTCGetState(IN OTCObjectHandle objectHandle, OUT unsigned char* pCurrentState, OUT unsigned char* pTargetState);

	long OTCAPI_CALL OTCChangeTargetState(IN OTCObjectHandle objectHandle, IN unsigned char state, IN unsigned char deep);

	long OTCAPI_CALL OTCPerformStateTransition(IN OTCObjectHandle objectHandle, IN unsigned char deep, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCRead(IN OTCObjectHandle objectHandle, IN unsigned long count, IN OTCObjectHandle* pItemHandles, IN OTChar** pItemIDs, IN OTChar** pItemPaths, IN unsigned long maxAge, OUT OTCValueData* pValues, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCWrite(IN OTCObjectHandle objectHandle, IN unsigned long count, IN OTCObjectHandle* pItemHandles, IN OTChar** pItemIDs, IN OTChar** pItemPaths, IN OTCValueData* pValues, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCGetAttributes(IN OTCObjectHandle objectHandle, IN unsigned long whatAttributes, OUT OTCObjectAttributes* pObjectAttributes);

	long OTCAPI_CALL OTCSetAttributes(IN OTCObjectHandle objectHandle, IN unsigned long whatAttributes, IN OTCObjectAttributes* pObjectAttributes);

	long OTCAPI_CALL OTCUpdateAttributes(IN OTCObjectHandle objectHandle, IN unsigned char fromServer, IN unsigned long attributeCount, IN unsigned long* pWhatAttributes, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCUpdateDAItemAttributes(IN OTCObjectHandle subscriptionHandle, IN unsigned long itemCount, IN OTCObjectHandle* pItemHandles, IN unsigned char fromServer, IN unsigned long attributeCount, IN unsigned long* pWhatAttributes, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCGetServerStatus(IN OTCObjectHandle sessionHandle, OUT OTCServerStatus* pServerStatus, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCValidateDAItems(IN OTCObjectHandle subscriptionHandle, IN unsigned long itemCount, IN OTCObjectHandle* pItemHandles, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCActivateConnectionMonitor(IN OTCObjectHandle sessionHandle, unsigned char activate, unsigned long checkCycle, unsigned long connect1Attempts, unsigned long connect1Cycle, unsigned long connect2Cycle);

	long OTCAPI_CALL OTCBrowseServer(IN OTChar* ipAddress, IN unsigned char whatOPCSpec, IN unsigned char whatServerData, OUT unsigned long* pServerDataCount, OUT OTCServerData** ppServerData, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCBrowseAddressSpace(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN OTChar* addressSpaceElementID, IN OTChar* addressSpaceElementPath, IN OTCAddressSpaceBrowseOptions* pBrowseOptions, OUT unsigned long* pAddressSpaceElementDataCount, OUT OTCAddressSpaceElementData** ppAddressSpaceElement, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCReleaseAddressSpaceElement(IN OTCObjectHandle addressSpaceElementHandle, IN OTBool deep);

	long OTCAPI_CALL OTCGetDAProperties(IN OTCObjectHandle sessionHandle, IN OTCObjectHandle addressSpaceElementHandle, IN OTChar* addressSpaceElementID, IN OTChar* addressSpaceElementPath, IN OTCGetDAPropertiesOptions* pGetPropertiesOptions, OUT unsigned long* pPropertiesDataCount, OUT OTCDAPropertyData** ppProperty, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCRefreshDAGroup(IN OTCObjectHandle subscriptionHandle, IN unsigned long maxAge, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCAddAESession(IN OTChar* url, IN unsigned long sessionUserData, OUT OTCObjectHandle* pObjectHandle);

	long OTCAPI_CALL OTCAddAESubscription(IN OTCObjectHandle sessionHandle, IN unsigned long subscriptionUserData, OUT OTCObjectHandle* pSubscriptionHandle);

	long OTCAPI_CALL OTCRemoveAESession(IN OTCObjectHandle objectHandle);

	long OTCAPI_CALL OTCRemoveAESubscription(IN OTCObjectHandle objectHandle);

	long OTCAPI_CALL OTCRefreshAEConditions(IN OTCObjectHandle subscriptionHandle, IN unsigned char cancelRefresh, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCAcknowledgeAEConditions(IN OTCObjectHandle sessionHandle, IN OTChar* ackID, IN OTChar* ackComment, IN unsigned long count, IN OTCEventData* pEvents, OUT long* pResults, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAvailableAEFilters(IN OTCObjectHandle sessionHandle, OUT unsigned long* pAvailableFilters, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAECategories(IN OTCObjectHandle sessionHandle, OUT unsigned long* pCount, OUT unsigned long** ppEventTypes, OUT unsigned long** ppCategoryIds, OUT OTChar** *ppCategoryDescriptions, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAEAttributes(IN OTCObjectHandle sessionHandle, IN unsigned long categoryId, OUT unsigned long* pCount, OUT unsigned long** ppAttributeIds, OUT OTChar** *ppAttributeDescriptions, OUT unsigned short** ppAttributeDatatypes, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAEConditions(IN OTCObjectHandle sessionHandle, IN unsigned long categoryId, OUT unsigned long* pCount, OUT OTChar** *ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAESubConditions(IN OTCObjectHandle sessionHandle, IN OTChar* conditionName, OUT unsigned long* pCount, OUT OTChar** *ppSubConditionsNames, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCQueryAESourceConditions(IN OTCObjectHandle sessionHandle, IN OTChar* sourcePath, OUT unsigned long* pCount, OUT OTChar** *ppConditionsNames, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCGetAEConditionState(IN OTCObjectHandle sessionHandle, IN OTChar* sourcePath, IN OTChar* conditionName, IN unsigned long attributeCount, IN unsigned long* pAttributeIds, OUT OTCAEConditionStateData* pConditionState, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCEnableAEConditions(IN OTCObjectHandle sessionHandle, IN unsigned char enable, IN unsigned char areaOrSource, IN OTChar* path, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCGetErrorString(IN OTCObjectHandle sessionHandle, IN long errorCode, OUT OTChar** pErrorString, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCLogon(IN OTCObjectHandle sessionHandle, IN OTChar* userName, IN OTChar* password, IN OTCExecutionOptions* pExecutionOptions);

	long OTCAPI_CALL OTCLogoff(IN OTCObjectHandle sessionHandle, IN OTCExecutionOptions* pExecutionOptions);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

