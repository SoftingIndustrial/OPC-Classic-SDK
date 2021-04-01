#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnList.h>


//-----------------------------------------------------------------------------
// SOCmnListConfig                                                            |
//-----------------------------------------------------------------------------

SOCmnListConfig::SOCmnListConfig(void)
{
	m_type = SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_OBJ_DWORD | SOCMNLIST_TYPE_KEY_NO;
	m_hashTableSize = 17;
	m_entriesPerBlock = 10;
}


//-----------------------------------------------------------------------------
// SOCmnListEntry                                                             |
//-----------------------------------------------------------------------------

SOCmnListEntry::SOCmnListEntry(void)
{
	m_object = NULL;
	m_next = NULL;
}

SOCmnListEntry::~SOCmnListEntry(void)
{
}


//-----------------------------------------------------------------------------
// SOCmnListEntryWithKey                                                      |
//-----------------------------------------------------------------------------

SOCmnListEntryWithKey::SOCmnListEntryWithKey(void)
{
	m_key = NULL;
}

SOCmnListEntryWithKey::~SOCmnListEntryWithKey(void)
{
}


//-----------------------------------------------------------------------------
// SOCmnListInfoSimpleList                                                    |
//-----------------------------------------------------------------------------

SOCmnListInfoSimpleList::SOCmnListInfoSimpleList(void)
{
	m_start = NULL;
	m_end = NULL;
}

SOCmnListInfoSimpleList::~SOCmnListInfoSimpleList(void)
{
}


//-----------------------------------------------------------------------------
// SOCmnListInfoFullList                                                      |
//-----------------------------------------------------------------------------

SOCmnListInfoFullList::SOCmnListInfoFullList(void)
{
	m_sync = NULL;
	m_free = NULL;
	m_blockFirst = NULL;
	m_defaultBlockElements = 10;
	m_count = 0;
	m_flags = 0;
}

SOCmnListInfoFullList::~SOCmnListInfoFullList(void)
{
	SOCMN_FREE_OBJECT(m_sync)
}


//-----------------------------------------------------------------------------
// SOCmnListInfoMap                                                           |
//-----------------------------------------------------------------------------

SOCmnListInfoMap::SOCmnListInfoMap(void)
{
	m_sync = NULL;
	m_free = NULL;
	m_blockFirst = NULL;
	m_defaultBlockElements = 10;
	m_count = 0;
	m_hashTableSize = 17;
	m_hashTable = NULL;
	m_flags = 0;
}

SOCmnListInfoMap::~SOCmnListInfoMap(void)
{
	SOCMN_FREE_OBJECT(m_sync)
}


//-----------------------------------------------------------------------------
// SOCmnListHeader                                                            |
//-----------------------------------------------------------------------------

#ifdef _DEBUG
#define CHECK_INIT checkInit();
#else
#define CHECK_INIT
#endif

BOOL SOCmnListHeader::s_skipRehashing = FALSE;

SOCmnListHeader::SOCmnListHeader(void)
{
	m_type = SOCMNLIST_TYPE_NOTINIT;
	m_state = SOCMNLIST_STATE_AUTO_UNLOCKED;
	m_info = NULL;
	m_rehashCount = 0;
}

SOCmnListHeader::~SOCmnListHeader(void)
{
	if (isInit())
	{
		autoUnlock();

		if ((m_state & SOCMNLIST_STATE_CREATED) == SOCMNLIST_STATE_CREATED)
		{
			destroy();
		}
	}
}

void SOCmnListHeader::ensureType(IN WORD type)
{
	if ((m_type & type) != type)
	{
		m_type |= type;

		if (isInit())
		{
			SOCmnListConfig cfg;
			cfg.m_type = m_type;
			destroy();
			create(&cfg);
		}
	}
}

SOCmnListHeader& SOCmnListHeader::operator=(IN const SOCmnListHeader& list)
{
	if (isInit())
	{
		autoUnlock();

		if ((m_state & SOCMNLIST_STATE_CREATED) == SOCMNLIST_STATE_CREATED)
		{
			destroy();
		}
	}

	m_type = list.m_type;
	m_info = list.m_info;
	m_state = SOCMNLIST_STATE_AUTO_UNLOCKED;
	m_rehashCount = list.m_rehashCount;
	return *this;
}

void SOCmnListHeader::detach(void)
{
	if (isInit())
	{
		autoUnlock();
		m_type = SOCMNLIST_TYPE_NOTINIT;
		m_state = SOCMNLIST_STATE_AUTO_UNLOCKED;
		m_info = NULL;
	}
}

void SOCmnListHeader::checkInit(void)
const
{
	_ASSERTION(m_type != SOCMNLIST_TYPE_NOTINIT, "Not initialized");
	_ASSERTION(m_info != NULL, "Not initialized");
}

BOOL SOCmnListHeader::createBlock(DWORD blockElements)
{
	SOCmnListEntry* entry;
	DWORD entrySize;
	DWORD size;
	SOCmnListBlock* block;
	DWORD i;

	if (blockElements == 0)
	{
		return FALSE;
	}

	if ((m_type & SOCMNLIST_TYPE_BLOCKED) == 0)
	{
		return FALSE;    // no blocked list
	}

	// allocate block
	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
	{
		entrySize = sizeof(SOCmnListEntry);
	}
	else
	{
		entrySize = sizeof(SOCmnListEntryWithKey);
	}

	size = sizeof(SOCmnListBlock) + (entrySize * blockElements);
	block = (SOCmnListBlock*)malloc(size);
	memset(block, 0, size);
	block->m_count = blockElements;

	// create sequential list in block
	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
	{
		entry = (SOCmnListEntry*)(block + 1);

		for (i = 0; i < blockElements - 1; i++)
		{
			entry->m_next = entry + 1;
			entry++;
		}
	}
	else
	{
		SOCmnListEntryWithKey* entryWK = (SOCmnListEntryWithKey*)(block + 1);

		for (i = 0; i < blockElements - 1; i++)
		{
			entryWK->m_next = entryWK + 1;
			entryWK++;
		}

		entry = (SOCmnListEntry*)entryWK;
	}

	if (isList())
	{
		// full list
		SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
		SOCmnListEntry* firstFreeEntry = (SOCmnListEntry*)(block + 1);
		// add block to the head of the block list
		block->m_next = infoF->m_blockFirst;
		infoF->m_blockFirst = block;
		// add the block entries to the head of the free list
		entry->m_next = (SOCmnListEntry*)infoF->m_free;
		infoF->m_free = firstFreeEntry;
	}
	else
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		SOCmnListEntry* firstFreeEntry = (SOCmnListEntry*)(block + 1);
		// add block to the head of the block list
		block->m_next = infoM->m_blockFirst;
		infoM->m_blockFirst = block;
		// add the block entries to the head of the free list
		entry->m_next = (SOCmnListEntry*)infoM->m_free;
		infoM->m_free = firstFreeEntry;
	}

	return TRUE;
}

