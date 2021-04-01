#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

MySession* MySession::s_controlingSession = NULL;
unsigned long MySession::s_clientCount = 0;

OpcServer* instance = NULL;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

// list of sessions
std::vector<MySession*> g_sessionList;

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
}


OpcServer::~OpcServer(void)
{
}


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{5D108F17-8FEA-40db-AC7F-6F83A88F05BC}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Session.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Sample.Session.DA"));
	getApp()->setDescription(_T("Softing OPC Toolkit C++ Session Sample Server"));
	getApp()->setServiceName(tstring());
	getApp()->setIpPortHTTP(8079);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(tstring(_T("Softing Industrial Automation GmbH")));
	getApp()->setMinUpdateRateDa(100);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('#'));
	getApp()->setSupportDisableConditions(true);
	getApp()->ShutdownRequest = handleShutdownRequest;
	//  proceed with initializing of the Toolkit tracing mechanism
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_SERVER,
		EnumTraceGroup_SERVER,
		_T("Trace.txt"),
		1000000,
		0);
	return S_OK;
}


long OpcServer::start(void)
{
	return getApp()->start();
}   //  end start


long OpcServer::stop(void)
{
	return getApp()->stop();
}   //  end stop


long OpcServer::ready(void)
{
	return getApp()->ready();
}   //  end ready


long OpcServer::terminate(void)
{
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate


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
	//  a valid creator must be provided to the initialize
	result = getApp()->initialize(creator);

	if (!SUCCEEDED(result))
	{
		return result;
	}

	return result;
}   //  end prepare



long OpcServer::processCommandLine(IN tstring commandLine)
{
	return getApp()->processCommandLine(commandLine);
}   //  end processCommandLine


long OpcServer::buildAddressSpace(void)
{
	DateTime time;
	MyDaAddressSpaceElement* pTag = NULL;
	tstring name;
	time.now();
	pTag = new MyDaAddressSpaceElement();
	name = _T("client specific");
	pTag->setName(name);
	pTag->setAccessRights(EnumAccessRights_READABLE);
	pTag->setDatatype(VT_UI4);
	pTag->setIoMode(EnumIoMode_POLL_OWNCACHE);
	getApp()->getDaAddressSpaceRoot()->addChild(pTag);
	time.now();
	pTag = new MyDaAddressSpaceElement();
	name = _T("secured write");
	pTag->setName(name);
	pTag->setAccessRights(EnumAccessRights_READWRITEABLE);
	pTag->setDatatype(VT_UI4);
	pTag->setIoMode(EnumIoMode_POLL);
	getApp()->getDaAddressSpaceRoot()->addChild(pTag);
	Variant aVariant(2);
	ValueQT aValueQT(aVariant, EnumQuality_GOOD, time);
	pTag->valueChanged(aValueQT);
	return S_OK;
}   //  end buildNamesapce


void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName


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


void OpcServer::showObjectTree(void)
{
	_tprintf(_T("CLIENTS:\n"));

	for (size_t i = 0; i < g_sessionList.size(); i++)
	{
		tstring type;

		switch (g_sessionList[i]->getType())
		{
		case EnumSessionType_DA:
			type = _T("DA");
			break;

		case EnumSessionType_XMLDA:
			type = _T("XMLDA");
			break;

		case EnumSessionType_XMLSUBSCRIPTIONS:
			type = _T("INTERNAL XML-DA");
			break;

		default:
			type = _T("unknown");
			break;
		}   // end switch

		MySession* pSession = g_sessionList[i];
		_tprintf(_T("[%d] [%d] type: [%s]"), i, (int)pSession->getHandle(), type.c_str());

		if (pSession == MySession::s_controlingSession)
		{
			_tprintf(_T("[Login Session]\n"));
		}
		else
		{
			_tprintf(_T("[No Login Session]\n"));
		}   //  end if .. else
	}   //  end for

	_tprintf(_T("ITEMS:\n"));
	std::vector<AddressSpaceElement*> elementList = getApp()->getDaAddressSpaceRoot()->getChildren();

	for (size_t j = 0; j < elementList.size(); j++)
	{
		if (elementList[j] != NULL)
		{
			MyDaAddressSpaceElement* pElement = (MyDaAddressSpaceElement*)elementList[j];
			_tprintf(_T("[%d] Name: [%s] Active: "), j, pElement->getName().c_str());

			if (pElement->getActive() == FALSE)
			{
				_tprintf(_T("[No]"));
			}
			else
			{
				_tprintf(_T("[Yes]"));
			} // end if else

			_tprintf(_T(" UpdateRate: [%d]\n"), pElement->getUpdateRate());
		} // end if
	} // end for

	_tprintf(_T("Press Ctrl-C to exit\n"));
}   // end showObjectTree
