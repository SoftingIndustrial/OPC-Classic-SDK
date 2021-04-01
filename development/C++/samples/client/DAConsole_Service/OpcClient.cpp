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
	m_daSession = NULL;
	m_daSubscription = NULL;
	m_daItem1 = NULL;
	m_daItem2 = NULL;
	m_daItem3 = NULL;
	m_daItem4 = NULL;
	m_areDaObjectsInitialized = 0;
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
	if (m_areDaObjectsInitialized)
	{
		if (m_daSession->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			m_daSession->disconnect(NULL);
		}

		if (m_daSubscription->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			m_daSubscription->disconnect(NULL);
		}

		m_daSubscription->removeDaItem(m_daItem1);
		m_daSubscription->removeDaItem(m_daItem2);
		m_daSubscription->removeDaItem(m_daItem3);
		m_daSubscription->removeDaItem(m_daItem4);
		m_daSession->removeDaSubscription(m_daSubscription);
		getApp()->removeDaSession(m_daSession);
	}   //  end if

	getApp()->terminate();

	if (m_daSession != NULL)
	{
		delete m_daSession;
		m_daSession = NULL;
	}

	if (m_daSubscription != NULL)
	{
		delete m_daSubscription;
		m_daSubscription = NULL;
	}

	if (m_daItem1 != NULL)
	{
		delete m_daItem1;
		m_daItem1 = NULL;
	}

	if (m_daItem2 != NULL)
	{
		delete m_daItem2;
		m_daItem2 = NULL;
	}

	if (m_daItem3 != NULL)
	{
		delete m_daItem3;
		m_daItem3 = NULL;
	}

	if (m_daItem4 != NULL)
	{
		delete m_daItem4;
		m_daItem4 = NULL;
	}

	releaseApplication();
} // end terminate

void OpcClient::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
} // end setServiceName

void OpcClient::setServiceDescription(tstring serviceDescription)
{
	getApp()->setServiceDescription(serviceDescription);
} // end setServiceDescription

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
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	// TODO add your server URL here
	// this is the server url for Softing OPCToolbox Demo Server
	// COM-DA url
	tstring url(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
	m_daSession = new MyDaSession(url);
	m_daSubscription = new MyDaSubscription(1000, m_daSession);
	m_daItem1 = new MyDaItem(_T("maths.sin"), m_daSubscription);
	m_daItem2 = new MyDaItem(_T("time.local.second"), m_daSubscription);
	m_daItem3 = new MyDaItem(_T("increment.UI1"), m_daSubscription);
	m_daItem3 = new MyDaItem(_T("increment.BSTR"), m_daSubscription);
	long connectResult = m_daSession->connect(TRUE, TRUE, &executionOptions);
	return connectResult;
} // end initializeDaObjects

tstring OpcClient::readItem()
{
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	tstring message;
	unsigned long readCount = 1;
	long result;
	std::vector<long> readResults(readCount, E_FAIL);
	std::vector<DaItem*>itemsToRead(readCount, NULL);
	std::vector<ValueQT*> valuesToRead(readCount, NULL);
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	itemsToRead[0] = m_daItem1;

	if (SUCCEEDED(result = m_daSubscription->read(
							   0,
							   itemsToRead,
							   valuesToRead,
							   readResults,
							   &executionOptions)))
	{
		message.append(_T(" \nRead item synchronously using subscription \n"));

		for (unsigned int i = 0; i < readCount; i++)
		{
			if (SUCCEEDED(readResults[i]))
			{
				message.append(itemsToRead[i]->getId().c_str());
				message.append(_T(" - "));
				message.append(valuesToRead[i]->toString().c_str());
				message.append(_T("\n"));
			} //end if
			else
			{
				message.append(_T("Read failed"));
			} //end if...else
		} //end for

		for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
		{
			if (*valuesToReadIterator != NULL)
			{
				delete *valuesToReadIterator;
			} // end if
		} //end for
	} //end if
	else
	{
		message.append(_T("Read failed"));
	} //end if...else

	return message;
} // end readItem

