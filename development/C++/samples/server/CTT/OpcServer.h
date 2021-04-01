#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "Da/ServerDaTransaction.h"
#include "Da/ServerDaRequest.h"
#include "Da/ServerDaProperty.h"

#include "MyDaAddressSpaceElement.h"
#include "MyAeAddressSpaceElement.h"
#include "MyTransaction.h"
#include "MyCreator.h"

using namespace SoftingOPCToolboxServer;

typedef enum
{
	FirstCondition = 0,
	SecondCondition
} AEConditionIndex;

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
	long buildDaAddressSpace(void);

	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		TCHAR* anObjectID,
		TCHAR* aMessage,
		...);

#ifdef TBC_OS_WINDOWS
	long buildAeAddressSpace(void);
	long buildEventCategories(void);
	void initializeConditions(void);
	void updateAECondition(
		unsigned long severity,
		tstring& message,
		BOOL ackReq,
		tstring& subCondName,
		std::vector<Variant>& eventAttributes,
		unsigned short changeMask,
		EnumConditionState conditionState,
		AEConditionIndex conditionIndex);

	void clearConditionList(void);
	void changeConditions(void);

	void handleSimpleEvents(
		unsigned short& second,
		unsigned short& minute,
		DateTime& now);

	void handleKeyEvent(TCHAR& key);

#endif

#ifdef TBC_OS_WINDOWS

private:
	AeCondition* m_condition1;
	AeCondition* m_condition2;
	AeCategory* m_timeTick;
	AeCategory* m_keyHit;
	AeCategory* m_timeSlot;
	std::vector<Variant> m_eventAttributes1;
	std::vector<Variant> m_eventAttributes2;
	std::vector<AeCondition*> m_conditionList;

#endif

	ShutdownRequestHandler m_ShutdownRequest;

};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_
