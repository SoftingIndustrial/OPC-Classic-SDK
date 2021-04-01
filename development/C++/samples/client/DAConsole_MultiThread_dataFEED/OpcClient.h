#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Enums.h"
#include "ClientApplication.h"

using namespace SoftingOPCToolboxClient;

#include "MyDaItem.h"
#include "MyDaSession.h"
#include "MyDaSubscription.h"

extern unsigned short g_itemDataType;
extern const unsigned long g_minItemsPerSubscription;
extern const unsigned long g_maxItemsPerSubscription;
extern const unsigned int g_minSubscriptionUpdateRate;
extern const unsigned int g_maxSubscriptionUpdateRate;
extern const unsigned int g_itemsTotalCount;
extern const TCHAR *g_itemNames[];
extern MyMutex g_syncDisplay;
extern const BOOL g_connectItemsFirst;
extern const unsigned int g_silentDisplayCount;
extern const BOOL g_writeQuality;
extern const BOOL g_useHardcodedDataType;
extern const VARENUM g_hardcodedDataType;

const unsigned int g_qualities[17] = {
	EnumQuality_GOOD,
	EnumQuality_GOOD_LOCAL_OVERRIDE,
	EnumQuality_BAD,
	EnumQuality_BAD_CONFIG_ERROR,
	EnumQuality_BAD_NOT_CONNECTED,
	EnumQuality_BAD_DEVICE_FAILURE,
	EnumQuality_BAD_SENSOR_FAILURE,
	EnumQuality_BAD_LAST_KNOWN,
	EnumQuality_BAD_COMM_FAILURE,
	EnumQuality_BAD_OUT_OF_SERVICE,
	EnumQuality_BAD_WAITING_FOR_INITIAL_DATA,
	EnumQuality_UNCERTAIN,
	EnumQuality_UNCERTAIN_LAST_USABLE,
	EnumQuality_UNCERTAIN_SENSOR_CAL,
	EnumQuality_UNCERTAIN_EGU_EXCEEDED,
	EnumQuality_UNCERTAIN_SUB_NORMAL,
	EnumQuality_QUALITY_NOT_SET
};

class OpcClient
{

private:
	MyDaSession* m_session;

	MyDaSubscription **m_subscriptions;
	MyDaItem **m_items;
	std::vector<tstring> m_itemIds;
	std::vector<tstring> m_itemPaths;
	std::vector<MyDaItem*> m_removedItems;
	Mutex m_removedItemsJanitor;

	unsigned long m_executionContext;
	ExecutionOptions m_executionOptions;

	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient();

	//void setCredentials(IN const tstring& usr, IN const tstring& pwd);

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
	void activateConnectionMonitor();
	void deactivateConnectionMonitor();
	void logOnAsyncForSecureCommunication();
	void logOnSyncForSecureCommunication();
	void logOffAsyncForSecureCommunication();
	void logOffSyncForSecureCommunication();
	void setCredentials();
	void modifyUpdateRate(const DWORD updateRate = INFINITE);
	void connectSession();
	void disconnectSession();
	void removeRandomItems();
	void addRandomItems();

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
