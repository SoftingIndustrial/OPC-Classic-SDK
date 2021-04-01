#include "stdafx.h"

#ifdef SOFEATURE_CMN_HTTP

#include "SOCmn.h"
#include "SOCmnHttp.h"


//-----------------------------------------------------------------------------
// SOCmnHTTPHandler                                                           |
//-----------------------------------------------------------------------------

SOCmnHTTPHandler::SOCmnHTTPHandler(void)
{
	m_httpMethod = 0;
	m_httpStatusCode = 0;
	m_attributeList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
}

SOCmnHTTPHandler::~SOCmnHTTPHandler(void)
{
	m_attributeList.destroy();
}

DWORD SOCmnHTTPHandler::decodeHTTPRequestHeader(
	DWORD /* dataSize */,
	BYTE* pData,
	BYTE** pBodyS)
{
	CHAR* pCur = (CHAR*)pData;
	CHAR* pToken;
	DWORD lenToken;
	DWORD res;
	BOOL ok;
	*pBodyS = NULL;
	m_httpMethod = 0;
	m_httpURL.empty();
	m_httpVersion = 0;
	m_attributeList.removeAll();
	// 1) find request line
	ok = FALSE;

	do
	{
		res = skipSpaces(&pCur);

		if (res == SOCMNHTTP_OK)
		{
			res = getToken(&lenToken, &pToken, &pCur);

			if (res == SOCMNHTTP_OK)
			{
				// a) line has to start with supported command
				res = isHTTPMethod(lenToken, pToken, &m_httpMethod);

				if (res == SOCMNHTTP_OK)
				{
					res = skipSpaces(&pCur);

					if (res == SOCMNHTTP_OK)
					{
						res = getToken(&lenToken, &pToken, &pCur);

						if (res == SOCMNHTTP_OK)
						{
							// b) 2nd token is the URL
							m_httpURL.setCHAR(pToken, lenToken);
							res = skipSpaces(&pCur);

							if (res == SOCMNHTTP_OK)
							{
								res = getToken(&lenToken, &pToken, &pCur);

								if (res == SOCMNHTTP_OK)
								{
									// c) 3nd token is the HTTP version
									res = isHTTPVersion(lenToken, pToken, &m_httpVersion);

									if (res == SOCMNHTTP_OK)
									{
										ok = (skipSpaces(&pCur) == SOCMNHTTP_EOL);
									}
								}
							}
						}
					}
				}
			}
		}

		if (!ok)
		{
			if (res != SOCMNHTTP_EOR)
			{
				res = nextLine(&pCur);
			}

			if (res == SOCMNHTTP_EOR)
			{
				return (DWORD)((pCur - (CHAR*)pData) + 1);
			}
		}
	}
	while (!ok);

	// get the attributes
	SOCmnString attrName;
	SOCmnString attrValue;

	while ((res = nextLine(&pCur)) == SOCMNHTTP_OK)
	{
		res = skipSpaces(&pCur);

		if (res == SOCMNHTTP_OK)
		{
			res = getToken(&lenToken, &pToken, &pCur);

			if (res == SOCMNHTTP_OK)
			{
				// attribute name
				if ((*(pToken + (lenToken - 1))) == ':')
				{
					// attribute name has to end with a :
					attrName.setCHAR(pToken, lenToken - 1);
					attrName.makeLower();
					res = skipSpaces(&pCur);

					if (res == SOCMNHTTP_OK)
					{
						res = getLine(&lenToken, &pToken, &pCur);

						if (res == SOCMNHTTP_OK)
						{
							// attribute value
							attrValue.setCHAR(pToken, lenToken);
							m_attributeList.add(attrValue, attrName);
						}
					}
				}
			}
		}
		else
		{
			if (res == SOCMNHTTP_EOL)
			{
				// end of header
				res = nextLine(&pCur);

				if (res == SOCMNHTTP_OK)
				{
					*pBodyS = (BYTE*)pCur;
				}
			}

			break;
		}
	}

	if (res == SOCMNHTTP_EOR)
	{
		return (DWORD)((pCur - (CHAR*)pData) + 1);
	}

	return (DWORD)(pCur - (CHAR*)pData);
}

