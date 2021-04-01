#ifndef _SOPROXYDEF_H_
#define _SOPROXYDEF_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


#define SOWATCH_SPACE_ROOT          _T("RT")
#define SOWATCH_SPACE_DA_OBJECT     _T("DO")
#define SOWATCH_SPACE_DA_NAMESPACE  _T("DN")
#define SOWATCH_SPACE_AE_OBJECT     _T("AO")
#define SOWATCH_SPACE_AE_AREASPACE  _T("AA")
#define SOWATCH_SPACE_AE_EVENTSPACE _T("AE")
#define SOWATCH_SPACE_CUSTOM        _T("CM")

#define SOWATCH_TYPE_DA_SERVER            _T("DSE")
#define SOWATCH_TYPE_DA_GROUP             _T("DGR")
#define SOWATCH_TYPE_DA_ITEM              _T("DIT")
#define SOWATCH_TYPE_DA_NODE              _T("DNO")
#define SOWATCH_TYPE_DA_TAG               _T("DTA")
#define SOWATCH_TYPE_DA_PROPERTY          _T("DPR")
#define SOWATCH_TYPE_DA_OBJECTROOT        _T("DOR")
#define SOWATCH_TYPE_DA_NAMESPACEROOT     _T("DNS")
#define SOWATCH_TYPE_AE_SERVER            _T("ASE")
#define SOWATCH_TYPE_AE_SUBSCRIPTION      _T("ASU")
#define SOWATCH_TYPE_AE_AREABROWSER       _T("AAB")
#define SOWATCH_TYPE_AE_AREA              _T("AAR")
#define SOWATCH_TYPE_AE_SOURCE            _T("ASO")
#define SOWATCH_TYPE_AE_CONDITION         _T("ACO")
#define SOWATCH_TYPE_AE_CATEGORY          _T("ACA")
#define SOWATCH_TYPE_AE_ATTRIBUTE         _T("AAT")
#define SOWATCH_TYPE_AE_ATTRIBUTEVALUE    _T("AAV")
#define SOWATCH_TYPE_AE_CONDITIONCLASS    _T("ACC")
#define SOWATCH_TYPE_AE_SUBCONDITIONCLASS _T("ASC")
#define SOWATCH_TYPE_AE_OBJECTROOT        _T("AOR")
#define SOWATCH_TYPE_AE_AREASPACEROOT     _T("AAS")
#define SOWATCH_TYPE_AE_EVENTSPACEROOT    _T("AES")
#define SOWATCH_TYPE_CM_ROOT              _T("CMR")
#define SOWATCH_TYPE_CM_BRANCH            _T("CMB")
#define SOWATCH_TYPE_CM_LEAF              _T("CML")
#define SOWATCH_TYPE_CM_ADDITIONAL        _T("CMA")

#define SOWATCH_ELEMENT_SPACE_T         _T("sp")
#define SOWATCH_ELEMENT_SPACE_L         L"sp"
#define SOWATCH_ELEMENT_TYPE_T          _T("ty")
#define SOWATCH_ELEMENT_TYPE_L          L"ty"
#define SOWATCH_ELEMENT_SERVERHANDLE_T  _T("sh")
#define SOWATCH_ELEMENT_SERVERHANDLE_L  L"sh"
#define SOWATCH_ELEMENT_CLIENTHANDLE_T  _T("ch")
#define SOWATCH_ELEMENT_CLIENTHANDLE_L  L"ch"
#define SOWATCH_ELEMENT_OBJECT_T        _T("ob")
#define SOWATCH_ELEMENT_OBJECT_L        L"ob"
#define SOWATCH_ELEMENT_PARENT_T        _T("pa")
#define SOWATCH_ELEMENT_PARENT_L        L"pa"
#define SOWATCH_ELEMENT_OBJECTS_T       _T("obs")
#define SOWATCH_ELEMENT_OBJECTS_L       L"obs"
#define SOWATCH_ELEMENT_ATTRIBUTES_T    _T("ats")
#define SOWATCH_ELEMENT_ATTRIBUTES_L    L"ats"
#define SOWATCH_ELEMENT_CHANGE_T        _T("ch")
#define SOWATCH_ELEMENT_CHANGE_L        L"ch"
#define SOWATCH_ELEMENT_ADD_T           _T("ad")
#define SOWATCH_ELEMENT_ADD_L           L"ad"
#define SOWATCH_ELEMENT_REMOVE_T        _T("re")
#define SOWATCH_ELEMENT_REMOVE_L        L"re"
#define SOWATCH_ELEMENT_CHANGES_T       _T("chs")
#define SOWATCH_ELEMENT_CHANGES_L       L"chs"
#define SOWATCH_ELEMENT_STATISTICS_T    _T("sta")
#define SOWATCH_ELEMENT_STATISTICS_L    L"sta"
#define SOWATCH_ATTR_RAW_T              _T("raw")
#define SOWATCH_ATTR_RAW_L              L"raw"

