#ifndef _SOCMNHANDLEMANAGER_H_
#define _SOCMNHANDLEMANAGER_H_

#pragma pack(push, 4)

#ifndef MAXUINT16
#define MAXUINT16   ((UINT16)~((UINT16)0))
#endif //MAXUINT16

#ifndef MAXUINT8
#define MAXUINT8   ((UINT8)~((UINT8)0))
#endif //MAXUINT8

#ifndef OPCHANDLE
#define OPCHANDLE DWORD
#endif //OPCHANDLE

#define PROVIDE_OPC_HANDLE(pObject) SOCmnHandleManager::getInstance()->provideHandle(pObject)
#define GET_MEMORY_ADDRESS(opcHandle) SOCmnHandleManager::getInstance()->getMemoryAddress(opcHandle)
#define RELEASE_OPC_HANDLE(opcHandle) SOCmnHandleManager::getInstance()->releaseHandle(opcHandle)

typedef struct _Page
{
	ptrdiff_t handles[65536];
	UINT32 handleValid[2048]; // 2048 * 32 bits = 65536; one bit for each handle inside the page
	UINT16 pageNumber;
	UINT16 nextHandle;
	UINT16 lastHandle;
	UINT16 freeHandlesCount;
} Page;

typedef struct _PageList
{
	Page *page;
	_PageList *nextPage;
} PageList;

class SOCmnHandleManager
{
public:
	static SOCmnHandleManager *getInstance();
	static void destroyInstance();

private:
	SOCmnHandleManager();
	~SOCmnHandleManager();

public:
	UINT32 provideHandle(IN void* pObject);
	ptrdiff_t getMemoryAddress(IN UINT32 opcHandle);
	void releaseHandle(IN UINT32 opcHandle);

private:
	static SOCmnHandleManager *s_pSingletonInstance;
	PageList *m_activePageList;
	SOCmnMutex m_mutex;
	Page *m_pages[65536];
	UINT16 m_nextPageNumber;
};

#pragma pack(pop)
#endif

