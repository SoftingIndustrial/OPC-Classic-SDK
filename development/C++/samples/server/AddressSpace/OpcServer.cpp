#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

OpcServer* instance = NULL;
std::vector<MyDaAddressSpaceElement*> g_reconfigHandles;
MyDaAddressSpaceElement* g_objectBased = NULL;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

long API_CALL handleShutdownRequest(void)
{
#ifdef TBC_OS_WINDOWS
	::SetEvent(g_endEvent);
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
	//  add your dcom settings here
	getApp()->setClsidDa(_T("{EBAE0AB2-99A5-4879-B4F8-9DC2943A4CF3}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Sample.AddressSpace.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Sample.AddressSpace.DA"));
	getApp()->setDescription(_T("Softing OPC Toolkit C++ AddressSpace Sample Server"));
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
	
	// Set string based object data expiry timeout to 5 minutes.
	// The Toolkit CORE will keep all the data queried through DaAddressSpaceRoot::queryAddressSpaceElementDataEx
	// and DaAddressSpaceRoot::queryAddressSpaceElementData in its internal cache for 5 minutes.
	getApp()->setStringBasedObjectDataExpiry(300000);
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
}   //  end initialize

void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName

void OpcServer::setServiceDescription(tstring serviceDescription)
{
	getApp()->setServiceDescription(serviceDescription);
}   //  end setServiceDescription

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
	AddressSpaceElement* root = (AddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	((MyDaAddressSpaceRoot*)root)->lockElementsMutex();

	MyDaAddressSpaceElement* tag = NULL;
	tstring name;
	g_objectBased = new MyDaAddressSpaceElement();
	name = _T("objectBased");
	g_objectBased->setName(name);
	g_objectBased->setIoMode(EnumIoMode_NONE);
	g_objectBased->setHasChildren(TRUE);
	g_objectBased->setType(1);
	getApp()->getDaAddressSpaceRoot()->addChild(g_objectBased);
	tag = new MyDaAddressSpaceElement();
	name = _T("T1");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I1);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(2);
	g_objectBased->addChild(tag);
	//  set the inital cahce value. This must only be done after the element is
	//  added to the addresss space via the addChild() method
	tag = new MyDaAddressSpaceElement();
	name = _T("T2");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I1);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(2);
	g_objectBased->addChild(tag);
	tag = new MyDaAddressSpaceElement();
	name = _T("T3");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I1);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(2);
	g_objectBased->addChild(tag);

	for (size_t i = 0; i < 3; i++)
	{
		g_reconfigHandles.push_back(new MyDaAddressSpaceElement());
	}

	tstringstream str;
	str << rand();
	name = _T("R") + str.str();
	g_reconfigHandles[0]->setName(name);
	g_reconfigHandles[0]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[0]->setDatatype(VT_I4);
	g_reconfigHandles[0]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[0]->setHasChildren(TRUE);
	g_reconfigHandles[0]->setType(3);
	g_objectBased->addChild(g_reconfigHandles[0]);
	str.clear();
	str << rand();
	name = _T("R") + str.str();
	g_reconfigHandles[1]->setName(name);
	g_reconfigHandles[1]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[1]->setDatatype(VT_I4);
	g_reconfigHandles[1]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[1]->setType(4);
	g_objectBased->addChild(g_reconfigHandles[1]);
	str.clear();
	str << rand();
	name = _T("R") + str.str();
	g_reconfigHandles[2]->setName(name);
	g_reconfigHandles[2]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[2]->setDatatype(VT_I4);
	g_reconfigHandles[2]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[2]->setType(5);
	g_reconfigHandles[0]->addChild(g_reconfigHandles[2]);
	str.clear();

	((MyDaAddressSpaceRoot*)root)->unlockElementsMutex();
	return S_OK;
}   //  end buildAddressSpace

void OpcServer::rebuildRandomNamespace()
{
	AddressSpaceElement* root = (AddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	((MyDaAddressSpaceRoot*)root)->lockElementsMutex();

	if (g_objectBased == NULL)
	{
		return;
	}

	if (g_objectBased->getName() != _T("objectBased"))
	{
		return;
	}

	tstringstream str;
	str << rand();
	g_reconfigHandles[0] = new MyDaAddressSpaceElement();
	tstring elemName = _T("R") + str.str();
	g_reconfigHandles[0]->setName(elemName);
	g_reconfigHandles[0]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[0]->setDatatype(VT_I4);
	g_reconfigHandles[0]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[0]->setHasChildren(TRUE);
	g_reconfigHandles[0]->setType(3);
	g_objectBased->addChild(g_reconfigHandles[0]);
	str.clear();
	str << rand();
	g_reconfigHandles[1] = new MyDaAddressSpaceElement();
	elemName = _T("R") + str.str();
	g_reconfigHandles[1]->setName(elemName);
	g_reconfigHandles[1]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[1]->setDatatype(VT_I4);
	g_reconfigHandles[1]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[1]->setType(4);
	g_objectBased->addChild(g_reconfigHandles[1]);
	str.clear();
	str << rand();
	g_reconfigHandles[2] = new MyDaAddressSpaceElement();
	elemName = _T("R") + str.str();
	g_reconfigHandles[2]->setName(elemName);
	g_reconfigHandles[2]->setAccessRights(EnumAccessRights_READWRITEABLE);
	g_reconfigHandles[2]->setDatatype(VT_I4);
	g_reconfigHandles[2]->setIoMode(EnumIoMode_REPORT);
	g_reconfigHandles[2]->setType(5);
	g_reconfigHandles[0]->addChild(g_reconfigHandles[2]);
	str.clear();

	((MyDaAddressSpaceRoot*)root)->unlockElementsMutex();
} // end rebuildRandomNamespace


void OpcServer::printChildren(AddressSpaceElement* element, unsigned char bLevel)
{
	std::vector<AddressSpaceElement*> children = element->getChildren();

	for (size_t i = 0; i < children.size(); i++)
	{
		AddressSpaceElement* child = children[i];

		if (child != NULL)
		{
			tstring line(_T(""));

			for (unsigned char i = 0; i < bLevel; i++)
			{
				line += _T("  ");
			}   //  end for

			g_syncDisplay.lock();

			_tprintf(_T("%s [%s]\n"), line.c_str(), child->getName().c_str());

			g_syncDisplay.unlock();

			if (child->getHasChildren() == TRUE)
			{
				bLevel++;
				printChildren(child, bLevel);
				bLevel--;
			}   // end if
		} // end if
	} // end for
}   //  end printChildren

void OpcServer::showObjectTree(void)
{
	_tprintf(_T("CURRENT ADDRESS SPACE:\n"));
	AddressSpaceElement* root = (AddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	unsigned char level = 0;
	((MyDaAddressSpaceRoot*)root)->lockElementsMutex();
	printChildren(root, level);
	((MyDaAddressSpaceRoot*)root)->unlockElementsMutex();
	_tprintf(_T("Press Ctrl-C to exit\n"));
} // end showObjectTree

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

void OpcServer::updateAddressSpaceElements()
{
	AddressSpaceElement* root = (AddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	((MyDaAddressSpaceRoot*)root)->lockElementsMutex();
	g_reconfigHandles[0]->removeChild(g_reconfigHandles[2]);
	g_reconfigHandles[2] = NULL;
	g_objectBased->removeChild(g_reconfigHandles[0]);
	g_reconfigHandles[0] = NULL;
	g_objectBased->removeChild(g_reconfigHandles[1]);
	g_reconfigHandles[1] = NULL;
	((MyDaAddressSpaceRoot*)root)->unlockElementsMutex();
}
