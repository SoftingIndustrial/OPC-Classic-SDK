#ifndef _MYDASUBSCRIPTION_H_
#define _MYDASUBSCRIPTION_H_

#include <fstream>
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > tofstream;

#include "Da\ClientDaSubscription.h"
#include "Da\ClientDaItem.h"

class MyDaSubscription :
	public SoftingOPCToolboxClient::DaSubscription
{
private:
	tofstream m_file;
public:

	MyDaSubscription(
		unsigned long updateRate,
		SoftingOPCToolboxClient::DaSession* parentSession) :
		SoftingOPCToolboxClient::DaSubscription(updateRate, parentSession)
	{}

	virtual ~MyDaSubscription()
	{}  //  end dtor

	void handleStateChangeCompleted(SoftingOPCToolboxClient::EnumObjectState state)
	{
		tstring stateToString;

		switch (state)
		{
		case SoftingOPCToolboxClient::EnumObjectState_DISCONNECTED:
			stateToString = _T("DISCONNECTED");
			break;

		case SoftingOPCToolboxClient::EnumObjectState_CONNECTED:
			stateToString = _T("CONNECTED");
			break;

		case SoftingOPCToolboxClient::EnumObjectState_ACTIVATED:
			stateToString = _T("ACTIVATED");
		} //end switch

		// TODO change the name of the file where to write the results
		m_file.open("C:\\Service_TraceData.txt", tofstream::in  | tofstream::out  | tofstream::app);
		m_file << "\n" << _T("Session state changed - ") << stateToString.c_str();
		m_file.close();
	}   //  end handleStateChange

	void handlePerformStateTransitionCompleted(unsigned long executionContext, long result)
	{
		// TODO change the name of the file where to write the results
		m_file.open("C:\\Service_TraceData.txt", tofstream::in  | tofstream::out  | tofstream::app);

		if (SUCCEEDED(result))
		{
			m_file << "\n" << _T("Subscription performed state transition - context: ") << executionContext;
		}
		else
		{
			m_file << "\n" << _T("Subscription state transition failed!");
		}

		m_file.close();
	}   //  end handlePerformStateTransitionCompleted

	void handleDataChanged(
		const std::vector<SoftingOPCToolboxClient::DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results)
	{
		unsigned long i;
		size_t count = items.size();
		// TODO change the name of the file where to write the results
		m_file.open("C:\\Service_TraceData.txt", tofstream::in  | tofstream::out  | tofstream::app);
		m_file << "\n" << _T("Subscription Data changed \n");

		for (i = 0; i < count; i++)
		{
			TCHAR buffer[300];
			_stprintf(buffer, _T("%-23.23s - %s\n"), items[i]->getId().c_str(), values[i]->toString().c_str());
			m_file << buffer;
		}   //  end for

		m_file.close();
	}   //  end handleDataChanged

};  //  end class MyDaSubscription

#endif
