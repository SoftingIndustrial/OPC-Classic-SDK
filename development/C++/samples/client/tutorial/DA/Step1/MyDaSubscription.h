#ifndef _MYDASUBSCRIPTION_H_
#define _MYDASUBSCRIPTION_H_

#include "Da\ClientDaSubscription.h"

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

};  //  end class MyDaSubscription

#endif
