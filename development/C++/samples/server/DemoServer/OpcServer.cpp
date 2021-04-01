#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"
#include "Function.h"
#include "TimeVariable.h"

#ifdef TBC_OS_WINDOWS
extern tstring COND_NAME_BETWEEN_SINGLE;
extern tstring COND_NAME_BETWEEN_MULTIPLE;

extern unsigned long CAT_ID_TIME_TICK   = 1;
extern AeCategory* g_timeTick;

extern unsigned long CAT_ID_KEY_HIT = 2;
extern AeCategory* g_keyHit;

extern unsigned long CAT_ID_TIME_SLOT   = 3;
extern AeCategory* g_timeSlot;

extern unsigned long CAT_ID_DA_WRITE_SIMPLE = 4;
extern AeCategory* g_daSimpleWrite;

extern unsigned long CAT_ID_DA_WRITE_TRACKING   = 5;
extern AeCategory* g_daTrackingWrite;
#endif

extern HANDLE g_endEvent;

OpcServer* instance = NULL;


OpcServer* getOpcServer(void)
{
	return instance;
}   //  end getOpcServer


void createOpcServer(void)
{
	if (!instance)
	{
		instance = new OpcServer();
	}   //  end if
}   //  end createOpcServer


void destroyOpcServer(void)
{
	if (!instance)
	{
		delete instance;
		instance = NULL;
	}   //  end if
}   //  end destroyOpcServer


long API_CALL HandleShutdownRequest(void)
{
	SetEvent(g_endEvent);
	return S_OK;
}   //end HandleShutdownRequest


OpcServer::OpcServer(void)
{
	m_pkeyElement = NULL;
}   //  end constructor


