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
//                           OPC Toolkit - SOCmn                              |
//                                                                            |
//  Filename    : SOCmnXML.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : XML parser classes                                          |
//                - SOCmnXML: eXpat parser class                              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#ifdef SOOS_LINUX
#include <sys/stat.h>
#endif
#include <SOCmn.h>
#include <SOCmnXML.h>

const TCHAR g_szArrayOfByte[]           = _T("ArrayOfByte");
const TCHAR g_szArrayOfShort[]          = _T("ArrayOfShort");
const TCHAR g_szArrayOfUnsignedShort[]  = _T("ArrayOfUnsignedShort");
const TCHAR g_szArrayOfInt[]            = _T("ArrayOfInt");
const TCHAR g_szArrayOfUnsignedInt[]    = _T("ArrayOfUnsignedInt");
const TCHAR g_szArrayOfLong[]           = _T("ArrayOfLong");
const TCHAR g_szArrayOfUnsignedLong[]   = _T("ArrayOfUnsignedLong");
const TCHAR g_szArrayOfFloat[]          = _T("ArrayOfFloat");
const TCHAR g_szArrayOfDecimal[]        = _T("ArrayOfDecimal");
const TCHAR g_szArrayOfDouble[]         = _T("ArrayOfDouble");
const TCHAR g_szArrayOfBoolean[]        = _T("ArrayOfBoolean");
const TCHAR g_szArrayOfString[]         = _T("ArrayOfString");
const TCHAR g_szArrayOfDateTime[]       = _T("ArrayOfDateTime");
const TCHAR g_szArrayOfAnyType[]        = _T("ArrayOfAnyType");

#ifdef SOFEATURE_CMN_XML

//-----------------------------------------------------------------------------
// SOCmnXML                                                                   |
//-----------------------------------------------------------------------------

SOCmnXML::SOCmnXML()
{
	m_parser = NULL;
}

SOCmnXML::~SOCmnXML()
{
	destroy();
}

BOOL SOCmnXML::create(LPCSTR encod, CHAR namespaceSeparat)
{
	destroy();
	m_parser = XML_ParserCreate_MM(encod, NULL, &namespaceSeparat);

	if (!m_parser)
	{
		return FALSE;
	}

	XML_SetUserData(m_parser, (void*)this);
	return TRUE;
}


void SOCmnXML::destroy(void)
{
	if (m_parser != NULL)
	{
		XML_ParserFree(m_parser);
	}

	m_parser = NULL;
}

BOOL SOCmnXML::parse(LPCSTR buff, LONG len, BOOL isFinal)
{
	return (XML_Parse(m_parser, buff, len, isFinal) == XML_STATUS_OK);
}

BOOL SOCmnXML::parseBuffer(LONG len, BOOL isFinal)
{
	return (XML_ParseBuffer(m_parser, len, isFinal) == XML_STATUS_OK);
}

LPSTR SOCmnXML::getBuffer(LONG len)
{
	return (LPSTR)XML_GetBuffer(m_parser, len);
}

