#ifndef _SODASTPGROUP_H_
#define _SODASTPGROUP_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#include "SOSrvTP.h"

class SODAS_EXPORT SODaSTPGroup : public SOCmnTPReceiver
{
public:
	SODaSTPGroup(void);

	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void addItems(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void removeItems(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void syncWrite(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void syncRead(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void asyncWrite(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void asyncRead(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void validateItems(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);
	void refresh(IN SODaSServer* pServer, IN SOCmnTPCall* pCall);

protected:
	virtual ~SODaSTPGroup(void);
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SODASTPGROUP_H_