OpcServer::~OpcServer(void)
{
	m_elementList.clear();
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{5B5EBB5B-706E-404E-93D5-72A77212A14B}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Demo.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Demo.DA"));
#ifdef TBC_OS_WINDOWS
	getApp()->setClsidAe(_T("{C4ACF9BF-9B2C-4CF2-AC40-43222862CE0F}"));
	getApp()->setProgIdAe(_T("Softing.OPCToolbox.C++.Sample.Demo.AE.1"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPCToolbox.C++.Sample.Demo.AE"));
#endif
	getApp()->setDescription(_T("Softing OPC Toolkit C++ Demo Sample Server"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(_T("Softing Industrial Automation GmbH"));
	getApp()->setMinUpdateRateDa(1000);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('/'));
	getApp()->setIpPortHTTP(8079);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setSupportDisableConditions(true);
	getApp()->ShutdownRequest = HandleShutdownRequest;
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

long OpcServer::prepare(DemoCreator* pCreator)
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
	// result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

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
	result = getApp()->initialize(pCreator);

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
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate

long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine

DemoDaAddressSpaceElement* OpcServer::getKeyElement()
{
	return m_pkeyElement;
}   //  end getKeyElement

long OpcServer::buildAddressSpace(void)
{
	long res = EnumResultCode_S_OK;
	res = buildDAAddressSpace();

	if (res != EnumResultCode_S_OK)
	{
		return res;
	}

#ifdef TBC_OS_WINDOWS
	res = buildAEAddressSpace();

	if (res != EnumResultCode_S_OK)
	{
		return res;
	}

	res = buildAEEventCategories();
#endif
	return res;
}   //  end buildAddressSpace

long OpcServer::buildDAAddressSpace(void)
{
	DemoCreator* creator = (DemoCreator*)getApp()->getCreator();

	if (creator == NULL)
	{
		return EnumResultCode_E_FAIL;
	}

	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();

	if (daRoot == NULL)
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* allDataTypesTag = creator->createBasicElement(tstring(_T("all data types")));
	allDataTypesTag->setHasChildren(TRUE);

	if (!daRoot->addChild(allDataTypesTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* dynamicTag = creator->createBasicElement(tstring(_T("dynamic")));
	dynamicTag->setHasChildren(TRUE);

	if (!allDataTypesTag->addChild(dynamicTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* tag = NULL;
	tag = creator->createBasicDynamicElement(VT_I1);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_UI1);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_I2);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_UI2);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_I4);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_UI4);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_I8);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_UI8);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_R4);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_BOOL);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_BSTR);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement(VT_DATE);

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_I1));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_UI1));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_I2));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_UI2));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_I4));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_UI4));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_R4));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_BOOL));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_BSTR));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	tag = creator->createBasicDynamicElement((VARENUM)(VT_ARRAY | VT_DATE));

	if (!dynamicTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	m_elementList.push_back(tag);
	DemoDaAddressSpaceElement* staticTag = creator->createBasicElement(tstring(_T("static")));
	staticTag->setHasChildren(TRUE);

	if (!allDataTypesTag->addChild(staticTag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag = creator->createBasicStaticElement(VT_I1);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_UI1);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_I2);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_UI2);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_I4);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_UI4);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_I8);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_UI8);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_R4);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_BOOL);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_BSTR);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement(VT_DATE);

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_I1));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_UI1));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_I2));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_UI2));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_I4));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_UI4));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_R4));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_BOOL));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_BSTR));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	tag = creator->createBasicStaticElement((VARENUM)(VT_ARRAY | VT_DATE));

	if (!staticTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag->init();
	DemoDaAddressSpaceElement* functionTag = creator->createBasicElement(tstring(_T("function")));
	functionTag->setHasChildren(TRUE);

	if (!daRoot->addChild(functionTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* cosTag = creator->createCosinusElement();

	if (!functionTag->addChild(cosTag))
	{
		return EnumResultCode_E_FAIL;
	}

	m_elementList.push_back(cosTag);
	DemoDaAddressSpaceElement* sinTag = creator->createSinusElement();

	if (!functionTag->addChild(sinTag))
	{
		return EnumResultCode_E_FAIL;
	}

	m_elementList.push_back(sinTag);
	DemoDaAddressSpaceElement* tanTag = creator->createTanElement();

	if (!functionTag->addChild(tanTag))
	{
		return EnumResultCode_E_FAIL;
	}

	m_elementList.push_back(tanTag);
	DemoDaAddressSpaceElement* pSimVariable = creator->createSimulationElement();

	if (!functionTag->addChild(pSimVariable))
	{
		return EnumResultCode_E_FAIL;
	}

	pSimVariable->init();
	DemoDaAddressSpaceElement* pSleepIntVar = creator->createSleepIntervalElement();

	if (!functionTag->addChild(pSleepIntVar))
	{
		return EnumResultCode_E_FAIL;
	}

	pSleepIntVar->init();
	DemoDaAddressSpaceElement* timeTag = creator->createBasicElement(tstring(_T("time")));
	timeTag->setHasChildren(TRUE);

	if (!daRoot->addChild(timeTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* date = creator->createTimeElement(tstring(_T("date")), TimeVariable::date, TimeVariable::local);

	if (!timeTag->addChild(date))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* local = creator->createBasicElement(tstring(_T("local")));
	local->setHasChildren(TRUE);

	if (!timeTag->addChild(local))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* localHour = creator->createTimeElement(tstring(_T("hour")), TimeVariable::hour, TimeVariable::local);

	if (!local->addChild(localHour))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* localMinute = creator->createTimeElement(tstring(_T("minute")), TimeVariable::minute, TimeVariable::local);

	if (!local->addChild(localMinute))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* localSecond = creator->createTimeElement(tstring(_T("second")), TimeVariable::second, TimeVariable::local);

	if (!local->addChild(localSecond))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* localString = creator->createTimeElement(tstring(_T("string")), TimeVariable::string, TimeVariable::local);

	if (!local->addChild(localString))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* localArray = creator->createTimeElement(tstring(_T("array")), TimeVariable::array, TimeVariable::local);

	if (!local->addChild(localArray))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmt = creator->createBasicElement(tstring(_T("GMT")));
	gmt->setHasChildren(TRUE);

	if (!timeTag->addChild(gmt))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmtHour = creator->createTimeElement(tstring(_T("hour")), TimeVariable::hour, TimeVariable::GMT);

	if (!gmt->addChild(gmtHour))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmtMinute = creator->createTimeElement(tstring(_T("minute")), TimeVariable::minute, TimeVariable::GMT);

	if (!gmt->addChild(gmtMinute))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmtSecond = creator->createTimeElement(tstring(_T("second")), TimeVariable::second, TimeVariable::GMT);

	if (!gmt->addChild(gmtSecond))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmtString = creator->createTimeElement(tstring(_T("string")), TimeVariable::string, TimeVariable::GMT);

	if (!gmt->addChild(gmtString))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* gmtArray = creator->createTimeElement(tstring(_T("array")), TimeVariable::array, TimeVariable::GMT);

	if (!gmt->addChild(gmtArray))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* timeSlot = creator->createBasicElement(tstring(_T("time slot")));
	timeSlot->setHasChildren(TRUE);

	if (!timeTag->addChild(timeSlot))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot1 = creator->createBasicElement(tstring(_T("slot 1")));
	slot1->setHasChildren(TRUE);

	if (!timeSlot->addChild(slot1))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot1Start = creator->createTimeElement(tstring(_T("start")), TimeVariable::limitSecond, TimeVariable::none);

	if (!slot1->addChild(slot1Start))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot1End = creator->createTimeElement(tstring(_T("end")), TimeVariable::limitSecond, TimeVariable::none);

	if (!slot1->addChild(slot1End))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot2 = creator->createBasicElement(tstring(_T("slot 2")));
	slot2->setHasChildren(TRUE);

	if (!timeSlot->addChild(slot2))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot2Start = creator->createTimeElement(tstring(_T("start")), TimeVariable::limitSecond, TimeVariable::none);

	if (!slot2->addChild(slot2Start))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* slot2End = creator->createTimeElement(tstring(_T("end")), TimeVariable::limitSecond, TimeVariable::none);

	if (!slot2->addChild(slot2End))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* alarmSimulationTag = creator->createBasicElement(tstring(_T("alarm simulation")));
	alarmSimulationTag->setHasChildren(TRUE);

	if (!daRoot->addChild(alarmSimulationTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* fireSimpleEvent = creator->createAlarmSimulationElement(tstring(_T("fire simple event")), AlarmSimulation::simple);

	if (!alarmSimulationTag->addChild(fireSimpleEvent))
	{
		return EnumResultCode_E_FAIL;
	}

	fireSimpleEvent->init();
	DemoDaAddressSpaceElement* fireTrackingEvent = creator->createAlarmSimulationElement(tstring(_T("fire tracking event")), AlarmSimulation::tracking);

	if (!alarmSimulationTag->addChild(fireTrackingEvent))
	{
		return EnumResultCode_E_FAIL;
	}

	fireTrackingEvent->init();
	DemoDaAddressSpaceElement* keyboard = creator->createBasicElement(tstring(_T("keyboard")));
	keyboard->setHasChildren(TRUE);

	if (!daRoot->addChild(keyboard))
	{
		return EnumResultCode_E_FAIL;
	}

	m_pkeyElement = creator->createKeyElement();

	if (!keyboard->addChild(m_pkeyElement))
	{
		return EnumResultCode_E_FAIL;
	}

	m_pkeyElement->init();
#ifdef UNICODE
	DemoDaAddressSpaceElement* unicodeSpecificTag = creator->createBasicElement(tstring(_T("unicode specific")));
	unicodeSpecificTag->setHasChildren(TRUE);

	if (!daRoot->addChild(unicodeSpecificTag))
	{
		return EnumResultCode_E_FAIL;
	}

	DemoDaAddressSpaceElement* chineaseTag = creator->createBasicElement(tstring(_T("chinease")));
	chineaseTag->setHasChildren(TRUE);

	if (!unicodeSpecificTag->addChild(chineaseTag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag = creator->createBasicStaticElement(VT_BSTR);
	tstring chineaseName = _T("中国的");
	tag->setName(chineaseName);

	if (!chineaseTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	DateTime time;
	time.now();
	Variant theValue;
	theValue.setBSTR(_T("中国的"));
	ValueQT chineaseValueQT(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(chineaseValueQT);
	DaProperty* chProperty = new DaProperty();
	chProperty->setId(100);
	tstring chPropName(_T("中国的"));
	chProperty->setName(chPropName);
	tstring chPropDescription(_T("中国的 Chinease"));
	chProperty->setDescription(chPropDescription);
	chProperty->setItemId(chPropName);
	chProperty->setDatatype(VT_BSTR);
	chProperty->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->addProperty(chProperty);
	DemoDaAddressSpaceElement* japaneseTag = creator->createBasicElement(tstring(_T("japanese")));
	japaneseTag->setHasChildren(TRUE);

	if (!unicodeSpecificTag->addChild(japaneseTag))
	{
		return EnumResultCode_E_FAIL;
	}

	tag = creator->createBasicStaticElement(VT_BSTR);
	tstring japaneseName = _T("御国言葉");
	tag->setName(japaneseName);

	if (!japaneseTag->addChild(tag))
	{
		return EnumResultCode_E_FAIL;
	}

	time.now();
	theValue.setBSTR(_T("御国言葉"));
	ValueQT japaneseValueQT(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(japaneseValueQT);
	DaProperty* jpProperty = new DaProperty();
	jpProperty->setId(100);
	tstring jpPropName(_T("御国言葉"));
	jpProperty->setName(jpPropName);
	tstring jpPropDescription(_T("御国言葉 Japanese"));
	jpProperty->setDescription(jpPropDescription);
	jpProperty->setItemId(jpPropName);
	jpProperty->setDatatype(VT_BSTR);
	jpProperty->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->addProperty(jpProperty);
#endif // UNICODE
	return S_OK;
} // end buildDAAddressSpace

#ifdef TBC_OS_WINDOWS
long OpcServer::buildAEAddressSpace(void)
{
	DemoCreator* creator = (DemoCreator*)getApp()->getCreator();

	if (creator == NULL)
	{
		return EnumResultCode_E_FAIL;
	}

	AeAddressSpaceRoot* aeRoot = getApp()->getAeAddressSpaceRoot();
	DemoAeAddressSpaceElement* keyboard = creator->createDemoAeAddressSpaceElement();
	tstring aName = tstring(_T("keyboard"));
	keyboard->setName(aName);
	keyboard->setHasChildren(FALSE);
	aeRoot->addChild(keyboard);
	DemoAeAddressSpaceElement* mouse = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("mouse"));
	mouse->setName(aName);
	mouse->setHasChildren(TRUE);
	aeRoot->addChild(mouse);
	DemoAeAddressSpaceElement* rightButton = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("right button"));
	rightButton->setName(aName);
	rightButton->setHasChildren(FALSE);
	mouse->addChild(rightButton);
	DemoAeAddressSpaceElement* middleButton = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("middle"));
	middleButton->setName(aName);
	middleButton->setHasChildren(FALSE);
	mouse->addChild(middleButton);
	DemoAeAddressSpaceElement* time = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("time"));
	time->setName(aName);
	time->setHasChildren(TRUE);
	aeRoot->addChild(time);
	DemoAeAddressSpaceElement* local = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("local"));
	local->setName(aName);
	local->setHasChildren(TRUE);
	time->addChild(local);
	DemoAeAddressSpaceElement* second = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("second"));
	second->setName(aName);
	second->setHasChildren(FALSE);
	local->addChild(second);
	DemoAeAddressSpaceElement* minute = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("minute"));
	minute->setName(aName);
	minute->setHasChildren(FALSE);
	local->addChild(minute);
	DemoAeAddressSpaceElement* alarmSimulation = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("alarm simulation"));
	alarmSimulation->setName(aName);
	alarmSimulation->setHasChildren(TRUE);
	aeRoot->addChild(alarmSimulation);
	DemoAeAddressSpaceElement* fireSimpleEvent = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("fire simple event"));
	fireSimpleEvent->setName(aName);
	fireSimpleEvent->setHasChildren(FALSE);
	alarmSimulation->addChild(fireSimpleEvent);
	DemoAeAddressSpaceElement* fireTrackingEvent = creator->createDemoAeAddressSpaceElement();
	aName = tstring(_T("fire tracking event"));
	fireTrackingEvent->setName(aName);
	fireTrackingEvent->setHasChildren(FALSE);
	alarmSimulation->addChild(fireTrackingEvent);
	return S_OK;
}

long OpcServer::buildAEEventCategories(void)
{
	long result = S_OK;
	tstring timeTick(_T("time tick"));
	g_timeTick = new AeCategory(EnumEventType_SIMPLE, CAT_ID_TIME_TICK, timeTick);
	result = getApp()->addAeCategory(g_timeTick);

	if (result == S_OK)
	{
		tstring hour(_T("hour"));
		g_timeTick->addAttribute(1, hour, VT_UI2);
		tstring minute(_T("minute"));
		g_timeTick->addAttribute(2, minute, VT_UI2);
		tstring second(_T("second"));
		g_timeTick->addAttribute(3, second, VT_UI2);
	} // end if

	tstring keyHit(_T("key hit"));
	g_keyHit = new AeCategory(EnumEventType_TRACKING, CAT_ID_KEY_HIT, keyHit);
	result = getApp()->addAeCategory(g_keyHit);

	if (result == S_OK)
	{
		tstring key(_T("key"));
		g_keyHit->addAttribute(1, key, VT_UI2);
	} // end if

	tstring timeSlot(_T("time slot"));
	g_timeSlot = new AeCategory(EnumEventType_CONDITION, CAT_ID_TIME_SLOT, timeSlot);
	result = getApp()->addAeCategory(g_timeSlot);

	if (result == S_OK)
	{
		tstring second(_T("second"));
		tstring str;
		g_timeSlot->addAttribute(1, second, VT_UI2);
		g_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_SINGLE);
		g_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_MULTIPLE);
		str = _T("[10,30]");
		g_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
		AeSubConditionDefinition* subConditionDefinition = g_timeSlot->getSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);

		if (subConditionDefinition != NULL)
		{
			subConditionDefinition->setDescription(str);
			subConditionDefinition->setSeverity(100);
		}

		str = _T("[30,50]");
		g_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
		subConditionDefinition = g_timeSlot->getSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);

		if (subConditionDefinition != NULL)
		{
			subConditionDefinition->setDescription(str);
			subConditionDefinition->setSeverity(200);
		}
	} // end if

	tstring alarmSimulationSimple(_T("alarm simulation simple"));
	g_daSimpleWrite = new AeCategory(EnumEventType_SIMPLE, CAT_ID_DA_WRITE_SIMPLE, alarmSimulationSimple);
	result = getApp()->addAeCategory(g_daSimpleWrite);
	tstring alarmSimulationTracking(_T("alarm simulation tracking"));
	g_daTrackingWrite = new AeCategory(EnumEventType_TRACKING, CAT_ID_DA_WRITE_TRACKING, alarmSimulationTracking);
	result = getApp()->addAeCategory(g_daTrackingWrite);
	return result;
}
#endif

void OpcServer::changeSimulationValues(void)
{
	std::vector<DemoDaAddressSpaceElement*>::iterator it;

	for (it = m_elementList.begin(); it != m_elementList.end(); ++it)
	{
		(*it)->simulation();
	}
}   //  end changeSimulationValues

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
