#ifndef _SERVERDACREATOR_H_
#define _SERVERDACREATOR_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDACreator
//-----------------------------------------------------------------------------

class ServerDACreator : public SODaSCreator
{
public:
	virtual SODaSServer* createServer(IN BOOL publiServerDAGroups, IN BYTE serverInstance);
	virtual SODaSGroup* createGroup(IN SODaSServer* parent);
	virtual SODaSItem* createItem(IN SODaSGroup* parent, IN SODaSItemTag* tag);
	virtual SODaSTag* createTag(IN LPCTSTR qualItemID);
	virtual SODaSNode* createNode(IN LPCTSTR nodePath);
	virtual SODaSProperty* createProperty(IN SOCmnElement* AddressSpaceElement, IN DWORD propertyId,
										  IN LPCTSTR propertyName);
	virtual SODaSTransaction* createTransaction(IN SODaSGroup* group, IN BYTE transactionType,
												IN BYTE connectionType, IN DWORD connection);
	SODaSCache* createCache(IN SODaSItemTag* tag);
}; // ServerDACreator

#pragma pack(pop)
#endif
