#include "stdafx.h"
#include "SOCmn.h"
#include "SocmnHandleManager.h"

SOCmnHandleManager* SOCmnHandleManager::s_pSingletonInstance = NULL;

SOCmnHandleManager::SOCmnHandleManager(void)
{
	// initialize all pages to NULL
	memset(m_pages, NULL, sizeof(Page*) * 65536);

	// set the current page as NULL
	m_activePageList = NULL;

	// initialize next page number
	// page 0 is NOT used due to OPC Considerations
	m_nextPageNumber = 1;
}

SOCmnHandleManager::~SOCmnHandleManager(void)
{
	// 1) free all elements in the page list queue
	PageList *pCurrentActivePage;
	//		a) free all elements which refer a page
	while (m_activePageList != NULL)
	{
		pCurrentActivePage = m_activePageList;
		m_activePageList = m_activePageList->nextPage;
		free(pCurrentActivePage);
	}
	m_activePageList = NULL;

	// 2) free the content inside each used page
	for (int i = 0; i < 65536; ++i)
	{
		if (m_pages[i] != NULL)
		{
			free(m_pages[i]);
			m_pages[i] = NULL;
		}
	}
}

SOCmnHandleManager* SOCmnHandleManager::getInstance()
{
	if (s_pSingletonInstance == NULL)
	{
		s_pSingletonInstance = new SOCmnHandleManager();
	}

	return s_pSingletonInstance;
}

void SOCmnHandleManager::destroyInstance()
{
	if (s_pSingletonInstance != NULL)
	{
		delete s_pSingletonInstance;
		s_pSingletonInstance = NULL;
	}
}

UINT32 SOCmnHandleManager::provideHandle(IN void* pObject)
{
	// acquire lock
	m_mutex.lock();

	Page *pPage = NULL;

	if (m_activePageList == NULL)
	{
		/********************************************
		 * IF THERE IS NO PAGE AVAILABLE
		 ********************************************/
		
		// create and initialize a new page object
		pPage = (Page*)malloc(sizeof(Page));
		if (!pPage)
		{
			m_mutex.unlock();
			return NULL;
		}
		memset(pPage, 0, sizeof(Page));

		for (int i = 0; i < 65535; ++i)
		{
			pPage->handles[i] = i + 1;
		}
		pPage->lastHandle = 65535;

		pPage->pageNumber = m_nextPageNumber;
		pPage->nextHandle = 1; // handle 0 is NOT used due to OPC Considerations
		pPage->freeHandlesCount = 65535;

		// add the newly created page to the pages array
		m_pages[m_nextPageNumber] = pPage;

		// create a new PageList object which to include the newly created page
		PageList *pNewPageList = (PageList*)malloc(sizeof(PageList));
		if (!pNewPageList)
		{
			m_mutex.unlock();
			return NULL;
		}
		memset(pNewPageList, 0, sizeof(PageList));
		pNewPageList->page = pPage;
		pNewPageList->nextPage = m_activePageList;

		// include the newly created PageList object into the active page list
		m_activePageList = pNewPageList;

		// (preparation for next page) advance with the page cursor to an empty (non allocated) page
		while (m_pages[m_nextPageNumber] != NULL)
		{
			if (m_nextPageNumber < 65536)
			{
				++m_nextPageNumber;
			}
			else
			{
				// page 0 is NOT used due to OPC Considerations
				m_nextPageNumber = 1;
			}
		}
	}
	else
	{
		/********************************************
		 * IF AN AVAILABLE PAGE ALREADY EXISTS
		 ********************************************/

		// get the currently active page
		pPage = m_activePageList->page;
	}

	// compute the OPC Handle
	UINT32 handleToReturn = (((UINT32)pPage->pageNumber) << 16) | pPage->nextHandle;

	// get the index of the valid array
	UINT16 validIndex = pPage->nextHandle >> 5; // 2^5 = 32; equivalent to (pPage->nextHandle / 32)
	// get the bit inside the respective valid array value
	UINT16 validBit = pPage->nextHandle & 0x1F; // 0x1f = 31; equivalent to (pPage->nextHandle % 32)

	// set validity bit
	pPage->handleValid[validIndex] |= 1 << validBit;

	// using "validIndex" as a "temp" variable
	validIndex = pPage->nextHandle;

	// decrement free handles counter
	--(pPage->freeHandlesCount);
	// advance the next handle cursor if page is not depleted
	if (pPage->handles[pPage->nextHandle] != 0)
	{
		pPage->nextHandle = (UINT16)pPage->handles[pPage->nextHandle];
	}

	// store the object`s memory address at the previous handle position
	pPage->handles[validIndex] = (ptrdiff_t)pObject;

	/********************************************
	 * CHECH IF CURRENTLY AVAILABLE/USED PAGE
	 * HAS NO MORE AVAILABLE HANDLES
	 ********************************************/
	if (pPage->freeHandlesCount == 0)
	{
		PageList *pPageListToDelete = m_activePageList;
		m_activePageList = m_activePageList->nextPage;
		free(pPageListToDelete);
	}
	// release lock
	m_mutex.unlock();

	// return the OPC Handle
	return handleToReturn;
}

