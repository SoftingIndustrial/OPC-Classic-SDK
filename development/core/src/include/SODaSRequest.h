#ifndef _SODASREQUEST_H_
#define _SODASREQUEST_H_

#include <opcda.h>
#include "SOCmnObject.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// Request Data (depends on I/O mode, operation and  type of the request)

class SODaSItemTag;
class SODaSRequest;
class SODaSCache;

#define SODASREQUEST_FLAG_QUALITY   0x01
#define SODASREQUEST_FLAG_TIMESTAMP 0x02

//-----------------------------------------------------------------------------
// SODaSRequestSyncReadData                                                   |
//-----------------------------------------------------------------------------
// synchron read data
class SODaSRequestSyncReadData
{
public:
	SODaSRequestSyncReadData(void);
	~SODaSRequestSyncReadData(void);

	// result of request
	void setResultAddress(IN HRESULT* resAddr);
	HRESULT getResult(void);
	void setResult(HRESULT res);

#ifdef SOFEATURE_DCOM
	// read buffer (only used for sync read operation)
	void setItemStateAddress(OPCITEMSTATE* itemStateAddr);
	OPCITEMSTATE* getItemStateAddress(void);
#endif

#ifdef SOFEATURE_DCOM
	void setReadValueAddress(LPVARIANT readValueAddr)
	{
		m_readValueAddr = readValueAddr;
	}
	LPVARIANT getReadValueAddress(void)
	{
		return m_readValueAddr;
	}
#endif
#ifdef SOFEATURE_SOAP
	void setReadValueAddress(SOCmnVariant* readValueAddr)
	{
		m_readValueVAddr = readValueAddr;
	}
	void getReadValueAddress(SOCmnVariant *&readValueAddr)
	{
		readValueAddr = m_readValueVAddr;
	}
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	void setReadValueAddress(SOCmnVariant* readValueAddr)
	{
		m_readValueVAddr = readValueAddr;
	}
	void getReadValueAddress(SOCmnVariant *&readValueAddr)
	{
		readValueAddr = m_readValueVAddr;
	}
#endif
#endif

	void setReadQualityAddress(WORD* readQualityAddr)
	{
		m_readQualityAddr = readQualityAddr;
	}
	WORD* getReadQualityAddress(void)
	{
		return m_readQualityAddr;
	}
#ifdef SOFEATURE_DCOM
	void setReadTimeStampAddress(FILETIME* readTimeStampAddr)
	{
		m_readTimeStampAddr = readTimeStampAddr;
	}
	FILETIME* getReadTimeStampAddress(void)
	{
		return m_readTimeStampAddr;
	}
#endif
#ifdef SOFEATURE_SOAP
	void setReadTimeStampAddress(SOCmnDateTime* readTimeStampAddr)
	{
		m_readTimeStampDTAddr = readTimeStampAddr;
	}
	void getReadTimeStampAddress(SOCmnDateTime *&pDT)
	{
		pDT = m_readTimeStampDTAddr;
	}
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	void setReadTimeStampAddress(SOCmnDateTime* readTimeStampAddr)
	{
		m_readTimeStampDTAddr = readTimeStampAddr;
	}
	void getReadTimeStampAddress(SOCmnDateTime *&pDT)
	{
		pDT = m_readTimeStampDTAddr;
	}
#endif
#endif

	void setMaxAge(DWORD maxAge)
	{
		m_maxAge = maxAge;
	}
	DWORD getMaxAge(void)
	{
		return m_maxAge;
	}

	void setReqDatatype(IN VARTYPE reqDatatype);
	VARTYPE getReqDatatype(void)
	{
		return m_reqDatatype;
	}

	// tag object
	BOOL setItemTag(IN SODaSItemTag* itemTag);
	SODaSItemTag* getItemTag(void);

	// cache object
	SODaSCache* getCache(void);

protected:
	HRESULT* m_resAddr;                      // address of result buffer (only for sync)
	SODaSItemTag* m_itemTag;         // item tag
	WORD* m_readQualityAddr;         // read quality
#ifdef SOFEATURE_DCOM
	OPCITEMSTATE* m_itemStateAddr;   // read output buffer (only for sync read)
	LPVARIANT m_readValueAddr;       // read value
	FILETIME* m_readTimeStampAddr;   // read timestamp
#endif
#ifdef SOFEATURE_SOAP
	SOCmnVariant* m_readValueVAddr;       // read value
	SOCmnDateTime* m_readTimeStampDTAddr; // read timestamp
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	SOCmnVariant* m_readValueVAddr;       // read value
	SOCmnDateTime* m_readTimeStampDTAddr; // read timestamp
#endif
#endif

