#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

OpcServer* instance = NULL;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

tstring COND_NAME_BETWEEN_SINGLE    = _T("between [10,50]");
tstring COND_NAME_BETWEEN_MULTIPLE  = _T("between");

const unsigned long CAT_ID_TIME_TICK    = 1;
const unsigned long CAT_ID_TIME_SLOT    = 2;

const unsigned char MyDaAddressSpaceElement::TYPE_UNKNOWN       = 0x00;
const unsigned char MyDaAddressSpaceElement::TYPE_NODEMATH      = 0x80;
const unsigned char MyDaAddressSpaceElement::TYPE_NODERANDOM    = 0x40;
const unsigned char MyDaAddressSpaceElement::TYPE_RANDOM_100MS  = 0x01;
const unsigned char MyDaAddressSpaceElement::TYPE_RANDOM_1S     = 0x02;
const unsigned char MyDaAddressSpaceElement::TYPE_SINE          = 0x04;
const unsigned char MyDaAddressSpaceElement::TYPE_ACCEPT        = 0x08;

long API_CALL handleShutdownRequest(void)
{
#ifdef TBC_OS_WINDOWS
	SetEvent(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
	g_events.signal(0);
#endif
	return S_OK;
}   //  end HandleShutdownRequest

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


OpcServer::OpcServer(void)
{
	m_sin = NULL;
	m_rand100ms = NULL;
	m_rand1s = NULL;
#ifdef TBC_OS_WINDOWS
	m_timeTick = NULL;
	m_timeSlot = NULL;
	m_condition1 = NULL;
	m_condition2 = NULL;
#endif
}   //  end constructor


OpcServer::~OpcServer(void)
{
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{04789CF1-20C8-4a6e-B405-BE84A79695AA}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Console.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Console.DA"));
#ifdef TBC_OS_WINDOWS
	getApp()->setClsidAe(_T("{FBAF1760-F61E-4602-829F-5CCA0D18766B}"));
	getApp()->setProgIdAe(_T("Softing.OPCToolbox.C++.Sample.Console.AE.1"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPCToolbox.C++.Sample.Console.AE"));
#endif
	getApp()->setDescription(_T("Softing OPC Toolkit C++ Console Sample Server"));
	getApp()->setServiceName(tstring());
	getApp()->setIpPortHTTP(8079);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(tstring(_T("Softing Industrial Automation GmbH")));
	getApp()->setMinUpdateRateDa(1000);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('/'));
	getApp()->setPropertyDelimiter(_T('#'));
	getApp()->setSupportDisableConditions(true);
	getApp()->ShutdownRequest = handleShutdownRequest;
	getApp()->setWebRootDirectory(_T("www"));
	getApp()->setWebRootFile(_T("index.html"));
	getApp()->setWebAdministratorPassword(_T("ad"));
	getApp()->setWebOperatorPassword(_T("op"));
	return S_OK;
	//  proceed with initializing of the Toolkit tracing mechanism
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_SERVER,
		EnumTraceGroup_SERVER,
		_T("Trace.txt"),
		1000000,
		0);
}   //  end initialize


void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName


long OpcServer::prepare(MyCreator* creator)
{
	long result = S_OK;

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Server feature
	//result = getApp()->activate(EnumFeature_DA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Server Feature
	//result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the COM-AE Server Feature
	//result = getApp()->activate(EnumFeature_AE_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  END TODO - binary license activation
	//  a valid creator must be provided to the initialize
	result = getApp()->initialize(creator);

	if (!SUCCEEDED(result))
	{
		return result;
	}

	return result;
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
#ifdef TBC_OS_WINDOWS

	if (m_timeTick)
	{
		delete m_timeTick;
		m_timeTick = NULL;
	}

	if (m_timeSlot)
	{
		delete m_timeSlot;
		m_timeSlot = NULL;
	}

	if (m_condition1)
	{
		delete m_condition1;
		m_condition1 = NULL;
	}

	if (m_condition2)
	{
		delete m_condition2;
		m_condition2 = NULL;
	}

#endif
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate


long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine


long OpcServer::buildDaNameSpace(void)
{
	MyDaAddressSpaceElement* tag, *node;
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	tag = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	tstring aName(_T("accept"));
	tag->setName(aName);
	tag->setType(MyDaAddressSpaceElement::TYPE_ACCEPT);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I4);
	tag->setIoMode(EnumIoMode_POLL);
	daRoot->addChild(tag);
	node = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("maths"));
	node->setName(aName);
	node->setType(MyDaAddressSpaceElement::TYPE_NODEMATH);
	node->setIoMode(EnumIoMode_NONE);
	node->setHasChildren(TRUE);
	daRoot->addChild(node);
	m_sin = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("sin"));
	m_sin->setName(aName);
	m_sin->setType(MyDaAddressSpaceElement::TYPE_SINE);
	m_sin->setAccessRights(EnumAccessRights_READABLE);
	m_sin->setDatatype(VT_R8);
	m_sin->setIoMode(EnumIoMode_REPORT);
	node->addChild(m_sin);
	DaProperty* newProperty = new DaProperty();
	newProperty->setId(6020);
	tstring propName(_T("Angle"));
	newProperty->setName(propName);
	tstring propDescription(_T("Angle Description"));
	newProperty->setDescription(propDescription);
	newProperty->setItemId(propName);
	newProperty->setDatatype(VT_I2);
	newProperty->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_sin->addProperty(newProperty);
	m_sin->setEUInfoAnalog(-1.0, 1.0);
	node = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("random"));
	node->setName(aName);
	node->setType(MyDaAddressSpaceElement::TYPE_NODERANDOM);
	node->setIoMode(EnumIoMode_NONE);
	node->setHasChildren(TRUE);
	daRoot->addChild(node);
	m_rand100ms = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("100ms"));
	m_rand100ms->setName(aName);
	m_rand100ms->setType(MyDaAddressSpaceElement::TYPE_RANDOM_100MS);
	m_rand100ms->setAccessRights(EnumAccessRights_READABLE);
	m_rand100ms->setDatatype(VT_I4);
	m_rand100ms->setIoMode(EnumIoMode_REPORT);
	node->addChild(m_rand100ms);
	m_rand1s = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("1s"));
	m_rand1s->setName(aName);
	m_rand1s->setType(MyDaAddressSpaceElement::TYPE_RANDOM_1S);
	m_rand1s->setAccessRights(EnumAccessRights_READABLE);
	m_rand1s->setDatatype(VT_I4);
	m_rand1s->setIoMode(EnumIoMode_REPORT);
	node->addChild(m_rand1s);
	return S_OK;
}   //  end buildDaNameSpace



