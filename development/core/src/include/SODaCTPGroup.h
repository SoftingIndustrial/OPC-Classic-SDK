#ifndef _SODACTPGROUP_H_
#define _SODACTPGROUP_H_

#ifdef SOFEATURE_TUNNEL

#include "SODaC.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaCTPGroupImpl                                                          |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPGroupImpl : virtual public SODaCGroupICommunication
{
public:
	SODaCTPGroupImpl(void);

	virtual void init(SODaCServer* pServer);

	virtual HRESULT updateItemsImpl(IN BOOL fromServer, IN SOCmnList<SODaCItem> *itemList, IN OPTIONAL DWORD whatParameters = SODAC_ITMPAR_ALL);

	// synchronous I/O
	virtual HRESULT syncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								 OUT OPTIONAL SOCmnVariant* pValues = NULL, OUT OPTIONAL WORD* pQualities = NULL,
								 OUT OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT syncWriteImpl(IN DWORD count, IN SODaCItem** itemArray, IN OPTIONAL SOCmnVariant* pValues = NULL,
								  IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);

	// asynchronous I/O
	virtual HRESULT asyncReadImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE,
								  IN OPTIONAL DWORD transactionID = 0, OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncWriteImpl(IN DWORD count, IN OPTIONAL SODaCItem** itemArray, IN OPTIONAL DWORD transactionID = 0,
								   IN OPTIONAL SOCmnVariant* pValues = NULL, IN OPTIONAL WORD* pQualities = NULL, IN OPTIONAL SOCmnDateTime* pTimestamps = NULL,
								   OUT OPTIONAL DWORD* cancelID = NULL, OUT OPTIONAL HRESULT* pErrors = NULL);
	virtual HRESULT asyncRefreshImpl(IN OPTIONAL DWORD maxAge = SODAC_MAXAGE_DEVICE, IN OPTIONAL DWORD transactionID = 0,
									 OUT OPTIONAL DWORD* cancelID = NULL);
	virtual HRESULT asyncCancelImpl(IN DWORD cancelID);

	DWORD getServerHandle(void)
	{
		return m_serverHandle;
	}

	virtual HRESULT validateItems(IN SOCmnList<SODaCItem> *itemList, OUT HRESULT* pErrors = NULL);

protected:
	DWORD m_serverHandle;

	virtual ~SODaCTPGroupImpl(void);

	// group management
	virtual HRESULT doUpdateImpl(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = SODAC_GRPPAR_ALL_MGT);

	// do the state transitions
	virtual HRESULT doDisconnectImpl(void);
	virtual HRESULT doStateTransitionImpl(BOOL deep, IN BYTE stateDeep);
	HRESULT doItemsStateTransition(void);
}; // SODaCTPGroupImpl


//-----------------------------------------------------------------------------
// SODaCTPGroupDefault                                                       |
//-----------------------------------------------------------------------------

class SODAC_EXPORT SODaCTPGroupDefault :
	public SODaCGroup,
	virtual public SODaCTPGroupImpl,
	virtual public SOCltElementErrorStore
{
public:
	SODaCTPGroupDefault(void);

protected:
	virtual SOCmnObject* getObjectCltElementIError(void);
};

#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif

