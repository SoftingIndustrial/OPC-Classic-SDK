#ifndef _SOCLTTP_H_
#define _SOCLTTP_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


class SOCltTPConnection;
class SOCLT_EXPORT SOCltTP : public SOCmnSocketClient
{
public:
	SOCltTP();

protected:
	~SOCltTP();
	virtual void rcvData(IN SOCmnObject* pSocket, IN LONG size, IN BYTE* pData);
	virtual BOOL addClient(IN SOCKET hSocket, IN struct sockaddr_in* pSockAddr, IN OUT SOCmnObject** pSocket);
	virtual void timeOut(IN SOCmnObject* pSocket);

	SOCmnList<SOCmnTPReceiver> m_receiverList;

public:
	virtual BOOL removeClient(IN SOCmnObject* pSocket, IN LONG reason = SOCMNSOCKET_REQ_GRACEFUL_CLOSE);

	SOCmnList<SOCmnTPReceiver> *getTPReceiverList(void)
	{
		return &m_receiverList;
	}
	BOOL addTPReceiver(SOCmnTPReceiver* pRec)
	{
		return m_receiverList.add(pRec);
	}
};

class SOCLT_EXPORT SOCltTPConnection : public SOCmnTPConnection
{
public:
	SOCltTPConnection();

	bool init(SOCmnSocket* pSrv, SOCmnObject* pSocket);
	void close(void);
	void sendPing(void);

	SOCmnEvent* getConnectEvent(void)
	{
		return &m_connectEvent;
	}

	virtual HRESULT invoke(SOCmnTPCall* pCall, BOOL waitForReply = TRUE);
	virtual SOCmnList<SOCmnTPReceiver> *getReceiverList(void);

	SOCltServer* getServer(void)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_sync);
		m_server.addRef();
		return m_server;
	}
	void setServer(SOCltServer* s)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_sync);
		m_server = s;
		m_server.addRef();
	}

	DWORD getTimeOut(void)
	{
		return m_timeout;
	}
	void setTimeOut(DWORD to)
	{
		m_timeout = to > 50000 ? 50000 : to;
	}

	SOCmnTPCredentials* getCredentials();
	void setCredentials(SOCmnTPCredentials* pCredentials);

protected:
	SOCmnEvent m_connectEvent;
	SOCmnPointer<SOCltServer> m_server;
	bool m_waitForPing;
	DWORD m_timeout;

	SOCmnPointer<SOCmnTPCredentials> m_pCredentials;

	virtual ~SOCltTPConnection();

	virtual bool sendInitMessage();
	virtual bool handleInitMessage();
	virtual bool handlePingMessage();
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SOCLTTP_H_
