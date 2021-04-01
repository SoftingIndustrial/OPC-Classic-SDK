#ifndef _SOAESTPSERVER_H_
#define _SOAESTPSERVER_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#include "SOAeSTPServer.h"


class SOAES_EXPORT SOAeSTPServer : public SOCmnTPReceiver
{
public:
	SOAeSTPServer(void);

	virtual void closeConnection(IN SOCmnTPConnection* pCon);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void getStatus(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	void addSubscription(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void removeSubscription(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	void queryEventCategories(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void queryConditionNames(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void querySubConditionNames(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void querySourceConditions(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void queryEventAttributes(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	void ackCondition(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	void addAreaBrowser(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void removeAreaBrowser(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);

	/* not used until now
	    void queryAvailableFilters(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);

	    void getConditionState(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);

	    void enableConditionByArea(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void enableConditionBySource(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void disableConditionByArea(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	    void disableConditionBySource(IN SOAeSServer *pServer, IN SOCmnTPCall *pCall);
	not used until now */

protected:

	virtual ~SOAeSTPServer(void);
	HRESULT enableConditions(SOAeSServer* pServer, BOOL enable, BOOL areaOrSource, SOCmnStringList& anConditionList);
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SOAESTPSERVER_H_
