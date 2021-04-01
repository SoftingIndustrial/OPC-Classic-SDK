#ifndef _SODASTRANSACTION_H_
#define _SODASTRANSACTION_H_

#include "SOCmnObject.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SODaSTransaction                                                           |
//-----------------------------------------------------------------------------

#define SODASTRANSACTION_TYPE_READ  0x10
#define SODASTRANSACTION_TYPE_WRITE 0x20

#define SODASTRANSACTION_STATE_INACTIVE   0x01
#define SODASTRANSACTION_STATE_ACTIVE     0x02

#define SODASTRANSACTION_FLAG_NO_CANCEL_ON_START 0x01
#define SODASTRANSACTION_FLAG_NO_CLIENT_TRANSACTION_ID 0x08

class SODaSComGroup;
class SODaSServer;
class SODaSGroup;
class SODaSRequest;
class SODaSDataCallback;
#ifdef SOFEATURE_SOAP
class SODaSXmlMethod;
#endif

class SODAS_EXPORT SODaSTransaction : public SOCmnObject
{
	friend class SODaSServer;
	friend class SODaSGroup;
	friend class SODaSComGroup;

public:
	enum transactionState
	{
		inactive      = SODASTRANSACTION_STATE_INACTIVE,
		active        = SODASTRANSACTION_STATE_ACTIVE,
	};

	// type of OPC transaction
	enum transactionType
	{
		refresh       = SODASTRANSACTION_TYPE_READ  + 0x40, // refresh transaction
		cyclic        = SODASTRANSACTION_TYPE_READ  + 0x80, // cyclic transaction
		asyncRead     = SODASTRANSACTION_TYPE_READ  + 0x01, // asyncronous item read transaction
		asyncWrite    = SODASTRANSACTION_TYPE_WRITE + 0x01, // asyncronous item write transaction
		syncRead      = SODASTRANSACTION_TYPE_READ  + 0x02, // syncronous item read transaction
		syncWrite     = SODASTRANSACTION_TYPE_WRITE + 0x02, // syncronous item write transaction
		serverRead    = SODASTRANSACTION_TYPE_READ  + 0x04, // syncronous server read transaction
		serverWrite   = SODASTRANSACTION_TYPE_WRITE + 0x04, // syncronous server write transaction
		polledRefresh = SODASTRANSACTION_TYPE_READ  + 0x20  // polled refresh transaction
	};

	// type of client connections
	enum connectionType
	{
		dataObject,           // IDataObject (OPC DA V1.0)
		connectionPoint,    // IConnectionPoint (OPC DA V2.0 and DA V3.0)
		synchronous,      // synchronous
		asyncSOAPMethod,  // asynchronous performed SOAP method (OPC XML DA 1.0)
		tunnel,           // tunnel protocol (OPC Tunnel product)
		allConnections      // all connections
	};

	SODaSTransaction(IN SODaSGroup* group, IN BYTE type,
					 IN BYTE connectionType, IN OPTIONAL DWORD connection = 0);

	// server transaction id (== cancel id (OPC V2.0))
	DWORD getTransactionId(void);

	// client transaction id
	DWORD getClientTransactionId(void);
	void setClientTransactionId(DWORD transactionId);

	// transaction type
	transactionType getType(void);
	BOOL isCyclic(void) const
	{
		return (m_type == SODaSTransaction::cyclic);
	}
	BOOL isRead(void) const
	{
		return ((m_type & SODASTRANSACTION_TYPE_READ) == SODASTRANSACTION_TYPE_READ);
	}
	BOOL isWrite(void) const
	{
		return ((m_type & SODASTRANSACTION_TYPE_WRITE) == SODASTRANSACTION_TYPE_WRITE);
	}

	// generic group of transaction
	SODaSGroup* getGroup(void);

	// connection of transaction (0 for cyclic transactions)
	DWORD getConnection(void);

	// request list
	SOCmnList<SODaSRequest> *getRequestList(void);
	SOCmnSync* getRequestListLock(void);

	// request list handling
	BOOL insertRequest(IN SODaSRequest* req);
	BOOL removeRequest(IN SODaSRequest* req);

	SODaSRequest* getRequestByItem(IN SODaSItem* srcItem);
	DWORD getRequestCount(void);

	// actions on transaction
	virtual void start(IN SODaSServer* pServer);
	virtual BOOL update(IN SODaSServer* pServer, IN DWORD now, IN DWORD dueTime, OUT DWORD* span);
	virtual BOOL complete(IN SODaSServer* pServer);
	virtual BOOL cancel(SODaSServer* pServer);
	virtual void execute(IN SODaSServer* pServer, SOCmnEvents* pEvents, DWORD reqEventId, DWORD dueTime);

	virtual DWORD getTransactionTimeout(void)
	{
		return INFINITE;
	};

	virtual void timeoutTransaction(IN SODaSServer* pServer, IN DWORD now, IN DWORD dueTime, OUT DWORD* span);
	// implement this to return TRUE if you want the functionality enabled
	virtual BOOL getStopTraceOnTransactionTimeout(void)
	{
		return FALSE;
	};

	// timeout for the synchronous calls default value 1000 microseconds
	virtual DWORD getSyncTimeout(void);

	// actions on request list
	virtual void handleRequests(IN SOCmnList<SODaSRequest> *reqList);
	void cancelRequests(void);
	virtual void notiofyCompletedInternally(IN SODaSRequest* pRequest);