void SOCmnListHeader::freeUnusedBlocks(void)
{
	/*
	    CHECK_INIT
	    autoLock();

	    if ((m_type & SOCMNLIST_TYPE_BLOCKED) == 0)
	        return; // no blocked list

	    if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) != SOCMNLIST_TYPE_OBJ_OBJECT)
	        return; // this optimization is only implemented for object lists

	    SOCmnListBlock *block = NULL;
	    if (isList())
	    { // full list
	        SOCmnListInfoFullList *infoF = (SOCmnListInfoFullList *)m_info;
	        block = infoF->m_blockFirst;
	    }
	    else
	    { // map
	        SOCmnListInfoMap *infoM = (SOCmnListInfoMap *)m_info;
	        block = infoM->m_blockFirst;
	    }

	    if (block)
	    {
	        SOCmnListEntry *cur;
	        DWORD cnt;
	        BOOL unusedBlock;
	        SOCmnListBlock *blockPrev = NULL;
	        while (block)
	        { // all blocks
	            DWORD entrySize;
	            if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
	                entrySize = sizeof(SOCmnListEntry);
	            else
	                entrySize = sizeof(SOCmnListEntryWithKey);

	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ALL, _T("SOCmnList::freeUnusedBlocks"), (_T("block: 0x%X  count: %d"), block, block->m_count));

	            cur = (SOCmnListEntry *)(block+1);
	            cnt = 0;
	            unusedBlock = TRUE;
	            while ((cur) && (cnt < (DWORD)block->m_count))
	            {
	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ALL, _T("SOCmnList::freeUnusedBlocks"), (_T("entry: 0x%X  object: 0x%X"), cur, cur->m_object));
	                if (cur->m_object != NULL)
	                {
	                    unusedBlock = FALSE;
	                    break;
	                }
	                cur = (SOCmnListEntry *)(((BYTE *)cur) + entrySize);
	                cnt++;
	            }
	            if (unusedBlock)
	            {
	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ALL, _T("SOCmnList::freeUnusedBlocks"), (_T("unusedBlock")));
	                // remove block from block list
	                if (blockPrev)
	                    blockPrev->m_next = block->m_next;
	                else
	                { // first block
	                    if (isList())
	                    { // full list
	                        SOCmnListInfoFullList *infoF = (SOCmnListInfoFullList *)m_info;
	                        infoF->m_blockFirst = block->m_next;
	                    }
	                    else
	                    { // map
	                        SOCmnListInfoMap *infoM = (SOCmnListInfoMap *)m_info;
	                        infoM->m_blockFirst = block->m_next;
	                    }
	                }
	                // remove block's entries from the free entry list
	                SOCmnListEntry *curPrev = NULL;
	                DWORD blockSize;
	                blockSize = sizeof(SOCmnListBlock) + (entrySize * block->m_count);
	                if (isList())
	                { // full list
	                    SOCmnListInfoFullList *infoF = (SOCmnListInfoFullList *)m_info;
	                    cur = (SOCmnListEntry *)infoF->m_free;
	                }
	                else
	                { // map
	                    SOCmnListInfoMap *infoM = (SOCmnListInfoMap *)m_info;
	                    cur = (SOCmnListEntry *)infoM->m_free;
	                }
	                cnt = 0;
	                while ((cur) && (cnt < (DWORD)block->m_count))
	                {
	                    if (((void *)cur > (void *)block) && ((void *)cur < (void *)(((BYTE *)block) + blockSize)))
	                    { // entry of this block
	                        // remove the entry of the free list
	_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_ALL, _T("SOCmnList::freeUnusedBlocks"), (_T("remove 0x%X  cnt:%d"), cur, cnt));
	                        if (curPrev)
	                            curPrev->m_next = cur->m_next;
	                        else
	                        {
	                            if (isList())
	                            { // full list
	                                SOCmnListInfoFullList *infoF = (SOCmnListInfoFullList *)m_info;
	                                infoF->m_free = cur->m_next;
	                            }
	                            else
	                            { // map
	                                SOCmnListInfoMap *infoM = (SOCmnListInfoMap *)m_info;
	                                infoM->m_free = cur->m_next;
	                            }
	                        }
	                        cnt++;
	                    }
	                    else
	                        curPrev = cur;

	                    cur = cur->m_next;
	                }

	                // free block
	                void *blkFree;
	                blkFree = (void *)block;
	                block = block->m_next;
	                free(blkFree);
	            }
	            else
	            {
	                blockPrev = block;
	                block = block->m_next;
	            }
	        }
	    }
	*/
}

DWORD SOCmnListHeader::getAvailableAllocatedEntries(void)
{
	CHECK_INIT
	autoLock();

	if ((m_type & SOCMNLIST_TYPE_BLOCKED) == 0)
	{
		return 0;    // no blocked list
	}

	SOCmnListEntry* cur = NULL;

	if (isList())
	{
		// full list
		SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
		cur = (SOCmnListEntry*)infoF->m_free;
	}
	else
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		cur = (SOCmnListEntry*)infoM->m_free;
	}

	if (cur != NULL)
	{
		DWORD cnt = 0;

		while (cur)
		{
			cur = cur->m_next;
			cnt++;
		}

		return cnt;
	}
	else
	{
		return 0;
	}
}

SOCmnListEntry* SOCmnListHeader::allocateEntry(void)
{
	SOCmnListEntry* entry = NULL;

	if ((m_type & SOCMNLIST_TYPE_BLOCKED) == SOCMNLIST_TYPE_BLOCKED)
	{
		if (isList())
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;

			if (!infoF->m_free)
			{
				createBlock(infoF->m_defaultBlockElements);
			}

			entry = (SOCmnListEntry*)infoF->m_free;
			infoF->m_free = (void*)entry->m_next;
			entry->m_next = NULL;
		}
		else
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;

			if (!infoM->m_free)
			{
				createBlock(infoM->m_defaultBlockElements);
			}

			entry = (SOCmnListEntry*)infoM->m_free;
			infoM->m_free = (void*)entry->m_next;
			entry->m_next = NULL;
		}
	}
	else
	{
		if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
		{
			SOCMN_ALLOCATE_OBJECT(entry, SOCmnListEntry)
		}
		else
		{
			SOCmnListEntryWithKey* entryWK = NULL;
			SOCMN_ALLOCATE_OBJECT(entryWK, SOCmnListEntryWithKey)
			entry = entryWK;
		}
	}

	return entry;
}

SOCmnListEntry* SOCmnListHeader::createEntry(
	IN void* obj,
	IN void* key,
	IN WORD keyType)
{
	SOCmnListEntry* entry;
	SOCmnListEntryWithKey* entryWK;
	entry = allocateEntry();

	if (!entry)
	{
		return NULL;
	}

	entryWK = (SOCmnListEntryWithKey*)entry;

	if ((keyType == SOCMNLIST_TYPE_KEY_DWORD) && ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_DWORD))
	{
		entryWK->m_key = key;
	}
	else if ((keyType == SOCMNLIST_TYPE_KEY_STRING) && ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_STRING))
	{
		SOCMN_ALLOCATE_OBJECT_C(entryWK->m_key, SOCmnString((LPCTSTR)key), (void*))
	}

	entry->m_object = obj;

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
	{
		((SOCmnObject*)entry->m_object)->addRef();
	}
	else if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_STRING)
	{
		SOCMN_ALLOCATE_OBJECT_C(entry->m_object, SOCmnString((SOCmnString*)obj), (void*))
	}
#ifdef SOFEATURE_CMN_VARIANT
	else if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VARIANT)
	{
		SOCMN_ALLOCATE_OBJECT_C(entry->m_object, SOCmnVariant((SOCmnVariant*)obj), (void*))
	}
#endif
#ifdef SOOS_WINDOWS
	else if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_INTERFACE)
	{
		((IUnknown*)entry->m_object)->AddRef();
	}
#endif // SOOS_WINDOWS
	else if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VOID)
	{
#ifdef SOOS_WINDOWS
		DWORD size = (DWORD)_msize(obj);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
		DWORD size = malloc_usable_size(obj);
#endif
		entry->m_object = malloc(size);
		memcpy(entry->m_object, obj, size);
	}

	return entry;
}

