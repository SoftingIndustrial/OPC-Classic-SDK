#ifndef _SOCLT_ERROR_H_
#define _SOCLT_ERROR_H_

//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: SOCLT_ERR_COM_INIT
//
// MessageText:
//
// Failed to initialize DCOM runtime library.
//
#define SOCLT_ERR_COM_INIT               0xE00003E8L

//
// MessageId: SOCLT_ERR_CREATE_SERVER_BROWSER
//
// MessageText:
//
// Failed to create server browser object.
//
#define SOCLT_ERR_CREATE_SERVER_BROWSER  0xE00003E9L

//
// MessageId: SOCLT_ERR_ENUM_SERVERS
//
// MessageText:
//
// Failed to enumerate OPC servers.
//
#define SOCLT_ERR_ENUM_SERVERS           0xE00003EAL

//
// MessageId: SOCLT_ERR_GET_CLASS_DETAILS
//
// MessageText:
//
// Failed to get class details.
//
#define SOCLT_ERR_GET_CLASS_DETAILS      0xE00003EBL

//
// MessageId: SOCLT_ERR_GET_CLSID
//
// MessageText:
//
// Failed to get the CLSID of the server.
//
#define SOCLT_ERR_GET_CLSID              0xE00003ECL

//
// MessageId: SOCLT_ERR_ADVISE_SHUTDOWN
//
// MessageText:
//
// Failed to advise IOPCShutdown interface.
//
#define SOCLT_ERR_ADVISE_SHUTDOWN        0xE00003F2L

//
// MessageId: SOCLT_ERR_UNADVISE_SHUTDOWN
//
// MessageText:
//
// Failed to unadvise IOPCShutdown interface.
//
#define SOCLT_ERR_UNADVISE_SHUTDOWN      0xE00003F3L

//
// MessageId: SOCLT_ERR_EXCEPTION
//
// MessageText:
//
// Failed to unadvise IOPCShutdown interface.
//
#define SOCLT_ERR_EXCEPTION              0xE00003F4L

//
// MessageId: SOCLT_ERR_OUT_OF_MEMORY
//
// MessageText:
//
// Out of memory.
//
#define SOCLT_ERR_OUT_OF_MEMORY          0xE00003F5L

//
// MessageId: SOCLT_ERR_CREATE_SERVER
//
// MessageText:
//
// Failed to create OPC Server object.
//
#define SOCLT_ERR_CREATE_SERVER          0xE00003F6L

//
// MessageId: SOCLT_ERR_COM_QI
//
// MessageText:
//
// No interface.
//
#define SOCLT_ERR_COM_QI                 0xE00003F7L

//
// MessageId: SOCLT_ERR_SET_CLIENT_NAME
//
// MessageText:
//
// Failed to set client name.
//
#define SOCLT_ERR_SET_CLIENT_NAME        0xE00003F8L

//
// MessageId: SOCLT_ERR_SET_LOCALE_ID
//
// MessageText:
//
// Failed to set locale id.
//
#define SOCLT_ERR_SET_LOCALE_ID          0xE00003F9L

//
// MessageId: SOCLT_ERR_GET_LOCALE_ID
//
// MessageText:
//
// Failed to retrieve locale id.
//
#define SOCLT_ERR_GET_LOCALE_ID          0xE00003FAL

//
// MessageId: SOCLT_ERR_NO_PARENT_OBJECT
//
// MessageText:
//
// No reference to the parent object.
//
#define SOCLT_ERR_NO_PARENT_OBJECT       0xE00003FCL

//
// MessageId: SOCLT_ERR_PARENT_NOT_CONNECTED
//
// MessageText:
//
// Parent object not connected.
//
#define SOCLT_ERR_PARENT_NOT_CONNECTED   0xE00003FDL

//
// MessageId: SOCLT_ERR_DISCONNECT
//
// MessageText:
//
// Error disconnecting object.
//
#define SOCLT_ERR_DISCONNECT             0xE00003FEL

//
// MessageId: SOCLT_ERR_PAR_INPUT
//
// MessageText:
//
// Invalid input parameter.
//
#define SOCLT_ERR_PAR_INPUT              0xE00003FFL

//
// MessageId: SOCLT_ERR_PAR_OUTPUT
//
// MessageText:
//
// Invalid output parameter.
//
#define SOCLT_ERR_PAR_OUTPUT             0xE0000400L

