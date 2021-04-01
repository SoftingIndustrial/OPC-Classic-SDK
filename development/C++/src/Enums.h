#ifndef _ENUMS_H_
#define _ENUMS_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

//  Declaration of the used Enums
typedef enum tagEnumResultCode
{
	EnumResultCode_S_OK =   0x00000000,
	EnumResultCode_S_FALSE = 0x00000001,
	EnumResultCode_E_NOTIMPL = 0x80004001,
	EnumResultCode_E_OUTOFMEMORY = 0x8007000E,
	EnumResultCode_E_INVALIDARG = 0x80070057,
	EnumResultCode_E_NOINTERFACE  = 0x80004002,
	EnumResultCode_E_ACCESSDENIED = 0x80070005,
	EnumResultCode_E_FAIL = 0x80004005,
	EnumResultCode_E_UNEXPECTED = 0x8000FFFF,
	EnumResultCode_CONNECT_E_NOCONNECTION = 0x80040200,
	EnumResultCode_CONNECT_E_ADVISELIMIT = 0x80040201,
	EnumResultCode_DISP_E_TYPEMISMATCH = 0x80020005,
	EnumResultCode_DISP_E_OVERFLOW = 0x8002000A,
	EnumResultCode_DISP_E_BADINDEX = 0x8002000B,
	EnumResultCode_E_OPC_INVALIDHANDLE = 0xC0040001,
	EnumResultCode_E_OPC_BADTYPE = 0xC0040004,
	EnumResultCode_E_OPC_PUBLIC = 0xC0040005,
	EnumResultCode_E_OPC_BADRIGHTS = 0xC0040006,
	EnumResultCode_E_OPC_UNKNOWNITEMID = 0xC0040007,
	EnumResultCode_E_OPC_INVALIDITEMID = 0xC0040008,
	EnumResultCode_E_OPC_INVALIDFILTER = 0xC0040009,
	EnumResultCode_E_OPC_UNKNOWNPATH   = 0xC004000A,
	EnumResultCode_E_OPC_RANGE = 0xC004000B,
	EnumResultCode_E_OPC_DUPLICATENAME = 0xC004000C,
	EnumResultCode_S_OPC_UNSUPPORTEDRATE =  0x0004000D,
	EnumResultCode_S_OPC_CLAMP = 0x0004000E,
	EnumResultCode_S_OPC_INUSE = 0x0004000F,
	EnumResultCode_E_OPC_INVALIDCONFIGFILE = 0xC0040010,
	EnumResultCode_E_OPC_NOTFOUND = 0xC0040011,
	EnumResultCode_E_OPC_INVALID_PID = 0xC0040203,
	EnumResultCode_E_OPC_DEADBANDNOTSET = 0xC0040400,
	EnumResultCode_E_OPC_DEADBANDNOTSUPPORTED = 0xC0040401,
	EnumResultCode_E_OPC_NOBUFFERING = 0xC0040402,
	EnumResultCode_E_OPC_INVALIDCONTINUATIONPOINT = 0xC0040403,
	EnumResultCode_S_OPC_DATAQUEUEOVERFLOW = 0x00040404,
	EnumResultCode_E_OPC_RATENOTSET = 0xC0040405,
	EnumResultCode_E_OPC_NOTSUPPORTED = 0xC0040406,
	EnumResultCode_E_EXCEPTION = 0x80070428
}   EnumResultCode;

typedef enum tagEnumFeature
{

	EnumFeature_DA_SERVER          = 0x01,
	EnumFeature_XMLDA_SERVER       = 0x02,
	EnumFeature_AE_SERVER          = 0x04,
	EnumFeature_TP_SERVER          = 0x08,

	EnumFeature_DA_CLIENT          = 0x10,
	EnumFeature_XMLDA_CLIENT       = 0x20,
	EnumFeature_AE_CLIENT          = 0x40,
	EnumFeature_TP_CLIENT          = 0x80

}   EnumFeature;

