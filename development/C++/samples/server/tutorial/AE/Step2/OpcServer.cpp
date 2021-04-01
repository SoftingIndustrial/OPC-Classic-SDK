#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

OpcServer* instance = NULL;

const unsigned long CATEGORY_ID_SIMULATION  = 1;
const unsigned long ATTRIBUTE_ID_SIMULATION = 1;

OpcServer* getOpcServer(void)
{
	return instance;
}   //  end getOpcServer


void createOpcServer(void)
{
	if (instance == NULL)
	{
		instance = new OpcServer();
	}   //  end if
}   //  end createOpcServer


void destroyOpcServer(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}   //  end if
}   //  end destroyOpcServer


long API_CALL HandleShutdownRequest(void)
{
	return S_OK;
}   //end HandleShutdownRequest


OpcServer::OpcServer(void)
{
}   //  end constructor


OpcServer::~OpcServer(void)
{
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidAe(_T("{E23019EE-C8DC-4C1B-8D88-93C916CEE8B0}"));
	getApp()->setProgIdAe(_T("Softing.OPCToolbox.C++.Tutorial.AE.1"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPCToolbox.C++.Tutorial.AE"));
	getApp()->setDescription(_T("Softing OPC Toolkit C++ Tutorial Server"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(_T("Softing Industrial Automation GmbH"));
	getApp()->setMinUpdateRateDa(1000);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('/'));
	getApp()->setSupportDisableConditions(true);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_SERVER,
		EnumTraceGroup_SERVER,
		_T("Trace.txt"),
		1000000,
		0);
	return S_OK;
}   //  end initialize

void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName

long OpcServer::prepare(MyCreator* creator)
{
	return getApp()->initialize(creator);
}   //  end prepare

long OpcServer::start(void)
{
	return getApp()->start();
}   //  end start

long OpcServer::ready(void)
{
	return getApp()->ready();
}   //  end ready

long OpcServer::stop(void)
{
	return getApp()->stop();
}   //  end stop

long OpcServer::terminate(void)
{
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate

long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine


long OpcServer::buildAddressSpace(void)
{
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	tstring aName;
	//  AE
	AeAddressSpaceRoot* aeRoot = getApp()->getAeAddressSpaceRoot();
	AeAddressSpaceElement* aeElement = creator->createAeAddressSpaceElement();
	aName = tstring(_T("SimulationElement"));
	aeElement->setName(aName);
	aeElement->setHasChildren(FALSE);
	aeRoot->addChild(aeElement);
	return S_OK;
}   //  end buildAddressSpace

long OpcServer::buildEventCategories(void)
{
	tstring categoryName(_T("SimulationCategory"));
	AeCategory* simulationCategory = new AeCategory(EnumEventType_SIMPLE, CATEGORY_ID_SIMULATION, categoryName);
	getApp()->addAeCategory(simulationCategory);
	tstring attributeName(_T("SimulationAttribute"));
	simulationCategory->addAttribute(ATTRIBUTE_ID_SIMULATION, attributeName, VT_UI2);
	return S_OK;
}   //  end buildEventCategories


void OpcServer::fireSimulationEvents(void)
{
	// simple events
	std::vector<AeEvent*> events;
	std::vector<Variant> attributeValues;
	tstring message;
	tstring anActorID;
	AeEvent simulationEvent(EnumEventType_SIMPLE, tstring(_T("SimulationElement")), CATEGORY_ID_SIMULATION);
	simulationEvent.setSeverity(10);
	anActorID = tstring(_T("OpcUser"));
	simulationEvent.setActorId(anActorID);
	message = tstring(_T("Simulation event generated"));
	simulationEvent.setMessage(message);
	Variant randomValue(::rand());
	attributeValues.push_back(randomValue);
	simulationEvent.setAttributeValueList(attributeValues);
	events.push_back(&simulationEvent);
	getApp()->fireEvents(events);
}   //  end fireEvents



void OpcServer::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	const TCHAR* anObjectID,
	const TCHAR* aMessage,
	...)
{
	const unsigned long LENGTH = 400;
	TCHAR buffer[LENGTH + 1] = { 0 };
	va_list arglist;
	va_start(arglist, aMessage);
#ifdef TBC_OS_WINDOWS
	_vsntprintf(buffer, LENGTH, aMessage, arglist);
#endif
#ifdef TBC_OS_LINUX
	vsnprintf(buffer, LENGTH, aMessage, arglist);
#endif
	getApp()->trace(aLevel, aMask, anObjectID, buffer);
}   //  end trace