	// object trace id
	virtual LPCTSTR getObjTraceId(void);

	BOOL isStarted(void)
	{
		return ((getObjectState() & SODASTRANSACTION_STATE_ACTIVE) == SODASTRANSACTION_STATE_ACTIVE);
	}

	SOCmnEventPointer getCompletedEvent(void)
	{
		return m_completionEvent;
	}
	void setCompletedEvent(SOCmnEventPointer completionEvent)
	{
		m_completionEvent = completionEvent;
	}

	void getLastCompletionTime(SOCmnDateTime& ct)
	{
		ct = m_lastCompletionTime;
	}

	virtual void sendKeepAlive(void);
	void getLastSendDataChangeTime(SOCmnDateTime& ct)
	{
		ct = m_lastSendDataChangeTime;
	}
	void updateLastSendDataChangeTime(void)
	{
		m_lastSendDataChangeTime.now();
	}
	// implement this to return error code for the desired types of requests
	virtual HRESULT rejectRequests(IN SODaSServer* pServer);

#ifdef SOFEATURE_SOAP
	void setXMLMethod(IN SODaSXmlMethod* pMth);
	SODaSXmlMethod* getXMLMethod(void);

	void setReCheckSpan(IN DWORD span)
	{
		m_reCheckSpan = span;
	}
	DWORD getReCheckSpan(void)
	{
		return m_reCheckSpan;
	}

	void setWaitTime(IN DWORD time)
	{
		m_waitTime = time;
	}
	DWORD getWaitTime(void)
	{
		return m_waitTime;
	}

	void setHoldTime(IN DWORD time)
	{
		m_holdTime = time;
	}
	DWORD getHoldTime(void)
	{
		return m_holdTime;
	}
#endif

	void setFlag(BYTE flag)
	{
		m_flag = flag;
	}
	BYTE getFlag(void)
	{
		return m_flag;
	}

	DWORD getRequestType(void);

	virtual BOOL notifyAllChanges(void);

	// reimplement this in your child class if you wish to enable support for OPC buffering
	virtual BOOL checkItemTwice(void)
	{
		return TRUE;
	};

	void emptyRequestsCompletedEvent(void);

protected:
	virtual ~SODaSTransaction(void);

	SOCmnList<SODaSRequest> m_reqList;  // request list
	BYTE m_type;                        // transaction type
	BYTE m_connectionType;              // type of client connections
	BYTE m_flag;
	DWORD m_connection;                 // connection id
	SODaSGroup* m_group;                // generic group object

	DWORD m_transactionId;              // transaction id
	DWORD m_clientTransId;              // clients transaction id (OPC V2.0)
	SOCmnEventPointer m_completionEvent;
	SOCmnDateTime m_lastCompletionTime;
	SOCmnDateTime m_lastSendDataChangeTime;
	DWORD m_reqStartedCnt;

#ifdef SOFEATURE_SOAP
	SODaSXmlMethod* m_xmlMethod;
	DWORD m_waitTime;
	DWORD m_holdTime;
	DWORD m_reCheckSpan;
#endif

	BYTE m_traceAdditionOnUpdate;

	// server transaction id (== cancel id (OPC V2.0))
	void setTransactionId(DWORD transactionId);
#ifdef SOFEATURE_DCOM
	// send OPC data to the client
	virtual HRESULT sendReadData(IN SODaSDataCallback* advise);
	virtual HRESULT sendWriteData(IN SODaSDataCallback* advise);
#endif
#ifdef SOFEATURE_TUNNEL
	virtual HRESULT sendReadDataTP(IN SODaSDataCallback* advise);
	virtual HRESULT sendWriteDataTP(IN SODaSDataCallback* advise);
	virtual HRESULT sendKeepAliveTP(IN SODaSDataCallback* advise);
#endif

	// disconnect client of transaction
	void disconnectClient(void);
	virtual BOOL isSyncronisationNeeded(void)
	{
		return FALSE;
	}
}; // SODaSTransaction


inline DWORD SODaSTransaction::getTransactionId(void)
{
	return m_transactionId;
}

inline DWORD SODaSTransaction::getConnection(void)
{
	return m_connection;
}

inline void SODaSTransaction::setTransactionId(DWORD transactionId)
{
	m_transactionId = transactionId;
}

inline DWORD SODaSTransaction::getClientTransactionId(void)
{
	return m_clientTransId;
}

inline void SODaSTransaction::setClientTransactionId(DWORD transactionId)
{
	m_flag &= ~SODASTRANSACTION_FLAG_NO_CLIENT_TRANSACTION_ID;
	m_clientTransId = transactionId;
}

inline SODaSTransaction::transactionType SODaSTransaction::getType(void)
{
	return (SODaSTransaction::transactionType)m_type;
}

inline SOCmnList<SODaSRequest> *SODaSTransaction::getRequestList(void)
{
	return &m_reqList;
}

inline SOCmnSync* SODaSTransaction::getRequestListLock(void)
{
	return m_reqList.getSyncObject();
}

inline DWORD SODaSTransaction::getRequestCount(void)
{
	SOCmnList<SODaSRequest> reqList(getRequestList());
	return reqList.getCount();
}

#define OPCTransaction SODaSTransaction

#pragma pack(pop)
#endif

