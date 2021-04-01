#ifndef _OTB_H_
#define _OTB_H_

#pragma pack(push, 4)


extern void SetOutputStructMemberString(const SOCmnString& string, LPTSTR* pStructString);
extern void SetOutputStructMemberDWORDArray(SOCmnDWORDList* pDwordList, DWORD* pArrayCount, DWORD** ppDwordArray);
extern void SetOutputStructMemberStringArray(SOCmnStringList* pStringList, DWORD* pArrayCount, LPTSTR** ppStringArray);

void FreeOTSAddressSpaceElementData(IN OTSAddressSpaceElementData* pData);
void FreeOTSPropertyData(IN OTSPropertyData* pPropDataO);



//-----------------------------------------------------------------------------
// TraceHandler
//-----------------------------------------------------------------------------

class TraceHandler : public SOCmnTraceHandler
{
public:
	virtual void getUserMaskString(IN DWORD mask, OUT LPTSTR string);
	virtual void onTrace(IN LPCTSTR traceString, IN USHORT traceStringLen, IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text);
}; // TraceHandler

#pragma pack(pop)
#endif
