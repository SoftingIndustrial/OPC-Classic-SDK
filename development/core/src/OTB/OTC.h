#ifndef _OTC_H_
#define _OTC_H_

#pragma pack(push, 4)


void getOTCObjectData(IN OTCObjectHandle objectHandle, OUT OTCObjectData* pObjectData);
void getOTCObjectContext(IN OTCObjectHandle objectHandle, OUT OTCObjectContext* pObjectContext);
void getOTCObjectsData(IN DWORD objectCount, IN OTCObjectHandle* pObjectHandles, OUT OTCObjectData* pObjectData);

extern OTCCallbackFunctions g_callbackFunctions;

#pragma pack(pop)
#endif
