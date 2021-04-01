#ifndef _MBT_H_
#define _MBT_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define MODBUSTCP_TABLE_OUTPUT_REGISTER   4
#define MODBUSTCP_TABLE_INPUT_REGISTER    3
#define MODBUSTCP_TABLE_OUTPUT_COIL       0
#define MODBUSTCP_TABLE_INPUT_COIL        1
#define MODBUSTCP_TABLE_EXCEPTION_STATUS  7


//-----------------------------------------------------------------------------
// Type Definitions                                                           |
//-----------------------------------------------------------------------------

	typedef void (WINAPI* MBTReadCompleted)(
		IN HANDLE hSocket,         // socket handle
		IN DWORD callbackContext,  // callback context, handed over at the call
		IN LONG errorCode,         // result of the read operation
		IN BYTE tableType,         // type of MODBUS/TCP tables(MODBUSTCP_TABLE_xxx)
		IN WORD dataStartAddress,  // start address of the registers or coils to be
		// read
		IN WORD numRead,           // number of the registers or coils to be read
		IN WORD numBytes,          // number of the bytes to be read
		IN LPBYTE pReadBuffer      // memory section with the data to be written
	);

	typedef void (WINAPI* MBTWriteCompleted)(
		IN HANDLE hSocket,         // socket handle
		IN DWORD callbackContext,  // callback context, handed over at the call
		IN LONG errorCode,         // result of the write operation
		IN BYTE tableType,         // type of MODBUS/TCP tables(MODBUSTCP_TABLE_xxx)
		// output registers or output coils
		IN WORD dataStartAddress,  // start address of the registers or coils to be
		// written
		IN WORD numWrite,          // number of the registers or coils to be written
		IN LPBYTE pWriteBuffer     // memory section with the data to be written
	);


//-----------------------------------------------------------------------------
// Prototypes                                                                 |
//-----------------------------------------------------------------------------

// initializes the MODBUS/TCP library
	LONG WINAPI MBTInit();

// terminates the MODBUS/TCP library
	LONG WINAPI MBTExit();

// creates a socket and connects it to the given device port
	LONG WINAPI MBTConnect(
		IN LPCTSTR szHostAddress,  // TCP/IP address of device
		IN WORD port,              // TCP port in device for communication
		IN BOOL useTCPorUDP,       // TRUE - TCP; FALSE - UDP
		IN DWORD requestTimeout,   // maximal time for managing an I/O request (ms)
		OUT HANDLE* hSocket        // handle of the connected socket
	);

// aborts the connection to a device and releases the socket
	LONG WINAPI MBTDisconnect(
		IN HANDLE hSocket          // handle of the connected socket
	);

// read from a connected socket
	LONG WINAPI MBTReadRegisters(
		IN HANDLE hSocket,         // handle of the connected socket
		IN BYTE tableType,         // Modbus/TCP Tabellen Typ (MODBUSTCP_TABLE_xxx)
		// (here: input register or output register
		IN WORD dataStartAddress,  // start address of the registers to be read
		IN WORD numWords,          // number of the registers to be read
		OUT LPBYTE pReadBuffer,    // memory section from which the data are read
		// (NULL at asynchronous call)
		OPTIONAL IN MBTReadCompleted fpReadCompletedCallback = NULL,
		// C-callback function, called after termination
		// of asynchronous reading (NULL at synchronous
		// call)
		OPTIONAL IN DWORD callbackContext = 0
											// context, handed over to the asynchronous
											// (callback function (0 at synchronous call)
	);


// write to a connected socket
	LONG WINAPI MBTWriteRegisters(
		IN HANDLE hSocket,         // handle of the connected socket
		IN WORD dataStartAddress,  // start address of the registers to be written
		IN WORD numWords,          // number of the registers to be written
		IN LPBYTE pWriteBuffer,    // memory section from which the data are written
		// (NULL at asynchronous call)
		OPTIONAL IN MBTWriteCompleted fpWriteCompletedCallback = NULL,
		// C-callback function, called after termination
		// of asynchronous writing (NULL at synchronous
		// call)
		OPTIONAL IN DWORD callbackContext = 0
											// context, handed over to the asynchronous
											// (callback function (0 at synchronous call)
	);

// read from a connected socket
	LONG WINAPI MBTReadCoils(
		IN HANDLE hSocket,         // handle of the connected socket
		IN BYTE tableType,         // Modbus/TCP Tabellen Typ (MODBUSTCP_TABLE_xxx)
		// (here: input coil or output coil
		IN WORD dataStartAddress,  // start address of the coils to be read
		IN WORD numBits,           // number of the coils to be read
		OUT LPBYTE pReadBuffer,     // memory section from which the data are read
		// (NULL at asynchronous call)
		OPTIONAL IN MBTReadCompleted fpReadCompletedCallback = NULL,
		// C-callback function, called after termination
		// of asynchronous reading (NULL at synchronous
		// call)
		OPTIONAL IN DWORD callbackContext = 0
											// context, handed over to the asynchronous
											// (callback function (0 at synchronous call)
	);


// write to a connected socket
	LONG WINAPI MBTWriteCoils(
		IN HANDLE hSocket,         // handle of the connected socket
		IN WORD dataStartAddress,  // start address of the coils to be written
		IN WORD numBits,           // number of the coils to be written
		IN LPBYTE pWriteBuffer,    // memory section from which the data are written
		// (NULL at asynchronous call)
		OPTIONAL IN MBTWriteCompleted fpWriteCompletedCallback = NULL,
		// C-callback function, called after termination
		// of asynchronous writing (NULL at synchronous
		// call)
		OPTIONAL IN DWORD callbackContext = 0
											// context, handed over to the asynchronous
											// (callback function (0 at synchronous call)
	);


// read from a connected socket
	LONG WINAPI MBTReadExceptionStatus(
		IN HANDLE hSocket,         // handle of the connected socket
		OUT LPBYTE pExceptionStatus,
		// memory section from which the data are read
		// (NULL at asynchronous call)
		OPTIONAL IN MBTReadCompleted fpReadCompletedCallback = NULL,
		// C-callback function, called after termination
		// of asynchronous reading (NULL at synchronous
		// call)
		OPTIONAL IN DWORD callbackContext = 0
											// context, handed over to the asynchronous
											// (callback function (0 at synchronous call)
	);


	WORD WINAPI MBTSwapWord(const WORD wData);

	DWORD WINAPI MBTSwapDWord(const DWORD dwData);


#ifdef __cplusplus
}
#endif

#endif
