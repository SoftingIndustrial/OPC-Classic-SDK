#ifndef _BASEEVENT_H_
#define _BASEEVENT_H_

#include "Ae/ServerAeEvent.h"

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// BaseEvent
//-----------------------------------------------------------------------------
class BaseEvent : public AeEvent
{
public:
	BaseEvent(
		tstring aSource,
		unsigned int aCategoryId)  :
		AeEvent(EnumEventType_SIMPLE, aSource, aCategoryId)
	{
	} // end ctr

	virtual ~BaseEvent(
		void)
	{
	} // end dtor
};  //  end BaseEvent

#endif
