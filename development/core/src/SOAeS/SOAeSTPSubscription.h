#ifndef _SOAESTPSUBSCRIPTION_H_
#define _SOAESTPSUBSCRIPTION_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#include "SOSrvTP.h"

class SODAS_EXPORT SOAeSTPSubscription : public SOCmnTPReceiver
{
protected:
	BOOL validConnection(DWORD aConnection);

public:
	SOAeSTPSubscription(void);
	virtual ~SOAeSTPSubscription(void);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void selectReturnedAttributes(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void getReturnedAttributes(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void refresh(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void cancelRefresh(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	/* not used until now
	    void setFilter(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void getFilter(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void getState(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void setState(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	not used until now */
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SOAESTPSUBSCRIPTION_H_
