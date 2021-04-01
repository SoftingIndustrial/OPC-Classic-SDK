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

	MyDaSession* m_daSession;
	MyDaSubscription* m_daSubscription;
	MyDaItem* m_daItem1;
	MyDaItem* m_daItem2;
	MyDaItem* m_daItem3;
	MyDaItem* m_daItem4;
	bool m_areDaObjectsInitialized;
	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);
	void setServiceDescription(tstring serviceDescription);


	long initializeDaObjects();
	tstring readItem();


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
