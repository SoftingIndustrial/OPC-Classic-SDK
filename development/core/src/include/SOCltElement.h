#ifndef _SOCLTELEMENT_H_
#define _SOCLTELEMENT_H_

#include "SOCmnElement.h"

#pragma pack(push, 4)

#ifdef _MSC_VER
#pragma warning( disable : 4407 )
#endif

//-----------------------------------------------------------------------------
// SOCltElementIError                                                         |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltElementIError
{
public:
	SOCltElementIError();
	virtual ~SOCltElementIError();

	// last error
	virtual HRESULT getLastError(OUT OPTIONAL DWORD* errorcode = NULL);
	virtual BOOL setLastError(IN HRESULT res, IN DWORD errorcode);
	virtual BOOL getErrorDescription(IN DWORD errorcode, OUT SOCmnString& str, IN LCID lcid);

protected:
	virtual void onError(IN HRESULT res, IN DWORD errorcode);

	virtual LPCTSTR getIntTraceId(void);
	virtual SOCmnObject* getObjectCltElementIError(void);
}; // SOCltElementIError




//-----------------------------------------------------------------------------
// SOCltElementErrorStore                                                     |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltElementErrorStore : virtual public SOCltElementIError
{
public:
	SOCltElementErrorStore();

	// last error
	virtual HRESULT getLastError(OUT DWORD* errorcode);
	virtual BOOL setLastError(IN HRESULT res, IN DWORD errorcode);

protected:
	HRESULT m_lastError;            // includes the last system error
	DWORD m_lastErrorCode;          // Toolkit error code of last error
}; // SOCltElementErrorStore




//-----------------------------------------------------------------------------
// SOCltElement                                                               |
//-----------------------------------------------------------------------------

// object state flags for server, group and item objects
#define SOCLT_STATE_FLAG_CONNECTED      0x01
#define SOCLT_STATE_FLAG_STARTED          0x02

#define SOCLT_EXEC_ASYNC       FALSE
#define SOCLT_EXEC_AUTO_SYNC   TRUE

class SOCltStorage;
class SOCltWorkerThread;
class SOCltTask;


