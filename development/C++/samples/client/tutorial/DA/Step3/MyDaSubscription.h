#ifndef _MYDASUBSCRIPTION_H_
#define _MYDASUBSCRIPTION_H_

#include "Da\ClientDaSubscription.h"
#include "Da\ClientDaItem.h"

class MyDaSubscription :
	public SoftingOPCToolboxClient::DaSubscription
{
public:

	MyDaSubscription(
		unsigned long updateRate,
		SoftingOPCToolboxClient::DaSession* parentSession) :
		SoftingOPCToolboxClient::DaSubscription(updateRate, parentSession)
	{}

	virtual ~MyDaSubscription()
	{}  //  end dtor

	void handleDataChanged(
		const std::vector<SoftingOPCToolboxClient::DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results)
	{
		unsigned long i;
		size_t count = items.size();
		_tprintf(_T("\n%s - Data changed\n"), _T("Subscription"));

		for (i = 0; i < count; i++)
		{
			_tprintf(_T("%-23.23s - %s\n"), items[i]->getId().c_str(), values[i]->toString().c_str());
		} //end for
	}

};  //  end class MyDaSubscription

#endif
