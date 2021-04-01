#ifndef _SO_DMS_ALLOC_HELPERS
#define _SO_DMS_ALLOC_HELPERS

void* OTAllocateMemory(DWORD size);

void OTFreeMemory(void* pMemory);

typedef struct _OTSAddressSpaceElementData
{
	TCHAR* m_name;
	TCHAR* m_itemID;
	unsigned char m_accessRights;
	unsigned char m_ioMode;
	unsigned char m_hasChildren;
	unsigned char m_isBrowsable;
	unsigned long m_userData;
	unsigned short m_datatype;
	unsigned char m_elementType;
} OTSAddressSpaceElementData;

#endif // _SO_DMS_ALLOC_HELPERS