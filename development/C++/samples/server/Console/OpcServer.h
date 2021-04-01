#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

#include "MyCreator.h"

using namespace SoftingOPCToolboxServer;

typedef enum
{
	Sin = 0,
	Rand100,
	Rand1
} DAVariableType;

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
	long buildDaNameSpace(void);

	void changeValue(Variant& aVar, DAVariableType varType);
	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

#ifdef TBC_OS_WINDOWS
	long buildAeNameSpace(void);
	long buildEventCategories(void);
	void initializeConditions(void);
	void updateAECondition(
		unsigned long severity,
		tstring& message,
		BOOL ackReq,
		tstring& subCondName,
		unsigned short changeMask,
		EnumConditionState conditionState,
		AEConditionIndex conditionIndex);

	void clearConditionList();
	void changeConditions();

	void handleSimpleEvents(
		unsigned short& second,
		unsigned short& minute,
		unsigned short& hour,
		DateTime& now);
#endif

private:
	MyDaAddressSpaceElement* m_sin;
	MyDaAddressSpaceElement* m_rand100ms;
	MyDaAddressSpaceElement* m_rand1s;

#ifdef TBC_OS_WINDOWS
	AeCategory* m_timeTick;
	AeCategory* m_timeSlot;
	AeCondition* m_condition1;
	AeCondition* m_condition2;
	std::vector<Variant> m_eventAttributes1;
	std::vector<Variant> m_eventAttributes2;
	std::vector<AeCondition*> m_conditionList;
#endif
};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_
