#ifndef _CLIENTDAITEM_H_
#define _CLIENTDAITEM_H_

#include "OTClient.h"

#pragma pack(push, 4)


class ClientDAItem :
	public SODaCItem,
	virtual public SODaCItemAttrFull
{
public:
	ClientDAItem();

	DWORD getOTCUserData(void)
	{
		return m_userData;
	}
	void setOTCUserData(DWORD ud)
	{
		m_userData = ud;
	}

	void storeTargetState(void)
	{
		m_storedTargedState = getTargetObjectState();
	}
	void restoreTargetState(void)
	{
		setTargetObjectState(m_storedTargedState);
	}

protected:
	DWORD m_userData;
	BYTE m_storedTargedState;

	virtual void onSetObjectState(void);
};

#pragma pack(pop)
#endif
