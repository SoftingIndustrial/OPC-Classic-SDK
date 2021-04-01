#include "stdafx.h"
#include "OpcServer.h"

#include "MyTransaction.h"
#include "MyCreator.h"
#include "ServerCommon.h"
#include "MBT.h"

OpcServer* instance = NULL;

//	global handle to the end thread event
HANDLE g_endHandleRequestsEvent = 0;

//	global handle to the event that triggers the re-handling of the requests
HANDLE g_HandleRequestsEvent = 0;

//	global handle of the requests thread
HANDLE g_HandleRequestsThread = 0;

//	global handle of the socket for the Modbus/TCP I/O
HANDLE g_Socket;

//	global mutex to sincronize the finish of the request thread
Mutex g_handleRequestsMutex;

//	global flag (TRUE if there is a request pending)
BOOL g_ioPending = FALSE;

//-----------------------------------------------------------------------------
//	getOpcServer - returns the handle of the server instance
//
//-----------------------------------------------------------------------------
OpcServer* getOpcServer(void)
{
	return instance;
}   //  end getOpcServer

//-----------------------------------------------------------------------------
//	createOpcServer - creates the server instance
//	if there isn't one already created
//-----------------------------------------------------------------------------
void createOpcServer(void)
{
	if (instance == NULL)
	{
		instance = new OpcServer();
	}   //  end if
}   //  end createOpcServer


//-----------------------------------------------------------------------------
//	destroyOpcServer - destroys the server instance if there is one
//
//-----------------------------------------------------------------------------
void destroyOpcServer(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}   //  end if
}   //  end destroyOpcServer

//-----------------------------------------------------------------------------
//	HandleShutdownRequest
//
//-----------------------------------------------------------------------------
long API_CALL HandleShutdownRequest(void)
{
	return S_OK;
}   //  end HandleShutdownRequest

//-----------------------------------------------------------------------------
//	handleRequestsThread - thread method used to handle the requests
//
//-----------------------------------------------------------------------------
DWORD WINAPI handleRequestsThread(void* context)
{
	//  get the handle to the OPC server instance
	OpcServer* pServer = getOpcServer();
	//  create the handles array for the WaitForMultipleObjects call
	HANDLE pHandles[2];
	pHandles[0] = g_endHandleRequestsEvent; // the end thread event
	pHandles[1] = g_HandleRequestsEvent;// new request - handling event
	//  set the time-out interval
	unsigned long waitTime = 100;
	//  set the end while flag to false
	BOOL end = FALSE;
	unsigned long waitRet = 0;
	//  lock the thread so it will stop only after the last requests are processed
	g_handleRequestsMutex.lock();

	while (!end)
	{
		//  wait for one of the two events set or for the time-out
		waitRet = WaitForMultipleObjects(2, pHandles, FALSE, waitTime);

		if (waitRet == WAIT_OBJECT_0)
		{
			//  the end thread event has been thrown
			end = TRUE;
		}
		else
		{
			//  remove the completed requests from the server's requests vector
			pServer->removeCompletedRequests();
			//  handle the remaining requests
			pServer->handleRequests();
		}   //  end if...else
	}   //  end while

	//  the end thread event has been thrown
	//  release the handles to the events
	::CloseHandle(g_endHandleRequestsEvent);
	::CloseHandle(g_HandleRequestsEvent);
	//  disconnect the socket to the Modbus and exit
	LONG result = MBTDisconnect(g_Socket);
	MBTExit();
	//  trace the action for information purposes
	pServer->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_OPCSERVER,
		_T("MBT"),
		_T("End of I/O Thread"));
	//  unlock the requests thread mutex
	g_handleRequestsMutex.unlock();
	return 0;
}   //  end handleRequestsThread

