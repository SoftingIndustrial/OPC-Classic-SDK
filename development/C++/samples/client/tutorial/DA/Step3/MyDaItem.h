#ifndef _MYDAITEM_H_
#define _MYDAITEM_H_

#include "Da\ClientDaItem.h"

class MyDaItem :
	public SoftingOPCToolboxClient::DaItem
{
public:

	MyDaItem(
		tstring itemId,
		SoftingOPCToolboxClient::DaSubscription* parentSubscription) :
		DaItem(itemId, parentSubscription)
	{}

	virtual ~MyDaItem()
	{}  //  end dtor

};  //  end class MyDaItem

#endif