	DWORD m_maxAge;                  // maximum age of cache value
	VARTYPE m_reqDatatype;           // requested datatype
}; // SODaSRequestSyncReadData


//-----------------------------------------------------------------------------
// SODaSRequestSyncWriteData                                                  |
//-----------------------------------------------------------------------------
// synchron write data
class SODaSRequestSyncWriteData
{
public:
	SODaSRequestSyncWriteData(void);
	~SODaSRequestSyncWriteData(void);

	// result of request
	void setResultAddress(IN HRESULT* resAddr);
	HRESULT getResult(void);
	void setResult(HRESULT res);

	// write buffer (only used for write operation)
	BOOL setWriteValue(IN LPCVARIANT value, IN LCID localeID, IN SODaSItemTag* tag, IN BOOL isSOAPRequest);
	BOOL getWriteValue(OUT LPVARIANT value);
	void setWriteQuality(WORD writeQuality)
	{
		m_writeQuality = writeQuality;
		m_validQT |= SODASREQUEST_FLAG_QUALITY;
	}
	WORD getWriteQuality(void)
	{
		return m_writeQuality;
	}
	void setWriteTimeStamp(const SOCmnDateTime& writeTimeStamp)
	{
		m_writeTimeStamp = writeTimeStamp;
		m_validQT |= SODASREQUEST_FLAG_TIMESTAMP;
	}
	void getWriteTimeStamp(SOCmnDateTime& writeTimeStamp)
	{
		writeTimeStamp = m_writeTimeStamp;
	}
	BYTE getValidQT(void)
	{
		return (BYTE)m_validQT;
	}

	// tag object
	BOOL setItemTag(IN SODaSItemTag* itemTag);
	SODaSItemTag* getItemTag(void);

	// cache object
	SODaSCache* getCache(void);

protected:
	HRESULT* m_resAddr;                       // address of result buffer (only for sync)
	SODaSItemTag* m_itemTag;          // item tag
	SOCmnVariant m_writeValue;        // value to write (only for write)
	WORD m_writeQuality;              // quality to write
	BOOL m_validQT;                   // valid flags for quality and timestamp
	SOCmnDateTime m_writeTimeStamp; // timestamp to write
}; // SODaSRequestSyncWriteData


//-----------------------------------------------------------------------------
// SODaSRequestAsyncReadData                                                  |
//-----------------------------------------------------------------------------
// asynchron read data
class SODaSRequestAsyncReadData
{
public:
	SODaSRequestAsyncReadData(void);
	~SODaSRequestAsyncReadData(void);

	// result of request
	HRESULT getResult(void)
	{
		return m_result;
	}
	void setResult(HRESULT res)
	{
		m_result = res;
	}

	void setMaxAge(DWORD maxAge)
	{
		m_maxAge = maxAge;
	}
	DWORD getMaxAge(void)
	{
		return m_maxAge;
	}

protected:
	HRESULT m_result;                   // result of request
	DWORD m_maxAge;                // maximum age of cache value
}; // SODaSRequestAsyncReadData


//-----------------------------------------------------------------------------
// SODaSRequestAsyncWriteData                                                 |
//-----------------------------------------------------------------------------
// asynchron write data
class SODaSRequestAsyncWriteData
{
public:
	SODaSRequestAsyncWriteData(void);
	~SODaSRequestAsyncWriteData(void);

	// result of request
	HRESULT getResult(void)
	{
		return m_result;
	}
	void setResult(HRESULT res)
	{
		m_result = res;
	}

