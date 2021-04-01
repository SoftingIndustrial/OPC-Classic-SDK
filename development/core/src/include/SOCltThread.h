#ifndef _SOCLTWORKERTHREAD_H_
#define _SOCLTWORKERTHREAD_H_

#include "SOCltTask.h"

#ifdef SOFEATURE_DCOM

#if _MSC_VER == 1100

extern const CLSID CLSID_StdGlobalInterfaceTable;

#ifndef __IGlobalInterfaceTable_FWD_DEFINED__
#define __IGlobalInterfaceTable_FWD_DEFINED__
typedef interface IGlobalInterfaceTable IGlobalInterfaceTable;
#endif  /* __IGlobalInterfaceTable_FWD_DEFINED__ */

#ifndef __IGlobalInterfaceTable_INTERFACE_DEFINED__
#define __IGlobalInterfaceTable_INTERFACE_DEFINED__

typedef /* [unique] */ IGlobalInterfaceTable __RPC_FAR* LPGLOBALINTERFACETABLE;

EXTERN_C const IID IID_IGlobalInterfaceTable;

MIDL_INTERFACE("00000146-0000-0000-C000-000000000046")
IGlobalInterfaceTable :
public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE RegisterInterfaceInGlobal(
		/* [in] */ IUnknown __RPC_FAR* pUnk,
		/* [in] */ REFIID riid,
		/* [out] */ DWORD __RPC_FAR* pdwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE RevokeInterfaceFromGlobal(
		/* [in] */ DWORD dwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetInterfaceFromGlobal(
		/* [in] */ DWORD dwCookie,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppv) = 0;

};

#endif
#endif

#endif // SOFEATURE_DCOM

#pragma pack(push, 4)


#ifdef SOFEATURE_DCOM

//-----------------------------------------------------------------------------
// SOCltInterfaceAccess                                                       |
//-----------------------------------------------------------------------------

#ifndef _ATL_NO_UUIDOF
#define _ATL_IIDOF(x) __uuidof(x)
#else
#define _ATL_IIDOF(x) IID_##x
#endif

template <class T, const IID* piid>
class SOCltInterfaceAccess
{
public:
	SOCltInterfaceAccess()
	{
		m_cookie = 0;
		m_creatorThread = 0;
	}

	~SOCltInterfaceAccess()
	{
		if (m_ptr != NULL)
		{
			m_ptr = NULL;
		}
		_ASSERTION(!m_cookie, "Cookie left");
	}

	HRESULT putInterface(T* p, BOOL connectionMonitorStatus)
	{
		HRESULT res = S_OK;
		CComPtr<IGlobalInterfaceTable> pGIT;

		if (0 != m_cookie || NULL != p)
		{
			res = ::CoCreateInstance(CLSID_StdGlobalInterfaceTable,
									 NULL,
									 CLSCTX_INPROC_SERVER,
									 IID_IGlobalInterfaceTable,
									 (void**)&pGIT);
			_ASSERTION(SUCCEEDED(res) , "Failed to create Global Interface Table");

			if (FAILED(res))
			{
				return res;    // return error, if GIT can't be instanziated
			}
		}

		if (0 != m_cookie)
		{
			_ASSERTION(::GetCurrentThreadId() == m_creatorThread, "Wrong thread");
			res = pGIT->RevokeInterfaceFromGlobal(m_cookie);
			m_creatorThread = 0;
			m_cookie = 0;
			if (!connectionMonitorStatus)
			{
				m_ptr = NULL;
			}
		}

		res = S_OK;
		if ((m_ptr != NULL) && (!connectionMonitorStatus))
		{
			m_ptr = NULL;
		}

		if (p)
		{
			res = pGIT->RegisterInterfaceInGlobal(p, *piid, &m_cookie);

			if (SUCCEEDED(res))
			{
				m_ptr = p;
				m_creatorThread = ::GetCurrentThreadId();
			}
		}

		return res;
	}

	HRESULT getInterface(T** ppv)
	{
		_ASSERTION(ppv, "NULL pointer passed");
		HRESULT res = S_OK;
		*ppv = NULL;

		if (!m_ptr)
		{
			return E_NOINTERFACE;
		}

		if (::GetCurrentThreadId() == m_creatorThread)
		{
			(*ppv = m_ptr)->AddRef();
			res = S_OK;
		}
		else
		{
			CComPtr<IGlobalInterfaceTable> pGIT;
			res = ::CoCreateInstance(CLSID_StdGlobalInterfaceTable,
									 NULL,
									 CLSCTX_INPROC_SERVER,
									 IID_IGlobalInterfaceTable,
									 (void**)&pGIT);
			_ASSERTION(SUCCEEDED(res) , "Failed to create Global Interface Table");

			if (SUCCEEDED(res))
			{
				res = pGIT->GetInterfaceFromGlobal(m_cookie, *piid, (void**)ppv);
			}
		}

		return res;
	}

	void operator=(T* p)
	{
		putInterface(p, FALSE);
	}

	BOOL operator !(void)
	{
		return !m_ptr;
	}

	BOOL isAvailable(void)
	{
		return (m_ptr != NULL);
	}

protected:
	CComPtr<T> m_ptr;
	DWORD m_cookie;
	DWORD m_creatorThread;
}; // SOCltInterfaceAccess

#endif // SOFEATURE_DCOM


//-----------------------------------------------------------------------------
// SOCltWorkerThread                                                          |
//-----------------------------------------------------------------------------
class SOCLT_EXPORT SOCltWorkerThread : public SOCmnThread
{
public:
	SOCltWorkerThread();
	virtual ~SOCltWorkerThread();

	void shutdown(IN DWORD timeout = INFINITE);

	// Shedule a task for execution in worker thread's contexts
	BOOL addTask(IN SOCltTask* task, IN BOOL wakeup = TRUE);

	// Shedule a recurrent task
	BOOL addCyclicTask(IN SOCltCyclicTask* task, OUT DWORD* cookie = NULL);

	// Remove a recurrent task
	BOOL removeCyclicTask(IN DWORD cookie);

	// Get a recurrent task
	SOCltCyclicTask* getCyclicTask(IN DWORD cookie);

	// Signal worker thread
	void wakeUp();
	void wakeUpCyclic();

#ifdef SOOS_WINDOWS
	// Get thread ID
	DWORD getThreadID()
	{
		return m_id;
	};

	// Get thread HANDLE
	HANDLE getThreadHandle()
	{
		return m_handle;
	};

#endif // SOOS_WINDOWS

#ifdef SOOS_LINUX
	// Get thread HANDLE
	pthread_t getThreadHandle()
	{
		return m_thread;
	};
	// Get thread ID
	pthread_t getThreadID()
	{
		return m_thread;
	};

#endif // SOOS_LINUX

	// set worker thread's priority
	BOOL setThreadPriority(IN int priority);

	void setErrorCallback(IN SOCltElementIError* error);

protected:
	void cleanUp(void);

	SOCmnList<SOCltTask>       m_taskQueue;   // task queue
	SOCmnList<SOCltCyclicTask> m_cyclicQueue;   // cyclic task queue
	DWORD m_evtWakeup;
	DWORD m_evtCyclic;
	SOCmnThread m_workThread;                           // handle of workerthread

	// worker thread main routine
	DWORD work(void);

	SOCltElementIError* m_error;
};


inline void SOCltWorkerThread::setErrorCallback(IN SOCltElementIError* error)
{
	m_error = error;
}


#pragma pack(pop)

#endif // _SOCLTWORKERTHREAD_H_

