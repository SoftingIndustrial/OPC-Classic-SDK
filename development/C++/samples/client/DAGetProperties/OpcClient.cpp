#include "stdafx.h"
#include "OpcClient.h"
#include "ClientCommon.h"

using namespace std;
using namespace SoftingOPCToolboxClient;

OpcClient* instance = NULL;

OpcClient* getOpcClient(void)
{
	return instance;
} // end getOpcClient

void createOpcClient(void)
{
	if (instance == NULL)
	{
		instance = new OpcClient();
	}
} // end createOpcClient

void destroyOpcClient(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
} // end destroyOpcClient

OpcClient::OpcClient()
{
	session = NULL;
} // end constructor


Application* OpcClient::getApp()
{
	return getApplication();
} // end getApp

long OpcClient::initialize()
{
	long result = EnumResultCode_S_OK;
	getApp()->setVersionOtb(447);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		0);

	// TODO - binary license activation
	// Fill in your binary license activation keys here
	//
	// NOTE: you can activate one or all of the features at the same time

	//  activate the COM-DA Client feature
	// result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
	if (!SUCCEEDED(result))
	{
		return result;
	}

	// activate the XML-DA Client Feature
	// result = getApplication()->activate(EnumFeature_XMLDA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
	if (!SUCCEEDED(result))
	{
		return result;
	}

	// END TODO - binary license activation
	result = getApp()->initialize();

	return result;
} // end initialize

long OpcClient::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
} // end processCommandLine

void OpcClient::terminate()
{
	if (session != NULL)
	{
		if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			session->disconnect(NULL);
		}

		getApp()->removeDaSession(session);

		delete session;
		session = NULL;
	}

	getApp()->terminate();
	releaseApplication();
} // end terminate

void OpcClient::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
} // end setServiceName

void OpcClient::trace(
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
} // end trace


long OpcClient::initializeDaObjects()
{
#ifdef TBC_OS_WINDOWS
	addressSpaceElementId = _T("increment.I1");
#endif
#ifdef TBC_OS_LINUX
	//  TODO change the items accordingly to the server you use
	//  this values are used is the DemoServer server sample
	addressSpaceElementId = _T("all data types.dynamic.R4");
#endif
	// TODO set the path with the valid one if you use XML-DA
	addressSpaceElementPath = _T("");
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	executionContext = executionOptions.getExecutionContext();
	session = NULL;
#ifdef TBC_OS_WINDOWS
	//  set the COM-DA url here
	session = new MyDaSession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
	//  set the XML-DA url here
	//session = new MyDaSession(_T("http://localhost:8079/OPC/DA"));
#endif
#ifdef TBC_OS_LINUX
	//  set the COM-DA url here; Note the url must be changed to work properly
	session = new MyDaSession("http://localhost:8079/OPC/DA");
#endif

	if (!session->getValid())
	{
		return 0;
	}

	long connectResult = session->connect(TRUE, FALSE, NULL);
	return connectResult;
} // end initializeDaObjects


void OpcClient::getAsyncAddressSpaceElementProps()
{
	unsigned int result;
	std::vector<DaProperty*> allProperties;
	std::vector<DaProperty*> someProperties;
	std::vector<unsigned long> propertyIds;
	std::vector<tstring> propertyNames;
	executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);
	executionOptions.setExecutionContext(++executionContext);
	_tprintf(_T("Get all properties of address space element:  %s\n"), addressSpaceElementId.c_str());
	DaGetPropertiesOptions propertiesOptions;
	propertiesOptions.setWhatPropertyData(EnumPropertyData_ALL);
	result = session->getDaProperties(
				 addressSpaceElementId,
				 addressSpaceElementPath,
				 &propertiesOptions,
				 allProperties,
				 &executionOptions);
	_tprintf(_T("Get properties 1 and 5 of address space element:  %s\n"), addressSpaceElementId.c_str());
#ifdef TBC_OS_WINDOWS
	//  set the property IDs
	propertyIds.push_back(1);
	propertyIds.push_back(5);
	//  TODO uncomment this if you use XML-DA
	//  set the property names (it's a must for XML-DA)
	//  propertyNames.push_back(_T("dataType"));
	//  propertyNames.push_back(_T("accessRights"));
#endif
#ifdef TBC_OS_LINUX
	propertyNames.push_back(_T("dataType"));
	propertyNames.push_back(_T("accessRights"));