ptrdiff_t SOCmnHandleManager::getMemoryAddress(IN UINT32 opcHandle)
{
	if (!opcHandle)
	{
		return NULL;
	}

	// ignore "warning C4333: '>>' : right shift by too large amount, data loss"
#pragma warning(disable : 4333)
	// get the page
	UINT16 handlePage = (UINT16)(opcHandle >> 16);
#pragma warning(default: 4333)

	// acquire lock
	m_mutex.lock();

	Page *pPage = m_pages[handlePage];
	if (!pPage)
	{
		// page index not valid
		_ASSERTION(0, "OPC_HandleManager: No object to be returned, PROVIDED OPC HANDLE IS ~~INVALID~~ (or object was previously destroyed)!!! (invalid page number)");
		m_mutex.unlock();
		return NULL;
	}

	// get the index inside the page
	UINT16 handleIndex = (UINT16)(opcHandle & 0xffff);
	// get the index of the valid array
	UINT16 validIndex = handleIndex >> 5; // 2^5 = 32; equivalent to (handleIndex / 32)
	// get the bit inside the respective valid array value
	UINT8 validBit = (UINT8)(handleIndex & 0x1f); // 0x1f = 31; equivalent to (handleIndex % 32)
	
	// compute the bit mask
	UINT32 bitMask = 1 << validBit;

	if ((pPage->handleValid[validIndex] & bitMask) == 0)
	{
		// handle index not valid
		_ASSERTION(0, "OPC_HandleManager: No object to be returned, PROVIDED OPC HANDLE IS ~~INVALID~~ (or object was previously destroyed)!!! (invalid handle number inside page)");
		m_mutex.unlock();
		return NULL;
	}

	ptrdiff_t memAddress = pPage->handles[handleIndex];

	// release lock
	m_mutex.unlock();

	// return the memory address corresponding to the received opcHandle
	return memAddress;
}

void SOCmnHandleManager::releaseHandle(IN UINT32 opcHandle)
{
	// ignore "warning C4333: '>>' : right shift by too large amount, data loss"
#pragma warning(disable : 4333)
	// get the page containing the memory address
	UINT16 handlePage = (UINT16)(opcHandle >> 16);
#pragma warning(default: 4333)

	// acquire lock
	m_mutex.lock();

	Page *pPage = m_pages[handlePage];
	if (!pPage)
	{
		// page index not valid
		_ASSERTION(0, "Invalid OPC Handle released by current object!");
		m_mutex.unlock();
		return;
	}

	// get the index inside the page
	UINT16 handleIndex = (UINT16)(opcHandle & 0xffff);
	// get the index of the valid array
	UINT16 validIndex = handleIndex >> 5; // 2^5 = 32; equivalent to (handleIndex / 32)
	// get the bit inside the respective valid array value
	UINT8 validBit = (UINT8)(handleIndex & 0x1f); // 0x1f = 31; equivalent to (handleIndex % 32)
	// compute the bit mask
	UINT32 bitMask = 1 << validBit;
	
	if ((pPage->handleValid[validIndex] & bitMask) == 0)
	{
		// handle index not valid
		_ASSERTION(0, "Invalid OPC Handle released by current object!");
		m_mutex.unlock();
		return;
	}

	// clear validity bit
	pPage->handleValid[validIndex] &= ~bitMask;

	// increment free handles count
	++(pPage->freeHandlesCount);

	if (pPage->freeHandlesCount == 1)
	{
		// if this is the first freed handle

		// recycle the emptied handle
		pPage->nextHandle = handleIndex;
		pPage->lastHandle = handleIndex;
		pPage->handles[handleIndex] = NULL;

		// create a new pageList object
		PageList *pNewPageList = (PageList*)malloc(sizeof(PageList));
		if (!pNewPageList)
		{
			m_mutex.unlock();
			return;
		}
		memset(pNewPageList, 0, sizeof(PageList));
		pNewPageList->page = pPage;
		pNewPageList->nextPage = NULL;

		// append the newly created pageList object active page list
		if (m_activePageList == NULL)
		{
			m_activePageList = pNewPageList;
		}
		else
		{
			PageList *previous = m_activePageList;
			while (previous->nextPage != NULL)
			{
				previous = previous->nextPage;
			}
			previous->nextPage = pNewPageList;
		}
	}
	else
	{
		// recycle the emptied handle
		pPage->handles[pPage->lastHandle] = handleIndex;
		pPage->lastHandle = handleIndex;
		pPage->handles[handleIndex] = NULL;

		if (pPage->freeHandlesCount == 65535)
		{
			// if this is the last freed handle (page is EMPTY)

			if ((m_activePageList) && (m_activePageList->page == pPage))
			{
				// if it`s the first page in the active page list
				PageList* pPageListToDelete = m_activePageList;
				m_activePageList = m_activePageList->nextPage;
				m_pages[pPageListToDelete->page->pageNumber] = NULL;
				free(pPageListToDelete->page);
				free(pPageListToDelete);
			}
			else
			{
				PageList *currentPageCursor = m_activePageList;
				while (currentPageCursor->nextPage != NULL)
				{
					if (currentPageCursor->nextPage->page == pPage)
					{
						break;
					}
					currentPageCursor = currentPageCursor->nextPage;
				}

				// if the next page is not null (that is the corresponding PageList object was found) delete the object
				if ((currentPageCursor) && (currentPageCursor->nextPage))
				{
					PageList* pPageListToDelete = currentPageCursor->nextPage;
					currentPageCursor->nextPage = pPageListToDelete->nextPage;
					m_pages[pPageListToDelete->page->pageNumber] = NULL;
					free(pPageListToDelete->page);
					free(pPageListToDelete);
				}
			}
		}
	}
	// release lock
	m_mutex.unlock();
}