#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "DaAddressSpaceElement.h"
#include "AeAddressSpaceElement.h"
#include "Request.h"
#include "Transaction.h"
#include "DaAddressSpaceRoot.h"

#include "Creator.h"

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

	long prepare(IN DemoCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildAddressSpace(void);

	long buildDAAddressSpace(void);

#ifdef TBC_OS_WINDOWS
	long buildAEAddressSpace(void);
	long buildAEEventCategories(void);
#endif

	void changeSimulationValues(void);

	void setServiceName(tstring serviceName);

	DemoDaAddressSpaceElement* getKeyElement();

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

private:
	ShutdownRequestHandler m_ShutdownRequest;
	std::vector<DemoDaAddressSpaceElement*> m_elementList;
	DemoDaAddressSpaceElement* m_pkeyElement;

};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_