DWORD SOCmnHTTPHandler::decodeHTTPResponseHeader(
	DWORD /* dataSize */,
	BYTE* pData,
	BYTE** pBodyS)
{
	CHAR* pCur = (CHAR*)pData;
	CHAR* pToken;
	DWORD lenToken;
	DWORD res;
	BOOL ok;
	SOCmnString statusSZ;
	*pBodyS = NULL;
	m_httpVersion = 0;
	m_httpStatusCode = 0;
	m_attributeList.removeAll();
	// 1) find response line
	ok = FALSE;

	do
	{
		res = skipSpaces(&pCur);

		if (res == SOCMNHTTP_OK)
		{
			res = getToken(&lenToken, &pToken, &pCur);

			if (res == SOCMNHTTP_OK)
			{
				// a) line has to start with the HTTP version
				res = isHTTPVersion(lenToken, pToken, &m_httpVersion);

				if (res == SOCMNHTTP_OK)
				{
					res = skipSpaces(&pCur);

					if (res == SOCMNHTTP_OK)
					{
						res = getToken(&lenToken, &pToken, &pCur);

						if (res == SOCMNHTTP_OK)
						{
							// b) 2nd token is the status code
							statusSZ.setCHAR(pToken, lenToken);
							m_httpStatusCode = (WORD)_tcstoul(statusSZ, NULL, 10);
							res = getLine(&lenToken, &pToken, &pCur);
							ok = (res == SOCMNHTTP_OK);
						}
					}
				}
			}
		}

		if (!ok)
		{
			if (res != SOCMNHTTP_EOR)
			{
				res = nextLine(&pCur);
			}

			if (res == SOCMNHTTP_EOR)
			{
				return (DWORD)((pCur - (CHAR*)pData) + 1);
			}
		}
	}
	while (!ok);

	// get the attributes
	SOCmnString attrName;
	SOCmnString attrValue;

	while ((res = nextLine(&pCur)) == SOCMNHTTP_OK)
	{
		res = skipSpaces(&pCur);

		if (res == SOCMNHTTP_OK)
		{
			res = getToken(&lenToken, &pToken, &pCur);

			if (res == SOCMNHTTP_OK)
			{
				// attribute name
				if ((*(pToken + (lenToken - 1))) == ':')
				{
					// attribute name has to end with a :
					attrName.setCHAR(pToken, lenToken - 1);
					attrName.makeLower();
					res = skipSpaces(&pCur);

					if (res == SOCMNHTTP_OK)
					{
						res = getLine(&lenToken, &pToken, &pCur);

						if (res == SOCMNHTTP_OK)
						{
							// attribute value
							attrValue.setCHAR(pToken, lenToken);
							m_attributeList.add(attrValue, attrName);
						}
					}
				}
			}
		}
		else
		{
			if (res == SOCMNHTTP_EOL)
			{
				// end of header
				res = nextLine(&pCur);

				if (res == SOCMNHTTP_OK)
				{
					*pBodyS = (BYTE*)pCur;
				}
			}

			break;
		}
	}

	if (res == SOCMNHTTP_EOR)
	{
		return (DWORD)((pCur - (CHAR*)pData) + 1);
	}

	return (DWORD)(pCur - (CHAR*)pData);
}

DWORD SOCmnHTTPHandler::skipSpaces(CHAR** pCur)
{
	while (TRUE)
	{
		switch (**pCur)
		{
		case ' ':
		case '\t':
			(*pCur)++;
			break;

		case '\0':
			return SOCMNHTTP_EOR;
			break;

		case '\n':
		case '\r':
			return SOCMNHTTP_EOL;

		default:
			return SOCMNHTTP_OK;
			break;
		}
	}
}

DWORD SOCmnHTTPHandler::getToken(DWORD* pLen, CHAR** pToken, CHAR** pCur)
{
	CHAR* pStart = *pCur;

	while (TRUE)
	{
		switch (**pCur)
		{
		case ' ':
		case '\t':
		case '\0':
		case '\n':
		case '\r':
			if (pStart == (*pCur))
			{
				switch (**pCur)
				{
				case ' ':
				case '\t':
					return SOCMNHTTP_WS;

				case '\0':
					return SOCMNHTTP_EOR;

				case '\n':
				case '\r':
					return SOCMNHTTP_EOL;
				}
			}

			(*pLen) = (DWORD)((*pCur) - pStart);
			(*pToken) = pStart;
			return SOCMNHTTP_OK;
			break;

		default:
			(*pCur)++;
			break;
		}
	}
}

