#include "stdafx.h"
#include "OpcClient.h"
#include "ClientCommon.h"

using namespace std;
using namespace SoftingOPCToolboxClient;

OpcClient* instance = NULL;

OpcClient* getOpcClient(void)
{
	return instance;
}   //  end getOpcClient

void createOpcClient(void)
{
	if (instance == NULL)
	{
		instance = new OpcClient();
	}
}   //  end createOpcClient

void destroyOpcClient(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}   //  end destroyOpcClient

OpcClient::OpcClient()
{
	m_daSession = NULL;
	m_daSubscription = NULL;
	m_daItem = NULL;
}   //  end constructor


Application* OpcClient::getApp()
{
	return getApplication();
}   //  end getApp

long OpcClient::initialize()
{
	getApp()->setVersionOtb(447);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);
	long result = getApp()->initialize();

	return result;
}   //  end initialize

long OpcClient::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine

void OpcClient::terminate()
{
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

	if (m_daItem != NULL)
	{
		delete m_daItem;
		m_daItem = NULL;
	}

	releaseApplication();
}   //  end terminate

void OpcClient::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName

void OpcClient::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	TCHAR* anObjectID,
	TCHAR* aMessage,
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



long OpcClient::initializeDaObjects()
{
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	tstring url(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
	m_daSession = new MyDaSession(url);
	m_daSubscription = new MyDaSubscription(1000, m_daSession);
	m_daItem = new MyDaItem(_T("maths.sin"), m_daSubscription);
	long connectResult = m_daSession->connect(TRUE, TRUE, &executionOptions);
	return connectResult;
}   //  end initializeDaObjects

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
	itemsToRead[0] = m_daItem;

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
			}   //  end if
		} //end for
	} //end if
	else
	{
		message.append(_T("Read failed"));
	} //end if...else

	return message;
}   //  end readItem