//
// MessageId: SOCLT_ERR_SRV_GET_STATUS
//
// MessageText:
//
// Failed to retrieve server's status.
//
#define SOCLT_ERR_SRV_GET_STATUS         0xE0000401L

//
// MessageId: SODAC_ERR_SRV_QUERY_ORG
//
// MessageText:
//
// Cannot query namespace organization type.
//
#define SODAC_ERR_SRV_QUERY_ORG          0xE0000402L

//
// MessageId: SOCLT_ERR_BROWSE_UNEXPECTED
//
// MessageText:
//
// An unexpected error occured while browsing.
//
#define SOCLT_ERR_BROWSE_UNEXPECTED      0xE0000403L

//
// MessageId: SOCLT_ERR_ITEM_PROPS_NOT_SUPPORTED
//
// MessageText:
//
// Item properties are not supported.
//
#define SOCLT_ERR_ITEM_PROPS_NOT_SUPPORTED 0xE0000404L

//
// MessageId: SOCLT_ERR_SRV_QUERY_LCIDS
//
// MessageText:
//
// Faied to query available locale ids.
//
#define SOCLT_ERR_SRV_QUERY_LCIDS        0xE0000405L

//
// MessageId: SOCLT_ERR_GET_ERROR_STRING
//
// MessageText:
//
// Server was not able to deliver error message.
//
#define SOCLT_ERR_GET_ERROR_STRING       0xE0000406L

//
// MessageId: SOCLT_ERR_LOGON
//
// MessageText:
//
// Could not logon to the server.
//
#define SOCLT_ERR_LOGON                  0xE0000407L

//
// MessageId: SOCLT_ERR_LOGOFF
//
// MessageText:
//
// Could not logoff from the server.
//
#define SOCLT_ERR_LOGOFF                 0xE0000408L

//
// MessageId: SOCLT_ERR_ISAVAILABLEPRIV
//
// MessageText:
//
// Could not query for available security features.
//
#define SOCLT_ERR_ISAVAILABLEPRIV        0xE0000409L

//
// MessageId: SOCLT_ERR_BROWSE_NOTSUPPORTED
//
// MessageText:
//
// The called browse functionality is not supported by the OPC server.
//
#define SOCLT_ERR_BROWSE_NOTSUPPORTED    0xE000040AL

//////////////////////////////////////////////////////////////////////////////
//
// AEC - Errors
//
//////////////////////////////////////////////////////////////////////////////
//
// MessageId: SOAEC_ERR_ADD_SUBSCRIPTION
//
// MessageText:
//
// Failed to create event subscription.
//
#define SOAEC_ERR_ADD_SUBSCRIPTION       0xE000044CL

//
// MessageId: SOAEC_ERR_GET_FILTER
//
// MessageText:
//
// Failed to retrieve filter information.
//
#define SOAEC_ERR_GET_FILTER             0xE000044DL

//
// MessageId: SOAEC_ERR_SET_FILTER
//
// MessageText:
//
// Failed to set filter information.
//
#define SOAEC_ERR_SET_FILTER             0xE000044EL

//
// MessageId: SOAEC_ERR_SUBS_GET_ATTR
//
// MessageText:
//
// Failed to retrieve returned attributes for subscription.
//
#define SOAEC_ERR_SUBS_GET_ATTR          0xE000044FL

//
// MessageId: SOAEC_ERR_SUBS_SET_ATTR
//
// MessageText:
//
// Failed to select returned attributes for subscription.
//
#define SOAEC_ERR_SUBS_SET_ATTR          0xE0000450L

//
// MessageId: SOAEC_ERR_SUBS_GET_STATE
//
// MessageText:
//
// Failed to retrieve subscription state.
//
#define SOAEC_ERR_SUBS_GET_STATE         0xE0000451L

//
// MessageId: SOAEC_ERR_SUBS_SET_STATE
//
// MessageText:
//
// Failed to set subscription state.
//
#define SOAEC_ERR_SUBS_SET_STATE         0xE0000452L

//
// MessageId: SOAEC_ERR_TRANSLATE_TO_ITEM_IDS
//
// MessageText:
//
// Failed to translate to item ids.
//
#define SOAEC_ERR_TRANSLATE_TO_ITEM_IDS  0xE0000453L

//
// MessageId: SOAEC_ERR_GET_CONDITION_STATE
//
// MessageText:
//
// Failed to get condition state.
//
#define SOAEC_ERR_GET_CONDITION_STATE    0xE0000454L