//-----------------------------------------------------------------------------
//	startHandleRequestsThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startHandleRequestsThread(void)
{
	//Verify if the thread is already running
	if (g_HandleRequestsThread != NULL)
	{
		//  the thread is already running
		//  trace this for information purposes
		getOpcServer()->trace(
			EnumTraceLevel_WRN,
			EnumTraceGroup_OPCSERVER,
			_T("Thread is already running"),
			_T("startHandleRequestsThread"));
		return;
	}

	//  the thread is not running
	//  create the events and keep a handle to them
	g_endHandleRequestsEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_HandleRequestsEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD ret;
	// initialize Modbus/TCP communication library
	ret = MBTInit();

	if (ret == S_OK)
	{
		// connect to Modbus/TCP device
		// parameters:   IP address, IP port, TCP or UDP, request timeout
		ret = MBTConnect(_T("localhost"), 502, TRUE, 1000, &g_Socket);
	}

	if (ret == S_OK)
	{
		//  the connection is succesful
		//  trace the information
		getOpcServer()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_OPCSERVER,
			_T("MBT"),
			_T("Connected to Modbus/TCP device"));
		//  create the handleRequestsThread
		DWORD tID = -1;
		g_HandleRequestsThread = ::CreateThread(NULL, 0, &handleRequestsThread, NULL, 0, &tID);
	}
	else
	{
		//  the connection is not succesful
		//  trace a warning
		getOpcServer()->trace(
			EnumTraceLevel_WRN,
			EnumTraceGroup_OPCSERVER,
			_T("MBT"),
			_T("Not connected to Modbus/TCP device"));
	}
}   //  end startHandleRequestsThread

//-----------------------------------------------------------------------------
//	stopHandleRequestsThread - thread stopping function
//
//-----------------------------------------------------------------------------
void stopHandleRequestsThread(void)
{
	//  throw the end thread event
	::SetEvent(g_endHandleRequestsEvent);
	//  close the handle to the thread
	::CloseHandle(g_HandleRequestsThread);
}// end stopHandleRequestsThread

//-----------------------------------------------------------------------------
//	mbtReadCallback  - read callback of MBT lib
//
//-----------------------------------------------------------------------------
void WINAPI mbtReadCallback(
	IN HANDLE hSocket,         // socket handle
	IN DWORD callbackContext,  // callback context, handed over at the call
	IN LONG errorCode,         // result of the read operation
	IN BYTE tableType,         // type of MODBUS/TCP tables(MODBUSTCP_TABLE_xxx)
	IN WORD dataStartAddress,  // start address of the registers or coils to be read
	IN WORD numRead,           // number of the registers or coils to be read
	IN WORD numBytes,          // number of the bytes to be read
	IN LPBYTE pReadBuffer)     // memory section with the data to be written
{
	if (callbackContext)
	{
		// OPC DA request
		DaRequest* pRequest = (DaRequest*) callbackContext;
		//  get the element for which the read request has been sent
		MyDaAddressSpaceElement* pElement = (MyDaAddressSpaceElement*)pRequest->getAddressSpaceElement();

		if (errorCode == S_OK)
		{
			// read value
			// update cache
			// trace this for information purposes
			getOpcServer()->trace(
				EnumTraceLevel_INF,
				EnumTraceGroup_OPCSERVER,
				(TCHAR*)pElement->getName().c_str(),
				_T("Read Callback 0x%2.2X"), (BYTE)*pReadBuffer);
			//  process the data
			WORD returnValue = MBTSwapWord((WORD) * ((WORD*)pReadBuffer));

			//  depending on the IO identifier of the element:
			switch (pElement->getIOIdentifier())
			{
			case OpcServer::IOBYTE_TAG_INPUT:
			case OpcServer::IOBYTE_TAG_OUTPUT:
			{
				// set the value for the request
				Variant aVariant(LOBYTE(returnValue));
				DateTime now;
				ValueQT aValue(aVariant, EnumQuality_GOOD, now);
				pRequest->setValue(aValue);
				// set the request to completed
				pRequest->setResult(EnumResultCode_S_OK);
				pRequest->setRequestState(EnumRequestState_COMPLETED);
			}
			break;

			// bit IO
			case OpcServer::IOBIT_TAG_SWITCH1:
			case OpcServer::IOBIT_TAG_SWITCH2:
			case OpcServer::IOBIT_TAG_SWITCH3:
			case OpcServer::IOBIT_TAG_SWITCH4:
			{
				// process the read data and set the value for the request
				Variant aVariant(((bool)(0 != (HIBYTE(returnValue) & (BYTE)(1 << ((pElement->getIOIdentifier() & ~BIT_OFFSET) - 1))))));
				DateTime now;
				ValueQT aValue(aVariant, EnumQuality_GOOD, now);
				pRequest->setValue(aValue);
				// set the request to a completed state
				pRequest->setResult(EnumResultCode_S_OK);
				pRequest->setRequestState(EnumRequestState_COMPLETED);
			}
			break;

			default:
			{
				// never reach this point of execution...
				//  trace the warning
				getOpcServer()->trace(
					EnumTraceLevel_WRN,
					EnumTraceGroup_OPCSERVER,
					(TCHAR*)pElement->getName().c_str(),
					_T("Read item  IOIdent [0x%08.8X]"),  pElement->getIOIdentifier());
				//  set the value to uncertain
				ValueQT aValue;
				pRequest->getAddressSpaceElement()->getCacheValue(aValue);
				Variant aVariant = aValue.getData();
				DateTime now;
				ValueQT anotherValue(aVariant, EnumQuality_UNCERTAIN_LAST_USABLE, now);
				pRequest->setValue(anotherValue);
				//  set the request to completed
				pRequest->setResult(EnumResultCode_S_OK);
				pRequest->setRequestState(EnumRequestState_COMPLETED);
			}
			break;
			}
		}
		else
		{
			// error
			// set bad quality
			ValueQT aValue;
			pRequest->getAddressSpaceElement()->getCacheValue(aValue);
			Variant aVariant = aValue.getData();
			DateTime now;
			ValueQT anotherValue(aVariant, EnumQuality_BAD_COMM_FAILURE, now);
			//  set the request state to completed
			pRequest->setValue(anotherValue);
			pRequest->setResult(EnumResultCode_S_OK);
			pRequest->setRequestState(EnumRequestState_COMPLETED);
			//  trace the error
			getOpcServer()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_OPCSERVER,
				(TCHAR*)pElement->getName().c_str(),
				_T("Read Callback [0x%08.8X]"),  errorCode);
		}
	}
	else
	{
		// trace the error
		getOpcServer()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_OPCSERVER,
			_T("MBTCBK"),
			_T("Unknown callbackContext"));
	}

	// release I/O lock
	g_ioPending = FALSE;
	//  throw the event to clean the requests vector and handle the other requests
	::SetEvent(g_HandleRequestsEvent);
}// end mbtReadCallback


