#ifndef _SOCMNTP_H_
#define _SOCMNTP_H_

#ifdef SOFEATURE_TUNNEL

#pragma pack(push, 4)


#if defined (TB_OEM_ABB) || defined (TB_OEM_ABB_FREELANCE)

#define TP_DEFAULT_PORT             58590 // default port number
#define TP_MAGIC_VALUE_TPV1    1094795874 // reads 'ABB>'
#define TP_PROTOCOL_VERSION             1 // the prototype will use PROTOCOL_VERSION 0.

#ifndef TP_MAGIC_VALUE_USE_SRV
#define TP_MAGIC_VALUE_USE_SRV     1094795874 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_USE_CLT
#define TP_MAGIC_VALUE_USE_CLT     1094795874 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_ADD_ACCEPT
#define TP_MAGIC_VALUE_ADD_ACCEPT  1094795874 // TP_MAGIC_VALUE_TPV1
#endif

#elif TB_OEM_DW // Device Wise OEM

#define TP_DEFAULT_PORT             55555 // default port number
#define TP_MAGIC_VALUE_TPV1    1414669281 // reads 'TPv1'
#define TP_PROTOCOL_VERSION             1 // the prototype will use PROTOCOL_VERSION 0.

#ifndef TP_MAGIC_VALUE_USE_SRV
#define TP_MAGIC_VALUE_USE_SRV     1414669281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_USE_CLT
#define TP_MAGIC_VALUE_USE_CLT     1414669281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_ADD_ACCEPT
#define TP_MAGIC_VALUE_ADD_ACCEPT  1414669281 // TP_MAGIC_VALUE_TPV1
#endif

#elif TB_OEM_IFM // ifm OEM

//	this is Softing genuine product
#define TP_DEFAULT_PORT             56567 // default port number
#define TP_MAGIC_VALUE_TPV1    1414559281 // reads 'TPv1'
#define TP_PROTOCOL_VERSION             1 // the prototype will use PROTOCOL_VERSION 0.

#ifndef TP_MAGIC_VALUE_USE_SRV
#define TP_MAGIC_VALUE_USE_SRV     1414559281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_USE_CLT
#define TP_MAGIC_VALUE_USE_CLT     1414559281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_ADD_ACCEPT
#define TP_MAGIC_VALUE_ADD_ACCEPT  1414559281 // TP_MAGIC_VALUE_TPV1
#endif

#else // Softing original

//	this is Softing genuine product
#define TP_DEFAULT_PORT             56567 // default port number
#define TP_MAGIC_VALUE_TPV1    1414559281 // reads 'TPv1'
#define TP_PROTOCOL_VERSION             1 // the prototype will use PROTOCOL_VERSION 0.

#ifndef TP_MAGIC_VALUE_USE_SRV
#define TP_MAGIC_VALUE_USE_SRV     1414559281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_USE_CLT
#define TP_MAGIC_VALUE_USE_CLT     1414559281 // TP_MAGIC_VALUE_TPV1
#endif

#ifndef TP_MAGIC_VALUE_ADD_ACCEPT
#define TP_MAGIC_VALUE_ADD_ACCEPT  1414559281 // TP_MAGIC_VALUE_TPV1
#endif




#endif  //  TB_OEM_ABB || TB_OEM_ABB_FREELANCE