void SOCmnXML::setHandler(DWORD whichHandler)
{
	if (whichHandler & SCMNXML_HANDLER_ELEMENTDECL)
	{
		XML_SetElementDeclHandler(m_parser, elementDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_ATTLISTDECL)
	{
		XML_SetAttlistDeclHandler(m_parser, attlistDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_XMLDECL)
	{
		XML_SetXmlDeclHandler(m_parser, xmlDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_PROCESSINGINSTRUCTION)
	{
		XML_SetProcessingInstructionHandler(m_parser, processingInstructionH);
	}

	if (whichHandler & SCMNXML_HANDLER_STARTELEMENT)
	{
		XML_SetStartElementHandler(m_parser, startElementH);
	}

	if (whichHandler & SCMNXML_HANDLER_ENDELEMENT)
	{
		XML_SetEndElementHandler(m_parser, endElementH);
	}

	if (whichHandler & SCMNXML_HANDLER_CHARACTERDATA)
	{
		XML_SetCharacterDataHandler(m_parser, characterDataH);
	}

	if (whichHandler & SCMNXML_HANDLER_COMMENT)
	{
		XML_SetCommentHandler(m_parser, commentH);
	}

	if (whichHandler & SCMNXML_HANDLER_STARTCDATASECTION)
	{
		XML_SetStartCdataSectionHandler(m_parser, startCdataSectionH);
	}

	if (whichHandler & SCMNXML_HANDLER_ENDCDATASECTION)
	{
		XML_SetEndCdataSectionHandler(m_parser, endCdataSectionH);
	}

	if (whichHandler & SCMNXML_HANDLER_DEFAULT)
	{
		XML_SetDefaultHandler(m_parser, defaultH);
	}

	if (whichHandler & SCMNXML_HANDLER_DEFAULTEXPAND)
	{
		XML_SetDefaultHandlerExpand(m_parser, defaultH);
	}

	if (whichHandler & SCMNXML_HANDLER_STARTDOCTYPEDECL)
	{
		XML_SetStartDoctypeDeclHandler(m_parser, startDoctypeDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_ENDDOCTYPEDECL)
	{
		XML_SetEndDoctypeDeclHandler(m_parser, endDoctypeDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_UNPARSEDENTITYDECL)
	{
		XML_SetUnparsedEntityDeclHandler(m_parser, unparsedEntityDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_NOTATIONDECL)
	{
		XML_SetNotationDeclHandler(m_parser, notationDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_STARTNAMESPACEDECL)
	{
		XML_SetStartNamespaceDeclHandler(m_parser, startNamespaceDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_ENDNAMESPACEDECL)
	{
		XML_SetEndNamespaceDeclHandler(m_parser, endNamespaceDeclH);
	}

	if (whichHandler & SCMNXML_HANDLER_NOTSTANDALONE)
	{
		XML_SetNotStandaloneHandler(m_parser, notStandaloneH);
	}

	if (whichHandler & SCMNXML_HANDLER_SKIPPEDENTITY)
	{
		XML_SetSkippedEntityHandler(m_parser, skippedEntityH);
	}

	if (whichHandler & SCMNXML_HANDLER_SETENTITYDECL)
	{
		XML_SetEntityDeclHandler(m_parser, entityDeclH);
	}
}



SOCmnXmlValue::SOCmnXmlValue(void)
{
	m_simpleType = 0x12;
}

SOCmnXmlValue::~SOCmnXmlValue(void)
{
	m_array.destroy();
}

#ifdef SOFEATURE_CMN_VARIANT
VARTYPE SOCmnXmlValue::XmlArrayElementType2Vartype()
{
	if (m_simpleType == TRUE)
	{
		return VT_EMPTY;    // since I'm a simple type I return VT_EMPTY
	}

	int idx = -1;
	SOCmnString szTypeOnly = m_datatype.mid((((idx = m_datatype.reverseFind(_T('|'))) == -1) ? idx : idx + 1));

	if (szTypeOnly == (LPCTSTR)g_szArrayOfByte)
	{
		return VT_I1;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfShort)
	{
		return VT_I2;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfUnsignedShort)
	{
		return VT_UI2;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfInt)
	{
		return VT_I4;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfUnsignedInt)
	{
		return VT_UI4;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfLong)
	{
		return VT_I8;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfUnsignedLong)
	{
		return VT_UI8;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfFloat)
	{
		return VT_R4;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfDecimal)
	{
		return VT_CY;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfDouble)
	{
		return VT_R8;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfBoolean)
	{
		return VT_BOOL;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfString)
	{
		return VT_BSTR;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfDateTime)
	{
		return VT_DATE;
	}
	else if (szTypeOnly == (LPCTSTR)g_szArrayOfAnyType)
	{
		return VT_VARIANT;
	}
	else
	{
		return VT_EMPTY;    // I should never return this...
	}
}


//-----------------------------------------------------------------------------
// static decodeBase64String2String
//
// - decodes a base64 string to a Variant byte array (VT_ARRAY | VT_UI1)
//
// returns:
//      0  - if no array was decoded
//      >0 - number of array elements
//
DWORD SOCmnXmlValue::decodeBase64String2String(SOCmnString& szBase64, SOCmnString& string)
{
	BYTE bError = FALSE;
	DWORD dwBase64Length = szBase64.getLength();
	DWORD dwElements = ((dwBase64Length  * 6) / 8); // calculate length of byte array
	BYTE* pBytes = new BYTE[dwElements + 1];
	memset(pBytes, 0, dwElements + 1);
	LPTSTR pszChar = szBase64.getBuffer((DWORD) - 1);

	for (DWORD count = 0; count < dwBase64Length && pszChar[count] != _T('='); count++)
	{
		SHORT   sixBitVal = 0;
		BYTE    bitshift = (BYTE)(10 - (count * 6) % 8);
		DWORD   offset = (count * 6) / 8;

		if (pszChar[count] >= _T('A') && pszChar[count] <= _T('Z'))
		{
			sixBitVal = (pszChar[count] - _T('A'));
		}
		else if (pszChar[count] >= _T('a') && pszChar[count] <= _T('z'))
		{
			sixBitVal = ((pszChar[count] - _T('a')) + 26);
		}
		else if (pszChar[count] >= _T('0') && pszChar[count] <= _T('9'))
		{
			sixBitVal = ((pszChar[count] - _T('0')) + 52);
		}
		else if (pszChar[count] == _T('+'))
		{
			sixBitVal = 62;
		}
		else if (pszChar[count] == _T('/'))
		{
			sixBitVal = 63;
		}
		else
		{
			bError = TRUE;
			break;
		}

		sixBitVal <<= bitshift;

		if (bitshift != 10)
		{
			_ASSERTION(offset + 1 <= dwElements, "array overrun");
			pBytes[offset] |= HIBYTE(sixBitVal);
			pBytes[offset + 1] |= LOBYTE(sixBitVal);
		}
		else
		{
			_ASSERTION(offset <= dwElements, "array overrun");
			pBytes[offset] |= HIBYTE(sixBitVal);
		}
	}

	string.setCHAR((LPCSTR)pBytes, dwElements);
	delete [] pBytes;
	return dwElements;
}// decodeBase64String2Variant


//-----------------------------------------------------------------------------
// static decodeBase64String2Variant
//
// - decodes a base64 string to a Variant byte array (VT_ARRAY | VT_UI1)
//
// returns:
//      0  - if no array was decoded
//      >0 - number of array elements
//
DWORD SOCmnXmlValue::decodeBase64String2Variant(SOCmnString& szBase64, SOCmnVariant& ui1Array)
{
	BYTE bError = FALSE;
	DWORD dwBase64Length = szBase64.getLength();
	DWORD dwElements = ((dwBase64Length  * 6) / 8); // calculate length of byte array
	BYTE* pBytes = new BYTE[dwElements + 1];
	memset(pBytes, 0, dwElements + 1);
	LPTSTR pszChar = szBase64.getBuffer((DWORD) - 1);
	DWORD count;

	for (count = 0; count < dwBase64Length && pszChar[count] != _T('='); count++)
	{
		SHORT   sixBitVal = 0;
		BYTE    bitshift = (BYTE)(10 - (count * 6) % 8);
		DWORD   offset = (count * 6) / 8;

		if (pszChar[count] >= _T('A') && pszChar[count] <= _T('Z'))
		{
			sixBitVal = (pszChar[count] - _T('A'));
		}
		else if (pszChar[count] >= _T('a') && pszChar[count] <= _T('z'))
		{
			sixBitVal = ((pszChar[count] - _T('a')) + 26);
		}
		else if (pszChar[count] >= _T('0') && pszChar[count] <= _T('9'))
		{
			sixBitVal = ((pszChar[count] - _T('0')) + 52);
		}
		else if (pszChar[count] == _T('+'))
		{
			sixBitVal = 62;
		}
		else if (pszChar[count] == _T('/'))
		{
			sixBitVal = 63;
		}
		else
		{
			bError = TRUE;
			break;
		}

		sixBitVal <<= bitshift;

		if (bitshift != 10)
		{
			_ASSERTION(offset + 1 <= dwElements, "array overrun");
			pBytes[offset] |= HIBYTE(sixBitVal);
			pBytes[offset + 1] |= LOBYTE(sixBitVal);
		}
		else
		{
			_ASSERTION(offset <= dwElements, "array overrun");
			pBytes[offset] |= HIBYTE(sixBitVal);
		}
	}

	// correct dwElements with padding signs =
	if (count < dwBase64Length)
	{
		dwElements -= (dwBase64Length - count);
	}

	ui1Array.clear();

	if (!bError)
	{
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = dwElements;
		V_VT(&ui1Array) = (VT_ARRAY | VT_UI1);
		V_ARRAY(&ui1Array) = SOCmnVariant::safeArrayCreate((VARTYPE)(V_VT(&ui1Array) & ~VT_ARRAY), 1, &dim);
		BYTE* pArray;

		if (SUCCEEDED(SafeArrayAccessData(V_ARRAY(&ui1Array), (void**)&pArray)))
		{
			memcpy(pArray, pBytes, dwElements);
			SafeArrayUnaccessData(V_ARRAY(&ui1Array));
		}
		else
		{
			bError = TRUE;
		}
	}

	delete pBytes;
	return dwElements;
}// decodeBase64String2Variant


//-----------------------------------------------------------------------------
// static encodeBytes2base64
//
// -
//
// returns:
//      empty  - if nothing to encode or error
//      string - a wellformed base64 string
//
SOCmnString SOCmnXmlValue::encodeBytes2base64(SOCmnVariant& ui1Array)
{
	SOCmnString szBase64;

	if (V_VT(&ui1Array) != (VT_ARRAY | VT_UI1))
	{
		return szBase64;
	}

	BYTE    bError = FALSE;
	BYTE*   pArray;
	LONG    lBound, uBound;

	if (SUCCEEDED(SafeArrayGetLBound(V_ARRAY(&ui1Array), 1, &lBound))
		&&  SUCCEEDED(SafeArrayGetUBound(V_ARRAY(&ui1Array), 1, &uBound))
		&&  SUCCEEDED(SafeArrayAccessData(V_ARRAY(&ui1Array), (void**)&pArray)))
	{
		DWORD dwArrayCount = uBound - lBound + 1;
		DWORD sizeBase64 = (((dwArrayCount * 8) + 4) / 6);
		LPTSTR pszBase64 = NULL;
		DWORD numEquals = dwArrayCount % 3;
		DWORD idx;

		if (numEquals > 0)
		{
			numEquals = 3 - numEquals;
		}

		pszBase64 = (LPTSTR)malloc((sizeBase64 + numEquals + 1) * sizeof(TCHAR));

		for (idx = 0; idx < sizeBase64 + numEquals; idx++)
		{
			pszBase64[idx] = _T('=');
		}

		pszBase64[sizeBase64 + numEquals] = 0;

		for (idx = 0; (idx < sizeBase64); idx++)
		{
			BYTE    bitshift;
			bitshift = (BYTE)(10 - (idx * 6) % 8);
			DWORD offset = (idx * 6) / 8;
			SHORT number = 0;

			if (offset + 1 < dwArrayCount)
			{
				number = MAKEWORD(pArray[offset + 1], pArray[offset]);
			}
			else if (offset < dwArrayCount)
			{
				number = MAKEWORD(0, pArray[offset]);
			}

			BYTE sixBitVal = (BYTE)(number >> bitshift);
			sixBitVal &= 0x3f;

			// add decoding to base64 here...
			if (sixBitVal <= 25)
			{
				pszBase64[idx] = (_T('A') + sixBitVal);
			}
			else if (sixBitVal >= 26 && sixBitVal <= 51)
			{
				pszBase64[idx] = (_T('a') + (sixBitVal - 26));
			}
			else if (sixBitVal >= 52 && sixBitVal <= 61)
			{
				pszBase64[idx] = (_T('0') + (sixBitVal - 52));
			}
			else if (sixBitVal == 62)
			{
				pszBase64[idx]  = _T('+');
			}
			else if (sixBitVal == 63)
			{
				pszBase64[idx] = _T('/');
			}
			else
			{
				bError = TRUE;
				break;
			}
		}

		if (bError)
		{
			szBase64.empty();
		}
		else
		{
			szBase64 = pszBase64;
		}

		free(pszBase64);
		SafeArrayUnaccessData(V_ARRAY(&ui1Array));
	}

	return szBase64;
} //encodeBytes2base64

#endif // SOFEATURE_CMN_VARIANT

//-----------------------------------------------------------------------------
// SOCmnXMLFile                                                               |
//-----------------------------------------------------------------------------

SOCmnXmlFile::SOCmnXmlFile(LPCTSTR fileName)
{
	m_fileName = fileName;
	m_lenData = 0;
	m_pData = NULL;
	create(NULL, _T('|'));
	setHandler(SCMNXML_HANDLER_ELEMENT | SCMNXML_HANDLER_CHARACTERDATA | SCMNXML_HANDLER_NAMESPACEDECL);
	m_elementStack.create();
	m_namespaceList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	setObjectState(SOCMNXMLFILE_STATE_INIT);
}

SOCmnXmlFile::~SOCmnXmlFile(void)
{
	m_elementStack.destroy();
	m_namespaceList.destroy();
	freeContents();
}

BOOL SOCmnXmlFile::readContents(void)
{
#ifdef SOOS_LINUX
	// read contents of file to buffer
	struct stat fileStat;

	if (0 != stat(m_fileName, &fileStat))
	{
		return FALSE;
	}

	// open configuration file
	FILE* pFile;
	pFile = fopen(m_fileName, "r");

	if (!pFile)
	{
		return FALSE;
	}

	m_pData = (BYTE*)malloc(fileStat.st_size);
	m_lenData = fread((void*)m_pData, 1, fileStat.st_size, pFile);
	fclose(pFile);

	if (m_lenData != (DWORD)fileStat.st_size)
	{
		freeContents();
		return FALSE;
	}

	return TRUE;
#endif // SOOS_LINUX
#ifdef SOOS_WINDOWS
	// open configuration file
	HANDLE hFile = CreateFile((LPCTSTR)m_fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// calculate the size of the file
	DWORD fileSize;

	if ((fileSize = ::GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	// get the contents of the file
	m_pData = (BYTE*)malloc(fileSize);

	if (!ReadFile(hFile, m_pData, fileSize, &m_lenData, NULL))
	{
		CloseHandle(hFile);
		freeContents();
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
#endif // SOOS_WINDOWS
}

void SOCmnXmlFile::freeContents(void)
{
	if (m_pData)
	{
		free(m_pData);
	}

	m_pData = NULL;
	m_lenData = 0;
}

void SOCmnXmlFile::handleStartElement(LPCSTR nameA, LPCSTR* /* atts */)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);

	if (getObjectState() != SOCMNXMLFILE_STATE_ERROR)
	{
		m_elementStack.push(name);
	}
}

void SOCmnXmlFile::handleEndElement(LPCSTR nameA)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);

	if (getObjectState() != SOCMNXMLFILE_STATE_ERROR)
	{
		SOCmnString expEndEl = m_elementStack.pop();

		if (_tcsicmp(name, expEndEl) != 0)
		{
			setObjectState(SOCMNXMLFILE_STATE_ERROR);
		}
	}
}

void SOCmnXmlFile::handleStartNamespaceDecl(LPCSTR prefixA, LPCSTR uriA)
{
	USES_CONVERSION;
	LPCTSTR uri = A2CT(uriA);
	LPCTSTR prefix = A2CT(prefixA);

	if (prefix)
	{
		m_namespaceList.add(uri, prefix);
	}
	else
	{
		m_defNamespace = uri;
	}
}

LPCTSTR SOCmnXmlFile::expandNamespace(IN OUT SOCmnString& szXML)
{
	int nsIdx;
	nsIdx = szXML.find(_T(':'));

	if (nsIdx == -1)
	{
		szXML.format(_T("%s|%s"), (LPCTSTR)m_defNamespace, (LPCTSTR)szXML);
	}
	else
	{
		SOCmnString prefix = szXML.left(nsIdx);
		SOCmnString data = szXML.right(szXML.getLength() - nsIdx - 1);
		szXML.format(_T("%s|%s"), (LPCTSTR)m_namespaceList.findKey(prefix), (LPCTSTR)data);
	}

	return (LPCTSTR)szXML;
}

void SOCmnXmlFile::removeNamespace(OUT IN SOCmnString& szXML)
{
	int idx;
	idx = szXML.find(_T('|'));

	if (idx != -1)
	{
		szXML = szXML.right(szXML.getLength() - (idx + 1));
	}
}

BOOL SOCmnXmlFile::removeNamespace(OUT IN SOCmnString& szXML, IN LPCTSTR nspace)
{
	if (_tcsncmp(szXML, nspace, _tcslen(nspace)) == 0)
	{
		szXML = szXML.right(szXML.getLength() - (DWORD)(_tcslen(nspace) - 1));
		return TRUE;
	}

	return FALSE;
}

BOOL SOCmnXmlFile::parse(void)
{
	return SOCmnXML::parse((CHAR*)m_pData, m_lenData);
}

#endif // SOFEATURE_CMN_XML

#ifdef SOFEATURE_XML_METHOD

//-----------------------------------------------------------------------------
// SOCmnXmlMethod                                                             |
//-----------------------------------------------------------------------------

SOCmnXmlMethod::SOCmnXmlMethod(void)
{
	m_xmlMethod = SOCMNXMLMETHOD_NO;
	m_rcvTime.now();
	m_lenData = 0;
	m_pData = NULL;
	m_flags = 0;
	setObjectState(SOCMNXMLMETHOD_STATE_INIT);
	create(NULL, _T('|'));
	setHandler(SCMNXML_HANDLER_ELEMENT | SCMNXML_HANDLER_CHARACTERDATA | SCMNXML_HANDLER_NAMESPACEDECL);
	m_elementStack.create();
	m_namespaceList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
}

SOCmnXmlMethod::~SOCmnXmlMethod(void)
{
	m_elementStack.destroy();
	m_namespaceList.destroy();
	SOCMN_FREE_MEMORY(m_pData)
}

void SOCmnXmlMethod::handleStartElement(LPCSTR nameA, LPCSTR* /* atts */)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);

	switch (getObjectState())
	{
	case SOCMNXMLMETHOD_STATE_INIT:  // wait for SOAP envelope
		if (_tcsicmp(name, _T("http://schemas.xmlsoap.org/soap/envelope/|Envelope")) == 0)
		{
			setObjectState(SOCMNXMLMETHOD_STATE_SOAP_ENVELOPE);
			m_elementStack.push(name);
		}
		else
		{
			setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("No SOAP envelope: instead %s"), name));
		}

		break;

	case SOCMNXMLMETHOD_STATE_SOAP_ENVELOPE:  // wait for SOAP body
		if (_tcsicmp(name, _T("http://schemas.xmlsoap.org/soap/envelope/|Body")) == 0)
		{
			setObjectState(SOCMNXMLMETHOD_STATE_SOAP_BODY);
			m_elementStack.push(name);
		}
		else if (_tcsicmp(name, _T("http://schemas.xmlsoap.org/soap/envelope/|Header")) == 0)
		{
			//  do nothing just skip the header
			m_elementStack.push(name);
		}
		else
		{
			setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("No SOAP body: instead %s"), name));
		}

		break;

	case SOCMNXMLMETHOD_STATE_END:
		setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("End of method: instead %s"), name));
		break;

	default:
		break;
	}
}