#define SOWATCH_ATTRIBUTE_DEVSTATE_T     _T("devState")
#define SOWATCH_ATTRIBUTE_DEVSTATE_L     L"devState"
#define SOWATCH_ATTRIBUTE_DEVSTATEINFO_T _T("devStateInfo")
#define SOWATCH_ATTRIBUTE_DEVSTATEINFO_L L"devStateInfo"
#define SOWATCH_ATTRIBUTE_DESCRIPTION_T  _T("description")
#define SOWATCH_ATTRIBUTE_DESCRIPTION_L  L"description"
#define SOWATCH_ATTRIBUTE_VALUE_T        _T("value")
#define SOWATCH_ATTRIBUTE_VALUE_L        L"value"
#define SOWATCH_ATTRIBUTE_QUALITY_T      _T("quality")
#define SOWATCH_ATTRIBUTE_QUALITY_L      L"quality"
#define SOWATCH_ATTRIBUTE_DATATYPE_T     _T("datatype")
#define SOWATCH_ATTRIBUTE_DATATYPE_L     L"datatype"
#define SOWATCH_ATTRIBUTE_TIMESTAMP_T    _T("timeStamp")
#define SOWATCH_ATTRIBUTE_TIMESTAMP_L    L"timeStamp"
#define SOWATCH_ATTRIBUTE_ACCESSRIGHTS_T _T("accessRights")
#define SOWATCH_ATTRIBUTE_ACCESSRIGHTS_L L"accessRights"
#define SOWATCH_ATTRIBUTE_EUTYPE_T       _T("euType")
#define SOWATCH_ATTRIBUTE_EUTYPE_L       L"euType"
#define SOWATCH_ATTRIBUTE_EUINFO_T       _T("euInfo")
#define SOWATCH_ATTRIBUTE_EUINFO_L       L"euInfo"
#define SOWATCH_ATTRIBUTE_PROPERTYID_T   _T("propertyID")
#define SOWATCH_ATTRIBUTE_PROPERTYID_L   L"propertyID"
#define SOWATCH_ATTRIBUTE_CLSID_T        _T("CLSID")
#define SOWATCH_ATTRIBUTE_CLSID_L        L"CLSID"
#define SOWATCH_ATTRIBUTE_VENDORINFO_T   _T("vendorInfo")
#define SOWATCH_ATTRIBUTE_VENDORINFO_L   L"vendorInfo"
#define SOWATCH_ATTRIBUTE_VERSION_T      _T("version")
#define SOWATCH_ATTRIBUTE_VERSION_L      L"version"
#define SOWATCH_ATTRIBUTE_STARTTIME_T    _T("startTime")
#define SOWATCH_ATTRIBUTE_STARTTIME_L    L"startTime"
#define SOWATCH_ATTRIBUTE_CLIENTNAME_T   _T("clientName")
#define SOWATCH_ATTRIBUTE_CLIENTNAME_L   L"clientName"
#define SOWATCH_ATTRIBUTE_LCID_T         _T("LCID")
#define SOWATCH_ATTRIBUTE_LCID_L         L"LCID"
#define SOWATCH_ATTRIBUTE_SERVERSTATE_T  _T("serverState")
#define SOWATCH_ATTRIBUTE_SERVERSTATE_L  L"serverState"
#define SOWATCH_ATTRIBUTE_LASTUPDATETIME_T _T("lastUpdateTime")
#define SOWATCH_ATTRIBUTE_LASTUPDATETIME_L L"lastUpdateTime"
#define SOWATCH_ATTRIBUTE_BANDWIDTH_T    _T("bandwidth")
#define SOWATCH_ATTRIBUTE_BANDWIDTH_L    L"bandwidth"
#define SOWATCH_ATTRIBUTE_NAME_T         _T("name")
#define SOWATCH_ATTRIBUTE_NAME_L         L"name"
#define SOWATCH_ATTRIBUTE_PATH_T         _T("path")
#define SOWATCH_ATTRIBUTE_PATH_L         L"path"
#define SOWATCH_ATTRIBUTE_ACTIVE_T       _T("active")
#define SOWATCH_ATTRIBUTE_ACTIVE_L       L"active"
#define SOWATCH_ATTRIBUTE_UPDATERATE_T   _T("updateRate")
#define SOWATCH_ATTRIBUTE_UPDATERATE_L   L"updateRate"
#define SOWATCH_ATTRIBUTE_KEEPALIVETIME_T _T("keepAliveTime")
#define SOWATCH_ATTRIBUTE_KEEPALIVETIME_L L"keepAliveTime"
#define SOWATCH_ATTRIBUTE_PINGRATE_T     _T("pingRate")
#define SOWATCH_ATTRIBUTE_PINGRATE_L     L"pingRate"
#define SOWATCH_ATTRIBUTE_TIMEBIAS_T     _T("timeBias")
#define SOWATCH_ATTRIBUTE_TIMEBIAS_L     L"timeBias"
#define SOWATCH_ATTRIBUTE_DEADBAND_T     _T("deadBand")
#define SOWATCH_ATTRIBUTE_DEADBAND_L     L"deadBand"
#define SOWATCH_ATTRIBUTE_SERVERHANDLE_T _T("serverHandle")
#define SOWATCH_ATTRIBUTE_SERVERHANDLE_L L"serverHandle"
#define SOWATCH_ATTRIBUTE_CLIENTHANDLE_T _T("clientHandle")
#define SOWATCH_ATTRIBUTE_CLIENTHANDLE_L L"clientHandle"
#define SOWATCH_ATTRIBUTE_ENABLE_T       _T("enable")
#define SOWATCH_ATTRIBUTE_ENABLE_L       L"enable"
#define SOWATCH_ATTRIBUTE_PUBLIC_T       _T("public")
#define SOWATCH_ATTRIBUTE_PUBLIC_L       L"public"
#define SOWATCH_ATTRIBUTE_ITEMID_T       _T("itemID")
#define SOWATCH_ATTRIBUTE_ITEMID_L       L"itemID"
#define SOWATCH_ATTRIBUTE_HASPROPERTIES_T  _T("hasProperties")
#define SOWATCH_ATTRIBUTE_HASPROPERTIES_L  L"hasProperties"
#define SOWATCH_ATTRIBUTE_ACCESSPATH_T     _T("accessPath")
#define SOWATCH_ATTRIBUTE_ACCESSPATH_L     L"accessPath"
#define SOWATCH_ATTRIBUTE_PATHDELIMITER_T  _T("pathDelimiter")
#define SOWATCH_ATTRIBUTE_PATHDELIMITER_L  L"pathDelimiter"
#define SOWATCH_ATTRIBUTE_PROPERTYDELIMITER_T  _T("propertyDelimiter")
#define SOWATCH_ATTRIBUTE_PROPERTYDELIMITER_L  L"propertyDelimiter"
#define SOWATCH_ATTRIBUTE_AVAILABLEFILTERS_T _T("availableFilters")
#define SOWATCH_ATTRIBUTE_AVAILABLEFILTERS_L L"availableFilters"
#define SOWATCH_ATTRIBUTE_MAXSIZE_T        _T("maxSize")
#define SOWATCH_ATTRIBUTE_MAXSIZE_L        L"maxSize"
#define SOWATCH_ATTRIBUTE_BUFFERTIME_T     _T("bufferTime")
#define SOWATCH_ATTRIBUTE_BUFFERTIME_L     L"bufferTime"
#define SOWATCH_ATTRIBUTE_SELECTEDATTRIBUTES_T _T("selectedAttributes")
#define SOWATCH_ATTRIBUTE_SELECTEDATTRIBUTES_L L"selectedAttributes"
#define SOWATCH_ATTRIBUTE_FILTERSEVERITY_T _T("filterSeverity")
#define SOWATCH_ATTRIBUTE_FILTERSEVERITY_L L"filterSeverity"
#define SOWATCH_ATTRIBUTE_FILTERAREA_T     _T("filterArea")
#define SOWATCH_ATTRIBUTE_FILTERAREA_L     L"filterArea"
#define SOWATCH_ATTRIBUTE_FILTERSOURCE_T   _T("filterSource")
#define SOWATCH_ATTRIBUTE_FILTERSOURCE_L   L"filterSource"
#define SOWATCH_ATTRIBUTE_FILTERTYPE_T     _T("filterType")
#define SOWATCH_ATTRIBUTE_FILTERTYPE_L     L"filterType"
#define SOWATCH_ATTRIBUTE_FILTERCATEGORY_T _T("filterCategory")
#define SOWATCH_ATTRIBUTE_FILTERCATEGORY_L L"filterCategory"
#define SOWATCH_ATTRIBUTE_SUBCONDITION_T   _T("subCondition")
#define SOWATCH_ATTRIBUTE_SUBCONDITION_L   L"subCondition"
#define SOWATCH_ATTRIBUTE_MESSAGE_T        _T("message")
#define SOWATCH_ATTRIBUTE_MESSAGE_L        L"message"
#define SOWATCH_ATTRIBUTE_SEVERITY_T       _T("severity")
#define SOWATCH_ATTRIBUTE_SEVERITY_L       L"severity"
#define SOWATCH_ATTRIBUTE_QUALITY_T        _T("quality")
#define SOWATCH_ATTRIBUTE_QUALITY_L        L"quality"
#define SOWATCH_ATTRIBUTE_ACTIVETIME_T     _T("activeTime")
#define SOWATCH_ATTRIBUTE_ACTIVETIME_L     L"activeTime"
#define SOWATCH_ATTRIBUTE_INACTIVETIME_T   _T("inactiveTime")
#define SOWATCH_ATTRIBUTE_INACTIVETIME_L   L"inactiveTime"
#define SOWATCH_ATTRIBUTE_ACTSUBCONDITIONTIME_T _T("activeSubConditionTime")
#define SOWATCH_ATTRIBUTE_ACTSUBCONDITIONTIME_L L"activeSubConditionTime"
#define SOWATCH_ATTRIBUTE_ACKTIME_T        _T("ackTime")
#define SOWATCH_ATTRIBUTE_ACKTIME_L        L"ackTime"
#define SOWATCH_ATTRIBUTE_ACKID_T          _T("ackID")
#define SOWATCH_ATTRIBUTE_ACKID_L          L"ackID"
#define SOWATCH_ATTRIBUTE_ACKCOMMENT_T     _T("ackComment")
#define SOWATCH_ATTRIBUTE_ACKCOMMENT_L     L"ackComment"
#define SOWATCH_ATTRIBUTE_STATE_T          _T("state")
#define SOWATCH_ATTRIBUTE_STATE_L          L"state"
#define SOWATCH_ATTRIBUTE_ID_T             _T("id")
#define SOWATCH_ATTRIBUTE_ID_L             L"id"
#define SOWATCH_ATTRIBUTE_EVENTTYPE_T      _T("eventType")
#define SOWATCH_ATTRIBUTE_EVENTTYPE_L      L"eventType"
#define SOWATCH_ATTRIBUTE_DEFINITION_T     _T("definition")
#define SOWATCH_ATTRIBUTE_DEFINITION_L     L"definition"
#define SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVEL_T _T("authentificationLevel")
#define SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVEL_L L"authentificationLevel"
#define SOWATCH_ATTRIBUTE_IMPERSONATIONLEVEL_T _T("impersonationLevel")
#define SOWATCH_ATTRIBUTE_IMPERSONATIONLEVEL_L L"impersonationLevel"
#define SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVELCB_T _T("authentificationLevelForCallbacks")
#define SOWATCH_ATTRIBUTE_AUTHENTIFICATIONLEVELCB_L L"authentificationLevelForCallbacks"
#define SOWATCH_ATTRIBUTE_IMPERSONATIONLEVELCB_T _T("impersonationLevelForCallbacks")
#define SOWATCH_ATTRIBUTE_IMPERSONATIONLEVELCB_L L"impersonationLevelForCallbacks"
#define SOWATCH_ATTRIBUTE_IDENTITY_T       _T("identity")
#define SOWATCH_ATTRIBUTE_IDENTITY_L       L"identity"
#define SOWATCH_ATTRIBUTE_SERVICENAME_T    _T("serviceName")
#define SOWATCH_ATTRIBUTE_SERVICENAME_L    L"serviceName"
#define SOWATCH_ATTRIBUTE_ENABLEDDCOM_T    _T("enabledDCOM")
#define SOWATCH_ATTRIBUTE_ENABLEDDCOM_L    L"enabledDCOM"