SOCmnListEntry* SOCmnListHeader::cloneEntry(
	IN SOCmnListEntry* entryS)
{
	SOCmnListEntry* entry;
	SOCmnListEntryWithKey* entryWK;
	SOCmnListEntryWithKey* entryWKS = (SOCmnListEntryWithKey*)entryS;
	entry = allocateEntry();
	entryWK = (SOCmnListEntryWithKey*)entry;

	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_DWORD)
	{
		entryWK->m_key = entryWKS->m_key;
	}
	else if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_STRING)
		SOCMN_ALLOCATE_OBJECT_C(entryWK->m_key, SOCmnString((LPCTSTR)entryWKS->m_key), (void*))
		entry->m_object = entryS->m_object;

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_STRING)
		SOCMN_ALLOCATE_OBJECT_C(entry->m_object, SOCmnString((SOCmnString*)entryS->m_object), (void*))
		if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
		{
			((SOCmnObject*)entry->m_object)->addRef();
		}

#ifdef SOOS_WINDOWS

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_INTERFACE)
	{
		((IUnknown*)entry->m_object)->AddRef();
	}

#endif // SOOS_WINDOWS
#ifdef SOFEATURE_CMN_VARIANT

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VARIANT)
		SOCMN_ALLOCATE_OBJECT_C(entry->m_object, SOCmnVariant((VARIANT*)entryS->m_object), (void*))
#endif
		if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VOID)
		{
#ifdef SOOS_WINDOWS
			DWORD size = (DWORD)_msize(entryS->m_object);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
			DWORD size = malloc_usable_size(entryS->m_object);
#endif
			entry->m_object = malloc(size);
			memcpy(entry->m_object, entryS->m_object, size);
		}

	return entry;
}

void SOCmnListHeader::destroyEntry(
	IN SOCmnListEntry* entry)
{
	if (!entry)
	{
		return;
	}

	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_STRING)
	{
		SOCmnListEntryWithKey* entryWK = (SOCmnListEntryWithKey*)entry;
		SOCMN_FREE_OBJECT_C(entryWK->m_key, (SOCmnString*))
	}

	SOCMN_TRY
	{
		if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
		{
			((SOCmnObject*)entry->m_object)->release();
		}
		else if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_STRING)
		{
			SOCMN_FREE_OBJECT_C(entry->m_object, (SOCmnString*))
		}

#ifdef SOFEATURE_CMN_VARIANT
		else if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VARIANT)
		{
			SOCMN_FREE_OBJECT_C(entry->m_object, (SOCmnVariant*))
		}
#endif

		else if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_DATA)
		{
			delete((SOCmnData*)entry->m_object);
		}

		else if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VOID)
		{
			free(entry->m_object);
		}

#ifdef SOOS_WINDOWS
		else if ((m_type& SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_INTERFACE)
		{
			((IUnknown*)entry->m_object)->Release();
		}

#endif // SOOS_WINDOWS
	}
	SOCMN_CATCH(...)
	{
	}
	entry->m_object = NULL;

	if ((m_type & SOCMNLIST_TYPE_BLOCKED) == SOCMNLIST_TYPE_BLOCKED)
	{
		if (isList())
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			entry->m_next = (SOCmnListEntry*)infoF->m_free;
			infoF->m_free = entry;
		}
		else
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			entry->m_next = (SOCmnListEntry*)infoM->m_free;
			infoM->m_free = entry;
		}
	}
	else
	{
		SOCMN_FREE_OBJECT(entry);
	}
}


BOOL SOCmnListHeader::create(
	IN OPTIONAL SOCmnListConfig* creationConfig)
{
	if (isInit())
	{
		return FALSE;
	}

	if (isList())
	{
		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoFullList*));
		}
		else
		{
			SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoSimpleList*));
		}
	}
	else if (isMap())
	{
		SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoMap*));
	}
	else
	{
		// something went wrong and the old type can not determined
		SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoFullList*));
		SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoSimpleList*));
		SOCMN_FREE_OBJECT_C(m_info, (SOCmnListInfoSimpleList*));
	}

	WORD hashTableSize = 17;
	BYTE blockElements = 10;
	BOOL ok = TRUE;

	if (creationConfig)
	{
		m_type = creationConfig->m_type;

		if (creationConfig->m_hashTableSize > 0)
		{
			hashTableSize = creationConfig->m_hashTableSize;
		}

		if (creationConfig->m_entriesPerBlock > 0)
		{
			blockElements = creationConfig->m_entriesPerBlock;
		}
	}

	// create info
	if (isList())
	{
		SOCmnListInfoSimpleList* infoS;

		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCmnListInfoFullList* infoF;
			SOCMN_ALLOCATE_OBJECT_C(m_info, SOCmnListInfoFullList, (void*))
			infoF = (SOCmnListInfoFullList*)m_info;
			infoF->m_count = 0;

			if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
			{
				// synchronisation
				SOCMN_ALLOCATE_OBJECT(infoF->m_sync, SOCmnSync())
				ok &= (infoF->m_sync != NULL);
			}

			infoF->m_defaultBlockElements = blockElements;
		}
		else
		{
			SOCMN_ALLOCATE_OBJECT_C(m_info, SOCmnListInfoSimpleList, (void*))
		}
		
		infoS = (SOCmnListInfoSimpleList*)m_info;
		infoS->m_start = NULL;
		infoS->m_end = NULL;
	}

	if (isMap())
	{
		SOCmnListInfoMap* infoM;
		SOCMN_ALLOCATE_OBJECT(infoM, SOCmnListInfoMap)
		infoM->m_count = 0;

		if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
		{
			// synchronisation
			SOCMN_ALLOCATE_OBJECT(infoM->m_sync, SOCmnSync())
			ok &= (infoM->m_sync != NULL);
		}

		m_type |= SOCMNLIST_TYPE_BLOCKED;
		infoM->m_defaultBlockElements = blockElements;
		infoM->m_hashTableSize = hashTableSize;
		// create hash table start array
		DWORD size;
		size = sizeof(SOCmnListInfoSimpleList)  * infoM->m_hashTableSize;
		infoM->m_hashTable = malloc(size);
		memset(infoM->m_hashTable, 0, size);
		m_info = (void*)infoM;
	}

	m_state = (m_state & ~SOCMNLIST_STATE_AUTO_MASK) | SOCMNLIST_STATE_AUTO_NOTLOCK;
	m_state |= SOCMNLIST_STATE_CREATED;
	return ok;
}

BOOL SOCmnListHeader::setDefaultBlockElements(BYTE blockElements)
{
	if (((m_type & SOCMNLIST_TYPE_BLOCKED) == SOCMNLIST_TYPE_BLOCKED) ||
		(isMap()))
	{
		if (blockElements > 0)
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			infoF->m_defaultBlockElements = blockElements;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SOCmnListHeader::clone(
	IN SOCmnListHeader& list)
{
	SOCmnListConfig cfg;

	if (!list.isInit())
	{
		return FALSE;
	}

	// no cloning of data lists
	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_DATA)
	{
		return FALSE;
	}

	list.autoLock();
	cfg.m_type = list.m_type;

	if ((list.isList()) && ((list.m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0))
	{
		SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)list.m_info;
		cfg.m_entriesPerBlock = infoF->m_defaultBlockElements;
	}

	if (list.isMap())
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)list.m_info;
		cfg.m_hashTableSize = infoM->m_hashTableSize;
		cfg.m_entriesPerBlock = infoM->m_defaultBlockElements;
	}

	destroy();
	create(&cfg);
	SOCmnListInfoSimpleList* infoSD;
	SOCmnListInfoSimpleList* infoSS;
	SOCmnListEntry* curS;
	SOCmnListEntry* curD;
	BOOL loop = list.isMap();
	DWORD hash = 0;
	WORD cnt = 0;

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoMS = (SOCmnListInfoMap*)list.m_info;
			infoSS = &(((SOCmnListInfoSimpleList*)infoMS->m_hashTable)[hash]);
			SOCmnListInfoMap* infoMD = (SOCmnListInfoMap*)m_info;
			infoSD = &(((SOCmnListInfoSimpleList*)infoMD->m_hashTable)[hash]);
			hash++;

			if (hash == infoMD->m_hashTableSize)
			{
				loop = FALSE;
			}
		}
		else
		{
			infoSS = (SOCmnListInfoSimpleList*)list.m_info;
			infoSD = (SOCmnListInfoSimpleList*)m_info;
		}

		curS = (SOCmnListEntry*)infoSS->m_start;
		curD = NULL;

		// first entry
		if (curS)
		{
			infoSD->m_start = cloneEntry(curS);
			curS = (SOCmnListEntry*)curS->m_next;
			curD = (SOCmnListEntry*)infoSD->m_start;
			cnt++;
		}

		// other entries
		while (curS)
		{
			curD->m_next = cloneEntry(curS);
			curS = (SOCmnListEntry*)curS->m_next;
			curD = (SOCmnListEntry*)curD->m_next;
			cnt++;
		}

		if (curD)
		{
			infoSD->m_end = (void*)curD;
		}
	}
	while (loop);

	if ((isList()) && ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0))
	{
		SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;

		if (cnt < 0xFFFF)
		{
			infoF->m_count = cnt;
		}
		else
		{
			infoF->m_count = 0xFFFF;
		}
	}

	if (isMap())
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;

		if (cnt < 0xFFFF)
		{
			infoM->m_count = cnt;
		}
		else
		{
			infoM->m_count = 0xFFFF;
		}
	}

	setDirty(TRUE);
	return TRUE;
}

