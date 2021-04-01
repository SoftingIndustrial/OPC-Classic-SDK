#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDACreator.h"
#include "ServerDAServer.h"
#include "ServerDAGroup.h"
#include "ServerDAItem.h"
#include "ServerDATag.h"
#include "ServerDANode.h"
#include "ServerDAProperty.h"
#include "ServerDATransaction.h"
#include "ServerDACache.h"

//-----------------------------------------------------------------------------
// ServerDACreator
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// createServer
//
// - create server object
//
// returns:
//      created object
//
SODaSServer* ServerDACreator::createServer(
	IN BOOL publiServerDAGroups,   // create public groups server object
	IN BYTE serverInstance) // server instance number
{
	ServerDAServer* server;
	SOCMN_ALLOCATE_OBJECT(server, ServerDAServer(publiServerDAGroups, serverInstance))
	return server;
} // createServer

//-----------------------------------------------------------------------------
// createGroup
//
// - create group object
//
// returns:
//		created object
//
SODaSGroup* ServerDACreator::createGroup(
	IN SODaSServer* /* parent */)   // server
{
	SODaSGroup* group;
	SOCMN_ALLOCATE_OBJECT(group, ServerDAGroup())
	return group;
} // createGroup

//-----------------------------------------------------------------------------
// createItem
//
// - create item object
//
// returns:
//		created object
//
SODaSItem* ServerDACreator::createItem(
	IN SODaSGroup* /* parent */,    // group of item
	IN SODaSItemTag* /* tag */) // item tag of item
{
	SODaSItem* item;
	SOCMN_ALLOCATE_OBJECT(item, ServerDAItem())
	return item;
} // createItem

//-----------------------------------------------------------------------------
// createTag
//
// - create tag object
//
// returns:
//		created object
//
SODaSTag* ServerDACreator::createTag(
	IN LPCTSTR /* qualItemID */) // fully qualified id of the tag
{
	ServerDATag* tag;
	SOCMN_ALLOCATE_OBJECT(tag, ServerDATag())
	return tag;
} // createTag


//-----------------------------------------------------------------------------
// createNode
//
// - create node object
//
// returns:
//		created object
//
SODaSNode* ServerDACreator::createNode(
	IN LPCTSTR /* qualItemID */) // fully qualified id of the node
{
	ServerDANode* node;
	SOCMN_ALLOCATE_OBJECT(node, ServerDANode())
	return node;
} // createNode


//-----------------------------------------------------------------------------
//  createProperty
//
// - create property object
//
// returns:
//		created object
//
SODaSProperty* ServerDACreator::createProperty(
	IN SOCmnElement* /* addressSpaceElement */,  // namespace element to create property for
	IN DWORD propertyId,                         // property id
	IN LPCTSTR /* propertyName */)               // property id string
{
	ServerDAProperty* prop;
	SOCMN_ALLOCATE_OBJECT(prop, ServerDAProperty(propertyId))
	return prop;
} // createProperty


//-----------------------------------------------------------------------------
//  createTransaction
//
// - create transaction object
//
// returns:
//		created object
//
SODaSTransaction* ServerDACreator::createTransaction(
	IN SODaSGroup* pGroup,   // group object
	IN BYTE transactionType, // type of transaction
	IN BYTE connectionType,  // connection type
	IN DWORD connection)     // connection id
{
	ServerDATransaction* tac;
	SOCMN_ALLOCATE_OBJECT(tac, ServerDATransaction(pGroup, transactionType, connectionType, connection))
	return tac;
} // createTransaction

//-----------------------------------------------------------------------------
// createCache
//
// - create cache object
//
// returns:
//		created object
//
SODaSCache* ServerDACreator::createCache(
	IN SODaSItemTag* itemTag)   // tag object of cache
{
	SODaSCache* cache;
	BOOL ownCache = FALSE;

	if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
	{
		ownCache = ((ServerDATag*)itemTag)->getOwnCache();
	}
	else if (itemTag->is(SOCMNOBJECT_TYPE_NODE))
	{
		ownCache = ((ServerDANode*)itemTag)->getOwnCache();
	}

	if (!ownCache)
	{
		SOCMN_ALLOCATE_OBJECT(cache, SODaSCache())
	}
	else
	{
		SOCMN_ALLOCATE_OBJECT(cache, ServerDACache())
	}

	return cache;
} // createCache

#endif // OTB_SERVER_DA