//-----------------------------------------------------------------------------
//	mbtWriteCallback - write callback of MBT lib
//
//-----------------------------------------------------------------------------
void WINAPI mbtWriteCallback(
	IN HANDLE hSocket,         // socket handle
	IN DWORD callbackContext,  // callback context, handed over at the call
	IN LONG errorCode,           // result of the write operation
	IN BYTE tableType,         // type of MODBUS/TCP tables(MODBUSTCP_TABLE_xxx)
	IN WORD dataStartAddress,  // start address of the registers or coils to be written
	IN WORD numWrite,          // number of the registers or coils to be written
	IN LPBYTE pWriteBuffer)    // memory section with the data to be written
{
	if (callbackContext)
	{
		//  OPC DA request
		DaRequest* pRequest = (DaRequest*)callbackContext;
		//  get the element for the request
		MyDaAddressSpaceElement* pElement = (MyDaAddressSpaceElement*)pRequest->getAddressSpaceElement();
		//  get the value to be written
		ValueQT* pValue = pRequest->getValue();

		if (errorCode == S_OK)
		{
			// the value has been written
			//  set the request to completed
			pRequest->setResult(EnumResultCode_S_OK);
			pRequest->setRequestState(EnumRequestState_COMPLETED);
			//  trace this for information purposes
			getOpcServer()->trace(
				EnumTraceLevel_INF,
				EnumTraceGroup_OPCSERVER,
				(TCHAR*)pElement->getName().c_str(),
				_T("Write Callback 0x%2.2X"), (BYTE)*pWriteBuffer);
		}
		else
		{
			// error
			//  the value has not been written
			//  set the request to failed and completed
			pRequest->setResult(EnumResultCode_E_FAIL);
			pRequest->setRequestState(EnumRequestState_COMPLETED);
			//  trace this for information purposes
			getOpcServer()->trace(
				EnumTraceLevel_INF,
				EnumTraceGroup_OPCSERVER,
				(TCHAR*)pElement->getName().c_str(),
				_T("Write Callback [0x%8.8X]"), errorCode);
		}
	}
	else
	{
		//  trace the error
		getOpcServer()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_OPCSERVER,
			_T("MBTCBK"),
			_T("Unknown callbackContext"));
	}

	// release I/O lock
	g_ioPending = FALSE;
	//  throw the event to clean the requests vector and handle the other requests
	::SetEvent(g_HandleRequestsEvent);
}// end mbtWriteCallback