	// write buffer (only used for write operation)
	BOOL setWriteValue(IN LPCVARIANT value, IN LCID localeID, IN SODaSItemTag* tag, IN BOOL isSOAPRequest);
	BOOL getWriteValue(OUT LPVARIANT value);
	void setWriteQuality(WORD writeQuality)
	{
		m_writeQuality = writeQuality;
		m_validQT |= SODASREQUEST_FLAG_QUALITY;
	}
	WORD getWriteQuality(void)
	{
		return m_writeQuality;
	}
	void setWriteTimeStamp(const SOCmnDateTime& writeTimeStamp)
	{
		m_writeTimeStampDT = writeTimeStamp;
		m_validQT |= SODASREQUEST_FLAG_TIMESTAMP;
	}
	void getWriteTimeStamp(SOCmnDateTime& writeTimeStamp)
	{
		writeTimeStamp = m_writeTimeStampDT;
	}
	BYTE getValidQT(void)
	{
		return (BYTE)m_validQT;
	}

protected:
	HRESULT m_result;                           // result of request
	SOCmnVariant m_writeValue;          // value to write (only for write)
	WORD m_writeQuality;              // quality to write
	BOOL m_validQT;                   // valid flags for quality and timestamp
	SOCmnDateTime m_writeTimeStampDT; // timestamp to write
}; // SODaSRequestAsyncWriteData


//-----------------------------------------------------------------------------
// SODaSRequestDelegateData                                                   |
//-----------------------------------------------------------------------------
// delegate I/O data
class SODaSRequestDelegateData
{
public:
	SODaSRequestDelegateData(void);
	~SODaSRequestDelegateData(void);

	// result of request
	HRESULT getResult(void);
	void setResult(HRESULT res);

	// original request (only used for delegate)
	BOOL setOriginalRequest(IN SODaSRequest* req);
	SODaSRequest* getOriginalRequest(void);

	// tag object
	BOOL setItemTag(IN SODaSItemTag* itemTag);
	SODaSItemTag* getItemTag(void);

	// cache object
	SODaSCache* getCache(void);

	// write data (only used for write operation)
	BOOL getWriteValue(OUT LPVARIANT value);
	WORD getWriteQuality(void);
	void getWriteTimeStamp(OUT SOCmnDateTime& timeStamp);
	BYTE getValidQT(void);

protected:
	HRESULT m_result;                   // result of request
	SODaSItemTag* m_itemTag;            // tag object
	SODaSRequest* m_request;            // starter request (only for delegate read)
}; // SODaSRequestDelegateData


//-----------------------------------------------------------------------------
// SODaSRequestReportData                                                     |
//-----------------------------------------------------------------------------
// report I/O data
class SODaSRequestReportData
{
public:
	SODaSRequestReportData(void);
	~SODaSRequestReportData(void);

	// result of request
	HRESULT getResult(void)
	{
		return m_result;
	}
	void setResult(HRESULT res)
	{
		m_result = res;
	}

	// report value, quality and time stamp (only used for report requests)
	BOOL setReportValue(IN SOCmnVariant& value)
	{
		if (value)
		{
			m_value = value;
			return TRUE;
		}

		return FALSE;
	}
	BOOL getReportValue(OUT SOCmnVariant& value)
	{
		if (value)
		{
			value.clear();
			m_value.copyTo(value);
			return TRUE;
		}

		return FALSE;
	}
	void setReportQuality(IN WORD quality)
	{
		m_quality = quality;
	}
	WORD getReportQuality(void)
	{
		return m_quality;
	}
	BOOL setReportTimeStamp(IN SOCmnDateTime& timeStamp)
	{
		m_timeStamp = timeStamp;
		return TRUE;
	}
	BOOL getReportTimeStamp(OUT SOCmnDateTime& timeStamp)
	{
		timeStamp = m_timeStamp;
		return TRUE;
	}

protected:
	HRESULT m_result;     // result of request
	SOCmnDateTime m_timeStamp; // time stamp
	SOCmnVariant m_value; // value
	WORD m_quality;       // quality
}; // SODaSRequestReportData




//-----------------------------------------------------------------------------
// SODaSRequest                                                               |
//-----------------------------------------------------------------------------

#define SODASREQUEST_FLAG_STARTED   0x01
#define SODASREQUEST_FLAG_PENDING   0x02
#define SODASREQUEST_FLAG_COMPLETED 0x04
#define SODASREQUEST_FLAG_CANCELED  0x08
#define SODASREQUEST_FLAG_PROCESSED 0x10
#define SODASREQUEST_FLAG_TOPROCESS 0x20
#define SODASREQUEST_FLAG_CACHE_AGE 0x40
#define SODASREQUEST_FLAG_SOAP      0x80