// Flags used in the flags field of TP_INITIAL_MESSAGE_HEADER.
//
// Flag                              | Client request                            | Server response
// ----------------------------------+-------------------------------------------+-------------------------------
// TP_INIT_FLAG_NEW_SESSION_ID       | Request to establish a new session        | New session established
//                                   |                                           | Payload: TP_SESSION_ID
// ----------------------------------+-------------------------------------------+-------------------------------
// TP_INIT_FLAG_RESTORE_SESSION      | Request to reestablish a previous session | Previous session reestablished
//                                   | Payload: TP_SESSION_CONTEXT               |
// ----------------------------------+-------------------------------------------+-------------------------------
// TP_INIT_FLAG_CREDENTIALS_PROVIDED | Indicates presence of credentials         | na
//                                   | Payload: u_short credentials header       |
//                                   | followed by the actual credentials        |
// ----------------------------------+-------------------------------------------+-------------------------------
// TP_INIT_FLAG_USE_UTF16_STRINGS    | Use UTF16 strings instead of UTF8         | Use UTF16 strings instead of UTF8
//
// TP_INIT_FLAG_NEW_SESSION_ID and TP_INIT_FLAG_RESTORE_SESSION describe the way a session shall be established
// (client request) or the way a session has been established (server response). Combined in a client request
// they give the server the freedom to choose between them. The server will realize the one it regards the most
// feasible in its current state. The server response will have either none or exactly one of these flags set.
// No flag set means, the server has rejected the request.
//
// TP_INIT_FLAG_CREDENTIALS_PROVIDED is always combined with one of the session establishing flags in a client
// request and indicates credentials are contained in the request. The flag cannot be set in a server response.
//
// For to guarantee compatibility, the order in which payload follows the initial message header has to be in
// a way that older clients/servers never need to skip or even interpret payload of its newer counterpart. This
// means whenever you introduce new payload to the message, also introduce a flag that indicates the presence
// of this payload and add the actual payload behind any other payload that has been defined previously.
//
// The order in which payload has to be added to the message header and by which flag its presence is indicated
// is as follows:
//
// Client request: Payload data structure                                  | Indicator flag
//                 --------------------------------------------------------+----------------------------------
//                 TP_SESSION_CONTEXT                                      | TP_INIT_FLAG_RESTORE_SESSION
//                 u_short credentials type followed by actual credentials | TP_INIT_FLAG_CREDENTIALS_PROVIDED
//
// Server response: Payload data structure | Indicator flag
//                  -----------------------+-------------------------
//                  TP_SESSION_ID          | TP_INIT_FLAG_NEW_SESSION

#define TP_INIT_FLAG_NEW_SESSION_ID               0x0001
#define TP_INIT_FLAG_RESTORE_SESSION              0x0002
#define TP_INIT_FLAG_NOT_SUPPORT_UTF16_STRINGS    0x0010
#define TP_INIT_FLAG_USE_UTF16_STRINGS            0x0100
#define TP_INIT_FLAG_CREDENTIALS_PROVIDED         0x8000
#define TP_INIT_FLAG_USE_FULL_DATE_DEPRECATED     0x0200
#define TP_INIT_FLAG_USE_FULL_DATE                0x0400
#define TP_INIT_FLAG_ACCEPT_FULL_DATE             0x0800
#define TP_INIT_FLAG_PROTOCOL_MASK                0x0F00

#pragma pack(push,1)

struct TP_INITIAL_MESSAGE_HEADER
{
	u_long      message_len;
	u_long      magic_value;
	u_short     protocol_version;
	u_short     flags;
	u_long      reserved;
};

struct TP_SESSION_ID
{
	u_long      server_id[4];
};

struct TP_SESSION_CONTEXT
{
	TP_SESSION_ID   session_id;
	u_long          context_id;
};

#define TP_IDLE_MESSAGE_LEN             sizeof(u_long)

// flags in TP_BASIC_MESSAGE_HEADER
#define TP_MSG_FLAG_FUNCTION_RETURN     1
#define TP_MSG_FLAG_CONTEXT_CHANGE      2

struct TP_BASIC_MESSAGE_HEADER
{
	u_long      message_len;
	u_long      invoke_id;
	u_long      object_id;
	u_short     interface_id;
	u_char      function_id;
	u_char      flags;
};

#define TP_CREDENTIALS_TYPE_NONE             0
#define TP_CREDENTIALS_TYPE_USERNAME_PWDHASH 1

#define TP_DUMMY_VALUE_FILLED_IN_LATER           0xFFFFFFFF

#define TP_OBJECT_ID_CONNECTION_OBJECT           0

#define TP_INTERFACE_CMN_SERVER                  1
#define TP_INTERFACE_DA_SERVER                   2
#define TP_INTERFACE_DA_GROUP                    3
#define TP_INTERFACE_DA_CALLBACK                 4
#define TP_INTERFACE_AE_SERVER                   5
#define TP_INTERFACE_AE_SUBSCRIPTION             6
#define TP_INTERFACE_AE_AREA_BROWSER             7
#define TP_INTERFACE_AE_CALLBACK                 8

#define TP_FUNCTION_UNKNOWN_QUERY_SUPPORT        0

