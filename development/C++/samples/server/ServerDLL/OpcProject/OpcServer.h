#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyRequest.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

#include "MyAeAddressSpaceElement.h"

#include "MyCreator.h"

using namespace SoftingOPCToolboxServer;

class OpcServer
{
public:

	OpcServer(void);
	virtual ~OpcServer(void);

	long initialize(void);
	long start(void);
	long stop(void);
	long ready(void);
	long terminate(void);

	long prepare(IN MyCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildAddressSpace(void);

	long buildEventCategories(void);
	void fireSimulationEvents(void);

	void changeSimulationValues(void);

	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

private:
	ShutdownRequestHandler m_ShutdownRequest;
	MyDaAddressSpaceElement* m_pDaSimulationElement;
};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_