//-----------------------------------------------------------------------------
//	OpcServer    - the constructor
//
//-----------------------------------------------------------------------------
OpcServer::OpcServer(void)
{
	m_pDaSimulationElement1 = NULL;
	m_pDaSimulationElement2 = NULL;
	m_pDaSimulationElement3 = NULL;
	m_pDaSimulationElement4 = NULL;
	m_pDaSimulationElement5 = NULL;
	m_pDaSimulationElement6 = NULL;
	m_pDaSimulationElement7 = NULL;
	m_pDaSimulationElement8 = NULL;
}   //  end constructor

//-----------------------------------------------------------------------------
//	~OpcServer   - the destructor
//
//-----------------------------------------------------------------------------
OpcServer::~OpcServer(void)
{
}   //  end destructor

//-----------------------------------------------------------------------------
//	initialize   - initializes the server application
//					- sets the Classand Prog Id, version number etc
//-----------------------------------------------------------------------------
long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{6F3AF987-6E6D-4161-AA7A-03F0FC88F6EE}"));
	getApp()->setProgIdDa(_T("Softing.Seminar.ReferenceServer.V42X.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.Seminar.ReferenceServer.V42X.DA"));
	getApp()->setDescription(_T("Softing Seminar ReferenceServer V42X DA 1"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(1);
	getApp()->setVendorInfo(_T("Softing Industrial Automation GmbH"));
	getApp()->setMinUpdateRateDa(100);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('/'));
	getApp()->setIpPortHTTP(8081);
	getApp()->setUrlDa(_T("/DA"));
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_SERVER,
		EnumTraceGroup_SERVER,
		_T("ServerTrace"),
		1000000,
		2);
	return S_OK;
}   //  end initialize

//-----------------------------------------------------------------------------
//	setServiceName   - sets the server's service name
//
//-----------------------------------------------------------------------------
void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName

//-----------------------------------------------------------------------------
//	prepare  - prepares the server for start
//-----------------------------------------------------------------------------
long OpcServer::prepare(MyCreator* creator)
{
	long result = getApp()->initialize(creator);
	return result;
}   //  end prepare

//-----------------------------------------------------------------------------
//	start    - starts the server application
//
//-----------------------------------------------------------------------------
long OpcServer::start(void)
{
	return getApp()->start();
}   //  end start

//-----------------------------------------------------------------------------
//	ready    - gets the server application ready
//
//-----------------------------------------------------------------------------
long OpcServer::ready(void)
{
	return getApp()->ready();
}   //  end ready

//-----------------------------------------------------------------------------
//	stop - stops the server application
//
//-----------------------------------------------------------------------------
long OpcServer::stop(void)
{
	//  give the thread time to smoothly closetdown
	g_handleRequestsMutex.lock();
	g_handleRequestsMutex.unlock();
	return getApp()->stop();
}   //  end stop

