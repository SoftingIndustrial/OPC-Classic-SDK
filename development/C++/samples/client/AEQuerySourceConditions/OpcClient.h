#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "ClientApplication.h"


#include "MyAeSession.h"
#include "Ae\ClientAeCondition.h"

using namespace SoftingOPCToolboxClient;

class OpcClient
{

private:

	MyAeSession* session;
	ExecutionOptions executionOptions;
	unsigned long executionContext;
	std::vector<tstring> conditionNames;
	tstring sourcePath;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);

	void getAsyncSourceConditions();
	void getSyncSourceConditions();

	long initializeAeObjects();

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