#define SOWATCH_STAT_STATISTICSSTARTTIME _T("StartTime")
#define SOWATCH_STAT_DA_NRCYCREAD        _T("DA-nrCycRead")
#define SOWATCH_STAT_DA_NRCHANGEREPORT   _T("DA-nrChangeReport")
#define SOWATCH_STAT_DA_NRADEVREAD       _T("DA-nrADevRead")
#define SOWATCH_STAT_DA_NRACACHEREAD     _T("DA-nrACacheRead")
#define SOWATCH_STAT_DA_NRSDEVREAD       _T("DA-nrSDevRead")
#define SOWATCH_STAT_DA_NRSCACHEREAD     _T("DA-nrSCacheRead")
#define SOWATCH_STAT_DA_NRAWRITE         _T("DA-nrAWrite")
#define SOWATCH_STAT_DA_NRSWRITE         _T("DA-nrSWrite")
#define SOWATCH_STAT_DA_NRREADFAILED     _T("DA-nrReadFailed")
#define SOWATCH_STAT_DA_NRWRITEFAILED    _T("DA-nrWriteFailed")
#define SOWATCH_STAT_DA_AVREADTIME       _T("DA-avReadTime")
#define SOWATCH_STAT_DA_AVWRITETIME      _T("DA-avWriteTime")
#define SOWATCH_STAT_DA_MAXREADTIME      _T("DA-maxReadTime")
#define SOWATCH_STAT_DA_MAXWRITETIME     _T("DA-maxWriteTime")
#define SOWATCH_STAT_DA_NRSERVEROBJ      _T("DA-nrServerObj")
#define SOWATCH_STAT_DA_NRGROUPOBJ       _T("DA-nrGroupObj")
#define SOWATCH_STAT_DA_NRITEMOBJ        _T("DA-nrItemObj")
#define SOWATCH_STAT_DA_NRNODEOBJ        _T("DA-nrNodeObj")
#define SOWATCH_STAT_DA_NRTAGOBJ         _T("DA-nrTagObj")
#define SOWATCH_STAT_DA_NRPROPERTYOBJ    _T("DA-nrPropertyObj")
#define SOWATCH_STAT_AE_NRSIMPLEEVENTS   _T("AE-nrSimpleEvents")
#define SOWATCH_STAT_AE_NRTRACKINGEVENTS _T("AE-nrTrackingEvents")
#define SOWATCH_STAT_AE_NRCONDITIONALEVENTS _T("AE-nrConditionalEvents")
#define SOWATCH_STAT_AE_NRSERVEROBJ      _T("AE-nrServerObj")
#define SOWATCH_STAT_AE_NRSUBSCRIPTIONOBJ _T("AE-nrSubscriptionObj")
#define SOWATCH_STAT_AE_NRAREABROWSEROBJ _T("AE-nrAreaBrowserObj")
#define SOWATCH_STAT_AE_NRAREAOBJ        _T("AE-nrAreaObj")
#define SOWATCH_STAT_AE_NRSOURCEOBJ      _T("AE-nrSourceObj")
#define SOWATCH_STAT_AE_NRCONDITIONOBJ   _T("AE-nrConditionObj")
#define SOWATCH_STAT_AE_NRATTRIBUTEVALUEOBJ _T("AE-nrAttributeValueObj")
#define SOWATCH_STAT_AE_NRCATEGORYOBJ    _T("AE-nrCategoryObj")
#define SOWATCH_STAT_AE_NRATTRIBUTEOBJ   _T("AE-nrAttributeObj")
#define SOWATCH_STAT_AE_NRCONDITIONCLASSOBJ _T("AE-nrConditionClassObj")
#define SOWATCH_STAT_AE_NRSUBCONDITIONCLASSOBJ _T("AE-nrSubConditionClassObj")
#define SOWATCH_STAT_AE_NRNACTNACKCOND   _T("AE-nrNActNAckCond")
#define SOWATCH_STAT_AE_NRACTNACKCOND    _T("AE-nrActNAckCond")
#define SOWATCH_STAT_AE_NRACTACKCOND     _T("AE-nrActAckCond")