void SOCmnListHeader::destroy(void)
{
	if (!isInit())
	{
		return;
	}

	lock();
	removeAll();
	unlock();
	autoUnlock();

	if (isList())
	{
		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			infoF->m_count = 0;

			if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
			{
				SOCMN_FREE_OBJECT(infoF->m_sync)
			}

			SOCMN_FREE_OBJECT(infoF)
		}
		else
		{
			SOCmnListInfoSimpleList* infoS = (SOCmnListInfoSimpleList*)m_info;
			SOCMN_FREE_OBJECT(infoS)
		}
	}

	if (isMap())
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		infoM->m_count = 0;

		if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
		{
			SOCMN_FREE_OBJECT(infoM->m_sync)
		}

		if (infoM->m_hashTable)
		{
			free(infoM->m_hashTable);
		}

		SOCMN_FREE_OBJECT(infoM)
	}

	m_type = SOCMNLIST_TYPE_NOTINIT;
	m_info = NULL;
}


SOCmnListPosition SOCmnListHeader::addMemory(
	IN void* memory,
	IN DWORD len,
	IN void* key,
	IN WORD keyType)
{
	SOCmnListPosition pos;
	void* obj;

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) != SOCMNLIST_TYPE_OBJ_VOID)
	{
		return NULL;
	}

	obj = malloc(len);
	memcpy(obj, memory, len);
	pos = add(obj, key, keyType);
	free(obj);
	setDirty(TRUE);
	return pos;
}

SOCmnListPosition SOCmnListHeader::add(
	IN void* obj,
	IN void* key,
	IN WORD keyType)
{
	SOCmnListPosition pos;
	SOCmnListInfoSimpleList* infoS;
	SOCmnString objStrKey;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		WORD hash;
		ptrdiff_t objDWKey = 0;
		WORD hashKeyType = keyType;

		if (((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO) &&
			((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT))
		{
			void** locKey;
			hashKeyType = ((m_type & SOCMNLIST_TYPE_OBJECT_KEY_STRING) == SOCMNLIST_TYPE_OBJECT_KEY_STRING)
						  ? SOCMNLIST_TYPE_KEY_STRING : SOCMNLIST_TYPE_KEY_DWORD;

			if (hashKeyType == SOCMNLIST_TYPE_KEY_STRING)
			{
				locKey = (void**)&objStrKey;
			}
			else
			{
				locKey = (void**)&objDWKey;
			}

			getObjectKey(obj, hashKeyType, locKey);

			if (hashKeyType == SOCMNLIST_TYPE_KEY_STRING)
			{
				key = (void*)((LPCTSTR)objStrKey);
			}
			else
			{
				key = (void*)objDWKey;
			}
		}

		hash = hashKey(key, hashKeyType);
		hash = hash % infoM->m_hashTableSize;
		infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	if (infoS->m_end)
	{
		SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_end;
		cur->m_next = createEntry(obj, key, keyType);
		infoS->m_end = cur->m_next;
		pos = (SOCmnListPosition)cur->m_next;
	}
	else
	{
		// first list entry
		infoS->m_start = createEntry(obj, key, keyType);
		infoS->m_end = infoS->m_start;
		pos = (SOCmnListPosition)infoS->m_start;
	}

	updateCount(TRUE);
	setDirty(TRUE);
	return pos;
}

SOCmnListPosition SOCmnListHeader::push(
	IN void* obj)
{
	SOCmnListInfoSimpleList* infoS;
	SOCmnListPosition pos;

	if (((m_type & SOCMNLIST_TYPE_KEY_MASK) != SOCMNLIST_TYPE_KEY_NO) ||
		((m_type & SOCMNLIST_TYPE_ORG_MASK) != SOCMNLIST_TYPE_ORG_LIST))
	{
		return NULL;
	}

	CHECK_INIT
	autoLock();
	// list
	infoS = (SOCmnListInfoSimpleList*)m_info;
	SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;
	infoS->m_start = createEntry(obj, NULL, SOCMNLIST_TYPE_KEY_NO);
	infoS->m_startEntry->m_next = cur;

	if (infoS->m_end == NULL)
	{
		infoS->m_end = infoS->m_start;
	}

	pos = (SOCmnListPosition)infoS->m_start;
	updateCount(TRUE);
	setDirty(TRUE);
	return pos;
}


SOCmnListPosition SOCmnListHeader::insertAfter(
	IN SOCmnListPosition pos,
	IN void* obj,
	IN void* key,
	IN WORD keyType)
{
	SOCmnListInfoSimpleList* infoS;

	if ((m_type & SOCMNLIST_TYPE_ORG_MASK) != SOCMNLIST_TYPE_ORG_LIST)
	{
		return NULL;
	}

	CHECK_INIT
	autoLock();
	// list
	infoS = (SOCmnListInfoSimpleList*)m_info;
	SOCmnListEntry* newEntry;
	SOCmnListEntry* prev;

	if (pos != NULL)
	{
		prev = (SOCmnListEntry*)pos;
	}
	else
	{
		prev = NULL;
	}

	newEntry = createEntry(obj, key, keyType);

	if (prev)
	{
		newEntry->m_next = prev->m_next;
		prev->m_next = newEntry;
	}
	else
	{
		newEntry->m_next = infoS->m_startEntry;
		infoS->m_start = newEntry;
	}

	if (newEntry->m_next == NULL)
	{
		infoS->m_end = newEntry;
	}

	pos = (SOCmnListPosition)newEntry;
	updateCount(TRUE);
	setDirty(TRUE);
	return pos;
}


SOCmnListPosition SOCmnListHeader::update(
	IN void* obj,
	IN void* key,
	IN WORD keyType)
{
	SOCmnListInfoSimpleList* infoS;
	SOCmnListPosition pos = NULL;
	ptrdiff_t objDWKey = 0;
	SOCmnString objStrKey;
	CHECK_INIT

	if (((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO) &&
		((m_type & SOCMNLIST_TYPE_OBJ_MASK) != SOCMNLIST_TYPE_OBJ_OBJECT))
	{
		return NULL;
	}

	if (((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO) &&
		((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT))
	{
		void** locKey;
		keyType = ((m_type & SOCMNLIST_TYPE_OBJECT_KEY_STRING) == SOCMNLIST_TYPE_OBJECT_KEY_STRING)
				  ? SOCMNLIST_TYPE_KEY_STRING : SOCMNLIST_TYPE_KEY_DWORD;

		if (keyType == SOCMNLIST_TYPE_KEY_STRING)
		{
			locKey = (void**)&objStrKey;
		}
		else
		{
			locKey = (void**)&objDWKey;
		}

		getObjectKey(obj, keyType, locKey);

		if (keyType == SOCMNLIST_TYPE_KEY_STRING)
		{
			key = (void*)((LPCTSTR)objStrKey);
		}
		else
		{
			key = (void*)objDWKey;
		}
	}

	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		WORD hash;
		hash = hashKey(key, keyType);
		hash = hash % infoM->m_hashTableSize;
		infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;

	while (cur)
	{
		if (hasKey(cur, key, keyType))
		{
			pos = (SOCmnListPosition)cur;
#ifdef SOFEATURE_CMN_VARIANT

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VARIANT)
			{
				*((SOCmnVariant*)cur->m_object) = (SOCmnVariant*)obj;
			}

#endif

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_DWORD)
			{
				cur->m_object = obj;
			}

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_STRING)
			{
				*((SOCmnString*)cur->m_object) = *((SOCmnString*)obj);
			}

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_VOID)
			{
#ifdef SOOS_WINDOWS
				DWORD size = (DWORD)_msize(obj);
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX
				DWORD size = malloc_usable_size(obj);
#endif
				free(cur->m_object);
				cur->m_object = malloc(size);
				memcpy(cur->m_object, obj, size);
			}

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
			{
				((SOCmnObject*)cur->m_object)->release();
				cur->m_object = obj;
				((SOCmnObject*)cur->m_object)->addRef();
			}

#ifdef SOOS_WINDOWS

			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_INTERFACE)
			{
				((IUnknown*)cur->m_object)->Release();
				cur->m_object = obj;
				((IUnknown*)cur->m_object)->AddRef();
			}

#endif // SOOS_WINDOWS
			break;
		}
		else
		{
			cur = cur->m_next;
		}
	}

	if (!pos)
	{
		pos = add(obj, key, keyType);
	}

	setDirty(TRUE);
	return pos;
}

