#include "stdafx.h"

#include <SOCmnString.h>

#ifdef SOOS_WINDOWS

#ifdef SOFEATURE_TP_DISCOVERY

#include "SOCmnPipe.h"


SOCmnPipe::SOCmnPipe(SOCmnObject* parent)
{
	m_connected = FALSE;
	m_pipe = 0;
	SOCMN_ALLOCATE_OBJECT_C(m_writerThread, SOCmnThread(), (SOCmnThread*));
	m_pSecurityAttributes = (LPSECURITY_ATTRIBUTES)malloc(sizeof(SECURITY_ATTRIBUTES));
	m_pSecurityAttributes = (LPSECURITY_ATTRIBUTES)memset(m_pSecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
	m_pSecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
	m_pSecurityAttributes->bInheritHandle = FALSE;
	m_pSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(sizeof(SECURITY_DESCRIPTOR));
	m_pSecurityDescriptor = (PSECURITY_DESCRIPTOR)memset(m_pSecurityDescriptor, 0, sizeof(SECURITY_DESCRIPTOR));
} // end ctor



SOCmnPipe::SOCmnPipe(LPCTSTR aName)
{
	m_connected = FALSE;
	m_pipe = 0;
	m_name = aName;
	m_bufferSize = 0;
	SOCMN_ALLOCATE_OBJECT_C(m_writerThread, SOCmnThread(), (SOCmnThread*));
	m_pSecurityAttributes = (LPSECURITY_ATTRIBUTES)malloc(sizeof(SECURITY_ATTRIBUTES));
	m_pSecurityAttributes = (LPSECURITY_ATTRIBUTES)memset(m_pSecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
	m_pSecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
	m_pSecurityAttributes->bInheritHandle = FALSE;
	m_pSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(sizeof(SECURITY_DESCRIPTOR));
	m_pSecurityDescriptor = (PSECURITY_DESCRIPTOR)memset(m_pSecurityDescriptor, 0, sizeof(SECURITY_DESCRIPTOR));
} // end ctor



SOCmnPipe::~SOCmnPipe(void)
{
	disconnect();

	if (m_writerThread != NULL)
	{
		delete m_writerThread;
		m_writerThread = NULL;
	} // end if

	if (m_pSecurityAttributes != NULL)
	{
		free(m_pSecurityAttributes);
		m_pSecurityAttributes = NULL;
	} // end if

	if (m_pSecurityDescriptor != NULL)
	{
		free(m_pSecurityDescriptor);
		m_pSecurityDescriptor = NULL;
	} // end if
} // end ctor



BOOL
SOCmnPipe::create(
	void)
{
	if (m_name.isEmpty() == TRUE)
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - please set the pipe name")));
		return FALSE;
	} // end if

	SOCmnString pipeName;
	pipeName.format(_T("%s%s"), _T("\\\\.\\pipe\\"), m_name);

	if (!InitializeSecurityDescriptor(m_pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - could not initialize security descriptor for: %s"), m_name));
		return FALSE;
	} // end if

	if (!SetSecurityDescriptorDacl(m_pSecurityDescriptor, TRUE, NULL, FALSE))
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - could not set security descriptor on pipe: %s"), m_name));
		return FALSE;
	} // end i

	m_pSecurityAttributes->lpSecurityDescriptor = m_pSecurityDescriptor;

	while (TRUE)
	{
		m_pipe = CreateFile(pipeName, GENERIC_WRITE, 0, m_pSecurityAttributes, OPEN_EXISTING, 0, NULL);

		if (m_pipe != INVALID_HANDLE_VALUE)
		{
			// succesfully created
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - succesfully created pipe: %s"), m_name));
			break;
		} // end if

		if (getLastError() != ERROR_PIPE_BUSY)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - creating pipe: %s - failed with error 0x%x"), m_name, getLastError()));
			return FALSE;
		} // end if

		// all pipe instances are busy wait 2000 ms
		if (!WaitNamedPipe(pipeName, 2000))
		{
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::create - waited for pipe: %s"), m_name));
			return FALSE;
		} // end if
	} // end while

	return TRUE;
} // end create



