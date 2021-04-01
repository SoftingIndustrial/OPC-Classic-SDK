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
	{}  //  dtor

};  //  end class MyAeSubscription

#endif