class SOCLT_EXPORT SOCltElement :
	public SOCmnElement,
	virtual public SOCltElementIError
{
	friend class SOCltTaskStateTransition;
	friend class SOCltTaskUpdate;
	friend class SOCltTaskConnectionMonitor;

public:
	SOCltElement(IN DWORD objType);

	enum elementState
	{
		disconnected = 0,
		connected    = SOCLT_STATE_FLAG_CONNECTED,
		started      = SOCLT_STATE_FLAG_CONNECTED | SOCLT_STATE_FLAG_STARTED
	};

	// used for identifying the item, if the state change transaction reffers to a single item
	void setSingleItemTransactionHandle(DWORD pElementHandle);
	DWORD getSingleItemTransactionHandle();

	// overrides
	virtual BOOL setObjectState(IN BYTE state);
	virtual BOOL setTargetObjectState(IN BYTE state);
	virtual BOOL setParentBranch(IN SOCmnElement* parent);  // parent branch

	// state transition
	virtual LONG start(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);
	virtual LONG connect(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);
	virtual LONG disconnect(IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);
	virtual LONG performStateTransition(IN OPTIONAL BOOL deep = TRUE, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	BOOL isConnected(void);     // is object connected?
	BOOL isStarted(void);           // is object started?

#ifdef SOFEATURE_DCOM
	// query COM interface
	virtual HRESULT queryInterface(IN REFIID iid, OUT void** interf);
#endif

	BOOL getConnectionMonitorStatus(void);
	void setConnectionMonitorStatus(BOOL connectionMonitorStatus);

	// update attributes
	virtual LONG update(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters = 0xFFFFFFFF, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

	// serialization
	virtual BOOL serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring);

	// object persistence
	BOOL setPersistence(IN BOOL persist);
	BOOL getPersistence(void) const;

	virtual SOCltWorkerThread* getWorkerThread(void);

	virtual void beforeInterfaceCall(void);
	virtual void afterInterfaceCall(void);

	virtual void connectionFailure(void);

	// execute task
	LONG executeTask(IN SOCltTask* task, IN OPTIONAL BOOL waitopt = SOCLT_EXEC_AUTO_SYNC, IN OPTIONAL SOCmnEventPointer handle = NULL);

protected:

	BYTE m_persist;                     // should object be stored in configuration?

	// Implementation of state transitions
	virtual HRESULT doStateTransition(BOOL deep, BYTE stateDeep);

	// possibility to adapt the actions started at state transition
	virtual BOOL performStateTransitionAction(DWORD action);

	// Recurse children at state transition
	virtual HRESULT doRecurseChildren(SOCmnList<SOCmnElement>* plist, BYTE state);

	// Implementation of the update
	virtual HRESULT doUpdate(IN BOOL fromServer, IN OPTIONAL DWORD whatParameters);

	// Force the object to disconnect
	virtual HRESULT doDisconnect();

	virtual SOCmnObject* getObjectCltElementIError(void);

	virtual ~SOCltElement();

private:
	// holds Connection Monitor status flag
	BOOL m_connectionMonitorStatus;
	// holds the item handle if the state change transaction reffers to a single item
	DWORD m_singleItemTransactionHandle;

}; // SOCltElement

inline BOOL SOCltElement::isConnected(void)
{
	return ((getObjectState() & connected) == connected);
}

inline BOOL SOCltElement::getConnectionMonitorStatus(void)
{
	return m_connectionMonitorStatus;
}

inline void SOCltElement::setConnectionMonitorStatus(BOOL connectionMonitorStatus)
{
	m_connectionMonitorStatus = connectionMonitorStatus;
}

inline BOOL SOCltElement::isStarted(void)
{
	return ((getObjectState() & started) == started);
}

inline BOOL SOCltElement::getPersistence(void) const
{
	return m_persist;
}

inline BOOL SOCltElement::setPersistence(IN BOOL persist)
{
	m_persist = (BYTE)persist;
	return TRUE;
}



#ifdef SOOS_WINDOWS

//-----------------------------------------------------------------------------
// SOCltCallback                                                              |
//-----------------------------------------------------------------------------

typedef void (SOCmnObject::*SOCLTELEMENT_CALLBACKTYPE)(void);
typedef void (SOCmnObject::*SOCLTELEMENT_CALLBACKTYPE_ERROR)(HRESULT, DWORD);
typedef SOCmnObject* SOCLTELEMENT_VTBL;

class SOCLT_EXPORT SOCltCallback : public SOCmnData
{
public:
	SOCltCallback()
	{
		e = 0;
		vtlb = 0;
		type = 0;
	}

	virtual void execute(void) const
	{
		_ASSERTION(0 != vtlb || 0 != e , "Invalid callback-data");

		switch (type)
		{
		default:
			_ASSERTION(FALSE, "Invalid callback type");
			break;

		case 0 :
			(vtlb->*f)();
			break;

		case 1 :
			(vtlb->*f1.ferr)(f1.dw1, f1.dw2);
			break;
		}
	}

	DWORD type;
	union // switch[type]
	{
		SOCLTELEMENT_CALLBACKTYPE f;

		struct _f1
		{
			SOCLTELEMENT_CALLBACKTYPE_ERROR ferr;
			DWORD dw1;
			DWORD dw2;
		} f1;
	};

	SOCmnObject* e;                                 // pointer to object
	SOCLTELEMENT_VTBL vtlb;                 // v-table, may be different from the default one
	// if multiple inheritance is used
	virtual ~SOCltCallback(void)
	{
		if (e)
		{
			e->release();
		}
	}
}; // SOCltCallback

void SOCLT_EXPORT SOCltInvokeCallback(const SOCltCallback& clb);

// On this pointer
#define INVOKE_CALLBACK( cls, fcn )                                         \
        { SOCltCallback _clb;                                                               \
            (_clb.e = this)->addRef();                                              \
            _clb.vtlb = (SOCLTELEMENT_VTBL)(cls##*)this;            \
            _clb.f = (SOCLTELEMENT_CALLBACKTYPE)&cls##::fcn ;   \
            SOCltInvokeCallback(_clb);}                                             \
 

// On this pointer, virtual public class
#define INVOKE_CALLBACK_ON( vp, cls, fcn )                          \
        { if(vp) { SOCltCallback _clb;                                          \
            (_clb.e = vp)->addRef();                                                    \
            _clb.vtlb = (SOCLTELEMENT_VTBL)(cls##*)this;            \
            _clb.f = reinterpret_cast<SOCLTELEMENT_CALLBACKTYPE>(&cls##::fcn) ; \
            SOCltInvokeCallback(_clb);}}                                            \
 
// On object pointer
#define INVOKE_CALLBACK_ON_PTR( obj, cls, fcn )                 \
        { if(obj) { SOCltCallback _clb;                                         \
            (_clb.e = obj)->addRef();                                                   \
            _clb.vtlb = (SOCLTELEMENT_VTBL)(cls##*)obj;             \
            _clb.f = (SOCLTELEMENT_CALLBACKTYPE)(&cls##::fcn) ; \
            SOCltInvokeCallback(_clb);}}                                            \
 
#endif // SOOS_WINDOWS
#ifdef SOOS_LINUX

// On this pointer
#define INVOKE_CALLBACK( cls, fcn )                                         \
        { fcn(); }

// On this pointer, virtual public class
#define INVOKE_CALLBACK_ON( vp, cls, fcn )                          \
        { fcn(); }

// On object pointer
#define INVOKE_CALLBACK_ON_PTR( obj, cls, fcn )                 \
        { obj->fcn(); }

#endif // SOOS_LINUX

#pragma pack(pop)
#endif

