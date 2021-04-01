#ifndef _SOXPSENTRY_H_
#define _SOXPSENTRY_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


#include <SODaSEntry.h>
#include <SOAeSEntry.h>

//-----------------------------------------------------------------------------
// SOXpSEventHandler                                                          |
//-----------------------------------------------------------------------------

class SOXpSEventHandler : public SOSrvEventHandler
{
public:
	SOXpSEventHandler(IN BOOL startedByCom,	IN HWND wnd);

protected:
	BOOL m_startedByCom;
	HWND m_wnd;

	// released last COM object of the server
	virtual void onReleasedLastOPCObject(void);
}; // SOXpSEventHandler




//-----------------------------------------------------------------------------
// SOXpSAECreator                                                             |
//-----------------------------------------------------------------------------

class SOXpSAECreator : public SOAeSCreator
{
public:
	virtual SOAeSServer *createServer(IN BYTE serverInstance);
	virtual SOAeSSubscription *createSubscription(IN SOAeSServer *server);
}; // SOXpSAECreator




//-----------------------------------------------------------------------------
// SOXpSDACreator                                                             |
//-----------------------------------------------------------------------------

class SOXpSDACreator : public SODaSCreator
{
public:
	virtual SODaSServer *createServer(IN BOOL publicGroups, IN BYTE serverInstance);
	virtual SODaSTransaction *createTransaction(IN SODaSGroup *group, IN BYTE transactionType,
		IN BYTE connectionType, IN DWORD connection);
	virtual SODaSRequest *createRequest(IN SODaSServer *server, IN SODaSItem *item,
		IN SODaSItemTag *tag, IN BYTE operation, IN WORD type);
	virtual SODaSItem *createItem(IN SODaSGroup *parent, IN SODaSItemTag *tag);

}; // SOXpSDACreator

#pragma pack(pop)
#endif
