#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "ClientApplication.h"

#include "MyDaSession.h"

using namespace SoftingOPCToolboxClient;

class OpcClient
{

private:

	MyDaSession* session;
	ExecutionOptions executionOptions;
	unsigned long executionContext;
	tstring addressSpaceElementId;
	tstring addressSpaceElementPath;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);

	long initializeDaObjects();

	void getAsyncAddressSpaceElementProps();
	void getSyncAddressSpaceElementProps();

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

};  // end class OpcClient

// public OpcClient handlers
OpcClient* getOpcClient(void);
void createOpcClient(void);
void destroyOpcClient(void);

#endif
