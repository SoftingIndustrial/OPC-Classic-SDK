#ifndef _MYDAITEM_H_
#define _MYDAITEM_H_

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaItem.h"
#endif

#ifdef TBC_OS_LINUX
#include "ClientDaItem.h"
#endif

class MyDaItem :
	public SoftingOPCToolboxClient::DaItem
{
public:

	MyDaItem(tstring itemId,
			 SoftingOPCToolboxClient::DaSubscription* parentSubscription)
		: DaItem(itemId, parentSubscription)   {}

	virtual ~MyDaItem()
	{} // end dtor

	void handleStateChangeCompleted(
		EnumObjectState state)
	{
		tstring stateToString;

		switch (state)
		{
		case EnumObjectState_DISCONNECTED:
			stateToString = _T("DISCONNECTED");
			break;

		case EnumObjectState_CONNECTED:
			stateToString = _T("CONNECTED");
			break;

		case EnumObjectState_ACTIVATED:
			stateToString = _T("ACTIVATED");
		} //end switch

		_tprintf(_T("%s state changed - %s\n"), this->getId().c_str(), stateToString.c_str());
	} //end handleStateChange


	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T("%s performed state transition - context: %lu\n"), this->getId().c_str() , executionContext);
		} //end if
		else
		{
			_tprintf(_T(" performed state transition failed"));
		}
	} //handlePerformStateTransitionCompleted

};  // end class MyDaItem

#endif
