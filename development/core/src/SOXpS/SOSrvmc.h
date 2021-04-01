#ifndef __SOSRVMC_H
#define __SOSRVMC_H

// OPC AE
//
//  Values are 32 bit values layed out as follows:
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


//
// MessageId: SOSRV_S_ALREADYACKED
//
// MessageText:
//
//  The condition has already been acknowledged.%0
//
#define SOSRV_S_ALREADYACKED             ((HRESULT)0x02FF0200L)

//
// MessageId: SOSRV_S_INVALIDBUFFERTIME
//
// MessageText:
//
//  The buffer time parameter was invalid.%0
//
#define SOSRV_S_INVALIDBUFFERTIME        ((HRESULT)0x02FF0201L)

//
// MessageId: SOSRV_S_INVALIDMAXSIZE
//
// MessageText:
//
//  The max size parameter was invalid.%0
//
#define SOSRV_S_INVALIDMAXSIZE           ((HRESULT)0x02FF0202L)

// error codes
//
// MessageId: SOSRV_E_INVALIDBRANCHNAME
//
// MessageText:
//
//  The string was not recognized as an area name.%0
//
#define SOSRV_E_INVALIDBRANCHNAME        ((HRESULT)0xC2FF0203L)

//
// MessageId: SOSRV_E_INVALIDTIME
//
// MessageText:
//
//  The time does not match the latest active time.%0
//
#define SOSRV_E_INVALIDTIME              ((HRESULT)0xC2FF0204L)

//
// MessageId: SOSRV_E_BUSY
//
// MessageText:
//
//  A refresh is currently in progress.%0
//
#define SOSRV_E_BUSY                     ((HRESULT)0xC2FF0205L)

//
// MessageId: SOSRV_E_NOINFO
//
// MessageText:
//
//  Information is not available.%0
//
#define SOSRV_E_NOINFO                   ((HRESULT)0xC2FF0206L)

// OPC DA
//
// MessageId: SOSRV_E_INVALIDHANDLE
//
// MessageText:
//
//  The value of the handle is invalid.%0
//
#define SOSRV_E_INVALIDHANDLE            ((HRESULT)0xC1FF0001L)

//
// MessageId: SOSRV_E_BADTYPE
//
// MessageText:
//
//  The server cannot convert the data between the specified format and/or requested data type and the canonical data type.%0
//
#define SOSRV_E_BADTYPE                  ((HRESULT)0xC1FF0004L)

//
// MessageId: SOSRV_E_PUBLIC
//
// MessageText:
//
//  The requested operation cannot be done on a public group.%0
//
#define SOSRV_E_PUBLIC                   ((HRESULT)0xC1FF0005L)

//
// MessageId: SOSRV_E_BADRIGHTS
//
// MessageText:
//
//  The item's access rights do not allow the operation.%0
//
#define SOSRV_E_BADRIGHTS                ((HRESULT)0xC1FF0006L)

//
// MessageId: SOSRV_E_UNKNOWNITEMID
//
// MessageText:
//
//  The item ID is not defined in the server address space or no longer exists in the server address space.%0
//
#define SOSRV_E_UNKNOWNITEMID            ((HRESULT)0xC1FF0007L)

//
// MessageId: SOSRV_E_INVALIDITEMID
//
// MessageText:
//
//  The item definition does not conform to the server syntax.%0
//
#define SOSRV_E_INVALIDITEMID            ((HRESULT)0xC1FF0008L)

//
// MessageId: SOSRV_E_INVALIDFILTER
//
// MessageText:
//
//  The filter string was not valid.%0
//
#define SOSRV_E_INVALIDFILTER            ((HRESULT)0xC1FF0009L)

//
// MessageId: SOSRV_E_UNKNOWNPATH
//
// MessageText:
//
//  The item's access path is not known to the server.%0
//
#define SOSRV_E_UNKNOWNPATH              ((HRESULT)0xC1FF000AL)

//
// MessageId: SOSRV_E_RANGE
//
// MessageText:
//
//  The value was out of range.%0
//
#define SOSRV_E_RANGE                    ((HRESULT)0xC1FF000BL)

//
// MessageId: SOSRV_E_DUPLICATENAME
//
// MessageText:
//
//  Duplicate name not allowed.%0
//
#define SOSRV_E_DUPLICATENAME            ((HRESULT)0xC1FF000CL)

//
// MessageId: SOSRV_S_UNSUPPORTEDRATE
//
// MessageText:
//
//  The server does not support the requested data rate but will use the closest available rate.%0
//
#define SOSRV_S_UNSUPPORTEDRATE          ((HRESULT)0x01FF000DL)

//
// MessageId: SOSRV_S_CLAMP
//
// MessageText:
//
//  A value passed to write was accepted but the output was clamped.%0
//
#define SOSRV_S_CLAMP                    ((HRESULT)0x01FF000EL)

//
// MessageId: SOSRV_S_INUSE
//
// MessageText:
//
//  The operation cannot be performed because the object is bering referenced.%0
//
#define SOSRV_S_INUSE                    ((HRESULT)0x01FF000FL)

//
// MessageId: SOSRV_E_INVALIDCONFIGFILE
//
// MessageText:
//
//  The server's configuration file has an invalid format.%0
//
#define SOSRV_E_INVALIDCONFIGFILE        ((HRESULT)0xC1FF0010L)

