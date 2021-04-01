#include "ClientApplication.h"
#include "ClientAeSession.h"
#include "ClientAeSubscription.h"
#include "ClientCommon.h"
#include "ClientEnums.h"

#include <conio.h>

using namespace std;
using namespace SoftingOPCToolboxClient;

tstring GetOPCSpecificationString(EnumOPCSpecification specification)
{
	tstring spec;

	switch (specification)
	{
	case EnumOPCSpecification_DEFAULT:
		spec = _T("default");
		break;

	case EnumOPCSpecification_DA10:
		spec = _T("DA1");
		break;

	case EnumOPCSpecification_DA20:
		spec = _T("DA2");
		break;

	case EnumOPCSpecification_DA30 :
		spec = _T("DA3");
		break;

	case EnumOPCSpecification_XMLDA10:
		spec = _T("XML-DA");
		break;
	} //end switch

	return spec;
}
void PrintAeSession(AeSession& session)
{
	_tprintf(_T("\n%s\n"), _T("Session"));
	_tprintf(_T("  URL: %s\n"), session.getUrl().c_str());
	_tprintf(_T("  Client Name: %s\n"), session.getClientName().c_str());
	_tprintf(_T("  LCID: %s\n"), session.getLocaleId().c_str());
}
tstring EventTypesToString(unsigned long eventType)
{
	tstring eventTypeToString(_T(""));

	if ((eventType & EnumEventType_SIMPLE) == EnumEventType_SIMPLE)
	{
		eventTypeToString += _T(" Simple");
	} //end if

	if ((eventType & EnumEventType_CONDITION) == EnumEventType_CONDITION)
	{
		eventTypeToString += _T(" Condition");
	} //end if

	if ((eventType & EnumEventType_TRACKING) == EnumEventType_TRACKING)
	{
		eventTypeToString += _T(" Tracking");
	} //end if

	return eventTypeToString;
}
void PrintAeSubscription(AeSubscription& subscription)
{
	unsigned long i, j;
	std::vector<unsigned long> filterCategories;
	std::vector<tstring> filterAreas;
	std::vector<tstring> filterSources;
	std::vector<AeReturnedAttributes*> returnedAttributes;
	std::vector<AeReturnedAttributes*>::iterator returnedAttributesIterator;
	_tprintf(_T("\n%s\n"), _T("Subscription"));
	_tprintf(_T("  Requested Buffer Time: %lu ms\n"), subscription.getRequestedBufferTime());
	_tprintf(_T("  Revised Buffer Time: %lu ms\n"), subscription.getRevisedBufferTime());
	_tprintf(_T("  Requested Max Size: %lu events\n"), subscription.getRequestedMaxSize());
	_tprintf(_T("  Revised Max Size: %lu events\n"), subscription.getRevisedMaxSize());
	_tprintf(_T("  Filter:\n"));
	_tprintf(_T("    Low Severity: %lu\n"), subscription.getFilterSeverityLow());
	_tprintf(_T("    High Severity: %lu\n"), subscription.getFilterSeverityHigh());
	_tprintf(_T("    Event Types: %s\n"), (EventTypesToString(subscription.getFilterEventTypes())).c_str());
	filterCategories = subscription.getFilterCategories();
	_tprintf(_T("    Categories: count: %lu\n"), subscription.getFilterCategories().size());

	for (i = 0; i < subscription.getFilterCategories().size(); i++)
	{
		_tprintf(_T("      [%3.3lu] : %lu\n"), i, filterCategories[i]);
	}

	filterAreas = subscription.getFilterAreas();
	_tprintf(_T("    Areas: count: %lu\n"), subscription.getFilterAreas().size());

	for (i = 0; i < subscription.getFilterAreas().size(); i++)
	{
		_tprintf(_T("      [%3.3lu] : %s\n"), i, (filterAreas[i]).c_str());
	}

	filterSources = subscription.getFilterSources();
	_tprintf(_T("    Sources: count: %lu\n"), subscription.getFilterSources().size());

	for (i = 0; i < subscription.getFilterSources().size(); i++)
	{
		_tprintf(_T("      [%3.3lu] : %s\n"), i, (filterSources[i]).c_str());
	}

	returnedAttributes =  subscription.getReturnedAttributes();
	_tprintf(_T("  Returned Attributes: count: %lu\n"), returnedAttributes.size());

	for (i = 0; i < returnedAttributes.size(); i++)
	{
		_tprintf(_T("    [%3.3lu] : category ID %lu : count %lu\n"), i, returnedAttributes[i]->getCategoryId(), returnedAttributes[i]->getAttributesIds().size());

		for (j = 0; j < returnedAttributes[i]->getAttributesIds().size(); j++)
		{
			_tprintf(_T("      [%3.3lu] : %lu\n"), j, (returnedAttributes[i]->getAttributesIds())[j]);
		} //end for
	} //end for

	for (returnedAttributesIterator = returnedAttributes.begin(); returnedAttributesIterator != returnedAttributes.end(); returnedAttributesIterator++)
	{
		delete(*returnedAttributesIterator);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
	unsigned long count = 3;
	unsigned long size = 2, size1 = 1;
	std::vector<EnumObjectAttribute> whatAttributes;
	std::vector<long>sessionResults(2, 0);
	std::vector<long>subscriptionResults(4, 0);
	std::vector<unsigned long> filterCategories(count, 0);
	std::vector<tstring> filterAreas(size, _T(""));
	std::vector<tstring> filterSources(size, _T(""));
	std::vector<AeReturnedAttributes*>aeReturnedAttributes(size, NULL);
	std::vector<unsigned long> attributesIds(size, 0);
	std::vector<unsigned long> _attributesIds(size1, 0);
	Application* application = getApplication();
	application->setVersionOtb(447);
	application->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time

	//  activate the COM-AE Client Feature
	//result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
	}

	//  END TODO - binary license activation

	if (SUCCEEDED(application->initialize()))
	{
		application->setUpdateAttributesBehavior(EnumUpdateAttributesBehavior_EXPLICIT_UPDATE);
		AeSession* session = new AeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
		AeSubscription* subscription = new AeSubscription(session);
		session->connect(TRUE, FALSE, NULL);
		_tprintf(_T("--- Initial object attributes ---\n"));
		PrintAeSession(*session);
		PrintAeSubscription(*subscription);
		// change the session LCID and client name
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
		_tprintf(_T("\nUpdate session's attributes to server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), result, sessionResults[0], sessionResults[1]);
		// get it from the server to check
		result = session->getAttributesFromServer
				 (whatAttributes,
				  sessionResults,
				  NULL);
		_tprintf(_T("\nUpdate session's attributes from server - 0x%X\n  result LCID: 0x%X\n  result client name: 0x%X\n"), result, sessionResults[0], sessionResults[1]);
		PrintAeSession(*session);
		whatAttributes.clear();
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
		result = subscription->setAttributesToServer
				 (whatAttributes,
				  subscriptionResults,
				  NULL);
		_tprintf(_T("\nUpdate subscription's attributes to server - 0x%X\n  result buffer time: 0x%X\n  result max size: 0x%X\n  result filter: 0x%X\n"), result, subscriptionResults[0], subscriptionResults[1], subscriptionResults[2]);
		// get it from the server to check
		result = subscription->getAttributesFromServer
				 (whatAttributes,
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
		result = subscription->setAttributesToServer
				 (whatAttributes,
				  subscriptionResults,
				  NULL);
		_tprintf(_T("\nUpdate subscription's attributes to server - 0x%X\n  result returned attributes: 0x%X\n"), result, subscriptionResults[0]);
		// get it from the server to check
		result = subscription->getAttributesFromServer
				 (whatAttributes,
				  subscriptionResults,
				  NULL);
		_tprintf(_T("\nUpdate subscription's attributes from server - 0x%X\n  result returned attributes: 0x%X\n "), result, subscriptionResults[0]);
		PrintAeSubscription(*subscription);

		if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			session->disconnect(NULL);
		}

		if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			subscription->disconnect(NULL);
		}

		session->removeAeSubscription(subscription);
		application->removeAeSession(session);
		application->terminate();
		releaseApplication();
		delete aeReturnedAttributes[0];
		delete aeReturnedAttributes[1];
		delete session;
		delete subscription;
	} // end if initialize succedeed

	_tprintf(_T("\nPress any key to finish..."));
	_getch();
	return 0;
}