#endif
	propertiesOptions.setPropertyIds(propertyIds);
	propertiesOptions.setPropertyNames(propertyNames);
	result = session->getDaProperties(
				 addressSpaceElementId,
				 addressSpaceElementPath,
				 &propertiesOptions,
				 someProperties,
				 &executionOptions);
}// end getAsyncAddressSpaceElementProps

void OpcClient::getSyncAddressSpaceElementProps()
{
	unsigned int result;
	std::vector<DaProperty*> allProperties;
	std::vector<DaProperty*> someProperties;
	std::vector<unsigned long> propertyIds;
	std::vector<tstring> propertyNames;
	std::vector<DaProperty*>::iterator propertiesIterator;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	_tprintf(_T("Get all properties of address space element:  %s\n"), addressSpaceElementId.c_str());
	DaGetPropertiesOptions propertiesOptions;
	propertiesOptions.setWhatPropertyData(EnumPropertyData_ALL);
	result = session->getDaProperties(
				 addressSpaceElementId,
				 addressSpaceElementPath,
				 &propertiesOptions,
				 allProperties,
				 &executionOptions);

	if (SUCCEEDED(result))
	{
		size_t count = allProperties.size();

		for (unsigned long i = 0; i < count; i++)
		{
			_tprintf(_T("%s%-30.30s\n"), _T("Property Name: "), allProperties[i]->getName().c_str());
			_tprintf(_T("%s%5.5d\n"), _T("Property Id: "), (int)allProperties[i]->getId());
			_tprintf(_T("%s%-30.30s\n"), _T("Property Item Id: "), allProperties[i]->getItemId().c_str());
			_tprintf(_T("%s%5.5d\n"), _T("Property DataType: "), allProperties[i]->getDataType());
			_tprintf(_T("%s%-30.30s\n"), _T("Property Description: "), allProperties[i]->getDescription().c_str());
			_tprintf(_T("%s%s\n"), _T("Property Value: "), allProperties[i]->getValueQT().toString().c_str());
			_tprintf(_T("\n"));
		} //end for
	} //end if
	else
	{
		_tprintf(_T(" Failed to synchronously get properties of address space element: %s - 0x%08X\n"), addressSpaceElementId.c_str(), result);
	}

	_tprintf(_T("Get properties 1 and 5 of address space element:  %s\n"), addressSpaceElementId.c_str());
#ifdef TBC_OS_WINDOWS
	propertyIds.push_back(1);
	propertyIds.push_back(5);
	//  TODO uncomment this if you use XML-DA
	//  set the property names (it's a must for XML-DA)
	//propertyNames.push_back(_T("dataType"));
	//propertyNames.push_back(_T("accessRights"));
#endif
#ifdef TBC_OS_LINUX
	propertyNames.push_back(_T("dataType"));
	propertyNames.push_back(_T("accessRights"));
#endif
	propertiesOptions.setPropertyIds(propertyIds);
	propertiesOptions.setPropertyNames(propertyNames);
	result = session->getDaProperties(
				 addressSpaceElementId,
				 addressSpaceElementPath,
				 &propertiesOptions,
				 someProperties,
				 &executionOptions);

	if (SUCCEEDED(result))
	{
		size_t count = someProperties.size();

		for (unsigned long i = 0; i < count; i++)
		{
			_tprintf(_T("%s%-30.30s\n"), _T("Property Name: "), someProperties[i]->getName().c_str());
			_tprintf(_T("%s%5.5d\n"), _T("Property Id: "), (int)someProperties[i]->getId());
			_tprintf(_T("%s%-30.30s\n"), _T("Property Item Id: "), someProperties[i]->getItemId().c_str());
			_tprintf(_T("%s%5.5d\n"), _T("Property DataType: "), someProperties[i]->getDataType());
			_tprintf(_T("%s%-30.30s\n"), _T("Property Description: "), someProperties[i]->getDescription().c_str());
			_tprintf(_T("%s%s\n"), _T("Property Value: "), someProperties[i]->getValueQT().toString().c_str());
			_tprintf(_T("\n"));
		} //end for
	} //end if
	else
	{
		_tprintf(_T(" Failed to synchronously get properties 1 and 5 of address space element: %s - 0x%08X\n"), addressSpaceElementId.c_str(), result);
	}

	for (propertiesIterator = allProperties.begin(); propertiesIterator != allProperties.end(); propertiesIterator++)
	{
		delete(*propertiesIterator);
	} //end for

	for (propertiesIterator = someProperties.begin(); propertiesIterator != someProperties.end(); propertiesIterator++)
	{
		delete(*propertiesIterator);
	} //end for
}// end getSyncAddressSpaceElementProps

