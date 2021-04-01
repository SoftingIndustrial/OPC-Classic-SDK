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

	// activate the COM-AE Client Feature
	// result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
	if (!SUCCEEDED(result))
	{
		return FALSE;
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
	if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		session->disconnect(NULL);
	}

	getApp()->removeAeSession(session);
	getApp()->terminate();

	if (session != NULL)
	{
		delete session;
		session = NULL;
	}

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


long OpcClient::initializeAeObjects()
{
	sourcePath = _T("computer.clock.time slot 1");
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	executionContext = executionOptions.getExecutionContext();
	session = new MyAeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
	long connectResult = session->connect(TRUE, FALSE, NULL);
	return connectResult;
} // end initializeAeObjects


void OpcClient::getAsyncSourceConditions()
{
	unsigned int result;
	executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);
	executionOptions.setExecutionContext(++executionContext);
	_tprintf(_T("Conditions of the source  %s are:\n"), sourcePath.c_str());
	result = session->queryAeSourceConditions(
				 sourcePath,
				 conditionNames,
				 &executionOptions);
}// end getAsyncSourceConditions

void OpcClient::getSyncSourceConditions()
{
	unsigned int result;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	_tprintf(_T("Conditions of the source  %s are:\n"), sourcePath.c_str());
	result = session->queryAeSourceConditions(
				 sourcePath,
				 conditionNames,
				 &executionOptions);

	if (SUCCEEDED(result))
	{
		for (unsigned int i = 0; i < conditionNames.size(); i++)
		{
			_tprintf(_T("	[%i]%s"), i, conditionNames[i].c_str());
			_tprintf(_T("\n"));
		} //end for
	} //end if
	else
	{
		_tprintf(_T(" Failed to synchronously query the conditions of source: %s"), sourcePath.c_str());
	}
}// end getSyncSourceConditions
