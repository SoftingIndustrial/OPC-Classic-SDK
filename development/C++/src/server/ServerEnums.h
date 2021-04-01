#ifndef _SERVERENUMS_H_
#define _SERVERENUMS_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


namespace SoftingOPCToolboxServer
{

// Describes the data the DaAddressSpaceRoot::queryAddressSpaceElementData
// and Creator::createInternalDaAddressSpaceElementEx callbacks must provide.
typedef enum tagEnumAddressSpaceElementData
{
	// If set, OTSAddressSpaceElementData.m_name must be provided
	EnumElementData_NAME = 0x001,
	// If set, OTSAddressSpaceElementData.m_itemID must be provided
	EnumElementData_ITEMID = 0x002,
	// If set, OTSAddressSpaceElementData.m_accessRights must be provided
	EnumElementData_ACCESSRIGHTS = 0x004,
	// If set, OTSAddressSpaceElementData.m_ioMode must be provided
	EnumElementData_IOMODE = 0x008,
	// If set, OTSAddressSpaceElementData.m_hasChildren must be provided
	EnumElementData_HASCHILDREN = 0x010,
	// If set, OTSAddressSpaceElementData.m_isBrowsable must be provided
	EnumElementData_ISBROWSABLE = 0x020,
	// If set, OTSAddressSpaceElementData.m_userData must be provided
	EnumElementData_USERDATA = 0x040,
	// If set, OTSAddressSpaceElementData.m_dataType must be provided
	EnumElementData_DATATYPE = 0x080,
	// If set, OTSAddressSpaceElementData.m_elementType must be provided
	EnumElementData_ELEMENTTYPE = 0x100
} EnumAddressSpaceElementData;


typedef enum tagEnumAddressSpaceType
{

	//  The address space is based on objects
	EnumAddressSpaceType_OBJECT_BASED = 0x01,
	//  The address space consist of strings
	EnumAddressSpaceType_STRING_BASED = 0x02,
	//  The address space consist of objects and strings
	EnumAddressSpaceType_OBJECT_STRING_BASED = 0x03

}   EnumAddressSpaceType;


typedef enum tagEnumAddressSpaceElementType
{
	EnumAddressSpaceElementType_DA = 0x01,
	EnumAddressSpaceElementType_AE = 0x02
}   EnumAddressSpaceElementType;


// The Kind of Application
typedef enum tagEnumApplicationType
{

	//  The applications is an executable
	EnumApplicationType_EXECUTABLE  = 0x01,

	//  The applications is a library
	EnumApplicationType_LIBRARY     = 0x00

}   EnumApplicationType;

// Special cache expiry settings for the string-based (syntax-based) namespace object data.
typedef enum tagEnumStringBasedExpiryTimeout
{
	// Disabled
	EnumStringBasedExpiryTimeout_DISABLED = 0xFFFFFFFFUL,

	// Infinite (never expires)
	EnumStringBasedExpiryTimeout_INFINITE = 0x0UL
} EnumStringBasedExpiryTimeout;

//The way the IO is done
typedef enum tagEnumIoMode
{

	//  No IO
	EnumIoMode_NONE          = 0x00,

	//  Client driven
	EnumIoMode_POLL          = 0x01,

	//  Server reports changes
	EnumIoMode_REPORT        = 0x02,

	//  Polled own cache
	EnumIoMode_POLL_OWNCACHE = 0x11,

	//  Server reports changes for cyclic data
	EnumIoMode_REPORT_CYCLIC = 0x22

}   EnumIoMode;


//  The direction of the transaction
typedef enum tagEnumTransactionType
{

	//  The client asked for some value(s)
	EnumTransactionType_READ  = 0x01,
	//The client attempts to write some value(s)
	EnumTransactionType_WRITE = 0x02

}   EnumTransactionType;


//  Request's possible states
typedef enum tagEnumRequestState
{

	//  The request has just been created
	EnumRequestState_CREATED   = 0x01,
	//  The request is about to be processed
	EnumRequestState_PENDING   = 0x03,
	//  The request is being processed
	EnumRequestState_INPROGRESS   = 0x04,
	//  The request was processed and is now completed
	EnumRequestState_COMPLETED = 0x05

}   EnumRequestState;


// Type of a connected session
typedef enum tagEnumSessionType
{

	//  Data Access session
	EnumSessionType_DA = 0x01,

	//  The Data Access is over XML
	EnumSessionType_XMLDA = 0x06,

	//  Internal XML-DA subscription
	EnumSessionType_XMLSUBSCRIPTIONS = 0x02,

	//  Alarms and events session
	EnumSessionType_AE = 0x08

}   EnumSessionType;


//  Defines the DaSession's possible states
typedef enum tagEnumSessionState
{

	// Session created by the OPC client
	EnumSessionState_CREATE  =  0,

	//  session is loging on
	EnumSessionState_LOGON   =  1,

	//  session is logging off
	EnumSessionState_LOGOFF  =  2,

	//  the session got modified
	EnumSessionState_MODIFY  =  3,

	//  session is destroyed by the OPC client
	EnumSessionState_DESTROY = -1

}   EnumSessionState;

}   //  namespace SoftingOPCToolboxServer

#pragma pack(pop)
#endif  //  _SERVERENUMS_H_