#define TP_FUNCTION_DA_SERVER_GETSTATUS          1
#define TP_FUNCTION_DA_SERVER_BROWSE             2
#define TP_FUNCTION_DA_SERVER_GETPROPERTIES      3
#define TP_FUNCTION_DA_SERVER_READ               4
#define TP_FUNCTION_DA_SERVER_WRITE              5
#define TP_FUNCTION_DA_SERVER_ADDGROUP           6
#define TP_FUNCTION_DA_SERVER_REMOVEGROUP        7
#define TP_FUNCTION_DA_SERVER_SETCLIENTNAME      8

#define TP_FUNCTION_DA_GROUP_ADDITEMS            1
#define TP_FUNCTION_DA_GROUP_REMOVEITEMS         2
#define TP_FUNCTION_DA_GROUP_SYNCREAD            3
#define TP_FUNCTION_DA_GROUP_SYNCWRITE           4
#define TP_FUNCTION_DA_GROUP_ASYNCREAD           5
#define TP_FUNCTION_DA_GROUP_ASYNCWRITE          6
#define TP_FUNCTION_DA_GROUP_VALIDATEITEMS       7
#define TP_FUNCTION_DA_GROUP_REFRESH             8

#define TP_FUNCTION_DA_CALLBACK_ONDATACHANGE     1
#define TP_FUNCTION_DA_CALLBACK_ONREADCOMPLETE   2
#define TP_FUNCTION_DA_CALLBACK_ONWRITECOMPLETE  3
#define TP_FUNCTION_DA_CALLBACK_ONCANCELCOMPLETE 4

#define TP_FUNCTION_AE_SERVER_GETSTATUS                 1
#define TP_FUNCTION_AE_SERVER_ADDSUBSCRIPTION   2
#define TP_FUNCTION_AE_SERVER_REMOVESUBSCRIPTION    3
#define TP_FUNCTION_AE_SERVER_QUERYEVENTCATEGORIES      5
#define TP_FUNCTION_AE_SERVER_QUERYCONDITIONNAMES       6
#define TP_FUNCTION_AE_SERVER_QUERYSUBCONDITIONNAMES    7
#define TP_FUNCTION_AE_SERVER_QUERYSOURCECONDITIONS     8
#define TP_FUNCTION_AE_SERVER_QUERYEVENTATTRIBUTES      9
#define TP_FUNCTION_AE_SERVER_ACKCONDITION              15
#define TP_FUNCTION_AE_SERVER_ADDAREABROWSER            16
#define TP_FUNCTION_AE_SERVER_REMOVEAREABROWSER         17
/* not used until now
#define TP_FUNCTION_AE_SERVER_QUERYAVAILABLEFILTERS     4
#define TP_FUNCTION_AE_SERVER_GETCONDITIONSTATE         10
#define TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYAREA     11
#define TP_FUNCTION_AE_SERVER_ENABLECONDITIONBYSOURCE   12
#define TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYAREA    13
#define TP_FUNCTION_AE_SERVER_DISABLECONDITIONBYSOURCE  14
not used until now */

#define TP_FUNCTION_AE_SUBSCRIPTION_SELECTRETURNEDATTRIBUTES    3
#define TP_FUNCTION_AE_SUBSCRIPTION_GETRETURNEDATTRIBUTES       4
#define TP_FUNCTION_AE_SUBSCRIPTION_REFRESH                     5
#define TP_FUNCTION_AE_SUBSCRIPTION_CANCELREFRESH               6
/* not used until now
#define TP_FUNCTION_AE_SUBSCRIPTION_SETFILTER                   1
#define TP_FUNCTION_AE_SUBSCRIPTION_GETFILTER                   2
#define TP_FUNCTION_AE_SUBSCRIPTION_GETSTATE                    7
#define TP_FUNCTION_AE_SUBSCRIPTION_SETSTATE                    8
not used until now */

#define TP_FUNCTION_AE_AREA_BROWSER_CHANGEBROWSEPOSITION        1
#define TP_FUNCTION_AE_AREA_BROWSER_BROWSEOPCAREAS              2
#define TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDAREANAME        3
#define TP_FUNCTION_AE_AREA_BROWSER_GETQUALIFIEDSOURCENAME      4

#define TP_FUNCTION_AE_CALLBACK_ONEVENT 1

#pragma pack(pop)

class SOCMN_EXPORT SOCmnTP_Marshaler : public SOCmnObject
{
protected:
	u_char* msg_buf;
	u_long msg_len;
	u_long alloc_len;
	u_long next_pos;
	u_long cur_pos;
	WORD m_flags;