void OpcServer::changeValue(Variant& aVar, DAVariableType varType)
{
	DateTime now;
	now.now();
	ValueQT val(aVar, EnumQuality_GOOD, now);

	switch (varType)
	{
	case Sin:
	{
		if (m_sin)
		{
			m_sin->valueChanged(val);
		}
	}
	break;

	case Rand100:
	{
		if (m_rand100ms)
		{
			m_rand100ms->valueChanged(val);
		}
	}
	break;

	case Rand1:
	{
		if (m_rand1s)
		{
			m_rand1s->valueChanged(val);
		}
	}
	break;

	default:
	{
		//nothing to do
	}
	}
}


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


#ifdef TBC_OS_WINDOWS


long OpcServer::buildAeNameSpace(void)
{
	//  AE
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	AeAddressSpaceRoot* aeRoot = getApp()->getAeAddressSpaceRoot();
	AeAddressSpaceElement* computer = creator->createMyAeAddressSpaceElement();
	tstring aName(_T("computer"));
	computer->setName(aName);
	computer->setHasChildren(TRUE);
	aeRoot->addChild(computer);
	AeAddressSpaceElement* clock = creator->createMyAeAddressSpaceElement();
	aName = tstring(_T("clock"));
	clock->setName(aName);
	clock->setHasChildren(TRUE);
	computer->addChild(clock);
	AeAddressSpaceElement* timer = creator->createMyAeAddressSpaceElement();
	aName = tstring(_T("timer"));
	timer->setName(aName);
	timer->setHasChildren(FALSE);
	clock->addChild(timer);
	timer = creator->createMyAeAddressSpaceElement();
	aName = tstring(_T("time slot 1"));
	timer->setName(aName);
	timer->setHasChildren(FALSE);
	clock->addChild(timer);
	return S_OK;
}   //  end buildAeNameSpace


long OpcServer::buildEventCategories(void)
{
	long result = S_OK;
	tstring timeTick(_T("time tick"));
	m_timeTick = new AeCategory(EnumEventType_SIMPLE, CAT_ID_TIME_TICK, timeTick);
	result = getApp()->addAeCategory(m_timeTick);

	if (result == S_OK)
	{
		tstring hour(_T("hour"));
		m_timeTick->addAttribute(1, hour, VT_UI2);
		tstring minute(_T("minute"));
		m_timeTick->addAttribute(2, minute, VT_UI2);
		tstring second(_T("second"));
		m_timeTick->addAttribute(3, second, VT_UI2);
	}   //  end if

	tstring timeSlot(_T("time slot 1"));
	m_timeSlot = new AeCategory(EnumEventType_CONDITION, CAT_ID_TIME_SLOT, timeSlot);
	result = getApp()->addAeCategory(m_timeSlot);

	if (result == S_OK)
	{
		tstring second(_T("second"));
		m_timeSlot->addAttribute(1, second, VT_UI2);
		m_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_SINGLE);
		m_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_MULTIPLE);
		tstring str(_T("[10,30]"));
		m_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
		str = _T("[30,50]");
		m_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
	}   //  end if

	return S_OK;
}   //  end buildEventCategories