//
// MessageId: SOAEC_ERR_CREATE_BROWSE_OBJECT
//
// MessageText:
//
// Failed to create browser object.
//
#define SOAEC_ERR_CREATE_BROWSE_OBJECT   0xE0000455L

//
// MessageId: SOAEC_ERR_REFRESH
//
// MessageText:
//
// Failed to refresh.
//
#define SOAEC_ERR_REFRESH                0xE0000456L

//
// MessageId: SOAEC_ERR_CANCEL_REFRESH
//
// MessageText:
//
// Failed to cancel refresh operation.
//
#define SOAEC_ERR_CANCEL_REFRESH         0xE0000457L

//
// MessageId: SOAEC_ERR_ENABLE_CONDITIONS
//
// MessageText:
//
// Enabling/Disabling condition failed.
//
#define SOAEC_ERR_ENABLE_CONDITIONS      0xE0000458L

//
// MessageId: SOAEC_ERR_REMOVE_SUBSCRIPTION
//
// MessageText:
//
// Failed to remove the event subscription.
//
#define SOAEC_ERR_REMOVE_SUBSCRIPTION    0xE0000459L

//////////////////////////////////////////////////////////////////////////////
//
// DAC - Errors
//
//////////////////////////////////////////////////////////////////////////////
//
// MessageId: SODAC_ERR_ADVISE_DATAOBJECT_READ
//
// MessageText:
//
// Failed to advise IDataObject read connection.
//
#define SODAC_ERR_ADVISE_DATAOBJECT_READ 0xE00004B0L

//
// MessageId: SODAC_ERR_ADVISE_DATAOBJECT_WRITE
//
// MessageText:
//
// Failed to advise IDataObject write connection.
//
#define SODAC_ERR_ADVISE_DATAOBJECT_WRITE 0xE00004B1L

//
// MessageId: SODAC_ERR_UNADVISE_DATAOBJECT
//
// MessageText:
//
// Failed to unadvise IDataObject connection.
//
#define SODAC_ERR_UNADVISE_DATAOBJECT    0xE00004B2L

//
// MessageId: SODAC_ERR_ADVISE_DATACALLBACK
//
// MessageText:
//
// Failed to advise IOPCDataCallback connection.
//
#define SODAC_ERR_ADVISE_DATACALLBACK    0xE00004B3L

//
// MessageId: SODAC_ERR_UNADVISE_DATACALLBACK
//
// MessageText:
//
// Failed to unadvise IOPCDataCallback connection.
//
#define SODAC_ERR_UNADVISE_DATACALLBACK  0xE00004B4L

//
// MessageId: SODAC_ERR_REMOVE_GROUP
//
// MessageText:
//
// Failed to remove group object.
//
#define SODAC_ERR_REMOVE_GROUP           0xE00004B5L

//
// MessageId: SODAC_ERR_ADD_GROUP
//
// MessageText:
//
// Failed to create group object.
//
#define SODAC_ERR_ADD_GROUP              0xE00004B6L

//
// MessageId: SODAC_ERR_GET_PUBGROUPBYNAME
//
// MessageText:
//
// Failed to get public group object by name.
//
#define SODAC_ERR_GET_PUBGROUPBYNAME     0xE00004B7L

//
// MessageId: SODAC_BAD_GROUP_CONNECTION_TYPE
//
// MessageText:
//
// Bad group connection type.
//
#define SODAC_BAD_GROUP_CONNECTION_TYPE  0xE00004B8L

//
// MessageId: SODAC_ERR_GRP_ADD_ITEMS
//
// MessageText:
//
// Cannot add items.
//
#define SODAC_ERR_GRP_ADD_ITEMS          0xE00004B9L

#define SODAC_ERR_ITM_ADD_ITEM									SODAC_ERR_GRP_ADD_ITEMS
//
// MessageId: SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS
//
// MessageText:
//
// Items client handle cannot be set.
//
#define SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS 0xE00004BAL

#define SODAC_ERR_ITM_SET_CLIENTHANDLE				 SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS
//
// MessageId: SODAC_ERR_GRP_REMOVE_ITEMS
//
// MessageText:
//
// Cannot remove items.
//
#define SODAC_ERR_GRP_REMOVE_ITEMS       0xE00004BBL

#define SODAC_ERR_ITM_REMOVE_ITEM						SODAC_ERR_GRP_REMOVE_ITEMS
//
// MessageId: SODAC_ERR_GRP_ACTIVATE_ITEMS
//
// MessageText:
//
// Cannot activate items.
//
#define SODAC_ERR_GRP_ACTIVATE_ITEMS     0xE00004BCL

