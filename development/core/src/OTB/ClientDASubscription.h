#ifndef _CLIENTDASUBSCRIPTION_H_
#define _CLIENTDASUBSCRIPTION_H_

#include "OTClient.h"

#pragma pack(push, 4)


class ClientDASubscription : public SODaCGroup
{
public:
	ClientDASubscription();

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

protected:
	DWORD m_userData;
	BYTE m_storedTargedState;

	virtual void onSetObjectState(void);
	virtual void onReadAdvise(IN BYTE dataChangeOrRead, IN DWORD transactionId, IN HRESULT masterQuality, IN HRESULT masterError,
							  IN DWORD itemCnt, IN SODaCItem** ppItems, IN SOCmnVariant* pValues, IN WORD* pQualities,
							  IN SOCmnDateTime* pTimestamps, IN HRESULT* pErrors);
	virtual void onWriteAdvise(IN DWORD transactionId, IN HRESULT masterError, IN DWORD itemCount,
							   IN SODaCItem** ppItems, IN HRESULT* pErrors);
};


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// ClientDASubscriptionCom                                                            |
//-----------------------------------------------------------------------------

class ClientDASubscriptionCom :
	public ClientDASubscription,
	virtual public SODaCComGroupImpl
{
public:
	ClientDASubscriptionCom(void);
};

#endif // SOFEATURE_DCOM

#ifdef SOFEATURE_SOAP

//-----------------------------------------------------------------------------
// ClientDASubscriptionXml                                                    |
//-----------------------------------------------------------------------------

class ClientDASubscriptionXml :
	public ClientDASubscription,
	virtual public SODaCXmlGroupImpl
{
public:
	ClientDASubscriptionXml();
};

#endif // SOFEATURE_SOAP


#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// ClientDASubscriptionTp                                                     |
//-----------------------------------------------------------------------------

class ClientDASubscriptionTp :
	public ClientDASubscription,
	virtual public SODaCTPGroupImpl
{
public:
	ClientDASubscriptionTp();
};

#endif // SOFEATURE_TUNNEL

#pragma pack(pop)
#endif
