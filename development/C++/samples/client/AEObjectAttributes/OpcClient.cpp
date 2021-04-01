#include "stdafx.h"
#include "OpcClient.h"

#include "ClientCommon.h"

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
	subscription = NULL;
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

	if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		subscription->disconnect(NULL);
	}

	session->removeAeSubscription(subscription);
	getApp()->removeAeSession(session);
	getApp()->terminate();

	if (session != NULL)
	{
		delete session;
		session = NULL;
	}

	if (subscription != NULL)
	{
		delete subscription;
		subscription = NULL;
	}

	if (aeReturnedAttributes[0] != NULL)
	{
		delete aeReturnedAttributes[0];
		aeReturnedAttributes[0] = NULL;
	}

	if (aeReturnedAttributes[1] != NULL)
	{
		delete aeReturnedAttributes[1];
		aeReturnedAttributes[1] = NULL;
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
	count = 3;
	size = 2;
	size1 = 1;
	sessionResults.assign(2, 0);
	subscriptionResults.assign(4, 0);
	filterCategories.assign(count, 0);
	filterAreas.assign(size, _T(""));
	filterSources.assign(size, _T(""));
	aeReturnedAttributes.assign(size, NULL);
	attributesIds.assign(size, 0);
	_attributesIds.assign(size1, 0);
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	getApp()->setUpdateAttributesBehavior(EnumUpdateAttributesBehavior_EXPLICIT_UPDATE);
	session = new MyAeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
	subscription = new MyAeSubscription(session);
	long connectResult = session->connect(TRUE, FALSE, NULL);
	return connectResult;
} // end initializeAeObjects

void OpcClient::printInitialObjectAttributes()
{
	_tprintf(_T("--- Initial object attributes ---\n"));
	session->print();
	subscription->print();
}// end printInitialObjectAttributes

void OpcClient::setSessionAttributes()
{
	// change the session LCID and client name
	_tprintf(_T("\n--- Change the session's localeId and client name ---\n"));
	session->setLocaleId(_T("de-de"));
	session->setClientName(_T("ObjectAttributes Sample"));
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_LCID);
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_CLIENTNAME);
	// write changed data to the server
	result = session->setAttributesToServer(whatAttributes,
											sessionResults,
											NULL);
	_tprintf(_T("\nUpdate session's attributes to server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), result, sessionResults[0], sessionResults[1]);
	// get it from the server to check
	result = session->getAttributesFromServer(whatAttributes,
											  sessionResults,
											  NULL);
	_tprintf(_T("\nUpdate session's attributes from server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), result, sessionResults[0], sessionResults[1]);
	session->print();
	whatAttributes.clear();
}// end setSessionAttributes

void OpcClient::setSubscriptionAttributes()
{
	// change the subscriptions max size, buffer time and filter
	_tprintf(_T("\n--- Change the subscription's max size, buffer time and filter ---\n"));
	subscription->setRequestedBufferTime(2000);
	subscription->setRequestedMaxSize(5);
	subscription->setFilterSeverityHigh(700);
	subscription->setFilterSeverityLow(100);
	subscription->setFilterEventTypes(EnumEventType_SIMPLE | EnumEventType_TRACKING);
	filterCategories[0] = 1;
	filterCategories[1] = 2;
	filterCategories[2] = 3;
	subscription->setFilterCategories(filterCategories);
	filterAreas[0] = (_T("computer.mouse"));
	filterAreas[1] = (_T("computer.clock"));
	subscription->setFilterAreas(filterAreas);
	filterSources[0] = _T("computer.clock.time slot 1");
	filterSources[1] = _T("computer.clock.time slot 2");
	subscription->setFilterSources(filterSources);
	whatAttributes.push_back(EnumObjectAttribute_AESUBSCRIPTION_BUFFERTIME);
	whatAttributes.push_back(EnumObjectAttribute_AESUBSCRIPTION_MAXSIZE);
	whatAttributes.push_back(EnumObjectAttribute_AESUBSCRIPTION_FILTER);
	// write changed data to the server
	result = subscription->setAttributesToServer(whatAttributes,
												 subscriptionResults,
												 NULL);
	_tprintf(_T("\nUpdate subscription's attributes to server - 0x%X\n  result buffer time: 0x%X\n  result max size: 0x%X\n  result filter: 0x%X\n"), result, subscriptionResults[0], subscriptionResults[1], subscriptionResults[2]);
	// get it from the server to check
	result = subscription->getAttributesFromServer(whatAttributes,
												   subscriptionResults,
												   NULL);
	_tprintf(_T("\nUpdate subscription's attributes from server - 0x%X\n  result buffer time: 0x%X\n  result max size: 0x%X\n  result filter: 0x%X\n"), result, subscriptionResults[0], subscriptionResults[1], subscriptionResults[2]);
	attributesIds[0] = 3;
	attributesIds[1] = 4;
	aeReturnedAttributes[0] = new AeReturnedAttributes();
	aeReturnedAttributes[0]->setAttributesIds(attributesIds);
	aeReturnedAttributes[0]->setCategoryId(2);
	_attributesIds[0] = 6;
	aeReturnedAttributes[1] = new AeReturnedAttributes();
	aeReturnedAttributes[1]->setAttributesIds(_attributesIds);
	aeReturnedAttributes[1]->setCategoryId(3);
	subscription->setReturnedAttributes(aeReturnedAttributes);
	whatAttributes.clear();
	whatAttributes.push_back(EnumObjectAttribute_AESUBSCRIPTION_RETURNED_ATTRIBUTES);
	// write changed data to the server
	result = subscription->setAttributesToServer(whatAttributes,
												 subscriptionResults,
												 NULL);
	_tprintf(_T("\nUpdate subscription's attributes to server - 0x%X\n  result returned attributes: 0x%X\n"), result, subscriptionResults[0]);
	// get it from the server to check
	result = subscription->getAttributesFromServer(whatAttributes,
												   subscriptionResults,
												   NULL);
	_tprintf(_T("\nUpdate subscription's attributes from server - 0x%X\n  result returned attributes: 0x%X\n "), result, subscriptionResults[0]);
	subscription->print();
}// end setSubscriptionAttributes

