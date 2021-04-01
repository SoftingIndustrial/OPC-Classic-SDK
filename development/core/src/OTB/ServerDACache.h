#ifndef _SERVERDACACHE_H_
#define _SERVERDACACHE_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDACache                                                                 |
//-----------------------------------------------------------------------------

class ServerDACache :
	public SODaSCache
{
public:
	ServerDACache(void);

	virtual WORD getQuality(IN SODaSRequest* req);
	virtual BOOL getTimeStamp(IN SODaSRequest* req, OUT SOCmnDateTime& timeStamp);
protected:
	~ServerDACache(void);
	virtual BOOL getValue(IN SODaSRequest* req, IN VARTYPE typeWanted, IN LCID localeID, OUT SOCmnVariant& value, OPTIONAL OUT HRESULT* pConvRes = NULL);

	BOOL getCache(IN SODaSRequest* req, OTSValueData& value);
};


#pragma pack(pop)
#endif
