#ifndef _SOXPSCALLBACKS_H_
#define _SOXPSCALLBACKS_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------
// Callback functions                                                         |
//-----------------------------------------------------------------------------

DWORD SOXPSCFG_CALL _SOXpSCreateNode(IN SOXpSItemHandle parentNodeHandle,
		IN LPCTSTR name, IN VARTYPE datatype, IN DWORD accessRights, IN WORD ioMode,
		IN DWORD userData, OUT SOXpSItemHandle *nodeHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateTag(IN SOXpSItemHandle parentNodeHandle,
		IN LPCTSTR name, IN VARTYPE datatype, IN DWORD accessRights, IN WORD ioMode,
		IN DWORD userData, OUT SOXpSItemHandle *tagHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateProperty(IN SOXpSItemHandle parentHandle,
		IN DWORD propertyID, IN LPCTSTR description, IN VARTYPE datatype,
		IN DWORD accessRights, IN WORD ioMode, IN LPVARIANT constantValue,
		IN DWORD userData, OUT SOXpSItemHandle *propertyHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateArea(IN SOXpSAreaHandle parentAreaHandle,
		IN LPCTSTR name, OUT SOXpSAreaHandle *areaHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateSource(IN SOXpSAreaHandle parentAreaHandle,
		IN LPCTSTR name, OUT SOXpSSourceHandle *sourceHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateCondition(IN SOXpSSourceHandle parentSourceHandle,
		IN LPCTSTR conditionClassName, OUT SOXpSConditionHandle *condHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateCategory(IN DWORD eventType,	IN DWORD categoryID,
		IN LPCTSTR description, OUT SOXpSCategoryHandle *categoryHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateAttribute(IN SOXpSCategoryHandle categoryHandle,
		IN DWORD attributeID, IN LPCTSTR description, IN VARTYPE datatype);

DWORD SOXPSCFG_CALL _SOXpSCreateConditionClass(IN SOXpSCategoryHandle categoryHandle,
		IN LPCTSTR conditionClassName, IN LPCTSTR inactiveSubCondMessage,
		IN DWORD inactiveSubCondAckRequired, IN DWORD singleStateConditionClass,
		IN LPCTSTR definition, IN DWORD activeSubCondSeverity, IN LPCTSTR activeSubCondMessage,
		IN DWORD activeSubCondAckRequired, OUT SOXpSConditionClassHandle *conditionClassHandle);

DWORD SOXPSCFG_CALL _SOXpSCreateSubConditionClass(IN SOXpSConditionClassHandle conditionClassHandle,
		IN LPCTSTR name, IN DWORD severity, IN LPCTSTR description,
		IN DWORD ackRequired, IN LPCTSTR definition);

DWORD SOXPSCFG_CALL _SOXpSFireAEEvents(IN DWORD numEvents, IN SOXpSAEEventData *eventArray);

DWORD SOXPSCFG_CALL _SOXpSCompletedDARequests(IN DWORD numRequests, IN SOXpSDARequestData *requestArray);

DWORD SOXPSCFG_CALL _SOXpSChangeAEConditions(IN DWORD numConditions, IN SOXpSAEConditionData *condArray);

DWORD SOXPSCFG_CALL _SOXpSTranslatedItemID(IN DWORD attributeID, IN LPCTSTR itemID,
		IN LPCTSTR nodeName, IN CLSID *classID);

DWORD SOXPSCFG_CALL _SOXpSShutdown(IN LPCTSTR reason);

DWORD SOXPSCFG_CALL _SOXpSSetDeviceState(IN SOXpSDeviceHandle deviceHandle, IN LPCTSTR devState, IN LPCTSTR devStateInfo);




//-----------------------------------------------------------------------------
// DLL functions typedefs                                                     |
//-----------------------------------------------------------------------------

typedef SOXpSInitData *(SOXPSCFG_CALL *_SOXpSInitialize)(IN SOXpSCompletedDARequests completedReq,
		IN SOXpSFireAEEvents fireEvents, IN SOXpSChangeAEConditions changeCond, IN SOXpSShutdown shutdown,
		IN SOXpSSetDeviceState setDeviceState);

typedef DWORD (SOXPSCFG_CALL *_SOXpSStart)(void);

typedef void (SOXPSCFG_CALL *_SOXpSStop)(void);

typedef void (SOXPSCFG_CALL *_SOXpSBuildDANameSpace)(IN SOXpSCreateNode createNode,
		IN SOXpSCreateTag createTag, IN SOXpSCreateProperty createProperty);

typedef void (SOXPSCFG_CALL *_SOXpSBuildAEAreaSpace)(IN SOXpSCreateArea createArea,
		IN SOXpSCreateSource createSource, IN SOXpSCreateCondition createCondition);

typedef void (SOXPSCFG_CALL *_SOXpSBuildAEEventSpace)(IN SOXpSCreateCategory createCategory,
		IN SOXpSCreateAttribute createAttribute, IN SOXpSCreateConditionClass createConditionClass,
		IN SOXpSCreateSubConditionClass createSubConditionClass);

typedef void (SOXPSCFG_CALL *_SOXpSHandleDARequests)(IN DWORD ioOperation,
		IN DWORD numRequests, IN SOXpSDARequestData *requestArray);

typedef void (SOXPSCFG_CALL *_SOXpSActivateDAItems)(IN BYTE activate, IN DWORD numItems,
		IN SOXpSDAItemData *itemDataArray);

typedef void (SOXPSCFG_CALL *_SOXpSActivateAESubscription)(IN BYTE activate);

typedef DWORD (SOXPSCFG_CALL *_SOXpSReceivedAEAcknowledge)(IN SOXpSConditionHandle condHandle,IN LPCTSTR sourcePath,
		IN LPCTSTR condName, IN LPCTSTR ackID, IN LPCTSTR ackComment, IN FILETIME *activeTime);

typedef void (SOXPSCFG_CALL *_SOXpSTranslateToItemIDs)(IN SOXpSConditionHandle condHandle, IN LPCTSTR sourcePath,
		IN LPCTSTR condName, IN LPCTSTR subCondName, IN DWORD numAttributes, IN DWORD *attributesArray,
		IN SOXpSTranslatedItemID translatedItemID);

#ifdef __cplusplus
}
#endif

BOOL loadCfgDLL(void);

extern _SOXpSInitialize g_fpSOXpSInitialize;
extern _SOXpSStart g_fpSOXpSStart;
extern _SOXpSStop g_fpSOXpSStop;
extern _SOXpSBuildDANameSpace g_fpSOXpSBuildDANameSpace;
extern _SOXpSBuildAEAreaSpace g_fpSOXpSBuildAEAreaSpace;
extern _SOXpSBuildAEEventSpace g_fpSOXpSBuildAEEventSpace;
extern _SOXpSHandleDARequests g_fpSOXpSHandleDARequests;
extern _SOXpSActivateDAItems g_fpSOXpSActivateDAItems;
extern _SOXpSReceivedAEAcknowledge g_fpSOXpSReceivedAEAcknowledge;
extern _SOXpSTranslateToItemIDs g_fpSOXpSTranslateToItemIDs;
extern _SOXpSActivateAESubscription g_fpSOXpSActivateAESubscription;

extern HINSTANCE g_dllInstance;

extern SOCmnSync g_lockReceivedAEAcknowledge;
extern SOCmnSync g_lockTranslateToItemID;
extern SOCmnList<SOAeSAttributeValueDADescription> *g_translateToItemIDList;


#define SOXPS_IS_VALID_POINTER_TO_OBJECT(p, cls) \
		((p != NULL) && (!::IsBadReadPtr(p, sizeof(cls))))
#define SOXPS_IS_VALID_POINTER_TO_OBJECT_OR_NULL(p, cls) \
		((p == NULL) || (!::IsBadReadPtr(p, sizeof(cls))))
#define SOXPS_IS_VALID_POINTER_TO_SIZE(p, s) \
		((p != NULL) && (!::IsBadReadPtr(p, s)))
#define SOXPS_IS_VALID_POINTER(p) \
		((p != NULL) && (!::IsBadReadPtr(p, sizeof(*p))))
#define SOXPS_IS_VALID_POINTER_OR_NULL(p) \
		((p == NULL) || (!::IsBadReadPtr(p, sizeof(*p))))

#define SOXPS_IS_VALID_ARRAY(p, num) \
		( ((p == NULL) && (num == 0)) || (!::IsBadReadPtr(p, sizeof(*p) * num)) )

#define SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(p)	\
		((p == NULL) || ( (!::IsBadReadPtr(p, sizeof(*p))) && (!::IsBadWritePtr(p, sizeof(*p))) ))

#define SOXPS_INIT_WRITE_POINTER(p) \
		{ if (p) *p = NULL; }

#pragma pack(pop)
#endif