typedef enum tagEnumQuality
{

	EnumQuality_GOOD                            = 0xC0,
	EnumQuality_GOOD_LOCAL_OVERRIDE             = 0xD8,
	EnumQuality_BAD                             = 0x00,
	EnumQuality_BAD_CONFIG_ERROR                = 0x04,
	EnumQuality_BAD_NOT_CONNECTED               = 0x08,
	EnumQuality_BAD_DEVICE_FAILURE              = 0x0C,
	EnumQuality_BAD_SENSOR_FAILURE              = 0x10,
	EnumQuality_BAD_LAST_KNOWN                  = 0x14,
	EnumQuality_BAD_COMM_FAILURE                = 0x18,
	EnumQuality_BAD_OUT_OF_SERVICE              = 0x1C,
	EnumQuality_BAD_WAITING_FOR_INITIAL_DATA    = 0x20,
	EnumQuality_UNCERTAIN                       = 0x40,
	EnumQuality_UNCERTAIN_LAST_USABLE           = 0x44,
	EnumQuality_UNCERTAIN_SENSOR_CAL            = 0x50,
	EnumQuality_UNCERTAIN_EGU_EXCEEDED          = 0x54,
	EnumQuality_UNCERTAIN_EGU_EXCEEDED_LOW      = 0x55,
	EnumQuality_UNCERTAIN_EGU_EXCEEDED_HIGH     = 0x56,
	EnumQuality_UNCERTAIN_SUB_NORMAL            = 0x58,
	EnumQuality_QUALITY_NOT_SET                 = 0xFFFF
}   EnumQuality;


typedef enum tagEnumQualityLimit
{

	EnumQualityLimit_OK         = 0,
	EnumQualityLimit_LOW        = 1,
	EnumQualityLimit_HIGH       = 2,
	EnumQualityLimit_CONSTANT   = 3

}   EnumQualityLimit;


typedef enum tagEnumEUType
{

	EnumEUType_NOENUM       = 0,
	EnumEUType_ANALOG       = 1,
	EnumEUType_ENUMERATED   = 2

}   EnumEUType;


typedef enum tagEnumOPCSpecification
{

	EnumOPCSpecification_DEFAULT    = 0x00,
	EnumOPCSpecification_DA10       = 0x03,
	EnumOPCSpecification_DA20       = 0x04,
	EnumOPCSpecification_DA30       = 0x10,
	EnumOPCSpecification_XMLDA10    = 0x20,
	EnumOPCSpecification_AE10       = 0x08

}   EnumOPCSpecification;


// Accessrights for NameSpaceelements or their Properties
typedef enum tagEnumAccessRights
{

	//  OPC_READABLE : The client can only read the data item`s value
	EnumAccessRights_READABLE       = 0x01,

	//  OPC_WRITEABLE : The client can only change the data item`s value
	EnumAccessRights_WRITEABLE      = 0x02,

	// OPC_READWRITEABLE : Item can be read and changed by the client
	EnumAccessRights_READWRITEABLE  = 0x03

}   EnumAccessRights;

typedef enum tagEnumEventType
{
	EnumEventType_SIMPLE    = 0x01,
	EnumEventType_TRACKING  = 0x02,
	EnumEventType_CONDITION = 0x04
}   EnumEventType;


typedef enum tagEnumConditionState
{
	EnumConditionState_NO           = 0x00,
	EnumConditionState_ENABLED      = 0x01,
	EnumConditionState_ACTIVE       = 0x02,
	EnumConditionState_ACKED        = 0x04
}   EnumConditionState;


typedef enum tagEnumConditionChange
{

	EnumConditionChange_ACTIVE_STATE    = 0x0001,
	EnumConditionChange_ACK_STATE       = 0x0002,
	EnumConditionChange_ENABLE_STATE    = 0x0004,
	EnumConditionChange_STATE           = 0x0007,
	EnumConditionChange_QUALITY         = 0x0008,
	EnumConditionChange_SEVERITY        = 0x0010,
	EnumConditionChange_SUBCONDITION    = 0x0020,
	EnumConditionChange_MESSAGE         = 0x0040,
	EnumConditionChange_ATTRIBUTE       = 0x0080,
	EnumConditionChange_ACK_DATA        = 0x8000

}   EnumConditionChange;