BOOL SOCmnListHeader::removeObject(
	IN void* obj)
{
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	SOCmnListEntry* cur;
	SOCmnListEntry* prev;
	BOOL removed = FALSE;
	BOOL loop = FALSE;
	WORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		ptrdiff_t dwKey;
		SOCmnString strKey;
		WORD keyType;
		keyType = ((m_type & SOCMNLIST_TYPE_OBJECT_KEY_STRING) == SOCMNLIST_TYPE_OBJECT_KEY_STRING)
				  ? SOCMNLIST_TYPE_KEY_STRING : SOCMNLIST_TYPE_KEY_DWORD;

		if (keyType == SOCMNLIST_TYPE_KEY_DWORD)
		{
			if (getObjectKey(obj, keyType, (void**)&dwKey))
			{
				hash = hashKey((void*)dwKey, keyType);
				hash = hash % infoM->m_hashTableSize;
				infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			}
			else
			{
				loop = TRUE;
			}
		}
		else
		{
			if (getObjectKey(obj, keyType, (void**)&strKey))
			{
				hash = hashKey((void*)(LPCTSTR)strKey, keyType);
				hash = hash % infoM->m_hashTableSize;
				infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			}
			else
			{
				loop = TRUE;
			}
		}
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}

		cur = (SOCmnListEntry*)infoS->m_start;
		prev = NULL;

		while (cur)
		{
			if (hasObject(cur, obj))
			{
				if (cur == infoS->m_start)
				{
					infoS->m_start = cur->m_next;
				}
				else
				{
					prev->m_next = cur->m_next;
				}

				if (cur == infoS->m_end)
				{
					infoS->m_end = prev;
				}

				removed = TRUE;
				destroyEntry(cur);
				break;
			}
			else
			{
				prev = cur;
				cur = cur->m_next;
			}
		}
	}
	while ((!removed) && (loop));

	if (removed)
	{
		updateCount(FALSE);
		setDirty(TRUE);
	}

	return removed;
}

BOOL SOCmnListHeader::removeKey(
	IN void* key,
	IN WORD keyType)
{
	SOCmnListInfoSimpleList* infoS;
	BOOL removed = FALSE;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		WORD hash;
		hash = hashKey(key, keyType);
		hash = hash % infoM->m_hashTableSize;
		infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;
	SOCmnListEntry* prev = NULL;

	while (cur)
	{
		if (hasKey(cur, key, keyType))
		{
			if (cur == infoS->m_start)
			{
				infoS->m_start = cur->m_next;
			}
			else
			{
				prev->m_next = cur->m_next;
			}

			if (cur == infoS->m_end)
			{
				infoS->m_end = prev;
			}

			removed = TRUE;
			destroyEntry(cur);
			break;
		}
		else
		{
			prev = cur;
			cur = cur->m_next;
		}
	}

	if (removed)
	{
		updateCount(FALSE);
		setDirty(TRUE);
	}

	return removed;
}

BOOL SOCmnListHeader::removeAt(
	IN SOCmnListPosition pos)
{
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	SOCmnListEntry* cur;
	SOCmnListEntry* prev;
	BOOL removed = FALSE;
	BOOL loop = TRUE;
	DWORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isList())
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
		loop = FALSE;
	}

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}

		cur = (SOCmnListEntry*)infoS->m_start;
		prev = NULL;

		while (cur)
		{
			if (cur == (void*)pos)
			{
				if (cur == infoS->m_start)
				{
					infoS->m_start = cur->m_next;
				}
				else
				{
					prev->m_next = cur->m_next;
				}

				if (cur == infoS->m_end)
				{
					infoS->m_end = prev;
				}

				removed = TRUE;
				destroyEntry(cur);
				break;
			}
			else
			{
				prev = cur;
				cur = cur->m_next;
			}
		}
	}
	while ((!removed) && (loop));

	if (removed)
	{
		updateCount(FALSE);
		setDirty(TRUE);
	}

	return removed;
}

BOOL SOCmnListHeader::removeAt(
	IN SOCmnListPosition pos,
	IN SOCmnListPosition prevPos)
{
	SOCmnListInfoSimpleList* infoS;
	SOCmnListEntry* cur;
	SOCmnListEntry* prev;
	CHECK_INIT
	autoLock();

	if (isList())
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
		cur = (SOCmnListEntry*)pos;

		if (cur == infoS->m_start)
		{
			infoS->m_start = cur->m_next;
			prev = NULL;
		}
		else
		{
			prev = (SOCmnListEntry*)prevPos;
			prev->m_next = cur->m_next;
		}

		if (cur == infoS->m_end)
		{
			infoS->m_end = prev;
		}

		destroyEntry(cur);
		updateCount(FALSE);
		setDirty(TRUE);
		return TRUE;
	}
	else
	{
		_ASSERTION(TRUE, _T("removeAt(curPos, prevPos) not supported for maps"));
		return FALSE;
	}
}

BOOL SOCmnListHeader::removeHead(void)
{
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	BOOL removed = FALSE;
	BOOL loop = TRUE;
	DWORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isList())
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
		loop = FALSE;
	}

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}

		if (infoS->m_start)
		{
			SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;
			infoS->m_start = cur->m_next;

			if (cur == infoS->m_end)
			{
				infoS->m_end = NULL;
			}

			removed = TRUE;
			destroyEntry(cur);
		}
	}
	while ((!removed) && (loop));

	if (removed)
	{
		updateCount(FALSE);
		setDirty(TRUE);
	}

	return removed;
}

void* SOCmnListHeader::getHead(void)
{
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	void* head = NULL;
	BOOL loop = TRUE;
	DWORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isList())
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
		loop = FALSE;
	}

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}

		if (infoS->m_start)
		{
			SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;
			head = cur->m_object;
		}
	}
	while ((!head) && (loop));

	return head;
}


