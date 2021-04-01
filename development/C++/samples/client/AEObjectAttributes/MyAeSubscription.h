#ifndef _MYAESUBSCRIPTION_H_
#define _MYAESUBSCRIPTION_H_

#include "Ae\ClientAeSubscription.h"

class MyAeSubscription :
	public SoftingOPCToolboxClient::AeSubscription
{
public:

	MyAeSubscription(SoftingOPCToolboxClient::AeSession* parentSession) :
		SoftingOPCToolboxClient::AeSubscription(parentSession)
	{}

	virtual ~MyAeSubscription()
	{} // dtor

	void MyAeSubscription::print()
	{
		unsigned long i, j;
		std::vector<unsigned long> filterCategories;
		std::vector<tstring> filterAreas;
		std::vector<tstring> filterSources;
		std::vector<AeReturnedAttributes*> returnedAttributes;
		std::vector<AeReturnedAttributes*>::iterator returnedAttributesIterator;
		_tprintf(_T("\n%s\n"), _T("Subscription"));
		_tprintf(_T("  Requested Buffer Time: %lu ms\n"), getRequestedBufferTime());
		_tprintf(_T("  Revised Buffer Time: %lu ms\n"), getRevisedBufferTime());
		_tprintf(_T("  Requested Max Size: %lu events\n"), getRequestedMaxSize());
		_tprintf(_T("  Revised Max Size: %lu events\n"), getRevisedMaxSize());
		_tprintf(_T("  Filter:\n"));
		_tprintf(_T("    Low Severity: %lu\n"), getFilterSeverityLow());
		_tprintf(_T("    High Severity: %lu\n"), getFilterSeverityHigh());
		_tprintf(_T("    Event Types: %s\n"), (EventTypesToString(getFilterEventTypes())).c_str());
		filterCategories = getFilterCategories();
		_tprintf(_T("    Categories: count: %lu\n"), getFilterCategories().size());

		for (i = 0; i < getFilterCategories().size(); i++)
		{
			_tprintf(_T("      [%3.3lu] : %lu\n"), i, filterCategories[i]);
		}

		filterAreas = getFilterAreas();
		_tprintf(_T("    Areas: count: %lu\n"), getFilterAreas().size());

		for (i = 0; i < getFilterAreas().size(); i++)
		{
			_tprintf(_T("      [%3.3lu] : %s\n"), i, (filterAreas[i]).c_str());
		}

		filterSources = getFilterSources();
		_tprintf(_T("    Sources: count: %lu\n"), getFilterSources().size());

		for (i = 0; i < getFilterSources().size(); i++)
		{
			_tprintf(_T("      [%3.3lu] : %s\n"), i, (filterSources[i]).c_str());
		}

		returnedAttributes =  getReturnedAttributes();
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
	}// end print

	tstring MyAeSubscription::EventTypesToString(unsigned long eventType)
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
	}// end EventTypesToString

};  // end class MyAeSubscription

#endif