#define SODAC_ERR_ITM_ACTIVATE_ITEM					SODAC_ERR_GRP_ACTIVATE_ITEMS
//
// MessageId: SODAC_ERR_GRP_DEACTIVATE_ITEMS
//
// MessageText:
//
// Cannot deactivate items.
//
#define SODAC_ERR_GRP_DEACTIVATE_ITEMS   0xE00004BDL

#define SODAC_ERR_ITM_DEACTIVATE_ITEM				SODAC_ERR_GRP_DEACTIVATE_ITEMS
//
// MessageId: SODAC_ERR_GRP_CREATE_ITEMENUM
//
// MessageText:
//
// Cannot create item enumerator object.
//
#define SODAC_ERR_GRP_CREATE_ITEMENUM    0xE00004BEL

//
// MessageId: SOCLT_ERR_DEMO_TIME
//
// MessageText:
//
// This is a demo version with a limited runtime.
//
#define SOCLT_ERR_DEMO_TIME              0xE00004C4L

//
// MessageId: SODAC_ERR_OPCDA_QI_REQ
//
// MessageText:
//
// Query for an obligatory OPC DA interface failed.
//
#define SODAC_ERR_OPCDA_QI_REQ           0xE00004C5L

//
// MessageId: SODAC_ERR_OPCDA_QI_REQ_V20
//
// MessageText:
//
// Query for an obligatory OPC DA 2.0 interface failed.
//
#define SODAC_ERR_OPCDA_QI_REQ_V20       0xE00004C6L

//
// MessageId: SODAC_ERR_OPCDA_QI_REQ_V10
//
// MessageText:
//
// Query for an obligatory OPC DA 1.0 interface failed.
//
#define SODAC_ERR_OPCDA_QI_REQ_V10       0xE00004C7L

//
// MessageId: SODAC_ERR_OPCDA_QI_OPT
//
// MessageText:
//
// Query for an optional OPC DA interface failed.
//
#define SODAC_ERR_OPCDA_QI_OPT           0xE00004C8L

//
// MessageId: SODAC_ERR_GRP_GET_STATE
//
// MessageText:
//
// Failed to retrieve group state.
//
#define SODAC_ERR_GRP_GET_STATE          0xE00004C9L

//
// MessageId: SODAC_ERR_GRP_SET_STATE
//
// MessageText:
//
// Failed to set group state.
//
#define SODAC_ERR_GRP_SET_STATE          0xE00004CAL

//
// MessageId: SODAC_ERR_OPCDA_QI_REQ_V30
//
// MessageText:
//
// Query for an obligatory OPC DA 3.0 interface failed.
//
#define SODAC_ERR_OPCDA_QI_REQ_V30       0xE00004CBL

//
// MessageId: SODAC_ERR_GRP_GET_KEEPALIVE
//
// MessageText:
//
// Failed to get group keep-alive time.
//
#define SODAC_ERR_GRP_GET_KEEPALIVE      0xE00004CCL

//
// MessageId: SODAC_ERR_GRP_SET_KEEPALIVE
//
// MessageText:
//
// Failed to set group keep-alive time.
//
#define SODAC_ERR_GRP_SET_KEEPALIVE      0xE00004CDL

//
// MessageId: SODAC_ERR_GRP_BAD_CLIENTHANDLE
//
// MessageText:
//
// OPC server returned bad client handle.
//
#define SODAC_ERR_GRP_BAD_CLIENTHANDLE   0xE00004CEL

//
// MessageId: SODAC_ERR_GRP_WRONG_ACTMODE
//
// MessageText:
//
// OPC server has wrong activation mode.
//
#define SODAC_ERR_GRP_WRONG_ACTMODE      0xE00004CFL

//
// MessageId: SODAC_ERR_GRP_GET_STATE_PUB
//
// MessageText:
//
// Failed to retrieve public group state.
//
#define SODAC_ERR_GRP_GET_STATE_PUB      0xE00004D1L

//
// MessageId: SODAC_ERR_GRP_SET_STATE_PUB
//
// MessageText:
//
// Failed to set public group state.
//
#define SODAC_ERR_GRP_SET_STATE_PUB      0xE00004D2L

//
// MessageId: SODAC_ERR_GRP_SET_NAME
//
// MessageText:
//
// Failed to set group name.
//
#define SODAC_ERR_GRP_SET_NAME           0xE00004D4L