#define SODASREQUEST_TYPE_SYNC      0x0004
#define SODASREQUEST_TYPE_ASYNC     0x0008
#define SODASREQUEST_TYPE_DEVICE    0x0001
#define SODASREQUEST_TYPE_CACHE     0x0002
#define SODASREQUEST_TYPE_MAXAGE    0x0010
#define SODASREQUEST_TYPE_POLLED    0x0100
#define SODASREQUEST_TYPE_INIT_NOTIFY 0x0200
#define SODASREQUEST_TYPE_ITEM      0x1000
#define SODASREQUEST_TYPE_SERVER    0x2000
#define SODASREQUEST_TYPE_CYCLIC    0x8000
#define SODASREQUEST_TYPE_REPORT    0x4000

class SODaSRequest;
class SODaSItem;
class SODaSItemTag;
class SODaSCache;
class SODaSTransaction;
class SODaSGroup;
class SODaSServer;

class SODAS_EXPORT SODaSRequest :
	public SOCmnObject
{
	friend class SODaSTransaction;

public:
	// operation of request
	enum requestOperation
	{
		read,               // read request
		write               // write request
	};

	// type of request
	enum requestType
	{
		// synchron cache
		syncCache           = SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_CACHE | SODASREQUEST_TYPE_ITEM,
		// synchron device
		syncDevice        = SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_ITEM,
		// synchron maxage
		syncMaxAge        = SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_MAXAGE | SODASREQUEST_TYPE_ITEM,
		// asynchron cache
		asyncCache        = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_CACHE | SODASREQUEST_TYPE_ITEM,
		// asynchron device
		asyncDevice       = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_ITEM,
		// asynchron maxage
		asyncMaxAge       = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_MAXAGE | SODASREQUEST_TYPE_ITEM,
		// synchron property
		serverDevice      = SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_SERVER,
		// synchron itemIO
		serverMaxAge    = SODASREQUEST_TYPE_SYNC | SODASREQUEST_TYPE_MAXAGE | SODASREQUEST_TYPE_SERVER,
		// polled refresh
		polledRefresh   = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_CACHE | SODASREQUEST_TYPE_ITEM | SODASREQUEST_TYPE_POLLED,
		// initialisation notification
		asyncInitNotify = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_CACHE | SODASREQUEST_TYPE_ITEM | SODASREQUEST_TYPE_INIT_NOTIFY,
		// delegated device
		delegatedDevice = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_SERVER,
		// cyclic device
		cyclic              = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_DEVICE | SODASREQUEST_TYPE_ITEM | SODASREQUEST_TYPE_CYCLIC,
		// report
		report          = SODASREQUEST_TYPE_ASYNC | SODASREQUEST_TYPE_DEVICE |  SODASREQUEST_TYPE_ITEM | SODASREQUEST_TYPE_REPORT,
	};

	// request state
	enum requestState
	{
		stateInitialized = 0,
		stateStarted     = SODASREQUEST_FLAG_STARTED,
		statePending     = SODASREQUEST_FLAG_STARTED | SODASREQUEST_FLAG_PENDING,
		stateCompleted   = SODASREQUEST_FLAG_STARTED | SODASREQUEST_FLAG_COMPLETED,
		stateCanceled    = SODASREQUEST_FLAG_STARTED | SODASREQUEST_FLAG_COMPLETED | SODASREQUEST_FLAG_CANCELED,
		stateProcessed   = SODASREQUEST_FLAG_STARTED | SODASREQUEST_FLAG_COMPLETED | SODASREQUEST_FLAG_PROCESSED,
		stateProcessedCancel     = SODASREQUEST_FLAG_STARTED | SODASREQUEST_FLAG_COMPLETED | SODASREQUEST_FLAG_PROCESSED
	};

	SODaSRequest(IN BYTE operation, IN WORD type);

	// request type
	BOOL setType(IN enum requestType requestType);
	enum requestType getType(void) const;
	virtual BOOL isCacheRequest(void);
	virtual BOOL isDeviceRequest(void);

	virtual void setIsRefresh(void) { }

	// request operation
	BOOL setOperation(IN enum requestOperation Operation);
	enum requestOperation getOperation(void) const;

	// item object
	virtual BOOL setItem(IN SODaSItem* item);
	virtual SODaSItem* getItem(void);

	// tag object
	virtual BOOL setItemTag(IN SODaSItemTag* itemTag);
	virtual SODaSItemTag* getItemTag(void);

	// cache object
	virtual SODaSCache* getCache(void);

	// server object
	virtual BOOL setServer(IN SODaSServer* server);
	virtual SODaSServer* getServer(void);

	// start
	virtual BOOL start(IN OPTIONAL BOOL signalQueue = TRUE);
	BOOL isStarted(void) const;

	// complete
	virtual BOOL complete(IN OPTIONAL HRESULT result = S_OK,
						  IN OPTIONAL BOOL validWriteQuality = FALSE,
						  IN OPTIONAL WORD writeQuality = OPC_QUALITY_UNCERTAIN,
						  IN OPTIONAL BOOL errorBeforeStart = FALSE,
						  IN OPTIONAL BOOL writeChangeCache = TRUE);
	BOOL isCompleted(void) const;

	// pending
	virtual BOOL pending(void);
	BOOL isPending(void) const;

	// cancel
	virtual BOOL cancel(IN OPTIONAL HRESULT compResult = E_FAIL,
						IN OPTIONAL BOOL changeCacheQuality = FALSE,
						IN OPTIONAL WORD cacheQuality = OPC_QUALITY_LAST_KNOWN);
	BOOL isCanceled(void) const;

	// reset request object state and values
	void reset(void);

	// SOAP flag
	BYTE getFlag(void)
	{
		return m_flag;
	};
	void setFlag(BYTE flag)
	{
		m_flag |= flag;
	}
	void resetFlag(BYTE flag)
	{
		m_flag &= ~flag;
	}
	BOOL isSOAPRequest(void)
	{
		return ((m_flag & SODASREQUEST_FLAG_SOAP) != 0);
	}
	BOOL isDCOMRequest(void)
	{
		return ((m_flag & SODASREQUEST_FLAG_SOAP) == 0);
	}

	// start time
	DWORD getStartTime(void) const;

	// result of request
	virtual void setResultAddress(IN HRESULT* resAddr);
	virtual HRESULT getResult(void);
	virtual void setResult(HRESULT res);

	// read buffer (only used for sync read operation)
#ifdef SOFEATURE_DCOM
	virtual void setItemStateAddress(OPCITEMSTATE* itemStateAddr);
	virtual OPCITEMSTATE* getItemStateAddress(void);
#endif

	// read buffer (only used for sync server read operation)
#ifdef SOFEATURE_DCOM
	virtual void setReadValueAddress(LPVARIANT readValueAddr);
	virtual LPVARIANT getReadValueAddress(void);
#endif
#ifdef SOFEATURE_SOAP
	virtual void setReadValueAddress(SOCmnVariant* readValueAddr);
	virtual void getReadValueAddress(SOCmnVariant *&readValueAddr);
#endif

#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	virtual void setReadValueAddress(SOCmnVariant* readValueAddr);
	virtual void getReadValueAddress(SOCmnVariant *&readValueAddr);
#endif // SOFEATURE_SOAP
#endif // SOFEATURE_TUNNEL
	virtual void setReadQualityAddress(WORD* readQualityAddr);
	virtual WORD* getReadQualityAddress(void);
#ifdef SOFEATURE_DCOM
	virtual void setReadTimeStampAddress(FILETIME* readTimeStampAddr);
	virtual FILETIME* getReadTimeStampAddress(void);
#endif
#ifdef SOFEATURE_SOAP
	virtual void setReadTimeStampAddress(SOCmnDateTime* readTimeStampAddr);
	virtual void getReadTimeStampAddress(SOCmnDateTime *&pDT);
#endif
#ifdef SOFEATURE_TUNNEL
#ifndef SOFEATURE_SOAP
	virtual void setReadTimeStampAddress(SOCmnDateTime* readTimeStampAddr);
	virtual void getReadTimeStampAddress(SOCmnDateTime *&pDT);
#endif
#endif
	virtual void setMaxAge(DWORD maxAge);
	virtual DWORD getMaxAge(void);

	void setReqDatatype(IN VARTYPE reqDatatype);
	VARTYPE getReqDatatype(void);

	// write buffer (only used for write operation)
	virtual BOOL setWriteValue(IN LPCVARIANT value);
	virtual BOOL getWriteValue(OUT LPVARIANT value);
	virtual void setWriteQuality(WORD writeQuality);
	virtual WORD getWriteQuality(void);
	virtual void setWriteTimeStamp(const SOCmnDateTime& writeTimeStamp);
	virtual void getWriteTimeStamp(SOCmnDateTime& writeTimeStamp);
	virtual BYTE getValidWriteQT(void);

	// original request (only used for delegate)
	virtual BOOL setOriginalRequest(IN SODaSRequest* req);
	virtual SODaSRequest* getOriginalRequest(void);

	// report value, quality and time stamp (only used for report requests)
	virtual BOOL setReportValue(IN SOCmnVariant& value);
	virtual BOOL getReportValue(OUT SOCmnVariant& value);
	virtual BOOL setReportQuality(IN WORD quality);
	virtual WORD getReportQuality(void);
	virtual BOOL setReportTimeStamp(IN SOCmnDateTime& timeStamp);
	virtual BOOL getReportTimeStamp(OUT SOCmnDateTime& timeStamp);

	// completed event
	SOCmnEventPointer getCompletedEvent(void);
	void setCompletedEvent(SOCmnEventPointer event);

	// change between last value and cache
	virtual BOOL hasChanged(IN size_t connection, IN SOCmnVariant& value, IN WORD quality);

	// get the locale id to use for the request
	LCID getLCID(void);

	// object trace id
	virtual LPCTSTR getObjTraceId(void);

	static void signalQueueEvents(IN SOCmnList<SODaSRequest> *reqList);

protected:
	virtual ~SODaSRequest();

	WORD m_type;                        // request type
	BYTE m_operation;                   // request operation
	BYTE m_flag;                        // internal flag
	union
	{
		SODaSItem* m_item;                  // item object
		SODaSServer* m_server;
	};
	DWORD m_startTime;                  // start time in ms since system start
	SOCmnEventPointer m_completedEvent;         // event signals when request is completed
	union
	{
		HRESULT m_result;   // result
		void* m_depData;    // type and operation dependend data
		SODaSRequestSyncReadData* m_depSyncReadData;
		SODaSRequestSyncWriteData* m_depSyncWriteData;
		SODaSRequestAsyncReadData* m_depAsyncReadData;
		SODaSRequestAsyncWriteData* m_depAsyncWriteData;
		SODaSRequestDelegateData* m_depDelegateData;
		SODaSRequestReportData* m_depReportData;
	};

	virtual void startDelegateRequest(void);
	virtual BOOL completeDelegateRequest(void);
}; // SODaSRequest

