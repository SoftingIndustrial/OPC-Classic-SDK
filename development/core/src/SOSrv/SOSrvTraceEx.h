#ifndef _SOSRVTRACEEX_H_
#define _SOSRVTRACEEX_H_

#ifdef SOFEATURE_SOAP

#include "SOCmnObject.h"
#include "SOCmnTrace.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOSrvWebTrace                                                              |
//-----------------------------------------------------------------------------

#define SOSRVWEBTRACE_LINE_CNT 30

class SOSRV_EXPORT SOSrvWebTrace : public SOCmnTraceReceiver
{
public:
	SOSrvWebTrace(void);

	virtual void onTrace(IN LPCTSTR traceString, IN USHORT traceStringLen, IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text);
	void getTrace(SOCmnStringEx& trace);

protected:
	SOCmnSync m_getLock;
	SOCmnStringEx m_line[SOSRVWEBTRACE_LINE_CNT];
	DWORD m_nextLine;

	~SOSrvWebTrace(void);
}; // SOSrvWebTrace

#pragma pack(pop)

#endif // SOFEATURE_SOAP

#endif
