#ifndef _SOCMNPIPE_H_
#define _SOCMNPIPE_H_

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


#ifdef SOOS_WINDOWS

#ifdef SOFEATURE_TP_DISCOVERY

#define DEFAULT_PIPE_BUFFER_SIZE 4096

class SOCmnPipe : public SOCmnObject
{
public:
	SOCmnPipe(SOCmnObject* parent = NULL);
	SOCmnPipe(LPCTSTR aName);

	virtual BOOL create(void);

	virtual BOOL connect(void);
	virtual BOOL disconnect(void);

	virtual BOOL setMessage(BYTE* aMessage, DWORD aMessageSize);
	virtual BOOL write(void);

	SOCmnThread* getWriteThread(void)
	{
		return m_writerThread;
	};
protected:
	virtual ~SOCmnPipe(void);

	void setConnected(BOOL aFlag = TRUE)
	{
		m_connected = aFlag;
	};
	BOOL isConnected(void)
	{
		return m_connected;
	};

	HANDLE getPipeHandle(void)
	{
		return m_pipe;
	};

	void setName(LPCTSTR aName)
	{
		m_name.format(_T("%s"), aName);
	};
	LPCTSTR getName(void)
	{
		return m_name;
	};

	DWORD getLastError(void)
	{
		m_lastError = GetLastError();
		return m_lastError;
	};

	BYTE* getBuffer(void) { if (m_bufferSize != 0) return m_buffer; return NULL; };
	BOOL handleWriteRequest(void);
	static DWORD _stdcall handleWriteThreadProc(void* context);
private:
	HANDLE m_pipe;
	SOCmnString m_name;
   BYTE m_buffer[DEFAULT_PIPE_BUFFER_SIZE];
   DWORD m_bufferSize;

   SOCmnSync m_bufferSync;
	DWORD m_lastError;
	BOOL m_connected;

	SOCmnThread* m_writerThread;
	LPSECURITY_ATTRIBUTES m_pSecurityAttributes;
	PSECURITY_DESCRIPTOR m_pSecurityDescriptor;
};

#endif // SOFEATURE_TP_DISCOVERY
#endif // SOOS_WINDOWS
#endif // _SOCMNPIPE_H_
