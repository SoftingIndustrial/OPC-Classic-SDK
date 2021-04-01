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
	getApp()->setClsidDa(_T("{ED06F266-4FD0-4DA3-BB1A-8F906D2B85B2}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.Sample.AddressSpace.Modified.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.Sample.AddressSpace.Modified.DA"));
	getApp()->setDescription(_T("Softing OPC Toolkit AddressSpace Modified Sample Server"));
	getApp()->setServiceName(tstring());
	getApp()->setIpPortHTTP(8078);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(tstring(_T("Softing Industrial Automation GmbH")));
	getApp()->setMinUpdateRateDa(1);
	getApp()->setClientCheckPeriod(30000);
	g_namespaceDelimiter[0] = _T('.');
	getApp()->setAddressSpaceDelimiter(g_namespaceDelimiter[0]);
	getApp()->setPropertyDelimiter(_T('#'));
	getApp()->setSupportDisableConditions(true);
	getApp()->ShutdownRequest = handleShutdownRequest;
	getApp()->setStringBasedObjectDataExpiry(10000);
	getApp()->setOptimizeForSpeed(TRUE);
	//  proceed with initializing of the Toolkit tracing mechanism
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		_T("Trace.txt"),
		500000000,
		5);
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
	result = getApp()->activate(EnumFeature_DA_SERVER, _T("1100-042c-1d81-1d0f-8487"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Server Feature
	result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("1120-042c-1d81-db2f-c221"));

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

void OpcServer::freeElement(MyDaAddressSpaceElement* element)
{
	std::vector<AddressSpaceElement*> elems = element->getChildren();
	for (std::vector<AddressSpaceElement*>::iterator it = elems.begin(); it != elems.end(); ++it)
	{
		freeElement((MyDaAddressSpaceElement*)*it);
		delete *it;
	}
}

long OpcServer::terminate(void)
{
	MyDaAddressSpaceElement* rootNode = (MyDaAddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	std::vector<AddressSpaceElement*> elems = rootNode->getChildren();
	for (std::vector<AddressSpaceElement*>::iterator it = elems.begin(); it != elems.end(); ++it)
	{
		freeElement((MyDaAddressSpaceElement*)*it);
		delete *it;
	}

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
	SYSTEMTIME sstart, send;

	GetSystemTime(&sstart);
	printf(_T("%04d-%02d-%02d %02d:%02d:%02d.%04d\n"), sstart.wYear, sstart.wMonth, sstart.wDay, sstart.wHour, sstart.wMinute, sstart.wSecond, sstart.wMilliseconds);
	getApp()->setDaServerState(EnumServerState_COMM_FAULT);
	_tprintf(_T("Creating object-based namespace...\n"));
	TCHAR node[] = { 0 };
	const TCHAR *currPos = NULL;
	const TCHAR *lastPos = NULL;
	int position = 0;
	int length = 0;
	TCHAR l_nameBuffer[256] = { 0 };
	MyDaAddressSpaceElement* objectBasedNode = NULL;
	MyDaAddressSpaceElement* objectBasedFolderNode = NULL;
	MyDaAddressSpaceElement* stringBasedNode = NULL;
	MyDaAddressSpaceElement* rootNode = (MyDaAddressSpaceElement*)getApp()->getDaAddressSpaceRoot();
	MyDaAddressSpaceElement* generalParentTag = rootNode;

	if (g_stringBasedNodeName.size() > 0)
	{
		const TCHAR* fullNodeName = g_stringBasedNodeName.c_str();
		lastPos = fullNodeName;
		currPos = _tcsstr(fullNodeName, g_namespaceDelimiter);

		while (currPos != NULL)
		{
			length = currPos - fullNodeName - position;
			_tcscpy_s(l_nameBuffer, fullNodeName + position);
			l_nameBuffer[length] = 0;
			stringBasedNode = new MyDaAddressSpaceElement();
			stringBasedNode->setName(tstring(l_nameBuffer));
			stringBasedNode->setIoMode(EnumIoMode_NONE);
			stringBasedNode->setHasChildren(TRUE);
			generalParentTag->addChild(stringBasedNode);
			generalParentTag = stringBasedNode;
			lastPos = currPos + 1;
			position = currPos - fullNodeName + 1;
			currPos = _tcsstr(fullNodeName + position, g_namespaceDelimiter);
		}

		_tcscpy_s(l_nameBuffer, lastPos);
		stringBasedNode = new MyDaAddressSpaceElement();
		stringBasedNode->setName(tstring(l_nameBuffer));
		stringBasedNode->setIoMode(EnumIoMode_NONE);
		stringBasedNode->setHasChildren(TRUE);
		generalParentTag->addChild(stringBasedNode);
		generalParentTag = stringBasedNode;
	}

	m_stringBasedParentElement = generalParentTag;
	generalParentTag = rootNode;

	if (_tcslen(g_folderFullPath) > 0)
	{
		position = 0;
		lastPos = g_folderFullPath;
		currPos = _tcsstr(g_folderFullPath, g_namespaceDelimiter);

		while (currPos != NULL)
		{
			length = currPos - g_folderFullPath - position;
			_tcscpy_s(l_nameBuffer, g_folderFullPath + position);
			l_nameBuffer[length] = 0;
			objectBasedNode = new MyDaAddressSpaceElement();
			objectBasedNode->setName(tstring(l_nameBuffer));
			objectBasedNode->setIoMode(EnumIoMode_NONE);
			objectBasedNode->setHasChildren(TRUE);
			generalParentTag->addChild(objectBasedNode);
			generalParentTag = objectBasedNode;
			lastPos = currPos + 1;
			position = currPos - g_folderFullPath + 1;
			currPos = _tcsstr(g_folderFullPath + position, g_namespaceDelimiter);
		}

		_tcscpy_s(l_nameBuffer, lastPos);
		objectBasedNode = new MyDaAddressSpaceElement();
		objectBasedNode->setName(tstring(l_nameBuffer));
		objectBasedNode->setIoMode(EnumIoMode_NONE);
		objectBasedNode->setHasChildren(TRUE);
		generalParentTag->addChild(objectBasedNode);
		generalParentTag = objectBasedNode;
	}

	m_objectBasedParentElement = generalParentTag;

	VARENUM itemDataType = VT_EMPTY;
	TCHAR l_folderBuffer[256] = { 0 };
	TCHAR l_itemIdBuffer[256] = { 0 };
	tstring stringParam;

	double totalCount = g_foldersCount * g_itemsTotalCount;
	unsigned int currentCount = 0;
	double percent = 0;

	for (unsigned int j = 0; j < g_foldersCount; ++j)
	{
		_stprintf_s(l_folderBuffer, g_folderNameFormat, g_folderName, j);
		objectBasedFolderNode = new MyDaAddressSpaceElement();
		objectBasedFolderNode->setName(tstring(l_folderBuffer));
		objectBasedFolderNode->setIoMode(EnumIoMode_NONE);
		objectBasedFolderNode->setHasChildren(TRUE);
		m_objectBasedParentElement->addChild(objectBasedFolderNode);

		for (unsigned int i = 0; i < g_itemsTotalCount; ++i)
		{
			currentCount = j * g_itemsTotalCount + i;
			if (currentCount % 1000 == 0)
			{
				percent = 100.0f * currentCount / totalCount;
				_tprintf(_T("\r%3.2f%%\t%10u of %10.0f"), percent, currentCount, totalCount);
			}

			objectBasedNode = new MyDaAddressSpaceElement();
			_stprintf_s(l_nameBuffer, g_itemNameFormat, g_itemName, i);
			stringParam = l_nameBuffer;
			objectBasedNode->setName(stringParam);
			_stprintf_s(l_itemIdBuffer, _T("%s.%s.%s"), g_folderFullPath, l_folderBuffer, l_nameBuffer);
			stringParam = l_itemIdBuffer;
			objectBasedNode->setItemId(stringParam);
			objectBasedNode->setAccessRights(EnumAccessRights_READWRITEABLE);
			objectBasedNode->setRandomDatatype(i);

			switch (i % 4)
			{
			case 0:
				objectBasedNode->setIoMode(EnumIoMode_POLL);
				break;
			case 1:
				objectBasedNode->setIoMode(EnumIoMode_REPORT);
				break;
			case 2:
				objectBasedNode->setIoMode(EnumIoMode_REPORT_CYCLIC);
				break;
			case 3:
				objectBasedNode->setIoMode(EnumIoMode_POLL_OWNCACHE);
				break;
			case 4:
				// not used
				objectBasedNode->setIoMode(EnumIoMode_NONE);
				break;
			}

			objectBasedNode->setHasChildren(FALSE);
			objectBasedFolderNode->addChild(objectBasedNode);
		}
	}
	
	_tprintf(_T("\r 100%%\n"));


	GetSystemTime(&send);
	printf(_T("%04d-%02d-%02d %02d:%02d:%02d.%04d\n"), sstart.wYear, sstart.wMonth, sstart.wDay, sstart.wHour, sstart.wMinute, sstart.wSecond, sstart.wMilliseconds);
	printf(_T("%04d-%02d-%02d %02d:%02d:%02d.%04d\n"), send.wYear, send.wMonth, send.wDay, send.wHour, send.wMinute, send.wSecond, send.wMilliseconds);

	//Sleep(20000);

	//  set the initial cache value. This must only be done after the element is
	//  added to the addresss space via the addChild() method
	_tprintf(_T("\n...object-based namespace created.\n"));
	getApp()->setDaServerState(EnumServerState_RUNNING);
	return EnumResultCode_S_OK;
}   //  end buildAddressSpace

void OpcServer::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	const TCHAR* anObjectID,
	const TCHAR* aMessage,
	...)
{
	getApp()->trace(aLevel, aMask, anObjectID, aMessage);
}   //  end trace

void OpcServer::fireDataChanges(void)
{
	std::vector<AddressSpaceElement*> elements = m_objectBasedParentElement->getChildren();
	std::vector<AddressSpaceElement*>::iterator it;
	int rnd = rand();
	DateTime dateTime;
	dateTime.now();
	for (it = elements.begin(); it != elements.end(); ++it)
	{
		++rnd;
		((MyDaAddressSpaceElement*)*it)->randomValue(dateTime, rnd);
	}
	elements = m_stringBasedParentElement->getChildren();
	rnd = rand();
	for (it = elements.begin(); it != elements.end(); ++it)
	{
		++rnd;
		((MyDaAddressSpaceElement*)*it)->randomValue(dateTime, rnd);
	}
}

void OpcServer::emptyValues(void)
{
	std::vector<AddressSpaceElement*> elements = m_objectBasedParentElement->getChildren();
	std::vector<AddressSpaceElement*>::iterator it;
	DateTime dateTime;
	dateTime.now();
	for (it = elements.begin(); it != elements.end(); ++it)
	{
		((MyDaAddressSpaceElement*)*it)->emptyValue(dateTime);
	}
	elements = m_stringBasedParentElement->getChildren();
	for (it = elements.begin(); it != elements.end(); ++it)
	{
		((MyDaAddressSpaceElement*)*it)->emptyValue(dateTime);
	}
}