//
// MessageId: SODAC_ERR_GRP_MOVE_TO_PUB
//
// MessageText:
//
// Cannot move group to public.
//
#define SODAC_ERR_GRP_MOVE_TO_PUB        0xE00004D5L

//
// MessageId: SODAC_ERR_GRP_SYNC_READ
//
// MessageText:
//
// Synchronous read of item(s) failed.
//
#define SODAC_ERR_GRP_SYNC_READ          0xE00004D8L

//
// MessageId: SODAC_ERR_GRP_SYNC_WRITE
//
// MessageText:
//
// Synchronous writing items failed.
//
#define SODAC_ERR_GRP_SYNC_WRITE         0xE00004D9L

//
// MessageId: SODAC_ERR_GRP_NO_ASYNC_CON
//
// MessageText:
//
// No connection advised for asynchronous operations.
//
#define SODAC_ERR_GRP_NO_ASYNC_CON       0xE00004DAL

//
// MessageId: SODAC_ERR_GRP_ASYNC_READ
//
// MessageText:
//
// Asynchronous reading items failed.
//
#define SODAC_ERR_GRP_ASYNC_READ         0xE00004DBL

//
// MessageId: SODAC_ERR_GRP_ASYNC_WRITE
//
// MessageText:
//
// Asynchronous writing items failed.
//
#define SODAC_ERR_GRP_ASYNC_WRITE        0xE00004DDL

//
// MessageId: SODAC_ERR_GRP_ASYNC_REFRESH
//
// MessageText:
//
// Asynchronous refreshing items failed.
//
#define SODAC_ERR_GRP_ASYNC_REFRESH      0xE00004DEL

//
// MessageId: SODAC_ERR_GRP_ASYNC_CANCEL
//
// MessageText:
//
// Asynchronous operation could not be canceled.
//
#define SODAC_ERR_GRP_ASYNC_CANCEL       0xE00004DFL

//
// MessageId: SODAC_ERR_GRP_ITM_GET_DEADBAND
//
// MessageText:
//
// Error get item deadband.
//
#define SODAC_ERR_GRP_ITM_GET_DEADBAND   0xE00004E0L

//
// MessageId: SODAC_ERR_GRP_ITM_SET_DEADBAND
//
// MessageText:
//
// Error set item deadband.
//
#define SODAC_ERR_GRP_ITM_SET_DEADBAND   0xE00004E1L

//
// MessageId: SODAC_ERR_GRP_ITM_CLEAR_DEADBAND
//
// MessageText:
//
// Error clear item deadband.
//
#define SODAC_ERR_GRP_ITM_CLEAR_DEADBAND 0xE00004E2L

//
// MessageId: SODAC_ERR_GRP_SET_ITEM_DATATYPES
//
// MessageText:
//
// Cannot set item(s) datatypes.
//
#define SODAC_ERR_GRP_SET_ITEM_DATATYPES 0xE00004E3L

#define SODAC_ERR_ITM_SET_DATATYPE					SODAC_ERR_GRP_SET_ITEM_DATATYPES
//
// MessageId: SODAC_ERR_GRP_VALIDATE_ITEMS
//
// MessageText:
//
// Item validation failed.
//
#define SODAC_ERR_GRP_VALIDATE_ITEMS     0xE00004E4L

#define SODAC_ERR_ITM_VALIDATE_ITEM				SODAC_ERR_GRP_VALIDATE_ITEMS
//
// MessageId: SODAC_ERR_GRP_GET_DEADBAND
//
// MessageText:
//
// Error get group deadband.
//
#define SODAC_ERR_GRP_GET_DEADBAND       0xE00004E5L

//
// MessageId: SODAC_INF_SRV_VERSION_10A
//
// MessageText:
//
// OPC server version 1.0A.
//
#define SODAC_INF_SRV_VERSION_10A        0x600004E4L

//
// MessageId: SODAC_INF_SRV_VERSION_20
//
// MessageText:
//
// OPC server version 2.0.
//
#define SODAC_INF_SRV_VERSION_20         0x600004E6L

//
// MessageId: SODAC_INF_SRV_VERSION_30
//
// MessageText:
//
// OPC server version 3.0.
//
#define SODAC_INF_SRV_VERSION_30         0x600004E7L

//
// MessageId: SODAC_ERR_SRV_CHANGE_BRS_POS
//
// MessageText:
//
// Can't change namespace browse position.
//
#define SODAC_ERR_SRV_CHANGE_BRS_POS     0xE00004EBL