void* SOCmnListHeader::pop(void)
{
	void* head = NULL;

	if (m_type & (SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_OBJ_DWORD))
	{
		head = getHead();

		if (head)
		{
			if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
			{
				((SOCmnObject*)head)->addRef();
			}

			removeHead();
		}
	}

	return head;
}


DWORD SOCmnListHeader::getCount(void)
{
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	DWORD cnt = 0;
	BOOL done = FALSE;
	BOOL loop = FALSE;
	DWORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;

		if (infoM->m_count != 0xFFFF)
		{
			cnt = infoM->m_count;
			done = TRUE;
		}
		else
		{
			loop = TRUE;
		}
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;

		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;

			if (infoF->m_count != 0xFFFF)
			{
				cnt = infoF->m_count;
				done = TRUE;
			}
		}
	}

	if (!done)
	{
		SOCmnListEntry* cur;

		do
		{
			if (loop)
			{
				SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
				infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
				hash++;

				if (hash == infoM->m_hashTableSize)
				{
					loop = FALSE;
				}
			}

			cur = (SOCmnListEntry*)infoS->m_start;

			while (cur)
			{
				cur = cur->m_next;
				cnt++;
			}
		}
		while (loop);
	}

	return cnt;
}

BOOL SOCmnListHeader::isEmpty(void)
{
	return (NULL == getStartPosition());
}

SOCmnListPosition SOCmnListHeader::getStartPosition(void)
{
	SOCmnListPosition pos;
	SOCmnListInfoSimpleList* infoS;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		DWORD hash = 0;

		do
		{
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			pos = (SOCmnListPosition)infoS->m_start;
			hash++;
		}
		while ((pos == NULL) && (hash != infoM->m_hashTableSize));
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
		pos = (SOCmnListPosition)infoS->m_start;
	}

	return pos;
}

void* SOCmnListHeader::getNext(
	IN OUT SOCmnListPosition& pos,
	IN WORD keyType,
	OUT void** key)
const
{
	if (pos == NULL)
	{
		return NULL;
	}

	void* next;
	SOCmnListEntry* cur = (SOCmnListEntry*)pos;
	next = cur->m_object;
	pos = (SOCmnListPosition)cur->m_next;

	if (key)
	{
		getKey(cur, keyType, key);
	}

	if ((!pos) && (isMap()))
	{
		// end of hash list
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		SOCmnListInfoSimpleList* infoS;
		DWORD hash = 0;

		do
		{
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if ((void*)cur == infoS->m_end)
			{
				// found current list
				// get start of the next not empty list
				while ((pos == NULL) && (hash != infoM->m_hashTableSize))
				{
					infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
					pos = (SOCmnListPosition)infoS->m_start;
					hash++;
				}
			}
		}
		while ((pos == NULL) && (hash != infoM->m_hashTableSize));
	}

	return next;
}

void* SOCmnListHeader::getIndex(
	IN DWORD idx,
	IN WORD keyType,
	OUT void** key)
{
	void* obj = NULL;
	DWORD cnt = 0;
	CHECK_INIT
	autoLock();
	SOCmnListPosition pos = getStartPosition();

	while ((pos) && (cnt <= idx))
	{
		obj = getNext(pos, keyType, key);
		cnt++;
	}

	return (cnt <= idx) ? NULL : obj;
}

void SOCmnListHeader::removeAll(void)
{
	SOCmnListEntry* cur;
	SOCmnListEntry* prev;
	SOCmnListInfoSimpleList* infoS;
	BOOL loop = isMap();
	DWORD hash = 0;
	CHECK_INIT
	autoLock();

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}
		else
		{
			infoS = (SOCmnListInfoSimpleList*)m_info;
		}

		cur = (SOCmnListEntry*)infoS->m_start;
		prev = (SOCmnListEntry*)infoS->m_start;
		infoS->m_start = NULL;
		infoS->m_end = NULL;

		while (cur)
		{
			cur = cur->m_next;
			destroyEntry(prev);
			prev = cur;
		}
	}
	while (loop);

	if ((isList()) && ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0))
	{
		SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
		infoF->m_count = 0;
		infoF->m_free = NULL;

		// free all memory
		if ((m_type & SOCMNLIST_TYPE_BLOCKED) == SOCMNLIST_TYPE_BLOCKED)
		{
			SOCmnListBlock* blk = (SOCmnListBlock*)infoF->m_blockFirst;
			void* blkFree;
			infoF->m_blockFirst = NULL;

			while (blk)
			{
				blkFree = (void*)blk;
				blk = blk->m_next;
				free(blkFree);
			}
		}
	}

	if (isMap())
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		infoM->m_count = 0;
		infoM->m_free = NULL;
		// free all memory
		SOCmnListBlock* blk = (SOCmnListBlock*)infoM->m_blockFirst;
		void* blkFree;
		infoM->m_blockFirst = NULL;

		while (blk)
		{
			blkFree = (void*)blk;
			blk = blk->m_next;
			free(blkFree);
		}
	}

	setDirty(TRUE);
}

SOCmnListPosition SOCmnListHeader::findObject(
	IN void* obj)
{
	SOCmnListPosition foundPos = NULL;
	SOCmnListInfoSimpleList* infoS = NULL; /* fix gcc warning */
	SOCmnListEntry* cur;
	BOOL loop = FALSE;
	WORD hash = 0;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		WORD keyType;
		ptrdiff_t dwKey;
		SOCmnString strKey;
		keyType = ((m_type & SOCMNLIST_TYPE_OBJECT_KEY_STRING) == SOCMNLIST_TYPE_OBJECT_KEY_STRING)
				  ? SOCMNLIST_TYPE_KEY_STRING : SOCMNLIST_TYPE_KEY_DWORD;

		if (keyType == SOCMNLIST_TYPE_KEY_DWORD)
		{
			if (getObjectKey(obj, keyType, (void**)&dwKey))
			{
				hash = hashKey((void*)dwKey, keyType);
				hash = hash % infoM->m_hashTableSize;
				infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			}
			else
			{
				loop = TRUE;
			}
		}
		else
		{
			if (getObjectKey(obj, keyType, (void**)&strKey))
			{
				hash = hashKey((void*)(LPCTSTR)strKey, keyType);
				hash = hash % infoM->m_hashTableSize;
				infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			}
			else
			{
				loop = TRUE;
			}
		}
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	do
	{
		if (loop)
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
			hash++;

			if (hash == infoM->m_hashTableSize)
			{
				loop = FALSE;
			}
		}

		cur = (SOCmnListEntry*)infoS->m_start;

		while (cur)
		{
			if (hasObject(cur, obj))
			{
				foundPos = (SOCmnListPosition)cur;
				break;
			}
			else
			{
				cur = cur->m_next;
			}
		}
	}
	while ((!foundPos) && (loop));

	return foundPos;
}

void* SOCmnListHeader::findKey(
	IN void* key,
	IN WORD keyType,
	OUT BOOL* found)
{
	SOCmnListInfoSimpleList* infoS;
	void* foundObj = NULL;
	BOOL fnd = FALSE;
	CHECK_INIT
	autoLock();

	if (isMap())
	{
		// map
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		WORD hash;
		hash = hashKey(key, keyType);
		hash = hash % infoM->m_hashTableSize;
		infoS = &(((SOCmnListInfoSimpleList*)infoM->m_hashTable)[hash]);
	}
	else
	{
		// list
		infoS = (SOCmnListInfoSimpleList*)m_info;
	}

	SOCmnListEntry* cur = (SOCmnListEntry*)infoS->m_start;

	while (cur)
	{
		if (hasKey(cur, key, keyType))
		{
			foundObj = cur->m_object;
			fnd = TRUE;
			break;
		}
		else
		{
			cur = cur->m_next;
		}
	}

	if (foundObj)
	{
		if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
		{
			((SOCmnObject*)foundObj)->addRef();
		}
	}

	if (found)
	{
		*found = fnd;
	}

	return foundObj;
}

