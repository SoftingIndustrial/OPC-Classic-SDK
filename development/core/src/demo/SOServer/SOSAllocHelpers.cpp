#include "StdAfx.h"

void* OTAllocateMemory(DWORD size)
{
	void* pMemory;
	pMemory = malloc(size);
	memset(pMemory, 0, size);
	return pMemory;
}

void OTFreeMemory(void* pMemory)
{
	if (pMemory)
	{
		free(pMemory);
	}
}