	int ReserveSpace(u_long size, u_long align = 0);

public:
	SOCmnTP_Marshaler(WORD flags);
	~SOCmnTP_Marshaler();
	void BeginMarshal(u_long alloc_hint);
	u_char* GetMarshalData()
	{
		return msg_buf;
	}
	u_long GetMarshalDataLen()
	{
		return msg_len;
	}
	u_long GetWritePos()
	{
		return next_pos;
	}
	void SetWritePos(u_long offset)
	{
		next_pos = offset;
	}

	void MarshalChar(u_char value)
	{
		if (ReserveSpace(sizeof(value)))
		{
			msg_buf[cur_pos] = value;
		}
	}
	void MarshalShort(u_short value)
	{
		if (ReserveSpace(sizeof(value)))
		{
			*(u_short*)&msg_buf[cur_pos] = htons(value);
		}
	}
	void MarshalLong(u_long value)
	{
		if (ReserveSpace(sizeof(value)))
		{
			*(u_long*)&msg_buf[cur_pos] = htonl(value);
		}
	}
	void MarshalQuad(ULONGLONG value)
	{
		if (ReserveSpace(sizeof(value)))
		{
			((u_long*)&msg_buf[cur_pos])[0] = htonl((u_long)(value >> 32));
			((u_long*)&msg_buf[cur_pos])[1] = htonl((u_long)value);
		}
	}
	void MarshalFloat(FLOAT value)
	{
		MarshalLong(*(u_long*)&value);
	}
	void MarshalDouble(DOUBLE value)
	{
		MarshalQuad(*(ULONGLONG*)&value);
	}
	void MarshalDate(const SOCmnDateTime& value);
	void MarshalDate(const DATE& value);
	void MarshalBlob(const void* value, u_long size)
	{
		if (ReserveSpace(size, 1))
		{
			memcpy(&msg_buf[cur_pos], value, size);
		}
	}
	void MarshalString(LPCTSTR value);
	void MarshalBSTR(BSTR value);
	void MarshalDWORDList(const SOCmnDWORDList* value);
	void MarshalVariant(LPCVARIANT value);
	void MarshalSafeArray(LPSAFEARRAY value, VARTYPE vt = VT_EMPTY);
};

class SOCMN_EXPORT SOCmnTP_UnMarshaler : public SOCmnObject
{
protected:
	u_char* msg_buf;
	u_long msg_len;
	u_long next_pos;
	u_long cur_pos;
	WORD m_flags;

	int CheckSpace(u_long size, u_long align = 0);

public:
	SOCmnTP_UnMarshaler(WORD flags);
	~SOCmnTP_UnMarshaler();
	int SetMarshalData(u_char* data, u_long size);
	int AppendMarshalData(u_char* data, u_long size);
	u_char* GetMarshalData()
	{
		return msg_buf;
	}
	u_long GetMarshalDataLen()
	{
		return msg_len;
	}
	u_long GetReadPos()
	{
		return next_pos;
	}
	void SetReadPos(u_long offset)
	{
		next_pos = offset;
	}