#define SOWATCH_COLLECT_ADD                0x00000001
#define SOWATCH_COLLECT_REMOVE             0xFFFFFFFF
#define SOWATCH_COLLECT_ADD_ADDITIONAL     0x00000002
#define SOWATCH_COLLECT_REMOVE_ADDITIONAL  0xFFFFFFFE

#define SOWATCH_DEVSTATE_OK     _T("ok")
#define SOWATCH_DEVSTATE_ERROR  _T("error")


#define SOTRACE_ELEMENT_OPTIONS_T       _T("op")
#define SOTRACE_ELEMENT_OPTIONS_L       L"op"
#define SOTRACE_ELEMENT_LEVEL_T         _T("level")
#define SOTRACE_ELEMENT_LEVEL_L         L"level"
#define SOTRACE_ELEMENT_ERR_T           _T("err")
#define SOTRACE_ELEMENT_ERR_L           L"err"
#define SOTRACE_ELEMENT_WRN_T           _T("wrn")
#define SOTRACE_ELEMENT_WRN_L           L"wrn"
#define SOTRACE_ELEMENT_INF_T           _T("inf")
#define SOTRACE_ELEMENT_INF_L           L"inf"
#define SOTRACE_ELEMENT_DBG_T           _T("dbg")
#define SOTRACE_ELEMENT_DBG_L           L"dbg"
#define SOTRACE_ELEMENT_FILE_T          _T("file")
#define SOTRACE_ELEMENT_FILE_L          L"file"
#define SOTRACE_ELEMENT_NAME_T          _T("name")
#define SOTRACE_ELEMENT_NAME_L          L"name"
#define SOTRACE_ELEMENT_NAME2_T         _T("name2")
#define SOTRACE_ELEMENT_NAME2_L         L"name2"
#define SOTRACE_ELEMENT_SIZE_T          _T("size")
#define SOTRACE_ELEMENT_SIZE_L          L"size"
#define SOTRACE_ELEMENT_ENABLE_T        _T("enable")
#define SOTRACE_ELEMENT_ENABLE_L        L"enable"
#define SOTRACE_ELEMENT_BUFFER_T        _T("buffer")
#define SOTRACE_ELEMENT_BUFFER_L        L"buffer"
#define SOTRACE_ELEMENT_TRACE_T         _T("trc")
#define SOTRACE_ELEMENT_TRACE_L         L"trc"
#define SOTRACE_ELEMENT_LINE_T          _T("ln")
#define SOTRACE_ELEMENT_LINE_L          L"ln"

#pragma pack(pop)
#endif
