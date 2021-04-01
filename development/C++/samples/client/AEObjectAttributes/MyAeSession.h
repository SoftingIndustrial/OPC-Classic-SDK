#ifndef _MYAESESSION_H_
#define _MYAESESSION_H_

#include "Ae\ClientAeSession.h"

class MyAeSession :
	public SoftingOPCToolboxClient::AeSession
{

public:

	MyAeSession(const tstring& url) :
		SoftingOPCToolboxClient::AeSession(url) {}

	virtual ~MyAeSession()
	{} // dtor

	virtual unsigned char handleShutdownRequest(const tstring& reason)
	{
		// log the shutdown reason
		return TRUE; // reconnect automatically
	} // end handleShutdownRequest

	void MyAeSession::print()
	{
		_tprintf(_T("\n%s\n"), _T("Session"));
		_tprintf(_T("  URL: %s\n"), getUrl().c_str());
		_tprintf(_T("  Client Name: %s\n"), getClientName().c_str());
		_tprintf(_T("  LCID: %s\n"), getLocaleId().c_str());
	}// end print

}; // end MySession

#endif
