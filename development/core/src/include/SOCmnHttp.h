#ifndef _SOCMNHTTP_H_
#define _SOCMNHTTP_H_

#ifdef SOFEATURE_CMN_HTTP

#pragma pack(push, 4)


#define SOCMNHTTP_FAIL 4
#define SOCMNHTTP_WS   3
#define SOCMNHTTP_EOR  2
#define SOCMNHTTP_EOL  1
#define SOCMNHTTP_OK   0

#define SOCMNHTTP_METHOD_GET     0x0001
#define SOCMNHTTP_METHOD_HEAD    0x0002
#define SOCMNHTTP_METHOD_POST    0x0004
#define SOCMNHTTP_METHOD_PUT     0x0008
#define SOCMNHTTP_METHOD_TRACE   0x0010
#define SOCMNHTTP_METHOD_OPTIONS 0x0020
#define SOCMNHTTP_METHOD_DELETE  0x0030

#define SOCMNHTTP_CODE_CONTINUE              100
#define SOCMNHTTP_CODE_OK                    200
#define SOCMNHTTP_CODE_BAD_REQUEST           400
#define SOCMNHTTP_CODE_UNAUTHORIZED          401
#define SOCMNHTTP_CODE_FORBIDDEN             403
#define SOCMNHTTP_CODE_NOT_FOUND             404
#define SOCMNHTTP_CODE_METHOD_NOT_ALLOWED    405
#define SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR 500

#define SOCMNHTTP_CODEI_NOT_HANDLED        0xffff

//-----------------------------------------------------------------------------
// SOCmnHTTPHandler                                                           |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnHTTPHandler : public SOCmnObject
{
public:
	SOCmnHTTPHandler(void);

	static DWORD skipSpaces(CHAR** pCur);
	static DWORD getToken(DWORD* pLen, CHAR** pToken, CHAR** pCur);
	static DWORD getLine(DWORD* pLen, CHAR** pToken, CHAR** pCur);
	static DWORD nextLine(CHAR** pCur);
	static DWORD endOfRequest(BYTE** pData);

	static DWORD isHTTPMethod(DWORD lenToken, CHAR* pToken, WORD* pHTTPMethod);
	static DWORD isHTTPVersion(DWORD lenToken, CHAR* pToken, WORD* pHTTPVersion);

	static LPCTSTR getHTTPMethodString(IN WORD method);

	virtual DWORD decodeHTTPRequestHeader(DWORD dataSize, BYTE* pData, BYTE** pBodyS);
	virtual DWORD decodeHTTPResponseHeader(DWORD dataSize, BYTE* pData, BYTE** pBodyS);

	WORD getHTTPMethod(void)
	{
		return m_httpMethod;
	}
	WORD getHTTPVersion(void)
	{
		return m_httpVersion;
	}
	WORD getHTTPStatusCode(void)
	{
		return m_httpStatusCode;
	}
	void setHTTPStatusCode(WORD s)
	{
		m_httpStatusCode = s;
	}
	LPCTSTR getURL(void)
	{
		return m_httpURL;
	}
	DWORD getContentsLength(void);
	SOCmnStringList* getAttributeList(void)
	{
		return &m_attributeList;
	}

protected:
	virtual ~SOCmnHTTPHandler(void);

	WORD m_httpMethod;
	WORD m_httpVersion;
	WORD m_httpStatusCode;
	SOCmnString m_httpURL;
	SOCmnStringList m_attributeList;
}; // SOCmnHTTPHandler

#pragma pack(pop)

#endif // SOFEATURE_CMN_HTTP

#endif