//
// MessageId: SOSRV_E_NOTFOUND
//
// MessageText:
//
//  The requested object (e.g. a public group) was not found.%0
//
#define SOSRV_E_NOTFOUND                 ((HRESULT)0xC1FF0011L)

//
// MessageId: SOSRV_E_INVALID_PID
//
// MessageText:
//
//  The specified property ID is not valid for the item.%0
//
#define SOSRV_E_INVALID_PID              ((HRESULT)0xC1FF0203L)

//
// MessageId: SOSRV_E_DEADBANDNOTSET
//
// MessageText:
//
//  The item deadband has not been set for this item.%0
//
#define SOSRV_E_DEADBANDNOTSET           ((HRESULT)0xC1FF0400L)

//
// MessageId: SOSRV_E_DEADBANDNOTSUPPORTED
//
// MessageText:
//
//  The item does not support deadband.%0
//
#define SOSRV_E_DEADBANDNOTSUPPORTED     ((HRESULT)0xC1FF0401L)

//
// MessageId: SOSRV_E_NOBUFFERING
//
// MessageText:
//
//  The server does not support buffering of data items that are collected at a faster rate than the group update rate.%0
//
#define SOSRV_E_NOBUFFERING              ((HRESULT)0xC1FF0402L)

//
// MessageId: SOSRV_E_INVALIDCONTINUATIONPOINT
//
// MessageText:
//
//  The continuation point is not valid.%0
//
#define SOSRV_E_INVALIDCONTINUATIONPOINT ((HRESULT)0xC1FF0403L)

//
// MessageId: SOSRV_S_DATAQUEUEOVERFLOW
//
// MessageText:
//
//  Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.%0
//
#define SOSRV_S_DATAQUEUEOVERFLOW        ((HRESULT)0xC1FF0404L)

//
// MessageId: SOSRV_E_RATENOTSET
//
// MessageText:
//
//  There is no sampling rate set for the specified item.%0
//
#define SOSRV_E_RATENOTSET               ((HRESULT)0xC1FF0405L)

//
// MessageId: SOSRV_E_NOTSUPPORTED
//
// MessageText:
//
//  The server does not support writing of quality and/or timestamp.%0
//
#define SOSRV_E_NOTSUPPORTED             ((HRESULT)0xC1FF0406L)

// OPC XML DA
//
// MessageId: SOSRV_E_INVALIDHOLDTIME
//
// MessageText:
//
//  The hold time is too long (determined by server).%0
//
#define SOSRV_E_INVALIDHOLDTIME          ((HRESULT)0xC1FF5600L)

//
// MessageId: SOSRV_E_INVALIDITEMNAME
//
// MessageText:
//
//  The item name does not conform the server's syntax.%0
//
#define SOSRV_E_INVALIDITEMNAME          ((HRESULT)0xC1FF5601L)

//
// MessageId: SOSRV_E_INVALIDITEMPATH
//
// MessageText:
//
//  The item path does not conform the server's syntax.%0
//
#define SOSRV_E_INVALIDITEMPATH          ((HRESULT)0xC1FF5602L)

//
// MessageId: SOSRV_E_NOSUBSCRIPTION
//
// MessageText:
//
//  An invalid set of subscription handles was passed to the request.%0
//
#define SOSRV_E_NOSUBSCRIPTION           ((HRESULT)0xC1FF5603L)

//
// MessageId: SOSRV_E_READONLY
//
// MessageText:
//
//  The value is read only and may not be written to.%0
//
#define SOSRV_E_READONLY                 ((HRESULT)0xC1FF5604L)

//
// MessageId: SOSRV_E_WRITEONLY
//
// MessageText:
//
//  The value is write-only and may not be read from or returned as part of a write response.%0
//
#define SOSRV_E_WRITEONLY                ((HRESULT)0xC1FF5605L)

//
// MessageId: SOSRV_E_UNKNOWNITEMNAME
//
// MessageText:
//
//  The item name is no longer available in the server address space.%0
//
#define SOSRV_E_UNKNOWNITEMNAME          ((HRESULT)0xC1FF5606L)

//
// MessageId: SOSRV_E_UNKNOWNITEMPATH
//
// MessageText:
//
//  The item path is no longer available in the server address space.%0
//
#define SOSRV_E_UNKNOWNITEMPATH          ((HRESULT)0xC1FF5607L)

//
// MessageId: SOSRV_E_TIMEDOUT
//
// MessageText:
//
//  The operation took too long to complete (determined by server).
//
#define SOSRV_E_TIMEDOUT                 ((HRESULT)0xC1FF5608L)

//
// MessageId: SOSRV_E_SERVERSTATE
//
// MessageText:
//
//  The operation could not complete due to an abnormal server state.%0
//
#define SOSRV_E_SERVERSTATE              ((HRESULT)0xC1FF5609L)

//
// MessageId: SOSRV_E_ACCESS_DENIED
//
// MessageText:
//
//  The server denies access (read and/or write) to the specified item. This error is typically caused by Web Service security (e.g. globally disabled write capabilities).%0
//
#define SOSRV_E_ACCESS_DENIED            ((HRESULT)0xC1FF5610L)

//
// MessageId: SOSRV_E_BUSY
//
// MessageText:
//
//  The server is currenly processing another polled refresh for one or more of the subscriptions.%0
//
#define SOSRV_E_BUSY                     ((HRESULT)0xC1FF5611L)