//-----------------------------------------------------------------------------
//	terminate    - terminates the server application
//					- releases the application
//-----------------------------------------------------------------------------
long OpcServer::terminate(void)
{
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate

//-----------------------------------------------------------------------------
//	processCommandLine   - registers and unregisters the server
//
//-----------------------------------------------------------------------------
long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine


//-----------------------------------------------------------------------------
//	buildNameSpace   - creates the server namespace
//						- two branches (IOBytes and IOBits)
//						- two leaves (Input and Output) for IOBytes
//						- four leaves (Switch1 to Switch4) for IOBits
//						- the only leaf with write access rights is Output
//-----------------------------------------------------------------------------
long OpcServer::buildNameSpace(void)
{
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	tstring aName;
	DateTime now;
	now.now();
	Variant aVariant(rand());
	ValueQT value(aVariant, EnumQuality_GOOD, now);
	//  DA
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	/////////////////////////
	// create byte IO objects
	/////////////////////////
	m_pDaSimulationElement1 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("IOBytes"));
	m_pDaSimulationElement1->setName(aName);
	m_pDaSimulationElement1->setHasChildren(true);
	m_pDaSimulationElement1->setIoMode(EnumIoMode_NONE);
	daRoot->addChild(m_pDaSimulationElement1);
	// input byte
	m_pDaSimulationElement2 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBYTE_TAG_INPUT);
	aName = tstring(_T("Input"));
	m_pDaSimulationElement2->setName(aName);
	m_pDaSimulationElement2->setAccessRights(EnumAccessRights_READABLE);
	m_pDaSimulationElement2->setDatatype(VT_UI1);
	m_pDaSimulationElement2->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement1->addChild(m_pDaSimulationElement2);
	// output byte
	m_pDaSimulationElement3 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBYTE_TAG_OUTPUT);
	aName = tstring(_T("Output"));
	m_pDaSimulationElement3->setName(aName);
	m_pDaSimulationElement3->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_pDaSimulationElement3->setDatatype(VT_UI1);
	m_pDaSimulationElement3->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement1->addChild(m_pDaSimulationElement3);
	////////////////////////
	// create bit IO objects
	////////////////////////
	m_pDaSimulationElement4 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("IOBits"));
	m_pDaSimulationElement4->setName(aName);
	m_pDaSimulationElement4->setHasChildren(true);
	m_pDaSimulationElement4->setIoMode(EnumIoMode_NONE);
	daRoot->addChild(m_pDaSimulationElement4);
	// input bit1
	m_pDaSimulationElement5 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBIT_TAG_SWITCH1);
	aName = tstring(_T("Switch1"));
	m_pDaSimulationElement5->setName(aName);
	m_pDaSimulationElement5->setAccessRights(EnumAccessRights_READABLE);
	m_pDaSimulationElement5->setDatatype(VT_BOOL);
	m_pDaSimulationElement5->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement4->addChild(m_pDaSimulationElement5);
	// input bit2
	m_pDaSimulationElement6 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBIT_TAG_SWITCH2);
	aName = tstring(_T("Switch2"));
	m_pDaSimulationElement6->setName(aName);
	m_pDaSimulationElement6->setAccessRights(EnumAccessRights_READABLE);
	m_pDaSimulationElement6->setDatatype(VT_BOOL);
	m_pDaSimulationElement6->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement4->addChild(m_pDaSimulationElement6);
	// input bit3
	m_pDaSimulationElement7 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBIT_TAG_SWITCH3);
	aName = tstring(_T("Switch3"));
	m_pDaSimulationElement7->setName(aName);
	m_pDaSimulationElement7->setAccessRights(EnumAccessRights_READABLE);
	m_pDaSimulationElement7->setDatatype(VT_BOOL);
	m_pDaSimulationElement7->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement4->addChild(m_pDaSimulationElement7);
	// input bit4
	m_pDaSimulationElement8 = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement(IOBIT_TAG_SWITCH4);
	aName = tstring(_T("Switch4"));
	m_pDaSimulationElement8->setName(aName);
	m_pDaSimulationElement8->setAccessRights(EnumAccessRights_READABLE);
	m_pDaSimulationElement8->setDatatype(VT_BOOL);
	m_pDaSimulationElement8->setIoMode(EnumIoMode_POLL);
	m_pDaSimulationElement4->addChild(m_pDaSimulationElement8);
	return EnumResultCode_S_OK;
}   //  end buildNameSpace

//-----------------------------------------------------------------------------
//	addRequests  - adds the requests received to the server's request list
//
//-----------------------------------------------------------------------------
void OpcServer::addRequests(std::vector<DaRequest*>& requests)
{
	//  lock the requests mutex to syncronize the adding and removing of requests
	m_requestsJanitor.lock();
	{
		//  get the number of requests in the vector
		size_t count = requests.size();

		//  for each request
		for (size_t i = 0; i < count; i++)
		{
			//  set the state of the request to created and add it to the end of the server's requests vector
			requests[i]->setRequestState(EnumRequestState_CREATED);
			m_requests.push_back(requests[i]);
		}   //  end for
	}
	//  unlock the requests mutex to syncronize the adding and removing of requests
	m_requestsJanitor.unlock();
	//  throw the event to clean the requests vector and handle the other requests
	::SetEvent(g_HandleRequestsEvent);
}   //  end addRequests

