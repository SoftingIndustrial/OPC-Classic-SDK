#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#define BIT_OFFSET 0x80000000

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyRequest.h"
#include "MyDaAddressSpaceRoot.h"

using namespace SoftingOPCToolboxServer;

class MyCreator;

class OpcServer
{
public:

	enum ioIdentifier
	{
		IOBYTE_TAG_NOTDEFINED,
		IOBYTE_TAG_INPUT = 1,
		IOBYTE_TAG_OUTPUT,
		//bit IO
		IOBIT_TAG_SWITCH1 = BIT_OFFSET | 1,
		IOBIT_TAG_SWITCH2,
		IOBIT_TAG_SWITCH3,
		IOBIT_TAG_SWITCH4
	};

	OpcServer(void);
	virtual ~OpcServer(void);

	long initialize(void);
	long start(void);
	long stop(void);
	long ready(void);
	long terminate(void);

	long prepare(IN MyCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildNameSpace(void);

	void addRequests(std::vector<DaRequest*>& requests);
	void removeCompletedRequests();
	void handleRequests(void);

	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		TCHAR* anObjectID,
		TCHAR* aMessage,
		...);

	void startThread();
	void stopThread();

	Mutex m_requestsJanitor;
private:
	ShutdownRequestHandler m_ShutdownRequest;

	MyDaAddressSpaceElement* m_pDaSimulationElement1;
	MyDaAddressSpaceElement* m_pDaSimulationElement2;
	MyDaAddressSpaceElement* m_pDaSimulationElement3;
	MyDaAddressSpaceElement* m_pDaSimulationElement4;
	MyDaAddressSpaceElement* m_pDaSimulationElement5;
	MyDaAddressSpaceElement* m_pDaSimulationElement6;
	MyDaAddressSpaceElement* m_pDaSimulationElement7;
	MyDaAddressSpaceElement* m_pDaSimulationElement8;

	std::vector<DaRequest*> m_requests;


};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_

