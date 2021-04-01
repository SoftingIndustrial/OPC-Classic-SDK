#ifndef _SOSRVTRACE_H_
#define _SOSRVTRACE_H_

#include "SOProxyDef.h"

#ifdef SOFEATURE_WATCH

#include "SOCmnObject.h"
#include "SOCmnTrace.h"


#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOSrvTrace                                                                 |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvTrace : public SOCmnTraceReceiver
{
public:
	SOSrvTrace(void);

	virtual HRESULT setOptions(IN LPCTSTR options);
	virtual HRESULT getOptions(OUT SOCmnStringEx& options);
	virtual HRESULT saveOptions(void);
#ifdef SOFEATURE_DCOM
	virtual HRESULT get(OUT LPWSTR* trace);
#endif

	void enableTraceToBuffer(BOOL enable);
	void setBufferSize(DWORD size);

	virtual void onTrace(IN LPCTSTR traceString, IN USHORT traceStringLen, IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text);

protected:
	SOCmnSync m_optionsLock;
	SOCmnSync m_getLock;

	BOOL m_bufferTraceOn;
	DWORD m_bufferSize;
	TCHAR* m_buffer;
	DWORD m_bufferOffset;
	BOOL m_bufferOverflow;

	~SOSrvTrace(void);
}; // SOSrvTrace

#pragma pack(pop)

#endif // SOFEATURE_WATCH

#endif