inline BOOL SODaSRequest::setType(IN enum SODaSRequest::requestType requestType)
{
	m_type = (BYTE) requestType;
	return TRUE;
}

inline enum SODaSRequest::requestType SODaSRequest::getType(void) const
{
	return (SODaSRequest::requestType) m_type;
}

inline BOOL SODaSRequest::isCacheRequest(void)
{
	return (((m_type & SODASREQUEST_TYPE_CACHE) != 0) || (((m_type & SODASREQUEST_TYPE_MAXAGE) != 0) && (getMaxAge() == 0xFFFFFFFF)));
}

inline BOOL SODaSRequest::isDeviceRequest(void)
{
	return (((m_type & SODASREQUEST_TYPE_DEVICE) != 0) || (((m_type & SODASREQUEST_TYPE_MAXAGE) != 0) && (getMaxAge() == 0)));
}

inline BOOL SODaSRequest::setOperation(IN enum SODaSRequest::requestOperation operation)
{
	m_operation = (BYTE)operation;
	return TRUE;
}

inline enum SODaSRequest::requestOperation SODaSRequest::getOperation(void) const
{
	return (SODaSRequest::requestOperation) m_operation;
}

inline BOOL SODaSRequest::isStarted(void) const
{
	return (SODASREQUEST_FLAG_STARTED == (getObjectState() & SODASREQUEST_FLAG_STARTED));
}

inline BOOL SODaSRequest::isCompleted(void) const
{
	return (SODASREQUEST_FLAG_COMPLETED == (getObjectState() & SODASREQUEST_FLAG_COMPLETED));
}

inline BOOL SODaSRequest::isPending(void) const
{
	return (SODASREQUEST_FLAG_PENDING == (getObjectState() & SODASREQUEST_FLAG_PENDING));
}

inline BOOL SODaSRequest::isCanceled(void) const
{
	return (SODASREQUEST_FLAG_CANCELED == (getObjectState() & SODASREQUEST_FLAG_CANCELED));
}

inline DWORD SODaSRequest::getStartTime(void) const
{
	return m_startTime;
}

inline SOCmnEventPointer SODaSRequest::getCompletedEvent(void)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	return m_completedEvent;
}

inline void SODaSRequest::setCompletedEvent(SOCmnEventPointer event)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	m_completedEvent = event;
}

#define GenericRequest SODaSRequest

#pragma pack(pop)
#endif

