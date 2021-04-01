#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "ClientApplication.h"

#include "MyDaSession.h"
#include "MyDaSubscription.h"
#include "MyDaItem.h"

#include "MyAeSession.h"
#include "MyAeSubscription.h"
#include "Ae\ClientAeCondition.h"

using namespace SoftingOPCToolboxClient;

class OpcClient
{

private:

	MyDaSession* m_daSession;
	MyDaSubscription* m_daSubscription;
	MyDaItem* m_daItem;

	MyAeSubscription* m_aeSubscription;
	MyAeSession* m_aeSession;
	tstring getStateToString(EnumConditionState conditionState);

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

	long initializeAeObjects();
	tstring getConditionState();

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