void OpcServer::initializeConditions(void)
{
	m_condition1 = new AeCondition();
	m_condition2 = new AeCondition();
	m_eventAttributes1.push_back(Variant(1));
	m_eventAttributes2.push_back(Variant(2));
	m_condition1->setCategory(m_timeSlot);
	m_condition1->setName(COND_NAME_BETWEEN_MULTIPLE);
	m_condition1->setDefinition(m_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_MULTIPLE));
	tstring sourcePath(_T("computer/clock/time slot 1"));
	m_condition1->setSourcePath(sourcePath);
	m_condition1->setQuality(EnumQuality_GOOD);
	m_condition1->setEventAttributes(m_eventAttributes1);
	m_condition1->setUserData(1);
	m_condition2->setCategory(m_timeSlot);
	m_condition2->setName(COND_NAME_BETWEEN_SINGLE);
	m_condition2->setDefinition(m_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_SINGLE));
	sourcePath = tstring(_T("computer/clock/time slot 1"));
	m_condition2->setSourcePath(sourcePath);
	m_condition2->setQuality(EnumQuality_GOOD);
	m_condition2->setEventAttributes(m_eventAttributes2);
	m_condition2->setUserData(2);
}  // end initializeConditions


void OpcServer::updateAECondition(
	unsigned long severity,
	tstring& message,
	BOOL ackReq,
	tstring& subCondName,
	unsigned short changeMask,
	EnumConditionState conditionState,
	AEConditionIndex conditionIndex)
{
	switch (conditionIndex)
	{
	case FirstCondition:
	{
		m_condition1->setSeverity(severity);
		m_condition1->setMessage(message);
		m_condition1->setAckRequired(ackReq);
		m_condition1->setChangeMask(changeMask);
		m_condition1->setStateChange(conditionState);

		if (!subCondName.empty())
		{
			m_condition1->setActiveSubConditionName(subCondName);
		}

		m_conditionList.push_back(m_condition1);
	}
	break;

	case SecondCondition:
	{
		m_condition2->setSeverity(severity);
		m_condition2->setMessage(message);
		m_condition2->setAckRequired(ackReq);
		m_condition2->setChangeMask(changeMask);
		m_condition2->setStateChange(conditionState);

		if (!subCondName.empty())
		{
			m_condition2->setActiveSubConditionName(subCondName);
		}

		m_conditionList.push_back(m_condition2);
	}
	break;
	}
}  // end updateAECondition

void OpcServer::clearConditionList()
{
	m_conditionList.clear();
}  // end clearConditionList


void OpcServer::changeConditions()
{
	if (m_conditionList.size() > 0)
	{
		getApp()->changeConditions(m_conditionList);
	}   //  end if
}  // end changeConditions


void OpcServer::handleSimpleEvents(
	unsigned short& second,
	unsigned short& minute,
	unsigned short& hour,
	DateTime& now)
{
	std::vector<AeEvent*> events;
	std::vector<Variant> attributeValues;
	attributeValues.push_back(now.second());
	attributeValues.push_back(now.minute());
	attributeValues.push_back(now.hour());
	AeEvent secondEvent(EnumEventType_SIMPLE, tstring(_T("computer/clock/timer")), CAT_ID_TIME_TICK);
	AeEvent minuteEvent(EnumEventType_SIMPLE, tstring(_T("computer/clock/timer")), CAT_ID_TIME_TICK);
	AeEvent hourEvent(EnumEventType_SIMPLE, tstring(_T("computer/clock/timer")), CAT_ID_TIME_TICK);
	tstring anActorID;
	tstring aMessage;

	if (second != now.second())
	{
		second = now.second();
		secondEvent.setSeverity(10);
		anActorID = tstring(_T("sample"));
		secondEvent.setActorId(anActorID);
		aMessage = tstring(_T("Second Changed"));
		secondEvent.setMessage(aMessage);
		secondEvent.setAttributeValueList(attributeValues);
		events.push_back(&secondEvent);
	}   //  end if

	if (minute != now.minute())
	{
		minute = now.minute();
		minuteEvent.setSeverity(490);
		anActorID = tstring(_T("sample"));
		minuteEvent.setActorId(anActorID);
		aMessage = tstring(_T("Minute Changed"));
		minuteEvent.setMessage(aMessage);
		minuteEvent.setAttributeValueList(attributeValues);
		events.push_back(&minuteEvent);
	}   //  end if

	if (hour != now.hour())
	{
		hour = now.hour();
		hourEvent.setSeverity(990);
		anActorID = tstring(_T("sample"));
		hourEvent.setActorId(anActorID);
		aMessage = tstring(_T("Hour Changed"));
		hourEvent.setMessage(aMessage);
		hourEvent.setAttributeValueList(attributeValues);
		events.push_back(&hourEvent);
	}   //  end if

	getApp()->fireEvents(events);
}  // end handleSimpleEvents

#endif
