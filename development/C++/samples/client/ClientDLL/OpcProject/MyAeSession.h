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
	{}  //  dtor

	virtual unsigned char handleShutdownRequest(const tstring& reason)
	{
		//  log the shutdown reason
		return TRUE; // reconnect automatically
	}   //  end handleShutdownRequest

}; //end MySession

#endif
