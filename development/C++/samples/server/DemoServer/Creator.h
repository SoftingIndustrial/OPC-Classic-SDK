#ifndef _DEMOCREATOR_H_
#define _DEMOCREATOR_H_

#include "DaAddressSpaceElement.h"
#include "AeAddressSpaceElement.h"
#include "DaAddressSpaceRoot.h"
#include "Transaction.h"
#include "Request.h"
#include "Function.h"
#include "DefaultElement.h"
#include "TimeVariable.h"
#include "ServerCreator.h"

using namespace SoftingOPCToolboxServer;

class DemoCreator : public Creator
{
public:
	virtual DaAddressSpaceRoot* createDaAddressSpaceRoot(void);

	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey);

	virtual DaRequest* createRequest(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* pAnElement,
		int aPropertyId,
		unsigned long aRequestHandle);

	virtual DemoDaAddressSpaceElement* createBasicElement(tstring name);

	virtual DemoDaAddressSpaceElement* createBasicStaticElement(tstring name);

	virtual DemoDaAddressSpaceElement* createBasicStaticElement(VARENUM varType);

	virtual DemoDaAddressSpaceElement* createBasicDynamicElement(tstring name);

	virtual DemoDaAddressSpaceElement* createBasicDynamicElement(VARENUM varType);

	virtual DemoDaAddressSpaceElement* createSinusElement();

	virtual DemoDaAddressSpaceElement* createCosinusElement();

	virtual DemoDaAddressSpaceElement* createTanElement();

	virtual DemoDaAddressSpaceElement* createSimulationElement();

	virtual DemoDaAddressSpaceElement* createSleepIntervalElement();

	virtual DemoDaAddressSpaceElement* createTimeElement(tstring name);

	virtual DemoDaAddressSpaceElement* createTimeElement(
		tstring varName,
		enum TimeVariable::TimeType type,
		enum TimeVariable::TimeZone timeZone);

	virtual DemoDaAddressSpaceElement* createAlarmSimulationElement(
		tstring varName,
		enum AlarmSimulation::AlarmType type);

	virtual DemoDaAddressSpaceElement* createKeyElement();

	virtual DemoAeAddressSpaceElement* createDemoAeAddressSpaceElement();

};  //  end class DemoCreator

#endif