DWORD SOCmnHTTPHandler::getLine(DWORD* pLen, CHAR** pToken, CHAR** pCur)
{
	CHAR* pStart = *pCur;

	while (TRUE)
	{
		switch (**pCur)
		{
		case '\0':
		case '\n':
		case '\r':
			if (pStart == (*pCur))
			{
				switch (**pCur)
				{
				case '\0':
					return SOCMNHTTP_EOR;

				case '\n':
				case '\r':
					return SOCMNHTTP_EOL;
				}
			}

			(*pLen) = (DWORD)((*pCur) - pStart);
			(*pToken) = pStart;
			return SOCMNHTTP_OK;
			break;

		default:
			(*pCur)++;
			break;
		}
	}
}


DWORD SOCmnHTTPHandler::nextLine(CHAR** pCur)
{
	while (TRUE)
	{
		switch (**pCur)
		{
		case '\0':
			return SOCMNHTTP_EOR;
			break;

		case '\n':
			(*pCur)++;
			return SOCMNHTTP_OK;
			break;

		default:
			(*pCur)++;
			break;
		}
	}
}

DWORD SOCmnHTTPHandler::endOfRequest(BYTE** pData)
{
	while (TRUE)
	{
		switch (**pData)
		{
		case 0:
			return SOCMNHTTP_OK;
			break;

		default:
			(*pData)++;
			break;
		}
	}
}


DWORD SOCmnHTTPHandler::isHTTPMethod(DWORD lenToken, CHAR* pToken, WORD* pHTTPMethod)
{
	if (lenToken > 7)
	{
		return SOCMNHTTP_FAIL;
	}

	switch (lenToken)
	{
	case 4:
		if (strncmp(pToken, "POST", 4) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_POST;
			return SOCMNHTTP_OK;
		}

		if (strncmp(pToken, "HEAD", 4) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_HEAD;
			return SOCMNHTTP_OK;
		}

		break;

	case 3:
		if (strncmp(pToken, "GET", 3) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_GET;
			return SOCMNHTTP_OK;
		}

		if (strncmp(pToken, "PUT", 3) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_PUT;
			return SOCMNHTTP_OK;
		}

		break;

	case 5:
		if (strncmp(pToken, "TRACE", 5) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_TRACE;
			return SOCMNHTTP_OK;
		}

		break;

	case 6:
		if (strncmp(pToken, "DELETE", 6) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_DELETE;
			return SOCMNHTTP_OK;
		}

		break;

	case 7:
		if (strncmp(pToken, "OPTIONS", 7) == 0)
		{
			*pHTTPMethod = SOCMNHTTP_METHOD_OPTIONS;
			return SOCMNHTTP_OK;
		}

		break;
	}

	return SOCMNHTTP_FAIL;
}

DWORD SOCmnHTTPHandler::isHTTPVersion(DWORD lenToken, CHAR* pToken, WORD* pHTTPVersion)
{
	if (lenToken != 8)
	{
		return SOCMNHTTP_FAIL;
	}

	if (strncmp(pToken, "HTTP/", 5) == 0)
	{
		if ((isdigit(pToken[5]) != 0) && (isdigit(pToken[7]) != 0))
		{
			*pHTTPVersion = MAKEWORD(pToken[7] - '0', pToken[5] - '0');
			return SOCMNHTTP_OK;
		}
	}

	return SOCMNHTTP_FAIL;
}



DWORD SOCmnHTTPHandler::getContentsLength(void)
{
	SOCmnString szCL;
	DWORD cl = 0;

	if (m_attributeList.findKey(_T("content-length"), &szCL))
	{
		cl = _tcstoul(szCL, NULL, 10);
	}

	return cl;
}

LPCTSTR SOCmnHTTPHandler::getHTTPMethodString(IN WORD method)
{
	LPCTSTR ret = _T("?");

	switch (method)
	{
	case SOCMNHTTP_METHOD_GET:
		ret = _T("GET");
		break;

	case SOCMNHTTP_METHOD_HEAD:
		ret = _T("HEAD");
		break;

	case SOCMNHTTP_METHOD_POST:
		ret = _T("POST");
		break;

	case SOCMNHTTP_METHOD_PUT:
		ret = _T("PUT");
		break;

	case SOCMNHTTP_METHOD_TRACE:
		ret = _T("TRACE");
		break;

	case SOCMNHTTP_METHOD_OPTIONS:
		ret = _T("OPTIONS");
		break;

	case SOCMNHTTP_METHOD_DELETE:
		ret = _T("DELETE");
		break;
	}

	return ret;
}


#endif // SOFEATURE_CMN_HTTP
