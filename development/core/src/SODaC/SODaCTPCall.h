#ifndef _SODACTPCALL_H_
#define _SODACTPCALL_H_

#ifdef SOFEATURE_TUNNEL

#include "SODaCServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCTPCall                                                                |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPCall : public SOCmnTPCall
{
public:
	SODaCTPCall(IN SOCltTPConnection* pCon);

	// server object

	HRESULT getStatus(OUT SODaCServerStatus& status);

	HRESULT browse(IN SOCltBrowseObject* pParent, IN LPCTSTR itemID, IN LPCTSTR cp, IN DWORD maxElements, IN DWORD browseFilter, IN LPCTSTR elementNameFilter,
				   IN LPCTSTR vendorFilter, IN BOOL returnProperties, IN BOOL retrievePropertyValues, IN SOCmnList<SODaCPropertyData> *propteryList,
				   OUT SOCmnList<SOCltBrowseObject> &objList, OUT SOCmnString* cpRet, OUT BOOL* moreElements);
	HRESULT getProperties(IN LPCTSTR itemID, IN BOOL retrievePropertyValues, IN BOOL returnAllProperties, IN OUT SOCmnList<SODaCPropertyData> &propteryList);

	HRESULT read(IN DWORD count, IN SOCmnString* pItemIDs, IN DWORD maxAge, OUT SOCmnVariant* pValues,
				 OUT WORD* pQualities = NULL, OUT SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	HRESULT write(IN DWORD count, IN SOCmnString* pItemIDs, IN SOCmnVariant* pValues,
				  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	HRESULT addGroup(IN SODaCGroup* pGroup, OUT DWORD& groupServerHandle);
	HRESULT removeGroup(IN SODaCGroup* pGroup, IN DWORD groupServerHandle);
	HRESULT updateGroup(IN BOOL fromServer, IN DWORD whatParameters = SODAC_GRPPAR_ALL);
	HRESULT setClientName(IN LPCTSTR clientName);

	// group object
	HRESULT addItems(IN SODaCGroup* pGroup, DWORD groupServerHandle, IN SOCmnList<SODaCItem> *itemList, IN SOCmnDWORDList* targetStateList);
	HRESULT removeItems(IN SODaCGroup* pGroup, DWORD groupServerHandle, IN SOCmnList<SODaCItem> *itemList);
	HRESULT validateItems(IN SODaCGroup* pGroup, DWORD groupServerHandle, IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors = NULL);
	HRESULT setItemsState(IN SODaCGroup* pGroup, IN SOCmnList<SODaCItem> *itemList);

	HRESULT updateItems(IN SODaCGroup* pGroup, IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList);
	HRESULT validateItems(IN SODaCGroup* pGroup, IN DWORD count, IN SODaCItem** itemArray, OUT HRESULT* pErrors = NULL);

	HRESULT syncRead(IN SODaCGroup* pGroup, IN DWORD groupServerHandle, IN DWORD count, IN SODaCItem** itemArray, IN DWORD maxAge,
					 SOCmnVariant* pValues, OUT WORD* pQualities = NULL,
					 SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);
	HRESULT syncWrite(IN SODaCGroup* pGroup, IN DWORD groupServerHandle, IN DWORD count, IN SODaCItem** itemArray, IN SOCmnVariant* pValues,
					  IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL, OUT HRESULT* pErrors = NULL);

	HRESULT asyncRead(IN SODaCGroup* pGroup,    IN DWORD groupServerHandle, IN DWORD count, IN SODaCItem** itemArray, IN DWORD maxAge,
					  IN DWORD transactionID = 0, OUT DWORD* cancelID = NULL, OUT HRESULT* pErrors = NULL);
	HRESULT asyncWrite(IN SODaCGroup* pGroup, IN DWORD groupServerHandle, IN DWORD count, IN SODaCItem** itemArray, IN DWORD transactionID,
					   IN SOCmnVariant* pValues, IN WORD* pQualities = NULL, IN SOCmnDateTime* pTimestamps = NULL,
					   OUT DWORD* cancelID = NULL, OUT HRESULT* pErrors = NULL);

	HRESULT refresh(IN SODaCGroup* pGroup, IN DWORD groupServerHandle, IN DWORD maxAge, IN DWORD transactionID = 0, OUT DWORD* cancelID = NULL);

protected:
	virtual ~SODaCTPCall(void);
}; // SODaCTPCall


//-----------------------------------------------------------------------------
// SODaCTPReceiver                                                            |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPReceiver : public SOCmnTPReceiver
{
public:
	SODaCTPReceiver(void);

	virtual void closeConnection(IN SOCmnTPConnection* pCon);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void onDataChange(IN SODaCServer* pServer, IN SOCmnTPCall* pCall, BYTE functionId);
	void onWriteComplete(IN SODaCServer* pServer, IN SOCmnTPCall* pCall);
}; // SODaCTPReceiver

#pragma pack(pop)

#endif

#endif
