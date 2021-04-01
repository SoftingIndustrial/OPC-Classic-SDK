#ifndef _SOAECTPCALL_H_
#define _SOAECTPCALL_H_

#ifdef SOFEATURE_TUNNEL

#include "SOAeCServer.h"

#pragma pack(push, 4)


class SOAEC_EXPORT SOAeCTPCallData : public SOCmnData
{
public:
	DWORD m_id;
	SOCmnString m_description;
	VARTYPE m_datatype;
};

//-----------------------------------------------------------------------------
// SOAeCTPCall                                                                |
//-----------------------------------------------------------------------------

class SOAEC_EXPORT SOAeCTPCall : public SOCmnTPCall
{
public:
	SOAeCTPCall(IN SOCltTPConnection* pCon);

	// server object
	HRESULT addSubscription(IN SOAeCSubscription* pSubscription, OUT DWORD& subscriptionServerHandle);
	HRESULT removeSubscription(IN SOAeCSubscription* pSubscription, IN DWORD subscriptionServerHandle);
	HRESULT queryEventCategories(IN DWORD eventType, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT queryEventAttributes(IN DWORD catID, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT queryConditionNames(IN DWORD catID, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT querySubConditionNames(IN LPCTSTR condName, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT querySourceConditions(IN LPCTSTR sourceQName, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT addAreaBrowser(OUT DWORD& areaBrowserServerHandle);
	HRESULT removeAreaBrowser(IN DWORD areaBrowserServerHandle);
	HRESULT ackCondition(IN SOCmnList<SOAeCEvent> *pEventList, IN LPCTSTR ackID, IN LPCTSTR ackComment);
	HRESULT getStatus(OUT SOAeCServerStatus& status);

	// browse object
	HRESULT changeBrowsePosition(IN DWORD areaBrowserHandle, IN DWORD browseDirection, IN LPCTSTR name);
	HRESULT browseOPCAreas(IN DWORD areaBrowserHandle, IN DWORD typeFilter, IN LPCTSTR elementNameFilter, OUT SOCmnList<SOAeCTPCallData> *pCallOutDataList);
	HRESULT getQualifiedSourceName(IN DWORD areaBrowserHandle, IN LPCTSTR name, OUT SOCmnString& qName);
	HRESULT getQualifiedAreaName(IN DWORD areaBrowserHandle, IN LPCTSTR name, OUT SOCmnString& qName);

	// subscription object
	HRESULT getReturnedAttributes(IN DWORD subscriptionServerHandle, IN DWORD category, IN OUT SOCmnDWORDList* pListRAC);
	HRESULT selectReturnedAttributes(IN DWORD subscriptionServerHandle, IN DWORD category, IN SOCmnDWORDList* pListRAC);
	HRESULT refresh(IN DWORD subscriptionServerHandle);
	HRESULT cancelRefresh(IN DWORD subscriptionServerHandle);

protected:
	virtual ~SOAeCTPCall(void);
}; // SOAeCTPCall


//-----------------------------------------------------------------------------
// SOAeCTPReceiver                                                            |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SOAeCTPReceiver : public SOCmnTPReceiver
{
public:
	SOAeCTPReceiver(void);

	virtual void closeConnection(IN SOCmnTPConnection* pCon);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void onEvent(IN SOAeCServer* pServer, IN SOCmnTPCall* pCall);
}; // SOAeCTPReceiver

#pragma pack(pop)

#endif  //  SOFEATURE_TUNNEL

#endif
