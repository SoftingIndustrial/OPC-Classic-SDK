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

const unsigned char MyDaAddressSpaceElement::TYPE_NODE_NO_IO    = 0x00;
const unsigned char MyDaAddressSpaceElement::TYPE_REPORT_SEC    = 0x01;
const unsigned char MyDaAddressSpaceElement::TYPE_REPORT_MIN    = 0x02;
const unsigned char MyDaAddressSpaceElement::TYPE_POLL_SEC      = 0x04;
const unsigned char MyDaAddressSpaceElement::TYPE_POLL_MIN      = 0x08;

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
#ifdef TBC_OS_WINDOWS
	SetEvent(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
	g_events.signal(0);
#endif
	return S_OK;
}   //end HandleShutdownRequest


OpcServer::OpcServer(void)
{
	m_reportTagSec = NULL;
	m_reportTagMin = NULL;
}   //  end constructor


OpcServer::~OpcServer(void)
{
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{3CA5D6A3-D168-480B-BDDD-BEEB6BF82556}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Sample.UpdateMode.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Sample.UpdateMode.DA"));
	getApp()->setDescription(_T("Softing OPC Toolkit C++ UpdateMode Sample Server"));
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

	//  END TODO - binary license activation
	result = getApp()->initialize(creator);

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


long OpcServer::buildAddressSpace(void)
{
	MyDaAddressSpaceElement* tag, *clockNode, *reportNode, *pollNode;
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	tstring name;
	clockNode = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("clock"));
	clockNode->setName(name);
	clockNode->setType(MyDaAddressSpaceElement::TYPE_NODE_NO_IO);
	clockNode->setIoMode(EnumIoMode_NONE);
	clockNode->setHasChildren(TRUE);
	daRoot->addChild(clockNode);
	reportNode = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("report"));
	reportNode->setName(name);
	reportNode->setType(MyDaAddressSpaceElement::TYPE_NODE_NO_IO);
	reportNode->setIoMode(EnumIoMode_NONE);
	reportNode->setHasChildren(TRUE);
	clockNode->addChild(reportNode);
	m_reportTagSec = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("second"));
	m_reportTagSec->setName(name);
	m_reportTagSec->setType(MyDaAddressSpaceElement::TYPE_REPORT_SEC);
	m_reportTagSec->setAccessRights(EnumAccessRights_READABLE);
	m_reportTagSec->setDatatype(VT_I4);
	m_reportTagSec->setIoMode(EnumIoMode_REPORT_CYCLIC);
	reportNode->addChild(m_reportTagSec);
	m_reportTagMin = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("minute"));
	m_reportTagMin->setName(name);
	m_reportTagMin->setType(MyDaAddressSpaceElement::TYPE_REPORT_MIN);
	m_reportTagMin->setAccessRights(EnumAccessRights_READABLE);
	m_reportTagMin->setDatatype(VT_I4);
	m_reportTagMin->setIoMode(EnumIoMode_REPORT_CYCLIC);
	reportNode->addChild(m_reportTagMin);
	pollNode = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("poll"));
	pollNode->setName(name);
	pollNode->setType(MyDaAddressSpaceElement::TYPE_NODE_NO_IO);
	pollNode->setIoMode(EnumIoMode_NONE);
	pollNode->setHasChildren(TRUE);
	clockNode->addChild(pollNode);
	tag = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("second"));
	tag->setName(name);
	tag->setType(MyDaAddressSpaceElement::TYPE_POLL_SEC);
	tag->setAccessRights(EnumAccessRights_READABLE);
	tag->setDatatype(VT_I4);
	tag->setIoMode(EnumIoMode_POLL);
	pollNode->addChild(tag);
	tag = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	name = tstring(_T("minute"));
	tag->setName(name);
	tag->setType(MyDaAddressSpaceElement::TYPE_POLL_MIN);
	tag->setAccessRights(EnumAccessRights_READABLE);
	tag->setDatatype(VT_I4);
	tag->setIoMode(EnumIoMode_POLL);
	pollNode->addChild(tag);
	return S_OK;
}   //  end buildAddressSpace


void OpcServer::changeValue(Variant& aVar, DAVariableType varType)
{
	DateTime now;
	now.now();
	ValueQT val(aVar, EnumQuality_GOOD, now);

	switch (varType)
	{
	case DAVariableType_Sec:
	{
		if (m_reportTagSec)
		{
			m_reportTagSec->valueChanged(val);
		}
	}
	break;

	case DAVariableType_Min:
	{
		if (m_reportTagMin)
		{
			m_reportTagMin->valueChanged(val);
		}
	}
	break;

	default:
	{
		//nothing to do
	}
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
