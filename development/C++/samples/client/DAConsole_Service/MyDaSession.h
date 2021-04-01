#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#include <fstream>
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > tofstream;

#include "Da\ClientDaSession.h"

class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{

private:

	tofstream m_file;

public:

	MyDaSession(const tstring& url) :
		SoftingOPCToolboxClient::DaSession(url) {}

	virtual ~MyDaSession()
	{} // end dtor

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		// TODO change the name of the file where to write the results
		m_file.open("C:\\Service_TraceData.txt", tofstream::in  | tofstream::out  | tofstream::app);
		m_file << _T("Session shutdown - reason: ") << reason.c_str() << _T("\n");
		m_file.close();
		// reconnect automatically
		return TRUE;
	}   //  end handleShutdownRequest

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
	} //end handleStateChange

	void handlePerformStateTransitionCompleted(unsigned long executionContext, long result)
	{
		// TODO change the name of the file where to write the results
		m_file.open("C:\\Service_TraceData.txt", tofstream::in  | tofstream::out  | tofstream::app);

		if (SUCCEEDED(result))
		{
			m_file << "\n" << _T("Session performed state transition - context: ") << executionContext;
		}
		else
		{
			m_file << "\n" << _T("Session state transition failed!");
		}

		m_file.close();
	} //end handlePerformStateTransitionCompleted
}; //   end class MyDaSession

#endif
