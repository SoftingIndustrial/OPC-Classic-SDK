#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "ServerDAGroup.h"
#include "ServerDANode.h"
#include "ServerDATag.h"
#include "ServerDAAddressSpaceElement.h"
#include "SrvApp.h"

#include <limits.h>


//-----------------------------------------------------------------------------
// ServerDAGroup
//-----------------------------------------------------------------------------


ServerDAGroup::ServerDAGroup(void)
{
}

ServerDAGroup::~ServerDAGroup(void)
{
}

void ServerDAGroup::changeItems(void)
{
	ServerApp* pApp = ::getServerApp();

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSChangeItems)
		{
			SOCmnList<SODaSItemTag> itemTagList(pApp->m_itemTagChangedList);
			SOCmnListPosition posI, posIT;
			SODaSItemTag* pItemTag;
			LONG itemCnt = itemTagList.getCount();
			SOCmnList<SOCmnObject> itemList;
			SODaSItem* pItem;
			SOCmnPointer<ServerDAGroup> group;
			OTSItemData* pItemData = NULL;

			if (itemCnt > 0)
			{
				pItemData = (OTSItemData*)malloc(itemCnt * sizeof(OTSItemData));
			}
			else
			{
				return;
			}

			LONG i = 0;
			itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
			posIT = itemTagList.getStartPosition();

			while (posIT)
			{
				pItemTag = itemTagList.getNext(posIT);
				pItemData[i].m_object.m_objectHandle = (DWORD)pItemTag->getOpcHandle();

				if (pItemTag->is(SOCMNOBJECT_TYPE_TAG))
				{
					pItemData[i].m_object.m_userData = ((ServerDATag*)(SODaSItemTag*)pItemTag)->getOTSUserData();
				}
				else
				{
					pItemData[i].m_object.m_userData = ((ServerDANode*)(SODaSItemTag*)pItemTag)->getOTSUserData();
				}

				pItemTag->getAllConnectedObjects(SOCMNOBJECT_TYPE_ITEM, itemList);
				pItemData[i].m_active = FALSE;
				pItemData[i].m_sampleRate = LONG_MAX;
				posI = itemList.getStartPosition();

				while (posI)
				{
					pItem = (SODaSItem*)itemList.getNext(posI);

					if (pItem->isActive())
					{
						group = (ServerDAGroup*)pItem->getParentBranch();
						pItemData[i].m_active = TRUE;

						if (pItemData[i].m_sampleRate > (LONG)group->getUpdateRate())
						{
							pItemData[i].m_sampleRate = (LONG)group->getUpdateRate();
						}
					}
				}

				i++;
			}

			itemTagList.removeAll();
			itemTagList.autoUnlock();
			itemList.destroy();

			if (itemCnt > 0)
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSChangeItems(itemCnt, pItemData);
				}
				SOCMN_CATCH(...)
				{
					_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAGroup::changeItems - Exception occurred in vendor application callback: OTSChangeItems")));
				}				
				free(pItemData);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDAGroup::changeItems - Exception occurred")));
	}
}

void ServerDAGroup::onAddItems(void)
{
	changeItems();
}

void ServerDAGroup::onRemoveItems(void)
{
	changeItems();
}

void ServerDAGroup::onChangeStateItems(void)
{
	changeItems();
}

BOOL ServerDAGroup::setUpdateRate(
	IN DWORD updateRate)    // update rate
{
	if (updateRate > LONG_MAX)
	{
		updateRate = LONG_MAX;
	}

	BOOL ret = SODaSGroup::setUpdateRate(updateRate);
	SOCmnList<SODaSItem> itemList = ((SOCmnList<SODaSItem> *)getLeafList());
	SOCmnListPosition posItem;
	SODaSItem* pItem;
	SOCmnPointer<SODaSItemTag> itemTag;
	posItem = itemList.getStartPosition();

	while (posItem)
	{
		pItem = itemList.getNext(posItem);
		itemTag = pItem->getItemTag();

		if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
		{
			((ServerDATag*)(SODaSItemTag*)itemTag)->connectedObjectChanged(itemTag);
		}

		if (itemTag->is(SOCMNOBJECT_TYPE_NODE))
		{
			((ServerDANode*)(SODaSItemTag*)itemTag)->connectedObjectChanged(itemTag);
		}
	}

	changeItems();
	return ret;
}

#endif // OTB_SERVER_DA
