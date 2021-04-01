#ifndef _MBTERROR_H_
#define _MBTERROR_H_


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
//      ERROR 0xEF01....
//
//      S_OK = 0

#define MBT_ERROR_PREFIX ((DWORD) 0xEF010000)


#define MBT_THREAD_CREATION_ERROR     (MBT_ERROR_PREFIX | 0UL)
#define MBT_EXIT_TIMEOUT_ERROR        (MBT_ERROR_PREFIX | 1UL)
#define MBT_UNKNOWN_THREAD_EXIT_ERROR (MBT_ERROR_PREFIX | 2UL)
#define MBT_UNAVAILABLE_CLOCK_ERROR   (MBT_ERROR_PREFIX | 3UL)
#define MBT_NO_ENTRY_ADDABLE_ERROR    (MBT_ERROR_PREFIX | 4UL)
#define MBT_NO_JOB_ADDABLE_ERROR      (MBT_ERROR_PREFIX | 5UL)
#define MBT_HANDLE_INVALID_ERROR      (MBT_ERROR_PREFIX | 6UL)
#define MBT_CLOSE_FLAG_SET_ERROR      (MBT_ERROR_PREFIX | 7UL)
#define MBT_SOCKET_TIMEOUT_ERROR      (MBT_ERROR_PREFIX | 8UL)
#define MBT_WRONG_RESPONSE_FC_ERROR   (MBT_ERROR_PREFIX | 9UL)
#define MBT_RESPONSE_FALSE_LENGTH_ERROR  (MBT_ERROR_PREFIX | 10UL)
#define MBT_EXIT_ERROR                (MBT_ERROR_PREFIX | 11UL)


#define MBT_EXCEPTION_PREFIX ((DWORD) 0xEF01F000)

#define MBT_ILLEGAL_FUNCTION          (MBT_EXCEPTION_PREFIX | 0x01 )
#define MBT_ILLEGAL_DATA_ADDRESS      (MBT_EXCEPTION_PREFIX | 0x02 )
#define MBT_ILLEGAL_DATA_VALUE        (MBT_EXCEPTION_PREFIX | 0x03 )
#define MBT_ILLEGAL_RESPONSE_LENGTH   (MBT_EXCEPTION_PREFIX | 0x04 )
#define MBT_ACKNOWLEDGE               (MBT_EXCEPTION_PREFIX | 0x05 )
#define MBT_SLAVE_DEVICE_BUSY         (MBT_EXCEPTION_PREFIX | 0x06 )
#define MBT_NEGATIVE_ACKNOWLEDGE      (MBT_EXCEPTION_PREFIX | 0x07 )
#define MBT_MEMORY_PARITY_ERROR       (MBT_EXCEPTION_PREFIX | 0x08 )
#define MBT_GATEWAY_PATH_UNAVAILABLE  (MBT_EXCEPTION_PREFIX | 0x0A )
#define MBT_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND \
                                      (MBT_EXCEPTION_PREFIX | 0x0B )

#endif















