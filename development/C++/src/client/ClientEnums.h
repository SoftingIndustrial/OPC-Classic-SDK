#ifndef _CLIENTENUMS_H_
#define _CLIENTENUMS_H_

#include "../Enums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{


// Describes the possible ways of executing operations.
typedef enum tagEnumExecutionType
{

	//The execution is synchronous.
	EnumExecutionType_SYNCHRONOUS   = 0x01,

	//The execution is asynchronous.
	EnumExecutionType_ASYNCHRONOUS  = 0x02

}   EnumExecutionType;

typedef enum tagEnumClassContext
{

	EnumClassContext_NOT_COM = 0x00000000,

	EnumClassContext_INPROC_SERVER = 0x00000001,

	EnumClassContext_LOCAL_SERVER  = 0x00000004,

	EnumClassContext_REMOTE_SERVER = 0x00000010,

	EnumClassContext_SERVER        = (EnumClassContext_INPROC_SERVER |
									  EnumClassContext_LOCAL_SERVER |
									  EnumClassContext_REMOTE_SERVER)

} EnumClassContext;

class ExecutionOptions
{

protected:

	EnumExecutionType m_executionType;
	unsigned long m_executionContext;

public:

	ExecutionOptions()
	{
		//implicit the execution is synchronous
		m_executionType    = EnumExecutionType_SYNCHRONOUS;
		m_executionContext = 0x00;
	} // end constructor

	ExecutionOptions(EnumExecutionType executionType, unsigned long executionContext)
	{
		m_executionType     = executionType;
		m_executionContext  = executionContext;
	} // end constructor

	~ExecutionOptions() {}

	EnumExecutionType getExecutionType()
	{
		return (EnumExecutionType)m_executionType;
	} // end getExecutionType

	void setExecutionType(EnumExecutionType aType)
	{
		m_executionType = aType;
	} // end getExecutionType



	unsigned long getExecutionContext()
	{
		return m_executionContext;
	} // end getExecutionType


	void setExecutionContext(unsigned long aContext)
	{
		m_executionContext = aContext;
	} // end getExecutionType

}; // end class ExecutionOptions



//Describes the current state of the connection with a server.
typedef enum tagEnumObjectState
{

	//Client is not connected to the server.
	EnumObjectState_DISCONNECTED    = 0x00,

	//Client is connected to the server but no data exchange is made between the
	//client and the server.
	EnumObjectState_CONNECTED       = 0x01,

	//Client and server communicate with each other via a callback connection.
	EnumObjectState_ACTIVATED       = 0x03

}   EnumObjectState;

//Describes the data retrieval mode of DA Client subscription/group
typedef enum tagEnumDataRetrievalMode
{
	// Asynchronous advises
	EnumDataRetrievalMode_ASYNCADVISE = 0x01,

	// Synchronous reads
	EnumDataRetrievalMode_SYNCREAD = 0x02,

	// Polled refresh
	EnumDataRetrievalMode_POLLEDREFRESH = 0x04
} EnumDataRetrievalMode;

//Describes possible OPC objects type.
typedef enum tagEnumObjectType
{

	//Data Access Server.
	EnumObjectType_DASESSION        = 0x80000000,

	//Data Access Subscription.
	EnumObjectType_DASUBSCRIPTION   = 0x40000000,

	//Data Access Item.
	EnumObjectType_DAITEM           = 0x20000000,

	//Alarms and Events Server.
	EnumObjectType_AESESSION        = 0x08000000,

	//Alarms and Events subscription.
	EnumObjectType_AESUBSCRIPTION   = 0x04000000

}   EnumObjectType;