void SOCmnListHeader::updateCount(
	IN BOOL increment)
{
	if (isMap())
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;

		if (infoM->m_count != 0xFFFF)
		{
			if (increment)
			{
				infoM->m_count++;
			}
			else
			{
				infoM->m_count--;
			}
		}
	}
	else
	{
		if ((isList()) && ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0))
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;

			if (infoF->m_count != 0xFFFF)
			{
				if (increment)
				{
					infoF->m_count++;
				}
				else
				{
					infoF->m_count--;
				}
			}
		}
	}
}

BOOL SOCmnListHeader::isDirty(void)
const
{
	if (isList())
	{
		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			return ((infoF->m_flags & SOCMNLIST_FLAG_DIRTY) == SOCMNLIST_FLAG_DIRTY);
		}
	}
	else
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
		return ((infoM->m_flags & SOCMNLIST_FLAG_DIRTY) == SOCMNLIST_FLAG_DIRTY);
	}

	return TRUE;
}

BOOL SOCmnListHeader::setDirty(
	IN BOOL dirty)
{
	if (isList())
	{
		if ((m_type & SOCMNLIST_TYPE_INFO_FULL_LIST) != 0)
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;

			if (dirty)
			{
				infoF->m_flags |= SOCMNLIST_FLAG_DIRTY;
			}
			else
			{
				infoF->m_flags &= ~SOCMNLIST_FLAG_DIRTY;
			}

			return TRUE;
		}
	}
	else
	{
		SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;

		if (dirty)
		{
			infoM->m_flags |= SOCMNLIST_FLAG_DIRTY;
		}
		else
		{
			infoM->m_flags &= ~SOCMNLIST_FLAG_DIRTY;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL SOCmnListHeader::getObjectKey(
	IN void* obj,
	IN WORD keyType,
	OUT void** key)
const
{
	if (!key)
	{
		return FALSE;
	}

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
	{
		if ((keyType == SOCMNLIST_TYPE_KEY_STRING) && (((SOCmnObject*)obj)->hasKeyType(SOCMNOBJECT_KEYTYPE_STRING)))
		{
			((SOCmnObject*)(obj))->getKey(*((SOCmnString*)key));
			return TRUE;
		}
		else
		{
			if ((keyType == SOCMNLIST_TYPE_KEY_DWORD) && (((SOCmnObject*)obj)->hasKeyType(SOCMNOBJECT_KEYTYPE_DWORD)))
			{
				DWORD dwKey;

				if (((SOCmnObject*)obj)->getKey(dwKey))
				{
					*key = (void*)dwKey;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


BOOL SOCmnListHeader::getKey(
	IN SOCmnListEntry* entry,
	IN WORD keyType,
	OUT void** key)
const
{
	if (!key)
	{
		return FALSE;
	}

	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
	{
		return getObjectKey(entry->m_object, keyType, key);
	}
	else
	{
		SOCmnListEntryWithKey* entryWK = (SOCmnListEntryWithKey*)entry;

		if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_DWORD)
		{
			*key = entryWK->m_key;
		}
		else
		{
			*((SOCmnString*)key) = (*((SOCmnString*)entryWK->m_key));
		}

		return TRUE;
	}
}

BOOL SOCmnListHeader::hasKey(
	IN SOCmnListEntry* entry,
	IN void* key,
	IN WORD keyType)
const
{
	BOOL rightKey = FALSE;

	if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
	{
		if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_OBJECT)
			if ((keyType == SOCMNLIST_TYPE_KEY_STRING) && (((SOCmnObject*)entry->m_object)->hasKeyType(SOCMNOBJECT_KEYTYPE_STRING)))
			{
				rightKey = ((SOCmnObject*)entry->m_object)->hasKey((LPCTSTR)key);
			}
			else
			{
				if ((keyType == SOCMNLIST_TYPE_KEY_DWORD) && (((SOCmnObject*)entry->m_object)->hasKeyType(SOCMNOBJECT_KEYTYPE_DWORD)))
				{
					rightKey = ((SOCmnObject*)entry->m_object)->hasKey((DWORD)key);
				}
			}
	}
	else
	{
		SOCmnListEntryWithKey* entryWK = (SOCmnListEntryWithKey*)entry;

		if ((keyType == SOCMNLIST_TYPE_KEY_DWORD) && ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_DWORD))
		{
			rightKey = (entryWK->m_key == key);
		}
		else if ((keyType == SOCMNLIST_TYPE_KEY_STRING) && ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_STRING))
		{
			rightKey = (*((SOCmnString*)entryWK->m_key) == (LPCTSTR)key);
		}
	}

	return rightKey;
}

BOOL SOCmnListHeader::hasObject(
	IN SOCmnListEntry* entry,
	IN void* obj)
const
{
	BOOL rightObj = FALSE;

	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_STRING)
	{
		rightObj = (*((SOCmnString*)entry->m_object) == (*(SOCmnString*)obj));
	}
	else
	{
		rightObj = (entry->m_object == obj);
	}

	return rightObj;
}


WORD SOCmnListHeader::hashKey(
	IN void* key,
	IN WORD keyType)
const
{
	WORD hash = 0;

	if (keyType == SOCMNLIST_TYPE_KEY_DWORD)
	{
		hash = (WORD)key;
	}
	else if (keyType == SOCMNLIST_TYPE_KEY_STRING)
	{
		// string key
		if (key)
		{
			LPTSTR string = (LPTSTR)key;

			while (*string)
			{
				hash = (hash << 5) + hash + *string;
				string++;
			}
		}
	}

	return hash;
}


void SOCmnListHeader::doAutoLock(IN BOOL doIt)
{
	if (doIt)
	{
		if ((m_state & SOCMNLIST_STATE_AUTO_MASK) == SOCMNLIST_STATE_AUTO_NOTLOCK)
		{
			m_state = (m_state & ~SOCMNLIST_STATE_AUTO_MASK) | SOCMNLIST_STATE_AUTO_UNLOCKED;
		}
	}
	else
	{
		if ((m_state & SOCMNLIST_STATE_AUTO_MASK) == SOCMNLIST_STATE_AUTO_LOCKED)
		{
			unlock();
		}

		m_state = (m_state & ~SOCMNLIST_STATE_AUTO_MASK) | SOCMNLIST_STATE_AUTO_NOTLOCK;
	}
}


SOCmnSync* SOCmnListHeader::getSyncObject(void)
{
	if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
	{
		if (isMap())
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			return infoM->m_sync;
		}
		else
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			return infoF->m_sync;
		}
	}

	return NULL;
}

void SOCmnListHeader::lock(void)
{
	if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
	{
		if (isMap())
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoM->m_sync->lock();
		}
		else
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			infoF->m_sync->lock();
		}
	}
}

void SOCmnListHeader::unlock(void)
{
	if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
	{
		if (isMap())
		{
			SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
			infoM->m_sync->unlock();
		}
		else
		{
			SOCmnListInfoFullList* infoF = (SOCmnListInfoFullList*)m_info;
			infoF->m_sync->unlock();
		}
	}
}

