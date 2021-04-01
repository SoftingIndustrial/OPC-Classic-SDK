//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvTraceEx.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server monitoring classes                               |
//                - SOSrvWebTrace: watch object                               |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_SOAP

#include "SOSrvTraceEx.h"

//-----------------------------------------------------------------------------
// SOSrvWebTrace                                                                 |
//-----------------------------------------------------------------------------

SOSrvWebTrace::SOSrvWebTrace(void)
{
	m_nextLine = 0;
}

SOSrvWebTrace::~SOSrvWebTrace(void)
{
}

void SOSrvWebTrace::onTrace(
	IN LPCTSTR traceString,
	IN USHORT /* traceStringLen */,
	IN USHORT /* level */,
	IN DWORD /* mask */,
	IN LPCTSTR /* objId */,
	IN LPCTSTR /* text */)
{
	SOCmnSingleLock<SOCmnSync> lock(&m_getLock);
	m_line[m_nextLine].getBuffer((DWORD)_tcslen(traceString) + 5);
	m_line[m_nextLine] = traceString;
//	m_line[m_nextLine] += _T("<br>");
	m_nextLine++;

	if (m_nextLine >= SOSRVWEBTRACE_LINE_CNT)
	{
		m_nextLine = 0;
	}
}

void SOSrvWebTrace::getTrace(SOCmnStringEx& trace)
{
	SOCmnSingleLock<SOCmnSync> lock(&m_getLock);
	DWORD i, j;
	trace.getBuffer(SOSRVWEBTRACE_LINE_CNT * 200);
	j = m_nextLine;

	for (i = 0; i < SOSRVWEBTRACE_LINE_CNT; i++)
	{
		trace += m_line[j];
		j++;

		if (j >= SOSRVWEBTRACE_LINE_CNT)
		{
			j = 0;
		}
	}
}

#endif // SOFEATURE_SOAP

#endif // SOSRV
