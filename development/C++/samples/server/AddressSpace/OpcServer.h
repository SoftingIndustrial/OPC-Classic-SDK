#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

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
	void rebuildRandomNamespace();
	void printChildren(AddressSpaceElement* element, unsigned char bLevel);
	void showObjectTree(void);
	void updateAddressSpaceElements();

	void setServiceName(tstring serviceName);
	void setServiceDescription(tstring serviceDescription);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

private:
	ShutdownRequestHandler m_ShutdownRequest;

};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_
