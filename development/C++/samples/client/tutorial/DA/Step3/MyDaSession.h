#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#include "Da\ClientDaSession.h"

class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{
public:

	MyDaSession(const tstring& url) :
		SoftingOPCToolboxClient::DaSession(url) {}

	virtual ~MyDaSession()
	{}  //  end dtor

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		//  Log the result
		// reconnect automatically
		return TRUE;
	}   //  end handleShutdownRequest

}; //   end class MyDaSession

#endif