	int UnMarshalChar(u_char* value)
	{
		if (!CheckSpace(sizeof(*value)))
		{
			return 0;
		}

		*value = msg_buf[cur_pos];
		return 1;
	}
	int UnMarshalChar(CHAR* value)
	{
		return UnMarshalChar((u_char*)value);
	}
	int UnMarshalShort(u_short* value)
	{
		if (!CheckSpace(sizeof(*value)))
		{
			return 0;
		}

		*value = ntohs(*(u_short*)&msg_buf[cur_pos]);
		return 1;
	}
	int UnMarshalShort(SHORT* value)
	{
		return UnMarshalShort((u_short*)value);
	}
	int UnMarshalLong(u_long* value)
	{
		if (!CheckSpace(sizeof(*value)))
		{
			return 0;
		}

		*value = ntohl(*(u_long*)&msg_buf[cur_pos]);
		return 1;
	}
	int UnMarshalLong(LONG* value)
	{
		return UnMarshalLong((u_long*)value);
	}
	int UnMarshalLong(INT* value)
	{
		return UnMarshalLong((u_long*)value);
	}
	int UnMarshalLong(UINT* value)
	{
		return UnMarshalLong((u_long*)value);
	}
	int UnMarshalArrayLen(u_long* value);
	int UnMarshalArrayLen(LONG* value)
	{
		return UnMarshalArrayLen((u_long*)value);
	}
	int UnMarshalArrayLen(INT* value)
	{
		return UnMarshalArrayLen((u_long*)value);
	}
	int UnMarshalArrayLen(UINT* value)
	{
		return UnMarshalArrayLen((u_long*)value);
	}
	int UnMarshalQuad(ULONGLONG* value)
	{
		if (!CheckSpace(sizeof(*value)))
		{
			return 0;
		}

		*value = ((ULONGLONG)ntohl(((u_long*)&msg_buf[cur_pos])[0]) << 32)
				 | ntohl(((u_long*)&msg_buf[cur_pos])[1]);
		return 1;
	}
	int UnMarshalQuad(LONGLONG* value)
	{
		return UnMarshalQuad((ULONGLONG*)value);
	}
	int UnMarshalFloat(FLOAT* value)
	{
		return UnMarshalLong((u_long*)value);
	}
	int UnMarshalDouble(DOUBLE* value)
	{
		return UnMarshalQuad((ULONGLONG*)value);
	}
	int UnMarshalDate(SOCmnDateTime* value);
	int UnMarshalDate(DATE* value);
	int UnMarshalBlob(void* value, u_long size)
	{
		if (!CheckSpace(size, 1))
		{
			return 0;
		}

		memcpy(value, &msg_buf[cur_pos], size);
		return 1;
	}
	int UnMarshalString(SOCmnString* value);
	int UnMarshalString(SOCmnStringEx* value);
	int UnMarshalBSTR(BSTR* value);
	int UnMarshalDWORDList(SOCmnDWORDList* value);
	int UnMarshalVariant(LPVARIANT value);
	int UnMarshalSafeArray(LPSAFEARRAY* value);
};


class SOCMN_EXPORT SOCmnTPCredentials : public SOCmnObject
{
public:
	SOCmnTPCredentials(SOCmnString strUserName, const BYTE* pbyPasswordData, u_short unPasswordLength);
	SOCmnTPCredentials(SOCmnString strUserName, SOCmnString strPasswordBinHex);
	virtual ~SOCmnTPCredentials();

	u_short getType();

	SOCmnString getUserName();

	u_short getPasswordLength();
	BYTE* getPasswordData();

	bool check(SOCmnTPCredentials* pCredentials);

protected:
	static BYTE* binHexToBinary(SOCmnString strBinHex, u_short& unCount);

	u_short m_unType;

	SOCmnString m_strUserName;

	BYTE*    m_pbyPasswordData;
	u_short m_unPasswordLength;
};


#define SOCMNTPCONNECTION_MAX_CALLS      101

class SOCmnTPCall;
class SOCmnTPReceiver;
class SOCMN_EXPORT SOCmnTPConnection : public SOCmnObject
{
public:
	SOCmnTPConnection();
	~SOCmnTPConnection();

	virtual bool init(SOCmnSocket* pSrv, SOCmnObject* pSocket);
	virtual void close(void)
	{
		handleClose();
	}
	virtual void handleClose(void);

	virtual void rcvData(IN LONG size, IN BYTE* pData);

	bool isInit(void)
	{
		return m_init;
	}

	SOCmnObject* getSocket(void)
	{
		SOCmnSingleLock<SOCmnSync> lock(&m_sync);
		m_sock.addRef();
		return m_sock;
	}

