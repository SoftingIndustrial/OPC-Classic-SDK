#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Enums.h"
#include "ClientApplication.h"

using namespace SoftingOPCToolboxClient;

#include "MyAeSession.h"
#include "MyAeSubscription.h"
#include "Ae\ClientAeCondition.h"
#include "ClientServerStatus.h"
#include "Ae\ClientAeEvent.h"

class OpcClient
{

private:

	MyAeSubscription* subscription;
	MyAeSession* session;
	tstring getStateToString(EnumConditionState conditionState);
	ExecutionOptions executionOptions;
	unsigned long executionContext;
	unsigned long count;
	ServerStatus serverStatus;
	std::vector<tstring> areas;
	std::vector<tstring> sources;
	std::vector<unsigned long> categoryIds;
	std::vector<AeReturnedAttributes*>aeReturnedAttributes;
	std::vector<unsigned long> attributesIds;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);

	void activateSyncObjects();
	void activateAsyncObjects();
	void connectSyncObjects();
	void connectAsyncObjects();
	void disconnectSyncObjects();
	void disconnectAsyncObjects();
	void getSyncServerStatus();
	void getAsyncServerStatus();
	void activateConnectionMonitor();
	void deactivateConnectionMonitor();

	long initializeAeObjects();
	tstring getConditionState();

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
