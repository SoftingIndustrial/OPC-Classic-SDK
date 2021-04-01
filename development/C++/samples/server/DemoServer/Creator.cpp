#include "stdafx.h"
#include "Creator.h"


DaAddressSpaceRoot* DemoCreator::createDaAddressSpaceRoot(void)
{
	return (DaAddressSpaceRoot*) new DemoDaAddressSpaceRoot();
}   //  end createDaAddressSpaceRoot

DaTransaction* DemoCreator::createTransaction(
	EnumTransactionType aTransactionType,
	std::vector<DaRequest*>& aRequestList,
	unsigned long aSessionKey)
{
	return (DaTransaction*) new Transaction(aTransactionType, aRequestList, aSessionKey);
}   //  end createTransaction

DemoDaAddressSpaceElement* DemoCreator::createBasicElement(tstring name)
{
	return (DemoDaAddressSpaceElement*) new BasicElement(name);
}

DemoDaAddressSpaceElement* DemoCreator::createBasicStaticElement(tstring name)
{
	return (DemoDaAddressSpaceElement*) new BasicStaticElement(name);
}

DemoDaAddressSpaceElement* DemoCreator::createBasicStaticElement(VARENUM varType)
{
	return (DemoDaAddressSpaceElement*) new BasicStaticElement(varType);
}

DemoDaAddressSpaceElement* DemoCreator::createBasicDynamicElement(tstring name)
{
	return (DemoDaAddressSpaceElement*) new BasicDymanicElement(name);
}

DemoDaAddressSpaceElement* DemoCreator::createBasicDynamicElement(VARENUM varType)
{
	return (DemoDaAddressSpaceElement*) new BasicDymanicElement(varType);
}

DemoDaAddressSpaceElement* DemoCreator::createSinusElement()
{
	return (DemoDaAddressSpaceElement*) new SinFunction();
}

DemoDaAddressSpaceElement* DemoCreator::createCosinusElement()
{
	return (DemoDaAddressSpaceElement*) new CosFunction();
}

DemoDaAddressSpaceElement* DemoCreator::createTanElement()
{
	return (DemoDaAddressSpaceElement*) new TanFunction();
}

DemoDaAddressSpaceElement* DemoCreator::createSimulationElement()
{
	return (DemoDaAddressSpaceElement*) new SimulationVariable();
}

DemoDaAddressSpaceElement* DemoCreator::createSleepIntervalElement()
{
	return (DemoDaAddressSpaceElement*) new SleepIntervalVariable();
}

DemoDaAddressSpaceElement* DemoCreator::createTimeElement(tstring name)
{
	return (DemoDaAddressSpaceElement*) new TimeVariable(name);
}

DemoDaAddressSpaceElement* DemoCreator::createTimeElement(
	tstring varName,
	enum TimeVariable::TimeType type,
	enum TimeVariable::TimeZone timeZone)
{
	return (DemoDaAddressSpaceElement*) new TimeVariable(varName, type, timeZone);
}

DemoDaAddressSpaceElement* DemoCreator::createAlarmSimulationElement(
	tstring varName,
	enum AlarmSimulation::AlarmType type)
{
	return (DemoDaAddressSpaceElement*) new AlarmSimulation(varName, type);
}

DemoDaAddressSpaceElement* DemoCreator::createKeyElement()
{
	return (DemoDaAddressSpaceElement*) new KeyVariable();
}

DemoAeAddressSpaceElement* DemoCreator::createDemoAeAddressSpaceElement()
{
	return new DemoAeAddressSpaceElement();
}

DaRequest* DemoCreator::createRequest(
	EnumTransactionType aTransactionType,
	unsigned long aSessionHandle,
	DaAddressSpaceElement* pAnElement,
	int aPropertyId,
	unsigned long aRequestHandle)
{
	return new Request(aTransactionType, aSessionHandle, pAnElement, aPropertyId, aRequestHandle);
}   //  end createRequest