void SOCmnXmlMethod::handleEndElement(LPCSTR nameA)
{
	USES_CONVERSION;
	LPCTSTR name = A2CT(nameA);

	if (getObjectState() != SOCMNXMLMETHOD_STATE_ERROR)
	{
		SOCmnString expEndEl = m_elementStack.pop();

		if (_tcsicmp(name, expEndEl) != 0)
		{
			setObjectState(SOCMNXMLMETHOD_STATE_ERROR);
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL, (_T("Wrong end element: %s expected %s"), name, (LPCTSTR)expEndEl));
		}
	}
}

void SOCmnXmlMethod::handleStartNamespaceDecl(LPCSTR prefixA, LPCSTR uriA)
{
	USES_CONVERSION;
	LPCTSTR uri = A2CT(uriA);
	LPCTSTR prefix = A2CT(prefixA);

	if (prefix)
	{
		m_namespaceList.add(uri, prefix);
	}
	else
	{
		m_defNamespace = uri;
	}
}

LPCTSTR SOCmnXmlMethod::expandNamespace(IN OUT SOCmnString& szXML)
{
	int nsIdx;
	nsIdx = szXML.find(_T(':'));

	if (nsIdx == -1)
	{
		szXML.format(_T("%s|%s"), (LPCTSTR)m_defNamespace, (LPCTSTR)szXML);
	}
	else
	{
		SOCmnString prefix = szXML.left(nsIdx);
		SOCmnString data = szXML.right(szXML.getLength() - nsIdx - 1);
		szXML.format(_T("%s|%s"), (LPCTSTR)m_namespaceList.findKey(prefix), (LPCTSTR)data);
	}

	return (LPCTSTR)szXML;
}

BOOL SOCmnXmlMethod::removeNamespace(OUT IN SOCmnString& szXML, IN LPCTSTR nspace)
{
	if (_tcsncmp(szXML, nspace, _tcslen(nspace)) == 0)
	{
		szXML = szXML.right(szXML.getLength() - (DWORD)(_tcslen(nspace) - 1));
		return TRUE;
	}

	return FALSE;
}

#endif // SOFEATURE_XML_METHOD
