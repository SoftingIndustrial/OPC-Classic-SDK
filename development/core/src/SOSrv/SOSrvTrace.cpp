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
//  Filename    : SOSrvTrace.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Server monitoring classes                               |
//                - SOSrvTrace: watch object                                  |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_WATCH

#define SOSRVTRACE_XML_HEADER      _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><trc>")
#define SOSRVTRACE_XML_HEADER_SIZE 43
#define SOSRVTRACE_XML_FOOTER      _T("</trc>")
#define SOSRVTRACE_XML_FOOTER_SIZE 6
#define SOSRVTRACE_XML_LINE_SIZE   9
#define SOSRVTRACE_XML_BUFFER_OVERFLOW _T("<ln>Buffer overflow</ln>")
#define SOSRVTRACE_XML_BUFFER_OVERFLOW_SIZE 24

//-----------------------------------------------------------------------------
// SOSrvTrace                                                                 |
//-----------------------------------------------------------------------------

SOSrvTrace::SOSrvTrace(void)
{
	m_bufferTraceOn = FALSE;
	m_bufferSize = 1024;
	m_bufferOffset = 0;
	m_buffer = NULL;
	m_bufferOverflow = FALSE;
}

SOSrvTrace::~SOSrvTrace(void)
{
	if (m_buffer)
	{
		free(m_buffer);
	}

	m_buffer = NULL;
}

void SOSrvTrace::enableTraceToBuffer(BOOL enable)
{
	if (m_bufferTraceOn != enable)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_getLock);

		if (enable)
		{
			m_buffer = (TCHAR*)realloc(m_buffer, (m_bufferSize + SOSRVTRACE_XML_FOOTER_SIZE + SOSRVTRACE_XML_BUFFER_OVERFLOW_SIZE));
			_tcscpy(m_buffer, SOSRVTRACE_XML_HEADER);
			m_bufferOffset = SOSRVTRACE_XML_HEADER_SIZE;
			m_bufferOverflow = FALSE;
			SOCmnTrace* pTrace = ::getSOCmnTrace();
			SOCmnList<SOCmnTraceReceiver> recList(pTrace->getReceiverList());
			pTrace->m_lock.lock();
			recList.add(this);
			pTrace->m_lock.unlock();
		}
		else
		{
			SOCmnTrace* pTrace = ::getSOCmnTrace();
			SOCmnList<SOCmnTraceReceiver> recList(pTrace->getReceiverList());
			pTrace->m_lock.lock();
			recList.removeObject(this);
			pTrace->m_lock.unlock();

			if (m_buffer)
			{
				free(m_buffer);
			}

			m_buffer = NULL;
			m_bufferOffset = 0;
		}
	}

	m_bufferTraceOn = enable;
}

void SOSrvTrace::setBufferSize(DWORD size)
{
	if (size == m_bufferSize)
	{
		return;
	}

	SOCmnSingleLock<SOCmnSync> lock(&m_getLock);

	if (size < 1016)
	{
		size = 1016;
	}

	m_bufferSize = size;
	m_bufferOffset = 0;

	if (m_bufferTraceOn)
	{
		m_buffer = (TCHAR*)realloc(m_buffer, (m_bufferSize +  + SOSRVTRACE_XML_FOOTER_SIZE + SOSRVTRACE_XML_BUFFER_OVERFLOW_SIZE));
		_tcscpy(m_buffer, SOSRVTRACE_XML_HEADER);
		m_bufferOffset = SOSRVTRACE_XML_HEADER_SIZE;
		m_bufferOverflow = FALSE;
	}
}

HRESULT SOSrvTrace::setOptions(IN LPCTSTR optionsT)
{
	SOCmnTrace* pTrace = ::getSOCmnTrace();
	SOCmnSingleLock<SOCmnSync> lock(&m_optionsLock);
	// parse request
	SOCmnPointer<SOSrvWatchMethod> mth;
	USES_CONVERSION;
	LPCSTR options = T2CA(optionsT);
	SOCMN_ALLOCATE_OBJECT_C(mth, SOSrvWatchMethod, (SOSrvWatchMethod*))
	mth->setObjectState(SOSRVWATCHMETHOD_STATE_SETTRACE);

	if (!mth->parse(options, strlen(options)))
	{
		return E_FAIL;
	}

	if (mth->getObjectState() == SOCMNXMLMETHOD_STATE_ERROR)
	{
		return E_FAIL;
	}

	SOCmnStringList xmlElements(mth->getMethodElements());
	DWORD levelErr = 0xFFFFFFFF;
	DWORD levelWrn = 0xFFFFFFFF;
	DWORD levelInf = 0;
	DWORD levelDbg = 0;
	DWORD fileSize = 1000000;
	DWORD fileOn = FALSE;
	SOCmnString fileName;
	SOCmnString fileName2;
	SOCmnString convHlp;
	BOOL setLevel = FALSE;
	BOOL setFile = FALSE;

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCLERR, &convHlp))
	{
		levelErr = _tcstoul(convHlp, NULL, 0);
		setLevel = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCLWRN, &convHlp))
	{
		levelWrn = _tcstoul(convHlp, NULL, 0);
		setLevel = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCLINF, &convHlp))
	{
		levelInf = _tcstoul(convHlp, NULL, 0);
		setLevel = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCLDBG, &convHlp))
	{
		levelDbg = _tcstoul(convHlp, NULL, 0);
		setLevel = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCFENA, &convHlp))
	{
		fileOn = _tcstoul(convHlp, NULL, 0);
		setFile = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCFSIZ, &convHlp))
	{
		fileSize = _tcstoul(convHlp, NULL, 0);
		setFile = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCFNAM, &fileName))
	{
		setFile = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCFNA2, &fileName2))
	{
		setFile = TRUE;
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCBENA, &convHlp))
	{
		enableTraceToBuffer(_tcstoul(convHlp, NULL, 0) != 0);
	}

	if (xmlElements.findKey(SOSRVWATCHMETHODDATA_TYPE_TRCBSIZ, &convHlp))
	{
		setBufferSize(_tcstoul(convHlp, NULL, 0));
	}

	if (setLevel)
	{
		pTrace->setLevels(levelErr, levelWrn, levelInf, levelDbg);
	}

	if (setFile)
	{
		if (fileOn != (DWORD)pTrace->getTraceToFile())
		{
			if (fileOn)
			{
				pTrace->setFile(fileName, 5, fileName2, fileSize);
			}

			pTrace->enableTraceToFile(fileOn);

			if (!fileOn)
			{
				pTrace->setFile(fileName, 5, fileName2, fileSize);
			}
		}
		else
		{
			if ((fileName != pTrace->getFileName()) ||
				(fileName2 != pTrace->getSecondFileName()) ||
				(fileSize != pTrace->getFileSize()))
			{
				pTrace->setFile(fileName, 5, fileName2, fileSize);
			}
		}
	}

	return S_OK;
}


