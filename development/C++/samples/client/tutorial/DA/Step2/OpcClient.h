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
	MyDaItem* m_daItem;


	SoftingOPCToolboxClient::Application* getApp();

public:

	OpcClient();
	virtual ~OpcClient() {}

	long initialize();
	void terminate();
	long processCommandLine(IN tstring commandLine);
	void setServiceName(tstring serviceName);


	long initializeDaObjects();
	tstring readItem();


	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		TCHAR* anObjectID,
		TCHAR* aMessage,
		...);

};  //  end class OpcClient

//	public OpcClient handlers
OpcClient* getOpcClient(void);
void createOpcClient(void);
void destroyOpcClient(void);

#endif
