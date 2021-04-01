#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Enums.h"
#include "ClientApplication.h"

using namespace SoftingOPCToolboxClient;

#include "MyDaItem.h"
#include "MyDaSession.h"
#include "MyDaSubscription.h"

class OpcClient
{

private:

	MyDaSession* session;
	MyDaSubscription* subscription;
	MyDaItem* item1;
	MyDaItem* item2;
	MyDaItem* item3;
	MyDaItem* item4;
	MyDaItem* item5; // secured item
	MyDaItem* item6; // Array item
	unsigned long readCount;
	unsigned long writeCount;
	unsigned long updateCount;
	std::vector<ValueQT*> valuesToRead;
	std::vector<ValueQT*> valuesToWrite;
	std::vector<DaItem*> itemsToRead;
	std::vector<DaItem*> itemsToWrite;
	std::vector<tstring> itemIdsForRead;
	std::vector<tstring> itemPathsForRead;
	std::vector<tstring> itemIdsForWrite;
	std::vector<tstring> itemPathsForWrite;
	std::vector<long> readResults;
	std::vector<long> writeResults;
	std::vector<long> updateResults;
	unsigned long executionContext;
	ExecutionOptions executionOptions;
	ServerStatus serverStatus;
	Variant data;
	ValueQT* aValue;
	tstring m_usr;
	tstring m_pwd;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	void setCredentials(IN const tstring& usr, IN const tstring& pwd);

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);

	long initializeDaObjects();

	void readSyncItem();
	void connectAsyncSubscription();
	void connectSyncSubscription();
	void activateAsyncSubscription();
	void activateSyncSubscription();
	void disconnectAsyncSubscription();
	void disconnectSyncSubscription();
	void readSyncSubscription();
	void readAsyncSubscription();
	void readSyncSession();
	void readAsyncSession();
	void refreshSubscription();
	void writeAsyncSubscription();
	void writeSyncSubscription();
	void writeAsyncSession();
	void writeSyncSession();
	void getAsyncServerStatus();
	void getSyncServerStatus();
	void changeDataRetrievalMode();
	void activateConnectionMonitor();
	void deactivateConnectionMonitor();
	void logOnAsyncForSecureCommunication();
	void logOnSyncForSecureCommunication();
	void logOffAsyncForSecureCommunication();
	void logOffSyncForSecureCommunication();
	void setCredentials(void);

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
