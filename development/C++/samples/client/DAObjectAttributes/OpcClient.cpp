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
	subscription = NULL;
	item1 = NULL;
	item2 = NULL;
	item3 = NULL;
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
	if (session != NULL)
	{
		if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			session->disconnect(NULL);
		}

		if (subscription != NULL)
		{
			if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
			{
				subscription->disconnect(NULL);
			}

			subscription->removeDaItem(item1);
			subscription->removeDaItem(item2);
			subscription->removeDaItem(item3);
			session->removeDaSubscription(subscription);

			delete subscription;
			subscription = NULL;
		}

		getApp()->removeDaSession(session);

		delete session;
		session = NULL;

		if (item1 != NULL)
		{
			delete item1;
			item1 = NULL;
		}

		if (item2 != NULL)
		{
			delete item2;
			item2 = NULL;
		}

		if (item3 != NULL)
		{
			delete item3;
			item3 = NULL;
		}
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
	sessionResults.assign(2, 0);
	subscriptionResults.assign(4, 0);
	itemResults.assign(1, 0);
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	getApp()->setUpdateAttributesBehavior(EnumUpdateAttributesBehavior_EXPLICIT_UPDATE);
#ifdef TBC_OS_WINDOWS
	session = new MyDaSession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
#endif
#ifdef TBC_OS_LINUX
	session = new MyDaSession(_T("http://localhost:8079/OPC/DA"));
#endif
	subscription = new MyDaSubscription(1000, session);
#ifdef TBC_OS_WINDOWS
	item1 = new MyDaItem(_T("maths.sin"), subscription);
	item2 = new MyDaItem(_T("time.local.second"), subscription);
	item3 = new MyDaItem(_T("increment.UI1"), subscription);
#endif
#ifdef TBC_OS_LINUX
	//  TODO change the items accordingly to the server you use
	//  this values are used is the DemoServer server sample
	item1 = new MyDaItem(_T("all data types.dynamic.R4"), subscription);
	item2 = new MyDaItem(_T("all data types.dynamic.UI1"), subscription);
	item3 = new MyDaItem(_T("all data types.dynamic.UI2"), subscription);
#endif
	long connectResult = session->connect(TRUE, FALSE, NULL);
	return connectResult;
} // end initializeDaObjects

void OpcClient::printObjectAttributes()
{
	_tprintf(_T("--- Initial object attributes ---\n"));
	session->print();
	subscription->print();
	item1->print();
	item2->print();
	item3->print();
}// end printObjectAttributes

void OpcClient::changeSessionAttributes()
{
	long result;
	// change the servers LCID and client name
	_tprintf(_T("\n--- Change the session's localeId and client name ---\n"));
	session->setLocaleId(_T("de-de"));
	session->setClientName(_T("ObjectAttributes Sample"));
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_LCID);
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_CLIENTNAME);
	// write changed data to the server
	result = session->setAttributesToServer
			 (whatAttributes,
			  sessionResults,
			  NULL);
	_tprintf(_T("\nUpdate session's attributes to server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), (unsigned int)result, (unsigned int)sessionResults[0], (unsigned int)sessionResults[1]);
	// get it from the server to check
	result = session->getAttributesFromServer
			 (whatAttributes,
			  sessionResults,
			  NULL);
	_tprintf(_T("\nUpdate session's attributes from server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), (unsigned int)result, (unsigned int)sessionResults[0], (unsigned int)sessionResults[1]);
	session->print();
	whatAttributes.clear();
}// end changeSessionAttributes

void OpcClient::changeSubscriptionAttributes()
{
	long result;
	// change the subscriptions name, update rate, deadband and keep alive time
	_tprintf(_T("\n--- Change the subscription's name, update rate, deadband and keep alive time ---\n"));
	subscription->setName(_T("ObjectAttributes Subscription"));
	subscription->setRequestedUpdateRate(1000);
	subscription->setRequestedKeepAliveTime(4000);
	subscription->setDeadband(10.0);
	whatAttributes.push_back(EnumObjectAttribute_DASUBSCRIPTION_NAME);
	whatAttributes.push_back(EnumObjectAttribute_DASUBSCRIPTION_UPDATERATE);
	whatAttributes.push_back(EnumObjectAttribute_DASUBSCRIPTION_KEEPALIVETIME);
	whatAttributes.push_back(EnumObjectAttribute_DASUBSCRIPTION_DEADBAND);
	// write changed data to the server
	result = subscription->setAttributesToServer
			 (whatAttributes,
			  subscriptionResults,
			  NULL);
	_tprintf(_T("\nUpdate subscription's attributes to server - 0x%X\n  result name: 0x%X\n  result update rate: 0x%X\n  result keep alive time: 0x%X\n  result deadband: 0x%X\n"),
			 (unsigned int)result,
			 (unsigned int)subscriptionResults[0],
			 (unsigned int)subscriptionResults[1],
			 (unsigned int)subscriptionResults[2],
			 (unsigned int)subscriptionResults[3]);
	// get it from the server to check
	result = subscription->getAttributesFromServer
			 (whatAttributes,
			  subscriptionResults,
			  NULL);
	_tprintf(_T("\nUpdate subscription's attributes from server - 0x%X\n  result name: 0x%X\n  result update rate: 0x%X\n  result keep alive time: 0x%X\n  result deadband: 0x%X\n"),
			 (unsigned int)result,
			 (unsigned int)subscriptionResults[0],
			 (unsigned int)subscriptionResults[1],
			 (unsigned int)subscriptionResults[2],
			 (unsigned int)subscriptionResults[3]);
	subscription->print();
	whatAttributes.clear();
}// end changeSubscriptionAttributes

void OpcClient::changeItemAttributes()
{
	long result;
	// change the requested datatype of the items
	_tprintf(_T("\n--- Change requested datatype of the items ---\n"));
	item1->setRequestedDataType(VT_BSTR);
	item2->setRequestedDataType(VT_BSTR);
	item3->setRequestedDataType(VT_BSTR);
	whatAttributes.push_back(EnumObjectAttribute_DAITEM_REQUESTED_DATATYPE);
	items.push_back(item1);
	items.push_back(item2);
	items.push_back(item3);
	// write requested datatypes to the server
	result = subscription->setDaItemAttributesToServer(
				 items,
				 whatAttributes,
				 itemResults,
				 NULL);
	_tprintf(_T("\nUpdate item's attributes to server - 0x%X\n  result datatype: 0x%X\n"), (unsigned int)result, (unsigned int)itemResults[0]);
	// get it from the server to check
	result = subscription->getDaItemAttributesFromServer(
				 items,
				 whatAttributes,
				 itemResults,
				 NULL);
	_tprintf(_T("\nUpdate item's attributes from server - 0x%X\n  result datatype: 0x%X\n"), (unsigned int)result, (unsigned int)itemResults[0]);
	item1->print();
	item2->print();
	item3->print();
	whatAttributes.clear();
}//end changeItemAttributes
