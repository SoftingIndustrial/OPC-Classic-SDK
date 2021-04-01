#ifndef _CLIENTAESESSION_H_
#define _CLIENTAESESSION_H_

#include "OTClient.h"

#pragma pack(push, 4)


class ClientAESession : public SOAeCServer
{
public:
	ClientAESession(void);

	DWORD getOTCUserData(void)
	{
		return m_userData;
	}
	void setOTCUserData(DWORD ud)
	{
		m_userData = ud;
	}

	void storeTargetState(IN BYTE deep);
	void restoreTargetState(IN BYTE deep);

	// shutdown
	virtual void shutdown(IN LPCTSTR reason);

protected:
	DWORD m_userData;
	BYTE m_storedTargedState;

	virtual void onSetObjectState(void);
};

#ifdef SOFEATURE_DCOM

class ClientAESessionCom :
	public ClientAESession,
	virtual public SOCltComServerImpl,
	virtual public SOAeCComServerImpl
{
public:
	ClientAESessionCom(void);

protected:
	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
};

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_TUNNEL

class ClientAESessionTp :
	public ClientAESession,
	virtual public SOAeCTPServerImpl
{
public:
	ClientAESessionTp(void);
	virtual ~ClientAESessionTp(void);
};

#endif // SOFEATURE_TUNNEL

#pragma pack(pop)
#endif
