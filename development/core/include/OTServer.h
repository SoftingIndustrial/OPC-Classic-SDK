#ifndef _OTSERVER_H_
#define _OTSERVER_H_

#include "OTcommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// Defines                                                                    |
//-----------------------------------------------------------------------------

// calling convention
#define OTSAPI_CALL OTAPI_CALL

#define OTS_FEATURE_DA_SERVER           0x01
#define OTS_FEATURE_XMLDA_SERVER        0x02
#define OTS_FEATURE_AE_SERVER           0x04
#define OTS_FEATURE_TP_SERVER           0x08

#define OTS_APPTYPE_INPROC  0 // DLL 
#define OTS_APPTYPE_OUTPROC 1 // EXE

#define OTS_IOMODE_NONE          0x00
#define OTS_IOMODE_POLL          0x01
#define OTS_IOMODE_REPORT        0x02
#define OTS_IOMODE_POLL_OWNCACHE 0x11
#define OTS_IOMODE_REPORT_CYCLIC 0x22

#define OTS_ADDRESSSPACETYPE_OBJECT 0x01
#define OTS_ADDRESSSPACETYPE_STRING 0x02

#define OTS_SESSIONSTATE_CREATE      0
#define OTS_SESSIONSTATE_LOGON       1
#define OTS_SESSIONSTATE_LOGOFF      2
#define OTS_SESSIONSTATE_MODIFY      3
#define OTS_SESSIONSTATE_DESTROY     -1

#define OTS_SESSIONTYPE_DA               1
#define OTS_SESSIONTYPE_XMLDA            6
#define OTS_SESSIONTYPE_XMLSUBSCRIPTIONS 2
#define OTS_SESSIONTYPE_AE               8

//-----------------------------------------------------------------------------
// Structures                                                                 |
//-----------------------------------------------------------------------------

