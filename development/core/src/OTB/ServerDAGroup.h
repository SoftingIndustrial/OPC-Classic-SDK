#ifndef _SERVERDAGROUP_H_
#define _SERVERDAGROUP_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDAGroup
//-----------------------------------------------------------------------------

class ServerDAGroup : public SODaSGroupDefault
{
public:
	ServerDAGroup(void);

	virtual BOOL setUpdateRate(IN DWORD updateRate);

protected:
	virtual ~ServerDAGroup(void);

	// method callbacks
	virtual void onAddItems(void);
	virtual void onRemoveItems(void);
	virtual void onChangeStateItems(void);

	void changeItems(void);
}; // ServerDAGroup


#pragma pack(pop)
#endif