//Describes an OPC object attributes.
typedef enum tagEnumObjectAttribute
{

	//All attributes of a Data Access Server.
	EnumObjectAttribute_DASESSION_ALL                       = (0x80000000 | 0x0000FFFF),

	//The client name attribute of a Data Access Server.
	EnumObjectAttribute_DASESSION_CLIENTNAME                = (0x80000000 | 0x00000001),

	//The locale identifier attribute of a Data Access Server.
	EnumObjectAttribute_DASESSION_LCID                      = (0x80000000 | 0x00000002),

	//The url attribute of a Data Access Server.
	EnumObjectAttribute_DASESSION_URL                       = (0x80000000 | 0x00000004),

	//The forced OPC specification of a Data Access Server.
	EnumObjectAttribute_DASESSION_FORCEDOPCSPECIFICATION    = (0x80000000 | 0x00000008),

	//The supported Opc specification of a Data Access Server.
	EnumObjectAttribute_DASESSION_SUPPORTEDOPCSPECIFICATION = (0x80000000 | 0x00000010),

	//The HTTP request timeout of the session.
	EnumObjectAttribute_DASESSION_REQUESTTIMEOUT            = (0x80000000 | 0x00000020),

	//Whether to perform initial read or not.
	EnumObjectAttribute_DASESSION_PERFORMINITIALREAD        = (0x80000000 | 0x00000040),

	//The execution context in which the Data Access Server is to be run.
	EnumObjectAttribute_DASESSION_CLASSCONTEXT              = (0x80000000 | 0x8000),

	//All attributes of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_ALL              = (0x40000000 | 0xFFFF),

	//The name attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_NAME             = (0x40000000 | 0x0001),

	//The update rate attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_UPDATERATE       = (0x40000000 | 0x0002),

	//The deadband attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_DEADBAND         = (0x40000000 | 0x0004),

	//The timebias attribute  of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_TIMEBIAS         = (0x40000000 | 0x0008),

	//The locale ID attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_LCID             = (0x40000000 | 0x0010),

	//The keep alive time attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_KEEPALIVETIME    = (0x40000000 | 0x0020),

	//The data retrieval mode attribute of a Data Access Subscription.
	EnumObjectAttribute_DASUBSCRIPTION_DATARETRIEVAL    = (0x40000000 | 0x0040),

	//All the attributes of a Data Access Item.
	EnumObjectAttribute_DAITEM_ALL                  = (0x20000000 | 0xFFFF),

	//The requested datatype attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_REQUESTED_DATATYPE   = (0x20000000 | 0x0001),

	//The deadband attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_DEADBAND             = (0x20000000 | 0x0002),

	//The native datatype attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_NATIVE_DATATYPE      = (0x20000000 | 0x0004),

	//The access rights attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_ACCESSRIGHTS         = (0x20000000 | 0x0008),

	//The Engineering Unit Type attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_EUTYPE               = (0x20000000 | 0x0010),

	//The Engineering Unit Information attribute of a Data Access Item.
	EnumObjectAttribute_DAITEM_EUINFO               = (0x20000000 | 0x0020),

	// The identifier of a Data Access Item.
	EnumObjectAttribute_DAITEM_ID                   = (0x20000000 | 0x0040),

	//The path of a Data Access Item.
	EnumObjectAttribute_DAITEM_PATH                 = (0x20000000 | 0x0080),

	//All attributes of an Alarms and Events Server.
	EnumObjectAttribute_AESESSION_ALL               = (0x08000000 | 0xFFFF),

	//The client name attribute of an Alarms and Events Server.
	EnumObjectAttribute_AESESSION_CLIENTNAME        = (0x08000000 | 0x0001),

	//The locale ID of an Alarms and Events Server.
	EnumObjectAttribute_AESESSION_LCID              = (0x08000000 | 0x0002),

	//The url of an Alarms and Events Server.
	EnumObjectAttribute_AESESSION_URL               = (0x08000000 | 0x0004),

	EnumObjectAttribute_AESESSION_CLASSCONTEXT      = (0x08000000 | 0x8000),

	//All the attributes of an Alarms and Events Subscription.
	EnumObjectAttribute_AESUBSCRIPTION_ALL                 = (0x04000000 | 0xFFFF),

	//The buffertime attribute of an Alarms and Events Subscription.
	EnumObjectAttribute_AESUBSCRIPTION_BUFFERTIME          = (0x04000000 | 0x0001),

	//The maxsize attribute of an Alarms and Events Subscription.
	EnumObjectAttribute_AESUBSCRIPTION_MAXSIZE             = (0x04000000 | 0x0002),

	//The filter attribute of an Alarms and Events Subscription.
	EnumObjectAttribute_AESUBSCRIPTION_FILTER              = (0x04000000 | 0x0004),

	//The ReturnedAttributtes attribute of an Alarms and Events Subscription.
	EnumObjectAttribute_AESUBSCRIPTION_RETURNED_ATTRIBUTES = (0x04000000 | 0x0008)

}   EnumObjectAttribute;

//Describes the data a server browser can provide.
typedef enum tagEnumServerBrowserData
{
	//A 128-bit value which uniquely identifies a server.
	EnumServerBrowserData_CLASSID                    = 0x01,

	//A 128-bit value which uniquely identifies a server.
	EnumServerBrowserData_PROGID                    = 0x02,

	//The server's version independent program ID.
	EnumServerBrowserData_PROGIDVERSIONINDEPENDENT  = 0x04,

	//A server description.
	EnumServerBrowserData_DESCRIPTION        = 0x08,

	//A server url.
	EnumServerBrowserData_URL                = 0x10,

	//The Opc specifications.
	EnumServerBrowserData_OPCSPECIFICATIONS = 0x20,

	//All the above data.
	EnumServerBrowserData_ALL               = 0xFF

}   EnumServerBrowserData;


//	Type of the client address space element
typedef enum tagEnumAddressSpaceElementType
{

	//  A node in the hierarchical structure of the namespace.It provides a logical
	//  grouping for the process variables.
	EnumAddressSpaceElementType_BRANCH       = 0x01,

	//  A process variable which can be accessed via an OPC server.
	EnumAddressSpaceElementType_LEAF         = 0x02,

	//  A process variable which can be accessed via the OPC Data Access Server .
	EnumAddressSpaceElementType_DA_ITEM      = 0x04,

	//  All the server's namespace elements.
	EnumAddressSpaceElementType_ALL          = 0xFF

}   EnumAddressSpaceElementType;

//Describes the data a property can provide.
typedef enum tagEnumPropertyData
{

	//No data.
	EnumPropertyData_NONE   = 0x00,

	//The value the property contains.
	EnumPropertyData_VALUE  = 0x01,

	//The ID of the item the property belongs to.
	EnumPropertyData_ITEMID = 0x02,

	// All property data.
	EnumPropertyData_ALL    = 0xFF

}   EnumPropertyData;

//Describes the way the update for the OPC object attributes is made.
typedef enum tagEnumUpdateAttributesBehavior
{

	//The update of the OPC object attributes is made automatically,
	//without the user to request it explicitly.
	EnumUpdateAttributesBehavior_IMPLICIT_UPDATE  = 0x01,

	// The update of the OPC object attributes is made at the user request.
	EnumUpdateAttributesBehavior_EXPLICIT_UPDATE  = 0x02

}   EnumUpdateAttributesBehavior;

typedef enum tagEnumFilterBy
{

	EnumFilterBy_EVENT     = 0x01,
	EnumFilterBy_CATEGORY  = 0x02,
	EnumFilterBy_SEVERITY  = 0x04,
	EnumFilterBy_AREA      = 0x08,
	EnumFilterBy_SOURCE    = 0x10

} EnumFilterBy;

};
#pragma pack(pop)
#endif  //  _SERVERENUMS_H_