#define OTSObjectHandle OTObjectHandle
#define OTSVariant OTVariant
#define OTSDateTime OTDateTime
#define OTSObjectData OTObjectData
#define OTSValueData OTValueData
	typedef unsigned long OTSDataHandle;


	typedef struct _OTSInitData
	{
		// server identity
		OTChar* m_serviceName;                          // name of the windows service; leave empty if not a service
		OTChar* m_serviceDescription;                   // description of the windows service; optional
		OTChar* m_clsidDA;                              // class id of DA server
		OTChar* m_progIdDA;                             // prog id of DA server
		OTChar* m_verIndProgIdDA;                       // version independent prog id of DA server
		OTChar* m_clsidAE;                              // class id of DA server
		OTChar* m_progIdAE;                             // prog id of DA server
		OTChar* m_verIndProgIdAE;                       // version independent prog id of DA server
		OTChar* m_description;                          // description of server
		OTChar* m_urlDA;                                // URL of XML-DA server
		// version information
		short m_majorVersion;                           // major version number of the server
		short m_minorVersion;                           // minor version number of the server
		short m_patchVersion;                           // patch version number of the server
		short m_buildNumber;                            // build number of the server
		OTChar* m_vendorInfo;                           // version info of the server
		// server parameters
		long m_clientCheckPeriod;                       // period in ms to check the notification connections to the clients
		// DA server parameters
		long m_minUpdateRateDA;                         // minimal update rate for a group (ms)
		// Web server parameters
		OTChar* m_webRootDirectory;                     // root directory for web pages
		OTChar* m_webRootFile;                          // root file of the web server; default: index.html
		OTChar* m_webAdministratorPassword;             // web administrator user password
		OTChar* m_webOperatorPassword;                  // web operator user password

		unsigned long m_stringBasedObjectDataExpiry;    // string-based object data expiry timeout

		//  Tunnel protocol properties
		long m_tpCredentialsNumber;                     //  the number of TP credentials
		OTChar** m_tpUsers;                             //  the list containing the users
		OTChar** m_tpPasswords;                         //  the list containing the passwords
		// TODO: moved
		unsigned short m_tpPort;                        //  TP port number

		short m_versionOTS;                             // used OTS version
		unsigned short m_ipPortHTTP;                    // IP port number of HTTP server
		unsigned char m_appType;                        // application type of OPC server
		// server parameters
		OTChar m_addressSpaceDelimiter;                 // delimiter sign for addressSpace levels in the item id
		// DA server parameters
		OTChar m_propertyDelimiter;                     // delimiter sign for DA properties in the item id
		// AE server parameters
		unsigned char m_supportDisableConditions;       // support to enable or disable conditions
		// DA and AE server parameters
		unsigned char m_optimizeForSpeed;               // optimize the internal Toolkit CORE structures for speed in detriment of sorted items
		// DCOM-related security settings
		unsigned long m_authLevel;                      //  DCOM call authentication level
		unsigned long m_impLevel;                       //  DCOM call impersonation level
		unsigned long m_cbAuthLevel;                    //  DCOM callback authentication level
		unsigned long m_cbImpLevel;                     //  DCOM callback impersonation level

	} OTSInitData;

	typedef struct _OTSAddressSpaceElementData
	{
		OTChar* m_name;
		OTChar* m_itemID;
		unsigned char m_accessRights;
		unsigned char m_ioMode;
		unsigned char m_hasChildren;
		unsigned char m_isBrowsable;
		unsigned long m_userData;
		unsigned short m_datatype;
		unsigned char m_elementType;
	} OTSAddressSpaceElementData;

	typedef struct _OTSRequestData
	{
		OTSObjectData m_object;
		long m_propertyID;
		OTSObjectHandle m_requestHandle;
		OTSObjectHandle m_clientHandle;
	} OTSRequestData;

	typedef struct _OTSPropertyData
	{
		unsigned long m_pid;          // property id
		OTChar* m_name;       // property name
		OTChar* m_itemID;     // property part of the item id
		OTChar* m_descr;      // description
		unsigned short m_datatype;     // data type
		unsigned char m_accessRights; // access rights over item
	} OTSPropertyData;


	typedef struct _OTSItemData
	{
		OTSObjectData m_object;
		unsigned char m_active;
		long m_sampleRate;
	} OTSItemData;

	typedef struct _OTSSessionData
	{
		// OTS_SESSIONSTATE_LOGON
		OTChar* m_userName;       // logon user name
		OTChar* m_password;       // logon password

		OTChar* m_clientName;           // client name
		OTChar* m_clientIp;             // client Ip (and port)

		short m_state;           // client state : OTS_SESSIONSTATE_XXX
		unsigned char m_type;             // type of the client : OPC_CLIENTTYPE_XXX
		unsigned char m_clientRemote;   // 0 for localhost, != 0 for remote

	} OTSSessionData;

	typedef struct _OTSEventData
	{
		unsigned long m_eventType;
		unsigned long m_eventCategory;       // event category id
		unsigned long m_severity;            // severity
		OTChar* m_sourcePath;         // fully qualified path of event source
		OTChar* m_message;            // event message
		OTSDateTime m_occurenceTime; // time of event occurrence
		OTChar* m_actorID;
		unsigned long m_eventAttrCount;
		OTSVariant* m_pEventAttrs;  // array with all event attributes
	} OTSEventData;

	typedef struct _OTSConditionData
	{
		unsigned long m_eventCategory;       // event category id
		unsigned long m_severity;            // severity
		OTChar* m_sourcePath;         // fully qualified path of event source
		OTChar* m_message;            // event message
		OTSDateTime m_occurenceTime; // time of event occurrence
		OTChar* m_name;
		OTChar* m_activeSubConditionName;
		OTChar* m_ackID;
		OTChar* m_ackComment;
		OTSDateTime m_ackTime;
		OTSVariant* m_pEventAttrs;  // array with all event attributes
		unsigned long m_eventAttrCount;
		unsigned long m_userData;
		unsigned short m_changeMask;
		unsigned short m_quality;
		unsigned char m_ackRequired;
		unsigned char m_stateChange;
	} OTSConditionData;


	typedef struct _OTSConditionDefinitionData
	{
		OTChar* m_definition;
		OTChar** m_subConditionDefinitions;
		OTChar** m_subConditionDescriptions;
		unsigned long* m_subConditionSeverities;
		unsigned long m_subConditionCount;
	} OTSConditionDefinitionData;