BOOL
SOCmnPipe::connect(
	void)
{
	DWORD mode = PIPE_READMODE_MESSAGE;
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::connect - connecting to pipe: %s"), m_name));

	if (SetNamedPipeHandleState(m_pipe, &mode, NULL, NULL) == TRUE)
	{
		setConnected();
	}
	else
	{
		setConnected(FALSE);
	} // end if .. else

	return isConnected();
} // end connect



BOOL
SOCmnPipe::setMessage(
	BYTE* aMessage, DWORD aMessageSize)
{
	if (aMessage == NULL || aMessageSize > DEFAULT_PIPE_BUFFER_SIZE)
	{
		return FALSE;
	} // end if

	m_bufferSync.lock();

	m_bufferSize = aMessageSize;
	memcpy(m_buffer, aMessage, aMessageSize);

	m_bufferSync.unlock();
	return TRUE;
} // end setMessage



BOOL
SOCmnPipe::write(
	void)
{
	// start the internal handler thread
	m_writerThread->start(handleWriteThreadProc, this);
	return TRUE;
} // end write



BOOL
SOCmnPipe::handleWriteRequest(
	void)
{
	if (m_bufferSize == 0)
	{
		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::handleWriteRequest - writting to: %s failed - empty message buffer"), m_name));
		return FALSE;
	} // end if

	m_bufferSync.lock();
	if (create() == FALSE)
	{
		// failed to create the pipe
//		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnPipe::handleWriteRequest"), (_T("Named pipe: %s could not be created !"), m_name));
		m_bufferSync.unlock();
		return FALSE;
	} // end if

	if (connect() == FALSE)
	{
		// failed to connect to named pipe
		_TRACEC(SOCMNTRACE_L_WRN, SOCMNTRACE_G_ALL, _T("SOCmnPipe::handleWriteRequest"), (_T("Failed to connect to named named pipe: %s!"), m_name));
		m_bufferSync.unlock();
		return FALSE;
	} // end if

	DWORD written;

	if (WriteFile(m_pipe, m_buffer, m_bufferSize, &written, NULL) == TRUE)
	{
		_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::handleWriteRequest - writting to pipe: %s - buffer: %s"), m_name, m_buffer));
		m_bufferSync.unlock();
		return TRUE;
	} // end if

	_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::handleWriteRequest - writting to pipe: %s failed"), m_name));
	
	m_bufferSync.unlock();

	return FALSE;
} // end if



BOOL
SOCmnPipe::disconnect(
	void)
{
	if (m_writerThread->isRunning() == TRUE)
	{
		m_writerThread->getEvents()->signal(m_writerThread->getStopEventId());
	} // end if

	if (isConnected() == TRUE)
	{
		if (CloseHandle(m_pipe) == TRUE)
		{
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::disconnect - succesfully disconnected pipe: %s"), m_name));
			setConnected(FALSE);
			return TRUE;
		} // end if

		_TRACE(SOCMNTRACE_L_WRN, SOCMNTRACE_G_SOCKET, (_T("SOCmnPipe::disconnect - could not disconnect pipe: %s"), m_name));
	}

	return FALSE;
} // end disconnect



DWORD _stdcall SOCmnPipe::handleWriteThreadProc(
	void* pContext)
{
	if (pContext == NULL)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_SOCKET, _T("SOCmnPipe::handleTimeoutThreadProc"), (_T("NULL context passed in pipe's timeout handle thread")));
		return S_FALSE;
	} // end if

	SOCmnPipe* pPipe = reinterpret_cast<SOCmnPipe*>(pContext);

	if (pPipe == NULL)
	{
		return S_FALSE;
	} // end if

	pPipe->addRef();
	SOCmnThread* pMe = pPipe->getWriteThread();

	if (pMe == NULL)
	{
		pPipe->release();
		return S_FALSE;
	} // end if

	DWORD waitR;
	BOOL end = FALSE;
	DWORD dwWait = 5000;

	while (!end)
	{
		waitR = pMe->waitForEvents(dwWait);

		if (waitR == pMe->getStopEventId())
		{
			end = TRUE;
		}
		else //if (waitR == pPipe->getWriteEvent())
		{
			end = pPipe->handleWriteRequest();
		} // end if .. else
	} // end while

	pPipe->disconnect();
	pPipe->release();
	return S_OK;
} // end if

#endif // SOFEATURE_TP_DISCOVERY

#endif // SOOS_WINDOWS