	virtual void addRequestToWorkQueue(SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId)
	{
		handleRequest(pCall, interfaceId, functionId);
	}
	virtual bool handleRequest(SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	virtual HRESULT invoke(SOCmnTPCall* pCall, BOOL waitForReply = TRUE);
	virtual void reply(SOCmnTPCall* pCall);

	virtual DWORD getNewInvokeId(void);

	virtual SOCmnList<SOCmnTPReceiver> *getReceiverList(void);

	static void sendReply(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId, IN HRESULT res);

	WORD getFlags(void)
	{
		return m_flags;
	}

protected:
	SOCmnSync m_sync;
	SOCmnPointer<SOCmnSocket> m_srv;
	SOCmnPointer<SOCmnObject> m_sock;
	SOCmnPointer<SOCmnTP_UnMarshaler> m_unmarshal;
	SOCmnList<SOCmnTPCall> m_callArray;
	DWORD m_invokeIdLoop;
	WORD m_flags;
	bool m_init;

	virtual bool handleInitMessage() = 0;
	virtual bool handleBasicMessage();
	virtual bool handlePingMessage();

	static void marshalInitialMessageHeader(SOCmnTP_Marshaler* pMarshaler, const TP_INITIAL_MESSAGE_HEADER& header);
	static bool unmarshalInitialMessageHeader(SOCmnTP_UnMarshaler* pUnmarshaler, TP_INITIAL_MESSAGE_HEADER& header);
	static void marshalSessionId(SOCmnTP_Marshaler* pMarshaler, const TP_SESSION_ID& sessionId);
	static bool unmarshalSessionId(SOCmnTP_UnMarshaler* pUnmarshaler, TP_SESSION_ID& sessionId);
	static void marshalSessionContext(SOCmnTP_Marshaler* pMarshaler, const TP_SESSION_CONTEXT& sessionContext);
	static bool unmarshalSessionContext(SOCmnTP_UnMarshaler* pUnmarshaler, TP_SESSION_CONTEXT& sessionContext);
	static void marshalCredentials(SOCmnTP_Marshaler* pMarshaler, SOCmnTPCredentials* pCredentials);
	static bool unmarshalCredentials(SOCmnTP_UnMarshaler* pUnmarshaler, SOCmnPointer<SOCmnTPCredentials> &pCredentials);
};

class SOCMN_EXPORT SOCmnTPCall : public SOCmnObject
{
public:
	SOCmnTPCall(SOCmnTPConnection* pCon);

	DWORD getTimeout(void)
	{
		return m_timeout;
	}
	void setTimeout(DWORD dw)
	{
		if (dw < SOCMN_TIME_40DAYS)
		{
			m_timeout = dw;
		}
		else
		{
			m_timeout = SOCMN_TIME_40DAYS;
		}
	}

	DWORD getActivationTime(void)
	{
		return m_activationTime;
	}

	SOCmnTP_Marshaler* getSndMsg(void)
	{
		m_sndMsg.addRef();
		return m_sndMsg;
	}
	SOCmnTP_UnMarshaler* getRcvMsg(void)
	{
		m_rcvMsg.addRef();
		return m_rcvMsg;
	}
	void setRcvMsg(SOCmnTP_UnMarshaler* pMsg)
	{
		m_rcvMsg = pMsg;
		m_rcvMsg.addRef();
	}

	SOCmnEvent* getCompleteEvent(void)
	{
		return &m_completeEvent;
	}

	HRESULT getResult(void)
	{
		return m_result;
	}
	void setResult(HRESULT res)
	{
		m_result = res;
	}
	DWORD getInvokeId(void)
	{
		return m_invokeId;
	}
	void setInvokeId(DWORD invokeId)
	{
		m_invokeId = invokeId;
	}
	DWORD getObjectId(void)
	{
		return m_objectId;
	}
	void setObjectId(DWORD objectId)
	{
		m_objectId = objectId;
	}

	void setTPConnection(IN SOCmnTPConnection* pConnection)
	{
		m_tpConnection = pConnection;
		m_tpConnection.addRef();
	}
	SOCmnTPConnection* getTPConnection(void)
	{
		m_tpConnection.addRef();
		return m_tpConnection;
	}

protected:
	SOCmnPointer<SOCmnTPConnection> m_tpConnection;
	DWORD m_timeout;
	DWORD m_activationTime;
	SOCmnPointer<SOCmnTP_Marshaler> m_sndMsg;
	SOCmnPointer<SOCmnTP_UnMarshaler> m_rcvMsg;
	SOCmnEvent m_completeEvent;
	HRESULT m_result;
	DWORD m_invokeId;
	DWORD m_objectId;

	virtual ~SOCmnTPCall(void);
};


class SOCMN_EXPORT SOCmnTPReceiver : public SOCmnObject
{
public:
	SOCmnTPReceiver(void);

	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);
	virtual void closeConnection(IN SOCmnTPConnection* pCon);

protected:
	virtual ~SOCmnTPReceiver(void);
};


#pragma pack(pop)

#endif // SOFEATURE_TUNNEL

#endif // _SOCMNTP_H_