HRESULT SOSrvTrace::getOptions(OUT SOCmnStringEx& options)
{
	SOCmnTrace* pTrace = ::getSOCmnTrace();
	SOCmnSingleLock<SOCmnSync> lock(&m_optionsLock);
	options.format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<op><level><err>%lu</err><wrn>%lu</wrn><inf>%lu</inf><dbg>%lu</dbg></level>\
<file><enable>%lu</enable><name>%s</name><name2>%s</name2><size>%lu</size></file>\
<buffer><enable>%lu</enable><size>%lu</size></buffer></op>"),
				   pTrace->m_traceLevel[SOCMNTRACE_L_ERR], pTrace->m_traceLevel[SOCMNTRACE_L_WRN], pTrace->m_traceLevel[SOCMNTRACE_L_INF], pTrace->m_traceLevel[SOCMNTRACE_L_DEB],
				   (DWORD)pTrace->getTraceToFile(), pTrace->getFileName(), pTrace->getSecondFileName(), pTrace->getFileSize(),
				   (DWORD)m_bufferTraceOn, m_bufferSize);
	return S_OK;
}

HRESULT SOSrvTrace::saveOptions(void)
{
	SOCmnTrace* pTrace = ::getSOCmnTrace();
	SOCmnSingleLock<SOCmnSync> lock(&m_optionsLock);
	SOCmnPointer<SOCmnTraceHandler> traceH = pTrace->getTraceHandler();

	if (traceH.isNotNull())
	{
		return traceH->saveTraceOptions();
	}
	else
	{
		return E_NOTIMPL;
	}
}

#ifdef SOFEATURE_DCOM
HRESULT SOSrvTrace::get(OUT LPWSTR* trace)
{
	if (!m_bufferTraceOn)
	{
		return E_FAIL;
	}

	if (!m_buffer)
	{
		return E_FAIL;
	}

	SOCmnSingleLock<SOCmnSync> lock(&m_getLock);
	DWORD size = (m_bufferOffset + SOSRVTRACE_XML_FOOTER_SIZE + 1) * 2;
	_tcscpy(m_buffer + m_bufferOffset, SOSRVTRACE_XML_FOOTER);
	*trace = (LPWSTR)CoTaskMemAlloc(size);

	if (!trace)
	{
		return E_OUTOFMEMORY;
	}

	USES_CONVERSION;
	WCHAR* wBuffer = T2W(m_buffer);
	wcscpy(*trace, wBuffer);
	m_bufferOffset = SOSRVTRACE_XML_HEADER_SIZE;
	m_buffer[m_bufferOffset] = _T('\0');
	m_bufferOverflow = FALSE;
	return S_OK;
}
#endif

void SOSrvTrace::onTrace(IN LPCTSTR traceString, IN USHORT traceStringLen, IN USHORT level, IN DWORD mask, IN LPCTSTR objId, IN LPCTSTR text)
{
	SOCmnSingleLock<SOCmnSync> lock(&m_getLock);

	if (m_bufferOverflow)
	{
		return;
	}

	if ((m_bufferOffset + traceStringLen + SOSRVTRACE_XML_LINE_SIZE - 2) < m_bufferSize)
	{
		USES_CONVERSION;
		memcpy(m_buffer + m_bufferOffset, _T("<ln>"), 4);
		memcpy(m_buffer + (m_bufferOffset + 4), traceString, traceStringLen - 2);
		memcpy(m_buffer + (m_bufferOffset + traceStringLen + 2), _T("</ln>"), 5);
		m_bufferOffset += traceStringLen + 7;
		m_buffer[m_bufferOffset] = _T('\0');
	}
	else
	{
		m_bufferOverflow = TRUE;
		memcpy(m_buffer + m_bufferOffset, SOSRVTRACE_XML_BUFFER_OVERFLOW, SOSRVTRACE_XML_BUFFER_OVERFLOW_SIZE);
		m_bufferOffset += SOSRVTRACE_XML_BUFFER_OVERFLOW_SIZE;
		m_buffer[m_bufferOffset] = _T('\0');
	}
}

#endif // SOFEATURE_WATCH

#endif // SOSRV
