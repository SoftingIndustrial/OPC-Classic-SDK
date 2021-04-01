#ifndef _CLIENTDASESSION_H_
#define _CLIENTDASESSION_H_

#include "OTClient.h"

#pragma pack(push, 4)


class ClientDASession : public SODaCServer
{
public:
	ClientDASession(void);

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

class ClientDASessionCom :
	public ClientDASession,
	virtual public SOCltComServerImpl,
	virtual public SODaCComServerImpl
{
public:
	ClientDASessionCom(void);

protected:
	virtual SOCltComServerImpl* getObjectCltComServerImpl(void);
};

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP

class ClientDASessionXml :
	public ClientDASession,
	virtual public SODaCXmlServerImpl
{
public:
	ClientDASessionXml(void);
};

#endif // SOFEATURE_SOAP

#ifdef SOFEATURE_TUNNEL

class ClientDASessionTp :
	public ClientDASession,
	virtual public SODaCTPServerImpl
{
public:
	ClientDASessionTp(void);
	virtual ~ClientDASessionTp(void);
};

#endif // SOFEATURE_TUNNEL

#pragma pack(pop)
#endif