//-----------------------------------------------------------------------------
// OPC Toolkit C Server DLL callbacks                                         |
//-----------------------------------------------------------------------------

	typedef long(OTSAPI_CALL* OTSShutdown)(void);
	typedef long(OTSAPI_CALL* OTSHandleReadRequests)(IN long count, IN OTSRequestData* paRequests);
	typedef long(OTSAPI_CALL* OTSHandleWriteRequests)(IN long count, IN OTSRequestData* paRequests, IN OTSValueData* pValues);
	typedef long(OTSAPI_CALL* OTSRequestCompletedInternally)(IN unsigned long requestHandle);
	typedef long(OTSAPI_CALL* OTSQueryProperties)(IN OTSObjectData* pObjectData, IN OTChar* objItemId, IN long propID, OUT unsigned long* pPropCount, OUT OTSPropertyData** ppPropData);
	typedef long(OTSAPI_CALL* OTSQueryAddressSpaceElementData)(IN OTChar* path, IN unsigned char m_elementType, OUT OTSAddressSpaceElementData* pData);
	typedef long(OTSAPI_CALL* OTSQueryAddressSpaceElementDataEx)(IN OTChar* path, IN unsigned char m_elementType, IN unsigned short& elementDataFilter, OUT OTSAddressSpaceElementData* pData);
	typedef long(OTSAPI_CALL* OTSQueryAddressSpaceElementChildren)(IN OTChar* path, IN unsigned char m_elementType, OUT unsigned long* pCount, OUT OTSAddressSpaceElementData** pElementData);
	typedef long(OTSAPI_CALL* OTSChangeSessionState)(IN OTSObjectHandle hClient, IN OTSSessionData* pClientStateData);
	typedef long(OTSAPI_CALL* OTSQueryCacheValue)(IN OTSObjectHandle hClient, IN OTSObjectData objectData, OUT OTSValueData* pValue);
	typedef long(OTSAPI_CALL* OTSChangeItems)(IN long itemCnt, IN OTSItemData* pItemData);
	typedef long(OTSAPI_CALL* OTSCreateAddressSpaceElement)(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OTSAddressSpaceElementData* pDataIn, OUT OTSAddressSpaceElementData* pDataOut);
	typedef long(OTSAPI_CALL* OTSCreateAddressSpaceElementEx)(IN OTSObjectHandle hParent, IN OTSObjectHandle hObject, IN OTSAddressSpaceElementData* pDataIn, IN unsigned short elementDataFilter, OUT OTSAddressSpaceElementData* pDataOut);
	typedef long(OTSAPI_CALL* OTSDestroyAddressSpaceElement)(IN OTSObjectData objectData);
	typedef long(OTSAPI_CALL* OTSQueryConditions)(IN OTSObjectData* pObjectData, IN OTChar* sourcePath, OUT unsigned long* pConditionCount, OUT OTChar** *pConditionNames);
	typedef long(OTSAPI_CALL* OTSAcknowledgeCondition)(IN OTSObjectData conditionData, IN OTChar* ackId, OTChar* ackComment);
	typedef long(OTSAPI_CALL* OTSQueryConditionDefinition)(IN OTSObjectData conditionData, OUT OTSConditionDefinitionData* pConditionDefData);
	typedef long(OTSAPI_CALL* OTSEnableConditions)(IN unsigned char enable, IN OTChar* addressSpaceElementName);
	typedef long(OTSAPI_CALL* OTSWebHandleTemplate)(IN OTChar* templateName, IN unsigned long numArgs, IN OTChar** pArgs, OUT OTChar** pResult);

	typedef struct _OTSCallbackFunctions
	{
		OTOnTrace m_OTOnTrace;
		OTSShutdown m_OTSShutdown;
		OTSHandleReadRequests m_OTSHandleReadRequests;
		OTSHandleWriteRequests m_OTSHandleWriteRequests;
		OTSRequestCompletedInternally m_OTSRequestCompletedInternally;
		OTSQueryProperties m_OTSQueryProperties;
		OTSQueryAddressSpaceElementData m_OTSQueryAddressSpaceElementData;
		OTSQueryAddressSpaceElementDataEx m_OTSQueryAddressSpaceElementDataEx;
		OTSQueryAddressSpaceElementChildren m_OTSQueryAddressSpaceElementChildren;
		OTSChangeSessionState m_OTSChangeSessionState;
		OTSQueryCacheValue m_OTSQueryCacheValue;
		OTSChangeItems m_OTSChangeItems;
		OTSCreateAddressSpaceElement m_OTSCreateAddressSpaceElement;
		OTSCreateAddressSpaceElementEx m_OTSCreateAddressSpaceElementEx;
		OTSDestroyAddressSpaceElement m_OTSDestroyAddressSpaceElement;
		OTSQueryConditions m_OTSQueryConditions;
		OTSAcknowledgeCondition m_OTSAcknowledgeCondition;
		OTSQueryConditionDefinition m_OTSQueryConditionDefinition;
		OTSEnableConditions m_OTSEnableConditions;
		OTSWebHandleTemplate m_OTSWebHandleTemplate;
	} OTSCallbackFunctions;


