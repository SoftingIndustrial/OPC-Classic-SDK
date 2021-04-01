#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "ClientApplication.h"

#include "MyDaSession.h"
#include "MyDaSubscription.h"
#include "MyDaItem.h"


using namespace SoftingOPCToolboxClient;

class OpcClient
{

private:

	MyDaSession* session;
	MyDaSubscription* subscription;
	MyDaItem* item1;
	MyDaItem* item2;
	MyDaItem* item3;
	std::vector<EnumObjectAttribute> whatAttributes;
	std::vector<long>sessionResults;
	std::vector<long>subscriptionResults;
	std::vector<long>itemResults;
	std::vector<DaItem*>items;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);

	long initializeDaObjects();

	void printObjectAttributes();
	void changeSessionAttributes();
	void changeSubscriptionAttributes();
	void changeItemAttributes();

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
