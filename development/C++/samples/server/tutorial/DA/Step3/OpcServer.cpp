#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

OpcServer* instance = NULL;


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
	m_pDaSimulationElement = NULL;
	m_reportSimulationElement = NULL;
}   //  end constructor


OpcServer::~OpcServer(void)
{
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{00159634-AAB4-4F25-B75C-A632C476C0D4}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Tutorial.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Tutorial.DA"));
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
	//  DA
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	m_pDaSimulationElement = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("Random value poll"));
	m_pDaSimulationElement->setName(aName);
	m_pDaSimulationElement->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_pDaSimulationElement->setDatatype(VT_I4);
	m_pDaSimulationElement->setIoMode(EnumIoMode_POLL);
	daRoot->addChild(m_pDaSimulationElement);
	DaProperty* firstProperty = new DaProperty();
	firstProperty->setId(6020);
	tstring propName = _T("Angle");
	firstProperty->setName(propName);
	tstring propDescription = _T("Item Description");
	firstProperty->setDescription(propDescription);
	firstProperty->setItemId(propName);
	firstProperty->setDatatype(VT_I2);
	firstProperty->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_pDaSimulationElement->addProperty(firstProperty);
	m_reportSimulationElement = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("Random value report"));
	m_reportSimulationElement->setName(aName);
	m_reportSimulationElement->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_reportSimulationElement->setDatatype(VT_I4);
	m_reportSimulationElement->setIoMode(EnumIoMode_REPORT);
	daRoot->addChild(m_reportSimulationElement);
	DaProperty* secondProperty = new DaProperty();
	secondProperty->setId(6020);
	propName = _T("Angle");
	secondProperty->setName(propName);
	propDescription = _T("Item Description");
	secondProperty->setDescription(propDescription);
	secondProperty->setItemId(propName);
	secondProperty->setDatatype(VT_I2);
	secondProperty->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_reportSimulationElement->addProperty(secondProperty);
	return S_OK;
}   //  end buildAddressSpace


void OpcServer::changeSimulationValues(void)
{
	if (m_pDaSimulationElement != NULL)
	{
		DateTime now;
		Variant aVariant(::rand());
		ValueQT value(aVariant, EnumQuality_GOOD, now);
		m_pDaSimulationElement->valueChanged(value);
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
