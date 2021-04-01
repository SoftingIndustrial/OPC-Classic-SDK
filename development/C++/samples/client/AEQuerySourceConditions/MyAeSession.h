#ifndef _MYAESESSION_H_
#define _MYAESESSION_H_

#include "Ae\ClientAeSession.h"

class MyAeSession :
	public SoftingOPCToolboxClient::AeSession
{

public:

	MyAeSession(const tstring& url) :
		SoftingOPCToolboxClient::AeSession(url) {}

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		_tprintf(_T("Session shutdown - reason: %s\n"), reason.c_str());
		return TRUE; // reconnect automatically
	}

	void handleQueryAeSourceConditionsCompleted(
		unsigned long executioncontext,
		tstring& sourceName,
		std::vector<tstring>& conditionNames,
		long result)
	{
		unsigned long i;

		if (SUCCEEDED(result))
		{
			for (i = 0; i < conditionNames.size(); i++)
			{
				_tprintf(_T("	[%i]%s"), i, conditionNames[i].c_str());
				_tprintf(_T("\n"));
			} //end for
		} //end if
		else
		{
			_tprintf(_T(" Failed to asynchronously query source conditions for source: %s"), sourceName.c_str());
		}//end if ... else
	} //end handleGetDaPropertiesCompleted

	virtual ~MyAeSession()
	{} // dtor

}; // end MySession

#endif