//
// MessageId: SODAC_ERR_SRV_BRS_ITEMIDS
//
// MessageText:
//
// Can't browse OPC item identifiers.
//
#define SODAC_ERR_SRV_BRS_ITEMIDS        0xE00004ECL

//
// MessageId: SODAC_ERR_SRV_GET_ITEMID
//
// MessageText:
//
// Can't get fully qualified item name.
//
#define SODAC_ERR_SRV_GET_ITEMID         0xE00004EDL

//
// MessageId: SODAC_ERR_SRV_BRS_ACCESS_PATHS
//
// MessageText:
//
// Can't browse item's access path.
//
#define SODAC_ERR_SRV_BRS_ACCESS_PATHS   0xE00004EEL

//
// MessageId: SODAC_ERR_SRV_QUERY_PROPERTIES
//
// MessageText:
//
// Failed to retrieve item's available properties.
//
#define SODAC_ERR_SRV_QUERY_PROPERTIES   0xE00004EFL

//
// MessageId: SODAC_ERR_SRV_GET_PROP_VALUE
//
// MessageText:
//
// Failed to retrieve property values.
//
#define SODAC_ERR_SRV_GET_PROP_VALUE     0xE00004F0L

//
// MessageId: SODAC_ERR_SRV_GET_PROP_ITEMID
//
// MessageText:
//
// Failed to retrieve the property ids for item.
//
#define SODAC_ERR_SRV_GET_PROP_ITEMID    0xE00004F1L

//
// MessageId: SODAC_ERR_SRV_LOAD
//
// MessageText:
//
// Server's configuration file could not be loaded.
//
#define SODAC_ERR_SRV_LOAD               0xE00004F2L

//
// MessageId: SODAC_ERR_SRV_SAVE
//
// MessageText:
//
// Server's configuration file could not be saved.
//
#define SODAC_ERR_SRV_SAVE               0xE00004F3L

//
// MessageId: SODAC_ERR_SRV_READ_ITEM_VALUE
//
// MessageText:
//
// Failed to read at least one item value.
//
#define SODAC_ERR_SRV_READ_ITEM_VALUE    0xE00004F4L

//
// MessageId: SODAC_ERR_SRV_WRITE_ITEM_VALUE
//
// MessageText:
//
// Failed to write at least one item value
//
#define SODAC_ERR_SRV_WRITE_ITEM_VALUE   0xE00004F5L

//
// MessageId: SODAC_ERR_SRV_SAVE_COMPLETED
//
// MessageText:
//
// Saving server's configuration file could not be completed.
//
#define SODAC_ERR_SRV_SAVE_COMPLETED     0xE00004F6L

//
// MessageId: SODAC_ERR_SRV_GET_CUR_FILE
//
// MessageText:
//
// Failed to retrieve the path of the current configuration file.
//
#define SODAC_ERR_SRV_GET_CUR_FILE       0xE00004F7L

//
// MessageId: SODAC_ERR_NO_MAPPING_FOR_PROPERTIES
//
// MessageText:
//
// Mapping to item is not supported for this property.
//
#define SODAC_ERR_NO_MAPPING_FOR_PROPERTIES 0xA00004F8L

//
// MessageId: SODAC_ERR_SRV_GET_PROPERTIES
//
// MessageText:
//
// Failed to get properties.
//
#define SODAC_ERR_SRV_GET_PROPERTIES     0xE00004F9L

//
// MessageId: SODAC_ERR_SRV_BROWSE
//
// MessageText:
//
// Failed to browse.
//
#define SODAC_ERR_SRV_BROWSE             0xE00004FAL

//
// MessageId: SODAC_ERR_READANDDATACHANGE_CALLBACK
//
// MessageText:
//
// Error in the OnReadComplete or OnDataChange callback.
//
#define SODAC_ERR_READANDDATACHANGE_CALLBACK 0xE00004FBL

//
// MessageId: SODAC_ERR_WRITECOMPLETE_CALLBACK
//
// MessageText:
//
// Error in the OnWriteComplete callback.
//
#define SODAC_ERR_WRITECOMPLETE_CALLBACK 0xE00004FCL

//
// MessageId: SODAC_ERR_CANCELCOMPLETE_CALLBACK
//
// MessageText:
//
// Error in the OnCancelComplete callback.
//
#define SODAC_ERR_CANCELCOMPLETE_CALLBACK 0xE00004FDL

#endif  // _SOCLT_ERROR_H_