BOOL SOCmnListHeader::doRehash(BOOL enforceRehash)
{
	if ((!enforceRehash) && (SOCmnListHeader::s_skipRehashing))
	{
		return FALSE;
	}

	if ((m_type & SOCMNLIST_TYPE_ORG_MASK) != SOCMNLIST_TYPE_ORG_MAP)
	{
		return FALSE;
	}

	++m_rehashCount;
	if (m_rehashCount != 0)
	{
		return FALSE;
	}

	BOOL ret = FALSE;
	lock();
	DWORD count = getCount();
	WORD hashSize = getHashTableSize();
	DWORD countUp = count >> 2;
	WORD newHash = 0;

	if ((countUp > hashSize) && (hashSize < 65521))
	{
		// prime number hash sizes (hash up):
		switch (hashSize)
		{
		case 17:
			newHash = 53;
			break;
		case 53:
			newHash = 151;
			break;
		case 151:
			newHash = 499;
			break;
		case 499:
			newHash = 997;
			break;
		case 997:
			newHash = 4999;
			break;
		case 4999:
			newHash = 9973;
			break;
		case 9973:
			newHash = 49999;
			break;
		case 49999:
			newHash = 65521;
			break;
		default:
			if (hashSize < 53)
			{
				newHash = 53;
			}
			else if (hashSize < 151)
			{
				newHash = 151;
			}
			else if (hashSize < 499)
			{
				newHash = 499;
			}
			else if (hashSize < 997)
			{
				newHash = 997;
			}
			else if (hashSize < 4999)
			{
				newHash = 4999;
			}
			else if (hashSize < 9973)
			{
				newHash = 9973;
			}
			else if (hashSize < 49999)
			{
				newHash = 49999;
			}
			else
			{
				newHash = 65521;
			}
		}
	}

	if (newHash != 0)
	{
		ret = rehash(newHash);
	}

	unlock();

	return ret;

}

BOOL SOCmnListHeader::rehash(WORD newHashSize)
{
	SOCmnListConfig cfg;
	SOCmnListHeader dest;

	if (!isInit())
	{
		return FALSE;
	}

	if ((m_type & SOCMNLIST_TYPE_ORG_MASK) != SOCMNLIST_TYPE_ORG_MAP)
	{
		return TRUE;    // no map -> nothing to do
	}

	// no rehashing of data lists
	if ((m_type & SOCMNLIST_TYPE_OBJ_MASK) == SOCMNLIST_TYPE_OBJ_DATA)
	{
		return FALSE;
	}

	autoLock();
	cfg.m_type = m_type;
	SOCmnListInfoMap* infoM = (SOCmnListInfoMap*)m_info;
	cfg.m_hashTableSize = newHashSize;
	cfg.m_entriesPerBlock = infoM->m_defaultBlockElements;
	dest.create(&cfg);
	SOCmnListInfoSimpleList* infoSS;
	SOCmnListEntry* curS;
	DWORD hash = 0;
	BOOL loop = TRUE;

	do
	{
		SOCmnListInfoMap* infoMS = (SOCmnListInfoMap*)m_info;
		infoSS = &(((SOCmnListInfoSimpleList*)infoMS->m_hashTable)[hash]);
		hash++;

		if (hash == infoMS->m_hashTableSize)
		{
			loop = FALSE;
		}

		curS = (SOCmnListEntry*)infoSS->m_start;

		// first entry
		while (curS)
		{
			if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_NO)
			{
				dest.add(curS->m_object, 0, ((m_type & SOCMNLIST_TYPE_OBJECT_KEY_STRING) == SOCMNLIST_TYPE_OBJECT_KEY_STRING) ? SOCMNLIST_TYPE_KEY_STRING : SOCMNLIST_TYPE_KEY_DWORD);
			}
			else
			{
				SOCmnListEntryWithKey* curSK = (SOCmnListEntryWithKey*)curS;

				if ((m_type & SOCMNLIST_TYPE_KEY_MASK) == SOCMNLIST_TYPE_KEY_DWORD)
				{
					dest.add(curS->m_object, curSK->m_key, SOCMNLIST_TYPE_KEY_DWORD);
				}
				else
				{
					dest.add(curS->m_object, (void*)(LPCTSTR)(*((SOCmnString*)curSK->m_key)), SOCMNLIST_TYPE_KEY_STRING);
				}
			}

			curS = (SOCmnListEntry*)curS->m_next;
		}
	}
	while (loop);

	SOCmnListInfoMap* infoD = (SOCmnListInfoMap*)dest.m_info;
	// free previous list data
	removeAll();

	if ((m_type & SOCMNLIST_TYPE_SYNCED) == SOCMNLIST_TYPE_SYNCED)
	{
		SOCMN_FREE_OBJECT(infoD->m_sync) // free new sync - keep the old
	}

	if (infoM->m_hashTable)
	{
		free(infoM->m_hashTable);
	}

	// set rehashed
	m_state = dest.m_state;
	m_type = dest.m_type;
	infoM->m_hashTable = infoD->m_hashTable;
	infoM->m_free = infoD->m_free;
	infoM->m_blockFirst = infoD->m_blockFirst;
	infoM->m_count = infoD->m_count;
	infoM->m_hashTableSize = infoD->m_hashTableSize;
	infoM->m_defaultBlockElements = infoD->m_defaultBlockElements;
	infoM->m_flags = infoD->m_flags;
	dest.detach();
	SOCMN_FREE_OBJECT(infoD)
	setDirty(TRUE);
	return TRUE;
}

BOOL SOCmnListHeader::sort(SOCmnListSortCompareProc compare)
{
	if ((m_type & SOCMNLIST_TYPE_ORG_LIST) != SOCMNLIST_TYPE_ORG_LIST)
	{
		return FALSE; // only a list can be sorted ...
	}

	int iCnt = getCount();

	if (iCnt < 1)
	{
		return FALSE; //  an empty list may not be sorted ...
	}

	autoLock();
	SOCmnListPosition* PosList = new SOCmnListPosition[iCnt];
	SOCmnListPosition pos = getStartPosition();
	int posCnt = 0;

	while (pos)
	{
		PosList[posCnt] = pos;
		posCnt++;
		getNext(pos, SOCMNLIST_TYPE_KEY_NO, NULL);
	}

	quicksort(0, iCnt - 1, PosList, compare);
	delete PosList;
	autoUnlock();
	return TRUE;
}

const void* SOCmnListHeader::getItem(int index, SOCmnListPosition* PosList)
{
	SOCmnListPosition pos = PosList[index];
	return getNext(pos, SOCMNLIST_TYPE_KEY_NO, NULL);
}

void SOCmnListHeader::quicksort(int lo, int hi, SOCmnListPosition* PosList, SOCmnListSortCompareProc compare)
{
	int i = lo, j = hi;
	const void* x = getItem((lo + hi) / 2, PosList);

	while (i <= j)
	{
		while (compare(getItem(i, PosList), x) < 0)
		{
			i++;
		}

		while (compare(getItem(j, PosList), x) > 0)
		{
			j--;
		}

		if (i <= j)
		{
			if (i < j)
			{
				exchange(i, j, PosList);
			}

			i++;
			j--;
		}
	}

	// Recursion
	if (lo < j)
	{
		quicksort(lo, j, PosList, compare);
	}

	if (i < hi)
	{
		quicksort(i, hi, PosList, compare);
	}
}

// just for testing
void SOCmnListHeader::bubblesort(int lo, int hi, SOCmnListPosition* PosList, SOCmnListSortCompareProc compare)
{
	for (int i = lo; i < hi; i++)
		for (int j = i; j <= hi; j++)
			if (compare(getItem(i, PosList), getItem(j, PosList)) > 0)
			{
				exchange(i, j, PosList);
			}
}


void SOCmnListHeader::exchange(int i, int j, SOCmnListPosition* PosList)
{
	SOCmnListEntry* pElem1 = (SOCmnListEntry*)PosList[i];
	SOCmnListEntry* pElem2 = (SOCmnListEntry*)PosList[j];
	void* pObj = pElem1->m_object;
	pElem1->m_object = pElem2->m_object;
	pElem2->m_object = pObj;
}

void SOCmnListHeader::skipRehashing()
{
	SOCmnListHeader::s_skipRehashing = TRUE;
}

BOOL SOCmnListHeader::isRehashingEnabled()
{
	return !(SOCmnListHeader::s_skipRehashing);
}

// end QuickSorter