//-----------------------------------------------------------------------------
// OPC Toolkit C Server DLL functions                                         |
//-----------------------------------------------------------------------------

	long OTSAPI_CALL OTSInitialize(IN OTSInitData* pInitData);

	long OTSAPI_CALL OTSTerminate(void);

	long OTSAPI_CALL OTSProcessCommandLine(IN OTChar* commandLine);

	long OTSAPI_CALL OTSSetServerState(IN DWORD daServerState, IN DWORD aeServerState);

	long OTSAPI_CALL OTSStart(void);

	long OTSAPI_CALL OTSReady(void);

	long OTSAPI_CALL OTSStop(IN OTChar* reason = NULL);

	long OTSAPI_CALL OTSAdvise(IN OTSCallbackFunctions* pCallbacks);

	long OTSAPI_CALL OTSAddAddressSpaceElement(IN OTSObjectHandle hParent, IN OTSAddressSpaceElementData* pData, OUT OTSObjectHandle* phObject);

	long OTSAPI_CALL OTSRemoveAddressSpaceElement(IN OTSObjectHandle hObject);

	long OTSAPI_CALL OTSCompleteRequests(IN long count, IN OTSRequestData* paRequests, IN long* paResult, IN OTSValueData* paValues);

	long OTSAPI_CALL OTSAckRequestElement(IN OTSObjectHandle objectHandle);

	long OTSAPI_CALL OTSNackRequestElement(IN OTSObjectHandle objectHandle);

	long OTSAPI_CALL OTSGetCacheValue(IN OTSObjectHandle hObject, OUT OTSValueData* pValue);

	long OTSAPI_CALL OTSValuesChanged(IN long count, IN OTSObjectHandle* pahObjects, IN OTSValueData* paValues);

	long OTSAPI_CALL OTSGetAddressSpaceElementData(IN OTSObjectHandle hObject, OUT OTSAddressSpaceElementData* pData);

	long OTSAPI_CALL OTSGetParent(IN OTSObjectHandle hObject, OUT OTSObjectData* pParent);

	long OTSAPI_CALL OTSGetChildren(IN OTSObjectHandle hObject, IN unsigned char m_elementType, OUT long* pCount, OUT OTSObjectData** ppChildren);

	long OTSAPI_CALL OTSInitAddressSpace(IN unsigned char addressSpaceType);

	long OTSAPI_CALL OTSAddEventCategory(IN unsigned long categoryID, IN OTChar* description, IN unsigned long eventType, OUT OTSObjectHandle* pCatHandle);

	long OTSAPI_CALL OTSAddEventAttribute(IN OTSObjectHandle hCategory, IN unsigned long attributeID, IN OTChar* description, IN unsigned short datatype);

	long OTSAPI_CALL OTSFireEvents(IN unsigned long eventCount, IN OTSEventData* pEventData);

	long OTSAPI_CALL OTSAddCondition(IN OTSObjectHandle hCategory, IN OTChar* conditionName);

	long OTSAPI_CALL OTSAddSubCondition(IN OTSObjectHandle hCategory, IN OTChar* conditionName, IN OTChar* subConditionName);

	long OTSAPI_CALL OTSConditionsChanged(IN long count, IN OTSObjectHandle* pObjects, IN OTSConditionData* pData, OUT OTSObjectHandle* pObjectsOut);

	long OTSAPI_CALL OTSGetConditionData(IN OTSObjectHandle hObject, OUT OTSConditionData* pData);

	long OTSAPI_CALL OTSSetEUInfo(IN OTSObjectHandle hObject, IN OTBool isEnumerated, IN unsigned long count, IN OTChar** pEnumeratedValues, IN double lowEU, IN double highEU);
#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