//-----------------------------------------------------------------------------
//	removeCompletedRequests  - removes the completed requests
//										from the server's request list
//-----------------------------------------------------------------------------
void OpcServer::removeCompletedRequests()
{
	//  lock the requests mutex to syncronize the adding and removing of requests
	m_requestsJanitor.lock();
	{
		//  set the found flag to true
		BOOL found = TRUE;

		//  while the vector is not empty and at least one request is completed,
		//  search the vector again and remove the completed request from the list
		while (!m_requests.empty() && found)
		{
			std::vector<DaRequest*>::iterator iteratorRequests = m_requests.begin();
			//  reset the flag
			found = FALSE;

			while (iteratorRequests != m_requests.end())
			{
				if ((*iteratorRequests)->getRequestState() == EnumRequestState_COMPLETED)
				{
					//  complete the request and remove it from the requests vector
					(*iteratorRequests)->complete();
					m_requests.erase(iteratorRequests);
					//  set the flag to true
					found = TRUE;
					break;
				}

				iteratorRequests++;
			}
		}
	}
	//  unlock the requests mutex to syncronize the adding and removing of requests
	m_requestsJanitor.unlock();
}// end removeCompletedRequests

//-----------------------------------------------------------------------------
//	handleRequests   - it processes each request on the list
//						- it calls the MTB read or write register
//						- sets the callback function for each request
//						- it takes into account a global flag (g_ioPending)
//						- only one succesful request is processed at one time
//-----------------------------------------------------------------------------
void OpcServer::handleRequests(void)
{
	DWORD result = 0;
	//  lock the requests mutex to syncronize the adding and removing of requests
	m_requestsJanitor.lock();
	{
		//  get the number of requests in the vector
		size_t count = m_requests.size();

		//  for each request
		for (size_t i = 0; i < count; i++)
		{
			//  if the state of the request is created, then process the request
			if (m_requests[i]->getRequestState() == EnumRequestState_CREATED)
			{
				if (g_ioPending)
				{
					// stop if Modbus/TCP request is pending
					break;
				}

				MyDaAddressSpaceElement* element =
					(MyDaAddressSpaceElement*)m_requests[i]->getAddressSpaceElement();

				if (element == NULL)
				{
					//  error - the element could not be found
					//  never reach this point of execution
					m_requests[i]->setResult(EnumResultCode_E_FAIL);
					result = EnumResultCode_E_FAIL;
				}
				else
				{
					m_requests[i]->setRequestState(EnumRequestState_PENDING);
					//  set the global flag to true (there is one request pending)
					g_ioPending = TRUE;

					//  if the request is a read request
					if (m_requests[i]->getTransactionType() == EnumTransactionType_READ)
					{
						//  if the request concerns the value
						if (m_requests[i]->getPropertyId() == 0)
						{
							// get address space element value take the Toolkit cache value
							switch (element->getIOIdentifier())
							{
							case IOBYTE_TAG_INPUT:
							{
								//read the Modbus register
								result = MBTReadRegisters(g_Socket, MODBUSTCP_TABLE_INPUT_REGISTER, 0x0000, 1, NULL, mbtReadCallback, (DWORD)m_requests[i]);
								//trace the result
								trace(
									(result == S_OK) ? EnumTraceLevel_INF : EnumTraceLevel_ERR,
									EnumTraceGroup_OPCSERVER,
									(TCHAR*)element->getName().c_str(),
									_T("Read Input [0x%8.8X]"), result);
							}
							break;

							case IOBYTE_TAG_OUTPUT:
							{
								//read the Modbus register
								result = MBTReadRegisters(g_Socket, MODBUSTCP_TABLE_OUTPUT_REGISTER, 0x0200, 1, NULL, mbtReadCallback, (DWORD)m_requests[i]);
								//trace the result
								trace(
									(result == S_OK) ? EnumTraceLevel_INF : EnumTraceLevel_ERR,
									EnumTraceGroup_OPCSERVER,
									(TCHAR*)element->getName().c_str(),
									_T("Read Input [0x%8.8X]"), result);
							}
							break;

							// bit IO
							case IOBIT_TAG_SWITCH1:
							case IOBIT_TAG_SWITCH2:
							case IOBIT_TAG_SWITCH3:
							case IOBIT_TAG_SWITCH4:
							{
								// read the Modbus register
								result = MBTReadRegisters(g_Socket, MODBUSTCP_TABLE_INPUT_REGISTER, 0x0000, 1, NULL, mbtReadCallback, (DWORD)m_requests[i]);
								//trace the result
								trace(
									(result == S_OK) ? EnumTraceLevel_INF : EnumTraceLevel_ERR,
									EnumTraceGroup_OPCSERVER,
									(TCHAR*)element->getName().c_str(),
									_T("Read Input [0x%8.8X]"), result);
							}
							break;

							default:
							{
								// never reach this point of execution...
								//trace the error
								trace(
									EnumTraceLevel_WRN,
									EnumTraceGroup_OPCSERVER,
									(TCHAR*)element->getName().c_str(),
									_T("Read item '%s' IOIdent [0x%08.8X]"), (TCHAR*)element->getName().c_str(), element->getIOIdentifier());
								result = E_FAIL;
							}
							break;
							}
						}
						else
						{
							//  the element's property will handle this request
							element->getPropertyValue(m_requests[i]);
						}   //  end if ... else
					}
					else
					{
						//  EnumTransactionType_WRITE
						WORD writeValue;
						ValueQT* pValue = m_requests[i]->getValue();
						writeValue = MBTSwapWord((WORD)pValue->getData().bVal);
						// start Modbus/TCP request
						result = MBTWriteRegisters(
									 g_Socket,
									 0x0200,
									 1,
									 (BYTE*)&writeValue,
									 mbtWriteCallback,
									 (DWORD)m_requests[i]);
						//trace the result
						trace(
							(result == S_OK) ? EnumTraceLevel_INF : EnumTraceLevel_ERR,
							EnumTraceGroup_OPCSERVER,
							(TCHAR*)element->getName().c_str(),
							_T("Write Output 0x%2.2X [0x%8.8X]"), pValue->getData().bVal, result);
					}   //  end if ... else

					if (result != S_OK)
					{
						// error sending request to Modbus/TCP device
						// set bad quality
						ValueQT aValue;
						m_requests[i]->getAddressSpaceElement()->getCacheValue(aValue);
						Variant aVariant = aValue.getData();
						DateTime now;
						ValueQT anotherValue(aVariant, EnumQuality_BAD_COMM_FAILURE, now);
						m_requests[i]->setValue(anotherValue);
						//set the request to completed
						m_requests[i]->setResult(EnumResultCode_S_OK);
						m_requests[i]->setRequestState(EnumRequestState_COMPLETED);
						//trace the result
						getOpcServer()->trace(
							EnumTraceLevel_ERR,
							EnumTraceGroup_OPCSERVER,
							(TCHAR*)element->getName().c_str(),
							_T("handleRequests [0x%8.8X]"), result);
						g_ioPending = FALSE;
					}
				}   //  end if ... else
			}//end if Pending
		}   //  end for
	}
	m_requestsJanitor.unlock();
}   //  end handleRequests

//-----------------------------------------------------------------------------
//	trace    - trace function
//
//-----------------------------------------------------------------------------
void OpcServer::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	TCHAR* anObjectID,
	TCHAR* aMessage,
	...)
{
	getApp()->trace(aLevel, aMask, anObjectID, aMessage);
}   //  end trace

//-----------------------------------------------------------------------------
//	startThread  - calls the startHandleRequestsThread function
//
//-----------------------------------------------------------------------------
void OpcServer::startThread()
{
	//starts the handleRequestsThread
	startHandleRequestsThread();
}//end startThread

//-----------------------------------------------------------------------------
//	stopThread   - calls the stopHandleRequestsThread function
//
//-----------------------------------------------------------------------------
void OpcServer::stopThread()
{
	//stops the handleRequestsThread
	stopHandleRequestsThread();
}//end stopThread