typedef enum tagEnumPropertyId
{

	EnumPropertyId_ITEM_CANONICAL_DATATYPE  = 0x0001,
	EnumPropertyId_ITEM_VALUE               = 0x0002,
	EnumPropertyId_ITEM_QUALITY             = 0x0003,
	EnumPropertyId_ITEM_TIMESTAMP           = 0x0004,
	EnumPropertyId_ITEM_ACCESS_RIGHTS       = 0x0005,
	EnumPropertyId_SERVER_SCAN_RATE         = 0x0006,
	EnumPropertyId_ITEM_EU_TYPE             = 0x0007,
	EnumPropertyId_ITEM_EU_INFO             = 0x0008,
	EnumPropertyId_RESERVED_RANGE_1         = 0x0009,  // (9-99 dec)
	EnumPropertyId_EU_UNITS                 = 0x0064,
	EnumPropertyId_ITEM_DESCRIPTION         = 0x0065,
	EnumPropertyId_HIGH_EU                  = 0x0066,
	EnumPropertyId_LOW_EU                   = 0x0067,
	EnumPropertyId_HIGH_INSTRUMENT_RANGE    = 0x0068,
	EnumPropertyId_LOW_INSTRUMENT_RANGE     = 0x0069,
	EnumPropertyId_CONTACT_CLOSE_LABEL      = 0x006A,
	EnumPropertyId_CONTACT_OPEN_LABEL       = 0x006B,
	EnumPropertyId_ITEM_TIMEZONE            = 0x006C,
	EnumPropertyId_RESERVED_RANGE_2         = 0x006D,   // (109-199 dec)
	EnumPropertyId_CONDITION_STATUS         = 0x012C,
	EnumPropertyId_ALARM_QUICK_HELP         = 0x012D,
	EnumPropertyId_ALARM_AREA_LIST          = 0x012E,
	EnumPropertyId_ALARM_PRIMARY_AREA       = 0x012F,
	EnumPropertyId_CONDITION_LOGIC          = 0x0130,
	EnumPropertyId_LIMIT_EXCEEDED           = 0x0131,
	EnumPropertyId_DEADBAND                 = 0x0132,
	EnumPropertyId_HIHI_LIMIT               = 0x0133,
	EnumPropertyId_HI_LIMIT                 = 0x0134,
	EnumPropertyId_LO_LIMIT                 = 0x0135,
	EnumPropertyId_LOLO_LIMIT               = 0x0136,
	EnumPropertyId_RATE_OF_CHANGE_LIMIT     = 0x0137,
	EnumPropertyId_DEVIATION_LIMIT          = 0x0138,
	EnumPropertyId_SOUND_FILE               = 0x0139,
	EnumPropertyId_RESERVED_RANGE_3         = 0x0140,   // (314-399 dec) for AE
	EnumPropertyId_RESERVED_RANGE_4         = 0x0190    // (400-4999)
											  // Vendor specific property range >= 5000. Do not have to be sequential
}   EnumPropertyId;

//  Defines the Server possible states
typedef enum tagEnumServerState
{
    // The server is running normally	
	EnumServerState_RUNNING	= 1,
	
    // The server is no longer functioning
    EnumServerState_FAILED     = 2,
	
    // The server is running but no configuration information is loaded
    EnumServerState_NOCONFIG	= 3,
	
    // The server is suspended via some vendor specific method therefore cannot receive or send data.
    EnumServerState_SUSPENDED	= 4,
	
    // The server is running in Test Mode
    EnumServerState_TEST		= 5,
    
	// The server is running properly but is having difficulty accessing data from its data sources.	
    EnumServerState_COMM_FAULT = 6

}   EnumServerState;

#pragma pack(pop)

#endif  //  _ENUMS_H_
